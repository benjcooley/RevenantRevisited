#!/usr/bin/env python3
"""Smoke test of the port as a playtest host (docs/playtest/PLAN.md §6, §10).

    port_smoke.py [--binary build/Revenant] [--work DIR] [--testsaves DIR]
                  [--skip-opening] [--keep]

Two sessions, each recorded and then replayed with --verify:

1. Lockstep, over the control channel (`--control=stdio --clickwalk`): a new
   game; the opening stepped through with the digit-key choices the story
   tests send (testsaves/opening_input.txt, timed in ticks); an observation
   and a frame; a click to walk once control is on; a Quick Save
   (Ctrl+Backspace) exported with `export_slot @last`; quit.
2. Frame timing: a person's session, played headless at the display's rate
   by an --input-script from the post-opening slot.

Each replay must report no divergence. Standard library only. Exit status 0
when every check passes.

With --watch the lockstep session runs in the game window at real speed
(`--control-pace=24`), with captions saying what the driver is doing; its
replay still runs headless and unpaced, and must match.
"""
from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
import time
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]
TICK_HZ = 24


def log(msg: str) -> None:
    print(f'[smoke {time.strftime("%H:%M:%S")}] {msg}', flush=True)


class Failure(RuntimeError):
    pass


def check(cond: bool, what: str) -> None:
    if not cond:
        raise Failure(what)
    log(f'ok: {what}')


# ---- running the port -------------------------------------------------------

class Run:
    """A private run directory: cwd, a SavePath holding the test INI, and a
    copy of the binary beside the original (the engine finds its assets
    from the binary's location)."""

    def __init__(self, work: Path, name: str, binary: Path, ini: Path):
        self.dir = work / name
        shutil.rmtree(self.dir, ignore_errors=True)
        self.save = self.dir / 'save'
        (self.save / 'Save' / 'Single').mkdir(parents=True)
        shutil.copy2(ini, self.save / 'Revenant.ini')
        self.binary = binary.with_name(f'{binary.name}.smoke-{name}')
        shutil.copy2(binary, self.binary)
        self.env = dict(os.environ, REVENANT_SAVE_PATH=str(self.save))

    def add_slot(self, slot: Path) -> None:
        shutil.copytree(slot, self.save / 'Save' / 'Single' / slot.name)

    def run(self, *args: str, timeout: float) -> subprocess.CompletedProcess:
        return subprocess.run([str(self.binary), *args], cwd=self.dir, env=self.env,
                              capture_output=True, text=True, timeout=timeout)

    def close(self) -> None:
        self.binary.unlink(missing_ok=True)


class Host:
    """The control channel: one JSON request a line, one response each."""

    def __init__(self, run: Run, *args: str):
        self.run = run
        self.next_id = 0
        self.stderr = open(run.dir / 'stderr.txt', 'wb')
        self.proc = subprocess.Popen([str(run.binary), '--control=stdio', *args], cwd=run.dir,
                                     env=run.env, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                     stderr=self.stderr)

    def request(self, cmd: str, **fields) -> dict:
        self.next_id += 1
        line = json.dumps({'id': self.next_id, 'cmd': cmd, **fields}) + '\n'
        self.proc.stdin.write(line.encode())
        self.proc.stdin.flush()
        raw = self.proc.stdout.readline()
        if not raw:
            raise Failure(f'the host exited during {cmd} (see {self.run.dir}/revenant.log)')
        resp = json.loads(raw)
        if resp.get('id') != self.next_id:
            raise Failure(f'response {resp.get("id")} out of order for {self.next_id}')
        if not resp.get('ok'):
            raise Failure(f'{cmd}: {resp.get("error")}')
        return resp

    def step(self, ticks: int, events: list[dict] = ()) -> int:
        return self.request('step', ticks=ticks, events=list(events))['t']

    def close(self) -> int:
        if self.proc.poll() is None:
            try:
                self.request('quit')
            except Failure:
                pass
            self.proc.wait(timeout=60)
        self.stderr.close()
        return self.proc.returncode


