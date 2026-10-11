# Explicit original prelit RGBA5 helper

EnergySpray's original FVF_LVERTEX triangle colors contain alpha as well as
RGB. Scene8 does not make its untextured raster opaque: all four packed bytes
are truncated to five bits. For each RGB565 channel, the actual kernel returns
`D - floor(D*(A5+1)/32) + floor(S*(A5+1)/32)`. Green's source channel is twice
its RGB5 input. Alpha0 still contributes 1/32; it is not a discard or color key.

The new default-false `SHelperMeshSubmit::retail_prelit_rgba5` is an explicit
vertex format, admitted only with mode4 and software lighting. The existing
normal attribute carries literal RGB5 and uv.x carries literal alpha5; it does
not reinterpret any existing normal-lit draw. Internal shader mode5 preserves
the original source contribution and alpha. Existing untextured cull-none,
LESS_EQUAL, depth-write and per-triangle flat-depth policies are reused.
General destination RGB565 rounding, edge interpolation and GPU framebuffer
precision remain open. No camera, uniform layout, default lighting, existing
mode4 output or default blend policy changes.

`prelit_rgb5_probe.py` executes 384 original raster cases (all32 alpha values,
four colors, three backgrounds), plus nearer/behind/equal-depth controls.
`TPrelitRGB5_DIAGNOSTIC` independently submits literal source records through
the actual Metal helper. Eleven pixel/depth controls pass, with at most one
8-bit output step from RGB565 conversion. A flag-false draw uses the previous
normal-lighting path. `test_helper_uv_submission.py` compiles the actual emitter
and checks internal mode selection, existing uniforms and default pipelines.

The optional EnergySpray palette in the existing ARGB diagnostic checks seven
literal texture words from its own asset. Original zero/white input colors
produce identical pixels; 0x0fff remains nonzero, preventing an alpha0 key.
Existing ARGB diagnostic output is byte-identical to its prior full image.

Evidence under `research/vfx-wide-20261010/hard-astra`:
`energy-prelit-native-final2`, `energy-argb-native-final`,
`energy-prelit-gpu-final2/review.json`, `energy-argb-gpu-final`, and
`energy-shriek-default-final`. GPU controls share binary SHA256
`0897cd4317011b85201dcd29eec24c49e61bb20cebdb5efffde14a30d46dbc30`.
MSL executed; GLSL/HLSL source branches mirror it but their devices were not
executed. A final HLSL-only variable spelling correction (`i.normal`) follows
that capture; it does not alter the executed Metal branch.

Current same-source Metal replay uses binary
`e1647e36b68d3ae3d8ec6e79fdce53bdb11203f54ecbb1df77bb35f16c1712ae`
and `energy-prelit-gpu-final2-v2/review.json`, SHA256
`c91b55d383212ae7051b8ff62f048e3553b598ce2bc0fe4b1dd49dc20e68770d`.
All three diagnostic films are byte-identical to their prior captures; all11
prelit/depth and7ARGB assertions pass after the HLSL source spelling fix.
