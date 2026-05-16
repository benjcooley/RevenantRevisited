# VFX Inventory & Burndown

Source of truth for **what effects exist**, **what state each is in**, and
**who is working on what**. Update this file as work progresses — it is the
only place status lives, so a new agent (or a new session) can pick up cold.

Read [README.md](README.md) first for context. The engine architecture this
inventory ports *into* is in [../PARTICLE_EFFECTS.md](../PARTICLE_EFFECTS.md).

## Legend

### Status values

| Status        | Meaning                                                                             |
|---------------|-------------------------------------------------------------------------------------|
| `not-started` | No port work done. Default for everything except `TFlameEffect`.                   |
| `claimed`     | An agent has claimed this row (see Owner). No code yet.                            |
| `wip`         | Code in progress. Engine compiles, but the effect either doesn't render or is wrong. |
| `debug-solid` | Renders in `debug=solid_green` mode (per PARTICLE_EFFECTS §3.2). Bucket + submit path works. |
| `current-frame` | Renders with real UVs but no animation/material. Texture pipeline works.         |
| `normal`      | Renders with intended blend/animation/material. Looks plausible.                   |
| `validated`   | Side-by-side with retail (or with author judgment) confirms parity.                |
| `skip-render` | Audio-only / data-only — no draw output expected.                                  |
| `not-in-1999` | Class exists in source but was disabled / unused in shipped build. Confirm before skipping. |

### Pipeline codes

Maps to engine pipelines per [README.md §4 Phase 1](README.md) and PARTICLE_EFFECTS.md:

| Code  | Pipeline                  | Examples                                |
|-------|---------------------------|-----------------------------------------|
| `FB`  | Flipbook billboard        | torch flame, candle, hit spark          |
| `PE`  | Particle emitter          | blood, sparks, smoke, dust              |
| `SR`  | Strip / ribbon            | sword trail, lightning, wind streak     |
| `MP`  | Mesh projectile           | fireball, ice bolt, photon              |
| `LS`  | Light source              | dynamic light coupling                  |
| `VO`  | Volumetric / overlay      | mist, ice freeze, burn glow             |
| `CX`  | Composite (multi-pipeline)| meteor storm, tornado, waterfall        |
| `-`   | No render output          | ambient sound, infra base class         |

### Column reference

- **Retail src** — original `d:\revenant\*.cpp` filename, derived from REVSYNC fingerprints. Blank if not yet known.
- **Recon** — file under `recon/classes/` where the decompiled retail body lives, or `pending` if waiting on Phase 0.3 Ghidra extraction.
- **Port file** — current target file in `src/`. Most effects live in `src/effect.cpp` / `src/effectcomp.cpp` / `src/missileeffect.cpp` / `src/stripeffect.cpp`.
- **Owner** — name/handle of the agent currently working it, or `-`.
- **Notes** — one-liner: blockers, observations, links to other rows. Keep terse.

## Effects

> **Update this table as you work.** A row is the work unit. If your effect
> naturally splits (e.g. `TFireBallEffect` needs the missile path AND a tail
> ribbon AND a point light), leave the row but break the work down in `Notes`.

### Infrastructure (engine spine — Phase 1, not Phase 2)

These are the engine itself, not effects to port one-by-one. They block all
other rows.

