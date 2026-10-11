"""What the item-pane fixtures share (bottombar.py, inventory.py).

- `ItemPaneKit.place(container, spec, slot)`: a real item of the spec's type
  in the container's inventory slot, through retail's AddToInventory
  (`0x0046f940`); its amount through the item's own SetAmount (vtable
  +0x19c: ammo and money compose their icons for it, other items have no
  amount); a pouch's contents in its slots 0, 1, ...
  Spec: {"type": "...", "amount": N, "contents": [spec, ...]}.
- The frame's `alpha` mask: the screen pixels a DM_ALPHA Put mixed, and the
  anti-aliased edge pixels of a bitmap font's glyphs (DM_ALIAS: the glyph
  blit `0x004b5820` mixes each of the alias buffer's pixels at its
  coverage), coverage strictly between 0 and 31 (one byte per pixel, zlib,
  hex). Retail mixes those through its 5-bit scale tables (the blit at
  `0x004b3694`: d * (31 - a) // 31 + s * a // 31 per field, green in two
  halves), which truncate where a float blend doesn't: never more than two
  RGB565 steps apart, over every input.
- `game_frame(n)`: the game's frame count, which picks an animated icon's
  frame (TPlayScreen `0x0047e920` on `0x0065caf0`).
- `gdi_text_cells(calls)`: the screen rects retail laid GDI text in (its
  bitmap fonts the emulator draws; only GDI glyphs are missing).
- `save_capture(scene, out_dir, stem)` and `serve(run, sha, schema,
  setup_ms)`: a fixture's frame on disk, and its JSONL loop (`{"id", "case",
  "out"}` -> `{"id", "ok", "result" | "error"}`).

Retail bugs the kit fixes (`fix_retail_bugs = False` keeps them):
- A pouch's thumbnail of a stacked item (ammo, money) sits on a black square.
  The stack's icon is composed on 0, its key; the 2:1 reduction
  (`0x004a31a0`) keys 16-bit sources on the one value it is given -- the
  display's key, 0xf81f, which also fills the thumbnail's empty pixels -- so
  the background averages in as black. Fixed: the reduction reads a copy of a
  15/16-bit source whose own key pixels are the display's key, so they are
  skipped and fill as empty.
"""
from __future__ import annotations

import json
import struct
import sys
import time
import zlib
from pathlib import Path

from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP

from hudscene import png
from primitives import s32

ADD_TO_INVENTORY = 0x0046f940                    # TObjectInstance, thiscall (name, number, slot)
GET_INVENTORY = 0x004701f0                       # TObjectInstance, thiscall (slot) -> item
VT_SET_AMOUNT = 0x19c
REDUCE = 0x004a31a0                              # cdecl (source, destination, key)
GLYPH_BLIT = 0x004b5820                          # (source, drawparam, rise, glyph bitmap, index, font, ...)
PLAY_SCREEN = 0x0065caf0                         # TPlayScreen: game frames +0x680 + (+0x688 - +0x684)
GAME_FRAMES, SESSION_START, SESSION_FRAMES = 0x680, 0x684, 0x688
BM_15BIT, BM_16BIT, BM_ALIAS, BM_ALPHA = 0x2, 0x4, 0x80, 0x100
AL_EOL, AL_EOD = 0xfe, 0xff                      # the alias buffer's end of line / of data
DM_ALIAS, DM_ALPHA, DM_USEDEFAULT = 0x1000, 0x2000, 0x80000000
BITMAP_HEADER = 0x48                             # a TBitmap's pixels follow its header
OPAQUE_ALPHA = 31
SCREEN = (640, 480)


