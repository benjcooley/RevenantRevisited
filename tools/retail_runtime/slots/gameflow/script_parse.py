#!/usr/bin/env python3
"""Gameflow A/B target 1, retail side: TScriptProto::ParseScript over script files.

Runs the original parser (`0x00494e20`) the way TScriptManager::ParseScripts
(`0x00496860`) drives it, one OBJECT block per call, over each case's script
text, and dumps every prototype as JSON (schema `gameflow.scriptparse.v1`,
shared with the port's `Revenant --retail-ab=script-parse`):

- the prototype: name, text extent (file offsets), ParseScript's result;
- its triggers (`+0x0c` pointer array of 0x50-byte records): type, block
  position, name, cube, distance, region name;
- its DATA variables (`+0x24` pointer array of 0x2c-byte records);
- its labels, each resolved by the original `TScript::Jump` (`0x00493fa0`):
  where the script goes on and at what block depth;
- every script error the parser reported, with its line.

What runs as original code: the tokenizer, ParseScript, ParseCriteria,
ParseVariables (DATA), the trigger-array adds, CRT malloc/free and string
functions, the TScriptProto and TScript constructors and Jump. What is a
recorded boundary instead: the text bar (`0x0054d170`), the editor console
(`0x0041ee50`) and the error log file (`0x004820b0`) -- the three places a
script error goes. One observation hook (`0x00494e6c`, no state change) reads
where ParseScript starts the prototype's text.

The stream and token are laid out as ParseScripts' own inline code builds them
(`0x00496892`..`0x004968fe`): TStringParseStream {vtable `0x005a36f8`, name,
begin, end, cursor} and a 0x38-byte TToken with a 0x2000-byte text buffer from
the original allocator. ParseScripts' manager bookkeeping (same-named
prototype replacement, N_SCRIPTADDED) is not run; each block's prototype is
kept as parsed, and a block whose ParseScript returns -1 is reported with that
result, as ParseScripts then discards it.

Usage (persistent JSONL, one process per run):
    script_parse.py EXE --serve          # {"id":..,"case":{"name":..,"path":..}} per line
    script_parse.py EXE --case FILE.s    # one file, JSON to stdout
"""
from __future__ import annotations

import argparse
import json
import re
import struct
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from fixture import MALLOC, Boundaries, s32, serve, start  # noqa: E402
from unicorn import UC_HOOK_CODE  # noqa: E402
from unicorn.x86_const import UC_X86_REG_EDX  # noqa: E402

SCHEMA = 'gameflow.scriptparse.v1'

# Original code.
TOKEN_GET = 0x478a10                            # TToken::Get
PROTO_CTOR = 0x4946f0                           # TScriptProto(parent, owner, filename, buffer)
PARSE_SCRIPT = 0x494e20                         # TScriptProto::ParseScript(TToken&)
SCRIPT_CTOR = 0x492170                          # TScript(owner, proto)
JUMP = 0x493fa0                                 # TScript::Jump(context, label)
STRING_STREAM_VTABLE = 0x5a36f8
# Boundaries (recorded, not executed).
TEXTBAR_PRINT = 0x54d170                        # cdecl (textbar, fmt, ...)
CONSOLE_PRINT = 0x41ee50                        # cdecl (fmt, ...)
ERROR_LOG = 0x4820b0                            # cdecl (fmt, ...): appends to the log file
# Observation: ParseScript has computed the text start (EDX) and calls ParseCriteria.
OBSERVE_START = 0x494e6c

TKN_EOF = 10
LABEL_RE = re.compile(rb'(?m)^[ \t]*:[ \t]*([A-Za-z_][A-Za-z0-9_]*)')


