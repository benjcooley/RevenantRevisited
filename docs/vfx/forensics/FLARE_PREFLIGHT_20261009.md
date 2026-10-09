# Flare: exact animator, missing retail asset binding

The small-leaf preflight is deferred. Retail does register the animator name
`Flare`, but the supplied retail imagery archive has no exact Flare type and no
`Flare.i3d` asset. A genuine mesh/texture cannot be selected from the registry
for a rendered comparison. The existing IrisFlare substitution is not evidence
of original Flare rendering.

## Exact code and active count

Registration `0x4df380` names Flare at `0x5e1064` and installs builder
`0x5a89b0`; its factory is `0x4f4bc0`, animator vtable `0x5a89b4`.
Initialize is `0x4df3a0`, Animate `0x4df430`, Render `0x4df550`.
Initialize clears 15 position/velocity slots. Animate and Render each process
10 active particles; the extra cleared slots are not a reason to change the
snapshot port's active count to 15. Active velocity starts at animator `+0xfc`
and position at `+0x1b0`. Native Animate uses the snapshot bounce/decay/respawn
sequence, random ranges `(0,6)`, `(0,6)`, `(5,8)` and gravity 0.5. Native Render
draws imagery object 0 ten times with scale 4 and rotations `(-pi/2,0,-pi/4)`.

This is stationary-owner animation with internally moving particles, not a
moving missile and not a two-endpoint effect. A future state proof should cover
each particle's launch, trajectory, floor bounce, decay and respawn through
actual native Animate, not static particle positions. This inspection did not
execute the native functions or compare compiled port state.

## Registry and imagery blocker

The shipped `class.def`, SHA256
`bf46eaa4e25a43fa9828e1e8737114d63b9525f3beb137bf5b6b2963c39c1d5d`,
has no exact `"Flare"` type binding. Across all 6,101 entries in `data/imagery.rvi`,
the only flare-named resources are:

- `Imagery/Magic/tflare.i3d`, SHA256
  `2ad21dd5b5d6b8009cf925bb47b02fe43a3a2e5bb0ec264b31324f049420a40a`.
- `Imagery/Misc/irisflare.i3d`, SHA256
  `f697b802a7b0edb540c3b4f23ffd864b9b5e1253979af65a1a51c95ff70f30c5`.

Neither is substituted without an original Flare caller/binding. The old
knowledge catalog's `Misc\Flare.I3D` association is unsupported by this shipped
registry/archive. IrisFlare and Teleporter have distinct native compound
controllers; see [their preflight](IRISFLARE_TELEPORTER_PREFLIGHT_20261009.md).

`TFlareEffect_Bespoke` currently supplies ten-particle snapshot kinematics, loads
IrisFlare imagery and draws a guessed 48-unit card with a 41-ms cadence and an
added warm light. Authored card geometry/material, exact cadence and light
coupling have no retail acceptance from this preflight. Correcting those
without a verified asset/caller would require inference outside this bounded
task, so no production edits were made.

**Next action:** retain standalone Flare as a diagnostic preview. Resume its
retail appearance work if a genuine caller/asset binding is recovered; prioritize
an actual registered effect row in the meantime. No state/rendered row or
full-effect credit is added. No new queue, DOSBox use or shared-engine changes.

## Reproduce

```sh
python tools/retail_runtime/flare_preflight.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output <private-directory>/diagnostic.json
```

The read-only diagnostic pins the unchanged executable, registration bytes,
vtable slots, Initialize/Animate pool limits, registry hash and neighboring
resource hashes. It refuses a changed executable or newly available exact
Flare binding/asset so the deferred conclusion must then be reassessed.
Current evidence is retained at
`research/vfx-next-20261009/flare-preflight/diagnostic.json` in the retail lab;
`native_execution=false`, `state_acceptance=false`, `rendered_pairs=0`.
