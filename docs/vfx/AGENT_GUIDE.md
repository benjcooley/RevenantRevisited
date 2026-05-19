# VFX Agent Guide

Protocol for an agent picking up a slice of the VFX restoration. Designed so
multiple agents can work in parallel without stepping on each other.

Read [README.md](README.md) and [TEST_MODE.md](TEST_MODE.md) first.
[INVENTORY.md](INVENTORY.md) is your assignment board.

## 1. Before you start

Read, in order:

1. [README.md](README.md) — what we're doing and why.
2. [TEST_MODE.md](TEST_MODE.md) — what "done" looks like for any effect.
3. [../PARTICLE_EFFECTS.md](../PARTICLE_EFFECTS.md) — the engine architecture you are extending. **Do not invent a parallel architecture.**
4. [INVENTORY.md](INVENTORY.md) — pick an unclaimed row.

Spot-check the project-wide rules these touch:

- [../../CLAUDE.md](../../CLAUDE.md) — house rules.
- The user's standing feedback memories: no bodges; modern C++; preserve old code in `attic/` or `#ifdef UNUSED`; centralized rendering submission lives in the renderer class; renderer is game-agnostic; world-space invariant in deferred path; minimal includes; no raw threads.

## 2. Picking work

### 2.1 Claiming a row

1. Open [INVENTORY.md](INVENTORY.md).
2. Find an unclaimed row (`Owner: -`, `Status: not-started`) that is **unblocked** — its pipeline (FB / PE / SR / MP / VO / LS) exists per Phase 1.
3. Edit the row:
   - `Status: claimed`
   - `Owner: <your-handle-or-agent-id>`
   - `Notes: claimed <YYYY-MM-DD>; <one-line plan>`
4. Commit that change **before** writing any code, so other agents see the claim.

If your effect needs a pipeline that doesn't exist yet (e.g. you picked a strip
effect but the SR pipeline isn't done), put the row back to `not-started` and
either pick another row or claim the Phase-1 pipeline work itself.

### 2.2 Cluster claims

Some effects naturally cluster (`TFireBallEffect` + `TFireBallAnimator`;
`TBloodEffect` + `TBloodAnimator` + `TBloodSystem`). Claim them together,
update each row's `Owner` to the same handle, and note `clusters with: <other ids>`.

### 2.3 What not to claim

- **Discovery rows (D01–D06)** without resolving them first. Update the
  inventory row they map to (e.g. D04 → M05), don't create a duplicate.
- Engine-spine infrastructure rows (I01–I06) unless you are doing Phase 1 work.
  Those gate the whole catalogue and shouldn't be done concurrently with their
  consumers.

## 3. Doing the work

### 3.0 Forensics first (MANDATORY pre-step for every effect)

**Before writing any port code for an effect, complete the forensic
write-up.** This is a hard prerequisite — not optional. We don't ship
"plausible" behaviour and call it done; we ship behaviour grounded in
retail evidence, with explicit notes on anything we deliberately
deviated from.

The forensic pass for an effect produces this body of notes appended
to its INVENTORY row (or a separate `docs/vfx/forensics/<id>.md` file
if it grows beyond one paragraph):

1. **Retail constants** — every numeric the effect uses (counts,
   speeds, lifetimes, gravities, blend factors, atlas dims). Pull
   them from pre-release `src/effect{,comp}.cpp` macros AND
   cross-check the recon decomp if the class is mapped. Note
   anything that disagrees between pre-release source and retail
   recon — retail wins for behaviour per §3.1, but the divergence
   itself is information.
2. **Spawn / emit shape** — how many particles per trigger, what
   trigger semantics (one-shot? duration? continuous? loop?),
   initial direction (radial cone? caller-supplied vector?
   character-attached vector?), spread distribution, **and the
   emit anchor convention**:
   - Where in the trigger world does the effect ORIGINATE? At the
     spawn-call's exact (x, y, z), OR at the ground projection
     (z snapped to surface), OR at the character's feet, OR at
     the weapon tip, OR at a sub-object hand bone?
   - Many pulse effects (ground halo, heal aura, footstep ripple,
     teleport circle) emit from the **ground center of the target
     emitter point** — NOT from the spawn call's exact Z. Check
     the retail code for the snap-to-ground (or snap-to-water-
     surface, or snap-to-character-feet) convention.
   - Document the convention explicitly. A pulse that should emit
     at the player's feet but emits at the player's hand reads
     totally wrong even if the effect itself is otherwise correct.
