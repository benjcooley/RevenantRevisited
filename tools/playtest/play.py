#!/usr/bin/env python3
"""Run a Jev playtest session in either game: record it, log it, add waypoints.

    play.py port   --out SESSION_DIR [--from WAYPOINT | --slot SLOT_DIR] [--story keep] [--watch] [--verify]
    play.py retail --out SESSION_DIR [--build BUILD_JSON] [--from WAYPOINT] [--story keep] [--watch] [--verify]
    play.py fake   --out SESSION_DIR [--story smoke]       the reference host (a smoke run)

--from WAYPOINT  start from a waypoint (waypoints.py list): its save slot, and
                 its story from the milestone after it. GAME:NAME plays one
                 the other game made (port and retail saves are the same
                 format). Without it a session starts a new game.
--watch          watch it live: the port plays in its window at real speed
                 with captions; either game's frames, captions and Jev's
                 decisions show on a page (watch.py) opened in the browser.
--verify         replay the recording in a fresh host afterwards; report the
                 first tick where it differs.

The session directory holds the run (host logs, the game's save path) and
the bundle: the recording (session.rvrec, the INI, the start slot), the
whole gameplay log (observations, decisions, Jev's questions and answers,
frames, the host's logs) and timeline.html, which lays it all out by tick.
Every saving milestone reached adds a waypoint to the catalogue (default
~/RevenantRevisited-testsaves/waypoints) with the recording from the
session's start to it.

The OpenRouter key is $OPEN_ROUTER_KEY. Docs: docs/playtest/PLAN.md.
"""
from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import timeline  # noqa: E402
import watch  # noqa: E402
from director import Director  # noqa: E402
from host import PORT_BINARY, PORT_INI, Host, PortHost, RetailHost, clickwalk_build  # noqa: E402
from inputs import Bindings  # noqa: E402
from jev import Jev, Ledger, Router  # noqa: E402
from knowledge import Knowledge  # noqa: E402
from milestones import LISTS  # noqa: E402
from waypoints import ROOT, Catalogue  # noqa: E402
from world import World  # noqa: E402

REPO = HERE.parents[1]
RETAIL_INI = Path('/Users/benjamincooley/RevenantRetailLab/retail-cd/REVENANT/revenant.ini')
SHIPPED_INI = REPO / 'data' / 'revenant.ini'      # the shipped key bindings, under any INI's own
WATCH_PACE = 24                      # ticks a second while someone watches: real speed
HOST_LOGS = ('host.log', 'revenant.log')


def launch(args: argparse.Namespace, run: Path, purpose: str, watching: bool = False) -> Host:
    where = run / purpose
    frames = run / 'watch'
    if args.game == 'port':
        shown = [f'--control-pace={WATCH_PACE}', f'--watch-frames={frames}'] if watching else []
        return PortHost.launch(args.binary, where, args.ini, name=f'playtest-{purpose}',
                               args=['--clickwalk', *shown, *args.host_arg], headless=not watching)
    if args.game == 'retail':
        shown = ['--watch', str(frames), '--pace', str(WATCH_PACE)] if watching else []
        return RetailHost.launch(where, build=args.build or clickwalk_build(), args=[*shown, *args.host_arg])
    return Host([sys.executable, HERE / 'fakehost.py', *args.host_arg], where)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    parser.add_argument('game', choices=('port', 'retail', 'fake'))
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--binary', type=Path, default=PORT_BINARY)
    parser.add_argument('--build', type=Path, help='retail: a named build (build.json); default the clickwalk build')
    parser.add_argument('--ini', type=Path, default=PORT_INI)
    parser.add_argument('--data', type=Path, default=REPO / 'data', help='the install, for game knowledge')
    parser.add_argument('--from', dest='waypoint', help='start from this waypoint')
    parser.add_argument('--slot', type=Path, help='start from this save slot directory (no story position)')
    parser.add_argument('--story', default=None, choices=sorted(LISTS), help='the milestone list (default keep)')
    parser.add_argument('--seed', type=int, default=1)
    parser.add_argument('--minutes', type=float, default=20.0, help='game minutes to play at most')
    parser.add_argument('--waypoints', type=Path, default=ROOT)
    parser.add_argument('--verify', action='store_true')
    parser.add_argument('--watch', action='store_true', help='watch live (window and browser page)')
    parser.add_argument('--watch-port', type=int, default=8765)
    parser.add_argument('--host-arg', action='append', default=[], help='passed to the host')
    args = parser.parse_args()

    catalogue = Catalogue(args.waypoints)
    story, start, parent, slot = args.story or 'keep', 0, 'newgame', args.slot
    if args.waypoint:
        # 'port:opening' plays a waypoint the other game made: the saves are the same format
        source, _, name = args.waypoint.rpartition(':')
        point = catalogue.get(source or args.game, name)
        story, parent, slot = args.story or point.story, point.name, point.slot_dir
        names = [m.name for m in LISTS[story]]
        # resume after the waypoint's milestone by name; a story edited since keeps its place
        start = names.index(point.name) + 1 if point.name in names else point.index
    elif args.slot:
        parent = f'slot:{args.slot.name}'

    run = args.out.resolve()
    bundle = run / 'bundle'
    bundle.mkdir(parents=True, exist_ok=True)
    ledger = Ledger(bundle / 'session.rvlog')
    session = f'{args.game}-{time.strftime("%Y%m%d-%H%M%S")}'
    jev, router = Jev(ledger=ledger, seed=args.seed, session=session), Router(ledger=ledger, session=session)
    bindings = Bindings.from_ini(RETAIL_INI if args.game == 'retail' else args.ini, defaults=SHIPPED_INI)

    if args.watch:
        watch.serve(run, args.watch_port)
        url = f'http://127.0.0.1:{args.watch_port}/'
        print(f'watch at {url}', flush=True)
        subprocess.run(['open', url], check=False)
    host = launch(args, run, 'play', watching=args.watch)
    started = time.time()
    info: dict = {}
    director: Director | None = None
    summary: dict = {}
    try:
        info = host.hello()
        host.start('slot' if slot else 'newgame', seed=args.seed, slot=slot, record=bundle,
                   ini=args.ini if args.game == 'port' else None, timeout=1800)
        director = Director(host, jev, router, Knowledge.load(args.data), bindings, bundle, list(LISTS[story]),
                            waypoints=catalogue, story=story, start=start, parent=parent, game=args.game,
                            watch=run / 'watch' if args.watch else None,
                            world=World(args.data, slot) if args.game != 'fake' else None)
        summary = director.run(max_ticks=round(args.minutes * 60 * 24))
    finally:
        host.close()
        logs = bundle / 'logs'
        logs.mkdir(exist_ok=True)
        for name in HOST_LOGS:
            if (run / 'play' / name).exists():
                shutil.copy2(run / 'play' / name, logs / name)
        timeline.write(bundle)
    summary.update(game=args.game, build=info.get('build'), story=story, started_from=parent,
                   seconds=round(time.time() - started, 1), bundle=str(bundle),
                   timeline=str(bundle / timeline.REPORT))

    for waypoint, until in director.created:
        catalogue.attach_recording(waypoint, bundle, until)
    if args.verify:
        with launch(args, run, 'replay') as replay:
            replay.hello()
            summary['replay'] = replay.replay(bundle, verify=True, timeout=3600)
    (run / 'summary.json').write_text(json.dumps(summary, indent=2))
    print(json.dumps(summary, indent=2))
    return 1 if (summary.get('replay') or {}).get('diverged') else 0


if __name__ == '__main__':
    sys.exit(main())