class ItemPaneKit:
    def __init__(self, world, scene, tap):
        self.world, self.scene, self.vm = world, scene, world.vm
        self.fix_retail_bugs = True
        self.origin = (0, 0)                     # the drawing pane's screen position: its clip origin
        self.alpha = None                        # the frame's DM_ALPHA mask, while recording
        self.vm.uc.hook_add(UC_HOOK_CODE, self._reduce_on_own_key, begin=REDUCE, end=REDUCE)
        self.vm.uc.hook_add(UC_HOOK_CODE, self._track_glyph_alias, begin=GLYPH_BLIT, end=GLYPH_BLIT)
        tap.listeners.append(self._track_alpha)

    # ---- items ---------------------------------------------------------

    def place(self, container, spec, slot):
        """A real item of the spec's type in `container`'s inventory `slot`."""
        world, vm = self.world, self.vm
        if not world.call(ADD_TO_INVENTORY, (self.scene.string(spec['type']), 1, slot), this=container,
                          instruction_limit=2_000_000_000):
            raise RuntimeError(f'AddToInventory refused {spec["type"]!r}')
        item = world.call(GET_INVENTORY, (slot,), this=container, instruction_limit=50_000_000)
        if not item:
            raise RuntimeError(f'{spec["type"]!r} did not land in slot 0x{slot:x}: retail stacked it with '
                               f'an item of its type (give each item in a container its own type)')
        if 'amount' in spec:
            world.call(vm.u32(vm.u32(item) + VT_SET_AMOUNT), (int(spec['amount']),), this=item,
                       instruction_limit=500_000_000)
        for index, content in enumerate(spec.get('contents', [])):
            self.place(item, content, index)
        return item

    def game_frame(self, frame):
        vm = self.vm
        vm.put_u32(PLAY_SCREEN + GAME_FRAMES, int(frame))
        vm.put_u32(PLAY_SCREEN + SESSION_START, 0)
        vm.put_u32(PLAY_SCREEN + SESSION_FRAMES, 0)

    # ---- the frame's masks ---------------------------------------------

    def record(self, origin):
        """Starts the frame's alpha mask; panes draw at `origin`."""
        self.origin = origin
        self.alpha = bytearray(SCREEN[0] * SCREEN[1])

    def stop(self):
        """The frame's alpha mask, packed (one byte per pixel, zlib, hex)."""
        alpha, self.alpha = self.alpha, None
        return zlib.compress(bytes(alpha), 9).hex()

    def _track_alpha(self, name, this, a):
        if self.alpha is None or name != 'Put' or self.scene.screen_surface(this) != 'display':
            return
        bitmap = a[2]
        fields = struct.unpack('<18I', self.vm.uc.mem_read(bitmap, 72))
        width, height, flags, drawmode = fields[0], fields[1], fields[4], fields[5]
        # DM_USEDEFAULT adds the bitmap's own mode to the flags passed with
        # it (an arrow button's DM_USEDEFAULT | DM_ALPHA reaches the alpha
        # blit, 0x004b3510, whatever its bitmap's mode).
        mode = a[3] | (drawmode if a[3] & DM_USEDEFAULT else 0)
        if not (mode & DM_ALPHA and flags & BM_ALPHA and fields[9] and fields[10]):
            return
        coverage = self.vm.uc.mem_read(bitmap + 40 + fields[10], width * height)
        left, top = self.origin[0] + s32(a[0]), self.origin[1] + s32(a[1])
        for y in range(height):
            sy = top + y
            if not 0 <= sy < SCREEN[1]:
                continue
            for x in range(width):
                sx = left + x
                if 0 <= sx < SCREEN[0] and 0 < (coverage[y * width + x] & 0x1f) < OPAQUE_ALPHA:
                    self.alpha[sy * SCREEN[0] + sx] = 1

    def _track_glyph_alias(self, uc, address, size, user):
        if self.alpha is None:
            return
        vm = self.vm
        sp = uc.reg_read(UC_X86_REG_ESP)
        drawparam, glyph = vm.u32(sp + 8), vm.u32(sp + 16)
        p = [s32(v) for v in struct.unpack('<18i', vm.uc.mem_read(drawparam, 72))]
        mode, origin, dst, src, size = p[0], p[4:6], p[10:12], p[14:16], p[16:18]
        fields = struct.unpack('<9I', vm.uc.mem_read(glyph, 36))
        width, height, flags, alias_size = fields[0], fields[1], fields[4], fields[7]
        if not (mode & DM_ALIAS and flags & BM_ALIAS and alias_size and fields[8]):
            return
        for x, y, coverage in alias_pixels(bytes(vm.uc.mem_read(glyph + 32 + fields[8], alias_size)),
                                           width, height):
            if not (0 < coverage < OPAQUE_ALPHA and src[0] <= x < src[0] + size[0]
                    and src[1] <= y < src[1] + size[1]):
                continue
            sx, sy = origin[0] + dst[0] + x - src[0], origin[1] + dst[1] + y - src[1]
            if 0 <= sx < SCREEN[0] and 0 <= sy < SCREEN[1]:
                self.alpha[sy * SCREEN[0] + sx] = 1

    def _reduce_on_own_key(self, uc, address, size, user):
        if not self.fix_retail_bugs:
            return
        sp = uc.reg_read(UC_X86_REG_ESP)
        source, key = self.vm.u32(sp + 4), self.vm.u32(sp + 12) & 0xffff
        if not source or not self.vm.u32(source + 16) & (BM_15BIT | BM_16BIT):
            return
        own = self.vm.u32(source + 24) & 0xffff
        if own == key:
            return
        width, height = self.vm.u32(source), self.vm.u32(source + 4)
        pixels = bytearray(self.vm.uc.mem_read(source + BITMAP_HEADER, 2 * width * height))
        for i in range(0, len(pixels), 2):
            if pixels[i] | pixels[i + 1] << 8 == own:
                pixels[i:i + 2] = key.to_bytes(2, 'little')
        copy = self.vm.allocate(BITMAP_HEADER + len(pixels))
        self.vm.write(copy, bytes(self.vm.uc.mem_read(source, BITMAP_HEADER)) + bytes(pixels))
        self.vm.put_u32(sp + 4, copy)


