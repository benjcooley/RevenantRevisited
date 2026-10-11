#!/usr/bin/env python3
"""Play slot pieces that need no booted game: input messages, the write
overlay, frames, the machine INI.

    "$PY" -m unittest discover -s tools/retail_runtime/slots/play -p 'test_*.py'
"""
import struct
import sys
import tempfile
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(HERE.parent / 'hud'))
sys.path.insert(0, str(HERE.parents[1]))
from runtime import Runtime  # noqa: E402
from build_contract import BASELINE  # noqa: E402
from winfs import WinFileSystem, find_match  # noqa: E402
from ddraw import DirectDraw, RGB565  # noqa: E402
from gdi import Gdi  # noqa: E402
from gditext import GdiText  # noqa: E402
import frames  # noqa: E402
from overlay import OverlayError, SaveOverlay  # noqa: E402
from playworld import machine_ini  # noqa: E402
from userinput import UserInput  # noqa: E402

HWND = 0x1000
VK_A, VK_LEFT, VK_SHIFT, VK_CONTROL, VK_MENU, VK_CAPITAL = 0x41, 0x25, 0x10, 0x11, 0x12, 0x14


class InputTests(unittest.TestCase):
    def setUp(self):
        self.vm = Runtime(BASELINE)
        self.vm.windows[HWND] = 0x00448cc0
        self.input = UserInput(self.vm, HWND)

    def posted(self):
        messages = [m[1:4] for m in self.vm.messages]
        self.vm.messages.clear()
        return messages

    def test_keys_carry_scan_codes_repeat_and_transition_bits(self):
        events = [{'key': VK_A, 'down': True}, {'key': VK_A, 'down': True}, {'key': VK_A, 'down': False},
                  {'key': VK_LEFT, 'down': True}, {'key': VK_LEFT, 'down': False}]
        self.input.deliver(events)
        self.assertEqual(self.posted(), [(0x100, VK_A, 0x001e0001), (0x100, VK_A, 0x401e0001),
                                         (0x101, VK_A, 0xc01e0001), (0x100, VK_LEFT, 0x014b0001),
                                         (0x101, VK_LEFT, 0xc14b0001)])

    def test_alt_makes_system_keys_and_ctrl_alt_does_not(self):
        self.input.deliver([{'key': VK_MENU, 'down': True}, {'key': VK_A, 'down': True},
                            {'key': VK_A, 'down': False}, {'key': VK_MENU, 'down': False}])
        self.assertEqual(self.posted(), [(0x104, VK_MENU, 0x20380001), (0x104, VK_A, 0x201e0001),
                                         (0x105, VK_A, 0xe01e0001), (0x105, VK_MENU, 0xc0380001)])
        self.input.deliver([{'key': VK_CONTROL, 'down': True}, {'key': VK_MENU, 'down': True},
                            {'key': VK_A, 'down': True}])
        self.assertEqual([m[0] for m in self.posted()], [0x100, 0x100, 0x100])

    def test_char_takes_the_held_keys_scan_code(self):
        self.input.deliver([{'key': VK_A, 'down': True}, {'char': 97}, {'key': VK_A, 'down': False},
                            {'char': 13}])
        self.assertEqual(self.posted()[1::2], [(0x102, 97, 0x001e0001), (0x102, 13, 1)])

    def test_mouse_moves_then_buttons_with_mk_flags_and_point(self):
        self.input.deliver([{'key': VK_SHIFT, 'down': True}, {'mouse': [100, 50], 'buttons': 1},
                            {'mouse': [100, 50], 'buttons': 3}, {'mouse': [101, 50], 'buttons': 0}])
        point = 50 << 16
        self.assertEqual(self.posted()[1:], [(0x200, 0x4, point | 100), (0x201, 0x5, point | 100),
                                             (0x204, 0x7, point | 100), (0x200, 0x7, point | 101),
                                             (0x202, 0x6, point | 101), (0x205, 0x4, point | 101)])
        self.assertEqual(self.input.cursor, [101, 50])

    def test_keyboard_state_follows_removed_messages_and_cursor_follows_clip(self):
        self.input.deliver([{'key': VK_CAPITAL, 'down': True}, {'key': VK_CAPITAL, 'down': False},
                            {'key': VK_A, 'down': True}])
        state = self.vm.allocate(256)
        self.vm.call(self.vm.api_address('user32.dll', 'GetKeyboardState'), (state,))
        self.assertEqual(self.vm.uc.mem_read(state + VK_A, 1)[0], 0)      # nothing removed yet
        msg = self.vm.allocate(28)
        while self.vm.call(self.vm.api_address('user32.dll', 'PeekMessageA'), (msg, 0, 0, 0, 1)):
            pass
        self.vm.call(self.vm.api_address('user32.dll', 'GetKeyboardState'), (state,))
        self.assertEqual((self.vm.uc.mem_read(state + VK_A, 1)[0], self.vm.uc.mem_read(state + VK_CAPITAL, 1)[0]),
                         (0x81, 0x01))                                       # held + toggled; toggled
        rect = self.vm.allocate(16)
        self.vm.write(rect, struct.pack('<4i', 10, 20, 110, 120))
        self.vm.call(self.vm.api_address('user32.dll', 'ClipCursor'), (rect,))
        self.input.mouse(300, 5, 0)
        point = self.vm.allocate(8)
        self.vm.call(self.vm.api_address('user32.dll', 'GetCursorPos'), (point,))
        self.assertEqual(struct.unpack('<2i', self.vm.uc.mem_read(point, 8)), (109, 20))


