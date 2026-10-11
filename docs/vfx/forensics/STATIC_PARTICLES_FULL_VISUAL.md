# Static PartSys: complete native caller and four visual pairs

2026-10-10. Exact types: YEnergy `aeaeeb30`, YEnergyLose `aeaeeb33`,
YAbsorb `aeaeeb29`, nullifier `ad92bd38`. The generic isolated preview also
admits the already validated Regeneration `10ac03de`, SwiftStrike `e0a3bc43`,
fspray `0c052638` profiles. It constructs ordinary EFFECT owners and runs their
actual animator/controller; it supplies no independent particle simulation.

## Causal correction

Original Pulse `403771` reads the animator's cached frame. Original
GetObjectPos `40efc0` copies the cached object matrix at `+0x58`; it does not
resample keys. Animate refreshes those after Pulse. Production FillInputs
already selected the cached frame for the parameter curves, but its
GetObjectMatrix call refreshed emitter poses using the advanced owner frame.
Nullifier's animated emitters consequently emitted from the next pose.

Only exact admitted state 0 static PartSys profiles now sample their verified
root emitter keys at `animator.GetFrame()` and construct the original ordinary
local matrix. There is no owner-frame subtraction. Multiple authoritative
Pulses reuse that cached pose until Animate changes the cached frame. Other
profiles and states retain their existing behavior. Strict profile admission
pins the root hierarchy and all key words. No renderer, tint, geometry, seed,
or time-fit change accompanies this fix.

The retained regression compiles actual FillInputs and MakeMatrix with
production math under ASan/UBSan. It checks three Pulses with advancing owner
frames against one cached animator frame, the subsequent cache refresh, and
unrelated ID/state controls. A separate original-executable test executes
18 Pulses with a full controller Render only every third Pulse and observes
unchanged cached emitter matrices at every `40efc0` read.

## Native execution and declared boundaries

`static_particles_visual_reference.py` runs original registry/controller
construction, Initialize, full CopyVertices `40a0c0`, Pulse `403760`,
controller Render/hide `404270`, RenderSample `4026d0`, CalcObjectMatrix
`40a420`, complete RenderObject `40a8f0`, blend selection `417d60`, scene
`4174b0`, FVF transform `56eb30`, and raster `56d960`. LitSoftwareFixture
executes the actual camera/blend-table startup, including `54df70`.

All seven literal assets contain only one four-vertex/two-face particle
prototype; other objects have zero faces. The actual caller selects prelit
FVF `1e2`, native packed particle color, blend 16, CULL_NONE, depth test on,
depth writes off, and ONE/ONE. No fabricated white/material/normal-light
carrier feeds the reference. Ambient 38/directional 1, ambient 0/directional 0,
and ambient 255/directional 1 controls produce identical pixels and depth.
Initial cull 1/3/2 controls converge through the actual blend setter.

Boundaries are immutable decoded authored resource records, owner GetAnimator
returning the original factory animator, COM material handle storage,
single-frame literal RGB565 texture residency, seed 1 CRT RNG, and ground 0.
Nonselected frames skip only raster after complete native dispatch; selected
frames execute it. This is an independent full particle draw caller proof,
not an original running map/spell/caster/device acceptance claim.

