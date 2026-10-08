# X21 TFizzleEffect — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | X21 |
| **Class(es)** | `TFizzleEffect` (effect shell) + `TFizzleAnimator` (animator — all logic). Registered name `"Fizzle"`. |
| **Status** | source_behavior_audited (2026-10-04); retail visual/runtime acceptance **false / pending**. Numerical and renderer gaps are listed in §13. |
| **Retail fidelity** | Retail animator and particle-helper bodies recovered directly from the supplied executable. Constants, RNG-call order, system order, lifecycle and matrix chain corroborate the snapshot. Retail differs in local-Z handling and adds a `FAIL` sound. The x87 growth comparison is corrected; platform RNG remains an explicit comparison gap; a complete retail capture is available, but visual and gameplay acceptance are pending. |
| **Author / Date** | vfx-forensics-agent / 2026-05-20 |
| **Family** | magic |
| **Draws** | particle emitter — N small textured billboard quads (3 colored "dust" sub-objects of `Magic\Fizzle.I3D`), one quad per live particle |
| **Archetype(s)** | per [knowledge/05_EFFECT_ARCHETYPES.md](knowledge/05_EFFECT_ARCHETYPES.md): **(E) particle emitter** — a short fixed-duration *burst* (emits before tick 15, then plays out). With **(F2) custom transform animation** (each particle grows-then-shrinks via a 2-state scale machine and spins in-plane). No texture animation, no associated light, no sub-effects. |

---

## 1. Summary

`"Fizzle"` is the spell-failure puff: when the player tries to cast a spell that
fails, the game casts the built-in `"Fizzle"` spell, which spawns a small, brief
cloud of colored "dust" particles at the caster. Mechanically it is a particle
burst — `TFizzleAnimator` runs three small particle systems (the programmer
named them `blue`, `red`, `purple`), seeds 20 particles total before tick 15, and each particle is a tiny textured quad that **scales up from zero,
then scales back to zero** while spinning, then dies; when the last one is
gone the effect kills itself. Each of the three systems draws a different
authored sprite — a deep-blue, a violet/purple, and a magenta dust puff
(`Magic\Fizzle.I3D`, 3 sub-objects) — so the cloud reads as a mixed
blue/purple/magenta sparkle-dust burst. It is drawn translucent (Alpha blend),
uses the authored XY mesh tipped upright by `RotateX(-π/2)`, and is self-lit (color is the authored
sprite, no scene light).

---

### Port validation update — 2026-10-04

The canonical and shim previews now preserve the four authored corners/UVs,
material diffuse color and full spin → X tip → static Z rotation chain.
Both integrate the snapshot simulation at true 24 Hz, rather than a truncated
41 ms interval or one update per render. Source system order (blue/red/purple),
spawn RNG order (including the overwritten rotation draw), and next-tick
particle reaping are preserved. A source-state check compares both paths over
100 seeds at 24/30/60/144 render Hz and at 0.5/1/1.5 seconds; it verifies active
particle position, scale, spin, flicker, phase, lifecycle and subsequent RNG state.
The source-state check passes for the two port paths. A separate 71-case exact-
binary-rational check verifies both paths against recovered retail growth
thresholds for all max-scale picks 5–25, including `.2f + .05f` versus `.25f`.
It does not prove the retail CRT random sequence or the complete device
rasterization (§13). Integration build and retail comparison are
separate acceptance gates; **retail visual fidelity remains pending**.

The source's strict `frame_count < 15` and `add > 1` conditions emit exactly
20 particles over ticks 1–14. The asset has authored Z=0 vertices: the X tip
turns it upright, so the earlier flat-ground orientation inference was wrong.

---

## 2. Sources & evidence

