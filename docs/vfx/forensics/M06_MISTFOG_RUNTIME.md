# M06: MistFog source/runtime reference port

Status (2026-10-04): source/state checks, combined build, serialized object load
and actual create/move/delete checks pass. Retail visual acceptance remains
open, particularly authored-normal software illumination. The source audit
and the parent-controlled integration evidence below have separate scope.

## Exact shipped identity and asset

The retail `class.def` in `data/imagery.rvi`, line 3404, registers:
`"MistFog" "misc\\Mistfog.i3d" 0x180674ba {0,3} {}`.
This is distinct from Mist ID 0x2093487a and from the snapshot's additional
Magic MistFog registration 0x6471a673. The preview now loads only the exact
Misc asset; natural owners use their class-definition imagery.

`Imagery/Misc/mistfog.i3d` is 8916 bytes, SHA-256
`83022b78d13d39ea43bd7925a9e19966833e2e39af5fcad000cab4635230591c`.
It has one object (`photon01`), four vertices, one material and one 64x64
RGB565 texture: masks F800/07E0/001F, no alpha mask, pixel-format flags 40h.
Material diffuse/ambient/specular are all (1,1,1,1); emissive is (0,0,0,1).
Decoded texture maxima are (49,48,49), with 3631 nonzero words/4096 pixels.
These are actual dark texels, not a tint to be corrected empirically.

The similarly named Magic asset is also 8916 bytes, SHA-256
`1cb3c44e7b32f182fbec243d92c3a56b7c5ca0d2388365f6ef19b010d178ddf1`.
It has ARGB4444 masks 0F00/00F0/000F/F000 and emissive (1,1,1,1).
The prior candidate fallback preferred this different asset and therefore
changed the material, texture format and software raster path.

Both assets contain the same tilted, offset authored quad, approximately:

- corner0(-9.673756,-2.662171,8.347839), UV(.000499547,.000499785)
- corner1(-2.662162,-9.673761,8.347838), UV(.999500215,.000499547)
- corner2(-6.167960,.843627,-.239572), UV(.000499785,.999500453)
- corner3(.843634,-6.167964,-.239572), UV(.999500453,.999500215)

Normals are approximately (.612372,.612373,.500000). The complete raw
vertices/material/format evidence is retained in
`~/RevenantRetailLab/research/mistfog-asset-audit.json`, decoded directly
from the shipped file's relative offsets using S3DImageryBody/S3DVertex
layouts. No renderer harness was needed for that audit.

## Literal source behavior restored

Original `src/effect_old.cpp:11544–11646` defines 25 puffs. ResetBall consumes
six draws in this order: `random(-32,32)` forX, `rand()/RAND_MAX` forVX,
VY,VZ, `random(0,200)` forlife, then `rand()/RAND_MAX` forsize. Positions
start at Y0/Z0/rotation0; velocities are±.25in XY and0.. .5in Z; size .1..2.8.
The direct float divisions and order are preserved. The previous
`random(0,1000)/1000` substitutions changed both distribution and RNG use.

`SMOKE_GRAV` is explicitly **-.01f** in the snapshot effect.h:671 and port
header:1189. The prior -.012f was a guess. Each tick increments velocity Z,
X/Y, and life/size. The exact branch is `if(z>0) z+=vz; else z-=.008f`.
Starting Z0 therefore moves below zero; this is preserved, not raised to make
a reference visible. After life>200, size loses .2 after its normal +.02; a
size<=.01 resets the puff. Center motion is commented out in the source and
remains absent. `ticks` increments once per simulation tick.

The accumulator uses 1000.0/24.0milliseconds with a tiny floating boundary
epsilon, replacing integer 41ms. Update and submit are separated. Render
still submits all 25 puffs without invented lifetime/size visibility skips.

Snapshot Render requests `OBJ3D_SCL1 | OBJ3D_POS2`, object 0. That means
**authored corner * puff.size + puff.position**, then the live owner world
transform (`T3DImagery::CalcObjectMatrix`, src/3dimage.cpp:1311–1324).
The port submits SQuadDrawItem with those actual corners/UVs and authored
diffuse, removing the guessed 28-world-unit screen-aligned billboard and
warm(.95,.92,.85,.55) tint. There is no added facing rotation.

Snapshot SetAddBlendState (effect_old.cpp:235–243) requests DECALALPHA,
ONE/ONE, depth test enabled and depth writes disabled. The canonical port
keeps AdditiveStraight/TestNoWrite. The exact Misc texture has no alpha;
no Magic alpha texture or fitted material is substituted.

### Material illumination remains an open fidelity gate

