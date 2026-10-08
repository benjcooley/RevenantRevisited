#!/usr/bin/env python3
"""Execute only the added probe/trampoline, stop before retail's original entry."""
import argparse
import json
from pathlib import Path
import struct
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import (UC_X86_REG_EAX, UC_X86_REG_EBX, UC_X86_REG_ECX,
    UC_X86_REG_EDX, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP,
    UC_X86_REG_ESP, UC_X86_REG_EFLAGS)


def verify(root):
    manifest = json.loads((root/'manifest.json').read_text())
    data = (root/'Revenant.c-probe.exe').read_bytes()
    pe = struct.unpack_from('<I', data, 0x3c)[0]
    optional = pe+24
    count = struct.unpack_from('<H', data, pe+6)[0]
    optional_size = struct.unpack_from('<H', data, pe+20)[0]
    base = struct.unpack_from('<I', data, optional+28)[0]
    image_size, header_size = struct.unpack_from('<II', data, optional+56)
    emu = Uc(UC_ARCH_X86, UC_MODE_32)
    emu.mem_map(base, (image_size+4095)//4096*4096)
    emu.mem_write(base, data[:header_size])
    for i in range(count):
        p = optional+optional_size+40*i
        rva, raw_size, raw_offset = struct.unpack_from('<III', data, p+12)
        if raw_size:
            emu.mem_write(base+rva, data[raw_offset:raw_offset+raw_size])
    stack = 0x10000000
    emu.mem_map(stack, 0x20000)
    registers = [UC_X86_REG_EAX, UC_X86_REG_EBX, UC_X86_REG_ECX, UC_X86_REG_EDX,
                 UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP]
    initial = {r:0x11223300+i for i,r in enumerate(registers)}
    initial[UC_X86_REG_ESP] = stack+0x10000
    initial[UC_X86_REG_EFLAGS] = 0x246
    for r,v in initial.items():
        emu.reg_write(r,v)
    original_entry = base+manifest['original_entry_rva']
    reached = []
    def stop_before_retail(uc, address, size, context):
        if address == original_entry:
            reached.append(address)
            uc.emu_stop()
    emu.hook_add(UC_HOOK_CODE, stop_before_retail)
    emu.emu_start(base+manifest['new_entry_rva'], base+image_size, count=64)
    actual = {r:emu.reg_read(r) for r in initial}
    marker = struct.unpack('<I', emu.mem_read(base+manifest['marker_rva'],4))[0]
    passed = bool(reached) and actual == initial and marker == manifest['expected_marker']
    report = dict(status='pass' if passed else 'fail', reached_original_entry=bool(reached),
        original_entry_executed=False, register_and_flags_preserved=actual==initial,
        initial_registers={str(k):v for k,v in initial.items()},
        final_registers={str(k):v for k,v in actual.items()},
        marker=marker, expected_marker=manifest['expected_marker'],
        scope='Actual added x86 C function and trampoline executed in Unicorn. No import/OS stubs and no original retail startup instruction executed. This proves probe ABI/control transfer, not Windows98 game startup.')
    (root/'execution-verification.json').write_text(json.dumps(report,indent=2)+'\n')
    if not passed:
        raise SystemExit('C probe execution failed')
    print('PASS: compiled C marker executes; registers/flags/ESP preserved; returns to original entry.')


if __name__ == '__main__':
    p=argparse.ArgumentParser();p.add_argument('directory',type=Path);a=p.parse_args();verify(a.directory.resolve())
