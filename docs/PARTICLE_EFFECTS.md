# Particle Effects Architecture

This document describes the new runtime particle/effect system. The goal is to
replace one-off ports of legacy D3D effect animators with a small, data-driven
engine that is easy to debug and hard to misuse.

The first production use case is the torch/flame effect, but the architecture is
intended to grow into smoke, sparks, ribbons, trails, spell impacts, water
mist, magic glows, and other Revenant effects.

> **Operational plan & burndown** for restoring every retail VFX through this
> engine lives in [vfx/README.md](vfx/README.md). The per-effect status table
> is [vfx/INVENTORY.md](vfx/INVENTORY.md). The `--test=vfx` harness is spec'd
> in [vfx/TEST_MODE.md](vfx/TEST_MODE.md). This file remains the **engine
> architecture spec** — the VFX docs are the operational layer on top.

## 1. Design Goals

The system must preserve these invariants:

- An object instance can be taken over by a render component.
- If a component replaces the default visual, the default drawable path must not
  also run.
- Every render component must be able to submit a known-good diagnostic
  drawable.
- "Nothing draws" should mean only one of:
  - the component was not attached
  - the drawable submit path was not reached
- Texture, UV, blend, and animation mistakes must degrade to visible debug
  output, not silent failure.
- The renderer should not know about specific legacy effect names such as
  `FLAME`. It should know only generic drawable/component types.

## 2. Ownership Model

### 2.1 Object Instances

`TObjectInstance` owns runtime-only components:

```cpp
TObjectInstance
  components: TPointerArray<TObjectComponent>
```

Components are not serialized. They are reconstructed from existing object
class/type/imagery definitions at load time.

### 2.2 Animator Registration

Legacy 3D animator registration is the bridge from old data to new components.
The old system selected custom effect behavior through:

```cpp
REGISTER_3DANIMATOR("FLAME", TFlameAnimator)
```

The new system keeps that registration point, but the builder also gets a
component-attachment hook:

```cpp
class T3DAnimatorBuilder {
public:
    virtual T3DAnimator* Build(TObjectInstance* oi);
    virtual void AttachComponents(TObjectInstance* oi);
};
```

`T3DImagery::AttachAnimatorComponents()` resolves the same builder as
`T3DImagery::NewObjectAnimator()` and calls `AttachComponents()`.

Example:

```cpp
class TFlameAnimatorComponentBuilder : public T3DAnimatorBuilder {
public:
    TFlameAnimatorComponentBuilder() : T3DAnimatorBuilder("FLAME") {}

    T3DAnimator* Build(TObjectInstance* oi) override {
        return new T3DAnimator(oi);
    }

    void AttachComponents(TObjectInstance* oi) override {
        TFlameEffect::AttachVisualComponent(oi, oi ? oi->GetImagery() : nullptr);
    }
};
```

This means the renderer never asks whether an instance is named `FLAME`. The
renderer asks whether the instance has a render component.

### 2.3 Update Registration

Components are not pulsed by default. A component opts into updates by
registering a member method:

```cpp
RegisterUpdate(&TObjectComponent::Update);
```

The global update list stores:

```cpp
struct SComponentUpdateEntry {
    TSafeComponentRef<TObjectComponent> component;
    TObjectComponentUpdateMethod method;
};
```

Each frame:

```cpp
if (auto* component = entry.component.Get())
    (component->*entry.method)();
else
    remove entry;
```

Dead components prune themselves because the update list stores safe component
refs, not raw pointers.

## 3. Render Components

### 3.1 Render Component Responsibilities

A render component owns:

- effect definition reference
- runtime effect state
- debug render mode
- update registration
- drawable submission state

A render component does not directly draw. It fills generic drawable
descriptors, which the renderer submits.

### 3.2 Diagnostic Ladder

Every render component should support these modes:

```cpp
enum class EEffectDebugMode {
    Normal,
    SolidColor,
    FullTexture,
    CurrentFrame,
};
```

The modes isolate failures:

- `SolidColor`: verifies component, transform, size, and submit path.
- `FullTexture`: verifies texture resource and blend path.
- `CurrentFrame`: verifies UV/source rect logic.
- `Normal`: verifies intended animation/material behavior.

The debug mode should be owned by the component. It should not be injected by
the renderer as an unrelated override.

### 3.3 First Component: Flipbook Billboard

The current proof component is:

```cpp
TFlipbookBillboardComponent
```

It represents one persistent world-space billboard with a flipbook atlas. It
is enough for the torch flame proof case. Later, the particle engine will use
the same channel names and bucket paths for many particles.

