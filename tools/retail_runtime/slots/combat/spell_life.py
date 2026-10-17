#!/usr/bin/env python3
"""Combat A/B, retail side, kata S5: spell lifetimes and effects.

The case's `call`:

- `spell-ticks` (targets spell-life, spell-strike): the case's `spells`
  ({class, variant, invoker, targets, numtargs, sourcepos, owner (the
  invoker by default)}) built by their class's creator (as spell_damage.py
  builds one) and put in their owner's manager (`+0x170`, the array's Add
  `0x41c840`); then `ticks` ticks, each running the tick's `events` and then
  every character's TSpellManager::Pulse `0x540750` (case order): the
  spells' Pulse / Timer / Kill as original (TSpell `0x53f800` / `0x53f410` /
  `0x53f860`, TStrikeSpell `0x540d70` / `0x540eb0` / `0x542340`), the
  effects they make pulsed by TSpell::Pulse when in no sector (TEffect::Pulse
  `0x4de800`, original).
- `effect-pulse`: one TEffect (vtable `0x5a85ac`) set up from the case's
  `effect` (and with `spell`, that spell's) pulsed `ticks` times by
  TEffect::Pulse `0x4de800`.
- `effect-iced`: one Iced effect (vtable `0x5ab25c`) from the case's `iced`,
  given the case's `target` by Set `0x4ec930`, pulsed by its Pulse
  `0x4ec890`.

`events`: [{tick, do: "kill" (KillThisEffect `0x4defe0` on `effect`) |
"removed" (TSpellManager::ObjectRemoved `0x5408d0` (mode, ptr) on each of
`who`, every character by default: `what` the object by name, with mode 2
the sector it is in)}], run at the start of their tick.

`effect` / `iced`: {type, pos, level, flags (the def's), frame, follow (a
name), angle, dist, orbit, followtime, attach, visiblewhen, timed,
duration, blink, startwait}.

`control` ({flags, radius, hits, duration, wait, duration2, pattern,
hittarget, oncaster, attachset, follow, posset, repeatset,
multipletargets, imagery, rangedamage, pos, attach, sound}) and
`variantfields` ({facing}) change the CONTROLDATA / variant of
`controlvariant` for the case.

The effects the spells make: NewObject `0x450e40` (arrow kata's seam,
recorded, the imagery too) makes the n-th (from 1) "fx<n>", id 99 + n, in
a stand-in sector unless `sectorless` lists n; `fail` lists those it
refuses (-1). An effect is TEffect-shaped whatever its type: the class
behind the type is the effect's own (VFX), not modelled.

Seams beyond spell_arrow.py's: TObjectInstance::Pulse `0x4708e0` (the
animator and script: none here, not recorded); MapPane ObjectFlagsChanged
`0x451a50` (the stand-in sector's lists: not recorded); MapPane AddObject
`0x451090`: {who}, 0. Globals: Realtime lighting `0x5d7a18` off (an effect
is ready to go at once), the pause flag `0x65d0c4` off.

A tick's record: the casters (each manager's wait and spells, each spell's
effect list and last effect), every effect made, each character's freeze
(OF_PARALIZE, OF_ICED), the seams and draws.

Schema `combat.spell.v1`, shared with the port's `Revenant
--retail-ab=spell-life` (and spell-strike, effect-pulse, effect-iced).
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path[:0] = [str(HERE), str(HERE.parents[1])]
from guest import call, s32, serve  # noqa: E402
from spell_arrow import ArrowFixture  # noqa: E402
from spell_cast import G_EDITOR, POINTER_ARRAY_CTOR, SCHEMA  # noqa: E402

MANAGER_PULSE, MANAGER_REMOVED, ARRAY_ADD = 0x540750, 0x5408d0, 0x41c840
OBJECT_PULSE, FLAGS_CHANGED, ADD_OBJECT = 0x4708e0, 0x451a50, 0x451090
EFFECT_PULSE, KILL_EFFECT, FOLLOW = 0x4de800, 0x4defe0, 0x4def90
ICED_SET, ICED_PULSE = 0x4ec930, 0x4ec890
EFFECT_VTABLE, ICED_VTABLE, EFFECT_SIZE = 0x5a85ac, 0x5ab25c, 0x200
G_REALTIME, G_PAUSED = 0x5d7a18, 0x65d0c4
O_MANAGER = 0x170
# The guest effect: TObjectInstance's fields, then TEffect's (+0xd8..) and
# the Iced effect's (+0x184, +0x188).
O_CLASS, O_TYPE, O_FLAGS, O_STATE, O_LEVEL, O_POS, O_VEL = 0x04, 0x06, 0x08, 0x0c, 0x0e, 0x10, 0x1c
O_FACING, O_NAME, O_ID, O_SECTOR, O_FRAME, O_INVENTNUM = 0x36, 0x38, 0x40, 0x44, 0x5c, 0x7c
O_SPELL, O_FOLLOW, O_FOLLOWTIME, O_ATTACH = 0xd8, 0xe4, 0xe8, 0xec
O_ORBIT, O_ANGLE, O_DIST, O_DURATION, O_TIMED, O_BLINK, O_STARTWAIT = 0x120, 0x124, 0x128, 0x12c, 0x130, 0x134, 0x138
O_VISIBLEWHEN, O_ICED_TARGET, O_ICED_TICKS = 0x15c, 0x184, 0x188
# An object built from a def: notify (the object constructor), animate and
# pulse (its effect class), immobile and pulse (TEffect's).
EFFECT_FLAGS = 0x40000 | 0xc000 | 0x1
FLAG_NAMES = {0x4000: 'animate', 0x1: 'immobile', 0x80: 'invisible', 0x1000: 'kill', 0x80000: 'nonmap',
              0x8000: 'pulse'}
OF_PARALIZE, OF_ICED = 0x800000, 0x2000000
# SSpellControlData fields a case may change.
CONTROL_FIELDS = dict(flags=0x20, radius=0x24, hits=0x28, duration=0x2c, wait=0x30, duration2=0x34, pattern=0x38,
                      imagery=0x94, hittarget=0x9c, oncaster=0xa0, attachset=0xa4, follow=0xa8, posset=0xac,
                      repeatset=0xb0, multipletargets=0xb4)
C_RANGEDAMAGE, C_POS, C_ATTACH, C_SOUND = 0x48, 0xb8, 0x50, 0x74
V_FACING, V_CONTROL = 0x68, 0x70


class LifeFixture(ArrowFixture):
    def __init__(self, executable):
        super().__init__(executable)
        vm = self.vm
        vm.restore()
        b = self.world.boundaries
        b.add(OBJECT_PULSE, 'TObjectInstance::Pulse', 0, lambda args, ecx: 0)
        b.add(FLAGS_CHANGED, 'MapPane.ObjectFlagsChanged', 0xc, lambda args, ecx: 0)
        b.add(ADD_OBJECT, 'AddObject', 4, self._add_object)
        self.sector = vm.allocate(0x40)              # the effects' stand-in sector
        vm.checkpoint()

    # -- seams --------------------------------------------------------------
    def _new_object(self, args, ecx):
        super()._new_object(args, ecx)
        n = self.made + 1
        if n in self.case.get('fail', []):
            self.made = n
            return -1
        vm = self.vm
        d = args[0]
        objclass, objtype = struct.unpack('<hh', vm.uc.mem_read(d, 4))
        state, level = struct.unpack('<HH', vm.uc.mem_read(d + 8, 4))
        fx = self.make_effect(EFFECT_VTABLE, objclass, vm.u32(d + 4), level,
                              struct.unpack('<3i', vm.uc.mem_read(d + 0x0c, 12)),
                              struct.unpack('<3i', vm.uc.mem_read(d + 0x18, 12)), vm.uc.mem_read(d + 0x32, 1)[0])
        return vm.u32(fx + O_ID)

    def _add_object(self, args, ecx):
        self.world.seams.append(dict(seam='AddObject', who=self.world._name(args[0])))
        return 0

    # -- the effects --------------------------------------------------------
    def make_effect(self, vtable, objclass, flags, level, pos, vel=(0, 0, 0), facing=0):
        """The next effect, "fx<n>", as an object constructor and TEffect's
        leave one (its fields zero), in the stand-in sector unless the case
        keeps it out."""
        vm, w = self.vm, self.world
        self.made += 1
        n = self.made
        fx = vm.allocate(EFFECT_SIZE)
        vm.put_u32(fx, vtable)
        vm.write(fx + O_CLASS, struct.pack('<h', objclass))
        vm.put_u32(fx + O_FLAGS, flags | EFFECT_FLAGS)
        vm.write(fx + O_LEVEL, struct.pack('<H', level))
        vm.write(fx + O_POS, struct.pack('<3i', *pos))
        vm.write(fx + O_VEL, struct.pack('<3i', *vel))
        vm.write(fx + O_FACING, bytes([facing & 0xff]))
        vm.write(fx + O_INVENTNUM, struct.pack('<h', -1))
        name = f'fx{n}'
        text = vm.allocate(16)
        vm.write(text, name.encode() + b'\0')
        vm.put_u32(fx + O_NAME, text)
        vm.put_u32(fx + O_ID, 99 + n)
        if n not in self.case.get('sectorless', []):
            vm.put_u32(fx + O_SECTOR, self.sector)
        w.objects[fx] = name
        self.by_id[99 + n] = fx
        self.effects.append(fx)
        return fx

    def effect_dump(self, fx):
        vm, w = self.vm, self.world
        flags = vm.u32(fx + O_FLAGS)
        spell, follow = vm.u32(fx + O_SPELL), vm.u32(fx + O_FOLLOW)
        out = dict(name=w._name(fx), pos=list(struct.unpack('<3i', vm.uc.mem_read(fx + O_POS, 12))),
                   flags=sorted(n for bit, n in FLAG_NAMES.items() if flags & bit),
                   spell=list(self.by_variant[vm.u32(spell + 0x120)]) if spell else None,
                   follow=w._name(follow) if follow else None, followtime=s32(vm.u32(fx + O_FOLLOWTIME)),
                   attach=vm.string(fx + O_ATTACH), orbit=vm.u32(fx + O_ORBIT),
                   angle=s32(vm.u32(fx + O_ANGLE)), dist=s32(vm.u32(fx + O_DIST)),
                   duration=s32(vm.u32(fx + O_DURATION)), timed=vm.u32(fx + O_TIMED),
                   blink=s32(vm.u32(fx + O_BLINK)), startwait=s32(vm.u32(fx + O_STARTWAIT)))
        if vm.u32(fx) == ICED_VTABLE:
            target = vm.u32(fx + O_ICED_TARGET)
            out['target'] = w._name(target) if target else None
            out['ticks'] = s32(vm.u32(fx + O_ICED_TICKS))
        return out

    # -- the dump -----------------------------------------------------------
    def life_casters(self):
        vm, w = self.vm, self.world
        out = {}
        for spec in self.case['chars']:
            m = w.by_name[spec['name']] + O_MANAGER
            count, items = vm.u32(m), vm.u32(m + 0x10)
            spells = []
            for sp in (vm.u32(items + 4 * i) for i in range(count)):
                if not sp:
                    continue
                d = self.spell_dump(sp)
                n, fxs = vm.u32(sp + 0x13c), vm.u32(sp + 0x14c)
                d['effects'] = [w._name(p) for p in (vm.u32(fxs + 4 * i) for i in range(n)) if p]
                last = vm.u32(sp + 8)
                d['last'] = w._name(last) if last else None
                spells.append(d)
            out[spec['name']] = dict(wait=s32(vm.u32(m + 0x14)), spells=spells)
        return out

    def frozen(self):
        vm, w = self.vm, self.world
        return {spec['name']: dict(paralize=int(bool(vm.u32(w.by_name[spec['name']] + O_FLAGS) & OF_PARALIZE)),
                                   iced=int(bool(vm.u32(w.by_name[spec['name']] + O_FLAGS) & OF_ICED)))
                for spec in self.case['chars']}

    # -- one case -----------------------------------------------------------
    def control(self, case):
        vm = self.vm
        if 'control' not in case and 'variantfields' not in case:
            return
        sd, v = self.by_names[tuple(case['controlvariant'])]
        if 'facing' in case.get('variantfields', {}):
            vm.put_u32(v + V_FACING, case['variantfields']['facing'] & 0xffffffff)
        cd = vm.u32(v + V_CONTROL)
        c = case.get('control', {})
        for key, off in CONTROL_FIELDS.items():
            if key in c:
                vm.put_u32(cd + off, c[key] & 0xffffffff)
        if 'rangedamage' in c:
            vm.write(cd + C_RANGEDAMAGE, struct.pack('<2i', *c['rangedamage']))
        if 'pos' in c:
            vm.write(cd + C_POS, struct.pack('<3i', *c['pos']))
        for key, off in (('attach', C_ATTACH), ('sound', C_SOUND)):
            if key in c:
                vm.write(cd + off, c[key].encode('cp1252')[:31] + b'\0')

    def event(self, e):
        vm, w = self.vm, self.world
        if e['do'] == 'kill':
            call(vm, KILL_EFFECT, (), this=self.find(e['effect']))
            return
        if e['do'] != 'removed':
            raise ValueError(f"unknown event {e['do']!r}")
        obj = self.find(e['what']) if e.get('what') else 0
        ptr = vm.u32(obj + O_SECTOR) if e['mode'] == 2 and obj else obj
        for name in e.get('who', [spec['name'] for spec in self.case['chars']]):
            call(vm, MANAGER_REMOVED, (e['mode'], ptr), this=w.by_name[name] + O_MANAGER)

    def find(self, name):
        w = self.world
        for fx in self.effects:
            if w._name(fx) == name:
                return fx
        return w.by_name[name]

    def run(self, case):
        vm, w = self.vm, self.world
        self.build_world(case)
        self.case = case
        vm.put_u32(G_EDITOR, int(case.get('globals', {}).get('editor', 0)))
        vm.put_u32(G_REALTIME, 0)
        vm.put_u32(G_PAUSED, 0)
        if not case.get('mainplayer', True):
            vm.put_u32(0x667fcc, 0)
        self.casts, self.types, self.items, self.effects, self.made = [], [], {}, [], 0
        self.ids = {spec['name']: spec.get('id', 0) for spec in case['chars']}
        self.by_id = {spec.get('id', 0): w.by_name[spec['name']] for spec in case['chars']}
        for spec in case['chars']:
            obj = w.by_name[spec['name']]
            call(vm, POINTER_ARRAY_CTOR, (0x10, 0x10), this=obj + O_MANAGER)
            vm.put_u32(obj + O_MANAGER + 0x14, spec.get('spellwait', 0) & 0xffffffff)
            self.spells.set_records(obj, spec)
        self.control(case)
        w.seams.clear()
        w.draws.clear()
        kind = case['call']
        ticks = []
        if kind == 'spell-ticks':
            for s in case.get('spells', []):
                spell = self.build_spell(s)
                owner = s.get('owner', s.get('invoker'))
                call(vm, ARRAY_ADD, (spell,), this=w.by_name[owner] + O_MANAGER)
            for t in range(case.get('ticks', 1)):
                w.seams.clear()
                w.draws.clear()
                for e in case.get('events', []):
                    if e['tick'] == t:
                        self.event(e)
                for spec in case['chars']:
                    call(vm, MANAGER_PULSE, (), this=w.by_name[spec['name']] + O_MANAGER)
                ticks.append(dict(tick=t, casters=self.life_casters(), effects=[self.effect_dump(fx) for fx in self.effects],
                                  frozen=self.frozen(), seams=list(w.seams), draws=list(w.draws)))
            return dict(schema=SCHEMA, side='retail', ticks=ticks)
        if kind not in ('effect-pulse', 'effect-iced'):
            raise ValueError(f'unknown call {kind!r}')
        iced = kind == 'effect-iced'
        f = case['iced'] if iced else case['effect']
        spell = self.build_spell(case['spell']) if case.get('spell') else 0
        fx = self.make_effect(ICED_VTABLE if iced else EFFECT_VTABLE, 0x19, f.get('flags', 0), f.get('level', 0),
                              f['pos'])
        vm.write(fx + O_FRAME, struct.pack('<h', f.get('frame', 0)))
        vm.put_u32(fx + O_SPELL, spell)
        if f.get('follow'):
            call(vm, FOLLOW, (w.by_name[f['follow']], f.get('angle', 0) & 0xffffffff, f.get('dist', 0) & 0xffffffff,
                              int(f.get('orbit', False))), this=fx)
            vm.put_u32(fx + O_FOLLOWTIME, f.get('followtime', 0) & 0xffffffff)
        if f.get('attach'):
            vm.write(fx + O_ATTACH, f['attach'].encode('cp1252')[:0x31] + b'\0')
        if f.get('visiblewhen'):
            vm.write(fx + O_VISIBLEWHEN, f['visiblewhen'].encode('cp1252')[:31] + b'\0')
        if f.get('timed'):
            vm.put_u32(fx + O_DURATION, f.get('duration', 0) & 0xffffffff)
            vm.put_u32(fx + O_BLINK, f.get('blink', 0) & 0xffffffff)
            vm.put_u32(fx + O_TIMED, 1)
        vm.put_u32(fx + O_STARTWAIT, f.get('startwait', 0) & 0xffffffff)
        if iced and 'target' in case:
            call(vm, ICED_SET, (w.by_name[case['target']] if case['target'] else 0,), this=fx)
        setup = dict(seams=list(w.seams), frozen=self.frozen())
        for t in range(case.get('ticks', 1)):
            w.seams.clear()
            w.draws.clear()
            for e in case.get('events', []):
                if e['tick'] == t:
                    self.event(e)
            call(vm, ICED_PULSE if iced else EFFECT_PULSE, (), this=fx)
            ticks.append(dict(tick=t, effect=self.effect_dump(fx), spell=self.spell_dump(spell) if spell else None,
                              frozen=self.frozen(),
                              chars={s['name']: w.character_dump(w.by_name[s['name']], []) for s in case['chars']},
                              seams=list(w.seams), draws=list(w.draws)))
        return dict(schema=SCHEMA, side='retail', set=setup, ticks=ticks)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable')
    parser.add_argument('--serve', action='store_true')
    parser.add_argument('--case', type=Path)
    args = parser.parse_args()
    fixture = LifeFixture(args.executable)
    if args.serve:
        serve(fixture.sha, SCHEMA, fixture.setup_ms, fixture.run)
    elif args.case:
        print(json.dumps(fixture.run(json.loads(args.case.read_text())), indent=1))
    else:
        parser.error('--serve or --case')


if __name__ == '__main__':
    main()
