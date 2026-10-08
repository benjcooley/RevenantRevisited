# CombatFlash state0/start1: bounded authored runtime

2026-10-05. **State0/start1 source integration, actual runtime and paired-preview workflow pass. Visual review is inconclusive; other 17 states and natural combat remain unaccepted.** This is one partial retail row, not acceptance of all impact variants.

## Identity, asset and first state

Shipped EFFECT `combatflash`, **0xad92bd29**, binds `misc\Impact.i3d`. The exact archive member is `Imagery/Misc/impact.I3D`, SHA256 `27f76140ed0c1a8a4bb2dd404c068422feb7d49345ec98e1c9c1b3dd159c7725`. Preserve the directory: `Magic/impact.I3D` was an earlier basename-collision hazard.

The version-3 body has flags 0xdc, 48 vertices, 24 faces, 21 objects, 11 materials, 10 textures and 18 states. Nine 19-frame start states alternate with one-frame end states. The enabled profile is **state0/start1**, 19 frames, nonlooping/NOMOTION flags 0x2000. A valid first-state clip does not prove the other states, their trails/grammar or the real impact selector.

State0 uses three objects:

- Object 0 `#particle01`: four-vertex/two-face particle prototype, hidden between controller submissions.
- Object 1 `impact01`: empty authored emitter.
- Object 2 `#impact01`: four-vertex/two-face base flash, retaining its authored pose and material.

Both populated objects use **material 0/texture 0**. Texture 0 is one 64×64 RGB565 image, masks f800/07e0/001f and no alpha mask. Material 0 diffuse, ambient and emissive RGBA are all `(1,1,1,1)`; specular RGB is approximately 0.9, alpha 1, power 0. Preserve the descriptors without inventing tint/specular/brightness. The software helper interprets its source normal lighting separately from the prelit particle colors.

Their source faces are `(2,3,0)/(1,2,0)`, with original corners and UVs. The particle FX-quad mapping `{0,1,3,2}` preserves that diagonal/winding. Exact [asset metadata](/Users/benjamincooley/RevenantRetailLab/research/ledger-reconciliation-20261005/combatflash-start1-integration/asset-state0-metadata.json) retains material, texture, object-key and tag evidence.

## Authored controllers and draw policy

State0/frame0 has `blendcont: litadd`. State0/frame1 has:

```text
obj=(impact01),particle=#particle01,pps=[0:250,2:0],initialvelocity=15:30,scale=[0:2.5,100:0],lifespan=30:45,localrotation=[0:(0,0,0),100:(0,360,0)],friction=0.3,gravity=0.5,color=[0:(255,225,100),25:(255,175,50),99:(0,0,0)],spread=120
```

The tags instantiate on state refresh; stored tagframe1 is not a delayed activation. PPS/expressions read the cached animator frame, while emitter pose follows current owner state/frame. Keep those inputs separate. Full-quality capacity is `floor(250*45/24)=468`; actual births 10→15 are logged. Capacity is not a simultaneous-visible-particle target.

Both visible names contain `#` and neither contains `$`. The scoped bridge preserves the recovered fixed camera matrices after source scale/rotation and before translation. Particles apply that orientation after their sampled rotations; the base preserves its authored pose and live root. Gold's dollar-derived 0x40/no-depth behavior is **not** borrowed.

Mode 16 is **ONE/ONE additive, Z testing enabled under the source global policy, no Z writes**. Particles use authored RGB565 texture/UVs and source packed RGB/alpha truncation as prelit/unlit FX quads. They are not normal-lit material helpers. The base uses authored normals/full material through the existing explicit RGB565 software-helper path, depth retained.

Actual retail default Render invokes controllers at **0x40e8ed** before base RenderObject at **0x40eaa3**. Both map and preview routes therefore submit the base through the existing after-FX helper queue, with `retail_gold_no_depth=false`. No new renderer behavior, generic parser extension, color quantizer, point light or fitted geometry was introduced. [Retail ordering/color contracts](/Users/benjamincooley/RevenantRetailLab/research/combatflash-state0-20261005/retail-contracts.json) retain original/compat byte-identical ranges.

## Scope and actual owner lifetime

