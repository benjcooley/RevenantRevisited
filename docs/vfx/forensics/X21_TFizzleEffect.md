# X21 TFizzleEffect — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | X21 |
| **Class(es)** | `TFizzleEffect` (effect shell) + `TFizzleAnimator` (animator — all logic). Registered name `"Fizzle"`. |
| **Status** | forensics-complete (see §13 for the genuine unknowns) |
| **Retail fidelity** | **retail-partial** — registration (effect + animator both under `"Fizzle"`) and the spell-fail trigger wiring are confirmed in the retail decomp, and the shipped asset's visual payload (3 colored dust sprites + geometry) is byte-identical to the snapshot; but the animator body (the `DUST_*` constants + per-tick logic) is **not** in the retail decomp and could not be corroborated — those are snapshot-only. The shipped `.I3D` container was also re-saved (header v1→v2, 9980 B → 8364 B) with the texture/geometry payload unchanged. See §2.1. |
| **Author / Date** | vfx-forensics-agent / 2026-05-20 |
| **Family** | magic |
| **Draws** | particle emitter — N small textured billboard quads (3 colored "dust" sub-objects of `Magic\Fizzle.I3D`), one quad per live particle |
| **Archetype(s)** | per [knowledge/05_EFFECT_ARCHETYPES.md](knowledge/05_EFFECT_ARCHETYPES.md): **(E) particle emitter** — a short fixed-duration *burst* (emits over 15 ticks, then plays out). With **(F2) custom transform animation** (each particle grows-then-shrinks via a 2-state scale machine and spins in-plane). No texture animation, no associated light, no sub-effects. |

---

## 1. Summary

`"Fizzle"` is the spell-failure puff: when the player tries to cast a spell that
fails, the game casts the built-in `"Fizzle"` spell, which spawns a small, brief
cloud of colored "dust" particles at the caster. Mechanically it is a particle
burst — `TFizzleAnimator` runs three small particle systems (the programmer
named them `blue`, `red`, `purple`), seeds ~22 particles total over the first 15
ticks, and each particle is a tiny textured quad that **scales up from zero,
holds, then scales back to zero** while spinning, then dies; when the last one is
gone the effect kills itself. Each of the three systems draws a different
authored sprite — a deep-blue, a violet/purple, and a magenta dust puff
(`Magic\Fizzle.I3D`, 3 sub-objects) — so the cloud reads as a mixed
blue/purple/magenta sparkle-dust burst. It is drawn translucent (Alpha blend),
lies on the world ground plane (WorldXY), and is self-lit (color is the authored
sprite, no scene light).

---

## 2. Sources & evidence

- **Retail decomp:** `recon/classes/cls_0x5ad758.cpp` — **SPARSE** (ctor/dtor/
  cleanup only; no Animate/Render bodies). Mapped LOW-MEDIUM to `"Fizzle"` in
  `recon/mappings/TFizzleEffect_cls_0x5ad758_candidate.yaml`. Its ctor allocates
  0x110 (272) B and builds two list/array sub-objects (`cls_0x41c7f0`, a 24-byte
  container, `cls_0x5ad758.cpp:71-79`) — this matches the **`TFizzleEffect`**
  effect shell, **not** the animator (the animator owns the 3 `TParticleSystem`s
  and the DUST math, which this body does not contain). The animator body is not
  extracted in recon (see §2.1, §13.1).
- **Pre-release (snapshot — authoritative for behavior):** `src/effect_old.cpp`
  - Effect `TFizzleEffect`: decl `:12280-12291`, builder `DEFINE_BUILDER("Fizzle",
    TFizzleEffect)` `:12329`, `REGISTER_BUILDER` `:12330`, `Initialize()` (empty)
    `:12332-12334`, `Pulse()` (base passthrough) `:12336-12339`.
  - Constants: `#define DUST_*` `:12293-12303`.
  - Animator `TFizzleAnimator`: decl `:12306-12323`, `REGISTER_3DANIMATOR("Fizzle",
    TFizzleAnimator)` `:12345`, `Initialize` `:12347-12359`, `Animate`
    `:12361-12487`, `Render` `:12489-12502`, `RefreshZBuffer` `:12504-12516`.
  - Generic particle helper `TParticleSystem`: struct `SParticleSystemInfo`
    `src/effectcomp.h:308-325`, class `:327-354`; impl `Init`
    `src/effectcomp.cpp:1031-1039`, `Animate` `:1041-1068`, `Render` `:1070-1109`,
    `RefreshZBuffer` `:1111-1131`, `Add` `:1133-1158`.
- **Trigger (snapshot):** `TPlayer::InvokeQuickSpell` `src/player.cpp:547-574`
  (three `CastByName("Fizzle")` paths); `TSpellManager::CastByName`
  `src/spell.cpp:492-512`; spell def `legacy/spell.def:253-263`
  (`SPELL "Fizzle"` → `VARIANT "Fizzle", … "fizzle" …`).
- **Asset:** `Magic\Fizzle.I3D` registered `legacy/Class.Def:2077`
  (`"Fizzle" "Magic\Fizzle.I3D" 0xab8800dd`); file at
  `legacy/Imagery/Magic/Fizzle.I3D` (9,980 B).
