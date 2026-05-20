# B01 TBloodEffect — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | B01 (covers B01 `TBloodEffect`, B02 `TBloodAnimator`, B03 `TBloodSystem` — they are one effect, three cooperating classes) |
| **Class(es)** | `TBloodEffect` (object) + `TBloodAnimator` (visual component) + `TBloodSystem` (the real droplet particle system, owned by the animator). `TBloodSystem` is the authoritative mechanism — see §2. |
| **Status** | forensics-complete (see §13 for the genuine unknowns) |
| **Author / Date** | vfx-forensics-agent / 2026-05-19 |
| **Family** | blood/combat |
| **Draws** | particle emitter — N world-positioned droplet billboards (`Misc\Blood.I3D` quads), each drawn TWICE per frame (Alpha base pass + AdditiveStraight overlay pass) |
| **Archetype(s)** | (E) particle emitter — **directional ballistic spray with gravity + air drag**, plus a per-particle **3-stage state machine** FLY → SPLAT → SHRINK (custom logic, archetype F2). Scene-**lit** (archetype G's inverse — it *receives* light, doesn't emit it). One-shot **burst**. |

---

## 1. Summary

Blood is the on-hit combat gore spray. When a melee attack lands (or kills), the
victim spawns a `"Blood"` effect that throws a small burst of red blood droplets
out **along the impact direction** (away from the attacker), arcing under gravity
with air drag, growing slightly as they fly, then **splatting flat on the ground**
into a short-lived pooled splat that holds, then shrinks away. The droplets are
billboard quads from `Misc\Blood.I3D` (sub-objects `box01`–`box08`), in three
size variants (small / medium / large) plus a screen-facing "blood-in-face" splat
(disabled in the shipped build). Blood is **scene-lit** (it samples the nearby
scene lights + ambient and tints itself, so it goes dark in shadow and bright in
light — it does NOT glow) and is drawn in two passes: a normal Alpha pass plus an
additive overlay pass that brightens the wettest core of each droplet. The amount,
direction, and size scale with the global `ViolenceLevel` (0–5).

> **The real mechanism is `TBloodSystem`** (`src/effectcomp.cpp:1182-1554`), a
> bespoke fixed-array particle system owned by `TBloodAnimator`. It is **NOT** the
> generic `TParticle3DAnimator` that sparks use, and it is **NOT** a simple radial
> cone burst. The prior port (INVENTORY B01 note) approximated it as a 10-particle
> radial cone with a single render pass and a linear alpha fade — that is wrong on
> spawn distribution, stage machine, render passes, lighting, and count. This doc
> is the corrective record (see §13 corrective callouts inline, and the §6 / §5
> diagrams).

---

## 2. Sources & evidence

- **Retail decomp:** `recon/classes/cls_0x5acaa8.cpp` — **SPARSE / merged.** The
  Ghidra class `cls_0x5acaa8` is a MEDIUM-confidence merge of four adjacent leaf
  effect classes (Blood + Mist + MistFog + WaterFall),
  `recon/mappings/TBloodEffect_cls_0x5acaa8_candidate.yaml:1-30`. It confirms
  `TBloodEffect` exists as a ~272-byte `TEffect` leaf (`yaml:17,27`) with the
  `"Blood"` string XREF'd into two ctor/dtor thunks (`yaml:9`), but the file
  contains **no blood constants and no `TBloodSystem` body** (verified by grep —
  no `0.37`, `0.95`, `gravity`, `squirt`, `splat`). Retail gives the class shape,
  not the kinematics.
- **Pre-release (authoritative for behavior + all constants):**
  - `TBloodSystem` declaration + structs: `src/effectcomp.h:356-401` (`SBloodParticle`
    `:356-364`, `SBloodSystemParams` `:366-374`, `MAX_BLOODS` `:376`, class
    `TBloodSystem` `:378-401`).
  - `TBloodSystem` bodies + `BLOOD_*` constants: `src/effectcomp.cpp:1164-1554`
    (constants `:1164-1180`; `Init` `:1182-1307`; `Animate` `:1309-1370`;
    `DoLighting` `:1372-1413`; `Render` `:1415-1530`; `RefreshZBuffer`
    `:1532-1554`). **Gated** under the file's `#if` archive region — the whole
    blood block ends at the `#endif` at `:1556`.
  - `TBloodEffect` object: builder `DEFINE_BUILDER("Blood", TBloodEffect)`
    `src/effect_old.cpp:11243`; `Initialize`/`Pulse` (both trivial)
    `:11246-11253`.
  - `TBloodAnimator` visual: `REGISTER_3DANIMATOR("Blood", TBloodAnimator)`
    `src/effect_old.cpp:11261`; `Initialize` `:11269-11297`; `Animate`
    `:11306-11317`; `Render` `:11326-11335`; `RefreshZBuffer` `:11337-11340`.
  - Class declarations: `TBloodEffect` `src/effect.h:2407-2458` (params + setters
    `:2410-2439`); `TBloodAnimator` `src/effect.h:2466-2486` (owns
    `TBloodSystem bloods;` `:2469`).
- **Caller (authoritative for the per-spawn params):** `TCharacter` combat code,
  `src/character.cpp`:
  - The **live combat call** is `((TBloodEffect*)inst)->SetParams(height,
    (GetFace() + 128) & 255, 0, 80, 20, random(1, 5))` —
    **currently commented out** at `src/character.cpp:2260` (disabled in the port
    only, see §12); this is the authoritative spawn handshake.
  - Spawn dispatch + the `"blood"` branch of `EffectBurst`:
    `src/character.cpp:2225-2261`.
  - Triggers: on-impact `src/character.cpp:1842-1848`; on-death
    `src/character.cpp:1924-1925`; impale-decap gore gout
    `src/character.cpp:232-256` (separate params, §5.2).
- **Existing port shell:** `src/effect.{h,cpp}`. `TBloodEffect::SpawnForTest` /
  `TickAndSubmitForTest` (`src/effect.h:2441-2457`) is a Phase-2.2 scaffold that
  does **NOT** use `TBloodSystem` — it's a placeholder 10-droplet radial burst
  for the `--test=vfx` harness. Treat it as the thing to replace, not a reference
  (INVENTORY B01 already flags it as non-faithful, tracked B01a).
- **Sister effects consulted:**
  - `TParticle3DAnimator` / `"sparks"` (`SPARKS_TSparkAnimator.md`,
    `src/effect_old.cpp:4694-5002`) — to **rule out** that blood uses the generic
    particle animator. It does not (§2 disambiguation).
  - `TPulpEffect` / `TPulpAnimator` gore (`src/effect_old.cpp:6770-6803`,
    `SBloodDrop` `src/effect.h:1265-1337`) — cross-reference only; Pulp has its
    **own** `SBloodDrop` droplet system, NOT `TBloodSystem` (§12).
