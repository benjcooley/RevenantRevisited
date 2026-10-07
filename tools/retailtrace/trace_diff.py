#!/usr/bin/env python3
"""Diff a retail trace against a port trace by [script]/[dialog] line sequence.

Both the traced retail build (retail-trace.log) and the port (revenant.log)
write the same four trace lines for a scene:

    [dialog] <speaker> says: <text>
    [script] <object>: <line>
    [script] <object>: trigger <N> of '<proto>' starts
    [script] <object>: trigger <N> ends

This tool extracts those lines from each log in order, strips the
log-framing (the retail build's leading tick; the port's
"<date> <time> <LEVEL> <file>:<line>:" prefix), and compares the two
sequences, printing the first divergence with context.

Only the per-line trace is compared. On the port side that is exactly the
TRACE-level [script] lines and the DEBUG-level "[dialog] ... says:" lines;
the INFO-level [script]/[dialog] bookkeeping (Load, Initialize, attached,
choices shown, choice committed) is skipped because the retail build does
not emit it. The retail log contains only the four forms already.

Usage:
    trace_diff.py --retail retail-trace.log --port revenant.log [--context N]
                  [--keep-tick] [--show-aligned]
"""
from __future__ import annotations

import argparse
import re
from pathlib import Path

# A port file-sink line: "YYYY-MM-DD HH:MM:SS LEVEL  file.cpp:123: <msg>"
PORT_LINE = re.compile(
    r'^\d{4}-\d\d-\d\d \d\d:\d\d:\d\d\s+(?P<level>[A-Z]+)\s+\S+?:\d+:\s+(?P<msg>.*)$')
# A retail-trace line: "<tick> [script|dialog] ..."
RETAIL_LINE = re.compile(r'^(?P<tick>\d+)\s+(?P<msg>\[(?:script|dialog)\].*)$')

# The four comparable forms (after framing is stripped).
SAYS = re.compile(r'^\[dialog\] .+ says: ')
TRIGGER = re.compile(r"^\[script\] .+: trigger \d+(?: of '.*')? (?:starts|ends)$")


def comparable(msg: str, *, port_level: str | None) -> bool:
    if SAYS.match(msg):
        # The port logs says at DEBUG; INFO [dialog] lines are bookkeeping.
        return port_level in (None, 'DEBUG')
    if TRIGGER.match(msg):
        return port_level in (None, 'TRACE')
    if msg.startswith('[script] '):
        # A per-line script trace: port logs it at TRACE. Exclude the INFO
        # bookkeeping ([script] Initialize:/Load(/attached ...).
        if port_level not in (None, 'TRACE'):
            return False
        return True
    return False


def extract(path: Path, *, is_retail: bool, keep_tick: bool):
    out = []
    for raw in path.read_text(encoding='cp1252', errors='replace').splitlines():
        if is_retail:
            m = RETAIL_LINE.match(raw.rstrip('\r'))
            if not m:
                continue
            msg = m.group('msg')
            if not comparable(msg, port_level=None):
                continue
            out.append((m.group('tick'), msg))
        else:
            m = PORT_LINE.match(raw)
            if not m:
                continue
            msg = m.group('msg')
            if not (msg.startswith('[script]') or msg.startswith('[dialog]')):
                continue
            if not comparable(msg, port_level=m.group('level')):
                continue
            out.append((None, msg))
    return out


def main():
    p = argparse.ArgumentParser(description=__doc__,
                                formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('--retail', type=Path, required=True)
    p.add_argument('--port', type=Path, required=True)
    p.add_argument('--context', type=int, default=3, help='lines of context at a divergence')
    p.add_argument('--show-aligned', action='store_true',
                   help='print every matched pair, not just the first divergence')
    a = p.parse_args()

    retail = extract(a.retail, is_retail=True, keep_tick=False)
    port = extract(a.port, is_retail=False, keep_tick=False)
    print(f'retail: {len(retail)} comparable lines   port: {len(port)} comparable lines')

    n = min(len(retail), len(port))
    first = None
    for i in range(n):
        if retail[i][1] != port[i][1]:
            first = i
            break
    if first is None and len(retail) == len(port):
        print('MATCH: the [script]/[dialog] sequences are identical.')
    elif first is None:
        print(f'PREFIX MATCH: identical for {n} lines, then one log has '
              f'{abs(len(retail) - len(port))} more.')
        longer, name = (retail, 'retail') if len(retail) > len(port) else (port, 'port')
        print(f'  {name} continues:')
        for tick, msg in longer[n:n + a.context]:
            print(f'    {msg}')
    else:
        print(f'DIVERGES at comparable line {first}:')
        lo = max(0, first - a.context)
        print('  --- retail ---')
        for i in range(lo, min(len(retail), first + a.context + 1)):
            mark = '>>' if i == first else '  '
            tick = retail[i][0]
            print(f'  {mark} [{tick:>6}] {retail[i][1]}')
        print('  --- port ---')
        for i in range(lo, min(len(port), first + a.context + 1)):
            mark = '>>' if i == first else '  '
            print(f'  {mark}          {port[i][1]}')

    if a.show_aligned:
        print('--- aligned pairs ---')
        for i in range(n):
            flag = '=' if retail[i][1] == port[i][1] else 'X'
            print(f'{flag} R:{retail[i][1]}')
            if flag == 'X':
                print(f'  P:{port[i][1]}')


if __name__ == '__main__':
    main()
