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

ArrowEffect now has its own exact type0xad92bd1f adapter and
`SpawnArrow(source,target)` API. Its declared physical caller begins at
(-180,0,78), facing64, with fixed16.16 velocity20 and weightless flags. Original
Move470920 advances the source before original Effect attachment copy4de800
and Arrow Pulse504920. At the declared terrain planeX180, blocked Move causes
an explicit source-deletion notification to original Notify4df0f0; that routine
clears the matching attachment and kills the follower at native record18.
A notification for an unrelated object leaves the follower unchanged. This
fixture proves the moving leaf and notification boundary, not natural bow or
physical-arrow construction/map deletion dispatch.

Compiled production state/RNG matches all26 ticks (260 actual fields), and
4,928 Render matrix fields agree within4.633e-8. Full and stride3 Render tests
preserve jitter cadence. The earlier shared trace printer included an unused
zero word at owner+0x1a0; ArrowEffect has no Queen countdown field. That word is
excluded from the state comparison and removed from the maintained fixture.
The common core's negative countdown sentinel only selects constant spacing3.
Fresh native replay matches all actual state, RNG and draw packets exactly;
phase1/10/17/18 PNGs are byte-identical.

Both actual24-frame Metal films exit cleanly, with36 active RNG milestones
matching native. Nineteen whole/detail phases are frozen in
`hard-alpha/arrow-attached-review-final`, pair-manifest SHA-256:
`4e2b0516021ce04edba47a0c2f857e29a5cf6f26ad0439dd447f8970ffb82b71`.
All full reference frames fit640x480; the supplemental Arrow crop includes the
complete final trail. A fresh QueenArrow native-domain film using the attached
implementation is byte-identical to its previous24 frames. Private regression
record SHA-256:
`b00ba49f0c99fb861690ef03bacb01a81648fb9d0f4f9f4031e7f6581d13e185`.
The Arrow registration uses its real IsAlive callback; gallery endpoint wiring
is a separate integration. Build and all three native regression tests pass.
Natural actors/bow/damage/audio, hardware device behavior, raster edges and
exact framebuffer equality remain open.

Root independently approved all19 ArrowEffect whole/detail phases after the
fresh original replay and unchanged Queen control: growing moving gray trail,
matching attachment deletion and real kill/blank tail agree in the declared
native-domain condition. `arrow-attached-review-final/root-approval.json`
pins source checkpointc5a70169, both films, panels, actual binary and witnesses.
Approval SHA-256:
`5e54c04e9e8ce980633379430dd106884da99f222bae5712b077803adf9e6f3a`.
Captured binary SHA-256:
`4f4307c7f75eb0ffbdfc0f5079849223ea2a02b66e11b9ce00eff926197a1240`.
The three final runtime regressions pass in24.193s; the documented natural
caller/device/raster limits are unchanged.
