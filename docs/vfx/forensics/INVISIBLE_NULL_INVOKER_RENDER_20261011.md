# Invisible: complete null-invoker retail controller

The exact `invisible` EFFECT `0xad92bc16`, `Magic/Invisible.I3D`, now has the `Invisible_AUTHORED` preview. This is the complete executed retail null-invoker appearance, with six rings and 70 particles. The snapshot's 30-particle animator is not the reference. The owner lifetime is corrected from 300 to the executed 500 ticks, and its original `OF_NONMAP` initialization is restored. Natural character fade/invisibility gameplay, audio, normal map rendering, hardware rendering and exact pixel equality remain open.

The asset SHA is `dd2c7ebe6110b07b404ad4afa0ea6938ce9538448c376336931caabd6be43b52`. The original all 93 registry selects animator factory `500830`, instance `66cf38`, vtable `5aeae0`, Initialize `4fd970`, Animate `4fdae0`, Render `4fe040`. The genuine owner Initialize `4fd890` and Pulse `4fd8c0` execute; the upstream generic object provider, world/script callbacks, unavailable sound lookup, resources/COM and device handoff are explicit fixture boundaries. The complete animator factory allocates `0x814` zeroed bytes. No fake actor or synthetic controller is supplied.

The asset has two modern version2 roots, 16 vertices and 14 triangles, an ARGB4444 ring texture and RGB565 particle texture, and no tags. Original initialization copies the full 12-vertex ring to LVERTEX, then initializes all 70 particles from MSVC seed 1. Their delayed rise, respawn and scale decay use the original random sequence. Rings converge at tick 35, the WAIT interval is blank by tick 100, SHOW begins after owner life 430, and the original owner kills at 500. The preliminary inference of 50 particles confused the height threshold with count; all actual initialization/update/render loops use literal `0x46` (70). Earlier wording is retained as superseded, with its executed trace unchanged.

The original null-invoker placement quirk is preserved: ring POS3 includes the owner position and then multiplies the owner root; particle ABSPOS bypasses that root. At owner 100,200,30, a first ring therefore lands at 200,400,44.11 while a particle lands at 95,207,23.48. This is not a claim that the untranslated null-caller visual establishes natural character placement.

Evidence is under `/Users/benjamincooley/RevenantRetailLab/research/vfx-next-invisible-20261011/`:

- `native-life-final-v2/manifest.json`: all 501 original owner/Animate/Render calls, selected whole software rasters at 16 declared phases. `native-repeat.json` confirms two fresh501-row traces, 5,112 draw packets and12 common whole rasters repeat. Resource pointers change with viewport allocation; topology, formats, states and matrices do not. The common rasters agree exactly under the declared viewport-origin change, without fitting.
- `state-final/manifest.json`: independently compiled actual production core passes 221,884 state/RNG/cache fields and81,792 matrix fields, maximum errors below 5e-8, ASan/UBSan.
- `profile-material-qualified/manifest.json`: exact own path/state/header/root/rawkeys/fullvertices/fullfaces/material/texture-descriptor admission, 4 positives and271 mutation rejects, ASan/UBSan. Raw texture payloads are pinned by the complete asset SHA. The failed initial material offset and interrupted replay remain retained.
- `owner-qualified/manifest.json`: actual extracted submission branch passes 6,112 whole-geometry checks and21,120 original worldmatrix fields across origin/translated owners, maximum error below 5e-8. Source-alpha unlit rings and normal-lit additive particles retain their separate original state policies.
- `metal-first/manifest.json`: actual501-frame Metal movie, 108 visible/109 distinct frames, clean exit 95.286 s; binary `cf1825160b82a16124c36d6c9a01c01b80f9c19dc794bd9f5fa4c168035f91f0`.
- `review-final/{whole.png,fixed-crop.png,manifest.json}`: unchanged pairs 1/6/16/30/35/40/100/430/431/432/459/461/480/499/500/501. Entire geometry is inside640×480, camera320,240, owner 0,0,0, Face0, source ambient 32 white/native 38 plus directional1, 24 Hz, seed 1, warmup 0. The actual-size crop 240,130,400,290 is identical for both sides. The maximum visible-bound residual is1px; no pose, geometry, lighting, phase or camera fit.
- `tests-final.log`: three focused tests pass 70.123 s. `build-final.log`: private build passes. `checkpoint-freeze.json` pins the source and all evidence; its SHA is `6e35cec58072055df2d93b8b1642324a4813a5ead2bc34f2af0cbb496c81d0ab` (verify the recorded artifact before integration).

Independent root appearance approval and the integrated exact gallery route are pending at this source checkpoint. Sampling/edge/color quantization, natural actor fade/status/audio, the full general gameplay object constructor and device/other-lighting gates remain open. Full acceptance stays false.

Reproduce the focused checks with the retail lab Python and `PYTHONPATH=tools/retail_asm:tools/retail_runtime`:

```sh
python -m unittest tools/retail_runtime/test_invisible.py
python tools/retail_runtime/invisible_native_preflight.py --ticks 501 --output NEW_NATIVE_DIRECTORY
python tools/retail_runtime/port_capture.py --scenario /Users/benjamincooley/RevenantRetailLab/research/vfx-next-invisible-20261011/scenario.json --output NEW_METAL_DIRECTORY
```

Root inspected the complete sixteen-phase whole/detail comparison and verified all 32 paired image hashes. Integrated build and three tests pass; 51 same-phase Metal PNGs through tick501 are byte-identical to the full lane movie. A longer 602-tick gallery records the actual second factory creation after the500-tick lifetime. Scoped origin-zero appearance is approved; translated null-invoker map placement and the other stated limits remain open. Root approval SHA256: `c8203463753496b88ccb0ae30da3d7bd1f7ba6eea494d22852c496364ea49c28`.
