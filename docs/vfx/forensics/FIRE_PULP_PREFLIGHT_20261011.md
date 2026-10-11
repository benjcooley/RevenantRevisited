# Fire/Pulp feasibility boundaries

Fire `0x119f01e3`, `Misc/Fire.I3D`, resolves the original default factory
`40dc00`/builder `5e8508`. Its old-format resource has one4-vertex/2-triangle
quad, one looping authored frame, and33 real ARGB4444 texture frames. The full
original default initializer, owner Pulse/NextFrame, Animate and Render `40e8d0`
execute through66ticks. The actual owner/animator frame and texture selector stay0.
Seven sampled software frames (0/1/2/16/32/33/66) are blank in the declared
origin/camera0, ambient38/dir1, Scene0/cull2/depth-write configuration. The literal
quad spans about10×5 projected pixels. This does not authorize inventing a33-frame
owner loop, enlarging its geometry or changing its caller's draw policy.

`tools/retail_runtime/fire_default_preflight.py` reproduces this bounded result;
private evidence is `research/vfx-wide-20261010/hard-astra/fire-preflight`.
No Fire production or appearance claim is added. A different real caller or
texture/matrix configuration remains possible and unverified.

Pulp `0x152dafdd`, `Misc/Pulp.I3D`, resolves actual custom factory `4f6910`,
builder `66cd98`, animator vtable `5aa820`. Original Animate `4e8d10` calls its
base then gates on owner+`1a0` and character+`184` at `4e8d28..4e8d3e`.
Render `4e9400` returns immediately when owner+`1a0` is zero at `4e940b..4e9419`.
Its initialized body-part/blood arrays are owned by the character pulp caller;
the snapshot's empty standalone preview does not supply them. That actor/caller
recovery is deferred, rather than replacing body parts with a generic emitter.


A genuine loaded-world Fire follow-up uses actual class NewObject, the public
CreateAnimator path and untouched loaded old-body imagery/COM textures. It
executes NextFrame, complete Effect Pulse, Animate, original BeginScene,
PreRender, Render, PostRender and EndScene. After PreRender, the original camera
API binds only the declared1280×960 RAM target. Source(3185,3185,16), camera
(3185,3185,0), ambient38/dir1 and material LightState1 are explicit boundaries;
there are no actor/geometry/color or scene-mode replacements.

All67 ticks submit one actual FVF112 quad. Contrary to the older injected
scene policy above, the original caller chooses CULL_NONE, source-alpha and
ZWRITE_OFF. Seven sampled frames0/1/2/16/32/33/66 remain completely blank.
Owner state/frame and selected texture frame remain0, with no random draws.
The projected quad is approximately10×5 pixels. Its literal first texture
has119 nonzero ARGB4444 texels;33 frames exist but no original caller advancing
them is established. This confirms a bounded default-path limitation, not a
reason to invent a texture timer, change the geometry, or award visual credit.

Reproducer: `tools/retail_runtime/fire_authentic_preflight.py`.
Private evidence: `hard-alpha/fire-authentic-final/manifest.json`.
Manifest SHA256: `9bb956f1c4c3b8885fec9c7b7a42107e53f4e7f83ecc4a9778dd912a31427c0c`.
