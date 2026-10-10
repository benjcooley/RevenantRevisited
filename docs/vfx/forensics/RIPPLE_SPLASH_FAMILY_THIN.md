# Ripple longer/default path: bounded recursive proof

[`ripple_splash_probe.py`](../../../tools/retail_runtime/ripple_splash_probe.py)
extends the previous length 4/20/24 no-splash and linked Drip length 20 evidence.
Current production source passes unchanged for length 64 at origin and
`(-37,29,0)`, and length 96 at `(37,-29,0)`.

Original native Initialize/Animate/Render execute for root, children and
grandchildren in one private Runtime sharing original malloc and CRT RNG.
Independent compiled production factory/Advance/Submit create and own their
descendants. This is not port rings placed from native traces. Three profiles
give 363 state frames, 1,239 live-tree actor records, 2,088 splash records / 12,528
float fields, 120 range/raw RNG invocations and 36 births plus 36 removals. All
states, birth coordinates/lengths, removal identities/ticks and RNG consumption
match. The final child generation is no-splash. Two complete replays are exact.

All 105 authored ring/splash RGB565/depth image pairs match through the common
original software renderer. Maximum corner error is below `4.78e-7`; median warm
pairs take approximately 16–42 ms. UVs and real asset texels are unchanged.

Four end-to-end tests pass. A temporary compiled mutation truncating local
displacement before adding the owner fails at the translated child birth, tick 19.
Original instructions add owner X/Y before ftol, matching current production.
No conversion fix was necessary. Cardinality tests prevent incomplete pool/tree
comparison. Initial raw failed evidence is retained: native kill-flag observation
and unique_ptr vector-compaction destruction have different within-tick orders;
the correct contract compares removal identity/tick and preserves both journals.

The explicit component schedule advances existing children before the parent;
newborns first advance next tick. Native sector scheduling, map allocation/owner
installation, natural lighting/occlusion and modern GPU appearance are open.
Local native matrices execute `0x43ad80`; owner translation is a declared fixture
adapter, not the entire retail world-render path. Supplied observed RNG verifies
effect draw consumption, not whole-game generator identity. No DOSBox was used.

[Detailed instructions/boundaries](../../../recon/retail_asm/runtime/effects/ripple-splash-family-ab/README.md)
and [report](../../../recon/retail_asm/runtime/effects/ripple-splash-family-ab/manifest.json)
retain raw native/port journals, source spans, original RNG observations and pixels.
