#!/usr/bin/env python3
"""The story variables of a module: who writes them, who reads them, and where.

    storymap.py [--data DIR] [--module Ahkuilon] [--format md|json] [--out FILE]
                [--var NAME ...] [--spine MISTSTATE] [--no-positions]

Reads the module's own files: state.def (the variables and their starting
values), area.def (which script file loads on which level and rect), every
module script, and master.s from resources.rvr (the pack copy, which the game
reads first; the loose Resources/master.s only when the pack has none). For
every story variable it lists:

- the writers: each `SET <var> = <n>`, with its file:line, the OBJECT block,
  the trigger block it sits in (CUBE with its box, DIALOG, ALWAYS, USE,
  ACTIVATE, ...), the enclosing IF conditions that gate it, the nearest
  `:label` above it (a dialog choice's path), and where the object stands
  (level, position, area) on the levels whose areas load its script;
- the readers: each `IF <var> <op> <n>`, with the same context and a short
  summary of what the guarded block does (lines said, shop items offered,
  objects moved, shown or hidden, items given, other variables set);
- which script files read it, and whether a reader is in town.s (the house
  interiors, level 1), in forest.s at a Misthaven position (the town's
  streets, level 0) or in keep.s.

The Markdown also cross-references the writes from location triggers (CUBE,
PROXIMITY, ACTIVATE, USE) with their readers elsewhere, lists the town's
readers with the writes from outside the town that feed them, and tables the
spine variable (MISTSTATE) by value: which IF lines hold at each stage.

`SET` stores only into a variable state.def declares (COMMAND_SYSTEM.md §4,
`set`: an unknown state is ignored), and `IF` on an unknown one compares
retail's STATE_INVALID sentinel; both are reported. Names declared in a DATA
block are prototype variables, not story variables, and are left out.

Conditions too complex to render as `<var> <op> <n>` are carried as their
raw text. The walk is lexical: a SET under `:label` is credited to that
label, though a `jump` may reach it from elsewhere.

`--format json` is the machine-readable form for the engine-side coverage
check (ARCHITECTURE.md §9.4): every event carries an `id` (`file:line`),
and `coverage` lists the spine's writers in stage order and, for each
variable the town reads, the writes from outside the town and the town
readers. Docs: docs/shadow/STORY_TRIGGERS.md. Game text is Windows-1252.
"""
from __future__ import annotations

import argparse
import json
import os
import re
import sys
import zipfile
from collections import Counter
from dataclasses import asdict, dataclass, field
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[1]
sys.path.insert(0, str(REPO / 'tools' / 'storytest'))
sys.path.insert(0, str(REPO / 'tools' / 'playtest'))
import storytest  # noqa: E402  (ModuleData, parse_script, first_word, TRIGGERS)

TEXT = storytest.TEXT_ENCODING
LOCATION_TRIGGERS = ('CUBE', 'PROXIMITY', 'ACTIVATE', 'USE')
COMPARE = re.compile(r'^([A-Za-z_]\w*)\s*(<>|<=|>=|=|<|>)\s*(-?\d+)\s*$')
SET_LINE = re.compile(r'^(?:NOWAIT\s+)?SET\s+([A-Za-z_]\w*)\s*=?\s*(\S+)', re.I)
CUBE_HEAD = re.compile(r'^CUBE(?:\s+([A-Za-z_]\w*))?\s+(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s+'
                       r'(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)', re.I)
MAX_PLACES = 6                       # placements listed per object (type-matched doors run to hundreds)


# =====================================================================
# Sources
# =====================================================================

def master_script(data: Path) -> tuple[str, str] | None:
    """master.s as the game reads it: resources.rvr first, else the loose file."""
    pack = data / 'resources.rvr'
    if pack.exists():
        with zipfile.ZipFile(pack) as z:
            real = next((n for n in z.namelist() if n.lower() == 'master.s'), None)
            if real:
                return z.read(real).decode(TEXT), 'resources.rvr'
    loose = data / 'Resources' / 'master.s'
    if loose.exists():
        return loose.read_bytes().decode(TEXT), 'Resources/master.s'
    return None


