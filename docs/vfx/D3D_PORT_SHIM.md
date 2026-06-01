# `d3d::*` Port Shim — usage guide

Lets bespoke effect ports transliterate the original 1998/99 D3D render
bodies near-verbatim. Each D3D-flavored snapshot call has a 1:1 `d3d::`
equivalent that hides the convention conversions our sokol pipeline
requires.

**Header**: `src/d3dport.h` · **Impl**: `src/d3dport.cpp` · **Extracted from**:
`TIceBoltEffect_Bespoke::SubmitWorldMeshes_BESPOKE` (src/effect.cpp:8264-8480),
`TMeteorStormEffect_Bespoke::SubmitWorldMeshes_BESPOKE` (src/effect.cpp:14497-14690).

---

## The four conventions it hides

| Convention | Without the shim you'd have to | With the shim |
|---|---|---|
| Matrix accumulation (D3D row-vector vs our column-vector) | Reverse `M *= S; M *= R; M *= T` to `T * R * S` manually, debug short-fat geometry from wrong order | Just write the D3D ops in snapshot order |
| Rotation sign (`RotateX(+π/2)` vs `-π/2`) | Read the shader, do basis vectors by hand | Shim preserves D3D sign convention |
| Render-queue scope (regular `submit` vs `submit_world`) | Hunt down why mesh is invisible despite no errors | `d3d::RenderObject` documented to be called from `submit_world` |
| Degenerate I3D UVs (cylinder bodies, glow blobs) | Discover via UV-diagnostic log, manually bind WhiteTexture + route material colors | `d3d::RegisterSubMesh` auto-handles |

---

## Snapshot → modern translation example

**Snapshot** (`effect_old.cpp:8472-8505` IceBolt core cylinder):

```cpp
if (subspell > 1) {
    for (i = 0; i < 4; i++) {
        obj = animator->GetObject(i);                   // cyl04/01/02/03
        D3DMATRIXClear(&obj->matrix);
        obj->scl = {cylscale/2, cylscale/2, length/64};
        D3DMATRIXScale(&obj->matrix, &obj->scl);
        D3DMATRIXRotateX(&obj->matrix, M_PI/2);
        D3DMATRIXRotateY(&obj->matrix, 0);
        D3DMATRIXRotateZ(&obj->matrix, 0);
        obj->pos = {0, 0, 0};
        D3DMATRIXTranslate(&obj->matrix, &obj->pos);
        SetBlendState();                                // alpha modulate
        animator->RenderObject(obj);
    }
}
```

**Modern** (via shim):

```cpp
#include "d3dport.h"
// ...

if (subspell_ > 1) {
    d3d::Matrix inst_world;
    d3d::MatrixWorldFromBase(inst_world, base_pos, face_rad);
    for (int32_t i = 0; i < 4; ++i) {
        d3d::Matrix m;
        d3d::MatrixIdentity(m);
        const float scl_xy = cylscale_ * 0.5f;
        const float scl_z  = length_ / 64.0f;
        d3d::MatrixScale(m, scl_xy, scl_xy, scl_z);
        d3d::MatrixRotateX(m, float(M_PI) * 0.5f);
        d3d::MatrixRotateY(m, 0.0f);
        d3d::MatrixRotateZ(m, 0.0f);
        d3d::MatrixTranslate(m, 0.0f, 0.0f, 0.0f);
        d3d::RenderObject(*Renderer, cyl_meshes_[i], m,
                          cyl_diffuse_[i], cyl_emissive_[i],
                          d3d::BlendMode::Alpha,
                          &inst_world);
    }
}
```

The shape is the same as the snapshot. No matrix-order reasoning, no
sign debugging, no queue-scope guessing.

---

## Mesh registration (in `SpawnForTest_BESPOKE`)

**Before** — verbose loop with degenerate-UV special case (the snapshot
had a `ResolveSubObjUv` + manual WhiteTexture binding):

```cpp
// ~50 lines of texture-slot walk + WhiteTextureHandle + RegisterMesh + ...
```

**With shim**:

