# D3D11: map tiles render as a flat color on some systems (albedo G-buffer content wrong)

## Summary

On one Windows 11 machine (NVIDIA, driver 566.xx-era, genuine `d3dcompiler_47.dll` 10.0.19041.3636), the D3D11 backend renders the entire tile world as a single flat color (first a violet fill, after unrelated rebuilds black), while characters, effects, HUD, menus and icons render correctly. The same build behaves differently between runs without code changes, which suggests a GPU/driver-side instability interacting with the image-pair texture path rather than a deterministic logic bug.

## What I verified (symbols + cdb stacks + pixel probes)

- The two earlier crashes on this machine (title screen AV in `sg_apply_pipeline`, level-load AV in the lighting pass) were caused by the D3D11 runtime shader compiler failing on:
  1. `src/shaders/composite.hlsl.h` — the pixel shader used `chroma_key` with no `cbuffer` declaration (fixed on main now);
  2. single-argument splat constructors `float3(dot(base, lw))` in `lightmodel.hlsl.h` and `float2(size)` in `fx.hlsl.h` / `mesh.hlsl.h` — the d3dcompiler on this machine returns `E_FAIL` with an **empty** error blob for `float3(scalar)`, even though the construct is standard HLSL. Rewriting them as explicit components/casts made the shaders compile.
- With the shaders compiling, `InitLightPipeline` reports `sg_query_shader_state(light_shader) == SG_RESOURCESTATE_VALID`, the level loads and runs.
- The tile bitmaps themselves decode correctly on the CPU: I dumped the first opaque RGBA of 12 different uploaded tiles — sane stone colors (e.g. `144,152,160,255`), formats chunked 8-bit + z-buffer, and `RegisterImagePairAsset` succeeds (`SG_RESOURCESTATE_VALID` on all three images of the pair).
- Forcing the tile pixel shader to a constant color floods the screen with that color through the whole deferred chain — so the submit path, G-buffer targets, lighting pass, and final composite all work.
- But with the real shader, sampling `color_tex` returns a flat color everywhere (pixel probes across the viewport: `72,59,95` ± tiny variation), while the same texture's CPU bytes are the correct tile art. The alpha channel comes back 255 (matching the CPU alpha), the RGB is flat.
- A debug probe of the tile FS's cbuffer (`rect`, and `in_.uv` via the FS output) shows nearly-constant values across the whole viewport (`in_.uv ≈ (0.75, 0.80)`, `rect ≈ (-0.21, -0.01)`), which looks like the per-draw constant buffer update (or the constant-buffer binding) is not reaching the GPU for the tile pipeline — every draw appears to reuse one stale uniform set.

## Suspicions (not confirmed)

- `sg_apply_uniforms`/constant-buffer update for `tile_pipeline` on D3D11, or the SRV/constant-buffer reflection mapping for the 12-vec4 block, misbehaves on this driver.
- The machine's d3dcompiler failing silently on legal HLSL (empty error blob + `E_FAIL`) makes me suspect a system-level shim/driver issue rather than sokol logic; but the flat-color result persisted across many rebuilds and reboots-adjacent runs, so I can't rule out a real backend issue.

## Reproduction

Build with MSVC on the affected machine, run `Revenant.exe --quickstart`, enter The Keep: world area is a single flat fill; characters render normally. `REVENANT_VFS_TRACE=1` confirms all tile data comes from the stock install packs.

Happy to test candidate fixes or provide cdb stacks / dumps on request.