def declared_states(text: str) -> tuple[dict[str, dict], list[dict]]:
    """state.def: NAME[=]value per line (TGameState::Load). Lookups take the
    first of a repeated name, so a repeat is listed apart."""
    states: dict[str, dict] = {}
    repeats = []
    for n, raw in enumerate(text.splitlines(), 1):
        m = re.match(r'\s*([A-Za-z_]\w*)\s*=?\s*(-?\d+)', raw.split('//', 1)[0])
        if not m:
            continue
        name = m.group(1).upper()
        if name in states:
            repeats.append({'name': name, 'line': n, 'first': states[name]['line']})
        else:
            states[name] = {'initial': int(m.group(2)), 'line': n}
    return states, repeats


@dataclass
class Area:
    name: str
    line: int
    level: int | None = None
    rect: tuple[int, int, int, int] | None = None     # left, top, right, bottom
    script: str = ''

    def holds(self, level: int, x: int, y: int) -> bool:
        if self.level is not None and level != self.level:
            return False
        if self.rect is None:
            return True
        left, top, right, bottom = self.rect
        return left <= x <= right and top <= y <= bottom


def parse_areas(text: str) -> list[Area]:
    """area.def's AREA blocks, in file order (TAreaManager: the last area
    holding the camera wins, so a RECT sub-area after its level overrides it)."""
    areas: list[Area] = []
    cur = None
    for n, raw in enumerate(text.splitlines(), 1):
        line = raw.split('//', 1)[0].strip()
        m = re.match(r'(?i)area\s+"([^"]*)"', line)
        if m:
            cur = Area(m.group(1).strip(), n)
            areas.append(cur)
            continue
        if cur is None:
            continue
        m = re.match(r'(?i)level\s+(\d+)', line)
        if m:
            cur.level = int(m.group(1))
        m = re.match(r'(?i)rect\s+(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)', line)
        if m:
            v = [int(g) for g in m.groups()]
            cur.rect = (min(v[0], v[2]), min(v[1], v[3]), max(v[0], v[2]), max(v[1], v[3]))
        m = re.match(r'(?i)script\s+"([^"]+)"', line)
        if m:
            cur.script = m.group(1).lower()
    return areas


def area_at(areas: list[Area], level: int, x: int, y: int) -> Area | None:
    winner = None
    for a in areas:
        if a.holds(level, x, y):
            winner = a
    return winner


# =====================================================================
# The walk over a trigger block
# =====================================================================

@dataclass
class Event:
    kind: str                # 'write' or 'read'
    var: str
    file: str
    line: int
    object: str
    trigger: str
    trigger_line: int
    header: str              # the trigger's header line, as written
    guards: list[str]        # enclosing IF conditions, outermost first
    label: str = ''          # nearest `:label` above the line in the block
    value: str = ''          # write: the value; read: the compared number
    op: str = ''             # read: the comparison
    condition: str = ''      # read: the IF's text
    effects: list[str] = field(default_factory=list)   # read: what its block does
    latches: list[str] = field(default_factory=list)   # read: guard variables its block sets
    poll: int | None = None  # ALWAYS: its WAIT before the first IF, in ticks
    end: int = 0             # read: the last line the IF guards
    wrapper: bool = False    # read: its block holds only another IF on the same variable (a range's first half)


def code_of(raw: str) -> str:
    return raw.split('//', 1)[0].strip()


def trigger_header(lines: list[str], begin: int) -> tuple[int, str]:
    """The trigger keyword's line above a block's BEGIN (1-based)."""
    for i in range(begin - 2, -1, -1):
        if storytest.first_word(lines[i]) in storytest.TRIGGERS:
            return i + 1, code_of(lines[i])
    return begin, ''


