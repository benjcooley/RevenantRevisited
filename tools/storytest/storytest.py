#!/usr/bin/env python3
"""Talk to every scripted NPC headless and report where the port breaks.

    storytest.py [--slot DIR] locate [--scripts forest.s,town.s] [--levels 0,1] [--json FILE]
    storytest.py --slot DIR run --out DIR --ini FILE [--npc NAME ...] [--jobs N]
                 [--keys 3,1 | --choice K] [--set VAR=N ...] [--window S] [--tag TAG]
    storytest.py [--slot DIR] report [--reanalyze] [--retail] DIR [DIR ...]

`locate` reads the module's scripts and maps: every OBJECT block with a
DIALOG trigger, matched to the map object it attaches to (instance name, else
type name, as TScriptManager::ObjectScript does) on the levels whose areas
load that script (area.def). Sector files are the slot's working set
(`<slot>/CurMap`) where it has one, else the module's base map.

`run` starts one game per NPC from the save slot, teleports Locke beside the
NPC, `use`s it, and presses choice keys over the conversation. Each run gets
its own directory under DIR (save path, revenant.log, run.json, result.json)
and its own copy of the binary. A run stops a few seconds after the DIALOG
block ends, when the block goes quiet for too long, or at the window's end.

`report` gathers the result.json files into a table: per NPC, whether the
block ran to its end, the choices taken, and every console error, script
error, ERROR/FATAL line, early end, hang and crash, with the script line
behind it.

Docs: docs/gameflow/STORY_TESTING.md §7. Environment: REVENANT_DATA_PATH (the
install: Modules/, imagery.rvi). Game text is Windows-1252.
"""

from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import signal
import subprocess
import sys
import threading
import time
import zipfile
from concurrent.futures import ThreadPoolExecutor
from dataclasses import asdict, dataclass, field
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parent.parent
sys.path.insert(0, str(REPO / 'tools' / 'savefmt'))
import revsave  # noqa: E402  (sector decoder)

TEXT_ENCODING = 'cp1252'
# Block keywords inside an OBJECT block: the triggers, and DATA (variables).
TRIGGERS = {'ALWAYS', 'TRIGGER', 'DIALOG', 'PROXIMITY', 'CUBE', 'ACTIVATE',
            'USE', 'GIVE', 'GET', 'COMBAT', 'DEAD', 'DATA'}
TRIGGER_DIALOG = 3


# =====================================================================
# Module data: scripts, areas, sectors, types
# =====================================================================

class ModuleData:
    """The installed module (zip archive) plus the imagery pack's class.def."""

    def __init__(self, data_path: Path, module: str = 'Ahkuilon'):
        self.data_path = data_path
        self.archive = zipfile.ZipFile(data_path / 'Modules' / f'{module}.rvm')
        self.names = {n.lower(): n for n in self.archive.namelist()}
        self._types = None

    def read(self, name: str) -> bytes | None:
        real = self.names.get(name.lower())
        return self.archive.read(real) if real else None

    def scripts(self) -> dict[str, str]:
        out = {}
        for low, real in self.names.items():
            if low.endswith('.s') and '/' not in low:
                out[low] = self.archive.read(real).decode(TEXT_ENCODING)
        return out

    def script_levels(self) -> dict[str, list[int]]:
        """area.def: script file -> the levels whose areas load it."""
        text = self.read('area.def').decode(TEXT_ENCODING)
        levels: dict[str, list[int]] = {}
        level = script = None
        for raw in text.splitlines():
            line = raw.split('//', 1)[0].strip()
            if not line:
                continue
            m = re.match(r'(?i)area\b', line)
            if m:
                level = script = None
                continue
            m = re.match(r'(?i)level\s+(\d+)', line)
            if m:
                level = int(m.group(1))
            m = re.match(r'(?i)script\s+"([^"]+)"', line)
            if m:
                script = m.group(1).lower()
            if re.match(r'(?i)end$', line) and level is not None and script:
                levels.setdefault(script, [])
                if level not in levels[script]:
                    levels[script].append(level)
        return levels

    def sector_names(self, level: int) -> list[str]:
        out = []
        for low, real in self.names.items():
            m = re.match(r'map/(\d+)_(\d+)_(\d+)\.dat$', low)
            if m and int(m.group(1)) == level:
                out.append(real)
        return out

    def types(self) -> dict[tuple[int, int], str]:
        """class.def: (objclass, type unique id) -> type name."""
        if self._types is None:
            self._types = {}
            pack = zipfile.ZipFile(self.data_path / 'imagery.rvi')
            text = pack.read('class.def').decode(TEXT_ENCODING)
            byname = {v: k for k, v in revsave.OBJCLASS_NAMES.items()}
            objclass = None
            for raw in text.splitlines():
                m = re.match(r'\s*CLASS\s+"([^"]+)"', raw)
                if m:
                    objclass = byname.get(m.group(1).upper())
                    continue
                m = re.match(r'\s*"([^"]*)"\s+"[^"]*"\s+(0x[0-9a-fA-F]+)', raw)
                if m and objclass is not None:
                    self._types[(objclass, int(m.group(2), 16))] = m.group(1).strip()
        return self._types


