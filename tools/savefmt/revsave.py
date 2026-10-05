#!/usr/bin/env python3
"""Decode and compare retail Revenant save files and sector files.

    revsave.py dump  <file>            decoded listing (game.sav / newgame.sav / N_X_Y.DAT)
    revsave.py diff  <a> <b>           field-level diff of two files of the same kind
    revsave.py cmpdir <dirA> <dirB>    diff every sector file present in both directories

The format follows docs/gameflow/forensics/SAVE_GAME.md (save file and
object stream) and recon/docs/SECTOR_FILE_FORMAT.md (sector header). Object
bodies are decoded down to the fields each retail Load reads; the class tail
after TObjectInstance's fields is decoded for the classes whose layout is
known (player, character, complex object) and shown as raw bytes otherwise,
so a diff still localises a difference to an object and a byte offset.

`diff` ignores the save header's game time (it is the only field a save
written right after a load is expected to change).
"""
import struct
import sys
from pathlib import Path

OF_IMMOBILE = 1 << 0
OF_LIGHT = 1 << 2
OF_ANIMATE = 1 << 14
OF_NONMAP = 1 << 19

OBJCLASS_NAMES = {
    0: 'ITEM', 1: 'WEAPON', 2: 'ARMOR', 3: 'TALISMAN', 4: 'FOOD', 5: 'CONTAINER',
    6: 'LIGHTSOURCE', 7: 'TOOL', 8: 'MONEY', 9: 'TILE', 10: 'EXIT', 11: 'PLAYER',
    12: 'CHARACTER', 13: 'TRAP', 14: 'SHADOW', 15: 'HELPER', 16: 'KEY',
    17: 'INVCONTAINER', 18: 'POTION', 21: 'AMMO', 22: 'SCROLL', 23: 'RANGEDWEAPON',
    25: 'EFFECT', 26: 'MAPSCROLL',
}
OBJCLASS_PLAYER = 11
OBJCLASS_CHARACTER = 12


class Truncated(Exception):
    pass


class Reader:
    def __init__(self, data, pos=0, end=None):
        self.d = data
        self.pos = pos
        self.end = len(data) if end is None else end

    def take(self, n):
        if self.pos + n > self.end:
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
        """TInputStream string (0x0049ce00): uint8 length + bytes, no NUL."""
        n = self.u8()
        return self.take(n).decode('latin1')

    def remaining(self):
        return self.end - self.pos


class Field:
    """One decoded value: path, file offset, size, value."""
    __slots__ = ('path', 'off', 'size', 'value')

    def __init__(self, path, off, size, value):
        self.path, self.off, self.size, self.value = path, off, size, value


class Decoder:
    def __init__(self, data):
        self.r = Reader(data)
        self.fields = []

    def emit(self, path, off, value):
        self.fields.append(Field(path, off, self.r.pos - off, value))
        return value

    def f(self, path, kind):
        off = self.r.pos
        return self.emit(path, off, getattr(self.r, kind)())

    def raw(self, path, n):
        off = self.r.pos
        return self.emit(path, off, self.r.take(n).hex())

    # ---------------------------------------------------------------- objects

    def object(self, path, version):
        """TObjectInstance::LoadObject (0x00471ce0) for stream version >= 14."""
        r = self.r
        objversion = self.f(f'{path}.objversion', 'i16')
        if objversion < 0:
            return None
        objclass = self.f(f'{path}.objclass', 'i16')
        if objclass < 0:
            return None
        self.emit(f'{path}.class', r.pos, OBJCLASS_NAMES.get(objclass, f'?{objclass}'))
        uid = r.pos
        self.emit(f'{path}.uniqueid', uid, f'{r.u32():#010x}')
        blocksize = self.f(f'{path}.blocksize', 'i16')
        invblocksize = self.f(f'{path}.invblocksize', 'i16') if version >= 14 else 0
        bodystart = r.pos
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
        r = self.r
        if objclass == OBJCLASS_PLAYER:
            self.player(path, objversion, version)
        elif objclass == OBJCLASS_CHARACTER:
            self.character(path, objversion, version)
        else:
            self.instance(path, version)

    def instance(self, path, version):
        """TObjectInstance::Load (0x00472430), stream version >= 9."""
        r = self.r
        off = r.pos
        n = r.u8()
        name = bytes(b & 0x7f for b in r.take(n)).decode('latin1')
        self.emit(f'{path}.name', off, name if n else '<type name>')
        flags = self.f(f'{path}.flags', 'u32')
        self.fields[-1].value = f'{flags:#010x}'
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
        return flags

    def complexobj(self, path, objversion, version):
        """TComplexObject::Load (0x004db930)."""
        if objversion >= 1:
            base = self.f(f'{path}.TObjectInstance.objversion', 'u8')
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
        """TPlayer::Load (0x0051b960) and the tail it calls (0x00529770)."""
        r = self.r
        chv = self.f(f'{path}.TCharacter.objversion', 'u8') if objversion > 3 else objversion
        self.character(path, chv, version)
        if objversion > 3:
            for i in range(5):
                self.f(f'{path}.quickspell[{i}]', 'pstr')
        if objversion > 4:
            n = self.f(f'{path}.spells.count', 'i32')
            for i in range(n):
                self.raw(f'{path}.spells[{i}]', 6)
            if 5 < objversion < 9:
                for i in range(n):
                    self.raw(f'{path}.spells_old[{i}]', 4)
        if objversion > 6:
            for i in range(4):
                self.f(f'{path}.p304[{i}]', 'i32')
        if objversion >= 8:
            for i in range(3):
                self.f(f'{path}.p360[{i}]', 'i32')
        if objversion >= 13:
            self.f(f'{path}.s494', 'pstr')
            self.f(f'{path}.s4c6', 'pstr')
            self.f(f'{path}.p4d8', 'i32')
            self.f(f'{path}.p490', 'i32')
            self.f(f'{path}.p4dc', 'i32')
            self.f(f'{path}.s4f0', 'pstr')
        elif objversion >= 11:
            self.f(f'{path}.s494', 'pstr')
            self.f(f'{path}.s4c6', 'pstr')
            self.f(f'{path}.p4d8', 'i32')
            self.f(f'{path}.s4f0', 'pstr')
        elif objversion >= 10:
            self.raw(f'{path}.s494_fixed', 0x32)
        if objversion >= 13:
            self.f(f'{path}.p36c', 'i32')
            self.f(f'{path}.p370', 'i32')
            for n in ('s378', 's570', 's590', 's5d0'):
                self.f(f'{path}.{n}', 'pstr')
            self.f(f'{path}.p650', 'i32')
            self.f(f'{path}.p654', 'i32')
            if objversion >= 15:
                self.f(f'{path}.p658', 'i32')
                self.f(f'{path}.p65c', 'i32')
        if objversion > 13:
            self.f(f'{path}.tail.name', 'pstr')
            n = self.f(f'{path}.tail.count', 'i32')
            for i in range(n):
                self.f(f'{path}.tail[{i}].a', 'i32')
                k = self.f(f'{path}.tail[{i}].n', 'i32')
                for j in range(k):
                    self.f(f'{path}.tail[{i}].v[{j}]', 'i16')

    # ------------------------------------------------------------- top level

    def save(self):
        r = self.r
        gt = self.f('header.gametime', 'i32')
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
            for i in range(n):
                off = r.pos
                ln = r.u8()
                name = bytes(b ^ 0x80 for b in r.take(ln)).decode('latin1')
                val = r.i32()
                self.emit(f'states[{name}]', off, val)
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
        fcc = r.take(4)
        self.emit('fcc', off, fcc.decode('latin1'))
        version = self.f('version', 'i32')
        if version > 13:
            h = self.f('statehash', 'u32')
            self.fields[-1].value = f'{h:#010x}'
        n = self.f('numobjects', 'i32')
        for i in range(n):
            self.object(f'obj[{i}]', version)
        if r.pos != len(r.d):
            self.raw('TRAILING', len(r.d) - r.pos)


