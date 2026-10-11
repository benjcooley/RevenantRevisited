#!/usr/bin/env python3
"""Combat A/B, retail side, kata S4d: a spell's fireball, tick by tick.

A FireBall (object vtable `0x5b408c`, 0x198 bytes) carrying the case's
spell (built as spell_damage.py builds one) is initialized by FireBall's
Init `0x510bd0` and then pulsed `ticks` times by its Pulse `0x510c10`
(thiscall, ret 0): the missile's step (`0x510220`: the base Move `0x470920`
as original over the case's `ground`, the launch, the fly countdown and its
hit test over guest.py's map iterator, the explosion), then the blast
(AreaDamage `0x4de3c0`, original, its seams spell_damage.py's). A record
per tick: state, position, velocity, accumulator, life, the flags that
matter (immobile, moving, weightless, kill), the blast's armed flag
(`+0x194`), the seams and draws of that tick.

`fireball`: {pos, level (0), animator (true): the animator's ball offset
`+0x4ac..` is 0}. The FireBall's SetState / SetPos (slots 0x18 / 8) store
the state and the position without a record (the port's missile keeps both
itself); its animator is a stand-in (no calls reach it in these ticks).

Seams beyond spell_damage.py's: TEffect::Pulse `0x4de800` (the generic
effect's lights, attachments and RANGEDAMAGE: left out, not recorded);
KillThisEffect `0x4defe0` as the port has it (the kill and pulse flags; its
poison cure and the spell's Kill belong to the spell's lifetime, not
ported), not recorded.

Schema `combat.spell.v1`, shared with the port's `Revenant
--retail-ab=missile-fireball`.
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path[:0] = [str(HERE), str(HERE.parents[1])]
from guest import SET_POS, SET_STATE, call, s32, serve  # noqa: E402
from spell_cast import SCHEMA  # noqa: E402
from spell_damage import DamageFixture  # noqa: E402

FIREBALL_VTABLE, FIREBALL_SIZE = 0x5b408c, 0x198
FIREBALL_INIT, FIREBALL_PULSE = 0x510bd0, 0x510c10
EFFECT_PULSE, KILL_EFFECT = 0x4de800, 0x4defe0
O_CLASS, O_FLAGS, O_STATE, O_LEVEL, O_POS, O_VEL, O_ACCUM = 0x04, 0x08, 0x0c, 0x0e, 0x10, 0x1c, 0x28
O_NAME, O_IMAGERY, O_ANIMATOR, O_SPELL, O_LIFE, O_ARMED = 0x38, 0x54, 0x58, 0xd8, 0x184, 0x194
O_INVENTNUM = 0x7c                               # IsInInventory (slot 0xb4) is inventnum >= 0
ANIMATOR_SIZE = 0x500                            # the ball offset is at +0x4ac..+0x4b4
FLAGS = {0x1: 'immobile', 0x8: 'moving', 0x1000: 'kill', 0x8000: 'pulse', 0x10000: 'weightless'}
FIREBALL_STATES = ['launch', 'fly', 'explode']


class FireBallFixture(DamageFixture):
    def __init__(self, executable):
        super().__init__(executable)
        vm = self.vm
        vm.restore()
        b = self.world.boundaries
        b.add(EFFECT_PULSE, 'TEffect::Pulse', 0, lambda args, ecx: 0)
        b.add(KILL_EFFECT, 'KillThisEffect', 0, self._kill)
        # The fireball's own SetState / SetPos: stored, unrecorded.
        for address, store in ((SET_STATE, self._set_state), (SET_POS, self._set_pos)):
            name, pop, handler = b.handlers[address]
            b.handlers[address] = (name, pop, (lambda store, handler: lambda args, ecx:
                                               store(args, ecx) if ecx == self.fireball else handler(args, ecx))(store, handler))
        self.fireball = 0
        vm.checkpoint()

    def _set_state(self, args, ecx):
        self.vm.write(ecx + O_STATE, struct.pack('<h', s32(args[0])))
        return 1

    def _set_pos(self, args, ecx):
        self.vm.write(ecx + O_POS, bytes(self.vm.uc.mem_read(args[0], 12)))
        return 1

    def _kill(self, args, ecx):
        self.vm.put_u32(ecx + O_FLAGS, self.vm.u32(ecx + O_FLAGS) | 0x1000 | 0x8000)
        return 0

    def build_fireball(self, case, spell):
        vm, w = self.vm, self.world
        spec = case.get('fireball', {})
        fb = vm.allocate(FIREBALL_SIZE)
        vm.put_u32(fb, FIREBALL_VTABLE)
        vm.write(fb + O_CLASS, struct.pack('<h', 0x19))
        vm.put_u32(fb + O_FLAGS, 0x1 | 0x8000)           # TEffect's: immobile, pulse
        vm.write(fb + O_LEVEL, struct.pack('<H', spec.get('level', 0)))
        vm.write(fb + O_POS, struct.pack('<3i', *spec.get('pos', (1000, 1000, 60))))
        vm.write(fb + O_INVENTNUM, struct.pack('<h', -1))     # on the map, not in an inventory
        text = vm.allocate(16)
        vm.write(text, b'fireball\0')
        vm.put_u32(fb + O_NAME, text)
        table = [dict(name=n, frames=10, aniflags=0) for n in FIREBALL_STATES]
        w.states[fb] = table
        vm.put_u32(fb + O_IMAGERY, w._new_imagery(fb, table))
        if spec.get('animator', True):
            animator = vm.allocate(ANIMATOR_SIZE)
            vm.put_u32(animator, w.animator_vtable)
            w.animators[animator] = fb
            vm.put_u32(fb + O_ANIMATOR, animator)
        vm.put_u32(fb + O_SPELL, spell if case.get('spell', True) else 0)
        w.objects[fb] = 'fireball'
        self.fireball = fb
        call(vm, FIREBALL_INIT, (), this=fb)
        return fb

    def tick_dump(self, fb):
        vm = self.vm
        flags = vm.u32(fb + O_FLAGS)
        return dict(state=struct.unpack('<h', vm.uc.mem_read(fb + O_STATE, 2))[0],
                    pos=list(struct.unpack('<3i', vm.uc.mem_read(fb + O_POS, 12))),
                    vel=list(struct.unpack('<3i', vm.uc.mem_read(fb + O_VEL, 12))),
                    accum=list(struct.unpack('<3i', vm.uc.mem_read(fb + O_ACCUM, 12))),
                    life=s32(vm.u32(fb + O_LIFE)), armed=vm.u32(fb + O_ARMED),
                    flags=sorted(n for bit, n in FLAGS.items() if flags & bit))

    def run(self, case):
        vm, w = self.vm, self.world
        self.fireball = 0
        self.build_world(case)
        spell = self.build_spell(case)
        w.seams.clear()
        w.draws.clear()
        fb = self.build_fireball(case, spell)
        ticks = []
        for t in range(case.get('ticks', 1)):
            w.seams.clear()
            w.draws.clear()
            call(vm, FIREBALL_PULSE, (), this=fb)
            ticks.append(dict(tick=t, **self.tick_dump(fb), seams=list(w.seams), draws=list(w.draws)))
        names = [spec['name'] for spec in case['chars']]
        return dict(schema=SCHEMA, side='retail', ticks=ticks,
                    casters={n: self.caster_dump(w.by_name[n]) for n in names})


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable')
    parser.add_argument('--serve', action='store_true')
    parser.add_argument('--case', type=Path)
    args = parser.parse_args()
    fixture = FireBallFixture(args.executable)
    if args.serve:
        serve(fixture.sha, SCHEMA, fixture.setup_ms, fixture.run)
    elif args.case:
        print(json.dumps(fixture.run(json.loads(args.case.read_text())), indent=1))
    else:
        parser.error('--serve or --case')


if __name__ == '__main__':
    main()
