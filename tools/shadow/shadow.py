#!/usr/bin/env python3
"""Run Shadow Smoke and file its saves (docs/shadow/README.md).

Plays a plan in the port (`--shadow`), headless, from its own run
directory and save path, then adds each milestone save the run made to the
waypoint catalogue (tools/playtest/waypoints.py) as `shadow <name>`. With
--check each save makes the round trip first (ARCHITECTURE.md §10): a fresh
game loads it (`--shadow-verify`) and Locke's state there must match the
state the run recorded as it saved, field by field. Exits with the game's
code (0 pass, 1 a step failed, 2 a ledger violation), or 3 when a save
doesn't come back as it went.

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


def private_binary(binary: Path) -> Path:
    """A private copy of the binary: a rebuild never writes the file a run
    is executing (tools/playtest/host.py does the same)."""
    copy = binary.with_name(f'{binary.name}.shadow-{uuid.uuid4().hex[:8]}')
    shutil.copy2(binary, copy)
    return copy


def run_game(args: argparse.Namespace, run_dir: Path) -> int:
    """The run itself, in `run_dir`: its save path, the game's output, the
    run's log in `out/`. Returns the game's exit code."""
    save = run_dir / 'save'
    save.mkdir(parents=True, exist_ok=True)
    shutil.copy2(args.ini, save / 'Revenant.ini')
    binary = private_binary(args.binary)
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
    if args.style:
        command.append(f'--shadow-style={args.style}')
    try:
        with open(run_dir / 'game.txt', 'w') as out:
            return subprocess.run(command, cwd=run_dir, stdout=out, stderr=subprocess.STDOUT,
                                  env={**os.environ, 'REVENANT_SAVE_PATH': str(save)}).returncode
    finally:
        binary.unlink(missing_ok=True)


# How far the reload may drift in the second it settles; everything not
# listed must be equal.
POS_SLACK = 32          # units, x and y
VITAL_SLACK = 2         # health and mana points (regeneration, ~1 a minute)


def compare_states(saved: dict, loaded: dict) -> list[str]:
    """The fields where the reload differs from the save."""
    found = []
    for key in sorted(set(saved) | set(loaded)):
        was, now = saved.get(key), loaded.get(key)
        if key == 'stateffects':
            continue                        # a save keeps none (PLAYER_STATS.md §4, as retail)
        if key == 'modified' and saved.get('stateffects'):
            continue                        # so a stat effect in force makes these differ
        if key == 'pos':
            if was is None or now is None or any(abs(a - b) > POS_SLACK for a, b in zip(was[:2], now[:2])):
                found.append(f'pos {was} -> {now}')
        elif key in ('health', 'mana'):
            if was[1] != now[1] or abs(was[0] - now[0]) > VITAL_SLACK:
                found.append(f'{key} {was} -> {now}')
        elif key == 'fatigue':
            if was[1] != now[1]:            # it refills in seconds: the maximum only
                found.append(f'fatigue max {was[1]} -> {now[1]}')
        elif key == 'items':
            lost = sorted(set(was) - set(now))
            new = sorted(set(now) - set(was))
            found += [f'item lost: {i}' for i in lost] + [f'item new: {i}' for i in new]
        elif isinstance(was, dict) and isinstance(now, dict):
            found += [f'{key}.{k}: {was.get(k)} -> {now.get(k)}'
                      for k in sorted(set(was) | set(now)) if was.get(k) != now.get(k)]
        elif was != now:
            found.append(f'{key}: {was} -> {now}')
    return found


def verify_saves(args: argparse.Namespace, run_dir: Path) -> list[str]:
    """The round trip of each save the run made: a fresh game loads it and
    writes Locke's state, which must match the state recorded at the save.
    Returns the differences, by save."""
    listing = run_dir / 'out' / 'waypoints.json'
    if not listing.exists():
        return []
    problems = []
    save = run_dir / 'save'
    verify = run_dir / 'verify'
    verify.mkdir(exist_ok=True)
    for saved in json.loads(listing.read_text())['waypoints']:
        state = verify / f'{saved["slot"]}.json'
        state.unlink(missing_ok=True)
        binary = private_binary(args.binary)
        try:
            with open(verify / f'{saved["slot"]}.txt', 'w') as out:
                subprocess.run([str(binary), '--headless', '--max-runtime=300', f'--quickstart={saved["slot"]}',
                                f'--shadow-verify={state}', f'--seed={args.seed}'],
                               cwd=run_dir, stdout=out, stderr=subprocess.STDOUT,
                               env={**os.environ, 'REVENANT_SAVE_PATH': str(save)})
        finally:
            binary.unlink(missing_ok=True)
        if not state.exists():
            found = [f'the reload wrote no state (see {verify / (saved["slot"] + ".txt")})']
        else:
            found = compare_states(saved['state'], json.loads(state.read_text()))
        print(f'  round trip {saved["slot"]!r}: {"ok" if not found else f"{len(found)} differences"}')
        for f in found:
            print(f'    {f}')
        problems += [f'{saved["slot"]}: {f}' for f in found]
    return problems


# Readers that run by themselves once Locke is in their area (a dialog
# reader needs him to talk to its owner).
AUTOMATIC_TRIGGERS = ('ALWAYS', 'CUBE', 'PROXIMITY')


def story_report(run_dir: Path) -> list[str]:
    """The story as the run moved it (ARCHITECTURE.md §9.4): each change of
    a story variable with the writer that made it, and for a variable the
    town reads, whether the town has seen the new value -- an automatic
    reader there ran after the change. Returns the changes the town hasn't
    seen."""
    coverage_file = run_dir / 'out' / 'coverage.json'
    if not coverage_file.exists():
        return []
    run = json.loads(coverage_file.read_text())
    out = subprocess.run([sys.executable, str(REPO / 'tools' / 'shadow' / 'storymap.py'), '--format', 'json'],
                         capture_output=True, text=True, check=True)
    story = json.loads(out.stdout)
    ran = {(b['file'].lower(), b['object'].lower()): {int(k): v for k, v in b['lines'].items()}
           for b in run['blocks']}

    def ticks(event: dict) -> dict:
        return ran.get((event['file'].lower(), event['object'].lower()), {})

    def last_run(event: dict) -> int | None:
        return ticks(event).get(event['line'] - event['object_line'])

    unseen = []
    print('  story:')
    for change in run['changes']:
        var = story['variables'].get(change['var'], {})
        by = [w for w in var.get('writers', []) if w['value'] == str(change['is'])
              and last_run(w) is not None and last_run(w) <= change['tick']]
        writer = ', '.join(f'{w["object"]} ({w["id"]})' for w in by) or 'a writer the map lacks'
        print(f'    {change["tick"]:>7} {change["var"]} {change["was"]} -> {change["is"]} by {writer}')
        town = story['coverage']['town_variables'].get(change['var'])
        if not town:
            continue
        readers = {r['id'] for r in town['town_readers']}
        events = [r for r in var.get('readers', []) if r['id'] in readers]
        automatic = [r for r in events if r['trigger'] in AUTOMATIC_TRIGGERS]
        seen = [r for r in events if (last_run(r) or -1) > change['tick']]
        if automatic and not any(r in seen for r in automatic):
            unseen.append(f'{change["var"]} = {change["is"]} (tick {change["tick"]}): no automatic town reader ran '
                          f'after it ({", ".join(r["id"] for r in automatic)})')
            print(f'            the town hasn\'t seen it: {", ".join(r["id"] for r in automatic)}')
        elif seen:
            print(f'            the town has seen it: {", ".join(r["object"] + " " + r["id"] for r in seen)}')
        if not seen and not automatic:
            print(f'            only dialogs read it in town; none talked to since '
                  f'({", ".join(sorted({r["object"] for r in events}))})')
    return unseen


def file_waypoints(run_dir: Path, plan: Path, seed: int) -> None:
    """Adds the run's saves to the catalogue."""
    listing = run_dir / 'out' / 'waypoints.json'
    if not listing.exists():
        return
    run = json.loads(listing.read_text())
    catalogue = waypoints.Catalogue()
    plan_sha = hashlib.sha256(plan.read_bytes()).hexdigest()[:12]
    parent = 'newgame'
    for index, saved in enumerate(run['waypoints'], 1):
        name = f'shadow {saved["name"]}'
        added = catalogue.add(
            'port', name, STORY, index, parent, Path(saved['dir']), obs={'player': saved['player']},
            build={'sha': run['sha'], 'name': run['build']}, tick=saved['tick'],
            note=f'Shadow Smoke, plan {plan.name}@{plan_sha}, seed {seed}, stage {saved["stage"]}',
            slot_name=saved['slot'])
        print(f'  filed {name!r} -> {added.dir}')
        parent = name


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    parser.add_argument('plan', type=Path, nargs='?', default=PLAN)
    parser.add_argument('--seed', type=int, default=1)
    parser.add_argument('--stage', help='start at this stage, from the waypoint --start names')
    parser.add_argument('--start', help='the waypoint (catalogue name or directory) --stage starts from')
    parser.add_argument('--until', help='stop after this stage')
    parser.add_argument('--story-gate', action='store_true',
                        help="fail (4) when the town hasn't seen a story change by the end of the run")
    parser.add_argument('--style', choices=['melee', 'hands', 'bow', 'spells', 'mixed'],
                        help="play the plan in this style instead of its own (ARCHITECTURE.md §8.4)")
    parser.add_argument('--lenient', action='store_true', help='log ledger violations instead of stopping')
    parser.add_argument('--check', action='store_true',
                        help="reload each save in a fresh game and compare Locke's state with the save's")
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
    if args.check and verify_saves(args, run_dir) and code == 0:
        code = 3
    unseen = story_report(run_dir)
    if args.story_gate and unseen and code == 0:
        print('  story gate:\n' + '\n'.join(f'    {u}' for u in unseen))
        code = 4
    if not args.no_file:
        file_waypoints(run_dir, args.plan.resolve(), args.seed)
    return code


if __name__ == '__main__':
    sys.exit(main())
