# Burn ordinary-world projection correction

Burn `0x55471bff` was passing its original software MODELZ matrices directly
to the ordinary renderer. This compressed and lowered the rising fire/smoke
plume. The fixed submission uses the same coordinate contract as the existing
ordinary `T3DAnimator::SubmitPartSys` and object transforms:

```
ordinary vertex Z = particle's raw map Z
                  + (native vertex Z - native matrix center Z) * WORLD3D_Z_SCALE
```

`WORLD3D_Z_SCALE` is the existing engine constant1.5, not a value selected from
images. The actual original slot position supplies the center; the render
matrix supplies the local MODELZ offset. XY, simulation, RNG, particle life,
authored mesh/UV/material, damage and native-domain matrices are unchanged.
The inverse normal scale keeps original lighting normals after the ordinary
Z stretch. Neither the effect owner nor the victim's position is added again.

The correction lives in `src/effects/burnprojection.h` and
`src/effects/burnpublicpreview.cpp`. It changes the ordinary gallery's existing
`Burn_AUTHENTIC_ACOLYTE_INTEGER` route. It does not turn that fixture into a
natural moving-character or spell-caller test.

## Original and source execution

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-burn-common-agent-20261011/`.

The fresh original90-tick run constructs the real Acolyte/22-object animator,
typed Burn owner and Burn animator, invokes the actual public setter, owner
Pulse, custom Animate and complete custom Render, and runs normal Illuminate
and the original RGB565 raster kernel. Actual character bone calculation and
Damage execute. The declared shared inputs are Acolyte state14/frame0,
facing96, interpolation disabled for integer sampling, origin(0,0,16),
post-actor seed2, camera(320,320), complete640x480 viewport and the existing
white ambient32/normal lighting fixture. HP102→97 and finite kill75 are retained.

Boundaries remain explicit: generic map/world owner Pulse is upstream, the
owner/FIX/facing/PreRender matrix operations use original instruction
fragments through `native_owner`, and full tag/audio/base traversal and
natural map insertion are outside this fixture. No original effect method,
bone, stat or damage return is replaced with a host model. Readonly original
slot centers and render matrices are exported only to the invariant checker;
they are never injected into production gameplay.

- Fresh original/source state comparison passes108,750 meaningful state
  fields,90,240 matrix fields and90 RNG states; maximum error1.58e-5.
- The compiled **actual production submission branch** passes22,560 original
  native vertices and22,560 ordinary coordinate-contract vertices across
  5,640 draws. It checks all8 authored vertices, raw original slot centers,
  native matrix preservation, XY, local Z stretch, exact raw map translation,
  additive/cull-none flags and normal scale, under ASan/UBSan.
- Full private game build passes. Both native and ordinary90-frame captures
  complete with clean exits. Every new native pixel buffer equals the prior
  approved90-frame recording exactly. The source controller is unchanged.
- Both actual captures have identical5,714 phase/active-slot trace rows, and
  all74 logged live-phase health values agree with the genuine original actor.
- Ordinary gallery100-frame smoke passes:89 visible/90 distinct frames,
  two finite creations and live game RNG without per-factory reseeding.
- Four existing Burn unit tests pass, including integer interpolation-policy
  restoration on failure and the expanded ordinary submission contract.

The original and actual ordinary source14-phase review is saved in
`review/whole.png`; `review/detail-*.png` also includes the previous ordinary
render. Raw frames are unchanged. Fixed ROI(256,120,384,340) and clearly labeled
2x nearest enlargement are display-only. At tick16 the unmodified original
bounds are(292,160,346,310), before ordinary(293,208,346,315), and corrected
ordinary(293,163,346,311). These bounds describe the observed fix; they are
not its criterion or a fitting input. Independent approval remains separate.

## Reproduce

Use the retail lab Python environment and `PYTHONPATH` described in the
[emulator guide](../../RETAIL_AB_TESTING.md). Run from this worktree:

```
python tools/retail_runtime/burn_actor_visual_reference.py \
  --output PRIVATE/native90 --ticks 90 --no-interpolation --seed 2
python tools/retail_runtime/burn_projection_reference.py \
  --native PRIVATE/native90/manifest.json --output PRIVATE/projection-proof
python tools/retail_runtime/burn_state_probe.py \
  --native PRIVATE/native90/manifest.json \
  --source-log PRIVATE/source-actual-pose.log --output PRIVATE/state-proof
python tools/retail_runtime/port_capture.py \
  --scenario PRIVATE/scenario-common.json --output PRIVATE/common
python tools/retail_runtime/burn_common_review.py \
  --native PRIVATE/native90 --common PRIVATE/common \
  --before PRIVATE/common-before --output PRIVATE/review
```

The source scenario uses effect `Burn_AUTHENTIC_ACOLYTE_INTEGER`, the declared
inputs above,90 frames at24Hz/warmup0, black background and
`native_domain:false`. Run a separate `native_domain:true` scenario to verify
native regression. Pin the executable before either capture; use separate
save folders and keep data, binaries and images outside source Git.

## Evidence pins and remaining scope

- Frozen private binary SHA256:
  `ad6253ba44c06546be4b4c691fbdf0dff5d0391d9c4f9c63b1fbeec7592b5dca`.
- `native90/manifest.json`:
  `cd06a4fc67f5475bc3e346c3e7dd11444dbbeb4829860a77343b72ecf4a57448`.
- `original-projection-invariants/manifest.json`:
  `95ab3f87859687aca922ee39eb2bb0f914fcb579234e81eb14852aef1295fca6`.
- `fixed-common/manifest.json`:
  `2cf8359041b8061d5400d77975cb42ca77db22b1b1c7de79386ea6ebf30308b0`.
- `review/manifest.json`:
  `76bf50f2d56572c5c4866d2e44d1142c32c70f2da06eda04aebc91c334a5855f`.

Natural moving/animated victims, other rigs, Burn() poster/map insertion,
reignite, spells/audio and graphics-device behavior remain open. Common
projection intentionally differs from original software projection; retained
float/edge/texture/RGB565 and framebuffer residuals are not exact pixel parity.
This correction earns no full-acceptance or additional type-row count.
