#!/usr/bin/env python3
"""Measure real gallery draws without labels; this does not grant retail parity."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import time
from PIL import Image

ROOT = Path(__file__).resolve().parents[2]


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def capture(row, args, binary_hash):
    name = re.sub(r'[^A-Za-z0-9_.-]', '_', row['retail_name'])
    output = args.output / (row['retail_type_id'] + '-' + name)
    output.mkdir()
    save = output / 'save'
    save.mkdir()
    shutil.copyfile(args.data_root / 'revenant.ini', save / 'Revenant.ini')
    step = 1 / 24
    interval = 1.3 if args.approach else 1 / 6
    command = [str(args.binary), '--headless', '--test=vfx-review', '--vfx-no-ui',
        '--resolution=640x480', '--vfx-review-effects=' + row['retail_type_id'],
        '--vfx-review-gap=720', '--vfx-review-speed=' + ('48' if args.approach else '1'),
        '--filmstrip=' + str(args.frames) + ',' + str(interval),
        '--snapstep=' + str(step), '--snapwarmup=1', '--snapseed=1',
        '--snapprefix=' + str(output / 'frame-')]
    command += ['--vfx-review-offset=70'] if args.approach else ['--vfx-review-first']
    env = os.environ.copy()
    env.update(REVENANT_DATA_PATH=str(args.data_root), REVENANT_SAVE_PATH=str(save),
               REVENANT_ASSETS_PATH=str(args.assets_root))
    start = time.monotonic()
    timed_out = False
    with (output / 'run.log').open('wb') as log:
        try:
            process = subprocess.run(command, cwd=ROOT, env=env, stdout=log,
                                     stderr=log, timeout=args.timeout)
            code = process.returncode
        except subprocess.TimeoutExpired:
            timed_out = True
            code = None
    log_text = (output / 'run.log').read_text(errors='replace')
    frames = sorted(output.glob('frame-[0-9][0-9][0-9]-*.png'))
    samples = []
    for path in frames:
        with Image.open(path) as source:
            pixels = source.convert('RGB')
            assert pixels.size == (640, 480), pixels.size
            samples.append(dict(path=str(path), sha256=sha(path),
                pixel_sha256=hashlib.sha256(pixels.tobytes()).hexdigest(),
                bbox=pixels.getbbox()))
    visible = sum(sample['bbox'] is not None for sample in samples)
    unavailable = '[vfx-review] unavailable name=' in log_text
    complete = (code == 0 and not timed_out and len(samples) == args.frames and
                '[vfx-review] ready types=1 ' in log_text and sha(args.binary) == binary_hash)
    status = ('capture_failed' if not complete else 'visible' if visible else
              'unavailable' if unavailable else 'render_empty')
    report = dict(type_id=row['retail_type_id'], name=row['retail_name'], status=status,
        phase='approach' if args.approach else 'centered', command=command,
        binary_sha256=binary_hash, exit_code=code, timeout=timed_out,
        elapsed_seconds=time.monotonic()-start, visible_frames=visible,
        distinct_frames=len({sample['pixel_sha256'] for sample in samples}),
        samples=samples, routes=[line for line in log_text.splitlines()
            if '[vfx-review]' in line and any(word in line for word in
                ('unavailable name=', 'preview name=', 'spawn name=', 'projectile name='))],
        scope='Full-frame actual port gallery with labels suppressed. Visible pixels and motion are diagnostics, not effect correctness or retail acceptance.')
    (output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    print(name + ': ' + status + ' (' + str(visible) + '/' + str(len(samples)) + ')', flush=True)
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--binary', type=Path, default=ROOT / 'build-merge/Revenant')
    parser.add_argument('--data-root', type=Path, required=True)
    parser.add_argument('--assets-root', type=Path, default=ROOT / 'assets')
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--effects', help='Comma-separated exact names or hexadecimal type IDs')
    parser.add_argument('--reviewed', action='store_true')
    parser.add_argument('--approach', action='store_true', help='Record normal scroll-in from 650px away')
    parser.add_argument('--frames', type=int, default=12)
    parser.add_argument('--workers', type=int, default=2)
    parser.add_argument('--timeout', type=float, default=90)
    args = parser.parse_args()
    if not 1 <= args.workers <= 4 or not 2 <= args.frames <= 120:
        parser.error('workers must be 1..4 and frames 2..120')
    for field in ('binary', 'data_root', 'assets_root', 'output'):
        setattr(args, field, getattr(args, field).resolve())
    ledger = ROOT / 'docs/vfx/EFFECT_BURNDOWN.json'
    rows = json.loads(ledger.read_text())['retail_effects']
    if args.reviewed:
        rows = [row for row in rows if row['acceptance']['visual_fidelity']]
    if args.effects:
        selected = {value.strip().lower() for value in args.effects.split(',')}
        matches = [row for row in rows if row['retail_name'].lower() in selected or row['retail_type_id'] in selected]
        found = {value for value in selected if any(value in (row['retail_name'].lower(), row['retail_type_id']) for row in matches)}
        if found != selected:
            parser.error('Unknown or filtered effect: ' + ', '.join(sorted(selected-found)))
        rows = matches
    args.output.mkdir(parents=True, exist_ok=False)
    binary_hash = sha(args.binary)
    with ThreadPoolExecutor(max_workers=args.workers) as pool:
        reports = list(pool.map(lambda row: capture(row, args, binary_hash), rows))
    counts = {status: sum(report['status'] == status for report in reports)
              for status in ('visible', 'unavailable', 'render_empty', 'capture_failed')}
    report = dict(binary_sha256=binary_hash, ledger_sha256=sha(ledger), counts=counts,
                  effects=reports, scope='Gallery runtime diagnostics only; no retail fidelity credit.')
    (args.output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(counts), flush=True)


if __name__ == '__main__':
    main()
