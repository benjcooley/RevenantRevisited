#!/usr/bin/env python3
"""Which basic blocks of the retail melee functions a kata's cases reached.

    MELEE_COVERAGE=<dir> python3 tools/retail_ab/retail_ab.py melee-attack-choice
    <retail-asm venv python> tools/retail_ab/melee_coverage.py <dir> [--show]

The retail fixture (slots/combat/melee_attack.py) writes the code blocks
it executed to <dir>/<pid>.json; this lists, per function in its COVERED
table, the block starts the asm has (the entry, every branch target, the
instruction after every branch or call) whose first byte no case ran. `--show`
prints each unreached block's first instructions.
"""
from __future__ import annotations

import json
import struct
import sys
from pathlib import Path

from capstone import CS_ARCH_X86, CS_MODE_32, Cs

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[1]
sys.path.insert(0, str(REPO / 'tools' / 'retail_runtime' / 'slots' / 'combat'))
MAIN = Path('/Users/benjamincooley/projects/RevenantRevisited/RevenantRevisited')

# Switch tables inside the covered functions: (table, entries).
JUMP_TABLES = [(0x4d1db8, 4)]


def image():
    for root in (REPO, MAIN):
        exe = root / 'recon' / 'retail_asm' / 'baseline' / 'Revenant.rebuilt.exe'
        if exe.exists():
            data = exe.read_bytes()
            break
    pe = struct.unpack_from('<I', data, 0x3c)[0]
    nsec = struct.unpack_from('<H', data, pe + 6)[0]
    optsz = struct.unpack_from('<H', data, pe + 20)[0]
    base = struct.unpack_from('<I', data, pe + 24 + 28)[0]
    secs = []
    for i in range(nsec):
        o = pe + 24 + optsz + i * 40
        vsz, va, rsz, roff = struct.unpack_from('<IIII', data, o + 8)
        secs.append((base + va, max(vsz, rsz), roff, rsz))

    def read(addr, n):
        for va, sz, roff, rsz in secs:
            if va <= addr < va + sz:
                off = addr - va
                return data[roff + off: roff + min(off + n, rsz)]
        raise ValueError(hex(addr))
    return read


def blocks(read, lo, hi):
    """The block starts of [lo, hi) and the instructions by address."""
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    insns = {i.address: i for i in md.disasm(read(lo, hi - lo), lo)}
    starts = {lo}
    for table, n in JUMP_TABLES:
        if lo <= table < hi:
            for k in range(n):
                starts.add(struct.unpack('<I', read(table + 4 * k, 4))[0])
    for a, ins in insns.items():
        m = ins.mnemonic
        if m.startswith('j') or m == 'call' or m.startswith('ret'):
            nxt = a + ins.size
            if m.startswith('j') and m != 'jmp' or m == 'call':
                starts.add(nxt)
            if m.startswith('j') and ins.op_str.startswith('0x'):
                t = int(ins.op_str, 16)
                if lo <= t < hi:
                    starts.add(t)
    return sorted(s for s in starts if s in insns), insns


def main():
    from melee_attack import COVERED
    folder = Path(sys.argv[1])
    show = '--show' in sys.argv
    reached = set()
    for f in folder.glob('*.json'):
        for start, size in json.loads(f.read_text()):
            reached.update(range(start, start + size))
    read = image()
    total_all = hit_all = 0
    for name, (lo, hi) in COVERED.items():
        starts, insns = blocks(read, lo, hi)
        missing = [s for s in starts if s not in reached]
        total_all += len(starts)
        hit_all += len(starts) - len(missing)
        print(f'{name:22s} {len(starts) - len(missing):4d}/{len(starts):4d}  unreached: '
              + ' '.join(f'{m:#x}' for m in missing))
        if show:
            for m in missing:
                a, lines = m, []
                while a in insns and len(lines) < 3:
                    ins = insns[a]
                    lines.append(f'{ins.mnemonic} {ins.op_str}')
                    a += ins.size
                print(f'    {m:#x}: ' + ' ; '.join(lines))
    print(f'total {hit_all}/{total_all}')


if __name__ == '__main__':
    main()
