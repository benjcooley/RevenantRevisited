#!/usr/bin/env python3
"""Combat A/B, retail side, kata S2: the cast.

The case's `call`, each on the case's `self`:

- `cast-talismans`: TCharacter::CastByTalismans `0x4d5c20` (thiscall
  (talismans, targets, numtargs, sourcepos), ret 0x10);
- `cast-name`: TCharacter::CastByName `0x4d5b90` (same arguments);
- `cast`: TCharacter::Cast `0x4d5ae0` (thiscall (talismans, sourcepos), ret 8);
- `manager-cast-name` / `manager-cast-talismans`: TSpellManager::CastByName
  `0x53f920` / CastByTalismans `0x53fe80` (thiscall (text, invoker, targets,
  numtargs, sourcepos, master), ret 0x18) on `self`'s manager (`+0x170`),
  with the case's `invoker` (a name, or null), as an arrow's poison proc
  casts on its victim's manager for its shooter.

`targets`: names (an array of that many), or null for no array; `numtargs`
(default: their count); `sourcepos` ([x, y, z] or null).

Original code: the gates, the lookups, the cooldown, mana and skill roll,
the Invoke experience's arithmetic, the spell class registry (its static
init, `0x540b80` / `0x540ba0`, run at setup) and both creators, the TSpell
constructor `0x53f090` (poison rolls), ManaDrain `0x53f680`, SetCast
`0x4d5900` with CombatAnimName / AnimPrefix / HoldsLight / HasActionAni,
ForceCommand, the pointer arrays.

Seams (recorded in `seams`, in order; guest.py's shared ones, and):
- the text bar `0x54d170` (spell_guest.SpellWorld);
- AddSkillExp (TPlayer slot 0x418, `0x51ac90`): skill, exp;
- AddStatEffect `0x51c2c0`: the line;
- TPlayer::MaxMana `0x520770`: the case's `maxmana` (a character's is its
  chardata `mana`, read as original code);
- SetPlayerState `0x51d680`: the value, stored;
- the map walk a buff cast makes (iterator `0x44cf10` / next `0x44d080`):
  the case's `around` (none by default); the walk recorded.

Globals: authority `0x676838` = 3 (the single-player session's; a cast
needs >= 2), the editor flag `0x668154` and the magic cheat `0x66810c`
from the case's `globals` (`editor`, `cheat`), multiplayer off.

Schema `combat.spell.v1`, shared with the port's `Revenant
--retail-ab=spell-cast`.
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path[:0] = [str(HERE), str(HERE.parents[1])]
from guest import MALLOC, CombatWorld, call, s32, serve, start  # noqa: E402
from spell_guest import (TALISMAN_CLASS, SpellFiles, SpellWorld, build_class, class_section,  # noqa: E402
                         construct_list, load_spells, shipped_class_def, shipped_spell_def, variant_names)

SCHEMA = 'combat.spell.v1'
CAST_BY_TALISMANS, CAST_BY_NAME, CAST = 0x4d5c20, 0x4d5b90, 0x4d5ae0
MANAGER_CAST_BY_NAME, MANAGER_CAST_BY_TALISMANS = 0x53f920, 0x53fe80
SPELL_CLASS_INITS = (0x540b80, 0x540ba0)
POINTER_ARRAY_CTOR = 0x41c7f0                    # thiscall (size, grow), ret 8
ADD_SKILL_EXP, ADD_STAT_EFFECT, PLAYER_MAXMANA, SET_PLAYER_STATE = 0x51ac90, 0x51c2c0, 0x520770, 0x51d680
MAP_ITERATOR = 0x44cf10                          # the walk round an object; next is guest.py's 0x44d080
G_AUTHORITY, G_EDITOR, G_CHEAT = 0x676838, 0x668154, 0x66810c
O_MANAGER, O_INVOKEDELAY = 0x170, 0x188
SPELL_VTABLES = {0x5b99c0: 'Spell', 0x5b9bdc: 'Strike'}


class CastFixture:
    def __init__(self, executable):
        started = time.perf_counter()
        self.vm, self.sha = start(executable)
        vm = self.vm
        self.world = CombatWorld(vm)
        self.spells = SpellWorld(self.world)
        b = self.world.boundaries
        construct_list(vm, call)
        load_spells(vm, call, SpellFiles(vm, b), shipped_spell_def())
        stats, types = class_section(shipped_class_def(), 'TALISMAN')
        build_class(vm, TALISMAN_CLASS, stats, types)
        for init in SPELL_CLASS_INITS:
            call(vm, init)
        vm.put_u32(G_AUTHORITY, 3)
        b.add(ADD_SKILL_EXP, 'AddSkillExp', 8, self._add_skill_exp)
        b.add(ADD_STAT_EFFECT, 'AddStatEffect', 4, self._add_stat_effect)
        b.add(PLAYER_MAXMANA, 'MaxMana', 0, self._max_mana)
        b.add(SET_PLAYER_STATE, 'SetPlayerState', 4, self._set_player_state)
        b.add(MAP_ITERATOR, 'MapIterator', 0x14, self._iterator)
        self.by_variant, self.by_spell = variant_names(vm)
        self.text = vm.allocate(256)
        vm.checkpoint()
        self.setup_ms = (time.perf_counter() - started) * 1000
        self.case = {}

    # -- seams --------------------------------------------------------------
    def _add_skill_exp(self, args, ecx):
        self.world.seams.append(dict(seam='AddSkillExp', who=self.world._name(ecx), skill=s32(args[0]),
                                     exp=s32(args[1])))
        return 0

    def _add_stat_effect(self, args, ecx):
        self.world.seams.append(dict(seam='AddStatEffect', who=self.world._name(ecx),
                                     line=self.vm.string(args[0]) if args[0] else None))
        return 0

    def _max_mana(self, args, ecx):
        spec = self.specs[self.world._name(ecx)]
        if 'maxmana' not in spec:
            raise KeyError(f"{spec['name']} has no maxmana in the case")
        self.world.seams.append(dict(seam='MaxMana', who=spec['name'], result=spec['maxmana']))
        return spec['maxmana']

    def _set_player_state(self, args, ecx):
        self.vm.put_u32(ecx + 0x36c, args[0])
        self.world.seams.append(dict(seam='SetPlayerState', who=self.world._name(ecx), value=args[0]))
        return 0

    def _iterator(self, args, ecx):
        """The walk around an object: the case's `around` (names) in order,
        handed out by guest.py's iterator (next 0x44d080, the item at +0x0c)."""
        w = self.world
        walk = [w.by_name[n] for n in self.case.get('around', [])]
        w.seams.append(dict(seam='MapIterator', who=w._name(args[0]), objset=s32(args[2]),
                            result=[w._name(o) for o in walk]))
        w.iters[ecx] = walk
        w._iter_next(args, ecx)
        return ecx

    # -- the dump -----------------------------------------------------------
    def spell_dump(self, sp):
        vm, w = self.vm, self.world
        vt = vm.u32(sp)
        n = s32(vm.u32(sp + 0x0c))
        sd, v, master = vm.u32(sp + 0x11c), vm.u32(sp + 0x120), vm.u32(sp + 0x118)
        out = dict(**{'class': SPELL_VTABLES.get(vt, hex(vt))},
                   invoker=w._name(vm.u32(sp + 4)) if vm.u32(sp + 4) else None,
                   targets=[w._name(t) if t else None for t in (vm.u32(sp + 0x10 + 4 * i) for i in range(n))],
                   timer=s32(vm.u32(sp + 0x110)), frame=s32(vm.u32(sp + 0x114)),
                   master=None if not master else hex(master),
                   spell=self.by_spell.get(sd) if sd else None,
                   variant=list(self.by_variant[v]) if v else None,
                   wait=s32(vm.u32(sp + 0x124)),
                   source=list(struct.unpack('<3i', vm.uc.mem_read(sp + 0x128, 12))),
                   defense=s32(vm.u32(sp + 0x134)), offense=s32(vm.u32(sp + 0x138)))
        if out['class'] == 'Strike':
            t = vm.u32(sp + 0x150)
            out['striketarget'] = w._name(t) if t else None
        return out

    def manager_dump(self, obj):
        vm = self.vm
        m = obj + O_MANAGER
        count, items = vm.u32(m), vm.u32(m + 0x10)
        spells = [self.spell_dump(p) for p in (vm.u32(items + 4 * i) for i in range(count)) if p]
        return dict(wait=s32(vm.u32(m + 0x14)), spells=spells)

    def caster_dump(self, obj):
        """What a cast changes on a character besides its blocks."""
        vm = self.vm
        out = dict(charflags=vm.u32(obj + 0x110), invokedelay=s32(vm.u32(obj + O_INVOKEDELAY)))
        out.update(self.manager_dump(obj))
        return out

    # -- one case -----------------------------------------------------------
    def put_text(self, text):
        self.vm.write(self.text, text.encode('cp1252') + b'\0')
        return self.text

    def run(self, case):
        vm, w = self.vm, self.world
        vm.restore()
        w.reset()
        self.case = case
        g = case.get('globals', {})
        w.set_globals(g)
        vm.put_u32(G_EDITOR, int(g.get('editor', 0)))
        vm.put_u32(G_CHEAT, int(g.get('cheat', 0)))
        w.set_rng(case)
        self.specs = {}
        for spec in case['chars']:
            obj = w.new_character(spec)
            self.specs[spec['name']] = spec
            call(vm, POINTER_ARRAY_CTOR, (0x10, 0x10), this=obj + O_MANAGER)
            vm.put_u32(obj + O_MANAGER + 0x14, spec.get('spellwait', 0) & 0xffffffff)
            vm.put_u32(vm.u32(obj + 0xfc) + 0x1e0, spec.get('chardata', {}).get('mana', 0) & 0xffffffff)
        for spec in case['chars']:
            w.set_blocks(w.by_name[spec['name']], spec)
            self.spells.set_records(w.by_name[spec['name']], spec)
        if not case.get('mainplayer', True):
            vm.put_u32(0x667fcc, 0)
        w.seams.clear()
        me = w.by_name[case['self']]

        names = case.get('targets')
        targets = 0
        if names is not None:
            targets = vm.allocate(4 * max(1, len(names)))
            for i, n in enumerate(names):
                vm.put_u32(targets + 4 * i, w.by_name[n] if n else 0)
        numtargs = case.get('numtargs', len(names) if names is not None else 0)
        source = 0
        if case.get('sourcepos') is not None:
            source = vm.allocate(12)
            vm.write(source, struct.pack('<3i', *case['sourcepos']))
        text = self.put_text(case['text'])
        kind = case['call']
        if kind in ('cast-talismans', 'cast-name'):
            entry = CAST_BY_TALISMANS if kind == 'cast-talismans' else CAST_BY_NAME
            returned = s32(call(vm, entry, (text, targets, numtargs & 0xffffffff, source), this=me))
        elif kind == 'cast':
            returned = s32(call(vm, CAST, (text, source), this=me))
        elif kind in ('manager-cast-name', 'manager-cast-talismans'):
            entry = MANAGER_CAST_BY_NAME if kind == 'manager-cast-name' else MANAGER_CAST_BY_TALISMANS
            invoker = w.by_name[case['invoker']] if case.get('invoker') else 0
            returned = s32(call(vm, entry, (text, invoker, targets, numtargs & 0xffffffff, source, 0),
                                this=me + O_MANAGER))
        else:
            raise ValueError(f'unknown call {kind!r}')
        names = [spec['name'] for spec in case['chars']]
        return dict(schema=SCHEMA, side='retail', returned=returned,
                    chars={n: w.character_dump(w.by_name[n], []) for n in names},
                    casters={n: self.caster_dump(w.by_name[n]) for n in names},
                    seams=list(w.seams), draws=list(w.draws))


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable')
    parser.add_argument('--serve', action='store_true')
    parser.add_argument('--case', type=Path)
    args = parser.parse_args()
    fixture = CastFixture(args.executable)
    if args.serve:
        serve(fixture.sha, SCHEMA, fixture.setup_ms, fixture.run)
    elif args.case:
        print(json.dumps(fixture.run(json.loads(args.case.read_text())), indent=1))
    else:
        parser.error('--serve or --case')


if __name__ == '__main__':
    main()
