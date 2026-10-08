# PartSys Z-domain handoff: raw center and local model offsets

**Terminal update:** both Gold reference pairs now PASS isolated default appearance. Earlier A/B-pending sections are source-stage history; native cadence/map collision/caster/audio/context/full acceptance stay open.
2026-10-05. **A minimal shared source coordinate bridge is built; new Gold A/B is pending.** No appearance or full acceptance credit. The source/math checks are bounded and retain a known camera-basis residual.

Original particle simulation stores logical/map XYZ. SampleRender applies the recovered retail logical-to-model Z conversion before authored prototype transforms. Modern normal meshes preserve raw common-world translation and apply WORLD3D_Z_SCALE1.5 only to local model offsets. The old PartSys quad submission handed its full transformed model Z directly to the modern common-world projector.

Root applies only the `src/3dimage.cpp` handoff at3144, preserving raw particle state center and converting localMODEL offset:

```cpp
(particles[slot].position[2] +
 (world.Z - sample.position[2]) * WORLD3D_Z_SCALE) * render_scale.Z
```

Patch `bf191b3b1231cef9dd36513194292738c7148930d2777e12a8a056e5ee207440`, targetSHA `31e59323d42c54517688ea9d7b9a97fb06bbeef7291ee89bf36d54ca769be6ed`; build0 binary `bb0a0f1c4fe9dbd024d11360fa0410bd7d291bfa0cee1145d644a814f3d84cf0`. No physics/velocity/timing/XY/UV/color/profile/shader changes. It applies to authored PartSys submission, not a Gold-only size/height fit. Initial homogeneous/fullcenter×1.5 proposal is archived and **not used**.

[Handoff validation](/Users/benjamincooley/RevenantRetailLab/research/partsys-z-domain-20261005/handoff-validation.json) checks36corner/9center source/math cases, rawcenter preservation and caller scale. [Actual-retail numeric source proof](/Users/benjamincooley/RevenantRetailLab/research/partsys-z-domain-20261005/z-domain-proof.json) executes controlled FIX/matrix/camera/projection instructions with authored Gold geometry. These are not compiled production bridge/GPU/live-owner or picture proofs. Even after localnormalmesh conversion, modern localZ coefficient1.3005 versus actualretail1.237179 leaves **5.118%** residual. No exact fullprojector or nativefloor/cadence acceptance is inferred.

Two own retained native Gold references are the next A/B check. Existing Gold arc/collision/duration and naturalcontext remain open. All thirteen earlier isolated appearance passes retain their tested-binary scope; they are not automatically current-bridge regression proof. [Source snapshot/hash verification](/Users/benjamincooley/RevenantRetailLab/research/ledger-reconciliation-20261005/might-pool-dispatch-and-z-checkpoint/integrity.json) preserves the staged/applied source, build and bounded numeric scope. No coverage counts change.

The same checkpoint records corrected Might dispatch: canonicalONE/ONE uses54e3a0/5605b0 additive, not54e810/567020 non-additive. Existing production parser/state pool proves65,396meaningful fields under controlled inputs, while canonical pixel math and exact Might owner/runtime remain pending. [Might scope](MIGHT_NATIVE_REFERENCE_GAPS.md). All176 mandatory rows remain unfinished in the [ledger](../EFFECT_BURNDOWN.md).

## Completed Gold appearance and runtime/protection

Both own240-frame currentbb0a Gold pairs rootPASS default coin shape/scale/color/spray/higharc with independent RNG; no phase/height/physics/scale/color fit. Source rawcenter/localMODELoffset bridge restores porttop39 vsbothnative40, previousport75. Mask>8/FPS12excluded nativelast42/43 vsport45 differs from historical53/54mask scopes, which remain preserved. Known5.118%localZcamera approximation and exactnativecadence/fullcycle remain separate.

Gold natural4/MOVEDELETE8rows40/88groundtails and Combat4/8rows40/100 pass; Cure240rawframes EXACTpostDepthStorebaseline. Four existing water controllers each pass150frames/12rows/sixemitters/cap140/140/52/52, runtime protection only. Combat ownpair remainsINCONCLUSIVE: nativeahead/portbright0.4–0.6sec, initialphase/cadence andPNGpreviewdepth/nativefloor unisolated; no other17states/naturalcombat acceptance.

[Immutable final verification](/Users/benjamincooley/RevenantRetailLab/research/ledger-reconciliation-20261005/z-domain-final-checkpoint/integrity.json) checks1080runtime/protectionimages,720pairedimages/contact/reviews and240exactCureimages. Gold isolatedpass raises13→14 only; repeatableGold protocol was alreadycounted. SectorlessFloorCallback0 is not nativewalkheight/collision; naturalcaster/audio/context/full acceptance remain open. Earliera89historical44frame cause is not newly explained. Latest Ripple user confirmation retains its same existing ringappearance pass without matching randompositions or another count.

Subsequent Might source/runtime/pair is implemented and protected, but the firstquality0 brighter/denser mismatch is now a known settings mismatch against read-onlynativequality1. Matchedquality1 capture/review is pending; no Mightvisual credit. [Current Might scope](MIGHT_NATIVE_REFERENCE_GAPS.md).
