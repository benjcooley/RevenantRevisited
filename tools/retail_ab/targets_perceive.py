"""Kata M9b for retail_ab.py: how a character perceives the others
(docs/gameplay/COMBAT_DOJO.md).

`combat-perceive` calls, as original code on the retail side, one of:

- IsEnemy `0x4c89c0`: idle players, teams and the player-killer bit
  between players, the one fighting me, ENEMIES by name, type and group,
  aggression;
- CanSeeCharacter `0x4cd540`: from eye to eye, centre to centre, the sight
  range and angle, the line of sight, the glimpse against Sight `0x4cdb30`
  (its draw), infravision and light-blindness, asleep, invisible;
- FindCharacters `0x4cd690`: a small world through the map iterator, each
  filter (enemies, hearing, sight), the remembered characters (HasSeenMe's
  45 seconds, SetHasSeen), the angle cone and its score, ties, the list's
  head;
- Hearing `0x4cda80` and Sight `0x4cdb30` alone, round their edges.

The line of sight is the case's (`walls`, both sides). Compared: the
result, FindCharacters' list, the perceiver's memory, the draws, the
seams (the pure queries left out, as in M7).
"""
from __future__ import annotations

import itertools
from pathlib import Path

from combat_targets import _char, _toward, finish, port_fields
from targets_move import compare_move

PLAYER, CHARACTER = 11, 12
ENEMY, HEAR, SEE = 1, 2, 4
OF_INVISIBLE = 0x80
CF_INFRAVISION, CF_LIGHTBLIND = 4, 8
PK = 1 << 24                       # a player killer (player state bit 24)
SEEN_FRAMES = 0x438                # HasSeenMe's 45 seconds
FRAME = 5000

ME_AT = [2000, 2000, 0]
SENSES = dict(sight=[30, 100, 320, 64], hearing=[10, 50, 320])


def _who(name, objclass, pos, facing, chardata=None, **extra):
    """A character with the senses (sight 320 within 64 of the facing,
    hearing 320) and whatever else the case gives."""
    who = 'Locke' if objclass == PLAYER else 'Araknid'
    stats = {'health': 100, 'sleeping': 0, 'aggressive': 0, **extra.pop('stats', {})}
    spec = _char(name, objclass, pos, facing, type=extra.pop('type', who), states=['combat'], **extra)
    spec['stats'] = stats
    spec['chardata'].update(SENSES, **(chardata or {}))
    return spec


