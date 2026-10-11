#!/usr/bin/env python3
"""Play the story in both games, stage by stage, and report how each went.

    autotest.py --out DIR [--games port retail] [--story keep] [--stages NAME ...]
                [--minutes M] [--jobs K] [--chain] [--verify] [--crosscheck]

A stage is one milestone of the story (milestones.py). Each (game, stage) is
a play.py session that starts from the newest good waypoint of the milestone
before it -- the game's own, else the other game's (the saves are one
format, so that also tests one game loading the other's save) -- or from a
new game for the first stage, and stops when the stage's milestone is
reached or after M game minutes. Sessions run K at a time. With --chain each
game plays its stages in order instead, each from the waypoint its previous
stage just made: a whole playthrough.

Each stage is scored:

- reached: the milestone, in how many ticks and seconds;
- save: the waypoint made; the director retires one whose story states are
  wrong (waypoints.py);
- interop: the new save loaded in the other game (waypoints.load_check);
- replay: with --verify, the recording replayed in a fresh host;
- variances: with --crosscheck, crosscheck.py on the new save;
- incidents, from the director's log: Locke's deaths, failed or suspect
  saves, dead ends, calls to the router, a session that broke off.

DIR/autotest.json holds it all, rewritten as each stage finishes;
DIR/report.html lays it out, with links to each session's timeline.
"""
from __future__ import annotations

import argparse
import concurrent.futures
import html
import json
import subprocess
import sys
import threading
import time
from dataclasses import asdict, dataclass, field
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from milestones import LISTS  # noqa: E402
from waypoints import ROOT, Catalogue, Waypoint, load_check  # noqa: E402

GAMES = ('port', 'retail', 'fake')   # fake: the reference host (a smoke run of the harness)
DEATH_GAP = 240                      # ticks between 'dead' notes that make them separate deaths


@dataclass
class Stage:
    game: str
    name: str
    index: int
    start: str = ''                  # GAME:WAYPOINT it started from, 'newgame', or '' when it couldn't
    status: str = 'pending'          # pending, running, reached, missed, broke, skipped
    ticks: int | None = None
    seconds: float | None = None
    waypoint: str = ''               # the waypoint directory it made
    retired: str = ''                # why that save is no good
    interop: dict | None = None      # the other game's load check of the save
    replay: dict | None = None
    variances: list | None = None
    incidents: dict = field(default_factory=dict)
    spend: dict = field(default_factory=dict)
    error: str = ''
    session: str = ''

    @property
    def ok(self) -> bool:
        return (self.status == 'reached' and not self.retired and (self.interop or {}).get('ok', True)
                and not (self.replay or {}).get('diverged') and not self.variances and not self.error)


def other(game: str) -> str | None:
    """The game whose saves this one's interchange with."""
    return {'port': 'retail', 'retail': 'port'}.get(game)


def incidents(bundle: Path) -> dict:
    """Counts of the things worth a look in a session's director log."""
    counts: dict[str, int] = {}
    last_dead = None
    path = bundle / 'director.jsonl'
    for line in path.read_text().splitlines() if path.exists() else ():
        entry = json.loads(line)
        kind = entry.get('kind')
        if kind == 'dead':
            if last_dead is None or entry['t'] - last_dead > DEATH_GAP:
                counts['deaths'] = counts.get('deaths', 0) + 1
            last_dead = entry['t']
        elif kind in ('save failed', 'save suspect', 'dead end', 'router'):
            counts[kind] = counts.get(kind, 0) + 1
    return counts


