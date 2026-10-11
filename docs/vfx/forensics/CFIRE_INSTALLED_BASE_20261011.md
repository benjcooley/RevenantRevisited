# CFire: installed missing-preset base path

Exact effect `0xad92bd27`, `Misc/cfire.I3D`, now has the separate preview
`CFire_INSTALLED_NO_PRESETS`. This is the genuine installed fallback, not an
implementation of the absent CFire/CSparks particle definitions. The older
bespoke entry is preserved.

The original cached class factory, default CreateAnimator, whole owner Pulse,
Animate, BeginScene, PreRender, Render, PostRender and EndScene all execute.
Original `fopen` and CreateFileA request `Resources/cfire.txt` and
`Resources/csparks.txt`; both fail. No controller or replacement preset is
injected. The real animator contains zero controllers, so the two authored
prototype meshes remain visible: a tiny dim gray-green sprite and a separate
cross-shaped spark. Original flags kill the owner at tick 30. Ticks 1–29 each have
two real FVF112 draws and 112 nonzero pixels; ticks 30–64 are blank. Fresh complete
64-tick runs are identical, including all matrices, file outcomes, images and
zero RNG consumption.

The source guard requires the exact four objects, compact keys, geometry,
materials, two filename tags, 30-frame nonlooping state and two strict ARGB4444
textures. It submits only the two existing textured meshes, in original order,
using the original source-alpha/CULL_NONE/depth-test-without-write policy.
Existing software-policy source contribution quantization is reused; no shared
renderer/parser changes. The explicit native-domain configuration uses the
proven native owner and projector. Ordinary common projection is preserved.
`IsAlive` reads the actual owner OF_KILL flag; there is no invented timer.

Validation: private full build passed; ASan/UBSan production profile and key
matrix harness passed 13 rejection controls and 1,856 original matrix channels,
maximum absolute error 5e-8. Both actual 64-frame Metal films exit cleanly with
29 nonblack frames and the real blank tail. A retained unit test executes the
loaded original path and source comparison. The native-domain pair has about
one pixel of edge displacement; low-alpha destination quantization and
interpolation remain open. Missing expected particle definitions, natural
caller/map, other devices and exact framebuffer equality remain unclaimed.

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-alpha/`.

- `cfire-loaded-first/manifest.json` and `cfire-loaded-repeat/manifest.json`:
  `ddda46d6b9ed52643bb573a96af67a6b2f1863079c7122b4c4fce2950e6440f3`.
- `cfire-source-final/manifest.json`:
  `0cbe17dfebda24e218f2586cdcd6ea0cd230bad983e25cdbde616aae6dd45bb5`.
- `cfire-metal-native/manifest.json`:
  `ac8bc4ec104c04ebefa75f79a12767d00ccf97eb5945484cc9aca5ec22a4b96f`.
- `cfire-metal-common/manifest.json`, diagnostic:
  `be62aa80ee60643f209d7f1689741fd763ccd8267038405de5b6df0c5829bf8b`.
- `cfire-review-final/pair-manifest.json`, 37 verified pinned files, ten phases:
  `4f61851c68ed23cc757918e494b1d0bfb2c063be191c55f205e1616177d2c21c`.
- Actual Metal binary:
  `afdf4e3e35e8602b4a2af9bec0014ed970eb5bb2ff89ff7157da6d500d8ea739`.
- Exact asset:
  `f917d2502e7855bdbe36a3bd7c203c2d4cb8c8a01422180f549ef9e6bdc3d9f6`.

`cfire_visual_review.py` uses literal same-tick pairs, a fixed identical crop
and nearest-neighbor 8x presentation. It retains full raw pairs and makes no
brightness, geometry, phase or camera adjustment. Independent root review,
integrated repeat and gallery routing are separate acceptance steps.
