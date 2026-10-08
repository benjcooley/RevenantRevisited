"""Combat dojo targets for retail_ab.py (docs/gameplay/COMBAT_DOJO.md).

Each target: a case generator, the retail fixture in the emulator's combat
slot (tools/retail_runtime/slots/combat/), the port's
`Revenant --retail-ab=<target>`, and a compare. A combat case is one JSON
object -- a small world of characters and action blocks -- handed whole to
both sides (the port reads it from the case line's one field).

The compare is generic: every field of the two results, by path, except
the seams, which are aligned as sequences so a call one side makes and the
other doesn't reads as one inserted or missing record, not a shifted list.
"""
from __future__ import annotations

import difflib
import hashlib
import itertools
import json
import math
from pathlib import Path

IGNORED = {'schema', 'side', 'elapsed_ms'}


def _sha(case: dict) -> str:
    body = {k: v for k, v in case.items() if k not in ('name', 'sha256', 'origin')}
    return hashlib.sha256(json.dumps(body, sort_keys=True).encode()).hexdigest()


def finish(cases: list[dict]) -> list[dict]:
    for c in cases:
        c['sha256'] = _sha(c)
        c.setdefault('origin', 'combat_targets.py')
    return cases


def port_fields(case: dict) -> list[str]:
    body = {k: v for k, v in case.items() if k not in ('name', 'sha256', 'origin')}
    return [json.dumps(body, separators=(',', ':'), ensure_ascii=True)]


# ---- compare ------------------------------------------------------------------

def _walk(retail, port, path, out):
    if isinstance(retail, dict) and isinstance(port, dict):
        for key in sorted(set(retail) | set(port)):
            if key in IGNORED:
                continue
            sub = f'{path}.{key}' if path else key
            if key == 'seams':
                _seams(retail.get(key, []), port.get(key, []), out)
            elif key not in retail or key not in port:
                out.append(dict(where=sub, line=None, field=sub, retail=retail.get(key, '<absent>'),
                                port=port.get(key, '<absent>')))
            else:
                _walk(retail[key], port[key], sub, out)
    elif retail != port:
        out.append(dict(where=path, line=None, field=path, retail=retail, port=port))


def _seams(retail: list, port: list, out):
    a = [json.dumps(s, sort_keys=True) for s in retail]
    b = [json.dumps(s, sort_keys=True) for s in port]
    for tag, i1, i2, j1, j2 in difflib.SequenceMatcher(a=a, b=b, autojunk=False).get_opcodes():
        if tag == 'equal':
            continue
        out.append(dict(where=f'seams {tag} retail[{i1}:{i2}] port[{j1}:{j2}]', line=None,
                        field=f'seams.{tag}', retail=retail[i1:i2], port=port[j1:j2]))


def compare(case: dict, retail: dict, port: dict) -> list[dict]:
    out = []
    _walk(retail, port, '', out)
    return out


# ---- M3: Go(angle) ---------------------------------------------------------------

# Combat move states: the root, the eight steps, a transition into each.
STEPS = ['f', 'fr', 'r', 'br', 'b', 'bl', 'l', 'fl']
FULL_STATES = ['combat'] + [f'combat{s}' for s in STEPS] + [f'combat to combat{s}' for s in STEPS]
MINIMAL_STATES = ['combat', 'combatf', 'combat to combatf']


def _toward(origin, angle, dist):
    """Retail's facing convention: 64 is +x, 0 is -y (ConvertToVector)."""
    a = angle * 2 * math.pi / 256
    return [origin[0] + round(dist * math.sin(a)), origin[1] - round(dist * math.cos(a)), origin[2]]


def _char(name, objclass, pos, facing, root_obj=None, states=FULL_STATES, health=100, doing=None, **extra):
    spec = dict(name=name, type=extra.pop('type', name), **{'class': objclass}, pos=pos, facing=facing,
                moveangle=facing, stats=dict(health=health), classstats=dict(radius=extra.pop('radius', 16)),
                chardata=dict(combatrangemax=extra.pop('combatrangemax', 300)), states=states,
                root=dict(name='combat', action=3, angle=facing, moveangle=facing, obj=root_obj))
    if doing:
        spec['doing'] = doing
    spec.update(extra)
    return spec


