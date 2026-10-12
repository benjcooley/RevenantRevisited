"""Katas M11 / M12 for retail_ab.py: the map queries combat leans on
(docs/gameplay/forensics/MAP_QUERIES.md).

`map-los` runs TMapPane::LineOfSight `0x4533d0` over a stand-in walkmap (the
case's `ground`: a floor, boxes of cells at other heights, holes, missing
sectors, the level its sectors are on). Every cell the walk reads is
recorded on both sides, so the walk is compared step by step, with the
result and, in half the cases, both out-points. Cases: every octant and the
axes and diagonals at lengths from the same cell to 160 cells; walls across,
beside and at the ends of the line; a cell exactly at the line's height and
one above; ramps (both ends at different heights) with a bump at the
interpolated height; ledges looked down from and up at; holes, missing
sectors, another level; points on cell edges; negative coordinates; height
differences past 15 bits (the fixed point's wrap); and a seeded sweep of
random lines over random boxes.

`map-find-objects` runs TMapPane::FindObjectsInRange `0x452060` with the map
iterator as the seam: it hands out the case's `objects` (characters, whose
Radius goes through the GetStat seam, and stand-in items) in the case's
order, those in the rect it was given on its level. Cases: the sphere's
edge (centre to centre, height included) on the axes and diagonals; the
radius term for character and player searches (and none for any-class
searches); the half-open rect's clipping; the box's edges, even and odd;
inventory objects; classes; levels (-1 is the map pane's); the order (ties,
insertion, a full list dropping its farthest, maxnum 0, 1, past 64); and a
seeded sweep of random worlds.
"""
from __future__ import annotations

import itertools
import math
import random
from pathlib import Path

from combat_targets import compare, finish, port_fields
from targets_move import compare_move
from targets_perceive import SEE, _who

CELL = 16
FLOOR = 100
EYE = 50
AT = [3200, 3200, FLOOR + EYE]          # walk cell (200, 200), sector (3, 3)


def _cell(v):
    return (v + 8) >> 4


def _point(origin, angle, dist, z=None):
    a = angle * 2 * math.pi / 256
    return [origin[0] + round(dist * math.sin(a)), origin[1] - round(dist * math.cos(a)),
            origin[2] if z is None else z]


def _box(x, y, height, half=0):
    """The cells within `half` of the cell holding world (x, y)."""
    cx, cy = _cell(x), _cell(y)
    return [cx - half, cy - half, cx + half, cy + half, height]


