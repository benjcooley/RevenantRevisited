# Reference-footage catalog

This is the in-repo pointer to the **reference-footage catalog** — original
1998/99 Revenant gameplay video clips and extracted PNG frame sets used as
ground-truth A/B references when porting VFX, UI, and gameflow.

The catalog **bytes** live out-of-repo at `$REVENANT_REFS_DIR` (default
`~/RevenantRefs/`). Only the catalog **inventory pointer** (`index.yaml`,
listing `{domain, id, clip_id, sha256, frame_count}`) and this README live
in the repo so reviewers and CI can see what reference data the host
machine is expected to hold without dragging hundreds of MB of video
through git or LFS.

See [docs/vfx/AGENT_GUIDE.md](../docs/vfx/AGENT_GUIDE.md) §3.2 for how this
catalog plugs into the per-effect forensics + port workflow.

## Storage layout

```
$REVENANT_REFS_DIR/                          # ~/RevenantRefs by default
    <domain>/                                # vfx | ui | gameflow
        <id>/                                # e.g. TBloodEffect
            manifest.yaml                    # per-id catalog entry
            clips/<clip_id>.mp4              # raw captured videos
            frames/<clip_id>__<set_id>/      # ffmpeg-extracted PNG sets
                frame_01.png
                frame_02.png
                ...
```

`refcat` (under `tools/refcap/`) is the only thing that writes to this
tree. Don't hand-create directories or rename clip files — let `refcat
import` and `refcat extract` do it, otherwise the `frames_dir` /
`video_path` recorded in the manifest will drift from what's on disk.

## Manifest format (`manifest.yaml`)

YAML. Hand-editable for `markers` (time anchors) and `notes`; everything
else is filled in by `refcat` and shouldn't need manual edits.

```yaml
catalog_version: 1
domain: vfx
id: TBloodEffect
description: "Locke melee-hits an enemy; blood spawn (FLY) then SPLAT impact"
created: 2026-05-29
clips:
  - clip_id: c00_locke_dungeon
    schema_version: 1
    label: "Locke punch -> goblin, dungeon lighting"
    src: "Module M01 Ahkuilon village near scribe; in-game timer 02:14"
    video_path: clips/c00_locke_dungeon.mp4   # relative to manifest
    video_sha256: "ab12...cd"
    container: mp4
    codec: h264
    width: 1280
    height: 960
    fps: 60.0
    duration_ms: 4200
    capture_tool: "QuickTime screen recording, native 1280x960 fullwin"
    notes: "Fist swing at t=900ms; impact frame near t=1080ms"
    markers:                       # ground-truth time anchors (hand-added)
      - { t_ms: 1080, label: "impact",   note: "first SPLAT pixel" }
      - { t_ms: 1160, label: "peak_fly", note: "droplet count peak" }
      - { t_ms: 1900, label: "settle",   note: "last visible droplet" }
    extracts:                      # named frame sets (refcat extract)
      - set_id: peak_window
        mode: at
        at_ms: [1040, 1080, 1120, 1160, 1200, 1240]
        frames_dir: frames/c00_locke_dungeon__peak_window/
        frame_count: 6
        produced: 2026-05-29
      - set_id: full_8col
        mode: every
        every_ms: 80
        start_ms: 900
        end_ms: 1540
        frames_dir: frames/c00_locke_dungeon__full_8col/
        frame_count: 9
