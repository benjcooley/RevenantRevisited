"""gdi32 for the HUD slot: fonts, device contexts on DirectDraw surfaces, and
the text calls retail's HUD makes.

Retail draws every WINFONT string (FONT.DEF: Arial / Times New Roman) with
GDI into a DirectDraw surface's DC (`IDirectDrawSurface4::GetDC`), then runs
its own shadow / color pass over the result (`0x004be2b0`). This module:

- Keeps GDI objects: fonts from CreateFontA (the full LOGFONT recorded),
  stock objects, DCs (a surface's, or the screen's from user32 GetDC).
- Answers GetTextMetricsA / GetTextExtentPoint32A from the TrueType file of
  the requested face (`ttf.py`: hdmx advance widths, VDMX/OS2 vertical
  metrics -- the tables GDI itself uses). Exact Win98 values come later from
  a capture of real GDI (docs/ui/HUD_REBUILD.md P6); until then these are
  the font file's own device metrics.
- Records TextOutA / DrawTextA (`text_calls`: DC surface, string, position
  or rect, format, font, color, background mode) and draws NO glyphs yet:
  the HUD A/B compares text calls structurally until the glyph shim lands.
"""
from __future__ import annotations

import struct
from dataclasses import dataclass, field
from pathlib import Path

import ttf

FACES = {('arial', False): '/System/Library/Fonts/Supplemental/Arial.ttf',
         ('arial', True): '/System/Library/Fonts/Supplemental/Arial Bold.ttf',
         ('times new roman', False): '/System/Library/Fonts/Supplemental/Times New Roman.ttf',
         ('times new roman', True): '/System/Library/Fonts/Supplemental/Times New Roman Bold.ttf'}
TRANSPARENT, OPAQUE = 1, 2
STOCK_BASE = 0x80000000
SYSTEM_FONT = STOCK_BASE | 13         # what a new DC has selected


@dataclass
class Font:
    height: int
    width: int
    weight: int
    italic: int
    face: str
    metrics: ttf.FontMetrics = None

    @property
    def bold(self):
        return self.weight >= 600


@dataclass
class DeviceContext:
    surface: int = 0                  # fake IDirectDrawSurface4 pointer (0: screen)
    font: int = SYSTEM_FONT
    text_color: int = 0
    back_color: int = 0xffffff
    back_mode: int = OPAQUE
    align: int = 0


@dataclass
class TextCall:
    function: str
    surface: int
    text: str
    x: int
    y: int
    rect: tuple | None
    format: int
    font: str
    color: int
    back_mode: int


