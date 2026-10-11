# YManaDrain: distinct original Pulse and uncapped transfer

Exact type `0xae92bc14`, own `Magic/YManadrain.I3D` SHA-256
`cb60167ba7f3930529078929bfed959a0a714fb8c2f559c8a2169930ec821e15`.
The declared fixture uses genuine original Acolytes at(3185,3185,16) and
(3281,3185,16), actual BeginFighting, loaded `YManaDrain` spell160..260,
exact owner5b1ff0/animator5b21f0 and full original Render508bd0. The own green
textures and authored cone/glow/stream geometry are retained. This is separate
from normal ManaDrain, not an asset alias or a generic particle approximation.

Original5087a0 is animator **Pulse**, vtable+2c, invoked inside owner5082c0's
base Pulse before the phase update. Animate+30 is generic40e460. Normal
ManaDrain instead updates in its custom Animate. The source Y wrapper therefore
animates before its owner phase, skips that update until the caster is initialized,
and retains cold zero positions/facing for the first draw. The original first
frame is blank in this fixture. An explicit extra5087a0 call is wrong.

Original508369 deliberately skips its capacity adjustment when caster mana plus
transfer exceeds maximum. With seed1 the initial damage roll is201; at tick21
the real setters change caster252→453 and victim252→51. The port preserves that
uncapped result. Health102/102, positions and flags remain unchanged. The original
20/50/20 phase schedule kills at91; subsequent ticks remain blank. Render consumes
one RNG draw on each of90 live frames. Scale/rotation and all fifteen streams use
the independently verified shared arithmetic, with distinct Y caller cadence.

The authentic startup/actor helper, original SetSpell and summoned-effect list
insertion execute. Timer's reverse owner pointer remains the explicit standalone
residency interface; natural MapPane insertion/cast scheduling is not certified.
Actual Spell::Kill marks the registered owner. Actor/resource/rand/effect/raster
instructions are never replaced. Original owner/PreRender matrix fragments,
material-handle COM getter and RAM output bindings remain declared boundaries.
The fixture uses source ambient38/dir1, camera(3185,3185,0), viewport1280×960,
center(640,640); inherited startup is NOSOUND.

Validation:

- ASan/UBSan compiled actual Y core:10,490 defined state/RNG/mana fields pass,
  maximum float decimal-output difference4.97e-8. Dormant undefined particle
  fields are excluded until their original first activation2+3*index.
- Every original draw through100 ticks:19,680 matrix channels pass, maximum
  difference4.77e-6;704,640 own authored vertex bytes and97,560 indices match.
- Negative controls reject extra custom Pulse and post-owner animation on100
  frames each; an invented caster cap changes80 frames. Wrong old stream speed
  changes98 and omitted initial damage RNG changes100.
- Actual Metal100-frame native-domain run exits0. All90 render RNG counts and
  first positions match original; actual GPU phase error4.56e-8. CSV determines
  ticks2..101; the filmstrip is not an additional simulation sample.
- Fourteen whole pairs5,10,20,21,30,40,50,60,70,71,80,90,91,100 cover visible
  birth, transfer, motion, shrinking and blank tail. Every sampled nonzero-scale
  original geometry bound is contained: aggregate x644.63..803.35,
  y564.42..708.32. No geometry, camera, scale or phase fitting is applied.
- Separate ordinary-world100-frame capture exits0 and retains its projection
  diagnostic. Python exact mapping tests18 and compiled bridge lifecycle tests
  pass; unsupported arbitrary endpoint calls remain rejected.

Two fixed mesh keys reuse existing ARGB4444 source-alpha and native-domain source
quantization policies. No renderer, shader, parser, normal ManaDrain or global
policy changes are made. Configured gallery factories never reseed the game RNG;
only the explicitly named isolated fixture can apply its requested snapshot seed.

Private frozen evidence is under
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/`:

- `ymanadrain-target-native-corrected/manifest.json`:
  `60b2a93157232b75856d7e573b23d5e97d9163d171fafc9107c919872ba9a1a3`
- `ymanadrain-target-state-final/manifest.json`:
  `0cf8e00d0d6082c25fc727051e2841f775d7d8263d18644f5fa337126916fb4d`
- `ymanadrain-target-metal-final/manifest.json`:
  `c9d6d9f8674877d316943d29820fc188c636f8596c3e79ab85de1ee91a6f47cb`
- `ymanadrain-review-final/manifest.json`:
  `b455eabeb910da77ac02a052c88f8bc42b80626802d7685e94cbcfb9f691fbfb`
- `ymanadrain-target-metal-common/manifest.json`:
  `f8a57cd346e04d6e3f2f7cec669dad1741a7bcd4766aa8662d58466d6df13b63`
- Binary: `602e84ecca13b1119ccb55be5aa123df6b2e2a6cd5c14655941db87ba189698c`.

The exploratory `ymanadrain-target-preflight` and interrupted
`ymanadrain-target-native-final` datasets remain retained **invalid double-Pulse
controls**. They are not the authoritative corrected reference and cannot support
appearance credit. Independent root appearance approval/integrated repeat remain
separate. Natural moving actors, health-drain variants, original allocator cold
state, cast scheduling, audio/device rendering and exact destination/framebuffer
pixels remain open.

The same-binary translated gallery exits0 with120 no-UI640×480 captures,
117 visible/114 distinct images. Three real91-tick completions transfer225,202,243
mana (caster477,454,495; victim27,50,9); completed spell timers are0. Health,
flags and positions stay unchanged. A fourth instance is destroyed safely at16
before transfer (timer−1 while live); all four factories log `seeded=0`.
`ymanadrain-configured-gallery-final/manifest.json` SHA-256:
`0ad450fb9093d282d0c88997afaa6c19d22c9952a59a39cf23fa4ca527f37e59`.
This is runtime/restart/cleanup validation, not ordinary-world pixel parity.