class AutoTest:
    def __init__(self, args: argparse.Namespace):
        self.args = args
        self.out = args.out.resolve()
        self.catalogue = Catalogue(args.waypoints)
        names = [m.name for m in LISTS[args.story]]
        wanted = args.stages or names
        self.names = names
        self.stages = [Stage(game, name, names.index(name)) for game in args.games for name in wanted]
        self.lock = threading.Lock()
        self.started = time.strftime('%Y-%m-%d %H:%M:%S')

    # -- running --
    def start_point(self, stage: Stage) -> str | None:
        """'' for a new game; GAME:NAME for the newest good waypoint of the
        milestone before; None when there is none in either game."""
        if stage.index == 0:
            return ''
        before = self.names[stage.index - 1]
        for game in filter(None, (stage.game, other(stage.game))):
            try:
                self.catalogue.get(game, before)
                return f'{game}:{before}'
            except KeyError:
                continue
        return None

    def play(self, stage: Stage) -> None:
        start = self.start_point(stage)
        if start is None:
            stage.status, stage.error = 'skipped', f'no waypoint for {self.names[stage.index - 1]!r} in either game'
            self.save()
            return
        stage.start, stage.status = start or 'newgame', 'running'
        session = self.out / stage.game / f'{stage.index:02d}-{stage.name.replace(" ", "-")}'
        stage.session = str(session.relative_to(self.out))
        self.save()
        command = [sys.executable, HERE / 'play.py', stage.game, '--out', session, '--story', self.args.story,
                   '--until', stage.name, '--minutes', str(self.args.minutes),
                   '--waypoints', self.args.waypoints, *(['--from', start] if start else []),
                   *(['--verify'] if self.args.verify else [])]
        session.mkdir(parents=True, exist_ok=True)
        with (session / 'play.out').open('w') as log:
            subprocess.run([str(c) for c in command], stdout=log, stderr=subprocess.STDOUT, check=False)
        self.score(stage, session)
        self.save()

    def play_through(self, stages: list[Stage]) -> None:
        """The stages in order, each from the save the one before made; the
        rest are skipped once one isn't reached."""
        for i, stage in enumerate(stages):
            if i and stages[i - 1].status != 'reached':
                stage.status, stage.error = 'skipped', f'{stages[i - 1].name!r} was not reached'
                continue
            self.play(stage)
        self.save()

    def score(self, stage: Stage, session: Path) -> None:
        path = session / 'summary.json'
        if not path.exists():
            stage.status, stage.error = 'broke', f'no summary: see {session / "play.out"}'
            return
        summary = json.loads(path.read_text())
        stage.ticks, stage.seconds = summary.get('tick'), summary.get('seconds')
        stage.spend, stage.replay = summary.get('spend') or {}, summary.get('replay')
        stage.incidents = incidents(Path(summary.get('bundle', session / 'bundle')))
        if summary.get('error'):
            stage.status, stage.error = 'broke', summary['error']
        reached = stage.name in summary.get('reached', [])
        if not stage.error:
            stage.status = 'reached' if reached else 'missed'
        if not reached or not summary.get('waypoints'):
            return
        waypoint = Waypoint.load(Path(summary['waypoints'][-1]))
        stage.waypoint, stage.retired = waypoint.dir.name, waypoint.retired
        if self.args.interop and other(stage.game):
            try:
                stage.interop = load_check(waypoint, other(stage.game), session / f'interop-{other(stage.game)}')
                waypoint.checks[other(stage.game)] = stage.interop
                waypoint.save_manifest()
            except Exception as error:                   # a load that breaks is a result too
                stage.interop = dict(ok=False, problems=[f'{type(error).__name__}: {error}'])
        if self.args.crosscheck and other(stage.game):
            out = session / 'crosscheck'
            subprocess.run([sys.executable, str(HERE / 'crosscheck.py'), '--slot', str(waypoint.slot_dir),
                            '--out', str(out)], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=False)
            result = out / 'crosscheck.json'
            stage.variances = (json.loads(result.read_text())['variances'] if result.exists()
                               else [dict(kind='crosscheck', what='broke', port=None, retail=None)])

    def run(self) -> int:
        self.out.mkdir(parents=True, exist_ok=True)
        self.save()
        if self.args.chain:
            jobs = [lambda game=game: self.play_through([s for s in self.stages if s.game == game])
                    for game in self.args.games]
        else:
            jobs = [lambda s=s: self.play(s) for s in self.stages]
        with concurrent.futures.ThreadPoolExecutor(max_workers=self.args.jobs) as pool:
            for future in [pool.submit(job) for job in jobs]:
                future.result()
        self.save()
        return 0 if all(s.ok for s in self.stages) else 1

    # -- reporting --
    def save(self) -> None:
        with self.lock:
            data = dict(started=self.started, story=self.args.story, minutes=self.args.minutes,
                        stages=[asdict(s) | {'ok': s.ok} for s in self.stages])
            (self.out / 'autotest.json').write_text(json.dumps(data, indent=1))
            (self.out / 'report.html').write_text(report(data))


