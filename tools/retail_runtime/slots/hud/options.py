#!/usr/bin/env python3
"""Retail's Options pane over the HUD world: options.def panel "default",
640 x 480, through retail's DEF screen engine (TOptionsPane, class
0x005b9744, the shared instance `0x0066fcc0`).

Per case, from the booted world's checkpoint:

1. The setting globals the pane copies on opening (`settings`, by the pane's
   control names), and `fromGame` (pane +0x17c: the in-game menu's modal,
   DEF flags 0x11, else the title's Options screen).
2. Open (slot 0, `0x0053a8b0`): options.def, the members, control event 1
   (the toggles and sliders from the members, the controller list).
3. The case's input through retail's own handlers, in order (`input`:
   [["down"|"up"|"move", x, y] pane-local, or ["key", vk, down], ...]) --
   MouseClick (slot 25, `0x0053ba50`), MouseMove (slot 26, `0x00436660`),
   KeyPress (slot 27, `0x0053b9e0`).
4. `frames` frames as the screen draws the pane: its clip rect (vtable
   +0x24), the pane dirty, slot 20 (`0x00435de0`); from the game, each as
   HudScene.frame (the fade's tick, and slot 23's overlay quad of the
   pane's surface, `0x00436090`), six by default so it has faded in. The
   back buffer is cleared to black before the last.
5. Recorded: the back buffer, the last frame's primitives, the GDI text
   calls (measured with the real faces, no glyphs), the blend masks (2D
   `alpha`; from the game the quad's `screen` and the 4-bit compose's
   `composed`),
   the members the pane holds after the input (`members`), the rebind
   buffer (`bindings`, +0x1ac: each control's three codes of three keys)
   and the key capture (`rebind`: +0x1b0 capturing, +0x1c0 the item), and the
   blits retail's alpha blitter dropped (`known_bugs.narrow_alpha`).

    options.py EXE --case case.json --out DIR      one case
    options.py EXE --serve                          JSONL: {"id","case","out"}
"""
from __future__ import annotations

import argparse
import json
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(HERE.parents[1]))
from blendmap import BlendMap  # noqa: E402
from hudscene import HudScene  # noqa: E402
from hudworld import HudWorld  # noqa: E402
from itempanes import ItemPaneKit, save_capture, serve  # noqa: E402
from overlayraster import OverlayRaster  # noqa: E402
from primitives import PrimitiveTap, narrow_alpha_drops, s32  # noqa: E402

OPTIONS_PANE = 0x0066fcc0
FROM_GAME = 0x17c
SLOT_OPEN, SLOT_DRAW, SLOT_CLICK, SLOT_MOVE, SLOT_KEY = 0, 20, 25, 26, 27
REBIND_BUFFER, CAPTURING, REBIND_ITEM = 0x1ac, 0x1b0, 0x1c0
CONTROL_COUNT = 0x005d697c                    # retail's control table (0x005d5500) entries
KEYS_PER_CONTROL = 9
PANE_SURFACE, SURFACE_TEXTURE = 0x64, 0x54    # the overlay pane's surface and its texture
VT_SET_CLIP_RECT = 0x24
DIRTY = 0x50
MB_LEFTDOWN, MB_LEFTUP = 1, 4
PANE_RECT = (0, 0, 640, 480)
SCHEMA = 'hud.options.v1'

# The settings by control name: the global Open copies (docs/gameflow/
# forensics/OPTIONS.md §1), and the pane member it lands in. "Limit" is the
# negation of its global, NoGameSpeedLimit.
SETTINGS = {
    'Realtime': (0x005d7a18, 0x180), 'Auto': (0x005d7a68, 0x184), 'Dialog': (0x005d7a60, 0x188),
    'Face': (0x005d7a64, 0x18c), 'Enhanced': (0x005e91c0, 0x190), 'Limit': (0x006682a8, 0x194),
    'NoCombatRes': (0x00668194, 0x198), 'Violence': (0x005d79e8, 0x19c), 'Music': (0x005d7a9c, 0x1a0),
    'Sound': (0x005d7aa0, 0x1a4), 'Gamma': (0x005d7a48, 0x1a8),
}


