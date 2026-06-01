#!/usr/bin/env python3
"""Capture --test=vfx against each diagnostic backdrop (Black/LtGray/
Forest/Dungeon) for a single effect, then compose into one 2x2 grid PNG.

Boots ./build/Revenant --test=vfx --vfx=<id> --vfx-no-ui --vfx-bg=<bg>
once per backdrop, captures the Revenant window via Quartz windowID, kills
the process, and arranges the four captures into a 2x2 grid labeled
Black / LtGray / Forest / Dungeon.

Usage:
    tools/vfx/snap_bg_check.py <effect_id> [--out PATH] [--warmup-ms MS]

Example:
    tools/vfx/snap_bg_check.py TWeaponSwipe \\
        --out docs/vfx/captures/bg_check_S09.png
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
    from PIL import Image, ImageDraw, ImageFont
except ImportError:
    print("error: Pillow not available. pip install Pillow.", file=sys.stderr)
    sys.exit(2)


def capture_one(revenant: Path, effect_id: str, bg: str,
                out_png: Path, warmup_ms: int) -> int:
    proc = subprocess.Popen(
        [str(revenant), "--test=vfx", f"--vfx={effect_id}",
         "--vfx-no-ui", f"--vfx-bg={bg}"],
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
    )
    try:
        time.sleep(warmup_ms / 1000.0)
        wid = None
        try:
            import Quartz  # type: ignore
            wl = Quartz.CGWindowListCopyWindowInfo(
                Quartz.kCGWindowListOptionOnScreenOnly, Quartz.kCGNullWindowID)
            for w in wl:
                if 'Revenant' in (w.get('kCGWindowOwnerName') or ''):
                    wid = w.get('kCGWindowNumber')
                    break
        except Exception as e:
            print(f"warning: Quartz unavailable ({e}); falling back to "
                  f"full-screen capture", file=sys.stderr)
        if wid is not None:
            subprocess.run(
                ["screencapture", "-x", "-t", "png", "-l", str(wid),
                 str(out_png)], check=True)
        else:
            subprocess.run(
                ["screencapture", "-x", "-t", "png", str(out_png)], check=True)
        return 0
    finally:
        proc.terminate()
        try:
            proc.wait(timeout=2)
        except subprocess.TimeoutExpired:
            proc.kill()


def main() -> int:
    ap = argparse.ArgumentParser(description="VFX BG-cycle 2x2 capture")
    ap.add_argument("effect_id", help="effect id (e.g. TWeaponSwipe)")
    ap.add_argument("--out", type=Path,
                    default=Path("docs/vfx/captures/bg_check.png"))
    ap.add_argument("--build-dir", type=Path, default=Path("build"))
    ap.add_argument("--warmup-ms", type=int, default=2500)
    args = ap.parse_args()

    revenant = args.build_dir / "Revenant"
    if not revenant.exists():
        print(f"error: {revenant} not found.", file=sys.stderr)
        return 2

    tmp = Path(tempfile.mkdtemp(prefix="vfxbg_"))
    try:
        bgs = ["black", "ltgray", "forest", "dungeon"]
        shots = []
        for bg in bgs:
            shot = tmp / f"{bg}.png"
            print(f"capturing {bg} ...", flush=True)
            rc = capture_one(revenant, args.effect_id, bg, shot, args.warmup_ms)
            if rc != 0 or not shot.exists():
                print(f"error: capture failed for {bg}", file=sys.stderr)
                return 1
            shots.append((bg, Image.open(shot).convert("RGB")))

        # Normalize cell size to the smallest capture (window sizes may
        # differ slightly between launches if a different monitor is in
        # use). Cap to keep file size reasonable.
        min_w = min(img.size[0] for _, img in shots)
        min_h = min(img.size[1] for _, img in shots)
        max_cell_w = 1200
        scale = min(1.0, max_cell_w / float(min_w))
        cell_w = int(min_w * scale)
        cell_h = int(min_h * scale)

        # 2x2 grid: top-left Black, top-right LtGray, bot-left Forest, bot-right Dungeon
        grid = Image.new("RGB", (cell_w * 2, cell_h * 2), (32, 32, 32))
        try:
            font = ImageFont.truetype("/System/Library/Fonts/Helvetica.ttc", 32)
        except Exception:
            font = ImageFont.load_default()
        for i, (bg, img) in enumerate(shots):
            small = img.resize((cell_w, cell_h), Image.LANCZOS)
            r = i // 2
            c = i % 2
            x0 = c * cell_w
            y0 = r * cell_h
            grid.paste(small, (x0, y0))
            d = ImageDraw.Draw(grid)
            label = bg.upper()
            pad = 6
            tw = int(font.getlength(label)) if hasattr(font, "getlength") else 8 * len(label)
            d.rectangle([x0 + 8, y0 + 8, x0 + 8 + tw + pad * 2, y0 + 8 + 40],
                        fill=(0, 0, 0, 200))
            d.text((x0 + 8 + pad, y0 + 10), label, fill=(255, 255, 255), font=font)

        args.out.parent.mkdir(parents=True, exist_ok=True)
        grid.save(args.out)
        print(f"wrote {args.out} ({grid.size[0]}x{grid.size[1]})")
        return 0
    finally:
        shutil.rmtree(tmp, ignore_errors=True)


if __name__ == "__main__":
    sys.exit(main())
