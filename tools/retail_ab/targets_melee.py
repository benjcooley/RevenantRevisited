"""Combat dojo melee targets for retail_ab.py (docs/gameplay/COMBAT_DOJO.md
C2-C5; forensics COMBAT_ATTACK_CHOICE.md, COMBAT_HIT.md).

`melee-attack-choice` (kata C3): the attack search and what it fixes --
IsValidAttack over every attack of real char.def tables in worlds that
take each of its gates both ways, the searches (button, percentage,
interactive), DoAttack, and the callers (ButtonAttack with its chain,
same-button and counter rules, ButtonAction, RandomAttack,
SpecificAttack), every random branch steered with RNG tapes.

`melee-hit` (kata C4): ResolveAttack, ResolveHit and OnAttacked, with
Damage a seam (recorded with the hit block by meaning).

`melee-damage` (kata C2): Damage itself -- the gates and cheats, the
amount by type, the held victim, the death and impact blocks (the given
block, the CHARIMPACT search by side, state and share, the stock names),
the snaps and turns, the retreat, the kills and deaths the player counts.

`melee-resolvers` (kata C5): ResolveImpact, ResolveBlock, ResolveDead on
the doing block (blood and flash, the loops, the way back to the root),
Block (the gates, the monster's timing, the duration), StopBlock, Dodge
(the gates, the roll by facing and direction).

`melee-ai` (kata C3b): one tick of AI() -- the gates, the target kept or
found, the attack branch, the move branch (retreat, standing, stepping:
stop, chase, search), the retreat state, the boss kind.

`melee-pulse` (kata C6, forensics CHARACTER_PULSE.md): ticks of the
character's Pulse and the player's -- recovery (the share by the stamp's
age, the carry, the 50000 pull, the 32-bit wrap, the player's per-minute
percentages), running fatigue, poison through Damage, the alreadydead
cheat, Died and the corpse (its death, the fade, the removal), the blood of
the scripted deaths, the impale holds, the invoke / burn / flail timers,
the player's auto-combat, the chain, the AI gate, the exit mark, the
player's clock and death screen; long runs for how fast it all goes.

`melee-exp` (kata C7, PLAYER_STATS.md §7): KillExp, AwardKillExp with the
level-up, AwardSkillExp and AddSkillExp, AwardStealthExp over the victim's
45-second memory.

Retail: tools/retail_runtime/slots/combat/melee_attack.py; port:
src/retailab_melee.cpp (`Revenant --retail-ab=melee-*`). The compare is
combat_targets.compare (every field; the seams aligned as sequences).

The character data are the shipped char.def and rules.def as the port
loads them: the combat-data dump of the archives' files (kata D1, which
holds the port's parse to retail's), taken from the port binary that
build/ holds when the cases are made.
"""
from __future__ import annotations

import copy
import itertools
import json
import os
import subprocess
from pathlib import Path

from combat_targets import _toward, compare, finish, port_fields

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[1]
PORT = Path(os.environ.get('RETAIL_AB_PORT', REPO / 'build' / 'Revenant'))

# Attack flags (rules.h CA_*), impact flags (CAI_*).
CA_SPECIAL, CA_RESPONSE, CA_DEATH = 0x1, 0x2, 0x8
CA_CHAIN, CA_AUTOCOMBO, CA_FATIGUEATTACK = 0x4000, 0x8000, 0x10000
CA_ATTACKDOWN, CA_ATTACKSTUN, CA_MOVING = 0x40000, 0x80000, 0x100000
CA_MAGICATTACK, CA_PLAYANIM, CA_INTERACTIVE = 0x800000, 0x1000000, 0x2000000
CA_SNEAKMODE, CA_WALKMODE, CA_BOWMODE, CA_RUNNING, CA_ACTION = 0x4000000, 0x8000000, 0x10000000, 0x20000000, 0x40000000
CAI_DEATH, CAI_INTERACTIVE = 0x4, 0x80


# ---- The shipped data ------------------------------------------------------------

_DATA = {}


def shipped(data: Path, workdir: Path) -> dict:
    """The shipped rules and characters as the port parses them (D1's dump)."""
    if 'shipped' in _DATA:
        return _DATA['shipped']
    from targets_data import data_cases, port_fields as data_fields
    root = (workdir / 'data').resolve()
    case = next(c for c in data_cases(data, root) if c['name'] == 'shipped')
    tsv, out = root / 'shipped.tsv', root / 'shipped.port.jsonl'
    tsv.write_text('\t'.join([case['name']] + data_fields(case)) + '\n')
    subprocess.run([str(PORT), '--retail-ab=combat-data', f'--ab-cases={tsv}', f'--ab-out={out}'],
                   cwd=root, check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                   env=dict(os.environ, REVENANT_DATA_PATH=os.environ.get('REVENANT_DATA_PATH', str(data))))
    record = json.loads(out.read_text().splitlines()[0])
    if not record.get('ok'):
        raise RuntimeError(f"combat-data dump failed: {record.get('error')}")
    result = record['result']
    rules = result['rules']
    _DATA['shipped'] = dict(
        rules={k: rules[k] for k in ('tohitcenter', 'tohitrangechar', 'tohitrangeplyr', 'tohitblock', 'tohitface',
                                     'tohitdamage', 'statlevels', 'healthrecovval', 'fatiguerecovval',
                                     'manarecovval', 'healthrecovrate', 'fatiguerecovrate', 'manarecovrate',
                                     'poisondamageval', 'poisondamagerate', 'exp', 'skillexp')},
        chars={c['name']: c for c in result['chars']})
    return _DATA['shipped']


# ---- Worlds ------------------------------------------------------------------------

ME_AT = [1000, 1000, 0]


def chardata(record: dict) -> dict:
    """A character's data as the case carries it: the dump's record whole."""
    return copy.deepcopy(record)


def attack_states(cd: dict) -> list:
    """Every animation the attack table names, as the character's states."""
    names = []
    for ad in cd['attacks']:
        names.append(ad['name'])
    return names


def make_char(name, objclass, pos, facing, cd, *, typ=None, states=(), root=None, doing=None, stats=None,
              classstats=None, attackstate=None, weapon=None, resists=None, charflags=0, frame=0, ident=0,
              desired=None, objflags=0):
    spec = {'name': name, 'type': typ or cd['name'], 'class': objclass, 'pos': list(pos), 'facing': facing,
            'moveangle': facing, 'charflags': charflags, 'objflags': objflags, 'frame': frame, 'id': ident,
            'stats': stats or {}, 'classstats': classstats or {'radius': 16, 'value': 1},
            'chardata': cd, 'states': list(states), 'root': root or dict(name='combat', action=3)}
    if doing is not None:
        spec['doing'] = doing
    if desired is not None:
        spec['desired'] = desired
    if attackstate is not None:
        spec['attackstate'] = attackstate
    if objclass == 11:
        spec['weapon'] = weapon or {'type': 0, 'damage': cd['weapondamage']}
        spec['resists'] = resists or [0] * 10
    return spec


PLAYER_STATS = dict(health=100, fatigue=78, mana=50, damagemod=0, level=3, attacklevel=5, acbonus=2, edgebonus=10,
                    strn=14, cons=12, agil=15, rflx=13, mind=14, luck=16,
                    attack=4, defense=3, invoke=2, hands=5, knife=4, sword=6, bludgeons=3, axes=2, bows=1,
                    stealth=1, lockpick=0)
MONSTER_STATS = dict(health=30, fatigue=104, mana=0, damagemod=0)


def duel(data, me, target, *, dist=20, bearing=64, me_stats=None, target_stats=None, me_states=None,
         target_states=None, me_root=None, me_doing=None, target_doing=None, me_state=None, target_state=None,
         me_flags=0, target_flags=0, me_frame=0, weapon=None, target_root=None, me_value=1, target_value=1,
         me_desired=None, me_objflags=0, target_objflags=0):
    """Two characters: `me` facing `target`, `dist` apart edge to edge
    (radius 16 each), its combat root on the target."""
    s = shipped(data, None)
    mcd, tcd = chardata(s['chars'][me]), chardata(s['chars'][target])
    mcls = 11 if me in ('Locke', 'Bayne', 'Morganna', 'Navarro') else 12
    tcls = 11 if target in ('Locke', 'Bayne', 'Morganna', 'Navarro') else 12
    tpos = _toward(ME_AT, bearing, dist + 32)
    mname, tname = 'Me', 'Target'
    me_spec = make_char(mname, mcls, ME_AT, bearing, mcd, states=me_states or (['combat', 'walk'] + attack_states(mcd)),
                        root=me_root or dict(name='combat', action=3, angle=bearing, moveangle=bearing, obj=tname),
                        doing=me_doing, stats=dict(PLAYER_STATS if mcls == 11 else MONSTER_STATS, **(me_stats or {})),
                        classstats={'radius': 16, 'value': me_value}, attackstate=me_state, weapon=weapon,
                        charflags=me_flags, frame=me_frame, ident=0x10, desired=me_desired, objflags=me_objflags)
    tg_spec = make_char(tname, tcls, tpos, (bearing + 128) & 0xff, tcd, states=target_states or ['combat'],
                        root=target_root or dict(name='combat', action=3, angle=(bearing + 128) & 0xff, obj=mname),
                        doing=target_doing, stats=dict(PLAYER_STATS if tcls == 11 else MONSTER_STATS, **(target_stats or {})),
                        classstats={'radius': 16, 'value': target_value}, attackstate=target_state,
                        charflags=target_flags, ident=0x20, objflags=target_objflags)
    return [me_spec, tg_spec], s['rules']


def case(name, call, chars, rules, *, self_='Me', args=None, calls=None, tape=None, seed=1, frame=100, **extra):
    c = dict(name=name, call=call, globals=dict(combatface=1, frame=frame, nahkranoth=extra.pop('nahkranoth', 0),
                                               alreadydead=extra.pop('alreadydead', 0)),
             rules=rules, chars=chars, self=self_)
    if args is not None:
        c['args'] = args
    if calls is not None:
        c['calls'] = calls
    if tape is not None:
        c['tape'] = tape
    c['seed'] = seed
    c.update(extra)
    return c


# ---- C3: attack choice ---------------------------------------------------------------

# Pairs (attacker, target) whose tables between them hold every shape the
# shipped char.def has: plain, chain / autocombo, special interactive with
# held and death impacts (snap distances), fatigue attacks, sneak and walk
# mode, PLAYANIM, MAGICATTACK with each condition, death attacks.
PAIRS = [('Araknid', 'Locke'), ('Locke', 'Araknid'), ('Locke', 'Rahul'), ('Jong', 'Locke'),
         ('Pale Ogrok', 'Locke'), ('Kantha', 'Locke'), ('Yhagoro', 'Locke'), ('Navarro', 'Rahul'),
         ('Bayne', 'Pale Ogrok'), ('Zombie', 'Locke'), ('Spider Queen', 'Locke'), ('Rahul', 'Locke'),
         ('Arakna', 'Locke'), ('Morganna', 'Skeleton')]
PLAYERS = ('Locke', 'Bayne', 'Morganna', 'Navarro')


def impact_states(attacker_cd: dict, prefix: str) -> list:
    """What a target needs to play every impact of the attacker's table:
    the names raw and prefixed, their loops, the death fallbacks."""
    names = []
    for ad in attacker_cd['attacks']:
        for imp in ad.get('impacts', []):
            names += [imp['name'], prefix + imp['name']]
            if imp['loopname']:
                names.append(imp['loopname'])
    names += [prefix + 'dead', 'combat to ' + prefix + 'dead', 'dead']
    out = []
    for n in names:
        if n.lower() not in [o.lower() for o in out]:
            out.append(n)
    return out


