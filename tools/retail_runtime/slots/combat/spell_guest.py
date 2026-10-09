"""Combat slot: retail's spell list in guest memory, shared by the spell and
missile katas (docs/gameplay/forensics/SPELLS_MISSILES.md).

- `SpellFiles`: the file layer TToken::Open reads through (zfopen_rel
  `0x4a13f0`, length `0x4a17b0`, fread `0x4a15a0`, close `0x4a1540`), answered
  from a spell.def held by the host; FatalError `0x481c10` / `0x481d10`
  stops the call with the message; the imagery lookup a CONTROLDATA IMAGERY
  tag makes (`0x446aa0`, the registry by file name) answers a handle that
  names the file, so a dump can say which imagery a block named.
- `load_spells(vm, files)`: TSpellList's constructor (`0x4883a0`, static
  init isn't run) and Load (`0x53ead0`) on the list at `0x667c38`, as
  original code: the tokenizer, Parse, SSpellData::Load `0x53e4e0`,
  LoadControlData `0x53dd10`.
- `shipped_spell_def()`: the spell.def retail reads (resources.rvr, packs
  first: COMBAT_DATA.md §2), from $REVENANT_DATA_PATH.
- `spell_list_dump` / `variant_dump`: the list by field name, the compared
  record of kata D1s and the vocabulary of the other spell katas.

Layouts (SPELLS_MISSILES.md §2.17): TSpellList count `+0x04`, items
`+0x14`; SSpellData 0xcc bytes; SSpellVariant 0x78; control data 0xc4.
"""
from __future__ import annotations

import os
import struct
import zipfile
from pathlib import Path

from fixturekit import s32

TEXT = 'cp1252'

SPELL_LIST = 0x667c38
LIST_CTOR, LIST_LOAD = 0x4883a0, 0x53ead0
ZFOPEN_REL, ZLENGTH, ZREAD, ZCLOSE = 0x4a13f0, 0x4a17b0, 0x4a15a0, 0x4a1540
FATAL_ERROR, FATAL_ERROR2, FATAL_GUARD = 0x481c10, 0x481d10, 0x65b9f8
FIND_IMAGERY = 0x446aa0                         # cdecl (filename) -> registry index or -1
CLASSDEF_PATH = 0x65bd48
IMAGERY_HANDLE = 0x7000                         # handles the imagery seam answers: base + call index

# SSpellData
SD_VARIANTS, SD_FLAGS, SD_NAME, SD_OBJNAME, SD_ICON, SD_DESC = 0x00, 0x18, 0x1c, 0x3c, 0x5c, 0x7c
SD_DAMAGETYPE, SD_INVOKE, SD_DELAY, SD_POISON = 0x80, 0x84, 0xa4, 0xc8
# SSpellVariant
V_FIELDS = [('flags', 0x00), ('type', 0x60), ('mana', 0x4c), ('wait', 0x50), ('min', 0x54), ('max', 0x58),
            ('skill', 0x5c), ('height', 0x64), ('facing', 0x68), ('ani_delay', 0x6c)]
V_NAME, V_TALISMANS, V_EFFECT, V_CONTROL, V_STATLINE = 0x04, 0x24, 0x2a, 0x70, 0x74
# Control data
CD_FIELDS = [('flags', 0x20), ('radius', 0x24), ('hits', 0x28), ('duration', 0x2c), ('wait', 0x30),
             ('duration2', 0x34), ('pattern', 0x38), ('shake', 0x3c), ('unknown40', 0x40),
             ('repeatdamage', 0x44), ('hittarget', 0x9c), ('oncaster', 0xa0), ('attachset', 0xa4),
             ('follow', 0xa8), ('posset', 0xac), ('repeatset', 0xb0), ('multipletargets', 0xb4)]
CD_RANGEDAMAGE, CD_ATTACH, CD_ATTACHMULTIPLE, CD_SOUND, CD_IMAGERY, CD_POS = 0x48, 0x50, 0x70, 0x74, 0x94, 0xb8


class Fatal(Exception):
    """FatalError was called: the case stops with the message."""


def shipped_spell_def() -> bytes:
    data = Path(os.environ.get('REVENANT_DATA_PATH',
                               Path.home() / 'RevenantRetailLab' / 'retail-cd' / 'REVENANT'))
    with zipfile.ZipFile(data / 'resources.rvr') as z:
        for n in z.namelist():
            if n.lower() == 'spell.def':
                return z.read(n)
    raise FileNotFoundError('resources.rvr has no spell.def')


class SpellFiles:
    """The file seams under TToken::Open for spell.def, and the imagery
    lookup. `seams` records each call in order."""

    def __init__(self, vm, boundaries):
        self.vm = vm
        self.spell_def = b''
        self.handles = {}
        self.imagery = []                         # file names the imagery lookup was asked for
        self.seams = []
        vm.write(CLASSDEF_PATH, b'.\\Resources\\\0')
        vm.put_u32(FATAL_GUARD, 0)
        b = boundaries
        b.add(ZFOPEN_REL, 'zfopen_rel', 0, self._open)
        b.add(ZLENGTH, 'zlength', 0, lambda a, c: len(self.handles[a[0]][0]))
        b.add(ZREAD, 'zread', 0, self._read)
        b.add(ZCLOSE, 'zclose', 0, self._close)
        b.add(FATAL_ERROR, 'FatalError', 0, self._fatal)
        b.add(FATAL_ERROR2, 'FatalError', 0, self._fatal)
        b.add(FIND_IMAGERY, 'FindImagery', 0, self._find_imagery)

    def _open(self, args, ecx):
        path = self.vm.string(args[0])
        self.seams.append(['open', path.lower()])
        if not path.lower().endswith('spell.def'):
            return 0
        handle = self.vm.allocate(16)
        self.handles[handle] = [self.spell_def, 0]
        return handle

    def _read(self, args, ecx):
        buf, size, count, handle = args[:4]
        data, pos = self.handles[handle]
        chunk = data[pos:pos + size * count]
        self.vm.write(buf, chunk)
        self.handles[handle][1] = pos + len(chunk)
        return len(chunk) // size if size else 0

    def _close(self, args, ecx):
        self.handles.pop(args[0], None)
        return 0

    def _find_imagery(self, args, ecx):
        self.imagery.append(self.vm.string(args[0]))
        return IMAGERY_HANDLE + len(self.imagery) - 1

    def _fatal(self, args, ecx):
        fmt = self.vm.string(args[0])
        arg = args[1]
        if '%s' in fmt:
            fmt = fmt.replace('%s', self.vm.string(arg) if arg else '(null)', 1)
        elif '%d' in fmt or '%i' in fmt:
            fmt = fmt.replace('%d', str(s32(arg)), 1).replace('%i', str(s32(arg)), 1)
        raise Fatal(fmt)


