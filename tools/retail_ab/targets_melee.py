"""Combat dojo melee targets for retail_ab.py (docs/gameplay/COMBAT_DOJO.md
C2-C5; forensics COMBAT_ATTACK_CHOICE.md, COMBAT_HIT.md).

`melee-attack-choice` (kata C3): the attack search and what it fixes --
IsValidAttack over every attack of real char.def tables in worlds that
take each of its gates both ways, the searches (button, percentage,
interactive), DoAttack, and the callers (ButtonAttack with its chain,
same-button and counter rules, ButtonAction, RandomAttack,
SpecificAttack), every random branch steered with RNG tapes.

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
                                     'tohitdamage', 'statlevels')},
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
    c = dict(name=name, call=call, globals=dict(combatface=1, frame=frame, nahkranoth=extra.pop('nahkranoth', 0)),
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
            chars, rules = world(**v)
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


TARGETS = {
    'melee-attack-choice': dict(fixture='slots/combat/melee_attack.py', cases=attack_choice_cases, compare=compare,
                                port_fields=port_fields, unit=lambda r: len(r.get('calls', [])) or 1),
    'melee-hit': dict(fixture='slots/combat/melee_attack.py', cases=hit_cases, compare=compare,
                      port_fields=port_fields, unit=lambda r: 1),
}
