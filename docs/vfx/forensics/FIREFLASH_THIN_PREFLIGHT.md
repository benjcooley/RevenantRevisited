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
sets additive metadata. This discrepancy needs fresh proof before correction.
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
helper packets use additive metadata. This is a concrete render-state mismatch,
not evidence for brightness fitting. Software pixels are deliberately not
executed or credited in this diagnostic; common-Z conversion, normal/device
lighting and culling remain open. The unvalidated full-pixel route must not
be treated as a passing oracle. No production edit is left in progress.
