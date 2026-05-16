# VFX Restoration

Operational plan for restoring all of Revenant's visual effects — spell
effects, combat slashes & hit flashes, blood, fire/flame, teleport, environment
— in the modern renderer, validated through a dedicated `--test=vfx` harness.

This document is the entry point. New agents picking up this work should start
here, then read whichever sister doc matches their task.

## Sister documents

| Doc                                  | What it is                                                                  |
|--------------------------------------|-----------------------------------------------------------------------------|
| [INVENTORY.md](INVENTORY.md)         | Per-effect burndown. The list of every effect, current status, owner.       |
| [AGENT_GUIDE.md](AGENT_GUIDE.md)     | Protocol for parallel agents claiming, working, and handing off rows.       |
| [TEST_MODE.md](TEST_MODE.md)         | `--test=vfx` spec: UI, navigation, registration, what "passing" looks like. |

Adjacent existing docs (read these — do **not** duplicate them):

- [../PARTICLE_EFFECTS.md](../PARTICLE_EFFECTS.md) — the **engine architecture**: buckets, emitters, expression language, channel naming, computed buffer, flame MVP. This is the spec for the engine you are extending.
- [../RENDERER_ARCHITECTURE.md](../RENDERER_ARCHITECTURE.md) — sokol-side renderer layout.
- [../PORT_PLAN.md](../PORT_PLAN.md) — overall port plan (this effort sits inside Phase 4: feature recovery from Ghidra).
- [../../recon/docs/RETAIL_SYNC_PLAN.md](../../recon/docs/RETAIL_SYNC_PLAN.md) — retail-sync conventions (REVSYNC headers, class fingerprinting).

## 1. Goal

`./revenant --test=vfx` opens an ImGui browser showing one visual effect at a
time. **Left/Right arrow keys** cycle through every effect in the catalogue.
Each effect renders in a neutral 3D scene (ground plane, simple lighting) using
its real runtime — same code path the game will use in-engine. Each effect
matches the retail 1999 build closely enough that a fan of the original would
not notice a regression.

Success criteria are spelled out in [TEST_MODE.md](TEST_MODE.md) §4.

## 2. Scope

**In scope:**

- Every effect class identified in retail (full list in [INVENTORY.md](INVENTORY.md)).
- All visual aspects: emitter behavior, particles, billboards, strips/trails, light coupling, blend modes, animation curves.
- A `--test=vfx` browser harness that survives engine refactors.
- Pre-extraction of retail effect functions from `Revenant.exe` into `recon/` so per-effect agents are not blocked on Ghidra.

**Explicitly out of scope** (defer or skip):

- Re-wiring effects into in-game triggers (combat hit, spell cast, environmental spawn). That is a follow-on task, tracked separately once the catalogue renders standalone.
- Audio. `TAmbSoundEffect` and other audio-only effects are tabled here as no-ops; sound is a separate subsystem.
- Modern enhancements beyond parity (HDR bloom, GPU particles, ray-traced lights). Park ideas in a "Phase 5" section once parity is real.
- New effects that did not ship in 1999.

## 3. Current state (as of 2026-05-16)

From the bring-up research summarized at the top of this effort:

- **Engine spine partially in place.** [src/effect.h](../../src/effect.h) declares 50+ effect class shells. [src/effect.cpp](../../src/effect.cpp) registers exactly one (`TFlameEffect`) — every other animator builder is commented out at lines 9-56.
- **D3D-era animator `Render()` bodies preserved under `#if 0`** in [src/effectcomp.cpp](../../src/effectcomp.cpp), [src/stripeffect.cpp](../../src/stripeffect.cpp). Reference material — do not delete, do not port verbatim. See [../PARTICLE_EFFECTS.md §15](../PARTICLE_EFFECTS.md) ("do not port legacy D3D animators one by one as bespoke C++ renderers").
- **Full retail D3D effect implementations archived** in [src/effect_old.cpp](../../src/effect_old.cpp) (13.7k lines, not compiled).
- **Renderer has no particle/billboard submission path yet.** `TFlipbookBillboardComponent` infrastructure exists ([src/effect.h:70-105](../../src/effect.h)) but is not wired into the sokol frame submission.
- **No in-game callers active.** Nothing spawns effects today — no `SpawnEffect()` or equivalent runs.
- **No `--test=vfx` mode.** `--test=` dispatch exists ([src/revmain.cpp:1631](../../src/revmain.cpp), [src/testmodes.cpp:2208](../../src/testmodes.cpp)); we add a new branch.

## 4. Phased plan

The dependency shape is: **engine spine → per-effect ports → in-game wiring**.
Phase 0 and Phase 1 are mostly serial; Phase 2 fans out wide and is where
multiple agents work in parallel.

### Phase 0 — Foundation (this commit + immediate follow-ups)

| Step | Output | Parallelizable |
|------|--------|----------------|
| 0.1 | These planning docs | done |
| 0.2 | Stub `--test=vfx` mode that boots an empty browser ("no effects registered yet") | no — single PR |
| 0.3 | Ghidra headless extraction of all `Effect`/`Particle`/`Spell`/`Animator` functions from `data/Revenant.exe` into `recon/` (single background job) | runs in background |
| 0.4 | Cross-reference retail decomp against [INVENTORY.md](INVENTORY.md); add any classes we missed; populate `retail_addr` and `recon_file` columns | depends on 0.3 |

