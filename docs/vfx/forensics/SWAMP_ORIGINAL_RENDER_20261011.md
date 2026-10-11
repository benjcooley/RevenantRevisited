# Swamp: original default owner and whole renderer

Exact shipped identity: `Swamp` / `0xbecffeda` / `Magic/Swamp.i3d`, SHA-256
`8fffedd844d739286531d3799b5cc93d8fb6f5fd2bd517f2f713aef6a07cc614`.
The complete 93-constructor registry selects default builder `5e8508`, factory
`40dc00`, animator vtable `5a370c`. It is distinct from Quick and has its own
30 objects, 56-frame nonlooping state, three textures and two literal tags.

The fixture executes original owner NextFrame `470cc0`, effect Pulse `4de800`,
controller initialization through RefreshControllers `40df90`, default Animate
and complete Render `40e460/40e8d0`, RenderObject and software raster. Decoded
immutable resource arrays/COM residency, base script/map notifications, flat
walk height 1, declared camera and lights are explicit boundaries. The registry
constructors are also executed after resource setup, with byte-equivalent
observed simulation state before/after that registry-only check.

The `fgeys` emitter has 31 slots and uses hidden `#sparks` geometry. RGB565
particles render first with original additive flags; nine ARGB4444 ground cards
and eighteen bubble meshes follow in texture-major source-alpha order. The
remaining `bubble19` object has zero authored faces. The production adapter
keeps all 27 actual meshes under fixed keys, literal inclusive authored keys,
source particle curves, and cached emitter poses. Existing explicit native-domain
source-alpha quantization is reused; no renderer, shader or global policy changes.

Original generic Pulse advances its particle controller before the owner tests
command completion. Consequently tick 56 includes the last controller update,
then sets kill and renders nothing. Later ticks do neither. The native full-pool
comparison includes this final update; a compiled test of the actual adapter
Advance function checks it against an early-return negative control. This is
an observed Swamp caller contract, not a change to other adapters.

Validation:

- All 1,680 object/frame samples: 15,120 key channels and 15,008 accepted matrix
  channels match executed original code; maximum errors 2.41e-7 and 3.58e-7.
  The default decoder path remains byte-identical to explicit `false`.
- Sixty ticks: 38,172 particle field checks; render-stride-three control: 38,104.
  Every RNG seed is exact; maximum pool float error 3.06e-5. ASan/UBSan enabled.
- Actual same-binary common and native-domain Metal films: 60 frames each,
  54 visible and correct tick56/60 blank tail. Eleven whole-frame phases and
  identical actual-size crops are frozen without time, geometry or color fitting.
- Actual translated review map: 120 frames, 118 visible, 114 distinct,
  real owner completion and factory restart, clean shutdown.
- Release build, mapping tests (10), generator consistency, Python compilation,
  and compiled lifecycle control pass.

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra`.

- `swamp-review-final/manifest.json`:
  `6c433527e81b21e8828a83383f662a218494d870213d27431f909bc64084050b`.
- `swamp-gallery/manifest.json`:
  `3fe332b05b6807e36d2b710d00a94631e984bfe68585c9193c6acb653866b9e5`.
- `swamp-native-final2/trace.json`:
  `8ef82841b679e38c4c7932b3f4cf0ea80203c811fc448d1a51973858926c2694`.
- Film binary:
  `7c80941b50e5d5ee923efeeaf77d17fab083e7417db2d55bf6974ea0d3ecc617`.

Configuration: quality0, seed1, source ambient38/directional1, owner/camera0,
1024×768 screen center512/640. Common projection is retained separately.
Independent appearance review is pending at this checkpoint. Natural spell
caller, terrain/actor context, audio, device behavior, destination rounding and
exact pixel equality remain open. Approval records are owned by the root reviewer.
