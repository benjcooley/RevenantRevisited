# I21 TIceBoltEffect — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | I21 |
| **Class(es)** | `TIceBoltEffect` (object) + `TIceBoltAnimator` (visual). Couples to `TIcedEffect`/`TIcedAnimator` (sub-effect, I22). |
| **Status** | forensics-complete (see §13 for the few genuine unknowns) |
| **Author / Date** | vfx-forensics-agent / 2026-05-19 |
| **Family** | ice |
| **Draws** | I3D mesh — multiple sub-objects of `Magic\icebolt.I3D` (stretched cylinders + spirals + rings + glow spheres + frost/snow billboard-meshes). Composite. |
| **Archetype(s)** | (B) I3D-geometry-with-transforms, (D) beam (two-endpoint mesh stretch), (E) particle emitters (frost + snow), (F) custom procedural logic + state machine, (F-composite). Posts the **(I22) Iced sub-effect**. NO associated dynamic light in the pre-release source (§9). |

---

## 1. Summary

Ice Bolt is the player's mid/high-tier frost attack spell. Despite the name and
the catalog/INVENTORY classification, **in the available source it is NOT a
flying projectile** — it is a *stationary freeze-beam* that snaps into existence
along the caster's facing direction, from the caster out to the first character
or wall it reaches, and plays a ~5 s grow/sustain/shrink animation in place. The
beam is drawn as I3D cylinder meshes stretched to the strike length, wrapped with
counter-rotating spiral meshes and rings sliding along it, capped with a glow
sphere at each end, surrounded by a continuous spray of ice "frost" particles,
and (at the highest spell tier) a swirling snow flurry. It deals `DAMAGE_ICE`
along the beam at one timing beat and, one beat later, posts the **Iced** status
(I22) on any enemy in range — freezing/paralysing them under a shatter overlay.
Its color identity (ice-blue/white) comes from the **authored textures of
`icebolt.I3D`**, drawn self-lit under Alpha blend; the spell's `LIGHT` line in
`spell.def` is a *retail-only* tag the pre-release parser discards (§9, §10).

---

## 2. Sources & evidence

- **Retail decomp:** `recon/mappings/TIceBoltEffect_cls_0x5aaf28_candidate.yaml`
  — **SPARSE** (MEDIUM-confidence class identity only; `fields:`/`methods:`
  empty). It confirms two facts: (a) `"IceBolt"` string XREFs land in
  `cls_0x5aaf28`, and (b) **`cls_0x5aaf28` inherits from `cls_0x5b0074`
  (the TEffect candidate) — i.e. retail IceBolt is a `TEffect`, NOT a
  `TMissileEffect`.** No per-method behavior recovered.
  `recon/mappings/TIcedEffect_cls_0x5aaf28_note.yaml` adds that the `"Iced"`
  status string is posted from `cls_0x5aaf28` (IceBolt) and `cls_0x5ab460`
  (TStormAnimator), with no standalone retail `TIcedEffect` class.
- **Pre-release:** `src/effect_old.cpp:7968-8713` — authoritative.
  - `TIceBoltEffect` (object): `:7984-7998` (`DEFINE_BUILDER("IceBolt", …)`
    `:7984`; `Initialize` `:7987-7992` empty; `Pulse` `:7994-7998` → `TEffect::Pulse()`).
  - `TIceBoltAnimator` (visual): `Initialize` `:8015-8135`, `Animate`
    `:8144-8455`, `Render` `:8464-8688`, `RefreshZBuffer` `:8690-8713`.
  - Constants `:7972-7982`; `FROST_HEIGHT` `:153`.
  - **Iced sub-effect** (I22): `TIcedEffect`/`TIcedAnimator`,
    `DEFINE_BUILDER("Iced", …)` `:8723`; `InitIced` `:8758-8768`; animator
    `Initialize`/`Animate`/`Render` `:8749-8900+`.
- **Existing port shell:** none in `src/effect.{h,cpp}` (only the class
  *declarations* `src/effect.h:1464-1547` for `TIceBoltEffect`/`TIceBoltAnimator`
  and `:1555-1593` for `TIcedEffect`/`TIcedAnimator`). INVENTORY I21 is
  `not-started`.
- **Sister effects consulted:**
  - `TStripEffect`/`TLightningAnimator` (`src/stripeffect.cpp:560-639`) — the
    **length-finder** loop in IceBolt's `Animate` (`:8152-8233`) is a near-verbatim
    copy of lightning's "march along facing until you hit a character/ground"
    code; used to confirm `MAX_MAXPOINTS`, the per-step distance, and the
    `flag`/`zheight` termination.
  - `TMissileEffect` (`src/missileeffect.{h,cpp}`) — the family base IceBolt was
    *planned* to use but does **not** (see §6.0 / §13). Documented for contrast.
  - `TFlareAnimator::Initialize` (`src/effect_old.cpp:517-549`) — the canonical
    material-zeroing self-lit pattern, which IceBolt does **not** do (§7 lit-mode).
- **Source-of-truth ranking:** pre-release `effect_old.cpp` is authoritative for
  *all* behavior/constants (retail is ctor-identity-only). Retail confirms the
  **base class is `TEffect`** and the **Iced-status posting**, both of which the
  pre-release source also shows — no behavior divergence found, only the
  catalog/INVENTORY mislabel (TMissileEffect → actually TEffect; see §13).

---

## 3. Constants

