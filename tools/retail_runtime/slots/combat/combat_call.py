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
- `sidestep` (kata M10): SideStep `0x4d6220` with the case's `dir`
  ('l', 'r', or none: retail's random pick), as original.
- `knockback` (kata M10): KnockBack `0x4d3750` from the case's `from`
  with its `variant`, as original (CombatAnimName, the impact tables).
- `leap` / `start-retreat` (kata M10): Leap `0x4d2be0` (the case's
  `angle`), StartRetreat `0x4d5fc0`, as original.
- `update-move` (kata M6): TPlayScreen::UpdateMove `0x47de30` with the
  case's held controls (`controls`: state, changed; the debug camera off):
  Go / Leap / Stop as original, the map pane's MouseClick (the player's
  Stop) and Block / StopBlock / the bow aim as seams. Adds `controls`.
- `update-action` (kata M1u): UpdateAction `0x4c3260` (slot 0x210) with
  the case's `bits` (the last Move's): ResolveAction and the resolvers,
  ResetStealthValues, TryCommand / ForceCommand as original; Sleeping /
  SetSleeping through the object-stat seams. Adds `motion`.
- `find-characters` / `can-see` / `is-enemy` / `hearing` / `sight` (kata
  M9b): FindCharacters
  `0x4cd690` (the case's `max`, `range`, `angle`, `anglerange`, `flags`),
  CanSeeCharacter `0x4cd540` (`target`, `angle`), IsEnemy `0x4c89c0`
  (`target`), Hearing `0x4cda80` / Sight `0x4cdb30` (slots 0x2e0 / 0x2e4,
  the case's `dist`), as original with IsValidTarget, HasSeenMe /
  SetHasSeen, over the map iterator's characters; the case sets
  `perception`. Adds `found` (FindCharacters' list) and `memory` (the
  characters remembered, +0x1c0).

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
  query recorded -- except in a case with `perception`, where it runs as
  original;
- TPlayer SetPlayerState `0x51d680` (thiscall, 1 arg): recorded, the value
  stored at +0x36c (assumption: the UI side of it is not modelled);
- CanSeeCharacter `0x4cd540` (thiscall, 2 args): the case's `sees`
  (default yes), recorded -- original with `perception`;
- TMapPane LineOfSight `0x4533d0` (thiscall (from, to, level, 0, 0), ret
  0x14): clear unless the case's `walls` holds the pair of characters whose
  eyes (LIGHTINGCHARHEIGHT above their positions) the points are, recorded.

The case's characters also take their `chardata` whole (data_parse's
layout), the type name (`type`, else the name), `invisiblespell`, a
player's `team`, and `hasseen` ([name, frame, noautocombat] entries).

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
from guest import (O_CHARDATA, O_POS, SLOT_MOVE, Boundaries, CombatWorld, call, s32, serve, start,  # noqa: E402
                   write_fields)
from data_parse import CHAR_FIELDS  # noqa: E402

SCHEMA = 'combat.call.v1'
GO = 0x4ce350
RESOLVE_COMBAT, RESOLVE_COMBAT_MOVE = 0x4c7980, 0x4c7f80
CALCULATE_DAMAGE = 0x4c4860
FIND_CLEAR_PATH = 0x4c39d0
FIND_CHARACTERS = 0x4cd690
SET_PLAYER_STATE = 0x51d680
SLOT_UPDATE_ACTION = 0x210                       # TCharacter::UpdateAction 0x4c3260 (both classes)
UPDATE_MOVE = 0x47de30                           # TPlayScreen::UpdateMove (thiscall, PlayScreen)
G_CMDSTATE, G_CMDCHANGED, G_DEBUGCAMERA = 0x65a9c4, 0x65a9c8, 0x6671f0
# Block / StopBlock and the bow aim are the melee and spell tracks' (seams
# here, recorded: a case that reaches them shows it).
INPUT_SEAMS = ((0x4d2e30, 'Block', 4), (0x4d30f0, 'StopBlock', 0), (0x4d1050, 'IsBowDrawn', 0),
               (0x4d0dc0, 'AimBowLeft', 0), (0x4d0de0, 'AimBowRight', 0))
SIDE_STEP = 0x4d6220                             # TCharacter::SideStep (thiscall (char dir), ret 4)
LEAP, START_RETREAT = 0x4d2be0, 0x4d5fc0         # thiscall (angle) ret 4; thiscall () ret 0
KNOCK_BACK = 0x4d3750                            # thiscall (S3DPoint* from, variant) ret 8
COMPLEX_PULSE = 0x4db190                         # TComplexObject::Pulse (UpdateAction with +0xbc)
SET_OBJECT_MOTION, NEXT_FRAME = 0x470bb0, 0x470cc0
CAN_SEE = 0x4cd540
IS_ENEMY = 0x4c89c0                              # thiscall (chr) ret 4
LINE_OF_SIGHT = 0x4533d0                         # TMapPane, thiscall (from*, to*, level, 0, 0) ret 0x14
EYE_HEIGHT = 50                                  # LIGHTINGCHARHEIGHT
SLOT_HEARING, SLOT_SIGHT = 0x2e0, 0x2e4             # thiscall (dist) ret 4
PERCEPTION_CALLS = ('find-characters', 'can-see', 'is-enemy', 'hearing', 'sight')


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
        b.add(LINE_OF_SIGHT, 'LineOfSight', 0x14, self._line_of_sight)
        for address, name, pop in INPUT_SEAMS:
            b.add(address, name, pop, (lambda n: lambda args, ecx: self._input_seam(n, args, ecx))(name))
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
        if self.case.get('perception'):
            return Boundaries.ORIGINAL
        query = dict(max=s32(args[1]), range=s32(args[2]), angle=s32(args[3]),
                     anglerange=s32(args[4]), flags=s32(args[5]))
        self.world.seams.append(dict(seam='FindCharacters', who=self.world._name(ecx), **query, result=0))
        return 0

    def _set_player_state(self, args, ecx):
        self.vm.put_u32(ecx + 0x36c, args[0])
        self.world.seams.append(dict(seam='SetPlayerState', who=self.world._name(ecx), value=args[0]))
        return 0

    def _can_see(self, args, ecx):
        if self.case.get('perception'):
            return Boundaries.ORIGINAL
        sees = int(bool(self.case.get('sees', True)))
        self.world.seams.append(dict(seam='CanSeeCharacter', who=self.world._name(ecx),
                                     target=self.world._name(args[0]), result=sees))
        return sees

    def _line_of_sight(self, args, ecx):
        line = [self._eyes_of(args[0]), self._eyes_of(args[1])]
        clear = int(line not in self.case.get('walls', []))
        self.world.seams.append(dict(seam='LineOfSight', **{'from': line[0]}, to=line[1], result=clear))
        return clear

    def _eyes_of(self, point):
        """The character whose eyes are at the S3DPoint at `point`."""
        x, y, z = struct.unpack('<3i', self.vm.uc.mem_read(point, 12))
        for name, obj in self.world.by_name.items():
            px, py, pz = struct.unpack('<3i', self.vm.uc.mem_read(obj + O_POS, 12))
            if (px, py, pz + EYE_HEIGHT) == (x, y, z):
                return name
        return '?'

    def _input_seam(self, name, args, ecx):
        self.world.seams.append(dict(seam=name, who=self.world._name(ecx)))
        return 0

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
            obj = world.by_name[spec['name']]
            world.set_blocks(obj, spec)
            write_fields(vm, vm.u32(obj + O_CHARDATA), CHAR_FIELDS, spec.get('chardata', {}))
            world.set_memory(obj, spec)
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
        elif kind == 'sidestep':
            call(vm, SIDE_STEP, (ord(case['dir']) if case.get('dir') else 0,), this=me)
            result = 0                            # retail's return is a leftover register
        elif kind == 'leap':
            result = s32(call(vm, LEAP, (case['angle'] & 0xffffffff,), this=me))
        elif kind == 'knockback':
            point = vm.allocate(12)
            vm.write(point, struct.pack('<3i', *case['from']))
            result = s32(call(vm, KNOCK_BACK, (point, case.get('variant', -1) & 0xffffffff), this=me))
        elif kind == 'start-retreat':
            call(vm, START_RETREAT, (), this=me)
            result = 0
        elif kind == 'update-move':
            controls = case.get('controls', {})
            vm.put_u32(G_CMDSTATE, controls.get('state', 0))
            vm.put_u32(G_CMDCHANGED, controls.get('changed', 0))
            vm.put_u32(G_DEBUGCAMERA, 0)
            call(vm, UPDATE_MOVE, (), this=0x65caf0)
            result = 0
        elif kind == 'update-action':
            call(vm, vm.u32(vm.u32(me) + SLOT_UPDATE_ACTION), (case.get('bits', 0) & 0xffffffff,), this=me)
            result = 0
        elif kind == 'find-characters':
            most = case.get('max', 1)
            chars = vm.allocate(4 * max(most, 1))
            result = s32(call(vm, FIND_CHARACTERS, tuple(v & 0xffffffff for v in (
                chars, most, case.get('range', 0), case.get('angle', 0), case.get('anglerange', 0),
                case.get('flags', 0))), this=me))
            found = [world._name(vm.u32(chars + 4 * i)) for i in range(max(result, 0))]
        elif kind == 'can-see':
            result = s32(call(vm, CAN_SEE, (world.by_name[case['target']], case.get('angle', -1) & 0xffffffff),
                              this=me))
        elif kind in ('hearing', 'sight'):
            slot = SLOT_HEARING if kind == 'hearing' else SLOT_SIGHT
            result = s32(call(vm, vm.u32(vm.u32(me) + slot), (case['dist'] & 0xffffffff,), this=me))
        elif kind == 'is-enemy':
            result = s32(call(vm, IS_ENEMY, (world.by_name[case['target']],), this=me))
        elif kind == 'calculate-damage':
            result = [s32(call(vm, CALCULATE_DAMAGE, tuple(v & 0xffffffff for v in inp), this=me))
                      for inp in case['inputs']]
        else:
            raise ValueError(f'unknown call {kind!r}')
        new_blocks = []
        out = dict(schema=SCHEMA, side='retail', returned=result,
                   self=world.character_dump(me, new_blocks), seams=list(world.seams),
                   draws=list(world.draws))
        if kind in ('move', 'update-action', 'start-retreat', 'knockback'):
            out['motion'] = world.motion_dump(me)
        if kind in PERCEPTION_CALLS:
            out['found'] = found if kind == 'find-characters' else []
            out['memory'] = world.memory_dump(me)
        if kind == 'update-move':
            out['controls'] = dict(state=vm.u32(G_CMDSTATE), changed=vm.u32(G_CMDCHANGED))
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
