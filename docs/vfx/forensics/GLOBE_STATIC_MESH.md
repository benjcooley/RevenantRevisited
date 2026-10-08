# Globe: authored static mesh and retail A/B

Verified 2026-10-05. Retail type `0xad92bd24`, asset `misc\Globe.I3D`.
The former preview was an invented 32-unit blue pulsing billboard. It has
been replaced with the shipped mesh, texture/UVs, fixed pose and ordinary
mesh submission. The actual map type already uses a generic EFFECT and
its default animator; no custom Globe component or guessed effect definition
is needed.

The shipped asset SHA256 is
`c43170c7e6cf1dd59146bfd5446a46a70eb2b9f45da75ec508198c8ce4552514`.
Its one object `circle01` has 28 vertices and 26 triangles. STILL loops over
100 frames with a single constant transform, no controller tags and one
ARGB4444 texture frame. The texture itself contains the pink/yellow glows.
The native software recording is visible but unchanged below the diagnostic
FPS band for its 10.285 seconds. Static output is expected; it must not be
rejected by a universal changing-frame test. Empty-floor output is still
rejected by the new static capture validation.

Evidence is under `/Users/benjamincooley/RevenantRetailLab`:

- `research/quick-globe-20261005/asset.json`, extracted mesh/texture and build logs.
- `captures/runs/sw-quick-globe-20261005/`: original native lossless recording,
  before/ground/restored-ground images and hashes. Background error and
  restored-ground error are exactly zero below the FPS band.
- `captures/ab/sw-quick-globe-20261005/`: retained wrong-billboard baseline.
- `captures/ab/sw-quick-globe-fixed-20261005/`: failed helper-material diagnostic.
- `captures/ab/sw-quick-globe-normal-mesh-20261005/`: corrected authored preview,
  with a baked background that cannot supply floor depth.
- `captures/ab/sw-quick-globe-actual-map-20261005/`: accepted visual review of
  the real named map object against retail; no geometry/position/brightness fitting.
- `captures/runtime-fixtures/globe-command-20261005/command-capture-final/`:
  final-binary real ADDAT/MOVE/DELETE; six command/observation rows pass,
  one default animator, three scene states, stable deleted tail.

The floor is part of this effect's appearance. Its lower mesh is below ground
and is occluded by real tiles. A baked PNG has no depth and exposes a lower
rim; the opaque mesh pass can also expose dark low-alpha texture regions
against that depth-free background. Comparing this preview alone would
misdiagnose the authored geometry. The actual-map comparison shows the two
matching pink/yellow forms and the correct floor occlusion. The port uses
smoother filtering than the retail16bit software raster; pixel identity is not
claimed. Source-authored mesh geometry and real runtime lifecycle are verified,
and visual appearance passes in the actual synthetic floor context. A natural
shipped placement/story-context check remains separate.

The preview retains its imagery while GPU mesh resources refer to its texture,
releases the mesh reference on destruction, samples the existing static-pose
helper and composes the owner's transform. World submission runs after
BeginTilePass, matching the ordinary mesh rendering scope.


## 2026-10-07 thin-runtime original/production static frontend pass

Globe now also passes the original-executable versus compiled production
static-mesh frontend batch in
`recon/retail_asm/runtime/effects/static-mesh-frontend-ab/manifest.json`.
Its sampled0/50/99 poses and identity/translated owner placements replay twice
with zero differing RGB565 pixels. Every case has visible output; each placement
is static across all sampled frames, and translation changes its image.

The exact Globe asset above supplies28raw vertices,26triangles, originalUVs,
ARGB4444 texture and six scalar keys. Actual original GetAniKey409950, decoder
409430, CalcObjectMatrix40a420 and original matrix/projection/raster execute;
compiled production key/matrix/mesh extraction and generic SubmitMesh bodies
run with explicit asset-provider/owner/renderer boundaries. No billboard, pose,
dimension, color or brightness fitting is used.

This is an isolated frontend/software-raster check and preserves the existing
map/backend limits: its black depth-free background exposes the authored lower
rim, while actual floor tiles are needed for its proper appearance. Scene
illumination, map culling/depth/placement and modern GPU output remain separate
from this bounded pass. The prior actual-map visual acceptance is retained;
natural shipped context remains open.