def effect_of(code: str) -> str | None:
    """A guarded line's story-relevant effect, in a few words (None: not one)."""
    n = ' '.join(code.split()).upper()
    if n.startswith('NOWAIT '):
        n = n[7:]
    head, _, rest = n.partition(' ')
    ctx, _, cmd = head.rpartition('.')
    who = ctx.lower() or 'this'
    if cmd == 'SET':
        m = SET_LINE.match(n)
        return f'set {m.group(1)}={m.group(2)}' if m else None
    if cmd == 'SAY':
        return 'say'
    if cmd == 'CHOICE':
        return 'choice'
    if cmd == 'BUYSELLADD':
        return 'shop item'
    if cmd == 'BUYSELLADDCRITERIA':
        return f'shop stock by {rest.lower()}'
    if cmd == 'POS':
        return f'{who} to ({rest.replace(" ", ",")})'
    if cmd == 'TOGGLE':
        m = re.match(r'(INVISIBLE|NOWALK|AI|PAUSE)\s*=?\s*(\S+)', rest)
        if m and m.group(1) == 'INVISIBLE':
            return f'{who} {"hidden" if m.group(2) in ("1", "ON", "TRUE") else "shown"}'
        if m and m.group(1) == 'NOWALK':
            return f'{who} nowalk={m.group(2)}'
        if m and m.group(1) == 'PAUSE' and m.group(2) in ('1', 'ON', 'TRUE'):
            return f'{who} script paused'
        return None
    if cmd == 'STAT':
        m = re.match(r'(LOCKED|AGGRESSIVE)\s*=?\s*(\S+)', rest)
        if m:
            return f'{who} {m.group(1).lower()}={m.group(2)}'
        return None
    if cmd in ('ADDINV', 'DELINV'):
        return f'{who} {"+" if cmd == "ADDINV" else "-"}{rest.strip(chr(34)).lower()}'
    if cmd == 'GIVE':
        return f'{who} give {rest.lower()}'
    if cmd == 'GIVEWEAPONS':
        return f'{who} weapons to {rest.strip(chr(34)).lower()}'
    if cmd == 'DELETE':
        return f'delete {(rest or ctx).lower()}'
    if cmd in ('PLAYMOVIE', 'ENDGAME'):
        return f'{cmd.lower()} {rest.lower()}'.strip()
    if cmd in ('FADECHARACTERIN', 'FADECHARACTEROUT'):
        return f'{rest.lower() or who} fades {"in" if cmd.endswith("IN") else "out"}'
    if cmd == 'ADDAT':
        return f'spawn {rest.split()[-1].lower()}' if rest else None
    return None


def summarize(effects: list[str]) -> list[str]:
    """Counts for the repeated kinds, the rest in order, each once."""
    counts = Counter(effects)
    out = []
    for kind, one, many in (('say', 'line said', 'lines said'), ('choice', 'choice', 'choices'),
                            ('shop item', 'shop item', 'shop items')):
        if counts[kind]:
            out.append(f'{counts[kind]} {one if counts[kind] == 1 else many}')
    seen = set()
    for e in effects:
        if e not in counts or e in ('say', 'choice', 'shop item') or e in seen:
            continue
        seen.add(e)
        out.append(e)
    return out


def leading_wait(block: storytest.Block) -> int | None:
    """An ALWAYS block's `WAIT <ticks>` before its first IF, at its own
    level: how long it sleeps, each time it starts, before it looks (24 ticks
    is one second). None: it looks at once."""
    depth = 0
    for raw in block.lines:
        code = code_of(raw)
        word = storytest.first_word(code)
        if word == 'BEGIN':
            depth += 1
        elif word == 'END':
            depth -= 1
        elif word == 'IF':
            return None
        elif depth == 1:
            m = re.match(r'WAIT\s+(\d+)$', code, re.I)
            if m:
                return int(m.group(1))
    return None


