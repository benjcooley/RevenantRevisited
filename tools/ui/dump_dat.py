#!/usr/bin/env python3
"""Decode and dump bitmaps from a Revenant TMulti .dat file as PNGs.

On-disk format (from src/resource.cpp + src/resourcehdr.h):

    FileResHdr {                       // 20 bytes
        uint32_t resmagic = 'CGSR';
        uint16_t topbm;                // bitmap-table entry count
        uint8_t  comptype;             // COMP_NONE=0 or COMP_ZIP=1
        uint8_t  version;
        uint32_t datasize;             // bytes of resource data after table
        uint32_t objsize;              // in-memory ptr alloc size
        uint32_t hdrsize;              // optional extra header bytes (skipped)
    };
    [hdrsize bytes of optional custom header]
    [topbm * uint32 — bitmap offset table into resource data]
    [datasize bytes — the TMultiData struct]

TMulti format (from src/multidat.h):

    TMultiData {
        int32_t numoffsets;
        OFFSET names[256];    // each OFFSET = uint32 relative to itself
        OFFSET offsets[256];  // each OFFSET = uint32 relative to itself
    };

Each bitmap stored at offsets[i].ptr() is a TBitmapData:

    int32_t  width, height, regx, regy;
    uint32_t flags, drawmode, keycolor;
    uint32_t aliassize;  OFFSET alias;
    uint32_t alphasize;  OFFSET alpha;
    uint32_t zbuffersize; OFFSET zbuffer;
    uint32_t normalsize; OFFSET normal;
    uint32_t palettesize; OFFSET palette;
    uint32_t datasize;
    <pixel data starts here>

Pixel data format depends on flags:
    0x0001 BM_8BIT   — 8 bit indexed (needs palette)
    0x0002 BM_15BIT  — 5-5-5 RGB
    0x0004 BM_16BIT  — 5-5-5 RGB (Revenant decoder uses 555 not 565)
    0x0008 BM_24BIT, 0x0010 BM_32BIT
    0x0100 BM_ALPHA  — alpha buffer separately stored
    0x4000 BM_COMPRESSED, 0x8000 BM_CHUNKED — unsupported

Usage:
    tools/ui/dump_dat.py <dat-file> [--out-dir DIR]

Example:
    tools/ui/dump_dat.py /tmp/plyr_dat/statusbarnotex.dat \\
                         --out-dir /tmp/dump
"""

from __future__ import annotations
import argparse
import struct
import sys
from pathlib import Path

try:
    from PIL import Image
except ImportError:
    print("error: Pillow required (pip install Pillow)", file=sys.stderr)
    sys.exit(2)

MAX_OFFSETS = 256
SIZEOF_OFFSET = 4
SIZEOF_HEADER = 4 + MAX_OFFSETS * SIZEOF_OFFSET * 2  # numoffsets + names + offsets

# TBitmapData layout: 18 uint32 fields before pixel data
BITMAP_HEADER_SIZE = 18 * 4

# BM_ flags
BM_8BIT       = 0x0001
BM_15BIT      = 0x0002
BM_16BIT      = 0x0004
BM_24BIT      = 0x0008
BM_32BIT      = 0x0010
BM_ALPHA      = 0x0100
BM_PALETTE    = 0x0200
BM_5BITPAL    = 0x1000
BM_COMPRESSED = 0x4000
BM_CHUNKED    = 0x8000


def offset_ptr(data: bytes, off_pos: int) -> int | None:
    """Read an OFFSET at off_pos, return absolute file offset of target."""
    o = struct.unpack_from("<I", data, off_pos)[0]
    if o == 0:
        return None
    return off_pos + o  # OFFSET is relative to its own location


def read_str(data: bytes, off: int, maxlen: int = 64) -> str:
    end = data.find(b"\0", off, off + maxlen)
    if end < 0:
        end = off + maxlen
    return data[off:end].decode("latin-1", errors="replace")


def decode_555(buf: bytes, w: int, h: int,
               keycolor: int = 0, magenta_key: bool = True) -> Image.Image:
    """Decode 5-5-5 RGB pixel data to RGBA image.

    Pixels equal to keycolor OR magenta (0x7c1f) become transparent.
    Magenta is the implicit transparency key in many 90s-era Revenant
    sprites — bitmap header's keycolor field is often unused.
    """
    img = Image.new("RGBA", (w, h))
    px = img.load()
    MAGENTA = 0x7c1f
    for y in range(h):
        for x in range(w):
            o = (y * w + x) * 2
            v = buf[o] | (buf[o + 1] << 8)
            if v == keycolor or (magenta_key and v == MAGENTA):
                px[x, y] = (0, 0, 0, 0)
            else:
                r = ((v >> 10) & 0x1F) << 3
                g = ((v >> 5) & 0x1F) << 3
                b = (v & 0x1F) << 3
                px[x, y] = (r, g, b, 255)
    return img


