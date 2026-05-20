# SPARKS TSparkAnimator — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | (none yet — needs an INVENTORY row; suggest **X22**, see §1) |
| **Class(es)** | `TParticle3DAnimator` (the actual animator). `TSparkAnimator` is **not a class** — it is only the *builder symbol name* the registration macro mints (see §2, §7). Registered name = `"sparks"`. No paired `TEffect` subclass; the spawned object is a generic `TEffect`. |
| **Status** | forensics-complete (see §13 for the few genuine unknowns) |
| **Author / Date** | vfx-forensics-agent / 2026-05-19 |
| **Family** | blood/combat (combat-feedback; mechanically a generic spark burst) |
| **Draws** | particle emitter — N screen-aligned billboards, each one sub-object of `Misc\Sparks.I3D` placed at a particle position |
| **Archetype(s)** | (E) particle emitter — **simple emitter with gravity** (spawn, ballistic arc, fixed lifetime, die). No texture animation, no associated light, no state machine. The single-trigger **burst** sub-flavor. |

---

## 1. Summary

`"sparks"` is the generic spark-burst particle effect: a one-shot puff of small
bright billboards that fly out in a cone, arc under gravity, and wink out after a
short fixed lifetime. In the shipped game it fires when a melee attack is
**blocked** by a target's shield/parry (the impact sparks), spawned by
`TCharacter::EffectBurst("sparks")` (`src/character.cpp:1808`). It is a *reusable
building block*: the animator class, `TParticle3DAnimator`, is a fully generic
particle system parameterized entirely by an `SParticleParams` struct the caller
fills in — the commented-out sibling registrations (`icedsparks`, `sand`,
`blood`, `snow`, `src/effect_old.cpp:4689-4692`) confirm it was designed to drive
many particle effects, not just sparks. There are **no spark-specific constants
inside the animator**; every numeric (count, spread, gravity, lifetime, …) comes
from the caller's params (§3, §5). The sparks read as bright white/yellow
metallic sparks — the color is the authored `Misc\Sparks.I3D` photon-sprite
texture (§10), drawn translucent (Alpha blend), self-lit (§7).

> **INVENTORY action:** no row exists for this effect (INVENTORY has the generic
> infra row `I03 TParticle3DAnimator`, but no concrete "sparks" effect row). Add
> one to the *Misc / other* section. Suggested: `X22 | "sparks" (impact-block
> spark burst) | PE | effect_old.cpp:4688 | not-extracted | src/effect.h:1010
> (TParticle3DAnimator) | forensics-complete | - | forensics doc at
> docs/vfx/forensics/SPARKS_TSparkAnimator.md`.

---

## 2. Sources & evidence

- **Retail decomp:** **NONE.** No `recon/classes/` body and no `recon/mappings/`
  entry exists for `TParticle3DAnimator`, `TSparkAnimator`, or the `"sparks"`
  builder (confirmed by grep — see §13.1). The `"sparks"` *string* is XREF'd from
  the unrelated `TSymGlowAnimator` candidate
  (`recon/mappings/TSymGlowAnimator_cls_0x5a9e50_candidate.yaml:9-11`), which is a
  different effect that also emits spark particles — not this animator.
- **Pre-release (authoritative):** `src/effect_old.cpp`
  - Registration: `REGISTER_MULTI_3DANIMATOR_("sparks", TSparkAnimator, TParticle3DAnimator)` `src/effect_old.cpp:4688`.
  - `TParticle3DAnimator` bodies: `Initialize` `:4694-4705`, `Close` `:4707-4723`,
    `ResetTargetInfo` `:4731-4749`, `InitParticles` `:4752-4788`, `Animate`
    `:4790-4942`, `Render` `:4944-4986`, `RefreshZBuffer` `:4989-5002`.
  - Class + params declarations: `SParticleParams` `src/effect.h:987-1008`;
    `class TParticle3DAnimator` `src/effect.h:1010-1043`.
  - Registration macro: `REGISTER_MULTI_3DANIMATOR_` `src/3dimage.h:352-360`.
- **Caller (authoritative for the spark params):** `TCharacter::EffectBurst`
  `src/character.cpp:2225-2318` (the `else` branch `:2262-2317` fills the
  `SParticleParams`); the trigger `EffectBurst("sparks")`
  `src/character.cpp:1808`.
