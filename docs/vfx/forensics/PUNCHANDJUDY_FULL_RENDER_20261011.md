# PunchAndJudy: complete original loop and native-domain renderer

2026-10-11, source checkpoint `9d929fe5`. This extends the historical integer-decoder/map-loop proof in `PUNCHANDJUDY_AUTHORED_RUNTIME.md` with the complete genuine loaded owner/default renderer and current GPU evidence. Root independently reviews and grants any appearance credit; this file does not create an approval.

Exact type `0x0c05263a`, asset `Imagery/Misc/punchandjudy.I3D`, SHA256 `c6d4b31eb0926a644cf9db863f4cc032bca9312f6244e02ba5d227dae8729a7c`. Actual loaded class factory returns owner vtable `5a85ac` and default animator `5a370c`. The full constructor/Initialize, owner NextFrame/Pulse, Animate, PreRender, Render, PostRender and original software raster run through600 updates. All eight roots and both RGB565 textures remain; zero controllers and zero RNG calls are observed. Owner frames wrap580→0→1 at ticks580/581/582. A fresh full original rerun reproduces all draw packets and saved pixels exactly.

The isolated native-domain branch uses existing opaque normal-lit helper policy: original scene1, CULL3, depth test/write, ONE/ZERO. Geometry, immutable eight mesh keys and material/texture associations are preserved. The ordinary SubmitMesh/common-camera branch remains intact, and31 sampled PNGs are byte-identical before/after through601 actual24Hz updates (stride20 only limits image persistence). No shared renderer, shader, decoder or parser change.

A real caller mismatch was found at opening frames2/3 and again at583/584. Original PreRender supplies imagery previous state0/frame0, and GetAniKey409950 performs its literal first-five-frame interpolation even after a same-state loop. The port animator suppresses that transition in its common animation policy. The new native-domain adapter passes the actual owner GetPrevState/GetPrevFrame through existing SetPrevState before CalcObjectMatrix; no frame, clock, key or pose fitting. The retained GPU negative control omitting this call differs in26 matrices, maximum0.193748446. After correction, actual compiled GPU output matches600×8×16=76,800 original matrix channels with maximum9.5547790524e-7. Original previous-state/frame values are observed from genuine initialization, not seeded.

Final actual Metal film contains601 frames at24Hz with clean exit. Eighteen pinned whole phases1,20,50,100,150,200,250,300,350,400,450,500,550,579,580,581,582,600 show the complete articulated loop and wrap. Both sides use fixed1280x960, center640,640, source ambient38/dir1, facing0 and ownerZ16. Native source/camera XY3185/3185 corresponds to the explicit relative port origin0/0. Every original submitted vertex across600 updates is contained within `[637.941624,573.510788,677.111809,627.034580]`. Actual-size pairs100/300/500/580/581 are included.

Translated common-world gallery:32/32 no-UI frames visibly render, one persistent owner,746 real24Hz updates with every counter modulo581 exact, clean owned teardown746. No invented finite lifetime, resets or factory RNG seeding. This gallery validates the current runtime path separately from the isolated native-domain comparison.

Validation: private Revenant build; complete original repeat; actual GPU full-loop pose/phase proof and missing-caller negative control;31 ordinary-policy unchanged PNGs; translated persistent gallery; Python compilation and diff checks. Natural story/caller, fractional rendering outside the declared24Hz cadence, arbitrary state transitions, terrain/light context, audio/device behavior, edge/surface quantization and exact framebuffer equality remain open.

Evidence root: `/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra`. Final binary SHA256 `d736e97f2901d773c7502c806d49d0b42984746a04593c1bd39e841cb22234e3`. Source-only tools and this compact record are committed; native resources, images, binaries and full reports remain private.

- `punch-native-final/manifest.json`: `109717e485d2282e108e0f3e989e8d2b758cbd4e1e014d9be7a52ad689db9905`.
- `punch-metal-final/manifest.json`: `ae65645fb72fd975b073d8e63a03b37b02e3088d427ad18ee44f9966f53fbfaf`.
- `punch-common-final/manifest.json`: `9e1d2245354c46310dd5c2dafec44924ec60087a4a672541ea542c9cc3673816`.
- `punch-gallery-final/manifest.json`: `7264af8333d1fef6f40fc0302468fc333c03ca1d695cbfe7fa0a2cc315463712`.
- `punch-review-final/manifest.json`: `af91577310ccc61e8798231f2f660789778407c77af773d98ac8f3c6156e7e15`.
- `punch-transition-control.json`: `5d5705808a7b5add331f6b547de41b5177531ac6339714072d8068acaa036665`.
- `punch-common-control.json`: `514306d386e0545b57701e0192d2807052111b1abbfd9854a5445c551d029ae8`.

Reproduction tools: `punch_full_visual_reference.py --boot-image ... --output ... --frames 600`; `port_capture.py` with frozen `punch-native-scenario.json`/`punch-common-scenario.json`; `punch_full_visual_review.py`; `punch_transition_control.py`; `punch_gallery_smoke.py`. Root repeats may replace only the binary path in the pinned scenarios.