MistFog does not request `OBJ3D_VERTS` or prelit vertex overrides. Original
`Revenant/3dimage.cpp:1663–1704` selects the object's actual material and
submits its normal-bearing vertices as **D3DVT_VERTEX**. Its Misc material's
zero emissive differs from the old Magic candidate's white emissive; additive
framebuffer blending alone does not establish that those materials should be
treated as self-lit. The snapshot requests DECALALPHA at the texture stage,
but that request is not evidence of identical illumination across hardware
and software devices.

In the software path this distinction is concrete:
`Revenant/blue/swscene.cpp:435–499` starts with scene ambient, transforms and
normalizes each authored normal, then adds directional and point-light
contributions. `DrawIndexedPrimitive` calls this Illuminate routine at
567 and passes its per-vertex colors to RGB565 textured Gouraud modulation
at 670–709. Illuminate does not consume material emissive. Thus the software
RGB565 path is scene-lit, even though the animator requested additive blending.

The current port's `Unlit` submission does **not** reproduce that software
illumination. Its only alternate quad policy, `LitFlat`
(`src/renderer.h:173`, `src/renderer.cpp:5238–5258`), uses an implicit world-up
normal and scene ambient/sun; it does not preserve this quad's authored
normals, per-vertex point lighting, material response or RGB565 quantization.
Changing to LitFlat would introduce another approximation rather than close
the gate. No illumination change or fitted tint is applied in this batch.
Geometry, state and runtime lifecycle can be verified independently, while
device blend/material illumination parity stays unaccepted.

The smallest useful follow-up is a matched lighting diagnostic with this
exact Misc asset: retain native frames at a documented brighter ambient and
controlled light state, alongside the existing low-ambient failures. Once it
is visible, compare the original vertex-light computation and RGB565 raster
output to the port rather than substituting Magic or fitting brightness.

## Natural runtime and ownership

The typed object and animator builders register `MistFog`. The earlier
single generic MistFog builder was removed with explicit parent authorization
because TObjectBuilder::GetBuilder returns the first matching registration
(object.cpp:324–330). Other generic registrations and fallback remain intact.

Lazy animator attachment calls Initialize only after the owner has its final
nonnegative MapIndex, exact EFFECT/ID 0x180674ba and a ready authored texture.
Initialization checks existing state/component before consuming RNG, so
repeated attachment/upload retries cannot duplicate the effect or reseed it.
One `mistfog_reference` component owns simulation update and custom quad
submission, replacing the ordinary static asset draw. Logs include exact
ID, final index, origin,25 puffs and diffuse.

OffScreen preserves this persistent environmental effect. Local puff positions
are rendered through the current owner transform, so actual MOVE follows the
owner. Components detach/unregister before TObjectInstance frees imagery
(object.cpp:670–690); the runtime borrows owner imagery without separate
allocation/free. The preview owns its loaded imagery through its object and
calls Initialize(false); its existing TickAndSubmit drives manual update only,
avoiding a second global component tick.

## Source/state verification

The standalone helper
`~/RevenantRetailLab/research/validate_mistfog_state.py` extracts actual
snapshot ResetBall/Initialize/Animate and current ResetBall/Advance method
bodies, supplies identical CRT random streams, then compares all 25 puff state
bytes, tick count and the next random draw. **1600 cases pass:**100 seeds,
24/30/60/144 Hz, durations .5/1/3/10seconds, including lifetime resets.
Log: `research/mistfog-runtime-state-validation.log`.

This proves preservation under identical **host CRT** streams, not matching
Windows 98's seeded CRT sequence or binary floating-point implementation.
No global RNG replacement was introduced. effect.cpp syntax checking passed
with the configured project includes/SDK; the parent owns integration build.

## Software reference limitation and next gates

The actual snapshot software device supports RGB565; it must not be described
as generically unsupported. `blue/swscene.cpp:670–727` selects textured
Gouraud lighting modulation for RGB565, while its ARGB4444 branch 613–666
bypasses lighting. With Z enabled and writes disabled, RGB565 uses
`DrawTextureAndModulationGoraudZbufferWOff`, passing **maximum triangle Z**
(at 700–709), whereas ARGB4444 uses its separate alpha compositor and average
triangle Z. Software ignores the stored requested blend factors. Consequently
Mist's approximate ARGB alpha-over diagnostic does not describe this RGB565
Misc asset. Switching blend to Alpha or choosing Magic would change evidence,
not establish reference accuracy.

Parent fresh SW captures retained no changing MistFog pixels at origin Z16 and
in the explicitly elevatedZ80 diagnostic, ambient 16. A known Mist control at
ambient 32 did animate, so global redraw/control failure is not established.
Keep RGB565 lighting, software triangle culling/depth and scene visibility
as open device/reference gates; absence of visible pixels does not justify
changing authored puffZ, texture format, color or geometry. The source audit
above identifies paths; it does not prove which caused the native outcome.

