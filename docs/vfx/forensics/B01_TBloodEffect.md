# B01 TBloodEffect — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | B01 (covers B01 `TBloodEffect`, B02 `TBloodAnimator`, B03 `TBloodSystem`) |
| **Class(es)** | `TBloodEffect` (game object) + `TBloodAnimator` (visual component) + **`TBloodSystem`** (the real particle system the animator owns) + `SBloodParticle` / `SBloodSystemParams` (its data structs) |
| **Status** | forensics-complete (see §13 for the genuine unknowns) |
| **Author / Date** | vfx-forensics-agent / 2026-05-19 |
| **Family** | blood/combat |
| **Draws** | particle emitter — up to 30 small/medium billboard droplets, each one sub-object of `Misc\Blood.I3D`, placed at a particle position; each droplet drawn **twice** (alpha pass + additive overlay) |
| **Archetype(s)** | (E) particle emitter — **ballistic spray with a 3-stage state machine** (FLY → SPLAT → SHRINK). Plus (F2) custom procedural logic: directional velocity distribution, ground-plane collision at a fixed local z, scale-not-alpha lifecycle, "trail-fill" particle multiplication, and (G-adjacent) per-droplet **scene lighting sample** (`DoLighting`). NOT a simple emitter — it has real custom logic. |

---

## 1. Summary

Blood is the combat gore spray: when a character takes a damaging hit (or is
decapitated, or dies), a burst of dark-red droplets squirts out of the wound,
flies through the air on a ballistic arc (gravity + horizontal air-drag),
**slaps onto the ground** at a fixed height where it freezes as a held splat,
then shrinks away to nothing. The whole effect is implemented by **`TBloodSystem`**
(`src/effectcomp.cpp:1182-1554`) — a self-contained particle system with a
3-stage per-droplet state machine. `TBloodEffect`/`TBloodAnimator` are thin
wrappers: the effect just holds 6 spawn parameters (`height, hangle, vangle,
hspread, vspread, num`), and the animator forwards them plus 8 mesh sub-objects
into `TBloodSystem::Init` and then calls `Animate`/`Render`/`RefreshZBuffer` on it
each frame (`src/effect_old.cpp:11269-11340`). Droplets are the red textures of
`Misc\Blood.I3D` (§4), drawn **scene-lit** (each droplet samples the three nearest
scene lights + ambient and tints itself gray-scale, §10) in **two passes** per
droplet (a translucent Alpha pass and an additive ONE/ONE overlay, §7). The blood
volume and gore detail scale with the global `ViolenceLevel` (0–5). It does **not**
fade by alpha — it changes **scale** through its life and is simply removed when
the splat finishes shrinking.

---

## 2. Sources & evidence

- **Retail decomp:** `recon/mappings/TBloodEffect_cls_0x5acaa8_candidate.yaml` —
  **SPARSE / MEDIUM-confidence.** Ghidra merged `TBloodEffect` with `TMistEffect`
  / `TMistFogEffect` / `TWaterFallEffect` into one 1032-byte file
  (`cls_0x5acaa8`); only XREF evidence (the `"Blood"` string at `005e11e8`),
  no extracted method bodies, no fields. `TBloodEffect` itself is identified as a
  **272-byte leaf `TEffect`** (yaml lines 19-22). No retail body exists for
  `TBloodSystem`/`TBloodAnimator` (grep: no `recon/classes/` entry). **Behavior
  must come from pre-release.**
- **Pre-release (authoritative):**
  - **Effect object** `TBloodEffect`: `DEFINE_BUILDER("Blood", TBloodEffect)`
    `src/effect_old.cpp:11243`; bodies `:11246-11253` (both empty — all gameplay is
    upstream in the combat caller). Class decl + the param accessors
    `SetParams`/`GetParams` at `src/effect.h:2405-2458` (the two accessors at
    `:2438-2439` are the original signature contract).
  - **Animator** `TBloodAnimator`: `REGISTER_3DANIMATOR("Blood", TBloodAnimator)`
    `src/effect_old.cpp:11261`; bodies `Initialize` `:11269-11297`, `Animate`
    `:11306-11317`, `Render` `:11326-11335`, `RefreshZBuffer` `:11337-11340`.
    Class decl `src/effect.h:2464-2486` (holds `TBloodSystem bloods;`).
  - **The real particle system** `TBloodSystem`: declared `src/effectcomp.h:376-402`
    (with `SBloodParticle` `:356-363`, `SBloodSystemParams` `:365-372`,
    `MAX_BLOODS` `:374`); implemented `src/effectcomp.cpp:1182-1554` — `Init`
    `:1182-1307`, `Animate` `:1309-1370`, `DoLighting` `:1372-1413`, `Render`
    `:1415-1530`, `RefreshZBuffer` `:1532-1554`. All `BLOOD_*` constants
    `:1164-1180`. **This is the live, authoritative mechanism.**
- **Caller (authoritative for spawn params):** `TCharacter::EffectBurst(name,
  height)` `src/character.cpp:2225-2318`; live `SetParams` calls / spawn sites at
  `:254` (decap fountain), `:1843`/`:1848` (impact/impale), `:1925` (death). The
  combat-hit param call is the **commented** `SetParams(...)` at
  `src/character.cpp:2260` (the original contract; see §5 + §13.2).
- **Existing port shell:** present in `src/effect.{h,cpp}` but **not consulted**
  per forensics hard-rule 8 (clean slate). Only class *declarations* /
  `SetParams`/`GetParams` *signatures* in `src/effect.h` were used to confirm the
  param contract.
- **Sister effects consulted:**
  - `THealAnimator::RefreshZBuffer` (`src/effect_old.cpp:835-850`) +
    `RestoreZ` (`:162-171`) — the Z-restore patch pattern (§7).
  - `TWaterFallAnimator` (`src/effect_old.cpp:11801-11810`) — the
    `GetAmbientLight()` scene-lit vertex-color pattern, the family `DoLighting`
    is in (§10).
  - `TParticle3DAnimator` / SPARKS forensics — the **builder-vs-real-class**
    subtlety (§ disambiguation below).
- **Source-of-truth ranking:** `src/effectcomp.cpp` (`TBloodSystem`) is
  authoritative for all visual behavior; `src/character.cpp` (`EffectBurst` +
  the `SetParams` sites) is authoritative for spawn parameters; `src/effect_old.cpp`
  is authoritative for the wiring (Init handshake). Retail is **sparse** and does
  not contradict — it only confirms `TBloodEffect` is a small leaf `TEffect`.

### Disambiguation — which "blood" is real

There are FOUR "blood" registrations / bodies in the source. Only one is live:

