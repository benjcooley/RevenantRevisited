#!/usr/bin/env python3
"""Fit the modern lighting settings against retail, over every location.

    sweep.py NAME KEY=VALUE [KEY=VALUE ...] [--retail-run base] [--skip id ...]

Writes a scratch overlay (the repo's revisited/resources with a Revenant.ini
holding LightingMode = 1 and the given [Revisited] keys), captures every
location in modern (port_capture.py), and scores each shot against retail's
(compare.py's statistics on the aligned map):

- luma: the port's mean luma over retail's, as a ratio;
- p90: the same for the 90th luma percentile (how bright the bright parts get);
- chroma: the port's mean saturation over retail's.

Prints a row per location and the geometric means and their spread; 1.0 is
retail. docs/lighting/LIGHTING_AB.md §6.4.
"""
from __future__ import annotations

import argparse
import json
import math
import shutil
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[1]
sys.path.insert(0, str(HERE))
from compare import OUT, compare  # noqa: E402


def overlay(name: str, settings: dict[str, str]) -> Path:
    root = OUT / 'overlays' / name
    if root.exists():
        shutil.rmtree(root)
    shutil.copytree(REPO / 'revisited' / 'resources', root)
    ini = {'LightingMode': '1', **settings}
    (root / 'Revenant.ini').write_text('[Revisited]\n' + ''.join(f'{k} = {v}\n' for k, v in ini.items()))
    return root


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('name')
    p.add_argument('settings', nargs='*')
    p.add_argument('--retail-run', default='base')
    p.add_argument('--retail', default='retail-no')
    p.add_argument('--skip', nargs='*', default=[])
    p.add_argument('-j', type=int, default=4)
    a = p.parse_args(argv)
    settings = dict(kv.split('=', 1) for kv in a.settings)
    root = overlay(a.name, settings)
    locations = [l['id'] for l in json.loads((HERE / 'locations.json').read_text())['locations']
                 if l['id'] not in a.skip]
    run = f'sweep-{a.name}'
    subprocess.run([sys.executable, str(HERE / 'port_capture.py'), '--run', run, '--mode', 'modern',
                    '--overlay', str(root), '-j', str(a.j), *locations],
                   check=True, stdout=subprocess.DEVNULL)
    ratios = {'luma': [], 'p90': [], 'chroma': []}
    for ident in locations:
        rp = OUT / a.retail_run / a.retail / f'{ident}.png'
        pp = OUT / run / 'modern' / f'{ident}.png'
        if not rp.exists() or not pp.exists():
            continue
        c = compare(rp, pp, OUT / run / f'compare-vs-{a.retail}' / f'{ident}.png')
        r, q = c['retail'], c['port']
        row = {k: q[k2] / max(r[k2], 1.0) for k, k2 in (('luma', 'luma'), ('p90', 'p90'), ('chroma', 'chroma'))}
        for k, v in row.items():
            ratios[k].append(v)
        print(f'{ident:26s} luma {row["luma"]:.2f}  p90 {row["p90"]:.2f}  chroma {row["chroma"]:.2f}', flush=True)
    summary = []
    for k, vs in ratios.items():
        logs = [math.log(v) for v in vs if v > 0]
        mean = math.exp(sum(logs) / len(logs))
        spread = math.exp(math.sqrt(sum((x - math.log(mean)) ** 2 for x in logs) / len(logs)))
        summary.append(f'{k} {mean:.2f} (x/ {spread:.2f})')
    print(f'{a.name}: {settings} -> ' + ', '.join(summary))


if __name__ == '__main__':
    main()
