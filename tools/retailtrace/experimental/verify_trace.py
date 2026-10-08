#!/usr/bin/env python3
"""Independently check a traced build differs from retail only where allowed.

Reads the traced build's manifest and the real shipped exe and confirms the
changed byte spans are exactly the wrapper blob and the per-hook detours —
no stray edits, no header changes. Rerunnable by anyone with the retail exe.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path


def section_offset(va):
    rva = va - 0x400000
    for r, s, o in [(0x1000, 0x1a2000, 0x1000), (0x1a3000, 0x22000, 0x1a3000),
                    (0x1c5000, 0x24000, 0x1c5000), (0x279000, 0xb000, 0x1e9000)]:
        if r <= rva < r + s:
            return o + rva - r
    raise ValueError(hex(va))


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--retail', type=Path, required=True)
    p.add_argument('--traced', type=Path, required=True)
    p.add_argument('--manifest', type=Path, required=True)
    a = p.parse_args()

    retail = a.retail.read_bytes()
    traced = a.traced.read_bytes()
    manifest = json.loads(a.manifest.read_text())
    if len(retail) != len(traced):
        raise SystemExit('size mismatch')

    reg = manifest['wrapper_region']
    allowed = [(reg['file_offset'], reg['file_offset'] + reg['size'])]
    for h in manifest['hooks']:
        off = section_offset(int(h['target'], 16))
        allowed.append((off, off + h['stolen']))

    changed = [i for i in range(len(retail)) if retail[i] != traced[i]]
    stray = [i for i in changed if not any(lo <= i < hi for lo, hi in allowed)]
    inside_blob = sum(1 for i in changed if allowed[0][0] <= i < allowed[0][1])
    inside_hooks = len(changed) - inside_blob
    report = dict(
        total_changed=len(changed), stray=len(stray),
        wrapper_bytes_changed=inside_blob, detour_bytes_changed=inside_hooks,
        allowed_spans=[[lo, hi] for lo, hi in allowed],
        status='pass' if not stray else 'fail',
    )
    print(json.dumps(report, indent=2))
    if stray:
        raise SystemExit(f'FAIL: {len(stray)} bytes changed outside the allowed spans '
                         f'(first at 0x{stray[0]:08x})')
    print('PASS: only the wrapper blob and hook detours differ from retail.')


if __name__ == '__main__':
    main()
