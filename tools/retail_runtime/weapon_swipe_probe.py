#!/usr/bin/env python3
"""Retail weapon swipe against the port's TWeaponSwipe (src/game/effects/weaponswipe.cpp).

Drives the original code a drawn frame at a time, as TCharAnimator does:
SetupWeaponSwipe `0x4d8f10` (which calls TWeaponSwipe::Init `0x4d90b0`)
while the swipe isn't running (Animate `0x4d7a20`), then Update `0x4d95b0`
and BuildStrip `0x4d9380` while it is (Render `0x4d7c47..0x4d7c96`). The
fixture animator holds the swipe at +0x1a8; its character has a class word,
chardata (SWIPECOLOR at +0x168, SWIPEFULL +0x16c), a doing block (action,
attack with swipeframeon/off at +0xec/+0xf0), a frame and, for a player, a
weapon in hand (+0x2b0; name +0x38, a SwipeFull stat through vtable +0xd4).
The engine calls are answered by the host and recorded: GetWeaponExtents
`0x4d8e60` (the case's bounds), the animator's GetObjectNum `0x40edc0` (the
case's names) and GetObject `0x40eef0` (an object whose +0x58 is the step's
weapon matrix). Allocation and the name compare are the original CRT.

The port side compiles src/game/effects/weaponswipe.cpp unchanged with a driver that
plays the same frames through the same selection TCharAnimator makes.
After every frame the running flag, the colour, both sample histories and
the 80 strip vertices (position bits and packed colour) are compared bit
for bit; the original's 32 unwritten vertices are checked to be zero.

    weapon_swipe_probe.py EXE --output DIR [--cases N]
"""
from __future__ import annotations

import argparse
import json
import math
import random
import struct
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE))
from fixturekit import Boundaries, start  # noqa: E402

SETUP, UPDATE, BUILD = 0x4d8f10, 0x4d95b0, 0x4d9380
EXTENTS, FIND_OBJECT, GET_OBJECT = 0x4d8e60, 0x40edc0, 0x40eef0
SWIPE = 0x1a8                    # TCharAnimator +0x1a8
RUNNING = SWIPE + 0x380
POINTS, VERTICES, STRIP = 8, 112, 80
ATTACK = 7                       # ACTION_ATTACK

DRIVER = r'''
#include "weaponswipe.h"
#include <cstdio>
#include <cstring>
#include <string>
static void Bits(float f) { uint32_t u; std::memcpy(&u, &f, 4); printf(" %u", u); }
int main()
{
    // case: player red green blue swipefull weaponfull lo.x lo.y lo.z hi.x hi.y hi.z steps
    int player, red, green, blue, swipefull, weaponfull, steps;
    TWeaponSwipe::SVec3 lo, hi;
    if (scanf("%d %d %d %d %d %d %f %f %f %f %f %f %d", &player, &red, &green, &blue, &swipefull,
              &weaponfull, &lo[0], &lo[1], &lo[2], &hi[0], &hi[1], &hi[2], &steps) != 13)
        return 2;
    TWeaponSwipe swipe;
    for (int n = 0; n < steps; ++n)
    {
        // step: doing action frame on off hasattack weapon(name|-) m[16]
        int doing, action, frame, on, off, has_attack;
        char name[64];
        TWeaponSwipe::SMatrix m;
        if (scanf("%d %d %d %d %d %d %63s", &doing, &action, &frame, &on, &off, &has_attack, name) != 7)
            return 3;
        for (float& v : m)
            if (scanf("%f", &v) != 1)
                return 4;
        const bool in_hand = player && std::strcmp(name, "-") != 0;
        // TCharAnimator::WeaponSwipeFrame
        TWeaponSwipe::SFrame now;
        now.player = player != 0;
        now.in_hand = in_hand ? name : nullptr;
        now.attacking = doing && action == 7 && (!player || in_hand);
        now.frame = frame;
        if (now.attacking && has_attack) { now.frame_on = on; now.frame_off = off; }
        now.weapon = &m;
        // TCharAnimator::UpdateWeaponSwipe / SetupWeaponSwipe
        if (!swipe.Initialized() && (red || green || blue))
        {
            TWeaponSwipe::SParams params;
            params.extent_min = lo;
            params.extent_max = hi;
            params.color = TWeaponSwipe::ColorOf(uint8_t(red), uint8_t(green), uint8_t(blue));
            params.full = player ? (in_hand && weaponfull) : swipefull != 0;
            if (in_hand)
                params.weapon = name;
            swipe.Init(params, now);
        }
        if (swipe.Initialized())
            swipe.Update(now);
        printf("%d %d", n, int(swipe.Initialized()));
        if (swipe.Initialized())
        {
            for (float c : swipe.Color()) Bits(c);
            for (const auto* side : {&swipe.Ring().hilt, &swipe.Ring().tip})
                for (const auto& p : *side)
                    for (float c : p) Bits(c);
            TWeaponSwipe::SStrip strip;
            TWeaponSwipe::BuildStrip(swipe.Ring(), swipe.Color(), strip);
            for (const auto& v : strip)
            {
                for (float c : v.pos) Bits(c);
                printf(" %u", v.argb);
            }
        }
        printf("\n");
    }
}
'''