## Parent integration evidence

The combined build and 163 existing tests pass (16 particle, 108 transform,
39 host capture/comparison). Tested binary SHA-256:
`e09bd65a1c23e865fec9d5fc2eb41dfa3fc7910a40edfabf7fcf1c590a6d07fb`.

The retained results are under
`/Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/environment-saved-20261004/`:

- `mistfog-saved/runtime-capture/manifest.json`: byte-identical original
  `Map/48_18_19.dat` slot293 record, position `(19350,20044,16)`,
  MapIndex2032347848, exact ID180674ba. Exactly one `mistfog_reference`
  component attaches beside the normal animator. The 150-image recording
  contains 121 distinct images; frame31 versus91 has 5,708 changing pixels.
- `mistfog-commands/command-capture/manifest.json`: all seven actual ADDAT,
  identity/component, MOVE, persistence, DELETE and absence rows pass.
  Position moves from `(19350,20044,16)` to `(19370,20054,56)`.
  There are 49 distinct images; the final 60 post-delete images are identical
  ground, with no retained effect pixels.
- `validated-runtime-results.json`: hashes both successful manifests and
  records motion and deletion-tail checks, alongside the Waterfall results.

These fixtures retain the original saved effect record but use synthetic
surrounding ground; they do not accept the original map's geometry/lighting.
Numeric ADDAT's requested amount7 is parsed as amount, not Z. EFFECT has no
Amount stat, so its authored `Amount()` fallback remains1. The first failed
validator run is retained as `command-capture-first-attempt`; correcting that
expectation changed no production code. Capture frames completed, but the
owned process stalled during teardown and required termination (exit−15).

Next: a matched native lighting diagnostic with the exact Misc asset, followed
by original vertex-light/RGB565 raster parity and the original Pit setting.
Offscreen persistence is source-audited; a camera-away/back fixture remains
open. General effects-system migration remains deferred until fidelity is
verified.


## 2026-10-07 actual executable state and isolated pixel verification

The thin-runtime probe `tools/retail_runtime/mistfog_probe.py` now runs original
MistFog Initialize0x4f2840, Animate0x4f2950, Render0x4f2ae0 and actual
CalcObjectMatrix0x40a420, followed by original projection/software raster.
It uses exact shipped Misc/mistfog.i3d (RGB565) and records unchanged native
CRT/range random outputs. Compiled current production ResetBall_, Advance and
Submit consume those same inputs with strict range/order checks.

This exposed a rounding difference that the older snapshot-versus-port C++
audit above could not show: source float division/multiply/add rounded sooner
on the host than the shipped x87 code. Retail ResetBall multiplies a float
reciprocal of RAND_MAX and retains intermediate precision until the final
field store. Production ResetBall_ now uses double intermediates with the same
float reciprocal/constants, retaining all six random calls and their order.
Before the correction, 241 native state cases had 10,465 differing float fields;
afterward all 241 cases (25puffs, 0..240authored ticks, including reset cycles)
match float32 values exactly. The actual 432 native random outputs and their
input SHA remain unchanged. The original executable result supersedes the
older host-C++ snapshot audit for these rounding details.

Nine sampled frames (0,1,2,24,48,100,150,200,240) have zero differing RGB565
pixels and exact two-run warm replay. Actual original SCL1|POS2 matrix handling,
geometry/UV and texture bytes are verified; all corners remain inside the
selected viewport. Evidence, source-span/driver hashes, before/after state
counts and paired original/port PNGs are retained in
`recon/retail_asm/runtime/effects/mistfog-frontend-ab/`.

The fixture deliberately supplies full-intensity white transformed-vertex color
and an identity owner/camera setup to isolate effect output. It does not close
the authored-normal illumination, modern hardware ONE/ONE blend, real-map
lighting/depth/placement or natural owner lifecycle gates discussed above.
No brightness, tint, gravity, growth, footprint or lifetime was fitted to an
image. Thin execution remains the normal fast route; targeted DOSBox-X 3D-device
checks remain available to classify software-renderer questions if needed.

A declared main-stack snapshot region additionally removes per-pixel callbacks
for original raster interpolation locals. All16pages are preserved before the
call; other memory writes remain tracked. Nine before/after original color/depth
hashes match and software raster/projection tests pass. The measured MistFog
25-puff pixel-pair median falls from805.24ms to390.26ms. This changes memory
tracking performance, not original rendering instructions or pixel behavior.
