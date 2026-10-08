#!/usr/bin/env python3
"""Gameflow A/B, retail side: the trigger test, `0x004927b0`, in a fixture world.

Runs the original test Continue makes per trigger (SCRIPT_ENGINE.md §3,
`TScript::Triggered(trigger, priority, context)`, thiscall) on one trigger
record per case and dumps what it decided (schema `gameflow.triggertest.v1`,
shared with the port's `Revenant --retail-ab=trigger-test`): whether it
fires, the user it records (`+0xc4`) and its alias (`+0xcc`), the trigger
guard (`+0x10`) after, and the world queries it made.

The fixture world, in guest memory: a list of objects, each a zeroed
0x400-byte record with the fields the test reads -- class `+0x04`, level
`+0x0e`, position `+0x10/+0x14/+0x18`, name `+0x38`, id `+0x40`; one of
them is the script's owner (the context), one may be the player (the
global `0x00667fcc`, else 0).
The script (original constructor `0x00492170`) gets the case's request
state: requested trigger type `+0x18`, its strings `+0x20` / `+0x34`, the
guard `+0x10`, the running-trigger record `+0xa8`.

Recorded boundaries (the world's enumeration, nothing behind them runs):
- the map iterator `0x0044cf80` (init: rect, flags, objset, -, level) and
  `0x0044d080` (next), as the CUBE search `0x00452480` drives them: they
  hand out the world's objects on the requested level, in list order --
  the owner and the player included (the world lists moving objects only,
  the set `0x00452480` asks for).
  The search's own containment test (inclusive, x/y/z) runs as original;
- the object lookup by id `0x00452690` (the guard): the guard object
  exists or not, as the case says.

Usage:
    trigger_test.py EXE --serve        # {"id":..,"case":{...}} per line
"""
from __future__ import annotations

import argparse
import struct
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from fixture import Boundaries, s32, serve, start  # noqa: E402

SCHEMA = 'gameflow.triggertest.v1'

TRIGGER_TEST = 0x4927b0                         # thiscall (trigger, priority, context), ret 0xc
SCRIPT_CTOR = 0x492170                          # TScript(owner, proto)
PLAYER = 0x667fcc                               # the main player
ITER_INIT = 0x44cf80                            # TMapIterator init, thiscall, ret 0x14
ITER_NEXT = 0x44d080                            # TMapIterator next, thiscall
OBJECT_BY_ID = 0x452690                         # TMapPane lookup (id, flag), thiscall, ret 8
ITER_CURRENT = 0x0c                             # the iterator's current object


class TriggerTestFixture:
    def __init__(self, executable):
        self.vm, self.sha = start(executable)
        vm = self.vm
        b = Boundaries(vm)
        b.add(ITER_INIT, 'iterator init', 0x14, self._iter_init)
        b.add(ITER_NEXT, 'iterator next', 0, self._iter_next)
        b.add(OBJECT_BY_ID, 'object by id', 8, self._object_by_id)
        self.player_slot = vm.u32(PLAYER)
        vm.checkpoint()

    # -- the world ------------------------------------------------------------
    def _object(self, spec, oid):
        vm = self.vm
        obj = vm.allocate(0x400)
        name = vm.allocate(len(spec['name']) + 1)
        vm.write(name, spec['name'].encode('cp1252') + b'\0')
        vm.write(obj + 4, struct.pack('<h', spec.get('class', 12)))
        vm.write(obj + 0x0e, struct.pack('<H', spec.get('level', 0)))
        vm.write(obj + 0x10, struct.pack('<3i', *spec['pos']))
        vm.put_u32(obj + 0x38, name)
        vm.put_u32(obj + 0x40, oid)
        self.names[obj] = spec['id']
        return obj

    def _iter_init(self, args, ecx):
        rect, flags, objset, _, level = args[:5]
        self.queries.append(dict(kind='objects', level=s32(level), flags=flags, objset=objset))
        self.iterators[ecx] = [o for o, spec in self.world if spec.get('level', 0) == s32(level)]
        return self._iter_next(args, ecx)

    def _iter_next(self, args, ecx):
        pending = self.iterators.get(ecx, [])
        self.vm.put_u32(ecx + ITER_CURRENT, pending.pop(0) if pending else 0)
        return 0

    def _object_by_id(self, args, ecx):
        oid = s32(args[0])
        self.queries.append(dict(kind='object by id', id=oid))
        return self.guard_object if (oid == self.guard_id and self.guard_object) else 0

    # -- one case ----------------------------------------------------------
    def run(self, case):
        vm = self.vm
        vm.restore()
        self.names, self.queries, self.iterators = {}, [], {}
        world = case['world']
        self.world = [(self._object(spec, 100 + i), spec) for i, spec in enumerate(world['objects'])]
        by_id = {spec['id']: obj for obj, spec in self.world}
        vm.put_u32(PLAYER, by_id[world['player']] if world.get('player') else 0)
        owner = by_id[world['owner']]
        # The guard: an object that set off the running trigger, by id.
        guard = case['request'].get('guard', 'none')
        self.guard_id = 50 if guard != 'none' else -1
        self.guard_object = self._object(dict(id='guard', name='Guard', pos=[0, 0, 0]), 50) \
            if guard == 'exists' else 0

        t = case['trigger']
        trigger = vm.allocate(0x50)
        vm.write(trigger, struct.pack('<iI', t['type'], 0))
        vm.write(trigger + 8, t['name'].encode('cp1252')[:19] + b'\0')
        vm.write(trigger + 0x1c, struct.pack('<6i', *t['cube']))
        vm.write(trigger + 0x34, struct.pack('<ii', t['dist'], t['priority']))

        script = vm.allocate(0xe8)
        vm.call(SCRIPT_CTOR, (owner, 0), this=script)
        r = case['request']
        vm.put_u32(script + 0x18, r['type'])
        vm.write(script + 0x20, r['str'].encode('cp1252')[:19] + b'\0')
        vm.write(script + 0x34, r['str2'].encode('cp1252')[:19] + b'\0')
        vm.put_u32(script + 0x10, self.guard_id)
        if r.get('running'):
            vm.put_u32(script + 0xa8, trigger)

        fires = vm.call(TRIGGER_TEST, (trigger, case['priority'], owner), this=script)
        user, alias = vm.u32(script + 0xc4), vm.u32(script + 0xcc)
        return dict(schema=SCHEMA, side='retail', case=case['name'], fires=fires,
                    user=self.names.get(user, user or None),
                    alias=vm.string(alias) if alias else None,
                    guard_after=s32(vm.u32(script + 0x10)), queries=self.queries)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable', type=Path)
    parser.add_argument('--serve', action='store_true')
    args = parser.parse_args()
    started = time.perf_counter()
    fixture = TriggerTestFixture(args.executable)
    setup_ms = (time.perf_counter() - started) * 1000
    if not args.serve:
        parser.error('--serve')
    serve(fixture.sha, SCHEMA, setup_ms, fixture.run)


if __name__ == '__main__':
    main()
