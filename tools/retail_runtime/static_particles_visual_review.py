#!/usr/bin/env python3
"""Freeze matched full-frame native/Metal evidence without fitted transforms."""
import argparse
import hashlib
import json
import re
from pathlib import Path
from PIL import Image, ImageChops, ImageDraw, ImageStat


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def image_error(a, b):
    return sum(ImageStat.Stat(ImageChops.difference(a, b)).sum)


def run(native, metal, output, negative=None):
    output.mkdir(parents=True, exist_ok=False)
    reference = json.loads((native / 'manifest.json').read_text())
    cases = []
    for name in dict.fromkeys(c['name'] for c in reference['cases']):
        destination = output / name
        destination.mkdir()
        rows = [c for c in reference['cases'] if c['name'] == name]
        panel = Image.new('RGB', (1536, 404 * len(rows)))
        detail = Image.new('RGB', (768, 276 * len(rows)))
        pairs, captures, cadence_checks = [], {}, []
        for policy in ('native', 'common'):
            folder = metal / (name + '-' + policy)
            manifest = json.loads((folder / 'manifest.json').read_text())
            assert manifest['status'] == 'pass' and manifest['exit_code'] == 0
            assert manifest['scenario']['warmup'] == 1
            assert manifest['scenario']['fps'] == 24
            assert manifest['scenario']['origin'] == [0, 0, 0]
            assert manifest['scenario']['camera'] == [512, 384]
            assert manifest['scenario']['native_domain'] == (policy == 'native')
            captures[policy] = dict(path=str(folder / 'manifest.json'),
                                    sha256=sha(folder / 'manifest.json'),
                                    binary_sha256=manifest['binary_sha256'])
            cadence = json.loads((native / name / 'cadence.json').read_text())
            for frame, count in re.findall(r'before_render=(\d+) global_random_draws=(\d+)',
                                            (folder / 'run.log').read_text()):
                actual = cadence[int(frame)-1]['rng_before_draws']
                assert actual == int(count), (name, policy, frame, actual, count)
                cadence_checks.append(dict(policy=policy, before_render=int(frame), draws=actual))
        for index, row in enumerate(rows):
            number = row['capture']
            assert number > 0  # Render0 is the explicitly discarded warmup.
            original = Path(row['image'])
            assert sha(original) == row['image_sha256']
            native_image = Image.open(original).convert('RGB')
            assert native_image.size == (1024, 768)
            paths = [original] + [next((metal / (name + '-' + policy)).glob(f'frame-{number:03d}-*.png'))
                                  for policy in ('native', 'common')]
            pair = dict(capture=number, native=dict(path=str(original), sha256=sha(original)), modern={})
            for column, (label, path) in enumerate(zip(('native', 'Metal native', 'Metal common'), paths)):
                image = Image.open(path).convert('RGB')
                assert image.size == native_image.size
                panel.paste(image.resize((512, 384)), (column*512, index*404+20))
                detail.paste(image.crop((384, 256, 640, 512)), (column*256, index*276+20))
                ImageDraw.Draw(panel).text((column*512+4, index*404+3), f'{name} sample {number} {label}', fill='white')
                ImageDraw.Draw(detail).text((column*256+4, index*276+3), f'{name} {number} {label}', fill='white')
                if column:
                    policy = ('native', 'common')[column-1]
                    record = dict(path=str(path), sha256=sha(path), rgb_l1_error=image_error(native_image, image))
                    if negative:
                        old = next((negative / (name + '-' + policy)).glob(f'frame-{number:03d}-*.png'))
                        record['prior_current_owner_pose'] = dict(path=str(old), sha256=sha(old),
                            rgb_l1_error=image_error(native_image, Image.open(old).convert('RGB')))
                    pair['modern'][policy] = record
            pairs.append(pair)
        panel.save(destination / 'whole-life.png')
        detail.save(destination / 'detail.png')
        report = dict(name=name, status='awaiting_independent_visual_review', accepted=False,
            strict_pixel_parity=False, native_manifest_sha256=sha(native / 'manifest.json'),
            viewport=[1024, 768], whole_life_display_scale=0.5, detail_crop=[384, 256, 640, 512],
            cadence='24Hz, seed1, warmup1; native captureN is Metal film frameN (N>=1), PulseN+1; no time alignment fit',
            capture_manifests=captures, rng_checks=cadence_checks, pairs=pairs,
            panels={n:dict(path=str(destination/n),sha256=sha(destination/n))
                    for n in ('whole-life.png','detail.png')},
            limitations='Full original particle draw caller on black, declared immutable resource/owner/COM/ground interfaces. Independent Metal factory runtime. No natural spell/caster/character/device acceptance or exact pixels. Detail crop is supplementary; whole-life panel includes full viewport.')
        (destination / 'pair-manifest.json').write_text(json.dumps(report, indent=2)+'\n')
        cases.append(dict(name=name, manifest=str(destination / 'pair-manifest.json'),
                          sha256=sha(destination / 'pair-manifest.json')))
    report = dict(status='awaiting_independent_visual_review', accepted=False, cases=cases,
                  script_sha256=sha(Path(__file__)))
    (output / 'manifest.json').write_text(json.dumps(report, indent=2)+'\n')
    return report


if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--native', type=Path, required=True)
    p.add_argument('--metal', type=Path, required=True)
    p.add_argument('--negative-control', type=Path)
    p.add_argument('--output', type=Path, required=True)
    a = p.parse_args()
    run(a.native, a.metal, a.output, a.negative_control)
