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
