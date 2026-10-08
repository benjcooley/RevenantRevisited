# Cure: bounded retail mapping and port plan

## Executed native null-spell proof — 2026-10-07

The [current thin comparison](../../../recon/retail_asm/runtime/effects/cure-nullspell-frontend-ab/README.md)
now executes the original Initialize/Animate/Render rather than relying only
on the reconstructed source oracle. All 269,535 fields across 151 samples and
85 records match byte for byte, with natural owner kill at tick 132. Fourteen
sampled image/depth pairs match and replay. Double-trig coefficient stores and
the unrounded FST/FCOS phase were corrected; no lifetime, geometry or phase
fitting was applied. Controlled light/culling/raster overwrite, real additive
lighting/sorting/backend and real spell/target/poison gates remain explicit.

Status: executable/asset audit and bounded null-spell module implemented;
integration, state-check results, runtime and visual acceptance have separate
gates. The clean software reference is
`/Users/benjamincooley/RevenantRetailLab/captures/runs/sw-fps-cure-20261005`.
Its image count is capture evidence, not a simulation tick-count oracle.

## Proven identity

Literal Cure (`0x152dafef`, `Magic\\Cure.I3D`) uses a bespoke retail animator.
It is not a later authored `partsys`/Comet controller substitution.
Original retail executable SHA256:
`28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.

- Builder registration `0x4e0aa0` references literal Cure at `0x5e1078`.
- Animator registration `0x4e0c30` references the same string.
- Animator vtable `0x5a8f20` contains Initialize `0x4e0c50`, Animate
  `0x4e0d90`, Render `0x4e1580`, and RefreshZBuffer `0x4e16f0`.
- Effect initialization `0x4e0ac0` sets its first-time flag; effect Pulse is
  `0x4e0ad0` and ends by calling retail base-effect Pulse `0x4de800`.

These match the snapshot TCureEffect/TCureAnimator family in
`/Users/benjamincooley/projects/Revenant/effect.cpp:1292` onward. Complete
retail function instructions are present even though only the animator vtable,
not its full body, was exported in the reconstructed class files.

Shipped asset member `Imagery/Magic/cure.i3d` has SHA256
`e9f42e51042c832dcf2acc5964017663d4089bec109666c9ebd8711cf9560216`.
It is version 1, with 12 vertices, six triangles, three objects/materials and
three textures, one 100-frame state, and zero controller tags. Its old animation
keys are eight bytes per frame (not version-3 key records); each object's 100
keys is identical. The three meshes are authored 75 by 75 quads in the XZ
plane, with their original winding and UVs. All three textures are single-frame
64 by 64 RGB565 without an alpha mask. Object 0's texture supplies the blue
color; there is no need to synthesize a blue vertex tint.

## State and geometry

Retail Initialize clears 80 records at `0x100` and five records at `0x1b40`;
each record is 84 bytes, matching the snapshot CURE_PARTICLE layout. The five
balls are simulation pivots only. Render loops the 80 swirls, checks state 3
and positive size, and looks up the swirl's color object. The ball color random
helper is called with endpoints 0,0 at `0x4e0e46..5f`; it returns zero without
consuming CRT RNG. Swirls inherit that color. Therefore **only object 0 renders**;
objects 1/2 and guide balls must not become additional visible particles.

Render at `0x4e15ed` sets flags to `OBJ3D_MATRIX` (`0x100`), rather than copied
lit vertices. It clears the matrix, rotates X by float bits `0xc0060a92`,
rotates Z by float bits `0xbf490fdb`, rotates Z by the negative initialized
facing angle, then applies uniform swirl size and translates to swirl position.
The rotation values are approximately -2.09439516 and -.785398185 radians.
The facing conversion uses the original double constant 6.283185308 at
`0x5a3a68`, divided by 256. This is the original transform call order, not a
camera-facing billboard replacement. The mesh uses its authored material,
vertices, UVs and texture. Its source additive blend setup and ordinary vertex
path must be retained; a CPU gray/color override is not established by this
audit.

Particle positions are **local to the effect**. Simulation pivots start at
X/Y zero and Z -5/15/35/55/75. Ball positions add the rotated displacement with
vertical displacement halved and Z clamped to 10. Swirl positions add a full
rotated displacement around their ball. Keep this local simulation domain
separate from the live owner's world transform/position during Submit; do not
add map coordinates both in state and again in the render matrix. The facing
angle is initialized once from the effect's aiming angle.

## Timing, RNG and natural end

Animate increments the integer frame counter before traversal. It preserves
the five-ball array order, decrements positive start delays, and activates a
ball on the same tick its delay reaches zero. Ball zero has delay zero; the
other four delays are sampled inclusively from 0..15 in Initialize.

Activation draws three initial angles from 0..359, the fixed color helper,
an axis-pair choice from 0..2, then two angular speeds from 5..9. Retail folds
degrees to the float multiplier `.01745329238474369f` at `0x5a39f0`; use that
literal float and storage boundaries instead of inventing a new conversion.
Newly activated balls advance in the same tick. Their size grows by .1,
distance phase by .04; distance is main distance plus 10*cos(phase). Main
distance starts at 60 and decreases by 1.5 while above 2. On a later tick it is
clamped to 2 and marked lined up. The final ball changes all five to BALL2;
the exact order means later records can enter the second branch immediately.

BALL2 continues rotation, moves pivot Z toward 35 by .5, and shrinks size.
The snapshot's `pivot>30 || pivot<40` expression is also present in retail
`0x4e109d..10cb`: preserve its literal OR behavior, rather than fixing it to
AND. For finite pivots it selects shrink amount 1. Size is decremented while
above .1, then clamped on a subsequent tick; do not rewrite as an immediate
clamped decrement.

Before all balls line up, one inactive swirl is created per Animate. It draws
its assigned ball index, three angles, distance phase, axis-pair choice and two
speeds 5..10 in that order; fixed helper ranges do not consume RNG. The ball
index is sampled directly from `0..numactiveballs-1`; do not substitute a
compacted list of active ball records when delayed activations are out of order.
Newly
created swirls also advance immediately. They use the attached ball's size as
orbit distance and grow from .01 by .03 while below .5, then clamp on a later
tick. Once all balls line up, growth stops and one swirl record is marked dead
per tick. At kill index 80, retail takes the natural effect kill path
`0x4e1223..129d`. This depends on state progression; it is not a fitted fixed
duration or an authored 100-frame animation timeout. Render has no RNG calls.

Advance exactly once per actual 24 Hz legacy tick. Render/Submit must not
advance state; manual preview must not tick twice. Do not calibrate against the
number of native distinct images or stretch the state to the eight-second
video derivative.

## Safe null-spell path and full-scope caveat

Retail Initialize checks the spell at `0x4e0cb5..d15`. With no spell it sets
effect angle to zero explicitly, then initializes the full ball/swirl state.
The first-time effect Pulse checks spell and invoker before target access.
The natural end also guards spell, variant/target and spell cleanup before
access; its owner kill flags still execute with no spell. A real editor ADD
can therefore exercise the full visual lifecycle without a character/spell.

The spell branch is real behavior, not disposable code: first Pulse can select
a fighting/nearby enemy within 400 units, move the effect to it and mark poison;
the end branch can clear target poison and finish its spell. A visual component
attached to generic TEffect alone does not establish these gameplay semantics.
The current generic constructors initialize angle/spell deterministically, and
safe `OF_KILL|OF_PULSE` cleanup exists, but full Cure acceptance still requires
the separate real-spell path and source-backed target behavior. The pointer
provenance and nonzero cleanup predicate are now resolved as SpellData's
POISONCHANCE, not a variant field; see CURE_SPELL_DATA.md. Shipped CureP binds
Streamer rather than literal Cure, so those two scopes must remain distinct.

## Recommended bounded correction

Use a new `src/effects/cure.h/.cpp` module: fixed 80/5 records, explicit
Initialize/one-legacy-tick Advance/Submit, borrowed actual owner imagery, and a
typed animator/component registration restricted to `0x152dafef`. Start with
the proved null-spell editor path and one-shot manual preview (no retrigger).
Bind only the actual object-0 quad/material/texture for visible submission;
preserve original simulation/RNG/math/matrix order and owner coordinate split.
At natural completion call the existing safe owner kill lifecycle, never
delete the owner inside component update. Root owns CMake and preview-table
integration; no shared renderer/light/device changes are required for this
bounded state/geometry implementation.

Retain a separate gameplay gate for the custom effect first-time spell Pulse
and guarded target/spell cleanup. Validate deterministic per-tick state against
the recovered executable before visual comparison, then use the existing real
NewObject timeline to prove nonzero final identity, exactly one attachment,
changing animation, natural disappearance, and no forced DELETE. Finally pair
the single burst against the retained software reference with matched ground,
camera/ambient, original coordinates and documented unsynchronized RNG.

The initial audit performed no engine edits or runtime tests. The subsequent
authorized implementation is restricted to `src/effects/cure.h/.cpp`; its
integration contract and explicit open gates are recorded in CURE_RUNTIME.md.
This source association is not visual acceptance.
