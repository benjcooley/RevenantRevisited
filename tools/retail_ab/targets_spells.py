"""Combat dojo spell and missile targets for retail_ab.py (docs/gameplay/
COMBAT_DOJO.md §2, katas S1-S4 and D1s; docs/gameplay/forensics/
SPELLS_MISSILES.md).

- `spell-data` (D1s): spell.def as each side parses it. Retail runs
  TSpellList::Load in the emulator (slots/combat/spell_data.py); the port
  runs TSpellList::Load (`Revenant --retail-ab=spell-data`,
  src/retailab_spells.cpp). Both dump every spell, variant and CONTROLDATA
  block by field name; the compare walks them by spell and variant name.

Cases are written under the work directory so both sides read the same
bytes.
"""
from __future__ import annotations

import hashlib
import itertools
import json
import re
import zipfile
from pathlib import Path

from combat_targets import compare as generic_compare, finish, port_fields  # noqa: F401

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[1]
IGNORED = {'schema', 'side', 'elapsed_ms'}


def _member(z: zipfile.ZipFile, name: str) -> bytes:
    for n in z.namelist():
        if n.lower() == name.lower():
            return z.read(n)
    raise KeyError(f'{name} not in {z.filename}')


def shipped_spell_def(data: Path) -> bytes:
    """The spell.def retail reads: resources.rvr's (packs first)."""
    with zipfile.ZipFile(data / 'resources.rvr') as z:
        return _member(z, 'spell.def')


# ---- D1s: the spell.def parse ------------------------------------------------------

def spell_data_cases(data: Path, workdir: Path) -> list[dict]:
    cases = []

    def add(name, blob, origin):
        root = workdir / 'cases' / name
        path = root / 'Resources' / 'spell.def'
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(blob)
        cases.append(dict(name=name, dir=str(root), file=str(path), origin=origin,
                          sha256=hashlib.sha256(blob).hexdigest()))

    add('shipped', shipped_spell_def(data), 'resources.rvr:spell.def')
    loose = REPO / 'data' / 'Resources' / 'spell.def'
    if loose.exists():
        add('gog-loose', loose.read_bytes(), 'data/Resources/spell.def (GOG loose; one LIGHT differs)')
    for folder in sorted((HERE / 'cases' / 'spell_data').glob('*')):
        f = folder / 'Resources' / 'spell.def'
        if f.exists():
            add(f'edge.{folder.name}', f.read_bytes(), str(f.relative_to(REPO)))
    return cases


def spell_data_port_fields(case: dict) -> list[str]:
    return [json.dumps(dict(dir=case['dir']), separators=(',', ':'), ensure_ascii=True)]


def _fields(where, retail, port, out):
    for key in list(retail) + [k for k in port if k not in retail]:
        if key in IGNORED or key in ('variants', 'controldata', 'light'):
            continue
        r, p = retail.get(key, '<absent>'), port.get(key, '<absent>')
        if r != p:
            out.append(dict(where=where, line=None, field=f'{where} {key}', retail=r, port=p))


def compare_spell_data(case: dict, retail: dict, port: dict) -> list[dict]:
    out = []
    if retail.get('fatal') or port.get('fatal'):
        if retail.get('fatal') != port.get('fatal'):
            out.append(dict(where='load', line=None, field='fatal', retail=retail.get('fatal'),
                            port=port.get('fatal')))
        return out
    rs, ps = retail['spells'], port['spells']
    if [s['name'] for s in rs] != [s['name'] for s in ps]:
        out.append(dict(where='spells', line=None, field='spell order', retail=[s['name'] for s in rs],
                        port=[s['name'] for s in ps]))
    for r, p in zip(rs, ps):
        where = f"spell {r['name']}"
        _fields(where, r, p, out)
        _fields(f'{where} light', r['light'], p.get('light', {}), out)
        rv, pv = r['variants'], p.get('variants', [])
        if len(rv) != len(pv):
            out.append(dict(where=where, line=None, field=f'{where} variant count', retail=len(rv), port=len(pv)))
        for a, b in zip(rv, pv):
            vw = f"{where} variant {a['name']}"
            _fields(vw, a, b, out)
            ca, cb = a.get('controldata'), b.get('controldata')
            if (ca is None) != (cb is None):
                out.append(dict(where=vw, line=None, field=f'{vw} controldata', retail=ca, port=cb))
            elif ca is not None:
                _fields(f'{vw} controldata', ca, cb, out)
    return out


def _leaves(value) -> int:
    if isinstance(value, dict):
        return sum(_leaves(v) for k, v in value.items() if k not in IGNORED)
    if isinstance(value, list):
        return sum(_leaves(v) for v in value)
    return 1


# ---- the shipped spells, as the case generators see them ---------------------------

def shipped_spells(data: Path) -> list[dict]:
    """[{"name", "invoke", "variants": [{"name", "talismans", "mana",
    "skill"}]}] in file order, read off resources.rvr's spell.def -- what
    the case generators need to aim at each variant (the parse itself is
    kata D1s's)."""
    spells, current = [], None
    for raw in shipped_spell_def(data).decode('cp1252').splitlines():
        line = raw.split('//', 1)[0].strip()
        m = re.match(r'SPELL\s+"([^"]*)"', line)
        if m:
            current = dict(name=m.group(1), invoke='', variants=[])
            spells.append(current)
            continue
        m = re.match(r'ANIMATION\s+"([^"]*)"', line, re.I)
        if m and current is not None:
            current['invoke'] = m.group(1)
            continue
        m = re.match(r'VARIANT\s+"([^"]*)"\s*,\s*[^,]+,\s*"([^"]*)"\s*,\s*"[^"]*"\s*,(.*)', line, re.I)
        if m and current is not None:
            numbers = [v.strip() for v in m.group(3).split(',')]
            current['variants'].append(dict(name=m.group(1), talismans=m.group(2)[:5], mana=int(numbers[0]),
                                            skill=int(numbers[4])))
    return spells


# Talisman codes: A-L name the twelve TALISMAN types (class.def Code 65-76);
# M-P and R appear in the shipped strings with no type.
CODES = 'ABCDEFGHIJKLMNOPR'
TALISMAN_TYPES = ['Sun', 'Life', 'Ocean', 'Law', 'Soul', 'Stars', 'Death', 'Chaos', 'Sky', 'Earth', 'Ward', 'Moon']


def talisman_queries(tal: str) -> list[str]:
    """A talisman string and its near misses: case, order, one code dropped,
    added, changed or doubled, blanks around it."""
    out = [tal, tal.lower(), tal.upper(), tal.swapcase(), tal[::-1], tal + ' ', ' ' + tal, tal * 2]
    if len(tal) <= 4:
        out += [''.join(p) for p in itertools.permutations(tal)]
    for i in range(len(tal)):
        out.append(tal[:i] + tal[i + 1:])
        out.append(tal[:i] + tal[i] + tal[i:])
        out += [tal[:i] + c + tal[i + 1:] for c in CODES if c != tal[i]]
    for i in range(len(tal) + 1):
        out += [tal[:i] + c + tal[i:] for c in CODES]
    seen, unique = set(), []
    for q in out:
        if q not in seen:
            seen.add(q)
            unique.append(q)
    return unique


def name_queries(name: str) -> list[str]:
    return list(dict.fromkeys([name, name.lower(), name.upper(), name + ' ', ' ' + name, name[:-1], name + 'x',
                               name.replace(' ', '')]))


# ---- S1: talismans to spell ----------------------------------------------------------

