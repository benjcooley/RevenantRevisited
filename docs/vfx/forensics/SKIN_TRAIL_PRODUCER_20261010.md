# Original constant particle trails and malformed zero-vector key, 2026-10-10

The shipped Stone/Iskin tags contain the exact literal
`localrotation=[0:(0,0,0),100(0,0,0)]`. Executed original parser `4010d0`
stores two zero-time, zero-value curve keys: the missing colon causes the first
vector zero to overwrite the key time. The production compatibility case
recognizes only this literal and preserves both keys and curve presence. It
does not repair the second time to 100 or replace the curve with a constant.
A native colon-insertion negative control instead stores time 100. Nonzero,
underflowing nonzero and other malformed literals still fail closed.

`numtrails` admits constant equal integer endpoints from 0 through 64; ranges
and curves stay unsupported. The original spawn path `403275..4032e7` allocates
history entries initialized to the original corrected spawn position. Original
Render `402925..402a0b` shifts history, draws all shifted historical positions,
and writes/draws current at the end, after its main current draw. Three trails
therefore produce four draws per renderable particle, including duplicate
current. They reuse current color, scale and sampled rotation. History advances
only through the explicit `SampleTrailPositions` Render API, never during Pulse.
Constant trail counts consume no RNG. The default remains zero.

Four independent whole-native/source comparisons (both exact assets, render
stride 1 and 3) pass 19,030 fields each over 92 ticks, including all nine history
coordinates per live slot: 865 complete 22-field rows each. Every RNG/live/age/
lifespan value is exact; maximum float error is 0.0000114515. Native initialization
and draw-count controls verify default-zero and omitted trails produce identical
serialized state/RNG/draw/pixel records for both assets. Tiny/zero-scale particles
retain history even when their geometry rejects. Repeated-render and skipped-
render controls exercise the separate history clock.

`test_skin_trails.py` compiles source parser/history controls under ASan/UBSan.
It also compares byte traces against published source `c692baf5` for unchanged
Speed, Restorelife, Dexterity and Antimagic tags, including render samples/RNG.
All four default-zero traces are byte-identical. No shader, renderer, generic
profile admission or natural-caller claim changes with this producer addition.

Impact inventory: Magic/Stone and Magic/Iskin use 3:3. Magic/Impact and
Misc/Impact contain 2:3 (still rejected) and 0:0 (equivalent to omitted). They
receive no new identity or appearance credit. The dedicated skin adapter is
responsible for calling the history API and retaining all authored draws.

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra`.

Frozen manifests (SHA256):

- `skin-parser-controls-final/manifest.json`: `4a25f830dd606bfb33ec13a11cd90d17e27dfb177771b573e07d63ab3402bafd`.
- `stoneskin-state-first/manifest.json`: `e3a9edd1e36788110bf3b68d709421ce75328ec6c24111972acd47482a3fba2d`.
- `stoneskin-state-stride3/manifest.json`: `3fcb1c968fe155ca649791f35f6eba32152f8edf4fa3babc5c6b31c64577f110`.
- `ironskin-state-first/manifest.json`: `7102b80de1de8c0967fb7586837a0a67b2519f6e5ad79824f12b8be3c8338c30`.
- `ironskin-state-stride3/manifest.json`: `2261b882256aaf0412e7a564417f9c41b91410a5f18a296f8712f06948d093c5`.
