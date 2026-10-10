#!/usr/bin/env python3
"""Retail's bottom bar over the HUD world: TBottomBarPane (`0x0065b638`) and
the two panes it paints, TBarInvPane (`0x0065b028`, the potion shelf) and
TQuickSpellPane (`0x0065c6f8`, the quick-spell rings).

Per case, from the booted world's checkpoint:

1. BottomBar.dat and SpellIcons.dat through retail's loader into the
   globals TPlayScreen::Initialize fills (`0x0065a570` / `0x0065bc3c`), and
   spell.def into the spell list (`0x00667c38`: clear `0x0053ea00`, load
   `0x0053ead0`), as Initialize does.
2. A fixture player (hudscene.py) with the case's quick spells: talisman
   strings at player +0x2cc, 6 bytes a button (`0x0051b560`). Whether the
   player can cast one -- TCharacter `0x0051b7c0`, which looks for the
   talismans in the player's inventory -- is answered from the case's
   `castable` list, so the rings' states don't need inventory objects.
3. The belt: real items (HudScene.new_object, retail's own NewObject) put
   in belt slots 0x10b + n by retail's AddToInventory (`0x0046f940`). They
   go in a real container, a Pouch -- a TPlayer can't be built outside a
   game, and the shelf only walks the container it is given (pane +0x60).
   A pouch item's contents go in its slots 0, 1, ...
   - An amount goes in through the item's SetAmount (vtable +0x19c), which
     for ammo and money also composes the icon for it; other items have no
     amount (their SetAmount does nothing).
   - The game's frame count, which picks an animated icon's frame
     (TPlayScreen `0x0047e920` on `0x0065caf0`), is the case's `tick`.
4. The width TPlayScreen::Pulse gives the bar (`TBottomBarPane::SetRect`
   `0x0052c930`, which sets all three panes' template size), then the
   panes' Initialize in TPlayScreen's order: quick spells `0x00544160`, the
   shelf `0x0052c970`, the bar `0x0052c780`.
5. `frames` frames, each as the screen draws the bar:
   - the bar's clip rect on the display (vtable +0x24) and its slot 20
     (`0x0052c800`: the bar, the shelf's boxes and still items, the rings);
   - the shelf's clip rect and its Animate (slot 22 `0x0052cd80`, draw
     on): the animated items.
   The back buffer is cleared to black before the last frame.
6. Recorded: the back buffer (RGB565 + PNG), the last frame's primitives
   (primitives.py; bitmaps named Archive:Name), the GDI text calls, and a
   mask of the screen (one byte per pixel, zlib, hex): `blend.alpha`, the
   pixels a DM_ALPHA Put mixed -- coverage strictly between 0 and 31. Retail
   mixes those through its 5-bit scale tables (the blit at 0x004b3694:
   d * (31 - a) // 31 + s * a // 31 per field, green in two halves), which
   truncate where a float blend doesn't: never more than two RGB565 steps
   apart, over every input.

Retail bugs the fixture fixes (a case's `"retail_bugs": true` keeps them):
- A pouch's thumbnail of a stacked item (ammo, money) sits on a black square.
  The stack's icon is composed on 0, its key; the 2:1 reduction
  (`0x004a31a0`) keys 16-bit sources on the one value it is given -- the
  display's key, 0xf81f, which also fills the thumbnail's empty pixels -- so
  the background averages in as black. Fixed: the reduction reads a copy of a
  15/16-bit source whose own key pixels are the display's key, so they are
  skipped and fill as empty.

    bottombar.py EXE --case case.json --out DIR      one case
    bottombar.py EXE --serve                          JSONL: {"id","case","out"}

Case: {"name": "...", "player": {"name": "Locke", "quickspells": ["LI", "",
"", ""], "castable": ["LI"]}, "belt": [{"type": "Lesser Healing"}, null,
{"type": "Gold", "amount": 42}, {"type": "Pouch", "contents": [{"type":
"Lesser Healing"}]}], "scroll": 0, "tick": 0, "pane_width": 452, "frames": 1}
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
import time
import zlib
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(HERE.parents[1]))
from unicorn import UC_HOOK_CODE  # noqa: E402
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_EIP, UC_X86_REG_ESP  # noqa: E402
from hudscene import HudScene, png  # noqa: E402
from hudworld import HudWorld  # noqa: E402
from primitives import PrimitiveTap, s32  # noqa: E402

BOTTOM_BAR, BAR_INV, QUICK_SPELL = 0x0065b638, 0x0065b028, 0x0065c6f8
PANE_NAMES = {BOTTOM_BAR: 'bottombar', BAR_INV: 'barinv', QUICK_SPELL: 'quickspell'}
G_BOTTOMBAR_DAT, G_SPELLICONS_DAT = 0x0065a570, 0x0065bc3c
SPELL_LIST, SPELL_LIST_CLEAR, SPELL_LIST_LOAD = 0x00667c38, 0x0053ea00, 0x0053ead0
CAN_CAST = 0x0051b7c0                            # TCharacter, thiscall (talismans), ret 4
REDUCE = 0x004a31a0                              # cdecl (source, destination, key)
BM_15BIT, BM_16BIT = 0x2, 0x4
BITMAP_HEADER = 0x48                             # a TBitmap's pixels follow its header
SET_RECT = 0x0052c930
INITIALIZE = ((0x00544160, QUICK_SPELL), (0x0052c970, BAR_INV), (0x0052c780, BOTTOM_BAR))
PAINT = 0x0052c800                                # TBottomBarPane slot 20
SHELF_ANIMATE = 0x0052cd80                        # TBarInvPane slot 22, (draw)
ADD_TO_INVENTORY = 0x0046f940                     # TObjectInstance, thiscall (name, number, slot)
GET_INVENTORY = 0x004701f0                        # TObjectInstance, thiscall (slot) -> item
SHELF_CONTAINER, SHELF_SCROLL = 0x60, 0x88        # TBarInvPane fields
BELT_FIRST = 0x10b                                # the belt's first inventory slot
VT_SET_AMOUNT = 0x19c
PLAY_SCREEN = 0x0065caf0                          # TPlayScreen: game frames +0x680 + (+0x688 - +0x684)
GAME_FRAMES, SESSION_START, SESSION_FRAMES = 0x680, 0x684, 0x688
VT_SET_CLIP_RECT = 0x24
DM_ALPHA, DM_USEDEFAULT, BM_ALPHA = 0x2000, 0x80000000, 0x100
OPAQUE_ALPHA = 31
SCREEN = (640, 480)
QUICKSPELLS, QUICKSPELL_BYTES, QUICK_BUTTONS = 0x2cc, 6, 4
BAR_HEIGHT = 60
DEFAULT_PANE_WIDTH = 640 - 188                    # the display less the shown side pane
SCHEMA = 'hud.bottombar.v1'


class BottomBarFixture:
    def __init__(self, executable):
        started = time.perf_counter()
        self.world = world = HudWorld.open(executable)
        self.vm = world.vm
        self.scene = HudScene(world)
        self.scene.load_archive('BottomBar.dat', G_BOTTOMBAR_DAT)
        self.scene.load_archive('SpellIcons.dat', G_SPELLICONS_DAT)
        world.call(SPELL_LIST_CLEAR, this=SPELL_LIST, instruction_limit=500_000_000)
        world.call(SPELL_LIST_LOAD, this=SPELL_LIST, instruction_limit=500_000_000)
        self.tap = PrimitiveTap(self.vm, self._surface_name, self.scene.bitmap_names())
        self.castable = set()
        self.vm.uc.hook_add(UC_HOOK_CODE, self._can_cast, begin=CAN_CAST, end=CAN_CAST)
        self.alpha = None                         # the last frame's DM_ALPHA mask, while it draws
        self.fix_retail_bugs = True
        self.vm.uc.hook_add(UC_HOOK_CODE, self._reduce_on_own_key, begin=REDUCE, end=REDUCE)
        self.origin = (0, 0)                      # the bar's screen position: its panes' clip origin
        self.tap.listeners.append(self._track_alpha)
        world.checkpoint()
        self.setup_ms = (time.perf_counter() - started) * 1000

    def _surface_name(self, pointer):
        return (PANE_NAMES.get(pointer) or self.scene.screen_surface(pointer)
                or (f'0x{pointer:08x}' if pointer else None))

    def _track_alpha(self, name, this, a):
        """Marks the screen pixels a DM_ALPHA Put mixes (the panes draw onto
        the display at the bar's origin)."""
        if self.alpha is None or name != 'Put' or self.scene.screen_surface(this) != 'display':
            return
        bitmap = a[2]
        fields = struct.unpack('<18I', self.vm.uc.mem_read(bitmap, 72))
        width, height, flags, drawmode = fields[0], fields[1], fields[4], fields[5]
        mode = drawmode if a[3] & DM_USEDEFAULT else a[3]
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

    def _reduce_on_own_key(self, uc, address, size, user):
        """The thumbnail fix: the 2:1 reduction reads a 15/16-bit source whose
        own key pixels are recoloured to the key it is given."""
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

    def _can_cast(self, uc, address, size, user):
        sp = uc.reg_read(UC_X86_REG_ESP)
        talismans = self.vm.string(self.vm.u32(sp + 4))
        uc.reg_write(UC_X86_REG_EAX, int(talismans in self.castable))
        uc.reg_write(UC_X86_REG_EIP, self.vm.u32(sp))
        uc.reg_write(UC_X86_REG_ESP, sp + 8)

    def _item(self, container, spec, slot):
        """A real item of the spec's type in `container` at `slot`."""
        if not self.world.call(ADD_TO_INVENTORY, (self.scene.string(spec['type']), 1, slot), this=container,
                               instruction_limit=2_000_000_000):
            raise RuntimeError(f'AddToInventory refused {spec["type"]!r}')
        item = self.world.call(GET_INVENTORY, (slot,), this=container, instruction_limit=50_000_000)
        if not item:
            raise RuntimeError(f'{spec["type"]!r} did not land in slot 0x{slot:x}: retail stacked it with '
                               f'an item of its type (give each item in a container its own type)')
        if 'amount' in spec:
            self.world.call(self.vm.u32(self.vm.u32(item) + VT_SET_AMOUNT), (int(spec['amount']),), this=item,
                            instruction_limit=500_000_000)
        for index, content in enumerate(spec.get('contents', [])):
            self._item(item, content, index)
        return item

    def _belt(self, specs):
        """A container holding the case's belt -> its address."""
        belt = self.scene.new_object('Pouch')
        for index, spec in enumerate(specs):
            if spec:
                self._item(belt, spec, BELT_FIRST + index)
        return belt

    def _player(self, spec):
        self.castable = set(spec.get('castable', []))
        player = self.scene.character(spec.get('name', 'Locke'), player=True)
        spells = spec.get('quickspells', [])
        if len(spells) > QUICK_BUTTONS:
            raise ValueError(f'at most {QUICK_BUTTONS} quick spells')
        for button, talismans in enumerate(spells, start=1):
            data = talismans.encode('ascii')
            if len(data) >= QUICKSPELL_BYTES:
                raise ValueError(f'quick spell {talismans!r}: at most {QUICKSPELL_BYTES - 1} talismans')
            self.vm.write(player.address + QUICKSPELLS + QUICKSPELL_BYTES * button,
                          data.ljust(QUICKSPELL_BYTES, b'\0'))
        return player

    def run(self, case, out_dir=None):
        world, scene, vm = self.world, self.scene, self.vm
        world.restore()
        self.fix_retail_bugs = not case.get('retail_bugs', False)
        scene.set_player(self._player(case.get('player', {})))
        belt = self._belt(case.get('belt', []))
        vm.put_u32(PLAY_SCREEN + GAME_FRAMES, int(case.get('tick', 0)))
        vm.put_u32(PLAY_SCREEN + SESSION_START, 0)
        vm.put_u32(PLAY_SCREEN + SESSION_FRAMES, 0)
        world.call(SET_RECT, (int(case.get('pane_width', DEFAULT_PANE_WIDTH)), BAR_HEIGHT), this=BOTTOM_BAR)
        for initialize, pane in INITIALIZE:
            world.call(initialize, this=pane, instruction_limit=200_000_000)
        vm.put_u32(BAR_INV + SHELF_CONTAINER, belt)
        vm.put_u32(BAR_INV + SHELF_SCROLL, int(case.get('scroll', 0)))
        frames = int(case.get('frames', 1))
        text_start = 0
        self.origin = (s32(vm.u32(BOTTOM_BAR + 4)), s32(vm.u32(BOTTOM_BAR + 8)))
        for frame in range(frames):
            if frame == frames - 1:
                scene.clear()
                self.tap.start()
                self.alpha = bytearray(SCREEN[0] * SCREEN[1])
                text_start = len(world.gdi.text_calls)
            world.call(vm.u32(vm.u32(BOTTOM_BAR) + VT_SET_CLIP_RECT), (0,), this=BOTTOM_BAR)
            world.call(PAINT, this=BOTTOM_BAR, instruction_limit=500_000_000)
            world.call(vm.u32(vm.u32(BAR_INV) + VT_SET_CLIP_RECT), (0,), this=BAR_INV)
            world.call(SHELF_ANIMATE, (1,), this=BAR_INV, instruction_limit=500_000_000)
        primitives = self.tap.stop()
        alpha, self.alpha = self.alpha, None
        width, height, rows = scene.capture()
        result = dict(schema=SCHEMA, case=case.get('name'), frames=frames,
                      pane=[s32(vm.u32(BOTTOM_BAR + o)) for o in (4, 8, 0xc, 0x10)],
                      primitives=primitives, blend=dict(alpha=zlib.compress(bytes(alpha), 9).hex()),
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
    fixture = BottomBarFixture(args.executable)
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
        name='rings', player=dict(name='Locke', quickspells=['LI', '', '', '']))
    result = fixture.run(case, args.out)
    print(json.dumps({k: v for k, v in result.items() if k != 'primitives'}, indent=2))
    for entry in result['primitives']:
        print(json.dumps(entry))


if __name__ == '__main__':
    main()
