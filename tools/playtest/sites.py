#!/usr/bin/env python3
"""Grinding start points: Locke placed at the best hunting sites, saved.

    sites.py [--from 'left the keep'] [--level 0] [--count 3]

The best sites on the level for the Locke of the parent waypoint (his level
and style, grind.py) each get a waypoint: sites for a stronger Locke come
from a waypoint grinding reached (`--from 'grind: level 3'`). The port loads the parent waypoint's save,
puts Locke (the control channel's `exec player.pos`) on the walkable spot
nearest the site that keeps SPACE from its placed monsters -- walkable by
the game's own step rule over the walk grid (nav.Walkable) -- lets him
stand two seconds, and quick-saves. The waypoint, `site L<band>: <its
monsters>` in the `grind` story, is where a grinding session starts in
either game (the saves are one format):

    play.py retail --story grind --from 'port:site L1: 6 Araknid, 1 generators'

Placing isn't input, so these waypoints have no recording: the save is the
fixture. A spot is checked before the save: in play, control on, Locke
alive and standing where he was put.
"""
from __future__ import annotations

import argparse
import math
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from grind import Grounds, Site, hostile  # noqa: E402
from host import PORT_BINARY, PORT_INI, PortHost  # noqa: E402
from inputs import Bindings, Plan  # noqa: E402
from knowledge import Knowledge  # noqa: E402
from nav import DEFAULT_RADIUS, Walkable, centre  # noqa: E402
from walk import CELL, WalkGrid, cell_of  # noqa: E402
from waypoints import ROOT, Catalogue  # noqa: E402
from world import World  # noqa: E402

REPO = HERE.parents[1]
SPACE = 200                          # world units a start point keeps from the site's placed monsters
SEARCH = 640                         # how far from the site's centre a start point may be
SETTLE = 48                          # ticks Locke stands before the save
PLACED_WITHIN = 24                   # he must stand this near where he was put


def start_point(grid: WalkGrid, site: Site, monsters: list[tuple[int, int]], radius: int) -> tuple[int, int, int] | None:
    """The cell nearest the site's centre where Locke can stand, SPACE
    from every placed monster of the site."""
    walk = Walkable(lambda c: grid.cell_height(*c), radius)
    cx, cy = cell_of(site.centre[0]), cell_of(site.centre[1])
    reach = SEARCH // CELL
    best = None
    for gy in range(cy - reach, cy + reach + 1):
        for gx in range(cx - reach, cx + reach + 1):
            x, y = centre((gx, gy))
            d = math.dist((x, y), site.centre[:2])
            if d > SEARCH or (best and d >= best[0]):
                continue
            if any(math.dist((x, y), m) < SPACE for m in monsters):
                continue
            height = walk.stand((gx, gy))
            if height:
                best = (d, (x, y, height))
    return best[1] if best else None


def place(host: PortHost, level: int, point: tuple[int, int, int], settle: int = SETTLE) -> dict:
    host.exec(f'player.pos {point[0]} {point[1]} {point[2]} {level}')
    host.step(settle)
    return host.observe(objects=800)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    parser.add_argument('--from', dest='parent', default='left the keep', help='the port waypoint to place from')
    parser.add_argument('--level', type=int, default=0)
    parser.add_argument('--count', type=int, default=3, help='how many sites')
    parser.add_argument('--style', default='mixed')
    parser.add_argument('--waypoints', type=Path, default=ROOT)
    parser.add_argument('--run', type=Path, default=Path(tempfile.mkdtemp(prefix='sites-')))
    args = parser.parse_args()

    catalogue = Catalogue(args.waypoints)
    parent = catalogue.get('port', args.parent)
    kb = Knowledge.load(REPO / 'data')
    world = World(REPO / 'data', parent.slot_dir)
    grounds = Grounds.survey(world, kb, args.level)
    band = (parent.player.get('xp') or [1])[0]
    ranked = sorted(grounds.sites, key=lambda s: -grounds.score(s, band, args.style))
    chosen = [(band, s) for s in ranked if grounds.score(s, band, args.style) > 0][:args.count]
    bindings = Bindings.from_ini(PORT_INI, defaults=REPO / 'data' / 'revenant.ini')

    made = 0
    for band, site in chosen:
        name = f'site L{band}: {site.words()}'
        monsters = [p.pos[:2] for p in world.objects(args.level) if hostile(kb, p)
                    and math.dist(p.pos[:2], site.centre[:2]) <= SEARCH + SPACE]
        run = args.run / f'L{band}-{site.centre[0]}-{site.centre[1]}'
        with PortHost.launch(PORT_BINARY, run, PORT_INI, name='sites') as host:
            host.hello()
            host.start('slot', seed=1, slot=parent.slot_dir, timeout=1800)
            place(host, args.level, site.centre, settle=2)            # near it, for its walk grid
            grid = WalkGrid(host.observe(objects=0, walkgrid=SEARCH + 64)['walkgrid'])
            point = start_point(grid, site, monsters, host.observe(objects=0)['player'].get('radius') or DEFAULT_RADIUS)
            if not point:
                print(f'{name}: no place to stand near it')
                continue
            obs = place(host, args.level, point)
            player = obs.get('player') or {}
            problems = [p for p, bad in (
                (f"screen {obs.get('screen')}", obs.get('screen') != 'play'),
                ('control off', not obs.get('control')),
                ('Locke dead', (player.get('health') or [0])[0] <= 0),
                (f"standing at {player.get('pos')}, put at {point}",
                 math.dist(player.get('pos', (0, 0))[:2], point[:2]) > PLACED_WITHIN)) if bad]
            if problems:
                print(f'{name}: {"; ".join(problems)}')
                continue
            plan = Plan()
            end = plan.press(0, bindings['QuickSave'])
            host.step(plan.ticks(end + 48), plan.events())
            exported = host.export_slot('@last', run / 'export')
            waypoint = catalogue.add('port', name, 'grind', 0, parent.name, exported['path'], obs=obs,
                                     build=host.info.get('build'), tick=host.tick, slot_name=exported['slot'],
                                     note=f'placed by sites.py at {list(point)}, level {args.level}, '
                                          f'for a level-{band} Locke; site centre {list(site.centre)}')
            made += 1
            print(f'{name}: {waypoint.dir}')
    return 0 if made == len(chosen) else 1


if __name__ == '__main__':
    sys.exit(main())
