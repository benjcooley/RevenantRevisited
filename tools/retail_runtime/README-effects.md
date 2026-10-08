# Fast retail effects A/B

The thin in-process runtime is the primary tool for VFX reference execution.
DOSBox-X with the emulated 3D device is reserved for targeted checks of
software-rendering issues, with guest ownership coordinated first. Routine
state, geometry, animation, motion and comparisons stay in this fast loop.
Existing guest recordings remain evidence. Unsupported fixtures stay queued
while the next easy effect moves forward.

The complete scope remains the 176 retail type rows in
[`EFFECT_BURNDOWN.json`](../../docs/vfx/EFFECT_BURNDOWN.json). As audited for this
work, 16 rows have a bounded visual appearance check and zero have every runtime
and context acceptance gate. These probes add retained evidence; they do not
silently mark any effect fully accepted or replace the ledger.

[`scenarios/effect_candidates.json`](scenarios/effect_candidates.json) records
the first easy fixtures and moving missile contract, addresses, shipped assets, original map
placements, callback boundaries and remaining acceptance gates. It is a
candidate catalogue, not a `run.py` operation scenario.

## Run the passing fixtures

Run from the repository root. The existing environment is
`/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python` and
contains Unicorn and Capstone. Both commands read the verified immutable image;
they do not modify or launch it as a Windows process.

```sh
/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python \
  tools/retail_runtime/flame_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output recon/retail_asm/runtime/effects/worker-flame --repeat 2

/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python \
  tools/retail_runtime/effect_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output recon/retail_asm/runtime/effects/worker-partsys/report.json --repeat 3
```

