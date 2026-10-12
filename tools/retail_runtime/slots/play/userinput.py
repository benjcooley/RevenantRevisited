r"""user32 input for the play slot: the keyboard and the mouse as Windows 98
delivers them to the shipped game, from the recording's events
(docs/playtest/PROTOCOL.md §2).

Which path retail reads for each control (traced in Revenant.exe):

- Keys: WM_KEYDOWN / WM_KEYUP / WM_CHAR posted to the game window. The
  screen's pump (`TScreen::TimerTick` 0x00490bd0, PeekMessage / Translate /
  Dispatch) hands them to the WndProc 0x00448cc0: WM_KEYDOWN notes Ctrl /
  Alt / Shift (0x0065c9e0 / 0x0065c9d8 / 0x0065b9fc) and passes the key to
  the current screen's KeyPress; WM_CHAR to CharPress. The control map turns
  keys into commands (PLAYER_INPUT.md §1).
- Mouse buttons: WM_[LRM]BUTTONDOWN / UP. The WndProc calls the screen's
  MouseClick 0x0048f870 with retail's button code (1 / 4 left down / up,
  2 / 5 right, 3 / 6 middle) and keeps the held mask 0x00668518. The class
  has no CS_DBLCLKS (style 3), so Windows never sends double clicks.
- The pointer is polled, not messaged: every TimerTick reads GetCursorPos,
  GetClientRect + ClientToScreen, and calls MouseMove 0x0048f950 when the
  point changed (0x00668510 / 14). The WndProc leaves WM_MOUSEMOVE to
  DefWindowProc; it is posted all the same, as Windows posts it.
- GetKeyboardState: only TimerTick's idle test (0x00490e90: is any key
  held), after the pump.
- TranslateMessage is called on every message (0x00490c99, 0x004912c7).
  It posts nothing here: the recording carries every character as its own
  `char` event (PROTOCOL.md §2), so translating keys as well would type
  each letter twice. It returns non-zero for key messages, as Windows does.
- SetCursorPos / ClipCursor: retail's cursor capture (0x0043a880, 0x0043a900;
  fullscreen only) puts the pointer back and clips it to the 640x480 client.
- DirectInput stays absent (the HUD machine's DirectInputCreateA fails), as
  does a joystick: retail then reads keys and the mouse as above only.

Windows behaviour reproduced, and where:

- Posting: an event for tick t is posted before that tick's timer fires
  (`deliver`), in event order. A move posts WM_MOUSEMOVE; then each button
  whose state changed (left, right, middle) posts its down or up message.
  Button messages carry MK_* flags (buttons held after the change, Shift,
  Ctrl) in wParam and the client point in lParam; MSG.pt is the pointer.
- Keys: lParam = repeat count 1 | scan code << 16 | extended << 24 |
  context (Alt) << 29 | previous state << 30 | transition << 31, scan codes
  from the US set 1 table below. A down for a key already down is an
  auto-repeat (previous state 1). With Alt held and Ctrl not, and for F10,
  the messages are WM_SYSKEYDOWN / WM_SYSKEYUP, as Windows sends them
  (retail's pump dispatches WM_SYSKEYDOWN; it keeps only Alt+F4 and
  Alt+Tab of WM_SYSKEYUP, 0x0049128b).
- WM_CHAR: wParam the Windows-1252 code; lParam that of the latest key
  down still held (repeat 1 and its scan code), else repeat 1.
- The keyboard state GetKeyboardState returns changes as the game removes
  key and button messages from its queue (PeekMessage with PM_REMOVE), as
  Windows' thread key state does: high bit held, low bit toggled on each
  press. The async state (what the hardware holds now) decides MK_ flags
  and the system keys.
- The pointer: GetCursorPos answers the virtual cursor, which events and
  SetCursorPos move and ClipCursor confines (Windows clamps the cursor to
  the clip rectangle; NULL clips to the 640x480 screen). The window sits
  at 0,0, so client and screen points are the same (ClientToScreen).
"""
from __future__ import annotations

import copy
import struct

WM_MOUSEMOVE = 0x200
WM_KEYDOWN, WM_KEYUP, WM_CHAR = 0x100, 0x101, 0x102
WM_SYSKEYDOWN, WM_SYSKEYUP = 0x104, 0x105
MK_LBUTTON, MK_RBUTTON, MK_SHIFT, MK_CONTROL, MK_MBUTTON = 0x1, 0x2, 0x4, 0x8, 0x10
VK_LBUTTON, VK_RBUTTON, VK_MBUTTON = 0x01, 0x02, 0x04
# Pointer mask bit (PROTOCOL.md), button down / up message, MK_ flag, VK code.
BUTTONS = ((1, 0x201, 0x202, MK_LBUTTON, VK_LBUTTON), (2, 0x204, 0x205, MK_RBUTTON, VK_RBUTTON),
           (4, 0x207, 0x208, MK_MBUTTON, VK_MBUTTON))
