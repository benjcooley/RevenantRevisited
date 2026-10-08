# Cure white-rectangle correction: explicit Blue software helper mode

Current status, 2026-10-05: root built and executed the explicit helper mode on Metal. Final Cure isolated retail appearance passes, with 240 identical preview files to the prior reviewed pair and a retained zero-red GPU proof. Source/equation assertions below remain distinct from backend execution and full software raster/point-light parity.

The retained failed comparison is
`lab/captures/ab/sw-fps-cure-authored-preview-fix-20261005/comparison.png`.
Its white rectangles were a concrete helper shading error, not a difference
that can be accepted as uncertain normal lighting.

## Cause and original software behavior

Shipped Cure object 0 has white diffuse/ambient, specular RGB .9, emissive RGB
zero and power zero. Its RGB565 texture contains zero red in all 4,096 texels,
including 268 completely black texels. The former helper material branch adds
specular after sampling texture and forces the exponent to at least one. With
the authored Cure normal, source X/Z rotations, owner Z scale and the preview
light/view, that branch adds approximately .522 white per channel even to a
black texel. ONE/ONE overlap then saturates the quad footprint white. Moving
emissive inside a texture product alone would not fix Cure: its emissive RGB
is already zero.

The actual software source establishes a different path:

- `legacy/blue/swscene.cpp:435` Illuminate transforms/normalizes the vertex
  normal, accumulates ambient and active light contributions, and packs five
  bits per channel. It does not read material specular or emissive.
- `legacy/blue/swscene.cpp:659` RGB565 texture drawing selects texture plus
  Gouraud modulation; its write-off branch calls
  `DrawTextureAndModulationGoraudZbufferWOff`.
- `legacy/blue/swscene.cpp:286` SetRenderState does not implement
  SPECULARENABLE. It is not correct to derive SW specular from the .9 material
  coefficient or the power value. The hardware scene's SpecularEnable option
  is a separate control and must not be confused with this software path.

For general Direct3D context, specular is a separate enabled render state and
can be added after texture stages; it is not implied by material color alone.
[Microsoft render-state documentation](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3drenderstatetype).
The correction here is grounded in Revenant's own Blue software implementation,
not inferred from modern Direct3D defaults.

## Scoped backend change

`SHelperMeshSubmit.retail_lighting` defaults to zero. Mode zero retains the
previous helper material fragment branch byte-for-byte. Mode one explicitly
selects the software RGB565 path. Root selects it for the audited Cure asset;
no Cure material value, simulation parameter, geometry or color is changed.

All three helper shader backends (Metal, GLSL, HLSL) now:

1. Receive the existing retail ambient/directional environment in the vertex
   uniform block and the explicit mode in directional.w.
2. Calculate the same source normal/directional expression already used by
   the rigid mesh software mode; truncate to five-bit lighting **before**
   raster interpolation.
3. Forward that Gouraud light and the uniform mode through matching stage
   interfaces.
4. Read nearest wrapped texels directly, using the already implemented rigid
   mesh software addressing policy instead of changing shared texture samplers.
5. Return texture RGB multiplied by interpolated source lighting. Material
   specular, emissive, alpha and diffuse terms cannot inject color into black
   texture texels in this explicit mode.

The vertex descriptor/upload is nine float4 values (36 floats): the original
seven world/camera values plus retail ambient and directional/mode. The
original seven-float4 material fragment upload, mesh/texture binding, world
projection, blend/depth state and mode-zero formula remain unchanged. Unused
legacy fragment varyings were replaced by the matching new varyings, avoiding
Metal/HLSL stage-interface location mismatches.

This mode covers the existing ambient/directional source environment. Original
per-owner point-light selection is separate work; it is not replaced with a
new sun or fitted brightness here. The empty reference fixture has no such
positional-light contribution.

## Required matching environment

Before this correction VFX::Render set a bright modern light but never called
SetRetailMeshLighting, so a software helper would inherit its default/stale
retail environment. The matched reference uses ambient 32. Root integration
uses the existing explicit scene-ambient profile and
`RetailSoftwareMeshLighting(32, Ambient3D, native_color, UseDirLight,
DirLightPercent)`, passing its results through SetRetailMeshLighting. Native
Ambient3D, color and directional settings must be retained in the capture
provenance. There is no helper-specific sun/brightness constant.

## Regression evidence

Run `/Users/benjamincooley/RevenantRetailLab/check_helper_software.py`.
The source/equation checker passed 34,174 checks:

- 20,480 zero-red cases using the actual 4,096-texel Cure texture and five
  source lighting levels, plus 1,340 black-texel cases.
- Dense binary32 five-bit quantization comparisons against the original SW
  packed-channel arithmetic.
- All three backends' mode-one expressions and direct nearest wrapped reads.
- Exact mode-zero fragment-tail comparisons with the prior source version.
- Default mode, descriptor size, 36-float upload and environment/mode packing.

The report at `lab/research/cure/helper-software/source-regression.json`
records renderer/software-source/asset hashes and shader branch hashes.
This is a source/equation regression, not shader execution. The child did not
build, compile shaders, render or control the guest. Root's integration build
and fresh runtime/reference captures provide those additional gates. The
failed white-rectangle and initially invisible map captures remain retained;
they are not promoted to successful visual evidence by this source change.

## Final execution and appearance evidence

Root final binary `e829e8dbd53c8d4d26f7c82f21159147137d0905181d2591bf9cde7670764870` compiles/runs Metal and completes the Cure, FireFlash and FireCone helper pairs plus six real lifecycle captures. Cure's final 240 port frame files exactly equal the prior `sw-fps-cure-software-helper-20261005` files. That prior manifest stores root's isolated visual pass and `software-channel-review.json` SHA256 `36b6fbd4ec4e97c0fc5f181825261c3f1b022a931452b9e0cf1262ac4f37f2f5`, with red-channel max error 0 across240 frames. The [integrity audit](/Users/benjamincooley/RevenantRetailLab/research/ledger-reconciliation-20261005/cure-firecone-final-integrity.json) verifies transfer and all final evidence hashes.

This removes the concrete white-rectangle bug and passes Cure's overall isolated blue-glow appearance with independent RNG. FireFlash and FireCone color/visibility improve but their own visual reviews remain deferred (annulus-versus-filled cloud and bright dense core respectively). No broad hardware-material, GLSL/HLSL runtime, per-owner point-light or complete SW blend/raster parity is certified. Earlier white and all-ground failures retain no acceptance credit.