def decode(path):
    data = Path(path).read_bytes()
    d = Decoder(data)
    if data[:4] == b'MAP ':
        d.sector()
    else:
        d.save()
    return d.fields


def dump(path):
    for fl in decode(path):
        print(f'{fl.off:#08x} +{fl.size:<3d} {fl.path} = {fl.value}')


IGNORED = {'header.gametime'}


def diff_fields(fa, fb):
    """Returns a list of difference lines (empty when identical)."""
    out = []
    a = {f.path: f for f in fa}
    b = {f.path: f for f in fb}
    for f in fa:
        if f.path in IGNORED:
            continue
        g = b.get(f.path)
        if g is None:
            out.append(f'- {f.path} = {f.value}  (only in A @ {f.off:#x})')
        elif g.value != f.value:
            out.append(f'~ {f.path}: {f.value}  ->  {g.value}  (A @ {f.off:#x}, B @ {g.off:#x})')
    for g in fb:
        if g.path not in a and g.path not in IGNORED:
            out.append(f'+ {g.path} = {g.value}  (only in B @ {g.off:#x})')
    return out


def diff(pa, pb):
    lines = diff_fields(decode(pa), decode(pb))
    ra, rb = Path(pa).read_bytes(), Path(pb).read_bytes()
    same_bytes = ra == rb or (ra[:4] != b'MAP ' and ra[4:] == rb[4:])
    for ln in lines:
        print(ln)
    print(f'{len(lines)} field difference(s); bytes {"identical" if same_bytes else "differ"}'
          f'{" (ignoring game time)" if ra[:4] != b"MAP " else ""}; sizes {len(ra)} / {len(rb)}')
    return 0 if same_bytes and not lines else 1


def cmpdir(da, db):
    a = {p.name.upper(): p for p in Path(da).iterdir() if p.suffix.upper() == '.DAT'}
    b = {p.name.upper(): p for p in Path(db).iterdir() if p.suffix.upper() == '.DAT'}
    common = sorted(set(a) & set(b))
    same = 0
    for name in common:
        if a[name].read_bytes() == b[name].read_bytes():
            same += 1
            continue
        lines = diff_fields(decode(a[name]), decode(b[name]))
        print(f'{name}: {len(lines)} field difference(s)')
        for ln in lines[:20]:
            print('   ', ln)
    print(f'{same}/{len(common)} byte-identical; only in A: {len(set(a) - set(b))}, '
          f'only in B: {len(set(b) - set(a))}')
    return 0 if same == len(common) else 1


def main(argv):
    if len(argv) >= 3 and argv[1] == 'dump':
        dump(argv[2])
        return 0
    if len(argv) >= 4 and argv[1] == 'diff':
        return diff(argv[2], argv[3])
    if len(argv) >= 4 and argv[1] == 'cmpdir':
        return cmpdir(argv[2], argv[3])
    print(__doc__)
    return 2


if __name__ == '__main__':
    sys.exit(main(sys.argv))
