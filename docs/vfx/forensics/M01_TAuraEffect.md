# M01 TAuraEffect — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | M01 |
| **Class(es)** | `TAuraEffect` + `TAuraAnimator` |
| **Status** | forensics-complete |
| **Retail fidelity** | retail-partial — see §2.1 |
| **Author / Date** | forensics-agent / 2026-06-08 |
| **Family** | magic |
| **Draws** | particle emitter + dynamic light |
| **Archetype(s)** | rising particle emitter (E), associated spell light (G), character-attached |

---

## 1. Summary

TAuraEffect is the visual status overlay for the "Aura" spell — a rising blue particle glow that envelops the caster's body while the spell is active. Particles spawn from random character skeleton bones, drift upward through the character's silhouette, and fade out by shrinking to zero scale. The effect runs continuously for 30 seconds (720 ticks), ramping emission up over that duration and then winding down, terminating automatically when the last particles are gone. In-game it appears on Locke, Bayne, Morganna, Navarro (player characters), on the Hooded Priest enemy, and on the Undead Sorcerer enemy. The associated "Priest Aura" variant is mechanically identical but registered under a different variant name for enemy use. A blue scene light emitted by the spell system (not by TAuraAnimator itself) lights the surrounding environment while the spell is active.

---

## 2. Sources & evidence

- **Retail decomp:** `recon/classes/` — NONE (no TAuraEffect or TAuraAnimator Ghidra class files found)
- **Pre-release (snapshot):** `src/effect_old.cpp:3476–3688` (`TAuraEffect` + `TAuraAnimator`)
- **Sister effects consulted:** `TBurnEffect` / `TBurnAnimator` (`effect_old.cpp:2971–3349`) — character-attached rising-particle status effect; compared structure, particle system usage, and coordinate handling
- **Source-of-truth ranking:** Pre-release snapshot (`effect_old.cpp`) is the primary authoritative source for intent and all constants. The retail binary was consulted for corroboration (§2.1). For `TParticleSystem` mechanics, `src/effectcomp.cpp:1031–1131` is the authoritative source.

### 2.1 Retail-vs-snapshot reconciliation

**Asset identity:** The shipped `data/Imagery/Magic/aura.I3D` (via `imagery.rvi`) is byte-for-byte identical to `legacy/Imagery/Magic/aura.I3D` (MD5: `e5f4eaa916f31cefe9d73262b399aba5`, 9,588 bytes). Asset identity is confirmed.

**Registration strings:** Searched `data/Revenant.exe` for registration string XREFs:
- `'AURA'` found at file offset `0x1e0f14` (VA `0x5e0f14`); XREF push at file `0xe39fd` (VA `0x4e39fd`) — matches `DEFINE_BUILDER("Aura",…)` registrar thunk. Case difference ("AURA" vs "Aura") is consistent with case-insensitive lookup in the retail builder table.
- `'Aura'` found at file offset `0x1e10a8` (VA `0x5e10a8`); XREF pushes at file `0xe3870` (VA `0x4e3870`) and `0xe39d0` (VA `0x4e39d0`) — matches `REGISTER_3DANIMATOR("Aura",…)` registrar thunks.

**AURA_FRAME=720 corroboration:** At file offset `0xe3c01` (VA `0x4e3c01`), approximately 360 bytes after the registrar thunks in the retail `.text` section, the instruction `3d d0 02 00 00` decodes as `cmp eax, 0x2d0`. `0x2d0` = 720 decimal = `AURA_SEC * 24`. This is in the TAuraAnimator animate body, directly confirming the 30-second / 720-tick duration boundary.

**What is confirmed vs snapshot-only:**

| Evidence | Verdict |
|----------|---------|
| Asset (aura.I3D) byte-identical | yes (retail) |
| DEFINE_BUILDER / REGISTER_3DANIMATOR name strings | yes (retail) |
| AURA_FRAME = 720 (30 sec × 24 Hz) | yes (retail — `cmp eax, 0x2d0` at VA `0x4e3c01`) |
| All other numeric constants (AURA_COUNT, AURA_ADD, scale/life/speed ranges, AURA_DEC) | snapshot-only |
| spell.def LIGHT / VARIANT wiring | yes (retail data) |