def build_port(output: Path, source: Path | None = None) -> Path:
    driver = output / 'driver.cpp'
    driver.write_text(DRIVER)
    binary = output / 'driver'
    command = ['clang++', '-std=c++17', '-O1', '-g', '-fsanitize=address,undefined', '-iquote', str(ROOT / 'src'),
               str(driver), str(source or ROOT / 'src/game/effects/weaponswipe.cpp'), '-o', str(binary)]
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
        self.case = None
        self.matrix = [0.0] * 16
        self.stats = []

        def extents(args, ecx):
            lo, hi = self.case['extents']
            vm.write(args[0], struct.pack('<3f', *lo))
            vm.write(args[1], struct.pack('<3f', *hi))
            return 0
        self.seams.add(EXTENTS, 'GetWeaponExtents', 8, extents)
        self.seams.add(FIND_OBJECT, 'GetObjectNum', 4,
                       lambda args, ecx: self.case['objects'].get(vm.string(args[0]), -1))
        self.object = vm.allocate(0x400)

        def get_object(args, ecx):
            vm.write(self.object + 0x58, struct.pack('<16f', *self.matrix))
            return self.object
        self.seams.add(GET_OBJECT, 'GetObject', 4, get_object)

        self.character = vm.allocate(0x400)
        vtable = vm.allocate(0x400)
        vm.put_u32(self.character, vtable)
        vm.put_u32(vtable + 0x1fc, self.seams.add_stub('GetDoing', 0, lambda args, ecx: vm.u32(ecx + 0xd8)))
        self.chardata = vm.allocate(0x400)
        vm.put_u32(self.character + 0xfc, self.chardata)
        self.doing = vm.allocate(0x100)
        self.attack = vm.allocate(0x400)
        self.weapon = vm.allocate(0x100)
        weapon_vtable = vm.allocate(0x100)
        vm.put_u32(self.weapon, weapon_vtable)

        def get_stat(args, ecx):
            self.stats.append(vm.string(args[0]))
            return self.case['weaponfull']
        vm.put_u32(weapon_vtable + 0xd4, self.seams.add_stub('GetStat', 4, get_stat))
        self.names = {}
        self.animator = vm.allocate(0x600)
        vm.put_u32(self.animator + 4, self.character)
        vm.checkpoint()

    def name(self, text):
        if text not in self.names:
            self.names[text] = self.vm.allocate(len(text) + 1)
            self.vm.write(self.names[text], text.encode() + b'\0')
        return self.names[text]

    def run(self, case):
        vm = self.vm
        vm.restore()
        self.names = {}
        self.case = case
        self.stats = []
        vm.write(self.character + 4, struct.pack('<H', 0xb if case['player'] else 0xc))
        red, green, blue = case['color']
        vm.write(self.chardata + 0x168, bytes([blue, green, red]))
        vm.put_u32(self.chardata + 0x16c, case['swipefull'])
        rows = []
        for n, step in enumerate(case['steps']):
            vm.put_u32(self.character + 0xd8, self.doing if step['doing'] else 0)
            vm.put_u32(self.doing, step['action'])
            vm.put_u32(self.doing + 0x48, self.attack if step['has_attack'] else 0)
            vm.put_u32(self.attack + 0xec, step['on'] & 0xffffffff)
            vm.put_u32(self.attack + 0xf0, step['off'] & 0xffffffff)
            vm.write(self.character + 0x5c, struct.pack('<h', step['frame']))
            weapon = step['weapon']
            vm.put_u32(self.character + 0x2b0, self.weapon if weapon else 0)
            vm.put_u32(self.weapon + 0x38, self.name(weapon) if weapon else 0)
            self.matrix = step['matrix']
            if not vm.u32(self.animator + RUNNING):
                vm.call(SETUP, this=self.animator, instruction_limit=5_000_000)
            swipe = self.animator + SWIPE
            row = [n]
            if vm.u32(self.animator + RUNNING):
                vm.call(UPDATE, this=swipe, instruction_limit=2_000_000)
            running = vm.u32(self.animator + RUNNING)
            row.append(int(running != 0))
            zero_tail = None
            if running:
                vm.call(BUILD, this=swipe, instruction_limit=2_000_000)
                row += list(struct.unpack('<3I', bytes(vm.uc.mem_read(swipe + 0x3a0, 12))))
                for base in (0x35c, 0x360):
                    row += list(struct.unpack(f'<{POINTS * 3}I',
                                              bytes(vm.uc.mem_read(vm.u32(swipe + base), POINTS * 12))))
                obj = vm.u32(swipe + 0x350)
                raw = bytes(vm.uc.mem_read(vm.u32(obj + 0xa4), VERTICES * 32))
                for v in range(STRIP):
                    x, y, z, _, argb = struct.unpack_from('<5I', raw, v * 32)
                    row += [x, y, z, argb]
                zero_tail = not any(raw[STRIP * 32:])
            rows.append((row, zero_tail))
        return rows