@dataclass
class MapObject:
    level: int
    sector: str
    name: str            # instance name, '' when the object uses its type's
    typename: str
    objclass: str
    x: int
    y: int
    z: int
    source: str          # 'save' or 'base'


def read_sector(data: bytes) -> list[dict]:
    """Top-level objects of one sector file: name, class, type id, position."""
    d = revsave.Decoder(data)
    try:
        d.sector()
    except revsave.Truncated:
        pass
    objs: dict[int, dict] = {}
    for f in d.fields:
        m = re.match(r'obj\[(\d+)\]\.(name|pos\.[xyz]|objclass|uniqueid)$', f.path)
        if m:
            objs.setdefault(int(m.group(1)), {})[m.group(2)] = f.value
    return [o for _, o in sorted(objs.items()) if 'objclass' in o and 'pos.x' in o]


def scan_level(mod: ModuleData, level: int, slot: Path | None) -> list[MapObject]:
    """Every object on a level: the slot's working-set sector when it has one,
    else the base map's (the game's own order, TSector::Load)."""
    files: dict[str, tuple[str, bytes]] = {}
    for real in mod.sector_names(level):
        files[Path(real).name.upper()] = ('base', mod.archive.read(real))
    if slot:
        curmap = next((p for p in (slot / 'CurMap', slot / 'curmap') if p.is_dir()), None)
        if curmap:
            for p in curmap.iterdir():
                m = re.match(r'(\d+)_\d+_\d+\.DAT$', p.name, re.I)
                if m and int(m.group(1)) == level:
                    files[p.name.upper()] = ('save', p.read_bytes())
    types = mod.types()
    out = []
    for sector, (source, data) in sorted(files.items()):
        for o in read_sector(data):
            name = o.get('name', '')
            if name == '<type name>':
                name = ''
            uid = int(o['uniqueid'], 16) if isinstance(o.get('uniqueid'), str) else 0
            out.append(MapObject(level, sector[:-4], name,
                                 types.get((o['objclass'], uid), ''),
                                 revsave.OBJCLASS_NAMES.get(o['objclass'], str(o['objclass'])),
                                 o['pos.x'], o['pos.y'], o['pos.z'], source))
    return out


def module_names(mod: ModuleData, slot: Path | None) -> set[str]:
    """Every name an object answers to (instance, else type), on every level."""
    levels = sorted({int(m.group(1)) for n in mod.names for m in [re.match(r'map/(\d+)_', n)] if m})
    names = set()
    for lv in levels:
        for o in scan_level(mod, lv, slot):
            names.add((o.name or o.typename).upper())
    return names


def names_file(out: Path, mod: ModuleData | None, slot: Path | None) -> set[str] | None:
    """The sweep's name list (`<out>/names.json`), built once."""
    f = out / 'names.json'
    if not f.exists():
        if mod is None:
            return None
        f.write_text(json.dumps(sorted(module_names(mod, slot))))
    return set(json.loads(f.read_text()))


# =====================================================================
# Scripts
# =====================================================================

@dataclass
class Block:
    trigger: str
    begin: int           # line of the block's BEGIN (1-based)
    end: int             # line of its END
    lines: list[str] = field(default_factory=list)   # file lines begin..end


@dataclass
class Proto:
    name: str
    script: str
    line: int
    blocks: list[Block] = field(default_factory=list)
    labels: set[str] = field(default_factory=set)   # every `:label` in the OBJECT block
    end: int = 0                                    # line of the object's END
    lines: list[str] = field(default_factory=list)  # the file's lines

    def dialog(self) -> Block | None:
        return next((b for b in self.blocks if b.trigger == 'DIALOG'), None)


def first_word(line: str) -> str:
    s = line.split('//', 1)[0].strip()
    return s.split(None, 1)[0].upper() if s else ''


def parse_script(script: str, text: str) -> list[Proto]:
    """OBJECT blocks and their trigger blocks (TScriptProto::ParseScript's
    shape: OBJECT "<name>" BEGIN { <trigger> ... BEGIN ... END } END)."""
    lines = text.splitlines()
    protos: list[Proto] = []
    i = 0
    while i < len(lines):
        m = re.match(r'\s*OBJECT\s+"?([^"\s]+)"?', lines[i], re.I)
        if not m:
            i += 1
            continue
        proto = Proto(m.group(1), script, i + 1, lines=lines)
        protos.append(proto)
        depth = 0
        trigger = None
        bstart = None
        i += 1
        while i < len(lines):
            w = first_word(lines[i])
            lm = re.match(r'\s*:\s*(\w+)', lines[i])
            if lm:
                proto.labels.add(lm.group(1).upper())
            if w == 'BEGIN':
                depth += 1
                if depth == 2:
                    bstart = i
            elif w == 'END':
                depth -= 1
                if depth == 1 and trigger and bstart is not None:
                    proto.blocks.append(Block(trigger, bstart + 1, i + 1, lines[bstart:i + 1]))
                    trigger = None
                if depth == 0:
                    proto.end = i + 1
                    break
            elif depth == 1 and w in TRIGGERS:
                trigger = w
            i += 1
        i += 1
    return protos