- **Blend helpers:** `SetBlendState` `src/effect_old.cpp:221-233`;
  `SaveBlendState`/`RestoreBlendState` `:181-210`.
- **Source-of-truth ranking:** pre-release `effect_old.cpp` + `effectcomp.cpp`
  are authoritative for all behavior (retail animator body unavailable). Retail
  decomp + shipped asset corroborate registration, trigger, and the visual
  payload (§2.1).

### 2.1 Retail-vs-snapshot reconciliation (REQUIRED)

`src/effect_old.cpp` is a pre-release development snapshot. Verdict:
**retail-partial.** The registration, trigger wiring, and the asset's *visual
payload* are confirmed against retail; the animator's per-tick constants/logic
are snapshot-only because the retail animator body was not extracted.

**(1) Constant grep — could NOT corroborate the DUST constants.** The animator's
distinctive float immediates were searched across the whole retail decomp:
`DUST_SCL_INC = .05f` (`0x3d4ccccd`) and `DUST_SCL_DEC = .02f` (`0x3ca3d70a`)
are **absent** from `recon/classes/`; `DUST_ADD = 1.5f` (`0x3fc00000`) appears
only in unrelated classes (`cls_0x5a393c`, `cls_0x41c7d0`). This is consistent
with the animator's translation unit simply not being in the extracted set (the
sparse `cls_0x5ad758` body is the effect shell, not the animator) — it is **not**
positive evidence of a change, but it means the `DUST_*` values in §3 are
**snapshot-only / unverified against shipped retail**.

**(2) Asset identity — payload IDENTICAL, container re-saved.** The shipped
`Imagery/Magic/fizzle.i3d` member of `data/imagery.rvi` (a PK/ZIP archive) is
**8,364 B, dated 1998-12-17**, vs the snapshot `legacy/Imagery/Magic/Fizzle.I3D`
**9,980 B** — different size, different md5, so the file was re-saved before ship.
But the *contents that matter are unchanged*: both are `CGSR`/`I3D_3DIMAGEBODY2`
meshes with **3 sub-objects `box01`/`box02`/`box03`, 3 materials, 3 textures, 12
verts, 6 faces**; all three textures are **32×32, 16-bit ARGB4444, single frame**;
and the decoded texture pixels are **byte-identical** between snapshot and retail
(dominant RGB blue `(17,34,136)`, purple `(85,17,136)`, magenta `(136,17,119)`;
163/1024 fully-transparent edge pixels in each). The size delta is the header
preamble (body offset 0x68 snapshot vs 0x100 retail) + version field (v1→v2). So
the **effect's visual identity shipped intact** — the geometry/sprite consumer is
very likely unchanged.

**(3) Structure / registration table — registration corroborated.** The retail
binary holds a builder function-pointer table; the consecutive entries
`005c541c → 004f3ef0` and `005c5420 → 004f3f70` (`recon/classes/_data.txt:56060-
56062`) **both** reference the `"Fizzle"` string at `005e121c`
(`_data.txt:107594`, XREF `004f3ef0`, `004f3f70`). The pair = the
`DEFINE_BUILDER("Fizzle", TFizzleEffect)` effect builder **and** the
`REGISTER_3DANIMATOR("Fizzle", TFizzleAnimator)` animator builder, matching the
snapshot's two registrations (`effect_old.cpp:12329`, `:12345`). The sparse
effect-shell body `cls_0x5ad758` (272 B, builds two `cls_0x41c7f0` containers)
corroborates the *effect* class exists; the animator class layout is **not**
extracted, so its 3-particle-system structure is uncorroborated.

**(4) Registration + naming + trigger — confirmed.** The spell-fail trigger is
`TPlayer::InvokeQuickSpell` = retail `cls_0x5a7b98_TCharacter::meth_0x51b5d0`
(`recon/classes/cls_0x5a7b98.cpp:15057-15120`), whose three spell-fail branches
each call `meth_0x4d5b90` (the fizzle-cast helper, `:12132`, calls
`meth_0x4d5c20_Cast`) — exactly matching the snapshot's three
`CastByName("Fizzle")` paths (`player.cpp:558,567,571`). The `"Fizzle"`/`"fizzle"`
strings are XREF'd from these caller sites (`_data.txt:105896` lowercase variant
from `meth_0x4d5c20:004d5de6`; `005e2998/29d0/29e8` TextBar messages from
`meth_0x51b5d0`). Trigger wiring is **retail-confirmed**.

**Bottom line:** retail-confirms *that* a "Fizzle" effect + animator are
registered and *that* it is the spell-fail puff cast from `InvokeQuickSpell`, and
the shipped sprites/geometry are unchanged. It does **not** confirm the animator's
numeric constants (§3) or its exact per-tick logic (§6) — those are snapshot-only
and must be visually matched against in-game ground truth (§12, §13).

---

## 3. Constants

