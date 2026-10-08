# Archived DOSBox-X workflow

Historical instructions only. DOSBox-X is retired for VFX validation as of 2026-10-07.
Use [the thin-emulator workflow](../RETAIL_AB_TESTING.md) for all new VFX work.

# Retail Revenant A/B testing on this Mac

Updated 2026-10-06. The priority is rapid, accurate effect verification. Use
software rendering for new reference work while the hardware depth path is
under investigation. Startup/scripting work is being handled separately; check
the installed tool schemas and scripts rather than assuming source changes
have already been deployed.

## Where everything lives

- Retail lab: `/Users/benjamincooley/RevenantRetailLab`
- Patched emulator: `/Users/benjamincooley/RevenantRetailLab/vendor/dosbox-x/src/dosbox-x`
- Installed Windows 98 disk: `/Users/benjamincooley/RevenantRetailLab/win98-control-test.img`
- Emulator configurations: `win98-base.conf` and `win98-control-test.conf` in the lab.
- MCP server deployed on the Mac: `/Users/benjamincooley/RevenantRetailLab/dosbox_x_mcp.py`
- Emulator control socket: `RevenantRetailLab/control.sock`
- Guest agent serial socket: `RevenantRetailLab/guest-serial.sock`
- Guest agent: `C:\MCP\GUESTCTL.EXE`; its startup copy is under
  `C:\WINDOWS\Start Menu\Programs\StartUp`.
- Guest Lua scripts: `C:\MCP\SCRIPTS`; deployed Mac copies are in
  `RevenantRetailLab/guest-agent/scripts`.
- Retail executable used in Windows: `C:\REVENANT\REV98.EXE`.
- Original host game files: `/Users/benjamincooley/RevenantRetailLab/retail-cd/REVENANT`.
- Port repository: `/Users/benjamincooley/projects/RevenantRevisited/RevenantRevisited`.
- Port build and executable: `/Users/benjamincooley/projects/RevenantRevisited/build/Revenant`.
- Emulator, MCP, guest-agent and capture source: [`../tools/win98-agent`](../tools/win98-agent).

The patched lab executable provides shared human/agent input, exact guest
coordinates, screenshots and native video recording. The older
`launch-win98.sh` launches the Homebrew emulator; use the registered
`guest_start` tool for this workflow.

The mounted `D:` drive is a CD-ROM. Its contents depend on the current emulator
configuration; inspect them rather than treating `D:` as a writable game disk.

## Start and inspect through MCP

Call these registered tools in order:

1. `guest_start({})` — attaches to the existing runner or starts it. The current
   launcher refuses duplicate runners and an already-open guest disk.
2. `guest_status({})` and `guest_screen({})` — confirm emulator/display access.
3. `guest_agent_status({})` — wait for Windows serial RPC, screen dimensions and
   a completed/idle Lua job.
4. `guest_inspect_retail({})` — inspect game processes, window responsiveness and
   error dialogs before launching or changing scenes.

Zero retail processes is a valid state before launch. A loaded game should have
exactly one responsive Revenant window/process and no fatal modal dialog.
`focus` failing does **not** mean a process exited. Scripts use
`guest.window_exists` and a separate `CONTROLPROBE.EXE` process check.

Only one controller may operate this guest at a time. Other agents should work
from captured references and their own port outputs, then request the guest
when they need another retail capture. Never open the installed disk for
offline writes while DOSBox has it open. Offline installation checks `lsof`;
keep a disk snapshot before changing the installed agent.

Audio is disabled with `[mixer] nosound=true`. Do not add DOSBox `-silent` to
this runner: it selects a dummy video driver and prevents Voodoo OpenGL startup.

## Guest controls and reusable scripts

- `guest_run_command({"name":"editor_command","parameters":{"command":"..."}})`
  submits a paced retail editor console command.
- Poll `guest_command_status({})` until `running` is false, and inspect its error
  and log before the next command. A submitted command is not proof that the
  game accepted it; check the screenshot at scene transitions.
