# IceBolt original software leaf and Metal comparison

IceBolt is a stationary beam. The shipped registrations select owner
Initialize `4eb210`, owner Pulse `4eb230`, animator Initialize `4eb570`,
animator **Pulse** `4eba00`, and Render `4ebea0` (vtable `5ab1f4`). The
older snapshot-based account's 300-particle allocation belongs to the preceding
effect. IceBolt has 50 frost and 50 snow particles; initialization consumes 600
CRT draws. The owner raises Z by 50, posts Iced at pulse 20 and marks itself for
removal at pulse 120. Damage, a live Iced victim, audio and natural game caller
integration remain outside the controlled leaf comparison.

`iceboltcore.h` carries the executed original state arithmetic and ordered
geometry. The original x87 expression makes a 500-unit beam draw four sliding
rings, not five. All seven authored objects retain their vertices, normals,
UVs and triangles. The endpoint adapter follows original ten-unit float steps,
integer world queries and the 50-step cap; its explicit radial world boundary
is a preview control, not a claim about natural collision dispatch.

The original Render submits FVF112 and source-alpha/cull-none scene state.
Four cylinders are untextured; the other objects use three strict ARGB4444
textures. Original Illuminate ignores the four different cylinder materials.
Untextured RGB5 is packed as R5/G6-even/B5, with opaque alpha. A white RGB565
texture is not equivalent: original white results are 65503 and 65535,
respectively. The helper shader's mode4 therefore does not sample a texture.

A second kernel distinction matters visibly. Original untextured triangles
write depth even with ZWRITE=0 or ZENABLE=0, and use **trunc(max(vertex Z))**
for every pixel of each triangle. Equal-depth later draws overwrite. Ordinary
GPU interpolated depth hides the authored spirals. The explicit mode4 helper
submits original triangles in order with their constant native depth; mode3
textured draws retain no depth writes. Defaults, generic alpha materials and
modern/hardware policy are unchanged. MSL, GLSL and HLSL share the added path.

Validation tools execute actual original transform, normal Illuminate,
RenderObject and software raster code; none inject white vertex colors:

- `icebolt_visual_reference.py`: explicit empty-world beam, original owner and
  animator pulses, native resource/COM surfaces, black 1024x768 target.
- `icebolt_state_probe.py`: 87,384 exact scalar/binary32 comparisons over
  initialization and 131 pulses (post-kill state pulses are fixture controls).
- `icebolt_geometry_probe.py`: 1,059 ordered original packets, 16,944 matrix
  components; maximum error 4.78e-7 before original owner cleanup.
- `icebolt_endpoint_probe.py`: six original world marches, including face13
  radius250 producing length260 rather than a fitted Euclidean length.
- `icebolt_untextured_probe.py`: 36 lighting/kernel cases, white-texture
  negative control, four authored material controls, four Z state controls,
  and 21 independently varied/cyclic-index flat-depth controls.
- `test_helper_uv_submission.py` and `test_helper_source_alpha.py`: compile
  actual production submission/pipeline code, checking default two-pass
  behavior, explicit cull-none state, modern fallback, draw ranges, depth
  override gating and texture/native-projector uniform independence.

`TIceBoltEffect_AUTHORED_BEAM` is a single-life controlled registration; the
existing entry retains its retrigger behavior. Capture uses origin (0,0,78),
owner Z128 after initialization, screen center (310,485), 24Hz, seed1 and one
warmup. Original ambient is (38,38,38), directional (1,1,1); Metal selects the
same retail software light environment through scene32white. Both common
world and explicit native owner/projector movies are retained. Capture N
follows original pulse N+1, checked with CRT draw counts. Fixed crops are
identical across columns. No geometry, tint or screenshot registration fit is
used. Common-world projection, raster edge/quantization differences and exact
framebuffer/device equivalence remain open.

Private evidence is under `research/vfx-wide-20261010/hard-alpha/` in the
retail lab, with hashed pair manifests, native traces, binary and capture
manifests. Earlier no-write and interpolated-depth films are retained as
negative controls, not appearance passes. Independent appearance approval is
recorded separately after review; this source note grants no ledger credit.
