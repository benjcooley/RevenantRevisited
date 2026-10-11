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

## Full rendered comparison and source-alpha correction

The first full comparison was held: the shape and motion matched, but Metal's
unquantized source contribution made the puff edges gray. The eight actual
ARGB4444 words now pass512 original Scene2 raster controls (four destinations,
two ignored vertex-color inputs, eight overlapping draws). Dominant `0x0555`
and `0x0666` contribute RGB565 `(0,1,0)` on black. This is the source of the green
rings; a fitted tint or transparent color key would be incorrect.

Smoke now opts into the existing `retail_alpha_source_quantize` policy only for
explicit native-domain rendering. No renderer or shader changed. Original
background arithmetic is `dst - floor(dst*alpha) + floor(src*alpha)` per channel;
GPU destination arithmetic still retains fractional values. This remaining
overlap brightness/raster difference is not exact-pixel acceptance.

Final comparison `smoke-review-quant/manifest.json` SHA-256:
`be59d0eda39b0b174aa35d4605925210c66a179f5cf35ddc3781a36fcd5f5e9f`.
Actual binary: `4fd590321af13234481be13ae60ca9696b3664a4d6acdfca5863011b21b46bfe`.
Ten whole phases2/20/80/140/199/200/250/300/500/600 cover motion, expansion,
shrink/reset and the center's bounce. Both actual films contain600 visible,
distinct frames; logged state samples differ from original by at most4.9952e-8.
The ordinary projection's600 pixel hashes are unchanged by the opt-in fix.

`smoke-native-repeat/replay-check.json` verifies601 state/RNG snapshots,
30,050 draw calls and all11 raster PNGs against a fresh original replay.
`smoke-gallery-quant` verifies the real translated review owner, exact runtime
mapping,300 visible/distinct frames and clean shutdown. Quality0/1 actual GPU
controls remain blank. Natural map placement/caller, device, destination
precision and exact pixels remain separate open gates.

## Independent scoped approval

Root reviewed the corrected actual-size phases80/140 and all ten phases, and
approved overall native-domain quality3 appearance: green rings, puff forms,
motion, shrink/reset and center bounce agree. The destination-rounding brightness
residual remains open; this is not exact pixels or ordinary-mode visual parity.
The frozen `smoke-review-quant/root-approval.json` pins the final comparison,
original repeat, state/key/parser/lifecycle controls, both600-frame films,
quality0/1 blank GPU controls and300-frame translated gallery to the binary above.

Root independently passed the512 literal alpha controls and300-frame translated gallery. A24-PNG repeat rendered all600ticks at24Hz and matched every sampled lane PNG byte-for-byte. The first dense root capture rendered600ticks but exceeded180s while writing376PNGs; it is retained as failed evidence. Final root approval SHA256: `df5fb292d2fabf2ae85ebededbbc645979cb2792a5b4869671a194b520950994`. Destination rounding and the other stated limits remain open.