def lookup_cases(data: Path, workdir: Path) -> list[dict]:
    """Every shipped variant's talisman string and its near misses, every
    variant and spell name and its near misses, through both lookups of each
    kind; one case per spell."""
    cases = []
    for spell in shipped_spells(data):
        queries = []
        for v in spell['variants']:
            queries += [dict(by='talismans', q=q) for q in talisman_queries(v['talismans'])]
            queries += [dict(by='name', q=q) for q in name_queries(v['name'])]
        queries += [dict(by='name', q=q) for q in name_queries(spell['name'])]
        queries += [dict(by='talismans', q=spell['name'])]
        cases.append(dict(name=f"lookup.{spell['name']}", call='lookup', queries=queries))
    cases.append(dict(name='lookup.edges', call='lookup', queries=[
        dict(by='talismans', q=''), dict(by='name', q=''), dict(by='talismans', q='ABCDEF'),
        dict(by='talismans', q='ABCDE'), dict(by='talismans', q='MDR'), dict(by='talismans', q='KBEF'),
        dict(by='name', q='Fizzle'), dict(by='name', q='fizzle'), dict(by='name', q='Priest Aura'),
        dict(by='name', q='Red Dragon Attack'), dict(by='name', q='SummonDark Ogrok'),
        dict(by='name', q='SummonDark Ogrok '), dict(by='talismans', q='PEP')]))
    return finish(cases)


def _player(inventory=None, quickspells=None, health=100, charflags=0, **extra):
    spec = dict(name='Locke', type='Locke', **{'class': 11}, pos=[1000, 1000, 0], facing=0, moveangle=0,
                stats=dict(health=health), classstats=dict(radius=16), chardata=dict(combatrangemax=300),
                states=['walk'], root=dict(name='walk', action=1), charflags=charflags)
    if inventory is not None:
        spec['inventory'] = inventory
    if quickspells is not None:
        spec['quickspells'] = quickspells
    spec.update(extra)
    return spec


def _talisman(name):
    return dict(name=name, **{'class': 3})


def _pouch(talismans, name='Spell Pouch', extra=()):
    return dict(name=name, type='Spell Pouch' if name.lower() != 'pouch' else 'Pouch', **{'class': 17},
                items=[_talisman(t) for t in talismans] + list(extra))


def _needed(tal: str) -> list[str]:
    """The talismans a string takes (types by code; untyped codes take none)."""
    return [TALISMAN_TYPES[ord(c) - ord('A')] for c in tal if 'A' <= c <= 'L']


def talisman_cases(data: Path, workdir: Path) -> list[dict]:
    """HasTalismans over pouches built to just cover, just miss and overshoot
    the shipped strings, every type, no pouch, a pouch inside another, talismans
    outside the pouch and in a bag inside it, names in other cases. The
    queries: every shipped talisman string with near misses."""
    spells = shipped_spells(data)
    strings = list(dict.fromkeys(v['talismans'] for s in spells for v in s['variants']))
    near = []
    for t in strings:
        near += [t.lower(), t[::-1], t + t[-1], t[0] + t] + [t[:i] + t[i + 1:] for i in range(len(t))]
    queries = list(dict.fromkeys(strings + near + ['', 'ABCDEFGHIJKL', 'AABBCCDDEEFF', 'LLLL', 'MNOPR', 'a']))
    cases = []

    def case(name, inventory):
        cases.append(dict(name=f'has.{name}', call='has-talismans', self='Locke', globals=dict(frame=0),
                          chars=[_player(inventory=inventory)], queries=queries))

    case('no-inventory', [])
    case('no-pouch', [_talisman(t) for t in TALISMAN_TYPES])
    case('empty-pouch', [_pouch([])])
    for n in (1, 2, 3):
        case(f'every-type-x{n}', [_pouch(TALISMAN_TYPES * n)])
    for i, t in enumerate(TALISMAN_TYPES):
        case(f'all-but-{t}', [_pouch([x for x in TALISMAN_TYPES * 2 if x != t])])
        case(f'only-{t}-x2', [_pouch([t, t])])
    for tal in strings:
        need = _needed(tal)
        if not need:
            continue
        case(f'exact.{tal}', [dict(name='Training Sword', **{'class': 1}), _pouch(need)])
        for k in sorted(set(need)):
            short = list(need)
            short.remove(k)
            case(f'short.{tal}.{k}', [_pouch(short)])
    case('pouch-in-pouch', [dict(name='Pouch', type='Pouch', **{'class': 17}, items=[_pouch(TALISMAN_TYPES)])])
    case('bag-in-spell-pouch', [_pouch(['Sun'], extra=[dict(name='Pouch', type='Pouch', **{'class': 17},
                                                            items=[_talisman(t) for t in TALISMAN_TYPES])])])
    case('spellpouch-name', [_pouch(TALISMAN_TYPES, name='spellpouch')])
    case('lowercase-names', [_pouch([t.lower() for t in TALISMAN_TYPES] + [t.upper() for t in TALISMAN_TYPES])])
    case('second-pouch', [_pouch(['Sun']), _pouch(TALISMAN_TYPES)])
    return finish(cases)


def quick_cases(data: Path, workdir: Path) -> list[dict]:
    """InvokeQuickSpell: each button and the edges, the slot empty / its
    talismans held / missing, the cast's answer, dead, the interactive gates
    (charflags 0x80000 lifts them), the main player or not."""
    pouch = [_pouch(['Moon', 'Sky', 'Sun'])]
    slots = ['', 'LI', 'BE', 'MA', 'LA']        # empty, held, missing (Life, Soul), untyped codes, held
    cases = []
    for button, cast, main in itertools.product((-1, 0, 1, 2, 3, 4, 5), (1, 0), (True, False)):
        cases.append(dict(name=f'quick.b{button}.cast{cast}.main{int(main)}', call='quick-spell', self='Locke',
                          button=button, casts=[cast], mainplayer=main, globals=dict(frame=0),
                          chars=[_player(inventory=pouch, quickspells=slots)]))
    gates = [('dead', dict(health=0)), ('health1', dict(health=1)),
             ('attack-interactive', dict(doing_attack=dict(flags=0x2000000))),
             ('attack-other', dict(doing_attack=dict(flags=0x1ffffff))),
             ('impact-interactive', dict(doing_impact=dict(flags=0x80))),
             ('impact-other', dict(doing_impact=dict(flags=0x7f))),
             ('interactive-move', dict(charflags=0x80000, doing_attack=dict(flags=0x2000000),
                                       doing_impact=dict(flags=0x80)))]
    for (label, extra), button in itertools.product(gates, (0, 1, 3)):
        cases.append(dict(name=f'quick.{label}.b{button}', call='quick-spell', self='Locke', button=button,
                          casts=[0], globals=dict(frame=0),
                          chars=[_player(inventory=pouch, quickspells=slots, **extra)]))
    return finish(cases)


# ---- S2: the cast ----------------------------------------------------------------------

# The invoke animations a caster can be asked for: every prefix AnimPrefix
# gives (0x4cdf60) with inv1..inv5, the fallback, and the spells' own names.
PREFIXES = ['c', 'cr', 'h', 'hr', 'b', 'br', 's', 'r', 'tr', 'w', 't']
INVOKE_STATES = [p + 'inv' + n for p in PREFIXES for n in '12345'] + ['invoke', 'Vomit']
# Roots (name, action) for the prefixes: combat 3, bow 0x19, the rest animate.
ROOTS = [('combat', 3), ('combatrun', 3), ('hand', 3), ('handrun', 3), ('bow', 25), ('bowrun', 25),
         ('sneak', 1), ('walk', 1), ('run', 1)]


