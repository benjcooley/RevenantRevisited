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
  `block_result` (1).
- The spell list's Find `0x53f010` and Cast `0x4d5c20`, as one record
  `CastByName` (spell, targets, source, result): a spell in the case's
  `spells` is cast with `cast_result` (1); any other isn't found (result 0).
- TPlayer WeaponType `0x520810` / WeaponDamage `0x520830`: the player's
  `weapon` {type, damage}.
- TPlayer SetPlayerState `0x51d680`: recorded, the value stored.

Rules (`rules` in the case, the combat-data dump's names): TOHITCENTER ..
TOHITDAMAGE at Rules `0x65d7a8` + `0x94` .. `0xcc`, the six STATLEVEL
tables behind `+0x58`. Globals: guest.py's, and `nahkranoth` (`0x668108`).

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
from guest import AB, CombatWorld, call, s32, serve, start  # noqa: E402
from data_parse import (A_IMPACTS, A_NUMIMPACTS, ATTACK_BODY, ATTACK_HEAD, ATTACK_MAGIC, C_ATTACKS,  # noqa: E402
                        C_IMPACTS, C_NUMIMPACTS, CA_MAGICATTACK, CHAR_FIELDS, IMPACT_FIELDS, IMPACT_SIZE)

SCHEMA = 'combat.melee.v1'
TEXT = 'cp1252'

IS_VALID_ATTACK = 0x4d1120
FIND_BUTTON, FIND_PCNT, FIND_INTERACTIVE = 0x4d1dd0, 0x4d1eb0, 0x4d1ff0
DO_ATTACK = 0x4d2120
BUTTON_ATTACK, BUTTON_ACTION, RANDOM_ATTACK, SPECIFIC_ATTACK = 0x4d2480, 0x4d27f0, 0x4d2900, 0x4d2a60
FIND_CHARACTERS, FIND_CLEAR_PATH, BLOCK = 0x4cd690, 0x4c39d0, 0x4d2e30
SPELL_FIND, CAST = 0x53f010, 0x4d5c20
PLAYER_WEAPONTYPE, PLAYER_WEAPONDAMAGE, SET_PLAYER_STATE = 0x520810, 0x520830, 0x51d680

# The functions under test, for coverage: [start, end).
COVERED = {'AnimPrefix': (0x4cdf60, 0x4ce1b0), 'CombatAnimName': (0x4ce1b0, 0x4ce290),
           'IsValidAttack': (0x4d1120, 0x4d1db8), 'FindButtonAttack': (0x4d1dd0, 0x4d1eab),
           'FindPcntAttack': (0x4d1eb0, 0x4d1feb), 'FindInteractiveAttack': (0x4d1ff0, 0x4d211f),
           'DoAttack': (0x4d2120, 0x4d2480), 'ButtonAttack': (0x4d2480, 0x4d27e3),
           'ButtonAction': (0x4d27f0, 0x4d28fb), 'RandomAttack': (0x4d2900, 0x4d2a53),
           'SpecificAttack': (0x4d2a60, 0x4d2bd9)}

ATTACK_SIZE = 0x320
RULES = 0x65d7a8
RULES_TOHIT = [('tohitcenter', 0x94), ('tohitrangechar', 0x98), ('tohitrangeplyr', 0x9c),
               ('tohitblock', 0xa0), ('tohitface', 0xa4)]
R_TOHITDAMAGE, R_STATLEVELS = 0xa8, 0x58
G_NAHKRANOTH = 0x668108

# TCharacter attack bookkeeping (COMBAT_ATTACK_CHOICE.md §2.4). Defaults as
# ClearChar 0x4c18a0 leaves them (the port's ClearChar is the same).
ATTACKSTATE = [('nextattack', 0x120, 1), ('magictimer', 0x124, 1), ('requestbits', 0x128, 0),
               ('attackcount', 0x12c, 0), ('lastattackticks', 0x164, 0), ('lasthit', 0x168, 0),
               ('chainhits', 0x16c, 0), ('lastbutton', 0x28c, -1), ('buttonrepeat', 0x290, 0)]
O_LASTATTACK = 0x160
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
        b.add(BLOCK, 'Block', 4, self._block)
        b.add(SPELL_FIND, 'SpellFind', 4, self._spell_find)
        b.add(CAST, 'Cast', 0x10, self._cast)
        b.add(PLAYER_WEAPONTYPE, 'WeaponType', 0, lambda a, c: self._weapon(c, 'type', 'WeaponType'))
        b.add(PLAYER_WEAPONDAMAGE, 'WeaponDamage', 0, lambda a, c: self._weapon(c, 'damage', 'WeaponDamage'))
        b.add(SET_PLAYER_STATE, 'SetPlayerState', 4, self._set_player_state)
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
            vm.put_u32(obj + off, int(state.get(name, default)) & 0xffffffff)
        last = state.get('lastattack')
        cd = vm.u32(obj + 0xfc)
        vm.put_u32(obj + O_LASTATTACK, vm.u32(vm.u32(cd + C_ATTACKS + 0x10) + 4 * last) if last is not None else 0)

    def _block_fields(self, obj, spec):
        """What a block of the case carries beyond the shared spec: its
        attack (an index into the owner's table), impact (an index into that
        attack's impacts), damage, to-hit and roll."""
        vm = self.vm
        cd = vm.u32(obj + 0xfc)
        for role, off in (('root', 0xe0), ('doing', 0xd8), ('desired', 0xdc)):
            b = spec.get(role)
            if not isinstance(b, dict):
                continue
            ab = vm.u32(obj + off)

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
        """An attack's impact as "<owner>#<attack>/<impact>"."""
        imp = self.vm.u32(ab + AB['impact'])
        if not imp:
            return None
        for rec, (owner, i) in self.records.items():
            if rec + A_IMPACTS <= imp < rec + A_IMPACTS + 6 * IMPACT_SIZE:
                return f'{owner}#{i}/{(imp - rec - A_IMPACTS) // IMPACT_SIZE}'
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
        owner = self.records.get(vm.u32(obj + O_LASTATTACK))
        state['lastattack'] = owner[1] if owner and owner[0] == world._name(obj) else None
        out['attackstate'] = state
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
        elif kind in ('button-attack', 'button-action', 'random-attack', 'specific-attack'):
            entry = {'button-attack': BUTTON_ATTACK, 'button-action': BUTTON_ACTION,
                     'random-attack': RANDOM_ATTACK, 'specific-attack': SPECIFIC_ATTACK}[kind]
            arg = a['pcnt'] if kind == 'random-attack' else a['attack'] if kind == 'specific-attack' else a['button']
            result['returned'] = s32(call(vm, entry, (arg & 0xffffffff,), this=me))
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