def norm(line: str) -> str:
    return ' '.join(line.split('//', 1)[0].split()).upper()


def block_facts(block: Block, labels: set[str]) -> dict:
    """What a run can be checked against: choice labels, waits, game states,
    and the jump/choice labels the OBJECT block lacks (TScript::Jump looks
    through the whole prototype)."""
    choices, says, states = [], 0, set()
    responses = 0
    missing = []
    for raw in block.lines:
        n = norm(raw)
        w = n.split(' ', 1)[0] if n else ''
        if w == 'NOWAIT':
            n = n[7:]
            w = n.split(' ', 1)[0]
        cmd = w.split('.')[-1]
        if cmd in ('CHOICE', 'JUMP'):
            parts = n.split()
            if len(parts) > 1:
                if cmd == 'CHOICE':
                    choices.append(parts[1])
                if parts[1] not in labels:
                    missing.append(parts[1])
        elif cmd == 'SAY':
            says += 1
        elif n.startswith('WAIT RESP'):
            responses += 1
        for m in re.finditer(r'\b(?:IF|SET)\s+([A-Z][A-Z0-9_]*)\s*(?:=|<|>|<>)', n):
            states.add(m.group(1))
    return {'choices': choices, 'says': says, 'responses': responses,
            'states': sorted(s for s in states if not s.startswith('PLAYER')),
            'missing_labels': sorted(set(missing))}


# =====================================================================
# Locate
# =====================================================================

@dataclass
class Npc:
    name: str
    script: str
    proto_line: int
    dialog_begin: int
    dialog_end: int
    levels: list[int]
    matches: list[dict]
    facts: dict
    dialog_lines: list[str]
    # After a jump the block runs on through the triggers after it to the
    # object's END (TScript::Jump counts the object's BEGIN; SCRIPT_ENGINE.md
    # §4.2): the lines from the DIALOG block's BEGIN to there.
    object_end: int = 0
    run_lines: list[str] = field(default_factory=list)

    @property
    def found(self) -> bool:
        return bool(self.matches)


def locate(mod: ModuleData, slot: Path | None, scripts: list[str] | None,
           levels_filter: set[int] | None) -> list[Npc]:
    texts = mod.scripts()
    script_levels = mod.script_levels()
    cache: dict[int, list[MapObject]] = {}
    npcs = []
    for script in sorted(texts):
        if scripts and script not in scripts:
            continue
        levels = script_levels.get(script, [])
        if levels_filter:
            levels = [lv for lv in levels if lv in levels_filter]
            if not levels:
                continue
        for proto in parse_script(script, texts[script]):
            block = proto.dialog()
            if not block:
                continue
            want = proto.name.upper()
            matches = []
            for lv in levels:
                if lv not in cache:
                    cache[lv] = scan_level(mod, lv, slot)
                for o in cache[lv]:
                    by = None
                    if o.name and o.name.upper() == want:
                        by = 'instance'
                    elif not o.name and o.typename.upper() == want:
                        by = 'type'
                    if by:
                        matches.append(dict(asdict(o), by=by))
            npcs.append(Npc(proto.name, script, proto.line, block.begin, block.end,
                            levels, matches, block_facts(block, proto.labels), block.lines,
                            proto.end, proto.lines[block.begin - 1:proto.end]))
    return npcs


def print_locate(npcs: list[Npc]) -> None:
    print(f'{"npc":<14} {"script":<12} {"dialog":>11}  {"lvl":>3} {"x":>6} {"y":>6} {"z":>4}  '
          f'{"class":<10} {"by":<8} {"src":<4} sector     choices says states')
    for n in npcs:
        f = n.facts
        tail = f'{len(f["choices"]):>7} {f["says"]:>4} {",".join(f["states"])}'
        if f['missing_labels']:
            tail += f'  NO LABEL: {",".join(f["missing_labels"])}'
        if not n.matches:
            print(f'{n.name:<14} {n.script:<12} {n.dialog_begin:>5}-{n.dialog_end:<5}  '
                  f'{"-":>3} {"(no object on levels " + ",".join(map(str, n.levels)) + ")":<46}{tail}')
            continue
        for k, m in enumerate(n.matches):
            head = (f'{n.name:<14} {n.script:<12} {n.dialog_begin:>5}-{n.dialog_end:<5}' if k == 0
                    else f'{"":<14} {"":<12} {"":>11}')
            print(f'{head}  {m["level"]:>3} {m["x"]:>6} {m["y"]:>6} {m["z"]:>4}  {m["objclass"]:<10} '
                  f'{m["by"]:<8} {m["source"]:<4} {m["sector"]:<10} {tail if k == 0 else ""}')


