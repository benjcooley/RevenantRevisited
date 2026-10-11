#!/usr/bin/env python3
"""Build a combat test arena into a save slot that retail and the port both load.

    make_arena.py BASE_SLOT OUT_SLOT [--level 2] [--origin 24,2] [--size 2] [--lights 3]

The arena is a flat square of floor tiles (KInFloor2, 128 x 128 units, 8 x 8
to a sector) with neutral lights over it, on sectors of a level the module
leaves empty (level 2's map stops at sector x 18: see `--origin`). The
sectors are written as retail sector files (docs/gameflow/forensics/
SAVE_GAME.md §11.6) into a copy of BASE_SLOT's CurMap, cloned from the
floor tile and light records of the Keep's own sector 2_1_0 with their
positions moved; nothing else in the slot changes. A sector holds no
characters, so its state hash is the empty one.

After loading the slot, a scenario puts Locke on the arena (`pos X Y Z L`)
and the monsters (`addat X Y Type`): script lines retail's Enter prompt
takes (`@pos ...`) as the port's --exec does, so both games start from the
same ground (docs/gameplay/COMBAT_ARENA.md).
"""
from __future__ import annotations

import argparse
import re
import shutil
import struct
import sys
import zipfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[1]
MODULE = REPO / 'data' / 'Modules' / 'Ahkuilon.rvm'
sys.path.insert(0, str(HERE.parents[0] / 'savefmt'))
from revsave import Decoder, retail_adler32  # noqa: E402

SECTOR = 1024                   # world units a sector (SECTORWSHIFT 10)
TILE = 128                      # KInFloor2: 8 x 8 walk cells
FLOOR_Z = 16                    # the Keep's floor tiles stand at 16
TEMPLATE_SECTOR = '2_1_0.DAT'
FLOOR_AT = (1088, 416)          # a KInFloor2 in it
LIGHT_AT = (1316, 564)          # a Light in it
LIGHT_RGB = (255, 240, 220)     # neutral, for comparing colour as well as motion
MAPINDEX_BASE = 0x5A000000      # well clear of the shipped map's indices


class Template:
    """One object record of a sector file, and where its fields sit."""

    def __init__(self, data: bytes, decoder: Decoder, slot: int):
        rec = decoder.records[slot]
        self.bytes = bytearray(data[rec.header:rec.bodystart + rec.blocksize])
        fields = {f.path: f for f in decoder.fields}
        self.offsets = {name: fields[f'obj[{slot}].{name}'].off - rec.header
                        for name in ('pos.x', 'pos.y', 'pos.z', 'mapindex', 'light.red', 'light.green',
                                     'light.blue') if f'obj[{slot}].{name}' in fields}

    def place(self, x: int, y: int, z: int, mapindex: int, rgb=None) -> bytes:
        out = bytearray(self.bytes)
        for name, value in (('pos.x', x), ('pos.y', y), ('pos.z', z), ('mapindex', mapindex)):
            struct.pack_into('<i', out, self.offsets[name], value)
        if rgb and 'light.red' in self.offsets:
            for name, value in zip(('light.red', 'light.green', 'light.blue'), rgb):
                out[self.offsets[name]] = value
        return bytes(out)


def templates(module: Path) -> tuple[Template, Template]:
    with zipfile.ZipFile(module) as z:
        member = next((n for n in z.namelist() if re.fullmatch(r'Map/' + TEMPLATE_SECTOR, n, re.I)), None)
        if not member:
            raise SystemExit(f'{module}: no Map/{TEMPLATE_SECTOR}')
        data = z.read(member)
    d = Decoder(data)
    d.sector()
    fields = {f.path: f.value for f in d.fields}
    found = {}
    for slot, rec in enumerate(d.records):
        if rec is None:
            continue
        at = (fields.get(f'obj[{slot}].pos.x'), fields.get(f'obj[{slot}].pos.y'))
        if at == FLOOR_AT and 'floor' not in found:
            found['floor'] = Template(data, d, slot)
        elif at == LIGHT_AT and 'light' not in found:
            found['light'] = Template(data, d, slot)
    if len(found) != 2:
        raise SystemExit(f'{TEMPLATE_SECTOR}: no floor tile at {FLOOR_AT} or light at {LIGHT_AT}')
    return found['floor'], found['light']


def sector_file(level: int, sx: int, sy: int, records: list[bytes]) -> bytes:
    statehash = retail_adler32([struct.pack('<4i', level, sx, sy, len(records))]) or 0xF0F0F0F0
    return b'MAP ' + struct.pack('<iIi', 15, statehash, len(records)) + b''.join(records)


def add_arena(slot: Path, level: int = 2, origin: tuple[int, int] = (24, 2), size: int = 2, lights: int = 3,
              module: Path = MODULE) -> dict:
    """Write the arena's sectors into `slot`'s CurMap; returns where it is."""
    curmap = slot / 'CurMap'
    curmap.mkdir(exist_ok=True)
    floor, light = templates(module)
    x0, y0 = origin[0] * SECTOR, origin[1] * SECTOR
    span = size * SECTOR
    index = MAPINDEX_BASE
    per_sector: dict[tuple[int, int], list[bytes]] = {}

    def add(x, y, record_fn):
        nonlocal index
        key = (x // SECTOR, y // SECTOR)
        per_sector.setdefault(key, []).append(record_fn(index))
        index += 1

    for ty in range(span // TILE):
        for tx in range(span // TILE):
            cx, cy = x0 + tx * TILE + TILE // 2, y0 + ty * TILE + TILE // 2
            add(cx, cy, lambda i, cx=cx, cy=cy: floor.place(cx, cy, FLOOR_Z, i))
    step = span // lights
    for ly in range(lights):
        for lx in range(lights):
            cx, cy = x0 + lx * step + step // 2, y0 + ly * step + step // 2
            add(cx, cy, lambda i, cx=cx, cy=cy: light.place(cx, cy, FLOOR_Z + 44, i, LIGHT_RGB))

    for (sx, sy), records in sorted(per_sector.items()):
        name = curmap / f'{level}_{sx}_{sy}.DAT'
        if name.exists():
            raise SystemExit(f'{name.name} exists in the base slot: pick an empty --origin')
        name.write_bytes(sector_file(level, sx, sy, records))
    return dict(level=level, x0=x0, y0=y0, size=span, floor=FLOOR_Z + 1,
                centre=[x0 + span // 2, y0 + span // 2], sectors=sorted(per_sector))


def build(base: Path, out: Path, **arena) -> dict:
    if out.exists():
        shutil.rmtree(out)
    shutil.copytree(base, out)
    return add_arena(out, **arena)


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('base', type=Path, help='a save slot directory (game.sav, CurMap/) on the Keep level')
    p.add_argument('out', type=Path, help='the arena slot to write')
    p.add_argument('--level', type=int, default=2)
    p.add_argument('--origin', default='24,2', help='the arena\'s first sector, x,y (empty in the base map)')
    p.add_argument('--size', type=int, default=2, help='sectors a side')
    p.add_argument('--lights', type=int, default=3, help='lights a side')
    a = p.parse_args()
    info = build(a.base.resolve(), a.out.resolve(), level=a.level,
                 origin=tuple(int(v) for v in a.origin.split(',')), size=a.size, lights=a.lights)
    print(f"arena: level {info['level']}, {info['size']} x {info['size']} from ({info['x0']},{info['y0']}), "
          f"floor z {info['floor']}, centre {info['centre']}, sectors {info['sectors']}")


if __name__ == '__main__':
    main()
