# I22 TIcedEffect — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | I22 |
| **Class(es)** | `TIcedEffect` (object, shell) + `TIcedAnimator` (visual + state machine). Posted as a sub-effect by `TIceBoltAnimator` (I21) and — in retail only — `TStormAnimator`. |
| **Status** | forensics-complete (see §13 for the genuine unknowns) |
| **Author / Date** | vfx-forensics-agent / 2026-05-19 |
| **Family** | ice |
| **Draws** | I3D mesh — sub-objects of `Magic\iced.I3D`. **Crystal phase:** progressively-revealed facet meshes (`face01..face43` + `icicle01..06`) forming a crystal shell over the victim. **Shatter phase:** up to `MAX_ICED_CHUNKS=30` instances of the single `cube` sub-mesh tumbling/bouncing under gravity. Composite + custom-logic. |
| **Archetype(s)** | (B) I3D-geometry-with-transforms, (F2) custom procedural logic + a 3-phase state machine, (E-ish) chunk "particles" that are full mesh instances with gravity/bounce. Character-attached status overlay. NO associated dynamic light (§9). NO texture animation (§8). NO audio (§11). |

---

## 1. Summary

"Iced" is the **frozen-status overlay** that the Ice Bolt spell (I21) and the
Ice Storm post onto an enemy: a big ice crystal forms over a paralysed character,
holds for ~10 s, then **shatters into tumbling, bouncing ice chunks** that fall
under gravity, bounce 2–3 times off the floor, and vanish. It is the memorable
payoff of the frost spells. It is **not a free-standing world spawn** — it is
attached to a specific victim (`icedchar`), freezes them (`SetParalize(true)` +
`SetIced(true)`) for its hold duration, and un-freezes them at shatter. The
`TIcedEffect` object itself is an empty shell; **all behaviour lives in
`TIcedAnimator`**, a 3-phase state machine (crystal-hold → shatter-init →
chunk-tumble). Every visible polygon is a transformed sub-object of
`Magic\iced.I3D`; the ice-blue/white colour is baked into that asset's textures,
drawn self-lit under Alpha blend. There is no per-vertex colour, no light, no
audio, no texture animation in the pre-release source.

---

## 2. Sources & evidence

- **Retail decomp:** NONE for a standalone class.
  `recon/mappings/TIcedEffect_cls_0x5aaf28_note.yaml` records that the `"Iced"`
  effect-name string (`.rdata 005e1184`) has **two XREFs** —
  `cls_0x5aaf28` (the confirmed-candidate **TIceBoltEffect**, I21) and
  `cls_0x5ab460` (the **TStormAnimator** candidate). The note concludes there is
  **no standalone retail `TIcedEffect` class**: "Iced" is a status tag *posted by*
  those two effects. **This is true in retail but does NOT match the pre-release**,
  which DOES define a `TIcedEffect`/`TIcedAnimator` pair (see §13.1 — the
  INVENTORY "(no class)" note is reconciled there).
- **Pre-release (authoritative):** `src/effect_old.cpp:8716-8985` —
  - Constants `:8719-8721` (`ICED_LENGTH`, `ICED_DURATION`, `ICED_CHUNK_GRAVITY`).
  - `TIcedEffect` (object shell): `DEFINE_BUILDER("Iced", TIcedEffect)` `:8723`;
    `Initialize()` empty `:8726-8728`; `Pulse()` → `TEffect::Pulse()` `:8730-8733`.
  - `TIcedAnimator`: `REGISTER_3DANIMATOR("Iced", …)` `:8739`; `Initialize`
    `:8749-8756`; `InitIced(iceme)` `:8758-8768`; `Animate(bool)` `:8777-8891`
    (the crystal-hold → shatter → tumble/bounce state machine); `Render`
    `:8900-8953`; `RefreshZBuffer` `:8955-8985`.
  - Dev comment `:8744-8746`: "a big crystal thing over a character who is
    disabled, then it shatters."
- **Existing port shell:** none in `src/effect.cpp`. Only the class *declarations*
  in `src/effect.h:1549-1600` (`TIcedEffect` `:1555-1565`, `TIcedAnimator`
  `:1574-1600`, with `MAX_ICED_CHUNKS` `#define` at `:1573` and the per-chunk
  member arrays `:1577-1583`).
- **Poster (the coupling source):** `TIceBoltAnimator::Animate` `:8409-8450`
  spawns "Iced" and calls `InitIced` (the posting handshake — see §6.6/§12).
  Cross-references the companion doc `docs/vfx/forensics/I21_TIceBoltEffect.md`
  §6.6/§6.7 — **don't duplicate the beam; this doc owns the overlay/shatter.**
- **Sister effect consulted:** `TQuicksandAnimator` (`effect_old.cpp:8990+`,
  posting site `:9576-9615`) — the same "find characters in range → spawn an
  attached status effect → `InitXxx(victim)`" pattern, and the same
  `SetParalize`/`Stop()`/`IsDead`/`frameon==DURATION` lifecycle scaffolding.
  Confirms the status-overlay archetype. (Note `:9601` literally reuses the
  `PTIcedEffect` type name when spawning a Quicksand — a copy-paste tell, not a
  shared class.)
- **Source-of-truth ranking:** pre-release `effect_old.cpp` is authoritative for
  **all** behaviour/constants/assets — retail has no recovered TIcedEffect body,
  only the two posting XREFs. No behaviour to diverge against; the only
  reconciliation is "retail folds Iced into the posters; pre-release has a real
  class" (§13.1).

---

## 3. Constants