def _caster(name='Locke', player=True, root=('combat', 3), root_obj=None, states=None, mana=5000, health=100,
            invoke=40, manacostpct=0, maxmana=5000, **extra):
    """A caster with plenty by default: mana, Invoke, every invoke state."""
    stats = dict(health=health, mana=mana)
    if player:
        stats.update(manacostpct=manacostpct, invoke=invoke, invokeexp=7, spelldamageinc=0)
    spec = dict(name=name, type='Locke' if player else 'Araknid', **{'class': 11 if player else 12},
                pos=[1000, 1000, 0], facing=0, moveangle=0, stats=stats, classstats=dict(radius=16),
                chardata=dict(combatrangemax=300, mana=maxmana),
                states=[root[0]] + (INVOKE_STATES if states is None else states),
                root=dict(name=root[0], action=root[1], obj=root_obj))
    if player:
        spec['maxmana'] = maxmana
    spec.update(extra)
    return spec


def _target(name='Araknid', kind='Araknid', x=1100, **extra):
    spec = dict(name=name, type=kind, **{'class': 12}, pos=[x, 1000, 0], facing=192, moveangle=192,
                stats=dict(health=30, mana=0, poisoned=0), classstats=dict(radius=16),
                chardata=dict(combatrangemax=300, mana=0), states=['combat'], root=dict(name='combat', action=3))
    spec.update(extra)
    return spec


def _cast_case(name, call, text, chars, self='Locke', targets=('Araknid',), **extra):
    case = dict(name=name, call=call, self=self, text=text, targets=list(targets) if targets is not None else None,
                sourcepos=None, globals=dict(frame=0), chars=chars)
    case.update(extra)
    return case


def _chance(d: int) -> int:
    return 100 if d >= 0 else {-1: 80, -2: 40, -3: 20}.get(d, 0)


def cast_cases(data: Path, workdir: Path) -> list[dict]:
    """The cast, entry by entry: every shipped variant by talismans and by
    name (a player, a monster; the managers' by-name cast for an arrow's
    shooter), then the gates (dead, interactive, immobile, paralysed, iced,
    state bit 2), the cooldown with and without the editor, the mana at the
    edge of each cost and ManaCostPct, the skill roll at every chance and
    its edges for the main player and not, the cheat, the invoke animation
    by root and what states exist, a doing block with priority, the
    targets and source, a buff with the buff flag on."""
    spells = shipped_spells(data)
    variants = [(s, v) for s in spells for v in s['variants']]
    cases = []
    add = cases.append

    # Every variant, by talismans and by name; a player and a monster.
    for i, (s, v) in enumerate(variants):
        tag = f"{i:02d}." + re.sub(r'\W+', '_', v['name']).strip('_')
        tape = [50, 10, 95]
        add(_cast_case(f'variant.tal.player.{tag}', 'cast-talismans', v['talismans'],
                       [_caster(), _target()], tape=tape))
        add(_cast_case(f'variant.tal.monster.{tag}', 'cast-talismans', v['talismans'],
                       [_caster(player=False), _target()], tape=tape))
        add(_cast_case(f'variant.name.player.{tag}', 'cast-name', v['name'], [_caster(), _target()], tape=tape))
        add(_cast_case(f'variant.manager-name.{tag}', 'manager-cast-name', v['name'],
                       [_caster(), _target()], self='Araknid', invoker='Locke', tape=tape))
    for i, s in enumerate(spells):
        tag = f"{i:02d}." + re.sub(r'\W+', '_', s['name']).strip('_')
        add(_cast_case(f'spellname.player.{tag}', 'cast-name', s['name'], [_caster(), _target()], tape=[50, 10]))

    # The gates, by each entry; a player and a monster.
    gates = [('dead', dict(health=0)), ('alive1', dict(health=1)),
             ('attack-interactive', dict(doing_attack=dict(flags=0x2000000))),
             ('impact-interactive', dict(doing_impact=dict(flags=0x80))),
             ('interactive-move', dict(charflags=0x80000, doing_attack=dict(flags=0x2000000),
                                       doing_impact=dict(flags=0x80))),
             ('immobile', dict(objflags=0x1)), ('paralysed', dict(objflags=0x800000)), ('iced', dict(objflags=0x2000000)),
             ('state2', dict(playerstate=2)), ('state3', dict(playerstate=3))]
    for (label, mine), player, call in itertools.product(gates, (True, False), ('cast-talismans', 'cast-name', 'cast')):
        if label.startswith('state') and not player:
            continue
        text = 'LI' if call != 'cast-name' else 'Fire Flash'
        add(_cast_case(f'gate.{label}.{"player" if player else "monster"}.{call}', call, text,
                       [_caster(player=player, root_obj='Araknid', **mine), _target()], tape=[50]))

    # The cooldown (the manager's wait) and the editor.
    for wait, editor, call in itertools.product((0, 1, 50), (0, 1), ('cast-talismans', 'cast-name', 'manager-cast-name',
                                                                      'manager-cast-talismans')):
        text = 'Fire Flash' if 'name' in call else 'LI'
        manager = call.startswith('manager')
        chars = [_caster(spellwait=0 if manager else wait), _target(spellwait=wait if manager else 0)]
        add(_cast_case(f'wait.{wait}.editor{editor}.{call}', call, text, chars, self='Araknid' if manager else 'Locke',
                       invoker='Locke' if manager else None, globals=dict(frame=0, editor=editor), tape=[50]))

    # Mana at each cost's edge, with ManaCostPct, the cheat; a fizzle follows a
    # player's failure when the spell costs anything.
    for (vname, tal, cost), pct, delta, cheat, player in itertools.product(
            (('Fire Flash', 'LI', 31), ('IronSkin', 'KJL', 267), ('Fizzle', 'PEP', 0), ('Sid Dragon Attack', 'MDR', 0)),
            (0, 10, 50, 100, -20), (-1, 0, 1), (0, 1), (True, False)):
        if not player and pct:
            continue
        need = cost - int(pct * cost / 100) if player else cost
        mana = max(0, need + delta)
        tag = re.sub(r'\W+', '_', vname)
        for call, text in (('cast-talismans', tal), ('cast-name', vname), ('manager-cast-name', vname)):
            manager = call.startswith('manager')
            add(_cast_case(f'mana.{tag}.pct{pct}.d{delta}.cheat{cheat}.{"player" if player else "monster"}.{call}',
                           call, text, [_caster(player=player, mana=mana, manacostpct=pct), _target()],
                           self='Araknid' if manager else 'Locke', invoker='Locke' if manager else None,
                           globals=dict(frame=0, cheat=cheat), tape=[50, 50]))

    # The skill roll: Invoke against the skill at every distance, the roll at
    # the chance's edges; main player or not; the cheat.
    for (vname, tal, skill), d, cheat, main in itertools.product(
            (('Fire Flash', 'LI', 1), ('Swift Strike', 'CL', 5), ('Maelstrom', 'GH', 27)),
            (2, 0, -1, -2, -3, -4, -9), (0, 1), (True, False)):
        chance = _chance(d)
        for roll in sorted({1, 100, max(1, chance), min(100, chance + 1)}):
            tag = re.sub(r'\W+', '_', vname)
            for call, text in (('cast-talismans', tal), ('cast-name', vname), ('manager-cast-name', vname)):
                manager = call.startswith('manager')
                add(_cast_case(f'skill.{tag}.d{d}.r{roll}.cheat{cheat}.main{int(main)}.{call}', call, text,
                               [_caster(invoke=skill + d), _target()], mainplayer=main,
                               self='Araknid' if manager else 'Locke', invoker='Locke' if manager else None,
                               globals=dict(frame=0, cheat=cheat), tape=[roll - 1, 60]))
    # Invoke experience across the clamp: the skill well above and below Invoke.
    for invoke in (0, 1, 5, 10, 17, 30, 40):
        add(_cast_case(f'exp.maelstrom.invoke{invoke}', 'cast-talismans', 'GH', [_caster(invoke=invoke), _target()],
                       tape=[0]))

    # The invoke animation: by root, by what the caster has; a monster's "c".
    for (root, action), anim_states, player in itertools.product(
            ROOTS, ('all', 'invoke-only', 'none'), (True, False)):
        states = INVOKE_STATES if anim_states == 'all' else (['invoke'] if anim_states == 'invoke-only' else [])
        for tal in ('LI', 'JI', 'JFI', 'E', 'B', 'DM'):          # invoke1, 2, 3, 4, 5, Vomit
            add(_cast_case(f'anim.{root}.{anim_states}.{"player" if player else "monster"}.{tal}', 'cast-talismans', tal,
                           [_caster(player=player, root=(root, action), states=states), _target()], tape=[50]))
    for flags in (['priority'], ['priority', 'interrupt'], []):
        doing = dict(name='cinv3', action=11, flags=flags)
        add(_cast_case(f'anim.doing-{"-".join(flags) or "plain"}', 'cast-talismans', 'LI',
                       [_caster(doing=doing), _target()], tape=[50]))
    add(_cast_case('anim.walk-prefix-flag', 'cast-talismans', 'LI',
                   [_caster(root=('walk', 1), charflags=0x2000, states=['winv1', 'invoke']), _target()], tape=[50]))

    # Targets and the source.
    for targets, numtargs in ((None, None), ([], 0), (['Araknid'], 1), (['Araknid', 'Arakna', 'Araknid3'], 3),
                              (['Araknid', 'Arakna', 'Araknid3'], 2), (['Araknid'], 0)):
        for tal in ('L', 'LI'):
            if tal == 'L' and targets == []:
                continue        # retail rolls poison for the empty slot it copies and calls through null
            chars = [_caster(), _target(), _target('Arakna', 'Arakna', 1200), _target('Araknid3', 'Araknid', 1300)]
            extra = dict(numtargs=numtargs) if numtargs is not None else {}
            add(_cast_case(f'targets.{len(targets) if targets is not None else "none"}.n{numtargs}.{tal}',
                           'cast-talismans', tal, chars, targets=targets, sourcepos=[3, -4, 5],
                           tape=[50, 89, 90, 0], **extra))
    # Poison rolls at the chance's edge (Poison 90: below poisons).
    for rolls in ([89], [90], [0], [100]):
        add(_cast_case(f'poison.roll{rolls[0]}', 'cast-talismans', 'L', [_caster(), _target()], tape=[50] + rolls))
        add(_cast_case(f'poison.spider.roll{rolls[0]}', 'cast-talismans', 'MC', [_caster(player=False), _target()],
                       tape=[50] + rolls))

    # Cast: at the combat or bow root's opponent.
    for (root, action), obj in itertools.product((('combat', 3), ('bow', 25), ('walk', 1)), ('Araknid', None)):
        add(_cast_case(f'cast.{root}.{"target" if obj else "none"}', 'cast', 'LI',
                       [_caster(root=(root, action), root_obj=obj), _target()], targets=None, tape=[50]))

    # A buff while a buff is on (charflags 0x20): the walk round the caster.
    for tal, buffed in itertools.product(('E', 'LI', 'EF'), (0, 0x20)):
        add(_cast_case(f'buff.{tal}.flag{buffed:#x}', 'cast-talismans', tal, [_caster(charflags=buffed), _target()],
                       targets=None, tape=[50]))
    # The managers with no invoker (no gates), and a monster shooter.
    for tal in ('LI', 'L', 'PEP'):
        add(_cast_case(f'noinvoker.{tal}', 'manager-cast-talismans', tal, [_caster(), _target()], self='Araknid',
                       invoker=None, tape=[50, 10]))
    for vname in ('Poison', 'Fire Flash'):
        add(_cast_case(f'proc.monster-shooter.{vname}', 'manager-cast-name', vname,
                       [_caster(player=False), _target()], self='Araknid', invoker='Locke', tape=[50, 10]))
    return finish(cases)


