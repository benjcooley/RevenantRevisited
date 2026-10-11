# Native owner and projector: explicit VFX capture domain

The common-world owner stretches modelZ by1.5 and keeps raw ownerZ translation.
The pinned retail software path does neither. `retailsoftwaretransform.h`
independently implements the executed owner40e470/40e740 and view/projection
56cdc0/411640/56d5f0 inputs. It is used only by opt-in Speed, Quicksilver and
Fmastery isolated previews; gameplay owners, camera, terrain and input are not
changed.

Owner translation follows the x87 constants at5a351c..28:
`z/(1.4600000381469727-z*.0033333334140479565*.009999999776482582)*1.0379999876022339`,
then one float store. Facing uses the literal0.02454369328916073 constant;
owner order isRz*Rx*Ry*T without a default Z scale. Software view subtracts the
integer camera, rotatesZ by0x3f490fdb andX by0xc0060a92, then translates+1925Z.
411640/56d5f0 yield projection diagonal0x3fb6db6e. Their resulting coefficients
are included explicitly in all three backend helper/quad vertex shaders, not
estimated from screenshot geometry. CameraZ is an explicit per-draw input;
these isolated fixtures declare0. The projection is orthographic with source
forward depth2750. CameraXY and viewport center use the existing reconstruction
inputs; no global camera or mesh-projection switch is changed.

`--vfx-native-domain` makes only the exact three buff preview adapters supply an
independent native owner override to their existing base matrix producers and
use the original MODELZ particles directly. The helper/quad draw carries its
native projection flag. The default false path retains common-world geometry.
Native matrices also preserve the correct normal domain, with the helper normal
bridge multiplier left1. The new metadata shares unused helper uniform slots;
quad metadata expands the strip vertex from16 to18 floats. Strip defaults and
all other draws remain common-domain. Capture validation requires the adapter's
`[native-domain] enabled=1 camera_z=0` log when the option is requested.

## Causal controls

`native_projection_contract.py` executes30 constant authored quad cases with
six facings, moving XYZ, negative Z, cameraZ and nonzero ownerRX/RY. Independently
compiled C++ owner/view matrices agree with executed native matrices to
5.98e-8 /4.375e-5 respectively; maximum projected point residual is0.00133pixels
at world10000. All30 original normal-lit raster images are identical. The
negative control reinstates the common owner's1.5 modelZ and raw ownerZ:
all30 images change, with up to5342 differing pixels. It does not fit any offset.
The focused regression repeats identity, moving and tilted cases against the
actual executable, including that visible negative control.

The compiled Speed and Quicksilver particle probes also exercise the new native
mode directly. Both retain2221 live packets,2109 admitted draws,112 scale-gated
rejects and five exact original software pixel/depth pairs. Max corner errors
are1.9571e-6 and1.9559e-6. This branch uses no old raw-world-to-MODELZ vertex or
centre comparison adapter; actual production vertices are already native-domain.

## Actual Metal and map results

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/`.

- `native-projector-negative/manifest.json`:30 controls and negative controls,
  SHA256 `e05e2b1bfc67c34adf1ea88008661e678fe519b5972ad30455cf62c857d513f8`.
- `speed-native-domain-final/` and `quicksilver-native-domain-final/`: direct
  compiled native-domain packet proofs described above.
- `domain-metal/`:six60-frame films, common/native modes for each of three buffs.
  All use the same actual Metal binary
  `c84b20b2aa71efc9eaa9d71f0b81b4cd274043c17c3a40f0d3998ce6ac3e51e7`.
  Source ambient38/255, directional1, no point lights, quality0, origin0/face0,
  seed1, warmup1 and24Hz are unchanged. Independent native and both Metal modes
  have matching random draw counts at all18 sampled capture points.
- `domain-map/`:fresh ordinary FontRuntimeLab runs on that same binary. All
  three6-row command timelines pass;58frames and41 distinct active frames each;
  movement is visible and13 final frames exactly restore the initial floor.
  This verifies the default map route, not use of native projection on a map.
- `domain-visual-final2/manifest.json`:18 original/common/native comparisons and
  the3 map lifecycle checks, SHA256
  `ec0367f7d6155c3c10ba6da2ca2756006c62382aac873d16de5d074cb7c32e0a`.
  `domain-capture005.png`, `015.png`, `029.png` show identical fixed crops at
  nearest2x, ordered original software, Metal common, Metal native-domain.

This ablation includes alpha lane's70c0aa3 source raster corrections in BOTH
Metal branches. Across18 samples, total absolute RGB error falls from675975
(common) to513731 (native-domain), about24%. At capture15, Fmastery's apex moves
from y131 to native y133. Quicksilver's vertical extent becomes39pixels versus
native39. Its earlier oversized78pixel halo width already falls to64 under the
shared raster correction, versus native65; that large horizontal difference
was not attributable to the roughly1% projector X coefficient. Speed retains
thin-tail and color differences. These measurements distinguish the two changes
without claiming that every residual has the same cause.

No strict GPU pixel parity is claimed. Original scan conversion, texture/color
interpolation and destination-channel arithmetic still differ. Natural callers,
casters, audio, point-light selection and broader device scenes remain outside
this fixed configuration. The native domain is an explicit diagnostic policy,
not automatic gameplay camera migration.

Validation: full Metal build,6 actual isolated captures,3 map captures, the
30-case native control run plus focused3-case regression,4 existing buff
matrix/packet tests, actual helper-uniform packing test (including native
flag/cameraZ and software/modern alpha selection), and strip/quad capacity
negative control. Build initially exposed the older checkout's missing
`UsesRetailSoftwareMeshLighting` accessor from00008ec; its exact existing
implementation is included for the70c0aa3 dependency. The helper uniform fixture
was updated for that accessor and mode3's software route.

## Root appearance approval, October 10

After independently viewing the original/common/native-domain comparisons,
root approved the overall form, size and color family of Speed (`0xad92bd36`),
Quicksilver (`0xad92bd35`) and Fmastery (`0xb0e024df`) for the explicit native-domain
isolated fixture. This records one scoped appearance pass per shipped type.
It does not accept common-domain projection residuals, gameplay camera changes,
strict framebuffer equality or all remaining effect gates.

`hard-astra/buff-root-approved/manifest.json` freezes that decision at SHA256
`34478260c150e24c91c82171f02d8a717bc26cd643988902cf9589a97c71a520`.
Separate `Speed.json`, `Quicksilver.json` and `Fmastery.json` records each retain
six native/Metal image pairs and hashes, capture/binary provenance, exact source
light logs and matching RNG draw counts. All18 pairs were hash-checked again.
The original review manifest remains unchanged at `ec0367f7…` above. The declared
source policy is `EXPLICIT_NATIVE_DOMAIN_ISOLATED_PREVIEW`: ambient38/255,
directional1, no point lights, quality0, owner/facing/cameraZ0, seed1, warmup1,
24Hz. No position, scale, color or timing fit was introduced. Remaining RGB,
raster/interpolation and destination framebuffer differences remain explicit.
