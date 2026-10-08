#!/usr/bin/env python3
"""Build a traced retail Revenant.exe from a declarative hook spec.

This is the hook-spec compiler RETAIL_TRACE.md §3 describes. It grew out of
the single fixed-message detour this file used to hold: that experiment
stole a function's prologue, jumped to one hand-written wrapper in a
section's zero tail, and logged one hard-coded line through the game's
Win32 imports. The same mechanism is here, generalised:

- a spec (``tools/retail_asm/trace_hooks.py`` by default) lists hooks
  (address, handler C, what each handler captures) and one shared C
  runtime;
- every wrapper is generated into one NASM translation unit and every
  handler into one C translation unit, compiled and linked **once** into a
  single relocatable blob;
- the blob is self-relocated (via its own ``.reloc`` table) into a
  verified-zero section tail, so no PE header field changes and the only
  differences from the baseline are the per-hook detour spans and the one
  wrapper blob;
- the tool verifies exactly that.

External references from the blob to the fixed retail image (the game's
CRT ``sprintf``, the kernel32 IAT slots, the per-hook return addresses) are
emitted as absolute immediates / indirect calls, so they carry no
relocation and survive the move unchanged. Only blob-internal data
references are relocated.

Design notes that matter when extending this:

- **Prologue theft** copies whole instructions until at least five bytes
  are taken. A PC-relative branch/call/return inside that window cannot be
  copied verbatim to another address; the tool rewrites a trailing
  ``E8``/``E9`` to an absolute, relocation-free sequence and refuses any
  other relative control flow with a clear error (RETAIL_TRACE.md §3).
- **The wrapper** saves flags and all GPRs (``pushfd``/``pushad``), hands
  the handler one pointer to the saved register block, restores
  everything, replays the stolen bytes and returns absolutely
  (``push imm32``/``ret``) so no register — not even EAX — is clobbered.
- **Handlers only read** game memory; they never write game state.

Run: ``function_hook.py <baseline.exe> --output <dir>`` (``--spec`` to pick
another spec module). The baseline is any byte-identical retail image; the
tool patches a copy and never touches the input.
"""
from __future__ import annotations

import argparse
import hashlib
import importlib.util
import json
from pathlib import Path
import shutil
import struct
import subprocess
import sys

from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_GRP_JUMP, CS_GRP_CALL, CS_GRP_RET
from capstone.x86 import X86_OP_IMM

from reconstruct import pe_layout


# ---------------------------------------------------------------------------
# PE helpers
# ---------------------------------------------------------------------------

def file_offset(layout, va):
    """Raw file offset of a virtual address, or None if it is not file-backed."""
    base = layout['image_base']
    for section in layout['sections']:
        delta = va - base - section['rva']
        if 0 <= delta < section['size']:
            return section['offset'] + delta
    return None


def iat_slots(data, layout, names):
    """Map kernel32 import names to their IAT slot VAs."""
    base = layout['image_base']
    rva, _ = struct.unpack_from('<II', data, layout['optional_offset'] + 104)
    p = file_offset(layout, base + rva)
    found = {}

    def string(string_rva):
        start = file_offset(layout, base + string_rva)
        return data[start:data.index(0, start)].decode('ascii')

    while any(data[p:p + 20]):
        lookup, _, _, name, iat = struct.unpack_from('<IIIII', data, p)
        dll = string(name).lower()
        cursor = file_offset(layout, base + (lookup or iat))
        i = 0
        while True:
            value = struct.unpack_from('<I', data, cursor + 4 * i)[0]
            if not value:
                break
            if not value & 0x80000000:
                found[(dll, string(value + 2))] = base + iat + 4 * i
            i += 1
        p += 20
    return {n: found[('kernel32.dll', n)] for n in names}


# ---------------------------------------------------------------------------
# Prologue theft + relocation
# ---------------------------------------------------------------------------

class PrologueError(Exception):
    pass


