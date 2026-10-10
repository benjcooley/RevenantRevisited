"""Kata M7 for retail_ab.py: one Move per case (docs/gameplay/COMBAT_DOJO.md).

`combat-move` runs the character's Move slot -- TCharacter::Move `0x4c46d0`
(TPlayer::Move `0x518df0` for Locke), MoveStep `0x4c3bc0`, FindClearPath
`0x4c39d0`, CharBlocking `0x4d4db0`, GetWalkHeight / GetWalkHeightRadius as
original -- over a case's walkmap (`ground`: a height per 16-unit walk cell)
and the characters the map gives near a point (`nearby`). See
forensics/COMBAT_MOTION.md §3.7-3.9 for what each case is after.

The compare drops the pure queries from the seams (GetObjStat, GetStat,
imagery.*): which stats and flags a check reads, and how often, change
nothing. What a move does is compared whole: the returned bits, the
position, the motion fields (vel, accum, shovedir, the MoveTo, the sight
fields a blocked step clears), and the SetPos calls and map queries in
order.
"""
from __future__ import annotations

import itertools
from pathlib import Path

from combat_targets import _toward, compare, finish, port_fields

PURE = ('GetObjStat', 'GetStat', 'imagery.')

ACTION_ANIMATE, ACTION_MOVE, ACTION_COMBAT, ACTION_COMBATMOVE = 1, 2, 3, 4
OF_IMMOBILE, OF_INVISIBLE, OF_PARALIZE = 0x1, 0x80, 0x800000
AF_FLY = 0x800

BASE = 100                      # the ground's height (0 is no walkmap)
AT = [4000, 4000, BASE]         # sector 3, 3; walk cell (250, 250)
STEP = 0x80000                  # 8 units, one substep


def compare_move(case: dict, retail: dict, port: dict) -> list[dict]:
    def strip(result):
        out = dict(result)
        out['seams'] = [s for s in result.get('seams', []) if not s.get('seam', '').startswith(PURE)]
        return out
    return compare(case, strip(retail), strip(port))


def _cell(x):
    """The walk cell holding world coordinate x (GetWalkHeight's)."""
    return (x + 8) >> 4


def _mover(name='Araknid', objclass=12, pos=AT, facing=64, radius=16, health=100, root=None, doing=None,
           states=None, **extra):
    spec = dict(name=name, type=extra.pop('type', name), **{'class': objclass}, pos=list(pos), facing=facing,
                moveangle=extra.pop('moveangle', facing), stats=dict(health=health),
                classstats=dict(radius=radius), chardata=extra.pop('chardata', {}),
                states=states or [dict(name='walk', aniflags=extra.pop('aniflags', 0))],
                root=root or dict(name='walk', action=ACTION_ANIMATE, angle=facing, moveangle=facing))
    if doing:
        spec['doing'] = doing
    spec.update(extra)
    return spec


def _walking(facing, **extra):
    """Doing a walk move at `facing` over the walk root."""
    return _mover(facing=facing, doing=dict(name='walk', action=ACTION_MOVE, angle=facing, moveangle=facing,
                                            turnrate=8), **extra)


def _wall_ahead(pos, angle, dist, height, width=6):
    """A box of walk cells `dist` ahead of pos along angle, `width` cells
    each way across, two cells deep."""
    x, y, _ = _toward(pos, angle, dist)
    gx, gy = _cell(x), _cell(y)
    return [gx - width, gy - width, gx + width, gy + width, height] if angle % 64 else \
        ([gx, gy - width, gx + 1, gy + width, height] if angle in (64, 192) else
         [gx - width, gy, gx + width, gy + 1, height])


