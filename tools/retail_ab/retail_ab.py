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


def run_port(port: Path, target: str, cases: list[dict], workdir: Path) -> tuple[dict, dict]:
    casefile = workdir / f'{target}.cases.tsv'
    casefile.write_text(''.join(f"{c['name']}\t{c['path']}\t{c['filename']}\n" for c in cases))
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


TARGETS = {
    'script-parse': dict(fixture='slots/gameflow/script_parse.py', cases=script_cases,
                         compare=compare_script_parse,
                         unit=lambda r: len(r['protos'])),
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

    retail_info, retail = run_retail(RUNTIME / spec['fixture'], cases)
    port_info, port = run_port(args.port, args.target, cases, workdir)

    report_cases, total_diffs, matched, units = [], 0, 0, 0
    for case in cases:
        r, p = retail.get(case['name']), port.get(case['name'])
        entry = dict(name=case['name'], origin=case['origin'], sha256=case['sha256'])
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
