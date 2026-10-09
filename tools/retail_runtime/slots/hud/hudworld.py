r"""The HUD slot's retail world: the shipped game booted by its own code, in
one Runtime, ready for HUD fixtures to checkpoint.

Boot (`HudWorld(executable)`), every step original code:
1. CRT entry up to `_ioinit` (heap, TLS), `_ioinit` `0x0059299f`, the
   environment block `0x00593c9c`. Not run: `_setargv` / `_setenvp` (WinMain
   parses its own command line).
2. C initializers `0x005c58f0..0x005c5908` and the 571 C++ static
   constructors `0x005c5000..0x005c58ec`: every global pane, the display
   object, the font manager.
3. WinMain `0x004865a0` with the command line `NOSOUND` (retail's switch that
   skips Miles and Red Book audio; fullscreen), stopped right after game
   init `0x00485870` returns (`0x00486e7f`), before the lobby check, intro
   movie and first screen. On the way: mutexes, Revenant.ini, the
   RESOURCE.RVR / IMAGERY.RVI packs, the window, DirectDraw, the 640x480x16
   display, the font manager and FONT.DEF, the 3D scene, rules, classes,
   gamedata.dat and the widget archives.

The environment (each module documents its scope and choices):
- `system.py`: the machine -- Windows 98, 64 MB, wall clock, volumes,
  locale/CRT answers, no DirectInput, recorded multimedia timers.
- `winfs.py`: the install read-only at C:\REVENANT; `profile.py`: INI from
  the CD's Revenant.ini.
- `window.py`: the game window; `gdi.py` (+ `ttf.py`): fonts and text calls.
- `ddraw.py`: DirectDraw and Direct3D 3 on RAM surfaces, a 16 MB card.

Resulting configuration, asserted after boot (docs/ui/HUD_REBUILD.md §3 on
feature/ui): a 16 MB 3D-capable DirectDraw card whose Direct3D offers no
texture-capable device -- retail's `DAT_00669ad8 == 0` path.
- `DAT_006680c8 == 0`: texture overlays (Revenant.ini NoTexOverlay = No; more
  than 8 MB of video memory). The Classic HUD.
- `DAT_005d7a28 == 1`: T3DScene::Initialize `0x00411eb0` keeps Revenant's
  internal software rasterizer. A hardware player got the same composition
  rasterized by the D3D device.

Boundaries (original entries answered by the host, recorded in
`boundary_calls`): the boot log `0x004820b0` (format string kept, no file
I/O) and network init `0x00575890` (DirectPlay: reports success with no
network object -- single player; retail would stop with "Unable to
initialize Network" otherwise).
"""
from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))
from unicorn import UC_HOOK_CODE, UC_HOOK_MEM_FETCH_UNMAPPED, UC_HOOK_MEM_READ_UNMAPPED, UC_HOOK_MEM_WRITE_UNMAPPED  # noqa: E402
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EIP, UC_X86_REG_ESP  # noqa: E402
from runtime import Runtime  # noqa: E402
from ddraw import DirectDraw  # noqa: E402
from gdi import Gdi  # noqa: E402
from profile import PrivateProfile  # noqa: E402
from system import SystemApis  # noqa: E402
from window import Windows  # noqa: E402
from winfs import WinFileSystem  # noqa: E402

RETAIL_SHA = '28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5'
CRT_INIT, CRT_INIT_STOP = 0x0058ed0d, 0x0058ed8e     # entry: heap + TLS, before _ioinit
IOINIT, ENVIRONMENT, ENVIRONMENT_PTR = 0x0059299f, 0x00593c9c, 0x00676f58
C_INIT = (0x005c58f0, 0x005c5908)
CPP_INIT = (0x005c5000, 0x005c58ec)
WINMAIN = 0x004865a0
GAME_INITIALIZED = 0x00486e7f                # WinMain, just after `call 0x00485870` (game init)
FRAME_RESERVE = 0x1000                       # below WinMain's live frame for later host calls
COMMAND_LINE = b'NOSOUND'               # retail's own switch: no Miles / Red Book audio
RETAIL_INSTALL = Path.home() / 'RevenantRetailLab' / 'retail-cd' / 'REVENANT'
BOOT_LOG = 0x004820b0                        # printf-style append to revboot.log, cdecl
NETWORK_INIT = 0x00575890                    # DirectPlay provider enumeration, thiscall

G_NOTEXOVERLAY = 0x006680c8
G_INTERNAL_RASTER = 0x005d7a28