All from the gated pre-release body unless noted. "wu" = world units; the
animator is ungated in the original (runs at render rate) — per NOMENCLATURE §6
the port must gate to 24 Hz so timing/cadence don't scale with framerate.

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| `GROW_DURATION` | 10 | frames (timeline beat) | effect_old.cpp:7972 | yes — the master clock; every phase boundary is a multiple of it |
| `SPHERE_STEP` | 0.5 | scale/frame | effect_old.cpp:7973 | yes |
| `RING_SPEED` | 20 | wu/frame | effect_old.cpp:7974 | yes |
| `SPIRAL_STEP` | 1.0 | rad/frame (spiral spin) | effect_old.cpp:7975 | yes |
| `SPIRAL_SCALE_STEP` | 0.1 | scale/frame | effect_old.cpp:7976 | yes |
| `FROST_INIT_SIZE` | 0.5 | scale | effect_old.cpp:7977 | yes |
| `FROST_STEP` | 0.03 | scale/frame (shrink) | effect_old.cpp:7978 | yes |
| `FROST_DAMAGE_MIN` | 10 | hp | effect_old.cpp:7979 | yes — defined but **unused** (damage comes from spell variant, see §6) |
| `FROST_DAMAGE_MAX` | 25 | hp | effect_old.cpp:7980 | yes — defined but **unused** |
| `SW_ROTSPEED` | 0.5 | rad/frame | effect_old.cpp:7981 | yes — only used in commented-out shockwave code (§13) |
| `SW_SCALESPEED` | 0.2 | scale/frame | effect_old.cpp:7982 | yes — commented-out only |
| `FROST_HEIGHT` | 70 | wu | effect_old.cpp:153 | yes — z-drop used in the strike-test sample point |
| `MAX_FROST_PARTICLES` | 50 | count | effect.h:1513 | yes |
| `MAX_SNOW_PARTICLES` | 50 | count | effect.h:1514 | yes |
| `MAX_MAXPOINTS` | 100 | count (max march steps) | legacy/StripEffect.H:87 | yes — **not** defined in `src/`; resolves via the legacy header. Caps `length` at 100 steps. |
| length step | 10 | wu/step | effect_old.cpp:8170-8171,8232 | yes — `point += (dx,dy)` of magnitude `r=10`; `length = ticks*10` so `length ≤ 1000 wu` |
| march radius `r` | 10 | wu | effect_old.cpp:8154 | yes — per-step march distance along facing |
| strike hit radius | 10 | wu | effect_old.cpp:8209 | yes — character within 10 wu of sample stops the march |
| Initialize z-lift | +50 | wu | effect_old.cpp:8023 | yes — bolt origin raised 50 wu above caster pos at spawn |
| `SetSubSpell(3)` | 3 | — | effect_old.cpp:8049 | yes — **animator force-sets subspell=3** (max tier) at Initialize, overriding whatever the cast set; gates which visuals/timings run (see §6) |
| frost init pos jitter | ±10 | wu | effect_old.cpp:8037-8039 | yes |
| frost init vel jitter | ±3 | wu/frame | effect_old.cpp:8040-8042 | yes |
| frost init delay `t[i]` | random(0, FROST_INIT_SIZE*2/FROST_STEP)=random(0,33) | frames | effect_old.cpp:8044 | yes — staggers spawn |
| frost respawn delay | random(0, FROST_INIT_SIZE/FROST_STEP)=random(0,16) | frames | effect_old.cpp:8351 | yes (becomes hard 100 after `GROW_DURATION*3`, `:8352-8353`) |
| snow `h[i]` (z) | random(-50,50) | wu | effect_old.cpp:8123 | yes |
| snow `th[i]` (angle) | random(0,359)·TORADIAN | rad | effect_old.cpp:8124 | yes |
| snow `rs[i]` (ang vel) | random(5,15)/100 = 0.05..0.15 | rad/frame | effect_old.cpp:8125 | yes; accelerates `+0.01`/frame (`:8243`), `+0.05`/frame after beat 5 (`:8270`) |
| snow `sz[i]` (scale) | random(FROST_STEP·100, FROST_STEP·GROW_DURATION·100)/100 = random(3,30)/100 = 0.03..0.30 | scale | effect_old.cpp:8126 | yes |
| snow `r[i]` (radius) | random(25,50) | wu | effect_old.cpp:8127 | yes; shrinks `-1`/frame after beat 5 (`:8267`), floored at 1 |
| snow rise after beat 5 | `h[i] += 5` | wu/frame | effect_old.cpp:8266 | yes |
| `TORADIAN` | π/180 | rad/deg | revdefs.h:25 | yes |
| damage range radius | 200 | wu | effect_old.cpp:8400,8425 | yes — `DamageCharactersInRange`/`FindObjectsInRange` |
| damage type | `DAMAGE_ICE` (13) | enum | effect_old.cpp:8400; object.h:663 | yes — **hardcoded** in animator, overriding `spell->SpellData()->damagetype` (`:8399` commented out). Note spell.def says `DT_FREEZE`(8); the live call forces `DAMAGE_ICE`(13). See §13. |
| damage min/max | `spell->VariantData()->mindamage/maxdamage` | hp | effect_old.cpp:8400 | yes — from spell.def VARIANT (e.g. 350/392 for Ice Bolt, 120/142 for Priest variant; spell.def:1228,1230) |
| lifetime / kill | `GROW_DURATION*12` = 120 frames | frames | effect_old.cpp:8451-8454 | yes — at 24 Hz ≈ **5.0 s** |

### Iced sub-effect (I22) constants — for the coupling note (§6)

| name | value | units | source |
|------|-------|-------|--------|
| `ICED_LENGTH` | 10 | s | effect_old.cpp:8719 |
| `ICED_DURATION` | 24·10 = 240 | frames (≈10 s) | effect_old.cpp:8720 |
| `ICED_CHUNK_GRAVITY` | 0.25 | wu/frame² | effect_old.cpp:8721 |
| `MAX_ICED_CHUNKS` | 30 | count | effect.h:1573 |

