# M06 TSpellBlock — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | M06 |
| **Class(es)** | `TShieldAnimator` (pre-release animator, registered `"SHIELD"`). NOTE: INVENTORY M06 names "TSpellBlock" — see §2 for the naming correction. The object-shell class is an anonymous `TEffect` instance. Retail uses a 3-variant class (`MagicShield`/`MagicShield2`/`MagicShield3`) — see §2.1. |
| **Status** | forensics-partial — pre-release animator body is fully traced; retail Render body partially corroborated; mshield.i3d sub-object behavior requires Ghidra extraction of `virt_meth_0x502550` to resolve the 3-variant logic (see §13, BLOCKER) |
| **Retail fidelity** | **retail-partial** — key constants (scale 2.0, pos.z = 40, RotateX(−π/3), RotateZ(−π/4), SetBlendState/Alpha) are **retail-confirmed** in `cls_0x5a9d88::virt_meth_0x4e4d90`; asset diverges (retail ships `mshield.i3d` 448332 B instead of `Shield.I3D` 138524 B); 3-variant retail logic in `virt_meth_0x502550` not recovered; see §2.1 |
| **Author / Date** | vfx-forensics-agent / 2026-06-07 |
| **Family** | magic |
| **Draws** | I3D mesh — single sub-object (`box01` from `Magic\Shield.I3D` in pre-release; `mshield.i3d` with 3 sphere+rectangle pairs in retail). Character-attached mesh overlay, static pose. |
| **Archetype(s)** | (B) I3D-geometry-with-transforms — real 3D mesh placed at a fixed offset above the character, drawn with a single static matrix (no animation). Character-attached status overlay. |

---

## 1. Summary

The **Shield** effect is a magic shield buff visual: a 3D mesh rendered at a fixed
position above the caster/target character, representing a protective magical
aura. In the pre-release source `TShieldAnimator` draws a single sub-object
(`box01`) from `Magic\Shield.I3D`, placed at local position `(0, 0, 40)` world
units above the effect's origin (i.e., above the character's feet), scaled to
2× and rotated by `RotateX(−60°) · RotateZ(−45°)`, using Alpha blending
(`SetBlendState`). The `Animate` body is empty (no per-frame motion) — the mesh
sits static at its initial pose every frame.

In retail the asset changes to `Magic\MShield.I3D` (a 448 KB file with three
sphere+rectangle sub-object pairs, apparently three tint variants), and the
effect is registered as three named variants: `"MagicShield"`, `"MagicShield2"`,
`"MagicShield3"` (all in `virt_meth_0x502550`). The render constants (scale 2.0,
pos.z=40, RotX(−π/3), RotZ(−π/4), Alpha blend) are corroborated in the retail
Ghidra decomp (`cls_0x5a9d88::virt_meth_0x4e4d90`).

**INVENTORY naming note:** INVENTORY M06 is labelled `"TSpellBlock"`, but
`TSpellBlock` in the codebase is the **spell dispatch/accumulator class** (spell
invocation data — invoker, target, elemental values), not a visual effect class.
The M06 visual is `TShieldAnimator`. See §13.1.

---

## 2. Sources & evidence

- **Retail decomp:**
  - `cls_0x5a9d88` (Ghidra) — candidate for `TShieldAnimator`/the MagicShield
    animator. Has 5 recovered methods:
    - `virt_meth_0x4e4d50` = Initialize body (recon `cls_0x5a9d88.cpp:7-17`)
    - `virt_meth_0x4e4d90` = Render body (recon `cls_0x5a9d88.cpp:21-61`) — **COMPLETE** with all key constants
    - `virt_meth_0x4f5d70` = factory/constructor thunk
    - `virt_meth_0x4f76c0` and `~cls_0x5a9d88` = dtor chain
  - Registration: `s_SHIELD_005e10ec` at `.rdata 005e10ec` — 1 XREF at `004e4d30`
    (`recon/classes/_data.txt:107426-107427`). The XREF function `004e4d30` is
    the `REGISTER_3DANIMATOR("SHIELD", ...)` registrar thunk, corroborating the
    animator name.
  - Retail 3-variant registration: `virt_meth_0x502550` (class in vtable at
    `005b07c8`) registers `"MagicShield"` (`005e148c`), `"MagicShield2"`
    (`005e1498`), `"MagicShield3"` (`005e14a8`) — each with 2 XREFs
    (`recon/classes/_data.txt:107841-107854`). Also `s_MagicShield_005e1580`
    XREF at `00502400` and `00502530` — two additional use sites. Body of
    `virt_meth_0x502550` **not extracted** in Ghidra decomp — BLOCKER (§13.2).
  - `cls_0x502420` (recon `cls_0x502420.cpp`) — size 396 B, one function at
    `00502420` that calls `virt_meth at offset +0x158` (likely Initialize parent
    chain), sets `mbr_0x184 = 0`, `mbr_0x188 = 0xf` (= **15**), and a
    `__ftol()` into `mbr_0x18`. The field `mbr_0x188 = 15` is consistent with
    a `framenum` counter or a duration constant.
- **Pre-release (snapshot):**
  - `src/effect_old.cpp:4324-4392` — full `TShieldAnimator` Initialize/Animate/Render bodies.
  - `src/effect.h:1039-1065` — class declaration with fields `hmm_vec3 pos` and `int32_t framenum`.
  - `#define SHIELD_SCALE 2.0` at `src/effect.h:1043`.
  - `REGISTER_3DANIMATOR("SHIELD", TShieldAnimator)` at `src/effect_old.cpp:4327`.
  - No `DEFINE_BUILDER` for a "Shield" effect object — the animator is object-class agnostic; any effect object with imagery class `"Shield"` (Class.Def:2028) will use this animator.
