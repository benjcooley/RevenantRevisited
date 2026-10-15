#!/usr/bin/env python3
"""Waypoints: save games at story points that sessions play from.

    waypoints.py list [GAME]                       every waypoint, in story order
    waypoints.py show GAME NAME                    one waypoint's manifest
    waypoints.py path GAME NAME                    its slot directory (play.py --slot)
    waypoints.py import GAME NAME SLOT_DIR --story keep --index N [--note TEXT]
                                                   an existing save slot as a waypoint
    waypoints.py check [GAME [NAME]] [--load port retail]
                                                   is each save what its name says? (below)
    waypoints.py retire GAME DIR --why TEXT        keep a bad one, but never play from it

A waypoint is a save slot at a point in a story (a milestone list in
milestones.py), in one game, with what produced it: its parent waypoint
(or a new game), the build, the session tick, the player's state, and the
recording bundle that replays the way from the parent to it. Sessions start
from a waypoint (play.py --from NAME) and every saving milestone they reach
adds the next one, so the catalogue grows along the story, in both games.

A waypoint is checked two ways. Its save's story variables (game.sav's
state list, which scripts gate on) must hold what its milestone expects
(Milestone.expect, from the module's scripts); the director checks that as
it saves and retires a save that fails. And `check --load` loads it in a
game, port or retail (the saves are the same format), plays two seconds and
looks: in play, Locke alive, on the level and at the spot it was saved at.
Results go in the manifest's `checks`.

Nothing is overwritten: each waypoint gets its own directory,
`<root>/<game>/<name>--<created>/`, and a name finds the newest. The root
is ~/RevenantRevisited-testsaves/waypoints.

    <waypoint>/waypoint.json     the manifest
    <waypoint>/slot/<slot>/      the save: game.sav, CurMap/, ss.bmp
    <waypoint>/recording/        the bundle from the parent (session.rvrec, Revenant.ini, start/)
"""
from __future__ import annotations

import argparse
import hashlib
import json
import math
import re
import shutil
import sys
import tempfile
import time
from dataclasses import asdict, dataclass, field
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import rec  # noqa: E402

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'savefmt'))
import revsave  # noqa: E402  (game.sav decoding)

ROOT = Path.home() / 'RevenantRevisited-testsaves' / 'waypoints'
MANIFEST = 'waypoint.json'
LOAD_TICKS = 48                      # a load check plays this long before it looks
LOAD_DRIFT = 96                      # how far from the saved spot Locke may stand then


def slug(name: str) -> str:
    return re.sub(r'[^a-z0-9]+', '-', name.lower()).strip('-') or 'waypoint'


@dataclass
class Waypoint:
    game: str
    name: str
    story: str                       # the milestone list (milestones.LISTS)
    index: int                       # milestones of the story done here: play on from this one
    parent: str                      # the waypoint it was played from, or 'newgame'
    slot: str                        # the save slot's name
    slot_sha: str
    created: str
    build: dict = field(default_factory=dict)
    tick: int | None = None
    level: int | None = None
    pos: list | None = None
    player: dict = field(default_factory=dict)
    note: str = ''
    recording: dict | None = None    # {'bundle': 'recording', 'until': tick}
    retired: str = ''                # why it must not be played from; '' for a good one
    checks: dict = field(default_factory=dict)   # game -> the latest load check
    dir: Path | None = field(default=None, compare=False)

    @property
    def slot_dir(self) -> Path:
        return self.dir / 'slot' / self.slot

    def save_manifest(self) -> None:
        data = {k: v for k, v in asdict(self).items() if k != 'dir'}
        (self.dir / MANIFEST).write_text(json.dumps(data, indent=2))

    @classmethod
    def load(cls, directory: Path) -> Waypoint:
        data = json.loads((directory / MANIFEST).read_text())
        return cls(**data, dir=directory)


