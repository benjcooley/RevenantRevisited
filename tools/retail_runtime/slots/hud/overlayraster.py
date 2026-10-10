"""Retail's overlay quads as the D3D device draws them.

Retail draws every HUD texture with T3DScene's quad submit (`0x00414550`,
thiscall, 18 stack arguments, `ret 0x48`): x, y, z, the DirectDraw surface,
its IDirect3DTexture2, two unused, width, height, the tint, the texture's
width and height, the source rect, a matrix, and the blend mode. On a
hardware device that is a textured quad: texels mapped 1:1, the texture
modulated by the tint -- blend mode 4 sets COLOROP and ALPHAOP to MODULATE
(TEXTURE x DIFFUSE; T3DScene `0x00417d60`) -- and alpha-blended onto the
back buffer with no z.

The emulator has no such device. Retail's own software rasterizer, which it
falls back to, sets no texture stages: it draws the quads at full opacity,
whatever the tint, and its texture step drifts a texel along a wide quad.
`OverlayRaster` takes over the submit and draws each quad the device's way.
Everything retail composes into its textures stays retail's own code.

Texels: ARGB4444 decodes as the port decodes it (colour c4 << 4, alpha
a4 * 17); RGB565 by bit replication. The blend is computed in float and
written to the RGB565 back buffer truncated. `screen` marks the pixels
whose blend mixed (effective alpha below 1): the port blends in float too,
so those match to within one RGB565 step.
"""
from __future__ import annotations

import struct
import zlib

from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_EIP, UC_X86_REG_ESP

QUAD_SUBMIT, QUAD_ARGS = 0x00414550, 18
BLEND_MODULATE_ALPHA = 0x4
ARGB4444 = (16, 0x0f00, 0x00f0, 0x000f, 0xf000)      # bit count, R, G, B, A masks
RGB565 = (16, 0xf800, 0x07e0, 0x001f, 0)
PF_BITCOUNT = 12                                       # DDPIXELFORMAT dwRGBBitCount


class OverlayRaster:
    def __init__(self, vm, ddraw, back_buffer):
        """`back_buffer()` -> the ddraw Surface quads draw into."""
        self.vm, self.ddraw, self.back_buffer = vm, ddraw, back_buffer
        self.screen = None
        vm.uc.hook_add(UC_HOOK_CODE, self._submit, begin=QUAD_SUBMIT, end=QUAD_SUBMIT)

    def new_frame(self, width, height):
        self.width, self.height = width, height
        self.screen = bytearray(width * height)

    def screen_mask(self):
        """The mixed pixels: one byte per pixel, zlib, hex."""
        return zlib.compress(bytes(self.screen), 9).hex()

    def _submit(self, uc, address, size, user):
        sp = uc.reg_read(UC_X86_REG_ESP)
        a = struct.unpack(f'<{QUAD_ARGS}i', uc.mem_read(sp + 4, 4 * QUAD_ARGS))
        x, y, surface, width, height, tint = a[0], a[1], a[3] & 0xffffffff, a[7], a[8], a[9] & 0xffffffff
        sx, sy, sw, sh, matrix, mode = a[12], a[13], a[14], a[15], a[16], a[17] & 0xffffffff
        if mode != BLEND_MODULATE_ALPHA or matrix:
            raise NotImplementedError(f'quad blend mode 0x{mode:x}, matrix 0x{matrix & 0xffffffff:x}')
        self._draw(x, y, width, height, self.ddraw.surface(surface), sx, sy, sw, sh, tint)
        uc.reg_write(UC_X86_REG_EAX, 1)                    # as the submit returns
        uc.reg_write(UC_X86_REG_EIP, self.vm.u32(sp))
        uc.reg_write(UC_X86_REG_ESP, sp + 4 + 4 * QUAD_ARGS)

    def _texels(self, texture):
        fmt = struct.unpack_from('<5I', texture.pixel_format, PF_BITCOUNT)
        if fmt not in (ARGB4444, RGB565):
            raise NotImplementedError(f'texture format {fmt}')
        rows = bytes(self.vm.uc.mem_read(texture.memory, texture.pitch * texture.height))
        return fmt, rows

    @staticmethod
    def _decode(fmt, value):
        """(r, g, b, a) in 0..255."""
        if fmt == ARGB4444:
            return (value >> 8 & 15) << 4, (value >> 4 & 15) << 4, (value & 15) << 4, (value >> 12) * 17
        r, g, b = value >> 11, value >> 5 & 0x3f, value & 0x1f
        return (r << 3) | (r >> 2), (g << 2) | (g >> 4), (b << 3) | (b >> 2), 255

    def _draw(self, x, y, width, height, texture, sx, sy, sw, sh, tint):
        fmt, texels = self._texels(texture)
        target = self.back_buffer()
        tint_rgba = ((tint >> 16 & 0xff) / 255, (tint >> 8 & 0xff) / 255, (tint & 0xff) / 255,
                     (tint >> 24 & 0xff) / 255)
        for j in range(height):
            ty = y + j
            fy = sy + j * sh // height
            if not (0 <= ty < target.height and 0 <= fy < texture.height):
                continue
            row_at = target.memory + ty * target.pitch
            row = bytearray(self.vm.uc.mem_read(row_at, target.width * 2))
            for i in range(width):
                tx = x + i
                fx = sx + i * sw // width
                if not (0 <= tx < target.width and 0 <= fx < texture.width):
                    continue
                r, g, b, a = self._decode(fmt, struct.unpack_from('<H', texels, fy * texture.pitch + fx * 2)[0])
                alpha = a / 255 * tint_rgba[3]
                if alpha <= 0:
                    continue
                dst = struct.unpack_from('<H', row, tx * 2)[0]
                dr, dg, db, _ = self._decode(RGB565, dst)
                out = [c * t * alpha + d * (1 - alpha)
                       for c, t, d in zip((r, g, b), tint_rgba[:3], (dr, dg, db))]
                r5, g6, b5 = (int(out[0] + 0.5) >> 3, int(out[1] + 0.5) >> 2, int(out[2] + 0.5) >> 3)
                struct.pack_into('<H', row, tx * 2, (r5 << 11) | (g6 << 5) | b5)
                if alpha < 1 and self.screen is not None and ty < self.height and tx < self.width:
                    self.screen[ty * self.width + tx] = 1
            self.vm.write(row_at, bytes(row))