The executable SHA-256 must be
`28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.
Reports retain production source and compiled driver hashes. A source edit
during validation rejects the run. Warm reset restores the same guest memory,
clock, RNG, handles and virtual filesystem without OS startup.

### Flame: actual original software pixels

[`flame_probe.py`](flame_probe.py) runs retail Render `0x4e4f20`, its original x87
matrix/vertex operations, original camera/projection and textured software
dispatcher `0x56d960`. The shipped `Imagery/Magic/flame.i3d` supplies all four raw
vertices, original triangle indices and the unconverted 128×128 ARGB4444 atlas.
There is no replacement texture, guessed quad size or fitted camera. The
selected fixture camera is `(0,0,0)`, z distance 1925, a 96×96 viewport and an
identity owner world matrix.

The port side compiles the exact current `TFlameQuadComponent` and
`TFlipbookBillboardComponent` class bodies, actual expression VM and matrix
implementation. External fixture providers supply the same authored geometry,
owner matrix and time steps, and record the actual `SubmitFxQuad` output. Both
effect frontends feed the **same original software rasterizer**. This isolates
effect geometry, UV and timing decisions. It does not compare the Revisited
Metal backend or certify a natural torch map.

The retained first report is
[`flame-frontend-ab/manifest.json`](../../recon/retail_asm/runtime/effects/flame-frontend-ab/manifest.json).
Its 18 selected animation frames produce eight distinct images and repeat
exactly. All 18 original/port RGB565 images match byte for byte; the original UVs
match exactly and the largest local corner difference is about `1.89e-6`.
Initial setup took about 133 ms and a warm image pair about 27 ms on this Mac.
The output directory has `retail-00.png` through `retail-17.png`, corresponding
port images, generated driver source and compile log. Original Animate
`0x4e4ef0` is checked separately through repeated 18-frame wraps.

Base hierarchy refresh and extents tracking are excluded in this isolated
owner fixture. The original Scene blend request `(2,1)` is asserted; imagery
submission is captured, then original projection and raster execute. These
boundaries appear in the report. They are not silently successful Win32 stubs.

The original software renderer retains its own ARGB4444/RGB565 quantization,
alpha floor, edge and depth behavior. Known device differences are recorded
separately from effect state/geometry discrepancies. Do not fit production VFX
colors, scale, density or timing to compensate for these renderer limits, and
do not stop the effect queue waiting for a perfect software rasterizer.

### Partsys: original state against the production module

[`effect_probe.py`](effect_probe.py) loads once and runs retail Pulse `0x403760`,
spawn `0x4031c0`, particle Update `0x402a20` and x87 transform `0x43ad80`. Optional
Render `0x4026d0` runs through original pose/color setup and stops at `0x4028e2`
before submission. The module `src/authoredpartsys.cpp` is compiled directly for
the port side; no Python copy of its equations acts as the oracle.

The base fixture compares 19,800 fields across four emitter shapes, five owner
facings, 30 ticks, and with/without render-time RNG sampling. Integers and RNG
are exact; floats use the established `2e-5` absolute bound. Every case replays
from the checkpoint at least twice, including an exact complete-pool memory
hash. The retained
[`partsys-state-ab.json`](../../recon/retail_asm/runtime/effects/partsys-state-ab.json)
passes with zero errors; a warm 30-tick replay took about 85 ms. Timing is a
local measurement, not a full-game throughput promise.

This is a synthetic one-emitter controller with explicit zero ground, identity
emitter/imagery transforms and a matched MSVC RNG callback. First-slot fields,
aggregate RNG/emission credit and pose/color are compared to the port. The
entire pool is checked for exact replay, **not** every port field. Constants in
this base fixture do not call the curve evaluator. A single synthetic lit
vertex is enough for copied-color setup, but it is not a water mesh or rendered
pixel acceptance test.

Public controls are `configure(shape, face, render, seed, owner)`, `step(ticks)`,
`inspect()`, `checkpoint()` and `reset()`. The host command adapter uses these
methods to create, step and inspect bounded original-code effect fixtures
without editor console input. Missing general map/owner/camera functionality
must be reported as unsupported until an original implementation is connected.

The existing full water-tag, parser and initialize oracles remain useful leads
under `RevenantRetailLab/research/partsys/`; the catalogue records their actual
paths and provenance. Their shipped six-emitter geometry, curves and
140/52-slot pools are the next controller extension.

## Parallel workers

Every agent can read the same immutable retail executable simultaneously.
Each worker owns a separate `Runtime`, software surfaces, warm snapshot,
virtual filesystem, RNG and output directory. There is no shared guest desktop
or controller loan. Give workers distinct paths such as `worker-flame`,
`worker-partsys` and `worker-ripple`. Driver filenames are scoped to the probe's
output location. Never let workers overwrite each other's manifests or images.

Changes to original assembly or C hooks must use a named private executable
variant and preserve the byte-identical baseline. Validate and retain the
variant's actual SHA, base SHA and patch manifest. Current effect fixture
addresses are verified against the baseline; accepting a variant also requires
an explicit compatible-address contract rather than disabling the SHA check.

## Next easy work and acceptance

1. Retain the Flame frontend image proof, then compare the actual Revisited
   renderer output under a matching selected camera/fixture. Keep backend
   limitations distinct from effect geometry/UV regressions.
2. Connect shipped authored water prototypes and six emitter transforms to
   the passing partsys CPU fixture and original software raster. Reuse existing
   track-array layout evidence, without introducing a guessed spray.
3. Exercise Ripple's short `length=20` original no-splash branch first. Its
   original Animate is `0x4f08b0`; Render is `0x4f0c10`. Longer rings add heap,
   RNG and recursive children and can follow afterward.
4. Use retained real map records once those fixtures are stable. Base Flame
   has 784 shipped placements; the City level-41 torch pair is recorded in the
   catalogue. Caverns Drip `(8159,9591,102)` has parameters `(20,200,35)`, ambient
   12 and color `(155,155,210)`. Preserve those parameters and surrounding
   imagery/lights for the context gate.
5. Continue the full 176-row queue. Record CPU state, frontend rendering,
   actual port backend comparison, creation/removal, saved type identity and
   natural trigger/map context as separate gates. Hard-to-reproduce effects
   stay deferred while easier ones advance.

Independent seeds do not invalidate an appearance match. Exact field/RNG
comparisons use matched seeds; visual comparisons with different streams review
shape, geometry, cadence, color and distributions without tuning to individual
particle positions. Ripple's existing emitted-effect appearance acceptance
remains valid in its stated scope.

## Source, destination and moving effects

Effect scenarios must describe their real caller inputs when they have them:
source and target entity identities, positions, stats, facing, attachment/bone,
launch state and spell/combat parameters. A map Flame has no invented combat
target. A moving Fireball needs its source/destination and launch fixture;
rendering it once at a selected coordinate cannot certify its motion.

For moving or triggered effects, acceptance additionally requires original
Pulse/Animate execution over fixed ticks, full trajectory or attachment motion,
world collision queries, impact/child creation, terminal state and normal reaping.
Compare those events and positions against the port before tuning pixels. Record
owner/target motion and RNG consumption in the trace, and capture frames before
launch, in flight, at impact and after cleanup. A static frontend image proof is
only the stated rendering gate. The candidate schema retains these requirements
and marks unexercised source/target/movement/impact capabilities explicitly; it
does not imply that the current Flame/partsys fixture hosts a complete world.

## Moving missile proof and Fireball fix

[`projectile_probe.py`](projectile_probe.py) provides a working
`shared_missile_base` reference profile. It executes original shared missile
Pulse `0x510220`, original Move `0x470920`, endpoint aiming `0x4df070`, vector and
distance helpers and kill request `0x4defe0`. The original table-building
subrange `0x41e2de` through `0x41e535` initializes the original lookup inputs once.
Then scenarios reset and run in milliseconds, with no OS boot. Actor source and
destination are required and distinct. Source/target setters update actor inputs
only; original Move computes the missile's position and 16.16 accumulators.

The public reference API is `configure(source, target, speed, target_hp, enemy,
wall_x, ground_height, launch_ready, animator_present)`, `step(ticks)`, `inspect()`,
`checkpoint()`, `reset()`, `set_source(position)` and `set_target(position, ...)`.
Typed controls expose this specific validated profile. They do not turn every
unknown projectile into a fake moving marker. Named private variants require
`--build <build.json>` and the explicit image/layout contract; no SHA bypass is
allowed.

The first strict comparison against the old Fireball preview body found
**876 state/position mismatches**. It deliberately slowed speed to 40%, discarded
fractional movement each tick and omitted character and world collision. Its
normal runtime Pulse only called the effect base, and its constructor did not
call Initialize. This evidence is retained in
[`projectile-current-ab/manifest.json`](../../recon/retail_asm/runtime/effects/projectile-current-ab/manifest.json).

Production [`src/missilestate.cpp`](../../src/missilestate.cpp) now owns the
original fixed-point movement and launch/range/impact/kill machine. Both the real
Fireball runtime Pulse and preview use this module at full speed. Runtime queries
actual map walk heights and nearby living/enemy characters; preview explicitly
selects its empty-character plane. Normal object state follows launch/fly/explode,
and the generic map mover cannot integrate the same missile a second time.
Fireball's actual object factory and constructor are active, endpoints can be
set before launch, and its preview clock is 24 Hz rather than truncated 41 ms.

The expanded comparison
[`projectile-expanded-ab/manifest.json`](../../recon/retail_asm/runtime/effects/projectile-expanded-ab/manifest.json)
passes **21,615 exact integer fields across 15 scenarios / 1,441 ticks**, including
positive/negative/diagonal motion, fractional accumulators, five additional
speeds, live/dead/friendly targets, range expiry and a selected wall. Each
original trace repeats exactly from its warm snapshot. It compares position,
velocity, accumulators, flags, range, state, facing and kill requests. Actual
retail leaf Initialize `0x510bd0` also verifies speed `8*65536`, ready status 1 and
initial range 32768; default speed is not inferred from a chosen test input.

The compiled port uses its actual object.cpp facing/vector/distance helper
bodies with identical original-initialized lookup inputs. Full modern lookup
initialization is a separate gate. External ground topology, character queries,
health and enmity are explicit matched inputs; general effect scripts, damage,
full Fireball animation and map reaping are excluded from this CPU oracle.

The integrated native runtime smoke uses the real class factory, constructor,
normal virtual Pulse and loaded-map walk heights. It passes with 18 distinct
positions, a world-edge impact, visual draining and kill request at tick 39;
normal object state matches the controller. It uses neither console commands nor
the preview tick helper. See
[`native-runtime-manifest.json`](../../recon/retail_asm/runtime/effects/projectile-expanded-ab/native-runtime-manifest.json).
The object is owned by the diagnostic; map reaper behavior, damage, full retail
Fireball visual parity and natural spell/combat casting remain open.

```sh
/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python \
  tools/retail_runtime/projectile_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output recon/retail_asm/runtime/effects/worker-projectile
