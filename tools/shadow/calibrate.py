#!/usr/bin/env python3
"""Calibrate Shadow Smoke's duel model against real fights (docs/shadow/CALIBRATION.md).

The same matchups both ways, from the same game state:

- **real**: the combat arena (tools/combatarena/arena.py), Locke played by
  the player bot pressing his keys, one run a seed, each run's combat trace
  read for the fight's numbers;
- **model**: the duel harness (`--shadow-duel`), which sets up the same
  arena with the same console lines, then throws the duel model's Trial
  fight once a seed, seeds 1..N, in one process.

    calibrate.py scenarios                  # write the matrix's arena scenarios
    calibrate.py real   [CELL ...] [--seeds 1-100] [--jobs 8]
    calibrate.py model  [CELL ...] [--runs 200]
    calibrate.py report [CELL ...] [--markdown]
    calibrate.py all    [CELL ...]          # model, real, report

A CELL is a scenario name without its prefix, e.g. `l1-fists-araknid`, or a
part of one (`araknid`, `l1-`). Results go to build/calibrate/<cell>/; a
real sweep resumes where it stopped (seeds already read are kept).

Environment: REVENANT_DATA_PATH (the install; default the repo's data/).
"""
from __future__ import annotations

import argparse
import concurrent.futures
import json
import math
import os
import shutil
import statistics
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
os.environ.setdefault('REVENANT_DATA_PATH', str(REPO / 'data'))
sys.path.insert(0, str(REPO / 'tools' / 'combatarena'))
import arena  # noqa: E402  (run_once, parse)

SCENARIOS = REPO / 'tools' / 'combatarena' / 'scenarios' / 'calibrate'
PREFIX = 'calib_'
OUT = REPO / 'build' / 'calibrate'
BINARY = REPO / 'build' / 'Revenant'
# The arena slot: Port Resave's Locke saved standing on the flat arena, so a
# run loads level 2 alone (make_arena_slot.py).
SLOT = 'build/arena_slots/ArenaLocke'
LOCKE_AT = (25375, 3072)
# The monster appears this far east of him: inside its hearing, so it comes
# at once, as COMBAT_ARENA.md's A1b (kata W1 checked that opening in retail).
FOE_GAP = 150


@dataclass(frozen=True)
class Cell:
    level: int
    weapon: str         # 'fists' or 'sword' (the Short Sword Port Resave's Locke holds)
    foe: str

    @property
    def name(self) -> str:
        return f"l{self.level}-{self.weapon}-{self.foe.lower().replace(' ', '')}"


# Locke level 1 against the first monsters he meets (GRIND.md §3.2), with
# fists and with the Short Sword; then a few cells further on.
MATRIX = [Cell(1, weapon, foe) for foe in ('Araknid', 'Under Druhg', 'Mongrel Druhg', 'Kantha', 'Thrush Druhg', 'Arakna')
          for weapon in ('fists', 'sword')]
MATRIX += [Cell(4, 'sword', 'Thrush Druhg'), Cell(4, 'fists', 'Mongrel Druhg'),
           Cell(8, 'sword', 'Pale Ogrok'), Cell(8, 'sword', 'Vashar')]


def scenario(cell: Cell) -> dict:
    lines = ['sleep 12', f'playerlevel {cell.level}', 'delinv "Lesser Healing"']
    if cell.weapon == 'fists':
        lines.append('unequip "Short Sword"')
    lines += ['sleep 12', f'addat {LOCKE_AT[0] + FOE_GAP} {LOCKE_AT[1]} "{cell.foe}"']
    return {
        'name': PREFIX + cell.name.replace('-', '_'),
        'about': f"Calibration (tools/shadow/calibrate.py): Locke level {cell.level}, "
                 f"{'barehanded' if cell.weapon == 'fists' else 'with the Short Sword'}, no potion, played by the "
                 f"player bot, against {'an' if cell.foe[0] in 'AEIOU' else 'a'} {cell.foe} {FOE_GAP} east on the flat arena; "
                 f"until one of them dies.",
        'slotdir': SLOT,
        'seed': 1,
        'ticks': 4 * 60 * 24,
        'until_deaths': 1,
        'exec': lines,
        'playerbot': True,
    }


def spawn_tick(lines: list[str]) -> int:
    """The trace tick the last command runs on: the queue runs one entry a
    tick from tick 1, and `sleep N` holds it N more."""
    tick = 1
    for line in lines[:-1]:
        tick += 1 + (int(line.split()[1]) if line.startswith('sleep') else 0)
    return tick


def foes_of(lines: list[str]) -> list[str]:
    return [line.split(None, 3)[3].strip('"') for line in lines if line.startswith('addat ')]


