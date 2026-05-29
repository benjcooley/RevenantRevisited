# F07 TFireBallEffect — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | F07 (covers F07 `TFireBallEffect` + F08 `TFireBallAnimator`; also documents the shared **S08 `TMissileEffect`** base) |
| **Class(es)** | `TFireBallEffect : TMissileEffect : TEffect` + `TFireBallAnimator : T3DAnimator`; base `TMissileEffect`; helpers `TSubParticleAnimator` (spark trail), `TShockAnimator` (impact ring) |
| **Status** | forensics-complete (see §13 for the snapshot-only kinematics + the FireColumn-impact question) |
| **Retail fidelity** | **retail-partial** — `NewFireball.I3D` is **byte-identical** to the shipped `data/imagery.rvi` copy; the `TFireBallAnimator` class layout is a **PERFECT** structural match in the Ghidra decomp (44 bytes, 11 fields, ±0); registration (`"FireBall"`) + caster wiring + the `DT_BURN`/spell-LIGHT data are retail-confirmed. The numeric tuning constants (grow rate, max size, trail scale/count, spark counts, gravity, damage radius, ring colors) are **snapshot-only**. Retail also references a `FIRESHOOT`/`FIREHIT` sound pair the snapshot only stubs. See §2.1. |
| **Author / Date** | agent (worktree-agent-a22bfdf0e7eac32f7) / 2026-05-20 |
| **Family** | fire (projectile / missile sub-family) |
| **Draws** | composite — I3D mesh (flying ball + glow + trail quads) + particle emitter (photon-spark trail) + shockwave ring + associated dynamic light |
| **Archetype(s)** | **B** I3D-geo (the ball mesh, transform-animated), **D** trail (mesh-billboard ring-buffer trail), **E** simple/gravity particle emitter (spark trail), **F** composite + custom procedural logic (3-state launch/fly/explode machine, impact burst + ring), **G** associated light, **H** audio (stubbed in snapshot, present in retail) — per [knowledge/05_EFFECT_ARCHETYPES.md](knowledge/05_EFFECT_ARCHETYPES.md) |

---

## 1. Summary

The Fire Ball is the player's flagship offensive spell (talismans "AF" / priest
"MAD", `spell.def:848,850`): a warm-orange flaming ball that **grows above the
caster, launches toward the aim direction, flies ballistically, and explodes** on
hitting a character / wall / range limit. It is built on the shared
`TMissileEffect` base — the same travel + impact state machine used by Photon,
FireColumn, FlameDisc, IceStorm and the Yhagoro NPC fireball. The visual is a
composite: a single 4×4-atlas **fire-ball quad** (drawn rotated to face the user)
with an additive **glow** copy behind it, a **decaying mesh trail** of past
positions, and a gravity-driven **photon-spark particle trail** streaming off it.
On impact it spawns a 10-quad **burst**, a colored expanding **shockwave ring**,
a final spark burst, deals blast damage in a 150-wu radius, and dies once all
sub-systems are quiet. While alive the spell contributes a flickering warm point
light (`LIGHT COLOR 255,130,0 INT 255 MULT 20`, `spell.def:844`).

The `TMissileEffect` machinery documented in §5–§6 is **shared by the whole
family** and is marked **[BASE]**; fireball-specifics are marked **[FIREBALL]**.

---

## 2. Sources & evidence

- **Retail decomp:**
  - `recon/mappings/TFireBallAnimator_cls_0x41cdd0_map.yaml` — **PERFECT** confidence, full field map (44 bytes / 11 fields, ±0).
  - `recon/mappings/TFireBallEffect_cls_0x5b4290_candidate.yaml` — MEDIUM (string-XREF located; `FIRESHOOT`/`FIREHIT` event tokens).
  - `recon/mappings/TMissileEffect_cls_0x5b3c18_candidate.yaml` — MEDIUM (base class; shared `"FireBall"`/`"Photon"`/`"lightning"` XREFs land here).
  - Bodies SPARSE — Ghidra isolated structure + registration, not readable method bodies.
- **Pre-release (snapshot):** `src/missileeffect.cpp` (whole file under `#if 0`, gate `:15`/`:1702`) + `src/missileeffect.h`. `TMissileEffect` `:31-141`; `TFireBallEffect` `:482-492`; `TFireBallAnimator` `:501-1150`. Helpers `TSubParticleAnimator` / `TShockAnimator` in `src/effectcomp.{h,cpp}` (`effectcomp.cpp:335-770`).
- **Sister effects consulted:** `TPhotonEffect`/`TPhotonAnimator` (`missileeffect.cpp:147-471`) — same base, alternative launch/fly/explode spark choreography; `TFlameDiscEffect`/`TFireColumnEffect` (`missileeffect.cpp:1158-1701`) — confirm the base's shared `Initialize`/`Pulse` contract. Blood/Shock color packing per [knowledge/03_RENDERING_CONVENTIONS.md §6](knowledge/03_RENDERING_CONVENTIONS.md).
- **Asset:** `legacy/Imagery/Magic/NewFireball.I3D` (144,664 B) registered as `"FireBall"` (`Class.Def:2031`).
- **Source-of-truth ranking:** snapshot `missileeffect.cpp` is authoritative for *behavior + constants* (the only readable body). Retail decomp corroborates *structure + asset + registration + audio names* but not the numbers. Where they speak: retail wins; recorded as divergence in §2.1/§13.

### 2.1 Retail-vs-snapshot reconciliation (verdict: retail-partial)

**1. Asset identity — CONFIRMED IDENTICAL.** The shipped `data/imagery.rvi`
(a stored ZIP) member `Imagery/Magic/newfireball.i3d` is **byte-for-byte
identical** to the legacy disk copy:

