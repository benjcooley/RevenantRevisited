# Drip → length 20 Ripple: linked thin reference

The merged-production Drip/Ripple child link now has a bounded native reference.
This extends the earlier emitter-only request proof for parameters
`(length 20,height 128,period 1)`; it does not expand default length 64 splashes,
map lighting/scheduling or modern GPU acceptance.

[`drip_ripple_probe.py`](../../../tools/retail_runtime/drip_ripple_probe.py)
runs native parent emitter events and actual original child Init/Animate/Render.
The port independently runs compiled current Drip Advance/Submit, actual Ripple
SpawnForTest factory and Advance/Submit, shared time/random/quad helpers and
authored asset providers. No native ages or ring positions seed port children.
Production source was unchanged for this comparison.

All 145 state frames pass: five children born at ticks 24, 48, 72, 96, 120, with all
live ages 0–20 and removals at 45, 69, 93, 117, 141. This covers 105 live child records,
143 valid parent position/velocity records and their complete ordered journals.
Forty-three selected whole-image RGB565/depth pairs pass, including 17 mixed
head/ring frames; the sequence has 17 distinct images. Maximum corner error is
below 3.35e-7. Two complete state/RNG/pixel replays are exact. Median warm composite
pair time is approximately 5.9ms in the recorded run.

Existing children advance before the parent, and newborns first advance the
following tick. Both actual parameter setters increase the period to1,000,000
before tick 121 to drain the final child. This schedule matches the production
component's time splitter; native sector iteration and newly allocated map-object
ordering remain unverified. Identity owner, white input lighting, fixed camera
and common original software raster remain declared boundaries. Native audio
selection consumes five separately observed raw draws; port audio/global RNG
coupling remains open. No splash or child physics RNG executes in length 20.

[`test_drip_ripple_composite.py`](../../../tools/retail_runtime/test_drip_ripple_composite.py)
passes four regressions: full lifecycle, mixed-texture/final-empty pixels,
field-count truncation and an isolated compiled mutation which removes the actual
parent's child-advancement call. That mutation is rejected at the first slipped
age, tick 25, and missing removal, tick 45. It never edits production files.

[Run instructions and detailed boundaries](../../../recon/retail_asm/runtime/effects/drip-ripple-composite-ab/README.md)
and [machine-readable report](../../../recon/retail_asm/runtime/effects/drip-ripple-composite-ab/manifest.json)
retain native/port states, source-span hashes, actual assets, PNGs and whole-frame
image/depth hashes. Thin execution is primary; no DOSBox or guest OS boot was used.