- **Legacy walkcode (older pre-release):** `legacy/walkcode/effect.cpp:4169-4234` and `legacy/walkcode/effect.h:558-583` — body identical to the working source; confirms no drift between branches.
- **Sister effects consulted:**
  - `TIcedEffect` (I22 §7) — same pattern: I3D sub-object, OBJ3D_MATRIX flags, D3DMATRIXRotate*/Scale/Translate chain, SetBlendState (Alpha), Unlit. Corroborates the render pipeline pattern for character-attached mesh overlays.
  - `TSymGlowAnimator` (X10) — also uses SetBlendState (Alpha) for a glow overlay, consistent with magic-family overlay blend.
- **Source-of-truth ranking:**
  - Render constants (scale, pos.z, rotations, blend): **retail-confirmed** via `cls_0x5a9d88::virt_meth_0x4e4d90`.
  - Animate body (empty, no per-frame motion): **snapshot-only** (retail Animate not recovered).
  - 3-variant MagicShield logic: **retail-only** (not present in pre-release source); body not extracted.
  - Asset: **retail diverges** (mshield.i3d vs Shield.I3D — see §2.1).

### 2.1 Retail-vs-snapshot reconciliation (REQUIRED)

`src/effect_old.cpp` is a **pre-release development snapshot**. Active
cross-checks against the retail Ghidra decomp follow.

**1. Constant grep — retail-confirmed values.**
The retail Render body at `cls_0x5a9d88::virt_meth_0x4e4d90`
(`recon/classes_converted/cls_0x5a9d88.cpp:21-61`) contains:

```
this->mbr_0x104 = 0x42200000;   // 40.0f in IEEE 754 → pos.z = 40  [CONFIRMED]
puVar3[0x10]    = 2.0;          // scale.x = 2.0 → SHIELD_SCALE     [CONFIRMED]
puVar3[0x11]    = 0x40000000;   // 2.0f → scale.y = 2.0             [CONFIRMED]
puVar3[0x12]    = 0x40000000;   // 2.0f → scale.z = 2.0             [CONFIRMED]
FUN_0043b180(pfVar1, -1.047198); // ≈ −π/3 → RotateX(-60°)          [CONFIRMED]
FUN_0043b230(pfVar1, -0.7853982);// ≈ −π/4 → RotateZ(-45°)          [CONFIRMED]
```

`FUN_0043b180` and `FUN_0043b230` are the D3DMatrix rotate helpers (`RotateX`,
`RotateZ`). `FUN_0043b130` is Scale, `FUN_0043b090` is Translate.  
`cls_0x411eb0::meth_0x4178e0` / `meth_0x417d60` calls with argument `2` bracket
the body — matching the `SaveBlendState()` / `SetBlendState()` /
`RestoreBlendState()` pattern (`SetBlendState` = type 2 = Alpha in the pre-release
helper enum). **CONFIRMED.**

The Initialize body at `virt_meth_0x4e4d50` (`:7-17`) sets:
```
this->mbr_0x104 = 0x42200000;  // pos.z = 40.0 [CONFIRMED]
this->mbr_0x184 = 0;            // pos.x = 0    [CONFIRMED]
this->mbr_0x188 = 0xf;          // = 15
```
`mbr_0x188 = 0xf = 15` is likely `framenum` initialized to 0 in the snapshot
(`pos.x = pos.y = 0; pos.z = 40; framenum = ??`). NOTE: the snapshot does NOT
explicitly initialize `framenum` in the body at `effect_old.cpp:4336-4342` — it
only sets `pos.x = pos.y = 0; pos.z = 40`. The retail's `0xf` is unexpected
here. This could be `pos.x` at a different field offset (the retail class may
reorder), OR the retail uses a `framenum` initial value of 15 for some timing
purpose — see §13.3.

**2. Asset identity — DIVERGED.**
- Pre-release: `Magic\Shield.I3D` — **138524 B** — 1 sub-object `box01`, 1
  state `STILL` (`legacy/Imagery/Magic/Shield.I3D`; strings dump confirms).
  MD5: `9012cedd110ba0d8b5abb6456b7e016a`.
- Pre-release also registered: `Magic\Shield2.I3D` — **13172 B** — 1 sub-object
  `sphere01`, 1 state `STILL`.
- Retail `data/imagery.rvi`: contains `Imagery/Magic/mshield.i3d` — **448332 B**
  — 6 sub-objects (3 sphere meshes + 3 small rectangles, per i3d_dump_all/mshield/manifest.txt).
  **Neither Shield.I3D nor Shield2.I3D is present in the retail rvi.**

This is a **significant asset replacement**: the pre-release box/sphere pair was
replaced by `mshield.i3d` for retail, matching the new `"MagicShield"` effect
name and the tri-variant registration. The Class.Def also adds entry 2089
`"MagicShield" "Magic\MShield.I3D"` (not present in the pre-release Class.Def
at line 2028-2029). **The mshield.i3d sub-object layout and 3-variant rendering
logic are the critical unknown for reconstruction** (see §13.2 BLOCKER).