# =====================================================================
# Run
# =====================================================================

@dataclass
class RunSpec:
    npc: Npc
    tag: str
    keys: str
    sets: list[str]
    out: Path
    exec_line: str = ''
    input_script: str = ''
    window_s: int = 0


def parse_keys(spec: str) -> list[tuple[list[str], int | None]]:
    """`--keys`: phases separated by '/', each a comma list of keys pressed in
    turn every cycle, with an optional cycle count: "2x6/3" presses 2 for six
    cycles, then 3 to the end; "3,1" presses 3 then 1 every cycle."""
    phases = []
    for part in spec.split('/'):
        m = re.match(r'^(.*?)(?:x(\d+))?$', part.strip())
        phases.append(([k.strip() for k in m.group(1).split(',') if k.strip()],
                       int(m.group(2)) if m.group(2) else None))
    return phases


def plan(npc: Npc, keys: str, sets: list[str], tag: str, out: Path,
         offset: tuple[int, int], window: int | None = None) -> RunSpec:
    """One run: Locke placed beside the NPC, the states set, `use`; then every
    cycle of the window presses the phase's keys in turn (a number key picks a
    choice when a menu is up and is ignored otherwise; `e` closes a shop)."""
    m = npc.matches[0]
    x, y = m['x'] + offset[0], m['y'] + offset[1]
    cmds = ['sleep 48', f'player.pos {x} {y} {m["z"]} {m["level"]}', 'sleep 120']
    cmds += [s if " " in s.strip() else f'set {s.replace("=", " = ")}' for s in sets]
    cmds += [f'use {npc.name}']
    # The window: the conversation's lines at a few seconds each, plus the
    # start-up and teleport; the run stops early once the block ends.
    window = window or min(600, 60 + 5 * npc.facts['says'] + 20 * npc.facts['responses'])
    phases = parse_keys(keys)
    events = []
    t, gap, step = 12000, 4000, 400
    phase, cycles = 0, 0
    while t < window * 1000:
        cycle, count = phases[phase]
        for k, key in enumerate(cycle):
            wait = (t if not events else gap) if k == 0 else step
            events.append(f'wait {wait}; key_press {key}')
        t += gap + step * (len(cycle) - 1)
        cycles += 1
        if count is not None and cycles >= count and phase + 1 < len(phases):
            phase, cycles = phase + 1, 0
    events.append('wait 2000; take_snapshot')
    name = f'{npc.script[:-2]}_{npc.name}' + (f'_{tag}' if tag else '')
    return RunSpec(npc, tag, keys, sets, out / name, '; '.join(cmds), '; '.join(events), window)


def prepare(spec: RunSpec, binary: Path, slot: Path, ini: Path) -> Path:
    if spec.out.exists():
        shutil.rmtree(spec.out)
    save = spec.out / 'save'
    (save / 'Save' / 'Single').mkdir(parents=True)
    shutil.copy(ini, save / 'Revenant.ini')
    shutil.copytree(slot, save / 'Save' / 'Single' / slot.name)
    # The engine assets are found from the binary: the copy lives beside it.
    copy = binary.parent / f'{binary.name}.st_{spec.out.name}'
    shutil.copy2(binary, copy)
    return copy


def log_lines(data: bytes) -> list[str]:
    """revenant.log's lines: the dialog and text bar lines are UTF-8, a traced
    script line is the script's own Windows-1252 text."""
    out = []
    for raw in data.split(b'\n'):
        try:
            out.append(raw.decode('utf-8').rstrip('\r'))
        except UnicodeDecodeError:
            out.append(raw.decode(TEXT_ENCODING, 'replace').rstrip('\r'))
    return out


LOG_LINE = re.compile(r'^(?:\d{4}-\d\d-\d\d )?\d\d:\d\d:\d\d (TRACE|DEBUG|INFO|WARN|ERROR|FATAL)\s+\S+: (.*)$')


