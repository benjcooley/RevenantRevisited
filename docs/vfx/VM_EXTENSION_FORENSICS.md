# Particle VM Extension Forensics (B01 / M05 / F03 / H03 / H04)

Date: 2026-05-17
Branch: feature/vfx (worktree-agent-aad98a6fd1af083ab)

## 0. Goal

Five "Phase B landed" effects (B01 blood, M05 mist, F03 fire, H03 ripple,
H04 drip) each ship their per-particle dynamics in raw C++ on
`TParticleBucket` (effect.cpp `TickAndSubmitForTest`) rather than through
the data-driven `effects.def` + expression VM. This document inventories
what they do, what the current VM (`src/particlefx.h`
`EParticleOp{Add,Sub,Mul,Div,Mod,Neg,Floor,Sin,Cos,Min,Max,Clamp,Lerp,
Frame,Flipbook,Vec2,Vec3,Vec4}` + `time_frame / age / age01 / seed`
identifiers) cannot express, and proposes the additions needed to migrate
them.

## 1. Per-effect dynamics inventory

| Effect | Dynamic | Today's C++ on `TParticleBucket` | Missing VM vocabulary |
|--------|---------|-----------------------------------|------------------------|
| **B01 TBloodEffect** | Initial velocity from polar cone | `cos(angle)*radial, sin(angle)*radial, upward`; `angle = u1*2π`, `radial = 60+80*u2`, `upward = 140+80*u2`, `u1,u2 = rand01()` | **rand01 / rand(lo,hi)** — no random source per-particle at spawn |
| | Initial scale jitter | `s = 24 + 16*u1` | **rand** (covered above) |
| | Random pick from 4 atlas variants | `df = rand() % 4` | **rand_int / floor(rand*4)** — once rand exists, `floor(rand01() * 4)` suffices |
| | Initial rotation jitter | `dr = rand01() * 2π` | rand |
| | Tint jitter (red ±0.2, green +0.1*u1, blue +0.05*u2) | `col[0]=1.00-0.20*u2; col[1]=0.10+0.10*u1; col[2]=0.10+0.05*u2` | rand |
| | Per-frame gravity | `vel.z += -480 * dt`; `pos += vel * dt` | **per-tick integration in the VM** — current ops are stateless eval; no way to read+write a particle var across frames |
| | Alpha curve over Age01 | `fade = (t<0.5)?1 : 1 - 2*(t-0.5)` (held-then-linear-fade) | **step / smoothstep / piecewise-linear** — expressible as `clamp((1-age01)*2, 0, 1)`, so already possible IF we let bucket-level expressions write `DrawColor.a` per tick |
| | Reap on age > life | `*o = kDeadSentinel; KillParticlesByOwner` | **kill predicate** — bucket-side; the VM doesn't even need to know |
| **M05 TMistEffect** | Initial pos in ±L/2 × ±W/2 × +5 envelope | `(frand()-0.5)*L*3, (frand()-0.5)*W*3, kMistSpawnZ` (port widened the spawn area 3×) | rand |
| | Initial vel: vx ±L/32, vy ±W/32, vz random(H/2, H) | `(frand()*2-1)*(L/32), ..., (H*0.5)+frand()*(H*0.5)` | rand |
| | Per-tick integration: pos+=vel; vel.z -= 0.37 | per-tick inner loop accumulates wall-clock to a 24Hz tick gate | **tick-frequency-gated integration**, **statement-form bucket tick expression** |
| | Respawn on pos.z ≤ origin.z | C++ tests `pos[2] <= origin.z`, re-runs `SeedMistDrop()` | **predicate + re-init/branch** — could be modelled as "particle-died at this condition" + re-fire spawn expressions, OR as `kill_when` predicate that lets normal respawn-rate refill the bucket |
| **F03 TFireEffect** | Scatter pattern: 15 quads at random ±50 wu | C++ `SFireScatterQuad{ox,oy,frame}` array on the effect; not particles in a bucket at all | The dynamic itself IS a bucket-of-15 with rand init — already expressible IF rand exists |
| | Per-quad atlas frame counter -22..30 looping | `if (frame >= 30) reroll; else ++frame` per tick | **per-tick scalar accumulator** + **periodic reset/reroll** — same "statement-form bucket tick expression" + rand for the re-roll |
| | Startup-delay window (frame<0 → invisible) | `if (frame<0) continue` in the submit loop | **clip / skip-submit predicate** — could be expressed by setting `DrawColor.a = step(0, frame)` (alpha 0 = invisible). Today's `Frame` op produces frame counter, but the bucket needs a way to say "if this expression < 0 the particle is invisible." Cleanest: a per-bucket `visibility_expr` returning 0/1 multiplier on alpha. |
| **H03 TRippleEffect** | Single-billboard effect (not a particle bucket!) | bespoke `SBillboardDrawItem` per frame, integrates `scale_+=step` + `ripframe` cycle | This one is **FB pipeline, not PE**. Per AGENT_GUIDE §3.2.1 a single-billboard ground pulse with a scale/atlas envelope IS a "standard pulse" candidate. Migrating it means routing it through a PE bucket of 1 particle with VM-driven scale and frame curves, OR through `TFlipbookBillboardComponent` (which already supports `frame_expr` + `uv_rect_expr` via VM). The custom frame schedule (4-step expansion loop, then jump to 4..15 dissipation, then kill) needs: **conditional select (if/else / step)** and a **kill-when-age01==1 lifecycle**. The scale grows linearly with frameon → already expressible as `(0.5 + age * scale_step)`. The frame schedule is a piecewise function `if (frameon ≤ len-24) ripframe = (frameon/2) mod 4; else ripframe = 4 + min(11, (frameon-(len-24))/2)`. Expressible as `min((age/scale_rate), 4) + max(0, ((age-tail)/2))` once we have `step` / `select`. |
| | Atlas reordering table `kRippleFrameOf[]` | C++ table lookup | **table indexed by frame** — a 16-entry constant table. Could be done with a chain of `select` ops; cleanest is a new `LookupTable` op that takes (idx, table-index-into-constants-region). Cost-benefit: not justified for one effect — handle by precomputing the visible cell directly in the expression. The table is just `(15-frame) XOR 3 within each row of 4`. Bit-XOR isn't in the VM; alternative is the equivalent arithmetic `floor(frame/4)*4 + (3 - (frame mod 4))`. **Already expressible with existing ops** (`Floor`, `Mod`, `Sub`, `Mul`). |
| | Orientation = WorldXY | already set per-billboard at submit; bucket-level orientation knob exists | covered |
| **H04 TDripEffect** | Single-drop state machine: dead→wait→fall→land→spawn ripple→dead | bespoke C++ with `dead_`, `time_`, ownership of `std::unique_ptr<TRippleEffect>` | **multi-state per-particle FSM** — the dead/wait/fall states are too rich for an expression VM; this is genuinely bespoke control flow. The drop's BALLISTIC fall (gravity, position) is expressible. The state transitions (respawn coin flip, sub-effect spawn on landing) are not. **DEFER as bespoke**, but the fall-phase kinematics (pos += vel; vel.z -= g) share the M05/B01 tick infrastructure. |
| | Random respawn coin flip | `time > period && rand() % (period/2+1) == 0` | rand, plus condition handling — same theme as M05 |
| | Drip→ripple sub-effect spawn on landing | `TRippleEffect::SpawnForTest(landing)` + own as unique_ptr | **sub-effect spawn from VM** — this is real new infrastructure (cross-effect coupling). NOT proposing it in this round (YAGNI per the brief; only 2 of 5 need to migrate). |

