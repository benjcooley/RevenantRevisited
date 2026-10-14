#!/usr/bin/env python3
"""Run Shadow Smoke and file its saves (docs/shadow/README.md).

Plays a plan in the port (`--shadow`), headless, from its own run
directory and save path, then adds each milestone save the run made to the
waypoint catalogue (tools/playtest/waypoints.py) as `shadow <name>`, and
with --check load-checks each one in a fresh game. Exits with the game's
code (0 pass, 1 a step failed, 2 a ledger violation), or 3 when a load
check fails.

    tools/shadow/shadow.py                          # the whole plan, seed 1
    tools/shadow/shadow.py --until opening --check  # one stage, then check its save
    tools/shadow/shadow.py --stage s1 --start "left the keep" --until s1
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import shutil
import subprocess
import sys
import tempfile
import time
import uuid
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(REPO / 'tools' / 'playtest'))
import waypoints  # noqa: E402  (the catalogue)

PLAN = REPO / 'tools' / 'shadow' / 'ahkuilon.plan'
BINARY = REPO / 'build' / 'Revenant'
INI = Path.home() / 'RevenantRevisited-testsaves' / 'Revenant.ini'
STORY = 'shadow'                     # the catalogue's story list for these saves


def run_game(args: argparse.Namespace, run_dir: Path) -> int:
    """The run itself, in `run_dir`: its save path, the game's output, the
    run's log in `out/`. Returns the game's exit code."""
    save = run_dir / 'save'
    save.mkdir(parents=True, exist_ok=True)
    shutil.copy2(args.ini, save / 'Revenant.ini')
    # A private copy of the binary: a rebuild never writes the file a run
    # is executing (tools/playtest/host.py does the same).
    binary = args.binary.with_name(f'{args.binary.name}.shadow-{uuid.uuid4().hex[:8]}')
    shutil.copy2(args.binary, binary)
    command = [str(binary), '--headless', f'--max-runtime={args.max_runtime}',
               f'--shadow={args.plan.resolve()}', f'--shadow-out={run_dir / "out"}', f'--seed={args.seed}']
    if args.stage:
        # Start at a stage, from a waypoint's save copied into this run's slots.
        waypoint = waypoints.Catalogue().get('port', args.start)
        shutil.copytree(waypoint.slot_dir, save / 'Save' / 'Single' / waypoint.slot, dirs_exist_ok=True)
        command += [f'--shadow-stage={args.stage}', f'--quickstart={waypoint.slot}']
    if args.until:
        command.append(f'--shadow-until={args.until}')
    if args.lenient:
        command.append('--shadow-lenient')
    try:
        with open(run_dir / 'game.txt', 'w') as out:
            return subprocess.run(command, cwd=run_dir, stdout=out, stderr=subprocess.STDOUT,
                                  env={**os.environ, 'REVENANT_SAVE_PATH': str(save)}).returncode
    finally:
        binary.unlink(missing_ok=True)


def file_waypoints(run_dir: Path, plan: Path, seed: int, check: bool) -> list[str]:
    """Adds the run's saves to the catalogue; with `check`, load-checks
    each. Returns the problems found."""
    listing = run_dir / 'out' / 'waypoints.json'
    if not listing.exists():
        return []
    run = json.loads(listing.read_text())
    catalogue = waypoints.Catalogue()
    plan_sha = hashlib.sha256(plan.read_bytes()).hexdigest()[:12]
    parent = 'newgame'
    problems = []
    for index, saved in enumerate(run['waypoints'], 1):
        name = f'shadow {saved["name"]}'
        added = catalogue.add(
            'port', name, STORY, index, parent, Path(saved['dir']), obs={'player': saved['player']},
            build={'sha': run['sha'], 'name': run['build']}, tick=saved['tick'],
            note=f'Shadow Smoke, plan {plan.name}@{plan_sha}, seed {seed}, stage {saved["stage"]}',
            slot_name=saved['slot'])
        print(f'  filed {name!r} -> {added.dir}')
        if check:
            found = waypoints.check(added, ['port'], run_dir / 'checks')
            print(f'    load check: {"ok" if not found else "; ".join(found)}')
            problems += [f'{name}: {p}' for p in found]
        parent = name
    return problems


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    parser.add_argument('plan', type=Path, nargs='?', default=PLAN)
    parser.add_argument('--seed', type=int, default=1)
    parser.add_argument('--stage', help='start at this stage, from the waypoint --start names')
    parser.add_argument('--start', help='the waypoint (catalogue name or directory) --stage starts from')
    parser.add_argument('--until', help='stop after this stage')
    parser.add_argument('--lenient', action='store_true', help='log ledger violations instead of stopping')
    parser.add_argument('--check', action='store_true', help='load-check each save in a fresh game')
    parser.add_argument('--no-file', action='store_true', help="don't add the saves to the catalogue")
    parser.add_argument('--run-dir', type=Path,
                        default=Path(tempfile.gettempdir()) / f'shadow-{time.strftime("%Y%m%d-%H%M%S")}')
    parser.add_argument('--binary', type=Path, default=BINARY)
    parser.add_argument('--ini', type=Path, default=INI)
    parser.add_argument('--max-runtime', type=int, default=3600, help='seconds of wall time')
    args = parser.parse_args()
    if bool(args.stage) != bool(args.start):
        parser.error('--stage and --start go together')

    run_dir = args.run_dir.resolve()
    run_dir.mkdir(parents=True, exist_ok=True)
    print(f'shadow: {args.plan} seed {args.seed} in {run_dir}')
    code = run_game(args, run_dir)
    summary = run_dir / 'out' / 'summary.txt'
    print(summary.read_text() if summary.exists() else f'no summary; see {run_dir / "game.txt"}')
    if not args.no_file:
        if file_waypoints(run_dir, args.plan.resolve(), args.seed, args.check) and code == 0:
            code = 3
    return code


if __name__ == '__main__':
    sys.exit(main())