- **Asset:** `Misc\Sparks.I3D` registered `legacy/Class.Def:2035`
  (`"Sparks" "Misc\Sparks.I3D" 0x14db0f2e`); file at
  `legacy/Imagery/Misc/Sparks.i3d`.
- **Existing port shell:** none. `src/effect.h:1010-1043` declares
  `TParticle3DAnimator`; `src/effect.cpp:26` mentions the registration in a
  comment only. Nothing is wired in the modern port / `--test=vfx`.
- **Sister effects consulted:**
  - `TRibbonAnimator` (`src/effect_old.cpp:4084-4243`) — to **rule out** the
    "ribbon sparks" code (§13.2 / disambiguation below). That code is NOT this
    effect.
  - `TPhotonAnimator` (`src/missileeffect.h:84-93`) — same photon sprite family;
    corroborates the bright-spark color reading (§10).
- **Source-of-truth ranking:** pre-release `effect_old.cpp` is authoritative for
  all behavior; the **caller** `character.cpp` is authoritative for the actual
  spark parameter values (the animator has none of its own). No retail to diverge
  from.

### Disambiguation — the `:4117-4260` "ribbon sparks" are NOT this effect

The task flagged "ribbon sparks" code around `effect_old.cpp:4117-4260` (comments
"Initialize the starting positions of the ribbon sparks" `:4117`, "Accelerate the
sparks movements upward" `:4225`). **That code belongs to `TRibbonAnimator`**
(`src/effect_old.cpp:4084` Initialize, `:4143` Animate, `:4244` Render), the
"Ribbon" magic effect (`Magic\Ribbon.I3D`, INVENTORY X11), whose fields are
`NUM_RIBBON_SPARKS = 30` et al. at `src/effect.h:865-883`. Its upward
acceleration (`:4225`) is the ribbon-spiral behavior and has **nothing to do**
with `TSparkAnimator`. The generic spark burst is `TParticle3DAnimator`
(`:4688`, `:4694-5002`) and applies **downward** gravity (`v.z -= gravity`,
`:4850`), not upward acceleration. Do not conflate them.

---

## 3. Constants

`TParticle3DAnimator` has **no hardcoded spark constants** — every numeric is a
field of the `SParticleParams` the caller passes to `InitParticles`. The table
below gives the values for the **shipped `"sparks"` use** (the block-spark burst),
all from `TCharacter::EffectBurst` (`src/character.cpp`), plus the engine-side
constants in the animator.

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| particle count | `random(15, 25)` (inclusive) | count | character.cpp:2290 | yes |
| spawn origin (pos) | `vect0` = vector(caster→target, dist/2) with `z += 45` | wu, caster-local | character.cpp:2283-2293 | yes |
| pspread (position jitter) | `(3, 3, 3)` × `random(-100,100)/100` ⇒ ±3 wu each axis | wu | character.cpp:2294-2296 | yes |
| dir (mean velocity) | `vect/100` where `vect = vector(face±random(-80,80), 100)` ⇒ unit-ish XY toward a randomized facing, z≈0 | wu/tick | character.cpp:2274-2277, 2297-2299 | yes |
| spread (velocity jitter) | `(0.5, 0.5, 0.5)` × `random(-100,100)/100` ⇒ ±0.5 wu/tick each axis | wu/tick | character.cpp:2300-2302 | yes |
| gravity | `0.2` | wu/tick² (added to `v.z` each tick) | character.cpp:2303 | yes |
| trails | `1` | count (render-only sub-steps per particle, §7) | character.cpp:2304 | yes |
| minstart / maxstart | `0` / `8` | tick start-delay range, `random()` per particle | character.cpp:2305-2306 | yes |
| minlife / maxlife | `20` / `40` | tick lifetime range, `random()` per particle | character.cpp:2307-2308 | yes |
| bounce | `false` | bool | character.cpp:2309 | yes |
| killobj | `true` | bool — object self-destructs when all particles done | character.cpp:2310 | yes |
| objflags | `1 << (ObjId() & 0x3)` | bitmask — selects exactly ONE of the 4 sub-objects (§4) | character.cpp:2311 | yes |
| seektargets / numtargets | `false` / `0` | no homing | character.cpp:2312-2313 | yes |
| EffectBurst `height` arg | `50` (default, unused for "sparks") | wu | character.h:311 | yes (unused — see §5) |
| spawn-facing jitter | `random(-80, 80)` byte-angle | added to caster face | character.cpp:2274 | yes |
| **sample-default count** | `10` | count — fallback only if `params.particles==0` (§6.1) | effect_old.cpp:4797 | yes (dev fallback, not the live path) |
| RefreshZBuffer patch | `size_x=75, size_y=50`, offset `+25 / −50` px | screen px | effect_old.cpp:4996-5000 | yes |

`random(min,max)` is **inclusive** on both ends (`rand() % (max-min+1) + min`,
`src/revutils.cpp:1609-1610`).

> **Reconstruction note:** because the animator is generic, the only way to get
> "sparks" right is to feed it *these* params. They are not in the animator — they
> are in `character.cpp:2290-2313`. Treat that block as the spark constant table.

---

## 4. Assets

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| Sparks | `legacy/Imagery/Misc/Sparks.i3d` | 16,300 B | the spark sprite imagery — STILL (2D billboard) I3D with **4 sub-objects** | registered `Class.Def:2035` under name `"Sparks"` (ID `0x14db0f2e`); loaded by the OBJCLASS_EFFECT registry when the effect spawns; bound by `T3DAnimator` |

**Sub-objects (4)** — read directly from the I3D object table at file offset
`0x7c4` (`04 00 00 00` count, then four 28-byte name records):

| # | name | role |
|---|------|------|
| 0 | `photon`   | spark billboard quad (variant 0) |
| 1 | `photon01` | spark billboard quad (variant 1) |
| 2 | `photon02` | spark billboard quad (variant 2) |
| 3 | `photon03` | spark billboard quad (variant 3) |

All four are billboard quads sharing the same authored photon-sparkle sprite
(the imagery is the same family as `TPhotonAnimator`, `src/missileeffect.h:84`).
The four named variants exist so each particle can pick one at random
(`objflags` / `o[c]`, §6) for subtle variety. Imagery header (file offset `0x14`
onward): type tag `STILL` (`0x1C`), texture cell **64 × 80** px
(`0x44`: `40 00` = 64 wide, `50 00` = 80 high), 16-bit (RGB565) pixel data — the
sprite is a small bright spark/sparkle, **white/yellow-white** (texture region
`0x174`–`0x3FAC`). No multi-frame `framehtexs` array (single still per object →
no flipbook, §8).

**The effect loads a real asset — do NOT substitute a procedural spark sprite.**
The authored 4-variant photon sprite is the visual identity (§10).

---

## 5. Spawn & emit

- **Trigger semantics:** **one-shot burst.** All particles are seeded at once in
  `InitParticles` (`effect_old.cpp:4774-4787`); no continuous emission. The object
  lives only until every particle's life elapses, then self-destructs
  (`killobj=true`, §6).
- **Count per trigger:** `random(15, 25)` particles (`character.cpp:2290`).
- **Initial direction / distribution:** a **cone**, biased along a randomized
  horizontal facing. The caller computes `vect = ConvertToVector(face +
  random(-80,80), 100)` (`character.cpp:2274-2277`) — i.e. a horizontal unit-ish
  vector pointing roughly back along the attacker's facing, jittered ±80
  byte-angle (≈ ±112°, a wide fan). Mean per-particle velocity is `dir = vect/100`
  (so XY magnitude ≈ 1 wu/tick, z ≈ 0). Each particle then adds velocity jitter
  `spread = ±0.5` wu/tick on each of x/y/z (`effect_old.cpp:4779-4781`,
  `character.cpp:2300-2302`). Net: a fan of sparks spraying horizontally outward
  with a little vertical scatter, then falling under gravity.
- **Emit anchor convention:** the effect *object* is placed at the **caster's
  world position** (`def.pos = pos`, `character.cpp:2264`). Particles are seeded
  **relative to that object origin** by `pos = params.pos + jitter`
  (`effect_old.cpp:4776-4778`), where `params.pos = vect0` is a caster-local
  offset = `ConvertToVector(caster→target facing, distance/2)` with `z += 45`
  (`character.cpp:2283-2293`). So the burst originates **roughly halfway between
  the caster and the blocked target, at ~45 wu up** — i.e. about chest/weapon
  height at the point of impact, not at the caster's feet. (`EffectBurst`'s
  `height` arg defaults to 50 but is only used by the `"blood"` branch; for
  `"sparks"` the z offset is the hardcoded `+45` on `vect0`,
  `character.cpp:2288`.)