| #   | Class                          | Pipeline | Retail src      | Recon file | Port file                        | Status        | Owner | Notes |
|-----|--------------------------------|----------|-----------------|------------|----------------------------------|---------------|-------|-------|
| I01 | `TEffect` (base)               | -        | effect.cpp      | not-extracted | src/effect.h, src/effect.cpp     | wip           | -     | base class shell exists; needs Ghidra extraction |
| I02 | `TParticleSystem`              | PE infra | effectcomp.cpp  | not-extracted | src/effectcomp.{h,cpp}           | not-started   | -     | engine backbone for PE pipeline; needs Ghidra extraction |
| I03 | `TParticle3DAnimator`          | PE infra | effect.cpp      | not-extracted | src/effect.h                     | not-started   | -     | needs Ghidra extraction |
| I04 | `TSubParticleAnimator`         | PE infra | effectcomp.cpp  | not-extracted | src/effectcomp.h                 | not-started   | -     | sub-particle spawning; needs Ghidra extraction |
| I05 | `TParticleEffectManager`       | PE infra | effect2.cpp     | not-extracted | src/effect2.cpp                  | not-started   | -     | global manager / parser of particle defs; needs Ghidra extraction |
| I06 | `TFlipbookBillboardComponent`  | FB infra | (port-only)     | n/a        | src/effect.h:70                  | done          | phase1-foundation | wired to renderer via `Submit(TRenderer&, const TObjectInstance&)`; uses `EFxDebugMode` + `SBillboardDrawItem`; legacy `SubmitParticleBillboards` shim deleted (Phase 1 spine complete) |
| I07 | FX submission API + bucket batcher | infra | (port-only) | n/a | src/renderer.{h,cpp}, src/shaders/fx.* | done | phase1-foundation | `SubmitFxBillboard / SubmitFxParticleBucket / SubmitFxStrip` + per-pipeline dynamic instance VB + new `fx_pass` after `RunLightingPass`; per-bucket back-to-front sort; per-instance `EFxDebugMode`. Metal shaders authored; GLSL/HLSL stubs filled but not visually validated (Apple-only host). |
| I08 | `--test=vfx` browser harness   | infra    | (port-only)     | n/a        | src/vfxtest.{h,cpp}, src/testmodes.cpp | done | phase1-foundation | data-driven registry, Left/Right cycles effects, R restarts, Space/. pause-step, D and 1–4 cycle debug mode, ESC quits. Four placeholder effects registered (FB / PE / SR / LS); see X14–X17. |

### Fire family

| #   | Class                | Pipeline | Retail src      | Recon file | Port file               | Status        | Owner | Notes |
|-----|----------------------|----------|-----------------|------------|-------------------------|---------------|-------|-------|
| F01 | `TFlameEffect`       | FB       | effect.cpp      | not-extracted | src/effect.cpp:577      | debug-solid   | phase1-foundation | wired through new `TFlipbookBillboardComponent::Submit` -> `SubmitFxBillboard`; in-sector path verified (build clean). Still `debug-solid` because `--test=vfx` uses the X14 placeholder rather than a live `TFlameEffect` instance (needs a real loaded sector to exercise). Phase 2 should drop the placeholder once a real flame is spawned in-test. |
| F02 | `TFlameAnimator`     | FB       | effect.cpp      | not-extracted | src/effect.cpp          | not-started   | -     | bridge animator → TFlameEffect; needs Ghidra extraction |
| F03 | `TFireEffect`        | FB+PE    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | "fire" generic — likely ambient flame patch; needs Ghidra extraction |
| F04 | `TFireAnimator`      | FB       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | bridge → TFireEffect; confirm distinct from TFlameAnimator; needs Ghidra extraction |
| F05 | `TFireSwarmEffect`   | PE       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | swarm of small fire particles; needs Ghidra extraction |
| F06 | `TFaultFireEffect`   | PE       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | "Fault Fire" spell variant; needs Ghidra extraction |
| F07 | `TFireBallEffect`    | IM+PE+LS | MissileEffect.cpp | not-extracted | src/missileeffect.h     | not-started   | -     | I3D mesh ball + ember trail (PE) + dynamic light (LS); **mesh is a direct retail port** (see AGENT_GUIDE §3.5), particles + light through the engine. Needs Ghidra extraction. |
| F08 | `TFireBallAnimator`  | -        | MissileEffect.cpp | not-extracted | src/missileeffect.h     | not-started   | -     | **2026-05-16 extraction-pass DEMOTED:** the existing `cls_0x41cdd0` mapping is almost certainly wrong — that class is a hash/LRU cache (lock/unlock around every op, parallel arrays sized 0x1000/0x2000, LRU counter, no vtable). See recon/mappings/EXTRACTION_PASS_2026-05-16.md. Old yaml kept at recon/mappings/TFireBallAnimator_cls_0x41cdd0_map.yaml for traceability but should be deleted by the next confirmed mapping. |
| F09 | `TFireColumnEffect`  | IM+PE+LS | MissileEffect.cpp | not-extracted | src/missileeffect.h     | not-started   | -     | animated I3D mesh core + ember swirl + light. Mesh = direct retail port. Needs Ghidra extraction. |
| F10 | `TFireColumnAnimator`| -        | MissileEffect.cpp | not-extracted | src/missileeffect.h     | not-started   | -     | bridge → TFireColumnEffect; needs Ghidra extraction |
| F11 | `TFlameDiscEffect`   | IM+PE    | MissileEffect.cpp | not-extracted | src/missileeffect.h     | not-started   | -     | spinning I3D disc mesh + trail particles. Mesh = direct retail port. Needs Ghidra extraction. |

