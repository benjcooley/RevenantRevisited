#!/usr/bin/env python3
"""Combat A/B, retail side, kata D1s: spell.def as retail parses it.

Runs TSpellList::Load `0x53ead0` (ecx = the list `0x667c38`, built by its
constructor `0x4883a0` before the checkpoint) over the case's spell.def as
original code: the tokenizer, Parse, SSpellData::Load `0x53e4e0` and
LoadControlData `0x53dd10`. Dumps every spell, variant and CONTROLDATA
block by field name (spell_guest.py). Schema `combat.spelldata.v1`, shared
with the port's `Revenant --retail-ab=spell-data`.

Seams (spell_guest.SpellFiles): the file layer under TToken::Open (the
case's bytes for any `...spell.def`), FatalError (the case stops with the
message), and the imagery registry lookup `0x446aa0` an IMAGERY tag makes
(the dump names the file asked for; the registry's state is the game's,
not the parse's).

A case: {"file": <path of a spell.def>}.
"""
from __future__ import annotations

import argparse
import json
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path[:0] = [str(HERE), str(HERE.parents[1])]
from fixturekit import Boundaries, serve, start  # noqa: E402
from guest import _watch_faults, call  # noqa: E402
from spell_guest import Fatal, SpellFiles, construct_list, load_spells, spell_list_dump  # noqa: E402

SCHEMA = 'combat.spelldata.v1'


class SpellDataFixture:
    def __init__(self, executable):
        started = time.perf_counter()
        self.vm, self.sha = start(executable)
        _watch_faults(self.vm)
        construct_list(self.vm, call)
        self.files = SpellFiles(self.vm, Boundaries(self.vm))
        self.vm.checkpoint()
        self.setup_ms = (time.perf_counter() - started) * 1000

    def run(self, case):
        vm = self.vm
        vm.restore()
        self.files.seams = []
        fatal, loaded = None, None
        try:
            loaded = load_spells(vm, call, self.files, Path(case['file']).read_bytes())
        except Fatal as stop:
            fatal = str(stop)
        out = dict(schema=SCHEMA, side='retail', fatal=fatal, loaded=loaded)
        if fatal is None:
            out['spells'] = spell_list_dump(vm, self.files)
        return out


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable')
    parser.add_argument('--serve', action='store_true')
    parser.add_argument('--case', type=Path)
    args = parser.parse_args()
    fixture = SpellDataFixture(args.executable)
    if args.serve:
        serve(fixture.sha, SCHEMA, fixture.setup_ms, fixture.run)
    elif args.case:
        print(json.dumps(fixture.run(json.loads(args.case.read_text())), indent=1))
    else:
        parser.error('--serve or --case')


if __name__ == '__main__':
    main()
