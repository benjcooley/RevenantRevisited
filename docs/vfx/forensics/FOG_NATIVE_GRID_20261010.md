# Fog: actual animated prelit grid

`Fog` / `0xc24b2a85` now renders the shipped `Misc\Fog.I3D` mesh: 36 vertices,
50 triangles, no texture. The old preview discarded every draw while searching
for a nonexistent texture, and would otherwise have collapsed the grid to one
mean-colored billboard. Its replacement retains each vertex's authored position,
independent color/alpha and position drift. The existing FX triangle API carries
those values through an explicitly initialized neutral white texel; no renderer
or shader was changed. Gallery fallback uses this same factory and tick/submit.

Original registration/factory `0x4f6870` produces vtable `0x5aa7bc`:
Initialize `0x4e84c0`, Animate `0x4e86f0`, Render `0x4e8b10`. Init requests
FVF_LVERTEX `0x1e2`. Render sets flags `0x209c`, packs each vertex's ARGB8,
and scales XY by 7.475 with Z unchanged. The 20 anchored vertices and 16 drifting
vertices retain literal RNG range/order and the original velocity reset tests.
The production state uses exact 24 Hz accumulation instead of integer 41 ms.

`fog_probe.py` executes these original routines, the owner/matrix composition,
FVF_LVERTEX transformation and original software raster. Authored resource
residency, base animator traversal, extents and the RenderObject dispatch boundary
are explicit fixtures. The reference does not supply fitted positions, colors,
lighting or a replacement texture. Eight native constant-triangle controls prove
that untextured alpha uses `(A5+1)/32` and writes depth even when Scene mode2
requests `ZWRITE=false`. Zero/full alpha negative controls produce different
images. The actual software framebuffer is RGB565.

`fog_state_compare.py` compiles production `effects/fog_grid.h` and compares
43,560 float fields over 121 states: all exact, including 9,680 original RNG
calls. Two 1/48-second advances equal one 1/24-second advance every tick.
Native cold/warm replays agree. Three actual GPU runtime checkpoints also match
native RNG counts and drifting vertex2 values; the first captured image is tick2
following one warmup tick, not a fitted phase selection.

