"""Kata M1u for retail_ab.py: one UpdateAction per case (docs/gameplay/COMBAT_DOJO.md).

`combat-update` runs TCharacter::UpdateAction `0x4c3260` (slot 0x210) with
the last Move's bits: ResetStealthValues `0x4cdbb0` (a draw every tick off
the 24-frame beat), the sleep check, ResolveAction `0x4c3490` and the
resolver of the doing block -- ResolveMove `0x4c5e90` (pivot, blocked
bounce, Goto arrival, steps), ResolvePivot, ResolveSay, ResolveCombat /
ResolveCombatMove -- the fallback "done" rule, the fall, and TryCommand /
ForceCommand of the desired block, all as original. What it does is
compared whole: the blocks (by meaning), facing and move angle, the motion
and stealth fields, the state changes, the draws. The pure queries are left
out of the seams as in M7.
"""
from __future__ import annotations

import itertools
from pathlib import Path

from combat_targets import finish, port_fields
from targets_move import AT, compare_move

ANIMATE, MOVE, COMBAT, COMBATMOVE, SAY, PIVOT, SLEEP = 1, 2, 3, 4, 0x10, 0x11, 0x17
MOVED, BLOCKED, FALLING = 1, 2, 4

WALK_STATES = ['walk', 'walkf', 'walkl', 'walkr', 'walk to walkf', 'walkf to walk', 'fall', 'pivotl', 'run',
               'runf', 'runl', 'runr', 'sleep', 'say']
COMBAT_STATES = ['combat', 'combatf', 'combatl', 'combatr', 'combatb', 'combat to combatf', 'fall']


def _who(name='Araknid', objclass=12, facing=64, states=None, root=None, doing='root', desired='root', **extra):
    spec = dict(name=name, type=name if objclass == 12 else 'Locke', **{'class': objclass}, pos=list(AT),
                facing=facing, moveangle=extra.pop('moveangle', facing),
                stats=dict(health=extra.pop('health', 100), sleeping=extra.pop('sleeping', 0)),
                classstats=dict(radius=16), chardata=dict(combatrangemax=300), states=states or WALK_STATES,
                animator=extra.pop('animator', 1),
                root=root or dict(name='walk', action=ANIMATE, angle=facing, moveangle=facing),
                doing=doing, desired=desired)
    spec.update(extra)
    return spec


def _step(name='walkf', angle=64, action=MOVE, turnrate=8, **extra):
    return dict(name=name, action=action, angle=angle, moveangle=extra.pop('moveangle', angle), turnrate=turnrate,
                **extra)


