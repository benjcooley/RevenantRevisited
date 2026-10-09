"""Combat dojo data targets for retail_ab.py (docs/gameplay/COMBAT_DOJO.md, kata D1).

`combat-data`: the combat data as each side parses it. Retail runs
TRules::Initialize (Load over rules.def, stats.def, char.def, weapon.def,
armor.def) and BindTypes in the emulator (slots/combat/data_parse.py); the
port runs TRules::Initialize (`Revenant --retail-ab=combat-data`,
src/retailab_data.cpp). Both dump every record by field name; the compare
walks them record by record, so a difference reads as
`char Araknid attack 3 hitmaxrange`.

A case is a directory of .def files laid out as an install's (Resources/,
Imagery/), written under the work directory so both sides read the same
bytes: retail through its file seams (`.\\Resources\\rules.def`), the port
with ClassDefPath / ImageryPath pointed at the case's folders. Plus the game
speed (TWILIGHT's steps depend on it) and class.def's type names by class
(retail binds CHARACTER names to object types through them).

Cases:
- `shipped`: the archives' files, the ones retail reads (COMBAT_DATA.md §2):
  rules.def and stats.def from resources.rvr; char.def, weapon.def and
  armor.def from imagery.rvi.
- `shipped.gamespeed5`: the same at GameSpeed 5.
- `gog-loose`: the GOG install's loose rules.def and char.def (the value
  differences in §2.5) over the archives' other files.
- `edge.<name>`: tools/retail_ab/cases/combat_data/<name>/, every tag and
  default the shipped files don't reach.
"""
from __future__ import annotations

import hashlib
import json
import re
import zipfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[1]
IGNORED = {'schema', 'side', 'elapsed_ms', 'seams'}

# class.def's CLASS names -> class ids (src/object.h OBJCLASS_*, retail's
# class registry index).
CLASS_IDS = dict(ITEM=0, WEAPON=1, ARMOR=2, TALISMAN=3, FOOD=4, CONTAINER=5, LIGHTSOURCE=6, TOOL=7,
                 MONEY=8, TILE=9, EXIT=10, PLAYER=11, CHARACTER=12, TRAP=13, SHADOW=14, HELPER=15,
                 KEY=16, INVCONTAINER=17, POTION=18, AMMO=21, SCROLL=22, RANGEDWEAPON=23, EFFECT=25,
                 MAPSCROLL=26)


def class_table(text: bytes) -> list[dict]:
    """class.def's type names per class, in file order (the type index)."""
    classes, current, in_types, depth = [], None, False, 0
    for raw in text.decode('cp1252').splitlines():
        line = raw.split('//', 1)[0].strip()
        if not line:
            continue
        m = re.match(r'CLASS\s+"([^"]+)"', line)
        if m and not in_types:
            current = dict(id=CLASS_IDS[m.group(1).upper()], types=[])
            classes.append(current)
            continue
        if line.upper() == 'TYPES':
            in_types, depth = True, 0
            continue
        if in_types:
            if line.upper() == 'BEGIN':
                depth += 1
            elif line.upper() == 'END':
                depth -= 1
                if depth == 0:
                    in_types = False
            elif depth == 1:
                name = re.match(r'"([^"]*)"', line)
                if name:
                    current['types'].append(name.group(1))
    return classes


def _member(z: zipfile.ZipFile, name: str) -> bytes:
    for n in z.namelist():
        if n.lower() == name.lower():
            return z.read(n)
    raise KeyError(f'{name} not in {z.filename}')


def data_cases(data: Path, workdir: Path) -> list[dict]:
    rvr, rvi = zipfile.ZipFile(data / 'resources.rvr'), zipfile.ZipFile(data / 'imagery.rvi')
    shipped = {'Resources/rules.def': _member(rvr, 'rules.def'), 'Resources/stats.def': _member(rvr, 'stats.def'),
               'Imagery/char.def': _member(rvi, 'char.def'), 'Imagery/weapon.def': _member(rvi, 'weapon.def'),
               'Imagery/armor.def': _member(rvi, 'armor.def')}
    classes = class_table(_member(rvi, 'class.def'))
    cases = []

    def add(name, files, origin, gamespeed=3):
        root = workdir / 'cases' / name
        paths = {}
        for rel, blob in sorted(files.items()):
            path = root / rel
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(blob)
            paths[rel] = str(path)
        digest = hashlib.sha256(json.dumps([gamespeed, classes, sorted(
            (rel, hashlib.sha256(blob).hexdigest()) for rel, blob in files.items())]).encode()).hexdigest()
        cases.append(dict(name=name, dir=str(root), files=paths, gamespeed=gamespeed, classes=classes,
                          origin=origin, sha256=digest))

    add('shipped', shipped, 'resources.rvr + imagery.rvi')
    add('shipped.gamespeed5', shipped, 'resources.rvr + imagery.rvi', gamespeed=5)
    loose_rules, loose_char = REPO / 'data' / 'Resources' / 'rules.def', REPO / 'data' / 'Imagery' / 'char.def'
    if loose_rules.exists() and loose_char.exists():
        add('gog-loose', dict(shipped, **{'Resources/rules.def': loose_rules.read_bytes(),
                                          'Imagery/char.def': loose_char.read_bytes()}),
            'data/Resources/rules.def + data/Imagery/char.def (GOG loose)')
    for folder in sorted((HERE / 'cases' / 'combat_data').glob('*')):
        if folder.is_dir():
            files = {str(p.relative_to(folder)): p.read_bytes() for p in sorted(folder.rglob('*.def'))}
            add(f'edge.{folder.name}', files, str(folder.relative_to(REPO)))
    return cases


