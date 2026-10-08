#!/usr/bin/env python3
"""Deterministic auto-battles in the port (docs/gameplay/COMBAT_DOJO.md §9).

    arena.py run SCENARIO [--out DIR] [--repeat N] [--watch] [--port BIN]
    arena.py show TRACE

A scenario (tools/combatarena/scenarios/*.json) starts the game from a save
slot, sets up a fight with console commands (spawn monsters, point them at
each other, take the player out of it) and lets the AI fight it out for a
number of game ticks. The run is deterministic: --seed fixes the random
number generator, --fixedstep advances exactly one 24 Hz tick per frame,
and nothing reads the wall clock. --combattrace records every fighter every
tick (combattrace.h); the run stops at the scenario's last tick.

`--repeat N` runs the scenario N times (in parallel, each from its own
directory) and checks the traces are identical: the proof of determinism.
`--watch` runs one game in a window instead of headless.

Scenario keys:
  name      label
  slot      save slot name, extracted from `slotzip` (default the gameflow
            interop slots, docs/gameflow/SAVE_INTEROP_SLOTS.zip)
  seed      random seed
  ticks     game ticks to run after the play screen starts
  exec      console commands in order; `sleep N` waits N ticks
  ini       optional Revenant.ini overrides {"Section": {"Key": "Value"}}

Environment: REVENANT_DATA_PATH (the install; default the retail lab's).
"""
from __future__ import annotations

import argparse
import concurrent.futures
import hashlib
import json
import os
import shutil
import signal
import subprocess
import sys
import time
import zipfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[1]
DATA = Path(os.environ.get('REVENANT_DATA_PATH', Path.home() / 'RevenantRetailLab' / 'retail-cd' / 'REVENANT'))
DEFAULT_ZIP = REPO / 'docs' / 'gameflow' / 'SAVE_INTEROP_SLOTS.zip'

# The INI a run starts from: the shipped game's options, and paths that
# resolve every rules file through the install's archives (the retail lab's
# loose Resources/ holds port files; COMBAT_DATA.md D0).
BASE_INI = {
    'Paths': {'ClassDefPath': '"."', 'ExileRCPath': '"."', 'ResourcePath': '"."', 'CurMapPath': '"."',
              'BaseMapPath': '"."', 'MoviePath': '".\\Disk2"', 'SaveGamePath': '".\\Save"',
              'ImageryPath': '".\\Imagery"', 'ModulesPath': '".\\Modules"'},
    'Options': {'CombatFace': 'Yes', 'AutoCombat': 'Yes', 'ShowDialog': 'Yes', 'PlaySpeech': 'No',
                'Violence': '4', 'NoCombatResults': 'No'},
    'Modules': {'MainModule': '"Ahkuilon"'},
}


def write_ini(path: Path, overrides: dict) -> None:
    ini = {s: dict(v) for s, v in BASE_INI.items()}
    for section, keys in (overrides or {}).items():
        ini.setdefault(section, {}).update(keys)
    path.write_text(''.join(f'[{s}]\n' + ''.join(f'{k} = {v}\n' for k, v in keys.items()) + '\n'
                            for s, keys in ini.items()))


def extract_slot(zip_path: Path, slot: str, dest: Path) -> None:
    with zipfile.ZipFile(zip_path) as z:
        names = [n for n in z.namelist() if n.startswith(slot + '/') and not n.endswith('/')]
        if not names:
            raise SystemExit(f'no slot {slot!r} in {zip_path}')
        for n in names:
            out = dest / n
            out.parent.mkdir(parents=True, exist_ok=True)
            out.write_bytes(z.read(n))


def exec_line(commands: list[str]) -> str:
    return '; '.join(commands)


def last_tick(trace: Path) -> int:
    if not trace.exists():
        return -1
    with open(trace, 'rb') as f:
        f.seek(0, os.SEEK_END)
        size = f.tell()
        f.seek(max(0, size - 4096))
        tail = f.read().split(b'\n')
    for line in reversed(tail):
        parts = line.split(b'\t')
        if len(parts) >= 2 and parts[1] == b'tick':
            return int(parts[0])
    return -1


def run_once(scn: dict, port: Path, out: Path, watch: bool = False, extra: list[str] = ()) -> dict:
    if out.exists():
        shutil.rmtree(out)
    save = out / 'save'
    (save / 'Save' / 'Single').mkdir(parents=True)
    write_ini(save / 'Revenant.ini', scn.get('ini'))
    extract_slot(Path(scn.get('slotzip', DEFAULT_ZIP)), scn['slot'], save / 'Save' / 'Single')
    # The engine finds its own assets beside the binary: run a copy from there.
    exe = port.parent / f'{port.name}.arena_{out.name}_{os.getpid()}'
    shutil.copy2(port, exe)
    trace = out / 'trace.tsv'
    ticks = int(scn['ticks'])
    args = [str(exe), f'--quickstart={scn["slot"]}', f'--seed={scn["seed"]}', '--fixedstep',
            f'--combattrace={trace}', f'--exec={exec_line(scn["exec"])}',
            f'--max-runtime={scn.get("max_runtime", 600)}']
    if not watch:
        args.insert(1, '--headless')
    args += list(extra)
    env = dict(os.environ, REVENANT_SAVE_PATH=str(save), REVENANT_DATA_PATH=str(DATA))
    (out / 'cmd.txt').write_text(' '.join(repr(a) for a in args) + '\n')
    begin = time.time()
    stop = 'exited'
    with open(out / 'stdout.txt', 'wb') as so:
        proc = subprocess.Popen(args, cwd=out, env=env, stdout=so, stderr=subprocess.STDOUT)
        wall_limit = scn.get('wall_limit_s', 600)
        while proc.poll() is None:
            time.sleep(0.25)
            done = last_tick(trace) >= ticks
            if done or time.time() - begin > wall_limit:
                stop = 'ticks reached' if done else f'wall limit {wall_limit} s'
                proc.send_signal(signal.SIGTERM)
                try:
                    proc.wait(10)
                except subprocess.TimeoutExpired:
                    proc.kill()
                    proc.wait()
    exe.unlink(missing_ok=True)
    # Keep exactly the scenario's ticks: the tail past it depends on when
    # the signal landed.
    lines = trace.read_bytes().split(b'\n') if trace.exists() else []
    kept = [l for l in lines if l and int(l.split(b'\t', 1)[0]) <= ticks]
    trace.write_bytes(b'\n'.join(kept) + b'\n')
    digest = hashlib.sha256(trace.read_bytes()).hexdigest()
    result = dict(out=str(out), rc=proc.returncode, stop=stop, wall_s=round(time.time() - begin, 1),
                  last_tick=last_tick(trace), trace_sha256=digest, summary=summarize(trace))
    (out / 'result.json').write_text(json.dumps(result, indent=1) + '\n')
    return result


