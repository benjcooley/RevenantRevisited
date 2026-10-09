#!/usr/bin/env python3
"""Combat A/B, retail side, kata D1: the combat data as retail parses it.

Runs TRules::Initialize `0x48b690` (ecx = Rules `0x65d7a8`) as original
code: the defaults it sets, Load `0x48b990` over rules.def, stats.def,
char.def, weapon.def, armor.def and equip.def (each block through the
original loaders: SClassData `0x4891c0`, SCharData `0x489850`, the weapon
and armor blocks `0x48ac30` / `0x48b1a0`, STATLEVEL `0x49c9f0`), and the
experience tables it builds after. Then BindTypes `0x48cab0`, the late
name -> object type binding retail runs at the end of LoadClasses
(`0x47654f`), over a stand-in class table. Dumps every parsed record by
field name (docs/gameplay/forensics/COMBAT_DATA.md §3-§8). Schema
`combat.data.v1`, shared with the port's `Revenant --retail-ab=combat-data`.

What runs as original code: TRules' constructor `0x488160` (once, before
the checkpoint: static init isn't run), Initialize, Load, every block
loader, the tokenizer (`TToken::Open` `0x4789c0` -> `SetFile` `0x4788d0`,
with its XOR-0xCC unscramble), Parse `0x47a410`, the pointer arrays,
malloc/free and the CRT string functions, BindTypes / BindType `0x48c930`
/ FindObjType `0x475210`.

Seams (answered from the case, recorded in `seams`):
- zfopen_rel `0x4a13f0` (path, mode, looseFirst): a handle onto the case's
  bytes for that path (`.\\Resources\\rules.def` -> `resources/rules.def`),
  0 when the case has no such file. Length `0x4a17b0`, read `0x4a15a0`
  (fread) and close `0x4a1540` on that handle.
- FileExists `0x4a1c00` (path, isdir): whether the case has the file.
- FatalError `0x481c10` / `0x481d10` (fmt, arg): the message, and the case
  stops there (`fatal` in the result).
- The weapon / armor halves of BindTypeData (`0x48af30` / `0x48b4a0`, which
  copy an item's numbers into its object class's stats): recorded, not run;
  the stand-in classes have no stats.
- FindObjType `0x475210` (name, partial) on a stand-in class: the first type
  of that name, case-blind (`_stricmp`; partial is never set here), else -1.
  The original is a linear `_stricmp` scan; BindTypes asks it for every type
  of every class (~3,000, 2,177 of them tiles) against every class in turn,
  ~40 s of emulation, so the host answers it from the same table.

The stand-in class table (`classes` in the case: class.def's classes by id,
each with its type names in file order) fills the class registry
`0x65a148` / count `0x65a258` with objects laid out as BindTypes reads them:
id `+0x08`, type array `+0x24` (count) / `+0x34` (data), each type a record
whose `+0x00` is its name.

A case: {"files": {"Resources/rules.def": path, ...}, "gamespeed": 3,
"classes": [{"id": 12, "types": [...]}, ...]}.
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
from fixturekit import Boundaries, s32, serve, start  # noqa: E402
from guest import _watch_faults, call  # noqa: E402

SCHEMA = 'combat.data.v1'
TEXT = 'cp1252'

# Original code.
RULES_CTOR, RULES_INITIALIZE, BIND_TYPES = 0x488160, 0x48b690, 0x48cab0
# Seams.
ZFOPEN_REL, ZLENGTH, ZREAD, ZCLOSE, FILE_EXISTS = 0x4a13f0, 0x4a17b0, 0x4a15a0, 0x4a1540, 0x4a1c00
FATAL_ERROR, FATAL_ERROR2 = 0x481c10, 0x481d10
WEAPON_CLASSDATA, ARMOR_CLASSDATA = 0x48af30, 0x48b4a0
FIND_OBJTYPE = 0x475210                          # thiscall (name, partial), ret 8

# Globals (COMBAT_DATA.md §2.1, §10).
RULES = 0x65d7a8
CLASSDEF_PATH, IMAGERY_PATH, RESOURCE_PATH, MODULES_PATH = 0x65bd48, 0x65bc44, 0x65dde8, 0x65d6a4
WORK_DIR, EXE_DIR = 0x65d254, 0x6666cc
GAME_SPEED, FATAL_GUARD = 0x5d79e4, 0x65b9f8
CLASSES, NUM_CLASSES, MAX_CLASSES = 0x65a148, 0x65a258, 64

# TRules (§3): pointer arrays {count +0x00, data +0x10}.
R_CLASSES, R_CHARS, R_WEAPONS, R_ARMOR, R_DEF, R_STATLEVELS = 0x04, 0x18, 0x2c, 0x40, 0x54, 0x58
RULES_FIELDS = [
    ('initialized', 0x00), ('daylength', 0x5c), ('twilight', 0x60), ('twilightsteps', 0x64),
    ('healthperlevel', 0x68), ('fatigueperlevel', 0x6c), ('manaperlevel', 0x70),
    ('healthrecovval', 0x74), ('fatiguerecovval', 0x78), ('manarecovval', 0x7c),
    ('healthrecovrate', 0x80), ('fatiguerecovrate', 0x84), ('manarecovrate', 0x88),
    ('poisondamageval', 0x8c), ('poisondamagerate', 0x90),
    ('tohitcenter', 0x94), ('tohitrangechar', 0x98), ('tohitrangeplyr', 0x9c),
    ('tohitblock', 0xa0), ('tohitface', 0xa4),
    ('maxstealth', 0xd0), ('sneakstealth', 0xd4), ('minstealth', 0xd8),
]
TOHITDAMAGE, AMMODATA, EXP, SKILLEXP = 0xa8, 0xdc, 0xf0, 0x168

# SClassData (§4, 0x74 bytes).
CLASS_FIELDS = [('healthmod', 0x64), ('fatiguemod', 0x68), ('manamod', 0x6c), ('weapons', 0x70)]

# SCharData (§5.1, 0x590 bytes). Kinds: int, (str, size), [ints...].
CHAR_FIELDS = [
    ('groups', ('s', 0x20, 80)), ('enemies', ('s', 0x70, 80)),
    ('objtype', 0xc0), ('objclass', 0xc4), ('flags', 0xc8),
    ('damagemods', ('i', 0xe4, 10)),
    ('blocksounds', ('s', 0x10c, 32)), ('misssounds', ('s', 0x12c, 32)),
    ('playerblock', ('i', 0x14c, 3)), ('combatrange', ('i', 0x158, 2)), ('maxattackrange', 0x160),
    ('bleeder', 0x164), ('swipefull', ('b', 0x16c)), ('bodytype', ('s', 0x170, 32)),
    ('block', ('i', 0x194, 3)), ('sight', ('i', 0x1a0, 4)), ('hearing', ('i', 0x1b0, 3)),
    ('weapontype', 0x1bc), ('weapondamage', 0x1c0), ('armor', 0x1c4), ('defensemod', 0x1c8),
    ('attackmod', 0x1cc), ('attackfreq', ('i', 0x1d0, 2)), ('magicfreq', ('i', 0x1d8, 2)),
    ('mana', 0x1e0), ('fatigue', 0x1e4), ('health', 0x1e8),
    ('walkspeed', 0x1ec), ('runspeed', 0x1f0), ('sneakspeed', 0x1f4), ('combatwalkspeed', 0x1f8),
    ('arrowpos', ('i', 0x1fc, 3)), ('arrowspeed', 0x208), ('bowwait', 0x20c), ('bowaimspeed', 0x210),
    ('retreatat', 0x440), ('retreatatmana', 0x444), ('retreatfor', 0x448),
    ('runfatigue', ('i', 0x44c, 2)), ('noparalyze', 0x454),
    ('poisonchance', 0x588), ('labelheight', 0x58c),
]
C_ATTACKS, C_CLASSDATA, C_SWIPECOLOR = 0xcc, 0x190, 0x168
C_NUMIMPACTS, C_IMPACTS, C_ATTACHEFFECT = 0x214, 0x218, 0x458
IMPACT_SIZE, ATTACHEFFECT_SIZE = 0x5c, 0x4c

# SCharAttackData (§6.1, 0x320 bytes): the common head, then the ordinary or
# the magic tail (a union in the source; MAGICATTACK sets flag 0x800000).
CA_MAGICATTACK = 0x800000
ATTACK_HEAD = [('flags', 0x24), ('button', 0x28), ('attackpcnt', 0x2c), ('mindist', 0x30), ('maxdist', 0x34)]
ATTACK_BODY = [
    ('responsename', ('s', 0x38, 32)), ('blockname', ('s', 0x58, 32)), ('missname', ('s', 0x78, 32)),
    ('chainname', ('s', 0x98, 32)), ('blocktime', 0xb8), ('impacttime', 0xbc),
    ('chainexptime', 0xc0), ('nextwait', 0xc4), ('hitminrange', 0xc8), ('hitmaxrange', 0xcc),
    ('hitangle', 0xd0), ('damagemod', 0xd4), ('fatigue', 0xd8), ('attackskill', 0xdc),
    ('weaponmask', 0xe0), ('weaponskill', 0xe4), ('swipeframeon', 0xec), ('swipeframeoff', 0xf0),
    ('maxfatigue', 0xf4),
]
ATTACK_MAGIC = [('spellname', ('s', 0x38, 32)), ('spellsource', ('i', 0x58, 3)),
                ('condition', 0x64), ('conditionvalue', 0x68)]
A_NUMIMPACTS, A_IMPACTS = 0xe8, 0xf8
# Impact record (§6.2, 0x5c bytes).
IMPACT_FIELDS = [('index', 0x20), ('flags', 0x24), ('loopname', ('s', 0x28, 32)), ('looptime', 0x48),
                 ('damagemin', 0x4c), ('damagemax', 0x50), ('snapdist', 0x54), ('snaptime', 0x58)]
# Weapon (0xd0) / armor (0xcc) records (§8): statline is a malloc'd string.
# BASICMODS in the tag's field order: the weapon loader stores them in order
# at +0xa0..+0xbc; the armor loader (0x48b25b) puts fields 5 and 6 (stealth,
# value) at +0xb4 / +0xb0, where its class binding (0x48b4a0) reads Stealth
# and Value.
ITEM_FIELDS = [('description', ('s', 0x20, 0x80))]
BASICMODS = {'weapon': [0xa0, 0xa4, 0xa8, 0xac, 0xb0, 0xb4, 0xb8, 0xbc],
             'armor': [0xa0, 0xa4, 0xa8, 0xac, 0xb4, 0xb0, 0xb8, 0xbc]}
STATLINE = {'weapon': 0xcc, 'armor': 0xc8}


class Fatal(Exception):
    """FatalError was called: the case stops with the message."""


def normal_path(path: str) -> str:
    """A retail path as the case names files: `.\\Resources\\rules.def` and
    `C:\\REVENANT\\Resources\\rules.def` -> `resources/rules.def`."""
    p = path.replace('\\', '/').lower()
    for prefix in ('c:/revenant/', './'):
        if p.startswith(prefix):
            p = p[len(prefix):]
    return p.lstrip('/')


class DataFixture:
    def __init__(self, executable):
        started = time.perf_counter()
        self.vm, self.sha = start(executable)
        vm = self.vm
        _watch_faults(vm)
        # Static init isn't run: construct the Rules object as its initializer
        # (0x480f50) does -- the pointer arrays' growth, the STATLEVEL holder.
        call(vm, RULES_CTOR, this=RULES)
        for address, text in ((CLASSDEF_PATH, '.\\Resources\\'), (RESOURCE_PATH, '.\\Resources\\'),
                              (IMAGERY_PATH, '.\\Imagery\\'), (MODULES_PATH, '.\\Modules\\'),
                              (WORK_DIR, 'C:\\REVENANT\\'), (EXE_DIR, 'C:\\REVENANT\\')):
            vm.write(address, text.encode(TEXT) + b'\0')
        vm.put_u32(FATAL_GUARD, 0)
        b = Boundaries(vm)
        b.add(ZFOPEN_REL, 'zfopen_rel', 0, self._open)
        b.add(ZLENGTH, 'zlength', 0, lambda a, c: len(self.handles[a[0]][0]))
        b.add(ZREAD, 'zread', 0, self._read)
        b.add(ZCLOSE, 'zclose', 0, self._close)
        b.add(FILE_EXISTS, 'FileExists', 0, self._exists)
        b.add(FATAL_ERROR, 'FatalError', 0, self._fatal)
        b.add(FATAL_ERROR2, 'FatalError', 0, self._fatal)
        b.add(WEAPON_CLASSDATA, 'WeaponClassData', 4, lambda a, c: self._classdata('weapon', c))
        b.add(ARMOR_CLASSDATA, 'ArmorClassData', 4, lambda a, c: self._classdata('armor', c))
        b.add(FIND_OBJTYPE, 'FindObjType', 8, self._find_objtype)
        self.types = {}
        vm.checkpoint()
        self.setup_ms = (time.perf_counter() - started) * 1000

    # -- seams ------------------------------------------------------------
    def _classdata(self, kind, record):
        self.seams.append(['classdata', kind, self.cstr(record, 32)])
        return 0

    def _open(self, args, ecx):
        path = self.vm.string(args[0])
        key = normal_path(path)
        self.seams.append(['open', key, self.vm.string(args[1]), args[2]])
        data = self.case_files.get(key)
        if data is None:
            return 0
        handle = self.vm.allocate(16)
        self.handles[handle] = [data, 0]
        return handle

    def _read(self, args, ecx):
        buf, size, count, handle = args[:4]
        data, pos = self.handles[handle]
        want = size * count
        chunk = data[pos:pos + want]
        self.vm.write(buf, chunk)
        self.handles[handle][1] = pos + len(chunk)
        return len(chunk) // size if size else 0

    def _close(self, args, ecx):
        self.handles.pop(args[0], None)
        return 0

    def _exists(self, args, ecx):
        key = normal_path(self.vm.string(args[0]))
        found = key in self.case_files
        self.seams.append(['exists', key, found])
        return int(found)

    def _find_objtype(self, args, ecx):
        if args[1]:
            raise ValueError('FindObjType with partial matching is not answered')
        # _stricmp in the C locale folds ASCII only.
        return self.types.get(ecx, {}).get(bytes(self.vm.string(args[0]).encode(TEXT)).lower(), -1)

    def _fatal(self, args, ecx):
        fmt = self.vm.string(args[0])
        arg = args[1]
        if '%s' in fmt:
            message = fmt.replace('%s', self.vm.string(arg) if arg else '(null)', 1)
        elif '%d' in fmt or '%i' in fmt:
            message = fmt.replace('%d', str(s32(arg)), 1).replace('%i', str(s32(arg)), 1)
        else:
            message = fmt
        raise Fatal(message)

    # -- the stand-in class registry -------------------------------------
    def _classes(self, classes):
        vm = self.vm
        top = 0
        self.types = {}
        for spec in classes:
            cid, types = spec['id'], spec['types']
            if not 0 <= cid < MAX_CLASSES:
                raise ValueError(f'class id {cid} out of range')
            cls = vm.allocate(0x40)
            data = vm.allocate(4 * max(1, len(types)))
            for i, name in enumerate(types):
                record = vm.allocate(8)
                text = vm.allocate(len(name) + 1)
                vm.write(text, name.encode(TEXT) + b'\0')
                vm.put_u32(record, text)
                vm.put_u32(data + 4 * i, record)
            vm.put_u32(cls + 0x08, cid)
            vm.put_u32(cls + 0x24, len(types))
            vm.put_u32(cls + 0x34, data)
            vm.put_u32(CLASSES + 4 * cid, cls)
            names = self.types[cls] = {}
            for i, name in enumerate(types):
                names.setdefault(name.encode(TEXT).lower(), i)
            top = max(top, cid + 1)
        vm.put_u32(NUM_CLASSES, top)

    # -- one case ----------------------------------------------------------
    def run(self, case):
        vm = self.vm
        vm.restore()
        self.seams, self.handles = [], {}
        self.case_files = {normal_path(name): Path(path).read_bytes() for name, path in case['files'].items()}
        vm.put_u32(GAME_SPEED, case.get('gamespeed', 3))
        self._classes(case.get('classes', []))
        fatal, initialized = None, None
        try:
            initialized = s32(call(vm, RULES_INITIALIZE, this=RULES, instruction_limit=2_000_000_000))
            call(vm, BIND_TYPES, this=RULES, instruction_limit=200_000_000)
        except Fatal as stop:
            fatal = str(stop)
        out = dict(schema=SCHEMA, side='retail', fatal=fatal, initialize=initialized, seams=self.seams)
        if fatal is None:
            out.update(self.dump())
        return out

    # -- the dump -----------------------------------------------------------
    def i32(self, address):
        return s32(self.vm.u32(address))

    def cstr(self, address, size):
        raw = bytes(self.vm.uc.mem_read(address, size))
        return raw.split(b'\0')[0].decode(TEXT)

    def fields(self, base, spec):
        out = {}
        for name, where in spec:
            if isinstance(where, int):
                out[name] = self.i32(base + where)
            elif where[0] == 's':
                out[name] = self.cstr(base + where[1], where[2])
            elif where[0] == 'b':
                out[name] = self.vm.uc.mem_read(base + where[1], 1)[0]
            else:
                out[name] = [self.i32(base + where[1] + 4 * i) for i in range(where[2])]
        return out

    def items(self, array):
        count, data = self.vm.u32(array), self.vm.u32(array + 0x10)
        return [p for p in (self.vm.u32(data + 4 * i) for i in range(count)) if p]

    def impact(self, base):
        out = dict(name=self.cstr(base, 32))
        out.update(self.fields(base, IMPACT_FIELDS))
        return out

    def attack(self, base):
        out = dict(name=self.cstr(base, 32))
        out.update(self.fields(base, ATTACK_HEAD))
        if out['flags'] & CA_MAGICATTACK:
            out.update(self.fields(base, ATTACK_MAGIC))
        else:
            out.update(self.fields(base, ATTACK_BODY))
            n = self.i32(base + A_NUMIMPACTS)
            out['impacts'] = [self.impact(base + A_IMPACTS + IMPACT_SIZE * i) for i in range(n)]
        return out

    def character(self, base):
        vm = self.vm
        out = dict(name=self.cstr(base, 32))
        out.update(self.fields(base, CHAR_FIELDS))
        classdata = vm.u32(base + C_CLASSDATA)
        out['class'] = self.cstr(classdata, 32) if classdata else None
        # SWIPECOLOR: r -> +0x16a, g -> +0x169, b -> +0x168 (a 0x00RRGGBB dword).
        b, g, r = bytes(vm.uc.mem_read(base + C_SWIPECOLOR, 3))
        out['swipecolor'] = [r, g, b]
        effects = []
        for i in range(4):
            e = base + C_ATTACHEFFECT + ATTACHEFFECT_SIZE * i
            # ATTACHEFFECT `%20s, %i, %i, %i, %20s, %20s`: field 1 -> +0x34, 2-4 ->
            # +0x00..+0x08, 5 -> +0x0c, 6 -> +0x20 ("none" -> ""); used +0x48.
            effects.append(dict(used=self.i32(e + 0x48), name=self.cstr(e + 0x34, 20),
                                values=[self.i32(e + 4 * k) for k in range(3)],
                                arg5=self.cstr(e + 0x0c, 20), arg6=self.cstr(e + 0x20, 20)))
        out['attacheffects'] = effects
        out['impacts'] = [self.impact(base + C_IMPACTS + IMPACT_SIZE * i)
                          for i in range(self.i32(base + C_NUMIMPACTS))]
        out['attacks'] = [self.attack(p) for p in self.items(base + C_ATTACKS)]
        return out

    def item(self, base, kind):
        out = dict(name=self.cstr(base, 32))
        out.update(self.fields(base, ITEM_FIELDS))
        out['basicmods'] = [self.i32(base + off) for off in BASICMODS[kind]]
        statline = self.vm.u32(base + STATLINE[kind])
        out['statline'] = self.vm.string(statline) if statline else ''
        return out

    def dump(self):
        vm = self.vm
        rules = {name: self.i32(RULES + off) for name, off in RULES_FIELDS}
        rules['tohitdamage'] = [[self.i32(RULES + TOHITDAMAGE + 8 * i), self.i32(RULES + TOHITDAMAGE + 8 * i + 4)]
                                for i in range(5)]
        rules['ammodata'] = [self.i32(RULES + AMMODATA + 4 * i) for i in range(5)]
        rules['exp'] = [self.i32(RULES + EXP + 4 * i) for i in range(30)]
        rules['skillexp'] = [self.i32(RULES + SKILLEXP + 4 * i) for i in range(30)]
        tables = vm.u32(RULES + R_STATLEVELS)
        rules['statlevels'] = [[self.i32(vm.u32(tables + 4 * t) + 4 * v) for v in range(31)] for t in range(6)]
        default = vm.u32(RULES + R_DEF)
        rules['default'] = self.cstr(default, 32) if default else None
        classes = []
        for p in self.items(RULES + R_CLASSES):
            c = dict(name=self.cstr(p, 32), statreqs=[self.i32(p + 0x20 + 4 * i) for i in range(6)],
                     skillmods=[self.i32(p + 0x38 + 4 * i) for i in range(11)])
            c.update(self.fields(p, CLASS_FIELDS))
            classes.append(c)
        return dict(rules=rules, classes=classes,
                    chars=[self.character(p) for p in self.items(RULES + R_CHARS)],
                    weapons=[self.item(p, 'weapon') for p in self.items(RULES + R_WEAPONS)],
                    armor=[self.item(p, 'armor') for p in self.items(RULES + R_ARMOR)])


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable')
    parser.add_argument('--serve', action='store_true')
    parser.add_argument('--case', type=Path)
    args = parser.parse_args()
    fixture = DataFixture(args.executable)
    if args.serve:
        serve(fixture.sha, SCHEMA, fixture.setup_ms, fixture.run)
    elif args.case:
        print(json.dumps(fixture.run(json.loads(args.case.read_text())), indent=1))
    else:
        parser.error('--serve or --case')


if __name__ == '__main__':
    main()