- **Retail executable, direct disassembly (2026-10-04):** supplied
  `/Users/benjamincooley/RevenantRetailLab/retail-cd/REVENANT/Revenant.exe`,
  SHA-256 `28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.
  The running compatibility copy `guest-agent/compat-game/Rev98.exe`, SHA-256
  `33545a2f4e055dfaa3a62e3b03488d6f3bd63fe6c9827b1f88e141e1387073eb`,
  redirects multimedia import DLL names only (`guest-agent/make-compat-game.py`).
  Byte comparisons confirm the inspected Fizzle, particle, blend and RNG code
  ranges are identical between supplied and compatibility executables. Compared ranges: `0x4f3ef0–0x4f4505`, `0x4f9f50–0x4fa300`,
  `0x50c170–0x50c5f0`, `0x417d60–0x418534`, `0x483300–0x48332d`,
  `0x58c582–0x58c5a4` (half-open intervals). All
  addresses below are executable virtual addresses (image base `0x400000`).
- **Correct retail identity:** effect builder startup `0x4f3ef0`, builder vtable
  `0x5ad7cc`, factory `0x4f9f50`, effect-instance vtable `0x5ad7d0`, allocation
  `0x184` bytes. Animator builder startup `0x4f3f70`, builder vtable `0x5ad9d0`,
  factory `0x4fa070`, animator-instance vtable `0x5ad9d4`, allocation `0x17c`
  bytes. Both registrations use `"Fizzle"` at `0x5e121c`.
  `recon/classes/cls_0x5ad9d4.cpp` contains the animator factory and Render;
  `cls_0x5ad9d4__vftable_5ad9d4.cpp` identifies Initialize `0x4f3f90`,
  Animate `0x4f4050`, Render `0x4f4440`, RefreshZBuffer `0x4f44b0`.
  The former `cls_0x5ad758` candidate is **not the Fizzle effect shell**;
  its adjacent-class layout is not evidence for Fizzle.
- **Retail particle helper:** vtable `0x5a96d8`; Init `0x50c170`,
  Animate `0x50c1b0`, Render `0x50c220`, RefreshZ `0x50c460`, Add `0x50c560`.
  Raw traces are saved under `/Users/benjamincooley/RevenantRetailLab/research/`
  as `fizzle-retail-animator.asm`, `fizzle-retail-particle-system.asm`, and
  `fizzle-retail-blend-manager.asm`. The decompiler omits several bodies;
  searching its text for float literals was insufficient to establish absence.
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
- **Source-of-truth ranking:** executable instructions and shipped assets
  determine shipped behavior; the snapshot explains intent and names. The port's
  source-state test establishes agreement between its canonical and shim paths,
  not exact equality with retail's random generator or floating-point execution.

### 2.1 Retail-vs-snapshot reconciliation

**(1) Animator body and constants recovered.** Initialize `0x4f3f90` binds
blue/red/purple to sub-objects **0/2/1**, with movement enabled and facing zero.
The factory `0x4fa070` allocates three arrays of `0xa50` bytes, each holding
**30 × 0x58-byte particles**, at animator offsets `0xfc`, `0x124`, `0x14c`.
Animate `0x4f4050` increments the accumulator by 1.5 and the frame count, advances
blue/red/purple, then emits while accumulator **> 1** and frame count **< 15**.
Its spawn call order is X, Y, Z, downward velocity, overwritten initial rotation,
flicker, spin, max scale, system choice. Scale phase values are 100/200/0; growth
is .05, shrink .02, spin wraps at 360, and flicker rerolls even on the final
zero-scale tick. Generic Animate `0x50c1b0` frees phase-zero slots on the next
tick. These are shipped instructions, not extrapolation from captured images.

Float-pool VAs and decoded IEEE-754 values: `0x5a3810` **1.5**,
`0x5a34e4` **1.0**, `0x5ada34` **−.1**,
`0x5a3524` **.01**, `0x5a7e9c` **.05**, `0x5a8d10` **.02**,
`0x5aad10` **360**. The source constants are corroborated; x87 intermediate
comparison precision remains a separate issue (§13.0).

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

**(3) Registration and structure confirmed.** Startup table entries
`0x5c541c → 0x4f3ef0` and `0x5c5420 → 0x4f3f70` register the effect and animator.
Tracing their builder vtables to factories establishes the exact identities
listed above. The three embedded particle systems and their allocations are
visible in the real animator factory. This supersedes the old LOW-MEDIUM
`TFizzleEffect_cls_0x5ad758_candidate.yaml` association.

**(4) Registration + naming + trigger — confirmed.** The spell-fail trigger is
`TPlayer::InvokeQuickSpell` = retail `cls_0x5a7b98_TCharacter::meth_0x51b5d0`
(`recon/classes/cls_0x5a7b98.cpp:15057-15120`), whose three spell-fail branches
each call `meth_0x4d5b90` (the fizzle-cast helper, `:12132`, calls
`meth_0x4d5c20_Cast`) — exactly matching the snapshot's three
`CastByName("Fizzle")` paths (`player.cpp:558,567,571`). The `"Fizzle"`/`"fizzle"`
strings are XREF'd from these caller sites (`_data.txt:105896` lowercase variant
from `meth_0x4d5c20:004d5de6`; `005e2998/29d0/29e8` TextBar messages from
`meth_0x51b5d0`). Trigger wiring is **retail-confirmed**.

Retail confirms the simulation constants, emission/lifecycle order and matrix
chain, in addition to registration, trigger and asset payload. Shipped changes
from the snapshot include a conditional local-Z conversion and a `FAIL` sound.
Renderer sampling and numerical precision still require explicit validation.

---

## 3. Constants

From `TFizzleAnimator` / `TParticleSystem`, corroborated against the retail
executable (§2.1). Numeric constants do not imply identical CRT RNG streams or
x87 intermediate precision (§13).

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| DUST_COUNT | 30 | particles per system (×3 systems = 90 capacity) | effect_old.cpp:12293 | retail executable (§2.1, §7) |
| DUST_FRAME | 15 | ticks — emission window (stop spawning + death gate) | effect_old.cpp:12294 | retail executable (§2.1, §7) |
| DUST_SPREAD | 15 | wu — ± position jitter on x and y at spawn | effect_old.cpp:12295 | retail executable (§2.1, §7) |
| DUST_MIN_Z / DUST_MAX_Z | 5 / 45 | (×0.1) → z-velocity magnitude 0.5..4.5 wu/tick downward | effect_old.cpp:12296-12297 | retail executable (§2.1, §7) |
| DUST_ROT | 15 | deg/tick — ± in-plane rotational velocity (`rot.z`) | effect_old.cpp:12298 | retail executable (§2.1, §7) |
| DUST_MIN_SCL / DUST_MAX_SCL | 5 / 25 | (×0.01) → per-particle max scale 0.05..0.25 | effect_old.cpp:12299-12300 | retail executable (§2.1, §7) |
| DUST_SCL_INC | 0.05 | scale/tick — grow rate (life_span==100 phase) | effect_old.cpp:12301 | retail executable (§2.1, §7) |
| DUST_SCL_DEC | 0.02 | scale/tick — shrink rate (life_span==200 phase) | effect_old.cpp:12302 | retail executable (§2.1, §7) |
| DUST_ADD | 1.5 | emission accumulator increment per tick (→ ~1.5 spawns/tick) | effect_old.cpp:12303 | retail executable (§2.1, §7) |
| spawn pos.z | random(70,130) | wu above effect origin (caster) | effect_old.cpp:12387 | retail executable (§2.1, §7) |
| life_span (initial) | 100 | sentinel/phase tag = "growing" (NOT a tick count; see §6) | effect_old.cpp:12405 | retail executable (§2.1, §7) |
| flicker | random(0,1) | bool re-rolled each tick → 1.5× scale that frame when set | effect_old.cpp:12408, :12475 | retail executable (§2.1, §7) |
| acc (x,y,z) | 1.0 | velocity multiplier per tick → **constant velocity** (no accel/drag) | effect_old.cpp:12395 | retail executable (§2.1, §7) |
| vel.x / vel.y | 0.0 | particles fall straight down (no horizontal drift) | effect_old.cpp:12390-12391 | retail executable (§2.1, §7) |
| system pick | random(1,3) | which of blue/red/purple each new particle joins | effect_old.cpp:12415 | retail executable (§2.1, §7) |
| RefreshZBuffer patch | 300 × 300 px, centered | screen px Z-restore over the effect origin | effect_old.cpp:12512-12515 | retail executable (§2.1, §7) |
| Render rot.x | -π/2 | tip authored XY mesh upright (in `TParticleSystem::Render`) | effectcomp.cpp:1088 | retail executable (§2.1, §7) |
| Render rot.z | -π/4 | static in-plane spin (in `TParticleSystem::Render`) | effectcomp.cpp:1089 | retail executable (§2.1, §7) |
| flicker scale boost | ×1.5 | per-axis scale multiply when flicker set | effectcomp.cpp:1095-1097 | retail executable (§2.1, §7) |
| Class.Def hash | 0xab8800dd | asset version key for `Magic\Fizzle.I3D` | Class.Def:2077 | yes (retail asset present) |

`random(min,max)` is **inclusive** on both ends (`rand() % (max-min+1) + min`,
`src/revutils.cpp:1597-1612`; retail wrapper `0x483300`). Retail CRT `rand`
at `0x58c582` uses `state = state*214013 + 2531011` (32-bit wrap), returns
`(state >> 16) & 0x7fff`. Mac libc `rand()` is not assumed to share that stream.
`TORADIAN = π/180` (`src/revdefs.h:25`).

> **Particle budget.** Emission runs only while `frame_count < DUST_FRAME (15)`,
> adding 1.5/tick (DUST_ADD), so 20 particles total spawn across the burst,
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
- **Count per trigger:** 20 particles total. Per tick, `add += 1.5`
  (DUST_ADD) and the `while(add > 1.0 …)` loop spawns one particle per whole unit
  (`effect_old.cpp:12367,12378-12380`) → 20 particles over ticks 1–14 (the frame count increments before the strict <15 test). Each new
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
    │      │ each quad: authored XY mesh tipped upright by rot.x=-π/2,
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

    // --- emission (ticks 1–14; strict frame_count < DUST_FRAME=15) ---
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
                grown_x = extended(scl.x) + extended(0.05f)  // retail x87
                scl.x = float(grown_x); scl.y/z += 0.05f // stored float scales
                if grown_x > temp.z: life_span = 200       //   reached max → SHRINKING // :12451-12452
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
  then shrinks at −0.02/tick back to 0, then dies. Retail compares the
  unrounded X growth sum, while storing float scales (§13.0). Asymmetric: grows ~2.5× faster
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
emission:  ██████████████                (ticks 1..14, 20 particles total)
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
RotateX(-π/2)                                //   tip authored XY mesh upright // :1088
RotateZ(-π/4)                                //   static in-plane spin // :1089
RotateZ(facing)                              //   facing = 0 for Fizzle (Init s=default) // :1090
scl = particle.scl;  if flicker && particle.flicker: scl *= 1.5  // :1092-1098
Scale(scl)                                   // :1099
if abs_pos: pos.z = FIX_Z_VALUE(pos.z)       // retail 0x50c392..0x50c3b2
// Fizzle passes abs_pos=false, so local particle Z remains raw in retail.
// Snapshot effectcomp.cpp:1103 applies FIX_Z_VALUE unconditionally.
Translate(pos)                               // :1104
RenderObject(object)                          // :1106
```

