"""Kata M4 for retail_ab.py: combat walking with someone else about
(docs/gameplay/COMBAT_DOJO.md).

`combat-go-world` calls Go `0x4ce350` as M3's `combat-go` does, in a world
where perception runs as original code (M9b): the walker has a target and
a second enemy stands round the direction held. Go looks for whoever is in
that direction (FindCharacters (1, -1, angle, 32, ENEMY|HEAR|SEE)) and
takes him for the new target unless he is more than 47 away and no nearer
than the current one; it only looks while the current target is valid
(docs/gameplay/forensics/COMBAT_MOVEMENT.md §2, step 5).

The cases: the walker the player or a monster; the direction held; the
newcomer on the cone's edges and either side of 47 and of the current
target's distance; the current target valid, dead or none; the newcomer
no enemy, quiet and unseen, or under the invisibility spell. Compared as
M3's: the result, the blocks, the target, the memory, the draws.
"""
from __future__ import annotations

import itertools
from pathlib import Path

from combat_targets import _char, _toward, finish, port_fields
from targets_move import compare_move
from targets_perceive import SENSES, _who

PLAYER, CHARACTER = 11, 12
FRAME = 5000
ME_AT = [2000, 2000, 0]
TAPE = [7, 30000, 15000, 2, 11, 400, 9000, 3]


def goworld_cases(data: Path, workdir: Path) -> list[dict]:
    cases = []

    def world(mover, held, target, other, *, other_enemy=True, other_noise=40, other_glimpse=100,
              other_invisible=False):
        """The walker facing its target, the target (bearing, distance,
        alive) or none, the newcomer (bearing off the direction held,
        distance)."""
        objclass, typ = (PLAYER, 'Locke') if mover == 'player' else (CHARACTER, 'Araknid')
        foe_class, foe_type = (CHARACTER, 'Araknid') if mover == 'player' else (PLAYER, 'Locke')
        me = _char('Me', objclass, ME_AT, target[0] if target else held,
                   root_obj='Target' if target is not None else None, type=typ)
        me['stats'] = dict(health=100, sleeping=0, aggressive=1)
        me['chardata'].update(SENSES, enemies='Target,Other' if other_enemy else 'Target')
        chars = [me]
        if target is not None:
            tb, td, alive = target
            chars.append(_who('Target', foe_class, _toward(ME_AT, tb, td), (tb + 128) & 0xff, type=foe_type,
                              glimpse=100, noise=40, playerstate=1,
                              stats=dict(health=100 if alive else 0, aggressive=1)))
        ob, od = other
        chars.append(_who('Other', CHARACTER, _toward(ME_AT, ob & 0xff, od), (ob + 128) & 0xff, type='Araknid',
                          glimpse=other_glimpse, noise=other_noise, invisiblespell=int(other_invisible),
                          stats=dict(health=100, aggressive=1)))
        return chars

    def case(name, chars, held):
        cases.append(dict(name=f'goworld.{name}', call='go', perception=True,
                          globals=dict(combatface=1, frame=FRAME), self='Me', angle=held, chars=chars,
                          walls=[], tape=TAPE))

    # The newcomer round the direction held, near and far (Distance is edge
    # to edge: two radii of 16 off, so 79-80 straddle Go's 47), the target
    # at the bearing the walker faces, 100 away or 60 (nearer than the
    # newcomer, where the 47 alone decides).
    for mover, held, off, od, (td, tstate) in itertools.product(
            ('player', 'monster'), (0, 96), (0, 20, 31, 32, -33), (30, 77, 78, 79, 80, 81, 82, 99, 100, 101, 250),
            ((100, 'valid'), (60, 'valid'), (100, 'dead'), (0, 'none'))):
        target = None if tstate == 'none' else (64, td, tstate == 'valid')
        chars = world(mover, held, target, (held + off, od))
        case(f'{mover}.a{held}.off{off}.d{od}.{tstate}{td if target else ""}', chars, held)
    # Who isn't taken: no enemy, quiet and dark, invisible.
    for mover, (label, kw) in itertools.product(
            ('player', 'monster'),
            (('notenemy', dict(other_enemy=False)), ('quiet', dict(other_noise=0, other_glimpse=0)),
             ('invisible', dict(other_invisible=True)))):
        for od in (30, 120):
            chars = world(mover, 0, (64, 100, True), (0, od), **kw)
            case(f'{mover}.{label}.d{od}', chars, 0)
    return finish(cases)


TARGETS = {
    'combat-go-world': dict(fixture='slots/combat/combat_call.py', cases=goworld_cases, compare=compare_move,
                            port_fields=port_fields, unit=lambda r: 1),
}
