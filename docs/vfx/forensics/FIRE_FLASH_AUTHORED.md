# FireFlash authored controller and runtime bridge

Current status, 2026-10-05: final authored controller/source-software-helper build and 11 actual lifecycle checks pass. Color is corrected; native orange annulus versus softer filled port cloud remains visual deferral. Natural spell/target damage and original setting remain unaccepted.

2026-10-05. Source correction built; actual command runtime and own retail A/B are retained. This is not visual or natural-spell acceptance.

Exact retail type: `FireFlash`, `0x37780ae2`, `Magic\fireflash.I3D`.
Preview: `TFireFlashEffect_BESPOKE`. Actual map component:
`fireflash_reference`. The older `TFireFlashAnimator_SHIM` remains unchanged
and is not the corrected candidate.

## Source and asset evidence

Original `/Users/benjamincooley/projects/Revenant/effect.cpp:1851–2354` contains
the effect builder, guarded damage initialization, animator initialization,
particle loop and render. Original `object.cpp:104–111` supplies the integer
lookup-table `ConvertToVector`. FireFlash does not depend on a general particle
helper from `effectcomp.cpp`; its pool is local animator state.

The shipped archive member `Imagery/Magic/fireflash.i3d` is 25,908 bytes,
SHA256 `7c7471fced7e8175a56327ad2bfa370579d2cc08400ce735e4fe63104244d526`.
It has 8 vertices, 4 faces, 2 materials, 2 textures and 2 subobjects:

- Object 0 `smoke`: 4 vertices, 2 triangles, material 0/texture 0.
- Object 1 `smoke01`: 4 vertices, 2 triangles, material 1/texture 1.
- Both textures are 64×64, single-frame RGB565; neither is an 8-frame strip.
- Both material diffuse RGBA values are white. Material 0 emissive RGB is 0;
  material 1 emissive RGB is 1. Full authored material descriptors are retained.

The earlier `F-FIREFLASH_TFireFlashAnimator.md` one-texture/eight-frame prose
uses an incorrect asset-header interpretation and must not guide this port.

## Implemented behavior

The previous bespoke single tinted billboard, 1.6-second lifetime and fabricated
grow/flash schedule are replaced by the literal 150-slot source controller.
First 75 slots are sphere particles; each receives three ordered random angles
and angular velocity 0.12. Column emission follows the source 4/7/7/2 schedule.
The sphere becomes visible at tick 26, expands 1→19 by tick 34, then tick 35
releases its slots and attempts 150 ring-origin **SMOKEY1** spawns. The source's
commented-out RING creation is not enabled. Ring radius grows 8 per later tick;
its 300-radius cutoff clears the entire pool at tick 71. Owner cleanup is requested
at tick 100, approximately 4.167 seconds, without automatic respawn.

Spawn order remains integer radius, integer angle, integer-vector lookup,
lifespan random draw, then velocity random draw. Ring radius is truncated from
`rsize` and is not clamped to 15. Its initial 19-radius lifespan expression uses
C++ signed integer division, giving 14..45 ticks; replacing the integer expression
with float arithmetic changes its negative-numerator endpoint. Startfade and
the 70% texture-selection threshold also use integer division. Newly spawned
particles are neither moved nor aged during their creation tick.

Rendering selects authored object 0 for late smoke and object 1 for early smoke
and sphere particles. Raw authored vertices, triangle indices, UVs, texture
handles and material descriptors are copied once from normal imagery loading;
there is no substitute quad, tint or point light. Per-particle transforms retain
source order: RotateX −120°, RotateZ −45°, RotateZ by cached negative aim angle,
uniform scale, then translation. Blend is `AdditiveStraight` (ONE/ONE), depth
tests with no writes, nearest retail texture sampling and source-compatible
screen-down triangle culling.

## Ownership and coordinate contracts

The typed `FireFlash` builder and animator hook attach only after the final
valid map identity and both textures are ready. Initialization guards reject
duplicates and other type IDs. `fireflash_reference` advances the simulation
once through the existing component update registration and submits separately.
The accumulator uses actual `1000.0/24.0` milliseconds with a boundary epsilon.
Default object/component destruction unregisters updates and releases normal
imagery ownership. Manual preview initializes with `false`, attaches no automatic
component and advances once per regular harness callback and draws from its world callback. Its wrapper does not retrigger.
No competing active FireFlash-specific builder/animator was found in compiled
source; old excluded source and registration comments remain historical evidence.