class OptionsFixture:
    def __init__(self, executable):
        started = time.perf_counter()
        self.world = world = HudWorld.open(executable)
        self.vm = world.vm
        self.scene = HudScene(world)
        self.tap = PrimitiveTap(self.vm, self._surface_name, self.scene.bitmap_names())
        self.kit = ItemPaneKit(world, self.scene, self.tap)
        # From the game the pane composes into an ARGB4444 texture that its
        # overlay quad draws (slot 23): the quad as the D3D device draws it,
        # and where the compose blended in 4-bit steps.
        self.raster = OverlayRaster(self.vm, world.ddraw, self.scene.back_buffer)
        self.blend = None
        self.tap.listeners.append(self._track_blend)
        world.checkpoint()
        self.setup_ms = (time.perf_counter() - started) * 1000

    def _surface_name(self, pointer):
        """The display's surfaces by name; from the game, the pane's own
        surface and its texture (the overlay quad's) as 'pane_surface'."""
        if name := self.scene.screen_surface(pointer):
            return name
        surface = self.vm.u32(OPTIONS_PANE + PANE_SURFACE)
        if pointer and surface and pointer in (surface, self.vm.u32(surface + SURFACE_TEXTURE)):
            return 'pane_surface'
        return f'0x{pointer:08x}' if pointer else None

    def _track_blend(self, name, this, a):
        if self.blend is None:
            return
        if name == 'Put':
            self.blend.put(this, s32(a[0]), s32(a[1]), a[2], a[3])
        elif name == 'PutSubrect':
            self.blend.put_subrect(this, s32(a[0]), s32(a[1]), a[2], *(s32(v) for v in a[3:7]), a[7])
        elif name == 'Box':
            self.blend.box(this, *(s32(v) for v in a[:4]))
        elif name == 'Quad' and self.tap.recording is not None:
            # The quad names the surface's texture; the marks are the surface's.
            surface = self.vm.u32(OPTIONS_PANE + PANE_SURFACE)
            texture = surface if a[3] == self.vm.u32(surface + SURFACE_TEXTURE) else a[3]
            self.blend.quad(s32(a[0]), s32(a[1]), s32(a[5]), s32(a[6]), texture, s32(a[8]), s32(a[9]))

    def _slot(self, slot):
        return self.vm.u32(self.vm.u32(OPTIONS_PANE) + 4 * slot)

    def run(self, case, out_dir=None):
        world, scene, vm, kit = self.world, self.scene, self.vm, self.kit
        world.restore()
        for name, value in case.get('settings', {}).items():
            address, _member = SETTINGS[name]
            vm.put_u32(address, (0 if value else 1) if name == 'Limit' else int(value) & 0xffffffff)
        vm.put_u32(OPTIONS_PANE + FROM_GAME, 1 if case.get('from_game') else 0)
        if not world.call(self._slot(SLOT_OPEN), this=OPTIONS_PANE, instruction_limit=3_000_000_000):
            raise RuntimeError('the options pane did not open')
        for kind, x, y in case.get('input', []):
            if kind == 'key':
                world.call(self._slot(SLOT_KEY), (x, y), this=OPTIONS_PANE, instruction_limit=1_000_000_000)
            elif kind == 'move':
                world.call(self._slot(SLOT_MOVE), (MB_LEFTDOWN if case.get('held') else 0, x, y),
                           this=OPTIONS_PANE, instruction_limit=1_000_000_000)
            else:
                world.call(self._slot(SLOT_CLICK), (MB_LEFTDOWN if kind == 'down' else MB_LEFTUP, x, y),
                           this=OPTIONS_PANE, instruction_limit=1_000_000_000)
        from_game = bool(case.get('from_game'))
        frames = int(case.get('frames', 6 if from_game else 1))
        text_start = 0
        self.blend = BlendMap(vm) if from_game else None
        for frame in range(frames):
            if frame == frames - 1:
                scene.clear()
                self.tap.start()
                kit.record(PANE_RECT[:2])
                self.raster.new_frame(*PANE_RECT[2:])
                if self.blend:
                    self.blend.new_frame()
                text_start = len(world.gdi.text_calls)
            vm.put_u32(OPTIONS_PANE + DIRTY, 1)
            world.call(vm.u32(vm.u32(OPTIONS_PANE) + VT_SET_CLIP_RECT), (0,), this=OPTIONS_PANE)
            if from_game:
                scene.frame(OPTIONS_PANE)
            else:
                world.call(self._slot(SLOT_DRAW), this=OPTIONS_PANE, instruction_limit=3_000_000_000)
        primitives = self.tap.stop()
        blend = dict(alpha=self.kit.stop(), screen=self.raster.screen_mask())
        if self.blend:
            blend['composed'] = self.blend.screen_mask()
            self.blend = None
        members = {name: s32(vm.u32(OPTIONS_PANE + member)) for name, (_address, member) in SETTINGS.items()}
        buffer, controls = vm.u32(OPTIONS_PANE + REBIND_BUFFER), vm.u32(CONTROL_COUNT)
        bindings = [s32(vm.u32(buffer + 4 * i)) for i in range(controls * KEYS_PER_CONTROL)] if buffer else []
        return dict(schema=SCHEMA, case=case.get('name'), frames=frames,
                    pane=[s32(vm.u32(OPTIONS_PANE + o)) for o in (4, 8, 0xc, 0x10)], members=members,
                    bindings=bindings,
                    rebind=dict(capturing=s32(vm.u32(OPTIONS_PANE + CAPTURING)),
                                item=s32(vm.u32(OPTIONS_PANE + REBIND_ITEM))),
                    primitives=primitives, blend=blend,
                    known_bugs=dict(narrow_alpha=narrow_alpha_drops(primitives)),
                    text_calls=[call.__dict__ for call in world.gdi.text_calls[text_start:]],
                    capture=save_capture(scene, out_dir, case.get('name', 'case')))


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('executable', type=Path)
    parser.add_argument('--case', type=Path)
    parser.add_argument('--out', type=Path)
    parser.add_argument('--serve', action='store_true')
    args = parser.parse_args()
    fixture = OptionsFixture(args.executable)
    if args.serve:
        serve(fixture.run, fixture.world.sha, SCHEMA, fixture.setup_ms)
        return
    case = json.loads(args.case.read_text()) if args.case else dict(name='sample')
    result = fixture.run(case, args.out)
    print(json.dumps({k: v for k, v in result.items() if k not in ('primitives', 'text_calls')}, indent=1)[:3000])
    for entry in result['primitives']:
        print(json.dumps(entry))


if __name__ == '__main__':
    main()
