# Restored Sand: two public target moves

This followup extends the explicitly restored legacy Sand configuration in
[the binding record](SANDSWIRL_LEGACY_SAND_RESTORATION_20261011.md). It does not
enable a shipped missing binding, alias Sparks, add a natural Sandswirl spell,
or change the production controller. `REV_SANDSWIRL_MOVE_TARGET=1` affects only
the explicit restored-Sand map fixture; absent the variable, behavior is unchanged.

Both engines execute the genuine character `SetPos` API before the ordinary
owner/child snapshot at ticks 60 and 85. The declared positions are respectively
(3265,3233,16) and (3249,3265,16), relative to the existing source at
(3185,3185,16). Both moves return their actual map index and consume no RNG.
Native and source allocator IDs deliberately differ; neither return is replaced.

The original query samples the actor at tick 75 and produces relative target
(80,48,-32). All 236 subsequent native parameter rows retain this cached point,
including after the second actor move. The source independently reproduces that
behavior. This is a public-caller motion test, not continuous homing or actor AI.

## Results

- All 310 original Pulse/Animate/Render calls and software framebuffer hashes
  retained; 26 selected original PNGs supplement the full actual GPU film.
- Compiled controller: 283,030 pool/RNG fields and 6,004 draw packets bit-exact.
- Actual source map runtime: 282,100 pool fields, 6,004 packets, 310 phase/RNG
  rows, 310 cached-target rows and 236 Quicksand lifetime rows exact.
- Original stationary/moving states, packets and framebuffer hashes are equal
  through tick 74. The first pool difference is tick 75. Actual GPU stationary
  and moving images remain equal through tick 75 and first differ at tick 76,
  when the newly directed velocity advances position. No phase adjustment.
- Both actors finish with health 102 and flags 0x406c028; spell timer is zero.
  The target retains the second declared position. All captures exit cleanly.
- The current binary's stationary control preserves every one of the previously
  approved 310 PNGs and decoded images. No renderer, particle core, default
  factory, RNG seed or event-injection change was made.

## Frozen artifacts

All paths below are private under
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/`.
Each digest is SHA-256 of `manifest.json` in the named directory.

- `sandswirl-restored-sand-moving-native`:
  `ab5815c2cd7393e8a1a1a1cc2de1e33193571b017870c65f47fd9fcbd4b129d0`
- `sandswirl-restored-sand-moving-core`:
  `558dce13ec1b4afa6597d537ba8d9e441d223ce4d2d3976a72e8d2ad574d8c02`
- `sandswirl-restored-sand-moving-metal`:
  `de15b96a31e26401b69439aaa338786fc402d77614eaced6b287a173cfff32fe`
- `sandswirl-restored-sand-moving-review`:
  `b13125598f04c72a2b8e7777e8da3b5d0a7df384b3ef7af1a6397d0b8697621b`
- `sandswirl-restored-sand-moving-causal`:
  `e279cdf9a82a933da3e6be92c44ca897589f8495762e6ee2b71df950cc1ca267`

Current binary:
`1dcf434dc1cae51b3d9d68f5ca7ca6fed518bb319566501b0cd0abd937a13f1d`.
The review includes the complete fixed viewport and a consistent ROI; its
`contact.png` hash is
`3d8ae1cce7c6f022b6e5482ab0841c7254ae4d0d08dfc557838ab278cadd831a`;
`detail.png` is
`32b7c0b3292d21acc4c0088f1e29aaba51c3b83a6456bddbb68d8205f494f021`.
The unchanged stationary capture and runtime review are retained separately as
`sandswirl-restored-sand-stationary-control` and its `-review` sibling.

`sandswirl_moving_target_probe.py` checks the causal stationary/moving original
prefix and cached target. `sandswirl_map_runtime_review.py` checks the actual map
logs against all original pools, packets, actor positions and public moves.
The private `reproduce_sandswirl_moving_target_root.py` pins these artifacts,
requires an explicit binary hash, guards source/binary immutability, supports a
verified `--reuse-native` directory, and captures a new traced 310-frame source
film before regenerating the review. It never builds or edits the root checkout.

## Limits

The guest registry restoration, real named Quicksand spell control, RNG-neutral
map-allocation reference boundary and nonzero seek-Z control remain explicit.
This does not prove a natural Sandswirl caller, actor AI, arbitrary moving targets,
terrain interaction, intended shipped-enabled Sand registration, destination
RGB565 rounding, pixel equality or other device paths. The later actor move is
a negative control for retargeting; the original keeps the tick-75 target.

Root independent integrated proof passed under `research/vfx-sandswirl-root-20261011/moving-target-integrated/`: all310currentGPUimages byteidenticalto frozenmovingcandidate, fullpool/packet/query/cachedtarget/Quicksand/spell checks, actualSetPos noRNG effects, cleanexit andunchangedactorhealth. Original310records/imagesverified beforecurrentcompiledchecks. Rootreviewedfixedquery/contraction/drain phases; qualifiedstationaryappearanceapproval remainsunchanged.