def replay(work: Path, binary: Path, ini: Path, name: str, bundle: Path) -> dict:
    """--replay=<bundle> --verify, headless; the verdict line and the exit code."""
    run = Run(work, name, binary, ini)
    try:
        started = time.time()
        done = run.run('--headless', f'--replay={bundle}', '--verify', '--max-runtime=1800',
                       timeout=1900)
        verdict = done.stdout.strip().splitlines()[-1] if done.stdout.strip() else ''
        log(f'replay {bundle.name}: "{verdict}" (exit {done.returncode}, {time.time() - started:.0f} s)')
        return {'code': done.returncode, 'verdict': verdict}
    finally:
        run.close()


# ---- recordings ---------------------------------------------------------------

def read_recording(bundle: Path) -> tuple[dict, list[dict]]:
    lines = [json.loads(line) for line in (bundle / 'session.rvrec').read_text().splitlines() if line]
    return lines[0], lines[1:]


def checks(body: list[dict]) -> dict[int, dict]:
    return {line['t']: line['check'] for line in body if 'check' in line}


# ---- the opening's input, in ticks ------------------------------------------

def opening_events(script: str) -> list[dict]:
    """An --input-script's key presses as tick-stamped key events: the input
    simulator fires an event at the first frame whose clock (ticks * 1000 / 24
    ms) has reached the event's time."""
    events, now = [], 0.0
    for command in (c.strip() for c in script.split(';')):
        words = command.split()
        if not words:
            continue
        if words[0] in ('wait', 'pause', 'delay'):
            now += float(words[1])
        elif words[0] in ('key_press', 'keypress'):
            key = words[1].upper()
            hold = float(words[2]) if len(words) > 2 else 200.0
            for at, down in ((now, True), (now + hold, False)):
                tick = -(-int(at * TICK_HZ) // 1000)          # ceil(at * 24 / 1000)
                events.append({'t': tick, 'key': key, 'down': down})
            now += hold
    return events


def step_with(host: Host, until: int, events: list[dict], chunk: int = 240) -> int:
    """Steps to tick `until`, each step delivering the events that fall in it."""
    t = host.request('observe', objects=0)['t']
    while t < until:
        ticks = min(chunk, until - t)
        batch = [dict(e, t=e['t'] - t) for e in events if t <= e['t'] < t + ticks]
        t = host.step(ticks, batch)
    return t


# ---- session 1: lockstep over the control channel ------------------------------

def lockstep_session(args, work: Path) -> Path:
    bundle = work / 'opening.bundle'
    shutil.rmtree(bundle, ignore_errors=True)
    run = Run(work, 'control', args.binary, args.ini)
    view = ['--control-pace=24'] if args.watch else ['--headless']
    host = Host(run, *view, '--clickwalk')

    def caption(text: str) -> None:
        host.request('caption', text=text)

    try:
        hello = host.request('hello')
        check(hello['game'] == 'port' and hello['protocol'] == 1, 'hello answers game port, protocol 1')
        check('clickwalk' in hello['build']['patches'], 'hello reports the clickwalk patch')

        if args.skip_opening:
            start = host.request('start', **{'from': 'slot', 'slot': str(args.testsaves / 'postopen' / 'New Game'),
                                            'seed': 1, 'record': str(bundle)})
        else:
            start = host.request('start', **{'from': 'newgame', 'seed': 1, 'record': str(bundle)})
        check(start['t'] == 0, 'start reaches tick 0')

        t = 0
        if not args.skip_opening:
            script = (args.testsaves / 'opening_input.txt').read_text()
            events = opening_events(script)
            end = max(e['t'] for e in events) + 1
            log(f'the opening: {len(events)} key events over {end} ticks')
            caption('smoke: the opening, answering 3 at each choice')
            started = time.time()
            t = step_with(host, end, events)
            log(f'stepped to t={t} in {time.time() - started:.0f} s')

        # Control comes back once the scenes are over (the story tests allow
        # two and a half minutes after the last choice).
        for _ in range(300):
            obs = host.request('observe', objects=600)
            if obs['control'] and obs['screen'] == 'play':
                break
            t = host.step(24)
        check(obs['control'], f'control is on at t={obs["t"]}')

        frame = run.dir / 'observe.png'
        obs = host.request('observe', frame=str(frame), objects=800, walkgrid=64)
        check(frame.exists() and frame.stat().st_size > 10000, f'observe wrote a frame ({frame})')
        player = obs['player']
        check(player is not None and player['screen'] is not None, 'the player is on screen')
        check(len(obs['walkgrid']['heights']) == obs['walkgrid']['w'] * obs['walkgrid']['h'],
              'the walk grid has w x h cells')
        log(f'player at {player["pos"]} screen {player["screen"]}, mode {player["mode"]}, '
            f'{len(obs["objects"])} objects near')

        caption('smoke: control is back; observing')
        # Click to walk needs Locke out of combat (Enter toggles it).
        if player['mode'] == 'combat':
            t = host.step(4, [{'t': 0, 'key': 'RETURN', 'down': True}, {'t': 1, 'key': 'RETURN', 'down': False}])
            player = host.request('observe', objects=0)['player']
        log(f'mode {player["mode"]}')

        # A point on open ground a little south-east of Locke: try a few until
        # `pick` finds no object under it.
        x0, y0, z0 = player['pos']
        target_pixel = None
        for dx, dy in ((96, 96), (128, 0), (0, 128), (-96, 96), (64, 64)):
            screen = host.request('project', points=[[x0 + dx, y0 + dy, z0]])['screen'][0]
            if not screen:
                continue
            pick = host.request('pick', x=screen[0], y=screen[1])
            if pick['object'] is None:
                target_pixel, target = screen, pick['pos']
                break
        check(target_pixel is not None, f'found open ground to click at {target_pixel}')
        sx, sy = target_pixel
        caption(f'smoke: click to walk at ({sx}, {sy})')
        t = host.step(2, [{'t': 0, 'mouse': [sx, sy], 'buttons': 0}, {'t': 1, 'mouse': [sx, sy], 'buttons': 1}])
        t = host.step(1, [{'t': 0, 'mouse': [sx, sy], 'buttons': 0}])
        t = host.step(72)
        after = host.request('observe', objects=0)['player']['pos']
        moved = abs(after[0] - x0) + abs(after[1] - y0)
        near = abs(after[0] - target[0]) + abs(after[1] - target[1])
        log(f'click at ({sx}, {sy}) -> map {target}; Locke {player["pos"]} -> {after}')
        check(moved > 32 and near < moved, 'click to walk moved Locke toward the point clicked')
        host.request('mark', kind='milestone', name='walked')

        # Quick Save: Ctrl+Backspace, then the save lands a frame or two later.
        caption('smoke: Quick Save (Ctrl+Backspace)')
        t = host.step(24, [{'t': 0, 'key': 'CONTROL', 'down': True}, {'t': 1, 'key': 'BACK', 'down': True},
                           {'t': 2, 'key': 'BACK', 'down': False}, {'t': 3, 'key': 'CONTROL', 'down': False}])
        exported = host.request('export_slot', slot='@last', to=str(work / 'exported-slot'))
        check(exported['slot'].startswith('Quick Save') and (Path(exported['path']) / 'game.sav').exists(),
              f'Quick Save exported as "{exported["slot"]}" to {exported["path"]}')
        host.request('mark', kind='quicksave', slot=exported['slot'])
        t = host.step(24)
    finally:
        code = host.close()
        run.close()
    check(code == 0, 'the host quit cleanly')

    header, body = read_recording(bundle)
    check(header['timing'] == 'lockstep' and header['seed'] == 1, 'the header: lockstep, seed 1')
    check(body[-1].get('end') is True, f'the recording ends at t={body[-1]["t"]}')
    check((bundle / 'Revenant.ini').exists(), 'the bundle holds its INI')
    return bundle


# ---- session 2: frame timing ---------------------------------------------------

def frames_session(args, work: Path) -> Path:
    bundle = work / 'frames.bundle'
    shutil.rmtree(bundle, ignore_errors=True)
    run = Run(work, 'frames', args.binary, args.ini)
    run.add_slot(args.testsaves / 'postopen' / 'New Game')
    script = ('wait 2000; move 420 260 400; wait 300; mouse_click; wait 1500; key_press v; wait 1200; '
              'move 605 280 300; wait 200; mouse_click; wait 1500; key_press enter; wait 800; key_press a; '
              'wait 1500; key_press enter; wait 1000; take_snapshot')
    try:
        done = run.run('--headless', '--quickstart=New Game', '--seed=5', '--clickwalk',
                       f'--record={bundle}', f'--input-script={script}', '--max-runtime=300', timeout=360)
    finally:
        run.close()
    check(done.returncode == 0, 'the interactive session ran')
    header, body = read_recording(bundle)
    frames = [line for line in body if 'dt' in line]
    events = [line for line in body if 'f' in line and 'dt' not in line]
    check(header['timing'] == 'frames' and header['start']['from'] == 'slot', 'the header: frame timing, from a slot')
    check(len(frames) > 100 and len(events) > 5, f'{len(frames)} frames and {len(events)} events recorded')
    check(body[-1].get('end') is True, f'the recording ends at t={body[-1]["t"]}')
    return bundle


# ---- main ------------------------------------------------------------------------

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    parser.add_argument('--binary', type=Path, default=REPO / 'build' / 'Revenant')
    parser.add_argument('--work', type=Path, default=Path(os.environ.get('TMPDIR', '/tmp')) / 'port_smoke')
    parser.add_argument('--testsaves', type=Path, default=Path.home() / 'RevenantRevisited-testsaves')
    parser.add_argument('--skip-opening', action='store_true',
                        help='start the lockstep session from the post-opening slot')
    parser.add_argument('--keep', action='store_true', help='keep the work directory')
    parser.add_argument('--watch', action='store_true',
                        help='play the lockstep session in the game window at 24 ticks a second')
    args = parser.parse_args()
    args.binary = args.binary.resolve()
    args.ini = args.testsaves / 'Revenant.ini'
    work = args.work.resolve()
    work.mkdir(parents=True, exist_ok=True)
    log(f'binary {args.binary}, work {work}')

    try:
        opening = lockstep_session(args, work)
        result = replay(work, args.binary, args.ini, 'replay-opening', opening)
        recorded = checks(read_recording(opening)[1])
        last = max(recorded)
        check(result['code'] == 0 and 'no divergence' in result['verdict'],
              f'the lockstep recording replays with no divergence ({len(recorded)} check lines, '
              f'draws {recorded[last]["draws"]} at t={last})')
        draws = re.search(r'draws (\d+)', result['verdict'])
        check(draws is not None and int(draws.group(1)) == recorded[last]['draws'],
              'RandomDraws is equal in the recording and the replay')

        frames = frames_session(args, work)
        result = replay(work, args.binary, args.ini, 'replay-frames', frames)
        recorded = checks(read_recording(frames)[1])
        last = max(recorded)
        check(result['code'] == 0 and 'no divergence' in result['verdict'],
              f'the frame-timed recording replays with no divergence ({len(recorded)} check lines)')
        draws = re.search(r'draws (\d+)', result['verdict'])
        check(draws is not None and int(draws.group(1)) == recorded[last]['draws'],
              'RandomDraws is equal in the recording and the replay')
    except Failure as failure:
        log(f'FAILED: {failure}')
        return 1
    finally:
        if not args.keep:
            for name in ('control', 'frames', 'replay-opening', 'replay-frames'):
                shutil.rmtree(work / name, ignore_errors=True)
    log('all checks passed')
    return 0


if __name__ == '__main__':
    sys.exit(main())
