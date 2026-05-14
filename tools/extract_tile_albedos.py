#!/usr/bin/env python3
"""Copy albedo PNGs out of a tile-dump folder into a separate folder.

The tile dumper (testmodes.cpp tile-dump path) writes four files per tile:
  XXXX_Name.png          <- albedo (what we want)
  XXXX_Name_alpha.png    <- alpha-mask visualisation
  XXXX_Name_depth.png    <- depth visualisation
  XXXX_Name.ztile        <- packed RGBA+depth binary

This script grabs just the albedo PNGs and copies them somewhere an
upscaler can chew on without filtering.

Usage:
  tools/extract_tile_albedos.py [SRC] [DST]
Defaults:
  SRC = build/real_tile_diagnostics_fit
  DST = build/real_tile_diagnostics_fit/albedo
"""

import argparse
import re
import shutil
import sys
from pathlib import Path

ALBEDO_RE = re.compile(r"^\d{4}_.+(?<!_alpha)(?<!_depth)\.png$")

def main() -> int:
    repo_root = Path(__file__).resolve().parents[1]
    default_src = repo_root / "build" / "real_tile_diagnostics_fit"
    default_dst = default_src / "albedo"

    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("src", nargs="?", default=str(default_src),
                    help=f"source folder (default: {default_src})")
    ap.add_argument("dst", nargs="?", default=str(default_dst),
                    help=f"destination folder (default: {default_dst})")
    args = ap.parse_args()

    src = Path(args.src)
    dst = Path(args.dst)
    if not src.is_dir():
        print(f"error: src is not a directory: {src}", file=sys.stderr)
        return 1

    dst.mkdir(parents=True, exist_ok=True)

    copied = 0
    skipped = 0
    for entry in sorted(src.iterdir()):
        if not entry.is_file():
            continue
        if ALBEDO_RE.match(entry.name):
            shutil.copy2(entry, dst / entry.name)
            copied += 1
        else:
            skipped += 1

    print(f"copied {copied} albedo PNGs -> {dst}")
    print(f"skipped {skipped} non-albedo entries")
    return 0

if __name__ == "__main__":
    sys.exit(main())
