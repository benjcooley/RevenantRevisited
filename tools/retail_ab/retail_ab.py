#!/usr/bin/env python3
"""Gameflow A/B against the shipped game's own code (docs/gameflow/RETAIL_AB.md).

    retail_ab.py TARGET [--out DIR] [--all] [--port BIN] [--data DIR] [--jobs N] [--case S]

One command per target. It gathers every case, runs retail's original
function in the in-process emulator (persistent fixture processes, --jobs of
them at once; a fixture checkpoints after setup and restores per case) and
the port's function (`Revenant --retail-ab=<target>`, one process), compares
the two dumps and prints the first difference per case (`--all`: every
difference). The report (DIR/report.json) records the retail build hash, the
port commit, the case set's hash, every difference and the timing; both
dumps stay beside it (<target>.retail.jsonl, <target>.port.jsonl).

Targets:
  script-parse   TScriptProto::ParseScript 0x00494e20 (+ TScript::Jump
                 0x00493fa0 on every label) over every shipped script and the
                 edge cases in tools/retail_ab/cases/script_parse/.
  script-step    TScript::Continue 0x004933d0 over every trigger block and
                 label of the same scripts (+ cases/script_step/), commands
                 as boundaries: the lines each block runs.
  trigger-test   the trigger test 0x004927b0 in a fixture world.
  say-duration   TCharacter::Say 0x004d0610: how long a line holds.
  dialog-layout  the dialog pane and entry pulses: positions and slides.

Environment / defaults:
  RETAIL_RUNTIME  the emulator (main checkout tools/retail_runtime)
  RETAIL_PY       its Python (the retail-asm venv)
  REVENANT_DATA_PATH  the retail install (Modules/, resources.rvr, Resources/)
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import queue
import re
import subprocess
import sys
import threading
import time
import zipfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[1]
MAIN_CHECKOUT = Path('/Users/benjamincooley/projects/RevenantRevisited/RevenantRevisited')
RUNTIME = Path(os.environ.get('RETAIL_RUNTIME', MAIN_CHECKOUT / 'tools' / 'retail_runtime'))
RETAIL_PY = Path(os.environ.get('RETAIL_PY', '/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python'))
RETAIL_EXE = RUNTIME.parents[1] / 'recon' / 'retail_asm' / 'baseline' / 'Revenant.rebuilt.exe'
DATA = Path(os.environ.get('REVENANT_DATA_PATH', Path.home() / 'RevenantRetailLab' / 'retail-cd' / 'REVENANT'))
TEXT = 'cp1252'


# =====================================================================
# Cases
# =====================================================================

def script_cases(data: Path, workdir: Path, edges: str = 'script_parse') -> list[dict]:
    """Every shipped script file, plus the edge cases kept in this tree
    (tools/retail_ab/cases/<edges>/).

    Shipped: each .s in the module archive, master.s and multiplayer.s in
    resources.rvr, the loose Resources/master.s, and the demo module's
    scripts. Written out under workdir so both sides read the same bytes.
    """
    cases = []
    out = workdir / 'scripts'
    out.mkdir(parents=True, exist_ok=True)

    def add(name, blob, filename, origin):
        path = out / name
        path.write_bytes(blob)
        cases.append(dict(name=name, path=str(path), filename=filename, origin=origin,
                          sha256=hashlib.sha256(blob).hexdigest()))

    for archive, prefix in ((data / 'Modules' / 'Ahkuilon.rvm', 'module'), (data / 'resources.rvr', 'rvr')):
        with zipfile.ZipFile(archive) as z:
            for n in sorted(z.namelist()):
                if n.lower().endswith('.s') and '/' not in n:
                    add(f'{prefix}.{n.lower()}', z.read(n), n, f'{archive.name}:{n}')
    for path, prefix in ([(p, 'disk.resources') for p in sorted((data / 'Resources').glob('*.s'))] +
                         [(p, 'demo') for p in sorted((data / 'Modules' / 'Demo').glob('*.s'))]):
        add(f'{prefix}.{path.name.lower()}', path.read_bytes(), path.name, str(path))
    for path in sorted((HERE / 'cases' / edges).glob('*.s')):
        add(f'edge.{path.name}', path.read_bytes(), path.name, str(path.relative_to(REPO)))
    return cases


# =====================================================================
# Running both sides
# =====================================================================

def run_retail(fixture: Path, cases: list[dict], jobs: int = 1, split=None, merge=None) -> tuple[dict, dict]:
    """Every case through the fixture, in `jobs` processes at once.

    A target with `split` hands each process a share of a case (`split(case,
    n)` -> n sub-cases) and `merge(case, responses)` puts the shares back
    together; otherwise a case goes whole to the next free process.
    """
    started = time.perf_counter()
    parts = []
    for case in cases:
        subs = split(case, jobs) if split and jobs > 1 else [case]
        parts += [(case['name'], i, sub) for i, sub in enumerate(subs)]
    order = queue.Queue()
    for part in parts:
        order.put(part)
    responses: dict[str, dict[int, dict]] = {}
    hellos = []
    lock = threading.Lock()

    def worker():
        proc = subprocess.Popen([str(RETAIL_PY), str(fixture), str(RETAIL_EXE), '--serve'],
                                stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)
        hellos.append(json.loads(proc.stdout.readline()))
        while True:
            try:
                name, index, sub = order.get_nowait()
            except queue.Empty:
                break
            proc.stdin.write(json.dumps(dict(id=name, case=sub)) + '\n')
            proc.stdin.flush()
            response = json.loads(proc.stdout.readline())
            with lock:
                responses.setdefault(name, {})[index] = response
        proc.stdin.close()
        proc.wait(timeout=60)

    threads = [threading.Thread(target=worker) for _ in range(max(1, min(jobs, len(parts))))]
    for t in threads:
        t.start()
    for t in threads:
        t.join()
    results = {}
    for case in cases:
        got = [responses[case['name']][i] for i in sorted(responses.get(case['name'], {}))]
        results[case['name']] = merge(case, got) if split and len(got) > 1 else got[0]
    hello = dict(hellos[0]) if hellos else {}
    hello['wall_s'] = time.perf_counter() - started
    hello['jobs'] = len(threads)
    return hello, results


def run_port(port: Path, target: str, cases: list[dict], workdir: Path, fields) -> tuple[dict, dict]:
    casefile = workdir / f'{target}.cases.tsv'
    casefile.write_text(''.join('\t'.join([c['name']] + fields(c)) + '\n' for c in cases))
    outfile = workdir / f'{target}.port.jsonl'
    started = time.perf_counter()
    subprocess.run([str(port), f'--retail-ab={target}', f'--ab-cases={casefile}', f'--ab-out={outfile}'],
                   cwd=workdir, check=False, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    wall = time.perf_counter() - started
    results = {}
    for line in outfile.read_text().splitlines():
        record = json.loads(line)
        results[record['id']] = record
    return dict(wall_s=wall), results


# =====================================================================
# Target 1: script-parse
# =====================================================================

class Lines:
    """File offset -> 1-based line:column."""

    def __init__(self, data: bytes):
        self.starts = [0] + [i + 1 for i, b in enumerate(data) if b == 0x0a]

    def at(self, offset):
        if offset is None:
            return None
        import bisect
        line = bisect.bisect_right(self.starts, offset)
        return f'{line}:{offset - self.starts[line - 1] + 1}'


def script_view(proto: dict, lines: Lines) -> dict:
    """The comparable content of one prototype, positions as file line:col."""
    start = proto['text_start']

    def absolute(rel):
        return None if start is None or rel is None else lines.at(start + rel)

    end = None if start is None else lines.at(start + proto['text_len'])
    view = {
        'name': proto['name'],
        'discarded (ParseScript -1)': proto['result'] < 0,
        'parent': proto['parent'],
        'text extent': (lines.at(start), end),
        'text is a file span': proto['text_is_file_span'],
        'trigger count': (len(proto['triggers']), proto['numtriggers']),
        'parse errors': [e['text'].rstrip('\n') for e in proto['errors']],
    }
    for i, t in enumerate(proto['triggers']):
        key = f'trigger {i}'
        view[f'{key} type'] = t['type']
        view[f'{key} block at'] = absolute(t['pos'])
        view[f'{key} name'] = t['name']
        view[f'{key} cube'] = t['cube']
        view[f'{key} dist'] = t['dist']
        view[f'{key} +0x38'] = t['field38']
        view[f'{key} region'] = t['region']
    view['variables'] = [(v['type'], v['name'], v['number'], v['text']) for v in proto['variables']]
    view['labels'] = sorted(l['label'].lower() for l in proto['labels'])
    for l in proto['labels']:
        key = f"label {l['label'].lower()}"
        view[f'{key} found'] = l['found']
        view[f'{key} resumes at'] = absolute(l['ip'])
        view[f'{key} depth'] = l['depth']
        view[f'{key} errors'] = [e['text'].rstrip('\n') for e in l['errors']]
    return view


def compare_script_parse(case: dict, retail: dict, port: dict) -> list[dict]:
    data = Path(case['path']).read_bytes()
    lines = Lines(data)
    diffs = []
    rp, pp = retail['protos'], port['protos']
    if len(rp) != len(pp):
        diffs.append(dict(where='file', line=None, field='prototype count', retail=len(rp), port=len(pp)))
    for i in range(min(len(rp), len(pp))):
        rv, pv = script_view(rp[i], lines), script_view(pp[i], lines)
        where = f"#{i} {rp[i]['name']}"
        for key in list(rv) + [k for k in pv if k not in rv]:
            if rv.get(key, '<absent>') != pv.get(key, '<absent>'):
                diffs.append(dict(where=where, line=rp[i]['line'], field=key,
                                  retail=rv.get(key, '<absent>'), port=pv.get(key, '<absent>')))
    return diffs


# =====================================================================
# Target 2: say-duration
# =====================================================================

DEF_LINE = re.compile(rb'^\s*([^\s"/][^\s"]*)\s+"(.*)"\s*$')
NUMBER = re.compile(r'^-?\d+$')
TOKEN = re.compile(r'"[^"]*"|\S+')
SWEEP_MS = (0, 30000, 1)


def dialog_list(blob: bytes) -> dict[str, bytes]:
    """A <Language>.def: TAG "line" per line (0x0049d2a0); tags upper-cased."""
    lines: dict[str, bytes] = {}
    for raw in blob.splitlines():
        m = DEF_LINE.match(raw)
        if m:
            lines.setdefault(m.group(1).decode(TEXT).upper(), m.group(2))
    return lines


def say_lines(text: str):
    """Each `say` command: (line number, frames, sound, subject token)."""
    for number, raw in enumerate(text.splitlines(), 1):
        tokens = TOKEN.findall(raw.split('//', 1)[0])
        for i, tok in enumerate(tokens):
            if tok.lower() == 'say' or tok.lower().endswith('.say'):
                break
        else:
            continue
        rest, frames, sound = tokens[i + 1:], -1, None
        if rest and rest[0].lower() == 'nowait':
            rest = rest[1:]
        if rest and NUMBER.match(rest[0]):
            frames, rest = int(rest[0]), rest[1:]
        while len(rest) >= 2 and rest[0].lower() in ('anim', 'sound'):
            if rest[0].lower() == 'sound':
                sound = rest[1].strip('"')
            rest = rest[2:]
        if rest:
            yield number, frames, sound, rest[0], len(rest) > 1


def say_cases(data: Path, workdir: Path) -> list[dict]:
    """Every `say` line of the module's scripts and master.s, as Say sees it.

    A tag the dialog list has: SayIndex -- the line, the tag as the voice
    (0x004d09b0). Quoted text, or an unknown identifier: SayText -- the text,
    with `sound <name>` as the voice (frames forced to -1). `say choice` is
    left out (its text is the chosen response's, a runtime value). Identical
    inputs are one case. Plus one sweep of the voice length.
    """
    rvm = zipfile.ZipFile(data / 'Modules' / 'Ahkuilon.rvm')
    rvr = zipfile.ZipFile(data / 'resources.rvr')
    base, module = dialog_list(rvr.read('english.def')), dialog_list(rvm.read('english.def'))
    voices = {}
    for z in (rvm, rvr):
        for n in z.namelist():
            low = n.lower()
            if low.startswith('sound/english/') and low.endswith('.mp3'):
                voices.setdefault(Path(low).stem, (z, n))
    scripts = [(f'module.{n.lower()}', rvm.read(n)) for n in sorted(rvm.namelist()) if n.lower().endswith('.s') and '/' not in n]
    scripts += [(f'rvr.{n.lower()}', rvr.read(n)) for n in sorted(rvr.namelist()) if n.lower().endswith('.s')]
    vdir = workdir / 'voices'
    vdir.mkdir(parents=True, exist_ok=True)
    cases: dict[tuple, dict] = {}
    for name, blob in scripts:
        for line, frames, sound, subject, parts in say_lines(blob.decode(TEXT)):
            if subject.lower() == 'choice':
                continue
            if subject.startswith('"'):
                text, voice = subject.strip('"').encode(TEXT), sound
            else:
                tag = subject[:39].upper()
                found = base.get(tag, module.get(tag))
                if found is not None:
                    text, voice = found, tag
                else:
                    text, voice = subject.encode(TEXT), sound
            if voice is not None and voice == sound:
                frames = -1                    # `sound <name>` forces the voice's length
            key = (text, frames, voice)
            if key in cases:
                cases[key]['locations'] += 1
                continue
            path = ''
            if voice is not None and voice.lower() in voices:
                z, member = voices[voice.lower()]
                out = vdir / f'{voice.lower()}.mp3'
                if not out.exists():
                    out.write_bytes(z.read(member))
                path = str(out)
            cases[key] = dict(name=f'{name}:{line}', text_hex=text.hex(), frames=frames, sound=voice,
                              voice_path=path, parts=parts, locations=1,
                              sha256=hashlib.sha256(repr(key).encode()).hexdigest())
    out = list(cases.values())
    out.append(dict(name='sweep.voice-ms', text_hex=b'Sweep.'.hex(), frames=-1, sound='SWEEP', voice_path='',
                    sweep=list(SWEEP_MS), parts=False, locations=0,
                    sha256=hashlib.sha256(repr(SWEEP_MS).encode()).hexdigest()))
    # Forms no shipped `say` uses (every one is `say TAG` with a voice):
    # given frames, plain text, DialogLine's two-bytes-at-a-time copy (odd
    # and even lengths, bytes >= 0x80, a `[me]` behind one), truncation.
    first_voice = next(c for c in out if c.get('voice_path'))
    edges = [
        ('edge.frames-with-voice', bytes.fromhex(first_voice['text_hex']), 30, first_voice['sound'], first_voice['voice_path']),
        ('edge.frames-zero', b'Zero frames.', 0, None, ''),
        ('edge.text-odd', b'Odd', -1, None, ''),
        ('edge.text-even', b'Even', -1, None, ''),
        ('edge.text-empty', b'', -1, None, ''),
        ('edge.text-one', b'x', -1, None, ''),
        ('edge.text-cp1252', b'Caf\xe9 \x93quoted\x94', -1, None, ''),
        ('edge.text-brackets', b'Say [[me]] and [chr]', -1, None, ''),
        ('edge.text-high-then-me', b'\xe9[me]', -1, None, ''),
        ('edge.text-255', b'a' * 255, -1, None, ''),
        ('edge.text-300', b'b' * 300, -1, None, ''),
        ('edge.text-with-sound-missing', b'A line with a voice that is not there.', -1, 'NOSUCHVOICE', ''),
    ]
    for name, text, frames, sound, path in edges:
        out.append(dict(name=name, text_hex=text.hex(), frames=frames, sound=sound, voice_path=path,
                        parts=False, locations=0,
                        sha256=hashlib.sha256(repr((text, frames, sound)).encode()).hexdigest()))
    return out


def say_port_fields(case: dict) -> list[str]:
    sweep = ':'.join(map(str, case['sweep'])) if case.get('sweep') else ''
    return [str(case['frames']), case['sound'] or '', case['voice_path'], case['text_hex'], sweep]


def say_retail_case(case: dict, port: dict | None) -> dict:
    """Retail gets the voice's length the port measured (Miles doesn't run)."""
    out = dict(case)
    if case.get('voice_path') and port and port.get('ok'):
        out['voice_ms'] = port['result']['voice_ms']
    else:
        out['voice_ms'] = None
    return out


def compare_say(case: dict, retail: dict, port: dict) -> list[dict]:
    diffs = []
    for variant in ('novoice', 'voice'):
        r, p = retail.get(variant), port.get(variant)
        if (r is None) != (p is None):
            diffs.append(dict(where=variant, line=case['name'], field='variant present',
                              retail=r is not None, port=p is not None))
            continue
        if r is None:
            continue
        for key in ('wait', 'ticks', 'line_len'):
            if r[key] != p[key]:
                diffs.append(dict(where=variant, line=case['name'], field=key, retail=r[key], port=p[key],
                                  voice_ms=port.get('voice_ms')))
    if 'sweep' in retail or 'sweep' in port:
        rs, ps = retail.get('sweep', []), port.get('sweep', [])
        first, step = case['sweep'][0], case['sweep'][2]
        for i, (a, b) in enumerate(zip(rs, ps)):
            if a != b:
                diffs.append(dict(where='sweep', line=case['name'], field=f'voice {first + i * step} ms',
                                  retail=a, port=b))
        if len(rs) != len(ps):
            diffs.append(dict(where='sweep', line=case['name'], field='length', retail=len(rs), port=len(ps)))
    return diffs


# =====================================================================
# Target 3: dialog-layout
# =====================================================================

# Classic: the map view is the whole 640x480 screen (the port's headless
# fallback), the side tabs 52 wide, the status bar 0x70 high at the top.
CLASSIC = dict(map=[0, 0, 640, 480], sidetabs_w=52, statusbar=[0, 0x70])
LINE_SHAPES = [[1], [2], [1], [3], [1, 1], [2], [1], [1, 1, 1]]


def layout_cases(data: Path, workdir: Path) -> list[dict]:
    """Scripted pane timelines: both stacks, 1-8 entries, restacks."""
    cases = []

    def case(name, events, ticks=None):
        last = max((e['tick'] + max(e.get('life', 0), 0) for e in events), default=0)
        cases.append(dict(name=name, geometry=CLASSIC, ticks=ticks or last + 40, events=events,
                          sha256=hashlib.sha256(json.dumps([name, events, ticks]).encode()).hexdigest()))

    def add(tick, mode, life, i):
        return dict(tick=tick, op='add', mode=mode, life=life, lines=LINE_SHAPES[i % len(LINE_SHAPES)])

    for mode, stack in ((1, 'npc'), (2, 'player')):
        for n in range(1, 9):
            # All at once; the oldest goes first, each departure restacks the rest.
            case(f'{stack}.{n}.together', [add(0, mode, 24 + 18 * i, i) for i in range(n)])
            # One every 6 ticks (arrivals restack the player stack mid-slide).
            case(f'{stack}.{n}.staggered', [add(6 * i, mode, 40 + 6 * (n - i), i) for i in range(n)])
            # The newest goes first.
            case(f'{stack}.{n}.newest-first', [add(0, mode, 24 + 18 * (n - 1 - i), i) for i in range(n)])
        # Neighbours that fade out on the same tick (the deletion loop's skip).
        case(f'{stack}.same-tick-expiry', [add(0, mode, 20, i) for i in range(4)] + [add(0, mode, 60, 4)])
        # Dismissed mid-slide: a second restack starts from where the first got to.
        case(f'{stack}.dismiss-mid-slide',
             [add(0, mode, -1, i) for i in range(5)] +
             [dict(tick=10, op='dismiss', entry=0), dict(tick=15, op='dismiss', entry=1),
              dict(tick=40, op='dismiss', entry=3)], ticks=80)
    # Both stacks at once, interleaved.
    case('both.interleaved', [add(3 * i, 1 + i % 2, 30 + 9 * i, i) for i in range(8)])
    case('both.no-timeout',
         [add(0, 1, -1, 0), add(0, 2, -1, 1), add(5, 2, -1, 2), add(5, 1, -1, 3),
          dict(tick=20, op='dismiss', entry=1), dict(tick=21, op='dismiss', entry=0)], ticks=60)
    return cases


def layout_port_fields(case: dict) -> list[str]:
    items = []
    for e in case['events']:
        if e['op'] == 'add':
            items.append(f"{e['tick']}:add:{e['mode']}:{e['life']}:{','.join(map(str, e['lines']))}")
        else:
            items.append(f"{e['tick']}:dismiss:{e['entry']}")
    return [str(case['ticks']), ';'.join(items)]


def layout_retail_case(case: dict, port: dict | None) -> dict:
    """Retail gets the entries' heights the port measured (the retail entry
    constructor wraps with GDI fonts, which the emulator doesn't have)."""
    out = dict(case)
    out['heights'] = port['result']['heights'] if port and port.get('ok') else []
    return out


