# YLose original sphere and fade controller

2026-10-10. Exact `YLose` / `0xaeaeeb31` / `Magic/ylose.i3d` now has an independent original-owner adapter `ylose_original::{Spawn,Advance,SubmitWorld,IsAlive,Destroy}` and `YLose_AUTHORED` preview. Asset SHA256: `ffcba84f57a77bbcf56d0863253584195b7a95393a9aa682a156bf90ef67af87`. The strict profile pins its single 14-frame state (flags8192), 17 objects, 476 vertices, 562 triangles, four materials/four RGB565 textures, all key/geometry bytes and both literal controller definitions.

The complete original factory, owner NextFrame/Pulse, Animate, controller Render, RenderObject and software raster execute. Registering `fadeobj` through original401d40 is required; the first preflight omitted this registration and is rejected evidence. Original4021a0 parses the malformed authored target list and selects objects2..16 except12. The missing quotes genuinely omit boltring08/object12. These14 objects clone their vertices to FVF1e2; objects0/1/12 retain actual normal-lit FVF112. Original408a8e initializes the clone color to literal `0xfffffff8`; no white or host replacement colors are fed to the native renderer.

Original4022b0 updates color during Render, not Pulse. The curve interpolates100→180→0 at frames0/5/13; at/after the final key, it retains the color from the last actual Render. The production cache therefore updates only during SubmitWorld. The native differential checks4,200 color fields over fresh Render strides1/3/5/13 and no renders, including initial clone values, actual target membership and skipped-render persistence. All values match exactly with zero RNG calls. Source faded meshes use existing textured additive triangle submission and the native RGB5 color carrier; the three unfaded objects use actual helper lighting. No shared renderer, parser or generic animator behavior changes.

The two exact camera-facing rings use the independently proven camera matrix; other objects use R*S*T. Source/native checks pass2,142 key channels and3,808 matrix channels (maximum errors2.39e-7 and3.28e-7). ASan/UBSan controls reject591 profile mutations, preserve238 default decodes and show220 frames needing the explicit inclusive key API. Four retained tests pass, including real owner termination, both actual FVF formats and CULL_NONE/no-depth mode80. Owner OF_KILL occurs at tick13/frame13; IsAlive exposes that flag without an invented timer.

The final expansion exercises the original640/480 projected primitive-span guard. Widening only those two native constants to6400/4800 changes active pixels87,610→200,889 with identical owner state and draw packets. The existing source-derived guard applies only under explicit native-domain plus software policy. Common/hardware rendering keeps full geometry. The final live expansion reaches the viewport bottom; off-screen geometry and arbitrary-camera appearance are not certified.

The fixture uses null spell, owner/camera zero, facing zero, viewport1024x768 and center512,640, original ambient38 white/directional1 and Metal scene ambient32 white. Actual films run24Hz, warmup1, seed1. Native-domain depth is0..65535; common diagnostic depth remains2238..3262. No geometry, color, timing or camera fit is applied. Original map notifications, immutable resource/COM surfaces, silent audio and base script callback remain explicit fixture boundaries.

Private evidence root: `/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-alpha/`.

- `ylose-native-freeze/` and `ylose-native-repeat/`: identical complete30-tick state/controller/draw traces and all16 sampled PNGs.
- `ylose-pose-first/`, `ylose-fade-first/`, `ylose-bounds-first/`: numerical, cadence and native guard controls.
- `ylose-metal-first/{native-domain,common-domain}/`: actual30-frame films, each12 visible frames followed by the blank tail.
- `ylose-review-final/`:15 matched phases, full viewport and identical fixed detail crops, all10 logged RNG controls matching; pair/cases/replay pin all inputs.

Pair SHA256 `00a20cef28d88f2cb69fa8eba4b80f7204256eef891f4f72e88196b39c636af4`; cases SHA256 `5c9afc71c706fed38aacb0b9cc2889307ceedc259616fe99547435b7654dbaec`; captured binary SHA256 `189363952a270e88b82eeb39597e7acd016946b3c50d0aa46f5f3991ec550ca8`.

The fixed null-spell leaf remains separate from natural Yhagoro/boss/spell triggering, actor/audio/gameplay integration, layered saved-map context, complete device caller and exact framebuffer acceptance. Common/native projection and backend raster/quantization differences remain explicit. Independent appearance review is pending.
