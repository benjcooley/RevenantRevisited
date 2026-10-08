# H02 — literal Waterfall runtime reference

Status (2026-10-04): source/state checks and prior combined-build/saved-load/command checks pass. A subsequent retail vertex-selection audit found and corrected an ignored-color submission error; that correction passes its extracted render test and awaits parent integration/capture. Full retail visual/runtime acceptance remains open. Scope is **Waterfall `0xa907dabf`**, asset **`misc\Water.i3d`**. No runtime registrations were added for WaterFlft, WaterFrt, WaterClft, WaterCrt or RiverFall. Those names/assets require their own animator evidence.

## Provenance and identity

Snapshot evidence is `/Users/benjamincooley/projects/Revenant/effect.cpp:11462–11666`, effect.h:1854–1908, and the corresponding preserved port snapshot `src/effect_old.cpp:11669–11873`. The latter has a malformed facing expression; the original source and executable resolve it unambiguously as integer degrees followed by TORADIAN conversion.

Supplied retail executable SHA256 is `28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`; compatibility executable SHA256 is `33545a2f4e055dfaa3a62e3b03488d6f3bd63fe6c9827b1f88e141e1387073eb`. Examined code `0x4f2c50–0x4f3260` is byte-identical. Saved assembly and constants are `lab/research/waterfall-retail-animator.asm` and `waterfall-retail-constants.json` under `/Users/benjamincooley/RevenantRetailLab`.

Retail builder startup `0x4f2c90` registers the string `WaterFall` at `0x5e1200`, animator factory table `0x5ad268`, instance vtable `0x5ad26c`. Recovered methods: InitParticle `0x4f2cb0`, UpdateStuff `0x4f2d70`, Initialize `0x4f2e60`, Animate `0x4f2f00`, DoLighting `0x4f2f20`, Render `0x4f3060`, RefreshZ `0x4f3210`. Older recon mapping labels wrongly merged the effect shell with Blood/Mist and called this animator Water; the actual initializer, constants and registration distinguish it.

The shipped placement audit records this exact type in Ahkuilon `Map/31_4_9.dat`, slot273, position `(5041,9503,-96)`, MapIndex95962744. See [FIRST_BATCH_MAP_PLACEMENTS.md](FIRST_BATCH_MAP_PLACEMENTS.md). The literal preview alias in current vfxtest uses `kVariantWater_I3D`, correctly `misc\Water.i3d`; the canonical preview is `TWaterFallEffect_BESPOKE`.

## State and RNG

The effect initializes 100 particles in index order, then runs exactly100 UpdateStuff warm-up ticks before the first normal update. Each InitParticle draws X from inclusive `[-64,64]`, divides by2.0, then draws width from inclusive `[2,4]` times0.1f. No Y/Z or velocity random draws occur. Height64, initial velocity0, alternating gravity0.25/0.5, tall scale2, depth scale1 and initial delay=`i` are preserved. Retail allocates4000bytes, confirming 100 records of40bytes.

Update preserves branch order: positive delay decrements and continues; `time==-1` calls InitParticle and resets height/velocity/time, then integrates position, subtracts gravity, calculates tall scale, finally marks `time=-1` on `z<=0`. Reset occurs next tick, not in the landing tick. Rendering performs no RNG calls and skips all particles whose time is not0 before lighting/submission.

`Advance(seconds)` uses true `TTime::LegacyFrameSeconds` (1/24), replacing the old truncated41ms gate. Simulation and Submit are separate. Negative, zero and nonfinite durations are ignored. No interpolation, physics-envelope adjustment or image-fitted tuning was introduced. Streams remain host libc RNG, unsynchronized with retail CRT; matching seed numbers do not prove matching retail trajectories.

## Authored rendering and lighting

The runtime borrows the actual owner imagery through normal TObjectInstance ownership. Subobject0 must contain four authored vertices; its texture is resolved from actual textured faces after upload. Original corner positions and individual UVs replace the old16wu screen billboard/UV rectangle. Material diffuse metadata is retained, logged and submitted without a fitted tint.

The source matrix order is Scale, Rz(-pi/4), Rz(integer negative face degrees × double TORADIAN), Translate(drop position), then the live owner transform. The executable computes truncated negative integer degrees and uses the double conversion constant at `0x5a8a98` (`0.017453292522222223`); the port rounds to float only after the double multiplication. The owner's transform retains its normal root scale/rotation/translation. No billboard extrusion or average-scale approximation remains.