def spec(case):
    lo, hi = case['extents']
    out = [f"{int(case['player'])} {case['color'][0]} {case['color'][1]} {case['color'][2]} "
           f"{case['swipefull']} {case['weaponfull']} " + ' '.join(f'{v!r}' for v in (*lo, *hi)) +
           f" {len(case['steps'])}"]
    for s in case['steps']:
        out.append(f"{int(s['doing'])} {s['action']} {s['frame']} {s['on']} {s['off']} {int(s['has_attack'])} "
                   f"{s['weapon'] or '-'} " + ' '.join(f'{v!r}' for v in s['matrix']))
    return '\n'.join(out) + '\n'


def f32(value):
    return struct.unpack('<f', struct.pack('<f', value))[0]


def matrix(rng, angle, swing_axis):
    """A weapon pose: a turn about the swing axis on a random tilt, a scale
    and an offset, as float32 (row vectors)."""
    def rot(axis, a):
        c, s = math.cos(a), math.sin(a)
        if axis == 0:
            return [[1, 0, 0], [0, c, s], [0, -s, c]]
        if axis == 1:
            return [[c, 0, -s], [0, 1, 0], [s, 0, c]]
        return [[c, s, 0], [-s, c, 0], [0, 0, 1]]

    def mul(a, b):
        return [[sum(a[i][k] * b[k][j] for k in range(3)) for j in range(3)] for i in range(3)]
    m = mul(rot(swing_axis, angle), mul(rot(0, rng.uniform(-0.4, 0.4)), rot(1, rng.uniform(-0.4, 0.4))))
    scale = rng.choice([1.0, 1.0, 0.75, 1.3])
    offset = [rng.uniform(-40, 40), rng.uniform(-40, 40), rng.uniform(20, 90)]
    flat = []
    for r in range(3):
        flat += [f32(m[r][c] * scale) for c in range(3)] + [0.0]
    flat += [f32(v) for v in offset] + [1.0]
    return flat


