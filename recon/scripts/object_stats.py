#!/usr/bin/env python3
"""List the object classes and stats retail registers in code.

Retail registers each object class with a static TObjectClass (constructor
0x004742e0: name, class id, flags, base class) and each code-defined stat
with a static SStatEntry (constructor 0x00473e40: class, name, unique id,
index, default, min, max, object-stat flag), the DEFOBJSTAT / DEFSTAT
macros of src/object.h. The pushes before each call give the arguments;
an object stat's index is its position in the class's object-stat array,
which is the order a sector or save file lists the stats in.

usage: object_stats.py <path to retail Revenant.exe> [class name ...]
"""
import collections
import re
import struct
import sys

sys.path.insert(0, __file__.rsplit('/', 1)[0])
from object_vtables import PE  # noqa: E402

CLASS_CTOR = 0x004742e0
STAT_CTOR = 0x00473e40


def cstr(p, va):
    for sva, vsz, rp, rsz in p.secs.values():
        if sva <= va < sva + rsz:
            o = rp + va - sva
            s = p.d[o:p.d.index(b'\0', o)]
            return s.decode('latin1') if s and all(32 <= c < 127 for c in s) else None
    return None


def calls_to(p, target):
    tva, tvsz, trp, trsz = p.secs['.text']
    tb = p.d[trp:trp + trsz]
    for m in re.finditer(rb'\xe8(....)', tb, re.S):
        if (tva + m.start() + 5 + struct.unpack('<i', m.group(1))[0]) & 0xffffffff == target:
            yield tva + m.start(), tb, m.start()


def pushes_before(tb, at, window=48):
    """Immediate pushes (push imm8 / push imm32) and `mov ecx, imm32` before a call, in order."""
    code = tb[at - window:at]
    out, ecx = [], None
    i = 0
    while i < len(code):
        b = code[i]
        if b == 0x6a and i + 1 < len(code):
            out.append(struct.unpack('<b', code[i + 1:i + 2])[0]); i += 2; continue
        if b == 0x68 and i + 4 < len(code):
            out.append(struct.unpack('<I', code[i + 1:i + 5])[0]); i += 5; continue
        if b == 0xb9 and i + 4 < len(code):
            ecx = struct.unpack('<I', code[i + 1:i + 5])[0]; i += 5; continue
        if b in (0xc3, 0x90, 0xcc):
            out, ecx = [], None
        i += 1
    return out, ecx


def main():
    p = PE(sys.argv[1])
    wanted = {a.upper() for a in sys.argv[2:]}

    classes = {}
    for site, tb, off in calls_to(p, CLASS_CTOR):
        args, ecx = pushes_before(tb, off)
        if ecx is None or len(args) < 3:
            continue
        # pushed last-to-first: base, flags, id, name
        name, cid = cstr(p, args[-1]), args[-2]
        classes[ecx] = (name, cid)

    stats = collections.defaultdict(list)
    for site, tb, off in calls_to(p, STAT_CTOR):
        args, ecx = pushes_before(tb, off)
        if len(args) < 8:
            continue
        cls, name, uid, idx, df, mn, mx, objstat = (args[-1], args[-2], args[-3], args[-4],
                                                    args[-5], args[-6], args[-7], args[-8])
        stats[cls].append((idx, cstr(p, name), cstr(p, uid), df, mn, mx, objstat))

    for cls, (cname, cid) in sorted(classes.items(), key=lambda kv: kv[1][1]):
        if wanted and (cname or '').upper() not in wanted:
            continue
        print(f'{cname} (id {cid}, TObjectClass @ {cls:#x})')
        for idx, name, uid, df, mn, mx, objstat in sorted(stats.get(cls, [])):
            kind = 'objstat' if objstat else 'stat'
            print(f'    {kind:7s} [{idx:3d}] {name:16s} {uid:5s} def={df} min={mn} max={mx}')


if __name__ == '__main__':
    main()