3. **Per-frame integration** — gravity/drag/wind, alpha curve,
   scale curve, frame-cell curve, death condition (life-elapsed,
   ground-impact, owner-state).
4. **Render passes** — single-pass alpha? Two-pass alpha+additive
   overlay (TBloodSystem-style)? Lit or unlit? Per-frame texture
   swap? UV pick from atlas?
5. **Caller / trigger sites** — what game code spawns this
   effect and with what context. Hit resolution? Spell cast?
   Environment timer? Script event?
6. **Test-mode rig requirements** — what does the harness need to
   exercise this effect on the *real* code path? Not every effect
   is standalone-spawnable like a torch flame. Categories so far:
   - **Standalone** — `SpawnForTest(origin)` is enough (F01 flame,
     B01 blood, S01 lightning). No external state needed.
     Use `EVfxPreviewStyle::Static` / `Combat` / `SpellGround` /
     `Projectile` depending on the in-game cadence.
   - **Character-attached** — needs a character imagery loaded +
     TCharAnimator running so the effect's owner exists. Status
     overlays (TBurnEffect / TAuraEffect / TIcedEffect), aura
     glows, healing visuals. **Rig available** as of 2026-05-17:
     register with `EVfxPreviewStyle::CharacterIdle` and set
     `SEffect.anchor` to one of CharacterRoot / BoundsCenter /
     Bone("rhand"|"lhand"|"head"|"chest") / BoneLocalPoint. Use
     `submit_attached(ctx, dbg, attachment)` instead of `submit`;
     the rig resolves the anchor every frame and the effect just
     calls `effect->ForcePos(...)` from the world point. See
     `TDripEffect.OnHand` in src/vfxtest.cpp for the canonical
     1-line example. Design + forensics:
     [CHARACTER_RIG_FORENSICS.md](CHARACTER_RIG_FORENSICS.md).
   - **Character + weapon + attack-anim** — needs a character
     with a weapon equipped and a swing animation cycling so the
     effect's per-tick logic has live weapon-extents to scan.
     TWeaponSwipe. **Rig available**: register with
     `EVfxPreviewStyle::CharacterAttack` and anchor on
     Bone("weapon") for hilt, BoneLocalPoint("weapon",
     (0,0,+tipZ)) for tip. The rig picks an `attack*` anim state
     automatically; cycle to a specific one via the "Next state"
     button in the rig ImGui panel. Most character meshes name
     their weapon sub-object `"weapon"` or `"sword"`; per-
     character fallback list lives in
     `CHARACTER_RIG_FORENSICS.md §2`.
   - **Projectile (source→target)** — needs a launch point, a
     target point, and a way to advance the projectile along a
     vector each tick. TMissileEffect / TFireBallEffect /
     TIceBoltEffect / TPhotonEffect. **Rig pending**.
   - **Spell-cast (caster→ground)** — needs a caster character
     plus a ground-impact point. Spell visuals that originate at
     a hand and play out at a target location. **Rig available**:
     register with `EVfxPreviewStyle::CharacterCast` and anchor
     on Bone("rhand") for the cast-from-hand convention (matches
     `src/spell.cpp:399`'s in-game lookup). The rig picks a
     `cast*`/`magic*`/`invoke*`/`spell*` anim state automatically.
     **M09 TTeleporterAnimator (Misthaven recall) is the headline
     downstream consumer** — its port author should pick this
     category, anchor on CharacterRoot for the ground burst plus
     a Bone("rhand") sub-effect for the flourish.
   - **Environment-context** — needs a sector / ground plane /
     water surface to anchor against. TWaterFallEffect, ambient
     drips, ripples on water.

   Identify which category fits, list exactly what mock objects /
   loaders / loops the test harness must construct, and whether
   the rig is reusable from an existing harness primitive or
   needs to be built. The library of rig primitives grows
   incrementally — when a new category appears, add the primitive
   to vfxtest.{h,cpp} as `Rig*` helpers (e.g. `CharacterRig`
   [done], `ProjectileRig` [pending]) so the next effect in the
   same category reuses it.
7. **Gaps / unknowns** — anything still ambiguous. Mark explicitly
   what's documented vs guessed.
8. **Trace every helper call.** When the retail draw / tick / pulse
   body calls into a helper (texture lookup, mesh-load, particle-spawn,
   color/scale envelope), the forensics must follow the helper, not
   stop at the call site. Visual constants — colors, sizes, scales,
   atlas frame counts, blend factors, rotation rates — frequently
   live one or two helper-levels down from the lifecycle code that
   first attracts the agent's eye. A port that nails the state
   machine but reads scale/color/texture from "looked about right"
   is the failure mode we keep hitting. If you find yourself making
   up a number that retail computes, you have not finished forensics.

Only after the row's notes carry these facts do you start the port.
Reason: a port grounded in evidence is reviewable against the
evidence; a port grounded in vibes can't be verified at all and
guarantees rework. The blood port (B01) is the example: it shipped
with the forensics gap labelled, so the author could see exactly
which choices were retail-faithful and which were placeholders.

### 3.1 Source-of-truth order

For any given effect:

1. **Retail decomp** under `recon/classes/` (Ghidra output) is the ground
   truth for behavior. Check the `Recon file` column. If it says `pending`,
   Phase 0.3 hasn't reached your effect yet — either wait, or run Ghidra
   yourself per [../../recon/docs/PIPELINE.md](../../recon/docs/PIPELINE.md).
2. **Pre-release source** under `src/effect_old.cpp` / `src/effectcomp.cpp`
   (`#if 0` regions) — useful for *intent*, not for verbatim porting.
3. **Existing port shell** in `src/effect.h` etc. — extend, don't rebuild.

If retail and pre-release disagree, **retail wins** for behavior. Note the
divergence in the row's `Notes`.

### 3.2 What you're producing

Per [../PARTICLE_EFFECTS.md §15](../PARTICLE_EFFECTS.md) — do *not* port D3D
animators one-by-one as bespoke C++ renderers. Instead:

1. Identify the animator registration (`REGISTER_3DANIMATOR("X", TXAnimator)`).
2. Attach an effect component from the animator builder.
3. Express the behavior as an effect definition (expression-based, see PARTICLE_EFFECTS §6–§7).
4. Add only the **missing** generic engine capability to support it. Generalize, don't special-case.
5. Leave the old D3D body in `effect_old.cpp` / `#if 0` blocks — do not delete.

If your effect genuinely needs a new pipeline capability (e.g. a soft-particle
depth blend mode), that is a Phase-1 expansion. Stop, file it as a separate
row, and either do it or claim it explicitly.

#### 3.2.1 Engine-vs-bespoke decision (mandatory before Phase B)

Not every effect should be a data-driven engine definition. **Use the engine
for the common shapes; keep bespoke when behavior is genuinely unique.**

**Use the engine** (declare in `data/Resources/effects.def`, drive via
`TParticleEffectComponent` + expression VM, minimal C++ shim) when the
effect is one of these shapes:

- **Standard spray** — burst or continuous emitter, particles with
  initial velocity + spread cone + gravity + lifetime + alpha/scale/color
  curves. (Blood, mist, sparks, smoke, fountains, swirls.)
- **Standard pulse** — single billboard or N-particle burst with
  scale/alpha envelope over a fixed life. (Halo pulses, hit flashes,
  spell-cast rings, ripple expansions.)
- **Standard flipbook billboard** — atlas-cell animation cycling at a
  fixed rate. (Torch flame, fire patches, sprite-based ambient
  visuals.)
- **Standard ray / missile particle trail** — projectile leaves a
  particle trail behind it as it flies. (Fireball ember tails, ice
  bolt frost trails.)
- **Screen-space or world-space quads with standard blend/lit modes**.
  (Most overlay effects, glow halos, hit decals.)

**Stay bespoke** (write a focused C++ class with its own state machine
and `SubmitFx*` calls) when the effect needs:

- **Per-vertex custom geometry** beyond standard quads (sword swipe
  with weapon-extents-driven width, lightning bolt with
  spline-smoothed per-segment jitter + miter joins).
- **Effect-specific gameplay state machines** (multi-phase spells with
  LAUNCH→FLY→EXPLODE stages, hit-resolution callbacks, owner-state
  coupling).
- **Character-attached behavior** that scans the owner's mesh / bones
  / equipped items per frame.
- **Unique kinematics** that aren't well-served by standard spray /
  pulse vocabulary (composite multi-emitter effects with cross-emitter
  coordination, complex collision response).

**Decision criteria during Phase A forensics:** when you write the
forensic body, name the shape category. If it matches a standard one,
the Phase B port goes through the engine. If it doesn't, the Phase B
port is bespoke C++. If you write bespoke when the shape was standard,
that's drift — call it out and refactor.

**Don't rabbit-hole on conversions.** If the engine VM is missing one
op a standard-shape effect needs (e.g. gravity, alpha curve), add the
op generically and migrate. If the engine needs three new ops *just*
for one effect's quirks, that effect is probably bespoke.

**Bespoke effects often combine bespoke + engine.** A "fireball" effect
class is fundamentally bespoke (multi-phase state machine, I3D mesh
advance along a flight path, hit-resolution callback), but the
particle sub-pieces it spawns — ember trail behind the missile,
explosion-burst at impact, smoke puffs at death — should go through
the engine via `TParticleEffectComponent` / declared bucket-emitter
defs. Same for a lightning bolt with spline-jittered geometry that
also emits spark particles, or a sword swipe whose strip is bespoke
but whose hit-spark accent is engine. **Bespoke owns the unique;
engine owns the particles inside it.**

The decision is per-subsystem, not per-effect: a bespoke effect can
declare an `effects.def` block for its particle sub-emitters and
construct a `TParticleEffectComponent` from C++ rather than from a
top-level builder registration. The engine doesn't care who calls it.

**Engine-shape decision checklist** (run for every billboard / particle
submission in the Phase B port):

- [ ] **Blend mode** — pick the `EFxBlend` / `EParticleBlendMode` that
      matches the retail blend constants from the forensics §4 row
      (Alpha / Additive / AdditiveStraight / PremulAlpha).
- [ ] **Depth mode** — `TestNoWrite` is the standard transparent path;
      pick `TestWrite` only for mostly-opaque alpha-tested impostors;
      `None` for always-on-top overlays.
- [ ] **Light mode** — `LitFlat` for blood / debris / lit smoke; `Unlit`
      for explosions / sparks / additive glows and most overlay effects.
- [ ] **Orientation** — if the effect is **ground / water / wall-
      projected** (a decal-like overlay that lies on a surface — ripple
      on water, halo / heal aura / spell circle on the floor, blood
      pool, magic glyph, footprint), set `orientation = WorldXY`. If
      the effect is a **camera-facing glow** (torch flame, fire patch,
      flare, spark, smoke puff), leave the default `ScreenAligned`.
      Retail-faithful: if the pre-release Render path applies
      `RotateX(±π/2)` to the mesh-template plane (tipping the sprite
      onto the ground), that's a strong signal the effect wants
      `WorldXY`; if it composes the I3D quad through the camera-basis
      matrices without any axis flip, it's `ScreenAligned`. Document
      the retail anchor convention in the row's notes — callers should
      know whether to snap pos.z to `GroundZAt(x,y)` / `WaterSurfaceZAt(x,y)`
      before spawning, or whether the harness's z=0 (PickPreviewOrigin)
      is enough.

### 3.3 Diagnostic ladder (mandatory)

Per PARTICLE_EFFECTS §3.2, every effect goes through these visible stages
before you flip its row to `validated`:

1. `Status: debug-solid` — `debug=solid_green` mode renders. This proves
   attachment, transform, bucket, and submit path work.
2. `Status: current-frame` — real UVs with no animation. Proves texture and blend.
3. `Status: normal` — full intended behavior.
4. `Status: validated` — looks right next to retail reference (see §4).

Bump the status as each stage clears. Do not skip stages.

### 3.4 Coding rules (project-wide, summarized)

- **Modern C++ at touch sites.** `override`, `=default`, `nullptr`, `constexpr`, `[[nodiscard]]`, field initializers, `enum class`. Const-correctness.
- **No P\* pointer typedefs.** Use raw `T*` pointers. Remove existing `PT*`/`PS*` at touch sites if local.
- **No new raw threads.** Use the worker abstractions.
- **Minimal includes.** Trim umbrella includes.
- **No bodges.** If something is structurally wrong, fix it; if you can't fix it now, write a row in INVENTORY notes for the right fix and proceed deliberately.
- **Preserve old code.** Move retired retail D3D code to `attic/` or wrap in `#ifdef UNUSED` — do not delete what you intend to migrate.
- **No new raw `fprintf(stderr)`.** Use `src/logging.h` (rxi log.c facade).
- **World-space throughout** in any deferred-lighting path.
- **Renderer stays game-agnostic.** Effect-specific code lives in `src/`, not in the core renderer. The renderer exposes generic submission (`SBillboardDrawItem`, `SParticleDrawItem`, etc.).

### 3.5 Dual-track porting: engine vs direct mesh port

Many retail effects are **composites** of a bone-animated I3D mesh + particles
+ light. (Meteor body + flame trail + impact spark; fire-column animated core
+ ember swirl; tornado funnel mesh + dust particles; dragon-fire ball mesh +
ember tail.) The data-driven engine handles the particles / strips /
billboards. The I3D mesh subsystem does **not** belong in that engine — port
it directly from retail animation code instead.

**Two tracks for any given effect:**

| Track                       | Use when                                                          | Where it lives                                                   |
|-----------------------------|-------------------------------------------------------------------|------------------------------------------------------------------|
| **Engine extension**        | Behavior is billboards, particles, strips, light-pulse, blend-mode permutation. | Effect definition + (rare) new engine capability per §3.2 above. |
| **Direct retail mesh port** | Behavior requires a bone-animated I3D mesh, per-frame pose updates, mesh-specific state (rotation, scale tween, anim curve). | New C++ class in `src/`, modelled on retail decomp. Held as an `IM` component of the effect.  |

**How the two combine:** the effect class owns *both*. It instantiates an
I3D-mesh component (using existing renderer mesh-submit path — **not** a new
sokol pipeline), spawns particle buckets through the VFX engine, optionally
registers a `Renderer->AddPointLight` each frame, and coordinates their
lifecycle. Lifecycle goes through `RegisterUpdate` like any other component.

**Rules of thumb:**

- If the retail class is mostly bone math + animation table lookups + I3D
  draw → it's a direct port. Don't shoehorn it into the expression VM.
- If the retail class is mostly `SpawnParticle()` calls + simple geometry →
  it's an engine effect definition.
- Composites: factor cleanly. The mesh half is a port; the particle half is
  a def. They share an owner.
- The renderer's existing `SMeshSubmit` / skinned-mesh path is already there
  — don't add a new sokol pipeline for `IM`. (PHASE1_SPINE.md §1 lists this
  explicitly: MP/IM mesh-paths reuse `SMeshSubmit`, no new pipeline.)
