# Antimagic sphere and fire composite, 2026-10-10

Exact `Antimagic:0xd3ae043a`, `Magic\Antimagic.i3d`, SHA256
`877ea32a3d7be5351aebbbcd9a30f80ff00fb8c1cc5bc66a5d43eae3979bca8d`.
The full original 93-builder registry selects default factory `40dc00` and
animator `5a370c`. Original RefreshControllers initializes blend controller
`5a357c` and particle controller `5a3544`. The complete render contains three
normal-lit sphere layers and a separate moving camera-facing `#cfire1` emitter
with `#rect` particles. The authored `*helix01` path stays hidden.

The exact `litadd` tag and particle emission curve are preserved. Emission is
zero through curve frame 19, 50 from 20 through 270, then zero at 271. Cached
emitter timing, original local rotation, alpha curve, scale jitter, gravity
and constant negative bounce range are unchanged. Tag frame 40 is not a host
spawn delay: original full controller pulse determines births. No actor,
replacement geometry, pose/color fit or time shift is introduced.

All 1,800 authored object/frame poses pass independent source/native comparison:
16,200 key fields and 28,368 matrix fields, with 27 original zero-scale matrix
rejections. The default decoder control is unchanged. All 134,966 particle
fields over 300 active ticks pass with exact RNG, alive slots, ages and
lifespans; maximum float error is 0.000175473 against the declared 0.0002 bound.
The shared simulator and parser are unchanged. Both probes use ASan/UBSan.

Actual original NextFrame `470cc0` and effect Pulse `4de800` kill the owner at
300. Setting only the loop bit is the negative control: frame 299 wraps to 0
without kill. The gallery adapter retains actual NextFrame/Pulse/Alive hooks.
The native reference executes whole Animate and Render every active tick;
17 selected frames additionally execute the original software raster. Other
frames stop at `56eb30` after complete draw callers and render samples. Every
Render RNG seed is unchanged. Immutable decoded resources, exact texture COM
residency, state-header queries, ground 0 and base script/audio Pulse are named
boundaries; the owner completion gate and controller bodies execute normally.

Both exact shipped textures are RGB565 (64² fire, 128² sphere). Source particles
use the proven software RGB565 additive policy, followed by all three authored
sphere meshes with original normal lighting and additive no-cull/no-write
submission. The original packet state confirms these settings on every sphere
pass. No renderer, shader or shared particle code changes are included.

Both 304-frame actual Metal films exit cleanly and contain 293 visible frames.
Their 15-phase review includes sphere growth, emission onset, moving fire,
emission cutoff, shrink and blank 299/300/304. All 15 logged particle counts and
owner frames match the original. Build, owner/loop negative control and six
preview mapping tests pass. The exact owner callbacks keep the gallery tied
to real completion rather than a browser timer.

Configuration: 640×480 black; owner/camera 0; source ambient 38/directional 1;
MSVC seed 1; quality 0; flat ground 0; 24 Hz. The explicit native-domain fixture
is distinct from the ordinary projection comparison. Natural spell/caller,
actor/scene attachment, gameplay, script/audio, exact pixels and device parity
remain outside this isolated appearance scope.

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra`.

Frozen manifests (SHA256):

- `antimagic-state-first/manifest.json`: `7a8f55e98d282bb0b45a858023ac04dbf37b05563653db15c18de21c14142249`.
- `antimagic-poses-first/manifest.json`: `6661ee537e682fa728adacf27d4dac8457dfceac0ca9aa4d8d5bef876446e3d9`.
- `antimagic-native-first/manifest.json`: `3a89e9d2c0003be136df530a1cc5c7f2b322b4d111a6b2ff91ffd8c23197a8d5`.
- `antimagic-metal-domain-first/manifest.json`: `276c50016d40c0801d6a907cff268fee74401eb4ccd2b19d2c521d6008b587d2`.
- `antimagic-metal-common-first/manifest.json`: `df0e42c31e760d3f4f0347f14f80d9f27a9eaceaeee1cb26a20120c134a69be3`.
- `antimagic-review-final/manifest.json`: `8bc48291d019a0115325a15f1c94f3390cfa49e0924670bcd50da8937fc043fc`.

Reviewed pre-guard Metal binary: `4fc2dfce6e6ca1dafed90f8d872eae86dfd572a51d17ff5aff37652d9c3f3567`.

Final owner audit adds a post-Pulse Alive guard: original Pulse kills before
PartSys advances, and the adapter now preserves that order. A compiled test
of the actual Advance function verifies one live simulation step and none on
the kill or subsequent tick. Both final 304-frame films use the guarded binary.
All 15 independently approved native-domain sample PNG hashes are unchanged.
Four unsampled final native-domain captures (185/217/244/265) retain the exact
previous image; the common film similarly retains 138, while its first run
retained 220. Both immutable runs and this capture diagnostic remain explicit;
no timestamps or simulation states were fitted or relabeled.

Root independently approved the fifteen-phase native-domain appearance. Final
approval carries the unchanged reviewed samples and preserves all caller,
float, capture, exact-pixel and device limits above.

Final manifests (SHA256):

- `antimagic-metal-domain-final/manifest.json`: `eb81db056c5629845bd07bea3f1f137ae987a3a77154680eda5e24a62a5ebf13`.
- `antimagic-metal-common-final/manifest.json`: `f829823798a0640896019f13bf99dfc521cfaa521e5b1b3a0ddf33418ab5dcfd`.
- `antimagic-review-final2/manifest.json`: `130754dd56a94a097059474641b406a5e589c4ec9fcf7c36a1a11a02b7f83823`.
- `antimagic-review-final2/kill-guard-visual-regression.json`: `05247cdf6ca11c05439b53352043a6cdc4e6540c2ed125faf6eacfd0a47877eb`.
- `antimagic-review-final2/root-approval.json`: `021d8bccc79da8749a4ae8673bbe328c69355ed0147099595eeed95f57376e92`.

Final binary: `d130ae0511544dfa8b9b328ec6b487d9054bbfcebaa03c9782f12f46a8ee4d56`.