def update_cases(data: Path, workdir: Path) -> list[dict]:
    cases = []

    def case(name, chars, bits=0, frame=25, **extra):
        globals_ = dict(combatface=1, frame=frame, ambient=extra.pop('ambient', 128))
        cases.append(dict(name=f'ua.{name}', call='update-action', bits=bits, globals=globals_,
                          self=chars[0]['name'], chars=chars, **extra))

    # The root: done unless mid-transition; done or not by the animation,
    # the animator, invisibility; priority dropped when done.
    for trans, cdone, anim, invis, prio in itertools.product((0, 1), (0, 1), (0, 1), (0, 1), (0, 1)):
        flags = (['transition'] if trans else []) + (['priority'] if prio else [])
        root = dict(name='walk', action=ANIMATE, angle=64, moveangle=64, flags=flags)
        case(f'root.tr{trans}.cd{cdone}.an{anim}.inv{invis}.pr{prio}',
             [_who(root=root, commanddone=cdone, animator=anim, objflags=0x80 if invis else 0)])
    # A non-root animation (an emote) with a desired block waiting.
    for cdone, anim, prio, nowait in itertools.product((0, 1), (0, 1), (0, 1), (0, 1)):
        doing = dict(name='say', action=ANIMATE, flags=(['priority'] if prio else []) + (['nowaitdone'] if nowait else []))
        case(f'emote.cd{cdone}.an{anim}.pr{prio}.nw{nowait}',
             [_who(doing=doing, desired=_step('walkf'), commanddone=cdone, animator=anim)])

    # Stealth: on and off the 24-frame beat, a finished non-root action,
    # negative glimpse / noise, sneaking, attacking, the ambient light.
    for frame, cdone, glimpse, noise in itertools.product((24, 48, 25, 47), (0, 1), (5, -1), (5, -1)):
        case(f'stealth.f{frame}.cd{cdone}.g{glimpse}.n{noise}',
             [_who(doing=dict(name='say', action=ANIMATE), commanddone=cdone, glimpse=glimpse, noise=noise)],
             frame=frame, seed=frame * 7 + cdone)
    for root, ambient in itertools.product(('walk', 'sneak'), (0, 30, 128, 255, 400)):
        r = dict(name=root, action=ANIMATE, angle=64, moveangle=64)
        case(f'stealth.{root}.amb{ambient}', [_who(root=r, states=WALK_STATES + ['sneak'])], ambient=ambient,
             tape=[0, 12345, 32767][ambient % 3:])
    # (An attacking character's stealth needs ResolveAttack, kata C4.)

    # Asleep: woken unless the root is a sleep root.
    for root_action, sleeping in itertools.product((ANIMATE, SLEEP), (0, 1)):
        root = dict(name='sleep' if root_action == SLEEP else 'walk', action=root_action, angle=64, moveangle=64)
        case(f'sleep.root{root_action}.s{sleeping}', [_who(root=root, sleeping=sleeping)])

    # Walking (ResolveMove): turning to pivot first, by hand or with a pivot
    # animation, then the first step that exists.
    for facing, angle, cdone in itertools.product((64, 60, 0), (64, 128), (0, 1)):
        doing = _step('walk', angle, flags=['waitpivot'], turnrate=12)
        case(f'pivot.root.f{facing}.a{angle}.cd{cdone}',
             [_who(facing=facing, doing=doing, desired='doing', commanddone=cdone)])
        doing = _step('pivotl', angle, flags=['waitpivot'])
        case(f'pivot.anim.f{facing}.a{angle}.cd{cdone}',
             [_who(facing=facing, doing=doing, desired='doing', commanddone=cdone)])
    for first in ('walkf', 'walkl', 'walkr', None):
        states = ['walk', 'fall'] + ([first] if first else [])
        doing = _step('walk', 64, flags=['waitpivot'])
        case(f'pivot.first-{first}', [_who(facing=64, doing=doing, desired='doing', states=states)])
    # Blocked: the bounce, every 16th angle and the octant edges.
    for angle in sorted(set(range(0, 256, 16)) | {0x1f, 0x20, 0x40, 0x41, 0x5f, 0x60, 0x7e, 0x7f, 0xa0, 0xa1,
                                                    0xbe, 0xbf, 0xe0, 0xe1, 0xff}):
        case(f'blocked.a{angle:#x}', [_who(doing=_step('walkf', angle), desired='doing')], bits=BLOCKED)
    # A Goto: there (MoveTo), not yet (heading), from various bearings.
    for dx, dy in ((3, 2), (7, 0), (5, 5), (8, 0), (40, 0), (-30, 50), (0, -100)):
        doing = _step('walkf', 0, target=[AT[0] + dx, AT[1] + dy, 0])
        case(f'goto.{dx}.{dy}', [_who(doing=doing, desired='doing')], bits=MOVED)
    # Steps: turning toward the angle, the next step at the end of one
    # (left and right alternate, forward repeats), stopped.
    for name, cdone, stop, angle, facing in itertools.product(('walkf', 'walkl', 'walkr'), (0, 1), (0, 1),
                                                              (64, 100), (64, 20)):
        doing = _step(name, angle, flags=['stop'] if stop else [], turnrate=8)
        case(f'step.{name}.cd{cdone}.st{stop}.a{angle}.f{facing}',
             [_who(facing=facing, doing=doing, desired='doing', commanddone=cdone)], bits=MOVED)
    # Running: a run root's steps go through ResolveMove too.
    for action in (MOVE, COMBATMOVE):
        root = dict(name='run', action=ANIMATE, angle=64, moveangle=64)
        case(f'run.action{action}', [_who(root=root, doing=_step('runl', 64, action=action), desired='doing',
                                          commanddone=1)], bits=MOVED)

    # Falling: the fall animation, unless missing or the doing block has
    # priority; nothing else is tried that tick.
    for has_fall, prio in itertools.product((1, 0), (0, 1)):
        states = WALK_STATES if has_fall else [s for s in WALK_STATES if s != 'fall']
        doing = _step('walkf', 64, flags=['priority'] if prio else [])
        case(f'fall.has{has_fall}.pr{prio}', [_who(doing=doing, desired='doing', states=states)], bits=FALLING)

    # Say and pivot blocks.
    for wait, stop in itertools.product((0, 1, 5), (0, 1)):
        doing = dict(name='say', action=SAY, wait=wait, flags=['stop'] if stop else [])
        case(f'say.w{wait}.st{stop}', [_who(doing=doing, desired='doing')])
    for name, facing, cdone in itertools.product(('walk', 'pivotl'), (64, 30), (0, 1)):
        doing = dict(name=name, action=PIVOT, angle=64, moveangle=64, turnrate=8)
        case(f'pivotblock.{name}.f{facing}.cd{cdone}', [_who(facing=facing, doing=doing, desired='doing',
                                                             commanddone=cdone)])

    # The desired block tried after the doing one: incidentals off (charflags
    # 2), a desired step while standing, priority on the doing block.
    for incid, cdone, prio in itertools.product((0, 2), (0, 1), (0, 1)):
        doing = dict(name='say', action=ANIMATE, flags=['priority'] if prio else [])
        case(f'desire.ch{incid}.cd{cdone}.pr{prio}',
             [_who(doing=doing, desired=_step('walkf', 64), commanddone=cdone, charflags=incid)])

    # Combat: the root and a step through the combat resolvers, a player and
    # a monster, with a target.
    for objclass, name in ((11, 'Locke'), (12, 'Araknid')):
        root = dict(name='combat', action=COMBAT, angle=64, moveangle=64, obj='Foe')
        foe = _who(name='Foe', objclass=12, type='Araknid', states=['combat'],
                   root=dict(name='combat', action=COMBAT, angle=192, moveangle=192))
        foe['type'] = 'Araknid'
        foe['pos'] = [AT[0] + 100, AT[1], AT[2]]
        for doing, cdone, bits in itertools.product(('root', 'step'), (0, 1), (0, MOVED, BLOCKED)):
            d = 'root' if doing == 'root' else _step('combatr', 64, action=COMBATMOVE, moveangle=128,
                                                     obj='Foe', flags=['noroot'])
            me = _who(name=name, objclass=objclass, states=COMBAT_STATES, root=root, doing=d,
                      desired='doing' if doing == 'step' else 'root', commanddone=cdone)
            case(f'combat.{name}.{doing}.cd{cdone}.b{bits}', [me, foe], bits=bits, frame=0x11)
    return finish(cases)


TARGETS = {
    'combat-update': dict(fixture='slots/combat/combat_call.py', cases=update_cases, compare=compare_move,
                          port_fields=port_fields, unit=lambda r: 1),
}
