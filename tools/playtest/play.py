#!/usr/bin/env python3
"""Run a Jev playtest session in either game, record it, keep its saves.

    play.py port   --binary build/Revenant --ini ~/RevenantRevisited-testsaves/Revenant.ini \\
                   --out SESSION_DIR [--slot SLOT_DIR] [--milestones keep] [--minutes 20] [--verify]
    play.py retail --out SESSION_DIR [--build EXE] [--slot SLOT_DIR] [--milestones keep] ...
    play.py fake   --out SESSION_DIR                    the reference host (a smoke run)

    --watch        watch it live: the port plays in its window at real speed
                   with captions; either game's frames, captions and Jev's
                   decisions show on a page (watch.py) opened in the browser

The session directory holds the run (host log, save path), the recording
bundle (`bundle/`: session.rvrec, the INI, the start slot, director.jsonl,
the Jev ledger session.rvlog, frames) and the summary. Milestones marked
for saving go to the save library (default ~/RevenantRevisited-testsaves/jev).
`--verify` replays the bundle in a fresh host afterwards and reports the
first divergent tick, if any.

The OpenRouter key is $OPEN_ROUTER_KEY. Docs: docs/playtest/PLAN.md.
"""
from __future__ import annotations

import argparse
import json
import subprocess
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from director import Director, SaveLibrary  # noqa: E402
from host import Host, PortHost, RetailHost  # noqa: E402
from inputs import Bindings  # noqa: E402
from jev import Jev, Ledger, Router  # noqa: E402
from knowledge import Knowledge  # noqa: E402
from milestones import LISTS  # noqa: E402
import watch  # noqa: E402

REPO = HERE.parents[1]
RETAIL_INI = Path('/Users/benjamincooley/RevenantRetailLab/retail-cd/REVENANT/revenant.ini')
DEFAULT_SAVES = Path.home() / 'RevenantRevisited-testsaves' / 'jev'
WATCH_PACE = 24                      # ticks a second while someone watches: real speed


def launch(args: argparse.Namespace, run: Path, purpose: str, watching: bool = False) -> Host:
    where = run / purpose
    frames = run / 'watch'
    if args.game == 'port':
        shown = [f'--control-pace={WATCH_PACE}', f'--watch-frames={frames}'] if watching else []
        return PortHost.launch(args.binary, where, args.ini, name=f'playtest-{purpose}',
                               args=['--clickwalk', *shown, *args.host_arg], headless=not watching)
    if args.game == 'retail':
        shown = ['--watch', str(frames), '--pace', str(WATCH_PACE)] if watching else []
        return RetailHost.launch(where, build=args.build, args=[*shown, *args.host_arg])
    return Host([sys.executable, HERE / 'fakehost.py', *args.host_arg], where)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    parser.add_argument('game', choices=('port', 'retail', 'fake'))
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--binary', type=Path, default=REPO / 'build' / 'Revenant')
    parser.add_argument('--build', type=Path, help='retail: a named build of the executable')
    parser.add_argument('--ini', type=Path, default=Path.home() / 'RevenantRevisited-testsaves' / 'Revenant.ini')
    parser.add_argument('--data', type=Path, default=REPO / 'data', help='the install, for game knowledge')
    parser.add_argument('--slot', type=Path, help='start from this save slot directory')
    parser.add_argument('--seed', type=int, default=1)
    parser.add_argument('--milestones', default='keep', choices=sorted(LISTS))
    parser.add_argument('--minutes', type=float, default=20.0, help='game minutes to play at most')
    parser.add_argument('--saves', type=Path, default=DEFAULT_SAVES)
    parser.add_argument('--verify', action='store_true')
    parser.add_argument('--watch', action='store_true', help='watch live (window and browser page)')
    parser.add_argument('--watch-port', type=int, default=8765)
    parser.add_argument('--host-arg', action='append', default=[], help='passed to the host')
    args = parser.parse_args()

    run = args.out.resolve()
    bundle = run / 'bundle'
    bundle.mkdir(parents=True, exist_ok=True)
    ledger = Ledger(bundle / 'session.rvlog')
    session = f'{args.game}-{time.strftime("%Y%m%d-%H%M%S")}'
    jev, router = Jev(ledger=ledger, seed=args.seed, session=session), Router(ledger=ledger, session=session)
    bindings = Bindings.from_ini(args.ini if args.game == 'port' else RETAIL_INI)

    if args.watch:
        watch.serve(run, args.watch_port)
        url = f'http://127.0.0.1:{args.watch_port}/'
        print(f'watch at {url}', flush=True)
        subprocess.run(['open', url], check=False)
    host = launch(args, run, 'play', watching=args.watch)
    started = time.time()
    info: dict = {}
    try:
        info = host.hello()
        host.start('slot' if args.slot else 'newgame', seed=args.seed, slot=args.slot, record=bundle,
                   ini=args.ini if args.game == 'port' else None, timeout=1800)
        director = Director(host, jev, router, Knowledge.load(args.data), bindings, bundle,
                            list(LISTS[args.milestones]), SaveLibrary(args.saves), game=args.game,
                            watch=run / 'watch' if args.watch else None)
        summary = director.run(max_ticks=round(args.minutes * 60 * 24))
    finally:
        host.close()
    summary.update(game=args.game, build=info.get('build'), seconds=round(time.time() - started, 1),
                   bundle=str(bundle))

    if args.verify:
        with launch(args, run, 'replay') as replay:
            replay.hello()
            summary['replay'] = replay.replay(bundle, verify=True, timeout=3600)
    (run / 'summary.json').write_text(json.dumps(summary, indent=2))
    print(json.dumps(summary, indent=2))
    return 1 if summary.get('replay', {}).get('diverged') else 0


if __name__ == '__main__':
    sys.exit(main())
