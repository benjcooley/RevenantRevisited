#!/usr/bin/env python3
"""Freeze unfitted original/common/native-domain mesh leaf appearance panels."""
import argparse
import hashlib
import json
import re
from pathlib import Path
from PIL import Image, ImageChops, ImageDraw


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def run(artifacts, output, name):
    output.mkdir(parents=True, exist_ok=False)
    native_path = artifacts/f'lit-{name}/manifest.json'
    native = json.loads(native_path.read_text())
    modes = ('baseline', 'fixed-common', 'fixed-native')
    metas = {mode: json.loads((artifacts/f'{mode}-{name}/manifest.json').read_text()) for mode in modes}
    lights = {}; rng = {}
    for mode, meta in metas.items():
        assert meta['status'] == 'pass' and len(meta['frames']) == 103
        log = (artifacts/f'{mode}-{name}/run.log').read_text()
        lights[mode] = [line for line in log.splitlines() if '[vfx-source-light]' in line]
        assert any('ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0' in line for line in lights[mode])
        rng[mode] = {int(frame)-1: int(count) for frame, count in re.findall(r'before_render=(\d+) global_random_draws=(\d+)', log)}
        for frame in meta['frames']:
            assert sha(frame['path']) == frame['sha256']
    assert metas['fixed-common']['binary_sha256'] == metas['fixed-native']['binary_sha256']
    cases = []
    for case in native['cases']:
        capture = case['capture']
        assert sha(case['path']) == case['sha256']
        paths = [case['path']] + [metas[mode]['frames'][capture-1]['path'] for mode in modes]
        images = [Image.open(path).convert('RGB') for path in paths]
        expected_rng = capture if name == 'Faultfire' else 0
        for mode in modes:
            if capture in rng[mode]:
                assert rng[mode][capture] == expected_rng, (name, mode, capture, rng[mode])
        cases.append(dict(capture=capture, tick=case['tick'], paths=paths,
                          sha256=[sha(path) for path in paths], bbox=[im.getbbox() for im in images],
                          absolute_rgb_error=[sum((i % 256)*v for i,v in enumerate(ImageChops.difference(images[0], im).histogram())) for im in images[1:]],
                          expected_random_draws_before_capture=expected_rng))
    roi, scale = ((0,0,640,340),1) if name == 'FireSwarm' else (((275,65,345,195),3) if name == 'Faultfire' else ((220,40,365,185),3))
    selected = (1,29,35,59,71,75) if name == 'FireSwarm' else (1,15,35,59,71,99)
    w = (roi[2]-roi[0])*scale; h = (roi[3]-roi[1])*scale+24
    panel = Image.new('RGB',(w*3,h*len(selected)),(20,20,20)); draw=ImageDraw.Draw(panel)
    for row, capture in enumerate(selected):
        case = next(c for c in cases if c['capture'] == capture)
        for col, (index,label) in enumerate(((0,'original software'),(2,'current common'),(3,'current native-domain'))):
            draw.text((col*w+4,row*h+5),f'{name} frame{capture} {label}',fill='white')
            im=Image.open(case['paths'][index]).convert('RGB').crop(roi).resize((w,h-24),Image.Resampling.NEAREST)
            panel.paste(im,(col*w,row*h+24))
    contact=output/'contact.png';panel.save(contact)
    report=dict(status='review_ready',accepted=False,strict_pixel_parity=False,name=name,type_id=native['type_id'],
                native_manifest=str(native_path),native_manifest_sha256=sha(native_path),
                metal_manifests={mode:dict(path=str(artifacts/f'{mode}-{name}/manifest.json'),sha256=sha(artifacts/f'{mode}-{name}/manifest.json'),binary_sha256=meta['binary_sha256']) for mode,meta in metas.items()},
                native_configuration=native['configuration'],light_logs=lights,rng_logs=rng,cases=cases,
                contact=str(contact),contact_sha256=sha(contact),roi=roi,nearest_display_scale=scale,fit=False,
                timing='One-based retained capture i follows tick i+1 after one declared warmup render; no temporal alignment search.',
                scope='Full original authored-normal source versus actual GPU. Common map projector unchanged; native-domain isolated preview is explicit. Source and destination precision/raster differences remain. FireSwarm early oversized-triangle software rejection is retained in the original and is not emulated by Metal; early native appearance is empty, so it cannot certify the hardware-visible early cylinder.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    return report


if __name__ == '__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--artifacts',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--effect',choices=['FireSwarm','Faultfire','Streamer'],required=True)
    a=p.parse_args();run(a.artifacts,a.output,a.effect)
