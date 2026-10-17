# FireCone native leaf state comparison

2026-10-09: the null-spell original FireCone controller and all three original
particle pools execute in the thin emulator. After a small arithmetic correction,
the compiled production simulation matches every defined active field bit for
bit through natural drain. This is state evidence, not a rendered comparison or
complete FireCone acceptance.

Type `FireCone`, ID `ab92cd01`, animator vtable `5aacac`: original Initialize
`4e9f00`, Animate `4ea060`. The comparison runs original shared
`TParticleSystem::Init/Animate/Add` at `50c170/50c1b0/50c560`; it does not substitute
a host particle model. Original CRT heap/thread initialization and original CRT
allocation run before the pools. Original random `483300` executes and its
observed integer results feed the compiled production with strict argument/order
checks.

## Executed fixture and scope

The fixture has a null spell, owner at `(10000,10000,16)`, face zero, and the exact
three pool capacities: fire 80, smoke 80, burst 100. Native Initialize moves the
owner to Z116 and initializes fire, burst, then smoke, with movement enabled.
Every tick's actual shared-pool Animate/Add entry order is compared to an
observed compiled production entry order. Entry observers do not change the
simulation. Base animator operations, owner virtual position/command/flags
operations, imagery GetObject and absent audio are declared boundaries.
The audio lookup returns unavailable, and the original skips that branch.

The candidate driver compiles the current source bodies of
`TFireConeEffect_Bespoke::Pool::Add`, `Pool::Animate`, and `SimulateTick`, with the
actual particle definition and pool capacities. It starts from the initialized
leaf state; it does not certify the port's imagery loading, complete Initialize,
runtime component registration, or render callbacks.

A 100-tick run checks all 260 slot occupancy values, every defined active field,
phase/frame/done/alive values, 1,243 native RNG calls and their argument order.
The source transitions to MID at tick6, BLAST at15, done at33, and drains at93.
Unused stale slot fields have no simulation meaning. Original burst `temp` and
smoke `flicker` are copied from stack fields without initialization and are never
read by this effect's Animate/Render, so their arbitrary bytes are excluded
explicitly. The complete occupancy pools remain checked.

## Causal arithmetic correction

The first original/port difference was tick2 fire slot0 scale:
`0.30263999104499817` versus `0.3026399612426758`. At `4ea915..4ea937`, original x87
retains `(125-pos.z)*float(0.01)` while duplicating the expression and multiplying
by each `temp` component. Only final scale values are stored as float32. The
previous C++ `float equ` introduced a rounding store absent from that code.
Smoke uses the same chain at `4ea983..4ea9be`, with its existing zero clamp.

The MID approach at `4ea8b2..4ea8f1` similarly retains subtraction, multiplication
by 0.25 and addition until the position store. A float `diff` caused the remaining
position drift. FireCone now uses extended intermediates for these two expression
groups, preserving the literal float coefficient `0.01f`. No constants, random
calls, phase thresholds, pool lifetime, motion rules, material, geometry, camera,
or blend mode were tuned.

Before: 3,812 differing float32 fields, maximum `1.9073486328125e-6`; all integer,
occupancy, RNG and pool entry-order checks already matched.
After: **73,806 defined active float fields exactly match**, with zero state,
RNG or pool call-order differences through natural drain.

Local reports are intentionally outside the source repository:

- Before: `/Users/benjamincooley/RevenantRetailLab/research/vfx-next-20261009/firecone-native-order/manifest.json`, SHA256 `ba668714a74227d8345feac5aa71aff6f0675ea028a7f11002063935d2af626c`.
- After: `/Users/benjamincooley/RevenantRetailLab/research/vfx-next-20261009/firecone-native-corrected/manifest.json`, SHA256 `76124520063ea50d68aeb060de48dc3dd3190d449abcb56761d522896da7055d`.

Each includes source/body/driver/binary hashes, full-pool state-file hashes,
per-tick summaries and original helper entry ordering. Original executable SHA256
is `28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.

## Reproduction and regression

From the validation worktree, using the documented thin runtime Python environment:

```sh
python tools/retail_runtime/firecone_state_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output /tmp/firecone-state-new-run
python -m unittest discover -s tools/retail_runtime -p 'test_firecone_state.py'
```

The output directory must be new. Two tests verify the complete 100-tick oracle
and a negative control: restoring the float scale intermediate must reproduce the
tick2 mismatch before the first phase transition. These tests pass in 3.7 seconds
on this machine.

Rendering, live caster/target interactions, damage/burn, audio, nonzero-facing
render matrices, device differences and natural game context remain open.
`DragonFire` is a different controller and gains no coverage from this proof.
The older textual source-sequence checker needs its declared arithmetic adaptations
updated if reused; the current executed binary comparison is the relevant proof.
