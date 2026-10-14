#!/usr/bin/env python3
"""Retail's TFood::Use (`0x0050e780`, FOOD's and POTION's vtable slot 0xbc)
over the HUD slot's booted world (docs/gameplay/forensics/CONSUMABLES.md).

Per case, from the booted world's checkpoint:

1. The user: a fixture character (`hudscene.py`), a player (class 0x0b) or a
   character (0x0c). Its Health, Mana, Fatigue, their maxima and Poisoned are
   the case's; its setters SetHealth / SetFatigue / SetMana / SetPoisoned
   (slots 0x1c4 / 0x1cc / 0x1d4 / 0x1b4) are recorded and change them. Its
   position (+0x10) and level (+0xe) are the case's. The main player
   (`0x00667fcc`) is the user, another fixture player, or none.
2. The item: a real object of the case's type. In the user's pack or belt it
   is made there by retail's AddToInventory(name, 1, slot) (`0x0046f940`);
   in a pouch, a Pouch is made at the slot and the item in its slot 0; on the
   map, by NewObject alone (no owner). Its amount through its own SetAmount
   (slot 0x19c), its state (+0xc) the case's.
3. `item->Use(user, with)`.

Recorded while Use runs (boundaries, answered by the host):
- FindSound `0x0049c430` (the name; answered with a sound id), the load
  `0x0049b650` (answered 1) and Play `0x0049b990` (its arguments);
- TTextBar::Print `0x0054d170` (cdecl: the text bar, the format, its
  arguments; the line as formatted);
- the map iterator over a square `0x0044ceb0` (the cure's scan: the square,
  objset and level; answered with no objects, the iterator's current object
  `+0xc` left null).
Then the item: still where it was put (GetInventorySlot `0x004701f0`) and
its amount (slot 0x198); an item on the map is never removed.

    food_use.py EXE --case case.json     one case
    food_use.py EXE --serve              JSONL: {"id", "case"}

Case: {"name": "...", "item": {"type": "Bread", "amount": 1, "state": 0,
"where": "pack" | "pouch" | "map", "slot": 0}, "user": {"class": 11, "name":
"Locke", "health": 50, "maxhealth": 500, "mana": 50, "maxmana": 500,
"fatigue": 10, "maxfatigue": 100, "poisoned": 0, "pos": [1000, 2000, 0],
"level": 0}, "main": "user" | "other" | "none", "with": -1}
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / 'hud'))
sys.path.insert(0, str(HERE.parents[1]))
from unicorn.x86_const import UC_X86_REG_ECX  # noqa: E402
from hudscene import HudScene, Stats  # noqa: E402
from hudworld import HudWorld  # noqa: E402
from itempanes import serve  # noqa: E402

USE = 0x0050e780                                  # TFood::Use, thiscall (user, with), ret 8
ADD_TO_INVENTORY = 0x0046f940                     # TObjectInstance, thiscall (name, number, slot)
GET_INVENTORY = 0x004701f0                        # TObjectInstance, thiscall (slot) -> item
VT_AMOUNT, VT_SET_AMOUNT = 0x198, 0x19c
FIND_SOUND, LOAD_SOUND, PLAY_SOUND = 0x0049c430, 0x0049b650, 0x0049b990
TEXTBAR_PRINT = 0x0054d170                        # cdecl (textbar, format, ...)
MAP_SQUARE = 0x0044ceb0                           # iterator ctor, thiscall (pos, range, flags, objset, ?, level)
ITERATOR_CURRENT = 0xc
O_STATE, O_LEVEL, O_POS = 0x0c, 0x0e, 0x10
CLASS_PLAYER = 0x0b
SOUND_ID = 7                                      # what FindSound answers
FIXTURE_DLL = 'items.fixture'
# The user's stat setters and the Poisoned getter (TCharacter / TPlayer slots).
SETTERS = {0x1c4: 'health', 0x1cc: 'fatigue', 0x1d4: 'mana', 0x1b4: 'poisoned'}
SLOT_POISONED = 0x1b0
SCHEMA = 'items.food_use.v1'


def s32(value):
    return value - (1 << 32) if value & 0x80000000 else value


class FoodUseFixture:
    def __init__(self, executable):
        started = time.perf_counter()
        self.world = world = HudWorld.open(executable)
        self.vm = world.vm
        self.scene = HudScene(world)
        self.poisoned = {}                        # user address -> Poisoned
        self.recording = False
        self.log = None
        # The user's stubbed slots -> their thunks, made before the checkpoint.
        self.stubs = {slot: self._stub(f'set+0x{slot:x}', lambda args, s=stat: self._set(s, args[0]), argc=1)
                      for slot, stat in SETTERS.items()}
        self.stubs[SLOT_POISONED] = self._stub(f'get+0x{SLOT_POISONED:x}', lambda args: self.poisoned[self._ecx()])
        world.boundary(FIND_SOUND, 'FindSound', 4, self._find_sound)
        world.boundary(LOAD_SOUND, 'LoadSound', 4, lambda args, ecx: (1, args[0]))
        world.boundary(PLAY_SOUND, 'PlaySound', 0x18, self._play_sound)
        world.boundary(TEXTBAR_PRINT, 'TextBar', 0, self._text)
        world.boundary(MAP_SQUARE, 'MapSquare', 0x18, self._map_square)
        world.checkpoint()
        self.setup_ms = (time.perf_counter() - started) * 1000

    # ---- seams -----------------------------------------------------------

    def _stub(self, name, handler, argc=0):
        self.vm.handlers[name] = (argc, handler)          # thiscall: the callee pops `argc`
        self.vm.api_dlls[name] = {FIXTURE_DLL}
        return self.vm.api_address(FIXTURE_DLL, name)

    def _ecx(self):
        return self.vm.uc.reg_read(UC_X86_REG_ECX)

    def _set(self, stat, value):
        address, value = self._ecx(), s32(value)
        if stat == 'poisoned':
            self.poisoned[address] = value
        else:
            setattr(self.scene.characters[address].stats, stat, value)
        if self.recording:
            self.log['writes'].append([stat, value])
        return 0

    def _find_sound(self, args, ecx):
        name = self.vm.string(args[0])
        if self.recording:
            self.log['sounds'].append(name)
        return SOUND_ID, name

    def _play_sound(self, args, ecx):
        if self.recording:
            self.log['plays'].append([s32(a) for a in args[:6]])
        return 1, args[0]

    def _text(self, args, ecx):
        line = self._format(args[1], list(args[2:]))
        if self.recording:
            self.log['text'].append(line)
        return 0, line

    def _format(self, fmt_address, values):
        """The printf subset the game's messages use: %s, %d, %%."""
        fmt, out, i = self.vm.string(fmt_address), [], 0
        while i < len(fmt):
            if fmt[i] == '%' and i + 1 < len(fmt):
                kind = fmt[i + 1]
                if kind == 's':
                    out.append(self.vm.string(values.pop(0)))
                elif kind == 'd':
                    out.append(str(s32(values.pop(0))))
                else:
                    out.append(kind)
                i += 2
            else:
                out.append(fmt[i])
                i += 1
        return ''.join(out)

    def _map_square(self, args, ecx):
        x, y, z = struct.unpack('<3i', self.vm.uc.mem_read(args[0], 12))
        radius, objset, level = s32(args[1]), args[3], args[5] & 0xffff
        self.vm.put_u32(ecx + ITERATOR_CURRENT, 0)      # no objects
        if self.recording:
            self.log['scans'].append(dict(rect=[x - radius, y - radius, x + radius, y + radius],
                                          objset=objset, level=level, flags=args[2]))
        return ecx, None

    # ---- the case --------------------------------------------------------

    def _user(self, spec):
        player = spec.get('class', CLASS_PLAYER) == CLASS_PLAYER
        stats = Stats(health=spec.get('health', 100), max_health=spec.get('maxhealth', 100),
                      fatigue=spec.get('fatigue', 100), max_fatigue=spec.get('maxfatigue', 100),
                      mana=spec.get('mana', 100), max_mana=spec.get('maxmana', 100))
        user = self.scene.character(spec.get('name', 'Locke'), player=player, stats=stats)
        vm, address = self.vm, user.address
        vtable = vm.u32(address)
        for slot, thunk in self.stubs.items():
            vm.put_u32(vtable + slot, thunk)
        self.poisoned[address] = spec.get('poisoned', 0)
        x, y, z = spec.get('pos', [0, 0, 0])
        vm.write(address + O_POS, struct.pack('<3i', x, y, z))
        vm.write(address + O_LEVEL, struct.pack('<H', spec.get('level', 0)))
        return address

    def _add(self, owner, name, slot):
        world = self.world
        if not world.call(ADD_TO_INVENTORY, (self.scene.string(name), 1, slot), this=owner,
                          instruction_limit=2_000_000_000):
            raise RuntimeError(f'AddToInventory refused {name!r} at slot {slot}')
        item = world.call(GET_INVENTORY, (slot,), this=owner)
        if not item:
            raise RuntimeError(f'no {name!r} at slot {slot} after AddToInventory')
        return item

    def _item(self, spec, user):
        """(item, owner, slot); owner 0 for an item on the map."""
        where, slot, kind = spec.get('where', 'pack'), spec.get('slot', 0), spec['type']
        if where == 'map':
            item, owner, slot = self.scene.new_object(kind), 0, -1
        elif where == 'pouch':
            owner = self._add(user, 'Pouch', slot)
            item, slot = self._add(owner, kind, 0), 0
        else:
            item, owner = self._add(user, kind, slot), user
        if spec.get('amount', 1) != 1:
            self.world.call(self.vm.u32(self.vm.u32(item) + VT_SET_AMOUNT), (spec['amount'],), this=item)
        self.vm.write(item + O_STATE, struct.pack('<H', spec.get('state', 0)))
        return item, owner, slot

    def run(self, case, out_dir=None):
        world, vm, scene = self.world, self.vm, self.scene
        world.restore()
        self.poisoned = {}
        user = self._user(case.get('user', {}))
        main = case.get('main', 'user')
        scene.set_player(None)
        if main == 'user':
            scene.set_player(scene.characters[user])
        elif main == 'other':
            scene.set_player(scene.character('Somebody', player=True))
        item, owner, slot = self._item(case['item'], user)

        self.log = dict(writes=[], sounds=[], plays=[], text=[], scans=[])
        self.recording = True
        try:
            returned = world.call(USE, (user, case.get('with', -1) & 0xffffffff), this=item,
                                  instruction_limit=2_000_000_000)
        finally:
            self.recording = False

        if owner:
            present = world.call(GET_INVENTORY, (slot,), this=owner) == item
        else:
            present = True                                   # nothing removes an item on the map
        amount = s32(world.call(vm.u32(vm.u32(item) + VT_AMOUNT), this=item)) if present else None
        return dict(schema=SCHEMA, case=case.get('name'), returned=returned, **self.log,
                    item=dict(present=present, amount=amount))


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('executable', type=Path)
    parser.add_argument('--case', type=Path)
    parser.add_argument('--serve', action='store_true')
    args = parser.parse_args()
    fixture = FoodUseFixture(args.executable)
    if args.serve:
        serve(fixture.run, fixture.world.sha, SCHEMA, fixture.setup_ms)
        return
    case = json.loads(args.case.read_text()) if args.case else dict(
        name='bread', item=dict(type='Bread'), user=dict(health=50, maxhealth=500, mana=50, maxmana=500))
    print(json.dumps(fixture.run(case), indent=2))


if __name__ == '__main__':
    main()