```
md5  0207b59d11bf6b11c8df0ed9b23a7ee0   data/imagery.rvi:Imagery/Magic/newfireball.i3d  (144664 B, dated 1998-08-07)
md5  0207b59d11bf6b11c8df0ed9b23a7ee0   legacy/Imagery/Magic/NewFireball.I3D          (144664 B)
cmp → IDENTICAL
```

So the mesh + textures the shipped fireball consumes are exactly what the
snapshot's animator addresses (the registry points at `Magic\NewFireBall.I3D`,
`Class.Def:2031`). Strong corroboration that the *visual asset* did not change.
(Note: `Magic\fireball.i3d` 144,780 B and `Magic\testfireball.i3d` are stale dev
variants in the archive — the registry uses `NewFireBall.I3D`, not those.)

**2. Structure — CONFIRMED (animator PERFECT).**
`TFireBallAnimator_cls_0x41cdd0_map.yaml` is a 123/100 "PERFECT" match: 44 bytes,
11 fields, ±0 bytes, ±0 fields, every offset mapping cleanly to the snapshot's
declared fields (`trail`, `next_state`, `frame_count`, `glow_frame`, `spark`,
`ring`, `fireball_angle`, `explode`, `burst`, `firsttime`, `old_state` —
matching `missileeffect.h:194-205`). The animator class the shipped game ran has
the **same shape** as the snapshot's, so the snapshot's per-frame logic is the
right algorithm even though the constants inside it can't be read from Ghidra.

**3. Base class — CONFIRMED EXISTS + SHARED.** `cls_0x5b3c18` (TMissileEffect)
carries the `"FireBall"`, `"Photon"` and `"lightning"` sound-key XREFs together
(per the candidate yaml), confirming a single registered base for the whole
missile family — exactly the `TMissileEffect : TEffect` hierarchy in
`missileeffect.h:31`. `TFireBallEffect = cls_0x5b4290` inherits the same
`cls_0x5a50e8` family root.

**4. Registration + caster wiring — CONFIRMED.** `DEFINE_BUILDER("FireBall", …)`
(`missileeffect.cpp:479`) + `REGISTER_3DANIMATOR("FireBall", …)` (`:498`); the
string `"FireBall"` is XREF'd in retail (candidate yaml). The spell casts effect
name `"fireball"` (`spell.def:848,850`) with `DAMAGETYPE DT_BURN` (`:838`,
`DT_BURN = 7`, `spell.def:40`) — and the snapshot's `BlastCharactersInRange`
uses the variant's mindamage/maxdamage/damagetype (`missileeffect.cpp:681-684`),
consistent with the spell-data wiring.

**5. Audio — DIVERGENCE (retail has more).** Retail XREFs name a
**`FIRESHOOT`** (launch) + **`FIREHIT`** (impact) sound pair on the fireball
class (`TFireBallEffect_cls_0x5b4290_candidate.yaml` evidence). The snapshot only
has *commented-out* `//PLAY("fireball grow")` (`missileeffect.cpp:517`) and
`//PLAY("fireball explosion")` (`:686`). **The shipped game played fireball
launch/impact sounds the snapshot stubbed.** Record both in §11.

**6. Constants — NOT corroborated (snapshot-only).** Every tuning number in §3
(grow rate 0.05, max size 0.60, trail scale 0.85 / count 10, spark 40/30,
gravity 0.37, blast radius 150, the 4 ring ARGB stops, etc.) lives inside method
bodies the decomp did not isolate. Treat them as snapshot-only — the
reconstruction must visually match against in-game ground truth (§12) and is free
to re-tune within the verified structure.

**Verdict — retail-partial.** *What it is, what asset it draws, its class shape,
how it's registered/cast, its damage type and its scene light* are
retail-confirmed; the *numeric feel* (sizes, rates, counts, colors, gravity) and
the *exact sound names firing* are snapshot-only / partially divergent. The
reconstruction's fidelity risk is concentrated in the §3 numbers and the §11
audio.

---

## 3. Constants

`[BASE]` = lives in `TMissileEffect` (shared by Photon/FireColumn/FlameDisc/IceStorm).
`[FB]` = fireball-specific.

