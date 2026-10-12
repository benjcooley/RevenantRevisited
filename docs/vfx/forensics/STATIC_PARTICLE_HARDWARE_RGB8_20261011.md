# Three exact static PartSys hardware color corrections

Regeneration `0x10ac03de`, SwiftStrike `0xe0a3bc43`, and nullifier `0xad92bd38`
retain full packed RGB8 in actual original hardware FVF1e2 draw packets. Their
source submission incorrectly applied the software RGB5 normalization in modern
policy too. Commit `c259ad23` adds only these three exact, strictly admitted IDs
to the existing fspray/Dust renderer-policy distinction. Software, every other
profile, geometry, timing, texture decoding and shaders remain unchanged.
The policy depends on `UsesRetailSoftwareMeshLighting()`, not projection mode.

Initial genuine cached factory/default Render audits observed906 affected quads
for each of Regeneration and SwiftStrike and1145 for nullifier across36 updates.
The compiled production clause was checked against every observed color. The
expanded unit controls cover30720 channels over every byte value, exact admitted
IDs, an unadmitted matching-ID negative control and all unchanged profile/renderer
policies; six tests pass, including the existing Dust23040-channel controls.
Private complete build passes.

Full qualification uses original NewObject/CreateAnimator, NextFrame, owner
Pulse, Animate, scene/PreRender/Render/PostRender/EndScene calls and original COM
RGB565 texture data. Every actual device packet must be FVF1e2, MODULATE, ONE/ONE,
CULL_NONE, depth test without writes. No authored pose/color/particle substitute
feeds the original. The independent device is explicitly **qualified**: linear
RGB8 target, nearest rounding per fragment, point and linear controls, modern
half-integer versus retained legacy integer sample centers. Original transforms
project the exact packets with zero indices. This is not period-GPU fidelity.

Each profile has77 original updates, a fresh77-update repeat with identical
packets/state/pools/RNG, and a77-update original software control with identical
state/pools/RNG. Actual Metal before/after films contain75 frames each. All225
software-policy **RGBA** pixel buffers are byte-identical before/after. The
recorded timing CSV selects13 fixed phases5/10/15/20/25/30/39/45/59/60/65/71/76,
covering the60-frame Regeneration/SwiftStrike loop and repeated25-frame nullifier
loops. No phase, brightness, sampling or geometry fit is used.

The complete private evidence root is
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-alpha/static-rgb8-correction`.
Each named folder has `native`, `native-repeat`, `software-native`,
`modern-before`, `modern-after`, `software-before`, `software-after` and `review`.
Every review pins494 files and includes13 full-size triples and five detail
sheets: qualified original RGB8 / old source RGB5 / corrected source RGB8.

- Regeneration: aggregate absolute RGB channel error27348→8012; pair SHA256
  `15e635daeaf757f2f4e0bcf0d0207fb8c66cdc445dc3c232f090431b2b115439`.
- SwiftStrike:45335→9546; pair SHA256
  `71f075e2bdf86cfad72789d34fde2481ba0652ff803d7f8334aae2b9b1c07963`.
- nullifier:160829→71684; pair SHA256
  `87465bdbfb24c648805e20576c8dc45597ff4eac6e240bd5e52421d2177462e0`.

Every selected phase improves. Nullifier retains a visible bright-core
coverage/shape residual near phases5/30: the central notch at30 already exists
in the before film. The color correction does not explain that residual or
certify whole hardware geometry/coverage. Texture realization/interpolation and
quantization also leave nonzero residuals. Existing original-software primary
acceptance is unchanged; this adds no effect credit. Independent review of the
qualified hardware correction remains separate.

`batch-review.json` SHA256 is
`bc8eff4510f78e85020a154d7a8b453986a60b50bd4c13d984f93a73a98110cb`.
`source-checkpoint.json` pins the two binaries and capture source;
`software-rgba-regression.json` retains all225 unchanged buffer checks.

Read-only follow-on audit also found genuine RGB8 differences in YEnergy
(148/1100 quads) and YEnergyLose (117/480). YAbsorb is a useful negative control:
all383 observed quads are white255 and unchanged by the RGB5 normalization.
These and the other source-only suspects remain untouched. Water's genuine
loaded CreateAnimator failed in the limited cached fixture; no device defect is
claimed from that failure.

Reproduction tools: `static_rgb8_device_reference.py --profile NAME --boot-image
BOOT --output DIR` (add `--software` for the software control),
`static_rgb8_device_review.py --help`, plus `test_fspray_device.py` and
`test_dust_device.py`. The default fspray fixture/review APIs remain compatible.