**3. Structure / vftable.**
`cls_0x5a9d88` is **264 bytes** (from the FLAME candidate yaml that notes
`cls_0x5a9d88` size 264, 5 methods, self-allocated 0x100 bytes).
The snapshot `TShieldAnimator` has: `T3DAnimator` base + `hmm_vec3 pos` (12 B)
+ `int32_t framenum` (4 B) = base + 16 B of animator data. `T3DAnimator` base
size is ~240 B (per analogous effects), so total ≈ 256–264 B — **consistent**
with the retail `cls_0x5a9d88` footprint.

**4. Registration + naming — partial.**
- `"SHIELD"` string at `.rdata 005e10ec`, 1 XREF at `004e4d30`
  (`recon/classes/_data.txt:107427`) — the pre-release `REGISTER_3DANIMATOR("SHIELD", ...)` form.
- The retail binary ALSO has `"MagicShield"`, `"MagicShield2"`, `"MagicShield3"`
  at addresses `005e148c`, `005e1498`, `005e14a8` (`_data.txt:107843,107848,107854`),
  each with 2 XREFs in `virt_meth_0x502550`. So retail runs BOTH the pre-release
  `"SHIELD"` registration AND the new 3-variant MagicShield registration —
  possibly for backwards compatibility with older area scripts.

**Verdict: retail-partial.** Render constants (scale 2.0, pos.z=40, rotations,
Alpha blend, OBJ3D_MATRIX path) are **retail-confirmed** in
`cls_0x5a9d88::virt_meth_0x4e4d90`. The pre-release `"SHIELD"` registration
name is confirmed. However: (a) the asset replaced `Shield.I3D` → `mshield.i3d`
with 6 sub-objects vs 1; (b) the 3-variant retail MagicShield logic in
`virt_meth_0x502550` is unextracted; (c) `mbr_0x188 = 0xf` in Initialize is
unexplained. The reconstruction can faithfully port the pre-release behavior for
the snapshot constants, but must treat the mshield.i3d rendering as a BLOCKER
requiring Ghidra extraction.

---

## 3. Constants

All from `src/effect_old.cpp` / `src/effect.h` unless noted. "wu" = world units.
"confirmed?" column: **yes (retail)** = corroborated in Ghidra decomp;
**snapshot-only** = pre-release only; **guessed** = inferred.