def decode_565(buf: bytes, w: int, h: int) -> Image.Image:
    """Decode 5-6-5 RGB pixel data to RGB image."""
    img = Image.new("RGB", (w, h))
    px = img.load()
    for y in range(h):
        for x in range(w):
            o = (y * w + x) * 2
            v = buf[o] | (buf[o + 1] << 8)
            r = ((v >> 11) & 0x1F) << 3
            g = ((v >> 5) & 0x3F) << 2
            b = (v & 0x1F) << 3
            px[x, y] = (r, g, b)
    return img


def decode_8bit(buf: bytes, w: int, h: int, palette_rgba: list[tuple]) -> Image.Image:
    """Decode 8-bit indexed pixel data using palette."""
    img = Image.new("RGB", (w, h))
    px = img.load()
    for y in range(h):
        for x in range(w):
            idx = buf[y * w + x]
            r, g, b, a = palette_rgba[idx]
            px[x, y] = (r, g, b)
    return img


def parse_palette(data: bytes, palette_off: int) -> list[tuple]:
    """Parse SPalette at given offset: 256 uint16 colors + 256 uint32 rgbcolors.

    Use the rgbcolors (Windows COLORREF: 0x00BBGGRR).
    """
    rgba = []
    # rgbcolors is 256*4 bytes, starts at palette_off + 256*2
    base = palette_off + 256 * 2
    for i in range(256):
        c = struct.unpack_from("<I", data, base + i * 4)[0]
        r = c & 0xFF
        g = (c >> 8) & 0xFF
        b = (c >> 16) & 0xFF
        rgba.append((r, g, b, 255))
    return rgba


def dump_bitmap(data: bytes, bm_off: int, name: str, out_path: Path) -> tuple[str, str]:
    """Decode and save one bitmap. Returns (status, info_str)."""
    width, height, regx, regy = struct.unpack_from("<iiii", data, bm_off)
    flags, drawmode, keycolor = struct.unpack_from("<III", data, bm_off + 16)
    info = f"{width}x{height} flags=0x{flags:x} kc=0x{keycolor:x}"

    if width <= 0 or height <= 0:
        return ("skip", info + " (invalid size)")

    # Pixel data starts at bm_off + 72
    px_off = bm_off + BITMAP_HEADER_SIZE
    palettesize, palette_rel = struct.unpack_from("<II", data, bm_off + 60)

    if flags & BM_COMPRESSED:
        return ("skip", info + " (compressed)")
    if flags & BM_CHUNKED:
        return ("skip", info + " (chunked)")

    try:
        if flags & BM_8BIT:
            if palette_rel == 0:
                return ("skip", info + " (8bit no palette)")
            palette_off = bm_off + 60 + 4  # palette OFFSET is at bm_off+64
            pal_target = palette_off + palette_rel
            palette = parse_palette(data, pal_target)
            buf = data[px_off:px_off + width * height]
            img = decode_8bit(buf, width, height, palette)
        elif flags & (BM_15BIT | BM_16BIT):
            buf = data[px_off:px_off + width * height * 2]
            # Disk format is 555 (BM_15BIT); engine Convert15to16 maps to
            # 565 at load time. Decode raw disk bytes as 555.
            img = decode_555(buf, width, height, keycolor)
        else:
            return ("skip", info + f" (unsupported bit-depth, flags=0x{flags:x})")
    except Exception as e:
        return ("err", info + f" (decode failed: {e})")

    img.save(out_path)
    return ("ok", info + f" -> {out_path}")


RESMAGIC = ord('C') | (ord('G') << 8) | (ord('S') << 16) | (ord('R') << 24)
FILERESHDR_SIZE = 20


