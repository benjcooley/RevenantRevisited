"""Actual configured Paralize gallery completion, cleanup and recreation."""
import argparse
import json
import os
import re
import subprocess
import time
from pathlib import Path
from PIL import Image
from port_capture import ROOT, sha


def run(binary, data, output):
    output.mkdir(parents=True, exist_ok=False)
    save = output / 'save'
    save.mkdir()
    (save / 'Revenant.ini').write_bytes((data / 'revenant.ini').read_bytes())
    pinned = sha(binary)
    command = [str(binary), '--headless', '--test=vfx-review',
               '--vfx-review-effects=0xad92bc14', '--vfx-review-first',
               '--partsys-quality=0', '--vfx-review-speed=0.1',
               '--resolution=640x480', '--vfx-no-ui', '--filmstrip=32,1',
               '--snapstep=0.0416666666666667', '--snapwarmup=1',
               '--snapseed=1', '--snaprect=0,0,640,480',
               '--snapprefix=' + str(output / 'frame-')]
    start = time.monotonic()
    with (output / 'run.log').open('wb') as log:
        process = subprocess.run(command, cwd=ROOT, env=dict(os.environ,
            REVENANT_DATA_PATH=str(data), REVENANT_SAVE_PATH=str(save),
            REVENANT_ASSETS_PATH=str(ROOT / 'assets')),
            stdout=log, stderr=log, timeout=120)
    log = (output / 'run.log').read_text()
    frames = sorted(output.glob('frame-[0-9][0-9][0-9]-*.png'))
    samples = []
    for frame in frames:
        with Image.open(frame) as image:
            image = image.convert('RGB')
            assert image.size == (640, 480)
            samples.append(dict(path=str(frame), sha256=sha(frame),
                                visible=image.getbbox() is not None))
    cleanup = re.findall(r'\[paralize-target-cleanup\] tick=(\d+) alive=(\d+) flags_before=([0-9a-f]+) flags_after=([0-9a-f]+) health=(\d+) target=\(([^)]+)\)', log)
    rng = re.findall(r'\[paralize-target-rng\] actor_setup_draws=(\d+) seeded=(\d+)', log)
    completed = [row for row in cleanup if row[0] == '260' and row[1] == '0']
    restored = all(row[2] == row[3] and row[4] == '102' and
                   row[5] == '3281,3185,16' for row in cleanup)
    ok = (process.returncode == 0 and len(samples) == 32 and
          'implementation=TParalize1_CONFIGURED_TARGET' in log and
          '[shutdown] complete' in log and len(completed) >= 2 and
          len(rng) >= 3 and all(row[1] == '0' for row in rng) and
          restored and any(row['visible'] for row in samples) and sha(binary) == pinned)
    report = dict(status='pass' if ok else 'fail', type_id='0xad92bc14',
        binary_sha256=pinned, command=command, exit_code=process.returncode,
        elapsed_seconds=time.monotonic()-start, frames=samples,
        cleanup_records=cleanup, completed_lifecycles=len(completed),
        target_flags_health_position_restored=restored,
        creation_rng_records=rng, gallery_reseeded=any(row[1] != '0' for row in rng),
        visible_frames=sum(row['visible'] for row in samples),
        log_sha256=sha(output / 'run.log'), accepted=False,
        scope='Actual scrolling gallery configured genuine Acolyte target, fixed offset96/radius16, full24Hz simulation through repeated260tick completion. Check real cleanup and no gallery RNG reseeding; appearance, natural gameplay target selection, audio/device and other configurations are separate.')
    (output / 'manifest.json').write_text(json.dumps(report, indent=2)+'\n')
    print(report['status'], len(completed), report['visible_frames'], flush=True)
    if not ok:
        raise RuntimeError('Configured target gallery failed: '+str(output / 'manifest.json'))
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--binary', type=Path, required=True)
    parser.add_argument('--data-root', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    run(args.binary.resolve(), args.data_root.resolve(), args.output.resolve())
