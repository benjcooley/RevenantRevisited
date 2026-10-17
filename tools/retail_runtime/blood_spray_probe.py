#!/usr/bin/env python3
"""Retail blood droplets against the port's simulation (effects/bloodspray.cpp).

Runs the original TBloodAnimator::Initialize `0x4f18d0` and Pulse `0x4f1c60`
on a fixture animator (0x558 bytes) whose instance has the real
TBloodEffect::GetParams `0x4f94d0`. The engine calls around the spray are
answered by the host and recorded: T3DAnimator::Initialize `0x40dd60` and
Pulse `0x40e2e0`, GetObject `0x40eef0` (the eight boxes are only drawn),
the imagery's GetVerts `0x40a0c0`, and the instance's SetFlags (slot 0x40,
the kill). The CRT rand `0x58c582` is the MSVC stream from a fixed seed.

The port side compiles src/effects/bloodspray.cpp unchanged with a driver
that feeds the same stream. Every droplet field (30 x 0x24 bytes), the kill
tick and the number of rand() calls are compared after Initialize and after
every tick. Float fields are compared bit for bit, apart from Initialize's
sin/cos products, which x87 computes in extended precision (a tolerance of
2 ulp is reported separately).

    blood_spray_probe.py EXE --output DIR [--cases N] [--ticks T]
"""
from __future__ import annotations

import argparse
import json
import struct
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE))
from fixturekit import Boundaries, start  # noqa: E402
from unicorn import UC_HOOK_CODE  # noqa: E402
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_EIP, UC_X86_REG_ESP  # noqa: E402

INITIALIZE, PULSE, GET_PARAMS = 0x4f18d0, 0x4f1c60, 0x4f94d0
RAND, VIOLENCE = 0x58c582, 0x5d79e8
DROPS, DROP_SIZE, COUNT = 0xfc, 0x24, 30

DRIVER = r'''
#include "effects/bloodspray.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
static uint32_t seed = 1; static int calls = 0;
static int32_t Draw(int32_t lo, int32_t hi) {
    if (lo == hi) return lo;
    if (hi < lo) { int32_t t = lo; lo = hi; hi = t; }
    seed = seed * 214013u + 2531011u; ++calls;
    return lo + int32_t((seed >> 16) & 0x7fff) % (hi - lo + 1);
}
static void Dump(const char* tag, int tick, const retail_blood::TSpray& s, int done) {
    printf("%s %d %u %d %d", tag, tick, seed, calls, done);
    for (const auto& d : s.Drops()) {
        uint32_t f[7]; float v[7] = {d.vel[0], d.vel[1], d.vel[2], d.pos[0], d.pos[1], d.pos[2], d.scale};
        std::memcpy(f, v, sizeof(f));
        for (uint32_t w : f) printf(" %u", w);
        printf(" %d %d %d %d %d", int(d.used), d.size, d.stage, d.count, d.delay);
    }
    printf("\n");
}
int main(int argc, char** argv) {
    // seed violence height hangle vangle hspread vspread ticks
    seed = uint32_t(strtoul(argv[1], nullptr, 10));
    const int violence = atoi(argv[2]);
    retail_blood::SParams p; p.height = atoi(argv[3]); p.hangle = atoi(argv[4]);
    p.vangle = atoi(argv[5]); p.hspread = atoi(argv[6]); p.vspread = atoi(argv[7]);
    const int ticks = atoi(argv[8]);
    retail_blood::TSpray spray;
    spray.Initialize(p, violence, Draw);
    Dump("I", 0, spray, 0);
    for (int t = 1; t <= ticks; ++t) { const bool done = spray.Step(violence, Draw); Dump("T", t, spray, done); if (done) break; }
}
'''


