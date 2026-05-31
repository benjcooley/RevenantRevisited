#!/usr/bin/env python3
"""Capture UI test-mode frames and compose a pixel-scale A/B board.

Based on tools/vfx/snap_grid.py. Boots ./build/Revenant --test=<mode>,
captures N frames at a configurable interval (to see any animation
cycle), crops to the Revenant window content, then places the retail
reference and current build side by side at matching integer zoom.

This is a regression/transcription viewer. For retail UI reconstruction,
derive coordinates from /recon and disassembly first, then use this image
to catch obvious capture or porting mistakes.

Usage:
    tools/ui/snap_compare.py <mode> <reference.png>
                             [--frames N] [--interval-ms MS]
                             [--out PATH]

Example:
    tools/ui/snap_compare.py ui-plyrstatusbar docs/ui/plyr_stats_panel.png \\
                             --frames 1 --crop 0,0,640,120 \\
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


def find_revenant_window_info(owner_pid: int | None = None) -> tuple[int, dict] | None:
    """Return the CGWindowID and Quartz info for the launched Revenant window."""
    if not HAVE_QUARTZ:
        return None
    infos = CGWindowListCopyWindowInfo(kCGWindowListOptionAll, kCGNullWindowID)
    for info in infos:
        if owner_pid is not None:
            if info.get("kCGWindowOwnerPID") == owner_pid:
                return info["kCGWindowNumber"], info
        elif info.get("kCGWindowOwnerName", "") == "Revenant":
            return info["kCGWindowNumber"], info
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
                   build_dir: Path, warmup_ms: int) -> tuple[list[Image.Image], bool, float]:
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
            win_info = find_revenant_window_info(proc.pid)
            if win_info is None:
                win_id = None
                capture_scale = 1.0
                print("warning: Quartz unavailable or launched window not found; "
                      "falling back to full-screen capture",
                      file=sys.stderr)
            else:
                win_id, info = win_info
                bounds = info.get("kCGWindowBounds", {})
                bounds_w = float(bounds.get("Width", 0.0) or 0.0)
                capture_scale = 1.0
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
                if win_id is not None and bounds_w > 0:
                    capture_scale = max(1.0, frames[-1].width / bounds_w)
                if i + 1 < n:
                    time.sleep(interval_ms / 1000.0)
        finally:
            proc.terminate()
            try:
                proc.wait(timeout=2)
            except subprocess.TimeoutExpired:
                proc.kill()
        return frames, (win_id is not None), capture_scale
    finally:
        shutil.rmtree(tmp, ignore_errors=True)


def crop_rect(img: Image.Image, spec: str | None) -> Image.Image:
    if not spec:
        return img
    x, y, w, h = [int(v) for v in spec.split(",")]
    return img.crop((x, y, x + w, y + h))


def detect_content_top(img: Image.Image) -> int | None:
    """Find the first game-content row in a captured app window.

    macOS `screencapture -l` includes the titlebar. The Revenant test window
    begins with large runs of near-black backbuffer pixels, while the titlebar
    is dark gray chrome with very few true-black pixels. Scan the top of the
    capture and choose the first stable run of rows that looks like content.
    """
    rgb = img.convert("RGB")
    a = np.array(rgb)
    max_scan = min(160, max(1, img.height // 3))
    for y in range(max_scan):
        band = a[y:min(y + 4, img.height), :, :]
        black = (
            (band[:, :, 0] < 8)
            & (band[:, :, 1] < 8)
            & (band[:, :, 2] < 8)
        )
        if float(black.mean()) > 0.20:
            return y
    return None


def crop_window_content(img: Image.Image, chrome_top: int | None) -> Image.Image:
    """Drop macOS window chrome from screencapture -l output."""
    if chrome_top is None:
        chrome_top = detect_content_top(img)
        if chrome_top is None:
            chrome_top = 0
            print("warning: could not auto-detect content top; keeping "
                  "full window capture", file=sys.stderr)
        else:
            print(f"[snap] content top = {chrome_top}px")
    if chrome_top <= 0 or img.height <= chrome_top:
        return img
    return img.crop((0, chrome_top, img.width, img.height))


def normalize_window_scale(img: Image.Image, scale: float) -> Image.Image:
    """Convert a Retina/window capture from physical pixels to game pixels."""
    if scale <= 1.01:
        return img
    w = max(1, int(round(img.width / scale)))
    h = max(1, int(round(img.height / scale)))
    return img.resize((w, h), Image.Resampling.BOX)


def zoom_image(img: Image.Image, zoom: int) -> Image.Image:
    img = img.convert("RGB")
    if zoom <= 1:
        return img
    return img.resize((img.width * zoom, img.height * zoom),
                      Image.Resampling.NEAREST)


def compose_ab(ref: Image.Image, mockup_frames: list[Image.Image],
               out_path: Path, frame_label_fmt: str = "t={i}",
               zoom: int = 2, layout: str = "side-by-side",
               ref_zoom: int | None = None,
               current_zoom: int | None = None) -> None:
    """Compose reference/current images without aspect-changing resizes."""
    LABEL_H = 22
    GAP = 10
    BG = (28, 28, 32)

    if ref_zoom is None:
        ref_zoom = zoom
    if current_zoom is None:
        current_zoom = zoom

    ref_z = zoom_image(ref, ref_zoom)
    mocks_z = [zoom_image(f, current_zoom) for f in mockup_frames]

    try:
        font = ImageFont.truetype(
            "/System/Library/Fonts/Supplemental/Arial.ttf", 14)
        font_small = ImageFont.truetype(
            "/System/Library/Fonts/Supplemental/Arial.ttf", 11)
    except Exception:
        font = font_small = ImageFont.load_default()

    if layout == "stack":
        mocks_row_w = sum(m.width for m in mocks_z) + GAP * max(0, len(mocks_z) - 1)
        total_w = max(ref_z.width, mocks_row_w) + GAP * 2
        total_h = LABEL_H + ref_z.height + LABEL_H + max(m.height for m in mocks_z) + GAP
        out = Image.new("RGB", (total_w, total_h), BG)
        draw = ImageDraw.Draw(out)
        draw.text((GAP, 3), f"REFERENCE (retail)  zoom={ref_zoom}x",
                  (220, 220, 220), font=font)
        out.paste(ref_z, (GAP, LABEL_H))
        y = LABEL_H + ref_z.height + 3
        draw.text((GAP, y), f"CURRENT BUILD — {len(mocks_z)} frame(s)",
                  (220, 220, 220), font=font)
        x = GAP
        yy = LABEL_H + ref_z.height + LABEL_H
        for i, img in enumerate(mocks_z):
            out.paste(img, (x, yy))
            x += img.width + GAP
    else:
        current_w = sum(m.width for m in mocks_z) + GAP * max(0, len(mocks_z) - 1)
        current_h = max(m.height for m in mocks_z)
        total_w = ref_z.width + GAP + current_w + GAP * 2
        total_h = LABEL_H + max(ref_z.height, current_h) + GAP
        out = Image.new("RGB", (total_w, total_h), BG)
        draw = ImageDraw.Draw(out)

        left_x = GAP
        right_x = GAP + ref_z.width + GAP
        draw.text((left_x, 3), f"REFERENCE (retail)  zoom={ref_zoom}x",
                  (220, 220, 220), font=font)
        draw.text((right_x, 3), f"CURRENT BUILD — {len(mocks_z)} frame(s)",
                  (220, 220, 220), font=font)

        y = LABEL_H
        out.paste(ref_z, (left_x, y))
        x = right_x
        for i, img in enumerate(mocks_z):
            out.paste(img, (x, y))
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
    ap.add_argument("--warmup-ms", type=int, default=7000,
                    help="ms to wait after launch before first capture")
    ap.add_argument("--out", type=Path,
                    default=Path("/tmp/ui_ab.png"))
    ap.add_argument("--layout", choices=("side-by-side", "stack"),
                    default="side-by-side",
                    help="comparison layout (default side-by-side)")
    ap.add_argument("--zoom", type=int, default=2,
                    help="integer nearest-neighbor zoom for both sides (default 2)")
    ap.add_argument("--ref-zoom", type=int, default=0,
                    help="integer zoom for the reference side. Default 0 uses --zoom.")
    ap.add_argument("--current-zoom", type=int, default=0,
                    help="integer zoom for captured frames. Default 0 uses --zoom.")
    ap.add_argument("--chrome-top", type=int, default=-1,
                    help="pixels of macOS titlebar to remove from window captures "
                         "before --crop. Default -1 auto-detects the game "
                         "content top; use 0 to keep chrome.")
    ap.add_argument("--window-scale", type=float, default=0.0,
                    help="physical-to-logical scale for window captures. "
                         "Default 0 auto-detects from Quartz bounds; use 1 "
                         "to disable Retina normalization.")
    ap.add_argument("--crop", type=str, default=None,
                    help="x,y,w,h crop for current build, after titlebar removal. "
                         "Use to zero in on a UI region for tight A/B.")
    ap.add_argument("--ref-crop", type=str, default=None,
                    help="x,y,w,h crop for the reference image")
    ap.add_argument("--build-dir", type=Path, default=Path("build"))
    args = ap.parse_args()

    if not args.reference.exists():
        print(f"error: reference {args.reference} not found", file=sys.stderr)
        return 2

    ref = crop_rect(Image.open(args.reference).convert("RGB"), args.ref_crop)
    raw_frames, used_winid, capture_scale = capture_frames(
        args.mode, args.frames, args.interval_ms,
        args.build_dir, args.warmup_ms)
    if args.window_scale > 0.0:
        capture_scale = args.window_scale
    if used_winid:
        chrome_top = None if args.chrome_top < 0 else args.chrome_top
        cropped = [
            normalize_window_scale(
                crop_window_content(f.convert("RGB"), chrome_top),
                capture_scale)
            for f in raw_frames
        ]
    else:
        cropped = [find_window_crop(f) for f in raw_frames]

    if args.crop:
        try:
            cropped = [crop_rect(c, args.crop) for c in cropped]
        except Exception as e:
            print(f"warning: --crop parse failed ({e}); using full frames",
                  file=sys.stderr)

    # Frame timing labels.
    compose_ab(ref, cropped, args.out,
               frame_label_fmt="frame {i}",
               zoom=max(1, args.zoom),
               layout=args.layout,
               ref_zoom=max(1, args.ref_zoom) if args.ref_zoom > 0 else None,
               current_zoom=max(1, args.current_zoom) if args.current_zoom > 0 else None)
    return 0


if __name__ == "__main__":
    sys.exit(main())
