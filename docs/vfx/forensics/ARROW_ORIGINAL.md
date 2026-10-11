# Original ArrowEffect and QueenArrow

Exact bindings are `ArrowEffect:0xad92bd1f` and `QueenArrow:0xad92bd1e`, both
using `Misc/arroweffects.i3d`: seven untextured color meshes, 56 vertices and
42 faces. They are distinct original classes, not tint aliases. Complete
registrations select owner factories5097c0/509940 and animator factories
509870/509a30. Original Effect constructor4de770 executes in the owner proof;
TObjectInstance resource/definition construction46e1f0 remains an interface.

The first moving reference executes QueenArrow owner504d20, generic Animate
40e460, custom Render505000, original RenderObject/normal lighting and software
raster. Source(-180,0,38), facing64 launches at sourceZ+40 and moves23 units per
pulse. The declared stationary collision query returns a fixture character at
X>=180. Original reaction and damage calls are observed boundaries. Native
owner countdown kills at zero-based tick18; no preview lifetime is invented.

The production core matches40 original ticks, all440 state/RNG fields, and
1,312 sampled full-render matrix fields (maximum error4.742e-7). Render consumes
its own jitter RNG every call; simulation-only cadence must not consume those
samples. Native Scene8 selects ONE/ONE, CULL_NONE and ZWRITE_FALSE, but its
untextured software kernel has separate raster behavior. A port appearance
claim requires the correct existing untextured software helper policy.

Private initial evidence is in RevenantRetailLab
`research/vfx-wide-20261010/hard-alpha/{arrow-native-preflight,queen-arrow-first,queen-arrow-core-first}`.
No Metal appearance acceptance is granted by this checkpoint. ArrowEffect's
attached moving-owner path and natural arrow/bow/queen callers remain open.

The QueenArrow adapter now selects exact type0xad92bd1e and fingerprints all
seven literal untextured mesh streams. It registers seven shared meshes, carries
the original object2 material in common hardware, and uses the already proven
untextured RGB5/flat-depth software helper without renderer changes. Full
geometry and the authored Scene8 additive policy remain available in hardware.
The registered `QueenArrow_ORIGINAL` exposes real core/owner liveness.

Build and two native regression tests pass (27.945s). The full/stride3 controls
execute original moving Pulse/Render and compare all state, RNG and available
packet matrices to the compiled production core. Both actual24-frame Metal
films exit cleanly. Their36 active RNG milestones match the independent native
reference exactly. Fresh26-tick native state replay is exact and phase1/10/17/18
PNGs are byte-identical. All full original/native frames fit640x480. The fixed
detail crop is supplemental and may cut the bottom of the last tiny impact
shape; complete whole frames remain authoritative.

Nineteen phase comparisons are frozen under
`hard-alpha/queen-arrow-review-final`; pair-manifest SHA-256:
`a9bab890cbe30a91d67a6364d1d0752d95548021c62720aa405d2e63e013af19`.
One-based film fileN follows pulseN+1/native recordN, verified from CSV and RNG
logs. Independent appearance approval is pending. Natural queen/bow dispatch,
actor selection/reaction/damage, audio/device and exact pixels remain open.

Root approved QueenArrow's scoped native-domain moving appearance after all19
whole/detail phases and full actual-size9/17 impact views. Original gray
untextured brightness, launch, trail growth, collision collapse and real kill
agree; the caller/device/exact-pixel limits above remain open. Frozen private
`queen-arrow-review-final/root-approval.json` pins the pairs, panels, native
replay and captured binary. Approval SHA-256:
`5d7ec006a594db826d78997689ce53b54c234c2c77fcfa68f3a8a419b05f7ee9`.