| # | site | what | status |
|---|------|------|--------|
| 1 | `REGISTER_3DANIMATOR("Blood", TBloodAnimator)` `effect_old.cpp:11261` | the **real** animator → drives `TBloodSystem` | **LIVE** |
| 2 | `//REGISTER_3DANIMATOR_("blood", TBloodAnimator, TBloodParticle3DAnimator)` `effect_old.cpp:4687` | commented-out registration of a generic-particle variant | **dead (commented)** |
| 3 | `REGISTER_MULTI_3DANIMATOR_("blood", TBloodAnimator, TParticle3DAnimator)` `effect_old.cpp:4691` | inside the comment block `:4689-4692` — would have routed `"blood"` to the **generic** `TParticle3DAnimator` (the sparks engine) | **dead (commented)** |
| 4 | `/* TBloodParticle3DAnimator::Initialize/Close/Animate/Render/RefreshZBuffer */` `effect_old.cpp:5008-5034` | a generic-particle blood subclass; its `Animate` body is just `TParticle3DAnimator::Animate(draw); // tenshu` (a placeholder, no custom logic) | **dead (commented)** |

So unlike SPARKS (where `TSparkAnimator` turned out to be a mere builder symbol
over the generic `TParticle3DAnimator`), **blood's real animator is NOT the
generic particle engine.** The generic-particle route was a dev experiment that
was *commented out*; the shipped blood is the bespoke `TBloodSystem`. The
do-not-reconstruct alternates (#2, #3, #4) are recorded only so a reconstruction
agent doesn't mistake them for the live path.

> **`TBloodSystem` vs `TPulpEffect`/`GetBlood`.** The task noted "`TBloodSystem`
> is also driven by `TPulpEffect` via `GetBlood()`." **This is not the case in the
> source.** `TPulpEffect` (gore/gib) has its **own**, separate inline blood system
> using a **different** struct `SBloodDrop` (`src/effect.h:1265-1338`), integrated
> by hand in `TPulpAnimator::Animate` (`src/effect_old.cpp:6779-6800`: `pos += vel;
> vel *= .75; ground = GetWalkHeight(map)`). `TBloodSystem` (with `SBloodParticle`)
> is referenced **only** by `TBloodAnimator` (`src/effect_old.cpp:11296-11339`) —
> confirmed by grep. The two share the "blood" concept but no code. The Pulp gore
> system is **out of scope** (it is B04 `TPulpEffect`); documented here only to
> rule out the cross-link.

---

## 3. Constants

All `BLOOD_*` defines from `src/effectcomp.cpp:1164-1180`; `MAX_BLOODS` from
`src/effectcomp.h:374`. RNG-range constants from `TBloodSystem::Init`
(`src/effectcomp.cpp:1220-1306`). Spawn-param values from the callers
(`src/character.cpp`). `random(min,max)` is **inclusive both ends**
(`src/revutils.cpp:1609`). All timing is at the intended 24 Hz sim tick.

### 3.1 Engine constants (in `TBloodSystem`)

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| `MAX_BLOODS` | 30 | count (particle array size) | effectcomp.h:374 | yes |
| `BLOOD_AIRRESIST` | 0.95 | per-tick multiplier on horizontal (x,y) velocity | effectcomp.cpp:1164 | yes |
| `BLOOD_GRAVITY` | 0.37 | wu/tick² subtracted from vel.z each tick (FLY stage) | effectcomp.cpp:1165 | yes |
| `BLOOD_FLY` | 0 | stage enum: airborne | effectcomp.cpp:1166 | yes |
| `BLOOD_SPLAT` | 1 | stage enum: stuck-on-ground, held | effectcomp.cpp:1167 | yes |
| `BLOOD_SHRINK` | 2 | stage enum: scaling down to death | effectcomp.cpp:1168 | yes |
| `BLOOD_SPLATSIZE` | 1.8 | scale clamp the droplet snaps to in SPLAT | effectcomp.cpp:1169 | yes |
| `BLOOD_SPLATDURATION` | 25 | ticks the splat is held before shrinking | effectcomp.cpp:1170 | yes |
| `BLOOD_SPLATSCALE` | 0.1 | scale decrement per tick in SHRINK | effectcomp.cpp:1171 | yes |
| `BLOOD_ZOFFSET` | 20 | wu — local z at which a flying droplet "hits ground" & splats | effectcomp.cpp:1172 | yes |
| `BLOOD_VEL` | 2.0 | wu/tick — base launch speed magnitude | effectcomp.cpp:1173 | yes |
| `BLOOD_SQUIRTDELAY` | 3 | ticks of start-delay added to each trail child | effectcomp.cpp:1174 | yes |
| `BLOOD_BIFDELAY` | 8 | initial value of `bifon` (big-splat-flash timer) | effectcomp.cpp:1175 | yes (but bif render is dead — §6.4) |
| `BLOOD_BIFSCALESTEP` | 0.5 | per-tick growth of `bifscale` (dead) | effectcomp.cpp:1176 | yes (dead — §6.4) |
| `BLOOD_FLYSCALESTEP` | 0.01 | per-tick scale growth while FLYing | effectcomp.cpp:1177 | yes |
| `BLOOD_FLYSCALE` | 1.0 | initial droplet scale at spawn | effectcomp.cpp:1178 | yes |
| `BLOOD_BLOODSPERSQUIRT` | 4 | trail-fill weighting (see `left` formula §6.1) | effectcomp.cpp:1179 | yes |
| `BLOOD_MOSTBLOODS` | 10 | trail-fill weighting reference count | effectcomp.cpp:1180 | yes |
| `me.maxsize` | 2 | max droplet size variant (set by animator, clamped by ViolenceLevel) | effect_old.cpp:11292 | yes |
| `DIVISOR` | 1.5 | lighting brightness clamp/normalize divisor (`DoLighting`) | effectcomp.cpp:1404 | yes |
| ambient weight | ×4.0 | ambient-light contribution multiplier in `DoLighting` | effectcomp.cpp:1402 | yes |

### 3.2 RNG ranges (in `TBloodSystem::Init`)

| quantity | range / formula | source | confirmed? |
|----------|-----------------|--------|------------|
| per-droplet horizontal angle `ha` | `(((hangle + random(-hspread, hspread)) & 255) * 360 / 256) * TORADIAN` | effectcomp.cpp:1222 | yes |
| per-droplet vertical angle `va` | `(((vangle + random(-vspread, vspread)) & 255) * 360 / 256) * TORADIAN` | effectcomp.cpp:1223 | yes |
| droplet size variant | `random(0, min(maxsize, 2))` | effectcomp.cpp:1225 | yes |
| velocity-magnitude jitter | `random(100, 300) / 100.0` ⇒ 1.0–3.0× `BLOOD_VEL` per axis | effectcomp.cpp:1229-1231 | yes |
| trail-child velocity (smalls) | parent vel × `random(60, 75) / 80` ⇒ 0.75–0.9375× | effectcomp.cpp:1297-1299 | yes |
| trail-child delay | parent.delay + `BLOOD_SQUIRTDELAY` + `random(-1, 1)` | effectcomp.cpp:1300 | yes |
| trail-child size | `random(0, 1)` | effectcomp.cpp:1293 | yes |

`TORADIAN = M_PI/180` (`src/revdefs.h:25`). The `& 255` makes angle a byte-angle
wrapped to 0–255 then mapped to 0–360°.

### 3.3 ViolenceLevel gates (global `ViolenceLevel`, 0–5, default 5)

`ViolenceLevel` declared `src/revenant.h:54`, defined/defaulted `5` at
`src/revmain.cpp:165`, settable 0–5 (`src/revmain.cpp:1581`).

| gate | effect | source |
|------|--------|--------|
| `bifon = 0 if !ViolenceLevel` | disables the (dead) bif flash at violence 0 | effectcomp.cpp:1207-1208 |
| `if (num > ViolenceLevel*2) num = ViolenceLevel*2` | caps droplet count to 2× violence | effectcomp.cpp:1214-1215 |
| `if (maxsize > ViolenceLevel) maxsize = ViolenceLevel` | caps max size variant | effectcomp.cpp:1216-1217 |
| `if (ViolenceLevel < 4) left = 0` | disables trail-fill below violence 4 | effectcomp.cpp:1238-1239 |
| `if (ViolenceLevel < 3) used = false` on splat | droplets vanish on impact (no splat held) below violence 3 | effectcomp.cpp:1341-1342 |

### 3.4 Spawn parameters from the callers (`SetParams(height, hangle, vangle, hspread, vspread, num)`)

| caller | call | meaning | source |
|--------|------|---------|--------|
| **decap fountain** (LIVE) | `SetParams(95, 0, 64, 255, 5, 1)` | height 95 (neck), hangle 0, **vangle 64 (=90° = straight up)**, **hspread 255 (full 360° random)**, vspread 5, num 1 (× many calls every 5 frames) | character.cpp:254 |
| **combat hit / impale** | `SetParams(height, (GetFace()+128)&255, 0, 80, 20, random(1,5))` | hangle = attacker-facing **+128 = opposite (sprays back toward attacker / away from victim's facing)**, vangle 0 (horizontal), hspread 80, vspread 20, num 1–5 | character.cpp:2260 (commented in port; original contract — §13.2) |
| `EffectBurst` height arg | `40` if impale else `50` | added to victim z as the spawn height | character.cpp:1843,1848,1925 |

---

## 4. Assets

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| Blood | `legacy/Imagery/Misc/Blood.I3D` | 68,820 B | the blood-droplet/splat imagery — a **STILL** (non-morph, billboard) I3D with **8 sub-objects** + 2 red textures | registered `Class.Def:2014` as `"Blood" "Misc\Blood.I3D" 0xddc4042e`; loaded by the `OBJCLASS_EFFECT` registry when the Blood effect spawns; bound to `TBloodAnimator` |

**Header facts** (`legacy/Imagery/Misc/Blood.I3D`): magic `CGSR`; type tag
`STILL` at file offset `0x1c` (a still/billboard imagery, no per-frame morph);
8 sub-objects (count at `0x94`); 2 textures (`0x84`), dimensions in the header at
`0x44-0x4c`: one **128 × 160** surface and one **64 × 120** surface. RGB565 pixel
data; **no `framehtexs` multi-frame array** → no flipbook (§8).

**Sub-objects (8)** — read from the I3D object name table (28-byte records from
file offset `0xb4`): `box01`…`box08`. The animator maps them by index in
`TBloodAnimator::Initialize` (`src/effect_old.cpp:11284-11291`):

| GetObject(n) | name | param field | role |
|-------------|------|-------------|------|
| 0 | box01 | `s2` | small droplet, **pass-2 (additive)** |
| 1 | box02 | `m2` | medium droplet, pass-2 |
| 2 | box03 | `b2` | big droplet, pass-2 (unused — §7) |
| 3 | box04 | `sp2` | splat, pass-2 (unused — bif is dead, §6.4) |
| 4 | box05 | `s` | small droplet, **pass-1 (alpha)** |
| 5 | box06 | `m` | medium droplet, pass-1 |
| 6 | box07 | `b` | big droplet, pass-1 (unused — §7) |
| 7 | box08 | `sp` | splat, pass-1 (unused — bif dead) |

So there are **two identical sets of 4 quads** (small / medium / big / splat).
Each droplet is rendered once from set-1 (alpha blend) and once from set-2
(additive overlay), §7. The "big" (size 2) and "splat" (sp) sub-objects are
**addressed but never drawn** by live code (size 2 is clamped out and the big/sp
render branches are commented, §7/§6.4).

**Textures are blood-red** (decoded RGB565): the 128×160 surface averages
**RGB(170, 0, 0)** over its non-black pixels (deep arterial red); the 64×120
surface is also red. Background is **pure black** = the chroma-key transparency
convention (knowledge 02 §4). The asset is genuinely red — **do NOT substitute a
procedural red blob**; the visual identity (droplet shape + splat shape + red
gradient) is in this mesh's authored quads + textures.

> Note — separate assets, do not confuse: `Blood1/2/3.I2D` and `BloodS1-4.I2D`
> (`Class.Def:305-311`) are **2D** decal images used by the **Pulp** gore system
> (`sprintf("Blood%d")` / `"BloodS%d"`, `src/effect_old.cpp:6894,6905`), and
> `Magic\blood.i3d` / `Magic\bloods.i3d` are unregistered dev copies. The
> registered asset for `TBloodSystem` is **`Misc\Blood.I3D`** only.

---

## 5. Spawn & emit

- **Trigger semantics:** **one-shot burst** per spawn. `TBloodSystem::Init` seeds
  every droplet at once (`src/effectcomp.cpp:1220-1306`); the effect self-kills
  when all droplets finish (`bloods.GetDone()` → `OF_KILL`,
  `src/effect_old.cpp:11313-11316`). (The decap *fountain* re-spawns a fresh Blood
  effect every 5 frames from the caller, `src/character.cpp:234`, but each Blood
  effect itself is one-shot.)
- **Count per trigger:** `num` (1 for decap, `random(1,5)` for combat),
  clamped to `min(num, ViolenceLevel*2)` then `min(num, MAX_BLOODS)`
  (`src/effectcomp.cpp:1214,1220`). Plus **trail-fill**: at ViolenceLevel ≥ 4 the
  array is back-filled with extra small droplets until a budget `left` is
  exhausted (§6.1), so a single primary squirt can balloon toward `MAX_BLOODS=30`.
- **Initial direction / distribution:** spherical-coordinate launch, **biased by
  the caller's `hangle`/`vangle`**, NOT a fixed radial cone. Per droplet
  (`src/effectcomp.cpp:1222-1231`):
  ```
  ha = byteangle(hangle + random(-hspread, hspread)) → radians   // horizontal heading
  va = byteangle(vangle + random(-vspread, vspread)) → radians   // vertical pitch
  speed = BLOOD_VEL(2.0) * random(100,300)/100        // 2.0..6.0 wu/tick
  vel.x = speed * cos(ha)
  vel.y = speed * sin(ha)
  vel.z = speed * sin(va)
  ```
  - **Decap** (`hangle 0, hspread 255, vangle 64=90°, vspread 5`): horizontal
    heading is **fully random over 360°**, vertical pitch ≈ 90° → droplets shoot
    **straight up** and rain back down — a vertical fountain.
  - **Combat hit** (`hangle (face+128), hspread 80, vangle 0, vspread 20`):
    horizontal heading is centered **opposite the attacker's facing** (i.e. blood
    sprays roughly **back toward the attacker / away from where the victim is
    looking**) over a ±~112° fan; vertical pitch ≈ horizontal (vangle 0) with
    ±~28° scatter — a near-horizontal directional spray. **This spray is biased,
    not radial.**

  > **Non-obvious fact:** `va` uses `sin(va)` for `vel.z` only — `cos(va)` is NOT
  > applied to scale the horizontal magnitude, so `vel.x/y` already carry the full
  > `speed*cos(ha)/sin(ha)`. With `vangle=64` (`sin 90°≈1`) the up-component equals
  > the (separately-scaled) horizontal speed; the launch is not a true normalized
  > 3D direction. Reproduce the formula literally (`vel.z = speed*sin(va)`), do not
  > "fix" it to a unit sphere.

- **Emit anchor convention:** the Blood *effect object* is placed at the victim's
  world position `pos`, raised by the `height` argument: `def.pos = pos;
  def.pos.z += height;` (`src/character.cpp:244-247,2244-2245`). Inside the system
  every droplet starts at **local (0, 0, height)** — `pos.x=pos.y=0`,
  `pos.z = me.height` (`src/effectcomp.cpp:1226-1228`), where `me.height` is the
  same `height` value passed via `GetParams`. (So the droplet's local-z `height`
  cancels against the render offset `pos.z - height`, §7 — the visual originates at
  the object origin, i.e. the wound point, and falls to `local z = BLOOD_ZOFFSET`.)
- **Coordinate space:** droplet positions/velocities are integrated in the
  effect's **local space** (origin = the object's world pos = wound point); render
  places each sub-object at the local particle pos via `OBJ3D_MATRIX` (§7).
