#!/usr/bin/env python3
"""Apply the lab's 3-byte Win98 compatibility patch to a traced build.

The dosbox-x lab launches ``C:\\REVENANT\\Rev98.exe`` — the shipped
``Revenant.exe`` with a tiny patch that renames two imported DLLs
(``_INMM.dll`` -> ``WINMM.dll`` and ``mss32.dll`` -> ``mss98.dll``) so the
import names match the DLLs beside the exe. The patch is derived here by
diffing the shipped exe against the lab's ``Rev98.exe`` (never guessed), and
the same byte edits are applied to the traced build, which ships as
``RevTrace.exe``. The guest's ``Rev98.exe`` and ``Revenant.exe`` are never
touched.
"""
from __future__ import annotations

import argparse
from pathlib import Path


def diff_bytes(a: bytes, b: bytes):
    if len(a) != len(b):
        raise SystemExit(f'size mismatch {len(a)} vs {len(b)}; not the same build')
    return [(i, a[i], b[i]) for i in range(len(a)) if a[i] != b[i]]


def main():
    p = argparse.ArgumentParser(description=__doc__,
                                formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('--retail', type=Path, required=True, help='shipped Revenant.exe')
    p.add_argument('--rev98', type=Path, required=True, help="lab's Rev98.exe")
    p.add_argument('--traced', type=Path, required=True, help='Revenant.hooked.exe')
    p.add_argument('--out', type=Path, required=True, help='RevTrace.exe to write')
    a = p.parse_args()

    retail = a.retail.read_bytes()
    rev98 = a.rev98.read_bytes()
    patch = diff_bytes(retail, rev98)
    print(f'compatibility patch: {len(patch)} byte(s)')
    for off, old, new in patch:
        print(f'  0x{off:08x}: {old:02x} -> {new:02x}')
    if len(patch) > 16:
        raise SystemExit('refusing: that is not the small compatibility patch')

    traced = bytearray(a.traced.read_bytes())
    if len(traced) != len(retail):
        raise SystemExit('traced build is a different size from retail')
    for off, old, new in patch:
        if traced[off] != old:
            raise SystemExit(f'traced build already differs at 0x{off:08x}; patch target moved')
        traced[off] = new
    a.out.write_bytes(bytes(traced))
    print(f'wrote {a.out} ({len(traced):,} bytes)')


if __name__ == '__main__':
    main()