### Ice family

| #   | Class               | Pipeline | Retail src      | Recon file | Port file               | Status        | Owner | Notes |
|-----|---------------------|----------|-----------------|------------|-------------------------|---------------|-------|-------|
| I20 | `TIceEffect`        | PE       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | ice shard burst; needs Ghidra extraction |
| I21 | `TIceBoltEffect`    | IM+SR    | MissileEffect.cpp | not-extracted | src/missileeffect.h     | not-started   | -     | I3D bolt mesh + ice-strip trail. Mesh = direct retail port. Candidate match: cls_0x5aaf28 (see recon/mappings/TIceBoltEffect_cls_0x5aaf28_candidate.yaml). |
| I22 | `TIcedEffect`       | VO       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | "frozen" status overlay on target; needs Ghidra extraction |

### Blood / gore

| #   | Class            | Pipeline | Retail src      | Recon file | Port file               | Status        | Owner | Notes |
|-----|------------------|----------|-----------------|------------|-------------------------|---------------|-------|-------|
| B01 | `TBloodEffect`   | PE       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | spray on hit; needs Ghidra extraction |
| B02 | `TBloodAnimator` | -        | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | bridge; needs Ghidra extraction |
| B03 | `TBloodSystem`   | PE       | effectcomp.cpp  | not-extracted | src/effectcomp.h        | not-started   | -     | particle system specialization for blood; needs Ghidra extraction |
| B04 | `TPulpEffect`    | PE+IM    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | gore + I3D body-part meshes — body parts are direct retail port, blood splash is engine PE. Needs Ghidra extraction. |

### Magic / spells (status & overlay)

| #   | Class           | Pipeline | Retail src      | Recon file | Port file               | Status        | Owner | Notes |
|-----|-----------------|----------|-----------------|------------|-------------------------|---------------|-------|-------|
| M01 | `TAuraEffect`   | VO+LS    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | character aura glow; needs Ghidra extraction |
| M02 | `TAuraAnimator` | -        | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | bridge; needs Ghidra extraction |
| M03 | `THealEffect`   | VO+PE    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | healing spell visual; needs Ghidra extraction |
| M04 | `TBurnEffect`   | VO       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | burn-status overlay; needs Ghidra extraction |
| M05 | `TMistEffect`   | VO       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | **likely Misthaven teleport** — verify via spell registry; needs Ghidra extraction |
| M06 | `TSpellBlock`   | IM+VO    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | I3D shield mesh + overlay glow. Mesh = direct retail port. Needs Ghidra extraction. |
| M07 | `TPhotonEffect` | IM+LS    | MissileEffect.cpp | not-extracted | src/missileeffect.h     | not-started   | -     | photon magic projectile (I3D mesh + light). Mesh = direct retail port. Candidate match: cls_0x5aacac maps to "DragonFire" registry entry (see recon/mappings/TPhotonEffect_cls_0x5aacac_candidate.yaml). |
| M08 | `TPixieEffect`  | PE+LS    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | fairy dust; needs Ghidra extraction |

