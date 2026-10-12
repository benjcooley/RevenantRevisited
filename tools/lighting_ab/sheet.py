#!/usr/bin/env python3
"""Side-by-side sheets: retail and modern variants, location by location.

    sheet.py OUT.png --columns retail=base/retail-no ref=sweep-ref/modern a=sweep-a/modern ...
             [--locations id ...] [--retail-column retail]

Each column is a directory of shots under ~/RevenantRevisited-lightab; each
row a location. The map area of every port shot is aligned to the retail
shot (compare.py's alignment) and cropped, so rows line up for the eye.
docs/lighting/LIGHTING_AB.md §5.
"""
from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw

from compare import OUT, align, load, mask, shift

CROP = (0, 40, 452, 420)             # the map, without the side panel or the bottom bar


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('out', type=Path)
    p.add_argument('--columns', nargs='+', required=True, help='LABEL=DIR, the first is the reference')
    p.add_argument('--locations', nargs='+', required=True)
    p.add_argument('--scale', type=float, default=0.75)
    a = p.parse_args(argv)
    cols = [c.split('=', 1) for c in a.columns]
    w = int((CROP[2] - CROP[0]) * a.scale)
    h = int((CROP[3] - CROP[1]) * a.scale)
    head, label_h = 18, 16
    sheet = Image.new('RGB', (w * len(cols), head + (h + label_h) * len(a.locations)), (24, 24, 24))
    d = ImageDraw.Draw(sheet)
    for c, (label, _) in enumerate(cols):
        d.text((c * w + 4, 3), label, fill=(255, 255, 255))
    m = mask()
    for r, ident in enumerate(a.locations):
        y = head + r * (h + label_h)
        d.text((4, y + 1), ident, fill=(255, 220, 0))
        ref_path = OUT / cols[0][1] / f'{ident}.png'
        ref = load(ref_path) if ref_path.exists() else None
        for c, (_, directory) in enumerate(cols):
            path = OUT / directory / f'{ident}.png'
            if not path.exists():
                continue
            img = load(path)
            if ref is not None and c > 0:
                dx, dy = align(ref, img, m)
                img = shift(img, dx, dy)
            tile = Image.fromarray(np.clip(img, 0, 255).astype(np.uint8)).crop(CROP).resize((w, h))
            sheet.paste(tile, (c * w, y + label_h))
    a.out.parent.mkdir(parents=True, exist_ok=True)
    sheet.save(a.out)
    print(a.out)


if __name__ == '__main__':
    main()
