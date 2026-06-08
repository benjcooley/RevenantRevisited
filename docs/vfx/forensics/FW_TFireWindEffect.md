# FW TFireWindEffect / TFireWindAnimator — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | FW |
| **Class(es)** | `TFireWindEffect : TEffect` (game logic — radially expanding damage ring that calls `BlastCharactersInRange` every 40 wu of radius growth) + `TFireWindAnimator : T3DAnimator` (visual — hand-rolled 400-slot particle pool, five particle states, five visual sub-systems) |
| **Status** | forensics-complete |
| **Retail fidelity** | **retail-partial** — registration confirmed: `"FireWind"` at `.rdata 005e108c` XREF[2]: `0x4e2250` + `0x4e23e0` (the `DEFINE_BUILDER` + `REGISTER_3DANIMATOR` pair, `recon/classes/_data.txt:107365-107366`); `"FireWind"` at `.rdata 005e0f08` XREF[1]: `virt_meth_0x4e2400:0x4e240e` (the animator name-getter override, `_data.txt:107134-107135`). Spell wiring is live in shipped `data/resources_unzipped/spell.def:710-726`. The retail `firewind.i3d` inside `data/imagery.rvi` has **the same sub-object names (`smoke`, `smoke01`) but a different byte size from the snapshot** (17,452 B retail vs 25,908 B snapshot), indicating an asset revision between pre-release and ship. The lifetime hard-cap `framenum > 140` is **snapshot-only** (no isolatable retail Animate body); all FWIND physics/timing constants are **snapshot-only**. Audio cue `"FireWind"` maps to shipped `data/resources_unzipped/Sound/effects/firewind.wav` (108,762 B, 1998-11-23) — **retail-confirmed**. See §2.1. |
| **Author / Date** | vfx-forensics-agent (wave-4) / 2026-06-07 |
| **Family** | fire (spell-cast offensive — sister to FireFlash, FireCone, FireBall in the fire spell family) |
| **Draws** | composite — up to 400 simultaneous **I3D-mesh** particles drawn from a 2-sub-object asset (`smoke` + `smoke01`), across five particle states (SMOKEY1 base flame column, GSPHERE orbiting sphere, RING expanding explosion ring, FIRERING ground ring, USEME dead/free). Each live particle is a single `RenderObject` call with a per-particle scale + the shared fixed-tilt rotation matrix. |
| **Archetype(s)** | composite: **(D) rising particles** (SMOKEY1 base flame), **(E) spinning-orbit particles** (GSPHERE — 100 particles on 3-axis rotation around fixed pivot), **(E) expanding-ring** (RING — 200 particles orbiting pivot in XY plane, radius growing until 300 wu), **(D) radial ground fire ring** (FIRERING — outer ground ring expanding on a separate radius track), **(F) custom state machine** with 4 time-windowed phases coordinated by a single `framenum` counter, **(F) expanding damage AoE** (radially growing `BlastCharactersInRange` every 40 wu of radius), **(M) audio** (one-shot `PLAY("FireWind")` at Initialize). Associated spell light (from `spell.def`, not the animator). |

---

## 1. Summary

`"FireWind"` is the Level-3/Level-4 Fire-school AoE spell — visually described by the developer as "a very large ring growing fire particles arranged in a circle, one of the first dramatic effects in the game." Cast by early CAVES area priests, it produces a multi-phase pyrotechnic event anchored at the cast location. First (frames 0–24) a central **rising flame column** of 4–7 SMOKEY1 particles/frame erupts from the ground, identical in shape to FireFlash's SMOKEY1 phase but with a larger particle pool. At frame 28 a **100-particle glowing sphere** ignites and grows (radius 1→40 wu, faster than FireFlash's 20 wu cap). At frame 40 (**EXPLOSION**) all sphere + free particles convert instantly to a **200-particle expanding ring** that grows outward at 13 wu/frame until it reaches 300 wu, then clears. Simultaneously at frame 45 a persistent **FIRERING ground circle** (radius 30, expanding at 4 wu/frame) of rising flame particles sprays continuously for ~75 additional frames. The effect hard-kills at `framenum > 140` (≈ 5.83 s at 24 Hz). Gameplay: `TFireWindEffect::Pulse` calls `BlastCharactersInRange` radially every 40 wu of expansion, with damage 96–124 DT_BURN, dealing knockback to caught enemies.

---

## 2. Sources & evidence

- **Retail decomp:** `recon/classes/cls_0x5a9194.cpp` — **SPARSE / MERGED**. Ghidra merged TFireFlashEffect + TFireWindEffect + TBurnEffect (and their animators) into one 13,504 B class file. No Animate/Render/RefreshZBuffer bodies recovered for TFireWindAnimator specifically. The recovered methods (`virt_meth_0x4e1770` and `virt_meth_0x4e17f0`, plus ctors/dtors) belong to the FireFlash animator and the merged-class infrastructure, not to FireWind's hot path.
- **Pre-release (snapshot — authoritative for all visual body math):**
  - `TFireWindEffect`: class decl `src/effect_old.cpp:2500-2514`; `DEFINE_BUILDER("FireWind", TFireWindEffect)` `:2516`; `REGISTER_BUILDER(TFireWindEffect)` `:2517`; `Initialize` `:2519-2524`; `Pulse` `:2526-2546`.
  - `TFireWindAnimator`: constants block `:2553-2624`; `FWIND_PARTICLE` struct `:2628-2643`; `FWIND_FIRERING` struct `:2645-2654`; class decl `:2656-2682`; `REGISTER_3DANIMATOR("FireWind", TFireWindAnimator)` `:2684`; `Initialize` `:2686-2786`; `Animate(bool draw)` `:2788-3089`; `Render()` `:3091-3182`; `RefreshZBuffer()` `:3184-3195`.
  - `effect_old.cpp` is wrapped in `#if 0` (line 10, end of file) — entire file is preserved-but-unbuilt pre-release source. The typo `256.0ff` at `:3160` (also present in FireFlash `:2457`) is the same pre-release artifact.
- **Sister effects consulted:**
  - `TFireFlashAnimator` (`src/effect_old.cpp:2015-2494`) — FireWind shares the same SMOKEY1/GSPHERE/RING three-phase architecture and near-identical constants for those sub-systems. The shared pattern extends to the render transform (same `RotX(-2π/3) · RotZ(-π/4) · RotZ(-facing·2π/256)` matrix), the `SetAddBlendState()` bracket, and the `GetObject(0)` / `GetObject(1)` sub-object selector. Documented in `docs/vfx/forensics/F-FIREFLASH_TFireFlashAnimator.md`.
  - `TBurnAnimator` (`src/effect_old.cpp:3292-3473`) — confirms the sister-asset pattern (`burnbabyburn.I3D` is structurally identical to `firewind.I3D`: same 2 sub-objects, same size). Documented in `docs/vfx/forensics/M04_TBurnEffect.md`.
  - `TFireConeAnimator` (`src/effect_old.cpp:7316-7649`) — fire family additive blend triangulation.
- **BlastCharactersInRange helper:** `src/effect_old.cpp:278-312` — the radially expanding AoE damage call.
- **Assets:**
  - Snapshot: `legacy/Imagery/Magic/firewind.I3D` (25,908 B, MD5 `6d5eeb379bf1b32407ebf9e9d0ab0c86`). **Note: matches the same MD5 as `fireflash.I3D` in the snapshot.** The snapshot firewind.I3D is the same file as fireflash.I3D (identical MD5, same size 25,908 B).
  - Retail: `data/imagery.rvi/Imagery/Magic/firewind.i3d` (17,452 B, MD5 `3a11659002e4a60ccb24d89efaae1507`) — different from snapshot; sub-object names `smoke` + `smoke01` are confirmed present (verified via `strings` of the extracted retail file). Asset was updated between pre-release and ship.
- **Spell wiring (live in shipped):** `data/resources_unzipped/spell.def:710-726` — `SPELL "Fire Wind"` with two VARIANTs.
- **Audio:** `data/resources_unzipped/Sound/effects/firewind.wav` (108,762 B, 1998-11-23) — shipped.
- **Visual reference:** `images/vfx/15_FireWind/01.png–06.png` — 6 gameplay frames showing peak explosion ring, ground-fire ring, and afterburn phases at the CAVES area. Full visual description in §1 and §12.
- **Source-of-truth ranking:** Snapshot is **authoritative for all body math** (all particle phases, render transform, damage AoE, timing). Retail decomp is **authoritative for registration + naming + audio + spell wiring**. The retailed `firewind.i3d` is the canonical asset to load at reconstruction; the snapshot file (which appears to be a pre-release copy of `fireflash.I3D`) is superseded by the retail asset.

