#!/usr/bin/env python3
"""Catalogued retail patches (recon/retail_asm/patches/<name>/manifest.json,
schema `retail-patch.v1`): check a built variant against its manifest, show
its disassembly diff, or apply the manifest's bytes to the baseline.

A manifest lists each change (`file_offset`, `size`, `baseline` and
`patched` bytes as hex), the `allowed_modified_spans` (file offsets, end
exclusive) the build contract enforces, and the variant's SHA-256. The
assembled source (`source_patch`) is the patch; the bytes are its output,
pinned by the SHA, for checks and for applying without NASM.

    retail_patch.py diff  --manifest M --baseline B --variant V [--out DIFF]
    retail_patch.py apply --manifest M --baseline B --out EXE
"""
from __future__ import annotations

import argparse
import difflib
import hashlib
import json
from pathlib import Path

from capstone import CS_ARCH_X86, CS_MODE_32, Cs
from reconstruct import pe_layout


def load(path):
    manifest = json.loads(Path(path).read_text())
    if manifest.get('schema') != 'retail-patch.v1':
        raise ValueError(f'{path}: not a retail-patch.v1 manifest')
    return manifest


def changed_spans(a, b):
    """[start, end) file-offset runs where two equal-length images differ."""
    if len(a) != len(b):
        raise ValueError('images differ in length')
    spans = []
    for i, (x, y) in enumerate(zip(a, b)):
        if x != y:
            if spans and spans[-1][1] == i:
                spans[-1][1] = i + 1
            else:
                spans.append([i, i + 1])
    return spans


def apply(baseline, manifest):
    """The variant's bytes from the baseline's and the manifest's changes;
    each change's baseline bytes must be what is there."""
    if hashlib.sha256(baseline).hexdigest() != manifest['baseline_sha256']:
        raise ValueError('baseline SHA-256 differs from the manifest')
    data = bytearray(baseline)
    for change in manifest['changes']:
        offset, size = int(change['file_offset'], 16), change['size']
        old, new = bytes.fromhex(change['baseline']), bytes.fromhex(change['patched'])
        if len(old) != size or len(new) != size or data[offset:offset + size] != old:
            raise ValueError(f'change at {change["file_offset"]}: baseline bytes differ')
        data[offset:offset + size] = new
    if hashlib.sha256(data).hexdigest() != manifest['executable_sha256']:
        raise ValueError('applied bytes do not give the manifest SHA-256')
    return bytes(data)


def verify(baseline, variant, manifest):
    """Raise unless the variant is the manifest's: its SHA, every changed byte
    inside a declared span, every change's patched bytes in place. Returns
    the changed spans."""
    if hashlib.sha256(baseline).hexdigest() != manifest['baseline_sha256']:
        raise ValueError('baseline SHA-256 differs from the manifest')
    if hashlib.sha256(variant).hexdigest() != manifest['executable_sha256']:
        raise ValueError('variant SHA-256 differs from the manifest')
    allowed = manifest['allowed_modified_spans']
    spans = changed_spans(baseline, variant)
    for start, end in spans:
        if not any(a <= start and end <= b for a, b in allowed):
            raise ValueError(f'undeclared change at file offsets [{start:#x}, {end:#x})')
    for change in manifest['changes']:
        offset, size = int(change['file_offset'], 16), change['size']
        if variant[offset:offset + size] != bytes.fromhex(change['patched']):
            raise ValueError(f'change at {change["file_offset"]} is not in the variant')
    return spans


def disassembly(data):
    """Linear-sweep listing of the executable sections up to each one's
    VirtualSize: 'VA: bytes  insn' (undecodable bytes as `db`). The raw tail
    past it is one line: its zero count, or its bytes."""
    layout = pe_layout(data)
    cs = Cs(CS_ARCH_X86, CS_MODE_32)
    cs.skipdata = True
    lines = []
    for s in layout['sections']:
        if not s['executable']:
            continue
        base = layout['image_base'] + s['rva']
        code = data[s['offset']:s['offset'] + min(s['virtual_size'], s['size'])]
        for address, size, mnemonic, operands in cs.disasm_lite(code, base):
            raw = code[address - base:][:size].hex()
            lines.append(f'{address:08x}: {raw:<20} {mnemonic} {operands}'.rstrip())
        tail = data[s['offset'] + len(code):s['offset'] + s['size']]
        if tail:
            what = f'{len(tail)} zero bytes' if not any(tail) else tail.hex()
            lines.append(f'{base + len(code):08x}..{base + s["size"]:08x}: past VirtualSize, {what}')
    return lines


def header_lines(data, spans, layout):
    """The bytes of `spans` outside executable sections, by file offset."""
    out = []
    for start, end in spans:
        inside = any(s['executable'] and s['offset'] <= start < s['offset'] + s['size']
                     for s in layout['sections'])
        if not inside:
            out.append(f'file {start:08x}..{end:08x}: {data[start:end].hex()}')
    return out


def disassembly_diff(baseline, variant, spans=None):
    """Unified diff of the two listings; bytes outside code are shown for
    `spans` (default: the changed runs), e.g. a manifest's declared spans."""
    layout = pe_layout(baseline)
    spans = changed_spans(baseline, variant) if spans is None else spans
    a = disassembly(baseline) + header_lines(baseline, spans, layout)
    b = disassembly(variant) + header_lines(variant, spans, layout)
    return list(difflib.unified_diff(a, b, 'baseline', 'variant', n=2, lineterm=''))


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    commands = parser.add_subparsers(dest='command', required=True)
    for name in ('diff', 'apply'):
        child = commands.add_parser(name)
        child.add_argument('--manifest', type=Path, required=True)
        child.add_argument('--baseline', type=Path, required=True)
        if name == 'diff':
            child.add_argument('--variant', type=Path, required=True)
            child.add_argument('--out', type=Path)
        else:
            child.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    manifest = load(args.manifest)
    baseline = args.baseline.read_bytes()
    if args.command == 'apply':
        args.out.write_bytes(apply(baseline, manifest))
        print(f'{args.out}: {manifest["executable_sha256"]}')
        return
    variant = args.variant.read_bytes()
    spans = verify(baseline, variant, manifest)
    diff = disassembly_diff(baseline, variant, manifest['allowed_modified_spans'])
    text = '\n'.join(diff) + '\n'
    if args.out:
        args.out.write_text(text)
    else:
        print(text, end='')
    changed = sum(1 for line in diff if line[:1] in '+-' and line[:3] not in ('+++', '---'))
    print(json.dumps(dict(status='pass', variant_sha256=manifest['executable_sha256'],
                          changed_spans=[[hex(a), hex(b)] for a, b in spans],
                          declared_spans=[[hex(a), hex(b)] for a, b in manifest['allowed_modified_spans']],
                          changed_listing_lines=changed)))


if __name__ == '__main__':
    main()