def move_cases(data: Path, workdir: Path) -> list[dict]:
    cases = []

    def case(name, chars, ground=None, **extra):
        cases.append(dict(name=f'move.{name}', call='move', globals=dict(combatface=1, frame=100),
                          self=chars[0]['name'], chars=chars, ground=ground or dict(z=BASE), **extra))

    # Nothing to move: no motion and no velocity (accum and shovedir reset),
    # or a gate before anything is read.
    case('idle', [_walking(64)])
    case('idle.accum-shovedir', [_walking(64, accum=[0x7000, -0x3000, 0], shovedir=32)])
    for label, extra in (('immobile', dict(objflags=OF_IMMOBILE)), ('paralized', dict(objflags=OF_PARALIZE)),
                         ('in-inventory', dict(inventnum=3)), ('forcenomove', dict(forcenomove=1))):
        case(f'gate.{label}', [_walking(64, movedist=STEP, **extra)])

    # The ground under the mover: up at once from below, a drop at once (with
    # gravity on vel.z, to the terminal speed) from above.
    for dz, velz, dist in itertools.product((-40, -17, -16, -1, 0, 1, 16, 17, 40),
                                            (0, -0x100000, -0x2e0000), (0, STEP)):
        pos = [AT[0], AT[1], BASE + dz]
        case(f'ground.dz{dz}.vz{velz:#x}.d{dist:#x}', [_walking(64, pos=pos, vel=[0, 0, velz], movedist=dist)])

    # Free walking: every 8th angle, distances from a fraction to several
    # substeps (the nudge onto the target), with and without a carried
    # fraction, and a vertical part.
    for angle, dist, accum in itertools.product(range(0, 256, 8), (0x8000, 0x40000, STEP, 0x81000, 0x100000,
                                                                   0x1c0000), ((0, 0, 0), (0x8000, -0x7fff, 0x4000))):
        case(f'walk.a{angle}.d{dist:#x}.acc{int(any(accum))}',
             [_walking(angle, moveangle=angle, movedist=dist, accum=list(accum))])
    for vert in (0x8000, 0x100000, -0x100000):
        case(f'walk.vert{vert:#x}', [_walking(64, movedist=STEP, movevert=vert)])
    # Velocity alone, and with motion: a shove from a hit, a jump.
    for vel in ((0x40000, 0, 0), (-0x123456, 0x98765, 0), (0x81000, 0x81000, 0), (0, 0, 0x60000)):
        case(f'vel.{vel[0]:#x}.{vel[1]:#x}.{vel[2]:#x}', [_walking(64, vel=list(vel))])
        case(f'vel+walk.{vel[0]:#x}.{vel[1]:#x}.{vel[2]:#x}', [_walking(32, movedist=STEP, vel=list(vel))])

    # The ground in the way: a wall or a ledge ahead at a few distances,
    # heights around the 0x20 step, radii, straight on and oblique (shove).
    for angle, dist, rise, radius in itertools.product((64, 72, 96, 128), (10, 16, 22, 30), (0x10, 0x20, 0x21, 0x40),
                                                       (8, 16, 30)):
        ground = dict(z=BASE, cells=[_wall_ahead(AT, angle, dist, BASE + rise)])
        case(f'wall.a{angle}.at{dist}.rise{rise:#x}.r{radius}',
             [_walking(angle, movedist=0x100000, radius=radius)], ground)
    # A drop, a hole (no walkmap), no sector at all.
    for angle, dist in itertools.product((64, 96), (12, 20)):
        case(f'drop.a{angle}.at{dist}', [_walking(angle, movedist=0x100000)],
             dict(z=BASE, cells=[_wall_ahead(AT, angle, dist, BASE - 0x30)]))
        case(f'hole.a{angle}.at{dist}', [_walking(angle, movedist=0x100000)],
             dict(z=BASE, cells=[_wall_ahead(AT, angle, dist, 0, width=1)]))
    case('nosector', [_walking(64, movedist=0x100000)], dict(z=BASE, nosector=[[4, 3]], cells=[]),
         )
    # The shove: a wall two cells ahead of a small mover (clear at rest, hit
    # by the second substep), a way round on one side or neither, the side
    # already picked (shovedir); a carried fraction and a velocity (accum is
    # dropped, vel kept), oblique approaches.
    gx, gy = _cell(AT[0]) + 2, _cell(AT[1])
    for gap, shovedir, angle in itertools.product(('left', 'right', 'none', 'open'), (-1, -32, 32, -64, 64),
                                                  (64, 56, 80)):
        cells = [] if gap == 'open' else [[gx, gy - 1, gx + 1, gy + 1, BASE + 0x40]]
        if gap in ('right', 'none'):
            cells.append([gx - 3, gy - 6, gx + 1, gy - 2, BASE + 0x40])
        if gap in ('left', 'none'):
            cells.append([gx - 3, gy + 2, gx + 1, gy + 6, BASE + 0x40])
        if gap == 'open':
            cells.append([gx, gy - 8, gx + 1, gy + 8, BASE + 0x40])
        case(f'shove.{gap}.side{shovedir}.a{angle}',
             [_walking(angle, moveangle=angle, movedist=0x100000, shovedir=shovedir, radius=8)],
             dict(z=BASE, cells=cells))
    # Blocked on the first substep (exactly 8 units, one substep), so the
    # shove meets a velocity no clear substep has zeroed: it stays.
    for vy in (0x4000, -0x4000):
        case(f'shove.first-substep.vel{vy:#x}',
             [_walking(64, pos=[AT[0] - 8, AT[1], BASE], movedist=STEP, vel=[0, vy, 0], radius=8)],
             dict(z=BASE, cells=[[gx - 1, gy - 8, gx, gy + 8, BASE + 0x40]]))
    for extra in (dict(accum=[0x9000, -0x2000, 0]), dict(vel=[0x10000, 0x8000, 0]), dict(movedist=0x300000)):
        label = '-'.join(f'{k}' for k in extra)
        case(f'shove.wall.{label}', [_walking(64, **dict(dict(movedist=0x100000, radius=8), **extra))],
             dict(z=BASE, cells=[[gx, gy - 8, gx + 1, gy + 8, BASE + 0x40]]))

    # Characters in the way: in front at distances around touching, each
    # exemption, a combat root with a target (no shove), already overlapping.
    def blocker(dist, angle=64, name='Rat', objclass=12, **extra):
        return _mover(name=name, objclass=objclass, type=extra.pop('type', 'Araknid'),
                      pos=_toward(AT, angle, dist), facing=192, **extra)
    for dist, radius in itertools.product((20, 30, 33, 34, 40, 48), (8, 16)):
        case(f'char.at{dist}.r{radius}', [_walking(64, movedist=0x100000, radius=radius), blocker(dist)])
    for label, extra in (('dead', dict(health=0)), ('invisible', dict(objflags=OF_INVISIBLE)),
                         ('flying', dict(aniflags=AF_FLY)), ('moving-to', dict(moveto=[0, 0, BASE])),
                         ('player-idle', dict(objclass=11, type='Locke', name='Locke', playerstate=3)),
                         ('player-awake', dict(objclass=11, type='Locke', name='Locke', playerstate=1))):
        case(f'char.{label}', [_walking(64, movedist=0x100000), blocker(40, **extra)])
    case('char.self-flying', [_walking(64, movedist=0x100000, aniflags=AF_FLY), blocker(40)])
    case('char.self-invisible', [_walking(64, movedist=0x100000, objflags=OF_INVISIBLE), blocker(40)])
    case('char.self-dead', [_walking(64, movedist=0x100000, health=0), blocker(40)])
    case('char.overlapping', [_walking(64, movedist=STEP), blocker(10)])
    case('char.not-nearby', [_walking(64, movedist=0x100000), blocker(40)], nearby=['Araknid'])
    case('char.second-of-two', [_walking(64, movedist=0x100000), blocker(40, name='Far', angle=0, health=0),
                                blocker(40)])
    # Standing in one character, stepping into another (the map gives the
    # one ahead first): both checks find someone, so the step goes through.
    for order in (['Rat', 'Mole'], ['Mole', 'Rat']):
        case(f'char.in-one-into-another.{order[0]}-first',
             [_walking(64, movedist=0x100000), blocker(40), blocker(12, angle=192, name='Mole')],
             nearby=['Araknid'] + order)
    for side in (32, 96):
        case(f'char.oblique.a{side}', [_walking(side, moveangle=side, movedist=0x100000, radius=8),
                                       blocker(26, angle=side)])
    for dist in (40, 48):
        fighter = _mover(facing=64, root=dict(name='combat', action=ACTION_COMBAT, angle=64, moveangle=64, obj='Rat'),
                         doing=dict(name='combatf', action=ACTION_COMBATMOVE, angle=64, moveangle=64, turnrate=16,
                                    obj='Rat'), movedist=0x100000,
                         states=['combat', 'combatf'])
        case(f'char.combat-target.at{dist}', [fighter, blocker(dist)])
        case(f'wall.combat-target.at{dist}', [fighter, blocker(200, angle=0)],
             dict(z=BASE, cells=[_wall_ahead(AT, 64, dist - 10, BASE + 0x40)]))
    # A blocked step clears the sight fields (+0x254..+0x25c).
    case('blocked.sight', [_walking(64, movedist=0x100000, out_of_sight=1, out_of_sight_prev=1,
                                    sight_lost_ticks=5), blocker(40)])
    case('clear.sight', [_walking(64, movedist=STEP, out_of_sight=1, out_of_sight_prev=1, sight_lost_ticks=5)])

    # MoveTo: straight there (Move's up to ten MoveSteps), already there,
    # past a character (ignored), into a wall.
    for dx, dy in ((3, 0), (0, -40), (70, 30), (-200, 5), (0, 0)):
        case(f'moveto.{dx}.{dy}', [_walking(64, moveto=[AT[0] + dx, AT[1] + dy, BASE])])
    case('moveto.through-char', [_walking(64, moveto=[AT[0] + 60, AT[1], BASE]), blocker(30)])
    case('moveto.into-wall', [_walking(64, moveto=[AT[0] + 60, AT[1], BASE])],
         dict(z=BASE, cells=[_wall_ahead(AT, 64, 30, BASE + 0x40)]))

    # The walk speeds (none in the shipped char.def; the code is there):
    # combat steps, walk / sneak / run roots, a pivot first.
    speeds = dict(walkspeed=4, runspeed=12, sneakspeed=2, combatwalkspeed=6)
    for root, action, flags in (('combat', ACTION_COMBATMOVE, []), ('combat', ACTION_COMBATMOVE, ['waitpivot']),
                                ('walk', ACTION_MOVE, []), ('walk', ACTION_MOVE, ['waitpivot']),
                                ('sneak', ACTION_MOVE, []), ('run', ACTION_MOVE, [])):
        for chardata in (speeds, dict(speeds, walkspeed=0)):
            r = dict(name=root, action=ACTION_COMBAT if root == 'combat' else ACTION_ANIMATE, angle=40, moveangle=40)
            d = dict(name=f'{root}f', action=action, angle=40, moveangle=40, turnrate=8, flags=flags)
            for who, objclass in (('Araknid', 12), ('Locke', 11)):
                case(f'speed.{who}.{root}.{action}.{"-".join(flags) or "plain"}.walk{chardata["walkspeed"]}',
                     [_mover(name=who, objclass=objclass, facing=40, root=r, doing=d, chardata=chardata,
                             movedist=0x30000, states=[root, f'{root}f'])])
    return finish(cases)


TARGETS = {
    'combat-move': dict(fixture='slots/combat/combat_call.py', cases=move_cases, compare=compare_move,
                        port_fields=port_fields, unit=lambda r: 1),
}
