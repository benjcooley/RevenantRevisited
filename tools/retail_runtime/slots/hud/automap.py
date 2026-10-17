#!/usr/bin/env python3
"""Retail's automap pane over the HUD world: the side panel's lower slot,
TAutoMap (`0x0065b4f0`, vtable `0x005a5658`).

Retail draws the whole pane through T3DScene's overlay quads (`0x00414d70`,
which submits through `0x00414550`; `overlayraster.py` draws them as the D3D
device does): the map body (the tile mosaic, scaled and wrapped), the
fog-of-war mesh and the creatures' markers, then the chrome.

This fixture covers the chrome (`0x0052bdb0`): the `Amap` frame, and the
`PlusSel` / `MinusSel` overlays at pane (8, 3) and (0x90, 3) while their
buttons are held (pane +0x9c / +0xa0). The map body needs a level in the
world (the player's discovered area at player +0x314, the level list
`0x0065a77c`, the module's automap tiles); it is not covered yet.

Per case, from the booted world's checkpoint:

1. The pane at the side panel's lower slot, (452, 306), 188 x 174, and its
   Initialize (`0x00529970`) with no player: AutoMap.dat, the overlay
   textures, zoom 100; the player's map is left out.
2. The case's held buttons (pane +0x9c "+", +0xa0 "-").
3. One frame: the chrome inside a T3DScene scene, over a black back buffer.
4. Recorded: the back buffer (RGB565 + PNG), the primitives (primitives.py),
   and `blend.screen`, the pixels a quad's blend mixed (overlayraster.py).

    automap.py EXE --case case.json --out DIR      one case
    automap.py EXE --serve                          JSONL: {"id","case","out"}

Case: {"name": "...", "plus": false, "minus": false}.
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
from hudscene import BEGIN_SCENE, END_SCENE, SCENE, HudScene  # noqa: E402
from hudworld import HudWorld  # noqa: E402
from itempanes import save_capture, serve  # noqa: E402
from overlayraster import OverlayRaster  # noqa: E402
from primitives import PrimitiveTap, s32  # noqa: E402

AUTOMAP = 0x0065b4f0
INITIALIZE = 0x00529970
CHROME = 0x0052bdb0
PLUS_HELD, MINUS_HELD = 0x9c, 0xa0               # TAutoMap
PANE_RECT = (452, 306, 188, 174)                 # the side panel's lower slot
SCREEN = (640, 480)
SCHEMA = 'hud.automap.v1'


class AutoMapFixture:
    def __init__(self, executable):
        started = time.perf_counter()
        self.world = world = HudWorld.open(executable)
        self.vm = world.vm
        self.scene = HudScene(world)
        self.tap = PrimitiveTap(self.vm, self._surface_name)
        self.raster = OverlayRaster(self.vm, world.ddraw, self.scene.back_buffer)
        world.checkpoint()
        self.setup_ms = (time.perf_counter() - started) * 1000

    def _surface_name(self, pointer):
        return self.scene.screen_surface(pointer) or (f'0x{pointer:08x}' if pointer else None)

    def run(self, case, out_dir=None):
        world, scene, vm = self.world, self.scene, self.vm
        world.restore()
        scene.set_player(None)
        for offset, value in zip((4, 8, 0xc, 0x10), PANE_RECT):
            vm.put_u32(AUTOMAP + offset, value)
        if not world.call(INITIALIZE, this=AUTOMAP, instruction_limit=2_000_000_000):
            raise RuntimeError('the automap did not initialize')
        vm.put_u32(AUTOMAP + PLUS_HELD, int(bool(case.get('plus'))))
        vm.put_u32(AUTOMAP + MINUS_HELD, int(bool(case.get('minus'))))
        scene.clear()
        self.raster.new_frame(*SCREEN)
        self.tap.start()
        world.call(BEGIN_SCENE, this=SCENE, instruction_limit=50_000_000)
        world.call(CHROME, this=AUTOMAP, instruction_limit=200_000_000)
        world.call(END_SCENE, this=SCENE, instruction_limit=50_000_000)
        primitives = self.tap.stop()
        return dict(schema=SCHEMA, case=case.get('name'),
                    pane=[s32(vm.u32(AUTOMAP + o)) for o in (4, 8, 0xc, 0x10)],
                    primitives=primitives, blend=dict(screen=self.raster.screen_mask()),
                    text_calls=[], capture=save_capture(scene, out_dir, case.get('name', 'case')))


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('executable', type=Path)
    parser.add_argument('--case', type=Path)
    parser.add_argument('--out', type=Path)
    parser.add_argument('--serve', action='store_true')
    args = parser.parse_args()
    fixture = AutoMapFixture(args.executable)
    if args.serve:
        serve(fixture.run, fixture.world.sha, SCHEMA, fixture.setup_ms)
        return
    case = json.loads(args.case.read_text()) if args.case else dict(name='sample', plus=True, minus=False)
    result = fixture.run(case, args.out)
    print(json.dumps({k: v for k, v in result.items() if k != 'primitives'}, indent=2))
    for entry in result['primitives']:
        print(json.dumps(entry))


if __name__ == '__main__':
    main()