def compare_layout(case: dict, retail: dict, port: dict) -> list[dict]:
    diffs = []
    if port['map'] != case['geometry']['map']:
        diffs.append(dict(where='geometry', line=None, field='map view', retail=case['geometry']['map'], port=port['map']))
    for tick, (rt, pt) in enumerate(zip(retail['ticks'], port['ticks'])):
        rids, pids = [e['id'] for e in rt], [e['id'] for e in pt]
        if rids != pids:
            diffs.append(dict(where=f'tick {tick}', line=tick, field='entries', retail=rids, port=pids))
        pmap = {e['id']: e for e in pt}
        for re_ in rt:
            pe = pmap.get(re_['id'])
            if pe is None:
                continue
            for key in re_:
                if re_[key] != pe.get(key):
                    diffs.append(dict(where=f"tick {tick} entry {re_['id']}", line=tick, field=key,
                                      retail=re_[key], port=pe.get(key)))
    if len(retail['ticks']) != len(port['ticks']):
        diffs.append(dict(where='run', line=None, field='ticks', retail=len(retail['ticks']), port=len(port['ticks'])))
    return diffs


# =====================================================================
# Target 4: script-step
# =====================================================================

# The result bits that steer Continue (CMD_WAIT, DELETED, CONDTRUE/FALSE,
# ELSE, SKIPBLOCK, LOOP, BEGIN, END, JUMP, WAITSAY); the error bits (2, 4,
# 8, 0x10) depend on what the interpreter could resolve in each fixture.
FLOW_BITS = 0x1 | 0x20 | 0x40 | 0x80 | 0x100 | 0x200 | 0x400 | 0x800 | 0x1000 | 0x2000 | 0x4000