VK_SHIFT, VK_CONTROL, VK_MENU, VK_F10 = 0x10, 0x11, 0x12, 0x79
PM_REMOVE = 1
SCREEN = (640, 480)
START_POINTER = (320, 240)                 # PROTOCOL.md §2

# US keyboard, scan code set 1 as Windows reports it in lParam bits 16-23,
# and whether the key is an extended (E0) key (bit 24). Left Shift / Ctrl /
# Alt for the generic modifier codes.
SCAN = {0x08: 0x0e, 0x09: 0x0f, 0x0d: 0x1c, 0x10: 0x2a, 0x11: 0x1d, 0x12: 0x38, 0x13: 0x45,
        0x14: 0x3a, 0x1b: 0x01, 0x20: 0x39, 0x90: 0x45, 0x91: 0x46,
        0x6a: 0x37, 0x6b: 0x4e, 0x6d: 0x4a, 0x6e: 0x53,
        0xba: 0x27, 0xbb: 0x0d, 0xbc: 0x33, 0xbd: 0x0c, 0xbe: 0x34, 0xbf: 0x35, 0xc0: 0x29,
        0xdb: 0x1a, 0xdc: 0x2b, 0xdd: 0x1b, 0xde: 0x28,
        **{0x30: 0x0b}, **{0x31 + i: 0x02 + i for i in range(9)},
        **{ord(c): code for c, code in zip('QWERTYUIOP', range(0x10, 0x1a))},
        **{ord(c): code for c, code in zip('ASDFGHJKL', range(0x1e, 0x27))},
        **{ord(c): code for c, code in zip('ZXCVBNM', range(0x2c, 0x33))},
        **{0x70 + i: 0x3b + i for i in range(10)}, 0x7a: 0x57, 0x7b: 0x58,
        **{0x60 + n: code for n, code in enumerate((0x52, 0x4f, 0x50, 0x51, 0x4b, 0x4c, 0x4d,
                                                    0x47, 0x48, 0x49))}}
EXTENDED = {0x21: 0x49, 0x22: 0x51, 0x23: 0x4f, 0x24: 0x47, 0x25: 0x4b, 0x26: 0x48, 0x27: 0x4d,
            0x28: 0x50, 0x2d: 0x52, 0x2e: 0x53, 0x6f: 0x35, 0x5b: 0x5b, 0x5c: 0x5c, 0x5d: 0x5d,
            0x90: 0x45}                   # Num Lock reports the extended bit
TOGGLES = (0x14, 0x90, 0x91)              # Caps / Num / Scroll Lock


def key_lparam(vk, down, previous, context):
    scan = EXTENDED.get(vk, SCAN.get(vk, 0))
    return (1 | scan << 16 | int(vk in EXTENDED) << 24 | int(context) << 29
            | int(previous) << 30 | int(not down) << 31)


