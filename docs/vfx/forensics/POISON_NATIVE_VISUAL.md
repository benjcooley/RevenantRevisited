# Poison original looping cloud and bubbles

2026-10-10. Exact `poison` / `0xe0a3bc42` / `Magic/PoisonCloud.i3d` now uses its original default owner and two authored particle controllers. Asset SHA256 `ce243725520cbbe3c4e02ee873b06487dc92de3411a65401eb3ff7ff7a72c43a`. This is a stationary looping emitter, not a traveling missile or a finite invented puff.

The literal state is60 frames with loop flags8193. Four independent `ground01..04` roots feed `#pcloud` (capacity16) and `#bubble` (capacity31); tags at authored frames6 and15 are initialized by original InitializeControllers, not delayed trigger timers. The source preserves cached Animate poses, original shared RNG, rotation sampling during Render, source quad order and independent texture slots. Cloud and bubble retain their authored greens, scale jitter, gravity and relative velocity. The owner stays alive across both observed loops; `IsAlive` directly reads `OF_KILL`.

Strict admission pins one state, six objects, eight vertices/four faces, three materials, both literal tags, all key streams/hierarchy, and the two8x8/32x32 RGB565 textures. The shared native-proven integer modulation is used only under software policy; ordinary modern rendering preserves its existing normalized path. No renderer/shader change, palette fit, seed fit or per-effect projector adjustment is involved.

The complete original factory, NextFrame/Pulse, default Animate/Render and actual FVF1e2/software raster execute using the thin x86 fixture. Boundaries are decoded asset/COM surfaces, flat walk height1, null spell, silent audio and map/script notifications. Camera/owner0, screen center512,640, viewport1024x768, native ambient38/directional1, seed1/quality0. Actual Metal uses isolated `Poison_AUTHORED`,24Hz/warmup1, explicit scene32white, both existing native and common projector paths. Native depth0..65535 and common2238..3262 remain declared.

130-tick production/native comparisons pass125,768 fields each at render strides1 and3, with exact integer/RNG state and maximum float error1.91e-5. The real owner wraps to frame0 on ticks59 and119. Pose tests check3,240 key and5,760 matrix channels (maximum5e-8),65 negative profile mutations, and360 unchanged default decodes. Constant keys have no inclusive/default differences. ASan/UBSan checks are retained in `test_poison.py`; shared cached-pose tests cover repeated Pulses before Animate.

Evidence lives outside git under `/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-alpha/`:

- `poison-native-final/` and `poison-native-freeze/`: independent full original runs.
- `poison-native-stride3/`, `poison-state-final/`, `poison-state-stride3/`: controller/cadence evidence.
- `poison-pose-final/`: key/matrix/profile proofs.
- `poison-metal-first/{native-domain,common-domain}/`: actual130-frame films.
- `poison-review-final/`:19 matched phases and hash-pinned pair/case records.

Natural poison spell targeting/damage, saved scene placement, complete game/device caller, arbitrary cameras and exact framebuffer parity remain open. Appearance requires independent review; a compiled source test alone does not grant it. Ordinary loop cleanup belongs to the caller; no duration is fabricated here.
