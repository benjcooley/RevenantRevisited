# LabGateBarrierS/E: authored untextured frontend proof

2026-10-10 correction: the red diffuse-fed fixture below does not establish the
actual untextured software appearance. Original D3DVERTEX/Illuminate ignores
material diffuse and produces normal-lit gray cards. The exact S/E map route
now follows that producer in source-software lighting mode; modern/positional
fallback retains prior material behavior. See the
[current visual batch](SMALL_FAMILY_VISUAL_BATCH_20261010.md). The earlier red
map captures are historical material-policy diagnostics, not native visual credit.

Both exact shipped barrier roots pass original-code versus compiled-production
geometry and untextured software image/depth comparisons. A separate actual-map
test exposed white material fallback; the map fix and runtime evidence are
recorded separately below. Full lighting and natural caller visibility remain
open; neither barrier is fully accepted.

## Asset, default dispatch and stationary contract

- EFFECT `LabGateBarrierS`, type `0xad92bc37`, `Misc/IBarrier1.I3D`, SHA256
  `aee3ba4debae7894ae093a8047e03491b7e2b2eb22275a85ad0048fca354fb91`.
- EFFECT `LabGateBarrierE`, type `0xad92bc38`, `Misc/IBarrier2.I3D`, SHA256
  `782a2fc0a2f0ffe3fe0811df78dcd39752603ced7df6213ec5bdceb1b8439718`.

Each asset has one looping STILL frame, flags `0xdc`, version 3, four vertices,
two triangles, one material, **zero textures**, one independent root named
`barrier`, no tags and six constant scalar position/rotation keys. Both faces
occupy the actual untextured bin. No billboard, texture, mesh subset or invented
dimensions replace the authored geometry. These are stationary roots, not
projectiles or point-to-point effects. The translated owner test exercises root
placement; it does not invent an animation trajectory.

The fixture executes all 92 pinned retail animator registration constructors
`0x40db90`, then actual builder lookup `0x40dca0`. Both exact names and EFFECT
return default builder `0x5e8508`; FLAME returns a different builder as a positive
control. A preliminary TeleportDoorInsideB candidate was rejected because its
actual lookup returned custom builder `0x66cfb0`, despite a simple asset.

## Material and actual untextured raster

Material 0 is an unchanged 80-byte descriptor: diffuse `(1,0,0,1)`, ambient
`(1,0.101960793,0.101960793,1)`, specular `(0.899999976,0.899999976,0.899999976,1)`,
emissive `(0,0,0,1)`, power 0, texture handle `0xffffffff`, ramp size 16.
The probe compiles the actual `D3D::LoadMaterial` body and verifies its diffuse
and emissive outputs against these authored bytes.

The declared raster input is the authored red diffuse mapped to five-bit vertex
RGBA. It is an **upstream material/illumination boundary**: original full
material-to-vertex lighting has not been executed. This is not a claim that
actual world lighting always produces the fixture's red brightness. The native
software dispatcher `0x56d960` follows its no-texture branch at `0x56d992`; the
texture branch at `0x56dbce` is never entered. Twenty draw calls include exact
pairs, replays and controls. No texture is bound or generated, no surface COM
method is called and no raster code is intercepted.

A red-to-blue diffuse mutation changes **every visible pixel** while preserving
the complete depth buffer. Thus the material is consequential rather than an
implicit white texture. Original RGB565 packing/Gouraud interpolation/depth
writes run through the executable.

## Bounded comparisons and replay

The candidate compiles the actual key decoder, key getter, object matrix,
mesh extraction, static pose, `TAuthoredStaticMeshEffect`'s static preview
`SubmitMesh` body and matrix helpers. It does not compile `maprenderer.cpp`'s
opaque submission branch; the actual-map proof below covers that path separately. The
opt-in `untextured=True` fixture setting executes the real zero-texture
`ExtractSubMeshTextureSlot` fallback to `ExtractSubMesh`; existing textured
fixtures retain their original behavior. Complete local indices and all four
vertices are compared.

Four cases—two exact types, identity and translated owners `(16,-8,4)`—each
repeat twice from the same warm snapshot. Camera `(0,0,0)`, z-distance 1925,
512×512 viewport, no culling and depth test/write are fixed fixture inputs.
Positions stay within 5e-5, UVs are exact, all color/depth pairs and warm hashes
are equal. Nonempty original pixel counts are S: 1,957/1,943 and E: 1,957/1,944.
The owner translation changes the image. Final median warm pair time was about 35ms.
Shared original raster isolates frontend differences; it does not independently
certify the Metal backend, original graphics device or natural map context.

## Separate actual-map material correction

Root's initial real-map lifecycle test passed typed create/frame/move/delete
checks, but both cards were white: the opaque map branch initialized tint RGB
to 1 and did not load the zero-texture material. The root-owned correction reads
authored material diffuse for zero-texture EFFECT meshes, preserving draw-alpha
multiplication. Textured and helper paths retain their prior behavior.

The corrected actual-map runs each pass ten typed observations and 16 images,
static frame 1, movement, deletion and exact floor restoration. All 1,875 changed
pixels per barrier are red-dominant. These are controlled map runtime/material
smokes, not independent original-map lighting/caller or device parity. Their
evidence is under
`recon/retail_asm/runtime/effects/lab-barrier-material-fixed-20261009/`.

## Reproduce and retained evidence

```sh
python tools/retail_runtime/barrier_static_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output <private-directory>
python tools/retail_runtime/test_barrier_static.py
```

The focused regression reproduces both exact authored meshes, compiled untextured
extraction/material decoding and native raster, verifies all nonempty pairs,
replays, material controls and explicit scope exclusions. Baseline EXE, licensed
imagery archive, Unicorn and clang++ are required.

Local native evidence is retained at
`research/vfx-next-20261009/barrier-static-final/manifest.json` in the retail lab.
Manifest SHA256:
`af837f2f86e5588263197e87fa17b5c4fa91fc5161286c96f7bc6dc7dc00169d`.
It retains executable, asset, compiled source/binary/material and image/depth
hashes, original registry lookup, raster branch counts and explicit boundaries.
