# FireCone authored controller and runtime bridge

Current status, 2026-10-05: final source/helper build, own clean retail pair and 11 actual command lifecycle checks pass. Visual fidelity remains deferred; natural caster/target and full original setting remain unaccepted.

2026-10-05. The source correction is ready for the root-owned build, runtime
fixtures and A/B comparison. No visual or natural caller acceptance is claimed.

Exact retail type: `FireCone`, ID `0xab92cd01`, `Magic\FireCone.I3D`.
Preview: `TFireConeEffect_BESPOKE`. Runtime component: `firecone_reference`.
`dragonfire`/`0xad92bc1a` has a different controller; sharing this asset does
not establish coverage. The historical DragonFire preview returns unsupported
through the non-null asset-override guard.

## Mapping and asset evidence

Shipped `imagery.rvi:class.def:3406` binds FireCone to this asset and exact ID.
Row 3430 separately binds `dragonfire`. Retail string `FIRECONE` at `0x5e1164`
has builder/animator XREFs `0x4e9d30` and `0x4e9ee0`
([recon listing:107489](../../../recon/classes/_data.txt:107489)).
`DragonFire` at `0x5e1170` has different XREFs `0x4eabb0` and `0x4eac10`.
The FireCone vtable identifies Initialize `0x4e9f00`, Animate `0x4ea060`
and Render `0x4eaae0`
([vtable](../../../recon/classes/cls_0x5aacac__vftable_5aacac.cpp:13)).
Recovered Render calls three helper render methods under the additive bracket
([Render](../../../recon/classes/cls_0x5aacac.cpp:24)). This mapping supports
the custom FireCone controller, rather than a default asset-tag reconstruction.
Full retail Animate machine equivalence has not been established.

Archive member `Imagery/Magic/firecone.i3d` is 18,996 bytes, SHA256
`8683cc1a0921f4c0bb7ef8544e94e1e391b180caca0db14260e8538fba948802`.
It contains eight vertices, four faces, two objects, two materials, two textures
and **zero tags**. Object 0 `box01` uses material 0/texture 0; object 1 `box02`
uses material 1/texture 1. Both are authored XZ quads with X endpoints
±35.265701 and Z endpoints ±34.782608, Y0. Their full UV corners are
`(0,0),(1,0),(0,1),(1,1)` and faces `(2,0,3)/(1,3,0)`.
Both textures are single-frame 64×64 RGB565, with no authored alpha channel.
Both materials have white diffuse RGBA and zero emissive RGB. Full descriptors,
raw vertices, indices and textures are preserved through normal imagery loading.

The imagery header has a looping 100-frame `STILL` state (`0x2001`), but the
custom controller owns its own phase and particle lifetime; this is not a
100-frame one-shot or an animated atlas.

## Literal controller correction

Original source is
[effect.cpp:7113](/Users/benjamincooley/projects/Revenant/effect.cpp:7113)
through the FireCone Render, plus
[effectcomp.cpp:1020](/Users/benjamincooley/projects/Revenant/effectcomp.cpp:1020)
`TParticleSystem::Init/Animate/Render/Add`. The shared helper remains disabled
in the port; this implementation privately recovers its small required operations
without enabling or changing unrelated helpers.

The previous tinted single billboard, invented growth and fixed 1.8-second
lifetime are replaced by fire 80, smoke 80 and burst 100 pools. Add selects the
first free slot, copies source fields and sets life 0. Helper Animate first
reaps particles whose life reached their span; remaining particles increment
life, integrate position and multiply velocity by per-axis acceleration.
All three original pools enable movement. There is no replacement policy,
particle sorting, extra random draw or generalized effects migration.

Each simulation tick preserves the complete source sequence:

1. Advance existing smoke.
2. Promote expired fire into new smoke with lifespan 15.
3. Advance fire and burst.
4. Execute START, MID or BLAST emission and transition logic.
5. Apply fire/smoke velocity and scale adjustments, then burst shrink.
6. Kill only when emission is done and all three pools are empty.

The source thresholds transition to MID on tick 6, BLAST on tick 15 and done
on tick 33. START attempts 24 fire spawns and BLAST attempts 68; burst creation
is separately randomized. No fixed wall-clock kill substitutes for the drain.
Smoke's height-derived scale is clamped to zero; **fire's negative scale is
preserved**. Burst scale shrinks by 0.95. Source random-call order and bounds
are retained, including shared velocity/acceleration across each burst group.

