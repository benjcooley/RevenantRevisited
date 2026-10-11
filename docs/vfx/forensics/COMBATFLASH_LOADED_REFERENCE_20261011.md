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
