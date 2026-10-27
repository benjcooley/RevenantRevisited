#!/usr/bin/env python3
"""Copy upscaled tile albedos (AI 4x + hand-finished "Final" picks) into the
Revisited overlay so the engine can pick them up as tile texture overrides.

Source layout (external working tree, e.g. D:\\Игры\\Revinant Revisited\\textures_sorted):

    upscaled_x4/<area>/<category>/<NNNN>_<Name>.png    4x albedo (RGBA)
    upscaled_x4/<area>/<category>/alpha|depth|Remade/  masks, visualisations, raw AI output - skipped
    Final/<NNNN>_<Name>.png                             hand-picked / remade versions (win over the 4x base)

Destination (overlay, mirrored as-is):

    <overlay>/Imagery/<area>/<category>/<NNNN>_<Name>.png
    <overlay>/Imagery/Final/<NNNN>_<Name>.png

The engine scans the overlay recursively and resolves an asset by
"<NNNN>_<Name>" (see src/tiletexoverride.cpp); entries under a `Final/`
folder win over the plain 4x set, so hand-finished tiles shadow the base ones.

These files are local test content: they are NOT meant to be committed. The
script never writes outside the overlay and refuses to copy unless the
destination resolves under <repo>/revisited/resources.

Usage:
  python revisited/builders/import_tile_textures.py [--src DIR] [--dst DIR] [--dry-run]
"""

import argparse
import os
import re
import shutil
import sys

TILE_RE = re.compile(r"^\d{4}_[A-Za-z0-9_.-]+\.png$")
SKIP_DIRS = {"alpha", "depth", "Remade"}


def tiles_under(root: str):
    """Yield albedo PNGs under root, skipping mask/visualisation dirs."""
    for dirpath, dirnames, filenames in os.walk(root):
        dirnames[:] = [d for d in dirnames if d not in SKIP_DIRS]
        for f in filenames:
            if TILE_RE.match(f):
                yield os.path.join(dirpath, f)


def main() -> int:
    repo = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    default_dst = os.path.join(repo, "revisited", "resources", "Imagery")

    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--src", default=r"D:\Игры\Revinant Revisited\textures_sorted",
                    help="textures_sorted root (contains upscaled_x4/ and Final/)")
    ap.add_argument("--dst", default=default_dst,
                    help="overlay destination (default: <repo>/revisited/resources/Imagery)")
    ap.add_argument("--dry-run", action="store_true", help="report only, copy nothing")
    args = ap.parse_args()

    dst = os.path.abspath(args.dst)
    resources = os.path.abspath(os.path.join(repo, "revisited", "resources"))
    if not dst.startswith(resources + os.sep):
        print(f"refusing: destination {dst} is outside {resources}", file=sys.stderr)
        return 2

    jobs = []  # (src, rel_dst)
    base = os.path.join(args.src, "upscaled_x4")
    if os.path.isdir(base):
        for src in tiles_under(base):
            rel = os.path.relpath(src, base)
            jobs.append((src, os.path.join(dst, rel)))
    else:
        print(f"warning: no upscaled_x4 under {args.src}", file=sys.stderr)

    final = os.path.join(args.src, "Final")
    if os.path.isdir(final):
        for f in sorted(os.listdir(final)):
            src = os.path.join(final, f)
            if os.path.isfile(src) and TILE_RE.match(f):
                jobs.append((src, os.path.join(dst, "Final", f)))

    copied = skipped = 0
    bytes_copied = 0
    for src, out in jobs:
        size = os.path.getsize(src)
        if os.path.exists(out) and os.path.getsize(out) == size:
            skipped += 1
            continue
        if not args.dry_run:
            os.makedirs(os.path.dirname(out), exist_ok=True)
            shutil.copy2(src, out)
        copied += 1
        bytes_copied += size

    verb = "would copy" if args.dry_run else "copied"
    print(f"{verb} {copied} files ({bytes_copied / 1e6:.1f} MB), "
          f"up to date {skipped}, total {len(jobs)}")
    print(f"destination: {dst}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
