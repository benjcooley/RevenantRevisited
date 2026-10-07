# Renderer Architecture

The port's render stack is split into three strictly separated layers. Game-
side code holds data; the core renderer owns GPU state and submission. This
document captures the layering, the per-frame pass pipeline, and how the new
`TRenderer` class relates to the demoted `TDisplay`.

For the shader math (normal reconstruction, sun-shadow ray march, AO, iso
inverse), see [DEFERRED_LIGHTING.md](DEFERRED_LIGHTING.md). For the lighting
models themselves (Classic = retail, modern), see
[LIGHTING_FIDELITY.md](LIGHTING_FIDELITY.md). For the retail
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
| 5 | Transparent world  | `lit_target` + scene depth (`helper_pass`) | `DrainTransparentWorldQueue`: transparent tiles, helper meshes, translucent meshes, back to front |
| 6 | Transparent FX     | `lit_target` + scene depth (`fx_pass`) | `DrainFxQueue` |
| 7 | Debug 3D           | lit color                    | *forthcoming* (line/tri prims)     |
| 8 | Game UI            | backbuffer or swapchain      | `TScreen`/`TPane` via `TRenderer::Composite` |
| 9 | Debug UI           | swapchain                    | `simgui_render` (inside `FlipPage`) |

At present, [TMapRenderer](../src/maprenderer.cpp) drives passes 1-3 each
frame by pushing point lights, tile draws, and calling `RunLightingPass`,
which runs passes 5 and 6 on the lit result.
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
Renderer->SetClassicLightModel(model);
Renderer->ClearPointLights();
Renderer->AddRetailPointLight(wx, wy, wz, radius, r, g, b, multiplier);
Renderer->AddPointLight(wx, wy, wz, radius, r, g, b, intensity);
Renderer->BeginTilePass(0.0f, 0.0f, 0.0f, 1.0f);
Renderer->DrawTile(color, depth, dst_x, dst_y, ...);
Renderer->EndTilePass();
Renderer->SetReconstructionParams(ox, oy, znear, zfar, cx, cy, kcam, 0);
Renderer->RunLightingPass();
```

Non-renderer surface calls (`Put`, `WriteText`, `Box`, `ZPut`, dirty-rect
hooks) still go through `Display.` -- those are TSurface operations, not
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
`SetLightingMode(0)` selects the Classic (retail) model, whose inputs come
from `SetClassicLightModel` (computed by `classiclighting.cpp`); it lights
tiles and meshes differently, keyed by the G-buffer normal target's alpha
(1 tile, 0 mesh). A submission whose id carries `kObjFlagSelfLit` (retail's
`ANIIM_LIT` imagery, drawn after lighting) shows its albedo unlit in every
mode. See [LIGHTING_FIDELITY.md](LIGHTING_FIDELITY.md).

**Point lights (rebuild each frame)** -- `ClearPointLights`,
`AddRetailPointLight(wx,wy,wz,radius,r,g,b,multiplier)` for authored map
lights (retail units; modern mode reads them through
`SetRetailLightModernScale`), `AddPointLight(wx,wy,wz,radius,r,g,b,intensity)`
for direct (VFX) lights. Max
[`kMaxPointLights` (16)](../src/renderer.h#L94) across both; extras silently dropped.

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

## Translucent meshes

A mesh submitted with a tint alpha below `kOpaqueMeshAlpha` (0.999) is
translucent: a character fading in or out. It can't go through the
G-buffer, where it would replace the albedo, normal and depth of whatever is
behind it, so `TRenderer::SubmitMesh` routes it to the transparent-world
pass (pass 5) instead. That pass runs after the deferred light pass, over
`lit_target`, with the scene depth buffer bound.

**Submission.** The scene manager doesn't decide the pass. It fills two sort
keys on every `SMeshSubmit` and the renderer uses them only for translucent
meshes:

- `sort_depth`: the object's camera depth (world units, greater is
  farther). The pass draws back to front.
- `surface_id`: shared by every mesh of one object. `TMapRenderer` uses the
  object's map index + 1.

**Drawing a surface.** A character is a dozen or more meshes, one per body
part. Blending each part separately would show the arm through the chest.
The pass draws the meshes of one surface as one layer, in two steps:

1. `mesh_depth_pipeline`: depth only, colour writes masked off. Leaves the
   surface's nearest depth.
2. `mesh_translucent_pipeline`: lit colour, depth test `LESS_EQUAL`, no depth
   write, blended at the tint alpha. Only fragments at the nearest depth
   pass, so each pixel blends once.

Both steps run behind `kMeshVs`, the G-buffer mesh vertex shader, so their
depths agree. The surface's depth stays in the scene depth buffer
afterwards. Transparent draws behind it are hidden by it, as retail's
z-written translucent characters hid them.

**Lighting.** The translucent fragment shader calls `shade_surface()`, the
same function the deferred light pass calls for a G-buffer sample. Both
shaders are built from `kLightModel` (`src/shaders/lightmodel.*.h`, the light
uniform block, the Classic and modern models, and `shade_surface`) followed
by the pass's own body (`TRenderer` prepends it at pipeline creation), and
both upload the same uniforms (`TRenderer::PackLightUniforms`). The light
pass reconstructs the world position from scene depth and the normal from
the G-buffer; the translucent pass has both from the vertex shader. The
lighting is in world space either way. Measured on the opening scene with
every mesh forced through the translucent pass at alpha 1, about 97% of the
pixels of the two idle NPCs (Sardok, Tendrick) match the G-buffer path
within 2/255, in Classic and in modern lighting, with no mean bias. The
rest are idle-animation differences between the two runs and the
G-buffer's 8- and 16-bit storage.

What a translucent mesh doesn't get, because it isn't in the G-buffer:
screen-space AO and the sun cast-shadow mask (modern lighting only; the
shader passes 1 for both), the editor's id target (it can't be picked while
it fades), and a place in the AO and sun-shadow inputs of other surfaces.

Instance rows for the pass's meshes are uploaded once per drain into the
next buffer of the mesh instance ring (`NextMeshInstanceBuffer`).

## Character transparency and invisibility

### Retail (forensics)

Retail draws a character through `TCharAnimator` (vtable `0x005a7dd8`):
ctor `0x004d76b0`, `Animate` `0x004d7a00`, `Render` `0x004d7a50` (slot
`+0x34`). The fade itself is simulation, on `TCharacter` (`+0x194..+0x1a4`,
`Fade` / `SetFade` / `UpdateFade` / `Transparency`,
[COMMAND_SYSTEM.md](gameflow/forensics/COMMAND_SYSTEM.md) §6.4). The animator
keeps its own *drawn* alpha at `+0x588` and moves it toward the fade:

```
Render (0x004d7a50), head:
  if (!(inst->flags & OF_INVISIBLE) || Editor) {       // flags bit 0x80, Editor = DAT_00668154
      if (!preview) {                                  // +0x1a4, see below
          t = inst->Transparency() * 0.01             // vtable +0x2d8 = 0x004c5a50; 0.01 @0x005a350c
          if (!Editor && |t - a| >= 0.05)              // 0.05 @0x005a7e9c
              t = a +/- 0.05
          a = t                                        // a = +0x588
          if (a < 0.01) return                         // 0.01 @0x005a3524: nothing drawn, no shadow
      }
  } else if (!preview) return                          // OF_INVISIBLE: not drawn, a holds
