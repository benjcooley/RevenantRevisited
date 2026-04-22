# Renderer Architecture

The port's render stack is split into three strictly separated layers. Game-
side code holds data; the core renderer owns GPU state and submission. This
document captures the layering, the per-frame pass pipeline, and how the new
`TRenderer` class relates to the demoted `TDisplay`.

For the shader math (normal reconstruction, sun-shadow ray march, AO, iso
inverse), see [DEFERRED_LIGHTING.md](DEFERRED_LIGHTING.md). For the retail
1998 high-level system map, see [ARCHITECTURE.md](ARCHITECTURE.md). For the
map-rendering data flow specifically, see [MAP_RENDERING.md](MAP_RENDERING.md).

## The three layers

```
          +------------------------------------------------+
   (3)    |  UI                                            |
          |    TScreen / TPane, font atlases, ImGui        |
          |    submits via TRenderer::Composite            |
          +------------------------------------------------+
          +------------------------------------------------+
   (2)    |  GAME CONSUMERS                                |
          |    TMapRenderer (sectors, tiles, point lights) |
          |    TMeshRenderer  (characters -- forthcoming)  |
          |    Effects, water, debug draw -- forthcoming   |
          |    walk game data, cull, sort, submit          |
          +------------------------------------------------+
          +------------------------------------------------+
   (1)    |  CORE RENDERER    (TRenderer, src/renderer.h)  |
          |    sokol pipelines, shaders, vertex buffers    |
          |    G-buffer RTs (albedo / normal / scene-z)    |
          |    pass objects, composite quad, ImGui backend |
          |    directional + point lights, AO, sun shadow  |
          |    submission API: tile pass, lighting pass,   |
          |    composite, present-to-swapchain             |
          +------------------------------------------------+
          +------------------------------------------------+
   (0)    |  SURFACE / SWAPCHAIN (TDisplay, TSurface)      |
          |    sokol context setup, backbuffer/front/z     |
          |    TSurfaces for legacy 2D blits, FlipPage     |
          +------------------------------------------------+
```

**The invariant**: layer (1) knows nothing about sectors, tiles,
`TObjectInstance`, imagery, animators, or any `T3D*`/`TObject*`-anything. It
speaks only in pipelines, images, uniforms, and screen pixels. Consumers at
layer (2) hold all game state and issue calls into layer (1).

Centralized pass management, pass ordering, and draw submission live in the
core renderer. Game code never creates its own `sg_pass`, pipeline, or RT.

## Per-frame pass pipeline

The eight logical passes (not all wired today -- forthcoming passes are
marked):

| # | Pass               | RT / attachment              | Owner / status                     |
|---|--------------------|------------------------------|------------------------------------|
| 1 | G-buffer fill      | `albedo` + `normal` + `scene_z` + depth (MRT `default_pass`) | `TRenderer::BeginTilePass` / `DrawTile` / `EndTilePass` |
| 2 | Ambient occlusion  | `ao_target` R32F (`ao_pass`) | `TRenderer::RunAOPass` (inside `RunLightingPass`) |
| 3 | Deferred lighting  | `lit_target` RGBA8 (`lit_pass`) | `TRenderer::RunLightingPass`     |
| 4 | Water / refraction | scene-color ping-pong        | *forthcoming*                      |
| 5 | Transparent FX     | lit color                    | *forthcoming*                      |
| 6 | Debug 3D           | lit color                    | *forthcoming* (line/tri prims)     |
| 7 | Game UI            | backbuffer or swapchain      | `TScreen`/`TPane` via `TRenderer::Composite` |
| 8 | Debug UI           | swapchain                    | `simgui_render` (inside `FlipPage`) |

