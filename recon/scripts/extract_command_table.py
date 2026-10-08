#!/usr/bin/env python3
"""Dump the retail script command table (SCommand[] @ 0x005c6e88) from Revenant.exe.

Each entry is 7 dwords: name*, handler, classcontext, classcontext2,
requiresparams, editoronly, usage*. Writes JSON to stdout.

usage: extract_command_table.py data/Revenant.exe > retail_cmds.json
"""
import json
import struct
import sys

TABLE_VA = 0x5C6E88


def main(path):
    exe = open(path, 'rb').read()
    pe = struct.unpack_from('<I', exe, 0x3C)[0]
    nsec = struct.unpack_from('<H', exe, pe + 6)[0]
    optsz = struct.unpack_from('<H', exe, pe + 20)[0]
    base = struct.unpack_from('<I', exe, pe + 24 + 28)[0]
    secs = []
    off = pe + 24 + optsz
    for i in range(nsec):
        _, vsz, va, rsz, rptr = struct.unpack_from('<8sIIII', exe, off + 40 * i)
        secs.append((va + base, max(vsz, rsz), rptr))

    def read(va, n):
        for sva, size, rptr in secs:
            if sva <= va < sva + size:
                return exe[rptr + va - sva: rptr + va - sva + n]
        raise ValueError(hex(va))

    def cstr(va):
        b = read(va, 1024)
        return b[:b.index(0)].decode('latin1')

    cmds = []
    va = TABLE_VA
    while True:
        name, fn, cc, cc2, req, ed, usage = struct.unpack('<IIiiiiI', read(va, 28))
        if name == 0:
            break
        cmds.append(dict(name=cstr(name), addr='0x%08x' % fn, cc=cc, cc2=cc2,
                         req=req, ed=ed, usage=cstr(usage)))
        va += 28
    json.dump(cmds, sys.stdout, indent=1)


if __name__ == '__main__':
    main(sys.argv[1])
