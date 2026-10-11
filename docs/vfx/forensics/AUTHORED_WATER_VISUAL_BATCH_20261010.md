# Four authored six-emitter water effects, 2026-10-10

WaterFlft (`0xd0c0f036`), WaterFrt (`0xd0c0f037`), WaterClft
(`0xd0c0f038`) and WaterCrt (`0xd0c0f039`) now use exact profiles in the
existing audited static particle adapter. This restores their original `#`
camera-facing prototype matrix, cached emitter-frame sampling, five-bit prelit
colors and RGB565 additive metadata. The previous raw ID-range admission is
removed: altered water resources fail closed. Nine existing profiles and the
Speed/Quicksilver families retain their behavior. No renderer/shader change.

Each exact asset has seven independent roots, six literal named emitters, one
four-vertex/two-face `#particle01`, two materials, one 64-square RGB565 texture,
one15-frame looping state and one frame5 PartSys tag. All root names, material
indices, constant key words, material channels, topology, prototype vertices,
texture descriptor and the complete literal tag are fingerprinted. The fixture
pins all texture bytes through individual whole asset SHAs. The fall variants
have45PPS/gravity.5; caps25PPS/gravity0, with their own authored curves.

The four `TWater{Flft,Frt,Clft,Crt}_AUTHORED_TAGS` previews create actual typed
EFFECT/default animator owners and use normal NextFrame/Pulse/Animate, the
same production SubmitPartSys and the existing native-domain projector. No
native particle poses, fitted sizes/colors, placeholder emitters or mesh
substitution feed the production preview. `HasRetailAuthoredWaterProfile(id)`
exposes exact admission for review integration; real map spawning uses the
existing typed factories.

Root independently reviewed the full fall panels and all four six-phase fixed
center diagnostics. Blue/green shape, left/right progression and birth-to-steady
behavior pass for the recorded native-domain configuration. Full frames and
every original transformed quad remain inside1280x960. The supplementary
diagnostics use one declared512x384 viewport rectangle at384,288, without
rescaling, phase alignment or pixel changes. Exact pixels and natural
setting/caller/device/depth acceptance remain open.

## Frozen proof

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-next-authored-water-20261010/`.
No assets, executable binaries or generated captures are committed.

- `native-final/manifest.json`: 24 visible full software frames, six per type at
  captures5/10/20/35/65/95. Two independent fresh native runtimes agree on RNG,
  draw counts and complete image/depth. Full default factory/registrations,
  literal parser/Initialize, six emitter matrices, cached-frame Pulse,
  RenderSample, hide/restore and complete RenderObject/FVF_LVERTEX/raster
  execute. Every drawn transformed vertex, including transparent regions, is
  observed after original dispatch. Decoded immutable resources, owner animator
  lookup, material COM, single-frame exact RGB565 residency, seed1 and ground0
  are declared boundaries. Nonsampled ticks skip only the final raster.
- `metal-final/{name}/manifest.json`: four96-frame actual binary captures,
  24Hz/seed1/warmup1, origin0, fixed1280x960/native domain, center640,480 and
  ambient32/white. All exit cleanly with private fixture saves.
- `review-final/manifest.json`: six unchanged whole-frame pairs per type;
  source before-render RNG matches the corresponding native cached cadence.
  Whole geometry and both image bounds are unclipped. Nonzero software/GPU
  pixel differences are measured. Individual `{name}-root-approval.json`
  records pin all six pairs, diagnostics, binary/source and map manifests.
- `map-frozen/{name}/manifest.json`:48 frames and ten create/owner-frame/MOVE/
  position/DELETE/absence observations pass per type. Actual particles become
  visible after their authored zero-scale birth and evolve, then deletion
  restores the exact synthetic floor. Map ground16 differs from the isolated
  native fixture ground0 and is not claimed as a matched natural scene.
- `profile-frozen/manifest.json`: ASan/UBSan actual guard execution passes
  sixteen literal/path positives and934 independent rejection cases. Actual
  four-type classifier passes169 profile/type pairs and26 unknown/unselected
  negatives. `nine-profiles-regression/manifest.json` preserves all nine prior
  profiles:36 positives/4,606 rejections.
- `regressions-final.log`: five existing strict/native/static cached-pose/Speed/
  Quicksilver tests pass. `water-tests-final.log`: new sanitized water admission
  test passes. Full private build, Python compilation and diff checks pass.

Capture binary SHA256:
`2e9ce06509e2427f855ffcef9143029ee3bf0b7c6c8596ff66573f1b42c6d15a`.
`source-freeze.json` pins the source and canonical manifests.

## Retained numeric residuals and superseded attempts

`packet-frozen/{name}/manifest.json` retains the unchanged90-tick source/native
diagnostic, not an all-pass claim. Full state errors and all six shared-raster
image differences are0 for each type. WaterClft passes the tight packet test.
WaterFlft and WaterFrt retain27/45 raw-center/FIX failures: maximum raw center
about0.0001297 and MODELZ about0.0000923. WaterCrt retains15 corner failures,
maximum0.0000859, under the inherited tighter frontend threshold. No tolerance
was changed to turn these diagnostics into passes. These small float residuals
remain separate from root's current appearance approval and exact-pixel gates.

The initial `native-feasibility` attempt stopped on a mistyped WaterFrt SHA,
after WaterFlft; it is incomplete and is not a reference. The `map-final`16-frame
static-texture checker passed its command rows but failed premature image
assertions before newborn scale became visible. `map-frozen` supplies the
appropriate48-frame lifecycle. An initial `packet-final` attempt requested
capture95 from a90-tick probe; it failed before a manifest. The corrected
`packet-frozen` samples stay inside that probe's own90-tick domain. None of
these historical attempts is silently rewritten or granted credit.

Central ledger/index/coverage and review-mode files were not edited.
