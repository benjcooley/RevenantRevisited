"""Summarize genuine arena actor progression without inferring hardware cadence.

Inputs come from mapped_scene_frame's full original TimerTick(draw=true) path.
The 41ms WinMM deliveries are controlled virtual time, not a measurement of a
Windows98 machine or the emulator's host throughput.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import zipfile


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def report(initial_path, frames_path, imagery_path):
    initial = json.loads(initial_path.read_text())
    run = json.loads(frames_path.read_text())
    if run['failure'] or not run['draw']:
        raise ValueError('Require a successful full original draw=true capture')
    if not run['frames']:
        raise ValueError('Capture has no frames')
    timers = list(run['multimedia_timers'].values())
    if len(timers) != 1 or timers[0][:2] != [0x49e2f0, 41]:
        raise ValueError('Unexpected registered original WinMM timer')
    before = initial['frames'][-1]
    source_frames = [before] + run['frames']
    screen_frames = [frame['screen_frame'] for frame in source_frames]
    if any(b != a + 1 for a, b in zip(screen_frames, screen_frames[1:])):
        raise ValueError('Screen frames are not consecutive')
    targets = []
    with zipfile.ZipFile(imagery_path) as archive:
        members = {name.lower().replace('\\', '/'): name for name in archive.namelist()}
        for actor in before['targets']:
            x, y, _ = actor['position']
            if not (11500 <= x <= 13200 and 16900 <= y <= 18100):
                continue
            asset = 'Imagery/' + actor['asset'].replace('\\', '/')
            raw = archive.read(members[asset.lower()])
            count = struct.unpack_from('<I', raw, 24)[0]
            state = actor['state']
            if not 0 <= state < count:
                raise ValueError('Actor state outside actual imagery header')
            headers = {}
            for s in range(count):
                offset = 28 + s * 76
                label = raw[offset:offset + 40].split(b'\0')[0].decode('cp1252')
                sflags, slength = struct.unpack_from('<HH', raw, offset + 40)
                headers[s] = dict(name=label, flags=sflags, frames=slength)
            name = headers[state]['name']
            if ':watch' not in name.lower():
                continue
            flags, length = headers[state]['flags'], headers[state]['frames']
            if length != actor['clip_frames']:
                raise ValueError('Original GetAniLength and packed header disagree')
            rows = []
            for frame in source_frames:
                matches = [a for a in frame['targets'] if a['index'] == actor['index']]
                if len(matches) != 1:
                    raise ValueError('Saved arena spectator vanished or duplicated')
                a = matches[0]
                if a['state'] not in headers or headers[a['state']]['frames'] != a['clip_frames']:
                    raise ValueError('Captured state and original packed clip disagree')
                rows.append(dict(screen_frame=frame['screen_frame'], state=a['state'],
                                 state_name=headers[a['state']]['name'],
                                 state_flags=headers[a['state']]['flags'],
                                 frame=a['frame'], increment=a['frame_increment'],
                                 clip_frames=a['clip_frames']))
            transitions = []
            restarts = []
            terminal_holds = []
            mismatches = []
            for a, b in zip(rows, rows[1:]):
                if a['state'] != b['state']:
                    transitions.append(dict(before=a, after=b))
                elif b['frame'] < a['frame'] and a['frame'] == a['clip_frames'] - 1:
                    restarts.append(b['screen_frame'])
                elif b['frame'] == a['frame'] == a['clip_frames'] - 1 and not (a['state_flags'] & 1):
                    # Non-looping watch states clamp at their terminal frame.
                    # The actor's own action controller decides when to restart.
                    terminal_holds.append(b['screen_frame'])
                elif b['frame'] != a['frame'] + a['increment']:
                    mismatches.append(dict(before=a, after=b))
            targets.append(dict(index=actor['index'], asset=actor['asset'],
                                position=actor['position'], watch_state=state,
                                watch_name=name, authored_frames=length,
                                authored_state_flags=hex(flags),
                                asset_sha256=hashlib.sha256(raw).hexdigest(),
                                records=rows, state_transitions=transitions,
                                observed_restart_screen_frames=restarts,
                                terminal_hold_screen_frames=terminal_holds,
                                progression_mismatches=mismatches,
                                authored_traversal_virtual_ms=length * 41,
                                observed_restart_intervals_virtual_ms=[(b - a) * 41 for a, b in zip(restarts, restarts[1:])],
                                full_cycle_observed=(len(restarts) >= 2 and
                                                     not transitions and not mismatches)))
    return dict(status='natural_actor_progression_report', accepted=False,
                source_sha256=digest(Path(__file__)), initial_sha256=digest(initial_path),
                frames_sha256=digest(frames_path), imagery_sha256=digest(imagery_path),
                ticks=len(run['frames']), virtual_callback_period_ms=41,
                controlled_virtual_elapsed_ms=len(run['frames']) * 41,
                spectator_count=len(targets), spectators=targets,
                all_selected_cycles_observed=bool(targets) and all(t['full_cycle_observed'] for t in targets),
                scope=__doc__,
                limits='Original actors, saved watch states and full drawing run. '
                       'Cache warming omitted by the input fixture; source drawing '
                       'colors remain under investigation. No visual parity, '
                       'device pacing, measured Windows98 wall cadence, or helper '
                       'EFFECT cadence acceptance. No actor/rate writes or fitting.')


if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--initial-manifest', type=Path, required=True)
    p.add_argument('--frames-manifest', type=Path, required=True)
    p.add_argument('--imagery', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    a = p.parse_args()
    if a.output.exists():
        raise FileExistsError('Use a new report path')
    r = report(a.initial_manifest, a.frames_manifest, a.imagery)
    a.output.write_text(json.dumps(r, indent=2) + '\n')
    print(json.dumps(dict(ticks=r['ticks'], spectators=r['spectator_count'],
                          full_cycles=r['all_selected_cycles_observed'],
                          clips=[(s['watch_name'], s['authored_frames'],
                                  len(s['observed_restart_screen_frames']),
                                  len(s['progression_mismatches'])) for s in r['spectators']])))
