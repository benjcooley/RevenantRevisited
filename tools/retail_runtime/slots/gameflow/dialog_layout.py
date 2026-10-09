#!/usr/bin/env python3
"""Gameflow A/B target 3, retail side: the dialog pane's layout over ticks.

Runs the original `TDialogPane` pulse (`0x005351d0`: every entry's pulse
`0x005348f0`, the layout, the deletion of faded entries, the base
`TButtonPane` pulse `0x00435d70`) once per tick over a scripted case, and
reports every entry's placement after each tick. Schema
`gameflow.dialoglayout.v1`, shared with `Revenant --retail-ab=dialog-layout`.

A case: the geometry (map view y/w/h, side tabs width, status bar y/h:
the globals the layout reads, `0x006668e0/e4/e8`, `0x0065be5c`,
`0x0065a8c8/d0`), a tick count, and events: at tick t, add an entry (mode
1 NPC or 2 player, its height, its lifetime) or dismiss one (original
`Dismiss` `0x00534a40`). Events apply before that tick's pulse.

Fixture: the pane is zeroed 0x1f0 bytes with its entry array built by the
original TPointerArray constructor (`0x0041c7f0`) and appended by the
original Add (`0x0041c840`), as AddSpeech does. An entry is 0x160 bytes
from the original allocator with the fields AddSpeech's constructor call
(`0x00535d13`..`0x00535d2c`) sets for a line: base and offsets -10000,
width 400, the case's height (the constructor's text wrap needs GDI fonts;
the height is the port's for the same texts), its lifetime, fade 0 toward
12, one text (freed by the original destructor). The player has control
(`0x0065d0d0` = 1). Recorded boundary: the screen invalidate the entry
destructor asks for (`0x004aacb0`).

    dialog_layout.py EXE --serve     # JSONL: {"id":..,"case":{geometry,ticks,events}}
"""
from __future__ import annotations

import argparse
import struct
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from fixture import MALLOC, Boundaries, s32, serve, start  # noqa: E402

SCHEMA = 'gameflow.dialoglayout.v1'
PANE_PULSE, ENTRY_DISMISS = 0x5351d0, 0x534a40
ARRAY_CTOR, ARRAY_ADD = 0x41c7f0, 0x41c840
INVALIDATE = 0x4aacb0
MAP_Y, MAP_W, MAP_H = 0x6668e0, 0x6668e4, 0x6668e8
SIDETABS_W = 0x65be5c
STATUSBAR_Y, STATUSBAR_H = 0x65a8c8, 0x65a8d0
CONTROL_ON = 0x65d0d0
NOT_PLACED = -10000


class LayoutFixture:
    def __init__(self, executable):
        self.vm, self.sha = start(executable)
        vm = self.vm
        self.b = Boundaries(vm)
        self.invalidates = []
        self.b.add(INVALIDATE, 'invalidate', 0x14, self._invalidate)
        self.pane = vm.allocate(0x1f0)
        vm.call(ARRAY_CTOR, (0, 4), this=self.pane + 0x17c)
        vm.put_u32(CONTROL_ON, 1)
        vm.checkpoint()

    def _invalidate(self, args, ecx):
        self.invalidates.append(list(map(s32, args[:5])))
        return 0

    def _add(self, mode, height, life):
        vm = self.vm
        entry = vm.call(MALLOC, (0x160,))
        text = vm.call(MALLOC, (2,))
        for offset in (0x18, 0x1c, 0x20, 0x24, 0x30, 0x34):
            vm.put_u32(entry + offset, NOT_PLACED)
        vm.put_u32(entry + 0x00, self.pane)
        vm.put_u32(entry + 0x08, mode)
        vm.put_u32(entry + 0x0c, 0xffffff)
        vm.put_u32(entry + 0x10, 0xffffff)
        vm.put_u32(entry + 0x14, life)
        vm.put_u32(entry + 0x28, 400)
        vm.put_u32(entry + 0x2c, height)
        vm.put_u32(entry + 0x58, 12)
        vm.put_u32(entry + 0x5c, 1)
        vm.put_u32(entry + 0x60, text)
        vm.call(ARRAY_ADD, (entry,), this=self.pane + 0x17c)
        return entry

    def _state(self, ids):
        vm = self.vm
        count, data = vm.u32(self.pane + 0x17c), vm.u32(self.pane + 0x18c)
        out = []
        for i in range(count):
            e = vm.u32(data + 4 * i)
            f = [s32(v) for v in struct.unpack('<17i', vm.uc.mem_read(e + 0x14, 0x44))]
            # +0x14 ticks, +0x18/1c base, +0x20/24 offset, +0x28 w, +0x2c h,
            # +0x30/34 target, +0x38/3c pos, +0x40/44 step, +0x48/4c, +0x50 done, +0x54 fade
            out.append(dict(id=ids[e], ticksleft=f[0], basex=f[1], basey=f[2], offx=f[3], offy=f[4],
                            targetx=f[7], targety=f[8], posx=f[9], posy=f[10], stepx=f[11], stepy=f[12],
                            dismissed=bool(f[15]), fade=f[16]))
        return out

    def run(self, case):
        vm = self.vm
        vm.restore()
        self.invalidates = []
        g = case['geometry']
        vm.put_u32(MAP_Y, g['map'][1]); vm.put_u32(MAP_W, g['map'][2]); vm.put_u32(MAP_H, g['map'][3])
        vm.put_u32(SIDETABS_W, g['sidetabs_w'])
        vm.put_u32(STATUSBAR_Y, g['statusbar'][0]); vm.put_u32(STATUSBAR_H, g['statusbar'][1])
        heights = case['heights']
        events = sorted(case['events'], key=lambda e: e['tick'])
        ids, entries, ticks = {}, [], []
        for tick in range(case['ticks']):
            for ev in (e for e in events if e['tick'] == tick):
                if ev['op'] == 'add':
                    entry = self._add(ev['mode'], heights[len(entries)], ev['life'])
                    ids[entry] = len(entries)
                    entries.append(entry)
                elif ev['op'] == 'dismiss':
                    vm.call(ENTRY_DISMISS, this=entries[ev['entry']])
            vm.call(PANE_PULSE, this=self.pane, instruction_limit=5_000_000)
            ticks.append(self._state(ids))
        return dict(schema=SCHEMA, side='retail', case=case['name'], ticks=ticks,
                    invalidates=len(self.invalidates))


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable', type=Path)
    parser.add_argument('--serve', action='store_true', required=True)
    args = parser.parse_args()
    started = time.perf_counter()
    fixture = LayoutFixture(args.executable)
    serve(fixture.sha, SCHEMA, (time.perf_counter() - started) * 1000, fixture.run)


if __name__ == '__main__':
    main()