- **Coordinate space:** particle positions/velocities are integrated in the
  effect's **local space** (relative to the object origin); the object origin is
  the caster's world pos. Render places each sub-object at the particle's local
  `p[c]` via `OBJ3D_POS1` (§7).
- **Spread / jitter:** position ±3 wu/axis (`pspread`), velocity ±0.5 wu/axis
  (`spread`), facing ±80 byte-angle, count 15–25, lifetime 20–40 ticks, start
  delay 0–8 ticks — all uniform `random()`.

### Spatial diagram

```
   wz (up)
   │                  ScreenAligned spark billboards (face camera),
   │        · ·  ·    one of 4 photon variants each
   │      ·  ·  · ·   ← 15..25 sparks, ±3wu pos jitter, fan cone
   │     · ·· ·       velocity ≈1 wu/tick along (face±80°), z≈0,
   │    ·  ·          then gravity 0.2 wu/tick² pulls them down
   └──────────────── wx
  ╱   ▲ emit origin = caster_local + vector(caster→target, dist/2),
 ╱      z = +45 wu  (≈ impact point, mid-way to the blocked target)
wy

cone fan (top-down):  attacker faces →  sparks spray back over ±~112° arc
```

---

## 6. Behavior & per-frame logic

The whole effect is the generic `TParticle3DAnimator::Animate`
(`effect_old.cpp:4790-4942`). For the spark use only the *non-seeking* path runs
(`seektargets=false`), so the large homing block (`:4859-4936`) is **dead code
for sparks** — documented in §13.3, not reconstructed.