@dataclass
class Catalogue:
    root: Path = ROOT

    def all(self, game: str | None = None) -> list[Waypoint]:
        found = [Waypoint.load(m.parent) for m in sorted(self.root.glob(f'{game or "*"}/*/{MANIFEST}'))]
        return sorted(found, key=lambda w: (w.game, w.story, w.index, w.created))

    def get(self, game: str, name: str) -> Waypoint:
        """The newest good waypoint by that name, or the one in that
        directory (retired or not)."""
        matches = [w for w in self.all(game) if w.dir.name == name] or \
            [w for w in self.all(game) if w.name == name and not w.retired]
        if not matches:
            known = ', '.join(sorted({w.name for w in self.all(game)})) or 'none'
            raise KeyError(f'no {game} waypoint named {name!r} (known: {known})')
        return max(matches, key=lambda w: w.created)

    def add(self, game: str, name: str, story: str, index: int, parent: str, slot_dir: Path,
            obs: dict | None = None, build: dict | None = None, tick: int | None = None,
            note: str = '', slot_name: str | None = None, retired: str = '') -> Waypoint:
        """A new waypoint from an exported (or existing) slot: a retail slot
        directory, or the port's save file (<name>.rvsave,
        docs/gameflow/SAVE_FORMAT.md), copied in under `slot_name` (default:
        its own name). A save file's slot_dir is that file."""
        created = time.strftime('%Y%m%d-%H%M%S')
        directory = self.root / game / f'{slug(name)}--{created}'
        n = 1
        while directory.exists():                            # two in one second
            n += 1
            directory = self.root / game / f'{slug(name)}--{created}-{n}'
        slot_dir = Path(slot_dir)
        slot_name = slot_name or slot_dir.name
        if slot_dir.is_file():
            slot_name = Path(slot_name).stem + slot_dir.suffix
            (directory / 'slot').mkdir(parents=True)
            shutil.copy2(slot_dir, directory / 'slot' / slot_name)
            sha = hashlib.sha256(slot_dir.read_bytes()).hexdigest()
        else:
            shutil.copytree(slot_dir, directory / 'slot' / slot_name)
            sha = rec.slot_sha(slot_dir)
        player = (obs or {}).get('player', {})
        waypoint = Waypoint(
            game=game, name=name, story=story, index=index, parent=parent, slot=slot_name,
            slot_sha=sha, created=created, build=build or {}, tick=tick,
            level=player.get('level'), pos=player.get('pos'),
            player={k: player.get(k) for k in ('health', 'mana', 'fatigue', 'xp', 'gold', 'skills', 'weapon')
                    if player.get(k) is not None},
            note=note, retired=retired, dir=directory)
        waypoint.save_manifest()
        return waypoint

    def attach_recording(self, waypoint: Waypoint, bundle: Path, until: int) -> None:
        """The bundle that plays from the waypoint's parent to it; `until`
        is the tick of its saving mark (a replay stops there)."""
        target = waypoint.dir / 'recording'
        if target.exists():
            shutil.rmtree(target)
        shutil.copytree(bundle, target, ignore=shutil.ignore_patterns('frames'))
        waypoint.recording = {'bundle': 'recording', 'until': until}
        waypoint.save_manifest()


def story_states(slot_dir: Path) -> dict[str, int]:
    """The story variables a save holds (game.sav's state list)."""
    return {f.path[len('states['):-1]: f.value for f in revsave.decode(Path(slot_dir) / 'game.sav')
            if f.path.startswith('states[')}


def wrong_states(states: dict[str, int], expect: dict[str, int]) -> dict[str, list[int]]:
    """Each expected variable the save doesn't hold: [expected, saved]."""
    return {k: [v, states.get(k, 0)] for k, v in expect.items() if states.get(k, 0) != v}


def load_check(waypoint: Waypoint, game: str, run_dir: Path) -> dict:
    """Load the waypoint's save in `game`, play LOAD_TICKS and look."""
    from host import PORT_BINARY, PORT_INI, PortHost, RetailHost, clickwalk_build
    if game == 'port':
        host = PortHost.launch(PORT_BINARY, run_dir, PORT_INI, name='check')
    else:
        host = RetailHost.launch(run_dir, build=clickwalk_build())
    with host:
        info = host.hello()
        host.start('slot', seed=1, slot=waypoint.slot_dir, timeout=1800)
        host.step(LOAD_TICKS)
        obs = host.observe(objects=600)
    player = obs.get('player') or {}
    problems = []
    if obs.get('screen') != 'play':
        problems.append(f"screen {obs.get('screen')}")
    if not player:
        problems.append('no player')
    else:
        if (player.get('health') or [0])[0] <= 0:
            problems.append('Locke dead')
        if waypoint.level is not None and player.get('level') != waypoint.level:
            problems.append(f"level {player.get('level')}, saved on {waypoint.level}")
        elif waypoint.pos and math.dist(player['pos'][:2], waypoint.pos[:2]) > LOAD_DRIFT:
            problems.append(f"at {player['pos']}, saved at {waypoint.pos}")
    return dict(ok=not problems, problems=problems, when=time.strftime('%Y%m%d-%H%M%S'),
                build=info.get('build'), control=obs.get('control'),
                level=player.get('level'), pos=player.get('pos'), health=player.get('health'),
                near=[o['name'] or o['type'] for o in obs.get('objects', []) if o.get('class') == 'character'])


