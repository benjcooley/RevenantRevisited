"""Kata M8 for retail_ab.py: game ticks in a row (docs/gameplay/COMBAT_DOJO.md).

`combat-sequence` runs `ticks` ticks of retail's order (TimerTick
`0x490bd0`): the held direction (Go), then for every character
TComplexObject::Pulse (UpdateAction), Move, SetObjectMotion, the game frame,
NextFrame -- with the animation layer as original code: SetState
`0x46f250` / ResetState, T3DImagery::SetObjectMotion `0x40cd20` over the
case's motion tables (one SMotionData a frame), NextFrame `0x470cc0`, and a
recording stand-in animator. Each tick is compared whole (position,
facing, move angle and distance, accum, state, frame and rate, blocks by
meaning, the state changes and animator calls); the pure queries are left
out of the seams as in M7.

The motion tables are synthetic but shaped as the game's: a walk cycle and
combat steps whose per-frame `ang` turns the motion off the facing (a side
step's 64), a looping root, transitions, a non-looping step that ends and
alternates.
"""
from __future__ import annotations

import itertools
import json
from pathlib import Path

from combat_targets import _toward, _walk, finish, port_fields
from targets_move import AT, BASE, PURE

ANIMATE, MOVE, COMBAT, COMBATMOVE = 1, 2, 3, 4
AF_LOOPING, AF_PINGPONG, AF_REVERSE, AF_ROOT, AF_NOMOTION = 0x1, 0x80, 0x100, 0x400, 0x2000


def compare_sequence(case: dict, retail: dict, port: dict) -> list[dict]:
    """Tick by tick: each tick's record compared whole (pure queries out of
    its seams), then the draws; a difference names its tick."""
    def strip(tick):
        out = dict(tick)
        out['seams'] = [s for s in tick.get('seams', []) if not s.get('seam', '').startswith(PURE)]
        return out
    out = []
    rt, pt = retail.get('ticks', []), port.get('ticks', [])
    if len(rt) != len(pt):
        out.append(dict(where='ticks', line=None, field='tick count', retail=len(rt), port=len(pt)))
    for i, (r, p) in enumerate(zip(rt, pt)):
        _walk(strip(r), strip(p), f't{i}', out)
    _walk({'draws': retail.get('draws')}, {'draws': port.get('draws')}, '', out)
    return out


def _cycle(frames, dist, ang=0, vert=0, rotz=0):
    """A step's motion: `dist` (SMotionData units, << 8 = 1/0x10000 world
    units) every frame, at `ang` off the facing."""
    return [[dist, vert, ang, 0, 0, rotz] for _ in range(frames)]