- `guest_run_command({"name":"key","parameters":{"keys":["ctrl","shift","f"]}})`
  sends a guest-timed chord. `guest_type`, `guest_move_to`, `guest_click` and
  `guest_click_at` provide direct tools as well.
- `guest_upload`, `guest_read_file`, `guest_write_file`, `guest_list_files` and
  `guest_copy` handle guest files. Video and large assets stay on the Mac.
- Upload `.lua` files to `C:\MCP\SCRIPTS`; a script returns a function accepting
  a parameter table. Updating Lua does not require rebuilding the guest app.

`open_editor_software.lua` is the new guarded software launcher source. Its
launch options are `EDITOR DEVICE=display SOFTWARE3D`. This launcher was uploaded
to the guest and used successfully for `sw-vfx-fast-01-flame`. The newer
software VFX preparation scripts still require deployment and host integration
by the scripting owner. The already-existing `guest_capture_flame` tool accepts
`renderer="software"`; its earlier software captures passed the scene and
stale-pixel checks described below.

## Capture a software reference

The isolated fixture is module `MCP_AB`, level 100, camera `(10000,10000,16)`.
Its saved baseline contains nine `Dunffff` ground tiles. Resetting this fixture
discards its unsaved edits; do not reset unrelated editor work.

For the existing software Flame capture pipeline, call:

```text
guest_capture_flame({
  "capture_id": "sw-flame-unique-01",
  "renderer": "software",
  "duration_seconds": 4
})
guest_capture_status({})
```

Poll the status tool until completion. Use a new ID for every run. This older
fixture uses ambient 128 and is useful for control/animation regression; prefer
a lower, recorded ambient setting for new visual fidelity references.

For a new effect/reference recipe, use the editor tools to establish the saved
fixture and the exact effect settings, with screenshots between stages:

1. Verify the ground-only scene, camera and lighting. Capture an **effect-free**
   ground PNG before spawning the effect. `guest_screen` writes
   `RevenantRetailLab/captures/mcp-current.png`; copy it to the reference directory
   before another screenshot overwrites it.
   In the disposable MCP_AB fixture, compare the fresh viewport below the
   top twelve FPS rows against a previously verified empty floor at the same
   camera/ambient. Reject any residual pixels before starting another capture.
   `captures/runs/sw-fps-fireflash-20261005/ground.png` is a verified baseline
   for camera `(10000,10000,16)`, ambient32 and this nine-tile floor. FireWind
   left edge residue even with FPS enabled; the first subsequent FireCone
   capture was correctly retained without reference credit and repeated after
   camera cleanup. For other maps, use that scene's own verified baseline.
2. Before every software reference, verify the yellow `Frames:` readout is
   visible at the top of the guest screen. If absent, enable it with
   Ctrl+Shift+F and take a fresh ground screenshot. Do not toggle blindly:
   repeated chords turn it off. Original `mainwnd.cpp:238–246` handles this
   without an editor/debug prerequisite. In the 2026-10-05 recovery,
   FireSwarm accumulated frozen trails with the readout absent, then animated
   and restored exact ground after natural expiry with it enabled. The retained
   clean reference is `captures/runs/sw-fps-fireswarm-20261005`. A responsive
   game and changing images alone cannot validate this prerequisite.
3. Restore the camera explicitly with `map 10000 10000 16`, park the guest
   pointer over the console, and verify the empty backdrop again. Host window
   activity can disturb the view; background guards must reject a shifted
   fixture. Run retail recordings and port renders serially.
4. Place the effect using paced console commands. For Flame, the existing recipe
   is `add Flame`, `pos 10000 10000 96`, then `deselect all`.
5. Use `guest_recording_start`, wait the requested host duration, then
   `guest_recording_stop`. Recording is provided by DOSBox, not the game's BMP
   recorder. For short bursts, start recording **before** spawning the effect.
6. Retain the lossless AVI, ground PNG, before/after screenshots and a manifest
   with renderer, effect parameters, camera, lighting and hashes. Check changing
   effect frames and stable background pixels before accepting the reference.

