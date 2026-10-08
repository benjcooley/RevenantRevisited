# Exact type routing and separate validation gates

`coverage.py` joins the direct-control registry with all 176 retail ledger rows
by exact hexadecimal type ID. Names alone are insufficient: there are three
registered rows named Flame, while base Ribbon and its seven static colored
assets have different behavior.

Generate or inspect the machine-readable map:

```sh
PY=/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python
"$PY" tools/retail_runtime/coverage.py \
  --output recon/retail_asm/runtime/profile-coverage.json
"$PY" tools/retail_runtime/coverage.py --type-id 0x50ba373c
"$PY" tools/retail_runtime/test_coverage.py \
  --output recon/retail_asm/runtime/coverage-verification.json
```

Current generated counts are **37 exposed retail IDs**, **36 ledger bounded
render frontend rows**, and **0 fully accepted rows**. These are different
counters with different meanings. The map retains all ledger acceptance flags,
hashes the ledger/control inputs and referenced reports, and records:

- `control`: profile, exact selector parameters, supported methods, named-build
  policy and ready-to-use structured example actions. Missing controls stay null.
- `state_frontend`: passing checks for the specified controller/mesh/kernel
  subset; not complete effect behavior.
- `pixel_frontend`: sampled pixel equality with compiled production frontends
  through the **shared original software raster**; not modern GPU parity.
- `runtime`: typed fixture availability, retained command-lifecycle evidence,
  natural trigger and actual map/character context as separate fields.
- `gpu`: unchecked, bounded pass or differences found from explicit GPU
  evidence. Flame's retained Metal diagnostic differences remain open even
  though its frontend/software pairs match.
- `full_acceptance`: the ledger's accepted flag, never inferred from controls
  or partial rendering.

Each exposed type has `control.example_actions` that can become a scenario's
`operations` array. Create a persistent named session with the selected build,
execute the actions, retain captures and pursue the open gates. The moving
FireBall head example supplies distinct source/destination actors and validates
original motion before capture. Its renderer pose inputs remain selected;
trail/burst/sparks/full animator/damage/map removal are not certified.

`rendered_without_direct_adapter` highlights new evidence that needs a typed
wrapper, and `exposed_without_ledger` exposes identity mismatches. Auxiliary
Partsys and shared-missile kernels remain separate from real leaf IDs. Never
assign an unrelated registered ID merely because a family shares code.

Regenerate the map after either the registry or ledger changes. A stored JSON
map is a snapshot; its ledger SHA identifies precisely which gates it read.
The primary loop stays UI-free and thin. Coordinated DOSBox-X hardware checks
remain a separate targeted graphics diagnostic, not implicit test execution.