Source null-target rendering multiplies its local particle matrix by the owner's
matrix. Therefore editor-created particles follow actual owner movement instead
of rendering at world origin. The owner's current common-world transform already
contains the renderer's 1.5 raw-mesh Z stretch. Procedural particle translation is
legacy D3D Z: its local Z is converted by `REV_FIX_Z_VALUE/WORLD3D_Z_SCALE` before
that owner transform, keeping authored vertex stretch distinct from particle
height. This mirrors the separation used by the SetVortex cached-position bridge.

Optional spell initialization safely derives aim from the invoker/target and
captures the invoker's fighting target in a generation-checked weak reference.
That live target's position is read at each submit. Original `OBJ3D_ABSPOS`
bypasses the owner matrix and translates by the **raw** target position, including
raw target Z (original `3dimage.cpp:1624`, `effect.cpp:2319–2345`). This branch
retains that source translation and converts the complete absolute D3D Z back to
common world Z with `REV_FIX_Z_VALUE`; it does not apply owner facing or stretch.

Guarded natural hooks include initial target damage, runtime sound lookup and
spell expiry notification at tick 100. Initialization retries cannot repeat
damage. The snapshot animator invokes effect initialization again; exact retail
caller damage multiplicity has not been proved, and no spell/caster/target
integration acceptance is claimed. The current general `TEffect::GetAngle` is a
stub, so source aim calculation is localized to this effect.

## Validation and remaining gates

Offline checker:
`/Users/benjamincooley/RevenantRetailLab/research/fireflash-authored/check_source.py`.
Its `source-validation.json` records source/script hashes, 25 passing checks,
156 source-extracted lifespan/RNG-bound cases and 1,080 texture-switch cases.
It evaluates expressions taken from both actual source files using C++ signed
integer division; known clamp, floating-lifespan and floating-texture-threshold
mutations fail. It checks integer helper selection, random statement ordering,
initialization reset safety, live owner/target branches, asset selection, component
guards, 24Hz advancement and the one-shot preview. Scoped `git diff --check` passes.
These are arithmetic/structural source checks, not an executed full-pool oracle,
engine build or runtime capture. No build, render or guest control was performed
by this worker.

The recovered clean native lifecycle is retained at
`/Users/benjamincooley/RevenantRetailLab/captures/runs/sw-fps-fireflash-20261005`.
Root must build and compare the corrected candidate, verify exact ADDAT attachment,
movement and deletion, and inspect natural caller behavior separately.

## Software helper lighting correction

The initial Unlit FX path omitted the source software normal-light modulation.
The current implementation binds two immutable authored GPU meshes with explicit
per-owner retain/release references and submits `SHelperMeshSubmit` with
`retail_lighting=1`. It preserves raw authored normals, UVs and complete material
descriptors. This selects the recovered Blue RGB565 quantized vertex-light times
nearest wrapped texture path; it does not add a fabricated emissive/specular
contribution. The renderer's mode 0 behavior is unchanged by these effect edits.
The two object handles are cached by effect/imagery/object identity, and a
successful binding retains exactly once until owner destruction.

Simulation, source Render scale updates, object selection, procedural Z bridge
and live-target ABSPOS behavior are unchanged. The composed helper matrix is the
mathematical equivalent of the previous two-step CPU world transform, including
conversion of the complete absolute-target D3D Z. Original per-face projected
culling still checks the old two-step points. Both triangles must be nonnegative
before submitting the immutable object mesh; any mixed-sign grazing case fails
closed because the helper has no per-instance index-subrange API. No negative
source face is enabled by disabling culling.

Preview's regular callback advances only; `submit_world` draws after
`BeginTilePass`. Runtime component updates and submissions remain separate.
Helper rendering currently supports only normal debug mode, without invented
FX debug shader colors. There is no automatic respawn or extra simulation tick.

Latest source checks are
[check_source_software_helper.py](/Users/benjamincooley/RevenantRetailLab/research/fireflash-authored/check_source_software_helper.py)
and
[source-validation-software-helper.json](/Users/benjamincooley/RevenantRetailLab/research/fireflash-authored/source-validation-software-helper.json):
30 checks pass, retaining the original arithmetic/RNG/identity tests and adding
mesh ownership, world-callback timing, material copying and culling contracts.
The previous `source-validation.json` remains the pre-lighting-change record.

