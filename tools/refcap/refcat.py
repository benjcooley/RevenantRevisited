#!/usr/bin/env python3
"""refcat — single-binary CLI for the reference-footage catalog.

Subcommands:

    refcat import    <domain> <id> <video.mp4>
                     [--label STR] [--src STR] [--clip-id STR]
        Copy a captured video into the catalog, compute sha256, append a
        stub clip entry to manifest.yaml (creates manifest if absent).
        Probes container/codec/fps/duration via ffprobe.

    refcat extract   <domain> <id> <clip_id> --set-id STR
                     (--every-ms N | --at MS1,MS2,...)
                     [--start-ms N] [--end-ms N]
        ffmpeg-extract PNG frames into frames/<clip_id>__<set_id>/.
        Records frame_count + produced date on the extract entry.

    refcat list      [<domain>] [<id>]
        Print catalog inventory.

    refcat validate  [--domain D] [--strict]
        Walk catalog: missing videos, dangling extract dirs, frame_count
        mismatch, sha256 drift vs index.yaml. Nonzero exit on issues.

    refcat lookup    <domain> <id> [<clip_id>]
                     [--set-id STR] [--frame N]
        Resolve a catalog tuple to absolute on-disk paths. snap_ab uses
        this to find frames at runtime.

    refcat sync-index
        Regenerate the in-repo references/index.yaml so reviewers/CI see
        catalog inventory without storing bytes.

Storage: $REVENANT_REFS_DIR (default ~/RevenantRefs). See
`references/REFS_README.md` for the catalog protocol and manifest
schema.
"""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from datetime import date
from pathlib import Path

# Make sibling catalog.py importable regardless of how this is invoked.
sys.path.insert(0, str(Path(__file__).resolve().parent))
import catalog as cat  # noqa: E402


# ---------------------------------------------------------------------------
# import
# ---------------------------------------------------------------------------

def cmd_import(args: argparse.Namespace) -> int:
    src_video = Path(args.video).expanduser().resolve()
    if not src_video.exists():
        print(f"error: video not found: {src_video}", file=sys.stderr)
        return 2

    cat.require_ffprobe()
    eid_dir = cat.id_dir(args.domain, args.id, create=True)
    data = cat.ensure_manifest(args.domain, args.id)

    clip_id = args.clip_id or src_video.stem
    if cat.find_clip(data, clip_id):
        print(f"error: clip_id '{clip_id}' already present in "
              f"{args.domain}/{args.id}", file=sys.stderr)
        return 2

    # Probe before copying so we can fail fast on a broken file.
    probe = cat.probe_video(src_video)

    dst_rel = Path("clips") / f"{clip_id}{src_video.suffix.lower()}"
    dst_abs = eid_dir / dst_rel
    dst_abs.parent.mkdir(parents=True, exist_ok=True)
    if dst_abs.resolve() != src_video.resolve():
        dst_abs.write_bytes(src_video.read_bytes())
    sha = cat.sha256_of(dst_abs)

    clip_entry = {
        "clip_id": clip_id,
        "schema_version": 1,
        "label": args.label or "",
        "src": args.src or "",
        "video_path": str(dst_rel),
        "video_sha256": sha,
        "container": probe.container,
        "codec": probe.codec,
        "width": probe.width,
        "height": probe.height,
        "fps": round(probe.fps, 3),
        "duration_ms": probe.duration_ms,
        "capture_tool": args.capture_tool or "",
        "notes": "",
        "markers": [],
        "extracts": [],
    }
    data.setdefault("clips", []).append(clip_entry)
    cat.save_manifest(args.domain, args.id, data)

    print(f"imported {args.domain}/{args.id}/{clip_id}")
    print(f"  video : {dst_abs}")
    print(f"  sha256: {sha[:16]}..")
    print(f"  fmt   : {probe.codec} {probe.width}x{probe.height} "
          f"@ {probe.fps:.2f}fps {probe.duration_ms}ms")
    print("next: edit the manifest to add markers, then "
          f"`refcat extract {args.domain} {args.id} {clip_id} "
          "--set-id <name> ...`")
    return 0


