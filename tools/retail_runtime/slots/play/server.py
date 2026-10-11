#!/usr/bin/env python3
r"""The retail host: docs/playtest/PROTOCOL.md §5 on stdin / stdout.

    server.py [--build build.json] [--executable EXE] [--machine software|hud]
              [--work DIR] [--watch DIR] [--pace N]

One JSON request per line in, one response per line out, in order. The
game is the shipped Revenant.exe, or a named build of it (`--build`,
checked by build_contract.py), in the emulator: `playworld.py` boots it,
continues into play, and runs one tick per `step` tick, in lockstep
(nothing moves between requests). `--machine` picks the machine it boots
on (playworld.machine_ini; default `software`).

The log (stderr, one line per entry, `t=<tick> <kind> <detail>`): each
command, each input event delivered, each boundary the host answers, and
the game's own text -- text-bar lines and speech (playworld.py's hooks),
retail's boot-log format strings.

Watching (`--watch DIR`): after every tick, the front buffer as
`DIR/frame.png` (zlib level 1, written to a temporary name and renamed),
and `DIR/caption.txt`, the latest `caption`. `--pace N`: at most N ticks
per wall-clock second, by sleeping between ticks; the game's virtual time
is untouched.

Recording (`start` with `record`): the bundle of PROTOCOL.md §4 --
`session.rvrec` written line by line as events, check lines (every 24
ticks, at marks, at the end) and marks happen; the `Revenant.ini` the
machine booted with; `start/<slot>` for a slot start. `checkpoint` keeps
the recording so far with the machine image, and `restore` brings both
back, so a session that returns to a checkpoint records what was played
from there.

Commands: hello, start, step, observe, project, pick, export_slot, mark,
replay, checkpoint, restore, caption, quit. `pick` answers `pos` (retail's
pointer-to-map conversion); the object under the pointer needs retail's 3D
pick, which draws a frame, so it is not answered.
"""
from __future__ import annotations

import argparse
import datetime
import hashlib
import json
import os
import pickle
import shutil
import struct
import sys
import time
import traceback
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[3]
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(REPO / 'tools' / 'playtest'))
sys.path.insert(0, str(HERE.parents[1]))
import rec  # noqa: E402  (tools/playtest/rec.py: the recording format)
from build_contract import BASELINE  # noqa: E402
from playworld import PlayWorld, RETAIL_INSTALL, machine_ini  # noqa: E402
import observe  # noqa: E402

PROTOCOL_VERSION = 1
CHECK_EVERY = 24
CAPABILITIES = ['observe', 'project', 'pick', 'export_slot', 'mark', 'replay', 'checkpoint',
                'restore', 'caption']
DEFAULT_WORK = HERE / '__pycache__' / 'work'
SCREEN_TO_WORLD = 0x0046dad0      # cdecl (sx, sy, S3DPoint *out, z): map-screen point to world at height z
PICK_HEIGHT = 0x32                # the right-button walk asks at Locke's z + 50 (UpdateMouseMovement 0x0044ee00)


class Recorder:
    """A bundle's session.rvrec, appended as the session goes."""

    def __init__(self, bundle, header, items=()):
        self.bundle = Path(bundle)
        self.header = header
        self.items = list(items)
        self.path = self.bundle / rec.RECORDING_NAME
        rec.Recording(self.header, self.items).save(self.path)

    def add(self, item):
        self.items.append(item)
        with self.path.open('a', encoding='utf-8', newline='\n') as f:
            f.write(json.dumps(rec.item_line(item), separators=(',', ':')) + '\n')