Shared offline geometry evidence:
[geometry-validation.json](/Users/benjamincooley/RevenantRetailLab/research/fire-helper-software-20261005/geometry-validation.json).
It compares actual shipped FF/Cone vertices over 5,040 composed-matrix cases and
20,160 vertex comparisons, including FF absolute targets, multiple origins and
facings, and negative/zero Cone scales. Maximum world difference is 0.0009765625,
within the declared float32 arithmetic bound; no tested source quad had mixed
face signs. This is a mathematical regression check, not rendered or lighting
acceptance. Historical Unlit pairs remain pre-correction diagnostics.

Root owns the new build, runtime capture and fresh A/B. Full software raster,
projection and initial-visible-phase equivalence remain unaccepted; natural
spell/target behavior remains a separate gate. No shared renderer, device,
Cure module or unrelated effect was changed by this worker.

## Root build, runtime and retail comparison

Build passed with binary `e0a92c7a37083aece4f6fe16e63681b0cfcbe6618bbe965a6ae8335c8e46b697`;16 particle and108 transform tests pass. The initial particle run from the build directory failed its relative asset lookup; repo-cwd rerun passes all16. Logs are under `research/fireflash-20261005/`.

`fireflash-natural-authored-20261005` passes five actual command checks, animation at original/moved poses and16 exact-ground natural-expiry tail frames. `fireflash-explicit-delete-authored-20261005` passes six checks and29 exact-ground deletion frames. Both attach one `fireflash_reference` component beside the normal animator, and retain binary/fixture/module/timing/log/image hashes. Owned teardown needed SIGTERM after complete frames; normal shutdown remains unproved. These synthetic-ground command cases do not exercise a real spell caller.

`sw-fps-fireflash-authored-20261005` pairs240 current port frames with the clean FPS-enabled native reference. Both outside-effect backgrounds are exact below the documented FPS band; the last90 port frames are exact ground with no respawn. At1.6seconds native shows a dim orange annulus and port a brighter filled near-white burst. Bounded source/pair review finds no new literal pool/arithmetic/subobject correction; material normal lighting and initial-visible phase remain unresolved. Preserve the source clock/data rather than fitting tint, size or timing. Visual fidelity and natural spell damage multiplicity/context remain open.

## Final explicit software-helper recapture

Final binary `e829e8dbd53c8d4d26f7c82f21159147137d0905181d2591bf9cde7670764870` compiles/executes the source software-helper normal/texture path. `research/fireflash-authored/source-validation-software-helper.json` records 30 checks plus 156 source lifespan and 1,080 texture-switch arithmetic cases. Natural/explicit-delete recaptures under `lab/captures/runtime-fixtures/fireflash-{natural,explicit-delete}-software-helper-20261005/command-capture/` pass 5/6 timeline rows and 16/29 exact-ground tails with visible animation and one `fireflash_reference`.

The fresh 240-frame `sw-fps-fireflash-software-helper-20261005` pair has background drift 0 on both sides. Root review: source software lighting corrects color, but at 1.6 seconds native shows an orange annulus while port has a softer filled cloud. Earlier brighter-white material evidence is historical; this remaining concrete mismatch still prevents visual credit. Actual SW blend/raster, initial-visible phase and stochastic coverage stay a bounded deferral without fitted tint/size/time. Natural spell damage multiplicity, target/lighting/audio and full original setting remain open. Evidence hashes are verified by the [final integrity audit](/Users/benjamincooley/RevenantRetailLab/research/ledger-reconciliation-20261005/cure-firecone-final-integrity.json).

The shared [geometry validation](/Users/benjamincooley/RevenantRetailLab/research/fire-helper-software-20261005/geometry-validation.json) passes 5,040 matrix cases and 20,160 vertex comparisons. Maximum absolute world error 0.0009765625 stays below `32*float32_epsilon*(1+max_abs_world_coordinate)`; this bounds CPU two-step versus composed GPU matrix arithmetic only. It does not certify rendered projection or software lighting. Current source hashes match that proof. Root passes 16 particle, 108 transform and 25 capture tests in the final build.

A bounded software RGB565 audit finds both additive and direct-store kernels. The existence of either does not establish the selected scene/effect caller path, so no blanket overwrite patch or visual pass is warranted.