def construct_list(vm, call):
    """TSpellList's static constructor (0x480f80 calls it): the pointer array."""
    call(vm, LIST_CTOR, this=SPELL_LIST)


def load_spells(vm, call, files: SpellFiles, spell_def: bytes):
    files.spell_def = spell_def
    files.imagery = []
    return s32(call(vm, LIST_LOAD, this=SPELL_LIST, instruction_limit=2_000_000_000))


# ---- the dump -------------------------------------------------------------------

def cstr(vm, address, size):
    raw = bytes(vm.uc.mem_read(address, size))
    return raw.split(b'\0')[0].decode(TEXT)


def items(vm, array):
    """A pointer array {count +0, data +0x10, default +0x14}: its items in
    order (an empty slot reads as the default, as the array's Get does)."""
    count, data, default = vm.u32(array), vm.u32(array + 0x10), vm.u32(array + 0x14)
    return [vm.u32(data + 4 * i) or default for i in range(count)]


def spells(vm):
    return items(vm, SPELL_LIST + 4)


def i32(vm, address):
    return s32(vm.u32(address))


def control_dump(vm, cd, files: SpellFiles):
    out = dict(name=cstr(vm, cd, 32))
    out.update({k: i32(vm, cd + off) for k, off in CD_FIELDS})
    out['rangedamage'] = [i32(vm, cd + CD_RANGEDAMAGE), i32(vm, cd + CD_RANGEDAMAGE + 4)]
    out['attach'] = cstr(vm, cd + CD_ATTACH, 32)
    multiple = vm.u32(cd + CD_ATTACHMULTIPLE)
    hits = i32(vm, cd + 0x28)
    out['attachmultiple'] = [cstr(vm, multiple + 32 * i, 32) for i in range(max(hits, 0))] if multiple else None
    out['sound'] = cstr(vm, cd + CD_SOUND, 32)
    handle = i32(vm, cd + CD_IMAGERY)
    index = handle - IMAGERY_HANDLE
    out['imagery'] = files.imagery[index] if 0 <= index < len(files.imagery) else None
    out['pos'] = list(struct.unpack('<3i', vm.uc.mem_read(cd + CD_POS, 12)))
    return out


def variant_dump(vm, v, files: SpellFiles):
    out = dict(name=cstr(vm, v + V_NAME, 32), talismans=cstr(vm, v + V_TALISMANS, 6),
               effect=cstr(vm, v + V_EFFECT, 32))
    out.update({k: i32(vm, v + off) for k, off in V_FIELDS})
    statline = vm.u32(v + V_STATLINE)
    out['statline'] = cstr(vm, statline, 100) if statline else None
    cd = vm.u32(v + V_CONTROL)
    out['controldata'] = control_dump(vm, cd, files) if cd else None
    return out


def spell_dump(vm, sd, files: SpellFiles):
    desc = vm.u32(sd + SD_DESC)
    b, g, r = bytes(vm.uc.mem_read(sd + 0xa8, 3))
    light = dict(color=[r, g, b], mult=i32(vm, sd + 0xac), intensity=i32(vm, sd + 0xb0),
                 pos=list(struct.unpack('<3i', vm.uc.mem_read(sd + 0xb4, 12))),
                 fadein=i32(vm, sd + 0xc0), fadeout=i32(vm, sd + 0xc4))
    return dict(name=cstr(vm, sd + SD_NAME, 32), objname=cstr(vm, sd + SD_OBJNAME, 32),
                iconname=cstr(vm, sd + SD_ICON, 32), description=vm.string(desc) if desc else None,
                flags=i32(vm, sd + SD_FLAGS), damagetype=i32(vm, sd + SD_DAMAGETYPE),
                invoke=cstr(vm, sd + SD_INVOKE, 32), delay=i32(vm, sd + SD_DELAY),
                poisonchance=i32(vm, sd + SD_POISON), light=light,
                variants=[variant_dump(vm, v, files) for v in items(vm, sd + SD_VARIANTS)])


def spell_list_dump(vm, files: SpellFiles):
    return [spell_dump(vm, sd, files) for sd in spells(vm)]


def variant_names(vm):
    """{guest address: (spell name, variant name)} for every variant, and
    {guest address: spell name} for every spell: how a dump names a pointer."""
    by_variant, by_spell = {}, {}
    for sd in spells(vm):
        by_spell[sd] = cstr(vm, sd + SD_NAME, 32)
        for v in items(vm, sd + SD_VARIANTS):
            by_variant[v] = (by_spell[sd], cstr(vm, v + V_NAME, 32))
    return by_variant, by_spell