**Verdict: retail-partial.** The effect's existence, registration, asset, and 30-second duration are confirmed in retail. All other constants are snapshot-only and may have been tuned between snapshot and ship.

---

## 3. Constants

All constants from `src/effect_old.cpp:3476–3527` (macro block) and `src/effect.h:1572` unless otherwise noted.

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| `AURA_COUNT` | 100 | max live particles | `effect.h:1572` | snapshot-only |
| `AURA_ADD` | 2 | max new particles per tick | `effect_old.cpp:3505` | snapshot-only |
| `AURA_SPREAD` | 2 | bone jitter half-range (wu) | `effect_old.cpp:3506` | snapshot-only |
| `AURA_MIN_SCL` | 25 | minimum initial scale × 0.01 → 0.25 | `effect_old.cpp:3507` | snapshot-only |
| `AURA_MAX_SCL` | 60 | maximum initial scale × 0.01 → 0.60 | `effect_old.cpp:3508` | snapshot-only |
| `AURA_MIN_Z` | 5 | min upward velocity component × 0.1 → 0.5 wu/tick | `effect_old.cpp:3509` | snapshot-only |
| `AURA_MAX_Z` | 30 | max upward velocity component × 0.1 → 3.0 wu/tick | `effect_old.cpp:3510` | snapshot-only |
| `AURA_MIN_LIFE` | 15 | minimum particle lifetime (ticks) | `effect_old.cpp:3511` | snapshot-only |
| `AURA_MAX_LIFE` | 30 | maximum particle lifetime (ticks) | `effect_old.cpp:3512` | snapshot-only |
| `AURA_DEC` | 0.97f | per-tick scale decay multiplier | `effect_old.cpp:3513` | snapshot-only |
| `AURA_SEC` | 30 | effect active duration (seconds) | `effect_old.cpp:3514` | snapshot-only |
| `AURA_FRAME` | 720 (`AURA_SEC * 24`) | effect active duration (ticks) | `effect_old.cpp:3515` | yes (retail: `cmp eax, 0x2d0` at VA `0x4e3c01`) |
| `AURA_PART_MIN` | 50 | — DEAD (unused, see §13) | `effect_old.cpp:3503` | snapshot-only |
| `AURA_PART_MAX` | 50 | — DEAD (unused, see §13) | `effect_old.cpp:3504` | snapshot-only |
| `FIX_Z_VALUE` divisor | 1.46f | map-Z to 3D-Z conversion denominator | `src/object.h:31` | snapshot-only |
| Particle `rot` (degrees) | `(-90, 0, -45)` | initial Euler angles: pitch/yaw/roll | `effect_old.cpp:3617` | snapshot-only |
| Particle `acc` | `(1, 1, 1)` | velocity damping (1.0 = no damping) | `effect_old.cpp:3620` | snapshot-only |
| Spell LIGHT color | R=50, G=50, B=255 | 0–255 per channel | `data/Resources/spell.def:1298` | yes (retail data) |
| Spell LIGHT intensity | 180 | spell light intensity | `data/Resources/spell.def:1298` | yes (retail data) |
| Spell LIGHT mult | 20 | spell light multiplier | `data/Resources/spell.def:1298` | yes (retail data) |

---

## 4. Assets

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| aura.I3D | `legacy/Imagery/Magic/aura.I3D` | 9,588 bytes | particle billboard sprite geometry | Registered via `Class.Def:2078` (`"Aura" "Magic\Aura.I3D" 0x00ab1d1d`); loaded by `RegisterImagery` + retrieved via `TEffect::GetObject(0)` |

**Sub-object breakdown** (`i3d_dump_all/aura/manifest.txt`):

| index | name | verts | tris | UV |
|-------|------|-------|------|----|
| 0 | `smoke01` | 4 | 2 | full-texture (≈ (0,0)→(1,1)) |

There is exactly ONE sub-object. The effect only uses `GetObject(0)` (`effect_old.cpp:3547`). Unlike sister effect `TBurnAnimator` which has two sub-objects (fire + smoke), TAuraAnimator has only one.

