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
    """[{"name", "variants": [{"name", "talismans"}]}] in file order, read off
    resources.rvr's spell.def (names and codes only: the parse itself is
    kata D1s's)."""
    spells, current = [], None
    for raw in shipped_spell_def(data).decode('cp1252').splitlines():
        line = raw.split('//', 1)[0].strip()
        m = re.match(r'SPELL\s+"([^"]*)"', line)
        if m:
            current = dict(name=m.group(1), variants=[])
            spells.append(current)
            continue
        m = re.match(r'VARIANT\s+"([^"]*)"\s*,\s*[^,]+,\s*"([^"]*)"', line, re.I)
        if m and current is not None:
            current['variants'].append(dict(name=m.group(1), talismans=m.group(2)[:5]))
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


TARGETS = {
    'spell-data': dict(fixture='slots/combat/spell_data.py', cases=spell_data_cases, compare=compare_spell_data,
                       port_fields=spell_data_port_fields, unit=lambda r: _leaves(r.get('spells', []))),
    'spell-lookup': dict(fixture='slots/combat/spell_talismans.py', cases=lookup_cases, compare=generic_compare,
                         port_fields=port_fields, unit=lambda r: len(r['returned'])),
    'spell-talismans': dict(fixture='slots/combat/spell_talismans.py', cases=talisman_cases,
                            compare=generic_compare, port_fields=port_fields, unit=lambda r: len(r['returned'])),
    'spell-quick': dict(fixture='slots/combat/spell_talismans.py', cases=quick_cases, compare=generic_compare,
                        port_fields=port_fields, unit=lambda r: 1),
}
