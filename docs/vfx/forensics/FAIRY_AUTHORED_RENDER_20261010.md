# Fairy authored moving sprite and dust, 2026-10-10

Exact shipped `Fairy` (`0xad92bd25`, `Misc\Fairy.I3D`, asset SHA256
`45ba6f8b7ace0f070fc1928a3af4c051d8b3af84a64a976d5383e8c72551eb8a`) now
uses its moving `#fairy` mesh and separate `#dust` PartSys controller in the
isolated/review preview. The old bobbing single-billboard preview is replaced.
No shared renderer or viewer/layout change is included.

The complete original 93-entry registry selects default factory `40dc00` and
animator vtable `5a370c`. The fixture executes full `Animate40e2e0` and
`Render40e8d0`, both controller initializers, original compressed keys, owner
matrix, normal-lit base, prelit particles, and both exact 32x32 RGB565 textures.
State0 has a 200-frame loop; the pool persists at the wrap. PartSys tag frame20
is metadata, not a delayed start. Particle Pulse uses the previously rendered
emitter pose. The port samples its own authored keys and uses the production
PartSys simulator; native poses/colors are never inserted into port draws.

`scljitter=-1:1` required the narrow optional parser/simulator extension in
`c7df7987`. Original `402c6b..402ca8` samples integer **tenths**, then adds jitter,
spawn delta, and current scale. Literal constants `5a353c=10` and
`5a3538=0.10000000149` establish the precision. Default-zero endpoints consume
no random draw. Curve jitter is rejected until independently recovered.

Validation:

- 202 complete ticks across the 199→0 wrap: 50,804 live state fields match the
  independently compiled source, maximum error `4.9804689e-7`; every RNG seed
  matches. ASan/UBSan enabled.
- The same 202-tick moving-emitter replay without jitter is byte-identical
  between the previous and current production PartSys implementations.
- Actual common-world and native-domain Metal films each contain 202 nonblack,
  distinct frames, with clean exit. Seven runtime log phases match native dust
  draw counts and emitter translations. Ten matched phases are frozen.
- A separate original state1 transition removes both controllers immediately
  and retains the authored base. The fixture does not invent a particle drain.
  Natural caller/end-state trigger is still outside the looping preview scope.
- Focused constant-range/default-zero/curve-rejection C++ test and full build pass.

Declared configuration: owner/camera `(0,0,0)`, 640x480, black background,
original source ambient38/directional1, quality0, MSVC seed1, 24Hz Animate/Render,
flat ground0, no invoker. Immutable decoded resource records and texture COM
residency are fixture boundaries. Native-domain projection is explicit;
common-world captures retain the ordinary projection. Exact raster equality,
natural caller/attachment/audio and broader scene lighting are not claimed.

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra`.
Binary SHA256: `7541a75a16e6dc2f2da75191833340dd610e9a05b3893ff64ab5e419790fd3ec`.
Manifest pins:

- `fairy-state-final`: `aa9416e1c62657ddae7a7e1f6312ba9a89dc1cc3237647d57a1b5ef9410f2bc2`
- `fairy-native-final`: `09bf44ec26bfb4db71342ed6ad604dac7d4e0313a6d5450fb3dbecf333862890`
- `fairy-metal-final`: `afd9cfec62f272478efa50d4bcd1c10a1510e9ec374d2bb08efbf28d364eecfb`
- `fairy-metal-common`: `cddc0e100b6393041bf1413dfa896fb64cde7155fdaaba6fe52dc826c2512fff`
- `fairy-review-final`: `1c654d5f365528435a20fa300e1d18483e77ac99c85e385d19187ddb945f086f`

Root independently reviewed the ten-phase contact/detail and approved scoped
state0 native-domain appearance: moving sprite, dust colors, form, placement,
and 199→0 wrap. Frozen `fairy-review-final/root-approval.json` SHA256:
`8c076a620de9218cdb9780efe9dc52c6f9c4f74f97281acdebecb763508ac9e3`. This is not exact-pixel or natural-caller acceptance. Root owns ledger
promotion and the integrated gallery capture.
