#!/usr/bin/env python3
"""Retail's spell book over the HUD world: the side panel's upper slot, the
known-spell list (class 0x005a5ae8, global `0x0065a9d8`), a TButtonPane.

Per case, from the booted world's checkpoint:

1. SpellScroll.dat and SpellIcons.dat through retail's loader into the
   globals TPlayScreen::Initialize fills (`0x00666640` / `0x0065bc3c`), and
   spell.def into the spell list (`0x00667c38`: clear `0x0053ea00`, load
   `0x0053ead0`), as Initialize does.
2. A fixture player (hudscene.py) with the case's known spells, added in
   order through retail's own pointer-array Add (`0x0041c840`) to player
   +0x2ec (talisman strings), and the case's items (itempanes.py: a "Spell
   Pouch" of talismans makes spells castable, `0x0051b7c0`).
3. The pane at the side panel's upper slot, (452, 0), 188 x 306, its
   Initialize (`0x005449e0`) and the list rebuild (`0x00546830`, which
   TPlayerManager::SetMainPlayerNum runs for the main player); then the
   case's pane fields (`fields`: {"0x17c": value, ...}) -- the scroll and
   the selection, as the forensics name them.
4. `frames` frames, each as the screen draws the pane: its clip rect on the
   display (vtable +0x24), the pane dirty, its slot 20 (`0x00544eb0`: the
   paint, slot 21 `0x005452d0`, then the buttons). The back buffer is
   cleared to black before the last frame.
5. Recorded: the back buffer (RGB565 + PNG), the last frame's primitives
   (primitives.py; bitmaps named Archive:Name), the GDI text calls (the
   emulator measures with the real Arial / Times metrics but draws no
   glyphs), and the `blend.alpha` mask (itempanes.py).

    spellbook.py EXE --case case.json --out DIR      one case
    spellbook.py EXE --serve                          JSONL: {"id","case","out"}

Case: {"name": "...", "spells": ["DEB", "KJL", "LI"], "items": [{"type":
"Spell Pouch", "contents": [{"type": "Law"}]}], "player": {"stats":
{"mana": 40}}, "fields": {"0x17c": scroll, "0x1c8": pressed row},
"frames": 1}.
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
from hudscene import HudScene, Stats  # noqa: E402
from hudworld import HudWorld  # noqa: E402
from itempanes import ItemPaneKit, save_capture, serve  # noqa: E402
from primitives import PrimitiveTap, s32  # noqa: E402

SPELL_BOOK = 0x0065a9d8
G_SPELLSCROLL_DAT, G_SPELLICONS_DAT = 0x00666640, 0x0065bc3c
SPELL_LIST, SPELL_LIST_CLEAR, SPELL_LIST_LOAD = 0x00667c38, 0x0053ea00, 0x0053ead0
INITIALIZE = 0x005449e0
REBUILD = 0x00546830                             # the list's rows, as SetMainPlayerNum has them built
DRAW = 0x00544eb0                                # slot 20: the paint when dirty, then the buttons
PTR_ARRAY_ADD = 0x0041c840                       # thiscall (item)
KNOWN_SPELLS, ARRAY_GROWBY = 0x2ec, 0xc          # player: the known spells' pointer array
CONTENT, CONTENT_RECT = 0x180, (18, 47, 166, 276)  # the rows' surface, where it shows in the pane
DIRTY = 0x50                                     # TPane
VT_SET_CLIP_RECT = 0x24
PANE_RECT = (452, 0, 188, 306)                   # the side panel's upper slot
SCHEMA = 'hud.spellbook.v1'


class SpellBookFixture:
    def __init__(self, executable):
        started = time.perf_counter()
        self.world = world = HudWorld.open(executable)
        self.vm = world.vm
        self.scene = HudScene(world)
        self.scene.load_archive('SpellScroll.dat', G_SPELLSCROLL_DAT)
        self.scene.load_archive('SpellIcons.dat', G_SPELLICONS_DAT)
        world.call(SPELL_LIST_CLEAR, this=SPELL_LIST, instruction_limit=500_000_000)
        world.call(SPELL_LIST_LOAD, this=SPELL_LIST, instruction_limit=500_000_000)
        self.tap = PrimitiveTap(self.vm, self._surface_name, self.scene.bitmap_names())
        self.kit = ItemPaneKit(world, self.scene, self.tap)
        world.checkpoint()
        self.setup_ms = (time.perf_counter() - started) * 1000

    def _surface_name(self, pointer):
        if pointer == SPELL_BOOK:
            return 'spellbook'
        return self.scene.screen_surface(pointer) or (f'0x{pointer:08x}' if pointer else None)

    def _player(self, case):
        world, vm = self.world, self.vm
        spec = case.get('player', {})
        player = self.scene.character(spec.get('name', 'Locke'), player=True, stats=Stats(**spec.get('stats', {})))
        known = player.address + KNOWN_SPELLS
        vm.put_u32(known + ARRAY_GROWBY, 4)
        for talismans in case.get('spells', []):
            world.call(PTR_ARRAY_ADD, (self.scene.string(talismans),), this=known, instruction_limit=5_000_000)
        for index, spec in enumerate(case.get('items', [])):
            if spec:
                self.kit.place(player.address, spec, int(spec.get('slot', index)))
        return player

    def run(self, case, out_dir=None):
        world, scene, vm, kit = self.world, self.scene, self.vm, self.kit
        world.restore()
        kit.fix_retail_bugs = not case.get('retail_bugs', False)
        scene.set_player(self._player(case))
        for offset, value in zip((4, 8, 0xc, 0x10), PANE_RECT):
            vm.put_u32(SPELL_BOOK + offset, value)
        if not world.call(INITIALIZE, this=SPELL_BOOK, instruction_limit=2_000_000_000):
            raise RuntimeError('the spell book did not initialize')
        world.call(REBUILD, this=SPELL_BOOK, instruction_limit=2_000_000_000)
        for offset, value in case.get('fields', {}).items():
            vm.put_u32(SPELL_BOOK + int(offset, 0), int(value) & 0xffffffff)
        frames = int(case.get('frames', 1))
        text_start = 0
        for frame in range(frames):
            if frame == frames - 1:
                scene.clear()
                self.tap.start()
                kit.record(PANE_RECT[:2], {vm.u32(SPELL_BOOK + CONTENT): (*CONTENT_RECT[:2], CONTENT_RECT)})
                text_start = len(world.gdi.text_calls)
            vm.put_u32(SPELL_BOOK + DIRTY, 1)
            world.call(vm.u32(vm.u32(SPELL_BOOK) + VT_SET_CLIP_RECT), (0,), this=SPELL_BOOK)
            world.call(DRAW, this=SPELL_BOOK, instruction_limit=2_000_000_000)
        primitives = self.tap.stop()
        return dict(schema=SCHEMA, case=case.get('name'), frames=frames,
                    pane=[s32(vm.u32(SPELL_BOOK + o)) for o in (4, 8, 0xc, 0x10)],
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
    fixture = SpellBookFixture(args.executable)
    if args.serve:
        serve(fixture.run, fixture.world.sha, SCHEMA, fixture.setup_ms)
        return
    case = json.loads(args.case.read_text()) if args.case else dict(
        name='sample', spells=['DEB', 'KJL', 'LI', 'CIB', 'BEA'])
    result = fixture.run(case, args.out)
    print(json.dumps({k: v for k, v in result.items() if k not in ('primitives',)}, indent=1)[:6000])
    for entry in result['primitives']:
        print(json.dumps(entry))


if __name__ == '__main__':
    main()
