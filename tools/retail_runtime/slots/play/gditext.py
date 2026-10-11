r"""Glyphs for the GDI text the HUD machine records but never draws.

Retail draws its WINFONT text (FONT.DEF's Arial / Times New Roman: dialog,
the text bar, panes) with GDI: the compose `0x004be2b0` clears a work
surface to the key 0x7f7f, takes its DC (IDirectDrawSurface4::GetDC),
draws each line with DrawTextA -- black at (+1, 0) and (0, +1) for the
shadow, then the text colour, background TRANSPARENT -- releases the DC
and blits the surface onto the screen with the key. `slots/hud/gdi.py`
records those calls and draws nothing (the HUD A/B compares text calls),
so retail's composite finds an empty surface and frames carry no text.

This module, used by the play slot only, wraps TextOutA / DrawTextA to
also draw the glyphs into the DC's surface, so retail's own compose and
shadow passes work on real pixels. Approximate by nature, and documented
as such: glyph shapes are FreeType's (Pillow), hinted and monochrome
(Windows 98 drew TrueType without smoothing), from the same font files
gdi.py measures; their placement follows gdi.py's GDI metrics (advance
widths, ascent, cell height) -- the numbers retail itself laid the text
out with -- so a string fills the box retail measured for it.

TextOutA: TA_LEFT / TA_CENTER / TA_RIGHT, TA_TOP / TA_BASELINE /
TA_BOTTOM, TA_UPDATECP; an OPAQUE background fills the cell. DrawTextA:
DT_CENTER / DT_RIGHT, DT_VCENTER / DT_BOTTOM with DT_SINGLELINE,
DT_WORDBREAK, explicit line breaks, DT_NOCLIP; DT_CALCRECT draws nothing.
Pixels are written in the surface's own format (its RGB masks; alpha
bits kept), clipped to the surface and, unless DT_NOCLIP, to the rect.
"""
from __future__ import annotations

import struct
from pathlib import Path

from PIL import ImageFont

from gdi import FACES, OPAQUE

TA_UPDATECP, TA_RIGHT, TA_CENTER, TA_BOTTOM, TA_BASELINE = 0x1, 0x2, 0x6, 0x8, 0x18
DT_CENTER, DT_RIGHT, DT_VCENTER, DT_BOTTOM = 0x1, 0x2, 0x4, 0x8
DT_WORDBREAK, DT_SINGLELINE, DT_NOCLIP, DT_CALCRECT = 0x10, 0x20, 0x100, 0x400


def signed(value):
    return struct.unpack('<i', struct.pack('<I', value & 0xffffffff))[0]


