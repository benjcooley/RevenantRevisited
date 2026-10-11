# EnergySpray original null-spell composite

Exact shipped `energyspray` / `0xad92bc1c`, `Magic\\Energyspray.I3D`, SHA256
`bdca48f08b388c385c78acf6e3b01e35ca5d7969ff910b8a9dd2dbd66f1ac75f`.
The strict adapter retains its two authored ARGB4444 rectangles and untextured
triangle: 11 vertices, five faces, three materials, two textures, no tags,
one 100-frame state and V2 compressed keys. It does not use a shared-asset alias.

Original registered builder `66cff8`, factory `509390`, vtable `5b0a28`, owner
Init/Pulse `502b40/502b70` and leaf Init/Animate/Render `502c50/502da0/503110`
execute in the reference. Null spell is an actual branch: it skips actor-hand
attachment and damage/knockback, and leaves six anchor coordinates zero.
Phases last60/50/25 ticks; original owner completion kills at136. Initialization
consumes160 original MSVC random draws; each actual Render consumes one core
scale jitter draw. Radial births have their own exact RNG ordering. No Render0
is inserted in the appearance fixture: its first Pulse/Animate/Render is tick1.

The port independently reproduces 25 inward radial cards, central jittered
flare, and20 colored moving triangles. It preserves the literal radial
Y rotation64 radians, signed-byte color updates and their alpha sign extension,
original matrix operation order, triangle local translation and original draw
order. Its live owner completion callback supplies gallery repetition. Actual
Advance lifecycle testing covers external owner death, guard-removal negative
control, native kill136 and post-kill no-op. Render RNG occurs once per actual
submission frame; skipped-render stride3 is separately proven.

Original/full source state proof: 87,984 fields, all RNG seeds/counts exact,
maximum float error4.90e-7. Independent matrix proof: 82,096 fields, maximum
5.01e-7, plus2,220 exact packed color writes. Two full141-tick native replays
match every state, draw packet and framebuffer hash. Both actual140-frame
Metal films have135 live phase/countdown/RNG/draw rows matching the original.

ARGB texels ignore incoming normal-lit color. Literal seven-word original/GPU
controls justify reusing the existing source-contribution floor only for the
explicit native-domain effect. The untextured original uses prelit RGBA5 with
flat maximum-vertex depth and writes depth despite caller ZWRITE0. The separate
`PRELIT_RGBA5_SOURCE_20261010.md` records its default-false helper and384 native
alpha/color controls, actual GPU controls and unchanged default diagnostic.

The source textured and untextured rasters reject a triangle when either edge0-to1 or0-to2
exceeds its literal640/480 span. At tick111, changing only those original limits
to6400/4800 restores233,018 pixels from zero, with identical state, RNG, vertices,
colors and matrices. The larger supported viewport exposed the same guard in the untextured branch
`56d9fd..56da7f`. All20 original per-triangle decisions at110 match the compiled
source helper. Widening only the constants increases12,165 to367,520 pixels in
that1920x1080 negative control, with all state and draw inputs identical.
The adapter applies the already proven Comet admission helper to both formats
only for native-domain plus software lighting. Common-world geometry
remains complete; there is no fitted scale or deleted outer component.

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra`.
The frozen640x480 fifteen-phase contact is `energy-review-final`, manifest
SHA256 `62e3f7eb54282b254923f48dca7e683c246327c42452ba49625ed850802f230a`,
original comparison common/native-domain Metal binary SHA256
`0897cd4317011b85201dcd29eec24c49e61bb20cebdb5efffde14a30d46dbc30`.
It records birth, phase transition60/61, fan, native primitive rejection,
shrink120/130/135, kill136 and blank140. Its outer fan is clipped by that
viewport; supported larger overlapping views are recorded separately before
appearance acceptance.

Scope: facing0, null spell, owner/camera world0, seed1,24Hz, black background,
source ambient38/directional1. Reference boundaries are immutable decoded
resources/COM texture/material residency, base bookkeeping/extents and owner
script/light/map-flag services. Original triangle ABSPOS and zero anchor are
retained in the isolated fixture; the gallery translates the whole standalone
fixture to its station. Natural actor hand/spell target/damage/audio, nonzero
facing, natural caller, end-to-end device behavior, framebuffer destination
rounding and exact pixels remain open. Common projection/full oversized
geometry are separately retained and are not native-domain appearance claims.
