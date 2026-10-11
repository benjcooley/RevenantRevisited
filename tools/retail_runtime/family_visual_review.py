#!/usr/bin/env python3
"""Retain actual Metal map/native software inspection pairs for small families.

Produces evidence for manual visual review, never automatic appearance credit.
Original software draws onto a quantized clean modern floor for inspection;
that shared backdrop is not an original retail scene or device parity claim.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import zipfile
from PIL import Image, ImageChops, ImageDraw
import barrier_static_probe as barrier
import speaker_static_probe as speaker
import warp_probe as warp
from default_static_probe import parse_parts
from map_texture_probe import run as capture_map
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png

ROOT = Path(__file__).resolve().parents[2]
PROFILES = [*warp.PROFILES[1:], speaker.PROFILE, *barrier.PROFILES]


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def record(path):
    return dict(path=str(Path(path).resolve()), sha256=sha(path))


def metadata(profile, archive):
    members = {n.lower(): n for n in archive.namelist()}
    asset = archive.read(members[('Imagery/' + profile['asset']).lower()])
    if profile['name'].startswith('TeleportDoorInside'):
        return parse_parts(asset, profile)[0]
    if profile['name'] == 'speaker':
        return speaker.parse_asset(asset)
    return barrier.parse_asset(asset, profile)


def retain_reference(executable, output, profile, meta, port):
    frames = [Path(p['path']) for p in port['frames']]
    backdrop = Image.open(frames[0]).convert('RGB')
    native_background = backdrop.crop((64, -86, 576, 426))
    raw = b''.join(struct.pack('<H', ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3))
                   for r, g, b in native_background.getdata())
    is_warp = profile['name'].startswith('TeleportDoorInside')
    if is_warp:
        fixture = warp.WarpFixture(executable)
        states = fixture.trace(meta, profile=profile)
        fixture.setup(meta)
    elif profile['name'] == 'speaker':
        fixture = speaker.SpeakerFixture(executable, 512, 512, 5000000)
        fixture.setup(meta)
        states = [dict(tick=i, uvs=[v[6:8] for v in meta['vertices']]) for i in range(4)]
    else:
        fixture = barrier.BarrierFixture(executable, 512, 512, 5000000)
        fixture.setup(meta)
        states = [dict(tick=i, uvs=[v[6:8] for v in meta['vertices']]) for i in range(4)]
    matrix, points = fixture.original(0, 0)
    rgba = [31] * 4 if is_warp else None
    if not is_warp:
        lit = LitSoftwareFixture(executable, 512, 512)
        # Actual map log: ambient32/white, INI Ambient3D130/DirLight85
        # yields38/255 base and1 directional via RetailSoftwareMeshLighting.
        lit.lighting((38, 38, 38), (1, 1, 1))
        lit.vm.call(0x56d3a0, (0, 0))
        lit.clear()
        lit.checkpoint()
    folder = output / 'native'
    folder.mkdir()
    save_rgb565_png(folder / 'quantized-background.png', raw, 512, 512)
    quantized_background = Image.open(folder / 'quantized-background.png').convert('RGB')
    sheet = Image.new('RGB', (800, 472), (35, 35, 35))
    labels = ImageDraw.Draw(sheet)
    cases = []
    for tick in range(4):
        if is_warp:
            fixture.software.restore()
            fixture.software.clear()
            fixture.vm.write(fixture.software.screen.address, raw)
            for state, value in fixture.blend_mapping['states']:
                fixture.vm.call(0x56d400, (state, value))
            projected = fixture.software.project(points, camera=(0, 0, 0), zdist=1925)
            color, depth = fixture.software.draw([(*p, *rgba, *uv) for p, uv in zip(projected, states[tick]['uvs'])],
                                                meta['indices'], z_enabled=True, z_write=False,
                                                instruction_limit=5000000)
            transformed = None
        else:
            lit.restore()
            lit.clear()
            lit.vm.write(lit.screen.address, raw)
            color, depth, transformed = lit.draw_authored(meta['vertices'], meta['indices'], matrix,
                                                          cull=1, z_enabled=True, z_write=True)
            projected = None
        path = folder / f'retail-floor-{tick:02d}.png'
        save_rgb565_png(path, color, 512, 512)
        original = Image.open(path).convert('RGB')
        modern = Image.open(frames[tick + 2]).convert('RGB')
        sheet.paste(original.crop((156, 96, 356, 306)), (tick * 200, 26))
        sheet.paste(modern.crop((220, 10, 420, 220)), (tick * 200, 262))
        labels.text((tick * 200 + 4, 6), f'Original SW {tick}', fill='white')
        labels.text((tick * 200 + 4, 242), f'Current Metal frame {tick + 3}', fill='white')
        source_bounds = ImageChops.difference(original, quantized_background).getbbox()
        modern_bounds = ImageChops.difference(modern.crop((0, 20, 640, 340)),
                                              backdrop.crop((0, 20, 640, 340))).getbbox()
        if modern_bounds:
            modern_bounds = [modern_bounds[0], modern_bounds[1] + 20, modern_bounds[2], modern_bounds[3] + 20]
        cases.append(dict(tick=tick, modern_frame=tick + 3,
                          offset=states[tick].get('offset'), native_image=record(path),
                          modern_image=record(frames[tick + 2]),
                          native_effect_bounds=source_bounds, modern_effect_bounds=modern_bounds,
                          projected_original_corners=projected, native_lit_vertices=transformed,
                          original_depth_sha256=hashlib.sha256(depth).hexdigest()))
    sheet_path = output / 'same-floor-native-modern-sheet.png'
    sheet.save(sheet_path)
    report = dict(status='awaiting_manual_visual_review', profile=profile,
                  source_sha256=sha(__file__), modern_manifest=record(output / 'modern/manifest.json'),
                  modern_binary_sha256=port['binary_sha256'], native_executable=record(executable),
                  case_count=4, cases=cases, sheet=record(sheet_path),
                  source_local_matrix=matrix, source_positions=points,
                  native_material_diffuse=None if is_warp else meta['diffuse'],
                  native_material_diffuse_supplied=False,
                  native_rgba_5bit=rgba, source_geometry=dict(vertices=len(meta['vertices']), indices=meta['indices']),
                  native_input_policy='Pinned authored asset, original key/matrix/point transform and software projection/raster. '
                      + ('Original Warp Init/Animate/Render and alpha texture raster, white31 vertex input.' if is_warp else
                         'Whole original56eb30 D3DVERTEX normal transform/Illuminate/raster. Authored normals, ambientRGB38/38/38 and directionalRGB1/1/1 descriptors supplied; no host diffuse or vertex-color input.'),
                  modern_input_policy='Actual map-owned effect and current Metal renderer, camera110/10000/10000/16, ambient32white,24Hz,seed1. '
                      + ('Warp mode2 bypasses normal/deferred-light modulation.' if is_warp else
                         'Exact3-ID mode1/white-tint normal-prelit path only when current source-software mesh policy is enabled; modern/positional-light material path preserved.'),
                  lit_fixture=None if is_warp else record(ROOT/'tools/retail_runtime/lit_software_probe.py'),
                  presentation=dict(native_crop=[156,96,356,306], modern_crop=[220,10,420,220],
                                    backdrop_translation=[-64,86], geometry_rescaling=False, fitted_shift=False),
                  backdrop_policy='Clean modern frame1 quantized toRGB565 is the original software destination; not an original retail map scene.',
                  full_acceptance=False, exact_device_parity=False)
    (output / 'pair-manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    return report


def run(binary, data, executable, output, selected=None):
    output.mkdir(parents=True, exist_ok=False)
    reports = []
    with zipfile.ZipFile(ROOT / 'data/imagery.rvi') as archive:
        for profile in PROFILES:
            if selected and profile['name'] not in selected:
                continue
            folder = output / profile['name']
            folder.mkdir()
            is_warp = profile['name'].startswith('TeleportDoorInside')
            map_profile = dict(name=profile['name'], id=profile['id'],
                               owner_frames=100 if profile['name'] == 'speaker' else 1,
                               render_frames=4 if is_warp else 1)
            capture = capture_map(binary, data, 'FontRuntimeLab', (110,10000,10000,16),
                                  folder / 'modern', [map_profile])
            if capture['status'] != 'pass':
                reports.append(dict(name=profile['name'], status='bounded_capture_failure',
                                    manifest=record(folder / 'modern/manifest.json')))
                continue
            reports.append(retain_reference(executable, folder, profile, metadata(profile, archive), capture))
            print(profile['name'], 'pairs retained', flush=True)
    report = dict(status='awaiting_manual_visual_review', cases=reports, actual_gpu_captures=True,
                  binary=record(binary), fixture_module=record(data/'Modules/FontRuntimeLab.rvm'),
                  probe=record(__file__), visual_credit_automatic=False)
    (output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--binary', type=Path, required=True)
    parser.add_argument('--data-root', type=Path, required=True)
    parser.add_argument('--executable', type=Path, default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe')
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--names', nargs='+')
    args = parser.parse_args()
    run(args.binary.resolve(), args.data_root.resolve(), args.executable.resolve(), args.output.resolve(), args.names)