All from `TFizzleAnimator` / `TParticleSystem`. Every `DUST_*` value is
**snapshot-only** (the retail animator body is not extracted; see §2.1(1)).

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| DUST_COUNT | 30 | particles per system (×3 systems = 90 capacity) | effect_old.cpp:12293 | snapshot-only |
| DUST_FRAME | 15 | ticks — emission window (stop spawning + death gate) | effect_old.cpp:12294 | snapshot-only |
| DUST_SPREAD | 15 | wu — ± position jitter on x and y at spawn | effect_old.cpp:12295 | snapshot-only |
| DUST_MIN_Z / DUST_MAX_Z | 5 / 45 | (×0.1) → z-velocity magnitude 0.5..4.5 wu/tick downward | effect_old.cpp:12296-12297 | snapshot-only |
| DUST_ROT | 15 | deg/tick — ± in-plane rotational velocity (`rot.z`) | effect_old.cpp:12298 | snapshot-only |
| DUST_MIN_SCL / DUST_MAX_SCL | 5 / 25 | (×0.01) → per-particle max scale 0.05..0.25 | effect_old.cpp:12299-12300 | snapshot-only |
| DUST_SCL_INC | 0.05 | scale/tick — grow rate (life_span==100 phase) | effect_old.cpp:12301 | snapshot-only |
| DUST_SCL_DEC | 0.02 | scale/tick — shrink rate (life_span==200 phase) | effect_old.cpp:12302 | snapshot-only |
| DUST_ADD | 1.5 | emission accumulator increment per tick (→ ~1.5 spawns/tick) | effect_old.cpp:12303 | snapshot-only |
| spawn pos.z | random(70,130) | wu above effect origin (caster) | effect_old.cpp:12387 | snapshot-only |
| life_span (initial) | 100 | sentinel/phase tag = "growing" (NOT a tick count; see §6) | effect_old.cpp:12405 | snapshot-only |
| flicker | random(0,1) | bool re-rolled each tick → 1.5× scale that frame when set | effect_old.cpp:12408, :12475 | snapshot-only |
| acc (x,y,z) | 1.0 | velocity multiplier per tick → **constant velocity** (no accel/drag) | effect_old.cpp:12395 | snapshot-only |
| vel.x / vel.y | 0.0 | particles fall straight down (no horizontal drift) | effect_old.cpp:12390-12391 | snapshot-only |
| system pick | random(1,3) | which of blue/red/purple each new particle joins | effect_old.cpp:12415 | snapshot-only |
| RefreshZBuffer patch | 300 × 300 px, centered | screen px Z-restore over the effect origin | effect_old.cpp:12512-12515 | snapshot-only |
| Render rot.x | -π/2 | WorldXY ground tip (in `TParticleSystem::Render`) | effectcomp.cpp:1088 | snapshot-only |
| Render rot.z | -π/4 | static in-plane spin (in `TParticleSystem::Render`) | effectcomp.cpp:1089 | snapshot-only |
| flicker scale boost | ×1.5 | per-axis scale multiply when flicker set | effectcomp.cpp:1095-1097 | snapshot-only |
| Class.Def hash | 0xab8800dd | asset version key for `Magic\Fizzle.I3D` | Class.Def:2077 | yes (retail asset present) |

`random(min,max)` is **inclusive** on both ends (`rand() % (max-min+1) + min`,
`src/revutils.cpp:1597-1612`). `TORADIAN = π/180` (`src/revdefs.h:25`).

> **Particle budget.** Emission runs only while `frame_count < DUST_FRAME (15)`,
> adding ~1.5/tick (DUST_ADD), so ~22–23 particles total spawn across the burst,
> distributed randomly across the 3 systems. Each system caps at DUST_COUNT (30),
> never reached.

---

## 4. Assets

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| Fizzle | `legacy/Imagery/Magic/Fizzle.I3D` (snapshot) / `Imagery/Magic/fizzle.i3d` in `data/imagery.rvi` (shipped) | 9,980 B snapshot / 8,364 B shipped (payload identical, §2.1) | the 3-color dust-puff imagery — STILL 3D mesh, **3 sub-objects** each a textured quad | registered `Class.Def:2077` name `"Fizzle"` (hash `0xab8800dd`); loaded by the EFFECT registry; the animator binds sub-objects via `GetObject(0/1/2)` (`effect_old.cpp:12354-12356`) |

**Sub-objects (3)** — named `box01`, `box02`, `box03` (read from the I3D object
table; counts `numobjects=3, nummaterials=3, numtextures=3, numverts=12,
numfaces=6` = three 4-vertex / 2-triangle quads). The animator maps them to its
three particle systems:

| obj index | name | system label | dominant texture color (ARGB4444) |
|-----------|------|--------------|------------------------------------|
| 0 | `box01` | `blue` (`effect_old.cpp:12354`) | deep **blue** RGB≈(17,34,136) |
| 1 | `box02` | `purple` (`effect_old.cpp:12356`) | **violet/purple** RGB≈(85,17,136) |
| 2 | `box03` | `red` (`effect_old.cpp:12355`) | **magenta** RGB≈(136,17,119) |

> Note the index↔label crossing in `Initialize`: `red.Init(…, GetObject(2), …)`
> and `purple.Init(…, GetObject(1), …)` (`effect_old.cpp:12355-12356`). The
> `red` system draws `box03` (a red-magenta sprite); `purple` draws `box02`.

