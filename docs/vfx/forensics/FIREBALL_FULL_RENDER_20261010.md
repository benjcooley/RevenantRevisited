# FireBall full leaf render and impact

The original complete FireBall animator exposed three impact errors: an expired
burst skipped its final RNG/frame update, the ring discarded its first scale
overshoot, and the draw path replaced authored glow/body pairs and a colored
annulus with generic burst particles and a uniformly tinted helper mesh.
The production fix preserves the native update order and submits both authored
cards, followed by all 144 ring triangles with their four packed ARGB stops.
The optional per-corner quad colors leave ordinary uniform-tint submissions
unchanged. Both exact ARGB4444 textures opt into the existing software policy;
other formats and hardware/modern blending retain their existing policy.

`fireball_visual_reference.py` executes the original factory `514bf0`,
initializer `510d00`, missile `510220`/movement `470920`, complete leaf Animate
`510e40`, and complete Render `512150`. RenderObject `40a8f0`, scene `4174b0`
and software raster `56eb30` execute for selected reference frames. The source
selects FVF `1e2` for cards/ring and normal-lit FVF `112` for sparks. Neither
RenderObject nor selected raster draws are replaced by host colors/geometry.
Native draw-state observations show cull NONE, Z test enabled, Z writes off,
and SRCALPHA/INVSRCALPHA for the impact ring (states 22=1, 7=1, 14=0,
19=5, 20=6). Ring stops are `009e0f0c`, `ffc63d0f`, `bff79b0f`, `00fcd384`;
only their alpha fades. No fitted tint, geometry, time or seed is used.

Evidence is under the private lab root
`research/vfx-wide-20261010/hard-alpha/`:

- `fireball-impact-freeze/state-report.json`: 45,180 binary32/RNG checks across
  90 original complete animation ticks. `fireball_impact_probe.py` compiles the
  actual production StepAnimate body. Restoring the two old state operations
  with `--negative-control` produces 1,124 differences, first at tick 64.
- `fireball-impact-freeze/geometry-report.json`: 444 checks across 39 native
  impact packets. Compiled authored glow/body geometry differs by at most
  0.000031616 world units; packed ring colors match exactly.
- `fireball-head-regression/manifest.json`: the prior moving head/glow control
  remains exact over 48 raster pairs, 1,920 fields and 48 original warm replays.
- `fireball-native-freeze/manifest.json`: original executable/asset/trace and
  selected-frame hashes. Retail EXE SHA256 is
  `28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.
- `fireball-metal-freeze/{common-domain,native-domain}/manifest.json`: two
  actual 90-frame Metal films, each clean PASS and 79 nonblack frames followed
  by its empty tail. Binary SHA256:
  `12950d55d16c0140db0b317e73a26d239357af2ecd90213362347f227ce5b8a5`.
- `fireball-review-final/{whole-life.png,detail.png,pair-manifest.json}`:
  eleven matched phases, original/common/native-projector columns. Pair SHA256
  `01daee6cc8853fecc8cfbd74249c83ee7ae8fcc9e73a9add054571f028865923`.

The isolated `TFireBallEffect_AUTHORED_FLIGHT` registration uses the existing
SpawnForTest, overrides its endpoints to +X 1000, then runs the production
24 Hz controller and both submission paths. It performs one lifecycle only.
Its outer random aim draw is retained and explicitly consumed by the native
visual replay. Seven independently logged global RNG counts agree exactly:
1, 98, 300, 646, 1236, 1846, 2382. Capture N follows pulse N+1 (one warmup).
The origin is (-240,0,78), initialization lifts Z by 50, and the native camera
is (0,0,0), corresponding to the port's (512,384) pixel camera. Both use a
1024x768 black framebuffer. Scene input 32/white yields native ambient 38/255
plus directional 1. Detail crops follow the original owner projection; the
same 512-square rectangle is applied to every member, without image alignment.

Build and `tools/test_fx_capacity.py` pass. The capacity test executes the
actual packing routine and verifies authored triangle corner order, optional
per-corner RGBA, uniform fallback, capacity boundaries and frame reset.

This establishes full **leaf** animation/render evidence and matched appearance
for flight, range impact and disappearance. Decoded I3D resources/COM surfaces,
silent audio, base animation bookkeeping and world-query interfaces remain
explicit boundaries; native owner transforms still execute separately. A live
spell/damage caller, actual character collision, natural map reaper and exact
device/framebuffer parity remain open. YFireBall inherits shared frontend
changes but receives no new appearance claim from this FireBall recording.
The common-world projection and backend quantization differences remain visible
and declared. Independent root review is recorded separately from this source
commit; this document does not update the central ledger.

Reproduce the original film with `fireball_visual_reference.py EXE --frames 91
--visual-replay --output DIR`. The two Metal manifests retain complete capture
scenarios for `port_capture.py`. Run `fireball_visual_review.py --native DIR
--metal DIR --output DIR` to regenerate the hashed panels and RNG controls.
