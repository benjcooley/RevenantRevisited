# FireWind native null-spell feasibility

2026-10-09. Native thin execution is feasible without a target actor, but the
current port needs a separate controller reconstruction rather than an
angle/fade arithmetic correction. No source fix or rendered A/B credit is made.

Verified retail registration `4e23ea` installs builder vtable `5a9400`; the
animator uses `5a9404`, Initialize `4e2400`, Animate `4e2620`, Render `4e2c80`.
The inline pool contains400 records of92 bytes at animator+13c. Init clears
the complete36,800-byte pool and seeds100 sphere slots with300 range-RNG calls.
Native movement tables execute through `41e2de..41e535`. Base animator services,
silent audio lookup and SetCommandDone are explicit interfaces; original
particle movement executes without host repositioning.

Actual null-spell execution reaches100 ticks without a fault. Slot0 moves from
`(0,0,0)` at birth to `(-0.0738788,-0.951011,29.6998)` at tick1 and
`(-73,-243,17)` at tick100. Active counts include211 at tick28,291 at40,
388 at60 and387 at100; cumulative range calls are4,404 at100. Complete pool
hashes and sample positions are retained in
`recon/retail_asm/runtime/effects/firewind-nullspell-preflight-1791544140352855000/manifest.json`.
The exact shipped asset is `Imagery/Magic/firewind.i3d`, SHA
`ab55c2c0bacb33c2dad3d5a84aa51f57fe502fec498045ded9aebd10862266ea`.

Current `TFireWindEffect_Bespoke` emits one generated billboard, with an
invented sine scale/fade and a two-second lifetime. It has no corresponding
literal400-slot pool/init arithmetic to verify or narrowly patch. YFireWind's
separate placeholder is not evidence of an equivalent controller mapping.

Next work is a source-grounded FireWind controller/render task, preserving
native motion and defining actual spell-target/damage/owner-expiry boundaries.
This preflight does not execute Render, test natural targeting, establish
complete lifecycle, or accept the frozen historical software capture. Keep the
old device diagnostic separate; no DOSBox was used here.

Reproduce from the repository root, using the environment from the runtime
setup guide and a new private output directory:

```sh
.venv/bin/python tools/retail_runtime/firewind_preflight.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output /tmp/firewind-native-preflight
```

The checked-in script pins both executable and asset, executes the original
movement-table builder, and checks the native birth/final metrics. It hashes
all400 records at each sample; the report pins the script/fixture dependencies.
The output directory must not exist, so an earlier result is never overwritten.
