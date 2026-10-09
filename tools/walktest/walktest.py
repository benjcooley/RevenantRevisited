#!/usr/bin/env python3
"""Walk Locke around a saved game and report where he sticks.

    walktest.py SLOT_DIR [--out DIR] [--binary build/Revenant] [--pattern sweep|walks|both] [--analyse]

Runs the port headless at 640x480 from the save (a copy of the slot folder
under a scratch SavePath), drives the mouse with the input simulator and
traces the player every tick (--combattrace; the player is always traced).

- `sweep`: the right button held while the pointer circles Locke (16
  bearings, 1.5 s each, then the other way round), the way a player walks
  him around a room: along walls, into corners, through doorways.
- `walks`: the button held toward each of eight bearings for 4 s, near
  him and far (walk and run), a snapshot after each. (A left click on the
  bare floor does nothing, in retail too: MAP_INPUT.md §4.3.)

A stretch of 2 s or more in which he doesn't move while doing a move
action (walk, run, combat step) or anything but a root stance is reported with his position, and
the port's own reason for refusing the step from the log ([move] lines:
which blocking test -- a hole in reach, the ground height, a step between
cells, no walkmap, or a character). Filmstrip frames go to OUT/cap.

Environment: the INI with retail paths is written into the SavePath; the
binary finds the game data beside itself (as the gameflow runs do).
"""
from __future__ import annotations

import argparse
import math
import os
import re
import shutil
import subprocess
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[1]
ROOTS = ('walk', 'combat', 'hand', 'bow', 'sneak', 'run', 'torch', 'sleep')
CENTRE = (216, 186)                 # Locke on screen at 640x480 (the camera follows him)
INI = """[Paths]
ClassDefPath = ".\\Resources"
ExileRCPath = "."
ResourcePath = ".\\Resources"
CurMapPath = "."
BaseMapPath = "."
MoviePath = ".\\Disk2"
SaveGamePath = ".\\Save"
ImageryPath = ".\\Imagery"
ModulesPath = ".\\Modules"

[Options]
PlaySpeech = No
"""


def sweep(radius=90, bearings=16, hold_ms=1500) -> list[str]:
    cx, cy = CENTRE
    pts = [(round(cx + radius * math.cos(2 * math.pi * k / bearings)),
            round(cy + radius * math.sin(2 * math.pi * k / bearings))) for k in range(bearings)]
    script = ['wait 4000', f'move {pts[0][0]} {pts[0][1]} 0', 'right_down']
    for x, y in pts + list(reversed(pts)):
        script.append(f'move {x} {y} {hold_ms}')
    script += ['right_up', 'wait 1000']
    return script


def walks(radii=(60, 160), bearings=8, hold_ms=4000) -> list[str]:
    """Long walks: the right button held toward each of eight bearings for
    four seconds, near Locke (walk) and far (run), a snapshot after each."""
    cx, cy = CENTRE
    script = ['wait 1000']
    for r in radii:
        for k in range(bearings):
            a = 2 * math.pi * k / bearings
            script += [f'move {round(cx + r * math.cos(a))} {round(cy + r * math.sin(a))} 0', 'right_down',
                       f'wait {hold_ms}', 'right_up', f'take_snapshot r{r}b{k}', 'wait 300']
    return script


def run(slot: Path, out: Path, binary: Path, pattern: str, exec_line: str = '') -> dict:
    if out.exists():
        shutil.rmtree(out)
    save = out / 'save'
    (save / 'Save' / 'Single').mkdir(parents=True)
    (save / 'Revenant.ini').write_text(INI)
    shutil.copytree(slot, save / 'Save' / 'Single' / slot.name)
    exe = binary.parent / f'{binary.name}.walktest_{os.getpid()}'
    shutil.copy2(binary, exe)
    steps = (sweep() if pattern in ('sweep', 'both') else []) + (walks() if pattern in ('walks', 'both') else [])
    if pattern == 'none':
        steps = ['wait 150000']
    steps.append('take_snapshot end')
    shots = sum(1 for s in steps if s.startswith('take_snapshot'))
    (out / 'cap').mkdir()
    args = [str(exe), '--headless', '--resolution=640x480', f'--quickstart={slot.name}',
            f'--combattrace={out / "trace.tsv"}', '--max-runtime=400', f'--filmstrip={shots},0',
            f'--snapprefix={out / "cap" / "walk_"}', '--input-script=' + '; '.join(steps)]
    if exec_line:
        args.append(f'--exec={exec_line}')
    (out / 'cmd.txt').write_text(' '.join(repr(a) for a in args) + '\n')
    try:
        with open(out / 'stdout.txt', 'wb') as so:
            rc = subprocess.run(args, cwd=out, env=dict(os.environ, REVENANT_SAVE_PATH=str(save)),
                                stdout=so, stderr=subprocess.STDOUT).returncode
    finally:
        exe.unlink(missing_ok=True)
    return dict(rc=rc, **analyse(out))