def run_one(spec: RunSpec, binary: Path, slot: Path, ini: Path, data: Path,
            grace_s: float, idle_s: float, start_s: float) -> dict:
    exe = prepare(spec, binary, slot, ini)
    env = dict(os.environ, REVENANT_SAVE_PATH=str(spec.out / 'save'), REVENANT_DATA_PATH=str(data))
    args = [str(exe), '--headless', f'--max-runtime={spec.window_s + 60}',
            f'--quickstart={slot.name}', f'--exec={spec.exec_line}',
            f'--input-script={spec.input_script}']
    (spec.out / 'cmd.txt').write_text(' '.join(repr(a) for a in args) + '\n')
    log = spec.out / 'revenant.log'
    begin = time.time()
    with open(spec.out / 'stdout.txt', 'wb') as so:
        proc = subprocess.Popen(args, cwd=spec.out, env=env, stdout=so, stderr=subprocess.STDOUT)
        name = spec.npc.name.upper()
        started_at = ended_at = None
        last_activity = time.time()
        stop_reason = 'exited'
        pos = 0
        while proc.poll() is None:
            time.sleep(1.0)
            if log.exists():
                with open(log, 'rb') as f:
                    f.seek(pos)
                    chunk = f.read()
                cut = chunk.rfind(b'\n') + 1       # whole lines only
                pos += cut
                for raw in log_lines(chunk[:cut]):
                    if '[script] ' not in raw:
                        continue
                    body = raw.split('[script] ', 1)[1]
                    if not body.upper().startswith(name + ':'):
                        continue
                    last_activity = time.time()
                    if f'trigger {TRIGGER_DIALOG} of' in body and started_at is None:
                        started_at = time.time()
                    elif body.endswith(f'trigger {TRIGGER_DIALOG} ends') and started_at:
                        ended_at = time.time()
            now = time.time()
            if ended_at and now - ended_at > grace_s:
                stop_reason = 'block ended'
            elif started_at and not ended_at and now - last_activity > idle_s:
                stop_reason = f'no script line for {idle_s:.0f}s'
            elif not started_at and now - begin > start_s:
                stop_reason = f'block not started after {start_s:.0f}s'
            else:
                continue
            proc.send_signal(signal.SIGTERM)
            try:
                proc.wait(10)
            except subprocess.TimeoutExpired:
                proc.kill()
                proc.wait()
            break
    exe.unlink(missing_ok=True)
    run = {'spec': {'npc': asdict(spec.npc), 'tag': spec.tag, 'keys': spec.keys, 'sets': spec.sets,
                    'exec': spec.exec_line, 'input': spec.input_script, 'window_s': spec.window_s},
           'rc': proc.returncode, 'stop': stop_reason, 'wall_s': round(time.time() - begin, 1)}
    (spec.out / 'run.json').write_text(json.dumps(run, indent=1))
    return analyze_dir(spec.out)


def add_run_lines(npc: dict, mod: ModuleData | None) -> None:
    """A run recorded before the NPC carried the lines it runs on through
    (`run_lines`, `object_end`): read them from the module's script."""
    if 'run_lines' in npc or mod is None:
        return
    text = mod.scripts().get(npc['script'])
    for proto in parse_script(npc['script'], text or ''):
        block = proto.dialog()
        if proto.name.upper() == npc['name'].upper() and block and block.begin == npc['dialog_begin']:
            npc['object_end'] = proto.end
            npc['run_lines'] = proto.lines[block.begin - 1:proto.end]
            return


def analyze_dir(d: Path, mod: ModuleData | None = None) -> dict:
    """Analyze one run directory (run.json + revenant.log) into result.json."""
    run = json.loads((d / 'run.json').read_text())
    add_run_lines(run['spec']['npc'], mod)
    result = analyze(run, d / 'revenant.log', names_file(d.parent, None, None))
    (d / 'result.json').write_text(json.dumps(result, indent=1))
    return result


# =====================================================================
# Analysis
# =====================================================================

CONSOLE_ERRORS = ('Bad parameters', 'Unrecognized command', 'Context not found',
                  'Object context required', 'Command not availible', "Can't find",
                  'not found', 'Invalid', 'required', 'usage:', 'extra parameters')

# Console output retail prints for the same line, checked against its
# handlers (docs/gameflow/forensics/COMMAND_SYSTEM.md): reported as 'retail',
# not as a port failure. (script line pattern, messages, why)
RETAIL_CONSOLE = (
    (r'^(NOWAIT )?(\w+\.)?JUMP ', ('(extra parameters ignored)',),
     'jump 0x00420c70 leaves its label unread'),
    (r'^(NOWAIT )?BUYSELL(ADD|ADDBUYITEM|SALESPERSON|NOGOLDDIALOG|PURCHASEDIALOG|REMOVE|(ADD|REMOVE)(BUY)?CRITERIA) ', ('(extra parameters ignored)',),
     'the buy/sell name commands leave their token (COMMAND_SYSTEM §6.6)'),
    (r'^(NOWAIT )?FADECHARACTER(OUT|IN) ', ('(extra parameters ignored)',),
     'fadecharacterout/in 0x00428020/0x00428070 leave the name (COMMAND_SYSTEM §6.4)'),
    (r'^(NOWAIT )?(\w+\.)?STAT .*=', ('Bad parameters.', 'usage:'),
     'stat 0x00424010 wants the end of input after the value (COMMAND_SYSTEM §4)'),
    (r'^(ALWAYS|TRIGGER|PROXIMITY|CUBE|ACTIVATE|GIVE|GET|COMBAT|DEAD)\b', ('Unrecognized command',),
     "after a jump the block runs on into the next trigger, whose header isn't a command "
     '(TScript::Jump 0x00493fa0, SCRIPT_ENGINE §4.2)'),
)


