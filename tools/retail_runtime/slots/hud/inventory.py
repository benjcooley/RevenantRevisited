#!/usr/bin/env python3
"""Retail's inventory pane over the HUD world: the side panel's lower slot,
TInventory (`0x0065d4f8`, vtable `0x005a58c0`).

Per case, from the booted world's checkpoint:

1. Inventory.dat through retail's loader into the global TPlayScreen fills
   (`0x0065c130`).
2. A fixture player (hudscene.py) carrying the case's items: real items in
   its inventory slots (itempanes.py). Gold is an item like any other; the
   pane shows the player's total of it (`GetInventoryAmount("Gold")`,
   vtable +0x84).
3. The pane at the side panel's lower slot, (452, 306), 188 x 174, and its
   Initialize (`0x00537650`).
4. What it shows: the player's inventory, or the inside of a bag the player
   carries (`"bag": slot`), through the pane's SetContainer (`0x005391a0`:
   +0x17c, and the bag's icon at half size, `0x00538e40`), from the case's
   `first` slot (pane +0x188, the page arrows' offset). The game frame, which picks an animated
   icon's frame, is the case's `tick`.
5. `frames` frames, each as the screen draws the pane: its clip rect on the
   display (vtable +0x24), its background pass (slot 20 `0x00537a30`: the
   grid, slot 21 `0x00537a70`, when the pane is dirty, then its arrow
   buttons), and its Animate (`0x00537f90`, draw on: the animated items).
   The back buffer is cleared to black before the last frame.
6. Recorded: the back buffer (RGB565 + PNG), the last frame's primitives
   (primitives.py; bitmaps named Archive:Name), the GDI text calls, and the
   `blend.alpha` mask (itempanes.py). The retail bugs itempanes.py fixes are
   fixed unless a case's `"retail_bugs": true` keeps them.

    inventory.py EXE --case case.json --out DIR      one case
    inventory.py EXE --serve                          JSONL: {"id","case","out"}

Case: {"name": "...", "items": [{"type": "Short Sword"}, null, {"type":
"Gold", "amount": 42}, {"slot": 13, "type": "Pouch", "contents": [{"type":
"Ale Mug"}]}], "bag": null, "first": 0, "tick": 0, "frames": 1}. An item's
slot is its index in `items` unless it names one.
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
from itempanes import GET_INVENTORY, ItemPaneKit, save_capture, serve  # noqa: E402
from primitives import PrimitiveTap, s32  # noqa: E402

INVENTORY = 0x0065d4f8
G_INVENTORY_DAT = 0x0065c130
INITIALIZE = 0x00537650
SET_CONTAINER = 0x005391a0                       # thiscall (container): + the bag's icon, page 0
BACKGROUND = 0x00537a30                          # slot 20: the grid when dirty, then the buttons
ANIMATE = 0x00537f90                             # (draw)
FIRST_SLOT = 0x188                               # TInventory
DIRTY = 0x50                                     # TPane
VT_SET_CLIP_RECT = 0x24
PANE_RECT = (452, 306, 188, 174)                 # the side panel's lower slot
SCHEMA = 'hud.inventory.v1'


class InventoryFixture:
    def __init__(self, executable):
        started = time.perf_counter()
        self.world = world = HudWorld.open(executable)
        self.vm = world.vm
        self.scene = HudScene(world)
        self.scene.load_archive('Inventory.dat', G_INVENTORY_DAT)
        self.tap = PrimitiveTap(self.vm, self._surface_name, self.scene.bitmap_names())
        self.kit = ItemPaneKit(world, self.scene, self.tap)
        world.checkpoint()
        self.setup_ms = (time.perf_counter() - started) * 1000

    def _surface_name(self, pointer):
        if pointer == INVENTORY:
            return 'inventory'
        return self.scene.screen_surface(pointer) or (f'0x{pointer:08x}' if pointer else None)

    def _player(self, case):
        player = self.scene.character(case.get('player', {}).get('name', 'Locke'), player=True)
        for index, spec in enumerate(case.get('items', [])):
            if spec:
                self.kit.place(player.address, spec, int(spec.get('slot', index)))
        return player

    def run(self, case, out_dir=None):
        world, scene, vm, kit = self.world, self.scene, self.vm, self.kit
        world.restore()
        kit.fix_retail_bugs = not case.get('retail_bugs', False)
        player = self._player(case)
        scene.set_player(player)
        kit.game_frame(case.get('tick', 0))
        for offset, value in zip((4, 8, 0xc, 0x10), PANE_RECT):
            vm.put_u32(INVENTORY + offset, value)
        if not world.call(INITIALIZE, this=INVENTORY, instruction_limit=500_000_000):
            raise RuntimeError('the inventory pane did not initialize')
        container = player.address
        if case.get('bag') is not None:
            container = world.call(GET_INVENTORY, (int(case['bag']),), this=player.address,
                                   instruction_limit=50_000_000)
            if not container:
                raise RuntimeError(f'no bag in slot {case["bag"]}')
        world.call(SET_CONTAINER, (container,), this=INVENTORY, instruction_limit=500_000_000)
        vm.put_u32(INVENTORY + FIRST_SLOT, int(case.get('first', 0)))
        frames = int(case.get('frames', 1))
        text_start = 0
        for frame in range(frames):
            if frame == frames - 1:
                scene.clear()
                self.tap.start()
                kit.record(PANE_RECT[:2])
                text_start = len(world.gdi.text_calls)
            vm.put_u32(INVENTORY + DIRTY, 1)
            world.call(vm.u32(vm.u32(INVENTORY) + VT_SET_CLIP_RECT), (0,), this=INVENTORY)
            world.call(BACKGROUND, this=INVENTORY, instruction_limit=500_000_000)
            world.call(ANIMATE, (1,), this=INVENTORY, instruction_limit=500_000_000)
        primitives = self.tap.stop()
        return dict(schema=SCHEMA, case=case.get('name'), frames=frames,
                    pane=[s32(vm.u32(INVENTORY + o)) for o in (4, 8, 0xc, 0x10)],
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
    fixture = InventoryFixture(args.executable)
    if args.serve:
        serve(fixture.run, fixture.world.sha, SCHEMA, fixture.setup_ms)
        return
    case = json.loads(args.case.read_text()) if args.case else dict(
        name='sample', items=[{'type': 'Short Sword'}, {'type': 'Ale Mug'}, {'type': 'Gold', 'amount': 42}])
    result = fixture.run(case, args.out)
    print(json.dumps({k: v for k, v in result.items() if k != 'primitives'}, indent=2))
    for entry in result['primitives']:
        print(json.dumps(entry))


if __name__ == '__main__':
    main()
