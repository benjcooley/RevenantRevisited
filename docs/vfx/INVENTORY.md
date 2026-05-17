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
| I08 | `--test=vfx` browser harness   | infra    | (port-only)     | n/a        | src/vfxtest.{h,cpp}, src/testmodes.cpp | done | phase1-foundation | data-driven registry, Left/Right cycles effects, R restarts, Space/. pause-step, D and 1–4 cycle debug mode, ESC quits. FB slot = real `TFlameEffect` (F01); PE slot = real `TBloodEffect` (B01); SR slot = real `TStripEffect` (S01); LS slot still placeholder (X17) pending TFlareAnimator port. |
| I09 | Per-bucket blend/light/depth modes | infra | (port-only) | n/a | src/particlefx.h, src/renderer.{h,cpp}, src/shaders/fx.* | done | phase2-blood-followup | `EParticleBlendMode` gains `AdditiveStraight` (ONE/ONE, matches retail `D3DBLEND_ONE/ONE`) + `PremulAlpha`; new `EParticleLightMode { Unlit, LitFlat }` (cheap hemisphere lighting, world-up implicit normal); new `EParticleDepthMode { TestNoWrite, TestWrite, None }`. Per-instance `light_mode` lane in fx instance buffers (same pattern as `debug_mode`); pipeline cross-product flattens to `fx_*_pip[blend][depth_mode]` = 36 sg_pipelines. UBO gains `sun_dir + sun_color + ambient_color`. Per-particle normals deferred. TBloodEffect updated to `LitFlat`. |

### Fire family