def los_cases(data: Path, workdir: Path) -> list[dict]:
    cases = []
    n = 0

    def case(name, frm, to, *, z=FLOOR, cells=(), nosector=(), ground_level=None, level=0):
        nonlocal n
        n += 1
        ground = dict(z=z, cells=[list(c) for c in cells], nosector=[list(s) for s in nosector], record=True)
        if ground_level is not None:
            ground['level'] = ground_level
        cases.append(dict(name=f'los.{name}', call='line-of-sight', ground=ground, level=level,
                          outputs=n % 2 == 0, **{'from': list(frm)}, to=list(to)))

    # Every direction (256ths: the octants, their edges, the axes and the
    # diagonals) at lengths from the same cell to 160 cells, on a flat floor
    # below the eyes: clear; with a wall halfway, blocked; with the wall
    # beside the line, clear.
    angles = sorted(set(range(0, 256, 8)) | {1, 31, 33, 63, 65, 95, 97, 127, 129, 191, 193, 255})
    for angle, dist in itertools.product(angles, (0, 7, 8, 16, 23, 40, 100, 333, 1000, 2560)):
        to = _point(AT, angle, dist)
        case(f'flat.a{angle}.d{dist}', AT, to)
        if dist >= 40:
            mid = _point(AT, angle, dist // 2)
            case(f'wall.a{angle}.d{dist}', AT, to, cells=[_box(mid[0], mid[1], FLOOR + 200, 1)])
            side = _point(mid, (angle + 64) & 0xff, 64)
            case(f'beside.a{angle}.d{dist}', AT, to, cells=[_box(side[0], side[1], FLOOR + 200, 1)])

    # A cell on the line at the line's height and one above it, level lines
    # and lines with the ends at different heights (a ramp: the cell's
    # height is the walk's interpolated height there, to the unit).
    for angle, (z0, z1), k in itertools.product((0, 21, 64, 100, 160, 224), ((150, 150), (100, 300), (300, 101),
                                                                            (150, 149), (0, 1000)), (1, 4, 9)):
        frm, to = list(AT[:2]) + [z0], _point(AT, angle, 160, z1)
        sx, sy = _cell(frm[0]), _cell(frm[1])
        dx, dy = _cell(to[0]) - sx, _cell(to[1]) - sy
        loop = max(abs(dx), abs(dy))
        if not loop:
            continue
        # The walk's own numbers for step k (retail's fixed point).
        def step(d, major):
            q = int((d << 16) / loop)
            return q if major else q + (1 if d >= 0 else -1)
        xmaj = abs(dx) >= abs(dy)
        fx, fy, fz = step(dx, xmaj), step(dy, not xmaj), step(z1 - z0, False)
        cx = ((sx << 16) + k * fx) >> 16
        cy = ((sy << 16) + k * fy) >> 16
        hz = ((z0 << 16) + k * fz) >> 16
        for bump in (hz, hz + 1, hz - 1):
            if bump <= 0:
                continue
            case(f'ramp.a{angle}.z{z0}-{z1}.k{k}.b{bump - hz:+d}', frm, to, z=1,
                 cells=[[cx, cy, cx, cy, bump]])

    # Ledges: looking down from a ledge's top over its edge, and up at it.
    for drop, back, eye in itertools.product((16, 31, 32, 100, 300), (8, 24, 40, 80), (EYE, 5)):
        top = FLOOR + drop
        ledge = [_cell(AT[0]) - 20, _cell(AT[1]) - 20, _cell(AT[0]), _cell(AT[1]) + 20, top]
        on = [AT[0] - back, AT[1], top + eye]
        below = [AT[0] + 200, AT[1], FLOOR + eye]
        case(f'ledge.down.d{drop}.b{back}.e{eye}', on, below, cells=[ledge])
        case(f'ledge.up.d{drop}.b{back}.e{eye}', below, on, cells=[ledge])

    # Steps along the way, each a cell higher than the last.
    for rise, eye in itertools.product((4, 16, 33), (EYE, 20)):
        cells = [[_cell(AT[0]) + 3 + 2 * i, _cell(AT[1]) - 5, _cell(AT[0]) + 4 + 2 * i, _cell(AT[1]) + 5,
                  FLOOR + rise * (i + 1)] for i in range(6)]
        for z1 in (FLOOR + eye, FLOOR + 6 * rise + eye):
            case(f'steps.r{rise}.e{eye}.z{z1}', AT, [AT[0] + 16 * 16, AT[1], z1], cells=cells)

    # Holes and missing sectors on the way, at the ends; another level.
    far = [AT[0] + 1600, AT[1] + 700, AT[2]]
    for where in (0, 1, 50, 99, 100):
        x = AT[0] + (far[0] - AT[0]) * where // 100
        y = AT[1] + (far[1] - AT[1]) * where // 100
        case(f'hole.at{where}', AT, far, cells=[_box(x, y, 0, 1)])
        case(f'nosector.at{where}', AT, far, nosector=[[_cell(x) >> 6, _cell(y) >> 6]])
    for gl, lv in ((0, 0), (2, 2), (2, 0), (0, 3)):
        case(f'level.ground{gl}.los{lv}', AT, far, ground_level=gl, level=lv)

    # The ends in the same cell, against the start's height; points on the
    # cell edges (x + 8 a multiple of 16 or one short of it).
    for dz, h in itertools.product((-60, 0, 60), (FLOOR, AT[2] - 1, AT[2], AT[2] + 1)):
        case(f'same.dz{dz}.h{h}', AT, [AT[0] + 5, AT[1] - 3, AT[2] + dz], cells=[_box(AT[0], AT[1], h)])
    for ox, oy in itertools.product((-9, -8, 7, 8), (-9, -8, 7, 8)):
        frm = [AT[0] + 16 * 3 + ox, AT[1] + oy, AT[2]]
        to = [AT[0] + 16 * 40 + oy, AT[1] + 16 * 7 + ox, AT[2]]
        case(f'edges.{ox}.{oy}', frm, to, cells=[_box(AT[0] + 16 * 20, AT[1] + 16 * 4, AT[2], 0)])

    # Negative coordinates (cells and sectors below zero), and height
    # differences past what 16.16 holds (the shift wraps alike).
    for frm, to in (([-40, 30, 150], [300, -500, 150]), ([-2000, -1000, 150], [-100, -3000, 150]),
                    ([8, 8, 150], [-8, -8, 150])):
        case(f'negative.{frm[0]}.{frm[1]}', frm, to)
    for dz in (32767, 32768, 40000, -32768, -32769, -70000):
        case(f'wrap.dz{dz}', [AT[0], AT[1], 150], [AT[0] + 16 * 9, AT[1] + 16 * 2, 150 + dz],
             cells=[_box(AT[0] + 16 * 4, AT[1] + 16, 120)])

    # A seeded sweep: random lines over random boxes and holes.
    rng = random.Random(0x10e)
    for i in range(600):
        frm = [AT[0] + rng.randint(-900, 900), AT[1] + rng.randint(-900, 900), rng.randint(60, 400)]
        to = [frm[0] + rng.randint(-1500, 1500), frm[1] + rng.randint(-1500, 1500), rng.randint(60, 400)]
        cells = []
        for _ in range(rng.randint(0, 8)):
            x, y = rng.randint(min(frm[0], to[0]), max(frm[0], to[0])), rng.randint(min(frm[1], to[1]),
                                                                                  max(frm[1], to[1]))
            cells.append(_box(x, y, rng.choice((0, rng.randint(1, 450))), rng.randint(0, 3)))
        case(f'sweep.{i}', frm, to, z=rng.choice((FLOOR, 60, 17)), cells=cells)
    return finish(cases)


# ---- M12 ------------------------------------------------------------------------

PLAYER, CHARACTER, HELPER, ITEM = 11, 12, 15, 2
CENTRE = [5000, 5000, 100]


def _char(name, ident, pos, *, objclass=CHARACTER, radius=16, level=0, inventnum=-1):
    return dict(name=name, id=ident, type='Locke' if objclass == PLAYER else 'Araknid', **{'class': objclass},
                pos=list(pos), facing=0, level=level, inventnum=inventnum, classstats=dict(radius=radius),
                stats=dict(health=10), root=dict(name='walk', action=1))


def _item(name, ident, pos, *, objclass=ITEM, level=0, inventnum=-1):
    return dict(name=name, id=ident, **{'class': objclass}, pos=list(pos), level=level, inventnum=inventnum)


def find_cases(data: Path, workdir: Path) -> list[dict]:
    cases = []

    def case(name, chars, items, *, order=None, pos=CENTRE, **query):
        objects = order if order is not None else [o['name'] for o in chars + items]
        query.setdefault('maxnum', 10)
        cases.append(dict(name=f'find.{name}', call='find-objects', chars=chars, items=items, objects=objects,
                          pos=list(pos), globals=dict(maplevel=0), **query))

    def at(dx, dy, dz=0):
        return [CENTRE[0] + dx, CENTRE[1] + dy, CENTRE[2] + dz]

    # The sphere's edge, any class (no radius taken off): on each axis and
    # diagonal, at width - 1, width, width + 1, and with a height
    # difference that pushes a near one out.
    for width in (1, 10, 60, 200, 250, 2000):
        items = []
        k = 0
        for (ux, uy), off in itertools.product(((1, 0), (-1, 0), (0, 1), (0, -1), (1, 1), (-1, -1), (1, -1)),
                                               (-1, 0, 1)):
            d = width + off
            if ux and uy:
                d = int((width + off) / math.sqrt(2)) + (1 if off > 0 else 0)
            k += 1
            items.append(_item(f'i{k}', 0x100 + k, at(ux * d, uy * d)))
        items.append(_item('high', 0x1ff, at(width // 2, 0, width)))
        items.append(_item('lowish', 0x1fe, at(0, width // 3, -(width // 2))))
        for objclass in (-1, ITEM):
            case(f'sphere.w{width}.c{objclass}', [], items, width=width, objclass=objclass, maxnum=40)

    # The radius term: character and player searches take (0.4 Radius)^2
    # off (an object a little past the width is found), an any-class search
    # takes nothing; the rect still cuts one exactly `width` east or south.
    for radius, objclass, width in itertools.product((0, 1, 5, 16, 25, 40, 100), (CHARACTER, PLAYER, -1),
                                                     (20, 60, 200)):
        chars = []
        for k, (dx, dy) in enumerate(((width, 0), (0, width), (-width, 0), (0, -width), (width + 1, 0),
                                      (-width - 2, 0), (0, -width - 6), (width - 1, 0), (-width - 20, 0))):
            cls = objclass if objclass != -1 else (CHARACTER if k % 2 else PLAYER)
            chars.append(_char(f'c{k}', 0x200 + k, at(dx, dy), objclass=cls, radius=radius))
        case(f'radius.r{radius}.c{objclass}.w{width}', chars, [], width=width, objclass=objclass, maxnum=12)

    # The box: its edges at +-width/2 and +-height/2, even and odd sizes
    # (the rect cuts the far edge of an even one), height ignored.
    for width, height in ((10, 10), (11, 11), (10, 7), (60, 61), (0x3200, 0x3200), (2, 1)):
        items = []
        k = 0
        for dx, dy in itertools.product((-(width >> 1) - 1, -(width >> 1), 0, (width >> 1), (width >> 1) + 1),
                                        (-(height >> 1) - 1, -(height >> 1), (height >> 1), (height >> 1) + 1)):
            k += 1
            items.append(_item(f'b{k}', 0x300 + k, at(dx, dy, 999 * (k % 3))))
        case(f'box.{width}x{height}', [], items, width=width, height=height, maxnum=30)

    # Classes, inventory slots, levels (-1: the map pane's, 0), objsets.
    mixed_chars = [_char('Locke', 0x400, at(30, 0), objclass=PLAYER), _char('Spider', 0x401, at(0, 40)),
                   _char('Packed', 0x402, at(10, 10), inventnum=3), _char('Upstairs', 0x403, at(5, 5), level=1)]
    mixed_items = [_item('Rock', 0x410, at(-20, 0)), _item('Waypoint', 0x411, at(0, -25), objclass=HELPER),
                   _item('Pouch', 0x412, at(1, 1), inventnum=0), _item('Torch', 0x413, at(2, 3), level=1),
                   _item('Trap', 0x414, at(-50, 50), objclass=9)]
    for objclass, level, objset in itertools.product((-1, PLAYER, CHARACTER, HELPER, ITEM, 9, 3),
                                                     (-1, 0, 1, 2), (0, 2)):
        case(f'mixed.c{objclass}.l{level}.s{objset}', mixed_chars, mixed_items, width=100, objclass=objclass,
             level=level, objset=objset)

    # The order: nearer ones found later go in front; equal distances, the
    # later one first; a full list drops its farthest; maxnum 0, -1, 1, 2,
    # 3, and past 64 (retail's allocated list).
    ring = [_item(f'r{k}', 0x500 + k, at(*d)) for k, d in enumerate(
        ((40, 0), (10, 0), (0, 40), (30, 0), (-10, 0), (0, 10), (-40, 0), (20, 20), (0, -30), (0, 30),
         (28, 28), (-30, 0)))]
    for maxnum in (-1, 0, 1, 2, 3, 5, 12, 13):
        for rev in (False, True):
            order = [o['name'] for o in (reversed(ring) if rev else ring)]
            case(f'order.m{maxnum}.{"rev" if rev else "fwd"}', [], ring, order=order, width=100, maxnum=maxnum)
            case(f'order.box.m{maxnum}.{"rev" if rev else "fwd"}', [], ring, order=order, width=100, height=90,
                 maxnum=maxnum)
    crowd = [_item(f'q{k}', 0x600 + k, at((k * 37) % 181 - 90, (k * 53) % 173 - 86)) for k in range(90)]
    for maxnum in (64, 65, 70, 100):
        case(f'crowd.m{maxnum}', [], crowd, width=200, maxnum=maxnum)
        case(f'crowd.box.m{maxnum}', [], crowd, width=150, height=120, maxnum=maxnum)

    # A seeded sweep: random worlds of characters and items, random queries.
    rng = random.Random(0x452060)
    for i in range(400):
        chars, items = [], []
        for k in range(rng.randint(0, 6)):
            chars.append(_char(f'c{k}', 0x700 + k, at(rng.randint(-300, 300), rng.randint(-300, 300),
                                                      rng.randint(-80, 80)),
                               objclass=rng.choice((CHARACTER, PLAYER)), radius=rng.choice((8, 16, 20, 33)),
                               level=rng.choice((0, 0, 0, 1)), inventnum=rng.choice((-1,) * 5 + (2,))))
        for k in range(rng.randint(0, 10)):
            items.append(_item(f'i{k}', 0x780 + k, at(rng.randint(-300, 300), rng.randint(-300, 300),
                                                      rng.randint(-80, 80)),
                               objclass=rng.choice((ITEM, HELPER, 9)), level=rng.choice((0, 0, 1)),
                               inventnum=rng.choice((-1,) * 6 + (0,))))
        order = [o['name'] for o in chars + items]
        rng.shuffle(order)
        box = rng.random() < 0.3
        case(f'sweep.{i}', chars, items, order=order, width=rng.randint(1, 400),
             height=rng.randint(1, 400) if box else 0,
             objclass=rng.choice((-1, -1, CHARACTER, PLAYER, ITEM, HELPER)), level=rng.choice((-1, 0, 1)),
             maxnum=rng.choice((1, 2, 3, 5, 10, 10, 20)), objset=rng.choice((0, 2)))
    return finish(cases)


def sight_cases(data: Path, workdir: Path) -> list[dict]:
    """CanSeeCharacter `0x4cd540` (kata M9b's call) with the line of sight
    as the real function over a floor (combat_call.py, a case with a
    `ground`): eye to eye, 50 above each, over walls a little under, level
    with and over the eyes, ledges either one stands on, pits, holes, a
    ground on another level; every bearing round the watcher. FindCharacters
    (its sight filter) over the same. Compared as kata M9b compares."""
    cases = []
    me_at = [3200, 3200, FLOOR]
    eye = FLOOR + EYE

    def add(name, chars, call, ground, **extra):
        cases.append(dict(name=f'sight.{name}', call=call, perception=True, ground=ground,
                          globals=dict(combatface=1, frame=5000), self=chars[0]['name'], chars=chars, **extra))

    def between(frm, to, height, half=1):
        mid = [(frm[0] + to[0]) // 2, (frm[1] + to[1]) // 2]
        return _box(mid[0], mid[1], height, half)

    for bearing, wall, dist in itertools.product(range(0, 256, 16), (None, eye - 1, eye, eye + 1, 0), (64, 200)):
        at = _point(me_at, bearing, dist)
        cells = [] if wall is None else [between(me_at, at, wall)]
        chars = [_who('Watcher', 12, me_at, bearing), _who('Hero', 11, at, 0, glimpse=100, playerstate=1)]
        add(f'wall.b{bearing}.h{wall}.d{dist}', chars, 'can-see', dict(z=FLOOR, cells=cells), target='Hero',
            angle=-1, tape=[0])
    # Ledges and pits: the one seen standing high or low, the edge in the way.
    for rise, back in itertools.product((-200, -60, 60, 200), (16, 48, 120)):
        at = [me_at[0] + 200, me_at[1], FLOOR + rise]
        ledge = [_cell(at[0]) - back // 16, _cell(at[1]) - 10, _cell(at[0]) + 10, _cell(at[1]) + 10, FLOOR + rise]
        chars = [_who('Watcher', 12, me_at, 64), _who('Hero', 11, at, 0, glimpse=100, playerstate=1)]
        add(f'ledge.r{rise}.b{back}', chars, 'can-see', dict(z=FLOOR, cells=[ledge]), target='Hero', angle=-1,
            tape=[0])
    # The ground on another level than theirs; FindCharacters asked for
    # the seen, with and without a wall, heard or not.
    far = _point(me_at, 64, 150)
    for level in (0, 1):
        chars = [_who('Watcher', 12, me_at, 64), _who('Hero', 11, far, 0, glimpse=100, playerstate=1)]
        add(f'level.g{level}', chars, 'can-see', dict(z=FLOOR, level=level), target='Hero', angle=-1, tape=[0])
    for wall, noise in itertools.product((None, eye + 10), (0, 50)):
        cells = [] if wall is None else [between(me_at, far, wall, 2)]
        chars = [_who('Watcher', 12, me_at, 64, chardata=dict(enemies='Hero')),
                 _who('Hero', 11, far, 0, glimpse=100, noise=noise, playerstate=1)]
        add(f'find.h{wall}.n{noise}', chars, 'find-characters', dict(z=FLOOR, cells=cells), max=3, range=-1,
            angle=-1, anglerange=0, flags=SEE, tape=[0, 0])
    return finish(cases)


TARGETS = {
    'combat-sight': dict(fixture='slots/combat/combat_call.py', cases=sight_cases, compare=compare_move,
                         port_fields=port_fields, unit=lambda r: 1),
    'map-los': dict(fixture='slots/combat/map_query.py', cases=los_cases, compare=compare,
                    port_fields=port_fields, unit=lambda r: 1),
    'map-find-objects': dict(fixture='slots/combat/map_query.py', cases=find_cases, compare=compare,
                             port_fields=port_fields, unit=lambda r: 1),
}