For Fizzle and other short bursts, begin recording first, then submit
`addat 10000 10000 Fizzle` and `deselect all`. `addat` supplies XY atomically;
Z comes from the restored camera center (16 here). A separate `pos` command
arrives after the burst has already started. Keep the whole native recording,
find the first effect-region activity against the empty ground, and retain a
lossless onset derivative plus a blank tail. Check early frames for selection
overlays; the verified Fizzle recording has none. Ctrl+Shift+N is gated by
retail debug controls and did not freeze this session; the working protocol
requires no freeze or debug change.

Use `TFizzleEffect_SINGLE_BURST` for the corresponding port lifecycle test.
The regular `TFizzleEffect` preview automatically fires another burst and
cannot prove an uninterrupted blank tail. Compare elapsed time without phase
search for this test. Retail recording FPS, distinct visible images and
simulation ticks are different measurements.

The exercised Fizzle protocol is retained in the lab's
`capture_fizzle_lifecycle_protocol.py` and
`repeat_fizzle_lifecycle_protocol.py`. They call registered MCP tools through
a protocol client. Completed native runs are `sw-burndown-06-fizzle` and
`sw-burndown-08-fizzle-repeat`; the camera-shifted `07` preflight was rejected
before recording. The post-numerical-fix paired results are
`captures/ab/sw-burndown-06-fizzle-single-burst-v2` and
`captures/ab/sw-burndown-08-fizzle-cached-port-v2`. The latter reuses 150
hash-checked port images and produces the comparison offline in about
1.57 seconds, with no guest calls. Reuse the recorded renderer, camera,
ambient, particle quality, ground, asset/definition and binary provenance; a matching effect
name alone is not a valid cache key.

For current per-effect status and remaining runtime work, use
[vfx/EFFECT_BURNDOWN.md](vfx/EFFECT_BURNDOWN.md), rather than treating a
completed capture as an accepted effect.

Previously checked software recordings are
`captures/runs/flame-fps-01` and `flame-fps-02` in the lab. Their accompanying
`render-verification.json` files record the later per-frame rectangle checks.
Older artifacted software recordings are diagnostics, not usable baselines.

## Make the port side and compare

The isolated port harness accepts an effect preview ID, an effect-free retail
backdrop, the matched camera/origin, and fixed simulation capture timing. For
Flame at retail Z=96 with camera Z=16, the relative origin is `(0,0,80)`.

Example port capture, using a ground PNG recorded for this exact scene:

```sh
REVENANT_DATA_PATH=/Users/benjamincooley/projects/RevenantRevisited/RevenantRevisited/data \
/Users/benjamincooley/projects/RevenantRevisited/build/Revenant \
  --test=vfx --vfx=TFlameEffect --vfx-no-ui --headless \
  --vfx-backdrop=/ABSOLUTE/PATH/TO/ground.png \
  --vfx-camera=320,170 --vfx-origin=0,0,80 \
  --filmstrip=150,0.0333333333333333 \
  --snapstep=0.0166666666666667 --snapseed=1 --snapwarmup=1 \
  --snaprect=0,0,640,340 --snapprefix=/ABSOLUTE/NEW/OUTPUT/frame-
```

### Isolate port VFX lighting mode — 2026-10-05

`--vfx-lighting-mode=auto|classic|modern` is a diagnostic for `--test=vfx`:

- `auto` (also the omitted default) preserves existing behavior: an explicit `--scene-ambient=...` profile selects classic mode 0; otherwise modern mode 1.
- `classic` explicitly selects mode 0 after the same ambient/directional inputs are uploaded.
- `modern` explicitly selects mode 1 after those identical inputs are uploaded.

Use a fresh output directory for each variant of the same capture command, changing only this flag. Record the explicit/omitted choice, resolved mode, source ambient settings, complete command and binary/asset/backdrop/timing/seed hashes in the manifest and cache key. The selector covers the host port VFX render path; the original guest reference retains its recorded device/lighting settings. A mode diagnostic does not justify editing effect geometry, motion, materials or matching video phase.