---

## 4. Assets

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| icebolt mesh | `legacy/Imagery/Magic/icebolt.I3D` | 204 428 B | the entire bolt: cylinders, spiral, rings, glow sphere, particle billboards | Registry name `"IceBolt"`, `Class.Def:2055` (`0xb1c4c90f`); resolved via the EFFECT object class → `T3DImagery`; the animator addresses sub-objects with `GetObject(idx)` |
| iced overlay mesh | `legacy/Imagery/Magic/iced.i3d` | 94 888 B | I22 frozen-character crystal + shatter chunks (sub-effect) | Registry name `"Iced"`, `Class.Def:2056` (`0xfacecabb`). Also aliased `"Icedsparks"` (`:2057`) and `"Snow"` (`:2061`) — same file. |

**`icebolt.I3D` structure (verified by string-dump of the file):**

- **States:** `launch`, `fly`, `explode` (animation states; the pre-release
  animator does not switch state — it drives everything procedurally via
  `frameon`, so the states are vestigial here).
- **Sub-objects (file order = `GetObject` index):**

  | index | name | role in `TIceBoltAnimator::Render` |
  |-------|------|-------------------------------------|
  | 0 | `cylinder04` | one of 4 nested core beam cylinders (loop `i=0..3`, `:8475-8504`) |
  | 1 | `cylinder01` | core beam cylinder |
  | 2 | `cylinder02` | core beam cylinder |
  | 3 | `cylinder03` | core beam cylinder |
  | 4 | `box01` | **the glow sphere / billboard quad** — reused for end-cap glow spheres (`:8573-8600`), frost particles (`:8602-8628`), and snow flurry (`:8630-8657`). `obj = GetObject(4)` then re-transformed per draw. |
  | 5 | `cylinder05` | the **rings** sliding along the beam (`:8547-8570`) |
  | 6 | `cylinder06` | the counter-rotating **spirals** wrapped around the beam (`:8512-8544`) |

  (`GetObject(7)` is referenced only in a commented-out shockwave block,
  `:8658-8683`; the mesh has no 8th sub-object.)

- **Textures:** stored **inline** in the `.I3D` (no external `.bmp`/`.tga`
  filenames in the string dump). The ice-blue/white coloration is baked into
  these embedded textures + the per-sub-object materials. Transparency follows
  the engine's black-key convention (knowledge 02 §4): black background → keyed
  transparent.

**Stand-in guard:** This effect is **entirely asset-driven** — every visible
quad is a transformed sub-object of `icebolt.I3D` drawn via `RenderObject`. There
is NO procedural geometry. A reconstruction that draws billboards/gradients
instead of loading and transforming the seven `icebolt.I3D` sub-objects is a
stand-in and is wrong (AGENT_GUIDE §4.2.1). The "frost", "snow", and "glow
sphere" particles are **the same `box01` sub-mesh** re-instanced with different
transforms — not separate textures.

---

## 5. Spawn & emit

- **Trigger semantics:** **fixed-duration** — spawns on cast, lives
  `GROW_DURATION*12 = 120` frames (~5 s at 24 Hz), then `KillThisEffect()`
  (`:8451-8454`). Not a projectile (no flight), not looping.
- **Count per trigger:** one beam composite per cast. Within it, per frame:
  4 core cylinders + (`length/100`)·2 spiral segments + (`length/100`) rings +
  2 end glow spheres + up to 50 frost billboards + (tier-3 only) up to 50 snow
  billboards.
- **Initial direction / distribution:** the beam extends along the **caster's
  byte-facing** resolved at spawn. `angle = ((PTEffect)inst)->GetAngle()`
  (`:8047`) — `TEffect::GetAngle` resolves invoker→target facing
  (`effect_old.cpp:434-455`). The beam *length* is found by marching from the
  origin along `(dx,dy) = (10·sin θ, −10·cos θ)`, `θ = GetFace()·360/256·TORADIAN`
  (`:8154-8156`), stepping 10 wu at a time until it (a) reaches a non-friend live
  character within 10 wu, or (b) drops below ground walk-height, or (c) hits
  `MAX_MAXPOINTS=100` steps. `length = ticks·10` (`:8168-8232`), so **0 < length
  ≤ 1000 wu**. This march runs once, on `frameon == 0` (`:8152`).
- **Emit anchor convention:** the effect ORIGINATES at the **caster's cast
  position raised +50 wu in z** — set in `Initialize` (`:8022-8026`:
  `GetPos; z += 50; SetPos`). The spell system already places `def.pos` at the
  caster pos + source-pos (hand) + `HEIGHT=70` fallback (spell.cpp:442-447;
  spell.def variant `HEIGHT=70`). So the bolt's anchor is roughly caster-hand
  height. All beam geometry is built in the animator's **local space** (origin =
  this anchor) and the beam runs out along **−y in local space** (see §7 — the
  glow/particles/rings translate by `−length`), oriented to world by the
  per-quad facing rotation.
- **Coordinate space:** **Local** — every `obj->matrix` is built fresh from
  identity each draw (`D3DMATRIXClear`), so geometry is authored relative to the
  effect's own object position; the engine places the object in world space.
- **Spread / jitter:** frost particles seed at ±10 wu position / ±3 wu/frame
  velocity (`:8037-8042`); snow seeds on a ring of radius 25–50 wu at random
  angle, z ∈ ±50 (`:8123-8127`).

### Spatial diagram (beam + frost cloud, viewed in the beam's local frame)