def steal_prologue(data, layout, target):
    """Copy whole instructions from `target` until >= 5 bytes are taken.

    Returns (length, replay_lines). `replay_lines` are NASM statements that
    reproduce the stolen instructions at the wrapper's address and then
    transfer control back into the function body, using only absolute
    immediates so the wrapper can be relocated freely.
    """
    offset = file_offset(layout, target)
    if offset is None:
        raise PrologueError(f'0x{target:08x} is not in a file-backed section')
    cs = Cs(CS_ARCH_X86, CS_MODE_32)
    cs.detail = True
    taken = []
    length = 0
    for insn in cs.disasm(data[offset:offset + 32], target):
        taken.append(insn)
        length += insn.size
        if length >= 5:
            break
    if length < 5:
        raise PrologueError(f'0x{target:08x}: cannot recover a 5-byte prologue')
    resume = target + length

    lines = []
    for i, insn in enumerate(taken):
        is_branch = insn.group(CS_GRP_JUMP) or insn.group(CS_GRP_CALL)
        if insn.group(CS_GRP_RET):
            raise PrologueError(
                f'0x{target:08x}: prologue returns ({insn.mnemonic}); cannot relocate')
        if is_branch:
            # Only a trailing near call/jmp (E8/E9) can be rewritten to an
            # absolute, relocation-free form. Anything else (short jcc, a
            # branch that is not last) is refused.
            if i != len(taken) - 1 or insn.operands[0].type != X86_OP_IMM:
                raise PrologueError(
                    f'0x{target:08x}: relative branch in prologue cannot be moved')
            dest = insn.operands[0].imm & 0xffffffff
            op = insn.bytes[0]
            if op == 0xe9:                       # jmp rel32 -> absolute jmp
                lines.append(f'    push dword 0x{dest:08x}')
                lines.append('    ret')
                return length, lines             # control leaves; no resume
            if op == 0xe8:                       # call rel32 -> absolute call
                lines.append(f'    push dword 0x{resume:08x}')   # return address
                lines.append(f'    push dword 0x{dest:08x}')     # callee
                lines.append('    ret')
                # falls through to resume after the callee returns
                lines.append(f'    push dword 0x{resume:08x}')
                lines.append('    ret')
                return length, lines
            raise PrologueError(
                f'0x{target:08x}: unsupported relative opcode 0x{op:02x}')
        raw = data[offset + insn.address - target: offset + insn.address - target + insn.size]
        lines.append('    db ' + ', '.join(f'0x{b:02x}' for b in raw))
    lines.append(f'    push dword 0x{resume:08x}')
    lines.append('    ret')
    return length, lines


# ---------------------------------------------------------------------------
# Placement: largest verified-zero, file-backed, in-bounds section tail
# ---------------------------------------------------------------------------

def choose_region(data, layout, needed):
    best = None
    for s in layout['sections']:
        end = s['offset'] + s['size']
        z = end
        while z > s['offset'] and data[z - 1] == 0:
            z -= 1
        avail = end - z
        # 16-align the start inside the zero run.
        start = (z + 15) & ~15
        avail -= start - z
        start_va = layout['image_base'] + s['rva'] + (start - s['offset'])
        # Must stay inside the section's virtual size (already mapped) so no
        # header field needs changing.
        in_vsize = (start - s['offset']) + needed <= s['virtual_size']
        if avail >= needed and in_vsize and (best is None or avail > best['avail']):
            best = dict(section=s['name'], file=start, va=start_va, avail=avail)
    if best is None:
        raise PrologueError(f'no verified-zero section tail fits {needed} bytes')
    return best


# ---------------------------------------------------------------------------
# Blob: compile + link + self-relocate
# ---------------------------------------------------------------------------

def _data_dir(data, opt, index):
    return struct.unpack_from('<II', data, opt + 96 + index * 8)


