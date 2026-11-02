#!/usr/bin/env python3
"""Combat A/B, retail side, the scenario kata (docs/gameplay/forensics/
SCENARIO_KATA.md): retail's whole game tick over a small world, `ticks`
times, each tick recorded.

The world is the melee fixture's (melee_attack.py: characters with their
whole character data, stats, attack, AI and tick state; the rules), with
the case's real imagery state tables (each state's name, frames, flags and
motion a frame) and a flat floor (`ground`). One tick is TScreen::TimerTick
`0x490bd0` as far as it plays the game (SCENARIO_KATA.md §2):

1. the player's key presses that came in before the tick: the held
   controls are the control map's state and changed masks (`0x65a9c4` /
   `0x65a9c8`); a command key runs TPlayScreen::Command `0x47cf40` as
   original (`command`: "combat", case 1: into or out of combat); an attack
   key is Command's ButtonAttack(n) `0x4d2480` on the player (cases
   0x21..0x2f);
2. TPlayScreen::Pulse `0x47bd20`'s UpdateMove `0x47de30`, while the player
   has control (PlayScreen `+0x5e0`);
3. TMapPane::Pulse `0x454390`'s PulseObjects `0x4580f0` (each object's
   Pulse, slot 0x110) and MoveObjects `0x458390` (Move, slot 0x114, its
   bits to `+0xbc`; then SetObjectMotion, slot 0x118);
4. TPlayScreen::Animate `0x47c2c0`'s frame count and game time:
   `+0x680`++, `+0x68c` = GameFrame * 100 / 24 (`0x47c36b..0x47c3af`);
5. screen slot 0x24 `0x4902c0` -> NextFrameObjects `0x457ef0` (slot 0x120);
6. the main loop's screen frame, CurrentScreen `+0x48`++ (`0x491454`).

The three map loops run as original code; the map iterator they open
(`0x44cf80`, flags 0xa0, objsets 4 / 1 / 5) gives the case's characters in
case order. Everything they call runs as original -- Pulse, UpdateAction,
the resolvers, AI, FindCharacters and the senses, IsEnemy, BeginFighting,
the attack choice, ResolveAttack / ResolveHit, Damage, Block, the line of
sight over the ground (LineOfSight `0x4533d0`, kata M11's), Move /
MoveStep / FindClearPath / CharBlocking over the ground, SetState,
SetObjectMotion, NextFrame, the imagery's state queries (FindState with
its frequency variants) -- except the melee fixture's seams for what lies
outside the game's state (sounds, effects, the text bar, the death screen,
RefreshStats, the spell list's pulse) and these:

- the map's object removal (`0x451840`, an OF_KILL object PulseObjects
  takes out): recorded, the object left out of the next ticks' loops.

Not run (rendering): TScreen::Animate's object Animate (slot 0x11c, the
animator's Animate), drawing, the camera (`0x47e0f0`).

Case: the melee fixture's (`chars`, `rules`, `globals` with `frame`,
`gametime`, `screenframe`), `call` "scenario", `ticks`, `ground`,
`perception` (true: the senses as original), `seam_ai` / `seam_update` /
`seam_damage` / `seam_block` / `seam_begin` / `seam_path` false, and
`inputs`: a list of {tick, controls: [state, changed]} / {tick, command:
name} / {tick, attack: button}, a tick's taken in that order. Out: `ticks`,
one record a tick ({tick, frame, chars: each character's dump and vitals,
seams, draws}), then each character's AI and tick state.

Schema `combat.scenario.v1`, shared with the port's `Revenant
--retail-ab=combat-scenario` (src/game/ab/retailab_scenario.cpp).
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from guest import G_PLAYSCREEN, G_PS_CONTROL, OBJSTAT_IDS, call, s32, serve  # noqa: E402
from melee_attack import SCHEMA as MELEE_SCHEMA, SCREEN_FRAMES, MeleeFixture  # noqa: E402,F401

SCHEMA = 'combat.scenario.v1'
MAPPANE = 0x6668d8                               # the TMapPane global (static init stores its vtable)
UPDATE_MOVE = 0x47de30                           # TPlayScreen::UpdateMove (thiscall, PlayScreen)
PULSE_OBJECTS, MOVE_OBJECTS, NEXT_FRAME_OBJECTS = 0x4580f0, 0x458390, 0x457ef0
BUTTON_ATTACK = 0x4d2480                         # thiscall (button) ret 4
COMMAND = 0x47cf40                               # TPlayScreen::Command, thiscall (command) ret 4
COMMANDS = {'combat': 1}                         # the command keys a case presses, by retail's number
G_CMDSTATE, G_CMDCHANGED, G_DEBUGCAMERA = 0x65a9c4, 0x65a9c8, 0x6671f0
PS_FRAMES, PS_GAMETIME = 0x680, 0x68c
PS_SESSIONSTART, PS_SESSIONFRAMES = 0x684, 0x688
# The tick loops' iterator: TMapIterator init (thiscall (rect, flags, objset,
# 0, level), ret 0x14), its item at +0x0c, next 0x44d080 (guest.py's).
TICK_ITERATOR, TICK_FLAGS, TICK_OBJSETS = 0x44cf80, 0xa0, (1, 4, 5)
REMOVE_OBJECT = 0x451840                         # TMapPane (thiscall (inst), ret 4)
VITALS = ('health', 'fatigue', 'mana')
# Instructions a step of the tick may take: the attack search runs FindState
# over the player's 352 states for each of his ~90 attacks.
TICK_BUDGET = 200_000_000
# A player's weapon in hand (`wielding`): the prime hand, TPlayer +0x2b0.
# TWeapon (vtable 0x5b9088) as the code asks it: Type (slot 0x1f0,
# 0x528f70; the combat root's name 0x51b340), DamageType (slot 0x210,
# 0x529060; TPlayer::GetDamageType 0x51b2f0), a stat by name (slot 0xd4;
# WeaponDamage 0x520830's "Damage").
O_PRIMEHAND, OBJCLASS_WEAPON = 0x2b0, 1
ITEM_SLOTS, ITEM_SIZE = 0x100, 0x100
SLOT_TYPE, SLOT_DAMAGETYPE, SLOT_GETSTAT_NAMED = 0x1f0, 0x210, 0xd4


class ScenarioFixture(MeleeFixture):
    def __init__(self, executable):
        super().__init__(executable)
        b = self.world.boundaries
        b.add(TICK_ITERATOR, 'MapIterator(tick)', 0x14, self._tick_iterator)
        b.add(REMOVE_OBJECT, 'RemoveObject', 4, self._remove_object)
        self.weapons = {}                        # stand-in weapon -> the case's `wielding`
        self.weapon_vtable = self.world._stub_table(ITEM_SLOTS, {
            SLOT_TYPE: ('Type', 0, lambda owner, args: self.weapons[owner]['type']),
            SLOT_DAMAGETYPE: ('DamageType', 0, lambda owner, args: self.weapons[owner]['damagetype']),
            SLOT_GETSTAT_NAMED: ('GetStat', 4, self._weapon_stat)},
            lambda ecx: ecx if ecx in self.weapons else None, 'weapon', record=False)
        self.vm.checkpoint()
        self.removed = set()

    # -- the world -------------------------------------------------------------
    def _prepare(self, case):
        """The animation layer as original code over the case's imagery."""
        self.world.real_setstate = True
        self.world.real_imagery = True
        self.removed = set()

    def _tick_iterator(self, args, ecx):
        """The map loops' iterator: the case's characters in case order (all
        of them are in the pulse, moving and animate sets), less the
        removed. Any other walk with this iterator fails the case."""
        flags, objset = args[1], args[2]
        if args[0] or flags != TICK_FLAGS or objset not in TICK_OBJSETS:
            raise RuntimeError(f'map iterator 0x44cf80 with flags {flags:#x} objset {objset} (not modelled)')
        world = self.world
        world.iters[ecx] = [world.by_name[c['name']] for c in self.case['chars']
                            if world.by_name[c['name']] not in self.removed]
        world._iter_next(args, ecx)
        return ecx

    def _remove_object(self, args, ecx):
        self.removed.add(args[0])
        self.world.seams.append(dict(seam='RemoveObject', who=self.world._name(args[0])))
        return 0

    def _weapon_stat(self, owner, args):
        """A weapon's type stat by name: the case's `wielding` holds the ones
        the code asks (Type, Damage)."""
        name = self.vm.string(args[0]).lower()
        wielding = self.weapons[owner]
        if name not in wielding:
            raise KeyError(f"the wielded {wielding['name']} has no {name} in the case")
        return wielding[name]

    def _wield(self, case):
        """Each player's `wielding` ({name, type, damage, damagetype}): a stand-in weapon
        (class WEAPON) in its prime hand."""
        vm, world = self.vm, self.world
        self.weapons = {}
        for spec in case['chars']:
            wielding = spec.get('wielding')
            if not wielding:
                continue
            item = vm.allocate(ITEM_SIZE)
            vm.put_u32(item, self.weapon_vtable)
            vm.write(item + 4, struct.pack('<h', OBJCLASS_WEAPON))
            self.weapons[item] = wielding
            world.objects[item] = wielding['name']
            vm.put_u32(world.by_name[spec['name']] + O_PRIMEHAND, item)

    # -- the tick ----------------------------------------------------------------
    def _tick(self, t, step, player):
        vm, ps = self.vm, G_PLAYSCREEN
        if 'controls' in step:
            state, changed = step['controls']
            vm.put_u32(G_CMDSTATE, state & 0xffffffff)
            vm.put_u32(G_CMDCHANGED, changed & 0xffffffff)
        if 'command' in step:
            call(vm, COMMAND, (COMMANDS[step['command']],), this=ps, instruction_limit=TICK_BUDGET)
        if 'attack' in step and player:
            call(vm, BUTTON_ATTACK, (step['attack'] & 0xffffffff,), this=player, instruction_limit=TICK_BUDGET)
        if vm.u32(G_PS_CONTROL):
            call(vm, UPDATE_MOVE, (), this=ps, instruction_limit=TICK_BUDGET)
        call(vm, PULSE_OBJECTS, (), this=MAPPANE, instruction_limit=TICK_BUDGET)
        call(vm, MOVE_OBJECTS, (), this=MAPPANE, instruction_limit=TICK_BUDGET)
        frames = vm.u32(ps + PS_FRAMES) + 1
        vm.put_u32(ps + PS_FRAMES, frames)
        game_frame = s32(vm.u32(ps + PS_SESSIONFRAMES)) - s32(vm.u32(ps + PS_SESSIONSTART)) + frames
        vm.put_u32(ps + PS_GAMETIME, (game_frame * 100 // 24) & 0xffffffff)
        call(vm, NEXT_FRAME_OBJECTS, (), this=MAPPANE, instruction_limit=TICK_BUDGET)
        vm.put_u32(self.screen + SCREEN_FRAMES, vm.u32(self.screen + SCREEN_FRAMES) + 1)
        return game_frame

    def _char_record(self, obj):
        out = self._dump(obj, [])
        stats = self.world.stats.get(obj, {})
        out['vitals'] = {k: stats.get(OBJSTAT_IDS[k][1]) for k in VITALS}
        return out

    def run(self, case):
        if case.get('call') != 'scenario':
            raise ValueError(f"the scenario fixture runs `scenario` cases, not {case.get('call')!r}")
        vm, world = self.vm, self.world
        self.build(case)
        self._wield(case)
        vm.put_u32(G_DEBUGCAMERA, 0)
        vm.put_u32(G_CMDSTATE, 0)
        vm.put_u32(G_CMDCHANGED, 0)
        names = [c['name'] for c in case['chars']]
        player = next((world.by_name[c['name']] for c in case['chars'] if c.get('class') == 11), None)
        inputs = {}
        for event in case.get('inputs', []):
            inputs.setdefault(event['tick'], {}).update({k: v for k, v in event.items() if k != 'tick'})
        ticks = []
        for t in range(case['ticks']):
            first_seam, first_draw = len(world.seams), len(world.draws)
            frame = self._tick(t, inputs.get(t, {}), player)
            chars = {n: self._char_record(world.by_name[n]) for n in names if world.by_name[n] not in self.removed}
            ticks.append(dict(tick=t, frame=frame, chars=chars, seams=world.seams[first_seam:],
                              draws=world.draws[first_draw:]))
        return dict(schema=SCHEMA, side='retail', ticks=ticks,
                    ai={n: self._ai_dump(world.by_name[n]) for n in names},
                    pulse={c['name']: self._pulse_dump(world.by_name[c['name']], c)
                           for c in case['chars'] if 'pulsestate' in c})


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable')
    parser.add_argument('--serve', action='store_true')
    parser.add_argument('--case', type=Path)
    args = parser.parse_args()
    fixture = ScenarioFixture(args.executable)
    if args.serve:
        serve(fixture.sha, SCHEMA, fixture.setup_ms, fixture.run)
    elif args.case:
        print(json.dumps(fixture.run(json.loads(args.case.read_text())), indent=1))


if __name__ == '__main__':
    main()
