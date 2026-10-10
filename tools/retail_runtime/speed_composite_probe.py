#!/usr/bin/env python3
"""Original normal-lit Speed/Quicksilver base plus independent particle packets.

Coherent stationary-owner samples before the movement fixture at tick60. This
tests the complete isolated image, while common-world map/Metal remains open.
"""
import argparse
import json
from pathlib import Path
import struct
import zipfile
import speed_packet_probe
import quicksilver_probe
from buff_base_lighting_probe import punctuation
from firecone_state_probe import ROOT, sha
from speed_controller_preflight import NativeSpeed, read_asset
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png


def run(executable, output):
    output.mkdir(parents=True, exist_ok=False)
    cases = []
    for name, producer in (('speed', speed_packet_probe.run), ('quicksilver', quicksilver_probe.run)):
        destination = output / name
        packet_report = producer(executable, ROOT / 'data/imagery.rvi', destination, retain_draws=True)
        if packet_report['status'] != 'pass':
            raise AssertionError('Independent particle frontend prerequisite failed')
        with zipfile.ZipFile(ROOT / 'data/imagery.rvi') as archive:
            members = {n.lower():n for n in archive.namelist()}
            data = archive.read(members['imagery/magic/' + name + '.i3d'])
        asset = read_asset(data)
        asset['name'] = name
        native = NativeSpeed(executable, asset)
        punctuation(native)
        native.initialize(0)
        software = LitSoftwareFixture(executable)
        software.lighting((38, 38, 38), (1, 1, 1))
        v = software.vm
        for address, value in ((0x5d7a28, 1), (0x66818c, 0), (0x5e91c0, 0), (0x5e8740, 0), (0x5c61ac, 1), (0x5e8790, 0)):
            v.put_u32(address, value)
        u = lambda p: struct.unpack_from('<I', data, p)[0]
        r = lambda p: p + u(p)
        textures = []
        for index in range(2):
            descriptor = r(asset['body_offset'] + 40) + index * 120
            height, width = struct.unpack_from('<2I', data, descriptor + 8)
            bits = r(r(descriptor + 108))
            textures.append((width, height, data[bits:bits + width*height*2]))
        base = asset['objects'][1]
        vertices = list(struct.iter_unpack('<8f', bytes.fromhex(base['vertex_hex'])))
        indices = list(struct.unpack_from('<6H', bytes.fromhex(asset['face_hex']), base['texture_runs'][2] * 6))
        for tick, draws in packet_report['selected_frontend_draws'].items():
            if native.vm.call(0x40a420, (native.animobjs[1], 0, tick, 0, 0), this=native.imagery) != 1:
                raise AssertionError('Native base matrix rejected')
            native_matrix = list(struct.unpack('<16f', native.vm.uc.mem_read(native.animobjs[1] + 0x58, 64)))
            images, depths, base_colors, particle_images = [], [], [], []
            for label in ('native', 'port', 'native'):
                software.clear()
                software.set_texture(*textures[1])
                v.call(0x417d60, (16, 1), this=0x65a57c)
                for draw in draws:
                    projected = software.project(draw[label])
                    uv = draw[label + '_uv']
                    software.draw([(*p, *draw['color'], draw['alpha'], *t) for p,t in zip(projected, uv)],
                        [2, 3, 0, 1, 2, 0] if label == 'native' else [2, 0, 3, 1, 3, 0],
                        z_enabled=True, z_write=False, instruction_limit=5000000)
                particle_images.append(v.surface_bytes('screen'))
                # Original default Render draws controllers before the ordinary
                # authored base; Speed's per-object mode80 supersedes mode16.
                software.set_texture(*textures[0])
                v.call(0x417d60, (80, 1), this=0x65a57c)
                matrix = native_matrix if label == 'native' else packet_report['independent_base_matrices'][tick]
                image, depth, transformed = software.draw_authored(vertices, indices, matrix, z_enabled=False)
                images.append(image); depths.append(depth); base_colors.append([list(p[3:7]) for p in transformed])
            if (images[0], depths[0]) != (images[2], depths[2]):
                raise AssertionError('Complete original composite warm replay differs')
            for label, image in zip(('native', 'production'), images):
                save_rgb565_png(destination / f'composite-{label}-{tick:02d}.png', image, 512, 512)
            cases.append(dict(name=name, tick=tick, particles=len(draws),
                differing_pixels=sum(a!=b for a,b in zip(struct.iter_unpack('<H', images[0]), struct.iter_unpack('<H', images[1]))),
                nonzero_pixels=sum(p!=(0,) for p in struct.iter_unpack('<H', images[0])),
                pixels_changed_by_base=sum(a!=b for a,b in zip(struct.iter_unpack('<H', particle_images[0]), struct.iter_unpack('<H', images[0]))),
                depth_equal=depths[0] == depths[1], native_warm_replay_equal=True,
                native_base_colors=base_colors[0], production_base_colors=base_colors[1],
                image_sha256=[sha(image) for image in images[:2]]))
    report = dict(status='pass' if all(c['differing_pixels'] == 0 and c['depth_equal'] and c['nonzero_pixels'] > 0 and c['pixels_changed_by_base'] > 0 for c in cases) else 'differences_found',
        cases=cases, full_effect_accepted=False, actual_map_compared=False, metal_compared=False,
        scope='Complete isolated Speed/Quicksilver composite at stationary origin/face0, ticks5/15/29. Independently compiled production keys/emitter/parser/simulation/SubmitPartSys/base matrix; original sampled particle colors and normal-lit authored base. Source controller-before-base order and mode16→80, original projection/raster, equal warm replays. Existing raw-world-to-native-MODELZ particle contract remains explicit. No common-world owner/Metal, natural caller/caster, moving owner composite, fractional pose or full scene light selection acceptance.',
        probe_sha256=sha(Path(__file__).read_bytes()))
    (output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('executable', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    result = run(args.executable, args.output)
    print(json.dumps(result, indent=2))