# ---- S3: a spell's construction and its damage -----------------------------------------

def _damage_case(name, call, variant, chars, **extra):
    case = dict(name=name, call=call, variant=list(variant), invoker='Locke', targets=['Araknid'], sourcepos=None,
                globals=dict(frame=0), chars=chars)
    case.update(extra)
    return case


def _victim(name='Araknid', player=False, magicresist=None, resists=None, x=1100):
    if player:
        spec = _caster(name, root=('combat', 3), states=[])
        spec['stats'].update(poisoned=0)
        spec['resists'] = resists or [0] * 10
    else:
        spec = _target(name, x=x)
    if magicresist is not None:
        spec['magicresist'] = magicresist
    return spec


def spell_new_cases(data: Path, workdir: Path) -> list[dict]:
    """The TSpell constructor through each class's creator: every shipped
    variant with a player, a monster and no invoker; targets none, one,
    three, fewer than given; the source; the poison roll at its edges for
    one to three targets; a STATLINE on each kind of invoker."""
    cases = []
    for i, (s, v) in enumerate((s, v) for s in shipped_spells(data) for v in s['variants']):
        tag = f"{i:02d}." + re.sub(r'\W+', '_', v['name']).strip('_')
        for cls in ('Spell', 'Strike'):
            cases.append(_damage_case(f'new.{cls}.player.{tag}', 'spell-new', (s['name'], v['name']),
                                      [_caster(), _victim()], tape=[10, 95], **{'class': cls}))
        cases.append(_damage_case(f'new.monster.notargets.{tag}', 'spell-new', (s['name'], v['name']),
                                  [_caster(player=False), _victim()], targets=None, sourcepos=[7, -8, 9], tape=[10]))
        cases.append(_damage_case(f'new.noinvoker.{tag}', 'spell-new', (s['name'], v['name']),
                                  [_caster(), _victim()], invoker=None, tape=[10]))
    three = [_caster(), _victim(), _victim('Arakna', x=1200), _victim('Araknid3', x=1300)]
    for rolls in ([89, 90, 0], [90, 100, 89], [0, 0, 0], [100, 100, 100]):
        for vname in ('Poison', 'SpiderPoison'):
            for n in (1, 2, 3):
                cases.append(_damage_case(f'new.poison.{vname}.n{n}.r{"-".join(map(str, rolls))}', 'spell-new',
                                          ('Poison', vname), three, targets=['Araknid', 'Arakna', 'Araknid3'][:n],
                                          tape=rolls, **{'class': 'Strike'}))
    for targets, numtargs in ((['Araknid', 'Arakna', 'Araknid3'], 2), (['Araknid'], 0), (['Araknid'], -5)):
        cases.append(_damage_case(f'new.numtargs.{len(targets)}.n{numtargs}', 'spell-new', ('Poison', 'Poison'), three,
                                  targets=targets, numtargs=numtargs, tape=[0, 0, 0], **{'class': 'Strike'}))
    for invoker in ('Locke', 'Monster', None):
        chars = [_caster(), _caster('Monster', player=False), _victim()]
        cases.append(_damage_case(f'new.statline.{invoker}', 'spell-new', ('Might', 'Might'), chars, invoker=invoker,
                                  **{'class': 'Strike'}))
    return finish(cases)


