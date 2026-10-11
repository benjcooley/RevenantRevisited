# Fizzle owner, texture and live RNG correction

`Fizzle:0xab8800dd` now composes each relative particle corner with its
owner matrix in `TFizzleEffect::TickAndSubmitForTest`. Previously the isolated
preview only added `Pos()`, bypassing the common-world owner's geometry
conversion. Original `0x4f4440` calls particle `Render(1,0)` at `0x50c220`;
particle translations retain raw local Z and `RenderObject` composes the owner.
No particle FIX_Z, fitted offset, or global camera change was introduced.

The explicit `--vfx-native-domain` isolated preview uses the existing executed
owner/projector formulas with declared camera Z=0. Ordinary runtime still uses
its existing common-world owner and camera. As a nonzero-owner control, executing
the original owner with position `(11,-7,16)` produces translation
`(11,-7,11.379498481750488)` and identity 3x3; a particle at `(-14,13,110)`
composes to `(-3,6,121.37950134277344)`. This is the original nonlinear owner
conversion, not a correction fitted to the images.

Both the canonical preview and actual `TFizzleRuntimeComponent` now request the
existing ARGB4444 source policy. The original ARGB4444 kernel ignores vertex
color; the previous generic quad route multiplied authored material diffuse.
The runtime component already composed its owner correctly. Its independent
`snap_random` helper, however, called host `std::rand`; it now calls the shared
original MSVC `random(lo,hi)` implementation, retaining equal-endpoint behavior.

Validation:

- `fizzle_probe.py`: forty ticks, two warm replays, exact original state/RNG,
  authored corners/UVs and software pixels versus compiled canonical preview.
  The owner is an explicit identity boundary in this frontend test.
- `fizzle_runtime_probe.py`: compiles actual map constants, structures,
  `snap_random`, `SpawnParticle` and `Animate` unchanged; forty ticks,
  2,940 used-particle float fields, 425 original RNG calls, effect lifetime,
  phases and flicker pass. Original RNG outputs are common inputs, with every
  call's range/order checked. The live generator is the existing shared engine
  MSVC implementation, not a host-C replacement in the effect.
- `fizzle_visual_reference.py`: forty complete leaf lifecycle frames through
  original owner, relative world compose, software projection and ARGB4444
  raster; two identical warm renders per frame. Imagery lookup and full
  `RenderObject` resource dispatch remain explicit boundaries.
- `fizzle_visual_review.py`: forty actual Metal frames per common/native-domain
  route, seed1, 24Hz, one warmup, 640x480 black, origin0, camera320/240,
  ambient32 white (logged source ambient38/directional1). Five live RNG
  checkpoints per route match original: 0/10/81/330/425 before ticks1/2/6/16/30.
  All three routes have 25 visible frames and a blank exhausted tail.

The native-domain panel closes the large projection discrepancy in this
isolated configuration. Common-world captures retain the shared projector
residual; raster and framebuffer precision are not pixel-identical. Natural
spell creation, original full-map rendering, device behavior and audio are
separate gates. The map movie is a runtime smoke check, not a retail map A/B.

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/fizzle-visual`.
Source tools produce manifests and images there; licensed assets, binaries and
rendered artifacts are not committed.

Frozen final evidence (2026-10-10):

- `state-current/manifest.json`: SHA256 `1ef0809edfa08837ced2f7587cd0f49231462ea2cb014e3bb9c35459d0e3cdc9`.
- `runtime-state/manifest.json`: SHA256 `172dbadf7d77f2d00e4efd2bcdeca57cf1776be0d856db1db9a0039b8eb8f8b1`.
- `native-current/manifest.json`: SHA256 `fee183d87733892fbc41a1329a47e76de4c88aae250e97860bacd22dfb0460f7`.
- `review-final/manifest.json`: SHA256 `58f855d8ba59019014b95620336ad245e83cbe1e65a53b6abb8ae2c38ba655c8`.
- `map-final/manifest.json`: SHA256 `4130fe782ca7e70efd1b66bf208bd4cf62fa2c7c3f4ecc374dd82440b35ad83a`.
- Final Metal binary: SHA256 `b7aa6841d55a1b0697152e6e5659ceba6858b65209526dfd23a0a59503a0042c`.
- All 120 final native/common/native-domain image hashes equal the previously
  independently reviewed panel inputs. Final map run exits0; 33/40 nonempty,
  34 distinct images, actual component attachment, simulation and reaping logged.
