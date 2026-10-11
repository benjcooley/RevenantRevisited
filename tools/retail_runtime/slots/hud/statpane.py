#!/usr/bin/env python3
"""Retail's stat pane over the HUD world: the side panel's upper slot, the
main player's stats and skills (TStatPane, class 0x005a5ba0, global
`0x0065b140`), a TButtonPane.

Per case, from the booted world's checkpoint:

1. StatsPane.dat and SpellIcons.dat through retail's loader into the
   globals TPlayScreen::Initialize fills (`0x0065c6f4` / `0x0065bc3c`), and a fixture player (hudscene.py)
   with the case's stats and skills.
2. The pane at the side panel's upper slot, (452, 0), 188 x 306, and its
   Initialize (`0x00546b50`); then the case's pane fields (`fields`:
   {"0x17c": value, ...}) -- the page and the scroll, as the forensics name
   them.
3. `frames` frames, each as the screen draws the pane: its clip rect on the
   display (vtable +0x24), the pane dirty, its slot 20 (`0x00549010`: the
   paint, slot 21 `0x005491c0`, then the buttons). The back buffer is
   cleared to black before the last frame.
4. Recorded: the back buffer (RGB565 + PNG), the last frame's primitives
   (primitives.py; bitmaps named Archive:Name), the GDI text calls, and the
   `blend.alpha` mask (itempanes.py).

    statpane.py EXE --case case.json --out DIR      one case
    statpane.py EXE --serve                          JSONL: {"id","case","out"}

Case: {"name": "...", "player": {"name": "Locke", "stats": {hudscene.Stats},
"objstats": {"Strn": 16, ...}, "class": "...", "levelup": [a, b, last],
"combat": {"0x2bc": 7, ...}}, "format": "page2", "fields": {}, "frames": 1}.
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
from hudscene import HudScene, StatSheet, Stats  # noqa: E402
from hudworld import HudWorld  # noqa: E402
from itempanes import ItemPaneKit, save_capture, serve  # noqa: E402
from primitives import PrimitiveTap, s32  # noqa: E402

STAT_PANE = 0x0065b140
G_STATSPANE_DAT, G_SPELLICONS_DAT = 0x0065c6f4, 0x0065bc3c
INITIALIZE = 0x00546b50
DRAW = 0x00549010                                # slot 20: the paint when dirty, then the buttons
DIRTY = 0x50                                     # TPane
VT_SET_CLIP_RECT = 0x24
VT_SET_FORMAT = 0xbc                             # slot 47, 0x00488a30
PANE_RECT = (452, 0, 188, 306)                   # the side panel's upper slot
SCHEMA = 'hud.statpane.v1'


class StatPaneFixture:
    def __init__(self, executable):
        started = time.perf_counter()
        self.world = world = HudWorld.open(executable)
        self.vm = world.vm
        self.scene = HudScene(world)
        self.scene.load_archive('StatsPane.dat', G_STATSPANE_DAT)
        self.scene.load_archive('SpellIcons.dat', G_SPELLICONS_DAT)
        self.tap = PrimitiveTap(self.vm, self._surface_name, self.scene.bitmap_names())
        self.kit = ItemPaneKit(world, self.scene, self.tap)
        world.checkpoint()
        self.setup_ms = (time.perf_counter() - started) * 1000

    def _surface_name(self, pointer):
        if pointer == STAT_PANE:
            return 'statpane'
        return self.scene.screen_surface(pointer) or (f'0x{pointer:08x}' if pointer else None)

    def run(self, case, out_dir=None):
        world, scene, vm, kit = self.world, self.scene, self.vm, self.kit
        world.restore()
        spec = case.get('player', {})
        player = scene.character(spec.get('name', 'Locke'), player=True, stats=Stats(**spec.get('stats', {})))
        scene.give_stat_sheet(player, StatSheet(
            objstats=spec.get('objstats', {}), class_name=spec.get('class', ''),
            levelup=tuple(spec.get('levelup', (-1, -1, 0))),
            combat={int(k, 0): v for k, v in spec.get('combat', {}).items()}))
        scene.set_player(player)            # Initialize binds the main player (SetObject 0x005496a0)
        for offset, value in zip((4, 8, 0xc, 0x10), PANE_RECT):
            vm.put_u32(STAT_PANE + offset, value)
        if not world.call(INITIALIZE, this=STAT_PANE, instruction_limit=2_000_000_000):
            raise RuntimeError('the stat pane did not initialize')
        if case.get('format', 'page1') != 'page1':
            world.call(vm.u32(vm.u32(STAT_PANE) + VT_SET_FORMAT), (scene.string(case['format']),), this=STAT_PANE)
        for offset, value in case.get('fields', {}).items():
            vm.put_u32(STAT_PANE + int(offset, 0), int(value) & 0xffffffff)
        frames = int(case.get('frames', 1))
        text_start = 0
        for frame in range(frames):
            if frame == frames - 1:
                scene.clear()
                self.tap.start()
                kit.record(PANE_RECT[:2])
                text_start = len(world.gdi.text_calls)
            vm.put_u32(STAT_PANE + DIRTY, 1)
            world.call(vm.u32(vm.u32(STAT_PANE) + VT_SET_CLIP_RECT), (0,), this=STAT_PANE)
            world.call(DRAW, this=STAT_PANE, instruction_limit=2_000_000_000)
        primitives = self.tap.stop()
        return dict(schema=SCHEMA, case=case.get('name'), frames=frames,
                    pane=[s32(vm.u32(STAT_PANE + o)) for o in (4, 8, 0xc, 0x10)],
                    primitives=primitives, blend=dict(alpha=kit.stop()),
                    text_calls=[call.__dict__ for call in world.gdi.text_calls[text_start:]],
                    capture=save_capture(scene, out_dir, case.get('name', 'case')))


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('executable', type=Path)
    parser.add_argument('--case', type=Path)
    parser.add_argument('--out', type=Path)
    parser.add_argument('--serve', action='store_true')
    args = parser.parse_args()
    fixture = StatPaneFixture(args.executable)
    if args.serve:
        serve(fixture.run, fixture.world.sha, SCHEMA, fixture.setup_ms)
        return
    case = json.loads(args.case.read_text()) if args.case else dict(name='sample')
    result = fixture.run(case, args.out)
    print(json.dumps({k: v for k, v in result.items() if k not in ('primitives', 'text_calls')}, indent=1)[:3000])
    for entry in result['primitives']:
        print(json.dumps(entry))
    for call in result['text_calls']:
        print(json.dumps({k: call[k] for k in ('text', 'rect', 'format', 'font', 'color')}))


if __name__ == '__main__':
    main()
