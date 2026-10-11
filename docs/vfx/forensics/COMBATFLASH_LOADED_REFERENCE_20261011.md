# CombatFlash loaded reference

`tools/retail_runtime/combatflash_loaded_visual_reference.py` constructs the
actual `combatflash` owner and loaded default animator in the source-matched
startup VM. It exercises null-spell state0/start1 through64 ticks, original
NextFrame, whole owner Pulse, controller Pulse, default Animate, complete scene
render, original normal producer and software raster. The declared scene has
camera0, a640x480 RAM target, source ambient38/directional1 and24Hz frame count.
Only material-handle COM get/set behavior and the isolated framebuffer are
supplied; no owner/controller, geometry, color, RNG or raster handler is replaced.

The software fixture is established before original object/imagery/animator
creation, and the render scheduler calls actual Animate40e460 with draw0.
CreateAnimator already creates both blendcont and PartSys controllers. The
468-slot PartSys emits10 then15 live particles; the recorded draws have9
normal-vertex submissions plus the prelit particle submissions. Seventeen
frames have visible output; original owner completion makes the tail blank.
The extra normal submissions still need comparison with the port's declared
base-mesh route. This reference alone grants no visual acceptance.

Retained controls under the local evidence directory include an invalid run
that skipped generic owner Pulse4708e0 (which also skipped the controller
update), an explicit RefreshControllers experiment, and a whole-owner run
created before the software scene with Animate draw1. These controls were
misconfigured for the intended full reference. The latter two setup differences
have not been independently isolated, so no single-cause graphics claim follows.
Original disassembly confirms4708e0 dispatches animator Pulse slot0x2c and
40e2e0 dispatches each controller Pulse. Instrumented entry observations confirm
the corrected whole owner path invokes all three methods.

The positive report is
`/Users/benjamincooley/RevenantRetailLab/research/vfx-combatflash-loaded-20261011/scene-before-resource/manifest.json`.
The first current port film is retained under `metal/`: it exits cleanly but
fails the native-domain validation because the old CombatFlash adapter does
not confirm or forward that opt-in. Its images are a diagnostic rather than a
passing A/B result. The next work is an explicit source-backed native-domain
adapter, particle/base-mesh packet comparison, and fresh same-tick image review.
State0/start1, other17 states, natural combat selection/audio/device and exact
pixels remain separate gates; CombatFlash remains one of28 unreviewed rows.

## Explicit native-policy follow-up

The adapter now forwards the requested native-domain option to the existing
particle and helper submission policies and confirms it in the capture log.
Only exact CombatFlash/state0/profile native submissions enable RGB565
additive modulation and native RGB5 prelit channels. Ordinary submissions and
other effect identities keep their existing policy. The controlled face0 owner
uses the original FIX_Z translation; no shader or fitted texture/geometry is
added. The integrated build and actual extracted default SubmitPartSys compile
check pass.

`native-policy-retry/manifest.json` in the local evidence directory records
64 actual GPU frames, proper input confirmation and clean exit0. Same-tick
particle color is closer to the original, but the base flare has a visible
vertical-shape/brightness residual. The additional original normal submissions
and complete base-matrix/projection comparison are still unresolved. This
follow-up is diagnostic progress, not an appearance pass. The ordinary path
is separately checked by `ordinary-policy-control/manifest.json`: all64 PNGs
are byte-identical to the pre-change ignored-flag control, with clean exit0.
The application rebuilt after water integration; no frozen copy of the earlier
CombatFlash executable was created. Its captured manifest still records the
actual binary hash and files. Base-matrix/extra-object diagnosis remains open.

## Original key/matrix diagnosis and scoped correction

The native probe now records actual object IDs, scene states, original
GetAniKey return values, CalcObjectMatrix results and transformed lighting
channels. Read-only hooks preserve every original call. Its sampled-output
option reduces PNG encoding only; all requested ticks still render.

`combatflash_base_matrix_probe.py` compiles the actual production decoder,
GetAniKey interpolation and CombatFlash base-matrix body against immutable
first-state Impact keys. Original owner previous-state/frame0 and five-frame
transition inputs are declared. Across18 live matrices/288 channels, the
inclusive branch matches within2.412e-7; the unadmitted half-open control
differs by more than1 matrix unit. Early frames2/3 also agree once the actual
five-frame transition is included, without a frame shift or fitted scale.

Production GetAniKey now requests inclusive decode only for admitted
CombatFlash/state0/object2, including its previous-state0 sample. Other
objects/profiles retain their prior policy. The application rebuild and
independent matrix check pass. The existing default static native/source/
shared-raster regression passes in23.960seconds after its extraction stub
gains the unchanged false CombatFlash-profile predicate.

`inclusive-key-fix/manifest.json` records a fresh64-frame real GPU run with
clean exit. This fixes authored flare timing but leaves a separate visible
shape/brightness/sampling residual. The original tick2 lighting observation
and independently evaluated source normal produce identical RGB5(21,21,21).
Replaying each observed normal draw through the original software raster
shows object2 contributes1070 pixels and the other eight contributezero at
tick2. This is a bounded isolation result, not proof of all extra-object
frames or complete color/raster equality. All these reports remain private
under `vfx-combatflash-loaded-20261011/`; no appearance approval is added.

The independent `combatflash_projection_probe.py` bounds the production helper
projection coefficients using the original observed local vertices and matrices.
For ticks1..3/object2, all12 vertices/36 XYZ channels agree with original
post-transform output to maximum0.000105629 after normalizing native invertedY.
The explicit camera0/center320240/owner0 policy is retained. This is a shader
formula contract, not actual GPU vertex readback or a raster/UV/color pass.
Private `projection-contract/manifest.json` pins the trace, renderer and probe.
No geometry/projection fit or new visual credit follows from this result.