class GdiText:
    def __init__(self, vm, gdi, ddraw):
        self.vm, self.gdi, self.ddraw = vm, gdi, ddraw
        self._fonts = {}
        self.drawn = 0                        # strings drawn
        text_out, draw_text = vm.handlers['TextOutA'][1], vm.handlers['DrawTextA'][1]
        vm.handlers['TextOutA'] = (5, lambda a: self._text_out(text_out, a))
        vm.handlers['DrawTextA'] = (5, lambda a: self._draw_text(draw_text, a))

    # ---- GDI -------------------------------------------------------------

    def _text_out(self, record, args):
        result = record(args)
        handle, x, y, text, count = args
        dc = self.gdi.dcs[handle]
        data = bytes(self.vm.uc.mem_read(text, count))
        x, y = signed(x), signed(y)
        if dc.align & TA_UPDATECP:
            x, y = dc.position
        metrics = self.gdi._font(dc).metrics
        width = self.gdi.width(dc, data)
        if dc.align & TA_CENTER == TA_CENTER:
            x -= width // 2
        elif dc.align & TA_RIGHT:
            x -= width
        if dc.align & TA_BASELINE == TA_BASELINE:
            y -= metrics.ascent
        elif dc.align & TA_BOTTOM:
            y -= metrics.height
        if dc.back_mode == OPAQUE:
            self._fill(dc, (x, y, x + width, y + metrics.height), dc.back_color)
        self._line(dc, data, x, y, None)
        if dc.align & TA_UPDATECP:
            dc.position = (x + width if not dc.align & (TA_RIGHT | TA_CENTER) else x, dc.position[1])
        return result

    def _draw_text(self, record, args):
        result = record(args)
        handle, text, count, rect, flags = args
        if flags & DT_CALCRECT:
            return result
        dc = self.gdi.dcs[handle]
        data = (self.vm.string(text).encode('cp1252') if count == 0xffffffff
                else bytes(self.vm.uc.mem_read(text, count)))
        left, top, right, bottom = struct.unpack('<4i', bytes(self.vm.uc.mem_read(rect, 16)))
        metrics = self.gdi._font(dc).metrics
        lines = [data.replace(b'\r', b'').replace(b'\n', b' ')] if flags & DT_SINGLELINE else \
            self._wrap(dc, data, right - left if flags & DT_WORDBREAK else None)
        height = metrics.height * len(lines)
        y = top
        if flags & DT_SINGLELINE and flags & DT_VCENTER:
            y = top + (bottom - top - height) // 2
        elif flags & DT_SINGLELINE and flags & DT_BOTTOM:
            y = bottom - height
        clip = None if flags & DT_NOCLIP else (left, top, right, bottom)
        for line in lines:
            width = self.gdi.width(dc, line)
            x = (left + (right - left - width) // 2 if flags & DT_CENTER
                 else right - width if flags & DT_RIGHT else left)
            if dc.back_mode == OPAQUE:
                self._fill(dc, (x, y, x + width, y + metrics.height), dc.back_color, clip)
            self._line(dc, line, x, y, clip)
            y += metrics.height
        return result

    def _wrap(self, dc, data, width):
        """Lines at explicit breaks, then (DT_WORDBREAK) at spaces so each
        fits `width`; a word wider than the box stands alone."""
        lines = []
        for paragraph in data.replace(b'\r\n', b'\n').split(b'\n'):
            if width is None:
                lines.append(paragraph)
                continue
            current = b''
            for word in paragraph.split(b' '):
                candidate = word if not current else current + b' ' + word
                if current and self.gdi.width(dc, candidate) > width:
                    lines.append(current)
                    current = word
                else:
                    current = candidate
            lines.append(current)
        return lines

    # ---- pixels ------------------------------------------------------------

    def _surface(self, dc):
        return self.ddraw.surfaces.get(dc.surface)

    def _pixel(self, surface, colorref):
        """A COLORREF (0x00BBGGRR) in the surface's format: its RGB masks."""
        r, g, b = colorref & 0xff, (colorref >> 8) & 0xff, (colorref >> 16) & 0xff
        masks = struct.unpack_from('<3I', surface.pixel_format, 16)
        value = 0
        for channel, mask in zip((r, g, b), masks):
            if mask:
                shift = (mask & -mask).bit_length() - 1
                bits = (mask >> shift).bit_length()
                value |= (channel >> (8 - bits)) << shift
        return value

    def _put(self, surface, points, value, clip):
        bpp = surface.bytes_per_pixel
        if bpp not in (2, 4):
            return
        alpha = struct.unpack_from('<I', surface.pixel_format, 28)[0]
        x0, y0, x1, y1 = 0, 0, surface.width, surface.height
        if clip:
            x0, y0, x1, y1 = max(x0, clip[0]), max(y0, clip[1]), min(x1, clip[2]), min(y1, clip[3])
        rows = {}
        for x, y in points:
            if x0 <= x < x1 and y0 <= y < y1:
                rows.setdefault(y, []).append(x)
        code = '<H' if bpp == 2 else '<I'
        for y, xs in rows.items():
            start, end = min(xs), max(xs) + 1
            address = surface.memory + y * surface.pitch + start * bpp
            row = bytearray(self.vm.uc.mem_read(address, (end - start) * bpp))
            for x in xs:
                offset = (x - start) * bpp
                old = struct.unpack_from(code, row, offset)[0]
                struct.pack_into(code, row, offset, (old & alpha) | (value & ~alpha))
            self.vm.write(address, bytes(row))

    def _fill(self, dc, box, colorref, clip=None):
        surface = self._surface(dc)
        if surface is None:
            return
        left, top, right, bottom = box
        points = [(x, y) for y in range(top, bottom) for x in range(left, right)]
        self._put(surface, points, self._pixel(surface, colorref), clip)

    def _face(self, dc):
        font = self.gdi._font(dc)
        key = (font.face.lower(), font.bold, font.metrics.ppem)
        if key not in self._fonts:
            self._fonts[key] = ImageFont.truetype(str(Path(FACES[key[:2]])), font.metrics.ppem)
        return self._fonts[key], font.metrics

    def _line(self, dc, data, x, y, clip):
        """One line of text, top-left at (x, y): each glyph's FreeType
        bitmap at the pen position gdi.py's advances give, its baseline at
        y + the GDI ascent."""
        surface = self._surface(dc)
        if surface is None or not data:
            return
        face, metrics = self._face(dc)
        ascent = face.getmetrics()[0]
        baseline = y + metrics.ascent
        points = []
        pen = x
        for byte in data:
            char = bytes([byte]).decode('cp1252', errors='replace')
            if char.strip():
                mask, (ox, oy) = face.getmask2(char, mode='1')
                width, height = mask.size
                pixels = bytes(mask)
                for my in range(height):
                    for mx in range(width):
                        if pixels[my * width + mx]:
                            points.append((pen + ox + mx, baseline - ascent + oy + my))
            pen += metrics.advance(byte) + dc.char_extra
        self._put(surface, points, self._pixel(surface, dc.text_color), clip)
        self.drawn += 1
