# VFX Nomenclature

Canonical vocabulary for forensics + reconstruction docs. Use these exact terms
so the forensics agent and reconstruction agent never talk past each other. If
you need a term not defined here, add it (with a definition) rather than
inventing a synonym for an existing concept.

The coordinate/projection facts below are project invariants — see
[../../DEFERRED_LIGHTING.md](../../DEFERRED_LIGHTING.md) and the
`project-world-space-invariant` memory.

---

## 1. Coordinate spaces

| term | meaning |
|------|---------|
| **world space** | Absolute game coordinates `(wx, wy, wz)`. `wz` is up. The deferred renderer is world-space end-to-end; effect math should be world-space. |
| **local space** | Relative to an effect's anchor point. `DrawPos` in a `Local`-space bucket is added to the bucket's `anchor` at materialization. Used for owner-following effects (torch wisps, auras). |
| **screen space** | 2D framebuffer pixels. Only the final billboard expansion touches this; never leak screen math into depth/light/emit logic. |
| **anchor** | The world point an effect originates from / follows. For character-attached effects, resolved each frame from an `SVfxAnchor` (bone / root / bounds-center). |
| **emit origin** | Where a spawn actually places particles. May equal the trigger `(x,y,z)` OR be ground-projected (z=floor) OR character-relative. The **emit anchor convention** (forensics §3) records which. |
| **bone-local** | Relative to a character mesh sub-object's matrix (`T3DAnimator::GetObjectMatrix`). Bone names are real I3D sub-object names: `rhand`, `lhand`, `head`, `weapon`, `sword`, etc. |

### Iso projection (reference)
World → screen for the classic iso view: `S = wx - wy`, `T = 0.5*(wx+wy) - 0.867*wz`.
A reconstruction agent rarely computes this directly (the FX shaders do it), but
forensics should note when a pre-release transform is pre- vs post-projection.

---

## 2. Orientation (quad facing)

Revenant billboards are almost always ONE of these — not an arbitrary world
rotation. If forensics produces a loose world rotation, re-read the matrix.

| term | engine enum | meaning | pre-release tell |
|------|-------------|---------|------------------|
| **ScreenAligned** | `EFxBillboardOrientation::ScreenAligned` | Quad always faces the camera (camera-aligned billboard). Default. Fire sparks, flame, glows, projectile heads. | no `rot.x` tip; quad expands in screen space |
| **WorldXY** | `EFxBillboardOrientation::WorldXY` | Quad lies flat on the world XY (ground) plane, foreshortens under the iso/perspective view. Ring halos, ripples, ground decals, AoE markers, ground-scatter fire. | `rot.x = -π/2` tips the authored-XY quad onto the floor |
| **WorldUpAligned** | *(future enum)* | Quad's up axis pinned to world +Z, rotates about Z to face camera (flame-style columns). Not yet implemented — flag as a needed extension if forensics finds it. | `rot.x = 0`, billboard about Z only |
| **StretchedAlongVelocity** | *(future enum)* | Quad stretched along the particle's motion vector (motion-streak). Not yet implemented. | per-particle length scaled by speed |

