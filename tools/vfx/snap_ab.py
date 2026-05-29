#!/usr/bin/env python3
"""Side-by-side A/B filmstrip of two --test=vfx effect ids.

Canonical Phase B2 A/B verification capture per docs/vfx/AGENT_GUIDE.md
§3.2 / §3.2.0 (bespoke baseline before engine port methodology). Boots
the Revenant harness once per effect id (the harness doesn't multi-
instance cleanly), captures N frames of each at a fixed interval, then
stitches the two filmstrips into ONE PNG: the bespoke row on top, the
engine row directly below, with frame-N of bespoke vertically aligned
with frame-N of engine. A reviewer can scan left-to-right and immediately
spot where the two diverge.

Usage:
    tools/vfx/snap_ab.py <bespoke_id> <engine_id> [--cols N]
                         [--interval-ms MS] [--warmup-ms MS]
                         [--out PATH] [--label-bespoke STR]
                         [--label-engine STR] [--layout strip|grid]
                         [--grid-cols N]

Layouts:
    strip  (default)  2 rows of N frames, left-to-right time order.
    grid              Each side composed as a rows×cols grid; bespoke
                      grid stacked above engine grid with the same
                      left-gutter rotated label. Bigger per-tile
                      resolution for LLM-vision review at the cost of
                      strict horizontal time order.

Example (the B01 blood A/B):
    tools/vfx/snap_ab.py TBloodEffect_BESPOKE TBloodEffect \\
        --cols 8 --interval-ms 80 --warmup-ms 1500 \\
        --out /tmp/blood_ab.png

    tools/vfx/snap_ab.py TBloodEffect_BESPOKE TBloodEffect \\
        --cols 8 --interval-ms 80 --warmup-ms 1500 \\
        --layout grid --out /tmp/blood_ab_grid.png

Requires Pillow (`pip install Pillow`) and macOS screencapture +
osascript. Runs from the worktree root (the build/ dir must already
exist; the script does NOT cmake).
"""

import argparse
import math
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


def _resolve_revenant_wid():
    """Return the Quartz window id of the Revenant binary, or None."""
    try:
        import Quartz  # type: ignore
        wl = Quartz.CGWindowListCopyWindowInfo(
            Quartz.kCGWindowListOptionOnScreenOnly, Quartz.kCGNullWindowID)
        for w in wl:
            if 'Revenant' in (w.get('kCGWindowOwnerName') or ''):
                return w.get('kCGWindowNumber')
    except Exception as e:
        print(f"warning: Quartz unavailable ({e}); falling back to "
              f"full-screen capture", file=sys.stderr)
    return None


