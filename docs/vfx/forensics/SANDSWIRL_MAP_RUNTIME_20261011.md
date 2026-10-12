# Sandswirl actual map runtime, qualified Sparks dependency

This implements the exact Sandswirl class builder (`0xbecefeda`) and parent /
seeking-particle flow. The intended `sand` type is absent from the licensed
registry. Ordinary construction therefore fails closed, sets the kill flag,
and creates no child. It never registers a global alias or silently substitutes
Sparks. A caller can explicitly opt one instance into `UseQualifiedSparks(41)`
before initialization. That admits only the previously verified four-object,
one-texture ARGB4444 Sparks asset and uses four stable mesh keys.

The particle animator runs from its real owner's virtual `Pulse`, using the
engine GameRand stream. Parent setup creates its child with actual
`MapPane.NewObject` and resolves it through `GetInstance`. Tick 75 uses the real
map range query and live character health/identity; the first five eligible
characters are selected in query order. Target reset uses the original 176-byte
particle contract. No captured seed, count, position, or target list is injected.
The global Sparks implementation remains unchanged.

Each Quicksand child is an actual map object, with its own verified resource.
Sandswirl's original public initializer `4edb30` is `ret 8`: it does not bind a
victim or spell. Only these newly created null-spell instances receive the
original three-frame / 56-pulse lifetime adapter. No global Quicksand builder,
accepted target-bound fixture, or damage behavior is replaced. In the declared
snapshot schedule a tick-75 child first pulses at 76 and kills at 131; the parent
kills at 300. The parent never submits its otherwise misleading prototype mesh.

`SANDSWIRL_MAP_QUALIFIED_SPARKS` is a direct diagnostic name only. It creates a
disposable source map, two real Acolytes, actual BeginFighting and the loaded
Quicksand TSpell constructor, then allocates the exact Sandswirl builder. No
named Sandswirl variant exists, so Quicksand remains an explicit caller control.
It also constructs and deletes a normal, unqualified Sandswirl instance and
checks missing-child failure, no child allocation and unchanged RNG.

## Deliberately modified retail allocator boundary

The modern map allocator intentionally uses monotonic, collision-checked IDs
without RNG. Original `44ce30` and the inline `451154..4511c7` allocator reseed
and consume two CRT draws each. The opt-in
`--source-map-allocation-control` runs those original bodies, then replaces only
the generated index and restores their incoming RNG state/counter. This matches
the modern policy while leaving the original NewObject, GetInstance, map
insertion, target query, public initializers, controllers and full renderer
executing. Every replacement records its addresses, incoming/outgoing seed,
raw draw count and supplied ID. It is not an unmodified-retail reference.

The existing unmodified allocator / qualified imagery reference stays separate.
An initial attempt to restore RNG alone retained duplicate time-based IDs and
hit the native allocator's retry limit; that failed run is retained. Unique IDs
are essential to this declared allocator control, not an effect/RNG fit.

Retail `seekz` is an uninitialized stack word. The map-resident parent changes
its observed raw value, but both that nonzero value and explicit source 41 take
the same native boolean branch. No equality claim covers that raw word or other
unused stack parameter bytes. Intended sand appearance, a natural Sandswirl
caller, arbitrary scene/AI scheduling, and undefined-zero `seekz` remain open.

## Frozen current-source evidence

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra`.
Source checkpoint `796ac9b3`; no renderer or retail-preview mapping change.

- `sandswirl-map-native-final/manifest.json`:
  `276a2bbc7787e97f726a85a4a8f950414347b01e973c185462051716e1251805`.
- `sandswirl-map-metal-final2/manifest.json`:
  `b9833ee6ed9ddbeb826452ff017bc921cce346d3fd364754d842855877c40a2b`.
- `sandswirl-map-review-final/manifest.json`:
  `6861534ee61650255457476382537b74ced4965b964ffc677ce79c23431b3bfa`.
- Actual binary:
  `be73391bf82eb1f588b0db2c478df55fcc57d25e50b49b4213bdc32db902183a`.

The 310-frame actual Metal film exits cleanly, with 93 nonblank and 94 distinct
frames. All 28,210 pool rows (282,100 fields), 5,942 particle packets, 310
parent/RNG/spell-timer rows and 236 spawned Quicksand lifetime rows agree with
the declared modified-retail reference. The six-draw global counter prefix is
recorded; it is not a stream reset. The real spell remains at timer −1 through
299 and becomes 0 at 300. Both actors finish at their original positions,
health 102 and flags `0x406c028`. Actual source missing-dependency failure,
map deletion and RNG invariance pass. The eight-phase early check and final
18-phase whole/detail panels retain source-alpha raster differences; no
intended sand appearance or exact-pixel claim is granted here.

The earlier 310-frame capture is retained as failed because its new factory
omitted the mandatory native-domain policy log. The completion callback fix and
log addition leave all 310 PNG hashes byte-identical. The separate initial
stride-10 scenario accidentally requested 3,100 simulation ticks and was
explicitly terminated; it is not final capture evidence.

Focused checks: current build, compiled seeking allocation/reset/render guards
and RNG callback equivalence, unchanged existing Sparks native lifecycle,
32 preview-mapping tests, and Python syntax checks pass. The original-core
comparison additionally matches 283,030 state/RNG fields with zero float error.

Private root runner `reproduce_sandswirl_map_root.py` accepts `--output` and a
required `--binary-sha256`, defaulting to the root checkout's existing
`build-merge/Revenant`. It does not build, edit root or launch anything until
invoked. It checks frozen pins, reexecutes the complete qualified original,
runs current-source focused tests, captures all 310 current GPU phases, checks
decoded-image repeats and regenerates the full runtime review. Every artifact
and copied SAVE lives under fresh output; source and binary hashes are checked
before and after every stage.

The private runner SHA-256 is
`bd200c7dd6d7076404621975640972cfaf8d6e4d9fae9aedc7f009b82883ee1a`.
Use the retail-asm venv Python, then the runner with
`--binary-sha256 <current-root-binary-hash> --output <fresh-private-directory>`.
`--verify-only` performs pin checks without any capture; `--root` and `--binary`
allow a separately owned checkout/binary during preflight.

The complete runner self-check passed in the private checkout: all 310 original
records and 310 decoded/PNG frames repeat exactly, with all focused tests and
runtime checks passing. `sandswirl-map-repro-selfcheck/reproduction-manifest.json`
SHA-256: `cf13fbdd9b7b75f83a8493386e539da66cc8b0a0335870ecb204525f66d4ee3e`. Root reproduction and approval remain separate.