**Exit criterion:** `--test=vfx` launches and shows an empty list; INVENTORY rows have retail/recon references where they exist.

### Phase 1 — Engine spine

Per [../PARTICLE_EFFECTS.md](../PARTICLE_EFFECTS.md) §3–§13. The catalogue is
useless until the engine can submit draws. These are **prerequisites** to any
parallel work in Phase 2.

| Step | Output |
|------|--------|
| 1.1 | Renderer-side submission API for billboards (per PARTICLE_EFFECTS §13). Game-agnostic — lives in the core renderer, consumers submit `SBillboardDrawItem`. |
| 1.2 | Flipbook-billboard pipeline (sokol). Validates with `TFlameEffect` debug-solid → current-frame → normal ladder. |
| 1.3 | Particle-emitter pipeline. Bucket + computed-buffer + expression bytecode per PARTICLE_EFFECTS §4–§11. |
| 1.4 | Strip / ribbon pipeline. Sword trails, lightning, wind streaks. |
| 1.5 | Light-source coupling. Effects can emit dynamic lights into the deferred lighting path. |
| 1.6 | `--test=vfx` browser actually navigates effects, runs full sim, draws on a neutral ground plane. Debug-mode ladder accessible per-effect (Solid/FullTexture/CurrentFrame/Normal). |

**Exit criterion:** `--test=vfx` shows the torch flame correctly. The five
pipelines exist and have at least one effect each validating them.

### Phase 2 — Per-effect ports (parallel)

This is where most of the work lives. Each row in [INVENTORY.md](INVENTORY.md)
becomes a small, claimable unit of work. The [AGENT_GUIDE.md](AGENT_GUIDE.md)
defines the claim/work/handoff protocol.

Suggested ordering for the first wave (so the engine grows in capability
rather than getting stuck on composites):

1. **FB-pipeline effects first** — TFlameEffect (done), TFireEffect, TRippleEffect, THaloEffect. Smallest blast radius.
2. **PE-pipeline effects** — TBloodEffect, TPulpEffect, TFireSwarmEffect, TSandswirlEffect, TPixieEffect, TDripEffect. Shakes out the emitter+expression path.
3. **SR-pipeline effects** — TStripEffect (sword slash), TLightningAnimator, TWindStripEffect, TStreamerEffect. Shakes out ribbons.
4. **MP-pipeline (mesh projectile)** — TMissileEffect, TFireBallEffect, TIceBoltEffect, TFlameDiscEffect, TPhotonEffect.
5. **VO-pipeline (volumetric/overlay)** — TMistEffect (Misthaven teleport), TFogAnimator, TIcedEffect, TBurnEffect, THealEffect, TAuraEffect, TSpellBlock.
6. **Composites** — TMeteorStormEffect, TTornadoEffect, TVortexEffect, TFireColumnEffect, TWaterFallEffect. Last, because they assume the simpler pipelines work.

**Exit criterion:** every non-skipped row in [INVENTORY.md](INVENTORY.md) is
`status: validated` and visible in `--test=vfx`.

### Phase 3 — In-game wiring (out of scope for this effort)

Reconnect effect spawning to combat hits, spell casts, environment timers,
script triggers. Tracked separately; mention here only to make the boundary
explicit.

## 5. Parallel-agent model

Because Phase 2 is naturally a wide fan-out, this work is designed to be split
across multiple agents. The full protocol — how to claim a row, what to read
first, how to verify visually, how to update status — is in
[AGENT_GUIDE.md](AGENT_GUIDE.md).

The short version:

- **Source of truth for "what's left" is [INVENTORY.md](INVENTORY.md).** No other notebook.
- Each agent claims one row (or one small adjacent cluster) by editing the `Status` and `Owner` columns. Agents avoid overlapping by reading the table first.
- Each agent verifies visually via `--test=vfx` before flipping status to `validated`.
- Each agent leaves a one-line `Notes` entry on hand-off.

## 6. Open questions

Tracked here so they're not lost between sessions:

- **Hit flashes / combat sparks** — the user listed these explicitly but no obvious 1:1 class match exists in the recon inventory. Possibilities: short-lived `THaloEffect`, an unnamed billboard burst, or a not-yet-decompiled function. Resolved when Phase 0.3 (Ghidra extraction) lands.
- **Candle flames** — likely `TFlameEffect` parameterized small, attached to a candle imagery; confirm during Phase 2 FB pass.
- **Misthaven teleport** — best guess is `TMistEffect`. Verify against retail script triggers and the spell registry once available.
- **Sword slash trail** — best guess is `TStripEffect` attached to weapon during attack frames. Confirm from retail combat code.

## 7. Reading order for a brand-new agent

1. This file (README.md).
2. [TEST_MODE.md](TEST_MODE.md) — so you know what "done" looks like.
3. [../PARTICLE_EFFECTS.md](../PARTICLE_EFFECTS.md) — the engine architecture.
4. [INVENTORY.md](INVENTORY.md) — pick a row.
5. [AGENT_GUIDE.md](AGENT_GUIDE.md) — claim it and follow the protocol.
