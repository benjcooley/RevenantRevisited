r"""Win32 calls retail makes once it plays, beyond the HUD machine's boot set
(`slots/hud/system.py`, `winfs.py`, ...). Same rules: Windows 98's
documented behaviour on the emulator's state, anything else fails by name.

- IsBadReadPtr / IsBadWritePtr (kernel32): non-zero unless every byte of
  the range lies in mapped guest memory with that access (Unicorn's
  regions and protections); a zero length is never bad. Retail checks
  pointers before releasing them (e.g. 0x004c3980, eight at +0x1c0).
"""
from __future__ import annotations

from unicorn import UC_PROT_READ, UC_PROT_WRITE


class PlayMachine:
    def __init__(self, vm):
        self.vm = vm
        kernel32 = {'kernel32.dll'}
        for name, argc, fn in [
                ('IsBadReadPtr', 2, lambda a: self._bad(a, UC_PROT_READ)),
                ('IsBadWritePtr', 2, lambda a: self._bad(a, UC_PROT_WRITE))]:
            vm.handlers[name] = (argc, fn)
            vm.api_dlls[name] = kernel32

    def _bad(self, args, access):
        address, size = args
        if not size:
            return 0
        end = address + size
        if end > 1 << 32:
            return 1
        regions = sorted((begin, last + 1, perms) for begin, last, perms in self.vm.uc.mem_regions())
        cursor = address
        for begin, stop, perms in regions:
            if stop <= cursor:
                continue
            if begin > cursor or not perms & access:
                return 1
            cursor = stop
            if cursor >= end:
                return 0
        return 1