### Weather / large area

| #   | Class                  | Pipeline | Retail src      | Recon file | Port file               | Status        | Owner | Notes |
|-----|------------------------|----------|-----------------|------------|-------------------------|---------------|-------|-------|
| W01 | `TMeteorStormEffect`   | IM+PE+LS+SR | effect.cpp   | not-extracted | src/effect.h            | not-started   | -     | meteor I3D mesh (per shot) + ember tail + impact-particle burst + flash light + optional strip. Mesh = direct retail port (likely the most ambitious composite in the catalog). Needs Ghidra extraction. |
| W02 | `TTornadoEffect`       | IM?+PE+SR | effect.cpp     | not-extracted | src/effect.h            | not-started   | -     | funnel may be I3D mesh or pure PE (confirm on extraction); dust particles + lift strips are engine. Needs Ghidra extraction. |
| W03 | `TVortexEffect`        | PE+SR    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | spiral magic; needs Ghidra extraction |
| W04 | `TSandswirlEffect`     | PE       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | desert dust; needs Ghidra extraction |
| W05 | `TQuicksandEffect`     | PE       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | hazard surface; needs Ghidra extraction |
| W06 | `TFogAnimator`         | VO       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | volumetric fog; needs Ghidra extraction |
| W07 | `TStormAnimator`       | PE+SR    | effectcomp.cpp  | not-extracted | src/effectcomp.h        | not-started   | -     | base storm rendering; needs Ghidra extraction |

### Water

| #   | Class              | Pipeline | Retail src      | Recon file | Port file               | Status        | Owner | Notes |
|-----|--------------------|----------|-----------------|------------|-------------------------|---------------|-------|-------|
| H01 | `TWaterEffect`     | PE       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | splash; needs Ghidra extraction |
| H02 | `TWaterFallEffect` | CX       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | falling + mist + ripples at base; needs Ghidra extraction |
| H03 | `TRippleEffect`    | FB       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | water ring; needs Ghidra extraction |
| H04 | `TDripEffect`      | PE       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | drips from ceiling / surfaces; needs Ghidra extraction |

### Light / glow

| #   | Class           | Pipeline | Retail src      | Recon file | Port file               | Status        | Owner | Notes |
|-----|-----------------|----------|-----------------|------------|-------------------------|---------------|-------|-------|
| L01 | `TLightSource`  | LS       | lightsource.cpp | not-extracted | (port pending)          | not-started   | -     | **2026-05-16 extraction-pass DEMOTED:** the existing `cls_0x5427f0` mapping is suspect — that class is 332 bytes / 3 fields, its sole method calls TCharacter::method_0x51b580 to attach a light index. Looks like a "light attached to character" helper, not the TLightSource data class. The "HIGH fingerprint match" in class_matches.json was file-size matching (663-byte src file ≈ 663-byte decompiled file) — neither score reflects struct-level evidence. |
| L02 | `THaloEffect`   | FB+LS    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | halo ring; needs Ghidra extraction |
| L03 | `TBallAnimator` | IM+LS    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | glowing I3D sphere mesh + light. Mesh = direct retail port. Needs Ghidra extraction. |

### Strip / trail / projectile-shared