def perceive_cases(data: Path, workdir: Path) -> list[dict]:
    cases = []

    def case(name, chars, call, **extra):
        cases.append(dict(name=f'perceive.{name}', call=call, perception=True,
                          globals=dict(combatface=1, frame=extra.pop('frame', FRAME)),
                          self=chars[0]['name'], chars=chars, **extra))

    # ---- IsEnemy ---------------------------------------------------------------
    # A monster judging the player and another monster: ENEMIES naming him,
    # his type, one of his groups, or nothing; he aggressive or not; he
    # fighting me (a combat or bow root on me), or someone else.
    for (tclass, tname), listed, aggressive, fighting in itertools.product(
            ((PLAYER, 'Hero'), (CHARACTER, 'Spider')), ('name', 'type', 'group', 'none', 'other-group'),
            (0, 1), ('no', 'me', 'bow-me', 'other')):
        enemies = dict(name=tname, type='Locke' if tclass == PLAYER else 'Araknid', group='beasts',
                       none='', **{'other-group': 'goblins'})[listed]
        me = _who('Watcher', CHARACTER, ME_AT, 64, chardata=dict(enemies=f'trolls,{enemies}' if enemies else ''))
        other = _who(tname, tclass, _toward(ME_AT, 64, 100), 192, chardata=dict(groups='beasts,dragons'),
                     stats=dict(health=100, aggressive=aggressive))
        bystander = _who('Bystander', CHARACTER, _toward(ME_AT, 128, 100), 0)
        if fighting != 'no':
            other['root']['obj'] = 'Watcher' if fighting != 'other' else 'Bystander'
            if fighting == 'bow-me':
                other['root'].update(name='bow', action=0x19)
        case(f'enemy.{tname}.{listed}.aggr{aggressive}.{fighting}', [me, other, bystander], 'is-enemy',
             target=tname)
    # Players: idle (state bit 2) whatever else; between two players, the
    # team names (empty, the same any case, different) and the PK bits.
    for idle, fighting in itertools.product((0, 1), (0, 1)):
        me = _who('Watcher', CHARACTER, ME_AT, 64, chardata=dict(enemies='Hero'))
        hero = _who('Hero', PLAYER, _toward(ME_AT, 64, 100), 192, playerstate=1 | (2 if idle else 0),
                    root_obj='Watcher' if fighting else None)
        case(f'enemy.player.idle{idle}.fighting{fighting}', [me, hero], 'is-enemy', target='Hero')
    for (my_team, his_team), my_pk, his_pk, listed in itertools.product(
            (('', ''), ('Red', 'red'), ('Red', 'Blue'), ('', 'Blue')), (0, 1), (0, 1), (0, 1)):
        me = _who('Hero', PLAYER, ME_AT, 64, team=my_team, playerstate=1 | (PK if my_pk else 0),
                  chardata=dict(enemies='Rival' if listed else ''))
        rival = _who('Rival', PLAYER, _toward(ME_AT, 64, 100), 192, team=his_team,
                     playerstate=1 | (PK if his_pk else 0), root_obj='Hero')
        case(f'enemy.pvp.{my_team or "-"}.{his_team or "-"}.pk{my_pk}{his_pk}.listed{listed}', [me, rival],
             'is-enemy', target='Rival')

    # ---- CanSeeCharacter ---------------------------------------------------------
    # The target round the watcher (facing 64) at 100 and 300, near and past
    # the range (centre to centre: 330 is past 320 though the edges are
    # within it), the glimpse it gave off round Sight's 99/100 edge, the
    # draw that decides it.
    for bearing, dist, glimpse, roll in itertools.product(
            range(0, 256, 32), (100, 300, 330), (0, 98, 99, 100, 150), (0, 32767)):
        me = _who('Watcher', CHARACTER, ME_AT, 64)
        other = _who('Hero', PLAYER, _toward(ME_AT, bearing, dist), 0, glimpse=glimpse, playerstate=1)
        case(f'see.b{bearing}.d{dist}.g{glimpse}.r{roll}', [me, other], 'can-see', target='Hero', angle=-1,
             tape=[roll])
    # The angle given (0 is the facing too: below 1), heights apart (the
    # distance is flat), a wall, infravision, light-blind, asleep,
    # invisible, the sight angle edge.
    variants = dict(angle0=dict(angle=0), angle200=dict(angle=200), angle100=dict(angle=100),
                    high=dict(z=400), wall=dict(walls=[['Watcher', 'Hero']]), wall_back=dict(walls=[['Hero', 'Watcher']]),
                    infra=dict(flags=CF_INFRAVISION), blind=dict(flags=CF_LIGHTBLIND), asleep=dict(sleeping=1),
                    invisible=dict(objflags=OF_INVISIBLE), cone=dict(sightangle=20))
    for (label, v), glimpse in itertools.product(variants.items(), (0, 100)):
        chardata = dict(flags=v.get('flags', 0))
        if 'sightangle' in v:
            chardata['sight'] = [30, 100, 320, v['sightangle']]
        me = _who('Watcher', CHARACTER, ME_AT, 64, chardata=chardata,
                  stats=dict(health=100, sleeping=v.get('sleeping', 0)))
        at = _toward(ME_AT, 90, 150)
        at[2] = v.get('z', 0)
        other = _who('Hero', PLAYER, at, 0, glimpse=glimpse, playerstate=1, objflags=v.get('objflags', 0))
        case(f'see.{label}.g{glimpse}', [me, other], 'can-see', target='Hero', angle=v.get('angle', -1),
             walls=v.get('walls', []), tape=[16384])

    # ---- FindCharacters ----------------------------------------------------------
    # A watcher (facing 64) and a world round him: the player ahead, a
    # friend, a spider on his ENEMIES list, one under the invisibility
    # spell, a dead enemy, one past the range, one behind him; each filter,
    # ranges, the cone, the list's length.
    def world(memory=(), noise=0, glimpse=100, wall=False):
        me = _who('Watcher', CHARACTER, ME_AT, 64, chardata=dict(enemies='Hero,Spider,Corpse'),
                  hasseen=list(memory))
        chars = [me,
                 _who('Hero', PLAYER, _toward(ME_AT, 70, 120), 192, glimpse=glimpse, noise=noise, playerstate=1),
                 _who('Friend', CHARACTER, _toward(ME_AT, 60, 60), 0, glimpse=100, noise=50),
                 _who('Spider', CHARACTER, _toward(ME_AT, 100, 200), 0, glimpse=60, noise=80,
                      stats=dict(health=100, aggressive=1)),
                 _who('Ghost', CHARACTER, _toward(ME_AT, 64, 90), 0, glimpse=100, noise=100, invisiblespell=1,
                      stats=dict(health=100, aggressive=1)),
                 _who('Corpse', CHARACTER, _toward(ME_AT, 50, 80), 0, glimpse=100, noise=100,
                      stats=dict(health=0, aggressive=1)),
                 _who('Far', CHARACTER, _toward(ME_AT, 64, 400), 0, glimpse=100, noise=100),
                 _who('Behind', CHARACTER, _toward(ME_AT, 192, 100), 0, glimpse=100, noise=0)]
        return chars, ([['Watcher', 'Hero']] if wall else [])

    queries = [(-1, -1, 32), (0x200, -1, 32), (100, -1, 32), (-1, 64, 32), (-1, 64, 8), (-1, 192, 64),
               (0x200, 70, 0x20), (150, 0, 128)]
    for (rng, angle, cone), flags, most in itertools.product(
            queries, (0, ENEMY, HEAR, SEE, HEAR | SEE, ENEMY | HEAR | SEE, ENEMY | SEE), (1, 3, 8)):
        chars, walls = world(noise=40)
        case(f'find.r{rng}.a{angle}.c{cone}.f{flags}.n{most}', chars, 'find-characters', max=most, range=rng,
             angle=angle, anglerange=cone, flags=flags, walls=walls, tape=[7, 30000, 15000, 2])
    # Memory: neither heard nor seen (the player quiet, in the dark, or
    # behind a wall), remembered recently, long ago, or not at all; the
    # memory full (the oldest replaced).
    for when, noise, glimpse, wall in itertools.product(
            ('none', 'recent', 'edge', 'old', 'full'), (0, 40), (0, 100), (0, 1)):
        memory = dict(none=[], recent=[['Hero', FRAME - 100, 0]], edge=[['Hero', FRAME - SEEN_FRAMES + 1, 1]],
                      old=[['Hero', FRAME - SEEN_FRAMES, 0]],
                      full=[[n, FRAME - 10 * i, i & 1] for i, n in
                            enumerate(['Friend', 'Spider', 'Far', 'Behind', 'Ghost', 'Corpse', 'Friend', 'Spider'])])[when]
        chars, walls = world(memory=memory, noise=noise, glimpse=glimpse, wall=bool(wall))
        case(f'find.memory.{when}.n{noise}.g{glimpse}.w{wall}', chars, 'find-characters', max=8, range=-1, angle=-1,
             anglerange=32, flags=ENEMY | HEAR | SEE, walls=walls, tape=[1, 2, 3, 4, 5, 6, 7, 8])
    # The map's order: the head of the list (the first found is never
    # scored) and what is displaced to the end.
    for order in (['Watcher', 'Far', 'Hero', 'Friend', 'Spider'], ['Spider', 'Friend', 'Hero', 'Watcher'],
                  ['Hero', 'Spider', 'Friend', 'Behind', 'Ghost', 'Corpse', 'Far', 'Watcher']):
        for (angle, cone), most in itertools.product(((-1, 32), (64, 64), (100, 16)), (1, 2, 8)):
            chars, _ = world(noise=40)
            case(f'find.order.{"-".join(n[0] for n in order)}.a{angle}.c{cone}.n{most}', chars, 'find-characters',
                 max=most, range=-1, angle=angle, anglerange=cone, flags=0, nearby=order)
    # Asleep: hearing halved, sight none; light-blind, infravision; no
    # room (max 0).
    for label, extra in (('asleep', dict(stats=dict(health=100, sleeping=1))),
                         ('infra', dict(chardata=dict(flags=CF_INFRAVISION))),
                         ('blind', dict(chardata=dict(flags=CF_LIGHTBLIND)))):
        for flags in (HEAR, SEE, ENEMY | HEAR | SEE):
            chars, _ = world(noise=40, glimpse=30)
            for key in ('stats', 'chardata'):
                chars[0][key] = {**chars[0][key], **extra.get(key, {})}
            case(f'find.{label}.f{flags}', chars, 'find-characters', max=8, range=-1, angle=-1, anglerange=32,
                 flags=flags, tape=[32767, 0, 16000, 9])
    chars, _ = world()
    case('find.max0', chars, 'find-characters', max=0, range=-1, angle=-1, anglerange=32, flags=ENEMY)
    # Ties and the cone's edge: two alike either side of the facing (the
    # same score), a third further along it; every order the map could
    # give them in, the cone narrowing past the pair.
    for order in itertools.permutations(['Left', 'Right', 'Ahead']):
        for (angle, cone), most in itertools.product([(-1, 32), (64, 64)] + [(64, c) for c in range(12, 21)],
                                                     (1, 2, 3)):
            chars = [_who('Watcher', CHARACTER, ME_AT, 64),
                     _who('Left', CHARACTER, _toward(ME_AT, 48, 120), 0),
                     _who('Right', CHARACTER, _toward(ME_AT, 80, 120), 0),
                     _who('Ahead', CHARACTER, _toward(ME_AT, 64, 200), 0)]
            case(f'find.tie.{"".join(n[0] for n in order)}.a{angle}.c{cone}.n{most}', chars, 'find-characters',
                 max=most, range=-1, angle=angle, anglerange=cone, flags=0, nearby=['Watcher', *order])

    # The cone's edge scores its distance once (the +1): one on the edge far
    # off, then one just inside it near, after a first (never scored).
    for cone, most in itertools.product(range(12, 21), (1, 3)):
        chars = [_who('Watcher', CHARACTER, ME_AT, 64),
                 _who('First', CHARACTER, _toward(ME_AT, 64, 150), 0),
                 _who('Edge', CHARACTER, _toward(ME_AT, 48, 250), 0),
                 _who('Near', CHARACTER, _toward(ME_AT, 49, 60), 0)]
        case(f'find.edge.c{cone}.n{most}', chars, 'find-characters', max=most, range=-1, angle=64, anglerange=cone,
             flags=0)

    # ---- Hearing, Sight ------------------------------------------------------------
    # Alone: distances round 0, hearing's radius and 32, the ranges' edges
    # (hearing's moved in by them); asleep; the draw's ends.
    for kind, dist, asleep, roll in itertools.product(
            ('hearing', 'sight'), (-10, 0, 1, 47, 48, 49, 100, 319, 320, 321, 360, 368, 369, 400), (0, 1),
            (0, 16384, 32767)):
        me = _who('Watcher', CHARACTER, ME_AT, 64, stats=dict(sleeping=asleep))
        case(f'{kind}.d{dist}.s{asleep}.r{roll}', [me], kind, dist=dist, tape=[roll])
    return finish(cases)


TARGETS = {
    'combat-perceive': dict(fixture='slots/combat/combat_call.py', cases=perceive_cases, compare=compare_move,
                            port_fields=port_fields, unit=lambda r: 1),
}