All from `src/effect_old.cpp` / `src/effect.h` unless noted. "wu" = world units.
`random(a,b)` is **inclusive on both ends** (`src/revutils.cpp:1597-1613`:
`rand() % (max-min+1) + min`). The animator is **ungated** in the original (runs
once per `Animate`/render frame); per NOMENCLATURE §6 the port must gate to 24 Hz
so timing/velocity/gravity don't scale with framerate. Note: in this effect a
"frame" *is* the sim unit — `ICED_DURATION` is defined as `24 * ICED_LENGTH`,
i.e. 24 frames/second is the intended tick rate.

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| `ICED_LENGTH` | 10 | seconds (hold) | effect_old.cpp:8719 | yes |
| `ICED_DURATION` | `24 * ICED_LENGTH` = **240** | frames (≈10 s @ 24 Hz) | effect_old.cpp:8720 | yes — the master clock; phase boundary |
| `ICED_CHUNK_GRAVITY` | 0.25 | wu/frame² (z accel) | effect_old.cpp:8721 | yes |
| `MAX_ICED_CHUNKS` | 30 | count | effect.h:1573 | yes |
| crystal reveal cap | `min(frameon*4, 50)` | sub-object index | effect_old.cpp:8909 | yes — # of crystal sub-objects drawn grows 4/frame, capped at 49 (`i < 50`) |
| crystal spin `angle` | `random(0,359) * TORADIAN` | rad | effect_old.cpp:8753 | yes — fixed at Initialize, one static Z-spin for the whole crystal |
| `TORADIAN` | π/180 | rad/deg | revdefs.h:25 | yes |
| `M_2PI` | 2π | rad | revdefs.h:24 | yes — angular wrap for chunk spin |
| **chunk velocity x** | `c*random(5,10)/10 + random(-5,5)/10` = `c*[0.5..1.0] + [-0.5..0.5]` | wu/frame | effect_old.cpp:8813 | yes (`c`=±1 quadrant sign, §6.2) |
| **chunk velocity y** | `d*random(5,10)/10 + random(-5,5)/10` = `d*[0.5..1.0] + [-0.5..0.5]` | wu/frame | effect_old.cpp:8814 | yes (`d`=±1 quadrant sign) |
| **chunk velocity z** | `random(0,10)/10` = `[0.0..1.0]` | wu/frame (upward) | effect_old.cpp:8815 | yes |
| **chunk position x** | `c*random(20,40) + random(-20,20)` = `c*[20..40] + [-20..20]` | wu | effect_old.cpp:8817 | yes |
| **chunk position y** | `d*random(20,40) + random(-20,20)` = `d*[20..40] + [-20..20]` | wu | effect_old.cpp:8818 | yes |
| **chunk position z** | `(i/6)*25` (integer div) | wu | effect_old.cpp:8819 | yes — rows of 6, each row +25 wu (see §6.2; comment says "rows of 4" but math = 6) |
| **chunk ang. vel x,y** (`w[i].x=w[i].y`) | `random(0,25)/100` = `[0.0..0.25]` | rad/frame | effect_old.cpp:8821 | yes |
| chunk ang. vel z (`w[i].z`) | **commented out** `random(-25,25)/100` | rad/frame | effect_old.cpp:8822 | yes — DEAD: `w[i].z` left uninitialised → see §13.3 |
| chunk ang. pos x,y (`t[i].x/y`) | **commented out** `random(0,6)` | rad | effect_old.cpp:8824-8825 | yes — DEAD: `t[i].x/y` start at 0 (default-init) |
| chunk ang. pos z (`t[i].z`) | `atan2(v[i].y, v[i].x)` | rad | effect_old.cpp:8826 | yes — initial spin oriented to travel direction |
| **chunk scale** (50% of chunks) | `random(25,75)/100` per axis = `[0.25..0.75]` | scale | effect_old.cpp:8828-8833 | yes — `if(random(0,1))` branch (large chunks) |
| **chunk scale** (other 50%) | `random(10,15)/100` per axis = `[0.10..0.15]` | scale | effect_old.cpp:8834-8839 | yes — else branch (small chunks/shards) |
| **chunk life** (`l[i]`) | `random(2,3)` | bounces remaining | effect_old.cpp:8841 | yes — chunk dies after 2 or 3 floor bounces |
| **bounce floor threshold** | `p[i].z <= 16` | wu | effect_old.cpp:8876 | yes (comment "fix this") |
| bounce reset height | `p[i].z = 20` | wu | effect_old.cpp:8878 | yes — snap above floor on bounce |
| **bounce z reflection** | `v[i].z *= -0.5` | factor | effect_old.cpp:8879 | yes — damped bounce (loses half speed) |
| **bounce spin boost** | `w[i].{x,y,z} *= 2.0` | factor | effect_old.cpp:8880-8882 | yes — spin doubles each bounce |
| RefreshZBuffer crystal patch | 150 × 200 px (offset −50 in y) | px | effect_old.cpp:8962-8964 | yes |
| RefreshZBuffer chunk patch | 70 × 70 px (offset −10 in y) | px | effect_old.cpp:8968-8982 | yes |

---

## 4. Assets

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| iced mesh | `legacy/Imagery/Magic/iced.I3D` | 94 888 B | the crystal shell, the icicles, and the shatter chunk | Registry name `"Iced"`, `Class.Def:2056` (`0xfacecabb`); resolved via the EFFECT object class → `T3DImagery`; the animator addresses sub-objects with `GetObject(idx)` (`effect_old.cpp:8911,8923`) |

**Same file, three registry aliases** (all `Magic\iced.i3d`, `Class.Def`):
`"Iced"` `:2056` (`0xfacecabb`), `"Icedsparks"` `:2057` (`0xacabfada`),
`"Snow"` `:2061` (`0xfacedbed`). The Iced effect uses the `"Iced"` registration.

