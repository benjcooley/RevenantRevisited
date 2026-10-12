"""Kata M9 (Wander) for retail_ab.py: TCharacter::Wander `0x4c9790`
(docs/gameplay/forensics/COMBAT_ATTACK_CHOICE.md §3.15), in the tick.

`melee-wander` runs the character's Pulse (melee_attack.py's `pulse` call)
for a character with charflags `0x100000`, so Wander runs before the AI
(recorded and skipped, as the tick's UpdateAction is). Wander's callees run
as original -- Goto and Go, its waypoint pick `0x4c95e0` with
FindObjectsInRange over the case's helpers (the map iterator the seam),
WaypointReachable's line of sight over the case's `ground`, ProbeLedge
`0x4c9460`'s walk heights, FacingDiff, the random draws on the tape --
except SetWalkMode, SetRunMode, EndFighting and BeginFighting, which are
recorded (the port's are 1998 code with katas of their own).

Cases: the idle countdown; a run in a combat, hand or bow root with and
without an opponent (and roots named so in other actions); the combat
root with no opponent, and retreating with and without a run going (the
draw round 30); out of combat, the run burst at full fatigue (the draw at
100) and a run's end (fatigue round half its maximum and the draw round
50; a bow fight and the draw round 15; the last tick); the patrol: no
waypoint near, one, several (the pick by facing, the draw round 60, the
random pick), the type, the current and previous waypoints passed over,
a wall in the way, flat ground (no ledge behind), the search's 2000 edge
and its ten, another level, the arrival round 25 (height included), a
blocked first step; a player; a few ticks in a row.
"""
from __future__ import annotations

import itertools
from pathlib import Path

from combat_targets import compare, finish, port_fields
from targets_melee import NOW, case, pulse_char, shipped

WANDER = 0x100000
ANIMATE, COMBAT, BOW = 1, 3, 0x19
ME = [5000, 5000, 100]
FLOOR = 100
STATES = ['walk', 'walkf', 'walkl', 'walkr', 'walk to walkf', 'run', 'runf', 'combat', 'combatf', 'combatrun',
          'handrun', 'bowrun', 'cbow']


def _cell(v):
    return (v + 8) >> 4


# A plateau round the character at the floor's height, ground 60 below it
# past 300: ProbeLedge, stepping away from a waypoint far enough, finds the
# drop; on FLAT it never does.
PLATEAU = dict(z=FLOOR - 60, cells=[[_cell(ME[0]) - 19, _cell(ME[1]) - 19, _cell(ME[0]) + 19, _cell(ME[1]) + 19,
                                     FLOOR]])
FLAT = dict(z=FLOOR)


def _at(dx, dy, dz=0):
    return [ME[0] + dx, ME[1] + dy, ME[2] + dz]


