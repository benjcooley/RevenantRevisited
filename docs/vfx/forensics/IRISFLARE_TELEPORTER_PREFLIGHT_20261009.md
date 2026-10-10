# IrisFlare and Teleporter: deferred exact-type preflight

The existing `TFlareAnimator_BESPOKE__Teleporter` preview is not a retail
Teleporter comparison. Its factory uses `TFlareEffect_Bespoke`, whose snapshot
controller emits ten bouncing particles. Sharing an imagery path does not make
that controller equivalent to either retail IrisFlare or Teleporter. No new
rendered row or full-effect acceptance follows from this diagnostic.

## Verified registrations and asset

The unchanged retail executable binds two distinct animator builders:

- IrisFlare: registration `0x4e5fa0`, builder `0x5a9f28`, factory `0x4f5f50`,
  vtable `0x5a9f2c`; Initialize `0x4e5fc0`, Animate `0x4e6070`, Render `0x4e61b0`.
- Teleporter: registration `0x4e6a40`, builder `0x5aa3fc`, factory `0x4f6310`,
  vtable `0x5aa400`; Initialize `0x4e7160`, Animate `0x4e7210`, Render `0x4e72b0`.
  Effect registration `0x4e6a20` separately installs effect builder `0x5aa1fc`.

The shipped archive's `class.def` gives exact types IrisFlare `0x120054d1`
and Teleporter `0x369a5d2e`, both referencing `Misc\IrisFlare.I3D`. The resource
`Imagery/Misc/irisflare.i3d` is 31,088 bytes, SHA256
`f697b802a7b0edb540c3b4f23ffd864b9b5e1253979af65a1a51c95ff70f30c5`.
Its body starts at `0x68`, flags 4, with 186 vertices and 204 faces in
`SOld3DImageryBody`. The `I3D_3DIMAGEBODY2` bit 16 is absent. Reading body2
cardinality offsets here misleadingly returns zeros. The production loader
already routes this format to `OldInitializeMesh`; a thin authored-render
fixture needs the corresponding fixed-array object/material/texture/key adapter.
This is a fixture gap, not evidence that the asset is empty or unsupported by
the production loader.

## Native controller contract and next action

The recovered IrisFlare Initialize/Animate functions match
`TIrisFlareAnimator` in `src/effect_old.cpp:5041–5130`: five morphing positions,
growth scales 0.3 and 0.1, phase increment 0.1, 50-tick morph and a reset when
ticks exceeds 200. Render combines five flares and a cylinder. It is stationary
compound animation with internal morphing, not a moving missile. The old
`TFlareAnimator_cls_0x5a9f2c_candidate.yaml` speculation is superseded by this
exact registration/function match; no production mapping was silently changed.

Teleporter has its own effect Pulse and gameplay caster/variant logic. A future
natural teleport comparison must declare source and destination and execute the
caster transition, even though its portal animator is not a missile trajectory.
It cannot borrow IrisFlare state acceptance solely because the two names share
an asset, and it is distinct from the already-audited `teleportation` class.

**Proposed next action:** label the current Flare-based Teleporter preview
unsupported for retail comparison and retain it as a placeholder. Later build
the exact legacy-asset IrisFlare adapter and compiled controller; test Teleporter
separately with its source/destination/caster context. This bounded preflight
stops before broad controller reconstruction. No engine edits, emulator launch,
native function execution, state or image acceptance were performed.

## Reproduce the diagnostic

```sh
python tools/retail_runtime/irisflare_preflight.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output <private-directory>/diagnostic.json
```

The read-only diagnostic verifies baseline and asset hashes, exact registration
bytes, vtable slots, archive type bindings, old-header fields and the current
preview factory association. It refuses changed inputs and emits an explicit
deferred result with `rendered_pairs=0`, `native_execution=false` and the next
action. Current local evidence is retained at
`research/vfx-next-20261009/irisflare-preflight/diagnostic.json` in the retail lab.