```

A few rules:

- `catalog_version: 1` is the current on-disk schema. `refcat validate`
  warns if it sees a different version.
- `clip_id`, `set_id`: `[a-z0-9_]+`. They show up in directory names,
  filenames, and snap_ab CLI args.
- `markers[].t_ms`: absolute ms into the clip; what you'd see if you
  scrubbed the video in QuickTime. The user hand-adds these after watching
  the clip.
- `extracts[].mode`: `at` (explicit ms list in `at_ms`) or `every`
  (periodic in `every_ms` between `start_ms` and `end_ms`). `refcat
  extract --at` and `--every-ms` pick the right one.

## Tools (`tools/refcap/refcat.py`)

Single CLI with subcommands. Each subcommand also has a thin shim binary
in `tools/refcap/` so `tools/refcap/list TBloodEffect` works directly.

| Command | What it does |
| --- | --- |
| `refcat import <domain> <id> <video> [--label STR] [--src STR] [--clip-id STR]` | Copy a captured video into the catalog, sha256 it, append a stub clip to the manifest. Probes container/codec/fps/duration via `ffprobe`. |
| `refcat extract <domain> <id> <clip_id> --set-id STR (--every-ms N \| --at MS1,MS2,...)` | ffmpeg-extract PNG frames into `frames/<clip_id>__<set_id>/`, record `frame_count` + `produced` date. |
| `refcat list [<domain>] [<id>]` | Print catalog inventory (clip_id, duration, marker count, frame sets). |
| `refcat validate [--domain D] [--strict]` | Walk catalog: missing videos, dangling extract dirs, frame_count vs files-on-disk mismatch, sha256 drift vs `index.yaml`, unknown `schema_version`. Nonzero exit on any issue. `--strict` re-hashes every video. |
| `refcat lookup <domain> <id> [<clip_id>] [--set-id STR] [--frame N]` | Resolve a tuple to absolute on-disk paths (JSON). `snap_ab.py` calls this. |
| `refcat sync-index` | Regenerate the in-repo `references/index.yaml` so reviewers/CI see catalog inventory without bytes. |

All tools require `ffmpeg`/`ffprobe`. If missing, they exit with
`brew install ffmpeg` guidance. Catalog root: `$REVENANT_REFS_DIR`
(falls back to `~/RevenantRefs/`).

## snap_ab `--ref` integration

`tools/vfx/snap_ab.py` gains three flags:

```
--ref <ID>[:<CLIP_ID>[:<SET_ID>]]
--ref-domain <DOMAIN>           # default: vfx
--ref-label STR                 # default: "reference"
```

When `--ref` is set and a manifest exists, snap_ab samples N frames
(`N = --cols`) from the chosen extract set, scales them to the harness
tile width preserving aspect (letterboxed — honest scale, not stretched),
and stacks them as the **TOP row** above bespoke + engine:

```
[ reference ]   row of cols tiles    <- top, --ref-label
[ bespoke   ]   row of cols tiles
[ engine    ]   row of cols tiles
```

Resolution rule (matches design):

- `clip_id` omitted -> first clip in the manifest
- `set_id` omitted -> prefer an `every` set whose `every_ms` ==
  `--interval-ms`, else the first set whose `frame_count >= cols`
- Frame selection: if `>= cols` frames, take the first `cols`; if `<`,
  repeat the last with a `(held)` caption suffix so columns still align;
  if `>`, stride-sample

Without `--ref`, snap_ab is byte-for-byte unchanged (2-row grid). If
`--ref` points at a missing/empty manifest, snap_ab prints a friendly
warning and falls back to the 2-row mode.

Output gains a `_ref` suffix:
`/tmp/TBloodEffect_BESPOKE_vs_TBloodEffect_ref.png`.

## Capture protocol (first capture)

1. **Launch the original game** in a clean state for the moment you want.
   Pause / quicksave just before the moment so you can re-do the capture
   if it goes wrong.
2. **Record with QuickTime Player**: File > New Screen Recording. Region-
   select tightly around the game window — Cmd-drag the client area; do
   **not** include the macOS title bar. Record at the game's **native**
   resolution (Classic 640x480, or whatever mode is running). Do **not**
   upscale — snap_ab handles tile scaling.
3. **Codec / container**: H.264, MP4 (QuickTime defaults are fine). 60 fps
   preferred, 30 fps acceptable. Each clip should be 5-30 seconds covering
   **one visual moment** (e.g. the FLY, the SPLAT, the bleed-out tail —
   one clip each, or one combined clip with markers).
4. **Name descriptively**: `blood_locke_dungeon_angle1.mov`. If the file
   isn't already MP4/H.264:
   - same codec, just rewrap: `ffmpeg -i in.mov -c copy out.mp4`
   - needs re-encode: `ffmpeg -i in.mov -c:v libx264 -crf 18 out.mp4`
5. **Import into the catalog**:

   ```
   tools/refcap/import_clip vfx TBloodEffect \
       /path/to/blood_locke_dungeon_angle1.mp4 \
       --label "Locke punch -> goblin, dungeon lighting" \
       --src "module M01 Ahkuilon village near scribe; in-game timer 02:14"
   ```

   This copies the video into `~/RevenantRefs/vfx/TBloodEffect/clips/`,
   computes sha256, probes the file, and writes a stub clip entry.

6. **Scrub for markers**: open the imported MP4 in any player, jot the
   `t_ms` of key moments (impact, peak, settle, dissipate), then hand-add
   a `markers:` block to `manifest.yaml`. The markers feed snap_ab's
   per-tile captions.
7. **Extract frame sets** you want available to A/B:

   ```
   # six frames spanning the SPLAT peak (matches a 6-col snap_ab grid)
   tools/refcap/extract_frames vfx TBloodEffect c00_locke_dungeon \
       --set-id peak_window --at 1040,1080,1120,1160,1200,1240

   # 80ms-stride sequence over the whole hit (matches default snap_ab interval)
   tools/refcap/extract_frames vfx TBloodEffect c00_locke_dungeon \
       --set-id full_8col --every-ms 80 --start-ms 900 --end-ms 1540
   ```

8. **Refresh the in-repo pointer**:

   ```
   tools/refcap/sync_index
   git add references/index.yaml
   ```

   This is the single artifact reviewers see in PRs.

9. **A/B against your port**:

   ```
   tools/vfx/snap_ab.py TBloodEffect_BESPOKE TBloodEffect \
       --ref TBloodEffect --cols 8 --interval-ms 80
   # -> /tmp/TBloodEffect_BESPOKE_vs_TBloodEffect_ref.png with reference
   # row on top, bespoke middle, engine bottom.
   ```

Audio: leave on in the capture (free metadata; v1 of the catalog consumes
video only).

## What lives in this directory

- `REFS_README.md` (this file) — catalog protocol + tool reference
- `index.yaml` — flat, sha256-keyed inventory of the on-disk catalog
  (regenerated by `refcat sync-index`)

That's it. No video, no PNGs — those are at `$REVENANT_REFS_DIR`.
