#!/usr/bin/env python3
"""Decode, compare and check retail Revenant save files and sector files.

    revsave.py dump   <file>                         decoded listing (game.sav, newgame.sav, N_X_Y.DAT)
    revsave.py diff   [--fixed-flags] <a> <b>        field-level diff of two files of the same kind
    revsave.py cmpdir [--fixed-flags] <dirA> <dirB>  diff every sector file present in both directories
    revsave.py statehash <sector file or dir> ...    recompute each sector's state hash and compare
    revsave.py bmp    <ss.bmp> ...                   check a slot thumbnail's format

Layouts: docs/gameflow/forensics/SAVE_GAME.md §3 (save file) and §11 (object
stream, sector hash, thumbnail). Object bodies are decoded down to every
field retail's Load reads. A class tail whose layout depends on the C++ class
rather than the object class (an ambient sound effect, a monster generator)
is shown as raw bytes, so a diff still pins a difference to an object and an
offset.

`diff` and `cmpdir` ignore the save header's game time, the one field a save
written right after a load is expected to change. With --fixed-flags they
also ignore the object flag bits retail's TObjectInstance::Load takes from
the constructor rather than the file (MOVING, AI, COMPLEX, NOTIFY, NONMAP,
INVENTORY, CALLEDPREDEL): runtime state that no load ever reads back.
"""
import re
import signal
import struct
import sys
from pathlib import Path

OF_IMMOBILE = 1 << 0
OF_LIGHT = 1 << 2
OF_ANIMATE = 1 << 14
OF_NONMAP = 1 << 19
FIXED_FLAGS = 0xc00e0028        # ~0x3ff1ffd7: TObjectInstance::Load @ 0x00472430

OBJCLASS_NAMES = {
    0: 'ITEM', 1: 'WEAPON', 2: 'ARMOR', 3: 'TALISMAN', 4: 'FOOD', 5: 'CONTAINER',
    6: 'LIGHTSOURCE', 7: 'TOOL', 8: 'MONEY', 9: 'TILE', 10: 'EXIT', 11: 'PLAYER',
    12: 'CHARACTER', 13: 'TRAP', 14: 'SHADOW', 15: 'HELPER', 16: 'KEY',
    17: 'INVCONTAINER', 18: 'POTION', 21: 'AMMO', 22: 'SCROLL', 23: 'RANGEDWEAPON',
    25: 'EFFECT', 26: 'MAPSCROLL',
}
OBJCLASS_WEAPON = 1
OBJCLASS_EXIT = 10
OBJCLASS_PLAYER = 11
OBJCLASS_CHARACTER = 12
OBJCLASS_SCROLL = 22


class Truncated(Exception):
    pass


class Reader:
    def __init__(self, data, pos=0, end=None):
        self.d = data
        self.pos = pos
        self.end = len(data) if end is None else end

    def take(self, n):
        if n < 0 or self.pos + n > self.end:
            raise Truncated(f'need {n} bytes at {self.pos:#x}, block ends at {self.end:#x}')
        b = self.d[self.pos:self.pos + n]
        self.pos += n
        return b

    def u8(self): return self.take(1)[0]
    def i16(self): return struct.unpack('<h', self.take(2))[0]
    def u16(self): return struct.unpack('<H', self.take(2))[0]
    def i32(self): return struct.unpack('<i', self.take(4))[0]
    def u32(self): return struct.unpack('<I', self.take(4))[0]

    def pstr(self):
        """Stream string (retail 0x0049ce00): uint8 length + bytes, no NUL."""
        return self.take(self.u8()).decode('latin1')

    def cstr(self):
        """NUL-terminated string."""
        end = self.d.index(b'\0', self.pos, self.end)
        s = self.d[self.pos:end].decode('latin1')
        self.pos = end + 1
        return s


class Field:
    """One decoded value: path, file offset, size, value."""
    __slots__ = ('path', 'off', 'size', 'value')

    def __init__(self, path, off, size, value):
        self.path, self.off, self.size, self.value = path, off, size, value