**`iced.I3D` sub-object enumeration (verified by offset-ordered string-dump of the
file; file order = `GetObject` index — `S3DAnimObj* GetObject(int objnum)`,
`src/3dimage.h:464`):**

- **States:** `STILL` (single state; the pre-release animator never switches
  state — it transforms sub-objects imperatively).
- **Sub-objects (file order = index):**

  | index | name | role |
  |-------|------|------|
  | **0** | `cube` | **the shatter chunk** — drawn 30× (one per `MAX_ICED_CHUNKS`) in the tumble phase via `GetObject(0)`, each re-scaled/rotated/translated (`effect_old.cpp:8923`). |
  | **1..43** | `face01` … `face43` | **the crystal shell facets** — 43 faceted pieces that compose the big ice block over the victim. Revealed progressively in the hold phase via `GetObject(i)`, `i=1..min(frameon*4,50)` (`:8909-8911`). |
  | **44..49** | `icicle01` … `icicle06` | 6 **icicles** — the trailing crystal sub-objects; revealed once `frameon*4` exceeds 43 (so they appear last, completing the crystal). Drawn by the same hold-phase loop. |

  (String offsets confirming order: `cube`@172, `face01`@220 … `face43`@2236,
  `icicle01`@2332 … `icicle06`@2524.)

- **Textures:** stored **inline** in the `.I3D` (no external `.bmp`/`.tga`
  filenames in the string dump — same as `icebolt.I3D`, see I21 §4). The
  ice-blue/white look is baked into these embedded textures + per-sub-object
  materials. Transparency follows the engine's black-key convention (knowledge
  02 §4): black background → keyed transparent.

**Stand-in guard:** This effect is **entirely asset-driven**. The crystal is the
43 `face*` + 6 `icicle*` sub-objects; every shatter chunk is the **single `cube`
sub-mesh** re-instanced 30× with different transforms (NOT 30 distinct meshes,
NOT procedural cubes, NOT billboards). A reconstruction that draws procedural
crystals/cubes/gradients instead of loading and transforming `iced.I3D`'s
sub-objects is a stand-in and is wrong (AGENT_GUIDE §4.2.1; memory
`feedback-no-standins`). If the chunks look like flat quads, the `cube` sub-mesh
wasn't loaded.

---

## 5. Spawn & emit

- **Trigger semantics:** **attached status overlay**, not a world emitter. Spawned
  by a poster (§12) at the victim's position; lives a fixed hold of
  `ICED_DURATION=240` frames (≈10 s), then shatters and tumbles until all 30
  chunks exhaust their 2–3 bounce lives, then `KillThisEffect()` (`:8887-8890`).
  So total life = ~10 s hold + a short tumble tail (a few seconds).
- **Count per trigger:** one crystal overlay per victim. The crystal is 43 facets
  + 6 icicles = up to 49 sub-objects shown; the shatter spawns
  `MAX_ICED_CHUNKS=30` chunks.
- **Initial direction / distribution:** N/A for the crystal (it's a static mesh
  shell at the victim). For the shatter chunks, see §6.2 — chunks are scattered in
  **4 XY quadrants** around the victim and given outward+upward velocities.
- **Emit anchor convention:** the effect ORIGINATES at the **victim's position**.
  The poster sets `def.pos = victim->GetPos()` when spawning the Iced effect
  (`effect_old.cpp:8437-8438`). All crystal facets and all chunks are positioned
  in the effect's **local space** (origin = victim pos); the engine places the
  object in world. Chunk world positions for the Z-restore patch are
  `WorldToScreen(victim_pos) + WorldToScreen(chunk_local_pos)` (`:8974-8981`).
- **Coordinate space:** **Local** — every `obj->matrix` is rebuilt from identity
  each draw (`D3DMATRIXClear`, `:8913,8930`); geometry is authored relative to the
  effect's own (victim) position.
- **Spread / jitter:** crystal = none (fixed mesh). Chunk scatter: position
  `±[20..40]` in the quadrant direction plus `±20` jitter per XY axis, z stacked
  in rows of 6 spaced 25 wu (§6.2); velocities outward `±[0.5..1.0]` + `±0.5`
  jitter, upward `[0..1]` (§3).

### Spatial diagram (chunk scatter at shatter, top-down XY + side ZY)

```
 TOP-DOWN (XY), victim at origin; chunks fill all 4 quadrants in a (c,d)=±1 cycle
   +y
    │   Q2 (c=-1,d=+1)    Q1 (c=+1,d=+1)
    │      · ·  ·            ·  · ·
    │     ·   ·               ·   ·
 ───┼────────────●──────────────────── +x   ● = victim pos (effect origin)
    │     ·   ·   (origin)     ·   ·
    │      · ·  ·            ·  · ·
   −y   Q3 (c=-1,d=-1)    Q4 (c=+1,d=-1)
        each chunk: pos = (c·[20..40]±20, d·[20..40]±20),  vel outward + up

 SIDE (ZY) at shatter — chunks pre-stacked in 5 rows of 6, 25 wu apart:
   z
  100┤ · · · · · ·     row i=24..29  (z=(i/6)*25)
   75┤ · · · · · ·     row i=18..23
   50┤ · · · · · ·     row i=12..17
   25┤ · · · · · ·     row i= 6..11
    0┤ · · · · · ·     row i= 0.. 5
    └──────────────► y     then gravity pulls them down; bounce at z<=16
```

---

## 6. Behavior & per-frame logic

