#!/usr/bin/env python3
"""Build a self-contained retail assembly tree; refuse a non-identical baseline."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess


def build(root, nasm, output, reference=None):
    manifest = json.loads((root/'manifest.json').read_text())
    pending = output.with_name(output.name+'.pending')
    command = [nasm, *manifest['assembly_flags'], 'retail.asm', '-o', str(pending)]
    assembled = subprocess.run(command, cwd=root, capture_output=True, text=True)
    if assembled.returncode:
        (root/'verification.json').write_text(json.dumps(dict(status='fail',stage='assembler',
            byte_identical=False,command=command,assembler_error=assembled.stderr,
            previous_output_preserved=output.exists()),indent=2)+'\n')
        pending.unlink(missing_ok=True)
        raise SystemExit(assembled.stderr)
    data = pending.read_bytes()
    actual = hashlib.sha256(data).hexdigest()
    passed = len(data) == manifest['input_size'] and actual == manifest['input_sha256']
    report = dict(status='pass' if passed else 'fail', byte_identical=passed,
                  expected_sha256=manifest['input_sha256'], actual_sha256=actual,
                  expected_size=manifest['input_size'], actual_size=len(data), command=command,
                  source_sha256={str(p.relative_to(root)): hashlib.sha256(p.read_bytes()).hexdigest()
                                 for p in sorted(root.rglob('*')) if p.suffix in ('.asm','.inc')})
    if reference:
        original = reference.read_bytes()
        report['direct_reference_comparison'] = data == original
        report['reference_sha256'] = hashlib.sha256(original).hexdigest()
        if data != original:
            report['first_mismatch'] = next((i for i, (a,b) in enumerate(zip(data, original)) if a != b), min(len(data),len(original)))
        passed = passed and data == original
    report['status']='pass' if passed else 'fail'
    report['byte_identical']=passed
    (root/'verification.json').write_text(json.dumps(report, indent=2)+'\n')
    if not passed:
        pending.rename(output.with_name(output.name+'.mismatch'))
        raise SystemExit('FAIL: assembled image does not match retail; see verification.json')
    pending.replace(output)
    print(f'PASS: {len(data):,} bytes, SHA-256 {actual}')
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parent)
    parser.add_argument('--nasm', default=shutil.which('nasm'))
    parser.add_argument('--output', type=Path)
    parser.add_argument('--reference', type=Path)
    args = parser.parse_args()
    if not args.nasm:
        parser.error('NASM is required')
    root = args.root.resolve()
    output = (args.output or root/'Revenant.rebuilt.exe').resolve()
    build(root, args.nasm, output, args.reference)