**Correction: DoLighting's copied vertex colors are not selected for drawing.**
Initialize copies object0 as LVERTEX, and DoLighting computes grayscale from
three CPU-light brightness values plus ambient/255. However Render then
assigns `obj->flags = OBJ3D_MATRIX`, clearing `OBJ3D_VERTS`. The supplied
retail executable confirms this assignment at **0x4f30be** (`mov [edi],0x100`).
RenderObject **0x40ac4d** tests the VERTS bit (`test ah,0x20`), branches at
0x40ac50 to the authored array when clear, and writes **FVF0x112** at
**0x40ac7a**: XYZ, authored NORMAL and TEX1, with no diffuse color field.
The authored 32-byte vertex array is resolved at 0x40aca2–0x40acaf. Copied
vertex/FVF/pointer fields are read only on the other branch at 0x40ac52–0x40ac6c.
Material selection separately tests MAT0x80000 at 0x40abb1 and takes the
object's actual material at 0x40abc1 when clear. The matching snapshot path is
`Revenant/3dimage.cpp:1663–1704`.

The earlier port and test oracle incorrectly applied the unused grayscale to
submitted RGBA. This made ambient32/no-light particles use byte8 even though
that copied color never reaches the retail draw call. The corrected Submit
uses authored material diffuse RGBA and makes no ignored CPU-light queries;
state, RNG, geometry, transforms, runtime ownership and blend/depth remain
unchanged. Unlit is retained as an explicit **normal/device illumination gap**,
not as a claim that original lighting is already applied. Available LitFlat
uses world-up and does not reproduce authored normals/material/device response.
Retained dim captures are pre-correction visual diagnostics; they still prove
their tested runtime identity/lifecycle but do not validate current color.

Snapshot writes texture MODULATE, ONE/ONE blend, Z-test enabled and Z-write disabled. Retail Render starts with manager mode8; its mode8 branch at `0x418184` confirms ONE/ONE and no Z-write, with normal global Z-enable. Submission uses AdditiveStraight/TestNoWrite. Texture-stage device branches and the complete software rasterizer are not independently accepted by this patch; sampling/blending differences remain a visual gate. The shared lighting backend was not changed.

## Actual runtime attachment

A typed object builder registers only literal Waterfall. The animator hook checks EFFECT class, exact ID and positive final MapIndex before initializing. It waits for real uploaded geometry/texture before consuming initialization RNG. Idempotent initialization attaches exactly one `waterfall_reference` component, replaces the default static visual and logs owner identity, position, material, warm-up and first simulation tick.

The component calls Advance on its global update callback and Submit on the render callback. The owner stays persistent when offscreen; normal TObjectInstance offscreen animator/light bookkeeping remains, with no KillThisEffect override. It neither creates child map objects nor saves runtime components. Destructor deletes only its owned particle array; normal base destruction releases imagery/components.

Standalone SpawnForTest initializes with `attach_runtime_component=false` and its manual TickAndSubmit wrapper owns the Advance call. This prevents the preview from advancing twice through both manual and global component callbacks.

## Validation and remaining gates

`lab/check_waterfall_state.py` extracts actual original InitParticle/UpdateStuff and production InitParticle/UpdateStuff/Advance. It passed **24,100 exact particle-state and next-RNG comparisons**, 100 seeds over warm-up plus240ticks, and125 multiple-rate cases at15/24/30/60/144Hz yielding exactly48ticks in2seconds. Invalid duration and first-tick boundaries pass. Original and port positions, velocity, scale and branch state are compared exactly.

`lab/check_waterfall_render.py` checks the actual instruction bytes in the
hashed retail executable for the flag assignment, VERTS branch, authored
FVF/array and default material branch. It executes extracted corrected Submit
with production matrix helpers and an independent corner oracle: time-branch
skipping, zero ignored CPU-light queries, unchanged state/RNG, asymmetric
authored vertices/UVs, nonuniform scale, both rotations, live owner transform,
material RGBA and retained blend/depth/light policies pass. Prior syntax
validation covers the preceding implementation; the parent owns the corrected
integration build. Logs: `lab/research/waterfall-state-validation.log`,
`waterfall-authored-vertex-render-validation.log` and its matching `.json`
record current script/executable/Submit hashes; the older
`waterfall-render-validation.log` remains pre-correction history alongside
`waterfall-runtime-syntax.log`. No full
build, port capture or guest control was performed by this child agent.

## Parent integration evidence before vertex-color correction

The combined build and 163 existing tests pass (16 particle, 108 transform, 39 host capture/comparison). Tested binary SHA-256: `e09bd65a1c23e865fec9d5fc2eb41dfa3fc7910a40edfabf7fcf1c590a6d07fb`.

Retained results under `/Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/environment-saved-20261004/`:

- `waterfall-saved/runtime-capture/manifest.json` loads the byte-identical original slot273 record at `(5041,9503,-96)`, MapIndex95962744 and exact IDa907dabf. One `waterfall_reference` component attaches beside the normal animator. There are 121 distinct images in 150 samples. Frame31 versus91 contains 7,319 changing pixels, maximum channel difference15; this is visible but dim evidence, not accepted illumination.
- `waterfall-commands/command-capture/manifest.json` passes all seven actual ADDAT, identity/component, MOVE, persistence, DELETE and absence rows. Creation position is `(5041,9503,0)` and MOVE gives `(5061,9513,40)`. There are 49 distinct images; all final60 post-delete ground images are byte-identical.
- `validated-runtime-results.json` records both successful manifests, motion bounds, deletion tails and provenance, alongside MistFog.