- **Spread / jitter:** horizontal ±`hspread` byte-angle, vertical ±`vspread`
  byte-angle, speed 1.0–3.0× base each axis; size `random(0..min(maxsize,2))`.

### Spatial diagram

```
DECAP fountain (hangle 0/hspread 255, vangle 90°):     COMBAT hit (hangle face+180°, vangle 0):
   wz ▲   droplets shoot UP in random              wz ▲
      │   horizontal headings, rain back            │   victim faces →   attacker
      │      · ·  ·                                  │    ░░░ spray ░░░    (X)
      │     · ·· · ·   ← 360° random ha              │  ◄══ ░░ wound ░░ ══  blood sprays BACK
      │    ·  ·· ·                                   │       toward attacker (hangle=face+128),
   ●──┴──────────── wx  ● = wound @ z=height(95)  ●──┴──── near-horizontal, ±~112° fan
  (neck)                                          (z=height 40/50)

per droplet: launch (vel from ha,va,speed) → FLY (gravity 0.37 down, x/y *0.95 drag,
  scale +0.01/tick) → hit local z=BLOOD_ZOFFSET(20) → SPLAT (snap scale 1.8, hold 25t)
  → SHRINK (scale −0.1/tick) → die at scale 0.
```

---

## 6. Behavior & per-frame logic

