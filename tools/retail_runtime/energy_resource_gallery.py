#!/usr/bin/env python3
"""Run all176 gallery slots beyond two loops and audit GPU mesh allocations."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import time


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def audit(output, binary_hash, command, elapsed, exit_code, baseline=None):
    log = (output / 'run.log').read_text()
    frames = sorted(output.glob('frame-[0-9][0-9][0-9]-*.png'))
    ready = re.search(r'\[vfx-review\] ready types=(\d+).*loop_seconds=([\d.]+)', log)
    per_loop, allocations, loop = {}, {}, 1
    for line in log.splitlines():
        transition = re.search(r'\[vfx-review\] loop=(\d+)', line)
        if transition:
            loop = int(transition[1])
        allocation = re.search(r'\[mesh-allocation\] handle=(\d+) key=([0-9a-f]+) vertices=(\d+) indices=(\d+) texture=(\d+)', line)
        if allocation:
            allocations.setdefault(loop, []).append(dict(zip(('handle', 'key', 'vertices', 'indices', 'texture'), allocation.groups())))
        resource = re.search(r'\[energyspray-resources\] fixed_meshes=3 cache_meshes=(\d+) vertex_bytes=(\d+) index_bytes=(\d+)', line)
        if resource:
            per_loop.setdefault(loop, []).append(list(map(int, resource.groups())))
    checks = dict(clean_exit=exit_code == 0 and '[shutdown] complete' in log,
                  all176=bool(ready and int(ready[1]) == 176), screenshots=len(frames) == 22,
                  beyond_two_loops=loop >= 3, traced=bool(allocations.get(1)),
                  no_pool_failures=not any(x in log for x in ('mesh upload failed', 'buffer pool exhausted', 'BUFFER_POOL_EXHAUSTED', 'VALIDATE_')),
                  no_allocations_after_first_loop=not any(allocations.get(i) for i in range(2, loop + 1)),
                  stable_resources=2 in per_loop and 3 in per_loop and max(per_loop[2]) == max(per_loop[3]))
    rows = [dict(path=str(p), sha256=sha(p)) for p in frames]
    comparison = None
    if baseline:
        prior = sorted(baseline.glob('frame-[0-9][0-9][0-9]-*.png'))
        differences = [i + 1 for i, (a, b) in enumerate(zip(prior, frames)) if sha(a) != sha(b)]
        checks['same_index_gallery_pixels'] = len(prior) == len(frames) and not differences
        comparison = dict(baseline=str(baseline), frame_count=len(prior), differing_frames=differences,
                          baseline_log_sha256=sha(baseline / 'run.log'))
    report = dict(status='pass' if all(checks.values()) else 'fail',
                  resource_status='pass' if all(value for name, value in checks.items() if name != 'same_index_gallery_pixels') else 'fail',
                  pixel_status=('pass' if checks.get('same_index_gallery_pixels') else 'different') if baseline else 'not_requested',
                  checks=checks,
                  binary_sha256=binary_hash, command=command, exit_code=exit_code, elapsed_seconds=elapsed,
                  loop_seconds=float(ready[2]) if ready else None, last_loop=loop,
                  per_loop_resources=per_loop, allocations_by_loop=allocations,
                  log_sha256=sha(output / 'run.log'), frames=rows, pixel_comparison=comparison,
                  scope='Actual all176 gallery at24Hz, 22 sparse screenshots over630 seconds. Allocation tracing spans every draw/owner recreation. Require zero new meshes after the first complete loop, stable byte totals, and optional same-index PNG identity. No phase matching, geometry changes or pool-size increase.')
    (output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    print(report['status'], checks, flush=True)
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('binary', 'data-root', 'save-root', 'assets-root', 'output'):
        parser.add_argument('--' + name, type=Path, required=True)
    parser.add_argument('--baseline', type=Path)
    args = parser.parse_args()
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    binary = args.binary.resolve()
    pinned = sha(binary)
    command = [str(binary), '--headless', '--resolution=640x480', '--test=vfx-review', '--vfx-no-ui',
               '--vfx-review-speed=96', '--filmstrip=22,30', '--snapstep=0.0416666666666667',
               '--snapwarmup=1', '--snapseed=1', '--snapprefix=' + str(output / 'frame-')]
    start = time.monotonic()
    with (output / 'run.log').open('w') as stream:
        result = subprocess.run(command, env=dict(os.environ, REVENANT_TRACE_MESH_ASSETS='1',
                                REVENANT_DATA_PATH=str(args.data_root.resolve()),
                                REVENANT_SAVE_PATH=str(args.save_root.resolve()),
                                REVENANT_ASSETS_PATH=str(args.assets_root.resolve())),
                                stdout=stream, stderr=stream, timeout=900)
    assert sha(binary) == pinned, 'binary changed during capture'
    report = audit(output, pinned, command, time.monotonic() - start, result.returncode, args.baseline)
    raise SystemExit(0 if report['status'] == 'pass' else 1)
