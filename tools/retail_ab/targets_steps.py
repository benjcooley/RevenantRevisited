"""Kata M10 for retail_ab.py: the combat steps a character is told to take
(docs/gameplay/COMBAT_DOJO.md).

`combat-steps` calls one of them per case, as original code on the retail
side: SideStep `0x4d6220` (left, right, or the random pick with no side;
refused mid-sidestep, without the animation, or when the doing block has
priority). The step block is compared by meaning (its angle and move
angle, the flags), with the facing it sets and the draw it makes.
"""
from __future__ import annotations

import itertools
from pathlib import Path

from combat_targets import FULL_STATES, _char, _toward, finish, port_fields
from targets_move import compare_move

SIDESTEPS = ['sidestepl', 'sidestepr']


def steps_cases(data: Path, workdir: Path) -> list[dict]:
    me_at = [1000, 1000, 0]
    cases = []
    target = _char('Target', 11, _toward(me_at, 64, 100), 192, type='Locke', states=['combat'])

    def case(name, me, **extra):
        cases.append(dict(name=f'steps.{name}', globals=dict(combatface=1, frame=0x40), self=me['name'],
                          chars=[me, target], **extra))

    for who, objclass, dir_, facing, moveangle, doing_angle in itertools.product(
            ('Araknid', 'Locke'), (12, 11), ('l', 'r', ''), (64, 200), (64, 10, 250), (64, 100)):
        if (who == 'Locke') != (objclass == 11):
            continue
        me = _char(who, objclass, me_at, facing, root_obj='Target', states=FULL_STATES + SIDESTEPS,
                   moveangle=moveangle)
        me['root']['angle'] = doing_angle
        tape = [] if dir_ else [[0], [1], [32767]][(facing + moveangle) % 3]
        case(f'side.{who}.{dir_ or "rand"}.f{facing}.m{moveangle}.a{doing_angle}', me, call='sidestep', dir=dir_,
             tape=tape)
    # Refusals: already sidestepping (any case of the name), no animation,
    # the doing block has priority (SetDesired refuses and the block goes).
    for label, doing, states in (('mid-sidestep', dict(name='sidestepl', action=3), FULL_STATES + SIDESTEPS),
                                 ('mid-Sidestep-upper', dict(name='SideStepR', action=3), FULL_STATES + SIDESTEPS),
                                 ('sidestepper', dict(name='sidestepper', action=3), FULL_STATES + SIDESTEPS),
                                 ('no-anim', None, FULL_STATES),
                                 ('priority', dict(name='combatf', action=4, flags=['priority']),
                                  FULL_STATES + SIDESTEPS)):
        me = _char('Araknid', 12, me_at, 64, root_obj='Target', states=states)
        if doing:
            me['doing'] = dict(doing, obj='Target', angle=64, moveangle=64)
            me['desired'] = dict(name='combatl', action=4, angle=64, moveangle=192, obj='Target')
        for dir_ in ('l', ''):
            case(f'side.refuse.{label}.{dir_ or "rand"}', me, call='sidestep', dir=dir_, tape=[1])
    return finish(cases)


TARGETS = {
    'combat-steps': dict(fixture='slots/combat/combat_call.py', cases=steps_cases, compare=compare_move,
                         port_fields=port_fields, unit=lambda r: 1),
}
