#!/usr/bin/env python3
"""Exercise real map texture animation/create/move/delete on a clean floor.

Companion to static_texture_probe's retail A/B. Requires a prepared ground-only
module; does not prove natural placement or retail/Metal device pixel parity.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import time
from PIL import Image

ROOT = Path(__file__).resolve().parents[2]


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def image_digest(path):
    return hashlib.sha256(Image.open(path).convert('RGB').tobytes()).hexdigest()


def run(binary, data, module, camera, output, profiles):
    output = output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    level, x, y, z = camera
    rows = []
    positions = {}
    for i, profile in enumerate(profiles):
        name = profile['name']
        delta = int((i - (len(profiles) - 1) / 2) * 40)
        positions[name] = (x + delta, y - delta)
        px, py = positions[name]
        rows.extend([f'3 | - | addat {px} {py} {name}',
                     f'3 | {name} | @expect {profile["id"]} {px} {py} {z} unused 0'])
    for tick in range(4, 8):
        for profile in profiles:
            frame = (tick - 3) % profile['owner_frames']
            rows.append(f'{tick} | {profile["name"]} | @frame {frame}')
    for profile in profiles:
        rows.append(f'8 | {profile["name"]} | move 16 -8 4')
    for profile in profiles:
        px, py = positions[profile['name']]
        rows.append(f'9 | {profile["name"]} | @expect {profile["id"]} {px+16} {py-8} {z+4} unused 0')
    rows.extend(f'11 | {p["name"]} | delete' for p in profiles)
    rows.extend(f'12 | {p["name"]} | @absent' for p in profiles)
    commands = output / 'commands.txt'
    commands.write_text('\n'.join(rows) + '\n')
    save = output / 'save'
    save.mkdir()
    (save / 'Revenant.ini').write_bytes((data / 'revenant.ini').read_bytes())
    binary = binary.resolve()
    env = os.environ.copy()
    env.update(REVENANT_DATA_PATH=str(data.resolve()), REVENANT_SAVE_PATH=str(save),
               REVENANT_ASSETS_PATH=str(ROOT / 'assets'))
    command = [str(binary), '--headless', '--test=sector', '--scene-module='+module,
        '--scene-camera='+','.join(map(str, camera)), '--scene-ambient=32,255,255,255',
        '--scene-command-file='+str(commands), '--filmstrip=16,0.0416666666666667',
        '--snapstep=0.0416666666666667', '--snapseed=1', '--snapwarmup=1',
        '--snaprect=0,0,640,340', '--snapprefix='+str(output / 'frame-')]
    pinned_binary = digest(binary)
    started = time.monotonic()
    with (output / 'run.log').open('wb') as log:
        process = subprocess.Popen(command, cwd=output, env=env, stdout=log, stderr=log)
        forced = False
        try:
            process.wait(timeout=45)
        except subprocess.TimeoutExpired:
            forced = True
            process.terminate()
            try:
                process.wait(timeout=2)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait(timeout=2)
    log = (output / 'run.log').read_text(errors='replace')
    frames = sorted(output.glob('frame-[0-9][0-9][0-9]-*.png'))
    hashes = [image_digest(p) for p in frames]
    errors = []
    if forced or process.returncode != 0:
        errors.append('Process did not exit cleanly')
    if f'COMPLETE rows={len(rows)} failures=0' not in log or 'status=FAIL' in log:
        errors.append('Map command observations did not all pass')
    if len(frames) != 16:
        errors.append('Incomplete frame capture')
    distinct = len(set(hashes[3:7]))
    if distinct != 4:
        errors.append('Four owner frames did not produce four different rendered images')
    owner_regions = {}
    for profile in profiles:
        if not profile['name'].startswith('ogrokwatcher') or len(frames) != 16:
            continue
        px, py = positions[profile['name']]
        center = 320 + (px-x) - (py-y)
        # Independent fixed windows sample each watcher's upper body; these
        # are observation regions, not fitted production geometry or scale.
        rectangle = (center-30,0,center+30,160)
        samples = [hashlib.sha256(Image.open(p).convert('RGB').crop(rectangle).tobytes()).hexdigest()
                   for p in frames[3:7]]
        owner_regions[profile['name']] = dict(rectangle=rectangle,pixel_sha256=samples,
                                              distinct_frames=len(set(samples)))
        if len(set(samples)) != 4:
            errors.append(profile['name']+' did not render all four texture frames')
    floor_restored = len(hashes) == 16 and hashes[0] == hashes[-1]
    if not floor_restored:
        errors.append('Deletion did not restore the original clean floor')
    if digest(binary) != pinned_binary:
        errors.append('Executable changed during capture')
    report = dict(status='pass' if not errors else 'differences_found', errors=errors,
        command=command, binary_sha256=pinned_binary, commands_sha256=digest(commands),
        profiles=profiles, elapsed_seconds=time.monotonic()-started,
        exit_code=process.returncode, forced_shutdown=forced, command_rows=len(rows),
        completed_rows=len(re.findall(r'row line=.*status=PASS', log)),
        distinct_stationary_animation_frames=distinct, floor_restored=floor_restored,
        owner_regions=owner_regions,probe_sha256=digest(__file__),
        frames=[dict(path=str(p),pixel_sha256=h) for p,h in zip(frames,hashes)],
        log_sha256=digest(output/'run.log'),
        scope='Actual map NewObject/default animator/NextFrame/MOVE/DELETE and Metal output. '
              'Synthetic ground-only module; natural context and retail device fidelity remain open.')
    (output / 'manifest.json').write_text(json.dumps(report, indent=2)+'\n')
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--binary', type=Path, required=True)
    parser.add_argument('--data-root', type=Path, required=True)
    parser.add_argument('--module', default='FontRuntimeLab')
    parser.add_argument('--camera', default='110,10000,10000,16')
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    profiles = [dict(name='StillWater',id='0xadbcef14',owner_frames=1),
                *[dict(name=f'ogrokwatcher{i}',id=f'0x{0xaeaeeb22+i:08x}',owner_frames=4) for i in (1,2,3)]]
    report = run(args.binary,args.data_root,args.module,tuple(map(int,args.camera.split(','))),args.output,profiles)
    print(json.dumps({k:v for k,v in report.items() if k != 'frames'},indent=2))
    raise SystemExit(0 if report['status']=='pass' else 1)