`TIcedEffect` (object) is a **pure shell**: `Initialize()` is empty
(`:8726-8728`); `Pulse()` just chains `TEffect::Pulse()` (`:8730-8733`). All logic
is in `TIcedAnimator`. The state machine is driven by `frameon`, incremented once
per `Animate` (`:8783`), against the single boundary `ICED_DURATION=240`.

### 6.0 Setup — `Initialize` + `InitIced` (cite `:8749-8768`)

```
Initialize():                                  // :8749-8756
    T3DAnimator::Initialize()
    frameon = 0
    angle = random(0,359) * TORADIAN           // one fixed Z-spin for the crystal
    icedchar = null
    donebouncing = 0

InitIced(iceme):                               // :8758-8768  — called by the poster
    icedchar = iceme
    if icedchar:
        icedchar->SetParalize(true)            // OF_PARALIZE (1<<23): freeze mid-animation
        icedchar->SetIced(true)                // OF_ICED (1<<25): marks "still iced"
        // SetFlag(OF_DISABLED) and SetFlag(OF_ICED) are COMMENTED OUT (:8763,8766) — DEAD
```

`SetIced(true)` simply sets `OF_ICED` (`object.h:911`), and `IsIced()` reads it
(`object.h:909`) — so the commented `SetFlag(OF_ICED)` lines are **redundant
duplicates** of the live `SetIced` call, not missing functionality (§13.2).

### 6.1 The 3-phase state machine (cite `:8777-8891`)

```
Animate(draw):
    T3DAnimator::Animate(draw)
    inst->SetCommandDone(false)                 // :8780 (keeps the effect pulsing)
    frameon++                                    // :8783

    // ── PHASE 1: CRYSTAL HOLD (frameon < ICED_DURATION) ──
    if icedchar:                                 // :8785-8792
        icedchar->Stop()                         // halt victim's action every frame (frozen)
        if !icedchar->IsIced() and frameon < ICED_DURATION:
            frameon = ICED_DURATION              // EARLY-EXIT: victim was un-iced → jump to shatter
    if icedchar->IsDead() and frameon < ICED_DURATION:   // :8794-8797
        frameon = ICED_DURATION                  // EARLY-EXIT: victim died → jump to shatter

    // ── PHASE 2: SHATTER INIT (exactly when frameon == ICED_DURATION) ──
    if frameon == ICED_DURATION:                 // :8798-8847
        if icedchar: icedchar->SetParalize(false)   // UN-FREEZE the (surviving) victim
        spawn MAX_ICED_CHUNKS chunks (see §6.2)

    // ── PHASE 3: TUMBLE + BOUNCE (frameon > ICED_DURATION) ──
    if frameon > ICED_DURATION:                  // :8848-8886
        donebouncing = 1
        for each chunk i:
            if l[i] == 0: continue               // dead chunk
            donebouncing = 0                      // at least one chunk still alive
            p[i] += v[i]                          // integrate position
            v[i].z -= ICED_CHUNK_GRAVITY          // gravity (0.25)
            t[i] += w[i]                          // integrate spin; wrap each axis into [0,2π]
            if p[i].z <= 16:                      // BOUNCE off floor
                p[i].z = 20
                v[i].z *= -0.5                    // damped reflection
                w[i].{x,y,z} *= 2.0               // spin doubles
                l[i]--                            // one bounce consumed

    if donebouncing:                              // :8887-8890  — all chunks dead
        ((PTEffect)inst)->KillThisEffect()
```

### 6.2 Shatter-init: chunk spawn (cite `:8806-8847`)

Runs once, on the frame `frameon == ICED_DURATION`. `a`,`b` start 0 and toggle to
walk the 4 XY quadrants; `c=(a?1:-1)`, `d=(b?1:-1)` are the quadrant signs.

```
a = b = 0
for i in 0..MAX_ICED_CHUNKS-1 (0..29):
    c = (a ? 1 : -1);  d = (b ? 1 : -1)              // quadrant signs ±1

    v[i].x = c*random(5,10)/10 + random(-5,5)/10     // outward + jitter
    v[i].y = d*random(5,10)/10 + random(-5,5)/10
    v[i].z = random(0,10)/10                          // upward [0..1]

    p[i].x = c*random(20,40) + random(-20,20)         // scatter in quadrant
    p[i].y = d*random(20,40) + random(-20,20)
    p[i].z = (i/6)*25                                 // rows of 6, 25 wu apart

    w[i].x = w[i].y = random(0,25)/100                // angular velocity x,y
    // w[i].z   left UNINITIALISED (commented out, :8822) — see §13.3
    // t[i].x/y left at 0 (commented out, :8824-8825)
    t[i].z = atan2(v[i].y, v[i].x)                    // initial spin faces travel dir

    if random(0,1):                                   // 50%: LARGE chunk
        s[i].{x,y,z} = random(25,75)/100              //   scale 0.25..0.75
    else:                                             // 50%: SMALL shard
        s[i].{x,y,z} = random(10,15)/100              //   scale 0.10..0.15

    l[i] = random(2,3)                                // 2 or 3 bounces of life

    a = 1 - a;  if (a) b = 1 - b                       // advance quadrant cycle
```

**Quadrant cycle** (the `a/b` toggle): the `(c,d)` pair walks the 4 quadrants on a
period-4 cycle — i=0→(−1,−1) Q3, i=1→(+1,+1) Q1, i=2→(−1,+1) Q2, i=3→(+1,−1) Q4,
then repeats — so the 30 chunks are spread evenly across all four XY quadrants
around the victim.