def parse(trace: Path):
    for raw in trace.read_text(encoding='cp1252').splitlines():
        parts = raw.split('\t')
        if len(parts) < 2:
            continue
        rec = dict(tick=int(parts[0]), kind=parts[1])
        for kv in parts[2:]:
            k, _, v = kv.partition('=')
            rec[k] = v
        yield rec


def summarize(trace: Path) -> dict:
    """Who fought, the events by kind, health at the end, deaths."""
    if not trace.exists():
        return {}
    fighters, events, deaths = {}, {}, []
    for r in parse(trace):
        if r['kind'] == 'char':
            fighters[f"{r['name']}#{r['id']}"] = dict(first=fighters.get(f"{r['name']}#{r['id']}", {}).get('first', r['tick']),
                                                       last=r['tick'], hp=int(r['hp']), state=r['state'])
        elif r['kind'] == 'event':
            events[r['what']] = events.get(r['what'], 0) + 1
            if r['what'] == 'death':
                deaths.append(f"{r['name']}#{r['id']}@{r['tick']}")
    return dict(fighters=fighters, events=events, deaths=deaths)


def first_difference(a: Path, b: Path) -> str | None:
    la, lb = a.read_bytes().split(b'\n'), b.read_bytes().split(b'\n')
    for i, (x, y) in enumerate(zip(la, lb)):
        if x != y:
            return f'line {i + 1}:\n  {x.decode("cp1252")}\n  {y.decode("cp1252")}'
    if len(la) != len(lb):
        return f'length {len(la)} vs {len(lb)}'
    return None


def cmd_run(a) -> int:
    scn = json.loads(Path(a.scenario).read_text())
    port = Path(a.port).resolve()
    base = Path(a.out).resolve() / scn['name']
    if a.watch:
        r = run_once(scn, port, base / 'watch', watch=True, extra=a.extra)
        print(json.dumps(r, indent=1))
        return 0
    with concurrent.futures.ThreadPoolExecutor(max_workers=a.repeat) as pool:
        results = list(pool.map(lambda i: run_once(scn, port, base / f'run{i + 1}', extra=a.extra), range(a.repeat)))
    for r in results:
        print(f"{Path(r['out']).name}: {r['stop']}, tick {r['last_tick']}, {r['wall_s']} s, "
              f"trace {r['trace_sha256'][:16]}")
    s = results[0]['summary']
    print(f"fighters: {', '.join(f'{k} hp {v['hp']} ({v['state']})' for k, v in s.get('fighters', {}).items()) or 'none'}")
    print(f"events: {s.get('events')}  deaths: {s.get('deaths')}")
    shas = {r['trace_sha256'] for r in results}
    if len(results) > 1:
        if len(shas) == 1:
            print(f'DETERMINISTIC: {len(results)} runs, identical traces')
        else:
            print('NOT DETERMINISTIC:')
            print(first_difference(Path(results[0]['out']) / 'trace.tsv', next(
                Path(r['out']) / 'trace.tsv' for r in results if r['trace_sha256'] != results[0]['trace_sha256'])))
            return 1
    return 0


def cmd_show(a) -> int:
    for r in parse(Path(a.trace)):
        if r['kind'] == 'event' or (r['kind'] == 'char' and r['tick'] % a.every == 0):
            print('\t'.join(f'{k}={v}' for k, v in r.items()))
    return 0


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = p.add_subparsers(dest='cmd', required=True)
    r = sub.add_parser('run')
    r.add_argument('scenario')
    r.add_argument('--out', default=str(REPO / 'build' / 'arena'))
    r.add_argument('--repeat', type=int, default=2)
    r.add_argument('--watch', action='store_true')
    r.add_argument('--port', default=str(REPO / 'build' / 'Revenant'))
    r.add_argument('--extra', action='append', default=[], help='an extra port argument (repeatable), '
                   'e.g. --extra=--combattrace-rng=89:90')
    s = sub.add_parser('show')
    s.add_argument('trace')
    s.add_argument('--every', type=int, default=24)
    a = p.parse_args()
    return cmd_run(a) if a.cmd == 'run' else cmd_show(a)


if __name__ == '__main__':
    sys.exit(main())