def step_cases(data: Path, workdir: Path) -> list[dict]:
    """Every shipped script file and the stepping edge cases."""
    return script_cases(data, workdir, edges='script_step')


def step_split(case: dict, n: int) -> list[dict]:
    """A file's runs, every n-th to a process (the fixture's `share`)."""
    return [dict(case, share=[k, n]) for k in range(n)]


def step_merge(case: dict, responses: list[dict]) -> dict:
    """The shares back into one dump: each prototype's runs in run order."""
    failed = next((r for r in responses if not r.get('ok')), None)
    if failed:
        return failed
    merged = json.loads(json.dumps(responses[0]))
    for other in responses[1:]:
        for proto, more in zip(merged['result']['protos'], other['result']['protos']):
            proto['runs'] += more['runs']
    for proto in merged['result']['protos']:
        proto['runs'].sort(key=lambda run: run['index'])
    merged['result']['elapsed_ms'] = sum(r['result'].get('elapsed_ms', 0) for r in responses)
    return merged


class FileLines(Lines):
    """Lines with their text, for readable sequences."""

    def __init__(self, data: bytes):
        super().__init__(data)
        self.text = data.split(b'\n')

    def number(self, offset):
        import bisect
        return bisect.bisect_right(self.starts, offset)

    def show(self, number):
        return f"{number}: {self.text[number - 1].decode(TEXT).strip()[:48]}"