**Row spread** (`p[i].z = (i/6)*25`, integer div): chunks 0–5 at z=0, 6–11 at
z=25, 12–17 at z=50, 18–23 at z=75, 24–29 at z=100 — a 5-row vertical stack so the
shatter bursts from the whole height of the crystal, not just the base. (The code
comment `:8819` says "rows of 4" but the math gives **rows of 6** — comment is
stale; the math is authoritative — §13.5.)

### 6.3 Per-chunk tumble integration detail (cite `:8851-8885`)

Each live chunk per frame: `p += v`; `v.z -= 0.25` (gravity); spin `t += w` with
each axis wrapped into `[0, 2π]` (`:8863-8874`). On `p.z <= 16` it bounces:
snap `p.z=20`, reflect `v.z *= -0.5` (keeps half the downward speed as upward),
double all spin (`w *= 2`), decrement `l`. After `l` reaches 0 the chunk is
skipped (`:8853`) and stops drawing/integrating. When all 30 reach `l==0`,
`donebouncing` stays 1 → `KillThisEffect()`.

### 6.4 Early-exit conditions

Two ways the hold phase ends before the full 240 frames (both **snap
`frameon = ICED_DURATION`**, dropping straight into the shatter on the next check):
1. **Victim un-iced** — `!icedchar->IsIced()` (something cleared `OF_ICED`,
   e.g. a cure / the freeze wearing off elsewhere) — `:8790-8791`.
2. **Victim died** while frozen — `icedchar->IsDead()` — `:8794-8797`.
Either way the crystal shatters immediately rather than hanging on a dead/freed
victim.

### Temporal diagram (phase timeline)

```
phase:   CRYSTAL HOLD (victim paralysed, Stop() each frame)  │ SHATTER │ TUMBLE+BOUNCE      │ dead
frameon: 0 ──────────────────────────────────────────── 240 │  ==240  │ 241 ............... │
         │ crystal reveals 4 sub-objs/frame, capped at 49    │ un-     │ chunks fall (g=.25),│ KillThisEffect
         │ (full crystal by frameon≈13)                      │ freeze; │ bounce z<=16        │ when all l[i]==0
         │ EARLY-EXIT → 240 if victim un-iced or dies        │ spawn   │ (×2-3), spin doubles│
         └───────────────────────────────────────────────────┘ 30 cube│ per bounce          │
                                                                chunks  │                     │
crystal-reveal envelope (# sub-objects drawn vs frameon):
  count
   49 ┤        ●━━━━━━━━━━━━━━━ (cap min(frameon*4,50)=49)
      │      ╱
    1 ┤●━━━╱   slope = 4 sub-objects per frame
      └──┼──┼──────────────────────► frameon
         0  ~13
```

---

## 7. Rendering (original render state + geometry)

Full body `Render()` `:8900-8953`. Brackets the whole draw with
`SaveBlendState(); SetBlendState(); … RestoreBlendState();` (`:8903-8904,8950`).

- **What it draws:** an **I3D composite**, two distinct draw paths gated by
  `frameon < ICED_DURATION`:

  **Crystal hold path** (`:8907-8920`):
  ```
  for i = 1; i < min(frameon*4, 50); i++:        // reveal 4 sub-objs/frame, cap 49
      obj = GetObject(i)                          // face01..face43, then icicle01..06
      D3DMATRIXClear(&obj->matrix)
      obj->flags = OBJ3D_MATRIX
      D3DMATRIXRotateZ(&obj->matrix, angle)       // one fixed random Z-spin for whole crystal
      ResetExtents(); RenderObject(obj); UpdateExtents()
  ```
  The crystal is the loaded mesh's facets, all sharing the **same** fixed `angle`
  Z-rotation (set once at Initialize, §6.0) — so the crystal sits at a random but
  static orientation over the victim. No per-facet transform beyond that shared
  spin; the facets' relative geometry is authored into the mesh.

  **Shatter/tumble path** (`:8922-8949`):
  ```
  obj = GetObject(0)                              // the single `cube` chunk mesh
  for i = 0; i < MAX_ICED_CHUNKS; i++:
      if l[i] == 0: continue                      // skip dead chunks
      ResetExtents()
      D3DMATRIXClear(&obj->matrix); obj->flags = OBJ3D_MATRIX
      obj->scl = s[i];        D3DMATRIXScale(&obj->matrix, &obj->scl)     // per-chunk scale
      D3DMATRIXRotateX(&obj->matrix, t[i].x)                              // per-chunk spin
      D3DMATRIXRotateY(&obj->matrix, t[i].y)
      D3DMATRIXRotateZ(&obj->matrix, t[i].z)
      obj->pos = p[i];        D3DMATRIXTranslate(&obj->matrix, &obj->pos) // per-chunk position
      RenderObject(obj)
      UpdateExtents()
  ```
  Build order is **Scale · RotateX · RotateY · RotateZ · Translate** (right-mul,
  `math3d.h:9-11` convention) — the chunk is sized, spun on all three axes by its
  integrated spin `t[i]`, then placed at `p[i]`.

- **Blend mode (original):** **Alpha (modulated)** — `Render` calls
  `SetBlendState()` (`:8904`), **not** `SetAddBlendState()`. Per knowledge 03
  §1.1 / NOMENCLATURE §3, `SetBlendState()` = `D3DTBLEND_MODULATE`,
  `SRCBLEND=SRC_ALPHA`, `DESTBLEND=INV_SRC_ALPHA`, depth-write off, depth-test on
  (`effect_old.cpp:221-233`). **NOT additive** — same as IceBolt (I21 §7).
