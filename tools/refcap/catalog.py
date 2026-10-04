"""Shared library for the reference-footage catalog.

All tools/refcap/* CLIs build on this module. There is exactly one place
that knows how to find the catalog root, read/write a manifest, probe a
video, and compute an extract path; the per-tool scripts are thin
arg-parsing wrappers.

Catalog layout (out-of-repo, default ~/RevenantRefs):

    $REVENANT_REFS_DIR/
        <domain>/                   e.g. vfx, ui, gameflow
            <id>/                   e.g. TBloodEffect, TPlyrStatusBar
                manifest.yaml       per-id manifest (this module owns format)
                clips/              <clip_id>.mp4 etc.
                frames/             <clip_id>__<set_id>/frame_NN.png

Manifest format: YAML, schema described in references/REFS_README.md.
The on-disk catalog_version is 1.

The in-repo pointer file (references/index.yaml) is a flat list of
{domain, id, clip_id, sha256, frame_count}; reviewers/CI see what
reference data the host machine is expected to hold without storing
the bytes in git. `refcat sync-index` regenerates it.
"""

from __future__ import annotations

import hashlib
import json
import os
import shutil
import subprocess
from dataclasses import dataclass, field
from datetime import date
from pathlib import Path
from typing import Any, Iterable, Optional

try:
    import yaml
except ImportError as e:  # pragma: no cover - obvious error path
    raise SystemExit(
        "error: PyYAML not installed. `pip3 install PyYAML`"
    ) from e


# Public version. Bump when the on-disk manifest schema changes.
CATALOG_VERSION = 1
DEFAULT_CATALOG_ROOT = Path.home() / "RevenantRefs"
SUPPORTED_DOMAINS = ("vfx", "ui", "gameflow")
INDEX_RELPATH = Path("references") / "index.yaml"


# ---------------------------------------------------------------------------
# Catalog root resolution
# ---------------------------------------------------------------------------

def catalog_root() -> Path:
    """Resolve the on-disk catalog root.

    Order: $REVENANT_REFS_DIR -> ~/RevenantRefs. The directory is
    created if missing (we don't want every tool to special-case the
    bootstrap of the host machine).
    """
    env = os.environ.get("REVENANT_REFS_DIR")
    root = Path(env).expanduser() if env else DEFAULT_CATALOG_ROOT
    root.mkdir(parents=True, exist_ok=True)
    return root


def id_dir(domain: str, eid: str, *, create: bool = False) -> Path:
    """Path to <root>/<domain>/<id>/. Optionally create + clips/ + frames/."""
    d = catalog_root() / domain / eid
    if create:
        (d / "clips").mkdir(parents=True, exist_ok=True)
        (d / "frames").mkdir(parents=True, exist_ok=True)
    return d


def manifest_path(domain: str, eid: str) -> Path:
    return id_dir(domain, eid) / "manifest.yaml"


# ---------------------------------------------------------------------------
# Manifest I/O
# ---------------------------------------------------------------------------

def empty_manifest(domain: str, eid: str, description: str = "") -> dict:
    return {
        "catalog_version": CATALOG_VERSION,
        "domain": domain,
        "id": eid,
        "description": description,
        "created": date.today().isoformat(),
        "clips": [],
    }


def load_manifest(domain: str, eid: str) -> Optional[dict]:
    """Return parsed manifest dict, or None if absent."""
    p = manifest_path(domain, eid)
    if not p.exists():
        return None
    with p.open("r") as f:
        data = yaml.safe_load(f) or {}
    return data


def save_manifest(domain: str, eid: str, data: dict) -> None:
    p = manifest_path(domain, eid)
    p.parent.mkdir(parents=True, exist_ok=True)
    # sort_keys=False preserves the structural ordering humans expect.
    with p.open("w") as f:
        yaml.safe_dump(data, f, sort_keys=False, default_flow_style=False)


def ensure_manifest(domain: str, eid: str, description: str = "") -> dict:
    """Load the manifest, creating an empty skeleton if missing."""
    data = load_manifest(domain, eid)
    if data is None:
        id_dir(domain, eid, create=True)
        data = empty_manifest(domain, eid, description=description)
        save_manifest(domain, eid, data)
    return data


# ---------------------------------------------------------------------------
# Hashing
# ---------------------------------------------------------------------------

