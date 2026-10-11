"""Where retail's HUD textures hold colours it blended in 4-bit steps.

Retail composes a pane's art into ARGB4444 textures with its 2D blitters.
Where a layer with partial alpha (0 < a < 15) is blended over another
(DM_ALPHA: the Ring and the icons over the BackPanel and the portrait),
each step truncates the result to 4 bits a channel; the port blends the
same layers in float on the GPU (docs/ui/HUD_REBUILD.md section 5).
tools/retail_ab/hud_ab.py compares the screen pixels drawn from those
texels within the composition tolerance.

`BlendMap` follows the primitives a pane calls, per surface:
- a Put whose draw mode (its own, or the bitmap's for DM_USEDEFAULT) has
  DM_ALPHA marks the texels its bitmap's partial-alpha texels blend into;
- a Box clears its rect;
- a surface-to-surface ParamBlit copies the source rect's marks;
- a Quad (`0x00414d70`) marks the screen pixels it draws from marked texels.

The marks are conservative: an opaque layer drawn over a marked texel
leaves the mark. How the quads themselves blend onto the screen is
`overlayraster.py`'s.
"""
from __future__ import annotations

import struct
import zlib

BM_ARGB4444 = 0x10000
DM_ALPHA, DM_USEDEFAULT = 0x2000, 0x80000000
TBITMAP_HEADER = 0x48
TBITMAP_FLAGS, TBITMAP_DRAWMODE = 0x10, 0x14
SURFACE_WIDTH, SURFACE_HEIGHT = 0x04, 0x08     # TSurface fields


class BlendMap:
    def __init__(self, vm, screen=(640, 480)):
        self.vm = vm
        self.size = screen
        self.surfaces: dict[int, tuple[int, int, bytearray]] = {}
        self.screen = bytearray(screen[0] * screen[1])

    def new_frame(self):
        """The screen's marks start over (surfaces keep theirs)."""
        self.screen = bytearray(self.size[0] * self.size[1])

    def screen_mask(self):
        """The screen's marks: one byte per pixel, zlib, hex."""
        return zlib.compress(bytes(self.screen), 9).hex()

    def _surface(self, pointer):
        if pointer not in self.surfaces:
            width, height = self.vm.u32(pointer + SURFACE_WIDTH), self.vm.u32(pointer + SURFACE_HEIGHT)
            if not (0 < width <= 4096 and 0 < height <= 4096):
                raise ValueError(f'surface 0x{pointer:08x} reports {width}x{height}')
            self.surfaces[pointer] = (width, height, bytearray(width * height))
        return self.surfaces[pointer]

    def put(self, surface, x, y, bitmap, mode):
        width, height = struct.unpack('<II', self.vm.uc.mem_read(bitmap, 8))
        flags, drawmode = self.vm.u32(bitmap + TBITMAP_FLAGS), self.vm.u32(bitmap + TBITMAP_DRAWMODE)
        if mode & DM_USEDEFAULT:
            mode = drawmode
        if not (flags & BM_ARGB4444 and mode & DM_ALPHA):
            return
        sw, sh, marks = self._surface(surface)
        pixels = struct.unpack(f'<{width * height}H',
                               self.vm.uc.mem_read(bitmap + TBITMAP_HEADER, width * height * 2))
        for j in range(height):
            ty = y + j
            if not 0 <= ty < sh:
                continue
            for i in range(width):
                tx = x + i
                if 0 <= tx < sw and 0 < pixels[j * width + i] >> 12 < 15:
                    marks[ty * sw + tx] = 1

    def box(self, surface, left, top, width, height):
        sw, sh, marks = self._surface(surface)
        x0, x1 = max(left, 0), min(left + width, sw)
        for ty in range(max(top, 0), min(top + height, sh)):
            if x1 > x0:
                marks[ty * sw + x0:ty * sw + x1] = bytes(x1 - x0)

    def blit(self, dst, src, dx, dy, sx, sy, width, height):
        dw, dh, dmarks = self._surface(dst)
        swid, sht, smarks = self._surface(src)
        for j in range(height):
            ty, fy = dy + j, sy + j
            if not (0 <= ty < dh and 0 <= fy < sht):
                continue
            for i in range(width):
                tx, fx = dx + i, sx + i
                if 0 <= tx < dw and 0 <= fx < swid:
                    dmarks[ty * dw + tx] = smarks[fy * swid + fx]

    def quad(self, x, y, width, height, texture, sx, sy):
        tw, th, marks = self._surface(texture)
        sw = self.size[0]
        for j in range(height):
            ty, fy = y + j, sy + j
            if not (0 <= ty < self.size[1] and 0 <= fy < th):
                continue
            for i in range(width):
                tx, fx = x + i, sx + i
                if 0 <= tx < sw and 0 <= fx < tw and marks[fy * tw + fx]:
                    self.screen[ty * sw + tx] = 1
