# FireFlash native null-spell preflight

2026-10-09. Before any engine correction, the thin VM executes the original
FireFlash effect Initialize `4e1770`, animator Initialize `4e18c0`, Animate
`4e1ad0` and Render `4e1fa0`. The null-spell editor branch requires no live
target actor. Its 150 records are 88 bytes each at animator+12c; 75 initial
sphere slots consume225 RNG calls. All100 simulation ticks execute, with759
range calls and no active particles remaining at tick100. Original movement
lookup tables are initialized by the actual `41e2de..41e535` code range.

Particles genuinely move through original simulation; this is a stationary
owner's null-spell plume, not a stationary substitute for a projectile or a
two-endpoint cast. Natural invoker/target, damage and owner-reaper behavior are
separate. Base animator services and audio are explicit adapters.

The native Render requests mode8, while the current audited helper submission
sets additive metadata. Numeric mode names alone do not establish a discrepancy;
the actual Scene mapping must be executed before any blend correction.
The port also bridges local D3D Z into its common-world owner transform; the
software comparison declares that conversion and retains residuals instead of
adjusting geometry or camera to force equality.

`tools/retail_runtime/fireflash_probe.py` compiles the actual current particle
simulation, integer ConvertToVector and submission bodies, supplies exact
native RNG/table and authored-mesh inputs, and records full-pool, packet and
software image/depth differences. Normal/device illumination, culling and
blend arithmetic remain explicit later gates. No engine or ledger edit is
made by this preflight.

## Retained current-source diagnostic

`effects/fireflash-current-state-diagnostic-20261009/manifest.json` compares
all150 records over101 samples through100 ticks. Frame/RNG and integer fields
match, including759 native range calls. All10 sampled submission counts match
(149 at tick30). There are8,120 binary32 field discrepancies with maximum
absolute difference1.71346e-5. They begin in the initial angles: native Init
multiplies each integer RNG result by one float coefficient; production
divides by360 and then multiplies by float2π, introducing another rounding.
The exact native coefficient and downstream trig/matrix precision require a
separate focused correction and refreshed proof.

Every visible sampled native render requests mode8 while the compiled current
helper packets use additive metadata. The initial diagnostic incorrectly called
that a mismatch; **this classification is withdrawn**. Software pixels are deliberately not
executed or credited in this diagnostic; common-Z conversion, normal/device
lighting and culling remain open. The unvalidated full-pixel route must not
be treated as a passing oracle. No production edit is left in progress.

## Native mapping and causal angle correction

The original Scene function `417d60(mode8,1)` now executes with only its
low-level SetRenderState `417060` interface observed. It writes SrcBlend19=2
and DestBlend20=2, meaning ONE/ONE, alongside ZWrite14=0, ZEnable7=1 and
Cull22=1. The production additive metadata is correct and is not changed.
The older artifact is retained with its false blend classification marked as
historical; it must not guide a renderer correction.

Native `5a39f0` is float `0.01745329238474369`, bytes `35fa8e3c`. Init
`4e1a6a/87/a3` multiplies each integer angle by that coefficient, then stores
once to float. FireFlash-only initialization now uses that single-round
operation instead of float division by360 followed by multiplication by2π.
RNG ordering, range endpoints, emitted counts, trajectories and lifetime are
preserved. Fresh full-pool/packet evidence follows this causal change; no
shared math or rendering helper is edited.

The second narrow correction preserves the original Render fade expression:
`4e203b..55` evaluates mainscale×life/(stopfade−startfade)+float0.05 in extended
precision and stores once. The FireFlash Submit expression now does the same.
No scale, color, geometry or lifetime constant is adjusted to a reference.

Final state/packet evidence is
`effects/fireflash-angle-scale-corrected-20261009/manifest.json`, SHA
`e1dc490eb41cc82b4cc5796a3f1a61e1bf56713c9b51c68f73004bd57737f952`.
Birth state, all angles/scales/other non-position float fields, all integer
fields, six global float fields, frame/RNG and all10 sampled draw counts match.
The original8,120 float discrepancies fall to1,124, exclusively sphere
position X535/Y586/Z3, maximum3.78994e-6 world units. Shared port MtxMultiply
rounds each float accumulation; original43aa90 retains extended products/sums
until a float store. This residual is retained as an open numerical gate;
shared math is not changed in this slice. The diagnostic remains
`differences_found`, not exact-state acceptance.

`test_fireflash.py` executes the full150-record/101-sample native/current proof,
requires exact angles/fade/global/integer/RNG fields and native ONE/ONE mapping,
and permits only the documented small position residual. Software pixels and
the unvalidated common-Z bridge are not executed or credited. The full-port
visible preview and natural spell/target/owner lifecycle remain separate checks.
