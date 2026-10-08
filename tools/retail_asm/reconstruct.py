#!/usr/bin/env python3
"""Lossless PE32 -> NASM source. The executable is the only reconstruction input."""
from __future__ import annotations
import argparse
import collections
import hashlib
import json
from pathlib import Path
import re
import shutil
import struct
import subprocess
import tempfile

from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_GRP_JUMP, CS_GRP_CALL
from capstone.x86 import X86_OP_IMM


def digest(data):
    return hashlib.sha256(data).hexdigest()


def pe_layout(data):
    if len(data) < 64 or data[:2] != b'MZ':
        raise ValueError('Input is not a DOS/PE image')
    pe = struct.unpack_from('<I', data, 0x3c)[0]
    if data[pe:pe+4] != b'PE\0\0':
        raise ValueError('Missing PE signature')
    machine, count = struct.unpack_from('<HH', data, pe+4)
    optional_size = struct.unpack_from('<H', data, pe+20)[0]
    optional = pe+24
    if machine != 0x14c or struct.unpack_from('<H', data, optional)[0] != 0x10b:
        raise ValueError('Only x86 PE32 is supported')
    image_base = struct.unpack_from('<I', data, optional+28)[0]
    entry = struct.unpack_from('<I', data, optional+16)[0]
    sections = []
    for index in range(count):
        pos = optional+optional_size+40*index
        name = data[pos:pos+8].split(b'\0')[0].decode('ascii', 'replace')
        virtual_size, rva, size, offset = struct.unpack_from('<IIII', data, pos+8)
        flags = struct.unpack_from('<I', data, pos+36)[0]
        if offset+size > len(data):
            raise ValueError(f'Truncated section {index}')
        sections.append(dict(index=index, name=name, rva=rva, virtual_size=virtual_size,
                             offset=offset, size=size, flags=flags, executable=bool(flags & 0x20000000)))
    spans = sorted((s for s in sections if s['size']), key=lambda s: s['offset'])
    cursor = 0
    regions = []
    for s in spans:
        if s['offset'] < cursor:
            raise ValueError('Overlapping raw PE sections are not supported')
        if s['offset'] > cursor:
            regions.append(dict(name=f'file_{cursor:08x}', offset=cursor, size=s['offset']-cursor,
                                va=cursor, executable=False, kind='headers_or_gap'))
        regions.append(dict(s, name=f'section_{s["index"]}', original_name=s['name'],
                            va=image_base+s['rva'], kind='section'))
        cursor = s['offset']+s['size']
    if cursor < len(data):
        regions.append(dict(name='overlay', offset=cursor, size=len(data)-cursor,
                            va=cursor, executable=False, kind='overlay'))
    return dict(pe_offset=pe, optional_offset=optional, optional_size=optional_size,
                image_base=image_base, entry_rva=entry, sections=sections, regions=regions)


def db(data):
    return 'db ' + ', '.join(f'0x{x:02x}' for x in data)


def nasm_instruction(insn):
    """Keep relative branch widths. Other ambiguous encodings are certified below."""
    mnemonic = insn.mnemonic
    operands = insn.op_str.replace(' ptr ', ' ')
    operands = re.sub(r'\btbyte\b', 'tword', operands)
    operands = re.sub(r'\bxword\b', 'tword', operands)
    relative = None
    if (insn.group(CS_GRP_JUMP) or insn.group(CS_GRP_CALL)) and insn.operands and insn.operands[0].type == X86_OP_IMM:
        relative = insn.operands[0].imm & 0xffffffff
        delta = (relative-insn.address) & 0xffffffff
        if delta >= 0x80000000:
            delta -= 0x100000000
        width = ''
        op = bytes(insn.bytes)
        # Prefixes don't alter displacement width, except operand-size 66.
        prefix_length = 0
        while prefix_length < len(op) and op[prefix_length] in (0x66, 0x67, 0xf0, 0xf2, 0xf3, 0x2e, 0x36, 0x3e, 0x26, 0x64, 0x65):
            prefix_length += 1
        opcode = op[prefix_length:] if prefix_length < len(op) else op
        if mnemonic == 'jmp' or mnemonic.startswith('j'):
            width = 'short ' if opcode[0] == 0xeb or 0x70 <= opcode[0] <= 0x7f or mnemonic in ('jcxz', 'jecxz') else 'near '
        operands = f'{width}($ + ({delta}))'
    return f'{mnemonic} {operands}'.rstrip(), relative


