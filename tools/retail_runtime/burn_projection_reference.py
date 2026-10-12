#!/usr/bin/env python3
"""Audit Burn's actual submission against unmodified original slot/draw records.

The native manifest comes from burn_actor_visual_reference: real owner, actor,
bone sampling, controller and renderer. This exports readonly evidence to the
compiled production submission test, never into the game's runtime.
"""
import argparse
import json
import struct
from pathlib import Path
from burn_owner_probe import run as submission
from speed_profile_contract import sha


def run(native, output):
    reference = json.loads(native.read_text())
    if reference.get('error') or reference.get('interpolate') is not False:
        raise ValueError('Require complete original integer-pose reference')
    output.mkdir(parents=True, exist_ok=False)
    lines = []
    slots = 0
    for row in reference['rows']:
        live = []
        for pool in (0, 1):
            raw = bytes.fromhex(row['smoke_hex'] if pool else row['fire_hex'])
            for index in range(70):
                fields = struct.unpack_from('<18f4i', raw, index * 88)
                used = bool(fields[21]) and not row['dead_no_calls']
                lines.append(f'S {row["tick"]} {pool} {index} {int(used)}' +
                             (''.join(f' {value:.9g}' for value in fields[:3]) if used else ''))
                if used:
                    live.append((pool, index))
                    slots += 1
        if len(live) != len(row['draws']):
            raise ValueError(f'Original live slots/draw count disagree at {row["tick"]}')
        for (pool, index), draw in zip(live, row['draws']):
            if draw['object'] != (0 if pool else 1):
                raise ValueError('Original fire/smoke object order differs')
            lines.append(f'D {row["tick"]} {pool} {index} {draw["object"]}' +
                         ''.join(f' {value:.9g}' for value in draw['matrix']))
    trace = output / 'original-slot-draw.trace'
    trace.write_text('\n'.join(lines) + '\n')
    checked = submission(output / 'production', trace)
    report = dict(status=checked['status'], accepted=False, original_slots=slots,
                  native_manifest_sha256=sha(native.read_bytes()),
                  original_trace_sha256=sha(trace.read_bytes()),
                  production_manifest_sha256=sha((output / 'production/manifest.json').read_bytes()),
                  native_vertices=checked['vertices'], common_vertices=checked['common_vertices'],
                  scope='Original controller slot centers and full Render matrices feed readonly compiled production submission audit. Native matrices/XY remain exact; common raw map center and existing local WORLD3D_Z_SCALE contract pass. This does not replace the original renderer, certify common framebuffer equivalence, or prove natural moving-actor attachment.')
    (output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--native', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    print(json.dumps(run(args.native, args.output), indent=2))