- **Source-of-truth ranking:** pre-release `effectcomp.cpp` is authoritative for
  all kinematics and rendering (full bodies present, all constants present); the
  **caller** `character.cpp:2260` is authoritative for the per-spawn parameter
  values (the system reads them in via `SBloodSystemParams`). Retail recon only
  corroborates the class shape. No retail-vs-pre-release behavioral divergence
  found (retail has no extractable blood body to diverge).

### Disambiguation — blood is NOT the generic particle animator

A commented-out dev experiment registered blood on the generic system:
`//REGISTER_3DANIMATOR_("blood", TBloodAnimator, TBloodParticle3DAnimator)`
(`src/effect_old.cpp:4687`) and `REGISTER_MULTI_3DANIMATOR_("blood",
TBloodAnimator, TParticle3DAnimator)` (`src/effect_old.cpp:4691`, inside a
`/* ... */` block). The `TBloodParticle3DAnimator` bodies (`:5008-5033`) are also
fully commented out and are pass-throughs to `TParticle3DAnimator` with a `// tenshu`
TODO and an empty `Animate`. **These are dev-disabled alternates — DO NOT
reconstruct them.** What they reveal: the team briefly considered driving blood
off the same generic emitter as sparks, then abandoned it in favour of the
purpose-built `TBloodSystem` (the live `REGISTER_3DANIMATOR("Blood",
TBloodAnimator)` at `:11261`, which owns a `TBloodSystem`). The live path is
`TBloodSystem`.

---

## 3. Constants

All `BLOOD_*` engine constants are `#define`s at `src/effectcomp.cpp:1164-1180`.
The per-spawn params (count / angles / spread / height) come from the **caller**
(`character.cpp`) via `SBloodSystemParams` — the live combat values are the
commented `SetParams(...)` at `character.cpp:2260`.

### 3.1 Engine constants (`TBloodSystem`)

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| `MAX_BLOODS` | 30 | count (fixed array cap) | effectcomp.h:376 | yes |
| `BLOOD_VEL` | 2.0 | wu/tick (base speed) | effectcomp.cpp:1173 | yes |
| `BLOOD_GRAVITY` | 0.37 | wu/tick² (subtracted from vel.z each FLY tick) | effectcomp.cpp:1165 | yes |
| `BLOOD_AIRRESIST` | 0.95 | unitless (vel.x,vel.y *= per FLY tick) | effectcomp.cpp:1164 | yes |
| `BLOOD_FLYSCALE` | 1.0 | scale (initial droplet scale) | effectcomp.cpp:1178 | yes |
| `BLOOD_FLYSCALESTEP` | 0.01 | scale/tick (grows while flying) | effectcomp.cpp:1177 | yes |
| `BLOOD_ZOFFSET` | 20 | wu (ground/floor plane the droplet splats at; z is local-to-effect, §5) | effectcomp.cpp:1172 | yes |
| `BLOOD_SPLATSIZE` | 1.8 | scale (held scale during SPLAT) | effectcomp.cpp:1169 | yes |
| `BLOOD_SPLATDURATION` | 25 | ticks (SPLAT hold before SHRINK) | effectcomp.cpp:1170 | yes |
| `BLOOD_SPLATSCALE` | 0.1 | scale/tick (shrink rate in SHRINK) | effectcomp.cpp:1171 | yes |
| `BLOOD_SQUIRTDELAY` | 3 | ticks (trail-droplet launch delay offset) | effectcomp.cpp:1174 | yes |
| `BLOOD_BLOODSPERSQUIRT` | 4 | count (trail-budget multiplier, §6.1) | effectcomp.cpp:1179 | yes |
| `BLOOD_MOSTBLOODS` | 10 | count (trail-budget reference, §6.1) | effectcomp.cpp:1180 | yes |
| `BLOOD_FLY` | 0 | stage enum (flying) | effectcomp.cpp:1166 | yes |
| `BLOOD_SPLAT` | 1 | stage enum (splatted/holding) | effectcomp.cpp:1167 | yes |
| `BLOOD_SHRINK` | 2 | stage enum (shrinking away) | effectcomp.cpp:1168 | yes |
| `BLOOD_BIFDELAY` | 8 | ticks ("blood-in-face" overlay delay — disabled, §13.3) | effectcomp.cpp:1175 | yes (vestigial) |
| `BLOOD_BIFSCALESTEP` | 0.5 | scale/tick (bif growth — disabled) | effectcomp.cpp:1176 | yes (vestigial) |
| velocity speed multiplier | `random(100, 300) / 100.0` ⇒ **1.0 .. 3.0×** | unitless | effectcomp.cpp:1229-1231 | yes |
| trail velocity (smalls) | parent vel × `random(60,75)/80` ⇒ **0.75 .. 0.94×** | unitless | effectcomp.cpp:1297-1299 | yes |
| `DIVISOR` (lighting clamp) | 1.5 | luminance clamp in DoLighting | effectcomp.cpp:1404 | yes |
| ambient light weight | `ambient/255 * 4.0` | added to droplet luminance | effectcomp.cpp:1400-1402 | yes |
| `TORADIAN` | π/180 | byte-angle→deg→rad conversion factor | revdefs.h:25 | yes |

`random(min,max)` is **inclusive** on both ends (`rand()%(max-min+1)+min`,
`src/revutils.cpp:1597-1612`).

### 3.2 Per-spawn parameters (`SBloodSystemParams`, set by caller)

`SBloodEffect::SetParams(he, ha, va, hs, vs, nu)` → fields `height, hangle,
vangle, hspread, vspread, num` (`src/effect.h:2438-2439`). `maxsize=2` is set by
the animator (`effect_old.cpp:11292`). Angles are **8-bit byte-angles** (0–255 =
0–360°).

| param | live-combat value | impale-decap value | meaning | source |
|-------|-------------------|--------------------|---------|--------|
| `height` (`he`) | 40 (impale) / 50 (normal) | 95 | spawn z above the effect origin (wu); also the splat-down reference | character.cpp:1843,1848,1925 / :236 |
| `hangle` (`ha`) | `(GetFace() + 128) & 255` ⇒ **opposite the victim's facing** | 0 | mean horizontal spray direction (byte-angle) | character.cpp:2260 / :254 |
| `vangle` (`va`) | 0 ⇒ horizontal | 64 ⇒ **straight up (90°)** | mean vertical spray angle (byte-angle) | character.cpp:2260 / :254 |
| `hspread` (`hs`) | 80 ⇒ ±80 byte-angle (±112°) | 255 ⇒ **full 360° fountain** | horizontal jitter half-range | character.cpp:2260 / :254 |
| `vspread` (`vs`) | 20 ⇒ ±20 byte-angle (±28°) | 5 ⇒ ±7° | vertical jitter half-range | character.cpp:2260 / :254 |
| `num` (`nu`) | `random(1, 5)` | 1 | base droplet count (before ViolenceLevel clamp + trail fill) | character.cpp:2260 / :254 |
| `maxsize` | 2 | 2 | max size variant index (0=small,1=med,2=big) | effect_old.cpp:11292 |
| `ViolenceLevel` | 0–5 (default **5**) | — | global gore clamp (§6) | revmain.cpp:165, revenant.h:54 |