```

The native smoke is `Revenant --headless --test=fireball-runtime` with an explicit
loaded module and `--scene-camera`. It rejects a missing/nonzero walk-height
floor rather than synthesizing one. The retained smoke uses FontRuntimeLab at
`110,10000,10000,16` in its existing private fixture data root. Its process
produced PASS but stalled after the quit request; only that owned process was
terminated. Normal application shutdown remains a separately recorded gap.

## Fireball head/glow geometry and moving pixels

[`fireball_head_probe.py`](fireball_head_probe.py) extends the moving missile
fixture with actual retail head `0x511dd0` and glow `0x511fa0` Render kernels in
the **same Runtime**. Original Pulse/Move computes every position from supplied
source `(0,0,128)` and destination `(1000,0,128)`; no host trajectory calculation
or per-tick position forcing occurs. Render frame, integer spin, scale and glow
are selected upstream pose inputs. This isolates head/glow rendering without
pretending to reproduce the full burst/spark animator.

The shipped `Imagery/Magic/newfireball.i3d` supplies its raw four `box01` vertices
at `0x270`, original indices at `0xf70` and 256×256 ARGB4444 atlas at `0x1514`.
Its material is opaque white diffuse, zero emissive; original copied vertices
are white. Original matrix composition, vertex transformation, camera/projection
and textured raster execute unchanged. Four face inputs and twelve moving
ticks give 48 rendered comparisons, including visible movement in every case.

The retained
[`before-canonical` report](../../recon/retail_asm/runtime/effects/fireball-head-before-canonical/manifest.json)
found **1,536 geometry/UV mismatches and 333,101 differing RGB565 pixels**. The
actual old `SubmitBillboards` body emitted a guessed 192-unit WorldXY square,
used aim instead of the original owner face, discarded the original tilt and
swapped UV vertices 1/2. Its descriptors were expanded using the documented
WorldXY shader contract for this diagnostic; this was not a Metal measurement.

Production [`fireballquad.cpp`](../../src/fireballquad.cpp) now preserves the
original authored corners, integer spin/facing conversion, matrix order and UV
mapping. The actual head/glow submission uses the existing `SubmitFxQuad` queue;
there is no new rasterizer or fitted size. Asset binding checks the original
quad topology and obtains vertices and texture from current imagery.

The
[`after` report](../../recon/retail_asm/runtime/effects/fireball-head-after/manifest.json)
passes all **1,920 position/UV comparisons** within the recorded float bound and
all **48 image pairs match byte for byte** through the common original software
raster. Every original moving image repeats exactly after a warm reset. Selected
raw PNG pairs are retained alongside generated production submission code.
The integrated native preview also loads the actual asset and produces changing
frames; that is a rendering smoke, not an original/Metal pixel sign-off.

```sh
/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python \
  tools/retail_runtime/fireball_head_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output recon/retail_asm/runtime/effects/worker-fireball-head
