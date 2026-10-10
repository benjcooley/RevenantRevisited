# FaultFire: executed thin state/render scope, 2026-10-08

FaultFire now matches the original executable for 129 animation states and 12
nonempty two-pass software images. The fresh probe exposed two production
arithmetic differences, corrected only in `TFaultFireEffect_Bespoke::Advance`
and `Submit`. This supplements the older F06 narrative within the executed
scope; it does not close the historical opaque-versus-translucent device issue.

## Original code and authored inputs

FaultFire is a stationary `TEffect` with an empty effect Initialize, not a
missile or a point-to-point effect. Registration `0x4f1530` installs effect
builder `0x5ac8a0`; animator registration `0x4f1570` installs builder `0x5acaa4`
and vtable `0x5acaa8`. The fixture executes Setup `0x4f1590`, Initialize
`0x4f15f0`, Animate `0x4f1610`, Render `0x4f16b0` and RandomRange `0x483300`.

The shipped `Imagery/Magic/faultfire.i3d` is pinned to SHA256
`475179128ede7f08dc19c33c7cb8d5bf4a42597d964cb65a065e82995d411418`.
It supplies all four authored vertices at `0x228`, both faces at `0x2a8`
(`2,0,3; 1,3,0`) and the unchanged 128×128 ARGB4444 texture at `0x398`.
Setup stores authored V minus float 0.01 in both mutable vertices and the four
immutable animator baseline fields. Render produces two passes: their V scales
come from native x87 FCOS with phase offsets 0 and pi/2. The original matrix
scales `(0.25,0.25,1)`, rotates Z by float pi/2 and translates Z by 32.
These authored inputs and transforms are retained without fitted dimensions.

## Causal corrections and measured results

Animate keeps the theta sum in x87 across its comparison against double
`6.283185308` and possible subtraction of float `6.2831854820251465`, then stores
float32. Rounding before that comparison/subtraction changed the stored phase.
The port now preserves that intermediate. Native Animate also adds each random
U increment independently to all four stored vertices. Deferring that addition
through one accumulated scalar changed rounding for vertices initially at U=1.
The port now mutates the authored U values and submits those stored values.

The retained before report has 325 failures: 234 stored-U comparisons, 43 theta
comparisons and 48 submitted-UV consequences. Its sampled pixels already matched;
the correction establishes arithmetic parity without claiming a demonstrated
visible change in those samples. The final report has zero failures:

- Ticks 0–128: 645 exact binary32 theta/stored-U comparisons.
- 128 ordered original RandomRange calls, all with bounds `(2,8)`; the fixture
  supplies the declared MSVC CRT random stream and compiles the candidate against
  the captured native return tape.
- Render ticks 0, 1, 12, 24, 40, 62, 63, 64, 80, 100, 126 and 128: all 12 have
  1,352 nonzero RGB565 pixels, zero pixel differences and identical depth bytes.
- Authored positions agree within 3e-5 and submitted UVs within 1e-7; preview
  and map-owned compiled production submissions agree.
- Reset from the same warm snapshot reproduces all 129 original states and
  all 12 original packet/image/depth samples exactly.

## Boundaries and reproduction

The fixture declares null spell, identity owner, white vertices, camera zero,
z-distance 1925 and a 256×256 software viewport. Imagery object access, base
animator/world calls and the owner's command-completion setter are explicit
upstream boundaries. Complete constructor, loader and actual caller behavior
are not reconstructed. Original software blend Save `0x4178e0`, Set `0x417d60`
and Restore `0x417b00`, matrix/point transformation, projection and raster run;
the imagery render boundary captures submitted matrices/UVs. There is no raster
hook. Both packet sources use the same original software raster, so equal pixels
is a frontend proof, not independent modern GPU or original graphics-device
parity. Normal lighting, arbitrary owner poses, real spell contexts and the
older device-opacity discrepancy remain open.

```sh
python tools/retail_runtime/faultfire_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --ticks 128 --output <private-report-directory>
python tools/retail_runtime/test_faultfire.py
```

The focused test executes the native fixture and compiles the actual production
method spans, compares all stored U values through submitted authored triangles,
checks the two phase wraps and repeats the full native state trace. Licensed
assets and the baseline executable are required, along with Unicorn and clang++.

Local evidence is retained under
`research/vfx-next-20261007/faultfire-current-before-20261008/manifest.json` and
`faultfire-current-final-20261008/manifest.json` in the retail lab. The final
manifest SHA256 is
`fa92387916f74afa4e794a3423f85e3ee1600edd690b56f5fbcefd5be54127c5`.
The report retains baseline, asset, source-span, probe, compiler-input and
compiled-binary provenance plus native state, geometry, UV and image hashes.
This supports one bounded frontend/state row, not complete FaultFire acceptance.