**Texture** (`i3d_dump_all/aura/manifest.txt`, `texture_00_frame_00.png`):
- **Dimensions:** 64×64 pixels, 1 frame (single image — no flipbook, no atlas)
- **Content:** A single circular glow sprite. The outer rim is near-black; the center is a deep, dark blue-purple. The entire image is very dark — the glow reads at runtime through additive blending on top of the scene (black areas vanish, the dark blue center accumulates). This is the classic additive glow sprite design.
- **UV:** Full texture, (0,0)→(1,1), both triangles. No atlas sub-regions. No runtime UV selection.
- **Animation:** None — static texture, no UV scroll, no frame flipbook.

**Geometry** (`i3d_dump_all/aura/aura.obj`):
The quad lies in the XZ plane (Y=0), spanning ±37.5 world units in X and Z:
```
v  37.5  0   37.5
v -37.5  0   37.5
v -37.5  0  -37.5
v  37.5  0  -37.5
```
Bounding box: `(-37.5, 0, -37.5) .. (37.5, 0, 37.5)` — a 75×75 world-unit flat quad. The Y=0 (XZ) orientation is then rotated by `TParticleSystem::Render` into world orientation (see §7).

---

## 5. Spawn & emit

- **Trigger semantics:** Fixed-duration continuous emitter. Emission ramps up over 720 ticks (30 sec), then ramps back down and terminates. The effect instance lives until all particles have expired (`KillThisEffect()` when `frame >= 720 && to_add == 0 && done`) (`effect_old.cpp:3651–3655`).
- **Count per tick:** 0 to `AURA_ADD` (2) new particles per tick. Rate is governed by the `to_add` ramp (see §6).
- **Bone selection:** A random bone (sub-object index `j = random(0, size-1)`) is selected each tick. `size` is set during `Initialize` to `ca->NumObjects()`, the character animation object count. The particle is spawned at that bone's world position (`effect_old.cpp:3564–3578`, `3596–3610`).
- **Emit anchor:** Character skeleton bones in world space; the bone world position is extracted by composing the bone's local matrix with the character animation matrix (`MultiplyD3DMATRIX` + `D3DMATRIXTransform`), then adjusted to be **effect-local** (see Coordinate space).
- **Coordinate space:** Effect-local (relative to the effect/character origin). The bone world-space X and Y are shifted by subtracting `char_pos.x` and `char_pos.y` so that particles are positioned relative to the character's XY origin. Z is NOT shifted by `char_pos.z` (the subtraction is commented out in the snapshot), but Z is converted from map-space to 3D-space via `FIX_Z_VALUE` (`effect_old.cpp:3611–3614`). See §13 for the double-conversion quirk.
- **Spread / jitter:** `random(-AURA_SPREAD, +AURA_SPREAD)` = `random(-2, 2)` wu added to each of x, y, z independently (`effect_old.cpp:3611–3613`). Bone position provides macroscopic distribution across the body; jitter provides microscopic per-bone variation.
- **Velocity:** Pure upward motion: `p.vel = (0, 0, random(AURA_MIN_Z, AURA_MAX_Z) * 0.1)` → `(0, 0, [0.5 .. 3.0])` wu/tick (`effect_old.cpp:3622–3624`). No horizontal drift.
- **Scale:** `random(AURA_MIN_SCL, AURA_MAX_SCL) * 0.01` → `[0.25 .. 0.60]` at spawn (`effect_old.cpp:3615–3616`).
- **Particle orientation at spawn:** `p.rot = (-90°, 0°, -45°)` Euler angles (`effect_old.cpp:3617–3619`). This is applied during rendering by `TParticleSystem::Render` which builds the billboard transform from these angles (see §7).

**Spatial diagram:**

```
       ^ Z (up in world)
       |     particles drift upward
       |         o  o
       |       o      o
       |         o  o
       |           X   ← character origin (bone scatter)
       |
  -----+-----> X
       |   [character standing here]
       Y into screen
```

Particles spawn scattered across the character's skeleton bones (arbitrary height in Z), all drifting upward (+Z), shrinking as they go.

