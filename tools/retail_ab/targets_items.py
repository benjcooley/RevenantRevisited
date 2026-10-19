"""The item targets of retail_ab.py (docs/gameplay/forensics/CONSUMABLES.md).

`item-food-use` runs TFood::Use `0x0050e780` (FOOD's and POTION's Use) on
every FOOD and POTION type of the shipped class.def, and on the edge cases:
caps, a stat above its maximum, stacks, the belt, a pouch, an item on the
map, a dead user, a `with`, a non-player user, a user who isn't the main
player, the cure. Retail side: tools/retail_runtime/slots/items/food_use.py;
port side: `Revenant --retail-ab=item-food-use` (src/game/ab/retailab_items.cpp).
Compared: the return, the stat writes in order, the sounds looked up, the
text-bar lines, the cure's map scan (square, objset, level) and the item
afterwards (still held, its amount).
"""
from __future__ import annotations

import hashlib
import json
import re
import zipfile
from pathlib import Path

PLAYER, CHARACTER = 11, 12
LOCKE = dict(name='Locke', type='Locke', **{'class': PLAYER}, health=50, maxhealth=2000, mana=50,
             maxmana=2000, fatigue=10, maxfatigue=100, poisoned=0, pos=[1000, 2000, 0], level=0)
NPC = dict(LOCKE, name='Tendrick', type='Tendrick', **{'class': CHARACTER})
SOMEBODY = dict(LOCKE, name='Somebody', pos=[5000, 5000, 0])
BELT_FIRST = 0x10b


def class_types(data: Path, classes: tuple[str, ...]) -> list[str]:
    """The type names of the named classes in the shipped class.def (imagery.rvi)."""
    with zipfile.ZipFile(data / 'imagery.rvi') as z:
        name = next(n for n in z.namelist() if n.lower().endswith('class.def'))
        text = z.read(name).decode('cp1252')
    types = []
    for cls in classes:
        block = re.search(r'CLASS\s+"%s"(.*?)\nEND' % re.escape(cls), text, re.S).group(1)
        rows = re.search(r'TYPES\s+BEGIN(.*?)END', block, re.S).group(1)
        types += re.findall(r'^\s*"([^"]+)"\s+"', rows, re.M)
    return types


def _sha(case: dict) -> str:
    body = {k: v for k, v in case.items() if k not in ('name', 'sha256', 'origin')}
    return hashlib.sha256(json.dumps(body, sort_keys=True).encode()).hexdigest()


def food_cases(data: Path, workdir: Path) -> list[dict]:
    cases = []

    def case(name, item, user=None, base=LOCKE, main='user', with_=-1):
        body = dict(item=dict(dict(where='pack', slot=0, amount=1), **item), user=dict(base, **(user or {})),
                    main=main, **{'with': with_})
        cases.append(dict(name=f'food.{name}', origin='targets_items.py', **body))

    # Every shipped food and potion, poisoned so that only the cure clears it.
    for kind in class_types(data, ('FOOD', 'POTION')):
        case('type.' + re.sub(r'\W+', '-', kind.lower()), dict(type=kind), dict(poisoned=1))
    # The caps: over, exactly at, a stat already above its maximum.
    case('cap.health-over', dict(type='Meat'), dict(health=480, maxhealth=500))
    case('cap.health-exact', dict(type='Bread'), dict(health=460, maxhealth=500))
    case('cap.mana-over', dict(type='Greater Mana'), dict(mana=1900, maxmana=2000))
    case('cap.above-max', dict(type='Apple'), dict(health=600, maxhealth=500, mana=10, maxmana=100))
    # The stack and where the item is held.
    case('stack.3', dict(type='Bread', amount=3))
    case('stack.2-potion', dict(type='Lesser Healing', amount=2))
    case('belt', dict(type='Lesser Healing', slot=BELT_FIRST))
    case('belt.food', dict(type='Cheese', slot=BELT_FIRST + 4))
    case('pouch', dict(type='Bread', where='pouch', slot=2))
    case('map.food', dict(type='Bread', where='map'))
    case('map.potion', dict(type='Lesser Mana', where='map'))
    # Refused: a dead user, an item used with another.
    case('dead', dict(type='Bread'), dict(health=0))
    case('dead.npc', dict(type='Bread'), dict(health=0), base=NPC, main='other')
    case('with', dict(type='Bread'), with_=12345)
    # Who hears it: a non-player user, a player who isn't the main one.
    case('npc', dict(type='Meat'), base=NPC, main='other')
    case('not-main.none', dict(type='Bread'), main='none')
    case('not-main.other', dict(type='Bread'), main='other')
    # The cure: the flag cleared and the scan round the user on their level.
    case('cure', dict(type='Cure Poison'), dict(poisoned=1, pos=[1000, 2000, 0], level=2))
    case('cure.not-poisoned', dict(type='Cure Poison'), dict(poisoned=0, pos=[-300, 77, 40], level=0))
    case('cure.npc', dict(type='Cure Poison'), dict(poisoned=1, pos=[3000, 4000, 0], level=1), base=NPC,
         main='other')
    for c in cases:
        c['sha256'] = _sha(c)
    return cases


def _fixture_char(user: dict, ident: int) -> dict:
    """A user as the port fixture's character (src/game/ab/retailab_fixture.h)."""
    spec = dict(name=user['name'], type=user['type'], id=ident, pos=user['pos'], facing=0, noai=True,
                stats=dict(health=user['health'], mana=user['mana'], fatigue=user['fatigue'],
                           poisoned=user['poisoned']),
                root=dict(name='walk', action=1), **{'class': user['class']})
    if user['class'] == PLAYER:
        spec.update(maxhealth=user['maxhealth'], maxmana=user['maxmana'], maxfatigue=user['maxfatigue'])
    else:
        spec['chardata'] = dict(health=user['maxhealth'], mana=user['maxmana'])
    return spec


def food_port_fields(case: dict) -> list[str]:
    body = {k: v for k, v in case.items() if k not in ('name', 'sha256', 'origin')}
    body['chars'] = [_fixture_char(case['user'], 1)]
    if case['main'] == 'other':
        body['chars'].append(_fixture_char(SOMEBODY, 2))
    return [json.dumps(body, separators=(',', ':'), ensure_ascii=True)]


def compare_food(case: dict, retail: dict, port: dict) -> list[dict]:
    def scans(side):
        return [{k: s[k] for k in ('rect', 'objset', 'level')} for s in side.get('scans', [])]

    diffs = []
    for key in ('returned', 'writes', 'sounds', 'text', 'item'):
        if retail.get(key) != port.get(key):
            diffs.append(dict(where='use', line=None, field=key, retail=retail.get(key), port=port.get(key)))
    if scans(retail) != scans(port):
        diffs.append(dict(where='use', line=None, field='scans', retail=scans(retail), port=scans(port)))
    return diffs


TARGETS = {
    'item-food-use': dict(fixture='slots/items/food_use.py', cases=food_cases, compare=compare_food,
                          port_fields=food_port_fields, unit=lambda r: 1),
}
