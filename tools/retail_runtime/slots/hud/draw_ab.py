#!/usr/bin/env python3
"""Retail's 2D Put on explicit bytes: the unit oracle for the port's blitters.

A case is a destination surface (pixel format, size, initial pixels) and a
source bitmap given as the raw bytes of a retail TBitmap (the 0x48-byte
header with its relative offsets, then the data, alpha, alias, palette --
the port's TBitmapData has the same layout). Retail creates a real
TDDSurface in that format (`TDDSurface::Initialize` `0x004a5740`, system
memory), writes the initial pixels, and calls `TSurface::Put` `0x004bd680`
(x, y, bitmap, drawmode, color) -- the same path a pane's compose takes,
down through Draw `0x004ad0d0` and the routine selector `0x004ad1d0`. The
result is the destination's pixels.

Formats (TSurface `+0x38` / TBitmap flags): `565` 0x4, `555` 0x2,
`4444` 0x10000, `1555` 0x20000. Creation flags for `0x004a5740`: 0x200
(system memory) | 0x4 / 0x2 / 0x8 / 0x10.

    draw_ab.py EXE --serve       JSONL {"id", "case"} -> {"pixels": hex}
    draw_ab.py EXE --bitmaps     dump the StatusBar.dat bitmaps as cases' sources

Case: {"dest": {"format": "4444", "width": 64, "height": 64, "pixels": hex},
"bitmap": hex, "x": 0, "y": 0, "mode": 8192}
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(HERE.parents[1]))
from hudworld import HudWorld  # noqa: E402

SURFACE_CONSTRUCT, SURFACE_INITIALIZE, PUT = 0x004bcb00, 0x004a5740, 0x004bd680
TDDSURFACE_VTABLE, TDDSURFACE_SIZE = 0x005a3980, 0x78
FIND_IN_ARCHIVE = 0x0046d710                     # thiscall archive blob, (name)
CREATE_FLAGS = {'565': 0x204, '555': 0x202, '4444': 0x208, '1555': 0x210}
SURFACE_FORMAT = {'565': 0x4, '555': 0x2, '4444': 0x10000, '1555': 0x20000}
SCHEMA = 'hud.draw-put.v1'
HEADER_SIZE = 0x48


def bitmap_size(data):
    """Length of a TBitmap from its header: the furthest relative block end."""
    fields = struct.unpack_from('<18I', data, 0)
    end = HEADER_SIZE + fields[17]                         # datasize
    for size_index, offset_index in ((7, 8), (9, 10), (11, 12), (13, 14), (15, 16)):
        size, offset = fields[size_index], fields[offset_index]
        if size and offset:
            end = max(end, 4 * offset_index + offset + size)
    return end


class DrawFixture:
    def __init__(self, executable):
        started = time.perf_counter()
        self.world = HudWorld.open(executable)
        self.vm = self.world.vm
        self.world.checkpoint()
        self.setup_ms = (time.perf_counter() - started) * 1000

    def surface(self, fmt, width, height):
        vm = self.vm
        obj = vm.allocate(TDDSURFACE_SIZE)
        self.world.call(SURFACE_CONSTRUCT, this=obj)
        vm.put_u32(obj, TDDSURFACE_VTABLE)
        vm.put_u32(obj + 0x1a * 4, 0)
        self.world.call(SURFACE_INITIALIZE, (width, height, CREATE_FLAGS[fmt], vm.u32(obj + 0x10), 0),
                        this=obj)
        vm.put_u32(obj + 0x1c * 4, 1)
        if vm.u32(obj + 0x38) != SURFACE_FORMAT[fmt]:
            raise RuntimeError(f'Surface format flags 0x{vm.u32(obj + 0x38):x}, wanted {fmt}')
        return obj, self.world.ddraw.surface(vm.u32(obj + 0x68))

    def run(self, case):
        world, vm = self.world, self.vm
        world.restore()
        dest = case['dest']
        width, height = dest['width'], dest['height']
        obj, ddsurface = self.surface(dest['format'], width, height)
        pixels = bytes.fromhex(dest['pixels'])
        if len(pixels) != width * height * 2:
            raise ValueError('dest.pixels must be width*height 16-bit pixels')
        for y in range(height):
            vm.write(ddsurface.memory + y * ddsurface.pitch, pixels[y * width * 2:(y + 1) * width * 2])
        data = bytes.fromhex(case['bitmap'])
        bitmap = vm.allocate(len(data))
        vm.write(bitmap, data)
        world.call(PUT, (case['x'] & 0xffffffff, case['y'] & 0xffffffff, bitmap,
                         case['mode'] & 0xffffffff, 0), this=obj, instruction_limit=50_000_000)
        out = b''.join(bytes(vm.uc.mem_read(ddsurface.memory + y * ddsurface.pitch, width * 2))
                       for y in range(height))
        return dict(schema=SCHEMA, pixels=out.hex())

    def archive_bitmaps(self, archive, names):
        """{name: TBitmap bytes} through retail's loader and lookup."""
        world, vm = self.world, self.vm
        world.restore()
        name = vm.allocate(64)
        vm.write(name, archive.encode() + b'\0')
        blob = world.call(0x0047f670, (name, 0xffffffff, 0), instruction_limit=200_000_000)
        result = {}
        for entry in names:
            vm.write(name, entry.encode() + b'\0')
            pointer = world.call(FIND_IN_ARCHIVE, (name,), this=blob)
            header = bytes(vm.uc.mem_read(pointer, HEADER_SIZE))
            result[entry] = bytes(vm.uc.mem_read(pointer, bitmap_size(header))).hex()
        return result


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('executable', type=Path)
    parser.add_argument('--serve', action='store_true')
    parser.add_argument('--bitmaps', action='store_true')
    args = parser.parse_args()
    fixture = DrawFixture(args.executable)
    if args.bitmaps:
        print(json.dumps(fixture.archive_bitmaps('StatusBar.dat', [
            'bars', 'BackPanel', 'Ring', 'HealthIcon', 'ManaIcon', 'FatigueIcon'])))
        return
    sys.stdout.write(json.dumps(dict(id='hello', ok=True, schema=SCHEMA, retail_sha256=fixture.world.sha,
                                     setup_ms=fixture.setup_ms)) + '\n')
    sys.stdout.flush()
    for raw in sys.stdin:
        if not raw.strip():
            continue
        request = json.loads(raw)
        try:
            response = dict(id=request.get('id'), ok=True, result=fixture.run(request['case']))
        except Exception as error:
            response = dict(id=request.get('id'), ok=False, error=f'{type(error).__name__}: {error}')
        sys.stdout.write(json.dumps(response) + '\n')
        sys.stdout.flush()


if __name__ == '__main__':
    main()