def relocate_blob(dll_bytes, dest_va):
    """Extract the merged .text of a linked blob, relocate it to dest_va."""
    pe = struct.unpack_from('<I', dll_bytes, 0x3c)[0]
    opt = pe + 24
    count = struct.unpack_from('<H', dll_bytes, pe + 6)[0]
    optsize = struct.unpack_from('<H', dll_bytes, pe + 20)[0]
    base = struct.unpack_from('<I', dll_bytes, opt + 28)[0]
    secs = {}
    for i in range(count):
        p = opt + optsize + 40 * i
        name = dll_bytes[p:p + 8].rstrip(b'\0').decode()
        vsize, rva, rsize, roff = struct.unpack_from('<IIII', dll_bytes, p + 8)
        secs[name] = dict(rva=rva, vsize=vsize, rsize=rsize, roff=roff)
    text = secs['.text']
    blob = bytearray(dll_bytes[text['roff']:text['roff'] + text['vsize']])
    link_va = base + text['rva']
    delta = dest_va - link_va

    def roff_of(rva):
        for s in secs.values():
            if s['rva'] <= rva < s['rva'] + max(s['vsize'], s['rsize']):
                return s['roff'] + (rva - s['rva'])
        raise PrologueError(f'reloc site rva 0x{rva:x} not in any section')

    sites = []
    reloc_rva, reloc_size = _data_dir(dll_bytes, opt, 5)
    if reloc_rva:
        off = roff_of(reloc_rva)
        end = off + reloc_size
        while off < end:
            page_rva, block = struct.unpack_from('<II', dll_bytes, off)
            if block == 0:
                break
            for j in range((block - 8) // 2):
                e = struct.unpack_from('<H', dll_bytes, off + 8 + 2 * j)[0]
                typ, o = e >> 12, e & 0xfff
                if typ == 0:
                    continue
                if typ != 3:
                    raise PrologueError(f'unexpected reloc type {typ}')
                site = page_rva + o - text['rva']
                if not (0 <= site < len(blob)):
                    raise PrologueError('reloc site outside .text')
                old = struct.unpack_from('<I', blob, site)[0]
                struct.pack_into('<I', blob, site, (old + delta) & 0xffffffff)
                sites.append(site)
            off += block
    return bytes(blob), link_va, sites


def read_exports(dll_bytes):
    pe = struct.unpack_from('<I', dll_bytes, 0x3c)[0]
    opt = pe + 24
    count = struct.unpack_from('<H', dll_bytes, pe + 6)[0]
    optsize = struct.unpack_from('<H', dll_bytes, pe + 20)[0]
    base = struct.unpack_from('<I', dll_bytes, opt + 28)[0]
    secs = []
    for i in range(count):
        p = opt + optsize + 40 * i
        vsize, rva, rsize, roff = struct.unpack_from('<IIII', dll_bytes, p + 8)
        secs.append((rva, max(vsize, rsize), roff))

    def roff_of(rva):
        for srva, size, roff in secs:
            if srva <= rva < srva + size:
                return roff + (rva - srva)
        raise PrologueError(f'export rva 0x{rva:x} not mapped')

    exp_rva, exp_size = _data_dir(dll_bytes, opt, 0)
    off = roff_of(exp_rva)
    nfuncs, nnames = struct.unpack_from('<II', dll_bytes, off + 20)
    addr_rva, name_rva, ord_rva = struct.unpack_from('<III', dll_bytes, off + 28)
    exports = {}
    for i in range(nnames):
        name_ptr = struct.unpack_from('<I', dll_bytes, roff_of(name_rva) + 4 * i)[0]
        ordinal = struct.unpack_from('<H', dll_bytes, roff_of(ord_rva) + 2 * i)[0]
        func_rva = struct.unpack_from('<I', dll_bytes, roff_of(addr_rva) + 4 * ordinal)[0]
        s = roff_of(name_ptr)
        name = dll_bytes[s:dll_bytes.index(0, s)].decode('ascii')
        exports[name] = base + func_rva
    return exports


# ---------------------------------------------------------------------------
# Spec loading
# ---------------------------------------------------------------------------

def load_spec(path):
    spec = importlib.util.spec_from_file_location('trace_spec', path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


# ---------------------------------------------------------------------------
# Build
# ---------------------------------------------------------------------------

def build(baseline, output, spec, clang, nasm, lld):
    data = baseline.read_bytes()
    layout = pe_layout(data)
    image_chars = struct.unpack_from('<H', data, layout['pe_offset'] + 22)[0]
    if not image_chars & 1:
        raise PrologueError('this detour tool needs the fixed-base, relocation-stripped retail image')
    output.mkdir(parents=True, exist_ok=False)

    hooks = spec.HOOKS
    iat = iat_slots(data, layout, list(spec.IAT_IMPORTS))

    # Steal each prologue (so the generated return addresses are known) and
    # emit the single wrapper translation unit.
    stolen = {}
    wrapper = ['BITS 32', 'section .text code align=16']
    for h in hooks:
        wrapper.append(f'extern _{h["handler"]}')
    for h in hooks:
        length, replay = steal_prologue(data, layout, h['target'])
        stolen[h['name']] = length
        wrapper.append(f'global _hook_{h["name"]}')
        wrapper.append(f'_hook_{h["name"]}:')
        wrapper.append('    pushfd')
        wrapper.append('    pushad')
        wrapper.append('    push esp')                 # &pushad-block -> handler
        wrapper.append(f'    call _{h["handler"]}')
        wrapper.append('    add esp, 4')
        wrapper.append('    popad')
        wrapper.append('    popfd')
        wrapper.extend(replay)
    (output / 'wrappers.asm').write_text('\n'.join(wrapper) + '\n')

    # The single C translation unit: shared runtime + every handler.
    c_src = [spec.runtime_c(iat)]
    for h in hooks:
        c_src.append(h['c'])
    (output / 'handlers.c').write_text('\n'.join(c_src) + '\n')

    # Compile and assemble.
    subprocess.run([nasm, '-f', 'win32', str(output / 'wrappers.asm'),
                    '-o', str(output / 'wrappers.obj')], check=True)
    subprocess.run([clang, '--target=i686-pc-windows-msvc', '-march=i586',
                    '-mno-sse', '-mno-sse2', '-mno-mmx', '-mno-x87', '-Os',
                    '-ffreestanding', '-fno-builtin', '-fno-stack-protector',
                    '-fomit-frame-pointer', '-fno-unwind-tables',
                    '-fno-asynchronous-unwind-tables',
                    '-c', str(output / 'handlers.c'),
                    '-o', str(output / 'handlers.obj')], check=True)

    # Link into one relocatable blob (a DLL carries a .reloc table).
    exports = sum(([f'/export:hook_{h["name"]}'] for h in hooks), [])
    subprocess.run([lld, '/nologo', '/machine:x86', '/base:0x10000000', '/dll',
                    '/noentry', '/nodefaultlib',
                    '/merge:.rdata=.text', '/merge:.data=.text', '/merge:.bss=.text',
                    '/section:.text,erw', '/align:0x1000', '/filealign:0x200',
                    *exports, '/out:' + str(output / 'blob.dll'),
                    str(output / 'wrappers.obj'), str(output / 'handlers.obj')],
                   check=True, stdout=subprocess.DEVNULL)
    dll = (output / 'blob.dll').read_bytes()

    # Size it, choose a home, relocate it there.
    pe = struct.unpack_from('<I', dll, 0x3c)[0]
    opt = pe + 24
    count = struct.unpack_from('<H', dll, pe + 6)[0]
    optsize = struct.unpack_from('<H', dll, pe + 20)[0]
    text_vsize = None
    for i in range(count):
        p = opt + optsize + 40 * i
        if dll[p:p + 8].rstrip(b'\0') == b'.text':
            text_vsize = struct.unpack_from('<I', dll, p + 8)[0]
    region = choose_region(data, layout, text_vsize)
    blob, link_va, reloc_sites = relocate_blob(dll, region['va'])
    exports = read_exports(dll)
    hook_va = {h['name']: exports[f'hook_{h["name"]}'] + (region['va'] - link_va)
               for h in hooks}

    # Patch a copy: the blob, then each detour.
    result = bytearray(data)
    result[region['file']:region['file'] + len(blob)] = blob
    changed_spans = [(region['file'], region['file'] + len(blob))]
    for h in hooks:
        off = file_offset(layout, h['target'])
        length = stolen[h['name']]
        rel = (hook_va[h['name']] - (h['target'] + 5)) & 0xffffffff
        patch = b'\xe9' + struct.pack('<I', rel) + b'\x90' * (length - 5)
        result[off:off + length] = patch
        changed_spans.append((off, off + length))

    # Verify: nothing outside the blob and the detours changed.
    allowed = changed_spans
    bad = [i for i, (a, b) in enumerate(zip(data, result))
           if a != b and not any(lo <= i < hi for lo, hi in allowed)]
    if bad:
        raise PrologueError(f'{len(bad)} bytes changed outside the hook spans / wrapper area')

    exe = output / 'Revenant.hooked.exe'
    exe.write_bytes(result)

    report = dict(
        status='built_not_guest_run',
        spec=str(Path(spec.__file__).name),
        baseline_sha256=hashlib.sha256(data).hexdigest(),
        variant_sha256=hashlib.sha256(result).hexdigest(),
        wrapper_region=dict(section=region['section'], file_offset=region['file'],
                            va=region['va'], size=len(blob), avail=region['avail']),
        reloc_site_count=len(reloc_sites),
        iat={k: f'0x{v:08x}' for k, v in iat.items()},
        hooks=[dict(name=h['name'], target=f'0x{h["target"]:08x}',
                    handler=h['handler'], stolen=stolen[h['name']],
                    wrapper_va=f'0x{hook_va[h["name"]]:08x}',
                    capture=h.get('capture', '')) for h in hooks],
        changed_byte_count=sum(1 for a, b in zip(data, result) if a != b),
        scope='Spec-driven multi-hook entry detours. Each wrapper saves flags+GPRs, '
              'calls read-only C compiled freestanding and linked into one self-relocated '
              'blob in a verified-zero section tail, restores state, replays the stolen '
              'prologue and returns absolutely. Only the detour spans and the wrapper blob '
              'differ from the baseline. Guest run validated separately.')
    (output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f'traced build: {exe}  ({len(hooks)} hooks, blob {len(blob)} bytes in '
          f'{region["section"]} tail @ 0x{region["va"]:08x})')
    return report


def main():
    here = Path(__file__).resolve().parent
    p = argparse.ArgumentParser(description=__doc__,
                                formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('baseline', type=Path)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--spec', type=Path, default=here / 'trace_hooks.py')
    p.add_argument('--clang', default=shutil.which('clang'))
    p.add_argument('--nasm', default=shutil.which('nasm'))
    p.add_argument('--lld', default=shutil.which('lld-link'))
    a = p.parse_args()
    for tool, name in ((a.clang, 'clang'), (a.nasm, 'nasm'), (a.lld, 'lld-link')):
        if not tool:
            p.error(f'{name} is required')
    sys.path.insert(0, str(here))
    spec = load_spec(a.spec)
    build(a.baseline.resolve(), a.output.resolve(), spec, a.clang, a.nasm, a.lld)


if __name__ == '__main__':
    main()
