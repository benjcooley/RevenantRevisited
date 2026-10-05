#!/usr/bin/env python3
"""Map retail object vtables to their class.def builder names.

Retail registers one TObjectBuilder per C++ object class (DEFINE_BUILDER /
REGISTER_BUILDER, src/object.h). The registration (a static initializer)
pushes the builder name, calls the TObjectBuilder constructor and stores
the builder's vtable; the builder vtable's two Build overrides `new` the
object, and the inlined object constructor stores the object vtable.

Ghidra (no auto-analysis) has no functions at the Build overrides, which
only .rdata points to, so this works on raw bytes:
  object vtable store site S  ->  nearest .rdata-referenced code address
  B <= S (the Build override)  ->  the .rdata slot holding B (builder
  vtable)  ->  `mov [imm32], builder vtable` sites  ->  the string pushed in
  the preceding 48 bytes.

usage: object_builders.py <path to retail Revenant.exe>
"""
import bisect
import collections
import re
import struct
import sys

sys.path.insert(0, __file__.rsplit('/', 1)[0])
from object_vtables import PE, SLOT_LOAD, SLOT_SAVE, SLOT_SAVEINV, OI_SAVEINVENTORY  # noqa: E402

MAX_BUILD_SIZE = 0x180


def object_vtables(p):
    tlo, tsz = p.secs['.text'][0], p.secs['.text'][1]
    rlo, rsz = p.secs['.rdata'][0], p.secs['.rdata'][1]
    return {va for va in range(rlo, rlo + rsz - 0x200, 4)
            if p.u32(va + SLOT_SAVEINV) == OI_SAVEINVENTORY
            and all(tlo <= p.u32(va + i * 4) < tlo + tsz for i in range(93))}


def cstr(p, va):
    for sva, vsz, rp, rsz in p.secs.values():
        if sva <= va < sva + rsz:
            o = rp + va - sva
            e = p.d.find(b'\0', o)
            s = p.d[o:e]
            if 0 < len(s) < 48 and all(32 <= c < 127 for c in s):
                return s.decode()
    return None


def main():
    p = PE(sys.argv[1])
    tva, tvsz, trp, trsz = p.secs['.text']
    tb = p.d[trp:trp + trsz]
    rva, rvsz, rrp, rrsz = p.secs['.rdata']
    vts = object_vtables(p)

    # .rdata dwords that point into .text: candidate virtual function entries
    code_ptrs = collections.defaultdict(list)       # code address -> rdata slots
    for off in range(0, rrsz - 4, 4):
        v = struct.unpack_from('<I', p.d, rrp + off)[0]
        if tva <= v < tva + trsz:
            code_ptrs[v].append(rva + off)
    entries = sorted(code_ptrs)

    # mov dword [imm32], imm32 (C7 05) sites: builder-vtable stores
    abs_stores = collections.defaultdict(list)     # imm -> site
    for m in re.finditer(rb'\xc7\x05(....)(....)', tb, re.S):
        abs_stores[struct.unpack('<I', m.group(2))[0]].append(tva + m.start())

    def builder_name(bvt):
        for site in abs_stores.get(bvt, []):
            o = site - tva
            window = tb[max(0, o - 48):o]
            for pm in reversed(list(re.finditer(rb'\x68(....)', window, re.S))):
                s = cstr(p, struct.unpack('<I', pm.group(1))[0])
                if s:
                    return s
        return None

    names = collections.defaultdict(set)
    for vt in vts:
        needle = struct.pack('<I', vt)
        for m in re.finditer(re.escape(needle), tb):
            site = tva + m.start()
            if tb[m.start() - 2] != 0xc7:           # mov [reg], imm32
                continue
            i = bisect.bisect_right(entries, site) - 1
            if i < 0 or site - entries[i] > MAX_BUILD_SIZE:
                continue
            build = entries[i]
            for slot in code_ptrs[build]:
                for back in (0, 4, 8):
                    n = builder_name(slot - back)
                    if n:
                        names[vt].add(n)
    for vt in sorted(vts):
        print(f'{vt:#x} load={p.u32(vt + SLOT_LOAD):#x} save={p.u32(vt + SLOT_SAVE):#x} '
              f'builders={sorted(names.get(vt, []))}')


if __name__ == '__main__':
    main()