def step_view(proto: dict, run: dict, lines: FileLines) -> dict:
    """One run as comparable fields; lines as file line numbers.

    A line is where the interpreter was handed it: the stream position
    after its first token (minus one: retail's tokenizer holds the next
    character), in the prototype's text.
    """
    start = proto['text_start']

    def at(rel):
        return None if start is None or rel is None else lines.at(start + rel)

    seq = run['lines']
    return {
        'start found': run['start'].get('found'),
        'start resumes at': at(run['start'].get('ip')),
        'start depth': run['start']['depth'],
        'start errors': [e.rstrip('\n') for e in run['start'].get('errors', [])],
        'lines': [lines.number(start + l['at'] - 1) for l in seq] if start is not None else None,
        'depths': [l['depth'] for l in seq],
        'bits': [l.get('bits', -1) & FLOW_BITS for l in seq],
        'truncated': run['truncated'],
        'ended': run['end']['ip'] is None,
        'end at': at(run['end']['ip']),
        'end depth': run['end']['depth'],
        'waiting': run['waiting'],
        'errors': [e.rstrip('\n') for e in run['errors']],
        'fault': run.get('fault'),
    }


def compare_script_step(case: dict, retail: dict, port: dict) -> list[dict]:
    lines = FileLines(Path(case['path']).read_bytes())
    diffs = []
    rp, pp = retail['protos'], port['protos']
    if len(rp) != len(pp):
        diffs.append(dict(where='file', line=None, field='prototype count', retail=len(rp), port=len(pp)))
    for r, p in zip(rp, pp):
        where = f"#{r['index']} {r['name']}"
        if r['name'] != p['name']:
            diffs.append(dict(where=where, line=None, field='name', retail=r['name'], port=p['name']))
            continue
        pruns = {run['key']: run for run in p['runs']}
        for rrun in r['runs']:
            prun = pruns.pop(rrun['key'], None)
            kind = rrun['kind']
            if prun is None:
                diffs.append(dict(where=f"{where} {rrun['key']}", line=None, field=f'{kind} run',
                                  retail='present', port='<absent>'))
                continue
            rv, pv = step_view(r, rrun, lines), step_view(p, prun, lines)
            for key in rv:
                if rv[key] == pv[key]:
                    continue
                d = dict(where=f"{where} {rrun['key']}", line=None, field=f'{kind} {key}',
                         retail=rv[key], port=pv[key])
                if key == 'lines' and rv[key] is not None and pv[key] is not None:
                    a, b = rv[key], pv[key]
                    k = next((i for i in range(min(len(a), len(b))) if a[i] != b[i]), min(len(a), len(b)))
                    d.update(line=(a[k] if k < len(a) else b[k]), first_difference=k,
                             retail=[lines.show(n) for n in a[k:k + 16]],
                             port=[lines.show(n) for n in b[k:k + 16]])
                elif key == 'depths' and len(rv[key]) == len(pv[key]):
                    offsets = {x - y for x, y in zip(rv[key], pv[key])}
                    if len(offsets) == 1:
                        d.update(field=f'{kind} depths (retail - port = {offsets.pop():+d} throughout)',
                                 retail=rv[key][:12], port=pv[key][:12])
                diffs.append(d)
        for key in pruns:
            diffs.append(dict(where=f'{where} {key}', line=None, field=f"{pruns[key]['kind']} run",
                              retail='<absent>', port='present'))
    return diffs