def retail_console(script_line: str | None, message: str) -> str | None:
    if not script_line:
        return None
    n = norm(script_line)
    for pattern, messages, why in RETAIL_CONSOLE:
        if re.match(pattern, n) and any(message.startswith(m) for m in messages):
            return why
    return None


class SourceMap:
    """Maps traced lines of a block back to its source lines: forward from
    the last match, else from the block's start (a jump back)."""

    def __init__(self, block_lines: list[str], first: int):
        self.lines = [norm(x) for x in block_lines]
        self.first = first
        self.at = 0

    def find(self, traced: str) -> int | None:
        want = norm(traced)
        for rng in (range(self.at, len(self.lines)), range(0, self.at)):
            for k in rng:
                if self.lines[k] == want:
                    self.at = k + 1
                    return self.first + k
        return None


# Commands whose first parameter names an object for the resolver.
NAMED_OBJECT = re.compile(r'^(?:NOWAIT )?(?:\S+\.)?(?:GOTORELATIVEPOSITION|GOTORELATIVEDISTANCE|PIVOTOBJECT|'
                          r'FACEOBJECT|GOTO|USE|SCROLLTO|CENTERON) "?([^"\s]+)')


def analyze(run: dict, log: Path, names: set[str] | None = None) -> dict:
    spec = run['spec']
    npc = spec['npc']
    name = npc['name'].upper()
    rc, stop_reason = run['rc'], run['stop']
    smap = SourceMap(npc.get('run_lines') or npc['dialog_lines'], npc['dialog_begin'])
    r = {'npc': npc['name'], 'script': npc['script'], 'tag': spec['tag'], 'keys': spec['keys'],
         'sets': spec['sets'], 'exec': spec['exec'], 'rc': rc, 'stop': stop_reason,
         'wall_s': run['wall_s'], 'started': False, 'ended': False, 'lines_run': 0,
         'last_line': None, 'issues': [], 'committed': [], 'menus_shown': 0,
         'responses_run': 0, 'choices_run': [], 'states_set': [], 'level_entered': [],
         'ran_on': False,
         'exec_results': [], 'shutdown': False}
    if not log.exists():
        r['issues'].append({'kind': 'no log'})
        return r
    in_block = False
    last_script = None          # (object, text, source line) of the latest traced line
    seen_issue = set()

    def issue(kind, text, where=None):
        why = retail_console(where[1], text) if (kind == 'console' and where) else None
        if why:
            kind, text = 'retail', f'{text} ({why})'
        elif kind == 'console' and where and names is not None:
            m = NAMED_OBJECT.match(norm(where[1] or ''))
            if m and m.group(1) not in names and m.group(1) not in ('PLAYER', 'USER', 'THIS', 'TARGET'):
                kind, text = 'data', f'{text} (no object named {m.group(1)} on any level)'
        key = (kind, text, where and where[2])
        if key in seen_issue:
            return
        seen_issue.add(key)
        r['issues'].append({'kind': kind, 'text': text,
                            'object': where[0] if where else None,
                            'script_line': where[1] if where else None,
                            'source_line': where[2] if where else None})

    for raw in log_lines(log.read_bytes()):
        m = LOG_LINE.match(raw)
        if not m:
            continue
        level, msg = m.groups()
        if msg.startswith('[script] ') and level != 'TRACE':
            # not a traced line: attachments, loads, errors, unported members
            if msg.startswith('[script] error at'):
                issue('script error', msg[9:], last_script)
            elif "isn't ported" in msg or 'not ported' in msg:
                issue('not ported', msg[9:], last_script)
            continue
        if msg.startswith('[script] '):
            body = msg[9:]
            obj, _, rest = body.partition(': ')
            if obj.upper() == name and rest.startswith(f'trigger {TRIGGER_DIALOG} of'):
                in_block = r['started'] = True
                continue
            if obj.upper() == name and rest == f'trigger {TRIGGER_DIALOG} ends':
                in_block = False
                r['ended'] = True
                # A block ends at its own END; one that ends at an inner END
                # skipped its last lines. A block that ran on past its END
                # after a jump ends only at the object's END: its depth counts
                # the object's BEGIN (SCRIPT_ENGINE.md §4.2). (The source map
                # can't tell that END from a skipped block's.)
                last = r['last_line'] or {}
                if r['ran_on']:
                    ok = norm(last.get('text') or '') == 'END'
                else:
                    ok = not last.get('source_line') or last['source_line'] == npc['dialog_end']
                if not ok:
                    issue('early end', f'the block ended at line {last["source_line"]}, not at its END '
                          f'(line {npc["dialog_end"]})', (npc['name'], last.get('text'), last['source_line']))
                continue
            if rest.startswith('trigger '):
                continue
            src = smap.find(rest) if (in_block and obj.upper() == name) else None
            last_script = (obj, rest.strip(), src)
            if in_block and obj.upper() == name:
                r['lines_run'] += 1
                r['last_line'] = {'text': rest.strip(), 'source_line': src}
                if src and src > npc['dialog_end']:
                    r['ran_on'] = True
                n = norm(rest)
                if n.startswith('WAIT RESP'):
                    r['responses_run'] += 1
                c = re.match(r'(?:NOWAIT )?(?:\w+\.)?CHOICE (\S+)', n)
                if c:
                    r['choices_run'].append(c.group(1))
                s = re.match(r'SET (\w+) ?= ?(-?\d+)', n)
                if s:
                    r['states_set'].append(f'{s.group(1)}={s.group(2)}')
            continue
        if msg.startswith('[console] '):
            text = msg[10:]
            if any(e.lower() in text.lower() for e in CONSOLE_ERRORS):
                issue('console', text, last_script)
            continue
        if msg.startswith('[dialog] '):
            if ' choice(s) shown' in msg:
                r['menus_shown'] += 1
            c = re.match(r'\[dialog\] choice (\d+) committed \(label \'([^\']*)\'\)', msg)
            if c:
                r['committed'].append(f'{c.group(1)}:{c.group(2)}')
            continue
        if msg.startswith('[exec] > '):
            r['exec_results'].append(msg[9:])
            continue
        if msg.startswith('[session] entered level'):
            r['level_entered'].append(msg[10:])
            continue
        if msg.startswith('[shutdown] complete'):
            r['shutdown'] = True
        if level in ('ERROR', 'FATAL'):
            issue(level.lower(), msg, last_script)
        elif level == 'WARN' and ('not ported' in msg or "isn't ported" in msg):
            issue('not ported', msg, last_script)

    if not r['started']:
        issue('not started', 'the DIALOG block never started')
    elif not r['ended']:
        last = r['last_line'] or {}
        # A shop waits for its Exit (key `e`), and a choice that jumps back
        # to its menu, picked again and again, never leaves: neither is a
        # failure of the block, only of the key schedule.
        labels = [c.split(':', 1)[1] for c in r['committed']]
        if norm(last.get('text') or '') == 'WAIT BUYSELL':
            kind = 'shop open'
        elif any(labels.count(lb) >= 3 for lb in labels):
            kind = 'loop'
        else:
            kind = 'hang'
        issue(kind, f'block still running at the end ({stop_reason})',
              (npc['name'], last.get('text'), last.get('source_line')))
    if r['responses_run'] > r['menus_shown']:
        issue('menu', f'{r["responses_run"]} wait response, {r["menus_shown"]} menu(s) shown')
    if rc not in (0, -signal.SIGTERM):
        issue('crash', f'exit code {rc}')
    return r