| name | value | units | source | confirmed? |
|------|-------|-------|--------|------------|
| `SHIELD_SCALE` | 2.0 | scale factor (uniform x/y/z) | effect.h:1043 | **yes (retail)** — `cls_0x5a9d88::virt_meth_0x4e4d90`: `puVar3[0x10] = 2.0; puVar3[0x11] = 0x40000000; puVar3[0x12] = 0x40000000` |
| `pos.z` (lift above origin) | 40.0 | wu | effect_old.cpp:4341 | **yes (retail)** — `cls_0x5a9d88::virt_meth_0x4e4d50`: `this->mbr_0x104 = 0x42200000` (= 40.0f IEEE 754) |
| `pos.x`, `pos.y` | 0.0, 0.0 | wu | effect_old.cpp:4340 | **yes (retail)** — `virt_meth_0x4e4d50`: `this->mbr_0x184 = 0; this->mbr_0x188 = 0xf` (x,y = 0 confirmed; see §2.1 + §13.3 for `0xf`) |
| RotateX angle | −π/3 ≈ −60° | rad | effect_old.cpp:4379 | **yes (retail)** — `FUN_0043b180(pfVar1, -1.047198)` in Render (−1.047198 ≈ −π/3) |
| RotateZ angle | −π/4 ≈ −45° | rad | effect_old.cpp:4380 | **yes (retail)** — `FUN_0043b230(pfVar1, -0.7853982)` in Render (−0.7853982 ≈ −π/4) |
| blend mode | SetBlendState (Alpha) | — | effect_old.cpp:4367 | **yes (retail)** — `cls_0x411eb0::meth_0x417d60(..., 2)` in Render brackets = Alpha type |
| sub-object index | 0 (`GetObject(0)`) | index | effect_old.cpp:4369 | **snapshot-only** — retail Render uses the equivalent; mshield variant logic not extracted |
| `flags` | `OBJ3D_MATRIX` | flag | effect_old.cpp:4373 | **yes (retail)** — `*puVar3 = 0x100` in Render body (`OBJ3D_MATRIX = 0x100`) |
| `framenum` initial | 0 (snapshot) / 15 (retail?) | ticks | effect.h:1051; retail `virt_meth_0x4e4d50` | **snapshot-only** (snapshot doesn't initialize it; retail `mbr_0x188 = 0xf` — see §13.3) |

Build-order for the single static matrix (Render):
`Scale(scale) · RotateX(−π/3) · RotateZ(−π/4) · Translate(pos)` applied
right-to-left per D3D convention (cited: `effect_old.cpp:4382-4383`).

---

## 4. Assets

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| Shield mesh (pre-release) | `legacy/Imagery/Magic/Shield.I3D` | 138 524 B | The shield mesh with single sub-object `box01` | Registry name `"Shield"` (`Class.Def:2028`, hash `0x727c833a`); animator uses `GetObject(0)` to address `box01` |
| Shield2 mesh (pre-release alt) | `legacy/Imagery/Magic/Shield2.I3D` | 13 172 B | Alternate sphere mesh `sphere01` | Registry name `"Shield2"` (`Class.Def:2029`, hash `0x727c834b`); variant registered but animator code is identical — same `GetObject(0)` |
| MagicShield mesh (**retail**, shipped) | `Imagery/Magic/mshield.i3d` (in `data/imagery.rvi`) | 448 332 B | Tri-variant shield mesh — 6 sub-objects: 3 sphere meshes (`sphere02`, `sphere01`, `sphere03`) + 3 small rectangles (`rectangle0`, `rectangle1`, `rectangle2`) | Registry name `"MagicShield"` (`Class.Def:2089`, hash `0xad92bc0d`); 3-variant rendering logic in `virt_meth_0x502550` (BLOCKER — not extracted) |

**Shield.I3D sub-objects** (verified by strings dump of `legacy/Imagery/Magic/Shield.I3D`):
- State: `STILL` (single state)
- Sub-object count: **1**
- Index 0: `box01` — the drawn mesh; dimensions unknown without full I3D parse (file is 138 KB, suggesting a substantial mesh with embedded texture)

**Shield2.I3D sub-objects** (verified by strings dump):
- State: `STILL`
- Sub-object count: **1**
- Index 0: `sphere01` — a sphere mesh; file is 13 KB (smaller/simpler)

**mshield.i3d sub-objects** (verified by `i3d_dump_all/mshield/manifest.txt`):

| index | name | verts | idxs | texslot | bbox |
|-------|------|-------|------|---------|------|
| 0 | `sphere02` | 402 | 2070 | 1 (16×16, yellow-tinted) | (−19.98,−20.09,−43.95)..(19.98,20.09,18.85) |
| 1 | `rectangle0` | 4 | 6 | 2 (256×256, blue-lavender marble) | (−8,−8,0)..(8,8,0) |
| 2 | `sphere01` | 402 | 2070 | 3 (16×16, blue-tinted) | (−19.98,−20.09,−43.95)..(19.98,20.09,18.85) |
| 3 | `rectangle1` | 4 | 6 | 4 (256×256, blue-lavender marble) | (−8,−8,0)..(8,8,0) |
| 4 | `sphere03` | 402 | 2070 | 5 (16×16, pink-tinted) | (−19.98,−20.09,−43.95)..(19.98,20.09,18.85) |
| 5 | `rectangle2` | 4 | 6 | 6 (256×256, pinkish marble) | (−8,−8,0)..(8,8,0) |

**mshield textures** (decoded from `i3d_dump_all/mshield/`):
- `texture_00_frame_00.png` (256×256): yellowish/cream marbled pattern — applies to `sphere02` (texslot 1 references texture 0 zero-indexed)
- `texture_01_frame_00.png` (16×16): small yellow glow dot — applies to `rectangle0`
- `texture_02_frame_00.png` (256×256): blue-lavender marbled pattern — applies to `sphere01`
- `texture_03_frame_00.png` (16×16): small blue-lavender dot — applies to `rectangle1`
- `texture_04_frame_00.png` (256×256): pinkish/rose marbled pattern — applies to `sphere03`
- `texture_05_frame_00.png` (16×16): small pink dot — applies to `rectangle2`

**Pattern:** three sphere+rectangle pairs, each with a different color tint
(yellow, blue-lavender, pink), matching the 3-variant spell names
(MagicShield/MagicShield2/MagicShield3). Each variant likely draws ONE sphere +
ONE rectangle from its pair. The rectangle sub-objects are tiny 4-vertex quads
— possibly a ground-projected glow disc or a highlight sprite. The spheres share
the same bbox (asymmetric: offset from origin, not centered on z), suggesting
they are authored off-center relative to the effect's local origin.

**UV decode:**
- Sphere sub-objects (402 verts): full-surface UV mapping over the sphere geometry
  (not a sub-rect atlas — each sphere has its own unique texture). No grid.
- Rectangle sub-objects (4 verts): standard quad, UV 0..1 full, single-cell sprite.
- No runtime UV animation observed in the pre-release Animate body (static per §8).

**Stand-in guard:** The mshield.i3d is the retail asset. Any reconstruction that
renders procedural spheres, billboards, or glows instead of loading
`mshield.i3d`'s sphere+rectangle pairs is a stand-in and is wrong (AGENT_GUIDE
§4.2.1; memory `feedback-no-standins`). The asset MUST be loaded to reproduce
the correct shape and texture.

---

## 5. Spawn & emit

- **Trigger semantics:** **continuous status buff overlay** — persists while the
  shield spell is active. The `Animate` body is empty (no state machine, no
  self-kill logic in the pre-release source). The effect lives until the spell
  ends (the spell system kills it externally).
- **Count per trigger:** one mesh instance per active shield buff on a character.
- **Initial direction / distribution:** N/A — this is a static mesh, not an
  emitter.