| name | value | units | scope | source | confirmed? |
|------|-------|-------|-------|--------|------------|
| MISSILE_SPEED (default) | 16 | pos-units/tick (pre-ROLLOVER) | [BASE] | missileeffect.h:26; set `Initialize` :37 | snapshot-only |
| MISSILE_RANGE | 2 | range multiplier | [BASE] | missileeffect.h:27 | snapshot-only |
| range (init) | 32768 | ticks (huge until launched) | [BASE] | missileeffect.cpp:35 | snapshot-only |
| range (flying) | `(240*MISSILE_RANGE)/(speed/ROLLOVER)` | ticks of flight | [BASE] | missileeffect.cpp:68 | snapshot-only |
| ROLLOVER | 65536 (`1<<16`) | sub-unit precision | [BASE] | revdefs.h:447 | snapshot-only |
| vel.z on launch | `speed / -16` | pos-units/tick (slight downward arc) | [BASE] | missileeffect.cpp:66 | snapshot-only |
| character-hit radius | 32 | wu (Distance check during FLY) | [BASE] | missileeffect.cpp:105 | snapshot-only |
| states | LAUNCH=0, FLY=1, EXPLODE=2 | enum | [BASE] | missileeffect.h:20-24 | yes (retail: I3D states `launch`/`fly`/`explode`) |
| FIREBALL_SPEED | 8 | pos-units/tick (pre-ROLLOVER) | [FB] | missileeffect.h:121; set :486 | snapshot-only |
| spawn z-lift | +50 | wu (ball starts 50 above caster pos) | [FB] | missileeffect.cpp:513-514 | snapshot-only |
| FIREBALL_GROW_RATE | 0.05 | scale/tick (FLY & EXPLODE grow) | [FB] | missileeffect.h:145 | snapshot-only |
| FIREBALL_MAX_SIZE | 0.60 | scale cap | [FB] | missileeffect.h:146 | snapshot-only |
| fireball.scale (init) | `FIREBALL_MAX_SIZE/2` = 0.30 | scale | [FB] | missileeffect.cpp:519 | snapshot-only |
| FIREBALL_MAX_FRAME | 16 | atlas frame count (`frame_count`) | [FB] | missileeffect.h:153 | snapshot-only |
| FIREBALL_GLOW_FRAME | 3 | atlas cell used for the glow + skipped by the ball | [FB] | missileeffect.h:154 | snapshot-only |
| atlas grid | 4×4 (`u=(f%4)*.25, v=(f/4)*.25`) | UV cells | [FB] | missileeffect.cpp:763-764 | snapshot-only |
| frame advance | +1/tick, wrap at frame_count, skip glow_frame | frames/tick | [FB] | missileeffect.cpp:642-650 | snapshot-only |
| rotation step | +2 (normal) / +12 (`WHITE_FADE`) %360 | deg/tick (in-plane spin) | [FB] | missileeffect.cpp:652-655 | snapshot-only |
| glow size | `1.0 + .05*random(0,15)` ⇒ ×1.0..1.75 | scale mult, re-rolled each tick (flicker) | [FB] | missileeffect.cpp:639,695,744 | snapshot-only |
| FIREBALL_TRAIL_SCALE | 0.85 | per-slot shrink down the trail | [FB] | missileeffect.h:156 | snapshot-only |
| FIREBALL_TRAIL_NUM / _SIZE | 10 (SPACEOUT off) | trail ring-buffer length | [FB] | missileeffect.h:157,169-172 | snapshot-only |
| base mesh rotation | rotX −30°, rotY +60° | deg (orient ball/trail to face user) | [FB] | missileeffect.cpp:847-848,904-905,998-999 | snapshot-only |
| FIREBALL_MAX_SPARK | 40 | trail particle cap | [FB] | missileeffect.h:159 | snapshot-only |
| FIREBALL_NORM_SPARK | 30 (`MAX_SPARK*.75`) | trail particles during normal flight | [FB] | missileeffect.h:160 | snapshot-only |
| spark spawn chance | 25 (LAUNCH) / 30 (FLY) / 100 (EXPLODE) | % per particle slot per tick | [FB] | missileeffect.cpp:582-587 | snapshot-only |
| spark scale | 0.15 (×0.90/tick decay) | scale | [FB] | missileeffect.cpp:598-599 | snapshot-only |
| spark velocity spread | ±1 all axes (FLY); ±3 xy, ±1 z (LAUNCH) | wu/tick | [FB] | missileeffect.cpp:602-610 | snapshot-only |
| spark gravity | 0.37 | wu/tick² (subtracted from vel.z) | [FB] | missileeffect.cpp:613; effectcomp.cpp:401 | snapshot-only |
| spark life | random(15,20) | ticks | [FB] | missileeffect.cpp:615-616 | snapshot-only |
| spark flicker | true, size 1.75 | scale boost when flickering | [FB] | missileeffect.cpp:618-619 | snapshot-only |
| FIREBALL_MAX_BURST | 10 | impact burst quad count | [FB] | missileeffect.h:174 | snapshot-only |
| burst pos jitter | random(−15,20) per axis | wu around impact | [FB] | missileeffect.cpp:690-692 | snapshot-only |
| burst scale (init) | `.75 + random(0,5)*.15` ⇒ 0.75..1.50 | scale | [FB] | missileeffect.cpp:693 | snapshot-only |
| burst shrink | ×0.90/tick; cull < 0.50 | scale | [FB] | missileeffect.cpp:741-743 | snapshot-only |
| blast radius | 150 | wu | [FB] | missileeffect.cpp:682 | snapshot-only |
| FIREBALL_DAMAGE_MIN/MAX | 10 / 25 (`// CHANGE THESE!`) | hp — **overridden** by spell variant | [FB] | missileeffect.h:176-177 | snapshot-only (dead: damage comes from `VariantData()`) |
| ring rings × verts | 4 rings × 24 verts | shockwave geometry | [FB] | missileeffect.cpp:538 | snapshot-only |
| ring init scale | 25 (all axes) | scale | [FB] | missileeffect.cpp:711-713 | snapshot-only |
| ring scale_factor | 1.085/tick (grow) | scale mult | [FB] | missileeffect.cpp:715-717 | snapshot-only |
| ring max_size | 120 | scale cap (then done, FADE flag) | [FB] | missileeffect.cpp:719-723 | snapshot-only |
| ring color 0 | RGBA(.62,.06,.05, 0.0) deep red, a=0 | 0–1 floats | [FB] | missileeffect.cpp:728 | snapshot-only |
| ring color 1 | RGBA(.78,.24,.06, 1.0) red-orange, a=1 | 0–1 floats | [FB] | missileeffect.cpp:729 | snapshot-only |
| ring color 2 | RGBA(.97,.61,.06, .75) orange, a=.75 | 0–1 floats | [FB] | missileeffect.cpp:730 | snapshot-only |
| ring color 3 | RGBA(.99,.83,.52, 0.0) pale-yellow, a=0 | 0–1 floats | [FB] | missileeffect.cpp:731 | snapshot-only |
| TORADIAN | π/180 | rad/deg | — | revdefs.h:25 | snapshot-only |
| TORADIANf | (undefined macro = π/180) | rad/deg | — | missileeffect.cpp:846 (see §13) | snapshot-only (pre-release artifact) |
| spell LIGHT color | (255,130,0) | 0–255 RGB | [FB] | spell.def:844 | yes (retail data file) |
| spell LIGHT INT / MULT | 255 / 20 | intensity / multiplier | [FB] | spell.def:844 | yes (retail data file) |