```

Then, in order: hide the `sword`/`weapon` objects; a player hides the body
parts its equipment replaces (`0x004d7f20(1)`); the blob shadow
(`0x004d86c0`, alive characters only) scaled by `Radius() * 1/12 * a`
(1/12 @`0x005a7eac`), so the shadow shrinks as the character fades;
`SetMaterialTransparency` (`0x004d82a0`) writes `a` to every material's
ambient/diffuse/specular/emissive alpha and sets the blend state through
`0x00417d60`; `T3DAnimator::Render` (`0x0040e8d0`); the equipment
(`0x004d7f20(2)`), each item through `SetMaterialTransparency`, so worn items
fade with the body.

The blend state: 1 (opaque: z-write, z-test, cull CCW, `ONE`/`ZERO`) when
`|a - 1| <= 0.001` (@`0x005a7ea8`), else `0x84`: `SRCALPHA`/`INVSRCALPHA`,
z-write on (flag `0x80`), z-test on, culling off, texture alpha times
material alpha. Triangles blend in draw order.

The ctor seeds `a = Transparency() * 0.01` (`0x004d77b2`) and sets every
material's alpha to 1 (its loop writes material 0 each pass).

`+0x1a4` is a preview flag. The inventory/paperdoll draw sets it on the
player's animator around its render (`0x00536fda`/`0x00537003`,
`0x00538c7c`); with it set the character draws opaque whatever its fade or
OF_INVISIBLE, without the shadow.

Who gets an animator: `TMapPane::AnimateObjects` (`0x00458750`) walks the
map with `0x29` = `CHECK_SECTRECT | CHECK_INVIS | CHECK_NOINVENT`. An
OF_INVISIBLE object gets no `OnScreen` (no animator) and no `Animate`. A
character loaded invisible therefore has no animator until it shows, and its
first drawn alpha is the fade at that moment.

Retail's timer ran at 24 Hz and drew at most one frame per tick, so the
drawn alpha moved at most 0.05 x 24 = 1.2 a second. The script fade moves
5 a pulse at 24 pulses a second, the same rate: either takes 0.83 s from 0
to 100.

What this means for the Keep's opening (`keep.s`, `SardokR`):

- Locke is saved OF_INVISIBLE in `newgame.sav`: not drawn, no animator.
- `fadecharacterout player`: his fade runs 100 -> 0 unseen.
- `player.toggle invisible` + `fadecharacterin player`: he shows with fade
  0, and the alpha follows the fade back to 1 over 0.83 s.
- Rahul (`MUDOKON`) is OF_INVISIBLE until `RAHUL.TOGGLE INVISIBLE = 0`. Load
  leaves every character at fade 100 (`0x004d4eb0`), so
  `FADECHARACTERIN RAHUL` has nothing to do: he appears at full opacity as
  the door opens.

### Port

- `TCharAnimator` holds the drawn alpha (`transparency`, retail `+0x588`)
  and exposes it through two `T3DAnimator` virtuals: `UpdateDrawState(dt)`,
  called by `TMapRenderer::RenderFrame` once per drawn frame for every
  animated object, and `DrawAlpha()`, read by the mesh submit (0 = skip).
  The base `T3DAnimator` draws at 1.
- `UpdateDrawState` is retail's head: hidden while OF_INVISIBLE outside the
  editor (the alpha holds), else `UpdateTransparency(dt)`. `DrawAlpha` is 0
  while hidden or below 0.01.
- `TMapRenderer` submits each mesh of the object with `tint.a = DrawAlpha()`
  and the translucent sort keys. `TRenderer` decides the pass.
- The paperdoll (`uiequiptest.cpp`, the equip pane) draws through its own
  submit at alpha 1, which is retail's `+0x1a4` behaviour.

Checked on the opening (`--quickstart --headless`, filmstrips): Locke isn't
drawn until `player.toggle invisible`; he shows with his alpha seeded at
0.05 (fade 5) and his colour over the black pit rises linearly to full in
about 0.8 s (0.1 s captures), in Classic and in modern lighting. Rahul shows
at alpha 1 (fade 100) and walks in opaque.

Deviations from retail:

- **Time-based ramp.** The alpha moves `1.2 * dt` (`kTransparencyPerSecond`
  = 0.05 x `TTime::LegacyFramerate`) per drawn frame instead of 0.05 per
  drawn frame, so it takes 0.83 s at any frame rate. `dt` is
  `TTime::DeltaTime()` (scaled), so slow motion slows it with the fade.
- **Re-seeding.** Port characters keep a permanent animator
  (`TCharacter::IsAnimatorPermanent`), so there is no animator creation to
  seed the alpha when a character shows. `UpdateDrawState` re-seeds it from
  `Transparency()` on the first drawn frame after the character was hidden.
  That is retail for a character that had no animator while invisible (every
  scripted appearance, Locke's included). It differs only for a character
  that was drawn, went invisible, and showed again at a different fade:
  retail would ramp from the alpha it had.
- **One layer per surface.** Retail blended a fading character's triangles
  in draw order with z-write on and culling off, so some inner and far-side
  faces showed through, depending on order. The port draws the nearest
  layer only.
- **Not in the G-buffer while translucent.** No screen-space AO or sun cast
  shadow on the fading character (modern lighting), no id-target picking.
- **Not ported here:** the blob shadow (the port doesn't draw the retail
  `CharUtility` shadow yet; when it does, scale it by `DrawAlpha()`), the
  equipment fade (the port doesn't draw worn equipment meshes on characters
  yet; they should share the character's alpha and `surface_id`), the
  `+0x224` additive second pass (`0x00417d60(0x10)`), and OF_INVISIBLE for
  non-character objects (retail's map-pane walks skip them via
  `CHECK_INVIS`; the port draws them).

Fixed in `charanimator.cpp` along the way: `InitTransparency` assigned a
local (the member was never set); `Set`/`ResetMaterialTransparency` wrote
material 0 on every pass; the reset alpha was 100, not 1; `abs()` on a float
difference resolved to the integer overload.

Retail reference shots that would settle the open details (dosbox-x, New
Game, frame-by-frame capture of the opening):

1. The ~1 s after `player.toggle invisible`: Locke fading in over the pit.
   Settles whether he starts from nothing (the re-seed), whether inner or
   far-side faces show through (draw order), and whether his shadow grows
   with him.
2. Rahul at the door: a pop to full opacity, as predicted above.
3. The invisibility spell on Locke (fade 30): the steady translucent look,
   and whether worn equipment fades with him.

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
- **Ordered-mode batching** -- the transparent-world pass (pass 5) sorts
  back to front and issues one draw per entry; translucent meshes draw one
  per mesh per step. Batch depth-adjacent entries that share keys.
- **Debug line/triangle pass** -- small dedicated pipeline, submitted by
  debug UI or test harness.
- **`TMapPane` owns `TMapRenderer`** -- migrate `g_mapRenderer` off the
  global so the test harness and the real pane both go through
  `TMapPane::Update -> MapRenderer::RenderFrame -> TRenderer::Submit*`.
- **Frame-graph scaffolding** -- each pass declares its read/write RTs so
  pass ordering and aliasing can be validated at startup rather than by
  convention.