# ---------------------------------------------------------------------------
# extract
# ---------------------------------------------------------------------------

def _ffmpeg_frame_at(video: Path, t_ms: int, out_png: Path) -> None:
    """Extract a single PNG frame at t_ms. -ss before -i for fast seek,
    then -ss after -i for accurate frame; combined gives best of both."""
    cmd = [
        "ffmpeg", "-y", "-v", "error",
        "-ss", f"{t_ms / 1000.0:.3f}",
        "-i", str(video),
        "-frames:v", "1",
        str(out_png),
    ]
    subprocess.check_call(cmd)


def _ffmpeg_frames_every(video: Path, start_ms: int, end_ms: int,
                         every_ms: int, out_dir: Path) -> int:
    """Extract one PNG every `every_ms` from start_ms..end_ms. Returns
    the count of files written."""
    if every_ms <= 0:
        raise ValueError("--every-ms must be > 0")
    fps = 1000.0 / every_ms
    args = [
        "ffmpeg", "-y", "-v", "error",
        "-ss", f"{start_ms / 1000.0:.3f}",
    ]
    if end_ms > start_ms:
        args += ["-to", f"{end_ms / 1000.0:.3f}"]
    args += [
        "-i", str(video),
        "-vf", f"fps={fps:.6f}",
        "-vsync", "vfr",
        str(out_dir / "frame_%02d.png"),
    ]
    subprocess.check_call(args)
    return len(list(out_dir.glob("frame_*.png")))


def cmd_extract(args: argparse.Namespace) -> int:
    cat.require_ffmpeg()
    data = cat.load_manifest(args.domain, args.id)
    if data is None:
        print(f"error: no manifest for {args.domain}/{args.id}; "
              f"run `refcat import` first", file=sys.stderr)
        return 2
    clip = cat.find_clip(data, args.clip_id)
    if clip is None:
        print(f"error: clip_id '{args.clip_id}' not found in "
              f"{args.domain}/{args.id}", file=sys.stderr)
        return 2

    video_abs = cat.clip_video_abspath(args.domain, args.id, clip)
    if not video_abs.exists():
        print(f"error: video missing on disk: {video_abs}", file=sys.stderr)
        return 2

    set_id = args.set_id
    frames_rel = Path("frames") / f"{args.clip_id}__{set_id}"
    frames_abs = cat.id_dir(args.domain, args.id) / frames_rel
    frames_abs.mkdir(parents=True, exist_ok=True)
    # Clear any previous content so frame_count is honest.
    for old in frames_abs.glob("frame_*.png"):
        old.unlink()

    extract: dict = {
        "set_id": set_id,
        "frames_dir": str(frames_rel),
        "produced": date.today().isoformat(),
    }

    if args.at:
        ats = [int(s.strip()) for s in args.at.split(",") if s.strip()]
        if not ats:
            print("error: --at requires at least one ms value", file=sys.stderr)
            return 2
        for i, t_ms in enumerate(ats, start=1):
            out_png = frames_abs / f"frame_{i:02d}.png"
            _ffmpeg_frame_at(video_abs, t_ms, out_png)
        extract.update({"mode": "at", "at_ms": ats,
                        "frame_count": len(ats)})
    elif args.every_ms:
        start_ms = args.start_ms if args.start_ms is not None else 0
        end_ms = args.end_ms if args.end_ms is not None else \
            int(clip.get("duration_ms") or 0)
        count = _ffmpeg_frames_every(video_abs, start_ms, end_ms,
                                     args.every_ms, frames_abs)
        extract.update({
            "mode": "every",
            "every_ms": args.every_ms,
            "start_ms": start_ms,
            "end_ms": end_ms,
            "frame_count": count,
        })
    else:
        print("error: pass --every-ms or --at", file=sys.stderr)
        return 2

    # Replace any prior extract with this set_id.
    extracts = clip.setdefault("extracts", [])
    extracts[:] = [e for e in extracts if e.get("set_id") != set_id]
    extracts.append(extract)
    cat.save_manifest(args.domain, args.id, data)

    print(f"extracted {extract['frame_count']} frames to {frames_abs}")
    return 0


