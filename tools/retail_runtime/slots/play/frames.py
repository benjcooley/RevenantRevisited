"""Frames: the 640x480 RGB565 front buffer as PNG.

RGB565 widens to RGB888 by bit replication (r << 3 | r >> 2, ...), as the
HUD slot's `hudscene.png` does, through a 64K-entry table; Pillow encodes.
`write_png` writes a temporary file and renames it over the target, so a
reader polling the path (the watch viewer) never sees half a file.
"""
from __future__ import annotations

import os
from array import array
from pathlib import Path

from PIL import Image

WIDTH, HEIGHT = 640, 480


def _expand(v):
    r, g, b = v >> 11, (v >> 5) & 0x3f, v & 0x1f
    return bytes(((r << 3) | (r >> 2), (g << 2) | (g >> 4), (b << 3) | (b >> 2)))


RGB888 = [_expand(v) for v in range(1 << 16)]


def rgb888(rgb565):
    """Packed little-endian RGB565 pixels -> packed RGB888 bytes."""
    return b''.join(map(RGB888.__getitem__, array('H', rgb565)))


def write_png(path, rgb565, width=WIDTH, height=HEIGHT, level=6):
    """`rgb565`: rows packed (width * 2 bytes each). `level`: zlib level."""
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_name(f'.{path.name}.{os.getpid()}.tmp')
    Image.frombytes('RGB', (width, height), rgb888(rgb565)).save(temporary, 'PNG', compress_level=level)
    os.replace(temporary, path)
    return path