```cpp
#include "d3dport.h"
// ...

for (int32_t cyl = 0; cyl < 4; ++cyl) {
    bolt->cyl_meshes_[cyl]   = d3d::RegisterSubMesh(*Renderer, img3d, cyl, num_tex);
    d3d::LoadMaterial(img3d, cyl, bolt->cyl_diffuse_[cyl], bolt->cyl_emissive_[cyl]);
}
bolt->ring_mesh_   = d3d::RegisterSubMesh(*Renderer, img3d, kIceBoltCylRing,   num_tex);
bolt->spiral_mesh_ = d3d::RegisterSubMesh(*Renderer, img3d, kIceBoltCylSpiral, num_tex);
d3d::LoadMaterial(img3d, kIceBoltCylRing,   bolt->ring_diffuse_,   bolt->ring_emissive_);
d3d::LoadMaterial(img3d, kIceBoltCylSpiral, bolt->spiral_diffuse_, bolt->spiral_emissive_);
```

`RegisterSubMesh` returns `0` on failure — check it.

---

## Three load-bearing rules when adopting the shim

1. **`d3d::RenderObject` MUST be called from your `submit_world` callback.**
   That's the harness's `BeginTilePass`-scope hook (`vfxtest.cpp:1515-1529`).
   Calling from regular `submit` makes draws silently invisible.

   Register the callback in vfxtest.cpp like icebolt does (`:4310-4314`):
   ```cpp
   icebolt_bespoke.submit       = [](void* c, EFxDebugMode d) { /* billboards */ };
   icebolt_bespoke.submit_world = [](void* c, EFxDebugMode d) { /* d3d::RenderObject */ };
   icebolt_bespoke.pipeline     = "FB+IM";
   ```

2. **Blend mode is explicit per `RenderObject` call.** The snapshot's
   stateful `SetBlendState()` / `SetAddBlendState()` becomes an arg:
   ```cpp
   d3d::RenderObject(..., d3d::BlendMode::Alpha, ...);
   d3d::RenderObject(..., d3d::BlendMode::Additive, ...);
   ```

3. **`inst_world` is for per-entity world placement.** Build it once
   per tick via `d3d::MatrixWorldFromBase` (typically
   `T(base_pos) * Rz(face_rad)`), then pass it to every
   `RenderObject` call for that tick. The shim composes
   `world = inst_world * local`. Pass `nullptr` if your local matrix
   is already in world space.

---

## What the shim does NOT do

- Tell you **which** sub-objects to draw → read the snapshot's render body + forensics
- Tell you **what** each sub-object is → log per-sub-obj bbox + UV, cross-check forensics §4 / §7
- Handle particle emission → use `SubmitFxParticle` / `SubmitFxBillboard` / `SubmitFxParticleBucket` directly
- Handle procedural strips (LightStrip family) → use `SubmitFxStrip` directly with the per-vertex jitter pattern
- Decide blend mode for you → derive from snapshot's `SetBlendState()` / `SetAddBlendState()` calls

---

## Debugging checklist when the rendered output looks wrong

Run through this **before** assuming a port bug:

1. **Render on `--vfx-bg=black`.** If nothing visible on black, the mesh isn't
   drawing. If it's visible on black but wrong on dungeon → background interaction,
   not a port bug.
2. **Confirm the callback wiring.** `submit_world` registered? pipeline tag set?
   Look at `vfxtest.cpp`'s registration for your harness ID.
3. **Per-sub-obj UV-rect log at `SpawnForTest`.** Run once, look for `area=0.0000` —
   that's a solid-fill mesh, and `RegisterSubMesh` handles it via WhiteTexture.
   If you bypassed `RegisterSubMesh` and bound the cyl texture directly, that's
   your bug.
4. **Per-sub-obj vertex bbox log.** Mesh native size in WU. Confirm your snapshot
   scale divisors match (cyl: `scl.z = length/64` implies mesh z-span ~64).
5. **Read `src/shaders/mesh.glsl.h:36-41`.** If you composed matrices by hand
   (didn't use the shim), confirm column-vector convention.

---

## Future extensions (when there's a 7th effect that needs them)

- `d3d::SubmitParticleBatch` wrapper around `SubmitFxParticleBucket` with
  D3D-style `lverts[]` per-particle UV writes
- `d3d::Light` ambient/point-light placement helpers (currently
  spell-side; see Lightstrip forensics §9)
- `d3d::ChromaKeyTexture` for asset textures that need chroma-key →
  alpha conversion at load time
