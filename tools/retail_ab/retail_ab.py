#!/usr/bin/env python3
"""Gameflow A/B against the shipped game's own code (docs/gameflow/RETAIL_AB.md).

    retail_ab.py script-parse [--out DIR] [--all] [--port BIN] [--data DIR]

One command per target. It gathers every case, runs retail's original
function in the in-process emulator (one persistent process; the fixture
checkpoints after setup and restores per case) and the port's function
(`Revenant --retail-ab=<target>`, one process), compares the two dumps and
prints the first difference per case (`--all`: every difference). The report
(DIR/report.json) records the retail build hash, the port commit, the case
set's hash, every difference and the timing.

Targets:
  script-parse   TScriptProto::ParseScript 0x00494e20 (+ TScript::Jump
                 0x00493fa0 on every label) over every shipped script and the
                 edge cases in tools/retail_ab/cases/script_parse/.

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
import re
import subprocess
import sys
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

def script_cases(data: Path, workdir: Path) -> list[dict]:
    """Every shipped script file, plus the edge cases kept in this tree.

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
    for path in sorted((HERE / 'cases' / 'script_parse').glob('*.s')):
        add(f'edge.{path.name}', path.read_bytes(), path.name, str(path.relative_to(REPO)))
    return cases


# =====================================================================
# Running both sides
# =====================================================================

def run_retail(fixture: Path, cases: list[dict]) -> tuple[dict, dict]:
    started = time.perf_counter()
    proc = subprocess.Popen([str(RETAIL_PY), str(fixture), str(RETAIL_EXE), '--serve'],
                            stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)
    hello = json.loads(proc.stdout.readline())
    results = {}
    for case in cases:
        proc.stdin.write(json.dumps(dict(id=case['name'], case=case)) + '\n')
        proc.stdin.flush()
        response = json.loads(proc.stdout.readline())
        results[case['name']] = response
    proc.stdin.close()
    proc.wait(timeout=60)
    hello['wall_s'] = time.perf_counter() - started
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


TARGETS = {
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
    args = parser.parse_args()

    spec = TARGETS[args.target]
    workdir = (args.out / args.target).resolve()
    workdir.mkdir(parents=True, exist_ok=True)
    cases = spec['cases'](args.data, workdir)
    if args.case:
        cases = [c for c in cases if any(s in c['name'] for s in args.case)]
    case_hash = hashlib.sha256(json.dumps([(c['name'], c['sha256']) for c in cases]).encode()).hexdigest()

    if spec.get('port_first'):
        port_info, port = run_port(args.port, args.target, cases, workdir, spec['port_fields'])
        retail_cases = [spec['retail_case'](c, port.get(c['name'])) for c in cases]
        retail_info, retail = run_retail(RUNTIME / spec['fixture'], retail_cases)
    else:
        retail_info, retail = run_retail(RUNTIME / spec['fixture'], cases)
        port_info, port = run_port(args.port, args.target, cases, workdir, spec['port_fields'])

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
            kind = re.sub(r'^(trigger|label) \S+', r'\1 *', d['field'])
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
                  units=units, retail_wall_s=retail_s, port_wall_s=port_info['wall_s'],
                  retail_cases_per_s=len(cases) / retail_s if retail_s else None,
                  port_cases_per_s=len(cases) / port_info['wall_s'] if port_info['wall_s'] else None,
                  results=report_cases)
    (workdir / 'report.json').write_text(json.dumps(report, indent=1) + '\n')
    print(f"\n{args.target}: {len(cases)} cases, {matched} match, {total_diffs} difference(s); "
          f"retail {retail_s:.1f} s ({report['retail_cases_per_s'] or 0:.2f} cases/s), "
          f"port {port_info['wall_s']:.2f} s")
    print(f"retail build {report['retail_build_sha256']}  port {report['port_commit']}  cases {case_hash[:16]}")
    print(f"report: {workdir / 'report.json'}")
    return 0 if total_diffs == 0 else 1


if __name__ == '__main__':
    sys.exit(main())
