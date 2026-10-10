#!/usr/bin/env python3
"""Combat A/B, retail side, kata S1: talismans to spell.

The case's `call`:

- `lookup`: the spell list's lookups over the case's `queries` ({"by":
  "talismans" | "name", "q"}), each answered by both of its kind:
  GetSpellDataByTalismans `0x53ed70` and GetVariantDataByTalismans
  `0x53ef90`, or GetSpellDataByName `0x53ede0` and GetVariantDataByName
  `0x53f010` (thiscall (text), ret 4, on the list `0x667c38`).
- `has-talismans`: TPlayer::HasTalismans `0x51b7c0` (thiscall (text), ret 4)
  for each of the case's `queries` (talisman strings), on the player whose
  inventory the case gives (`inventory` on the character: FindInventory
  slot 0xa8 `0x470280` and the inventory walk `0x46dfb0` run as original
  code), the TALISMAN class built from class.def's.
- `quick-spell`: TPlayer::InvokeQuickSpell `0x51b5d0` (thiscall (button),
  ret 4) with the case's `quickspells` (5 talisman strings at +0x2cc).

Original code: the lookups (over retail's own list, loaded at setup from
resources.rvr's spell.def by TSpellList::Load), HasTalismans, FindInventory,
the walk, the class's FindStat `0x474210`, the string table `0x49d800` (an
empty table answers "[TAG]"), InvokeQuickSpell's gates.

Seams (recorded in `seams`, in order; guest.py for the shared ones):
- GetObjStat (Health) and the rest of guest.py's;
- the text bar `0x54d170`: the message;
- TCharacter::CastByTalismans `0x4d5c20` / CastByName `0x4d5b90` (thiscall
  (text, targets, numtargs, sourcepos), ret 0x10): the call, answered from
  the case's `casts` in order (default 1). Their insides are kata S2's.

Schema `combat.spell.v1`, shared with the port's `Revenant
--retail-ab=spell-lookup` / `spell-talismans` / `spell-quick`.
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path[:0] = [str(HERE), str(HERE.parents[1])]
from guest import CombatWorld, call, s32, serve, start  # noqa: E402
from spell_guest import (SPELL_LIST, TALISMAN_CLASS, SpellFiles, SpellWorld, build_class, class_section,  # noqa: E402
                         construct_list, load_spells, shipped_class_def, shipped_spell_def, variant_names)

SCHEMA = 'combat.spell.v1'
SPELL_BY_TALISMANS, VARIANT_BY_TALISMANS = 0x53ed70, 0x53ef90
SPELL_BY_NAME, VARIANT_BY_NAME = 0x53ede0, 0x53f010
HAS_TALISMANS, INVOKE_QUICK_SPELL = 0x51b7c0, 0x51b5d0
CAST_BY_TALISMANS, CAST_BY_NAME = 0x4d5c20, 0x4d5b90
QUICKSPELLS, QUICKSPELL_SIZE = 0x2cc, 6
G_PLAYER = 0x667fcc


class TalismanFixture:
    def __init__(self, executable):
        started = time.perf_counter()
        self.vm, self.sha = start(executable)
        self.world = CombatWorld(self.vm)
        self.spells = SpellWorld(self.world)
        b = self.world.boundaries
        construct_list(self.vm, call)
        load_spells(self.vm, call, SpellFiles(self.vm, b), shipped_spell_def())
        stats, types = class_section(shipped_class_def(), 'TALISMAN')
        build_class(self.vm, TALISMAN_CLASS, stats, types)
        b.add(CAST_BY_TALISMANS, 'CastByTalismans', 0x10, lambda a, c: self._cast('CastByTalismans', a, c))
        b.add(CAST_BY_NAME, 'CastByName', 0x10, lambda a, c: self._cast('CastByName', a, c))
        self.by_variant, self.by_spell = variant_names(self.vm)
        self.text = self.vm.allocate(256)
        self.vm.checkpoint()
        self.setup_ms = (time.perf_counter() - started) * 1000
        self.case = {}
        self.casts = []

    # -- seams --------------------------------------------------------------
    def _cast(self, seam, args, ecx):
        vm, w = self.vm, self.world
        text, targets, numtargs, source = args[:4]
        names = None
        if targets:
            names = [w._name(vm.u32(targets + 4 * i)) for i in range(max(1, s32(numtargs)))]
        src = list(struct.unpack('<3i', vm.uc.mem_read(source, 12))) if source else None
        result = self.casts.pop(0) if self.casts else 1
        w.seams.append(dict(seam=seam, who=w._name(ecx), text=vm.string(text), targets=names,
                            numtargs=s32(numtargs), source=src, result=result))
        return result

    # -- one case -----------------------------------------------------------
    def put_text(self, text):
        self.vm.write(self.text, text.encode('cp1252') + b'\0')
        return self.text

    def lookup(self, query):
        vm = self.vm
        text = self.put_text(query['q'])
        if query['by'] == 'talismans':
            sd, v = (call(vm, SPELL_BY_TALISMANS, (text,), this=SPELL_LIST),
                     call(vm, VARIANT_BY_TALISMANS, (text,), this=SPELL_LIST))
        else:
            sd, v = (call(vm, SPELL_BY_NAME, (text,), this=SPELL_LIST),
                     call(vm, VARIANT_BY_NAME, (text,), this=SPELL_LIST))
        return dict(spell=self.by_spell.get(sd) if sd else None,
                    variant=list(self.by_variant[v]) if v else None)

    def run(self, case):
        vm, w = self.vm, self.world
        vm.restore()
        w.reset()
        self.case = case
        self.casts = list(case.get('casts', []))
        kind = case['call']
        if kind == 'lookup':
            return dict(schema=SCHEMA, side='retail', returned=[self.lookup(q) for q in case['queries']])
        w.set_globals(case.get('globals', {}))
        w.set_rng(case)
        for spec in case['chars']:
            obj = w.new_character(spec)
            if spec.get('inventory') is not None:
                self.spells.new_inventory(obj, spec['inventory'])
            for i, tal in enumerate(spec.get('quickspells', [])):
                vm.write(obj + QUICKSPELLS + QUICKSPELL_SIZE * i, tal.encode('cp1252')[:5].ljust(6, b'\0'))
        for spec in case['chars']:
            w.set_blocks(w.by_name[spec['name']], spec)
            self.spells.set_records(w.by_name[spec['name']], spec)
        if not case.get('mainplayer', True):
            vm.put_u32(G_PLAYER, 0)
        w.seams.clear()
        me = w.by_name[case['self']]
        if kind == 'has-talismans':
            returned = [s32(call(vm, HAS_TALISMANS, (self.put_text(q),), this=me)) for q in case['queries']]
        elif kind == 'quick-spell':
            returned = s32(call(vm, INVOKE_QUICK_SPELL, (case['button'] & 0xffffffff,), this=me))
        else:
            raise ValueError(f'unknown call {kind!r}')
        return dict(schema=SCHEMA, side='retail', returned=returned, seams=list(w.seams), draws=list(w.draws))


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable')
    parser.add_argument('--serve', action='store_true')
    parser.add_argument('--case', type=Path)
    args = parser.parse_args()
    fixture = TalismanFixture(args.executable)
    if args.serve:
        serve(fixture.sha, SCHEMA, fixture.setup_ms, fixture.run)
    elif args.case:
        print(json.dumps(fixture.run(json.loads(args.case.read_text())), indent=1))
    else:
        parser.error('--serve or --case')


if __name__ == '__main__':
    main()
