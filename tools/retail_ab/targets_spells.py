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
import json
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


TARGETS = {
    'spell-data': dict(fixture='slots/combat/spell_data.py', cases=spell_data_cases, compare=compare_spell_data,
                       port_fields=spell_data_port_fields, unit=lambda r: _leaves(r.get('spells', []))),
}