Everything lives in `TBloodSystem`. `TBloodAnimator::Animate` simply calls
`bloods.Animate()` and kills the effect when `bloods.GetDone()` is true
(`src/effect_old.cpp:11306-11317`).

### 6.1 Seeding (`TBloodSystem::Init`, `src/effectcomp.cpp:1182-1307`)

```
Init(me):                                           // :1182
    cache animator, 8 sub-objects, size(zbuf=10,10,0), height, effectpos
    for each of the 8 sub-objects: animator->GetVerts(obj, D3DVT_LVERTEX)   // :1195-1202 (lit-vertex)
    bifon = BLOOD_BIFDELAY(8);  if !ViolenceLevel: bifon = 0                 // :1206-1208
    bifscale = BLOOD_BIFSCALESTEP(0.5)                                        // :1209

    left = max(MAX_BLOODS - (BLOOD_MOSTBLOODS - num)*BLOOD_BLOODSPERSQUIRT,  // :1211
               BLOOD_BLOODSPERSQUIRT)         // trail-fill budget
    if num > ViolenceLevel*2: num = ViolenceLevel*2          // :1214-1215
    if maxsize > ViolenceLevel: maxsize = maxsize-clamp      // :1216-1217

    // PRIMARY droplets
    for i in 0 .. min(num, MAX_BLOODS)-1:                    // :1220
        ha = byteangle(hangle + random(-hspread,hspread)) → rad   // :1222
        va = byteangle(vangle + random(-vspread,vspread)) → rad   // :1223
        used=true; size=random(0,min(maxsize,2))                  // :1224-1225
        pos = (0, 0, height)                                       // :1226-1228
        vel = (BLOOD_VEL*cos(ha), BLOOD_VEL*sin(ha), BLOOD_VEL*sin(va)) * random(100,300)/100  // :1229-1231
        scl = BLOOD_FLYSCALE(1.0); stage=FLY; delay=0             // :1232-1234
        left--

    if ViolenceLevel < 4: left = 0                            // :1238-1239 (no trail-fill)

    // (bigs→ / meds→ trail-fill loops are COMMENTED OUT, :1242-1283 — do not reconstruct)

    // TRAIL-FILL with smalls (the only live trail loop, :1287-1306)
    for i = 0; left; i++:                                      // walks existing droplets
        if blood[i].used && num < MAX_BLOODS:
            blood[num].used=true; size=random(0,1)              // small/med
            pos=(0,0,height)
            vel = parent.vel * random(60,75)/80     // 0.75..0.94× parent velocity   // :1297-1299
            delay = parent.delay + BLOOD_SQUIRTDELAY(3) + random(-1,1)               // :1300
            stage=FLY; scl=1.0
            num++; left--
```