| #   | Class                  | Pipeline | Retail src     | Recon file | Port file               | Status        | Owner | Notes |
|-----|------------------------|----------|----------------|------------|-------------------------|---------------|-------|-------|
| S01 | `TStripEffect`         | SR       | stripeffect.cpp | not-extracted | src/stripeffect.h       | not-started   | -     | base strip — sword slash uses this; needs Ghidra extraction |
| S02 | `TWindStripEffect`     | SR       | stripeffect.cpp | not-extracted | src/stripeffect.h       | not-started   | -     | wind streak; needs Ghidra extraction |
| S03 | `TWindStripAnimator`   | -        | stripeffect.cpp | not-extracted | src/stripeffect.h       | not-started   | -     | bridge; needs Ghidra extraction |
| S04 | `TLightningAnimator`   | SR       | stripeffect.cpp | not-extracted | src/stripeffect.h       | not-started   | -     | **2026-05-16 extraction-pass DEMOTED:** the existing `cls_0x5a47f0` mapping is suspect — that class is allocated by *other* effect classes (e.g. cls_0x5ac340 Ripple candidate) as a generic 0x104-byte animator instance, strongly suggesting cls_0x5a47f0 is `T3DAnimator` (the base) rather than `TLightningAnimator` specifically. See recon/mappings/EXTRACTION_PASS_2026-05-16.md. Old yaml kept for traceability. Note: cls_0x5b0a28 (the class containing the `d:\revenant\Effect3.cpp` assert) references the "LightningStorm" registry string and is a *better* candidate for TLightningAnimator. |
| S05 | `TShockAnimator`       | SR       | effectcomp.cpp  | not-extracted | src/effectcomp.h        | not-started   | -     | shockwave ring; needs Ghidra extraction |
| S06 | `TStripAnimator`       | SR       | effectcomp.cpp  | not-extracted | src/effectcomp.h        | not-started   | -     | generic strip animator; needs Ghidra extraction |
| S07 | `TStreamerEffect`      | SR       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | banner / streamer trail; needs Ghidra extraction |
| S08 | `TMissileEffect`       | IM       | MissileEffect.cpp | not-extracted | src/missileeffect.h     | not-started   | -     | generic I3D-mesh projectile base — parent of F07/F09/F11/I21/M07/L03. Mesh = direct retail port; first composite to port should be this base class so the derived effects share infrastructure. Needs Ghidra extraction. |

### Misc / other

| #   | Class              | Pipeline    | Retail src      | Recon file | Port file               | Status        | Owner | Notes |
|-----|--------------------|-------------|-----------------|------------|-------------------------|---------------|-------|-------|
| X01 | `TFlyEffect`       | PE          | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | flight / levitation; needs Ghidra extraction |
| X02 | `TAmbSoundEffect`  | -           | effect.cpp      | not-extracted | src/effect.h            | skip-render   | -     | audio-only; needs Ghidra extraction |
| X03 | `TFlareAnimator`   | FB+LS       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | flare flash — added during mapping pass; needs Ghidra extraction |
| X04 | `TFountainAnimator`| PE          | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | base fountain particle; added during mapping pass; needs Ghidra extraction |
| X05 | `TBlueFountainAnimator`  | PE    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | blue variant of TFountainAnimator; added during mapping pass; needs Ghidra extraction |
| X06 | `TCyanFountainAnimator`  | PE    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | cyan variant; added during mapping pass; needs Ghidra extraction |
| X07 | `TGreenFountainAnimator` | PE    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | green variant; added during mapping pass; needs Ghidra extraction |
| X08 | `TRedFountainAnimator`   | PE    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | red variant; added during mapping pass; needs Ghidra extraction |
| X09 | `TShieldAnimator`  | VO+mesh     | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | shield bubble visual; added during mapping pass; needs Ghidra extraction |
| X10 | `TSymGlowAnimator` | FB+LS       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | symbol glow; added during mapping pass; needs Ghidra extraction |
| X11 | `TRibbonAnimator`  | SR          | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | trailing ribbon; added during mapping pass; needs Ghidra extraction |
| X12 | `TAmbSoundAnimator`| -           | effect.cpp      | not-extracted | src/effect.h            | skip-render   | -     | bridge → TAmbSoundEffect; added during mapping pass; needs Ghidra extraction |
| X13 | `TTest3DAnimator`  | -           | effect.cpp      | not-extracted | src/effect.h            | not-in-1999   | -     | dev-only 3D test animator; confirm not-shipped; added during mapping pass |
| X14 | placeholder flame (vfx test) | FB | n/a | n/a | src/vfxtest.cpp | wip | phase1-foundation | Phase-1 test entry that exercises the FB pipeline via direct `SubmitFxBillboard`. Replace with a real `TFlameEffect.SpawnForTest` once a standalone-spawn path exists (current `TFlameEffect` needs `T3DImagery` w/ texture slots, not available in the harness). |
| X15 | placeholder smoke (vfx test) | PE | n/a | n/a | src/vfxtest.cpp | wip | phase1-foundation | Phase-1 test entry: creates a `TParticleBucket` directly and drives it via `SubmitFxParticleBucket`. Phase-2 should replace with a real `TBloodEffect` / `TSmokeEffect` port. |
| X16 | placeholder ribbon (vfx test) | SR | n/a | n/a | src/vfxtest.cpp | wip | phase1-foundation | Phase-1 test entry: static 16-segment ribbon via `SubmitFxStrip`. Replace with a real `TStripEffect` Phase-2 port. |
| X17 | placeholder flare (vfx test) | LS | n/a | n/a | src/vfxtest.cpp | wip | phase1-foundation | Phase-1 test entry: pulses a `Renderer->AddPointLight` plus an additive billboard for the glow. Replace with a real `TFlareAnimator` Phase-2 port. |