```
   local +z (up)
   │            glow sphere (box01, end cap, on caster end)
   │         ◓ ────────────────────────────────────────── ◓  glow sphere (target end)
   │        ╱ ║ core: 4 nested cylinders (cyl01..04), stretched scl.z = length/64
   │  · · ·   ║≈≈≈≈ spirals (cyl06) counter-rotate around beam, stepped every 100 wu
   │ ·  ·  ·  ○  ○  ○   rings (cyl05) slide along −y at RING_SPEED, spaced 100 wu
   │   · · ·  (frost billboards: 50× box01, spray ±10 wu around the TARGET end)
   └───────────────────────────────► local −y  (beam runs from origin out to −length)
        origin = caster pos, z+50      target end at y = −length
```

The beam is built along local **−y**; the iso/world placement comes from the
object's facing. `RefreshZBuffer` projects origin→origin+`ConvertToVector(angle,
length)` to size the depth-restore patch (`:8690-8713`), confirming the beam runs
from the anchor along `angle` for `length` wu in world space.

---

## 6. Behavior & per-frame logic

### 6.0 Base-class note (it is NOT a missile)

`TIceBoltEffect : public TEffect` (effect.h:1464) — **not** `TMissileEffect`,
contrary to the catalog/INVENTORY. `TIceBoltEffect::Initialize` is empty (the
`TMissileEffect::Initialize()` / `SetSpeed(...)` calls are **commented out**,
`:7989-7991`) and `TIceBoltEffect::Pulse` calls `TEffect::Pulse()` (the
`TMissileEffect::Pulse()` call is commented out, `:7996`). Retail confirms the
TEffect base (§2). **Consequence:** none of the missile LAUNCH/FLY/EXPLODE
flight, `Move()`-driven travel, range-decay, or impact-spawn machinery
(`missileeffect.cpp:31-90+`) runs. IceBolt is a **stationary, instantly-cast
beam**, not a flying bolt with a trail. Document the missile base only as the
*path not taken* (§13).

### 6.1 The state machine (driven by `frameon`, master beat `GROW_DURATION=10`)

`frameon` increments every `Animate` (`:8234`). All transitions key off
multiples of `GROW_DURATION`:

```
phase / beat (frames):   subspell forced to 3 at Initialize (:8049), so ALL branches run
 0          : length-march (once); frost+ seed in Initialize
 1..9   (<GD)        : GROW   — spheres+cyl+spiral grow; spiral spins
 10     (==GD)       : DAMAGE — DamageCharactersInRange(... DAMAGE_ICE ...) along beam
 10..19 (GD..2GD)    : cyl shrinks (half-rate); spheres still grow; spiral spins
 20     (==2GD)      : FREEZE — find enemies in range, spawn "Iced" sub-effect on each
 20..29 (2GD..3GD)   : cyl+spheres shrink; spiral spins; snow grows toward beam length (cy)
 30..39 (3GD..4GD)   : spheres+spiral shrink; frost particles given long life (t=100); rings/spirals STOP (frameon<3GD gate, :8507)
 50     (==5GD)      : snow flurry "blows up" — particles rise (+5/f), tighten (r−1/f), spin faster
 120    (==12GD)     : KillThisEffect()   (~5 s)
```

(Several `frameon == GROW_DURATION*5` and `*4` blocks for an extra damage burst
and particle re-targeting are **commented out**, `:8274-8330`, `:8402-8408`; see
§13.)

### 6.2 Length march (once, `frameon == 0`) — cite `:8152-8233`

```
θ = GetFace()*360/256 * TORADIAN            // byte facing → radians  (:8155-8156)
(dx,dy) = (10*sin θ, -10*cos θ)             // 10 wu step along facing
point = caster_pos (with z+50 from Initialize)
ticks = 0; flag = false
do:
    point.x += dx; point.y += dy            // step out 10 wu      (:8170-8171)
    zheight = MapPane.GetWalkHeight(point)  // floor under sample  (:8177)
    sample = (point.x, point.y, point.z - FROST_HEIGHT)   // 70 wu below (:8181-8183)
    for each live, non-friend CHARACTER within 10 wu of sample:  flag = true; break  (:8198-8217)
    ticks++
while (point.z > zheight && !flag && ticks < MAX_MAXPOINTS)   // (:8231)
length = ticks * 10                          // beam length in wu  (:8232)
```

This is the lightning length-finder (`stripeffect.cpp:560-628`) ported verbatim
(per-step 16 wu and z−100 in lightning; here 10 wu and z−`FROST_HEIGHT`=70).

### 6.3 Frost particles (continuous, every frame) — cite `:8331-8355`, render `:8602-8628`

```
for i in 0..MAX_FROST_PARTICLES:            // 50 particles
    t[i]--                                  // count down spawn delay
    if t[i] > 0: continue                   // not yet active
    t[i] = 0
    p[i] += v[i]                            // drift (vel set ±3 at seed)
    s[i] -= FROST_STEP                      // shrink 0.03/frame
    if s[i] < 0:                            // respawn
        p[i] = random(±10); v[i] = random(±3)
        s[i] = FROST_INIT_SIZE (0.5)
        t[i] = random(0,16)                 // (or hard 100 after beat 3, :8352)
```

Rendered only when `t[i] <= 0` (active), each as a `box01` quad scaled `s[i]`,
positioned at `p[i]` but translated by `−length` in y (i.e. **clustered at the
TARGET end** of the beam, `:8623`), oriented screen-ward (§7).

### 6.4 Snow flurry (tier subspell>2, i.e. always since subspell forced to 3) — cite `:8236-8273`, render `:8630-8657`

```
each frame, for i in 0..MAX_SNOW_PARTICLES (50):
    th[i] += rs[i]; wrap to [0,2π]          // orbit angle advances
    rs[i] += 0.01                           // angular velocity accelerates
