#!/usr/bin/env python3
"""Original FireSwarm/Faultfire/Streamer full authored-normal visual samples.

Reuses the executed leaf controller fixtures, then executes the original owner,
RenderObject matrix carrier, Illuminate, projector, and software raster. Scene
lights and null-spell owner are explicit; no production geometry/colors enter.
"""
import argparse
import json
import struct
import zipfile
from pathlib import Path
from fireswarm_probe import FireSwarmFixture
from faultfire_probe import FaultFireFixture
from streamer_probe import StreamerFixture, parse_asset as streamer_asset
from firecone_owner_probe import native_owner, native_world
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from fizzle_probe import sha

ROOT = Path(__file__).resolve().parents[2]
NAMES = {'FireSwarm': ('fireswarm', '0x582c1e78', 2),
         'Faultfire': ('faultfire', '0x51753bce', 2),
         'Streamer': ('streamer', '0x482dfe82', 8)}


def run(executable, output, name):
    output.mkdir(parents=True, exist_ok=False)
    member, type_id, mode = NAMES[name]
    with zipfile.ZipFile(ROOT / 'data/imagery.rvi') as archive:
        asset = archive.read(f'Imagery/Magic/{member}.i3d')
    if name == 'Streamer':
        meshes = streamer_asset(asset)
        fixture = StreamerFixture(executable, meshes)
    else:
        fixture = (FireSwarmFixture if name == 'FireSwarm' else FaultFireFixture)(executable, asset)
        meshes = [fixture.mesh]
        fixture.objects = [fixture.obj]
    fixture.reset()
    software = LitSoftwareFixture(executable, 640, 340)
    software.lighting((38, 38, 38), (1, 1, 1))
    v = software.vm
    for address, value in ((0x5d7a28, 1), (0x66818c, 0), (0x5e91c0, 0),
                           (0x5e8740, 0), (0x5c61ac, 1), (0x5e8790, 0)):
        v.put_u32(address, value)
    v.call(0x417d60, (mode, 1), this=0x65a57c)
    samples = (1, 5, 15, 29, 35, 45, 59, 71, 75, 80, 99, 100, 103)
    cases = []
    fixture.animate()  # Declared FrameSnap one-render warmup.
    for capture in range(1, 104):
        fixture.animate()
        if capture not in samples:
            continue
        owner = native_owner(fixture)
        if name == 'Faultfire':
            fixture.draws()
            packets = [dict(object=0, matrix=list(struct.unpack('<16f', p['matrix'])),
                            uvs=p['uvs']) for p in fixture.packets]
        else:
            fixture.original_draws()
            packets = [dict(object=p['stream'], matrix=p['matrix']) for p in fixture.packets] if name == 'Streamer' else [dict(object=0, matrix=m) for m in fixture.packets]
        for packet in packets:
            packet['matrix'] = native_world(fixture, packet)
        def raster():
            software.clear()
            colors = []
            for packet in packets:
                mesh = meshes[packet['object']]
                dimension = 128 if name == 'Faultfire' else 64
                software.set_texture(dimension, dimension, mesh['texture'],
                                     format='RGB565' if name == 'Streamer' else 'ARGB4444')
                vertices = [list(x) for x in mesh['vertices']]
                if 'uvs' in packet:
                    for vertex, uv in zip(vertices, packet['uvs']):
                        vertex[6:8] = uv
                _, _, transformed = software.draw_authored(vertices, mesh['indices'], packet['matrix'], instruction_limit=50000000)
                colors.extend(tuple(p[3:7]) for p in transformed)
            return v.surface_bytes('screen'), v.surface_bytes('depth'), sorted(set(colors))
        image, depth, colors = raster()
        if raster() != (image, depth, colors):
            raise AssertionError('Original warm image/depth/color replay differs')
        path = output / f'native-capture{capture:03d}.png'
        save_rgb565_png(path, image, 640, 340)
        cases.append(dict(capture=capture, tick=capture+1, owner_matrix=owner,
                          packets=len(packets), path=str(path), sha256=sha(path.read_bytes()),
                          state=fixture.state(), native_rgba5=colors,
                          nonzero_pixels=sum(p != (0,) for p in struct.iter_unpack('<H', image))))
    report = dict(status='reference_complete', accepted=False, name=name, type_id=type_id,
                  executable_sha256=sha(executable.read_bytes()), asset_sha256=sha(asset),
                  probe_sha256=sha(Path(__file__).read_bytes()), cases=cases,
                  configuration=dict(origin=[0,0,0], facing=0, camera=[0,0,0], viewport=[640,340],
                                     ambient=[38]*3, directional=[1]*3, point_lights=[], scene_mode=mode,
                                     cull=3, z_test=True, z_write=False, fps=24, seed=1, warmup=1),
                  scope='Original leaf state/Render packets, executed owner and relative RenderObject carrier, full authored normal/UV/texture, original Illuminate/projector/software raster. Null spell, explicit scene lights. Camera viewport clipping and original oversized-triangle rejection retained. Sound unavailable in Streamer fixture; no audio/caller/device claim.')
    (output/'manifest.json').write_text(json.dumps(report, indent=2)+'\n')
    return report


if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('executable', type=Path)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--effect', choices=NAMES, required=True)
    a = p.parse_args()
    report = run(a.executable, a.output, a.effect)
    print(json.dumps({k:v for k,v in report.items() if k != 'cases'}, indent=2))