### 6.1 Seeding (`InitParticles`, `effect_old.cpp:4752-4787`)

```
InitParticles(params):                       // effect_old.cpp:4752
    copy params; allocate p[],v[],s[],l[],o[] (size = particles)   // :4754-4760
    build objs[] = list of sub-object indices whose bit is set in objflags  // :4764-4770
        if none set, objs = {0}                                    // :4771-4772
    for c in 0..particles-1:                                       // :4774
        p[c] = params.pos    + params.pspread * random(-1.00..1.00)   // :4776-4778  (±jitter per axis)
        v[c] = params.dir    + params.spread  * random(-1.00..1.00)   // :4779-4781
        l[c] = random(minlife, maxlife)        // ticks of life     // :4782
        s[c] = random(minstart, maxstart)      // start delay ticks  // :4783
        o[c] = objs[random(0, numobjs-1)]      // which sub-object   // :4784
```

For sparks, `objflags = 1 << (ObjId() & 3)` selects exactly **one** variant, so
every particle in a given burst draws the **same** photon variant (the per-burst
variety, not per-particle). (If `objflags` had multiple bits, particles would mix
variants.)

### 6.2 Per-tick update (non-seeking path, `effect_old.cpp:4826-4942`)

```
Animate(draw):                                // effect_old.cpp:4790
    T3DAnimator::Animate(draw)                //   base: refresh hierarchy
    if params.particles == 0: seed sample defaults (dev only, §6.1 note)  // :4794-4824

    isdone = true
    for c in 0..particles-1:                  // :4829
        if l[c] <= 0: continue                //   already dead       // :4831
        isdone = false
        if s[c] > 0: s[c]--; continue         //   still in start delay  // :4836-4840
        p[c] += v[c]                          //   integrate position // :4842-4844
        l[c]--                                //   age                 // :4846
        v[c].z -= params.gravity              //   gravity (sparks: 0.2) // :4847-4851
        // bounce path (params.bounce=false for sparks → skipped)     // :4852-4858
        // seek path (params.seektargets=false for sparks → skipped)  // :4859-4936

    inst->SetCommandDone(isdone)              // :4939
    if params.killobj && isdone: inst->SetFlags(OF_KILL)   // self-destruct  // :4940-4941
```

