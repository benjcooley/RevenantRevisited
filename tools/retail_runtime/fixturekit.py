"""Pieces every retail A/B fixture shares (the tracks' slots/<track>/).

Promoted from the gameflow slot's fixture.py (same API) so a slot that is
versioned with the emulator doesn't depend on another slot's files.

- `start(executable)`: a Runtime on the unchanged retail image, original CRT
  heap/TLS initialization run (stopped before the static constructors, as
  the software/VFX fixtures do), so the original allocators work.
- `Boundaries`: replace exact original function entries with recorded host
  handlers that return under the function's own ABI (stack bytes it pops).
- `serve(sha, schema, setup_ms, run)`: the JSONL loop of the A/B driver
  (tools/retail_ab/retail_ab.py): one request per line, `{"id":…,
  "case":{…}}` in, `{"id":…, "ok":…, "result"|"error":…}` out.
"""
from __future__ import annotations

import hashlib
import json
import struct
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from unicorn import UC_HOOK_CODE  # noqa: E402
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EIP, UC_X86_REG_ESP  # noqa: E402
from runtime import Runtime  # noqa: E402

RETAIL_SHA = '28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5'
CRT_INIT, CRT_INIT_STOP = 0x58ed0d, 0x58ed8e
MALLOC = 0x482fb0                                   # engine malloc (zeroing), cdecl


def s32(value):
    return struct.unpack('<i', struct.pack('<I', value & 0xffffffff))[0]


def start(executable):
    vm = Runtime(executable)
    sha = hashlib.sha256(vm.image).hexdigest()
    if sha != RETAIL_SHA:
        raise ValueError('these fixtures are verified for the unchanged retail image only')
    vm.call(CRT_INIT, stop_address=CRT_INIT_STOP)
    return vm, sha


class Boundaries:
    """Original function entries answered by the host, every call recorded."""

    def __init__(self, vm):
        self.vm = vm
        self.handlers = {}
        self.calls = []

    def add(self, address, name, pop, handler):
        """`pop`: argument bytes the callee removes (0 for cdecl).
        handler(args, ecx) -> EAX; `args` are the first stack arguments."""
        self.handlers[address] = (name, pop, handler)
        self.vm.uc.hook_add(UC_HOOK_CODE, self._enter, begin=address, end=address)

    def add_stub(self, name, pop, handler):
        """A host function at a fresh address, for synthetic vtable slots."""
        address = self.vm.allocate(16)
        self.vm.write(address, b'\xc3')                 # never executed: the hook returns first
        self.add(address, name, pop, handler)
        return address

    def _enter(self, uc, address, size, user):
        name, pop, handler = self.handlers[address]
        vm = self.vm
        sp = uc.reg_read(UC_X86_REG_ESP)
        args = struct.unpack('<8I', uc.mem_read(sp + 4, 32))
        ecx = uc.reg_read(UC_X86_REG_ECX)
        try:
            result = handler(args, ecx)
        except Exception as error:      # surface as a fixture error
            vm.error = error
            uc.emu_stop()
            return
        self.calls.append(name)
        uc.reg_write(UC_X86_REG_EAX, (result or 0) & 0xffffffff)
        uc.reg_write(UC_X86_REG_EIP, vm.u32(sp))
        uc.reg_write(UC_X86_REG_ESP, sp + 4 + pop)


def serve(sha, schema, setup_ms, run):
    sys.stdout.write(json.dumps(dict(id='hello', ok=True, retail_sha256=sha, schema=schema,
                                     setup_ms=setup_ms)) + '\n')
    sys.stdout.flush()
    for raw in sys.stdin:
        raw = raw.strip()
        if not raw:
            continue
        request = json.loads(raw)
        try:
            started = time.perf_counter()
            result = run(request['case'])
            result['elapsed_ms'] = (time.perf_counter() - started) * 1000
            response = dict(id=request.get('id'), ok=True, result=result)
        except Exception as error:
            response = dict(id=request.get('id'), ok=False, error=f'{type(error).__name__}: {error}')
        sys.stdout.write(json.dumps(response) + '\n')
        sys.stdout.flush()