- **Retail render bracket:** `0x4f4440` saves manager state (`0x4178e0`),
  calls `0x417d60(mode=2, texture_count=1)`, renders blue/red/purple with
  `(flicker=true, abs_pos=false)`, then restores (`0x417b00`). The mode-2 branch
  at `0x417f4f` sets render states **14/ZWRITE=0**, **7/ZENABLE=1 only when
  global `0x5c61ac` is nonzero and mode bit 0x40 is absent**,
  **22/CULL=1 (NONE)**, **19/SRCBLEND=5 (SRCALPHA)**,
  **20/DESTBLEND=6 (INVSRCALPHA)**. The legacy texture-map-blend path sets
  **21/TEXTUREMAPBLEND=2 (MODULATE)**. The texture-stage path sets COLOROP and
  ALPHAOP to **SELECTARG1**, with ARG1=TEXTURE (stage calls at
  `0x417fb1..0x417ff7`); it selects the texture color/alpha directly. Thus
  the shared port's Alpha blend agrees with the shipped blend factors. Its
  white authored diffuse makes texture selection and modulation equivalent
  here, but this audit does not prove the shared rasterizer matches the device.
- **Lit vs self-lit:** **Unlit / self-lit.** The animator never folds ambient
  light into vertex color and never zeroes the material; it draws the imagery's
  authored verts/texture directly (materials are neutral white, §4). Color is
  literal from the sprite. Classify **Unlit** (NOMENCLATURE §4).