- **Velocity integration:** plain Euler, `p += v` then `v.z -= gravity` each tick.
- **Gravity:** `0.2` wu/tick² downward (sparks fall in a ballistic arc).
- **Lifetime / death:** each particle dies when `l[c]` reaches 0 (after 20–40
  ticks). When *all* are dead, `isdone` stays true and the object kills itself
  (`killobj=true`).
- **No color/alpha/scale curve.** The animator never modifies per-vertex color,
  alpha, or scale over life — particles render at constant size/brightness then
  simply stop being drawn when dead (a hard pop-off, not a fade). The visual
  fade-feel comes only from sparks thinning out as individuals expire at staggered
  times (lifetime + start-delay jitter). (Confirmed: `Render` `:4944-4986` writes
  only `obj->pos`, no color/alpha/scale.)

> **6.1 sample-default note.** If a caller leaves `params.particles == 0`, Animate
> seeds a built-in 10-particle demo set (`effect_old.cpp:4794-4824`: pos
> (0,0,70), pspread 3, dir (1,−1,0.5), spread 0.5, gravity 0.2, life 10–30,
> objflags 0xF). This is a **developer fallback**, not the live spark path
> (`EffectBurst` always sets `particles = random(15,25)`). Do not reconstruct it
> as spark behavior; note it as the no-params default.

### Temporal diagram

```
per particle (no curves — constant draw, then off):
draw? ┤████████████████████          (drawn while s==0 and l>0)
      │        ↑start-delay s (0..8)  ↑death at l ticks (20..40)
      └────────────────────────────── ticks
            staggered start + staggered death ⇒ the BURST as a whole
            ramps in over ~0..8 ticks and thins out over ~20..40 ticks
position z: ↗ small up/flat, then ↘ parabola under gravity 0.2
```

---

## 7. Rendering (original render state + geometry)

- **What it draws:** for each live, started particle, one sub-object of
  `Sparks.I3D` (a billboard quad) placed at the particle's position.
  `TParticle3DAnimator::Render` (`effect_old.cpp:4944-4986`):

```
Render():                                     // effect_old.cpp:4944
    SaveBlendState(); SetBlendState();         //   Alpha mode (see below)  // :4946-4947
    ResetExtents()
    for c in 0..particles-1:                   // :4951
        if s[c] > 0 or l[c] <= 0: continue     //   skip delayed/dead       // :4953
        pp = p[c]; vv = v[c]
        obj = GetObject(o[c])                  //   pick the chosen sub-object // :4959
        for d in 0..trails-1:                  //   trails=1 for sparks ⇒ 1 draw // :4961
            obj->flags |= OBJ3D_POS1           //   override position only   // :4963
            obj->pos = pp                      //   place at particle pos    // :4964
            RenderObject(obj)                  // :4965
            pp += vv; vv.z -= gravity          //   (only matters if trails>1) // :4967-4970
    UpdateExtents(); RestoreBlendState()        // :4982-4983
```

- **Blend mode (original):** **Alpha.** `Render` calls `SetBlendState()`
  (`effect_old.cpp:4947`), NOT `SetAddBlendState()`. `SetBlendState`
  (`effect_old.cpp:221-233`) = texture stage `D3DTBLEND_MODULATE`,
  `SRCBLEND=SRC_ALPHA`, `DESTBLEND=INV_SRC_ALPHA`, `ZWRITEENABLE=false`,
  `ZENABLE=true` → classic translucent **Alpha (modulated)** blend
  (NOMENCLATURE §3). Trace confirms it: this is alpha-blended, not additive,
  despite sparks "looking" glowy.
- **Lit vs self-lit:** **Unlit / self-lit.** The animator never folds ambient
  light into vertex color and never zeroes the material — it simply draws the
  imagery's authored verts/texture. The color is literal (from the sprite). No
  scene lighting applied. Classify **Unlit** (NOMENCLATURE §4).