def spell_damage_cases(data: Path, workdir: Path) -> list[dict]:
    """TSpell::Damage: every shipped variant from a player at a monster,
    from a monster at the player, with no caster; then a grid over the
    target's magic resistance (per mille, the float's cut), a player
    caster's SpellDamageInc, a player target's DmgResMagical and the roll's
    edges; no target."""
    cases = []
    for i, (s, v) in enumerate((s, v) for s in shipped_spells(data) for v in s['variants']):
        tag = f"{i:02d}." + re.sub(r'\W+', '_', v['name']).strip('_')
        variant = (s['name'], v['name'])
        cases.append(_damage_case(f'dmg.player-at-monster.{tag}', 'spell-damage', variant,
                                  [_caster(), _victim()], targets=None, target='Araknid', tape=[12345]))
        cases.append(_damage_case(f'dmg.monster-at-player.{tag}', 'spell-damage', variant,
                                  [_caster('Monster', player=False), _victim('Locke', player=True, resists=[0, 0, 0, 0, 0, 0, 20, 0, 0, 0])],
                                  invoker='Monster', targets=None, target='Locke', tape=[777]))
        cases.append(_damage_case(f'dmg.nocaster.{tag}', 'spell-damage', variant, [_caster(), _victim()],
                                  invoker=None, targets=None, target='Araknid', tape=[4242]))
    for (sname, vname), resist, sdi, magres, roll in itertools.product(
            (('Fire Flash', 'Fire Flash'), ('Maelstrom', 'Maelstrom'), ('Lightning', 'Priest Bolt')),
            (None, 0, 1, 100, 250, 333, 500, 999, 1000, 1500, -200), (0, 25, -50), (0, 30, 100, -50), (0, 32767)):
        player = _caster()
        player['stats']['spelldamageinc'] = sdi
        victim = _victim('Locke2', player=True, magicresist=resist,
                         resists=[0, 0, 0, 0, 0, 0, magres, 0, 0, 0]) if magres else _victim(magicresist=resist)
        target = victim['name']
        cases.append(_damage_case(f'grid.{vname.replace(" ", "_")}.mr{resist}.sdi{sdi}.res{magres}.r{roll}', 'spell-damage',
                                  (sname, vname), [player, victim], targets=None, target=target, tape=[roll]))
    cases.append(_damage_case('dmg.notarget', 'spell-damage', ('Fire Flash', 'Fire Flash'), [_caster(), _victim()],
                              targets=None, target=None, tape=[5]))
    return finish(cases)


# ---- S4: missiles -------------------------------------------------------------------------