Both fresh actual Metal routes produced 120/120 nonempty, distinct frames with
clean process exit: ordinary world projection and explicit native-domain test
projection. `fog_visual_review.py` freezes nine phase triples, ticks2–120.
Native-domain is an isolated owner/projector configuration, not a change to the
gameplay camera. GPU gradients are smoother; its interpolation, blending,
fragment depth and framebuffer precision remain different from the original
scanline RGB565 raster and flat-per-triangle depth. Natural map caller and actual
gallery integration are separate checks; no complete-effect credit follows
solely from this implementation or its nonempty movie.

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/fog-visual/`

- Original executable: `28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`
- Exact Fog asset: `8ff5a3f3718757241a267f0e494d1b4dcc75f67c40f182902a7d46bdde70e8cf`
- `native-final/manifest.json`: `af4902dd86941144fb28ae7b72c85895743c2f2162351ed2d850b28cfe6d83b5`
- `state-final/manifest.json`: `cbcc5fa8cb0c73db27b8a595caac4a30cbfe4969ca8539a916c85c8d57395f57`
- Captured Metal binary: `46eb8a0d41971fd564916daeaea5b7cb1ce906c26a33b6d189f9ed9fc2efdba4`
- `metal-common-final/manifest.json`: `3e4f7468d27f1f15e66b1a7b1551541e3f33d31e687c39059a5f95ae1fa7fbe8`
- `metal-domain-final/manifest.json`: `095375591c9753b026c3a0306f7b64f7d984d87d681cea9a4c726c1b9465412c`
- `review-final/manifest.json`: `f1f6e03b32c57019598f8a964e6d05ae2516f1ae8aab67f032b22811f194516f`

`metal-common` and `metal-diagnostic` retain the blank-before-white-binding
negative controls; they are historical failed captures, not the final reference.
The preview retains partial status with a corrected warning about the remaining
RGB565 quantization gap; the obsolete collapsed-grid/texture warning is removed.

## Root visual review and bounded brightness diagnosis

Root reviewed all nine contact phases and the detailed crop: grid, moving lobes
and shape align, but Metal is visibly brighter/smoother. **Appearance credit is
held.** Source/runtime conversion and nonempty animated films remain valid.

`fog_alpha_probe.py` executes one varying-alpha triangle with RGB5=22 and vertex
A5=(0,31,0). Eight samples independently match interpolated alpha truncated to
A5, followed by the original RGB565 lookup. Thus alpha is not selected once per
triangle. The anchored Fog vertices isolate the material brightness residual:
RGB5=22/A5=3 produces native channels `(2,5,2)` after lookup, while the existing
GPU blend retains fractional `(2.75,5.5,2.75)`. This is a source quantization
cause, not evidence for a fitted brightness multiplier. Exact arbitrary-backdrop
framebuffer quantization would require a renderer policy change and remains
outside this conversion. The current shaders are untouched.

## Dynamic RGBA5 source policy, 2026-10-11

Source commit `41628be8` adds an explicit, default-false untextured RGBA5 format
for dynamic FX triangles. Fog selects it only in the native-domain fixture with
software lighting policy. Ordinary gallery/common projection keeps its previous
normalized-color path. No helper renderer, global light policy or gameplay
camera changes. The literal format remains explicit independently of lighting.

The new path interpolates raw RGB5/A5, truncates them, and reproduces the original
integer source contribution: `floor(channel * (A5+1)/32)`. The green source is
`2*G5`; source-over blending retains the original `(A5+1)/32` alpha. A per-triangle
unused UV field carries `trunc(max(vertex depth))`; native projection uses the
original truncated screen-coordinate sample lattice. All three shader backends
share this arithmetic. Default paths retain their previous values and routing.

`fog_dynamic_native_controls.py` executes 24 varying-color/alpha controls and
three sloped-depth controls through the original untextured raster. The depth
controls establish that the entire triangle uses its maximum integer vertex
depth and writes depth even with caller `ZWRITE=false`. The original scanline
routine `54eba0` calculates slopes using signed `(delta<<8)/length`. All 24
samples match an independent 8-bit slope calculation. Actual GPU output matches
22 exactly; two samples differ by one RGB5 quantum because GPU interpolation is
precise instead of fixed8. These exact two differences are retained explicitly
by `fog_dynamic_gpu_review.py`, not hidden by a broad tolerance. All three GPU
depth controls and the constant RGB5=22/A5=3 source-contribution control pass;
flag-false output retains the brighter fractional result as a negative control.
The compiled test checks 1,024 fractional input combinations across the actual
three shader bodies, format admission and unchanged RGB565/ARGB4444 defaults.
Existing helper-UV and 514 RGB565 quad controls pass.

The fresh original replay and 120-frame actual Metal native/common movies retain
the same state/RNG checkpoints. Nine whole-effect phases show the corrected
native-domain brightness bands. This is still not exact raster equality: fixed8
interpolation, destination RGB565 rounding and triangle-edge ownership remain
open. The original leaf, owner, FVF_LVERTEX transform and raster execute; decoded
resource residency, base animator traversal, extents and RenderObject dispatch
remain the declared fixture boundaries. Natural caller/device gates remain open.

Default regression recordings preserve all 24 fspray and three existing helper
PNG hashes. Fog common preserves 119/120 and WindStrip 254/255; each exception
is a frozen baseline image byte-identical to its preceding current frame, with
all other frames identical. These are retained capture-timing exceptions, not
phase-adjusted comparisons. A second current Fog common replay agrees 120/120.
Actual ordinary translated gallery Fog at `(3185,3185,16)` exits cleanly with
47 visible and 48 distinct images across 60 captured frames; this proves runtime
submission, not native-domain appearance or natural map-caller equivalence.

Private evidence root remains `hard-astra/` under the research directory above:

- `fog-quant-review-final/{contact,detail}.png`: original/common/native-domain nine-phase panels.
- Review manifest: `e4e3d75ba7c15caebefd414ee94a7275d23b726a1e89fcf1c0594bf999af8845`.
- Current Metal binary: `e1dcd56a9df31acde3ac13bc6e150d29ec438c0055d9a3be99b5085e8d442794`.
- `fog-quant-final-evidence-v2.json`: `bf886b9dcd29e39958c9aac53da97b2db03e0daab954db436262f7a8e86cb672`.
- That record pins literal original/GPU controls, fresh native replay, gallery,
  default regressions and the unchanged common repeat by SHA256.
