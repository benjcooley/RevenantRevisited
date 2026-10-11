#!/usr/bin/env python3
"""Waypoints: save games at story points that sessions play from.

    waypoints.py list [GAME]                       every waypoint, in story order
    waypoints.py show GAME NAME                    one waypoint's manifest
    waypoints.py path GAME NAME                    its slot directory (play.py --slot)
    waypoints.py import GAME NAME SLOT_DIR --story keep --index N [--note TEXT]
                                                   an existing save slot as a waypoint

A waypoint is a save slot at a point in a story (a milestone list in
milestones.py), in one game, with what produced it: its parent waypoint
(or a new game), the build, the session tick, the player's state, and the
recording bundle that replays the way from the parent to it. Sessions start
from a waypoint (play.py --from NAME) and every saving milestone they reach
adds the next one, so the catalogue grows along the story, in both games.

Nothing is overwritten: each waypoint gets its own directory,
`<root>/<game>/<name>--<created>/`, and a name finds the newest. The root
is ~/RevenantRevisited-testsaves/waypoints.

    <waypoint>/waypoint.json     the manifest
    <waypoint>/slot/<slot>/      the save: game.sav, CurMap/, ss.bmp
    <waypoint>/recording/        the bundle from the parent (session.rvrec, Revenant.ini, start/)
"""
from __future__ import annotations

import argparse
import json
import re
import shutil
import sys
import time
from dataclasses import asdict, dataclass, field
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import rec  # noqa: E402

ROOT = Path.home() / 'RevenantRevisited-testsaves' / 'waypoints'
MANIFEST = 'waypoint.json'


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
        matches = [w for w in self.all(game) if w.name == name or w.dir.name == name]
        if not matches:
            known = ', '.join(sorted({w.name for w in self.all(game)})) or 'none'
            raise KeyError(f'no {game} waypoint named {name!r} (known: {known})')
        return max(matches, key=lambda w: w.created)

    def add(self, game: str, name: str, story: str, index: int, parent: str, slot_dir: Path,
            obs: dict | None = None, build: dict | None = None, tick: int | None = None,
            note: str = '', slot_name: str | None = None) -> Waypoint:
        """A new waypoint from an exported (or existing) slot directory,
        copied in under `slot_name` (default: the directory's name)."""
        created = time.strftime('%Y%m%d-%H%M%S')
        directory = self.root / game / f'{slug(name)}--{created}'
        n = 1
        while directory.exists():                            # two in one second
            n += 1
            directory = self.root / game / f'{slug(name)}--{created}-{n}'
        slot_dir = Path(slot_dir)
        slot_name = slot_name or slot_dir.name
        shutil.copytree(slot_dir, directory / 'slot' / slot_name)
        player = (obs or {}).get('player', {})
        waypoint = Waypoint(
            game=game, name=name, story=story, index=index, parent=parent, slot=slot_name,
            slot_sha=rec.slot_sha(slot_dir), created=created, build=build or {}, tick=tick,
            level=player.get('level'), pos=player.get('pos'),
            player={k: player.get(k) for k in ('health', 'mana', 'fatigue', 'xp', 'gold', 'skills', 'weapon')
                    if player.get(k) is not None},
            note=note, dir=directory)
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
    args = parser.parse_args()
    catalogue = Catalogue(args.root)

    if args.cmd == 'list':
        for w in catalogue.all(args.game):
            where = f'level {w.level} at {w.pos}' if w.pos else ''
            replay = 'replayable' if w.recording else 'no recording'
            print(f'{w.game:6s} {w.story}#{w.index:<2d} {w.name:28s} from {w.parent:24s} {where:28s} '
                  f'{replay:13s} {w.dir.name}')
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
