#!/usr/bin/env python3
"""Example variant: put freestanding C in verified zero section padding."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import struct
import subprocess


def aligned(value, alignment):
    return (value+alignment-1)//alignment*alignment


def coff_text(path):
    data = path.read_bytes()
    machine, count = struct.unpack_from('<HH', data)
    optional_size = struct.unpack_from('<H', data, 16)[0]
    if machine != 0x14c:
        raise ValueError('Expected i386 COFF')
    chunks = []
    for i in range(count):
        pos = 20+optional_size+i*40
        name = data[pos:pos+8].split(b'\0')[0]
        size, offset = struct.unpack_from('<II', data, pos+16)
        relocations = struct.unpack_from('<H', data, pos+32)[0]
        if name == b'.text':
            if relocations:
                raise ValueError('Example C routine must have no unresolved relocations')
            chunks.append(data[offset:offset+size])
    if len(chunks) != 1 or not chunks[0]:
        raise ValueError('Expected one nonempty .text section')
    return chunks[0]


def create_variant(baseline, destination, clang, nasm):
    original = baseline.read_bytes()
    destination.mkdir(parents=True, exist_ok=False)
    pe = struct.unpack_from('<I', original, 0x3c)[0]
    count = struct.unpack_from('<H', original, pe+6)[0]
    optional_size = struct.unpack_from('<H', original, pe+20)[0]
    optional = pe+24
    if original[pe:pe+4] != b'PE\0\0' or struct.unpack_from('<H', original, optional)[0] != 0x10b:
        raise ValueError('Expected PE32')
    table = optional+optional_size
    section_alignment, file_alignment = struct.unpack_from('<II', original, optional+32)
    image_base = struct.unpack_from('<I', original, optional+28)[0]
    old_entry = struct.unpack_from('<I', original, optional+16)[0]
    size_headers = struct.unpack_from('<I', original, optional+60)[0]
    sections = []
    for i in range(count):
        p = table+40*i
        virtual_size, rva, size, offset = struct.unpack_from('<IIII', original, p+8)
        sections.append(dict(rva=rva, virtual_size=virtual_size, size=size, offset=offset, header=p))
    # The retail header contains nonzero material immediately after its table.
    # Do not overwrite it to add a fifth descriptor. A small probe fits in a
    # section's already allocated, verified-zero tail without moving any data.
    candidates = []
    for s in sections:
        local = aligned(s['virtual_size'], 16)
        if local+128 <= s['size'] and not any(original[s['offset']+local:s['offset']+s['size']]):
            candidates.append((s,local))
    if not candidates:
        raise ValueError('No sufficiently large verified-zero section padding')
    selected, local = candidates[-1]
    new_rva = selected['rva']+local
    new_offset = selected['offset']+local
    source = destination/'probe.c'
    source.write_text('/* Freestanding: no CRT, imports, allocation, or FPU/SIMD operations. */\n'
                      '__declspec(noinline) unsigned int retail_probe_marker(void) {\n'
                      '    return 0x524C4142u; /* RLAB */\n}\n')
    obj = destination/'probe.obj'
    command = [clang, '--target=i686-pc-windows-msvc', '-Os', '-ffreestanding',
               '-fno-stack-protector', '-fomit-frame-pointer', '-fno-unwind-tables',
               '-fno-asynchronous-unwind-tables', '-c', str(source), '-o', str(obj)]
    subprocess.run(command, check=True)
    code = coff_text(obj)
    (destination/'probe.text.bin').write_bytes(code)
    asm = destination/'lab.asm'
    asm.write_text(f'''BITS 32
ORG 0x{image_base+new_rva:x}
lab_entry:
    pushfd
    pushad
    call lab_c_probe
    call .position
.position:
    pop edx
    mov [edx + lab_result - .position], eax
    popad
    popfd
    jmp 0x{image_base+old_entry:x}
    align 16, db 0x90
lab_result:
    dd 0
    align 16, db 0x90
lab_c_probe:
    incbin "probe.text.bin"
    align 4, db 0
lab_metadata:
    dd lab_result - $$
    dd lab_c_probe - $$
    dd 0x524c4142
''')
    blob = destination/'lab.bin'
    subprocess.run([nasm, '-f', 'bin', '-Ox', 'lab.asm', '-o', 'lab.bin'], cwd=destination, check=True)
    added = blob.read_bytes()
    if local+len(added) > selected['size']:
        raise ValueError('Probe exceeds reserved zero padding')
    result = bytearray(original)
    result[new_offset:new_offset+len(added)] = added
    struct.pack_into('<I', result, optional+16, new_rva)
    struct.pack_into('<I', result, optional+64, 0)  # Checksum is optional for this user-mode image.
    struct.pack_into('<I', result, selected['header']+8, local+len(added))
    old_flags = struct.unpack_from('<I', original, selected['header']+36)[0]
    struct.pack_into('<I', result, selected['header']+36, old_flags | 0xe0000000)
    for s in sections:
        a = s['offset']
        b = a+(min(s['virtual_size'],s['size']) if s is selected else s['size'])
        if result[a:b] != original[a:b]:
            raise AssertionError('Original section contents changed outside reserved padding')
    output = destination/'Revenant.c-probe.exe'
    output.write_bytes(result)
    marker_local, c_local, signature = struct.unpack_from('<III', added, len(added)-12)
    if signature != 0x524c4142 or added[marker_local:marker_local+4] != bytes(4):
        raise AssertionError('Invalid assembler-produced lab metadata')
    manifest = dict(status='built_not_guest_run', byte_identical_to_retail=False,
        baseline_sha256=hashlib.sha256(original).hexdigest(), variant_sha256=hashlib.sha256(result).hexdigest(),
        original_section_payloads_byte_identical=True, original_overlay_preserved=True,
        original_entry_rva=old_entry, new_entry_rva=new_rva, image_base=image_base,
        allocation='verified_zero_section_tail', selected_section_header=selected['header'],
        probe_file_offset=new_offset, probe_size=len(added),
        marker_rva=new_rva+marker_local, c_rva=new_rva+c_local, expected_marker=0x524c4142,
        compiler_command=command, c_text_hex=code.hex(),
        scope='Experimental variant only. Adds C marker and preserving trampoline in verified zero section padding; original referenced section payloads/overlay/addresses unchanged. Changes entrypoint, that section descriptor and optional checksum. Baseline separately hash-verified. Guest startup/game behavior not yet exercised.')
    (destination/'manifest.json').write_text(json.dumps(manifest, indent=2)+'\n')
    print(f'C probe variant built: {output}')


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('baseline', type=Path)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--clang', default=shutil.which('clang'))
    p.add_argument('--nasm', default=shutil.which('nasm'))
    a = p.parse_args()
    if not a.clang or not a.nasm:
        p.error('clang and NASM are required')
    create_variant(a.baseline.resolve(), a.output.resolve(), a.clang, a.nasm)
