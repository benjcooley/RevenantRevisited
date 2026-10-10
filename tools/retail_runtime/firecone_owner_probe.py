#!/usr/bin/env python3
"""FireCone local translation through executed native owner/lighting/raster.

The native owner matrix is an explicit shared input to compiled SubmitPool.
This isolates the leaf's relative-coordinate contract. The game's common-world
owner producer and GPU projector are deliberately not certified by this probe.
"""
import argparse
import json
from pathlib import Path
import struct
import zipfile
from unicorn.x86_const import UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP
from firecone_render_probe import (ROOT, SELECTED, FireConeRenderFixture, parse_asset,
                                  compile_submit, candidate_packets, verify_state, sha)
from firecone_state_probe import run as state_run
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png


def native_owner(fixture):
    v = fixture.vm
    v.uc.reg_write(UC_X86_REG_ESI, fixture.animator)
    # The actual base animator owner-position/FIX/facing fragment. The earlier
    # tag/audio/base traversal remains outside this isolated owner operation.
    v.call(0x40e470, stop_address=0x40e546)
    v.put_u32(fixture.animator + 0x74, 0)
    v.call(0x40e740, this=fixture.animator, stop_address=0x40e83d)
    return list(struct.unpack('<16f', v.uc.mem_read(fixture.animator + 0xac, 64)))


def native_world(fixture, packet):
    v = fixture.vm
    obj = fixture.objects[packet['object']]
    v.write(obj + 0x58, struct.pack('<16f', *packet['matrix']))
    v.put_u32(0x5d7a28, 1)
    v.uc.reg_write(UC_X86_REG_EDI, obj)
    v.uc.reg_write(UC_X86_REG_EBP, fixture.animator + 0xac)
    # Actual relative RenderObject compose -> Scene SetTransform -> software
    # world storage, without substituting host multiplication or a Z adapter.
    v.call(0x40aad5, stop_address=0x40ab92)
    return list(struct.unpack('<16f', v.uc.mem_read(0x675ed0, 64)))


def pixels(software, packets, parts, asset):
    software.clear()
    colors = []
    for packet in packets:
        part = parts[packet['object']]
        offset = part['texture_offset']
        software.set_texture(64, 64, asset[offset:offset + 8192])
        _, _, vertices = software.draw_authored(part['vertices'], part['indices'], packet['matrix'])
        colors.extend(list(v[3:7]) for v in vertices)
    return software.vm.surface_bytes('screen'), software.vm.surface_bytes('depth'), colors


