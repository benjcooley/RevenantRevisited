#!/usr/bin/env python3
"""The lighting A/B locations: the game's landmarks, every AREA covered.

    areas.py [--module PATH] [--write tools/lighting_ab/locations.json]

Every AREA block in the module's area.def sets the light of a place
(AMBLIGHT / AMBCOLOR, and NIGHTAMBLIGHT / NIGHTAMBCOLOR where it has a
night). Each area gets its key points: the landmarks in KEYPOINTS, found on
the map by instance or type name (find.py's map search), with the player
put a little in front of them so the landmark and its light are in view.
An area without a landmark gets the best-lit floor spot on its level
(scan_lights.py). A key point must lie in its area: on its level, inside
its rectangles and outside every later area's (the last matching area
wins, TAreaManager::Pulse). Areas with a night get a noon and a 01:00
location. Areas whose level has no map in the module are listed and
skipped.

The result goes to locations.json, which the other tools read, so a run's
places don't move when the map search or the scan changes.
docs/lighting/LIGHTING_AB.md §2.
"""
from __future__ import annotations

import argparse
import json
import re
import sys
import zipfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / 'storytest'))
import storytest  # noqa: E402
from scan_lights import DATA, SECTOR, best_spots, scan  # noqa: E402

NOON = 12 * 60
NIGHT = 60                      # 01:00, inside the flat night (daylight 0 until 05:30)
HUGE = (-1 << 30, -1 << 30, 1 << 30, 1 << 30)
STAND_OFF = (32, 32)            # the player stands this far from a landmark (toward the viewer)

# Key points per (area, level): (id, what to find, note). What to find is an
# instance name, `type:<typename>` (the first such object inside the area),
# or an explicit [x, y, z] the player stands on.
KEYPOINTS = {
    ('The Forest', 0): [('forest-waves', 'type:Wave', 'surf on the forest coast')],
    ('The Ruins', 5): [('ruins-navarro', 'varro', 'Navarro'),
                       ('ruins-waves', 'type:WaveM', 'surf by the ruins')],
    ('Misthaven', 0): [('misthaven-puppets', 'lucas1', "Lucas's puppet show")],
    ('The Keep', 2): [('keep-pit', [1212, 545, 0], 'the resurrection pit, shot S1'),
                      ('keep-hall', 'TendrickT', 'Tendrick and Sardok upstairs')],
    ('The Keep', 6): [('keep-gow', 'GowE', "Gow's council")],
    ('House Interior', 1): [('misthaven-bar', 'Gus1', "Gus's bar")],
    ('The Ancient Tower', 3): [('tower-harowen', 'HAROWEN1', 'Harowen')],
    ('Arakna Island', 4): [('arakna-gus', 'gus4', 'Gus on the island'),
                           ('arakna-waves', 'type:WaveS', 'surf round the island')],
    ('The Caverns', 30): [('caverns-ollihoot', 'olihoot2', 'Ollihoot')],
    ('The Spiral Arm', 31): [('spiral-solifuge', 'type:Solifuge', 'the Solifuge')],
    ('The Maze of Darkness', 36): [('maze-recluse', 'Recluse', 'the Recluse')],
    ("The Queen's Crypt", 37): [('crypt-andria', 'ANDRIA4', 'Andria')],
    ('The City of The Children', 41): [('children-druhg', 'Blue Druhg', 'the Blue Druhg')],
    ('The Chamber of Horrors', 42): [('horrors-torturer', 'torturer', 'the Ogrok Torturer')],
    ('Interiors of The City of the Children', 43): [('interiors-chef', 'chef1', 'the Ogrok Chef')],
    ('The Chase of Sabu', 45): [('sabu-ogrok-arena', 'slagg', 'the Ogrok arena: Slagg, Mort, Urgg')],
    ('The Slave Camp', 46): [('slavecamp-shegra', 'shegra1', 'Shegra'),
                             ('slavecamp-workers', 'slave1', 'the Druhg workers')],
    ('The Pit', 48): [('pit-jhaga', 'Jhaga1', 'Jhaga and Sabu')],
    ('The Nahkranoth Lair', 49): [('lair-nahkranoth', 'nakra', 'Nahkranoth')],
    ('The Labyrinth', 51): [('lab-red', 'MarkerR', 'red marker')],
    ('The Labyrinth', 52): [('lab-white', 'MarkerW', 'white marker')],
    ('The Labyrinth', 53): [('lab-green', 'MarkerG', 'green marker')],
    ('The Labyrinth', 54): [('lab-blue', 'MarkerB', 'blue marker')],
    ('The Labyrinth', 55): [('lab-orange', 'MarkerO', 'orange marker')],
    ('The Labyrinth', 56): [('lab-yellow', 'MarkerY', 'yellow marker')],
    ('The Labyrinth', 57): [('lab-purple', 'MarkerP', 'purple marker')],
    ('The Labyrinth', 58): [('lab-yhagoro', 'type:Yhagoro', 'Yhagoro')],
}

# Places no AREA covers that the player spends time in.
EXTRA = [
]


