# Static town signs: runtime coverage and retail differences

Audited 2026-10-05. The twelve town-sign assets have one constant-pose object, 42 vertices, 24 triangles, one texture/material, and no controller tags. Their previous Fountain preview factory rejected this geometry. The preview factories now use the authored static mesh path shared with Globe; historical preview IDs remain available.

The actual map command fixture creates each exact type, checks its ID and single default animator, moves it, checks the resulting position, deletes it, and confirms absence. The corrected binary passes all 72 command/owner checks and all twelve visible-image, movement, and restored-floor checks.

Actual retail software captures for all twelve signs at ambient 32, camera `(10000,10000,16)`, and position `(10000,10000,16)` establish corresponding geometry and lettering. The initial full-path-fixed port pairs had brighter wood in every image; the fresh source-lighting pairs supersede that broad illumination diagnosis. No per-sign brightness fitting was applied.

MistSign initially rendered blank. Its preview loaded four vertices despite the requested town-sign asset having 42. The VFS flattened archive paths to basenames, selecting `Imagery/Magic/mist.i3d` instead of `Imagery/Misc/mist.i3d`. Full archive paths now remain distinct; longest matching path suffixes handle resource/host prefixes, and ambiguous basename aliases are rejected. A focused regression test covers both Mist paths, prefixes, separator/case normalization, ambiguity, exact-path first-wins, and module precedence. The real-map batch confirms that MistSign now renders and moves correctly.

All twelve visual checks remain open under residual shared software raster/shading differences. The prior port used a tunable ambient divisor. Fixture lighting mode 0 does not apply the modern sun. The retained original `blue/swscene.cpp` instead specifies fixed directional vertex illumination for RGB565, five-bit vertex light, direct wrapped texel indexing, and unlit ARGB4444. The shared source-derived correction builds and passes all72 current command checks and all12 visible/move/delete lifecycles. Root reviewed all12 retained native pairs: the broad overbright-board discrepancy is corrected, but texture/raster and shading differences remain visible. Defer these shared renderer details while advancing simple references. RGB source lighting is currently limited to maps without resident positional lights; original per-owner point-light selection remains open. Metal runtime shaders were exercised; GLSL/HLSL equivalents were updated without runtime validation on this Mac. Natural authored map placement is still a separate gate.

Retained evidence under `/Users/benjamincooley/RevenantRetailLab`:

- `research/easy-authored-mesh-inventory-20261005.json`: asset/controller inventory; not a coverage claim.
- `research/vfs-path-collisions-20261005/audit.json`: eight affected EFFECT rows, six with different bytes; other affected types require current-asset rechecks.
- `captures/runtime-fixtures/static-signs-vfs-fixed-20261005/command-capture/manifest.json`: corrected binary hash, all command rows, all twelve render checks and captured poses. The earlier `static-signs-20261005` batch retains the blank MistSign regression.
- `captures/runs/sw-quick-olihootsign-20261005/`, `sw-quick-mistsign-20261005/`, and `sw-static-signs-<typename>-20261005/`: raw native AVI, before/empty/restored frames and reference manifests. Each static sign is visible, outside-ROI change is zero, and deletion restores the exact floor below the FPS band.
- `captures/runtime-fixtures/static-signs-source-raster-20261005/command-capture/manifest.json`: current source-lighting/sampling binary and all72 command checks,12 visible lifecycles.
- `captures/ab/static-signs-source-raster-contact-20261005/manifest.json`: fresh all12 own native pairs and residual-raster reviews.
- `captures/ab/static-signs-vfs-fixed-contact-20261005/manifest.json` and its two contact sheets: all twelve individual pair paths and failed visual reviews. Each pair retains original reference, binary and port-frame hashes.

The batch verifier checks each sign's original, moved and deleted images separately. A global distinct-frame count cannot prove that every profile rendered. Command success, visible lifecycle, retail appearance and natural authored placement remain separate gates.


## 2026-10-07 thin-runtime static mesh A/B

All twelve exact sign identities now pass original-executable versus compiled
production static-mesh frontend tests in
`recon/retail_asm/runtime/effects/static-mesh-frontend-ab/manifest.json`.
For each sign, frames0/50/99 and origin/translated-owner cases replay twice,
render nonempty pixels, stay static at each placement, move when translated,
and have zero differing RGB565 pixels. Exact local indices, UVs, raw asset
records, six fixed keys and matrices are retained. MistSign keeps the exact
Misc/Mist.i3d sign path.

Original GetAniKey409950/decoder409430, CalcObjectMatrix40a420, matrix/point
functions and original software raster execute. Compiled current production
GetAniKey/CalcObjectMatrix, ExtractSubMeshTextureSlot, BuildStaticObjectMatrix
and generic SubmitWorldMesh bodies execute at explicit asset-provider/owner/
renderer boundaries. No guessed controller or animated-frame requirement is
introduced. The batch also includes Globe, for13profiles/78cases.

This proof uses controlled white vertex input, identity/translated owners,
black RAM background and shared original raster. It does not close original
authored-normal/positional illumination, map culling/placement, modern GPU
sampling/blending or natural owner lifecycle gates. Existing real-map command
audits above remain separate evidence. Full instructions and provenance are
in the batch README; thin runtime is primary, with targeted DOSBox-X device
checks available for specific renderer questions.