# =====================================================================
# Target 5: trigger-test
# =====================================================================

TRIGGER_TYPES = dict(ALWAYS=1, TRIGGER=2, DIALOG=3, PROXIMITY=4, CUBE=5, ACTIVATE=6, USE=7, GIVE=8,
                     GET=9, COMBAT=10, DEAD=11)
CHARACTER, PLAYER_CLASS, ITEM = 12, 11, 2
OWNER = dict(id='owner', name='Owner', **{'class': CHARACTER}, level=0, pos=[1000, 1000, 0])
LOCKE = dict(id='player', name='Locke', **{'class': PLAYER_CLASS}, level=0, pos=[1100, 1000, 0])


def trigger_cases(data: Path, workdir: Path) -> list[dict]:
    """Each trigger kind over the inputs the test reads (SCRIPT_ENGINE.md §3).

    A case: the trigger record, the script's request state (type, strings,
    guard, running record), the priority Continue passes, and the world
    (objects in enumeration order; one is the owner, one may be the player).
    """
    cases = []

    def case(name, ttype, tname='', cube=(0, 0, 0, 0, 0, 0), dist=0, tprio=0, request=0, s1='', s2='',
             guard='none', running=False, priority=0, objects=None, player='player', owner='owner'):
        objects = [dict(o) for o in (objects if objects is not None else [OWNER, LOCKE])]
        body = dict(trigger=dict(type=ttype, name=tname, cube=list(cube), dist=dist, priority=tprio),
                    request=dict(type=request, str=s1, str2=s2, guard=guard, running=running),
                    priority=priority, world=dict(objects=objects, player=player, owner=owner))
        cases.append(dict(name=name, **body, sha256=hashlib.sha256(json.dumps(body, sort_keys=True).encode()).hexdigest()))

    def at(obj, x, y, z=0, level=None, **extra):
        o = dict(obj, pos=[x, y, z], **extra)
        if level is not None:
            o['level'] = level
        return o

    # Every type: no request, its own request, another type's request.
    for tname, ttype in [('NONE', 0)] + sorted(TRIGGER_TYPES.items(), key=lambda kv: kv[1]) + [('TWELVE', 12)]:
        for req in (0, ttype, 3 if ttype != 3 else 6):
            case(f'type.{tname}.request{req}', ttype, 'Lever', request=req, s1='Lever')
    # Priority: below the running one, equal, above; the running-trigger record.
    for tprio, prio in ((0, 1), (1, 1), (2, 1), (-1, 0)):
        case(f'priority.trigger{tprio}.running{prio}', 1, tprio=tprio, priority=prio)
        case(f'priority.dialog.trigger{tprio}.running{prio}', 3, tprio=tprio, priority=prio, request=3)
    case('running-record.always', 1, running=True)
    # Named manual triggers: names compare without case, USE takes either string.
    for ttype in ('TRIGGER', 'GIVE', 'GET', 'USE'):
        n = TRIGGER_TYPES[ttype]
        for label, tname, s1, s2 in (('same', 'Lever', 'Lever', ''), ('case', 'Lever', 'LEVER', ''),
                                     ('other', 'Lever', 'Rope', ''), ('second', 'Lever', 'Rope', 'lever'),
                                     ('empty-both', '', '', ''), ('empty-trigger', '', 'Lever', ''),
                                     ('empty-request', 'Lever', '', ''),
                                     ('long', 'ABCDEFGHIJKLMNOPQRS', 'abcdefghijklmnopqrs', '')):
            case(f'named.{ttype}.{label}', n, tname, request=n, s1=s1, s2=s2)
    # PROXIMITY: the player's name, the box, then dx^2 + dy^2 <= 2 dist^2.
    for label, dx, dy, z in (('same-spot', 0, 0, 0), ('on-x-edge', 100, 0, 0), ('past-x', 101, 0, 0),
                             ('on-y-edge', 0, -100, 0), ('past-y', 0, 101, 0), ('diag-in', 70, 70, 0),
                             ('corner', 100, 100, 0), ('past-corner', 100, 101, 0), ('z-ignored', 0, 0, 5000),
                             ('neg', -60, -80, 0)):
        case(f'proximity.dist100.{label}', 4, 'Locke', dist=100,
             objects=[OWNER, at(LOCKE, 1000 + dx, 1000 + dy, z)])
    for label, dist, dx, dy in (('dist0-same', 0, 0, 0), ('dist0-off', 0, 1, 0), ('dist-neg', -5, 0, 0),
                                ('big-in', 40000, 30000, 0), ('big-sum-wraps', 40000, 40000, 40000),
                                ('big-on-edge', 32768, 32768, 0)):
        case(f'proximity.{label}', 4, 'Locke', dist=dist, objects=[OWNER, at(LOCKE, 1000 + dx, 1000 + dy)])
    case('proximity.name-case', 4, 'LOCKE', dist=100)
    case('proximity.other-name', 4, 'Owner', dist=100)
    case('proximity.empty-name', 4, '', dist=100)
    case('proximity.no-player', 4, 'Locke', dist=100, objects=[OWNER], player=None)
    case('proximity.requested-far', 4, 'Somebody', dist=1, request=4)
    case('proximity.player-other-level', 4, 'Locke', dist=200, objects=[OWNER, at(LOCKE, 1100, 1000, level=1)])
    case('proximity.guard-exists', 4, 'Locke', dist=200, guard='exists')
    case('proximity.guard-gone', 4, 'Locke', dist=200, guard='gone')
    # CUBE: the player by name (or "player"), inclusive on every face.
    box = (900, 900, 0, 1200, 1200, 100)
    for tname in ('player', 'PLAYER', 'Locke', 'locke'):
        case(f'cube.named-{tname}.inside', 5, tname, cube=box)
    for label, x, y, z in (('min-corner', 900, 900, 0), ('max-corner', 1200, 1200, 100), ('past-x', 1201, 1000, 0),
                           ('below-y', 1000, 899, 0), ('above-z', 1000, 1000, 101), ('below-z', 1000, 1000, -1)):
        case(f'cube.player.{label}', 5, 'player', cube=box, objects=[OWNER, at(LOCKE, x, y, z)])
    case('cube.player.reversed-corners', 5, 'player', cube=(1200, 1200, 100, 900, 900, 0))
    case('cube.player.outside-other-inside', 5, 'player', cube=(0, 0, 0, 100, 100, 100),
         objects=[OWNER, LOCKE, at(OWNER, 50, 50, id='ogre', name='Ogre')])
    case('cube.requested', 5, 'Locke', cube=(0, 0, 0, 1, 1, 1), request=5)
    # CUBE, someone else: the first moving object in the cube, if a character
    # (or player) that isn't the named object.
    far = at(LOCKE, 5000, 5000)
    away = at(OWNER, 3000, 3000)                # the owner, outside the cube
    ogre = dict(id='ogre', name='Ogre', **{'class': CHARACTER}, level=0, pos=[1050, 1050, 0])
    rat = dict(id='rat', name='Rat', **{'class': CHARACTER}, level=0, pos=[1060, 1060, 0])
    crate = dict(id='crate', name='Crate', **{'class': ITEM}, level=0, pos=[1040, 1040, 0])
    case('cube.other.character', 5, 'Owner', cube=box, objects=[away, far, ogre])
    case('cube.other.owner-only', 5, 'Owner', cube=box, objects=[OWNER, far])
    case('cube.other.owner-first', 5, 'Owner', cube=box, objects=[OWNER, ogre, far])
    case('cube.other.owner-second', 5, 'Owner', cube=box, objects=[ogre, OWNER, far])
    case('cube.other.unnamed', 5, '', cube=box, objects=[OWNER, far])
    case('cube.other.named-ogre', 5, 'Ogre', cube=box, objects=[away, far, ogre, rat])
    case('cube.other.item-first', 5, 'Owner', cube=box, objects=[away, crate, ogre, far])
    case('cube.other.player-class', 5, 'Owner', cube=box,
         objects=[away, far, dict(ogre, **{'class': PLAYER_CLASS}, id='player2', name='Second')])
    case('cube.other.no-player', 5, 'Owner', cube=box, objects=[OWNER, ogre], player=None)
    case('cube.other.no-player-owner-only', 5, 'Owner', cube=box, objects=[OWNER], player=None)
    case('cube.other.other-level', 5, 'Owner', cube=box, objects=[away, far, dict(ogre, level=1)])
    case('cube.other.owner-other-level', 5, 'Owner', cube=box,
         objects=[dict(OWNER, level=1), far, dict(ogre, level=1)])
    case('cube.other.player-other-level', 5, 'Owner', cube=box,
         objects=[OWNER, dict(far, level=1), ogre])
    # Whose level: retail searches the owner's, the port's map pane its
    # window's (the player's).
    case('cube.level.owner1-player0-ogre1', 5, 'Owner', cube=box,
         objects=[dict(away, level=1), far, dict(ogre, level=1)])
    case('cube.level.owner0-player1-ogre0', 5, 'Owner', cube=box,
         objects=[away, dict(far, level=1), ogre])
    case('cube.level.owner0-player1-ogre1', 5, 'Owner', cube=box,
         objects=[away, dict(far, level=1), dict(ogre, level=1)])
    case('cube.other.guard-exists', 5, 'Owner', cube=box, objects=[away, far, ogre], guard='exists')
    case('cube.other.guard-gone', 5, 'Owner', cube=box, objects=[away, far, ogre], guard='gone')
    # The guard after a test that fires, and after one that doesn't.
    case('guard.always-exists', 1, guard='exists')
    case('guard.always-gone', 1, guard='gone')
    case('guard.dialog-unrequested-gone', 3, guard='gone')
    return cases