- **Depth / Z:** **TestNoWrite when retail Z is enabled**; depth writes remain
  disabled. See the retail global gate above rather than assuming every device
  configuration enables Z. `0x4f44b0` confirms the snapshot patch below. `TFizzleAnimator::RefreshZBuffer`
  (`:12504-12516`) restores scene Z over a fixed **300×300 px** patch centered on
  the projected effect origin so the no-depth-write puffs composite correctly.
  (Note: the per-particle `TParticleSystem::RefreshZBuffer` at
  `effectcomp.cpp:1111-1131` is **not** used — the animator overrides it with the
  single 300×300 patch.)
- **Orientation:** **Authored mesh with source rotations.** The shipped mesh
  vertices have Z=0 (an XY quad); `RotateX(-π/2)`
  (`effectcomp.cpp:1088`) therefore tips that mesh upright. Preserve animated
  `RotateZ(rot.z)` before the X tip and static `RotateZ(-π/4)` after it. The
  X-tip call alone does not imply a flat ground billboard.
- **Per-quad transform:** full matrix (`OBJ3D_MATRIX`): rotation (per-particle
  `rot.z` spin + the −π/2 tip + −π/4 static), uniform scale (the grow/shrink
  envelope, ×1.5 on flicker frames), translation (the particle position with
  raw local Z in retail; absolute-position callers alone apply `FIX_Z_VALUE`).
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

