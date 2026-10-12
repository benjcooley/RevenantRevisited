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

## Display-cadence correction, 2026-10-11

The review adapter previously called the trail's random jitter producer on
every display Render. At 60 Hz that resampled jitter more often than the
24 Hz legacy animation. It now samples and caches packets once per legacy
update, then submits the cached geometry once per display frame.

The compiled actual `Advance` span produces identical per-tick owner state,
RNG state/count and every cached packet channel at 24, 60 and 120 Hz for both
QueenArrow and attached ArrowEffect. Four native/adapter tests pass under the
existing controls. Fresh 24-frame native-domain captures of each effect match
all 48 prior approved decoded images exactly, including blank tails. Evidence:
private `research/vfx-arrow-cache-root-20261011/baseline-comparison.json` and
the two capture manifests. The full game build passes.

This verifies the declared adapter cadence and preserves the isolated 24 Hz
appearance. The user's ordinary moving-gallery appearance report remains open;
shared RNG ordering among simultaneous effects, natural actors, audio, skipped
native Render calls and device/raster limits are not certified by this check.


## Actual reviewer direction control, 2026-10-11

`arrow_attached_visual_reference.py --gallery-direction` now uses the actual
logged path delta(-170,-219), sourceZ144 and facing229. XY is algebraically
relocated to source(0,0); no per-tick positions are supplied. Original startup
builds the facing tables. Their literal velocity input is(-803840,-1034240,0)
in16.16; original Move470920 computes the trajectory. The declared terrain
boundary is a stationary plane through the target, rather than the old+X wall.
Original Notify handles the explicit source-deletion event after Move blocks.

Two independent24-tick original replays match every state/RNG/event and ordered
draw matrix. The actual source core matches240 state fields and3,648 matrix
channels; maximum matrix residual4.960937474152161e-7. The first position is
(-12,-15,144); deletion starts at zero-based tick14. All existing horizontal
and sparse-render controls remain passing; five arrow tests pass.

The first source checker compared9-digit decimal velocity output against exact
native float values and failed on decimal rounding alone. Its attempt is
retained under `source-first`; velocities now print as hex floats and compare
exactly, without widening a state tolerance. Matrix printing retains the existing
declared error bound. `source-exact-velocity` records the corrected check.

Private evidence: `research/vfx-arrow-gallery-direction-root-20261011/`.
`direction-proof.json` SHA256:
`061500c77a5b371d130bdf4bb31fb117470c04a181299a9a5d7f32d86b345de4`.
This extends motion/controller coverage to the review direction; it is not
rendered ordinary-gallery approval. The physical source/bow object is an
upstream fixture and is not drawn, so the comparison covers the trail itself.
Natural parent rendering, camera/raster/lighting and scene interaction remain
open. No effect-production code changed in this direction audit.

## Unclipped diagonal lifecycle, 2026-10-11

The early 640x480 diagonal pictures clipped the trail while the owner was still
alive. The attached-original tool now accepts `--resolution W H` and
`--center X Y`, preserving its 640x480 / (320,320) defaults. The new comparison
uses a declared 1280x960 surface and (640,640) center for both the original
software raster and actual port renderer. The origin (0,0,144), destination
(-170,-219,144), facing229, seed1 and 24Hz cadence remain unchanged. Nothing is
fitted to the images, and no effect code or shared renderer policy changed.

All24 original state/RNG/event records and ordered draw matrices are identical
to the earlier 640x480 capture; pointer-valued draw arguments differ because the
larger surfaces change fixture allocations. The source core still matches240
state fields and3,648 matrix channels, with maximum matrix residual
4.960937474152161e-7. All14 live frames fit inside the surface in the original,
port native-domain and port common-domain captures. Deletion begins at zero-based
tick14; all10 remaining frames are blank in allthree recordings. None of the
full-frame effect bounds touches an edge.

`arrow_fullcycle_review.py` checks those frame-pairing/lifecycle/bounds invariants,
pins all72 full PNGs, checks the actual port phase/growth/RNG logs against the
original live records, and packages a supplemental detail panel using the same
fixed rectangle (600,240,760,480) without scaling or color changes. Its metrics
are measurements, not appearance thresholds: native-domain support IoU is
0.8767–0.9004(minimum/median), with at most one pixel of bounding-edge difference.
Pixel equality is not claimed. The common projection remains a diagnostic.
The independently viewed native-domain samples have closely corresponding
shapes, growth, trajectory and deletion, while ordinary moving-gallery approval
and natural physical-arrow/bow/map deletion dispatch remain open.

Private evidence: `research/vfx-arrow-fullcycle-agent-20261011/` under
`RevenantRetailLab`. Full package `review-final/manifest.json` SHA256:
`7b6e54b85e79b84bbeb82000aafb36654c77f8261856daf918a9b0c1b38e1afa`.
The frozen port binary SHA256 is
`9875275ea0a1c820e1de8f63c1655dd7d259af70635714f8f8b8001bc3ed808a`.
This comparison uses the same private endpoint-enabled source binary as the
previous clipped attempt, rather than rebuilding during another agent's work.

Reproduce the original frames from the worktree, with the retail-runtime Python
environment and `PYTHONPATH=tools/retail_runtime:tools/retail_asm`:

```sh
python tools/retail_runtime/arrow_attached_visual_reference.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output PRIVATE/native --gallery-direction --frames 24 \
  --resolution 1280 960 --center 640 640 \
  --samples 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
python tools/retail_runtime/arrow_core_probe.py PRIVATE/native/trace.json \
  --output PRIVATE/source
```

Use `port_capture.py` twice with explicit scenarios matching those coordinates,
24frames, fps24, warmup0, seed1, black background, resolution[1280,960],
camera[640,640], rectangle[0,0,1280,960], ambient[32,255,255,255], and
`ArrowEffect_ORIGINAL`; set `native_domain` true/false and write respectively to
`PRIVATE/metal-native` and `PRIVATE/metal-common`. Pin an owned source binary and
independent save directory in each scenario. Then package the unchanged frames:

```sh
python tools/retail_runtime/arrow_fullcycle_review.py \
  --native PRIVATE/native --metal PRIVATE --output PRIVATE/review
```

These original fixtures still declare immutable resource/COM setup, base
animator initialization, terrain plane and explicit source-deletion delivery;
there is no newly certified natural scene, actor, audio or device result.
