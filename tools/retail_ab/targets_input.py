"""Kata M6 for retail_ab.py: the player's held controls, one tick
(docs/gameplay/COMBAT_DOJO.md).

`combat-input` runs TPlayScreen::UpdateMove `0x47de30` with the case's held
controls (the control map's state and changed masks): the direction (the
lowest held bit, 32 a step), Leap in a combat root, Go -- unless a move
already heads that way -- and Stop with nothing held after a change (the
player's Stop lets go of every control and of the right-button walk).
Block and the bow aim are left out (their own tracks; a case reaching them
shows as a seam on the retail side only).
"""
from __future__ import annotations

import itertools
from pathlib import Path

from combat_targets import FULL_STATES, _char, _toward, finish, port_fields
from targets_move import compare_move

UPRIGHT, RIGHT, DOWNRIGHT, DOWN, DOWNLEFT, LEFT, UPLEFT, UP = (1 << i for i in range(8))
RUN, SNEAK, LEAP = 0x100, 0x200, 0x400
WALK_STATES = ['walk', 'walkf', 'walkl', 'walkr', 'walk to walkf', 'pivotl']
LEAP_STATES = [f'combatleap{s}' for s in ('f', 'fr', 'r', 'br', 'b', 'bl', 'l', 'fl')]


def input_cases(data: Path, workdir: Path) -> list[dict]:
    me_at = [1000, 1000, 0]
    cases = []

    def case(name, me, controls, others=()):
        cases.append(dict(name=f'input.{name}', call='update-move', controls=controls,
                          globals=dict(combatface=1, frame=0x40), self='Locke', chars=[me, *others]))

    def walker(doing='root', moveangle=64, **extra):
        spec = _char('Locke', 11, me_at, 64, states=WALK_STATES, type='Locke', **extra)
        spec['root'] = dict(name='walk', action=1, angle=64, moveangle=64)
        if doing == 'walk':
            spec['doing'] = dict(name='walkf', action=2, angle=moveangle, moveangle=moveangle, turnrate=8)
        elif doing == 'goto':
            spec['doing'] = dict(name='walkf', action=2, angle=moveangle, moveangle=moveangle, turnrate=8,
                                 target=[1200, 1000, 0])
        elif doing == 'pivot':
            spec['doing'] = dict(name='pivotl', action=0x11, angle=moveangle, moveangle=moveangle, turnrate=8)
        return spec

    # Walking: every direction bit alone, the lowest of two, nothing held;
    # standing, already walking that way (no new Go), walking another way,
    # a Goto walk, a pivot; changed or not.
    # (Two adjacent arrows, Up + Right say, the port walks as their diagonal
    # -- a REVSYNC-DIVERGENCE for keyboards with no numpad; retail takes the
    # lowest bit. The combinations here are ones that rule doesn't touch.)
    dirs = [(f'd{i}', 1 << i) for i in range(8)] + [('two', RIGHT | LEFT), ('three', DOWN | LEFT | UPRIGHT),
                                                    ('none', 0)]
    for (dname, bits), doing, held_way, changed in itertools.product(
            dirs, ('root', 'walk', 'goto', 'pivot'), (0, 32, 64), (0, 1)):
        if doing == 'root' and held_way:
            continue
        me = walker(doing, moveangle=held_way)
        case(f'walk.{dname}.{doing}.m{held_way}.ch{changed}', me,
             dict(state=bits | (RUN if dname == 'd3' else 0), changed=changed and 0x100))
    # Combat: strafing (a step already heading the held way, or not), leap,
    # stop; a target to face.
    target = _char('Target', 12, _toward(me_at, 64, 100), 192, type='Araknid', states=['combat'])
    for (dname, bits), doing, leap, changed in itertools.product(
            [(f'd{i}', 1 << i) for i in range(0, 8, 2)] + [('none', 0)], ('root', 'step0', 'step64'),
            (0, LEAP), (0, 1)):
        me = _char('Locke', 11, me_at, 64, root_obj='Target', states=FULL_STATES + LEAP_STATES, type='Locke')
        if doing != 'root':
            m = int(doing[4:])
            me['doing'] = dict(name='combatr' if m == 64 else 'combatf', action=4, angle=64, moveangle=m,
                               turnrate=16, obj='Target', flags=['noroot'])
        case(f'combat.{dname}.{doing}.leap{int(bool(leap))}.ch{changed}', me,
             dict(state=bits | leap, changed=changed), [target])
    return finish(cases)


TARGETS = {
    'combat-input': dict(fixture='slots/combat/combat_call.py', cases=input_cases, compare=compare_move,
                         port_fields=port_fields, unit=lambda r: 1),
}
