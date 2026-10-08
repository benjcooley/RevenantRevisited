# Retail authored `partsys` runtime

The production particle state is implemented in `src/authoredpartsys.h/.cpp`.
The immutable parser is `src/partsysdefinition.h/.cpp`; the animator and map
renderer bridge has separate ownership. This document records the state module
and executable evidence. It does not mark the water or geyser visuals accepted.

## Source identity and recovered bodies

The oracle is the original retail executable at
`/Users/benjamincooley/RevenantRetailLab/retail-cd/REVENANT/Revenant.exe`, SHA256
`28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.
The exported OOAnalyzer class `recon/classes/cls_0x5a3544.cpp` is incomplete:
it contains Close and Render, but omits the principal Initialize/Pulse bodies.
Those bodies are present and recoverable in the executable. The vtable and
registration establish the controller name, independently of class guesses.

- `0x403300`: registers the literal `partsys` name through `0x40d320`.
- `0x405e20`: factory; vtable `0x5a3544`.
- `0x403320`: Initialize, default particle parameters, controller parsing,
  maximum rate/lifespan scan, quality and pool allocation, hidden prototype.
- `0x403760`: Pulse, authored animation-frame tracks, emission accumulator,
  slot update/reuse, emitter order, spawn random calls and source transforms.
- `0x4031c0`: initializes a particle by copying 45 parameter words and assigning
  its position, velocity, lifespan, scale delta, and collision flag.
- `0x402a20`: particle lifespan tracks, gravity, rotations, friction, walk-height
  collision, bounce, integration, and death.
- `0x404270`: Render owner-state check and active-slot loop.
- `0x4026d0`: per-particle copied lit vertices, packed color, absolute position,
  z correction, scale, and render-time local rotation.
- `0x401390`: expression evaluation; `0x483300`: inclusive random helper;
  `0x58a840`: truncating x87 `_ftol`; `0x43ad80`: vector/matrix transform.
- `0x4042e0`: field parser, decoded independently by the parser agent.

Disassemblies, validation executables, scripts and JSON results are retained in
`/Users/benjamincooley/RevenantRetailLab/research/partsys/`. Existing complete
controller disassemblies are also in `research/water-variant-dispatch-20261004`.

## Covered authored data

The first production scope is the actual WaterFlft/WaterFrt/WaterClft/WaterCrt
`partsys` tags. These use an ordered six-emitter object list and the named
`#particle01` prototype, rather than synthesized geometry. The fall variants
have 45 PPS, speed 3:5, life 25:75, gravity .5, authored local rotation, RGB and
scale curves. The cap variants have 25 PPS, speed 7:8, life 25:50, zero gravity,
and different RGB/scale curves. Both use negative ten-percent bounce.

The state module receives resolved emitter origins, positions, scales and
original D3D row-vector matrices as immutable inputs. It does not load imagery,
mutate a shared mesh, allocate a GPU mesh per frame, create an owner, advance a
render clock, or kill the persistent map object. The bridge resolves prototype
geometry/material/UV and stable saved identity, advances on actual legacy
animator Pulse, and submits each sampled particle pose.

The supplied definitions also describe the first geyser particle controllers,
but this is not a complete geyser implementation: the steam asset has a later
`filename vapor` controller whose external definition is unresolved. Unsupported
trails, position/scale/alpha jitter, initial random-rotation fields, external
filenames and other grammar remain explicit parser failures. No ballistic or
tinted fallback is used. `relvel`/`charvel` curves are additionally rejected by
State initialization. Nonzero constant `relvel` requires the original object
zero initialization origin; the four water tags have constant `relvel=0`.

## Simulation contract

Initialize computes the maximum PPS and maximum lifespan, starts curve maxima
at zero, applies quality to the maximum rate (quality 2=.5, quality 1=.25), and
truncates `max_life * adjusted_max_pps * 0.0416666679084301f` to pool size.
For quality 0/3 the fall pool has 140 slots and the cap pool 52. Quality affects
the initialized capacity and stored constant PPS. Pulse evaluates a PPS curve
without reapplying that quality factor, but a constant PPS keeps the initialized
adjustment. The module preserves this unusual distinction.

Each Advance represents one original 24 Hz Pulse. It evaluates controller
tracks against the current authored animation frame, adds `PPS * float(1/24)`
to a stored float emission credit, and traverses slots in order. An active slot
updates first. A slot that dies during that update can respawn in the same
Pulse when credit is available. Every spawn attempt advances the ordered
emitter index and consumes one credit, including an emitter with a zero scale
component that prevents creation. Credit is retained when the pool is full.

Spawn samples speed first, then the six initial-rotation ranges (zero in the
supported subset), two shape angles, a shape radius, optional cube/square
coordinates, lifespan, spread, azimuth, and optional scale variation. The
selected emitter matrix transforms the source shape and velocity. The original
owner-face operations and initial-rotation terms are retained literally; the
code does not replace them with a modern, intuitive spray formula. Relative
velocity and optional spell-invoker horizontal movement use stored prior
positions. Owner world translation is applied to the emitted position.