**Textures (3):** each **32×32, 16-bit ARGB4444** (R=0x0f00 / G=0x00f0 /
B=0x000f / A=0x f000), **single frame** (no `framehtexs` flipbook). Each is a
soft dust-puff sprite: a bright white core fading out to a colored halo, with a
**real per-pixel alpha gradient** (≈163/1024 edge pixels fully transparent, mean
opaque alpha ≈ 56/255). Materials are **neutral white** (diffuse/ambient =
(1,1,1,1), specular ≈ (1,.9,.9,.9), emissive ≈ 0) — so the **color comes from the
texture, not the material**.

**The effect loads a real authored asset — do NOT substitute procedural dust.**
The three colored sprites + their alpha falloff ARE the visual identity (§10).

---

## 5. Spawn & emit

- **Trigger semantics:** **fixed-duration burst.** Emission happens only while
  `frame_count < DUST_FRAME (15)` (`effect_old.cpp:12378`); after that the effect
  just animates the remaining particles out and self-destructs (§6). Not
  continuous, not looping.
- **Count per trigger:** ~22–23 particles total. Per tick, `add += 1.5`
  (DUST_ADD) and the `while(add > 1.0 …)` loop spawns one particle per whole unit
  (`effect_old.cpp:12367,12378-12380`) → ~1.5/tick over 15 ticks. Each new
  particle joins a random one of the 3 systems (`random(1,3)`,
  `effect_old.cpp:12415-12422`).
- **Initial direction / distribution:** particles **fall straight down.**
  `vel.x = vel.y = 0`, `vel.z = -random(5,45)·0.1` (i.e. −0.5..−4.5 wu/tick;
  z is up, so negative = downward) (`effect_old.cpp:12390-12392`). `acc = 1.0` on
  all axes → velocity is **constant** (no gravity term, no drag). So each particle
  drifts straight down at its own fixed speed.
