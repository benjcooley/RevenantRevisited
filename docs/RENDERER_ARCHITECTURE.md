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
   (3)    |  UI / PANES                                    |
          |    TScreen, TPane tree -- layout + compose     |
          |    Update() per frame; Draw() is 2D + blits    |
          |    of offscreen surfaces only (no main display)|
          +------------------------------------------------+
          +------------------------------------------------+
   (2)    |  SCENE MANAGERS                                |
          |    TMapRenderer (map scene: camera, sectors,   |
          |       visibility/culling, sort inputs)         |
          |    UI-preview scenes (future: Locke in         |
          |       inventory pane, portrait panel, ...)     |
          |    walk scene contents -> Submit() on each     |
          |       drawable component                       |
          +------------------------------------------------+
          +------------------------------------------------+
  (1.5)   |  DRAWABLE STATE CLASSES (held by instances)    |
          |    TTileRenderer, TMeshRenderer, TSkinRenderer,|
          |    TQuadRenderer -- per-instance payload +     |
          |    Submit() that hands itself to TRenderer     |
          +------------------------------------------------+
          +------------------------------------------------+
   (1)    |  CORE RENDERER    (TRenderer, src/renderer.h)  |
          |    sokol pipelines, shaders, vertex buffers    |
          |    G-buffer RTs (albedo / normal / scene-z)    |
          |    per-pass instance buckets + instance-VB     |
          |    upload + sg_draw(..., num_instances)        |
          |    directional + point lights, AO, sun shadow  |
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
speaks only in pipelines, images, uniforms, and screen pixels. Scene managers
at layer (2) hold game state and decide *what* to draw; drawable state
classes at layer (1.5) describe *how a given instance looks* to the renderer;
the core renderer at layer (1) batches and dispatches GPU work.

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

## Frame pipeline (distinct from pass pipeline)

The *pass* table above is what TRenderer does inside one main render. The
*frame* pipeline is how the main loop drives everything each tick. Four
phases, strictly ordered:

| # | Phase               | Who runs                                         | What happens |
|---|---------------------|--------------------------------------------------|--------------|
| 1 | **Update**          | game sim + `TPane::Update()` walk                | advance state; panes queue any offscreen work they need |
| 2 | **Pane offscreen**  | panes that need a 3D preview (inventory Locke, portrait, ...) | each runs its own scene through TRenderer into *its own* offscreen surface -- finishes before step 3 |
| 3 | **Main render**     | frame loop, directly (**not** from the pane graph) | TRenderer renders the world scene via `TMapPane`'s `TMapRenderer` into the framebuffer targets (G-buffer -> AO -> lighting) |
| 4 | **Composite / UI**  | `TPane::Draw()` walk                             | 2D bits only: HUD text, boxes, cursor, and blits of the offscreen surfaces from step 2 into their pane rects. No direct 3D here. |

The pane graph is a **UI layout + composition tree**, not a draw-dispatch
tree. The main render fires regardless of what panes do; panes consume
its output (and their own offscreen output from step 2) at composite time.

`TPane::Draw()` survives in the API but its contract narrows to "2D on top,
plus compositing my offscreen surface if I have one." If a pane wants 3D,
it queues it in step 2 and blits the result in step 4.

## Scene managers vs. core renderer

Clean boundary between the two layers that do real work:

- **Scene manager (e.g. `TMapRenderer`)** -- *what* to draw. Camera, frustum,
  sector walk, visibility/culling, LOD, sort inputs, instance lifecycle.
  Produces a per-frame stream of submits by iterating its scene and calling
  `Submit()` on each drawable (directly or via the per-drawable state
  classes on instances).
- **Core renderer (`TRenderer`)** -- *how* to draw. Pipelines, passes,
  bucket aggregation, instance buffer upload, material/atlas registries,
  G-buffer + AO + lighting + composite. Knows nothing about sectors,
  objects, cameras-as-game-concepts, or visibility rules.

Swap `TMapRenderer` for a UI-preview scene driver and TRenderer doesn't
notice. Where new things land:

- New culling strategy, sector streaming, "show enemies through walls"
  debug toggle -> **scene manager**.
- New pass (water, decals, debug lines), new shader, new instance-data
  layout -> **core renderer**.
- New drawable *kind* (particle ribbon, trail) -> new drawable state class
  + core-renderer pipeline; scene managers just submit it like anything
  else.

