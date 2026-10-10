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

    # Leap: every 16th angle off two facings (the octant table), armed and
    # bare-handed (the root's name is the prefix), a player's idle mark,
    # refused out of a combat root, dead, or without the animation.
    leaps = [f'{root}leap{s}' for root in ('combat', 'hand')
             for s in ('f', 'fr', 'r', 'br', 'b', 'bl', 'l', 'fl')]
    for who, objclass, root, facing, angle in itertools.product(
            ('Araknid', 'Locke'), (12, 11), ('combat', 'hand'), (0, 70), range(0, 256, 16)):
        if (who == 'Locke') != (objclass == 11):
            continue
        me = _char(who, objclass, me_at, facing, root_obj='Target', states=FULL_STATES + ['hand'] + leaps)
        me['root']['name'] = root
        case(f'leap.{who}.{root}.f{facing}.a{angle}', me, call='leap', angle=angle)
    for label, extra, states in (('walk-root', dict(root=dict(name='walk', action=1)), FULL_STATES + leaps),
                                 ('dead', dict(health=0), FULL_STATES + leaps),
                                 ('no-anim', {}, FULL_STATES),
                                 ('player-idle', dict(playerstate=3), FULL_STATES + leaps),
                                 ('interactive-move', dict(charflags=0x80000), FULL_STATES + leaps)):
        me = _char('Locke', 11, me_at, 0, root_obj='Target', states=states,
                   **{k: v for k, v in extra.items() if k != 'root'})
        if 'root' in extra:
            me['root'] = extra['root']
        case(f'leap.refuse.{label}', me, call='leap', angle=96)

    # StartRetreat: standing or walking, already retreating or not, with a
    # target (the fight is dropped), a player (Stop lets go of his controls).
    for who, objclass, doing, already in itertools.product(('Araknid', 'Locke'), (12, 11), ('root', 'step'),
                                                           (0, 1)):
        if (who == 'Locke') != (objclass == 11):
            continue
        me = _char(who, objclass, me_at, 64, root_obj='Target', states=FULL_STATES, retreating=already,
                   retreat_latch=already, retreat_frames=5 if already else 0)
        if doing == 'step':
            me['doing'] = dict(name='combatf', action=4, angle=64, moveangle=64, turnrate=16, obj='Target',
                               flags=['noroot'])
        foe = _char('Foe', 12, _toward(me_at, 64, 100), 192, type='Araknid', states=['combat'])
        me['root']['obj'] = 'Foe'
        if doing == 'step':
            me['doing']['obj'] = 'Foe'
        cases.append(dict(name=f'steps.retreat.{who}.{doing}.already{already}', call='start-retreat',
                          globals=dict(combatface=1, frame=0x40), self=who, chars=[me, foe]))

    # KnockBack: the blow's bearing all round two facings (the back impact
    # past 0x48 off the facing, measured unwrapped: a facing of 250 sees a
    # blow at 10 as from behind), each variant and the random pick (out of
    # range too), with and without the back impact, a player (the prefix),
    # an idle player, a paralysed character.
    impacts = ['impk', 'imphh', 'imph', 'implh', 'impl']
    for who, objclass, facing, bearing, variant, has_back in itertools.product(
            ('Araknid', 'Locke'), (12, 11), (0, 250), range(0, 256, 32), (-1, 0, 3, 7), (1, 0)):
        if (who == 'Locke') != (objclass == 11):
            continue
        prefix = 'c'
        states = FULL_STATES + [prefix + n for n in impacts] + ([prefix + 'impb'] if has_back else [])
        me = _char(who, objclass, me_at, facing, root_obj='Target', states=states)
        frm = _toward(me_at, bearing, 80)
        tape = [[1], [4], [32767], [8]][(facing // 2 + bearing // 32) % 4] if variant in (-1, 7) else []
        case(f'knock.{who}.f{facing}.b{bearing}.v{variant}.back{has_back}', me, call='knockback', **{'from': frm},
             variant=variant, tape=tape)
    for label, extra in (('player-idle', dict(playerstate=2)), ('paralysed', dict(objflags=0x800000)),
                         ('no-anim', dict(states=FULL_STATES))):
        states = extra.pop('states', FULL_STATES + ['c' + n for n in impacts])
        me = _char('Locke', 11, me_at, 0, root_obj='Target', states=states, **extra)
        case(f'knock.refuse.{label}', me, call='knockback', **{'from': _toward(me_at, 20, 80)}, variant=1)
    return finish(cases)


TARGETS = {
    'combat-steps': dict(fixture='slots/combat/combat_call.py', cases=steps_cases, compare=compare_move,
                         port_fields=port_fields, unit=lambda r: 1),
}