```

The owner matrix is explicitly selected model-Z stretch 1.5 plus the original
missile's translation; face tests isolate the kernel independently of owner
rotation. Natural owner hierarchy, full original animation, trail/burst/spark
geometry and cross-pipeline ordering, damage, and actual Metal A/B remain open.
The shared-raster head/glow result must not mark the complete Fireball visual
or its natural spell/combat lifecycle accepted.

## Fireball trail through impact and drain

[`fireball_trail_probe.py`](fireball_trail_probe.py) adds a substantial trail
render gate. Original missile Pulse/Move runs 72 ticks, reaches its real range
impact at tick 61 and stops. Original Animate's common prefix
`0x510e40..0x511179` performs the actual ten-slot copy/decay, owner translation,
glow RNG, frame advance and integer spin. The original state-history commit is
also executed. Spark callbacks are explicitly isolated without RNG; the
state-specific burst/ring choreography is excluded. A steady head scale `.6`
is an upstream fixture input, not a claimed full original animator lifecycle.

Original trail Render `0x511950` submits all historic glow cards back-to-front,
then all historic body cards. It excludes slot zero and stops history at the
newest repeated position. The previous port drew one guessed WorldXY pass,
included its current head as an extra trail sprite and left ten ghost slots
after movement stopped. The retained
[`visible-before` report](../../recon/retail_asm/runtime/effects/fireball-trail-visible-before/manifest.json)
shows **1,671 geometry/count failures and 97,626 differing RGB565 pixels**.

Production `SubmitAuthoredTrail` now uses the original two passes, history
selection and relative historical positions through the same authored quad
helper. Its head/glow/trail entries share the quad queue in that order. Full
burst/spark ordering and visuals remain open.

The
[`visible-after` report](../../recon/retail_asm/runtime/effects/fireball-trail-visible-after/manifest.json)
passes **2,920 geometry/UV checks and all 14 image pairs**. A fixed 960×540
viewport and camera `(240,0,128)` keep the genuine trajectory and impact visible;
the missile is never relocated. After impact, original trail submissions drain
through 16, 10 and two visible cards to zero at tick 70. Original pixels decline
from 8,852 at impact to 463 at tick 69, then zero. Warm reset repeats all sampled
states and images exactly.

Float positions use the established authored-transform bound
`max(3e-5 absolute, eight binary32 ULPs)`; UVs use `1e-7`. Four earlier strict
absolute misses were precisely one ULP at world X≈380–430 and are retained in
`fireball-trail-after/strict_absolute_manifest.json`. This accounts for original
x87/native arithmetic; no effect dimensions or coordinates were fitted.

The earlier rendering gate supplies original trail state. The next generated
state proof below removes that input dependency for the bounded head/history
stage. Shared spark RNG consumption, original burst/ring animation, damage
and Metal/backend acceptance remain separate unfinished gates.

### Fireball generated head/history state and moving pixels

[`fireball_core_probe.py`](fireball_core_probe.py) compares the exact production
head/history helper in [`fireballcore.h`](../../src/fireballcore.h), now called
by normal `TFireBallEffect::StepAnimate`, with original
`0x510e40..0x511179`. Original missile Pulse/Move supplies each owner pose;
the candidate helper generates its own ten history slots and head animation.
The comparison covers 472 ticks across three explicit MSVC seeds, frame skip
and wrap, integer rotation wrap, range impact and history drain. All 36,344
card-state fields normalize to identical binary32 values; the other 472 checks
compare RNG state exactly.

Fourteen selected software pairs match exactly using generated candidate state.
In-flight images include the head, glow and trail. After impact they include
trail only, matching original render dispatch. The native integrated factory/
normal Pulse test also passes with real loaded-map queries and exits normally.
The production change makes rotation use the original integer signed remainder;
other head/history equations were retained when sharing this stage.

```sh
python tools/retail_runtime/fireball_core_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output recon/retail_asm/runtime/effects/fireball-core-ab
```

[Retained report](../../recon/retail_asm/runtime/effects/fireball-core-ab/manifest.json)
and [native smoke](../../recon/retail_asm/runtime/effects/fireball-core-ab/native-private/manifest.json)
retain source/binary provenance. The isolated spark callbacks consume no RNG
on either path; the chosen stream tests the head/history stage's random draw
contract, not the complete native game's CRT or shared spark draw order. Head
scale is selected at .6, excluding state-specific growth, burst, ring and damage.
The larger authored glow card uses an explicit 5M instruction budget in the
unchanged original software raster. This remains a scoped stage proof and
shared-raster frontend comparison, not complete Fireball or Metal acceptance.

### Fireball shared spark state and random ordering

[`fireball_spark_probe.py`](fireball_spark_probe.py) replaces the isolated spark
callbacks with retail's actual `Set 0x50ad50`, `Animate 0x50adb0` and
`Create 0x50af40`, reached through the original `0x5b4308` vtable. Empty backing
storage has the recovered 40-particle capacity and 72-byte retail stride.
Original missile Pulse/Move drives owner motion. Three scenarios cover flight,
a three-tick launch-readiness wait, diagonal travel, range impact and pool drain.

The [retained before producer](../../recon/retail_asm/runtime/effects/fireball-spark-before-corrected/manifest.json)
has 100,389 mismatches. The port used per-hole spawning, continuous velocity
samples, lifetime-based visual flicker and early particle expiry; it omitted
flicker random draws before head animation. Normal `StepAnimate` now calls
[`fireballspark.h`](../../src/fireballspark.h): desired-minus-live creation
attempts, integer velocity samples, spawn/update flicker draws, expiry at
`life < 0` and final scale/motion integration on the expiry tick.

The [after report](../../recon/retail_asm/runtime/effects/fireball-spark-after/manifest.json)
passes 138,240 strict checks across 288 ticks, including all 40 pool slots,
head/history fields and RNG state. All 11,222 ordered `RandomRange` calls match,
including equal-endpoint calls that consume no CRT random value. All 288
original ticks replay exactly. Fourteen head/glow/trail software image pairs
also match using candidate-generated state with the real shared spark random
consumption. Spark particles themselves remain excluded from those images;
their authored geometry/material/backend is a separate gate.

```sh
python tools/retail_runtime/fireball_spark_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe --pixels \
  --output recon/retail_asm/runtime/effects/fireball-spark-after
