#!/usr/bin/env python3
"""Make one save slot per lighting A/B location (locations.json).

    make_slots.py [--base DIR] [--out DIR] [location ...]

Each slot is a copy of the base slot (default: the postopen New Game slot,
the opening done, control on) with its game.sav patched in place: the
player's position and level, and the game time moved to the location's
minute of the day on the day after the base slot's (interiors keep the
base time). The offsets come from tools/savefmt/revsave.py's decode, so no
other byte moves. The map sectors (CurMap) stay the base slot's; a
location's own sectors are read from the module by both games.
docs/lighting/LIGHTING_AB.md §3.
"""
from __future__ import annotations

import argparse
import json
import shutil
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / 'savefmt'))
from revsave import Decoder  # noqa: E402

BASE = Path.home() / 'RevenantRevisited-testsaves' / 'postopen' / 'New Game'
OUT = Path.home() / 'RevenantRevisited-lightab' / 'slots'
DAYLENGTH = 80000                  # rules.def: hundredths of a second in a game day


def load_locations(ids=()):
    locations = json.loads((HERE / 'locations.json').read_text())['locations']
    if not ids:
        return locations
    known = {loc['id'] for loc in locations}
    unknown = [i for i in ids if i not in known]
    if unknown:
        raise SystemExit(f'unknown location(s): {", ".join(unknown)}')
    return [loc for loc in locations if loc['id'] in ids]


def patch(data: bytes, loc: dict) -> bytes:
    d = Decoder(data)
    d.save()
    fields = {f.path: (f.off, f.size) for f in d.fields}
    out = bytearray(data)

    def put(path, fmt, value):
        off, size = fields[path]
        assert struct.calcsize(fmt) == size, (path, size)
        struct.pack_into(fmt, out, off, value)

    for c, v in zip('xyz', loc['pos']):
        put(f'player[0].pos.{c}', '<i', v)
    put('player[0].level', '<h', loc['level'])
    if loc['minute'] is not None:
        base_time = struct.unpack_from('<i', data, fields['header.gametime'][0])[0]
        day = base_time // DAYLENGTH + 1
        put('header.gametime', '<i', day * DAYLENGTH + loc['minute'] * DAYLENGTH // 1440)
    return bytes(out)


def make(base: Path, out: Path, loc: dict) -> Path:
    slot = out / loc['id']
    if slot.exists():
        shutil.rmtree(slot)
    shutil.copytree(base, slot)
    sav = slot / 'game.sav'
    sav.write_bytes(patch(sav.read_bytes(), loc))
    return slot


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('locations', nargs='*')
    p.add_argument('--base', type=Path, default=BASE)
    p.add_argument('--out', type=Path, default=OUT)
    a = p.parse_args(argv)
    a.out.mkdir(parents=True, exist_ok=True)
    for loc in load_locations(a.locations):
        print(f'{loc["id"]:30s} -> {make(a.base, a.out, loc)}')


if __name__ == '__main__':
    main()