At present, [TMapRenderer](../src/maprenderer.cpp) drives passes 1-3 each
frame by pushing point lights, tile draws, and calling `RunLightingPass`.
[TDisplay::FlipPage](../src/display.cpp#L163) opens the swapchain pass,
asks the core renderer to present the freshest scene image (pass 3 output if
available, falling back to the 2D backbuffer for UI-only frames), and then
renders the ImGui overlay on top.

### G-buffer padding

The G-buffer is allocated at `(width + 2*kGBufPad) x (height + 2*kGBufPad)`
so screen-space effects -- particularly the sun-shadow ray march -- can still
find occluders that sit just off the visible edge.
[TRenderer::PresentToSwapchain](../src/renderer.cpp) composites only the
centered display-sized sub-rect of `lit_target` to the final swapchain. The
pad is [`TRenderer::kGBufPad` (128px)](../src/renderer.h#L90).

## The `Renderer` global

```cpp
// src/renderer.h
extern TRenderer* Renderer;
```

Created by [TDisplay::Initialize](../src/display.cpp#L38) after `sg_setup()`,
destroyed by [TDisplay::Close](../src/display.cpp#L119). All renderer-
specific calls go through this pointer:

```cpp
Renderer->SetLight(dx, dy, dz, intensity, r, g, b, ambient);
Renderer->ClearPointLights();
Renderer->AddPointLight(wx, wy, wz, radius, r, g, b, intensity);
Renderer->BeginTilePass(0.12f, 0.16f, 0.10f, 1.0f);
Renderer->DrawTile(color, depth, dst_x, dst_y, ...);
Renderer->EndTilePass();
Renderer->SetReconstructionParams(ox, oy, znear, zfar, cx, cy, kcam, 0);
Renderer->RunLightingPass();
```

Non-renderer surface calls (`Put`, `WriteText`, `Box`, `ZPut`, dirty-rect
hooks) still go through `Display->` -- those are TSurface operations, not
pipeline submission.

## Submission API at a glance

The full declaration lives at [src/renderer.h](../src/renderer.h). Grouped by
purpose:

**Lifetime / state** -- `Initialize(w,h)`, `Shutdown()`, `Width()`,
`Height()`, `ColorTarget()`.

**G-buffer fill** -- `BeginTilePass(r,g,b,a)`, `DrawTile(...)`,
`EndTilePass()`.

**Directional + ambient lighting** -- `SetLight`, `SetAmbientColor`,
`SetAmbientOcclusion`, `SetNormalRadius`, `SetEdgeThreshold`,
`SetNormalLightingHardness`, `SetTileViewMode`, `SetLightingMode`,
`SetSunShadow`, `SetShadowWorldDir`, `SetShadowVariance`.

**Point lights (rebuild each frame)** -- `ClearPointLights`,
`AddPointLight(wx,wy,wz,radius,r,g,b,intensity)`. Max
[`kMaxPointLights` (16)](../src/renderer.h#L94); extras silently dropped.

**Deferred reconstruction** -- `SetReconstructionParams(ox, oy, z_near,
z_far, center_wx, center_wy, kcam_forward, reserved)` once per frame before
`RunLightingPass()`.

**Compositing** -- `Composite(TSurface*)`, `Composite(sg_image, dst, target)`
for RT blits, atlas-friendly `Composite(sg_image, dst, target, src, texdim)`
for sub-rect blits inside another pass.

**Present** -- `PresentToSwapchain()` called from `TDisplay::FlipPage` inside
the swapchain pass. Returns `true` if it drew 3D output; `false` if it drew
nothing (callers fall back to a 2D backbuffer composite).

## How TDisplay relates

`TDisplay` has been demoted. Its job now is narrow:

- Own the sokol context (`sg_setup` / `sg_shutdown`)
- Hold the backbuffer / frontbuffer / zbuffer TSurfaces used by legacy
  Put/WriteText/Box/ZPut call sites
- Drive the swapchain pass in `FlipPage`
- Wire ImGui's sokol backend

Everything else -- pipelines, shaders, render targets, pass objects, the
composite quad, lighting state -- moved to `TRenderer`. `FlipPage` delegates
the final blit to `Renderer->PresentToSwapchain`, then renders ImGui on top
of whatever the renderer presented.

## Shaders + backend selection

Shader sources live under [src/shaders/](../src/shaders/), one file per pass
per backend:

    composite.metal.h   composite.glsl.h   composite.hlsl.h
    tile.metal.h        tile.glsl.h        tile.hlsl.h
    ao.metal.h          ao.glsl.h          ao.hlsl.h
    light.metal.h       light.glsl.h       light.hlsl.h

`shaders/shaders.h` is an umbrella that selects one bundle at compile time
based on the `SOKOL_METAL` / `SOKOL_GLCORE33` / `SOKOL_D3D11` define set by
CMake, and exposes backend-agnostic names (`kCompositeVs`, `kTileFs`,
`kShaderVsEntry`, ...) that `renderer.cpp` consumes.

CMake picks a default per platform (Metal on Apple, D3D11 on Windows, GL core
on Linux) via the `SOKOL_BACKEND` cache variable; override at configure time
if you need a non-default backend. VS entry points differ across backends
(`_main` on MSL, `main` on GLSL, `main_vs`/`main_ps` on HLSL), and HLSL
needs attribute semantics on the pipeline layout -- both are handled in
`renderer.cpp` under `SOKOL_<BACKEND>` guards.

## World-space invariant

The deferred path is world-space end-to-end. Screen-space math is only valid
as a view-time convenience; anything the lighting or depth math operates on
must be in world units. Screen math leaking into the depth path is a bug.
See [DEFERRED_LIGHTING.md](DEFERRED_LIGHTING.md) for the derivation.

## Forthcoming work

Tracked here so future additions land in the right layer:

- **Atlas pool + handle system** -- `TRenderer` registry of bitmap atlases
  keyed by opaque `AtlasId`. Consumers hand over pixel data, get back a
  handle. Weak / ref-counted variants TBD.
- **Tile instancing** -- single draw per atlas per frame instead of per-tile
  `DrawTile`. Instance data: dst rect, anchor_z, mul factors.
- **Mesh pipeline** -- forward lit, skinned shader used for *all* meshes
  (rigid meshes are "one bone" skinned). Per-frame bone-palette buffer; one
  draw per mesh. `TMeshRenderer` feeds it.
- **Water / refraction pass** -- scene-color A<->B ping-pong; sampled back
  into a subsequent pass for refraction.
- **Transparent effects pass** -- forward transparency over `lit_target`.
- **Debug line/triangle pass** -- small dedicated pipeline, submitted by
  debug UI or test harness.
- **Frame-graph scaffolding** -- each pass declares its read/write RTs so
  pass ordering and aliasing can be validated at startup rather than by
  convention.
