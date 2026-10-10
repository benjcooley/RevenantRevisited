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