## 2. Cross-cutting summary — what's missing

Three real gaps surface across the five effects (in priority order):

1. **Per-particle random source** (`rand01()`, `rand(lo, hi)`). All five
   touch rand at spawn-init for cone/scatter spread, color jitter, scale
   jitter, atlas pick, respawn timing. Today's `seed` identifier exists
   but is a single deterministic value per particle (set in eval ctx —
   never written), not a per-call PRNG. This is the single highest-value
   add: it unlocks B01's whole spawn-init block, M05's whole spawn-init
   block, F03's per-quad XY scatter + frame stagger, and any future
   "scatter N particles in a cone with jitter."
2. **Per-tick integration** — the ability for a bucket to run an
   expression *every tick* that reads particle vars and writes them back.
   Today's VM is stateless eval: you evaluate `frame_expr(time_frame)`
   to get a value, but you can't say "vel.z -= 0.37; pos += vel" and
   have that persist into the next frame's particle state. Mist's
   gravity-fall + respawn-on-landing IS this pattern, B01's gravity-fall
   IS this pattern, F03's frame counter IS this pattern. The fix is a
   per-bucket **tick expression** with statement-form syntax (assignment
   `var = expr;`) and a `StoreVar` op (already in the enum, never used)
   that writes back to particle slot vars.
