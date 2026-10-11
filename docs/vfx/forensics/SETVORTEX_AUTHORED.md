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

## Genuine loaded renderer correction, 2026-10-11

`setvortex_loaded_preflight.py` now executes the actual null-spell class factory,
owner5a9f9c, CreateAnimator5aa19c, complete owner Pulse4e67d0, custom
Animate4e6890, and full Render4e6920. Original Begin/PreRender/Post/End remain
active, with explicit isolated RAM target1280x960/camera0/source0 and ambient38/dir1.
All105 updates persist without RNG or death. Actual render state is SRC_ALPHA /
INV_SRC_ALPHA, depth test on, no depth writes, **CULL_NONE**. Both coincident
opposite-facing quads contribute; the old host-side winding rejection was wrong.

Native Init4e6800 uses the nonlinear original FIX(ownerZ+30), not the port's
linear macro: initialZ21.3433853. The source now uses the independently proven
RetailSoftwareOwnerZ helper without changing the global macro. The exact32x32
ARGB4444 format is checked and opts into existing texture mode3. Ordinary mesh
geometry remains complete; native-domain projection is explicitly opt-in.
No renderer/shader or other effect policy changes are needed.

Compiled actual Advance and Submit functions under ASan/UBSan agree with all105
native updates:315 bob channels max4.89e-8,1260 submitted triangles and18900
vertex/UV channels. The old linear-height negative control differs0.79544 at
first draw. Evidence: hard-alpha/setvortex-submit-proof/manifest.json and
setvortex-loaded-first/manifest.json. Fixed24Hz updates/render are certified by
this comparison; arbitrary skipped-render scheduling remains unverified.

Final build and retained full native/source unittest PASS (36.5s). Fresh105-tick
native state/full-draw replay is exact. `setvortex-review-final/pair-manifest.json`
SHA256 f28b40db2eef19719173a70439e0bd1be1c0964655a6d76276c7022df397af4e
pins27 full-frame original/Metal pairs and nine identical4x nearest detail pages.
Final captured binary SHA256
4ab18b5620806b6b555c23235838a2168279df8c372aa9b09c14d03590a6e620.
`setvortex-metal-final` records27/27 nonblank snapshots across105 render ticks,
clean shutdown, actual1280x960, explicit native policy. All27 images are byte
identical to the earlier corrected film whose collector rejected a missing
policy-confirmation log; that negative collection result is preserved.
`setvortex-metal-common` separately records27 ordinary-projection snapshots.

Head orientation and persistent bob now agree in the matched images. Tiny
scanline/nearest sampling and overlap/RGB565 quantization differences remain
visible at4x; exact pixels are not claimed. The fixed null-spell preview has
no death tail by design. Real OF_KILL is reported through the gallery callback,
and world submission is separated from advancement. The existing default/map
factory remains available. Independent root approval/integrated gallery are
separate from this source evidence submission.

## Independent root review

Exact persistent nullspell SetVortex genuine loaded owner/customAnimate/fullRender, original nonlinear FIX(+30) cachedheight, complete12authored triangles/CULL_NONE and strictARGB4444 decode. Full10524Hzupdates/render; source315bobfields max4.89e-8,1260triangles/18900XYZUVchannels checked, oldlinearZnegative differs0.79544. Rootnative/sourceunitPASS35.165s;27exacttick original/actualMetal wholepairs and nine4x detailpages independentlyreviewed,96pinsverified. Integrated27PNGsatstride4/all105renders byte-identical/cleanexit. Realruntime map gallery100/100visible52distinct/onepersistentfactory/cleanexit is liveness only, not standaloneappearance certification; initialcopiedvalidator incorrectly expectedpreviewruntime0/log retained and corrected againstactualruntime1 route. Initialcapture duringbuild rejectedchangedbinary, finalstablebinaryrepeatused. Spellteleportmarking/previousmarkerremoval,naturalmapcaller/skippedrenders/ordinaryprojection/audio/device/exactpixels andnearest/alphaRGB565overlap residualsremainopen.

Root approval SHA256 `be346f066bcb25f35b461998cca9fc9c6a74b661124e0196d89c636910e8d16d`. Full acceptance remains open.
