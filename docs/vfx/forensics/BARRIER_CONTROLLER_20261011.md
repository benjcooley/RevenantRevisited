# BarrierEW and BarrierNS controller checkpoint

The compiled source in `src/barriercore.h` reconstructs original SetupObjects
`4e7550` and Animate `4e7700`. It preserves all 30 slots, the four movement
directions, inclusive boundary tests, first-empty-slot allocation, integer
truncation of vertical spawn coordinates, flare selection and spawn delay.
Setup computes bounds and average Y from the immutable own wall vertices.

`tools/retail_runtime/barrier_state_probe.py` executes each actual original
constructor, Setup, Animate and every intervening Render through 600 ticks.
All 149,649 scalar fields per resource agree, including exact integers and
MSVC RNG state. Maximum printed-float residuals are 3.2519532e-8 for EW and
4.2822266e-7 for NS. The compiled core runs with ASan and UBSan.

The first probe incorrectly consumed a random value for equal endpoints.
EW's integer Z bounds are both zero; this exposed immediate stream divergence.
The corrected helper returns the endpoint without consuming CRT RNG, as in
the original integer helper. The first failure remains private evidence.

Private evidence is under
`RevenantRetailLab/research/vfx-next-barrier-20261011/`:
`state-first` retains the failure; `state-equal-range` contains the complete
600-tick successful comparison.

The [zero-scale audit](BARRIER_ZERO_SCALE_20261011.md) independently proves that
active flare objects request zero Y scale and fail original CalcObjectMatrix.
They still consume random values and advance their pool state. Only the wall
draw reaches the original software raster. Do not make these flares visible
by replacing their scale.

The independent strict predicate in `src/barrierlegacykeys.h` admits only the
two complete immutable retail bodies. Each resource passes four path forms
and rejects every byte mutation plus nine header/path/null/cross-resource
controls (25,253 rejections per resource) under ASan/UBSan. The full-body
fingerprint includes all five keys, parents, geometry, materials and textures.
`barrier_profile_probe.py` reproduces this contract; private results are in
`profile-first`. The narrow loader path now copies the five eight-byte keys.
The extracted actual production branch passes 90 pose channels against
original409950, complete-body negatives and unchanged zero-key defaults.
Ribbon, Vortex and Smoke native key regressions all remain passing.

The preview retains one fixed wall mesh per resource and advances the complete
flare pool even though the zero-Y flares do not draw. Both 31-frame actual
Metal movies render and exit cleanly. Their recorded 24Hz cadence covers
91 simulation ticks, saving every third frame. Original references render
the same cadence without an extra initialization draw. The first EW pair
matches the blue wall and texture placement, with a one-pixel edge offset.
Full paired review and repeated/gallery captures remain pending.

Original wall Render sets POS1 and position zero; its actual local matrix is
identity after CalcObjectMatrix, rather than the authored key transform.
The GPU uses a uniform accumulating UV offset; the original adds0.01 to each
copied vertex separately. Differences in float rounding remain open and
must be checked in the complete comparison.

This checkpoint adds no visual pass. Whole appearance, UV rounding, natural
caller/audio/device and exact framebuffer equality remain open.