class OverlayTests(unittest.TestCase):
    def setUp(self):
        self.vm = Runtime(BASELINE)
        self.fs = WinFileSystem(self.vm)
        self.fs.mount('C:\\REVENANT\\Modules\\x.dat', b'install')
        self.overlay = SaveOverlay(self.vm, self.fs)

    def create(self, path, access=0x40000000, disposition=2):
        name = self.vm.allocate(len(path) + 1)
        self.vm.write(name, path.encode() + b'\0')
        return self.vm.call(self.vm.api_address('kernel32.dll', 'CreateFileA'),
                            (name, access, 0, 0, disposition, 0x80, 0))

    def test_writes_land_in_the_overlay_and_elsewhere_fail_by_name(self):
        directory = self.vm.allocate(64)
        self.vm.write(directory, b'C:\\REVENANT\\Save\\Single\\Quick Save 1\0')
        self.vm.call(self.vm.api_address('kernel32.dll', 'CreateDirectoryA'), (directory, 0))
        handle = self.create('C:\\REVENANT\\Save\\Single\\Quick Save 1\\game.sav')
        self.assertNotEqual(handle, 0xffffffff)
        self.assertEqual(self.overlay.last_slot, 'quick save 1')
        with self.assertRaisesRegex(OverlayError, 'outside the save overlay'):
            self.create('C:\\REVENANT\\Modules\\x.dat', disposition=3)
        self.assertIsInstance(self.vm.files['c:/revenant/modules/x.dat'], bytes)
        self.create('C:\\REVENANT\\CurMap\\2_1_1.DAT')
        with tempfile.TemporaryDirectory() as temp:
            name, path = self.overlay.export_slot('@last', Path(temp) / 'out')
            self.assertEqual(name, 'Quick Save 1')
            self.assertEqual(sorted(p.name for p in path.iterdir()), ['game.sav'])

    def test_find_patterns_match_as_windows(self):
        self.assertTrue(find_match('quick save 1', '*.*'))
        self.assertTrue(find_match('game.sav', '*.*'))
        self.assertTrue(find_match('revboot', 'revboot.*'))
        self.assertFalse(find_match('revboot.log', '*.sav'))

    def test_place_slot_keeps_names_for_the_games_scan(self):
        with tempfile.TemporaryDirectory() as temp:
            slot = Path(temp) / 'Keep After Opening'
            (slot / 'CurMap').mkdir(parents=True)
            (slot / 'game.sav').write_bytes(b'sav')
            (slot / 'CurMap' / '2_1_1.DAT').write_bytes(b'sector')
            self.assertEqual(self.overlay.place_slot(slot, slot.name), 2)
        find = self.vm.allocate(320)
        pattern = self.vm.allocate(64)
        self.vm.write(pattern, b'C:\\REVENANT\\Save\\Single\\*.*\0')     # retail's own slot scan pattern
        self.vm.call(self.vm.api_address('kernel32.dll', 'FindFirstFileA'), (pattern, find))
        self.assertEqual(self.vm.string(find + 44), 'Keep After Opening')
        self.assertEqual(self.overlay.slot_name('keep after opening'), 'Keep After Opening')
        self.assertEqual(self.overlay.slot_files('Keep After Opening')['CurMap/2_1_1.DAT'], b'sector')


