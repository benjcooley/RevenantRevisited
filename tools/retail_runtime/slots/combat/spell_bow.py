#!/usr/bin/env python3
"""Combat A/B, retail side, kata S4c: the bow.

The case's `call`, on the case's `self`:

- `draw-bow`: DrawBow `0x4d0aa0` (thiscall, ret 0);
- `aim-bow`: AimBow `0x4d0c70` (thiscall (angle), ret 4); `aim-left` /
  `aim-right`: `0x4d0dc0` / `0x4d0de0` (by chardata's bow aim speed);
- `shoot-bow`: ShootBow `0x4d0e00` (thiscall (angle), ret 4);
- `is-bow-drawn`: `0x4d1050`;
- `resolve-bow-aim` / `resolve-bow-shoot`: ResolveBowAim `0x4c80c0` /
  ResolveBowShoot `0x4c8130` (thiscall (ab, bits), ret 8) on the doing
  block.

A character's `ammo`: [{name, amount, equipped, type (1)}], guest items in
its inventory (the walk FindObjInventory makes runs as original); the
equipped one is the ammo slot (`+0x2c0`). `lastbowshot` / `bowshots` are
`+0x22c` / `+0x230`; chardata `arrowpos` / `arrowspeed` / `bowwait` /
`bowaimspeed` its `+0x1fc..+0x210`. `arrow`: the object NewObject's id
(`newobject`) gives, SetShooter `0x4c00b0` run on it as original.

Original code: the methods, StName `0x4dadd0`, AngleDiff, AdvanceAngles,
ConvertToVector / the vector angle and length (`0x46dbe0` / `0x46de10`),
Stop, SetDesired, the TActionBlock ctor, FindObjInventory `0x4703c0` and
the inventory walk, the game frame, the network checks (a single-player
game: they do nothing).

Seams beyond spell_arrow.py's (each recorded): TPlayer::Equip `0x5199b0`
{who, item, slot}; DeleteFromInventory (slot 0x78, `0x477950`) {who,
name, count}; the map add `0x451090` after NewObject (not recorded: retail's
NewObject has added the object, so it returns at once).

Schema `combat.spell.v1`, shared with the port's `Revenant
--retail-ab=missile-bow`.
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path[:0] = [str(HERE), str(HERE.parents[1])]
from guest import O_DOING, call, s32, serve  # noqa: E402
from spell_arrow import ArrowFixture  # noqa: E402
from spell_cast import SCHEMA  # noqa: E402

CALLS = {'draw-bow': (0x4d0aa0, None), 'aim-bow': (0x4d0c70, 'angle'), 'aim-left': (0x4d0dc0, None),
         'aim-right': (0x4d0de0, None), 'shoot-bow': (0x4d0e00, 'angle'), 'is-bow-drawn': (0x4d1050, None)}
RESOLVERS = {'resolve-bow-aim': 0x4c80c0, 'resolve-bow-shoot': 0x4c8130}
EQUIP, DELETE_FROM_INVENTORY, ADD_OBJECT = 0x5199b0, 0x477950, 0x451090
NO_REDIRECT = 0x477d50                           # TObjectInstance slot 0x170: xor eax, eax; ret
O_INV_COUNT, O_INV_ITEMS, O_CONTAINER, O_INVINDEX, O_NAME = 0x68, 0x78, 0x64, 0x7e, 0x38
O_LASTBOWSHOT, O_BOWSHOTS, O_AMMO, O_SHOOTER = 0x22c, 0x230, 0x2c0, 0xd8
CD_ARROWPOS, CD_ARROWSPEED, CD_BOWWAIT, CD_BOWAIMSPEED = 0x1fc, 0x208, 0x20c, 0x210
ITEM_SIZE, ITEM_SLOTS = 0x100, 0x100


class BowFixture(ArrowFixture):
    def __init__(self, executable):
        super().__init__(executable)
        vm = self.vm
        vm.restore()
        b = self.world.boundaries
        b.add(EQUIP, 'Equip', 8, self._equip)
        b.add(DELETE_FROM_INVENTORY, 'DeleteFromInventory', 8, self._delete)
        b.add(ADD_OBJECT, 'AddObject', 4, lambda args, ecx: 0xffffffff)
        self.ammo_vtable = self._ammo_vtable(b)
        vm.checkpoint()

    def _ammo_vtable(self, b):
        """An ammunition item's vtable: the redirect slot 0x170 as the base
        class's, GetStat by name (0xd4) and Amount (0x198) answered from the
        case, unrecorded (they are data); any other slot fails the case."""
        vm = self.vm

        def unmodelled(args, ecx):
            raise RuntimeError(f'ammo slot reached for {self.world._name(ecx)} (not modelled)')

        def stat(args, ecx):
            name = vm.string(args[0]).lower()
            spec = self.ammo[ecx]
            return spec.get(name, 1 if name == 'type' else 0)

        fail = b.add_stub('ammo.unmodelled', 0, unmodelled)
        vtable = vm.allocate(ITEM_SLOTS * 4)
        for i in range(ITEM_SLOTS):
            vm.put_u32(vtable + 4 * i, fail)
        vm.put_u32(vtable + 0x170, NO_REDIRECT)
        vm.put_u32(vtable + 0xd4, b.add_stub('ammo.GetStat', 4, stat))
        vm.put_u32(vtable + 0x198, b.add_stub('ammo.Amount', 0, lambda args, ecx: self.ammo[ecx].get('amount', 1)))
        return vtable

    # -- seams --------------------------------------------------------------
    def _equip(self, args, ecx):
        w = self.world
        w.seams.append(dict(seam='Equip', who=w._name(ecx), item=w._name(args[0]) if args[0] else None,
                            slot=s32(args[1])))
        return 1

    def _delete(self, args, ecx):
        w = self.world
        w.seams.append(dict(seam='DeleteFromInventory', who=w._name(ecx), name=self.vm.string(args[0]),
                            count=s32(args[1])))
        return 1

    # -- the world ----------------------------------------------------------
    def _inventory(self, owner, items):
        vm, w = self.vm, self.world
        array = vm.allocate(4 * max(1, len(items)))
        for k, spec in enumerate(items):
            item = vm.allocate(ITEM_SIZE)
            vm.put_u32(item, self.ammo_vtable)
            vm.write(item + 0x04, struct.pack('<h', 0x15))
            text = vm.allocate(len(spec['name']) + 1)
            vm.write(text, spec['name'].encode('cp1252') + b'\0')
            vm.put_u32(item + O_NAME, text)
            vm.put_u32(item + O_CONTAINER, owner)
            vm.write(item + O_INVINDEX, struct.pack('<h', k))
            w.objects[item] = spec['name']
            self.ammo[item] = spec
            vm.put_u32(array + 4 * k, item)
            if spec.get('equipped'):
                vm.put_u32(owner + O_AMMO, item)
        vm.put_u32(owner + O_INV_COUNT, len(items))
        vm.put_u32(owner + O_INV_ITEMS, array)

    def run(self, case):
        vm, w = self.vm, self.world
        self.build_world(case)
        self.casts = []
        self.types = []
        self.items = {}
        self.ammo = {}
        self.ids = {spec['name']: spec.get('id', 0) for spec in case['chars']}
        self.by_id = {spec.get('id', 0): w.by_name[spec['name']] for spec in case['chars']}
        for spec in case['chars']:
            obj = w.by_name[spec['name']]
            self.spells.set_records(obj, spec)
            vm.put_u32(obj + O_LASTBOWSHOT, spec.get('lastbowshot', 0) & 0xffffffff)
            vm.put_u32(obj + O_BOWSHOTS, spec.get('bowshots', 0) & 0xffffffff)
            cd = vm.u32(obj + 0xfc)
            chardata = spec.get('chardata', {})
            vm.write(cd + CD_ARROWPOS, struct.pack('<3i', *chardata.get('arrowpos', (-1, -1, -1))))
            for key, off, default in (('arrowspeed', CD_ARROWSPEED, 20), ('bowwait', CD_BOWWAIT, 12),
                                      ('bowaimspeed', CD_BOWAIMSPEED, 8)):
                vm.put_u32(cd + off, chardata.get(key, default) & 0xffffffff)
            if spec.get('ammo') is not None:
                self._inventory(obj, spec['ammo'])
        arrow = self.build_arrow(case) if case.get('arrow') else 0
        if not case.get('mainplayer', True):
            vm.put_u32(0x667fcc, 0)
        w.seams.clear()
        me = w.by_name[case['self']]
        kind = case['call']
        if kind in CALLS:
            entry, arg = CALLS[kind]
            returned = s32(call(vm, entry, (case[arg] & 0xffffffff,) if arg else (), this=me))
        elif kind in RESOLVERS:
            returned = s32(call(vm, RESOLVERS[kind], (vm.u32(me + O_DOING), case.get('bits', 0)), this=me))
        else:
            raise ValueError(f'unknown call {kind!r}')
        out = dict(schema=SCHEMA, side='retail', returned=returned, self=w.character_dump(me, []),
                   motion=w.motion_dump(me),
                   bow=dict(lastbowshot=s32(vm.u32(me + O_LASTBOWSHOT)), bowshots=s32(vm.u32(me + O_BOWSHOTS))),
                   seams=list(w.seams), draws=list(w.draws))
        if arrow:
            sid = s32(vm.u32(arrow + O_SHOOTER))
            names = {v: k for k, v in self.ids.items()}
            out['arrow'] = dict(shooter=names.get(sid) if sid != -1 else None)
        return out


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable')
    parser.add_argument('--serve', action='store_true')
    parser.add_argument('--case', type=Path)
    args = parser.parse_args()
    fixture = BowFixture(args.executable)
    if args.serve:
        serve(fixture.sha, SCHEMA, fixture.setup_ms, fixture.run)
    elif args.case:
        print(json.dumps(fixture.run(json.loads(args.case.read_text())), indent=1))
    else:
        parser.error('--serve or --case')


if __name__ == '__main__':
    main()