def attack_steps(rng, frames, on, off, weapon=None, has_attack=True, stride=1, action=ATTACK):
    axis = rng.randrange(3)
    start = rng.uniform(-math.pi, math.pi)
    speed = rng.uniform(0.2, 0.9) * rng.choice([-1, 1])
    steps = []
    for frame in range(0, frames, stride):
        steps.append(dict(doing=True, action=action, frame=frame, on=on, off=off, has_attack=has_attack,
                          weapon=weapon, matrix=matrix(rng, start + speed * frame, axis)))
    return steps


def idle_steps(rng, count, weapon=None):
    axis = rng.randrange(3)
    return [dict(doing=rng.random() < 0.7, action=rng.choice([1, 3, 2]), frame=i, on=0, off=0, has_attack=False,
                 weapon=weapon, matrix=matrix(rng, 0.1 * i, axis)) for i in range(count)]


EXTENTS_SET = [
    ((-0.5, -2.4, -3.6), (0.5, 2.3, 24.5)),     # Skeleton.I3D "ogrokaxe" (the port's bounds)
    ((-30.0, -1.0, -1.0), (2.0, 1.0, 1.0)),      # long x, hilt below the pivot
    ((-1.0, -2.0, -1.0), (1.0, 40.0, 3.0)),      # long y
    ((-1.0, -5.0, -5.0), (1.0, 5.0, 5.0)),       # y and z tie: z
    ((-4.0, -4.0, -1.0), (4.0, 4.0, 1.0)),       # x and y tie: z
    ((0.0, 0.0, 0.0), (0.0, 0.0, 0.0)),          # no extent
    ((1e6, 1e6, 1e6), (-1e6, -1e6, -1e6)),       # an object without vertices
]


def cases(extra: int):
    rng = random.Random(1999)
    out = []

    def case(player, color, swipefull, weaponfull, extents, steps, objects=None):
        out.append(dict(player=player, color=color, swipefull=swipefull, weaponfull=weaponfull,
                        extents=extents, steps=steps, objects=objects or {'weapon': -1, 'sword': 2}))
    # A character's swings, as char.def sets them.
    for i, (color, window) in enumerate((((255, 0, 0), (3, 7)), ((200, 0, 230), (2, 6)), ((5, 5, 5), (7, 16)),
                                         ((50, 0, 75), (6, 16)), ((0, 25, 0), (0, 9)), ((155, 0, 155), (2, 8)))):
        steps = idle_steps(rng, 4) + attack_steps(rng, 14, *window) + idle_steps(rng, 6) + \
            attack_steps(rng, 18, *window) + idle_steps(rng, 12)
        case(False, color, i % 2, 0, EXTENTS_SET[i % len(EXTENTS_SET)], steps,
             objects={'ogrokaxe': 6} if i == 0 else None)
    # Every bounds shape, full and not.
    for i, extents in enumerate(EXTENTS_SET):
        case(False, (rng.randrange(256), rng.randrange(256), 255), i % 2, 0, extents,
             attack_steps(rng, 12, 2, 8) + idle_steps(rng, 3))
    # Windows: none, data missing, the opening frame skipped, off before on, an
    # attack outside the window, a different action.
    case(False, (255, 0, 0), 0, 0, EXTENTS_SET[0], attack_steps(rng, 10, 0, 0) + attack_steps(rng, 10, 3, 7, has_attack=False))
    case(False, (255, 0, 0), 0, 0, EXTENTS_SET[0], attack_steps(rng, 16, 3, 9, stride=2))
    case(False, (255, 0, 0), 0, 0, EXTENTS_SET[0], attack_steps(rng, 12, 6, 2) + attack_steps(rng, 12, 20, 30))
    case(False, (255, 0, 0), 0, 0, EXTENTS_SET[0], attack_steps(rng, 12, 2, 8, action=8))
    # A player: armed (SwipeFull from the weapon), unarmed, a weapon change mid-swing.
    case(True, (200, 0, 230), 0, 1, EXTENTS_SET[1], attack_steps(rng, 14, 2, 6, weapon='ShortSword') + idle_steps(rng, 4, 'ShortSword'))
    case(True, (200, 0, 230), 1, 0, EXTENTS_SET[0], idle_steps(rng, 3) + attack_steps(rng, 12, 2, 6) +
         attack_steps(rng, 12, 2, 6, weapon='Club'))
    case(True, (200, 0, 230), 0, 0, EXTENTS_SET[0], attack_steps(rng, 6, 1, 12, weapon='ShortSword') +
         attack_steps(rng, 12, 1, 12, weapon='BonePickaxe') + attack_steps(rng, 12, 1, 12, weapon='bonepickaxe'))
    # No SWIPECOLOR: never runs.
    case(False, (0, 0, 0), 0, 0, EXTENTS_SET[0], attack_steps(rng, 10, 2, 6))
    # Fuzz.
    for i in range(extra):
        steps = []
        player = rng.random() < 0.3
        weapon = rng.choice([None, 'ShortSword', 'Club']) if player else None
        while len(steps) < 60:
            if rng.random() < 0.6:
                on = rng.randrange(0, 10)
                steps += attack_steps(rng, rng.randrange(4, 20), on, on + rng.randrange(-2, 10), weapon=weapon,
                                      has_attack=rng.random() < 0.9, stride=rng.choice([1, 1, 1, 2]))
            else:
                steps += idle_steps(rng, rng.randrange(1, 8), weapon)
            if player and rng.random() < 0.15:
                weapon = rng.choice([None, 'ShortSword', 'Club'])
        case(player, (rng.randrange(256), rng.randrange(256), rng.randrange(256)), rng.randrange(2),
             rng.randrange(2), rng.choice(EXTENTS_SET), steps)
    return out