def walk_block(file: str, proto: storytest.Proto, block: storytest.Block, header_line: int,
               header: str, is_story) -> list[Event]:
    """Every SET and IF on a story variable in one trigger block, with the
    IF chain around it. An IF guards the next line, or the BEGIN block on
    it (SCRIPT_ENGINE.md §4.2); ELSE guards the next with the IF negated."""
    events: list[Event] = []
    frames: list[dict] = []          # one per open BEGIN: its guard, readers, statement count, first IF's variable
    pending: str | None = None       # an IF/ELSE condition waiting for its line
    pending_readers: list[Event] = []
    last_closed: dict[int, str] = {}  # depth -> the last IF condition closed there
    label = ''

    def guards() -> list[str]:
        return [f['guard'] for f in frames if f['guard']]

    def credit(effect: str | None) -> None:
        if effect:
            for f in frames:
                for r in f['readers']:
                    r.effects.append(effect)

    for k, raw in enumerate(block.lines):
        line = block.begin + k
        code = code_of(raw)
        if not code:
            continue
        if code.startswith(':'):
            label = code[1:].strip().split()[0] if code[1:].strip() else ''
            continue
        word = storytest.first_word(code)
        if word == 'BEGIN':
            frames.append({'guard': pending, 'readers': pending_readers, 'stmts': 0, 'first_if': None})
            pending, pending_readers = None, []
            continue
        if word == 'END':
            if frames:
                f = frames.pop()
                for r in f['readers']:
                    r.end = line
                    r.wrapper = f['stmts'] == 1 and f['first_if'] == r.var
                if f['guard']:
                    last_closed[len(frames)] = f['guard']
            continue
        line_guard, line_readers = pending, pending_readers
        pending, pending_readers = None, []
        outer = guards() + ([line_guard] if line_guard else [])
        if frames and not line_guard:
            frames[-1]['stmts'] += 1
        if word == 'IF':
            cond = code[2:].strip()
            pending = cond
            m = COMPARE.match(cond)
            if frames and frames[-1]['stmts'] == 1 and m:
                frames[-1]['first_if'] = m.group(1).upper()
            if m and is_story(m.group(1)):
                ev = Event('read', m.group(1).upper(), file, line, proto.name, block.trigger, header_line,
                           header, outer, label, m.group(3), m.group(2), cond)
                events.append(ev)
                pending_readers = [ev]
            continue
        if word == 'ELSE':
            prev = last_closed.get(len(frames))
            pending = f'not ({prev})' if prev else 'ELSE'
            continue
        m = SET_LINE.match(code)
        if m and is_story(m.group(1)):
            events.append(Event('write', m.group(1).upper(), file, line, proto.name, block.trigger,
                                header_line, header, outer, label, m.group(2)))
        effect = effect_of(code)
        credit(effect)
        if line_guard:               # a single guarded line: the IF closes here
            for r in line_readers:
                r.end = line
                if effect:
                    r.effects.append(effect)
            last_closed[len(frames)] = line_guard
    if block.trigger == 'ALWAYS':
        poll = leading_wait(block)
        for ev in events:
            ev.poll = poll
    return events


def finish_reads(events: list[Event]) -> None:
    """Summaries, and the guard variables a reader's block sets itself
    (a block that resets its own guard runs once)."""
    for ev in events:
        if ev.kind != 'read':
            continue
        sets = {e.split()[1].split('=')[0] for e in ev.effects if e.startswith('set ')}
        conds = ev.guards + [ev.condition]
        mine = {m.group(1).upper() for c in conds for m in [COMPARE.match(c)] if m}
        ev.latches = sorted(sets & mine)
        ev.effects = summarize(ev.effects)


# =====================================================================
# The map
# =====================================================================

def scan(data: Path, module: str) -> dict:
    """Every story variable's writers and readers, from the module's files."""
    mod = storytest.ModuleData(data, module)
    states, repeats = declared_states(mod.read('state.def').decode(TEXT))
    areas = parse_areas(mod.read('area.def').decode(TEXT))
    texts = dict(sorted(mod.scripts().items()))
    origins = {name: f'{module}.rvm' for name in texts}
    master = master_script(data)
    if master:
        texts['master.s'], origins['master.s'] = master

    # Names declared in a DATA block are prototype variables (the evaluator
    # looks them up on the object's prototypes); any other bare name in a
    # SET or an IF comparison is a story variable, declared or not.
    parsed = {name: storytest.parse_script(name, text) for name, text in texts.items()}
    protovars = {m.group(1).upper() for protos in parsed.values() for proto in protos
                 for b in proto.blocks if b.trigger == 'DATA'
                 for ln in b.lines for m in [re.match(r'\s*(?:NUMBER|TEXT)\s+(\w+)', ln, re.I)] if m}

    def is_story(name: str) -> bool:
        return name.upper() not in protovars

    events: list[Event] = []
    for name, protos in parsed.items():
        for proto in protos:
            for b in proto.blocks:
                if b.trigger == 'DATA':
                    continue
                hl, head = trigger_header(proto.lines, b.begin)
                events += walk_block(name, proto, b, hl, head, is_story)
    finish_reads(events)

    script_levels: dict[str, list[int]] = {}
    for a in areas:
        if a.script and a.level is not None:
            script_levels.setdefault(a.script, [])
            if a.level not in script_levels[a.script]:
                script_levels[a.script].append(a.level)
    sources = {name: {'origin': origins[name],
                      'levels': 'all' if name == 'master.s' else script_levels.get(name, []),
                      'areas': ['(every level: loaded at start)'] if name == 'master.s' else
                      [f'{a.name} (level {a.level}' + (f', rect {a.rect}' if a.rect else '') + ')'
                       for a in areas if a.script == name]}
               for name in texts}
    return {'states': states, 'repeats': repeats, 'areas': areas, 'sources': sources, 'events': events}


