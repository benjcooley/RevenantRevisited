# Literal Snow default mesh and legacy key correction

CLASS EFFECT Snow `0xfacedbed` uses `Magic/Iced.i3d`, SHA256
`0a2cdd9255322792ecb15f80bec3330a04aac363232c8adf5c40570949ab9ac8`.
Executed original registration resolves default builder `0x5e8508`, factory
`0x40dc00` and animator `0x5a370c`. Iced independently resolves its custom
actor/shatter controller. The old Snow preview incorrectly reused Iced;
it now creates the real typed Snow owner and renders all 50 authored roots
with its exact default profile. All 100 legacy keys per root are constant.
This is static blue authored ice geometry, including its detached cube;
there is no snowflake, victim or shatter animation claim.

Current Clang allocated the old unsigned-int bitfields as twelve bytes,
while the serialized/native record has two 32-bit words (eight bytes).
Both record stride and Z's cross-word extraction were wrong. `SAniKey`
now stores those exact words, asserts its eight-byte size and uses native
bit extraction. Setter behavior is retained, including RZ clearing the
unused top bit; Z's high-byte write uses an unsigned shift under UBSan.
Existing loader allocation/copy uses the corrected size. The old-body
zero-key fallback policy is unchanged.

Evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-next-map-controller-20261010/`

- `legacy-key-contract-v2/manifest.json`: all 50×100×9 pose channels match
  original `0x409430` exactly (45,000, maximum error zero), plus all 13,824
  representable setter values and unrelated-field preservation under ASan/UBSan.
- `native-snow-full/manifest.json`: original empty factory array preserved;
  whole base Initialize/SetupObjects/legacy keys/owner/full base Render/
  RenderObject/FVF/software raster. Exactly 50 real native-created draws
  and 15,895 nonzero pixels per phase. Two fresh VM replays agree on all
  six whole image/depth/draw sets; every transformed vertex is unclipped.
- `metal-snow-final/manifest.json`: actual typed-owner Metal film, 60 frames,
  one distinct static image, clean exit. Seed1, 24Hz, warmup1, origin/camera0,
  1280×960 and source ambient38/directional1 explicitly declared.
- `map-final/Snow/manifest.json`: same binary, 16 real map frames and ten
  create/frame/MOVE/position/delete/absence rows pass; visible static mesh,
  100-frame owner loop and exact restored floor.
- `review-final-v2/manifest.json`: unchanged six-phase full native/Metal
  pairs and whole bounds. Fixed-center `(384,288,896,672)` supplements use
  no rescaling and never replace full images. Root independently approved
  the full/fixed-center appearance and final static film; approval/freeze
  records pin every sample and capture binary.
- `snow-regression.log`: existing compressed decoder, generic profile and
  actual custom legacy-water fixtures checked against the changed header.

The source profile pins the exact type/path/header/root names/parents,
all legacy key bytes, geometry, face bins, materials and both texture
descriptors. Other generic profiles retain their four-byte key policy.
No shared renderer/shader change or image fitting was made. Native decoded
resource/one-frame COM residency/material handles/nullspell command methods
are explicit boundaries. Native writes depth while the existing isolated
helper tests depth without writing; internal face/edge, device, natural
caller and exact-pixel gates remain open. Full acceptance stays false.
