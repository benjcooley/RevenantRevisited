"""user32 windowing for the HUD slot: the one game window, no desktop.

Retail creates a single top-level window and renders through DirectDraw;
nothing the HUD draws depends on window management. This layer gives the
startup what it asks for, deterministically:

- Desktop 800x600 (GetSystemMetrics); the game switches to 640x480x16
  through DirectDraw.
- RegisterClassA records the class (its WndProc); CreateWindowExA returns
  one HWND per call and registers the class WndProc with the core's
  message queue (`vm.windows`) so posted messages reach it.
  CreateWindowExA does NOT send WM_NCCREATE/WM_CREATE (the core has no
  sent messages): recorded in `created`, so a dependency shows up.
- Show/Update/focus/cursor calls are recorded and succeed.
"""
from __future__ import annotations

import copy
import struct

DESKTOP = (800, 600)
SYSTEM_METRICS = {0: DESKTOP[0], 1: DESKTOP[1],          # SM_CXSCREEN / SM_CYSCREEN
                  4: 19,                                  # SM_CYCAPTION
                  5: 1, 6: 1,                             # SM_CXBORDER / SM_CYBORDER
                  7: 3, 8: 3,                             # SM_CXDLGFRAME / SM_CYDLGFRAME
                  15: 0,                                  # SM_CYMENU (no menu)
                  32: 4, 33: 4,                           # SM_CXFRAME / SM_CYFRAME
                  43: 2,                                  # SM_CMOUSEBUTTONS
                  80: 1}                                  # SM_CMONITORS


class Windows:
    def __init__(self, vm, state=None):
        self.vm = vm
        self.classes = {}                     # name -> WndProc
        self.created = []                     # (hwnd, class, title, style, x, y, w, h)
        self.next_hwnd = 0x1000
        self.calls = []                       # recorded no-effect calls
        user32 = {'user32.dll'}
        for name, argc, fn in [
                ('GetSystemMetrics', 1, self._metrics),
                ('RegisterClassA', 1, self._register_class),
                ('CreateWindowExA', 12, self._create_window),
                ('LoadIconA', 2, lambda a: 0x2001),
                ('LoadCursorA', 2, lambda a: 0x2002),
                ('ShowWindow', 2, self._recorded('ShowWindow', 1)),
                ('UpdateWindow', 1, self._recorded('UpdateWindow', 1)),
                ('SetFocus', 1, self._recorded('SetFocus', 0)),
                ('SetForegroundWindow', 1, self._recorded('SetForegroundWindow', 1)),
                ('SetActiveWindow', 1, self._recorded('SetActiveWindow', 0)),
                ('ShowCursor', 1, self._show_cursor),
                ('SetCursor', 1, self._recorded('SetCursor', 0)),
                ('GetClientRect', 2, self._client_rect),
                ('GetWindowRect', 2, self._client_rect),
                ('SystemParametersInfoA', 4, self._parameters),
                ('DefWindowProcA', 4, lambda a: 0)]:
            vm.handlers[name] = (argc, fn)
            vm.api_dlls[name] = user32
        self.cursor_count = 0
        if state is not None:
            self.set_state(state)

    def _recorded(self, name, result):
        def handler(args):
            self.calls.append((name, list(args)))
            return result
        return handler

    def _metrics(self, args):
        if args[0] not in SYSTEM_METRICS:
            raise ValueError(f'GetSystemMetrics({args[0]}) not in the fixed machine')
        return SYSTEM_METRICS[args[0]]

    def _register_class(self, args):
        # WNDCLASSA: style, lpfnWndProc, cbClsExtra, cbWndExtra, hInstance,
        # hIcon, hCursor, hbrBackground, lpszMenuName, lpszClassName
        fields = struct.unpack('<10I', bytes(self.vm.uc.mem_read(args[0], 40)))
        name = self.vm.string(fields[9]).lower()
        self.classes[name] = fields[1]
        return 0xc000 + len(self.classes)                 # class atom

    def _create_window(self, args):
        ex_style, class_name, title, style, x, y, w, h, parent, menu, instance, param = args
        name = self.vm.string(class_name).lower()
        if name not in self.classes:
            raise ValueError(f'CreateWindowExA for unregistered class {name}')
        hwnd = self.next_hwnd
        self.next_hwnd += 4
        self.vm.windows[hwnd] = self.classes[name]
        self.created.append((hwnd, name, self.vm.string(title) if title else '', style, x, y, w, h))
        return hwnd

    def _parameters(self, args):
        action, uparam, pvparam, _ini = args
        self.calls.append(('SystemParametersInfoA', list(args)))
        if action == 0x10:                                # SPI_GETSCREENSAVEACTIVE
            self.vm.put_u32(pvparam, 0)
        elif action == 0x30:                              # SPI_GETWORKAREA
            self.vm.write(pvparam, struct.pack('<4i', 0, 0, *DESKTOP))
        elif action not in (0x11, 0x61, 0x97):            # SET screensaver / running / (Win98) same
            raise ValueError(f'SystemParametersInfoA action 0x{action:x}')
        return 1

    def _show_cursor(self, args):
        self.cursor_count += 1 if args[0] else -1
        return self.cursor_count & 0xffffffff

    def _client_rect(self, args):
        hwnd, rect = args
        self.vm.write(rect, struct.pack('<4i', 0, 0, 640, 480))
        return 1

    STATE = ('classes', 'created', 'next_hwnd', 'calls', 'cursor_count')

    def state(self):
        return copy.deepcopy({name: getattr(self, name) for name in self.STATE})

    def set_state(self, state):
        for name, value in copy.deepcopy(state).items():
            setattr(self, name, value)