def cells(selected: list[str]) -> list[Cell]:
    if not selected:
        return MATRIX
    chosen = [c for c in MATRIX if any(s in c.name for s in selected)]
    if not chosen:
        raise SystemExit(f'no cell matches {selected}; cells: {", ".join(c.name for c in MATRIX)}')
    return chosen


def ensure_slot(binary: Path) -> None:
    if (REPO / SLOT / 'game.sav').exists():
        return
    subprocess.run([sys.executable, str(REPO / 'tools' / 'combatarena' / 'make_arena_slot.py'), Path(SLOT).name,
                    '--locke', f'{LOCKE_AT[0]},{LOCKE_AT[1]}', '--out', str((REPO / SLOT).parent),
                    '--binary', str(binary)], check=True)


def private_binary(binary: Path, tag: str) -> Path:
    """A copy of the binary beside it (it finds its assets there), so a
    rebuild during a sweep never changes the runs."""
    copy = binary.with_name(f'{binary.name}.calibrate-{tag}')
    shutil.copy2(binary, copy)
    return copy


# ---- The real side --------------------------------------------------------------------

def trace_stats(trace: Path, spawn: int) -> dict:
    """One arena fight's numbers, in the duel report's terms. A blow is a
    swing that struck (ResolveHit's hit or miss: a glance is a miss); a swing
    out of reach strikes nothing and isn't one."""
    s = dict(won=False, died=False, ticks=0, first=-1, blows=0, hits=0, dealt=0, swings=0,
             foeblows=0, foehits=0, foeswings=0, foemoves=0, taken=0, suffered=0,
             blocks=0, dodges=0, evades=0)
    foes, dead, end = set(), set(), -1
    for r in arena.parse(trace):
        if r['kind'] != 'event':
            continue
        what, who, tick = r['what'], r['name'], r['tick']
        if what == 'attack':
            if who == 'Locke':
                s['swings'] += 1
            elif r.get('damage') == '-1':
                s['foemoves'] += 1       # a PLAYANIM move: a step, a jump, a feint
            else:
                s['foeswings'] += 1
                foes.add(who)
        elif what in ('hit', 'miss'):
            if s['first'] < 0:
                s['first'] = tick - spawn
            if who == 'Locke':
                s['blows'] += 1
                s['hits'] += what == 'hit'
                foes.add(r['target'])
            elif r.get('target') == 'Locke':
                s['foeblows'] += 1
                s['foehits'] += what == 'hit'
        elif what == 'damage':
            amount = int(r['amount'])
            if who == 'Locke':
                s['taken'] += 1
                s['suffered'] += amount
            elif r.get('by') == 'Locke':
                s['dealt'] += amount
        elif what == 'death':
            if who == 'Locke':
                s['died'], end = True, tick
            else:
                dead.add(who)
                end = tick
        elif what == 'bot':
            state, why = r.get('state'), r.get('why', '')
            s['blocks'] += state == 'block'
            s['dodges'] += 'dodge' in why
            s['evades'] += 'side step' in why or 'back away' in why
    s['won'] = not s['died'] and bool(dead) and foes <= dead
    s['ticks'] = end - spawn if end >= 0 else -1
    return s


def run_real(cell: Cell, seeds: range, jobs: int, binary: Path) -> None:
    scn = json.loads((SCENARIOS / f"{PREFIX}{cell.name.replace('-', '_')}.json").read_text())
    spawn = spawn_tick(scn['exec'])
    base = OUT / cell.name / 'real'
    todo = [n for n in seeds if not (base / f'seed{n}' / 'stats.json').exists()]

    def one(n: int) -> None:
        out = base / f'seed{n}'
        result = arena.run_once(dict(scn, seed=n), binary, out)
        stats = trace_stats(out / 'trace.tsv', spawn)
        stats.update(seed=n, stop=result['stop'])
        (out / 'stats.json').write_text(json.dumps(stats) + '\n')

    with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as pool:
        list(pool.map(one, todo))
    rows = [json.loads((base / f'seed{n}' / 'stats.json').read_text()) for n in seeds
            if (base / f'seed{n}' / 'stats.json').exists()]
    (OUT / cell.name / 'real.jsonl').write_text(''.join(json.dumps(r) + '\n' for r in rows))
    print(f'{cell.name}: real, {len(rows)} fights ({len(todo)} new)')


# ---- The model side -------------------------------------------------------------------