- **Trail-fill** multiplies a small primary squirt into a denser spray by cloning
  existing droplets at slightly reduced velocity and a staggered start `delay`, so
  the secondary droplets emerge a few ticks later "trailing" the primaries. Only
  the smalls loop is live; the bigs/meds trail loops are commented out (do not
  reconstruct). Gated off entirely below ViolenceLevel 4.

### 6.2 Per-tick update (`TBloodSystem::Animate`, `src/effectcomp.cpp:1309-1370`)

```
Animate():                                          // :1309
    done = true
    for i in 0 .. MAX_BLOODS-1:                      // :1312
        if !used: continue
        if delay: delay--; continue                 // start-delay countdown   // :1316-1320
        done = false

        if stage == FLY:                            // :1324
            pos += vel                              // Euler integrate          // :1326-1328
            vel.x *= BLOOD_AIRRESIST(0.95)          // horizontal drag          // :1330
            vel.y *= BLOOD_AIRRESIST(0.95)          //                          // :1331
            vel.z -= BLOOD_GRAVITY(0.37)            // gravity (z only)         // :1332
            scl += BLOOD_FLYSCALESTEP(0.01)         // droplet grows in flight  // :1334
            if pos.z <= BLOOD_ZOFFSET(20):          // hit the ground plane     // :1336
                pos.z = BLOOD_ZOFFSET; stage = SPLAT; count = 0                 // :1338-1340
                if ViolenceLevel < 3: used = false  // low violence: no splat   // :1341-1342
                if size == 0:        used = false   // smallest droplets vanish on impact  // :1343-1344

        else if stage == SPLAT:                     // :1347
            scl = BLOOD_SPLATSIZE(1.8)              // snap to held splat size   // :1349
            count++                                                              // :1350
            if count > BLOOD_SPLATDURATION(25): stage = SHRINK; count = 0        // :1351-1355

        else if stage == SHRINK:                    // :1357
            scl -= BLOOD_SPLATSCALE(0.1)            // shrink                    // :1359
            if scl <= 0: scl = 0; used = false      // die                       // :1360-1365
    bifon--; bifscale += BLOOD_BIFSCALESTEP         // (bif counters; render dead) // :1368-1369
```

- **Velocity integration:** plain Euler. **Gravity acts on z only**
  (`vel.z -= 0.37`); **horizontal velocity decays** by ×0.95/tick (air-drag) —
  this is why blood sprays out then curves down and "slows" laterally, rather than
  flying in a clean parabola. Reproduce both.
- **Ground-plane interaction:** a droplet "hits ground" when its **local** z drops
  to `BLOOD_ZOFFSET = 20` (not 0 — there is a 20-wu floor offset). It then snaps to
  `pos.z = 20` and enters SPLAT. There is **no bounce, no pooling/decal mesh** — the
  splat is the same billboard, just scaled to 1.8 and held flat (§7).
- **Death conditions:** smallest droplets (size 0) and *all* droplets at
  ViolenceLevel < 3 die immediately on impact (no splat). Otherwise a droplet
  holds its splat for 25 ticks then shrinks at 0.1/tick to zero and dies.
- **Effect end:** when every used droplet has died, `done` stays true →
  `TBloodAnimator::Animate` sets `OF_KILL` (`src/effect_old.cpp:11313-11316`).

> **Non-obvious fact: droplets change SCALE, not ALPHA.** There is no alpha or
> color fade over life — the lifecycle is entirely a scale curve (grow slowly in
> flight 1.0→, snap to 1.8 on splat, shrink to 0). Per-vertex color each frame
> comes only from `DoLighting` (scene brightness, §10), not a fade envelope.

### 6.3 Temporal diagram (one droplet, scale vs time)

```
scale
 1.8 ┤            ┌──────────────┐
     │           ╱│   SPLAT hold │╲
 1.0 ┤··········╱ │   (25 ticks) │ ╲ SHRINK (−0.1/tick)
     │  FLY    ╱  │              │  ╲
     │ grow  ╱    │              │   ╲
   0 ┼─────╱──────┴──────────────┴────╲──── ticks
       launch   impact(z<=20)        die(scale 0)
     (+0.01/tick grow during FLY; impact time depends on launch va & gravity)
position z: launch at z=height → parabola down (g 0.37, x/y drag 0.95) → clamps at z=20
```

### 6.4 Dead code — do NOT reconstruct

- **The "bif" big-splat flash.** `bifon`/`bifscale` are maintained each tick
  (`src/effectcomp.cpp:1206-1209,1368-1369`) and a render block exists to draw the
  `sp`/`sp2` (splat) sub-objects scaled by `bifscale` while `bifon > 0` — but that
  entire render block is **commented out** (`src/effectcomp.cpp:1485-1529`). So the
  splat sub-objects (GetObject 3, 7) are never drawn and the bif flash never
  appears. Record as do-not-reconstruct.
- **Bigs/meds trail-fill loops** (`src/effectcomp.cpp:1242-1283`) — commented.
- **The "big" (size 2) droplet draw** — the `size == 2 → object = big` branches in
  Render are commented (`src/effectcomp.cpp:1447-1448,1455-1456`), and size 2 only
  occurs when `maxsize ≥ 2` which requires `ViolenceLevel ≥ 2`; even then the big
  sub-object is never selected, so size-2 droplets fall through to the **small**
  object. Effectively only small (box05/box01) and medium (box06/box02) quads draw.