3. **Sim-tick gating** — each effect has its own copy of the 24 Hz
   accumulator loop (`sim_accum_ms_ += dt*1000; while (>=tickMs)
   ++ticks;`). This belongs on the bucket as a declarative
   `tick_hz = 24` field; the runtime accumulates and calls the tick
   expression N times per render frame.

Lower-priority items the brief explicitly says to defer or YAGNI:

- Sub-effect spawn (drip→ripple) — bespoke only for H04, keep bespoke.
- Atlas reorder table lookup — H03 only, expressible with existing
  ops via the closed-form arithmetic identity.
- State machines (H04's dead/wait/fall/land FSM) — bespoke C++ shape;
  not a particle-VM concern.

## 3. Proposed VM additions

Design preference per the brief: **lean on tick expressions over new ops**.
Each addition justified against ≥1 of the 5 effects.

### 3.1 New ops

| Op | Args | Justification | Used by |
|----|------|---------------|---------|
| `Rand01` | (none) | per-call PRNG returning U[0,1); cheap LCG seeded per-particle from `seed * 16807u` advanced on each call. Needed for ALL five effects' spawn-init. Without it, no spawn-init expressions are useful. | B01, M05, F03, H04 |
| `Rand` | (lo, hi) | sugar for `lo + (hi-lo)*Rand01`; one op so the compiler doesn't have to emit the LCG chain twice. | B01, M05 |
| `Step` | (edge, x) | returns 0 if `x < edge` else 1. Lets F03 express "frame<0 → invisible" as `step(0, frame)` and B01 express "age01<0.5 → 1, else fade" as `step(0.5, age01)`. Both already model-able with the existing `Min/Max/Sub` but step is the standard graphics primitive and reads cleanly in DEF files. | B01 alpha curve, F03 visibility |
| `Select` | (cond, t_if_nonzero, f_if_zero) | branchless conditional. Used for the F03 per-quad respawn (`select(frame >= 30, reroll, frame+1)`) and for any "switch on age01 phase" curve. Avoids re-introducing real branches. | F03, B01 alpha curve |
| `StoreVar` | (var_id, src) | already in the enum but currently never emitted; expose as a parser statement `var = expr;`. **The keystone op** — without this, tick expressions can't mutate particle state. | M05 gravity, B01 gravity, F03 frame counter |

That's **5 ops total**. Three of them (`Step`, `Select`, `Rand`) are
sugar over existing capability + `Rand01`. The two real new
capabilities are `Rand01` (a stateful per-particle PRNG) and `StoreVar`
(the persistence path).

### 3.2 Bucket-level "tick expression" convention

Add to `SParticleBucketEffectDef`:

```
tick_hz = 24            // integration cadence; 0 = render rate
tick_expr = """
  vel.z = vel.z - 0.37;
  pos = pos + vel;
"""
spawn_expr = """
  pos = vec3(rand(-32, 32), rand(-10, 10), 5);
  vel = vec3(rand(-2, 2), rand(-0.625, 0.625), rand(2, 4));
"""
kill_expr = "pos.z <= 0"   // boolean; true → respawn (treated as new particle, re-run spawn_expr)
```

Where:

- `tick_expr` is parsed as a **sequence of statements** (`var = expr;`).
  Statements compile to `StoreVar`. The right-hand side is a normal
  expression. Implicit "read existing slot value" if the LHS is the
  variable. `var.z = ...` writes lane 2 of a vector slot.
- `spawn_expr` runs once per particle at add time. Same statement
  syntax. `Rand01`/`Rand` reseed using the per-particle seed.
- `kill_expr` is a single boolean expression evaluated each tick after
  the tick body. If true, the particle is respawned (re-running
  `spawn_expr` and zeroing age). This is the M05 "drops never die,
  they recycle" pattern and the F03 "frame >= 30 → re-roll" pattern.

Identifiers exposed in tick/spawn expressions:

- All current ones (`time_frame`, `age`, `age01`, `seed`).
- New: `pos`, `vel`, `color`, `scale`, `frame`, `rot` — read/write
  aliases for `DrawPos`, `EmitVel`, `DrawColor`, `DrawScl`,
  `DrawFrame`, `DrawRot`. The aliases keep DEF files readable; the
  parser maps them to the underlying `EParticleVar`.
- New: `emit_pos`, `emit_vel` — bucket-anchor world origin (read-only,
  set by the runtime from the owning effect's position).

Burst / continuous spawn rate as bucket fields:

```
spawn_burst = 10        // one-shot at attach (B01 blood)
spawn_count = 50        // steady-state target population (M05 mist, F03 fire)
                        // runtime tops up by spawning replacements when count drops below target
```

`spawn_count` covers F03 (steady 15 quads) and M05 (steady 50 drops)
cleanly. `spawn_burst` covers B01 (one-shot impact). Continuous-rate
emission (`spawn_per_sec = N`) is YAGNI — none of the 5 needs it (B01
is burst, M05/F03 are steady-state target population). Add later when
a real effect demands it.

### 3.3 effects.def syntax additions

The existing block parser already accepts `frame_expr` / `uv_rect_expr`
as multi-character keys with quoted-string values. Reusing that
machinery, add (inside `bucket` blocks):

```
bucket droplets
{
    texture_slot = 0
    width = 64
    height = 64
    blend = additive_straight
    light = unlit
    depth = test_no_write
    orientation = screen_aligned

    spawn_burst = 10
    spawn_count = 0              // (default 0; only one of burst/count is non-zero)
    tick_hz = 24

    spawn_expr = "
        pos    = emit_pos + vec3(0, 0, 30);
        vel    = vec3(cos(rand(0,6.2832))*(60 + 80*rand01()),
                      sin(rand(0,6.2832))*(60 + 80*rand01()),
                      140 + 80*rand01());
        scale  = vec3(24 + 16*rand01(), 24 + 16*rand01(), 1);
        rot    = rand(0, 6.2832);
        frame  = floor(rand01() * 4);
        color  = vec4(1.00 - 0.20*rand01(), 0.10 + 0.10*rand01(),
                      0.10 + 0.05*rand01(), 1);
    "

    tick_expr = "
        vel.z  = vel.z - 480 * (1/24);    // per-tick (tick_hz=24)
        pos    = pos + vel * (1/24);
        color.a = clamp((1 - age01) * 2, 0, 1);
    "

    kill_expr = "age >= life"
}
```

Lifetime is set by the bucket's `default_life = 1.4` (also new), set
into `EParticleVar::Life` at spawn time. The existing
`emitter`/`output` blocks remain for compatibility with TorchFlame but
become optional: a bucket can self-emit via `spawn_burst` /
`spawn_count` without an explicit emitter block.

### 3.4 Engine-side wiring outline

The `TParticleEffectComponent::Tick` (new method, called per render
frame from the existing `OnUpdate` path) on each runtime:

1. Resolve render-frame `dt`; accumulate into bucket's
   `sim_accum_ms_`; compute `tick_count` from `tick_hz`.
2. For each particle in bucket:
   a. For each of `tick_count` ticks:
      - Eval `tick_expr` (statement sequence; each `StoreVar` writes
        the particle's slot).
      - Advance age by `1/tick_hz`.
      - Eval `kill_expr`; if true OR `age >= life`, respawn (re-run
        `spawn_expr`, age=0).
3. After integration, if `spawn_count > 0` and the bucket has fewer
   live particles than `spawn_count`, top up by adding particles and
   running `spawn_expr` on each.

Existing `frame_expr` / `uv_rect_expr` continue to be eval-each-frame
read-only expressions (no statements), evaluated per-particle at
submit time inside the renderer's bucket-walk — those don't change.
The new `tick_expr` / `spawn_expr` / `kill_expr` are statement-form,
evaluated as part of the per-tick integration loop.

## 4. Migration plan (Phase 5)

Two effects map cleanly to the new vocab:

- **M05 TMistEffect**: pure spawn-init + per-tick integration + respawn-
  in-place. Exact match for `spawn_expr` + `tick_expr` +
  `kill_expr="pos.z <= 0"` + `spawn_count = 50`. Zero bespoke C++
  needed.
- **B01 TBloodEffect**: one-shot burst + per-tick gravity + alpha fade
  curve. Exact match for `spawn_burst = 10` + `spawn_expr` (cone +
  jitter) + `tick_expr` (gravity + age) + `kill_expr = "age >= life"`.
  Zero bespoke C++ needed.

These two effects together cover both spawn modes (burst + steady),
all the new ops, and the tick-gated integration. Migrating them proves
the vocab end-to-end.

**Deferred** (feasible-on-paper, not migrating now):

- **F03 TFireEffect**: 90% migratable (`spawn_count = 15` + per-quad
  spawn + tick that increments `frame` + `kill_expr = "frame >= 30"`
  → respawn). The only awkward bit is the negative-frame "invisible
  startup delay" — expressible as `color.a = step(0, frame)`, but
  there's no existing F03 retail behaviour test bed beyond the
  harness, and the visual is currently a bunch of static-position
  quads at the harness, so it's safe to defer until a real fire
  spawn site exists. **Not migrated to keep scope tight; documented
  as Phase 5 follow-up.**
- **H03 TRippleEffect**: feasible via a 1-particle bucket with
  `scale` driven by `age` and `frame` driven by the closed-form
  arithmetic identity. But it's an FB-pipeline effect (single
  billboard, not particle bucket), and the bespoke version is small,
  clean, and uses the orientation knob that just landed. Migrating
  would be a sideways move; the value is in proving the VM, not in
  reshuffling already-clean code. **Not migrated.**
- **H04 TDripEffect**: drop kinematics (dead→wait→fall) are
  bucket-expressible, but the drip→ripple sub-effect spawn on landing
  is genuinely cross-effect state and would need new infrastructure
  (sub-effect spawn from VM) we're explicitly deferring per YAGNI.
  The bespoke drop FSM is the cleaner home for now. **Not migrated.**

The deferred three are tagged in `docs/vfx/INVENTORY.md` as
"feasible-on-paper via VM, kept bespoke pending future migration."

## 5. Out-of-scope (explicitly NOT adding)

- `Pow`, `Sqrt`, `Atan2` — no effect of the 5 uses them.
- `Noise` / perlin / hash — no effect of the 5 uses them.
- Per-particle wind / drag fields — no effect of the 5 has wind.
- Sub-effect spawn from VM — H04 needs cross-effect coupling that's
  bespoke-shaped; YAGNI.
- Curve/spline ops — none of the 5 has a multi-keyframe curve;
  expressions cover the alpha/scale envelopes they actually need.
- `LookupTable` op — H03's `kRippleFrameOf[]` is the only candidate
  and is expressible via existing arithmetic ops.