**Retail has an effect-owned `FAIL` cue; the snapshot does not.** The effect
factory `0x4f9f50`, at `0x4f9fca` onward, looks up `"FAIL"` (string VA
`0x5e104c`, sound-manager lookup `0x49c430`) and, when present, calls sound
playback `0x49b990` with volume 127, flag 1, the effect position, and distances
80/700. Method `0x4f3f10` contains a further lookup/load/play path for the same
cue. Its association must be read from the effect vtable, not the decompiler's
misleading ownership label in `cls_0x5ad9d4.cpp`.

The snapshot `TFizzleEffect`/`TFizzleAnimator` and `SPELL "Fizzle"` have no sound
call/directive. Audio playback and duplicate-call/lifetime semantics have not
been tested in the port or the recorded software reference. The old speculative
`TAmbSoundEffect` identification is superseded by these executable call sites.

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

- **13.0 Retail x87 growth threshold corrected.** Retail growth uses FADD then
  **FST (without pop)** at `0x4f42ac`, and compares the still-extended result
  against max scale at `0x4f42c7`. Previously, both port paths compared a rounded
  float. With prior `.2f`, increment `.05f`, max `.25f`, the extended sum is
  `0.2500000037252903 > 0.25`; the stored float is exactly `0.25`. The old port
  could stay in the grow phase one extra tick. Canonical and shim now compute
  the sum in double, store rounded float scales, and compare the unrounded sum.
  Double represents the sum of these nearby float operands exactly; this is a
  focused reproduction of the shipped threshold, not a global x87 emulation.
  It intentionally differs from comparing after the snapshot's C++ float
  assignment on a modern compiler. Constants, physics, tint and random calls
  are unchanged. Shrink also retains its intermediate through the zero
  comparison at `0x4f4305`; the examined bounded scale/step values do not create
  the same positive-peak boundary problem there.

  Validation: `research/fizzle-retail-numeric-validation.log` in the retail lab
  records **71 independently generated exact-rational growth boundary cases**
  spanning every max-scale pick 5–25 and `.2f + .05f` versus `.25f`,
  **independent lifecycle assertions** covering final motion/spin/flicker,
  next-tick reaping, effect death and no additional random draw in both paths,
  **900 multi-rate state comparisons** (100 seeds, 24/30/60/144 Hz,
  .5/1/1.5 seconds), and syntax checks of both modified translation units.
  `check_fizzle_state.py` extracts the actual production simulation bodies;
  its boundary oracle uses Python `Fraction` of decoded float values, preserving
  retail's unrounded comparison rather than expecting snapshot-store behavior.