Root verified Gold on binary `69d7dc13558990d33b6590454db207190f67fd7674921bf665f177e6c3d5b89d`: classic, modern and default auto each 240 raw frames exactly equal one another and retained 60026/f4 corrected Gold output. The older a89 predecessor reproduced its 44-frame difference, but same-binary isolation rules out lighting mode as its cause. That historical cause remains unresolved; original retail trajectory/collision/visible-duration parity still needs separate evidence. [Isolation record](/Users/benjamincooley/RevenantRetailLab/research/kinsecretdoor-20261005/gold-mode-isolation.json).

Primary implementation: [testconfig.h](../src/testconfig.h), [testconfig.cpp](../src/testconfig.cpp), [revmain.cpp](../src/revmain.cpp) and [vfxtest.cpp](../src/vfxtest.cpp). Invalid values or a missing flag value are rejected. The guide records those four source files and tested capture evidence; no new unpersisted source-contract test count is claimed.

### Match the retail particle quality

Record particle quality before comparing authored `partsys` effects. A lower
retail quality setting changes emission and capacity; a denser port can be a
settings mismatch. For this installed retail executable, read the setting
without changing the game through the installed guest helper:

```json
{
  "name": "read_retail_memory",
  "parameters": {
    "requests": [{"addr": "005d79e4", "size": 4}]
  }
}
```

Submit that object to `guest_run_command`, await the same job with
`guest_command_status`, then read `C:\MCP\RETREAD.JSON` through
`guest_read_file`. Decode the four bytes as a little-endian integer and retain
the complete report, process identity and job result. This address is specific
to the verified original executable used here.

The port accepts `--partsys-quality=0|1|2` in map and VFX captures. Its omitted
default is 0. Retail quality 1 reduces constant emission to one quarter, and 2
to one half; curved emission follows its separately recovered source policy.
Use the measured value and include it in the capture manifest and cache key.
Do not change authored PPS, scale, lifetime or color to compensate.

Might demonstrated this on 2026-10-05: guest job 267 read quality 1, while the
first port recording used 0. The matched capture uses
`--partsys-quality=1`, preserving authored PPS 30 while the original controller
initializes PPS 7.5 and capacity 7. Its steady appearance passes against retail;
the full-quality recording remains historical evidence of mismatched settings.
See [the Might audit](vfx/forensics/MIGHT_AUTHORED_TAGS.md) and
[the completed comparison](/Users/benjamincooley/RevenantRetailLab/captures/ab/sw-fps-might-quality1-actual-map-20261005/comparison-elapsed.mp4).

For Immortalmight, also retain the incoming scene blend state. Its RGB565
base flare has no object blend override and inherits the last particle's
additive state. Before the first particle exists, the original renderer
preserves the caller's state. The capture option
`--partsys-incoming-blend=auto|16|80` affects only this exact profile: use 16
for the measured additive/depth-tested fixture; 80 is an explicit diagnostic
without depth testing. The omitted `auto` keeps the port's visible ordinary
cold mesh route, whose parity with an unmeasured original caller is unproven.

Verified original state locations are SRCBLEND DWORD `00675fc8`, DESTBLEND
DWORD `00675f10`, ZWRITE byte `005e595c` and ZENABLE byte `005e595d`.
Read them with the same guest helper before spawning, retaining the complete
report across 1024-byte chunks when necessary. Job 273 measured source/dest
2/2 and Z write/test 0/1 before the repeated Immortalmight capture. Also retain
the requested blend DWORD `005e88b4`; backend factors alone can correspond to
more than one source mode. `00675f0c` is part of the WORLD matrix and is not
SRCBLEND. See [the independent ABI proof](/Users/benjamincooley/RevenantRetailLab/research/retail-render-state-abi-20261005/abi-validation.json)
and [the Immortalmight audit](vfx/forensics/IMMORTALMIGHT_AUTHORED_TAGS.md).

