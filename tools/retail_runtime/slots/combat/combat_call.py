#!/usr/bin/env python3
"""Combat A/B, retail side: one original TCharacter method per case on a
fixture world (docs/gameplay/COMBAT_DOJO.md on feature/combat). The case's
`call` picks it:

- `go` (kata M3): Go(angle) `0x4ce350` (thiscall, ret 4) -- what holding a
  direction calls every tick (UpdateMove `0x47de30`) and AI walking calls.
  In a combat or bow root it decides the orbit: the facing stays on the
  target (CombatFace `0x5d7a64`; monsters always), the step animation comes
  from the angle between moving and facing (GetAngleMoveAnim `0x4d39f0`),
  and whether to pivot first. Case: `angle`.
- `calculate-damage` (kata C1): CalculateDamage `0x4c4860` (thiscall
  (damage, type, modifier), ret 0xc) over the case's `inputs`, with the
  character's own resistance / armour slots (chardata) as original code.
- `resolve-combat` / `resolve-combat-move` (kata M5): ResolveCombat
  `0x4c7980` / ResolveCombatMove `0x4c7f80` (thiscall (ab, bits), ret 8),
  the per-tick resolvers of the combat root and of a combat step, called
  on the character's doing block. Case: `bits`.
- `move` (kata M7): the Move slot (0x114: TCharacter::Move `0x4c46d0`,
  TPlayer::Move `0x518df0`) -- MoveStep `0x4c3bc0`, FindClearPath and
  CharBlocking `0x4d4db0` as original over the case's `ground` and
  `nearby` (guest.py). Adds `motion` to the result.
- `sequence` (kata M8): `ticks` game ticks of retail's order (TimerTick
  0x490bd0): the case's `inputs[t]` (`{"go": angle}` -> Go), then for
  every character TComplexObject::Pulse (UpdateAction with the last
  Move's bits), Move (bits stored at +0xbc), SetObjectMotion, the game
  frame, NextFrame. SetState, ResetState and the imagery's
  SetObjectMotion run as original over the case's motion tables
  (guest.py); a record per tick (`ticks`: self, motion, bits, seams).
- `update-action` (kata M1u): UpdateAction `0x4c3260` (slot 0x210) with
  the case's `bits` (the last Move's): ResolveAction and the resolvers,
  ResetStealthValues, TryCommand / ForceCommand as original; Sleeping /
  SetSleeping through the object-stat seams. Adds `motion`.

Schema `combat.call.v1`, shared with the port's `Revenant
--retail-ab=combat-go` / `combat-resolve`.

Runs as original code: the method, HasActionAni `0x4d6c20`,
GetAngleMoveAnim, IsValidTarget `0x4cd990`, SetFighting `0x4d4790`,
AdvanceAngles `0x4c5ad0`, Distance / AngleTo / AngleDiff / ConvertToVector,
TActionBlock ctor and Is, SetDesired / TryCommand / ForceCommand, the
engine allocator.

Seams (guest.py for the shared ones), recorded in order:
- FindState / FindTransitionState / GetStat (guest.py);
- FindClearPath `0x4c39d0` (thiscall, 5 args): the case's `blocked`
  (default clear), the probe point recorded -- except in a case with a
  `ground`, where it runs as original;
- FindCharacters `0x4cd690` (thiscall, 6 args): an empty world (M3); the
  query recorded. M4 gives it a world;
- TPlayer SetPlayerState `0x51d680` (thiscall, 1 arg): recorded, the value
  stored at +0x36c (assumption: the UI side of it is not modelled);
- CanSeeCharacter `0x4cd540` (thiscall, 2 args): the case's `sees`
  (default yes), recorded.

Usage:
    combat_call.py EXE --serve             # {"id":..,"case":{...}} per line
    combat_call.py EXE --case case.json    # one case, dump to stdout
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from guest import SLOT_MOVE, Boundaries, CombatWorld, call, s32, serve, start  # noqa: E402

SCHEMA = 'combat.call.v1'
GO = 0x4ce350
RESOLVE_COMBAT, RESOLVE_COMBAT_MOVE = 0x4c7980, 0x4c7f80
CALCULATE_DAMAGE = 0x4c4860
FIND_CLEAR_PATH = 0x4c39d0
FIND_CHARACTERS = 0x4cd690
SET_PLAYER_STATE = 0x51d680
SLOT_UPDATE_ACTION = 0x210                       # TCharacter::UpdateAction 0x4c3260 (both classes)
COMPLEX_PULSE = 0x4db190                         # TComplexObject::Pulse (UpdateAction with +0xbc)
SET_OBJECT_MOTION, NEXT_FRAME = 0x470bb0, 0x470cc0
CAN_SEE = 0x4cd540


class CallFixture:
    def __init__(self, executable):
        started = time.perf_counter()
        self.vm, self.sha = start(executable)
        self.world = CombatWorld(self.vm)
        b = self.world.boundaries
        b.add(FIND_CLEAR_PATH, 'FindClearPath', 0x14, self._find_clear_path)
        b.add(FIND_CHARACTERS, 'FindCharacters', 0x18, self._find_characters)
        b.add(SET_PLAYER_STATE, 'SetPlayerState', 4, self._set_player_state)
        b.add(CAN_SEE, 'CanSeeCharacter', 8, self._can_see)
        self.vm.checkpoint()
        self.setup_ms = (time.perf_counter() - started) * 1000
        self.case = {}

    # -- seams --------------------------------------------------------------
    def _find_clear_path(self, args, ecx):
        if 'ground' in self.case:
            return Boundaries.ORIGINAL
        point = list(struct.unpack('<3i', self.vm.uc.mem_read(args[1], 12)))
        blocked = int(bool(self.case.get('blocked', False)))
        self.world.seams.append(dict(seam='FindClearPath', who=self.world._name(ecx), to=point, result=blocked))
        return blocked

    def _find_characters(self, args, ecx):
        query = dict(max=s32(args[1]), range=s32(args[2]), angle=s32(args[3]),
                     anglerange=s32(args[4]), flags=s32(args[5]))
        self.world.seams.append(dict(seam='FindCharacters', who=self.world._name(ecx), **query, result=0))
        return 0

    def _set_player_state(self, args, ecx):
        self.vm.put_u32(ecx + 0x36c, args[0])
        self.world.seams.append(dict(seam='SetPlayerState', who=self.world._name(ecx), value=args[0]))
        return 0

    def _can_see(self, args, ecx):
        sees = int(bool(self.case.get('sees', True)))
        self.world.seams.append(dict(seam='CanSeeCharacter', who=self.world._name(ecx),
                                     target=self.world._name(args[0]), result=sees))
        return sees

    # -- a case ---------------------------------------------------------------
    def run(self, case):
        vm, world = self.vm, self.world
        vm.restore()
        world.reset()
        self.case = case
        world.set_globals(case.get('globals', {}))
        world.set_rng(case)
        world.set_ground(case)
        for spec in case['chars']:
            world.new_character(spec)
        for spec in case['chars']:
            world.set_blocks(world.by_name[spec['name']], spec)
        world.seams.clear()                       # building the blocks made none, but be sure
        me = world.by_name[case['self']]
        kind = case.get('call', 'go')
        if kind == 'go':
            result = s32(call(vm, GO, (case['angle'] & 0xffffffff,), this=me))
        elif kind in ('resolve-combat', 'resolve-combat-move'):
            entry = RESOLVE_COMBAT if kind == 'resolve-combat' else RESOLVE_COMBAT_MOVE
            doing = vm.u32(me + 0xd8)
            result = s32(call(vm, entry, (doing, case.get('bits', 0) & 0xffffffff), this=me))
        elif kind == 'move':
            result = s32(call(vm, vm.u32(vm.u32(me) + SLOT_MOVE), (), this=me))
        elif kind == 'sequence':
            return self._sequence(case, me)
        elif kind == 'update-action':
            call(vm, vm.u32(vm.u32(me) + SLOT_UPDATE_ACTION), (case.get('bits', 0) & 0xffffffff,), this=me)
            result = 0
        elif kind == 'calculate-damage':
            result = [s32(call(vm, CALCULATE_DAMAGE, tuple(v & 0xffffffff for v in inp), this=me))
                      for inp in case['inputs']]
        else:
            raise ValueError(f'unknown call {kind!r}')
        new_blocks = []
        out = dict(schema=SCHEMA, side='retail', returned=result,
                   self=world.character_dump(me, new_blocks), seams=list(world.seams),
                   draws=list(world.draws))
        if kind in ('move', 'update-action'):
            out['motion'] = world.motion_dump(me)
        return out


    def _sequence(self, case, me):
        vm, world = self.vm, self.world
        world.real_setstate = True
        order = [world.by_name[spec['name']] for spec in case['chars']]
        for obj in order:
            vm.put_u32(obj + 0xbc, case.get('movebits', 0))
        inputs = case.get('inputs', [])
        ticks = []
        for t in range(case['ticks']):
            first = len(world.seams)
            step = inputs[t] if t < len(inputs) else None
            if step and 'go' in step:
                call(vm, GO, (step['go'] & 0xffffffff,), this=me)
            for obj in order:
                call(vm, COMPLEX_PULSE, (), this=obj)
            bits = {}
            for obj in order:
                bits[obj] = call(vm, vm.u32(vm.u32(obj) + SLOT_MOVE), (), this=obj) & 0xffffffff
                vm.put_u32(obj + 0xbc, bits[obj])
            for obj in order:
                call(vm, SET_OBJECT_MOTION, (), this=obj)
            vm.put_u32(0x65caf0 + 0x680, vm.u32(0x65caf0 + 0x680) + 1)
            for obj in order:
                call(vm, NEXT_FRAME, (), this=obj)
            ticks.append(dict(tick=t, bits=bits[me], self=world.character_dump(me, []),
                              motion=world.motion_dump(me), seams=world.seams[first:]))
        return dict(schema=SCHEMA, side='retail', returned=0, ticks=ticks, draws=list(world.draws))


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable')
    parser.add_argument('--serve', action='store_true')
    parser.add_argument('--case', type=Path)
    args = parser.parse_args()
    fixture = CallFixture(args.executable)
    if args.serve:
        serve(fixture.sha, SCHEMA, fixture.setup_ms, fixture.run)
    elif args.case:
        print(json.dumps(fixture.run(json.loads(args.case.read_text())), indent=1))


if __name__ == '__main__':
    main()
