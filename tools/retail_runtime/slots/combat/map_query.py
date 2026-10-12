#!/usr/bin/env python3
"""Combat A/B, retail side: the map queries combat leans on
(docs/gameplay/forensics/MAP_QUERIES.md on feature/combat), one original
call per case on a stand-in map. The case's `call`:

- `line-of-sight` (kata M11): TMapPane::LineOfSight `0x4533d0` (thiscall
  (from*, to*, level, blockedat*, laststep*), ret 0x14) from the case's
  `from` to its `to` on its `level`, over its `ground` (guest.py: the
  sector lookup and the walkmap read answered from it, each cell read
  recorded). With `outputs`, both out-points are passed (pre-filled with
  -7777) and dumped.
- `find-objects` (kata M12): TMapPane::FindObjectsInRange `0x452060`
  (thiscall (pos*, level, array, width, height, objclass, maxnum, objset),
  ret 0x20) with the case's `pos`, `level`, `width`, `height`, `objclass`,
  `maxnum`, `objset`. The rect iterator (guest.py `set_map_objects`) walks
  the case's `objects` (names, in order): its `chars` (the fixture's
  characters: Radius through the GetStat seam) and its `items`, stand-ins
  with a class, position, map index (`id`, at +0x40), inventory slot
  (`inventnum`, +0x7c) and the original IsInInventory (slot 0xb4,
  `0x477a10`); any other slot fails the case. Each spec's `level` (0) is
  the level the iterator finds it on. The array is pre-filled with -1 and
  dumped whole (`maxnum` slots) by name.

The map pane's own level (`+0x98`, `0x666970`) is the case's
`globals.maplevel` (0).

Schema `combat.map.v1`, shared with the port's `Revenant
--retail-ab=map-los` / `map-find-objects` (src/retailab_map.cpp).

Usage:
    map_query.py EXE --serve             # {"id":..,"case":{...}} per line
    map_query.py EXE --case case.json    # one case, dump to stdout
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from guest import CombatWorld, call, s32, serve, start  # noqa: E402

SCHEMA = 'combat.map.v1'
MAP_PANE, MAP_LEVEL = 0x6668d8, 0x666970         # TMapPane; its level (+0x98)
LINE_OF_SIGHT = 0x4533d0
FIND_OBJECTS = 0x452060
IS_IN_INVENTORY, SLOT_IS_IN_INVENTORY = 0x477a10, 0xb4
O_CLASS, O_POS, O_ID, O_INVENTNUM = 0x04, 0x10, 0x40, 0x7c
ITEM_SIZE, VTABLE_SLOTS = 0x100, 0x100
UNWRITTEN = -7777                                 # an out-point's fill: still there, it wasn't written
# The functions under test, for coverage (tools/retail_ab/melee_coverage.py
# --fixture map_query): [start, end).
COVERED = {'LineOfSight': (0x4533d0, 0x453632), 'FindObjectsInRange': (0x452060, 0x452476)}


class MapFixture:
    def __init__(self, executable):
        started = time.perf_counter()
        self.vm, self.sha = start(executable)
        self.world = CombatWorld(self.vm)
        # The stand-in items' vtable: IsInInventory as original, every other
        # slot one host stub that fails the case.
        self.item_vtable = self.vm.allocate(4 * VTABLE_SLOTS)
        unexpected = self.world.boundaries.add_stub('item.unexpected', 0, self._unexpected)
        for slot in range(VTABLE_SLOTS):
            self.vm.put_u32(self.item_vtable + 4 * slot, unexpected)
        self.vm.put_u32(self.item_vtable + SLOT_IS_IN_INVENTORY, IS_IN_INVENTORY)
        self._coverage()
        self.vm.checkpoint()
        self.setup_ms = (time.perf_counter() - started) * 1000
        self.names = {}

    def _coverage(self):
        """With MAP_COVERAGE=<dir>: the code reached in the functions under
        test (each block run, as [start, size]), written to <dir>/<pid>.json
        when the process ends."""
        import atexit
        import os
        out = os.environ.get('MAP_COVERAGE')
        if not out:
            return
        from unicorn import UC_HOOK_BLOCK
        reached = set()
        for lo, hi in COVERED.values():
            self.vm.uc.hook_add(UC_HOOK_BLOCK, lambda uc, address, size, user: reached.add((address, size)),
                                begin=lo, end=hi)

        def write():
            Path(out).mkdir(parents=True, exist_ok=True)
            (Path(out) / f'{os.getpid()}.json').write_text(json.dumps(sorted(reached)))
        atexit.register(write)

    @staticmethod
    def _unexpected(args, ecx):
        raise RuntimeError('a stand-in item was asked a virtual it does not model')

    def _point(self, xyz):
        address = self.vm.allocate(12)
        self.vm.write(address, struct.pack('<3i', *xyz))
        return address

    def _new_item(self, spec):
        vm = self.vm
        obj = vm.allocate(ITEM_SIZE)
        vm.put_u32(obj, self.item_vtable)
        vm.write(obj + O_CLASS, struct.pack('<h', spec['class']))
        vm.write(obj + O_POS, struct.pack('<3i', *spec['pos']))
        vm.put_u32(obj + O_ID, spec['id'] & 0xffffffff)
        vm.write(obj + O_INVENTNUM, struct.pack('<h', spec.get('inventnum', -1)))
        return obj

    def run(self, case):
        vm, world = self.vm, self.world
        vm.restore()
        world.reset()
        world.set_globals(case.get('globals', {}))
        world.set_rng(case)
        world.set_ground(case)
        vm.put_u32(MAP_LEVEL, case.get('globals', {}).get('maplevel', 0) & 0xffffffff)
        specs, self.names = {}, {}
        for spec in case.get('chars', []):
            obj = world.new_character(spec)
            specs[spec['name']] = (obj, spec)
        for spec in case.get('items', []):
            specs[spec['name']] = (self._new_item(spec), spec)
        for name, (obj, spec) in specs.items():
            self.names[spec.get('id', 0)] = name
        world.set_map_objects((name, specs[name][0], specs[name][1].get('level', 0))
                              for name in case.get('objects', []))
        world.seams.clear()

        out = dict(schema=SCHEMA, side='retail')
        kind = case['call']
        if kind == 'line-of-sight':
            outputs = bool(case.get('outputs'))
            blocked = self._point([UNWRITTEN] * 3) if outputs else 0
            step = self._point([UNWRITTEN] * 3) if outputs else 0
            out['returned'] = s32(call(vm, LINE_OF_SIGHT, (
                self._point(case['from']), self._point(case['to']), case.get('level', 0) & 0xffffffff, blocked,
                step), this=MAP_PANE))
            if outputs:
                out['blockedat'] = list(struct.unpack('<3i', vm.uc.mem_read(blocked, 12)))
                out['laststep'] = list(struct.unpack('<3i', vm.uc.mem_read(step, 12)))
        elif kind == 'find-objects':
            most = case['maxnum']
            slots = max(most, 0)
            array = vm.allocate(4 * slots + 4)
            vm.write(array, b'\xff' * (4 * slots + 4))
            out['returned'] = s32(call(vm, FIND_OBJECTS, tuple(v & 0xffffffff for v in (
                self._point(case['pos']), case.get('level', -1), array, case['width'], case.get('height', 0),
                case.get('objclass', -1), most, case.get('objset', 0))), this=MAP_PANE))
            out['array'] = [self._index_name(s32(vm.u32(array + 4 * i))) for i in range(slots + 1)]
        else:
            raise ValueError(f'unknown call {kind!r}')
        out['seams'] = list(world.seams)
        out['draws'] = list(world.draws)
        return out

    def _index_name(self, index):
        if index == -1:
            return None
        return self.names.get(index, f'#{index}')


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable')
    parser.add_argument('--serve', action='store_true')
    parser.add_argument('--case', type=Path)
    args = parser.parse_args()
    fixture = MapFixture(args.executable)
    if args.serve:
        serve(fixture.sha, SCHEMA, fixture.setup_ms, fixture.run)
    elif args.case:
        print(json.dumps(fixture.run(json.loads(args.case.read_text())), indent=1))


if __name__ == '__main__':
    main()
