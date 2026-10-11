# Labback authored spheres and sparks, 2026-10-10

`Labback:0xdcc4011d` (`Misc\Labback.I3D`, SHA256
`1291c57862c12eec8e691585228905a0c1a0257a823704e203ac02df8b914dbc`) now has
an exact standalone preview, `TLabback_AUTHORED_TAGS`, and an explicit gallery
mapping. The actual default animator is selected by the complete original
93-builder registry. No actor, guessed sprite or shared-asset alias is used.

The original asset has a looping 400-frame state, three independently rotating
49-vertex/80-triangle sphere layers, an invisible `fgeys` emitter, and a separate
`#sparks` quad prototype. `blendcont=litadd` and the authored PartSys tag take
effect during initialization; tag frames 40/97 do not delay those controllers.
The port uses its own inclusive decoded keys, real owner NextFrame/Pulse, the
production PartSys simulator and two exact RGB565 textures (64 and 256 square).

A causal culling correction was required in this new adapter. Original mode16
sets D3DCULL_NONE (`state22=1`). The prior single-sided helper policy discarded
all three sphere layers. Using the existing two-sided helper route restores
them without geometry, brightness or timing changes. Original full-render
negative controls at frame0 produce 57,301 nonblack pixels for unchanged mode1
and zero for forced cull values 2/3. No renderer changes are included.

Validation:

- All 2,000 object/frame poses: 18,000 key fields and 32,000 matrix fields match
  original409430/40a420. Existing default key traversal remains unchanged.
- Whole original Animate40e2e0/Render40e8d0 runs 402 ticks including 399→0.
  204,685 live particle fields match; every RNG seed is exact. The maximum
  accumulated float residual is 0.000304895 at particle Z=-341, bounded at 0.0005;
  this is recorded precision drift, not a fitted tolerance for appearance.
- Every render caller and particle sample executes; only selected review
  frames execute the expensive software raster. Other frames end at the
  explicit device boundary56eb30. Render consumes zero RNG in all 402 frames.
- Actual isolated common-world/native-domain Metal movies each have 402
  nonblack distinct frames and clean exit. Native-domain draw counts match
  all logged runtime checkpoints. Nine selected original/current phases are
  frozen, including the loop boundary.
- The source state/pose probes use ASan/UBSan. Compiled default-zero PartSys
  traces remain identical to the previous implementation. Full build passes.

Configuration: 640x480 black, owner/camera 0, source ambient 38/directional 1,
quality 0, MSVC seed 1, flat ground 0, 24Hz. Immutable decoded resources and texture
COM residency are explicit reference boundaries. Full original resource
loading/startup, natural map/story placement, audio, surrounding geometry and
broader lighting remain separate. Native-domain projection is opt-in; ordinary
projection residual is shown in the common-world column. Exact raster pixels
are not claimed.

Evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra`.
Actual Metal binary:
`f70ea661ceecb13a79400c665b8fec8caaf8ff467354f1cf9d5b3f2f5c7ad0e4`.
Manifest pins:

- `labback-native-sparse`: `8c6d4451b8a49d6c0acb53ad0b9daf31b83cf618b31d5a0c7dd161f354e31139`
- `labback-state-final`: `09d5960165173ea9d72e08b2333a36a3f023865c61679d859b05509b26197851`
- `labback-poses-final`: `4c8119baf8470ac538314a0f0906782d0adaf9dccf4da7bdc1996ecc417a939b`
- `labback-cull-native`: `5dc94c65bd9856ab79990a3bbeaed472a6a323290aa3a04902d981375f9d9975`
- `labback-metal-final`: `b935b3a11f6a0dbb1269a8a08d6a79dda70c3b244eca2438cac1063ae2b382a2`
- `labback-metal-common`: `cb644e6069f21f9c1dfd8ea2f84235706bf1263abac26cd7558cac7adadc05f3`
- `labback-review-final`: `c8875523bc7ffeee8ad2ad4174a993dba2f01e21e34286cd390968b6563b80d6`

Scoped appearance decision is recorded separately by root; source progress
alone does not establish full-effect/natural-caller acceptance.
