# RGB565 software particle modulation

2026-10-10. fspray's actual Metal sprites were whiter/cyan than its original
software sprites. Timing was already correct: all 42 logged RNG checkpoints
across Regeneration, SwiftStrike and fspray's native/common films matched.
Their literal textures are RGB565 (`f800/07e0/001f`, no alpha); the separate
ARGB texture preservation correction does not apply.

The complete original fspray controller selects prelit FVF `1e2`, blend16,
ONE/ONE, depth test on/no writes, CULL_NONE. The FX quad shader point-sampled
RGB565 but still used normalized texture times color. Original software first
indexes its RGB5 modulation table and clears low bits before additive blending.
The helper mesh path already had that operation; the authored quad path did not.

For a source texel and independent RGB5 light channels, the operation is:

1. Extract five red/blue bits and the high five green texture bits.
2. Per channel, truncate `texture5 * light5 / 31`.
3. Clear red/blue bit zero; double green into its six-bit field.
4. Normalize by 31/63/31 for GPU output.

An actual fspray color `ff0e2618` becomes RGB5 `(1,4,3)`. Literal texel `9492`
produces native channel integers `(0,4,0)`; normalized multiplication instead
retains `(0.581,4.645,1.742)`. This explains why the dim native spray is greener.
No tint constant, fitted texture, geometry, phase or seed is involved.

## Scope and safeguards

`SubmitPartSys` opts in only for exact admitted state0 static particle profiles
and original blend16. Strict admission pins the RGB565 masks, prototype,
materials, hierarchy and key streams. This currently covers Regeneration,
SwiftStrike, fspray, YEnergy, YEnergyLose, YAbsorb, nullifier, Ogrestrength and
trollblood. The last two were additionally checked through the complete original
controller Render/RenderObject: their particle draws also select FVF `1e2` and
ONE/ONE. Their normal-lit base meshes are unchanged.

The quad queue selects explicit texture policy4 only while
`UsesRetailSoftwareMeshLighting()` is true and the requested blend is
AdditiveStraight. Existing ARGB4444 policy3 has precedence. Modern/hardware,
alpha, and unadmitted producers retain their existing policy. Metal, GLSL and
HLSL implement the same source operation. This corrects source modulation;
it does not emulate software destination-bit clearing after every overlapping
blend, coverage, viewport clipping or exact framebuffer storage.

## Proof and reproducible checks

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-alpha/fspray-diagnosis/`.

- `report.json`: 126 original blend16 kernel checks using 43 actual fspray
  capture25 packed colors and 35 literal texture values. The normalized shader
  is retained as an analytical negative control, not a fabricated GPU image.
- `camera-original-caller.json`: original Ogrestrength/trollblood particle
  RenderObject FVF/state traces; no source geometry/colors supply these calls.
- `test_rgb565_quad_shader.py`: mechanically adapts the actual arithmetic blocks
  of all three shaders to Clang vector syntax, compiles with ASan/UBSan, and
  compares 514 independent colored cases against the original raster. It
  preserves arithmetic and swizzles and checks all backend results agree.
- `tools/test_fx_source_alpha.py`: compiles the actual quad queue selection,
  checks 32 combinations of software/modern, ARGB/RGB565 opt-ins and blend modes.
  Existing alpha and modern fallbacks remain unchanged.
- Existing strict profile and full quad producer proofs retain their raw
  geometry/color comparisons and now check the RGB565 opt-in flag.

`metal-final/` contains fresh 90-frame native/common Metal films for all seven
prototype-only profiles. `camera-metal-final/` captures the two whole camera
particle effects and their base meshes. `native-index/manifest.json` indexes
already frozen independent native references without changing them.
`old-metal-index/` points to the preceding normalized-quad films.

`fspray-review-final/fspray/` freezes twelve pairs, panels, binary/configuration
and hashes. Across twelve phases, RGB L1 error falls 382,623 to 281,771 with the
native projector and 416,159 to 284,374 with the common projector. All 14 phase
RNG checks remain equal. The green tint and its time evolution now match in
lane visual review; exact pixels and natural spell/map callers remain open.

No images, assets, executable or generated test binary are committed. Central
ledger acceptance is independently owned by the integration reviewer.

## Final regression freeze

All 16 actual films pass (1,380 frames), with 110 equal logged RNG checkpoints.
`review-final/` contains twelve phases for each of the seven static types under
both native and common projectors; `camera-review-final/` contains six phases
for each complete camera effect. Lane review finds the form, evolution and
color preserved or closer for all nine types. Every type has lower summed
native-domain RGB error. `regression-summary.json` retains all before/after
values; no automatic pixel-equality claim is made.

- Static review index SHA256:
  `37786202898977eedaeda0d5293f348710fd96f35705b6aaa14a23764ea81c34`.
- Camera review manifest SHA256:
  `1944d62c6ee4a95d57114224a7292cc29507c5d170ac38c8bc5afe8a79f5af45`.
  Its separate `panel-hashes.json` pins both full-frame panels.
- Regression summary SHA256:
  `3f0bf58d4f2ce6b7d8dbe9aae7a426dab620d361898bd6dc5a40204faeddf85e`.
- Actual Metal capture binary SHA256:
  `24a12313b3255b3d4a65f02d04e59e44d0df031f36916de753be47840fb52205`.

Build, shader/native arithmetic, queue policy, quad capacity, five static tests,
additional producer opt-in assertions, four original ARGB/RGB raster tests,
Speed packets and Quicksilver packets all pass. A broader Quicksilver admission
test exposed an independent regression already present in merged `f5d5644`:
that camera batch removed the existing `NumStates()!=1` guards from several
one-state profiles. Its two-state header mutation is incorrectly admitted.
The failure is retained in `quicksilver-profile-debug/production-run.log` and
reported to integration for restoration; this patch does not edit those guards.
