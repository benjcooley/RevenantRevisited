# YFireBall full leaf render

YFireBall now selects its original green shock-ring palette. Its existing
`TFireBallEffect_Bespoke__YFireBall_BESPOKE` preview also submits the ring;
previously that entry only called the card/spark submission. The shared
FireBall controller and quad renderer remain unchanged. A separate
`TYFireBallEffect_AUTHORED_FLIGHT` entry runs one fixed-endpoint lifecycle for
comparison, using the same production factory, controller and draw methods.

The native reference executes YFireBall's own factory `515150`, initializer
`513230`, complete Animate `5133c0` and Render `5146d0`, followed by original
RenderObject and software raster. Its version-3 compressed I3D resource and
all three original states are supplied to the original matrix/key decoder.
The green ring stops observed at `513942` are `000f9e0c`, `ff3dc60f`,
`bf9bf70f`, `00d3fc84`; alpha follows the original shared shock animator.
The source fix selects these literal stops only in the derived Y class.

Private evidence root: `research/vfx-wide-20261010/hard-alpha/`.

- `yfireball-impact-proof/state-report.json`: 45,180 strict binary32/RNG checks
  across 90 complete native animation ticks, all pass.
- `yfireball-impact-proof/geometry-report.json`: 492 checks over 43 actual
  impact packets, all pass. Maximum world-position error 0.0000159307;
  packed vertex colors exact. SHA256
  `51c6c43da7200319090dc4a2bd186b98e64f30218a3ecfdd8b3ae68f6ba2a7a6`.
- `yfireball-wrong-palette/geometry-report.json`: explicit old FireBall palette
  control fails every sampled ring packet (ticks 60, 65, 70).
- `yfireball-impact-proof/initializer-controls.json`: four original initializer
  controls pass. An ordinary invoker adds 50 Z; `yhagoro` adds 110 Z;
  `yhagoro` with character flag `0x4000` adds 50 Z; the name comparison is
  case-insensitive. These are executed branch controls with explicit input
  resources, not a claim about the natural NPC caller.
- `yfireball-metal-freeze/{common-domain,native-domain}/manifest.json`: both
  actual 90-frame Metal films PASS, 79 nonblack frames then empty tail.
  Binary SHA256
  `204aa53dab00a057c846e571c196e5fae89b72c25eac3ff736e95123233117fb`.
- `yfireball-review-final/{detail.png,whole-life.png,pair-manifest.json}`:
  eleven matched phases, original software/common world/native projector.
  Pair manifest SHA256
  `7e4dab16fbd43e19e539a7a3b23bcbef268dcdd466507dff14588f38f35688c7`.
- `yfireball-existing-route-centered/manifest.json`: actual existing preview entry,
  six distinct nonblack impact-phase Metal frames after 65 warmup ticks;
  the green ring is submitted through the new world callback. Camera
  (128,384) keeps the unchanged random aim41 trajectory in view. The first
  centered-camera recording remains as a clipped framing diagnostic.

The ordinary-invoker comparison uses the same explicit source, camera, light,
24 Hz cadence and one outer preview RNG draw as the
[FireBall full-render proof](FIREBALL_FULL_RENDER_20261010.md). All seven logged
Metal/native RNG checkpoints agree. No fitted geometry, tint, seed, time or
image alignment is introduced. The native asset SHA256 is
`1f5f85d4477435b2c0bc4e6486e4a070795fa9366b18fb7fc7eb49392dedea98`;
retail executable SHA remains pinned by the shared fixture.

Build passes. The existing FireBall default palette retains all 444 packet
checks, and the expanded native fixture reproduces its first two previously
frozen original images byte-for-byte. Shared renderer/shader files are untouched.

The natural Yhagoro source/target and conditional extra launch lift are still
open production integration gates. So are live spell/damage/hit-handler
execution, map reaping and device/framebuffer equality. The scoped appearance
proof is the explicitly selected ordinary-invoker branch and range impact;
it does not assert complete NPC combat acceptance. Independent root approval
is frozen beside the panels separately; no central ledger is edited here.

Use `fireball_visual_reference.py EXE --effect YFireBall --visual-replay
--frames 91 --output DIR` for the full reference and
`fireball_impact_geometry_probe.py TRACE --effect YFireBall --output DIR` for
packet comparison. `--fireball-palette-control` executes the wrong-palette
negative control. `yfireball_initializer_probe.py EXE --output DIR` retains
the conditional source-height controls.