class UserInput:
    """The game window's input. `deliver(events)` posts one tick's events;
    the handlers answer the user32 queries retail makes."""

    def __init__(self, vm, hwnd, state=None):
        self.vm = vm
        self.hwnd = hwnd
        self.keys = bytearray(256)            # thread state: GetKeyboardState
        self.held = bytearray(256)            # async state: what is held now
        self.cursor = list(START_POINTER)
        self.buttons = 0                      # PROTOCOL.md mask: 1 left, 2 right, 4 middle
        self.clip = None                      # (left, top, right, bottom) or None
        self.last_key_lparam = None           # (vk, lParam) of the latest key down
        self.posted = 0                       # messages posted from events
        user32 = {'user32.dll'}
        core_peek = vm.handlers['PeekMessageA'][1]
        for name, argc, fn in [
                ('GetKeyboardState', 1, self._keyboard_state),
                ('GetCursorPos', 1, self._cursor_pos),
                ('SetCursorPos', 2, self._set_cursor_pos),
                ('ClipCursor', 1, self._clip_cursor),
                ('ClientToScreen', 2, lambda a: 1),           # the window is at 0,0
                ('TranslateMessage', 1, self._translate),
                ('PeekMessageA', 5, lambda a: self._peek(core_peek, a))]:
            vm.handlers[name] = (argc, fn)
            vm.api_dlls[name] = user32
        if state is not None:
            self.set_state(state)

    STATE = ('keys', 'held', 'cursor', 'buttons', 'clip', 'last_key_lparam', 'posted')

    def state(self):
        return copy.deepcopy({name: getattr(self, name) for name in self.STATE})

    def set_state(self, state):
        for name, value in copy.deepcopy(state).items():
            setattr(self, name, value)

    # ---- events -> messages --------------------------------------------

    def deliver(self, events):
        """Post one tick's events in order: PROTOCOL.md §2 dicts, with the
        key as its virtual-key code (`{"key": 0x41, "down": true}`)."""
        for event in events:
            if 'key' in event:
                self.key(event['key'], event['down'])
            elif 'char' in event:
                self.char(event['char'])
            elif 'mouse' in event:
                self.mouse(*event['mouse'], event['buttons'])
            else:
                raise ValueError(f'Not an input event: {event}')

    def _post(self, message, wparam, lparam):
        self.vm.post_message(self.hwnd, message, wparam, lparam, *self.cursor)
        self.posted += 1

    def key(self, vk, down):
        if not 0 < vk < 0x100:
            raise ValueError(f'Virtual key 0x{vk:x}')
        previous = bool(self.held[vk] & 0x80)
        if not down and not previous:
            raise ValueError(f'Key 0x{vk:x} released while not held')
        alt = bool(self.held[VK_MENU] & 0x80) or (vk == VK_MENU and down)
        ctrl = bool(self.held[VK_CONTROL] & 0x80)
        system = (alt and not ctrl) or (vk == VK_F10 and not ctrl)
        # The context bit: Alt held while the key changes (released Alt: 0).
        context = alt and not (vk == VK_MENU and not down)
        lparam = key_lparam(vk, down, previous, context)
        if down:
            self.held[vk] |= 0x80
            self.last_key_lparam = (vk, lparam)
        else:
            self.held[vk] &= 0x7f
        self._post((WM_SYSKEYDOWN if down else WM_SYSKEYUP) if system
                   else (WM_KEYDOWN if down else WM_KEYUP), vk, lparam)

    def char(self, code):
        if not 0 < code < 0x100:
            raise ValueError(f'Character {code}')
        lparam = 1
        if self.last_key_lparam and self.held[self.last_key_lparam[0]] & 0x80:
            lparam = self.last_key_lparam[1] & 0x01ff0000 | 1
        self._post(WM_CHAR, code, lparam)

    def _mk(self, buttons):
        flags = sum(mk for bit, _, _, mk, _ in BUTTONS if buttons & bit)
        if self.held[VK_SHIFT] & 0x80:
            flags |= MK_SHIFT
        if self.held[VK_CONTROL] & 0x80:
            flags |= MK_CONTROL
        return flags

    def mouse(self, x, y, buttons):
        if not (0 <= x < SCREEN[0] and 0 <= y < SCREEN[1]) or buttons & ~7:
            raise ValueError(f'Pointer state {(x, y, buttons)}')
        if [x, y] != self.cursor:
            self._move(x, y)
            self._post(WM_MOUSEMOVE, self._mk(self.buttons), self._point())
        for bit, down_message, up_message, _, _ in BUTTONS:
            if (buttons ^ self.buttons) & bit:
                self.buttons ^= bit
                self._post(down_message if buttons & bit else up_message,
                           self._mk(self.buttons), self._point())

    def _point(self):
        return (self.cursor[1] & 0xffff) << 16 | (self.cursor[0] & 0xffff)

    def _move(self, x, y):
        left, top, right, bottom = self.clip or (0, 0) + SCREEN
        self.cursor = [min(max(x, left), right - 1), min(max(y, top), bottom - 1)]

    # ---- user32 --------------------------------------------------------

    def _peek(self, core_peek, args):
        result = core_peek(args)
        if result and args[4] & PM_REMOVE:
            _, message, wparam, _ = struct.unpack('<4I', bytes(self.vm.uc.mem_read(args[0], 16)))
            self._removed(message, wparam)
        return result

    def _removed(self, message, wparam):
        """The thread key state follows the messages the game removes."""
        if message in (WM_KEYDOWN, WM_SYSKEYDOWN):
            if not self.keys[wparam] & 0x80:
                self.keys[wparam] ^= 1                         # toggled on each press
            self.keys[wparam] |= 0x80
        elif message in (WM_KEYUP, WM_SYSKEYUP):
            self.keys[wparam] &= 0x7f
        for _, down_message, up_message, _, vk in BUTTONS:
            if message == down_message:
                self.keys[vk] |= 0x80
            elif message == up_message:
                self.keys[vk] &= 0x7f

    def _keyboard_state(self, args):
        self.vm.write(args[0], bytes(self.keys))
        return 1

    def _cursor_pos(self, args):
        self.vm.write(args[0], struct.pack('<ii', *self.cursor))
        return 1

    def _set_cursor_pos(self, args):
        x, y = struct.unpack('<ii', struct.pack('<II', *args))
        self._move(x, y)
        return 1

    def _clip_cursor(self, args):
        if args[0]:
            left, top, right, bottom = struct.unpack('<4i', bytes(self.vm.uc.mem_read(args[0], 16)))
            self.clip = (max(left, 0), max(top, 0), min(right, SCREEN[0]), min(bottom, SCREEN[1]))
        else:
            self.clip = None
        self._move(*self.cursor)
        return 1

    def _translate(self, args):
        message = self.vm.u32(args[0] + 4)
        return int(message in (WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, WM_SYSKEYUP))