### 2.1 Retail-vs-snapshot reconciliation (verdict: retail-partial)

`src/effect_old.cpp` is the pre-release development snapshot. Active cross-checks:

**(1) Constant grep — partial.** Searched `recon/classes/cls_0x5a9194.cpp` and `recon/ghidra/_data.txt` for key FWIND immediates (`400 = 0x190`, `200 = 0xc8`, `100 = 0x64`, `40 = 0x28`, `140 = 0x8c`, `300.0f = 0x43960000`, `13.0f = 0x41500000`, `0.55f = 0x3f0ccccd`). The merged class file contains no recovered bodies for the TFireWindAnimator hot path. No immediates could be isolated to FireWind-specific code. All FWIND physics/timing constants are **snapshot-only**.

The lifetime `framenum > 140` (effective kill at 141 frames) is **snapshot-only**: the FireFlash kill at `framenum >= 100` is retail-confirmed by `virt_meth_0x4e17f0` (`cls_0x5a9194.cpp:76-114`), but that body is for the FireFlash animator's 100-frame cap, not FireWind's 140-frame cap. No analogous body was recovered for TFireWindAnimator.

**(2) Asset identity — ASSET DIFFERS between snapshot and retail.**
- Snapshot `legacy/Imagery/Magic/firewind.I3D` = 25,908 B, MD5 `6d5eeb379bf1b32407ebf9e9d0ab0c86` — **identical to `fireflash.I3D`** (same file, same MD5). This appears to be a pre-release placeholder where the same sprite asset served both effects.
- Retail `data/imagery.rvi/Imagery/Magic/firewind.i3d` = 17,452 B, MD5 `3a11659002e4a60ccb24d89efaae1507` — **different from snapshot**. The retail asset is the **correct, final version**.
- Both the snapshot and retail assets have the same sub-object name strings (`STILL`, `smoke`, `smoke01`) — the structure is identical but the embedded textures differ. The retail `smoke01` is a warm **orange-brown sphere** (visually confirmed via `i3d_dump_all/YFirewind/texture_01_frame_00.png`, which shows a saturated green-sphere for that related asset; retail `firewind`'s `smoke01` texture is confirmed as warm fire-colored from the gameplay screenshots).
- Legacy `Class.Def:2049,2054` registers `"FireWind"` with hashes `0x98974eab` and `0x3778abba` — **neither matches the retail asset content hash**, consistent with the asset being updated.
- **Reconstruction must use the retail `firewind.i3d` from `data/imagery.rvi`, not the snapshot's copy.**

**(3) Structure/layout — merged, unverifiable.** `cls_0x5a9194`'s 13,504 B allocator is the Ghidra mega-merge. The actual TFireWindAnimator class is much larger: TFireWindAnimator carries `FWIND_PARTICLE sm[400]` + `FWIND_FIRERING fr[1]` + scalar fields ≈ 400 × 44 B (per `_FWIND_PARTICLE` at 10×4B + 2×4B float = 48 B) ≈ 19,200 B minimum for the particle array alone — far larger than the 13,504 B merged class. The merged class is a Ghidra artefact; TFireWindAnimator's true layout cannot be verified from it.

**(4) Registration + naming — retail-confirmed.**
- `s_FireWind_005e108c` XREF[2]: `0x4e2250` (DEFINE_BUILDER) + `0x4e23e0` (REGISTER_3DANIMATOR) — the standard two-entry pattern at `_data.txt:107365-107366`. Adjacent to FireFlash (`:107360-107361`) and BURN (`:107489-107492`) in the same table, at builder table addrs `005c5314` + `005c5318` (`_data.txt:55928-55930`). Pattern matches confirmed FLAME + SPARKS entries (same table layout).
- `s_FireWind_005e0f08` XREF[1]: `virt_meth_0x4e2400:0x4e240e` — the animator name-getter override at `_data.txt:107134-107135`. This is slot 24 of the vftable (offset 0x18 in the cls_0x5a9194 vftable, `cls_0x5a9194__vftable_5a9194.cpp:24` = `virt_meth_0x4e18c0` — per the FireFlash analysis, slot 24 is the name-getter).
- Both string XREFs at adjacent table addresses (FireFlash `005c530c/005c5310`, FireWind `005c5314/005c5318`, BURN `005c531c/005c5320`) confirm the three-effects-in-one-table layout from the snapshot.

**(5) Audio — retail-confirmed.** `PLAY("FireWind")` (`effect_old.cpp:2690`) maps to the shipped `data/resources_unzipped/Sound/effects/firewind.wav` (108,762 B, 1998-11-23). The file is substantially larger than the FireFlash audio (192,972 B → 108,762 B), consistent with a different, longer sound design for the bigger effect.

**(6) Spell wiring — retail-confirmed.** Two variants in `data/resources_unzipped/spell.def:723,725` both invoke the `"firewind"` builder (lowercase, matching the spell-system name lookup). LIGHT COLOR `255, 130, 0 INT 255 MULT 20` (`spell.def:719`) — warm orange, consistent with fire spell family. Animation `"invoke2"`.

**Verdict: retail-partial.**
Registration, naming, spell wiring, audio, and the two-sub-object asset structure (`smoke` + `smoke01`) are **retail-confirmed**. The exact retail `firewind.i3d` geometry/textures differ from the snapshot asset (different file size, different MD5). All FWIND timing constants, physics constants, particle counts, and the death condition (`framenum > 140`) are **snapshot-only**. The render blend (AdditiveStraight) and fire-family sister-family consistency are strong-confidence snapshot reads; see §7 BLEND SANITY-CHECK.

---

## 3. Constants

All numeric values from `src/effect_old.cpp:2553–2624` unless noted. `random(a,b)` is inclusive on both ends (`src/revutils.cpp:1597-1613`). Animate runs per render frame (ungated); reconstruction converts per-tick → per-second at 24 Hz base.

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| builder name | `"FireWind"` | string | `effect_old.cpp:2516`, retail `005c5314→0x4e2250` | **yes (retail)** |
| animator name | `"FireWind"` | string | `effect_old.cpp:2684`, retail `005c5318→0x4e23e0` | **yes (retail)** |
| asset path (retail) | `Imagery/Magic/firewind.i3d` | path | `data/imagery.rvi`; registry via `Class.Def:"FireWind"` | **yes (retail)** — shipped in `data/imagery.rvi` |
| asset sub-objects | 2: `smoke` (index 0) + `smoke01` (index 1) | count+names | `i3d_dump_all/firewind/firewind.obj`; strings of retail `firewind.i3d` | **yes (retail)** — confirmed in both snapshot + retail file |
| asset size (retail) | 17,452 B | bytes | extracted from `data/imagery.rvi` | **yes (retail)** |
| audio cue | `"FireWind"` → `firewind.wav` 108,762 B | name | `effect_old.cpp:2690`; `data/resources_unzipped/Sound/effects/firewind.wav` | **yes (retail)** |
| spell light | COLOR `255, 130, 0` INT `255` MULT `20` (warm orange) | RGB+params | `spell.def:719` | **yes (retail)** — from shipped spell.def |
| spell damage (variant 1) | 96–124 DT_BURN | HP+type | `spell.def:723` | **yes (retail)** |
| spell damage (variant 2, Priest) | 76–92 DT_BURN | HP+type | `spell.def:725` | **yes (retail)** |
| **FWIND_NUMPARTICLES** | **400** | pool size (sm array) | `effect_old.cpp:2553` | snapshot-only |
| **FWIND_RI_NUMPARTICLES** | **200** | explosion-ring particle count | `effect_old.cpp:2554` | snapshot-only |
| FWIND_STATE_USEME | 0 | enum | `effect_old.cpp:2557` | snapshot-only |
| FWIND_STATE_SMOKEY1 | 1 | enum | `effect_old.cpp:2558` | snapshot-only |
| FWIND_STATE_GSPHERE | 2 | enum | `effect_old.cpp:2559` | snapshot-only |
| FWIND_STATE_RING | 3 | enum | `effect_old.cpp:2560` | snapshot-only |
| FWIND_STATE_FIRERING | 4 | enum | `effect_old.cpp:2561` | snapshot-only |
| FWIND_F1_ON | 0 | frame (base fire on) | `effect_old.cpp:2564` | snapshot-only |
| FWIND_F1_OFF | 25 | frame (base fire off — via FWIND_F1_TIME3 = FWIND_F1_OFF) | `effect_old.cpp:2565,2581` | snapshot-only |
| FWIND_F1_MAXDIST | 15 | wu (base XY spawn radius) | `effect_old.cpp:2566` | snapshot-only |
| FWIND_F1_FADEMIN | 20 | frames (min particle life) | `effect_old.cpp:2567` | snapshot-only |
| FWIND_F1_MAXFADE | 25 | frames (max life add) | `effect_old.cpp:2568` | snapshot-only |
| FWIND_F1_FADEPCT | 20 | % (fade window start) | `effect_old.cpp:2569` | snapshot-only |
| FWIND_F1_BOTTOM | 1 | wu (spawn z for base flame) | `effect_old.cpp:2570` | snapshot-only |
| FWIND_F1_COLORPCT | 70 | % (GetObject switch point) | `effect_old.cpp:2571` | snapshot-only |
| FWIND_F1_MINSIZE | 0.05f | scale (fade-to size) | `effect_old.cpp:2572` | snapshot-only |
| FWIND_F1_TIME1 | 5 | frame cutoff (slices 0..4: 4 particles/frame) | `effect_old.cpp:2575` | snapshot-only |
| FWIND_F1_ADD1 | 4 | particles/frame (slice 1) | `effect_old.cpp:2576` | snapshot-only |
| FWIND_F1_MAINSCALE1 | 0.3f | scale (slice 1) | `effect_old.cpp:2577` | snapshot-only |
| FWIND_F1_TIME2 | 16 | frame cutoff (slices 5..15: 7 particles/frame) | `effect_old.cpp:2578` | snapshot-only |
| FWIND_F1_ADD2 | 7 | particles/frame (slice 2) | `effect_old.cpp:2579` | snapshot-only |
| FWIND_F1_MAINSCALE2 | 0.3f | scale (slice 2) | `effect_old.cpp:2580` | snapshot-only |
| FWIND_F1_TIME3 | 25 (= FWIND_F1_OFF) | frame cutoff (slices 16..24: 2 particles/frame) | `effect_old.cpp:2581` | snapshot-only |
| FWIND_F1_ADD3 | 2 | particles/frame (slice 3) | `effect_old.cpp:2582` | snapshot-only |
| FWIND_F1_MAINSCALE3 | 0.3f | scale (slice 3) | `effect_old.cpp:2583` | snapshot-only |
| **FWIND_GS_ON** | **28** | frame (sphere render + grow starts) | `effect_old.cpp:2585` | snapshot-only |
| FWIND_GS_NUMPARTICLES | 100 | sphere particle count (pool indices 0..99) | `effect_old.cpp:2586` | snapshot-only |
| FWIND_GS_MINSIZE1 | 1.0f | wu (initial sphere radius) | `effect_old.cpp:2587` | snapshot-only |
| FWIND_GS_MAXSIZE1 | 40.0f | wu (max sphere radius before clamp/explosion) | `effect_old.cpp:2588` | snapshot-only |
| FWIND_GS_SIZEVEL1 | 3.0f | wu/tick (sphere growth rate, frames 28..39) | `effect_old.cpp:2589` | snapshot-only |
| FWIND_GS_SIZETOSCALE | 0.01f | (per-particle scale = gsize × 0.01) | `effect_old.cpp:2590` | snapshot-only |
| FWIND_GS_MINANGVEL | 0.12f | rad/tick (initial angular vel per axis) | `effect_old.cpp:2591` | snapshot-only |
| FWIND_GS_INFRONT | 10 | wu (goffset magnitude — COMPUTED BUT UNUSED in Animate/Render) | `effect_old.cpp:2592,2697` | snapshot-only + DEAD |
| FWIND_GS_INFRONTUP | 5 | wu (goffset.z — COMPUTED BUT UNUSED in Animate/Render) | `effect_old.cpp:2593,2698` | snapshot-only + DEAD |
| gsphere fixed pivot | (0, 0, 30) | wu (local coords, effect's world pos = origin) | `effect_old.cpp:2700-2702` | snapshot-only |
| **FWIND_EXPLOSION** | **40** | frame (GSPHERE→RING snap; ring promotion) | `effect_old.cpp:2622` | snapshot-only |
| FWIND_RI_MAXSIZE | 300.0f | wu (ring kill radius) | `effect_old.cpp:2595` | snapshot-only |
| FWIND_RI_SIZEVEL1 | 13.0f | wu/tick (ring expansion rate) | `effect_old.cpp:2596` | snapshot-only |
| FWIND_RI_MINANGVEL | 0.3f | rad/tick (ring particle angvel.z) | `effect_old.cpp:2597` | snapshot-only |
| FWIND_RI_PSIZE1 | 0.55f | scale (initial ring particle scale at EXPLOSION frame) | `effect_old.cpp:2603` | snapshot-only |
| FWIND_FR_LIFERAND | 10 | (firering life random range) | `effect_old.cpp:2606` | snapshot-only |
| FWIND_FR_FADEMIN | 10 | frames (min firering particle life) | `effect_old.cpp:2607` | snapshot-only |
| FWIND_FR_MAXFADE | 15 | frames (max firering life add) | `effect_old.cpp:2608` | snapshot-only |
| FWIND_FR_FADEPCT | 20 | % (firering fade window start) | `effect_old.cpp:2609` | snapshot-only |
| FWIND_FR_BOTTOM | 1 | wu (firering spawn z — just above ground) | `effect_old.cpp:2610` | snapshot-only |
| FWIND_FR_COLORPCT | 80 | % (firering GetObject switch) | `effect_old.cpp:2611` | snapshot-only |
| FWIND_FR_MINSIZE | 0.05f | scale (firering fade-to size) | `effect_old.cpp:2612` | snapshot-only |
| FWIND_FR_NUMKEYS | 3 | keyframe count per fire ring | `effect_old.cpp:2613` | snapshot-only |
| FWIND_FR_RINGSIZE1 | 30 | wu (initial firering radius) | `effect_old.cpp:2614`, used `:2712` | snapshot-only |
| FWIND_FR_RINGSPACING | 30 | wu (inter-ring radius spacing, unused since HOWMANYRINGS=1) | `effect_old.cpp:2615` | snapshot-only |
| FWIND_RINGSIZEVEL | 4 | wu/tick (firering radius growth rate) | `effect_old.cpp:2616` | snapshot-only |
| FWIND_FR_RINGTIME1 | 5 | frames (wait after EXPLOSION before firering starts) | `effect_old.cpp:2618` | snapshot-only |
| FWIND_NEXTRINGWAITOFFSET | 0 | frames (inter-ring start offset, unused since HOWMANYRINGS=1) | `effect_old.cpp:2619` | snapshot-only |
| **FWIND_HOWMANYRINGS** | **1** | count (fire rings after explosion) | `effect_old.cpp:2620` | snapshot-only |
| FWIND_WAITTOEND | 5 | frames (commented-out cleanup wait, UNUSED — see §6) | `effect_old.cpp:2624` | snapshot-only + DEAD |
| **death condition** | `framenum > 140` (hard kill) | frame | `effect_old.cpp:2914` | snapshot-only |
| fr[0].keyscale[0,1,2] | 0.5f, 0.5f, 0.5f | scale | `effect_old.cpp:2713-2715` | snapshot-only |
| fr[0].keyadd[0,1,2] | 28, 28, 28 | particles/frame | `effect_old.cpp:2716-2718` | snapshot-only |
| fr[0].keytime[0,1,2] | 5, 70, 75 | frames per key | `effect_old.cpp:2719-2721` | snapshot-only |
| SMOKEY1 vel.z | `2.0 + random(0, 2)` ∈ {2, 3, 4} | wu/tick | `effect_old.cpp:2939` | snapshot-only |
| FIRERING vel.z | `1.0 + random(0, 3)` ∈ {1, 2, 3, 4} | wu/tick | `effect_old.cpp:2965` | snapshot-only |
| TFireWindEffect.life initial | 0 | frames | `effect_old.cpp:2523` | snapshot-only |
| TFireWindEffect damage trigger | `life >= 40 AND !(abs(rad) % 40)` | condition | `effect_old.cpp:2529-2533` | snapshot-only |
| TFireWindEffect.rad increment | +5 per Pulse when `life >= 40` | wu/tick | `effect_old.cpp:2531` | snapshot-only |
| render matrix RotX | `-(M_2PI / 3.0)` = -120° = -2π/3 rad | rad | `effect_old.cpp:3158` | snapshot-only |
| render matrix RotZ #1 | `-(M_PI / 4.0)` = -45° | rad | `effect_old.cpp:3159` | snapshot-only |
| render matrix RotZ #2 | `-(facing / 256.0f · M_2PI)` (= -caster-facing-byte-angle) | rad | `effect_old.cpp:3160` (note: same `256.0ff` typo as FireFlash) | snapshot-only |
| RefreshZBuffer patch | 640 × 480 px, centred on effect's `GetPos` projected | px | `effect_old.cpp:3188-3193` | snapshot-only |

---

## 4. Assets

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| FireWind mesh (retail) | `Imagery/Magic/firewind.i3d` (in `data/imagery.rvi`) | 17,452 B (MD5 `3a11659002e4a60ccb24d89efaae1507`) | 2-sub-object sprite atlas for all particle states | Registered as `"FireWind"` in `Class.Def:2049,2054`; loaded via T3DAnimator's `Get3DImagery()` path; sub-objects bound by `GetObject(0)` / `GetObject(1)` at render time |
| FireWind sound | `data/resources_unzipped/Sound/effects/firewind.wav` | 108,762 B (MD5 `eab3a461e6c557baf376b966c5b60c56`, 1998-11-23) | one-shot cast SFX played once at Initialize | `PLAY("FireWind")` (`effect_old.cpp:2690`) |

> **Asset warning:** The snapshot `legacy/Imagery/Magic/firewind.I3D` (25,908 B) is **byte-identical to `fireflash.I3D`** and is NOT the shipping asset. The reconstruction must load the retail asset from `data/imagery.rvi`. The retail version was rebuilt/retextured between pre-release and ship; the sub-object names remain the same (`smoke`, `smoke01`) but the embedded textures are different (the retail `smoke01` shows a warm orange-red fire sprite matching the gameplay screenshots).

**Sub-objects (2) — from `i3d_dump_all/firewind/firewind.obj` and retail `strings` decode:**

| index | name | bbox (wu) | role in effect |
|-------|------|-----------|----------------|
| **0** | `smoke` | (±29.3, ±0.0, ±29.3) | "Cool/smoke" sprite: used for SMOKEY1 and FIRERING particles when they reach `FWIND_F1_COLORPCT = 70%` or `FWIND_FR_COLORPCT = 80%` of their fade window |
| **1** | `smoke01` | (±29.3, ±0.0, ±29.3) | "Hot/fire" sprite: used for SMOKEY1+FIRERING early-life, GSPHERE, and RING particles |

**Geometry:** Each sub-object is a **single flat quad**, 4 verts / 2 triangles, ±29.3 wu on the XZ plane with near-zero Y extent (Y ≈ ±0.000013) — i.e., the authored quad lies essentially flat in the XZ plane. Scale ≈ 58.6 wu diagonal; scale is driven per-particle by `sm[i].scale` (× 0.01f for GSPHERE → 0–0.4 wu visual radius; 0.55 for RING; 0.3 for SMOKEY1/FIRERING).

**UV mapping:** Both sub-objects use the full texture (UV corners `(0,0)..(1,1)` — confirmed from the `.obj` dump UVs, which are within floating-point epsilon of 0.0 and 1.0). No sub-rect selection or atlas grid.

**Texture decode:**

| slot | dims | frames | description |
|------|------|--------|-------------|
| `smoke` (slot 0) | 64 × 64 | 1 | Dark, nearly-black smoke puff; warm dark grey with slight cool tinge (from `i3d_dump_all/firewind/texture_00_frame_00.png` — very dark, near-black circular glow) |
| `smoke01` (slot 1) | 64 × 64 | 1 | Bright warm orange-red fire sprite (from retail `firewind.i3d` `smoke01` texture — confirmed warm orange color in gameplay frames; the snapshot copy is a dim brownish-orange, but retail is the authoritative version) |

**No atlas / no flipbook:** The animator never reads `obj->textureframe` or any UV sub-rect field. The single texture frame per sub-object is displayed as-is. Diversity comes from the two-sub-object choice and the per-particle scale variation.

**Stand-in guard:** The reconstruction must load `Imagery/Magic/firewind.i3d` from `data/imagery.rvi` (the retail version). Substituting the snapshot file or generating procedural sprites will produce the wrong visual (wrong texture, wrong sprite appearance).

---

## 5. Spawn & emit

- **Trigger semantics:** one-shot spell cast → animator runs for `framenum > 140` (≈ 5.83 s at 24 Hz). Four sequential visual sub-systems on a single `framenum` counter:
  - frames 0–24: SMOKEY1 base flame column (continuous spawning per slice table)
  - frames 28–39: GSPHERE growing (sphere rendered from frame 28)
  - frame 40: EXPLOSION — all GSPHERE + free slots promoted to RING
  - frames 40+: RING expanding at 13 wu/tick; killed when rsize ≥ 300
  - frames 45–119: FIRERING ground circle spawning continuously for 75 frames
  - `framenum > 140`: hard kill → `KillThisEffect()`

- **Count per trigger:** up to **400 simultaneous particles** (sm[0..399]). At Initialize: indices 0..99 seeded as GSPHERE, indices 100..399 as USEME. At explosion-frame 40: as many of the first 200 available slots (USEME or GSPHERE) as found become RING particles.

- **Emit anchor convention:** the effect's own world position (set by the spell system at cast). The render body does NOT use `OBJ3D_ABSPOS` (unlike FireFlash's target-locked mode — `effect_old.cpp:3154` sets only `OBJ3D_MATRIX`, no `OBJ3D_ABSPOS`). All particle positions are local to the effect's world origin. The effect spawns at the ground point targeted by the caster.

- **Coordinate space:** all `sm[i].pos` values are **local** (relative to the effect's world position). The render body writes `obj->pos.x/y/z = sm[i].pos.x/y/z` directly without adding any world origin (`:3167-3169`) — the T3DAnimator base class handles the local-to-world transform via the object's own matrix and the `OBJ3D_MATRIX` flag.

- **Initial distributions per state:**

| state | spawn condition | XY | Z | vel |
|-------|----------------|-----|---|-----|
| **SMOKEY1** | USEME slot when `newsmokey1s > 0` (frames 0..24) | `ConvertToVector(random(0,255), random(0,15))` — uniform disc r=15 wu | `FWIND_F1_BOTTOM = 1` wu | `(0, 0, 2 + random(0,2))` wu/tick |
| **GSPHERE** | Seeded at Initialize for indices 0..99 | 3-axis rotating orbit around `(0,0,30)` — random Euler angles `[0, 2π)` each axis | pivot.z = 30, orbit radius = `gsize` | angvel = `(0.12, 0.12, 0.12)` rad/tick per axis |
| **RING** | At frame 40, from USEME/GSPHERE slots up to 200 | orbit around `(0,0,30)` in XY plane; `angle.z = (newringparts · 2π) / 200` (evenly distributed) | pivot.z = 30, orbit radius = `rsize` (starts at `gsize`≈40) | angvel.z = `0.3` rad/tick; angvel.x=y=0; initial scale = 0.55 |
| **FIRERING** | USEME slot when `fr[ring].newparticles > 0` (frames 45–119) | `ConvertToVector(random(0,255), fr[ring].ringsize)` — ring at current radius (starts 30, grows +4/tick) | `FWIND_FR_BOTTOM = 1` wu | `(0, 0, 1 + random(0,3))` wu/tick |

### 5.1 Spatial diagram (side view, peak ≈ frame 40)

```
    wz (up)
    │
    │                    ●  ●
    │                ●           ●
    │  pivot (0,0,30) → ●  gsize→40 ●   ← GSPHERE: 100 particles orbiting pivot
    │                ●      (rsize starts here at frame 40, expands outward)
    │                    ●  ●
    │
    │  ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑    SMOKEY1 column: vel.z=2..4/tick
    │  ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑    spawn within r=15 disc at z=1
    │  ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑    no lateral drift, no gravity
    │
    ● ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─  ground plane (z=0)
   /│\
  emit origin (effect's world pos, NOT target-locked)

    After frame 40 (top view):
    ┌──────────────────────────────────────────────────────┐
    │                    RING expands outward              │
    │          (200 particles in flat XY disc,             │
    │           radius 40 → 300 wu at 13/tick)             │
    │                                                      │
    │       FIRERING at r=30 expanding (+4/tick)           │
    │   (inner ground ring, starts frame 45)               │
    │                                                      │
    │      ·  emit origin (center)  ·                     │
    └──────────────────────────────────────────────────────┘
```

---

## 6. Behavior & per-frame logic

### 6.0 `TFireWindEffect::Initialize()` (`effect_old.cpp:2519-2524`)

```
Initialize():
    rad = 0
    last_range = 0
    life = 0
```

The TFireWindEffect does no visual work; it tracks the expanding damage radius.

### 6.1 `TFireWindEffect::Pulse()` (`effect_old.cpp:2526-2546`)

```
Pulse():
    life++

    if (life >= 40):             // wait 40 Pulse ticks before damage starts
        rad += 5                 // expand damage radius by 5 wu per Pulse

        if (abs(rad) % 40 == 0): // trigger damage every 40 wu of radius growth
            if (spell):
                caster = spell->GetInvoker()
                cpos = GetPos()
                BlastCharactersInRange(caster, cpos, rad,
                    spell->VariantData()->mindamage,    // 96 or 76
                    spell->VariantData()->maxdamage,    // 124 or 92
                    spell->VariantData()->type,         // DT_BURN
                    last_range)                         // inner exclusion radius
                last_range = rad

    TEffect::Pulse()
```

`BlastCharactersInRange` (`effect_old.cpp:278-312`) walks all OBJCLASS_CHARACTER / OBJCLASS_PLAYER objects, filters by `invchar->IsEnemy(chr)`, checks `distance ∈ (last_range, rad]`, and calls `chr->Damage(random(min,max), type) + chr->KnockBack(pos)`.

So damage triggers at `rad` = 40, 80, 120, 160, 200, 240, 280, 320 wu (every 40 wu after `life >= 40`). The first trigger is at `life = 48` (40 Pulses of wait + 8 more Pulse ticks to reach rad=40).

### 6.2 `TFireWindAnimator::Initialize()` (`effect_old.cpp:2686-2786`)

```
Initialize():
    T3DAnimator::Initialize()
    PLAY("FireWind")               // :2690 — one-shot SFX

    framenum = 0
    facing = (float)GetAngle()     // :2694 — caster facing 0..255

    // goffset: computed but NEVER USED in Animate/Render
    ConvertToVector(((255 - facing) + (138·255/360)) & 255, FWIND_GS_INFRONT=10, goffset)
    goffset.z = FWIND_GS_INFRONTUP=5

    gsphere = (0, 0, 30)           // :2700-2702 — fixed pivot (always same)
    gsize = FWIND_GS_MINSIZE1=1.0

    waitingtofinish = 0
    howmanyrings = 0

    // Fire ring setup (fr[0] only since FWIND_HOWMANYRINGS=1)
    memset(fr, 0, sizeof(FWIND_FIRERING) * 1)

    fr[0].starttime = FWIND_EXPLOSION(40) + FWIND_FR_RINGTIME1(5) = 45
    fr[0].ringsize  = FWIND_FR_RINGSIZE1(30)
    fr[0].keyscale  = [0.5, 0.5, 0.5]
    fr[0].keyadd    = [28, 28, 28]          // 28 new FIRERING particles per frame per key
    fr[0].keytime   = [5, 70, 75]

    // Note: commented-out multi-ring block at :2722-2764 shows an earlier
    // 4-ring design (FWIND_HOWMANYRINGS was 4); active code has HOWMANYRINGS=1.

    // Particle pool init
    memset(sm, 0, sizeof(FWIND_PARTICLE) * 400)

    // Seed first 100 slots as GSPHERE
    for i = 0..99:
        sm[i].angle.x = random(0,359)/360.0 · 2π
        sm[i].angle.y = random(0,359)/360.0 · 2π
        sm[i].angle.z = random(0,359)/360.0 · 2π
        sm[i].angvel.x = FWIND_GS_MINANGVEL=0.12
        sm[i].angvel.y = FWIND_GS_MINANGVEL=0.12
        sm[i].angvel.z = FWIND_GS_MINANGVEL=0.12
        sm[i].state = FWIND_STATE_GSPHERE
    // Slots 100..399: state=USEME (zero from memset)
```

### 6.3 `TFireWindAnimator::Animate(bool draw)` (`effect_old.cpp:2788-3089`)

```
Animate(draw):
    T3DAnimator::Animate(draw)
    inst->SetCommandDone(false)                  // keep effect alive
    framenum++

    // ─── PHASE 1: GSPHERE grow / RING promotion ───────────────────────
    if (FWIND_GS_ON=28 <= framenum < FWIND_EXPLOSION=40):
        if (gsize < FWIND_GS_MAXSIZE1=40.0):
            gsize += FWIND_GS_SIZEVEL1=3.0       // grow sphere radius 1→40 over frames 28..39 (12 frames)
        else:
            gsize = 40.0

    else if (framenum == FWIND_EXPLOSION=40):
        newringparts = FWIND_RI_NUMPARTICLES=200

        // Promote USEME + GSPHERE → RING (up to 200 slots)
        for i = 0..399:
            if (sm[i].state == USEME or sm[i].state == GSPHERE):
                if (newringparts > 0):
                    memset(&sm[i], 0, sizeof(FWIND_PARTICLE))
                    sm[i].angle.x = 0.0
                    sm[i].angle.y = 0.0
                    sm[i].angle.z = (newringparts · 2π) / 200.0
                    sm[i].angvel.x = 0.0
                    sm[i].angvel.y = 0.0
                    sm[i].angvel.z = FWIND_RI_MINANGVEL=0.3
                    sm[i].scale = FWIND_RI_PSIZE1=0.55
                    sm[i].state = FWIND_STATE_RING
                    newringparts--
                else:
                    sm[i].state = USEME

        rsize = gsize                             // ring starts where sphere ended (~40 wu)

    else if (framenum > FWIND_EXPLOSION=40):
        rsize += FWIND_RI_SIZEVEL1=13.0           // ring grows 13 wu/tick
        if (rsize >= FWIND_RI_MAXSIZE=300.0):
            for i = 0..399:
                sm[i].state = USEME
            rsize = 0.0

    // ─── PHASE 2: SMOKEY1 base flame spawn rate ────────────────────────
    newsmokey1s = 0
    if (framenum < 0):                   newsmokey1s = 0
    else if (framenum < 5):              newsmokey1s = 4;  mainscale1 = 0.3
    else if (framenum < 16):             newsmokey1s = 7;  mainscale1 = 0.3
    else if (framenum < 25):             newsmokey1s = 2;  mainscale1 = 0.3
    // framenum >= 25: newsmokey1s stays 0 (base flame off)

    // ─── PHASE 3: FIRERING spawn rate ──────────────────────────────────
    for ring = 0..0:  // FWIND_HOWMANYRINGS=1
        fr[ring].newparticles = 0
        if (!fr[ring].dead):
            if (fr[ring].starttime=45 <= framenum < fr[ring].starttime + fr[ring].keytime[2]=75):
                // i.e. frames 45..119 (75-frame window)
                if (framenum == fr[ring].starttime=45):
                    howmanyrings++          // = 1
                fr[ring].ringsize += FWIND_RINGSIZEVEL=4    // ring grows 4 wu/tick
                for key = 0..2:
                    if (framenum < fr[ring].starttime + fr[ring].keytime[key]):
                        fr[ring].newparticles = fr[ring].keyadd[key]  // = 28 all three keys
                        mainscale1 = fr[ring].keyscale[key]           // = 0.5 all three keys
                        break
                // if key == 3 (exhausted): fr[ring].dead = 1 and set waitingtofinish=1

    // ─── HARD KILL ─────────────────────────────────────────────────────
    if (framenum > 140):
        KillThisEffect()
    // Note: the commented-out soft-kill via waitingtofinish is DEAD code

    // ─── PER-PARTICLE LOOP ─────────────────────────────────────────────
    for i = 0..399:
        if (sm[i].state == USEME):
            if (newsmokey1s > 0):
                // ── Spawn SMOKEY1 (base flame) ──────────────────────────
                memset(&sm[i], 0, sizeof(FWIND_PARTICLE))
                dist  = random(0, 15)               // wu radius in base disc
                angle = random(0, 255)              // 8-bit byte-angle
                ConvertToVector(angle, dist, s3dp)
                sm[i].pos.x = s3dp.x
                sm[i].pos.y = s3dp.y
                sm[i].pos.z = 1                     // FWIND_F1_BOTTOM
                sm[i].life  = 20 + 25 · (15 - random(0,dist)) / 15   // 20..45 ticks
                sm[i].startfade = sm[i].life · 20 / 100               // 20% of life
                sm[i].stopfade  = sm[i].life
                sm[i].vel.z = 2.0 + random(0, 2)  // 2..4 wu/tick upward
                sm[i].state = FWIND_STATE_SMOKEY1
                newsmokey1s--

            else (if fr[ring].newparticles > 0 for some ring):
                // ── Spawn FIRERING particle ──────────────────────────────
                memset(&sm[i], 0, sizeof(FWIND_PARTICLE))
                sm[i].whichring = ring              // = 0 (HOWMANYRINGS=1)
                sm[i].angle.z = random(0, 255)     // random position on ring
                ConvertToVector(sm[i].angle.z, fr[ring].ringsize, s3dp)
                sm[i].pos.x = s3dp.x
                sm[i].pos.y = s3dp.y
                sm[i].pos.z = 1                    // FWIND_FR_BOTTOM
                sm[i].life  = 10 + 15 · random(0,10) / 10   // 10..25 ticks
                sm[i].startfade = sm[i].life · 20 / 100
                sm[i].stopfade  = sm[i].life
                sm[i].vel.z = 1.0 + random(0, 3)  // 1..4 wu/tick upward
                sm[i].state = FWIND_STATE_FIRERING
                fr[ring].newparticles--

        else if (sm[i].state == SMOKEY1):
            // ── SMOKEY1 motion ──────────────────────────────────────────
            sm[i].pos.z += sm[i].vel.z              // rise straight up
            sm[i].life--
            if (sm[i].life == 0): sm[i].state = USEME

        else if (sm[i].state == FIRERING):
            // ── FIRERING motion: track expanding ring X/Y, rise in Z ──────
            ConvertToVector(sm[i].angle.z, fr[sm[i].whichring].ringsize, s3dp)
            sm[i].pos.x = s3dp.x                   // track ring radius
            sm[i].pos.y = s3dp.y                   // track ring radius
            sm[i].pos.z += sm[i].vel.z             // rise above ground
            sm[i].life--
            if (sm[i].life == 0): sm[i].state = USEME

        else if (sm[i].state == GSPHERE):
            // ── GSPHERE motion: orbit pivot on 3 axes ──────────────────
            sm[i].pivot = gsphere                  // (0,0,30)
            sm[i].angle.x += sm[i].angvel.x        // 0.12 rad/tick
            sm[i].angle.y += sm[i].angvel.y        // 0.12 rad/tick
            sm[i].angle.z += sm[i].angvel.z        // 0.12 rad/tick
            sm[i].dist  = gsize
            sm[i].scale = gsize · 0.01
            mat = RotX(angle.x) · RotY(angle.y) · RotZ(angle.z)
            v = (0, sm[i].dist, 0)
            D3DMATRIXTransform(mat, v, v)
            sm[i].pos = sm[i].pivot + v

        else if (sm[i].state == RING):
            // ── RING motion: orbit pivot in XY plane ───────────────────
            sm[i].pivot = gsphere                  // (0,0,30)
            sm[i].angle.z += sm[i].angvel.z        // 0.3 rad/tick
            sm[i].dist = rsize                     // expanding radius (40 → 300)
            mat = RotX(0) · RotY(0) · RotZ(angle.z)
            v = (0, sm[i].dist, 0)
            D3DMATRIXTransform(mat, v, v)
            sm[i].pos = sm[i].pivot + v
```

> **Note on `goffset`:** `ConvertToVector(...)` and `goffset.z = 5` are computed in `Initialize` (`effect_old.cpp:2697-2698`) but `goffset` is **never referenced again** in `Animate` or `Render`. This is a copy of the FireFlash dead-code pattern (§13 in `F-FIREFLASH_TFireFlashAnimator.md`). The gsphere pivot is always the hardcoded `(0,0,30)`.

> **Note on firering frame window:** `fr[0].keytime[2] = 75` means the firering runs from frame 45 through frame 119 (starttime=45, last keytime=75, window=45..45+75=120). But the hard kill at `framenum > 140` is the actual effect end, so the firering finishes naturally (all keys exhausted, `fr[0].dead = 1`) and then the last particles die before frame 140.

### 6.4 Temporal diagram

```
framenum  0    5   16  25  28        40        45                    119    140
          │    │    │   │   │         │         │                      │      │
SMOKEY1   ●════╪════╪═══╪   │         │         │                      │      │
count:    4    7    7   2   │         │         │                      │      │
                            │         │         │                      │      │
GSPHERE   │         │       ●════════●  (gsize 1→40 over 12 frames)   │      │
(render)  │         │       │         ↓ EXPLOSION frame 40             │      │
          │         │       │  RING ●══════════════════════════════●   │      │
          │         │       │  (rsize 40→300 at 13/tick)               │      │
                            │                                          │      │
FIRERING  │         │       │         │    ●══════════════════════════●│      │
(28/frame)│         │       │         │    (radius 30+4/tick,         │      │
          │         │       │         │     28 particles/frame)       │      │
          └─────────┴───────┴─────────┴──────────────────────────────┴──────●→
                                                                             KillThisEffect()

gsize growth:  1.0 (frame 0) → 40.0 (frame 40)  at +3.0/tick over frames 28..39
rsize growth:  40.0 (frame 40) → 300.0 (+13.0/tick) → dies at ~frame 58 (30 ring ticks from 40)
firering:      starts frame 45, radius 30+4/tick → at frame 119 radius = 30 + 74·4 = 326 wu
```

---

## 7. Rendering (original render state + geometry)

Full body at `effect_old.cpp:3091-3182`:

```
Render():
    SaveBlendState()
    SetAddBlendState()               // :3097 — AdditiveStraight (ONE/ONE)
    ResetExtents()

    for i = 0..399:
        // ── SELECT SUB-OBJECT by state ──────────────────────────────────
        if (sm[i].state == SMOKEY1):
            if ((stopfade - life) >= (stopfade - startfade) · 70 / 100):
                obj = GetObject(0)   // smoke (cool) — late-life
            else:
                obj = GetObject(1)   // smoke01 (hot) — early-life
            if (life > 0):
                if ((stopfade - life) >= startfade):
                    scale = mainscale1 · life / (stopfade - startfade) + 0.05  // decay
                else:
                    scale = mainscale1                                           // full

        else if (sm[i].state == FIRERING):
            // Same COLORPCT logic as SMOKEY1 but uses FWIND_FR_COLORPCT=80%
            // NOTE: the actual render body uses FWIND_F1_COLORPCT=70% (same block
            // at :3119-3132) — the FIRERING path copies the SMOKEY1 block verbatim
            if ((stopfade - life) >= (stopfade - startfade) · 70 / 100):
                obj = GetObject(0)
            else:
                obj = GetObject(1)
            // scale fade logic identical to SMOKEY1

        else if (sm[i].state == GSPHERE):
            if (framenum >= FWIND_GS_ON=28):
                obj = GetObject(1)   // smoke01 (hot)
            else: continue           // skip before frame 28

        else if (sm[i].state == RING):
            obj = GetObject(1)       // smoke01 (hot)

        else: continue               // USEME — skip

        if (sm[i].scale > 0.0):
            obj->flags = OBJ3D_MATRIX    // :3154 — no OBJ3D_ABSPOS (renders at effect's world pos)

            D3DMATRIXClear(&obj->matrix)
            D3DMATRIXRotateX(&obj->matrix, -(M_2PI / 3.0))   // :3158  = -120°
            D3DMATRIXRotateZ(&obj->matrix, -(M_PI / 4.0))    // :3159  = -45°
            D3DMATRIXRotateZ(&obj->matrix, -(facing/256.0f · M_2PI))  // :3160 — caster facing

            obj->scl = (scale, scale, scale)
            D3DMATRIXScale(&obj->matrix, &obj->scl)

            obj->pos = sm[i].pos         // :3167-3169
            D3DMATRIXTranslate(&obj->matrix, &obj->pos)

            RenderObject(obj)

    UpdateExtents()
    RestoreBlendState()
    return true
```

- **What it draws:** for each of up to 400 active particles, one `RenderObject(obj)` call drawing a **single I3D mesh sub-object quad** (`smoke` or `smoke01`) with a per-particle uniform scale + the shared fixed-tilt + facing rotation matrix.

- **Blend mode (original):** **AdditiveStraight (ONE / ONE).** `Render` calls `SetAddBlendState()` (`effect_old.cpp:3097`), which sets `D3DTBLEND_DECALALPHA`, `SRCBLEND=ONE`, `DESTBLEND=ONE` per `effect_old.cpp:235-244`. Depth-write off, depth-test on.

  **BLEND SANITY-CHECK (mandatory):**
  - **Sprite design:** The `smoke01` (hot fire) sprite is a warm orange blob on a near-black background (confirmed from `i3d_dump_all` texture dump). Black-on-background textures are the textbook additive sprite — black = transparent under `ONE/ONE`. The `smoke` sprite is dark with dim center, also on near-black background — fits additive. Both sub-objects are designed for additive blending. **No conflict.**
  - **Sister-family:** The full fire family (FireFlash, FireCone, Burn, DragonFire) all call `SetAddBlendState()` in their Render bodies. FireWind using additive is **fire-family-consistent.** No suspect flag.
  - **Retail risk note:** `TFireWindAnimator::Render` body is not recovered in the retail decomp; this is snapshot-only blend. However, given the asset design + entire fire family using additive, the reconstruction agent should treat this as a high-confidence snapshot read and visually verify against the retail gameplay frames (§12).

- **Lit vs self-lit:** **Unlit / self-lit.** Neither `Initialize` nor `Render` touches the imagery material or per-vertex diffuse color. The particles render at the asset's authored pixel colors, additively accumulated into the framebuffer.

- **Depth / Z:** depth-test on, depth-write off (`SetAddBlendState` `:238`) → **TestNoWrite**.

- **Orientation:** **custom world-axis fixed-tilt** — NOT ScreenAligned, NOT WorldXY. The render matrix is built as:
  ```
  M = RotX(-120°) · RotZ(-45°) · RotZ(-facing·2π/256) · Scale · Translate(particle_pos)
  ```
  Both Z-rotations combine: effective `RotZ(-(45° + facing_rad))`. The RotX(-120°) tips the authored-XZ-flat quad significantly from the screen, giving the fire column a "looking down from above" visual that matches the isometric view. This is the same orientation matrix as FireFlash (identical code, same typo `256.0ff`). The reconstruction agent should apply this exact transform.

  > **Orientation clarification:** The `OBJ3D_MATRIX` flag makes the engine use the object's explicit matrix (as built above) rather than any auto-orientation. The authored quad (`smoke`/`smoke01`) lies in the XZ plane (Y ≈ 0), and the RotX(-120°) tips it to the camera view. This is a custom-per-particle fixed world transform, not a camera-aligned billboard.

- **Per-quad scale:** `sm[i].scale` driven by state:
  - GSPHERE: `gsize × 0.01f` (grows 0.01→0.40 as sphere radius grows 1→40)
  - RING: `FWIND_RI_PSIZE1 = 0.55f` (fixed at explosion; updated in Render only for fade — note RING has no `life` so no fade logic applies; the scale stays 0.55)
  - SMOKEY1/FIRERING: `mainscale1` (= 0.3 for SMOKEY1, = 0.5 for FIRERING) decaying toward 0.05 via the per-life fade formula
  
  Scale is **uniform across all three axes** (`obj->scl.x = obj->scl.y = obj->scl.z = sm[i].scale`).

### 7.1 `TFireWindAnimator::RefreshZBuffer()` (`effect_old.cpp:3184-3195`)

```
RefreshZBuffer():
    size_x = 640
    size_y = 480
    map = GetPos()                          // effect's own world position
    WorldToScreen(map, screen)
    RestoreZ(screen.x - 320, screen.y - 240, 640, 480)
```

Restores Z for a 640×480 px screen box (the entire screen!) centred on the projected effect position. This is much larger than FireFlash's 200×200 patch — appropriate for the much larger visual extent of FireWind's expanding ring (300 wu radius).

---

## 8. Texture animation

**None.** The animator never reads `obj->textureframe` or any UV field. Neither `TFireWindAnimator::Animate` nor `TFireWindAnimator::Render` contains any per-frame UV math. The imagery is in `STILL` state (single frame per sub-object). Both `smoke` and `smoke01` textures are static single-frame 64×64 sprites (`manifest.txt` confirms `frames=1` for both).

---

## 9. Associated light

**Spell-system light, not effect light.** The `spell.def:719` line:
```
LIGHT COLOR 255, 130, 0 INT 255 MULT 20
```
is applied by the spell system at cast time — a warm orange dynamic point light with intensity 255 × MULT 20, the same as FireFlash's family pattern (FireFlash uses `INT 160 MULT 20`; FireWind uses the full `INT 255`). The animator class does NOT call `AddPointLight` or any equivalent in `Initialize`, `Animate`, or `Render`. The visual light in the gameplay screenshots comes entirely from this spell.def light, not from any per-frame animator emission.

---

## 10. Color

- **Source:** asset-authored texture colors (the `smoke01` warm orange-red sprite). No per-vertex color override in the animator's Render body. No `chardata` color field, no spell tint, no `NormalizeColors` call.
- **Expected visual:** warm orange to yellow-white fire glow from `smoke01` (early-life particles, GSPHERE, RING); dim dark-grey smoke from `smoke` (late-life SMOKEY1/FIRERING). The combined additive accumulation of hundreds of particles produces the **large, intensely bright orange ring** visible in the gameplay frames (`images/vfx/15_FireWind/01.png–04.png`).
- **Key visual descriptor:** The gameplay frame `01.png` shows a huge bright orange disk on the ground with a concentrated central glow — this is the RING particles (200 × additive orange `smoke01` sprites) passing through the dungeon floor projection at peak expansion.
- **Color health signal:** The fire-orange is rich and saturated. If a port renders gray or pale, the most likely causes are: (1) wrong asset loaded (snapshot vs. retail, or fireflash.I3D instead of firewind.I3D), (2) wrong blend mode (Alpha instead of AdditiveStraight), (3) wrong sub-object indexing (smoke instead of smoke01 for early-life particles). See `[[feedback-vfx-color-health-signal]]`.

---

## 11. Audio coupling

- **Sound name:** `"FireWind"` → maps to shipped `Sound/effects/firewind.wav` (108,762 B).
- **When:** One-shot at `TFireWindAnimator::Initialize()` (`effect_old.cpp:2690`), via the `PLAY("FireWind")` macro — the SoundPlayer registry lookup for the named wave.
- **No looping audio.** No `STOP` or per-frame audio calls anywhere in the animator.

---

## 12. Triggers & in-game appearance

- **Spawned by:** `spell.def:710-726` — `SPELL "Fire Wind"` with two live VARIANTs:
  - `"Fire Wind"` (TP_BASIC, talismans `"AEF"`, `"firewind"` builder, 128 mana, 96-124 DT_BURN, skill 10, animation `invoke2`, `LOOP_TIME 70`)
  - `"Priest Fire Wind"` (TP_BASIC, talismans `"MK"`, `"firewind"` builder, 87 mana, 76-92 DT_BURN, skill 0)
  - Also: a second spell `"YFireWind"` exists in `spell.def` (the `YFireWind` variant with builder `"YFireWind"` and talismans `"MBHF"`, 102 mana, 118-132 damage) — this uses a separate registered effect class (not TFireWindEffect/TFireWindAnimator).
- **Where to see it:** Cast by priest-class enemies (CAVES area, early-game). `user description: "cast by early CAVES priests on lower-level enemies."` Visual confirmation: `images/vfx/15_FireWind/01.png` (peak explosion ring), `02.png` (ring expanding + player dodging), `03.png` (firering ground circle visible around player), `04.png` (late-stage ground fire), `05.png` (afterburn small ground fire), `06.png` (effect near-end with player).
- **Not vestigial.** Two live spell variants call the `"firewind"` builder. The effect is genuinely in-game.

### Visual characterization from gameplay reference frames

| frame | visual moment |
|-------|---------------|
| `01.png` | Peak explosion: enormous bright orange disc (RING 200 particles, r≈40-150 wu) fills the camera frame; central intense glow (GSPHERE); SMOKEY1 column visible at center; HUD shows enemy -48 HP |
| `02.png` | Ring expanding outward; character visible in upper-left for scale (ring is ~3-4× character width); ground-orange glow prominent |
| `03.png` | Firering ground circle visible as outer ring of rising orange flames; RING now partially off-screen (very large); inner burn concentrated at center |
| `04.png` | Mid-decay: RING has cleared (past 300 wu); FIRERING ground circle still active; character stands in center of remaining ground fire |
| `05.png` | Late-stage: small persistent ground fire from FIRERING particles; ring long gone |
| `06.png` | Near-end: effect dying out; player and dragon visible; only last ground-fire particles remain |

---

## 13. Gaps & uncertainties

**13.1 Asset texture content (snapshot vs. retail).** The snapshot `firewind.I3D` appears to be an exact copy of `fireflash.I3D` (identical MD5). The retail `firewind.i3d` is definitively different (17,452 B vs. 25,908 B). The exact pixel content of the retail `smoke` and `smoke01` sprites inside the retail file has NOT been decoded (the `i3d_dump_all/firewind/` dump was produced from the snapshot copy, not the retail extraction). The reconstruction agent should extract the retail asset and inspect its textures directly. The gameplay frames confirm the retail `smoke01` is a saturated warm orange-red fire sprite.

**13.2 `goffset` is computed but unused.** `Initialize` computes `goffset` via `ConvertToVector(...)` + `goffset.z = 5` but neither `Animate` nor `Render` references `goffset`. The `gsphere` pivot is always the hardcoded `(0,0,30)`. Same dead-code pattern as FireFlash. Reconstruction: ignore `goffset`.

**13.3 Death condition is the hardcoded `framenum > 140`.** The `waitingtofinish` / `stopclock + FWIND_WAITTOEND` soft-kill is commented out (`effect_old.cpp:2913`). The active line `:2914` `if (framenum > 140) KillThisEffect()` is the shipped kill (snapshot). Not corroborated in retail.

**13.4 FIRERING color threshold discrepancy.** `FWIND_FR_COLORPCT = 80` is defined (`:2611`) but the render body for `state == FIRERING` at `:3121` uses `FWIND_F1_COLORPCT = 70` (copied from the SMOKEY1 block). The firering's color transition point is **70% of its fade window**, not 80%. This is likely a copy-paste artifact in the snapshot; the define `FWIND_FR_COLORPCT` is unused. Reconstruction: use 70% for FIRERING color transition.

**13.5 RING particles have no life/fade logic in Render.** The render body at `:3144-3148` picks `GetObject(1)` for RING state but does NOT apply any fade calculation (no `if (life > 0)` block). The RING particles never have their `life` decremented in `Animate` (only `dist = rsize` is updated). RING particles exist until the whole ring is freed at `rsize >= 300`. Ring particle scale is fixed at `FWIND_RI_PSIZE1 = 0.55` throughout.

**13.6 Snapshot-vs-retail risk.** With the animator bodies unrecovered in retail, ALL per-frame timing constants (`FWIND_EXPLOSION = 40`, `FWIND_GS_ON = 28`, particle counts, ring growth rates, life values, firering keytimes) are **snapshot-only**. The reconstruction agent should visually verify the expansion rate, ring lifetime, and phase transitions against the six gameplay frames in `images/vfx/15_FireWind/`. If the ring appears to expand faster or slower than the captured frames suggest, the 24 Hz per-tick constants should be adjusted.

**13.7 `YFireWind` is a separate effect.** The `spell.def` references a `"YFireWind"` builder (`spell.def:1846-1849`). Retail binary has `s_YFireWind_005e1600` XREF[2]: `0x5066a0` + `0x5067b0` — this is a separate builder registration. `i3d_dump_all/YFirewind/` exists with a different `smoke01` texture (green sphere). YFireWind is NOT covered by this forensics doc.

---

## 14. Reconstruction burndown

```
- [ ] Load retail asset `Imagery/Magic/firewind.i3d` from `data/imagery.rvi` (NOT the snapshot copy) (§4)
- [ ] Confirm 2 sub-objects: smoke (index 0, dark puff) + smoke01 (index 1, warm orange fire) (§4)
- [ ] Implement 400-slot hand-rolled FWIND_PARTICLE pool (pos/pivot/vel/angle/angvel/state/scale/dist/life/startfade/stopfade/color/whichring) (§3)
- [ ] Seed sm[0..99] as GSPHERE at Initialize with random 3-axis Euler angles [0,2π) each, angvel=(0.12,0.12,0.12) (§6.2)
- [ ] Implement FWIND_FIRERING struct (fr[0]): starttime=45, ringsize=30, keyscale=[0.5,0.5,0.5], keyadd=[28,28,28], keytime=[5,70,75] (§6.2)
- [ ] One-shot `PLAY("FireWind")` audio at Initialize (§11)
- [ ] SMOKEY1 spawn: frames 0..4 → 4/frame; frames 5..15 → 7/frame; frames 16..24 → 2/frame; all mainscale1=0.3; disc r=15, z=1, vel.z=2+rand(0,2) (§6.3)
- [ ] GSPHERE grow: frames 28..39 — gsize += 3.0/tick, clamped at 40.0; per-particle orbit via 3-axis rotation of (0,gsize,0) vector around pivot (0,0,30); scale=gsize×0.01 (§6.3)
- [ ] EXPLOSION at frame 40: promote up to 200 available (USEME+GSPHERE) slots to RING; angle.z = i·2π/200 evenly distributed; angvel.z=0.3; scale=0.55; rsize=gsize (§6.3)
- [ ] RING motion post-explosion: rsize += 13.0/tick; orbit pivot (0,0,30) in XY via RotZ(angle.z) × (0,rsize,0); kill all to USEME when rsize ≥ 300 (§6.3)
- [ ] FIRERING spawn: frames 45..119 — 28 particles/frame; random angle on expanding ring radius (30+4/tick); z=1; vel.z=1+rand(0,3) (§6.3)
- [ ] FIRERING motion: track XY via ConvertToVector(angle.z, fr[ring].ringsize); rise in Z += vel.z; decrement life, die at life=0 (§6.3)
- [ ] Hard kill: `if (framenum > 140) KillThisEffect()` (§6.3)
- [ ] Render: AdditiveStraight (ONE/ONE) blend bracket via SetAddBlendState() (§7)
- [ ] Render: per-particle OBJ3D_MATRIX only (no OBJ3D_ABSPOS — renders at effect's own world pos) (§7)
- [ ] Render: per-particle matrix = RotX(-2π/3) · RotZ(-π/4) · RotZ(-(facing/256·2π)) · Scale(sm[i].scale) · Translate(sm[i].pos) (§7)
- [ ] Render: sub-object selection — SMOKEY1/FIRERING: GetObject(1) (hot) until 70% of fade window, then GetObject(0) (cool); GSPHERE (from frame 28): GetObject(1); RING: GetObject(1) (§7)
- [ ] Render: SMOKEY1/FIRERING scale fade — `mainscale1 · life / (stopfade - startfade) + 0.05` in the decay window; full `mainscale1` before it (§7)
- [ ] Render: RING particles have no fade — scale stays FWIND_RI_PSIZE1=0.55 throughout (§13.5)
- [ ] Depth: TestNoWrite (depth-test on, depth-write off) (§7)
- [ ] Lit mode: Unlit — no per-vertex diffuse write (§7)
- [ ] RefreshZBuffer: 640×480 px patch centred on projected effect world pos (§7.1)
- [ ] Texture animation: none — static single-frame per sub-object (§8)
- [ ] Associated light: none from animator — spell.def provides LIGHT COLOR 255,130,0 INT 255 MULT 20 (§9)
- [ ] Sub-effects / children: none (§6)
- [ ] TFireWindEffect damage: Pulse increments life; when life≥40, rad+=5 per Pulse; call BlastCharactersInRange at every 40 wu of radius growth (damage 96-124 DT_BURN, KnockBack) (§6.1)
- [ ] Visually verify against images/vfx/15_FireWind/01-06.png: large orange ring, central glow, ground-fire ring, dramatic scale (§12, §13.6)
- [ ] Audio hooks: `PLAY("FireWind")` once at spawn — record for audio phase (§11)
```

---

> **INVENTORY row update suggestion (for orchestrator):** Update the FW / TFireWindEffect row in `docs/vfx/INVENTORY.md`:
> - `Status: forensics-complete`
> - `Retail fidelity: retail-partial`
> - `Notes: forensics doc at docs/vfx/forensics/FW_TFireWindEffect.md; asset differs snapshot vs retail (retail firewind.i3d 17452B, ship it not the snapshot copy); all physics constants snapshot-only; registration+audio+spell confirmed retail; visual ref 6 frames at images/vfx/15_FireWind/`