- **13.1 Retail simulation recovered; rendering acceptance open.** The old claim
  that the animator body was unavailable is superseded (§2). Retail local Z
  stays raw for `abs_pos=false`; the snapshot converted it unconditionally.
  Matrix calls and blend factors are now audited, but texture filtering,
  rasterization, device capability gates, shared projection and real-map
  depth/occlusion are not established by this instruction trace.
- **13.1a Sampling and random streams.** Native recording is 70 Hz; distinct
  images are drawn frames, not simulation ticks. The recorded debug header
  reports roughly 24.4 update / 14.5 render rates. Fifteen distinct active images
  over 1.0713 seconds is consistent with drawing near 14 Hz and does not prove a
  slower emitter. A second retail burst has 30 distinct active images over
  1.228 s with exact matched ground and ambient, further illustrating sampling
  and random lifetime variation. This is not a reason to change emitter
  constants. The interpretation of the two counters is supported by snapshot
  `screen.cpp:615–715` (total updates versus frames actually drawn). Retail's
  MSVC random generator differs from the Mac CRT;
  equal seed values do not guarantee matched particles. The 100-seed source-state
  test verifies canonical/shim agreement only. Retail random-state capture or a
  dedicated retail-compatible RNG fixture would be needed for exact trajectories.
- **13.2 Dead spawn lines.** At spawn, `p.rot.z = random(0,359)` is written then
  immediately overwritten by `p.rot.x = p.rot.y = p.rot.z = 0.0f` on the next line
  (`effect_old.cpp:12401-12402`) — so the initial random spin **never takes
  effect**; every particle starts at `rot.z = 0` and accumulates only via
  `temp.y`. Preserve the random draw to retain RNG ordering, then overwrite
  the rotation with zero; the random value must not become a visible spin.
- **13.3 `life_span` overload.** `life_span` is both the `TParticleSystem` death
  threshold (compared against `life`) AND the animator's phase tag (100/200/0).
  The initial value 100 is a phase tag, not a 100-tick lifetime — the real
  lifetime is the grow+shrink scale cycle (§6.1 note, §6.2). A reconstruction that
  reads `life_span=100` as "lives 100 ticks" would be wrong.
- **13.4 Effective lifetime is data-dependent and discrete.** Each particle
  grows through the strict peak threshold, then shrinks to zero; the zero-scale
  phase still spins/flickers and is reaped next tick. For example, stored
  max-scale picks 5 and 25 reach zero on particle ticks 4 and 18 respectively,
  and are reaped on ticks 5 and 19. Picks 10/15/20 reach zero on ticks 7/11/15.
  Counting `max/.05 + max/.02` alone misses strict comparisons and float storage.
  Staggered births over ticks 1–14 produce a burst lasting roughly 30 simulation
  ticks, with actual duration depending on random max scales and birth order.
  Recorded visible duration also depends on render sampling and excludes the
  zero-scale tail; it is not an exact simulation-clock measurement.
- **13.5 `facing` arg.** Each system's `Init` is called with a default `S3DPoint s`
  (zero) and the 5th `facing_angle` arg defaulting to 0 (`effect_old.cpp:12354-
  12356`; `Init` `effectcomp.cpp:1031`), so the Render `RotateZ(facing)` is a
  no-op for Fizzle. Reconstruct with facing = 0.
- **13.6 Retail audio cue confirmed in code, playback pending.** `FAIL` is
  absent from the snapshot but present in shipped effect creation/method code
  (§11). Its audible runtime behavior and port implementation remain open.

---

## 14. Reconstruction burndown