- **Lit vs self-lit:** **Unlit / self-lit, without material zeroing.**
  `Initialize` (`:8749-8756`) does **not** touch the imagery material (no
  `GetMaterial`/`SetMaterial`), and `Render` never writes `lverts[].color`. So the
  visible colour is the **mesh's authored materials × embedded textures** under
  MODULATE, modulated by the default per-vertex diffuse (white). No
  `GetAmbientLight()` mix → not scene-lit. Classify **Unlit**: the ice colour is
  literal from the asset (same lit-mode as IceBolt, I21 §7).
- **Depth / Z:** depth-test on, **depth-write off** (`SetBlendState`,
  effect_old.cpp:224-225) → **TestNoWrite**. `RefreshZBuffer` (`:8955-8985`)
  restores scene Z under the effect's screen footprint, with two patch shapes:
  - **Crystal phase:** project the victim pos, restore a **150×200 px** patch
    centred on it (offset −50 in y to cover the upward crystal) (`:8959-8964`).
  - **Tumble phase:** for each live chunk, project victim pos + chunk-local pos
    and restore a **70×70 px** patch (offset −10 in y) at each chunk
    (`:8966-8983`).
- **Orientation:** **NOT** a billboard effect — these are real 3D meshes placed by
  full matrices, NOT camera-facing quads. There is **no** `rot.x=-π/2` WorldXY tip
  and **no** ScreenAligned billboard expansion. The crystal uses a single Z-spin
  (`D3DMATRIXRotateZ(angle)`); chunks use full XYZ rotation from their integrated
  spin `t[i]`. Orientation is therefore "authored-mesh in world space, rotated by
  the per-object matrix" — there is no orientation enum to assign.
- **Per-object transform:** crystal — shared `RotateZ(angle)`. Chunk — per-chunk
  `Scale(s[i]) · RotateX/Y/Z(t[i]) · Translate(p[i])` (§ above).
- **Per-vertex color packing:** **none written by the animator** — it never sets
  `lverts[].color`/`.diffuse`. The diffuse stays at the mesh default (white); under
  MODULATE the texture/material colour passes through unchanged. (Same as IceBolt.)

---

## 8. Texture animation

- **NONE.** The animator never mutates `.tu`/`.tv`, never calls
  `ScrollTexture`/`SetTextureFrame`, and never advances a `framenum` (searched the
  full `Animate`+`Render` bodies, `:8777-8953`). The texture is **static** per
  draw. All apparent motion is **mesh transform animation** (the progressive
  reveal of crystal facets, and the per-chunk Scale/Rotate/Translate during the
  tumble), not texture animation.

---

## 9. Associated light

- **NO dynamic light in the pre-release source.** `TIcedAnimator` emits no light:
  no `AddLight`/`CreateLight`/`SLightInfo` anywhere in
  `Initialize`/`InitIced`/`Animate`/`Render`/`RefreshZBuffer` (`:8749-8985`).
- The Ice Bolt **spell.def `LIGHT` line** (`LIGHT COLOR 100,100,255 …`,
  spell.def:1223) belongs to the *poster's* spell (I21 §9) and is parsed-and-
  discarded by the pre-release parser; it does not attach to the Iced overlay.
- So the Iced overlay produces **no light**. (A reconstruction may *optionally*
  add an ice-blue glow for polish, but it is not a pre-release fact — flag if
  added, same caveat as I21 §13.5.)

---

## 10. Color

- **Source:** the **authored textures + materials of `iced.I3D`** (the crystal and
  chunk ice-blue/white look is baked into the embedded textures, §4). The animator
  writes no per-vertex colour and zeroes no material, so the colour is literally
  the asset under MODULATE/Alpha (§7). There is **no** chardata/spell-colour/tint
  path feeding this effect's mesh colour in the pre-release source.