def run_model(cell: Cell, runs: int, binary: Path, traced: int = 0) -> None:
    scn = json.loads((SCENARIOS / f"{PREFIX}{cell.name.replace('-', '_')}.json").read_text())
    scn = {k: v for k, v in scn.items() if k not in ('playerbot', 'until_deaths')}
    scn['ticks'] = 10 ** 6          # the harness ends the run itself
    out = OUT / cell.name / 'model-run'
    duels = OUT / cell.name / 'duels'
    if duels.exists():
        shutil.rmtree(duels)
    extra = [f'--shadow-duel={",".join(foes_of(scn["exec"]))}', f'--shadow-duel-runs={runs}',
             f'--shadow-out={duels}', f'--shadow-duel-trace={traced}']
    result = arena.run_once(scn, binary, out, extra=extra)
    lines = (duels / 'duels.jsonl') if (duels / 'duels.jsonl').exists() else None
    if not lines:
        raise SystemExit(f'{cell.name}: the duel harness wrote nothing ({result["stop"]}; see {out}/revenant.log)')
    shutil.copy2(lines, OUT / cell.name / 'model.jsonl')
    print(f'{cell.name}: model, {sum(1 for _ in lines.open())} fights')


# ---- The comparison -------------------------------------------------------------------

def load(path: Path) -> list[dict]:
    return [json.loads(line) for line in path.read_text().splitlines() if line.strip()] if path.exists() else []


def measures(rows: list[dict]) -> dict:
    """What the table compares, over a cell's fights."""
    n = len(rows)
    if not n:
        return {}
    ended = [r['ticks'] for r in rows if r.get('won') or r.get('died')]
    total = lambda key: sum(r.get(key, 0) for r in rows)  # noqa: E731
    return dict(
        n=n,
        win=100.0 * sum(bool(r.get('won')) for r in rows) / n,
        win_n=n,
        died=100.0 * sum(bool(r.get('died')) for r in rows) / n,
        median=statistics.median(ended) if ended else float('nan'),
        mean=statistics.fmean(ended) if ended else float('nan'),
        first=statistics.median([r['first'] for r in rows if r.get('first', -1) >= 0] or [float('nan')]),
        swings=total('swings') / n,
        connect=100.0 * total('blows') / max(total('swings'), 1),
        blows=total('blows') / n,
        hitpct=rate(total('hits'), total('blows')),
        hitpct_n=total('blows'),
        perblow=total('dealt') / total('blows') if total('blows') >= MIN_BLOWS else float('nan'),
        foeswings=total('foeswings') / n,
        foeevery=sum(r['ticks'] for r in rows if r['ticks'] > 0) / max(total('foeswings'), 1),
        foeconnect=100.0 * total('foeblows') / max(total('foeswings'), 1),
        foeblows=total('foeblows') / n,
        foehitpct=rate(total('foehits'), total('foeblows')),
        foehitpct_n=total('foeblows'),
        foemoves=total('foemoves') / n,
        suffered=total('suffered') / n,
        blocks=total('blocks') / n,
    )


# A rate over fewer blows than this is noise (its standard error is over 9
# points), so the table shows it but doesn't judge it.
MIN_BLOWS = 30


def rate(part: int, whole: int) -> float:
    return 100.0 * part / whole if whole >= MIN_BLOWS else float('nan')


# Tolerances (CALIBRATION.md §3 says why): (measure, label, kind, bound).
# 'rate': a percentage, judged within the bound in points or two standard
# errors of the difference, whichever is wider (a rate over a few dozen
# blows can't be held to 10 points); 'ratio': model / real - 1, with a
# floor so small numbers aren't judged by their relative error.
GATES = [
    ('win', 'win %', 'rate', 10.0),
    ('median', 'median ticks', 'ratio', (0.25, 24.0)),
    ('hitpct', 'Locke hit %', 'rate', 10.0),
    ('perblow', 'dmg / blow', 'ratio', (0.20, 1.0)),
    ('foehitpct', 'foe hit %', 'rate', 10.0),
    ('suffered', 'HP lost', 'ratio', (0.25, 5.0)),
]
SHOWN = [('swings', 'Locke swings', 'info', None), ('connect', 'swings struck %', 'info', None),
         ('blows', 'Locke blows', 'info', None), ('foeswings', 'foe swings', 'info', None),
         ('foeevery', 'ticks a foe swing', 'info', None), ('foeconnect', 'foe struck %', 'info', None),
         ('foeblows', 'foe blows', 'info', None), ('foemoves', 'foe moves', 'info', None),
         ('blocks', 'blocks', 'info', None), ('first', 'first blow', 'info', None)]


