# Blood review: missing color pass and erased alpha

The actual map review submitted live Blood particles but produced no effect
pixels. An otherwise identical SolidColor control showed their motion, splat,
shrink and repeat, excluding queue timing, projection and depth as the cause.

The shipped `Misc/Blood.I3D` descriptors contradict the previous resettled
interpretation: texture0 is RGB565 (`f800/07e0/001f`) red artwork; texture1 is
ARGB4444 (`0f00/00f0/000f/f000`) black opacity artwork. The previous controller
submitted only texture1, having removed the red pass. The loader additionally
converted every black texel to transparent even when authored alpha existed.

`src/effectcomp.cpp` `TBloodSystem::Render` iterates two passes **per particle**:
`sml/med` (objects4/5, alpha mask), then `sml2/med2` (objects0/1, ONE/ONE color).
The current preview restores this sequence. Billboard material sorting would
reverse texture13→12, so explicitly ordered sprites now act as sort barriers;
ordinary uninterrupted runs retain their existing sort. This is snapshot-source
restoration and a real runtime visibility fix, not a new full native-renderer
fidelity pass. Current billboard approximation and scene-light fidelity remain
open; no claim about dormant device blend state is inferred from this test.

The loader now preserves coverage whenever the pixel format declares alpha.
RGB-only atlases retain the existing strictly-greater-than20% black-key rule.
Focused C++ tests cover transparent/partial/opaque black, RGB565-style inference,
the20% boundary, ordinary sorting and ordered interleaved mask/color pairs.
`tools/retail_runtime/i3d_alpha_impact.py` scans the shipped descriptors/pixels.
Eight catalogue types contain black authored-alpha texels previously erased:
Blood, Quick, Swamp, CharUtility, puke, goldeffect, ogrokwatcher2, ogrokwatcher3.
Only goldeffect had an existing primary visual pass at this audit.

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/blood-review/`.
`review.json` pins baseline/control/final binary and frame hashes;
`control-contact.png` compares them. All three60-frame captures exit normally.
Baseline has0/60 red frames; final has52/60 with a maximum419 red pixels/frame.
The blank tail is the existing shrink/end/camera-exit interval. Settings:
`--test=vfx-review --vfx-review-effects=Blood --vfx-review-first`,640×480,
`--filmstrip=60,0.1 --snapstep=0.1 --snapwarmup=1 --snapseed=1`, private saves,
fountain-command-20261004 data, root review callback integration.

`gold-regression/manifest.json` repeats the prior accepted map configuration:
FontRuntimeLab camera110,10000,10000,16, ambient32 white, seed1/warmup1,
120frames at30Hz with60Hz simulation. All four command/lifecycle rows pass;
process exits normally.84/120 frames are pixel-identical to the prior approved
capture; the remaining36 differ by at most6 pixels/frame. The contact preserves
coin forms/color/trajectory; `comparison.json` records exact deltas. Root independently reviewed the panel and retained scoped approval, with
existing trajectory/timing gaps.

Validation: full Revenant build, `tools/test_blood_render.cpp`, generator/bridge
admission tests, actual Blood negative control and final capture, and Gold replay.
No review layout/source integration files are included in this change.
