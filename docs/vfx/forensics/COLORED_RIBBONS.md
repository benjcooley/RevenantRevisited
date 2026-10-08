# Colored Ribbon meshes and shared illumination

Audited 2026-10-05. `RibbonB/G/O/P/R/W/Y` are distinct retail types. Each audited asset contains two objects, two textures, 186 vertices, 204 faces, one constant `STILL` frame and no controller tags. None has a name-registered retail animator in the retained registration audit. Base `Ribbon` has a separate custom animator and must not supply their behavior.

The native software `RibbonB` fixture is a fixed blue spiral and floor glow. Its clip has one distinct ROI image, a visible effect, zero outside-ROI change and exact restored ground after deletion. The corrected preview preserves all authored subobjects and texture slots, with their static parent transforms; it adds no lift, fade or retrigger. One six-frame RibbonB preview is visible and static. The other six preview factories compile but have no separate preview smoke evidence yet.

All seven exact colored types pass 42 real-map command/owner checks and individual visible original/moved/deleted image checks through their default animators. This is runtime regression evidence, not retail visual acceptance or natural shipped-map acceptance.

The initial actual-map RibbonB pair had corresponding blue spiral/glow geometry with overly bright port highlights. Fresh source-lighting/sampling comparisons now cover each of all seven colors using its own native recording. Root confirms corresponding fixed spiral and glow geometry/color, but port glow/highlight distribution remains smoother and differs from native software. All seven overall visual checks remain open under the shared raster/blending issue; defer further renderer investigation while advancing simple references. Each new native clip has one expected static ROI image, zero outside-ROI drift and exact restored empty floor. The source/settings audit gives a concrete shared illumination discrepancy: retail `T3DScene::SetAmbientLight` clamps `ambient*8*Ambient3D/100` at 255, then scales white color by `/256`. The retained native INI uses `Ambient3D=130`, `Software3D=Yes`. Before the shared correction, fixture ambient 32 used ambient divisor 20 (multiplier 1.6) and light ceiling 1.5. The earlier synthetic-sun explanation was incorrect: fixture mode 0 does not apply the modern directional light. Original Blue software RGB meshes use a fixed directional light and five-bit vertex illumination; ARGB4444 meshes bypass lighting. The shared correction also uses original nearest wrapped texel indexing. Material/raster differences may also contribute; the settings discrepancy is not proof of the sole cause. No per-effect brightness fitting was applied.

Evidence under `/Users/benjamincooley/RevenantRetailLab`:

- `research/easy-authored-mesh-inventory-20261005.json`: each exact colored asset's static pose, textures, objects and tags.
- `captures/runs/sw-quick-ribbonb-20261005/manifest.json`: raw native AVI, visibility, background/cleanup checks and borrowed empty-floor provenance.
- `captures/runtime-fixtures/static-ribbons-20261005/command-capture/manifest.json`: earlier static-mesh binary,42 real command rows and seven visible lifecycles.
- `captures/runtime-fixtures/static-ribbons-globe-source-raster-20261005/command-capture/manifest.json`: current source-lighting/sampling binary,48 command rows and8 visible lifecycles including Globe.
- `captures/runs/sw-static-ribbon<g/o/p/r/w/y>-20261005/manifest.json`: six own additional native references with original and restored floor images.
- `captures/ab/colored-ribbons-source-raster-contact-20261005/manifest.json`: six new exact-type pairs and own root visual review.
- `captures/ab/sw-static-ribbonb-source-raster-20261005/manifest.json`: rechecked Blue pair against the same current binary.
- `captures/ab/ribbonb-preview-static-20261005/manifest.json`: corrected preview's command, binary hash and six static image hashes.
- `captures/ab/sw-static-ribbonb-actual-map-20261005/manifest.json`: exact-name actual-map retail pair and failed brightness review.
- `research/vfs-path-collisions-20261005/retail-revenant.ini`: retained native settings.

Next: correct or isolate source-derived retail mesh illumination, recheck RibbonB and the twelve retained sign pairs, and capture each remaining color's own retail reference. Base Ribbon and natural authored contexts remain separate work.


## 2026-10-07 native key-pose correction and seven-color thin A/B

All seven exact assets now pass14 original/compiled-production composite cases
(origin and translated owner, two warm replays) in
`recon/retail_asm/runtime/effects/colored-ribbon-frontend-ab/manifest.json`.
Both root objects/texture slots,186vertices/204triangles, exact raw textures,
local indices, UVs and one constant STILL frame are retained. Seven distinct
visible colors and individual movement/replay checks pass. The base Ribbon
revive/combat controller is deliberately excluded.

The batch found a shared key-pose bug. Box01's scale2.734375 incorrectly
scaled its position(0.13671875,-0.13671875,0) in the port. Actual retail
CalcObjectMatrix40a55f..40a576 scales the rotation basis, then stores key
position unchanged; production MakeMatrix translated before scaling. Reordering
only that recovered key-pose helper drops maximum corner error from0.237122
world units to1.94e-6 and maximum differing RGB565 pixels from380 to0.
Original image/depth hashes and all assets/settings are unchanged.

Original GetAniKey409950/decoder409430, CalcObjectMatrix40a420, matrix/point
functions and software raster execute. Current production key/matrix/mesh
extraction/static submission bodies compile verbatim with explicit decoded-
asset, owner and renderer boundaries. The two parent=-1 parts are independently
remapped to object0 for bounded helper execution and composited in original
asset order. Current previews and generic map meshes use these shared helpers;
complete map-renderer/VFS/owner lifecycle execution is not claimed.

Low-level scaling and flag-selected order are preserved. An additional actual
native numeric test confirms a rotated/nonuniform-scale key pose keeps key
translation while explicit POS1|SCL2 still scales translation to(2,6,12).
The prior13static sign/Globe profiles rerun with all78cases passing; rebuilt
transform and animation tests pass. Other affected earlier pose evidence must
be refreshed, not assumed valid after a shared helper change. Before/after
source-span hashes, failed baseline and native unit matrices are retained.

These checks supply white vertex color and a depth-free diagnostic background.
Original map root placement/Z stretch, floor occlusion, authored-normal/point
illumination, map culling/depth and modern GPU output remain separate gates.
No offset, shape or brightness was fitted to compensate for them. Thin runtime
is primary, with targeted DOSBox-X device checks available when needed.