def parse_area_def(text: str):
    """The AREA blocks, in file order: name, level, rects, ambient values."""
    areas = []
    for m in re.finditer(r'^AREA\s+"([^"]+)"\s*BEGIN(.*?)\n\s*END', text, re.S | re.I | re.M):
        area = {'name': m.group(1), 'level': -1, 'rects': [], 'amb': None, 'ambcolor': None,
                'nightamb': None, 'nightcolor': None}
        for line in m.group(2).splitlines():
            line = line.split('//')[0].strip()
            key, _, value = line.partition(' ')
            nums = [int(v) for v in re.findall(r'-?\d+', value)]
            if key == 'LEVEL':
                area['level'] = nums[0]
            elif key == 'RECT':
                area['rects'].append(tuple(nums[:4]))
            elif key == 'AMBLIGHT':
                area['amb'] = nums[0]
            elif key == 'AMBCOLOR':
                area['ambcolor'] = nums[:3]
            elif key == 'NIGHTAMBLIGHT':
                area['nightamb'] = nums[0]
            elif key == 'NIGHTAMBCOLOR':
                area['nightcolor'] = nums[:3]
        areas.append(area)
    return areas


def inside(rects, x, y):
    return any(x0 <= x <= x1 and y0 <= y <= y1 for x0, y0, x1, y1 in (rects or [HUGE]))


class Map:
    def __init__(self, module: Path):
        self.archive = zipfile.ZipFile(module)
        self.data = storytest.ModuleData(module.parent.parent, module.stem)
        self.levels = {int(SECTOR.fullmatch(n).group(1)) for n in self.archive.namelist() if SECTOR.fullmatch(n)}
        self._objects, self._lights = {}, {}

    def objects(self, level):
        if level not in self._objects:
            self._objects[level] = storytest.scan_level(self.data, level, None)
        return self._objects[level]

    def lights(self, level):
        if level not in self._lights:
            self._lights[level] = scan(self.archive, level)
        return self._lights[level]

    def find(self, level, what, mine):
        """The landmark's position, or None."""
        if isinstance(what, list):
            return tuple(what), False
        if what.startswith('type:'):
            want = what[5:].lower()
            hits = [o for o in self.objects(level) if o.typename.lower() == want and mine(o.x, o.y)]
        else:
            hits = [o for o in self.objects(level) if o.name.lower() == what.lower() and mine(o.x, o.y)]
        if not hits:
            return None, False
        o = hits[0]
        return (o.x, o.y, o.z), True


def build(module: Path):
    world = Map(module)
    areas = parse_area_def(world.archive.read('area.def').decode('latin-1'))
    locations, skipped = [], []

    def add(ident, area, level, pos, source, minute_variants):
        common = {'area': area['name'] if area else '(none)', 'level': level, 'pos': list(pos),
                  'source': source, 'amb': area and area['amb'], 'ambcolor': area and area['ambcolor']}
        if area and area['nightamb'] is not None and minute_variants:
            locations.append({'id': ident + '-day', 'minute': NOON, **common})
            locations.append({'id': ident + '-night', 'minute': NIGHT, **common,
                              'amb': area['nightamb'], 'ambcolor': area['nightcolor']})
        else:
            locations.append({'id': ident, 'minute': None, **common})

    for i, area in enumerate(areas):
        level = area['level']
        if level not in world.levels:
            skipped.append(f'{area["name"]} (level {level}): no map in the module')
            continue
        later = [a for a in areas[i + 1:] if a['level'] in (level, -1)]

        def mine(x, y, area=area, later=later):
            return inside(area['rects'], x, y) and not any(inside(a['rects'], x, y) for a in later)

        points = KEYPOINTS.get((area['name'], level))
        if points:
            for ident, what, note in points:
                pos, stand_off = world.find(level, what, mine)
                if pos is None:
                    skipped.append(f'{area["name"]} (level {level}): {what} not found')
                    continue
                if stand_off:
                    pos = (pos[0] + STAND_OFF[0], pos[1] + STAND_OFF[1], pos[2])
                add(ident, area, level, pos, note, True)
            continue

        lights, tiles = world.lights(level)
        lights = [l for l in lights if mine(*l['pos'][:2])]
        tiles = [t for t in tiles if mine(*t[:2])]
        if not tiles:
            skipped.append(f'{area["name"]} (level {level}): no tiles inside its rectangles')
            continue
        ident = re.sub(r'[^a-z0-9]+', '-', area['name'].lower().replace('the ', '')).strip('-') + f'-{level}'
        spots = best_spots(lights, tiles, 1) if lights else []
        if spots:
            _, light, tile = spots[0]
            add(ident, area, level, tile, f'best-lit spot, by light {list(light["pos"])} {list(light["color"])}', True)
        else:
            add(ident, area, level, tiles[len(tiles) // 2], 'no lights: a middle tile', True)

    for extra in EXTRA:
        pos, _ = world.find(extra['level'], extra['find'], lambda x, y: True)
        if pos is None:
            skipped.append(f'{extra["id"]}: {extra["find"]} not found')
            continue
        add(extra['id'], None, extra['level'], (pos[0] + STAND_OFF[0], pos[1] + STAND_OFF[1], pos[2]),
            extra['note'], False)
    return locations, skipped


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('--module', type=Path, default=DATA / 'Modules' / 'Ahkuilon.rvm')
    p.add_argument('--write', type=Path)
    a = p.parse_args(argv)
    locations, skipped = build(a.module)
    for loc in locations:
        print(f'{loc["id"]:26s} L{loc["level"]:<3d} A={loc["amb"]!s:4s} {loc["ambcolor"]} at {loc["pos"]}: {loc["source"]}')
    for s in skipped:
        print('skipped:', s)
    if a.write:
        a.write.write_text(json.dumps({'module': a.module.name, 'locations': locations}, indent=1) + '\n')
        print('wrote', a.write)


if __name__ == '__main__':
    main()