def decode_region(data, region):
    cs = Cs(CS_ARCH_X86, CS_MODE_32)
    cs.detail = True
    # Explicit skip records for undecodable bytes. Linear sweep is not a proof
    # that every decoded record represents reachable code rather than inline data.
    cs.skipdata = True
    body_size = min(region.get('virtual_size', region['size']), region['size'])
    records = []
    for insn in cs.disasm(data[region['offset']:region['offset']+body_size], region['va']):
        raw = bytes(insn.bytes)
        if insn.id == 0:
            records.append(dict(va=insn.address, raw=raw, candidate=None,
                                disassembly='undecodable byte / possible inline data', reason='undecodable'))
        else:
            candidate, target = nasm_instruction(insn)
            records.append(dict(va=insn.address, raw=raw, candidate=candidate,
                                target=target, disassembly=f'{insn.mnemonic} {insn.op_str}'.rstrip()))
    if sum(len(r['raw']) for r in records) != body_size:
        raise ValueError('Decoder failed to account for an executable region')
    return records, body_size


def certify(records, nasm, scratch, batch_size=4000):
    """Batch independent assembly records into fixed cells, compare actual output."""
    for first in range(0, len(records), batch_size):
        batch = records[first:first+batch_size]
        rejected = {}
        source, binary = scratch/'probe.asm', scratch/'probe.bin'
        while True:
            lines = ['BITS 32', 'section probe start=0 vstart=0 align=1']
            for i, record in enumerate(batch):
                line = record['candidate'] if record['candidate'] and i not in rejected else db(record['raw'])
                lines.extend([f'record_{i}:', '    '+line, f'    times 32-($-record_{i}) db 0'])
            source.write_text('\n'.join(lines)+'\n')
            result = subprocess.run([nasm, '-f', 'bin', '-Ox', str(source), '-o', str(binary)], capture_output=True, text=True)
            if result.returncode == 0:
                break
            indices = set()
            for line in result.stderr.splitlines():
                match = re.search(r'probe\.asm:(\d+): error:', line)
                if match:
                    i = (int(match.group(1))-3)//3
                    if 0 <= i < len(batch) and i not in rejected:
                        indices.add(i)
                        rejected[i] = line.split('error:', 1)[1].strip()
            if not indices:
                raise RuntimeError('Unexpected NASM certification failure: '+result.stderr[:2000])
        output = binary.read_bytes()
        if len(output) != 32*len(batch):
            raise ValueError('Incorrect certification cell length')
        for i, record in enumerate(batch):
            expected = record['raw'] + bytes(32-len(record['raw']))
            actual = output[i*32:(i+1)*32]
            record['verified_mnemonic'] = bool(record['candidate'] and i not in rejected and actual == expected)
            if not record['verified_mnemonic']:
                record.setdefault('reason', 'assembler_syntax' if i in rejected else 'encoding_differs')
                if i in rejected:
                    record['assembler_error'] = rejected[i]
        print(f'certified {min(first+len(batch), len(records))}/{len(records)} records', flush=True)


def emit_bytes(stream, data, offset, va):
    for pos in range(0, len(data), 16):
        raw = data[pos:pos+16]
        ascii_text = ''.join(chr(x) if 32 <= x < 127 else '.' for x in raw).replace(';', '.')
        # A backslash at end of even a NASM comment continues the next line.
        # The closing delimiter keeps embedded paths from eating byte rows.
        stream.write(f'    {db(raw)} ; file {offset+pos:08x}, VA {va+pos:08x} | {ascii_text} |\n')


