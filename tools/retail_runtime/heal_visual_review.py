#!/usr/bin/env python3
"""Freeze tick-aligned original/common/native-domain Heal lifecycle images."""
import argparse
import hashlib
import json
from pathlib import Path
import re
from PIL import Image, ImageDraw


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def review(native, common, domain, output):
    output.mkdir(parents=True, exist_ok=False)
    reference = json.loads((native / 'manifest.json').read_text())
    captures = [json.loads((p / 'manifest.json').read_text()) for p in (common, domain)]
    assert reference['status'] == 'reference_complete' and reference['warm_replays'] == 2
    checkpoints, light_logs = [], []
    for index, (path, capture) in enumerate(zip((common, domain), captures)):
        assert capture['status'] == 'pass'
        expected = dict(frames=90, fps=24, warmup=1, seed=1, origin=[0, 0, 0], camera=[320, 240],
            rectangle=[0, 0, 640, 480], resolution=[640, 480], background='black',
            ambient=[32, 255, 255, 255], native_domain=bool(index), effect='THealEffect_BESPOKE_SINGLE_BURST')
        assert all(capture['scenario'].get(k) == v for k, v in expected.items())
        log = (path / 'run.log').read_text()
        assert f'[native-domain] enabled={index} camera_z=0 owner=RzRxRyT Heal level=1' in log
        lights = [line for line in log.splitlines() if '[vfx-source-light]' in line]
        assert len(lights) == 1 and 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0' in lights[0]
        light_logs.append(lights[0])
        counts = {int(t): int(c) for t, c in re.findall(r'before_render=(\d+) global_random_draws=(\d+)', log)}
        assert set(counts) == {1, 2, 6, 16, 30, 46, 60}
        checks = []
        for tick, count in counts.items():
            assert reference['cases'][tick - 1]['state']['rng_calls'] == count
            checks.append(dict(before_render=tick, original_prior_tick=tick - 1, rng_calls=count))
        checkpoints.append(checks)
    selected = (2, 6, 16, 30, 40, 50, 60, 70, 78, 91)
    panel = Image.new('RGB', (900, 250 * len(selected)))
    draw = ImageDraw.Draw(panel)
    cases = []
    for tick in range(2, 92):
        c = reference['cases'][tick]
        paths = [Path(c['image'])] + [Path(m['frames'][tick - 2]['path']) for m in captures]
        hashes = [c['sha256']] + [m['frames'][tick - 2]['sha256'] for m in captures]
        assert [sha(p) for p in paths] == hashes
        images = [Image.open(p).convert('RGB') for p in paths]
        counts = [sum(any(rgb) for rgb in im.getdata()) for im in images]
        cases.append(dict(capture=tick - 1, tick=tick, alive=c['state']['alive'], draws=len(c['packets']),
            rng_calls=c['state']['rng_calls'], images=list(map(str, paths)), sha256=hashes,
            nonzero_pixels=counts))
        if tick in selected:
            row = selected.index(tick)
            for col, (im, label) in enumerate(zip(images, ('Original', 'Common-world Metal', 'Native-domain Metal'))):
                panel.paste(im.resize((300, 225), Image.Resampling.NEAREST), (col * 300, row * 250 + 25))
                draw.text((col * 300 + 2, row * 250 + 4), f'{label} tick {tick}', fill='white')
    panel.save(output / 'contact.png')
    detail = Image.new('RGB', (960, 660)); draw = ImageDraw.Draw(detail)
    for col, path in enumerate(cases[28]['images']):
        im = Image.open(path).convert('RGB').crop((240, 0, 400, 315)).resize((320, 630), Image.Resampling.NEAREST)
        detail.paste(im, (col * 320, 30))
        draw.text((col * 320 + 3, 5), ('Original', 'Common-world', 'Native-domain')[col] + ' tick30 (2x)', fill='white')
    detail.save(output / 'detail.png')
    report = dict(status='review_ready', accepted=False, retail_name='heal', retail_type_id='0xad92bc1d',
        cases=cases, selected_ticks=selected, live_rng_checkpoints=checkpoints, source_light_logs=light_logs,
        source_color_policy='Exact ARGB4444 texels; native vertex color ignored',
        binary_sha256=[m['binary_sha256'] for m in captures],
        input_manifest_sha256=[sha(p / 'manifest.json') for p in (native, common, domain)],
        contact_sha256=sha(output / 'contact.png'), detail_sha256=sha(output / 'detail.png'), appearance_fitted=False,
        scope='Level1, fixed explicit caster/owner origin, facing0. Ninety actual whole-effect captures per route after one warmup tick; seven live RNG checks per route. Native Init/Animate/Render, glow TRS, relative owner composition and FVF_VERTEX/ARGB4444 raster versus current Metal. Native-domain isolated cameraZ=0. Cull2, depth test on and writes off are declared renderer inputs. Full RenderObject resource dispatch, natural spell attachment/HP/audio, levels2-4, common projection and exact framebuffer parity remain open.')
    (output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    print('PASS', len(cases), 'whole-effect phase triples')


if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    for name in ('native', 'common', 'domain', 'output'):
        p.add_argument('--' + name, type=Path, required=True)
    a = p.parse_args()
    review(a.native, a.common, a.domain, a.output)