class Record:
    """An object record's location in a sector file (for the state hash)."""
    __slots__ = ('objclass', 'header', 'bodystart', 'blocksize', 'invblocksize')

    def __init__(self, objclass, header, bodystart, blocksize, invblocksize):
        self.objclass, self.header = objclass, header
        self.bodystart, self.blocksize, self.invblocksize = bodystart, blocksize, invblocksize


class Decoder:
    def __init__(self, data):
        self.r = Reader(data)
        self.fields = []
        self.records = []           # top-level object records, in order (None = empty slot)

    def emit(self, path, off, value):
        self.fields.append(Field(path, off, self.r.pos - off, value))
        return value

    def f(self, path, kind):
        off = self.r.pos
        return self.emit(path, off, getattr(self.r, kind)())

    def hexf(self, path, kind):
        off = self.r.pos
        v = getattr(self.r, kind)()
        return self.emit(path, off, f'{v:#010x}')

    def raw(self, path, n):
        off = self.r.pos
        return self.emit(path, off, self.r.take(n).hex())

    # ---------------------------------------------------------------- objects

    def object(self, path, version, toplevel=False):
        """LoadObject (0x00471ce0). Bodies are decoded for stream version 14
        and up; older streams down to version 4 are delimited by their block
        sizes and shown raw."""
        r = self.r
        header = r.pos
        objversion = self.f(f'{path}.objversion', 'i16') if version >= 8 else 0
        if objversion < 0:
            if toplevel:
                self.records.append(None)
            return None
        objclass = self.f(f'{path}.objclass', 'i16')
        if objclass < 0:
            if toplevel:
                self.records.append(None)
            return None
        self.emit(f'{path}.class', r.pos, OBJCLASS_NAMES.get(objclass, f'?{objclass}'))
        if version < 4:
            # Before version 4 there are no block sizes: only a decoder for
            # every class's body could find where an object ends.
            raise Truncated(f'stream version {version} has no object block sizes')
        self.hexf(f'{path}.uniqueid', 'u32')
        blocksize = self.f(f'{path}.blocksize', 'i16')
        invblocksize = self.f(f'{path}.invblocksize', 'i16') if version >= 14 else -1
        bodystart = r.pos
        if toplevel:
            self.records.append(Record(objclass, header, bodystart, blocksize, invblocksize))
        if version < 14:
            # A single block size covers body and inventory: show it raw.
            self.raw(f'{path}.block', blocksize)
            return objclass
        bodyend = bodystart + blocksize - max(invblocksize, 0)
        outer_end = r.end
        r.end = bodyend
        try:
            self.body(path, objclass, objversion, version)
            if r.pos < bodyend:
                self.raw(f'{path}.tail', bodyend - r.pos)
        except Truncated as e:
            self.emit(f'{path}.DECODE_ERROR', r.pos, str(e))
        r.pos = bodyend
        r.end = bodystart + blocksize
        if invblocksize > 0:
            n = self.f(f'{path}.inventory.count', 'i32')
            for i in range(n):
                self.object(f'{path}.inv[{i}]', version)
        r.pos = bodystart + blocksize
        r.end = outer_end
        return objclass

    def body(self, path, objclass, objversion, version):
        if objclass == OBJCLASS_PLAYER:
            self.player(path, objversion, version)
        elif objclass == OBJCLASS_CHARACTER:
            self.character(path, objversion, version)
        else:
            self.instance(path, version)
            if objclass == OBJCLASS_WEAPON:
                self.f(f'{path}.poison', 'i32')
            elif objclass == OBJCLASS_EXIT:
                self.f(f'{path}.exitflags', 'i32')
            elif objclass == OBJCLASS_SCROLL:
                off = self.r.pos
                n = self.r.i16()
                self.emit(f'{path}.text', off, self.r.take(max(n, 0)).decode('latin1'))

    def instance(self, path, version):
        """TObjectInstance::Load (0x00472430), stream version >= 9."""
        r = self.r
        off = r.pos
        n = r.u8()
        name = bytes(b & 0x7f for b in r.take(n)).decode('latin1')
        self.emit(f'{path}.name', off, name if n else '<type name>')
        flags = self.hexf(f'{path}.flags', 'u32')
        flags = int(flags, 16)
        for c in 'xyz':
            self.f(f'{path}.pos.{c}', 'i32')
        if not (flags & OF_IMMOBILE):
            for c in 'xyz':
                self.f(f'{path}.vel.{c}', 'i32')
        self.f(f'{path}.state', 'u16')
        if flags & OF_NONMAP:
            self.f(f'{path}.level', 'u16')
        self.f(f'{path}.inventnum', 'i16')
        self.f(f'{path}.invindex', 'i16')
        self.f(f'{path}.shadow', 'i32')
        for c in 'xyz':
            self.f(f'{path}.rotate{c}', 'u8')
        self.f(f'{path}.mapindex', 'i32')
        if flags & OF_ANIMATE:
            self.f(f'{path}.frame', 'i16')
            self.f(f'{path}.framerate', 'i16')
        self.f(f'{path}.group', 'u8')
        nstats = self.f(f'{path}.numstats', 'u8')
        for i in range(nstats):
            off = r.pos
            val = r.i32()
            sid = r.u32()
            tag = struct.pack('<I', sid & 0x7f7f7f7f).rstrip(b'\0').decode('latin1')
            self.emit(f'{path}.stat[{i}]', off, f'{tag}={val} (id {sid:#010x})')
        if flags & OF_LIGHT:
            self.f(f'{path}.light.flags', 'u8')
            for c in 'xyz':
                self.f(f'{path}.light.pos.{c}', 'i32')
            for c in ('red', 'green', 'blue', 'intensity'):
                self.f(f'{path}.light.{c}', 'u8')
            self.f(f'{path}.light.multiplier', 'i16')

    def complexobj(self, path, objversion, version):
        """TComplexObject::Load (0x004db930)."""
        if objversion >= 1:
            self.f(f'{path}.TObjectInstance.objversion', 'u8')
        self.instance(path, version)
        if version >= 7:
            self.f(f'{path}.root.action', 'u8')
            self.f(f'{path}.root.name', 'pstr')

    def character(self, path, objversion, version):
        """TCharacter::Load (0x004d4eb0)."""
        cv = self.f(f'{path}.TComplexObject.objversion', 'u8') if objversion >= 3 else 0
        self.complexobj(path, cv, version)
        if objversion < 1:
            return
        for n in ('lasthealthrecov', 'lastfatiguerecov', 'lastmanarecov'):
            self.f(f'{path}.{n}', 'i32')
        if objversion >= 4:
            self.f(f'{path}.lastpoisondamage', 'i32')
        if objversion >= 2:
            for c in 'xyz':
                self.f(f'{path}.teleport.{c}', 'i32')
            self.f(f'{path}.teleport.level', 'i32')

    def player(self, path, objversion, version):
        """TPlayer::Load (0x0051b960) and the automap record (0x00529770)."""
        r = self.r
        chv = self.f(f'{path}.TCharacter.objversion', 'u8') if objversion > 3 else objversion
        self.character(path, chv, version)
        if objversion > 3:
            for i in range(5):
                self.f(f'{path}.quickspell[{i}]', 'pstr')
        if objversion > 4:
            n = self.f(f'{path}.knownspells.count', 'i32')
            for i in range(n):
                off = r.pos
                self.emit(f'{path}.knownspells[{i}]', off, r.take(6).rstrip(b'\0').decode('latin1'))
            if 5 < objversion < 9:
                for i in range(n):
                    self.raw(f'{path}.knownspells_old[{i}]', 4)
        if objversion > 6:
            for n in ('sidebaropen', 'uppermode', 'lowermode', 'unknown19c'):
                self.f(f'{path}.hud.{n}', 'i32')
        if objversion >= 8:
            for i in range(3):
                self.f(f'{path}.levelupstats[{i}]', 'i32')
        if objversion >= 11:
            self.f(f'{path}.team.name', 'pstr')
            self.f(f'{path}.team.name2', 'pstr')
            self.f(f'{path}.team.value', 'i32')
            if objversion >= 13:
                self.f(f'{path}.team.id', 'i32')
                self.f(f'{path}.team.teamindex', 'i32')
            self.f(f'{path}.module', 'pstr')
        elif objversion >= 10:
            self.raw(f'{path}.team.name_fixed', 0x32)
        if objversion >= 13:
            self.f(f'{path}.playerstate', 'i32')
            self.f(f'{path}.statetime', 'i32')
            for i in range(4):
                self.f(f'{path}.profile[{i}]', 'pstr')
            for i in range(4 if objversion >= 15 else 2):
                self.f(f'{path}.frags[{i}]', 'i32')
        if objversion >= 14:
            self.f(f'{path}.automap.module', 'pstr')
            n = self.f(f'{path}.automap.count', 'i32')
            for i in range(n):
                self.f(f'{path}.automap[{i}].level', 'i32')
                k = self.f(f'{path}.automap[{i}].words', 'i32')
                self.raw(f'{path}.automap[{i}].mask', 2 * k)

    # ------------------------------------------------------------- top level

    def save(self):
        r = self.r
        self.f('header.gametime', 'i32')
        self.raw('header.zero04', 0x10)
        mp = self.f('header.multiplayer', 'i32')
        fmt = self.f('header.playerformat', 'i32')
        version = self.f('header.version', 'i32')
        off = r.pos
        self.emit('header.module', off, r.take(32).split(b'\0')[0].decode('latin1'))
        self.raw('header.zero40', 0x40)
        if mp and fmt >= 1:
            self.raw('mpblock', 0x200)
        if version > 9:
            n = self.f('states.count', 'i32')
            for _ in range(n):
                off = r.pos
                ln = r.u8()
                name = bytes(b ^ 0x80 for b in r.take(ln)).decode('latin1')
                self.emit(f'states[{name}]', off, r.i32())
        if version >= 11 and fmt > 1:
            n = self.f('solduniques.count', 'i32')
            for i in range(n):
                self.f(f'solduniques[{i}].objclass', 'i32')
                self.f(f'solduniques[{i}].objtype', 'i32')
        count = 1
        if fmt < 1:
            if version > 12:
                self.raw('legacy_pad', 0x50)
        else:
            count = self.f('players.count', 'i32')
        for i in range(count):
            self.object(f'player[{i}]', version)
        if r.pos != len(r.d):
            self.raw('TRAILING', len(r.d) - r.pos)

    def sector(self):
        r = self.r
        off = r.pos
        self.emit('fcc', off, r.take(4).decode('latin1'))
        version = self.f('version', 'i32')
        if version > 13:
            self.hexf('statehash', 'u32')
        n = self.f('numobjects', 'i32')
        for i in range(n):
            self.object(f'obj[{i}]', version, toplevel=True)
        if r.pos != len(r.d):
            self.raw('TRAILING', len(r.d) - r.pos)
        return version


