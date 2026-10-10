#!/usr/bin/env python3
"""Combat A/B, retail side, the melee katas (docs/gameplay/COMBAT_DOJO.md
C2-C5; forensics COMBAT_ATTACK_CHOICE.md, COMBAT_HIT.md).

One original TCharacter method per case on a fixture world (guest.py's
CombatWorld) whose characters carry their whole character data -- the
attack table and impacts as char.def gives them, by the combat-data dump's
field names (data_parse.py's layouts) -- and their attack bookkeeping
(`attackstate`). The case's `call`:

- `iva`: IsValidAttack `0x4d1120` (thiscall, 12 args, ret 0x30) once per
  entry of `calls` ({attack, tdist, button, pcnt, dmgpcnt, mask, flags,
  targ, damage, tohit, roll}: the out-values start as given, -1 by
  default), in order on the same world.
- `find-button` FindButtonAttack `0x4d1dd0` (button, dmgpcnt, isaction,
  targ), `find-pcnt` FindPcntAttack `0x4d1eb0` / `find-interactive`
  FindInteractiveAttack `0x4d1ff0` (pcnt, dmgpcnt): the out-values from
  `args` (-1 by default).
- `do-attack` DoAttack `0x4d2120` (attack, impact, damage, tohit, roll,
  targ).
- `button-attack` ButtonAttack `0x4d2480` / `button-action` ButtonAction
  `0x4d27f0` (button), `random-attack` RandomAttack `0x4d2900` (pcnt),
  `specific-attack` SpecificAttack `0x4d2a60` (attack).
- `resolve-attack` ResolveAttack `0x4c6dd0` (bits) on the doing block,
  `resolve-hit` ResolveHit `0x4c62b0` (attack, impact, targ, damage, tohit,
  roll), `on-attacked` OnAttacked `0x4cdce0` (attacker, victim, flag).
- `damage` Damage `0x4c4950` (damage, type, mod, block, attacker): `block`
  a block spec as the case's roles take them, its attack / impact the
  attacker's by default; run with the case's `seam_damage` false.
- `resolve-impact` ResolveImpact `0x4c74b0`, `resolve-block` ResolveBlock
  `0x4c77a0`, `resolve-dead` ResolveDead `0x4c7810` (bits) on the doing
  block; `block` Block `0x4d2e30` (frames; with `seam_block` false),
  `stop-block` StopBlock `0x4d30f0`, `dodge` Dodge `0x4d3150` (dir).

Runs as original code: the method and what it calls -- the searches,
IsValidAttack, DoAttack, HasActionAni, CombatAnimName `0x4ce1b0` and the
prefix `0x4cdf60`, CalculateDamage, GetDamageType, the stat slots
(Offense / Defense / Luck / StrengthMod / DamageMod / EdgeBonus /
AttackModifier / DefenseModifier / Resist / ArmorValue), the STATLEVEL
lookup over the case's tables, FaceAngleTo, GetSnapPos `0x46f010`,
Distance, SetDesired / ForceCommand, the TActionBlock ctor, the network
sends (no session: each returns at its first test).

Seams (guest.py's, and these), recorded in order:
- FindCharacters `0x4cd690`: the case's `found` (a name, or none).
- FindClearPath `0x4c39d0`: the case's `blocked`; when blocked, the
  blocker is the case's `blocker` (a name, or none).
- Block `0x4d2e30` (frames): recorded on the character, the case's
  `block_result` (1); the original runs with the case's `seam_block` false.
- EffectCombatFlash `0x4c8500`: recorded (it spawns an effect).
- The spell list's Find `0x53f010` and Cast `0x4d5c20`, as one record
  `CastByName` (spell, targets, source, result): a spell in the case's
  `spells` is cast with `cast_result` (1); any other isn't found (result 0).
- TPlayer WeaponType `0x520810` / WeaponDamage `0x520830`: the player's
  `weapon` {type, damage}.
- TPlayer SetPlayerState `0x51d680`: recorded, the value stored.

Rules (`rules` in the case, the combat-data dump's names): TOHITCENTER ..
TOHITDAMAGE at Rules `0x65d7a8` + `0x94` .. `0xcc`, the six STATLEVEL
tables behind `+0x58`. Globals: guest.py's, `nahkranoth` (`0x668108`),
`alreadydead` (`0x668104`), `nocombatresults` (`0x668194`).

Schema `combat.melee.v1`, shared with the port's `Revenant
--retail-ab=melee-*` (src/retailab_melee.cpp).
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from guest import AB, CombatWorld, call, flag_names, s32, serve, start  # noqa: E402
from data_parse import (A_IMPACTS, A_NUMIMPACTS, ATTACK_BODY, ATTACK_HEAD, ATTACK_MAGIC, C_ATTACKS,  # noqa: E402
                        C_IMPACTS, C_NUMIMPACTS, CA_MAGICATTACK, CHAR_FIELDS, IMPACT_FIELDS, IMPACT_SIZE)

SCHEMA = 'combat.melee.v1'
TEXT = 'cp1252'

IS_VALID_ATTACK = 0x4d1120
FIND_BUTTON, FIND_PCNT, FIND_INTERACTIVE = 0x4d1dd0, 0x4d1eb0, 0x4d1ff0
DO_ATTACK = 0x4d2120
BUTTON_ATTACK, BUTTON_ACTION, RANDOM_ATTACK, SPECIFIC_ATTACK = 0x4d2480, 0x4d27f0, 0x4d2900, 0x4d2a60
FIND_CHARACTERS, FIND_CLEAR_PATH, BLOCK = 0x4cd690, 0x4c39d0, 0x4d2e30
STOP_BLOCK, DODGE = 0x4d30f0, 0x4d3150
RESOLVE_IMPACT, RESOLVE_BLOCK, RESOLVE_DEAD = 0x4c74b0, 0x4c77a0, 0x4c7810
EFFECT_COMBAT_FLASH = 0x4c8500
SPELL_FIND, CAST = 0x53f010, 0x4d5c20
PLAYER_WEAPONTYPE, PLAYER_WEAPONDAMAGE, SET_PLAYER_STATE = 0x520810, 0x520830, 0x51d680
# Hit resolution (COMBAT_HIT.md): original ResolveAttack / ResolveHit /
# OnAttacked; seams for what lies past them.
RESOLVE_ATTACK, RESOLVE_HIT, ON_ATTACKED = 0x4c6dd0, 0x4c62b0, 0x4cdce0
IS_ENEMY, BEGIN_FIGHTING, EFFECT_BURST, DAMAGE = 0x4c89c0, 0x4d3b90, 0x4c85d0, 0x4c4950
SOUND_FIND = 0x49c430                            # the sound manager's lookup by name (thiscall, ret 4)
TEXT_FIND, TEXT_GET = 0x49d6d0, 0x49d800         # the dialog list: id of a tag / its line
TEXTBAR_PRINT = 0x54d170                         # cdecl (bar, fmt, ...)
EXP = {'kill': (0x51a630, 4), 'skill': (0x51abe0, 8), 'stealth': (0x51ad80, 4)}   # TPlayer slots 0x414/0x41c/0x420

# The functions under test, for coverage: [start, end).
COVERED = {'AnimPrefix': (0x4cdf60, 0x4ce1b0), 'CombatAnimName': (0x4ce1b0, 0x4ce290),
           'IsValidAttack': (0x4d1120, 0x4d1db8), 'FindButtonAttack': (0x4d1dd0, 0x4d1eab),
           'FindPcntAttack': (0x4d1eb0, 0x4d1feb), 'FindInteractiveAttack': (0x4d1ff0, 0x4d211f),
           'DoAttack': (0x4d2120, 0x4d2480), 'ButtonAttack': (0x4d2480, 0x4d27e3),
           'ButtonAction': (0x4d27f0, 0x4d28fb), 'RandomAttack': (0x4d2900, 0x4d2a53),
           'SpecificAttack': (0x4d2a60, 0x4d2bd9), 'ResolveHit': (0x4c62b0, 0x4c6dca),
           'ResolveAttack': (0x4c6dd0, 0x4c748c), 'OnAttacked': (0x4cdce0, 0x4cdef1),
           'Damage': (0x4c4950, 0x4c5810), 'ObjectDamage': (0x46e970, 0x46ea0d),
           'PlayerKilled': (0x518ed0, 0x518f8b), 'PlayerDied': (0x518f90, 0x518fda),
           'ResolveImpact': (0x4c74b0, 0x4c7798), 'ResolveBlock': (0x4c77a0, 0x4c7804),
           'ResolveDead': (0x4c7810, 0x4c7973), 'Block': (0x4d2e30, 0x4d30ec), 'StopBlock': (0x4d30f0, 0x4d3147),
           'Dodge': (0x4d3150, 0x4d3471)}

ATTACK_SIZE = 0x320
RULES = 0x65d7a8
RULES_TOHIT = [('tohitcenter', 0x94), ('tohitrangechar', 0x98), ('tohitrangeplyr', 0x9c),
               ('tohitblock', 0xa0), ('tohitface', 0xa4)]
R_TOHITDAMAGE, R_STATLEVELS = 0xa8, 0x58
G_NAHKRANOTH, G_ALREADYDEAD, G_NOCOMBATRESULTS, G_PLAYER = 0x668108, 0x668104, 0x668194, 0x667fcc

# TCharacter attack bookkeeping (COMBAT_ATTACK_CHOICE.md §2.4). Defaults as
# ClearChar 0x4c18a0 leaves them (the port's ClearChar is the same).
ATTACKSTATE = [('nextattack', 0x120, 1), ('magictimer', 0x124, 1), ('requestbits', 0x128, 0),
               ('attackcount', 0x12c, 0), ('lastattackticks', 0x164, 0), ('lasthit', 0x168, 0),
               ('chainhits', 0x16c, 0), ('lastbutton', 0x28c, -1), ('buttonrepeat', 0x290, 0),
               ('flashticks', 0x224, 0), ('autocombat', 0xe8, 1), ('movevert', 0xb8, None),
               ('snapticks', 0x220, -1), ('charflags', 0x110, None)]
O_LASTATTACK = 0x160
O_FLAGS, OF_ICED_PARALIZE = 0x8, 0x2800000
P_FRAGS = 0x650                                 # TPlayer kills / deaths, 4 ints
# TPlayer's modified stat copy (PLAYER_STATS.md §1): count +0x34c (short),
# {id, value} pairs at +0x350. Resist reads entry type + 6.
P_MODCOUNT, P_MODSTATS, RESIST_FIRST = 0x34c, 0x350, 6


class MeleeFixture:
    def __init__(self, executable):
        started = time.perf_counter()
        self.vm, self.sha = start(executable)
        self.world = CombatWorld(self.vm)
        b = self.world.boundaries
        b.add(FIND_CHARACTERS, 'FindCharacters', 0x18, self._find_characters)
        b.add(FIND_CLEAR_PATH, 'FindClearPath', 0x14, self._find_clear_path)
        b.add(EFFECT_COMBAT_FLASH, 'EffectCombatFlash', 0, self._combat_flash)
        b.add(SPELL_FIND, 'SpellFind', 4, self._spell_find)
        b.add(CAST, 'Cast', 0x10, self._cast)
        b.add(PLAYER_WEAPONTYPE, 'WeaponType', 0, lambda a, c: self._weapon(c, 'type', 'WeaponType'))
        b.add(PLAYER_WEAPONDAMAGE, 'WeaponDamage', 0, lambda a, c: self._weapon(c, 'damage', 'WeaponDamage'))
        b.add(SET_PLAYER_STATE, 'SetPlayerState', 4, self._set_player_state)
        b.add(IS_ENEMY, 'IsEnemy', 4, self._is_enemy)
        b.add(BEGIN_FIGHTING, 'BeginFighting', 8, self._begin_fighting)
        b.add(EFFECT_BURST, 'EffectBurst', 8, self._effect_burst)
        b.add(SOUND_FIND, 'SoundFind', 4, self._sound_find)
        b.add(TEXT_FIND, 'TextFind', 4, lambda a, c: -1)
        b.add(TEXT_GET, 'TextGet', 4, self._text_get)
        b.add(TEXTBAR_PRINT, 'TextBar', 0, self._textbar)
        for kind, (address, pop) in EXP.items():
            b.add(address, f'Exp{kind}', pop, lambda a, c, kind=kind: self._exp(kind, a, c))
        self._conditional(DAMAGE, 0x14, self._damage, lambda: self.case.get('seam_damage', True))
        self._conditional(BLOCK, 4, self._block, lambda: self.case.get('seam_block', True))
        self._emulator_fixes()
        self._coverage()
        self.vm.checkpoint()
        self.setup_ms = (time.perf_counter() - started) * 1000
        self.case = {}

    def _coverage(self):
        """With MELEE_COVERAGE=<dir>: the code reached in the melee functions
        (each block run, as [start, size]), written to <dir>/<pid>.json when the process ends
        (tools/retail_ab/melee_coverage.py reads them)."""
        import atexit
        import os
        out = os.environ.get('MELEE_COVERAGE')
        if not out:
            return
        from unicorn import UC_HOOK_BLOCK
        reached = set()
        lo, hi = min(r[0] for r in COVERED.values()), max(r[1] for r in COVERED.values())
        self.vm.uc.hook_add(UC_HOOK_BLOCK, lambda uc, address, size, user: reached.add((address, size)),
                            begin=lo, end=hi)

        def write():
            Path(out).mkdir(parents=True, exist_ok=True)
            (Path(out) / f'{os.getpid()}.json').write_text(json.dumps(sorted(reached)))
        atexit.register(write)

    # -- seams --------------------------------------------------------------
    def _obj(self, name):
        return self.world.by_name[name] if name else 0

    def _find_characters(self, args, ecx):
        found = self._obj(self.case.get('found'))
        query = dict(max=s32(args[1]), range=s32(args[2]), angle=s32(args[3]),
                     anglerange=s32(args[4]), flags=s32(args[5]))
        n = 1 if found and s32(args[1]) > 0 else 0
        if s32(args[1]) > 0:
            self.vm.put_u32(args[0], found)
        self.world.seams.append(dict(seam='FindCharacters', who=self.world._name(ecx), **query, result=n))
        return n

    def _find_clear_path(self, args, ecx):
        point = list(struct.unpack('<3i', self.vm.uc.mem_read(args[1], 12)))
        blocked = int(bool(self.case.get('blocked', False)))
        blocker = self.case.get('blocker') if blocked else None
        if args[4]:
            self.vm.put_u32(args[4], self._obj(blocker))
        self.world.seams.append(dict(seam='FindClearPath', who=self.world._name(ecx), to=point,
                                     blocker=blocker, result=blocked))
        return blocked

    def _block(self, args, ecx):
        result = int(self.case.get('block_result', 1))
        self.world.seams.append(dict(seam='Block', who=self.world._name(ecx), frames=s32(args[0]), result=result))
        return result

    def _combat_flash(self, args, ecx):
        self.world.seams.append(dict(seam='EffectCombatFlash', who=self.world._name(ecx)))
        return 0

    def _spell_find(self, args, ecx):
        name = self.vm.string(args[0])
        if name.lower() not in [s.lower() for s in self.case.get('spells', [])]:
            self.world.seams.append(dict(seam='CastByName', spell=name, result=0))
            return 0
        spell = self.vm.allocate(0x80)
        self.vm.write(spell + 0x24, name.encode(TEXT) + b'\0')
        return spell

    def _cast(self, args, ecx):
        name = self.vm.string(args[0])
        n = s32(args[2])
        targets = [self.world._name(self.vm.u32(args[1] + 4 * i)) for i in range(max(0, n))]
        source = list(struct.unpack('<3i', self.vm.uc.mem_read(args[3], 12))) if args[3] else None
        result = int(self.case.get('cast_result', 1))
        self.world.seams.append(dict(seam='CastByName', spell=name, targets=targets, source=source, result=result))
        return result

    def _weapon(self, ecx, key, seam):
        spec = self.specs[self.world._name(ecx)]
        if 'weapon' not in spec:
            raise KeyError(f"{spec['name']}: a player needs `weapon` in the case")
        value = spec['weapon'][key]
        self.world.seams.append(dict(seam=seam, who=spec['name'], result=value))
        return value

    def _set_player_state(self, args, ecx):
        self.vm.put_u32(ecx + 0x36c, args[0])
        self.world.seams.append(dict(seam='SetPlayerState', who=self.world._name(ecx), value=args[0]))
        return 0

    def _is_enemy(self, args, ecx):
        """`friends`: [who, other] pairs where who doesn't count other an enemy."""
        who, other = self.world._name(ecx), self.world._name(args[0])
        result = 0 if [who, other] in self.case.get('friends', []) else 1
        self.world.seams.append(dict(seam='IsEnemy', who=who, other=other, result=result))
        return result

    def _begin_fighting(self, args, ecx):
        self.world.seams.append(dict(seam='BeginFighting', who=self.world._name(ecx),
                                     target=self.world._name(args[0]) if args[0] else None, action=s32(args[1]),
                                     result=1))
        return 1

    def _effect_burst(self, args, ecx):
        self.world.seams.append(dict(seam='EffectBurst', who=self.world._name(ecx), name=self.vm.string(args[0]),
                                     height=s32(args[1])))
        return 0

    def _sound_find(self, args, ecx):
        """No sound by that name: nothing plays."""
        self.world.seams.append(dict(seam='Sound', name=self.vm.string(args[0])))
        return -1

    def _text_get(self, args, ecx):
        """The dialog list is empty: a tag's line is "[tag]", as the port's
        unloaded list answers."""
        line = '[' + self.vm.string(args[0]) + ']'
        address = self.vm.allocate(len(line) + 1)
        self.vm.write(address, line.encode(TEXT) + b'\0')
        return address

    def _textbar(self, args, ecx):
        self.world.seams.append(dict(seam='TextBar', text=self.vm.string(args[1])))
        return 0

    def _exp(self, kind, args, ecx):
        skill = s32(args[0]) if kind == 'skill' else -1
        victim = args[1] if kind == 'skill' else args[0]
        self.world.seams.append(dict(seam='Exp', who=self.world._name(ecx), kind=kind, skill=skill,
                                     victim=self.world._name(victim)))
        return 0

    def _damage(self, args, ecx):
        """Damage (the C2 kata runs it): recorded, the hit block by meaning."""
        ab = args[3]
        block = None
        if ab:
            vm = self.vm
            block = dict(action=s32(vm.u32(ab + AB['action'])), name=vm.string(ab + AB['name']),
                         wait=s32(vm.u32(ab + AB['wait'])), damage=s32(vm.u32(ab + AB['damage'])),
                         obj=self.world._name(vm.u32(ab + AB['obj'])) if vm.u32(ab + AB['obj']) else None,
                         attack=self._attack_name(vm.u32(ab + AB['attack'])), impact=self._impact_index(ab),
                         flags=flag_names(vm.u32(ab + AB['flags'])))
        self.world.seams.append(dict(seam='Damage', who=self.world._name(ecx), damage=s32(args[0]),
                                     type=s32(args[1]), mod=s32(args[2]), block=block,
                                     attacker=self.world._name(args[4]) if args[4] else None))
        return 0

    def _emulator_fixes(self):
        """Where the emulator runs retail's code wrong. ResolveAttack's block
        sound switch (0x4c727c) is `cmp eax, 6; rep movsb; ja default`: the
        emulator loses the cmp's flags across the rep movsb (seen with the
        attacker's weapon type 0, eax -1, the ja not taken and the table
        read out of range). The ja is taken here as the hardware takes it."""
        from unicorn import UC_HOOK_CODE
        from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_EIP

        def ja(uc, at, size, user):
            if uc.reg_read(UC_X86_REG_EAX) > 6:
                uc.reg_write(UC_X86_REG_EIP, 0x4c731e)
        self.vm.uc.hook_add(UC_HOOK_CODE, ja, begin=0x4c7281, end=0x4c7281)

    def _conditional(self, address, pop, handler, active):
        """A seam only while active() holds; otherwise the original runs."""
        from unicorn import UC_HOOK_CODE
        from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EIP, UC_X86_REG_ESP
        vm = self.vm

        def enter(uc, at, size, user):
            if not active():
                return
            sp = uc.reg_read(UC_X86_REG_ESP)
            args = struct.unpack('<8I', uc.mem_read(sp + 4, 32))
            try:
                result = handler(args, uc.reg_read(UC_X86_REG_ECX))
            except Exception as error:
                vm.error = error
                uc.emu_stop()
                return
            uc.reg_write(UC_X86_REG_EAX, (result or 0) & 0xffffffff)
            uc.reg_write(UC_X86_REG_EIP, vm.u32(sp))
            uc.reg_write(UC_X86_REG_ESP, sp + 4 + pop)
        vm.uc.hook_add(UC_HOOK_CODE, enter, begin=address, end=address)

    # -- the world ------------------------------------------------------------
    def _text(self, address, size, text):
        raw = text.encode(TEXT)[:size - 1]
        self.vm.write(address, raw + bytes(size - len(raw)))

    def _fields(self, base, fields, values):
        vm = self.vm
        for name, kind in fields:
            if name not in values:
                continue
            v = values[name]
            if isinstance(kind, int):
                vm.put_u32(base + kind, int(v) & 0xffffffff)
            elif kind[0] == 's':
                self._text(base + kind[1], kind[2], v)
            elif kind[0] == 'b':
                vm.write(base + kind[1], bytes([int(v) & 0xff]))
            else:
                for i, x in enumerate(v[:kind[2]]):
                    vm.put_u32(base + kind[1] + 4 * i, int(x) & 0xffffffff)

    def _impact(self, address, imp):
        self._text(address, 32, imp.get('name', ''))
        self._fields(address, IMPACT_FIELDS, imp)

    def _chardata(self, obj, spec):
        """The case's whole character data over what CombatWorld wrote:
        every field the dump names, the CHARIMPACTs, the attack table (a
        pointer array: count +0xcc, items +0xdc, the empty record +0xe0)."""
        vm = self.vm
        cd = vm.u32(obj + 0xfc)
        data = spec.get('chardata', {})
        self._fields(cd, CHAR_FIELDS, data)
        if 'impacts' in data:
            vm.put_u32(cd + C_NUMIMPACTS, len(data['impacts']))
            for i, imp in enumerate(data['impacts']):
                self._impact(cd + C_IMPACTS + IMPACT_SIZE * i, imp)
        attacks = data.get('attacks', [])
        items = vm.allocate(4 * max(1, len(attacks)))
        empty = vm.allocate(ATTACK_SIZE + 0x60)
        vm.put_u32(cd + C_ATTACKS, len(attacks))
        vm.put_u32(cd + C_ATTACKS + 0x10, items)
        vm.put_u32(cd + C_ATTACKS + 0x14, empty)
        for i, ad in enumerate(attacks):
            # A little zeroed room after the record: IsValidAttack reads the
            # impact one past the last (0x4d1d35).
            rec = vm.allocate(ATTACK_SIZE + 0x60)
            vm.put_u32(items + 4 * i, rec)
            self._text(rec, 32, ad['name'])
            self._fields(rec, ATTACK_HEAD, ad)
            if ad['flags'] & CA_MAGICATTACK:
                self._fields(rec, ATTACK_MAGIC, ad)
            else:
                self._fields(rec, ATTACK_BODY, ad)
                vm.put_u32(rec + A_NUMIMPACTS, len(ad.get('impacts', [])))
                for k, imp in enumerate(ad.get('impacts', [])):
                    self._impact(rec + A_IMPACTS + IMPACT_SIZE * k, imp)
            self.records[rec] = (spec['name'], i)

    def _attackstate(self, obj, spec):
        vm = self.vm
        state = spec.get('attackstate', {})
        for name, off, default in ATTACKSTATE:
            if default is not None:             # (None: dumped, set elsewhere)
                vm.put_u32(obj + off, int(state.get(name, default)) & 0xffffffff)
        last = state.get('lastattack')
        cd = vm.u32(obj + 0xfc)
        vm.put_u32(obj + O_LASTATTACK, vm.u32(vm.u32(cd + C_ATTACKS + 0x10) + 4 * last) if last is not None else 0)

    def _apply_block_fields(self, ab, obj, b):
        """What a block of the case carries beyond the shared spec: its
        attack (an index into the table of `obj`, or of the character
        `attack_of` names), impact (of that attack, or of attack
        `impact_attack` of `impact_of`), damage, to-hit and roll."""
        vm = self.vm

        def record(owner, index):
            ocd = vm.u32((self.world.by_name[owner] if owner else obj) + 0xfc)
            return vm.u32(vm.u32(ocd + C_ATTACKS + 0x10) + 4 * index)
        if 'attack' in b:
            vm.put_u32(ab + AB['attack'], record(b.get('attack_of'), b['attack']))
        if 'impact' in b:
            rec = record(b.get('impact_of', b.get('attack_of')), b.get('impact_attack', b.get('attack')))
            vm.put_u32(ab + AB['impact'], rec + A_IMPACTS + IMPACT_SIZE * b['impact'])
        for key, off2 in (('damage', AB['damage']), ('tohit', 0x54), ('roll', 0x58)):
            if key in b:
                vm.put_u32(ab + off2, b[key] & 0xffffffff)

    def _block_fields(self, obj, spec):
        for role, off in (('root', 0xe0), ('doing', 0xd8), ('desired', 0xdc)):
            if isinstance(spec.get(role), dict):
                self._apply_block_fields(self.vm.u32(obj + off), obj, spec[role])

    def _resists(self, obj, spec):
        """A player's modified stat copy, as far as Resist reads it."""
        vm = self.vm
        resists = spec.get('resists', [0] * 10)
        count = RESIST_FIRST + 10
        pairs = vm.allocate(8 * count)
        for i in range(count):
            value = resists[i - RESIST_FIRST] if i >= RESIST_FIRST else 0
            vm.write(pairs + 8 * i, struct.pack('<ii', i, value))
        vm.write(obj + P_MODCOUNT, struct.pack('<h', count))
        vm.put_u32(obj + P_MODSTATS, pairs)

    def _rules(self, rules):
        vm = self.vm
        for name, off in RULES_TOHIT:
            vm.put_u32(RULES + off, int(rules.get(name, 0)) & 0xffffffff)
        for i, (key, pct) in enumerate(rules.get('tohitdamage', [[0, 0]] * 5)):
            vm.write(RULES + R_TOHITDAMAGE + 8 * i, struct.pack('<ii', key, pct))
        tables = rules.get('statlevels')
        if tables:
            ptrs = vm.allocate(4 * len(tables))
            for t, row in enumerate(tables):
                table = vm.allocate(4 * len(row))
                vm.write(table, struct.pack(f'<{len(row)}i', *row))
                vm.put_u32(ptrs + 4 * t, table)
            vm.put_u32(RULES + R_STATLEVELS, ptrs)
        else:
            vm.put_u32(RULES + R_STATLEVELS, 0)

    # -- dumps ------------------------------------------------------------------
    def _attack_name(self, rec):
        if not rec:
            return None
        owner = self.records.get(rec)
        return f'{owner[0]}#{owner[1]}' if owner else f'{rec:#x}'

    def _impact_index(self, ab):
        """An attack's impact as "<owner>#<attack>/<impact>", a CHARIMPACT as
        "<owner>/<impact>"; the slot after a full list (six) "past-end"."""
        imp = self.vm.u32(ab + AB['impact'])
        if not imp:
            return None
        for rec, (owner, i) in self.records.items():
            k, r = divmod(imp - rec - A_IMPACTS, IMPACT_SIZE)
            if r == 0 and 0 <= k <= 6:
                return 'past-end' if k == 6 else f'{owner}#{i}/{k}'
        for name, obj in self.world.by_name.items():
            k, r = divmod(imp - self.vm.u32(obj + 0xfc) - C_IMPACTS, IMPACT_SIZE)
            if r == 0 and 0 <= k <= 6:
                return 'past-end' if k == 6 else f'{name}/{k}'
        return f'{imp:#x}'

    def _block_extra(self, ab, out):
        if out is None:
            return out
        vm = self.vm
        out.update(attack=self._attack_name(vm.u32(ab + AB['attack'])), impact=self._impact_index(ab),
                   damage=s32(vm.u32(ab + AB['damage'])), tohit=s32(vm.u32(ab + 0x54)),
                   roll=s32(vm.u32(ab + 0x58)))
        return out

    def _dump(self, obj, new_blocks):
        vm, world = self.vm, self.world
        out = world.character_dump(obj, new_blocks)
        for role, off in (('root', 0xe0), ('doing', 0xd8), ('desired', 0xdc)):
            self._block_extra(vm.u32(obj + off), out[role])
        state = {name: s32(vm.u32(obj + off)) for name, off, _ in ATTACKSTATE}
        state['objflags'] = vm.u32(obj + O_FLAGS) & OF_ICED_PARALIZE
        owner = self.records.get(vm.u32(obj + O_LASTATTACK))
        state['lastattack'] = owner[1] if owner and owner[0] == world._name(obj) else None
        if self.specs[world._name(obj)].get('class', 12) == 11:
            state['frags'] = [s32(vm.u32(obj + P_FRAGS + 4 * i)) for i in range(4)]
        out['attackstate'] = state
        out['motion'] = world.motion_dump(obj)
        return out

    # -- a case -------------------------------------------------------------------
    def run(self, case):
        vm, world = self.vm, self.world
        vm.restore()
        world.reset()
        self.case = case
        self.records = {}
        self.specs = {c['name']: c for c in case['chars']}
        world.set_globals(case.get('globals', {}))
        vm.put_u32(G_NAHKRANOTH, int(case.get('globals', {}).get('nahkranoth', 0)))
        vm.put_u32(G_ALREADYDEAD, int(case.get('globals', {}).get('alreadydead', 0)))
        vm.put_u32(G_NOCOMBATRESULTS, int(case.get('globals', {}).get('nocombatresults', 0)))
        vm.put_u32(G_PLAYER, 0)
        self._rules(case.get('rules', {}))
        world.set_rng(case)
        for spec in case['chars']:
            obj = world.new_character(spec)
            self._chardata(obj, spec)
            if spec.get('class', 12) == 11:
                self._resists(obj, spec)
        for spec in case['chars']:
            obj = world.by_name[spec['name']]
            world.set_blocks(obj, spec)
            self._block_fields(obj, spec)
            self._attackstate(obj, spec)
        world.seams.clear()
        me = world.by_name[case['self']]
        kind = case['call']
        a = case.get('args', {})
        outs = vm.allocate(32)
        result = dict(schema=SCHEMA, side='retail')

        def put_outs(src):
            for i, key in enumerate(('attack', 'impact', 'damage', 'tohit', 'roll')):
                vm.put_u32(outs + 4 * i, int(src.get(key, -1)) & 0xffffffff)

        def read_outs():
            return {key: s32(vm.u32(outs + 4 * i)) for i, key in enumerate(('attack', 'impact', 'damage', 'tohit', 'roll'))}

        o = [outs + 4 * i for i in range(5)]         # attack, impact, damage, tohit, roll
        if kind == 'iva':
            results = []
            for c in case['calls']:
                put_outs(c)
                r = s32(call(vm, IS_VALID_ATTACK,
                             (c['attack'], o[1], o[2], o[3], o[4], c.get('tdist', 10000) & 0xffffffff,
                              c.get('button', -1) & 0xffffffff, c.get('pcnt', -1) & 0xffffffff,
                              c.get('dmgpcnt', 0) & 0xffffffff, c.get('mask', 0) & 0xffffffff,
                              c.get('flags', 0) & 0xffffffff, self._obj(c.get('targ'))), this=me))
                got = read_outs()
                del got['attack']
                results.append(dict(returned=r, **got))
            result['calls'] = results
        elif kind in ('find-button', 'find-pcnt', 'find-interactive'):
            put_outs(a)
            if kind == 'find-button':
                r = call(vm, FIND_BUTTON, (a['button'] & 0xffffffff, a.get('dmgpcnt', 0) & 0xffffffff,
                                           o[0], o[1], o[2], o[3], o[4], int(a.get('isaction', 0)),
                                           self._obj(a.get('targ'))), this=me)
            else:
                r = call(vm, FIND_PCNT if kind == 'find-pcnt' else FIND_INTERACTIVE,
                         (a['pcnt'] & 0xffffffff, a.get('dmgpcnt', 0) & 0xffffffff, o[0], o[1], o[2], o[3], o[4]),
                         this=me)
            result['returned'] = s32(r)
            result['outs'] = read_outs()
        elif kind == 'do-attack':
            result['returned'] = s32(call(vm, DO_ATTACK, (
                a['attack'] & 0xffffffff, a.get('impact', -1) & 0xffffffff, a.get('damage', 0) & 0xffffffff,
                a.get('tohit', 0) & 0xffffffff, a.get('roll', 0) & 0xffffffff, self._obj(a.get('targ'))), this=me))
        elif kind in ('resolve-attack', 'resolve-impact', 'resolve-block', 'resolve-dead'):
            entry = {'resolve-attack': RESOLVE_ATTACK, 'resolve-impact': RESOLVE_IMPACT,
                     'resolve-block': RESOLVE_BLOCK, 'resolve-dead': RESOLVE_DEAD}[kind]
            result['returned'] = s32(call(vm, entry, (vm.u32(me + 0xd8), a.get('bits', 0) & 0xffffffff), this=me))
        elif kind == 'block':
            result['returned'] = s32(call(vm, BLOCK, (a.get('frames', -1) & 0xffffffff,), this=me))
        elif kind == 'stop-block':
            result['returned'] = s32(call(vm, STOP_BLOCK, (), this=me))
        elif kind == 'dodge':
            result['returned'] = s32(call(vm, DODGE, (a.get('dir', -1) & 0xffffffff,), this=me))
        elif kind == 'resolve-hit':
            cd = vm.u32(me + 0xfc)
            rec = vm.u32(vm.u32(cd + C_ATTACKS + 0x10) + 4 * a['attack'])
            imp = rec + A_IMPACTS + IMPACT_SIZE * a['impact'] if a.get('impact', -1) >= 0 else 0
            result['returned'] = s32(call(vm, RESOLVE_HIT, (
                self._obj(a.get('targ')), rec, imp, a['damage'] & 0xffffffff, a['tohit'] & 0xffffffff,
                a['roll'] & 0xffffffff), this=me))
        elif kind == 'on-attacked':
            call(vm, ON_ATTACKED, (self._obj(a.get('attacker')), self._obj(a.get('victim')),
                                   a.get('flag', 0) & 0xffffffff), this=me)
            result['returned'] = 0
        elif kind in ('button-attack', 'button-action', 'random-attack', 'specific-attack'):
            entry = {'button-attack': BUTTON_ATTACK, 'button-action': BUTTON_ACTION,
                     'random-attack': RANDOM_ATTACK, 'specific-attack': SPECIFIC_ATTACK}[kind]
            arg = a['pcnt'] if kind == 'random-attack' else a['attack'] if kind == 'specific-attack' else a['button']
            result['returned'] = s32(call(vm, entry, (arg & 0xffffffff,), this=me))
        elif kind == 'damage':
            ab = 0
            attacker = self._obj(a.get('attacker'))
            if isinstance(a.get('block'), dict):
                ab = world.new_block(a['block'], world.by_name)
                self._apply_block_fields(ab, attacker, a['block'])
            call(vm, DAMAGE, (a['damage'] & 0xffffffff, a.get('type', -1) & 0xffffffff, a.get('mod', 0) & 0xffffffff,
                              ab, attacker), this=me)
            result['returned'] = 0
        else:
            raise ValueError(f'unknown call {kind!r}')
        # A block made in the call is `new N` per character, as the port numbers them.
        result['chars'] = {spec['name']: self._dump(world.by_name[spec['name']], []) for spec in case['chars']}
        result['seams'] = list(world.seams)
        result['draws'] = list(world.draws)
        return result


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable')
    parser.add_argument('--serve', action='store_true')
    parser.add_argument('--case', type=Path)
    args = parser.parse_args()
    fixture = MeleeFixture(args.executable)
    if args.serve:
        serve(fixture.sha, SCHEMA, fixture.setup_ms, fixture.run)
    elif args.case:
        print(json.dumps(fixture.run(json.loads(args.case.read_text())), indent=1))


if __name__ == '__main__':
    main()