- Preserve retail mesh code under `attic/` or `#ifdef UNUSED` if you replace
  rather than evolve it — per project-wide rule.

This rule reconciles PARTICLE_EFFECTS.md §15 ("don't port D3D animators
one-by-one as bespoke renderers") with the reality that some retail
animators *are* legitimate I3D-mesh code that has no place in a particle
engine. §15 stands for particle/billboard/strip animators; the I3D-mesh
slice is the explicit carve-out.

## 4. Verifying

### 4.1 Visual check via `--test=vfx`

The harness is the gate. Per [TEST_MODE.md](TEST_MODE.md):

1. `cmake --build build && ./build/revenant --test=vfx`
2. Arrow-key to your effect. It should appear in the browser list automatically
   if you followed the registration pattern (see TEST_MODE §3).
3. Step through debug modes (Solid → FullTexture → CurrentFrame → Normal).
4. Watch a few seconds of the loop. Effects with finite lifetime should restart
   on a timer or via the `R` key.

### 4.2 Retail reference

When possible, capture a short video / screenshot of the original Revenant
1999 running the same effect and compare. Sources:

- The user (Benjamin Cooley) is the original developer — when uncertain, ask
  via the conversation rather than guessing.
- Existing fan videos / Let's Plays for spells the user doesn't immediately recall.
- The retail `Revenant.exe` in `data/` runs under Wine/dosbox for spot checks.

Note any deliberate divergence (e.g. "blood is a touch brighter — better on modern displays") in the row's `Notes`.

### 4.2.1 No procedural stand-ins for asset-driven effects (HARD RULE)

If the retail code loads a `.I3D` mesh, `.RVI` imagery, or any named asset and
draws it, **the port must load and draw the same asset**. Writing procedural
geometry / billboards / gradients as a "stand-in" for an asset draw is a bodge
even if the harness output looks superficially similar.

Self-check during forensics (§3.0): does the retail source call
`FindImagery` / `RegisterImagery` / `LoadImagery` / `Load3DAsset` / any
imagery-lookup helper that resolves a named asset? If yes, the port path
must reach the equivalent engine API. If that engine API does not yet
exist, **stop and escalate as a blocker** — do NOT silently fall back to
procedural geometry.

Test for this in your own work: read your `SpawnForTest` and `Tick*` paths
back-to-back with retail. Every named-asset call in retail must have a
named-asset call in your port. Procedural code (`MakeTexture(...)`,
`GenerateGradient`, billboard-from-scratch) appearing where retail loaded
an asset means you have a stand-in and the port is not done.

Caught failures so far: M09 TTeleporterEffect first port shipped a procedural
glow column instead of `Magic\gvortex.I3D` (re-dispatched as M09b). F03
TFireEffect under audit for same pattern. Reference: [[feedback-no-standins]].

### 4.2.2 Capture practice (snap_grid framing)

Your own captures via `tools/vfx/snap_grid.py` must let a reviewer see the
whole effect without having to ask follow-up questions. That means:

- **Frame to the full visual extent** — when the effect has multiple phases
  (Init / peak / fade) with different bounding boxes, frame for the LARGEST.
  A cylinder that grows to engulf the caster must not get cropped at peak.
- **Capture the diagnostic moments**, not arbitrary intervals:
  1. Pre-trigger (state machine in starting state — proves baseline)
  2. First frame of visual (proves it spawns correctly)
  3. Visual peak (proves shape / scale / texture / color at apex)
  4. Mid-decay or motion mid-point (proves any rotation / drift / fade)
  5. Last frame before kill (proves clean death)
- **Animated effects need a sequence, not a grid.** For rotation, traveling
  projectiles, sword arcs, or anything where motion IS the effect, capture
  a per-frame sequence at the cadence the user can replay. A 4×4 stills grid
  fails to show whether rotation is happening or just translation.
- **Label the frames.** Annotate the moment each frame shows
  (e.g. "OUT peak — engulf", "MOVE start — rotation 90°"), or include a
  small timestamp burn-in. A 16-frame grid with no context shifts the
  parsing burden to the reviewer.
- **Include scene context.** Locke or a known-scale anchor in frame, so
  the reviewer can judge whether the effect is the correct size relative
  to a character / weapon / area.

Before declaring done, look at your own capture and ask: "if I had only
this image and the retail reference, could a stranger tell whether they
match?" If not, the capture is failing its job and the port is not
verifiable yet. Re-shoot before reporting.

### 4.3 Build-clean check

Before flipping to `validated`:

- Clean build with no new warnings introduced (clang -Wall -Wextra baseline).
- ASan-clean run of `--test=vfx` cycling through all effects at least once.
- The retail-sync test (`--test=...` per [../../recon/docs/RETAIL_SYNC_PLAN.md](../../recon/docs/RETAIL_SYNC_PLAN.md)) for any class with a confirmed retail mapping.

## 5. Handoff

After each work session — even mid-effect — leave the row in a state another
agent can pick up:

- Status reflects current reality (don't leave `debug-solid` if you've moved on).
- `Notes` updated with a one-line "where I left off" and any blockers.
- If you're stopping mid-effect, set `Status: wip` and leave a `next step: ...` line in `Notes`.
- Commit before you stop.

If you discover a class that isn't in [INVENTORY.md](INVENTORY.md), **add a
row**. Better to over-list and reconcile than to leave it implicit.

## 6. Spawning sub-agents

If your slice naturally subdivides (e.g. a composite effect like
`TMeteorStormEffect` is mesh + particles + light + strip), you can fan out to
sub-agents:

- Each sub-agent gets a self-contained brief: file paths, retail reference,
  exit criterion, what NOT to do.
- Brief them on the existence of this guide and PARTICLE_EFFECTS.md — they
  should not invent a parallel architecture.
- Reconcile sub-agent outputs back into the single INVENTORY row before
  marking it `validated`.

Do not spin up sub-agents for trivial single-file work — the overhead exceeds
the benefit. Rule of thumb: only fan out if the work is genuinely parallel and
each branch is ≥ 30 minutes of focused effort.

## 7. Things to avoid

- Editing `effect_old.cpp`. It is archival.
- Adding effect names into the core renderer. The renderer is game-agnostic.
- Building per-effect bespoke `Render()` methods. Use the bucket/emitter system.
- Reintroducing D3D types. Sokol only.
- Skipping the diagnostic ladder.
- Marking `validated` without a visible check in `--test=vfx`.
- Deleting `#if 0` D3D bodies until their replacement is `validated`.
- Quietly broadening scope mid-effect. If you find a structural problem,
  surface it as its own row.

## 8. If you're stuck

In order:

1. Re-read [../PARTICLE_EFFECTS.md](../PARTICLE_EFFECTS.md). Most "I don't know how to do this" answers are in there.
2. Look at how `TFlameEffect` is structured — it's the canonical FB example.
3. Look at the retail decomp for the effect itself.
4. Ask the user. They wrote this engine in 1999.