# ---------------------------------------------------------------------------
# list
# ---------------------------------------------------------------------------

def _summarize_clip(clip: dict) -> str:
    n_markers = len(clip.get("markers") or [])
    n_sets = len(clip.get("extracts") or [])
    n_frames = sum(int(e.get("frame_count") or 0)
                   for e in (clip.get("extracts") or []))
    dur_ms = int(clip.get("duration_ms") or 0)
    label = clip.get("label") or ""
    return (f"  {clip.get('clip_id','?'):28s}  {dur_ms:>5d}ms  "
            f"markers={n_markers:<2d} sets={n_sets:<2d} "
            f"frames={n_frames:<3d}  {label}")


def cmd_list(args: argparse.Namespace) -> int:
    root = cat.catalog_root()
    print(f"catalog root: {root}")
    # Convenience: if the first positional doesn't match any known
    # domain but does match an id under some domain, treat it as an id
    # filter. This is what makes `tools/refcap/list TBloodEffect` work
    # without needing to spell out the domain.
    domain = args.domain
    eid = args.id
    if domain and not eid:
        known_domains = {d.name for d in root.iterdir() if d.is_dir()}
        if domain not in known_domains:
            eid = domain
            domain = None

    found_any = False
    for d, i, data in cat.iter_manifests():
        if domain and d != domain:
            continue
        if eid and i != eid:
            continue
        found_any = True
        n_clips = len(data.get("clips") or [])
        desc = data.get("description") or ""
        print(f"\n[{d}/{i}] clips={n_clips}  {desc}")
        for clip in data.get("clips") or []:
            print(_summarize_clip(clip))
    if not found_any:
        scope = "/".join(filter(None, [domain, eid])) or "<all>"
        print(f"  (empty catalog for {scope})")
    return 0


# ---------------------------------------------------------------------------
# validate
# ---------------------------------------------------------------------------

def cmd_validate(args: argparse.Namespace) -> int:
    rep = cat.validate_catalog(domain_filter=args.domain, strict=args.strict)
    print(f"validated {rep.checked_manifests} manifest(s)")
    for w in rep.warnings:
        print(f"  warn: {w}")
    for e in rep.errors:
        print(f"  err : {e}", file=sys.stderr)
    if rep.ok:
        print("OK")
        return 0
    print(f"FAIL ({len(rep.errors)} error(s))", file=sys.stderr)
    return 1


# ---------------------------------------------------------------------------
# lookup
# ---------------------------------------------------------------------------

def cmd_lookup(args: argparse.Namespace) -> int:
    data = cat.load_manifest(args.domain, args.id)
    if data is None:
        print(json.dumps({"ok": False,
                          "error": f"no manifest for {args.domain}/{args.id}"}))
        return 1

    out: dict = {
        "ok": True,
        "domain": args.domain,
        "id": args.id,
        "id_dir": str(cat.id_dir(args.domain, args.id)),
        "manifest": str(cat.manifest_path(args.domain, args.id)),
    }

    clip = None
    if args.clip_id:
        clip = cat.find_clip(data, args.clip_id)
        if clip is None:
            print(json.dumps({"ok": False,
                              "error": f"clip_id '{args.clip_id}' "
                                       f"not found"}))
            return 1
    elif data.get("clips"):
        clip = data["clips"][0]

    if clip is not None:
        out["clip_id"] = clip.get("clip_id")
        out["video_path"] = str(
            cat.clip_video_abspath(args.domain, args.id, clip))
        out["markers"] = clip.get("markers") or []

        extract = None
        if args.set_id:
            extract = cat.find_extract(clip, args.set_id)
            if extract is None:
                print(json.dumps({"ok": False,
                                  "error": f"set_id '{args.set_id}' "
                                           f"not found"}))
                return 1
        elif clip.get("extracts"):
            extract = clip["extracts"][0]

        if extract is not None:
            fdir = cat.extract_frames_abspath(args.domain, args.id, extract)
            out["set_id"] = extract.get("set_id")
            out["frames_dir"] = str(fdir)
            pngs = sorted(fdir.glob("frame_*.png"))
            if args.frame is not None:
                idx = args.frame - 1
                if 0 <= idx < len(pngs):
                    out["frame_path"] = str(pngs[idx])
                else:
                    print(json.dumps({"ok": False,
                                      "error": f"frame {args.frame} out "
                                               f"of range (have "
                                               f"{len(pngs)})"}))
                    return 1
            else:
                out["frames"] = [str(p) for p in pngs]

    print(json.dumps(out, indent=2))
    return 0


