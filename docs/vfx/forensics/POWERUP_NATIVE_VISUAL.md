# PowerUp original keyed ring stack

2026-10-10. Exact `PowerUp` / `0xaeaefb27` / `Misc/Powerup.i3d` now has an isolated original-owner adapter `powerup_original::{Spawn,Advance,SubmitWorld,IsAlive,Destroy}` and `PowerUp_AUTHORED` preview. Asset SHA256 `524420e1937ccaf6754de684ef4b0b123c6ddd8117728e9a384ac0ab8a2ea948`.

The15 independent meshes contain609 vertices/592 triangles: six keyed rings, six trails, two cones and a pulsing glow. The literal90-frame state and17-frame `blendcont litadd` tag run through the normal default owner; original InitializeControllers applies the tag initially. The owner actually kills itself at tick89/frame89. A separate authored one-frame state1 exists and its pose is checked, but this controlled leaf does not invent a caller transition into it.

Admission pins both states, object/material topology, every key stream/hierarchy, geometry, the literal blend tag and all four RGB565 texture formats/sizes. Only exact typeaeaefb27 can spawn. The existing helper path submits texture-major geometry with original two-sided mode16, depth test/no writes, normal-lit source lighting and explicit inclusive key decoding. No shared renderer, parser or generic animator behavior changes. Ordinary geometry remains intact.

The full original factory, owner NextFrame/Pulse, default Animate/Render, normal-lit FVF112 and software raster execute. Bounds are decoded asset/COM surfaces, null spell, silent audio and map/script notifications. Camera/owner0, center512,640, viewport1024x768, native ambient38/directional1. Actual Metal uses24Hz/warmup1, scene32white, both existing native/common projector paths, with native depth0..65535 and common2238..3262 declared. There is no geometry, color, seed or camera fit.

`powerup_pose_probe.py` checks12,285 original key channels and7,696 matrix channels across all1,365 object/state/frame combinations, including native scale rejection. Maximum error2.39e-7. It rejects1,650 profile mutations, leaves1,365 default decodes unchanged, and observes494 frames that require the proven inclusive key API. Its float32 comparison avoids treating nine-significant-digit decimal serialization as a double-precision source error at authored scale11252; the initial decimal-only report remains a rejected diagnostic. ASan/UBSan controls pass.

`test_powerup.py` also executes original owner lifecycle and full normal-lit draw dispatch, verifies live frame88, kill frame89 and no late draws/RNG. Two fresh110-tick native runs preserve identical initial/state/draw records and18 sampled PNGs. Both actual110-frame Metal films pass; all14 logged RNG checkpoints agree. No active pixel reaches the full viewport boundary.

Private evidence under `/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-alpha/`:

- `powerup-native-first/`, `powerup-native-freeze/`: complete independent original references.
- `powerup-pose-float32/`: source/native poses and admission negatives.
- `powerup-metal-first/{native-domain,common-domain}/`: actual110-frame films.
- `powerup-review-final/{whole-life.png,detail.png,pair-manifest.json,cases.json,native-replay.json}`:17 matched phases and frozen hashes.

Natural boss/spell orchestration, state1 gameplay use, actor/audio/damage behavior, arbitrary camera and complete device/game caller remain open. The common/native projector and raster/backend differences are retained. The independent review below grants scoped appearance; source tests do not assert exact framebuffer equivalence.

Root independently reviewed all17 matched phases and approved scoped native-domain appearance. Frozen `root-approval.json` SHA256 `63f9f20979d8dd7c43d3d5daae288e4c7ae21d375bac25410b92538e4774d946`; pair SHA256 `839def6926855ca44fe89d5c70793d9972e914f32f15e76cda1d2abb38bed6d1`; cases SHA256 `b2e1bb5e15ca987513062f924737cd48e3bac57f7f8b44d4d385229c4e2be3d8`. Captured binary SHA256 `7330f433958e9d4959f4280e571e50148acf2c4fab17f34a131ad29f9e048674`. Both actual110-frame films have87 visible frames. Approval retains all caller/state1/backend/raster limits above.
