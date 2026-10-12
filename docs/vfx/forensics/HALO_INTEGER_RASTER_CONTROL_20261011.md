# Halo: bounded integer-grid and source-contribution correction

Exact `Halo:0xdedbdbed` now selects two existing helper flags only in its
explicit native-domain software policy: integer projected XYZ/sample lattice
and ARGB4444 source-contribution quantization. The two declared public cases
remain `InitParams(60,.04)` and `(20,.08)`. No geometry, scale, controller,
lighting, camera, timing, shared renderer or shader is changed.

## Independent original controls

The actual loaded original owner/animator renders all70/30 frames, with
read-only hooks at `56d960` and the selected ARGB4444 kernel `54e050`.
The two runs preserve all prior owner/controller/draw rows and all15/9 old
sampled images. Their936 triangle packets/14,040 XYZ/UV channels confirm the
original truncation toward zero before rasterization.

The exact128×128 Halo texture has16 distinct ARGB4444 words. All16 words plus
four diagnostic controls are drawn through the original Scene2 kernel with
incoming RGB5 zero and31:40 actual pixel controls pass. The source term is
floored in RGB565 before blending; alpha-zero white remains nonzero. No color
key, fitted threshold or brightness multiplier is introduced.

## Four-policy actual GPU differential

One binary renders both parameter cases with default, grid-only,
source-quantization-only and both policies. All1,312 logged source
scale/frame/done/RNG fields match original execution; RNG remains zero.
The old default70+30 frames are pixel-identical to their frozen approved films.

Full-film aggregate absolute RGB error for the60 case is90,782 /75,902 /
68,477 /51,051 in that policy order. For the20 case it is18,259 /14,095 /
14,765 /10,287. Both independently proven corrections together reduce the
combined diagnostic error109,041→61,338, about44%. This measures a causal
improvement; it is not a pixel-equality claim or a fitted selection rule.

The final default native captures equal the both-flags diagnostic100/100
frames. Common-policy before/after captures are identical100/100 frames.
Source flags are gated on native projection; the existing renderer further
gates the integer lattice on software lighting and only consumes source
quantization in its original ARGB4444 mode. Hardware/default semantics remain
as before. The explicit `REV_HALO_RASTER_DIAGNOSTIC=0..3` control retains all
four comparisons; unset/invalid values choose the new default3.

Current-source ASan/UBSan core tests pass412 state fields and1,248 local matrix
channels. The actual submission probe still passes1,872 vertex/156 draw
controls and now also tests all four policy values, native/common gating and
default/invalid diagnostic inputs. Existing helper UV/default-policy tests pass.

## Frozen artifacts

Base: `/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/`.
The following SHA-256 values pin each directory's `manifest.json`:

- `halo-raster-native60`: `3a28b58e9b02acfd432a5985b005b08204d7347ccba9bf1a0887cb2a252ef0e5`
- `halo-raster-native20`: `4a2bd4cf820f7101b6a4a61bbe17e3fbfd95427f719031f10a94427c868683af`
- `halo-raster-alpha`: `e912a615c90c5ef18270110b8e42d80394c2acea946fd1ddb93c6ac35472e670`
- `halo-raster-60-review`: `43ea86c43c1b8870227c8f03664405ef460f0bbf6fddc1cb125037a9d20c9455`
- `halo-raster-20-review`: `7982e22056c2078b1ae90cad406d463ae6f56ac7d0e006ca293b9fb0c9e093ae`
- `halo-raster-60-default-metal`: `001d52bccf383af2c56b83c090f1e54b049a3c01243d43439db33f2abdecc899`
- `halo-raster-20-default-metal`: `e53fc584b6343b316cefe87640cd16082e59631fb39bfdac4ddce7f67685013e`

Final binary: `64be0a5411f5750a6da37897916854b2d320d857fb85866f4750d22dd414184e`.
The four-policy binary is `ec3b322ae7b05aba9ffa880a5e4b182b1176ffc48060c10ceebe61a69a8f2e96`.
`halo-raster-final-regressions.json` pins both native100-frame equality and
common100-frame equality controls. Each review retains whole viewport pairs
and the unchanged64×64 ROI displayed at4× nearest for inspection. All full
native frames, not just the selected panel phases, are retained.

## Deliberately remaining limits

The original also truncates near-one authored UVs to127 for the128² texture.
The GPU retains the authored fractional UVs: this patch uses existing helper
flags only and does not change mesh UVs. Remaining UV quantization, fixed-point
scan conversion/interpolation, approximately one-pixel coverage and destination
rounding stay explicit. No further sampling change is inferred from this audit.
Natural caller/spell/map-animator inputs, other lighting, original hardware and
exact pixels remain open. Root independently reviews and repeats the new
scoped primary capture; this document does not create an approval.
