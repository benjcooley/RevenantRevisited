# WindStrip original null-spell composite

Exact shipped `WindStrip` / `0xded3dbed`, `Magic\\Wind.I3D`, SHA256
`d33eeaa85ebcad2ef0ef55c258ae259bd92bb3cb2f204589cb7724f502fb941a`.
The V1 asset has three one-frame states (`launch/fly/explode`, flags8192),
24 vertices/faces, two objects/materials, ARGB4444 strips/drops and RGB565 halo.

The reference executes registered builder `66dea8`, factory `5237d0`, animator
vtable `5b6124`, SetupObjects `522af0`, Init `5229c0`, custom Animate `522bc0`,
Render `522fa0`, owner Init/Pulse `5227f0/522800`, original GenerateStrip
`50bda0`, complete RenderObject, owner matrix and software raster. Source uses
its own strict identity/resource checks, 20 independent moving strip histories,
400 falling-drop slots, original last-free allocation, per-Render drop RNG,
normal-lit authored halo and actual owner kill252. The previous three-strip
preview remains historical; the exact gallery mapping now selects the new
`TWindStrip_BESPOKE_NULLSPELL` adapter first. No renderer policies changed.

This one-frame non-looping owner exposes an ordering requirement: original
screen Pulse precedes NextFrame; custom Animate then clears CommandDone.
Reversing NextFrame/Pulse kills the fixture immediately. The reference and
source retain actual Pulse and completion, without suppressing its kill.
Native reversed-order control and compiled actual Advance tests cover this,
external owner death, kill252 and subsequent no-op.

Original GenerateStrip has an uninitialized local when start/end widths are
both15. The full Render caller supplies bit patterns `00523043` and `101fec0c`;
their tiny float contributions truncate to zero, yielding integer halfwidth7.
All251 active frames retain those observed residues. Explicitly replacing that
read with1 changes native geometry while state/RNG remain identical, proving
this is a caller-stack limitation. The independent source reproduces the
observed width, not an invented width curve or a claim about other callers.

Source/core proof compares 2,030,592 fields over256 ticks and Render strides1/3,
including all20 histories/400 live drop slots: exact RNG seeds/counts and maximum
float error4.99e-7. Independent compiled geometry compares505,168 fields:
original strip positions/UVs and halo matrices, maximum4.99e-7. Original strip
triangles retain their literal order through the existing dynamic triangle FX
API; authored drops follow, then the additive halo through the existing
post-FX helper queue. Original sampled frames execute full software raster;
all intervening original Render calls preserve state and RNG.

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra`.
`wind-review-first/{contact.png,detail.png}` contains16 phases2–255. Review
manifest SHA256:
`5da3451f814c32757d182aa309289e946a8239f3e10e0164a3b6ff09f11e3cc7`.
The final `wind-metal-{common,domain}-final` films match all510 PNG hashes from
both reviewed films. Final binary SHA256:
`ec82a97e85e10efd0f4abcb9d8393137f2f4d605c202073615916a39deedbbdb`.
`wind-gallery-current` independently exercises the translated ordinary-gallery
owner `(3185,3185,16)`: clean exit,60 no-UI frames,53 nonempty and54 distinct.
The log confirms the new exact adapter. This is runtime smoke coverage.

Root independently approved null-spell state0/native-domain overall appearance:
20-strip blue swirl, falling drops, halo growth/shrink, late blank and lifetime.
The frozen machine record `wind-review-first/root-approval.json` pins all sample
hashes, final replays, gallery and native controls. SHA256:
`8b3796f49a093ea1adf5c36d98b269ff10f27868ac45b6c82719a411071d8fab`.

Scope: facing0, owner/camera world0, seed1,24Hz,640x480, black, source ambient38
and directional1. Immutable decoded-resource/COM residency, base animator
bookkeeping, extents and owner script/light/map notifications remain fixture
boundaries. Equal-width incoming stack, cold Render before Animate, exact
pixels, destination rounding/sampling/depth, other devices, nonzero facing,
live-spell invoker orbit/attachment/damage/audio and natural callers remain
open. Common projection remains a separate diagnostic. No natural-spell or
full device-completion credit follows from this scoped approval.