---

## 4. Assets

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| Fireball mesh | `legacy/Imagery/Magic/NewFireBall.I3D` | 144,664 B | the fireball quad + spark quad + ring geometry; carries the 4×4 fire atlas texture | registry name `"FireBall"` (`Class.Def:2031`), `RegisterImagery`/`LoadImagery` at class load |

**Asset identity:** byte-identical to shipped `data/imagery.rvi` (§2.1.1).

**I3D header (from the binary):** magic `CGSR`; **3 animation states** `launch`,
`fly`, `explode` (matching `MISSILE_LAUNCH/FLY/EXPLODE = 0/1/2`); **3
sub-objects** — `box01`, `box02`, `cylinder01`. The animator addresses them by
index:

| sub-object index | I3D name | role in animator | cite |
|------------------|----------|------------------|------|
| `GetObject(0)` | `box01` | the **fire-ball quad** — drawn for ball, glow, trail and burst (the same quad re-positioned/re-framed each draw); UV picks one of 16 atlas cells via `SetAnimFrame` | missileeffect.cpp:507,798,879,980,1027 |
| `GetObject(1)` | `box02` | the **spark quad** handed to the `TSubParticleAnimator` (`spark.Init`) | missileeffect.cpp:533-534 |
| `GetObject(2)` | `cylinder01` | the **shockwave ring** geometry handed to `TShockAnimator` (`ring.Init(…,4,24)`) | missileeffect.cpp:537-538 |

The fire texture is an internal **4×4 atlas** (16 cells); transparency is the
black-key convention common to effect textures (chroma-key → premultiplied,
[knowledge/02_ASSETS_IMAGERY.md §4](knowledge/02_ASSETS_IMAGERY.md)). No external
texture filenames — surfaces are embedded in the `.I3D`.

No procedural-only path: the visual identity is the loaded mesh + its atlas. Do
**not** substitute a procedural ball/glow ([feedback-no-standins]).

---

## 5. Spawn & emit

### 5.1 [BASE] How a missile enters the world & launches

- **Trigger semantics:** one-shot spell cast → 3-state machine (LAUNCH → FLY →
  EXPLODE → die). Continuous spark/trail emission while alive.
- **Spawn:** `TSpell::Timer` builds the effect at the caster's hand/source pos +
  the variant `HEIGHT` (50, `spell.def:848`), facing the caster's aim
  ([knowledge/01_ARCHITECTURE.md §6.2](knowledge/01_ARCHITECTURE.md)). `SetSpell`
  wires the spell back so the effect can read invoker/target/damage.
- **[FB] z-lift:** at `Initialize` the ball nudges its own world pos **+50 in z**
  (`missileeffect.cpp:513-514`) so it grows above the caster's hand.
- **Launch handshake:** the animator drives the LAUNCH→FLY transition. During
  LAUNCH the animator calls `((PTMissileEffect)inst)->SetStatus(true)`
  (`missileeffect.cpp:662`); the base `Pulse` sees `status`, converts the aim to
  a velocity (`ConvertToVector(GetAngle(), speed, vel)`, `:64`), clears
  `OF_IMMOBILE`, sets `OF_MOVING|OF_WEIGHTLESS`, applies the slight downward
  `vel.z = speed/-16` (`:66`), computes the flight `range` (`:68`) and switches
  to FLY (`:70`).
- **Aim direction:** `TEffect::GetAngle()` resolves the caster→target byte-angle
  (0–255). `fireball_angle` is cached at animator construction
  (`missileeffect.h:221`).
- **Coordinate space:** the effect object moves in **world space** (the engine
  mover integrates `vel`); the ball/trail/spark geometry is authored in
  **local space relative to the effect's world pos** (`effect_pos`), then
  positioned per draw.

### 5.2 [FB] Spark-trail emit (the photon-spark particles)

Each tick the animator fills an `SSubParticleParams` and calls `spark.Set` +
`spark.Animate` (`missileeffect.cpp:573-624`). The sub-particle system
(`TSubParticleAnimator::Create`, `effectcomp.cpp:455-524`) spawns up to
`params.particles` total, each tick rolling `random(1,100) ≤ chance` per empty
slot:

