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
| `IM`  | I3D mesh effect component | bone-animated mesh owned by an effect (meteor body, fire-column core, tornado funnel, dragon-fire). Uses existing renderer mesh path via `SMeshSubmit` / skinned-mesh path; **not** a new sokol pipeline. |
| `LS`  | Light source              | dynamic light coupling                  |
| `VO`  | Volumetric / overlay      | mist, ice freeze, burn glow             |
| `CX`  | Composite (multi-pipeline)| meteor storm, tornado, waterfall — typically `IM + PE + LS` |
| `-`   | No render output          | ambient sound, infra base class         |

> **IM is not a new renderer pipeline.** It denotes an effect class whose
> primary visual is an I3D bone-animated mesh — the renderer already supports
> these via `SMeshSubmit` / the existing skinned-mesh path. The effect class
> owns the I3D instance, advances its animation, and composes it with
> particles / strips / lights through the VFX engine. See
> [AGENT_GUIDE.md §3.5](AGENT_GUIDE.md) (dual-track porting).

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
| I01 | `TEffect` (base)               | -        | effect.cpp      | recon/classes/cls_0x5a47f0.cpp | src/effect.h, src/effect.cpp     | wip           | -     | **2026-05-16 pass-2:** mapped MEDIUM-HIGH to cls_0x5a47f0 (60-byte slim base; ctor takes TCharacter*; allocated fresh by every leaf effect's virtual creator with covariant return). See recon/mappings/TEffect_cls_0x5a47f0_candidate.yaml. Supersedes the demoted "cls_0x5a47f0 = TLightningAnimator" mapping. |
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
| F02 | `TFlameAnimator`     | FB       | effect.cpp      | recon/classes/cls_0x5a9d88.cpp | src/effect.cpp          | not-started   | -     | **2026-05-16 pass-2:** MEDIUM candidate cls_0x5a9d88 via "FLAME" loader-keyword XREF (single XREF, so weaker than 2-XREF registry matches). See recon/mappings/TFlameAnimator_cls_0x5a9d88_candidate.yaml. |
| F03 | `TFireEffect`        | FB+PE    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | "fire" generic — likely ambient flame patch; needs Ghidra extraction |
| F04 | `TFireAnimator`      | FB       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | bridge → TFireEffect; confirm distinct from TFlameAnimator; needs Ghidra extraction |
| F05 | `TFireSwarmEffect`   | PE       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | swarm of small fire particles; needs Ghidra extraction |
| F06 | `TFaultFireEffect`   | PE       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | "Fault Fire" spell variant; needs Ghidra extraction |
| F07 | `TFireBallEffect`    | IM+PE+LS | MissileEffect.cpp | recon/classes/cls_0x5b4290.cpp | src/missileeffect.h     | not-started   | -     | **2026-05-16 pass-2:** MEDIUM candidate cls_0x5b4290 via FireBall registry @0x5e1b28 + FIRESHOOT/FIREHIT event names. Also handles "FIRECOLUMN" so may share/merge with TFireColumnEffect. Inherits cls_0x5a50e8 (MissileEffect base, NOT TEffect). See recon/mappings/TFireBallEffect_cls_0x5b4290_candidate.yaml. |
| F08 | `TFireBallAnimator`  | -        | MissileEffect.cpp | not-extracted | src/missileeffect.h     | not-started   | -     | **2026-05-16 extraction-pass DEMOTED:** the existing `cls_0x41cdd0` mapping is almost certainly wrong — that class is a hash/LRU cache (lock/unlock around every op, parallel arrays sized 0x1000/0x2000, LRU counter, no vtable). See recon/mappings/EXTRACTION_PASS_2026-05-16.md. Old yaml kept at recon/mappings/TFireBallAnimator_cls_0x41cdd0_map.yaml for traceability but should be deleted by the next confirmed mapping. |
| F09 | `TFireColumnEffect`  | IM+PE+LS | MissileEffect.cpp | recon/classes/cls_0x5b4534.cpp | src/missileeffect.h     | not-started   | -     | **2026-05-16 pass-2:** MEDIUM candidate cls_0x5b4534 via "FireColumn" registry @0x5e1b40 + MissileEffect.cpp assert string lands in this class's fn 0x512740 (CONFIRMS the source file). Also touches FLAMEDISC. See recon/mappings/TFireColumnEffect_cls_0x5b4534_candidate.yaml. |
| F10 | `TFireColumnAnimator`| -        | MissileEffect.cpp | not-extracted | src/missileeffect.h     | not-started   | -     | bridge → TFireColumnEffect; needs Ghidra extraction |
| F11 | `TFlameDiscEffect`   | IM+PE    | MissileEffect.cpp | recon/classes/cls_0x5b45ac.cpp | src/missileeffect.h     | not-started   | -     | **2026-05-16 pass-2:** MEDIUM candidate cls_0x5b45ac via FLAMEDISC registry @0x5e1b4c second XREF. See recon/mappings/TFlameDiscEffect_cls_0x5b45ac_candidate.yaml. |

### Ice family

| #   | Class               | Pipeline | Retail src      | Recon file | Port file               | Status        | Owner | Notes |
|-----|---------------------|----------|-----------------|------------|-------------------------|---------------|-------|-------|
| I20 | `TIceEffect`        | PE       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | ice shard burst; needs Ghidra extraction |
| I21 | `TIceBoltEffect`    | IM+SR    | MissileEffect.cpp | not-extracted | src/missileeffect.h     | not-started   | -     | I3D bolt mesh + ice-strip trail. Mesh = direct retail port. Candidate match: cls_0x5aaf28 (see recon/mappings/TIceBoltEffect_cls_0x5aaf28_candidate.yaml). |
| I22 | `TIcedEffect`       | VO       | effect.cpp      | (no class) | src/effect.h            | not-started   | -     | **2026-05-16 pass-2:** "Iced" XREFs split across cls_0x5aaf28 (TIceBoltEffect) and cls_0x5ab460 (TStormAnimator) -- it's a status tag posted by other effects, NOT a standalone TEffect class. Likely script-side / character imagery overlay only. See recon/mappings/TIcedEffect_cls_0x5aaf28_note.yaml. |

### Blood / gore

| #   | Class            | Pipeline | Retail src      | Recon file | Port file               | Status        | Owner | Notes |
|-----|------------------|----------|-----------------|------------|-------------------------|---------------|-------|-------|
| B01 | `TBloodEffect`   | PE       | effect.cpp      | recon/classes/cls_0x5acaa8.cpp | src/effect.h            | not-started   | -     | **2026-05-16 pass-2:** MEDIUM candidate cls_0x5acaa8 via Blood registry @0x5e11e8. WARNING: this Ghidra file is a 3-class merge (Blood + Mist + WaterFall); needs split before clean port. See recon/mappings/TBloodEffect_cls_0x5acaa8_candidate.yaml. |
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
| M05 | `TMistEffect`   | VO       | effect.cpp      | recon/classes/cls_0x5acaa8.cpp (merged) | src/effect.h            | not-started   | -     | **2026-05-16 pass-2:** Mist + MistFog XREFs land in cls_0x5acaa8 (the 3-class Blood/Mist/WaterFall merge). Likely the 412-byte secondary class in that file. See recon/mappings/TBloodEffect_cls_0x5acaa8_candidate.yaml for the merge details. |
| M06 | `TSpellBlock`   | IM+VO    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | I3D shield mesh + overlay glow. Mesh = direct retail port. Needs Ghidra extraction. |
| M07 | `TPhotonEffect` | IM+LS    | MissileEffect.cpp | recon/classes/cls_0x5b401c.cpp | src/missileeffect.h     | not-started   | -     | **2026-05-16 pass-2 CORRECTED:** the real TPhotonEffect is cls_0x5b401c (Photon registry @0x5e1b20, MissileEffect.cpp family, inherits cls_0x5a50e8). The prior cls_0x5aacac mapping was the "DragonFire" registry entry (now relabeled TDragonFireEffect — see recon/mappings/TDragonFireEffect_cls_0x5aacac_candidate.yaml). Current mapping: recon/mappings/TPhotonEffect_cls_0x5b401c_candidate.yaml. |
| M08 | `TPixieEffect`  | PE+LS    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | fairy dust; needs Ghidra extraction |

### Weather / large area

| #   | Class                  | Pipeline | Retail src      | Recon file | Port file               | Status        | Owner | Notes |
|-----|------------------------|----------|-----------------|------------|-------------------------|---------------|-------|-------|
| W01 | `TMeteorStormEffect`   | IM+PE+LS+SR | effect.cpp   | not-extracted | src/effect.h            | not-started   | -     | meteor I3D mesh (per shot) + ember tail + impact-particle burst + flash light + optional strip. Mesh = direct retail port (likely the most ambitious composite in the catalog). Needs Ghidra extraction. |
| W02 | `TTornadoEffect`       | IM?+PE+SR | effect.cpp     | recon/classes/cls_0x5ab460.cpp (merged) | src/effect.h            | not-started   | -     | **2026-05-16 pass-2:** Tornado XREFs land in cls_0x5ab460 (the TStormAnimator merge that also hosts Sandswirl + Quicksand registrations). May actually be the same templated class as W04/W05. See recon/mappings/TStormAnimator_cls_0x5ab460_candidate.yaml. |
| W03 | `TVortexEffect`        | PE+SR    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | spiral magic; needs Ghidra extraction |
| W04 | `TSandswirlEffect`     | PE       | effect.cpp      | recon/classes/cls_0x5ab460.cpp (merged) | src/effect.h            | not-started   | -     | **2026-05-16 pass-2:** Sandswirl XREFs land in cls_0x5ab460 (TStormAnimator merge). See W02. |
| W05 | `TQuicksandEffect`     | PE       | effect.cpp      | recon/classes/cls_0x5ab460.cpp (merged) | src/effect.h            | not-started   | -     | **2026-05-16 pass-2:** Quicksand XREFs land in cls_0x5ab460 (TStormAnimator merge). See W02. |
| W06 | `TFogAnimator`         | VO       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | volumetric fog; needs Ghidra extraction |
| W07 | `TStormAnimator`       | PE+SR    | effectcomp.cpp  | recon/classes/cls_0x5ab460.cpp | src/effectcomp.h        | not-started   | -     | **2026-05-16 pass-2:** MEDIUM candidate cls_0x5ab460 (2064 bytes, hosts Tornado/Sandswirl/Quicksand under one templated storm class + an embedded SStormInstance helper that needs Ghidra-rerun to split). See recon/mappings/TStormAnimator_cls_0x5ab460_candidate.yaml. |

### Water

| #   | Class              | Pipeline | Retail src      | Recon file | Port file               | Status        | Owner | Notes |
|-----|--------------------|----------|-----------------|------------|-------------------------|---------------|-------|-------|
| H01 | `TWaterEffect`     | PE       | effect.cpp      | recon/classes/cls_0x5ad26c.cpp | src/effect.h            | not-started   | -     | **2026-05-16 pass-2:** MEDIUM candidate cls_0x5ad26c via Water registry @0x5e120c (2 XREFs both in this class). See recon/mappings/TWaterEffect_cls_0x5ad26c_candidate.yaml. |
| H02 | `TWaterFallEffect` | CX       | effect.cpp      | recon/classes/cls_0x5acaa8.cpp (merged) | src/effect.h            | not-started   | -     | **2026-05-16 pass-2:** WaterFall XREFs land in cls_0x5acaa8 (the Blood/Mist/WaterFall 3-class merge). See B01. |
| H03 | `TRippleEffect`    | FB       | effect.cpp      | recon/classes/cls_0x5ac340.cpp | src/effect.h            | not-started   | -     | Already in pass-1: MEDIUM cls_0x5ac340 via Ripple registry @0x5e11cc. See recon/mappings/TRippleEffect_cls_0x5ac340_candidate.yaml. |
| H04 | `TDripEffect`      | PE       | effect.cpp      | recon/classes/cls_0x5ac5b0.cpp | src/effect.h            | not-started   | -     | **2026-05-16 pass-2:** MEDIUM candidate cls_0x5ac5b0 via Drip registry @0x5e11d4. Class also handles two "ripple" XREFs (drip drops produce ripples). See recon/mappings/TDripEffect_cls_0x5ac5b0_candidate.yaml. |

### Light / glow

| #   | Class           | Pipeline | Retail src      | Recon file | Port file               | Status        | Owner | Notes |
|-----|-----------------|----------|-----------------|------------|-------------------------|---------------|-------|-------|
| L01 | `TLightSource`  | LS       | lightsource.cpp | not-extracted | (port pending)          | not-started   | -     | **2026-05-16 extraction-pass DEMOTED:** the existing `cls_0x5427f0` mapping is suspect — that class is 332 bytes / 3 fields, its sole method calls TCharacter::method_0x51b580 to attach a light index. Looks like a "light attached to character" helper, not the TLightSource data class. The "HIGH fingerprint match" in class_matches.json was file-size matching (663-byte src file ≈ 663-byte decompiled file) — neither score reflects struct-level evidence. |
| L02 | `THaloEffect`   | FB+LS    | effect.cpp      | recon/classes/cls_0x5ac0cc.cpp | src/effect.h            | not-started   | -     | **2026-05-16 pass-2:** MEDIUM candidate cls_0x5ac0cc via Halo registry @0x5e11c4 (2 XREFs both in this class). See recon/mappings/THaloEffect_cls_0x5ac0cc_candidate.yaml. |
| L03 | `TBallAnimator` | IM+LS    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | glowing I3D sphere mesh + light. Mesh = direct retail port. Needs Ghidra extraction. |

### Strip / trail / projectile-shared

| #   | Class                  | Pipeline | Retail src     | Recon file | Port file               | Status        | Owner | Notes |
|-----|------------------------|----------|----------------|------------|-------------------------|---------------|-------|-------|
| S01 | `TStripEffect`         | SR       | stripeffect.cpp | not-extracted | src/stripeffect.h       | not-started   | -     | base strip — sword slash uses this; needs Ghidra extraction |
| S02 | `TWindStripEffect`     | SR       | stripeffect.cpp | not-extracted | src/stripeffect.h       | not-started   | -     | wind streak; needs Ghidra extraction |
| S03 | `TWindStripAnimator`   | -        | stripeffect.cpp | not-extracted | src/stripeffect.h       | not-started   | -     | bridge; needs Ghidra extraction |
| S04 | `TLightningAnimator`   | SR       | Effect3.cpp     | recon/classes/cls_0x5b0a28.cpp | src/stripeffect.h       | not-started   | -     | **2026-05-16 pass-2 RESOLVED:** the cls_0x5b0a28 hypothesis from pass-1 is CONFIRMED as MEDIUM-HIGH candidate. cls_0x5b0a28 (1400 bytes, 5 methods) hosts the "LightningStorm" registry XREF + ENERGYSPRAY + "rhand" XREFs + the d:\revenant\Effect3.cpp assert string -- all inside its single registrar fn 0x502b70. The state machine in that fn (mbr_0x184 counter, mbr_0x188 timer with values 0x3c/0x32/0x19) matches a strike-cycle pattern. **Retail src corrected to Effect3.cpp** (not stripeffect.cpp). The pass-1 cls_0x5a47f0 mapping was demoted; that class is the TEffect base itself (see I01). See recon/mappings/TLightningAnimator_cls_0x5b0a28_candidate.yaml. |
| S05 | `TShockAnimator`       | SR       | effectcomp.cpp  | not-extracted | src/effectcomp.h        | not-started   | -     | shockwave ring; needs Ghidra extraction |
| S06 | `TStripAnimator`       | SR       | effectcomp.cpp  | not-extracted | src/effectcomp.h        | not-started   | -     | generic strip animator; needs Ghidra extraction |
| S07 | `TStreamerEffect`      | SR       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | banner / streamer trail; needs Ghidra extraction |
| S08 | `TMissileEffect`       | IM       | MissileEffect.cpp | recon/classes/cls_0x5b3c18.cpp | src/missileeffect.h     | not-started   | -     | **2026-05-16 pass-2:** MEDIUM candidate cls_0x5b3c18 -- shared base for the MissileEffect family (Photon + FireBall + lightning sound-key all XREF here). Inherits cls_0x5a50e8 (root animation class). See recon/mappings/TMissileEffect_cls_0x5b3c18_candidate.yaml. |

### Misc / other

| #   | Class              | Pipeline    | Retail src      | Recon file | Port file               | Status        | Owner | Notes |
|-----|--------------------|-------------|-----------------|------------|-------------------------|---------------|-------|-------|
| X01 | `TFlyEffect`       | PE          | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | flight / levitation; needs Ghidra extraction |
| X02 | `TAmbSoundEffect`  | -           | effect.cpp      | recon/classes/cls_0x5ad9d4.cpp | src/effect.h            | skip-render   | -     | **2026-05-16 pass-2:** MEDIUM candidate cls_0x5ad9d4 via "Speaker" registry @0x5e1224 (in-engine name for the ambient-sound effect). See recon/mappings/TAmbSoundEffect_cls_0x5ad9d4_candidate.yaml. |
| X03 | `TFlareAnimator`   | FB+LS       | effect.cpp      | recon/classes/cls_0x5a9f2c.cpp | src/effect.h            | not-started   | -     | **2026-05-16 pass-2:** MEDIUM candidate cls_0x5a9f2c via "IrisFlare" loader keyword + "SetVortex" shared XREF. See recon/mappings/TFlareAnimator_cls_0x5a9f2c_candidate.yaml. |
| X04 | `TFountainAnimator`| PE          | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | base fountain particle; added during mapping pass; needs Ghidra extraction |
| X05 | `TBlueFountainAnimator`  | PE    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | blue variant of TFountainAnimator; added during mapping pass; needs Ghidra extraction |
| X06 | `TCyanFountainAnimator`  | PE    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | cyan variant; added during mapping pass; needs Ghidra extraction |
| X07 | `TGreenFountainAnimator` | PE    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | green variant; added during mapping pass; needs Ghidra extraction |
| X08 | `TRedFountainAnimator`   | PE    | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | red variant; added during mapping pass; needs Ghidra extraction |
| X09 | `TShieldAnimator`  | VO+mesh     | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | shield bubble visual; added during mapping pass; needs Ghidra extraction |
| X10 | `TSymGlowAnimator` | FB+LS       | effect.cpp      | recon/classes/cls_0x5a9e50.cpp | src/effect.h            | not-started   | -     | **2026-05-16 pass-2:** MEDIUM candidate cls_0x5a9e50 via "SymGlow" + "sparks" XREFs. See recon/mappings/TSymGlowAnimator_cls_0x5a9e50_candidate.yaml. |
| X11 | `TRibbonAnimator`  | SR          | effect.cpp      | recon/classes/cls_0x5b9fac.cpp | src/effect.h            | not-started   | -     | **2026-05-16 pass-2:** MEDIUM candidate cls_0x5b9fac via "Ribbon" XREF + per-color FONT loader keywords (CYANFONT/REDFONT/GREENFONT/BLUEFONT all XREF here). See recon/mappings/TRibbonAnimator_cls_0x5b9fac_candidate.yaml. |
| X12 | `TAmbSoundAnimator`| -           | effect.cpp      | not-extracted | src/effect.h            | skip-render   | -     | bridge → TAmbSoundEffect; added during mapping pass; needs Ghidra extraction |
| X13 | `TTest3DAnimator`  | -           | effect.cpp      | not-extracted | src/effect.h            | not-in-1999   | -     | dev-only 3D test animator; confirm not-shipped; added during mapping pass |
| X18 | `TArrowEffect`     | PE          | effect.cpp?     | recon/classes/cls_0x5b0cfc.cpp | (new — needs port file) | not-started   | -     | **2026-05-16 pass-2 NEW ROW:** arrow-impact effect. cls_0x5b0cfc (288 bytes) via "ArrowEffect" + "rhand" XREFs. See recon/mappings/TArrowEffect_cls_0x5b0cfc_candidate.yaml. |
| X19 | `TYFireBallEffect` | IM+PE+LS    | MissileEffect.cpp | recon/classes/cls_0x5b4814.cpp | (new — needs port file) | not-started   | -     | **2026-05-16 pass-2 NEW ROW:** Yhagoro-NPC-specific fireball variant. cls_0x5b4814 (732 bytes, alloc 0x4cc same as TFireBallEffect F07). Likely a subclass with overridden behaviour. See recon/mappings/TYFireBallEffect_cls_0x5b4814_candidate.yaml. |
| X20 | `TDragonFireEffect`| FB+PE       | effect.cpp?     | recon/classes/cls_0x5aacac.cpp | (new — needs port file) | not-started   | -     | **2026-05-16 pass-2 NEW ROW:** Dragon-NPC fire attack. cls_0x5aacac (376 bytes) via "DragonFire" registry @0x5e1170. Source-side class name is conjectural (no `TDragonFireEffect` declared in pre-release tree); may be a parameterized TFireBallEffect/TFireSwarmEffect. See recon/mappings/TDragonFireEffect_cls_0x5aacac_candidate.yaml. |
| X21 | `TFizzleEffect`?   | PE          | (likely script) | recon/classes/cls_0x5ad758.cpp | (new — needs port file) | not-started   | -     | **2026-05-16 pass-2 NEW ROW:** spell-fail visual. cls_0x5ad758 (272 bytes) via "Fizzle" registry @0x5e121c. LOW-MEDIUM since no `TFizzleEffect` class in source -- may be a generic TParticleSystem driven by script data. See recon/mappings/TFizzleEffect_cls_0x5ad758_candidate.yaml. |
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
| D07 | Effect3.cpp contents       | `TLightningAnimator` (S04) — pass-2 RESOLVED | resolved | cls_0x5b0a28 is the Effect3.cpp class — assert string lands in its only registrar fn. The 1400-byte size suggests Effect3.cpp may only contain TLightningAnimator alone (large standalone storm class). |

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

### 2026-05-16 — extraction pass 2 (XREF continuation)

Continues the methodology from `recon/mappings/EXTRACTION_PASS_2026-05-16.md`
(XREF-driven, function-start index, no Ghidra rerun). All deliverables in
`recon/mappings/*_candidate.yaml`.

- **Rows resolved (`not-extracted` → recon file): 17.**
  - **I01** `TEffect` (base) → `cls_0x5a47f0.cpp` — MEDIUM-HIGH; ctor takes
    `TCharacter*`, allocated fresh by every leaf effect's virtual creator.
    *Supersedes the pass-1 demoted "cls_0x5a47f0 = TLightningAnimator".*
  - **F02** `TFlameAnimator` → `cls_0x5a9d88.cpp` — MEDIUM (FLAME loader kw).
  - **F07** `TFireBallEffect` → `cls_0x5b4290.cpp` — MEDIUM (FireBall +
    FIRESHOOT/FIREHIT). Inherits `cls_0x5a50e8`, NOT `cls_0x5a47f0`.
  - **F09** `TFireColumnEffect` → `cls_0x5b4534.cpp` — MEDIUM. **MissileEffect.cpp
    assert string lives in this class's fn 0x512740** — confirms the
    source-file fingerprint.
  - **F11** `TFlameDiscEffect` → `cls_0x5b45ac.cpp` — MEDIUM.
  - **B01** `TBloodEffect` → `cls_0x5acaa8.cpp` *(merged)* — MEDIUM. Ghidra
    has clustered Blood + Mist + WaterFall into one file; needs split.
  - **M05** `TMistEffect` → `cls_0x5acaa8.cpp` *(merged)* — same merge.
  - **M07** `TPhotonEffect` → `cls_0x5b401c.cpp` — MEDIUM. **CORRECTS pass-1**:
    the cls_0x5aacac mapping was the "DragonFire" registry, not Photon.
  - **L02** `THaloEffect` → `cls_0x5ac0cc.cpp` — MEDIUM.
  - **S04** `TLightningAnimator` → `cls_0x5b0a28.cpp` — MEDIUM-HIGH.
    **RESOLVES pass-1 hypothesis**: cls_0x5b0a28 is Effect3.cpp's class,
    confirmed via assert string + LightningStorm + ENERGYSPRAY XREFs. Retail
    src corrected from `stripeffect.cpp` to `Effect3.cpp`.
  - **S08** `TMissileEffect` → `cls_0x5b3c18.cpp` — MEDIUM (shared
    MissileEffect base).
  - **W02/W04/W05** `TTornadoEffect/TSandswirlEffect/TQuicksandEffect` →
    `cls_0x5ab460.cpp` *(merged)* — all three names register here; likely
    one templated `TStormAnimator` + an embedded `SStormInstance` helper.
  - **W07** `TStormAnimator` → `cls_0x5ab460.cpp` (same file) — MEDIUM.
  - **H01** `TWaterEffect` → `cls_0x5ad26c.cpp` — MEDIUM.
  - **H02** `TWaterFallEffect` → `cls_0x5acaa8.cpp` *(merged)* — same Blood
    merge.
  - **H04** `TDripEffect` → `cls_0x5ac5b0.cpp` — MEDIUM.
  - **X02** `TAmbSoundEffect` → `cls_0x5ad9d4.cpp` — MEDIUM ("Speaker" tag).
  - **X03** `TFlareAnimator` → `cls_0x5a9f2c.cpp` — MEDIUM ("IrisFlare").
  - **X10** `TSymGlowAnimator` → `cls_0x5a9e50.cpp` — MEDIUM.
  - **X11** `TRibbonAnimator` → `cls_0x5b9fac.cpp` — MEDIUM.

- **Rows refuted / re-attributed: 1.**
  - The pass-1 `TPhotonEffect → cls_0x5aacac` mapping was wrong — that
    class registers "DragonFire", not "Photon". The yaml has been
    *renamed* to `TDragonFireEffect_cls_0x5aacac_candidate.yaml` and the
    real TPhotonEffect candidate is `cls_0x5b401c`.

- **Rows confirmed from pass-1: 1.**
  - S04 `TLightningAnimator`'s pass-1 hint that `cls_0x5b0a28` is the
    better candidate than the demoted `cls_0x5a47f0` is CONFIRMED.

- **I22 re-evaluated: 1.** `TIcedEffect` has no dedicated class — "Iced"
  is a status tag posted by TIceBoltEffect + TStormAnimator. Recon-file
  set to "(no class)".

- **D07 (Effect3.cpp contents) RESOLVED.** It's TLightningAnimator's
  source file. 1400-byte class suggests Effect3.cpp may contain only
  this one class.

- **New rows added: 5.**
  - X18 `TArrowEffect` → cls_0x5b0cfc (arrow-impact effect, not previously
    in INVENTORY).
  - X19 `TYFireBallEffect` → cls_0x5b4814 (Yhagoro-NPC fireball variant).
  - X20 `TDragonFireEffect` → cls_0x5aacac (Dragon-NPC fire attack; name
    conjectural — no `TDragonFireEffect` in source).
  - X21 `TFizzleEffect`? → cls_0x5ad758 (spell-fail visual; likely a
    generic TParticleSystem, not a dedicated class).

- **Notable methodology finds:**
  - The effect-name **registry strings extend past 0x5e1224** (where
    pass-1 stopped) into the missile-family cluster at `005e1a00..005e1c00`
    — Photon, FireBall, FIRECOLUMN, FireColumn, FLAMEDISC, YFireBall all
    live here. This range was missed in pass-1, which explains why the
    Photon mapping went wrong: pass-1 only had "DragonFire" as a Photon
    candidate, not the real "Photon" string.
  - **Two distinct effect base hierarchies confirmed:**
    1. `TEffect` (cls_0x5a47f0) → most spell/area effects (Halo, Drip,
       Water, IceBolt, Streamer, FireSwarm, FaultFire, Ripple, Pixie,
       Blood, Mist, Tornado, Sandswirl, Quicksand, Speaker, Fizzle, etc.)
    2. `cls_0x5a50e8` (unknown — possibly TObjectInstance or T3DImage
       family) → MissileEffect family (Photon, FireBall, FireColumn,
       FlameDisc, YFireBall, TMissileEffect base). These do NOT derive
       from TEffect. The Ghidra `cls_0x5a50e8` file is itself 276 bytes
       with 107 functions — almost certainly a misattributed multi-class
       blob; its true identity is still OPEN.
  - **Ghidra-merged effect classes** (one file = multiple distinct
    classes that share .rdata vtable proximity):
    - cls_0x5ab460  (Tornado + Sandswirl + Quicksand + 2nd helper class)
    - cls_0x5acaa8  (Blood + Mist + WaterFall, 2 vtables seen)
    - cls_0x5a9194  (FireFlash + FireWind + BURN; 13504-byte mega-merge)
    All three need a Ghidra rerun with stricter scoping before clean
    per-class extraction is possible.

- **Files produced in this pass:**
  - `recon/mappings/TEffect_cls_0x5a47f0_candidate.yaml`
  - `recon/mappings/TLightningAnimator_cls_0x5b0a28_candidate.yaml`
  - `recon/mappings/THaloEffect_cls_0x5ac0cc_candidate.yaml`
  - `recon/mappings/TWaterEffect_cls_0x5ad26c_candidate.yaml`
  - `recon/mappings/TDripEffect_cls_0x5ac5b0_candidate.yaml`
  - `recon/mappings/TArrowEffect_cls_0x5b0cfc_candidate.yaml`
  - `recon/mappings/TFlameAnimator_cls_0x5a9d88_candidate.yaml`
  - `recon/mappings/TSymGlowAnimator_cls_0x5a9e50_candidate.yaml`
  - `recon/mappings/TFlareAnimator_cls_0x5a9f2c_candidate.yaml`
  - `recon/mappings/TRibbonAnimator_cls_0x5b9fac_candidate.yaml`
  - `recon/mappings/TAmbSoundEffect_cls_0x5ad9d4_candidate.yaml`
  - `recon/mappings/TFizzleEffect_cls_0x5ad758_candidate.yaml`
  - `recon/mappings/TMissileEffect_cls_0x5b3c18_candidate.yaml`
  - `recon/mappings/TPhotonEffect_cls_0x5b401c_candidate.yaml`
  - `recon/mappings/TFireBallEffect_cls_0x5b4290_candidate.yaml`
  - `recon/mappings/TFireColumnEffect_cls_0x5b4534_candidate.yaml`
  - `recon/mappings/TFlameDiscEffect_cls_0x5b45ac_candidate.yaml`
  - `recon/mappings/TYFireBallEffect_cls_0x5b4814_candidate.yaml`
  - `recon/mappings/TStormAnimator_cls_0x5ab460_candidate.yaml`
  - `recon/mappings/TBloodEffect_cls_0x5acaa8_candidate.yaml`
  - `recon/mappings/TFireFlashEffect_cls_0x5a9194_candidate.yaml`
  - `recon/mappings/TIcedEffect_cls_0x5aaf28_note.yaml`  (informational)
  - `recon/mappings/TDragonFireEffect_cls_0x5aacac_candidate.yaml`
    (renamed from `TPhotonEffect_cls_0x5aacac_candidate.yaml`)
  - `recon/mappings/EXTRACTION_PASS_2_2026-05-16.md`  (methodology delta)

  No `rename_classes.py` run — all yamls remain `_candidate` (MEDIUM/
  MEDIUM-HIGH); promote only after source-side struct compare.