def build_port(output: Path, simulation: Path | None = None) -> Path:
    driver = output / 'driver.cpp'
    driver.write_text(DRIVER)
    binary = output / 'driver'
    command = ['clang++', '-std=c++17', '-O1', '-g', '-fsanitize=address,undefined', '-iquote', str(ROOT / 'src'),
               '-iquote', str(ROOT / 'src/effects'), str(driver), str(simulation or ROOT / 'src/effects/bloodspray.cpp'), '-o', str(binary)]
    result = subprocess.run(command, capture_output=True, text=True)
    (output / 'compile.log').write_text(result.stdout + result.stderr)
    if result.returncode:
        raise RuntimeError('port driver failed to compile; see compile.log')
    return binary


class Native:
    def __init__(self, executable: Path):
        self.vm, self.sha = start(executable)
        vm = self.vm
        self.seams = Boundaries(vm)
        self.kills = []
        for address, name, pop in ((0x40dd60, 'T3DAnimator::Initialize', 0), (0x40e2e0, 'T3DAnimator::Pulse', 0),
                                   (0x40eef0, 'GetObject', 4), (0x40a0c0, 'GetVerts', 8)):
            self.seams.add(address, name, pop, lambda args, ecx: 0)
        self.rand_calls = 0

        def rand(uc, address, size, user):
            self.rand_calls += 1
            sp = uc.reg_read(UC_X86_REG_ESP)
            uc.reg_write(UC_X86_REG_EAX, vm.random_msvc())
            uc.reg_write(UC_X86_REG_EIP, vm.u32(sp))
            uc.reg_write(UC_X86_REG_ESP, sp + 4)
        vm.uc.hook_add(UC_HOOK_CODE, rand, begin=RAND, end=RAND)
        self.instance = vm.allocate(0x200)
        vtable = vm.allocate(0x210)
        vm.put_u32(self.instance, vtable)
        vm.put_u32(vtable + 0x204, GET_PARAMS)

        def set_flags(args, ecx):
            vm.put_u32(ecx + 8, args[0])
            self.kills.append(args[0])
            return 0
        vm.put_u32(vtable + 0x40, self.seams.add_stub('SetFlags', 4, set_flags))
        self.animator = vm.allocate(0x558)
        vm.put_u32(self.animator + 4, self.instance)
        vm.put_u32(self.animator + 8, vm.allocate(0x100))   # imagery: only GetVerts, answered above
        vm.checkpoint()

    def run(self, seed, violence, params, ticks):
        vm = self.vm
        vm.restore()
        self.kills.clear()
        self.rand_calls = 0
        vm.rng_seed = seed
        vm.put_u32(VIOLENCE, violence)
        for i, value in enumerate(params):
            vm.put_u32(self.instance + 0x184 + 4 * i, value)
        vm.put_u32(self.instance + 0x198, 3)   # the count SetParams stores; unread
        rows = []
        vm.call(INITIALIZE, this=self.animator, instruction_limit=2_000_000)
        if vm.error:
            raise RuntimeError(f'Initialize faulted: {vm.error}')
        rows.append(self.dump('I', 0, 0))
        for tick in range(1, ticks + 1):
            before = len(self.kills)
            vm.call(PULSE, this=self.animator, instruction_limit=2_000_000)
            if vm.error:
                raise RuntimeError(f'Pulse faulted: {vm.error}')
            done = int(len(self.kills) > before and bool(self.kills[-1] & 0x1000))
            rows.append(self.dump('T', tick, done))
            if done:
                break
        return rows

    def dump(self, tag, tick, done):
        vm = self.vm
        raw = bytes(vm.uc.mem_read(self.animator + DROPS, DROP_SIZE * COUNT))
        fields = [tag, tick, vm.rng_seed, self.rand_calls, done]
        for i in range(COUNT):
            record = raw[i * DROP_SIZE:(i + 1) * DROP_SIZE]
            fields += list(struct.unpack('<7I', record[:28]))
            used, size, stage, count, delay = struct.unpack('<I4B', record[28:36])
            fields += [int(used != 0), size, stage, count, delay]
        return fields


def ulps(a, b):
    def ordered(bits):
        return bits if bits < 0x80000000 else 0x80000000 - bits
    return abs(ordered(a) - ordered(b))