def dump_dat(dat_path: Path, out_dir: Path):
    import json
    raw = dat_path.read_bytes()
    if len(raw) < FILERESHDR_SIZE:
        print(f"error: {dat_path} too small for FileResHdr", file=sys.stderr)
        return 1

    # FileResHdr
    resmagic, topbm, comptype, version, datasize, objsize, hdrsize = \
        struct.unpack_from("<IHBBIII", raw, 0)
    if resmagic != RESMAGIC:
        print(f"error: bad magic 0x{resmagic:08x} (expected CGSR)", file=sys.stderr)
        return 1
    if comptype != 0:
        print(f"error: compressed .dat (comptype={comptype}) — unsupported",
              file=sys.stderr)
        return 1

    # Skip optional custom header. Per src/resource.cpp:96 the seek is to
    # `hdrsize + sizeof(FileResHdr)`, so the optional header lives RIGHT
    # AFTER FileResHdr.
    body_off = FILERESHDR_SIZE + hdrsize
    # Then the bitmap offset table (topbm uint32s)
    table_off = body_off
    table_size = topbm * 4
    # Then the resource data (TMultiData)
    data_off = table_off + table_size
    data = raw[data_off:data_off + datasize]

    if len(data) < SIZEOF_HEADER:
        print(f"error: data section too small ({len(data)} < {SIZEOF_HEADER})",
              file=sys.stderr)
        return 1

    numoffsets = struct.unpack_from("<i", data, 0)[0]
    if numoffsets < 0 or numoffsets > MAX_OFFSETS:
        print(f"error: invalid numoffsets={numoffsets}", file=sys.stderr)
        return 1

    print(f"{dat_path.name}: topbm={topbm} hdrsize={hdrsize} "
          f"datasize={datasize} numoffsets={numoffsets}")
    out_dir.mkdir(parents=True, exist_ok=True)

    names_base = 4
    offsets_base = 4 + MAX_OFFSETS * SIZEOF_OFFSET

    manifest = {"source": dat_path.name, "count": numoffsets, "assets": []}
    def_lines = [
        f"# Asset manifest extracted from {dat_path.name}",
        f"# {numoffsets} asset(s)",
        f"DAT \"{dat_path.name}\"",
        "BEGIN",
    ]

    for i in range(numoffsets):
        name_pos = names_base + i * SIZEOF_OFFSET
        off_pos = offsets_base + i * SIZEOF_OFFSET
        name_target = offset_ptr(data, name_pos)
        bm_target = offset_ptr(data, off_pos)
        if name_target is None or bm_target is None:
            print(f"  [{i}] (null offset)")
            continue
        name = read_str(data, name_target)
        safe_name = "".join(c if c.isalnum() or c in "._-" else "_" for c in name)
        png_path = out_dir / f"{i:02d}_{safe_name}.png"

        # Read header fields for the manifest before decoding.
        width, height, regx, regy = struct.unpack_from("<iiii", data, bm_target)
        flags, drawmode, keycolor = struct.unpack_from("<III", data, bm_target + 16)

        status, info = dump_bitmap(data, bm_target, name, png_path)
        print(f"  [{i}] {name:20s} : {info}")

        asset = {
            "index": i,
            "name": name,
            "width": width,
            "height": height,
            "regx": regx,
            "regy": regy,
            "flags": flags,
            "flags_decoded": decode_flag_bits(flags),
            "drawmode": drawmode,
            "keycolor": keycolor,
            "status": status,
            "png": png_path.name if status == "ok" else None,
        }
        manifest["assets"].append(asset)

        def_lines.extend([
            f"  BITMAP \"{name}\"",
            "  BEGIN",
            f"    INDEX {i}",
            f"    SIZE {width} {height}",
            f"    REG {regx} {regy}",
            f"    FLAGS 0x{flags:x}  # {' '.join(decode_flag_bits(flags))}",
            f"    DRAWMODE 0x{drawmode:x}",
            f"    KEYCOLOR 0x{keycolor:x}",
            f"    STATUS \"{status}\"",
        ])
        if status == "ok":
            def_lines.append(f"    PNG \"{png_path.name}\"")
        def_lines.append("  END")

    def_lines.append("END")

    json_path = out_dir / f"{dat_path.stem}.json"
    def_path  = out_dir / f"{dat_path.stem}.def"
    json_path.write_text(json.dumps(manifest, indent=2))
    def_path.write_text("\n".join(def_lines) + "\n")
    print(f"  wrote {json_path}")
    print(f"  wrote {def_path}")

    return 0


def decode_flag_bits(flags: int) -> list[str]:
    bits = [
        (BM_8BIT, "BM_8BIT"), (BM_15BIT, "BM_15BIT"),
        (BM_16BIT, "BM_16BIT"), (BM_24BIT, "BM_24BIT"),
        (BM_32BIT, "BM_32BIT"), (BM_ALPHA, "BM_ALPHA"),
        (BM_PALETTE, "BM_PALETTE"), (BM_5BITPAL, "BM_5BITPAL"),
        (BM_COMPRESSED, "BM_COMPRESSED"), (BM_CHUNKED, "BM_CHUNKED"),
        (0x10000, "BM_UNKNOWN_0x10000"),  # statusbar.dat uses this
    ]
    return [name for bit, name in bits if flags & bit]


def main():
    ap = argparse.ArgumentParser(description="Dump Revenant TMulti .dat bitmaps as PNGs")
    ap.add_argument("dat", type=Path, help="path to .dat file")
    ap.add_argument("--out-dir", type=Path, default=Path("/tmp/dat_dump"),
                    help="output directory (default /tmp/dat_dump)")
    args = ap.parse_args()
    return dump_dat(args.dat, args.out_dir)


if __name__ == "__main__":
    sys.exit(main())
