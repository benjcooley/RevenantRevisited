# Smoke: exact named custom controller and original quality gate

Smoke `0x224a5ccc` uses its own `Misc/smoke.I3D`, SHA-256
`5029b94d06420c8beacc253d88f75fbf8d167306a7950be3d6a37330b2bb72e9`.
The capitalized original registration selects factory `4f65d0`, initializer
`4e7d80`, animation `4e7e50`, render `4e7fa0`, reset `4e7c60`.
It is distinct from MistFog and the lowercase default animator lookup.

The source preserves all 50 puffs, seven original MSVC random draws per reset,
center motion/bounce, gravity, literal below-zero drift, growth and late shrink.
Original initialization consumes 350 random values and kills at quality below3.
`--partsys-quality=3` is now explicitly accepted; the existing default is unchanged.
The actual extracted CLI block admits0/1/2/3, rejects4/negative/fractional/missing
values, and retains the parser's existing first-value behavior for duplicates.

Its literal state0 flags are `0x2000`, not looping. The original map/screen order
is Pulse, NextFrame, then Animate; custom Animate clears CommandDone at `4e7e62`.
The adapter preserves this handshake. Reversing Pulse/NextFrame or omitting
Animate's clear kills the original owner; changing the asset loop flag is not a fix.

The old-format asset has one four-vertex/two-triangle object and one ARGB4444
texture. A strict own-resource predicate admits exactly its ten8-byte keys over
three states (1/1/8 frames). All90 decoded channels agree with original `409950`;
8,498 altered resource controls reject under ASan/UBSan. The other old-format
zero-key policy remains unchanged; existing Ribbon and Vortex controls pass.
One fixed GPU mesh is reused for all50 ordered source-alpha draws.

Private evidence under `research/vfx-wide-20261010/hard-astra/`:
- `smoke-state-final2`:917,727 fields, quality0/1/3,601 samples each; exact RNG,
  maximum printed float difference4.99023443e-7. Full original renders preserve state/RNG.
- `smoke-keys-final`, `smoke-ribbon-regression`, `smoke-vortex-regression`:
  strict keys, payload/bounds and unchanged default-path controls.
- `smoke-cli-final`, `smoke-owner-final`: parser and original lifecycle controls.

The full-render fixture executes original owner, authored key cache, all50
RenderObject calls, illumination and software ARGB4444 raster. Immutable decoded
resource/COM surfaces, base script/bookkeeping, declared light/camera and quality
are explicit boundaries. Appearance review and actual GPU/gallery evidence are
pending; no natural map, caller, device or exact-pixel acceptance is claimed here.