Initialize raises the **actual owner Z by 100 exactly once**, after both authored
textures/meshes are ready and before attaching its runtime component. The clean
native reference similarly initializes a spawn at Z16 to owner Z116. Cached
facing follows the original owner-face conversion. Simulation uses a true
`1000.0/24.0` millisecond accumulator with a boundary epsilon, separate from draw
submission. Runtime uses one idempotently attached `firecone_reference` after
final identity; destruction follows normal object/component ownership.

## Rendering and coordinates

Draw order is fire object 0, smoke object 1, burst object 0. The helper sets
`OBJ3D_MATRIX`, selecting default authored vertices/materials, not an invented
copied vertex-color stream. Each particle retains source transforms:
particle RotateX/Y/Z in degrees, additional RotateX −90°, RotateZ −45°,
cached facing rotation, particle scale, then translation. Source
`fire.Render()/smoke.Render()/burst.Render()` use default `flicker=false`;
the flagged first blast particle therefore receives **no 1.5× flicker boost**.
Zero or negative scale is not discarded early.

Literal helper translation uses `FIX_Z_VALUE(particle.pos.z)`. The port's owner
matrix already stretches raw authored mesh Z by 1.5. Procedural translation is
converted separately with
`REV_FIX_Z_VALUE(FIX_Z_VALUE(localZ))/WORLD3D_Z_SCALE` before that owner matrix,
keeping particle map-height offset distinct from raw mesh geometry. The owner
transform stays live, so later owner movement affects the particle field. No
fitted rotation, translation, scale, tint or clock correction is introduced.

Rendering preserves authored indices/UVs, nearest retail texture sampling,
source-compatible screen-down culling, additive ONE/ONE blend and depth testing
without writes. The initial Unlit FX submission omitted the source RGB565
normal-light modulation. The current submission uses two immutable authored
helper meshes and `retail_lighting=1`, preserving raw normals, texture UVs and
complete materials for Blue's quantized vertex-light times nearest texture
path. No fabricated emissive/specular term or fitted color is introduced.
Mode 0 renderer behavior is unchanged by this effect conversion. **Full rendered
software fidelity and shared projector equivalence remain open**, particularly
for owner pitch/roll and initial-visible phase.

## Preview, caller safeguards and validation

Manual preview uses `Initialize(false)` and has no automatic component or
respawn. Its regular wrapper advances once; a separate `submit_world` callback draws
after `BeginTilePass`, without a second simulation tick. Any non-default asset
override fails closed, including the historical DragonFire alias. Only exact
FireCone identity can attach this runtime implementation.

The source's guarded spell Pulse damages and burns the first eligible enemy
within 16 units of fire slot 0, retaining its unusual unoffset Z expression.
The port additionally requires the slot to be used, avoiding the original
uninitialized slot before first emission; it also validates the invoker's
character type. This is an explicit safety difference. Actual casting,
character damage/burn behavior and retail caller multiplicity require separate
verification. Runtime completion notifies an attached spell and flags owner
deletion. Retail Initialize also has a `napalm` audio XREF; audio parity is not
implemented or accepted in this correction.

Offline checker:
[check_source.py](/Users/benjamincooley/RevenantRetailLab/research/firecone-authored/check_source.py).
Results:
[source-validation.json](/Users/benjamincooley/RevenantRetailLab/research/firecone-authored/source-validation.json).
It passes 34 checks, comparing the **complete actual original Animate statement
and branch sequence** after explicit field/type/API/lifecycle adaptations,
including all 45 random expressions in order. Mutating velocity bounds or phase
thresholds fails that comparison. It also checks private helper expiration,
integration and allocation, actual source constants, phase boundaries, 92
attempted fire spawns, negative scale, render contracts, owner elevation,
identity/preview guards and 48 ticks over two seconds at 24/30/60/144 Hz.

This is source-statement equivalence plus structural/numeric contract validation,
not C++ execution, a full runtime state/RNG oracle, an engine build or image
acceptance. Scoped diff checks pass. No compiler, engine, render, recording or
guest-control process was started by this worker.

