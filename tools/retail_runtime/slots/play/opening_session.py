#!/usr/bin/env python3
"""A recorded retail session through the real control channel, end to end:
a new game's opening with its dialog choices answered by digit keys, Locke
walked with the arrow keys once control returns, a quick save
(Ctrl+Backspace) exported, then the recording replayed with `verify`.

    opening_session.py OUT [--keys 3,3] [--build build.json] [--machine software]

OUT gets `bundle/` (the recording), `frames/` (milestones), `slot/` (the
exported quick save), `host.log` / `replay/host.log` (the servers' logs)
and `summary.json` (ticks, rates, the replay's verdict). It drives
server.py with tools/playtest/host.py's RetailHost, as the director does.
"""
from __future__ import annotations

import argparse
import json
import shutil
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[2] / 'playtest'))
import host as hosts  # noqa: E402  (tools/playtest/host.py)

SERVER = HERE / 'server.py'
CHUNK = 12                     # ticks per step while waiting for something
OPENING_LIMIT = 24 * 60 * 15   # ticks: give up after 15 game minutes
FRAME_EVERY = 24 * 20          # a milestone frame every 20 game seconds of the opening


def typed(key, t=0):
    """A digit or letter as Windows delivers it: down, the character, up."""
    return [{'t': t, 'key': key, 'down': True}, {'t': t, 'char': ord(key)}, {'t': t + 1, 'key': key, 'down': False}]


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    parser.add_argument('out', type=Path)
    parser.add_argument('--keys', default='3', help='digits for the choice menus, in turn (the last repeats)')
    parser.add_argument('--build', type=Path)
    parser.add_argument('--machine', default='software')
    args = parser.parse_args()
    out = args.out.resolve()
    if out.exists():
        shutil.rmtree(out)
    (out / 'frames').mkdir(parents=True)
    keys = args.keys.split(',')
    server_args = ['--machine', args.machine]
    summary = {}

    def launch(run_dir):
        return hosts.RetailHost.launch(run_dir, build=args.build, args=server_args, server=SERVER)

    host = launch(out)
    summary['hello'] = host.hello()
    started = time.perf_counter()
    host.start('newgame', seed=1, record=out / 'bundle', timeout=1200)
    summary['start_seconds'] = round(time.perf_counter() - started, 1)

    def frame(name):
        host.observe(frame=out / 'frames' / f'{host.tick:05d}_{name}.png')

    frame('start')
    stepping = time.perf_counter()
    choices, next_frame = [], FRAME_EVERY
    while host.tick < OPENING_LIMIT:
        o = host.observe()
        dialog = o.get('dialog')
        if dialog and dialog.get('choices'):
            key = keys[min(len(choices), len(keys) - 1)]
            choices.append(dict(t=host.tick, choices=dialog['choices'], key=key))
            frame(f'choice{len(choices)}')
            host.step(2, typed(key))
            continue
        if o['control'] and host.tick > 24:
            break
        if host.tick >= next_frame:
            frame('opening')
            next_frame += FRAME_EVERY
        host.step(CHUNK)
    summary['opening'] = dict(control_at=host.tick, choices=choices)
    frame('control')
    before = host.observe()['player']['pos']
    host.step(48, [{'t': 0, 'key': 'RIGHT', 'down': True}, {'t': 47, 'key': 'RIGHT', 'down': False}])
    host.step(12)
    after = host.observe()['player']['pos']
    frame('walked')
    summary['walk'] = dict(before=before, after=after)
    host.step(4, [{'t': 0, 'key': 'CONTROL', 'down': True}, {'t': 1, 'key': 'BACK', 'down': True},
                  {'t': 2, 'key': 'BACK', 'down': False}, {'t': 3, 'key': 'CONTROL', 'down': False}])
    host.step(24)
    frame('saved')
    host.mark('quicksave')
    exported = host.request('export_slot', slot='@last', to=str(out / 'slot'))
    summary['quicksave'] = dict(slot=exported['slot'], path=exported['path'],
                                files=sorted(p.relative_to(out / 'slot').as_posix()
                                             for p in (out / 'slot').rglob('*') if p.is_file()))
    summary['session_ticks'] = host.tick
    summary['ticks_per_second'] = round(host.tick / (time.perf_counter() - stepping), 2)
    host.close()

    replay_dir = out / 'replay'
    replayer = launch(replay_dir)
    replayer.hello()
    started = time.perf_counter()
    verdict = replayer.replay(out / 'bundle', verify=True, timeout=7200)
    summary['replay'] = dict(seconds=round(time.perf_counter() - started, 1),
                             **{k: v for k, v in verdict.items() if k not in ('id', 'ok')})
    replayer.close()
    (out / 'summary.json').write_text(json.dumps(summary, indent=2) + '\n')
    print(json.dumps(summary, indent=2))
    return 0 if verdict.get('diverged') is None else 1


if __name__ == '__main__':
    sys.exit(main())
