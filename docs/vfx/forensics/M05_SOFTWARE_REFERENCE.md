# M05 Mist — software reference blend audit

2026-10-04. **The fresh software A/B exposes a device-path difference, not
evidence for an empirical Mist tint.** Retail draws dim grey wisps while the
current bespoke port accumulates bright white cores. The original software
renderer ignores the requested source/destination blend modes for ARGB4444
textures and uses a fixed alpha-over rasterizer. The port requests unweighted
additive RGB. Those operations produce substantially different results when
50 particles overlap.

Status: cause identified from snapshot source and its compiled software
library, consistent with the native retail observation. Exact equivalence of
the snapshot rasterizer to the final retail implementation remains unproven.
No canonical Mist blend, geometry, texture, position, scale or tint was changed
by this audit. Full Mist visual acceptance remains open.

## Native and port evidence

The native reference is
`~/RevenantRetailLab/captures/runs/sw-burndown-01-mist/`:

- `retail.avi` SHA-256:
  `20326c484b456d2116852a3587c7b5c7133db43dbc2de64ae18c58563d1817cf`.
- Software renderer, neutral RGB ambient 32, camera `(10000,10000,16)`,
  Mist position `(10000,10000,16)`, nine ground plates.
- 59 distinct effect-region frames; outside-region maximum mean error
  1.6425. The recording retained one responsive retail process before/after.
- The decoded stream is RGB565. This is a native emulator recording, not
  a photograph of the Mac window.

The first comparison is
`~/RevenantRetailLab/captures/ab/sw-burndown-01-mist-bespoke/`.
`comparison.png` shows grey retail wisps versus white port bubbles. Its
manifest records a single estimated phase offset of 0.5667 seconds, no
position/scale fitting, and static ground. This first diagnostic has null
binary hash/command fields; use the parent's regenerated hashed capture for
reproducibility rather than promoting this manifest to acceptance evidence.
The engine preview comparison is separately retained at
`captures/ab/sw-burndown-01-mist-engine/`.

Phase search cannot correct a blend-device difference. Its background-heavy
mean image error is not a fidelity percentage or an acceptance score.

## Actual shipped asset

`data/imagery.rvi:Imagery/Magic/mist.i3d` is 8,916 bytes, SHA-256
`bcddd7b7d1b8c7513c68a3d618499f3ceaeed52e544a43e349593fc294759ef9`.
The existing `--dumpi3d` loader was used to inspect it; the completed dump is
at `/tmp/revenant-mist-evidence/` on the development Mac.

- One object, `photon01`, four vertices/six indices, one material/texture.
- Authored bounds approximately `(-4.95794,23.3803,-4.95794)` through
  `(4.95796,23.3803,4.95795)`.
- Material diffuse `(1,1,1,1)`, emissive `(1,1,1)`. There is no grey material
  multiplier missing from the port.
- One 64×64 ARGB4444 texture. Decoded RGB ranges from 0 to 170, equal in all
  three channels. Alpha independently spans all 16 values from 0 to 255.
  For example, decoded pixels include `(51,51,51,102)` and
  `(153,153,153,255)`; RGB and alpha are not interchangeable.

The port's texture loader currently applies a black-background transparency
heuristic in `src/3dimage.cpp`, but does not multiply this texture's remaining
RGB by alpha. The texture has an actual authored alpha channel. The dim grey
source RGB should not be replaced, whitened or tinted to compensate for the
requested blend operation.

## Requested hardware state versus actual software draw

`src/effect_old.cpp:TMistAnimator::Render` calls `SetAddBlendState`, takes
object 0, applies scale 0.7 and the two source Z rotations, translates each
live particle, and calls `RenderObject`. The helper requests:

```text
TEXTUREMAPBLEND = D3DTBLEND_DECALALPHA
SRCBLEND       = D3DBLEND_ONE
DESTBLEND      = D3DBLEND_ONE
ZENABLE        = true
ZWRITEENABLE   = false
```

The port's `TMistEffect_Bespoke` retains raw corners/UVs and requests
`EFxBlend::AdditiveStraight`, `Unlit`, `TestNoWrite`. `SetupFxBlend` in
`src/renderer.cpp` maps that to ONE/ONE. The FX shader samples RGB directly;
the current path does not reproduce the old texture-stage DECALALPHA
operation. That hardware texture-stage question remains separate from the
fixed software path described below. A call to `SetAddBlendState` alone is
insufficient proof that every device produced the same pixels.

The snapshot's native software source is
`/Users/benjamincooley/projects/Revenant/blue/swscene.cpp`:

1. `SetRenderState` at lines 286–333 stores `_textureMapBlend`, `_srcBlend`
   and `_destBlend`. `GetRenderState` returns them for state restoration.
2. Those variables have no draw-time use anywhere in that file.
3. The ARGB branch at lines 613–666 explicitly says “no lighting” and chooses
   `DrawTextureZbuffer4444WOff` when depth is enabled and writes are off.
   It passes vertices/UVs/texture/depth, with no blend-state or material-color
   argument. Other depth combinations choose siblings of the same fixed
   ARGB rasterizer.
4. The non-alpha RGB565 path is separate and does modulate vertex lighting.
   Ambient changes therefore need not affect these ARGB mist particles.

## Compiled software rasterizer confirmation

The implementation bodies are retained in the snapshot library
`/Users/benjamincooley/projects/Revenant/blue/Release/BlueRev.lib`, SHA-256
`d940a28fe245a5e1fc6d799f5770565d377e84fbca53e6c65a7a55086f84f62a`.
They were inspected with the Command Line Tools `llvm-objdump`; the
disassembly is retained at `/tmp/revenant-mist-sw-raster.asm`.