def trigger_port_fields(case: dict) -> list[str]:
    t, r, w = case['trigger'], case['request'], case['world']
    hx = lambda s: s.encode(TEXT).hex()
    objects = ';'.join(f"{o['id']}|{hx(o['name'])}|{o['class']}|{o['level']}|{','.join(map(str, o['pos']))}"
                       for o in w['objects'])
    return [f"{t['type']}|{hx(t['name'])}|{','.join(map(str, t['cube']))}|{t['dist']}|{t['priority']}",
            f"{r['type']}|{hx(r['str'])}|{hx(r['str2'])}|{r['guard']}|{int(r['running'])}",
            str(case['priority']), objects, w['player'] or '', w['owner']]


def compare_trigger(case: dict, retail: dict, port: dict) -> list[dict]:
    diffs = []
    for key in ('fires', 'user', 'alias', 'guard_after'):
        if retail[key] != port.get(key):
            diffs.append(dict(where='test', line=None, field=key, retail=retail[key], port=port.get(key),
                              queries=retail.get('queries')))
    return diffs


TARGETS = {
    'trigger-test': dict(fixture='slots/gameflow/trigger_test.py', cases=trigger_cases,
                         compare=compare_trigger, port_fields=trigger_port_fields,
                         unit=lambda r: 1),
    'script-step': dict(fixture='slots/gameflow/script_step.py', cases=step_cases,
                        compare=compare_script_step, split=step_split, merge=step_merge,
                        port_fields=lambda c: [c['path'], c['filename']],
                        unit=lambda r: sum(len(p['runs']) for p in r['protos'])),
    'dialog-layout': dict(fixture='slots/gameflow/dialog_layout.py', cases=layout_cases,
                          compare=compare_layout, port_first=True, retail_case=layout_retail_case,
                          port_fields=layout_port_fields,
                          unit=lambda r: sum(len(t) for t in r['ticks'])),
    'script-parse': dict(fixture='slots/gameflow/script_parse.py', cases=script_cases,
                         compare=compare_script_parse,
                         port_fields=lambda c: [c['path'], c['filename']],
                         unit=lambda r: len(r['protos'])),
    'say-duration': dict(fixture='slots/gameflow/say_duration.py', cases=say_cases,
                         compare=compare_say, port_first=True, retail_case=say_retail_case,
                         port_fields=say_port_fields,
                         unit=lambda r: 1 + len(r.get('sweep', []))),
}