| #   | Class                | Pipeline | Retail src      | Recon file | Port file               | Status        | Owner | Notes |
|-----|----------------------|----------|-----------------|------------|-------------------------|---------------|-------|-------|
| F01 | `TFlameEffect`       | FB       | effect.cpp      | not-extracted | src/effect.cpp:577,690+ | normal        | phase2-flame | `TFlameEffect::SpawnForTest(origin)` loads `Magic\flame.i3d`, builds a sector-less instance, attaches the flipbook + particle components, gives it a fresh map index, and activates everything. `--test=vfx` F01 now drives that real instance through `flipbook->Submit(*Renderer, *flame)` (same call site the in-game per-instance walk uses). Flipbook frame counter ticks via the global RunUpdateList → legacy_frame % cols/rows UV stepping. Debug-mode cycling (D / 1–4) flows in via `flipbook->SetDebugMode(dbg)` per submit. Awaiting user-side visual confirmation to move to `validated`. |
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
| B01 | `TBloodEffect`   | PE       | effect.cpp      | recon/classes/cls_0x5acaa8.cpp | src/effect.{h,cpp}      | current-frame | phase2-blood | **2026-05-16 Phase 2.2 (preview-style harness lift):** `TBloodEffect::SpawnForTest(origin)` loads `Misc\Blood.I3D` (the canonical bloodimagery, also loaded by playscreen.cpp:236), allocates a global `vfx.blood.droplets` PE bucket (PremulAlpha blend, Unlit, TestNoWrite, 2x2 64x64 sprite-atlas) and **spawns a one-shot burst of 10 droplets at construction**. `TickAndSubmitForTest(dbg)` integrates existing particles (Euler, downward gravity, hold-half / linear-fade-half alpha curve, reap on expiry) and submits via `Renderer->SubmitFxParticleBucket()` — no further spawning. The `--test=vfx` harness drives Combat-style re-trigger (destroys + respawns the effect every ~0.6s at a random ±240 wu screen-area position; see EVfxPreviewStyle in vfxtest.{h,cpp}). Texture loader (3dimage.cpp) converts chroma-key bg pixels (>20% black) to premultiplied a=0 so bilinear edges blend cleanly with the scene.<br/>**Forensic gap vs retail (src/effectcomp.cpp:1165-1180+ TBloodSystem::Init, pre-release source; not yet cross-checked against recon/classes/cls_0x5acaa8.cpp which is a Blood+Mist+WaterFall Ghidra merge):**<br/>- Retail constants: `MAX_BLOODS=30`, `BLOOD_BLOODSPERSQUIRT=4`, `BLOOD_MOSTBLOODS=10`, `BLOOD_VEL=2.0` wu/tick, `BLOOD_GRAVITY=0.37` wu/tick², `BLOOD_FLYSCALE=1.0`, `BLOOD_FLYSCALESTEP=0.01`. All authored at retail's 24 Hz sim tick.<br/>- Retail Init takes `(num, height, hangle, vangle, hspread, vspread, maxsize)` — directional spray, not radial cone. Caller passes the player→attacker angle so blood sprays away from the impact direction.<br/>- 3 size variants (0=small,1=med,2=big); each rendered TWICE per frame (base pass + additive overlay, with sml/sml2/med/med2/big/big2 anim objects).<br/>- Trail logic: smalls trail smalls with reduced velocity (60-75%) and `delay = parent.delay + BLOOD_SQUIRTDELAY`. Bigs→med+small and meds→med+small trails are present-but-commented in pre-release.<br/>- ViolenceLevel global clamps num and maxsize.<br/>- Current port's `kBurstCount=10`, `kLife=1.4s`, radial cone spawn, single render pass — visually plausible "blood spurt" but NOT retail-faithful. **Tracked for B01a (Phase 2.2.1).** |
| B01a| `TBloodEffect` (faithful kinematics) | PE | effect.cpp | recon/classes/cls_0x5acaa8.cpp | src/effect.cpp, src/effectcomp.{h,cpp} | not-started | - | **Phase 2.2.1 follow-up to B01.** Replace the SpawnForTest-owned bucket + ad-hoc Euler integrator with real `TBloodSystem` (B03) + `TBloodAnimator` (B02) port from retail. Needs: (a) Ghidra-rerun split of cls_0x5acaa8 to isolate the Blood class from the Mist + WaterFall merge; (b) faithful droplet/splat state machine from retail TBloodSystem (SBloodParticle.size/stage/count/delay fields); (c) surface-intersection for blood-splat decals; (d) proper frame UV picking from the multi-frame Blood.I3D sheet. Until done, B01 looks "vaguely like blood" but isn't retail-faithful. |
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
| S01 | `TStripEffect`         | SR       | stripeffect.cpp | not-extracted | src/stripeffect.{h,cpp} | normal        | phase2-strip | **2026-05-16 Phase 2.3 (SR-pipeline validator landed):** `TStripEffect::SpawnForTest(origin)` constructs a sector-less standalone instance via the imagery-only ctor (with a small engine extension to TObjectInstance to accept nullptr imagery in the test harness path — see src/object.cpp around line 562), stamps it with a fresh map index + activates components, and seeds 2 anchors at origin. `TickAndSubmitForTest(dbg)` gates state-machine + jitter on a 24 Hz sim-tick (the retail animator cadence; without the gate the bolt crackles at render rate), grows the anchor ring at STRIP_SMOOTH_SIZE=4 anchors/tick up to a dynamically chosen 12-36 cap, re-jitters every 4th anchor in (-13,+13) wu with linear-interpolated intermediates (placeholder for retail's 0.5/2.0/3.5 cubic curve), shrinks from the tail during STRIP_EXPLODE, scrolls the texture U by -0.1/tick, and submits SStripSegment[] via `Renderer->SubmitFxStrip()`. Width tapers STRIP_WIDTH_HILT=32→STRIP_WIDTH_TIP=20 wu hilt-to-tip; alpha holds for the first 60% of the bolt then linearly fades. Texture = renderer white handle (Magic\lightning.* asset not yet identified — see gap below). Blend = Additive (matches the pre-release D3D SaveBlendState/SetBlendState call for the lightning render branch); depth = TestNoWrite.<br/>**X16 placeholder removed:** the bespoke RibbonSpawn/RibbonSubmit/RibbonDestroy + BlueRibbonTexture in src/vfxtest.cpp are deleted; SR slot is now the real TStripEffect lambda shim (mirrors F01 / B01 pattern).<br/>**Build + ASan clean:** `cmake --build build --target Revenant` succeeds with no new warnings; `--test=vfx` boots to TBloodEffect default and the TStripEffect entry cycles in cleanly via Left/Right; ASan run (`build-asan/Revenant --test=vfx`) shows zero findings in stripeffect.cpp / vfxtest.cpp / particlefx (pre-existing UBSan noise in parse.cpp / resource.cpp / graphics.cpp is unrelated and predates this row).<br/>**Carved-out follow-ups (S01a):** the full TLightningAnimator composite (rotating glow + spark sub-particles + spark impact at strike point) is PE+IM pipeline work blocked on I04 TSubParticleAnimator and the imagery wiring for the lightning texture; deferred from this row per the AGENT_GUIDE §3.0 forensics-gap pattern (same pattern as B01a tracks faithful blood kinematics). Once S04 TLightningAnimator's recon at recon/classes/cls_0x5b0a28.cpp is split + the PE infra rows (I02-I05) land, S01a can fold the glow + sparks onto the existing TStripEffect instance.<br/>**Phase A forensics body (six-point template per AGENT_GUIDE §3.0; pre-release source = src/stripeffect.{h,cpp} `#if 0` body; no recon for TStripEffect itself yet — S04 TLightningAnimator's recon at cls_0x5b0a28 is its animator, not this effect class):**<br/>**1. Retail constants** (from `src/stripeffect.h` + `Initialize/Pulse/SetupObjects/Animate` in the `#if 0` body):<br/>- `STRIP_SPEED = 16` wu/step (walker advance per Init scan tick).<br/>- `STRIP_RANGE = 2` (unused in pre-release Pulse; appears to be a +2 segment overshoot — see `targetrange = pnt + 2`).<br/>- `MAX_MAXPOINTS = 100` (hard cap on segment count).<br/>- `LIGHTNING_SCALE_N = 2`, `LIGHTNING_SCALE_D = 1` → segment width = (2*16)/1=32 wu at hilt, (2*10)/1=20 wu at tail.<br/>- `SMOOTH_SIZE = 4` (animator smoothing chunk; intermediate jitter points interpolated between every 4th anchor).<br/>- `ADD_FACTOR = 2` (segments added/removed per animator frame during growth + explode).<br/>- `TARGET_POS = 1` (compile-time switch: 1 = strike target, 0 = demo mode w/ SMOOTH_SIZE*maxpoints).<br/>- States: `STRIP_LAUNCH=0` (one-tick raycast), `STRIP_FLY=1` (duration=20 ticks at sim rate), `STRIP_EXPLODE=2` (duration=5 ticks shrink-back).<br/>- Per-strip segment Y stride = `-10` wu (segments march away from caster in local -Y).<br/>- Jitter: `random(-13, 13)` on x/z every 4th vertex; smoothed cubic in between.<br/>- Animator-side: `glow_scale = 3.4` initial, `*= 0.8` per explode frame; rotdegree advances 12°/frame, morrotdegree 16°/frame.<br/>- Spark sub-particle: 20 count, 25% chance, velocity_spread (2,2,1), gravity 0.18, life 25-35 ticks, scale 2.0 with 0.90 decay, flicker on (size 2.5).<br/>- ScrollTexture: `-0.1` per frame (U-coord scroll, animator-side).<br/>**2. Spawn / emit shape:**<br/>- Trigger: spell-cast `"Lightning"` (see `data/Resources/spell.def` lines 675-700: 3 variants — Electric Bolt, Priest Bolt, HighPriest Bolt — all `VARIANT "<name>", TP_BASIC, "<bone>", "lightstrip", ...`). The 3rd arg `"AI"/"MJ"/"MAL"` is the caster bone (right hand). Spawned once per cast.<br/>- Position: invoker's pos +50 wu in Z (eye/hand level). `Initialize` does GetPos → +50 z → SetPos.<br/>- Direction: invoker.facing (8-bit angle); walker steps `ConvertToVector(facing, 10, vel)` so segments march along caster's forward.<br/>- Segment count: dynamic. `SetupObjects` raycasts from caster forward in 10-wu increments, stopping on wall (`GetWalkHeight == 0`), character hit (`CharBlocking` or distance < 16 to a non-friendly char), or 100 cap. Final `maxpoints` = that count, clamped to 100.<br/>- Two distinct shapes possible (compile-time): `TARGET_POS=1` (ship) = straight-ish bolt with per-vertex jitter; `=0` (demo) = SMOOTH_SIZE * count = up to 400 vertex smoothed strip.<br/>**3. Per-frame integration:**<br/>- `Pulse` is the effect state machine (STRIP_LAUNCH → STRIP_FLY duration=20 → STRIP_EXPLODE duration=5).<br/>- `Animate` (animator-side, per draw frame):<br/>  * Growth phase: add `ADD_FACTOR=2` points/frame at p = (0, numpoints*-10, 0) until `numpoints == maxpoints`. Last point's "maxsize" flag freezes growth.<br/>  * Steady phase: re-jitter every 4th point (x,z) randomly in (-13,13); cubic-interp intermediates per the SMOOTH_SIZE=4 mod-pattern (mod 1: 0.5/SMOOTH, mod 2: 2.0/SMOOTH, mod 3: 3.5/SMOOTH).<br/>  * "Down" flag adds an arched droop along Y proportional to (i-1)*20/(maxpoints-1).<br/>  * Explode phase: shrink by 2 points/frame from tail; glow_scale *= 0.8; halts when no sparks left.<br/>- No gravity/drag on the strip itself (it's a fixed-in-space bolt, jitter only).<br/>- No alpha curve in the strip — width is constant per-vertex; color is white (1,1,1,1) from `RenderObject` lvert pack. Fade comes from glow shrink + sparks ending.<br/>- Sparks have own gravity 0.18 + scale decay + flicker.<br/>- Death: `KillThisEffect` when state==EXPLODE && both spark subsystems empty.<br/>**4. Render passes:**<br/>- Pre-release `Render` (D3D, archival): culling off (D3DCULL_NONE), `SaveBlendState/SetBlendState` (= additive by D3DRenderer convention) for the strip; glow rendered twice with rotated matrices (cross-pattern using rotdegree + morrotdegree, RotX -30°, RotY 60° tilts); sparks rendered via TSubParticleAnimator.<br/>- Texture: animator-built `TStripAnimator` with a single texture (scrolled u every frame, range -1..1 from WindStripEffect; lightning probably -1..1 too). Texture name not in source — needs imagery probe.<br/>- For the port: **strip-only** is the SR-pipeline validator (glow + sparks are PE-pipeline, defer per B01a-style scope cut). Single additive pass, no animator-built atlas — single-bolt color (electric cyan/white) modulated by U-scroll, alpha fades along length.<br/>**5. Caller / trigger sites:**<br/>- In retail: `TSpell::Cast` → effect builder lookup `"lightstrip"` → `TStripEffect(SObjectDef*, TObjectImagery*)`. Bone name from variant arg used by `TLightningAnimator::Initialize` via `GetObjectMatrix(GetObjectNum("lhand")|"rhand"|...)` to anchor the bolt start at caster's hand.<br/>- In current port: `TSpell` / cast plumbing is not wired yet (the entire `stripeffect.cpp` body is `#if 0`). No game-side caller exists — confirms the harness-driven `SpawnForTest` is the right validation surface.<br/>**6. Gaps / unknowns:**<br/>- **Task-brief framing of "sword slash + ribbon trail" is wrong.** TStripEffect is the **lightning bolt** spell effect. The sword-swipe / weapon-trail code is in `src/weapontrail.cpp` (`TWeaponSwipe`, originally TCharAnimator-owned; not a TEffect subclass and not driven through the spell registry). Both use a strip rendering primitive (`TStripAnimator` is the shared one, S06 row) but they are distinct classes with different ownership models. Treating S01 = lightning is the source-grounded read; tracked as a clarification in the row.<br/>- **No retail recon for TStripEffect itself.** S04 TLightningAnimator's cls_0x5b0a28 is the animator side (the per-frame visual driver). The TEffect-derived `TStripEffect` (state machine + raycaster) does not have a confirmed Ghidra mapping yet — it's small enough (3 methods, ~150 LOC pre-release) that retail constants for `duration=20/5` and `STRIP_SPEED=16` could differ. Pre-release values used; retail divergence acknowledged.<br/>- **Texture / atlas dims unknown.** Pre-release source never names the lightning texture. Imagery search for `Magic\lightning.*` or similar deferred — for Phase B we ship with `WhiteTextureHandle` (same dodge the X16 placeholder used) so the strip renders as solid white, no texture lookup required. Real atlas lookup tracked as S01a follow-up if/when faithful texture is needed.<br/>- **No spark / glow port.** Per scope-cut: SR pipeline only. Glow (~5 of the 11 render branches in `TLightningAnimator::Render`) and sparks (TSubParticleAnimator I04) are PE-pipeline + IM-pipeline work, blocked on I04 + S05/S06 ports. Tracked as S01a (lightning composite: strip + glow + sparks).<br/>- **Per-vertex jitter randomization frequency.** Pre-release jitters every animator frame (i.e. every render frame). At modern 60 fps that reads as a frantic crackle; retail ran the animator on the 24 Hz sim tick. Port follows retail intent: only re-jitter on the sim-tick boundary, not every render frame. |
| S02 | `TWindStripEffect`     | SR       | stripeffect.cpp | not-extracted | src/stripeffect.h       | not-started   | -     | wind streak; needs Ghidra extraction |
| S03 | `TWindStripAnimator`   | -        | stripeffect.cpp | not-extracted | src/stripeffect.h       | not-started   | -     | bridge; needs Ghidra extraction |
| S04 | `TLightningAnimator`   | SR       | Effect3.cpp     | recon/classes/cls_0x5b0a28.cpp | src/stripeffect.h       | not-started   | -     | **2026-05-16 pass-2 RESOLVED:** the cls_0x5b0a28 hypothesis from pass-1 is CONFIRMED as MEDIUM-HIGH candidate. cls_0x5b0a28 (1400 bytes, 5 methods) hosts the "LightningStorm" registry XREF + ENERGYSPRAY + "rhand" XREFs + the d:\revenant\Effect3.cpp assert string -- all inside its single registrar fn 0x502b70. The state machine in that fn (mbr_0x184 counter, mbr_0x188 timer with values 0x3c/0x32/0x19) matches a strike-cycle pattern. **Retail src corrected to Effect3.cpp** (not stripeffect.cpp). The pass-1 cls_0x5a47f0 mapping was demoted; that class is the TEffect base itself (see I01). See recon/mappings/TLightningAnimator_cls_0x5b0a28_candidate.yaml. |
| S05 | `TShockAnimator`       | SR       | effectcomp.cpp  | not-extracted | src/effectcomp.h        | not-started   | -     | shockwave ring; needs Ghidra extraction |
| S06 | `TStripAnimator`       | SR       | effectcomp.cpp  | not-extracted | src/effectcomp.h        | not-started   | -     | generic strip animator; needs Ghidra extraction |
| S07 | `TStreamerEffect`      | SR       | effect.cpp      | not-extracted | src/effect.h            | not-started   | -     | banner / streamer trail; needs Ghidra extraction |
| S08 | `TMissileEffect`       | IM       | MissileEffect.cpp | recon/classes/cls_0x5b3c18.cpp | src/missileeffect.h     | not-started   | -     | **2026-05-16 pass-2:** MEDIUM candidate cls_0x5b3c18 -- shared base for the MissileEffect family (Photon + FireBall + lightning sound-key all XREF here). Inherits cls_0x5a50e8 (root animation class). See recon/mappings/TMissileEffect_cls_0x5b3c18_candidate.yaml. |
| S09 | `TWeaponSwipe`         | SR       | weapontrail.cpp | recon/classes_readable/TWeaponSwipe.cpp (low-confidence; see gap 7.2) | src/weapontrail.cpp (+ Phase B target: src/weaponswipe.{h,cpp}) | forensics-done; phase-b-deferred-on-rig | phase2-swipe | **2026-05-16 Phase A landed; Phase B deferred behind CharacterWithWeaponRig (per AGENT_GUIDE §3.0 strategy-a).** The actual sword slash trail. Distinct from S01 TStripEffect (Lightning bolt) — both use the strip primitive but TWeaponSwipe is owned by TCharAnimator per-character (`weaponswipe` member, GetWeaponSwipe() accessor) and triggered by attack-frame events. User flagged "sword slashes" as essential VFX in the original brief. Pre-release source intact at src/weapontrail.cpp under `#if 0` (see lines 353-632 for Init/GenerateStrip/Animate/CycleStrip/GetWeaponExtents/NormalizeColors/Render); recon body intact at recon/classes_readable/TWeaponSwipe.cpp (with caveats — see gap 7.2).<br/>**Phase A forensics body (seven-point template per AGENT_GUIDE §3.0):**<br/>**1. Retail constants** (from `SetupWeaponSwipe()` in src/weapontrail.cpp `#if 0`):<br/>- `length = 5` (the **strip length parameter** in segments-per-spline-chunk; `p.maxsegs = length * smooth = 5 * 8 = 40` triangles after spline expansion).<br/>- `smooth = 8` (Catmull-Rom spline subdivisions between adjacent hilt/tip anchor pairs; renders a smooth arc through the discrete sample anchors).<br/>- `maxpoints = (maxsegs / smooth) + 3 = 8` (the ring-buffer length of stored (hilt,tip) anchor pairs; +3 = head insert + spline-tail lookahead requires 2 future points).<br/>- `maxverts = (maxpoints - 1) * smooth * 2 = 112` (S3DLVertex array; *2 = hilt-row + tip-row per smoothed sample).<br/>- `numfaces = maxsegs * 2 = 80` (triangle list, two tris per segment quad).<br/>- Strip width: **NOT a constant** — it's the per-frame distance between the hilt-anchor and the tip-anchor, set by `GetWeaponExtents()` from the weapon mesh's z-axis extents (so a long sword draws a wide arc; a dagger draws a narrow one). Structural difference vs. S01 lightning whose width is parametric.<br/>- Color (per character): pulled from `SCharData::swipecolor` (SColor, RGB 0..255), normalized via `NormalizeColors()` so the max channel = 1.0 (preserves hue but boosts brightness). Pre-release default `swipecolor = {0,0,0}` skips swipe entirely; characters' RULES.DEF entry must set a non-zero value to enable it. The **retail SCharData decomp at recon/classes_readable/SCharData.cpp:57 sets `swipecolor = 0x10`** = a packed default that, read as one of the four bytes, is ≈16/255 ≈ small blue tint. Pre-release matches structurally (SColor.red/green/blue/alpha = 4 bytes at offset +0x2c per recon/mappings/SCharData_cls_0x5b9fcc_map.yaml; matches src/rules.h:185).<br/>- Color gradient (along strip length): hilt end is brightened by +0.4 per channel (clamped to 1.0) — `startr = min(r+0.4, 1.0)` — and fades to the normalized base color across the strip's first vertices via `wfade = 4.0/maxverts` per vertex. Hilt-side reads brighter ("heat" near blade root), tapering to base hue.<br/>- Alpha gradient: starts at `0.3` (head), fades to 0 across the strip via `fadeoutstep = 0.6/maxverts` per vertex. So the strip is **always semi-transparent** (max alpha 0.3), and the tail dies out.<br/>**2. Spawn / emit shape:**<br/>- Trigger: NOT a one-shot. `TCharAnimator::SetupWeaponSwipe()` is called lazily on first frame after construction (inside `TCharAnimator::Animate(draw)`: `if (!weaponswipe.GetInitialized()) SetupWeaponSwipe();`), then **continuously runs** for the lifetime of the character. `Animate()` is called every `TCharacter::Pulse` tick (24 Hz sim rate). The strip is only **visible** when `weaponswipe.Render()` is invoked, which happens inside `TCharAnimator::Render()` (per render-frame) gated by `(initialized && IsAttack() && !(player && primehand==nullptr))`. So: the ring-buffer of (hilt,tip) anchor pairs is **always being filled** as the character animates, but the visible swipe only draws during ACTION_ATTACK frames.<br/>- Anchor source: each tick, `GetCharsWeaponMatrix()` resolves the weapon object's animator matrix (looks for `GetObjectNum("weapon")` then "sword" then "ogrokaxe" — covers all weapon-bearing characters), and `MtxTransform(weaponmat, &vweapbeg, &beg)` / `vweapend, &end` transforms the hilt-vertex (zero) and tip-vertex (extreme +Z mesh vertex) into world space. Those become `points[0][0]` and `points[1][0]`. `CycleStrip()` shifts the whole ring down before insertion, oldest sample drops off the end.<br/>- Strip geometry: pairs of world-space points (hilt-row + tip-row), one new pair per sim tick, ring length 8. `GenerateStrip()` Catmull-Rom-splines through each row to produce 8 smoothed samples between each adjacent anchor pair → 7 segments × 8 subdivisions × 2 rows = 112 vertices arranged as a triangle-stripped ribbon (in pre-release; the SR-pipeline port would emit `(maxpoints-1)*smooth = 56` `SStripSegment`s instead).<br/>- Direction: defined by **weapon motion through the swing animation** — the bone-mesh code drives the matrix that drives the anchors. The strip is a literal sweep of where the blade has been over the last 8 ticks (~0.33s at 24 Hz).<br/>**3. Per-frame integration:**<br/>- `Animate()` (sim-tick, 24 Hz): weapon-switch guard (primehand string compare with stored value — switching from sword to dagger calls `Close()` to free the buffers and resets `initialized=false`, so it re-Inits next frame with the new weapon's extents); then `weaponmat = GetCharsWeaponMatrix(); MtxTransform(weaponmat, vweapbeg/vweapend, beg/end); CycleStrip(); points[0][0]=beg; points[1][0]=end;`.<br/>- `CycleStrip()`: ring-shift, oldest sample drops off end (`points[o][i] = points[o][i-1]` for i = maxpoints-1 down to 1). Naive O(maxpoints) memmove; cheap enough at ring length 8.<br/>- `GenerateStrip()` (render-frame): for each adjacent (older, newer) pair in the ring, Catmull-Rom-spline 8 in-between samples for both hilt and tip rows, packing per-vertex color (with hilt-side brightness fade) + alpha (fade-out across length).<br/>- No gravity/drag/physics — pure animator-driven trail. Death = strip auto-clears as the weapon stops moving (the anchors keep getting overwritten with near-identical matrices once the swing finishes; spline output becomes a stationary smear, then `Render()` stops firing when ACTION_ATTACK ends).<br/>- The `initialized` flag is reset on weapon swap, on `Close()`, and not set if `swipecolor == (0,0,0)`. Characters without a colored swipe never allocate the buffers (zero overhead).<br/>**4. Render passes:**<br/>- Single pass. Pre-release `Render()`: culling off (`ERender3DState::Cull, 0`), `SrcAlpha/InvSrcAlpha` alpha blend, `ZWriteEnable=0` (write-through depth for translucency); `GenerateStrip()`; `charanim->RenderObject(obj)`.<br/>- No texture — pre-release strip is **untextured**, color comes from per-vertex `S3DLVertex.diffuse` (lvert ARGB-packed). Strip reads as a colored translucent ribbon, not a textured trail.<br/>- For the Phase B port: SR-pipeline (`Renderer->SubmitFxStrip`). Single quad-strip with miter-joined adjacent segments (already in renderer.cpp DrainFxQueue strip path). Texture = small procedural gradient (broader bright center, sharper alpha fade at edges — a sword-trail reads more like a slice than a glow) since we're already paying for the texture lookup, and an untextured strip with hard edges reads poorly at modern resolutions. Blend = `Alpha` (matches retail's SrcAlpha/InvSrcAlpha) **not** AdditiveStraight — pre-release explicitly uses alpha-blend, and the per-vertex alpha curve (starts at 0.3) means additive would never reach full intensity. **Divergence from S01 + the task brief:** the task brief suggested AdditiveStraight to match S01, but pre-release source overrides the brief: S01 lightning is additive because it IS a glow; S09 swipe is alpha because it's a translucent slice.<br/>**5. Caller / trigger sites:**<br/>- In retail: `TCharAnimator::Animate(draw)` → lazy `SetupWeaponSwipe()` once (after first attack pose loads the weapon mesh); thereafter `Animate()` ticks each sim frame from `TCharacter::Pulse` (see src/weapontrail.cpp:136 archival note: `TWeaponSwipe* ws = anim->GetWeaponSwipe(); if (ws && ws->GetInitialized()) ws->Animate();`). Render gated by `IsAttack() && initialized` inside `TCharAnimator::Render()`. ACTION_ATTACK enum is in src/character.h (per IsAttack() = `IsDoing(ACTION_ATTACK)`); the state is set by TCharacter::ButtonAction → SpecificAttack → action table parse (see src/character.cpp around 3850 for the attack-frame state machine).<br/>- In current port: TCharAnimator is alive (src/charanimator.cpp:50 `Animate()`, :55 `Render()`) but **all weapon-swipe call sites are in the `#if 0` body of src/weapontrail.cpp**. No live integration in the game-side render path. The Render() in charanimator.cpp:55 doesn't carry `RenderCombatFlashes()` either (line 102 — also commented out, same dormant-rendering slot). So the live game **never spawns or renders a weapon swipe today** — gameplay wiring is gated on rewiring SetupWeaponSwipe back into TCharAnimator + porting Render() to use the SR submit path.<br/>**6. Test-mode rig requirements — `CharacterWithWeaponRig` (NEW; nothing reusable yet):**<br/>- **Category:** Character + weapon + attack-anim (per AGENT_GUIDE §3.0 categories list). First effect in this category — the rig is **not yet built**. Phase B is blocked on building it.<br/>- **What the rig must construct, per real-code-path forensics above:**<br/>  1. **A loaded character imagery** — call `LoadImagery("Locke")` (or any character with a non-zero swipecolor in RULES.DEF — Locke is the safe pick; SCharData defaulting suggests his entry is already wired) so the I3D mesh + bone skeleton + RULES.DEF entry are resident. The harness lives in vfxtest.cpp under `--test=vfx`; today it has no sector, no game-world, no character pool. Loading imagery from a non-sector context may need a small engine assist (see TFlameEffect's no-sector path for the model — but characters are heavier than flame effects, they pull RULES.DEF parse via TCharacter ctor).<br/>  2. **A TCharacter (or TCharAnimator + TObjectInstance pair) constructed against that imagery** — needs a map-index (use MapPane.MakeIndex like F01/B01/S01) and `ActivateComponents()`. Probably needs a stubbed `chardata` pointer (the TCharacter ctor reads RULES.DEF entries).<br/>  3. **A weapon equipped** — for a TPlayer, this is `PrimeHand()` returning a TObjectInstance with weapon imagery. For TCharacter (non-player), the GetCharsWeaponMatrix path falls back to `GetObjectNum("weapon"|"sword"|"ogrokaxe")` lookup on the character's own imagery (i.e. the weapon is a sub-object of the character mesh, not a separate equip). For a Locke harness, picking the TCharacter (non-player) path is the simpler one — no need to instantiate a TObjectInstance for the sword, just rely on Locke's mesh having a "sword" sub-object. Need to verify by `GetObjectName` enumeration.<br/>  4. **A swing animation playing** — drive `SetAction(ACTION_ATTACK, ...)` (or whatever the lowest-level animation-state setter is) on a loop. The harness already ticks per-frame via TTime; the rig just needs to advance the animator's frame each tick. Loop the attack animation continuously: when it ends, re-trigger.<br/>  5. **A camera framing** that shows the character at viewable scale — current harness camera is centered at world origin facing -Y at kCam=2750wu back. Character would be ~150wu tall, swipe diameter ~80wu — the existing framing is generous enough. No camera changes needed.<br/>  6. **Wire SetupWeaponSwipe/Animate/Render back into TCharAnimator** (REQUIRED — the entire `#if 0` block in src/weapontrail.cpp:150-635 must move out of `#if 0` and be ported off D3D to SR-pipeline submit). This is the bulk of the Phase B work. The port itself is small (Catmull-Rom spline → SStripSegment[] pack is ~80 LOC); the integration into the live game render path is what blocks. Alternative: keep `#if 0` in charanimator.cpp's live path but expose a `RenderWeaponSwipe(Renderer*)` method on TCharAnimator that the harness can call directly without rewiring TCharAnimator::Render. **Recommended:** the direct-call alternative for the harness gate, then once Phase B is validated, fold into live TCharAnimator::Render() as a separate gameplay-wiring row (S09b).<br/>- **Reuse policy:** the rig should land as `CharacterWithWeaponRig` (or `CharacterRig` with weapon as an extension) in vfxtest.{h,cpp}. Future character-attached effects (B05 TBurnAnimator status overlay, M05 TAuraAnimator, M06 TIcedAnimator) can subclass / share the CharacterRig base; the weapon-bearing variant is S09-specific until other weapon-effects appear.<br/>- **Estimated rig-build cost:** medium-large. The character spawning path is the unknown — none of the test modes today instantiate a character from outside the sector loader. Probably 1-2 agent sessions of focused work just to get a static Locke + idle-anim rendering, then another session to drive ACTION_ATTACK in a loop.<br/>**7. Gaps / unknowns:**<br/>- **7.1 Harness can't drive the natural code path today.** See section 6 — this is the gating issue. Author preference per dispatch: build the rig as a follow-up (S09-rig agent), then unblock Phase B. Two pure-mock alternatives considered + rejected:<br/>  - *Synthetic hilt/tip arc fed into a ported TWeaponSwipe* — would ship strip-renderer math but hide weapon-extents bugs; flagged "synthetic data hides bugs" by coordinator. Not pursued.<br/>  - *Hand-built character-less harness with hand-coded sword polyline* — bodge; violates "no bodges". Not pursued.<br/>- **7.2 Retail recon mapping is low-confidence.** recon/mappings/TWeaponSwipe_cls_0x49e610_map.yaml claims HIGH on size + field-offset match, but the decompiled body at recon/classes_readable/TWeaponSwipe.cpp shows `meth_0x49e610` doing `VirtualUnlock(weaponmat, maxsegs)` + `FUN_00482f80` (memory-mapped file teardown) + iterating `weaponverts` to call `vtable[0](1)` on each — that's **destructor-like memory-management code on a memory-mapped object**, not a swipe renderer. References `s_video_03d_bmp_005dacf8` (a video-file path string). Most likely cls_0x49e610 is a **video frame buffer** that happens to have the same 36-byte size + ddw layout as TWeaponSwipe; the YAML auto-mapper false-matched. **No real recon for TWeaponSwipe exists yet.** Pre-release source = authoritative for this row; future S09-recon row should run class_index.tsv fingerprint over `d:\revenant\charanim*.cpp` source-path matches.<br/>- **7.3 Catmull-Rom spline math.** Pre-release uses `Spline(&out, ratio, &p[max(0,o-1)], &p[o], &p[o+1], &p[o+2])` — 4-point Catmull-Rom. src/math3d.h has Spline (verified by the Animate() callsite working in the legacy body). The new port can reuse it.<br/>- **7.4 Per-character swipecolor not read by harness.** Phase B will need to feed SCharData.swipecolor through. Once the rig is up, this becomes trivial (read chardata; if rig spawns Locke, his swipecolor is whatever RULES.DEF specifies — confirm via runtime log on rig boot).<br/>- **7.5 Animator gating cadence.** Pre-release `Animate()` runs every sim tick (24 Hz). Generation runs every render frame. Mirror this in Phase B: anchor ring updates on a 24 Hz sim-tick gate (same kSimTickMs = 1000/24 pattern as S01); strip submission happens every render frame from the current ring snapshot.<br/>- **7.6 Weapon-mesh sub-object enumeration.** GetCharsWeaponMatrix walks 3 hardcoded names ("weapon", "sword", "ogrokaxe"). At rig-build time we'll need to confirm Locke's mesh has one of these as a named sub-object — otherwise the test rig needs to extend the enumeration list (carry as a Phase B finding when the rig is built). |

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
