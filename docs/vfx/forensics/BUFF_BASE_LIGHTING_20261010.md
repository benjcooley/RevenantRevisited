# Speed, Quicksilver and Fmastery: independent matrices and native-lit bases

The three authored base meshes now have an independent integer-pose comparison
through the original normal-lighting and software raster path. All 30 frames of
each base give **90 nonempty, pixel/depth-identical pairs** and repeat-identical
native warm replays. This is a bounded base proof, not actual map/Metal or full
effect acceptance. No engine lighting, tint, geometry or projector was changed.

`buff_base_lighting_probe.py` compiles the actual production integer decoder,
`SpeedEmitterLocalMatrix`, `SpeedBaseMeshWorldMatrix`, and
`FmasteryBaseMeshWorldMatrix`, with production matrix headers and `math3d.cpp`
under ASan/UBSan. Shipped compressed keys are independently supplied to this
driver; it never receives native TRS or matrices. Native NewObject punctuation
and complete CalcObjectMatrix execute separately. Exact-profile guards,
no-interpolation resource access and identity owner are declared boundaries.

The maximum matrix differences are 2.38419e-7 for Speed/Quicksilver and 1.19209e-7
for Fmastery. Speed/Quicksilver's 60 independently produced emitter matrices are
checked too. The original `56eb30` D3DVERTEX path consumes the authored normals,
XYZ, UVs, local indices and real RGB565 texture. Ambient RGB38, directional RGB1,
and direction `(0,-0.78125,-0.625)` are explicit scene inputs corresponding to
ambient 32, white, Ambient3D 130 and DirLight 85. Base mode 80 for Speed/Quicksilver and
measured incoming mode 16 for Fmastery execute in the original blend setter.
Fmastery's unmeasured cold caller state remains separate.

Actual native base RGBA5 is `(21,21,21,31)` in every tested pose. Original
software Illuminate uses scene lights and transformed normals, ignoring
material diffuse/emissive. Restoring rotation-before-nonuniform-scale in the
compiled Fmastery base produces matrix errors above 0.1 and visible pixel
differences. A separate diagnostic applies the common-world 1.5 Z stretch before
native illumination: these exact base poses retain the same quantized RGB5.
That negative finding provides no justification for a production tint fix.

The Speed and Quicksilver particle probes now use independently compiled
production emitter matrices from this helper instead of sharing native poses.
Both retain 2,221 live samples, 2,109 draws, 112 zero-scale rejections, 4,218 raw/FIX
center checks, and five nonempty pixel/depth-identical pairs. Maximum corner
errors remain 2.996e-6/3.066e-6. The maximum original FIX center residual is now
9.53674e-7 in both types, within the existing 3e-6 bound. Quicksilver's prior
assertion of exact zero depended on shared emitter matrices; its test now uses
the same existing bound as Speed. The independent integer decoder itself still
matches all 810 authored TRS channels exactly. The older full-state comparison
is unchanged and remains a separate shared-emitter state contract.

Production common-world owner conversion, modern projection/Metal,
fractional/cross-state poses, native point-light selection and natural
map/caster/audio remain open. The historical pale Fmastery composite discrepancy
is not closed by these base-only pairs.

The follow-up `speed_composite_probe.py` combines both complete isolated
Speed/Quicksilver components at stationary origin/face 0, ticks 5,15,29. Independently
produced particle packets render first in mode 16; each authored base follows
with actual normal illumination and mode 80. **Six nonempty complete isolated
composite pairs have zero differing RGB565/depth pixels**, with identical native
warm replays. Base addition changes 296–1,608 pixels per image, so a missing base
cannot silently pass. Particle counts are 6,18,27. These samples precede the
separate owner-movement fixture at tick 60; moving-owner composites and actual
map/Metal appearance are not certified. The existing MODELZ particle boundary
is still explicit; no camera or coordinate correction was fitted.

Reproduce the composite and its regression:

```sh
python tools/retail_runtime/speed_composite_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe --output <new-private-directory>
python -m unittest discover -s tools/retail_runtime -p test_speed_composite.py
```

```sh
python tools/retail_runtime/buff_base_lighting_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe --output <new-private-directory>
python -m unittest discover -s tools/retail_runtime -p test_buff_base_lighting.py
python -m unittest discover -s tools/retail_runtime -p test_speed_packets.py
python -m unittest discover -s tools/retail_runtime -p test_quicksilver.py
```