The command camera deliberately remains at Z−128: both original `object.cpp:2329` and port `src/object.cpp:2899` clamp negative **new object** Z to0. Saved loading separately preserves original Z−96. Numeric ADDAT amount7 is not a Z coordinate, and the effect's absent Amount stat yields the source fallback1. The first command capture's wrong negative-Z expectation is retained as `command-capture-first-attempt`; only fixture expectations changed. No production clamp was altered.

Surrounding ground is synthetic even though the saved effect record is original. Offscreen persistence is source-audited; a camera-away/back exercise remains open. Owned capture processes produced every frame but stalled during teardown and required termination (exit−15).

Fresh native software `captures/runs/sw-environment-20261004-waterfall/` accepted ADDAT but showed no changing effect pixels, so it remains a rejected reference. A known Mist control did animate. Do not infer that the native device lacks RGB565 support or fit Waterfall brightness from this failure.

## Parent integration after vertex-color correction

The corrected build uses SHA-256
`9291e9e5d272988ef525ecd8b1cbe11b114c5289c90eaf257fa3bff3f0282005`.
The build, 163 existing tests and animation-system checks pass. Fresh results
under `captures/runtime-fixtures/environment-vertex-selection-20261004/`
retain the changed submission separately from the older dim captures:

- `waterfall-saved/runtime-capture/manifest.json` preserves the original
  ID/position/MapIndex and one reference component, with121 distinct images
  in150 samples. `effect-review.png` shows the authored texture/material
  streaks after removing the ignored grayscale override.
- `waterfall-commands/command-capture/manifest.json` passes all seven actual
  create/move/persist/delete/absence checks, with49 distinct images in150
  samples. The original negative-new-object clamp semantics remain intact.

The brighter ambient128 native attempt is retained separately at
`captures/runs/sw-environment-bright-20261004-waterfall/manifest.json`.
The ground and settled image below the FPS row are identical; no animation
was detected. It remains a rejected reference and does not identify the cause.
No fitted color or geometry change was introduced to match that absence.

Next: obtain a visible exact-asset native reference, then verify authored-normal/material
illumination and the original Caverns placement. The independent CPU-light
registry audit remains useful to effects that actually consume it, but the
unused Waterfall copied colors must not be used as its render oracle. Light
response, texture sampling, device rasterization, full environment/story and
any sound remain unaccepted. General-effects migration follows those gates.

## Thin native state and authored pixels, 2026-10-07

The [thin literal Waterfall report](../../../recon/retail_asm/runtime/effects/waterfall-frontend-ab/manifest.json) executes registered original Initialize `0x4f2e60`, Animate `0x4f2f00` and Render `0x4f3060`, including original InitParticle/UpdateStuff, allocation and all 100 warmup ticks. Compiled current actual shared map/preview initialization state, update, submission and matrix methods agree in every position/velocity/scale float32 field and integer time over ticks 0–96. It checks 9,700 particle records and 87,300 float32 fields. Original range-RNG is observed unchanged and replayed with exact ranges/order:602 calls at initialization, 1,566 total. The checked interval contains 482 landings and 482 respawns. No production fix or visual fitting was needed.

Eight selected authored geometry/UV/software RGB565 image/depth pairs match exactly and repeat twice with complete native state. Maximum corner error is below 7.69e-6. Exact `Misc/Water.i3d` geometry/texture/material inputs are used; both effect frontends feed the same original projection/raster. The selected camera/owner/face and white vertex inputs are explicit. Shared software target triangulation uses retail authored indices; modern quad triangulation is separate.

The existing ignored-color finding is preserved: native copied format 0x1e2 is asserted, Render clears VERTS, and each unused copied-color DoLighting callback is explicitly counted/omitted. Controlled white vertex input is not a proof of authored-normal/device illumination. Native map/component binding, arbitrary world/face poses, normal lighting/culling/blending and Metal remain separate. This closes a fast deterministic source/frontend gap beyond the earlier guest diagnostic; it does not clear other WFall/WCap controllers. See [instructions](../../../recon/retail_asm/runtime/effects/waterfall-frontend-ab/README.md).


The first thin state reader was found to visit only 50 of the 100 native records while the generated port and rendered output covered all 100. Its originally claimed full-pool field counts are withdrawn; the preserved `before-full-pool-*` artifacts are partial-state evidence. The reader now covers all 100 and asserts both native/compiled-port cardinalities before comparison. The complete 97-sample rerun verifies actual 9,700 records/87,300 fields and 482 landings/respawns, with exact two warm state/RNG/pixel replays. All eight color/depth hashes remain unchanged. Current manifest/coverage supersede the earlier scope claim; no production or raster change was made.