## 4. Buckets

Particles and billboards render through buckets. A bucket is a batch of
compatible draw items.

Bucket compatibility is determined by:

- drawable kind, for example billboard, ribbon, trail
- material/texture atlas
- blend mode, for example alpha, additive, lighten
- depth mode
- camera-facing mode

Example initial bucket:

```txt
bucket flame_billboard
{
    drawable = billboard
    blend = additive
    texture = flame_atlas
    columns = 4
    rows = 2
}
```

All torch flames emit into the same flame bucket. All quads draw from that
bucket.

## 5. Emitters

An emitter creates and updates particles. Buckets are orthogonal render/storage
targets; one emitter may write particles to several buckets, and several
emitters may write to the same bucket.

An emitter definition contains:

- spawn rate
- maximum particle count
- initial channel expressions
- lifetime channel expressions
- output bindings to one or more buckets
- deterministic seed policy

Example:

```txt
emitter torch_flame
{
    max_particles = 32
    spawn_rate = 12

    output flame_billboard
    {
        life = rand(0.35, 0.65)
    }

    emit_pos = owner_pos + vec3(rand(-1,1), rand(-1,1), rand(0,4))
    emit_vel = vec3(rand(-0.05,0.05), rand(-0.05,0.05), rand(0.3,0.8))
    emit_scale = rand(18, 26)
    emit_color = rgba(1.0, 0.55, 0.18, 1.0)
    emit_life = rand(0.35, 0.65)

    life_pos = emit_pos + emit_vel * age
    life_scale = emit_scale * (1.0 - age01 * 0.4)
    life_color = emit_color * fadeout(age01)
    life_frame = floor(age01 * 8)
}
```

## 6. Channel Naming

The engine uses named channels. Names should be stable across effects so
emitters, interpreters, and renderers can share code.

### 6.1 Owner Channels

These describe the source object or component:

```txt
owner_pos
owner_vel
owner_facing
owner_color
owner_scale
owner_seed
time
dt
```

### 6.2 Emit Channels

These are written once when a particle is created:

```txt
emit_pos
emit_vel
emit_accel
emit_color
emit_scale
emit_rotation
emit_frame
emit_life
emit_seed
```

### 6.3 Life Channels

These are recomputed or incrementally updated while the particle is alive:

```txt
life_pos
life_vel
life_accel
life_color
life_scale
life_rotation
life_frame
life_alpha
life_age
life_age01
```

### 6.4 Render Channels

Buckets read standardized render channels:

```txt
draw_pos
draw_color
draw_scale
draw_rotation
draw_frame
draw_uv_rect
draw_softness
draw_light
```

Many effects can directly map:

```txt
draw_pos = life_pos
draw_scale = life_scale
draw_color = life_color
draw_frame = life_frame
```

## 7. Expression Language

Effect definitions use string expressions rather than a complex serialized
node graph.

Example:

```txt
v1 = rand01()
emit_pos = owner_pos + vec3(rand(-1,1), rand(-1,1), 0)
emit_scale = 20.0 + 8.0 * v1
life_pos = emit_pos + emit_vel * age
```

The parser compiles expressions into bytecode. The original strings remain the
authoring format.

### 7.1 Supported Value Types

Initial value types:

```txt
float
vec2
vec3
rgba
int
bool
```

### 7.2 Operations

Initial operations:

```txt
add
sub
mul
div
mad
min
max
clamp
lerp
floor
fract
sin
cos
rand
rand01
noise
fadein
fadeout
vec2
vec3
rgba
```

The first implementation should support only what the torch needs. More ops
are added only when a real effect needs them.

## 8. Shared Subexpressions

Shared expressions are named assignments:

```txt
v1 = rand01()
emit_pos = owner_pos + 300.0 * v1
emit_scale = 200.0 * v1
```

Compilation treats `v1` as a named channel or temporary register.

Shared subexpressions can be optimized in two stages:

1. **Simple form**: compile `v1` as its own bytecode pass, store it in the
   computed buffer, and let later expressions read it.
2. **Fused form**: at startup, detect common expression patterns and replace
   multiple bytecode ops with one specialized block processor.

The engine does not need the fused form to be correct. It is a startup-time
optimization.

## 9. Computed Buffer

The computed buffer stores named channels for a block of particles.

Each channel has:

```cpp
struct SChannelLayout {
    EChannelType type;
    uint32_t offset_bytes;
    uint32_t stride_bytes;
    uint32_t component_count;
};
```

All channel offsets are computed at effect-definition compile time.

Example layout for a block of 64 particles:

```txt
pos_x[64]
pos_y[64]
pos_z[64]
vel_x[64]
vel_y[64]
vel_z[64]
scale[64]
age[64]
life[64]
frame[64]
color_r[64]
color_g[64]
color_b[64]
color_a[64]
v1[64]
```

The buffer is structure-of-arrays because every bytecode instruction runs over
many particles at once.

## 10. Block Execution

Bytecode is SIMP: single instruction, multiple particles.

The interpreter executes one instruction across a contiguous block:

```cpp
for (uint32_t i = 0; i < count; ++i)
    dst[i] = src0[i] + src1[i];
```

For this game, block size can often be the entire bucket or emitter list. A
reasonable default is:

```txt
block_size = 64
```

Small effects may run as one block. Larger effects can run in multiple
cache-friendly blocks.

### 10.1 Bytecode Shape

Example:

```cpp
enum class EParticleOp {
    LoadConst,
    LoadChannel,
    AddF,
    MulF,
    MadF,
    RandF,
    MakeVec3,
    StoreChannel,
};
```

Instructions reference channel offsets rather than names:

```cpp
struct SParticleInstruction {
    EParticleOp op;
    uint16_t dst;
    uint16_t a;
    uint16_t b;
    uint16_t c;
    float imm0;
    float imm1;
};
```

Names exist only during authoring/compile time.

## 11. Startup Compilation

At startup or effect load:

1. Parse effect def.
2. Parse expression strings.
3. Resolve channel names.
4. Allocate computed-buffer channels.
5. Compile expressions into bytecode.
6. Detect common subexpressions.
7. Optionally replace patterns with fused block processors.
8. Validate all render channels needed by the bucket exist.

Validation must report errors clearly:

```txt
torch_flame: missing draw_pos
torch_flame: emit_life must be scalar
torch_flame: life_frame must be scalar/int
torch_flame: texture flame_atlas missing
```

## 12. Flame MVP

The current flame component should evolve into:

```txt
effect TorchFlame
{
    debug = solid_green
    inactive_ttl_pulses = 12

    bucket flame_billboard
    {
        drawable = billboard
        blend = additive
    }

    emitter billboard
    {
        local_pos = [0.0, 0.0, 0.0]
        max_particles = 1
        spawn = persistent

        emit_pos = owner_pos
        emit_scale = 1.0
        emit_color = rgba(1, 1, 1, 1)
        emit_life = infinite

        life_pos = emit_pos
        life_scale = vec2(25.0, 62.5)
        life_frame = floor((time_frame % 18) * 11 / 24)
        draw_pos = life_pos
        draw_scale = life_scale
        draw_frame = life_frame
        draw_color = emit_color

        output flame_billboard
        {
            life = infinite
        }
    }
}
```

Phase 1 flame remains in `debug = solid_green` until the invariant is visually
confirmed.

Phase 2 switches to `debug = current_frame`.

Phase 3 removes debug mode and uses additive blending.

## 13. Renderer Contract

The renderer receives draw items from buckets. It does not know about effect
names or old animator names.

Billboard draw item:

```cpp
struct SBillboardDrawItem {
    S3DPoint world_pos;
    float width;
    float height;
    float color[4];
    int32_t frame;
    int32_t src_x, src_y, src_w, src_h;
    sg_image image;
    EBlendMode blend;
};
```

The renderer is responsible only for:

- world to screen/perspective projection
- batching compatible bucket items
- binding texture/material
- drawing quads

The effect engine is responsible for all semantic effect behavior.

## 14. Debugging Rules

Every effect component must be able to force:

```txt
debug = solid_green
```

That mode must use:

- the real component
- the real owner position
- the real bucket/submit path
- a known-good 1x1 material

If this mode does not appear on screen, the bug is in attachment, ownership,
projection, binning, or submission. It is not a texture/UV/blend bug.

This invariant is mandatory before adding normal material behavior.

## 15. Migration Strategy

Do not port legacy D3D animators one by one as bespoke C++ renderers.

Instead:

1. Identify the animator registration.
2. Attach an effect component from the animator builder.
3. Express the behavior as an effect definition.
4. Add only the missing generic engine capability.
5. Keep the old implementation in `effect_old.cpp` as reference only.

Examples:

```txt
FLAME       -> persistent flipbook billboard
Smoke       -> billboard emitter with fadeout
FireBall    -> emitter + trail + point light
RIBBON      -> ribbon/trail bucket
LightStrip  -> beam/trail bucket
FIRECONE    -> cone emitter
```

This keeps each migration additive and makes the engine more capable instead
of more special-cased.