def reconstruct(input_path, output_dir, nasm):
    data = input_path.read_bytes()
    layout = pe_layout(data)
    output_dir.mkdir(parents=True, exist_ok=False)
    asm_dir = output_dir/'sections'
    asm_dir.mkdir()
    counts = collections.Counter()
    reasons = collections.Counter()
    all_records = []
    with tempfile.TemporaryDirectory(prefix='retail-asm-certify-') as tmp:
        for region in layout['regions']:
            if region['executable']:
                records, body_size = decode_region(data, region)
                certify(records, nasm, Path(tmp))
            else:
                records, body_size = [], 0
            filename = f'{region["offset"]:08x}_{region["name"]}.asm'
            with (asm_dir/filename).open('w') as stream:
                stream.write('; Generated from executable bytes; fixed file and virtual layout.\n')
                stream.write(f'section {region["name"]} start=0x{region["offset"]:x} vstart=0x{region["va"]:x} align=1\n')
                for record in records:
                    raw = record['raw']
                    label = f'va_{record["va"]:08x}'
                    stream.write(f'{label}: ; original {raw.hex(" ")}\n')
                    if record['verified_mnemonic']:
                        stream.write('    '+record['candidate']+'\n')
                        stream.write(f'    times {len(raw)}-($-{label}) db 0\n')
                        counts['mnemonic_records'] += 1
                        counts['mnemonic_bytes'] += len(raw)
                    else:
                        stream.write(f'    {db(raw)} ; {record["disassembly"]} [{record["reason"]}]\n')
                        counts['explicit_encoding_records'] += 1
                        counts['explicit_encoding_bytes'] += len(raw)
                        reasons[record['reason']] += 1
                    all_records.append(dict(va=f'0x{record["va"]:08x}',
                        file_offset=region['offset']+record['va']-region['va'],
                        bytes=raw.hex(), disassembly=record['disassembly'],
                        source=record['candidate'] if record['verified_mnemonic'] else db(raw),
                        verified_mnemonic=record['verified_mnemonic'], reason=record.get('reason')))
                tail = data[region['offset']+body_size:region['offset']+region['size']]
                emit_bytes(stream, tail, region['offset']+body_size, region['va']+body_size)
                counts['data_and_padding_bytes'] += len(tail)
            region['assembly_file'] = 'sections/'+filename
    main = ['; Original retail PE32 image. No incbin of the executable or section blobs.', 'BITS 32']
    main += [f'%include "{r["assembly_file"]}"' for r in layout['regions']]
    (output_dir/'retail.asm').write_text('\n'.join(main)+'\n')
    (output_dir/'instructions.jsonl').write_text(''.join(json.dumps(r)+'\n' for r in all_records))
    version = subprocess.check_output([nasm, '-v'], text=True).strip()
    manifest = dict(schema=1, input_name=input_path.name, input_sha256=digest(data), input_size=len(data),
                    assembler=version, assembly_flags=['-f', 'bin', '-Ox'], layout=layout,
                    coverage=dict(counts), explicit_encoding_reasons=dict(reasons),
                    scope='Complete lossless file representation. Executable regions use linear sweep; decoded records may include inline data. Mnemonics are individually reassembled and checked; ambiguous encodings/undecodable bytes use explicit db. This is not recovered C++ or a proven function/control-flow inventory.',
                    source_sha256={str(p.relative_to(output_dir)): digest(p.read_bytes()) for p in sorted(output_dir.rglob('*.asm'))})
    (output_dir/'manifest.json').write_text(json.dumps(manifest, indent=2)+'\n')
    build_source = Path(__file__).with_name('build.py')
    shutil.copyfile(build_source, output_dir/'build.py')
    subprocess.run(['python3', str(output_dir/'build.py'), '--nasm', nasm], check=True)
    print(json.dumps(dict(coverage=manifest['coverage'], sha256=manifest['input_sha256'])), flush=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('executable', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--nasm', default=shutil.which('nasm'))
    args = parser.parse_args()
    if not args.nasm:
        parser.error('NASM is required')
    reconstruct(args.executable.resolve(), args.output.resolve(), args.nasm)