`TMapRenderer` is currently a free-standing class driven by the test
harness ([g_mapRenderer in testmodes.cpp](../src/testmodes.cpp#L26)). The
final wiring is `TMapPane` *has-a* `TMapRenderer` -- the pane is the UI
container, the renderer is the scene driver; they travel together.

## Drawable state classes (layer 1.5)

Each instance holds exactly one drawable state class matching its kind.
Siblings today:

- **`TTileRenderer`** -- atlas sub-rect + dst rect + anchor_z + tint. What
  the current `DrawTile` path does, repackaged as a per-instance component.
- **`TMeshRenderer`** -- rigid mesh handle + world transform + material
  slot. Used for any rigid 3D prop.
- **`TSkinRenderer`** -- mesh handle + bone palette + material slot. Used
  for characters and anything with hierarchical animation (Revenant's
  rigid sub-object `anikeys` count as "one-bone skinned"; a single mesh is
  rigged to one bone per sub-object).
- **`TQuadRenderer`** -- unlit or alpha-tested billboard / screen quad.

Each state class exposes a small `Submit()` that hands its payload to
`TRenderer` (see below). It owns per-frame per-instance state; it does not
own GPU resources. Heavy assets (vertex/index buffers, atlas pages,
textures, materials) live in `TRenderer` registries behind opaque handles;
the state class just references handles.

### Submit -> scheduled -> drawn

Scene managers and drawable state classes **just call `Submit()`** in
whatever order they walk their data -- no pre-sorting, no
pre-categorizing, no bucketing on their side. Each submit carries
sort keys (pipeline, mesh/atlas, material) + draw flags (opaque /
transparent / decal / preserve-order / pass assignment / ...) + the
per-instance payload.

**TRenderer owns scheduling.** At pass end it reads the flags on each
entry, honors the ordering constraints they impose, and within the
slack those constraints leave it optimizes for state-change cost:

- Opaque entries are free to reorder; they cluster by shared keys and
  emit as big instanced draws (one `sg_draw(..., num_instances)` per
  contiguous equal-key run).
- Transparent entries must respect back-to-front depth order; within
  depth-adjacent neighbors that happen to share keys, the renderer
  still batches. Worst case is one draw per entry -- the cost of
  correctness.
- `preserve-order` / decal / layered entries keep their emission
  position and flush any in-flight batch on boundary changes.
- Pass-assignment flags route entries into the right pass (G-buffer,
  shadow caster, transparent, debug, ...) with that pass's ordering
  rules.

There is no type registry and no per-kind container. Like objects end
up drawn together because they share sort keys, not because the arch
put them in separate buckets. Adding a new drawable kind = new
pipeline + its keys; `Submit()` doesn't care what kind the entry is.

Three carriers for per-instance data, picked by data shape:

- **Instance vertex buffer** (all backends): fixed, small payload.
  Default -- tiles, rigid meshes, quads. Instance id from
  `gl_InstanceIndex` / `SV_InstanceID`.
- **Palette texture** (all backends): variable-size-per-instance.
  Required for skinned meshes -- RGBA32F 2D texture, one row per
  instance, indexed by instance id.
- **Storage buffer / large UBO** (backend-gated on older GL/GLES) --
  deferred until actually needed.

Buckets are **per-scene** (or per-view). A map tile and a UI-preview
mesh can't batch together because their pipelines' uniforms and
targets differ; TRenderer drains scenes in order (map first, UI scenes
on top), each with its own camera UBO and pass scope.

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

- **Atlas pool + handle system** -- `TRenderer` registry of bitmap atlases,
  meshes, materials, and bone-palette slots, all behind opaque handles
  (`AtlasId`, `MeshHandle`, `MaterialHandle`, `PaletteSlot`). Consumers
  hand over source data, get back a handle. Weak / ref-counted variants
  TBD.
- **Per-pass instance buckets** -- bucket key `(pipeline, mesh/atlas,
  material)`, dynamic instance VBO uploaded once per bucket per frame.
  Default-batched drain path; ordered-mode flush-on-key-change path for
  transparent/decal/forward passes. Used by every drawable kind below.
- **`TTileRenderer`** -- repackage current `DrawTile` as an instanced
  drawable state class. Instance row: dst rect, atlas sub-rect,
  anchor_z, mul factors. One draw per atlas page after migration.
- **`TMeshRenderer`** -- rigid mesh drawable state class. Instance row:
  world-transform rows + material slot. Shares pipeline with
  `TSkinRenderer` (rigid = skinned with one bone).
- **`TSkinRenderer`** -- skinned mesh drawable state class. Instance row:
  palette-texture row index + material slot. Bone palette uploaded as
  RGBA32F texture each frame.
- **`TQuadRenderer`** -- unlit / alpha-tested billboards and screen
  quads. Thinnest kind; mostly UI + effects.
- **Water / refraction pass** -- scene-color A<->B ping-pong; sampled back
  into a subsequent pass for refraction.
- **Transparent effects pass** -- forward transparency over `lit_target`.
  First consumer of ordered-mode submission.
- **Debug line/triangle pass** -- small dedicated pipeline, submitted by
  debug UI or test harness.
- **`TMapPane` owns `TMapRenderer`** -- migrate `g_mapRenderer` off the
  global so the test harness and the real pane both go through
  `TMapPane::Update -> MapRenderer::RenderFrame -> TRenderer::Submit*`.
- **Frame-graph scaffolding** -- each pass declares its read/write RTs so
  pass ordering and aliasing can be validated at startup rather than by
  convention.