# =====================================================================
# Report
# =====================================================================

def collapse(items: list[str]) -> str:
    """'a a a b' -> 'a×3 b'."""
    out = []
    for it in items:
        if out and out[-1][0] == it:
            out[-1][1] += 1
        else:
            out.append([it, 1])
    return ' '.join(f'{v}×{n}' if n > 1 else v for v, n in out)


def report(dirs: list[Path], show_retail: bool = False) -> None:
    results = []
    for d in dirs:
        for p in sorted(d.glob('*/result.json')):
            results.append(json.loads(p.read_text()))

    def run_name(r):
        return f'{r["script"][:-2]}_{r["npc"]}' + (f'_{r["tag"]}' if r['tag'] else '')

    print(f'{"run":<36} {"start":<5} {"end":<5} {"lines":>5} {"menus":>5}  committed / last line / stop')
    for r in results:
        last = r['last_line'] or {}
        print(f'{run_name(r):<36} {"yes" if r["started"] else "NO":<5} {"yes" if r["ended"] else "NO":<5} '
              f'{r["lines_run"]:>5} {r["menus_shown"]:>5}  {collapse(r["committed"]) or "-"}'
              + ('  (ran on to the object\'s END)' if r.get('ran_on') else '')
              + ('' if r['ended'] else f'  | last {last.get("source_line")}: {last.get("text")} | {r["stop"]}'))
    print()
    # The same issue seen in several runs (a nearby ALWAYS block) is listed once.
    grouped: dict[tuple, list[str]] = {}
    for r in results:
        for i in r['issues']:
            where = f'{r["script"]}:{i["source_line"]}' if i.get('source_line') else (i.get('object') or '')
            key = (i['kind'], where, i.get('text', ''), i.get('script_line') or '')
            grouped.setdefault(key, []).append(run_name(r))
    counts: dict[str, int] = {}
    print('Issues (script line where known):')
    for (kind, where, text, line), runs in grouped.items():
        counts[kind] = counts.get(kind, 0) + 1
        if kind in ('retail', 'data') and not show_retail:
            continue
        shown = runs[0] if len(runs) == 1 else f'{runs[0]} +{len(runs) - 1}'
        print(f'  {shown:<36} {kind:<12} {where:<16} {text}' + (f' `{line}`' if line else ''))
    print()
    print('By kind (distinct): ' + ', '.join(f'{k} {n}' for k, n in sorted(counts.items()))
          + ('' if show_retail else '  (retail and data lines hidden: --retail)'))


