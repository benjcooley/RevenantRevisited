#!/usr/bin/env python3
"""Retail TPlyrStatusBar (cls_0x5a54e4, global 0x0065a8c0) over the HUD world.

One case = a player (and optionally a target) with explicit stats, a number
of frames, and what to record. Per case, from the booted world's checkpoint:

1. StatusBar.dat / Portraits.dat through retail's loader into the globals
   TPlayScreen::Initialize fills (`0x0065a9d0` / `0x0065a9d4`), and the pane
   width TPlayScreen::Pulse sets: the case's `pane_width`, else display - 188
   = 452 (the side pane shown).
2. TPlyrStatusBar::Initialize `0x00549740`, then `frames` frames
   (slot 19 state tick -> fades ramp +1 per frame to 6, slot 20 compose,
   slot 7 texture pass in a T3DScene scene, slot 23 text pass), the back
   buffer cleared to black before the last one. The texture pass's quads
   are drawn as the D3D device draws them (`overlayraster.py`).
3. Recorded: the back buffer (RGB565 + PNG), every primitive the pane calls
   during the last frame (Put / ParamBlit / Box / surface-from-bitmap /
   Quad `0x414d70` / text compose `0x4be2b0`, with arguments), the GDI
   text calls, and two masks of the screen (one byte per pixel, zlib, hex):
   `blend.composed`, the pixels drawn from texels retail blended in 4-bit
   steps (`blendmap.py`), and `blend.screen`, the pixels a quad's blend
   mixed (`overlayraster.py`).

The target, when given, is the player's current combat action target
(`player+0xe0` -> action type 3, `action[0x11]`).

    plyrstatusbar.py EXE --case case.json --out DIR      one case
    plyrstatusbar.py EXE --serve                          JSONL: {"id","case"}

Case: {"name": "...", "player": {"name": "Locke", "health": 1833,
"max_health": 1930, "fatigue": 191, "max_fatigue": 380, "mana": 2174,
"max_mana": 2650, "level": 26, "class": "player" | "character", "portrait":
{"tbitmap": TBitmap bytes, hex} | {"width": 40, "height": 40, "pixels":
RGB565 hex} | absent}, "target": {...} | null, "frames": 8, "pane_width": 452}

A character's class defaults to player for the player and character for the
target; it picks the vtable copied and the class id (a player's name cell
reads "<name>\nLevel <n>").
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(HERE.parents[1]))
from unicorn import UC_HOOK_CODE  # noqa: E402
from unicorn.x86_const import UC_X86_REG_ECX, UC_X86_REG_ESP  # noqa: E402
from blendmap import BlendMap  # noqa: E402
from overlayraster import OverlayRaster  # noqa: E402
from hudscene import DISPLAY_BACK, DISPLAY_POINTER, HudScene, Stats, png  # noqa: E402
from hudworld import HudWorld  # noqa: E402

PANE = 0x0065a8c0
INITIALIZE = 0x00549740
G_STATUSBAR_DAT, G_PORTRAITS_DAT = 0x0065a9d0, 0x0065a9d4
PANE_TEMPLATE_WIDTH = 0x1c
DEFAULT_PANE_WIDTH = 640 - 188                   # the display less the shown side pane
SCREEN = (640, 480)
SCHEMA = 'hud.plyrstatusbar.v1'
SURFACE_FIELDS = {0x60: 'portrait', 0x64: 'text', 0x68: 'bars', 0x6c: 'chrome_compose',
                  0x70: 'chrome', 0x74: 'target_chrome'}
PRIMITIVES = {0x004bd680: ('Put', 5), 0x004bd5e0: ('PutSubrect', 7), 0x004bd490: ('ParamBlit', 4),
              0x004bbed0: ('ParamBlit', 4), 0x004aa280: ('ParamBlit', 4), 0x004bde60: ('Box', 8),
              0x004a6930: ('Box', 8), 0x00414d70: ('Quad', 14), 0x004be2b0: ('Text', 10),
              0x004a5ca0: ('SurfaceFromBitmap', 2)}


class StatusBarFixture:
    def __init__(self, executable):
        started = time.perf_counter()
        self.world = world = HudWorld.open(executable)
        self.vm = world.vm
        self.scene = HudScene(world)
        self.scene.load_archive('StatusBar.dat', G_STATUSBAR_DAT)
        self.scene.load_archive('Portraits.dat', G_PORTRAITS_DAT)
        self.recording = None
        self.blend = None
        for address in PRIMITIVES:
            self.vm.uc.hook_add(UC_HOOK_CODE, self._primitive, begin=address, end=address)
        self.raster = OverlayRaster(self.vm, world.ddraw, self.scene.back_buffer)
        world.checkpoint()
        self.setup_ms = (time.perf_counter() - started) * 1000

    # ---- primitive recording -------------------------------------------

    def _surface_name(self, pointer):
        for offset, name in SURFACE_FIELDS.items():
            if pointer and self.vm.u32(PANE + offset) == pointer:
                return name
        display = self.vm.u32(DISPLAY_POINTER)
        if pointer == display:
            return 'display'
        if pointer == self.vm.u32(display + DISPLAY_BACK):
            return 'back_buffer'
        return f'0x{pointer:08x}' if pointer else None

    def _is_screen(self, pointer):
        """The display or its back buffer: the 2D pass's blits land there."""
        display = self.vm.u32(DISPLAY_POINTER)
        return pointer in (display, self.vm.u32(display + DISPLAY_BACK))

    def _bitmap(self, pointer):
        if not pointer:
            return None
        width, height, regx, regy, flags, drawmode, key = struct.unpack(
            '<7I', self.vm.uc.mem_read(pointer, 28))
        return dict(size=[width, height], flags=flags, drawmode=drawmode, key=key)

    def _primitive(self, uc, address, size, user):
        if self.blend is None:
            return
        name, count = PRIMITIVES[address]
        sp = uc.reg_read(UC_X86_REG_ESP)
        a = struct.unpack(f'<{count}I', uc.mem_read(sp + 4, 4 * count))
        s32 = lambda v: struct.unpack('<i', struct.pack('<I', v))[0]  # noqa: E731
        self._track_blend(name, uc.reg_read(UC_X86_REG_ECX), a, s32)
        if self.recording is None:
            return
        this = self._surface_name(uc.reg_read(UC_X86_REG_ECX))
        if name == 'Put':
            entry = dict(dst=this, x=s32(a[0]), y=s32(a[1]), bitmap=self._bitmap(a[2]), mode=a[3])
        elif name == 'ParamBlit':
            dp = struct.unpack('<21I', uc.mem_read(a[0], 84))
            entry = dict(dst=this, src=self._surface_name(a[1]), drawparam=list(dp))
        elif name == 'Box':
            entry = dict(dst=this, rect=[s32(v) for v in a[:4]], color=a[4], mode=a[7])
        elif name == 'Quad':
            entry = dict(x=s32(a[0]), y=s32(a[1]), texture=self._surface_name(a[3]),
                         size=[s32(a[5]), s32(a[6])], tint=a[7],
                         src=[s32(v) for v in a[8:12]], mode=a[13])
        elif name == 'Text':
            entry = dict(dst=this, rect=[s32(v) for v in a[:4]], text=self.vm.string(a[4]),
                         font=a[6], format=a[8])
        elif name == 'SurfaceFromBitmap':
            entry = dict(bitmap=self._bitmap(a[0]), flags=a[1])
        else:
            entry = dict(args=list(a))
        self.recording.append(dict(primitive=name, **entry))

    def _track_blend(self, name, this, a, s32):
        """Every frame's primitives feed the blend map (the chips compose on
        the first frame); the screen's marks are the last frame's quads."""
        if name == 'Put':
            self.blend.put(this, s32(a[0]), s32(a[1]), a[2], a[3])
        elif name == 'Box':
            self.blend.box(this, *(s32(v) for v in a[:4]))
        elif name == 'ParamBlit':
            dp = [s32(v) for v in struct.unpack('<21I', self.vm.uc.mem_read(a[0], 84))]
            if a[1] and not self._is_screen(this):
                self.blend.blit(this, a[1], dp[10], dp[11], dp[14], dp[15], dp[16], dp[17])
        elif name == 'Quad' and self.recording is not None:
            self.blend.quad(s32(a[0]), s32(a[1]), s32(a[5]), s32(a[6]), a[3], s32(a[8]), s32(a[9]))

    # ---- cases ---------------------------------------------------------

    def _character(self, spec, default_name, default_class):
        portrait = 0
        if spec.get('portrait'):
            p = spec['portrait']
            data = (bytes.fromhex(p['tbitmap']) if 'tbitmap' in p
                    else self.scene.rgb565_bitmap(p['width'], p['height'], bytes.fromhex(p['pixels'])))
            portrait = self.scene.bitmap(data)
        stats = Stats(**{k: v for k, v in spec.items() if k not in ('name', 'class', 'portrait')})
        return self.scene.character(spec.get('name', default_name),
                                    player=spec.get('class', default_class) == 'player',
                                    stats=stats, portrait=portrait)

    def run(self, case, out_dir=None):
        world, scene, vm = self.world, self.scene, self.vm
        world.restore()
        vm.put_u32(PANE + PANE_TEMPLATE_WIDTH, int(case.get('pane_width', DEFAULT_PANE_WIDTH)))
        player = self._character(case['player'], 'Locke', 'player')
        scene.set_player(player)
        target = self._character(case['target'], 'Target', 'character') if case.get('target') else None
        scene.set_target(player, target)
        world.call(INITIALIZE, this=PANE, instruction_limit=200_000_000)
        frames = int(case.get('frames', 8))
        text_start = 0
        self.blend = BlendMap(vm)
        for frame in range(frames):
            last = frame == frames - 1
            if last:
                scene.clear()
                self.recording = []
                self.blend.new_frame()
                self.raster.new_frame(*SCREEN)
                text_start = len(world.gdi.text_calls)
            scene.frame(PANE)
        primitives, self.recording = self.recording, None
        blend = dict(composed=self.blend.screen_mask(), screen=self.raster.screen_mask())
        self.blend = None
        width, height, rows = scene.capture()
        result = dict(schema=SCHEMA, case=case.get('name'), frames=frames,
                      fades=dict(player=vm.u32(PANE + 0xd4), target=vm.u32(PANE + 0xdc)),
                      primitives=primitives, blend=blend,
                      text_calls=[call.__dict__ for call in world.gdi.text_calls[text_start:]],
                      capture=dict(width=width, height=height, format='RGB565'))
        if out_dir is not None:
            out_dir = Path(out_dir)
            out_dir.mkdir(parents=True, exist_ok=True)
            stem = case.get('name', 'case')
            (out_dir / f'{stem}.rgb565').write_bytes(b''.join(rows))
            (out_dir / f'{stem}.png').write_bytes(png(width, height, rows))
            result['capture']['png'] = str(out_dir / f'{stem}.png')
        return result


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('executable', type=Path)
    parser.add_argument('--case', type=Path)
    parser.add_argument('--out', type=Path)
    parser.add_argument('--serve', action='store_true')
    args = parser.parse_args()
    fixture = StatusBarFixture(args.executable)
    if args.serve:
        sys.stdout.write(json.dumps(dict(id='hello', ok=True, schema=SCHEMA, retail_sha256=fixture.world.sha,
                                         setup_ms=fixture.setup_ms)) + '\n')
        sys.stdout.flush()
        for raw in sys.stdin:
            if not raw.strip():
                continue
            request = json.loads(raw)
            try:
                started = time.perf_counter()
                result = fixture.run(request['case'], request.get('out'))
                result['elapsed_ms'] = (time.perf_counter() - started) * 1000
                response = dict(id=request.get('id'), ok=True, result=result)
            except Exception as error:
                response = dict(id=request.get('id'), ok=False, error=f'{type(error).__name__}: {error}')
            sys.stdout.write(json.dumps(response) + '\n')
            sys.stdout.flush()
        return
    case = json.loads(args.case.read_text()) if args.case else dict(
        name='locke-l26', player=dict(name='Locke', health=1833, max_health=1930, fatigue=191,
                                      max_fatigue=380, mana=2174, max_mana=2650, level=26), frames=8)
    result = fixture.run(case, args.out)
    print(json.dumps({k: v for k, v in result.items() if k != 'primitives'}, indent=2))
    print(f'{len(result["primitives"])} primitives recorded in the last frame')


if __name__ == '__main__':
    main()