def alias_pixels(stream, width, height):
    """(x, y, coverage) of each pixel in a bitmap's alias buffer: per line,
    skip / run groups of (colour16, coverage5) pixels to AL_EOL, AL_EOD at
    the end."""
    i, y = 0, 0
    while y < height and i < len(stream):
        code = stream[i]
        i += 1
        if code == AL_EOD:
            return
        if code == AL_EOL:
            y += 1
            continue
        x = 0
        while i < len(stream):
            x += code
            run = stream[i]
            i += 1
            for _ in range(run):
                if 0 <= x < width:
                    yield x, y, stream[i + 2]
                i += 3
                x += 1
            code = stream[i]
            i += 1
            if code == AL_EOL:
                break
        y += 1


def gdi_text_cells(calls):
    """The screen rects of the GDI text calls (DrawTextA's layout rect)."""
    cells = []
    for call in calls:
        rect = call.get('rect')
        if rect and list(rect) not in cells:
            cells.append(list(rect))
    return cells


def save_capture(scene, out_dir, stem):
    """The back buffer -> {width, height, format[, png]}; with `out_dir`, as
    DIR/stem.rgb565 and DIR/stem.png."""
    width, height, rows = scene.capture()
    capture = dict(width=width, height=height, format='RGB565')
    if out_dir is not None:
        out_dir = Path(out_dir)
        out_dir.mkdir(parents=True, exist_ok=True)
        (out_dir / f'{stem}.rgb565').write_bytes(b''.join(rows))
        (out_dir / f'{stem}.png').write_bytes(png(width, height, rows))
        capture['png'] = str(out_dir / f'{stem}.png')
    return capture


def serve(run, sha, schema, setup_ms):
    """The JSONL loop: run(case, out) per request."""
    sys.stdout.write(json.dumps(dict(id='hello', ok=True, schema=schema, retail_sha256=sha,
                                     setup_ms=setup_ms)) + '\n')
    sys.stdout.flush()
    for raw in sys.stdin:
        if not raw.strip():
            continue
        request = json.loads(raw)
        try:
            started = time.perf_counter()
            result = run(request['case'], request.get('out'))
            result['elapsed_ms'] = (time.perf_counter() - started) * 1000
            response = dict(id=request.get('id'), ok=True, result=result)
        except Exception as error:
            response = dict(id=request.get('id'), ok=False, error=f'{type(error).__name__}: {error}')
        sys.stdout.write(json.dumps(response) + '\n')
        sys.stdout.flush()