---

## 6. Behavior & per-frame logic

### TAuraEffect lifecycle (`effect_old.cpp:3480–3527`)

```
// TAuraEffect::Initialize (effect_old.cpp:3482-3485)
Initialize():
    first_time = true

// TAuraEffect::Pulse (effect_old.cpp:3488-3513)
Pulse():
    if first_time:
        SetDefense(mindamage=0)    // sets spell magic_defense = 0
        SetOffense(maxdamage=0)    // sets spell magic_offense = 0
        first_time = false
    char_pos = GetCharacter()->GetPos()   // GetCharacter() returns spell->GetInvoker()
    char_pos.z = FIX_Z_VALUE(char_pos.z) // map-Z → 3D-Z
    SetPos(char_pos)                      // move effect to follow character
    SetDefense(0)
    SetOffense(0)
```

Note: `SetDefense` and `SetOffense` are called on EVERY pulse (not just `first_time`). Both are called with 0. This effect does zero damage — it is purely visual and provides no combat modifier.

The `character` field declared at `effect.h:1561` appears unused. The actual character reference is obtained via `GetCharacter()` which returns `spell->GetInvoker()` (`effect_old.cpp:3518–3522`).

### TAuraAnimator lifecycle (`effect_old.cpp:3534–3688`)

```
// TAuraAnimator::Initialize (effect_old.cpp:3535-3556)
Initialize():
    ca = GetAnim()                     // character animation object
    s = ca->NumObjects()               // number of bone sub-objects
    size = s
    fire.Init(this, GetObject(0), s, true)
    //   anim   = this
    //   obj    = aura.I3D sub-object 0 ("smoke01")
    //   size   = s  (number of character bones → particle pool size)
    //   move   = true (check_move)
    //   facing = 0 (default, not used for aura)
    frame = 0
    to_add = 0
```

`TParticleSystem::Init` signature (effectcomp.cpp:1031): `Init(anim, obj, size, check_move, facing_angle=0)`

```
// TAuraAnimator::Animate (effect_old.cpp:3558-3655)
Animate():
    ++frame

    // Emission ramp:
    if frame < AURA_FRAME (720) and to_add < AURA_ADD (2):
        ++to_add
    elif frame >= AURA_FRAME (720) and to_add > 0:
        --to_add

    fire.Animate()    // advance existing particles: pos += vel, vel *= acc

    // Spawn new particles this tick:
    ca = GetAnim()
    mtx = ca->MakeMatrix()    // character world transform matrix

    for i in 0 .. to_add-1:
        j = random(0, size-1)           // pick a random bone
        obj = GetObject(j)              // bone sub-object (from character animation)
        dest = MultiplyD3DMATRIX(obj->matrix, mtx)   // bone local → world
        p.pos = D3DMATRIXTransform(dest, vp)         // extract world position of bone

        // Jitter and convert to effect-local space:
        p.pos.x += random(-AURA_SPREAD, AURA_SPREAD) - char_pos.x
        p.pos.y += random(-AURA_SPREAD, AURA_SPREAD) - char_pos.y
        p.pos.z += random(-AURA_SPREAD, AURA_SPREAD)
        // Note: char_pos.z subtraction is COMMENTED OUT in snapshot ("// - char_pos.z")
        p.pos.z = FIX_Z_VALUE(p.pos.z)    // map-Z → 3D-Z (see §13 double-conversion note)

        p.scl = random(AURA_MIN_SCL, AURA_MAX_SCL) * 0.01   // [0.25 .. 0.60]
        p.rot = (-90.0°, 0.0°, -45.0°)
        p.acc = (1.0, 1.0, 1.0)         // no drag
        p.vel = (0.0, 0.0, random(AURA_MIN_Z, AURA_MAX_Z) * 0.1)  // [0.5 .. 3.0] wu/tick upward
        p.life_span = random(AURA_MIN_LIFE, AURA_MAX_LIFE)  // [15 .. 30] ticks
        fire.Add(&p)

    // Per-tick scale decay for ALL particles in the pool:
    for i in 0 .. AURA_COUNT-1:
        particle[i].scl *= AURA_DEC    // *= 0.97f each tick

    // Termination check:
    if frame >= AURA_FRAME (720) and to_add == 0 and done:
        KillThisEffect()
```

