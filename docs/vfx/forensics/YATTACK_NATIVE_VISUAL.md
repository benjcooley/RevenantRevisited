# YAttack original keyed starburst

2026-10-10. Exact `YAttack` / `0xaeaeeb34` / `Magic/yattack.i3d` has its own strict original-owner adapter `yattack_original::{Spawn,Advance,SubmitWorld,IsAlive,Destroy}` and `YAttack_AUTHORED` preview. Asset SHA256: `388123c97a3bbd9e13c909461215c38716f48211fa503a489579074656bfb457`.

Executed original registry lookup selects the default builder5e8508. Its asset has one45-frame state with flags8192,39 objects,282 vertices,204 triangles, two materials and two literal64x64 RGB565 textures. Seven '#$' camera-facing flares and32 rectangles form the expanding and contracting green starburst. The lone blendcont/litaddz tag at frame11 initializes mode80 immediately through the original constructor. Actual NextFrame and generic owner Pulse set OF_KILL at tick44/frame44; IsAlive exposes the flag without a timer or respawn. Natural boss/attack orchestration is separate from this null-spell leaf.

The profile pins the final type, complete state metadata, all39 names, geometry, hierarchy and key streams, two materials, texture formats/sizes and literal controller. Source/native checks cover15,795 key channels and28,080 matrix channels across all1,755 object/frame combinations. Maximum errors are2.39e-7 and1.34e-5. ASan/UBSan controls reject3,919 profile mutations, preserve1,755 default versus explicit-false key decodes and show1,716 frames needing the original inclusive key API. The seven camera objects use the separately proven native S*R/camera branch; the rectangles retain R*S*T.

The complete original factory, controller, owner NextFrame/Pulse, Animate, normal-lit FVF112 RenderObject and software raster execute. Actual mode80 is ONE/ONE, CULL_NONE and no depth test/write. The adapter retains texture-major order and uses existing normal-lit helpers; no shared renderer, shader, parser or generic animator changes are needed. Authored material/normal inputs remain intact, with no synthetic native vertex colors.

The original software renderer rejects oversized projected primitives. At capture14, widening only the literal640/480 edge limits to6400/4800 restores long rays and changes active pixels14,652→16,719 while preserving owner/controller state and draw packets exactly. The existing source-derived primitive policy runs only under explicit native-domain plus software mode. Ordinary hardware/common rendering keeps all long rays. A preliminary last-frame guard experiment produced no rejection because those ending rays are small; it is not evidence for active-frame rejection.

The fixed reference uses null spell, owner/camera zero, facing zero, viewport1024x768, center512,640, original ambient38 white and directional1. Metal uses scene ambient32 white,24Hz, warmup1 and seed1. Native-domain depth is0..65535; common diagnostics retain2238..3262. No geometry, timing, RNG, color or camera fitting is applied. Decoded resource/COM surfaces, map notifications, silent audio and base script services remain explicit fixture boundaries.

Private evidence root: `/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-alpha/`.

- `yattack-native-freeze/`, `yattack-native-repeat/`: complete original lifecycle and sampled raster.
- `yattack-pose-first/`: independent source keys/matrices and profile controls.
- `yattack-bounds-active/`: own original widened-guard negative control.
- `yattack-metal-first/{native-domain,common-domain}/`: actual60-frame films.
- `yattack-review-final/`:15 matched phases, full viewport and fixed detail crops,14 matching RNG checkpoints.

Three retained native/source regression tests and the build pass. Two fresh60-tick complete traces and all16 sampled PNGs are identical. Both actual60-frame films contain43 visible frames followed by the blank tail. No native-domain active frame reaches the viewport edge; common full rays reach an edge in13 frames23..35, retaining an explicit off-screen common limitation.

Pair SHA256 `e2f11d264a8ea0f84abc56d2860116eb42ffca3b80424c5168348d731bea27ed`; cases SHA256 `4fffb7dbbdd26f46b71747f79c16543885bf0504c36ae2962036456cc282a144`; captured binary SHA256 `d1d2454eb89b9bbb1159bfd05dd5aa36443247b27074484c7213e054f0c75559`. Root independently reviewed all15 phases and actual-size10/25/40/44, approving scoped native-domain appearance. Frozen root-approval SHA256 `cbeeaf834fb6def5b23a1e375222b0ed192c4b75097d673a5358509312179383`. Natural Yhagoro/boss/spell orchestration, actor/audio/gameplay integration, layered saved-map context, complete graphics-device caller, arbitrary-camera/off-screen behavior and exact pixels remain unclaimed. Common/native projection and backend raster/quantization differences are explicit.