def decode_full(path):
    data = Path(path).read_bytes()
    d = Decoder(data)
    try:
        if data[:4] == b'MAP ':
            d.sector()
        else:
            d.save()
    except Truncated as e:
        d.emit('DECODE_ERROR', d.r.pos, str(e))
    return d


def decode(path):
    return decode_full(path).fields


# ------------------------------------------------------------------- dump/diff

def dump(path):
    for fl in decode(path):
        print(f'{fl.off:#08x} +{fl.size:<3d} {fl.path} = {fl.value}')


def comparable(fl, fixed_flags):
    if fixed_flags and fl.path.endswith('.flags') and '.light.' not in fl.path:
        return f'{int(fl.value, 16) & ~FIXED_FLAGS:#010x}'
    return fl.value


def diff_fields(fa, fb, fixed_flags=False):
    """Field-level differences (empty when identical)."""
    ignored = {'header.gametime'}
    out = []
    b = {f.path: f for f in fb}
    a = {f.path for f in fa}
    for f in fa:
        if f.path in ignored:
            continue
        g = b.get(f.path)
        if g is None:
            out.append(f'- {f.path} = {f.value}  (only in A @ {f.off:#x})')
        elif comparable(g, fixed_flags) != comparable(f, fixed_flags):
            out.append(f'~ {f.path}: {f.value}  ->  {g.value}  (A @ {f.off:#x}, B @ {g.off:#x})')
    for g in fb:
        if g.path not in a and g.path not in ignored:
            out.append(f'+ {g.path} = {g.value}  (only in B @ {g.off:#x})')
    return out