**in-plane rotation** — a static spin of the quad about its own facing axis
(e.g. F03's per-quad `rot.z = -π/4`). Distinct from orientation; currently no
per-instance knob for WorldXY in-plane rotation (carve-out F03c).

---

## 3. Blend modes

| term | engine enum | factors | use |
|------|-------------|---------|-----|
| **Alpha** | `EFxBlend::Alpha` | SRC_ALPHA / ONE_MINUS_SRC_ALPHA | textured droplets, decal-style strips (sword swipe), default translucency |
| **Additive** | `EFxBlend::Additive` | SRC_ALPHA / ONE | alpha-weighted additive — smoke, spark trails |
| **AdditiveStraight** | `EFxBlend::AdditiveStraight` | ONE / ONE | retail "self-lit overlay" — glows, explosions, the TBloodSystem additive path |
| **PremulAlpha** | `EFxBlend::PremulAlpha` | ONE / ONE_MINUS_SRC_ALPHA | assets authored with baked-in coverage |
| **Decal** | maps to `Alpha` | D3DTBLEND_DECAL = SRC_ALPHA / INV_SRC_ALPHA | the pre-release `SetBlendState()` default for textured fire/flame/flare quads |

**Trace the actual `SetBlendState`/`SaveBlendState` helper** to classify — don't
guess from how the effect "looks." F03 was wrongly assumed additive; it was Decal.

---

## 4. Lighting modes

| term | engine enum | meaning |
|------|-------------|---------|
| **Unlit** | `EFxLightMode::Unlit` | Color is literal. Self-lit/glowing effects — explosions, sparks, glows, most magic. Default. |
| **LitFlat** | `EFxLightMode::LitFlat` | `rgb *= (ambient + max(0, Ldir.z) * sun_color)`, particle normal treated as world-up. Scene-lit effects — blood, debris, lit smoke. |

---

## 5. Depth modes

| term | engine enum | meaning |
|------|-------------|---------|
| **TestNoWrite** | `EFxDepthMode::TestNoWrite` | Depth-test on, no depth write. Default for transparent effects. |
| **TestWrite** | `EFxDepthMode::TestWrite` | Depth-test + write. Alpha-tested / mostly-solid (decals, sliced impostors). |
| **None** | `EFxDepthMode::None` | No test, no write. Always-on-top overlays (HUD-attached fx, locators). |

---

## 6. Animation & timing

| term | meaning |
|------|---------|
| **sim tick / 24Hz gate** | Retail simulation runs at 24Hz. Animators advance per sim tick, not per render frame. Pre-release math is often ungated (runs at render rate) — the port MUST gate to 24Hz (`kSimTickMs = 1000/24`) or speeds/cadence scale with framerate. |
| **age** | Ticks (or seconds) since a particle/effect spawned. |
| **age01** | Normalized age, `age / lifetime`, in `[0,1]`. Drives curves. |
| **lifetime** | Total ticks/seconds a particle/effect lives before it's killed. |
| **envelope / curve** | A value's trajectory over `age01` — color curve, alpha curve, scale curve. State the start, end, and easing (linear / ease-in / triangle / etc.). |
| **triangle-wave** | Ramp up then down (e.g. L02 halo scale pulse). Specify peak position. |
| **flipbook / atlas frame** | Sprite-sheet animation. A texture holds N frames; the effect advances a frame index over time. Record frame count, cell dims, advance rate, wrap, per-instance offset. Note: some Revenant assets use `framehtexs[]` (per-frame engine texture handles in one slot), NOT a UV sub-rect atlas. |
| **phase** | A named stage of a multi-stage effect's state machine (e.g. teleporter Init → Out → Move → In). Record duration + transition condition per phase. |

### Trigger semantics
| term | meaning |
|------|---------|
| **one-shot** | Fires once, plays out, dies (blood splat, hit flash). |
| **fixed-duration** | Plays for a set time then dies (cast burst). |
| **continuous** | Emits steadily while active (mist, torch wisp, ambient fire). |
| **looping** | Repeats its cycle indefinitely (rotating teleport cylinder until killed). |
| **burst** | N particles spawned in one instant (vs. a steady rate). |
| **spawn rate** | Particles per tick/second for continuous emitters. |
| **sub-emit** | An effect spawning a child effect (H04 drip → H03 ripple on landing). |

---

## 7. Pipelines

| code | name | what it draws |
|------|------|---------------|
| **FB** | Flipbook Billboard | textured billboard quads, optionally atlas-animated |
| **PE** | Particle Emitter | `TParticleBucket` of particles via the expression VM |
| **SR** | Strip / Ribbon | connected quad strip along a path (lightning, sword trail) |
| **LS** | Light Source | dynamic point light contribution |
| **IM** | I3D Mesh | a real 3D mesh component (teleporter cylinders) — drawn via `SubmitHelperMesh` |
| **VO** | Volumetric | deferred volumetric (future) |
| **CX** | Composite | bespoke combination of the above |

---

## 8. Test rig categories

| term | meaning | typical BG |
|------|---------|------------|
| **Standalone** | spawns at a fixed point, no character (torch, mist, ripple) | Dungeon / Black / LtGray |
| **Character-attached** | follows a bone / root of the rig character (hand glow, aura) | Forest / Dungeon |
| **Character+weapon+attack** | tied to weapon bone during an attack anim (sword swipe) | Forest (sparks read) |
| **Projectile** | travels from source along a vector (missile, fireball) | Dungeon |
| **Spell-cast** | plays at a target/ground point on cast (teleport, summon) | Dungeon |
| **Environment** | ambient world placement (area fire, fog) | matching area BG |

See [../AGENT_GUIDE.md](../AGENT_GUIDE.md) §4.2.1.7 for which background exposes
which diagnostic (Black = additive check, LtGray = alpha check, Dungeon =
fire/floor-parallel, Forest = bright outdoor).

---

## 9. Diagrams (use them — see FORENSICS_PROTOCOL §diagrams)

Forensics docs should include ASCII diagrams wherever spatial or temporal
behavior is clearer drawn than described. Conventions:

**Spatial (emit shape / orientation)** — show the world axes and the effect's
extent. Mark the emit origin, spread, and quad facing.

```
   wz (up)
   │      ╱ ScreenAligned quads (face camera)
   │     ╱
   │   ·· ·  · ·   ← 15-quad scatter, ±50wu patch, WorldXY (flat on floor)
   └──────────── wx     emit origin = ground-projected target (z=0)
  ╱
 wy
```

**Temporal (envelope / phases)** — show value vs age01 or a phase timeline.

```
alpha
0.3 ┤●
    │ ╲
    │  ╲____
0.0 ┤       ●────  age01
    0      1.0

phases:  Init │ Out (grow+rise) │ Move │ In (shrink+fade) │ dead
ticks:   0    │ 1..50           │ 51   │ 52..99           │ 100
```

A diagram is not a substitute for the constants table or pseudocode — it's a
fast orientation aid on top of them.