The four matrix/particle tests and the composite regression pass. The fresh Metal Release game build also passed after the
FireCone correction; this follow-up changes probes/tests/documentation only.
Artifacts are under
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/`:

- `buff-base-lit-final/manifest.json`: `c97c79c53328ad2bac66e5441ebc397dcfef772e9f853a96182b718fc2b24d30`.
- `speed-independent-emitter/manifest.json`: `71a7f7d80601d2fb9d983fb1582e6790d190e15e9b2d602904eea16338ec9309`.
- `quicksilver-independent-emitter/manifest.json`: `dba0841c3c9489d7c1469b0c66a4a8b7e22d674270b2eedc607f0c767821d66f`.
- `speed-quicksilver-composites/manifest.json`: `f77b2afc30931c6b60f904788925d5543259c99973b31d29f620c393011d4d47`.

## Actual Metal closure review: 2026-10-10

The new exact-owner `TSpeed_AUTHORED_TAGS` and `TQuicksilver_AUTHORED_TAGS`
previews instantiate the shipped generic effects, retain their real default
animators, and use the same production particle submission, authored base mesh,
base world matrix, mode80 and controller-before-base order as the map route.
The existing Fmastery preview is included with explicit incoming mode16. These
are capture adapters, not replacement simulation or renderer corrections.

`buff_visual_reference.py` executes the native parser, integer authored poses,
Pulse, RenderSample, prototype matrix, original normal illumination and software
raster for all three complete effects. The fixed owner is origin0/face0 and the
viewport is640x340 centered320,170. Native NextFrame precedes Pulse; render1 is
warmup, so recorded capture1 uses animation frame2. Captures1/5/15/29/45/59 are
compared to fresh actual Metal frames with the same cadence. No camera fitting,
image registration, replacement geometry or host-provided base colors occurs.
The reference's authored sample-to-device handoff remains the declared boundary
from the earlier packet proofs; this is not execution of an entire retail map.

Final private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/`.

- `native-visual-pinned/`:18 complete native images; all18 exactly repeat
  `native-visual-final/`. Quality0, native seed1, no point lights, RGB565 assets.
- `isolated-metal-final/`:60 actual Metal frames per effect, clean exits,
  final binary SHA256
  `40a08056eab423f56a12ef051ca99fb2ddb16ce3eca8af823930e30783b57fb5`.
  Each run has an independent SAVE directory and retained scenario/command/log.
- `buff-visual-final/manifest.json`:18 fixed-frame comparisons and3 map
  lifecycle checks, SHA256
  `dd470bdbbc6dab87d7fab387458674daa70c279bfd255e0e70cd582e51ad9f02`.
  `native-metal.png` uses identical fixed crops and nearest resizing; raw images
  remain untouched. The tool deliberately reports `review_ready`, not accepted.
- `actual-map/`:58 fresh FontRuntimeLab frames per effect using the earlier
  gameplay-identical binary76742d62906dc434da76cc3a4f02c5d719f4f4eecb6398fe96171aca2e77a621.
  All6 ADDAT/expect/MOVE/expect/DELETE/absent rows pass;41 distinct active frames
  each, visible displacement, and13 exact original-floor tail frames each.
  Camera110,10000,10000,16; ambient32white; quality0; explicit Fmastery incoming16.

Actual `[vfx-source-light]` logs prove source32white, Ambient3D100,
UseDirLight=true, DirLightPercent85, ambient(0.149019614)x3 (=38/255),
directional(1)x3 and pointlights0. Saturation makes Ambient3D100 and130 identical
for this input. These exact values feed `SetRetailMeshLighting` and the helper
vertex uniforms; the native branch calls its ambient38 and directional1 APIs.
This is declared source lighting, not a fit to vertex colors.

`[vfx-rng-scope]` confirms0 global random draws before first render. At the six
comparison points the actual pre-render draw counts match the independently
executed native seed1 stream: Speed/Quicksilver4,24,72,144,224,292;
Fmastery6,36,108,216,336,438. The report retains both counts. This verifies stream
consumption/cadence, not every actual GPU particle packet. No RNG correction or
seed fitting was introduced.

The green star, blue star/halo and orange/red flame have matching overall
families and authored forms. Metal is visibly brighter/smoother, with different
nonblack extents. For capture15: Speed native58x26 versus Metal61x30;
Quicksilver65x39 versus78x40; Fmastery50x31 versus52x35. Known common projection,
software RGB565 modulation/additive quantization and GPU sampling remain
separate residuals; these measurements do not establish which one explains each
pixel. In particular the Quicksilver horizontal extent cannot be attributed to
the roughly1% common horizontal projection difference alone. Overall appearance
is for explicit human review; strict pixels, natural caster/caller, map light
selection, audio and device edge cases remain open.

Validation: full Metal build; all3 isolated and all3 map runs complete;18 native
images repeat exactly;11 focused composite/capture/ARGB fixture tests pass.
Reproduce native references with `buff_visual_reference.py`, actual map scenarios
with `buff_map_capture.py`, isolated captures with `port_capture.py` and retained
scenario JSON, then validate/panel with `buff_visual_review.py`.

### Corrected native camera startup replays

After a7291cb binds the original camera additive tables through54df70, all
previous case records and image hashes remain unchanged. The current replay
locations, rather than earlier startup manifests, are:

- `firecone-owner-camera-replay/manifest.json`:44cases,30 nonempty pairs,
  SHA256 `dca0939bff25ce520e7735fda75192cfac48b52ced13bd1268b1959fe5c4c0d3`.
- `buff-base-camera-replay/manifest.json`:90cases, SHA256
  `c97c79c53328ad2bac66e5441ebc397dcfef772e9f853a96182b718fc2b24d30`.
- `composite-camera-replay/manifest.json`:6cases, SHA256
  `f77b2afc30931c6b60f904788925d5543259c99973b31d29f620c393011d4d47`.

The latter two manifest hashes themselves are unchanged; all three were freshly
executed with the corrected fixture. Their identical output does not waive the
camera binding requirement for other raster modes.