- The `TBloodParticle3DAnimator` generic-particle alternate (§2 #4) and the
  commented `"blood"` registrations (§2 #2, #3).

---

## 7. Rendering (original render state + geometry)

`TBloodSystem::Render` (`src/effectcomp.cpp:1415-1530`). For each used droplet,
**two passes** (`for k in 0..1`):

```
Render():                                            // :1415
    for i in 0 .. MAX_BLOODS-1:                       // :1420
        if !used: continue
        for k in 0..1:                                // TWO passes per droplet   // :1425
            if k == 0:  RestoreBlendState(); SetBlendState()      // PASS 1: Alpha // :1429-1430
            else:       RestoreBlendState();                       // PASS 2: additive overlay
                        SetRenderState(TEXTUREMAPBLEND, MODULATE)  //              // :1435
                        SetRenderState(SRCBLEND,  ONE)             //              // :1437
                        SetRenderState(DESTBLEND, ONE)             //              // :1438
            object = (k==0 ? sml : sml2)                           // pass→set selector
            if size == 1: object = (k==0 ? med : med2)             // (size 2 / big commented out)
            animator->ResetExtents()
            object->flags = OBJ3D_MATRIX | OBJ3D_VERTS              // :1461
            D3DMATRIXClear(matrix)
            scl = blood[i].scl (uniform x=y=z); D3DMATRIXScale(matrix, scl)   // :1464-1465
            if stage == FLY:                                        // only while airborne   // :1467
                D3DMATRIXRotateX(matrix, -90°)    // tip authored-XY quad onto ground  // :1469
                D3DMATRIXRotateZ(matrix, -45°)    // in-plane spin                       // :1470
            pos = (blood[i].pos.x, .y, .z - height)                // local placement      // :1473-1475
            D3DMATRIXTranslate(matrix, pos)
            DoLighting(blood[i].pos.x, .y, .z, object)             // per-vertex scene tint  // :1478
            animator->RenderObject(object)                         // :1480
            animator->UpdateExtents()
```

- **What it draws:** for each live droplet, one small or medium billboard quad
  from set-1 **and** the matching quad from set-2 (additive), both at the same
  position/scale. Net: every droplet is a small red quad with a brighter additive
  core on top.
- **Blend mode (original):** **two passes, two modes.**
  - **Pass 1 (`k==0`): Alpha.** `SetBlendState()` (`src/effect_old.cpp:221-233`)
    = `TEXTUREMAPBLEND = D3DTBLEND_MODULATE`, `SRCBLEND = SRC_ALPHA`,
    `DESTBLEND = INV_SRC_ALPHA`, `ZWRITE = false`, `ZENABLE = true` — translucent
    Alpha (modulated by vertex color).
  - **Pass 2 (`k==1`): AdditiveStraight.** Set inline (`src/effectcomp.cpp:1435-1438`):
    `TEXTUREMAPBLEND = MODULATE`, `SRCBLEND = ONE`, `DESTBLEND = ONE` (depth states
    *not* re-touched in pass 2, so they keep pass-1's `ZWRITE=false`, `ZENABLE=true`).
    This is an **additive overlay** on top of the alpha droplet — a self-lit
    bright-red glint over the translucent body.
  - The pass loop calls `RestoreBlendState()` before each pass to reset to the
    saved snapshot, then sets the pass mode. `TBloodAnimator::Render` wraps the
    whole thing in `SaveBlendState()/RestoreBlendState()` (`src/effect_old.cpp:11328-11332`).

  > **Non-obvious fact: blood is drawn in TWO passes per droplet** — an Alpha pass
  > then an Additive ONE/ONE pass. A faithful build must do both; a single Alpha
  > pass loses the bright additive glint and reads dull.

- **Lit vs self-lit:** **scene-LIT (LitFlat).** Unlike most magic glows (which
  zero the material and are self-lit), blood **samples the scene** every frame:
  `DoLighting` (§10) reads the 3 nearest scene lights + ambient and writes a
  grayscale brightness into all 4 vertices' diffuse, which (under MODULATE)
  multiplies the red texel. So blood **darkens in shadow and brightens near
  lights** — it tracks the environment. Reproduce as scene-lit, not constant.
- **Depth / Z:** **TestNoWrite** — `ZENABLE = true`, `ZWRITEENABLE = false` (set by
  `SetBlendState` in pass 1, `src/effect_old.cpp:224-225`; pass 2 leaves them).
  `RefreshZBuffer` restores scene Z under each droplet (§ below).
- **Orientation:** while **FLY**ing, each quad is tipped onto the ground plane
  (`rot.x = -90°`, the **WorldXY** tell) and given a static in-plane spin
  (`rot.z = -45°`) (`src/effectcomp.cpp:1469-1470`). While **SPLAT/SHRINK** these
  rotations are **omitted** (the `if stage == FLY` guard), so the splat draws with
  only scale+translate — i.e. in the mesh's authored orientation (a flat splat).
  Reproduce: airborne droplets are ground-tipped & 45°-spun; landed splats are not
  re-rotated.
- **Per-quad transform:** `OBJ3D_MATRIX | OBJ3D_VERTS` — a full matrix
  (scale → [rotX → rotZ if flying] → translate) plus the animator's lit-vertex
  buffer (`GetVerts(... D3DVT_LVERTEX)` cached in Init). Uniform scale = `blood[i].scl`.
- **Per-vertex color packing:** written by `DoLighting` as `D3DRGB(r, r, r)` into
  all 4 `lverts[j].color` (`src/effectcomp.cpp:1409-1412`) — a grayscale brightness
  (alpha implicit/opaque via `D3DRGB`). Under MODULATE this scales the red texel by
  scene brightness. The effect adds **no red tint of its own** — the red is in the
  texture.

### RefreshZBuffer (`src/effectcomp.cpp:1532-1554`)

For each used droplet: world pos = effect pos + droplet local pos; project with
`WorldToScreen`; `RestoreZ(screenx - size.x*scl/2, screeny - size.y*scl/2,
size.x*scl, size.y*scl)` where `size = (10, 10, 0)` (the `zbuf` set in
`TBloodAnimator::Initialize`, `src/effect_old.cpp:11279-11293`). So a small
(~10×scl px) Z-restore patch is stamped per droplet so the no-depth-write blood
composites correctly against later geometry.

---

## 8. Texture animation

**N/A — none.** `Blood.I3D` is a STILL imagery with no `framehtexs` multi-frame
array (§4). `TBloodSystem::Render` never mutates `tu/tv` and never calls
`SetTextureFrame` — confirmed (`src/effectcomp.cpp:1415-1530`). The textures are
static; all motion is positional (the droplet flying) and the scale curve. No UV
scroll, no flipbook.

---

## 9. Associated light

**N/A — none emitted.** `TBloodSystem` makes no `AddPointLight` / dynamic-light
call (Init/Animate/Render, `src/effectcomp.cpp:1182-1530`); the caller
`EffectBurst` adds none. Blood does not *cast* light. (It *receives* light — see
§10 `DoLighting` — but it does not add any to the scene.)

---

## 10. Color

- **Source:** the **authored `Misc\Blood.I3D` red textures** (§4), modulated
  per-vertex by a **scene-brightness** value computed each frame in
  `TBloodSystem::DoLighting` (`src/effectcomp.cpp:1372-1413`). The effect supplies
  no hardcoded RGB and reads no spell/chardata color — the hue is the texture's
  red; the brightness is the scene's.
- **Exact values:** texture ≈ **RGB(170, 0, 0)** average over opaque pixels (deep
  arterial red, decoded RGB565 from the 128×160 surface); 0–255 domain. The
  per-vertex modulator is grayscale `r = g = b` in 0..1, computed as:
  ```
  DoLighting(x,y,z):                                  // effectcomp.cpp:1372
      r = 0
      Scene3D.GetClosestLights(eff+x, eff+y, z) → l1,l2,l3            // :1381
      if l1>-1:                 r += brightness(l1)                    // :1384-1385
      if l2>-1 && MaxLights>1:  r += brightness(l2)                   // :1390
      if l3>-1 && MaxLights>2:  r += brightness(l3)                   // :1396
      r += (GetAmbientLight()/255.0) * 4.0            // ambient ×4    // :1400-1402
      if r > DIVISOR(1.5): r = 1.5;  r /= 1.5         // clamp+normalize to 0..1   // :1404-1407
      g = b = r
      for j in 0..3: object->lverts[j].color = D3DRGB(r, g, b)        // :1409-1412
  ```
  So a fully-lit droplet (`r→1.5+`) renders at full texture red; a droplet in
  darkness (only ambient) renders dimmer red. The ambient term is weighted ×4 and
  the whole sum is clamped at 1.5 then normalized — meaning even modest scene light
  saturates blood to its full red. This is the **LitFlat** pattern (same family as
  `TWaterFallAnimator`, knowledge 03 §2).
- **Expected visual:** dark, saturated **blood-red** droplets and splats; bright
  near lights, deeper in shadow; with an additive red glint from pass 2. A
  **pale/pink/gray** result at reconstruction = broken port (likely culprits per
  AGENT_GUIDE §4.2.1.5: a procedural stand-in instead of the real Blood.I3D #1,
  wrong blend, a missed chroma-key fringe, or `DoLighting` not wired so brightness
  defaults wrong). Blood is *meant* to be deep red, not bright pink.
- **Normalization / boosts:** the `DIVISOR = 1.5` clamp+normalize in `DoLighting`
  (`src/effectcomp.cpp:1404-1407`); ambient ×4 boost (`:1402`).

> **Non-obvious fact: blood is scene-LIT, not self-lit.** A faithful build must
> sample scene/ambient light per droplet and modulate the red texture by it
> (grayscale `D3DRGB(r,r,r)`). Drawing it at constant full-bright red is wrong —
> blood in a dark room should be dim.

---

## 11. Audio coupling

**No audio coupling in the effect.** `TBloodSystem` and `TBloodAnimator` issue no
sound calls (`src/effectcomp.cpp:1182-1554`, `src/effect_old.cpp:11269-11340`).
The combat caller `EffectBurst` plays no blood-specific sound either
(`src/character.cpp:2225-2318`) — impact/hit sounds come from the separate combat
audio path, not the blood VFX. Record: **no blood-specific SFX**; any "wet impact"
sound the audio phase wants would be added at the combat hit site, not in
`TBloodSystem`.

---

## 12. Triggers & in-game appearance

- **Spawned by:** combat damage resolution in `TCharacter`, via
  `EffectBurst("blood", height)` (`src/character.cpp:2225`), at these live sites:
  - **Impact / impale** — on first-time impact with `damage > 0` and (no attack or
    the attack's `CA_BLOOD` flag `0x0200`, `src/rules.h:41`):
    `EffectBurst("blood", impale ? 40 : 50)` (`src/character.cpp:1846-1848`); plus a
    random impale spurt `if (impale && random(0,5)==1)` (`:1842-1843`).
  - **Death** — first-time dead with `CA_BLOOD`: `EffectBurst("blood", ...)`
    (`src/character.cpp:1924-1925`); the death state itself is forced as a `"blood"`
    action block (`:263`).
  - **Decapitation fountain** — while in the `impdecap` state, frames 6–64 every
    5th frame: spawn a Blood effect directly with `SetParams(95, 0, 64, 255, 5, 1)`
    (`src/character.cpp:232-255`) — the straight-up neck fountain.
  - **Suppression:** if the victim is `burning`, blood is suppressed
    (`src/character.cpp:2229-2230`) — burning enemies don't bleed.
  - Registration: `DEFINE_BUILDER("Blood", TBloodEffect)` `src/effect_old.cpp:11243`
    + `REGISTER_3DANIMATOR("Blood", TBloodAnimator)` `:11261`; asset
    `"Blood" "Misc\Blood.I3D"` `Class.Def:2014`. (Callers look up `FindObjType("blood")`
    lowercase — the lookup is case-insensitive.)
- **Where to see it in the original game:** hit any enemy that bleeds (most humans)
  with a melee weapon — blood squirts from the wound on each damaging hit. For the
  fountain, decapitate an enemy (a kill that triggers the `impdecap` death state) —
  blood jets straight up from the neck for ~60 frames. Set `ViolenceLevel = 5` to
  see the full spray + trail-fill + held splats.
- **Vestigial?** No — multiple live combat callers.

---

## 13. Gaps & uncertainties

- **13.1 Retail decomp is sparse.** `recon/mappings/TBloodEffect_cls_0x5acaa8_candidate.yaml`
  is a Ghidra merge of Blood+Mist+MistFog+WaterFall with **no extracted bodies**
  for any blood class and no `TBloodSystem` recon at all. All behavior here is
  **pre-release-authoritative**; the pre-release bodies are complete, so risk is
  low, but there is no retail cross-check of the constants/state-machine. A Ghidra
  rerun that isolates the Blood class is the open item (also noted in INVENTORY
  B01a).
- **13.2 The combat-hit `SetParams` is commented in the port.** The combat-hit
  path's parameter call is currently `// ((TBloodEffect*)inst)->SetParams(height,
  (GetFace()+128)&255, 0, 80, 20, random(1,5));` (`src/character.cpp:2260`),
  disabled with a port comment about a vtable/typeinfo issue. **This commented line
  is the original retail spawn contract** for combat hits and is the correct
  parameterization to reconstruct (the decap path at `:254` is live and uncommented
  and corroborates the `SetParams` signature `(height, hangle, vangle, hspread,
  vspread, num)`). Treat the combat-hit params as authoritative from `:2260`.
- **13.3 `va`/`vel.z` is not a unit-sphere direction.** As noted in §5, `vel.z =
  speed*sin(va)` without a matching `cos(va)` factor on horizontal magnitude. This
  is the source's literal math; reproduce it verbatim. (Reasonable interpretation:
  the author wanted a quick "pitch adds upward speed" launch, not a normalized
  sphere — at `vangle=90°` it makes the fountain shoot up at ~the same speed as the
  horizontal scatter.)
- **13.4 `me.maxsize = 2` but big never draws.** The animator sets `maxsize=2` and
  Init clamps it by ViolenceLevel; droplets can be `size` 0/1/2. But the Render
  size-2 / big-object branch is commented (§6.4), so size-2 droplets render with
  the **small** object. So in practice only two visible droplet sizes exist (small,
  medium). Could-not-determine whether shipping intended a third visible size; the
  source as written draws two. Low impact — match the source (small/med only).
- **13.5 Splat (`sp`) sub-objects never draw.** The bif flash that would draw
  box04/box08 is fully commented (§6.4). The "splat" is therefore the *droplet*
  quad held at scale 1.8, not a dedicated splat sprite. If in-game footage shows a
  distinct splat decal shape, it would come from the held droplet quad (or from the
  separate Pulp `.I2D` decals, which are a different system). From this code path,
  the splat = the scaled droplet.
- **13.6 No surface/decal persistence.** Blood does not leave a permanent decal in
  this system — the splat holds 25 ticks then shrinks away. (Persistent floor blood,
  if any in-game, is the Pulp system's `.I2D` decals, B04, out of scope.)
- **13.7 Exact texture red.** RGB(170,0,0) is the decoded average; the precise per-
  pixel gradient (droplet has a darker rim / brighter center) should be confirmed by
  rendering the real texture during reconstruction — but it is unambiguously deep
  red, no guess needed for the hue.

---

## 14. Reconstruction burndown

```
- [ ] Load Misc\Blood.I3D (68,820 B, Class.Def:2014). It is a STILL imagery with 8
      sub-objects box01..box08 and 2 RED textures (128x160 + 64x120, RGB565, pure-
      black chroma-key bg). NO procedural blood blob — use the real red quads. (§4)
- [ ] Map the 8 sub-objects: set-1 (alpha pass) = box05/06/07/08 (s/m/b/sp); set-2
      (additive pass) = box01/02/03/04 (s2/m2/b2/sp2). Only small+medium draw; big &
      splat (sp) sub-objects are addressed but never drawn (dead code). (§4, §6.4, §7)
- [ ] Drive it as the bespoke TBloodSystem (NOT the generic TParticle3DAnimator) —
      thin TBloodAnimator forwards GetParams + the 8 sub-objects into TBloodSystem::
      Init, then calls Animate/Render/RefreshZBuffer each frame; effect self-kills on
      bloods.GetDone(). (§2, §6)
- [ ] Spawn = one-shot burst of `num` primary droplets seeded at once at local
      (0,0,height); num & maxsize clamped by ViolenceLevel; effect placed at victim
      world pos + height z. (§5, §6.1)
- [ ] Directional launch (NOT radial cone): per droplet ha=byteangle(hangle±hspread),
      va=byteangle(vangle±vspread); speed = BLOOD_VEL(2.0) * random(100,300)/100;
      vel=(speed*cos ha, speed*sin ha, speed*sin va). Combat hit sprays opposite the
      attacker facing (hangle=face+128, hspread 80, vangle 0); decap is a 360°
      straight-up fountain (hangle 0/hspread 255, vangle 64=90°). Reproduce the
      non-unit-sphere vel.z formula literally. (§5, §3.2, §13.3)
- [ ] Trail-fill (ViolenceLevel>=4 only): clone existing droplets into extra smalls
      at 0.75-0.94x velocity with staggered delay (parent.delay + BLOOD_SQUIRTDELAY
      + random(-1,1)) until budget `left` exhausts; bigs/meds trail loops are
      commented (do not reconstruct). (§6.1)
- [ ] 3-stage state machine per droplet at 24 Hz: FLY (pos+=vel; vel.x/y *=0.95 drag;
      vel.z -=0.37 gravity; scl +=0.01) → on local z<=BLOOD_ZOFFSET(20) snap to z=20,
      enter SPLAT → SPLAT (scl=1.8, hold 25 ticks) → SHRINK (scl -=0.1/tick) → die at
      scl 0. (§6.2, §6.3)
- [ ] Death rules: size-0 droplets and ALL droplets at ViolenceLevel<3 die on impact
      (no splat); otherwise hold-then-shrink. (§6.2)
- [ ] Lifecycle changes SCALE, not alpha/color — no fade envelope. (§6.2, §10)
- [ ] Render TWO passes per droplet: pass 1 Alpha (SetBlendState: MODULATE,
      SRC_ALPHA/INV_SRC_ALPHA, ZWRITE off, ZTEST on); pass 2 AdditiveStraight
      (MODULATE, ONE/ONE, depth states inherited). Both required. (§7)
- [ ] Orientation: while FLYing, tip quad to ground (rot.x=-90°, WorldXY) + in-plane
      spin (rot.z=-45°); while SPLAT/SHRINK omit both rotations (authored splat
      orientation). Uniform scale = blood[i].scl. Transform = OBJ3D_MATRIX|OBJ3D_VERTS,
      pos.z = local pos.z - height. (§7)
- [ ] Depth = TestNoWrite (ZTEST on, ZWRITE off). RefreshZBuffer: per droplet
      WorldToScreen the droplet pos, RestoreZ a ~10x10*scl px patch centered on it
      (size=(10,10,0)). (§7)
- [ ] Color = the real Blood.I3D red texture (~RGB 170,0,0), modulated per-vertex by
      DoLighting: sum 3 nearest scene lights + ambient*4, clamp/normalize by 1.5, write
      grayscale D3DRGB(r,r,r) to all 4 verts each frame ⇒ blood is SCENE-LIT (LitFlat),
      dim in shadow, full-red in light. Pale/pink = broken. (§10, §7)
- [ ] Texture animation: NONE (static red sprites, no UV scroll, no flipbook). (§8)
- [ ] Associated dynamic light: NONE emitted (blood receives light, casts none). (§9)
- [ ] Audio: NONE in the effect (impact sound is in combat audio, not here). (§11)
- [ ] Sub-effects spawned: NONE (the decap fountain is the CALLER re-spawning whole
      Blood effects every 5 frames, not a sub-emit). (§5, §12)
- [ ] Trigger wiring (record for integration): EffectBurst("blood", height) on
      damaging impact / impale / death with CA_BLOOD (rules.h:41); suppressed if
      victim burning; decap fountain SetParams(95,0,64,255,5,1) at character.cpp:254;
      combat-hit SetParams(height,(face+128)&255,0,80,20,random(1,5)) is the (currently
      commented) original contract at character.cpp:2260. (§12, §13.2)
- [ ] DO NOT reconstruct: the bif big-splat flash (commented render :1485-1529), the
      bigs/meds trail loops (commented :1242-1283), the size-2/big draw (commented),
      and the TBloodParticle3DAnimator / commented "blood" generic-particle
      registrations. (§6.4, §2)
```

**Definition of done:** a damaging melee hit produces a one-shot burst of deep-red
Blood.I3D droplets squirting *directionally* from the wound (away from the victim's
facing for hits; straight up for decapitation), each flying on a gravity arc with
horizontal air-drag, growing slightly in flight, slapping onto the ground at local
z=20 where it snaps to a held splat (scale 1.8 for 25 ticks) then shrinks to
nothing — every droplet drawn in two passes (translucent alpha + additive glint),
ground-tipped while airborne, scene-lit so it dims in shadow, with droplet count,
trail density and splat persistence all scaling with ViolenceLevel, and the whole
effect self-killing when the last droplet dies.
