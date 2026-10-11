# FireWind 400-slot native leaf

The shipped FireWind owner uses Initialize `4e2270` and Pulse `4e2290`;
animator vtable `5a9404` uses Initialize `4e2400`, Pulse `4e2620` and Render
`4e2c80`. The animator has 400 literal 92-byte particle records. It initializes
100 sphere particles with 300 CRT draws, then runs the initial plume, sphere,
explosion ring and expanding fire ring. The single-billboard sine/fade
placeholder is replaced by this controller and both authored RGB565 quads.

The shipped owner differs from the old snapshot: damage-radius growth begins
at owner pulse20, advances by3 toward320, and the owner requests removal after
pulse140. The animator itself does not kill at140. The controlled null-spell
preview reproduces the visual lifecycle/removal boundary; natural blast
damage, live target cleanup, spell/audio and game dispatch remain open.
Controller pulses beyond removal are native diagnostic controls only.

Render mutates smoke/fire-ring particle scales. The production core preserves
this division: simulation advances during Pulse; scale updates and ordered
object selection occur in the world-render callback. A stride3 native replay
checks skipped-render cadence instead of deriving cached state from a nominal
owner frame. `random(lo,lo)` returns without consuming a CRT draw; the probe
separately counts range calls and actual CRT draws.

The two authored objects retain literal geometry, normals, UVs and indices.
Original RenderObject selects FVF112 and additive/cull-none scene state, then
executes the actual original normal illumination and RGB565 raster. The port
uses the existing software RGB565 helper policy; no renderer changes, synthetic
white carrier, tint, geometry scaling or screenshot alignment fit are added.
An explicit native owner/projector mode is retained alongside the common-world
capture. Equal submission depths preserve particle order.

Validation:

- Original no-render 161-tick trace: 1,481,200 particle-field comparisons;
  1,479,375 binary32/integer exact. All RNG range-call counts match, ending at
  5,745. Only 1,825 matrix-derived position components differ; maximum error
  1.526e-5 world units. State, life, angles, scale and RNG remain exact.
- Full Render and stride3 records each check 1,490,400 fields including initial
  state; the same bounded position differences are the only discrepancies.
- Ordered geometry checks 3,255 original draw packets and 52,080 matrix
  components; maximum error 1.529e-5. No per-frame correction is applied.
- Actual Metal common/native-domain films: 150 frames each, 131 nonempty,
  seed1, 24Hz and one warmup. Fourteen logged CRT cadence controls match.
  Eighteen selected whole-life/detail phases use identical fixed crops.
- Fresh final captures reproduce all 300 Metal images pixel-for-pixel, all
  20 original images byte-for-byte, and all 161 original controller states.
  The independently approved panels retain identical pixels.
- Largest late-ring authored corner bounds at capture119/pulse120 are
  x19.545..1003.670 and y124.889..667.366, fully inside1024x768. All
  3,255 recorded native packets and all320 native/Metal image bounds fit
  the viewport. Unselected native frames are not individually rasterized.

The controlled registration is `TFireWindEffect_AUTHORED_NULLSPELL`. Spawn
origin is (0,0,78), without an extra lift; screen center is (512,485), image
1024x768, black backdrop. Original scene ambient is (38,38,38), directional
(1,1,1); Metal uses the existing scene32white retail software environment.
Capture N follows original pulse N+1. Exact framebuffer, device and natural
caller equivalence are not claimed.

**YFireWind is excluded.** Its own registrations at `5066a0`/`5067b0` and its
separate asset SHA require its own controller proof. The base factory rejects
asset overrides; an identical object layout cannot establish the same effect.
The independent Y placeholder class is not changed or granted conversion
credit by this work.

Tools: `firewind_visual_reference.py`, `firewind_state_probe.py`,
`firewind_geometry_probe.py`, `firewind_visual_review.py`. Private evidence
lives under `research/vfx-wide-20261010/hard-alpha/firewind-*` in the retail
lab. Pair/case manifests pin native and actual Metal frames, executable and
asset hashes, compiled binary, source, RNG and caller policy. Independent
appearance approval is recorded separately; this document grants no ledger
credit by itself.