def area_cases(data: Path, workdir: Path) -> list[dict]:
    """AreaDamage around a point: who attacks (a player, a monster, nobody);
    characters on each side of the radius and the minimum radius, straight
    and diagonal; the attacker in range; the dead, a friend, one in an impact,
    a player with DmgResMagical; a player attacker's SpellDamageInc; the
    order the map gives them; a fixed and a rolled damage."""
    at = [1000, 1000, 0]
    cases = []

    def char(name, dx, dy, player=False, health=30, impact=False, resist=0, monster_type='Araknid'):
        if player:
            spec = _caster(name, root=('combat', 3), states=[], health=health)
            spec['resists'] = [0, 0, 0, 0, 0, 0, resist, 0, 0, 0]
        else:
            spec = _target(name, monster_type)
            spec['stats']['health'] = health
        spec['pos'] = [at[0] + dx, at[1] + dy, 0]
        if impact:
            spec['doing'] = dict(name='impact', action=12)
        return spec

    def case(name, attacker, chars, radius=150, minradius=0, lo=40, hi=60, sdi=0, friends=(), nearby=None, tape=None):
        for c in chars:
            if c['name'] == attacker and c['class'] == 11:
                c['stats']['spelldamageinc'] = sdi
        extra = dict(nearby=nearby) if nearby is not None else {}
        cases.append(dict(name=f'area.{name}', call='area-damage', attacker=attacker, pos=at, radius=radius,
                          minradius=minradius, min=lo, max=hi, type=7, friends=[list(f) for f in friends],
                          globals=dict(frame=0), chars=chars, tape=tape or [100, 2000, 30000, 7, 0, 32767],
                          ground=dict(z=0), **extra))   # ground: the map's characters come from the case

    ring = lambda r: [char('E', r, 0), char('N', 0, -r), char('SW', -r * 7 // 10, r * 7 // 10)]
    for who in ('Locke', 'Monster', None):
        attackers = [char('Locke', 5, 5, player=True), char('Monster', -5, 5, monster_type='Arakna')]
        for r in (0, 1, 50, 106, 149, 150, 151, 200):
            case(f'{who}.ring{r}', who, attackers + ring(r))
        for minr in (0, 49, 50, 51):
            case(f'{who}.min{minr}', who, attackers + ring(50), minradius=minr)
        case(f'{who}.gates', who, attackers + [char('Dead', 20, 0, health=0), char('Alive1', 0, 20, health=1),
                                               char('Hit', 20, 20, impact=True), char('Friend', -20, 0),
                                               char('Player2', 0, -20, player=True, resist=40)],
             friends=[('Locke', 'Friend'), ('Monster', 'Friend')])
        case(f'{who}.fixed', who, attackers + ring(60), lo=33, hi=33)
        case(f'{who}.reversed', who, attackers + ring(60), lo=60, hi=40)
        case(f'{who}.order', who, attackers + ring(60), nearby=['SW', 'Locke', 'N', 'Monster', 'E'])
    for sdi, resist in itertools.product((0, 25, -50, 200), (0, 30, 100, -50)):
        case(f'sdi{sdi}.res{resist}', 'Locke', [char('Locke', 5, 5, player=True),
                                                char('Player2', 30, 0, player=True, resist=resist),
                                                char('Mon', -30, 0)], sdi=sdi)
    return finish(cases)


# ---- S4b: an arrow's hit -----------------------------------------------------------

# class.def's AMMO types: [MagicType, DamageMod].
ARROW_TYPES = {'Arrow': (2, 0), 'Poison Arrow': (2, 5), 'Fire Arrow': (7, 10), 'Ice Arrow': (8, 10),
               'Magic Arrow': (0, 15)}
MOVE_BLOCKED = 2
CA_INTERACTIVE, CAI_INTERACTIVE, CHAR_INTERACTIVE = 0x2000000, 0x80, 0x80000


def _archer(player=True, level=5, bows=3, bow=20, x=600, **extra):
    """The shooter, well away from the arrow: a player (Level, the bows skill,
    a bow's DamageMod or none) or a monster."""
    if player:
        spec = _caster('Locke', root=('bow', 0x19), states=[], id=1, pos=[x, 1000, 0])
        spec['stats'].update(level=level, fatigue=100)
        spec['classstats'].update(bows=bows)
        if bow is not None:
            spec['bow'] = dict(name='Light Bow', damagemod=bow)
    else:
        spec = _caster('Archer', player=False, root=('combat', 3), states=[], id=2, pos=[x, 1000, 0])
    spec.update(extra)
    return spec


def _mark(name='Mon', kind='Araknid', at=(1000, 1000, 0), oid=3, health=30, player=False, level=4, **extra):
    """A character at the arrow (or `at`)."""
    if player:
        spec = _caster(name, root=('combat', 3), states=[], id=oid, pos=list(at), health=health)
        spec['stats'].update(level=level, fatigue=100)
    else:
        spec = _target(name, kind, id=oid)
        spec['pos'] = list(at)
        spec['stats']['health'] = health
    spec.update(extra)
    return spec


def _arrow(kind='Arrow', shooter='Locke', at=(1000, 1000, 0), vel=(0x140000, 0, 0), killwait=-1, flags=0x10000,
           **extra):
    magic, mod = ARROW_TYPES.get(kind, (2, 0))
    spec = dict(name='arrow', type=kind, magictype=magic, damagemod=mod, pos=list(at), vel=list(vel),
                flags=flags, killwait=killwait, shooter=shooter, id=900)
    spec.update(extra)
    return spec


def arrow_cases(data: Path, workdir: Path) -> list[dict]:
    """TAmmo::Move on an arrow in flight: no hit (flying on, not flying, done
    already, a wall with nobody there), a hit from a player (Level, bows, the
    bow's DamageMod, none), a monster and a trap on a monster and on the
    player; a friend; the height window; every arrow type of class.def; the
    poison and fire procs either side of their rolls, the cast answered
    either way; a zero damage roll; the message with no main player; the
    ice burst's filters and its cap; then TAmmo::Pulse."""
    cases = []

    def case(name, chars, arrow, bits=0, tape=None, **extra):
        c = dict(name=f'arrow.{name}', call='arrow-move', chars=chars, arrow=arrow, bits=bits,
                 globals=dict(frame=0), ground=dict(z=0), tape=tape or [16000, 3000, 30000, 9000, 22000, 7, 0])
        c.update(extra)
        cases.append(c)

    # No hit.
    case('fly', [_archer(), _mark(at=(1300, 1000, 0))], _arrow())
    case('notflying', [_archer(), _mark()], _arrow(flags=0))
    case('handled', [_archer(), _mark()], _arrow(killwait=0))
    case('wall.nobody', [_archer(), _mark(at=(1300, 1000, 0))], _arrow(), bits=MOVE_BLOCKED)
    case('wall.monster', [_archer(player=False), _mark(at=(1300, 1000, 0))], _arrow(shooter='Archer'),
         bits=MOVE_BLOCKED)
    case('wall.trap', [_mark(at=(1300, 1000, 0))], _arrow(shooter=None), bits=MOVE_BLOCKED)
    # The height window (a wide victim so it is the one there).
    for dz in (80, 81, -80, -81):
        big = dict(classstats=dict(radius=120))
        case(f'height.{dz}', [_archer(), _mark(at=(1000, 1000, dz), **big)], _arrow())
        case(f'height.{dz}.wall', [_archer(), _mark(at=(1000, 1000, dz), **big)], _arrow(), bits=MOVE_BLOCKED)
    # Who shot it.
    for level, bows, bow in ((5, 3, 20), (1, 0, None), (12, 9, 0), (20, 15, 100), (3, 2, -50)):
        case(f'player.l{level}.b{bows}.bow{bow}', [_archer(level=level, bows=bows, bow=bow), _mark()], _arrow())
    case('player.blocked', [_archer(), _mark()], _arrow(), bits=MOVE_BLOCKED)
    case('player.friend', [_archer(), _mark()], _arrow(), friends=[('Locke', 'Mon')])
    case('player.nomain', [_archer(), _mark()], _arrow(), mainplayer=False)
    case('monster', [_archer(player=False), _mark()], _arrow(shooter='Archer'))
    case('monster.friend', [_archer(player=False), _mark()], _arrow(shooter='Archer'), friends=[('Archer', 'Mon')])
    case('monster.atplayer', [_archer(player=False), _mark('Locke', player=True, oid=1)], _arrow(shooter='Archer'))
    case('trap.monster', [_mark()], _arrow(shooter=None))
    for level in (1, 4, 20):
        case(f'trap.player.l{level}', [_mark('Locke', player=True, oid=1, level=level)], _arrow(shooter=None))
    # Every arrow type, from the player and from a trap.
    for kind in ARROW_TYPES:
        tag = kind.replace(' ', '')
        case(f'type.{tag}', [_archer(), _mark()], _arrow(kind))
        case(f'type.{tag}.trap', [_mark()], _arrow(kind, shooter=None))
    # The procs: poison (MagicType 9) under 33, fire (7) under 66, the cast
    # answered either way; by a monster's arrow too.
    for magic, rolls in ((9, (32, 33)), (7, (65, 66))):
        for roll in rolls:
            for cast in (1, 0):
                for who, chars, shooter in (('player', [_archer(), _mark()], 'Locke'),
                                            ('monster', [_archer(player=False), _mark()], 'Archer'),
                                            ('trap', [_mark()], None)):
                    case(f'proc.{magic}.r{roll}.c{cast}.{who}', chars, _arrow(magictype=magic, shooter=shooter),
                         tape=[16000, 3000, roll], casts=[cast])
    # The damage roll's ends, and nothing to roll (a bow taking it all off).
    for draw in (0, 32767):
        case(f'roll.{draw}', [_archer(), _mark()], _arrow(), tape=[0, draw])
    # AMMODATA other than the defaults (rules.def's commented line).
    for ammo in ((20, 6, 3, 2, 25), (0, 0, 0, 0, 25), (50, 10, 10, 5, 100), (20, 6, 4, 1, 0)):
        tag = '.'.join(map(str, ammo))
        case(f'ammodata.{tag}', [_archer(), _mark()], _arrow(), globals=dict(frame=0, ammodata=list(ammo)))
        case(f'ammodata.{tag}.monster', [_archer(player=False), _mark()], _arrow(shooter='Archer'),
             globals=dict(frame=0, ammodata=list(ammo)))
        case(f'ammodata.{tag}.trap', [_mark('Locke', player=True, oid=1)], _arrow(shooter=None),
             globals=dict(frame=0, ammodata=list(ammo)))
    case('roll.zero', [_archer(level=0, bows=0, bow=-100), _mark()], _arrow(damagemod=0), tape=[0])
    case('roll.zero.poison', [_archer(level=0, bows=0, bow=-100), _mark()], _arrow(magictype=9), tape=[0, 0])
    # The ice burst: only Solifuges, and of them not the main player, the
    # dead, one held in an interactive attack or impact (unless it is the
    # mover), one skywalking; the first ten characters the map gives.
    def sol(name, at, oid, **extra):
        return _mark(name, 'Solifuge', at=at, oid=oid, **extra)

    skip = [sol('Sol', (1100, 1000, 0), 10),
            sol('DeadSol', (1100, 1100, 0), 11, health=0),
            sol('HeldSol', (1100, 900, 0), 12, doing_attack=dict(flags=CA_INTERACTIVE)),
            sol('HitSol', (900, 1100, 0), 13, doing_impact=dict(flags=CAI_INTERACTIVE)),
            sol('MoverSol', (900, 900, 0), 14, doing_attack=dict(flags=CA_INTERACTIVE), charflags=CHAR_INTERACTIVE),
            sol('SkySol', (1200, 1000, 0), 15, doing=dict(name='skywalk2', action=3)),
            _mark('Plain', at=(1000, 1200, 0), oid=16)]
    names = [s['name'] for s in skip]
    case('ice.burst', [_archer()] + [_mark()] + skip, _arrow('Ice Arrow'), iced=names + ['Locke', 'Mon'])
    case('ice.burst.trap', [_mark()] + skip, _arrow('Ice Arrow', shooter=None), iced=['Mon'] + names)
    case('ice.burst.nomain', [_archer()] + [_mark()] + skip, _arrow('Ice Arrow'), iced=['Locke'] + names,
         mainplayer=False)
    crowd = [sol(f'S{i}', (1000 + 30 * i, 1300, 0), 20 + i) for i in range(12)]
    case('ice.cap', [_archer(), _mark()] + crowd, _arrow('Ice Arrow'), iced=[s['name'] for s in crowd])
    case('ice.wall', [_archer(), _mark(at=(1300, 1000, 0))] + skip, _arrow('Ice Arrow'), bits=MOVE_BLOCKED,
         iced=names)
    case('ice.friend', [_archer(), _mark()] + skip, _arrow('Ice Arrow'), iced=names, friends=[('Locke', 'Mon')])
    case('ice.onvictim', [_archer(), _mark('Target', 'Solifuge')], _arrow('Ice Arrow'), iced=['Target'])

    # TAmmo::Pulse.
    for killwait in (-1, 0, 1, 2, 5):
        for vel in ((0, 0, 0), (0x140000, 0, 0), (0, 0, -1)):
            for editor in (0, 1):
                tag = f'k{killwait}.v{"".join("1" if c else "0" for c in vel)}.e{editor}'
                cases.append(dict(name=f'pulse.{tag}', call='arrow-pulse', chars=[_archer()],
                                  arrow=_arrow(killwait=killwait, vel=vel), globals=dict(frame=0, editor=editor),
                                  ground=dict(z=0)))
    for killwait in (-1, 0, 3):
        cases.append(dict(name=f'pulse.owned.k{killwait}', call='arrow-pulse', chars=[_archer()],
                          arrow=_arrow(killwait=killwait, vel=(0, 0, 0), owner='Locke'), globals=dict(frame=0),
                          ground=dict(z=0)))
    return finish(cases)


# ---- S4c: the bow ------------------------------------------------------------------

BOW_CHARDATA = dict(arrowpos=[-10, -15, 50], arrowspeed=20, bowwait=12, bowaimspeed=8)
BOW_ROOT = dict(name='bow', action=0x19)
BOW_AIM = dict(name='bowaim', action=0x1b, angle=0, moveangle=0, turnrate=8)


def _bowman(player=True, facing=0, doing=None, root=None, ammo=None, chardata=None, **extra):
    """The bow root's shooter, aiming by default (doing "bowaim")."""
    name = 'Locke' if player else 'Archer'
    spec = _caster(name, player=player, root=('bow', 0x19), states=['bowaim', 'bowshoot'], id=1 if player else 2,
                   pos=[1000, 1000, 0], facing=facing, moveangle=facing)
    spec['root'] = dict(root or BOW_ROOT)
    spec['doing'] = dict(doing) if doing is not None else dict(BOW_AIM, angle=facing, moveangle=facing)
    spec['chardata'].update(BOW_CHARDATA, **(chardata or {}))
    if player:
        spec['stats'].update(fatigue=100)
        spec['ammo'] = [dict(name='Arrow', amount=12, equipped=True)] if ammo is None else ammo
    spec.update(extra)
    return spec


def bow_cases(data: Path, workdir: Path) -> list[dict]:
    """The bow's commands and resolvers: DrawBow from the roots and doings
    that let it and don't, with ammo equipped, carried or none; AimBow and
    its left / right steps over the turn-rate's steps and the stop; ShootBow
    queueing a shot (an idle player woken); IsBowDrawn by root name;
    ResolveBowAim's stop and turn; ResolveBowShoot's pace, the arrow's def
    from each ARROWPOS and facing, the ammo counted down, the slot emptied
    with the last, no ammo, a monster, the arrow given its shooter."""
    cases = []

    def case(name, call, chars, self='Locke', **extra):
        c = dict(name=f'bow.{name}', call=call, self=self, chars=chars, globals=dict(frame=100), tape=[7, 3, 11, 0])
        c.update(extra)
        cases.append(c)

    idle = dict(name='bow', action=0x19)
    walking = dict(name='bowwalk', action=0x1a)
    # DrawBow.
    case('draw.idle', 'draw-bow', [_bowman(doing=idle)])
    case('draw.walking', 'draw-bow', [_bowman(doing=walking)])
    case('draw.moving', 'draw-bow', [_bowman(doing=dict(name='walk', action=2))])
    case('draw.drawn', 'draw-bow', [_bowman()])
    case('draw.combatroot', 'draw-bow', [_bowman(root=dict(name='combat', action=3), doing=dict(name='combat', action=3))])
    case('draw.dead', 'draw-bow', [_bowman(doing=idle, health=0)])
    case('draw.locked', 'draw-bow', [_bowman(doing=idle, doing_attack=dict(flags=CA_INTERACTIVE))])
    case('draw.mover', 'draw-bow', [_bowman(doing=idle, doing_attack=dict(flags=CA_INTERACTIVE),
                                            charflags=CHAR_INTERACTIVE)])
    case('draw.carried', 'draw-bow', [_bowman(doing=idle, ammo=[dict(name='Fire Arrow', amount=3)])])
    case('draw.none', 'draw-bow', [_bowman(doing=idle, ammo=[])])
    case('draw.monster', 'draw-bow', [_bowman(player=False, doing=idle)], self='Archer')
    for facing in (0, 77, 200):
        case(f'draw.facing{facing}', 'draw-bow', [_bowman(doing=idle, facing=facing)])
    # AimBow and its steps.
    for facing, angle in ((0, 0), (0, 32), (0, 33), (0, 64), (0, 100), (0, 128), (0, 160), (10, 200), (200, 10),
                          (128, 0), (250, 5)):
        case(f'aim.{facing}.{angle}', 'aim-bow', [_bowman(facing=facing)], angle=angle)
    case('aim.stopped', 'aim-bow', [_bowman(doing=dict(BOW_AIM, flags=['stop']))], angle=90)
    case('aim.notdrawn', 'aim-bow', [_bowman(doing=idle)], angle=90)
    case('aim.dead', 'aim-bow', [_bowman(health=0)], angle=90)
    case('aim.locked', 'aim-bow', [_bowman(doing_attack=dict(flags=CA_INTERACTIVE))], angle=90)
    for facing in (0, 4, 250, 128):
        for speed in (8, 3):
            for call in ('aim-left', 'aim-right'):
                case(f'{call}.{facing}.s{speed}', call, [_bowman(facing=facing, chardata=dict(bowaimspeed=speed))])
    # ShootBow.
    for facing, angle in ((0, 0), (0, 40), (0, 200), (100, 30)):
        case(f'shoot.{facing}.{angle}', 'shoot-bow', [_bowman(facing=facing)], angle=angle)
    case('shoot.queued', 'shoot-bow', [_bowman(bowshots=2)], angle=10)
    case('shoot.idleplayer', 'shoot-bow', [_bowman(playerstate=2)], angle=10)
    case('shoot.idleplayer6', 'shoot-bow', [_bowman(playerstate=6)], angle=10)
    case('shoot.notdrawn', 'shoot-bow', [_bowman(doing=idle)], angle=10)
    case('shoot.dead', 'shoot-bow', [_bowman(health=0)], angle=10)
    case('shoot.monster', 'shoot-bow', [_bowman(player=False)], self='Archer', angle=10)
    # IsBowDrawn.
    case('drawn.yes', 'is-bow-drawn', [_bowman()])
    case('drawn.idle', 'is-bow-drawn', [_bowman(doing=idle)])
    case('drawn.longbow', 'is-bow-drawn', [_bowman(root=dict(name='longbow', action=0x19),
                                                   doing=dict(name='LONGBOWAIM', action=0x1b))])
    case('drawn.combat', 'is-bow-drawn', [_bowman(root=dict(name='combat', action=3),
                                                  doing=dict(name='combataim', action=0x1b))])
    # ResolveBowAim.
    for facing, angle, stop in ((0, 0, True), (0, 0, False), (0, 40, True), (0, 40, False), (250, 10, False),
                                (100, 100, True)):
        aim = dict(BOW_AIM, angle=angle, moveangle=angle, turnrate=12, flags=['stop'] if stop else [])
        case(f'resolve-aim.{facing}.{angle}.{"stop" if stop else "go"}', 'resolve-bow-aim',
             [_bowman(facing=facing, doing=aim, movedist=7)])
    # ResolveBowShoot.
    shot = dict(name='bowshoot', action=0x1c, angle=40, moveangle=40)

    def shooting(**extra):
        extra.setdefault('doing', shot)
        extra.setdefault('bowshots', 1)
        extra.setdefault('lastbowshot', 0)
        return _bowman(**extra)

    # NewObject gives the case's arrow unless a case says otherwise.
    def fire(name, chars, **extra):
        extra.setdefault('newobject', 900)
        extra.setdefault('arrow', _arrow(shooter=None))
        case(name, 'resolve-bow-shoot', chars, **extra)

    fire('fire.nothingmade', [shooting()], newobject=-1)
    fire('fire.notammo', [shooting()], newobject=1)
    fire('fire.one', [shooting()])
    fire('fire.three', [shooting(bowshots=3)])
    fire('fire.none', [shooting(bowshots=0)])
    fire('fire.negative', [shooting(bowshots=-1)])
    for last in (88, 87, 100, 101):
        fire(f'fire.pace.{last}', [shooting(lastbowshot=last)])
    fire('fire.lastarrow', [shooting(ammo=[dict(name='Arrow', amount=1, equipped=True)])])
    fire('fire.twoleft', [shooting(ammo=[dict(name='Arrow', amount=2, equipped=True)])])
    fire('fire.noammo', [shooting(ammo=[])])
    fire('fire.carriedonly', [shooting(ammo=[dict(name='Arrow', amount=5)])])
    for kind in ARROW_TYPES:
        fire(f'fire.type.{kind.replace(" ", "")}',
             [shooting(ammo=[dict(name=kind, amount=4, equipped=True)])])
    for facing, angle in ((0, 0), (0, 64), (90, 200), (200, 7), (255, 128)):
        for pos in ((-10, -15, 50), (-5, -15, 80), (0, 0, 0), (20, 3, 40), (-300, 400, 10)):
            tag = f'{facing}.{angle}.{"_".join(map(str, pos))}'
            fire(f'fire.aim.{tag}',
                 [shooting(facing=facing, doing=dict(shot, angle=angle, moveangle=angle),
                           chardata=dict(arrowpos=list(pos)))])
    for speed in (0, 1, 20, 37):
        fire(f'fire.speed{speed}', [shooting(chardata=dict(arrowspeed=speed))])
    fire('fire.monster', [shooting(player=False)], self='Archer')
    return finish(cases)


# ---- S4d: a spell's fireball, tick by tick -------------------------------------------

def fireball_cases(data: Path, workdir: Path) -> list[dict]:
    """A FireBall from launch to blast: at a target ahead, beside and behind
    (the aim from the spell's target, or the caster's facing), into a wall,
    past a friend and the dead, out of life; from a monster at the player; no
    caster; no spell (no blast); the two variants; the blast's radius over
    the ones near where it bursts."""
    cases = []

    def case(name, chars, ticks=40, variant=('Fireball', 'Fireball'), invoker='Locke', targets=('Mon',), **extra):
        c = dict(name=f'fireball.{name}', ticks=ticks, variant=list(variant), invoker=invoker,
                 targets=list(targets) if targets is not None else None, fireball=dict(pos=[1000, 1000, 60]),
                 globals=dict(frame=0), ground=dict(z=10), chars=chars, tape=[100, 2000, 30000, 7, 0, 32767])
        c.update(extra)
        cases.append(c)

    def caster(player=True, facing=0, **extra):
        spec = _caster('Locke' if player else 'Archer', player=player, root=('combat', 3), states=[], id=1,
                       pos=[1000, 1000, 0], facing=facing, moveangle=facing)
        spec['stats'].update(spelldamageinc=0) if player else None
        spec.update(extra)
        return spec

    def mon(name='Mon', at=(1200, 1000, 0), oid=3, **extra):
        spec = _target(name, 'Araknid', id=oid)
        spec['pos'] = list(at)
        spec.update(extra)
        return spec

    for at in ((1200, 1000, 0), (1000, 1200, 0), (800, 1000, 0), (1150, 1150, 0), (1003, 1300, 0)):
        tag = '_'.join(map(str, at))
        case(f'at.{tag}', [caster(), mon(at=at)])
    case('notarget.facing0', [caster(facing=0), mon(at=(1000, 800, 0))], targets=None)
    case('notarget.facing64', [caster(facing=64), mon(at=(1200, 1000, 0))], targets=None)
    case('target.self', [caster(facing=128), mon(at=(800, 1000, 0))], targets=('Locke',))
    case('miss.life', [caster(), mon(at=(1000, 1400, 0))], ticks=70)
    case('wall', [caster(), mon(at=(1300, 1000, 0))],
         ground=dict(z=10, cells=[[70, 0, 80, 200, 400]]))
    case('friend.inpath', [caster(), mon('Pal', at=(1100, 1000, 0), oid=4), mon(at=(1200, 1000, 0))],
         friends=[('Locke', 'Pal')])
    case('dead.inpath', [caster(), mon('Corpse', at=(1100, 1000, 0), oid=4, stats=dict(health=0, mana=0, poisoned=0)),
                         mon(at=(1200, 1000, 0))])
    case('crowd', [caster(), mon(at=(1200, 1000, 0)), mon('M2', at=(1260, 1060, 0), oid=4),
                   mon('M3', at=(1330, 1000, 0), oid=5), mon('M4', at=(1400, 1000, 0), oid=6),
                   mon('Pal', at=(1220, 940, 0), oid=7)], friends=[('Locke', 'Pal')])
    case('monster.atplayer', [caster(player=False), _caster('Hero', root=('combat', 3), states=[], id=8,
                                                           pos=[1200, 1000, 0])],
         invoker='Archer', targets=('Hero',))
    case('nocaster', [caster(), mon(at=(1200, 1000, 0))], invoker=None)
    case('nospell', [caster(), mon(at=(1200, 1000, 0))], spell=False, ticks=30)
    case('priest', [caster(), mon(at=(1200, 1000, 0))], variant=('Fireball', 'Priest Fireball'))
    case('high', [caster(), mon(at=(1200, 1000, 0))], fireball=dict(pos=[1000, 1000, 120]))
    case('low', [caster(), mon(at=(1200, 1000, 0))], fireball=dict(pos=[1000, 1000, 12]))
    case('underground', [caster(), mon(at=(1200, 1000, 0))], fireball=dict(pos=[1000, 1000, 5]))
    case('nowalkmap', [caster(), mon(at=(1200, 1000, 0))], ground=dict(z=0))
    return finish(cases)


TARGETS = {
    'missile-fireball': dict(fixture='slots/combat/spell_missile.py', cases=fireball_cases, compare=generic_compare,
                             port_fields=port_fields, unit=lambda r: len(r.get('ticks', []))),
    'missile-bow': dict(fixture='slots/combat/spell_bow.py', cases=bow_cases, compare=generic_compare,
                        port_fields=port_fields, unit=lambda r: 1),
    'missile-arrow': dict(fixture='slots/combat/spell_arrow.py', cases=arrow_cases, compare=generic_compare,
                          port_fields=port_fields, unit=lambda r: 1),
    'missile-area': dict(fixture='slots/combat/spell_damage.py', cases=area_cases, compare=generic_compare,
                         port_fields=port_fields, unit=lambda r: 1),
    'spell-new': dict(fixture='slots/combat/spell_damage.py', cases=spell_new_cases, compare=generic_compare,
                      port_fields=port_fields, unit=lambda r: 1),
    'spell-damage': dict(fixture='slots/combat/spell_damage.py', cases=spell_damage_cases, compare=generic_compare,
                         port_fields=port_fields, unit=lambda r: 1),
    'spell-cast': dict(fixture='slots/combat/spell_cast.py', cases=cast_cases, compare=generic_compare,
                       port_fields=port_fields, unit=lambda r: 1),
    'spell-data': dict(fixture='slots/combat/spell_data.py', cases=spell_data_cases, compare=compare_spell_data,
                       port_fields=spell_data_port_fields, unit=lambda r: _leaves(r.get('spells', []))),
    'spell-lookup': dict(fixture='slots/combat/spell_talismans.py', cases=lookup_cases, compare=generic_compare,
                         port_fields=port_fields, unit=lambda r: len(r['returned'])),
    'spell-talismans': dict(fixture='slots/combat/spell_talismans.py', cases=talisman_cases,
                            compare=generic_compare, port_fields=port_fields, unit=lambda r: len(r['returned'])),
    'spell-quick': dict(fixture='slots/combat/spell_talismans.py', cases=quick_cases, compare=generic_compare,
                        port_fields=port_fields, unit=lambda r: 1),
}
