"""Package unclipped diagonal ArrowEffect frames without image fitting.

This checks lifecycle, frame pairing and viewport coverage. Pixel metrics are
reported, never converted into an appearance approval. The detail panel uses
one declared rectangle for every image, without scaling or color changes.
"""
import argparse
import hashlib
import json
import re
from pathlib import Path
from PIL import Image, ImageChops, ImageDraw


def pin(path):
    return dict(path=str(path), sha256=hashlib.sha256(path.read_bytes()).hexdigest())


def review(native, metal, output):
    trace = json.loads((native / 'trace.json').read_text())
    assert trace['effect'] == 'ArrowEffect'
    assert trace['viewport'] == [1280, 960] and trace['center'] == [640, 640]
    assert trace['source'] == [0, 0, 144] and trace['target'] == [-170, -219, 144]
    assert trace['facing'] == 229 and trace['render_hz'] == 24 and trace['render_stride'] == 1
    assert len(trace['records']) == 24
    manifests = {d: json.loads((metal / ('metal-' + d) / 'manifest.json').read_text())
                 for d in ('native', 'common')}
    for domain, manifest in manifests.items():
        s = manifest['scenario']
        assert manifest['status'] == 'pass' and manifest['dosbox_used'] is False
        assert s['resolution'] == trace['viewport'] and s['camera'] == trace['center']
        assert s['origin'] == trace['source'] and s['target'] == trace['target']
        assert s['frames'] == 24 and s['fps'] == 24 and s['warmup'] == 0 and s['seed'] == 1
        assert s['native_domain'] == (domain == 'native') and len(manifest['frames']) == 24
        assert manifest['sample_simulation_ticks'] == list(range(1, 25))
        log = metal / ('metal-' + domain) / 'run.log'
        assert pin(log)['sha256'] == manifest['log_sha256']
        phases = [tuple(map(int, row)) for row in re.findall(r'\[arrow-phase\] tick=(\d+) grow=(\d+) countdown=(-?\d+) random=(\d+)', log.read_text())]
        expected = [(r['tick']+1, r['state']['grow'], -1, r['state']['rng_draws']) for r in trace['records'][:14]]
        assert phases == expected, (domain, 'growth/phase/RNG mismatch')
        for frame in manifest['frames']:
            assert pin(Path(frame['path']))['sha256'] == frame['sha256']
    assert manifests['native']['binary_sha256'] == manifests['common']['binary_sha256']
    output.mkdir(parents=True, exist_ok=False)
    images = {}; rows = []
    for tick, record in enumerate(trace['records']):
        paths = [native / f'native-{tick:03}.png'] + [
            Path(manifests[d]['frames'][tick]['path']) for d in ('native', 'common')]
        frames = [Image.open(p).convert('RGB') for p in paths]
        assert all(im.size == tuple(trace['viewport']) for im in frames)
        bounds = [im.getbbox() for im in frames]
        alive = not bool(record['state']['flags'] & 4096)
        for box in bounds:
            assert (box is not None) == alive, (tick, bounds, alive)
            if box:
                assert 0 < box[0] < box[2] < 1280 and 0 < box[1] < box[3] < 960
        assert record['state']['deleted'] == (tick >= 14)
        mask_a = frames[0].convert('L').point(lambda p: 255 if p else 0)
        mask_b = frames[1].convert('L').point(lambda p: 255 if p else 0)
        intersection = ImageChops.multiply(mask_a, mask_b).histogram()[255]
        union = ImageChops.lighter(mask_a, mask_b).histogram()[255]
        diff = ImageChops.difference(frames[0], frames[1]); hist = diff.histogram()
        rows.append(dict(tick=tick, simulation_tick=tick+1, alive=alive,
                         images=[pin(p) for p in paths], bounds=bounds,
                         native_port_support_iou=intersection/union if union else 1,
                         different_rgb_channels=sum(sum(hist[c*256+1:(c+1)*256]) for c in range(3))))
        images[tick] = frames
    # Supplemental detail only. Every source full frame remains independently pinned above.
    roi = (600, 240, 760, 480); selected = [0, 3, 6, 10, 13, 14]
    panel = Image.new('RGB', (480, 260*len(selected)), (24, 24, 24)); draw = ImageDraw.Draw(panel)
    for row, tick in enumerate(selected):
        for column, label in enumerate(('Retail software', 'Port native domain', 'Port common domain')):
            draw.text((column*160+3, row*260+3), f'{label} / tick {tick}', fill='white')
            panel.paste(images[tick][column].crop(roi), (column*160, row*260+20))
    panel.save(output / 'detail.png')
    report = dict(status='ready_for_independent_review', effect='ArrowEffect', type_id='0xad92bd1f',
                  viewport=trace['viewport'], center=trace['center'], full_cycle_unclipped=True, port_phase_growth_rng_match=True,
                  active_ticks=list(range(14)), deletion_tick=14, frames=rows,
                  trace=pin(native/'trace.json'), manifests=[pin(metal/('metal-'+d)/'manifest.json') for d in ('native','common')],
                  panel=pin(output/'detail.png'), detail_rectangle=list(roi), detail_has_scaling=False,
                  acceptance=False,
                  scope=trace['scope'],
                  limits=['Declared source/collision/deletion fixture, not natural bow or map dispatch.',
                          'Common projection is diagnostic; native projection compares original software domain.',
                          'No pixel equality, shared hardware/software raster, lighting, natural map, audio, or scrolling reviewer acceptance claim.'])
    (output/'manifest.json').write_text(json.dumps(report, indent=2)+'\n')
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--native', type=Path, required=True)
    parser.add_argument('--metal', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    result = review(args.native, args.metal, args.output)
    print(json.dumps({k: v for k, v in result.items() if k != 'frames'}, indent=2))
