#!/usr/bin/env python3
"""Combat A/B, retail side, kata S3: a spell's construction and its damage.

The case's `call`:

- `spell-new`: a spell of the case's `class` ("Spell" or "Strike") built by
  that class's registered creator (`0x542200` / `0x542290`, thiscall
  (invoker, targets, numtargs, sourcepos, spelldata, variant, master), ret
  0x1c) -- the TSpell constructor `0x53f090`: the targets, the poison roll
  per target, a player invoker's STATLINE step. The spell is dumped.
- `spell-damage`: the same spell built first (its records and draws
  dropped), then TSpell::Damage `0x53f560` (thiscall (target), ret 4) on
  the case's `target` (a name, or null).

`variant` names the spell and variant ([spell, variant], retail's own list
loaded from resources.rvr); `invoker`, `targets`, `numtargs`, `sourcepos`
as spell_cast.py. A character's `magicresist` (per mille: the float at
+0x190, the magic resistance spells and effects set) and a
player's `resists` (its modified copy, which Resist slot 0x2c8 reads) come
from its spec.

Seams beyond spell_cast.py's: TCharacter::Damage `0x4c4950` (slot 0x228):
recorded as the melee kata records it -- {who (the victim), damage, type,
mod, attacker, block} -- and not run; TPlayer AwardKillExp `0x51a630` (slot
0x414): recorded, not run.

Schema `combat.spell.v1`, shared with the port's `Revenant
--retail-ab=spell-damage`.
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path[:0] = [str(HERE), str(HERE.parents[1])]
from guest import call, s32, serve  # noqa: E402
from spell_cast import SCHEMA, CastFixture  # noqa: E402
from spell_guest import SD_VARIANTS, SPELL_LIST, items, spells  # noqa: E402

CREATORS = {'Spell': (0x670630, 0x542200), 'Strike': (0x670628, 0x542290)}
SPELL_DAMAGE = 0x53f560
DAMAGE, AWARD_KILL_EXP = 0x4c4950, 0x51a630
O_MAGICRESIST = 0x190
P_MODCOUNT, P_MODSTATS, RESIST_FIRST = 0x34c, 0x350, 6


class DamageFixture(CastFixture):
    def __init__(self, executable):
        super().__init__(executable)
        vm = self.vm
        vm.restore()
        b = self.world.boundaries
        b.add(DAMAGE, 'Damage', 0x14, self._damage)
        b.add(AWARD_KILL_EXP, 'AwardKillExp', 4, self._kill_exp)
        self.by_names = {}
        for sd in spells(vm):
            for v in items(vm, sd + SD_VARIANTS):
                self.by_names.setdefault(tuple(self.by_variant[v]), (sd, v))
        vm.checkpoint()

    # -- seams --------------------------------------------------------------
    def _damage(self, args, ecx):
        w = self.world
        if args[3]:
            raise ValueError('Damage with an action block: not modelled here')
        w.seams.append(dict(seam='Damage', who=w._name(ecx), damage=s32(args[0]), type=s32(args[1]),
                            mod=s32(args[2]), attacker=w._name(args[4]) if args[4] else None, block=None))
        return 0

    def _kill_exp(self, args, ecx):
        w = self.world
        w.seams.append(dict(seam='AwardKillExp', who=w._name(ecx), victim=w._name(args[0]) if args[0] else None))
        return 0

    # -- the world ----------------------------------------------------------
    def _resists(self, obj, resists):
        vm = self.vm
        count = RESIST_FIRST + 10
        pairs = vm.allocate(8 * count)
        for i in range(count):
            value = resists[i - RESIST_FIRST] if i >= RESIST_FIRST else 0
            vm.write(pairs + 8 * i, struct.pack('<ii', i, value))
        vm.write(obj + P_MODCOUNT, struct.pack('<h', count))
        vm.put_u32(obj + P_MODSTATS, pairs)

    def build(self, case):
        """The world and the spell, as spell_cast.py builds a case's world."""
        vm, w = self.vm, self.world
        vm.restore()
        w.reset()
        self.case = case
        g = case.get('globals', {})
        w.set_globals(g)
        w.set_rng(case)
        self.specs = {}
        for spec in case['chars']:
            obj = w.new_character(spec)
            self.specs[spec['name']] = spec
            if spec.get('magicresist') is not None:         # per mille, as a float
                vm.write(obj + O_MAGICRESIST, struct.pack('<f', spec['magicresist'] / 1000.0))
            if spec.get('class') == 11:
                self._resists(obj, spec.get('resists', [0] * 10))
        for spec in case['chars']:
            w.set_blocks(w.by_name[spec['name']], spec)
        w.seams.clear()
        names = case.get('targets')
        targets = 0
        if names is not None:
            targets = vm.allocate(4 * max(1, len(names)))
            for i, n in enumerate(names):
                vm.put_u32(targets + 4 * i, w.by_name[n] if n else 0)
        numtargs = case.get('numtargs', len(names) if names is not None else 0)
        source = 0
        if case.get('sourcepos') is not None:
            source = vm.allocate(12)
            vm.write(source, struct.pack('<3i', *case['sourcepos']))
        sd, v = self.by_names[tuple(case['variant'])]
        invoker = w.by_name[case['invoker']] if case.get('invoker') else 0
        builder, create = CREATORS[case.get('class', 'Spell')]
        spell = s32(call(vm, create, (invoker, targets, numtargs & 0xffffffff, source, sd, v, 0), this=builder))
        return spell & 0xffffffff

    def run(self, case):
        vm, w = self.vm, self.world
        spell = self.build(case)
        kind = case['call']
        if kind == 'spell-damage':
            w.seams.clear()
            w.draws.clear()
            target = w.by_name[case['target']] if case.get('target') else 0
            call(vm, SPELL_DAMAGE, (target,), this=spell)
        elif kind != 'spell-new':
            raise ValueError(f'unknown call {kind!r}')
        names = [spec['name'] for spec in case['chars']]
        return dict(schema=SCHEMA, side='retail', spell=self.spell_dump(spell),
                    casters={n: self.caster_dump(w.by_name[n]) for n in names},
                    seams=list(w.seams), draws=list(w.draws))


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable')
    parser.add_argument('--serve', action='store_true')
    parser.add_argument('--case', type=Path)
    args = parser.parse_args()
    fixture = DamageFixture(args.executable)
    if args.serve:
        serve(fixture.sha, SCHEMA, fixture.setup_ms, fixture.run)
    elif args.case:
        print(json.dumps(fixture.run(json.loads(args.case.read_text())), indent=1))
    else:
        parser.error('--serve or --case')


if __name__ == '__main__':
    main()
