#!/usr/bin/env python3
"""The save point repository: a save for every story point of the game.

Each `save` step of a Shadow plan is a story point ("rahul defeated", ...,
"jason freed"). A Shadow run files every save it makes into the waypoint
catalogue (tools/playtest/waypoints.py, ~/RevenantRevisited-testsaves/
waypoints/port/), with a sidecar, `shadow.json`, that says which plan and
stage made it, with what seed and style, and whether its round trip held
(ARCHITECTURE.md §10). A story point's save point is its newest save whose
round trip didn't fail. Segments start from them (`shadow.py --segment`),
and a person can load any of them in the port or in retail (`export`).

    tools/shadow/savepoints.py list                    # every story point and its save
    tools/shadow/savepoints.py export "jason freed" --to <game>/Save/Single
    tools/shadow/savepoints.py export all --to <dir>
    tools/shadow/savepoints.py prune --keep 3          # list what --yes would delete

Docs: docs/shadow/SAVEPOINTS.md.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import shutil
import sys
from dataclasses import dataclass, field
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[1]
sys.path.insert(0, str(REPO / 'tools' / 'playtest'))
import waypoints  # noqa: E402  (the catalogue)

PLAN = HERE / 'ahkuilon.plan'
GAME = 'port'                       # the catalogue's game for saves the port made
SIDECAR = 'shadow.json'
SLOT_PREFIX = 'Shadow '             # the run saves "Shadow <story point>" (TSaveRunner)

STAGE_LINE = re.compile(r'^stage\s+(\S+)\s+"([^"]*)"')
SAVE_LINE = re.compile(r'^\s+save\s+"([^"]*)"')


@dataclass
class Stage:
    id: str
    title: str
    saves: list[str] = field(default_factory=list)   # its story points, in order


def plan_stages(plan: Path) -> list[Stage]:
    """The plan's stages in order, each with the story points it saves.
    (Routines are written like stages but aren't played in order: left out.)"""
    stages: list[Stage] = []
    current: Stage | None = None
    for line in plan.read_text(encoding='utf-8').splitlines():
        if m := STAGE_LINE.match(line):
            current = Stage(m.group(1), m.group(2))
            stages.append(current)
        elif line.startswith('routine'):
            current = None
        elif current and (m := SAVE_LINE.match(line)):
            current.saves.append(m.group(1))
    return stages


def plan_sha(plan: Path) -> str:
    return hashlib.sha256(plan.read_bytes()).hexdigest()[:12]


@dataclass
class SavePoint:
    """A filed Shadow save: its catalogue entry and its sidecar."""
    waypoint: waypoints.Waypoint
    meta: dict

    @property
    def name(self) -> str:                  # the story point
        return self.meta['point']

    @property
    def good(self) -> bool:                 # not retired, and its round trip didn't fail
        return not self.waypoint.retired and self.meta.get('round_trip') is not False


def record(waypoint: waypoints.Waypoint, meta: dict) -> None:
    """Writes a filed save's sidecar."""
    (waypoint.dir / SIDECAR).write_text(json.dumps(meta, indent=2) + '\n')


def all_points(plan: Path) -> list[SavePoint]:
    """Every filed save of this plan (by its file name), oldest first."""
    found = []
    for w in waypoints.Catalogue().all(GAME):
        sidecar = w.dir / SIDECAR
        if sidecar.exists():
            meta = json.loads(sidecar.read_text())
            if meta.get('plan') == plan.name:
                found.append(SavePoint(w, meta))
    return sorted(found, key=lambda p: p.waypoint.created)


def current(plan: Path, point: str, points: list[SavePoint] | None = None) -> SavePoint | None:
    """A story point's save point: its newest good save, or None."""
    good = [p for p in (points if points is not None else all_points(plan)) if p.name == point and p.good]
    return good[-1] if good else None


def start_for(plan: Path, stage: str) -> tuple[SavePoint | None, str]:
    """Where a segment that ends with `stage` starts: the newest save point of
    the stages before it, the nearest first, and the stage after that save's;
    (None, the first stage) from a new game."""
    stages = plan_stages(plan)
    index = next((i for i, s in enumerate(stages) if s.id.lower() == stage.lower()), None)
    if index is None:
        raise KeyError(f'the plan has no stage {stage!r}')
    points = all_points(plan)
    for k in range(index - 1, -1, -1):
        for name in reversed(stages[k].saves):
            if found := current(plan, name, points):
                return found, stages[k + 1].id
    return None, stages[0].id


def segments(plan: Path) -> list[str]:
    """The stages a segment ends with: those that save."""
    return [s.id for s in plan_stages(plan) if s.saves]


# =====================================================================
# Commands
# =====================================================================

def describe(p: SavePoint, sha: str) -> str:
    player = p.waypoint.player or {}
    xp = player.get('xp') or [None]
    gold = player.get('gold')
    older = '' if p.meta.get('plan_sha') == sha else f', plan@{p.meta.get("plan_sha")} (older)'
    trip = {True: 'round trip ok', False: 'round trip FAILED', None: 'round trip unchecked'}[p.meta.get('round_trip')]
    return (f'{p.waypoint.created}  level {xp[0]}, {gold} gold  '
            f'build {(p.waypoint.build or {}).get("name", "?")}  {trip}{older}')


def cmd_list(plan: Path) -> int:
    sha = plan_sha(plan)
    points = all_points(plan)
    print(f'save points of {plan.name}@{sha} ({waypoints.ROOT / GAME}):')
    for stage in plan_stages(plan):
        for name in stage.saves:
            found = current(plan, name, points)
            count = sum(1 for p in points if p.name == name)
            text = describe(found, sha) if found else 'MISSING'
            print(f'  {stage.id:<10} {name!r:<20} {text}' + (f'  ({count} filed)' if count > 1 else ''))
            if found:
                print(f'  {"":<10} {"":<20} {found.waypoint.slot_dir}')
    return 0


def cmd_export(plan: Path, what: str, to: Path) -> int:
    """Copies save points into a game's save folder (Save/Single), each under
    its own slot name, so the game's Load lists them."""
    points = all_points(plan)
    names = [n for s in plan_stages(plan) for n in s.saves] if what == 'all' else [what]
    to.mkdir(parents=True, exist_ok=True)
    missing = 0
    for name in names:
        found = current(plan, name, points)
        if not found:
            print(f'  {name!r}: no save point')
            missing += 1
            continue
        target = to / found.waypoint.slot
        shutil.copytree(found.waypoint.slot_dir, target, dirs_exist_ok=True)
        print(f'  {name!r} -> {target}')
    return 1 if missing else 0


def cmd_prune(plan: Path, keep: int, delete: bool) -> int:
    """Each story point's saves beyond its newest `keep` good ones, and every
    one whose round trip failed: listed, and deleted with `delete`."""
    points = all_points(plan)
    names = {p.name for p in points}
    doomed: list[SavePoint] = []
    for name in sorted(names):
        mine = [p for p in points if p.name == name]
        good = [p for p in mine if p.good]
        doomed += [p for p in mine if not p.good] + good[:-keep]
    for p in doomed:
        print(f'  {"deleting" if delete else "would delete"} {p.name!r} {p.waypoint.created}: {p.waypoint.dir}')
        if delete:
            shutil.rmtree(p.waypoint.dir)
    if not doomed:
        print('  nothing to prune')
    elif not delete:
        print(f'  {len(doomed)} saves; --yes deletes them')
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    parser.add_argument('--plan', type=Path, default=PLAN)
    sub = parser.add_subparsers(dest='command', required=True)
    sub.add_parser('list', help='every story point of the plan and its save point')
    export = sub.add_parser('export', help="copy save points into a game's save folder")
    export.add_argument('point', help='a story point ("jason freed"), or all')
    export.add_argument('--to', type=Path, required=True, help='the folder: <game>/Save/Single')
    prune = sub.add_parser('prune', help="trim each story point's older saves")
    prune.add_argument('--keep', type=int, default=3, help='good saves kept per story point (at least 1)')
    prune.add_argument('--yes', action='store_true', help='delete; without it, only list')
    args = parser.parse_args()
    plan = args.plan.resolve()
    if args.command == 'list':
        return cmd_list(plan)
    if args.command == 'prune':
        if args.keep < 1:
            parser.error('--keep is at least 1: a story point keeps its save point')
        return cmd_prune(plan, args.keep, args.yes)
    return cmd_export(plan, args.point, args.to)


if __name__ == '__main__':
    sys.exit(main())
