"""The director's side of the GameHost protocol (docs/playtest/PROTOCOL.md §5).

    with PortHost.launch(binary, run_dir, ini) as host:
        host.hello()
        host.start('newgame', seed=1, record=bundle)
        host.step(plan.ticks(), plan.events())
        obs = host.observe(frame=run_dir / 'f.png')

A host is a child process speaking JSON Lines on stdio: one request, one
response, in order. Its stderr goes to a log file in the run directory;
when it dies, the error carries the log's tail.
"""
from __future__ import annotations

import json
import os
import queue
import shutil
import subprocess
import threading
import uuid
from pathlib import Path
from typing import Any, Sequence

PROTOCOL_VERSION = 1
EMULATOR_PYTHON = Path('/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python')
REPO = Path(__file__).resolve().parents[2]


class HostError(RuntimeError):
    pass


class Host:
    def __init__(self, args: Sequence[str], cwd: Path, env: dict[str, str] | None = None,
                 log: Path | None = None, timeout: float = 600.0):
        self.args = [str(a) for a in args]
        self.cwd = Path(cwd)
        self.log = Path(log) if log else self.cwd / 'host.log'
        self.timeout = timeout
        self.tick = 0
        self.info: dict = {}
        self._next_id = 0
        self._lines: queue.Queue[str | None] = queue.Queue()
        self.cwd.mkdir(parents=True, exist_ok=True)
        self._log_file = self.log.open('ab')
        self.proc = subprocess.Popen(self.args, cwd=self.cwd, env=env, stdin=subprocess.PIPE,
                                     stdout=subprocess.PIPE, stderr=self._log_file)
        self._reader = threading.Thread(target=self._read, daemon=True)
        self._reader.start()

    # -- transport --
    def _read(self) -> None:
        assert self.proc.stdout
        for raw in self.proc.stdout:
            self._lines.put(raw.decode('utf-8', 'replace'))
        self._lines.put(None)

    def request(self, cmd: str, timeout: float | None = None, **fields: Any) -> dict:
        self._next_id += 1
        ident = self._next_id
        line = json.dumps({'id': ident, 'cmd': cmd, **fields}, separators=(',', ':')) + '\n'
        if self.proc.poll() is not None:
            raise HostError(f'host exited ({self.proc.returncode}) before {cmd}{self._tail()}')
        assert self.proc.stdin
        try:
            self.proc.stdin.write(line.encode('utf-8'))
            self.proc.stdin.flush()
        except BrokenPipeError:
            raise HostError(f'host closed its input before {cmd}{self._tail()}') from None
        while True:
            try:
                raw = self._lines.get(timeout=timeout or self.timeout)
            except queue.Empty:
                raise HostError(f'no answer to {cmd} within {timeout or self.timeout:.0f} s{self._tail()}') from None
            if raw is None:
                raise HostError(f'host exited during {cmd}{self._tail()}')
            try:
                response = json.loads(raw)
            except json.JSONDecodeError:
                raise HostError(f'host wrote a line that is not JSON: {raw[:200]!r}') from None
            if response.get('id') != ident:
                raise HostError(f'response {response.get("id")} out of order for request {ident}')
            if not response.get('ok'):
                raise HostError(f'{cmd}: {response.get("error", "failed")}')
            return response

    def _tail(self, lines: int = 20) -> str:
        try:
            text = self.log.read_text('utf-8', 'replace').splitlines()[-lines:]
        except OSError:
            return ''
        return '\n  host log:\n    ' + '\n    '.join(text) if text else ''

    # -- the protocol --
    def hello(self) -> dict:
        self.info = self.request('hello')
        if self.info.get('protocol') != PROTOCOL_VERSION:
            raise HostError(f'host speaks protocol {self.info.get("protocol")}, not {PROTOCOL_VERSION}')
        return self.info

    def start(self, start_from: str = 'newgame', seed: int = 1, slot: Path | None = None,
              record: Path | None = None, ini: Path | None = None, timeout: float | None = None) -> dict:
        fields: dict[str, Any] = {'from': start_from, 'seed': seed}
        if slot:
            fields['slot'] = str(slot)
        if record:
            fields['record'] = str(record)
        if ini:
            fields['ini'] = str(ini)
        response = self.request('start', timeout=timeout, **fields)
        self.tick = response.get('t', 0)
        return response

    def step(self, ticks: int, events: Sequence[dict] = (), timeout: float | None = None) -> int:
        if ticks < 1:
            raise ValueError('a step runs at least one tick')
        for event in events:
            if not 0 <= event['t'] < ticks:
                raise ValueError(f'event outside the step: {event}')
        self.tick = self.request('step', timeout=timeout, ticks=ticks, events=list(events))['t']
        return self.tick

    def observe(self, frame: Path | None = None, objects: int | None = None,
                walkgrid: int | None = None) -> dict:
        fields: dict[str, Any] = {}
        if frame:
            fields['frame'] = str(frame)
        if objects is not None:
            fields['objects'] = objects
        if walkgrid is not None:
            fields['walkgrid'] = walkgrid
        response = self.request('observe', **fields)
        response.pop('id', None)
        response.pop('ok', None)
        return response

    def project(self, points: Sequence[Sequence[int]]) -> list[list[int] | None]:
        return self.request('project', points=[list(p) for p in points])['screen']

    def pick(self, x: int, y: int) -> dict:
        return self.request('pick', x=x, y=y)

    def export_slot(self, slot: str, to: Path) -> dict:
        """`slot` by name, or '@last' for the slot the game wrote last;
        returns {'path', 'slot'}."""
        response = self.request('export_slot', slot=slot, to=str(to))
        return {'path': Path(response['path']), 'slot': response.get('slot', slot)}

    def mark(self, kind: str, **fields: Any) -> None:
        self.request('mark', kind=kind, **fields)

    def replay(self, bundle: Path, verify: bool = True, until: int | None = None,
               timeout: float | None = None) -> dict:
        fields: dict[str, Any] = {'bundle': str(bundle), 'verify': verify}
        if until is not None:
            fields['until'] = until
        response = self.request('replay', timeout=timeout, **fields)
        return {k: v for k, v in response.items() if k not in ('id', 'ok')}

    def checkpoint(self, name: str) -> None:
        self.request('checkpoint', name=name)

    def restore(self, name: str) -> None:
        self.request('restore', name=name)

    def capable(self, cmd: str) -> bool:
        return cmd in self.info.get('capabilities', ())

    # -- lifetime --
    def close(self, grace: float = 30.0) -> int:
        if self.proc.poll() is None:
            try:
                self.request('quit', timeout=grace)
            except HostError:
                pass
            try:
                self.proc.wait(timeout=grace)
            except subprocess.TimeoutExpired:
                self.proc.kill()
                self.proc.wait()
        self._reader.join(timeout=grace)
        for stream in (self.proc.stdin, self.proc.stdout):
            if stream:
                stream.close()
        self._log_file.close()
        return self.proc.returncode

    def __enter__(self) -> Host:
        return self

    def __exit__(self, *exc: object) -> None:
        self.close()


