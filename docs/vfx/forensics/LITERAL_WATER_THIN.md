# Literal Water: native drop and authored-quad contract

Audited 2026-10-08 before the scoped production correction. Literal Water is
type `0x1903abcd`, sharing the exact `Misc/water.i3d` asset with Waterfall,
SHA `ece24f00eab927662fea2027dde203afec2525471eb6cde7b2a06f69c9b9aa75`.
The registered Water builder is `66ce98`/vtable `5ad4e0`; its actual factory
`4f9c30` installs animator vtable `5ad4e4`. Native Initialize `4f3410` stores
25 drops, allocates 1,000 bytes and runs 25 internal warmup updates. Native
InitParticle is `4f32c0`, UpdateStuff `4f3380`, Animate `4f34b0`, Render `4f3610`.
All 25 records are required in every comparison.

The native birth state consumes 87 range-RNG calls. Its quad has four authored
vertices, two original triangles and a 64×64 RGB565 texture. Local dimensions
are about 10.00396×9.86306 world units. Render selects mode16, skips delayed
and reset drops, sets OBJ3D_MATRIX and applies scale then translation with no
camera-facing rotation. Its copied-vertex lighting callback does not define
the mesh vertices subsequently selected by the native RenderObject boundary.

The current preview instead generates 16-unit ScreenAligned billboards and
uses integer `1000/24` milliseconds for its simulation gate. These are source
contract discrepancies: geometry is invented and long fixed24Hz runs can
advance extra updates. The before probe compiles the actual Water methods and
captures those submissions alongside the complete native drop machine.

The correction is restricted to literal Water's methods/members. Retain native
25-drop allocation, warmup, RNG, delays, velocities, scales and respawn rules;
use the true24Hz clock and exact authored vertices/UVs with S→T matrices.
Unverified alternate asset delegates retain their prior behavior and receive
no new mapping credit. Native map/component binding is a separate gate.

The bounded pixel fixture uses the original software projection/raster with
the original indices, identity owner, face0 and white vertex inputs. Material
illumination, additive device arithmetic, modern triangulation and Metal are
explicit separate gates; a shared-raster pass does not establish them.

## Before and after

`effects/water-before-20261008-ab/manifest.json` retains the compiled-current
failure: generated billboards instead of authored quad packets and the first
full-pool state mismatch at tick62. The original native images and source-span
hashes are retained; no fabricated port image is substituted for the missing
authored packet.

`effects/water-after-20261008-ab/manifest.json` passes 97 samples of all25 drops:
24,250 time/float state fields plus RNG count comparisons. Native and port use
420 RNG calls through96 ticks. All8 sampled RGB565/depth pairs are exact,
twice replayed; maximum corner error is3.79e-6 world units. The corrected
literal path submits the shipped quad with S→T and a true1/24-second tick.
It preserves mode16 metadata and the authored diffuse input. No asset size,
orientation, timing or brightness was fitted to the reference.

The actual extracted production Advance is also checked at6/24/30/60/144Hz:
four seconds yields96 updates and the same complete native25-drop final pool
and RNG count. Nonfinite, zero and negative deltas do not mutate state.

```sh
.venv/bin/python tools/retail_runtime/water_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output recon/retail_asm/runtime/effects/water-fresh-proof
.venv/bin/python -m unittest discover -s tools/retail_runtime -p 'test_water.py'
```

Output directories must be new, retaining earlier failure and pass evidence.
Actual map registration/component ownership and Metal/device/material parity
remain separate; this proof does not promote FlowWater or other asset aliases.

## Shared preview and map producer

Literal Water now follows the existing Waterfall owner pattern: the exact-ID
animator attachment calls an idempotent Initialize; preview requests
Initialize(false), and a map owner requests Initialize(true) with one
`TLiteralWaterReferenceComponent`. Both use the same Advance and Submit
methods. Imagery not yet ready consumes no RNG and attaches no component.
Repeated initialization does not allocate or repeat warmup. Normal offscreen
cleanup preserves a literal Water owner, with no additional RNG/component on
reattachment; unverified alternate previews retain their prior kill behavior.

`effects/water-runtime-shared-current-20261008-ab/manifest.json` repeats the
native state/pixel pass and compiles the actual Initialize/component methods.
Its independent preview and runtime producer traces are byte-identical across
all97 samples, including late imagery, duplicate-init and offscreen checks.
Asset delivery, component registration and base offscreen services are explicit
test boundaries; the live map ADDAT/MOVE/DELETE check remains root's separate
integration step. This result does not prove native natural-map lifecycle or
device/Metal pixel parity.

The live preview initially exposed an identity assumption in the new log:
preview owners have no class/type `inf`, so calling ObjId dereferenced null.
The logger now uses the known literal ID for preview and reads ObjId only for
runtime. The producer regression makes preview ObjId throw, catching this real
failure rather than hiding it behind a fully populated fake owner.

Final evidence is
`effects/water-final-interpolated-20261008-ab/manifest.json`. The same97 native
states and8 exact image/depth pairs pass after the shared-initializer and log
corrections. At display fractions, Submit adds active-drop velocity times the
remaining tick fraction to its render position only. Fractional6/60/144Hz
checks observe the expected displacement and no changes to all25 records or
RNG; exact integer-tick packets remain unchanged. This is a modern display
interpolation policy, separate from original discrete native simulation.

## Actual executable checks

The combined game build passes. Actual Water preview completes12changing,
nonempty images and exits normally. The typed map path creates one
`water_reference` component, stays live after movement, passes eight timeline
observations, generates59distinct active images over64captured frames, and
restores the prior clean floor after deletion. These use a synthetic floor-only
module, not the original environmental placement. The first native preview
attempt crashed in `ObjId()` from a diagnostic log; the failed manifest and
LLDB stack are preserved. Logging now avoids missing preview class/type info,
and the source-backed provider rejects such an access. Uninitialized alternate
previews also keep their prior offscreen policy without reading absent identity.

Final source proof is `effects/water-final-identity-safe-20261008-ab/manifest.json`;
previous passing and failed reports are retained. Actual binary captures remain
in the local lab's `research/vfx-next-20261008/water-*` directories.
