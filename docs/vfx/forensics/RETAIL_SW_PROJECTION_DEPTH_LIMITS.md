# Retail software projection and triangle-depth evidence limits

2026-10-05. **Actual retail software depth selection differs from modern interpolated depth. Controlled projection basis also differs slightly. Neither result establishes teleportation's flare cause or warrants replacing the default renderer.** No source changes, fits or acceptance credits result from this audit.

## Executed retail triangle selector

Unmodified RGB565 Z-enabled/no-write branch56e86b→56e90c..56e93f truncates three vertex depths to signed integers and selects their maximum.56e965 pushes that single constant and56e992 calls raster54e810. Six permutations of10.75/20.5/30.25 all deliver **30**, compared with an interpolated centroid20.5. Actual selector instructions execute; the final raster draw call is intercepted. This proves the selected path's per-triangle constant, not a rendered pixel comparison or all renderer branches.

[Numeric execution](/Users/benjamincooley/RevenantRetailLab/research/shared-projection-depth-20261005/numeric-probe.json) and [retail dispatcher assembly](/Users/benjamincooley/RevenantRetailLab/research/shared-projection-depth-20261005/retail-sw-raster-dispatch.asm) retain provenance. Modern hardware depth interpolation differs from this source behavior, but the controlled depth examples do not include authored glow triangles or native floor bitmap depths.

## Controlled projection basis

Actual411640 projection initialization,56d5f0 software camera,56cdc0 matrix and43ad80 transform execute under controlled inputs.20localX gives20.2030525horizontal pixels versus current port20;20localZ gives24.7435837vertical pixels versus port26.01. Retail's software projection diagonal is1.42857146 and its recovered controlled view translation is1925.

Owner16 FIX conversion16/1.46 is a **source-controlled model input**, not execution of actual owner Animate's nonlinear formula. Camera origin0/zdist1925 is controlled. Current port comparator evaluates the exact shader formula in Python; it does not execute GPU code. Live guest camera/owner state, full actual-asset projected corners and sampled bitmap-floor depth remain unproved. These small basis differences do not establish the floorflare's causal defect.

## Gold720 scope correction

Gold's previous720 matrix/corner oracle compares actualretail local matrix math to the production helper under isolated identical TRS input. That localmatrix evidence remains valid. **Both resulting local matrices then use the SAMEHOST projection formula.** Their projectedcorner agreement therefore does not establish actualretail view/projection/root/bitmapdepth or full rendered placement. The [Gold audit](BLENDCONT_GOLD_RETAIL_AUDIT.md) and authoritative ledger now state this bound explicitly; historical test reports remain retained.

## Next bounded evidence

Measure actual decoded owner/root and authored corners, then triangle depths against sampled real/native floor bitmap depths. Keep localmatrix, projection, triangle selector, raster result and game-context gates separate. Preserve the modern default renderer. Do not fit height/scale/phase/color or force a software approximation into default production behavior before causal proof and a deliberately scoped compatibility decision.

[Immutable audit](/Users/benjamincooley/RevenantRetailLab/research/ledger-reconciliation-20261005/shared-projection-depth-checkpoint/integrity.json) checks the numeric report plus four source artifacts. Teleportation floorflare, Shadow/Warrior projection/depth/raster and Gold arc/collision/duration remain unfinished. Prior user-reviewed isolated visual passes are unchanged. All176 remain mandatory in the [burn-down](../EFFECT_BURNDOWN.md).

## Subsequent shared backend correction

Root later restores Metal offscreen depth/stencilStore and proves actual map occlusion correction in teleportation/Warriorborn, with no per-type/source geometry or raster approximation. [Backend integration](METAL_OFFSCREEN_DEPTH_STORE.md) separates CPU descriptor evidence from actual GPU capture results. The signedMAX triangle-source fact remains valid, but was not the cause claimed for the corrected Metal base. Controlled basis/live-floor scope limits above remain; no hardware-control or full projector/cadence conclusion is inferred.

## Subsequent controlled PartSys corner-domain evidence

[Source Z-domain handoff](PARTSYS_Z_DOMAIN_HANDOFF.md) now adds controlled actual-retail Gold prototype/FIX/matrix/camera/corner evidence and36corner/9center affinecontract checks. It preserves raw logicalcenter and applies existingnormalmesh1.5 only to localMODELoffset. Known5.118%localZcamera residual, live-owner/full bitmapfloor/cadence and compiledbridge/GPU/A-B scope remain open; the old720sameHOST projection limit remains unchanged. One sourcefile builds, new Gold pairs pending, no appearance acceptance from these numeric cases.