def cube_of(header: str) -> dict | None:
    m = CUBE_HEAD.match(header)
    if not m:
        return None
    v = [int(g) for g in m.groups()[1:]]
    return {'who': (m.group(1) or '').lower() or 'any mover but the owner',
            'x': sorted((v[0], v[3])), 'y': sorted((v[1], v[4])), 'z': sorted((v[2], v[5]))}


def placements(data: Path, module: str, sources: dict, areas: list[Area], objects: set[tuple[str, str]]) -> dict:
    """Where each script object stands: its instance name, else its type
    name (TScriptManager::ObjectScript), on the levels that load its file."""
    from world import World       # noqa: PLC0415  (tools/playtest; only with positions)
    world = World(data, None, module)
    out = {}
    for file, name in sorted(objects):
        levels = sources[file]['levels']
        levels = world.levels if levels == 'all' else levels
        found = [(p, 'instance') for lv in levels for p in world.objects(lv)
                 if p.name and p.name.lower() == name.lower()]
        if not found:
            found = [(p, 'type') for lv in levels for p in world.objects(lv)
                     if p.typename.lower() == name.lower()]
        places = []
        for p, by in found[:MAX_PLACES]:
            a = area_at(areas, p.level, p.pos[0], p.pos[1])
            places.append({'level': p.level, 'pos': list(p.pos), 'by': by,
                           'area': a.name if a else '', 'area_script': a.script if a else ''})
        out[f'{file}:{name.upper()}'] = {'count': len(found), 'places': places}
    return out


OPS = {'=': lambda a, b: a == b, '<>': lambda a, b: a != b, '<': lambda a, b: a < b,
       '>': lambda a, b: a > b, '<=': lambda a, b: a <= b, '>=': lambda a, b: a >= b}


def spine_branches(var: str, info: dict) -> dict:
    """For each value the spine variable takes (its start and every value
    written), the IF lines on it that hold, per OBJECT and trigger: the
    innermost ones, so a nested `IF MISTSTATE < 6` inside `IF MISTSTATE > 3`
    shows, not both. Conditions on anything else are taken as true."""
    values = sorted({info['initial']} | {int(w['value']) for w in info['writers']
                                         if re.fullmatch(r'-?\d+', w['value'])} - {None})

    def holds(r: dict, v: int) -> bool:
        for c in r['guards'] + [r['condition']]:
            m = COMPARE.match(c)
            if m and m.group(1).upper() == var and not OPS[m.group(2)](v, int(m.group(3))):
                return False
        return True

    by: dict[tuple, list[dict]] = {}
    for r in info['readers']:
        by.setdefault((r['file'], r['object'], r['trigger']), []).append(r)
    rows = []
    for (file, obj, trig), rs in sorted(by.items(), key=lambda kv: (kv[0][0], kv[0][1].lower(), kv[0][2])):
        cells = {}
        for v in values:
            live = [r for r in rs if not r['wrapper'] and holds(r, v)]
            cells[v] = [r['line'] for r in live
                        if not any(q is not r and r['line'] < q['line'] <= r['end'] for q in live)]
        rows.append({'file': file, 'object': obj, 'trigger': trig, 'branches': cells,
                     'town': any(r['town'] for r in rs)})
    return {'variable': var, 'values': values, 'rows': rows}


def coverage(variables: dict, spine: str) -> dict:
    """What the engine-side gate checks (ARCHITECTURE.md §9.4): the spine's
    writers, one per value, each a stage of the story, in value order; and
    for every variable the town reads, the writes made outside the town and
    the town readers that should run after one of them."""
    sp = variables.get(spine)
    spine_writers = sorted(({'id': w['id'], 'object': w['object'], 'trigger': w['trigger'], 'value': w['value']}
                            for w in (sp['writers'] if sp else [])), key=lambda w: int(w['value']))
    town = {}
    for name, v in variables.items():
        if not v['town_reader']:
            continue
        town[name] = {
            'feeds': [{'id': w['id'], 'object': w['object'], 'trigger': w['trigger'], 'value': w['value']}
                      for w in v['writers'] if not w['town']],
            'town_readers': [{'id': r['id'], 'object': r['object'], 'trigger': r['trigger'],
                              'condition': r['condition']} for r in v['readers'] if r['town']],
        }
    return {'spine': spine, 'spine_writers': spine_writers, 'town_variables': town}