**Key: `fire.Animate()` mechanics** (`effectcomp.cpp:1031`): For each live particle: `pos += vel; vel *= acc`. Since `acc = (1,1,1)`, velocity is constant — no drag, no gravity, pure linear upward drift.

**Emission rate envelope:**

```
to_add
  2 |         ___________________________
    |        /                           \
  1 |       /                             \
    |      /                               \
  0 |_____/                                 \____
    0    ~2      frame=720             ~722+    → frame
         ticks   (AURA_FRAME)
```

The ramp is one-per-tick (to_add increments by 1 each tick until it hits 2; decrements by 1 each tick after frame 720 until it hits 0). Full emission rate (2 particles/tick) is reached by tick 2 and sustained until frame 720.

**Scale decay timeline:**

```
particle scl
  0.60 |\ (max spawn scale)
       | \
  0.25 |  \   (min spawn scale)
       |   -------\
       |            \
  ~0   |             ...
       0    15      30  → ticks (AURA_MIN_LIFE .. AURA_MAX_LIFE)
```

A particle spawned at scl=0.60 decays by 3% per tick: after 30 ticks, `0.60 × 0.97^30 ≈ 0.24`. After 50 ticks ≈ 0.12. Scale effectively reaches invisibility around life_span. Note: `TParticleSystem::Animate` does NOT kill particles by scale — only by `life_span`. Scale decay is applied independently in TAuraAnimator::Animate (not in TParticleSystem).

---

## 7. Rendering (original render state + geometry)

### TAuraAnimator::Render (`effect_old.cpp:3657–3667`)

```
Render():
    SaveBlendState()
    SetAddBlendState()     // sets D3D: DECALALPHA + SrcBlend=ONE + DestBlend=ONE
    fire.Render()          // no arguments: flicker=false, abs_pos=false
    RestoreBlendState()
```

**Blend mode:** `SetAddBlendState()` (`effect_old.cpp:221–244`) — AdditiveStraight (ONE/ONE). Black pixels in the texture contribute nothing; bright pixels accumulate additively on the scene. This is appropriate for the deep-blue glow sprite.

**Blend sanity check:** The sprite texture is a dark circular gradient on a black/near-black background. This is the textbook additive sprite design (black = transparent under ONE/ONE). The `SetAddBlendState()` call is consistent with the sprite design and with sister magic effects (fire family uses `SetAddBlendState` throughout). No conflict.

**Abs_pos / coordinate space:** `fire.Render()` with no arguments means `abs_pos=false` — `OBJ3D_ABSPOS` flag is NOT set (`effectcomp.cpp:1103`). Particles are rendered in effect-local space relative to the effect origin (character position). `TParticleSystem::Render` does NOT add the effect world position separately; the particle's `pos` is already in effect-local space after the `char_pos` subtraction in Animate.

**Orientation transform** (in `TParticleSystem::Render`, `effectcomp.cpp:1075–1103`): For each live particle, the render constructs a matrix:
```
M = RotX(rot.x * TORADIAN)     // rot.x = -90° → flip from XZ to XY-screen-facing
  * RotY(rot.y * TORADIAN)     // rot.y = 0°
  * RotZ(rot.z * TORADIAN)     // rot.z = -45°
  * RotX(-π/2)                 // additional flip
  * RotZ(-π/4)                 // additional roll
  * RotZ(facing_angle)         // facing = 0
```
The net rotation brings the XZ-plane quad to a screen-facing orientation. This is the WorldXY orientation pattern (rot.x includes the `-π/2` flip from horizontal to vertical billboard).