Use an existing, empty output directory. The extra 30 frames allow one second of phase estimation for a
four-second comparison. Warmup is effect-specific: Drip's established isolated
recipe uses 480 simulation ticks to accumulate rings; short bursts must not be
warmed past their visible lifetime. Keep `frame-timing.csv` and `run.log`.
Some test-mode teardown paths remain running after writing the filmstrip. The
offline helper below waits for complete frames/timing, then ends only its owned
port process; use it for batch work.

[`host/ab_compare.py`](../tools/win98-agent/host/ab_compare.py) contains the reusable
`port_frames` and `render_comparison` functions. Pass the recorded ground PNG
explicitly to both. `render_comparison` expects a reference directory containing
`retail.avi`; `port_frames` also creates numbered `port/sequence` images. They
generate elapsed-time and optional single-offset phase comparisons. This is
Mac-side frame/media processing; guest control remains through MCP.

For a complete offline Flame comparison from that saved software reference,
the following reuses those functions and generates the paired previews without
operating Windows. The reference directory must contain `retail.avi` and the
effect-free `ground.png` from the matched camera/lighting:

```sh
cd /Users/benjamincooley/RevenantRetailLab
AB_REFERENCE=/ABSOLUTE/PATH/TO/REFERENCE \
AB_OUTPUT=/ABSOLUTE/NEW/COMPARISON/DIRECTORY \
./.venv/bin/python - <<'PY'
import asyncio, json, os
from pathlib import Path
import ab_compare as ab

async def run():
    reference = Path(os.environ['AB_REFERENCE'])
    output = Path(os.environ['AB_OUTPUT'])
    output.mkdir(parents=True, exist_ok=False)
    binary = ab.DEFAULT_BINARY.resolve()
    ground = reference / 'ground.png'
    definitions = ab.snapshot_effect_definitions(ab.port_data_root(binary), output)
    command, frames, _ = await ab.port_frames(
        output / 'port', binary, 150, 'TFlameEffect',
        backdrop=ground, origin=(0, 0, 80), warmup_frames=1)
    alignment = await asyncio.to_thread(ab.render_comparison,
        output, reference, frames, 120, ground, (275, 35, 365, 170),
        ignored_top_rows=12, reference_label='Original software')
    (output / 'manifest.json').write_text(json.dumps({
        'status': 'complete', 'renderer': 'software', 'reference': str(reference),
        'reference_sha256': ab.digest(reference / 'retail.avi'),
        'ground_sha256': ab.digest(ground), 'binary_sha256': ab.digest(binary),
        'definitions': definitions, 'port_command': command, 'alignment': alignment,
        'scope': 'Isolated Flame appearance/animation; map/depth fidelity excluded'
    }, indent=2) + '\n')

asyncio.run(run())
PY
```

This offline workflow was run successfully against
`captures/runs/sw-vfx-fast-01-flame`, a new ambient 32 software reference with
an effect-free ground PNG. Outputs are `captures/ab/sw-vfx-fast-01-flame`.
The native check found ten distinct effect images and background error 0.423
(below the threshold 2); the paired preview uses a single estimated phase offset
and no position/scale fitting. This proves the isolated workflow, not complete
Flame fidelity or map/depth parity.

Software FPS/debug text changes in the first 12 rows. The explicit
`ignored_top_rows=12` excludes only that band from background validation and
records the exclusion in the comparison manifest. Floor movement still fails
the check. Keep the effect ROI below that text when comparing its appearance.

The older `fixtures/flame-ground*.png` images include a visible Flame. They are
scene-validation pictures; using them unchanged as effect backdrops would bake
a second Flame into the port image. Use the new effect-free ground capture.

The existing registered automated comparison tools are:

- `guest_compare_vfx` / `guest_vfx_status`: Flame, Drip and editor-default Sparks.
- `guest_compare_flame` / `guest_compare_status`: the older isolated Flame route.
- `guest_compare_map_vfx` / `guest_map_vfx_status`: shipped City torches and Caverns Drip.

