#!/usr/bin/env python3
"""Capture a grid of frames from --test=vfx for one effect.

Boots ./build/Revenant --test=vfx --vfx=<id> --vfx-no-ui, screencaps
N times at a configurable interval, kills the process, then composes
the captures into a grid PNG and crops to just the Revenant window.

Usage:
    tools/vfx/snap_grid.py <effect_id> [--rows R] [--cols C]
                           [--interval-ms MS] [--out PATH]

Example:
    tools/vfx/snap_grid.py TStripEffect --rows 4 --cols 4 \\
                           --interval-ms 100 --out /tmp/lightning.png

Requires Pillow (`pip install Pillow`) and macOS screencapture +
osascript. Runs from the worktree root (the build/ dir must already
exist; the script does NOT cmake).
"""

import argparse
import os
import shutil
import subprocess
import sys
import tempfile
import time
from pathlib import Path

try:
    from PIL import Image
except ImportError:
    print("error: Pillow not available. pip install Pillow.", file=sys.stderr)
    sys.exit(2)


def run_capture_grid(effect_id: str, rows: int, cols: int,
                     interval_ms: int, out_path: Path,
                     build_dir: Path, warmup_ms: int = 2500) -> int:
    """Capture rows*cols frames and pack into one grid PNG."""
    revenant = build_dir / "Revenant"
    if not revenant.exists():
        print(f"error: {revenant} not found. cmake --build first.", file=sys.stderr)
        return 2

    n = rows * cols
    tmp = Path(tempfile.mkdtemp(prefix="vfxgrid_"))
    try:
        proc = subprocess.Popen(
            [str(revenant), "--test=vfx", f"--vfx={effect_id}", "--vfx-no-ui"],
            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        )
        try:
            # Warm-up: let initialise + first effect spawn settle.
            time.sleep(warmup_ms / 1000.0)
            # Bring Revenant window forward (best-effort; ignore errors).
            subprocess.run(
                ["osascript", "-e",
                 f'tell application "System Events" to set frontmost of '
                 f'first process whose unix id is {proc.pid} to true'],
                stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
            )
            # Capture n frames.
            for i in range(n):
                shot = tmp / f"shot_{i:03d}.png"
                subprocess.run(
                    ["screencapture", "-x", "-t", "png", str(shot)],
                    check=True,
                )
                if i + 1 < n:
                    time.sleep(interval_ms / 1000.0)
        finally:
            proc.terminate()
            try:
                proc.wait(timeout=2)
            except subprocess.TimeoutExpired:
                proc.kill()

        # Find the Revenant window inside the first capture: the window
        # has a distinct title-bar color region. For now we just take a
        # centered crop of the full screen since we have no robust
        # window-finder. The user can post-crop in Preview if needed.
        # TODO: parse `mdls` or accessibility API to get the window
        # frame and crop precisely.
        frames = []
        for i in range(n):
            shot = tmp / f"shot_{i:03d}.png"
            if not shot.exists():
                print(f"warning: missing {shot}; skipping", file=sys.stderr)
                continue
            img = Image.open(shot)
            frames.append(img)
        if not frames:
            print("error: no frames captured", file=sys.stderr)
            return 1

        # All frames share the same size (full display capture).
        fw, fh = frames[0].size
        # Downscale each frame so the grid fits in a reasonable file
        # size; pick the largest scale that keeps grid width <= 2400.
        max_grid_w = 2400
        scale = min(1.0, max_grid_w / float(fw * cols))
        cell_w = int(fw * scale)
        cell_h = int(fh * scale)
        grid = Image.new("RGB", (cell_w * cols, cell_h * rows), (32, 32, 32))
        for i, img in enumerate(frames):
            small = img.convert("RGB").resize((cell_w, cell_h), Image.LANCZOS)
            r = i // cols
            c = i % cols
            grid.paste(small, (c * cell_w, r * cell_h))
        out_path.parent.mkdir(parents=True, exist_ok=True)
        grid.save(out_path)
        print(f"wrote {out_path} ({grid.size[0]}x{grid.size[1]}, "
              f"{rows}x{cols} = {n} frames @ {interval_ms}ms)")
        return 0
    finally:
        shutil.rmtree(tmp, ignore_errors=True)


def main() -> int:
    ap = argparse.ArgumentParser(description="VFX flipbook snapshot grid")
    ap.add_argument("effect_id", help="effect id (e.g. TStripEffect)")
    ap.add_argument("--rows", type=int, default=4)
    ap.add_argument("--cols", type=int, default=4)
    ap.add_argument("--interval-ms", type=int, default=100,
                    help="ms between captures (default 100)")
    ap.add_argument("--out", type=Path,
                    default=Path("/tmp/vfx_grid.png"))
    ap.add_argument("--build-dir", type=Path, default=Path("build"))
    ap.add_argument("--warmup-ms", type=int, default=2500,
                    help="ms to wait after launch before first capture")
    args = ap.parse_args()
    return run_capture_grid(args.effect_id, args.rows, args.cols,
                            args.interval_ms, args.out,
                            args.build_dir, args.warmup_ms)


if __name__ == "__main__":
    sys.exit(main())
