#!/usr/bin/env python3
"""Combat A/B, retail side, kata S4b: an arrow's hit.

The case's `call`, on the case's `arrow` (a TAmmo, real vtable `0x5a6b68`):

- `arrow-move`: TAmmo::Move `0x4c01f0` (thiscall, ret 0) -- the base move
  (a seam), then a flying arrow's hit: the character there (CharBlocking
  `0x4d4db0`, run as original over guest.py's map iterator), the ice burst,
  the impact sound, the shooter's enmity, the damage (AMMODATA), the poison /
  fire proc, Damage, KnockBack, the main player's message, OnAttacked, the
  kill flag;
- `arrow-pulse`: TAmmo::Pulse `0x4c0880` (thiscall, ret 0).

`arrow`: {type (its objinfo name), magictype, damagemod, pos, vel, flags
(retail's, `0x10000` weightless), killwait (-1), shooter (a name or null),
owner (a name: the arrow is in that inventory), id}. A character's `type`
names its objinfo (the ice burst skips a Solifuge); a player's `bow`:
{name, damagemod} is its ranged weapon (`+0x2bc`).

Seams beyond spell_damage.py's (each recorded):
- the base move TObjectInstance::Move `0x470920`: {who}, the case's `bits`;
- the arrow's MagicType (slot 0x200, `0x4c0d40`) and DamageMod (slot 0x208,
  `0x4c0d70`): GetStat {who, stat, result}; the bow's GetStat("damagemod")
  (its slot 0xd4) the same;
- FindObjectsInRange `0x452060` (thiscall (pos, level, array, width,
  height, objclass, maxnum, objset), ret 0x20): the case's `iced` (names;
  their ids go in the array); GetInstance `0x452690` (id -> object, not
  recorded); FindObjType `0x475210` (a name's index here, not recorded);
  NewObject `0x450e40`: the def {class, type (by name), flags, state,
  level, pos, vel, facing}, answered with the case's `newobject` (-1);
- the sound lookup `0x49c430`: {name}, -1 (no sound);
- AwardSkillExp (TPlayer slot 0x41c, `0x51abe0`): {skill, victim};
- TSpellManager::CastByName `0x53f920`: {who (the manager's owner), name,
  invoker, targets, numtargs, source, master}, the case's `casts` in order
  (1 when they run out);
- OnAttacked (slot 0x240, `0x4cdce0`): {who, attacker, victim, flag}.

Globals: AMMODATA (Rules `0x65d884..0x65d894`) from `globals.ammodata`, the
Rules clear's defaults (20, 6, 4, 1, 25) for what it leaves out (the
fixture runs no Rules clear); the editor flag `0x668154` from
`globals.editor`; the main player
`0x667fcc` the case's player (none with `mainplayer: false`). The message
table is the game's (english.def, loaded by `0x49ceb0` at setup).

Schema `combat.spell.v1`, shared with the port's `Revenant
--retail-ab=missile-arrow`.
"""
from __future__ import annotations

import argparse
import json
import os
import struct
import sys
import zipfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path[:0] = [str(HERE), str(HERE.parents[1])]
from guest import call, s32, serve  # noqa: E402
from spell_cast import G_EDITOR, POINTER_ARRAY_CTOR, SCHEMA  # noqa: E402
from spell_damage import DamageFixture  # noqa: E402