The preview ID is `TCombatFlashEffect_AUTHORED_START1`. Its adapter uses real exact-class `NewObject`, positive final identity and one default animator, not the historical `TCombatFlashEffect_BESPOKE` tinted billboard. Advance and world submission are separate; no periodic respawn or render-driven simulation is added. The profile and nonlooping cleanup are enabled only for exact ID/state0. Later states remain explicitly unsupported and retain their prior behavior.

The staged source report retains its original `staged_only` scope. Root's applied record confirms all seven resulting files match the frozen draft, and the subsequent engine build/runtime/pair supply separate execution proof. The 24 checks cover profile, material/topology, camera/depth, packed colors, ordering, identity, cleanup and ownership; they are source contracts, not a full retail simulation or combat test. Scheduling with multiple catch-up ticks and cached render-frame dependence remains a separate gate.

## Terminal integration evidence

Binary **`f4fe9f56044f156fdf3d3700e17ddf9fa62d25713a20395414e1940b23a2fc69`** builds successfully. Source/draft/applied hashes are under `lab/research/combatflash-state0-20261005/`.

- [Corrected static runtime](/Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/combatflash-generic-static-capacity-correct-20261005/command-capture/manifest.json): four actual creation/animation/natural-cleanup checks, one default animator, 21 distinct images and 40 exact-ground tail frames.
- [MOVE/DELETE runtime](/Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/combatflash-generic-move-delete-20261005/command-capture/manifest.json): eight checks, visible original/moved output, 13 distinct images and 100 exact-ground tail frames.
- The first static fixture remains **FAILED** because it expected capacity 1 instead of the source-derived 468. Its corrected recipe used a fresh directory. This is a fixture expectation error, not an engine failure; it receives no extra credit.
- [Actual retail A/B](/Users/benjamincooley/RevenantRetailLab/captures/ab/sw-fps-combatflash-authored-start1-20261005/manifest.json): 240 port frames against the own FPS-enabled clean state0 reference, both background errors 0, no position/scale/color/phase/RNG fitting.

Root review is **inconclusive**: both have a comparable flash/streak shape and 23 visible samples, but early onset and late brightness/progression differ. Native first-visible onset is not proved simulation tick0, and random streams differ. Do not phase-align or fit brightness/lifetime to obtain a pass. A second native seed or source-backed cadence/raster review is needed. The active/full-active zoom sheets and metrics are hashed in the manifest.

[Hash verification](/Users/benjamincooley/RevenantRetailLab/research/ledger-reconciliation-20261005/combatflash-start1-integration/integrity.json) covers 240 runtime images, 12 successful rows, 240 paired images, seven media files, fixture/module/command/log/timing/reference hashes and source snapshots. Completed owned captures required termination during stalled teardown; clean shutdown remains unproved.

## Remaining acceptance

- [ ] Overall state0/start1 visual fidelity, including onset/cadence and source raster progression.
- [ ] Other 17 states, trail fields and exact start/end transitions.
- [ ] Real impact-state selection, collision normal, attacker/victim transforms, natural combat and owner lifetime.
- [ ] Original setting, point lights, occlusion and multiple-owner ordering.
- [ ] Full retail acceptance in the [176-entry burn-down](../EFFECT_BURNDOWN.md).

This checkpoint raises actual runtime/preview/paired workflow coverage once for the retail type. It grants no visual, other-state or natural-combat acceptance, and does not shorten the all 176 finish line.

The post-integration [Gold regression](/Users/benjamincooley/RevenantRetailLab/captures/ab/sw-fps-gold-post-combat-regression-20261005/manifest.json) completes 240 raw images byte-identical to the previous corrected Gold pair. This shows the state0 profile preserved existing Gold output; its visual deferral remains unchanged and no natural-trigger credit is added.

## Current post-Z runtime protection, visual inconclusive

Currentbb0a natural4/MOVEDELETE8checks/40+100groundtails and own240-frame pair complete. [Root review](/Users/benjamincooley/RevenantRetailLab/captures/ab/sw-fps-combatflash-post-z-domain-20261005/visual-review.json) remainsINCONCLUSIVE: nativeahead/portbright0.4–0.6sec, exact initialphase/cadence andPNGpreviewdepth/nativefloor unisolated. No phasefit or appearance credit; other17states/naturalcombat selection/context unverified. [Current shared hashes](PARTSYS_Z_DOMAIN_HANDOFF.md).