**Current integration limitation:** deployed comparison hosts still assume 3dfx
references in several paths. A software Lua script alone does not make those
hosts software-compatible. The scripting/startup owner must connect renderer
selection, matching readiness checks and renderer-specific cache keys before
using them for new automated software baselines. Keep software and hardware
references separate. Do not silently compare a software capture against a
cached hardware recording.

Once an appropriate reference is validated, reuse it for port iterations.
Capture IDs remain unique; changing effect definitions requires another port
capture even when the executable has not changed. Save the executable hash and
the actual runtime `effects.def` with each comparison.

## Evidence, accuracy and map scenes

- Retail evidence: `RevenantRetailLab/captures/runs/<capture_id>/`.
- Paired outputs: `RevenantRetailLab/captures/ab/<capture_id>/`.
- Inspect `comparison-elapsed.mp4`, `comparison.png`, native AVI and manifest;
  use the phase-estimated preview as an additional view, not a replacement.
- Retail simulation and RNG are not synchronized with the port. The port uses
  fixed steps and seed. Do not fit per-frame positions, scale or colors to hide
  discrepancies. Repeat port frames to check determinism.
- An isolated effect over a recorded backdrop verifies VFX appearance and
  animation. It does not verify the port's map renderer, light pools or occlusion.
- For map effects, use original placements and matched camera/lighting in both
  real map renderers. Current recipes are `torches_city` (Ahkuilon level41,
  camera10603,10477,16) and `cave_drip` (level30, camera8159,9591,229).
  The [shipped placement catalogue](vfx/forensics/FIRST_BATCH_MAP_PLACEMENTS.md)
  adds the actual BlueFont altar pair in level 58, camera (10162,7858,416),
  ambient 24, RGB (250,150,250), plus Cyan/Red story contexts and the next water/geyser
  settings. Preserve each object's saved identity, height, mounting and lights.
- The current BlueFont actual-map pairing is retained at
  `captures/ab/sw-insitu-bluefont-labyrinth-20261004/`. Both saved MapIndexes
  attach once and animate in the port. Native software has only 21 distinct scene
  images over 18 seconds; widgets remain visible and shared scene lighting
  differs. Use this as a placement/lighting diagnostic; retain the fast isolated
  clips for animation iteration until the native full-scene cadence improves.
- Port command lifecycle tests use `--test=sector --scene-command-file=<path>`:
  each row is `legacy_tick | context_type_or_name_or_- | command`. ADD/ADDAT,
  MOVE and DELETE go through the real interpreter and map registry; `@expect`
  and `@absent` only observe identity, position and component count. See
  [runtime evidence](vfx/forensics/RUNTIME_FIRST_BATCH.md) for the 38 passing rows.
  Numeric retail ADDAT accepts `x y [amount] type`; Z inherits map-center Z,
  then new-object creation clamps negative Z to zero. Saved-sector loading
  separately preserves authored negative heights. EFFECT has no Amount stat;
  its `Amount()` fallback remains one even if the requested numeric amount is7.
  This verifies lifecycle without accepting authored story/cast execution.
- MistFog and literal Waterfall now also have successful exact saved-record
  and command tests in
  `captures/runtime-fixtures/environment-saved-20261004/validated-runtime-results.json`.
  Four captures verify one effect-state component per owner, animation and
  14 passing command rows; both deletion tails are stable ground. These are
  original records on synthetic surrounding tiles, not full original-map
  visual acceptance. See [MistFog](vfx/forensics/M06_MISTFOG_RUNTIME.md) and
  [Waterfall](vfx/forensics/H02_WATERFALL_RUNTIME.md) for source/asset fixes and
  remaining lighting gates. The new native software attempts showed no effect
  animation and remain failed references; a working Mist control is retained.