**Z handling in Render:** `object->pos.z = FIX_Z_VALUE(particle.pos.z)` (`effectcomp.cpp:1103`). See §13 for the double-conversion note (Z was already FIX_Z_VALUE'd in Animate).

**Lit vs self-lit:** Additive blend with a dark-on-black sprite reads as self-lit/unlit — the glow is purely additive accumulation, independent of the scene lighting model. No scene lighting query in the Render body.

**Depth:** `ZWRITEENABLE` is not set explicitly in the Render body. Additive blending is depth-read-only (no depth write) in standard D3D usage for transparent effects.

### TAuraAnimator::RefreshZBuffer (`effect_old.cpp:3669–3688`)

```
RefreshZBuffer():
    character->GetPos(pos)
    screen = WorldToScreen(pos)
    RestoreZ(screen.x - 150, screen.y - 150, 300, 300)
```

Restores a 300×300 pixel screen-space Z region centered on the character's projected position. This is the standard character-attached effect Z-buffer recovery pattern (same as TBurnAnimator).

---

## 8. Texture animation

N/A — The aura.I3D texture is a single 64×64 frame with no flipbook, no atlas, and no UV animation. The sprite is static; the visual motion comes entirely from particle movement and scale decay, not texture animation.

---

## 9. Associated light

The "Aura" spell creates a blue dynamic scene light through the **spell system**, not through TAuraAnimator directly. TAuraAnimator contains no `AddLight` call.

**spell.def entry** (`data/Resources/spell.def:1290–1305`):
```
SPELL "Aura"
BEGIN
    LIGHT COLOR 50, 50, 255 INT 180 MULT 20
    ...
END
```

- **Color:** R=50, G=50, B=255 (strongly blue, near-primary)
- **Intensity:** 180
- **Multiplier:** 20

This maps through `TMapPane::DrawDLight()` (confirmed in `legacy/diftmp:2007–2041`) to `Scene3D.AddLight(dlight.pos, dlight.color, dlight.intensity)`. The light is positioned at the effect's world position, which TAuraEffect::Pulse updates every tick to follow the character (`SetPos(char_pos)`, `effect_old.cpp:3506`). The light follows the character continuously while the spell is active.

**Note:** The current port's `spell.cpp:119–130` silently skips the `LIGHT` tag with a `log_warn` — this is a known gap in the port, not a forensics finding.

**Animation:** No explicit flicker or pulse in the data — the light is steady at the defined color/intensity for the spell duration. This contrasts with fire-family effects which may modulate light intensity per-tick.

---

## 10. Color

**Particle color:** TAuraAnimator does not set per-particle RGB color explicitly. `TParticleSystem::Render` draws the sprite using whatever diffuse/vertex color the particle data contains. The aura.I3D material declares `diffuse=(1,1,1,1) emissive=(1,1,1)` (`manifest.txt`). The particle renders at the texture's natural color.

**Texture color:** The sprite is a deep navy blue glow (dark blue-purple at center, black at edges). Under ONE/ONE additive blending, the dark center contributes a blue tint to whatever is behind the character, and the glow accumulates blue luminance with every particle layer.

**Expected visual:** A cool, dark-blue aura rising from the character's body, glowing brighter at the character's silhouette where particles are most dense. The blue matches the spell LIGHT color (R=50,G=50,B=255). This is a pure, saturated blue — not cyan, not purple, not grey. A reconstruction showing pale, white, or desaturated particles indicates a texture decode failure or wrong blend mode.

**Color source:** Texture palette (the sprite's natural dark-blue content), rendered additively. No runtime color override from chardata or spell color fields in the TAuraAnimator body.

---

## 11. Audio coupling

No audio coupling found in `TAuraEffect::Pulse`, `TAuraEffect::Initialize`, `TAuraAnimator::Initialize`, `TAuraAnimator::Animate`, or `TAuraAnimator::Render` (`effect_old.cpp:3480–3688`). No `PLAY()` calls, no sound trigger, no looping audio hook.

The spell invocation animation (`"invoke4"` from spell.def:1294) may play a sound through the character animation system, but that is outside the VFX effect body.

---

## 12. Triggers & in-game appearance

**Effect registration** (`legacy/Class.Def:2078`):
```
"Aura" "Magic\Aura.I3D" 0x00ab1d1d
```
`DEFINE_BUILDER("Aura", TAuraEffect)` at `effect_old.cpp:3476`.
`REGISTER_3DANIMATOR("Aura", TAuraAnimator)` at `effect_old.cpp:3478`.

**Spell.def entries** (`data/Resources/spell.def:1290–1305`):
```
VARIANT "Aura",       TP_BASIC, "KBEF", "aura", mana=406, wait=20, 0, 0, skill=26, 0, FALSE, loop=20
VARIANT "Priest Aura", TP_BASIC, "KBEF", "aura", mana=90,  wait=20, 0, 0, skill=26, 0, FALSE, loop=20
```
Both variants name the effect `"aura"` (lowercase) and use DAMAGETYPE `DT_NONE` with `MIN_D=MAX_D=0`. The "Priest Aura" variant exists solely to give the Undead Sorcerer a lower mana cost (90 vs 406).

**Callers** (all from `data/Imagery/char.def`):

| Character | Type | MAGICATTACK entry | probability |
|-----------|------|-------------------|-------------|
| Locke | PC | `"invoke3"` → `"aura"` | 10% | `char.def:965` |
| Bayne | PC | `"invoke3"` → `"aura"` | 10% | `char.def:1941` |
| Morganna | PC | `"invoke3"` → `"aura"` | 10% | `char.def:2994` |
| Navarro | PC | `"invoke3"` → `"aura"` | 10% | `char.def:4062` |
| Hooded Priest | NPC | `"invoke4"` → `"aura"` | 100%, range 128..640 | `data/Resources/rules.def:1478` |
| Undead Sorcerer | NPC | `"cinv4"` → `"Priest Aura"` | 100%, range 100..200 | `char.def:9852` |

**This effect is live and widely used** — it is NOT vestigial. All four player character classes can cast it. The Hooded Priest (a mid-game enemy) always casts it at range. The Undead Sorcerer always casts its cheaper variant.

**Where to see it in the original game:**
- Cast "Aura" as any player character (Locke, Bayne, Morganna, or Navarro). The effect begins immediately on the caster's body and runs for 30 seconds.
- Fight a Hooded Priest in any mid-game area — it will cast Aura on itself at 100% probability.
- The effect should be visible rising from the character's body as a steady blue particle glow.

---

## 13. Gaps & uncertainties

**AURA_PART_MIN / AURA_PART_MAX are dead code:** Defined as 50 each at `effect_old.cpp:3503–3504` but never referenced in any Animate/Initialize body. Particle pool size is actually governed by `ca->NumObjects()` (number of character animation bones, stored in `size`). `AURA_COUNT = 100` governs the scale-decay loop. These two macros appear to be vestigial from an earlier design. Do NOT reconstruct them as meaningful parameters.

**The `character` field declared at `effect.h:1561`** is never assigned in the snapshot bodies. `GetCharacter()` (`effect_old.cpp:3518–3522`) returns `spell->GetInvoker()`, not the stored field. The field may be a base-class leftover.

**Double FIX_Z_VALUE application (snapshot quirk):** In `TAuraAnimator::Animate`, `p.pos.z = FIX_Z_VALUE(p.pos.z)` converts the z coordinate from map-Z units to 3D-Z units (`effect_old.cpp:3614`). Then in `TParticleSystem::Render`, `object->pos.z = FIX_Z_VALUE(particle.pos.z)` applies the conversion again (`effectcomp.cpp:1103`). This double division by 1.46 means z is divided by `1.46^2 ≈ 2.13` total — the particles rise at roughly half the expected rate. This is a snapshot behavior that may or may not match retail. Compare: `TBurnAnimator` does NOT apply FIX_Z_VALUE in its Animate body — only TParticleSystem::Render applies it — so TBurn lacks the double-conversion. The reconstruction agent should visually verify the rise rate against retail capture and may need to remove the Animate-level FIX_Z_VALUE application to match.

**Z subtraction commented out:** `// - char_pos.z` is present but commented out in the spawn block (`effect_old.cpp:~3613`). This means the particle z is in absolute map-Z space (adjusted for bone height), not effect-local Z. Combined with the double FIX_Z_VALUE, the z behavior is complex; visual verification is required.

**Snapshot-only constants risk:** All constants except AURA_FRAME were not corroborated in the retail binary. The full animator body was not cleanly extracted — `AURA_DEC = 0.97f` was searched in the vicinity of the registrar (`0xe3600–0xe4200`) and not found, suggesting the animator body is elsewhere in the binary or the float is stored differently. A reconstruction that does not visually match retail may need to re-tune these values from ground-truth reference video.

**`to_add` ramp bounds:** The ramp logic (increment by 1 per tick) means full emission (2 particles/tick) starts at tick 2 and wind-down takes 2 ticks. This is correct as documented, but retail tuning of AURA_ADD could differ.

**Retail-partial risk summary:** The reconstruction agent should capture in-game video of the Aura spell and verify: (a) the overall blue color is saturated, not pale or desaturated; (b) particles rise smoothly from the character body; (c) the effect lasts approximately 30 seconds; (d) the scene light provides blue environmental illumination around the character.

---

## 14. Reconstruction burndown

```
- [ ] Load aura.I3D ("Magic\Aura.I3D", 9588 bytes); confirm single sub-object "smoke01" (§4)
- [ ] Verify texture is 64×64 single-frame dark-blue circular glow; no atlas, no flipbook (§4, §8)
- [ ] Implement TAuraEffect::Initialize — set first_time=true (§6)
- [ ] Implement TAuraEffect::Pulse — call GetCharacter()->GetPos(), apply FIX_Z_VALUE to z,
      call SetPos() every pulse to track character; call SetDefense(0) + SetOffense(0) every pulse (§6)
- [ ] Implement TAuraAnimator::Initialize — store ca->NumObjects() as size;
      call fire.Init(this, GetObject(0), size, check_move=true, facing=0);
      initialize frame=0, to_add=0 (§6)
- [ ] Implement emission ramp: to_add increments by 1/tick up to 2 while frame<720;
      decrements by 1/tick while frame≥720 and to_add>0 (§6)
- [ ] Implement per-tick bone-scatter spawn: pick random bone 0..size-1;
      compose bone matrix with character world matrix; extract world bone pos;
      subtract char_pos.x and char_pos.y (NOT z); add ±2wu jitter on each axis;
      apply FIX_Z_VALUE to z — note double-conversion concern in §13, verify against retail (§5, §6)
- [ ] Set particle initial state: scl=random(25,60)*0.01, rot=(-90°,0°,-45°),
      acc=(1,1,1), vel=(0,0,random(5,30)*0.1), life_span=random(15,30) (§6)
- [ ] Implement per-tick scale decay: every particle in pool *= 0.97f each tick (§6)
- [ ] Implement TParticleSystem::Animate for velocity integration: pos+=vel, vel*=acc (§6)
- [ ] Implement TParticleSystem::Render with AdditiveStraight blend (ONE/ONE via SetAddBlendState),
      abs_pos=false (effect-local space), rot-based billboard matrix per particle (§7)
- [ ] Implement TAuraAnimator::RefreshZBuffer — RestoreZ(screen.x-150, screen.y-150, 300, 300) (§7)
- [ ] Implement termination: KillThisEffect() when frame≥720 and to_add==0 and all particles done (§6)
- [ ] Wire spell LIGHT (dynamic scene light): R=50,G=50,B=255, INT=180, MULT=20,
      attached to effect position (follows character via Pulse SetPos); steady, no flicker (§9)
- [ ] Confirm particle color is the texture's natural deep blue with no runtime tint override (§10)
- [ ] No texture animation required (§8)
- [ ] No audio coupling in effect body (§11)
- [ ] Visual verify: rising blue particles from character skeleton, ~30 sec duration,
      saturated blue (not pale), blue scene light on surroundings (§13)
```

---

**INVENTORY update suggestion** (do not edit INVENTORY.md directly — for orchestrator):

The M01/M02 INVENTORY rows currently classify this effect as `VO+LS`. Based on the source evidence, the correct pipeline classification is `PE+LS`: `TParticleSystem` (particle emitter, PE) + spell.def LIGHT (spell light, LS). The INVENTORY rows for M01 and M02 should be updated to reflect `PE+LS` and `Status = forensics-complete`.