def go_cases(data: Path, workdir: Path) -> list[dict]:
    """The orbit sweep: who moves (Locke, the player; an Araknid), CombatFace
    on/off, the facing, where the target is (none, or a bearing at 100),
    the direction held; then the variants that take other branches."""
    me_at = [1000, 1000, 0]
    cases = []
    for who, cf, facing, bearing, angle in itertools.product(
            ('player', 'monster'), (1, 0), (0, 64, 128, 200), (None, 0, 64, 128, 192),
            (0, 32, 64, 96, 128, 160, 192, 224, 40)):
        objclass, name, type_ = (11, 'Locke', 'Locke') if who == 'player' else (12, 'Araknid', 'Araknid')
        chars = [_char(name, objclass, me_at, facing, root_obj='Target' if bearing is not None else None,
                       type=type_)]
        if bearing is not None:
            tclass, ttype = (12, 'Araknid') if who == 'player' else (11, 'Locke')
            chars.append(_char('Target', tclass, _toward(me_at, bearing, 100), (bearing + 128) & 0xff,
                               type=ttype, states=['combat']))
        bname = 'none' if bearing is None else f'b{bearing}'
        cases.append(dict(name=f'go.{who}.cf{cf}.f{facing}.{bname}.a{angle}', call='go',
                          globals=dict(combatface=cf, frame=100), self=name, angle=angle, chars=chars))
    # Already stepping: doing is a combat move block heading 64, facing the target.
    for who, angle in itertools.product(('player', 'monster'), (64, 96, 0, 192)):
        objclass, name = (11, 'Locke') if who == 'player' else (12, 'Araknid')
        tclass, ttype = (12, 'Araknid') if who == 'player' else (11, 'Locke')
        doing = dict(name='combatr', action=4, angle=0, moveangle=64, turnrate=16, obj='Target')
        chars = [_char(name, objclass, me_at, 0, root_obj='Target', doing=doing),
                 _char('Target', tclass, _toward(me_at, 0, 100), 128, type=ttype, states=['combat'])]
        cases.append(dict(name=f'go.{who}.moving.a{angle}', call='go', globals=dict(combatface=1, frame=100),
                          self=name, angle=angle, chars=chars))
    # Fallbacks and gates.
    for who in ('player', 'monster'):
        objclass, name = (11, 'Locke') if who == 'player' else (12, 'Araknid')
        tclass, ttype = (12, 'Araknid') if who == 'player' else (11, 'Locke')
        target = _char('Target', tclass, _toward(me_at, 64, 100), 192, type=ttype, states=['combat'])
        for label, mine, extra in (
                ('minimal-states', dict(states=MINIMAL_STATES), {}),
                ('dead', dict(health=0), {}),
                ('blocked', {}, dict(blocked=True)),
                ('busy', dict(charflags=0x80000), {}),
                ('engaged', dict(engage=1), {})):
            for angle in (0, 128, 160):
                chars = [_char(name, objclass, me_at, 0, root_obj='Target', **mine), target]
                cases.append(dict(name=f'go.{who}.{label}.a{angle}', call='go', globals=dict(combatface=1, frame=100),
                                  self=name, angle=angle, chars=chars, **extra))
    # A monster whose target is the player while a script holds control
    # (IsValidTarget refuses the player unless control, or +0x5d8, is set).
    for flags, angle in itertools.product(((0, 0), (1, 0)), (0, 128)):
        chars = [_char('Araknid', 12, me_at, 0, root_obj='Target'),
                 _char('Target', 11, _toward(me_at, 64, 100), 192, type='Locke', states=['combat'])]
        cases.append(dict(name=f'go.monster.nocontrol.ps5d8_{flags[0]}.a{angle}', call='go',
                          globals=dict(combatface=1, frame=100, ps_5d8=flags[0], control=flags[1]),
                          self='Araknid', angle=angle, chars=chars))
    return finish(cases)


# ---- M5: ResolveCombat / ResolveCombatMove --------------------------------------