if frameon > 2*GD: cy += length/(2*GD); clamp cy to length   // sweep toward beam length
if frameon > 5*GD:                          // "blow up"
    for each snow particle: h[i] += 5 (rise); r[i] -= 1 (tighten, min 1); rs[i] += 0.05 (spin faster)
```

Snow particle position (render): polar around the beam axis —
`pos = (r[i]·cos th[i], r[i]·sin th[i] − cy_if_past_2GD, h[i])` (`:8649-8653`),
each a `box01` quad scaled `sz[i]`. Visually: a swirling snow cloud that tightens
and lifts up the beam over time.

### 6.5 Beam-body transform animation — cite `:8356-8392`

```
if frameon < GD:           spherescale[0]+=0.5; spherescale[1]+=1.0; cylscale+=0.5; spiralscale+=0.1; spiralang+=1.0 (wrap 2π)
if GD <= frameon < 2GD:    cylscale-=0.25; spherescale[0]+=0.5; spherescale[1]+=0.5; spiralang+=1.0
if 2GD <= frameon < 3GD:   cylscale-=0.25; spherescale[0]-=0.5; spherescale[1]-=0.5; spiralang+=1.0
if 3GD <= frameon < 4GD:   spherescale[0]-=0.5; spherescale[1]-=1.0; spiralscale-=0.1
ringout += RING_SPEED (20); if ringout > length: ringout -= length    // rings slide, wrap (:8390-8392)
```

So the core cylinder thickens then thins; the two glow spheres pulse asymmetric
(index 0 vs 1) on a triangle-ish envelope; the spiral continuously spins at
`SPIRAL_STEP=1.0 rad/frame`; rings translate along the beam at 20 wu/frame and
wrap by `length`.

### 6.6 Damage & freeze beats (gameplay)

- **DAMAGE @ `frameon == GROW_DURATION` (beat 10)** — `:8393-8401`:
  `effect_pos = origin; effect_pos.y -= length;` (target end), then
  `DamageCharactersInRange(caster, effect_pos, 200, mindamage, maxdamage,
  DAMAGE_ICE)`. Min/max from the spell variant; type **hardcoded `DAMAGE_ICE`**
  (the `spell->SpellData()->damagetype` form is commented out, `:8399`).
- **FREEZE @ `frameon == 2*GROW_DURATION` (beat 20), subspell>1** — `:8409-8450`:
  compute a strike point `origin + ConvertToVector(angle, length)`, find
  CHARACTERs within 200 wu (`FindObjectsInRange`), and for each live non-caster:
  spawn an `"Iced"` effect at the victim's position, create its animator, and call
  `ia->InitIced(victim)` — see §6.7. This is the **sub-effect coupling**.

### 6.7 Sub-effect: posting "Iced" (I22) — coupling, cite `:8431-8448` + `:8758-8768`

```
SObjectDef def{}; def.objclass = OBJCLASS_EFFECT; def.level = MapPane.GetMapLevel();
def.pos = victim->GetPos();
def.objtype = EffectClass.FindObjType("Iced");
PTIcedEffect e = MapPane.GetInstance(MapPane.NewObject(&def));    // spawn child
if (!e->HasAnimator()) e->CreateAnimator();
((PTIcedAnimator)e->GetAnimator())->InitIced(victim);            // wire victim
```

`TIcedAnimator::InitIced(victim)` (`:8758-8768`) sets the victim
`SetParalize(true)` + `SetIced(true)` (freeze). The Iced effect then runs its own
~10 s timer (`ICED_DURATION=240`), holding the victim frozen
(`Stop()` each frame, `:8788`), and on expiry (or victim death/un-ice) it spawns
`MAX_ICED_CHUNKS=30` shatter chunks that bounce-and-die with
`ICED_CHUNK_GRAVITY=0.25` (`:8807-8890`). **This is a separate effect (I22)** —
not fully forensiced here; documented as the coupling. Full body
`effect_old.cpp:8723-8900+`.

### Temporal diagram (cylscale + spheres, value vs frame)

```
scale
  ▲                  spheres (idx1) keep growing to ~2GD then fall
  │      ╱╲___                      cyl thickens to GD, thins after
  │    ╱      ╲___
  │  ╱            ╲________
  └──┼────┼────┼────┼────┼────┼──────────────────► frameon
     0   10   20   30   40   50 ............... 120 (kill)
     │  DAMAGE FREEZE     snow "blow up"@50      │
     └─grow─┘                                    KillThisEffect