def diff(pa, pb, fixed_flags=False):
    lines = diff_fields(decode(pa), decode(pb), fixed_flags)
    ra, rb = Path(pa).read_bytes(), Path(pb).read_bytes()
    is_save = ra[:4] != b'MAP '
    same_bytes = ra == rb or (is_save and ra[4:] == rb[4:])
    for ln in lines:
        print(ln)
    print(f'{len(lines)} field difference(s); bytes {"identical" if same_bytes else "differ"}'
          f'{" (ignoring game time)" if is_save else ""}; sizes {len(ra)} / {len(rb)}')
    return 0 if not lines else 1


def cmpdir(da, db, fixed_flags=False):
    a = {p.name.upper(): p for p in Path(da).iterdir() if p.suffix.upper() == '.DAT'}
    b = {p.name.upper(): p for p in Path(db).iterdir() if p.suffix.upper() == '.DAT'}
    common = sorted(set(a) & set(b))
    same = equal_fields = 0
    for name in common:
        if a[name].read_bytes() == b[name].read_bytes():
            same += 1
            continue
        lines = diff_fields(decode(a[name]), decode(b[name]), fixed_flags)
        if not lines:
            equal_fields += 1
            continue
        print(f'{name}: {len(lines)} field difference(s)')
        for ln in lines[:20]:
            print('   ', ln)
    print(f'{same}/{len(common)} byte-identical'
          + (f', {equal_fields} more equal ignoring fixed flags' if fixed_flags else '')
          + f'; only in A: {len(set(a) - set(b))}, only in B: {len(set(b) - set(a))}')
    return 0 if same + equal_fields == len(common) else 1