def resolve_cases(data: Path, workdir: Path) -> list[dict]:
    """One tick of the combat resolvers on the doing block.

    Standing in the combat root (ResolveCombat) or stepping (ResolveCombat-
    Move on a combat step block): who (player / monster), CombatFace, the
    facing, the target's bearing (none or one at 100), the step's move angle,
    the retarget cadence (the frame on or off the character's slot: player
    every 8 frames, monsters every 32), a pivot block (waitpivot), sight,
    the blocked bit and a stopped step."""
    me_at = [1000, 1000, 0]
    cases = []

    def world(who, facing, bearing, doing=None, **mine):
        objclass, name = (11, 'Locke') if who == 'player' else (12, 'Araknid')
        tclass, ttype = (12, 'Araknid') if who == 'player' else (11, 'Locke')
        chars = [_char(name, objclass, me_at, facing, root_obj='Target' if bearing is not None else None,
                       doing=doing, id=0x10, **mine)]
        if bearing is not None:
            chars.append(_char('Target', tclass, _toward(me_at, bearing, 100), (bearing + 128) & 0xff,
                               type=ttype, states=['combat'], id=0x20))
        return name, chars

    for who, cf, facing, bearing, tick in itertools.product(
            ('player', 'monster'), (1, 0), (0, 64, 128, 200), (None, 0, 64, 160), ('on', 'off')):
        frame = 0x10 if tick == 'on' else 0x11          # (frame ^ id) & mask == 0 on the slot
        bname = 'none' if bearing is None else f'b{bearing}'
        name, chars = world(who, facing, bearing)
        cases.append(dict(name=f'rc.{who}.cf{cf}.f{facing}.{bname}.{tick}', call='resolve-combat', bits=0,
                          globals=dict(combatface=cf, frame=frame), self=name, chars=chars))
        for step, move in (('combatf', 0), ('combatl', 192), ('combatr', 64), ('combatb', 128)):
            doing = dict(name=step, action=4, angle=facing, moveangle=move, turnrate=16,
                         obj='Target' if bearing is not None else None, flags=['noroot', 'interrupt'])
            name, chars = world(who, facing, bearing, doing=doing)
            cases.append(dict(name=f'rcm.{who}.cf{cf}.f{facing}.{bname}.{step}.{tick}', call='resolve-combat-move',
                              bits=0, globals=dict(combatface=cf, frame=frame), self=name, chars=chars))
    for who in ('player', 'monster'):
        # Pivoting before the first step: the root animation with waitpivot.
        for facing in (0, 32, 64):
            doing = dict(name='combat', action=4, angle=64, moveangle=64, turnrate=12, obj='Target',
                         flags=['noroot', 'waitpivot'])
            name, chars = world(who, facing, 64, doing=doing)
            cases.append(dict(name=f'rcm.{who}.pivot.f{facing}', call='resolve-combat-move', bits=0,
                              globals=dict(combatface=1, frame=0x11), self=name, chars=chars))
        # Blocked, stopped, unseen target.
        doing = dict(name='combatf', action=4, angle=64, moveangle=64, turnrate=16, obj='Target', flags=['noroot'])
        name, chars = world(who, 64, 64, doing=doing)
        cases.append(dict(name=f'rcm.{who}.blocked', call='resolve-combat-move', bits=2,
                          globals=dict(combatface=1, frame=0x11), self=name, chars=chars))
        stopped = dict(doing, flags=['noroot', 'stop'])
        name, chars = world(who, 64, 64, doing=stopped)
        cases.append(dict(name=f'rcm.{who}.stopped', call='resolve-combat-move', bits=0,
                          globals=dict(combatface=1, frame=0x11), self=name, chars=chars))
        for tick, frame in (('on', 0x10), ('off', 0x11)):
            name, chars = world(who, 0, 64)
            cases.append(dict(name=f'rc.{who}.unseen.{tick}', call='resolve-combat', bits=0, sees=False,
                              globals=dict(combatface=1, frame=frame), self=name, chars=chars))
    return finish(cases)


TARGETS = {
    'combat-go': dict(fixture='slots/combat/combat_call.py', cases=go_cases, compare=compare,
                      port_fields=port_fields, unit=lambda r: 1),
    'combat-resolve': dict(fixture='slots/combat/combat_call.py', cases=resolve_cases, compare=compare,
                           port_fields=port_fields, unit=lambda r: 1),
}