def _capture_strip(effect_id: str, cols: int, interval_ms: int,
                   warmup_ms: int, build_dir: Path,
                   tmp: Path) -> list[Image.Image] | None:
    """Boot Revenant for one effect id, capture `cols` frames, return them."""
    revenant = build_dir / "Revenant"
    if not revenant.exists():
        print(f"error: {revenant} not found. cmake --build first.",
              file=sys.stderr)
        return None

    # Make sure no Revenant is running from a previous capture.
    subprocess.run(["pkill", "-9", "Revenant"],
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    time.sleep(0.5)

    proc = subprocess.Popen(
        [str(revenant), "--test=vfx", f"--vfx={effect_id}", "--vfx-no-ui"],
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
    )
    frames: list[Image.Image] = []
    try:
        time.sleep(warmup_ms / 1000.0)
        wid = _resolve_revenant_wid()
        print(f"[snap_ab] capturing {effect_id}: {cols} frames "
              f"@ {interval_ms}ms (wid={wid})", flush=True)
        for i in range(cols):
            shot = tmp / f"{effect_id}_{i:03d}.png"
            if wid is not None:
                subprocess.run(
                    ["screencapture", "-x", "-t", "png", "-l", str(wid),
                     str(shot)],
                    check=True,
                )
            else:
                subprocess.run(
                    ["screencapture", "-x", "-t", "png", str(shot)],
                    check=True,
                )
            if i + 1 < cols:
                time.sleep(interval_ms / 1000.0)
        for i in range(cols):
            shot = tmp / f"{effect_id}_{i:03d}.png"
            if shot.exists():
                frames.append(Image.open(shot).convert("RGB"))
            else:
                print(f"warning: missing {shot}; skipping", file=sys.stderr)
    finally:
        proc.terminate()
        try:
            proc.wait(timeout=2)
        except subprocess.TimeoutExpired:
            proc.kill()
        # Ensure no stale Revenant blocks the next capture.
        subprocess.run(["pkill", "-9", "Revenant"],
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    return frames if frames else None


def _compose_strip(frames: list[Image.Image], cell_w: int,
                   cell_h: int) -> Image.Image:
    strip = Image.new("RGB", (cell_w * len(frames), cell_h), (32, 32, 32))
    for i, img in enumerate(frames):
        small = img.resize((cell_w, cell_h), Image.LANCZOS)
        strip.paste(small, (i * cell_w, 0))
    return strip


def _compose_grid(frames: list[Image.Image], cell_w: int, cell_h: int,
                  cols: int, rows: int) -> Image.Image:
    """Lay out `frames` as a rows×cols grid, row-major (left-to-right,
    top-to-bottom). Unused tiles stay background-grey."""
    grid = Image.new("RGB", (cell_w * cols, cell_h * rows), (32, 32, 32))
    for i, img in enumerate(frames):
        r = i // cols
        c = i % cols
        if r >= rows:
            break
        small = img.resize((cell_w, cell_h), Image.LANCZOS)
        grid.paste(small, (c * cell_w, r * cell_h))
    return grid


def _load_label_font(size: int) -> ImageFont.ImageFont:
    for candidate in (
            "/System/Library/Fonts/Supplemental/Arial Bold.ttf",
            "/System/Library/Fonts/Supplemental/Arial.ttf",
            "/System/Library/Fonts/Helvetica.ttc",
    ):
        if os.path.exists(candidate):
            try:
                return ImageFont.truetype(candidate, size)
            except Exception:
                pass
    return ImageFont.load_default()


def _make_rotated_label(text: str, gutter_w: int, panel_h: int,
                        font: ImageFont.ImageFont) -> Image.Image:
    """Build a horizontal label tile sized to (panel_h, gutter_w) then
    rotate 90° so it reads bottom-to-top in a left-side gutter of
    height `panel_h` and width `gutter_w`."""
    tile = Image.new("RGB", (panel_h, gutter_w), (24, 24, 24))
    d = ImageDraw.Draw(tile)
    bbox = d.textbbox((0, 0), text, font=font)
    tw = bbox[2] - bbox[0]
    th = bbox[3] - bbox[1]
    tx = (tile.size[0] - tw) // 2
    ty = (tile.size[1] - th) // 2 - bbox[1]
    d.text((tx, ty), text, fill=(220, 220, 220), font=font)
    return tile.rotate(90, expand=True)


def run_capture_ab(bespoke_id: str, engine_id: str, cols: int,
                   interval_ms: int, warmup_ms: int, out_path: Path,
                   build_dir: Path, label_bespoke: str,
                   label_engine: str, layout: str,
                   grid_cols: int | None) -> int:
    tmp = Path(tempfile.mkdtemp(prefix="vfxab_"))
    try:
        # Capture bespoke first, then engine. Sequential by design — the
        # harness doesn't multi-instance cleanly. `cols` here means "total
        # frames captured per side" regardless of layout — for grid layout
        # the on-image column count is `grid_cols` (default 4).
        bespoke_frames = _capture_strip(
            bespoke_id, cols, interval_ms, warmup_ms, build_dir, tmp)
        if not bespoke_frames:
            print(f"error: no frames captured for {bespoke_id}",
                  file=sys.stderr)
            return 1
        engine_frames = _capture_strip(
            engine_id, cols, interval_ms, warmup_ms, build_dir, tmp)
        if not engine_frames:
            print(f"error: no frames captured for {engine_id}",
                  file=sys.stderr)
            return 1

        # Normalize frame sizes (screencapture by wid can vary if the
        # window resized between boots). Use the bespoke frame size as
        # the canonical cell size.
        fw, fh = bespoke_frames[0].size

        if layout == "strip":
            # Scale so each row fits within ~2400 px horizontal.
            max_row_w = 2400
            scale = min(1.0, max_row_w / float(fw * cols))
            cell_w = int(fw * scale)
            cell_h = int(fh * scale)

            bespoke_panel = _compose_strip(bespoke_frames, cell_w, cell_h)
            engine_panel  = _compose_strip(engine_frames,  cell_w, cell_h)
            panel_h = cell_h
        elif layout == "grid":
            g_cols = grid_cols if grid_cols is not None else min(4, cols)
            g_cols = max(1, g_cols)
            g_rows = max(1, math.ceil(cols / g_cols))
            # Scale so each grid row fits within ~2400 px horizontal.
            max_row_w = 2400
            scale = min(1.0, max_row_w / float(fw * g_cols))
            cell_w = int(fw * scale)
            cell_h = int(fh * scale)

            bespoke_panel = _compose_grid(
                bespoke_frames, cell_w, cell_h, g_cols, g_rows)
            engine_panel = _compose_grid(
                engine_frames, cell_w, cell_h, g_cols, g_rows)
            panel_h = cell_h * g_rows
        else:
            print(f"error: unknown --layout {layout!r}; "
                  f"expected 'strip' or 'grid'", file=sys.stderr)
            return 2

        panel_w = bespoke_panel.size[0]

        # Left-side rotated labels, sized to the panel height so the
        # gutter spans the full bespoke/engine block.
        label_w = max(48, panel_h // 8)
        label_font_size = max(14, label_w // 3)
        font = _load_label_font(label_font_size)
        bespoke_label = _make_rotated_label(
            label_bespoke, label_w, panel_h, font)
        engine_label = _make_rotated_label(
            label_engine, label_w, panel_h, font)

        # Final canvas: [label][panel] per side, sides stacked vertically
        # with a thin divider between them.
        divider_h = 2
        total_w = label_w + panel_w
        total_h = panel_h * 2 + divider_h
        canvas = Image.new("RGB", (total_w, total_h), (16, 16, 16))
        canvas.paste(bespoke_label, (0, 0))
        canvas.paste(bespoke_panel, (label_w, 0))
        # Divider band stays the (16,16,16) background fill.
        canvas.paste(engine_label, (0, panel_h + divider_h))
        canvas.paste(engine_panel, (label_w, panel_h + divider_h))

        out_path.parent.mkdir(parents=True, exist_ok=True)
        canvas.save(out_path)
        print(f"wrote {out_path} ({canvas.size[0]}x{canvas.size[1]}, "
              f"layout={layout}, {cols} frames/side, "
              f"bespoke='{bespoke_id}' engine='{engine_id}')")
        return 0
    finally:
        shutil.rmtree(tmp, ignore_errors=True)


def main() -> int:
    ap = argparse.ArgumentParser(
        description="VFX A/B side-by-side filmstrip (bespoke vs engine)")
    ap.add_argument("bespoke_id",
                    help="bespoke baseline effect id (e.g. TBloodEffect_BESPOKE)")
    ap.add_argument("engine_id",
                    help="engine-port effect id (e.g. TBloodEffect)")
    ap.add_argument("--cols", type=int, default=8,
                    help="frames per row (default 8)")
    ap.add_argument("--interval-ms", type=int, default=80,
                    help="ms between captures (default 80)")
    ap.add_argument("--warmup-ms", type=int, default=1500,
                    help="ms to wait after launch before first capture")
    ap.add_argument("--out", type=Path, default=None,
                    help="output PNG path (default "
                         "/tmp/<bespoke_id>_vs_<engine_id>.png)")
    ap.add_argument("--build-dir", type=Path, default=Path("build"))
    ap.add_argument("--label-bespoke", default="bespoke",
                    help="left-gutter label for the top row")
    ap.add_argument("--label-engine", default="engine",
                    help="left-gutter label for the bottom row")
    ap.add_argument("--layout", choices=("strip", "grid"), default="strip",
                    help="strip = 2 rows of N frames (default); "
                         "grid = each side as rows×cols grid stacked, "
                         "for bigger per-tile detail at the cost of "
                         "strict left-to-right time order")
    ap.add_argument("--grid-cols", type=int, default=None,
                    help="grid layout: columns per side (default 4); "
                         "rows = ceil(--cols / --grid-cols)")
    args = ap.parse_args()

    out = args.out
    if out is None:
        suffix = "grid" if args.layout == "grid" else "strip"
        out = Path(
            f"/tmp/{args.bespoke_id}_vs_{args.engine_id}_{suffix}.png")

    return run_capture_ab(args.bespoke_id, args.engine_id, args.cols,
                          args.interval_ms, args.warmup_ms, out,
                          args.build_dir, args.label_bespoke,
                          args.label_engine, args.layout, args.grid_cols)


if __name__ == "__main__":
    sys.exit(main())
