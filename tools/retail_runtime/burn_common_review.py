#!/usr/bin/env python3
"""Unaltered original/common-world Burn phases and explicitly marked details."""
import argparse
import json
from pathlib import Path
from PIL import Image, ImageDraw
from speed_profile_contract import sha

PHASES = (1, 3, 6, 16, 30, 49, 50, 57, 60, 70, 74, 75, 76, 90)
ROI = (256, 120, 384, 340)


def run(native, common, before, output):
    n = json.loads((native / 'manifest.json').read_text())
    c = json.loads((common / 'manifest.json').read_text())
    b = json.loads((before / 'manifest.json').read_text())
    if n.get('error') or c['status'] != 'pass' or b['status'] != 'pass':
        raise ValueError('Require completed original and source recordings')
    scenario = c['scenario']
    if scenario['native_domain'] or scenario['origin'] != [0, 0, 16] or scenario['camera'] != [320, 320]:
        raise ValueError('Require declared common-world Acolyte fixture')
    output.mkdir(parents=True, exist_ok=False)
    frames = {tick: frame for tick, frame in zip(c['sample_simulation_ticks'], c['frames'])}
    old = {tick: frame for tick, frame in zip(b['sample_simulation_ticks'], b['frames'])}
    rows = {row['tick']: row for row in n['rows']}
    whole = Image.new('RGB', (1280, 505 * len(PHASES)))
    draw = ImageDraw.Draw(whole)
    pins = []
    for index, tick in enumerate(PHASES):
        sources = [('Original', Path(rows[tick]['image'])),
                   ('Common before', Path(old[tick]['path'])),
                   ('Common corrected', Path(frames[tick]['path']))]
        details = Image.new('RGB', (768, 465))
        dd = ImageDraw.Draw(details)
        entry = dict(tick=tick, images=[])
        for column, (label, path) in enumerate(sources):
            image = Image.open(path).convert('RGB')
            if image.size != (640, 480):
                raise ValueError('Require complete 640x480 frames')
            entry['images'].append(dict(label=label, path=str(path), sha256=sha(path.read_bytes()), bbox=image.getbbox()))
            details.paste(image.crop(ROI).resize((256, 440), Image.Resampling.NEAREST), (column * 256, 25))
            dd.text((column * 256 + 4, 5), f'{label} tick{tick} 2x nearest', fill='white')
            if column != 1:
                slot = 0 if column == 0 else 1
                whole.paste(image, (slot * 640, index * 505 + 25))
                draw.text((slot * 640 + 4, index * 505 + 5), f'{label} tick{tick}', fill='white')
        details.save(output / f'detail-{tick:03d}.png')
        pins.append(entry)
    whole.save(output / 'whole.png')
    report = dict(status='awaiting_independent_review', accepted=False, phases=pins,
                  roi=ROI, appearance_fitted=False, whole_sha256=sha((output / 'whole.png').read_bytes()),
                  original_manifest_sha256=sha((native / 'manifest.json').read_bytes()),
                  common_manifest_sha256=sha((common / 'manifest.json').read_bytes()),
                  before_manifest_sha256=sha((before / 'manifest.json').read_bytes()),
                  scope='Whole original loaded actor-bound Render/software raster versus real common-world port. Same integer Acolyte14/0/facing96, origin0,0,16, post-actor seed2, camera320,320 and declared lighting. Before/corrected retain original phase/time inputs; raw images unchanged. Fixed crop and nearest enlargement are display-only. Expected common projection and float/edge/RGB565 raster residuals remain; natural moving actor/map/poster/audio/device and full acceptance remain open.')
    (output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('native', 'common', 'before', 'output'):
        parser.add_argument('--' + name, type=Path, required=True)
    args = parser.parse_args()
    result = run(args.native, args.common, args.before, args.output)
    print(result['status'], len(result['phases']))
