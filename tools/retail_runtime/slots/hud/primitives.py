"""Retail's drawing primitives, tapped with their arguments.

A pane fixture hooks the primitives its pane calls (the TSurface Puts and
blits, the fill, the GDI text compose, surface-from-bitmap and T3DScene's
overlay quad) through one `PrimitiveTap`:

- `listeners` see every call, recording or not: `(name, this, args)` with
  the raw stack arguments (the blend map follows composition this way);
- `start()` ... `stop()` returns the calls in between as JSON-ready entries,
  surfaces named by the fixture's `name_of(pointer)` and bitmaps by
  `bitmap_names` ({pointer: 'Archive:Name'}, HudScene.bitmap_names()).
"""
from __future__ import annotations

import struct

from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ECX, UC_X86_REG_ESP

# address -> (name, stack arguments)
PRIMITIVES = {
    0x004bd680: ('Put', 5),                 # TSurface::Put(x, y, bitmap, drawmode, color)
    0x004bd5e0: ('PutSubrect', 8),          # (x, y, bitmap, sx, sy, w, h, drawmode)
    0x004bd490: ('ParamBlit', 4),           # vtable +0x5c: TSurface
    0x004bbed0: ('ParamBlit', 4),           #               tiled surface
    0x004aa280: ('ParamBlit', 4),           #               display
    0x004bde60: ('Box', 8),                 # vtable +0x64: TSurface
    0x004a6930: ('Box', 8),                 #               DirectDraw surface
    0x00414d70: ('Quad', 14),               # T3DScene overlay quad
    0x004be2b0: ('Text', 10),               # GDI text compose
    0x004a5ca0: ('SurfaceFromBitmap', 2),
}
DRAWPARAM_WORDS = 21


def s32(value):
    return struct.unpack('<i', struct.pack('<I', value & 0xffffffff))[0]


class PrimitiveTap:
    def __init__(self, vm, name_of, bitmap_names=None):
        self.vm = vm
        self.name_of = name_of
        self.bitmap_names = bitmap_names or {}
        self.listeners = []
        self.recording = None
        for address in PRIMITIVES:
            vm.uc.hook_add(UC_HOOK_CODE, self._hook, begin=address, end=address)

    def start(self):
        self.recording = []

    def stop(self):
        recorded, self.recording = self.recording, None
        return recorded

    def drawparam(self, pointer):
        return [s32(v) for v in struct.unpack(f'<{DRAWPARAM_WORDS}I',
                                              self.vm.uc.mem_read(pointer, 4 * DRAWPARAM_WORDS))]

    def bitmap(self, pointer):
        if not pointer:
            return None
        width, height, _regx, _regy, flags, drawmode, key = struct.unpack('<7I', self.vm.uc.mem_read(pointer, 28))
        entry = dict(size=[width, height], flags=flags, drawmode=drawmode, key=key)
        if pointer in self.bitmap_names:
            entry['name'] = self.bitmap_names[pointer]
        return entry

    def _hook(self, uc, address, size, user):
        name, count = PRIMITIVES[address]
        sp = uc.reg_read(UC_X86_REG_ESP)
        a = struct.unpack(f'<{count}I', uc.mem_read(sp + 4, 4 * count))
        this = uc.reg_read(UC_X86_REG_ECX)
        for listener in self.listeners:
            listener(name, this, a)
        if self.recording is not None:
            self.recording.append(dict(primitive=name, **self._entry(name, this, a)))

    def _entry(self, name, this, a):
        dst = self.name_of(this)
        if name == 'Put':
            return dict(dst=dst, x=s32(a[0]), y=s32(a[1]), bitmap=self.bitmap(a[2]), mode=a[3])
        if name == 'PutSubrect':
            return dict(dst=dst, x=s32(a[0]), y=s32(a[1]), bitmap=self.bitmap(a[2]),
                        src=[s32(v) for v in a[3:7]], mode=a[7])
        if name == 'ParamBlit':
            return dict(dst=dst, src=self.name_of(a[1]), drawparam=self.drawparam(a[0]))
        if name == 'Box':
            return dict(dst=dst, rect=[s32(v) for v in a[:4]], color=a[4], mode=a[7])
        if name == 'Quad':
            return dict(x=s32(a[0]), y=s32(a[1]), texture=self.name_of(a[3]), size=[s32(a[5]), s32(a[6])],
                        tint=a[7], src=[s32(v) for v in a[8:12]], mode=a[13])
        if name == 'Text':
            return dict(dst=dst, rect=[s32(v) for v in a[:4]], text=self.vm.string(a[4]), font=a[6],
                        format=a[8])
        if name == 'SurfaceFromBitmap':
            return dict(bitmap=self.bitmap(a[0]), flags=a[1])
        return dict(args=list(a))
