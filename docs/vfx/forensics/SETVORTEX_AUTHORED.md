# SetVortex authored runtime baseline

Exact retail EFFECT type: `setvortex`, `0xad92bc15`,
`Magic\Setvortex.I3D`. The shipped imagery SHA-256 is
`3cd302348446d3b14dce6dcf8badcfc0eb24f5a146c876c9d59c0a7292f20524`.
It has one object (`box01`), 12 vertices, 12 triangles, one texture frame,
and no controller tags. No procedural geometry or texture fallback is valid.
Direct archive parsing confirms 3,760 bytes, imagery-body version 1,
flags `0x3c`, object `box01` at vertex start 0 with material 0. Its untextured
face run is empty; texture run 1 contains all 12 triangles. Material 0 links
texture 0, with diffuse/ambient/emissive RGBA `(1,1,1,1)`, specular RGBA
`(0.9,0.9,0.9,1)` and power 0. The one-frame texture is 32×32 ARGB4444:
16-bit masks R=`0x0f00`, G=`0x00f0`, B=`0x000f`, A=`0xf000`.

## Source behavior

Original snapshot `effect.cpp:5100–5236` supplies the effect and animator.
Retail `recon/classes/cls_0x4e6620.cpp` at `0x4e6620` corroborates the
guarded spell-invoker teleport mark and replacement of the previous marker.
The recovered render at `0x4e6920` in `cls_0x5b0074.cpp` corroborates object
0, flags `0x400048` (scale then translation, absolute position), uniform
scale `0.5`, and the alpha blend helper. This OOAnalyzer file merges unrelated
methods: those other methods are not SetVortex evidence. Animator initialization
and bobbing are snapshot-backed; retail init/update addresses `0x4e6800` and
`0x4e6890` are mapped but their full bodies are not in that decompilation.

Initialization caches owner X/Y and `FIX_Z_VALUE(owner.z + 30)` in the
animator's legacy D3D coordinate domain. It sets direction to 1 and current
height to 30. Each authored tick adds `0.4f` to both cached Z and height while
ascending, reversing only when height is strictly greater than 40. Descending
subtracts `0.4f`, reversing only when height is strictly less than 20.
Float accumulation and overshoot must be retained. It consumes no RNG and
has no timed death or offscreen kill.

Render uses the raw authored vertices, indices, UVs and object material;
uniform local scale is `0.5`, followed by cached absolute translation. There
is no owner rotation, later owner movement, animation-key pose, new light,
or invented billboard orientation. `SetBlendState` is SRC_ALPHA /
INV_SRC_ALPHA, depth test on and depth writes off.

## Modern bridge and limits

`TSetVortexEffect_Bespoke` owns the cached legacy state, immutable extracted
mesh, authored material diffuse and texture. A true `1000.0 / 24.0` ms
accumulator separates updates from submissions. The typed builder and
animator hook attach one `setvortex_reference` component only after stable
map identity and texture readiness. The owner destroys that component through
normal deletion. Preview `TSetVortexEffect_BESPOKE` initializes manually without
the automatic runtime component, so a submit cannot double-tick the bob.

`OBJ3D_ABSPOS` bypasses the live owner transform. The bridge converts the
cached D3D translation back through `REV_FIX_Z_VALUE` at submission, and uses
the existing `WORLD3D_Z_SCALE` convention for mesh-local Z. Consequently the
literal `0.4f` D3D bob maps to `0.584f` common-world Z. Applying FIX_Z_VALUE
again directly to the common-world submission would mix coordinate domains.
The modern projection's exact agreement with retail remains an A/B gate:
the historical D3D camera/projection and this port's common-world mesh bridge
are different implementations, and no fitted offsets have been introduced.

Authored diffuse is preserved. Snapshot `blue/swscene.cpp:612–614` selects the
ARGB4444 path explicitly described as “no lighting”; this supports unlit
submission for this particular software reference, unlike the RGB565 effects.
It does not establish the hardware normal/material illumination path. Asset fidelity,
geometry, timing, alpha raster behavior and depth must be assessed against
fresh retail captures; source reconstruction is not acceptance.

## Probe

Use the healthy marker-free floor and explicitly verify the FPS redraw
workaround. Begin recording before `addat 10000 10000 1 setvortex`, capture
at least five seconds, then delete explicitly. The third numeric argument is
amount, not Z. A runtime timeline should prove one reference component,
persistence, visually stationary X/Y after an owner MOVE, and clean deletion.
Spell-invoker teleport state is a separate gameplay gate; this editor recipe
does not exercise that caller.

## Source validation scope

The source-only review checked unique typed builder/animator registration,
exact runtime ID and asset/count validation, immutable cached translation,
literal branch/step/threshold agreement with the snapshot, separate 24 Hz
update/submit, and manual initialization without automatic attachment. Component
ownership follows `TObjectInstance::~TObjectInstance` detach/clear and
`TFlipbookBillboardComponent::OnDetach` update unregistration. The scoped diff
passes `git diff --check`. Build, runtime lifecycle and native A/B are owned by
the parent capture process and remain unclaimed here.

## Built runtime and actual retail A/B, 2026-10-05

Build passed (`research/setvortex-20261005/build-authored.log`, binary42bcd45414733efe11ac8e58d56ab88d8c05a1cee9eba054930d06280200d2c1). Actual command fixture passes seven checks: exact type/component, MOVE retaining cached visual X, persistence through9.958seconds and explicit deletion with17 exact-ground tail frames. Captured teardown required owned SIGTERM after complete frames; normal shutdown is unverified. Natural teleport caller was not exercised.

`captures/runs/sw-fps-setvortex-20261005` retains an FPS-enabled marker-free SW reference, with54 distinct images in the five-second derivative and exact cleanup. `captures/ab/sw-fps-setvortex-authored-20261005` has150 port frames and zero outside-ROI drift in both clips. Head orientation/surface appearance differs, so visual parity is deferred.

Read-only orientation audit found no justified rotation or mirror: SCL1/POS2 suppresses animation keys, ABSPOS suppresses owner/root rotation, and extraction preserves XYZ/UV/indices. Raw faces0–1 and2–3 are coincident opposite-facing quads with mirrored U; eight remaining triangles are degenerate. Snapshot and current projected winding both select faces0–1. Retail SW camera disassembly at0x56cdc0 confirms +45degreeZ then−120degreeX, matching snapshot.

Source versus current local screen-Y scale is approximately `.25254*(vx+vy)-.61859*vz` versus `.25*(vx+vy)-.65025*vz`; that modest difference does not explain a mirror/viewing-angle change. Next bounded diagnostic, later: trace actual retail SW projected triangles and UV raster arguments to identify selected surface/sampling orientation. Preserve authored data until evidence identifies the correction.