class FrameTests(unittest.TestCase):
    def test_rgb565_widens_by_bit_replication(self):
        pixels = struct.pack('<4H', 0xf800, 0x07e0, 0x8410, 0xffff)
        self.assertEqual(frames.rgb888(pixels), bytes([255, 0, 0, 0, 255, 0, 132, 130, 132, 255, 255, 255]))


class GlyphTests(unittest.TestCase):
    def test_draw_text_puts_glyphs_in_the_dc_surface_inside_its_rect(self):
        vm = Runtime(BASELINE)
        gdi, ddraw = Gdi(vm), DirectDraw(vm)
        ddraw.gdi = gdi
        text = GdiText(vm, gdi, ddraw)
        surface = ddraw._new_surface(200, 40, 0, RGB565)
        dc = gdi.create_dc(surface)
        face = vm.allocate(16)
        vm.write(face, b'Arial\0')
        font = gdi._create_font((0xfffffff4, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, face))   # -12: 12 ppem
        gdi._select((dc, font))
        gdi._swap((dc, 0x0000ff), 'text_color')                                            # red
        gdi._swap((dc, 1), 'back_mode')                                                    # TRANSPARENT
        call = vm.api_address('user32.dll', 'DrawTextA')
        string, rect = vm.allocate(16), vm.allocate(16)
        vm.write(string, b'Hi there\0')
        vm.write(rect, struct.pack('<4i', 20, 5, 180, 30))
        vm.call(call, (dc, string, 0xffffffff, rect, 0x2811))           # centred, word break
        s = ddraw.surface(surface)
        pixels = struct.unpack(f'<{s.pitch // 2 * s.height}H', vm.uc.mem_read(s.memory, s.pitch * s.height))
        lit = [(i % (s.pitch // 2), i // (s.pitch // 2)) for i, v in enumerate(pixels) if v]
        self.assertTrue(lit)
        self.assertEqual({pixels[y * (s.pitch // 2) + x] for x, y in lit}, {0xf800})
        xs, ys = [p[0] for p in lit], [p[1] for p in lit]
        self.assertTrue(20 <= min(xs) and max(xs) < 180 and 5 <= min(ys) and max(ys) < 30)
        self.assertLess(abs((min(xs) + max(xs)) / 2 - 100), 4)         # centred in 20..180
        self.assertEqual(len(gdi.text_calls), 1)                         # still recorded
        vm.write(rect, struct.pack('<4i', 20, 5, 180, 30))
        before = bytes(vm.uc.mem_read(s.memory, s.pitch * s.height))
        vm.call(call, (dc, string, 0xffffffff, rect, 0x400))            # DT_CALCRECT draws nothing
        self.assertEqual(bytes(vm.uc.mem_read(s.memory, s.pitch * s.height)), before)
        self.assertEqual(text.drawn, 1)


class MachineTests(unittest.TestCase):
    def test_software_machine_sets_only_software3d(self):
        ini = machine_ini('software')
        self.assertIn(b'Software3D = Yes', ini)
        self.assertNotIn(b'Software3D = No', ini)
        self.assertIsNone(machine_ini('hud'))
        with self.assertRaises(ValueError):
            machine_ini('hardware')


if __name__ == '__main__':
    unittest.main()