def compare(native_rows, port_rows):
    errors, near = [], 0
    if len(native_rows) != len(port_rows):
        errors.append(dict(kind='length', native=len(native_rows), port=len(port_rows)))
    for nrow, prow in zip(native_rows, port_rows):
        if nrow[:5] != prow[:5]:
            errors.append(dict(kind='header', native=nrow[:5], port=prow[:5]))
            continue
        for drop in range(COUNT):
            base = 5 + drop * 12
            nf, pf = nrow[base:base + 7], prow[base:base + 7]
            ni, pi = nrow[base + 7:base + 12], prow[base + 7:base + 12]
            if ni != pi:
                errors.append(dict(kind='state', tick=nrow[1], drop=drop, native=ni, port=pi))
            for field, (a, b) in enumerate(zip(nf, pf)):
                if a == b:
                    continue
                distance = ulps(a, b)
                if distance <= 2:
                    near += 1
                else:
                    errors.append(dict(kind='float', tick=nrow[1], drop=drop, field=field, native=a, port=b, ulps=distance))
    return errors, near


CASES = [
    # seed, violence, (height, hangle, vangle, hspread, vspread): EffectBurst's blood
    # (height 40/50, facing+128, 3, 5, 5), the death squirts, low violence.
    (1, 5, (50, 0x80 + 64, 3, 5, 5)),
    (7, 5, (40, 0x80 + 200, 3, 5, 5)),
    (12345, 4, (50, 0x80 + 13, 3, 5, 5)),
    (99, 5, (0x4b, -0x5b - 40, 0x40, 0x5b, 5)),
    (31337, 5, (0x82, -0x5b + 100, 0x40, 0x5b, 5)),
    (4242, 2, (50, 0x80, 3, 5, 5)),
    (5, 1, (40, 0x80 + 77, 3, 5, 5)),
    (2026, 0, (50, 0x80, 3, 5, 5)),
    (77, 5, (0x28, -0x80 + 9, 0x20, 0x20, 5)),
]


def run(executable: Path, output: Path, extra_cases: int = 24, ticks: int = 120, simulation: Path | None = None):
    """`simulation` replaces src/effects/bloodspray.cpp (negative controls)."""
    output.mkdir(parents=True, exist_ok=False)
    binary = build_port(output, simulation)
    native = Native(executable)
    cases = list(CASES)
    seed = 1
    for i in range(extra_cases):
        seed = (seed * 1103515245 + 12345) & 0x7fffffff
        cases.append((seed, 3 + i % 3, (40 + 10 * (i % 2), (seed >> 8) & 0x1ff, 3, 5, 5)))
    report = dict(retail_sha256=native.sha, cases=[], errors=0, near_float_fields=0)
    for case_seed, violence, params in cases:
        native_rows = native.run(case_seed, violence, params, ticks)
        args = [str(binary), str(case_seed), str(violence), *map(str, params), str(ticks)]
        result = subprocess.run(args, capture_output=True, text=True, check=True)
        port_rows = []
        for line in result.stdout.splitlines():
            parts = line.split()
            port_rows.append([parts[0]] + [int(v) for v in parts[1:]])
        errors, near = compare(native_rows, port_rows)
        drops = sum(native_rows[0][5 + d * 12 + 7] for d in range(COUNT))
        report['cases'].append(dict(seed=case_seed, violence=violence, params=params, ticks=len(native_rows) - 1,
                                    drops=drops, rand_calls=native_rows[-1][3], errors=errors[:8],
                                    error_count=len(errors), near_float_fields=near))
        report['errors'] += len(errors)
        report['near_float_fields'] += near
    report['status'] = 'pass' if report['errors'] == 0 else 'fail'
    (output / 'manifest.json').write_text(json.dumps(report, indent=1) + '\n')
    print(report['status'], 'cases', len(cases), 'errors', report['errors'], 'near', report['near_float_fields'])
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--cases', type=int, default=24)
    parser.add_argument('--ticks', type=int, default=120)
    a = parser.parse_args()
    sys.exit(0 if run(a.executable, a.output, a.cases, a.ticks)['status'] == 'pass' else 1)