def run(executable, output, facings=(0, 64, 128, 192), ticks=SELECTED):
    output.mkdir(parents=True, exist_ok=False)
    state_report = state_run(executable, output / 'state', 100)
    if state_report['status'] != 'pass':
        raise AssertionError('Complete independent production simulation prerequisite failed')
    states = json.loads((output / 'state/candidate-states.json').read_text())
    with zipfile.ZipFile(ROOT / 'data/imagery.rvi') as archive:
        asset = archive.read('Imagery/Magic/firecone.i3d')
    parts = parse_asset(asset)
    binary, assets, compiled = compile_submit(output, parts)
    software = LitSoftwareFixture(executable, camera=(10000, 10000, 0))
    # Ambient32/white/Ambient3D130/DirLight85 yields packed ambient38 and
    # directional255/256*(.85*1.5), capped at1. Original Illuminate consumes
    # these explicit scene descriptors; full scene light selection is separate.
    software.lighting((38, 38, 38), (1, 1, 1))
    # FireCone requests source mode8. Execute the actual shared blend setter.
    for address, value in ((0x5d7a28, 1), (0x66818c, 0), (0x5e91c0, 0),
                           (0x5e8740, 0), (0x5c61ac, 1), (0x5e8790, 0)):
        software.vm.put_u32(address, value)
    software.vm.call(0x417d60, (8, 1), this=0x65a57c)
    cases = []
    for face in facings:
        fixture = FireConeRenderFixture(executable, face)
        for tick in range(101):
            if tick and fixture.state()['alive']:
                fixture.step()
            verify_state(fixture.state(), states[tick])
            if tick not in ticks:
                continue
            owner = native_owner(fixture)
            native = fixture.render(parts)
            for packet in native:
                packet['matrix'] = native_world(fixture, packet)
            facing = struct.unpack('<f', fixture.vm.uc.mem_read(fixture.animator + 0x178, 4))[0]
            port = candidate_packets(binary, assets, output, states[tick], facing, parts, owner) if states[tick]['alive'] else []
            key = lambda p: (p['pool'], p['slot'], p['object'])
            original = {key(p): p for p in native}
            delta = max((abs(x-y) for p in port for x,y in zip(p['matrix'], original[key(p)]['matrix'])), default=0)
            native_image, native_depth, native_colors = pixels(software, native, parts, asset)
            port_image, port_depth, port_colors = pixels(software, port, parts, asset)
            repeated_image, repeated_depth, _ = pixels(software, native, parts, asset)
            if (native_image, native_depth) != (repeated_image, repeated_depth):
                raise AssertionError('Original lit raster warm replay changed')
            image_delta = sum(a != b for a,b in zip(struct.iter_unpack('<H', native_image), struct.iter_unpack('<H', port_image)))
            depth_delta = sum(a != b for a,b in zip(struct.iter_unpack('<H', native_depth), struct.iter_unpack('<H', port_depth)))
            basename = f'face-{face:03d}-tick-{tick:03d}'
            for name, image in (('native', native_image), ('production', port_image)):
                save_rgb565_png(output / f'{basename}-{name}.png', image, software.width, software.height)
            cases.append(dict(face=face, tick=tick, owner_matrix=owner, native_packets=len(native),
                production_packets=len(port), paired_matrix_max_error=delta,
                differing_pixels=image_delta, differing_depth=depth_delta,
                nonzero_pixels=sum(value != (0,) for value in struct.iter_unpack('<H', native_image)),
                native_rgba5_values=sorted(set(tuple(c) for c in native_colors)),
                production_rgba5_values=sorted(set(tuple(c) for c in port_colors)),
                native_image_sha256=sha(native_image), production_image_sha256=sha(port_image)))
    visible_cases = sum(c['nonzero_pixels'] > 0 for c in cases)
    report = dict(status='pass' if visible_cases and all(c['paired_matrix_max_error'] < .002 and c['differing_pixels'] == 0 and c['differing_depth'] == 0 for c in cases) else 'differences_found',
        nonempty_pairs=visible_cases,
        cases=cases, compiled_submit=compiled, exact_state_fields=state_report['float_fields'],
        executable_sha256=sha(Path(executable).read_bytes()), asset_sha256=sha(asset),
        original_illumination_executed=True, original_projector_executed=True,
        source_translation_fix=True, full_effect_accepted=False, modern_gpu_compared=False,
        original_functions=['40e470..40e546 owner pose', '40e740..40e83d owner matrix',
            '4eaae0/50c220 leaf/shared Render', '40aad5..40ab92 relative RenderObject world carrier',
            '417430/56d5f0 SetTransform', '56eb30 D3DVERTEX transform/Illuminate/raster'],
        scope='Compiled actual SubmitPool receives independently executed native owner matrices. Exact authored normals, UVs and RGB565 textures; original mode8, CCW culling, normal illumination and software projection/raster. Scene ambient/directional descriptors are explicit inputs. Production common-world owner conversion, complete scene lighting selection, point lights, GPU, live spell/caster/audio and actual map appearance remain separate. No inverse-Z/pixel adapter or matrix fit.',
        probe_sha256=sha(Path(__file__).read_bytes()))
    (output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('executable', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    result = run(args.executable, args.output)
    print(json.dumps({k:v for k,v in result.items() if k not in ('cases', 'compiled_submit')}))