def check(waypoint: Waypoint, loads: list[str], run_root: Path) -> list[str]:
    """Check one waypoint (the module docstring); its manifest records
    the load checks. Returns the problems found."""
    from milestones import LISTS
    milestone = next((m for m in LISTS.get(waypoint.story, []) if m.name == waypoint.name), None)
    problems = []
    if not (waypoint.slot_dir / 'game.sav').exists():
        return ['no game.sav']
    wrong = wrong_states(story_states(waypoint.slot_dir), milestone.expect if milestone else {})
    if wrong:
        problems.append(f'story states {wrong}')
    for game in loads:
        result = load_check(waypoint, game, run_root / f'{waypoint.game}-{waypoint.dir.name}-{game}')
        waypoint.checks[game] = result
        waypoint.save_manifest()
        problems += [f'{game}: {p}' for p in result['problems']]
    return problems


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    parser.add_argument('--root', type=Path, default=ROOT)
    sub = parser.add_subparsers(dest='cmd', required=True)
    sub.add_parser('list').add_argument('game', nargs='?')
    for name in ('show', 'path'):
        p = sub.add_parser(name)
        p.add_argument('game')
        p.add_argument('name')
    imp = sub.add_parser('import')
    imp.add_argument('game', choices=('port', 'retail'))
    imp.add_argument('name')
    imp.add_argument('slot', type=Path)
    imp.add_argument('--story', default='keep')
    imp.add_argument('--index', type=int, required=True, help='milestones of the story done at this save')
    imp.add_argument('--note', default='')
    chk = sub.add_parser('check')
    chk.add_argument('game', nargs='?')
    chk.add_argument('name', nargs='?')
    chk.add_argument('--load', nargs='*', default=[], choices=('port', 'retail'))
    chk.add_argument('--run', type=Path, default=Path(tempfile.gettempdir()) / 'waypoint-checks')
    chk.add_argument('--retired', action='store_true', help='check retired ones too')
    ret = sub.add_parser('retire')
    ret.add_argument('game')
    ret.add_argument('dir', help="the waypoint's directory name")
    ret.add_argument('--why', required=True)
    args = parser.parse_args()
    catalogue = Catalogue(args.root)

    if args.cmd == 'list':
        for w in catalogue.all(args.game):
            where = f'level {w.level} at {w.pos}' if w.pos else ''
            replay = 'replayable' if w.recording else 'no recording'
            loads = ' '.join(f"{g}:{'ok' if c['ok'] else 'FAIL'}" for g, c in sorted(w.checks.items()))
            status = f'RETIRED ({w.retired})' if w.retired else loads
            print(f'{w.game:6s} {w.story}#{w.index:<2d} {w.name:28s} from {w.parent:24s} {where:28s} '
                  f'{replay:13s} {w.dir.name}  {status}')
        return 0
    if args.cmd == 'check':
        chosen = [catalogue.get(args.game, args.name)] if args.name else catalogue.all(args.game)
        failed = 0
        for w in chosen:
            if w.retired and not (args.retired or args.name):
                continue
            problems = check(w, args.load, args.run)
            failed += bool(problems)
            print(f"{w.game:6s} {w.name:20s} {w.dir.name:40s} {'; '.join(problems) or 'ok'}", flush=True)
        return 1 if failed else 0
    if args.cmd == 'retire':
        w = catalogue.get(args.game, args.dir)
        w.retired = args.why
        w.save_manifest()
        return 0
    if args.cmd == 'import':
        w = catalogue.add(args.game, args.name, args.story, args.index, 'imported', args.slot, note=args.note)
        print(w.dir)
        return 0
    w = catalogue.get(args.game, args.name)
    print(w.slot_dir if args.cmd == 'path' else (w.dir / MANIFEST).read_text())
    return 0


if __name__ == '__main__':
    sys.exit(main())
