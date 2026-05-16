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
