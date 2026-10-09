#!/usr/bin/env python3
"""Combat A/B, retail side, kata M2: the angle and distance kernels.

Every range, facing and step in combat goes through these, table-driven
in retail (the tables built by the original init 0x41e2de..0x41e535):

- `angle-diff`: AngleDiff(a, b) `0x46ded0` (cdecl)
- `facing`: ConvertToFacing(pos, target) `0x46dc60` (cdecl, two point
  pointers)
- `distance`: Distance(pos, target) `0x46de60` (cdecl, two point pointers)
- `vector`: ConvertToVector(angle, speed, out, zangle) `0x46db20` (cdecl)
- `obj-distance` / `obj-angle`: TObjectInstance::Distance `0x46ea20` /
  AngleTo `0x46ea90` (thiscall (obj), ret 4) on two bare objects
  (position at +0x10)

A case is a batch: {"kernel", "inputs": [[...], ...]}; the result is the
outputs in order. Schema `combat.kernels.v1`, shared with the port's
`Revenant --retail-ab=combat-kernels`.
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from guest import s32, serve, start  # noqa: E402

SCHEMA = 'combat.kernels.v1'
ANGLE_DIFF, FACING, DISTANCE, VECTOR = 0x46ded0, 0x46dc60, 0x46de60, 0x46db20
OBJ_DISTANCE, OBJ_ANGLE = 0x46ea20, 0x46ea90


class KernelFixture:
    def __init__(self, executable):
        started = time.perf_counter()
        self.vm, self.sha = start(executable)
        vm = self.vm
        self.p1, self.p2, self.out = vm.allocate(16), vm.allocate(16), vm.allocate(16)
        self.o1, self.o2 = vm.allocate(0x100), vm.allocate(0x100)
        vm.checkpoint()
        self.setup_ms = (time.perf_counter() - started) * 1000

    def _points(self, a, b):
        self.vm.write(self.p1, struct.pack('<3i', a[0], a[1], a[2] if len(a) > 2 else 0))
        self.vm.write(self.p2, struct.pack('<3i', b[0], b[1], b[2] if len(b) > 2 else 0))

    def run(self, case):
        vm = self.vm
        vm.restore()
        kernel, out = case['kernel'], []
        for args in case['inputs']:
            if kernel == 'angle-diff':
                out.append(s32(vm.call(ANGLE_DIFF, tuple(a & 0xffffffff for a in args))))
            elif kernel in ('facing', 'distance'):
                self._points(args[0], args[1])
                out.append(s32(vm.call(FACING if kernel == 'facing' else DISTANCE, (self.p1, self.p2))))
            elif kernel == 'vector':
                angle, speed, zangle = args
                vm.write(self.out, bytes(12))
                vm.call(VECTOR, (angle & 0xffffffff, speed & 0xffffffff, self.out, zangle & 0xffffffff))
                out.append(list(struct.unpack('<3i', vm.uc.mem_read(self.out, 12))))
            elif kernel in ('obj-distance', 'obj-angle'):
                a, b = args
                vm.write(self.o1 + 0x10, struct.pack('<3i', a[0], a[1], a[2] if len(a) > 2 else 0))
                vm.write(self.o2 + 0x10, struct.pack('<3i', b[0], b[1], b[2] if len(b) > 2 else 0))
                out.append(s32(vm.call(OBJ_DISTANCE if kernel == 'obj-distance' else OBJ_ANGLE, (self.o2,),
                                       this=self.o1)))
            else:
                raise ValueError(f'unknown kernel {kernel!r}')
        return dict(schema=SCHEMA, side='retail', kernel=kernel, outputs=out)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable')
    parser.add_argument('--serve', action='store_true')
    parser.add_argument('--case', type=Path)
    args = parser.parse_args()
    fixture = KernelFixture(args.executable)
    if args.serve:
        serve(fixture.sha, SCHEMA, fixture.setup_ms, fixture.run)
    elif args.case:
        print(json.dumps(fixture.run(json.loads(args.case.read_text()))))


if __name__ == '__main__':
    main()