class PortHost(Host):
    """RevenantRevisited under `--control=stdio`. The game finds its engine
    assets beside its executable, so a private copy of the binary goes next
    to the original for the run, as the story tests do."""

    def __init__(self, args: Sequence[str], cwd: Path, env: dict[str, str], binary_copy: Path):
        self.binary_copy = binary_copy
        super().__init__(args, cwd, env)

    @classmethod
    def launch(cls, binary: Path, run_dir: Path, ini: Path, args: Sequence[str] = (),
               name: str = 'playtest', data: Path | None = None, headless: bool = True) -> PortHost:
        run_dir, binary = Path(run_dir).resolve(), Path(binary).resolve()
        save = run_dir / 'save'
        save.mkdir(parents=True, exist_ok=True)
        shutil.copy2(ini, save / 'Revenant.ini')
        # A private copy per run: a rebuild, or another run, never writes
        # the file a running game is executing.
        copy = binary.with_name(f'{binary.name}.{name}-{uuid.uuid4().hex[:8]}')
        shutil.copy2(binary, copy)
        env = dict(os.environ, REVENANT_SAVE_PATH=str(save))
        if data:
            env['REVENANT_DATA_PATH'] = str(data)
        window = ['--headless'] if headless else []
        return cls([copy, *window, '--control=stdio', *args], run_dir, env, binary_copy=copy)

    def close(self, grace: float = 30.0) -> int:
        code = super().close(grace)
        self.binary_copy.unlink(missing_ok=True)
        return code


class RetailHost(Host):
    """The shipped game in the emulator's play slot."""

    SERVER = REPO / 'tools' / 'retail_runtime' / 'slots' / 'play' / 'server.py'

    @classmethod
    def launch(cls, run_dir: Path, build: Path | None = None, args: Sequence[str] = (),
               python: Path = EMULATOR_PYTHON, server: Path | None = None) -> RetailHost:
        command = [python, server or cls.SERVER, *(['--build', build] if build else []), *args]
        return cls(command, run_dir)
