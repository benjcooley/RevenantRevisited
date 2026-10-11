#!/usr/bin/env python3
"""A reference GameHost: the protocol (docs/playtest/PROTOCOL.md §5) over a
toy world, for testing the director, the client and recordings without a
game.

    fakehost.py [--drift-at T]

The world is deterministic: held arrow keys move the player one unit per
tick, a left click on the screen walks the player toward the point it maps
to, and every tick draws three "random numbers". The camera follows the
player at the screen's centre, a pixel to a world unit. Floor ends at
x = 1150 (a wall: no walking past it), and one person, Sardok, stands at
(1000, 1800). `--drift-at T` nudges the player at tick T on every run, so a
replay through it diverges there: the divergence test.
"""
from __future__ import annotations

import argparse
import json
import shutil
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import rec  # noqa: E402

STEPS = {'LEFT': (-1, 0), 'RIGHT': (1, 0), 'UP': (0, -1), 'DOWN': (0, 1)}
WALL_X = 1150
SCALE = 1                            # screen pixels per world unit
SARDOK = {'id': 5, 'name': 'Sardok', 'type': 'Sardok', 'class': 'character', 'enemy': False,
          'pos': [1000, 1800, 0]}


class World:
    def __init__(self, seed: int, drift_at: int | None):
        self.t = 0
        self.pos = [1000 + seed, 2000, 0]
        self.goal: list[int] | None = None
        self.held: set[str] = set()
        self.pointer = [320, 240]
        self.buttons = 0
        self.draws = 0
        self.drift_at = drift_at

    def deliver(self, event: dict) -> None:
        if 'key' in event:
            (self.held.add if event['down'] else self.held.discard)(event['key'])
        elif 'mouse' in event:
            pressed = event['buttons'] & ~self.buttons
            self.pointer, self.buttons = list(event['mouse']), event['buttons']
            if pressed & rec.BUTTON_LEFT:
                self.goal = self.world(*self.pointer)[:2]

    def screen(self, pos) -> list[int]:
        return [320 + (pos[0] - self.pos[0]) * SCALE, 240 + (pos[1] - self.pos[1]) * SCALE]

    def world(self, x: int, y: int) -> list[int]:
        return [self.pos[0] + (x - 320) // SCALE, self.pos[1] + (y - 240) // SCALE, 0]

    def _move(self, dx: int, dy: int) -> None:
        if self.pos[0] + dx < WALL_X:
            self.pos[0] += dx
        self.pos[1] += dy

    def tick(self) -> None:
        for key in self.held & STEPS.keys():
            self._move(*STEPS[key])
        if self.goal:
            self._move(*((self.goal[axis] > self.pos[axis]) - (self.goal[axis] < self.pos[axis]) for axis in (0, 1)))
            if self.pos[:2] == self.goal:
                self.goal = None
        if self.t == self.drift_at:
            self.pos[0] += 1
        self.draws += 3
        self.t += 1

    def check(self) -> dict:
        return {'screen': 'play', 'level': 0, 'pos': list(self.pos), 'health': 100, 'draws': self.draws}


class FakeHost:
    def __init__(self, drift_at: int | None):
        self.drift_at = drift_at
        self.world: World | None = None
        self.recording: rec.Recording | None = None
        self.bundle: Path | None = None

    # -- recording --
    def _record(self, item: rec.Item) -> None:
        if self.recording:
            self.recording.items.append(item)

    def _check(self) -> None:
        self._record(rec.Check(self.world.t, self.world.check()))

    def _flush(self) -> None:
        if self.recording and self.bundle:
            self.recording.save(self.bundle / rec.RECORDING_NAME)

    def _run(self, ticks: int, events: list[dict]) -> None:
        by_tick: dict[int, list[dict]] = {}
        for event in events:
            by_tick.setdefault(event['t'], []).append(event)
        for k in range(ticks):
            for event in by_tick.get(k, []):
                absolute = dict(event, t=self.world.t)
                self._record(rec.parse_item(absolute))
                self.world.deliver(event)
            self.world.tick()
            if self.world.t % rec.TICK_HZ == 0:
                self._check()

    # -- commands --
    def hello(self, req: dict) -> dict:
        return {'game': 'port', 'build': {'sha': 'fake', 'name': 'fakehost', 'patches': []},
                'protocol': 1, 'tick_hz': rec.TICK_HZ, 'screen': list(rec.SCREEN),
                'capabilities': ['observe', 'mark', 'replay', 'export_slot', 'pick']}

    def start(self, req: dict) -> dict:
        self.world = World(req.get('seed', 1), self.drift_at)
        if req.get('record'):
            self.bundle = Path(req['record'])
            self.bundle.mkdir(parents=True, exist_ok=True)
            start = {'from': req.get('from', 'newgame')}
            if start['from'] == 'slot':
                slot = Path(req['slot'])
                shutil.copytree(slot, self.bundle / rec.START_DIR / slot.name, dirs_exist_ok=True)
                start.update(slot=slot.name, slot_sha=rec.slot_sha(slot))
            self.recording = rec.Recording({
                'rvrec': rec.FORMAT_VERSION, 'game': 'port',
                'build': {'sha': 'fake', 'name': 'fakehost', 'patches': []},
                'start': start, 'ini_sha': '', 'seed': req.get('seed', 1), 'tick_hz': rec.TICK_HZ,
                'screen': list(rec.SCREEN), 'timing': 'lockstep', 'by': 'fakehost'})
        return {'t': 0}

    def step(self, req: dict) -> dict:
        self._run(req['ticks'], req.get('events', []))
        return {'t': self.world.t}

    def observe(self, req: dict) -> dict:
        w = self.world
        obs = {'t': w.t, 'screen': 'play', 'control': True,
               'player': {'pos': list(w.pos), 'screen': [320, 240], 'level': 0, 'health': [100, 100]},
               'objects': [dict(SARDOK, screen=w.screen(SARDOK['pos']))], 'messages': []}
        if 'walkgrid' in req:
            r = req['walkgrid'] // 16
            ox, oy = w.pos[0] // 16 - r, w.pos[1] // 16 - r
            heights = [10 if (ox + gx) * 16 < WALL_X else 0 for gy in range(2 * r) for gx in range(2 * r)]
            obs['walkgrid'] = {'origin': [ox, oy], 'cell': 16, 'w': 2 * r, 'h': 2 * r, 'heights': heights}
        return obs

    def pick(self, req: dict) -> dict:
        pos = self.world.world(req['x'], req['y'])
        near = abs(pos[0] - SARDOK['pos'][0]) <= 4 and abs(pos[1] - SARDOK['pos'][1]) <= 4
        return {'pos': pos, 'object': SARDOK['id'] if near else None}

    def mark(self, req: dict) -> dict:
        extra = {k: v for k, v in req.items() if k not in ('id', 'cmd', 'kind')}
        self._record(rec.Mark(self.world.t, req['kind'], extra))
        self._check()
        return {}

    def export_slot(self, req: dict) -> dict:
        target = Path(req['to'])
        target.mkdir(parents=True, exist_ok=True)
        (target / 'game.sav').write_bytes(f'fake slot at tick {self.world.t}'.encode())
        return {'path': str(target), 'slot': 'Quick Save 1' if req['slot'] == '@last' else req['slot']}

    def replay(self, req: dict) -> dict:
        recorded = rec.Recording.load(Path(req['bundle']))
        self.world = World(recorded.header.get('seed', 1), self.drift_at)
        until = req.get('until') or recorded.end or 0
        actual: dict[int, dict] = {}
        events = [e.line() for e in recorded.events]
        index = 0
        while self.world.t < until:
            while index < len(events) and events[index]['t'] == self.world.t:
                self.world.deliver(events[index])
                index += 1
            self.world.tick()
            actual[self.world.t] = self.world.check()
        response = {'t': self.world.t}
        if req.get('verify'):
            divergence = rec.compare_checks(recorded.checks, actual)
            response['diverged'] = None if divergence is None else {
                't': divergence.t, 'fields': {k: list(v) for k, v in divergence.fields.items()}}
        return response

    def quit(self, req: dict) -> dict:
        if self.recording and self.world:
            self._check()
            self._record(rec.End(self.world.t))
            self._flush()
        return {}

    def serve(self) -> None:
        for raw in sys.stdin:
            req = json.loads(raw)
            handler = getattr(self, req.get('cmd', ''), None)
            try:
                if handler is None:
                    raise ValueError(f"unknown command {req.get('cmd')!r}")
                if req['cmd'] not in ('hello', 'start', 'replay') and self.world is None:
                    raise ValueError('start first')
                response = {'id': req.get('id'), 'ok': True, **handler(req)}
            except Exception as error:  # the protocol reports every failure as a response
                response = {'id': req.get('id'), 'ok': False, 'error': str(error)}
            sys.stdout.write(json.dumps(response) + '\n')
            sys.stdout.flush()
            if req.get('cmd') == 'quit':
                return


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    parser.add_argument('--drift-at', type=int)
    FakeHost(parser.parse_args().drift_at).serve()


if __name__ == '__main__':
    main()