- **Exact values:** not a code constant — it lives in the `.I3D` texture data.
  (The frost family's intended hue, ice-blue ≈ RGB `(100,100,255)` 0–255 from the
  Ice Bolt spell light, spell.def:1223, is a useful *target* if a reconstruction
  ever needs a reference colour — but it is not this effect's source.)
- **Expected visual:** cold **ice-blue / cyan-white**, translucent — the
  magic-family "cool" signature (AGENT_GUIDE §4.2.1.5). If the reconstruction
  renders pale, gray, or warm, suspect: (1) the `iced.I3D` textures weren't loaded
  (procedural stand-in — the #1 failure mode here), (2) black-key chroma miss
  eating the translucent edges, or (3) wrong blend (additive washes the asset).
- **Normalization / boosts:** none — no `NormalizeColors`/brightness math.

---

## 11. Audio coupling

- **No audio coupling found in `TIcedAnimator`** — no `PLAY(...)`, `SoundPlayer`,
  or imagery-tag sound call in `Initialize`/`InitIced`/`Animate`/`Render`
  (`:8749-8985`). The dtor is the default (effect.h:1588 `~TIcedAnimator(){Close();}`).
- The `iced.I3D` may carry per-(state,frame) `S3DTag` sounds (the data-driven
  mechanism, knowledge 02 §8), but the pre-release animator never switches imagery
  state, so any such tags would not fire. Record as "no code-driven audio"; the
  audio phase should check the `.I3D` tags directly (the shatter is an obvious
  candidate for an ice-crack/shatter SFX in the data).

---

## 12. Triggers & in-game appearance

- **Spawned by:** other effects post "Iced" as a sub-effect — it is **never cast
  directly**. Posters:
  1. **Ice Bolt (I21)** — `TIceBoltAnimator::Animate`, at the FREEZE beat
     `frameon == GROW_DURATION*2 && subspell > 1` (`effect_old.cpp:8410`). It
     finds CHARACTERs within 200 wu of the strike point
     (`FindObjectsInRange(... OBJCLASS_CHARACTER)`, `:8425`) and for each live,
     non-caster victim runs the posting handshake (§6.6 below). This is the
     **single live poster in the pre-release source.**
  2. **Ice Storm / `TStormAnimator`** (`cls_0x5ab460`) — the **second poster in
     RETAIL only** (`recon/mappings/TIcedEffect_cls_0x5aaf28_note.yaml`). There is
     no `TStormAnimator` Iced-posting site in the pre-release `effect_old.cpp`
     (the nearby `:9576-9615` site is the **Quicksand** sandswirl effect, a
     copy-paste sibling that posts "Quicksand", not "Iced"). Record as a
     retail-only second caster (§13.4).
- **Posting handshake** (cite `:8431-8448`, the canonical site):
  ```
  SObjectDef def{}; def.objclass = OBJCLASS_EFFECT; def.level = MapPane.GetMapLevel()
  def.pos      = victim->GetPos()                       // overlay sits ON the victim
  def.objtype  = EffectClass.FindObjType("Iced")
  PTIcedEffect e = MapPane.GetInstance(MapPane.NewObject(&def))   // spawn the shell
  if !e->HasAnimator(): e->CreateAnimator()                       // make the animator
  ((PTIcedAnimator)e->GetAnimator())->InitIced(victim)            // wire victim + freeze
  ```
  `InitIced` (§6.0) then `SetParalize(true)` + `SetIced(true)` on the victim. The
  Iced effect's own `Animate` calls `victim->Stop()` every hold frame to keep them
  frozen, and `SetParalize(false)` at shatter. **What un-freezes the target:**
  reaching `frameon == ICED_DURATION` (timeout) — OR an external cure clearing
  `OF_ICED` (caught by `!IsIced()` early-exit) — OR the victim dying. The
  paralyse is only lifted by the Iced effect itself (at shatter); `OF_ICED` is the
  "still iced" handshake flag the effect polls.
- **Where to see it in the original game:** cast **Ice Bolt** (talisman `CIB`, or
  Priest variant `MAK`) at an enemy with line of sight — on a hit the enemy freezes
  under this crystal, holds ~10 s, then shatters. (See I21 §12 for the beam rig.)
  **Capture rig:** character-attached / spell-cast — a target-dummy CHARACTER that
  the Iced effect attaches to (it needs a live `icedchar`, so a standalone
  no-character test will early-exit). Capture a *sequence* over the life: crystal
  forming (frameon 0–13), full crystal hold (mid), shatter instant (frameon 240),
  chunks tumbling/bouncing (240+), per memory `feedback-vfx-capture-framing`
  (motion-essential → sequence, frame to peak bbox).
- **Vestigial?** No — live status effect posted by a live spell (Ice Bolt).

---

## 13. Gaps & uncertainties

1. **INVENTORY "(no class)" reconciled.** INVENTORY I22 and
   `TIcedEffect_cls_0x5aaf28_note.yaml` say there is "no standalone TIcedEffect
   class — 'Iced' is a status tag posted by other effects." **That is true in
   RETAIL** (the two XREFs land in IceBolt + TStormAnimator) but **false for the
   pre-release**, which defines a real `TIcedEffect`/`TIcedAnimator` pair with
   `DEFINE_BUILDER("Iced", TIcedEffect)` (`effect_old.cpp:8723`) and a full
   animator body. The reconciliation: in the pre-release the *posting* is done by
   IceBolt but the *behaviour* is a genuine class; retail apparently inlined/merged
   the same behaviour into the posters (or the class survived but its body wasn't
   recovered by Ghidra). **Reconstruction should build the pre-release
   `TIcedAnimator` class** — it is the authoritative, complete spec. INVENTORY I22
   updated to point at this doc.
2. **`SetFlag(OF_ICED)` / `SetFlag(OF_DISABLED)` commented out — do-not-
   reconstruct (but harmless).** `:8763, 8766, 8787, 8789, 8803, 8804` are
   commented `SetFlag(OF_ICED/OF_DISABLED, …)` calls. The live `SetIced(true/false)`
   (`:8765, 8802` via `SetParalize`) already sets/clears `OF_ICED` internally
   (`object.h:909-911`), so these are **redundant duplicates**, not lost behaviour.
   Reconstruct the live `SetParalize`/`SetIced` only.
3. **Dead chunk-spin init — do-not-reconstruct.** `w[i].z` (angular velocity z,
   `:8822`) and `t[i].x`/`t[i].y` (initial angular position, `:8824-8825`) are
   **commented out**, so they start at 0 (member arrays are default-init). This
   means: chunks have **no z-spin** initially (only `t[i].z = atan2(...)` is set),
   and x/y spin starts at 0 then integrates from `w[i].x/y`. The bounce code still
   doubles `w[i].z` (`:8882`) and integrates `t[i].z` (`:8862`), but with `w[i].z`
   never seeded it stays 0 → no z-axis tumble accumulates. Reconstruct only the
   **live** init: `w.x=w.y=random(0,25)/100`, `w.z=0`, `t.x=t.y=0`,
   `t.z=atan2(v.y,v.x)`. (Do not "fix" it by re-enabling the commented z-spin —
   that would change the original look.)
4. **Second poster (`TStormAnimator`) is retail-only.** The Ice Storm posting of
   "Iced" exists only in retail (recon note); there is no pre-release
   `TStormAnimator` Iced-posting site. The pre-release `:9576-9615` "storm-like"
   site posts **Quicksand**, not Iced. So the second caster's exact timing/range
   are **undetermined** from the available source — but they are irrelevant to the
   Iced effect's own behaviour (the overlay/shatter is identical regardless of
   poster; only `def.pos`/`InitIced(victim)` matter). Reconstruction can wire the
   Storm poster the same way as the IceBolt poster when TStormAnimator is built.
5. **Stale code comment "rows of 4".** `:8819` comment says "spread in rows of 4,
   25 apart" but `p[i].z = (i/6)*25` with `MAX_ICED_CHUNKS=30` gives **5 rows of
   6**. The math is authoritative; the comment is stale. (Probably a leftover from
   when `MAX_ICED_CHUNKS` or the divisor was different.)
6. **Bounce threshold "fix this".** `:8876` `if (p[i].z <= 16) // fix this` —
   the floor is hardcoded at local-z 16 (with reset to 20), i.e. the chunks bounce
   off a flat plane at the effect's local z=16, **not** the actual ground height
   under each chunk. This is a known-imperfect placeholder (the dev comment flags
   it). Reconstruct as-is (flat local-z=16 bounce) for fidelity to the original
   look; a ground-aware bounce would be a deviation. Note: chunks with large `v.z`
   can fly well above before falling back; the bounce only triggers on the way
   down through z=16.
7. **`iced.I3D` per-sub-object materials / state animation.** The file has a
   `STILL` state; whether retail played any I3D state-animation instead of the
   pre-release's imperative transforms is **undetermined**. The pre-release
   imperative path (reveal facets, transform chunks) is the documented, complete
   spec.

---

## 14. Reconstruction burndown

```
- [ ] Load Magic\iced.I3D (94888 B); address its sub-objects by index:
      0 = `cube` (the shatter chunk), 1..43 = `face01..face43` (crystal facets),
      44..49 = `icicle01..icicle06` (icicles). NO procedural cubes/crystals/billboards. (§4)
- [ ] Build TIcedEffect as an empty TEffect shell (Initialize/Pulse do nothing
      but chain base Pulse); ALL behaviour lives in TIcedAnimator. (§2, §6)
- [ ] Attach to a victim character via InitIced(victim): store icedchar,
      SetParalize(true)+SetIced(true). It is a CHARACTER-ATTACHED overlay, not a
      world spawn — spawned at victim->GetPos() by the poster. (§5, §6.0, §12)
- [ ] State machine on frameon (++ per 24Hz tick), boundary ICED_DURATION=240:
      Phase1 hold (frameon<240) → Phase2 shatter-init (==240) → Phase3 tumble (>240). (§6.1)
- [ ] Phase1 CRYSTAL HOLD: each frame call victim->Stop() (keep frozen). Render
      crystal as sub-objects i=1..min(frameon*4,50) (reveal 4/frame, full by ~frame13),
      each with shared D3DMATRIXRotateZ(angle) where angle=random(0,359)*TORADIAN
      fixed at Initialize. (§6.1, §7)
- [ ] Phase1 EARLY-EXIT: if !victim->IsIced() OR victim->IsDead() while frameon<240,
      snap frameon=240 (shatter immediately). (§6.4)
- [ ] Phase2 SHATTER-INIT (frameon==240): SetParalize(false) on victim; spawn
      MAX_ICED_CHUNKS=30 chunks per the §6.2 RNG — per-axis velocity, position
      (4-quadrant a/b cycle, c·[20..40]±20), z=(i/6)*25 row-stack, ang.vel
      w.x=w.y=random(0,25)/100 (w.z=0), t.z=atan2(v.y,v.x) (t.x=t.y=0), scale
      (50% large 0.25..0.75 / 50% small 0.10..0.15), life l=random(2,3). (§6.2, §3)
- [ ] Phase3 TUMBLE: each live chunk p+=v; v.z-=ICED_CHUNK_GRAVITY(0.25); t+=w
      (wrap [0,2π]); bounce at p.z<=16 → p.z=20, v.z*=-0.5, w*=2.0, l--. When all
      l==0 → KillThisEffect(). (§6.1, §6.3)
- [ ] Chunk render: GetObject(0) `cube`, per-chunk Scale(s)·RotateX/Y/Z(t)·Translate(p). (§7)
- [ ] Blend = Alpha (SetBlendState: MODULATE, SRC_ALPHA/INV_SRC_ALPHA); depth =
      TestNoWrite; lit-mode = Unlit (no material zeroing, no per-vertex color —
      asset color passes through). (§7)
- [ ] Texture animation = NONE (no UV scroll, no flipbook); motion is geometric only. (§8)
- [ ] RefreshZBuffer: crystal phase restore 150×200px at victim screen pos (y−50);
      tumble phase restore 70×70px per live chunk at victim+chunk screen pos (y−10). (§7)
- [ ] Color: ice-blue/white from iced.I3D embedded textures; verify saturated cool
      tone (pale/gray ⇒ stand-in / chroma-key / wrong blend). (§10)
- [ ] Associated dynamic light: NONE in pre-release (optional ice-blue glow is a
      polish deviation, flag if added). (§9)
- [ ] Audio: none code-driven; check iced.I3D S3DTag sounds (esp. a shatter SFX)
      in the audio phase. (§11)
- [ ] Poster coupling: IceBolt (I21) posts Iced at its FREEZE beat — spawn "Iced",
      CreateAnimator, InitIced(victim). Wire TStormAnimator the same way when built
      (retail-only second poster). See effect_old.cpp:8431-8448. (§12)
- [ ] Do NOT reconstruct the commented-out SetFlag(OF_ICED/OF_DISABLED) duplicates,
      the commented w[i].z / t[i].x/y chunk-spin init, or "fix" the z<=16 flat-floor
      bounce — all dev-state to preserve as-is. (§13.2, §13.3, §13.6)
```
