#!/usr/bin/env python3
"""Side-by-side A/B filmstrip of two --test=vfx effect ids.

Canonical Phase B2 A/B verification capture per docs/vfx/AGENT_GUIDE.md
§3.2 / §3.2.0 (bespoke baseline before engine port methodology). Boots
the Revenant harness once per effect id (the harness doesn't multi-
instance cleanly), captures N frames of each at a fixed interval, then
stitches the two captures into ONE PNG with the bespoke panel above
the engine panel, sharing a left-gutter rotated label per side.

Layouts:
    grid   (default)  Each side composed as a rows×cols grid (default
                      4 cols × ceil(cols/4) rows); bespoke grid stacked
                      above engine grid. Larger per-tile resolution
                      makes individual droplet/glyph/silhouette shape
                      legible, which is what the canonical "is the port
                      visually equivalent?" review actually needs an
                      LLM-vision agent (or human) to resolve.
    strip             2 rows of N frames in strict left-to-right time
                      order, with frame-N of bespoke vertically aligned
                      to frame-N of engine. Use this when the question
                      is specifically about temporal progression / when
                      the divergence to spot is "wrong timing", not
                      "wrong droplet shape".

Default output path:
    grid   → /tmp/<bespoke_id>_vs_<engine_id>.png         (canonical)
    strip  → /tmp/<bespoke_id>_vs_<engine_id>_strip.png

Usage:
    tools/vfx/snap_ab.py <bespoke_id> <engine_id> [--cols N]
                         [--interval-ms MS] [--warmup-ms MS]
                         [--out PATH] [--label-bespoke STR]
                         [--label-engine STR] [--layout strip|grid]
                         [--grid-cols N]

Example (the B01 blood A/B):
    tools/vfx/snap_ab.py TBloodEffect_BESPOKE TBloodEffect \\
        --cols 8 --interval-ms 80 --warmup-ms 1500
    # → /tmp/TBloodEffect_BESPOKE_vs_TBloodEffect.png  (grid)

    tools/vfx/snap_ab.py TBloodEffect_BESPOKE TBloodEffect \\
        --cols 8 --interval-ms 80 --warmup-ms 1500 --layout strip
    # → /tmp/TBloodEffect_BESPOKE_vs_TBloodEffect_strip.png

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

# Reference-footage catalog (out-of-repo at $REVENANT_REFS_DIR). Optional —
# snap_ab degrades gracefully to the 2-row mode when the catalog is missing
# or empty for the requested id.
_REFCAP_DIR = Path(__file__).resolve().parent.parent / "refcap"
sys.path.insert(0, str(_REFCAP_DIR))
try:
    import catalog as refcat  # type: ignore
except Exception:  # pragma: no cover - cataolog absent => no --ref support
    refcat = None  # type: ignore


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


def _parse_ref_spec(spec: str) -> tuple[str, str | None, str | None]:
    """Split <ID>[:<CLIP_ID>[:<SET_ID>]] into a tuple."""
    parts = spec.split(":", 2)
    eid = parts[0]
    clip_id = parts[1] if len(parts) > 1 and parts[1] else None
    set_id = parts[2] if len(parts) > 2 and parts[2] else None
    return eid, clip_id, set_id


def _pick_extract(clip: dict, interval_ms: int, cols: int) -> dict | None:
    """Pick the best extract set for the requested capture cadence.

    Preference order (per design.snap_ab_integration):
      1. an `every` set whose every_ms == --interval-ms
      2. the first set whose frame_count >= cols
      3. any set (so we degrade-graceful even if it's smaller than cols)
    """
    extracts = clip.get("extracts") or []
    if not extracts:
        return None
    for e in extracts:
        if e.get("mode") == "every" and \
                int(e.get("every_ms") or 0) == int(interval_ms):
            return e
    for e in extracts:
        if int(e.get("frame_count") or 0) >= cols:
            return e
    return extracts[0]


def _select_frames(pngs: list[Path], cols: int) -> tuple[list[Path], list[bool]]:
    """Pick exactly `cols` frame paths and a per-tile `held` flag.

    - if len(pngs) >= cols and the chosen extract was the "right" one
      we take the first `cols`
    - if len(pngs) < cols, repeat the last (with held=True) so columns
      still align
    - if len(pngs) > cols, stride-sample evenly
    """
    n = len(pngs)
    if n == 0:
        return [], []
    if n == cols:
        return pngs, [False] * cols
    if n < cols:
        out = list(pngs) + [pngs[-1]] * (cols - n)
        held = [False] * n + [True] * (cols - n)
        return out, held
    # n > cols: stride-sample. floor(i * (n-1) / (cols-1)) covers
    # endpoints honestly.
    out = []
    if cols == 1:
        out = [pngs[0]]
    else:
        for i in range(cols):
            idx = (i * (n - 1)) // (cols - 1)
            out.append(pngs[idx])
    return out, [False] * cols


def _frame_t_ms_for_extract(extract: dict, frame_idx: int) -> int | None:
    """Map a 1-based frame index in an extract set to its source clip t_ms."""
    mode = extract.get("mode")
    if mode == "at":
        ats = extract.get("at_ms") or []
        if 0 <= (frame_idx - 1) < len(ats):
            return int(ats[frame_idx - 1])
        return None
    if mode == "every":
        start = int(extract.get("start_ms") or 0)
        every = int(extract.get("every_ms") or 0)
        return start + (frame_idx - 1) * every
    return None


def _nearest_marker(markers: list[dict], t_ms: int,
                    window_ms: int = 60) -> str | None:
    """Return the label of the marker nearest to t_ms within window_ms."""
    best: tuple[int, str] | None = None
    for m in markers or []:
        try:
            mt = int(m.get("t_ms"))
        except (TypeError, ValueError):
            continue
        dist = abs(mt - t_ms)
        if dist <= window_ms and (best is None or dist < best[0]):
            best = (dist, str(m.get("label") or ""))
    return best[1] if best else None


def _resolve_reference_frames(ref_spec: str, ref_domain: str,
                              interval_ms: int, cols: int
                              ) -> tuple[list[Image.Image], list[str]] | None:
    """Load reference frames + per-tile captions from the catalog.

    Returns None (with a printed warning) when anything is missing — the
    caller should fall back to the 2-row mode rather than aborting.
    """
    if refcat is None:
        print(f"[snap_ab] warning: refcap catalog module unavailable; "
              f"--ref {ref_spec!r} ignored", file=sys.stderr)
        return None

    eid, clip_id, set_id = _parse_ref_spec(ref_spec)
    data = refcat.load_manifest(ref_domain, eid)
    if data is None:
        print(f"[snap_ab] warning: no manifest for {ref_domain}/{eid} "
              f"at {refcat.manifest_path(ref_domain, eid)} -- "
              f"falling back to 2-row mode", file=sys.stderr)
        return None
    clips = data.get("clips") or []
    if not clips:
        print(f"[snap_ab] warning: {ref_domain}/{eid} manifest has no "
              f"clips yet -- falling back to 2-row mode; populate with "
              f"`tools/refcap/import_clip {ref_domain} {eid} <video>`",
              file=sys.stderr)
        return None

    clip = refcat.find_clip(data, clip_id) if clip_id else clips[0]
    if clip is None:
        print(f"[snap_ab] warning: clip_id {clip_id!r} not in "
              f"{ref_domain}/{eid} -- falling back to 2-row mode",
              file=sys.stderr)
        return None
    extract = refcat.find_extract(clip, set_id) if set_id else \
        _pick_extract(clip, interval_ms, cols)
    if extract is None:
        print(f"[snap_ab] warning: {ref_domain}/{eid}/"
              f"{clip.get('clip_id')} has no extracted frame sets -- "
              f"falling back to 2-row mode; populate with "
              f"`tools/refcap/extract_frames ...`", file=sys.stderr)
        return None

    fdir = refcat.extract_frames_abspath(ref_domain, eid, extract)
    pngs = sorted(fdir.glob("frame_*.png"))
    if not pngs:
        print(f"[snap_ab] warning: extract dir {fdir} has no PNGs -- "
              f"falling back to 2-row mode", file=sys.stderr)
        return None

    chosen, held = _select_frames(pngs, cols)
    markers = clip.get("markers") or []
    images: list[Image.Image] = []
    captions: list[str] = []
    for i, (p, is_held) in enumerate(zip(chosen, held), start=1):
        # Source-frame index inside the extract before any hold/stride.
        # If we held, the held frames map to the last real frame's t_ms.
        src_idx = i if i <= len(pngs) else len(pngs)
        t_ms = _frame_t_ms_for_extract(extract, src_idx)
        cap_bits: list[str] = []
        if t_ms is not None:
            mark = _nearest_marker(markers, t_ms)
            if mark:
                cap_bits.append(f"t={t_ms} ({mark})")
            else:
                cap_bits.append(f"t={t_ms}")
        if is_held:
            cap_bits.append("(held)")
        captions.append(" ".join(cap_bits))
        images.append(Image.open(p).convert("RGB"))

    print(f"[snap_ab] reference: {ref_domain}/{eid}/"
          f"{clip.get('clip_id')}/{extract.get('set_id')} "
          f"({len(pngs)} pngs -> {cols} tiles)")
    return images, captions


def _scale_letterbox(img: Image.Image, target_w: int,
                     target_h: int) -> Image.Image:
    """Scale `img` to fit (target_w, target_h) preserving aspect, padding
    with black bars (honest scale; no stretch). Per design.snap_ab_integration."""
    sw, sh = img.size
    if sw == 0 or sh == 0:
        return Image.new("RGB", (target_w, target_h), (0, 0, 0))
    scale = min(target_w / float(sw), target_h / float(sh))
    new_w = max(1, int(round(sw * scale)))
    new_h = max(1, int(round(sh * scale)))
    scaled = img.resize((new_w, new_h), Image.LANCZOS)
    canvas = Image.new("RGB", (target_w, target_h), (0, 0, 0))
    ox = (target_w - new_w) // 2
    oy = (target_h - new_h) // 2
    canvas.paste(scaled, (ox, oy))
    return canvas


def _compose_ref_row(images: list[Image.Image], captions: list[str],
                     cell_w: int, cell_h: int,
                     font: ImageFont.ImageFont) -> Image.Image:
    """Lay out a single-row strip of reference tiles with small captions
    burned into the bottom of each tile."""
    row = Image.new("RGB", (cell_w * len(images), cell_h), (32, 32, 32))
    for i, (img, cap) in enumerate(zip(images, captions)):
        tile = _scale_letterbox(img, cell_w, cell_h)
        if cap:
            d = ImageDraw.Draw(tile)
            # Half-height black band at the bottom for legible text.
            band_h = max(14, cell_h // 14)
            d.rectangle([0, cell_h - band_h, cell_w, cell_h],
                        fill=(0, 0, 0))
            bbox = d.textbbox((0, 0), cap, font=font)
            tw = bbox[2] - bbox[0]
            th = bbox[3] - bbox[1]
            d.text(((cell_w - tw) // 2,
                    cell_h - band_h + (band_h - th) // 2 - bbox[1]),
                   cap, fill=(220, 220, 220), font=font)
        row.paste(tile, (i * cell_w, 0))
    return row


def run_capture_ab(bespoke_id: str, engine_id: str, cols: int,
                   interval_ms: int, warmup_ms: int, out_path: Path,
                   build_dir: Path, label_bespoke: str,
                   label_engine: str, layout: str,
                   grid_cols: int | None,
                   ref_spec: str | None = None,
                   ref_domain: str = "vfx",
                   ref_label: str = "reference") -> int:
    tmp = Path(tempfile.mkdtemp(prefix="vfxab_"))
    try:
        # Resolve reference frames up front so the user sees the friendly
        # warning *before* we spend two harness boots on capture.
        ref_resolved: tuple[list[Image.Image], list[str]] | None = None
        if ref_spec:
            ref_resolved = _resolve_reference_frames(
                ref_spec, ref_domain, interval_ms, cols)

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

        # Reference row (optional): same width as bespoke/engine panels,
        # one row of `cols` tiles regardless of layout choice. Tile width
        # is panel_w/cols so the ref row visually aligns with the strip
        # layout; in grid layout the reference is still a single strip
        # since it's not multi-grid (it's just N evenly-sampled frames
        # of the original-game clip).
        ref_panel: Image.Image | None = None
        ref_label_panel: Image.Image | None = None
        ref_panel_h = 0
        if ref_resolved is not None:
            ref_imgs, ref_caps = ref_resolved
            ref_tile_w = panel_w // max(1, len(ref_imgs))
            # Pick height honoring source aspect; fall back to bespoke tile
            # height as a sensible ceiling.
            src_w, src_h = ref_imgs[0].size
            if src_w > 0:
                ref_tile_h = max(1, int(round(ref_tile_w * src_h / src_w)))
            else:
                ref_tile_h = cell_h
            # Cap so a single-row ref doesn't dwarf the bespoke/engine.
            max_ref_h = max(cell_h, panel_h // 2)
            if ref_tile_h > max_ref_h:
                # Shrink width too so we keep aspect.
                ratio = max_ref_h / float(ref_tile_h)
                ref_tile_h = max_ref_h
                ref_tile_w = max(1, int(ref_tile_w * ratio))
            caption_font_size = max(11, ref_tile_w // 24)
            caption_font = _load_label_font(caption_font_size)
            ref_strip = _compose_ref_row(
                ref_imgs, ref_caps, ref_tile_w, ref_tile_h, caption_font)
            # Center the strip in a full-panel-width row so it aligns with
            # the bespoke/engine panels below it.
            ref_panel = Image.new("RGB", (panel_w, ref_tile_h),
                                  (32, 32, 32))
            ox = (panel_w - ref_strip.size[0]) // 2
            ref_panel.paste(ref_strip, (ox, 0))
            ref_panel_h = ref_tile_h
            ref_label_panel = _make_rotated_label(
                ref_label, label_w, ref_panel_h, font)

        # Final canvas: stacked [ref?] / bespoke / engine, each with a
        # rotated left gutter label.
        divider_h = 2
        total_w = label_w + panel_w
        total_h = panel_h * 2 + divider_h
        if ref_panel is not None:
            total_h += ref_panel_h + divider_h
        canvas = Image.new("RGB", (total_w, total_h), (16, 16, 16))
        y = 0
        if ref_panel is not None and ref_label_panel is not None:
            canvas.paste(ref_label_panel, (0, y))
            canvas.paste(ref_panel, (label_w, y))
            y += ref_panel_h + divider_h
        canvas.paste(bespoke_label, (0, y))
        canvas.paste(bespoke_panel, (label_w, y))
        y += panel_h + divider_h
        canvas.paste(engine_label, (0, y))
        canvas.paste(engine_panel, (label_w, y))

        out_path.parent.mkdir(parents=True, exist_ok=True)
        canvas.save(out_path)
        n_rows = 3 if ref_panel is not None else 2
        print(f"wrote {out_path} ({canvas.size[0]}x{canvas.size[1]}, "
              f"layout={layout}, {cols} frames/side, rows={n_rows}, "
              f"bespoke='{bespoke_id}' engine='{engine_id}'"
              + (f" ref='{ref_spec}'" if ref_spec else "")
              + ")")
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
    ap.add_argument("--layout", choices=("strip", "grid"), default="grid",
                    help="grid = each side as rows×cols grid stacked "
                         "(default; bigger per-tile detail, canonical "
                         "for visual-equivalence review); "
                         "strip = 2 rows of N frames with frame-N "
                         "bespoke vertically aligned to frame-N engine "
                         "(use for temporal-progression review)")
    ap.add_argument("--grid-cols", type=int, default=None,
                    help="grid layout: columns per side (default 4); "
                         "rows = ceil(--cols / --grid-cols)")
    ap.add_argument("--ref", default=None,
                    help="reference clip in the form "
                         "<ID>[:<CLIP_ID>[:<SET_ID>]]; adds a top row of "
                         "original-game footage from the refcap catalog "
                         "($REVENANT_REFS_DIR / ~/RevenantRefs). When "
                         "missing or empty, snap_ab prints a friendly "
                         "warning and runs the 2-row mode.")
    ap.add_argument("--ref-domain", default="vfx",
                    help="catalog domain (default vfx)")
    ap.add_argument("--ref-label", default="reference",
                    help="left-gutter label for the reference row")
    args = ap.parse_args()

    out = args.out
    if out is None:
        # Canonical capture (grid, the default) gets the bare name; strip
        # layout is the special-case workflow and tags its filename so
        # both can coexist in /tmp without clobbering each other. The
        # `_ref` suffix (per design.snap_ab_integration) keeps reference
        # and reference-free captures from clobbering each other.
        ref_suffix = "_ref" if args.ref else ""
        if args.layout == "grid":
            out = Path(
                f"/tmp/{args.bespoke_id}_vs_{args.engine_id}{ref_suffix}.png")
        else:
            out = Path(
                f"/tmp/{args.bespoke_id}_vs_{args.engine_id}"
                f"_strip{ref_suffix}.png")

    return run_capture_ab(args.bespoke_id, args.engine_id, args.cols,
                          args.interval_ms, args.warmup_ms, out,
                          args.build_dir, args.label_bespoke,
                          args.label_engine, args.layout, args.grid_cols,
                          ref_spec=args.ref, ref_domain=args.ref_domain,
                          ref_label=args.ref_label)


if __name__ == "__main__":
    sys.exit(main())