# ---------------------------------------------------------------------------
# sync-index
# ---------------------------------------------------------------------------

def cmd_sync_index(_args: argparse.Namespace) -> int:
    p = cat.write_index()
    print(f"wrote {p}")
    return 0


# ---------------------------------------------------------------------------
# main
# ---------------------------------------------------------------------------

def _add_common_domain_id(sub: argparse.ArgumentParser) -> None:
    sub.add_argument("domain", help="domain (e.g. vfx, ui, gameflow)")
    sub.add_argument("id", help="effect/panel id (e.g. TBloodEffect)")


def build_parser() -> argparse.ArgumentParser:
    ap = argparse.ArgumentParser(
        prog="refcat",
        description="reference-footage catalog CLI; see "
                    "references/REFS_README.md for the catalog protocol")
    sub = ap.add_subparsers(dest="cmd", required=True)

    p_imp = sub.add_parser("import", help="copy a video into the catalog")
    _add_common_domain_id(p_imp)
    p_imp.add_argument("video", help="path to captured video file")
    p_imp.add_argument("--label", default="")
    p_imp.add_argument("--src", default="",
                       help="human-readable provenance (module + timer)")
    p_imp.add_argument("--clip-id", default=None,
                       help="defaults to the video's basename")
    p_imp.add_argument("--capture-tool", default="")
    p_imp.set_defaults(func=cmd_import)

    p_ext = sub.add_parser("extract", help="ffmpeg-extract PNG frames")
    _add_common_domain_id(p_ext)
    p_ext.add_argument("clip_id")
    p_ext.add_argument("--set-id", required=True,
                       help="extract set name (e.g. peak_window, full_8col)")
    mode = p_ext.add_mutually_exclusive_group()
    mode.add_argument("--every-ms", type=int, default=None,
                      help="periodic extraction interval")
    mode.add_argument("--at",
                      help="comma-separated absolute ms values "
                           "(e.g. 900,1000,1100)")
    p_ext.add_argument("--start-ms", type=int, default=None)
    p_ext.add_argument("--end-ms", type=int, default=None)
    p_ext.set_defaults(func=cmd_extract)

    p_ls = sub.add_parser("list", help="show catalog inventory")
    p_ls.add_argument("domain", nargs="?", default=None)
    p_ls.add_argument("id", nargs="?", default=None)
    p_ls.set_defaults(func=cmd_list)

    p_val = sub.add_parser("validate", help="check the catalog for drift")
    p_val.add_argument("--domain", default=None)
    p_val.add_argument("--strict", action="store_true",
                       help="rehash videos and compare to manifest")
    p_val.set_defaults(func=cmd_validate)

    p_look = sub.add_parser("lookup",
                            help="resolve a catalog tuple to disk paths")
    _add_common_domain_id(p_look)
    p_look.add_argument("clip_id", nargs="?", default=None)
    p_look.add_argument("--set-id", default=None)
    p_look.add_argument("--frame", type=int, default=None)
    p_look.set_defaults(func=cmd_lookup)

    p_sync = sub.add_parser("sync-index",
                            help="regenerate references/index.yaml")
    p_sync.set_defaults(func=cmd_sync_index)

    return ap


def main(argv: list[str] | None = None) -> int:
    ap = build_parser()
    args = ap.parse_args(argv)
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main())
