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


if __name__ == '__main__':
    unittest.main()