> **Corrective callout (spawn distribution).** Blood is a **directional spray
> along `(hangle, vangle)`**, jittered by `(hspread, vspread)`. In live combat
> `hangle = (victim_face + 128) & 255` = directly **away from the victim's
> facing** (i.e. roughly in the direction the blow came from / the wound exit) and
> `vangle = 0` (horizontal). It is NOT a symmetric radial cone and NOT a vertical
> fountain (that's only the decap variant). A reconstruction that sprays radially
> is wrong (this is what the prior port did).

---

## 4. Assets

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| Blood | `legacy/Imagery/Misc/blood.i3d` | 68,820 B | the blood droplet + splat imagery — a STILL (billboard) `.I3D` with **8 sub-objects** `box01`–`box08` | registered `legacy/Class.Def:2014` (`"Blood" "Misc\Blood.I3D" 0xddc4042e`); pre-loaded by `PlayScreen` at boot (`src/playscreen.cpp:241`, `bloodimagery = load("Misc\\Blood.I3D")`); bound to the spawned effect by the `OBJCLASS_EFFECT` registry; addressed by the animator via `GetObject(0..7)` (`effect_old.cpp:11284-11291`) |

**Sub-objects (8)** — names read directly from the `.I3D` object table (records
at 0x30 stride starting 0xb4): `box01` (0xb4), `box02` (0xe4), `box03` (0x114),
`box04` (0x144), `box05` (0x174), `box06` (0x1a4), `box07` (0x1d4), `box08`
(0x204). The animator maps them to the two render passes (`effect_old.cpp:11284-11291`):

| GetObject(n) | I3D sub-object | param slot | role |
|--------------|----------------|------------|------|
| 0 | `box01` | `s2` (sml2) | small droplet — **overlay (additive) pass** |
| 1 | `box02` | `m2` (med2) | medium droplet — overlay pass |
| 2 | `box03` | `b2` (big2) | large droplet — overlay pass (currently unused, §7) |
| 3 | `box04` | `sp2` (spl2) | "blood-in-face" splat — overlay pass (disabled, §13.3) |
| 4 | `box05` | `s` (sml) | small droplet — **base (alpha) pass** |
| 5 | `box06` | `m` (med) | medium droplet — base pass |
| 6 | `box07` | `b` (big) | large droplet — base pass (currently unused, §7) |
| 7 | `box08` | `sp` (spl) | "blood-in-face" splat — base pass (disabled, §13.3) |

So the asset carries **3 droplet size variants** (small/med/big) + **1 splat**,
each duplicated as a base-pass copy and an overlay-pass copy (8 total). The size
variant a particle draws is `blood[i].size` (0/1/2). The two copies of each let
the system bind a different blend state per pass while reusing the same authored
quad (the verts are fetched once per pass-copy in `Init`,
`effect_old.cpp:11195-11202`).

**Texture surfaces:** the I3D header carries two texture cell sizes — **128 × 160**
(`0x48`) and **64 × 120** (`0x4c`), RGB565 (2 B/px). These are the larger
splat/big-droplet sheet and the smaller droplet sheet. The texture content is the
red blood sprite (§10). Header type tag `STILL` (`0x1c`), single state / single
frame (`0x10`=84/`0x14`=1) — **no `framehtexs` flipbook** (§8).

**Separate 2D blood assets (NOT this effect):** `Misc\Blood1-3.I2D` and
`Misc\BloodS1-4.I2D` (`Class.Def:305-311`) are HUD/portrait wound-spatter bitmaps
(2D `.I2D`, 4–6 KB each), unrelated to the 3D `TBloodSystem`. Do not confuse them
with `Blood.I3D`.

**The effect loads a real asset — do NOT substitute procedural droplet sprites.**
The authored red blood droplet/splat sprites in `Blood.I3D` are the visual
identity. (The current port's `SpawnForTest` loads the right `.I3D` but draws a
generic atlas burst — replace with the real `box0N` quads.)

---

## 5. Spawn & emit

### 5.1 Live combat blood (the primary case)

- **Trigger semantics:** **one-shot burst.** All droplets are seeded at once in
  `TBloodSystem::Init` (`effectcomp.cpp:1220-1306`); no continuous emission. The
  effect self-kills when every droplet finishes its FLY→SPLAT→SHRINK life
  (`TBloodAnimator::Animate` sets `OF_KILL` when `bloods.GetDone()`,
  `effect_old.cpp:1313-1316`; `GetDone` returns the `done` flag,
  `effectcomp.h:400`). It also dies on `OffScreen()` (`effect.h:2433`).
- **Count per trigger:** base `num = random(1, 5)` (`character.cpp:2260`), then
  **clamped down** by `ViolenceLevel`: `if (num > ViolenceLevel*2) num =
  ViolenceLevel*2` (`effectcomp.cpp:1214-1215`), then **expanded** by trailing
  smalls up to a `left` budget toward `MAX_BLOODS` (§6.1). At default
  `ViolenceLevel=5` the count is the raw 1–5 plus trail fill.
- **Initial direction / distribution:** a **directional ballistic spray** centered
  on `(hangle, vangle)` byte-angles, jittered by `±hspread / ±vspread`. Per
  droplet (`effectcomp.cpp:1222-1231`):
  ```
  ha = ((hangle + random(-hspread,+hspread)) & 255) * 360/256 * (π/180)   // radians
  va = ((vangle + random(-vspread,+vspread)) & 255) * 360/256 * (π/180)
  speedmul = random(100,300)/100                                          // 1.0..3.0
  vel.x = BLOOD_VEL * cos(ha) * speedmul
  vel.y = BLOOD_VEL * sin(ha) * speedmul
  vel.z = BLOOD_VEL * sin(va) * speedmul
  ```
  In live combat `hangle=(victim_face+128)&255` (away from victim facing),
  `vangle=0` (so `vel.z = 2.0*sin(0)*mul = 0` at the mean — droplets launch
  **horizontally**, then gravity arcs them down), `hspread=80` (≈±112° fan),
  `vspread=20` (≈±28° vertical scatter). Note `speedmul` is per-axis-shared per
  droplet here (each component uses its own `random(100,300)` call, so axes
  scatter independently — see the three separate calls at `:1229-1231`).
- **Emit anchor convention:** the **effect object** is placed at the victim's
  world position raised by `height`: `def.pos = pos; def.pos.z += height`
  (`character.cpp:2244-2245`). Each droplet's *local* position starts at
  `(0, 0, height)` (`effectcomp.cpp:1226-1228`) — i.e. local-space, with the
  spawn point `height` wu up the local z. So the spray originates at roughly
  wound height (40–50 wu, ~waist/chest) above the victim's feet.
- **Coordinate space:** **Local** to the effect object. Droplet `pos`/`vel` are
  integrated in local space (`effectcomp.cpp:1326-1328`); `Render` subtracts
  `height` from local z so the quad sits relative to the object origin
  (`effect_old.cpp:1475`); `RefreshZBuffer` adds the effect world pos back in
  (`effectcomp.cpp:1544-1546`).
- **Spread / jitter:** horizontal ±`hspread` byte-angle, vertical ±`vspread`
  byte-angle, speed ×1.0–3.0, size `random(0, min(maxsize,2))`. Initial xy
  position offset is **zero** (`pos.x=pos.y=0`, the commented `me.offset`
  `:1226-1227` is unused).

### 5.2 Impale-decapitation gore gout (secondary, same classes)

The `impdecap` death animation spawns a `"Blood"` effect every 5 frames between
frame 5 and 65 (`character.cpp:232-256`), with
`SetParams(height=95, hangle=0, vangle=64, hspread=255, vspread=5, num=1)`:
- `vangle=64` byte-angle = **90° straight up**, so `vel.z = 2.0*sin(90°)*mul`
  (≈2–6 wu/tick upward) — an upward gout.
- `hspread=255` = full 360° horizontal randomization → a radial fountain in xy.
- Spawned repeatedly (≈12 spawns over the death anim) at `height=95` (head height,
  the decapitation point).
This is the **only** case where blood is a fountain/radial — and it's a continuous
re-spawn of the same one-shot system, not a different particle path.

### Spatial diagram (live combat spray)

```
   wz (up)                            ScreenAligned? NO — see §7: the
   │                                  flying droplet quad is tipped to
   │      · ·   ·                     WorldXY-ish (rot.x=-90°) + spun
   │    ·  ·  ·  ·   ← droplets spray HORIZONTALLY along (hangle, ±hspread)
   │   · · · ·  ·       mean vel.z = 0 (vangle 0), gravity 0.37 arcs them down,
   │  ·· · ·            air drag 0.95 bleeds xy speed each tick
   │ ·                  scale grows +0.01/tick while flying
   └─────────────●──────── wx     droplet local origin = (0,0,height≈45)
  ╱         splat plane z = BLOOD_ZOFFSET (20):  · · ·  flat splats, hold 25t, shrink
 ╱   ▲ effect origin = victim world pos, z += height(40/50)
wy     hangle = (victim_face + 128) & 255  ⇒ AWAY from victim facing (NOT radial)

top-down spray cone:   victim faces →     blood sprays back  ⤙ ±112° (hspread 80)
```

---

## 6. Behavior & per-frame logic

The whole effect is `TBloodSystem` driven by `TBloodAnimator`:
- `TBloodAnimator::Animate(draw)` (`effect_old.cpp:11306-11317`): calls
  `T3DAnimator::Animate(draw)`, marks command-not-done, calls
  `bloods.Animate()`, and sets `OF_KILL` if `bloods.GetDone()`.
- `TBloodAnimator::Render()` (`effect_old.cpp:11326-11335`): `SaveBlendState()`;
  `bloods.Render()`; `RestoreBlendState()`.
- `TBloodAnimator::RefreshZBuffer()` (`effect_old.cpp:11337-11340`): delegates to
  `bloods.RefreshZBuffer()`.

### 6.1 Seeding (`TBloodSystem::Init`, `effectcomp.cpp:1182-1307`)

```
Init(params):                                       // effectcomp.cpp:1182
    store animator, the 8 box sub-objects, height, effectpos          // :1184-1204
    GetVerts(...) on all 8 sub-objects (D3DVT_LVERTEX)                 // :1195-1202
    bifon = BLOOD_BIFDELAY (8); if (!ViolenceLevel) bifon = 0         // :1206-1208 (bif disabled, §13.3)
    bifscale = BLOOD_BIFSCALESTEP (0.5)                               // :1209

    left = max(MAX_BLOODS - (BLOOD_MOSTBLOODS - num)*BLOOD_BLOODSPERSQUIRT,
               BLOOD_BLOODSPERSQUIRT)               // trail budget   // :1211
    if (num > ViolenceLevel*2)  num = ViolenceLevel*2                 // :1214-1215  CLAMP count
    if (maxsize > ViolenceLevel) maxsize = ViolenceLevel             // :1216-1217  CLAMP size

    // -- primary droplets --
    for i in 0 .. min(num, MAX_BLOODS)-1:                            // :1220
        ha = byteangle_to_rad(hangle + random(-hspread,hspread))      // :1222
        va = byteangle_to_rad(vangle + random(-vspread,vspread))      // :1223
        blood[i].used  = true                                         // :1224
        blood[i].size  = random(0, min(maxsize, 2))                   // :1225
        blood[i].pos   = (0, 0, height)                               // :1226-1228
        blood[i].vel   = (BLOOD_VEL*cos(ha)*r1, BLOOD_VEL*sin(ha)*r2,
                          BLOOD_VEL*sin(va)*r3)  // r* = random(100,300)/100  :1229-1231
        blood[i].scl   = BLOOD_FLYSCALE (1.0)                         // :1232
        blood[i].stage = BLOOD_FLY (0); blood[i].delay = 0           // :1233-1234
        left--                                                        // :1235

    if (ViolenceLevel < 4)  left = 0    // no trails below violence 4 // :1238-1239

    // -- trail fill: launch extra SMALL droplets behind existing ones --
    // (the big->med/small and med->med/small trail loops are COMMENTED OUT, :1242-1283)
    for i = 0; left; i++:                                            // :1287
        if (blood[i].used && num < MAX_BLOODS):                       // :1290
            blood[num].used  = true
            blood[num].size  = random(0, 1)                           // small or med  :1293
            blood[num].pos   = (0, 0, height)                         // :1294-1296
            blood[num].vel   = parent.vel * random(60,75)/80  (0.75..0.94x)  :1297-1299
            blood[num].delay = parent.delay + BLOOD_SQUIRTDELAY + random(-1,1)  :1300
            blood[num].stage = BLOOD_FLY; blood[num].scl = 1.0       // :1301-1302
            num++; left--                                             // :1303-1304
```

**ViolenceLevel gates (the gore slider):**
- Count clamp: `num = min(num, ViolenceLevel*2)` (`:1214-1215`).
- Size clamp: `maxsize = min(maxsize, ViolenceLevel)` (`:1216-1217`).
- Trails only fire at `ViolenceLevel >= 4` (`:1238-1239`).
- At splat-down, droplets are removed early (no lingering splat) when
  `ViolenceLevel < 3` (`:1341-1342`), and small droplets (`size==0`) always
  vanish on impact rather than splatting (`:1343-1344`).
- "Blood-in-face" overlay disabled when `!ViolenceLevel` (`bifon=0`, `:1207-1208`).

> **Corrective callout (count).** The droplet count is NOT a fixed 10. It is
> `num=random(1,5)` clamped by `ViolenceLevel*2`, then **trail-expanded** by a
> `left` budget of small droplets toward `MAX_BLOODS=30` (only at violence ≥4).
> A faithful port at default violence 5 produces a *small* initial burst (1–5
> droplets) that grows a tail of delayed smalls — not a flat 10-particle puff.

### 6.2 Per-tick update (`TBloodSystem::Animate`, `effectcomp.cpp:1309-1370`)

A 3-stage state machine per droplet, plus a global `done` flag (true only when no
live, past-delay droplet exists this tick):

```
Animate():                                          // effectcomp.cpp:1309
    done = true
    for i in 0 .. MAX_BLOODS-1:                       // :1312
        if !blood[i].used: continue                  // :1314
        if blood[i].delay: blood[i].delay--; continue  // staggered launch  :1316-1320
        done = false

        if stage == BLOOD_FLY:                       // :1324
            pos += vel                               // Euler integrate     :1326-1328
            vel.x *= BLOOD_AIRRESIST (0.95)          // horizontal drag     :1330
            vel.y *= BLOOD_AIRRESIST (0.95)          //                     :1331
            vel.z -= BLOOD_GRAVITY (0.37)            // gravity (z only)    :1332
            scl   += BLOOD_FLYSCALESTEP (0.01)       // grow while flying   :1334
            if pos.z <= BLOOD_ZOFFSET (20):          // hit the floor       :1336
                pos.z = BLOOD_ZOFFSET                // clamp to floor      :1338
                stage = BLOOD_SPLAT; count = 0       // -> SPLAT            :1339-1340
                if ViolenceLevel < 3: used = false   // low gore: no splat  :1341-1342
                if size == 0:        used = false    // smalls never splat  :1343-1344

        else if stage == BLOOD_SPLAT:                // :1347
            scl = BLOOD_SPLATSIZE (1.8)              // snap to splat size  :1349
            count++                                  // :1350
            if count > BLOOD_SPLATDURATION (25):     // hold 25 ticks       :1351
                stage = BLOOD_SHRINK; count = 0      // -> SHRINK           :1353-1354

        else if stage == BLOOD_SHRINK:               // :1357
            scl -= BLOOD_SPLATSCALE (0.1)            // shrink 0.1/tick     :1359
            if scl <= 0: scl = 0; used = false       // gone (~18 ticks)    :1360-1363

    bifon--; bifscale += BLOOD_BIFSCALESTEP          // bif (disabled, §13.3)  :1368-1369
```

- **Drag is horizontal-only.** `vel.x` and `vel.y` decay by ×0.95/tick; `vel.z`
  gets **only** gravity (no drag on z). So horizontal travel bleeds off while the
  vertical fall accelerates — a forward-arcing splat, not a symmetric parabola.
- **Air drag + gravity together** are why droplets don't fly far: at ×0.95/tick,
  horizontal speed halves in ~13 ticks.
- **Ground splat at local z = `BLOOD_ZOFFSET` (20)**, NOT z=0. The droplet sticks
  there and transitions to a flat held splat (scale snaps to 1.8) for 25 ticks,
  then shrinks away at 0.1/tick (~18 ticks). Small droplets and low-violence
  droplets disappear on impact instead of splatting.
- **Death:** a droplet ends when `used=false` (on impact for smalls/low-violence,
  or when SHRINK scale reaches 0). The whole effect ends (`done` stays true, →
  `OF_KILL`) when no droplet is live-and-past-delay in a tick.

> **Corrective callout (the stage machine + ground splat).** The prior port had
> droplets fade out via a "hold-half / linear-fade-half alpha curve" then reap on
> expiry. The original has **no alpha curve at all** — droplets keep full alpha
> and instead change **scale** through three stages, *splat flat on a z=20 floor*
> (growing to 1.8× then shrinking), and die by scale→0 or by impact removal. The
> visual "wetness" comes from the splat-grow-then-shrink + the additive overlay
> pass, not from alpha fade. Also: gravity is **0.37** (not the sparks' 0.2), and
> there's horizontal **air drag 0.95** the sparks don't have.

### Temporal diagram (one medium/big droplet, ViolenceLevel ≥ 3)

```
stage:   delay │ FLY (arc + grow) │ SPLAT (hold flat) │ SHRINK │ dead
                            scl: 1.0 ─/+0.01─▶          1.8 (snap)  1.8 ─\─0.1▶ 0
ticks:   0..d  │ until pos.z<=20  │ 25 ticks          │ ~18 ticks │
pos.z:   ──────  height ↘ (gravity 0.37) ──▶ 20 (clamp) ── stays 20 ──
vel.xy:         × 0.95 each tick (drag)            (frozen at splat)
alpha:   ─────────────── CONSTANT (no fade) ──────────────────────────

small droplet (size 0):  delay │ FLY │ (pos.z<=20 ⇒ used=false, vanishes — no splat)
```

---

## 7. Rendering (original render state + geometry)

`TBloodSystem::Render` (`effectcomp.cpp:1415-1530`) iterates live droplets; for
each, it runs **two passes** `k=0` then `k=1` (`:1425`):

```
Render():                                            // effectcomp.cpp:1415
  for i in 0..MAX_BLOODS-1, blood[i].used:           // :1420-1423
    for k in 0..1:                                    // :1425  TWO passes per droplet
      if k==0: RestoreBlendState(); SetBlendState()   //   Alpha base pass        :1427-1431
      else:    RestoreBlendState();                    //   AdditiveStraight overlay :1434-1439
               SetRenderState(TEXTUREMAPBLEND, MODULATE)
               SetRenderState(SRCBLEND, ONE); SetRenderState(DESTBLEND, ONE)
      object = (k==0 ? sml : sml2)                     //   pick size variant + pass copy
      if size==1: object = (k==0 ? med : med2)         //   (size==2/big copy is COMMENTED, :1447-1448,1455-1456)
      animator->ResetExtents()                         // :1459
      object->flags = OBJ3D_MATRIX | OBJ3D_VERTS       // :1461
      build matrix: Scale(scl) ;                       // :1462-1465
        if stage==BLOOD_FLY: RotateX(-90°); RotateZ(-45°)   // tip flying droplet  :1467-1471
        Translate(pos.x, pos.y, pos.z - height)        // :1473-1476
      DoLighting(pos, object)                          //   scene-lit vertex color :1478
      animator->RenderObject(object); UpdateExtents()  // :1480-1481
```

- **What it draws:** for each live droplet, one billboard quad of `Blood.I3D`
  (the small or medium variant; the **big variant is commented out**,
  `:1447-1448` / `:1455-1456`, so shipped blood draws only size 0/1 quads even
  though `size` can be 2). Drawn **twice** (base + overlay).
- **Blend mode (original) — TWO passes:**
  - **Pass k=0: Alpha.** `SetBlendState()` (`effect_old.cpp:221-233`) = texture
    `D3DTBLEND_MODULATE`, `SRCBLEND=SRCALPHA`, `DESTBLEND=INVSRCALPHA`,
    `ZWRITEENABLE=false`, `ZENABLE=true` → classic translucent **Alpha**
    (NOMENCLATURE §3). This is the body of the droplet.
  - **Pass k=1: AdditiveStraight overlay.** Manually sets `MODULATE` +
    `SRCBLEND=ONE`, `DESTBLEND=ONE` (`effectcomp.cpp:1435-1438`) → **ONE/ONE
    additive**. The ZWRITE/ZENABLE lines are commented out (`:1436,1439`), so the
    overlay **inherits** the alpha pass's depth state (TestNoWrite). This second
    additive draw of the same quad brightens the wet core / highlight of the
    droplet over the alpha body.
  > **Corrective callout (blend).** Blood is **NOT** single-pass, and **NOT**
  > purely additive, and **NOT** purely alpha. It is an **Alpha base + additive
  > overlay** of the same sprite. The prior port did a single PremulAlpha pass.
  > Reproduce both passes (the additive overlay is what gives blood its glistening
  > read). Note the helper used for the base pass is `SetBlendState` (Alpha), not
  > `SetAddBlendState` — traced, not guessed.
- **Lit vs self-lit:** **scene-LIT (LitFlat).** `DoLighting`
  (`effectcomp.cpp:1372-1413`) samples the 3 closest scene lights
  (`Scene3D.GetClosestLights` + `GetLightBrightness`, gated by `MaxLights`) plus
  `MapPane.GetAmbientLight()/255 * 4.0`, sums to a luminance `r`, clamps to
  `DIVISOR=1.5` then normalizes (`r/=1.5`), and writes **grayscale**
  `D3DRGB(r,r,r)` into all 4 `lverts[].color` of the quad (`:1409-1412`). Under
  MODULATE this scene-luminance multiplies the red blood texel. So blood **darkens
  in shadow, brightens in light, and does not glow** — it is one of the few effects
  that is genuinely scene-lit (NOMENCLATURE §4 LitFlat; AGENT_GUIDE notes
  blood/debris as LitFlat candidates). The color/hue itself is the texture's red
  (§10), not the vertex color.
- **Depth / Z:** **TestNoWrite** — `ZENABLE=true`, `ZWRITEENABLE=false` (set by
  `SetBlendState` in pass 0, inherited by pass 1). `RefreshZBuffer`
  (`effectcomp.cpp:1532-1554`) projects each live droplet's world pos
  (`WorldToScreen`) and calls `RestoreZ` over a quad of `size.x×scl` by
  `size.y×scl` (`size` = the `zbuf` field = (10,10), `effect_old.cpp:11279-11281`),
  centered on the droplet, so the depth-write-disabled droplets don't punch holes
  in the scene depth.
- **Orientation:** **WorldXY-tipped while flying.** During `BLOOD_FLY` the matrix
  applies `RotateX(-90°)` then `RotateZ(-45°)` (`effect_old.cpp:1469-1470`) — the
  `rot.x = -π/2` tell tips the authored-XY quad toward the floor plane (so flying
  droplets read as flat-ish streaks at a 45° in-plane spin), and during
  SPLAT/SHRINK the rotation is **omitted** (the splat lies flat by its scale-only
  matrix, on the z=20 plane). This is not a camera-facing billboard.
- **Per-quad transform:** scale (`blood[i].scl`, uniform), then the FLY-only
  rotation, then translate to `(pos.x, pos.y, pos.z - height)`
  (`effect_old.cpp:1462-1476`). `object->flags = OBJ3D_MATRIX | OBJ3D_VERTS`
  (`:1461`) — a full per-object matrix override using the imagery verts.
- **Per-vertex color packing:** `DoLighting` writes `lverts[j].color =
  D3DRGB(r,g,b)` with `r=g=b` (grayscale luminance) for all 4 verts
  (`effectcomp.cpp:1409-1412`). `D3DRGB(r,g,b)` packs to `0xFF` alpha +
  `RR GG BB` (legacy D3D macro). No red tint is added by the effect — the red is
  in the texture.

---

## 8. Texture animation

**N/A — none.** `Blood.I3D` is a STILL imagery, single state / single frame
(`0x10`=84 verts-ish, `0x14`=1 frame), no `framehtexs[]` flipbook (§4). `Render`
never mutates `tu/tv` and never calls `SetTextureFrame`. All motion is positional
(droplet flight) and via the scale/stage machine; the texture is static.

---

## 9. Associated light

**N/A — none emitted.** `TBloodSystem` makes no `AddPointLight` / dynamic-light
call (verified across `Init`/`Animate`/`Render`/`DoLighting`,
`effectcomp.cpp:1182-1530`); the caller `EffectBurst` adds none
(`character.cpp:2242-2261`). Blood **receives** scene light (§7 DoLighting) but
casts no light into the scene. (Contrast fire/explosions, which emit light.)

---

## 10. Color

- **Source:** the **authored `Misc\Blood.I3D` droplet/splat texture** (the red
  blood sprite). The effect supplies no hue of its own — it only modulates by a
  grayscale scene-luminance (§7 DoLighting). There is no spell color, no chardata
  color, no hardcoded RGB in the blood code.
- **Exact values:** the droplet sprites are stored RGB565 in the two texture cells
  (128×160 and 64×120, §4); they read as **deep saturated red** (arterial blood),
  darkening toward the edges (the chroma-key black background → premultiplied a=0,
  per the loader convention `02_ASSETS_IMAGERY.md §4`). The shipped *brightness*
  varies with scene light (DoLighting luminance ×, clamped to ≤1.0 after `/1.5`),
  so blood in a lit area reads bright red, in shadow dark red.
- **Expected visual:** rich, wet, **saturated red** droplets/splats. Under the
  Alpha base + additive overlay (§7) the wettest core reads brighter/glistening.
  A **pale / pink / gray / orange** result at reconstruction = broken port —
  likely culprits (AGENT_GUIDE §4.2.1.5): a procedural stand-in instead of the
  real `box0N` sprites (#1), the additive overlay pass dropped (washes out the
  wet core), the chroma-key not honored (dark fringe), or the LitFlat luminance
  applied as a *color* tint instead of a multiply (would gray it out).
- **Normalization / boosts:** the DoLighting luminance is clamped to `DIVISOR=1.5`
  then divided by 1.5 to normalize to ≤1.0 (`effectcomp.cpp:1404-1407`); ambient
  is weighted ×4 before the clamp (`:1402`). This is a brightness envelope, not a
  hue change.

---

## 11. Audio coupling

**No audio coupling in the effect.** `TBloodSystem` and `TBloodAnimator` make no
sound call. Unlike sparks (whose caller plays a block clang) and drip (which plays
"drip" on landing), the blood spawn sites (`character.cpp:1842-1848`,
`:1924-1925`, `:232-256`) play **no blood-specific sound** — the hit/impact sounds
are driven by the attack/impact action elsewhere in combat, not by the blood
effect. Record: **no audio coupling found** for the blood VFX itself.

---

## 12. Triggers & in-game appearance

- **Spawned by:** `TCharacter` combat resolution (`src/character.cpp`), via
  `EffectBurst("blood", height)` (`character.h:311`, body `:2225-2261`), which
  builds an `OBJCLASS_EFFECT` object of type `FindObjType("blood")` at the victim's
  pos + height and (in retail) calls `((TBloodEffect*)inst)->SetParams(height,
  (GetFace()+128)&255, 0, 80, 20, random(1,5))` (`character.cpp:2260`). Live
  triggers:
  - **On impact** (`ResolveImpact`, `character.cpp:1842-1848`): blood when the
    impact does damage and the attack carries `CA_BLOOD` (`0x0200`, "Show blood on
    impact", `rules.h:41`), or randomly on `impale` attacks; height 40 (impale) /
    50.
  - **On death** (`ResolveDead`, `character.cpp:1924-1925`): blood when the killing
    attack has `CA_BLOOD`.
  - **Impale-decap gore** (`character.cpp:232-256`): repeated upward gouts during
    the `impdecap` animation (§5.2).
  - Registration: `DEFINE_BUILDER("Blood", TBloodEffect)` (`effect_old.cpp:11243`)
    + `REGISTER_3DANIMATOR("Blood", TBloodAnimator)` (`:11261`); asset wired by
    `Class.Def:2014`.
- **Where to see it in the original game:** land any damaging melee hit on an enemy
  with a `CA_BLOOD` weapon/attack — blood sprays from the wound away from the
  victim's facing. Kill an enemy for the on-death burst. Use an `impale` attack for
  the random impale spray, or trigger a decapitation (`impdecap`) for the upward
  gore fountain. Set `ViolenceLevel` to 5 (default) for full effect; 0 suppresses it.
- **Vestigial?** No — live combat trigger. **Note the port disablement:** the
  authoritative `SetParams` call is currently **commented out** in the port
  (`character.cpp:2260`) because the imagery factory returns a generic `TEffect`
  rather than a `TBloodEffect` (typeinfo not emitted — no key function), so the
  virtual `SetParams` would crash. The effect still spawns (and combat still kills
  the target) but with **zeroed params** (height/angles/num all 0 → `num=0` → no
  droplets) until the effect-class registration is ported. The reconstruction must
  restore this call (the values in it are the spec).
- **Cross-reference — Pulp gore (`TPulpEffect`) does NOT use `TBloodSystem`.**
  `TPulpAnimator::Animate` (`effect_old.cpp:6770-6803`) runs its **own** droplet
  loop over `SBloodDrop` (`effect.h:1265-1337`, accessed via `GetBlood(i)`
  `:1337`), with different kinematics (air drag ×0.75 `:6790-6791`, gravity
  `PULP_GRAVITY*0.75` `:6802`, killed at `GetWalkHeight` `:6798-6800`) plus
  tumbling body parts. That is a parallel gib system, out of scope for B01;
  documented here only to rule it out — the standalone `"Blood"` effect is
  `TBloodSystem`.

---

## 13. Gaps & uncertainties

- **13.1 No retail kinematics.** `recon/classes/cls_0x5acaa8.cpp` is a sparse,
  Ghidra-merged blob with no `TBloodSystem` body and no constants
  (`yaml:1-30`; grep found none). All kinematics here are **pre-release-authoritative**
  — but the pre-release bodies are complete (every constant + the full state
  machine present), so risk is low. There is no retail cross-check for the exact
  `BLOOD_*` values; treat the pre-release `#define`s as ground truth.
- **13.2 Big-droplet variant is dead in the shipped render.** `Render` selects
  only `sml`/`med` (and their overlay copies) — the `big`/`big2` (`box03`/`box07`)
  selection is **commented out** (`effectcomp.cpp:1447-1448,1455-1456`), even
  though `blood[i].size` can be 2 (`Init` allows `random(0, min(maxsize,2))` with
  `maxsize=2`). A size-2 droplet therefore draws with the `sml` quad (the default
  `object = sml`). Reconstruction options: (a) faithful — draw size-2 with the
  small quad as shipped, or (b) honor intent — draw `box03`/`box07` for size 2.
  Recommend **(a) faithful** (the big sub-objects exist in the asset but were
  disabled — likely a late art/perf call). Flag for visual verification.
- **13.3 "Blood-in-face" (bif) splat is fully disabled.** `bifon`/`bifscale` +
  the `spl`/`spl2` (`box04`/`box08`) screen-splat draw are present but the entire
  draw block is **commented out** (`effectcomp.cpp:1485-1529`). It was meant as a
  camera-facing blood-on-the-lens splat that grows over `BLOOD_BIFDELAY` ticks.
  **Do NOT reconstruct** (record as disabled intent). The `bifon--`/`bifscale +=`
  bookkeeping still runs (`:1368-1369`) but feeds nothing.
- **13.4 Trail loops for big/med are commented out.** Only the **smalls-trail-smalls**
  loop is live (`effectcomp.cpp:1287-1306`); the big→{med,small} and med→{med,small}
  trail loops (`:1242-1283`) are commented. Faithful = only small trailing droplets
  (at violence ≥4). Record the commented loops as do-not-reconstruct.
- **13.5 Per-axis speed multiplier.** Each velocity axis uses an **independent**
  `random(100,300)/100` (three separate calls, `effectcomp.cpp:1229-1231`), so the
  speed isn't a single magnitude scaled — the axes scatter independently. This
  slightly distorts the spray direction per droplet (intended jitter). Reproduce
  the three independent rolls, not one shared multiplier.
- **13.6 Exact texture red + cell mapping.** The droplet hue reads deep red by
  asset inspection (RGB565, two cells 128×160 + 64×120); the precise per-variant
  sprite (which `box` uses which cell) should be confirmed by decoding `Blood.I3D`
  during reconstruction or capturing in-game. Not a behavioral unknown — "render
  the real texture and verify it's saturated red."
- **13.7 `BLOOD_ZOFFSET` floor is local, flat at 20.** The splat plane is a flat
  local z=20, not the real terrain height (Pulp uses `GetWalkHeight`; blood does
  not). On sloped/raised ground the splat may float/sink slightly relative to the
  true floor. This is original behavior — reproduce the flat z=20 splat.
- **13.8 Sim cadence.** The pre-release `Animate` is ungated (runs per frame); per
  NOMENCLATURE §6 the port must gate to 24 Hz (`kSimTickMs`) or the gravity/drag/
  durations scale with framerate. All `BLOOD_*` rates are authored for 24 Hz ticks.

---

## 14. Reconstruction burndown

```
- [ ] Load Misc\Blood.I3D (68,820 B). Address its 8 STILL billboard sub-objects by
      index: 0..3 = box01..box04 (overlay-pass copies: sml2/med2/big2/spl2),
      4..7 = box05..box08 (base-pass copies: sml/med/big/spl). 3 size variants
      (small/med/big) + 1 disabled "bif" splat. NO procedural droplet stand-in. (§4)
- [ ] Drive blood as the bespoke TBloodSystem (effectcomp.cpp:1182-1554) owned by
      TBloodAnimator — NOT the generic TParticle3DAnimator, NOT a radial cone. (§2, §6)
- [ ] Wire per-spawn params via SBloodSystemParams from the caller: SetParams(height,
      hangle=(victim_face+128)&255, vangle=0, hspread=80, vspread=20, num=random(1,5)),
      maxsize=2. RESTORE the commented call at character.cpp:2260. (§3.2, §5, §12)
- [ ] Spawn = one-shot BURST seeded at once; effect self-kills (OF_KILL) when no
      droplet is live-and-past-delay (TBloodSystem::done -> GetDone). (§5, §6.2)
- [ ] Emit anchor: effect object at victim world pos, z += height(40/50); each droplet
      local pos starts (0,0,height); integrate in LOCAL space. (§5)
- [ ] Initial velocity = DIRECTIONAL spray along (hangle,vangle), jittered ±hspread/
      ±vspread (byte-angles -> rad via *360/256*pi/180): vel = BLOOD_VEL(2.0) *
      {cos(ha),sin(ha),sin(va)} * random(100,300)/100 (1.0..3.0, per-axis independent).
      Live combat = horizontal spray away from victim facing. (§3, §5, §6.1, §13.5)
- [ ] Count + ViolenceLevel gates: num = min(random(1,5), ViolenceLevel*2);
      maxsize = min(2, ViolenceLevel); trails only at ViolenceLevel>=4. (§3, §6.1)
- [ ] Trail fill: smalls-trail-smalls loop only — extra small/med droplets with vel =
      parent*random(60,75)/80 (0.75..0.94x) and delay = parent.delay+SQUIRTDELAY(3)
      +random(-1,1), up to MAX_BLOODS(30) within the `left` budget. (§6.1)
- [ ] Per-tick (24 Hz gate) FLY: pos += vel; vel.xy *= 0.95 (drag); vel.z -= 0.37
      (gravity, z only); scl += 0.01 (grow). Euler ballistic arc, horizontal drag. (§3, §6.2)
- [ ] Ground splat at local z = BLOOD_ZOFFSET(20): clamp z, stage->SPLAT; smalls(size 0)
      and ViolenceLevel<3 droplets DIE on impact (no splat). (§6.2, §13.7)
- [ ] SPLAT stage: scl snaps to BLOOD_SPLATSIZE(1.8), hold BLOOD_SPLATDURATION(25)
      ticks, then -> SHRINK. (§6.2)
- [ ] SHRINK stage: scl -= BLOOD_SPLATSCALE(0.1)/tick; die when scl<=0 (~18 ticks). (§6.2)
- [ ] NO alpha curve — alpha is CONSTANT across all stages; the "fade" is scale +
      staggered death + the additive overlay, not opacity. (§6.2, §10, §13)
- [ ] Per-droplet staggered launch: blood[i].delay counts down before the droplet
      starts moving/drawing. (§6.1, §6.2)
- [ ] Render geometry: per live droplet, one Blood.I3D quad placed at (pos.x,pos.y,
      pos.z-height) with uniform scale(scl); while FLYING apply RotateX(-90°) then
      RotateZ(-45°) (WorldXY-tipped streak); splat/shrink draw flat (scale only). (§7)
- [ ] TWO render passes per droplet: pass0 = Alpha (SetBlendState: MODULATE,
      SRCALPHA/INVSRCALPHA) base; pass1 = AdditiveStraight overlay (MODULATE, ONE/ONE)
      of the same quad. Both TestNoWrite (ZENABLE on, ZWRITE off). (§7, §10)
- [ ] Lit mode = LitFlat (scene-lit): per droplet sample 3 closest scene lights +
      ambient*4, clamp to 1.5, /1.5 -> grayscale luminance written to all 4 vertex
      colors (D3DRGB(r,r,r)); MODULATE multiplies the red texel. Blood darkens in
      shadow, does NOT glow. (§7, §10)
- [ ] Color from Blood.I3D texture = deep saturated RED; verify the real sprite
      loads (pale/pink/gray/orange => stand-in / dropped additive pass / chroma-key
      miss / luminance applied as tint). (§10)
- [ ] Texture animation = NONE (single-still sprite, no UV scroll, no flipbook). (§8)
- [ ] Associated dynamic light = NONE (blood receives light, emits none). (§9)
- [ ] Audio = NONE coupled to the effect (no blood sound; combat plays hit sounds
      separately). (§11)
- [ ] RefreshZBuffer: per live droplet, WorldToScreen(effectpos + droplet pos), then
      RestoreZ over a (10*scl) x (10*scl) screen patch centered on it. (§7)
- [ ] Trigger wiring: EffectBurst("blood", 40/50) on damaging CA_BLOOD impacts
      (character.cpp:1842-1848), on CA_BLOOD death (1924-1925); impdecap upward gout
      with SetParams(95,0,64,255,5,1) every 5 frames f5..f65 (232-256). (§5.2, §12)
- [ ] Do NOT reconstruct: TBloodParticle3DAnimator (commented alternate),
      big/med trail loops (commented), the "bif" screen-splat block (commented),
      and the size-2 big-quad render (commented — size 2 draws the small quad). (§2, §13)
```

**Definition of done:** a damaging CA_BLOOD melee hit spawns a small burst (1–5,
gated by ViolenceLevel) of deep-red `Blood.I3D` droplets that spray *away from the
victim's facing* (horizontal, jittered ±112°), arc down under gravity 0.37 with
horizontal air-drag 0.95 while growing 1%/tick, splat **flat on a local z=20
plane** (size 1.8, hold 25 ticks, shrink 0.1/tick) — small ones vanishing on
impact — each drawn twice (Alpha base + additive overlay), scene-lit (dark in
shadow), with delayed small trailing droplets at high violence, no light, no
audio, no alpha fade — and the effect kills itself when the last droplet expires.