```

---

## 7. Rendering (original render state + geometry)

- **What it draws:** an **I3D composite** — all seven `icebolt.I3D` sub-objects
  re-transformed and submitted via `RenderObject(obj)` inside one `Render`
  (`:8464-8688`). Each draw clears `obj->matrix`, sets `obj->flags = OBJ3D_MATRIX`
  (full matrix override), builds Scale·RotateX·RotateY·RotateZ·Translate, then
  `ResetExtents()/RenderObject/UpdateExtents()`.
- **Blend mode (original):** **Alpha (modulated)** — `Render` calls
  `SaveBlendState(); SetBlendState(); … RestoreBlendState();` (`:8466-8467,8685`).
  `SetBlendState()` = `D3DTBLEND_MODULATE`, `SRCBLEND=SRC_ALPHA`,
  `DESTBLEND=INV_SRC_ALPHA`, depth-write off, depth-test on
  (effect_old.cpp:221-233; knowledge 03 §1.1). **NOT additive** — IceBolt uses
  the plain Alpha default, like ripple/drip/symglow, NOT `SetAddBlendState()`.
- **Lit vs self-lit:** **Unlit / self-lit** in effect, but **without material
  zeroing**. Unlike `TFlareAnimator` (which zeroes the material at Initialize),
  `TIceBoltAnimator::Initialize` does **not** touch the imagery material
  (`:8015-8135` has no `GetMaterial`/`SetMaterial`). So the visible color comes
  from the **mesh's authored materials × textures** modulated by the default
  per-vertex diffuse (white, since the animator never writes `lverts[].color`).
  No `GetAmbientLight()` mix → not scene-lit. Classify **Unlit**: the ice color
  is literal from the asset.
- **Depth / Z:** depth-test on, **depth-write off** (`SetBlendState`,
  effect_old.cpp:224-225) → **TestNoWrite**. `RefreshZBuffer` (`:8690-8713`)
  restores scene Z under the beam's screen footprint: project origin and
  origin+`ConvertToVector(angle,length)`, take the bounding box ±100 px, call
  `RestoreZ(x1,y1,w,h)`.
- **Orientation:**
  - **Beam meshes (cylinders/spirals/rings):** tipped onto the beam axis with
    `D3DMATRIXRotateX(+π/2)` (`:8488,8530,8558`) — lays the unit cylinder along
    the local **−y** beam axis (the WorldXY `rot.x` tell, but here used to align
    the *mesh long-axis* to the beam, with the object then world-placed by facing).
    Spirals add `RotateY(spiralang)` (one mirror at `π − spiralang`, `:8531`) for
    the counter-rotating double helix.
  - **Glow spheres / frost / snow billboards (`box01`):** oriented toward the
    screen by a fixed tilt + facing spin:
    `RotateX(−π/2); RotateX(−π/6); RotateZ(−π/4); RotateZ(−facing)`
    (`:8587-8590, :8615-8618, :8642-8645`). This is the **ScreenAligned-ish**
    camera-facing convention for the particle quads. ⚠ The facing term is written
    `−((inst->GetFace(f*360)/256)*TORADIAN)` with an **undeclared `f`** and a
    `GetFace(int)` overload that does not exist (object.h:976 `GetFace()` takes no
    args) — this is **non-compiling leftover** in the gated body (§13). The
    intended value is the well-formed `GetFace()*360/256*TORADIAN` used in
    `Animate` (`:8155`).
- **Per-quad / per-object transform:** see §6.5/§6 — Scale from `cylscale`,
  `spherescale[]`, `spiralscale`, `s[i]`, `sz[i]`; the **beam length lives in a
  single scale factor** `scl.z = length/64` on the unit cylinder (`:8485`); rings
  translate by `−(ringout + 100·i) mod length` (`:8563`); spheres by `−length·i`
  (one per end, `:8595`); frost by `p[i]` offset `−length` in y (`:8623`).
- **Per-vertex color packing:** **none written by the animator** — it never sets
  `lverts[].color`/`.diffuse`. The diffuse stays at the mesh default (white), so
  under MODULATE the texture/material color passes through unchanged. (Contrast
  fire/vortex which pack a `D3DRGBA` brightness ramp per vertex.)

---

## 8. Texture animation

- **Mechanism:** **none** — no UV scroll, no `framehtexs` flipbook, no
  `SetTextureFrame`. The animator never mutates `.tu`/`.tv` and never calls
  `ScrollTexture`/`SetTextureFrame` (searched the full `Animate`+`Render` bodies
  `:8144-8688`). All apparent motion is **mesh transform animation** (scale, the
  spiral's `RotateY`, rings sliding, particles drifting), not texture animation.
- The texture is therefore **static** per draw; the streaming-beam look comes
  from the rings (`cylinder05`) translating along the beam and the spiral
  (`cylinder06`) rotating — geometric, not UV. If a reconstruction wants the
  classic "energy flowing along the beam" it must reproduce the **ring slide**
  (`ringout += RING_SPEED`) and **spiral spin** (`spiralang += SPIRAL_STEP`), not
  add UV scroll.

---

## 9. Associated light

- **NO dynamic light in the pre-release source.** The animator emits no light:
  there is no `AddLight`/`CreateLight`/`SLightInfo` call anywhere in
  `TIceBoltAnimator` (`:8015-8713`).
- **There IS a `LIGHT` line in spell.def** — `LIGHT COLOR 100,100,255 INT 180
  MULT 20` (spell.def:1223, an ice-blue point light) — **but the pre-release
  spell parser discards it**: `TSpellData::Load` only handles known tags
  (`DAMAGETYPE`, `VARIANT`, …) and routes `LIGHT` into the "skipping unknown tag"
  branch (`spell.cpp:119-157`, comment names `LIGHT` explicitly at `:123`). So in
  the available engine, **icebolt produces no light**, and `SSpellData` has no
  light field to read.
- **Reconstruction implication:** an ice-blue dynamic light is clearly the
  *intended* (retail) behavior — color `(100,100,255)`, intensity 180, mult 20,
  pinned to the beam — and the reconstruction agent may choose to honor the
  spell.def `LIGHT` tag. But it is **not present in the pre-release code path**;
  flag it as an intended-but-unimplemented retail feature (§13), not a
  pre-release fact.

---

## 10. Color

- **Source:** the **authored textures + materials of `icebolt.I3D`** (the bolt's
  ice-blue/white look is baked into the embedded textures, §4). The animator
  writes no per-vertex color and zeroes no material, so the color is literally
  the asset under MODULATE/Alpha (§7). There is **no** chardata/spell-color/tint
  path feeding this effect's mesh color in the pre-release source.
- **Exact values:** not a code constant — it lives in the `.I3D` texture data.
  The *spell's* intended light color (separate from the mesh) is RGB
  `(100,100,255)` 0–255 (ice-blue, spell.def:1223) — useful as the target hue if
  a reconstruction adds the light (§9).
- **Expected visual:** cold **ice-blue / cyan-white**, fully saturated in the
  blue channel — the magic-family "cool" signature (AGENT_GUIDE §4.2.1.5). If the
  reconstruction renders pale, gray, or warm, suspect: (1) the `icebolt.I3D`
  textures weren't loaded (procedural stand-in — the #1 failure mode here), (2)
  black-key chroma miss eating the blue edges, or (3) wrong blend (additive
  washes the asset).
- **Normalization / boosts:** none — no `NormalizeColors`/brightness math in
  this effect.

---

## 11. Audio coupling

- **No audio coupling found in `TIceBoltAnimator`** — no `PLAY(...)`,
  `SoundPlayer`, or imagery-tag sound call in `Initialize`/`Animate`/`Render`
  (`:8015-8713`). (Contrast `TPhotonEffect`/`TFlameDiscEffect` which `Unmount`
  `LIGHTNING_SOUND` in their dtors; IceBolt's dtor is empty, effect.h:1470/1537.)
- The `icebolt.I3D` may carry per-(state,frame) `S3DTag` sounds (the
  data-driven mechanism, knowledge 02 §8), but the pre-release animator never
  switches imagery state, so any such tags would not fire. Record as
  "no code-driven audio"; the audio phase should check the `.I3D` tags directly.

---

## 12. Triggers & in-game appearance

- **Spawned by:** the **Ice Bolt** spell. `spell.def:1215-1231` —
  `VARIANT "Ice Bolt", TP_BASIC, "CIB", "icebolt", mana 228, …, HEIGHT 70,
  FACING TRUE, …` (`:1228`) and `VARIANT "Priest Ice Bolt", …, "MAK", "icebolt",
  …` (`:1230`). The 4th field `"icebolt"` is the builder name →
  `DEFINE_BUILDER("IceBolt", TIceBoltEffect)` (effect_old.cpp:7984) +
  `REGISTER_3DANIMATOR("IceBolt", TIceBoltAnimator)` (`:8004`).
- **Spell-cast pipeline:** `TSpellManager::CastByName` → `TSpell::Timer` builds
  the `SObjectDef` at caster pos + hand source + `HEIGHT=70`, `facing` from
  invoker, `objtype = FindObjType("icebolt")` (spell.cpp:431-457), then
  `SetSpell(this)` so the animator can read invoker/target/damage.
- **Where to see it in the original game:** cast Ice Bolt (talisman combo `CIB`,
  or the Priest variant `MAK`) at an enemy with a clear line of sight. The
  beam fires along the caster's facing to the first target/wall; on a hit it
  freezes the enemy under the Iced overlay. **Capture rig:** projectile/spell-cast
  category — caster character facing a target dummy ~300–800 wu away on a flat
  floor (Dungeon BG per AGENT_GUIDE §4.2.1.7 for cool effect on cool stone; also
  Black to confirm the asset reads). The beam is **stationary** (no travel), so a
  per-frame *sequence* over the 120-frame life (capture beats 0/10/20/30/50/119)
  is needed to show grow → damage → freeze → snow-blowup → kill.
- **Vestigial?** No — live spell with two playable variants.

---

## 13. Gaps & uncertainties

1. **Base-class mislabel (resolved here).** Catalog `04_EFFECT_CATALOG.md:110`,
   INVENTORY I21, and the task brief all say base = `TMissileEffect`. **Source
   (both pre-release `effect.h:1464` and retail `cls_0x5aaf28` inheriting
   `cls_0x5b0074`) says `TEffect`.** The missile-base code is commented out
   (`:7989-7991, :7996`). IceBolt is a stationary beam, NOT a flying projectile —
   there is no trail ring-buffer, no flight, no impact-spawn. INVENTORY/catalog
   should be corrected. *(This is the single biggest source surprise.)*
2. **`subspell` forced to 3.** `Initialize` calls `SetSubSpell(3)` (`:8049`)
   **before** reading it, so the tiering (`subspell > 0/1/2`) gates in
   `Animate`/`Render` are effectively all-on in the pre-release build. The
   tier-based gating logic exists but the cast-supplied subspell is overwritten.
   Whether retail honored a real per-cast subspell is **undetermined** (recon
   sparse). Reasonable reconstruction: treat subspell=3 (full visuals) to match
   the pre-release behavior, and note the gates for a future tiering pass.
3. **`GetFace(f*360)` is non-compiling leftover.** In `Render` (`:8590, 8618,
   8645`) the billboard facing uses `inst->GetFace(f*360)` with `f` undeclared
   and a `GetFace(int)` overload that doesn't exist (`object.h:976`). This gated
   `#if 0` body never built. The **intended** value is
   `GetFace()*360/256*TORADIAN` (the well-formed form in `Animate` `:8155`). Use
   that for the particle/sphere screen-facing spin.