def wander_cases(data: Path, workdir: Path) -> list[dict]:
    s = shipped(data, workdir)
    rules = s['rules']
    cases = []
    seed = 900

    def me(typ='Araknid', *, pos=None, root=None, stats=None, aistate=None, **kw):
        return pulse_char(data, 'Me', typ, pos=pos or ME, facing=kw.pop('facing', 64), states=STATES,
                          root=root or dict(name='walk', action=ANIMATE), stats=stats, charflags=WANDER,
                          aistate=dict(aistate or {}), **kw)

    def target(pos=None):
        return pulse_char(data, 'Target', 'Locke', pos=pos or _at(60, 0), facing=192, ident=0x20, states=['combat'],
                          root=dict(name='combat', action=COMBAT))

    def add(name, chars, *, ticks=1, tape=None, waypoints=(), ground=None, **kw):
        nonlocal seed
        seed += 1
        c = case(f'wander.{name}', 'pulse', chars, rules, args=dict(ticks=ticks), seed=seed * 7919 + 5,
                 tape=list(tape or []), frame=0x200, seam_wander=True, real_map=True,
                 waypoints=[dict(w) for w in waypoints], ground=ground or PLATEAU, **kw)
        c['globals'].update(gametime=NOW, screenframe=500, autobegincombat=1, pvp=0, editor=0, control=1, ps_5d8=0)
        cases.append(c)

    def full(typ='Araknid'):
        """The character's MaxFatigue (a monster's chardata; the player's case value)."""
        return 78 if typ == 'Locke' else s['chars'][typ]['fatigue']

    # ---- The idle countdown ----------------------------------------------------
    for idle, ticks in ((1, 1), (2, 1), (1000, 1), (2, 3), (1, 2)):
        add(f'idle.{idle}.t{ticks}', [me(aistate=dict(wanderidle=idle))], ticks=ticks, tape=[0] * 6)

    # ---- A run in a fighting root -----------------------------------------------
    roots = [('combat', COMBAT), ('combatrun', COMBAT), ('handrun', COMBAT), ('run', COMBAT), ('bowrun', COMBAT),
             ('bowrun', BOW), ('run', BOW), ('combatrun', BOW), ('cbow', BOW), ('run', ANIMATE), ('walk', ANIMATE),
             ('handrun', ANIMATE)]
    for (rname, action), opponent, tired in itertools.product(roots, (True, False), (False, True)):
        root = dict(name=rname, action=action, obj='Target' if opponent else None)
        stats = dict(fatigue=10) if tired else None
        add(f'fightrun.{rname}.{action}.opp{int(opponent)}.tired{int(tired)}',
            [me(root=root, stats=stats), target()], tape=[99, 20, 70])

    # ---- The combat root ---------------------------------------------------------
    for runticks, roll, retreating in itertools.product((0, 1, 2, 500), (29, 30, 99, 0), (0, 1)):
        root = dict(name='combat', action=COMBAT, obj='Target')
        add(f'combat.run{runticks}.roll{roll}.retreat{retreating}',
            [me(root=root, retreating=retreating, aistate=dict(runticks=runticks)), target()], tape=[roll, 777])
    add('combat.alone', [me(root=dict(name='combat', action=COMBAT))], tape=[5])
    add('combat.alone.t3', [me(root=dict(name='combat', action=COMBAT))], ticks=3, tape=[5])

    # ---- Out of combat: run bursts and their ends ------------------------------
    for typ, fatigue, runticks, roll in itertools.product(
            ('Araknid', 'Locke'), ('full', 'full-1', 'half', 'half-1', 'zero'), (0, 1, 2, 300), (99, 98, 49, 50)):
        top = full(typ)
        f = dict(full=top, **{'full-1': top - 1}, half=top // 2, **{'half-1': top // 2 - 1}, zero=0)[fatigue]
        add(f'run.{typ}.{fatigue}.run{runticks}.roll{roll}',
            [me(typ, stats=dict(fatigue=f), aistate=dict(runticks=runticks, wanderidle=0))], tape=[roll, 333, 14])
    # A bow fight while running: the draw round 15, after the fatigue one.
    for runticks, tired, rolls in itertools.product((1, 5), (False, True), ((14, 0), (15, 0), (49, 14), (50, 99),
                                                                            (49, 15))):
        root = dict(name='cbow', action=BOW, obj='Target')
        top = full()
        add(f'bowrun.run{runticks}.tired{int(tired)}.r{rolls[0]}-{rolls[1]}',
            [me(root=root, stats=dict(fatigue=top // 3 if tired else top), aistate=dict(runticks=runticks)),
             target()], tape=list(rolls))
    add('bowrun.alone', [me(root=dict(name='cbow', action=BOW), aistate=dict(runticks=4))], tape=[60, 60])

    # ---- The patrol ---------------------------------------------------------------
    def wp(name, dx, dy, dz=0, **kw):
        return dict(name=name, pos=_at(dx, dy, dz), **kw)

    tired = dict(fatigue=1)            # no run burst draw: the patrol's draws alone
    add('patrol.none', [me(stats=tired)])
    add('patrol.none.flat', [me(stats=tired)], ground=FLAT)
    for roll in (0, 58, 59, 60, 99):
        add(f'patrol.one.r{roll}', [me(stats=tired)], waypoints=[wp('w1', 200, 0)], tape=[roll, 3])
        add(f'patrol.one.flat.r{roll}', [me(stats=tired)], waypoints=[wp('w1', 200, 0)], tape=[roll, 3],
            ground=FLAT)
    # Several in reach, round the facing (64, east): the best by facing, else
    # a random one.
    ring = [wp('n', 0, -250), wp('ne', 180, -180), wp('e', 260, 10), wp('s', 0, 240), wp('w', -230, 0),
            wp('far', 1200, 0)]
    for facing, roll, pick in itertools.product((64, 0, 128, 200, 32), (10, 60), (0, 1, 4)):
        add(f'patrol.ring.f{facing}.r{roll}.p{pick}', [me(stats=tired, facing=facing)], waypoints=ring,
            tape=[roll, pick])
    # What the pick passes over: other types, the current and the previous
    # waypoint, one walled off, one on another level.
    kinds = dict(axis=[wp('a', 220, 0, type='Axis'), wp('w', 0, 220)],
                 current=[wp('w', 220, 0), wp('v', 0, 220)],
                 previous=[wp('w', 220, 0), wp('v', 0, 220)],
                 both=[wp('w', 220, 0), wp('v', 0, 220)],
                 wall=[wp('w', 220, 0), wp('v', 0, 220)],
                 level=[wp('w', 220, 0, level=1), wp('v', 0, 220)])
    for kind, wps in kinds.items():
        ai = dict(current=dict(lookat='v'), previous=dict(lastwaypoint='w'),
                  both=dict(lookat='v', lastwaypoint='w')).get(kind, {})
        ground = PLATEAU
        if kind == 'wall':
            ground = dict(PLATEAU, cells=PLATEAU['cells'] + [[_cell(ME[0]) + 6, _cell(ME[1]) - 2,
                                                              _cell(ME[0]) + 7, _cell(ME[1]) + 2, FLOOR + 80]])
        for roll in (0, 80):
            add(f'patrol.skip.{kind}.r{roll}', [me(stats=tired, aistate=ai)], waypoints=wps, ground=ground,
                tape=[roll, 1])
    # None kept: back to the previous waypoint (or idle without one).
    for prev in (None, 'w'):
        add(f'patrol.back.prev{prev}', [me(stats=tired, aistate=dict(lastwaypoint=prev))],
            waypoints=[wp('w', 220, 0)], ground=FLAT, tape=[5])
    # The search's edge (2000, centre to centre, height included) and its ten.
    for d, dz in itertools.product((1990, 1999, 2000, 2001), (0, 30)):
        far_ground = dict(z=FLOOR - 60, cells=[[0, 0, 1000, 1000, FLOOR]])     # one plateau over it all
        add(f'patrol.edge.d{d}.z{dz}', [me(stats=tired)], waypoints=[wp('w', d, 0, dz)], ground=far_ground,
            tape=[0])
    dozen = [wp(f'w{k}', 150 + 37 * k, (-1) ** k * 40 * k) for k in range(12)]
    for roll in (0, 99):
        add(f'patrol.dozen.r{roll}', [me(stats=tired)], waypoints=dozen, tape=[roll, 7])
    # Walking to the current waypoint, and the arrival (under 25 by the
    # octagonal distance, height included): the next pick.
    for dx, dy, dz in ((0, 0, 0), (20, 0, 0), (24, 0, 0), (25, 0, 0), (17, 17, 0), (16, 16, 0), (14, 14, 0),
                       (10, 0, 14), (10, 0, 15), (10, 0, 16), (300, 0, 0)):
        wps = [wp('cur', 200, 0), wp('next', -200, 50), wp('other', 0, -260)]
        add(f'patrol.goto.{dx}.{dy}.{dz}', [me(pos=[ME[0] + 200 - dx, ME[1] - dy, ME[2] - dz], stats=tired,
                                               aistate=dict(lookat='cur', lastwaypoint='other'))],
            waypoints=wps, tape=[10, 1])
    add('patrol.goto.blocked', [me(stats=tired, aistate=dict(lookat='cur'))], waypoints=[wp('cur', 200, 0)],
        blocked=True)
    # A waypoint closer than one radius: ProbeLedge takes no step.
    add('patrol.near', [me(stats=tired)], waypoints=[wp('w', 10, 0)], tape=[0])
    # ProbeLedge's step both ways (the ground round the plateau lower or
    # higher) and round its 32, the probe ending short of the edge, on it
    # (the last pair) or past it.
    for step, d in itertools.product((31, 32, 33, -31, -32, -33, 60, -60), (120, 180, 200, 240, 300)):
        ground = dict(z=FLOOR - step, cells=PLATEAU['cells'])
        add(f'probe.s{step}.d{d}', [me(stats=tired)], waypoints=[wp('w', d, 0)], ground=ground, tape=[0])
    # The probe's direction: a plateau that reaches far one way and ends
    # near the other, the waypoint on either side (only stepping away from
    # it finds the near edge).
    cx, cy = _cell(ME[0]), _cell(ME[1])
    for (name, box), (dx, dy) in itertools.product(
            (('east', [cx - 12, cy - 12, cx + 80, cy + 80]), ('west', [cx - 80, cy - 80, cx + 12, cy + 12])),
            ((300, 0), (-300, 0), (0, 300), (0, -300), (220, 220), (-220, -220))):
        ground = dict(z=FLOOR - 60, cells=[box + [FLOOR]])
        add(f'probe.side.{name}.{dx}.{dy}', [me(stats=tired)], waypoints=[wp('w', dx, dy)], ground=ground, tape=[0])

    # ---- A player, and a few ticks ------------------------------------------------
    add('player.patrol', [me('Locke', stats=dict(fatigue=78))], waypoints=ring, tape=[99, 120, 10, 2])
    add('player.combat.retreat', [me('Locke', root=dict(name='combat', action=COMBAT, obj='Target'),
                                     retreating=1), target()], tape=[31, 500])
    add('ticks.burst', [me()], waypoints=ring, ticks=6, tape=[99, 2, 10, 1, 50, 3, 60, 2])
    add('ticks.idle', [me(aistate=dict(wanderidle=2))], waypoints=ring, ticks=4, tape=[99, 2, 10, 1])
    add('ticks.run-out', [me(aistate=dict(runticks=3))], ticks=4, tape=[99, 0, 99, 0, 99])
    cases.extend(ai_waypoint_cases(data, workdir))
    return finish(cases)


def ai_waypoint_cases(data: Path, workdir: Path) -> list[dict]:
    """The AI's own waypoint search (C3b's `ai.move.waypoints` layouts) with
    the real map queries: FindObjectsInRange over the helpers round the
    target's last place (250, height included), WaypointReachable over a
    floor, with and without a wall."""
    from targets_melee import ME_AT as AI_AT, block, full_world
    cases = []
    lost = dict(me_doing=block('cwalkf', 4, obj='Target'), dist=150)
    x, y = AI_AT[0], AI_AT[1]
    layouts = (('none', []), ('one', [dict(name='w1', pos=[x + 100, y, FLOOR])]),
               ('axis', [dict(name='a1', pos=[x + 100, y, FLOOR], type='Axis')]),
               ('nearest', [dict(name='w1', pos=[900, 950, FLOOR]), dict(name='w2', pos=[700, 700, FLOOR]),
                            dict(name='w3', pos=[880, 940, FLOOR])]),
               ('edge', [dict(name='w1', pos=[900 + 249, 950, FLOOR]), dict(name='w2', pos=[900 - 250, 950, FLOOR]),
                         dict(name='w3', pos=[900, 950 + 250, FLOOR])]),
               ('allfar', [dict(name='w1', pos=[4000, 4000, FLOOR]), dict(name='w2', pos=[3000, 3000, FLOOR])]),
               ('eleven', [dict(name=f'w{i}', pos=[900 + 20 * i, 950, FLOOR]) for i in range(11)]))
    wall = [_cell(x) - 3, _cell(y) - 30, _cell(x) - 2, _cell(y) + 30, FLOOR + 40]
    for (label, wps), lastz, walled in itertools.product(layouts, (FLOOR, 0), (False, True)):
        stats = dict(aggressive=1, sleeping=0)
        chars, rules = full_world(data, 'Araknid', 'Locke', me_stats=stats, target_stats=dict(sleeping=0), **lost)
        for c in chars:
            c['pos'][2] = FLOOR
        chars[0]['aistate'] = dict(lastpos=[900, 950, lastz])
        chars[1]['noise'] = 0
        ground = dict(z=FLOOR, cells=[wall] if walled else [])
        c = case(f'aiwaypoint.{label}.z{lastz}.wall{int(walled)}', 'ai', chars, rules, args={}, seed=4243,
                 tape=[40, 7, 1, 2], ground=ground, real_map=True, sees=False, waypoints=wps)
        cases.append(c)
    return cases


TARGETS = {
    'melee-wander': dict(fixture='slots/combat/melee_attack.py', cases=wander_cases, compare=compare,
                         port_fields=port_fields, unit=lambda r: 1),
}
