#!/usr/bin/env python3
"""Blast full geometry coverage in six fixed supported viewports, without scale changes."""
import argparse
import json
from pathlib import Path
import struct
import shutil
import zipfile
from PIL import Image, ImageChops, ImageDraw
from blast_visual_reference import NativeBlast, ROOT
from port_capture import capture
from software_probe import save_rgb565_png
from speed_profile_contract import sha

CENTERS = {f'{row}_{column}': (x,y)
           for row,y in (('upper',1300),('middle',540),('lower',-220))
           for column,x in (('left',1280),('right',640))}
TICKS = (60, 90)


def invariant_draws(record):
    return dict(owner=record['owner'], flags=record['flags'], draws=[
        {key: value for key, value in draw.items()
         if key not in ('lit_vertices_sha256', 'projected_bounds')}
        for draw in record['draws']])


def run(executable, binary, scenario_file, output):
    output.mkdir(parents=True, exist_ok=False)
    with zipfile.ZipFile(ROOT / 'data/imagery.rvi') as archive:
        asset = archive.read(next(n for n in archive.namelist()
                                  if n.lower() == 'imagery/magic/blast.i3d'))
    scenario = json.loads(scenario_file.read_text())['scenario']
    private_save = output / 'save'
    shutil.copytree(scenario['save_root'], private_save)
    scenario.update(binary=str(binary.resolve()), assets_root=str(ROOT / 'assets'), save_root=str(private_save.resolve()))
    scene = NativeBlast(executable, asset, 1920, 1080)
    initialization = scene.initialize_scene()
    assert len(scene.animobjs) == 56 and set(initialization['blend_modes']) == {80}
    scene.vm.checkpoint()
    tiles, invariant, relative_bounds = [], {}, {}
    for name, center in CENTERS.items():
        folder = output / name
        folder.mkdir()
        native = []
        for tick in TICKS:
            scene.vm.restore()
            scene.vm.write(0x675fc0, struct.pack('<2i', *center))
            pixels, depth, record = scene.render(tick, tick, instruction_limit=400000000)
            repeated = scene.render(tick, tick, instruction_limit=400000000)
            assert repeated == (pixels, depth, record), 'Whole original warm replay changed'
            assert struct.unpack('<f', scene.vm.uc.mem_read(0x5b9cc0, 4))[0] == 640
            assert struct.unpack('<f', scene.vm.uc.mem_read(0x5b9cbc, 4))[0] == 480
            semantic = invariant_draws(record)
            if tick in invariant:
                assert semantic == invariant[tick], 'Viewport changed native draw inputs or admission'
            else:
                invariant[tick] = semantic
            # Audit all56 authored root matrix decisions, including inactive scales.
            decisions = [bool(scene.vm.call(0x40a420, (obj, 0, tick, 0, 0), this=scene.imagery))
                         for obj in scene.animobjs]
            assert {i for i, active in enumerate(decisions) if active} == {d['object'] for d in record['draws']}
            boxes = [d['projected_bounds'] for d in record['draws']]
            bounds = [min(b[0] for b in boxes), min(b[1] for b in boxes),
                      max(b[2] for b in boxes), max(b[3] for b in boxes)]
            relative = [bounds[0] - center[0], bounds[1] - center[1],
                        bounds[2] - center[0], bounds[3] - center[1]]
            if tick in relative_bounds:
                assert max(abs(a-b) for a,b in zip(relative, relative_bounds[tick])) < .001
            else:
                relative_bounds[tick] = relative
            # Six overlapping windows cover this entire rectangle, including
            # geometry rejected by the original constant primitive-span guard.
            assert -1280 <= relative[0] <= relative[2] < 1280 and -1300 <= relative[1] <= relative[3] < 1300, relative
            path = folder / f'native-{tick:03d}.png'
            save_rgb565_png(path, pixels, 1920, 1080)
            native.append(dict(tick=tick, image=str(path.resolve()), sha256=sha(path.read_bytes()),
                               depth_sha256=sha(depth), record=record, root_matrix_active=decisions,
                               projected_geometry_bounds=bounds, relative_geometry_bounds=relative,
                               visible_bounds=Image.open(path).getbbox()))
            print(name, tick, bounds, 'covered', flush=True)
        config = dict(scenario, frames=2, capture_stride=30, warmup=59,
                      resolution=[1920, 1080], rectangle=[0, 0, 1920, 1080],
                      camera=list(center), timeout_seconds=180)
        metal = capture(config, folder / 'metal')
        assert metal['status'] == 'pass' and len(metal['frames']) == 2, metal
        assert metal.get('actual_camera') == list(center) and metal.get('actual_viewport') == [1920,1080], metal
        assert all(Image.open(f['path']).size == (1920, 1080) for f in metal['frames'])
        tiles.append(dict(name=name, center=center, native=native,
                          metal_manifest=str((folder / 'metal/manifest.json').resolve()),
                          metal_manifest_sha256=sha((folder / 'metal/manifest.json').read_bytes()),
                          binary_sha256=metal['binary_sha256']))
    cases, panels = [], []
    for index, tick in enumerate(TICKS):
        canvases = [Image.new('RGB', (2560, 2600)) for _ in range(2)]
        overlaps = []
        placed = []
        for tile in tiles:
            metal = json.loads(Path(tile['metal_manifest']).read_text())
            paths = [tile['native'][index]['image'], metal['frames'][index]['path']]
            images = [Image.open(p).convert('RGB') for p in paths]
            x,y = 1280 - tile['center'][0], 1300 - tile['center'][1]
            for prior_name,px,py,prior_images in placed:
                left,top,right,bottom=max(x,px),max(y,py),min(x+1920,px+1920),min(y+1080,py+1080)
                if left >= right or top >= bottom:
                    continue
                differences=[]
                for a,b in zip(prior_images,images):
                    delta=ImageChops.difference(a.crop((left-px,top-py,right-px,bottom-py)),b.crop((left-x,top-y,right-x,bottom-y)))
                    r,g,bl=delta.split();nonzero=ImageChops.lighter(ImageChops.lighter(r,g),bl)
                    differences.append((right-left)*(bottom-top)-nonzero.histogram()[0])
                overlaps.append(dict(tiles=[prior_name,tile['name']],differing_pixels=differences))
            for canvas, image in zip(canvases, images):
                canvas.paste(image, (x,y))
            placed.append((tile['name'],x,y,images))
        paths = []
        for label, image in zip(('native', 'metal'), canvases):
            path = output / f'{label}-{tick:03d}-whole.png'
            image.save(path)
            paths.append(str(path.resolve()))
        panel = Image.new('RGB', (1280, 680))
        draw = ImageDraw.Draw(panel)
        for i, (label, image) in enumerate(zip(('Original software', 'Actual native-domain Metal'), canvases)):
            panel.paste(image.resize((640, 650)), (i*640, 30))
            draw.text((i*640+5, 5), f'{label}, tick{tick}, fixed tiled coverage', fill='white')
        panels.append(panel)
        cases.append(dict(tick=tick, images=paths, sha256=[sha(Path(p).read_bytes()) for p in paths],
                          all_root_objects=56, geometry_relative_bounds=relative_bounds[tick],
                          covered_rectangle=[-1280,-1300,1280,1300], overlaps=overlaps))
    contact = Image.new('RGB', (1280, 1360))
    for i, panel in enumerate(panels):
        contact.paste(panel, (0,i*680))
    contact.save(output / 'contact.png')
    report = dict(status='awaiting_independent_review', accepted=False, type_id='0x0ea34fa3',
                  tiles=tiles, cases=cases, initialization=initialization,
                  original_sha256=sha(executable.read_bytes()), asset_sha256=sha(asset),
                  binary_sha256=tiles[0]['binary_sha256'], probe_sha256=sha(Path(__file__).read_bytes()),
                  contact_sha256=sha((output / 'contact.png').read_bytes()),
                  scope='Twelve whole original/actualMetal pairs at60/90 in six fixed1920x1080 views. All56 original root matrix decisions audited; every submitted geometry bound, including software-rejected triangles, fits the union. No scale, worldcamera, light, phase, geometry or fixed640/480 span-limit change. SourceMode80 retained; original fullRender/controller/RenderObject/lighting/raster runs twice per sample. ActualMetal renders every24Hz tick, screenshots stride30. Integer screen-origin assembly only; per-view overlap residuals reported, not hidden. Supplements existing eight-phase lifecycle comparison; naturalcaller/audio/device/exactpixel gates remain open.')
    (output / 'manifest.json').write_text(json.dumps(report, indent=2)+'\n')
    print(report['status'], sha((output / 'manifest.json').read_bytes()), flush=True)
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('executable', 'binary', 'scenario', 'output'):
        parser.add_argument('--'+name, type=Path, required=True)
    args = parser.parse_args()
    run(args.executable, args.binary, args.scenario, args.output)