# ------------------------------------------------------------------ state hash

def retail_adler32(chunks):
    """Adler-32 as retail computes it (0x0056ff60 / 0x0056ff80): bytes are
    signed, sums reduced mod 65521 after each 5552-byte run."""
    a, b = 1, 0
    for data in chunks:
        i = 0
        while i < len(data):
            run = data[i:i + 5552]
            for byte in run:
                a = (a + (byte - 256 if byte > 127 else byte)) & 0xffffffff
                b = (b + a) & 0xffffffff
            a %= 65521
            b %= 65521
            i += len(run)
    return (b << 16) | a


def sector_hash(path):
    """Recompute a sector's state hash (0x00499e90) from its file. Returns
    (stored, computed, computed_if_player) or None for pre-v14 files. A
    player in the sector is written as an empty slot, so the hash it
    contributed (its 0xffff placeholder, at its slot) can't be told from a
    real empty slot; the third value is the set of hashes for a player in
    each of the empty slots."""
    data = Path(path).read_bytes()
    m = re.match(r'(\d+)_(\d+)_(\d+)\.DAT$', Path(path).name, re.I)
    level, sx, sy = (int(g) for g in m.groups())
    if struct.unpack_from('<i', data, 4)[0] <= 13:
        return None
    d = Decoder(data)
    d.sector()
    stored = struct.unpack_from('<I', data, 8)[0]
    chunks = [struct.pack('<4i', level, sx, sy, len(d.records))]
    empty_at = []           # index into chunks where each empty slot falls
    for rec in d.records:
        if rec is None:
            empty_at.append(len(chunks))
            continue
        if rec.objclass not in (OBJCLASS_PLAYER, OBJCLASS_CHARACTER):
            continue
        bodysize = rec.blocksize - max(rec.invblocksize, 0)
        head = bytearray(data[rec.header:rec.bodystart])
        struct.pack_into('<hh', head, len(head) - 4, bodysize, 0)
        chunks.append(bytes(head) + data[rec.bodystart:rec.bodystart + bodysize])
    computed = retail_adler32(chunks) or 0xf0f0f0f0
    with_player = {retail_adler32(chunks[:at] + [b'\xff\xff'] + chunks[at:]) or 0xf0f0f0f0
                   for at in empty_at}
    return stored, computed, with_player


