# Dragonfire: original two-pool renderer, 2026-10-11

Source checkpoint `e22059e6`. Exact shipped `dragonfire` type `0xad92bc1a` binds `Magic/FireCone.I3D` (SHA256 `8683cc1a0921f4c0bb7ef8544e94e1e391b180caca0db14260e8538fba948802`), but has its own owner and animator. No FireCone controller alias or unrelated Misc/dragonfire asset is admitted. Appearance awaits root independent review/repeat; this document does not grant credit.

The actual loaded builder returns owner vtable `5aad24`, animator `5aaf28`; Initialize `4eac30`, Animate `4eacf0`, Render `4eb130`, owner Pulse `4eabf0`. Null spell is an original supported branch. Both 300-slot pools, original CRT RNG, full owner/controller calls and full software renderer execute unchanged in the source-matched cached HudWorld. The source uses raw random(-4,4) initial XYZ, as the executable does; snapshot +18/-50/+15 offsets are rejected by a negative control.

Emission lasts 34 updates, 18 attempts per update, with 6,120 original RNG calls. Expired fire is copied into the separate 15-update smoke pool before fire integration. The seed1 run kills at update93. The compiled production core matches 453,120 meaningful fields through100 updates with zero float error and exact RNG. The independent production matrix consumer matches all19,626 original draws /314,016 channels exactly. A double-animate negative control also fails. Cold/dead particle storage and uninitialized per-particle flicker are excluded: the original Render passes false,false, so that flicker field is unused.

Both authored four-vertex meshes and RGB565 textures are preserved. Two stable GPU mesh keys are reference-counted; no per-particle immutable mesh allocation. The standalone module uses existing additive/helper policies and explicit native-domain projection, with no renderer/shader/parser changes. Real owner Pulse and post-Pulse alive guards remain; preview/gallery uses the actual finite core completion callback and never reseeds per factory.

All100 original updates retain actual Render/raster calls. Fourteen pinned whole phases1,2,5,10,20,30,34,40,50,60,70,80,90,100 cover birth, movement, smoke transition, drain and blank tail. Original and Metal share fixed1280x960, center640,640, source ambient38/dir1, facing0, source owner3185,3185,16 and camera3185,3185,0; the isolated port uses the equivalent declared relative owner0,0,16. Executed projection of every submitted triangle across the full lifecycle stays within `[617.943569,417.290161,822.821411,641.305962]`. Actual GPU logs match94 pool/RNG states through kill. Final binary preserves all101 first-capture PNG hashes. Actual-size comparison pairs20/34/50/80 accompany the contact/detail sheets.

The common-camera film is diagnostic and has no native-domain equivalence claim. Actual translated common-world gallery passes20/20 no-UI visible frames at24Hz, four natural completions93/95/96/96, and final owned teardown at78. This is458 total controller updates across five factories with continued RNG, not fixed-time destruction or invented repeat lifetime.

Validation: private Revenant and review-bridge build; bridge mapping/lifecycle/callback test; generator --check; Python compilation; independent native state and ordered-matrix probes; actual Metal native/common films and translated gallery; diff --check. Natural caster/spell-facing behavior, arbitrary actor/target context, sound/device behavior, source light selection outside the declared configuration, sampling/destination precision and exact framebuffer equality remain open.

Evidence root: `/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra`. Final binary SHA256 `aa2187e3f0c431cfc2720ffe0aeacc02a5009bf0ac705dc25dcfbbe8a015eb0b`. No generated images, native assets, executables or large reports are committed.

- `dragonfire-native-first/manifest.json`: `5463224fddd6c6ed63797dcb92fe451381e09f24099dae1ba9ddbe7b6f78be58`.
- `dragonfire-state-controls/manifest.json`: `7e6479899ca55a8374b283965a0c4dd38c5534fa5124b258b30ef32948807b87`.
- `dragonfire-pose-first/manifest.json`: `9b8bc6a9e48790041d81c222dc27a49380e67e5eb0e91ae4ced005cea510ee34`.
- `dragonfire-metal-final/manifest.json`: `0ad0ba6e7500ea92ccdea9663e8441e0d49bdee0c8539f9b9482be854c766033`.
- `dragonfire-common-final/manifest.json`: `c8023792d124272a0b072554a3205c8105d9df8bc4bc75e4a4b8c17febb9e1aa`.
- `dragonfire-gallery-final/manifest.json`: `2944c61b79f3086379ee030119231028231fc8c1173753d93043f481a6c63cf3`.
- `dragonfire-review-final2/manifest.json`: `b2edb2793521605143155e9c2628f7fba74c929f5751503b379db87e4632af4f`.

Reproduce with `dragonfire_visual_reference.py --boot-image ... --output ... --frames 100`, `dragonfire_state_probe.py`, `dragonfire_pose_probe.py`, `port_capture.py --scenario dragonfire-scenario.json`, `dragonfire_visual_review.py`, and `dragonfire_gallery_smoke.py`. Each tool has explicit argparse inputs; scenarios are frozen privately. Root repeat substitutes only its current binary in the same scenario.