def analyse(out: Path) -> dict:
    ticks = []
    for line in (out / 'trace.tsv').read_text(errors='replace').splitlines():
        f = line.split('\t')
        if len(f) > 2 and f[1] == 'char' and any(x.startswith('name=Locke') for x in f):
            d = dict(x.split('=', 1) for x in f[2:] if '=' in x)
            ticks.append((int(f[0]), tuple(int(v) for v in d['pos'].split(',')), d.get('state', '?'),
                          d.get('doing', '')))
    stuck, start = [], None
    for i in range(1, len(ticks)):
        # Held up: a move action (MOVE, COMBATMOVE, BOWMOVE) going nowhere, or
        # any block but a root stance (the 1998 movement left Locke in a
        # "fall" block on the Keep's stairs for good).
        action, name = (ticks[i][3].split(':') + ['', ''])[:2]
        held = action in ('2', '4', '26') or name not in ROOTS
        if ticks[i][1] == ticks[i - 1][1] and held:
            start = start if start is not None else ticks[i - 1]
        else:
            if start is not None and ticks[i - 1][0] - start[0] >= 48:
                stuck.append(dict(tick=start[0], ticks=ticks[i - 1][0] - start[0], pos=start[1],
                                  state=f'{start[2]}, doing {start[3]}'))
            start = None
    if start is not None and ticks[-1][0] - start[0] >= 48:          # still held up at the end
        stuck.append(dict(tick=start[0], ticks=ticks[-1][0] - start[0], pos=start[1],
                          state=f'{start[2]}, doing {start[3]} (to the end)'))
    log = (out / 'revenant.log').read_text(errors='replace') if (out / 'revenant.log').exists() else ''
    blocks = [l.split('[move] ', 1)[1] for l in log.splitlines() if '[move] ' in l]
    travelled = sum(math.dist(ticks[i][1][:2], ticks[i - 1][1][:2]) for i in range(1, len(ticks)))
    return dict(ticks=len(ticks), travelled=round(travelled), stuck=stuck, blocks=blocks,
                end=ticks[-1][1] if ticks else None)


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('slot', type=Path)
    p.add_argument('--out', type=Path, default=REPO / 'build' / 'walktest')
    p.add_argument('--binary', type=Path, default=REPO / 'build' / 'Revenant')
    p.add_argument('--pattern', default='both', choices=('sweep', 'walks', 'both', 'none'))
    p.add_argument('--exec', default='', help='console commands, as the game takes them (e.g. player.goto X Y)')
    p.add_argument('--tag', default='', help='suffix for the output folder')
    p.add_argument('--analyse', action='store_true', help='only re-read the last run in --out')
    a = p.parse_args()
    if a.analyse:
        out = a.out.resolve() / (a.slot.name.replace(' ', '_') + a.tag)
        r = dict(rc='-', **analyse(out))
    else:
        r = run(a.slot.resolve(), a.out.resolve() / (a.slot.name.replace(' ', '_') + a.tag), a.binary.resolve(),
                a.pattern, a.exec)
    print(f"{a.slot.name}: exit {r['rc']}, {r['ticks']} ticks traced, {r['travelled']} units walked, ends at {r['end']}")
    for s in r['stuck']:
        print(f"  STUCK {s['ticks']} ticks from tick {s['tick']} at {s['pos']} ({s['state']})")
    if not r['stuck']:
        print('  never stuck for 2 s or more')
    print(f"  {len(r['blocks'])} refused-step reports" + (':' if r['blocks'] else ''))
    for b in r['blocks'][:20]:
        print('   ', b)


if __name__ == '__main__':
    main()
