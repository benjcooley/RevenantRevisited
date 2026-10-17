# Cataclysm: original custom leaf

Exact identity: `cataclysm` / `0xad92bc19`, `Imagery/Magic/cataclysm.i3d`.
Asset SHA-256: `a1d1a426a1c65c7254ce520d01845e54c37bfbeeec4ef4117fc066cf8b23e102`.
The admitted resource has five objects, five materials, four textures, no tags,
and one 100-frame state. Its compressed keys are version 2 with flag `0x80`.

`Cataclysm_ORIGINAL` reconstructs the independently executed custom owner and
animator. It preserves the falling rocky head and glow trail, impact dust,
expanding torus, fragment glows and rocks, decay, and actual life-80 kill.
`is_alive` observes the owner kill flag; the preview has no replacement timer.
The existing bespoke preview remains available. Gallery integration selects the
new exact identity separately.

## Original execution and state

The pinned executable is the standard rebuilt retail baseline, SHA-256
`28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.
The fixture executes owner Initialize `4fe9e0`, SetUp `4fe800`, Pulse `4fea00`,
animator factory `500b50`, Initialize `4fec80`, Animate `4fef10`, and Render
`4ff170`. Full original RenderObject, vertex transform, normal illumination and
software raster remain active. Native registry lookup identifies builder
`66cfe0`, factory `500b50`, and animator vtable `5aefb8`.

The original pools contain **30 pieces, 70 dust slots and 50 glows**. The excluded
old source's 40-piece definition is incorrect for this executable. Initial pool
construction consumes 673 CRT draws; the complete null-spell lifetime consumes
826. All 110,565 pool fields across initialization and 90 ticks compare exactly,
as do owner life/phase/position/velocity and every RNG seed. The retained
geometry proof compares 13,328 matrix fields (maximum difference below
`9.6e-7`), original draw order and actual packed colors.

Original owner Pulse already dispatches custom Animate through its base chain.
An early diagnostic that also called Animate explicitly is excluded. A second
fixture correction removes missile-only fields from the derived owner extension:
allocator `482fb0` actually zeroes the allocation. Dirty-memory controls execute
that original zeroing code. Null spell therefore leaves blend variant zero;
missile launch-ready at the same offset must not select named-spell variant one.
The final reference and an independent fresh run have identical complete states
and 15 shared pixel samples. Skipping Render at stride three leaves all 90
original states and RNG seeds identical.

## Rendering changes

Five immutable meshes supply the original geometry. Dust reuses the bounded
prelit-color uniform, avoiding a mesh cache per changing color. The torus uses a
new explicit helper input format, mode 6: RGB565 texture plus constant RGBA5 in
the existing diffuse uniform. It preserves authored UV and bypasses normal
lighting only for that declared prelit input. Three production fragment branches
compare against 7,616 original source-alpha kernel literals, covering 68,544
channel checks. Alpha zero skips; other alpha levels use `(A5+1)/32` and truncate
the source RGB565 contribution. Destination blend precision remains open.

Original additive Scene8 is CULL_NONE. A default-off helper option selects one
cull-none additive pass in authored triangle order. Ordinary helper defaults
retain their previous front/back passes. All Cataclysm draws share their owner
sort depth, retaining original dust → torus → fragment glows → rocks ordering.
The default helper/UV/pipeline regression and existing exhaustive packed-color
uniform test pass. Build and three Cataclysm runtime tests pass.

The native-domain comparison alone uses the existing proven source primitive
span guard and original positive 16-bit depth range. Common rendering retains
full geometry and the common projector. No geometry, color, seed or timing fit
is applied.

## Evidence and remaining limits

Private evidence is under
`research/vfx-wide-20261010/hard-alpha/cataclysm-*` in RevenantRetailLab:
`cataclysm-native-final`, `cataclysm-source-final`,
`cataclysm-geometry-final`, `cataclysm-shader-proof`,
`cataclysm-fixture-controls`, `cataclysm-native-repeat.json`, and
`cataclysm-metal-freeze`. The review tool produces fixed 16-phase whole-frame and
actual-size detail panels and pins their images, reference trace and binary.
Root independently reviewed all 16 whole-life and detail phases and approved
the scoped native-domain, null-spell variant-zero appearance at 1280×960.
The falling head/trail, torus/impact, fragments, glows, dust and blank tail align.
All native and native-domain sample bounds fit the complete viewport. Common
launch brightness and clipping remain diagnostic, without a common appearance
claim. Gallery exact-route and life-80 restart validation is an integration check.

Frozen `cataclysm-review-final/root-approval.json` SHA-256:
`80c10aa098e941db8ec4db51e0aa8e7aaff716ce339bc2d74d2e6372c5b5e070`. It pins pair manifest
`4117d17b0b929fc7f73af54696b21bba636dad8577348a36e2be259e8f9ffa92`,
cases `b93a0420e7614a932f42ba9f22a3abb4d708f97b83924063e28d05c52348d517`,
and captured binary
`501cffa3d25a4889d4d5829ab804a80b589569da6d103fa8e1a765eb7033ab6e`.

This is the null-spell, empty-character-world leaf with a fixed camera and
explicit resource/COM setup. Base animator initialization, audio and map/script
notifications remain fixture boundaries. Translation relocates the same empty
world for the gallery; actor selection, damage, named spell variants and natural
caller integration are not certified. Original/GPU scanline, destination blend,
backend and exact-framebuffer differences remain explicit.