def report(data: dict) -> str:
    """The results as a page: a row per stage, a column per game."""
    games = list(dict.fromkeys(s['game'] for s in data['stages']))
    names = list(dict.fromkeys(s['name'] for s in data['stages']))
    cell = {(s['game'], s['name']): s for s in data['stages']}

    def show(s: dict | None) -> str:
        if not s:
            return '<td></td>'
        e = html.escape
        lines = [f"<b class='{e(s['status'])}'>{e(s['status'])}</b>"
                 + (f" in {s['ticks']} ticks, {s['seconds']:.0f} s" if s.get('ticks') and s.get('seconds') else '')]
        lines.append(f"from {e(s['start'] or '?')}")
        if s['waypoint']:
            lines.append(f"save {e(s['waypoint'])}" + (f" <span class='bad'>retired: {e(s['retired'])}</span>"
                                                       if s['retired'] else ''))
        if s.get('interop'):
            i = s['interop']
            lines.append('loads in the other game' if i.get('ok') else
                         f"<span class='bad'>other game: {e('; '.join(i.get('problems', [])))}</span>")
        if s.get('replay'):
            d = s['replay'].get('diverged')
            lines.append('replays exactly' if not d else f"<span class='bad'>replay diverges at {e(json.dumps(d))}</span>")
        if s.get('variances') is not None:
            v = s['variances']
            lines.append('no variances' if not v else
                         f"<span class='bad'>{len(v)} variances: "
                         + e(', '.join(f"{x['kind']} {x['what']}" for x in v[:6])) + '</span>')
        if s['incidents']:
            lines.append(e(', '.join(f'{k} {n}' for k, n in s['incidents'].items())))
        if s['error']:
            lines.append(f"<span class='bad'>{e(s['error'])}</span>")
        if s['session']:
            lines.append(f"<a href='{e(s['session'])}/bundle/timeline.html'>timeline</a>")
        return f"<td class='{'ok' if s['ok'] else 'notok'}'>{'<br>'.join(lines)}</td>"

    rows = ''.join(f"<tr><th>{html.escape(n)}</th>{''.join(show(cell.get((g, n))) for g in games)}</tr>"
                   for n in names)
    done = sum(s['ok'] for s in data['stages'])
    return f"""<!doctype html><html><head><meta charset="utf-8"><title>Playtest autotest</title>
<style>
:root {{ --bg:#fff; --fg:#222; --ok:#e6f4ea; --notok:#fdecea; --bad:#b3261e; --line:#ddd; }}
@media (prefers-color-scheme: dark) {{ :root {{ --bg:#16181c; --fg:#e4e4e4; --ok:#1d3324; --notok:#3a1f1d;
  --bad:#ff8a80; --line:#333; }} }}
body {{ background:var(--bg); color:var(--fg); font:14px/1.4 system-ui, sans-serif; margin:16px; }}
table {{ border-collapse:collapse; width:100%; }}
td, th {{ border:1px solid var(--line); padding:8px; vertical-align:top; text-align:left; }}
td.ok {{ background:var(--ok); }} td.notok {{ background:var(--notok); }}
.bad {{ color:var(--bad); }} a {{ color:inherit; }}
</style></head><body>
<h1>Playtest autotest</h1>
<p>Story <b>{html.escape(data['story'])}</b>, started {html.escape(data['started'])},
{data['minutes']} game minutes a stage at most. {done} of {len(data['stages'])} stages clean.</p>
<table><tr><th>stage</th>{''.join(f'<th>{html.escape(g)}</th>' for g in games)}</tr>{rows}</table>
</body></html>"""


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--games', nargs='+', choices=GAMES, default=['port', 'retail'])
    parser.add_argument('--story', default='keep', choices=sorted(LISTS))
    parser.add_argument('--stages', nargs='+', help='milestone names (default all)')
    parser.add_argument('--minutes', type=float, default=10.0, help='game minutes a stage may take')
    parser.add_argument('--jobs', type=int, default=4)
    parser.add_argument('--chain', action='store_true', help="each game plays the stages in order, from its own saves")
    parser.add_argument('--verify', action='store_true', help='replay each recording')
    parser.add_argument('--no-interop', dest='interop', action='store_false',
                        help="don't load each new save in the other game")
    parser.add_argument('--crosscheck', action='store_true', help='crosscheck each new save')
    parser.add_argument('--waypoints', type=Path, default=ROOT)
    args = parser.parse_args()
    unknown = set(args.stages or []) - {m.name for m in LISTS[args.story]}
    if unknown:
        parser.error(f'no such milestones in {args.story}: {sorted(unknown)}')
    test = AutoTest(args)
    code = test.run()
    print(f'{sum(s.ok for s in test.stages)} of {len(test.stages)} stages clean: {test.out / "report.html"}')
    return code


if __name__ == '__main__':
    sys.exit(main())