class Gdi:
    def __init__(self, vm):
        self.vm = vm
        self.fonts: dict[int, Font] = {}
        self.dcs: dict[int, DeviceContext] = {}
        self.text_calls: list[TextCall] = []
        self.next_handle = 0x3000
        self._metrics_cache = {}
        gdi32 = {'gdi32.dll'}
        user32 = {'user32.dll'}
        for name, argc, fn, dlls in [
                ('CreateFontA', 14, self._create_font, gdi32),
                ('SelectObject', 2, self._select, gdi32),
                ('DeleteObject', 1, self._delete, gdi32),
                ('GetStockObject', 1, lambda a: STOCK_BASE | a[0], gdi32),
                ('SetTextColor', 2, lambda a: self._swap(a, 'text_color'), gdi32),
                ('SetBkColor', 2, lambda a: self._swap(a, 'back_color'), gdi32),
                ('SetBkMode', 2, lambda a: self._swap(a, 'back_mode'), gdi32),
                ('SetTextAlign', 2, lambda a: self._swap(a, 'align'), gdi32),
                ('GetDeviceCaps', 2, self._device_caps, gdi32),
                ('GetTextMetricsA', 2, self._text_metrics, gdi32),
                ('GetTextExtentPoint32A', 4, self._text_extent, gdi32),
                ('TextOutA', 5, self._text_out, gdi32),
                ('DrawTextA', 5, self._draw_text, user32),
                ('GetDC', 1, lambda a: self.create_dc(0), user32),
                ('GetWindowDC', 1, lambda a: self.create_dc(0), user32),
                ('CreateCompatibleDC', 1, lambda a: self.create_dc(0), gdi32),
                ('DeleteDC', 1, lambda a: self.release_dc(a[0]), gdi32),
                ('ReleaseDC', 2, lambda a: self.release_dc(a[1]), user32)]:
            vm.handlers[name] = (argc, fn)
            vm.api_dlls[name] = dlls

    # ---- objects -------------------------------------------------------

    def _handle(self):
        self.next_handle += 4
        return self.next_handle

    def create_dc(self, surface):
        handle = self._handle()
        self.dcs[handle] = DeviceContext(surface=surface)
        return handle

    def release_dc(self, handle):
        return int(self.dcs.pop(handle, None) is not None)

    def _dc(self, handle):
        if handle not in self.dcs:
            raise ValueError(f'Not a device context: 0x{handle:x}')
        return self.dcs[handle]

    def _create_font(self, args):
        height, width, _escapement, _orientation, weight, italic = args[:6]
        face = self.vm.string(args[13]) if args[13] else ''
        font = Font(struct.unpack('<i', struct.pack('<I', height))[0],
                    struct.unpack('<i', struct.pack('<I', width))[0], weight, italic, face)
        key = (face.lower(), font.bold)
        if key not in FACES or italic or font.width:
            raise ValueError(f'CreateFontA {face} {height} weight {weight} italic {italic} '
                             f'width {width}: no font file mapped')
        if key not in self._metrics_cache:
            self._metrics_cache[key] = ttf.TrueTypeFont(Path(FACES[key]))
        font.metrics = self._metrics_cache[key].metrics(font.height)
        handle = self._handle()
        self.fonts[handle] = font
        return handle

    def _select(self, args):
        dc, obj = self._dc(args[0]), args[1]
        if obj in self.fonts or (obj & STOCK_BASE and obj & 0xff in (10, 11, 12, 13, 14, 16, 17)):
            previous, dc.font = dc.font, obj
            return previous
        if obj & STOCK_BASE:
            return STOCK_BASE                         # pens/brushes: not used for text
        raise ValueError(f'SelectObject 0x{obj:x}')

    def _delete(self, args):
        self.fonts.pop(args[0], None)
        return 1

    def _swap(self, args, attribute):
        dc = self._dc(args[0])
        previous = getattr(dc, attribute)
        setattr(dc, attribute, args[1])
        return previous

    def _device_caps(self, args):
        index = args[1]
        caps = {88: 96, 90: 96,                       # LOGPIXELSX / LOGPIXELSY
                12: 16, 14: 1,                        # BITSPIXEL / PLANES
                8: 640, 10: 480}                      # HORZRES / VERTRES
        if index not in caps:
            raise ValueError(f'GetDeviceCaps index {index}')
        return caps[index]

    def _font(self, dc):
        font = self.fonts.get(dc.font)
        if font is None:
            raise ValueError('Text call with no font selected (stock fonts not modelled)')
        return font

    # ---- metrics -------------------------------------------------------

    def _text_metrics(self, args):
        m = self._font(self._dc(args[0])).metrics
        font = self._font(self._dc(args[0]))
        # TEXTMETRICA (56 bytes)
        block = struct.pack('<11i', m.height, m.ascent, m.descent, m.internal_leading,
                            m.external_leading, m.average_width, m.max_width, font.weight,
                            0, 96, 96)
        block += bytes([0x20, 0xff, 0x1f, 0x20, font.italic, 0, 0, 0x26, 0])  # chars, flags,
        block += bytes(56 - len(block))                                       # pitch/family
        self.vm.write(args[1], block[:56])
        return 1

    def _text_extent(self, args):
        dc, text, count, size = args
        m = self._font(self._dc(dc)).metrics
        data = bytes(self.vm.uc.mem_read(text, count))
        self.vm.write(size, struct.pack('<ii', sum(m.advance(b) for b in data), m.height))
        return 1

    # ---- text ----------------------------------------------------------

    def _record(self, function, dc_handle, text, x, y, rect, fmt):
        dc = self._dc(dc_handle)
        font = self._font(dc)
        self.text_calls.append(TextCall(function, dc.surface, text, x, y, rect, fmt,
                                        f'{font.face} {font.height}{" bold" if font.bold else ""}',
                                        dc.text_color, dc.back_mode))

    def _text_out(self, args):
        dc, x, y, text, count = args
        string = bytes(self.vm.uc.mem_read(text, count)).decode('cp1252')
        self._record('TextOutA', dc, string, struct.unpack('<i', struct.pack('<I', x))[0],
                     struct.unpack('<i', struct.pack('<I', y))[0], None, 0)
        return 1

    def _draw_text(self, args):
        dc, text, count, rect, fmt = args
        if count == 0xffffffff:
            string = self.vm.string(text)
        else:
            string = bytes(self.vm.uc.mem_read(text, count)).decode('cp1252')
        box = struct.unpack('<4i', bytes(self.vm.uc.mem_read(rect, 16)))
        self._record('DrawTextA', dc, string, box[0], box[1], box, fmt)
        if fmt & 0x400:                               # DT_CALCRECT: report the bounding box
            m = self._font(self._dc(dc)).metrics
            width = sum(m.advance(b) for b in string.encode('cp1252'))
            self.vm.write(rect, struct.pack('<4i', box[0], box[1], box[0] + width, box[1] + m.height))
        return self._font(self._dc(dc)).metrics.height

    def snapshot(self):
        import copy
        return copy.deepcopy((self.fonts, self.dcs, self.next_handle, len(self.text_calls)))

    def restore(self, state):
        import copy
        self.fonts, self.dcs, self.next_handle, calls = copy.deepcopy(state)
        del self.text_calls[calls:]