class HudWorld:
    def __init__(self, executable, install=RETAIL_INSTALL):
        self.vm = vm = Runtime(executable)
        self.sha = hashlib.sha256(vm.image).hexdigest()
        if self.sha != RETAIL_SHA:
            raise ValueError('HUD fixtures are verified for the unchanged retail image only')
        self.last_fault = None
        vm.uc.hook_add(UC_HOOK_MEM_READ_UNMAPPED | UC_HOOK_MEM_WRITE_UNMAPPED | UC_HOOK_MEM_FETCH_UNMAPPED,
                       self._fault)
        vm.call(CRT_INIT, stop_address=CRT_INIT_STOP)
        self.system = SystemApis(vm)
        self.boundary_calls = []               # (name, detail) in call order
        self._boundaries = {}
        self.boundary(BOOT_LOG, 'boot log', 0, self._boot_log)
        self.boundary(NETWORK_INIT, 'network init', 0, lambda args, ecx: (1, 'no DirectPlay'))
        self.fs = WinFileSystem(vm)
        self.mounted = self.fs.mount_tree(install)
        self.profile = PrivateProfile(vm, self.fs)
        self.call(IOINIT)
        vm.put_u32(ENVIRONMENT_PTR, self.call(ENVIRONMENT))
        for first, last in (C_INIT, CPP_INIT):
            for entry in range(first, last, 4):
                function = vm.u32(entry)
                if function:
                    self.call(function)
        self.ddraw = DirectDraw(vm)
        self.windows = Windows(vm)
        self.gdi = Gdi(vm)
        command = vm.allocate(len(COMMAND_LINE) + 1)
        vm.write(command, COMMAND_LINE + b'\0')
        self.call(WINMAIN, (vm.base, 0, command, 1), stop_address=GAME_INITIALIZED,
                  instruction_limit=2_000_000_000)
        if vm.uc.reg_read(UC_X86_REG_EAX) != 1:
            raise RuntimeError('Retail game init (0x00485870) reported failure')
        # WinMain's frame stays live for the rest of the game in retail; host
        # calls from here on use the stack below it.
        vm.call_sp = (vm.uc.reg_read(UC_X86_REG_ESP) - FRAME_RESERVE) & ~0xf
        if vm.u32(G_NOTEXOVERLAY) != 0 or vm.u32(G_INTERNAL_RASTER) != 1:
            raise RuntimeError('Boot did not reach the texture-overlay / internal-raster mode')

    # ---- boundaries: original entries answered by the host --------------

    def boundary(self, address, name, pop, handler):
        """Answer the original function at `address` from Python under its
        own ABI: `pop` = argument bytes the callee removes (0 for cdecl).
        handler(args, ecx) -> (EAX, detail); `args` = first 8 stack words.
        Every call is recorded in `boundary_calls`."""
        self._boundaries[address] = (name, pop, handler)
        self.vm.uc.hook_add(UC_HOOK_CODE, self._enter_boundary, begin=address, end=address)

    def _enter_boundary(self, uc, address, size, user):
        name, pop, handler = self._boundaries[address]
        sp = uc.reg_read(UC_X86_REG_ESP)
        args = struct.unpack('<8I', uc.mem_read(sp + 4, 32))
        try:
            result, detail = handler(args, uc.reg_read(UC_X86_REG_ECX))
        except Exception as error:
            self.vm.error = error
            uc.emu_stop()
            return
        self.boundary_calls.append((name, detail))
        uc.reg_write(UC_X86_REG_EAX, (result or 0) & 0xffffffff)
        uc.reg_write(UC_X86_REG_EIP, self.vm.u32(sp))
        uc.reg_write(UC_X86_REG_ESP, sp + 4 + pop)

    def _boot_log(self, args, ecx):
        return 0, self.vm.string(args[0])

    def _fault(self, uc, access, address, size, value, user):
        """Remember an unmapped access with the likely callers (code
        addresses on the stack, innermost first) for the error message."""
        words = struct.unpack('<64I', uc.mem_read(uc.reg_read(UC_X86_REG_ESP), 256))
        callers = [w for w in words if 0x401000 <= w < 0x5a0000][:8]
        self.last_fault = (uc.reg_read(UC_X86_REG_EIP), address, callers)
        return False

    def call(self, address, args=(), this=None, instruction_limit=5_000_000, stop_address=None):
        """vm.call; a guest fault names where it happened."""
        self.last_fault = None
        try:
            return self.vm.call(address, args, this=this, instruction_limit=instruction_limit,
                                stop_address=stop_address)
        except Exception as error:
            where = ''
            if self.last_fault:
                eip, at, callers = self.last_fault
                where = (f' (eip 0x{eip:08x}, access 0x{at:08x}, stack '
                         + ' '.join(f'0x{c:x}' for c in callers) + ')')
            raise RuntimeError(f'0x{address:08x}: {error}{where}') from error

    def checkpoint(self):
        self.vm.checkpoint()
        self._host_state = (self.ddraw.snapshot(), self.fs.snapshot(), self.system.snapshot(),
                            self.windows.snapshot(), self.gdi.snapshot())

    def restore(self):
        self.vm.restore()
        self.ddraw.restore(self._host_state[0])
        self.fs.restore(self._host_state[1])
        self.system.restore(self._host_state[2])
        self.windows.restore(self._host_state[3])
        self.gdi.restore(self._host_state[4])


def main():
    import argparse
    import json
    import time
    parser = argparse.ArgumentParser(description='Boot retail in the HUD world and summarize it.')
    parser.add_argument('executable', type=Path)
    args = parser.parse_args()
    started = time.perf_counter()
    world = HudWorld(args.executable)
    vm = world.vm
    summary = dict(
        retail_sha256=world.sha, boot_seconds=round(time.perf_counter() - started, 1),
        configuration={name: vm.u32(address) for name, address in
                       (('NoTexOverlay', G_NOTEXOVERLAY), ('InternalRaster', G_INTERNAL_RASTER),
                        ('TexturedDevice', 0x00669ad8))},
        files_mounted=world.mounted, boundaries=world.boundary_calls[-6:],
        fonts=sorted({f'{f.face} {f.height}{" bold" if f.bold else ""}'
                      for f in world.gdi.fonts.values()}),
        surfaces=len(world.ddraw.surfaces), timers=len(world.system.timers),
        threads={tid: t.state for tid, t in vm.scheduler.threads.items()},
        api_calls=len(vm.api_trace))
    print(json.dumps(summary, indent=2))


if __name__ == '__main__':
    main()