# The combat dojo's targets (combat_targets.py, docs/gameplay/COMBAT_DOJO.md).
from combat_targets import TARGETS as COMBAT_TARGETS  # noqa: E402
TARGETS.update(COMBAT_TARGETS)


# =====================================================================
# Driver
# =====================================================================

def git_commit() -> str:
    head = subprocess.run(['git', '-C', str(REPO), 'rev-parse', 'HEAD'], capture_output=True, text=True).stdout.strip()
    dirty = subprocess.run(['git', '-C', str(REPO), 'status', '--porcelain', '--untracked-files=no'],
                           capture_output=True, text=True).stdout.strip()
    return head + ('+dirty' if dirty else '')


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('target', choices=sorted(TARGETS))
    parser.add_argument('--out', type=Path, default=REPO / 'build' / 'retail_ab')
    parser.add_argument('--port', type=Path, default=REPO / 'build' / 'Revenant')
    parser.add_argument('--data', type=Path, default=DATA)
    parser.add_argument('--all', action='store_true', help='print every difference, not the first per case')
    parser.add_argument('--case', action='append', help='only cases whose name contains this')
    parser.add_argument('--jobs', type=int, default=max(1, (os.cpu_count() or 2) // 2),
                        help='retail fixture processes at once (default: half the cores)')
    args = parser.parse_args()
    # The port reads the same install the cases come from.
    os.environ.setdefault('REVENANT_DATA_PATH', str(args.data))

    spec = TARGETS[args.target]
    workdir = (args.out / args.target).resolve()
    workdir.mkdir(parents=True, exist_ok=True)
    cases = spec['cases'](args.data, workdir)
    if args.case:
        cases = [c for c in cases if any(s in c['name'] for s in args.case)]
    case_hash = hashlib.sha256(json.dumps([(c['name'], c['sha256']) for c in cases]).encode()).hexdigest()

    # A fixture versioned with this tree's emulator runs from it; the others
    # from RETAIL_RUNTIME (the main checkout's working copy).
    fixture = REPO / 'tools' / 'retail_runtime' / spec['fixture']
    if not fixture.exists():
        fixture = RUNTIME / spec['fixture']
    if spec.get('port_first'):
        port_info, port = run_port(args.port, args.target, cases, workdir, spec['port_fields'])
        retail_cases = [spec['retail_case'](c, port.get(c['name'])) for c in cases]
        retail_info, retail = run_retail(fixture, retail_cases, args.jobs)
    else:
        retail_info, retail = run_retail(fixture, cases, args.jobs, spec.get('split'), spec.get('merge'))
        port_info, port = run_port(args.port, args.target, cases, workdir, spec['port_fields'])
    # The retail dump beside the port's (<target>.port.jsonl), for reading
    # a run in full.
    (workdir / f'{args.target}.retail.jsonl').write_text(
        ''.join(json.dumps({**response, 'id': name}) + '\n' for name, response in retail.items()))

    report_cases, total_diffs, matched, units = [], 0, 0, 0
    for case in cases:
        r, p = retail.get(case['name']), port.get(case['name'])
        entry = dict(name=case['name'], origin=case.get('origin'), sha256=case['sha256'])
        if not r or not r.get('ok') or not p or not p.get('ok'):
            def status(side):
                return 'missing' if not side else ('ok' if side.get('ok') else side.get('error'))
            entry['error'] = dict(retail=status(r), port=status(p))
            diffs = [dict(where='case', line=None, field='run', retail=entry['error']['retail'], port=entry['error']['port'])]
        else:
            diffs = spec['compare'](case, r['result'], p['result'])
            entry['units'] = spec['unit'](r['result'])
            entry['retail_ms'] = r['result'].get('elapsed_ms')
            units += entry['units']
        entry['differences'] = diffs
        report_cases.append(entry)
        total_diffs += len(diffs)
        matched += not diffs
        if not diffs:
            print(f"MATCH  {case['name']}")
            continue
        print(f"DIFF   {case['name']}: {len(diffs)} difference(s)")
        shown = diffs if args.all else diffs[:1]
        for d in shown:
            print(f"       {d['where']} (script line {d['line']}): {d['field']}\n"
                  f"         retail: {json.dumps(d['retail'])}\n"
                  f"         port:   {json.dumps(d['port'])}")

    # Differences by kind (trigger and label indices folded), so a systematic
    # difference reads as one line with its count.
    kinds: dict[str, list] = {}
    for entry in report_cases:
        for d in entry['differences']:
            kind = re.sub(r'^(trigger|label) \S+', r'\1 *', d['field']) if args.target == 'script-parse' else d['field']
            kinds.setdefault(kind, []).append(f"{entry['name']} {d['where']}")
    if kinds:
        print('\ndifferences by kind:')
        for kind, where in sorted(kinds.items(), key=lambda kv: -len(kv[1])):
            print(f'  {len(where):5d}  {kind:34s} e.g. {where[0]}')

    retail_s = retail_info.get('wall_s', 0)
    report = dict(target=args.target, retail_build_sha256=retail_info.get('retail_sha256'),
                  retail_setup_ms=retail_info.get('setup_ms'), port_commit=git_commit(),
                  port_binary_sha256=hashlib.sha256(args.port.read_bytes()).hexdigest(),
                  case_set_sha256=case_hash, cases=len(cases), matches=matched, differences=total_diffs,
                  units=units, retail_wall_s=retail_s, retail_jobs=retail_info.get('jobs'),
                  port_wall_s=port_info['wall_s'],
                  retail_cases_per_s=len(cases) / retail_s if retail_s else None,
                  port_cases_per_s=len(cases) / port_info['wall_s'] if port_info['wall_s'] else None,
                  results=report_cases)
    (workdir / 'report.json').write_text(json.dumps(report, indent=1) + '\n')
    print(f"\n{args.target}: {len(cases)} cases, {matched} match, {total_diffs} difference(s); "
          f"retail {retail_s:.1f} s in {retail_info.get('jobs')} process(es) "
          f"({report['retail_cases_per_s'] or 0:.2f} cases/s), "
          f"port {port_info['wall_s']:.2f} s")
    print(f"retail build {report['retail_build_sha256']}  port {report['port_commit']}  cases {case_hash[:16]}")
    print(f"report: {workdir / 'report.json'}")
    return 0 if total_diffs == 0 else 1


if __name__ == '__main__':
    sys.exit(main())
