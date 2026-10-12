#!/usr/bin/env python3
"""List a level's authored lights and suggest a well-lit camera spot.

    scan_lights.py LEVEL [LEVEL ...] [--module PATH] [--top N]

Reads the module's base map sectors (Map/<level>_<x>_<y>.DAT) with
tools/savefmt/revsave.py and collects every object carrying a light
(OF_LIGHT with a non-zero intensity): its world position (object position
plus the light's offset), colour, intensity (radius) and multiplier.

A spot is scored by the lights whose pools reach into a 640x480 view
centred on it (the retail iso projection: sx = x - y, sy = (x + y)/2 -
0.867 z). The suggestion is the floor tile nearest the best-scoring light,
so the player stands on something. docs/lighting/LIGHTING_AB.md §2.
"""
from __future__ import annotations

import argparse
import collections
import os
import re
import sys
import zipfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[0] / 'savefmt'))
from revsave import Decoder, Truncated  # noqa: E402

DATA = Path(os.environ.get('REVENANT_DATA_PATH', Path.home() / 'RevenantRetailLab' / 'retail-cd' / 'REVENANT'))
SECTOR = re.compile(r'Map/(\d+)_(\d+)_(\d+)\.DAT', re.I)
TILE = 9
VIEW_HALF = (320, 240)


def screen(x, y, z):
    return x - y, (x + y) / 2 - 0.867 * z


def objects(data: bytes):
    """Each top-level object's fields as a dict (class, pos, light)."""
    d = Decoder(data)
    try:
        d.sector()
    except Truncated:
        pass
    objs = collections.defaultdict(dict)
    for f in d.fields:
        m = re.match(r'obj\[(\d+)\]\.(.*)', f.path)
        if m:
            objs[int(m.group(1))][m.group(2)] = f.value
    return objs.values()


def scan(archive: zipfile.ZipFile, level: int):
    lights, tiles = [], []
    for name in archive.namelist():
        m = SECTOR.fullmatch(name)
        if not m or int(m.group(1)) != level:
            continue
        for o in objects(archive.read(name)):
            if 'pos.x' not in o:
                continue
            pos = (o['pos.x'], o['pos.y'], o['pos.z'])
            if o.get('objclass') == TILE:
                tiles.append(pos)
            if o.get('light.intensity'):
                lights.append({
                    'pos': (pos[0] + o['light.pos.x'], pos[1] + o['light.pos.y'], pos[2] + o['light.pos.z']),
                    'color': (o['light.red'], o['light.green'], o['light.blue']),
                    'intensity': o['light.intensity'],
                    'multiplier': o['light.multiplier'],
                })
    return lights, tiles


def best_spots(lights, tiles, top):
    """Lights ranked by how many pools reach a view centred on them."""
    scored = []
    for c in lights:
        cx, cy = screen(*c['pos'])
        n = 0
        for light in lights:
            lx, ly = screen(*light['pos'])
            r = light['intensity']
            if abs(lx - cx) < VIEW_HALF[0] + r and abs(ly - cy) < VIEW_HALF[1] + r:
                n += 1
        scored.append((n, c))
    scored.sort(key=lambda s: -s[0])
    out = []
    for n, c in scored:
        if any(abs(c['pos'][0] - o[1]['pos'][0]) + abs(c['pos'][1] - o[1]['pos'][1]) < 600 for o in out):
            continue                                    # one suggestion per neighbourhood
        x, y, z = c['pos']
        tile = min(tiles, key=lambda t: (t[0] - x) ** 2 + (t[1] - y) ** 2 + 4 * (t[2] - z + 60) ** 2) if tiles else c['pos']
        out.append((n, c, tile))
        if len(out) == top:
            break
    return out


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('levels', nargs='+', type=int)
    p.add_argument('--module', type=Path, default=DATA / 'Modules' / 'Ahkuilon.rvm')
    p.add_argument('--top', type=int, default=3)
    a = p.parse_args(argv)
    archive = zipfile.ZipFile(a.module)
    for level in a.levels:
        lights, tiles = scan(archive, level)
        colours = collections.Counter(l['color'] for l in lights)
        print(f'level {level}: {len(lights)} lights, {len(tiles)} tiles; colours {colours.most_common(4)}')
        for n, light, tile in best_spots(lights, tiles, a.top):
            print(f'  {n:3d} pools in view: light {light["pos"]} {light["color"]} int {light["intensity"]} '
                  f'mult {light["multiplier"]}; stand at {tile}')


if __name__ == '__main__':
    main()