def edges(ad: dict) -> list:
    """The distances an attack's range test turns on."""
    lo, hi = ad['mindist'], ad['maxdist']
    return sorted({lo - 1, lo, hi, hi + 1, (lo + hi) // 2})


def thresholds(cd: dict) -> list:
    """Fatigue values each side of every attack's fatigue / maxfatigue."""
    vals = {0}
    for ad in cd['attacks']:
        for k in ('fatigue', 'maxfatigue'):
            v = ad.get(k)
            if v:
                vals |= {v - 1, v, v + 1}
    return sorted(vals)


def iva_calls(cd, targ='Target', **extra):
    calls = []
    for i, ad in enumerate(cd['attacks']):
        for tdist in edges(ad):
            calls.append(dict(attack=i, tdist=tdist, targ=targ, dmgpcnt=37, **extra))
    return calls


def block(name, action, **kw):
    return dict(name=name, action=action, **kw)


def chain_parent(cd):
    """The first chain link and the attack it follows (by index)."""
    names = {ad['name'].lower(): i for i, ad in enumerate(cd['attacks'])}
    for i, ad in enumerate(cd['attacks']):
        ch = ad.get('chainname')
        if ch and ch.lower() in names and ad['flags'] & (CA_CHAIN | CA_AUTOCOMBO):
            return i, names[ch.lower()]
    return None


def first_with(cd, flag):
    for i, ad in enumerate(cd['attacks']):
        if ad['flags'] & flag:
            return i
    return None


def synthetic(record: dict, **changes) -> dict:
    """A real attack record with some fields changed: the shapes the shipped
    data never uses but the code still reads."""
    r = copy.deepcopy(record)
    for k, v in changes.items():
        if k == 'add_flags':
            r['flags'] |= v
        else:
            r[k] = v
    return r


def impact(name, flags, loop='', **kw):
    return dict(dict(name=name, index=0, flags=flags, loopname=loop, looptime=12, damagemin=0, damagemax=10000,
                     snapdist=0, snaptime=0), **kw)


def magic_record(base: dict, condition: int, value: int, pcnt=50) -> dict:
    return dict(name=base['name'], flags=CA_MAGICATTACK, button=0, attackpcnt=pcnt, mindist=1, maxdist=640,
                spellname='heal3', spellsource=[-1, -1, -1], condition=condition, conditionvalue=value)


def synthetic_cases(data, add):
    """IsValidAttack over attack shapes the shipped tables don't have."""
    s = shipped(data, None)
    for me, tg in (('Araknid', 'Locke'), ('Locke', 'Araknid')):
        key = f"{me}.vs.{tg}"
        base_cd = s['chars'][me]
        a0 = copy.deepcopy(next(ad for ad in base_cd['attacks'] if not ad['flags'] & (CA_MAGICATTACK | CA_PLAYANIM)))
        a0.update(mindist=0, maxdist=100)
        prefix = 'c'

        def run(label, records, calls=None, me_states_extra=(), **world_kw):
            cd = copy.deepcopy(base_cd)
            cd['attacks'] = records
            mstates = ['combat', 'walk'] + [r['name'] for r in records] + list(me_states_extra)
            tstates = world_kw.pop('target_states', None)
            if tstates is None:
                tstates = ['combat'] + impact_states(cd, prefix)
            extra = world_kw.pop('case_extra', {})
            own_rules = world_kw.pop('rules', None)
            chars, rules = duel(data, me, tg, me_states=mstates, target_states=tstates,
                                me_state=dict(dict(nextattack=0, magictimer=0), **world_kw.pop('me_state', {})),
                                **world_kw)
            chars[0]['chardata'] = cd
            rules = own_rules or rules
            add(f'iva.synth.{key}.{label}', 'iva', chars, rules,
                calls=calls or [dict(attack=i, tdist=50, targ='Target', dmgpcnt=21) for i in range(len(records))],
                **extra)

        bow = synthetic(a0, add_flags=CA_BOWMODE)
        for root in ('bow', 'combat', 'bowrun'):
            action = 0x19 if root.startswith('bow') else 3
            run(f'bowmode.{root}', [bow], me_root=block(root, action, obj='Target'))
        running = synthetic(a0, add_flags=CA_RUNNING)
        for root, action in (('combatrun', 3), ('handrun', 3), ('combat', 3), ('bowrun', 0x19), ('bow', 0x19),
                             ('run', 3), ('walk', 1)):
            run(f'running.{root}', [running, synthetic(running, add_flags=CA_BOWMODE)],
                me_root=block(root, action, obj='Target'))
        for flag, label, action in ((CA_ATTACKSTUN, 'stun', 0xd), (CA_ATTACKDOWN, 'down', 0xe)):
            rec = synthetic(a0, add_flags=flag)
            run(f'{label}.yes', [rec], target_doing=block('cx', action))
            run(f'{label}.no', [rec], target_doing=block('cx', 0xc))
            run(f'{label}.notarget', [rec], calls=[dict(attack=0, tdist=50, targ=None)])
        resp = synthetic(a0, responsename='cblock', add_flags=CA_RESPONSE)
        run('response.yes', [resp], target_doing=block('cblock', 8))
        run('response.no', [resp], target_doing=block('cswing', 7))
        run('response.notarget', [resp], calls=[dict(attack=0, tdist=50, targ=None)])
        # MAGICATTACK conditions, each side of its value; a timer still running.
        for cond in (1, 2, 3, 4, 5, 6, 0):
            for stat in ('health', 'mana'):
                for v in (99, 100, 101):
                    run(f'magic.c{cond}.{stat}{v}', [magic_record(a0, cond, 100)], me_stats={stat: v})
        run('magic.timer1', [magic_record(a0, 1, 0)], me_state=dict(magictimer=1))
        run('magic.timerneg', [magic_record(a0, 1, 0)], me_state=dict(magictimer=-1))
        # Deaths by name: the built fallbacks, the "dead" loop.
        deathy = []
        for nm, loop in (('combat to dead', ''), ('combat to dead', 'dead'), ('combat to dead', 'xloop'),
                         ('dead', ''), ('dead', 'dead'), ('dead', 'xloop'), ('xdeath', ''), ('xdeath', 'dead')):
            deathy.append(synthetic(a0, add_flags=CA_DEATH, impacts=[impact(nm, CAI_DEATH, loop)]))
        tp = 'h' if tg == 'Locke' else 'c'
        weak = dict(target_stats=dict(health=1))
        for label, states in (
                ('none', ['combat']),
                ('raw', ['combat', 'combat to dead', 'dead', 'xdeath']),
                ('built', ['combat', 'combat to cdead', 'cdead']),
                ('built.loops', ['combat', 'combat to cdead', 'cdead', 'dead', 'xloop']),
                ('built.loop0', ['dead', 'combat', 'combat to cdead', 'cdead', 'xloop']),
                ('built.xloop0', ['xloop', 'combat', 'combat to cdead', 'cdead', 'dead']),
                ('built.cdead0', ['cdead', 'combat', 'combat to cdead', 'dead']),
                ('hand', ['hand', 'hand to hdead', 'hdead', 'dead', 'xloop'])):
            run(f'deathnames.{label}', deathy, target_states=states, **weak)
            if tg == 'Locke':
                run(f'deathnames.{label}.handroot', deathy, target_states=states,
                    target_root=block('hand', 3, obj='Me'), **weak)
        # Stun / knockdown impacts with loops; one at index 0.
        stunny = [synthetic(a0, impacts=[impact('imph', 1, 'sloop')]),
                  synthetic(a0, impacts=[impact('imph', 2, 'dloop'), impact('impl', 0)]),
                  synthetic(a0, impacts=[impact('imph', 0x81, 'sloop')])]
        for label, states in (('present', ['combat', 'cimph', 'cimpl', 'imph', 'sloop', 'dloop']),
                              ('loop0', ['sloop', 'combat', 'cimph', 'imph', 'dloop', 'cimpl']),
                              ('noloop', ['combat', 'cimph', 'imph', 'cimpl']),
                              ('noimpact', ['combat', 'sloop', 'dloop'])):
            run(f'loops.{label}', stunny, target_states=states)
        # Six impacts: retail tests the record one past them.
        six = synthetic(a0, add_flags=CA_DEATH,
                        impacts=[impact('imph', 0)] * 5 + [impact('xdeath', CAI_DEATH | CAI_INTERACTIVE)])
        run('six', [six], target_states=['combat', 'cimph', 'xdeath'], **weak)
        # The "w" prefix: a target fighting in its walk root.
        run('walkprefix.yes', [a0], target_states=['combat', 'walk', 'w' + a0['impacts'][0]['name'],
                                                    'c' + a0['impacts'][0]['name']],
            target_root=block('walk', 1, obj='Me'), target_flags=0x2000)
        run('walkprefix.no', [a0], target_states=['combat', 'walk', 'c' + a0['impacts'][0]['name']],
            target_root=block('walk', 1, obj='Me'), target_flags=0x2000)
        run('walkprefix.combatroot', [a0], target_states=['combat', 'c' + a0['impacts'][0]['name']],
            target_flags=0x2000)
        # The player's prefix from his root (a target's impact name).
        if tg == 'Locke':
            for root, action in (('hand', 3), ('handrun', 3), ('combatrun', 3), ('bow', 0x19), ('bowrun', 0x19),
                                 ('sneak', 1), ('walk', 1), ('run', 1), ('combat', 3), ('hand', 0x19)):
                states = ['combat', root] + [p + a0['impacts'][0]['name'] for p in
                                             ('h', 'hr', 'c', 'cr', 'b', 'br', 's', 'w', 'r', 't', 'tr')]
                run(f'prefix.{root}.{action}', [a0], target_states=states, target_root=block(root, action, obj='Me'))
                run(f'prefix.{root}.{action}.bare', [a0], target_states=['combat'],
                    target_root=block(root, action, obj='Me'))
        # To-hit out of the clamp; tier keys out of order.
        run('value.me20', [a0] * 3, me_value=20)
        run('value.targ20', [a0] * 3, target_value=20)
        rules = copy.deepcopy(s['rules'])
        rules['tohitdamage'] = [[-60, 10], [50, 20], [10, 30], [-20, 40], [-10, 50]]
        run('tierkeys', [a0] * 4, rules=rules,
            calls=[dict(attack=0, tdist=50, targ='Target', tohit=t, roll=r) for t, r in
                   ((50, 1), (50, 40), (50, 60), (50, 100), (10, 100), (100, 1))])
        # A target held in an impact with no interactive attack.
        mi = first_with(base_cd, CA_INTERACTIVE)
        if mi is not None:
            k = next((k for k, imp in enumerate(base_cd['attacks'][mi]['impacts']) if imp['flags'] & CAI_INTERACTIVE),
                     None)
            if k is not None:
                recs = [base_cd['attacks'][mi], a0]
                run('heldimpact.only', recs, target_doing=block('cheld', 0xc, impact=k, impact_attack=0,
                                                                impact_of='Me'))
                run('heldimpact.plain', recs, target_doing=block('cheld', 0xc, attack=1, attack_of='Me', impact=k,
                                                                 impact_attack=0, impact_of='Me'))
                run('heldimpact.self', recs, target_flags=0x80000,
                    target_doing=block('cheld', 0xc, impact=k, impact_attack=0, impact_of='Me'))


def full_world(data, me, tg, **kw):
    """A duel where each side has its own attacks and the impacts of the
    other's (so either can attack, as the counter needs), timers run out."""
    s = shipped(data, None)
    mcd, tcd = s['chars'][me], s['chars'][tg]
    state = dict(dict(nextattack=0, magictimer=0), **kw.pop('me_state', {}))
    tstate = dict(dict(nextattack=0, magictimer=0), **kw.pop('target_state', {}))
    mstates = ['combat', 'walk'] + attack_states(mcd) + impact_states(tcd, 'c')
    tstates = ['combat', 'walk'] + attack_states(tcd) + impact_states(mcd, 'c')
    return duel(data, me, tg, me_states=kw.pop('me_states', mstates), target_states=kw.pop('target_states', tstates),
                me_state=state, target_state=tstate, **kw)


def search_cases(data, add):
    """The searches and the calls that make an attack, on real tables."""
    s = shipped(data, None)
    armed = dict(nextattack=0, magictimer=0)

    def world(me, tg, **kw):
        return full_world(data, me, tg, **kw)

    # FindButtonAttack: every button, at the distances the tables turn on,
    # with and without a target; isaction never finds anything.
    for me, tg in (('Locke', 'Araknid'), ('Bayne', 'Pale Ogrok'), ('Navarro', 'Rahul'), ('Morganna', 'Skeleton')):
        for dist in (0, 5, 18, 19, 20, 40, 75, 80, 150, 300):
            chars, rules = world(me, tg, dist=dist)
            for button in range(0, 13):
                add(f'find-button.{me}.d{dist}.b{button}', 'find-button', chars, rules,
                    args=dict(button=button, dmgpcnt=33, targ='Target'))
        chars, rules = world(me, tg, dist=20)
        for button in (1, 2, 3, 10, 12):
            add(f'find-button.{me}.notarget.b{button}', 'find-button', chars, rules,
                args=dict(button=button, dmgpcnt=33, targ=None))
            add(f'find-button.{me}.action.b{button}', 'find-button', chars, rules,
                args=dict(button=button, dmgpcnt=33, targ='Target', isaction=1))
            add(f'find-button.{me}.preset.b{button}', 'find-button', chars, rules,
                args=dict(button=button, dmgpcnt=33, targ='Target', damage=9, tohit=60, roll=100))
            for label, kw in (('sneak', dict(me_root=block('sneak', 1, obj='Target'))),
                              ('weak', dict(target_stats=dict(health=1))),
                              ('tired', dict(me_stats=dict(fatigue=10)))):
                chars2, rules2 = world(me, tg, dist=20, **kw)
                add(f'find-button.{me}.{label}.b{button}', 'find-button', chars2, rules2,
                    args=dict(button=button, dmgpcnt=33, targ='Target'))
        cp = chain_parent(s['chars'][me])
        if cp:
            link, parent = cp
            chars, rules = world(me, tg, dist=20, me_state=dict(lastattack=parent, lastattackticks=95))
            add(f'find-button.{me}.chain', 'find-button', chars, rules,
                args=dict(button=s['chars'][me]['attacks'][link]['button'], dmgpcnt=33, targ='Target'))

    # FindPcntAttack: the monsters' random picks; the 2n tries running out.
    monsters = ('Araknid', 'Jong', 'Pale Ogrok', 'Kantha', 'Yhagoro', 'Arakna', 'Rahul', 'Spider Queen', 'Zombie')
    for me in monsters:
        n = len(s['chars'][me]['attacks'])
        for dist in (0, 5, 10, 30, 60):
            chars, rules = world(me, 'Locke', dist=dist)
            for pcnt in (1, 25, 50, 75, 100, 101):
                add(f'find-pcnt.{me}.d{dist}.p{pcnt}', 'find-pcnt', chars, rules, args=dict(pcnt=pcnt, dmgpcnt=21))
            # Steered picks: every index first.
            for i in range(n):
                add(f'find-pcnt.{me}.d{dist}.pick{i}', 'find-pcnt', chars, rules, args=dict(pcnt=1, dmgpcnt=21),
                    tape=[i] + list(range(n)) * 2)
        for label, kw in (('noroottarget', dict(me_root=block('combat', 3))),
                          ('bowroot', dict(me_root=block('bow', 0x19, obj='Target'))),
                          ('walkroot', dict(me_root=block('walk', 1, obj='Target'))),
                          ('nextattack1', dict(me_state=dict(nextattack=1))),
                          ('bare', dict(me_states=['combat'])),
                          ('weak', dict(target_stats=dict(health=1)))):
            chars, rules = world(me, 'Locke', dist=5, **kw)
            add(f'find-pcnt.{me}.{label}', 'find-pcnt', chars, rules, args=dict(pcnt=1, dmgpcnt=21))
        if n == 1:
            continue
    # One-attack table: no draw (random(0, 0)).
    cd = copy.deepcopy(s['chars']['Araknid'])
    cd['attacks'] = cd['attacks'][:1]
    chars, rules = world('Araknid', 'Locke', dist=5)
    chars[0]['chardata'] = cd
    add('find-pcnt.single', 'find-pcnt', chars, rules, args=dict(pcnt=1, dmgpcnt=21))
    # The tier compounding: the damage fixed once, re-tiered per candidate
    # that gets past the damage block and is refused after it.
    cd = copy.deepcopy(s['chars']['Araknid'])
    a0 = cd['attacks'][0]
    a0['flags'] |= CA_DEATH
    cd['attacks'] = [copy.deepcopy(a0), copy.deepcopy(a0), cd['attacks'][1]]
    for i, tape in enumerate(([0, 1, 2], [2, 0, 1], [1, 1, 2])):
        chars, rules = world('Araknid', 'Locke', dist=5)
        chars[0]['chardata'] = cd
        add(f'find-pcnt.compound.{i}', 'find-pcnt', chars, rules, args=dict(pcnt=1, dmgpcnt=21),
            tape=tape + [0, 30, 30] * 4)

    # FindInteractiveAttack.
    for me, tg in (('Pale Ogrok', 'Locke'), ('Kantha', 'Locke'), ('Locke', 'Araknid'), ('Navarro', 'Rahul'),
                   ('Bayne', 'Pale Ogrok')):
        for dist in (0, 10, 40):
            for label, kw in (('base', {}), ('held', dict(target_flags=0x80000)), ('weak', dict(target_stats=dict(health=1))),
                              ('noroottarget', dict(me_root=block('combat', 3)))):
                chars, rules = world(me, tg, dist=dist, **kw)
                for pcnt in (0, 50, 100):
                    add(f'find-interactive.{me}.d{dist}.{label}.p{pcnt}', 'find-interactive', chars, rules,
                        args=dict(pcnt=pcnt, dmgpcnt=21))


def making_cases(data, add):
    """DoAttack and the callers: ButtonAttack, ButtonAction, RandomAttack,
    SpecificAttack."""
    s = shipped(data, None)
    armed = dict(nextattack=0, magictimer=0)

    def world(me, tg, **kw):
        return full_world(data, me, tg, **kw)

    # DoAttack: each kind of record, an impact or none, refused, gated.
    for me, tg in (('Locke', 'Araknid'), ('Araknid', 'Locke'), ('Jong', 'Locke'), ('Pale Ogrok', 'Locke'),
                   ('Zombie', 'Locke'), ('Yhagoro', 'Locke')):
        mcd = s['chars'][me]
        picks = {}
        for i, ad in enumerate(mcd['attacks']):
            shape = (ad['flags'] & (CA_SPECIAL | CA_CHAIN | CA_AUTOCOMBO | CA_PLAYANIM | CA_MAGICATTACK | CA_INTERACTIVE),
                     bool(ad.get('chainname')), len(ad.get('impacts', [])))
            picks.setdefault(shape, i)
        for shape, i in picks.items():
            for label, kw, extra in (
                    ('base', {}, {}),
                    ('moving', dict(me_doing=block('combatf', 4, angle=40, moveangle=72, obj='Target')), {}),
                    ('noturn', dict(me_flags=4), {}),
                    ('priority', dict(me_doing=block('cswing', 7, obj='Target', flags=['priority'],
                                                     attack=0), me_desired=None), {}),
                    ('invoking', dict(me_doing=block('invoke', 0xb)), {}),
                    ('dead', dict(me_stats=dict(health=0)), {}),
                    ('iced', dict(me_objflags=0x2000000), {}),
                    ('paralysed', dict(me_objflags=0x800000), {}),
                    ('immobile', dict(me_objflags=1), {}),
                    ('requests', dict(me_state=dict(requestbits=7)), {}),
                    ('spell', {}, dict(spells=['heal3'])),
                    ('spell.fails', {}, dict(spells=['heal3'], cast_result=0))):
                chars, rules = world(me, tg, dist=10, **kw)
                for imp in sorted({-1, 0, len(mcd['attacks'][i].get('impacts', [])) - 1}):
                    add(f'do-attack.{me}.a{i}.{label}.i{imp}', 'do-attack', chars, rules,
                        args=dict(attack=i, impact=imp, damage=17, tohit=60, roll=44, targ='Target'), **extra)
            chars, rules = world(me, tg, dist=10)
            add(f'do-attack.{me}.a{i}.notarget', 'do-attack', chars, rules,
                args=dict(attack=i, impact=-1, damage=17, tohit=60, roll=44, targ=None))
        chars, rules = world(me, tg, dist=10)
        add(f'do-attack.{me}.outofrange', 'do-attack', chars, rules,
            args=dict(attack=len(mcd['attacks']), impact=-1, damage=1, tohit=1, roll=1, targ='Target'))

    # ButtonAttack: the target (the root's, else the one found), the chain
    # bank, the same-button rule and the counter.
    for me, tg in (('Locke', 'Araknid'), ('Bayne', 'Pale Ogrok'), ('Locke', 'Jong'), ('Navarro', 'Rahul')):
        mcd = s['chars'][me]
        cp = chain_parent(mcd)
        for button in (1, 2, 3, 4, 5, 10, 11, 12):
            for dist in (5, 20, 60):
                chars, rules = world(me, tg, dist=dist)
                add(f'button-attack.{me}.d{dist}.b{button}', 'button-attack', chars, rules, args=dict(button=button))
            chars, rules = world(me, tg, dist=20, me_root=block('combat', 3))
            add(f'button-attack.{me}.found.b{button}', 'button-attack', chars, rules, args=dict(button=button),
                found='Target')
            add(f'button-attack.{me}.nobody.b{button}', 'button-attack', chars, rules, args=dict(button=button))
            chars, rules = world(me, tg, dist=20, me_root=block('walk', 1))
            add(f'button-attack.{me}.walkroot.b{button}', 'button-attack', chars, rules, args=dict(button=button),
                found='Target')
            # The third press of a button: roll 100, and one in eleven the
            # monster counters (tape 0 then its search).
            for repeat, last in ((1, button), (2, button), (5, button), (2, button + 1)):
                for label, tape in (('nocounter', [5]), ('counter', [0, 20] + list(range(40))),
                                    ('counter11', [11, 3] + list(range(40)))):
                    chars, rules = world(me, tg, dist=10, me_state=dict(lastbutton=last, buttonrepeat=repeat),
                                         target_state=dict(nextattack=7))
                    add(f'button-attack.{me}.b{button}.last{last}.rep{repeat}.{label}', 'button-attack', chars, rules,
                        args=dict(button=button), tape=tape)
        if cp:
            link, parent = cp
            exp = mcd['attacks'][parent]['chainexptime']
            for hits in (0, 2, 3):
                for ago in (0, exp, exp + 1):
                    chars, rules = world(me, tg, dist=10, me_state=dict(lastattack=parent, lastattackticks=100 - ago,
                                                                        chainhits=hits))
                    add(f'button-attack.{me}.chainbank.h{hits}.ago{ago}', 'button-attack', chars, rules,
                        args=dict(button=mcd['attacks'][link]['button']))
        # Pressing on another player: no same-button rule.
        chars, rules = world(me, 'Locke' if me != 'Locke' else 'Bayne', dist=10,
                             me_state=dict(lastbutton=1, buttonrepeat=2))
        add(f'button-attack.{me}.vsplayer', 'button-attack', chars, rules, args=dict(button=1), tape=[0])
        for label, kw in (('dead', dict(me_stats=dict(health=0))), ('held', dict(
                me_doing=block('cheld', 0xc, attack=first_with(mcd, CA_INTERACTIVE) or 0)))):
            chars, rules = world(me, tg, dist=10, **kw)
            add(f'button-attack.{me}.{label}', 'button-attack', chars, rules, args=dict(button=1))
            chars, rules = world(me, tg, dist=10, me_flags=0x80000, **kw)
            add(f'button-attack.{me}.{label}.self', 'button-attack', chars, rules, args=dict(button=1))
        chars, rules = world(me, tg, dist=10)
        add(f'button-action.{me}', 'button-action', chars, rules, args=dict(button=1), found='Target')
        add(f'button-action.{me}.nobody', 'button-action', chars, rules, args=dict(button=1))

    # RandomAttack and SpecificAttack (monsters, and the player's chains).
    for me, tg in (('Araknid', 'Locke'), ('Jong', 'Locke'), ('Pale Ogrok', 'Locke'), ('Kantha', 'Locke'),
                   ('Yhagoro', 'Locke'), ('Arakna', 'Locke'), ('Locke', 'Araknid')):
        mcd = s['chars'][me]
        for dist in (2, 10, 40):
            for pcnt in (1, 50, 100):
                for label, kw in (('base', {}), ('interactive', dict(me_state=dict(requestbits=4, lastbutton=1))),
                                  ('interactive.nobutton', dict(me_state=dict(requestbits=4))),
                                  ('timer', dict(me_state=dict(nextattack=3)))):
                    chars, rules = world(me, tg, dist=dist, **kw)
                    add(f'random-attack.{me}.d{dist}.p{pcnt}.{label}', 'random-attack', chars, rules,
                        args=dict(pcnt=pcnt), spells=['heal3', 'fireball'])
        for i in range(len(mcd['attacks'])):
            chars, rules = world(me, tg, dist=10)
            add(f'specific-attack.{me}.a{i}', 'specific-attack', chars, rules, args=dict(attack=i))
        for label, kw, extra in (('found', dict(me_root=block('combat', 3)), dict(found='Target')),
                                 ('nobody', dict(me_root=block('combat', 3)), {}),
                                 ('dead', dict(me_stats=dict(health=0)), {})):
            chars, rules = world(me, tg, dist=10, **kw)
            add(f'specific-attack.{me}.{label}', 'specific-attack', chars, rules, args=dict(attack=0), **extra)

    # Held in someone's interactive move: an interactive attack, or only a
    # held impact; the character making the move isn't held.
    lcd = s['chars']['Locke']
    li = first_with(lcd, CA_INTERACTIVE)
    lk = next(k for k, imp in enumerate(lcd['attacks'][li]['impacts']) if imp['flags'] & CAI_INTERACTIVE)
    held = {'attack': dict(me_doing=block('cheld', 0xc, attack=li, attack_of='Target')),
            'impact': dict(me_doing=block('cheld', 0xc, impact=lk, impact_attack=li, impact_of='Target')),
            'impact.self': dict(me_doing=block('cheld', 0xc, impact=lk, impact_attack=li, impact_of='Target'),
                                me_flags=0x80000)}
    for label, kw in held.items():
        for call, args in (('button-attack', dict(button=1)), ('button-action', dict(button=1)),
                           ('random-attack', dict(pcnt=1)), ('specific-attack', dict(attack=0))):
            me = 'Bayne' if call.startswith('button') else 'Araknid'
            chars, rules = full_world(data, me, 'Locke', dist=10, **kw)
            add(f'{call}.held.{label}', call, chars, rules, args=args, found='Target')
        cd = copy.deepcopy(s['chars']['Araknid'])
        cd['attacks'] = [magic_record(cd['attacks'][0], 1, 0)]
        chars, rules = full_world(data, 'Araknid', 'Locke', dist=10, **kw)
        chars[0]['chardata'] = cd
        add(f'do-attack.magic.held.{label}', 'do-attack', chars, rules, spells=['heal3'],
            args=dict(attack=0, impact=-1, damage=1, tohit=1, roll=1, targ='Target'))

    # The counter's search: picks that are PLAYANIMs (passed over), a
    # monster that can't attack (ten failed searches), one that can't act.
    araknid = s['chars']['Araknid']['attacks']
    plays = [i for i, ad in enumerate(araknid) if ad['flags'] & CA_PLAYANIM]
    for label, tape, kw in (
            ('playanims', [0, 20] + plays * 3 + [0, 1], {}),
            ('allplayanims', [0, 20] + plays * 30, {}),
            ('bare', [0, 20], dict(target_states=['combat'])),
            ('dead', [0, 20, 0, 1], dict(target_stats=dict(health=0))),
            ('invoking', [0, 20, 0, 1], dict(target_doing=block('invoke', 0xb)))):
        chars, rules = full_world(data, 'Locke', 'Araknid', dist=10, me_state=dict(lastbutton=1, buttonrepeat=2),
                                  **kw)
        add(f'button-attack.counter.{label}', 'button-attack', chars, rules, args=dict(button=1), tape=tape)

    # Bow roots: the searches, the callers and DoAttack's turn to face.
    cd = copy.deepcopy(s['chars']['Araknid'])
    bowed = [synthetic(ad, add_flags=CA_BOWMODE) for ad in cd['attacks']]
    cd['attacks'] = bowed + cd['attacks']
    for root in (('bow', 0x19), ('walk', 1)):
        for call, args in (('random-attack', dict(pcnt=1)), ('specific-attack', dict(attack=0)),
                           ('find-interactive', dict(pcnt=0)), ('find-pcnt', dict(pcnt=1)),
                           ('do-attack', dict(attack=2, impact=-1, damage=3, tohit=50, roll=50, targ='Target'))):
            for flags in (0, 4):
                chars, rules = full_world(data, 'Araknid', 'Locke', dist=10,
                                          me_root=block(root[0], root[1], obj='Target'), me_flags=flags)
                chars[0]['chardata'] = cd
                chars[0]['states'] += [ad['name'] for ad in bowed]
                add(f'{call}.root{root[0]}.f{flags}', call, chars, rules, args=args)
    chars, rules = full_world(data, 'Pale Ogrok', 'Locke', dist=10, me_root=block('bow', 0x19, obj='Target'))
    add('find-interactive.rootbow.real', 'find-interactive', chars, rules, args=dict(pcnt=0))
    chars, rules = full_world(data, 'Pale Ogrok', 'Locke', dist=10, me_root=block('walk', 1, obj='Target'))
    add('find-interactive.rootwalk.real', 'find-interactive', chars, rules, args=dict(pcnt=0))
    # An empty attack table.
    for call, args in (('find-button', dict(button=1, targ='Target')), ('find-pcnt', dict(pcnt=1)),
                       ('find-interactive', dict(pcnt=0)), ('button-attack', dict(button=1)),
                       ('random-attack', dict(pcnt=1)), ('specific-attack', dict(attack=0))):
        me = 'Locke' if call.startswith('button') or call == 'find-button' else 'Araknid'
        chars, rules = full_world(data, me, 'Arakna', dist=10)
        chars[0]['chardata']['attacks'] = []
        add(f'{call}.emptytable', call, chars, rules, args=args)


def attack_choice_cases(data: Path, workdir: Path) -> list[dict]:
    s = shipped(data, workdir)
    cases = []
    seed = 1

    def add(name, call, chars, rules, **kw):
        nonlocal seed
        seed += 1
        cases.append(case(name, call, chars, rules, seed=seed * 7919 + 13, **kw))

    for me, tg in PAIRS:
        mcd, tcd = s['chars'][me], s['chars'][tg]
        mplayer, tplayer = me in PLAYERS, tg in PLAYERS
        tstates = ['combat'] + impact_states(mcd, 'c')
        mstates = ['combat', 'walk'] + attack_states(mcd)
        key = f"{me.replace(' ', '_')}.vs.{tg.replace(' ', '_')}"
        armed = dict(nextattack=0, magictimer=0)
        base = dict(me_states=mstates, target_states=tstates, me_state=armed)

        def world(**kw):
            args = dict(base)
            for k, v in kw.items():
                if k in ('me_state', 'target_state') and k in args:
                    args[k] = dict(args[k], **v)
                else:
                    args[k] = v
            return duel(data, me, tg, **args)

        variants = {
            'base': {},
            'nextattack1': dict(me_state=dict(nextattack=1)),
            'magictimer1': dict(me_state=dict(magictimer=1)),
            'noplayanim': dict(me_state=dict(requestbits=2)),
            'hurt': dict(me_stats=dict(health=5, mana=200)),
            'flush': dict(me_stats=dict(health=500, mana=0)),
            'root.walk': dict(me_root=block('walk', 1, obj='Target')),
            'root.sneak': dict(me_root=block('sneak', 1, obj='Target')),
            'root.bow': dict(me_root=block('bow', 0x19, obj='Target')),
            'root.combatnamedwalk': dict(me_root=block('walk', 3, obj='Target')),
            'root.combatrun': dict(me_root=block('combatrun', 3, obj='Target')),
            'root.handrun': dict(me_root=block('handrun', 3, obj='Target')),
            'root.run': dict(me_root=block('run', 1, obj='Target')),
            'root.bowrun': dict(me_root=block('bowrun', 0x19, obj='Target')),
            'root.hand': dict(me_root=block('hand', 3, obj='Target')),
            'doing.move': dict(me_doing=block('combatf', 4, obj='Target', moveangle=64)),
            'doing.walkmove': dict(me_doing=block('walkf', 2)),
            'doing.bowmove': dict(me_doing=block('bowf', 0x1a)),
            'targ.stunned': dict(target_doing=block('cstun', 0xd)),
            'targ.down': dict(target_doing=block('cdown', 0xe)),
            'targ.behind': dict(target_facing_away=True),
            'targ.weak': dict(target_stats=dict(health=1)),
            'targ.dead': dict(target_stats=dict(health=0)),
            'targ.bare': dict(target_states=['combat']),
            'targ.handroot': dict(target_root=block('hand', 3, obj='Me')),
            'me.bare': dict(me_states=['combat']),
            'me.transitions': dict(me_states=['combat'] + ['combat to ' + n for n in attack_states(mcd)]),
            'flags.playanimroots': dict(me_flags=0x10000),
            'flags.nokill.weak': dict(me_flags=0x80, target_stats=dict(health=1)),
        }
        if tplayer:
            variants['targ.resists'] = dict(target_resists=[5, 10, 20, -10, 30, 50, 0, 0, 0, 0],
                                            target_stats=dict(acbonus=4))
            # The armour worn (TPlayer::ArmorValue 0x519850): ACBonus plus
            # each armor item's Protection in slots 0-10 but the ammo's (8).
            cloth = [dict(slot=2, protection=1), dict(slot=9, protection=1)]
            every = [dict(slot=s, protection=s + 1) for s in range(11) if s != 4]
            for label, worn, acbonus in (('cloth', cloth, 0), ('cloth.ac', cloth, 3),
                                         ('every', every, 0), ('ammo', [dict(slot=8, protection=9)], 0),
                                         ('weaponclass', [dict(slot=2, protection=7, **{'class': 1})], 0),
                                         ('heavy', [dict(slot=s, protection=10) for s in (0, 2, 3, 9, 10)], 5),
                                         ('negative', cloth, -4)):
                variants[f'targ.armor.{label}'] = dict(target_equipment=[dict({'class': 2}, **w) for w in worn],
                                                       target_stats=dict(acbonus=acbonus))
        for fat in thresholds(mcd):
            variants[f'fatigue{fat}'] = dict(me_stats=dict(fatigue=fat))
        if mplayer:
            for wt in (1, 2, 3, 4, 5):
                variants[f'weapon{wt}'] = dict(weapon=dict(type=wt, damage=11 + wt))
            variants['skill.low'] = dict(me_stats=dict(attacklevel=0, hands=0, knife=0, sword=0))
            variants['nahkranoth'] = dict(nahkranoth=1)
            variants['nahkranoth.tired'] = dict(nahkranoth=1, me_stats=dict(fatigue=0))
        # An attack the target is in the middle of: the to-hit face bonus,
        # and an interactive one that holds it.
        ti = first_with(tcd, CA_INTERACTIVE)
        variants['targ.attacking'] = dict(target_doing=block(tcd['attacks'][0]['name'], 7, attack=0, obj='Me'))
        if ti is not None:
            variants['targ.held'] = dict(target_doing=block(tcd['attacks'][ti]['name'], 7, attack=ti, obj='Me'))
            variants['targ.held.self'] = dict(target_doing=block(tcd['attacks'][ti]['name'], 7, attack=ti, obj='Me'),
                                              target_flags=0x80000)
        mi = first_with(mcd, CA_INTERACTIVE)
        if mi is not None and any(imp['flags'] & CAI_INTERACTIVE for imp in mcd['attacks'][mi].get('impacts', [])):
            k = next(k for k, imp in enumerate(mcd['attacks'][mi]['impacts']) if imp['flags'] & CAI_INTERACTIVE)
            variants['targ.heldimpact'] = dict(target_doing=block('cheld', 0xc, attack=mi, impact=k, obj='Me',
                                                                  attack_of='Me'))
            variants['snap.blocked'] = dict(case_extra=dict(blocked=True, blocker=None))
            variants['snap.blocked.byme'] = dict(case_extra=dict(blocked=True, blocker='Me'))
        # A swing still early (frame before its nextwait) or done.
        a0 = next((i for i, ad in enumerate(mcd['attacks']) if not ad['flags'] & (CA_MAGICATTACK | CA_PLAYANIM)), 0)
        nw = mcd['attacks'][a0].get('nextwait', 0) or 0
        variants['doing.attack.early'] = dict(me_doing=block(mcd['attacks'][a0]['name'], 7, attack=a0, obj='Target'),
                                              me_frame=max(0, nw - 1))
        variants['doing.attack.done'] = dict(me_doing=block(mcd['attacks'][a0]['name'], 7, attack=a0, obj='Target'),
                                             me_frame=nw)
        cp = chain_parent(mcd)
        if cp:
            link, parent = cp
            exp = mcd['attacks'][parent]['chainexptime']
            for label, ago in (('live', 1), ('edge', exp), ('expired', exp + 1)):
                variants[f'chain.{label}'] = dict(me_state=dict(lastattack=parent, lastattackticks=100 - ago))
        # Loop states at index 0 (retail takes FindState's index for yes/no).
        loops = [imp['loopname'] for ad in mcd['attacks'] for imp in ad.get('impacts', []) if imp['loopname']]
        if loops:
            variants['targ.loop0'] = dict(target_states=[loops[0]] + ['combat'] + [n for n in tstates[1:] if n != loops[0]])
            variants['targ.noloops'] = dict(target_states=[n for n in tstates if n not in loops])
            variants['targ.deadonly'] = dict(target_states=['combat', 'dead', 'cdead', 'combat to cdead'] + loops)

        for vname, v in variants.items():
            v = dict(v)
            extra = v.pop('case_extra', {})
            nk = v.pop('nahkranoth', 0)
            away = v.pop('target_facing_away', False)
            resists = v.pop('target_resists', None)
            equipment = v.pop('target_equipment', None)
            chars, rules = world(**v)
            if equipment is not None:
                chars[1]['equipment'] = equipment
            if away:
                chars[1]['facing'] = chars[1]['moveangle'] = chars[0]['facing']
            if resists is not None:
                chars[1]['resists'] = resists
            add(f'iva.{key}.{vname}', 'iva', chars, rules, calls=iva_calls(chars[0]['chardata']), nahkranoth=nk,
                **extra)
        # No target at all.
        chars, rules = world()
        add(f'iva.{key}.notarget', 'iva', chars, rules, calls=iva_calls(chars[0]['chardata'], targ=None))
        # The search arguments: buttons, percentages, the mask the button
        # search uses, presets of the out-values.
        calls = []
        for i, ad in enumerate(mcd['attacks']):
            mid = (ad['mindist'] + ad['maxdist']) // 2
            for button in (-1, ad['button'], ad['button'] + 1):
                calls.append(dict(attack=i, tdist=mid, targ='Target', button=button))
            for pcnt in (-1, ad['attackpcnt'], ad['attackpcnt'] + 1):
                calls.append(dict(attack=i, tdist=mid, targ='Target', pcnt=pcnt))
            for mask, flags in ((3, 2), (3, 1), (3, 0), (3, 0x40000000), (0x40000003, 0x40000000)):
                calls.append(dict(attack=i, tdist=mid, targ='Target', mask=mask, flags=flags))
        add(f'iva.{key}.args', 'iva', chars, rules, calls=calls)
        # The damage tiers at their edges: to-hit and roll preset.
        calls = []
        for i, ad in enumerate(mcd['attacks'][:12]):
            mid = (ad['mindist'] + ad['maxdist']) // 2
            for roll in (1, 9, 10, 11, 49, 50, 51, 64, 65, 66, 89, 90, 91, 99, 100):
                calls.append(dict(attack=i, tdist=mid, targ='Target', tohit=50, roll=roll))
            calls.append(dict(attack=i, tdist=mid, targ='Target', damage=40, tohit=90, roll=5))
            calls.append(dict(attack=i, tdist=mid, targ='Target', damage=0, tohit=10, roll=100))
        add(f'iva.{key}.tiers', 'iva', chars, rules, calls=calls)
        # The to-hit bonus and the roll drawn: steered.
        for label, tape in (('bonus', [0, 0, 0]), ('nobonus', [1, 49, 50]), ('mid', [5, 24, 25])):
            add(f'iva.{key}.tape.{label}', 'iva', chars, rules, calls=iva_calls(chars[0]['chardata'])[:6],
                tape=tape * 6)
        # Each MAGICATTACK on its own: IsValidAttack spends the magic timer.
        for i, ad in enumerate(mcd['attacks']):
            if ad['flags'] & CA_MAGICATTACK:
                for label, stats in (('as-is', {}), ('low', dict(health=1, mana=0)), ('high', dict(health=500, mana=500))):
                    chars, rules = world(me_stats=stats)
                    add(f'iva.{key}.magic{i}.{label}', 'iva', chars, rules,
                        calls=[dict(attack=i, tdist=50, targ='Target'), dict(attack=i, tdist=50, targ='Target')])
    synthetic_cases(data, add)
    search_cases(data, add)
    making_cases(data, add)
    return finish(cases)
# ---- C4: hit resolution ------------------------------------------------------------------

def hit_world(data, me, tg, *, dist=10, bearing=64, target_facing=None, **kw):
    """A duel set up for a blow: both sides know each other's impacts."""
    chars, rules = full_world(data, me, tg, dist=dist, bearing=bearing, **kw)
    if target_facing is not None:
        chars[1]['facing'] = chars[1]['moveangle'] = target_facing
    return chars, rules


def hit_cases(data: Path, workdir: Path) -> list[dict]:
    s = shipped(data, workdir)
    cases = []
    seed = 7

    def add(name, call, chars, rules, **kw):
        nonlocal seed
        seed += 1
        cases.append(case(name, call, chars, rules, seed=seed * 104729 + 3, **kw))

    rules0 = s['rules']
    keys = [row[0] for row in rules0['tohitdamage']]
    # Margins (to-hit less roll) around every tier key, both ways.
    margins = sorted({k + d for k in keys for d in (-1, 0, 1)} | {60, -60})

    # ResolveHit, one blow at a time: each kind of impact the tables have,
    # hit / glance at every tier edge, lethal or not, the gates.
    pairs = (('Araknid', 'Locke'), ('Locke', 'Araknid'), ('Locke', 'Rahul'), ('Pale Ogrok', 'Locke'),
             ('Kantha', 'Locke'), ('Navarro', 'Rahul'), ('Jong', 'Locke'), ('Bayne', 'Pale Ogrok'))
    for me, tg in pairs:
        mcd = s['chars'][me]
        key = f"{me.replace(' ', '_')}.vs.{tg.replace(' ', '_')}"
        picks = {}
        for i, ad in enumerate(mcd['attacks']):
            if ad['flags'] & (CA_MAGICATTACK | CA_PLAYANIM):
                continue
            imps = tuple((imp['flags'], bool(imp['loopname'])) for imp in ad.get('impacts', []))
            shape = (ad['flags'] & (CA_INTERACTIVE | CA_FATIGUEATTACK | CA_DEATH), imps)
            picks.setdefault(shape, i)
        for i in picks.values():
            ad = mcd['attacks'][i]
            reach = max(ad['hitminrange'], min(ad['hitmaxrange'], 10))
            nimp = len(ad.get('impacts', []))
            for imp in range(-1, nimp):
                for margin in margins:
                    for health in (100, 3):
                        chars, rules = hit_world(data, me, tg, dist=reach, target_stats=dict(health=health))
                        tohit = 60
                        add(f'resolve-hit.{key}.a{i}.i{imp}.m{margin}.h{health}', 'resolve-hit', chars, rules,
                            args=dict(attack=i, impact=imp, targ='Target', damage=12, tohit=tohit,
                                      roll=tohit - margin))
            # Guarding (blocking / dodging), facing the blow or not.
            for action, label in ((8, 'block'), (9, 'dodge'), (0xc, 'impact')):
                for facing in (None, 64):
                    for margin in (45, 30, 5, -5, -30):
                        chars, rules = hit_world(data, me, tg, dist=reach, target_doing=block('cguard', action),
                                                 target_facing=facing)
                        add(f'resolve-hit.{key}.a{i}.{label}.f{facing}.m{margin}', 'resolve-hit', chars, rules,
                            args=dict(attack=i, impact=nimp - 1, targ='Target', damage=20, tohit=70,
                                      roll=70 - margin))
            # Reach, height, angle, the enemy test, a target that can't be fought.
            for label, kw, extra in (
                    ('tooclose', dict(dist=ad['hitminrange'] - 1), {}),
                    ('mindist', dict(dist=ad['hitminrange']), {}),
                    ('maxdist', dict(dist=ad['hitmaxrange']), {}),
                    ('toofar', dict(dist=ad['hitmaxrange'] + 1), {}),
                    ('friend', {}, dict(friends=[['Me', 'Target']])),
                    ('dead', dict(target_stats=dict(health=0)), {}),
                    ('untargetable', dict(target_flags=0x8000), {}),
                    ('fighting', dict(target_root=block('combat', 3, obj='Me')), {}),
                    ('fightingother', dict(target_root=block('combat', 3, obj='Target')), {}),
                    ('autocombat0', dict(me_state=dict(autocombat=0)), {}),
                    ('far', dict(dist=max(reach, 200)), {}),
                    ('moving', dict(target_accum=[3, 0, 0]), {}),
                    ('moving.blocking', dict(target_accum=[0, 2, 0], target_doing=block('cblock', 8)), {}),
                    ('held.byme', dict(target_doing=block('cheld', 7, attack=0, attack_of='Me'),
                                       target_root=block('combat', 3, obj='Me')), {}),
                    ('held.byother', dict(target_doing=block('cheld', 7, attack=0, attack_of='Me')), {}),
                    ('held.self', dict(target_doing=block('cheld', 7, attack=0, attack_of='Me'), target_flags=0x80000),
                     {}),
                    ('nocombatresults', {}, dict(nocombatresults=1)),
                    ('target.bowroot', dict(target_root=block('bow', 0x19, obj='Me')), {}),
                    ('target.blocking', dict(target_doing=block('cblock', 8)), {})):
                for margin in (30, -30):
                    for health in (100, 3):
                        kw2 = dict(dict(dist=reach), **kw)
                        accum = kw2.pop('target_accum', None)
                        nocr = extra.get('nocombatresults', 0)
                        stats = dict(dict(health=health), **kw2.pop('target_stats', {}))
                        chars, rules = hit_world(data, me, tg, **kw2, target_stats=stats)
                        if accum is not None:
                            chars[1]['accum'] = accum
                        c_extra = {k: v for k, v in extra.items() if k != 'nocombatresults'}
                        add(f'resolve-hit.{key}.a{i}.{label}.m{margin}.h{health}', 'resolve-hit', chars, rules,
                            args=dict(attack=i, impact=nimp - 1, targ='Target', damage=15, tohit=60,
                                      roll=60 - margin), nocombatresults=nocr, **c_extra)
            # Height.
            for dz in (40, 41, -41):
                chars, rules = hit_world(data, me, tg, dist=reach)
                chars[1]['pos'][2] += dz
                add(f'resolve-hit.{key}.a{i}.dz{dz}', 'resolve-hit', chars, rules,
                    args=dict(attack=i, impact=nimp - 1, targ='Target', damage=15, tohit=60, roll=30))
            # Angle: the target off to the side by the attack's hit angle.
            for off in (ad['hitangle'], ad['hitangle'] + 1):
                chars, rules = hit_world(data, me, tg, dist=reach, bearing=64)
                chars[0]['facing'] = chars[0]['moveangle'] = (64 - off) & 0xff
                add(f'resolve-hit.{key}.a{i}.angle{off}', 'resolve-hit', chars, rules,
                    args=dict(attack=i, impact=nimp - 1, targ='Target', damage=15, tohit=60, roll=30))
        # No target, a target missing its impact animations.
        chars, rules = hit_world(data, me, tg)
        add(f'resolve-hit.{key}.notarget', 'resolve-hit', chars, rules,
            args=dict(attack=next(iter(picks.values())), impact=-1, targ=None, damage=5, tohit=50, roll=10))
        for i in picks.values():
            nimp = len(mcd['attacks'][i].get('impacts', []))
            for health in (100, 3):
                chars, rules = hit_world(data, me, tg, target_states=['combat'], target_stats=dict(health=health))
                add(f'resolve-hit.{key}.a{i}.bare.h{health}', 'resolve-hit', chars, rules,
                    args=dict(attack=i, impact=nimp - 1, targ='Target', damage=9, tohit=60, roll=30))
    # The rarer turns of ResolveHit: the target to the left, held only by an
    # impact, in a bow root; a to-hit pushed past 100 by the guard's take; no
    # damage; drawn into the fight (no target, autocombat off, close); a
    # moving target forced hit at a glancing margin, through a dodge.
    pcd = s['chars']['Pale Ogrok']
    pi = first_with(pcd, CA_INTERACTIVE)
    pk = next(k for k, imp in enumerate(pcd['attacks'][pi]['impacts']) if imp['flags'] & CAI_INTERACTIVE)
    for me, tg in (('Araknid', 'Locke'), ('Pale Ogrok', 'Locke'), ('Locke', 'Rahul')):
        mcd = s['chars'][me]
        a0 = next(i for i, ad in enumerate(mcd['attacks']) if not ad['flags'] & (CA_MAGICATTACK | CA_PLAYANIM))
        ai = first_with(mcd, CA_INTERACTIVE)
        ad0 = mcd['attacks'][a0]
        reach = max(ad0['hitminrange'], min(ad0['hitmaxrange'], 10))
        nimp0 = len(ad0.get('impacts', []))
        for off in (-ad0['hitangle'], -ad0['hitangle'] - 1, -1):
            chars, rules = hit_world(data, me, tg, dist=reach)
            chars[0]['facing'] = chars[0]['moveangle'] = (64 - off) & 0xff
            add(f'resolve-hit.{me}.leftangle{off}', 'resolve-hit', chars, rules,
                args=dict(attack=a0, impact=nimp0 - 1, targ='Target', damage=15, tohit=60, roll=30))
        for label, kw in (('heldimpact.other', dict(target_doing=block('cheld', 0xc, impact=pk, impact_attack=pi,
                                                                             impact_of='Third'))),
                          ('heldimpact.me', dict(target_doing=block('cheld', 0xc, impact=pk, impact_attack=pi,
                                                                          impact_of='Third'),
                                                 target_root=block('combat', 3, obj='Me'))),
                          ('heldimpact.bowroot', dict(target_doing=block('cheld', 0xc, impact=pk, impact_attack=pi,
                                                                               impact_of='Third'),
                                                      target_root=block('bow', 0x19, obj='Me'))),
                          ('held.walkroot', dict(target_doing=block('cheld', 0xc, impact=pk, impact_attack=pi,
                                                                          impact_of='Third'),
                                                 target_root=block('walk', 1, obj='Me')))):
            chars, rules = hit_world(data, me, tg, dist=reach, **kw)
            chars.append(make_char('Third', 12, [3000, 3000, 0], 0, chardata(pcd), states=['combat'],
                                   stats=dict(MONSTER_STATS), ident=0x30))
            add(f'resolve-hit.{me}.{label}', 'resolve-hit', chars, rules,
                args=dict(attack=a0, impact=nimp0 - 1, targ='Target', damage=15, tohit=60, roll=30))
        for tohit, roll in ((160, 1), (155, 100), (151, 60)):
            chars, rules = hit_world(data, me, tg, dist=reach, target_doing=block('cblock', 8))
            add(f'resolve-hit.{me}.guard.tohit{tohit}.r{roll}', 'resolve-hit', chars, rules,
                args=dict(attack=a0, impact=nimp0 - 1, targ='Target', damage=15, tohit=tohit, roll=roll))
        for dmg in (0, -3):
            chars, rules = hit_world(data, me, tg, dist=reach)
            add(f'resolve-hit.{me}.damage{dmg}', 'resolve-hit', chars, rules,
                args=dict(attack=a0, impact=nimp0 - 1, targ='Target', damage=dmg, tohit=60, roll=30))
        for auto in (0, 1):
            for dist in (reach, 200):
                chars, rules = hit_world(data, me, tg, dist=dist, target_root=block('combat', 3),
                                         me_state=dict(autocombat=auto))
                for margin in (20, -20):
                    add(f'resolve-hit.{me}.engage.auto{auto}.d{dist}.m{margin}', 'resolve-hit', chars, rules,
                        args=dict(attack=a0, impact=nimp0 - 1, targ='Target', damage=15, tohit=60, roll=60 - margin))
        if ai is not None:
            nimpi = len(mcd['attacks'][ai].get('impacts', []))
            for margin in (-35, -45, -55):
                for guard in (None, 9):
                    kw = dict(target_doing=block('cdodge', guard)) if guard else {}
                    chars, rules = hit_world(data, me, tg, dist=5, **kw)
                    chars[1]['accum'] = [4, 0, 0]
                    add(f'resolve-hit.{me}.forced.g{guard}.m{margin}', 'resolve-hit', chars, rules,
                        args=dict(attack=ai, impact=nimpi - 1, targ='Target', damage=5, tohit=60, roll=60 - margin))
    for flags in (1, 2, 0x80):
        cd = copy.deepcopy(s['chars']['Araknid'])
        cd['attacks'][0]['impacts'] = [impact('imph', flags, 'sloop' if flags & 3 else '')]
        chars, rules = hit_world(data, 'Araknid', 'Locke', target_states=['combat', 'cimph', 'imph', 'sloop'])
        chars[0]['chardata'] = cd
        add(f'resolve-hit.glance.impactflags{flags:#x}', 'resolve-hit', chars, rules,
            args=dict(attack=0, impact=0, targ='Target', damage=9, tohit=30, roll=60))

    # The fatigue attack's own result tag; a stun and a knockdown impact.
    lcd = s['chars']['Locke']
    fa = next(i for i, ad in enumerate(lcd['attacks']) if ad['flags'] & CA_FATIGUEATTACK)
    for margin in (30, -30):
        chars, rules = hit_world(data, 'Locke', 'Araknid')
        add(f'resolve-hit.fatigueattack.m{margin}', 'resolve-hit', chars, rules,
            args=dict(attack=fa, impact=0, targ='Target', damage=9, tohit=60, roll=60 - margin))
    for flags, loop in ((1, 'sloop'), (2, 'dloop'), (0x81, 'sloop'), (0x82, '')):
        cd = copy.deepcopy(s['chars']['Araknid'])
        cd['attacks'][0]['impacts'] = [impact('imph', flags, loop)]
        for states in (['combat', 'cimph', 'imph', 'sloop', 'dloop'], ['combat']):
            chars, rules = hit_world(data, 'Araknid', 'Locke', target_states=states)
            chars[0]['chardata'] = cd
            add(f'resolve-hit.impactflags{flags:#x}.{len(states)}', 'resolve-hit', chars, rules,
                args=dict(attack=0, impact=0, targ='Target', damage=9, tohit=60, roll=30))

    # ResolveAttack: a tick of an attack block before, at and after its
    # impact frame; who it strikes; the miss, the guard's clash, fatigue.
    for me, tg in (('Araknid', 'Locke'), ('Locke', 'Araknid'), ('Pale Ogrok', 'Locke'), ('Navarro', 'Rahul'),
                   ('Jong', 'Locke'), ('Locke', 'Rahul')):
        mcd = s['chars'][me]
        key = f"{me.replace(' ', '_')}.vs.{tg.replace(' ', '_')}"
        picks = {}
        for i, ad in enumerate(mcd['attacks']):
            shape = ad['flags'] & (CA_INTERACTIVE | CA_PLAYANIM | CA_MAGICATTACK | 0x200000 | 0x1000 | 0x400000)
            picks.setdefault((shape, bool(ad.get('missname'))), i)
        for i in picks.values():
            ad = mcd['attacks'][i]
            it = ad.get('impacttime', 0) or 0
            nimp = len(ad.get('impacts', []))
            reach = max(ad.get('hitminrange', 0), min(ad.get('hitmaxrange', 0), 8))
            for frame, first in ((0, True), (max(0, it - 1), False), (it, False), (it, True), (it + 1, False)):
                for margin, health in ((30, 100), (-30, 100), (30, 2)):
                    doing = block(ad['name'], 7, attack=i, impact=nimp - 1 if nimp else None, damage=11, tohit=60,
                                  roll=60 - margin, obj='Target', flags=['firsttime'] if first else [])
                    if nimp == 0:
                        del doing['impact']
                    chars, rules = hit_world(data, me, tg, dist=reach, me_doing=doing, me_frame=frame,
                                             target_stats=dict(health=health))
                    add(f'resolve-attack.{key}.a{i}.f{frame}.{"first" if first else "next"}.m{margin}.h{health}',
                        'resolve-attack', chars, rules, args=dict(bits=0), tape=[3, 7, 11, 13])
            # At the impact frame: a guard, others in reach, no target.
            doing = block(ad['name'], 7, attack=i, damage=11, tohit=40, roll=90, obj='Target')
            for label, kw, extra in (
                    ('guard', dict(target_doing=block('cblock', 8)), {}),
                    ('guard.far', dict(target_doing=block('cblock', 8), dist=200), {}),
                    ('guard.armed', dict(target_doing=block('cblock', 8), weapon=dict(type=2, damage=9)), {}),
                    ('others', {}, dict(found='Target')),
                    ('notarget', dict(me_doing=dict(doing, obj=None)), {}),
                    ('iced', dict(me_objflags=0x2000000), {}),
                    ('noturn', dict(me_flags=4), {}),
                    ('tired', dict(me_stats=dict(fatigue=3)), {}),
                    ('nomiss.anim', dict(me_states=['combat', ad['name'], ad.get('missname') or 'cmiss']), {})):
                kw = dict(dict(me_doing=doing, me_frame=it, dist=reach), **kw)
                chars, rules = hit_world(data, me, tg, **kw)
                if label == 'others':
                    chars.append(make_char('Third', 12, _toward(ME_AT, chars[0]['facing'], reach + 32), 0,
                                           chardata(s['chars']['Araknid']), states=['combat', 'cimph', 'cimpl'],
                                           stats=dict(MONSTER_STATS), ident=0x30))
                    extra = dict(found='Third')
                add(f'resolve-attack.{key}.a{i}.{label}', 'resolve-attack', chars, rules, args=dict(bits=0),
                    tape=[1, 2, 3, 4], **extra)
    # The block clash's sound by both weapons.
    for mine in range(0, 9):
        for theirs in (0, 1, 2, 3, 4, 5):
            cd = copy.deepcopy(s['chars']['Araknid'])
            cd['weapontype'] = mine
            tcd = copy.deepcopy(s['chars']['Rahul'])
            tcd['weapontype'] = theirs
            doing = block(cd['attacks'][0]['name'], 7, attack=0, damage=11, tohit=40, roll=90, obj='Target')
            chars, rules = hit_world(data, 'Araknid', 'Rahul', me_doing=doing,
                                     me_frame=cd['attacks'][0]['impacttime'], target_doing=block('cblock', 8))
            chars[0]['chardata'] = cd
            chars[1]['chardata'] = tcd
            add(f'resolve-attack.clash.w{mine}.vs{theirs}', 'resolve-attack', chars, rules, args=dict(bits=0),
                tape=[mine % 2, 5])

    # OnAttacked: a monster told it is attacked.
    for me in ('Araknid', 'Pale Ogrok', 'Rahul'):
        mcd = s['chars'][me]
        for attacker in ('Locke', 'Jong'):
            acd = s['chars'][attacker]
            fat = next((i for i, ad in enumerate(acd['attacks']) if ad['flags'] & CA_FATIGUEATTACK), None)
            plain = next(i for i, ad in enumerate(acd['attacks']) if not ad['flags'] & (CA_PLAYANIM | CA_MAGICATTACK))
            for label, adoing in (('noattack', block('combat', 3)),
                                  ('plain', block('swing', 7, attack=plain)),
                                  ('fatigue', block('cfatigue', 7, attack=fat if fat is not None else plain))):
                for state in (dict(lastbutton=-1, buttonrepeat=0), dict(lastbutton=acd['attacks'][plain]['button'],
                                                                       buttonrepeat=1),
                              dict(lastbutton=acd['attacks'][plain]['button'], buttonrepeat=2)):
                    for flag in (0, 2):
                        for roll in (0, 99):
                            for root in ('me', 'other', 'none'):
                                chars, rules = hit_world(data, attacker, me, dist=20,
                                                         me_doing=dict(adoing, obj='Target'),
                                                         target_state=state,
                                                         target_root=block('combat', 3, obj={'me': 'Me', 'other': 'Target', 'none': None}[root]))
                                add(f'on-attacked.{me}.by{attacker}.{label}.lb{state["lastbutton"]}r{state["buttonrepeat"]}'
                                    f'.f{flag}.roll{roll}.root{root}', 'on-attacked', chars, rules, self_='Target',
                                    args=dict(attacker='Me', victim='Target', flag=flag), tape=[roll, 5])
        # Distances: retarget when the attacker is under 75% of the current target's.
        for d1 in (10, 40, 100):
            chars, rules = hit_world(data, 'Locke', me, dist=d1, target_root=block('combat', 3, obj='Third'))
            third = make_char('Third', 12, _toward(chars[1]['pos'], 0, 60 + 32), 0, chardata(s['chars']['Araknid']),
                              states=['combat'], stats=dict(MONSTER_STATS), ident=0x30)
            chars.append(third)
            add(f'on-attacked.{me}.retarget.d{d1}', 'on-attacked', chars, rules, self_='Target',
                args=dict(attacker='Me', victim='Target'), tape=[50])
        # Not for me, the player, held, nobody.
        for label, kw, args in (('notme', {}, dict(attacker='Me', victim='Me')),
                                ('noattacker', {}, dict(attacker=None, victim='Target')),
                                ('held', dict(target_doing=block('cheld', 0xc, attack=0, attack_of='Target')),
                                 dict(attacker='Me', victim='Target')),
                                ('attacker.held', dict(me_doing=block('cheld', 0xc, attack=0, attack_of='Target')),
                                 dict(attacker='Me', victim='Target'))):
            chars, rules = hit_world(data, 'Locke', me, dist=20, target_root=block('combat', 3, obj='Me'), **kw)
            add(f'on-attacked.{me}.{label}', 'on-attacked', chars, rules, self_='Target', args=args, tape=[0])
    chars, rules = hit_world(data, 'Araknid', 'Locke', dist=20)
    add('on-attacked.player', 'on-attacked', chars, rules, self_='Target', args=dict(attacker='Me', victim='Target'))
    # A victim in a bow root, or held only by an impact.
    for label, kw in (('bowroot', dict(target_root=block('bow', 0x19, obj='Me'))),
                      ('walkroot', dict(target_root=block('walk', 1, obj='Me'))),
                      ('heldimpact', dict(target_doing=block('cheld', 0xc, impact=pk, impact_attack=pi,
                                                                impact_of='Third')))):
        for roll in (0, 99):
            chars, rules = hit_world(data, 'Locke', 'Araknid', dist=20,
                                     me_doing=block('swing', 7, attack=0, obj='Target'), **kw)
            chars.append(make_char('Third', 12, [3000, 3000, 0], 0, chardata(pcd), states=['combat'],
                                   stats=dict(MONSTER_STATS), ident=0x30))
            add(f'on-attacked.{label}.roll{roll}', 'on-attacked', chars, rules, self_='Target',
                args=dict(attacker='Me', victim='Target'), tape=[roll])
    # ResolveAttack from a bow or walk root: the guard's clash with no
    # fighting target, a PLAYANIM's turn to face.
    acd = s['chars']['Araknid']
    play = next(i for i, ad in enumerate(acd['attacks']) if ad['flags'] & CA_PLAYANIM)
    for root in (('bow', 0x19), ('walk', 1), ('combat', 3)):
        for which, label in ((0, 'swing'), (play, 'playanim')):
            ad = acd['attacks'][which]
            doing = block(ad['name'], 7, attack=which, damage=11, tohit=40, roll=90, obj='Target')
            chars, rules = hit_world(data, 'Araknid', 'Locke', dist=5, me_doing=doing,
                                     me_frame=ad.get('impacttime', 0) or 0, target_doing=block('cblock', 8),
                                     me_root=block(root[0], root[1], obj='Target'))
            add(f'resolve-attack.root{root[0]}.{label}', 'resolve-attack', chars, rules, args=dict(bits=0),
                tape=[1, 2])
    return finish(cases)


# ---- C2: Damage ------------------------------------------------------------------------

# Every name Damage may look for on a victim: its CHARIMPACTs and their
# loops (raw and with the combat prefix), the block impact, "impact", the
# death fallbacks.
def victim_states(cd: dict, extra=()) -> list:
    names = ['combat', 'walk', 'block', 'cblock']
    for imp in cd.get('impacts', []):
        names += [imp['name'], 'c' + imp['name']]
        if imp['loopname']:
            names.append(imp['loopname'])
    names += ['blockimpact', 'cblockimpact', 'impact', 'cdead', 'combat to cdead', 'combat to dead', 'dead']
    names += list(extra)
    out = []
    for n in names:
        if n.lower() not in [o.lower() for o in out]:
            out.append(n)
    return out


def damage_world(data, victim, attacker, *, health=30, states=None, impacts=None, attacks=None, bearing=0, **kw):
    """The victim ('Me', the call's self) struck by `attacker` ('Target'),
    who stands at `bearing` (0: ahead, where Damage filters no CHARIMPACT
    by side): the victim's character data with `impacts` (CHARIMPACTs) and
    the attacker's with `attacks` in place of the shipped ones when given."""
    kw['bearing'] = bearing
    s = shipped(data, None)
    vcd = copy.deepcopy(s['chars'][victim])
    if impacts is not None:
        vcd['impacts'] = impacts
    acd = copy.deepcopy(s['chars'][attacker])
    if attacks is not None:
        acd['attacks'] = attacks
    stats = dict(kw.pop('me_stats', {}))
    stats.setdefault('health', health)
    chars, rules = duel(data, victim, attacker, me_states=states if states is not None else victim_states(vcd),
                        me_stats=stats, **kw)
    chars[0]['chardata'] = vcd
    chars[1]['chardata'] = acd
    return chars, rules


def hit_block(name, action, *, attack=None, impact=None, damage=0, wait=0, flags=('priority', 'interrupt')):
    """A block as ResolveHit hands Damage one: the attacker's attack and
    impact (indices into the attacker's table)."""
    b = dict(name=name, action=action, obj='Target', wait=wait, damage=damage, flags=list(flags))
    if attack is not None:
        b['attack'] = attack
    if impact is not None:
        b['impact'] = impact
        b.setdefault('attack', 0)
    return b


def damage_cases(data: Path, workdir: Path) -> list[dict]:
    s = shipped(data, workdir)
    cases = []
    seed = 11

    def add(name, chars, rules, *, damage, block=None, attacker='Target', type=-1, mod=0, **kw):
        nonlocal seed
        seed += 1
        args = dict(damage=damage, type=type, mod=mod, attacker=attacker)
        if block is not None:
            args['block'] = block
        cases.append(case(name, 'damage', chars, rules, args=args, seed=seed * 7919 + 5, seam_damage=False,
                          tape=kw.pop('tape', [1, 2, 3]), **kw))

    arak = s['chars']['Araknid']
    a_plain = next(i for i, ad in enumerate(arak['attacks']) if not ad['flags'] & (CA_MAGICATTACK | CA_PLAYANIM))
    pale = s['chars']['Pale Ogrok']
    p_int = first_with(pale, CA_INTERACTIVE)
    p_hold = next(k for k, imp in enumerate(pale['attacks'][p_int]['impacts']) if imp['flags'] & CAI_INTERACTIVE)
    locke = s['chars']['Locke']
    l_plain = next(i for i, ad in enumerate(locke['attacks']) if not ad['flags'] & (CA_MAGICATTACK | CA_PLAYANIM))

    # The gates: a frozen attacker, the cheats, an invulnerable or friendly
    # victim, a stopped player; no attacker at all.
    for victim, attacker in (('Araknid', 'Locke'), ('Locke', 'Araknid')):
        for label, kw, extra, pstate in (
                ('plain', {}, {}, None),
                ('attacker.iced', dict(target_objflags=0x2000000), {}, None),
                ('attacker.paralysed', dict(target_objflags=0x800000), {}, None),
                ('invulnerable', dict(me_objflags=0x10000000), {}, None),
                ('alreadydead', {}, dict(alreadydead=1), None),
                ('nahkranoth', {}, dict(nahkranoth=1), None),
                ('friend', {}, dict(friends=[['Me', 'Target']]), None),
                ('stopped', {}, {}, 2),
                ('stopped.other', {}, {}, 5)):
            for health, dmg in ((30, 7), (5, 7)):
                chars, rules = damage_world(data, victim, attacker, health=health, **kw)
                if pstate is not None:
                    chars[0]['playerstate'] = pstate
                add(f'damage.gate.{victim}.{label}.h{health}', chars, rules, damage=dmg, **extra)
        chars, rules = damage_world(data, victim, attacker, health=30)
        add(f'damage.gate.{victim}.noattacker', chars, rules, damage=7, attacker=None)
        chars, rules = damage_world(data, victim, attacker, health=5)
        add(f'damage.gate.{victim}.noattacker.lethal', chars, rules, damage=7, attacker=None)
        chars, rules = damage_world(data, victim, attacker, health=30)
        add(f'damage.gate.{victim}.noattacker.nahkranoth', chars, rules, damage=7, attacker=None, nahkranoth=1)

    # The amount: by type through CalculateDamage, the player's edge bonus by
    # weapon, the unkillable flag at every edge, nothing.
    for victim, attacker in (('Araknid', 'Locke'), ('Locke', 'Araknid'), ('Pale Ogrok', 'Locke')):
        for typ in (-1, 0, 1, 2, 3, 6, 8, 9):
            for mod in (0, 40, -60):
                for wt in ((0, 1, 2, 3, 4, 5) if victim == 'Locke' else (None,)):
                    kw = dict(weapon=dict(type=wt, damage=6)) if wt is not None else {}
                    chars, rules = damage_world(data, victim, attacker, health=60, **kw)
                    add(f'damage.amount.{victim}.t{typ}.m{mod}.w{wt}', chars, rules, damage=17, type=typ, mod=mod)
        for health in (0, 1, 2, 10):
            for dmg in (health - 2, health - 1, health, health + 4, 0):
                chars, rules = damage_world(data, victim, attacker, health=health, me_flags=8)
                add(f'damage.unkillable.{victim}.h{health}.d{dmg}', chars, rules, damage=dmg)

    # Held: in another's interactive attack or an interactive impact (no
    # health lost, no block), unless the move is the victim's own.
    for own in (0, 0x80000):
        for label, doing in (('attack', block('cheld', 7, attack=p_int, attack_of='Third')),
                             ('impact', block('cheld', 0xc, impact=p_hold, impact_attack=p_int, impact_of='Third')),
                             ('plain', block('cheld', 0xc, attack=0, attack_of='Third'))):
            for health in (40, 3):
                for victim in ('Araknid', 'Kantha'):
                    chars, rules = damage_world(data, victim, 'Locke', health=health, me_doing=doing, me_flags=own)
                    chars.append(make_char('Third', 12, [3000, 3000, 0], 0, chardata(pale), states=['combat'],
                                           stats=dict(MONSTER_STATS), ident=0x30))
                    add(f'damage.held.{label}.own{own:#x}.{victim}.h{health}', chars, rules, damage=6,
                        block=hit_block('cimph', 0xc, attack=l_plain))

    # A paralysed victim breaks free one hit in four; an iced one thaws.
    for flags, label in ((0x800000, 'paralysed'), (0x2000000, 'iced'), (0x2800000, 'both')):
        for roll in (0, 1, 3):
            chars, rules = damage_world(data, 'Araknid', 'Locke', health=40, me_objflags=flags)
            add(f'damage.{label}.r{roll}', chars, rules, damage=5, tape=[roll, 2])
        chars, rules = damage_world(data, 'Araknid', 'Locke', health=40, me_objflags=flags)
        add(f'damage.{label}.none', chars, rules, damage=0, tape=[0])

    # The death: the CHARIMPACT search by the side the target stands on,
    # stunned or down, raw or prefixed names, a loop or none, the share of
    # what was left; the stock names one by one; the given blocks.
    deaths = [impact('dfront', 0x1004, 'dloop', damagemin=0, damagemax=999),
              impact('dside', 0x1404, '', damagemin=0, damagemax=999),
              impact('dback', 0x1204, 'dloop', damagemin=0, damagemax=999),
              impact('dother', 0x1804, '', damagemin=0, damagemax=999)]
    for bearing in (0, 0x1f, 0x20, 0x5f, 0x60, 0x9f, 0xa0, 0xdf, 0xe0):
        for root in (('combat', 3), ('bow', 0x19), ('walk', 1)):
            chars, rules = damage_world(data, 'Araknid', 'Locke', health=4, bearing=bearing, impacts=deaths,
                                        me_root=block(root[0], root[1], obj='Target'),
                                        states=['combat', 'bow', 'walk', 'cdfront', 'dside', 'cdback', 'dloop',
                                                'dother', 'cdead'])
            add(f'damage.death.bearing{bearing:#x}.{root[0]}', chars, rules, damage=9)
        chars, rules = damage_world(data, 'Araknid', 'Locke', health=4, bearing=bearing, impacts=deaths,
                                    me_root=block('combat', 3), states=['combat', 'dfront', 'dside', 'dback', 'dloop'])
        add(f'damage.death.bearing{bearing:#x}.notarget', chars, rules, damage=9)
    when = [impact('dstun', 0x100c, '', damagemin=0, damagemax=999),
            impact('ddown', 0x1014, '', damagemin=0, damagemax=999),
            impact('dany', 0x1004, '', damagemin=0, damagemax=999)]
    for action in (3, 0xd, 0xe):
        chars, rules = damage_world(data, 'Araknid', 'Locke', health=4, impacts=when,
                                    me_doing=block('cimph', action, obj='Target'),
                                    states=['combat', 'cimph', 'dstun', 'ddown', 'dany'])
        add(f'damage.death.when.a{action}', chars, rules, damage=9)
    shares = [impact('dlow', 0x1004, '', damagemin=0, damagemax=40), impact('dmid', 0x1004, '', damagemin=41, damagemax=99),
              impact('dhigh', 0x1004, '', damagemin=100, damagemax=100), impact('dzero', 0x1004, '', damagemin=0,
                                                                                    damagemax=0)]
    for health, dmg in ((4, 9), (0, 0), (0, 5), (-3, 2), (-5, -3)):
        chars, rules = damage_world(data, 'Araknid', 'Locke', health=health, impacts=shares,
                                    states=['combat', 'dlow', 'dmid', 'dhigh', 'dzero'])
        add(f'damage.death.share.h{health}.d{dmg}', chars, rules, damage=dmg)
    for victim in ('Araknid', 'Hopper', 'Locke', 'Pale Ogrok'):
        vcd = s['chars'][victim]
        full = victim_states(vcd)
        variants = [('all', full), ('raw', [n for n in full if not n.startswith('c') or n == 'combat']),
                    ('noloops', [n for n in full if n not in [i['loopname'] for i in vcd.get('impacts', [])]]),
                    ('cdead', ['combat', 'cdead']), ('combattocdead', ['combat', 'combat to cdead']),
                    ('combattodead', ['combat', 'combat to dead']), ('dead', ['combat', 'dead']),
                    ('none', ['combat'])]
        for label, states in variants:
            for attacker in ('Locke', 'Araknid'):
                if attacker == victim:
                    continue
                for adoing in (None, block('swing', 7, attack=0, obj='Me')):
                    chars, rules = damage_world(data, victim, attacker, health=3, states=states, target_doing=adoing)
                    add(f'damage.death.{victim}.{label}.by{attacker}.{"swinging" if adoing else "idle"}', chars,
                        rules, damage=8)
            chars, rules = damage_world(data, victim, 'Locke', health=3, states=states)
            add(f'damage.death.{victim}.{label}.unsuitable', chars, rules, damage=8,
                block=hit_block('cimph', 0xc, attack=l_plain, impact=0 if locke['attacks'][l_plain].get('impacts')
                                else None))
            chars, rules = damage_world(data, victim, 'Locke', health=3, states=states)
            add(f'damage.death.{victim}.{label}.noattacker', chars, rules, damage=8, attacker=None)
    # Given death blocks: the attack's impact by the block's name, one past
    # the last, none; a death impact; an interactive attack's other impact;
    # the snaps and turns those impacts carry.
    turns = [impact('ia', 0x4, 'iloop'), impact('ib', 0x104), impact('ic', 0x204), impact('id', 0x404),
             impact('ie', 0x804)]
    snaps = [impact('if', 0x4, '', snapdist=24, snaptime=5), impact('ig', 0x0, '', snapdist=16, snaptime=3),
             impact('ih', 0x204, '', snapdist=8, snaptime=2), impact('ii', 0x0), impact('ij', 0x404),
             impact('ik', 0x80)]
    for attack_flags in (0, CA_INTERACTIVE):
        base = arak['attacks'][a_plain]
        flags = (base['flags'] & ~CA_INTERACTIVE) | attack_flags
        attacks = [synthetic(base, flags=flags, impacts=turns), synthetic(base, flags=flags, impacts=snaps),
                   synthetic(base, impacts=[])]
        for victim in ('Locke', 'Rahul'):
            for label, b in (
                    [(f'a0.imp{k}', hit_block(imp['name'], 0x13, attack=0, impact=k)) for k, imp in enumerate(turns)] +
                    [(f'a1.imp{k}', hit_block(imp['name'], 0x13, attack=1, impact=k)) for k, imp in enumerate(snaps)] +
                    [('byname', hit_block('id', 0x13, attack=0)), ('nomatch', hit_block('zz', 0x13, attack=0)),
                     ('nomatch.full', hit_block('zz', 0x13, attack=1)),
                     ('emptyattack', hit_block('zz', 0x13, attack=2)), ('noattack', hit_block('zz', 0x13)),
                     ('to', hit_block('combat to cdead', 0x13, attack=0, impact=0))]):
                for health, target_facing in ((3, None), (3, 0x30), (40, None)):
                    chars, rules = damage_world(data, victim, 'Araknid', health=health, attacks=attacks,
                                                states=['combat', 'ia', 'iloop', 'ib', 'ic', 'id', 'ie', 'if', 'ig',
                                                        'ih', 'ii', 'ij', 'ik', 'zz', 'combat to cdead', 'cdead',
                                                        'cimpact', 'impact'])
                    if target_facing is not None:
                        chars[1]['facing'] = chars[1]['moveangle'] = target_facing
                    add(f'damage.given.int{attack_flags:#x}.{victim}.{label}.h{health}.f{target_facing}', chars,
                        rules, damage=6, block=b)

    # The impact: blocking (the block impact, prefixed or not, none; an
    # interactive hit kept), the given block, the CHARIMPACT search, "impact".
    for victim, attacker in (('Locke', 'Araknid'), ('Araknid', 'Locke'), ('Hopper', 'Locke')):
        vcd = s['chars'][victim]
        acd = s['chars'][attacker]
        aplain = next(i for i, ad in enumerate(acd['attacks']) if not ad['flags'] & (CA_MAGICATTACK | CA_PLAYANIM))
        nimp = len(acd['attacks'][aplain].get('impacts', []))
        for label, states in (('both', ['combat', 'cblock', 'cblockimpact', 'blockimpact']),
                              ('raw', ['combat', 'cblock', 'blockimpact']), ('none', ['combat', 'cblock'])):
            for given in (None, hit_block('cimph', 0xc, attack=aplain, impact=0 if nimp else None)):
                chars, rules = damage_world(data, victim, attacker, health=50, states=states,
                                            me_doing=block('cblock', 8, obj='Target'))
                add(f'damage.blocking.{victim}.{label}.{"given" if given else "none"}', chars, rules, damage=4,
                    block=given)
        for dmg in (1, 3, 5, 7, 9, 12, 30, 99):
            for states_label, states in (('all', victim_states(vcd)), ('stock', ['combat', 'impact']),
                                         ('bare', ['combat'])):
                for given in (None, hit_block('cimph', 0xc, attack=aplain, impact=0 if nimp else None, wait=5)):
                    chars, rules = damage_world(data, victim, attacker, health=100, states=states)
                    add(f'damage.impact.{victim}.d{dmg}.{states_label}.{"given" if given else "none"}', chars, rules,
                        damage=dmg, block=given)
    held = synthetic(pale['attacks'][p_int])
    for k, imp in enumerate(held.get('impacts', [])):
        for victim in ('Locke', 'Araknid'):
            chars, rules = damage_world(data, victim, 'Pale Ogrok', health=60,
                                        me_doing=block('cblock', 8, obj='Target'),
                                        states=['combat', 'cblock', 'cblockimpact', imp['name']])
            add(f'damage.blocking.interactive.{victim}.i{k}', chars, rules, damage=4,
                block=hit_block(imp['name'], 0xc, attack=p_int, impact=k))
    # (`dmg` of `dmg + 100`: shares 5, 30, 50, 70, 90.)
    impacts = [impact('istun', 0x1001, 'sl', damagemin=0, damagemax=20, looptime=9),
               impact('idown', 0x1002, 'dl', damagemin=21, damagemax=40, looptime=7),
               impact('iplain', 0x1000, '', damagemin=41, damagemax=60),
               impact('ideath', 0x1004, '', damagemin=0, damagemax=100),
               impact('iright', 0x1400, '', damagemin=61, damagemax=80, snapdist=12, snaptime=4),
               impact('iback', 0x1200, '', damagemin=61, damagemax=100)]
    whens = [impact('wstun', 0x1008, '', damagemin=0, damagemax=100),
             impact('wdown', 0x1010, '', damagemin=0, damagemax=100),
             impact('wany', 0x1001, 'sl', damagemin=81, damagemax=100)]
    for dmg in (5, 30, 50, 70, 90):
        for bearing in (0x40, 0x80, 0x10, 0xc0):
            for action in (3, 0xd, 0xe):
                for imps, states in ((impacts, ['combat', 'istun', 'sl', 'idown', 'dl', 'iplain', 'iright', 'iback',
                                                'impact']),
                                     (impacts, ['combat', 'istun', 'idown', 'iplain']),
                                     (whens, ['combat', 'wstun', 'wdown', 'wany', 'sl'])):
                    chars, rules = damage_world(data, 'Araknid', 'Locke', health=dmg + 100, bearing=bearing,
                                                impacts=imps, states=states,
                                                me_doing=block('cimph', action, obj='Target'))
                    add(f'damage.impact.search.{imps[0]["name"]}.d{dmg}.b{bearing:#x}.a{action}.s{len(states)}',
                        chars, rules, damage=dmg)
    # Given impact blocks: the impact's snap and turns; a death impact on a
    # blow that doesn't kill.
    plain_turns = [impact('pa', 0x100), impact('pb', 0x200), impact('pc', 0x400), impact('pd', 0x800),
                   impact('pe', 0x0), impact('pf', 0x600)]
    for which, imps in enumerate((turns, snaps, plain_turns)):
        for k, imp in enumerate(imps):
            for victim in ('Locke', 'Rahul'):
                atk = synthetic(arak['attacks'][a_plain], impacts=imps)
                chars, rules = damage_world(data, victim, 'Araknid', health=40, attacks=[atk],
                                            states=['combat', imp['name'], 'impact', 'cimpact'])
                add(f'damage.given.impact.{victim}.s{which}.imp{k}', chars, rules, damage=6,
                    block=hit_block(imp['name'], 0xc, attack=0, impact=k))

    # The retreat a low-health hit starts.
    for victim in ('Under Druhg', 'Kantha', 'Araknid'):
        at = s['chars'][victim]['retreatat']
        for left in sorted({0, 1, at - 1, at, at + 1}):
            chars, rules = damage_world(data, victim, 'Locke', health=left + 6)
            add(f'damage.retreat.{victim}.left{left}', chars, rules, damage=6)

    # Kills and deaths in the player's frag record.
    for victim, attacker in (('Locke', 'Araknid'), ('Araknid', 'Locke'), ('Locke', 'Bayne'), ('Bayne', 'Locke')):
        for flags in (0, 0x40000):
            chars, rules = damage_world(data, victim, attacker, health=3, me_flags=flags)
            add(f'damage.frags.{victim}.by{attacker}.f{flags:#x}', chars, rules, damage=9)

    # The blow's number over the struck character (its animator's
    # ShowNumber, 0x004da3b0): minus the damage, at most the health it had;
    # red under a quarter of its maximum left after the blow, yellow under
    # half, else white; at its screen point (GetScreenPos 0x0046eb40), 135
    # up. A player's maximum is his MaxHealth (slot 0x1d8). Health is read
    # more often in retail's than in the port's (no side effect), so these
    # cases don't record the reads. Off the ground as well: GetScreenPos
    # truncates its float sums once (the kernel kata's screen-pos).
    numbers = {'Araknid': (30, [(30, 23), (30, 22), (30, 16), (30, 15), (30, 0), (5, 9), (40, 5), (1, 1)]),
               'Locke': (100, [(100, 76), (100, 75), (100, 51), (100, 50), (100, 0), (10, 40), (130, 2), (1, 1)])}
    for victim, (top, blows) in numbers.items():
        attacker = 'Locke' if victim != 'Locke' else 'Araknid'
        for health, dmg in blows:
            for at in (None, [1301, 700, 0], [600, 1500, 0], [1300, 700, 40], [1301, 700, 33]):
                chars, rules = damage_world(data, victim, attacker, health=health)
                chars[0]['animator'] = 1
                if victim == 'Locke':
                    chars[0]['maxhealth'] = top
                where = 'here' if at is None else f'{at[0]}_{at[1]}_{at[2]}'
                if at is not None:
                    chars[0]['pos'] = at
                add(f'damage.number.{victim}.h{health}.d{dmg}.{where}', chars, rules, damage=dmg, quiet_reads=True)
        for label, kw, extra in (('alreadydead', {}, dict(alreadydead=1)), ('unkillable', dict(me_flags=8), {}),
                                 ('invulnerable', dict(me_objflags=0x10000000), {}),
                                 ('typed', {}, dict(type=3, mod=40))):
            chars, rules = damage_world(data, victim, attacker, health=top // 2, **kw)
            chars[0]['animator'] = 1
            if victim == 'Locke':
                chars[0]['maxhealth'] = top
            add(f'damage.number.{victim}.{label}', chars, rules, damage=top // 3, quiet_reads=True, **extra)
    return finish(cases)


# ---- C5: the resolvers, Block, StopBlock, Dodge ----------------------------------------

def resolver_cases(data: Path, workdir: Path) -> list[dict]:
    s = shipped(data, workdir)
    cases = []
    seed = 23

    def add(name, call, chars, rules, *, args=None, **kw):
        nonlocal seed
        seed += 1
        cases.append(case(name, call, chars, rules, args=args or {}, seed=seed * 6271 + 9, seam_damage=False,
                          tape=kw.pop('tape', [1, 2, 3]), **kw))

    arak = s['chars']['Araknid']
    a_plain = next(i for i, ad in enumerate(arak['attacks']) if not ad['flags'] & (CA_MAGICATTACK | CA_PLAYANIM))
    a_play = first_with(arak, CA_PLAYANIM)
    CA_BLOOD = 0x200

    def world(victim='Araknid', *, cd_changes=None, attacks=None, foe_attacks=None, states=None, commanddone=0, **kw):
        foe = 'Araknid' if victim == 'Locke' else 'Locke'
        chars, rules = damage_world(data, victim, foe, states=states, attacks=foe_attacks, **kw)
        if cd_changes:
            chars[0]['chardata'].update(cd_changes)
        if attacks is not None:
            chars[0]['chardata']['attacks'] = attacks
        chars[0]['commanddone'] = commanddone
        return chars, rules

    # ResolveImpact: the impale's blood (one tick in six), the first tick's
    # blood by the share of health and the attack, the flash; when the
    # animation ends, the root once the wait is out, else the impact's loop
    # (forced while playing the last attack, or a PLAYANIM one). The victim
    # has always attacked: retail reads its last attack unguarded there.
    loops = [impact('ihit', 0x2, 'iloop', looptime=8), impact('impale', 0x1, 'ploop', looptime=5),
             impact('inoloop', 0x2, '')]
    base = arak['attacks'][a_plain]
    own = [synthetic(base, flags=base['flags'] | CA_BLOOD, impacts=loops),
           synthetic(base, name='ihit', flags=base['flags'] & ~CA_BLOOD, impacts=loops)]
    if a_play is not None:
        own.append(copy.deepcopy(arak['attacks'][a_play]))
    states = ['combat', 'ihit', 'iloop', 'impale', 'ploop', 'inoloop', own[0]['name']] + \
             ([own[2]['name']] if a_play is not None else [])
    for name, k in (('ihit', 0), ('impale', 1), ('inoloop', 2), ('other', 0)):
        for attack in (0, 1, None):
            for first in (True, False):
                for done in (0, 1):
                    for wait in (0, 6):
                        for bleed in ((0, 1), (0, 0), (0x10, 1)):
                            for dmg in (0, 1, 2, 40):
                                b = block(name, 0xc, impact=k, impact_attack=0, damage=dmg, wait=wait, obj='Target',
                                          flags=['firsttime'] if first else [])
                                if attack is not None:
                                    b['attack'] = attack
                                chars, rules = world(attacks=own, states=states, commanddone=done, me_doing=b,
                                                     cd_changes=dict(flags=bleed[0], bleeder=bleed[1], health=100),
                                                     me_state=dict(lastattack=0))
                                add(f'resolve-impact.{name}.a{attack}.f{int(first)}.d{done}.w{wait}.b{bleed[0]:#x}'
                                    f'{bleed[1]}.dmg{dmg}', 'resolve-impact', chars, rules, tape=[1, 0, 1])
    # The loop's turn: the last attack (the one the doing block plays,
    # another, a PLAYANIM); the impact's loop missing.
    for label, state in (('own', dict(lastattack=1)), ('other', dict(lastattack=0)),
                         ('playanim', dict(lastattack=2 if a_play is not None else 0))):
        for doing_name in ('ihit', 'cimpx'):
            for st in (states, [n for n in states if n != 'iloop']):
                b = block(doing_name, 0xd, impact=0, impact_attack=0, wait=6, obj='Target')
                chars, rules = world(attacks=own, states=st, commanddone=1, me_doing=b, me_state=state)
                add(f'resolve-impact.loop.{label}.{doing_name}.s{len(st)}', 'resolve-impact', chars, rules)

    # ResolveBlock: facing the guard's target, the wait, a stop.
    for obj in ('Target', None):
        for wait in (0, 1, 9):
            for stop in (False, True):
                for done in (0, 1):
                    b = block('cblock', 8, wait=wait, obj=obj, flags=['stop'] if stop else [])
                    chars, rules = world(me_doing=b, commanddone=done, bearing=0x50)
                    chars[0]['facing'] = chars[0]['moveangle'] = 0x10
                    add(f'resolve-block.o{obj}.w{wait}.s{int(stop)}.d{done}', 'resolve-block', chars, rules)

    # ResolveDead: the first tick's blood and flash; the death's loop.
    deaths = [impact('heavydeath', 0x1004, 'death'), impact('quiet', 0x1004, '')]
    datk = [synthetic(base, flags=base['flags'] | CA_BLOOD, impacts=deaths),
            synthetic(base, flags=base['flags'] & ~CA_BLOOD, impacts=deaths)]
    for name, k in (('heavydeath', 0), ('death', 0), ('quiet', 1), ('nimp', None)):
        for attack in (0, 1, None):
            for first in (True, False):
                for done in (0, 1):
                    for bleed in ((0, 1), (0x10, 1), (0x10, 0)):
                        for st in (['combat', 'heavydeath', 'death', 'quiet'], ['combat', 'heavydeath', 'quiet']):
                            b = block(name, 0x13, damage=30, obj='Target', flags=['firsttime'] if first else [])
                            if k is not None:
                                b.update(impact=k, impact_attack=0)
                            if attack is not None:
                                b['attack'] = attack
                            chars, rules = world(attacks=datk, states=st, commanddone=done, me_doing=b,
                                                 cd_changes=dict(flags=bleed[0], bleeder=bleed[1]))
                            add(f'resolve-dead.{name}.a{attack}.f{int(first)}.d{done}.b{bleed[0]:#x}{bleed[1]}'
                                f'.s{len(st)}', 'resolve-dead', chars, rules)

    # Block: the root, flinching, dead, held, the target's reach; a
    # monster's timing against the attacker's block time (or forced, -2);
    # the player needs none; the duration drawn or given; no-turn.
    for victim in ('Araknid', 'Locke'):
        tcd = s['chars']['Locke' if victim == 'Araknid' else 'Araknid']
        tplain = next(ad for ad in tcd['attacks'] if not ad['flags'] & (CA_MAGICATTACK | CA_PLAYANIM))
        # The attacker's table: an interactive attack (to hold the victim), the
        # blow the guard times.
        foe = [synthetic(tplain, add_flags=CA_INTERACTIVE, impacts=[impact('hold', CAI_INTERACTIVE)]),
               copy.deepcopy(tplain)]
        ti, bt = 1, tplain['blocktime']
        for label, kw, extra in (
                ('plain', {}, {}),
                ('bowroot', dict(me_root=block('bow', 0x19, obj='Target')), {}),
                ('walkroot', dict(me_root=block('walk', 1, obj='Target')), {}),
                ('notarget', dict(me_root=block('combat', 3)), {}),
                ('flinching', dict(me_doing=block('cimph', 0xc, obj='Target')), {}),
                ('blocking', dict(me_doing=block('cblock', 8, obj='Target')), {}),
                ('nodoingobj', dict(me_doing=block('combat', 3)), {}),
                ('dead', dict(health=0), {}),
                ('held', dict(me_doing=block('cheld', 7, attack=0, attack_of='Target', obj='Target')), {}),
                ('heldimpact', dict(me_doing=block('cheld', 3, impact=0, impact_attack=0, impact_of='Target',
                                                   obj='Target')), {}),
                ('refused', dict(me_doing=block('combat', 3, obj='Target', flags=['priority']),
                                 me_desired=block('cnext', 3)), {}),
                ('held.own', dict(me_doing=block('cheld', 7, attack=0, attack_of='Target', obj='Target'),
                                  me_flags=0x80000), {}),
                ('far', dict(dist=121), {}), ('reach', dict(dist=120), {}),
                ('noturn', dict(me_flags=4), {}),
                ('noanim', dict(states=['combat']), {}),
                ('rawanim', dict(states=['combat', 'block']), {})):
            for tframe, tdoing in ((0, block('swing', 7, attack=ti, obj='Me')), (bt - 1, block('swing', 7, attack=ti,
                                                                                               obj='Me')),
                                   (bt, block('swing', 7, attack=ti, obj='Me')), (0, None)):
                for frames in (-2, -1, 7):
                    kw2 = dict(kw)
                    kw2.setdefault('me_doing', block('combat', 3, obj='Target'))
                    kw2.setdefault('states', ['combat', 'cblock', 'block', 'wblock', 'hblock'])
                    chars, rules = world(victim, target_doing=tdoing, foe_attacks=foe, **kw2)
                    chars[1]['frame'] = tframe
                    chars[0]['facing'] = 0x30
                    add(f'block.{victim}.{label}.t{tframe if tdoing else "idle"}.fr{frames}', 'block', chars, rules,
                        args=dict(frames=frames), seam_block=False, tape=[4, 9], **extra)
    # StopBlock.
    for root in (('combat', 3), ('bow', 0x19), ('walk', 1)):
        for doing in (block('cblock', 8, wait=9), block('combat', 3)):
            chars, rules = world(me_root=block(root[0], root[1], obj='Target'), me_doing=doing)
            add(f'stop-block.{root[0]}.{doing["name"]}', 'stop-block', chars, rules)

    # Dodge: the gates, the "hand" root, the roll by facing sector and
    # direction.
    rolls = ['croll' + c for c in 'bflr'] + ['hroll' + c for c in 'bflr']
    for facing in (0x00, 0x0f, 0x10, 0x30, 0x31, 0x50, 0x51, 0x70, 0x71, 0x91, 0x92, 0xb2, 0xb3, 0xd2, 0xd3, 0xf2, 0xf3):
        for d in (-1, 0, 1, 2, 3, 4, 5, 6, 7, 8):
            chars, rules = world('Locke', states=['combat'] + rolls)
            chars[0]['facing'] = chars[0]['moveangle'] = facing
            add(f'dodge.f{facing:#x}.d{d}', 'dodge', chars, rules, args=dict(dir=d))
    for label, kw in (('handroot', dict(me_root=block('hand', 3, obj='Target'))),
                      ('bowroot', dict(me_root=block('bow', 0x19, obj='Target'))),
                      ('walkroot', dict(me_root=block('walk', 1, obj='Target'))),
                      ('moving', dict(me_doing=block('cwalkf', 4, obj='Target'))),
                      ('dead', dict(health=0)),
                      ('held', dict(me_doing=block('cheld', 3, attack=0, attack_of='Target', obj='Target'))),
                      ('heldimpact', dict(me_doing=block('cheld', 3, impact=0, impact_attack=0, impact_of='Target',
                                                         obj='Target'))),
                      ('refused', dict(me_doing=block('combat', 3, obj='Target', flags=['priority']),
                                       me_desired=block('cnext', 3))),
                      ('held.own', dict(me_doing=block('cheld', 3, attack=0, attack_of='Target', obj='Target'),
                                        me_flags=0x80000)),
                      ('noanim', dict(states=['combat']))):
        for victim in ('Locke', 'Araknid'):
            for pstate in (0, 2, 3):
                kw2 = dict(kw)
                kw2.setdefault('states', ['combat'] + rolls)
                tcd = s['chars']['Locke' if victim == 'Araknid' else 'Araknid']
                tplain = next(ad for ad in tcd['attacks'] if not ad['flags'] & (CA_MAGICATTACK | CA_PLAYANIM))
                chars, rules = world(victim, foe_attacks=[synthetic(tplain, add_flags=CA_INTERACTIVE,
                                                                    impacts=[impact('hold', CAI_INTERACTIVE)])], **kw2)
                chars[0]['facing'] = chars[0]['moveangle'] = 0x40
                if victim == 'Locke':
                    chars[0]['playerstate'] = pstate
                elif pstate:
                    continue
                add(f'dodge.{label}.{victim}.p{pstate}', 'dodge', chars, rules, args=dict(dir=1))
    return finish(cases)


# ---- C3b: AI ----------------------------------------------------------------------------

def ai_cases(data: Path, workdir: Path) -> list[dict]:
    s = shipped(data, workdir)
    cases = []
    seed = 31

    def add(name, chars, rules, **kw):
        nonlocal seed
        seed += 1
        cases.append(case(name, 'ai', chars, rules, args={}, seed=seed * 7907 + 1, tape=kw.pop('tape', [40, 7, 1, 2]),
                          ground=dict(z=ME_AT[2]), **kw))

    def world(me='Araknid', tg='Locke', *, aistate=None, me_stats=None, target_stats=None, **kw):
        stats = dict(dict(aggressive=1, sleeping=0), **(me_stats or {}))
        tstats = dict(dict(sleeping=0), **(target_stats or {}))
        chars, rules = full_world(data, me, tg, me_stats=stats, target_stats=tstats, **kw)
        if aistate is not None:
            chars[0]['aistate'] = aistate
        return chars, rules

    def third(at, name='Third', cls='Araknid', **kw):
        return make_char(name, 12, at, 0, chardata(s['chars'][cls]), states=['combat', 'sidestepl', 'sidestepr'],
                         stats=dict(MONSTER_STATS, aggressive=1, sleeping=0), ident=0x30, **kw)

    # The gates: a player without the AI switch, disabled (moving or not),
    # dead, the dummies cheat, the editor.
    for label, kw, extra in (
            ('plain', {}, {}),
            ('disabled', dict(me_objflags=0x40), {}),
            ('disabled.moving', dict(me_objflags=0x40, me_doing=block('walk', 2, obj='Target')), {}),
            ('disabled.combatstep', dict(me_objflags=0x40, me_doing=block('cwalkf', 4, obj='Target')), {}),
            ('dead', dict(me_stats=dict(health=0)), {}),
            ('dead.moving', dict(me_stats=dict(health=0), me_doing=block('walk', 2, obj='Target')), {}),
            ('dummies', {}, dict(globals_extra=dict(ai_off=1))),
            ('editor', {}, dict(globals_extra=dict(editor=1)))):
        chars, rules = world(**kw)
        add(f'ai.gate.{label}', chars, rules)
        cases[-1]['globals'].update(extra.get('globals_extra', {}))
    for flags in (0, 0x100000):
        chars, rules = world('Locke', 'Araknid', me_flags=flags, target_root=block('combat', 3, obj='Me'))
        add(f'ai.gate.player.f{flags:#x}', chars, rules)

    # The target: the root's, kept while valid; else every 32 frames
    # (phase by id), aggressive and not no-turn, the search; a retreating
    # monster takes it without fighting.
    for label, kw in (('valid', {}), ('dead', dict(target_stats=dict(health=0))), ('far', dict(dist=500)),
                      ('untargetable', dict(target_flags=0x8000)),
                      ('noroot', dict(me_root=block('walk', 1))), ('rootnotarget', dict(me_root=block('combat', 3)))):
        for frame in (0x10, 0x20, 0x30):
            for found in ('Target', None):
                for aggressive in (1, 0):
                    for flags in (0, 4):
                        for retreat in (0, 1):
                            chars, rules = world(me_stats=dict(aggressive=aggressive), me_flags=flags, **kw)
                            chars[0]['id'] = 0x10
                            if retreat:
                                chars[0]['retreating'] = 1
                                chars[0]['retreat_frames'] = 5
                            add(f'ai.target.{label}.fr{frame:#x}.found{found}.ag{aggressive}.f{flags}.r{retreat}',
                                chars, rules, frame=frame, found=found)

    # The attack branch: in the stance, the walk of a walk-fighter, or asked
    # by OnAttacked; the timers; still making the last attack; out of
    # reach; in reach, the attack chosen or none, someone in the way.
    acd = s['chars']['Araknid']
    a0 = next(i for i, ad in enumerate(acd['attacks']) if not ad['flags'] & (CA_MAGICATTACK | CA_PLAYANIM))
    play = first_with(acd, CA_PLAYANIM)
    for doing_label, doing, flags, req in (
            ('combat', None, 0, 0), ('walk', block('walk', 1, obj='Target'), 0, 0),
            ('walk.fighter', block('walk', 1, obj='Target'), 0x4000, 0),
            ('impact.asked', block('cimph', 0xc, obj='Target'), 0, 3), ('impact.asked1', block('cimph', 0xc, obj='Target'), 0, 1),
            ('impact.asked6', block('cimph', 0xc, obj='Target'), 0, 6)):
        for timers in ((0, 0), (1, 1), (2, 1), (1, 3), (3, 3), (0, 5), (5, 0)):
            for dist in (10, 60):
                for last in (None, a0, play):
                    state = dict(nextattack=timers[0], magictimer=timers[1], requestbits=req)
                    if last is not None:
                        state['lastattack'] = last
                    kw = dict(me_doing=doing) if doing else {}
                    chars, rules = world(me_state=state, me_flags=flags, dist=dist, **kw)
                    add(f'ai.attack.{doing_label}.t{timers[0]}{timers[1]}.d{dist}.l{last}', chars, rules,
                        tape=[40, 7, 3, 9, 50, 2, 1, 33])
    # Still making the last attack (the doing block plays it).
    chars, rules = world(me_state=dict(nextattack=0, magictimer=0, lastattack=a0),
                         me_doing=block(acd['attacks'][a0]['name'], 7, attack=a0, obj='Target'))
    add('ai.attack.stillmaking', chars, rules)
    # Someone in the way: a third character in line, at each side.
    for side in (0, 0x30, 0x50, 0x70, -0x30, -0x50, -0x70):
        for dist in (10, 25):
            chars, rules = world(me_state=dict(nextattack=0, magictimer=0), dist=dist)
            facing = chars[0]['facing']
            chars.append(third(_toward(ME_AT, (facing + side) & 0xff, 20)))
            add(f'ai.attack.blocker.s{side}.d{dist}', chars, rules, tape=[99, 99, 99, 99])

    # The move branch: retreating (run; or no-turn), standing (step aside of
    # a blocker, he the other way unless the target), stepping (stop near the
    # target, wait out a PLAYANIM, chase seen or heard, else search).
    for flags in (0, 4):
        for lookat in (None, 'Target'):
            chars, rules = world(me_doing=block('cwalkf', 4, obj='Target'), me_flags=flags,
                                 aistate=dict(lookat=lookat) if lookat else None)
            chars[0]['retreating'] = 1
            chars[0]['retreat_latch'] = 1
            chars[0]['retreat_frames'] = 9
            add(f'ai.move.retreat.f{flags}.l{lookat}', chars, rules)
    for accum in ([0, 0, 0], [2, 0, 0]):
        for blocker in ('Target', 'Third', None):
            for blocked in (True, False):
                chars, rules = world(me_doing=block('cimph', 0xc, obj='Target'))
                chars[0]['accum'] = accum
                chars.append(third(_toward(ME_AT, 0x90, 40)))
                add(f'ai.move.standing.a{accum[0]}.b{blocker}.{int(blocked)}', chars, rules,
                    blocked=blocked, blocker=blocker)
    for action in (2, 4, 0x1a):
        for dist in (10, 40, 120):
            for lookat in (None, 'Third'):
                for sees in (True, False):
                    for noise in (0, 50, 101):
                        for invisible in (0, 0x80):
                            chars, rules = world(me_doing=block('cwalkf', action, obj='Target'), dist=dist,
                                                 target_objflags=invisible,
                                                 aistate=dict(lookat=lookat, waypointticks=2, lastpos=[900, 950, 0])
                                                 if lookat else dict(lastpos=[900, 950, 0]))
                            chars[1]['noise'] = noise
                            chars.append(third(_toward(ME_AT, 0x40, 3)))
                            add(f'ai.move.step.a{action}.d{dist}.l{lookat}.s{int(sees)}.n{noise}.i{invisible}',
                                chars, rules, sees=sees)
    # A PLAYANIM under way; the waypoint's ticks in a chase; lost, with the
    # target fighting someone else in sight.
    if play is not None:
        chars, rules = world(me_doing=block(acd['attacks'][play]['name'], 4, obj='Target'), dist=60,
                             me_state=dict(lastattack=play))
        add('ai.move.step.playanim', chars, rules)
    for ticks in (0, 1, 3):
        chars, rules = world(me_doing=block('cwalkf', 4, obj='Target'), dist=60,
                             aistate=dict(lookat='Third', waypointticks=ticks))
        chars.append(third(_toward(ME_AT, 0x20, 200)))
        add(f'ai.move.chase.ticks{ticks}', chars, rules, sees=True)
    for root in ('Third', 'Me', None):
        for roll in (0, 1):
            for lookat, near in ((None, False), ('Third', True), ('Third', False)):
                chars, rules = world(me_doing=block('cwalkf', 4, obj='Target'), dist=60,
                                     target_root=block('combat', 3, obj=root),
                                     aistate=dict(lookat=lookat, lastpos=[800, 1200, 0]) if lookat
                                     else dict(lastpos=[800, 1200, 0]))
                chars[1]['noise'] = 0
                chars.append(third(_toward(ME_AT, 0x60, 2 if near else 300)))
                add(f'ai.move.lost.root{root}.r{roll}.l{lookat}.{"near" if near else "far"}', chars, rules,
                    sees=False, tape=[roll, 5, 5])

    # Bow roots: the target kept, standing beside a blocker, the lost
    # target's own target.
    for label, kw in (('bow', dict(me_root=block('bow', 0x19, obj='Target'))),
                      ('bow.standing', dict(me_root=block('bow', 0x19, obj='Target'),
                                            me_doing=block('cimph', 0xc, obj='Target'))),
                      ('walk.standing', dict(me_root=block('walk', 1, obj='Target'),
                                             me_doing=block('cimph', 0xc, obj='Target')))):
        for blocker in ('Target', 'Third'):
            chars, rules = world(**kw)
            chars.append(third(_toward(ME_AT, 0x90, 40)))
            add(f'ai.root.{label}.b{blocker}', chars, rules, blocked=True, blocker=blocker)
    for root in (('bow', 0x19), ('walk', 1)):
        chars, rules = world(me_doing=block('cwalkf', 4, obj='Target'), dist=150,
                             target_root=block(root[0], root[1], obj='Third'))
        chars[1]['noise'] = 0
        chars.append(third(_toward(ME_AT, 0x60, 300)))
        add(f'ai.move.lost.targetroot.{root[0]}', chars, rules, sees=['Third'], tape=[1, 5])
    # The magic timer's own re-arm; both run out.
    for timers in ((3, -1), (-1, -1), (-1, 4), (0, -1)):
        chars, rules = world(me_state=dict(nextattack=timers[0], magictimer=timers[1]))
        add(f'ai.attack.rearm.t{timers[0]}.{timers[1]}', chars, rules, tape=[5, 9, 2, 7])
    # Stepping while the last attack plays (a PLAYANIM waits; another
    # doesn't), far from the target.
    for last, name in ((play, acd['attacks'][play]['name'] if play is not None else 'x'),
                       (a0, acd['attacks'][a0]['name']), (a0, 'cwalkf')):
        if last is None:
            continue
        chars, rules = world(me_doing=block(name, 4, obj='Target'), dist=150, me_state=dict(lastattack=last))
        add(f'ai.move.step.last{last}.{name}', chars, rules)
    # Lost, the target fighting someone in sight: both step aside.
    for roll in (0, 1):
        chars, rules = world(me_doing=block('cwalkf', 4, obj='Target'), dist=150,
                             target_root=block('combat', 3, obj='Third'))
        chars[1]['noise'] = 0
        chars.append(third(_toward(ME_AT, 0x60, 300)))
        add(f'ai.move.lost.otherseen.r{roll}', chars, rules, sees=['Third'], tape=[roll, 5])

    # The waypoint search: none, one in sight or behind a wall, another
    # helper type, the nearest under 1000 (else the first), the committed
    # one found again (let go), more than ten.
    lost = dict(me_doing=block('cwalkf', 4, obj='Target'), dist=150)
    at = ME_AT
    for label, wps, walls, look in (
            ('none', [], [], None),
            ('one', [dict(name='w1', pos=[at[0] + 100, at[1], 0])], [], None),
            ('walled', [dict(name='w1', pos=[at[0] + 100, at[1], 0])], [['w1', 'Me']], None),
            ('axis', [dict(name='a1', pos=[at[0] + 100, at[1], 0], type='Axis')], [], None),
            ('nearest', [dict(name='w1', pos=[900, 950, 0]), dict(name='w2', pos=[700, 700, 0]),
                         dict(name='w3', pos=[880, 940, 0])], [], None),
            ('allfar', [dict(name='w1', pos=[4000, 4000, 0]), dict(name='w2', pos=[3000, 3000, 0])], [], None),
            ('mixed', [dict(name='w1', pos=[4000, 4000, 0]), dict(name='w2', pos=[905, 951, 0]),
                       dict(name='a1', pos=[900, 950, 0], type='Axis')], [['w2', 'Me']], None),
            ('eleven', [dict(name=f'w{i}', pos=[900 + 40 * i, 950, 0]) for i in range(11)], [], None)):
        for lastpos in ([900, 950, 0], [0, 0, 0]):
            ai = dict(lastpos=lastpos)
            chars, rules = world(**lost, aistate=ai)
            chars[1]['noise'] = 0
            c = dict(sees=False, waypoints=wps, walls=walls)
            add(f'ai.move.waypoints.{label}.c{lastpos[0]}', chars, rules, **c)
    # Committed to a waypoint: near it (search again from the target's
    # place), or still away (keep facing it).
    for near in (True, False):
        for found_again in (True, False):
            wp = dict(name='w1', pos=[ME_AT[0] + (2 if near else 200), ME_AT[1], 0])
            wps = [wp] if found_again else [dict(name='w2', pos=[ME_AT[0] + 60, ME_AT[1] + 60, 0])]
            chars, rules = world(**lost, aistate=dict(lastpos=[900, 950, 0], lookat='w1', waypointticks=3))
            chars[1]['noise'] = 0
            add(f'ai.move.waypoints.committed.n{int(near)}.f{int(found_again)}', chars, rules, sees=False,
                waypoints=wps + ([wp] if not found_again else []))

    # The retreat state: health against RETREATAT, the latch, the frames.
    for victim in ('Kantha', 'Under Druhg', 'Araknid'):
        at = s['chars'][victim]['retreatat']
        for health in sorted({1, at, at + 1, 50}):
            for latch in (0, 1):
                for frames in (0, 1, 7):
                    chars, rules = world(victim, me_stats=dict(health=health))
                    chars[0]['retreat_latch'] = latch
                    chars[0]['retreat_frames'] = frames
                    add(f'ai.retreat.{victim}.h{health}.l{latch}.f{frames}', chars, rules)

    # The boss kind: once, by type name (not for the bosses themselves,
    # whose behaviours aren't ported).
    for kind in (0, -1, 5):
        chars, rules = world()
        chars[0]['monsterkind'] = kind
        add(f'ai.kind.{kind}', chars, rules)
    return finish(cases)


# ---- C6: the character's tick (forensics/CHARACTER_PULSE.md) ---------------------------

# The tick's own state, by retail's names (melee_attack.py PULSESTATE): the
# recovery stamps and accumulators, the timers, the fade, the corpse.
PULSE_STATE = dict(lasthealth=0, lastfatigue=0, lastmana=0, lastrun=-1, accumhealth=0, accumfatigue=0,
                   accummana=0, accumrun=0, accumpoison=0, lastpoison=-1, invokedelay=0, fade=100, fadestep=0,
                   fadelimit=100, fadedir=0, burnticks=0, deadticks=0, exittimestamp=0, forcecommanddone=0)
PLAYER_PULSE_STATE = dict(deathcountdown=0xc0, stateframes=0, statetime=0, stateffectexpires=-1)
NOW = 20000                       # the clock a case's tick starts on (PlayScreen game time, the play clock)


def pulse_char(data, name, typ, *, pos=None, facing=64, ident=0x10, stats=None, pulse=None, states=('walk',),
               root=None, doing=None, desired=None, chardata_changes=None, **spec):
    """A character for the tick: `typ`'s shipped data, its own stats over
    the class's, an animator, the tick state over PULSE_STATE (a player's
    over PLAYER_PULSE_STATE too, its play clock at NOW)."""
    s = shipped(data, None)
    cd = chardata(s['chars'][typ])
    cd.update(chardata_changes or {})
    cls = 11 if typ in PLAYERS else 12
    base = dict(PLAYER_STATS, healthpct=0, manapct=0, fatiguepct=0) if cls == 11 else dict(MONSTER_STATS)
    base.update(poisoned=0, sleeping=0, aggressive=1, health=cd['health'], fatigue=cd['fatigue'], mana=cd['mana'])
    c = make_char(name, cls, pos or ME_AT, facing, cd, typ=typ, states=states,
                  root=root or dict(name='walk', action=1), doing=doing, desired=desired,
                  stats=dict(base, **(stats or {})), ident=ident,
                  charflags=spec.pop('charflags', 0), objflags=spec.pop('objflags', 0), frame=spec.pop('frame', 0))
    st = dict(PULSE_STATE, lasthealth=NOW, lastfatigue=NOW, lastmana=NOW)
    if cls == 11:
        st.update(PLAYER_PULSE_STATE, statetime=NOW, stateframes=NOW * 24 // 100)
        c.update(maxhealth=100, maxfatigue=78, maxmana=105)
    st.update(pulse or {})
    c['pulsestate'] = st
    c['animator'] = 1
    c.update(spec)
    return c


def pulse_cases(data: Path, workdir: Path) -> list[dict]:
    s = shipped(data, workdir)
    rules = s['rules']
    cases = []
    seed = 101

    def add(name, chars, *, ticks=1, self_='Me', frame=0x200, gametime=NOW, screenframe=500, tape=None, **kw):
        nonlocal seed
        seed += 1
        g = dict(gametime=gametime, screenframe=screenframe, autobegincombat=kw.pop('autobegincombat', 1),
                 pvp=kw.pop('pvp', 0), editor=kw.pop('editor', 0), control=kw.pop('control', 1),
                 ps_5d8=kw.pop('ps_5d8', 0))
        c = case(name, 'pulse', chars, rules, self_=self_, args=dict(ticks=ticks), seed=seed * 7919 + 3,
                 tape=tape or [], frame=frame, **kw)
        c['globals'].update(g)
        cases.append(c)

    def one(name, typ='Araknid', *, ticks=1, case_kw=None, **kw):
        add(name, [pulse_char(data, 'Me', typ, **kw)], ticks=ticks, **(case_kw or {}))

    # Recovery, one vital at a time below its maximum: the step's share by
    # the stamp's age, the accumulator's carry and its 10000 edge, the
    # stamps pulled from the future and from more than 50000 back.
    ages = (0, 4, 5, 100, 6000, 21474, 21475, 30000, 42950, 50000, 50001, -5)
    for typ in ('Araknid', 'Locke'):
        cd = s['chars'][typ]
        for vital, last, accum in (('health', 'lasthealth', 'accumhealth'), ('fatigue', 'lastfatigue', 'accumfatigue'),
                                   ('mana', 'lastmana', 'accummana')):
            top = {'Locke': dict(health=100, fatigue=78, mana=105)}.get(typ, cd)[vital]
            if top <= 0:
                continue
            for age in ages:
                for acc in (0, 9999, 10000, 25000, -20000):
                    one(f'pulse.recover.{typ}.{vital}.age{age}.acc{acc}', typ, stats={vital: top // 2},
                        pulse={last: NOW - age, accum: acc})
            for acc in (0, 9999, 30001):
                one(f'pulse.recover.{typ}.{vital}.full.acc{acc}', typ, stats={vital: top},
                    pulse={last: NOW - 400, accum: acc})
                one(f'pulse.recover.{typ}.{vital}.over.acc{acc}', typ, stats={vital: top + 3},
                    pulse={last: NOW - 400, accum: acc})
            one(f'pulse.recover.{typ}.{vital}.nearly', typ, stats={vital: top - 1},
                pulse={last: NOW - 9000, accum: 5000})
    # The player's per-minute percentages, signed.
    for pct in (0, 1, 7, 60, 144, -30, -1441):
        one(f'pulse.recover.Locke.pct{pct}', 'Locke', stats=dict(health=50, fatigue=30, mana=40, healthpct=pct,
                                                                 manapct=pct + 3, fatiguepct=-pct),
            pulse=dict(lasthealth=NOW - 4, lastfatigue=NOW - 4, lastmana=NOW - 4))
    # How fast it really goes: ten seconds and a minute from low.
    for typ in ('Araknid', 'Locke'):
        for ticks in (240, 1440):
            one(f'pulse.feel.{typ}.t{ticks}', typ, ticks=ticks,
                stats=dict(health=3, fatigue=0, mana=0, **({'healthpct': 25, 'manapct': 10, 'fatiguepct': 4}
                                                          if typ == 'Locke' else {})))

    # Running costs fatigue: the run roots (combat / bow by action), only
    # while a step plays (doing isn't the root), from the second tick, not
    # below 0; the other roots recover.
    runs = (('run', 2, 'run'), ('combatrun', 3, 'combatrun'), ('handrun', 3, 'handrun'), ('bowrun', 25, 'bowrun'),
            ('combatrun.walkaction', 1, 'combatrun'), ('bowrun.combat', 3, 'bowrun'), ('walk', 1, 'walk'),
            ('sneak', 1, 'sneak'))
    for typ in ('Locke', 'Araknid'):
        for label, action, rname in runs:
            for lastrun, fat in ((-1, 50), (NOW - 4, 50), (NOW - 400, 50), (NOW - 4, 0), (NOW - 4000, 3)):
                for stepping in (True, False):
                    root = dict(name=rname, action=action)
                    doing = dict(name='runl', action=2) if stepping else None
                    one(f'pulse.run.{typ}.{label}.last{lastrun}.f{fat}.step{int(stepping)}', typ,
                        states=['walk', rname, 'runl'], root=root, doing=doing, stats=dict(fatigue=fat),
                        pulse=dict(lastrun=lastrun, lastfatigue=NOW - 4, accumrun=7000))
        for rf in ([1, 100], [10, 100], [7, 30], [0, 100]):
            one(f'pulse.run.{typ}.rf{rf[0]}_{rf[1]}', typ, ticks=48, states=['walk', 'run', 'runl'],
                root=dict(name='run', action=2), doing=dict(name='runl', action=2), stats=dict(fatigue=40),
                chardata_changes=dict(runfatigue=rf), pulse=dict(lastrun=NOW))

    # Poison: through Damage, short of the last point, cured at 1; reduced
    # by the poison resistance; the first tick only starts the clock.
    for typ in ('Araknid', 'Locke'):
        for health in (30, 2, 1):
            for age in (-1, 4, 100, 1491, 1492, 5000, 50000):
                for resist in (0, 50, 100, 130, -40):
                    kw = {}
                    if typ == 'Locke':
                        kw['resists'] = [0] * 9 + [resist]
                    else:
                        kw['chardata_changes'] = dict(damagemods=[0] * 9 + [resist])
                    one(f'pulse.poison.{typ}.h{health}.age{age}.r{resist}', typ,
                        stats=dict(health=health, poisoned=1),
                        pulse=dict(lastpoison=-1 if age < 0 else NOW - age, accumpoison=3000), **kw)
        one(f'pulse.poison.{typ}.cured', typ, stats=dict(poisoned=0, health=5), pulse=dict(lastpoison=NOW - 50))
    # A minute of poison, the blows real (Damage's impacts) and seamed.
    for typ in ('Araknid', 'Locke'):
        cd = s['chars'][typ]
        for real in (False, True):
            one(f'pulse.poison.feel.{typ}.real{int(real)}', typ, ticks=240,
                states=['walk', 'combat'] + victim_states(cd), stats=dict(health=12, poisoned=1),
                pulse=dict(lastpoison=NOW), case_kw=dict(seam_damage=not real))

    # The run and poison stamps pulled from the future and from more than
    # 50000 back; past 50000 on the clock the "not running" / "not
    # poisoned" -1 is itself pulled to now each tick.
    for last in (NOW + 7, NOW - 50001, NOW - 50000):
        one(f'pulse.pull.run.{last - NOW}', 'Locke', ticks=3, states=['walk', 'run', 'runl'],
            root=dict(name='run', action=2), doing=dict(name='runl', action=2), stats=dict(fatigue=40),
            pulse=dict(lastrun=last))
        one(f'pulse.pull.poison.{last - NOW}', 'Araknid', ticks=3, stats=dict(health=20, poisoned=1),
            pulse=dict(lastpoison=last))
    for typ in ('Araknid', 'Locke'):
        for running in (True, False):
            big = dict(lasthealth=60000, lastfatigue=60000, lastmana=60000, lastrun=-1, lastpoison=-1)
            if typ == 'Locke':
                big.update(statetime=60000, stateframes=60000 * 24 // 100)
            one(f'pulse.bigclock.{typ}.run{int(running)}', typ, ticks=5, states=['walk', 'run', 'runl'],
                root=dict(name='run', action=2) if running else None,
                doing=dict(name='runl', action=2) if running else None,
                stats=dict(fatigue=40, poisoned=1, health=10), pulse=big, case_kw=dict(gametime=60000))

    # The alreadydead cheat keeps the player full and cured, and skips his
    # invoke timer; a character takes no notice.
    for typ in ('Locke', 'Araknid'):
        for poisoned in (0, 1):
            one(f'pulse.alreadydead.{typ}.p{poisoned}', typ, ticks=3,
                stats=dict(health=4, fatigue=2, mana=1, poisoned=poisoned),
                doing=dict(name='invoke', action=11), states=['walk', 'invoke'],
                pulse=dict(invokedelay=1, lastpoison=NOW - 100), case_kw=dict(alreadydead=1))

    # Death: Died told once, the paralysis gone; health back clears the
    # mark; the unkillable stay among the living.
    for typ in ('Araknid', 'Locke'):
        for health in (0, -3, 5):
            for flags in (0, 0x40000, 0x8, 0x40008):
                for objflags in (0, 0x800000):
                    one(f'pulse.died.{typ}.h{health}.c{flags:#x}.o{objflags:#x}', typ, stats=dict(health=health),
                        charflags=flags, objflags=objflags, states=['walk', 'dead'])

    # The corpse's death: the transition from its root and the dead loop,
    # prefixed or plain, or none; already dead; the fade after 240 ticks
    # and the removal.
    dead_sets = (('all', ['{root} to {p}dead', '{p}dead', 'dead']), ('trans.plain', ['{root} to {p}dead', 'dead']),
                 ('prefixed', ['{p}dead']), ('plain', ['dead']), ('none', []))
    corpse_roots = {'Araknid': [('walk', 1, 'c'), ('combat', 3, 'c')],
                    'Locke': [('walk', 1, 'w'), ('combat', 3, 'c'), ('hand', 3, 'h'), ('bow', 25, 'b')]}
    for typ, roots in corpse_roots.items():
        for rname, raction, p in roots:
            for label, names in dead_sets:
                states = ['walk', 'combat'] + [n.format(root=rname, p=p) for n in names]
                for doing in (None, dict(name='impact', action=12, flags=['priority']), dict(name='dead', action=19)):
                    dl = 'root' if doing is None else doing['name']
                    one(f'pulse.corpse.{typ}.{rname}.{label}.{dl}', typ, stats=dict(health=0), states=states,
                        root=dict(name=rname, action=raction), doing=doing)
    for ticks0, fade, fadestep, fadedir in ((0, 100, 0, 0), (238, 100, 0, 0), (239, 100, 0, 0), (239, 60, 3, -1),
                                            (300, 0, 0, 0), (300, 1, 1, -1), (300, 40, 0, 0)):
        one(f'pulse.corpse.fade.d{ticks0}.f{fade}.s{fadestep}.dir{fadedir}', 'Araknid', ticks=3,
            stats=dict(health=0), states=['walk', 'cdead'], doing=dict(name='cdead', action=19),
            root=dict(name='cdead', action=19),
            pulse=dict(deadticks=ticks0, fade=fade, fadestep=fadestep, fadelimit=0 if fadedir else 100,
                       fadedir=fadedir))
    for typ in ('Araknid', 'Locke'):
        for health in (0, 5):
            one(f'pulse.script.{typ}.h{health}', typ, ticks=2, stats=dict(health=health), script=1,
                states=['walk', 'cdead', 'wdead', 'dead'])
    one('pulse.corpse.feel', 'Araknid', ticks=360, stats=dict(health=0), states=['walk', 'combat to cdead', 'cdead'],
        root=dict(name='combat', action=3), objflags=0x800000)
    for real in (False, True):
        one(f'pulse.corpse.update.real{int(real)}', 'Araknid', ticks=12, stats=dict(health=0),
            states=['walk', 'combat to cdead', 'cdead'], root=dict(name='combat', action=3),
            case_kw=dict(seam_update=not real))

    # Blood from the scripted deaths by frame; a bad BLEEDER or none.
    sprays = (('decapdeath', range(3, 68)), ('combat to ripinhalfed', range(63, 76)),
              ('ripkilldeath', range(112, 128)), ('walk to dead', range(0, 12)))
    for anim, frames in sprays:
        for frame in frames:
            for facing in (0x40, 0x10):
                one(f'pulse.blood.{anim}.f{frame}.face{facing}', 'Araknid', frame=frame, facing=facing,
                    states=['walk', anim], doing=dict(name=anim, action=19), stats=dict(health=0),
                    root=dict(name=anim, action=19))
        for bleeder, cflags in ((0, 0), (1, 0x10), (2, 0)):
            one(f'pulse.blood.{anim}.b{bleeder}.fl{cflags:#x}', 'Araknid', frame=list(frames)[5],
                states=['walk', anim], doing=dict(name=anim, action=19), stats=dict(health=0),
                root=dict(name=anim, action=19), chardata_changes=dict(bleeder=bleeder, flags=cflags))
    for name in ('Rand', 'rand', 'Randy'):
        add(f'pulse.blood.walktodead.{name}', [pulse_char(data, name, 'Araknid', frame=5, states=['walk', 'walk to dead'],
                                                         doing=dict(name='walk to dead', action=19),
                                                         stats=dict(health=0))], self_=name)

    # The impale holds.
    for typ in ('Araknid', 'Locke'):
        for anim in ('atkimpale to walk', 'atkimpale', 'atkimpale2', 'walk'):
            one(f'pulse.impale.hold.{typ}.{anim}', typ, charflags=0x80, states=['walk', anim],
                doing=dict(name=anim, action=7))
    for kind in (4, 3):
        for with_player in (True, False):
            for anim in ('impimpale', 'impale'):
                chars = [pulse_char(data, 'Me', 'Yhagoro', monsterkind=kind, states=['walk', anim],
                                    doing=dict(name=anim, action=7))]
                if with_player:
                    chars.append(pulse_char(data, 'Locke', 'Locke', pos=[1100, 1000, 0], ident=0x20))
                add(f'pulse.impale.yhagoro.k{kind}.p{int(with_player)}.{anim}', chars)
    for label, root, doing, desired in (
            ('walking', dict(name='walk', action=1), None, dict(name='walk', action=1)),
            ('held', dict(name='combat', action=3, flags=['priority']), dict(name='impimpaled', action=12), None),
            ('held.priority', dict(name='combat', action=3), dict(name='impimpaled', action=12, flags=['priority']),
             dict(name='combat', action=3)),
            ('held.priority.same', dict(name='combat', action=3),
             dict(name='impimpaled', action=12, flags=['priority']), 'doing'),
            ('walk.desired', dict(name='combat', action=3), None, dict(name='walk', action=1)),
            ('desired.priority', dict(name='combat', action=3), None,
             dict(name='combat', action=3, flags=['priority'])),
            ('desired.priority.held', dict(name='combat', action=3),
             dict(name='impimpaled', action=12, flags=['priority']), dict(name='walk', action=1, flags=['priority'])),
            ('root.desired.priority', dict(name='combat', action=3, flags=['priority']), None, 'root'),
            ('walk.root.combat.desired', dict(name='walk', action=1), dict(name='impimpaled', action=12),
             dict(name='combat', action=3, flags=['priority']))):
        one(f'pulse.impale.release.{label}', 'Locke', charflags=0x20000, states=['walk', 'combat', 'impimpaled'],
            root=root, doing=doing, desired=desired)

    # The invoke, burn and flail timers.
    for delay in (0, 1, 2):
        for typ in ('Araknid', 'Locke'):
            one(f'pulse.timer.invoke.{typ}.d{delay}', typ, states=['walk', 'invoke'], doing=dict(name='invoke', action=11),
                pulse=dict(invokedelay=delay))
            one(f'pulse.timer.burn.{typ}.d{delay}', typ, states=['walk', 'onfire'], doing=dict(name='onfire', action=21),
                pulse=dict(burnticks=delay))
            one(f'pulse.timer.invoke.notdoing.{typ}.d{delay}', typ, states=['walk'], pulse=dict(invokedelay=delay))
    for done in (0, 1):
        for typ in ('Araknid', 'Locke'):
            one(f'pulse.timer.flail.{typ}.c{done}', typ, states=['walk', 'impact'], doing=dict(name='impact', action=22),
                commanddone=done)

    # The player falls into combat: the cadence by frame and id, the roots
    # that keep him out, the one found and every gate on it.
    def auto(name, *, frame=0x210, root=None, found='Target', tstats=None, dist=20, offset=0, autoc=1, memory=None,
             **kw):
        me = pulse_char(data, 'Me', 'Locke', root=root, states=['walk', 'combat', 'run', 'sneak', 'bow'],
                        attackstate=dict(autocombat=autoc))
        if memory is not None:
            me['hasseen'] = memory
        tg = pulse_char(data, 'Target', 'Araknid', pos=_toward(ME_AT, 64 + offset, dist + 32), facing=192,
                        ident=0x20, stats=tstats)
        add(name, [me, tg], frame=frame, found=found, **kw)

    for frame in (0x210, 0x230, 0x211, 0x200, 0x22f):
        auto(f'pulse.auto.frame{frame:#x}', frame=frame)
    for rname, action in (('walk', 1), ('run', 2), ('sneak', 1), ('combat', 3), ('bow', 25), ('combatrun', 3),
                          ('bowrun', 25)):
        auto(f'pulse.auto.root.{rname}.{action}', root=dict(name=rname, action=action))
    for autoc in (0, 1):
        auto(f'pulse.auto.none.a{autoc}', found=None, autoc=autoc)
        auto(f'pulse.auto.switch.a{autoc}', autoc=autoc)
    for opt in (0, 1):
        auto(f'pulse.auto.option{opt}', autobegincombat=opt)
    for dist in (0, 100, 127, 128, 200):
        auto(f'pulse.auto.dist{dist}', dist=dist)
    for offset in (0, 0x2f, 0x30, -0x2f, -0x30, 0x80):
        auto(f'pulse.auto.angle{offset}', offset=offset)
    for control, ps in ((1, 0), (0, 1), (0, 0)):
        auto(f'pulse.auto.control{control}.{ps}', control=control, ps_5d8=ps)
    auto('pulse.auto.target.dead', tstats=dict(health=0))
    for noauto in (0, 1):
        auto(f'pulse.auto.memory.n{noauto}', memory=[['Target', 0x100, noauto]])
    auto('pulse.auto.memory.other', memory=[['Me', 0x100, 1]])

    # A chain or autocombo continues on its timing (COMBAT_ATTACK_CHOICE.md §3.11).
    for typ in ('Locke', 'Araknid'):
        cd = s['chars'][typ]
        for i, ad in enumerate(cd['attacks']):
            if not ad['flags'] & (CA_CHAIN | CA_AUTOCOMBO):
                continue
            for frame_off in (ad['nextwait'] - 1, ad['nextwait'], ad['nextwait'] + ad.get('chainexptime', 0)):
                for hits in (0, 1):
                    chars, _ = duel(data, typ, 'Locke' if typ != 'Locke' else 'Araknid',
                                    me_state=dict(lastattack=i, lastattackticks=0x200 - frame_off, chainhits=hits))
                    chars[0]['pulsestate'] = dict(PULSE_STATE, lasthealth=NOW, lastfatigue=NOW, lastmana=NOW,
                                                  **(PLAYER_PULSE_STATE if typ in PLAYERS else {}))
                    if typ in PLAYERS:
                        chars[0].update(maxhealth=100, maxfatigue=78, maxmana=105)
                        chars[0]['stats'].update(healthpct=0, manapct=0, fatiguepct=0)
                    for c in chars:
                        c['stats'].setdefault('poisoned', 0)
                    add(f'pulse.chain.{typ}.a{i}.o{frame_off}.h{hits}', chars)

    # The AI runs with OF_AI; the exit mark wears off after 5 screen
    # frames; a forced command; the flash; the forced bit at frame 5.
    for noai in (0, 1):
        one(f'pulse.ai.noai{noai}', noai=noai)
    for since in (4, 5, 6, 30):
        one(f'pulse.exit.{since}', objflags=0x100000, pulse=dict(exittimestamp=500 - since))
    for wait in (0, 5):
        one(f'pulse.forcedone.w{wait}', pulse=dict(forcecommanddone=1), doing=dict(name='walk', action=1, wait=wait),
            states=['walk'])
    for flash in (0, 1, 4):
        one(f'pulse.flash{flash}', attackstate=dict(flashticks=flash))
    for frame in (4, 5, 9):
        one(f'pulse.forced.f{frame}', frame=frame, doing=dict(name='combat', action=3, flags=['forced', 'transition']),
            states=['walk', 'combat'])

    # The player's tick: the play clock by state, the stat effects, the
    # touched flag, the death screen countdown and its hold.
    for state in (0, 1, 3, 5):
        one(f'pulse.player.clock.s{state}', 'Locke', ticks=7, playerstate=state)
    for expires in (-1, NOW + 100, NOW + 1, NOW - 1, NOW + 50000, NOW + 50001, NOW - 50001):
        one(f'pulse.player.effects.e{expires}', 'Locke', pulse=dict(stateffectexpires=expires), playerstate=1)
    one('pulse.player.virgin', 'Locke', objflags=0x4000000)
    for health in (0, 1):
        for count in (192, 1, 0, -5):
            for hold in (0, 0x1000):
                one(f'pulse.player.death.h{health}.c{count}.hold{hold:#x}', 'Locke', stats=dict(health=health),
                    charflags=hold, pulse=dict(deathcountdown=count), states=['walk', 'dead'])
    one('pulse.player.death.editor', 'Locke', stats=dict(health=0), pulse=dict(deathcountdown=0),
        states=['walk', 'dead'], case_kw=dict(editor=1))
    one('pulse.player.death.feel', 'Locke', ticks=200, stats=dict(health=0), states=['walk', 'wdead', 'walk to wdead'])
    return finish(cases)


# ---- C7: experience and the level-up (PLAYER_STATS.md §7) ---------------------------------

SKILLS = ('attack', 'defense', 'invoke', 'hands', 'knife', 'sword', 'bludgeons', 'axes', 'bows', 'stealth',
          'lockpick')
ATTRIBUTES = ('strn', 'cons', 'agil', 'rflx', 'mind', 'luck')


def exp_player(data, name='Me', *, level=3, exp=None, ident=0x10, stats=None, attributes=None, levelup=(0, 1, 0),
               states=('walk', 'invoke', 'winv2'), pos=None):
    """A player for experience: his level and experience (as stats and,
    since KillExp reads them by name, as type stats), his skills' levels
    and experience, his attributes (the stats themselves too: the level-up
    raises those), his level-up picks."""
    s = shipped(data, None)
    table, skilltable = s['rules']['exp'], s['rules']['skillexp']
    exp = table[min(level, 29)] - 10 if exp is None else exp
    attrs = dict(zip(ATTRIBUTES, attributes or (14, 12, 15, 13, 14, 16)))
    st = dict(level=level, exp=exp, nextexp=table[min(level, 29)], **attrs)
    for i, sk in enumerate(SKILLS):
        lv = PLAYER_STATS[sk]
        st[f'{sk}exp'] = skilltable[max(lv - 1, 0)] + 7 * i
        st[f'{sk}next'] = skilltable[min(lv, 29)]
    st.update(stats or {})
    c = pulse_char(data, name, 'Locke', pos=pos, ident=ident, stats=st, states=list(states))
    c['classstats'] = dict(radius=16, value=1, level=st['level'], exp=st['exp'])
    c['basestats'] = {a: st[a] for a in ATTRIBUTES}
    c['levelup'] = list(levelup)
    return c


def exp_victim(data, *, typ='Araknid', health=0, value=5, level=None, memory=None, name='Victim'):
    v = pulse_char(data, name, typ, pos=[1100, 1000, 0], ident=0x20, stats=dict(health=health))
    v['classstats'] = dict(radius=16, value=value)
    if level is not None:
        v['classstats']['level'] = level
        v['stats']['level'] = level
    if memory is not None:
        v['hasseen'] = memory
    return v


# ---- The tag sounds' samples ---------------------------------------------------

TAG_NAMES = ['impact1', 'impact2', 'Impact3', 'IMPACT1', 'impact', 'imp', 'imp7', 'impx9', 'imp9x', 'Impact1h',
             'block1', 'Block', 'blockimpact2', 'swing2', 'roar', 'whoosh1']


def tagsound_cases(data: Path, workdir: Path) -> list[dict]:
    """The sample a tag sound plays (0x40af50): an IMP name ending in other
    than a letter takes the weapon kind of the one fought (the combat or
    bow root's object), "bigsword" or one time in six "sword" for a sword;
    BLOCK names play nothing; the rest as they are. STEP names are left
    out (their terrain is the port's divergence: no terrain bits)."""
    shipped(data, workdir)
    cases = []
    tape = [0, 5, 12, 3, 6, 1, 2, 0, 4, 7, 9, 11, 18, 23, 30, 8]

    def add(name, chars, rules):
        cases.append(case(name, 'tag-sound', chars, rules, args=dict(names=TAG_NAMES), tape=tape,
                          seed=len(cases) * 13 + 5))

    for wt in range(9):
        # Locke fights an Araknid of that weapon kind; an Araknid fights Locke
        # holding one.
        chars, rules = duel(data, 'Locke', 'Araknid')
        chars[1]['chardata'] = dict(chars[1]['chardata'], weapontype=wt)
        add(f'tag.locke.foe{wt}', chars, rules)
        chars, rules = duel(data, 'Araknid', 'Locke')
        chars[1]['weapon'] = dict(type=wt, damage=13)
        add(f'tag.araknid.foe{wt}', chars, rules)
    # Not fighting: a walk root; a combat root with no one; a bow root on him.
    for label, root in (('walk', dict(name='walk', action=1)), ('combat.none', dict(name='combat', action=3)),
                        ('bow', dict(name='bow', action=0x19, obj='Target'))):
        chars, rules = duel(data, 'Locke', 'Araknid', me_root=root)
        chars[1]['chardata'] = dict(chars[1]['chardata'], weapontype=2)
        add(f'tag.locke.{label}', chars, rules)
    return finish(cases)


def exp_cases(data: Path, workdir: Path) -> list[dict]:
    s = shipped(data, workdir)
    rules = s['rules']
    table, skilltable = rules['exp'], rules['skillexp']
    cases = []

    def add(name, call, chars, args, **kw):
        cases.append(case(name, call, chars, rules, args=args, seed=len(cases) * 31 + 7, tape=[], frame=0x2000,
                          seam_exp=False, **kw))

    # KillExp: the gain by the difference of levels.
    for level in (1, 2, 5, 15, 29, 30):
        for value in range(level - 6, level + 7):
            add(f'exp.killexp.l{level}.v{value}', 'kill-exp', [exp_player(data, level=level)], dict(value=value))

    # AwardKillExp: a dead victim only; a creature by its Value, a player by
    # his Level and half again; the level reached or not, capped at 30; the
    # messages by the new level; the picks raised, one past 15; a pick at
    # 29 moves on; the level-up's animation, sound and effect; full again.
    for health in (1, 0, -4):
        add(f'exp.kill.health{health}', 'exp-kill', [exp_player(data), exp_victim(data, health=health)],
            dict(victim='Victim'))
    for level in (1, 2, 5, 14, 15, 16, 23, 24, 26, 28, 29, 30):
        need = table[min(level, 29)]
        for value in (level, level + 3, level - 5):
            for exp in (need - 1, need - 2000, need):
                add(f'exp.kill.l{level}.v{value}.e{exp - need}', 'exp-kill',
                    [exp_player(data, level=level, exp=exp, levelup=(level % 6, (level + 2) % 6, 0)),
                     exp_victim(data, value=value)], dict(victim='Victim'))
    for attrs in ((29, 12, 15, 13, 14, 16), (14, 30, 29, 13, 14, 16), (29, 29, 29, 29, 29, 28),
                  (29, 29, 29, 29, 29, 29), (12, 12, 12, 12, 12, 40)):
        for picks in ((0, 1, 0), (5, 0, 0), (2, 2, 0)):
            add(f'exp.kill.picks.{"_".join(map(str, attrs))}.p{picks[0]}{picks[1]}', 'exp-kill',
                [exp_player(data, level=4, exp=table[4], attributes=attrs, levelup=picks),
                 exp_victim(data, value=4)], dict(victim='Victim'))
    for level in (3, 16):
        for vlevel in (1, level, level + 2):
            add(f'exp.kill.player.l{level}.v{vlevel}', 'exp-kill',
                [exp_player(data, level=level, exp=table[level] - 5), exp_victim(data, typ='Locke', level=vlevel)],
                dict(victim='Victim'))
    for states in (('walk',), ('walk', 'invoke'), ('walk', 'winv2')):
        add(f'exp.kill.anim.{"_".join(states)}', 'exp-kill',
            [exp_player(data, level=5, exp=table[5], states=states), exp_victim(data, value=9)],
            dict(victim='Victim'))

    # AwardSkillExp: a dead victim's Value into the skill (AddSkillExp: the
    # skill up one at its next figure, its next figure on; pinned at 30).
    for skill in range(11):
        for health in (0, 3):
            add(f'exp.skill.s{skill}.h{health}', 'exp-skill', [exp_player(data), exp_victim(data, health=health)],
                dict(skill=skill, victim='Victim'))
    for level, value in ((3, 9), (3, 3), (3, 1), (10, 1), (10, 7), (10, 6)):
        add(f'exp.skill.l{level}.v{value}', 'exp-skill', [exp_player(data, level=level), exp_victim(data, value=value)],
            dict(skill=5, victim='Victim'))
    for lv in (0, 1, 5, 28, 29, 30, 31):
        for gap in (-1, 0, 1, 500):
            nxt = skilltable[min(lv, 29)]
            add(f'exp.addskill.l{lv}.g{gap}', 'add-skill-exp',
                [exp_player(data, stats=dict(sword=lv, swordexp=nxt - 100, swordnext=nxt))],
                dict(skill=SKILLS.index('sword'), exp=100 + gap))
    for exp in (0, -50, 100000):
        add(f'exp.addskill.e{exp}', 'add-skill-exp', [exp_player(data)], dict(skill=9, exp=exp))

    # AwardStealthExp: only for a victim whose memory of him is gone
    # (45 seconds, 1080 frames) or never was.
    for ago in (None, 0, 1079, 1080, 1081, 5000):
        for health in (0, 2):
            memory = [] if ago is None else [['Me', 0x2000 - ago, 0]]
            add(f'exp.stealth.ago{ago}.h{health}', 'exp-stealth',
                [exp_player(data), exp_victim(data, health=health, memory=memory)], dict(victim='Victim'))
    add('exp.stealth.other', 'exp-stealth',
        [exp_player(data), exp_victim(data, memory=[['Victim', 0x2000, 0]])], dict(victim='Victim'))
    return finish(cases)


TARGETS = {
    'melee-attack-choice': dict(fixture='slots/combat/melee_attack.py', cases=attack_choice_cases, compare=compare,
                                port_fields=port_fields, unit=lambda r: len(r.get('calls', [])) or 1),
    'melee-hit': dict(fixture='slots/combat/melee_attack.py', cases=hit_cases, compare=compare,
                      port_fields=port_fields, unit=lambda r: 1),
    'melee-damage': dict(fixture='slots/combat/melee_attack.py', cases=damage_cases, compare=compare,
                         port_fields=port_fields, unit=lambda r: 1),
    'melee-resolvers': dict(fixture='slots/combat/melee_attack.py', cases=resolver_cases, compare=compare,
                            port_fields=port_fields, unit=lambda r: 1),
    'melee-ai': dict(fixture='slots/combat/melee_attack.py', cases=ai_cases, compare=compare,
                     port_fields=port_fields, unit=lambda r: 1),
    'melee-pulse': dict(fixture='slots/combat/melee_attack.py', cases=pulse_cases, compare=compare,
                        port_fields=port_fields, unit=lambda r: 1),
    'melee-exp': dict(fixture='slots/combat/melee_attack.py', cases=exp_cases, compare=compare,
                      port_fields=port_fields, unit=lambda r: 1),
    'melee-tagsound': dict(fixture='slots/combat/melee_attack.py', cases=tagsound_cases, compare=compare,
                           port_fields=port_fields, unit=lambda r: len(r['returned'])),
}
