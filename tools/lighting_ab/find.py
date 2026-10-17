#!/usr/bin/env python3
"""Map search: where named objects stand in the module's base map.

    find.py PATTERN [PATTERN ...] [--level N ...] [--class CLASS]

Each pattern is matched, case ignored, as a substring of an object's
instance name or its type name, over every level with a map (or the given
ones). Prints level, position, class, instance and type name. The sector
decoding is storytest.py's (through revsave.py). Used to pick the lighting
A/B key points at the game's landmarks (docs/lighting/LIGHTING_AB.md §2).
"""
from __future__ import annotations

import argparse
import os
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / 'storytest'))
import storytest  # noqa: E402

DATA = Path(os.environ.get('REVENANT_DATA_PATH', Path.home() / 'RevenantRetailLab' / 'retail-cd' / 'REVENANT'))


def levels_of(module: storytest.ModuleData) -> list[int]:
    return sorted({int(m.group(1)) for n in module.names for m in [re.match(r'map/(\d+)_', n, re.I)] if m})


def search(module, patterns, levels=None, objclass=None):
    wanted = [p.lower() for p in patterns]
    for level in levels or levels_of(module):
        for o in storytest.scan_level(module, level, None):
            label = f'{o.name} {o.typename}'.lower()
            if objclass and o.objclass.lower() != objclass.lower():
                continue
            if any(w in label for w in wanted):
                yield o


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('patterns', nargs='+')
    p.add_argument('--level', type=int, action='append')
    p.add_argument('--class', dest='objclass')
    p.add_argument('--module', default='Ahkuilon')
    a = p.parse_args(argv)
    module = storytest.ModuleData(DATA, a.module)
    for o in search(module, a.patterns, a.level, a.objclass):
        print(f'L{o.level:<3d} ({o.x:6d},{o.y:6d},{o.z:5d})  {o.objclass:12s} {o.name or "-":20s} {o.typename}')


if __name__ == '__main__':
    main()