def hash_cmd(paths):
    files = []
    for p in map(Path, paths):
        files += sorted(x for x in p.iterdir() if x.suffix.upper() == '.DAT') if p.is_dir() else [p]
    ok = player = bad = old = 0
    for f in files:
        res = sector_hash(f)
        if res is None:
            old += 1
            continue
        stored, computed, with_player = res
        if stored == computed:
            ok += 1
        elif stored in with_player:
            player += 1
        else:
            bad += 1
            print(f'{f.name}: stored {stored:#010x}, computed {computed:#010x}')
    print(f'{ok} match, {player} match with the player in the sector, {bad} differ, '
          f'{old} pre-v14 (no hash)')
    return 0 if bad == 0 else 1


# ------------------------------------------------------------------- thumbnail

def bmp_cmd(paths):
    """ss.bmp as retail's SaveBMP (0x004a2960) writes it: 216x160, 24-bit,
    bottom-up, 54-byte headers, file size field 103,734."""
    bad = 0
    for p in paths:
        d = Path(p).read_bytes()
        magic, size, r1, r2, off = struct.unpack_from('<2sIHHI', d, 0)
        hsize, w, h, planes, bpp, comp, isize, xr, yr, used, imp = struct.unpack_from('<IiiHHIIiiII', d, 14)
        expect = dict(magic=b'BM', size=103734, off=54, hsize=40, w=216, h=160, planes=1, bpp=24,
                      comp=0, isize=0, xr=0, yr=0, used=0, imp=0, length=103734)
        got = dict(magic=magic, size=size, off=off, hsize=hsize, w=w, h=h, planes=planes, bpp=bpp,
                   comp=comp, isize=isize, xr=xr, yr=yr, used=used, imp=imp, length=len(d))
        wrong = {k: (got[k], v) for k, v in expect.items() if got[k] != v}
        print(f'{p}: ' + ('retail format' if not wrong else f'differs {wrong}'))
        bad += bool(wrong)
    return 0 if bad == 0 else 1


def main(argv):
    args = argv[1:]
    fixed = '--fixed-flags' in args
    args = [a for a in args if a != '--fixed-flags']
    if len(args) >= 2 and args[0] == 'dump':
        dump(args[1])
        return 0
    if len(args) >= 3 and args[0] == 'diff':
        return diff(args[1], args[2], fixed)
    if len(args) >= 3 and args[0] == 'cmpdir':
        return cmpdir(args[1], args[2], fixed)
    if len(args) >= 2 and args[0] == 'statehash':
        return hash_cmd(args[1:])
    if len(args) >= 2 and args[0] == 'bmp':
        return bmp_cmd(args[1:])
    print(__doc__)
    return 2


if __name__ == '__main__':
    signal.signal(signal.SIGPIPE, signal.SIG_DFL)   # quiet when piped into head
    sys.exit(main(sys.argv))