- **anchor:** `params.pos = effect_pos` (the ball's current world pos),
  `pos_spread = 0` → sparks are born **at the ball**.
- **count:** target = 30 (FLY) / 40 (EXPLODE) / 30 (LAUNCH); cap 40.
- **velocity:** base 0 + `random(±spread)` per axis → isotropic jitter ±1 (FLY),
  ±3 xy ±1 z (LAUNCH). With `velocity_dir = 0` the sign is symmetric.
- **scale:** 0.15, decaying ×0.90/tick; **gravity 0.37** pulls vel.z down each
  tick → sparks arc and fall behind the ball, forming the streaming trail.
- **life:** random(15,20) ticks; **flicker** on (size ×1.75 on flicker ticks).

### 5.3 Spatial diagram

```
   wz (up)
   │                       ScreenAligned-ish fire quad (rotX-30°, rotY+60°,
   │            ☼ glow      rotZ = spin + facing) — same box01 quad drawn:
   │           (◯)            • ball   (frame N of 4×4 atlas)
   │      · ·  (●)  ← ball     • glow   (frame 3, scaled ×glow, behind)
   │     ·  · ·  ╲             • trail  (10 decaying copies, ×0.85 each slot)
   │    · spark   ╲ trail      • burst  (impact only, 10 copies)
   │   ·  trail    ●  (older, smaller, 0.85^n)
   │  ·  (gravity 0.37 → sparks fall + arc behind)
   └──────────────────────────── wx     effect travels along aim vector
  ╱                                      emit anchor = effect world pos (+50 z at spawn)
 wy

 IMPACT (EXPLODE):  burst 10 quads scattered ±(−15..20) wu
                    + shockwave ring (cylinder01, flat-ish, grows 25→120 ×1.085)
                    + 40-spark burst + blast damage r=150
```

---

## 6. Behavior & per-frame logic

### 6.1 [BASE] `TMissileEffect::Pulse` — the shared travel/impact machine
(`missileeffect.cpp:52-135`) — **this is the family-shared section.**

```
on each sim tick:
  bits = Move()                       // engine integrates vel; returns move flags
  switch state:
    LAUNCH:
      if status:                      // animator finished its launch flourish
        if speed:
          vel = ConvertToVector(GetAngle(), speed)   // aim → velocity
          flags = (~OF_IMMOBILE) | OF_MOVING | OF_WEIGHTLESS
          vel.z = speed / -16                          // slight downward arc
          range = (240*MISSILE_RANGE) / (speed/ROLLOVER)
        state = FLY
    FLY:
      range--;  if range<=0: explode = true            // ran out of range
      if bits & MOVE_BLOCKED: explode = true           // hit wall/ground
      else for each CHARACTER chr (TMapIterator):
        skip invoker / dead / Distance>32 / non-enemy
        else: explode = true                           // hit a foe within 32 wu
      if explode:
        flags = (~OF_MOVING & ~OF_WEIGHTLESS) | OF_IMMOBILE   // stop dead
        state = EXPLODE
    EXPLODE:
      if !HasAnimator(): KillThisEffect()              // no visual left → reap
  TEffect::Pulse()                                     // base resets frame 0
```

`OffScreen()` (`:137-141`) is overridden so a missile only goes off-screen once
it's *exploding and done* — keeps the flying ball simulating even off-screen.
Subclasses override `Initialize` to set their speed (FIREBALL_SPEED=8) and
`Pulse` to chain `TMissileEffect::Pulse()` (fireball's `Pulse` is a pure pass-through, `:489-492`).

### 6.2 [FB] `TFireBallAnimator::Animate` — per-frame visual + impact
(`missileeffect.cpp:549-759`)

```
on each render frame:
  T3DAnimator::Animate(draw)                    // refresh bone/pos
  inst->SetCommandDone(false); SetStatus(false)
  firsttime = (old_state != state)              // state-entry edge
  // explode edge tracker: 0=pre, 1=first explode frame, -1=after
  if explode==1: explode=-1
  elif explode==0 and state==EXPLODE: explode=1

  // --- spark trail params (see §5.2) ---
  params.particles = (explode==-1?0 : explode==1?40 : 30)
  params.chance    = (LAUNCH?25 : FLY?30 : 100)
  params.pos = effect_pos ; scale .15 dec .90 ; gravity .37 ; life 15..20 ; flicker 1.75
  spark.Set(&params); spark.Animate()

  // --- mesh trail ring-buffer (shift older, shrink) ---  (D archetype)
  for i = TRAIL_SIZE-1 .. 1:
      trail[i] = trail[i-1]; trail[i].scale *= 0.85
  trail[0] = fireball; trail[0].scale *= 0.85
  trail[0].pos += effect_pos                    // record world pos of head

  // --- ball self-animation ---
  fireball.glow  = 1.0 + .05*random(0,15)       // flicker glow size (×1..1.75)
  fireball.frame += 1 (wrap frame_count, skip glow_frame)
  fireball.rotation = (rotation + 2) % 360       // in-plane spin

  switch state:
    LAUNCH:  SetStatus(true)                      // tell base to launch (§5.1)
    FLY:     if scale<MAX_SIZE: scale += 0.05     // grow to full size mid-flight
    EXPLODE:
      if scale<MAX_SIZE: scale += 0.05
      if firsttime:                               // ←— impact, fires ONCE
        pos = fireball.pos + effect_pos
        BlastCharactersInRange(invoker, pos, 150, mindmg, maxdmg, damagetype)
        for i in 0..9: burst[i] = { used, pos+jitter(−15..20),
                                    scale .75+rand*.15, frame rand, glow flicker }
        ring.Set({pos, scale 25, factor 1.085, max 120, FADE})
        ring.SetRingColor(0..3, <4 ARGB stops>)   // §10
      else:
        ring.Animate()
        for each used burst[i]: scale*=0.90 (cull<0.50); glow flicker; ++frame
      if !IsTrail() and !spark.GetCount() and no live burst and ring.IsDone():
        KillThisEffect()                          // all sub-systems quiet → die
  old_state = state
```

**`IsTrail()`** (`:781-791`) returns true while the trail head ≠ tail position
(i.e. the ball still moved recently) — used as a "trail still draining" gate for
death.

**Impact is entirely self-contained in the snapshot:** blast damage + burst +
ring + final sparks. There is **no `NewObject("FireColumn")` spawn** in the
fireball code (verified: no `FindObjType`/FireColumn spawn in
`missileeffect.cpp`; no `FIRECOLUMN` spell variant in `spell.def`). See §13 re:
the catalog's "impact spawns TFireColumnEffect" note.

### 6.3 Helper sub-systems (shared building blocks)

**`TSubParticleAnimator` (the spark)** — `effectcomp.cpp:361-603`. `Animate`
spawns to fill `params.particles`, then per particle: flicker roll, `life--`
(cull <0), `scale *= scale_dec`, `pos += velocity`, `velocity.z -= gravity`.
`Render` (`:526-570`) draws each as the `box02` quad with a fixed face rotation
(rotZ −60°, rotX −45°) at absolute world pos, scaled (×flicker_size on flicker
ticks). This is the **simple/gravity emitter** archetype (E).

**`TShockAnimator` (the ring)** — `effectcomp.cpp:609-770`. `Set` records init
scale + grow flag; `Animate` multiplies scale by `scale_factor` (1.085) each tick
until `> max_size` (120) → `done` (or shrink, if `SHOCKWAVE_FLAG_SHRINK`).
`Render` draws the `cylinder01` ring geometry with per-ring vertex color from the
4 ARGB stops, fading **alpha** by `(max−scale)/(max−init)` under
`SHOCKWAVE_FLAG_FADE` (`:732-748`). It forces `CULLMODE=NONE` +
`SRCALPHA/INVSRCALPHA` Alpha blend, then restores (`:756-769`). This is a
world-oriented expanding ring (archetype C, alpha-blended).

### 6.4 Temporal diagram

```
state:   LAUNCH        │ FLY (travel)            │ EXPLODE                │ dead
         grow above    │ grow→0.60, spin, trail, │ blast r=150 + 10 burst │
         caster, spark │ 30 sparks/tick @30%     │ + ring 25→120 ×1.085   │
trigger: status→base   │ range-- / hit char(32wu)│ firsttime impact, then │ all subsys
         launches      │ / MOVE_BLOCKED          │ shrink burst, fade ring│ quiet→kill

ball.scale  0.30 ──────► 0.60 (── +0.05/tick clamp ──) ──────────────────►
ring.scale                                       25 ──×1.085──► 120 (done)
burst.scale                                      0.75..1.50 ──×0.90──► <0.50 cull
glow flicker  ×1.0..1.75 re-rolled every tick (warm flicker) throughout
```

---

## 7. Rendering (original render state + geometry)

- **What it draws:** composite — `box01` quad (4 lit-verts) reused as **ball**,
  **glow**, **trail copies**, **burst copies**; `box02` spark quads; `cylinder01`
  ring. All via `RenderObject(obj)` after building `obj->matrix`/flags.
- **Blend mode (ball/glow/trail/burst/spark):** the animator brackets its whole
  `Render` with `SaveBlendState(); SetBlendState(); … RestoreBlendState()`
  (`missileeffect.cpp:1062-1082`). **`SetBlendState()` = Alpha (modulated):**
  `TEXTUREMAPBLEND = D3DTBLEND_MODULATE`, `SRCBLEND = SRCALPHA`,
  `DESTBLEND = INVSRCALPHA`, ZWRITE off, ZTEST on
  ([knowledge/03_RENDERING_CONVENTIONS.md §1.1](knowledge/03_RENDERING_CONVENTIONS.md);
  `effect_old.cpp:221-233`). **Not** the additive helper — trace confirms
  straight Alpha with vertex-color tint. The black-keyed texture supplies the
  transparency; the modulate stage lets vertex diffuse tint the fire.
- **Blend mode (ring):** `TShockAnimator::Render` sets its **own** explicit
  states — `CULLMODE=NONE`, `SRCBLEND=SRCALPHA`, `DESTBLEND=INVSRCALPHA`,
  `ALPHABLENDENABLE=true` (`effectcomp.cpp:756-759`) → **Alpha** as well (with
  per-vertex ARGB from the ring stops), restored afterward.
- **Lit vs self-lit:** **self-lit / Unlit** — the fire reads as a literal warm
  glow; color comes from the atlas texel × vertex diffuse, not scene lights. (No
  ambient sampling in the body.)
- **Depth / Z:** TestNoWrite (ZWRITE off, ZTEST on). `RefreshZBuffer`
  (`missileeffect.cpp:1087-1150`) repairs scene Z under the ball, each trail
  entry, each burst, the ring (`ring.RefreshZBuffer(6,3)`) and the sparks
  (`spark.RefreshZBuffer(32,32)`) by projecting world→screen and
  `RestoreZ(x,y,w,h)` over each footprint.
- **Orientation / per-object transform:** the ball/trail/burst quads build a full
  `obj->matrix` (`OBJ3D_MATRIX`): `D3DMATRIXClear` →
  `RotateZ(−rotation·TORADIANf)` (spin) → `RotateX(−30°)` → `RotateY(+60°)` →
  `RotateZ(−(facing/256)·2π)` (face the user) → `Scale` → `Translate`
  (`missileeffect.cpp:844-861, 946-965, 995-1013`). The glow uses the same minus
  the per-instance spin (`:1037-1051`). So the ball is a **user-facing,
  in-plane-spinning** billboard tipped −30°/+60° — neither pure ScreenAligned nor
  pure WorldXY; treat as a fixed-tilt user-facing quad (see §13 re: the `f`/`GetFace`
  snapshot quirk).
- **Per-quad UV (atlas):** `SetAnimFrame(frame, obj)` writes the 4 corner UVs of
  the chosen 4×4 cell into `obj->lverts[0..3].tu/tv` with `OBJ3D_VERTS`
  (`missileeffect.cpp:761-779`).
- **Per-vertex color (ring):** `obj->lverts[k].color = D3DRGBA(r,g,b,a)` per ring
  stop, alpha scaled by the fade factor (`effectcomp.cpp:737-752`). The ball/glow
  themselves don't repaint vertex color each frame (texture-driven).
- **Draw order in `Render`:** spark first (`spark.Render()`), then per state:
  LAUNCH/FLY → glow, trail, ball; EXPLODE → trail, burst, ring
  (`missileeffect.cpp:1065-1080`).

---

## 8. Texture animation

**Mechanism: UV atlas-cell pick (flipbook via UV sub-rect), NOT framehtexs, NOT
UV scroll.** `SetAnimFrame` recomputes the four corner UVs from a frame index
into a **4×4 grid** (`u=(f%4)*.25`, `v=(f/4)*.25`, cell size 0.25)
(`missileeffect.cpp:761-779`). The frame index advances +1/tick, wraps at 16, and
**skips cell 3** (`glow_frame`) which is reserved for the glow draw
(`:642-650`). The trail and burst copies pick their own (stale / random) frame
index. The glow always uses cell 3 (`SetAnimFrame(glow_frame, …)`).

No `tu/tv` scrolling and no `SetTextureFrame` handle-swap — UVs **snap** to
discrete cells (distinguishing it from the UV-scroll family, per
[knowledge/03 §8.6](knowledge/03_RENDERING_CONVENTIONS.md)).

---

## 9. Associated light

**The fireball lights the scene — a warm point light carried by the spell while
it's alive.** It is data-driven, not in the animator body:

- **Source:** `spell.def:844` — `LIGHT COLOR 255, 130, 0 INT 255 MULT 20` on the
  `"Fireball"` SPELL block.
- **Color:** **(255, 130, 0)** = warm orange (0–255). Matches the texture's warm
  fire and the ring's orange stops.
- **Intensity / range:** `INT 255` (max intensity), `MULT 20` (range/falloff
  multiplier — the standard fire-spell value; cf. Fire Flash `INT 160 MULT 20`
  `spell.def:74`, MeteorStorm `INT 180 MULT 20` `:259`). **retail-confirmed** (data file).
- **Attachment:** the light tracks the spell/effect position, so it **follows the
  flying ball** and flashes at the impact point (the spell owns it for the cast's
  lifetime).
- **Animation:** the spell light itself is steady; the *visible* flicker comes
  from the ball's `glow` size jitter (×1.0..1.75 re-rolled every tick, §6.2) and
  the spark trail. No per-frame light-color math in the snapshot.

> **NOTE — light wiring is not in the current snapshot parser.** `spell.cpp` does
> not yet parse the `LIGHT` directive (`spell.cpp:123` only mentions it as a
> known single-line keyword). The retail game consumed it. The value is
> authoritative from the data file; the consumer is a §13 gap. Other fire spells
> in `spell.def` carry the same directive (`INT … MULT 20`), confirming the
> family convention.

---

## 10. Color

- **Ball / glow / trail / burst:** color comes from the **NewFireBall.I3D fire
  atlas texture** (warm orange→yellow flame cells), modulated by vertex diffuse
  under the MODULATE/Alpha blend (§7). Texture-driven; no hardcoded vertex color
  repaint for these.
- **Shockwave ring (4 ARGB stops, `missileeffect.cpp:728-731`):** a warm fire
  gradient across the 4 rings:
  | ring | RGB (0–1) | RGB (0–255) | alpha | reads as |
  |------|-----------|-------------|-------|----------|
  | 0 (inner) | (.62,.06,.05) | (158,15,13) | 0.00 | deep red, transparent edge |
  | 1 | (.78,.24,.06) | (199,61,15) | 1.00 | red-orange, opaque |
  | 2 | (.97,.61,.06) | (247,156,15) | 0.75 | bright orange |
  | 3 (outer) | (.99,.83,.52) | (252,212,133) | 0.00 | pale yellow, transparent edge |
  Alpha fades globally as the ring grows (`SHOCKWAVE_FLAG_FADE`, §6.3).
- **Spell light:** (255,130,0) warm orange (§9).
- **Expected visual:** **richly saturated warm orange→yellow fire** — ball, trail
  sparks, burst and ring all warm; the scene lit warm orange. Pale / grey / white
  fire = port broken (stand-in texture, wrong blend, missing chroma-key, or the
  scene light not wired) — see [feedback-vfx-color-health-signal].
- **Normalization:** none — the texture and the literal ring colors carry the hue
  directly.

---

## 11. Audio coupling

- **[FB] launch + impact sounds — RETAIL has them, snapshot stubs them.** Retail
  XREFs name **`FIRESHOOT`** (launch) and **`FIREHIT`** (impact)
  (`TFireBallEffect_cls_0x5b4290_candidate.yaml`). The snapshot only has
  commented `//PLAY("fireball grow")` (`missileeffect.cpp:517`) and
  `//PLAY("fireball explosion")` (`:686`). The audio phase should wire
  `FIRESHOOT` on launch (LAUNCH→FLY) and `FIREHIT` on impact (EXPLODE entry).
- **[BASE] family sound key:** the missile family mounts `"lightning"`
  (`LIGHTNING_SOUND`, `effect.h:479`) — used by Photon/FlameDisc on FLY
  (`missileeffect.cpp:156-168, 1468-1481`), commented out for FireColumn
  (`:1167,1183`). Fireball does **not** mount it.
- The `.I3D` may also carry tag-driven sounds per (state,frame)
  ([knowledge/02 §8](knowledge/02_ASSETS_IMAGERY.md)) — not enumerated here.

---

## 12. Triggers & in-game appearance

- **Spawned by:** the **Fire Ball** spell — `VARIANT "Fireball", … "fireball", …`
  (`spell.def:848`) and **Priest Fireball** (`:850`). Effect name `"fireball"` →
  `DEFINE_BUILDER("FireBall", TFireBallEffect)` (`missileeffect.cpp:479`) +
  `REGISTER_3DANIMATOR("FireBall", TFireBallAnimator)` (`:498`). Damage type
  `DT_BURN` (`spell.def:838`).
- **Where to see it:** cast Fire Ball at any enemy in a corridor (Dungeon BG
  exposes the warm floor flicker + the impact ring on the ground). Fire it at a
  wall to trigger `MOVE_BLOCKED` impact; fire into open range to see the
  range-limit explosion. The Yhagoro NPC variant (`YFireBall`, `spell.def:1839`,
  X19) is the same family if a fireball-casting NPC is available.
- **Not vestigial** — it's a core player offensive spell with live callers.

---

## 13. Gaps & uncertainties

1. **All §3 tuning constants are snapshot-only.** Sizes (max 0.60, grow 0.05),
   trail (scale 0.85, count 10), spark counts (40/30) + gravity 0.37, blast
   radius 150, the 4 ring ARGB stops, ring scale 25→120 ×1.085 — none readable in
   the Ghidra decomp. The structure is PERFECT-matched (§2.1.2) but the numbers
   are not. **Reconstruction must visually match against in-game ground truth
   (§12)** and may re-tune within the verified algorithm.
2. **Audio divergence.** Retail fires `FIRESHOOT`/`FIREHIT`; the snapshot stubs
   them. Exact sample names beyond those tokens are unconfirmed (§11).
3. **Catalog says "impact spawns TFireColumnEffect (F09)" — NOT in the
   snapshot.** The snapshot fireball's impact is fully self-contained (burst +
   ring + sparks + `BlastCharactersInRange`); there is no `NewObject("FireColumn")`
   and no `FIRECOLUMN` spell variant. Either (a) the catalog conflated the
   in-mesh `explode` state with a separate effect, or (b) retail re-wired impact
   to spawn a FireColumn (the retail FireBall class shares `cls_0x5b4290` with
   `FIRECOLUMN`/`FIRESHOOT`/`FIREHIT` tokens — see candidate yaml). **Treat the
   self-contained explosion as the source-of-truth visual; flag the FireColumn
   spawn as an open retail question** to resolve by capturing a retail impact.
4. **`TORADIANf` macro is undefined** in the snapshot (used at
   `missileeffect.cpp:846` etc.). Semantically it is π/180 (degrees→radians) like
   `TORADIAN`; the `#if 0` body does not compile as-is. Treat all `* TORADIANf`
   as degrees→radians.
5. **`inst->GetFace(f * 360)` is a pre-release artifact** — `GetFace()` takes no
   argument (`object.h:976`) and `f` is undefined. Intent (per
   [knowledge/03 §5.2](knowledge/03_RENDERING_CONVENTIONS.md)) is the
   facing→in-plane spin `rotZ = −(facing/256)·2π`. Use that.
6. **Quad orientation** is a fixed −30°/+60° tilt + facing spin, not a clean
   ScreenAligned or WorldXY (§7). Verify the on-screen look against retail; the
   intent is "fire ball faces the camera-ish while spinning."
7. **Spell `LIGHT` consumer not wired in the current snapshot parser** (§9) —
   the value is authoritative from `spell.def`; the runtime hookup is a separate
   (non-VFX) port task.
8. **Damage source:** `FIREBALL_DAMAGE_MIN/MAX` (10/25) in the header are dead —
   actual damage is `spell->VariantData()->mindamage/maxdamage` (200/246 for the
   basic variant, `spell.def:848`). Use the spell variant, not the header macro.

---

## 14. Reconstruction burndown

```
[BASE — shared by Photon / FireColumn / FlameDisc / IceStorm / YFireBall]
- [ ] TMissileEffect 3-state machine: LAUNCH→FLY→EXPLODE→die (§6.1)
- [ ] launch handshake: animator SetStatus(true) → base converts aim→vel,
      OF_MOVING|OF_WEIGHTLESS, vel.z = speed/-16, range=(240*2)/(speed/ROLLOVER) (§5.1)
- [ ] FLY impact detection: range countdown, MOVE_BLOCKED, char-within-32wu enemy check (§6.1)
- [ ] OffScreen override: keep simulating until exploding-and-done (§6.1)

[FIREBALL]
- [ ] load NewFireBall.I3D; address box01 (ball/glow/trail/burst), box02 (spark), cylinder01 (ring) (§4)
- [ ] spawn z-lift +50; grow above caster during LAUNCH (§5.1, §6.2)
- [ ] ball geometry: box01 quad, 4×4 atlas UV pick via SetAnimFrame, fixed −30°/+60° tilt + facing spin (§7, §8)
- [ ] ball self-anim: grow 0.30→0.60 @ +0.05/tick (FLY), frame +1/tick skip cell 3, rotation +2°/tick (§6.2)
- [ ] glow draw: box01 cell 3, scale ×glow (glow flickers ×1.0..1.75/tick), behind ball (§6.2, §7)
- [ ] mesh trail: 10-slot ring buffer, shift+×0.85/slot, draw each as box01 (D archetype) (§6.2, §7)
- [ ] spark trail (TSubParticleAnimator/box02): chance 25/30/100%, target 30/40, scale .15 dec .90,
      gravity .37, life 15..20, flicker ×1.75, isotropic ±1 (FLY) velocity (§5.2, §6.3)
- [ ] blend: Alpha (SetBlendState — MODULATE + SRCALPHA/INVSRCALPHA), self-lit, ZTest no-write (§7)
- [ ] RefreshZBuffer for ball/trail/burst/spark/ring footprints (§7)
- [ ] IMPACT (firsttime, EXPLODE): BlastCharactersInRange(r=150, spell variant damage, DT_BURN) (§6.2)
- [ ] IMPACT burst: 10 box01 quads, pos jitter −15..20, scale .75..1.50 ×0.90/tick cull<0.50 (§6.2)
- [ ] IMPACT shockwave ring (TShockAnimator/cylinder01): 4 rings × 24 verts, scale 25→120 ×1.085,
      4 warm ARGB color stops, alpha fade (SHOCKWAVE_FLAG_FADE), CULLMODE=NONE Alpha (§6.3, §10)
- [ ] death gate: !IsTrail() && spark count 0 && no live burst && ring done → KillThisEffect (§6.2)
- [ ] associated light: spell point light (255,130,0) INT 255 MULT 20, follows ball, flashes on impact (§9)
- [ ] color: warm orange/yellow fire from atlas + 4 ring stops + warm light — not pale/grey (§10)
- [ ] audio (audio phase): FIRESHOOT on launch, FIREHIT on impact (retail; snapshot stubs) (§11)
- [ ] OPEN: confirm whether retail impact also spawns TFireColumnEffect (§13.3) — none in snapshot
```
