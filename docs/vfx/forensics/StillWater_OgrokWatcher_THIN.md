# StillWater and OgrokWatcher texture-frame verification

The original reference executes the retail PE in the thin runtime. These four
assets share a single authored quad and a constant state-0 geometry track; the
watcher textures animate. This is a bounded frontend and texture-selection
proof, not complete effect or independent graphics-device acceptance.

## Exact asset contract

`tools/retail_runtime/static_additional_profiles.json` pins the complete asset
SHA256 and exact header cardinality for each retail type:

- StillWater `0xadbcef14`: `Misc\StillWater.I3D`, one `STILL` state, animation
  flags `0x2001`, one owner frame and one stored texture frame.
- ogrokwatcher1 `0xaeaeeb23`: one `STILL` state, flags `0x0001`, four owner
  frames and four stored texture frames.
- ogrokwatcher2 `0xaeaeeb24`: one `STILL` state, flags `0x0001`, four owner
  frames and four stored texture frames.
- ogrokwatcher3 `0xaeaeeb25`: one `STILL` state, flags `0x0001`, four owner
  frames and six stored texture frames. Frames 4 and 5 are stored but are
  outside the active four-frame `STILL` loop; they receive no animation credit.

Every active frame is tested. Each watcher has four distinct active reference
images. All six stored watcher3 frames have distinct raw hashes. The frame
arrays contain relative OFFSET entries followed directly by raw 16-bit pixels;
a preceding value `4` in a single-frame array is its offset, not a compression
chunk identifier. The profile reader rejects a changed state name, flags,
owner-frame count, texture-frame count or active-frame list.

## Original texture-selection rule

[Original RenderObject](../../../recon/classes_converted/cls_0x5a486c.cpp)
`0x40a8f0` and actual instructions at `0x40ad28..0x40ad4f` distinguish three
paths. The matching source is
[RenderObject](../../../src/3dimage.cpp) and the flags are in
[3dimage.h](../../../src/3dimage.h).

Texture-face slot `t` is one-based: `t=0` is untextured, and `t=1` addresses
imagery texture 0. For a textured slot without `OBJ3D_TEX (0x200000)`, native
code chooses `animobj.textureframe[t]` when `OBJ3D_TEXFRAME (0x100000)` is set;
otherwise it uses the caller's owner frame. It then calls
`SetTextureFrame(t-1, selected_frame)` at `0x40c520`. The native override array
is at object `+0x1b8+t*4`, so texture 0 uses its slot-1 entry.

With `OBJ3D_TEX`, normal imagery frame selection is bypassed. The native draw
reads the object's explicit texture/surface arrays at `+0x23c+t*4` and
`+0x2c0+t*4`. The source draw similarly reads `htextures[t]`. The source's
`NewObject` initialization writes `htextures[c]` with zero-based `c`; this
initialization/draw indexing difference is a separate override-path audit.
These four profiles use ordinary flags and do not exercise that override.

The actual native `0x40c520` branch with `copyframes=false` selects the surface
and texture from their per-frame arrays, writes current surface `+0x90` and
texture `+0x94`, and stores the current frame at `+0x80`. The fixture supplies
opaque per-frame identities and tests those writes without replacing this
function. A one-frame texture retains its initial binding. Requests are the
active owner frames only; the native boundary at `frame == texture_count` is
not tested or assumed safe.

Imagery is shared between owners. Its selected current handle is transient
state for a draw, not a stable albedo for an asset cache. A mesh asset first
loaded for owner frame 0 must not keep that handle forever. The draw/material
key must retain the selected texture binding for the specific owner/subobject
and draw frame. Mutating shared `SetTextureFrame` state alone cannot represent
two live owners at different animation phases.

## Retained proof and replay

Run from the merged checkout with Python and Unicorn available:

```sh
python tools/retail_runtime/static_texture_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output <private-report-directory>
```

The probe executes original `0x40c520`, `0x40a420`, `0x409950`, `0x43aa90`,
`0x43ad80` and software raster `0x56d960`. The candidate compiles actual
production key decoding, mesh extraction and generic `SubmitMesh` bodies.
The explicit fixture has a 512×512 viewport, camera `(0,0,0)`, white vertex
lighting, no culling, depth test/write and owner positions `(0,0,0)` and
`(16,-8,4)`. It compares topology, corners, UVs, texture identities and complete
RGB565/depth buffers, then repeats each from the same warm checkpoint.

The current merged report is retained under local lab
`research/vfx-next-20261007/static-texture-merged-f049de4-uv/manifest.json`.
Probe/profile SHA256, compiled source/binary hashes, production source-span
hashes, whole-asset hashes, raw frame hashes and image/depth hashes are included.
The test covers 52 native selector checks, 1,040 corner/UV checks, 26 image/depth
pairs and 26 exact warm replays. No DOSBox or guest boot is involved.

An earlier fixture changed an executable `MOV EAX, frame_count` stub between
one-frame StillWater and four-frame watchers. Unicorn reused translated code,
so some later reference frames incorrectly saw frame-count 1. This was a
fixture defect, not a port mismatch. The current metadata callback loads a
scalar from data memory; changing the value requires no rewritten instructions.
The runtime's `write_code` API separately supports intentional executable-byte
patches and cache invalidation.

## Port paths and remaining gates

The preview uses the shared authored mesh factory. StillWater's prior Water
bespoke dispatch is replaced by its actual mesh. The watcher previews use
registered per-frame meshes/textures, with the existing object `NextFrame`
loop at the preview's explicit 24 Hz. The original reference takes owner-frame
indices as fixture inputs; this does not prove natural original owner timing.

Normal port creation follows `TObjectClass::AddType/NewObject`: a type-specific
builder, then its class builder fallback. StillWater has a generic `TEffect`
builder; the watcher types use the generic EFFECT class fallback. Animator
lookup tries the type and then class and returns the default `T3DAnimator` when
no custom builder matches. Normal actor `NextFrameObjects/PulseObjects` and
map mesh submission must preserve the owner frame and its corresponding texture
binding. The root agent's normal-map command/capture report is separate from
this source/CPU proof and is responsible for crediting that dispatch test.

Lighting, culling, ownership/attachment variations, natural original caller
cadence, texture-copy/device paths and independent modern GPU image parity
remain open. A successful preview or normal-map create/frame/delete test does
not mark any of these rows fully accepted.
