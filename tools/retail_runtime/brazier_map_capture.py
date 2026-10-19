"""Owned capture of unchanged Nakranoth map brazier animation.

The camera is an explicit caller input. There are no spawns, asset replacements,
per-tick frame edits or color/coordinate fitting. This verifies map animation
liveness/cadence; original scene/device/pixel acceptance is a separate gate.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import time
import zipfile
from PIL import Image
ROOT = Path(__file__).resolve().parents[2]


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def capture(binary, data, output, expect_animation):
    binary = binary.resolve(); data = data.resolve(); output = output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    save = output/'save'; save.mkdir(); shutil.copy2(data/'revenant.ini',save/'Revenant.ini')
    commands = output/'commands.txt'
    commands.write_text('# Tick the unchanged real map. No spawns/edits.\n1 | TwnBrazier | @expect 0x70dc006a 9678 9614 259 unused 0\n')
    binary_sha = digest(binary)
    with zipfile.ZipFile(data/'imagery.rvi') as archive:
        asset = archive.read('Imagery/Town/twnbrazier.i2d')
    env = dict(os.environ, REVENANT_DATA_PATH=str(data), REVENANT_ASSETS_PATH=str(ROOT/'assets'),
               REVENANT_SAVE_PATH=str(save), REVENANT_MAP_ANIM_TRACE='TwnBrazier')
    command = [str(binary),'--headless','--test=sector','--scene-camera=49,9678,9614,259',
               '--scene-command-file='+str(commands),'--filmstrip=20,0.0416666666666667',
               '--snapstep=0.0416666666666667','--snapseed=1','--snapwarmup=1',
               '--snaprect=0,0,640,340','--snapprefix='+str(output/'frame-')]
    start = time.monotonic()
    with (output/'run.log').open('wb') as log:
        process = subprocess.run(command,cwd=output,env=env,stdout=log,stderr=log,timeout=90)
    log = (output/'run.log').read_text(errors='replace')
    frames = sorted(output.glob('frame-[0-9][0-9][0-9]-*.png'))
    hashes = [hashlib.sha256(Image.open(p).convert('RGB').tobytes()).hexdigest() for p in frames]
    samples = [tuple(map(int, row)) for row in re.findall(
        r'\[map-anim\] id=70dc006a index=1019174967 layer=(\d) state=(\d+) frame=(\d+) rate=(-?\d+) asset=(-?\d+) reg=(-?\d+),(-?\d+),(-?\d+)',log)]
    overlays = [r for r in samples if r[0] == 1]; stills = [r for r in samples if r[0] == 0]
    errors = []
    if process.returncode != 0 or len(frames) != 20 or 'COMPLETE rows=1 failures=0' not in log:
        errors.append('Actual map process/capture/typed owner observation failed')
    if expect_animation:
        if len(set(hashes)) != 15: errors.append('Expected the authored 15 distinct pictures')
        if len(hashes) != 20 or any(hashes[i] != hashes[i+15] for i in range(5)):
            errors.append('Fifteen-frame loop did not reproduce identical pixels')
        if len(overlays) != 21 or len(stills) != 21:
            errors.append('Expected separate still/animation records in every rendered frame')
        if any(r[1] != 0 or r[3] != 1 for r in overlays): errors.append('State/rate changed')
        if any(b[2] != (a[2]+1)%15 for a,b in zip(overlays,overlays[1:])):
            errors.append('Authored owner frame did not advance once per legacy tick')
        if len({r[4] for r in stills}) != 1 or any(r[5:] != (20,105,318) for r in stills):
            errors.append('Original static still/base asset or registration changed')
        if any(r[5:] != (13,204,56) for r in overlays):
            errors.append('Authored animation registration changed')
    elif len(set(hashes)) != 1:
        errors.append('Expected the pre-fix static map picture')
    if digest(binary) != binary_sha: errors.append('Owned executable changed during capture')
    report = dict(status='pass' if not errors else 'differences_found',errors=errors,accepted=False,
                  effect='TwnBrazier',type_id='0x70dc006a',class_name='TILE',outside_effect_176=True,
                  map_index=1019174967,position=[9678,9614,259],camera=[49,9678,9614,259],
                  command=command,exit_code=process.returncode,elapsed_seconds=time.monotonic()-start,
                  binary_sha256=binary_sha,asset_sha256=hashlib.sha256(asset).hexdigest(),
                  probe_sha256=digest(__file__),working_tree_map_renderer_sha256=digest(ROOT/'src/game/world/maprenderer.cpp'),
                  expected_animation=expect_animation,distinct_frames=len(set(hashes)),
                  readonly_layer_samples=samples,frames=[dict(path=str(p),sha256=digest(p),pixel_sha256=h) for p,h in zip(frames,hashes)],
                  log_sha256=digest(output/'run.log'),scope=__doc__)
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    if errors: raise RuntimeError('Brazier map capture failed; inspect '+str(output/'manifest.json'))
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--binary',type=Path,required=True)
    parser.add_argument('--data-root',type=Path,required=True)
    parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--expect-animation',choices=('yes','no'),required=True)
    args = parser.parse_args()
    result = capture(args.binary,args.data_root,args.output,args.expect_animation == 'yes')
    print(json.dumps({k:v for k,v in result.items() if k not in ('readonly_layer_samples','frames')},indent=2))