def build(data: Path, module: str, positions: bool, only: set[str] | None, spine: str = 'MISTSTATE') -> dict:
    s = scan(data, module)
    states, events, areas, sources = s['states'], s['events'], s['areas'], s['sources']
    names = set(states) | {e.var for e in events}
    if only:
        names &= only
    keyed = {(e.file, e.object.upper()) for e in events if e.var in names}
    objs = placements(data, module, sources, areas, keyed) if positions else {}
    town_rect = next((a for a in areas if a.rect and a.name.lower() == 'misthaven'), None)

    def where(ev: Event) -> dict:
        place = objs.get(f'{ev.file}:{ev.object.upper()}', {})
        cube = cube_of(ev.header) if ev.trigger == 'CUBE' else None
        places = place.get('places', [])
        if cube and places:
            # The cube is tested only while its owner pulses: while the owner
            # is in the 3x3 sector window around the camera (STORY_TRIGGERS.md §1.4).
            cx, cy = sum(cube['x']) / 2, sum(cube['y']) / 2
            cube['owner_distance'] = round(min(((p['pos'][0] - cx) ** 2 + (p['pos'][1] - cy) ** 2) ** 0.5
                                               for p in places))
        return {'cube': cube, 'object_places': places, 'object_count': place.get('count')}

    def in_town(ev: Event, w: dict) -> bool:
        if ev.file == 'town.s':
            return True
        if ev.file == 'forest.s' and town_rect:
            return any(p['level'] == 0 and town_rect.holds(0, p['pos'][0], p['pos'][1])
                       for p in w['object_places'])
        return False

    variables = {}
    for name in sorted(names):
        rows = {'writers': [], 'readers': []}
        for ev in events:
            if ev.var != name:
                continue
            d = asdict(ev)
            d['id'] = f'{ev.file}:{ev.line}'
            d.update(where(ev))
            d['town'] = in_town(ev, d)
            rows['writers' if ev.kind == 'write' else 'readers'].append(d)
        decl = states.get(name)
        read_in = sorted({r['file'] for r in rows['readers']})
        variables[name] = {
            'declared': decl is not None,
            'initial': decl['initial'] if decl else None,
            'decl_line': decl['line'] if decl else None,
            'writers': rows['writers'],
            'readers': rows['readers'],
            'read_in': read_in,
            'written_in': sorted({w['file'] for w in rows['writers']}),
            'town_reader': any(r['town'] for r in rows['readers']),
            'keep_reader': 'keep.s' in read_in,
        }
    spine = spine.upper()
    return {'module': module,
            'spine': spine_branches(spine, variables[spine]) if spine in variables else None,
            'coverage': coverage(variables, spine),
            'sources': sources,
            'areas': [asdict(a) for a in areas],
            'state_def_repeats': s['repeats'],
            'variables': variables,
            'objects': objs}


# =====================================================================
# Markdown
# =====================================================================

FAR = 1024                           # a cube this far from its owner fires only while the owner is in the window


def box(c: dict | None) -> str:
    if not c:
        return ''
    far = c.get('owner_distance') or 0
    return (f'cube {c["x"][0]}-{c["x"][1]}, {c["y"][0]}-{c["y"][1]}, z {c["z"][0]}-{c["z"][1]}'
            f' ({c["who"]})' + (f', owner {far} away' if far > FAR else ''))


def place_text(row: dict) -> str:
    places = row.get('object_places') or []
    if row.get('object_count') == 0:
        txt = 'no object found on its levels'
    elif not places:
        txt = ''
    else:
        p = places[0]
        txt = f'L{p["level"]} ({p["pos"][0]}, {p["pos"][1]}, {p["pos"][2]})'
        if p['area']:
            txt += f' {p["area"]}'
        more = (row.get('object_count') or 1) - 1
        if more > 0:
            txt += f' +{more} more' + (' (by type)' if p['by'] == 'type' else '')
    cube = box(row.get('cube'))
    return '; '.join(t for t in (cube, txt) if t)


def line_list(lines: list[int]) -> str:
    return ','.join(map(str, lines)) or 'none'