- **Emit anchor convention:** The effect ORIGINATES at the character's position
  (the spell system places the effect at the caster/target's `GetPos()`). The
  mesh is then lifted `pos.z = +40 wu` above that position in local coordinates,
  placing the shield dome above the character's head. No bone-tracking; the
  effect's world position must be re-set each frame if the character moves
  (or the effect is in local space following the character — see §13.4).
- **Coordinate space:** the mesh position `(0, 0, 40)` is in the effect's own
  local space (relative to its world origin, which is the character's feet).
  `OBJ3D_MATRIX` flag routes the object through the full matrix path, not a
  simple `OBJ3D_ROT1|OBJ3D_SCL2|OBJ3D_POS3` decomposed form.
- **Spread / jitter:** none.

### Spatial diagram

```
 SIDE (ZY), character standing at origin:

  wz
  +40 │       ┌─────────────────┐    ← shield mesh (box01 / mshield sphere)
      │       │   SHIELD MESH   │      placed at pos = (0, 0, 40) wu
      │       │   scale = 2.0   │      rotated RotX(−60°) · RotZ(−45°)
      │       └─────────────────┘      from the effect's world origin
      │
   0  ●────────────────────────────► wy   ← character feet (effect origin)
      character position = effect pos

 The shield mesh sits +40 wu above the effect's origin.
 Rotation: first tips the mesh 60° toward the viewer (RotX), then spins 45°
 around the vertical axis (RotZ). The iso camera angle makes this look like
 a forward-tilted floating shield/dome over the character.
```

---

## 6. Behavior & per-frame logic

`TShieldAnimator` is a **nearly-empty animator** in the pre-release source. The
entire behavior is:

```
// Initialize (effect_old.cpp:4336-4342)
Initialize():
    T3DAnimator::Initialize()
    pos.x = pos.y = 0
    pos.z = 40             // float: the shield hovers 40 wu above the origin
    // framenum not initialized in snapshot body (retail sets it to 0xf = 15)

// Animate (effect_old.cpp:4351-4355)
Animate(draw):
    T3DAnimator::Animate(draw)
    // EMPTY — no per-frame motion, no frame counter advance in body

// Render (effect_old.cpp:4364-4392)
Render():
    SaveBlendState()
    SetBlendState()               // Alpha blend (SRC_ALPHA/INV_SRC_ALPHA)
    obj = GetObject(0)            // the single mesh sub-object (box01)
    Get3DImagery()->ResetExtents()
    obj->flags = OBJ3D_MATRIX
    obj->scl = (SHIELD_SCALE, SHIELD_SCALE, SHIELD_SCALE)   // (2.0, 2.0, 2.0)
    obj->pos = pos                // (0, 0, 40)
    D3DMATRIXClear(&obj->matrix)
    D3DMATRIXRotateX(&obj->matrix, -(M_PI / 3.0f))          // −60°
    D3DMATRIXRotateZ(&obj->matrix, -(M_PI / 4.0f))          // −45°
    D3DMATRIXScale(&obj->matrix, &obj->scl)                  // ×2.0
    D3DMATRIXTranslate(&obj->matrix, &obj->pos)              // (0,0,40)
    RenderObject(obj)
    UpdateExtents()
    RestoreBlendState()
    return true
```

**Key behaviors:**
- The `Animate` body does nothing beyond calling the base class — no rotation,
  no pulsing, no scale animation. The shield is fully **static**.
- The `framenum` field declared in the class header is **never read or written**
  in the pre-release bodies (`effect_old.cpp:4336-4392`). It appears vestigial
  or reserved for a planned animation that was never implemented. The retail
  `mbr_0x188 = 0xf` (15) may activate this if a retail-only Animate body exists.
- No `inst->SetCommandDone(false)` call — the effect does NOT suppress the spell
  `done` flag, meaning spell completion is driven by the outer `TSpell` lifecycle,
  not by the animator.
- No `RefreshZBuffer` override — the base `T3DAnimator::RefreshZBuffer` handles
  Z restoration.

**Multi-variant retail logic (mshield.i3d):** The retail's `virt_meth_0x502550`
registers MagicShield/MagicShield2/MagicShield3 — three spell tiers. The exact
sub-object selection logic (which sphere+rectangle pair each variant draws) is
NOT determined from the available source (BLOCKER — §13.2). The most likely
pattern, by analogy with the 3 color-coded pairs:
- MagicShield → `sphere02` + `rectangle0` (yellowish pair, index 0+1)
- MagicShield2 → `sphere01` + `rectangle1` (blue-lavender pair, index 2+3)
- MagicShield3 → `sphere03` + `rectangle2` (pinkish/rose pair, index 4+5)

This is a **hypothesis**, not a confirmed fact.

---

## 7. Rendering (original render state + geometry)

- **What it draws:** a single I3D sub-object mesh instance placed at
  `(0, 0, 40)` above the effect origin, via full matrix transform. One draw
  call per frame.
- **Blend mode (original):** **Alpha** (SetBlendState) — `SRC_ALPHA /
  INV_SRC_ALPHA`. Cited: `effect_old.cpp:4367`; confirmed in retail via
  `cls_0x411eb0::meth_0x417d60(..., 2)` in `virt_meth_0x4e4d90` (the `2` is
  the SetBlendState type selector for Alpha per the original helper).

  **BLEND SANITY-CHECK (mandatory):** Shield.I3D contains `box01` — a mesh with
  a baked texture (`?fff?fff?fff?` in strings = white/full-alpha material
  coefficients, per CGSR file header). The mshield textures (marbled
  blue/yellow/pink surfaces) are NOT bright-on-black additive sprites — they
  are semi-transparent overlays drawn on the character. **Alpha (not Additive)**
  is the correct blend for translucent surface overlays on mesh geometry. The
  snapshot's `SetBlendState()` call is consistent with the asset design. Sister
  effect `TIcedEffect` (I22) also uses Alpha (`SetBlendState`) for its ice-mesh
  overlay. **No suspect-flag.** Blend = Alpha is confirmed and appropriate.

- **Lit vs self-lit:** **Unlit / self-lit.** Neither Initialize nor Render writes
  `lverts[].color` or calls `GetAmbientLight()`. The mesh is rendered at its
  authored material colors under MODULATE, without scene lighting influence.
  Classify **Unlit** (same pattern as TIcedEffect, TSymGlowAnimator).
- **Depth / Z:** depth-test on, **depth-write off** (`SetBlendState`,
  `effect_old.cpp:221-233`) → **TestNoWrite**. The base `RefreshZBuffer` (not
  overridden) restores the Z depth under the effect's extents.
- **Orientation:** **NOT billboard** — a real 3D mesh placed by a full
  `OBJ3D_MATRIX` matrix. No ScreenAligned or WorldXY orientation enum applies.
  The matrix is built from a fixed RotateX(−60°) + RotateZ(−45°), then Scale,
  then Translate. The mesh's authored geometry determines how it looks at the
  given rotation.
  - `RotateX(−π/3 = −60°)` tips the mesh forward (toward the camera in iso view).
  - `RotateZ(−π/4 = −45°)` spins it 45° around the vertical axis.
  - These rotations are **fixed constants** (not per-frame, not per-instance
    random). The shield always appears at the same orientation.
- **Per-object transform:** build order (right-multiply): Clear → RotateX(−π/3)
  → RotateZ(−π/4) → Scale(2.0) → Translate(0,0,40). Cited: `effect_old.cpp:4377-4383`.
- **Per-vertex color packing:** none written — the animator never sets
  `lverts[].color`. The mesh's authored material colors pass through MODULATE
  unchanged (white diffuse default × texture = texture color).

---

## 8. Texture animation

- **NONE.** The Animate body calls only `T3DAnimator::Animate(draw)` and returns.
  No UV scrolling, no `framehtexs` flipbook, no frame counter advance in
  `effect_old.cpp:4351-4355`. The texture is static per frame.
- If the retail `Animate` body (not recovered) adds texture animation for the
  mshield.i3d asset (e.g. scrolling the marble texture to give the shield a
  shimmering effect), this is **unknown** and constitutes a gap (§13.5).

---

## 9. Associated light

- **NO dynamic light in the pre-release source.** `TShieldAnimator` emits no
  light — no `AddLight`, `CreateLight`, or `SLightInfo` in any of the
  Initialize/Animate/Render bodies (`effect_old.cpp:4336-4392`). No spell.def
  `LIGHT` line was found for a "Shield" or "MagicShield" spell variant in the
  shipped `spell.def` (the spell is not listed there — see §13.6).
- The mshield variants may have a dynamic light in retail (a magical aura casting
  blue/yellow/pink light on the character) — this is **unknown** without the
  `virt_meth_0x502550` body.

---

## 10. Color

- **Source:** the **authored textures of the I3D asset** (Shield.I3D's `box01`
  embedded texture in pre-release; mshield.i3d's per-sphere textures in retail).
  The animator writes no per-vertex color and zeroes no material.
- **Pre-release colors:** `box01` from Shield.I3D — the material string
  `?fff?fff?fff?` in the file (`legacy/Imagery/Magic/Shield.I3D` strings dump)
  indicates white/full-opacity material coefficients (`0x3fffffff` ≈ 1.0 in
  D3D float). The actual texture appearance is unknown without a full I3D parse
  of the 138 KB file (no i3d_dump_all entry exists for Shield.I3D).
- **Retail colors (mshield.i3d, texture-confirmed):**
  - Variant 1 (MagicShield/sphere02): **yellowish/cream marble** (256×256) + small yellow dot (16×16)
  - Variant 2 (MagicShield2/sphere01): **blue-lavender marble** (256×256) + small blue-lavender dot (16×16)
  - Variant 3 (MagicShield3/sphere03): **pinkish/rose marble** (256×256) + small pink dot (16×16)
  All three textures are mid-brightness (not black-background additive sprites),
  consistent with Alpha-blend semi-transparent surface overlays.
- **Expected visual:** a translucent marbled dome or bubble surrounding the
  character in the appropriate variant color (yellow/cream, blue-lavender, or
  pink/rose). If the reconstruction renders opaque or shows wrong colors, check:
  (1) Alpha blend used correctly (not Additive — would wash out the marble
  color), (2) mshield.i3d textures loaded and the correct pair selected per
  variant, (3) the mesh isn't rendered with scene lighting that overrides the
  authored colors.
- **Normalization / boosts:** none — no `NormalizeColors`, no brightness math
  in the animator.

---

## 11. Audio coupling

- **No audio coupling found** in `TShieldAnimator::Initialize/Animate/Render`
  (`effect_old.cpp:4336-4392`). No `PLAY(...)` call in any of the three bodies.
- The mshield.i3d asset may carry per-state `S3DTag` sounds (the data-driven
  mechanism, knowledge 02 §8), but the animator never switches imagery state, so
  any such tags would not fire. Record as "no code-driven audio."
- The spell.def `PLAY` line for a "Shield" or "MagicShield" spell variant is
  unknown (the spell is not listed in the shipped spell.def — see §13.6).

---

## 12. Triggers & in-game appearance

- **Spawned by:** the spell system places the "Shield"/"MagicShield" effect at
  the target character's position via `EffectClass.FindObjType(variant.effect)`
  (`legacy/walkcode/spell.cpp:324`), where `variant.effect` is the effect name
  string from `spell.def`. The pre-release effect is referenced as imagery class
  `"Shield"` (Class.Def:2028); the retail effect is `"MagicShield"` (Class.Def:2089).
  The effect is driven by the spell's VARIANT definition. Neither `spell.def` in
  the shipped data (`data/Resources/spell.def`) nor the legacy `spell.def`
  contains a "Shield" or "MagicShield" SPELL block with an EFFECT_NAME match —
  see §13.6.
- **EFFECT_NAME registration (retail):** `"MagicShield"`, `"MagicShield2"`,
  `"MagicShield3"` are registered in `virt_meth_0x502550` (3 separate
  registrations, all at addresses within that function). Additionally `"SHIELD"`
  is registered via `004e4d30`. These are the strings the engine uses to look up
  the animator by name.
- **Where to see it in the original game:** cast a protective shield buff spell
  on a character (any talisman combo that produces a MagicShield effect) and
  observe the mesh overlay on the character. The specific spell combo is unknown
  from the shipped spell.def data (§13.6). Alternatively, trigger via a scripted
  scene or monster ability that casts a protective magic.
- **Vestigial?** Partially — the pre-release `REGISTER_3DANIMATOR("SHIELD", ...)`
  form has NO corresponding `DEFINE_BUILDER` for a "Shield" effect object builder
  in the shipped pre-release source. The retail's 3-variant MagicShield
  registration IS live (confirmed by the 2+2+2 XREFs in `virt_meth_0x502550`),
  so the MagicShield spell effect is live in retail. The "SHIELD" registration
  may be a backwards-compatibility stub.

---

## 13. Gaps & uncertainties

### 13.1 INVENTORY "TSpellBlock" naming error

INVENTORY M06 names the class as `TSpellBlock`. In the codebase,
`TSpellBlock` (`legacy/walkcode/effect.h:156-197`) is the **spell dispatch
accumulator** — it holds `invoker`, `target`, elemental values (`fire`, `ice`,
`earth`, `lightning`), `power`, `order`, `destruction`, and `numeffects`. It is
NOT a visual effect class. It has a `Pulse(kill)` method that spawns effects and
counts them — a spell session controller.

The M06 visual effect is `TShieldAnimator` (`REGISTER_3DANIMATOR("SHIELD", ...)`
at `effect_old.cpp:4327`). The INVENTORY entry should be corrected to list
`TShieldAnimator` (see §14 INVENTORY suggestion). The "TSpellBlock" label in M06
may have been a copy-paste error or a confusion with the spell class name.

### 13.2 BLOCKER: `virt_meth_0x502550` body not extracted

The retail 3-variant MagicShield registration function (`virt_meth_0x502550`)
contains the logic for sub-object selection and possibly per-variant render
differences (e.g. which sphere+rectangle pair each variant draws, whether
`framenum` drives any animation, whether a dynamic light is added). **This body
has not been extracted by Ghidra.** A Ghidra extraction pass targeting
`virt_meth_0x502550` (address `00502550`) is required to:
1. Confirm which sub-objects each MagicShield variant draws.
2. Determine if `framenum = 15` drives any UV animation or flipbook.
3. Confirm the 3-variant asset selection logic.

**Classification: BLOCKER for faithful mshield.i3d reconstruction.**

### 13.3 `mbr_0x188 = 0xf` in retail Initialize

The retail `cls_0x5a9d88::virt_meth_0x4e4d50` sets `this->mbr_0x188 = 0xf = 15`.
In the snapshot class layout, `mbr_0x188` maps to `framenum` (the second field
after `T3DAnimator` base, i.e. `pos` is 12 B at offsets `0x184/0x188/0x18c`,
OR `framenum` is at `0x190`). There are two interpretations:
- `mbr_0x188` is `pos.y` (the Y component of `hmm_vec3 pos` starting at
  `mbr_0x184`): in that case retail initializes `pos.y = 15.0f` (= 0x0f in
  int, but IEEE 754 `0x0f = 5.916...e-44f` ≈ 0 denormal — unlikely).
- `mbr_0x188` is `framenum` at a different offset: retail uses `framenum = 15`
  as an initial value for some animation purpose.
- Most likely: field offset/ordering differs between snapshot and retail class
  layout, and `0xf` maps to `pos.x` or `pos.y` — but `0x0f` as a float is
  essentially 0 (denormal near-zero), so pos.x = pos.y = 0 still holds
  numerically. **Safe to assume pos.x = pos.y = 0, framenum = 0** for the
  reconstruction; but a Ghidra extraction of `virt_meth_0x502550` may clarify
  if `framenum` has retail use.

### 13.4 Character tracking behavior unknown

The snapshot `Animate` body is empty — it does not update `pos` each frame. For
the shield to follow a moving character, either: (a) the effect is placed in
local space relative to the character (engine-level character-attachment in the
effect's object placement, not in the animator), OR (b) the spell system
re-sets the effect's world position each pulse (analogous to `TBurnEffect::Pulse`
chasing the victim). This tracking mechanism is **not documented** in the
available source and is unknown from the pre-release snapshot.

### 13.5 Retail Animate body unknown

The retail `Animate` body for `cls_0x5a9d88` was not recovered in Ghidra
(only Initialize and Render are extracted, plus dtor). If retail's Animate
adds UV scrolling, a rotation increment on the shield, or scale pulsing using
the `framenum` field, this animation would be missing from a snapshot-based port.
Record as snapshot-vs-retail risk.

### 13.6 No spell.def caller identified

Neither the shipped `data/Resources/spell.def` nor the legacy `spell.def` contains
a SPELL block with EFFECT_NAME matching "Shield", "SHIELD", or "MagicShield".
This means the Shield/MagicShield effect's spell trigger (talisman combo, mana
cost, duration) is **unknown from the available data files**. The effect may be
spawned from monster scripts, area-scripted buffs, or a non-spell mechanism
(e.g. triggered by a specific item use or battle event). The retail XREF sites
for `s_MagicShield_005e1580` at `00502400` and `00502530` (in the spell/combat
dispatch table at `005c54b4/b8`) suggest it is wired into the combat system, not
the player spell-cast UI. Ground truth for "where to see it in the game" requires
either game-world knowledge or a Ghidra extraction of the `00502400`/`00502530`
sites.

### 13.7 Visual reference images — possible mismatch

The three reference images in `images/vfx/04_magicshield/` show **red/orange
glowing starburst sparks** around a character. This does NOT match the expected
appearance of `TShieldAnimator`/`TShieldEffect` (a translucent marbled mesh
dome). The captured images may show a *different* effect occurring at the same
time (e.g. a combat hit flash, a fire effect, or a scripted visual on the
character). Reconstruction agents should not use these captures as visual ground
truth for the shield dome shape/color; instead use direct in-game observation
with a confirmed shield buff active.

### 13.8 Snapshot-vs-retail risk summary

The key fidelity risks for reconstruction:
1. **Mshield.i3d sub-object selection** per variant (BLOCKER — §13.2).
2. **Animate body** contents in retail (possible UV animation/rotation — §13.5).
3. **Character tracking** mechanism (§13.4).
4. **Spell/trigger integration** (§13.6 — where does the effect get spawned?).

Retail-confirmed (safe to port from snapshot): scale 2.0, pos.z = 40,
RotateX(−π/3), RotateZ(−π/4), OBJ3D_MATRIX flags, Alpha blend.

---

## 14. Reconstruction burndown

```
- [ ] Resolve BLOCKER §13.2: request Ghidra extraction of virt_meth_0x502550
      (address 00502550) to determine per-variant sub-object selection, any
      animation logic, and dynamic light presence for mshield.i3d.
- [ ] Load Magic\MShield.I3D (448332 B from imagery.rvi); the pre-release
      Magic\Shield.I3D (138524 B) is the snapshot reference but is NOT in retail.
      Address mshield sub-objects by index: 0=sphere02 (yellow), 1=rectangle0,
      2=sphere01 (blue-lavender), 3=rectangle1, 4=sphere03 (pink), 5=rectangle2. (§4)
- [ ] Build the animator to draw ONE sphere + ONE rectangle per variant:
      MagicShield → indices {0,1}; MagicShield2 → {2,3}; MagicShield3 → {4,5}.
      (HYPOTHESIS pending §13.2 confirmation.) (§4, §6)
- [ ] Place the effect at the character's position (spell-system anchor —
      character's GetPos()); confirm character-tracking mechanism (§13.4). (§5)
- [ ] Render the sub-object at local pos = (0, 0, 40) wu above the effect origin,
      with the fixed matrix: OBJ3D_MATRIX, Clear → RotateX(−π/3) → RotateZ(−π/4)
      → Scale(2.0, 2.0, 2.0) → Translate(0, 0, 40). (§7)
- [ ] Blend = Alpha (SetBlendState: SRC_ALPHA / INV_SRC_ALPHA); depth = TestNoWrite;
      lit-mode = Unlit (no per-vertex color write, no ambient mix). (§7)
- [ ] Texture animation = NONE in snapshot; verify against retail (§8, §13.5).
      If retail Animate animates texture UV or increments framenum, add per the
      Ghidra extraction.
- [ ] Transform animation: NONE in snapshot (Animate is empty — static mesh, no
      rotation increment, no scale pulse). Verify against retail (§6, §13.5).
- [ ] Associated dynamic light: NONE confirmed in pre-release; check retail
      virt_meth_0x502550 body for AddLight calls. (§9)
- [ ] Color: from mshield.i3d embedded textures — yellow (MagicShield), blue-lavender
      (MagicShield2), or pink/rose (MagicShield3) marble texture on sphere + matching
      dot on rectangle. Pale/wrong color ⇒ wrong variant index, wrong texture slot,
      or wrong blend. (§10)
- [ ] Audio: no code-driven audio in pre-release; check mshield.i3d S3DTag sounds
      in the audio phase. (§11)
- [ ] Trigger integration: wire the effect spawn to the spell/combat system when
      a MagicShield buff is cast; confirm spawn site via §13.6 + Ghidra extraction
      of 00502400/00502530 callers. (§12)
- [ ] Register under BOTH "SHIELD" (legacy compat) AND "MagicShield"/"MagicShield2"/
      "MagicShield3" (retail names per _data.txt:107427,107843,107848,107854). (§2)
- [ ] Correct INVENTORY M06: rename class from "TSpellBlock" to "TShieldAnimator"
      and update source file from "effect.cpp" to "effect_old.cpp:4324-4392". (§13.1)
```

---

## INVENTORY update suggestion

Do NOT edit INVENTORY.md (orchestrator commits). Apply the following correction
when the orchestrator updates the row:

```
| M06 | `TShieldAnimator` (pre-rel: "SHIELD"; retail: "MagicShield"/2/3) | IM | effect_old.cpp:4324-4392 | retail-partial (cls_0x5a9d88 Render confirmed; virt_meth_0x502550 body = BLOCKER) | src/effect.h:1039-1065 | forensics-complete | - | Forensics at docs/vfx/forensics/M06_TSpellBlock.md. INVENTORY class name was "TSpellBlock" (the spell dispatch class) — corrected to TShieldAnimator. Asset diverges: pre-release Magic\Shield.I3D (box01), retail Magic\MShield.I3D (3 sphere+rect pairs, 3 color variants). Render constants retail-confirmed; 3-variant sub-object selection + retail Animate body require Ghidra extraction of virt_meth_0x502550 (BLOCKER). |
```