- Authored controller effects require a different fixture from particle
  waterfall previews. [Water dispatch](vfx/forensics/WATER_VARIANT_DISPATCH.md)
  proves WFall/WCap use `partsys` tags, while
  [RiverFall](vfx/forensics/RIVERFALL_SCROLLTEX.md) uses `scrolltex`.
  RiverFall's restored ordinary mesh path passes create/move/persist/delete
  and visibly scrolls with the shared24Hz clock. Its native software attempt
  remains rejected. The four particle waters now use source-backed authored
  `partsys` through the default animator; six emitters and original 140/52-slot
  pools animate across loops. Retail ignores the stored tagframe5 here.
  Optional timeline `@frame N` observes the actual frame without changing it.
  `@partsys controllers emitters capacity min_alive min_ticks min_quads`
  observes controller state without advancing it. See
  [partsys audit](vfx/forensics/PARTSYS_RUNTIME.md).
  Root-owned lab `run_partsys_water_runtime.py` captures exact saved records;
  `prepare_partsys_insitu_fixtures.py` prepares unchanged nine-sector original
  Pit/Pyramid slices and `run_partsys_insitu.py` renders them serially. Retain
  fresh output directories, frozen binary hashes and runtime manifests. Current
  evidence is under `captures/runtime-fixtures/partsys-water-20261005/` and
  `partsys-insitu-20261005/`; native water references and visual sign-off remain
  open. Full-map clips retain widgets and shared lighting/projection limits.
- **Do not use `Light.toggle invisible on` to get accurate marker-free scenes.**
  It removes the Light from the lighting update list and its light pool disappears.
  A retail widget-only hotkey has not yet been verified. Keep markers visible
  and identify obscured ROIs until the correct control is established.
- The old editor snapshot's F12 behavior cannot be assumed to match retail.
  Verify the retail key's action and camera before adding it to a capture recipe.
- Hardware reference acceptance is currently held: the rotating waypoint marker
  reportedly self-occludes incorrectly, and the emulator has concrete raw-depth
  write/readback issues. Retain those recordings as diagnostics.

## When the MCP connection is closed

`Transport closed` refers to the client/server connection, not necessarily a
dead emulator. Refresh/reload the MCP client connection; preserve the running
Windows disk. A fresh protocol diagnostic client can use the registered tools:

```sh
cd /Users/benjamincooley/RevenantRetailLab
./.venv/bin/python inspect_live_guest.py --probe-processes --screen captures/health.png
```

`--start` adds guarded startup and five-second boot health updates.
`--screen-only --screen captures/boot.png` works before the guest agent is ready.
These Python files are MCP integration clients; they invoke registered tools.
The ordinary workflow uses the tools directly.

See also [`tools/win98-agent/README.md`](../tools/win98-agent/README.md),
[`CONTROLPROBE.md`](../tools/win98-agent/CONTROLPROBE.md), and the effect audits
under [`docs/vfx/forensics`](vfx/forensics).

## Static effects and floor occlusion

Some registered VFX are fixed authored meshes. Globe `0xad92bd24` has a fixed
STILL pose and no controller tags; a visible unchanged clip is a valid retail
reference. For such a profile, `ab_compare.port_frames(...,
require_animation=False)` accepts static output while requiring visible pixels
beyond the diagnostic band. Keep animation required for genuinely animated
profiles.

A baked retail floor PNG supplies color but no depth. Effects that cross the
floor plane need comparison through the real map renderer. Globe's lower rim
is hidden by actual tiles but exposed in the baked-background preview. Retain
that diagnostic without adjusting geometry to hide it. The exercised real
Globe comparison and six-row create/move/delete fixture are documented in
[vfx/forensics/GLOBE_STATIC_MESH.md](vfx/forensics/GLOBE_STATIC_MESH.md).

### Clearing software editor residue after a deleted effect

If sprites remain after the effect dies or is deleted, retain that capture as a diagnostic. `Ctrl+R` did not clear the tested Sparks residue. In the disposable MCP_AB fixture, use registered `guest_run_command` calls to `editor_command`, first with `command: "map 10001 10000 16"`, then with `command: "map 10000 10000 16"`. Wait for each exact job to finish and check a fresh guest screen. The second command restores the original camera; the verified Sparks case restores the entire floor exactly below the FPS band. Compare against the saved empty ground before the next capture. This clears residue without saving the map or restarting the game; it does not validate the affected animation frames. For another scene, use its recorded coordinates and return exactly.
