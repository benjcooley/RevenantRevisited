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
                         [--label-engine STR]

Example (the B01 blood A/B):
    tools/vfx/snap_ab.py TBloodEffect_BESPOKE TBloodEffect \\
        --cols 8 --interval-ms 80 --warmup-ms 1500 \\
        --out /tmp/blood_ab.png

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


def run_capture_ab(bespoke_id: str, engine_id: str, cols: int,
                   interval_ms: int, warmup_ms: int, out_path: Path,
                   build_dir: Path, label_bespoke: str,
                   label_engine: str) -> int:
    tmp = Path(tempfile.mkdtemp(prefix="vfxab_"))
    try:
        # Capture bespoke first, then engine. Sequential by design — the
        # harness doesn't multi-instance cleanly.
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
        # Scale so each row fits within ~2400 px horizontal.
        max_row_w = 2400
        scale = min(1.0, max_row_w / float(fw * cols))
        cell_w = int(fw * scale)
        cell_h = int(fh * scale)

        bespoke_row = _compose_strip(bespoke_frames, cell_w, cell_h)
        engine_row  = _compose_strip(engine_frames,  cell_w, cell_h)

        # Left-side labels: render each label rotated 90° into a strip
        # gutter, then paste the rows beside them.
        label_w = max(48, cell_h // 6)
        label_font_size = max(14, label_w // 3)
        font = _load_label_font(label_font_size)

        def _make_label(text: str) -> Image.Image:
            # Build the label horizontally then rotate so it reads
            # bottom-to-top in the gutter (consistent w/ filmstrip
            # convention; works for short strings like "bespoke"/"engine").
            tile = Image.new("RGB", (cell_h, label_w), (24, 24, 24))
            d = ImageDraw.Draw(tile)
            # Center the text in the horizontal tile before rotation.
            bbox = d.textbbox((0, 0), text, font=font)
            tw = bbox[2] - bbox[0]
            th = bbox[3] - bbox[1]
            tx = (tile.size[0] - tw) // 2
            ty = (tile.size[1] - th) // 2 - bbox[1]
            d.text((tx, ty), text, fill=(220, 220, 220), font=font)
            return tile.rotate(90, expand=True)

        bespoke_label = _make_label(label_bespoke)
        engine_label  = _make_label(label_engine)

        # Final canvas: [label][filmstrip] per row, rows stacked vertically.
        total_w = label_w + cell_w * cols
        total_h = cell_h * 2
        canvas = Image.new("RGB", (total_w, total_h), (16, 16, 16))
        canvas.paste(bespoke_label, (0, 0))
        canvas.paste(bespoke_row,   (label_w, 0))
        canvas.paste(engine_label,  (0, cell_h))
        canvas.paste(engine_row,    (label_w, cell_h))

        out_path.parent.mkdir(parents=True, exist_ok=True)
        canvas.save(out_path)
        print(f"wrote {out_path} ({canvas.size[0]}x{canvas.size[1]}, "
              f"{cols} cols, bespoke='{bespoke_id}' engine='{engine_id}')")
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
    args = ap.parse_args()

    out = args.out
    if out is None:
        out = Path(f"/tmp/{args.bespoke_id}_vs_{args.engine_id}.png")

    return run_capture_ab(args.bespoke_id, args.engine_id, args.cols,
                          args.interval_ms, args.warmup_ms, out,
                          args.build_dir, args.label_bespoke,
                          args.label_engine)


if __name__ == "__main__":
    sys.exit(main())
