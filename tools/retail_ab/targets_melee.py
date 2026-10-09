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
              classstats=None, attackstate=None, weapon=None, resists=None, charflags=0, frame=0, ident=0):
    spec = {'name': name, 'type': typ or cd['name'], 'class': objclass, 'pos': list(pos), 'facing': facing,
            'moveangle': facing, 'charflags': charflags, 'frame': frame, 'id': ident,
            'stats': stats or {}, 'classstats': classstats or {'radius': 16, 'value': 1},
            'chardata': cd, 'states': list(states), 'root': root or dict(name='combat', action=3)}
    if doing is not None:
        spec['doing'] = doing
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
         me_flags=0, target_flags=0, me_frame=0, weapon=None, target_root=None, me_value=1, target_value=1):
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
                        charflags=me_flags, frame=me_frame, ident=0x10)
    tg_spec = make_char(tname, tcls, tpos, (bearing + 128) & 0xff, tcd, states=target_states or ['combat'],
                        root=target_root or dict(name='combat', action=3, angle=(bearing + 128) & 0xff, obj=mname),
                        doing=target_doing, stats=dict(PLAYER_STATS if tcls == 11 else MONSTER_STATS, **(target_stats or {})),
                        classstats={'radius': 16, 'value': target_value}, attackstate=target_state,
                        charflags=target_flags, ident=0x20)
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
                                 ('sneak', 1), ('walk', 1), ('run', 1), ('combat', 3)):
                states = ['combat', root] + [p + a0['impacts'][0]['name'] for p in
                                             ('h', 'hr', 'c', 'cr', 'b', 'br', 's', 'w', 'r', 't', 'tr')]
                run(f'prefix.{root}', [a0], target_states=states, target_root=block(root, action, obj='Me'))
                run(f'prefix.{root}.bare', [a0], target_states=['combat'], target_root=block(root, action, obj='Me'))
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
    return finish(cases)
TARGETS = {
    'melee-attack-choice': dict(fixture='slots/combat/melee_attack.py', cases=attack_choice_cases, compare=compare,
                                port_fields=port_fields, unit=lambda r: len(r.get('calls', [])) or 1),
}
