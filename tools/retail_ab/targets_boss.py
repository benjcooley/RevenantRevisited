"""Combat dojo boss target for retail_ab.py (docs/gameplay/COMBAT_DOJO.md
C3c; forensics docs/gameplay/forensics/BOSS_AI.md).

`melee-boss` (kata C3c): ticks of AI_PerMonster (retail 0x4c9b70, port
TCharacter::AIPerMonster and src/game/ai/bossai.cpp) on the four bosses with their
real char.def records -- every state of each one's machine and its
transitions, the health bands and their edges, the timers at their turns,
the random draws by tape, the world each one reads (Baez's crowd, ring wall
and table; Solifuge's brood; Jhaga's waypoints; the player) -- and a few
ticks of the whole AI with a boss in it.

Retail: tools/retail_runtime/slots/combat/melee_attack.py (`per-monster`);
port: src/game/ab/retailab_melee.cpp (`Revenant --retail-ab=melee-boss`). The
compare is combat_targets.compare.
"""
from __future__ import annotations

import copy
import itertools
from pathlib import Path

from combat_targets import _toward, compare, finish, port_fields
from targets_melee import MONSTER_STATS, PLAYER_STATS, block, case, chardata, make_char, shipped

BOSS_AT = [2000, 2000, 0]
HERO = 'Hero'
BOSS = 'Boss'
KIND = {'Baez': 1, 'Solifuge': 2, 'Jhaga': 3, 'Yhagoro': 4}
MAXHEALTH = 200                       # each boss's chardata health in the cases: percentages are health / 2

# Every state name each boss's code asks for, and the transitions between
# them its blocks look up.
YHAGORO_STATES = ['walk', 'combat', 'walk to combat', 'combat to walk', 'impimpale', 'combatf', 'walkf']
SOLIFUGE_STATES = ['combat', 'combatf', 'stunned', 'pissedoff', 'skywalk', 'skywalkf', 'combat to skywalk',
                   'skywalk to combat', 'combat to stunned', 'cimph']
JHAGA_STATES = ['combat', 'cinv2', 'cinv2 to combat', 'combat to cinv2', 'walk']
BAEZ_STATES = ['combat', 'walk', 'walkf', 'combatf', 'combat to stunned', 'stunned', 'stunned to combat',
               'combat to fakedead', 'fakedead', 'fakedead to walk', 'combat to cdead', 'cdead', 'walk to combat']
HERO_STATES = ['combat', 'walk', 'sunsetflipper', 'combat to walk', 'impk', 'cimpk', 'walkf']
WATCHER_STATES = ['watcha', 'watchb', 'watchc', 'cheera', 'cheerb', 'cheerc', 'booa', 'boob', 'booc', 'walk']
SPIDER_STATES = ['sleeping', 'sleeping to combat', 'combat']


def held(spec: dict, waiting: str = 'walkf') -> dict:
    """The doing block holds priority over a block waiting: every want
    (SetDesired) is refused and every Force waits."""
    spec = copy.deepcopy(spec)
    doing = copy.deepcopy(spec.get('doing') or spec['root'])
    doing['flags'] = sorted(set(doing.get('flags', [])) | {'priority'})
    spec['doing'] = doing
    if not isinstance(spec.get('desired'), dict):
        spec['desired'] = block(waiting, 1)
    return spec