4. **Damage type mismatch.** Animator hardcodes `DAMAGE_ICE`(13) (`:8400`);
   spell.def declares `DAMAGETYPE DT_FREEZE`(8) (`spell.def:1220,41`). The live
   code overrides with `DAMAGE_ICE`. Gameplay/VFX-irrelevant for the visual, but
   recorded for the gameplay port.
5. **Associated light is retail-only.** spell.def `LIGHT COLOR 100,100,255 INT
   180 MULT 20` (`:1223`) is parsed-and-discarded by the pre-release engine
   (§9). It is the *intended* ice-blue glow but absent from the available code
   path. Reconstruction may add it; flagged so it's a deliberate choice, not an
   assumed pre-release fact.
6. **Commented-out subsystems.** A particle-seeking "snow" sub-emitter
   (`:8052-8118`, `:8274-8330`) and an extra damage burst + a `GetObject(7)`
   shockwave (`:8402-8408`, `:8658-8683`) are commented out. They reference
   `SW_ROTSPEED`/`SW_SCALESPEED` and a non-existent 8th sub-object. **Do not
   reconstruct these** — they were dev-disabled. Recorded so the reconstruction
   agent recognizes them as dead code, not missing work.
7. **`icebolt.I3D` per-sub-object materials/state animation** — the file has
   `launch/fly/explode` states with ani-keys, but the pre-release animator never
   plays them (it transforms sub-objects manually). Whether retail used the I3D's
   own state animation instead is **undetermined**; the pre-release imperative
   transform is the documented path.