def sha256_of(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


# ---------------------------------------------------------------------------
# Tool requirements
# ---------------------------------------------------------------------------

def require_ffmpeg() -> None:
    if shutil.which("ffmpeg") is None:
        raise SystemExit(
            "error: ffmpeg not found on PATH. Install with "
            "`brew install ffmpeg` and re-run."
        )


def require_ffprobe() -> None:
    if shutil.which("ffprobe") is None:
        raise SystemExit(
            "error: ffprobe not found on PATH. Install with "
            "`brew install ffmpeg` and re-run."
        )


# ---------------------------------------------------------------------------
# Video probe
# ---------------------------------------------------------------------------

@dataclass
class ProbeResult:
    container: str = ""
    codec: str = ""
    width: int = 0
    height: int = 0
    fps: float = 0.0
    duration_ms: int = 0


def probe_video(path: Path) -> ProbeResult:
    """ffprobe wrapper. Returns ProbeResult with best-effort fields."""
    require_ffprobe()
    cmd = [
        "ffprobe", "-v", "error",
        "-print_format", "json",
        "-show_format", "-show_streams",
        str(path),
    ]
    raw = subprocess.check_output(cmd).decode("utf-8", "replace")
    info = json.loads(raw)
    fmt = info.get("format", {}) or {}
    streams = info.get("streams", []) or []
    v = next((s for s in streams if s.get("codec_type") == "video"), {})

    container = (fmt.get("format_name") or "").split(",")[0]
    duration_ms = int(float(fmt.get("duration") or 0.0) * 1000)
    codec = v.get("codec_name") or ""
    width = int(v.get("width") or 0)
    height = int(v.get("height") or 0)

    # avg_frame_rate "60000/1001" or "60/1"; parse safely.
    fps_raw = v.get("avg_frame_rate") or v.get("r_frame_rate") or "0/1"
    fps = 0.0
    try:
        num, den = fps_raw.split("/")
        d = float(den)
        if d != 0:
            fps = float(num) / d
    except (ValueError, ZeroDivisionError):
        fps = 0.0

    return ProbeResult(
        container=container, codec=codec,
        width=width, height=height,
        fps=fps, duration_ms=duration_ms,
    )


# ---------------------------------------------------------------------------
# Clip helpers
# ---------------------------------------------------------------------------

def find_clip(data: dict, clip_id: str) -> Optional[dict]:
    for c in data.get("clips", []) or []:
        if c.get("clip_id") == clip_id:
            return c
    return None


def find_extract(clip: dict, set_id: str) -> Optional[dict]:
    for e in clip.get("extracts", []) or []:
        if e.get("set_id") == set_id:
            return e
    return None


def clip_video_abspath(domain: str, eid: str, clip: dict) -> Path:
    rel = clip.get("video_path") or ""
    return id_dir(domain, eid) / rel


def extract_frames_abspath(domain: str, eid: str, extract: dict) -> Path:
    rel = extract.get("frames_dir") or ""
    return id_dir(domain, eid) / rel


# ---------------------------------------------------------------------------
# In-repo index (references/index.yaml)
# ---------------------------------------------------------------------------

def iter_manifests() -> Iterable[tuple[str, str, dict]]:
    """Yield (domain, id, manifest_dict) for every manifest under the root."""
    root = catalog_root()
    for domain_dir in sorted(p for p in root.iterdir() if p.is_dir()):
        domain = domain_dir.name
        for id_dir_ in sorted(p for p in domain_dir.iterdir() if p.is_dir()):
            mp = id_dir_ / "manifest.yaml"
            if not mp.exists():
                continue
            with mp.open("r") as f:
                data = yaml.safe_load(f) or {}
            yield domain, id_dir_.name, data


def repo_root_from_here() -> Path:
    """Return the repository root by walking up from this file."""
    here = Path(__file__).resolve()
    # tools/refcap/catalog.py -> tools/refcap -> tools -> <repo root>
    return here.parent.parent.parent


def index_path() -> Path:
    return repo_root_from_here() / INDEX_RELPATH


def build_index() -> dict:
    """Build the in-repo pointer document from the on-disk catalog."""
    entries: list[dict] = []
    for domain, eid, data in iter_manifests():
        for clip in data.get("clips", []) or []:
            clip_id = clip.get("clip_id") or ""
            sha = clip.get("video_sha256") or ""
            total_frames = 0
            for ex in clip.get("extracts", []) or []:
                total_frames += int(ex.get("frame_count") or 0)
            entries.append({
                "domain": domain,
                "id": eid,
                "clip_id": clip_id,
                "sha256": sha,
                "frame_count": total_frames,
            })
    return {
        "catalog_version": CATALOG_VERSION,
        "generated": date.today().isoformat(),
        "entries": entries,
    }


def write_index() -> Path:
    """Regenerate references/index.yaml. Returns the path written."""
    doc = build_index()
    p = index_path()
    p.parent.mkdir(parents=True, exist_ok=True)
    with p.open("w") as f:
        yaml.safe_dump(doc, f, sort_keys=False, default_flow_style=False)
    return p


# ---------------------------------------------------------------------------
# Validation
# ---------------------------------------------------------------------------

@dataclass
class ValidationReport:
    errors: list[str] = field(default_factory=list)
    warnings: list[str] = field(default_factory=list)
    checked_manifests: int = 0

    def add_error(self, msg: str) -> None:
        self.errors.append(msg)

    def add_warning(self, msg: str) -> None:
        self.warnings.append(msg)

    @property
    def ok(self) -> bool:
        return not self.errors


def validate_catalog(
    *, domain_filter: Optional[str] = None, strict: bool = False
) -> ValidationReport:
    """Walk every manifest. Check video existence, sha256 freshness vs
    in-repo index, extract dir presence + frame count, schema version.
    """
    rep = ValidationReport()

    # Snapshot of the in-repo index for sha drift detection (best effort).
    indexed: dict[tuple[str, str, str], str] = {}
    ip = index_path()
    if ip.exists():
        with ip.open("r") as f:
            doc = yaml.safe_load(f) or {}
        for e in doc.get("entries", []) or []:
            key = (e.get("domain") or "", e.get("id") or "",
                   e.get("clip_id") or "")
            indexed[key] = e.get("sha256") or ""

    for domain, eid, data in iter_manifests():
        if domain_filter and domain != domain_filter:
            continue
        rep.checked_manifests += 1

        cv = data.get("catalog_version")
        if cv != CATALOG_VERSION:
            msg = (f"{domain}/{eid}: catalog_version={cv!r} "
                   f"(expected {CATALOG_VERSION})")
            rep.add_warning(msg)

        for clip in data.get("clips", []) or []:
            clip_id = clip.get("clip_id") or "<no clip_id>"
            label = f"{domain}/{eid}/{clip_id}"

            # Video on disk?
            vp = clip_video_abspath(domain, eid, clip)
            if not clip.get("video_path"):
                rep.add_error(f"{label}: missing video_path")
            elif not vp.exists():
                rep.add_error(f"{label}: video missing on disk ({vp})")
            else:
                # SHA drift vs in-repo index.
                expected = indexed.get((domain, eid, clip_id))
                actual = clip.get("video_sha256") or ""
                if expected and actual and expected != actual:
                    rep.add_error(
                        f"{label}: video_sha256 drift "
                        f"(index={expected[:10]}.. manifest={actual[:10]}..)"
                    )
                if strict:
                    if not actual:
                        rep.add_error(f"{label}: missing video_sha256")
                    else:
                        live = sha256_of(vp)
                        if live != actual:
                            rep.add_error(
                                f"{label}: video bytes changed "
                                f"(manifest={actual[:10]}.. "
                                f"live={live[:10]}..)"
                            )

            for ex in clip.get("extracts", []) or []:
                set_id = ex.get("set_id") or "<no set_id>"
                elabel = f"{label}/{set_id}"
                fdir = extract_frames_abspath(domain, eid, ex)
                if not ex.get("frames_dir"):
                    rep.add_error(f"{elabel}: missing frames_dir")
                    continue
                if not fdir.exists():
                    rep.add_error(f"{elabel}: frames_dir missing ({fdir})")
                    continue
                pngs = sorted(fdir.glob("*.png"))
                want = int(ex.get("frame_count") or 0)
                if want and want != len(pngs):
                    rep.add_error(
                        f"{elabel}: frame_count={want} but "
                        f"{len(pngs)} PNGs on disk"
                    )

    return rep
