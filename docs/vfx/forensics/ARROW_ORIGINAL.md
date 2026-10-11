# Original ArrowEffect and QueenArrow

Exact bindings are `ArrowEffect:0xad92bd1f` and `QueenArrow:0xad92bd1e`, both
using `Misc/arroweffects.i3d`: seven untextured color meshes, 56 vertices and
42 faces. They are distinct original classes, not tint aliases. Complete
registrations select owner factories5097c0/509940 and animator factories
509870/509a30. Original Effect constructor4de770 executes in the owner proof;
TObjectInstance resource/definition construction46e1f0 remains an interface.

The first moving reference executes QueenArrow owner504d20, generic Animate
40e460, custom Render505000, original RenderObject/normal lighting and software
raster. Source(-180,0,38), facing64 launches at sourceZ+40 and moves23 units per
pulse. The declared stationary collision query returns a fixture character at
X>=180. Original reaction and damage calls are observed boundaries. Native
owner countdown kills at zero-based tick18; no preview lifetime is invented.

The production core matches40 original ticks, all440 state/RNG fields, and
1,312 sampled full-render matrix fields (maximum error4.742e-7). Render consumes
its own jitter RNG every call; simulation-only cadence must not consume those
samples. Native Scene8 selects ONE/ONE, CULL_NONE and ZWRITE_FALSE, but its
untextured software kernel has separate raster behavior. A port appearance
claim requires the correct existing untextured software helper policy.

Private initial evidence is in RevenantRetailLab
`research/vfx-wide-20261010/hard-alpha/{arrow-native-preflight,queen-arrow-first,queen-arrow-core-first}`.
No Metal appearance acceptance is granted by this checkpoint. ArrowEffect's
attached moving-owner path and natural arrow/bow/queen callers remain open.