### Discovery rows (user-named effects with no obvious 1:1 retail class)

Resolve these once Phase 0.3 (Ghidra extraction) completes and we scan retail
for combat/spell trigger points.

| #   | Effect (user terms)        | Likely retail class           | Status        | Notes |
|-----|----------------------------|-------------------------------|---------------|-------|
| D01 | Sword slash trail          | `TStripEffect` attached to weapon during attack frames | not-started | confirm via retail combat code |
| D02 | Combat hit flash           | unknown — possibly short-lived billboard burst or `THaloEffect` | not-started | scan retail hit-resolution code |
| D03 | Blood splat                | `TBloodEffect` / `SBloodSplat` | not-started  | tracked under B01/B03 |
| D04 | Misthaven teleport         | `TMistEffect` (best guess)    | not-started | tracked under M05; verify against spell list |
| D05 | Candle flame               | `TFlameEffect` attached to candle imagery | not-started | tracked under F01 |
| D06 | Bonfire / large flame      | `TFireEffect` / `TFireSwarmEffect` | not-started | tracked under F03/F05 |
| D07 | Effect3.cpp contents       | unknown — retail-confirmed source file (per mapping pass log) | not-started | extract via Ghidra; may add new effect rows once contents known |

## Particle / data structs (reference)

Not rows of their own — these get ported as part of the effect that uses
them. Listed so a porting agent can grep them when needed.

`SParticle`, `SParticleParams`, `SParticleSystemInfo`, `SSubParticleParams`,
`SBloodDrop`, `SBloodSplat`, `SBloodParticle`, `SBloodSystemParams`,
`SFlameParticle`, `SIceChunkParticle`, `SWaterParticle`, `STornadoParticle`,
`SStreamerParticle`, `SSmoke`, `SDropParticle`, `SDropSparkSystem`,
`SShockParam`, `SStormParams`, `SStormInstance`, `FireBallData`, `SLightDef`.

Source-of-truth headers (current port): [src/effect.h](../../src/effect.h),
[src/effectcomp.h](../../src/effectcomp.h),
[src/stripeffect.h](../../src/stripeffect.h),
[src/missileeffect.h](../../src/missileeffect.h).

## Spell registry (reference)

The spell list maps player-facing names to effect-class instances. Tracked
separately for in-game wiring (Phase 3). Classes involved: `TSpell`,
`TSpellList`, `TSpellManager`, `SSpellData`, `SSpellVariant`. Source: retail
`spell.cpp`/`spell.h`. The script-side spell definitions live in
[../../data/Script/](../../data/Script/).

## Mapping pass log

### 2026-05-16 — initial cross-reference pass

