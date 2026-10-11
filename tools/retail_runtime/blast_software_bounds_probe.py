"""Change only original Blast raster span limits; retain full draw inputs."""
import argparse
import json
import struct
import zipfile
from pathlib import Path

from blast_visual_reference import NativeBlast, ROOT
from software_probe import save_rgb565_png
from speed_profile_contract import sha


def run(executable, output, tick=60):
    output.mkdir(parents=True, exist_ok=False)
    with zipfile.ZipFile(ROOT / 'data/imagery.rvi') as archive:
        data = archive.read(next(n for n in archive.namelist()
                                 if n.lower() == 'imagery/magic/blast.i3d'))
    fixture = NativeBlast(executable, data)
    fixture.initialize_scene()
    fixture.vm.checkpoint()
    controls = []
    for name, limits in [('original', (640, 480)),
                         ('widened_negative_control', (6400, 4800))]:
        fixture.vm.restore()
        fixture.vm.write(0x5b9cc0, struct.pack('<f', limits[0]))
        fixture.vm.write(0x5b9cbc, struct.pack('<f', limits[1]))
        # The widened diagnostic executes substantially more raster work.
        pixels, depth, packet = fixture.render(tick, tick % 120,
                                              instruction_limit=400000000)
        image = output / (name + '.png')
        save_rgb565_png(image, pixels, 640, 480)
        controls.append(dict(name=name, limits=limits, packet=packet,
                             image=str(image.resolve()), image_sha256=sha(image.read_bytes()),
                             depth_sha256=sha(depth),
                             nonzero_pixels=sum(p != (0,) for p in
                                                struct.iter_unpack('<H', pixels))))
    def inputs(control):
        packet = control['packet']
        return dict(owner=packet['owner'], flags=packet['flags'],
                    draws=[{k: v for k, v in draw.items()
                            if k != 'oversized_triangles'} for draw in packet['draws']])
    assert inputs(controls[0]) == inputs(controls[1]), 'Draw inputs changed'
    rejected = sum(d.get('oversized_triangles', 0)
                   for d in controls[0]['packet']['draws'])
    assert rejected > 0
    assert not any(d.get('oversized_triangles', 0)
                   for d in controls[1]['packet']['draws'])
    assert controls[0]['image_sha256'] != controls[1]['image_sha256']
    report = dict(status='pass', tick=tick, rejected_triangles=rejected,
                  controls=controls, executable_sha256=sha(executable.read_bytes()),
                  asset_sha256=sha(data), probe_sha256=sha(Path(__file__).read_bytes()),
                  scope='Only literal original software edge-span limits change. '
                        'Owner, flags, full draw packets, vertices, transformed colors, '
                        'UVs and matrices are identical. No fitted geometry or lighting.')
    (output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    print('PASS:', rejected, 'original rejected triangles;',
          [c['nonzero_pixels'] for c in controls], 'visible pixels')
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('executable', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--tick', type=int, default=60)
    args = parser.parse_args()
    run(args.executable, args.output, args.tick)
