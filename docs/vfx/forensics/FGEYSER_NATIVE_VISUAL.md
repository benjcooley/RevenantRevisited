# Fire geyser: original authored leaf

`FGeyser_AUTHORED` and `fgeyser_original::Spawn/Advance/SubmitWorld/Destroy`
use shipped EFFECT `fgeyser` `0xad92bd31`, `Cave\Cavfgeyser.i3d`, through the
normal generic owner factory. Its default animator is independently confirmed
by the executed original registry lookup. The existing snapshot
shell remains fail-closed; `sgeyser` and its unresolved `filename vapor` are
excluded from the new strict profile.

The original null-spell owner executes `NextFrame470cc0`, `Pulse4de800`,
default animator/controller initialization `40df90`, `40e2e0`, `40e460`,
`40e8d0`, full `RenderObject40a8f0`, normal lighting and software `56eb30`.
Decoded immutable resources/COM textures, silent audio, map/script callbacks
and flat particle ground height1 are explicit fixture boundaries. No injected
white vertex carrier replaces the original normal-light path. The actual
90-frame nonloop owner marks `OF_KILL` at tick89/frame89.

The literal asset has five objects, three populated quads, one material and
one64x64 RGB565 texture. Original blendcont selects litadd16 immediately.
Its two PartSys controllers initialize together: fire capacity145 and sparks
capacity104. Stored tagframe1 and50 do not delay their initialization or Pulse;
PPS evaluates the animator's last Animate frame. The base `#glow` is separate
from the hidden `#fire` and `#sparks` templates. Original Scene mode16 uses
cull-none, depth test, and no depth writes. Existing two-sided additive helper
submission already provides that policy; no renderer/shader change was needed.

All450 authored object/frame poses match original key walker and matrix code:
4,050 key fields and7,200 matrix fields, maxima5.01e-8 and2.35e-7. The exact
profile rejects81 key/header/path/texture/tag/material/vertex/face mutations,
including the steam filename and alpha-format substitutions. All450 existing
default-versus-explicit-false decoder results remain identical;34 inclusive
key-boundary cases differ and match the actual original path. Camera-facing
`#` objects use authored S*R plus original camera matrices; other emitters
retain R*S*T. No inferred billboard dimensions or fitted rotation is used.

Production particle simulation matches532,792 native fields at full render
cadence and532,242 at render stride3. Every CRT seed is exact, with1,814 draws
in the full-cadence110-tick trace. Maximum full-cadence float error is2.29e-5.
An extracted actual FillInputs regression verifies multiple owner Pulses keep
the previous animator pose until Animate, and unrelated profile fallback stays
unchanged. Both controllers submit in original tag order before the base mesh.
The existing RGB565 integer additive policy is admitted only for this literal
profile; other formats and generic controller admission are unchanged.

The declared comparison is1024x768, screen center(512,640), owner/camera0,
facing0, quality0, seed1,24Hz, warmup1, black background. Native ambient38white
and directional1 are compared with Metal scene32white under the existing
software lighting policy. Native-domain preview uses positive16-bit depth
0..65535; common preview retains2238..3262. CaptureN follows originalPulseN+1.
Sixteen fixed phases cover rise, flame column, low sparks, base-glow fade,
decay and empty tail; detail crop(320,320,720,768) is identical in all columns.

Validation: private application build; two original fgeyser regression tests;
render-cached pose test; existing Quicksilver and static-profile controls.
Native comparison probes compile production spans under ASan/UBSan.
The natural CavFGeyser trap trigger, sound/cycle, original saved layered-map
appearance and full device/caller remain open. A standalone source leaf does
not grant acceptance of those integrations or exact framebuffer equality.

Frozen evidence lives under the private licensed-asset lab
`research/vfx-wide-20261010/hard-alpha/fgeyser-review-final/`.
The pair manifest SHA256 is
`b1b77960d210dca15b4c9a7216745e84901a32caac5ad64dfe97ed6bb5806a3c`;
cases SHA256 is
`c9b0a6fc22e84f05d014044ad4c1947819b14fdb8a22aa5fbacce53e31fc3ead`;
the actual captured binary SHA256 is
`51e0d3c4d1d76993dedca36bb0d9062f6a8b75d3bea21e1e32cb17d2cd6717e8`.
Both110-frame Metal films exited normally, with14 exact runtime RNG controls.
Two fresh native visual runs reproduce all110 complete state/draw snapshots
and all17 selected PNGs exactly; the state-only comparison trace is identical.
All native sampled pixels and both complete Metal films remain inside the
full1024x768 viewport. Root independently reviewed and approved the16-phase
whole/detail comparison; the external approval record pins that scoped gate.

Frozen root approval SHA256:
`e7256c919e66d31539c039d6a47ffb2c8c52eef726700b41f9466bbb066d24a4`.
Its captured source revision is `be47dd51672922e6cf9984a1ef1e002de2a6f3ab`.
A subsequent integration-only `IsAlive(const State*)` query reads the owned
instance's actual `OF_KILL` flag. It adds no timer and changes no captured
Advance or Submit behavior: the owner remains alive through the blank tail
until the original frame89 completion. The query follow-up also builds cleanly.
