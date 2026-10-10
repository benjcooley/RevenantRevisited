# Blue Warp: current modern-port visual review

2026-10-10: **overall isolated blue Warp appearance passes for the tested
configuration**. Lane 3 and root independently inspected the original-software
reference against fresh actual Metal map footage. The triangular form,
blue/white cloudy perimeter, four changing atlas states and transparent floor
integration are close. No opaque blue block or frozen atlas remains. This
decision applies only to `TeleportDoorInsideB` (`0xad92bc28`), not the other six
colors or full gameplay acceptance.

The [machine review and provenance](WARP_BLUE_VISUAL_20261010.json) records the
decision, paired image hashes, binary, asset, fixture and reference manifests.
The retained [same-floor sheet](/Users/benjamincooley/RevenantRetailLab/research/vfx-next-20261010/warp-blue-modern-visual/same-floor-native-modern-sheet.png)
is the primary visual evidence; the [black-reference sheet](/Users/benjamincooley/RevenantRetailLab/research/vfx-next-20261010/warp-blue-modern-visual/first-native-modern-sheet.png)
helps distinguish portal detail from the backdrop.

## Tested configuration

The pinned modern executable is SHA256
`7a377760d05d3a006f3578e825e30208a20d324862e6c4f554d3a90730124fc6`.
It renders the real map-owned blue Warp on the synthetic FontRuntimeLab floor,
at camera `(110,10000,10000,16)`, white ambient 32, 640×340, 24 Hz, seed 1 and one
warmup frame. All ten typed create/frame/move/delete observations pass. The
16-frame capture contains four distinct stationary atlas samples and restores
the exact original floor after deletion, with a clean process exit.

The independent original executable runs the actual Warp Init/Animate/Render,
key/matrix/point functions, software projection and alpha raster. It retains
the four authored vertices and **all three triangles**, including the overlap,
and the pinned 256² ARGB4444 texture. Reference camera/owner are origin zero,
viewport 512², Z distance 1925; successive U offsets are 0,.25,.5,.75. Modern
frames 3–6 correspond to native ticks 0–3: map creation occurs after that tick's
Pulse, then each subsequent Pulse advances U once.

For the same-floor sheet, the clean modern floor is quantized to RGB565 and
supplied as the original software destination before the original alpha raster
draws. It is a shared inspection backdrop, not an original-retail map scene.
Fixed 200×210 crop windows recenter the declared viewport origins. There is no
effect-pixel alignment, rescaling, geometry, phase or color fitting.

Native illumination input is explicitly white 31 per vertex with the authored
white material. The modern scene uses ambient 32 white, but Warp ConfigureDraw
sets `retail_lighting=2`: Metal mode 2 preserves authored ARGB texture color and
bypasses normal and deferred-light modulation. Ambient policy is therefore not
used to excuse a brightness difference. The sheet retains RGB565 quantization
and raster-sampling differences without fitted tint or brightness changes.

## Open projection discrepancy

After viewport recentering, native effect bounds are `(268,64)..(369,195)`;
modern bounds are `(269,57)..(369,195)`. The modern apex is **7 pixels higher**,
and its height is 138 rather than 131 pixels, about **5.3% taller**. Width and
lower edge are close. This known renderer projection discrepancy remains open;
the visual decision follows the user's fast-software/known-rendering-loss
policy and grants no exact geometry or device pixel parity.

No production correction was made. Natural teleporter activation, original
scene illumination/background, saved visibility/story behavior and full effect
acceptance remain outside this scoped appearance review.