In `DrawTextureZbuffer4444WOff`, offsets `+0x5e7` through `+0x630`:

- Fetch one 16-bit texel using the interpolated integer texture coordinates.
- Separate RGB12 with mask `0x0fff` and alpha4 with mask `0xf000`.
- Convert RGB12 through `argb4444table` into the destination 16-bit format.
- Look up scaled source and scaled existing destination using the alpha
  table, subtract the latter from the former, add the existing destination,
  and store the resulting 16-bit pixel.

`C3GColorTable::MakeTable` builds 32 scale slices. Its RGB565 branch scales
each channel by the slice index divided by 31, with integer truncation.
The alpha4 index selects slice `2*alpha4`. The conceptual operation is:

```text
out565 = destination565
       + table[2*alpha4, converted_source565]
       - table[2*alpha4, destination565]
```

Thus it is approximately source-alpha-over, with effective alpha
`2*alpha4/31` and separate integer truncation of the two scaled colors.
Even alpha4=15 selects 30/31, not an exact fully opaque operation. The source
RGB4444-to-RGB565 conversion also expands nibble values by shifts rather than
the port's normalized 8-bit decode. Fixed-point UV rounding, depth/culling,
triangle coverage and particle submission order matter to pixel-level parity.
These details are not claimed to be reproduced by the current GPU preview.

This explains why summing many grey sprite RGB samples can produce bright
white port cores while the software reference remains a grey alpha composite.
It does not justify replacing all additive effects with alpha globally.

## Completed gates and remaining work

Completed source/asset gates: shipped Mist asset identified; authored quad,
UVs and white material established; literal particle initialization,
integration order, transforms and 24 Hz accumulator implemented; animated
port and native software output retained.

Geometry is close in this isolated diagnostic, but neither complete geometry
nor timing parity is accepted: RNG is unsynchronized, camera/model-Z parity
is shared work, and the native implementation's final parameters are not all
recovered. Brightness, overlap and blend parity are explicitly unaccepted.
The engine Mist preview also retains billboard geometry and cannot inherit
the bespoke raw-quad gate merely because it shares dynamics.

## Explicit Alpha diagnostic completed

`TMistEffect_SOFTWARE_ALPHA_DIAGNOSTIC` now selects GPU straight `Alpha`
(SRC_ALPHA/INV_SRC_ALPHA) while keeping the bespoke art, state, randomness,
geometry and transforms. The canonical `TMistEffect_BESPOKE` continues to
request authored additive blending. This is an approximation to the recovered
software operation, not an accepted replacement for it.

Both previews were rendered with seed 1, a 1/60-second simulation step,
150 frames sampled at 30Hz, one warmup tick, and the same effect-free native
backdrop. Retained manifests, commands, definitions and media hashes are in:

- `~/RevenantRetailLab/captures/ab/sw-mist-blend-20261004-canonical/`
- `~/RevenantRetailLab/captures/ab/sw-mist-blend-20261004-alpha/`

Both used binary SHA-256
`d2d40ffb215db8add4659a493229886a58299260369cccedcad97273a32ca637`.
Each outside-effect background check is exactly zero after the explicitly
recorded top-12-row FPS exclusion. The Alpha preview shows dim grey wisps
closer to retail than the additive preview's bright cores. Diagnostic ROI
mean errors are 1.366 versus 3.113, with independently estimated global
phase offsets of 0.767 versus 0.567 seconds. These background-heavy scores
and unsynchronized particle streams do not establish a fidelity percentage
or timing acceptance; the elapsed-time videos are retained separately.

For exact software acceptance, implement or exercise the fixed RGB565 table
operation and nearest sampling, then verify native single-sprite and overlap
cases. Residual texture filtering, coverage and quantization remain open.

Capture provenance must always state the retail renderer and the port
comparison semantics. Use software references now to verify behavior,
authored placement, lifecycle and software appearance. Do not interpret a
software/hardware compositing mismatch as wrong art or retune the effects to
erase it. Hardware acceptance requires a trustworthy device reference and
the DECALALPHA texture-stage audit; emulator OpenGL/depth work remains held.

## Thin native lifecycle and authored pixels, 2026-10-07

The [thin Mist report](../../../recon/retail_asm/runtime/effects/mist-frontend-ab/manifest.json) runs original Initialize `0x4f2260`, Animate `0x4f2400`, Render `0x4f2560` and observes actual range-RNG calls without replacing them. It compiles current shared preview/map initialization state, Advance/Submit and matrix code with explicitly supplied asset/owner/RNG providers. All fifty drop position/velocity/dead fields agree exactly through ticks 0–96, including repeated death and respawn and 1,380 random calls. No production arithmetic or visual parameter change was needed.

Eight discrete-tick geometry/UV/software image/depth comparisons repeat twice and match exactly; maximum corner error is under `1.95e-6`. Both frontends feed the same original projection/raster with the unconverted shipped ARGB4444 asset. The source additive request `(8,1)` is asserted; the original software device's own ARGB compositing executes unchanged. This verifies the canonical source frontend under the controlled original software backend and does not validate an empirical GPU tint or the optional alpha diagnostic.

The selected reference has identity owner, face zero, white vertex input, a fixed declared camera, and explicit base-animator/extents/imagery submission boundaries. Real map/component loading, arbitrary world/face transforms, natural context/lighting/culling and Metal remain separate. The historical recordings and blend audit retain their own narrower evidence; see the [thin probe instructions](../../../recon/retail_asm/runtime/effects/mist-frontend-ab/README.md).
