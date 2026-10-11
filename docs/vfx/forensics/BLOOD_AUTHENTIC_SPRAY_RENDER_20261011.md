# Blood: complete registered controller and public spray

`Blood_AUTHORED` ports the exact shipped EFFECT `0xddc4042e`, `Misc/Blood.I3D` (SHA `77e445a5c5d34d32e4948f596d048b15d82c0107bdcf1938642e6db805c3a119`). The genuine loaded registry resolves owner vtable `5acb0c`, public SetParams `4f9490`, animator factory `4f9550`/vtable `5acd18`, Initialize `4f18d0`, Animate `4f1c60`, PreRender `40e460`, Render `4f1de0`. The previous candidate owner vtable `5acaa8` is superseded. This asset is version1, 32 vertices, 16 triangles, eight authored quads, sixteen looping header frames and two one-frame textures: RGB565 and ARGB4444. The different `Magic/Blood.I3D` is not admitted.

The original fixture genuinely boots the game, constructs an Acolyte (Health 102, Radius 16, face 96), then constructs Blood and executes its public SetParams interface. Its supplied arguments are the actual `TCharacter::EffectBurst4c85d0` literals: height 40, face+128→224, vangle 3, spreads 5/5 and an original RandomRange count 1. Actual EffectBurst reaches the real map factory but insertion returns−1 in the cached ShowScreen-only world, so that complete map caller is open. Only generic TObjectInstancePulse `4708e0` is explicitly isolated as the map/world callback; actor/stat/bone/damage methods and the complete original Blood constructor/initializer/custom controller/render are not replaced. The material-handle Get/SetLightState key1 device boundary preserves actual booted get/set state, without a material/color override.

The final declared reference uses genuine actor0,0,16, effect0,0,56, common CRT Init seed 2 through actual srand immediately before animator initialization, detail 4, public MapPane ambient input32/white with original GammaLevel 4→raw 52, renderer ambient38 and directional1, viewport640×480 and center320,320. The earlier translated3185,3185 reference independently verifies the same controller; the new common origin is a declared factory/camera input, not fitted alignment. Source spawn uses the same public MapPane ambient setter and unchanged GammaLevel 4 INI.

Retail differs substantially from the snapshot. It chooses a random pool cap from min(14,3×detail), creates primary/secondary slots with additional heading/vertical trigonometric factors, begins scale0.5, and renders the delayed secondary particles before movement begins. All30 slots retain exact float, used/object/stage/age/delay fields. The fly→splat→shrink trajectory uses corroborated gravity0.37/drag0.95; the configured positive reference kills at60. Native prelit packing doubles raw map ambient, multiplies by255 and uses unbounded integer OR/shifts. With raw 52 this produces `ffffff98` (RGBA5 31,31,19,31). The source derives this from the original operations; no sampled native color is injected.

Both original passes are preserved in particle order: objects 4–7/ARGB4444 requested SRC_ALPHA/INV_SRC_ALPHA, then objects 0–3/RGB565 requested ONE/ONE. Original software's ARGB4444 kernel keeps alpha-over; the existing source helper supports this. No shader or renderer changes are needed. The new strict matcher hashes the complete68716-byte body, including all legacy keys, geometry, materials, descriptors and raw texels; altered or foreign bodies fail closed.

Evidence: `/Users/benjamincooley/RevenantRetailLab/research/vfx-next-blood-20261011/`.

- `native-final90/manifest.json`: full original Init and90 owner/custom Animate/PreRender/Render lifecycle rows; selected whole software rasters, explicit device handoff on other live frames; kill 60 and dead-no-calls tail through90. The prior full-raster60-row replay repeats state/RNG and draw matrices exactly.
- `state-final90/manifest.json`: actual compiled production core, ASan/UBSan; 32,760 state fields exact0, 11,104 local matrix fields max<5e-8, 2,776 prelit colors exact,91 RNG states exact. Native poses/states/colors are not source inputs.
- `profile-first/manifest.json`: actual whole-body source matcher,4 positive/2156 negative cases, ASan/UBSan.
- `owner-first/manifest.json`: extracted actual production submit code, all 32 authored vertices across every emitted draw and origin/translated owners; 5,552 geometry/1,388 draw checks, including exact blend/cull/lighting inputs, ASan/UBSan.
- `metal-texture-bin-qualified/manifest.json`: actual90-frame Metal capture,58 visible/32 distinct frames, clean exit. `review-final/{whole.png,fixed-crop.png,manifest.json}` pins unchanged phases 1/3/6/16/30/40/50/57/58/59/60/90; same full viewport and actual-size crop260,230,420,390.
- Empty controls are retained: the original seed 1-before-constructor reference legitimately initializes no slots; the final explicit Init seed 0 also initializes no slots and kills 1. These are separate declared RNG placements, not interchangeable seed claims.

The initial first90-frame Metal capture and three-frame diagnosis exited cleanly but produced no pixels because the two texture bins were reversed; their manifests remain failed and superseded. A first matrix checker incorrectly omitted delayed slots from source Render; corrected source includes them and passes the complete original packet sequence. The early black native capture lacked actual map ambient despite renderer lighting and is retained as an explicit configuration gate.

Independent appearance approval and integrated gallery are pending. Visible bounds generally differ0–1px; at near-zero tick 58 native is blank while Metal retains one pixel, with kill 60/blank 90 agreeing. Preserve this sampling/coverage residual, hardware/color-quantization, alternate lights, complete map insertion/natural combat/caster/damage/audio and full acceptance as open. No fitted geometry, phase, colors, source pose inputs or fake actor fields are used.

Reproduce with the retail lab Python and `PYTHONPATH=tools/retail_asm:tools/retail_runtime`:

```sh
python -m unittest tools/retail_runtime/test_blood_native.py
python tools/retail_runtime/blood_native_actor_reference.py --ticks 90 --seed 2 --isolate-map-pulse --origin-zero --selected-raster --output NEW_NATIVE_DIRECTORY
python tools/retail_runtime/port_capture.py --scenario /Users/benjamincooley/RevenantRetailLab/research/vfx-next-blood-20261011/scenario.json --output NEW_METAL_DIRECTORY
```

Source/evidence checkpoint freeze SHA `f0d9153d4ee5cd04215d7214984ac4f1c8d71ead5481716b5bda64efeea78f8b`; actual capture binary `65b3ddbd6b7ceb840df417e950290716cd4dd10d08e4d56340c60eb327866028`.

Root independently inspected all12 phases in whole frames and fixed nearest
detail crops and verified24 paired image hashes. The integrated binary repeats
all90 numbered PNGs byte-identically with clean exit0; its three focused native
tests pass in65.417seconds. Fixed public-spray appearance receives a scoped
pass with the recorded0–1px/nearzero58 residual. A live gallery/global-RNG route
is a separate pending follow-up; the fixed Init seed2 fixture does not prove it.

Approval:
`recon/retail_asm/runtime/effects/current-blood-spray-20261011/root-approval.json`,
SHA256 `5c7b353ffa6f3e75cb32b34ad805d66cfdcb3dd8bc059cd47813ebae45706396`.
Root capture: `/Users/benjamincooley/RevenantRetailLab/research/vfx-blood-root-20261011/film/manifest.json`.
The reviewed count advances to150/176;26 remain. Natural gameplay, alternate
lighting, audio/device and exact-pixel gates stay open.
