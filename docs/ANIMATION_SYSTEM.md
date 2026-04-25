# Animation System (Port Rewrite)

The retail animation system (described in [ANIMATION.md](ANIMATION.md)) was
built around bit-packed `SAniKey32` records that interleaved position,
rotation, and scale on a single timeline per sub-object, with
state-machine-driven transitions evaluated at render time. The port keeps
the retail data on disk and the retail decoder for backward compatibility,
but everything *above* the decoder has been rewritten as a channel-based
system that's friendlier to modern animation needs (blending, additive
layers, per-channel sources) and easier to feed from non-retail data.

This doc covers the rewrite. For the legacy retail system, see
[ANIMATION.md](ANIMATION.md).

## Core idea

Animation data is decomposed into independent **tracks**. Each track is
keyed by `(target, channel)` and stores its own keyframes. There is no
shared "pose record" -- a pose is just a sparse collection of channel
values, evaluated by sampling whichever tracks the clip provides.

Why this shape:

- A single sub-object's rotation can come from one source while its
  position comes from another (locomotion vs upper-body action).
- Layers blend per-channel cleanly: position channels can be overridden
  by a "force-position" layer while rotation channels remain driven by
  the base clip.
- New channels (alpha, tint, texture frame, UV scroll) drop in without
  changing the pose representation.

## Types

Defined in [`src/animsystem.h`](../src/animsystem.h):

```
AnimTrackId       -- packed (target u16, channel u16) -> u32
EAnimChannel      -- PosX/Y/Z, RotX/Y/Z/W, ScaleX/Y/Z,
                     Alpha, TintR/G/B, TextureFrame, UOffset, VOffset
EAnimTrackKind    -- Constant, Step, Linear
EAnimBlendMode    -- Override, Additive

SAnimKey          -- { float time, value }
SAnimQuat         -- { x, y, z, w } unit quaternion
SAnimTrack        -- track id + kind + key list; .Sample(time, dur, loop)
SAnimPose         -- sparse unordered_map<AnimTrackId, float>;
                     .GetQuat(target) recomposes RotX/Y/Z/W into a quat
SAnimClip         -- name + duration + loop + tracks; .Sample(time)
SAnimLayer        -- clip + time + weight + blend mode
```

The `target` half of `AnimTrackId` is by convention a 1-based sub-object
index; `ANIM_TARGET_ROOT` is reserved for the asset root. Channels
outside `EAnimChannel` are valid -- the value space is `uint16_t` and
custom channels are fine as long as readers and writers agree.

## Pose evaluation

`SAnimClip::Sample(time)` walks all tracks, samples each via
`SAnimTrack::Sample(time, duration, loop)`, and stuffs the values into a
`SAnimPose`. Tracks that don't produce a value at the given time
(empty, out-of-range, etc.) leave the channel absent from the pose --
consumers fall back to channel defaults (`RotW = 1`, `ScaleX/Y/Z = 1`,
`Alpha = 1`, everything else `0`).

`EvaluateAnimLayers(layers)` (in [`animsystem.cpp`](../src/animsystem.cpp))
samples each layer's clip at its own `time`, then blends per-channel:

- **Override** layers replace channel values weighted by their layer
  weight; the highest-weight override wins.
- **Additive** layers add their value (relative to channel default) on
  top of override results.
- Quaternion channels (`RotX/Y/Z/W`) are blended together as quats via
  `NlerpQuat` -- normalized lerp, hemisphere-corrected via dot. Each
  layer's four rotation channels are read as a unit, blended as a unit,
  written back as `RotX/Y/Z/W`.

This keeps blending a per-channel scalar operation with a quaternion
special case, instead of the retail system's "blend two whole packed
keys against each other."

## Hierarchy + matrix conversion

A pose is just channel values; turning it into a renderable matrix is
[`MatrixFromAnimPoseObject` / `BuildAnimPoseObjectMatrix`](../src/meshextract.h).
The hierarchy variant walks the imagery's parent chain at the requested
state, builds a local matrix per ancestor from its pose channels (T * R *
S), and concatenates upward. Output is row-major
[float[16]](../docs/RENDERER_ARCHITECTURE.md#submit--scheduled--drawn)
ready to drop into `SMeshSubmit::world`.

## Bridge to retail I3D data

Existing assets are still encoded as `SAniKey32` per
[`3dimagebody.h`](../src/3dimagebody.h). Rather than rewriting all
characters, the port samples the retail decoder into the modern pose:

- `SampleI3DAnimPose(img, state, frame)` walks the imagery's
  sub-objects, calls the retail `T3DImagery::GetAniKey(...)` for each,
  converts the Euler-XYZ rotation into a quaternion, and writes the
  pos/rot/scale channels into a `SAnimPose`.
- `SampleI3DAnimPose(img, state, frame, prevstate, prevframe)` does the
  same with retail's state-transition interpolation path.
- `BlendI3DAnimPoses(a, b, t)` blends two such poses, treating
  rotation channels as a quaternion unit (NlerpQuat) and other channels
  as scalars.

This bridge means retail content runs through the modern channel system
end-to-end -- there's no separate retail rendering path anymore. New
content (port-only assets, debug tools) can write
`SAnimClip`/`SAnimTrack` directly and skip the bridge entirely.

## What lives where

```
src/animsystem.{h,cpp}     -- Core types + sampler + layer blender
                              (no game data, no I3D, no rendering)
src/meshextract.{h,cpp}    -- I3D bridge: SampleI3DAnimPose,
                              BlendI3DAnimPoses, MatrixFromAnimPose...,
                              BuildAnimPoseObjectMatrix
src/3dimage.{h,cpp}        -- Retail decoder kept; GetAniKey(...) is
                              the only retail surface the bridge uses
```

The animation system has no rendering, audio, or game-state
dependencies. It's a pure data + math library.

## Testing

[`tools/test_anim_system.cpp`](../tools/test_anim_system.cpp) is a
standalone harness exercising track sampling, pose merging, and layer
blending without needing the full engine boot path.