```

The producer preserves the earlier .6 steady-head and explicit MSVC-stream
contract. State-specific burst/ring generation and their random calls remain
excluded on both paths. This closes shared spark ordering within the tested
Animate prefix, not the complete effect's random stream or global native CRT
sequence. Natural casting, growth, burst/ring, damage, normal map reaping and
Metal acceptance remain open. An earlier diagnostic used `firsttime +0x4a4`
under the label `explode`; its corrected original field is `+0x360`, and the
pinned old producer was rerun for the causal before report linked above.

### Fireball authored spark rendering and coordinate boundary

[`fireball_spark_render_probe.py`](fireball_spark_render_probe.py) executes
original `0x50b2b0`, including its real software-path blend Save/Set/Restore,
then retains each raw D3D matrix and transformed authored vertex. Shipped
`box02` vertices start at `0x2f0`, local indices at `0xf7c`, and its 64×64
ARGB4444 texture at `0x21518`. The authored UVs include the original orientation
and near-edge float values; they are not replaced with a generic 0–1 rectangle.
The selected material is white diffuse with zero RGB emissive, in an explicit
unlit fixture environment.

The [before producer](../../recon/retail_asm/runtime/effects/fireball-spark-render-before/manifest.json)
had 7,180 packet/UV mismatches and 24,460 differing bridge-relative pixels.
Production now loads the actual `box02` corners and submits fixed native tilt
`Rz(-π/3), Rx(-π/4), Ry(0)`, sampled flicker scale and absolute position.
`OBJ3D_ABSPOS` bypasses the owner's matrix. The binary's nonlinear `FIX_Z`
translation, including its `1.038` factor, is retained; the source snapshot's
simplified macro omits those terms.

The [after proof](../../recon/retail_asm/runtime/effects/fireball-spark-render-after/manifest.json)
passes 4,308 **raw native D3D** coordinate checks before any coordinate bridge,
and 7,180 common-world corner/UV checks. Seventeen selected moving/impact/drain
software images match and repeat exactly. Candidate state is generated by the
compiled, verified spark/head/history prefix; the missile is never repositioned
per tick. Float positions retain the established absolute/eight-ULP bound.

```sh
python tools/retail_runtime/fireball_spark_render_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output recon/retail_asm/runtime/effects/fireball-spark-render-after
```

The bridge is an explicit renderer boundary. `object.h` defines
`REV_FIX_Z_VALUE(z3d) = z3d × 1.46`; `T3DAnimator::GetObjectMapPos` applies it to
the complete transformed D3D point. The quad renderer forwards supplied
`world_pos` corners to the common map/world FX projection, whose isometric Y
term uses world Z directly. Accordingly the absolute spark converts its final
D3D Z by ×1.46, instead of concatenating an owner matrix or adding its Z stretch.
The raw native checks are independent of this bridge. The software images use
the same declared bridge/camera environment on both sides and **do not prove
independent original device or modern GPU parity**.

The [pinned native capture](../../recon/retail_asm/runtime/effects/fireball-spark-render-after/native-private/capture/manifest.json)
loads actual assets and produces six distinct moving Metal frames with normal
shutdown; it is an integration smoke test. Growth, burst/ring, damage, natural
casting/map reaping and full backend acceptance remain open.

### YFireBall shared moving preview

[`yfireball_probe.py`](yfireball_probe.py) executes actual Y leaf Initialize
`0x513100`, common Animate prefix `0x5133c0`, head `0x514350`, glow `0x514520`
and trail `0x513ed0`, with the actual green `Yfireball.I3D` asset. The original
Y registration and instruction clone evidence remain in the
[candidate provenance](../../recon/retail_asm/runtime/effects/yfireball-candidate.json).
Its `0xdc` header uses the three-level state/frame OFFSET pointer chain: head vertices
`0x294`, spark vertices `0x314`, atlas `0x1610`, spark texture `0x21614`.

The old stationary lifetime billboard is retained as
[before source evidence](../../recon/retail_asm/runtime/effects/yfireball-before-source/manifest.json),
not an executed before comparison. The Y preview now derives canonical
`TFireBallEffect`, preserving its actual projectile motion and verified shared
frontend. Base `SpawnForTest` keeps its public API; a protected asset/factory
adapter also loads the Y asset. The Y wrapper delegates `TickAndSubmit`, so the
old separate 41ms clock, guessed24-unit billboard and age fade are removed.
The green spell light `180,255,80` is an explicit preview input from `spell.def`;
its influence on a real map remains a separate gate.

The [after report](../../recon/retail_asm/runtime/effects/yfireball-frontend-ab/manifest.json)
passes 46,080 strict state checks and 3,695 ordered random calls across 96 original
moving ticks. It also passes 10,941 texture/corner/UV checks, 17 combined
spark/head/glow/trail software pairs and 17 exact original warm packet/image
replays. Source/destination are distinct; actual original missile Pulse/Move
drives the reference, and compiled shared production code generates candidate
state. The original Y leaf defaults independently confirm speed 8, range 32768,
launch status 1 and damage-once flag 1.

```sh
python tools/retail_runtime/yfireball_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output recon/retail_asm/runtime/effects/yfireball-frontend-ab
```

[Pinned native smoke](../../recon/retail_asm/runtime/effects/yfireball-frontend-ab/native-private/capture/manifest.json)
loads the real green asset and captures six distinct moving GPU frames with
normal shutdown. The replay uses the same steady .6-head, selected owner
matrix, white/unlit environment and native-ABS-to-common-Z boundary as the
FireBall fixtures; shared-raster images are not independent device/Metal
parity. Natural Yhagoro binding/hit logic, map builder/casting, growth,
burst/ring/damage and normal map reaping remain open. This completes a bounded
moving preview correction, not the whole Y effect.

### Streamer: full one-shot spiral state and four authored textures

[`streamer_probe.py`](streamer_probe.py) runs original Initialize/Animate/Render,
its parametric generator and last-free-slot insertion through ticks0–103. It
compiles actual initialization state and shared production preview/map methods,
checking both ownership modes and normal kill at101. All active slot and stream
float32 fields match after a scoped precision correction; eleven selected
software image/depth pairs repeat twice and match exactly. Four authored quads,
indices/UVs and independent RGB565 textures come from the shipped asset.

Run with the verified rebuilt executable and `--output` directory as shown in
[the retained instructions](../../recon/retail_asm/runtime/effects/streamer-frontend-ab/README.md).
The failed arithmetic run is retained alongside the current
[manifest](../../recon/retail_asm/runtime/effects/streamer-frontend-ab/manifest.json).
This is a controlled identity-owner/face-zero/no-spell frontend comparison using
the original rasterizer. Natural lighting/culling, map binding, arbitrary owner
poses, optional audio/poison cure and modern Metal pixels remain separate gates.

### Mist: fifty continuous drops and exact native random replay

[`mist_probe.py`](mist_probe.py) checks original Initialize/Animate/Render against
compiled current shared map/preview production methods. Fifty drops match in all
float32 position/velocity fields and dead flags through ticks 0–96, including
repeated death/respawn and 1,380 strict native random-call results. Eight selected
geometry/UV/software image/depth pairs repeat twice and match exactly. No
production change was needed. Exact shipped ARGB4444 geometry/texture and the
original projection/raster are used.

[Retained instructions](../../recon/retail_asm/runtime/effects/mist-frontend-ab/README.md)
and [manifest](../../recon/retail_asm/runtime/effects/mist-frontend-ab/manifest.json)
record the selected identity-owner/face-zero camera and API boundaries. Natural
lighting/map binding, arbitrary face/world poses, culling and Metal remain separate.

### Pixie: exact swarm state and authored object selection

[`pixie_probe.py`](pixie_probe.py) runs original registered Initialize/Animate/Render
against compiled shared production methods in both preview and map ownership
modes. The stationary25-particle context has explicitly empty nearby-character
queries. All state fields and object selections agree through ticks0–96, consuming
5,000 strict native RNG results. Eight selected object/UV/geometry/software
image/depth pairs repeat twice and match exactly. Scoped production arithmetic
corrections recover native59–79 scale initialization and retained reciprocal/
scale-jitter products; failed source/log evidence is retained.

[Instructions](../../recon/retail_asm/runtime/effects/pixie-frontend-ab/README.md)
and [report](../../recon/retail_asm/runtime/effects/pixie-frontend-ab/manifest.json)
record the exact two shipped ARGB4444 textures and native/API provenance.
Character fleeing/return behavior, map binding, arbitrary owner/face poses,
culling/lighting and Metal remain separate gates.

### Literal Waterfall: original warmup and one hundred authored drops

[`waterfall_probe.py`](waterfall_probe.py) runs original initialization, all 100
warmup ticks, Animate and Render versus compiled actual shared map/preview
production methods. All 100 drop position/velocity/scale/time fields match through
97 samples, with 1,566 strict native RNG results and 482 landings/respawns. Eight
selected geometry/UV/software image/depth pairs repeat twice and match exactly.
No production change was needed. This is exact type 0xa907dabf and Misc/Water.i3d,
separate from the controller-driven WFall/WCap variants.

[Instructions](../../recon/retail_asm/runtime/effects/waterfall-frontend-ab/README.md)
and [report](../../recon/retail_asm/runtime/effects/waterfall-frontend-ab/manifest.json)
record white input vertices, retail shared-target triangulation and the explicitly
omitted unused copied-color DoLighting callbacks. Native normal/device lighting,
map binding, arbitrary owner/face poses, modern triangulation/culling/blending and
Metal remain separate gates.


The original thin state reader covered only 50 native slots; its initial full-pool
state counts are withdrawn. The corrected reader and before-zip cardinality
assertions now compare all 100 native/compiled-port slots. The complete 97-sample
rerun passes 9,700 records/87,300 float32 fields and two full state/RNG/pixel replays;
all earlier color/depth hashes are unchanged. The current report supersedes
retained `before-full-pool-*` partial-state evidence.

### Drip: falling-head state and emitted Ripple requests

[`drip_probe.py`](drip_probe.py) compares the actual shared emitter producer with
native registered Initialize/Animate/Render. Three explicit parameter sets each
cover ticks 0–192: 579 samples, all valid six-field pos/vel state, wait/dead state,
112 physics range calls and eleven emitted child requests agree and repeat twice.
Fifty-two head-only geometry/UV/software image/depth pairs match. The compiled
actual WaterRandom helper exposed and now fixes equal-endpoint raw-RNG consumption;
retail invokes no CRT draw for random(0,0). The shared no-splash Ripple regression
was refreshed: 57 cases pass with all old color/depth hashes unchanged.

[Instructions](../../recon/retail_asm/runtime/effects/drip-frontend-ab/README.md)
and [report](../../recon/retail_asm/runtime/effects/drip-frontend-ab/manifest.json)
keep native audio RNG/global coupling and child factory/initialization/lifetime/
composite rendering as explicit open gates. This is identity-owner emitter/head
and request evidence, not a full Drip–Ripple map/lighting/modern-GPU comparison.

### FireSwarm: one-shot cylinder and original triangle-size rejection

[`fireswarm_probe.py`](fireswarm_probe.py) checks native registered controller and
compiled shared map/preview producer through ticks 0-78: all 237 scale/yaw fields
and expiry at 76 agree. Exact tube01 geometry/indices/UVs and texture 1 produce
twelve matching image/depth pairs, with two exact warm state/pixel repeats. Five
later samples are visibly nonempty; no production fix was needed.

[Instructions](../../recon/retail_asm/runtime/effects/fireswarm-frontend-ab/README.md)
and [report](../../recon/retail_asm/runtime/effects/fireswarm-frontend-ab/manifest.json)
retain the early-appearance limit: original software skips triangle edges over
640px X/480px Y despite a larger canvas. Initial tall-cylinder samples are empty;
[the diagnostic](../../recon/retail_asm/runtime/effects/fireswarm-frontend-ab/software-span-limit.json)
shows Z testing is not the cause. Do not fit VFX geometry/camera to hide this.
Early real-device appearance, device culling, lighting/blending, map binding and
modern GPU remain separate gates.