- **Depth / Z:** **TestNoWrite** — `ZENABLE=true`, `ZWRITEENABLE=false` (set by
  `SetBlendState`, `effect_old.cpp:224-225`). `RefreshZBuffer`
  (`effect_old.cpp:4989-5002`) restores scene Z over a fixed 75×50 px patch around
  the projected effect origin so the no-depth-write sparks composite correctly.
- **Orientation:** **ScreenAligned** (camera-facing billboards). Render sets only
  `obj->pos` via `OBJ3D_POS1`; it never sets `rot.x` (no `-π/2` ground tip) — the
  photon sub-objects keep their authored STILL-billboard facing. Default
  particle/spark orientation (NOMENCLATURE §2; AGENT_GUIDE §4.2.1.6 — "fire
  sparks … should always face the camera").
- **Per-quad transform:** position only (`OBJ3D_POS1`, particle `p[c]`). No
  rotation, no scale override, no per-quad spin. Size = the sub-object's authored
  quad size.
- **Per-vertex color packing:** NONE written by the effect. Verts come from the
  imagery (`T3DAnimator::GetVerts` → `GetObjVerts`, `src/3dimage.cpp:2749-2764`)
  with the imagery's authored color; under MODULATE the sampled spark texel is
  multiplied by that authored diffuse. The effect adds no tint.
- **`trails` field:** for sparks `trails = 1`, so the inner draw loop
  (`effect_old.cpp:4961-4978`) runs once — a single billboard per particle. With
  `trails > 1` the loop would draw the particle plus N−1 ghost copies stepped
  forward along its velocity (a motion-streak); not used by sparks.

---

## 8. Texture animation

**N/A — none.** The 4 photon sub-objects are single-still sprites (no
`framehtexs` array in the I3D; §4). `Render` never mutates `tu/tv` and never calls
`SetTextureFrame`. The texture is static; all motion is positional (the particle
flying), not UV/flipbook. (Contrast: F03 fire = framehtexs flipbook; this is
neither.)

---

## 9. Associated light

**N/A — none.** `TParticle3DAnimator` (Initialize/Animate/Render,
`effect_old.cpp:4694-5002`) makes no `AddPointLight` / dynamic-light call, and the
caller `EffectBurst` (`character.cpp:2225-2317`) adds none. Block sparks do not
light the scene in the original — they are self-lit billboards only.

---

## 10. Color

- **Source:** the **authored `Misc\Sparks.I3D` photon sprite texture** (§4). The
  effect supplies no color of its own (no per-vertex tint, no spell color, no
  chardata field). Color is whatever the sprite is, modulated by the imagery's
  authored vertex diffuse (effectively white → texture passes through unchanged).
- **Exact values:** the sprite is a bright **white / yellow-white** spark/sparkle
  (RGB565 texture data, `legacy/Imagery/Misc/Sparks.i3d` `0x174`–`0x3FAC`); same
  photon-sparkle family as `TPhotonAnimator` (`src/missileeffect.h:84`). Not a
  saturated hue — sparks are deliberately near-white hot, with a warm/yellow core.
- **Expected visual:** bright, hot, near-white metallic sparks (think
  steel-on-steel block impact). Under Alpha blend on a dark background they read
  as crisp bright pinpoints; against bright BG the sprite's own alpha edges keep
  them readable. **A pale/gray/washed result at reconstruction = broken port**
  (likely culprits per AGENT_GUIDE §4.2.1.5: a procedural stand-in instead of the
  real photon sprite #1, or wrong blend/chroma-key). The sparks are *meant* to be
  bright-white, so "near-white" is correct here — but verify it's the **sprite's**
  white, not a default-white fallback (i.e. confirm the texture loaded).
- **Normalization / boosts:** none.

---

## 11. Audio coupling

No audio is triggered by the effect itself (`TParticle3DAnimator` makes no
sound call). The **block sound** is played by the *caller*, right next to the
spark spawn: `PlayWave(listrnd(chardata->blocksounds))` (`character.cpp:1809`),
i.e. a per-character random "block/parry clang" plays on the same event that
spawns the sparks. Record it for the audio phase: **the block-spark burst is
audio-paired with `chardata->blocksounds` at the spawn site
(`src/character.cpp:1804-1810`)**, but the pairing is in combat code, not in the
effect.

---

## 12. Triggers & in-game appearance

- **Spawned by:** combat block resolution. `TCharacter` attack handling, on a
  **missed** swing that the target is **blocking**, calls
  `EffectBurst("sparks")` when the attack carries the `CA_SPARKS` flag
  (`0x0100`, "Show sparks on block", `src/rules.h:40`):
  `if (targ && targ->doing->action == ACTION_BLOCK) { if (attack->flags &&
  CA_SPARKS) EffectBurst("sparks"); ... }` (`src/character.cpp:1805-1810`).
  The registration is `REGISTER_MULTI_3DANIMATOR_("sparks", TSparkAnimator,
  TParticle3DAnimator)` (`effect_old.cpp:4688`); the object is built as a generic
  `TEffect` (no `DEFINE_BUILDER("sparks", …)` exists — the `EffectClass`
  catalogue registers the `"Sparks"` type from `Class.Def:2035`, and the
  per-instance animator resolves to `"sparks"` via `CreateAnimator`,
  `character.cpp:2271-2272`).
- **Where to see it in the original game:** attack any enemy who is actively
  **blocking** with a weapon/attack that has `CA_SPARKS`, and miss into the
  block — sparks fly off the parry. Easiest repro: get an enemy to raise a block
  and swing at it (shield/sword block). The burst appears mid-way between you and
  the target at ~chest height.
- **Vestigial?** No — it has a live caller (combat block). Note that
  `EffectBurst` itself begins with a commented `// return;` (`character.cpp:2227`)
  — the function is live (the `return` is disabled).
- **Reusability:** the same `TParticle3DAnimator` was meant to drive `icedsparks`,
  `sand`, `blood`, `snow` (commented siblings, `effect_old.cpp:4689-4692`) and is
  invoked generically anywhere a caller fills `SParticleParams` and calls
  `InitParticles` (the only other live spark-style call in the pre-release tree
  is the snow/sand emitters at `effect_old.cpp:8116`, `:9543`, which build their
  own params). For "sparks" specifically, the block path is the one live trigger.

---

## 13. Gaps & uncertainties

- **13.1 No retail decomp.** There is no `recon/classes/` body or
  `recon/mappings/` entry for `TParticle3DAnimator` / `TSparkAnimator` / `"sparks"`
  (verified by grep). Behavior here is **pre-release-authoritative**; the
  pre-release source is complete (full bodies present), so this is low risk — but
  flag it: there is no retail cross-check. The only `"sparks"` string XREF in
  recon lands in the *unrelated* `TSymGlowAnimator` candidate
  (`recon/mappings/TSymGlowAnimator_cls_0x5a9e50_candidate.yaml:9-11`).
- **13.2 Ribbon-spark code is a different class** — resolved (see §2
  disambiguation): `effect_old.cpp:4117-4260` is `TRibbonAnimator`, not this
  effect. The "accelerate upward" comment at `:4225` is ribbon behavior; sparks
  use downward gravity.
- **13.3 Dead code in the shared animator.** For the spark use, the homing/seek
  block (`effect_old.cpp:4859-4936`), the bounce block (`:4852-4858`), and the
  `ResetTargetInfo` machinery (`:4731-4749`) never run (`seektargets=false`,
  `bounce=false`, `numtargets=0`). Do NOT reconstruct them as spark behavior.
  They exist because the animator is generic.
- **13.4 No fade/scale curve.** Confirmed the animator applies no alpha/scale
  envelope (§6, §7) — particles pop off at end-of-life rather than fading. If the
  in-game sparks visibly fade, that fade would have to come from the sprite's own
  authored content or a render-side detail not in this code path; from the source,
  there is no per-particle fade. The "soft" feel comes from staggered
  start-delay + lifetime jitter thinning the burst. (Reasonable: bright sparks
  winking out individually reads fine without a per-spark fade.)
- **13.5 Object-variant selection.** `objflags = 1 << (ObjId() & 3)`
  (`character.cpp:2311`) keys the chosen photon variant off the *attacker's*
  object id, so a given character always throws the same spark variant. Whether
  this was intentional (cheap determinism) or meant to be per-particle random is
  unknown; reconstruction can keep it as-is or randomize per particle without a
  visible difference (the 4 variants are near-identical sprites). Low impact.
- **13.6 Exact sprite color.** The texture is RGB565 and reads white/yellow-white
  by inspection of the byte pattern + the photon-family lineage; the precise hot
  core hue should be confirmed against the decoded sprite during reconstruction
  (decode `Sparks.i3d` cell 64×80, or capture an in-game block). Not a behavioral
  unknown — just "render the real texture and verify."

---

## 14. Reconstruction burndown

```
- [ ] Load Misc\Sparks.I3D (16,300 B). Address its 4 STILL billboard sub-objects
      by index: 0=photon, 1=photon01, 2=photon02, 3=photon03 (64x80 sprite cell).
      NO procedural spark stand-in — use the real photon sprite. (§4)
- [ ] Drive it as the GENERIC particle system TParticle3DAnimator, parameterized by
      SParticleParams — there are no spark-specific constants in the animator; the
      values live in the caller (§3). (§2, §3, §6)
- [ ] Spawn = one-shot BURST: random(15,25) particles seeded at once; object self-
      destructs (killobj) when all particles' life elapses. (§5, §6.2)
- [ ] Emit origin = caster-local offset vector(caster→target, dist/2) with z=+45;
      object placed at caster world pos; particles in object-local space. (§5)
- [ ] Initial velocity = cone fan: dir = vector(face ± random(-80,80) byte-angle,
      mag~1)/100 (z≈0); per-particle velocity jitter ±0.5 wu/tick each axis;
      position jitter ±3 wu each axis. (§5, §6.1)
- [ ] Per-particle: random start-delay 0..8 ticks, random lifetime 20..40 ticks. (§3, §6.1)
- [ ] Per-tick update (24 Hz sim gate): p += v; v.z -= gravity(0.2); l--; die at l<=0.
      Plain Euler ballistic arc, downward gravity. (§6.2)
- [ ] NO color/alpha/scale curve — constant draw then pop off at end-of-life; the
      burst "fades" only via staggered start+death. (§6.2, §13.4)
- [ ] Core geometry: one ScreenAligned (camera-facing) billboard per live particle,
      placed at p[c] via position-override only (no rotation/scale). trails=1 ⇒ one
      draw per particle. (§7)
- [ ] Blend = Alpha (SetBlendState: MODULATE, SRC_ALPHA/INV_SRC_ALPHA — NOT additive),
      lit-mode = Unlit (asset color, no per-vertex tint, no material zeroing),
      depth = TestNoWrite. (§7)
- [ ] Texture animation = NONE (single-still sprite, no UV scroll, no flipbook). (§8)
- [ ] Variant pick: objflags selects one of the 4 photon sub-objects per burst
      (1 << (ObjId() & 3)); per-particle index o[c] from the selected set. (§6.1, §13.5)
- [ ] Associated dynamic light: NONE. (§9)
- [ ] Color from Sparks.I3D photon texture = bright white/yellow-white; verify the
      real sprite loads (pale/gray ⇒ stand-in / wrong blend / chroma-key). (§10)
- [ ] Sub-effects spawned: NONE. (§6)
- [ ] RefreshZBuffer: restore scene Z over a 75x50 px patch (offset +25/-50) around
      the projected effect origin. (§7)
- [ ] Audio (record for audio phase, NOT in effect): block sound is played by the
      CALLER alongside the spawn — PlayWave(listrnd(chardata->blocksounds)),
      character.cpp:1809. The effect itself triggers no sound. (§11)
- [ ] Trigger wiring: spawned on a blocked melee miss when attack has CA_SPARKS
      (rules.h:40); see character.cpp:1805-1810 + EffectBurst character.cpp:2262-2317. (§12)
- [ ] Do NOT reconstruct: the seek/homing block, the bounce block, ResetTargetInfo,
      or the params==0 sample-default — all dead/unused for the spark path. (§13.3, §6.1)
```

**Definition of done:** a blocked attack produces a one-shot fan of ~15–25 bright
white photon-sprite billboards at ~chest height between attacker and target, each
flying out along a wide cone, arcing down under gravity, winking out over ~20–40
ticks, alpha-blended and camera-facing, with no light and no per-particle
fade — and the burst dies on its own when the last spark expires.
