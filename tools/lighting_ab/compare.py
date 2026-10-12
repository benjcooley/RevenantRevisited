#!/usr/bin/env python3
"""Compare the port's shots with retail's, location by location.

    compare.py --run NAME [--port classic] [--retail retail-no] [location ...]

Each port shot is aligned to its retail shot by the integer offset that
best matches the map (retail sometimes hides the bottom bar, which moves
the camera by half its height), over the map area only: the HUD (status
corner, side panel and its buttons, the text log, the bottom bar) is
masked. Per location it reports:

- exact: the share of map pixels whose channels are all within 3 of 255
  of retail's (Classic's target is identity; characters, effects and
  animation phase show up as compact blobs and are judged by eye);
- near: the same within 12;
- blocks: the share of 16x16 blocks (wholly on the map) whose mean colour
  is within 4 of retail's in every channel: light level and colour,
  regardless of a pixel of registration or where a light's 6-bit steps
  fall;
- luma: mean, and 10th / 50th / 90th percentiles (Rec. 601, 0-255);
- chroma: mean saturation (HSV, 0-255);
- pool hue: the mean hue of the brightest quarter of the map (degrees).

and writes <run>/compare-<port>-vs-<retail>/<id>.png (retail | port |
difference x4) and report.md. docs/lighting/LIGHTING_AB.md §5.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

import numpy as np
from PIL import Image

HERE = Path(__file__).resolve().parent
OUT = Path.home() / 'RevenantRevisited-lightab'
MAP = (0, 64, 400, 372)            # x0, y0, x1, y1 of the compared map area (640x480 screen)
SEARCH = (24, 48)                  # +- dx, dy searched for the alignment
BLOCK = 16


def load(path: Path) -> np.ndarray:
    return np.asarray(Image.open(path).convert('RGB'), dtype=np.int16)


def mask() -> np.ndarray:
    m = np.zeros((480, 640), bool)
    x0, y0, x1, y1 = MAP
    m[y0:y1, x0:x1] = True
    m[0:96, 0:200] = False          # status corner
    return m


def align(retail: np.ndarray, port: np.ndarray, m: np.ndarray) -> tuple[int, int]:
    """The (dx, dy) such that port[y + dy, x + dx] best matches retail[y, x]."""
    r = retail.mean(axis=2)
    p = port.mean(axis=2)
    ys, xs = np.nonzero(m)
    best = None
    for coarse in (True, False):
        if coarse:
            cand = [(dx, dy) for dy in range(-SEARCH[1], SEARCH[1] + 1, 2) for dx in range(-SEARCH[0], SEARCH[0] + 1, 2)]
            sel = slice(None, None, 7)
        else:
            bx, by = best[1]
            cand = [(bx + dx, by + dy) for dy in (-1, 0, 1) for dx in (-1, 0, 1)]
            sel = slice(None)
        yy, xx = ys[sel], xs[sel]
        for dx, dy in cand:
            py, px = yy + dy, xx + dx
            ok = (py >= 0) & (py < 480) & (px >= 0) & (px < 640)
            err = np.abs(r[yy[ok], xx[ok]] - p[py[ok], px[ok]]).mean()
            if best is None or err < best[0]:
                best = (err, (dx, dy))
    return best[1]


def shift(img: np.ndarray, dx: int, dy: int) -> np.ndarray:
    out = np.zeros_like(img)
    h, w = img.shape[:2]
    ys, yd = (slice(dy, h), slice(0, h - dy)) if dy >= 0 else (slice(0, h + dy), slice(-dy, h))
    xs, xd = (slice(dx, w), slice(0, w - dx)) if dx >= 0 else (slice(0, w + dx), slice(-dx, w))
    out[yd, xd] = img[ys, xs]
    return out


def stats(img: np.ndarray, m: np.ndarray) -> dict:
    px = img[m].astype(np.float64)
    luma = px @ np.array([0.299, 0.587, 0.114])
    mx, mn = px.max(axis=1), px.min(axis=1)
    sat = np.where(mx > 0, (mx - mn) / np.maximum(mx, 1) * 255, 0)
    bright = luma >= np.percentile(luma, 75)
    b = px[bright]
    r, g, bl = b[:, 0], b[:, 1], b[:, 2]
    hue = np.degrees(np.arctan2(np.sqrt(3) * (g - bl), 2 * r - g - bl)) % 360
    hue_mean = np.degrees(np.arctan2(np.sin(np.radians(hue)).mean(), np.cos(np.radians(hue)).mean())) % 360
    return {'luma': luma.mean(), 'p10': np.percentile(luma, 10), 'p50': np.percentile(luma, 50),
            'p90': np.percentile(luma, 90), 'chroma': sat.mean(), 'hue': hue_mean}


def compare(retail_path: Path, port_path: Path, strip_path: Path) -> dict:
    retail, port = load(retail_path), load(port_path)
    m = mask()
    dx, dy = align(retail, port, m)
    port_a = shift(port, dx, dy)
    valid = m & (shift(np.ones((480, 640, 1), np.int16), dx, dy)[..., 0] > 0)
    diff = np.abs(retail - port_a).max(axis=2)
    blocks = []
    for by in range(0, 480 - BLOCK + 1, BLOCK):
        for bx in range(0, 640 - BLOCK + 1, BLOCK):
            if valid[by:by + BLOCK, bx:bx + BLOCK].all():
                d = np.abs(retail[by:by + BLOCK, bx:bx + BLOCK].mean(axis=(0, 1)) -
                           port_a[by:by + BLOCK, bx:bx + BLOCK].mean(axis=(0, 1))).max()
                blocks.append(d <= 4)
    out = {'dx': dx, 'dy': dy, 'blocks': float(np.mean(blocks)) if blocks else 0.0,
           'exact': float((diff[valid] <= 3).mean()), 'near': float((diff[valid] <= 12).mean()),
           'retail': stats(retail, valid), 'port': stats(port_a, valid)}
    heat = np.clip(np.abs(retail - port_a) * 4, 0, 255).astype(np.uint8)
    heat[~valid] = 0
    strip = Image.new('RGB', (1920, 480))
    strip.paste(Image.fromarray(retail.astype(np.uint8)), (0, 0))
    strip.paste(Image.fromarray(port_a.astype(np.uint8)), (640, 0))
    strip.paste(Image.fromarray(heat), (1280, 0))
    strip_path.parent.mkdir(parents=True, exist_ok=True)
    strip.save(strip_path)
    return out


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('locations', nargs='*')
    p.add_argument('--run', required=True)
    p.add_argument('--port', default='classic', help="the port shots' directory in the run")
    p.add_argument('--retail', default='retail-no')
    p.add_argument('--retail-run', help='take the retail shots from another run')
    a = p.parse_args(argv)
    locations = json.loads((HERE / 'locations.json').read_text())['locations']
    if a.locations:
        locations = [l for l in locations if l['id'] in a.locations]
    run = OUT / a.run
    retail_dir = OUT / (a.retail_run or a.run) / a.retail
    out_dir = run / f'compare-{a.port}-vs-{a.retail}'
    rows = ['| location | offset | blocks | exact | near | luma retail / port | p90 retail / port | chroma retail / port | pool hue retail / port |',
            '|---|---|---|---|---|---|---|---|---|']
    for loc in locations:
        rp, pp = retail_dir / f'{loc["id"]}.png', run / a.port / f'{loc["id"]}.png'
        if not rp.exists() or not pp.exists():
            continue
        c = compare(rp, pp, out_dir / f'{loc["id"]}.png')
        r, q = c['retail'], c['port']
        rows.append(f'| {loc["id"]} | {c["dx"]:+d},{c["dy"]:+d} | {c["blocks"]:.0%} | {c["exact"]:.0%} | {c["near"]:.0%} | '
                    f'{r["luma"]:.0f} / {q["luma"]:.0f} | {r["p90"]:.0f} / {q["p90"]:.0f} | '
                    f'{r["chroma"]:.0f} / {q["chroma"]:.0f} | {r["hue"]:.0f} / {q["hue"]:.0f} |')
        print(rows[-1], flush=True)
    (out_dir / 'report.md').write_text('\n'.join(rows) + '\n')
    print('wrote', out_dir / 'report.md')


if __name__ == '__main__':
    main()
