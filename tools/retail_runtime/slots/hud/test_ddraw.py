#!/usr/bin/env python3
"""The fake ddraw.dll's surfaces, over a flat guest memory (no boot image)."""
from __future__ import annotations

import struct
import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(HERE.parents[1]))
import ddraw  # noqa: E402

BASE = 0x10000000


class Memory:
    """Just enough of the VM for DirectDraw: a bump allocator over a bytearray."""

    def __init__(self, size=1 << 20):
        self.bytes = bytearray(size)
        self.next = BASE
        self.handlers, self.api_dlls = {}, {}
        self.uc = self

    def allocate(self, size):
        address, self.next = self.next, self.next + ((size + 15) & ~15)
        return address

    def mem_read(self, address, size):
        return bytes(self.bytes[address - BASE:address - BASE + size])

    def write(self, address, data):
        self.bytes[address - BASE:address - BASE + len(data)] = data

    write_code = write

    def put_u32(self, address, value):
        self.write(address, struct.pack('<I', value & 0xffffffff))

    def u32(self, address):
        return struct.unpack_from('<I', self.bytes, address - BASE)[0]

    def api_address(self, dll, name):
        return 0x7f000000 + len(name)


def surface_desc(memory, width, height, flags, pitch=0, surface=0):
    """A DDSURFACEDESC2 for a 16-bit texture in guest memory -> its address."""
    raw = bytearray(ddraw.SDESC_SIZE)
    struct.pack_into('<5I', raw, 0, ddraw.SDESC_SIZE, flags, height, width, pitch)
    struct.pack_into('<I', raw, ddraw.SD_SURFACE, surface)
    raw[ddraw.SD_PIXELFORMAT:ddraw.SD_PIXELFORMAT + 32] = ddraw.RGB565
    struct.pack_into('<I', raw, ddraw.SD_CAPS, ddraw.DDSCAPS_TEXTURE | ddraw.DDSCAPS_SYSTEMMEMORY)
    address = memory.allocate(len(raw))
    memory.write(address, bytes(raw))
    return address


class CreateSurfaceTest(unittest.TestCase):
    def setUp(self):
        self.memory = Memory()
        self.dd = ddraw.DirectDraw(self.memory)
        self.out = self.memory.allocate(4)

    def create(self, *args, **kwargs):
        self.assertEqual(self.dd._create_surface(surface_desc(self.memory, *args, **kwargs), self.out, 0), ddraw.DD_OK)
        return self.dd.surface(self.memory.u32(self.out))

    def test_ignores_the_callers_pitch_for_memory_it_allocates(self):
        # Retail's texture upload (0x0040bb30) passes DDSD_PITCH with the
        # width in texels; DirectDraw picks the pitch of a surface it allocates.
        flags = ddraw.DDSD_CAPS | ddraw.DDSD_WIDTH | ddraw.DDSD_HEIGHT | ddraw.DDSD_PITCH | ddraw.DDSD_PIXELFORMAT
        s = self.create(32, 32, flags, pitch=32)
        self.assertEqual(s.pitch, 64)
        self.assertGreaterEqual(self.memory.u32(self.out) - s.memory, s.pitch * s.height,
                                'the pixels overlap the surface object')

    def test_keeps_the_callers_pitch_for_its_own_memory(self):
        pixels = self.memory.allocate(128 * 8)
        flags = (ddraw.DDSD_CAPS | ddraw.DDSD_WIDTH | ddraw.DDSD_HEIGHT | ddraw.DDSD_PITCH
                 | ddraw.DDSD_PIXELFORMAT | ddraw.DDSD_LPSURFACE)
        s = self.create(32, 8, flags, pitch=128, surface=pixels)
        self.assertEqual((s.pitch, s.memory), (128, pixels))


class PresentAndStateTest(unittest.TestCase):
    """Flip, keyed blits and the device's state getters (play slot)."""

    def setUp(self):
        self.memory = Memory()
        self.dd = ddraw.DirectDraw(self.memory, width=8, height=4)
        self.out = self.memory.allocate(4)

    def plain(self, width, height):
        desc = surface_desc(self.memory, width, height, ddraw.DDSD_CAPS | ddraw.DDSD_WIDTH | ddraw.DDSD_HEIGHT
                            | ddraw.DDSD_PIXELFORMAT)
        self.dd._create_surface(desc, self.out, 0)
        return self.memory.u32(self.out)

    def test_flip_trades_the_front_and_back_memory(self):
        raw = bytearray(ddraw.SDESC_SIZE)
        struct.pack_into('<II', raw, 0, ddraw.SDESC_SIZE, ddraw.DDSD_CAPS | ddraw.DDSD_BACKBUFFERCOUNT)
        struct.pack_into('<I', raw, ddraw.SD_BACKBUFFERS, 1)
        struct.pack_into('<I', raw, ddraw.SD_CAPS, ddraw.DDSCAPS_PRIMARYSURFACE | ddraw.DDSCAPS_FLIP
                         | ddraw.DDSCAPS_COMPLEX)
        desc = self.memory.allocate(len(raw))
        self.memory.write(desc, bytes(raw))
        self.dd._create_surface(desc, self.out, 0)
        front = self.dd.surface(self.dd.primary)
        back = self.dd.surface(front.attached[0])
        before = (front.memory, back.memory)
        self.assertEqual(self.dd._surface_method('IDirectDrawSurface4', 'Flip', (self.dd.primary, 0, 1)), ddraw.DD_OK)
        self.assertEqual((back.memory, front.memory, self.dd.flips), before + (1,))

    def test_keyed_blit_keeps_the_destination_under_key_pixels(self):
        source, target = self.plain(4, 1), self.plain(4, 1)
        src, dst = self.dd.surface(source), self.dd.surface(target)
        self.memory.write(src.memory, struct.pack('<4H', 0x1234, 0xf81f, 0xf81f, 0x0001))
        self.memory.write(dst.memory, struct.pack('<4H', 7, 7, 7, 7))
        src.color_keys[ddraw.DDCKEY_SRCBLT] = (0xf81f, 0xf81f)
        self.dd._blt_fast(dst, 0, 0, source, 0, ddraw.DDBLTFAST_SRCCOLORKEY)
        self.assertEqual(struct.unpack('<4H', self.memory.mem_read(dst.memory, 8)), (0x1234, 7, 7, 0x0001))

    def test_device_states_read_back_what_was_set(self):
        device = self.dd._object('IDirect3DDevice3')
        value = self.memory.allocate(8)
        call = self.dd._device_method
        self.assertEqual(call('IDirect3DDevice3', 'GetLightState', (device, 1, value)), ddraw.DD_OK)
        self.assertEqual(self.memory.u32(value), 0)                  # MATERIAL: none until set
        call('IDirect3DDevice3', 'SetLightState', (device, 1, 0x55))
        call('IDirect3DDevice3', 'GetLightState', (device, 1, value))
        self.assertEqual(self.memory.u32(value), 0x55)
        with self.assertRaises(ddraw.MissingAPI):
            call('IDirect3DDevice3', 'GetRenderState', (device, 7, value))
        viewport = self.dd._object('IDirect3DViewport3')
        call('IDirect3DViewport3', 'GetBackground', (viewport, value, value + 4))
        self.assertEqual((self.memory.u32(value), self.memory.u32(value + 4)), (0, 0))
        call('IDirect3DViewport3', 'SetBackground', (viewport, 0x99))
        call('IDirect3DViewport3', 'GetBackground', (viewport, value, value + 4))
        self.assertEqual((self.memory.u32(value), self.memory.u32(value + 4)), (0x99, 1))


if __name__ == '__main__':
    unittest.main()