# =====================================================================
# CLI
# =====================================================================

def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--data', type=Path, default=os.environ.get('REVENANT_DATA_PATH'),
                    help='the install (default $REVENANT_DATA_PATH)')
    ap.add_argument('--slot', type=Path, help='save slot to start from (its CurMap overlays the base map)')
    sub = ap.add_subparsers(dest='cmd', required=True)

    lp = sub.add_parser('locate')
    lp.add_argument('--levels', help='comma list of levels (default: every level of each script)')
    lp.add_argument('--scripts', help='comma list of script files (default forest.s,town.s)',
                    default='forest.s,town.s')
    lp.add_argument('--json', type=Path)

    rp = sub.add_parser('run')
    rp.add_argument('--out', type=Path, required=True)
    rp.add_argument('--npc', action='append', help='NPC name (repeatable; default all located)')
    rp.add_argument('--scripts', default='forest.s,town.s')
    rp.add_argument('--levels')
    rp.add_argument('--jobs', type=int, default=4)
    rp.add_argument('--choice', type=int, default=1,
                    help='number key pressed at each menu; past a menu\'s count, 1 follows (= --keys K,1)')
    rp.add_argument('--keys', help='keys pressed each cycle: "3,1" (3 then 1), "2x6/3" (2 for six cycles, '
                                   'then 3); overrides --choice')
    rp.add_argument('--set', action='append', default=[],
                    help='VAR=N: a game state set before `use`; any other console line (with a space) runs as is')
    rp.add_argument('--tag', default='')
    rp.add_argument('--window', type=int, help='seconds of key presses (default: from the block\'s length, at most 600)')
    rp.add_argument('--offset', default='32,32', help='dx,dy from the NPC where Locke is placed')
    rp.add_argument('--binary', type=Path, default=REPO / 'build' / 'Revenant')
    rp.add_argument('--ini', type=Path, required=True, help='Revenant.ini for the save path')
    rp.add_argument('--grace', type=float, default=5.0, help='seconds kept after the block ends')
    rp.add_argument('--idle', type=float, default=120.0, help='seconds without a block line before giving up')
    rp.add_argument('--start-timeout', type=float, default=90.0)

    pp = sub.add_parser('report')
    pp.add_argument('dirs', type=Path, nargs='+')
    pp.add_argument('--reanalyze', action='store_true', help='rebuild each result.json from its log first')
    pp.add_argument('--retail', action='store_true',
                    help='also list the console lines retail prints too, and lookups of names no map has')

    a = ap.parse_args(argv)
    if a.cmd == 'report':
        if a.reanalyze:
            mod = ModuleData(a.data) if a.data else None
            for d in a.dirs:
                names_file(d, mod, a.slot)
                for p in sorted(d.glob('*/run.json')):
                    analyze_dir(p.parent, mod)
        report(a.dirs, a.retail)
        return 0
    if not a.data:
        ap.error('set REVENANT_DATA_PATH or pass --data')
    mod = ModuleData(a.data)
    scripts = [s.strip().lower() for s in a.scripts.split(',')] if a.scripts else None
    levels = {int(x) for x in a.levels.split(',')} if a.levels else None
    npcs = locate(mod, a.slot, scripts, levels)

    if a.cmd == 'locate':
        print_locate(npcs)
        if a.json:
            a.json.write_text(json.dumps([asdict(n) for n in npcs], indent=1))
        return 0

    if not a.slot:
        ap.error('run needs --slot')
    if a.npc:
        want = {n.upper() for n in a.npc}
        npcs = [n for n in npcs if n.name.upper() in want or f'{n.script[:-2]}_{n.name}'.upper() in want]
    offset = tuple(int(v) for v in a.offset.split(','))
    a.out.mkdir(parents=True, exist_ok=True)
    names_file(a.out, mod, a.slot)
    keys = a.keys or (f'{a.choice},1' if a.choice > 1 else str(a.choice))
    specs = [plan(n, keys, a.set, a.tag, a.out, offset, a.window) for n in npcs if n.found]
    for n in npcs:
        if not n.found:
            print(f'skip {n.name} ({n.script}): no map object')
    lock = threading.Lock()

    def go(spec):
        r = run_one(spec, a.binary.resolve(), a.slot.resolve(), a.ini.resolve(), a.data.resolve(),
                    a.grace, a.idle, a.start_timeout)
        with lock:
            print(f'{spec.out.name}: started={r["started"]} ended={r["ended"]} lines={r["lines_run"]} '
                  f'issues={len(r["issues"])} ({r["stop"]}, {r["wall_s"]}s)', flush=True)
        return r

    with ThreadPoolExecutor(max_workers=a.jobs) as pool:
        list(pool.map(go, specs))
    report([a.out])
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
