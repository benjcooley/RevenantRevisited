#!/usr/bin/env python3
"""Pair current map captures with retained, exact-type retail static recordings.

Independent capture processes and private save directories permit family batches.
No appearance pass is inferred: each pair needs an actual visual review.
"""
import argparse
from concurrent.futures import ThreadPoolExecutor, as_completed
import hashlib
import json
from pathlib import Path
import subprocess

from PIL import Image, ImageDraw
from map_texture_probe import run as capture_map

ROOT = Path(__file__).resolve().parents[2]


def record(path):
    path = Path(path).resolve()
    return dict(path=str(path), sha256=hashlib.sha256(path.read_bytes()).hexdigest())


def capture_pair(row, binary, data, lab, output):
    folder = output / row['retail_name']
    folder.mkdir()
    comparison = lab / row['comparison_runs'][-1] / 'manifest.json'
    old = json.loads(comparison.read_text())
    reference = lab / old['reference']
    native_manifest = reference / 'manifest.json'
    metadata = json.loads(native_manifest.read_text())
    avi = reference / 'retail.avi'
    if record(avi)['sha256'] != old['reference_sha256']:
        raise ValueError('Retained reference hash differs: ' + row['key'])
    if record(native_manifest)['sha256'] != old['reference_manifest_sha256']:
        raise ValueError('Retained reference manifest differs: ' + row['key'])
    if not metadata.get('static_effect_expected') or metadata['distinct_roi_frames'] != 1:
        raise ValueError('Reference must explicitly verify one static image: ' + row['key'])
    native = folder / 'retail-frame-000.png'
    subprocess.run(['ffmpeg', '-v', 'error', '-i', str(avi), '-frames:v', '1', str(native)],
                   check=True, timeout=30)
    # Sign keys are constant while their unused100-frame owner state advances.
    profile = dict(name=row['retail_name'], id=row['retail_type_id'],
                   owner_frames=100 if row['retail_name'].endswith('Sign') else 1,
                   render_frames=1)
    capture = capture_map(binary, data, 'FontRuntimeLab', (110, 10000, 10000, 16),
                          folder / 'modern', [profile])
    if capture['status'] != 'pass':
        raise RuntimeError('Current map capture failed: ' + row['key'])
    modern = Path(capture['frames'][3]['path'])
    roi = old['roi']
    rectangle = tuple(roi)
    width, height = roi[2] - roi[0], roi[3] - roi[1]
    sheet = Image.new('RGB', (width * 6, height * 3 + 24), (24, 24, 24))
    for i, path in enumerate((native, modern)):
        with Image.open(path) as image:
            crop = image.convert('RGB').crop(rectangle)
            sheet.paste(crop.resize((width * 3, height * 3), Image.Resampling.NEAREST),
                        (i * width * 3, 24))
    labels = ImageDraw.Draw(sheet)
    labels.text((4, 5), row['retail_name'] + ' / original software', fill='white')
    labels.text((width * 3 + 4, 5), 'Current actual map / Metal', fill='white')
    contact = folder / 'comparison.png'
    sheet.save(contact)
    report = dict(status='awaiting_manual_visual_review', retail_row=row['key'],
                  profile=profile, reference_manifest=record(native_manifest),
                  native_recording=record(avi), native_frame=record(native),
                  modern_manifest=record(folder / 'modern/manifest.json'),
                  modern_frame=record(modern), contact=record(contact),
                  binary_sha256=capture['binary_sha256'], roi=roi,
                  native_camera=metadata['camera'], native_ambient=metadata['ambient'],
                  modern_camera=[110, 10000, 10000, 16], modern_ambient=32,
                  reference_frame_index=0, appearance_fitted=False,
                  each_original_background_retained=True, automatic_visual_credit=False,
                  scope='Recorded static configuration only. Actual port/native pixels retained; '
                        'sampling, projection, illumination and natural-context findings need review.')
    (folder / 'pair-manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    return report


def run(binary, data, lab, output, names=None, workers=2):
    if not 1 <= workers <= 4:
        raise ValueError('Use one to four private capture processes')
    output.mkdir(parents=True, exist_ok=False)
    ledger = json.loads((ROOT / 'docs/vfx/EFFECT_BURNDOWN.json').read_text())
    rows = [r for r in ledger['retail_effects'] if r['retail_name'].endswith('Sign') or
            (r['retail_name'].startswith('Ribbon') and r['retail_name'] != 'Ribbon')]
    if names:
        rows = [r for r in rows if r['retail_name'] in names]
    results, failures = [], []
    with ThreadPoolExecutor(max_workers=workers) as pool:
        jobs = {pool.submit(capture_pair, row, binary, data, lab, output): row for row in rows}
        for future in as_completed(jobs):
            row = jobs[future]
            try:
                results.append(future.result())
                print(row['key'], 'pair ready', flush=True)
            except Exception as error:
                failures.append(dict(retail_row=row['key'], error=str(error)))
                print(row['key'], 'bounded failure:', error, flush=True)
    report = dict(status='awaiting_manual_visual_review' if not failures else 'bounded_failures',
                  binary=record(binary), probe=record(__file__), cases=results, failures=failures,
                  automatic_visual_credit=False)
    (output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--binary', type=Path, required=True)
    parser.add_argument('--data-root', type=Path, required=True)
    parser.add_argument('--lab-root', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--names', nargs='+')
    parser.add_argument('--workers', type=int, default=2)
    args = parser.parse_args()
    run(args.binary.resolve(), args.data_root.resolve(), args.lab_root.resolve(),
        args.output.resolve(), args.names, args.workers)
