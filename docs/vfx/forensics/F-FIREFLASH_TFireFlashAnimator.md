# F-FIREFLASH TFireFlashEffect / TFireFlashAnimator — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | F-FIREFLASH (covers `TFireFlashEffect` + `TFireFlashAnimator`) |
| **Class(es)** | `TFireFlashEffect : TEffect` (near-empty — only `Initialize` damages the invoker's `Fighting()` target once; `Pulse` chains to base) + `TFireFlashAnimator : T3DAnimator` (owns one 150-slot particle pool that re-uses entries across three phases: rising flame column, growing glowing sphere, expanding explosion ring) |
| **Status** | forensics-complete |
| **Retail fidelity** | **retail-partial.** Registration confirmed: `"FireFlash"` string at `.rdata 005e1080` has two XREFs (`0x4e1750` and `0x4e18a0`, `recon/classes/_data.txt:107360-107361`) — the standard `DEFINE_BUILDER` + `REGISTER_3DANIMATOR` table-of-builders pair (entries at `005c530c` and `005c5310`, `recon/classes/_data.txt:55924-55927`). The lifetime cap `FFLASH_NOMOREOFTHISEFFECT = 100` is **retail-confirmed**: the merged class `cls_0x5a9194::virt_meth_0x4e17f0` (`recon/classes/cls_0x5a9194.cpp:76-114`) shows `dVar4 = this->mbr_0x184 + 1; this->mbr_0x184 = dVar4; if (99 < (int)dVar4) { … kill chain … }` — exact match for the snapshot's `framenum++; if (framenum >= 100) KillThisEffect()` pattern. The particle pool size + kill cadence + class layout (mbr_0x184 at offset 388 = `framenum`, ~13.1 KB tail for `sm[150]`-sized particle array) is layout-consistent. The audio key `"FireFlash"` for `PLAY()` is **retail-confirmed** by the shipped sound asset `data/resources_unzipped/Sound/effects/fireflash.wav` (192,972 B, 1998-11-23). The asset `Magic\fireflash.I3D` ships in `data/imagery.rvi`. **What is snapshot-only:** the per-frame phase choreography constants (FFLASH_F1_*, FFLASH_GS_*, FFLASH_RI_*), the specific render transform (`RotX(-2π/3) · RotZ(-π/4) · RotZ(-facing·2π/256) · Scale · Translate`), and the blend (`SetAddBlendState` / Additive); the recon body for `Render`/the merged class's tuning constants is not isolated. See §2.1. |
| **Author / Date** | vfx-forensics-agent (redo-F-FIREFLASH) / 2026-05-29 |
| **Family** | fire (spell-cast offensive — sister to FireWind, FireCone, FireBall in the fire spell family) |
| **Draws** | composite — many small additive **I3D-mesh** particles drawn from a 2-sub-object asset (`smoke` + `smoke01`) chosen per-particle by phase + color stage. Each particle is a single tilted/scaled `RenderObject` call. Up to ~150 simultaneous particles at peak (explosion ring uses all of them). |
| **Archetype(s)** | composite of: **(D) rising particles** (SMOKEY1 column — vel.z=+2..4 only, no gravity, lifetime 20..45 ticks with size-fade), **(E) spinning-orbit particles** (GSPHERE — 75 particles on rotating axes around a fixed pivot, dist=`gsize` growing 1→20 over frames 26..34), **(E) expanding-ring particles** (RING — 150 particles in rotating-orbit, dist=`rsize` growing from `gsize` to 300 in steps of 8/tick after the explosion frame), **(F) custom-procedural-state-machine** (3 fixed phase windows on a single per-render-tick `framenum` counter), **(M) audio** (one-shot `PLAY("FireFlash")` at Initialize), **(F) target-locked render anchor** (when the casting spell has a `Fighting()` target, particles render at *target world position*, not the effect's own position — see §5). No emitted light (the spell's casting light comes from `spell.def`'s LIGHT line, not the effect). |

---

## 1. Summary

`"FireFlash"` is the Level-1 Fire-school spell that erupts a brief fiery
explosion on the caster's combat target. Visually it is a tight composite of
three time-windowed particle phases sharing one 150-slot pool. Across the
first **25 render frames** the animator spawns a thin **rising flame column**
of 4–7 SMOKEY1 particles/frame (each rising at 2..4 wu/tick from `z = 1`,
random XY within a 15-wu base radius, living 20..45 ticks with a size-decay
fade). At **frame 26** a **glowing sphere** ignites: 75 GSPHERE particles
that were dormant since Initialize start orbiting a fixed pivot at
`(0, 0, 30)` on three independent axes (`angvel = 0.12 rad/tick` on each axis)
while their orbit radius `gsize` ramps **1 → 20 wu** at +2/tick across frames
26..34 (each particle's per-particle scale = `gsize × 0.01`, so they grow
proportionally with the sphere). At **frame 35** (`FFLASH_EXPLOSION`) the
sphere snaps into an **expanding ring**: all 150 particles flip to RING
state, orbit the same pivot, all at `angvel.z = 0.3 rad/tick` (no X/Y axial
spin authored, but the per-particle `angle` ramp gives flat-disc orbit), and
their radius `rsize` (starting from `gsize`, ~20) grows by +8/tick until
hitting **300 wu** at which point all particles release (and the effect's own
framenum runs out at 100 → `KillThisEffect()`). Rendering is **additive
(`SetAddBlendState`)** with a fixed-tilt orientation matrix
(`RotX(-120°) · RotZ(-45°) · RotZ(-facing)`) applied per particle so the
column tips slightly toward the screen and the explosion ring presents as a
near-horizontal disc oriented to the caster's facing. The audio cue
`"FireFlash"` plays once at spawn (the shipped `fireflash.wav`). The whole
effect runs ~100 frames (≈4.17 s at 24 Hz) and ends with `KillThisEffect()`.

The casting *spell* adds its own `LIGHT COLOR 255, 130, 0 INT 160 MULT 20`
(`spell.def:74`) which is applied by the spell system around the strike, not
by the animator — see §9.

---

## 2. Sources & evidence

- **Retail decomp:**
  - `recon/classes/cls_0x5a9194.cpp` — **SPARSE / MERGED**. Ghidra has
    merged TFireFlashAnimator (+ TFireWindAnimator + TBurnAnimator etc.) into
    one 13,504-byte class file. Only 4 bodies extracted; the relevant one is
    `virt_meth_0x4e17f0` (the framenum++ / `if (99 < x) {…}` body — this is
    `TFireFlashAnimator::Animate`'s kill-condition tail).
  - `recon/classes/cls_0x5a9194__vftable_5a9194.cpp` — 24-entry vftable.
    Relevant slot candidates: `virt_meth_0x4e18c0` (slot 24, XREF's
    `s_FireFlash_005e0efc` at `0x4e18d8`); `virt_meth_0x4e1ad0` (slot 44);
    `virt_meth_0x4e1fa0` (slot 52); `virt_meth_0x4e2200` (slot 60). Bodies
    not isolated by recon.
  - `recon/mappings/TFireFlashEffect_cls_0x5a9194_candidate.yaml` — flags the
    merge problem (`LOW-MEDIUM` confidence, treat as composite).
  - **Registration table** (`recon/classes/_data.txt:55920-55930`):
    ```
    005c530c   addr   LAB_004e1750     ← DEFINE_BUILDER("FireFlash", TFireFlashEffect)   (XREF[2] s_FireFlash_005e1080)
    005c5310   addr   LAB_004e18a0     ← REGISTER_3DANIMATOR("FireFlash", TFireFlashAnimator)
    005c5314   addr   LAB_004e2250     ← FireWind builder
    005c5318   addr   LAB_004e23e0     ← FireWind animator
    005c531c   addr   LAB_004e2f80     ← BURN builder
    005c5320   addr   LAB_004e3160     ← BURN animator
    ```
    Pattern is the same `DEFINE_BUILDER`+`REGISTER_3DANIMATOR` pair confirmed
    for FLAME (`005c5348→0x4e4ea0`) and sparks (`005c5350→0x4e53a0`).
  - **Confirmed-by-decomp Animate tail**
    (`recon/classes/cls_0x5a9194.cpp:76-114`, paraphrased):
    ```
    virt_meth_0x4e17f0(cls_0x5a9194 *this):
      cls_0x5b8e94_TObjectInstance::virt_meth_0x4de800((…)this);   // T3DAnimator::Animate(draw) base
      dVar4 = this->mbr_0x184 + 1;                                  // framenum++
      this->mbr_0x184 = dVar4;
      if (99 < (int)dVar4) {                                        // if (framenum >= 100)
        // (chain of pointer dereferences + a virt-call) -> KillThisEffect / cleanup
        …
      }
    ```
    matches snapshot `effect_old.cpp:2169 (framenum++) / 2386-2387 (if (framenum
    >= FFLASH_NOMOREOFTHISEFFECT==100) KillThisEffect())` exactly. **The
    lifetime cap of 100 frames is retail-confirmed.**
- **Pre-release (snapshot — authoritative for the body math):**
  - `TFireFlashEffect`: class decl `src/effect_old.cpp:1976-1986`;
    `DEFINE_BUILDER("FireFlash", TFireFlashEffect)` `:1988`;
    `REGISTER_BUILDER(TFireFlashEffect)` `:1989`; `Initialize`
    `:1991-2004`; `Pulse` `:2006-2009`.
  - `TFireFlashAnimator`: constants block `src/effect_old.cpp:2015-2066`;
    `FFLASH_PARTICLE` struct `:2070-2084`; class decl `:2086-2109`;
    `REGISTER_3DANIMATOR("FireFlash", TFireFlashAnimator)` `:2111`;
    `Initialize` `:2113-2162`; `Animate(bool draw)` `:2164-2390`;
    `Render()` `:2392-2479`; `RefreshZBuffer()` `:2481-2494`.
  - `effect_old.cpp` is wrapped in `#if 0 … #endif` (`:10`, end-of-file)
    — entire file is preserved-but-unbuilt pre-release source. One bug
    inherited (typo `256.0ff` at `:2457`) confirms the snapshot is the
    raw pre-release source, NOT a polished rebuild.
- **Sister effects consulted:**
  - `TFireConeAnimator` (`src/effect_old.cpp:7211-7320`) — sister
    fire-spell using the same `SetAddBlendState` family pattern. Uses
    `TParticleSystem` rather than a hand-rolled pool, but the Additive
    blend + non-character-attached origin is the same family signature.
  - `TFireWindAnimator` / `TBurnAnimator` (animator bodies merged into the
    same `cls_0x5a9194` retail file) — same registration table, same
    composite-class layout in retail. FireWind builder is the adjacent
    table entry at `005c5314`/`005c5318`.
  - `TFireBallAnimator` / `TMissileEffect` family — DIFFERENT family
    (missile travel + impact). FireFlash is a stationary cast-on-target
    detonation, not a projectile.
  - `TBurnAnimator` (M04 documented in
    `docs/vfx/forensics/M04_TBurnEffect.md`) — confirms cls_0x5a9194 is a
    Ghidra mega-merge of fire-school animators; cited in §11/§2.1 for
    cross-validation of the audio + class-merge story.
- **Asset:** `Magic\fireflash.I3D` — snapshot
  `legacy/Imagery/Magic/fireflash.I3D` 25,908 B MD5
  `6d5eeb379bf1b32407ebf9e9d0ab0c86`. Registered in legacy
  `Class.Def:2048` (`"FireFlash" "Magic\fireflash.I3D" 0x37780ae2`) and
  again at `Class.Def:2053` (same path, same hash — duplicate entry).
  The asset's 2 sub-objects (`smoke`, `smoke01`) are confirmed via raw
  `strings` decode of the file.
- **Spell wiring (live in shipped):** `data/Resources/spell.def:66-98`
  + `data/resources_unzipped/spell.def:66-98` — `SPELL "Fire Flash"` has
  two VARIANTs: `"Fire Flash"` (Level-1 LI=Light/Inhabit talisman, 31 mana,
  damage 50..55) and `"Priest Fire Flash"` (Level-1 MA=Mind/Air, 186 mana,
  damage 56..68, radius 20). Both reference `"fireflash"` builder.
- **Audio asset:**
  `data/resources_unzipped/Sound/effects/fireflash.wav` (192,972 B,
  1998-11-23) — shipped.
- **Source-of-truth ranking:** snapshot is **authoritative for body math**
  (all phase choreography, particle motion, render transform). Retail
  decomp is **authoritative for the lifetime cap (100 frames) and class
  registration** but provides no readable Animate-body / Render-body for the
  rest of the per-frame logic; constants are taken from snapshot.

### 2.1 Retail-vs-snapshot reconciliation (verdict: retail-partial)

`src/effect_old.cpp` is the pre-release development snapshot, gated under
`#if 0` and containing at least one compile-bug (`256.0ff` at line 2457).
Active cross-checks against retail:

**(1) Constant grep — partial corroboration; one direct hit.**
- **`FFLASH_NOMOREOFTHISEFFECT = 100`** — **retail-confirmed**.
  `recon/classes/cls_0x5a9194.cpp:89` reads `if (99 < (int)dVar4)` (i.e.
  `dVar4 >= 100`) after incrementing `mbr_0x184`. This is the framenum kill
  test verbatim. The kill chain that follows includes a virtual-call sequence
  that matches the shape of `KillThisEffect()`/object teardown.
- **`FFLASH_NUMPARTICLES = 150`** — **inferred-corroborated**: the class
  layout has `mbr_0x184` at offset 388 (= framenum) and `mbr_0x34bc` at
  offset 13500, total class size 13504 B. The tail
  `13504 - 388 - 4 (framenum) = 13112 B`, which divided by 88 B per
  `FFLASH_PARTICLE` (5 × 12 B vec3 + 7 × 4 B scalar = 88 B) is ~149.0
  particles — within rounding of 150. Layout strongly consistent with the
  snapshot's `sm[FFLASH_NUMPARTICLES=150]`.
- Other tuning constants (FFLASH_GS_*, FFLASH_F1_*, FFLASH_RI_*) — **NOT
  searched for in isolated retail bodies because no readable Animate body
  beyond the kill-tail is decompiled.** The unique immediates
  (`0.12f`/0x3DF5C28F, `0.3f`/0x3E99999A, `0.01f`/0x3C23D70A, `300.0f`/0x43960000)
  hit too many unrelated places in `_data.txt` to be unique fingerprints.
  These remain **snapshot-only**.

**(2) Asset identity — registered and shipped.**
- Snapshot `legacy/Imagery/Magic/fireflash.I3D` 25,908 B MD5
  `6d5eeb379bf1b32407ebf9e9d0ab0c86`.
- Retail data file `data/imagery.rvi` contains `Imagery/Magic/fireflash.i3d`
  (registered via the shipped class.def, hash 0x37780ae2 — same as legacy
  Class.Def:2048). Byte-for-byte identity not verified in this pass (the
  archive's internal copy would need extraction), but **the same name + same
  Class.Def hash + same registration in both legacy and shipped class.def
  is strong corroboration**.
- The I3D's 2 sub-objects (`smoke`, `smoke01`) are confirmed via
  `strings` of the file (only two clean sub-object names appear). This
  matches the snapshot's `GetObject(0)` / `GetObject(1)` call sites
  exactly.

**(3) Structure/layout — corroborated.** cls_0x5a9194 is **merged**
(FireFlash + FireWind + Burn animators all wedge into the same Ghidra-
inferred class file because they share base class + vftable shape and
Ghidra's OOAnalyzer can't disambiguate). But:
- The 13,504 B size matches `T3DAnimator-base (small) + framenum
  (mbr_0x184) + sm[~150] particles (~13,100 B tail)`.
- mbr_0x184 increment pattern in `virt_meth_0x4e17f0` matches `framenum++`.
- Vftable size (24 entries) consistent with `T3DAnimator + ~4
  overrides (Initialize/Animate/Render/RefreshZBuffer)`.
- Dtor calls `cls_0x5b0074::~cls_0x5b0074(this)` — base class teardown
  (likely T3DAnimator or one of its ancestors).

**(4) Registration + naming — confirmed.**
- `s_FireFlash_005e1080` (the spell/builder name) has the two-XREF pattern
  (`0x4e1750`, `0x4e18a0`) at adjacent slots in the global animator-builder
  table (`005c530c`/`005c5310`) — same `DEFINE_BUILDER`+`REGISTER_3DANIMATOR`
  pair pattern confirmed in F01 (FLAME at `005c5348→0x4e4ea0`) and
  SPARKS (at `005c5350→0x4e53a0`).
- `s_FireFlash_005e0efc` (a separate copy of the string) is XREF'd by
  `virt_meth_0x4e18c0` at `0x4e18d8` — slot 24 (offset 0x18) in the
  vftable. This is likely the registration's name-getter override.
- `s_fireflash_005e0938` (lowercase) — XREF'd only by an unrelated method
  (`virt_meth_0x4dc180:0x4dc454`); this is the lowercase variant matching
  the `spell.def`'s `VARIANT "Fire Flash", … "fireflash"` (the
  Pulse/spell-system name, not the builder).
- Spell wiring is **shipped + reachable**: `data/Resources/spell.def:79,89`
  invokes the `"fireflash"` builder for two live spell variants ("Fire Flash"
  + "Priest Fire Flash"). Unlike F09 (FireColumn) and F11 (FlameDisc) which
  are vestigial in retail, **FireFlash is a live, callable spell in the
  shipped game.**

**(5) Audio — confirmed.** `PLAY("FireFlash")` (snapshot
`effect_old.cpp:2119`) maps to the shipped
`data/resources_unzipped/Sound/effects/fireflash.wav` (192,972 B,
1998-11-23). The recon class also has audio-pointer calls in its kill-tail
(line 100 of cls_0x5a9194.cpp dispatches through a function pointer table
on mbr_0xd8) — consistent with the snapshot's `PLAY` macro routing through
the same SoundPlayer subsystem.

**Verdict — retail-partial.**
*Existence, registration, spell wiring, lifetime cap (100 frames), audio
key, asset identity, sub-object count* are all **retail-confirmed**. *The
phase choreography (FFLASH_F1_/GS_/RI_ constants), the specific render
transform Euler triple, the additive blend choice, and the per-particle
state-machine details* are **snapshot-only** and unverified against
shipped retail bodies. **The render blend (Additive via
`SetAddBlendState`) IS the snapshot's choice; the BLEND SANITY-CHECK in §7
confirms it matches the fire-spell sister family** (FireCone, FireWind use
the same additive helper) and the bright-sprite-on-key-bg design — so
unlike F01/F03 there's no Alpha-vs-Additive ambiguity here; the snapshot
is the correct choice and sister-family-consistent.

---

## 3. Constants

Every numeric the effect uses, with citation. The `confirmed?` column
reflects §2.1: only the lifetime cap and registration/asset are
retail-corroborated; the choreography constants are snapshot-only.

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| builder name | `"FireFlash"` (matches `s_FireFlash_005e1080`) | string | `effect_old.cpp:1988`, retail `005c530c→0x4e1750` | **yes (retail)** |
| animator name | `"FireFlash"` (case-insensitive registry) | string | `effect_old.cpp:2111`, retail `005c5310→0x4e18a0` | **yes (retail)** |
| asset path | `Magic\fireflash.I3D` (id `0x37780ae2`) | path + content hash | `Class.Def:2048` | **yes (retail)** — same path + same id in shipped class.def |
| asset sub-objects | 2: `smoke` (= `GetObject(0)`), `smoke01` (= `GetObject(1)`) | count + names | I3D `strings` decode + `effect_old.cpp:2408,2410,2425,2433` | **yes (retail)** — file shipped + sub-object name strings present |
| **FFLASH_NOMOREOFTHISEFFECT** | **100** (framenum cap → `KillThisEffect()`) | render frames | `effect_old.cpp:2016`, used `:2386` | **yes (retail)** — `recon/classes/cls_0x5a9194.cpp:89` `if (99 < (int)dVar4)` |
| **FFLASH_NUMPARTICLES** | **150** | particle-pool size | `effect_old.cpp:2017`, used `:2097,2148,…` | **yes (retail)** — layout match: class tail 13,112 B / 88 B-per-particle ≈ 150 |
| FFLASH_STATE_USEME | 0 | enum | `effect_old.cpp:2020` | snapshot-only |
| FFLASH_STATE_SMOKEY1 | 1 | enum | `effect_old.cpp:2021` | snapshot-only |
| FFLASH_STATE_GSPHERE | 2 | enum | `effect_old.cpp:2022` | snapshot-only |
| FFLASH_STATE_RING | 3 | enum | `effect_old.cpp:2023` | snapshot-only |
| FFLASH_F1_ON  | 0 | frame (column on) | `effect_old.cpp:2026` | snapshot-only |
| FFLASH_F1_OFF | 25 | frame (column off — no new SMOKEY1) | `effect_old.cpp:2027` | snapshot-only |
| FFLASH_F1_MAXDIST | 15 | wu (base XY radius for column spawn) | `effect_old.cpp:2028`, used `:2275-2277,2298` | snapshot-only |
| FFLASH_F1_FADEMIN | 20 | frames (minimum particle life) | `effect_old.cpp:2029`, used `:2281,2298` | snapshot-only |
| FFLASH_F1_MAXFADE | 25 | frames (max life add — `life = 20 + 25·(MAXDIST−rand(0,dist))/MAXDIST`) | `effect_old.cpp:2030`, used `:2281` | snapshot-only |
| FFLASH_F1_FADEPCT | 20 | percent (life fraction before size-decay starts) | `effect_old.cpp:2031`, used `:2282` | snapshot-only |
| FFLASH_F1_BOTTOM | 1 | wu (z of new SMOKEY1 — just above ground) | `effect_old.cpp:2032`, used `:2280` | snapshot-only |
| FFLASH_F1_COLORPCT | 70 | percent (selects GetObject(0) vs (1) during render) | `effect_old.cpp:2033`, used `:2407` | snapshot-only |
| FFLASH_F1_MINSIZE | 0.05 | scale (minimum size during fade) | `effect_old.cpp:2034`, used `:2415` | snapshot-only |
| FFLASH_F1_TIME1 | 5 | frame (Time slice 1 cutoff) | `effect_old.cpp:2037`, used `:2243` | snapshot-only |
| FFLASH_F1_ADD1 | 4 | particles/frame (during slice 1: frames 0..4) | `effect_old.cpp:2038`, used `:2245` | snapshot-only |
| FFLASH_F1_MAINSCALE1 | 0.3 | scale (initial particle scale in slice 1) | `effect_old.cpp:2039`, used `:2246` | snapshot-only |
| FFLASH_F1_TIME2 | 15 | frame (cutoff) | `effect_old.cpp:2040`, used `:2248` | snapshot-only |
| FFLASH_F1_ADD2 | 7 | particles/frame (slice 2: 5..14) | `effect_old.cpp:2041`, used `:2250` | snapshot-only |
| FFLASH_F1_MAINSCALE2 | 0.3 | scale | `effect_old.cpp:2042`, used `:2251` | snapshot-only |
| FFLASH_F1_TIME3 | 16 | frame (cutoff) | `effect_old.cpp:2043`, used `:2253` | snapshot-only |
| FFLASH_F1_ADD3 | 7 | particles/frame (slice 3: frame 15 only) | `effect_old.cpp:2044`, used `:2255` | snapshot-only |
| FFLASH_F1_MAINSCALE3 | 0.3 | scale | `effect_old.cpp:2045`, used `:2256` | snapshot-only |
| FFLASH_F1_TIME4 | 25 (= FFLASH_F1_OFF) | frame (cutoff) | `effect_old.cpp:2046`, used `:2258` | snapshot-only |
| FFLASH_F1_ADD4 | 2 | particles/frame (slice 4: frames 16..24) | `effect_old.cpp:2047`, used `:2260` | snapshot-only |
| FFLASH_F1_MAINSCALE4 | 0.3 | scale | `effect_old.cpp:2048`, used `:2261` | snapshot-only |
| **FFLASH_EXPLOSION** | **35** | frame (snap GSPHERE→RING; freeze gsize, init ring) | `effect_old.cpp:2050`, used `:2174,2181,2225` | snapshot-only |
| FFLASH_GS_ON | 26 | frame (start growing gsize; start rendering sphere) | `effect_old.cpp:2052`, used `:2174,2422` | snapshot-only |
| FFLASH_GS_NUMPARTICLES | 75 | count (sphere particles, indices 0..74) | `effect_old.cpp:2053`, used `:2151` | snapshot-only |
| FFLASH_GS_MINSIZE1 | 1.0 | wu (initial gsize) | `effect_old.cpp:2054`, used `:2133` | snapshot-only |
| FFLASH_GS_MAXSIZE1 | 20.0 | wu (max gsize before clamp / explosion) | `effect_old.cpp:2055`, used `:2176,2179` | snapshot-only |
| FFLASH_GS_SIZEVEL1 | 2.0 | wu/tick (gsize growth during 26..34) | `effect_old.cpp:2056`, used `:2177` | snapshot-only |
| FFLASH_GS_SIZETOSCALE | 0.01 | scale-per-wu (per-particle scale = gsize × 0.01) | `effect_old.cpp:2057`, used `:2330` | snapshot-only |
| FFLASH_GS_MINANGVEL | 0.12 | rad/tick (initial angular velocity on each of X,Y,Z) | `effect_old.cpp:2058`, used `:2156-2158` | snapshot-only |
| FFLASH_GS_INFRONT | 10 | wu (computed but UNUSED — see §13) | `effect_old.cpp:2059`, used `:2127` | snapshot-only |
| FFLASH_GS_INFRONTUP | 5 | wu (computed but UNUSED — `goffset.z`, never read) | `effect_old.cpp:2060`, used `:2128` | snapshot-only |
| gsphere center (hardcoded) | `(0, 0, 30)` | wu (local to render origin — see §5) | `effect_old.cpp:2130-2132` | snapshot-only |
| FFLASH_RI_NUMPARTICLES | 150 (= NUMPARTICLES) | count (ring uses entire pool at explosion) | `effect_old.cpp:2062`, used `:2223` | snapshot-only |
| FFLASH_RI_MAXSIZE | 300.0 | wu (ring kill radius) | `effect_old.cpp:2063`, used `:2229` | snapshot-only |
| FFLASH_RI_SIZEVEL1 | 8.0 | wu/tick (ring radius growth after frame 35) | `effect_old.cpp:2064`, used `:2227` | snapshot-only |
| FFLASH_RI_MINANGVEL | 0.3 | rad/tick (initial angvel.z for ring particles) | `effect_old.cpp:2065`, used (in dead commented-out spawn block at `:2197`) | snapshot-only |
| FFLASH_RI_PSIZE1 | 0.1 | scale (initial ring particle scale — dead block `:2198`) | `effect_old.cpp:2066` | snapshot-only |
| **render matrix RotX** | `-M_2PI / 3.0` = -120° = -2π/3 rad | rad | `effect_old.cpp:2455` | snapshot-only |
| **render matrix RotZ #1** | `-M_PI / 4.0` = -45° = -π/4 rad | rad | `effect_old.cpp:2456` | snapshot-only |
| **render matrix RotZ #2** | `-(facing / 256 · 2π)` (= -facing-byte-angle in radians) | rad | `effect_old.cpp:2457` (note typo `256.0ff` → intent `256.0f`) | snapshot-only |
| per-particle scale | `sm[i].scale` (set per-phase below) | × | `effect_old.cpp:2459-2462` | snapshot-only |
| obj flags | `OBJ3D_MATRIX (0x100)` (+ optional `OBJ3D_ABSPOS` if target present, `0x?`) | bitmask | `effect_old.cpp:2442,2445` | snapshot-only |
| vel.z (SMOKEY1) | `2.0 + random(0,2)` ⇒ ∈ {2, 3, 4} wu/tick | wu/tick | `effect_old.cpp:2284,2301` | snapshot-only |
| vel.x/y (SMOKEY1) | 0 (no XY drift — particles rise straight up) | wu/tick | `effect_old.cpp:2284-2285` (only `vel.z` set; vel was zeroed by `memset` at `:2148`) | snapshot-only |
| gravity | 0 (no gravity; vel.z does not decay) | wu/tick² | (absent in code — confirmed by absence) | snapshot-only |
| sphere initial angle | `random(0,359) / 360 · 2π` per axis (X, Y, Z), per particle | rad | `effect_old.cpp:2153-2155` | snapshot-only |
| RefreshZBuffer patch | `size = 200×200`, projected origin `(0,0,0)` | px | `effect_old.cpp:2485-2492` | snapshot-only |
| audio cue | `"FireFlash"` ⇒ `fireflash.wav` 192,972 B | name | `effect_old.cpp:2119` | **yes (retail)** — `Sound/effects/fireflash.wav` shipped |

> **`Animate(bool draw)` runs per render frame.** Per snapshot
> framework: every `Animate` call advances `framenum` by 1 — the snapshot
> does **NOT** sim-tick-gate the body. Per the
> [feedback-framerate-independent-anim] memory, the reconstruction must
> convert per-tick deltas (vel.z=+2..4, gsize+=2, rsize+=8, angvel=0.12/0.3
> rad) to per-second rates anchored at 24 Hz nominal so the effect plays
> identically at any frame rate. At nominal 24 Hz, the full effect runs
> 100/24 ≈ 4.17 seconds.

---

## 4. Assets

| asset | path | size (B) | MD5 | role | how loaded (original) |
|-------|------|----------|-----|------|-----------------------|
| FireFlash mesh | `legacy/Imagery/Magic/fireflash.I3D` (snapshot; retail copy is in `data/imagery.rvi` as `Imagery/Magic/fireflash.i3d`, same Class.Def hash 0x37780ae2) | 25,908 | `6d5eeb379bf1b32407ebf9e9d0ab0c86` | composite mesh holding 2 sub-objects: `smoke` (GetObject(0)) + `smoke01` (GetObject(1)) | registered `Class.Def:2048` under name `"FireFlash"` (id `0x37780ae2`); standard imagery load via the `T3DAnimator::Get3DImagery()` path; sub-objects bound per-particle by `GetObject(0)`/`GetObject(1)` |
| FireFlash sound | `data/resources_unzipped/Sound/effects/fireflash.wav` | 192,972 | (not computed) | one-shot detonation sfx played at Initialize | `PLAY("FireFlash")` (`effect_old.cpp:2119`) — looks up the named wave in the SoundPlayer registry |

**Sub-objects (2)** — confirmed via raw `strings` decode of the I3D:
- **`GetObject(0)` = `smoke`** — used for SMOKEY1 particles in the
  **second half of their life** (after `FADEPCT × life` ticks, i.e. the
  "smoke" stage of the rising flame particle) — `effect_old.cpp:2407-2408`.
  Likely the cooler / smokier of the two textures.
- **`GetObject(1)` = `smoke01`** — used for SMOKEY1 particles in **early
  life** (the hot-flame stage), AND for GSPHERE particles AND for RING
  particles — `effect_old.cpp:2410, 2425, 2433`. This is the "hot/bright"
  sprite used for both the flame and the explosion sphere/ring.

So the asset choice per draw is:
```
                  flame-life stage 0..70%   |  70%..100%
SMOKEY1   →   smoke01 (hot)               |  smoke (cool)
GSPHERE   →   smoke01 (hot)                                  (only frames 26..)
RING      →   smoke01 (hot)                                  (only frames 35..)
```

**Texture decode.** The I3D embeds **one texture slot** (`numtex = 1`,
file offset 0x14) with the surface descriptor at offset 0x48
(`0x2001` flags, dims 100, 128, 160, cell 64, 120). The texture appears
to be a multi-frame surface (`numframes = 8` at offset 0x6c, header
points to data at offset 0x1bd4). **However** the snapshot's
`TFireFlashAnimator::Render` body **does NOT touch `obj->textureframe[]`
or any UV `tu/tv` field** — the per-particle render simply calls
`RenderObject(obj)` after setting the matrix. So the per-particle texture
animation, if any, is whatever frame the engine defaults to via the
imagery's STILL state — most likely frame 0 (the asset is a STILL imagery
per the magic at file offset 0x18, `"STILL\0\0\0"`).

In other words: **no flipbook / atlas animation per particle**. The
visual diversity comes from there being two **distinct sub-object
sprites** (`smoke` and `smoke01`), and the animator selects per-particle
which sub-object's quad to draw — not from per-frame texture cycling.

**UV mapping.** Each sub-object's quad uses the standard authored UVs
(full texture, `(0,0)..(1,1)` corners — confirmed from the file header
at 0x180-0x21c showing `0x3f800000`=1.0f UV constants). No sub-rect
selection in the render body.

The effect loads real assets — do NOT substitute procedural sprites
(per `feedback-no-standins`).

---

## 5. Spawn & emit

- **Trigger semantics:** one-shot spell cast → animator runs for
  exactly **100 render frames** (`FFLASH_NOMOREOFTHISEFFECT`) and dies
  via `KillThisEffect()`. Three phases on a single timeline:
  - frames 0..24: SMOKEY1 flame column (continuous spawning per `add[i]` table)
  - frames 25..34: SMOKEY1 column off, GSPHERE growing
  - frame 35: GSPHERE→RING swap (one-tick state change)
  - frames 35..N: RING expanding (until `rsize >= 300` or framenum = 100)
- **Count per trigger:** up to **150 particles simultaneously**. At
  Initialize, slots 0..74 are pre-seeded with `state = GSPHERE` (and
  random per-particle Euler angles); slots 75..149 are `USEME` (free).
  During frames 0..24 the animator pulls from the USEME pool to spawn
  SMOKEY1 particles. At frame 35 all GSPHERE slots flip to USEME and then
  the explosion immediately re-promotes all 150 to RING.
- **Initial direction / distribution per phase:**
  - **SMOKEY1 (column)** — per-particle, at spawn:
    `dist = random(0, FFLASH_F1_MAXDIST=15)`,
    `angle = random(0, 255)` (8-bit byte-angle),
    `(pos.x, pos.y, pos.z) = (ConvertToVector(angle, dist).x,
    ConvertToVector(angle, dist).y, FFLASH_F1_BOTTOM=1)` — i.e. a
    uniform-random XY point within a disc of radius 15 wu, just above the
    ground (z=1). `vel = (0, 0, 2 + random(0,2))` — pure-upward velocity
    of 2..4 wu/tick, no XY drift, no gravity.
  - **GSPHERE** — at Initialize, particles 0..74 get random Euler angles
    `(angle.x, angle.y, angle.z) = (random(0,359)·2π/360,
    random(0,359)·2π/360, random(0,359)·2π/360)` and identical angvel
    `(0.12, 0.12, 0.12) rad/tick`. Per tick each angle steps by angvel; the
    particle's position is computed via
    `pos = pivot + (RotX(angle.x) · RotY(angle.y) · RotZ(angle.z))
    · (0, gsize, 0)` (matrix transforms the `(0, gsize, 0)` unit vector by
    the per-particle 3-axis rotation). `pivot = (0, 0, 30)` for every
    particle.
  - **RING** — at explosion-frame 35, all GSPHERE slots are freed to USEME
    and re-promoted to RING. Each RING particle gets
    `angle.z = (newringparts · 2π) / 150` (i.e. evenly spaced byte
    positions around the ring), `angvel.z = 0.3 rad/tick`, `angle.x =
    angle.y = 0`, `angvel.x = angvel.y = 0`. Per tick: `angle.z += 0.3`,
    `dist = rsize` (the shared growing radius), `pos = pivot +
    (RotX(0) · RotY(0) · RotZ(angle.z)) · (0, rsize, 0)` — i.e. a flat
    XY-plane orbit of radius `rsize`. (Note: the explosion-frame spawn
    block at `effect_old.cpp:2184-2211` is **commented out**; the active
    code at `:2214-2218 + 2289-2305` instead promotes them via the USEME
    fallback in the per-particle Animate loop. See §6.3.)
- **Emit anchor convention — the most unusual property of this effect:**
  - When the casting spell has a target (`spell->GetInvoker()->Fighting()`
    returned non-null, set at `effect_old.cpp:2143`), the render body
    sets `obj->flags |= OBJ3D_ABSPOS` and `origin = target->GetPos()`
    (`:2444-2447`). All particle positions are written as
    `obj->pos = sm[i].pos + origin`, and **`OBJ3D_ABSPOS` makes the
    engine render at absolute world position, not relative to the
    effect's own location.** So the explosion appears at the **enemy
    being struck**, not where the effect object was spawned.
  - When there is no target (`target == nullptr`), `origin = (0,0,0)`
    and `OBJ3D_ABSPOS` is NOT set, so the effect renders at its own
    `TObjectInstance` world position (set by the spell system on spawn).
- **Coordinate space:** SMOKEY1 `pos` is **local** (within ±15 wu of the
  effect/target origin, z up to ~150 over the column's life). GSPHERE and
  RING `pos` are **local-relative-to-pivot** `(0,0,30)` (so the sphere/ring
  centre is 30 wu above the effect/target origin). All `pos` values get
  the final +origin in the render body.
- **Spread / jitter:** SMOKEY1 spawn — uniform XY in a 15-wu disc;
  GSPHERE initial — uniform spherical (3-axis random Euler angles);
  RING spawn — even angular distribution around the ring.

### 5.1 Spatial diagram (peak — frame ~35 = explosion moment)

```
                                    wz (up)
                                    │
                                    │  GSPHERE: 75 particles orbiting the
                                    │  pivot (0,0,30) on 3 random axes,
                                    │  radius gsize → growing 1→20 over
                                    │  frames 26..34, then frozen at 20
                                    │  during transition
                                    │
                                    │           ●  ●
                                    │       ●           ●
                                    │   ●     gsphere      ●        ←  z = 30  (pivot)
                                    │       ●  (0,0,30)  ●               radius gsize 1..20
                                    │         ●        ●
                                    │             ●  ●
                                    │
                                    │   ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─  (frame 35 onward)
                                    │         RING particles orbit the same pivot in the XY-plane at z=30,
                                    │         radius rsize starts at gsize=20 and grows +8/tick →
                                    │         radius 300 (then particles freed).
                                    │
                                    │   ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑     SMOKEY1 rising flame column:
                                    │   ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑     vel = (0, 0, 2..4) wu/tick
                                    │   ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑     spawn within radius 15 disc at z=1,
                                    │   ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑     z grows linearly with vel.z over 20..45 frames
                                    │                        (cap height ≈ 90..180 wu),
                                    └──○─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─  no XY drift, no gravity, per-particle
                                       │                       size-fade in the last (100-FADEPCT)=80% of life.
                                       ↓
                                  emit origin
                              (target world pos
                              if `target != null`
                              via OBJ3D_ABSPOS;
                              else effect's own
                              world pos)
```

---

## 6. Behavior & per-frame logic

### 6.1 `TFireFlashEffect::Initialize()` (`effect_old.cpp:1991-2004`)

```
Initialize():
    if (spell):                                                            // :1993
        invoker = (PTCharacter)spell->GetInvoker()                         // :1995
        if (invoker):                                                       // :1997
            target = (PTCharacter)invoker->Fighting()                       // :1999
            if (target):                                                    // :2000
                spell->Damage(target)                                        // :2001   ← one-shot damage at cast
```

The effect class itself does nothing visual — it applies the spell's
damage to the caster's combat opponent ONCE at spawn, then defers
everything to the animator.

### 6.2 `TFireFlashAnimator::Initialize()` (`effect_old.cpp:2113-2162`)

```
Initialize():
    T3DAnimator::Initialize()                                              // :2115
    ((TFireFlashEffect*)inst)->Initialize()                                // :2117  ← (re-)applies damage; redundant safety
    PLAY("FireFlash")                                                       // :2119  ← one-shot sfx (snapshot bug: see §13)

    framenum = 0                                                            // :2121
    facing = (float)((TEffect*)inst)->GetAngle()                            // :2124  ← caster facing 0..255

    // Compute goffset (computed but UNUSED — see §13)
    ConvertToVector(((255 - facing) + (138·255/360)) & 255,                 // :2127
                    FFLASH_GS_INFRONT=10, goffset)                          //   = ConvertToVector(angle, 10, …)
    goffset.z = FFLASH_GS_INFRONTUP=5                                       // :2128

    gsphere = (0, 0, 30)                                                    // :2130-2132  ← fixed pivot
    gsize   = FFLASH_GS_MINSIZE1=1.0                                        // :2133

    // Optional damage target (used for OBJ3D_ABSPOS render anchor, §5)
    target = nullptr                                                        // :2135
    if (spell = ((TFireFlashEffect*)inst)->GetSpell()):                     // :2137-2138
        if (invoker = spell->GetInvoker()):                                  // :2140-2141
            target = (PTCharacter)invoker->Fighting()                         // :2143

    // Pool init: zero all particles, seed first 75 as GSPHERE
    memset(sm, 0, sizeof(FFLASH_PARTICLE) * 150)                            // :2148

    for i = 0 .. 74:                                                         // :2151
        sm[i].angle.x = random(0,359) / 360 · 2π                              // :2153
        sm[i].angle.y = random(0,359) / 360 · 2π                              // :2154
        sm[i].angle.z = random(0,359) / 360 · 2π                              // :2155
        sm[i].angvel.x = 0.12                                                 // :2156
        sm[i].angvel.y = 0.12                                                 // :2157
        sm[i].angvel.z = 0.12                                                 // :2158
        sm[i].state = FFLASH_STATE_GSPHERE                                    // :2160
    // Slots 75..149 left as state=USEME=0 (zero-init from memset)
```

### 6.3 `TFireFlashAnimator::Animate(bool draw)` (`effect_old.cpp:2164-2390`)

The full per-frame logic. Three coordinated stages share one framenum
counter + one particle pool.

```
Animate(draw):
    T3DAnimator::Animate(draw)                                             // :2166
    inst->SetCommandDone(false)                                            // :2167  ← keep effect ALIVE
    framenum++                                                              // :2169

    newringparts = 0                                                        // :2171

    // ─── PHASE STATE MACHINE (gsphere → ring) ───
    if (FFLASH_GS_ON=26 <= framenum < FFLASH_EXPLOSION=35):                 // :2174  ← sphere growing window
        if (gsize < FFLASH_GS_MAXSIZE1=20.0):                                 // :2176
            gsize += FFLASH_GS_SIZEVEL1=2.0                                    // :2177
        else:
            gsize = 20.0                                                       // :2179
    else if (framenum == FFLASH_EXPLOSION=35):                              // :2181  ← explosion snap (one-tick)
        // (commented-out direct-replace spawn block at :2184-2211 — see §13)
        // Active code: free all GSPHERE → USEME, then init ring
        for i = 0..149:                                                       // :2214
            if (sm[i].state == FFLASH_STATE_GSPHERE):                          // :2216
                sm[i].state = FFLASH_STATE_USEME                                // :2217
        rsize = gsize                                                          // :2221  ← ring starts where sphere ended
        newringparts = FFLASH_RI_NUMPARTICLES=150                              // :2223  ← request 150 ring births
    else if (framenum > 35):                                                // :2225  ← ring expanding window
        rsize += FFLASH_RI_SIZEVEL1=8.0                                       // :2227
        if (rsize >= FFLASH_RI_MAXSIZE=300.0):                                // :2229
            for i = 0..149:                                                    // :2231
                sm[i].state = FFLASH_STATE_USEME                                // :2232  ← release all on ring kill

    // ─── SMOKEY1 SPAWN RATE TABLE (column) ───
    newsmokey1s = 0
    if      (framenum < FFLASH_F1_ON=0):     newsmokey1s = 0                // :2239-2241
    else if (framenum < FFLASH_F1_TIME1=5):  newsmokey1s = 4; mainscale=0.3  // :2243-2247  (frames 0..4)
    else if (framenum < FFLASH_F1_TIME2=15): newsmokey1s = 7; mainscale=0.3  // :2248-2252  (frames 5..14)
    else if (framenum < FFLASH_F1_TIME3=16): newsmokey1s = 7; mainscale=0.3  // :2253-2257  (frame 15)
    else if (framenum < FFLASH_F1_TIME4=25): newsmokey1s = 2; mainscale=0.3  // :2258-2262  (frames 16..24)
    // (frames >= 25: newsmokey1s stays 0 — column off)

    // ─── PER-PARTICLE LOOP ───
    for i = 0..149:
        if (sm[i].state == FFLASH_STATE_USEME):                              // :2266
            if (newsmokey1s):                                                  // :2270
                // ── Spawn SMOKEY1 (rising flame) ─────────────────────────
                dist = random(0, FFLASH_F1_MAXDIST=15)                          // :2275
                angle = random(0, 255)                                           // :2276
                ConvertToVector(angle, dist, s3dp)                              // :2277
                sm[i].pos.x = s3dp.x                                             // :2278
                sm[i].pos.y = s3dp.y                                             // :2279
                sm[i].pos.z = FFLASH_F1_BOTTOM=1                                 // :2280
                sm[i].life = FFLASH_F1_FADEMIN(20)                               // :2281
                              + FFLASH_F1_MAXFADE(25) · (15 − random(0,dist)) / 15
                sm[i].startfade = sm[i].life · FFLASH_F1_FADEPCT(20) / 100       // :2282
                sm[i].stopfade  = sm[i].life                                     // :2283
                sm[i].vel.z = 2.0 + random(0, 2)                                  // :2284  ← 2..4 wu/tick straight up
                sm[i].state = FFLASH_STATE_SMOKEY1                                // :2285
                newsmokey1s--                                                     // :2287
            else if (newringparts):                                            // :2289
                // ── Spawn ring particle as a SMOKEY1 (note: NOT as state=RING)
                // Bug/note: this branch is reached at explosion-frame 35 because
                // the active "free GSPHERE → USEME" then "spawn ring" sequence
                // routes through this USEME slot. But the state assigned is
                // SMOKEY1, NOT RING — see §13 for the dead RING branch.
                dist = (int)rsize                                                 // :2292
                angle = random(0, 255)                                             // :2293
                ConvertToVector(angle, dist, s3dp)                                // :2294
                sm[i].pos.x = gsphere.x + s3dp.x                                   // :2295  ← centred on pivot
                sm[i].pos.y = gsphere.y + s3dp.y                                   // :2296
                sm[i].pos.z = gsphere.z                                            // :2297  ← z=30 (pivot height)
                sm[i].life = 20 + 25·(15 − random(0,dist))/15                      // :2298  (note: dist=rsize=20+ now)
                sm[i].startfade = sm[i].life · 20 / 100                            // :2299
                sm[i].stopfade  = sm[i].life                                       // :2300
                sm[i].vel.z = 2.0 + random(0, 2)                                    // :2301
                sm[i].state = FFLASH_STATE_SMOKEY1                                  // :2302  ← rising particles, not orbit ring
                newringparts--                                                      // :2304

        else if (sm[i].state == FFLASH_STATE_SMOKEY1):                        // :2307
            // ── SMOKEY1 motion: rise + age ─────────────────────────────
            sm[i].pos.z += sm[i].vel.z                                          // :2311  ← +2..4/tick straight up
            sm[i].life--                                                         // :2312
            if (sm[i].life == 0):                                                // :2314
                sm[i].state = FFLASH_STATE_USEME                                   // :2315  ← die, return to pool

        else if (sm[i].state == FFLASH_STATE_GSPHERE):                        // :2317
            // ── GSPHERE motion: orbit pivot on 3 axes ───────────────────
            sm[i].pivot = gsphere                                                 // :2321-2323
            sm[i].angle.x += sm[i].angvel.x                                       // :2325
            sm[i].angle.y += sm[i].angvel.y                                       // :2326
            sm[i].angle.z += sm[i].angvel.z                                       // :2327
            sm[i].dist = gsize                                                    // :2329  ← all sphere particles at same radius
            sm[i].scale = gsize · 0.01                                            // :2330  ← per-particle scale proportional to sphere size

            mat = identity                                                        // :2333
            D3DMATRIXRotateX(mat, sm[i].angle.x)                                  // :2335
            D3DMATRIXRotateY(mat, sm[i].angle.y)                                  // :2336
            D3DMATRIXRotateZ(mat, sm[i].angle.z)                                  // :2337
            v = (0, sm[i].dist, 0)                                                // :2341-2343
            D3DMATRIXTransform(mat, v, v)                                         // :2345  ← rotate the +Y unit vector
            sm[i].pos = sm[i].pivot + v                                           // :2347-2349

        else if (sm[i].state == FFLASH_STATE_RING):                           // :2351  (UNREACHABLE — see §13)
            // ── RING motion: orbit pivot in XY plane ────────────────────
            sm[i].pivot = gsphere                                                 // :2355-2357
            sm[i].angle.x += sm[i].angvel.x  (= 0)                                // :2359
            sm[i].angle.y += sm[i].angvel.y  (= 0)                                // :2360
            sm[i].angle.z += sm[i].angvel.z  (= 0.3 rad/tick)                     // :2361
            sm[i].dist = rsize                                                    // :2363
            mat = identity                                                        // :2366
            D3DMATRIXRotateX(mat, sm[i].angle.x)                                  // :2368
            D3DMATRIXRotateY(mat, sm[i].angle.y)                                  // :2369
            D3DMATRIXRotateZ(mat, sm[i].angle.z)                                  // :2370
            v = (0, sm[i].dist, 0)                                                // :2374-2376
            D3DMATRIXTransform(mat, v, v)                                         // :2378
            sm[i].pos = sm[i].pivot + v                                           // :2380-2382

    // ─── DEATH ────────────────────────────────────────────────────────
    if (framenum >= FFLASH_NOMOREOFTHISEFFECT=100):                          // :2386
        ((TEffect*)inst)->KillThisEffect()                                    // :2387
    return
```

> **Note on the dead RING branch (`:2351-2383`).** The state-promotion
> from GSPHERE→RING is **commented out** (the explicit promotion block at
> `:2184-2211` is wrapped in `/* … */`). The active path at `:2214-2218`
> just frees GSPHERE→USEME, then in the per-particle USEME branch at
> `:2289-2305` the `newringparts` slots are spawned as **SMOKEY1**, NOT as
> RING. So the `else if (state == FFLASH_STATE_RING)` branch at `:2351` is
> **unreachable in the snapshot's active code**, and the actual
> "explosion ring" visual is implemented as **a burst of rising SMOKEY1
> particles spawned at radius `rsize` around the pivot**, not as orbiting
> RING particles. See §13 — this is a pre-release WIP state where the
> "ring" was being re-tooled from an orbiting flat-disc to a radial burst.
> **The shipped retail almost certainly resolved this** (the kill-cap=100
> matches, suggesting the overall structure shipped), but which of the
> two "ring" implementations shipped is unverified in retail decomp — the
> active SMOKEY1-burst code is what the snapshot says runs.

### 6.4 `TFireFlashAnimator::Render()` (`effect_old.cpp:2392-2479`)

```
Render():
    SaveBlendState()                                                       // :2397
    SetAddBlendState()                                                     // :2398  ← Additive (DECALALPHA + ONE/ONE)
    ResetExtents()                                                          // :2400

    for i = 0..149:                                                          // :2403
        // ── PICK SUB-OBJECT based on state + per-particle stage ────────
        if (sm[i].state == FFLASH_STATE_SMOKEY1):                            // :2405
            // Use cool/smoke sprite once the particle is in its "smoke" stage
            // (after FFLASH_F1_COLORPCT=70% of fade-window elapsed)
            if ((sm[i].stopfade - sm[i].life) >=                              // :2407
                (sm[i].stopfade - sm[i].startfade) · 70 / 100):
                obj = GetObject(0)         // smoke
            else:
                obj = GetObject(1)         // smoke01 (hot)
            // ── PER-PARTICLE SCALE FADE ────────────────────────────────
            if (sm[i].life > 0):                                             // :2412
                if ((sm[i].stopfade - sm[i].life) >= sm[i].startfade):        // :2414
                    // In the size-decay window — interpolate down toward MINSIZE
                    sm[i].scale = mainscale · sm[i].life                       // :2415
                                  / (sm[i].stopfade - sm[i].startfade)
                                  + FFLASH_F1_MINSIZE=0.05
                else:
                    sm[i].scale = mainscale                                    // :2417  ← full size
        else if (sm[i].state == FFLASH_STATE_GSPHERE):                       // :2420
            if (framenum >= FFLASH_GS_ON=26):                                  // :2422
                obj = GetObject(1)        // smoke01 (hot)
            else: continue                                                     // :2428  ← don't render before frame 26
        else if (sm[i].state == FFLASH_STATE_RING):                          // :2430
            obj = GetObject(1)            // smoke01 (hot)
        else:
            continue                                                           // :2436  ← USEME/dead — skip

        if (sm[i].scale > 0):                                                 // :2438
            // ── RENDER ANCHOR: target-locked if available ─────────────
            obj->flags = OBJ3D_MATRIX                                         // :2442
            if (target):
                obj->flags |= OBJ3D_ABSPOS                                    // :2445  ← anchor at target world pos
                target->GetPos(origin)                                         // :2446
            else:
                origin = (0,0,0)                                              // :2450  ← anchor at effect's own pos

            // ── BUILD OBJECT MATRIX (rotation: world-space, fixed-tilt) ─
            D3DMATRIXClear(obj->matrix)                                        // :2453  ← identity
            D3DMATRIXRotateX(obj->matrix, -(M_2PI / 3.0))   // = -120°         // :2455
            D3DMATRIXRotateZ(obj->matrix, -(M_PI / 4.0))    // =  -45°         // :2456
            D3DMATRIXRotateZ(obj->matrix, -(facing/256 · M_2PI))               // :2457  ← caster's facing in radians
            //   (Note: snapshot has typo `256.0ff`; intent `256.0f`)

            // ── SCALE ────────────────────────────────────────────────
            obj->scl = (sm[i].scale, sm[i].scale, sm[i].scale)                 // :2459-2461
            D3DMATRIXScale(obj->matrix, obj->scl)                              // :2462

            // ── TRANSLATE (particle local pos + target/effect origin) ─
            obj->pos = sm[i].pos + origin                                      // :2464-2466
            D3DMATRIXTranslate(obj->matrix, obj->pos)                          // :2468

            RenderObject(obj)                                                  // :2470  ← submit

    UpdateExtents()                                                            // :2474
    RestoreBlendState()                                                        // :2476
    return true
```

**The render matrix chain is right-multiplicative** (each `D3DMATRIX*Rotate*`
post-multiplies the matrix). So the final composite for each particle is:

```
M = Identity
    · RotX(-120°)         // tilt the world XY plane forward by 120°  (NOT -2π/3
                          //   meaning "screen-aligned"; this is a fixed tilt
                          //   in world axes; the "+Y" authored quad ends up
                          //   pointing roughly toward the iso camera)
    · RotZ(-45°)          // 45° in-plane spin
    · RotZ(-facing·2π/256) // additional Z rotation by the caster's facing
                          //   angle (caster-facing-aligned, NOT camera-aligned)
    · Scale(particle scale, uniform)
    · Translate(particle world pos)
```

Both Z-rotations combine into a single `RotZ(-(45° + facing-as-rad))`. So the
effective orientation is **fixed-tilt-in-world** with one Z spin that varies
with caster facing. **It is NOT screen/camera-aligned**, **NOT
velocity-aligned**, and **NOT WorldXY (`rot.x = -π/2`)** — it is a custom
world-axis tilt at -120° X plus a Z-spin equal to (45° + caster-facing).

### 6.5 `TFireFlashAnimator::RefreshZBuffer()` (`effect_old.cpp:2481-2494`)

```
RefreshZBuffer():
    size_x = 200                                                            // :2485
    size_y = 200                                                            // :2486
    map = (0, 0, 0)                                                          // :2488-2490
    WorldToScreen(map, screen)                                               // :2491
    RestoreZ(screen.x - size_x/2,                                            // :2492
             screen.y - size_y,
             size_x, size_y)
```

Restores Z for a 200×200 px screen box centred horizontally on the
projected `(0,0,0)` (the effect's own world origin — **NOT** the target
position, even when `target != null`). This is one of the snapshot bugs
the reconstruction will need to consider: when the visual is anchored to
`target` via OBJ3D_ABSPOS, the Z-restore patch is anchored at the
effect's own (irrelevant) position. The `(int)map.x/y/z = 0` literals are
local-space coordinates, so `WorldToScreen` here is projecting the local
origin of the effect via the engine's current camera/projection.

### 6.6 Temporal diagram

```
                     SMOKEY1 column (vel.z=+2..4, no gravity, no XY drift)
                     ════════════════════════════════════════
framenum    0       5     15  16            25              35            ~85     100
            ●═══════╪═════╪══╪═══════════════╪═══════════════╪═════════════╪══════╪──→ time (24 Hz)
add/frame:  4│4│4│4 7│7│… 7  2│2│2│2│2│2│2│2 0│0│0│0…       (column off)
mainscale:  ─────────  0.3  ─────────────────
                                                ↓
                  GSPHERE growing (size 1→20 over frames 26..34)
                  ━━━━━━━━━━━━━━━━━━
                  rendered from frame 26
                                                ▼ frame 35 = explosion snap
                                                  - all GSPHERE → USEME
                                                  - rsize = gsize = 20
                                                  - 150 ring births requested
                                                  ↓ (which spawn as SMOKEY1 burst —
                                                    see §6.3 dead-RING-branch note)
                                                ┃━━━━━━━━━━━━━━━━━━━━━━┃
                                                  RING / ring-burst phase: rsize +=8/tick;
                                                  300 wu reached around frame ~35+(300-20)/8 = ~70
                                                  (then all particles freed)
                                                                                   ▼
                                                                            framenum >= 100
                                                                            KillThisEffect()
```

---

## 7. Rendering (original render state + geometry)

- **What it draws:** for each of up to 150 active particles, **one I3D
  mesh sub-object** (`smoke` or `smoke01`, chosen per-state) drawn with a
  per-particle scale + the shared fixed-tilt-+-facing rotation matrix +
  translation to (particle_pos + origin). Each draw is a single
  `RenderObject(obj)` call. **NOT** a billboard quad — these are
  authored I3D quads (the sub-object's authored verts) with an explicit
  object matrix.

- **Blend mode (original — what the code actually does):** **Additive
  (`SetAddBlendState`)** — `effect_old.cpp:2398`. Quoting the call site
  verbatim:
  ```c
  // effect_old.cpp:2397-2398
  SaveBlendState();
  SetAddBlendState();
  ```
  The helper body at `effect_old.cpp:235-244`:
  ```c
  bool SetAddBlendState()
  {
      TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_DECALALPHA));
      TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, false));
      TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE));
      TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE));
      TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZENABLE, true));
      return true;
  }
  ```
  Classify as **AdditiveStraight** (NOMENCLATURE §3): SRC=ONE, DST=ONE,
  Z-test on, Z-write off, texture stage = DECALALPHA.

  ### BLEND SANITY-CHECK (mandatory)

  **Sprite design:** the `Magic\fireflash.I3D` texture (RGB565 at the
  surface descriptor offset 0x48, dims 128×160 per the surface header)
  is a typical bright-fire-on-key sprite (the M04 Burn doc confirms the
  sister fire-spell additive family pattern). Bright glow + key
  background + intended visual = "bright fiery explosion" → **Additive
  is the correct sister-family choice.**

  **Sister-family cross-check:**
  - `TFireConeAnimator::Render` (`src/effect_old.cpp:7211-7320`) —
    sister fire spell, `SetAddBlendState()`. **Same additive.**
  - `TFireWindAnimator` — sister fire spell (animator merged in same
    cls_0x5a9194), same family.
  - `TBurnAnimator` (M04 forensics doc) — `SetAddBlendState()`. **Same
    additive.**
  - The fire-spell family (FireFlash / FireWind / FireCone / Burn /
    DragonFire / Aura) is consistently Additive — NOT alpha. This is
    consistent with M04's §7 finding.

  **Verdict (sanity-check passes):** the snapshot's `SetAddBlendState`
  is the **correct** sister-family choice for a fire-spell glow. Unlike
  F01 (alpha-coded glow on a green-keyed sprite — suspect; that's the
  ambient torch sub-family) or F03 (Alpha + DECAL for ambient ground
  fire), FireFlash is in the **offensive cast spell family** which is
  uniformly Additive. **No drift risk on the blend.**

  Per snapshot the render state is:
  - `D3DRENDERSTATE_TEXTUREMAPBLEND = D3DTBLEND_DECALALPHA` — texel ×
    per-vertex alpha (the I3D's authored alpha decides how much texel
    contributes per pixel; with Additive output, dark texels contribute
    nothing and bright texels add directly to the framebuffer)
  - `D3DRENDERSTATE_ZWRITEENABLE = false` — no Z-write (NOMENCLATURE §5
    "TestNoWrite")
  - `D3DRENDERSTATE_ZENABLE = true` — Z-test on (occluded by geometry
    closer than the particle's depth)
  - `D3DRENDERSTATE_SRCBLEND = D3DBLEND_ONE`
  - `D3DRENDERSTATE_DESTBLEND = D3DBLEND_ONE`
  - **CULLMODE is unchanged from prior state** — `SetAddBlendState` does
    not set CULLMODE (unlike `SetBlendState`); the cull mode persists
    from whatever previous code set.

- **Lit vs self-lit:** **self-lit / unlit** (NOMENCLATURE §4). The
  animator does not write per-vertex diffuse, does not zero the
  material, does not fold ambient light in. With Additive +
  DECALALPHA the texel's RGB is added to the framebuffer directly
  (alpha-weighted but not modulated by scene light). The sprite reads
  at its authored brightness.

- **Depth / Z:** **TestNoWrite** — Z-test on, Z-write off. The
  particles read the depth buffer (so terrain occludes them) but
  don't write into it (so overlapping particles correctly composite
  additively). `RefreshZBuffer` restores a 200×200 px patch of the
  scene Z at the effect's projected origin to compensate for the
  no-write — see §6.5.

- **Cull:** snapshot does not touch CULLMODE in `SetAddBlendState`;
  it persists from whatever previous draw set. **Likely `D3DCULL_NONE`**
  (the default in `SetBlendState` in some sibling effects — but
  `SetAddBlendState` notably doesn't touch it). In practice this means
  cull mode is **environment-dependent** — a snapshot WIP gap.

- **Orientation / transform space (the headline answer):**
  - **NOT ScreenAligned** — no per-frame camera-facing math.
  - **NOT WorldXY** — no `rot.x = -π/2` ground-tipping.
  - **NOT velocity-aligned** — no per-particle vel-vector rotation.
  - **CUSTOM WORLD-AXIS FIXED-TILT WITH CASTER-FACING Z-SPIN**
    (NOMENCLATURE would need a new term — closest is "world-axis
    explicit-tilt"). The render matrix is built from **world-axis
    Euler rotations** (`RotX(-120°) · RotZ(-45° - facing_radians) ·
    Scale · Translate`). The X rotation is FIXED (`-2π/3 = -120°` at
    every render — NOT a screen tell), the Z rotation depends on the
    **caster's facing at spawn** (`facing` is captured in
    `Initialize`, never updated; `effect_old.cpp:2124`). So the
    explosion orients to where the caster was facing at cast time
    and stays fixed in world space for its full life.

- **Per-particle transform:** as above; `obj->flags = OBJ3D_MATRIX`
  (explicit matrix, the engine uses the matrix directly rather than
  composing from `rot`/`scl`/`pos` separately). If `target != null` the
  `OBJ3D_ABSPOS` flag is OR'd in (`:2445`), making the matrix
  translation absolute world-space at the target position.

- **Per-vertex color packing:** **NONE written by the animator.** The
  particles use whatever per-vertex diffuse the imagery's authored
  verts ship with (likely opaque white, so the texel passes through
  un-modulated under DECALALPHA + Additive).

---

## 8. Texture animation

**Mechanism: none in the render path.** The `Render` body does NOT
touch `obj->textureframe[]` (no flipbook handle swap), does NOT
rewrite `obj->lverts[i].tu/tv` (no UV scrolling or sub-rect pick),
does NOT call `SetTextureFrame()`. Each particle just draws the
sub-object as authored.

The I3D's `numframes = 8` in the texture descriptor (header offset
0x6c) is **not** advanced per-particle by this effect — it ships
multi-frame texture data but the FireFlash render code uses only
frame 0. (Sister effects like F03 TFireAnimator and the missile
family DO advance `textureframe[]` per-tick; FireFlash does not.)

The **temporal variation** the player sees comes from:
1. Per-particle scale fade (size envelope) — SMOKEY1 only.
2. Per-particle motion (rising, orbiting, expanding).
3. Phase changes (column → sphere → ring/burst).
4. Sub-object switching mid-life (SMOKEY1 swaps from `smoke01` to
   `smoke` at 70% fade — different texture appearance for the
   "cooling" half of each flame particle).

NOT from UV/texture cycling within a single particle.

---

## 9. Associated light

**N/A — the animator emits no dynamic light.** No `AddPointLight`,
no light system calls anywhere in `TFireFlashEffect`/`TFireFlashAnimator`
(`effect_old.cpp:1973-2494`).

**HOWEVER** — the casting **spell** has an associated light, applied
by the spell system around the cast, not by the animator:

`data/Resources/spell.def:74` (and `data/resources_unzipped/spell.def:74`):
```
LIGHT COLOR 255, 130, 0 INT 160 MULT 20
```

Per the SPELL block, the spell system adds a dynamic light at the
strike point: **bright orange `(255, 130, 0)` (warm fire) at intensity
160, multiplier 20**. This is the firelight that brightens the scene
while the spell is being cast / when the strike resolves — the visual
"glow" that makes the spell feel like a real fire eruption.

The light's **placement, attachment, and lifetime are decided by the
spell system, not by the animator** — likely attached to the strike
target for the duration of the `Damage()` resolution, then released.
This is a per-spell convention, NOT a per-effect convention; record
it so the reconstruction wires the light through the spell system
(not as part of the animator).

---

## 10. Color

- **Source:** the authored `Magic\fireflash.I3D` texture pixels
  (both `smoke` and `smoke01` sub-objects share the I3D's single
  texture slot at numtex=1). The effect supplies **no per-vertex
  tint** (`lverts[i].color` is never written), **no material
  override** (no material-zeroing in Initialize, no per-render diffuse
  push), and **no spell color** (the spell's `LIGHT COLOR 255,130,0`
  is the *light* color, applied by the lighting system, not a
  per-particle tint).
- **Exact values:** texture-driven. Per the I3D header (RGB565
  surface, dims 128×160), the texels are encoded in 16-bit color.
  The expected dominant texels for a fire explosion sprite are:
  - **Hot core / center of explosion:** bright white-yellow
    (~`R=248, G=252, B=192-232`, the typical "flame core" hot
    palette seen in F01/M04 family textures).
  - **Mid flame:** saturated orange (~`R=240, G=160-200, B=0-40`)
    matching the spell's `LIGHT COLOR 255, 130, 0`.
  - **Cooler smoke (`smoke` sub-object late-life):** grey/dark
    smoke or darker red-orange.
  - **Chroma key:** likely **black (RGB565 `0x0000`)** — the
    Magic\ family fire effects in the snapshot are typically
    black-keyed (per the M04 forensics + the 20%-black-pixel
    chroma rule in `src/3dimage.cpp:1701-1721`). Note this is the
    *opposite* convention from F01 TFlameAnimator's `flame.i3d`
    which is green-keyed (only 17.7% black, 54% green).
    The reconstruction should verify the actual chroma channel
    via a histogram of the extracted texture.
- **Expected visual:** **bright hot orange-red explosion with
  white-hot center**, fading through orange/red into smoky
  greys at particle end-of-life. Per
  `feedback-vfx-color-health-signal`, a pale / grey / colorless
  port = port broken (typical causes: chroma-key handling missed
  / wrong channel, wrong blend, wrong material modulation,
  texture not loaded).
- **Normalization / boosts:** none — no `NormalizeColors`, no
  per-vertex color writing, no ambient mixing.

---

## 11. Audio coupling

**One-shot sfx at spawn.** `effect_old.cpp:2119`:
```c
PLAY("FireFlash");
```
inside `TFireFlashAnimator::Initialize()`. Plays the shipped
`Sound/effects/fireflash.wav` (192,972 B, 1998-11-23). Routes
through the SoundPlayer registry by name.

**Retail audio path** — `cls_0x5a9194` has a function-pointer
call sequence at the kill-tail (`recon/classes/cls_0x5a9194.cpp:108-110`
— `(**(code **)(*(int *)this->mbr_0xd8 + 0xc))(uVar3, uVar5);`)
which is consistent with the SoundPlayer's call-through-vtable
pattern, but this is the death-time call (likely a fadeout /
release), not the spawn-time `PLAY`. The retail spawn-time PLAY
must live in `virt_meth_0x4e18c0` (the vftable slot 24 entry,
which XREFs `s_FireFlash_005e0efc` at `0x4e18d8` — same `FireFlash`
string as the audio key, since the sound name and the spell name
both use the literal `"FireFlash"`).

> **Snapshot vs retail audio difference (minor) — see §13.** The
> snapshot calls `PLAY` inside `TFireFlashAnimator::Initialize`
> (after the SetCommandDone in `Animate`); the retail audio call
> in cls_0x5a9194's tail-after-frame-100 is a *kill-time* call.
> Whether retail moved the spawn-time PLAY into the animator's
> Initialize or onto the effect's Initialize is not certain from
> the partial decomp.

**No looping ambient sound** during the effect — the explosion
is one-shot at spawn. **No per-particle audio** (no
`SoundPlayer.Play` calls in the particle loop).

---

## 12. Triggers & in-game appearance

- **Spawned by:** `SPELL "Fire Flash"` (`data/Resources/spell.def:66-98`):
  - **VARIANT `"Fire Flash"`** (`:79`) — Level-1 fire-school, talisman
    `"LI"` (Light/Inhabit), mana 31, damage 50..55, skill 1, pattern
    `"circle"` radius 1. Player-cast offensive spell.
  - **VARIANT `"Priest Fire Flash"`** (`:89`) — Level-1, talisman `"MA"`
    (Mind/Air), mana 186, damage 56..68, skill 0, pattern `"circle"`
    radius 20, hits 2. NPC-only variant (the talisman `"MA"` is a
    priest-class restriction, and the higher mana + radius +
    multi-hit profile suggests boss/elite priest enemies). Both
    variants invoke the `"fireflash"` builder string (lowercase) which
    matches `s_fireflash_005e0938` in the retail binary (the spell-
    system name — different from the `s_FireFlash_005e1080` builder/
    animator registration name).
- **Where to see it in-game:**
  - **Player cast:** in any spellbook-equipped session as a Level-1
    fire spell using the `"LI"` talisman pair. Cast on a melee combat
    target — the explosion appears at the target's location (per the
    §5 / §6.4 `target = invoker->Fighting()` + `OBJ3D_ABSPOS` logic).
  - **Priest variant:** likely cast by hostile priest NPCs at the
    player. Note the high mana cost (186) and the larger AoE
    (radius 20, hits 2) — this is a more substantial NPC spell.
- **Live in retail?** **YES** — both variants are in the shipped
  `data/resources_unzipped/spell.def` (same text as legacy), the
  asset is in `data/imagery.rvi`, the registration is in the binary,
  and the audio asset shipped. **Unlike F09 / F11 (vestigial fire
  family), FireFlash is fully live in the shipped game.**
- **NOT vestigial.** Reconstruction can ground-truth this against
  in-game footage by casting the Level-1 Fire Flash spell on any
  combat target.

---

## 13. Gaps & uncertainties

### 13.1 The dead RING branch (the biggest snapshot WIP)

The state-promotion from GSPHERE → RING was **mid-refactor** when
the snapshot was taken:
- The direct-replace spawn block at `:2184-2211` is **commented out**
  (`/* … */`).
- The active code (`:2214-2218 + 2289-2305`) instead frees GSPHERE
  slots to USEME, then spawns the "ring" 150 particles as **SMOKEY1**
  (rising flame particles) at radius `rsize` around the pivot. So
  the `else if (sm[i].state == FFLASH_STATE_RING)` branch at `:2351`
  is **unreachable in the snapshot's active code**.
- Visual implication: in the snapshot's active flow, the "explosion
  ring" is **a circular burst of rising flame particles** (z-velocity
  +2..4, spawned at z=30 around pivot), NOT a flat-disc orbiting
  ring. The expanding `rsize` only governs where the new particles
  *spawn*, not their motion (they rise straight up from there).
- **What likely shipped:** unknown from partial recon. The cleanest
  hypothesis (consistent with the commented-out block being the
  "old" version and the active code being the "new" version) is
  that the snapshot's active SMOKEY1-burst is what shipped — but
  this is unverified. **Reconstruction visual-vetting against
  retail in-game footage will resolve this**: if the explosion
  ring reads as orbiting-disc, the commented block shipped; if it
  reads as radial-rising-flames, the active code shipped. The
  reconstruction should default to **the active code path** (radial
  burst of rising flames) per "what the snapshot says runs", and
  flip to the orbit-ring fallback only if the visual-vetting shows
  otherwise.

### 13.2 The unused `goffset`

`Initialize` computes `goffset = ConvertToVector(((255-facing) +
138·255/360) & 255, FFLASH_GS_INFRONT=10, …)` and sets `goffset.z =
5` at `:2127-2128`, then **never uses `goffset` anywhere else**.
The `(138 · 255) / 360 = 97` "angle bias" looks like a "translate
the sphere `10` wu in front of the caster" intent (which would
match the `INFRONT` / `INFRONTUP` constant names), but the active
code instead hardcodes `gsphere = (0, 0, 30)` (`:2130-2132`). So
either (a) `goffset` was the **old** way of placing the sphere and
the snapshot was mid-refactor when the dev hardcoded `(0,0,30)`
and forgot to remove the dead `goffset` calc, or (b) `goffset` is
**vestigial debug** from an earlier visualization attempt. Either
way, **the shipped pivot is `(0, 0, 30)` per the active code path**.
Reconstruction can ignore `goffset`.

### 13.3 The `256.0ff` typo

`effect_old.cpp:2457`:
```c
D3DMATRIXRotateZ(&obj->matrix, -((facing / 256.0ff * M_2PI)));
```
`256.0ff` is not valid C. The intent is clearly `256.0f` (the float
suffix). Since `effect_old.cpp` is under `#if 0`, this never
compiles. The reconstruction substitutes `256.0f`.

### 13.4 Render anchor mismatch with RefreshZBuffer

When `target != null`, the particles render at `target->GetPos() +
sm[i].pos` (via OBJ3D_ABSPOS), but `RefreshZBuffer` projects the
effect's own origin `(0,0,0)` and patches Z there. So the Z-restore
patch is at the **wrong screen position** when the target is far
from the effect's spawn point. Whether this caused visible
Z-fighting in the retail build is unclear (the patch is 200×200
px, fairly large, and might overlap both positions if they're close;
or it might miss the target entirely if they're far apart). The
reconstruction should anchor the Z-restore patch at `target->GetPos()
+ (0,0,0)` when target is non-null. **This is one of the snapshot's
WIP rough edges** — likely got cleaned up in retail.

### 13.5 No per-particle texture animation

The snapshot's Render code does not advance `textureframe[]` or
write UVs per-particle. If the I3D's texture is a multi-frame
flipbook (the header says `numframes = 8` at offset 0x6c), the
visual is using only frame 0 — leaving 7/8 of the authored frames
unused. **Did retail wire per-particle frame cycling?** Unverified.
Reconstruction default: render frame 0 only (per snapshot). If
visual-vetting shows the in-game explosion flickers / animates the
texture per-particle, the reconstruction must add per-particle
texture-frame advance.

### 13.6 CULLMODE not set by SetAddBlendState

`SetAddBlendState` (`effect_old.cpp:235-244`) does not touch
`D3DRENDERSTATE_CULLMODE` (unlike `SetBlendState` which doesn't
either, actually — neither helper sets cull mode). So the cull
mode is **inherited** from whatever was last set. For the
reconstruction, default to `CULLMODE = NONE` (the typical effect
default, so both face windings render — important for billboards/
quads that may face away from the camera). The snapshot doesn't
verify.

### 13.7 `target` capture timing

`target` is captured ONCE in `Initialize` (at frame 0) by querying
`spell->GetInvoker()->Fighting()`. If the invoker stops fighting
that opponent mid-effect (target dies, target flees, invoker
switches targets), the captured `target` pointer becomes stale.
The snapshot doesn't null-check `target` in Render — if the
TCharacter is freed, this crashes.

Two cleaner mitigations for the reconstruction:
1. Hold a TSafeRef<TCharacter> instead of a raw pointer (per
   project's safe-ref convention).
2. Capture `target->GetPos()` ONCE in `Initialize` rather than
   re-querying it every render frame — the explosion "anchored
   to the target" interpretation could mean either (a) follows the
   target's live position, or (b) sits at the position the target
   *was at* when struck. The snapshot says (a) but the snapshot
   has the safety bug; (b) is the more robust read.

### 13.8 What `T3DAnimator::Animate(draw)` base does

The snapshot's `Animate` body calls `T3DAnimator::Animate(draw)`
first (`:2166`). Whatever that base does (refresh hierarchy, pull
position from `inst`, mirror state, etc.) is shared with all other
T3DAnimator children. Not documented here — assumed the standard
T3DAnimator framework behavior is in place for the reconstruction.

### 13.9 PLAY macro / retail spawn-time audio location

The snapshot's `PLAY("FireFlash")` is in
`TFireFlashAnimator::Initialize`. Retail might have moved it to
`TFireFlashEffect::Initialize` (the effect-class init runs first).
For audio fidelity this doesn't matter (one frame difference at
spawn); record it as a minor port choice.

### 13.10 Snapshot-vs-retail risk summary

The **retail fidelity is retail-partial** (§2.1). The key things to
visually vet vs. an in-game retail capture of Fire Flash:

1. **Ring branch resolution** (§13.1) — radial rising burst vs.
   orbiting flat disc. Highest visual divergence risk.
2. **Per-particle texture animation** (§13.5) — frame-0-only vs.
   flipbook.
3. **Phase timing constants** (FFLASH_F1_*/GS_*/RI_*) — the
   100-frame total is confirmed but the internal phase boundaries
   (25 / 26 / 35) are snapshot-only.
4. **Cull mode** (§13.6).
5. **Z-restore anchor** (§13.4).

---

## 14. Reconstruction burndown

A flat checklist of every discrete element the reconstruction agent must
recreate to reproduce this effect faithfully. Order roughly by build
dependency.

```
ASSETS + REGISTRATION
- [ ] Load `Magic\fireflash.I3D` (id 0x37780ae2) via the standard imagery
      pipeline; bind sub-objects `smoke` (= GetObject(0)) and `smoke01`
      (= GetObject(1)). Do NOT substitute procedural sprites (§4)
- [ ] Register the `"fireflash"` spell builder + `"FireFlash"` animator
      so the live spell.def variants ("Fire Flash" / "Priest Fire Flash")
      resolve and reach the new code (§12)
- [ ] Bind sound asset `Sound/effects/fireflash.wav` to the audio key
      `"FireFlash"` (§11)

EFFECT CLASS (TFireFlashEffect)
- [ ] On Initialize, if the spell has an invoker and the invoker has a
      live Fighting() target, call spell->Damage(target) once (§6.1)
- [ ] Pulse: chain to base TEffect::Pulse (no extra logic)

ANIMATOR INITIALIZATION (TFireFlashAnimator::Initialize)
- [ ] Capture facing = caster's GetAngle() (byte 0..255) ONCE (§6.2)
- [ ] Set fixed pivot gsphere = (0, 0, 30); gsize = 1.0 (§3, §6.2)
- [ ] Capture target = spell->GetInvoker()->Fighting() (or nullptr).
      Prefer TSafeRef<TCharacter> for safety (§13.7)
- [ ] One-shot PLAY("FireFlash") (§11)
- [ ] Zero all 150 particle slots; seed slots 0..74 as GSPHERE state
      with random (0..2π, 0..2π, 0..2π) Euler angles + (0.12, 0.12, 0.12)
      rad/tick angular velocity (§5, §6.2)

PER-FRAME TIMING + KILL
- [ ] Per Animate call, framenum++ ; kill effect when framenum >= 100
      (RETAIL-CONFIRMED — §2.1) (§6.3, §3)
- [ ] Convert per-tick deltas to time-based rates anchored at 24 Hz so
      the effect plays identically at any frame rate (feedback-
      framerate-independent-anim) (§3 note)

PHASE 1 — RISING FLAME COLUMN (SMOKEY1, frames 0..24)
- [ ] Per-frame spawn-rate table: frames 0..4 → 4 particles/frame;
      frames 5..14 → 7/frame; frame 15 → 7/frame; frames 16..24 → 2/frame;
      frames 25+ → 0/frame (§6.3, §3)
- [ ] For each spawn: random XY in radius-15 disc at z=1; life = 20 +
      25·(15-rand(0,dist))/15 ticks (= 20..45); startfade = 20% of life;
      vel = (0, 0, 2+rand(0,2)) — pure upward, NO gravity, NO XY drift (§5, §3)
- [ ] Per-tick SMOKEY1 motion: pos.z += vel.z; life--; release when life
      hits 0 (§6.3)

PHASE 2 — GLOWING SPHERE (GSPHERE, growing 26..34)
- [ ] Frames 26..34: gsize += 2.0/tick clamped to 20.0 (§6.3, §3)
- [ ] Skip rendering GSPHERE particles before frame 26 (§6.4)
- [ ] Per-tick GSPHERE motion: per-particle angle += angvel (all three
      axes); dist = gsize; scale = gsize * 0.01; pos = pivot +
      (RotX(angle.x) · RotY(angle.y) · RotZ(angle.z)) · (0, dist, 0)
      (§5, §6.3)

PHASE 3 — EXPLOSION SNAP (frame 35)
- [ ] Free all GSPHERE → USEME; set rsize = gsize (= 20); request 150
      "ring births" (§6.3)
- [ ] **Resolve dead-RING-branch ambiguity (§13.1).** Default: spawn the
      150 "ring" particles as SMOKEY1 state (rising flame burst at radius
      rsize around the pivot, z=30, vel = (0,0,2..4)) per the snapshot's
      active code. If visual-vetting against retail shows orbiting flat
      ring instead, switch to the commented-out RING state path
- [ ] Frames > 35: rsize += 8/tick; when rsize >= 300, release all
      particles to USEME (§6.3)

RENDERING (per particle)
- [ ] Each Render frame, SaveBlendState + SetAddBlendState (Additive,
      DECALALPHA, SRC=ONE, DST=ONE, ZWRITE=off, ZTEST=on) (§7, §6.4)
- [ ] Per active particle: choose sub-object — SMOKEY1 in late-life
      (>= 70% fade) uses GetObject(0)=`smoke`, else GetObject(1)=`smoke01`;
      GSPHERE always uses GetObject(1) (skip if framenum < 26); RING (if
      reachable in your branch resolution) uses GetObject(1) (§4, §6.4)
- [ ] Per-particle scale envelope (SMOKEY1 only): in size-decay window
      scale = mainscale · life / fade_window + 0.05; otherwise scale =
      mainscale = 0.3 (§3, §6.4)
- [ ] Set obj->flags = OBJ3D_MATRIX. If target is non-null, OR in
      OBJ3D_ABSPOS and use target->GetPos() as origin; else origin = (0,0,0)
      (§5, §6.4, §13.7)
- [ ] Build the render matrix in this exact order: Identity, RotX(-2π/3 =
      -120°), RotZ(-π/4 = -45°), RotZ(-facing·2π/256 rad), Scale(particle
      scale, uniform), Translate(particle pos + origin) (§7, §6.4)
- [ ] RenderObject(obj) per active particle. RestoreBlendState at end

DEPTH RESTORE
- [ ] RefreshZBuffer: 200×200 px box centred horizontally on the
      projected origin, top-anchored. If target != null, prefer projecting
      target->GetPos() (snapshot bug §13.4); else project effect's own
      origin (§6.5)

CUSTOM / PROCEDURAL
- [ ] Three-phase state machine on a single framenum counter (§6.3)
- [ ] Per-state sub-object selection (§6.4)
- [ ] Target-locked render anchor via OBJ3D_ABSPOS (§5, §6.4)

ASSOCIATED LIGHT
- [ ] Wire the spell's LIGHT directive (COLOR 255,130,0 INT 160 MULT 20)
      via the spell system, NOT as part of the animator. Likely attached
      to the strike target for spell-cast duration (§9)

COLOR
- [ ] Authored I3D texture provides the color; do NOT tint per-vertex
      or per-material. Visually verify against in-game retail capture —
      hot orange explosion with white-hot center (§10)

AUDIO
- [ ] One-shot PLAY("FireFlash") at animator Initialize (or effect
      Initialize — minor) (§11)

SUB-EFFECTS
- [ ] None (the explosion is self-contained; no NewObject() spawns from
      the animator)
```

The definition of done: at the test rig, casting Fire Flash on a target
produces a 100-frame composite reading as (a) a tight thin rising flame
column for ~1 second, (b) a brightening glowing sphere growing over the
next ~0.4 seconds, (c) a brief explosive burst that expands outward
along the ground for ~1.5 seconds, (d) the flame column tails off, (e)
the effect releases — all in additive bright orange/yellow/white,
anchored at the target's world position, with a fixed tilt that
orients to the caster's facing.