def run(executable: Path, output: Path, extra: int = 24, source: Path | None = None):
    """`source` replaces src/game/effects/weaponswipe.cpp (negative controls)."""
    output.mkdir(parents=True, exist_ok=False)
    binary = build_port(output, source)
    native = Native(executable)
    report = dict(retail_sha256=native.sha, cases=[], errors=0, frames=0, running_frames=0, strips=0)
    for index, case in enumerate(cases(extra)):
        native_rows = native.run(case)
        result = subprocess.run([str(binary)], input=spec(case), capture_output=True, text=True, check=True)
        port_rows = [[int(v) for v in line.split()] for line in result.stdout.splitlines()]
        errors = []
        if len(port_rows) != len(native_rows):
            errors.append(dict(kind='length', native=len(native_rows), port=len(port_rows)))
        for (nrow, zero_tail), prow in zip(native_rows, port_rows):
            if zero_tail is False:
                errors.append(dict(kind='tail', step=nrow[0]))
            if nrow != prow:
                first = next((i for i, (a, b) in enumerate(zip(nrow, prow)) if a != b), min(len(nrow), len(prow)))
                errors.append(dict(kind='frame', step=nrow[0], field=first, native=nrow[first:first + 4],
                                   port=prow[first:first + 4], lengths=[len(nrow), len(prow)]))
        running = sum(row[1] for row, _ in native_rows)
        report['frames'] += len(native_rows)
        report['running_frames'] += running
        report['strips'] += running
        report['cases'].append(dict(index=index, player=case['player'], color=case['color'],
                                    extents=case['extents'], frames=len(native_rows), running=running,
                                    stats=sorted(set(native.stats)), error_count=len(errors), errors=errors[:6]))
        report['errors'] += len(errors)
    report['seam_calls'] = sorted(set(native.seams.calls))
    report['status'] = 'pass' if report['errors'] == 0 else 'fail'
    (output / 'manifest.json').write_text(json.dumps(report, indent=1) + '\n')
    print(report['status'], 'cases', len(report['cases']), 'frames', report['frames'], 'running',
          report['running_frames'], 'errors', report['errors'])
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--cases', type=int, default=24)
    a = parser.parse_args()
    sys.exit(0 if run(a.executable, a.output, a.cases)['status'] == 'pass' else 1)
