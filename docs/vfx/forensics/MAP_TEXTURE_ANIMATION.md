# Animated textures on map meshes

The StillWater/watcher fixture executes retail `SetTextureFrame` at `0x40c520`
and compares its selected texture handle with compiled production submission.
StillWater has one active frame. Each watcher loops four owner frames;
watcher3 stores six texture frames, of which this state addresses four.
See `tools/retail_runtime/static_texture_probe.py` and its exact asset profiles.

The original render contract selects a texture for each object submission.
Actual retail instructions at `0x40ad30..0x40ad4f` confirm the one-based
texture slot, explicit handle override and texture-frame override rules.
`T3DImagery::RenderObject` in `src/3dimage.cpp` selects the current owner frame,
or `animobj->textureframe[t]` when `OBJ3D_TEXFRAME` is set. `OBJ3D_TEX` supplies
an explicit handle instead. `SetTextureFrame` wraps by the stored texture
count and, for `copyframes=false`, selects `framehtexs[frame]`. These are
texture frames, not UV atlas cells. Copy-to-surface animation remains a
separate unsupported branch in the port.

Normal-map reproduction on the merged game creates all four exact EFFECT
types through ADDAT, advances the actual map's NextFrame/Pulse, moves and
deletes them. All 40 command observations pass, including frames 1,2,3,0 on
the watchers. However, four successive GPU images are byte-identical before
movement. The map renderer registers a mesh with one albedo at cache build
time and reuses that handle; the renderer binds that cached albedo at draw
time. The standalone preview selects a frame-specific mesh and therefore
does not expose this runtime defect.

Repair the shared submission path: select the live instance's texture and
carry it with the draw. Keep cached geometry immutable. Opaque batches must
split on texture as well as mesh/culling; translucent depth and colour must
use the same texture. Explicit helper submissions need the same override.
Do not mutate the shared mesh's albedo: two owners of one asset can occupy
different animation frames in the same draw gather.

Explicit untextured overrides use the renderer's white texture, distinct from
the invalid sentinel meaning "keep cached albedo". Handle access is bounded
by `htextures[MAXTEXTURES]`; the separate `textureframe` array has one extra
slot. These edge cases are covered in the compiled submission regression.

The repaired normal-map capture passes all 40 observations, yields four
distinct stationary frames in each watcher's own nonoverlapping region, and
restores the initial floor exactly after deletion. Native registry constructors
and lookup were also executed for these four exact names: all select default
animator `0x5e8508`, with named FLAME `0x66ccc0` as a positive control.

Verification must include changing map pixels, wrap, create/move/delete,
two owners with different phases, and an unchanged floor after deletion.
This is controlled map integration; authored map/caller context and retail
versus Metal device fidelity remain separate acceptance gates.

The initial diagnostic lives under the local lab's
`research/vfx-next-20261007/map-static-textures/`; it is generated evidence,
not a repository asset. The floor-only module intentionally lacks dialog;
its dialog-list warning does not claim a story test.
