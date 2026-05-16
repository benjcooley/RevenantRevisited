# Phase 1 — Engine Spine

Concrete design for the renderer-side primitives, batching, and integration
points that Phase 2 per-effect agents will submit into. One layer below
[PARTICLE_EFFECTS.md](../PARTICLE_EFFECTS.md) §13: this picks the struct
shapes, batching strategy, sokol pipelines, and frame call sites.

Companion reading: [vfx/README.md](README.md) §4, [PARTICLE_EFFECTS.md](../PARTICLE_EFFECTS.md)
§4/§6/§9/§10/§13, [RENDERER_ARCHITECTURE.md](../RENDERER_ARCHITECTURE.md).

## 1. Scope of Phase 1

| Pipeline | Phase 1 | Reason |
|----------|---------|--------|
| **FB** flipbook billboard | **P1-must** | TFlameEffect already exercises 90%; closes the half-wired loop at [src/effect.cpp:579](../../src/effect.cpp#L579). |
| **PE** particle emitter   | **P1-must** | VM + computed buffer exist in [src/particlefx.h](../../src/particlefx.h); replace today's per-particle `SOverlaySubmit` shim with a real submit path. |
| **SR** strip / ribbon     | **P1-must** | No existing pipeline can fake it. One validator (sword trail) deferred to Phase 2. |
| **LS** light source       | **P1-must** | Glue around the existing `Renderer->AddPointLight` ([src/renderer.h:466](../../src/renderer.h#L466)). |
| **MP** mesh projectile    | **P1-could** | `SMeshSubmit` ([src/renderer.h:235](../../src/renderer.h#L235)) is reusable; only an effect-owned wrapper is missing. Defer if anything slips. |
| **VO** volumetric overlay | **deferred** | Needs a scene-color ping-pong pass; that's renderer work, not vfx. |

Non-goals for P1: GPU-side particle sim, soft-particle depth fade, global
cross-bucket transparency sort, new expression-language operators beyond
what the torch already compiles.

## 2. Renderer-side submission API

Three new submit structs live in [src/renderer.h](../../src/renderer.h)
alongside `STileSubmit` / `SOverlaySubmit` / `SMeshSubmit`. All world-space
positions — the deferred world-space invariant
([RENDERER_ARCHITECTURE.md](../RENDERER_ARCHITECTURE.md#world-space-invariant))
applies. Screen projection lives in the renderer, not in the caller.

Shared enums + key:

```cpp
enum class EFxBlend       : uint8_t { Alpha = 0, Additive = 1, PremulAlpha = 2 };
enum class EFxDepthMode   : uint8_t { TestNoWrite = 0, None = 1 };
enum class EFxDebugMode   : uint8_t { Normal = 0, SolidColor = 1, FullTexture = 2, CurrentFrame = 3 };

// Bucket coalescing key (§3); packed so equality is one 64-bit compare.
struct SFxBatchKey {
    TTextureHandle texture;     // 0 = white 1x1
    uint16_t       pipeline_id;
    uint8_t        blend;       // EFxBlend
    uint8_t        depth_mode;  // EFxDepthMode
};
```

### 2.1 SBillboardDrawItem

One persistent world-space billboard, screen-aligned. Replaces the overlay
path at [src/maprenderer.cpp:1163](../../src/maprenderer.cpp#L1163).

```cpp
struct SBillboardDrawItem {
    float        world_pos[3];   // world units
    float        size_wu[2];     // width, height in world units (not pixels)
    float        color_rgba[4];
    float        uv_rect[4];     // x, y, w, h normalized
    SFxBatchKey  key = {};
    EFxDebugMode debug_mode = EFxDebugMode::Normal;
};
```

The current code authors world-unit sizes (torch: 25.0/62.5) then
multiplies by `overlay_scale` in maprenderer to land in pixels; that
projection moves into the renderer.

### 2.2 SParticleDrawItem

Same fields as `SBillboardDrawItem` plus `float rotation_rad` (CCW around
camera axis). Distinct from billboard because particles want per-instance
rotation and bulk-SoA drain straight from a `TParticleBucket` without a
per-particle copy on the hot path.

Submission is per-bucket: `SubmitFxParticleBucket(const TParticleBucket&)`
walks the bucket's SoA columns indexed by
[`EParticleVar`](../../src/particlefx.h#L15) straight into the per-frame
instance VBO. `SParticleDrawItem` is the logical shape; the wire format is
SoA columns.

### 2.3 SStripDrawItem

An ordered chain of N segments sharing a material; renderer expands to a
screen-aligned triangle strip per segment.

```cpp
struct SStripSegment {
    float world_a[3], world_b[3];
    float width_a_wu, width_b_wu;
    float color_a[4],  color_b[4];
    float u_a, u_b;               // V is 0 / 1 at the two edges
};
struct SStripDrawItem {
    const SStripSegment* segments = nullptr;
    int32_t              num_segments = 0;
    SFxBatchKey          key = {};
    EFxDebugMode         debug_mode = EFxDebugMode::Normal;
};
```

Segment buffer is caller-owned; renderer copies on submit (strip counts
are small — sword trails are ~16 segments). Strips do not auto-batch.

## 3. Bucket batcher

Lives in `TRenderer` ([src/renderer.h:296](../../src/renderer.h#L296)) next
to the existing `tile_queue` / `overlay_queue` / `mesh_queue`. A new
`fx_queue` collects submissions during the frame; `DrainFxQueue()` runs at
the tail of the FX pass (§5).

**Match key**: `SFxBatchKey{ texture, pipeline_id, blend, depth_mode }`.
Two adjacent draws coalesce iff their keys are equal.

**Sort policy**: billboards + particles are sorted back-to-front by
camera-relative `world_pos` projected onto camera-forward, stable within
equal sort_z so per-bucket emit order is preserved. Strips emit in
submission order, after billboards/particles. Phase 1 picks per-bucket
sort, not global — see open questions.

**Memory shape**: SoA per bucket, materialized during drain:

```cpp
struct SFxBucketScratch {
    SFxBatchKey        key;
    std::vector<float> pos_xyz;       // 3 floats per instance
    std::vector<float> size_wh;       // 2
    std::vector<float> uv_xywh;       // 4
    std::vector<float> color_rgba;    // 4
    std::vector<float> rotation;      // particles only
    int32_t            count = 0;
};
```

One shared dynamic `sg_buffer` per pipeline, rebuilt-per-frame, drawn with
one `sg_draw(..., num_instances=count)` per bucket. Capacity is preserved
across frames — mirrors how
[`tile_queue`](../../src/renderer.h#L661) works today.

Alternative considered: per-bucket persistent `sg_buffer`s. Rejected
because Phase 1 has ~10s of buckets in flight, not hundreds, and the
shared-pool model matches the existing tile path.

## 4. Sokol pipeline objects

Three pipelines, each with its own shader pair. Shaders live in
[src/shaders/](../../src/shaders/) per the backend-fanout convention
documented in
[RENDERER_ARCHITECTURE.md](../RENDERER_ARCHITECTURE.md#shaders--backend-selection).

All three share one UBO `fx_camera` carrying `view_proj`, `camera_right`,
`camera_up`, `camera_pos`, `time_seconds`, `viewport_size_px`.

### 4.1 fx_billboard

Static 4-vertex corner VB; per-instance VB carries `float3 world_pos,
float2 size_wu, float4 uv_rect, float4 color_rgba, uint debug_mode`.

Vertex shader: build screen-aligned right/up from the camera basis; offset
by `corner * size_wu`; compute fragment depth in scene-z space so depth
test against `scene_z_target` works.

Fragment shader: sample atlas; multiply by `color_rgba`; branch on
`debug_mode` (§6). A soft-particle alpha-fade hook is reserved against
`scene_z_target` for Phase 2.

### 4.2 fx_particle

Same shader family as billboard plus a `float rotation_rad` per-instance
input that rotates `corner` before the right/up expansion.

Cheapest implementation is one shared pipeline with rotation set to 0 for
billboards; Phase 1 picks two pipelines so each can evolve independently
(particles will diverge first on soft-particle math). Cost: one extra
`sg_pipeline` object.

### 4.3 fx_strip

Different topology: each segment becomes a camera-aligned quad. No
instancing — CPU expands segments into a dynamic VB at submit time. Per
expanded corner:

```
float3 world_pos
float3 world_tangent    // (B - A)
float  half_width_wu    // signed: -w on one edge, +w on the other
float2 uv               // (u, 0) or (u, 1)
float4 color_rgba
```

Vertex shader: `side = normalize(cross(tangent, view_dir))`; offset
position by `side * half_width_wu`; pass uv + color through.

Fragment shader: sample, multiply by color, branch on `debug_mode`.

## 5. Integration point in the existing frame

[`TMapRenderer::RenderFrame`](../../src/maprenderer.cpp#L2569) today:

1. `Renderer->BeginTilePass(...)` ([src/maprenderer.cpp:2943](../../src/maprenderer.cpp#L2943))
2. `ParticleManager().BeginDrawPulsePass()` + per-instance `Submit()` walk ([src/maprenderer.cpp:2988-3012](../../src/maprenderer.cpp#L2988))
3. `SubmitParticleBillboards(...)` ([src/maprenderer.cpp:3013](../../src/maprenderer.cpp#L3013)) — **the shim we delete**
4. `Renderer->EndTilePass()`
5. `Renderer->RunLightingPass()`

Phase 1 changes:

- Effect components call `Renderer->SubmitFxBillboard` /
  `SubmitFxParticleBucket` / `SubmitFxStrip` from inside their existing
  `DrawPulse()` and per-instance Submit walk. **No new walk** — they
  piggyback on the loop already at [src/maprenderer.cpp:2989](../../src/maprenderer.cpp#L2989).
- `SubmitParticleBillboards` ([src/maprenderer.cpp:370](../../src/maprenderer.cpp#L370))
  is deleted; its work moves into `TRenderer::DrainFxQueue`.
- FX consume scene depth and write `lit_target`, so drain happens **after**
  `RunLightingPass()` in a new pass `fx_pass` (color = `lit_target`, depth
  read-only against `scene_z_target`). This slots into the Transparent FX
  pass slot already reserved at row 5 of the
  [pass table](../RENDERER_ARCHITECTURE.md#per-frame-pass-pipeline).
- LS-pipeline effects call `Renderer->AddPointLight` from the per-instance
  Submit walk **before** `BeginTilePass`, in the existing
  `point_lights_ms` phase at
  [src/maprenderer.cpp:2940](../../src/maprenderer.cpp#L2940). Lights are
  cleared per frame; components re-add each frame.

LS-pipeline effects therefore submit twice per frame: once before tile
pass (point-light add), once during the Submit walk (any visible flare
billboard).

## 6. Diagnostic ladder plumbing

Per [PARTICLE_EFFECTS §3.2](../PARTICLE_EFFECTS.md#32-diagnostic-ladder)
every component owns an `EFxDebugMode`. The mode reaches the shader
**per-draw, via the instance buffer** — not as a pipeline variant. Reasons:
pipeline variants 4x the pipeline count for negligible benefit, and
per-instance lets the `--test=vfx` browser highlight one effect at a time
inside a shared bucket.

| Mode | Shader behavior |
|------|-----------------|
| `Normal` | Sample atlas at `uv_rect`, multiply by `color_rgba`. |
| `SolidColor` | Skip sample; output `color_rgba`. Caller forces a known-good color. Validates submission + transform + size. |
| `FullTexture` | Sample at `uv = corner + 0.5`, ignoring `uv_rect`. Validates texture binding + blend. |
| `CurrentFrame` | Sample at `uv_rect` but force `color_rgba = white`. Validates UV math. |

Components expose `SetDebugMode(EFxDebugMode)`. The current
`SetDebugSolid(bool)` at [src/effect.h:91](../../src/effect.h#L91) is the
placeholder; see §7. `--test=vfx` twiddles the mode per effect per the
harness spec ([TEST_MODE.md](TEST_MODE.md)).

## 7. TFlipbookBillboardComponent migration

The component already holds everything the new path needs
([src/effect.h:70-163](../../src/effect.h#L70)). The migration is small —
add `EFxDebugMode debug_mode`, a `Submit(TRenderer&, const TObjectInstance&)`
that fills out an `SBillboardDrawItem` from existing members and calls
`Renderer->SubmitFxBillboard(item)`, and drop the cruft below.

Concretely:
- Replace `debug_solid` bool / `SetDebugSolid` ([src/effect.h:91](../../src/effect.h#L91)) with `EFxDebugMode debug_mode` + `SetDebugMode`.
- Drop `replaces_default_visual` plumbing once the FX layer is the only consumer.
- Drop `SourceX/Y/Width/Height/TextureWidth/TextureHeight`
  ([src/effect.h:104](../../src/effect.h#L104)) — they exist only to feed
  the `SOverlaySubmit` pixel-rect path. UV is normalized end-to-end via
  the existing `UvRect(out[4])` helper at [src/effect.h:136](../../src/effect.h#L136).
- Update the FLAME builder at [src/effect.cpp:584](../../src/effect.cpp#L584) accordingly.
- Replace the Billboard branch in [src/maprenderer.cpp:1129](../../src/maprenderer.cpp#L1129)
  with `flipbook->Submit(*Renderer, *oi); return;` — deleting the entire
  screen-space projection block inside it.

## 8. Exit criterion for Phase 1

- `Renderer->SubmitFxBillboard` / `SubmitFxParticleBucket` / `SubmitFxStrip` exist and route through the new `fx_pass`.
- `fx_billboard`, `fx_particle`, `fx_strip` sokol pipelines compile and run on Metal and GL.
- `TFlameEffect` renders via `SBillboardDrawItem`; the legacy `SubmitParticleBillboards` shim is deleted.
- At least one PE-pipeline test effect (smoke or sparks) draws from a `TParticleBucket`.
- At least one SR-pipeline test effect (a static ribbon hard-coded in `--test=vfx`) draws.
- LS-pipeline: at least one effect that re-adds a `Renderer->AddPointLight` each frame and lights the ground correctly.
- `EFxDebugMode::SolidColor` works for every Phase-1 component and shows a known-good color quad at the owner position.
- `--test=vfx` browser shows the torch flame plus the three test effects; arrow keys cycle.

## 9. Resolutions (was: open questions)

Architectural decisions for Phase 1. Each came up while drafting this doc;
revisited if Phase 2 evidence contradicts.

- **Particle sort granularity → per-bucket.** Stacked additive+alpha mixing is acknowledged as a known limitation. Revisit only if a Phase-2 effect produces visibly wrong ordering; the fix at that point is a global sort across the FX queue, not a redesign.
- **`fx_pass` blend with `lit_target` → straight blend over lit color.** No read-back of `lit_target` for underglow / refraction in P1. If a Phase-2 effect (e.g. fire heating its own smoke) needs it, that lands as a separate pass.
- **MP-pipeline → defer, do not stub.** No empty `TFxMeshComponent` placeholder. First Phase-2 agent on `TMissileEffect` / `TFireBallEffect` / `TIceBoltEffect` adds the wrapper at that point, modelled on the existing `SMeshSubmit` path. Phase 1 ships FB + PE + SR + LS only.
- **Effect update tick → P1 lands it if missing.** Audit [src/object.h](../../src/object.h) and `TObjectComponent` plumbing in [src/effect.h](../../src/effect.h) first. If the `RegisterUpdate` / global pulse list from [PARTICLE_EFFECTS.md §2.3](../PARTICLE_EFFECTS.md) is not already present, it ships as part of Phase 1 — Phase 2 fan-out depends on it being there.