Checked items describe the source-backed preview implementation. Retail visual
acceptance remains pending. A complete marker-free editor burst is recorded in
`/Users/benjamincooley/RevenantRetailLab/captures/runs/sw-burndown-06-fizzle`
(first native frame 525, last 599, 15 distinct active images; blank tail 9.78 s).
Console coordinates are `(10000,10000,16)`. The `TFizzleEffect_SINGLE_BURST`
preview disables harness retriggering only, preserving the existing preview.
Temporal onset alignment, RNG/device caveats, repeated A/B review, real spell trigger
and scene-depth checks still gate acceptance. `source_behavior_audited=true`
records evidence completion; ledger gates `visual_fidelity=false`,
`runtime_trigger_integration=false` and `accepted=false` remain appropriate.
The paired media generated before the numerical correction retain their prior
binary hash and must not be treated as validation of the new threshold.

The post-correction build and 55 integrated particle/host tests passed. Its
paired elapsed-time videos and focused lifecycle sheets are retained in
`captures/ab/sw-burndown-06-fizzle-single-burst-v2/` and
`captures/ab/sw-burndown-08-fizzle-cached-port-v2/` under the lab. Both native
and port background checks are exactly zero below the recorded FPS band;
no position, scale or phase fitting was used. The second comparison reuses
the same 150 hash-checked port frames offline and completed in 1.57 seconds
without guest calls. The fixed port seed has 0.933 seconds of visible pixels
and a 4.03-second blank tail; native bursts have 1.071/1.228 seconds of visible
pixels with different RNG streams. This establishes repeatable isolated
capture/review, while exact visual and real failed-cast acceptance remain open.

```
- [x] Load Magic\Fizzle.I3D (snapshot 9,980 B / shipped 8,364 B — payload
      identical). Address its 3 STILL textured-quad sub-objects: 0=box01(blue),
      1=box02(purple/violet), 2=box03(red/magenta), each a 32x32 ARGB4444 dust
      sprite (white core → colored halo → transparent edge). NO procedural dust
      stand-in. (§4)
- [x] Three particle systems (blue/red/purple), each drawing its sub-object; new
      particles assigned to a random system (random(1,3)). (§4, §6.2)
- [x] Emit = fixed-duration BURST: ~1.5 particles/tick (DUST_ADD) over ticks 1–14
      (strict frame_count < DUST_FRAME), 20 total; effect self-kills when emission is over
      AND every particle's scale cycle has finished. (§5, §6.2)
- [x] Emit origin = caster (player) world pos; particles seed at local
      (±15, ±15, 70..130) wu — a small horizontal scatter, 70-130 wu up. (§5)
- [x] Particle motion: fall straight down, vel=(0,0,-random(5,45)*0.1),
      constant velocity (acc=1.0, no gravity/drag). (§5, §6.1)
- [x] Per-particle SCALE state machine (the identity): start scl=0, grow +0.05/tick
      until scl > temp.z (max 0.05..0.25), then shrink -0.02/tick to 0, then die.
      Asymmetric pop-in / slow-out triangle envelope. (§6.2, §13.4)
- [x] Per-particle in-plane SPIN: rot.z += temp.y each tick, temp.y=random(-15,15)
      deg/tick; wrap [0,360). (§6.2)
- [x] FLICKER: re-roll random(0,1) each tick; on set frames multiply that frame's
      render scale by 1.5 (a per-frame twinkle). (§6.2, §7)
- [x] Core geometry: one textured quad per live particle, full per-particle matrix
      = spin(rot.z) ∘ tip(rot.x=-π/2) ∘ static(rot.z=-π/4) ∘ scale ∘
      translate(local particle pos), then owner transform. (§7)
- [x] Orientation preserves authored XY corners tipped upright by rot.x=-π/2,
      rather than replacing them with a ground billboard. (§7)
- [x] Blend = Alpha (SetBlendState: MODULATE, SRC_ALPHA/INV_SRC_ALPHA — NOT
      additive); lit-mode = Unlit (asset color, neutral-white material, no tint,
      uses the sprite's per-pixel alpha); depth = TestNoWrite. (§7)
- [x] Texture animation = NONE (single-still sprites, no UV scroll, no flipbook). (§8)
- [x] Associated dynamic light = NONE. (§9)
- [x] Color from the 3 Fizzle.I3D sprites = blue (17,34,136) / purple (85,17,136)
      / magenta (136,17,119), bright white cores. Cloud = a mix of all three.
      Pale/gray ⇒ stand-in / wrong blend / lost alpha. (§10)
- [x] Sub-effects spawned = NONE. (§6)
- [ ] Shared projection/depth behavior matches retail local/absolute Z rules, including
      real-map occlusion; the preview geometry check does not prove this. (§7)
- [ ] RefreshZBuffer: restore scene Z over a single 300x300 px patch centered on
      the projected effect origin (animator override; per-particle version unused). (§7)
- [ ] Retail FAIL sound playback / lifetime semantics, absent in snapshot. (§11, §13.6)
- [x] Match retail x87 growth-threshold comparison without changing constants;
      exact-rational numerical cases plus multi-rate state checks passed. (§13.0)
- [ ] Trigger wiring: cast on spell failure via TPlayer::InvokeQuickSpell →
      CastByName("Fizzle") (empty slot / missing talismans / invalid combo),
      player.cpp:547-574; spell.def:253-263. (§12)
- [x] Initial random rot.z has no visible effect, but preserve its RNG draw
      before overwriting rotation with zero (§13.2); do NOT treat
      life_span=100 as a 100-tick lifetime (it is a phase tag) (§13.3).
```