Reference retained at
[sw-fps-firecone-clean-20261005 manifest](/Users/benjamincooley/RevenantRetailLab/captures/runs/sw-fps-firecone-clean-20261005/manifest.json):
65 distinct viewport images, exact image cleanup and ground restoration without
camera reset. Root must now build, test actual runtime creation/movement/drain
and deletion, and compare the corrected candidate against that reference.


## Software helper conversion verification

Each effect owner retains each successfully bound mesh exactly once and releases
both references during destruction. Immutable meshes are cached by effect,
imagery and object identity. Controller state, pool updates/RNG, owner elevation,
source FIX translation and all local transforms remain unchanged. Helper matrix
composition replaces only the previous two-step CPU vertex world transform.
Both authored source faces still receive the same projected sign test before
submission. A mixed-sign numerical grazing case fails closed as a whole particle;
the helper has no per-instance index range. Negative fire scales are preserved
and tested; culling is not globally disabled. Normal debug mode is the supported
helper path; no replacement debug colors are manufactured.

Latest checker:
[check_source_software_helper.py](/Users/benjamincooley/RevenantRetailLab/research/firecone-authored/check_source_software_helper.py),
with
[source-validation-software-helper.json](/Users/benjamincooley/RevenantRetailLab/research/firecone-authored/source-validation-software-helper.json).
39 checks pass, including the complete literal controller statement/RNG comparison
plus helper ownership, world-phase callback, authored material and culling checks.
The initial `source-validation.json` remains the pre-lighting-conversion record.

[geometry-validation.json](/Users/benjamincooley/RevenantRetailLab/research/fire-helper-software-20261005/geometry-validation.json)
covers 5,040 actual shipped FF/Cone matrix cases and 20,160 vertex comparisons.
It includes multiple origins/facings, FF target ABSPOS, and negative/zero Cone
scales. Maximum difference is 0.0009765625 world units, inside the declared
float32 bound. No tested old-source quad had mixed triangle signs. This verifies
geometry regression bounds, not GPU/shader execution or lighting acceptance.
Root must build and capture the converted effects, and regress Cure separately.
No renderer, CMake, Cure module or general particle helper was changed here.

## Final source-helper runtime and retail pair

Final binary `e829e8dbd53c8d4d26f7c82f21159147137d0905181d2591bf9cde7670764870` executes the two source RGB565 helper meshes. `research/firecone-authored/source-validation-software-helper.json` retains 39 passing controller/helper/source checks. Natural and explicit-delete fixtures under `lab/captures/runtime-fixtures/firecone-{natural,explicit-delete}-software-helper-20261005/command-capture/` pass 5/6 timeline rows, animation at original/moved poses and 14/29 exact-ground tail frames with one `firecone_reference`. These are actual null-spell map runtime regressions, not a real caster/target fixture.

`lab/captures/ab/sw-fps-firecone-software-helper-20261005/` pairs240 current port frames with its own clean native reference, both outside-effect backgrounds 0 below the FPS band, no position/scale/color/phase fit. Root review remains deferred: port has a brighter/denser white core than native orange flame/smoke scatter. Investigate literal software blend/raster, initial-visible phase and stochastic coverage; do not retune authored controller, color or geometry. Full retail Animate machine equivalence and natural caller/context remain separate. The [hash audit](/Users/benjamincooley/RevenantRetailLab/research/ledger-reconciliation-20261005/cure-firecone-final-integrity.json) verifies terminal manifests/frame/media/fixture provenance. Capture teardown still requires termination.

The shared [geometry validation](/Users/benjamincooley/RevenantRetailLab/research/fire-helper-software-20261005/geometry-validation.json) passes 5,040 matrix cases and 20,160 vertex comparisons. Maximum absolute world error 0.0009765625 stays below `32*float32_epsilon*(1+max_abs_world_coordinate)`; this bounds CPU two-step versus composed GPU matrix arithmetic only. It does not certify rendered projection or software lighting. Current source hashes match that proof. Root passes 16 particle, 108 transform and 25 capture tests in the final build.

A bounded software RGB565 audit finds both additive and direct-store kernels. The existence of either does not establish the selected scene/effect caller path, so no blanket overwrite patch or visual pass is warranted.