---

## 14. Reconstruction burndown

```
- [ ] Load Magic\icebolt.I3D (204428 B); address its 7 sub-objects by index:
      0-3 = cylinder04/01/02/03 (core beam), 4 = box01 (glow sphere/particle quad),
      5 = cylinder05 (rings), 6 = cylinder06 (spirals). NO procedural stand-ins. (§4)
- [ ] Spawn as fixed-duration TEffect (NOT a missile — no flight/trail). Anchor at
      caster pos +50 wu z (Initialize), facing from GetAngle(); lifetime
      GROW_DURATION*12 = 120 frames (~5 s @ 24 Hz), then KillThisEffect. (§5, §6.0, §6.1)
- [ ] Length march once at frameon==0: step 10 wu along facing (10·sinθ, −10·cosθ)
      until a non-friend live character within 10 wu of the (z−70) sample, or below
      walk-height, or 100 steps; length = ticks·10 (0..1000 wu). (§5, §6.2)
- [ ] Core beam: 4 nested cylinders, scl.x/y = cylscale/2, scl.z = length/64,
      RotateX(+π/2) to lie along the local −y beam axis. (§7)
- [ ] Spirals (cyl06): numrevs = length/100 segments, two mirrored helices
      (RotateY spiralang and π−spiralang), scl.z = (length/numrevs)/64, stepped
      down −y; spin spiralang += SPIRAL_STEP(1.0)/frame; only while frameon<3·GD. (§6.5, §7)
- [ ] Rings (cyl05): length/100 instances, scl.x/y = cylscale/2, slide along −y at
      RING_SPEED(20)/frame wrapping by length (ringout); only while frameon<3·GD. (§6.5, §7)
- [ ] End glow spheres: 2× box01, scl = spherescale[i]/2, one at y=0 and one at
      y=−length; screen-facing tilt RotateX(−π/2)·RotateX(−π/6)·RotateZ(−π/4)·RotateZ(−facing)
      [use facing = GetFace()*360/256*TORADIAN; ignore the broken GetFace(f*360)]. (§7, §13.3)
- [ ] Frost particles: 50× box01 at p[i] (offset −length in y, clustered at target end),
      scale s[i]; per-frame drift p+=v, shrink s−=FROST_STEP(0.03), respawn on s<0 with
      ±10 pos/±3 vel; stagger via t[i] delay. (§5, §6.3, §7)
- [ ] Snow flurry: 50× box01 orbiting the beam (r[i]·cosθ, r[i]·sinθ, h[i]),
      angle th[i]+=rs[i] (rs accelerates), scale sz[i]; after 2·GD sweep cy toward
      length; after 5·GD rise (+5), tighten (r−1), spin faster. (§6.4, §7)
- [ ] Transform-animation envelopes per phase (cylscale, spherescale[0/1], spiralscale,
      spiralang) keyed to GROW_DURATION beats — grow <GD, then phased shrink. (§6.5)
- [ ] Blend = Alpha (SetBlendState: MODULATE, SRC_ALPHA/INV_SRC_ALPHA),
      depth = TestNoWrite, lit-mode = Unlit (no material zeroing — asset color passes
      through; do NOT write per-vertex color). (§7)
- [ ] Texture animation = NONE (no UV scroll, no flipbook) — motion is geometric only. (§8)
- [ ] RefreshZBuffer: restore Z over the bbox of origin→origin+vector(angle,length) ±100px. (§7)
- [ ] Color: ice-blue/white from icebolt.I3D's embedded textures; verify saturated
      blue (pale/gray ⇒ stand-in / chroma-key / wrong blend). (§10)
- [ ] Damage beat @ frameon==GD: DamageCharactersInRange(caster, target-end, 200,
      spell mindamage/maxdamage, DAMAGE_ICE). (§6.6)
- [ ] Sub-effect: @ frameon==2·GD spawn "Iced" (I22) on each enemy in 200 wu +
      InitIced(victim) (paralyze+ice). Couples to I22 — see effect_old.cpp:8723-8900. (§6.7)
- [ ] Associated dynamic light: NONE in pre-release; OPTIONAL retail addition
      RGB(100,100,255) INT 180 MULT 20 from spell.def:1223 (flag if added). (§9, §13.5)
- [ ] Audio: none code-driven; check icebolt.I3D S3DTag sounds in the audio phase. (§11)
- [ ] Do NOT reconstruct the commented-out particle-seeker / extra-burst / GetObject(7)
      shockwave — dev-disabled dead code. (§13.6)
```
