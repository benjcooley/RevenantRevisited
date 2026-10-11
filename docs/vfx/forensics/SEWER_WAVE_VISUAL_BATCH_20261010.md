# SewerWater and three waves, 2026-10-10

SewerWater (`0xadbcef19`), Wave (`0x9d92bc1d`), WaveS (`0x9d92bc1e`)
and WaveM (`0x9d92bc1f`) have complete original-render/actual Metal pairs and
fresh real map lifecycle captures. The existing generic map path reproduces
their authored static geometry and scrolltex behavior. This batch adds exact
resource guards and actual typed-owner preview registrations; it changes no
shader, renderer or general 3dimage implementation.

## Exact authored inputs

All four are v3, one-root, one-state, one-frame looping assets, with one material
and one single-frame 128-square ARGB4444 texture. SewerWater's `box02` has four
vertices/two triangles, nine constant key words and no controller tags. Each
wave has 38 vertices/47 triangles and six constant key words. Their roots are
`wave`, `wave02` and `wave01`, respectively. The literal scrolltex tags use
`dv=-0.005`, `du=0`; WaveS's unmatched `obj=wave` selects all roots through the
executed original fallback. None requires a bespoke effect/controller.

`other_water_profiles.json` pins the individual whole licensed asset hashes.
`scroll_water_profile::Matches` checks type/class/state/path identity, exact
frame/header/tag counts, root/parent/material, every vertex/index/key word,
17 material floats and the texture descriptor. Texture texels are fixture-pinned
by the whole asset SHA, not dynamically hashed by this engine guard. The
preview preserves normal NextFrame/Pulse/Animate and uses the existing actual
matrix, global legacy clock, ARGB4444 helper and native-domain projection.

Names: `TSewerWater_AUTHORED_ROOT`, `TWave_AUTHORED_SCROLL`,
`TWaveS_AUTHORED_SCROLL`, `TWaveM_AUTHORED_SCROLL`. Real map creation uses the
existing exact typed EFFECT/default animator rather than these gallery names.

## Frozen evidence

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-next-sewer-wave-20261010/`.
No binary assets, executable binaries or generated captures are committed.

- `native-visual/manifest.json`: 24 whole original software frames, each repeated
  with identical RGB565 and depth. Actual factory/full animator constructors,
  original scrolltex registration `401610`, whole RefreshControllers/parser/
  Initialize/CopyVertices, original owner/key/matrix and complete RenderObject
  `40a8f0` execute through FVF_VERTEX/ARGB4444 software raster. SewerWater has
  zero original controllers; each wave has one original `5a3480` scrolltex
  controller. Immutable decoded resources, COM material handles and single-frame
  texture residency are explicit adapter boundaries. No host geometry, UV or
  material color is inserted into the original draw.
- `metal-visual/{name}/manifest.json`: four actual private-binary 60-frame runs,
  24 Hz, warmup1/seed1, origin/facing0, black 640x340 viewport, native-domain
  projection and ambient32/white source light. SewerWater has one distinct frame;
  each wave has60. All processes exit cleanly using private saves and the real
  fixture Revenant.ini.
- `review-frozen/manifest.json`: 24 whole original/actual Metal pairs at captures
  1/5/15/29/45/59. The source global clock is capture+1; owner frame remains0.
  Logged source offsets match executed original controller rates, and every
  original copied vertex changes by those offsets. Whole panels have no crops,
  image registration or geometry/color/phase fitting. Nonzero pixel differences
  remain measured.
- `map-final/{name}/manifest.json`: ten actual create/frame/MOVE/position/DELETE/
  absence observations and sixteen captures pass per type. SewerWater remains
  static; waves have four distinct stationary texture frames. All are visibly
  created, move and delete, and restore the exact underlying floor. The long
  wave strip is authored geometry and extends beyond this small test viewport.
- `profile-contract/manifest.json`: actual guard compiled under ASan/UBSan;
  sixteen literal/path positives and1,642 independent rejection cases pass,
  including all nine SewerWater keys and every wave vertex/index/material/tag.
- `tests-final.log`: four old/new exact-guard and native/compiled scroll-selector
  regression tests pass. Full private build and `git diff --check` pass.

Capture binary SHA256:
`e2a6b21eba79dd1a5db51ccc946faafda53144366e857905b265203b290b2809`.

Original software/GPU edge and color quantization, exact framebuffer parity,
natural scene placement/caller/device/depth interactions remain open. These
fixed nullspell/native-domain and synthetic-floor runs do not close those gates.
Central ledger, coverage and review mode files are owned by root and unchanged
by this batch.

## Whole-wave framing follow-up

Root independently approved SewerWater's plane form, texture and constant color
over all six phases. Its individual `review-frozen/SewerWater-root-approval.json`
pins the unchanged initial samples, binary and source. Root found the initial
640x340 wave tails clipped; those initial wave panels do not establish whole
effect coverage or primary visual closure.

`wave-whole/native/manifest.json` supplies eighteen fresh whole original frames
at 1280x960, each with an identical warm RGB565/depth replay. The exact same
capture binary supplies `wave-whole/metal/{name}/manifest.json`, sixty frames
per wave. Origin0, authored scale, owner matrix, global clock and source light
are unchanged. The viewport is enlarged with its normal center at640,480;
both implementations use the same explicit camera/framing, with no alignment
fitting or geometry scale change.

The native probe observes every original transformed vertex after complete
RenderObject, including transparent vertices. Wave's whole projected bounds
are536.54,234.33..1131.34,531.73; WaveS's are518.97,211.12..1179.63,538.79;
WaveM's are510.83,180.32..1238.81,544.31. These all fit strictly inside1280x960
at all six phases. `wave-whole/review/manifest.json` passes `--require-unclipped`
for the executed full geometry and both nonzero-image bounds, plus the same
clock, native controller-rate and every-vertex UV checks. The full six-phase
panels retain all pixels and existing software/GPU differences. Earlier real
map proofs remain pinned to the same unchanged engine binary. Python compile
checks and `git diff --check` pass for the probe-only follow-up.

Whole-wave independent review remains separate from the automated comparison
checks; exact pixels and natural scene/device/depth acceptance stay open.
