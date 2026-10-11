#!/usr/bin/env python3
"""Retail's spell pane over the HUD world: the side panel's lower slot,
TSpellPane (`0x006661b0`, vtable `0x005a5978`), where the player puts a
spell together from talismans and casts it.

Per case, from the booted world's checkpoint:

1. SpellPane.dat through retail's loader into the global TPlayScreen fills
   (`0x0065a574`); the talismans' plates and icons are GameData's
   (playscrn.dat, `0x0065abc0`), which the boot loaded.
2. A fixture player (hudscene.py) carrying the case's items (itempanes.py):
   a "Spell Pouch" with talismans in it is what the pane lists. The spell
   being put together is the player's construct quick spell, button 0 of
   the talisman strings at player +0x2cc (6 bytes a button).
3. The pane at the side panel's lower slot, (452, 306), 188 x 174, and its
   Initialize (`0x005432a0`); then the case's names toggle (pane +0x17c,
   the "min" button), first line (+0x180, the arrows' offset), pressed
   talisman (+0x184, the type, with +0x188 on: the pointer still on it) and
   whether the "spell" button is held (its flag 0x10000).
4. `frames` frames, each as the screen draws the pane: its clip rect on the
   display (vtable +0x24), its slot 20 (`0x005435c0`: the background,
   slot 21 `0x005435f0`, when the pane is dirty, then the buttons) and its
   Animate (vtable +0x58, draw on). The back buffer is cleared to black
   before the last frame.
5. Recorded: the back buffer (RGB565 + PNG), the last frame's primitives
   (primitives.py; bitmaps named Archive:Name), the GDI text calls, and the
   `blend.alpha` mask (itempanes.py). The retail bugs fixed are fixed unless
   a case's `"retail_bugs": true` keeps them.

Retail bugs the fixture fixes:
- With no spell pouch the page arrows show, with nothing to page: the
  background (`0x005435f0`) sets them only while the player has a pouch, and
  Initialize built them shown. Fixed: with no pouch they are hidden (button
  flag 2), as the pane hides them with no talismans to page.

    spellpane.py EXE --case case.json --out DIR      one case
    spellpane.py EXE --serve                          JSONL: {"id","case","out"}

Case: {"name": "...", "items": [{"type": "Spell Pouch", "contents": [{"type":
"Sun"}, {"type": "Moon"}]}], "spell": "AL", "names": true, "first": 0,
"pressed": -1, "invoke": false, "tick": 0, "frames": 1}. An item's slot is its index in `items` unless it
names one.
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
from hudscene import HudScene  # noqa: E402
from hudworld import HudWorld  # noqa: E402
from itempanes import ItemPaneKit, save_capture, serve  # noqa: E402
from primitives import PrimitiveTap, s32  # noqa: E402

SPELL_PANE = 0x006661b0
G_SPELLPANE_DAT = 0x0065a574
G_GAMEDATA = 0x0065abc0                          # playscrn.dat: the talismans' plates and icons
INITIALIZE = 0x005432a0
DRAW = 0x005435c0                                # slot 20: the background when dirty, then the buttons
SHOW_NAMES, FIRST_LINE, PRESSED, ON_PRESSED = 0x17c, 0x180, 0x184, 0x188     # TSpellPane
BUTTONS = 0x98                                   # TButtonPane: the button array; "spell" is first
BUTTON_FLAGS, BUTTON_HIDDEN, BUTTON_PRESSED = 0x14, 0x2, 0x10000     # TButton
UP, DOWN = 2, 3                                  # the page arrows' buttons
DIRTY = 0x50                                     # TPane
VT_SET_CLIP_RECT, VT_ANIMATE = 0x24, 0x58
QUICKSPELLS, QUICKSPELL_BYTES = 0x2cc, 6         # player: the talisman strings; button 0 is the construct spell
PANE_RECT = (452, 306, 188, 174)                 # the side panel's lower slot
SCHEMA = 'hud.spellpane.v1'


class SpellPaneFixture:
    def __init__(self, executable):
        started = time.perf_counter()
        self.world = world = HudWorld.open(executable)
        self.vm = world.vm
        self.scene = HudScene(world)
        self.scene.load_archive('SpellPane.dat', G_SPELLPANE_DAT)
        self.scene.adopt_archive('playscrn.dat', G_GAMEDATA)
        self.tap = PrimitiveTap(self.vm, self._surface_name, self.scene.bitmap_names())
        self.kit = ItemPaneKit(world, self.scene, self.tap)
        world.checkpoint()
        self.setup_ms = (time.perf_counter() - started) * 1000

    def _surface_name(self, pointer):
        if pointer == SPELL_PANE:
            return 'spellpane'
        return self.scene.screen_surface(pointer) or (f'0x{pointer:08x}' if pointer else None)

    def _player(self, case):
        player = self.scene.character(case.get('player', {}).get('name', 'Locke'), player=True)
        for index, spec in enumerate(case.get('items', [])):
            if spec:
                self.kit.place(player.address, spec, int(spec.get('slot', index)))
        data = case.get('spell', '').encode('ascii')
        if len(data) >= QUICKSPELL_BYTES:
            raise ValueError(f'spell {case["spell"]!r}: at most {QUICKSPELL_BYTES - 1} talismans')
        self.vm.write(player.address + QUICKSPELLS, data.ljust(QUICKSPELL_BYTES, b'\0'))
        return player

    def run(self, case, out_dir=None):
        world, scene, vm, kit = self.world, self.scene, self.vm, self.kit
        world.restore()
        kit.fix_retail_bugs = not case.get('retail_bugs', False)
        scene.set_player(self._player(case))
        kit.game_frame(case.get('tick', 0))
        for offset, value in zip((4, 8, 0xc, 0x10), PANE_RECT):
            vm.put_u32(SPELL_PANE + offset, value)
        if not world.call(INITIALIZE, this=SPELL_PANE, instruction_limit=500_000_000):
            raise RuntimeError('the spell pane did not initialize')
        vm.put_u32(SPELL_PANE + SHOW_NAMES, int(case.get('names', True)))
        vm.put_u32(SPELL_PANE + FIRST_LINE, int(case.get('first', 0)))
        pressed = int(case.get('pressed', -1))
        vm.put_u32(SPELL_PANE + PRESSED, pressed & 0xffffffff)
        vm.put_u32(SPELL_PANE + ON_PRESSED, int(pressed >= 0))
        buttons = vm.u32(SPELL_PANE + BUTTONS)
        if case.get('invoke'):
            spell = vm.u32(buttons)
            vm.put_u32(spell + BUTTON_FLAGS, vm.u32(spell + BUTTON_FLAGS) | BUTTON_PRESSED)
        if kit.fix_retail_bugs and not any(spec and spec['type'].lower() in ('spell pouch', 'spellpouch')
                                           for spec in case.get('items', [])):
            for index in (UP, DOWN):
                arrow = vm.u32(buttons + 4 * index)
                vm.put_u32(arrow + BUTTON_FLAGS, vm.u32(arrow + BUTTON_FLAGS) | BUTTON_HIDDEN)
        frames = int(case.get('frames', 1))
        text_start = 0
        for frame in range(frames):
            if frame == frames - 1:
                scene.clear()
                self.tap.start()
                kit.record(PANE_RECT[:2])
                text_start = len(world.gdi.text_calls)
            vm.put_u32(SPELL_PANE + DIRTY, 1)
            world.call(vm.u32(vm.u32(SPELL_PANE) + VT_SET_CLIP_RECT), (0,), this=SPELL_PANE)
            world.call(DRAW, this=SPELL_PANE, instruction_limit=500_000_000)
            world.call(vm.u32(vm.u32(SPELL_PANE) + VT_ANIMATE), (1,), this=SPELL_PANE,
                       instruction_limit=500_000_000)
        primitives = self.tap.stop()
        return dict(schema=SCHEMA, case=case.get('name'), frames=frames,
                    pane=[s32(vm.u32(SPELL_PANE + o)) for o in (4, 8, 0xc, 0x10)],
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
    fixture = SpellPaneFixture(args.executable)
    if args.serve:
        serve(fixture.run, fixture.world.sha, SCHEMA, fixture.setup_ms)
        return
    case = json.loads(args.case.read_text()) if args.case else dict(
        name='sample', items=[{'type': 'Spell Pouch', 'contents': [{'type': 'Sun'}, {'type': 'Life'},
                                                                   {'type': 'Moon'}]}], spell='AL')
    result = fixture.run(case, args.out)
    print(json.dumps({k: v for k, v in result.items() if k != 'primitives'}, indent=2))
    for entry in result['primitives']:
        print(json.dumps(entry))


if __name__ == '__main__':
    main()