- **Rows resolved (`pending` → real recon file): 3.**
  - F08 `TFireBallAnimator` → `cls_0x41cdd0.cpp` (PERFECT fingerprint match in `recon/mappings/TFireBallAnimator_cls_0x41cdd0_map.yaml`).
  - S04 `TLightningAnimator` → `cls_0x5a47f0.cpp` (HIGH fingerprint match in `recon/mappings/TLightningAnimator_cls_0x5a47f0_map.yaml`).
  - L01 `TLightSource` → `cls_0x5427f0.cpp` (small 332-byte struct, matches `lightsource.cpp`'s size profile per `class_matches.json`).
- **Rows marked `not-extracted`: all remaining effect rows (~60).** No further effect/animator classes have a confirmed Ghidra-class-ID mapping in `recon/mappings/`, `recon/analysis/manual_verified_matches.json`, or `recon/classes_readable/`. Phase 0.3 Ghidra extraction is still required to surface them.
- **New rows added during pass: 11.** From `recon/analysis/all_source_classes.txt`, the following effect-family classes were declared in pre-release `effect.h` but absent from the previous INVENTORY. Added as X03–X13:
  - `TFlareAnimator`, `TFountainAnimator` (+ Blue/Cyan/Green/Red variants), `TShieldAnimator`, `TSymGlowAnimator`, `TRibbonAnimator`, `TAmbSoundAnimator`, `TTest3DAnimator`.
- **Retail-src name updates.** Where the d:\\revenant\\ assertion strings in `recon/classes/_data.txt` confirm a retail filename, the column was normalized to match the retail casing:
  - `missileeffect.cpp` → **`MissileEffect.cpp`** (verified via `s_d:\revenant\MissileEffect.cpp_*` strings).
  - `effect3.cpp` is a real retail filename (`s_d:\revenant\Effect3.cpp_*`) — no inventory row currently targets it; flagged below.
- **Surprises / things to flag:**
  - `recon/docs/CLASS_MAPPING.md` claims `cls_0x5a7e38 → src/effect.cpp` based on file-size analysis. Inspection contradicts this: `cls_0x5a7e38` is a ~3964-byte struct whose methods script-post "effect"/"deleteeffect" events and load shadow/comflash/comring/vision/flamef imagery — that profile matches a **character / TObjectInstance-derived** class, not `TEffect` itself. Do **not** trust the CLASS_MAPPING entry for effect-row recon work.
  - Similarly `cls_0x5a7b98` carries strings like "blood"/"onfire"/"equipeffect"/"maxhealth", but its methods are TCharacter behaviour (decompiled as `cls_0x5a7b98_TCharacter`). It is the character that consumes these effect-name strings, not an effect base class.
  - **`d:\revenant\` assertion strings observed for effect-family source files:** only `MissileEffect.cpp` (3 hits) and `Effect3.cpp` (1 hit). `effect.cpp`, `effectcomp.cpp`, `stripeffect.cpp`, `effect2.cpp`, `lightsource.cpp` produce no asserts in the retail binary, so the retail-src filenames for their rows remain assumptions, not fingerprints.
  - **`Effect3.cpp` has no INVENTORY row.** It is a retail-confirmed source file (assert string + a `src/effect3.cpp` exists in the pre-release tree) but its class contents are unknown without extraction. May correspond to one of the Phase-0.3 unknowns; revisit after extraction.
  - **Retail string `StripEffect`** (the bare name, not full path) is referenced at `00522040` — likely a script-side type tag rather than a source-file string, but it confirms the strip-effect subsystem ships.
  - The pre-release source class list (`recon/analysis/all_source_classes.txt`) shows several `*Animator` classes paired with their `*Effect` (e.g. `THaloAnimator`/`THaloEffect`, `TIceAnimator`/`TIceEffect`, `TIceBoltAnimator`/`TIceBoltEffect`, `TBurnAnimator`/`TBurnEffect`, etc.) that are not separately rowed in INVENTORY because the Effect row stands in for the pair. Porting agents should remember to bring the matching Animator along.