def esc(s: str) -> str:
    return s.replace('|', '\\|')


def gate(row: dict) -> str:
    g = ' and '.join(row['guards'])
    if row.get('label'):
        g = (g + ' ' if g else '') + f'[:{row["label"]}]'
    return esc(g) or '-'


def trig(row: dict) -> str:
    """The trigger, with an ALWAYS block's sleep before it looks."""
    return row['trigger'] + (f' (waits {row["poll"]} first)' if row.get('poll') else '')


def grouped(writers: list[dict]) -> list[tuple[str, dict]]:
    """Writes that differ only in their line (or a USE's item), as ('l1,l2', first write)."""
    out: dict[tuple, list] = {}
    for w in writers:
        key = (w['file'], w['object'].upper(), w['trigger'], tuple(w['guards']), w['value'])
        out.setdefault(key, []).append(w)
    return [(','.join(str(w['line']) for w in ws), ws[0]) for ws in out.values()]


def readers_by_object(readers) -> str:
    """'town.s: Gus2 (6), GUS2DUMMY (3); forest.s: ...'."""
    by: dict[str, Counter] = {}
    for r in readers:
        by.setdefault(r['file'], Counter())[r['object']] += 1
    return '; '.join(f'{f}: ' + ', '.join(f'{o} ({n})' for o, n in sorted(c.items(), key=lambda x: x[0].lower()))
                     for f, c in sorted(by.items()))


