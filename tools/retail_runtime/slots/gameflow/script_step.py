#!/usr/bin/env python3
"""Gameflow A/B, retail side: block stepping -- TScript::Continue over script blocks.

Runs the original `TScript::Continue` (`0x004933d0`) -- its line loop, the
original command interpreter (`0x0041e8e0`) with its command table, `begin`,
`end`, `else` and `jump` (`0x00420c70` -> `0x00471290` -> `TScript::Jump`
`0x00493fa0`), SkipBlock/SkipLine and the tokenizer -- over every block of a
script file, and records the lines it hands to the interpreter, in order
(schema `gameflow.scriptstep.v1`, shared with the port's
`Revenant --retail-ab=script-step`).

Per case (one script file), the file is parsed with the original
ParseScript (script_parse.py) once; then, for each prototype and each
condition policy:

- each trigger block runs from its start (the original Start `0x00492440`,
  the trigger's type and depth 0, as Continue's trigger start sets them);
- each `:label` runs from the original `Jump` to it, on a fresh script, as a
  `jump` command or a taken response leaves the script.

Then one `Continue(commanddone=1)` runs the block. No trigger fires: the
script's top prototype is an empty one (the trigger scan walks the top
prototype's chain, the text comes from the current one), so only the
jumped-to block runs.

Recorded boundaries (nothing behind them runs):
- every command handler except begin/end/else/jump: logs the command and
  returns 0 (success, no wait); `wait response` (and `responsenohide`,
  `respnohide`, `respctrlon`) also sets a one-frame wait on the script, so
  the run stops where the script would wait for the player's choice (what
  the choice jumps to is a label run); `if` / `while` return the policy's outcome
  (`if`: 0x40 true / 0x80 false; `while`: 0x400 / 0x200, as 0x0041fb70 /
  0x0041fbc0 encode their condition). The table's class contexts are set to
  -1 in the fixture's memory so every handler is reached;
- the context resolver `0x0041e690`: every name is the fixture object;
- the text bar `0x0054d170` (script errors), the console `0x0041ee50`
  (interpreter messages) and the error log `0x004820b0`;
- the owner's vtable `+0x144` (Continue's per-run call), `+0x148`/`+0x14c`
  (script started/ended). `+0x150` is the original
  `TObjectInstance::ParseCommand` `0x004713b0` (returns 2, "unrecognized"),
  what TCharacter's vtable holds.

Observation hooks (no state change): the interpreter call `0x00493942`
(stream position after the line's first token, block depth) and its return
`0x00493947` (the result bits). A run is capped at CAP interpreter calls
and JUMP_CAP jumps (a loop through a label re-reads the prototype from its
top on every pass; a few passes show it): the hook then sets the script's
pause bit (+0x4c 0x10000, what Break sets), so Continue's loop leaves after
that line.

Usage:
    script_step.py EXE --serve          # {"id":..,"case":{"name":..,"path":..}} per line
    script_step.py EXE --case FILE.s    # one file, JSON to stdout
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from fixture import MALLOC, s32, serve  # noqa: E402
from script_parse import LABEL_RE, PROTO_CTOR, SCRIPT_CTOR, JUMP, ScriptParseFixture  # noqa: E402
from unicorn import UC_HOOK_CODE  # noqa: E402
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ESI, UC_X86_REG_ESP  # noqa: E402

SCHEMA = 'gameflow.scriptstep.v1'

# Original code.
CONTINUE = 0x4933d0                             # TScript::Continue(commanddone), thiscall
START = 0x492440                                # TScript::Start(proto, pos, priority), thiscall
PARSE_COMMAND = 0x4713b0                        # TObjectInstance::ParseCommand: returns 2
COMMANDS = 0x5c6e88                             # SCommand[189], 0x1c each, null-name terminated
KEEP = ('begin', 'end', 'else', 'jump')         # structural handlers that run as original code
# Boundaries.
RESOLVE_CONTEXT = 0x41e690                      # cdecl (name, context, script) -> object
# Observation.
AT_INTERPRETER = 0x493942                       # call CommandInterpreter; token at [esp+0x14+0x10]
AFTER_INTERPRETER = 0x493947                    # EAX = result bits
CAP = 256                                       # interpreter calls per run
JUMP_CAP = 8                                    # jumps per run (each re-reads the text from the top)
PAUSED = 0x10000
CMD_JUMP = 0x2000
CHARACTER = 12

# `wait <these>` (0x0041fe30) waits for the player's response.
RESPONSE_WAITS = ('response', 'responsenohide', 'respnohide', 'respctrlon')
FRAMES_WAIT = 4                                 # +0xb4 wait type, +0xb8 frames

POLICIES = {'true': (0x40, 0x400), 'false': (0x80, 0x200)}


class ScriptStepFixture(ScriptParseFixture):
    def _setup(self, b):
        vm = self.vm
        self.policy = 'true'
        self.trace = None
        self.owner_calls = []
        # The command table: every handler a recorded boundary but the
        # structural ones; class contexts -1, so each handler is reached.
        entry = COMMANDS
        while vm.u32(entry):
            name = vm.string(vm.u32(entry))
            if name not in KEEP:
                if name in ('if', 'while'):
                    stub = b.add_stub(name, 0, lambda a, c, n=name: self._condition(n))
                else:
                    stub = b.add_stub(name, 0, lambda a, c, n=name: self._command(n, a))
                vm.put_u32(entry + 4, stub)
            vm.put_u32(entry + 8, 0xffffffff)
            vm.put_u32(entry + 12, 0xffffffff)
            entry += 0x1c
        # The owner: a character-class object whose vtable holds the slots
        # Continue and the interpreter reach; any other slot faults (0).
        self.vtable = vm.allocate(0x200)
        vm.put_u32(self.vtable + 0x144, b.add_stub('owner+0x144', 0, lambda a, c: self._hook('owner+0x144')))
        vm.put_u32(self.vtable + 0x148, b.add_stub('owner+0x148', 0, lambda a, c: self._hook('script started')))
        vm.put_u32(self.vtable + 0x14c, b.add_stub('owner+0x14c', 0, lambda a, c: self._hook('script ended')))
        vm.put_u32(self.vtable + 0x150, PARSE_COMMAND)
        self.owner = vm.allocate(0x400)
        vm.put_u32(self.owner, self.vtable)
        vm.write(self.owner + 4, struct.pack('<h', CHARACTER))
        b.add(RESOLVE_CONTEXT, 'resolve', 0, self._resolve)
        vm.uc.hook_add(UC_HOOK_CODE, self._before_line, begin=AT_INTERPRETER, end=AT_INTERPRETER)
        vm.uc.hook_add(UC_HOOK_CODE, self._after_line, begin=AFTER_INTERPRETER, end=AFTER_INTERPRETER)
        # An empty prototype: the top of every script here, so no trigger fires.
        self.fname = vm.allocate(16)
        vm.write(self.fname, b'step.s\0')
        self.empty = vm.call(MALLOC, (0x4c,))
        vm.call(PROTO_CTOR, (0, 0, self.fname, 0), this=self.empty)

    # -- boundaries ----------------------------------------------------------
    def _note(self, what):
        if self.trace:
            self.trace[-1].setdefault('calls', []).append(what)

    def _command(self, name, args):
        self._note(name)
        if name == 'wait' and self.vm.string(self.vm.u32(args[1] + 0x28)).lower() in RESPONSE_WAITS:
            # The script waits for the player's choice: set a wait, so
            # Continue leaves after this line; the label runs cover what
            # the choice jumps to.
            self.vm.write(args[3] + 0xb4, bytes([FRAMES_WAIT]))
            self.vm.put_u32(args[3] + 0xb8, 1)
        return 0

    def _condition(self, name):
        self._note(name)
        return POLICIES[self.policy][0 if name == 'if' else 1]

    def _hook(self, what):
        self.owner_calls.append(what)
        return 0

    def _resolve(self, args, ecx):
        self._note('resolve ' + self.vm.string(args[0]))
        return self.owner

    def _before_line(self, uc, address, size, user):
        vm = self.vm
        script = uc.reg_read(UC_X86_REG_ESI)
        token = uc.reg_read(UC_X86_REG_ESP) + 0x14 + 0x10     # four pushes since lea ecx,[esp+0x14]
        cursor = vm.u32(vm.u32(token + 0x0c) + 0x10)
        text = vm.u32(vm.u32(script + 8) + 4)
        self.trace.append(dict(at=cursor - text, depth=s32(vm.u32(script + 0xa4))))
        if len(self.trace) > CAP:
            vm.put_u32(script + 0x4c, vm.u32(script + 0x4c) | PAUSED)

    def _after_line(self, uc, address, size, user):
        bits = uc.reg_read(UC_X86_REG_EAX)
        self.trace[-1]['bits'] = bits
        if bits & CMD_JUMP:
            self.jumps += 1
            if self.jumps > JUMP_CAP:
                script = uc.reg_read(UC_X86_REG_ESI)
                self.vm.put_u32(script + 0x4c, self.vm.u32(script + 0x4c) | PAUSED)

    # -- one case ----------------------------------------------------------
    def run(self, name, data: bytes, filename='script.s', policies=('true', 'false'), share=None):
        """`share` (k, n): run only every n-th run from the k-th (the
        driver splits a file over n processes and merges by run index)."""
        vm = self.vm
        parsed = self.parse(data, filename)
        protos = []
        counter = 0
        for index, p in enumerate(parsed):
            proto = p['proto']
            text_ptr, length = vm.u32(proto + 4), vm.u32(proto + 0x40)
            entry = dict(index=index, name=self._cstr(vm.u32(proto)), result=p['result'],
                         text_start=(p['start'] - p['buf']) if p['start'] is not None else None,
                         text_len=length, runs=[])
            protos.append(entry)
            if p['result'] < 0 or not text_ptr:
                continue                                # ParseScripts discards it
            text = bytes(vm.uc.mem_read(text_ptr, length))
            count, records = vm.u32(proto + 0x0c), vm.u32(proto + 0x1c)
            triggers = [vm.u32(records + 4 * i) for i in range(count)]
            labels, seen = [], set()
            for m in LABEL_RE.finditer(text):
                if m.group(1).lower() not in seen:
                    seen.add(m.group(1).lower())
                    labels.append(m.group(1))
            for policy in policies:
                self.policy = policy
                todo = [(self._run_trigger, (proto, rec, i, policy)) for i, rec in enumerate(triggers)]
                todo += [(self._run_label, (proto, label, policy)) for label in labels]
                for run, args in todo:
                    counter += 1
                    if share and (counter - 1) % share[1] != share[0]:
                        continue
                    entry['runs'].append(dict(run(*args), index=counter - 1))
        return dict(schema=SCHEMA, side='retail', case=name, cap=CAP, protos=protos)

    def _script(self):
        vm = self.vm
        script = vm.allocate(0xe8)
        vm.call(SCRIPT_CTOR, (self.owner, self.empty), this=script)
        vm.put_u32(self.owner + 0x84, script)
        return script

    def _run_trigger(self, proto, rec, i, policy):
        vm = self.vm
        ttype, pos = struct.unpack('<iI', bytes(vm.uc.mem_read(rec, 8)))
        script = self._script()
        vm.call(START, (proto, pos, vm.u32(rec + 0x38)), this=script)
        vm.put_u32(script + 0x1c, ttype)
        out = dict(key=f'trigger {i} {policy}', kind='trigger', id=i, type=ttype, policy=policy,
                   start=dict(ip=self._ip(script, proto), depth=s32(vm.u32(script + 0xa4))))
        out.update(self._continue(script, proto))
        return out

    def _run_label(self, proto, label, policy):
        vm = self.vm
        script = self._script()
        vm.put_u32(script + 8, proto)                   # the current prototype; ip stays 0
        lp = vm.allocate(len(label) + 1)
        vm.write(lp, label + b'\0')
        first = len(self.errors)
        found = vm.call(JUMP, (0, lp), this=script, instruction_limit=50_000_000)
        out = dict(key=f'label {label.decode("cp1252").lower()} {policy}', kind='label',
                   id=label.decode('cp1252'), policy=policy,
                   start=dict(found=found, ip=self._ip(script, proto), depth=s32(vm.u32(script + 0xa4)),
                              errors=[e['text'] for e in self.errors[first:] if e['channel'] == 'textbar']))
        out.update(self._continue(script, proto))
        return out

    def _ip(self, script, proto):
        ip = self.vm.u32(script + 0x48)
        return ip - self.vm.u32(proto + 4) if ip else None

    def _continue(self, script, proto):
        vm = self.vm
        self.trace, self.owner_calls, self.jumps = [], [], 0
        first = len(self.errors)
        fault = None
        try:
            returned = vm.call(CONTINUE, (1,), this=script, instruction_limit=600_000_000)
        except Exception as e:                          # a fault is a result
            returned, fault = None, f'{type(e).__name__}: {e}'
        errors = self.errors[first:]
        return dict(lines=self.trace, truncated=len(self.trace) > CAP or self.jumps > JUMP_CAP,
                    returned=returned, fault=fault,
                    end=dict(ip=self._ip(script, proto), depth=s32(vm.u32(script + 0xa4)),
                             flags=vm.u32(script + 0x4c)),
                    errors=[e['text'] for e in errors if e['channel'] == 'textbar'],
                    console=[e['text'] for e in errors if e['channel'] != 'textbar'],
                    waiting=bool(vm.uc.mem_read(script + 0xb4, 1)[0]),
                    owner=self.owner_calls)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable', type=Path)
    parser.add_argument('--serve', action='store_true')
    parser.add_argument('--case', type=Path)
    args = parser.parse_args()
    started = time.perf_counter()
    fixture = ScriptStepFixture(args.executable)
    setup_ms = (time.perf_counter() - started) * 1000
    if args.serve:
        def run(case):
            path = Path(case['path'])
            return fixture.run(case['name'], path.read_bytes(), case.get('filename', path.name),
                               tuple(case.get('policies', ('true', 'false'))), case.get('share'))
        serve(fixture.sha, SCHEMA, setup_ms, run)
    elif args.case:
        print(json.dumps(fixture.run(args.case.name, args.case.read_bytes(), args.case.name), indent=1))
    else:
        parser.error('--serve or --case')


if __name__ == '__main__':
    main()