- **Emit anchor convention:** the effect object is placed at the **caster** (the
  spell is cast on the player with no target — `VARIANT "Fizzle" … "fizzle" …`,
  `spell.def:262`; the spell's source is the invoker). Particles seed at
  `pos = (random(±15), random(±15), random(70,130))` **relative to the effect
  origin** (`effect_old.cpp:12385-12387`) — i.e. a ±15 wu horizontal scatter
  centered on the caster, **70–130 wu up** (≈ hand/upper-body height). The burst
  appears as a puff in front of/around the failed caster's upper body.
- **Coordinate space:** particle positions integrate in the effect's **local
  space** (relative to the object origin); `TParticleSystem::Render` writes
  `object->pos` and builds the object matrix with `OBJ3D_MATRIX`
  (`effectcomp.cpp:1080,1101-1104`). Render does NOT set `OBJ3D_ABSPOS`
  (`abs_pos=false` from `TFizzleAnimator::Render`), so positions are
  object-relative.
- **Spread / jitter:** position ±15 wu in x/y and 70–130 wu in z; z-velocity
  0.5–4.5 wu/tick; per-particle max-scale 0.05–0.25; rot.z spin ±15 deg/tick;
  initial `rot.z = random(0,359)` then immediately overwritten to 0 (see §13.2).

### Spatial diagram

```
   wz (up)
130 ┤   ·   ·   ·     ← particles seed in a band 70..130 wu above origin,
    │  · · ·· ·  ·       ±15 wu scatter in x/y; each falls straight DOWN
 70 ┤ ·· ·  · · ·        (vel.x=vel.y=0, vel.z = -0.5..-4.5 wu/tick, const)
    │      │
    │      │ each quad: WorldXY (lies flat on ground, rot.x=-π/2),
    │      ▼ static in-plane spin rot.z=-π/4 + per-tick ±15°/tick
  0 └───────────────── wx     emit origin = caster (player) world pos
   ╱
  wy

per particle: scale 0 → grows (×0.05/tick) → max(0.05..0.25) → shrinks
              (×0.02/tick) → 0 → die.  flicker re-rolled each tick (×1.5 scale).
```

---

## 6. Behavior & per-frame logic

Two layers run each tick: the generic `TParticleSystem` (lifetime + optional
movement) and the bespoke `TFizzleAnimator::Animate` (spawn cadence + the
grow/shrink scale state machine + spin + the done-check).

### 6.1 `TParticleSystem::Animate` (per system, `effectcomp.cpp:1041-1068`)

```
for each used particle:                       // effectcomp.cpp:1045-1067
    if life >= life_span: used = false; continue   // <-- see note: life_span is a TAG here
    life += 1
    if check_move (true for Fizzle):                // effectcomp.cpp:1057
        pos += vel                                  // straight-line integrate (vel.x=y=0)
        vel *= acc                                  // acc=1.0 → velocity unchanged
```

> **Lifetime caveat.** `life` increments every tick and the particle is freed when
> `life >= life_span`. Fizzle sets `life_span = 100` at spawn, but then **reuses
> `life_span` as a phase tag** (100 = growing, 200 = shrinking, 0 = finished) in
> the animator. So a particle is freed by `TParticleSystem::Animate` only if it
> survives 100 ticks in the growing phase without the animator flipping it to 200;
> in practice the animator's scale machine sets `life_span = 0` (kill) well before
> that. The real lifetime is the grow+shrink scale cycle (§6.2), not a fixed tick
> count.

### 6.2 `TFizzleAnimator::Animate` (`effect_old.cpp:12361-12487`)

```
Animate(draw):                                       // effect_old.cpp:12361
    T3DAnimator::Animate(draw)                        //   base hierarchy update
    inst->SetCommandDone(false)                       //   keep effect alive this tick // :12365
    add += DUST_ADD (1.5)                              // :12367
    frame_count += 1                                   // :12371

    blue.Animate(); red.Animate(); purple.Animate()   //   §6.1 per system // :12373-12375

    // --- emission (only during the first DUST_FRAME=15 ticks) ---
    while add > 1.0 and frame_count < DUST_FRAME:      // :12378
        add -= 1.0
        p.pos   = (random(-15,15), random(-15,15), random(70,130))   // :12385-12387
        p.vel   = (0, 0, -random(5,45)*0.1)            // straight down // :12390-12392
        p.acc   = (1,1,1)                              //   constant velocity // :12395
        p.scl   = (0,0,0)                              //   start invisible // :12398
        p.rot   = (0,0,0)                              //   (random rot.z assigned then zeroed, §13.2) // :12401-12402
        p.life_span = 100                              //   phase tag = GROWING // :12405
        p.flicker   = random(0,1)                      // :12408
        p.temp.y    = random(-15,15)                   //   rot.z spin rate (deg/tick) // :12411
        p.temp.z    = random(5,25)*0.01                //   max scale 0.05..0.25 // :12413
        join random(1..3) → blue|red|purple .Add(&p)   // :12415-12422

    // --- per-particle scale state machine + spin ---
    done = true
    for ps in {blue, red, purple}:                     // :12429-12438
        for i in 0..DUST_COUNT-1:                       // :12439
            particle = ps.Get(i); if !used: continue
            if life_span == 100:                        //   GROWING // :12445
                scl += DUST_SCL_INC (0.05) on x,y,z      // :12447-12449
                if scl.x > temp.z: life_span = 200       //   reached max → SHRINKING // :12451-12452
            elif life_span == 200:                       //   SHRINKING // :12454
                scl -= DUST_SCL_DEC (0.02) on x,y,z      // :12456-12458
                if scl.x <= 0: scl = 0; life_span = 0    //   done → freed next Animate // :12460-12464
            rot.z += temp.y                              //   spin (deg) // :12467
            wrap rot.z into [0,360)                       // :12469-12472
            flicker = random(0,1)                        //   re-roll flicker // :12475
            done = false                                 //   something still alive

    // --- finish ---
    if frame_count >= DUST_FRAME and done:               // :12482
        ((TEffect*)inst)->KillThisEffect(); return        //   self-destruct // :12484-12485
```

- **Particle motion:** straight-line fall (`vel.z<0`, const). No gravity, no
  drag, no horizontal motion.
- **Scale envelope (the identity):** each particle grows linearly from 0 at
  +0.05/tick until its scale exceeds its own random max (`temp.z`, 0.05–0.25),
  then shrinks at −0.02/tick back to 0, then dies. Asymmetric: grows ~2.5× faster
  than it shrinks (pop in, fade out). This is a **triangle-wave-ish scale curve**
  with a sharp rise and a slower fall.
- **Spin:** `rot.z += temp.y` each tick, `temp.y = random(-15,15)` deg/tick — each
  particle spins in-plane at a random rate/direction.
- **Flicker:** `flicker` is re-rolled `random(0,1)` every tick; in Render, a set
  flicker multiplies that frame's scale by 1.5 (`effectcomp.cpp:1093-1098`) — a
  per-frame size shimmer (≈ a twinkle).
- **Lifetime / death:** the effect calls `SetCommandDone(false)` every tick so it
  is never reaped externally; it self-kills only once emission is over
  (`frame_count >= 15`) AND every particle has finished its scale cycle
  (`done`) via `KillThisEffect()` (`effect_old.cpp:12482-12485`,
  `TEffect::KillThisEffect` `effect.cpp:950`).
- **No color/alpha curve in code.** The fade-in/out is purely the **scale**
  envelope; per-vertex color/alpha is never written by the effect (the visible
  softness is the texture's own alpha, §7, §10).

### Temporal diagram

```
emission:  ████████████████              (ticks 0..15, ~1.5 particles/tick)
           0              15  ...→ effect self-kills when last particle's scale→0

per particle scale (the envelope):
scl ┤        ╱‾‾╲
    │      ╱     ╲___
  0 ┤____╱          ╲____   ticks
       grow(+.05)   shrink(-.02)   peak = temp.z (0.05..0.25)
    (rise ~2.5× faster than fall; spins throughout; flicker ⇒ ×1.5 random frames)
```

---

## 7. Rendering (original render state + geometry)

- **What it draws:** for each live particle, one textured quad — the system's
  sub-object (`box01`/`box02`/`box03`) placed/scaled/rotated by a per-particle
  matrix. `TFizzleAnimator::Render` (`effect_old.cpp:12489-12502`) brackets the
  draw and calls each system's `Render(true)` (the `true` = flicker enabled):

```
Render():                                  // effect_old.cpp:12489
    SaveBlendState(); SetBlendState();       //   Alpha mode // :12492-12493
    blue.Render(true); red.Render(true); purple.Render(true)   // :12495-12497
    RestoreBlendState()                       // :12499
    return true
```

`TParticleSystem::Render(flicker=true, abs_pos=false)` (`effectcomp.cpp:1070-1109`)
per particle:

```
object->flags = OBJ3D_MATRIX                 // matrix overrides anikey/pos/rot/scl // effectcomp.cpp:1080
matrix = identity
RotateX(rot.x·TORADIAN); RotateY(rot.y·TORADIAN); RotateZ(rot.z·TORADIAN)  // particle spin (rot.x/y are 0) // :1085-1087
RotateX(-π/2)                                //   tip onto ground = WorldXY // :1088
RotateZ(-π/4)                                //   static in-plane spin // :1089
RotateZ(facing)                              //   facing = 0 for Fizzle (Init s=default) // :1090
scl = particle.scl;  if flicker && particle.flicker: scl *= 1.5  // :1092-1098
Scale(scl)                                   // :1099
pos.z = FIX_Z_VALUE(pos.z)                   //   z-depth conversion // :1103
Translate(pos)                               // :1104
RenderObject(object)                          // :1106
```

- **Blend mode (original):** **Alpha.** `TFizzleAnimator::Render` calls
  `SetBlendState()` (`effect_old.cpp:12493`), **not** `SetAddBlendState()`.
  `SetBlendState` (`:221-233`) = texture stage `D3DTBLEND_MODULATE`,
  `SRCBLEND=SRC_ALPHA`, `DESTBLEND=INV_SRC_ALPHA`, `ZWRITEENABLE=false`,
  `ZENABLE=true` → classic translucent **Alpha (modulated)** (NOMENCLATURE §3).
  The dust puffs are alpha-blended (they use the sprite's authored per-pixel
  alpha, §4), not additive.
- **Lit vs self-lit:** **Unlit / self-lit.** The animator never folds ambient
  light into vertex color and never zeroes the material; it draws the imagery's
  authored verts/texture directly (materials are neutral white, §4). Color is
  literal from the sprite. Classify **Unlit** (NOMENCLATURE §4).
- **Depth / Z:** **TestNoWrite** — `ZENABLE=true`, `ZWRITEENABLE=false` (from
  `SetBlendState`, `effect_old.cpp:224-225`). `TFizzleAnimator::RefreshZBuffer`
  (`:12504-12516`) restores scene Z over a fixed **300×300 px** patch centered on
  the projected effect origin so the no-depth-write puffs composite correctly.
  (Note: the per-particle `TParticleSystem::RefreshZBuffer` at
  `effectcomp.cpp:1111-1131` is **not** used — the animator overrides it with the
  single 300×300 patch.)
- **Orientation:** **WorldXY** — `RotateX(-π/2)` (`effectcomp.cpp:1088`) tips each
  authored quad flat onto the world ground plane. Plus a static `RotateZ(-π/4)`
  and the animated per-particle `rot.z` spin. (This is the ground-tip tell from
  RENDERING_CONVENTIONS §5.1; the dust lies on the floor and foreshortens under
  the iso view, rather than facing the camera.)
- **Per-quad transform:** full matrix (`OBJ3D_MATRIX`): rotation (per-particle
  `rot.z` spin + the −π/2 tip + −π/4 static), uniform scale (the grow/shrink
  envelope, ×1.5 on flicker frames), translation (the particle position with
  `FIX_Z_VALUE` on z).
- **Per-vertex color packing:** NONE written by the effect. Vertices come from the
  imagery with its authored color; under MODULATE the sampled texel × the
  authored (white) diffuse = the sprite color passes through. No tint.

---

## 8. Texture animation

**N/A — none.** All three textures are single-frame 32×32 stills (no `framehtexs`
array; §4). `Render` never mutates `tu/tv` and never calls `SetTextureFrame`. All
motion is positional (falling) + transform (scale/spin); the texture itself is
static.

---

## 9. Associated light

**N/A — none.** Neither `TFizzleEffect` nor `TFizzleAnimator` (Initialize/Animate/
Render, `effect_old.cpp:12332-12516`) makes any `AddPointLight` / dynamic-light
call, and `TParticleSystem` adds none (`effectcomp.cpp:1031-1158`). The fizzle
puff does not light the scene — it is self-lit billboards only.

---

## 10. Color

- **Source:** the **authored `Magic\Fizzle.I3D` sprite textures** (§4). The effect
  supplies no color of its own (no per-vertex tint, no spell color, no chardata
  field); materials are neutral white. Each of the 3 systems draws a different
  colored sprite, and new particles are assigned to systems at random
  (`random(1,3)`), so the cloud is a **mix of blue, purple, and magenta** dust.
- **Exact values (texture dominant RGB, ARGB4444 decoded, 0–255):**
  - `box01`/`blue`: **(17, 34, 136)** — deep saturated blue, white core.
  - `box02`/`purple`: **(85, 17, 136)** — violet/purple, white core.
  - `box03`/`red`: **(136, 17, 119)** — magenta / red-violet, white core.
  Each sprite has a bright white center fading through its hue to fully
  transparent edges (per-pixel alpha, mean opaque ≈ 56/255).
- **Expected visual:** a small, brief sparkle-dust puff in cool magic colors —
  blue + violet + magenta motes with bright white hearts, twinkling (flicker) and
  spinning as they grow then shrink and fade. **Pale/gray/washed at reconstruction
  = broken port** (per AGENT_GUIDE §4.2.1.5: a procedural stand-in instead of the
  real 3 sprites #1, or wrong blend / lost alpha / chroma-key miss). The colors
  are rich and saturated by intent — verify it's the *textures'* color, not a
  default-white fallback.
- **Normalization / boosts:** none. (`flicker` ×1.5 boosts *scale*, not color.)

---

## 11. Audio coupling

**No audio coupling found** in the effect. Neither `TFizzleEffect`/
`TFizzleAnimator` (`effect_old.cpp:12325-12516`) nor `TParticleSystem`
(`effectcomp.cpp`) plays any sound. The `SPELL "Fizzle"` def has `FLAGS SF_NONE`
and no `SOUND`/`WAVE` directive (`legacy/spell.def:253-263`). (The recon
candidate yaml speculates a `TAmbSoundEffect` (`cls_0x5ad9d4`) might hook
"Fizzle" for a cue, but the snapshot effect code triggers none — record as a
possible audio-phase follow-up, not a confirmed coupling.) The caller
`InvokeQuickSpell` prints a TextBar message ("Spell failed" / "…talismans…
missing", `player.cpp:559,566`) but plays no wave.

---

## 12. Triggers & in-game appearance

- **Spawned by:** spell-cast failure. `TPlayer::InvokeQuickSpell`
  (`src/player.cpp:547-574`) calls `CastByName("Fizzle")` on **three** failure
  paths: (1) the quickspell slot is empty (`:571`); (2) the player is missing some
  required talismans (`HasTalismans` false, `:567`); (3) the talismans are present
  but don't form a valid spell (`CastByTalismans` false, `:558`). `CastByName`
  (`spell.cpp:492-512`) then builds the `"Fizzle"` spell on the caster, whose
  `VARIANT "Fizzle", TP_BASIC, "x", "fizzle", …` (`spell.def:262`) spawns the
  `"Fizzle"` effect (`DEFINE_BUILDER("Fizzle", TFizzleEffect)`,
  `effect_old.cpp:12329`; animator `REGISTER_3DANIMATOR("Fizzle", …)`, `:12345`).
  All confirmed in retail (§2.1(4)).
- **Where to see it in the original game:** as the player, trigger a failed cast —
  easiest is to invoke an **empty quickspell slot** or a **bad/incomplete talisman
  combination** (e.g. cast with talismans you don't fully have, or a combo that
  isn't a real spell). The blue/purple/magenta dust puff appears at the player's
  upper body with the "Spell failed" text.
- **Vestigial?** No — live caller (the spell-fail path is core to the magic
  system). Note `SPELL "Fizzle"` has `DAMAGETYPE DT_NONE` and `ANIMATION
  "invoke2"` — the player plays a cast animation but nothing happens beyond the
  puff.

---

## 13. Gaps & uncertainties

- **13.0 Snapshot-vs-retail risk (from §2.1).** The animator's `DUST_*` constants
  (§3) and per-tick logic (§6) are **snapshot-only** — the retail animator body is
  not in the recon extract, and the DUST float immediates were not found in the
  decomp. Registration + trigger + the asset's visual payload ARE retail-confirmed.
  The reconstruction should treat the §3 numbers as the best evidence but **visually
  match the result against an in-game failed-cast capture (§12)** — particularly
  the particle count/density, the grow/shrink timing, and the spin/flicker rate,
  which are exactly the kind of values that get late-tuned.
- **13.1 No retail animator body.** `recon/classes/cls_0x5ad758.cpp` is the sparse
  *effect shell* (ctor/dtor only); there is no extracted `TFizzleAnimator`
  Animate/Render. So §6 (scale state machine, spin, flicker, done-check) and §7
  (the WorldXY tip, the −π/4 spin, the Alpha blend, the 300×300 Z patch) are from
  the snapshot only. The mapping yaml is correctly flagged LOW-MEDIUM; this doc
  upgrades the *picture* (the source proves a real `TFizzleEffect` +
  `TFizzleAnimator` exist, contrary to the yaml's "no class in source" note) but
  cannot upgrade the *animator constant* confidence.
- **13.2 Dead spawn lines.** At spawn, `p.rot.z = random(0,359)` is written then
  immediately overwritten by `p.rot.x = p.rot.y = p.rot.z = 0.0f` on the next line
  (`effect_old.cpp:12401-12402`) — so the initial random spin **never takes
  effect**; every particle starts at `rot.z = 0` and accumulates only via
  `temp.y`. Reconstruct the effective behavior (start at 0), not the dead line.
- **13.3 `life_span` overload.** `life_span` is both the `TParticleSystem` death
  threshold (compared against `life`) AND the animator's phase tag (100/200/0).
  The initial value 100 is a phase tag, not a 100-tick lifetime — the real
  lifetime is the grow+shrink scale cycle (§6.1 note, §6.2). A reconstruction that
  reads `life_span=100` as "lives 100 ticks" would be wrong.
- **13.4 Effective lifetime is data-dependent.** A particle's on-screen time =
  grow time (`temp.z / 0.05` ticks) + shrink time (`temp.z / 0.02` ticks). For
  `temp.z` in 0.05..0.25 that's ≈ (1..5) + (2.5..12.5) ≈ 3.5–17.5 ticks. Combined
  with staggered spawns over 15 ticks, the whole effect lasts roughly ~30 ticks.
  Confirm the felt duration against an in-game capture.
- **13.5 `facing` arg.** Each system's `Init` is called with a default `S3DPoint s`
  (zero) and the 5th `facing_angle` arg defaulting to 0 (`effect_old.cpp:12354-
  12356`; `Init` `effectcomp.cpp:1031`), so the Render `RotateZ(facing)` is a
  no-op for Fizzle. Reconstruct with facing = 0.
- **13.6 Possible audio cue.** §11 — the recon yaml's `TAmbSoundEffect`/"Fizzle"
  speculation is not confirmed by snapshot code; flag for the audio phase to
  check whether shipped Fizzle has a sound the snapshot lacked.

---

## 14. Reconstruction burndown

```
- [ ] Load Magic\Fizzle.I3D (snapshot 9,980 B / shipped 8,364 B — payload
      identical). Address its 3 STILL textured-quad sub-objects: 0=box01(blue),
      1=box02(purple/violet), 2=box03(red/magenta), each a 32x32 ARGB4444 dust
      sprite (white core → colored halo → transparent edge). NO procedural dust
      stand-in. (§4)
- [ ] Three particle systems (blue/red/purple), each drawing its sub-object; new
      particles assigned to a random system (random(1,3)). (§4, §6.2)
- [ ] Emit = fixed-duration BURST: ~1.5 particles/tick (DUST_ADD) for the first 15
      ticks (DUST_FRAME), ~22-23 total; effect self-kills when emission is over
      AND every particle's scale cycle has finished. (§5, §6.2)
- [ ] Emit origin = caster (player) world pos; particles seed at local
      (±15, ±15, 70..130) wu — a small horizontal scatter, 70-130 wu up. (§5)
- [ ] Particle motion: fall straight down, vel=(0,0,-random(5,45)*0.1),
      constant velocity (acc=1.0, no gravity/drag). (§5, §6.1)
- [ ] Per-particle SCALE state machine (the identity): start scl=0, grow +0.05/tick
      until scl > temp.z (max 0.05..0.25), then shrink -0.02/tick to 0, then die.
      Asymmetric pop-in / slow-out triangle envelope. (§6.2, §13.4)
- [ ] Per-particle in-plane SPIN: rot.z += temp.y each tick, temp.y=random(-15,15)
      deg/tick; wrap [0,360). (§6.2)
- [ ] FLICKER: re-roll random(0,1) each tick; on set frames multiply that frame's
      render scale by 1.5 (a per-frame twinkle). (§6.2, §7)
- [ ] Core geometry: one textured quad per live particle, full per-particle matrix
      = spin(rot.z) ∘ groundtip(rot.x=-π/2) ∘ static(rot.z=-π/4) ∘ scale ∘
      translate(pos with FIX_Z_VALUE on z). (§7)
- [ ] Orientation = WorldXY (lies flat on ground, rot.x=-π/2 tip) — NOT
      camera-facing. (§7)
- [ ] Blend = Alpha (SetBlendState: MODULATE, SRC_ALPHA/INV_SRC_ALPHA — NOT
      additive); lit-mode = Unlit (asset color, neutral-white material, no tint,
      uses the sprite's per-pixel alpha); depth = TestNoWrite. (§7)
- [ ] Texture animation = NONE (single-still sprites, no UV scroll, no flipbook). (§8)
- [ ] Associated dynamic light = NONE. (§9)
- [ ] Color from the 3 Fizzle.I3D sprites = blue (17,34,136) / purple (85,17,136)
      / magenta (136,17,119), bright white cores. Cloud = a mix of all three.
      Pale/gray ⇒ stand-in / wrong blend / lost alpha. (§10)
- [ ] Sub-effects spawned = NONE. (§6)
- [ ] RefreshZBuffer: restore scene Z over a single 300x300 px patch centered on
      the projected effect origin (animator override; per-particle version unused). (§7)
- [ ] Audio = none in the effect (record possible TAmbSoundEffect "Fizzle" cue for
      the audio phase to verify). (§11, §13.6)
- [ ] Trigger wiring: cast on spell failure via TPlayer::InvokeQuickSpell →
      CastByName("Fizzle") (empty slot / missing talismans / invalid combo),
      player.cpp:547-574; spell.def:253-263. (§12)
- [ ] Do NOT reconstruct: the dead initial-random-rot.z line (§13.2); do NOT treat
      life_span=100 as a 100-tick lifetime (it is a phase tag) (§13.3).
```

**Definition of done:** a failed spell cast produces a brief (~30-tick) puff of
~22 small blue/purple/magenta dust quads at the caster's upper body, each lying
flat on the ground plane, popping in (fast grow) and fading out (slow shrink)
while spinning and twinkling, alpha-blended and self-lit from the three authored
sprites, with no light and no sound — and the effect dies on its own when the
last particle's scale reaches zero.