Particle tracks evaluate `age * 100 / max(1, lifespan)` before the age increment.
The update subtracts gravity from vertical velocity, applies the original
X/Y/Z velocity rotations, then subtracts friction independently from each
velocity component toward zero. Collision queries the old truncated particle
position; if walk height is above that position, the vertical velocity is
multiplied by a sampled bounce percentage. It does not clamp position to the
ground. Position then integrates velocity, age increments, and the particle
dies when age reaches lifespan. No offscreen condition kills the owner.

The recovered dispatch has a specific exception: lifespan curve type 5 falls
through Pulse's default branch. Initialize sets its upper endpoint to the
maximum authored key; its lower endpoint remains the parser's constant/default.
The state module retains that behavior instead of sampling the lifespan curve
at the current frame. Initial-velocity type 12 is evaluated normally.

Expression evaluation finds the first key strictly greater than the integer
parameter. A parameter before the first key returns the last key, as does a
parameter beyond the last key. Interpolation uses truncated key times and a
denominator of at least one. It is not silently changed to ordinary clamping.

## Rendering and RNG

Advance and SampleRender are separate. Retail genuinely samples local visual
rotation in Render, so a nonzero random local-rotation range affects the shared
random stream according to rendered particle count. SampleRender preserves
that behavior. There is no invented once-per-tick replacement. Consequently
unsynchronized retail/port render counts are an RNG comparison limitation.

The three local-rotation helper evaluations are not necessarily three RNG
consumptions: `0x483300` returns equal endpoints immediately, without calling
CRT rand, and swaps reversed endpoints before inclusive sampling. The callback
is invoked only for unequal endpoints. Render never advances particle age,
position, emission credit, or the authored animation frame.

The render helper ORs `0x140206c` into the prototype flags, uses copied lit
vertices and packed color, overrides uniform scale and sampled local rotation,
and submits absolute position. Ordinary authored-normal lighting is not applied
to the particle's supplied RGB. The module truncates RGB channels and
`alpha * 255` as the packed vertex color does. Its source-position z correction
is `z / (1.460000038146973f - z * .0033333334140479565f * .009999999776482582f)
* 1.037999987602234f`. Rendering must not multiply the authored emitter or
prototype animation transform again after applying this absolute particle pose.

## Validation and limits

`verify_retail.py` executes the original executable's Pulse, particle Initialize,
particle Update, expression helper where applicable, and x87 Transform in
Unicorn. Render tests execute `0x4026d0` through copied color and pose setup.
Only external bone-pose queries, walk-height queries and CRT random source are
isolated with matched deterministic inputs. Every run asserts return to the
expected sentinel. It compares the compiled production module rather than a
second transcription of its update equations.

The base fixture passed 19,800 field comparisons: four emitter shapes, five
owner facings (0/32/64/127/255), 30 ticks, with and without render sampling.
It compares full-pool RNG influence, emission credit, first-slot position,
velocity, alive/age state, plus sampled corrected position, rotations, packed
RGB/alpha and scale. Nonzero random local rotation verifies the render-time
random stream. Integer state and RNG are exact; float comparisons use a
2e-5 absolute bound. Extended translated/nonuniform-emitter validation has
independent parser-agent ownership.

`verify_water.py` separately constructs the original track-array layout for the
actual four shipped tags, six ordered emitters, 180 ticks and the complete
140/52-slot pools. It passed 15,120 field comparisons, including RGB, scale,
local rotation, lifespan and the next emitter index. Its result is retained in
`water-validation.json`. `verify_initialize.py` executes original Initialize
for 24 capacity/quality and stored-PPS cases. The sanitizer contract checks the
compiled module's matching capacity and first-Pulse credit, same-tick lifespan
one respawn, render/zero-range RNG semantics and fail-closed inherited velocity.

The repeatable final-source runner is
`/Users/benjamincooley/RevenantRetailLab/check_authored_partsys.py`; it compiles
both production-module drivers, runs the existing original-executable oracles,
asserts source hashes unchanged during validation, and writes
`research/partsys/source-validation-manifest.json`. Independent translated and
nonuniform emitter tests retain 352,800 comparisons under
`research/partsys-parser`, with integer/RNG state exact and the stated tolerance
of max(3e-5 absolute, eight binary32 ULPs). Their ten earlier stricter absolute
misses are retained; maximum accumulated position deviation was 3.811e-5.

On arm64 macOS `long double` is 64-bit double, while the original x87 arithmetic
retains 80-bit intermediates. Literal constants, order, float storage boundaries,
integer truncation and random-call roles are sourced; arbitrary numeric
bit-for-bit equivalence is not claimed. The executable oracle supplies bounded
numeric evidence. Original copied vertex packing may also have different
software 16-bit color paths; reference/device fidelity remains a separate gate.

Runtime captures, fixture lifecycle assertions, production binary/config hashes,
and visual retail comparisons belong to the root integration manifests. Earlier
captures retain their recorded binary/source provenance when subsequent guard
changes cause a rebuild. This state audit does not promote those captures to
full visual acceptance.

## Current Z-handoff runtime protection only

Currentbb0a four existing authoredwater controllers each pass150images/12lifecycle rows/sixemitters, capacities140/140/52/52 aftershared Zbridge. Saved identity/controller/MOVE/deletion protection remains; no newnative reference or visual row. [Shared final source/runtime hashes](PARTSYS_Z_DOMAIN_HANDOFF.md). Visible nativewater, exactprojection/cadence/originalsetting and fullacceptance remain open.