def judge(kind: str, bound, real: float, model: float, nreal: int = 0, nmodel: int = 0) -> tuple[str, bool]:
    if real != real or model != model:     # NaN: nothing to compare
        return 'n/a', True
    if kind == 'rate':
        d = model - real
        p = (real * nreal + model * nmodel) / max(nreal + nmodel, 1) / 100.0
        se = 100.0 * math.sqrt(p * (1 - p) * (1 / max(nreal, 1) + 1 / max(nmodel, 1)))
        return f'{d:+.0f}', abs(d) <= max(bound, 2 * se)
    if kind == 'ratio':
        relative, floor = bound
        d = model - real
        ok = abs(d) <= max(relative * abs(real), floor)
        return (f'{100.0 * d / real:+.0f}%' if real else f'{d:+.1f}'), ok
    return f'{model - real:+.1f}', True


def report(chosen: list[Cell], markdown: bool) -> int:
    failures = 0
    rows_out = []
    for cell in chosen:
        real, model = measures(load(OUT / cell.name / 'real.jsonl')), measures(load(OUT / cell.name / 'model.jsonl'))
        if not real or not model:
            print(f'{cell.name}: {"no real fights" if not real else "no model fights"} yet')
            continue
        verdicts = []
        for key, label, kind, bound in GATES + SHOWN:
            text, ok = judge(kind, bound, real[key], model[key], real.get(key + '_n', 0), model.get(key + '_n', 0))
            verdicts.append((label, real[key], model[key], text, ok if kind != 'info' else None))
        passed = all(v[4] is not False for v in verdicts)
        failures += not passed
        rows_out.append((cell, real['n'], model['n'], verdicts, passed))

    if markdown:
        # The gated measures and how many swings struck, real / model.
        shown = len(GATES) + 2
        labels = [v[0] for v in rows_out[0][3][:shown]] if rows_out else []
        print('| Cell | ' + ' | '.join(labels) + ' | |')
        print('|---|' + '---|' * len(labels) + '---|')
        for cell, nr, nm, verdicts, passed in rows_out:
            cols = [f'{fmt(r)} / {fmt(m)}' + ('' if ok is None else '' if ok else ' **✗**')
                    for _, r, m, _, ok in verdicts[:shown]]
            print(f'| {cell.name} | ' + ' | '.join(cols) + f" | {'pass' if passed else '**fail**'} |")
        return 1 if failures else 0

    for cell, nr, nm, verdicts, passed in rows_out:
        print(f"\n{cell.name}  (real {nr} fights, model {nm})  {'PASS' if passed else 'FAIL'}")
        print(f"  {'measure':<18}{'real':>9}{'model':>9}{'diff':>9}")
        for label, r, m, text, ok in verdicts:
            mark = '' if ok is None else '  ok' if ok else '  FAIL'
            print(f'  {label:<18}{fmt(r):>9}{fmt(m):>9}{text:>9}{mark}')
    print(f'\n{len(rows_out) - failures} of {len(rows_out)} cells within tolerance')
    return 1 if failures else 0


def fmt(v: float) -> str:
    return 'n/a' if v != v else f'{v:.0f}' if abs(v) >= 10 else f'{v:.1f}'


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('cmd', choices=['scenarios', 'real', 'model', 'report', 'all'])
    p.add_argument('cells', nargs='*')
    p.add_argument('--seeds', default='1-100', help='the real side: A-B')
    p.add_argument('--runs', type=int, default=200, help='the model side: duels a cell (seeds 1..N)')
    p.add_argument('--jobs', type=int, default=8, help='real fights at once')
    p.add_argument('--trace', type=int, default=0, help="the model side: write the first N fights' timelines "
                   '(build/calibrate/<cell>/duels/trace-<seed>.txt)')
    p.add_argument('--binary', type=Path, default=BINARY)
    p.add_argument('--markdown', action='store_true', help='the report as a markdown table')
    a = p.parse_args()
    chosen = cells(a.cells)

    if a.cmd == 'scenarios':
        SCENARIOS.mkdir(parents=True, exist_ok=True)
        for cell in MATRIX:
            scn = scenario(cell)
            (SCENARIOS / f"{scn['name']}.json").write_text(json.dumps(scn, indent=2) + '\n')
        print(f'{len(MATRIX)} scenarios in {SCENARIOS}')
        return 0
    if a.cmd == 'report':
        return report(chosen, a.markdown)

    ensure_slot(a.binary)
    if a.cmd in ('model', 'all'):
        binary = private_binary(a.binary, 'model')
        try:
            for cell in chosen:
                run_model(cell, a.runs, binary, a.trace)
        finally:
            binary.unlink(missing_ok=True)
    if a.cmd in ('real', 'all'):
        lo, hi = (int(v) for v in a.seeds.split('-'))
        binary = private_binary(a.binary, 'real')
        try:
            for cell in chosen:
                run_real(cell, range(lo, hi + 1), a.jobs, binary)
        finally:
            binary.unlink(missing_ok=True)
    return report(chosen, a.markdown) if a.cmd == 'all' else 0


if __name__ == '__main__':
    sys.exit(main())
