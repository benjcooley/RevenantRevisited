# Literal Waterfall and Water: whole native and Metal appearance

The literal CLASS EFFECT rows are Waterfall `0xa907dabf` and Water
`0x1903abcd`, both bound to shipped `Misc/Water.i3d`, SHA256
`ece24f00eab927662fea2027dde203afec2525471eb6cde7b2a06f69c9b9aa75`.
This is the custom 100-drop vertical fall and 25-drop horizontal stream,
distinct from the four authored PartSys WaterFlft/Frt/Clft/Crt rows.

The original full registration constructors resolve Waterfall builder
`0x66cc10`, factory `0x4f9a40`, animator `0x5ad26c`; Water resolves
`0x66ce98`, `0x4f9c30`, `0x5ad4e4`. Actual base constructor and array
constructors, base Initialize/SetupObjects, authored warmup, all Animate
ticks, owner matrix, complete custom Render and RenderObject execute.
The original chooses authored FVF_VERTEX normals, RGB565 lighting,
ONE/ONE blending, depth test/no write and CULLMODE=1 (none). Copied
LVERTEX grayscale work is executed but not used by that draw path.

The production fix caches the exact shipped four XYZ/normal/UV vertices
and indices `(1,0,2,2,3,1)`, then uses the existing normal-lit RGB565
additive helper. `literalwatermesh.h` admits only this path, header,
root, parent, vertex/face hashes, material floats and texture descriptor.
Other assets and debug modes retain their previous quad path. Live map
owners keep their common-world placement and ordinary camera. The
explicit isolated preview native-domain switch builds the original owner
matrix and chooses the existing source software projector. No renderer
or shader changes, scale adjustment, lighting fit or phase fit were made.
The gallery now separates Advance and world Submit, as existing helper
effects do; its prior late FX submission could not draw helper meshes.

## Frozen evidence

Evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-next-active-controller-20261010/`

- `native-water-full-v3/manifest.json`: two independent fresh VM replays;
  all 61 complete pool/RNG traces and six full image/depth/draw sets per
  type agree. Waterfall has 97–98 draws and 5,429–5,928 nonzero pixels;
  Water has 24–25 draws and 1,509–2,100 pixels. Every transformed vertex,
  including zero-area birth quads, is inside the fixed 1280×960 viewport.
- `full-state-compare-v3/manifest.json`: actual compiled Initialize/warmup,
  InitParticle, UpdateStuff and Advance match all nine float channels
  bit-for-bit, time and exact original range-RNG request order over 61
  ticks: 61,061 Waterfall and 15,311 Water checks, zero errors.
- `mesh-contract-v2/manifest.json`: actual admission compiled under
  ASan/UBSan, three positive path cases and 84 independent rejection
  cases across every geometry/material/descriptor channel.
- `metal-water-final/{name}/manifest.json`: 60 actual Metal frames each,
  all visible and distinct, seed1, 24Hz, warmup1, origin/camera0 and
  declared source ambient38/directional1. Full 1280×960 images are paired
  with native captures 1,5,15,29,45,59 without alteration.
- `map-water-final/{name}/manifest.json`: same binary, 48 actual map
  frames each, ten passing create/frame/MOVE/position/delete/absence
  observations, twenty distinct stationary frames, exact restored floor.
- `review-water-final-v3/manifest.json`: whole six-phase panels, hashed
  unchanged pairs and bounds; fixed-center supplements use rectangle
  `(384,288,896,672)` without rescaling and never replace whole frames.
- `final-tests-v2.log`: three focused admission/native replay/Water state
  and clock tests passed. `adapter-final-regression.log` records the
  existing static particle/Quicksilver/Speed/scroll fixtures under the
  same optional adapter defaults. `source-freeze.json` pins source,
  build and every manifest. Per-type independent root approval records,
  when present, pin exactly these pairs and scope.

Capture binary SHA256:
`2370d85b1047fe39bdcaaddb9f43df48e0891f70dcc024fd60a0a966081b0c41`.
Review manifest SHA256:
`4009cdd4577711dbda94b6cae90029165178d3ca1b69d273903020e79795e0a0`.

The shared probe extension is opt-in: default factory, eager pose checks
and existing compressed-key providers stay the defaults. Custom binding
executes all full original registration constructors and fails closed
on the exact name/builder/factory/vtable. Legacy eight-byte tracks retain
their original raw records; full SetupObjects receives the actual readonly
parent array. A nullspell owner command interface, resident single-frame
COM surfaces, decoded immutable resource metadata and null volume are
declared API boundaries. Geometry, poses, normals, colors and raster work
are never copied from production into the native branch.

## Bounds and deferred candidates

Current comparison covers the declared native-domain appearance and
bounded map lifecycle. Exact pixel equality, software edge/quantization
residuals, native hardware rendering and natural environmental placement
remain open; full acceptance stays false. Actual before-render RNG counts prove captured frame C uses native tick C:
world draw precedes its late-frame Advance and warmup1 excludes render1.
The earlier v2 panels used the late-quad capture+1 convention and compared
adjacent states; retained v3 pairs correct that protocol from executed logs,
without selecting a visually matching phase. The earlier empty Metal
captures in `metal-water-v1` and `metal-water-cull-control` remain negative
evidence, superseded by the corrected scheduling capture. The first
native geometry assertion incorrectly rejected zero-area birth quads;
`native-water-full-v3` checks all vertices including those degenerate draws.

FireSwarm full factory/base/Initialize/Animate/owner/Render is feasible,
but the original software raster has intrinsic 640-horizontal/480-vertical
primitive-edge limits. Its initial large tube is empty despite complete
unclipped geometry; a late frame draws. That partial phase does not close
the whole effect, and the port is not modified to reproduce the software
limit. Retained tick0/tick36 controls are under `fireswarm/`.

Drip full native factory/base Initialize and sixty Animate ticks followed
by whole head Render succeeds: one actual draw and twenty visible pixels
in `drip-preflight/manifest.json`. Original owner GetParams/SetParams
execute on their real record. Default period48 means delayed birth;
landing must also execute the real child Ripple/map factory and authored
sound before claiming the complete composite appearance. That work is
deferred with no new Drip primary credit.
