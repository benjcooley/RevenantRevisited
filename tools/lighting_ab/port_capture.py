#!/usr/bin/env python3
"""Capture the port at each lighting A/B location, in Classic or modern.

    port_capture.py --run NAME [--mode classic|modern] [--overlay DIR]
                    [--settle MS] [-j N] [location ...]

For each location the slot (make_slots.py) is copied into a fresh save
directory with an INI holding the lab's keys (RETAIL_KEYS: RealTimeLight=No,
GammaLevel=3, ...), and the port loads it headless at 640x480 and takes one
snapshot after `--settle` milliseconds. Modern runs add --revisited, with
REVENANT_REVISITED_PATH at the overlay under test (default the repo's
revisited/resources, which sets LightingMode = 1).

Output: ~/RevenantRevisited-lightab/<run>/<mode>/<id>.png, and the run's
log beside it (<id>.log: the area entered and its ambient, the light
counts). docs/lighting/LIGHTING_AB.md §4.
"""
from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[1]
sys.path.insert(0, str(HERE))
from make_slots import OUT as SLOTS, load_locations  # noqa: E402

LAB_OUT = Path.home() / 'RevenantRevisited-lightab'
DATA = os.environ.get('REVENANT_DATA_PATH', str(Path.home() / 'RevenantRetailLab' / 'retail-cd' / 'REVENANT'))

# The lab's lighting keys (docs/lighting/LIGHTING_AB.md §1), so both games
# light the same way.
INI = r'''[Paths]
ClassDefPath = ".\Resources"
ExileRCPath = "."
ResourcePath = ".\Resources"
CurMapPath = "."
BaseMapPath = "."
MoviePath = ".\Disk2"
SaveGamePath = ".\Save"
ImageryPath = ".\Imagery"
ModulesPath = ".\Modules"

[Lighting]
MaxLights = 3
Ambient3D = 130
LightRange3D = 180
LightMult3D = 250

[Options]
ShowDialog = Yes
PlaySpeech = No
AutoCombat = No
Software3D = Yes
GammaLevel = 3
EnhancedLighting = No
RealTimeLight = No

[Modules]
MainModule = "Ahkuilon"

[Language]
Language = "English"
'''


def capture(loc: dict, mode: str, out: Path, overlay: Path | None, settle_ms: int, binary: Path) -> str:
    ident = loc['id']
    work = out / '.work' / ident
    if work.exists():
        shutil.rmtree(work)
    save = work / 'save'
    (save / 'Save' / 'Single').mkdir(parents=True)
    (save / 'Revenant.ini').write_text(INI)
    shutil.copytree(SLOTS / ident, save / 'Save' / 'Single' / ident)

    # Data is found relative to the executable: each run gets its own copy
    # in build/ so runs can go side by side.
    exe = binary.parent / f'{binary.name}.lightab_{mode}_{ident}'
    shutil.copy2(binary, exe)
    env = dict(os.environ, REVENANT_SAVE_PATH=str(save), REVENANT_DATA_PATH=DATA)
    args = [str(exe), '--headless', '--resolution=640x480', f'--quickstart={ident}',
            f'--max-runtime={settle_ms // 1000 + 40}', f'--snapprefix={work}/',
            '--filmstrip=1,0', f'--input-script=wait {settle_ms}; take_snapshot shot', '--exec', 'sleep 48; timeofday']
    if mode == 'modern':
        args.append('--revisited')
        env['REVENANT_REVISITED_PATH'] = str(overlay)
    try:
        with open(work / 'out.txt', 'w') as log:
            subprocess.run(args, cwd=work, env=env, stdout=log, stderr=subprocess.STDOUT, timeout=settle_ms / 1000 + 60)
    finally:
        exe.unlink(missing_ok=True)

    shots = sorted(work.glob('*shot.png'))
    if not shots:
        return f'{ident}: no snapshot (see {work}/out.txt)'
    shutil.copy2(shots[-1], out / f'{ident}.png')
    lines = []
    if (work / 'revenant.log').exists():
        for line in (work / 'revenant.log').read_text(errors='replace').splitlines():
            if any(k in line for k in ('[area] entered', 'At the tone', '[retail-mesh-light]', 'FATAL', 'ERROR')):
                lines.append(line)
    (out / f'{ident}.log').write_text('\n'.join(lines) + '\n')
    entered = next((l.split('[area] ')[1] for l in lines if '[area] entered' in l), 'no area entered')
    return f'{ident}: {entered}'


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('locations', nargs='*')
    p.add_argument('--run', required=True)
    p.add_argument('--mode', choices=['classic', 'modern'], default='classic')
    p.add_argument('--overlay', type=Path, default=REPO / 'revisited' / 'resources')
    p.add_argument('--settle', type=int, default=9000, help='milliseconds from start to the snapshot')
    p.add_argument('-j', type=int, default=3)
    p.add_argument('--binary', type=Path, default=REPO / 'build' / 'Revenant')
    a = p.parse_args(argv)
    out = LAB_OUT / a.run / a.mode
    out.mkdir(parents=True, exist_ok=True)
    locations = load_locations(a.locations)
    with ThreadPoolExecutor(a.j) as pool:
        for line in pool.map(lambda loc: capture(loc, a.mode, out, a.overlay.resolve(), a.settle, a.binary), locations):
            print(line, flush=True)


if __name__ == '__main__':
    main()
