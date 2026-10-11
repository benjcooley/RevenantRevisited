"""TrueType device metrics, read the way GDI reads them (no dependencies).

`TrueTypeFont(path).metrics(lf_height)` maps a LOGFONT height to a pixel
size and returns integer metrics at that size:

- lf_height < 0: em height in pixels (ppem = -lf_height).
- lf_height > 0: cell height (ascent + descent). The largest size whose
  VDMX cell fits (yMax - yMin <= lf_height), or the scaled win metrics when
  the font has no VDMX entry.
- ascent/descent: the VDMX entry for the size, else the OS/2 win metrics
  scaled and rounded up; internal leading = cell - ppem.
- advances: the hdmx device width for the size (what GDI uses for hinted
  advances), else the hmtx advance scaled and rounded.

These are the font file's own device metrics. Windows 98's exact values come
from a capture of real GDI (docs/ui/HUD_REBUILD.md P6); this is the stand-in
until then, documented as such in every result that uses it.
"""
from __future__ import annotations

import math
import struct
from dataclasses import dataclass, field
from pathlib import Path


@dataclass
class FontMetrics:
    ppem: int
    ascent: int
    descent: int
    internal_leading: int
    external_leading: int
    average_width: int
    max_width: int
    widths: dict = field(default_factory=dict)        # cp1252 byte -> advance

    @property
    def height(self):
        return self.ascent + self.descent

    def advance(self, byte):
        return self.widths.get(byte, self.widths.get(0x20, self.average_width))


class TrueTypeFont:
    def __init__(self, path: Path):
        self.data = path.read_bytes()
        self.tables = {}
        count = struct.unpack_from('>H', self.data, 4)[0]
        for i in range(count):
            tag, _check, offset, length = struct.unpack_from('>4sIII', self.data, 12 + 16 * i)
            self.tables[tag.decode('latin1')] = (offset, length)
        head = self._table('head')
        self.units = struct.unpack_from('>H', head, 18)[0]
        self.x_min, _y_min, self.x_max, _y_max = struct.unpack_from('>hhhh', head, 36)
        hhea = self._table('hhea')
        self.hhea_ascender, self.hhea_descender, self.hhea_gap = struct.unpack_from('>hhh', hhea, 4)
        metrics_count = struct.unpack_from('>H', hhea, 34)[0]
        os2 = self._table('OS/2')
        self.average = struct.unpack_from('>h', os2, 2)[0]
        self.win_ascent, self.win_descent = struct.unpack_from('>HH', os2, 74)
        self.glyph_count = struct.unpack_from('>H', self._table('maxp'), 4)[0]
        hmtx = self._table('hmtx')
        advances = [struct.unpack_from('>H', hmtx, 4 * i)[0] for i in range(metrics_count)]
        self.advances = advances + [advances[-1]] * (self.glyph_count - metrics_count)
        self.cmap = self._cmap()
        self.hdmx = self._hdmx()
        self.vdmx = self._vdmx()

    def _table(self, tag):
        offset, length = self.tables[tag]
        return self.data[offset:offset + length]

    def _cmap(self):
        """Unicode BMP -> glyph from the (3,1) format 4 subtable."""
        cmap = self._table('cmap')
        count = struct.unpack_from('>H', cmap, 2)[0]
        for i in range(count):
            platform, encoding, offset = struct.unpack_from('>HHI', cmap, 4 + 8 * i)
            if (platform, encoding) == (3, 1) and struct.unpack_from('>H', cmap, offset)[0] == 4:
                return self._format4(cmap, offset)
        raise ValueError('No Windows Unicode format 4 cmap')

    @staticmethod
    def _format4(cmap, offset):
        segments = struct.unpack_from('>H', cmap, offset + 6)[0] // 2
        ends = struct.unpack_from(f'>{segments}H', cmap, offset + 14)
        starts = struct.unpack_from(f'>{segments}H', cmap, offset + 16 + 2 * segments)
        deltas = struct.unpack_from(f'>{segments}h', cmap, offset + 16 + 4 * segments)
        range_base = offset + 16 + 6 * segments
        ranges = struct.unpack_from(f'>{segments}H', cmap, range_base)
        mapping = {}
        for i in range(segments):
            for code in range(starts[i], ends[i] + 1):
                if code == 0xffff:
                    continue
                if ranges[i] == 0:
                    glyph = (code + deltas[i]) & 0xffff
                else:
                    address = range_base + 2 * i + ranges[i] + 2 * (code - starts[i])
                    glyph = struct.unpack_from('>H', cmap, address)[0]
                    if glyph:
                        glyph = (glyph + deltas[i]) & 0xffff
                if glyph:
                    mapping[code] = glyph
        return mapping

    def _hdmx(self):
        if 'hdmx' not in self.tables:
            return {}
        hdmx = self._table('hdmx')
        records, size = struct.unpack_from('>Hi', hdmx, 2)
        result = {}
        for i in range(records):
            base = 8 + i * size
            ppem, max_width = hdmx[base], hdmx[base + 1]
            result[ppem] = (max_width, hdmx[base + 2:base + 2 + self.glyph_count])
        return result

    def _vdmx(self):
        """ppem -> (yMax, yMin) from the first ratio group (1:1 aspect)."""
        if 'VDMX' not in self.tables:
            return {}
        vdmx = self._table('VDMX')
        _version, _records, ratios = struct.unpack_from('>HHH', vdmx, 0)
        offsets = struct.unpack_from(f'>{ratios}H', vdmx, 6 + 4 * ratios)
        for i in range(ratios):
            charset, x_ratio, y_start, y_end = vdmx[6 + 4 * i:10 + 4 * i]
            if (x_ratio == y_start == y_end == 0) or (x_ratio == 1 and y_start <= 1 <= y_end):
                group = offsets[i]
                count = struct.unpack_from('>H', vdmx, group)[0]
                return {struct.unpack_from('>H', vdmx, group + 4 + 6 * k)[0]:
                        struct.unpack_from('>hh', vdmx, group + 6 + 6 * k)
                        for k in range(count)}
        return {}

    def _cell(self, ppem):
        if ppem in self.vdmx:
            y_max, y_min = self.vdmx[ppem]
            return y_max, -y_min
        scale = ppem / self.units
        return math.ceil(self.win_ascent * scale), math.ceil(self.win_descent * scale)

    def ppem_for(self, lf_height):
        if lf_height < 0:
            return -lf_height
        if lf_height == 0:
            lf_height = 12
        fitting = [p for p in range(1, 256) if sum(self._cell(p)) <= lf_height]
        return max(fitting) if fitting else 1

    def metrics(self, lf_height):
        ppem = self.ppem_for(lf_height)
        ascent, descent = self._cell(ppem)
        scale = ppem / self.units
        gap = self.hhea_gap - ((self.win_ascent + self.win_descent)
                               - (self.hhea_ascender - self.hhea_descender))
        widths = {}
        device = self.hdmx.get(ppem)
        for byte in range(0x20, 0x100):
            try:
                code = ord(bytes([byte]).decode('cp1252'))
            except UnicodeDecodeError:
                continue
            glyph = self.cmap.get(code)
            if glyph is None:
                continue
            widths[byte] = device[1][glyph] if device else round(self.advances[glyph] * scale)
        return FontMetrics(ppem=ppem, ascent=ascent, descent=descent,
                           internal_leading=ascent + descent - ppem,
                           external_leading=max(0, round(gap * scale)),
                           average_width=round(self.average * scale),
                           max_width=device[0] if device else round((self.x_max - self.x_min) * scale),
                           widths=widths)
