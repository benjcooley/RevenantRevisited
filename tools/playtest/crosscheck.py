#!/usr/bin/env python3
"""Load one save in both games and list where they differ.

    crosscheck.py --from GAME:WAYPOINT [--ticks 24] [--out DIR]
    crosscheck.py --slot SLOT_DIR [--out DIR]

The port and the shipped game (in the emulator) load the same save format,
so the same slot, played the same number of ticks with no input, should
give the same observation. Each host is started from the slot with seed 1,
stepped, then asked the same questions:

- the player: level, position, facing, the stats, mode, radius, weapon,
  what's equipped, in the pack (with each cell's screen centre), on the belt
  and in the quick-spell slots;
- the objects that don't move by themselves (items, exits, containers, ...):
  the same ones, at the same place, drawn at the same screen pixel;
  characters only by who is there, since their AI draws from each game's
  own random numbers;
- where a left click reaches the world (`pick` over a grid of the screen),
  and the map point each pixel picks;
- the walk grid round the player, where both games know the cells.

A difference is a variance: one of the games is wrong, or the observation
is. The report (crosscheck.json in --out) lists each one with both values;
the exit status is 1 when there are any. Known differences that are the
port's by design are listed in EXPECTED and reported apart.
"""
from __future__ import annotations

import argparse
import json
import math
import sys
import tempfile
from dataclasses import dataclass, field
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from host import PORT_BINARY, PORT_INI, Host, PortHost, RetailHost, clickwalk_build  # noqa: E402
from waypoints import ROOT, Catalogue  # noqa: E402

GRID_STEP = 16                       # the click grid's spacing, in screen pixels
SCREEN = (640, 480)
WALKGRID_RADIUS = 640
PICK_TOLERANCE = 2                   # world units a picked map point may differ by (rounding)
MOVING = {'character', 'player'}     # classes whose places the games' AI and random numbers decide

PLAYER_FIELDS = ('level', 'pos', 'facing', 'health', 'mana', 'fatigue', 'mode', 'radius', 'gold',
                 'xp', 'skills', 'weapon', 'belt', 'quick_spells', 'equipped')

# By design: the port loads a level's whole map, so it knows cells retail's
# streaming hasn't loaded (retail -1, the port their real height).
EXPECTED = {'walkgrid unknown on retail'}


@dataclass
class Report:
    variances: list[dict] = field(default_factory=list)
    expected: list[dict] = field(default_factory=list)

    def add(self, kind: str, what: str, port, retail, **detail) -> None:
        entry = dict(kind=kind, what=what, port=port, retail=retail, **detail)
        (self.expected if kind in EXPECTED else self.variances).append(entry)


def start(host: Host, slot: Path, ticks: int) -> None:
    host.hello()
    host.start('slot', seed=1, slot=slot, timeout=1800)
    host.step(ticks)


def compare_player(report: Report, port: dict, retail: dict) -> None:
    a, b = port.get('player') or {}, retail.get('player') or {}
    for name in PLAYER_FIELDS:
        if a.get(name) != b.get(name):
            report.add('player', name, a.get(name), b.get(name))
    if a.get('screen') != b.get('screen'):
        report.add('screen', 'player', a.get('screen'), b.get('screen'))
    pack_a = {i['slot']: i for i in a.get('pack') or []}
    pack_b = {i['slot']: i for i in b.get('pack') or []}
    for slot in sorted(set(pack_a) | set(pack_b)):
        if pack_a.get(slot) != pack_b.get(slot):
            report.add('pack', f'slot {slot}', pack_a.get(slot), pack_b.get(slot))
    for name in ('screen', 'control'):
        if port.get(name) != retail.get(name):
            report.add('game', name, port.get(name), retail.get(name))


def compare_objects(report: Report, port: dict, retail: dict) -> None:
    def keyed(obs):
        return {o['id']: o for o in obs.get('objects', [])}
    a, b = keyed(port), keyed(retail)
    for oid in sorted(set(a) | set(b)):
        x, y = a.get(oid), b.get(oid)
        name = (x or y).get('name') or (x or y).get('type')
        if x is None or y is None:
            report.add('object', f'{name} ({oid}) present', x is not None, y is not None)
            continue
        for key in ('name', 'type', 'class', 'enemy'):
            if x.get(key) != y.get(key):
                report.add('object', f'{name} {key}', x.get(key), y.get(key))
        if x.get('class') in MOVING:
            continue
        if x.get('pos') != y.get('pos'):
            report.add('object', f'{name} pos', x.get('pos'), y.get('pos'))
        elif x.get('screen') != y.get('screen'):
            report.add('screen', name, x.get('screen'), y.get('screen'))