## Frozen evidence

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-alpha/`.
No images, executable, licensed assets, or generated probe binaries are committed.

- `y-visual-native-cached/`: original full caller, 90 ticks plus warmup,
  12 samples per type:1,5,9,15,20,25,29,35,45,59,79,89.
- `y-metal-cached/`: eight actual Metal films, 90 frames each, native-projector
  opt-in and ordinary common projector. Both use independent factory runtime.
- `y-visual-final/<type>/pair-manifest.json`: every paired PNG hash, capture
  binary/config, full-viewport panel hash and supplemental fixed detail crop.
- `y-metal-first/`: current-owner emitter sampling negative control.
- `y-map-final/`: four ordinary map create/controller/move/delete/absence
  runs, 32 typed assertions, 192 frames, restored floor, clean process shutdown.
- `y-pose-final/`: actual integer decoder/MakeMatrix versus original routines,
  all 32,805 TRS channels and 51,040 matrix channels pass.

The viewport is 1024x768, camera screen center 512,384, owner 0,0,0, 24Hz,
seed 1, quality 0, one discarded warmup render. Native capture N corresponds to
Metal film frame N for N>=1, following Pulse N+1. All 56 logged RNG checkpoints
across both policies match. Whole-viewport panels merely display at half size;
detail crops use the same fixed 384,256,640,512 rectangle on all three images.
No crop, translation, tint, or timing is fitted.

Final visual manifest SHA256:
`d3ff7efc97c7210c0b772128bfc0a5c1e1b03ee665a56df76d32c70dee410fc7`.
Capture binary SHA256:
`14ad6c184611c3e123b8f5c201f3847d258d0bd1d8ae29f15209173675202b7b`.
Original executable SHA256:
`28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.

Twelve-sample nullifier RGB L1 error falls 1,915,833→1,034,882 with the native
projector and 1,949,986→1,159,611 with the common projector. Its fan emergence,
spread and collapse now follow the original phase. The other three assets are
unchanged apart from tiny float-rounding differences; they are not credited
with an appearance improvement from this fix. Native/common projector and
software/Metal raster differences remain visible and explicitly bounded.

Lane visual review: all four pass the scoped whole-effect appearance check,
including YEnergy's continuing plume, YEnergyLose's disappearance, YAbsorb's
star-field progression and tail, and nullifier's emitter fan. Central ledger
and independent integration review remain root-owned. Exact pixels, natural
spell invocation, character/caster coupling, and original device parity are
not claimed.

## Reuse and verification

`--vfx=TYEnergy_AUTHORED_TAGS` selects the preview; replace YEnergy with any
of the seven exact case-sensitive names above. `--vfx-native-domain` selects
the independently proven native owner/projector policy. The preview owns and
destroys its normal factory object; it does not enter the map sector list.

Run the native fixture with the local Unicorn Python:

```sh
python tools/retail_runtime/static_particles_visual_reference.py EXE \
  --profiles Regeneration SwiftStrike fspray --output PRIVATE_DIR \
  --frames 90 --samples 1 5 9 15 20 25 29 35 45 59 79 89
```

`static_particles_visual_review.py --native DIR --metal DIR --output DIR`
freezes matched panels. Metal folders are `<type>-native` and `<type>-common`,
produced by `port_capture.capture` with the settings above. Its optional
`--negative-control DIR` retains before-fix comparisons. The manifest's
`accepted=false` prevents an evidence generator from granting itself review.

Validation: build PASS; five static PartSys tests PASS (including full original
caller/light/cull and stride 3 controls); ten capture-process tests PASS;
all four native pose probes PASS; eight isolated Metal films PASS; all four
ordinary map lifecycle captures PASS. Earlier `y-visual-native-final/` and
`y-visual-native-first/` used an advanced-frame Pulse and are superseded by
`y-visual-native-cached/`; they must not be reused as appearance references.

## Additional root review: Regeneration and SwiftStrike

The same complete original caller and independent Metal factory pipeline now
records twelve fixed samples each for Regeneration and SwiftStrike. Root reviewed
the whole viewport and fixed detail panels and approved their sampled emission,
form, color and continuing animation in the declared native-domain configuration.
Private frozen evidence is `research/vfx-static-closure-20261010/root-review-final/`
under the lab root; each type has a hashed `review-approved.json` linked from the
central ledger. Native/common actual films run90frames at24Hz with seed1 and one
discarded warmup, and every logged RNG checkpoint matches. Exact pixels and
natural caster/map invocation remain open.

fspray has the same complete measured captures but remains unapproved: its Metal
sprites look whiter/cyan than the original green-tinted software output. Its
source/clock/frontend proofs remain valid; a color diagnosis is pending.
