# Optional original ARGB4444 source-contribution truncation

The original software alpha kernel converts the texture to RGB565 channels,
then evaluates `D - floor(D*a/16) + floor(S*a/16)`, where `a` is the alpha
nibble plus one. Existing GPU straight-alpha blending retained sub-quantum
source contributions. Shriek's four authored textures contain 9,934 of 16,384
texels with nonzero color but a zero original contribution on black; 9,089 are
literal `0x0204`. This produces the visible rectangular skirt.

`SHelperMeshSubmit::retail_alpha_source_quantize` defaults false. It is consumed
only by the existing source-alpha/software-lighting/ARGB4444 mode3 branch.
The helper fragment shader returns the original truncated source contribution
divided by alpha before fixed-function straight-alpha blending. Alpha is never
zero (minimum 1/16). This changes the source term alone: destination rounding,
RGB565 framebuffer precision and overlapping-pixel equality remain open.
Metal, GLSL and HLSL use the same expression. Existing unused `shade.w` carries
the flag; no uniform layout changes. Mode4 untextured, additive and default
mode3 behavior are unchanged. No color key, fitted threshold or multiplier.

The diagnostic-only `TArgbSourceContribution_DIAGNOSTIC` draws seven literal
words twice in the same actual Metal frame, flag false above and true below.
`shriek_alpha_probe.py` executes the original kernel for these same words with
incoming vertex colors zero and white. `argb_source_gpu_review.py` checks the
capture against those outputs:

- `0x0204`: existing GPU RGB8 `(2,0,4)`; opt-in `(0,0,0)`; original zero.
- Four other shipped sub-quantum words also become exactly black only opt-in.
- Alpha-nibble-zero white `0x0fff` remains visible `(8,12,8)`, original `0x0861`.
- Opaque white `0xffff` remains `(247,243,247)` in both policies, original `0xf79e`.

Evidence root: `/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra`.
Actual binary SHA256: `2d7e39ae6db8ee941a36326f3476b6415770fc94b15566c59bfcd4281381101b`.
`shriek-alpha-gpu-final2/review.json` pins the capture/native manifests and image.
The complete 50-frame Shriek common-policy movie has identical image SHA256s
before and after this optional shader addition. Native-domain Shriek alone
opts in in its separate leaf change. Diagnostic controls do not represent an
effect appearance approval.

Validation: original lookup/ramp/background tests (`test_argb4444.py`), seven
literal executed-kernel controls, actual GPU controls, clean Metal build and
50-frame existing-policy negative control. Other shader backends are updated
in source but were not executed here.
