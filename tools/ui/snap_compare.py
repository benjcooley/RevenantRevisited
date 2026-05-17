#!/usr/bin/env python3
"""Capture frames of a UI test mode + compose A/B against a reference image.

Based on tools/vfx/snap_grid.py. Boots ./build/Revenant --test=<mode>,
captures N frames at a configurable interval (to see any animation
cycle), auto-crops each to the Revenant window region, then composites
the frame strip alongside a reference image into one PNG for visual
A/B comparison.

Usage:
    tools/ui/snap_compare.py <mode> <reference.png>
                             [--frames N] [--interval-ms MS]
                             [--out PATH]

Example:
    tools/ui/snap_compare.py ui-plyrstatusbar docs/ui/plyr_stats_panel.png \\
                             --frames 4 --interval-ms 1500 \\
                             --out /tmp/plyr_ab.png

Requires Pillow + numpy.
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
    import numpy as np
except ImportError:
    print("error: Pillow + numpy required (pip install Pillow numpy)",
          file=sys.stderr)
    sys.exit(2)

try:
    from Quartz import (
        CGWindowListCopyWindowInfo,
        kCGWindowListOptionAll,
        kCGNullWindowID,
    )
    HAVE_QUARTZ = True
except ImportError:
    HAVE_QUARTZ = False


def find_revenant_window_id() -> int | None:
    """Return the CGWindowID of the Revenant window, or None."""
    if not HAVE_QUARTZ:
        return None
    infos = CGWindowListCopyWindowInfo(kCGWindowListOptionAll, kCGNullWindowID)
    for info in infos:
        if info.get("kCGWindowOwnerName", "") == "Revenant":
            return info["kCGWindowNumber"]
    return None


def find_window_crop(img: Image.Image) -> Image.Image:
    """Auto-crop to the Revenant window in a full-screen capture.

    The Revenant test mode paints a near-uniform dark background plus
    a small panel mockup in the top-area. The IDE / terminal panes are
    also dark but have HIGH local variance (text, icons, syntax
    highlighting). We exploit that:

      1. Downsample for speed.
      2. Compute per-pixel "ui-likeness": dark AND low local variance.
      3. Find the largest connected rectangle (by axis-aligned run-length
         projection) of UI-likeness.

    Returns the cropped image, or the original if no good region found.
    """
    src = img.convert("RGB")
    a = np.array(src)
    H, W = a.shape[:2]

    # Downsample 4x for speed; variance computed on the smaller buffer.
    SCALE = 4
    small = src.resize((W // SCALE, H // SCALE), Image.LANCZOS)
    sa = np.array(small).astype(np.int32)
    sH, sW = sa.shape[:2]

    # Mean luminance.
    lum = (sa[:, :, 0] + sa[:, :, 1] + sa[:, :, 2]) // 3

    # Local variance over a small window — approximate via (lum^2 mean) - (lum mean)^2
    # using box-blur convolution. Use Pillow's filter for speed.
    from PIL import ImageFilter
    small_l = small.convert("L")
    blur = small_l.filter(ImageFilter.BoxBlur(8))
    blur_a = np.array(blur).astype(np.int32)
    blur2 = small_l.filter(ImageFilter.BoxBlur(8))
    # Variance proxy: per-pixel |luminance - local-mean|. Plenty for our use.
    deviation = np.abs(lum - blur_a)

    # UI-like: dark AND smooth (low deviation from local mean).
    dark = lum < 60
    smooth = deviation < 8
    ui_mask = dark & smooth

    # Row / col projections — find the largest contiguous run of rows /
    # cols that are MOSTLY UI-like.
    row_in_count = ui_mask.sum(axis=1)
    col_in_count = ui_mask.sum(axis=0)
    row_thresh = max(50, sW // 12)
    col_thresh = max(50, sH // 12)
    row_in = row_in_count > row_thresh
    col_in = col_in_count > col_thresh

    def longest_run(flags):
        best_lo, best_hi, best_len = 0, 0, 0
        cur_lo = None
        for i, v in enumerate(flags):
            if v and cur_lo is None:
                cur_lo = i
            elif not v and cur_lo is not None:
                if i - cur_lo > best_len:
                    best_len = i - cur_lo
                    best_lo, best_hi = cur_lo, i
                cur_lo = None
        if cur_lo is not None and len(flags) - cur_lo > best_len:
            best_lo, best_hi = cur_lo, len(flags)
        return best_lo, best_hi

    y0s, y1s = longest_run(row_in)
    x0s, x1s = longest_run(col_in)

    # Scale back up. Pad outwards a few pixels to catch the panel that
    # sits AT the top of the window (lighter than the bg).
    if y1s > y0s and x1s > x0s:
        y0, y1 = y0s * SCALE, y1s * SCALE
        x0, x1 = x0s * SCALE, x1s * SCALE
        # Pad upward to catch any UI elements right at the top edge.
        PAD = 4
        y0 = max(0, y0 - PAD)
        return src.crop((x0, y0, x1, y1))
    return src


def capture_frames(mode: str, n: int, interval_ms: int,
                   build_dir: Path, warmup_ms: int) -> list[Image.Image]:
    revenant = build_dir / "Revenant"
    if not revenant.exists():
        raise FileNotFoundError(f"{revenant} not found. cmake --build first.")

    tmp = Path(tempfile.mkdtemp(prefix="uisnap_"))
    try:
        proc = subprocess.Popen(
            [str(revenant), f"--test={mode}"],
            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        )
        try:
            time.sleep(warmup_ms / 1000.0)
            time.sleep(0.5)
            # Find the Revenant window via CGWindowList (no Accessibility
            # permission needed, just Screen Recording which screencapture
            # already has). Fall back to full-screen + auto-crop if Quartz
            # isn't available.
            win_id = find_revenant_window_id()
            if win_id is None:
                print("warning: Quartz unavailable or window not found; "
                      "falling back to full-screen capture",
                      file=sys.stderr)
            else:
                print(f"[snap] Revenant window id = {win_id}")
            frames = []
            for i in range(n):
                shot = tmp / f"shot_{i:03d}.png"
                if win_id is not None:
                    # -l <id>: capture just that window (includes shadow
                    # by default; -o omits shadow + extra chrome).
                    subprocess.run(
                        ["screencapture", "-x", "-o",
                         "-l", str(win_id), "-t", "png", str(shot)],
                        check=True)
                else:
                    subprocess.run(
                        ["screencapture", "-x", "-t", "png", str(shot)],
                        check=True)
                frames.append(Image.open(shot).copy())
                if i + 1 < n:
                    time.sleep(interval_ms / 1000.0)
        finally:
            proc.terminate()
            try:
                proc.wait(timeout=2)
            except subprocess.TimeoutExpired:
                proc.kill()
        return frames, (win_id is not None)
    finally:
        shutil.rmtree(tmp, ignore_errors=True)


def compose_ab(ref: Image.Image, mockup_frames: list[Image.Image],
               out_path: Path, frame_label_fmt: str = "t={i}") -> None:
    """Compose the reference image above a row of mockup frames.

    Layout:
        +-----------------------------------+
        | REFERENCE (retail)                |
        | [ ref image, scaled to row H ]    |
        +-----------------------------------+
        | MOCKUP (current build) — animation|
        | [frame 0] [frame 1] [frame 2] ... |
        +-----------------------------------+
    """
    LABEL_H = 24
    GAP = 8
    BG = (28, 28, 32)

    # Pick a row height: 2x reference height for retina legibility,
    # min 220, max 600.
    row_h = max(220, min(600, ref.height * 2))

    def resize_h(img: Image.Image, h: int) -> Image.Image:
        if img.height == h:
            return img.convert("RGB")
        w = max(1, int(img.width * (h / img.height)))
        return img.convert("RGB").resize((w, h), Image.LANCZOS)

    ref_r = resize_h(ref, row_h)
    mocks_r = [resize_h(f, row_h) for f in mockup_frames]
    mocks_row_w = sum(m.width for m in mocks_r) + GAP * max(0, len(mocks_r) - 1)
    total_w = max(ref_r.width, mocks_row_w) + GAP * 2

    total_h = LABEL_H + ref_r.height + LABEL_H + row_h + GAP
    out = Image.new("RGB", (total_w, total_h), BG)

    draw = ImageDraw.Draw(out)
    try:
        font = ImageFont.truetype(
            "/System/Library/Fonts/Supplemental/Arial.ttf", 14)
        font_small = ImageFont.truetype(
            "/System/Library/Fonts/Supplemental/Arial.ttf", 11)
    except Exception:
        font = font_small = ImageFont.load_default()

    # Reference row
    y = 4
    draw.text((GAP, y), "REFERENCE (retail)", (220, 220, 220), font=font)
    out.paste(ref_r, (GAP, LABEL_H))

    # Mockup row
    y = LABEL_H + ref_r.height + 4
    draw.text((GAP, y),
              f"MOCKUP (current build) — {len(mocks_r)} frames",
              (220, 220, 220), font=font)
    x = GAP
    yy = LABEL_H + ref_r.height + LABEL_H
    for i, img in enumerate(mocks_r):
        out.paste(img, (x, yy))
        draw.text((x + 2, yy + 2), frame_label_fmt.format(i=i),
                  (180, 220, 255), font=font_small)
        x += img.width + GAP

    out.save(out_path)
    print(f"wrote {out_path} ({out.size[0]}x{out.size[1]})")


def main() -> int:
    ap = argparse.ArgumentParser(description="UI test-mode A/B snapshot tool")
    ap.add_argument("mode", help="test mode name (e.g. ui-plyrstatusbar)")
    ap.add_argument("reference", type=Path,
                    help="reference image to compare against")
    ap.add_argument("--frames", type=int, default=4,
                    help="how many frames to capture (default 4)")
    ap.add_argument("--interval-ms", type=int, default=1500,
                    help="ms between frame captures (default 1500)")
    ap.add_argument("--warmup-ms", type=int, default=4000,
                    help="ms to wait after launch before first capture")
    ap.add_argument("--out", type=Path,
                    default=Path("/tmp/ui_ab.png"))
    ap.add_argument("--build-dir", type=Path, default=Path("build"))
    args = ap.parse_args()

    if not args.reference.exists():
        print(f"error: reference {args.reference} not found", file=sys.stderr)
        return 2

    ref = Image.open(args.reference).convert("RGB")
    raw_frames, used_winid = capture_frames(
        args.mode, args.frames, args.interval_ms,
        args.build_dir, args.warmup_ms)
    if used_winid:
        # screencapture -l already returned the window contents only;
        # skip the auto-crop heuristic.
        cropped = [f.convert("RGB") for f in raw_frames]
    else:
        cropped = [find_window_crop(f) for f in raw_frames]

    # Frame timing labels.
    label_fmt = "t=" + ("{i}*" + str(args.interval_ms/1000.0) + "s")
    compose_ab(ref, cropped, args.out,
               frame_label_fmt="t={i}*" + f"{args.interval_ms/1000.0}s")
    return 0


if __name__ == "__main__":
    sys.exit(main())
