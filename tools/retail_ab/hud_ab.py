#!/usr/bin/env python3
"""HUD A/B against the shipped game's own code (docs/ui/HUD_REBUILD.md).

    hud_ab.py TARGET [--out DIR] [--port BIN] [--only NAME ...]

One command per target. Per case it runs the port's production pane in a
headless --test=ab-* host, which reports what the pane showed (names,
stats, the portraits' bitmaps), then runs retail's original pane on exactly
those inputs in the emulator's HUD slot (one persistent fixture process: it
loads the boot image and restores a checkpoint per case). It compares the
two frames and writes, per case, a retail | port | diff triptych. Then it
prints a summary. DIR/report.json records the retail build hash, the port
binary hash, the case set and every case's result.

Targets:
  statusbar  TPlyrStatusBar (0x0065a8c0): the player's and opponent's chips
             over stat values, fills that hit the bar caps, target present /
             absent, and each fade tick.

             Text: the emulator draws no GDI glyphs yet (HUD_REBUILD.md P6),
             so the frames are compared outside retail's text cells, which
             retail blits once a chip passes half fade. Port pixels that
             differ within TEXT_BAND of a cell are reported as text overflow,
             open until P6, apart from the chrome defects.

             Retail composes the chips into its textures by its own code;
             the fixture draws the overlay quads as the D3D device does
             (overlayraster.py), fades included.

Both frames are quantised to the 16-bit screen retail draws (RGB565). A
pixel must match exactly, unless the fixture marks it (the masks of
tools/retail_runtime/slots/hud/plyrstatusbar.py):
- `screen`: a quad's blend mixed it (partial alpha, or a fade). Both sides
  blend in float (the port's layers are stored premultiplied in 8 bits
  first); they match within SCREEN_TOLERANCE, one RGB565 step (8 or 9 of
  255: the channels widen by bit replication).
- `composed`: it is drawn from a texel retail blended into its ARGB4444
  texture in 4-bit steps (blendmap.py); the port blends those layers in
  float (HUD_REBUILD.md section 5). They match within COMPOSED_TOLERANCE:
  two nested 4-bit truncations (17 each: the Ring over the portrait over
  the BackPanel) and the RGB565 step.
A channel off by more is a defect.

Environment / defaults:
  RETAIL_PY   the emulator's Python (the retail-asm venv)
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import subprocess
import sys
import time
import zlib
from pathlib import Path

import numpy as np
from PIL import Image

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[1]
SLOT = REPO / 'tools' / 'retail_runtime' / 'slots' / 'hud'
RETAIL_EXE = REPO / 'recon' / 'retail_asm' / 'baseline' / 'Revenant.rebuilt.exe'
RETAIL_PY = Path(os.environ.get('RETAIL_PY', '/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python'))
PORT = REPO / 'build' / 'Revenant'

SCREEN_TOLERANCE = 9
COMPOSED_TOLERANCE = 2 * 17 + SCREEN_TOLERANCE
SCREEN = (640, 480)
TRIPTYCH_ROWS = 128               # the strip of the screen a triptych shows
TEXT_SHADOW = 2                   # the black shadow reaches 2 px right of and below a cell's text
TEXT_BAND = 3                     # port text that strays this far outside a cell is text overflow


def sha256(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


# =====================================================================
# Fixture processes
# =====================================================================

class Fixture:
    """A slot fixture speaking JSONL ({"id", "case", ...} -> {"id", "ok", ...})."""

    def __init__(self, script, *args):
        self.process = subprocess.Popen([str(RETAIL_PY), str(script), str(RETAIL_EXE), *args],
                                        stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)
        self.hello = json.loads(self.process.stdout.readline())

    def run(self, case_id, case, **fields):
        self.process.stdin.write(json.dumps(dict(id=case_id, case=case, **fields)) + '\n')
        self.process.stdin.flush()
        return json.loads(self.process.stdout.readline())

    def close(self):
        self.process.stdin.close()
        self.process.wait(timeout=30)


# =====================================================================
# Frames
# =====================================================================

def quantise_565(rgb):
    """8-bit RGB -> the RGB565 screen retail draws, expanded back to 8 bits
    the way the capture PNGs are (bit replication)."""
    r, g, b = rgb[..., 0] >> 3, rgb[..., 1] >> 2, rgb[..., 2] >> 3
    return np.stack([(r << 3) | (r >> 2), (g << 2) | (g >> 4), (b << 3) | (b >> 2)], axis=-1)


def retail_frame(path):
    words = np.frombuffer(Path(path).read_bytes(), dtype='<u2').reshape(SCREEN[1], SCREEN[0]).astype(np.int32)
    r, g, b = words >> 11, (words >> 5) & 0x3f, words & 0x1f
    return np.stack([(r << 3) | (r >> 2), (g << 2) | (g >> 4), (b << 3) | (b >> 2)], axis=-1)


def port_frame(path):
    image = Image.open(path).convert('RGB')
    if image.size != SCREEN:
        raise ValueError(f'{path}: {image.size}, not the {SCREEN} Classic screen')
    return quantise_565(np.asarray(image).astype(np.int32))


def compare(retail, port, mask, band, tolerance):
    """Per-pixel channel deltas outside `mask` (True = not compared), each
    allowed up to `tolerance` there; one over that inside `band` counts as
    text overflow, elsewhere as a defect."""
    delta = np.abs(retail - port).max(axis=-1)
    delta[mask] = 0
    over = delta > tolerance
    bad = over & ~band
    worst = lambda where: int(np.where(where & ~band, delta, 0).max())  # noqa: E731
    result = dict(compared=int((~mask).sum()), exact=int(((delta == 0) & ~mask).sum()),
                  within_tolerance=int(((delta > 0) & ~over).sum()), defects=int(bad.sum()),
                  text_overflow=int((over & band).sum()),
                  max_delta=dict(exact=worst(tolerance == 0), screen=worst(tolerance == SCREEN_TOLERANCE),
                                 composed=worst(tolerance == COMPOSED_TOLERANCE)))
    if bad.any():
        ys, xs = np.nonzero(bad)
        result['defect_bbox'] = [int(xs.min()), int(ys.min()), int(xs.max()) + 1, int(ys.max()) + 1]
        y, x = int(ys[0]), int(xs[0])
        result['first_defect'] = dict(x=x, y=y, retail=retail[y, x].tolist(), port=port[y, x].tolist())
    return result, delta


def triptych(path, retail, port, delta, mask, band, tolerance):
    """retail | port | diff, stacked, the top TRIPTYCH_ROWS rows, 2x. The diff
    shows deltas within tolerance in grey, defects in red, text overflow in
    yellow and the masked text cells in blue."""
    rows = slice(0, TRIPTYCH_ROWS)
    diff = np.zeros_like(retail[rows])
    d, m, b = delta[rows], mask[rows], band[rows]
    over = d > tolerance[rows]
    diff[(d > 0) & ~over] = (96, 96, 96)
    diff[over & ~b] = (255, 48, 48)
    diff[over & b] = (230, 200, 40)
    diff[m] = (24, 32, 96)
    strip = np.concatenate([retail[rows], port[rows], diff], axis=0).astype(np.uint8)
    image = Image.fromarray(strip, 'RGB')
    image.resize((image.width * 2, image.height * 2), Image.NEAREST).save(path)


# =====================================================================
# statusbar
# =====================================================================

# (name, player fills, opponent fills or None, ticks). Fills are health,
# mana and fatigue as fractions of the port's demo characters' maxima; the
# port reports the values they come to and retail runs on those. 'near-caps'
# and 'far-caps' land a bar's fill on either side of the cap thresholds;
# 'fade-N' stops at tick N of the six-tick fade (text shows from tick 4).
SAMPLE = ((0.95, 0.82, 0.50), (0.40, 0.10, 0.60))
STATUSBAR_CASES = [
    ('sample', *SAMPLE, 6),
    ('no-target', (1.0, 1.0, 1.0), None, 6),
    ('full', (1.0, 1.0, 1.0), (1.0, 1.0, 1.0), 6),
    ('empty', (0.0, 0.0, 0.0), (0.0, 0.0, 0.0), 6),
    ('near-caps', (0.02, 0.03, 0.04), (0.03, 0.02, 0.05), 6),
    ('far-caps', (0.951, 0.95, 0.93), (0.95, 0.951, 0.97), 6),
    *[(f'sweep-{i:02d}', (i / 10, (10 - i) / 10, (i * 7 % 11) / 10),
       ((10 - i) / 10, i / 10, (i * 3 % 11) / 10), 6) for i in range(1, 10)],
    *[(f'fade-{tick}', *SAMPLE, tick) for tick in range(1, 6)],
]


def fills(values):
    return ','.join(f'{v:g}' for v in values)


def run_port_statusbar(port, name, player, target, ticks, workdir):
    """The production pane in --test=ab-plyrstatusbar -> (shown, frame path)."""
    shown, frame, log = (workdir / f'{name}.port.json', workdir / f'{name}.port.png',
                         workdir / f'{name}.port.log')
    for stale in (shown, frame):
        stale.unlink(missing_ok=True)
    spec = f'player={fills(player)};target={fills(target) if target else "none"};ticks={ticks}'
    with log.open('wb') as handle:
        subprocess.run([str(port), '--headless', '--test=ab-plyrstatusbar', f'--ab-case={spec}',
                        f'--ab-out={shown}', f'--snap={frame}', '--snapstep=0.03125', '--snapwarmup=12',
                        '--max-runtime=60'], cwd=REPO, stdout=handle, stderr=handle, timeout=120, check=False)
    if not shown.is_file() or not frame.is_file():
        raise RuntimeError(f'port wrote no {shown.name if not shown.is_file() else frame.name} (see {log})')
    return json.loads(shown.read_text()), frame


def text_cells(primitives):
    """The screen rects retail blits text cells to (its 2D pass, once a chip
    passes half fade) -> (mask: the cells widened by the shadow, band: the
    TEXT_BAND around the mask, cells)."""
    mask = np.zeros((SCREEN[1], SCREEN[0]), dtype=bool)
    band = np.zeros_like(mask)
    cells = []
    for p in primitives:
        if p['primitive'] == 'ParamBlit' and p['dst'] == 'display' and p['src'] == 'text':
            x, y, w, h = p['drawparam'][10:14]
            if [x, y, w, h] not in cells:
                cells.append([x, y, w, h])
                mask[y:y + h + TEXT_SHADOW, x:x + w + TEXT_SHADOW] = True
                band[max(0, y - TEXT_BAND):y + h + TEXT_SHADOW + TEXT_BAND,
                     max(0, x - TEXT_BAND):x + w + TEXT_SHADOW + TEXT_BAND] = True
    return mask, band & ~mask, cells


def screen_mask(packed):
    """A fixture mask: one byte per screen pixel, zlib, hex."""
    return np.frombuffer(zlib.decompress(bytes.fromhex(packed)), dtype=np.uint8).reshape(
        SCREEN[1], SCREEN[0]).astype(bool)


def tolerances(blend):
    """Per pixel: COMPOSED_TOLERANCE, SCREEN_TOLERANCE or 0 (exact)."""
    return np.where(screen_mask(blend['composed']), COMPOSED_TOLERANCE,
                    np.where(screen_mask(blend['screen']), SCREEN_TOLERANCE, 0))


def target_statusbar(args, workdir):
    cases = [c for c in STATUSBAR_CASES if not args.only or c[0] in args.only]
    retail = Fixture(SLOT / 'plyrstatusbar.py', '--serve')
    results, timing = [], dict(port_seconds=0.0, retail_seconds=0.0)
    for name, player, target, ticks in cases:
        entry = dict(case=name, ticks=ticks)
        try:
            started = time.perf_counter()
            shown, port_png = run_port_statusbar(args.port, name, player, target, ticks, workdir)
            timing['port_seconds'] += time.perf_counter() - started
            retail_case = dict(name=name, player=shown['player'], target=shown['target'],
                               frames=shown['ticks'], pane_width=shown['pane'][2])
            started = time.perf_counter()
            r = retail.run(name, retail_case, out=str(workdir / 'retail'))
            timing['retail_seconds'] += time.perf_counter() - started
            if not r.get('ok'):
                raise RuntimeError(f'retail: {r.get("error")}')
            result = r['result']
            mask, band, cells = text_cells(result['primitives'])
            retail_rgb = retail_frame(workdir / 'retail' / f'{name}.rgb565')
            port_rgb = port_frame(port_png)
            tolerance = tolerances(result['blend'])
            diff, delta = compare(retail_rgb, port_rgb, mask, band, tolerance)
            triptych(workdir / f'{name}.triptych.png', retail_rgb, port_rgb, delta, mask, band, tolerance)
            entry.update(diff, fades=result['fades'], text_cells=cells,
                         shown={side: None if not shown[side] else
                                {k: v for k, v in shown[side].items() if k != 'portrait'}
                                for side in ('player', 'target')})
        except Exception as error:      # one case's failure is reported, not fatal
            entry['error'] = f'{type(error).__name__}: {error}'
        results.append(entry)
    retail.close()
    return dict(retail_sha256=retail.hello.get('retail_sha256'), tolerance=dict(screen=SCREEN_TOLERANCE, composed=COMPOSED_TOLERANCE),
                case_set=[dict(name=c[0], player=c[1], target=c[2], ticks=c[3]) for c in cases],
                timing={k: round(v, 2) for k, v in timing.items()}, results=results)


TARGETS = {'statusbar': target_statusbar}


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('target', choices=sorted(TARGETS))
    parser.add_argument('--out', type=Path, default=REPO / 'build' / 'hud_ab')
    parser.add_argument('--port', type=Path, default=PORT)
    parser.add_argument('--only', nargs='*', help='run only the named cases')
    args = parser.parse_args()
    workdir = args.out / args.target
    workdir.mkdir(parents=True, exist_ok=True)
    report = TARGETS[args.target](args, workdir)
    report.update(target=args.target, port_sha256=sha256(args.port))
    (workdir / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    results = report['results']
    failing = [r for r in results if r.get('error') or r.get('defects')]
    print(f'{args.target}: {len(results) - len(failing)}/{len(results)} cases match '
          f'(port {report["timing"]["port_seconds"]}s, retail {report["timing"]["retail_seconds"]}s); '
          f'report {workdir / "report.json"}')
    for r in results:
        if r.get('error'):
            print(f'  {r["case"]}: {r["error"]}')
        else:
            worst = r['max_delta']
            print(f'  {r["case"]}: {r["defects"]} defects, {r["exact"]}/{r["compared"]} exact, '
                  f'{r["within_tolerance"]} within tolerance (max delta: exact {worst["exact"]}, '
                  f'screen {worst["screen"]}, composed {worst["composed"]}), {r["text_overflow"]} text overflow'
                  + (f', defects in {r["defect_bbox"]}' if r.get('defect_bbox') else ''))
    sys.exit(1 if failing else 0)


if __name__ == '__main__':
    main()