class ScriptParseFixture:
    def __init__(self, executable):
        self.vm, self.sha = start(executable)
        vm = self.vm
        self.errors = []
        self.starts = []
        b = Boundaries(vm)
        # All three are cdecl: the caller removes the arguments.
        b.add(TEXTBAR_PRINT, 'textbar', 0, lambda a, c: self._error('textbar', a[1], a[2:]))
        b.add(CONSOLE_PRINT, 'console', 0, lambda a, c: self._error('console', a[0], a[1:]))
        b.add(ERROR_LOG, 'errorlog', 0, lambda a, c: self._error('errorlog', a[0], a[1:]))
        vm.uc.hook_add(UC_HOOK_CODE, self._observe_start, begin=OBSERVE_START, end=OBSERVE_START)
        self.stream_name = vm.allocate(8)
        vm.write(self.stream_name, b'String\0')
        self._setup(b)
        vm.checkpoint()

    def _setup(self, boundaries):
        """A fixture built on this one adds its boundaries and state here,
        before the checkpoint."""

    def _error(self, channel, fmt, args):
        self.errors.append(dict(channel=channel, text=self._format(self.vm.string(fmt), list(args))))
        return 0

    def _format(self, fmt, args):
        """printf for the %d/%i/%s the script errors use; other specs verbatim."""
        vm = self.vm
        out, i = [], 0
        while i < len(fmt):
            c = fmt[i]
            if c == '%' and i + 1 < len(fmt):
                spec = fmt[i + 1]
                if spec in 'di':
                    out.append(str(s32(args.pop(0))))
                elif spec == 's':
                    # MSVC's printf prints "(null)" for a NULL %s.
                    pointer = args.pop(0)
                    out.append(vm.string(pointer) if pointer else '(null)')
                elif spec == '%':
                    out.append('%')
                else:
                    out.append(fmt[i:i + 2])
                i += 2
                continue
            out.append(c)
            i += 1
        return ''.join(out)

    def _observe_start(self, uc, address, size, user):
        self.starts.append(uc.reg_read(UC_X86_REG_EDX))

    # -- one case ----------------------------------------------------------
    def run(self, name, data: bytes, filename='script.s'):
        protos = [dict(line=p['line'], **self._dump_proto(p['proto'], p['buf'], p['start'], p['result']),
                       errors=p['errors'])
                  for p in self.parse(data, filename)]
        return dict(schema=SCHEMA, side='retail', case=name, protos=protos)

    def parse(self, data: bytes, filename='script.s'):
        """Restore, then ParseScript every OBJECT block of `data`: a list of
        {proto, buf, start, result, line, errors} in file order."""
        vm = self.vm
        vm.restore()
        self.errors, self.starts = [], []
        self.source = data
        if b'\0' in data:
            raise ValueError('script text contains NUL')
        buf = vm.allocate(len(data) + 1)
        vm.write(buf, data + b'\0')
        fname = vm.allocate(len(filename) + 1)
        vm.write(fname, filename.encode('cp1252') + b'\0')
        # TStringParseStream, as ParseScripts builds it (0x496892..0x4968ac).
        stream = vm.allocate(0x14)
        vm.write(stream, struct.pack('<5I', STRING_STREAM_VTABLE, self.stream_name, buf, buf + len(data), buf))
        # TToken (0x4968c9..0x4968fe): zeroed, stream +0x0c, text buffer +0x28, line +0x30 = 1.
        token = vm.allocate(0x38)
        textbuf = vm.call(MALLOC, (0x2000,))
        vm.write(textbuf, b'\0')
        vm.put_u32(token + 0x0c, stream)
        vm.put_u32(token + 0x28, textbuf)
        vm.put_u32(token + 0x30, 1)
        vm.call(TOKEN_GET, this=token)

        protos = []
        while vm.u32(token + 0x10) != TKN_EOF:
            line = vm.u32(token + 0x30)
            first_error = len(self.errors)
            proto = vm.call(MALLOC, (0x4c,))
            vm.call(PROTO_CTOR, (0, 0, fname, 0), this=proto)
            nstarts = len(self.starts)
            result = s32(vm.call(PARSE_SCRIPT, (token,), this=proto, instruction_limit=50_000_000))
            start = self.starts[nstarts] if len(self.starts) > nstarts else None
            protos.append(dict(proto=proto, buf=buf, start=start, result=result, line=line,
                               errors=self.errors[first_error:]))
            if len(protos) > 10000:
                raise RuntimeError('ParseScript made no progress')
        return protos

    def _cstr(self, address):
        return None if not address else self.vm.string(address)

    def _dump_proto(self, proto, buf, start, result):
        vm = self.vm
        text_ptr, length = vm.u32(proto + 4), vm.u32(proto + 0x40)
        text = bytes(vm.uc.mem_read(text_ptr, length)) if text_ptr and length else b''
        text_start = (start - buf) if start is not None else None
        source = self.source
        out = dict(result=result, name=self._cstr(vm.u32(proto)),
                   parent=bool(vm.u32(proto + 8)),
                   text_start=text_start, text_len=length,
                   # The text is a verbatim copy of the file at [start, start+len).
                   text_is_file_span=(text_start is not None and
                                      source[text_start:text_start + length] == text),
                   numtriggers=vm.u32(proto + 0x44))
        # Triggers: TPointerArray at +0x0c {count +0, data +0x10}, 0x50-byte records.
        count, data = vm.u32(proto + 0x0c), vm.u32(proto + 0x1c)
        triggers = []
        for i in range(count):
            raw = bytes(vm.uc.mem_read(vm.u32(data + 4 * i), 0x50))
            ttype, pos = struct.unpack_from('<iI', raw, 0)
            triggers.append(dict(type=ttype, pos=pos,
                                 name=raw[8:0x1c].split(b'\0')[0].decode('cp1252'),
                                 cube=list(struct.unpack_from('<6i', raw, 0x1c)),
                                 dist=struct.unpack_from('<i', raw, 0x34)[0],
                                 field38=struct.unpack_from('<i', raw, 0x38)[0],
                                 region=raw[0x3c:0x50].split(b'\0')[0].decode('cp1252')))
        out['triggers'] = triggers
        # DATA variables (ParseVariable 0x495830): TPointerArray at +0x24 {count +0,
        # data +0x10} of 0x2c-byte records {type +0 (0 NUMBER, 1 TEXT), value
        # pointer +4 (int, or 30-byte text), name +8, value size +0x28}.
        count, data = vm.u32(proto + 0x24), vm.u32(proto + 0x34)
        variables = []
        for i in range(count):
            rec = vm.u32(data + 4 * i)
            if not rec:
                continue
            vtype, value = vm.u32(rec), vm.u32(rec + 4)
            entry = dict(type=vtype, name=vm.string(rec + 8), number=0, text='')
            if vtype == 0 and value:
                entry['number'] = s32(vm.u32(value))
            elif vtype == 1 and value:
                entry['text'] = vm.string(value)
            variables.append(entry)
        out['variables'] = variables
        out['labels'] = self._labels(proto, text) if text_ptr else []
        return out

    def _labels(self, proto, text):
        """Resolve every `:label` of the prototype's text with the original Jump."""
        vm = self.vm
        text_ptr = vm.u32(proto + 4)
        labels, seen = [], set()
        for m in LABEL_RE.finditer(text):
            label = m.group(1)
            if label.lower() in seen:
                continue
            seen.add(label.lower())
            script = vm.allocate(0xe8)
            vm.call(SCRIPT_CTOR, (0, proto), this=script)
            lp = vm.allocate(len(label) + 1)
            vm.write(lp, label + b'\0')
            first_error = len(self.errors)
            found = vm.call(JUMP, (0, lp), this=script, instruction_limit=50_000_000)
            ip = vm.u32(script + 0x48)
            labels.append(dict(label=label.decode('cp1252'), found=found,
                               ip=(ip - text_ptr) if ip else None,
                               depth=s32(vm.u32(script + 0xa4)),
                               errors=self.errors[first_error:]))
        return labels


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable', type=Path)
    parser.add_argument('--serve', action='store_true')
    parser.add_argument('--case', type=Path)
    args = parser.parse_args()
    started = time.perf_counter()
    fixture = ScriptParseFixture(args.executable)
    setup_ms = (time.perf_counter() - started) * 1000
    if args.serve:
        def run(case):
            path = Path(case['path'])
            return fixture.run(case['name'], path.read_bytes(), case.get('filename', path.name))
        serve(fixture.sha, SCHEMA, setup_ms, run)
    elif args.case:
        print(json.dumps(fixture.run(args.case.name, args.case.read_bytes(), args.case.name), indent=1))
    else:
        parser.error('--serve or --case')


if __name__ == '__main__':
    main()