def port_fields(case: dict) -> list[str]:
    body = {k: case[k] for k in ('dir', 'gamespeed')}
    return [json.dumps(body, separators=(',', ':'), ensure_ascii=True)]


# ---- compare ------------------------------------------------------------------

def _fields(where: str, retail: dict, port: dict, out: list, skip=()):
    for key in list(retail) + [k for k in port if k not in retail]:
        if key in skip or key in IGNORED:
            continue
        r, p = retail.get(key, '<absent>'), port.get(key, '<absent>')
        if r != p:
            out.append(dict(where=where, line=None, field=f'{where} {key}', retail=r, port=p))


def _impacts(where, retail, port, out):
    for i in range(max(len(retail), len(port))):
        if i >= len(retail) or i >= len(port):
            out.append(dict(where=where, line=None, field=f'{where} impact {i}',
                            retail=retail[i] if i < len(retail) else '<absent>',
                            port=port[i] if i < len(port) else '<absent>'))
            continue
        _fields(f'{where} impact {i}', retail[i], port[i], out)


def _records(kind, retail, port, out, compare):
    rn, pn = [r['name'] for r in retail], [p['name'] for p in port]
    if rn != pn:
        out.append(dict(where=kind, line=None, field=f'{kind} order', retail=rn, port=pn))
    pmap = {}
    for p in port:
        pmap.setdefault(p['name'].lower(), p)
    for r in retail:
        p = pmap.get(r['name'].lower())
        if p is None:
            out.append(dict(where=kind, line=None, field=f"{kind} {r['name']}", retail='present', port='<absent>'))
            continue
        compare(f"{kind} {r['name']}", r, p, out)


def _char(where, r, p, out):
    _fields(where, r, p, out, skip=('impacts', 'attacks', 'attacheffects'))
    for i, (a, b) in enumerate(zip(r['attacheffects'], p.get('attacheffects', []))):
        _fields(f'{where} attacheffect {i}', a, b, out)
    _impacts(where, r['impacts'], p.get('impacts', []), out)
    ra, pa = r['attacks'], p.get('attacks', [])
    if len(ra) != len(pa):
        out.append(dict(where=where, line=None, field=f'{where} attack count', retail=len(ra), port=len(pa)))
    for i, (a, b) in enumerate(zip(ra, pa)):
        _fields(f'{where} attack {i}', a, b, out, skip=('impacts',))
        _impacts(f'{where} attack {i}', a.get('impacts', []), b.get('impacts', []), out)


def compare(case: dict, retail: dict, port: dict) -> list[dict]:
    out = []
    if retail.get('fatal') or port.get('fatal'):
        if retail.get('fatal') != port.get('fatal'):
            out.append(dict(where='load', line=None, field='fatal', retail=retail.get('fatal'),
                            port=port.get('fatal')))
        return out
    rr, pr = retail['rules'], port['rules']
    _fields('rules', rr, pr, out, skip=('statlevels',))
    names = ['Strength', 'Constitution', 'Agility', 'Reflexes', 'Mind', 'Luck']
    for t, (a, b) in enumerate(zip(rr['statlevels'], pr.get('statlevels', []))):
        for level, (x, y) in enumerate(zip(a, b)):
            if x != y:
                out.append(dict(where='statlevel', line=None, field=f'statlevel {names[t]} {level}', retail=x, port=y))
    _records('class', retail['classes'], port['classes'], out, _fields)
    _records('char', retail['chars'], port['chars'], out, _char)
    _records('weapon', retail['weapons'], port['weapons'], out, _fields)
    _records('armor', retail['armor'], port['armor'], out, _fields)
    return out


def _leaves(value) -> int:
    if isinstance(value, dict):
        return sum(_leaves(v) for k, v in value.items() if k not in IGNORED)
    if isinstance(value, list):
        return sum(_leaves(v) for v in value)
    return 1


def fields_compared(result: dict) -> int:
    """The unit: every leaf value of the dump (a list element is one)."""
    return _leaves({k: v for k, v in result.items() if k not in ('initialize', 'fatal')})


TARGETS = {
    'combat-data': dict(fixture='slots/combat/data_parse.py', cases=data_cases, compare=compare,
                        port_fields=port_fields, unit=fields_compared),
}