def boss_cases(data: Path, workdir: Path) -> list[dict]:
    s = shipped(data, workdir)
    cases = []
    seed = 71

    def add(name, chars, *, ticks=1, tape=None, frame=100, whole_ai=False, **kw):
        """A case of `ticks` ticks of AI_PerMonster; `whole_ai`: one tick of
        the AI around it (AI 0x4c8b60, the boss's world all the same)."""
        nonlocal seed
        seed += 1
        if whole_ai:
            cases.append(case(name, 'ai', chars, s['rules'], self_=BOSS, args={}, seed=seed * 7919 + 3,
                              tape=tape if tape is not None else [40, 7, 1, 2], frame=frame, ground=dict(z=0),
                              bossworld=True, **kw))
            return
        cases.append(case(name, 'per-monster', chars, s['rules'], self_=BOSS, args=dict(ticks=ticks),
                          seed=seed * 7919 + 3, tape=tape if tape is not None else [], frame=frame,
                          ground=dict(z=0), **kw))

    def boss(typ, *, health=MAXHEALTH, kind=None, state=None, states=None, root=None, doing=None, desired=None,
             animator=True, charflags=0, objflags=0, facing=64, pos=None, ident=0x10):
        cd = chardata(s['chars'][typ])
        cd['health'] = MAXHEALTH
        spec = make_char(BOSS, 12, pos or BOSS_AT, facing, cd, typ=typ, states=states, root=root, doing=doing,
                         desired=desired, stats=dict(MONSTER_STATS, health=health, aggressive=1, sleeping=0),
                         charflags=charflags, objflags=objflags, ident=ident)
        spec['animator'] = animator
        spec['monsterkind'] = KIND[typ] if kind is None else kind
        if state is not None:
            spec['boss'] = state
        return spec

    def hero(*, weapon=1, health=100, at=None, root=None, doing=None, desired=None, ident=0x20, facing=192):
        spec = make_char(HERO, 11, at or _toward(BOSS_AT, 64, 80), facing, chardata(s['chars']['Locke']),
                         states=HERO_STATES, root=root or block('combat', 3, obj=BOSS), doing=doing,
                         desired=desired, weapon={'type': weapon, 'damage': 4},
                         stats=dict(PLAYER_STATS, health=health), ident=ident)
        spec['animator'] = True
        return spec

    # ---- Yhagoro (BOSS_AI.md §5) --------------------------------------------------
    # The first tick sets him up; every band (and its edges) against every
    # mode, the two wishes, what he is doing, his root; the player's sword.
    for health in (200, 152, 150, 120, 102, 100, 52, 50, 20):
        for mode, towalk, tocombat in ((0, 0, 0), (1, 0, 0), (-1, 0, 0), (0, 1, 0), (1, 0, 1), (1, 1, 0),
                                       (0, 0, 1), (-1, 1, 1)):
            for doing in ('combat', 'walk', 'walk to combat', 'combat to walk', 'impimpale'):
                for rootname in ('combat', 'walk'):
                    chars = [boss('Yhagoro', health=health, state=dict(mode=mode, towalk=towalk, tocombat=tocombat),
                                  states=YHAGORO_STATES, root=block(rootname, 3, obj=HERO),
                                  doing=block(doing, 1, obj=HERO)), hero()]
                    add(f'yhagoro.h{health}.m{mode}.w{towalk}.c{tocombat}.{doing.replace(" ", "")}.r{rootname}',
                        chars)
    # Going back to walking: done only with doing, desired and root all "walk".
    for doing, desired, rootname in itertools.product(('walk', 'combat'), ('walk', 'combat', None),
                                                       ('walk', 'combat')):
        chars = [boss('Yhagoro', health=200, state=dict(mode=-1, towalk=1, tocombat=0), states=YHAGORO_STATES,
                      root=block(rootname, 3, obj=HERO), doing=block(doing, 1, obj=HERO),
                      desired=block(desired, 1) if desired else None), hero()]
        add(f'yhagoro.back.{doing}.{desired}.{rootname}', chars)
    # The doing block holds priority over a desired one: Force waits.
    for mode, doing in ((1, 'combat'), (0, 'walk'), (0, 'combat')):
        chars = [boss('Yhagoro', health=200 if mode == 1 else 120, state=dict(mode=mode, towalk=0, tocombat=0),
                      states=YHAGORO_STATES, root=block('combat', 3, obj=HERO),
                      doing=block(doing, 1, obj=HERO, flags=['priority']), desired=block('combatf', 1)), hero()]
        add(f'yhagoro.priority.m{mode}.{doing}', chars)
    # Set up on the first tick; the player's weapon; no player; no animator.
    for weapon in (2, 1, 0):
        chars = [boss('Yhagoro', kind=0, states=YHAGORO_STATES, root=block('walk', 1), doing=block('walk', 1)),
                 hero(weapon=weapon)]
        add(f'yhagoro.setup.w{weapon}', chars)
    chars = [boss('Yhagoro', kind=0, states=YHAGORO_STATES, root=block('walk', 1), doing=block('walk', 1),
                  animator=False)]
    add('yhagoro.setup.alone.noanimator', chars)
    # Not a boss: the kind -1, nothing else.
    chars = [boss('Araknid', kind=0, states=['combat'], root=block('combat', 3, obj=HERO)), hero()]
    add('notaboss.araknid', chars)
    # A run of ticks from each band (nothing else moves the blocks between).
    for health in (200, 120, 60, 20):
        chars = [boss('Yhagoro', kind=0, health=health, states=YHAGORO_STATES, root=block('walk', 1),
                      doing=block('walk', 1)), hero(weapon=2)]
        add(f'yhagoro.run.h{health}', chars, ticks=12)

    # ---- Jhaga (BOSS_AI.md §4) ----------------------------------------------------
    def waypoint(name, at, ident, typ='Waypoint'):
        return dict(name=name, type=typ, **{'class': 15}, pos=list(at), id=ident)

    # Four waypoints round him, one to a quadrant; sums that don't divide by
    # four, negative ones (the centre rounds toward zero).
    squares = {
        'square': [(1800, 1800, 0), (1800, 2200, 0), (2200, 1800, 0), (2200, 2200, 0)],
        'skew': [(1601, 1903, 7), (1755, 2311, 9), (2402, 1700, 2), (2290, 2420, 5)],
        'negative': [(-401, -397, -3), (-410, 13, -6), (9, -406, 1), (15, 22, 2)],
        'shuffled': [(2290, 2420, 5), (2402, 1700, 2), (1755, 2311, 9), (1601, 1903, 7)]}
    jhaga_points = {}
    for label, at in squares.items():
        wps = [waypoint(f'wp{i}', p, 0x40 + i) for i, p in enumerate(at)]
        cx, cy, cz = (int(sum(p[k] for p in at) / 4) for k in range(3))
        quad = lambda xs, ys: next(p for p in at if (p[0] < cx if xs < 0 else p[0] > cx) and
                                   (p[1] < cy if ys < 0 else p[1] > cy))
        jhaga_points[label] = [list(quad(-1, -1)), list(quad(-1, 1)), list(quad(1, -1)), list(quad(1, 1)),
                               [cx, cy, cz]]
        for extra in ('none', 'others', 'five'):
            props = list(wps)
            pane = [w['name'] for w in wps]
            if extra == 'others':
                props.append(waypoint('axis', (2000, 2100, 0), 0x50, typ='Axis'))
                pane = ['axis', HERO, pane[0], pane[1], BOSS, pane[2], pane[3]]
            elif extra == 'five':
                props.append(waypoint('wp4', (2500, 2500, 0), 0x45))
                pane = pane + ['wp4']
            chars = [boss('Jhaga', kind=0, states=JHAGA_STATES, root=block('combat', 3, obj=HERO),
                          doing=block('combat', 3, obj=HERO)), hero()]
            add(f'jhaga.setup.{label}.{extra}', chars, props=props, paneobjects=pane)
    for count in range(4):
        wps = [waypoint(f'wp{i}', p, 0x40 + i) for i, p in enumerate(squares['square'][:count])]
        chars = [boss('Jhaga', kind=0, states=JHAGA_STATES, root=block('combat', 3, obj=HERO),
                      doing=block('combat', 3, obj=HERO), charflags=4), hero()]
        add(f'jhaga.setup.fewer{count}', chars, props=wps,
            paneobjects=[HERO] + [w['name'] for w in wps])
    # Resting: the timer at its turn, the draw by tape.
    points = jhaga_points['skew']
    for timer in (1, 2, 240):
        for draw in (0, 1, 2):
            chars = [boss('Jhaga', state=dict(mode=1, points=points, resttimer=timer), states=JHAGA_STATES,
                          root=block('combat', 3, obj=HERO), doing=block('combat', 3, obj=HERO)), hero()]
            add(f'jhaga.rest.t{timer}.d{draw}', chars, tape=[draw])
    # Teleporting: every jump, every frame that does something and those
    # round it, what he is doing, the player there or not, the spells known
    # or not.
    for hop in range(7):
        for frame in (-1, 0, 1, 5, 6, 7, 23, 24, 29, 30, 31, 35, 36, 37, 71, 72):
            for doing in ('cinv2', 'combat'):
                for player in (True, False):
                    for known in (True, False):
                        if not known and (frame != 30 or not player):
                            continue
                        chars = [boss('Jhaga', state=dict(mode=2, points=points, resttimer=240, hop=hop,
                                                          dest=points[min(hop, 4)], frame=frame),
                                      states=JHAGA_STATES, root=block('combat', 3, obj=HERO if player else None),
                                      doing=block(doing, 1, obj=HERO if player else None))]
                        if player:
                            chars.append(hero(at=_toward(BOSS_AT, 100, 300)))
                        spells = ['Priest Fire Wind', 'Priest Fireball'] if known else []
                        add(f'jhaga.hop{hop}.f{frame}.{doing}.p{int(player)}.k{int(known)}', chars, spells=spells)
    # The cast's gates: paralysed, dead.
    for label, objflags, health in (('paralysed', 0x800000, 200), ('dead', 0, 0)):
        for hop in (1, 4):
            chars = [boss('Jhaga', state=dict(mode=2, points=points, resttimer=240, hop=hop, dest=points[hop],
                                              frame=30),
                          states=JHAGA_STATES, root=block('combat', 3, obj=HERO), doing=block('cinv2', 1),
                          objflags=objflags, health=health), hero()]
            add(f'jhaga.gate.{label}.hop{hop}', chars, spells=['Priest Fire Wind', 'Priest Fireball'])
    # Held: each want of a jump refused while the doing block holds.
    for hop in (0, 2, 4):
        for frame in (0, 6, 24, 30, 36, 72):
            chars = [held(boss('Jhaga', state=dict(mode=2, points=points, resttimer=240, hop=hop, dest=points[hop],
                                                   frame=frame),
                               states=JHAGA_STATES, root=block('combat', 3, obj=HERO), doing=block('cinv2', 1))),
                     hero()]
            add(f'jhaga.held.hop{hop}.f{frame}', chars, spells=['Priest Fire Wind', 'Priest Fireball'])
    # Without an animator.
    chars = [boss('Jhaga', state=dict(mode=1, points=points, resttimer=5), states=JHAGA_STATES,
                  root=block('combat', 3, obj=HERO), animator=False), hero()]
    add('jhaga.noanimator', chars)
    # A whole cycle from the set-up: the rest, five jumps, the rest again.
    for label in ('square', 'negative'):
        wps = [waypoint(f'wp{i}', p, 0x40 + i) for i, p in enumerate(squares[label])]
        chars = [boss('Jhaga', kind=0, states=JHAGA_STATES, root=block('combat', 3, obj=HERO),
                      doing=block('combat', 3, obj=HERO)), hero()]
        add(f'jhaga.cycle.{label}', chars, ticks=620, props=wps, paneobjects=[w['name'] for w in wps],
            spells=['Priest Fireball'], tape=[2])

    # ---- Solifuge (BOSS_AI.md §3) -------------------------------------------------
    def spider(name, ident, *, health=30, doing='combat', typ='Araknid'):
        spec = make_char(name, 12, _toward(BOSS_AT, 200, 60), 0, chardata(s['chars'][typ]), typ=typ,
                         states=SPIDER_STATES, root=block('combat', 3), doing=block(doing, 1),
                         stats=dict(MONSTER_STATS, health=health), ident=ident)
        spec['animator'] = True
        return spec

    def solifuge(*, health=MAXHEALTH, state=None, doing='combat', rootname='combat', desired=None, target=HERO,
                 **kw):
        return boss('Solifuge', health=health, state=state, states=SOLIFUGE_STATES,
                    root=block(rootname, 3, obj=target), doing=block(doing, 1, obj=target),
                    desired=block(desired, 1) if desired else None, **kw)

    near = _toward(BOSS_AT, 64, 40)                # rough distance 40, under 3 radii (48)
    chars = [solifuge(kind=0), hero()]
    add('solifuge.setup', chars)
    # The player dead: while Solifuge isn't in his stance the death screen waits.
    for doing in ('combat', 'combatf', 'stunned'):
        for health in (0, 1):
            chars = [solifuge(state={}, doing=doing), hero(health=health)]
            add(f'solifuge.playerdead.{doing}.h{health}', chars)
    chars = [solifuge(state=dict(skytimer=3), target=None)]
    add('solifuge.alone', chars)
    # The bands and their edges, from each band: a lost one makes him roar.
    for health in (202, 200, 134, 132, 68, 66, 2, 0):
        for band in (0, 1, 2):
            chars = [solifuge(health=health, state=dict(band=band)), hero()]
            add(f'solifuge.band.h{health}.b{band}', chars)
    # Dead: the brood dies with him (a spider there, one gone, one dead).
    chars = [solifuge(health=0, state=dict(band=2, brood=[0x60, 0x61, 0x77])), hero(),
             spider('Old0', 0x60), spider('Old1', 0x61, health=0)]
    add('solifuge.dead.brood', chars)
    # Paralysed: the stun armed, blued once.
    for band in (0, 1, 2):
        for tinted in (0, 1):
            chars = [solifuge(health=(200, 120, 40)[band], objflags=0x800000,
                              state=dict(band=band, tinted=tinted, skytimer=17, stunned=0)), hero()]
            add(f'solifuge.paralysed.b{band}.t{tinted}', chars)
    chars = [solifuge(state=dict(tinted=1, stunned=1, stunticks=72)), hero()]
    add('solifuge.untint', chars)
    # On the ground, not stunned: the roar, the slam's stance, the player near
    # or far or gone.
    for slam in (0, 1):
        for pissed in (0, 1):
            for doing in ('combat', 'combatf', 'pissedoff', 'cimph'):
                for where in ('near', 'far', 'none'):
                    if where != 'far' and not slam:
                        continue
                    chars = [solifuge(doing=doing, state=dict(slam=slam, pissedoff=pissed),
                                      charflags=0x8004, target=None if where == 'none' else HERO)]
                    if where != 'none':
                        chars.append(hero(at=near if where == 'near' else None))
                    add(f'solifuge.ground.s{slam}.p{pissed}.{doing}.{where}', chars)
    # Stunned: falls into "stunned" unless already down or in the sky walk;
    # the count down and the stance back.
    for doing in ('stunned', 'skywalk to combat', 'skywalk', 'combat'):
        for ticks in (1, 2, 72):
            for priority in (False, True):
                chars = [boss('Solifuge', state=dict(stunned=1, stunticks=ticks), states=SOLIFUGE_STATES,
                              root=block('combat', 3, obj=HERO),
                              doing=block(doing, 1, obj=HERO, flags=['priority'] if priority else []),
                              desired=block('combatf', 1)), hero()]
                add(f'solifuge.stunned.{doing.replace(" ", "")}.t{ticks}.pr{int(priority)}', chars)
    # The sky walk's timer on the ground; going up.
    for timer, phase in ((0, 0), (1, 0), (2, 0), (5, 1), (0, 1), (3, 2)):
        for band in (0, 2):
            for doing in ('skywalk', 'skywalkf', 'combat'):
                chars = [solifuge(health=(200, 40)[band // 2], doing=doing,
                                  state=dict(skytimer=timer, skyphase=phase, band=band, skyticks=50)), hero()]
                add(f'solifuge.sky.t{timer}.p{phase}.b{band}.{doing}', chars)
    # In the air: the brood called and woken (lowest band), the landing's
    # last 16 ticks, down.
    for ticks in (193, 192, 169, 168, 100, 17, 16, 2, 1, 0):
        for band in (0, 2):
            for landing in (0, 1):
                for doing in ('skywalkf', 'combat to skywalk', 'skywalk'):
                    chars = [solifuge(health=(200, 40)[band // 2], doing=doing, desired='combat' if ticks < 2 else None,
                                      state=dict(skyphase=2, skyticks=ticks, band=band, landing=landing,
                                                 brood=[0x60, 0x61, 0x62])),
                             hero(), spider('Old0', 0x60, doing='sleeping'), spider('Old1', 0x61, health=0),
                             spider('New0', 0x70, typ='Recluse'), spider('New1', 0x71), spider('New2', 0x72)]
                    add(f'solifuge.air.t{ticks}.b{band}.l{landing}.{doing.replace(" ", "")}', chars,
                        made=['New0', 'New1', 'New2'], tape=[0, 1, 37, 90, 1, 55, 3])
    # The brood: each slot's spider there and alive, dead, gone; both names
    # of each kind by tape; more spiders called than the case holds.
    for brood, tape in (([0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 0, 0]), ([0, 0, 0], [1, 64, 127, 1, 3, 5, 0, 100, 2]),
                        ([0x60, 0x61, 0x62], [1, 9, 9]), ([0x60, 0x77, 0x61], [0, 128, 0, 1, 7, 7])):
        chars = [solifuge(health=40, doing='skywalkf', state=dict(skyphase=2, skyticks=193, band=2, brood=brood)),
                 hero(), spider('Old0', 0x60), spider('Old1', 0x61, health=0), spider('Old2', 0x62),
                 spider('New0', 0x70, typ='Arakna'), spider('New1', 0x71)]
        add(f'solifuge.brood.{"-".join(map(str, brood))}.t{tape[0]}', chars, made=['New0', 'New1'], tape=tape)
    # Held: each want refused (his, the player's, the spiders').
    chars = [held(solifuge(doing='combat', state=dict(pissedoff=1))), hero()]
    add('solifuge.held.roar', chars)
    for bearing in (64, 128, 0):
        chars = [solifuge(doing='combat', state=dict(slam=1)),
                 held(hero(at=_toward(BOSS_AT, bearing, 40), doing=block('combat', 3)))]
        add(f'solifuge.held.slam.b{bearing}', chars)
        chars = [solifuge(doing='combatf', state=dict(slam=1)), hero(at=_toward(BOSS_AT, bearing, 40))]
        add(f'solifuge.slam.b{bearing}', chars)
    for timer, phase, doing in ((2, 0, 'skywalk'), (0, 1, 'combat')):
        chars = [held(solifuge(doing=doing, state=dict(skytimer=timer, skyphase=phase, band=1, skyticks=5))), hero()]
        add(f'solifuge.held.sky.t{timer}.p{phase}', chars)
    for ticks, landing in ((100, 0), (100, 1), (1, 0)):
        chars = [held(solifuge(doing='skywalkf', state=dict(skyphase=2, skyticks=ticks, band=0, landing=landing))),
                 hero()]
        add(f'solifuge.held.air.t{ticks}.l{landing}', chars)
    for ticks in (193, 169):
        chars = [solifuge(health=40, doing='skywalkf', state=dict(skyphase=2, skyticks=ticks, band=2,
                                                                  brood=[0x60, 0x61, 0x62])),
                 hero(), held(spider('Old0', 0x60, doing='sleeping')), spider('Old1', 0x61, health=0),
                 held(spider('Old2', 0x62, doing='sleeping')), held(spider('New0', 0x70, doing='sleeping'))]
        add(f'solifuge.held.brood.t{ticks}', chars, made=['New0'], tape=[1, 10, 20])
    # The whole brood asleep, woken (or held).
    for hold in (False, True):
        sleeper = lambda n, i: held(spider(n, i, doing='sleeping')) if hold else spider(n, i, doing='sleeping')
        chars = [solifuge(health=40, doing='skywalkf', state=dict(skyphase=2, skyticks=169, band=2,
                                                                  brood=[0x60, 0x61, 0x62])),
                 hero(), sleeper('Old0', 0x60), sleeper('Old1', 0x61), sleeper('Old2', 0x62)]
        add(f'solifuge.wake.all.h{int(hold)}', chars)
    # The whole brood dies with him.
    chars = [solifuge(health=0, state=dict(band=2, brood=[0x60, 0x61, 0x62])), hero(),
             spider('Old0', 0x60), spider('Old1', 0x61), spider('Old2', 0x62)]
    add('solifuge.dead.allbrood', chars)
    # Runs: a lowest-band sky walk from the ground to the slam; the
    # paralysis and its end.
    chars = [solifuge(health=40, state=dict(skytimer=3, band=2)), hero(at=near), spider('New0', 0x70),
             spider('New1', 0x71), spider('New2', 0x72)]
    add('solifuge.run.skywalk', chars, ticks=260, made=['New0', 'New1', 'New2'], tape=[1, 20, 40, 0, 30, 50, 1, 10, 60])
    chars = [solifuge(health=120, state=dict(band=1)), hero()]
    add('solifuge.run.ground', chars, ticks=300)

    # ---- The whole AI with a boss in it: AI 0x4c8b60 runs AI_PerMonster first,
    # then its own (the target kept or found, the timers, the branches).
    chars = [boss('Yhagoro', health=120, state=dict(mode=1), states=YHAGORO_STATES,
                  root=block('combat', 3, obj=HERO), doing=block('combat', 3, obj=HERO)), hero()]
    add('ai.yhagoro.fighting', chars, whole_ai=True)
    chars = [boss('Yhagoro', kind=0, states=YHAGORO_STATES, root=block('walk', 1), doing=block('walk', 1)), hero()]
    add('ai.yhagoro.setup', chars, whole_ai=True, found=HERO, frame=0x10)
    chars = [solifuge(doing='combat', state=dict(slam=1)), hero(at=near)]
    add('ai.solifuge.slam', chars, whole_ai=True)
    chars = [solifuge(health=40, doing='skywalkf', state=dict(skyphase=2, skyticks=100, band=2)), hero()]
    add('ai.solifuge.air', chars, whole_ai=True)
    chars = [boss('Jhaga', state=dict(mode=2, points=points, resttimer=240, hop=1, dest=points[3], frame=30),
                  states=JHAGA_STATES, root=block('combat', 3, obj=HERO), doing=block('cinv2', 1, obj=HERO)), hero()]
    add('ai.jhaga.cast', chars, whole_ai=True, spells=['Priest Fireball'])
    chars = [boss('Jhaga', state=dict(mode=1, points=points, resttimer=100), states=JHAGA_STATES,
                  root=block('combat', 3, obj=HERO)), hero()]
    add('ai.jhaga.rest', chars, whole_ai=True)

    baez_cases(s, add, boss, hero)
    return finish(cases)


# The ring (BOSS_AI.md §2): the table and the spots it gives.
TABLE_AT = [3000, 3000, 0]
FIRST_SPOT = [TABLE_AT[0] + 0x20, TABLE_AT[1] + 0x1b0, TABLE_AT[2] + 0x58]
PLAYER_SPOT = [TABLE_AT[0], TABLE_AT[1] + 0xa0, TABLE_AT[2]]
BAEZ_SPOT = [TABLE_AT[0], TABLE_AT[1] + 0x15e, TABLE_AT[2]]
CROWD = ['mark', 'rex', 'vince', 'redo']                 # four of the eleven, by their own names
CROWD_ROOTS = {'mark': 'watcha', 'rex': 'watchb', 'vince': 'watchc', 'redo': 'walk'}
TABLE = dict(name='table', type='ForBreakTable', **{'class': 9}, pos=TABLE_AT, id=0x80, states=['whole', 'broken'])
RINGWALL = dict(name='ringwall', type='ForRing6', **{'class': 9}, pos=[2500, 2500, 0], id=0x81)


def baez_cases(s, add, boss, hero):
    def watcher(name, ident, *, animator=True):
        spec = make_char(name, 12, _toward(BOSS_AT, ident * 13 % 256, 300), 0, chardata(s['chars']['Ogrok']),
                         typ='Ogrok', states=WATCHER_STATES, root=block(CROWD_ROOTS[name], 1),
                         doing=block(CROWD_ROOTS[name], 1), stats=dict(MONSTER_STATS), ident=ident)
        spec['animator'] = animator
        return spec

    def crowd(**kw):
        return [watcher(n, 0x90 + i, **kw) for i, n in enumerate(CROWD)]

    def watchers_state():
        return CROWD + [None] * 7

    def baez(*, health=MAXHEALTH, state=None, doing='combat', rootname='combat', desired=None, at=None,
             target=HERO, **kw):
        return boss('Baez', health=health, state=state, states=BAEZ_STATES, root=block(rootname, 3, obj=target),
                    doing=block(doing, 1, obj=target), desired=block(desired, 1) if desired else None, pos=at, **kw)

    def found(*, watchers=False, **fields):
        """A Baez that has looked up his crowd (`watchers`: the case's), last
        seen at these healths."""
        return dict(dict(watchersfound=1, health=MAXHEALTH, playerhealth=100,
                         watchers=watchers_state() if watchers else [None] * 11), **fields)

    # Set-up: the crowd found by name among the pane's objects (and not
    # among others); the healths noted.
    pane = ['ringwall', 'mark', HERO, 'table', 'rex', 'vince', BOSS, 'redo']
    for player in (True, False):
        chars = [baez(kind=0, target=HERO if player else None)] + ([hero()] if player else []) + crowd()
        add(f'baez.setup.p{int(player)}', chars, props=[TABLE, RINGWALL], paneobjects=pane if player else
            [p for p in pane if p != HERO])
    # The crowd: the player hurt (cheer), healed or Baez hurt (boo), both,
    # neither; each root; a watcher without its animator.
    for ph, h, label in ((90, 200, 'cheer'), (110, 200, 'boo.healed'), (100, 150, 'boo.hurt'),
                         (90, 150, 'both'), (100, 200, 'none')):
        for animator in (True, False):
            chars = [baez(health=h, state=found(watchers=True)), hero(health=ph)] + crowd(animator=animator)
            add(f'baez.crowd.{label}.a{int(animator)}', chars)
    # Above 1 health: the stun at each band (each needs the one before; a
    # big blow passes several), the stun's tick, the stance back.
    for health in (200, 162, 160, 122, 120, 82, 80, 42, 40, 2):
        for level in (0, 80, 60, 40, 20):
            for doing in ('combat', 'combat to stunned', 'stunned', 'walk'):
                chars = [baez(health=health, doing=doing, state=found(health=health, stunlevel=level)), hero()]
                add(f'baez.stun.h{health}.l{level}.{doing.replace(" ", "")}', chars)
    for doing in ('combat', 'combat to stunned', 'stunned', 'walk'):
        for ticks in (1, 2, 72):
            chars = [baez(health=150, doing=doing, state=found(health=150, stunlevel=80, stunned=1, stunticks=ticks),
                          charflags=4), hero()]
            add(f'baez.stunned.{doing.replace(" ", "")}.t{ticks}', chars)
    # The fall's first tick: the ring opens (or there is no ring wall), the
    # crowd leaves, the table found (or not: no boss any more).
    for ringwall in (True, False):
        for table in (True, False):
            props = ([RINGWALL] if ringwall else []) + ([TABLE] if table else [])
            chars = [baez(health=1, state=found(watchers=True)), hero()] + crowd()
            add(f'baez.fall.setup.w{int(ringwall)}.t{int(table)}', chars, props=props,
                paneobjects=[p['name'] for p in props] + [HERO, 'mark'])
    # Stage 1, the player walking to his spots: playing dead, getting up,
    # walking on; the arrival at the first spot.
    for doing in ('combat to fakedead', 'fakedead', 'fakedead to walk', 'combat', 'walk', 'walkf'):
        for falling in (0, 1):
            for spot, where in (('first.far', [3600, 3600, 0]), ('first.near', [3040, 3420, 80]),
                                ('second.far', [3600, 3600, 0])):
                arrived = int(spot.startswith('second'))
                chars = [baez(health=1, doing=doing, at=[2600, 2800, 0],
                              state=found(stage=1, falling=falling, playerarrived=arrived, table=0x80,
                                          firstspot=FIRST_SPOT, playerspot=PLAYER_SPOT, baezspot=BAEZ_SPOT,
                                          waitticks=7)),
                         hero(at=where)]
                add(f'baez.walk.{spot}.{doing.replace(" ", "")}.f{falling}', chars, props=[TABLE])
    # Stage 1, the player there: Baez walking on (the rest's count, falling
    # again from the stance), near his spot, at it (the throw begins).
    for at, label in (([2600, 2800, 0], 'far'), ([3000, 3250, 0], 'near'), ([3010, 3340, 0], 'there')):
        for doing in ('combat', 'walk', 'fakedead', 'fakedead to walk', 'combat to fakedead', 'combat to cdead'):
            for wait in (0, 1, 1000):
                chars = [baez(health=1, doing=doing, at=at,
                              state=found(stage=1, playerarrived=1, table=0x80, firstspot=FIRST_SPOT,
                                          playerspot=PLAYER_SPOT, baezspot=BAEZ_SPOT, waitticks=wait)),
                         hero(at=[3005, 3170, 0])]
                add(f'baez.there.{label}.{doing.replace(" ", "")}.w{wait}', chars, props=[TABLE])
    for table in (True, False):
        chars = [baez(health=1, doing='combat', at=[3010, 3340, 0],
                      state=found(stage=1, playerarrived=1, table=0x80, firstspot=FIRST_SPOT,
                                  playerspot=PLAYER_SPOT, baezspot=BAEZ_SPOT)), hero(at=[3005, 3170, 0])]
        add(f'baez.there.throw.t{int(table)}', chars, props=[TABLE] if table else [])
    # Stage 1 without a player (undefined in retail past the walk: not run).
    # Stage 2, the throw: its frames (the flipper at 4, the table at 53/150,
    # the end), the player's priority, no table, no player.
    for frame in (0, 3, 4, 5, 13, 14, 15, 39, 40, 41):
        for priority in (False, True):
            chars = [baez(health=1, doing='combat to cdead',
                          state=found(stage=2, frame=frame, throwlength=40, table=0x80, baezspot=BAEZ_SPOT)),
                     hero(doing=block('walk', 1, flags=['priority'] if priority else []),
                          desired=block('combat', 3) if priority else None)]
            add(f'baez.throw.f{frame}.pr{int(priority)}', chars, props=[TABLE])
    for frame in (52, 53, 54):          # a long fall: 53/150 of 150 is 53 (of 40, 14 by either rounding)
        chars = [baez(health=1, doing='combat to cdead',
                      state=found(stage=2, frame=frame, throwlength=150, table=0x80)), hero()]
        add(f'baez.throw.long.f{frame}', chars, props=[TABLE])
    chars = [baez(health=1, doing='combat to cdead', state=found(stage=2, frame=14, throwlength=40, table=0x99)),
             hero()]
    add('baez.throw.notable', chars, props=[TABLE])
    chars = [baez(health=1, doing='combat to cdead', state=found(stage=2, frame=4, throwlength=40, table=0x80),
                  target=None)]
    add('baez.throw.noplayer', chars, props=[TABLE])
    # Stage 3: dead.
    chars = [baez(health=1, doing='cdead', state=found(stage=3)), hero()]
    add('baez.dead', chars, props=[TABLE])
    # Held: each want and force refused (Baez's, the crowd's, the player's).
    for ph, h, label in ((90, 200, 'cheer'), (100, 150, 'boo')):
        chars = [baez(health=h, state=found(watchers=True)), hero(health=ph)] + [held(w) for w in crowd()]
        add(f'baez.held.crowd.{label}', chars)
    for health, level, stunned, doing in ((120, 80, 0, 'combat'), (150, 80, 1, 'combat'),
                                          (150, 80, 1, 'combat to stunned'), (150, 80, 0, 'stunned'),
                                          (150, 80, 1, 'walk')):
        chars = [held(baez(health=health, doing=doing,
                           state=found(health=health, stunlevel=level, stunned=stunned, stunticks=1))), hero()]
        add(f'baez.held.stun.h{health}.s{stunned}.{doing.replace(" ", "")}', chars)
    chars = [held(baez(health=1, state=found(watchers=True))), hero()] + [held(w) for w in crowd()]
    add('baez.held.fall.setup', chars, props=[RINGWALL, TABLE], paneobjects=['ringwall', 'table'])
    walking = dict(stage=1, table=0x80, firstspot=FIRST_SPOT, playerspot=PLAYER_SPOT, baezspot=BAEZ_SPOT)
    for doing in ('combat to fakedead', 'fakedead', 'fakedead to walk', 'combat', 'walk'):
        for falling, arrived, at, hero_at, wait in ((1, 0, [2600, 2800, 0], [3600, 3600, 0], 7),
                                                    (0, 0, [2600, 2800, 0], [3600, 3600, 0], 7),
                                                    (0, 1, [2600, 2800, 0], [3005, 3170, 0], 0),
                                                    (0, 1, [3010, 3340, 0], [3005, 3170, 0], 5)):
            chars = [held(baez(health=1, doing=doing, at=at,
                               state=found(falling=falling, playerarrived=arrived, waitticks=wait, **walking))),
                     held(hero(at=hero_at, doing=block('walk', 1)))]
            add(f'baez.held.walk.{doing.replace(" ", "")}.f{falling}.a{arrived}.w{wait}.{at[1]}', chars,
                props=[TABLE])
    chars = [held(baez(health=1, doing='walk', at=[2600, 2800, 0],
                       state=found(playerarrived=1, waitticks=5, **walking))), hero(at=[3005, 3170, 0])]
    add('baez.held.walkon.walkf', chars, props=[TABLE])
    for health, level in ((160, 0), (120, 80), (80, 60), (40, 40)):
        chars = [held(baez(health=health, state=found(health=health, stunlevel=level))), hero()]
        add(f'baez.held.stunband.h{health}.l{level}', chars)
    chars = [held(baez(health=1, doing='combat to cdead', state=found(stage=3))), hero()]
    add('baez.held.dead', chars, props=[TABLE])
    # Falling without a fall of his own (no "fakedead", no way into it): both
    # forces tried, in vain.
    chars = [boss('Baez', health=1, state=found(falling=1, **walking),
                  states=[s_ for s_ in BAEZ_STATES if 'fakedead' not in s_], root=block('combat', 3, obj=HERO),
                  doing=block('combat', 1, obj=HERO), pos=[2600, 2800, 0]), hero(at=[3600, 3600, 0])]
    add('baez.walk.nofakedead', chars, props=[TABLE])
    # Without an animator; beyond his spot (the distance's other sign).
    chars = [baez(health=1, doing='walk', at=[2600, 2800, 0], animator=False,
                  state=found(**walking)), hero(at=[3600, 3600, 0])]
    add('baez.walk.noanimator', chars, props=[TABLE])
    for at in ([3100, 3500, 0], [2950, 3400, 0]):
        chars = [baez(health=1, doing='walk', at=at, state=found(playerarrived=1, **walking)),
                 hero(at=[3005, 3170, 0])]
        add(f'baez.there.beyond.{at[0]}', chars, props=[TABLE])
    # The whole AI round him: in the fight, on his walk.
    chars = [baez(health=150, state=found(watchers=True)), hero(health=90)] + crowd()
    add('ai.baez.fight', chars, whole_ai=True)
    chars = [baez(health=1, doing='walk', at=[2600, 2800, 0], state=found(**walking)), hero(at=[3600, 3600, 0])]
    add('ai.baez.walk', chars, whole_ai=True, props=[TABLE])
    # Runs: the fight to the fall's first ticks; the throw from the spot to
    # the end.
    chars = [baez(health=1, state=found(watchers=True)), hero()] + crowd()
    add('baez.run.fall', chars, ticks=30, props=[RINGWALL, TABLE], paneobjects=['ringwall', 'table', HERO])
    chars = [baez(health=1, doing='combat', at=[3010, 3340, 0],
                  state=found(stage=1, playerarrived=1, table=0x80, firstspot=FIRST_SPOT, playerspot=PLAYER_SPOT,
                              baezspot=BAEZ_SPOT)), hero(at=[3005, 3170, 0])]
    add('baez.run.throw', chars, ticks=60, props=[TABLE])


TARGETS = {
    'melee-boss': dict(fixture='slots/combat/melee_attack.py', cases=boss_cases, compare=compare,
                       port_fields=port_fields, unit=lambda r: 1),
}
