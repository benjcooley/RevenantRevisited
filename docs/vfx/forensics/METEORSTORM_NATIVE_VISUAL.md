# MeteorStorm: shipped Comet leaf

The new `MeteorStorm_AUTHORED` preview uses retail type `0xf32bcfac`,
`Magic\Comet.I3D`, through the actual generic effect factory and default
animator/controller bridge. It does not invoke the snapshot MeteorStorm
simulator. `comet_original::Spawn(origin)` creates a disposable normal owner;
`Advance`, `SubmitWorld`, and `Destroy` expose its controlled leaf lifecycle.
Natural `StrikeEffect` / `Strike` spell orchestration remains unverified.

The literal asset has one nonlooping 45-frame state, eight objects, five
populated quads, three materials and two RGB565 textures. Three simultaneous
PartSys tags use `#comet`, `#trail`, and `#sparks` in that order, with capacities
125, 20, and 112. The `blendcont litaddz` tag initializes mode80 immediately;
its stored frame3 is not a delayed activation. Particles retain mode16.
Only this exact identity and strict keys, vertices, indices, materials, tags,
state flags and texture-format profile are newly admitted.

The native fixture executes original default animator factory `40dc00`,
controller initialization `40df90`, owner `NextFrame470cc0`, generic owner
`Pulse4de800`, animator `40e2e0/40e460/40e8d0`, full `RenderObject40a8f0`,
normal lighting and `56eb30` / software raster. The generic owner already
pulses its animator; an initial diagnostic with a duplicate explicit Pulse
was rejected. The final reference uses the actual generic owner vtable and
one dispatch. Decoded resource/COM providers, a null spell, silent audio,
map/script notifications and flat ground height1 are explicit boundaries.
No host white/material vertex carrier replaces original normal lighting.

All 360 object/frame poses match original `409430/40a420`: 3,240 key fields,
4,400 accepted matrix fields, maximum error `5.01e-7`. All 190 profile/key
mutations reject; all 360 default versus explicit-false decoder controls are
identical. Comet explicitly selects inclusive native key timing. Ordinary
emitters retain R*S*T; `#` objects rebuild S*R with original camera orientation.
Three-controller pool comparisons pass 301,886 fields normally and 301,270
with render stride3. Maximum errors are `3.82e-5` and `3.82e-6`; every CRT seed
matches. Cached animator poses persist across multiple Pulses until Animate.
Original owner completion sets `OF_KILL` at tick44, frame44.

A real software/hardware difference is retained. The expanding `shock01`
quad exceeds original software raster's literal edge limits: |dx|640 and
|dy|480, checked only from triangle vertex0 to vertices1 and2. These limits
are independent of viewport size. At capture14, original branches `56e36a`
and `56e395` reject its two triangles. A diagnostic changes only the original
limits to6400/4800 and restores the huge ring: 70,514 becomes 527,635 nonzero
pixels, while all controller state, RNG, matrices, vertices and colors remain
identical. Twenty exact/next-float boundary and winding controls compare the
compiled source predicate to original executed branch decisions. An additional
58 accepted-pose base triangles compare the full source world/view composition
to original projection and primitive admission.

`cometsoftwarebounds.h` applies that primitive rule only when both
`StartupVfxNativeDomain` and software mesh lighting are selected. The adapter
submits immutable original triangle ranges; it never scales or recolors them.
Ordinary game/gallery and hardware/modern paths retain the full authored ring.
The common-projector film therefore is a diagnostic column, not a claim that
software and hardware should produce the same impact ring. No shared renderer
or shader behavior changed.

The fixed comparison uses 1920x1080, screen center(960,850), owner/camera(0,0,0),
seed1, 24Hz and warmup1. Native lighting is ambient38 white / directional1;
Metal scene ambient is32 white, using the existing software lighting policy.
CaptureN follows native PulseN+1. There is no geometry, tint, timing, seed or
registration fit. The 1024x768 diagnostic clips the high authored launch;
1536x1152 was rejected by the application's supported-resolution list and is
not evidence. The supported final viewport preserves source scale.

The original positive16-bit depth storage permits 0..65535. The isolated
viewer's former 2750+/-512 range clipped the high launch in Metal despite
identical authored XYZ: native capture1 contained the head/trail, while Metal
was empty. The native-domain viewer now uses near0/far65535; ordinary/common
near2238/far3262 is unchanged. Both films record and verify these values.
Only reference-fixture depth admission changed: XY, camera, matrices, normal
lighting, color and geometry did not. The retained clipped-launch control and
corrected launch show that causal difference; no shared shader changed.

`port_capture.py` now lets the process use its remaining declared timeout to
finish writing the filmstrip and exit. File creation can precede PNG completion;
the old two-second wait killed a valid large capture during encoding. A delayed
producer test passes inside the budget; nonzero exits and actual stalls still
fail. No completed-image override grants a pass to a forced shutdown.

Validation: application build; five Comet/LightStrip/cached-pose tests; three
Quicksilver/static-particle regressions; thirteen capture-process tests and original primitive-bound
checks, all pass. Tests compile production spans under ASan/UBSan where applicable.
Full natural spell/actor dispatch, audio, arbitrary-camera rendering, and exact
framebuffer/device equality remain open. Root owns the independent appearance
review and central ledger; external hashed records below distinguish that gate.

Frozen candidate evidence (private licensed-asset lab, not checked in):
`research/vfx-wide-20261010/hard-alpha/meteorstorm-review-depth-final/`.
`pair-manifest.json` SHA256
`c8d5c5b0e0a64d337eed721fe117da70b957d3a35c768ebe9ab227c96eb22910`;
`cases.json` SHA256
`e55fa87f6df11d0e664b11f75a88508135b8dd372954917fc17b6fb8405371cc`;
capture binary SHA256
`533def995a5c289cc45df1149f4adbee9cddef5a97a575165283486f8c537bc8`.
The cases file pins all proofs, both actual-Metal manifests, 17 matched phases,
14 exact RNG checkpoints, original replay, depth control and viewport bounds.
Two fresh 60-tick native runs have identical complete pool/RNG/draw packets
and all 18 selected PNGs. Both Metal films contain 60 cleanly completed frames.
All selected native images and all 60 native-domain Metal frames stay inside
the full viewport. Common-domain frames13..17 clip the preserved oversized
hardware ring; that column is diagnostic. The older
`meteorstorm-review-final/diagnostic-status.json` explicitly rejects its
near-plane-clipped launch and must not receive appearance credit.
Independent root appearance review remains a separate external approval record.
