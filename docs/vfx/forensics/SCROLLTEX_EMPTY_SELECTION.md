# Shipped scrolltex targets and native empty-selection fallback

The six simple water assets use one constant root mesh and one authored scrolltex
tag at state 0/frame 0. FlowWater targets `water` but its object is `waterstr`;
BendWater1/2 target `water` but their object is `waterbend`; WaveS targets `wave`
but its object is `wave02`. Wave and WaveM target their exact object names.

The original object parser `0x40d4a0` compares complete lowercase names. Original
ScrollTex Initialize `0x401820` calls the actual base tag parser, then checks its
selected-object list at `0x401839`. An empty list triggers the loop
`0x401843`–`0x401864`, which adds every animator object via GetObject `0x40eef0`.
It then allocates and copies original UVs. This is a recovered controller fallback,
not permission to interpret malformed tags approximately or rename authored data.

Direct execution of factory `0x4059e0`, complete tag parsing and Initialize, then
Render `0x401990`, successfully scrolls all six actual asset vertex arrays at
global frame24. No tag spelling or object name was changed. Current port
InitializeScrollTexTracks rejects the four valid-but-unmatched names, leaving them
static. Its scoped correction should keep the strict supported grammar and finite
rate validation, resolve exact names first, and target every object only when a
valid selector produces no match. Overlap diagnostics remain in force per object.

Native Render gates on the live owner frame, multiplies the global frame count
by authored du/dv and writes original UV plus that offset. It does not accumulate
from the previous rendered UV. Full generic map installation, lighting, sampler
device equivalence and modern GPU appearance remain separate validation gates.

Rift1 has `animtex` instead of `scrolltex`; it is deferred from this batch.

The real FlowWater preview also required a scoped authored-mesh path rather than
its unrelated Water particle delegate. Static geometry may opt into exactly one
supported scroll controller, with passive state/audio metadata allowed and other
controllers rejected. Each part keeps its original object index and submits the
production UV offset. Force lazy mesh loading before querying tags; an earlier
preview inspected an empty tag cache and correctly failed closed, producing no
appearance credit. Current native preview has26changingnonemptyframes and exits
normally. Six actual generic map owners pass36 create/move/delete observations
with visible scroll and exact floor restoration in a synthetic ground module.


The final six-asset proof passes 84 original/production software image/depth pairs,
replayed twice, with 4,872 bit-exact vertex UV float comparisons. The candidate now
executes the actual production static mesh submission's scroll-imagery branch,
including its real ScrollTexOffset call. The native three-object fallback and
compiled strict grammar/overlap regressions both pass. Original asset geometry,
indices, textures and projection are unchanged. Natural lighting, device/sampler
fidelity and modern GPU parity remain separate.

[Run instructions and exact scope](../../../recon/retail_asm/runtime/effects/scrolltex-water-ab/README.md)
and [machine-readable proof](../../../recon/retail_asm/runtime/effects/scrolltex-water-ab/manifest.json)
retain the pre-fix compiled controller trace and original empty-selector counts.

Final native/compiled proof covers84whole-frame RGB565/depth pairs with exact
replays and4,872bit-exact UV fields. All six assets have14visible pairs each
(minimum24,985nonzero pixels); fixed1280×720 RAM framing keeps original camera,
z-distance1925, geometry and rate unchanged. Corner error is below4.73e-7.
The actual authored-mesh Submit scroll branch is compiled in the candidate.
Source/source-device lighting, sampler behavior, long-clock precision and Metal
remain separate. Native three-object fallback and strict malformed/overlap
regressions both pass. Generated report is retained under ignored
`recon/retail_asm/runtime/effects/scrolltex-water-ab/`.
