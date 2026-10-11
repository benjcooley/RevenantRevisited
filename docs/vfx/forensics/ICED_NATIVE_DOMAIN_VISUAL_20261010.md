# Iced crystal and shatter rendering

`Iced:0xfacecabb` now renders all49 authored crystal facets and the actual cube
mesh for its30 tumbling chunks. The previous preview stacked identical24-unit
billboards at the origin and discarded the authored mesh shapes/rotations.
The two shipped ARGB4444 textures use the existing source alpha policy,
positive-face culling and original ordered alpha submission. Update and world
submission are separate callbacks, including the actual gallery path.
No shared renderer, shader, gameplay camera or terrain changed.

The original registry resolves animator builder `0x66cdd0`, factory
`0x4f7eb0`, vtable `0x5ab460`, Init `0x4eca60`, Animate `0x4ecaa0`, Render
`0x4ecff0`. Unlike the old I22 forensics note, the full retail shatter physics
and render body are present and now execute. Asset
`Imagery/Magic/iced.i3d`, SHA256
`0a2cdd9255322792ecb15f80bec3330a04aac363232c8adf5c40570949ab9ac8`,
is version1,50 objects,290 vertices,110 triangles and two ARGB4444 textures.
The original leaf supplies every matrix with flags0x100; version1 animation
tracks are outside the geometry-only resource adapter and are not replaced
with guessed poses.

The owner contract matters: the animator returns without drawing or updating
when owner+0x184 has no victim. The fixture executes actual owner Initialize
`0x4ec880`, InitIced `0x4ec930` and Pulse `0x4ec890` with a stationary living
victim. Actual InitIced sets the victim's paralysis/iced bits; actual Pulse
increments the phase and clears those bits at240. The character virtual
health result is explicitly100, Stop and base Pulse are boundaries, audio
lookup reports absent, and allocation starts zeroed. No bones are invented.
A null-victim negative control produces zero draws and consumes only the
single initialization RNG call.

The standalone port profile models that fixed living-victim phase contract;
it does not claim natural character attachment. Phase240 seeds30 cubes,
subsequent ticks integrate gravity, spin and bounces, and death is tick322.
Current source initialization and Advance compile unchanged in the state
checker:421 ticks,189,450 exact chunk float fields, all counters/lifetimes and
all451 RNG calls match the original. Original range outputs are common inputs
with strict range/order guards. The port uses exact24Hz timing, the original
binary64 angle multiply, and double-input atan2 before its final float store
(as with native FPATAN); the previous float atan2 differed by1ULP for one
birth angle. No physics or image parameters were fitted.

Final native evidence includes422 states and18 sampled complete software
frames, repeated identically in a second warm run. Each actual Metal route
records420 frames. Sixteen original/common/native-domain phase triples cover
reveal, full hold, shatter, tumble/bounces, late320 and blank322/421. The
native-domain route has319 nonblack and92 distinct frames. Seven global RNG
checkpoints and four leaf phase logs pin the live sequence: phase239 has1
RNG call;240/241 and death322 have451. Source ambient38/directional1 is logged.
The explicit isolated owner/camera is origin0, facing0, cameraZ0; source
blend2, cull2, depth test on and depth writes off are declared renderer inputs.
Common-world projection and framebuffer/raster precision remain distinct.

Actual gallery smoke exits0 with99/100 visible,39 distinct frames, and logs
`TIcedEffect_BESPOKE` as the selected implementation. That proves runtime
submission/repetition, not original full-map parity. Natural IceBolt posting,
character attachment/health changes, early unfreeze, audio, complete native
RenderObject resource dispatch and device parity remain separate gates.
Icedsparks/Snow aliases receive no credit and remain excluded by the existing
preview admission guard.

Tools: `iced_probe.py`, `iced_state_compare.py`, `iced_visual_review.py`,
`port_capture.py`, `review_capture_survey.py`. Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/iced-visual`.
Generated artifacts and licensed assets are not committed.

Frozen evidence (2026-10-10):

- `native-final/manifest.json`: SHA256 `0fe4f031b9c9aea267c47d1db52a19d302ba75bfc306cc5a068164fd4eb7a8b4`.
- `state-final/manifest.json`: SHA256 `dd406b57dadef091a0213f0a52c4ab0fe265b8d7f0bfbcc2661ed35f5245b5e4`.
- `metal-common/manifest.json`: SHA256 `761e98770989a2d134a6fd7735c1e1e464386cca4f2a97198d148a5bb3a23018`.
- `metal-domain/manifest.json`: SHA256 `e6f86e12f3b43268428e61a92a22d24936301668f0d602a78f3c3681fa3ab813`.
- `review-final/manifest.json`: SHA256 `4efbe5facad3ce5f75dcd3ae6473c3d40a7fe6fb37435fe3f7e91d57f31af555`.
- `map-current/manifest.json`: SHA256 `a4ee44ccc331592e43e29f4b9017e476142c88bbc1983c2574eba069bc8f0e6f`.
- Metal binary: SHA256 `18584c2fb2645004ae7d1468d0126319cd61db5c4776ca345fa1bbc24285cd84`.

Root independently reviewed final contact/detail and approved the scoped
native-domain crystal reveal/hold,30-cube shatter/tumble,late320 andblank tail.
The approval retains the explicit living stationary-victim contract and all
natural-caller/audio/context limits above. `root-approval.json`: SHA256
`9410ae1ba6ab0c4666b12bb5e2576b99b28e41d349446fbb5ac2a548d30e7121` pins the16 phase triples, binary, live
RNG/light logs and state/gallery proofs. This grants one scoped appearance
pass only.