AMMO_VTABLE, AMMO_SIZE = 0x5a6b68, 0xe0
AMMO_MOVE, AMMO_PULSE = 0x4c01f0, 0x4c0880
BASE_MOVE = 0x470920
MAGIC_TYPE, DAMAGE_MOD = 0x4c0d40, 0x4c0d70       # TAmmo slots 0x200 / 0x208: GetStat(id)
FIND_OBJECTS, GET_INSTANCE, FIND_OBJ_TYPE, NEW_OBJECT = 0x452060, 0x452690, 0x475210, 0x450e40
FIND_SOUND = 0x49c430
AWARD_SKILL_EXP = 0x51abe0
MANAGER_CAST_BY_NAME = 0x53f920
ON_ATTACKED = 0x4cdce0
O_LEVEL, O_OBJINFO, O_OWNER, O_SHOOTER, O_KILLWAIT = 0x0e, 0x4c, 0x64, 0xd8, 0xdc
O_BOW, O_MANAGER = 0x2bc, 0x170
OF_KILL, OF_WEIGHTLESS = 0x1000, 0x10000
ITEM_SLOTS = 0x100
CLASSES = {0x66cc68: 'EFFECT', 0x66b160: 'AMMO'}
DIALOG_LIST, DIALOG_INIT, G_LANGUAGE = 0x65d4d0, 0x49ceb0, 0x65bc18   # the base table: <ClassDefPath><Language>.def
G_AMMODATA = 0x65d884                            # Rules AMMODATA (+0xdc..+0xec): base, per monster / player level, per bow skill, least %
AMMODATA_DEFAULTS = (20, 6, 4, 1, 25)            # the Rules clear's


def shipped_text(name: str) -> bytes:
    """A file of resources.rvr, by name without case."""
    data = Path(os.environ.get('REVENANT_DATA_PATH', Path.home() / 'RevenantRetailLab' / 'retail-cd' / 'REVENANT'))
    with zipfile.ZipFile(data / 'resources.rvr') as z:
        for n in z.namelist():
            if n.lower() == name:
                return z.read(n)
    raise FileNotFoundError(f'resources.rvr has no {name}')