**Definition of done:** a failed spell cast produces a brief (~30-tick) puff of
20 small blue/purple/magenta dust quads at the caster's upper body, each using the authored
quad and full source rotation chain, popping in (fast grow) and fading out (slow shrink)
while spinning and twinkling, alpha-blended and self-lit from the three authored
sprites, with no light and the shipped FAIL cue, and the effect dies after the
last zero-scale particle is reaped on the next simulation tick.


## 2026-10-07 thin-runtime original-executable A/B supplement

The retained source-only audits above are now supplemented by executable
evidence in `recon/retail_asm/runtime/effects/fizzle-frontend-ab/manifest.json`
and `tools/retail_runtime/fizzle_probe.py`. Original retail SHA256
`28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5` runs
Fizzle Initialize `0x4f3f90`, Animate `0x4f4050`, Render `0x4f4440`, original
particle Init/Animate/Render `0x50c170`/`0x50c1b0`/`0x50c220`, matrix code and
software raster. The exact shipped three meshes and ARGB4444 textures are used.

Forty frames replayed twice match current compiled production
`TFizzleEffect::TickAndSubmitForTest` in every used particle's float32 state,
phase, flicker, emission credit and alive state. All 40 paired RGB565 images
have zero differing pixels; 27 distinct retail image hashes and an explicit
viewport-bound check ensure this is not an empty/clipped capture. The original
performs 425 RNG draws, emits 20 particles, has the final zero-scale update on
tick 28 and reaps/kills on tick 29. Native range-RNG results are observed without
replacement and fed to the port with strict range/order validation. This common
input provider resolves the earlier unsynchronized-stream limitation for this
bounded comparison; matching libc seeds alone still does not do so.

A significant snapshot/retail correction: original Fizzle calls
`TParticleSystem::Render(flicker=1, abs_pos=0)`. The shipped generic renderer
only applies FIX_Z_VALUE in its nonzero-abs_pos branch
`0x50c392..0x50c3b5`. Its local Fizzle particle Z stays raw. The snapshot's
unconditional FIX_Z_VALUE statement is not authoritative for this call path.
The current production Fizzle's raw local Z matches the executable; applying
a conversion here would introduce a false discrepancy. No production effect
change was necessary.

This is isolated effect-output parity using original software projection and
rasterization, explicit owner/base-animator/imagery/extents boundaries, identity
owner placement and a selected camera. It does not prove natural failed-spell
triggering, map occlusion/transforms, spell light/audio, normal owner removal
or modern GPU rendering. The normal fast validation route remains the thin
runtime; targeted DOSBox-X device checks can classify software-renderer issues
when needed. Those integration/renderer gates remain open.
