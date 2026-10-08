#!/usr/bin/env python3
"""List retail TObjectInstance-derived vtables and their stream slots.

Every object class vtable in retail Revenant.exe shares the TObjectInstance
layout up to slot 0x170. This scans .rdata for 93-slot code-pointer arrays
whose SaveInventory slot (0x16c) is TObjectInstance::SaveInventory
(0x00472380; TPlayer keeps it too) and prints, per distinct stream
behaviour, the ObjVersion / Load / Save / LoadInventory / slot-0x170
functions. Evidence for docs/gameflow/forensics/SAVE_GAME.md §11.

usage: object_vtables.py <path to retail Revenant.exe>
"""
import collections
import struct
import sys

SLOT_OBJVERSION = 0x15c
SLOT_LOAD = 0x160
SLOT_SAVE = 0x164
SLOT_LOADINV = 0x168
SLOT_SAVEINV = 0x16c
SLOT_LINKEDINV = 0x170
OI_SAVEINVENTORY = 0x00472380


class PE:
    def __init__(self, path):
        self.d = open(path, 'rb').read()
        pe = struct.unpack_from('<I', self.d, 0x3c)[0]
        nsec = struct.unpack_from('<H', self.d, pe + 6)[0]
        optsz = struct.unpack_from('<H', self.d, pe + 20)[0]
        base = struct.unpack_from('<I', self.d, pe + 24 + 28)[0]
        so = pe + 24 + optsz
        self.secs = {}
        for i in range(nsec):
            name = self.d[so + i * 40:so + i * 40 + 8].rstrip(b'\0').decode()
            vsz, va, rsz, rp = struct.unpack_from('<IIII', self.d, so + i * 40 + 8)
            self.secs[name] = (base + va, vsz, rp, rsz)

    def u32(self, va):
        for sva, vsz, rp, rsz in self.secs.values():
            if sva <= va < sva + rsz:
                return struct.unpack_from('<I', self.d, rp + va - sva)[0]
        raise ValueError(hex(va))

    def objversion(self, fn):
        """Decode `xor eax,eax; ret` or `mov eax,imm32; ret`."""
        rp = self.secs['.text'][2] + fn - self.secs['.text'][0]
        b = self.d[rp:rp + 8]
        if b[:3] == b'\x33\xc0\xc3':
            return 0
        if b[0] == 0xb8 and b[5] == 0xc3:
            return struct.unpack_from('<I', b, 1)[0]
        return None


def main():
    p = PE(sys.argv[1])
    tlo, tsz = p.secs['.text'][0], p.secs['.text'][1]
    rlo, rsz = p.secs['.rdata'][0], p.secs['.rdata'][1]
    groups = collections.defaultdict(list)
    for va in range(rlo, rlo + rsz - 0x200, 4):
        if p.u32(va + SLOT_SAVEINV) != OI_SAVEINVENTORY:
            continue
        slots = [p.u32(va + i * 4) for i in range(SLOT_LINKEDINV // 4 + 1)]
        if not all(tlo <= s < tlo + tsz for s in slots):
            continue
        key = tuple(slots[s // 4] for s in (SLOT_OBJVERSION, SLOT_LOAD, SLOT_SAVE,
                                             SLOT_LOADINV, SLOT_LINKEDINV))
        groups[key].append(va)
    total = sum(len(v) for v in groups.values())
    print(f'{total} object vtables, {len(groups)} distinct stream behaviours')
    for (ov, ld, sv, li, lk), vts in sorted(groups.items(), key=lambda kv: kv[0][1]):
        print(f'objversion={p.objversion(ov)} load={ld:#010x} save={sv:#010x} '
              f'loadinv={li:#010x} slot170={lk:#010x}  x{len(vts)}: '
              + ' '.join(f'{v:#x}' for v in vts[:8]) + (' ...' if len(vts) > 8 else ''))


if __name__ == '__main__':
    main()
