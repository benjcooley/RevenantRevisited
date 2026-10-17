#!/usr/bin/env python3
"""Make an arena test slot: Locke and the monsters already in place, saved.

    make_arena_slot.py NAME --locke X,Y [--monster Type@X,Y ...] [--out DIR]
                       [--binary build/Revenant] [--base "Port Resave"]

The arena (make_arena.py) goes into a copy of the base slot; the port loads
it, puts Locke there (`pos`), adds the monsters (`addat`) and saves through
its in-game menu half a second later, before any of them has noticed him.
The slot it writes holds the arena sectors with the monsters in them and
Locke's place in game.sav: retail loads it as it is (the lab's `retail.py
upload` / `load`) and the port does (`arena.py`, scenario "slotdir"), so a
test starts from the same ground in both (docs/gameplay/COMBAT_ARENA.md).
The slot is checked with revsave (state hashes; Locke's place).
"""
from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
import zipfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[1]
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(REPO / 'tools' / 'savefmt'))
sys.path.insert(0, str(REPO / 'tools' / 'walktest'))
from make_arena import FLOOR_Z, add_arena  # noqa: E402
from revsave import Decoder, sector_hash  # noqa: E402
import walktest  # noqa: E402  (the INI with retail paths)

INTEROP = REPO / 'docs' / 'gameflow' / 'SAVE_INTEROP_SLOTS.zip'
DATA = Path(os.environ.get('REVENANT_DATA_PATH', Path.home() / 'RevenantRetailLab' / 'retail-cd' / 'REVENANT'))
ARENA_LEVEL = 2
WORK = 'Arena'                  # the slot name while building (the in-game save keeps the loaded name)


def make(name: str, locke: tuple[int, int], monsters: list[tuple[str, int, int]], out: Path, binary: Path,
         base: str) -> Path:
    work = out / f'.work-{name}'
    if work.exists():
        shutil.rmtree(work)
    slot = work / 'save' / 'Save' / 'Single' / WORK
    slot.mkdir(parents=True)
    with zipfile.ZipFile(INTEROP) as z:
        for member in z.namelist():
            if member.startswith(base + '/') and not member.endswith('/'):
                target = slot / member[len(base) + 1:]
                target.parent.mkdir(parents=True, exist_ok=True)
                target.write_bytes(z.read(member))
    arena = add_arena(slot, module=DATA / 'Modules' / 'Ahkuilon.rvm')
    (work / 'save' / 'Revenant.ini').write_text(walktest.INI)

    lines = ['sleep 24', f'pos {locke[0]} {locke[1]} {FLOOR_Z + 1} {ARENA_LEVEL}', 'sleep 12']
    lines += [f'addat {x} {y} {kind}' for kind, x, y in monsters]
    script = ('wait 2000; key_press esc; wait 400; move 408 175 0; mouse_click; wait 400; key_press enter; '
              'wait 3000; take_snapshot saved')
    exe = binary.parent / f'{binary.name}.arenaslot_{os.getpid()}'
    shutil.copy2(binary, exe)
    try:
        subprocess.run([str(exe), '--headless', '--resolution=640x480', f'--quickstart={WORK}', '--max-runtime=90',
                        '--filmstrip=1,0', f'--snapprefix={work}/s_', f'--exec={"; ".join(lines)}',
                        f'--input-script={script}'], cwd=work, env=dict(os.environ, REVENANT_SAVE_PATH=str(work / 'save')),
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=False)
    finally:
        exe.unlink(missing_ok=True)

    final = out / name
    if final.exists():
        shutil.rmtree(final)
    shutil.copytree(slot, final)
    check(final, locke, arena)
    return final


def check(slot: Path, locke: tuple[int, int], arena: dict) -> None:
    d = Decoder((slot / 'game.sav').read_bytes())
    d.save()
    fields = {f.path: f.value for f in d.fields}
    at = (fields.get('player[0].pos.x'), fields.get('player[0].pos.y'), fields.get('player[0].level'))
    if at != (locke[0], locke[1], ARENA_LEVEL):
        raise SystemExit(f'{slot.name}: Locke saved at {at}, not on the arena -- the save failed')
    for sx, sy in arena['sectors']:
        result = sector_hash(slot / 'CurMap' / f'{ARENA_LEVEL}_{sx}_{sy}.DAT')
        stored, computed, with_player = result
        if stored != computed and stored not in with_player:
            raise SystemExit(f'{slot.name}: sector {sx}_{sy} state hash {stored:#x} != {computed:#x}')


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('name')
    p.add_argument('--locke', required=True, help='X,Y on the arena')
    p.add_argument('--monster', action='append', default=[], help='Type@X,Y (repeatable)')
    p.add_argument('--out', type=Path, default=REPO / 'build' / 'arena_slots')
    p.add_argument('--binary', type=Path, default=REPO / 'build' / 'Revenant')
    p.add_argument('--base', default='Port Resave', help='a slot of docs/gameflow/SAVE_INTEROP_SLOTS.zip')
    a = p.parse_args()
    monsters = []
    for m in a.monster:
        kind, at = m.split('@')
        x, y = (int(v) for v in at.split(','))
        monsters.append((kind, x, y))
    locke = tuple(int(v) for v in a.locke.split(','))
    slot = make(a.name, locke, monsters, a.out.resolve(), a.binary.resolve(), a.base)
    print(f'{slot}: Locke at {locke}, {len(monsters)} monster(s); checked')


if __name__ == '__main__':
    main()
