# Natural Ogrok arena and Nakranoth chamber references

The in-process emulator now loads the original saved map objects through the
original module, play-screen and map initialization. These scenes are diagnostic
references, not full appearance approvals: their composited map backgrounds
currently have corrupted colors. DirectDraw copy tracing showed matching source
and destination masks; a cross-format raw DD copy is **not** the demonstrated
cause. Do not repair screenshots or change effect geometry to fit that defect.

## Locations

The Ogrok arena is level0 around Baez `(12482,16986,32)`. Camera/player input
`0 12482 17440 32` loads the surrounding saved sectors. Its spectators are real
CHARACTER objects, including PaleOgrokclub, Ogrokhammer, PaleOgrokmace,
DarkOgrokhammer and Ogrokmace. Their saved watch states have24–77frames, rather
than the four-frame EFFECT helper clips named OgrokWatcher1/2/3. The earlier
isolated helper test did not establish natural arena cadence. Reviewer rate0.25
remains a user-approved presentation setting, not a measured retail rate.

Nakranoth's chamber is level49. The saved NAKRA object is at `(9713,9649,272)`;
camera/player input `49 10515 10411 72` shows two saved base Flame objects above
`Dungeon/DunTorch4.I2D` sconces:

- Index1142598354, `(10474,10410,100)`.
- Index1145189969, `(10557,10413,100)`.

Both are exact `Flame:0x50ba373b`, `Magic/Flame.I3D`, actual owner vtable5a85ac
and animator5a9dec. Original screen ticks3/4 show internal frames2/3 for both,
using the registered original41ms WinMM callback. This is a short native scene
progression check, not complete wall-clock cadence acceptance.

## Reproduce the scene

Run from the VFX validation checkout. The local Python environment is
`/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python`.
Set `PYTHONPATH=tools/retail_runtime:tools/retail_asm`. The licensed retail
executable, assets, checkpoints and captured images stay outside source commits.

```sh
python tools/retail_runtime/mapped_scene_preflight.py \
  --camera 49 10515 10411 72 --move-player \
  --skip-bitmap-precache --accelerate-stores \
  --output /tmp/retail-nakra-scene

python tools/retail_runtime/mapped_scene_frame.py \
  --checkpoint /tmp/retail-nakra-scene/resident-map.image \
  --ticks 3 --draw --output /tmp/retail-nakra-frames
```

Output folders must be new. `--skip-bitmap-precache` omits optional all-state
bitmap cache warming; original cache misses still decode on demand. This is an
explicit qualified boundary until unchanged warm/cold pixels are compared.
`--accelerate-stores` uses seven opcode-pinned x86 loops, checked against the
actual instructions for memory, registers, flags and checkpoint restoration.
Changed opcodes reject the entire hook set before installation. No effect,
geometry, decoder or pixel function is replaced.

Use `--draw`: the observed `TimerTick(draw=false)` path deadlocks on its background
worker event even with a resident scene. Failure manifests/checkpoints are
retained, and CLI failures exit nonzero. First full draw may fill scratch layers
while the display is black; later full draws composite the scene. To continue,
pass the previous `after-frame.image` to a new output folder. Do not restart an
instruction-budget gate: preflight `--resume` continues its saved native stack.

Local natural-scene evidence lives under
`/Users/benjamincooley/RevenantRetailLab/research/vfx-ogrok-arena-root-20261011/`
and `vfx-nakra-flame-scene-root-20261011/`. Each manifest pins its input checkpoint,
source and actual original actor/frame records.

## Base Flame fix and full-loop control

The standalone base Flame preview had a null CLASS-record dereference introduced
by an unconditional `ObjId()` query. Attachment now guards imagery-only owners.
The shared authored-quad component derives ARGB4444 handling from the loaded
texture and applies explicit native-domain projection to base Flame too.
No atlas, rotation, size, scroll speed, brightness or animation rate was fitted.

`flame_loaded_reference.py` creates the original loaded CLASS owner, initializes
its original custom animator and executes every owner/render phase. Crucially,
4e4ef0 is animator **Pulse**, already called by ownerPulse4de800. Do not call it
again as Animate; default Animate is40e460. The regression checks one custom
Pulse per step and none during rendering. Two fresh VMs produce identical37frame
images/packets, two18tick cycles and eight atlas images.

```sh
python tools/retail_runtime/flame_loaded_reference.py \
  --boot-image /path/to/current/hud-boot.image \
  --output /tmp/retail-loaded-flame
python -m unittest tools/retail_runtime/test_flame_loaded.py
```

Actual Metal captures have36/36 visibleframes, eight distinct images and all37
logged UV phases equal the original. Eight fixed-crop pairs were inspected in
explicit native-domain mode: flame form/card angle and atlas cells agree.
Original low-alpha brown card, edge and quantization differences remain. Normal
map factory/reviewer integration has40/40 visibleframes and eight distinct
images. Blue/green Flame RGB captures remain byte-identical to their previously
approved captures. These are bounded appearance/lifecycle checks; ordinary
projection, natural scene lighting/depth and exact pixels remain open.

Fresh evidence is at
`/Users/benjamincooley/RevenantRetailLab/research/vfx-base-flame-loaded-root-20261011/`,
including `review.json`, `loaded-cycle{,-repeat}`, `metal-after-component`,
`metal-after-common`, `map-base` and unchanged colored variant regressions.
Retained earlier failing captures are diagnostics and grant no pass credit.
