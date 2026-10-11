# YPowerUp original keyed energy sphere

2026-10-10. Exact `YPowerUp` / `0xaeaeeb27` / `Magic/ypowerup.i3d` has an independent original-owner adapter `ypowerup_original::{Spawn,Advance,SubmitWorld,IsAlive,Destroy}` and `YPowerUp_AUTHORED` preview. Its own asset SHA256 is `64cf9f74c056ebdbdb1baf04351c1598ab50e8338a03b7e9c316062d40ada204`.

This is a separate 73-frame, single-state asset: 55 meshes, 730 vertices, 740 triangles, five materials and five literal RGB565 textures. It contains sphere segments, bolt rings, two camera-facing rings, seven camera-facing flares and 32 rectangles. The `blendcont litaddz` tag at frame 24 initializes its mode immediately through the original controller constructor. Actual NextFrame and owner Pulse set OF_KILL at tick 72 / frame 72; the adapter exposes that real flag through IsAlive and invents no timer or respawn.

Admission pins the exact final type, state length/flags, all object names, hierarchy and key streams, vertex/face bytes, material fields, texture formats/sizes and literal blend tag. Inclusive key decoding uses the existing explicit API. Ordinary objects retain R*S*T; the exact '#' names execute the separately proven S*R/camera-orientation branch. The 55 meshes remain in the original texture-major order. No shared animator, parser, renderer or shader changes are needed.

The complete original default factory, owner NextFrame/Pulse, Animate, RenderObject, normal-lit FVF112 and raster execute against this asset. Original mode 80 requests ONE/ONE, CULL_NONE and no depth test/write. `ypowerup_winding_probe.py` reverses every one of the 740 original triangle index windings at two visible phases while preserving all other inputs. Identical nonempty original pixels prove the two-sided requirement. The initial copied positive-face filter was removed from this adapter; its rejected first Metal film remains a diagnostic, not accepted evidence. The existing helper's default two-pass policy draws both sides.

The source/native pose probe checks 36,135 key channels and 64,240 matrix channels across all 4,015 object/frame combinations. Maximum errors are 2.39e-7 and 3.82e-6 respectively. ASan/UBSan controls reject 3,944 profile mutations, preserve 4,015 default versus explicit-false key decodes, and show 2,005 frames requiring the original inclusive key behavior. Four regression tests also execute the real owner lifetime, verify all normal-lit FVF112 draw calls and original scene states, repeat the winding control and exercise the widened-guard negative control. The reused primitive-boundary oracle passes all 20 original threshold/winding cases.

The controlled reference uses null spell, owner/camera zero, facing zero, viewport 1024x768 and center 512,640; original ambient is 38 white with directional 1. Metal uses 24 Hz, warmup 1, seed 1 and scene ambient 32 white. Native-domain depth is 0..65535; the common-domain diagnostic retains 2238..3262. No geometry, color, timing, RNG or camera fitting is applied. Immutable resource/COM surfaces, map notifications, silent audio and the base script callback remain declared fixture boundaries.

Private evidence lives under `/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-alpha/`:

- `ypowerup-native-first/` and `ypowerup-native-freeze/`: complete original lifecycle and sampled raster output.
- `ypowerup-pose-bounds-final/`: source/native keys, matrices and admission controls.
- `ypowerup-winding-final/`: two actual original winding controls.
- `ypowerup-metal-first/`: rejected one-sided policy diagnostic.
- `ypowerup-metal-final/`: full-ray software-guard diagnostic.
- `ypowerup-bounds-first/`: native widened-guard negative control.
- `ypowerup-metal-bounds-final/{native-domain,common-domain}/`: corrected actual films.

The last live frame exposes the original raster's literal 640/480 projected edge limits. Widening only those constants to 6400/4800 restores the long rays, with identical owner state and draw packets: 12,026 original versus 26,940 widened active pixels. The exact existing primitive guard therefore runs only under explicit native-domain plus software policy. It preserves contiguous accepted index runs in source order. Common/hardware rendering keeps all long rays; the widened native image is diagnostic only.

Both actual 90-frame Metal films pass and contain 71 visible frames followed by the empty tail. All logged RNG checkpoints agree at zero. Two fresh original runs preserve identical initial state, all 90 state/draw records and all 21 sampled PNGs. The native-domain film and original sampled output do not reach the viewport boundary. The common-domain last live frame retains the full rays and reaches the viewport edge; that diagnostic does not certify off-screen geometry.

The frozen 20-phase review is `ypowerup-review-final/{whole-life.png,detail.png,pair-manifest.json,cases.json,native-replay.json}`. Pair SHA256 `ed040f66252b727d79600f5022964510557015befcff155266950565bf3f44ac`; cases SHA256 `dedf0ef1d4b65f97152357a5ed6aeeb96ffe11a31d74c5ed12271fd2d094fdac`; captured binary SHA256 `649590d4303d0fa5f99b40946f3cba337905857d1be7b741397c182aa1340dab`.

Appearance review is pending. This leaf does not certify natural Yhagoro/boss/spell orchestration, actor/audio/gameplay integration, arbitrary-camera/off-screen rendering, exact pixels or complete game/device caller behavior. Common/native projector and backend quantization differences remain explicit.