def compare_clicks(report: Report, port: Host, retail: Host) -> None:
    """Where a click reaches the world, and the map point it picks; the
    disagreeing cells as a list of [x, y]."""
    world_differs, pos_differs = [], []
    for y in range(GRID_STEP // 2, SCREEN[1], GRID_STEP):
        for x in range(GRID_STEP // 2, SCREEN[0], GRID_STEP):
            a, b = port.pick(x, y), retail.pick(x, y)
            if a.get('world') != b.get('world'):
                world_differs.append([x, y, a.get('world'), b.get('world')])
            elif a.get('world') and math.dist(a['pos'], b['pos']) > PICK_TOLERANCE:
                pos_differs.append([x, y, a['pos'], b['pos']])
    if world_differs:
        report.add('clicks', 'world', sum(1 for d in world_differs if d[2]), sum(1 for d in world_differs if d[3]),
                   cells=world_differs, note='[x, y, port world, retail world]; port/retail: cells each calls world')
    if pos_differs:
        report.add('clicks', 'picked point', len(pos_differs), len(pos_differs), cells=pos_differs[:40],
                   note='[x, y, port pos, retail pos] (first 40)')


def compare_walkgrid(report: Report, port: dict, retail: dict) -> None:
    a, b = port.get('walkgrid'), retail.get('walkgrid')
    if not a or not b:
        report.add('walkgrid', 'offered', bool(a), bool(b))
        return
    if (a['origin'], a['w'], a['h']) != (b['origin'], b['w'], b['h']):
        report.add('walkgrid', 'shape', [a['origin'], a['w'], a['h']], [b['origin'], b['w'], b['h']])
        return
    differ, unknown = [], 0
    for i, (ha, hb) in enumerate(zip(a['heights'], b['heights'])):
        if ha == hb:
            continue
        if hb == -1 and ha != -1:                       # a sector retail hasn't streamed in
            unknown += 1
        else:
            differ.append([a['origin'][0] + i % a['w'], a['origin'][1] + i // a['w'], ha, hb])
    if differ:
        report.add('walkgrid', 'heights', len(differ), len(differ), cells=differ[:40],
                   note='[cell x, cell y, port, retail] (first 40)')
    if unknown:
        report.add('walkgrid unknown on retail', 'cells', 0, unknown)


def crosscheck(slot: Path, run: Path, ticks: int) -> Report:
    report = Report()
    with PortHost.launch(PORT_BINARY, run / 'port', PORT_INI, name='crosscheck') as port, \
            RetailHost.launch(run / 'retail', build=clickwalk_build()) as retail:
        start(port, slot, ticks)
        start(retail, slot, ticks)
        a = port.observe(objects=800, walkgrid=WALKGRID_RADIUS, frame=run / 'port.png')
        b = retail.observe(objects=800, walkgrid=WALKGRID_RADIUS, frame=run / 'retail.png')
        (run / 'port.json').write_text(json.dumps(a))
        (run / 'retail.json').write_text(json.dumps(b))
        compare_player(report, a, b)
        compare_objects(report, a, b)
        compare_walkgrid(report, a, b)
        compare_clicks(report, port, retail)
    return report


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    source = parser.add_mutually_exclusive_group(required=True)
    source.add_argument('--from', dest='waypoint', help='GAME:NAME, a waypoint')
    source.add_argument('--slot', type=Path, help='a save slot directory')
    parser.add_argument('--ticks', type=int, default=24)
    parser.add_argument('--out', type=Path, default=Path(tempfile.mkdtemp(prefix='crosscheck-')))
    parser.add_argument('--waypoints', type=Path, default=ROOT)
    args = parser.parse_args()
    if args.waypoint:
        game, _, name = args.waypoint.partition(':')
        slot = Catalogue(args.waypoints).get(game, name).slot_dir
    else:
        slot = args.slot
    run = args.out.resolve()
    run.mkdir(parents=True, exist_ok=True)
    report = crosscheck(slot, run, args.ticks)
    (run / 'crosscheck.json').write_text(json.dumps(dict(slot=str(slot), ticks=args.ticks,
                                                         variances=report.variances,
                                                         expected=report.expected), indent=1))
    for v in report.variances:
        print(f"{v['kind']:9s} {v['what']:32s} port={json.dumps(v['port'])[:70]}  retail={json.dumps(v['retail'])[:70]}")
    print(f'{len(report.variances)} variances, {len(report.expected)} expected; {run / "crosscheck.json"}')
    return 1 if report.variances else 0


if __name__ == '__main__':
    sys.exit(main())