class Host:
    def __init__(self, executable, build, work, machine='software', watch=None, pace=None, log=sys.stderr):
        self.executable = Path(executable)
        self.build_file = build
        self.machine = machine
        self.work = Path(work)
        self.watch = Path(watch) if watch else None
        self.pace = pace
        self.log_stream = log
        self.sha = hashlib.sha256(self.executable.read_bytes()).hexdigest()
        record = json.loads(Path(build).read_text()) if build else None
        changed = record is not None and record.get('patch_manifest', {}).get('kind') != 'unchanged-baseline'
        self.build = dict(sha=self.sha, name=record['name'] if record else 'retail',
                          patches=[record['name']] if changed else [])
        self.ini = machine_ini(machine) or (RETAIL_INSTALL / 'revenant.ini').read_bytes()
        self.world = None
        self.recorder = None
        self.observer = None
        self.caption_text = ''
        self._logged = (0, 0)                  # boundary calls, game text entries already logged
        self._last_tick_wall = None
        if self.watch:
            self.watch.mkdir(parents=True, exist_ok=True)
            self._write_caption()

    # ---- log --------------------------------------------------------------

    def log(self, kind, detail=''):
        tick = self.world.tick if self.world and self.world.tick is not None else 0
        text = ' '.join(str(detail).split('\n'))
        print(f't={tick} {kind} {text}'.rstrip(), file=self.log_stream, flush=True)

    def _log_game(self):
        """Boundary calls and game text since the last log."""
        world = self.world
        calls, texts = self._logged
        for name, detail in world.hud.boundary_calls[calls:]:
            self.log('boundary', f'{name}: {detail}')
        for tick, kind, text in world.game_text[texts:]:
            print(f't={tick} {kind} {text}', file=self.log_stream, flush=True)
        self._logged = (len(world.hud.boundary_calls), len(world.game_text))

    # ---- the world --------------------------------------------------------

    def _boot(self):
        started = time.perf_counter()
        self.world = PlayWorld.boot(self.executable, self.build_file, machine=self.machine)
        self._logged = (len(self.world.hud.boundary_calls), 0)
        self.log('boot', f'{self.machine} machine image ready in {time.perf_counter() - started:.1f} s')

    def _start_world(self, slot_dir, seed):
        self._boot()
        if slot_dir:
            self.world.overlay.place_slot(slot_dir, slot_dir.name)
        started = time.perf_counter()
        self.world.start(slot=slot_dir.name if slot_dir else None, seed=seed)
        self.observer = observe.Observer(self.world)
        self._log_game()
        self.log('play', f'reached in {time.perf_counter() - started:.1f} s: {self.world.start_info["command_line"]}')

    def _require(self):
        if self.world is None or self.world.tick is None:
            raise RuntimeError('start first')
        return self.world

    def _check(self):
        if self.recorder:
            self.recorder.add(rec.Check(self.world.tick, self.observer.check()))

    def _tick(self, events):
        """One tick with its events (protocol dicts without `t`)."""
        world = self.world
        for event in events:
            self.log('input', json.dumps(event, separators=(',', ':')))
            if self.recorder:
                self.recorder.add(rec.parse_item({'t': world.tick, **event}))
        world.step([guest_event(e) for e in events])
        self._log_game()
        if self.watch:
            world.write_frame(self.watch / 'frame.png', level=1)
        if self.pace:
            now = time.monotonic()
            if self._last_tick_wall is not None:
                wait = self._last_tick_wall + 1 / self.pace - now
                if wait > 0:
                    time.sleep(wait)
            self._last_tick_wall = time.monotonic()

    def _write_caption(self):
        target = self.watch / 'caption.txt'
        temporary = target.with_name(f'.caption.{os.getpid()}.tmp')
        temporary.write_text(self.caption_text, encoding='utf-8')
        os.replace(temporary, target)

    # ---- commands ---------------------------------------------------------

    def hello(self, req):
        return dict(game='retail', build=self.build, protocol=PROTOCOL_VERSION, tick_hz=rec.TICK_HZ,
                    screen=list(rec.SCREEN), capabilities=CAPABILITIES, machine=self.machine)

    def start(self, req):
        source = req.get('from', 'newgame')
        if source not in ('newgame', 'slot'):
            raise ValueError(f'start from {source!r}: newgame or slot (a checkpoint is `restore`)')
        if req.get('ini') and Path(req['ini']).read_bytes() != self.ini:
            raise ValueError('The retail host boots with its machine\'s Revenant.ini (read during game '
                             'init, before play); another INI needs a boot image made with it')
        seed = int(req.get('seed', 1))
        slot_dir = Path(req['slot']) if source == 'slot' else None
        self.recorder = None
        self._start_world(slot_dir, seed)
        if req.get('record'):
            bundle = Path(req['record'])
            bundle.mkdir(parents=True, exist_ok=True)
            (bundle / rec.INI_NAME).write_bytes(self.ini)
            start = {'from': source}
            if slot_dir:
                target = bundle / rec.START_DIR / slot_dir.name
                if slot_dir.resolve() != target.resolve():
                    shutil.copytree(slot_dir, target, dirs_exist_ok=True)
                start.update(slot=slot_dir.name, slot_sha=rec.slot_sha(slot_dir))
            header = {'rvrec': rec.FORMAT_VERSION, 'game': 'retail', 'build': self.build, 'start': start,
                      'ini_sha': hashlib.sha256(self.ini).hexdigest(), 'seed': seed, 'tick_hz': rec.TICK_HZ,
                      'screen': list(rec.SCREEN), 'timing': 'lockstep', 'machine': self.machine,
                      'created': datetime.datetime.now(datetime.timezone.utc).strftime('%Y-%m-%dT%H:%M:%SZ'),
                      'by': 'retail play slot'}
            self.recorder = Recorder(bundle, header)
            self._check()
        return dict(t=0)

    def step(self, req):
        self._require()
        ticks = int(req['ticks'])
        if ticks < 1:
            raise ValueError('a step runs at least one tick')
        by_tick = {}
        for event in req.get('events', []):
            if not 0 <= event['t'] < ticks:
                raise ValueError(f'event outside the step: {event}')
            rec.parse_item(dict(event))             # the format's own validation
            by_tick.setdefault(event['t'], []).append({k: v for k, v in event.items() if k != 't'})
        for k in range(ticks):
            self._tick(by_tick.get(k, []))
            if self.world.tick % CHECK_EVERY == 0:
                self._check()
        return dict(t=self.world.tick)

    def observe(self, req):
        self._require()
        return self.observer.observe(objects=req.get('objects', 800), frame=req.get('frame'),
                                     walkgrid=req.get('walkgrid'))

    def project(self, req):
        self._require()
        return dict(screen=[self.observer.project([int(v) for v in point]) for point in req['points']])

    def pick(self, req):
        world = self._require()
        player = self.observer.player()
        if not player:
            raise RuntimeError('no player')
        m = self.observer.m
        x, y = int(req['x']), int(req['y'])
        pane = observe.MAP_PANE
        sx = m.i32(pane + 0x78) - m.i32(pane + 0x04) + x
        sy = m.i32(pane + 0x7c) - m.i32(pane + 0x08) + y
        out = world.scratch
        world.peek(SCREEN_TO_WORLD, (sx & 0xffffffff, sy & 0xffffffff, out,
                                     (self.observer.pos(player)[2] + PICK_HEIGHT) & 0xffffffff))
        return dict(pos=list(struct.unpack('<3i', world.vm.uc.mem_read(out, 12))))

    def export_slot(self, req):
        self._require()
        name, path = self.world.overlay.export_slot(req['slot'], req['to'])
        self.log('export', f'{name} -> {path}')
        return dict(path=str(path), slot=name)

    def mark(self, req):
        self._require()
        extra = {k: v for k, v in req.items() if k not in ('id', 'cmd', 'kind')}
        if self.recorder:
            self.recorder.add(rec.Mark(self.world.tick, req['kind'], extra))
            self._check()
        return {}

    def caption(self, req):
        self.caption_text = str(req.get('text', ''))
        if self.watch:
            self._write_caption()
        return {}

    def replay(self, req):
        bundle = Path(req['bundle'])
        recording = rec.Recording.load(bundle)
        bundle = bundle if bundle.is_dir() else bundle.parent
        header = recording.header
        if header.get('game') != 'retail' or header.get('timing') != 'lockstep':
            raise ValueError('The retail host replays lockstep retail recordings')
        if header.get('ini_sha') != hashlib.sha256(self.ini).hexdigest():
            raise ValueError('The recording was made with another Revenant.ini (machine '
                             f'{header.get("machine", "?")}); this host runs {self.machine}')
        start = header['start']
        slot_dir = None
        if start['from'] == 'slot':
            slot_dir = bundle / rec.START_DIR / start['slot']
            if rec.slot_sha(slot_dir) != start['slot_sha']:
                raise ValueError(f'{slot_dir} does not match the recorded slot_sha')
        self.recorder = None
        self._start_world(slot_dir, header['seed'])
        until = req.get('until') or recording.end or 0
        events = [e for e in recording.events if e.t < until]
        expected = recording.checks
        actual = {0: self.observer.check()}
        index = 0
        while self.world.tick < until:
            batch = []
            while index < len(events) and events[index].t == self.world.tick:
                batch.append({k: v for k, v in events[index].line().items() if k != 't'})
                index += 1
            self._tick(batch)
            if self.world.tick in expected:
                actual[self.world.tick] = self.observer.check()
        response = dict(t=self.world.tick, build_matches=header['build'].get('sha') == self.sha)
        if req.get('verify'):
            divergence = rec.compare_checks(expected, actual)
            response['diverged'] = None if divergence is None else dict(
                t=divergence.t, fields={k: list(v) for k, v in divergence.fields.items()})
            response['checks_compared'] = len(expected.keys() & actual.keys())
        return response

    def checkpoint(self, req):
        self._require()
        path = self._checkpoint_path(req['name'])
        path.parent.mkdir(parents=True, exist_ok=True)
        started = time.perf_counter()
        size = self.world.save(path)
        session = dict(sha=self.sha, machine=self.machine, observer=self.observer.state(),
                       recorder=None if self.recorder is None else dict(
                           bundle=str(self.recorder.bundle), header=self.recorder.header,
                           items=self.recorder.items))
        path.with_suffix('.session').write_bytes(pickle.dumps(session))
        return dict(t=self.world.tick, bytes=size, seconds=round(time.perf_counter() - started, 2))

    def restore(self, req):
        path = self._checkpoint_path(req['name'])
        session = pickle.loads(path.with_suffix('.session').read_bytes())
        if session['sha'] != self.sha or session['machine'] != self.machine:
            raise ValueError('The checkpoint was made with another executable or machine')
        started = time.perf_counter()
        self.world = PlayWorld.load(self.executable, path, self.build_file, machine=self.machine)
        self.observer = observe.Observer(self.world, session['observer'])
        self._logged = (len(self.world.hud.boundary_calls), len(self.world.game_text))
        saved = session['recorder']
        self.recorder = None if saved is None else Recorder(saved['bundle'], saved['header'], saved['items'])
        return dict(t=self.world.tick, seconds=round(time.perf_counter() - started, 2))

    def _checkpoint_path(self, name):
        if not name or any(c in name for c in '/\\:') or name.startswith('.'):
            raise ValueError(f'checkpoint name {name!r}')
        return self.work / 'checkpoints' / f'{name}.image'

    def quit(self, req):
        if self.recorder and self.world and self.world.tick is not None:
            self._check()
            self.recorder.add(rec.End(self.world.tick))
        return {}

    def serve(self, stdin, stdout):
        for raw in stdin:
            if not raw.strip():
                continue
            req = None
            try:
                req = json.loads(raw)
                cmd = req.get('cmd', '')
                handler = getattr(self, cmd) if cmd in COMMANDS else None
                if handler is None:
                    raise ValueError(f'unknown command {cmd!r}')
                self.log('cmd', json.dumps({k: v for k, v in req.items() if k not in ('events',)},
                                           separators=(',', ':')))
                response = {'id': req.get('id'), 'ok': True, **handler(req)}
            except Exception as error:      # every failure is a response (PROTOCOL.md §5)
                self.log('error', traceback.format_exc().replace('\n', ' | '))
                response = {'id': (req or {}).get('id'), 'ok': False, 'error': f'{type(error).__name__}: {error}'}
            stdout.write(json.dumps(response, separators=(',', ':')) + '\n')
            stdout.flush()
            if req and req.get('cmd') == 'quit':
                return


COMMANDS = ('hello', 'start', 'step', 'observe', 'project', 'pick', 'export_slot', 'mark', 'replay',
            'checkpoint', 'restore', 'caption', 'quit')


def guest_event(event):
    """A protocol event for userinput.deliver: key names as VK codes."""
    if 'key' in event:
        return {'key': rec.vk_code(event['key']), 'down': event['down']}
    return event


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    parser.add_argument('--build', type=Path, help='a named build\'s build.json (build_contract.py)')
    parser.add_argument('--executable', type=Path, help='default: the build\'s, else the retail baseline')
    parser.add_argument('--machine', default='software', choices=('software', 'hud'))
    parser.add_argument('--work', type=Path, default=DEFAULT_WORK, help='checkpoint directory')
    parser.add_argument('--watch', type=Path, help='write frame.png and caption.txt here every tick')
    parser.add_argument('--pace', type=float, help='at most this many ticks per wall-clock second')
    args = parser.parse_args()
    executable = args.executable or (Path(json.loads(args.build.read_text())['executable'])
                                     if args.build else BASELINE)
    host = Host(executable, args.build, args.work, args.machine, args.watch, args.pace)
    host.serve(sys.stdin, sys.stdout)


if __name__ == '__main__':
    main()