class ArrowFixture(DamageFixture):
    def __init__(self, executable):
        super().__init__(executable)
        vm = self.vm
        vm.restore()
        b = self.world.boundaries
        b.add(BASE_MOVE, 'Move', 0, self._base_move)
        b.add(MAGIC_TYPE, 'MagicType', 0, lambda args, ecx: self._ammo_stat(ecx, 'magictype'))
        b.add(DAMAGE_MOD, 'DamageMod', 0, lambda args, ecx: self._ammo_stat(ecx, 'damagemod'))
        b.add(FIND_OBJECTS, 'FindObjectsInRange', 0x20, self._find_objects)
        b.add(GET_INSTANCE, 'GetInstance', 8, self._get_instance)
        b.add(FIND_OBJ_TYPE, 'FindObjType', 8, self._find_obj_type)
        b.add(NEW_OBJECT, 'NewObject', 8, self._new_object)
        b.add(FIND_SOUND, 'FindSound', 4, self._find_sound)
        b.add(AWARD_SKILL_EXP, 'AwardSkillExp', 8, self._award_skill_exp)
        b.add(MANAGER_CAST_BY_NAME, 'CastByName', 0x18, self._cast_by_name)
        b.add(ON_ATTACKED, 'OnAttacked', 0xc, self._on_attacked)
        self.item_vtable = self._item_vtable(b)
        # The game's messages (english.def), as the port's DialogList loads them.
        # Its two tables are pointer arrays the static init builds (0x41c7f0).
        for table in (DIALOG_LIST, DIALOG_LIST + 0x14):
            call(vm, POINTER_ARRAY_CTOR, (0x100, 0x100), this=table)
        vm.write(G_LANGUAGE, b'english\0')
        self.spell_files.files['english.def'] = shipped_text('english.def')
        call(vm, DIALOG_INIT, (), this=DIALOG_LIST, instruction_limit=200_000_000)
        vm.checkpoint()

    def _item_vtable(self, b):
        """A ranged weapon's vtable: slot 0xd4 (GetStat by name) answered from
        the case; any other slot fails the case."""
        vm = self.vm

        def unmodelled(args, ecx):
            raise RuntimeError(f'item slot reached for {self.world._name(ecx)} (not modelled)')

        fail = b.add_stub('item.unmodelled', 0, unmodelled)
        vtable = vm.allocate(ITEM_SLOTS * 4)
        for i in range(ITEM_SLOTS):
            vm.put_u32(vtable + 4 * i, fail)
        vm.put_u32(vtable + 0xd4, b.add_stub('item.GetStat', 4, self._item_stat))
        return vtable

    # -- seams --------------------------------------------------------------
    def _base_move(self, args, ecx):
        w = self.world
        w.seams.append(dict(seam='Move', who=w._name(ecx)))
        return self.case.get('bits', 0)

    def _ammo_stat(self, ecx, stat):
        value = self.case['arrow'].get(stat, 0)
        self.world.seams.append(dict(seam='GetStat', who=self.world._name(ecx), stat=stat, result=value))
        return value

    def _item_stat(self, args, ecx):
        name = self.vm.string(args[0]).lower()
        spec = self.items[ecx]
        if name not in spec:
            raise KeyError(f"{spec['name']} has no {name} in the case")
        self.world.seams.append(dict(seam='GetStat', who=spec['name'], stat=name, result=spec[name]))
        return spec[name]

    def _find_objects(self, args, ecx):
        vm, w = self.vm, self.world
        pos = list(struct.unpack('<3i', vm.uc.mem_read(args[0], 12)))
        found = [self.ids[n] for n in self.case.get('iced', [])][:s32(args[6])]
        for i, oid in enumerate(found):
            vm.put_u32(args[2] + 4 * i, oid & 0xffffffff)
        w.seams.append(dict(seam='FindObjectsInRange', pos=pos, width=s32(args[3]), height=s32(args[4]),
                            objclass=s32(args[5]), maxnum=s32(args[6]), objset=s32(args[7]),
                            result=list(self.case.get('iced', []))[:s32(args[6])]))
        return len(found)

    def _get_instance(self, args, ecx):
        return self.by_id.get(s32(args[0]), 0)

    def _find_obj_type(self, args, ecx):
        name = self.vm.string(args[0])
        self.types.append((CLASSES.get(ecx, hex(ecx)), name))
        return len(self.types) - 1

    def _new_object(self, args, ecx):
        vm = self.vm
        d = args[0]
        objclass, objtype = struct.unpack('<hh', vm.uc.mem_read(d, 4))
        state, level = struct.unpack('<HH', vm.uc.mem_read(d + 8, 4))
        record = dict(seam='NewObject', **{'class': objclass},
                      type=self.types[objtype][1] if 0 <= objtype < len(self.types) else objtype,
                      flags=vm.u32(d + 4), state=state, level=level,
                      pos=list(struct.unpack('<3i', vm.uc.mem_read(d + 0x0c, 12))),
                      vel=list(struct.unpack('<3i', vm.uc.mem_read(d + 0x18, 12))),
                      facing=vm.uc.mem_read(d + 0x32, 1)[0])
        self.world.seams.append(record)
        return self.case.get('newobject', -1)

    def _find_sound(self, args, ecx):
        self.world.seams.append(dict(seam='FindSound', name=self.vm.string(args[0])))
        return -1

    def _award_skill_exp(self, args, ecx):
        w = self.world
        w.seams.append(dict(seam='AwardSkillExp', who=w._name(ecx), skill=s32(args[0]),
                            victim=w._name(args[1]) if args[1] else None))
        return 0

    def _cast_by_name(self, args, ecx):
        vm, w = self.vm, self.world
        n = s32(args[3])
        targets = [w._name(vm.u32(args[2] + 4 * i)) for i in range(max(n, 1))] if args[2] else None
        source = list(struct.unpack('<3i', vm.uc.mem_read(args[4], 12))) if args[4] else None
        result = self.casts.pop(0) if self.casts else 1
        w.seams.append(dict(seam='CastByName', who=w._name(ecx - O_MANAGER), name=vm.string(args[0]),
                            invoker=w._name(args[1]) if args[1] else None, targets=targets, numtargs=n,
                            source=source, master=None if not args[5] else hex(args[5]), result=result))
        return result

    def _on_attacked(self, args, ecx):
        w = self.world
        w.seams.append(dict(seam='OnAttacked', who=w._name(ecx), attacker=w._name(args[0]) if args[0] else None,
                            victim=w._name(args[1]) if args[1] else None, flag=s32(args[2])))
        return 0

    # -- the world ----------------------------------------------------------
    def _objinfo(self, name):
        vm = self.vm
        text = vm.allocate(len(name) + 1)
        vm.write(text, name.encode('cp1252') + b'\0')
        info = vm.allocate(0x20)
        vm.put_u32(info, text)
        return info

    def build_arrow(self, case):
        vm, w = self.vm, self.world
        spec = case['arrow']
        arrow = vm.allocate(AMMO_SIZE)
        vm.put_u32(arrow, AMMO_VTABLE)
        vm.write(arrow + 0x04, struct.pack('<h', 0x15))
        vm.put_u32(arrow + 0x08, spec.get('flags', OF_WEIGHTLESS) | 0x8008)
        vm.write(arrow + O_LEVEL, struct.pack('<H', spec.get('level', 0)))
        vm.write(arrow + 0x10, struct.pack('<3i', *spec.get('pos', (0, 0, 0))))
        vm.write(arrow + 0x1c, struct.pack('<3i', *spec.get('vel', (0, 0, 0))))
        vm.put_u32(arrow + 0x40, spec.get('id', 900))
        vm.put_u32(arrow + O_OBJINFO, self._objinfo(spec.get('type', 'Arrow')))
        if spec.get('owner'):
            vm.put_u32(arrow + O_OWNER, w.by_name[spec['owner']])
        shooter = spec.get('shooter')
        vm.put_u32(arrow + O_SHOOTER, (self.ids[shooter] if shooter else -1) & 0xffffffff)
        vm.put_u32(arrow + O_KILLWAIT, spec.get('killwait', -1) & 0xffffffff)
        w.objects[arrow] = spec.get('name', 'arrow')
        self.by_id[spec.get('id', 900)] = arrow
        return arrow

    def run(self, case):
        vm, w = self.vm, self.world
        self.build_world(case)
        vm.put_u32(G_EDITOR, int(case.get('globals', {}).get('editor', 0)))
        given = case.get('globals', {}).get('ammodata', [])
        for i, value in enumerate(list(given) + list(AMMODATA_DEFAULTS[len(given):])):
            vm.put_u32(G_AMMODATA + 4 * i, value & 0xffffffff)
        if not case.get('mainplayer', True):
            vm.put_u32(0x667fcc, 0)
        self.casts = list(case.get('casts', []))
        self.types = []
        self.items = {}
        self.ids = {spec['name']: spec.get('id', 0) for spec in case['chars']}
        self.by_id = {spec.get('id', 0): w.by_name[spec['name']] for spec in case['chars']}
        for spec in case['chars']:
            obj = w.by_name[spec['name']]
            vm.put_u32(obj + O_OBJINFO, self._objinfo(spec.get('type', spec['name'])))
            self.spells.set_records(obj, spec)
            if spec.get('bow'):
                bow = vm.allocate(ITEM_SLOTS)
                vm.put_u32(bow, self.item_vtable)
                w.objects[bow] = spec['bow']['name']
                self.items[bow] = dict(spec['bow'])
                vm.put_u32(obj + O_BOW, bow)
        arrow = self.build_arrow(case)
        w.seams.clear()
        kind = case['call']
        if kind == 'arrow-move':
            returned = call(vm, AMMO_MOVE, (), this=arrow)
        elif kind == 'arrow-pulse':
            returned = call(vm, AMMO_PULSE, (), this=arrow)
        else:
            raise ValueError(f'unknown call {kind!r}')
        names = [spec['name'] for spec in case['chars']]
        out = dict(killed=int(bool(vm.u32(arrow + 0x08) & OF_KILL)), killwait=s32(vm.u32(arrow + O_KILLWAIT)))
        if kind == 'arrow-move':
            out['bits'] = s32(returned)
        return dict(schema=SCHEMA, side='retail', arrow=out,
                    casters={n: self.caster_dump(w.by_name[n]) for n in names},
                    seams=list(w.seams), draws=list(w.draws))


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable')
    parser.add_argument('--serve', action='store_true')
    parser.add_argument('--case', type=Path)
    args = parser.parse_args()
    fixture = ArrowFixture(args.executable)
    if args.serve:
        serve(fixture.sha, SCHEMA, fixture.setup_ms, fixture.run)
    elif args.case:
        print(json.dumps(fixture.run(json.loads(args.case.read_text())), indent=1))
    else:
        parser.error('--serve or --case')


if __name__ == '__main__':
    main()