def walk_states(step_frames=8, dist=0x500):
    return [dict(name='walk', frames=12, aniflags=AF_LOOPING | AF_ROOT),
            dict(name='walkf', frames=step_frames, motion=_cycle(step_frames, dist)),
            dict(name='walkl', frames=step_frames, motion=_cycle(step_frames, dist)),
            dict(name='walkr', frames=step_frames, motion=_cycle(step_frames, dist)),
            dict(name='walk to walkf', frames=4, motion=_cycle(4, dist // 2)),
            dict(name='walkf to walk', frames=4, motion=_cycle(4, dist // 2))]


def combat_states(step_frames=6, dist=0x400):
    states = [dict(name='combat', frames=10, aniflags=AF_LOOPING | AF_ROOT)]
    for sfx, ang in (('f', 0), ('fr', 32), ('r', 64), ('br', 96), ('b', 128), ('bl', 160), ('l', 192), ('fl', 224)):
        states.append(dict(name=f'combat{sfx}', frames=step_frames, motion=_cycle(step_frames, dist, ang)))
        states.append(dict(name=f'combat to combat{sfx}', frames=3, motion=_cycle(3, dist // 2, ang)))
    return states


def _char(name, objclass, pos, facing, states, root, doing='root', **extra):
    spec = dict(name=name, type=extra.pop('type', name), **{'class': objclass}, pos=list(pos), facing=facing,
                moveangle=facing, stats=dict(health=100, sleeping=0), classstats=dict(radius=16),
                chardata=dict(combatrangemax=300), states=states, animator=1, root=root, doing=doing,
                desired='root', state=0)
    spec.update(extra)
    return spec


def sequence_cases(data: Path, workdir: Path) -> list[dict]:
    cases = []

    def case(name, chars, ticks, inputs=(), ground=None, **extra):
        cases.append(dict(name=f'seq.{name}', call='sequence', ticks=ticks, inputs=list(inputs),
                          globals=dict(combatface=1, frame=extra.pop('frame', 0x40)),
                          self=chars[0]['name'], chars=chars, ground=ground or dict(z=BASE), **extra))

    walk_root = dict(name='walk', action=ANIMATE, angle=64, moveangle=64)
    # Standing: the looping root plays on (and wraps), nothing moves.
    case('stand.walk', [_char('Araknid', 12, AT, 64, walk_states(), walk_root)], 30)
    # Walking: Go every tick (a held direction), straight and turning.
    for angle, ticks in ((64, 40), (100, 40), (0, 24), (200, 48)):
        case(f'walk.hold{angle}', [_char('Araknid', 12, AT, 64, walk_states(), walk_root)], ticks,
             [dict(go=angle)] * ticks)
    # Let go after a while: the step ends, back to the root.
    case('walk.release', [_char('Araknid', 12, AT, 64, walk_states(), walk_root)], 40, [dict(go=64)] * 12)
    # A wall ahead: blocked, the bounce.
    gx, gy = (AT[0] + 8 + 40) >> 4, (AT[1] + 8) >> 4
    case('walk.into-wall', [_char('Araknid', 12, AT, 64, walk_states(), walk_root)], 40, [dict(go=64)] * 40,
         ground=dict(z=BASE, cells=[[gx, gy - 6, gx + 1, gy + 6, BASE + 0x40]]))

    # Combat: the orbit. A player and a monster, a target at 100, holding
    # each direction (strafing round it, closing, backing off), CombatFace
    # on and off.
    for who, objclass, face_on, hold in itertools.product(('Locke', 'Araknid'), (11, 12), (1, 0),
                                                         (64, 192, 0, 128, 32)):
        if (who == 'Locke') != (objclass == 11):
            continue
        tclass, ttype = (12, 'Araknid') if objclass == 11 else (11, 'Locke')
        target = _char('Target', tclass, _toward(AT, 64, 100), 192, [dict(name='combat', frames=10,
                       aniflags=AF_LOOPING | AF_ROOT)], dict(name='combat', action=COMBAT, angle=192, moveangle=192),
                       type=ttype)
        me = _char(who, objclass, AT, 64, combat_states(),
                   dict(name='combat', action=COMBAT, angle=64, moveangle=64, obj='Target'))
        cases.append(dict(name=f'seq.orbit.{who}.cf{face_on}.hold{hold}', call='sequence', ticks=48,
                          inputs=[dict(go=hold)] * 48, globals=dict(combatface=face_on, frame=0x40),
                          self=who, chars=[me, target], ground=dict(z=BASE)))
    # The orbit let go: the step finishes, the root again.
    me = _char('Locke', 11, AT, 64, combat_states(), dict(name='combat', action=COMBAT, angle=64, moveangle=64,
                                                          obj='Target'))
    target = _char('Target', 12, _toward(AT, 64, 100), 192, [dict(name='combat', frames=10,
                   aniflags=AF_LOOPING | AF_ROOT)], dict(name='combat', action=COMBAT, angle=192, moveangle=192),
                   type='Araknid')
    case('orbit.release', [me, target], 40, [dict(go=64)] * 10)
    # Animation edges: a reversed root, a ping-pong root, no motion.
    for label, flags in (('reverse', AF_LOOPING | AF_ROOT | AF_REVERSE), ('pingpong', AF_PINGPONG | AF_ROOT),
                         ('once', AF_ROOT), ('nomotion', AF_LOOPING | AF_ROOT | AF_NOMOTION)):
        states = walk_states()
        states[0] = dict(name='walk', frames=7, aniflags=flags, motion=_cycle(7, 0x100))
        case(f'anim.{label}', [_char('Araknid', 12, AT, 64, states, walk_root)], 24)

    # The game's own motion (tools/retail_ab/cases/motion/, i3ddump): Locke
    # in combat round an Araknid, each direction held for three seconds,
    # armed and bare-handed, CombatFace on and off; walking; the Araknid
    # stepping in and backing off.
    locke, araknid = (_real_states(n) for n in ('locke', 'araknid'))
    foe = _char('Araknid', 12, _toward(AT, 64, 100), 192, araknid,
                dict(name='combat', action=COMBAT, angle=192, moveangle=192))
    for root, cf, hold in itertools.product(('combat', 'hand'), (1, 0), range(0, 256, 32)):
        me = _char('Locke', 11, AT, 64, locke, dict(name=root, action=COMBAT, angle=64, moveangle=64, obj='Araknid'))
        cases.append(dict(name=f'seq.real.{root}.cf{cf}.hold{hold}', call='sequence', ticks=72,
                          inputs=[dict(go=hold)] * 72, globals=dict(combatface=cf, frame=0x40),
                          self='Locke', chars=[me, foe], ground=dict(z=BASE)))
    for hold in (64, 100, 220):
        me = _char('Locke', 11, AT, 64, locke, dict(name='walk', action=ANIMATE, angle=64, moveangle=64))
        case(f'real.walk.hold{hold}', [me], 60, [dict(go=hold)] * 60)
    for hold, ticks in ((64, 60), (192, 40)):
        me = _char('Araknid', 12, AT, 64, araknid, dict(name='combat', action=COMBAT, angle=64, moveangle=64,
                                                        obj='Locke'))
        loc = _char('Locke', 11, _toward(AT, 64, 120), 192, locke, dict(name='combat', action=COMBAT, angle=192,
                                                                        moveangle=192), type='Locke')
        case(f'real.araknid.hold{hold}', [me, loc], ticks, [dict(go=hold)] * ticks)
    return finish(cases)


def _real_states(name):
    path = Path(__file__).resolve().parent / 'cases' / 'motion' / f'{name}.json'
    return json.loads(path.read_text())['states']


TARGETS = {
    'combat-sequence': dict(fixture='slots/combat/combat_call.py', cases=sequence_cases, compare=compare_sequence,
                            port_fields=port_fields, unit=lambda r: len(r.get('ticks', []))),
}
