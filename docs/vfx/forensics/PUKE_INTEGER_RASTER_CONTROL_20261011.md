# Puke: integer raster inputs, four-policy differential

Exact `puke:0xad92bc11` keeps its previously approved null-spell controller,
three authored ARGB4444 meshes, timing, camera, source lighting and draw order.
Only explicit native-domain software rendering now selects the existing
`retail_software_pixel_grid` helper flag. Common projection and hardware policy
remain unchanged. No shared renderer or shader is edited.

## Executed source evidence

`puke_raster_packet_probe.py` observes the original transformed FVF112 vertices
at `56d960` and actual ARGB4444 triangle packets at `54e050`. All 3,000 packets
(45,000 XYZ/UV channels) match truncation toward zero of the original floating
XYZ and UV times actual texture dimensions. The authored UV endpoints are
exactly 0/16, so no additional texture-coordinate adapter is needed.
The observer runs the entire original raster: all 33 states, complete draw
packets and framebuffer hashes remain identical to the unobserved reference.

The existing helper grid maps integer raster inputs to GPU half-integer sample
centres using its previously proven +0.5 convention. It also truncates depth.
The existing renderer gates the grid on both native projection and software
lighting. Puke still uses its original source-alpha/cull-none/no-write route.

## Four-policy actual GPU control

One binary captures all four combinations of grid and source-contribution
quantization, 32 frames each. The native has 25 draw calls at both ticks24/25,
but only 1/0 nonzero pixels. Default GPU has15/7; grid-only0/0;
quantization-only15/7; both0/0. Thus extra late pixels follow geometry/sample
coverage, not the contribution floor. The grid still misses retail's one
tick24 pixel: this is not an exact raster fix.

Across the fixed fifteen phases, aggregate absolute RGB error is29,942 for
default,8,925 for grid-only,30,179 for quantization-only and9,937 for both.
Grid-only improves this diagnostic error at every active phase. These metrics
are evidence of the independently proven input correction, not a fitted
threshold or a substitute for appearance review. The source-contribution flag
stays off by default; the four-policy experiment remains reproducible through
the explicit `REV_PUKE_RASTER_DIAGNOSTIC=0..3` control.

All19,332 actual source state/slot/RNG fields checked across those four films
match original values. Fresh ASan/UBSan core proof passes5,940 fields. The actual
Advance kill/birth-RNG guard and compiled actual raster-policy test pass, as
does existing helper emission/UV/default-policy validation. No simulation or
RNG changes accompany the flag.

The current default GPU32 frames equal grid-only32 exactly. The diagnostic
baseline equals all32 previously approved PNG pixels; the new common-domain
32 frames equal the pre-change common film exactly. Invalid diagnostic values
select the same grid-only default, independently tested and captured.

## Frozen evidence

Private base:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/`.
The following are SHA-256 of each directory's `manifest.json`:

- `puke-raster-native-current`:
  `6b950682f0b7d426f6e92fafeae8a6db35afd0926c75323ad4e74d4c224f9494`
- `puke-raster-packets-final`:
  `a791ca323ddb98366e54984c724d440b2872815dfa10891d7a1a27eb3f279e71`
- `puke-raster-state-current`:
  `386cbca7bbbc1d7549abfaf4172a2b8d6c98ccd77209bb080b670b4955681d3c`
- `puke-raster-policy-review` (whole/contact and identical fixed crop):
  `a02038920260d53291387d0221353f858fa14e3b467dd9dd2cb615d2bab64dff`
- `puke-raster-default-metal`:
  `f3cf67cebe3d939074137d98c840ef16b195a3d7343920d0e6db9d24bcfbd01d`
- `puke-raster-common-after-metal`:
  `018b758186c4f084adf964595412dd4610c1c331eca8405cd93ccbe8e0c89dc2`

Final binary:
`087da8834095998b89a40631071dcefc6741547fa6f223db0dbeeeb2d47d7d75`.
The four-policy binary was
`960f474ce17f6c6baa2500cb63692d5047b59cbe2285168b31038bfd7feb964b`.
`puke-raster-final-regressions.json` pins all three32-frame equality controls.
Earlier packet-observer attempts are retained: their assertion compared integer
JSON dictionary keys to serialized string keys. The corrected observer
canonicalizes serialization; it changes no guest execution.

The new native-domain primary capture needs independent root review/repeat.
Natural actor-mouth attachment, destination RGB565 rounding, fixed-point scan
conversion/interpolation, original hardware and exact pixel identity remain
open. Halo is the next independent single-mesh candidate; this Puke result does
not authorize enabling the grid for other effects.