def markdown(m: dict) -> str:
    out = [f'# Story variables: {m["module"]}', '',
           'Generated by `tools/shadow/storymap.py`. A citation is `file:line` in the module\'s '
           'own scripts (master.s from resources.rvr).', '',
           '## Script files and where they load', '',
           '| File | From | Levels | Areas (area.def) |', '|---|---|---|---|']
    for name, s in m['sources'].items():
        lv = s['levels'] if s['levels'] == 'all' else ', '.join(map(str, s['levels'])) or '(none)'
        out.append(f'| {name} | {s["origin"]} | {lv} | {"; ".join(s["areas"]) or "(no area)"} |')
    if m['state_def_repeats']:
        out += ['', 'state.def repeats (lookups take the first): ' +
                ', '.join(f'{r["name"]} (lines {r["first"]}, {r["line"]})' for r in m['state_def_repeats'])]

    vs = m['variables']
    out += ['', '## Summary', '',
            '| Variable | Start | Writers (by trigger) | Readers | Read in | Town reader |',
            '|---|---|---|---|---|---|']
    for name, v in vs.items():
        kinds = Counter(w['trigger'] for w in v['writers'])
        wtxt = ', '.join(f'{k} {n}' for k, n in sorted(kinds.items())) or '**none**'
        start = v['initial'] if v['declared'] else '**undeclared**'
        out.append(f'| {name} | {start} | {wtxt} | {len(v["readers"])} | {", ".join(v["read_in"]) or "-"} '
                   f'| {"yes" if v["town_reader"] else ""} |')

    out += ['', '## Location writes and their readers', '',
            'Writes from CUBE, PROXIMITY, ACTIVATE and USE blocks, with the readers in other OBJECT '
            'blocks (file: object, number of IF lines). Writes that differ only in line are one row.', '',
            '| Variable | Write | Object / trigger | Where | Gate | Value | Read by (other objects) |',
            '|---|---|---|---|---|---|---|']
    for name, v in vs.items():
        for lines, w in grouped([w for w in v['writers'] if w['trigger'] in LOCATION_TRIGGERS]):
            others = readers_by_object(r for r in v['readers'] if r['object'].upper() != w['object'].upper())
            out.append(f'| {name} | {w["file"]}:{lines} | {w["object"]} {w["trigger"]} '
                       f'| {esc(place_text(w))} | {gate(w)} | {w["value"]} | {others or "-"} |')

    out += ['', '## Town readers and the writes that feed them', '',
            'Variables read in town.s (house interiors, level 1) or by a forest.s object standing in '
            'Misthaven, with every write made outside those places.', '',
            '| Variable | Town readers | Writes from elsewhere |', '|---|---|---|']
    for name, v in vs.items():
        if not v['town_reader']:
            continue
        readers = readers_by_object(r for r in v['readers'] if r['town'])
        feeds = [f'{w["file"]}:{lines} {w["object"]} {w["trigger"]} = {w["value"]}'
                 for lines, w in grouped([w for w in v['writers'] if not w['town']])]
        out.append(f'| {name} | {readers} | {"<br>".join(feeds) or "none (written only in town)"} |')

    sp = m.get('spine')
    if sp:
        vals = sp['values']
        out += ['', f'## {sp["variable"]} by value', '',
                f'Which `IF {sp["variable"]}` lines hold at each value, per object (the innermost ones; '
                'conditions on other things are taken as true). A row whose cells change between two values '
                'is a script the stage change turns on or off.', '',
                '| Object | Trigger | ' + ' | '.join(str(v) for v in vals) + ' |',
                '|---|---|' + '---|' * len(vals)]
        for row in sp['rows']:
            cells = [line_list(row['branches'][v]) for v in vals]
            town = ' (town)' if row['town'] else ''
            out.append(f'| {row["file"]}: {row["object"]}{town} | {row["trigger"]} | ' + ' | '.join(cells) + ' |')
        out += ['', f'What each step of {sp["variable"]} changes (the rows above whose lines differ):', '',
                '| Step | In town | Elsewhere |', '|---|---|---|']
        for a, b in zip(vals, vals[1:]):
            moved = {True: [], False: []}
            for row in sp['rows']:
                before, after = row['branches'][a], row['branches'][b]
                if before != after:
                    moved[row['town']].append(f'{row["object"]} {row["trigger"]} {line_list(before)} → '
                                              f'{line_list(after)}')
            out.append(f'| {a} → {b} | {"; ".join(moved[True]) or "-"} | {"; ".join(moved[False]) or "-"} |')

    for name, v in vs.items():
        out += ['', f'## {name}', '']
        if v['declared']:
            out.append(f'state.def:{v["decl_line"]}, starts at {v["initial"]}.')
        else:
            out.append('**Not in state.def**: `SET` ignores it and `IF` compares STATE_INVALID.')
        out.append(f'Written in {", ".join(v["written_in"]) or "no file"}; read in '
                   f'{", ".join(v["read_in"]) or "no file"}.')
        if v['writers']:
            out += ['', '| Write | Object | Trigger | Where | Gate [:label] | Value |', '|---|---|---|---|---|---|']
            for w in v['writers']:
                out.append(f'| {w["file"]}:{w["line"]} | {w["object"]} | {trig(w)} | {esc(place_text(w))} '
                           f'| {gate(w)} | {w["value"]} |')
        if v['readers']:
            out += ['', '| Read | Object | Trigger | Condition | Outer gate | Its block does | Town |',
                    '|---|---|---|---|---|---|---|']
            for r in v['readers']:
                does = esc(', '.join(r['effects'])) or '-'
                if r['latches']:
                    does += f' (sets its own guard: {", ".join(r["latches"])})'
                outer = esc(' and '.join(r['guards'])) or '-'
                out.append(f'| {r["file"]}:{r["line"]} | {r["object"]} | {trig(r)} | `{esc(r["condition"])}` '
                           f'| {outer} | {does} | {"yes" if r["town"] else ""} |')
    return '\n'.join(out) + '\n'


# =====================================================================
# CLI
# =====================================================================

def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--data', type=Path, default=Path(os.environ.get('REVENANT_DATA_PATH') or REPO / 'data'),
                    help='the install: Modules/, resources.rvr, imagery.rvi (default $REVENANT_DATA_PATH, else ./data)')
    ap.add_argument('--module', default='Ahkuilon')
    ap.add_argument('--format', choices=('md', 'json'), default='md')
    ap.add_argument('--out', type=Path, help='write here instead of stdout')
    ap.add_argument('--var', action='append', help='only this variable (repeatable)')
    ap.add_argument('--spine', default='MISTSTATE', help='the variable tabled by value (default MISTSTATE)')
    ap.add_argument('--no-positions', action='store_true',
                    help="skip the sector scan (object positions need imagery.rvi's class.def)")
    a = ap.parse_args(argv)
    only = {v.upper() for v in a.var} if a.var else None
    m = build(a.data, a.module, not a.no_positions, only, a.spine)
    text = json.dumps(m, indent=1) if a.format == 'json' else markdown(m)
    if a.out:
        a.out.write_text(text, encoding='utf-8')
    else:
        sys.stdout.write(text)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
