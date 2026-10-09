#!/usr/bin/env python3
"""HUD A/B against the shipped game's own code (docs/ui/HUD_REBUILD.md).

    hud_ab.py TARGET [--out DIR] [--port BIN] [--cases N] [--seed S]

One command per target. It builds the case set, runs retail's original code
in the emulator's HUD slot (one persistent fixture process; it loads the
boot image and restores a checkpoint per case) and the port's equivalent
(`Revenant --retail-ab=<target>`), compares the two results and prints a
summary and the first differences. DIR/report.json records the retail
build hash, the port binary hash, the case set's hash, every case's result
and the timing.

Targets:
  draw-put   TSurface::Put 0x004bd680 (Draw 0x004ad0d0, routine selector
             0x004ad1d0) vs the port's TBitmap::Put, on identical bytes:
             random ARGB4444 destinations, StatusBar.dat's bitmaps plus
             synthetic ARGB4444 bitmaps, default / plain / alpha modes,
             in-bounds positions, even destination widths (retail faults on
             odd widths with a DWORD-aligned pitch). Exact pixel match.

Environment / defaults:
  RETAIL_PY   the emulator's Python (the retail-asm venv)
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import random
import struct
import subprocess
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[1]
RUNTIME = REPO / 'tools' / 'retail_runtime'
SLOT = RUNTIME / 'slots' / 'hud'
RETAIL_EXE = REPO / 'recon' / 'retail_asm' / 'baseline' / 'Revenant.rebuilt.exe'
RETAIL_PY = Path(os.environ.get('RETAIL_PY', '/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python'))
PORT = REPO / 'build' / 'Revenant'

DM_USEDEFAULT, DM_ALPHA = 0x80000000, 0x2000
BM_ARGB4444 = 0x10000


def sha256(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


# =====================================================================
# Fixture processes
# =====================================================================

class Fixture:
    """A slot fixture speaking JSONL ({"id","case"} -> {"id","ok",...})."""

    def __init__(self, script, *args):
        self.process = subprocess.Popen([str(RETAIL_PY), str(script), str(RETAIL_EXE), *args],
                                        stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)
        self.hello = json.loads(self.process.stdout.readline())

    def run(self, case_id, case):
        self.process.stdin.write(json.dumps(dict(id=case_id, case=case)) + '\n')
        self.process.stdin.flush()
        return json.loads(self.process.stdout.readline())

    def close(self):
        self.process.stdin.close()
        self.process.wait(timeout=30)


def run_port(target, port, lines, workdir):
    """`Revenant --retail-ab=<target>` over tab-separated case lines."""
    cases = workdir / f'{target}.cases.tsv'
    out = workdir / f'{target}.port.jsonl'
    cases.write_text(''.join('\t'.join(map(str, line)) + '\n' for line in lines))
    subprocess.run([str(port), f'--retail-ab={target}', f'--ab-cases={cases}', f'--ab-out={out}'],
                   check=False, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    return {r['id']: r for r in map(json.loads, out.read_text().splitlines())}


# =====================================================================
# draw-put
# =====================================================================

def bitmap_4444(width, height, rng):
    """A synthetic ARGB4444 TBitmap: header, then pixels whose alpha is
    weighted to 0, 15 and the blend range."""
    header = struct.pack('<18I', width, height, 0, 0, BM_ARGB4444, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         width * height * 2)
    pixels = bytearray()
    for _ in range(width * height):
        alpha = rng.choice((0, 0, 15, 15, rng.randrange(1, 15)))
        pixels += struct.pack('<H', (alpha << 12) | rng.getrandbits(12))
    return header + bytes(pixels)


def draw_put_cases(count, seed, real_bitmaps):
    rng = random.Random(seed)
    cases = []
    for index in range(count):
        width, height = rng.randrange(8, 97), rng.randrange(8, 65)
        dest = bytearray()
        for _ in range(width * height):
            alpha = rng.choice((0, 15, rng.randrange(0, 16)))
            dest += struct.pack('<H', (alpha << 12) | rng.getrandbits(12))
        if real_bitmaps and rng.random() < 0.5:
            name = rng.choice(sorted(real_bitmaps))
            bitmap = bytes.fromhex(real_bitmaps[name])
        else:
            name = 'synthetic'
            bitmap = bitmap_4444(rng.randrange(1, 49), rng.randrange(1, 49), rng)
        bw, bh = struct.unpack_from('<II', bitmap, 0)
        # In bounds: retail surfaces wrap draws that cross an edge (their
        # clip mode; a separate check), the HUD's never do. Even widths:
        # retail's Put faults on an odd-width surface with a DWORD-aligned
        # pitch (as DirectDraw gives system-memory surfaces); every HUD
        # surface is even (128, 40, 640, ...).
        width, height = max(width, bw), max(height, bh)
        width += width & 1
        dest = dest[:0]
        for _ in range(width * height):
            alpha = rng.choice((0, 15, rng.randrange(0, 16)))
            dest += struct.pack('<H', (alpha << 12) | rng.getrandbits(12))
        x = rng.randrange(0, width - bw + 1)
        y = rng.randrange(0, height - bh + 1)
        mode = rng.choice((0, DM_USEDEFAULT, DM_ALPHA))
        cases.append(dict(name=f'put-{index:04d}', source=name,
                          case=dict(dest=dict(format='4444', width=width, height=height, pixels=bytes(dest).hex()),
                                    bitmap=bitmap.hex(), x=x, y=y, mode=mode)))
    return cases


def compare_pixels(width, retail_hex, port_hex):
    retail, port = bytes.fromhex(retail_hex), bytes.fromhex(port_hex)
    if len(retail) != len(port):
        return dict(size_mismatch=[len(retail), len(port)])
    diffs = [i // 2 for i in range(0, len(retail), 2) if retail[i:i + 2] != port[i:i + 2]]
    first = None
    if diffs:
        i = diffs[0]
        first = dict(x=i % width, y=i // width, retail=f'0x{struct.unpack_from("<H", retail, i * 2)[0]:04x}',
                     port=f'0x{struct.unpack_from("<H", port, i * 2)[0]:04x}')
    return dict(differing_pixels=len(diffs), first=first)


def target_draw_put(args, workdir):
    retail = Fixture(SLOT / 'draw_ab.py', '--serve')
    bitmaps = json.loads(subprocess.run([str(RETAIL_PY), str(SLOT / 'draw_ab.py'), str(RETAIL_EXE), '--bitmaps'],
                                        check=True, capture_output=True, text=True).stdout)
    cases = draw_put_cases(args.cases, args.seed, bitmaps)
    started = time.perf_counter()
    retail_results = {c['name']: retail.run(c['name'], c['case']) for c in cases}
    retail_seconds = time.perf_counter() - started
    retail.close()
    started = time.perf_counter()
    port_results = run_port('draw-put', args.port, [
        (c['name'], c['case']['dest']['format'], c['case']['dest']['width'], c['case']['dest']['height'],
         c['case']['dest']['pixels'], c['case']['bitmap'], c['case']['x'], c['case']['y'], c['case']['mode'])
        for c in cases], workdir)
    port_seconds = time.perf_counter() - started
    results = []
    for c in cases:
        r, p = retail_results[c['name']], port_results.get(c['name'])
        entry = dict(case=c['name'], source=c['source'], mode=c['case']['mode'],
                     x=c['case']['x'], y=c['case']['y'])
        if not r.get('ok') or not p or not p.get('ok'):
            entry['error'] = dict(retail=r.get('error'), port=(p or {}).get('error', 'missing'))
        else:
            entry.update(compare_pixels(c['case']['dest']['width'], r['result']['pixels'], p['result']['pixels']))
        results.append(entry)
    case_hash = hashlib.sha256(json.dumps([c['case'] for c in cases], sort_keys=True).encode()).hexdigest()
    return dict(retail_sha256=retail.hello.get('retail_sha256'), case_set_sha256=case_hash,
                timing=dict(retail_seconds=round(retail_seconds, 2), port_seconds=round(port_seconds, 2)),
                results=results)


TARGETS = {'draw-put': target_draw_put}


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('target', choices=sorted(TARGETS))
    parser.add_argument('--out', type=Path, default=REPO / 'build' / 'hud_ab')
    parser.add_argument('--port', type=Path, default=PORT)
    parser.add_argument('--cases', type=int, default=200)
    parser.add_argument('--seed', type=int, default=1)
    args = parser.parse_args()
    workdir = args.out / args.target
    workdir.mkdir(parents=True, exist_ok=True)
    report = TARGETS[args.target](args, workdir)
    report.update(target=args.target, port_sha256=sha256(args.port), seed=args.seed)
    (workdir / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    results = report['results']
    failing = [r for r in results if r.get('error') or r.get('differing_pixels') or r.get('size_mismatch')]
    print(f'{args.target}: {len(results) - len(failing)}/{len(results)} cases match '
          f'(retail {report["timing"]["retail_seconds"]}s, port {report["timing"]["port_seconds"]}s); '
          f'report {workdir / "report.json"}')
    for r in failing[:12]:
        print('  ', json.dumps(r))
    sys.exit(1 if failing else 0)


if __name__ == '__main__':
    main()
