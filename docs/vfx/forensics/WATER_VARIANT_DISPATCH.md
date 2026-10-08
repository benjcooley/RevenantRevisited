# WaterFlft / WaterFrt / WaterClft / WaterCrt / RiverFall dispatch audit

2026-10-05. **Dispatch, authored controller recovery and bounded runtime verification established; retail visual acceptance remains open for all five types.** These are generic EFFECT objects using the default 3D animator and authored tag controllers. They are not aliases for the literal `Waterfall` bespoke animator. WFall/WCap motion requires retail `partsys`; RiverFall motion requires `scrolltex`. The port now implements the recovered four-water `partsys` subset and RiverFall `scrolltex` through actual default-animator runtime dispatch. Synthetic-floor runtime success does not establish native blend/depth or full-map visual parity.

## Exact shipped identities

The actual `class.def` inside `data/imagery.rvi` contains:

```text
"WaterFlft" "misc\WFall.i3d"     0xd0c0f036 {0,3} {}
"WaterFrt"  "misc\WFall2.i3d"    0xd0c0f037 {0,3} {}
"WaterClft" "misc\WCap.i3d"      0xd0c0f038 {0,3} {}
"WaterCrt"  "misc\Wcap2.i3d"     0xd0c0f039 {0,3} {}
"RiverFall" "misc\Riverfall.i3d" 0xd0c0f03a {1,3} {}
```

All belong to EFFECT, class 25. RiverFall's ID is **0xd0c0f03a**. The separate literal `Waterfall`, ID **0xa907dabf**, uses `misc\Water.i3d` and its own particle animator. Its 100-drop behavior does not establish behavior for any of these five types. See [literal Waterfall runtime audit](H02_WATERFALL_RUNTIME.md).

Exact extracted asset SHA-256:

- WaterFlft: `306a42c1c57a89a568ee67173ffa0de8f1cc6c51c450a10b36e13074e5abfd5a`.
- WaterFrt: `e88b8439f16438a23c67642f0f30e38ad8e1a847226e00b167e52c6ca450e9d8`.
- WaterClft: `583416fb590936cc4f1e9b4d0e04ad13c177ea2d1d4ad5f57d5e73353507ca2e`.
- WaterCrt: `e5893cb78c8baa7a8dfffdda8509755bce9a0757e711154d3ad9cbda5e87d87e`.
- RiverFall: `3ca23b872888cb078bdafd6b7f6fb36c1e42a40a6b2da4fa30b7bba42dc12a32`.

## Actual object and animator dispatch

The original snapshot registers generic `effect` → `TEffect` in [legacy/effect.cpp](../../../legacy/effect.cpp). Its [3D imagery dispatch](../../../legacy/3dimage.cpp) searches the exact type name, then class name, then defaults to `T3DAnimator`; `GetBuilder` uses case-insensitive exact equality. Default animator initialization calls `RefreshControllers`, which resolves imagery tag names through `T3DControllerBuilder`. Controllers receive Pulse and Render callbacks; the latter run before ordinary mesh submission.

The retail executable independently establishes this path:

- All 92 direct named animator registrations and 121 object-builder registrations were decoded. None registers WaterFlft, WaterFrt, WaterClft, WaterCrt or RiverFall. There is no generic EFFECT animator override. The five names also have zero case-insensitive literal occurrences in the executable.
- `NewObjectAnimator`, **0x40cf50..0x40d0d1**, and builder lookup **0x40dca0** perform type/class lookup with the default pointer at **0x5e8508**. The equipment-specific fallback does not apply to class EFFECT.
- Generic effect startup **0x4de75a**, constructor **0x4de770**, uses instance vtable **0x5a85ac**. Retail `TEffect::Pulse`, **0x4de800..0x4deee5**, does **not** contain the snapshot's unconditional `SetFrame(0)`. Its tail permits looping non-spell effects to persist and clears command-done.
- Retail registers `scrolltex` at **0x401610**, `animtex` at **0x401a60**, and `partsys` at **0x403300**. `partsys` instance vtable **0x5a3544** resolves Initialize **0x403320**, Close **0x4036a0**, Pulse **0x403760**, Render **0x404270**, ParseItem **0x4042e0**, and destructor **0x405ec0**. Registration string is **0x5c5c8c**; builder is **0x5e82f8**.

This combines positive registry/dispatch evidence with the absence of a named override; name absence alone is not the dispatch proof. Examined dispatch, Pulse and controller code ranges are byte-identical between the original retail EXE and the compatibility EXE. Full hashes, assembly ranges and decoded registrations are retained in the [offline audit](</Users/benjamincooley/RevenantRetailLab/research/water-variant-dispatch-20261004/audit.json>) and [controller evidence](</Users/benjamincooley/RevenantRetailLab/research/water-variant-dispatch-20261004/controller-binary.json>).

## What actually animates

All five have a CGSR wrapper, imagery ID 1, version-3 I3D body, flags **0xdc**, one state, looping/NOMOTION flags **0x2001**, and compressed constant object transforms. Every object contains only one value per transform code; there are no changing packed transform keys. Each texture has **one frame**. Consequently the state header's 15 or 100 frames must not be described as varying mesh-key animation or a texture-frame sequence.

The WFall/WCap resources each contain **4 vertices / 2 faces**, two materials, seven objects, one 64×64 RGB565 texture, and state `start` with **15 frames**. `#particle01` is the four-vertex particle prototype; `bounce` through `bounce05` are six zero-vertex authored emitters. Their transforms and rotations differ across left/right/fall/cap assets. These distinctions must survive particle reconstruction.

WaterFlft and WaterFrt have the following state-0/frame-5 `partsys` tag:

```text
obj=(bounce,bounce01,bounce02,bounce03,bounce04,bounce05),particle=#particle01,pps=45,initialvelocity=3:5,lifespan=25:75,localrotation=[0:(0,0,0),100:(0,60,0)],friction=0,gravity=0.5,color=[0:(106,126,155),80:(32,85,55),100:(0,0,0)],relvel=0,spread=30,azimuth=30,scale=[0:0,20:4,100:0],bounce=-10:-10
```

WaterClft and WaterCrt instead have state-0/frame-5:

```text
obj=(bounce,bounce01,bounce02,bounce03,bounce04,bounce05),particle=#particle01,pps=25,initialvelocity=7:8,lifespan=25:50,localrotation=[0:(0,0,0),100:(0,0,0)],friction=0,gravity=0,color=[0:(32,85,55),100:(106,126,155)],relvel=0,spread=10,azimuth=0,scale=[0:0,80:1.5,100:0],bounce=-10:-10
```

The fall emitters are at local Z70; cap emitters are at approximately Z2.711. The prototype has its own authored transform. These are controller-driven particles; displaying the prototype once or preserving its owner's 15-frame loop does not reproduce the effect. `partsys` was added after the available source snapshot. The decompiled `cls_0x5a3544` material is partial, so complete initialization and simulation must be recovered from the saved executable ranges rather than inferred from a neighboring custom animator. The [geyser audit](W03_GEYSER_RUNTIME.md) shares this controller recovery work.

RiverFall is a **14-vertex / 12-face** authored mesh with one material, one 128×128 ARGB4444 texture, one object `water`, and state `waterflow` with **100 frames**. It is not a single billboard quad. State-0/frame-0 tag:

```text
scrolltex: obj=water,du=0,dv=0.01
```

The complete snapshot [scrolltex source](../../../legacy/3dcont.cpp) copies original UVs into per-instance vertices. Once the tag frame is reached, Render sets original UV plus `du/dv * PlayScreen.FrameCount()`. It does not repeatedly add a delta to the previous render's UV. This preserves synchronization and avoids render-frequency-dependent speed. RiverFall local geometry bounds are approximately X −58.521..53.426, Y −94.25..94.25, Z −35.175..−11.740; the prepared command fixture raises the owner to Z128 above floor Z0.

## Original port gap and current scoped implementation

The port reads I3D tags in [src/3dimage.cpp](../../../src/3dimage.cpp), but the baseline removed the default animator's controller initialization/Pulse/Render machinery. The original snapshot's generic [TEffect::Pulse](../../../legacy/effect.cpp) also resets frame zero, unlike the supplied retail binary. The exact-five-ID Pulse exception is now verified against retained original saved frames, including wrap; other EFFECT behavior remains outside that correction.

The standalone aliases around [src/vfxtest.cpp](../../../src/vfxtest.cpp) previously sent all five names through `WaterFallBespokeVariantSpawn<asset>`, borrowing the literal Waterfall mechanism. Those captures cannot serve as evidence of real gameplay dispatch. Use the actual saved objects or real `ADDAT` creation below. These aliases remain invalid retail-reference candidates; the supported path is the recovered authored controller. Unknown grammar, unsupported topology/material modes, morph/all-state tags or unrecovered companion controllers remain explicitly unsupported.

The recovered implementation uses immutable [parser metadata](../../../src/partsysdefinition.h), literal [particle simulation](../../../src/authoredpartsys.cpp), and [default animator ownership/submission](../../../src/3dimage.cpp) through [map rendering](../../../src/maprenderer.cpp). It resolves the six emitter names in authored order and the actual `#particle01` four-vertex/two-face prototype; it suppresses ordinary prototype mesh drawing and submits the original vertices/UVs with sampled particle pose and color. A controller belongs to one animator instance, rebuilds on state change, and is released with its owner. Simulation advances on the normal 24Hz map Pulse; render samples rotation without advancing simulation.

**The stored tag frame 5 is metadata, not an activation delay for `partsys`.** Retail Initialize/RefreshControllers creates the state's controllers regardless of tag frame. Pulse **0x403771** loads the animator's raw current frame, passing it to interpolation **0x401390** at **0x4038fb** without subtracting TagFrame. Render **0x404270** checks state, not a frame-5 threshold. This differs from the recovered `scrolltex` controller's own tag-frame gating; do not apply scrolltex scheduling to particles.

Parser defaults/range/curve selection are recovered from ParseItem **0x4042e0** and helpers, with [4,620 parser checks](</Users/benjamincooley/RevenantRetailLab/research/partsys-parser/validation.json>) including byte-verified original tags/assets and malformed/unsupported cases. The scoped runtime preserves PPS accumulation, lifespan/capacity, inclusive RNG order, ordered emitters, owner-face transforms, emitter position subtraction, particle age curves, gravity/friction/bounce and the original render-time rotation draws. Fall capacity is **140**, cap capacity **52**, from authored lifespan maximum × PPS × original float `1/24`, truncated. Quality is logged as 0 in these fixtures, retaining retail's unscaled default branch. Blend flag **16** selects ONE/ONE; current port depth/blend/device equivalence remains an acceptance gate.

The independent [matrix oracle](</Users/benjamincooley/RevenantRetailLab/research/partsys-parser/transform-validation.json>) executes original Pulse **0x403760**, particle Initialize **0x4031c0**, Update **0x402a20** and x87 Transform **0x43ad80**, isolating only external pose/ground/CRT inputs. It passed **352,800 comparisons** and covers identity, translation, nonuniform scale, quarter-turn rotation, matrix translation differing from stored emitter position, zero scale and two ordered emitters across four shapes/five owner faces/30 ticks. Logical/RNG comparisons are exact; floating comparisons allow `max(3e-5,8 binary32 ULPs)` and do not establish bit-exact arithmetic. The [initial stricter result](</Users/benjamincooley/RevenantRetailLab/research/partsys-parser/transform-validation.strict-absolute.json>) retains ten accumulated X-position differences up to 3.812e-5 in the deliberately mismatched translation fixture rather than concealing them.

Next bounded work is native device/reference recovery and paired captures in the original saved map settings. Keep unsupported parser/controller paths closed, and correct misleading standalone aliases before using them as evidence. Do not replace six authored emitters with literal Waterfall's 100-drop implementation.

## Saved placements and owned runtime fixtures

The compressed Ahkuilon archive scan covered **4,896 Map sectors**, with no parse errors. Versions 10/12/13/14/15 use genuine object serialization and exact end-of-file checks. The 27 version-1 sectors are not claimed as parsed: their raw bytes contain none of the five exact type IDs. Found saved EFFECT records: **WaterFlft 9, WaterFrt 8, WaterClft 2, WaterCrt 11, RiverFall 0**. All 30 preserve the animate/pulse flags. Full identities, object-body hashes, transforms, nearby terrain/light records and area candidates are in the audit JSON.

Four fixtures copy the **unchanged original serialized object records** into owned modules with nine synthetic Dunffff ground tiles. Position, type, MapIndex, flags, state, saved frame and framerate are retained. The floor is deliberately 256 units below the owner; the camera centers at the original owner height. Fixture lighting is ambient32 white, not the source area's lighting. These are diagnostic saved-object fixtures, not the full original setting.

- `waterflft`: EnvWaterFlftSavedLab; Ahkuilon `Map/48_18_19.dat`, slot20; position **(19299,20013,124)**; MapIndex **231055555**; frame9/rate1; flags0004c009.
- `waterfrt`: EnvWaterFrtSavedLab; `Map/44_12_8.dat`, slot9; **(13256,9114,341)**; MapIndex **1698617881**; frame2/rate1; flags0004c001.
- `waterclft`: EnvWaterClftSavedLab; `Map/48_18_19.dat`, slot15; **(19074,19978,148)**; MapIndex **224724730**; frame9/rate1; flags0004c009.
- `watercrt`: EnvWaterCrtSavedLab; `Map/44_12_8.dat`, slot12; **(13228,9128,198)**; MapIndex **1711762412**; frame8/rate1; flags0004c001.

For subsequent actual-setting captures, Map48 **The Pit** has area ambient26 RGB(150,150,250), with Clft and Flft in the same sector as terrain, lights and MistFog. Map44 **The Underground Pyramid** has ambient24 RGB(150,150,250), with Frt and Crt in the same sector. Use these pairs as the first environmental scenes; retain the authored heights and compare occlusion/ground attachment. The audit retains area candidates rather than silently choosing unrelated ambient or nearest light.

`riverfall-commands` is a separate owned EnvRiverFallCommandLab, level115, camera **(10000,10000,16)**, groundZ0. No RiverFall saved record was found; no saved MapIndex or shipped placement is invented. Its empty effect scene uses a donor only to construct module metadata, then actual runtime commands create RiverFall:

```text
0 | - | addat 10000 10000 1 RiverFall
1 | RiverFall | @expect d0c0f03a 10000 10000 16 animator 1
2 | RiverFall | move 0 0 112
3 | RiverFall | @expect d0c0f03a 10000 10000 128 animator 1
```

Additional observations at ticks12/24/36/72 keep checking the exact created identity, position and one animator. `ADDAT`'s third number is **amount**, not Z; creation inherits cameraZ16 and relative MOVE raises it to128. The type selector resolves the object and then binds its MapIndex. Numeric MapIndex text is not a supported name selector.

The [fixture index](</Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/water-authored-20261004/manifest.json>) provides each isolated `data_root`, environment, working directory and exact pre/post-fix command. [Generator](</Users/benjamincooley/RevenantRetailLab/prepare_authored_water_fixtures.py>) refuses to overwrite existing fixtures. Running it again is unnecessary. **Pass `--scene-command-file`**: this activates normal sector ticking; absent commands, a static scene capture is not a Pulse test. Startup includes a warmup tick before tick0 observation; preserve the saved frame and account for that tick when interpreting new `@frame` logs. Existing commands observe identity without changing the saved objects.

## Baseline captured by root

The pre-fix binary SHA-256 is `e09bd65a1c23e865fec9d5fc2eb41dfa3fc7910a40edfabf7fcf1c590a6d07fb`.

- Each of the four saved fixtures produced **60 frames at 30Hz** and passed all five exact-type/XYZ/animator observations. Each sequence has **one distinct image**. This demonstrates working saved identity/attachment and static baseline output, not an accepted particle effect.
- RiverFall produced **120 frames at 30Hz**, passing all eight creation/MOVE/identity observations. It has **two distinct images**, due to the explicit MOVE; all images after that transition are identical. This is not UV scrolling.
- Old logs have no state/frame fields and `frame_observations` is empty. Do not describe frame0 as an observed value; unconditional reset is established by the old source instead.
- All capture manifests are complete with `accepted:false`. The parent terminated each owned process after output completion because teardown stalled (exit−15); these are complete frame captures, not successful clean-exit checks.

Baseline manifests: [WaterFlft](</Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/water-authored-20261004/waterflft/pre-fix-capture/manifest.json>), [WaterFrt](</Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/water-authored-20261004/waterfrt/pre-fix-capture/manifest.json>), [WaterClft](</Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/water-authored-20261004/waterclft/pre-fix-capture/manifest.json>), [WaterCrt](</Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/water-authored-20261004/watercrt/pre-fix-capture/manifest.json>), [RiverFall](</Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/water-authored-20261004/riverfall-commands/pre-fix-capture/manifest.json>).

In the historical intermediate build with frame preservation alone, advancing or looping `frame` with static pixels was expected for the four still-unimplemented `partsys` variants. That closes only the frame prerequisite. RiverFall needs both frame/lifecycle observations and changing UV pixels after its controller restoration. Neither result substitutes for native retail A/B acceptance.

## Current recovered-controller runtime evidence

The four original saved records were loaded through the actual sector/default animator path on synthetic floor, with binary SHA-256 **`2a48e0da5f53440e06780088d93d996da38690778f0127951b494587b28f15fd`**. Each completed **150 frames**, passed **11 timeline rows**, retained exact saved type/position/MapIndex, and had one controller/six emitters with advancing 24Hz pulses, live particles and submitted quads. Distinct images: **WaterFlft118, WaterFrt117, WaterClft118, WaterCrt118**. These establish actual runtime animation, unlike the retained static pre-controller captures.

Manifests: [WaterFlft](</Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/partsys-water-20261005/waterflft/manifest.json>), [WaterFrt](</Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/partsys-water-20261005/waterfrt/manifest.json>), [WaterClft](</Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/partsys-water-20261005/waterclft/manifest.json>), [WaterCrt](</Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/partsys-water-20261005/watercrt/manifest.json>).

A separate [WaterFlft lifecycle run](</Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/partsys-water-20261005/waterflft-lifecycle/manifest.json>) passed **12 rows**: live controller observations, actual `MOVE 64 0 0`, retained owner identity at its new position, actual DELETE at tick84 and `@absent` at85. Its 150 frames include **82 distinct images** and a static deletion tail. This is saved-owner movement/deletion proof for WaterFlft; it does not independently prove command creation/lifecycle for every other water type. Completed capture processes were terminated after output (exit−15), so no clean-shutdown claim is made.

RiverFall's independently recovered scrolltex/lifecycle evidence is retained in [its dedicated audit](RIVERFALL_SCROLLTEX.md) and [validated Oct4 runtime aggregate](</Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/water-authored-20261004/validated-runtime-results.json>). Its command creation, frame progression, scrolling pixels, MOVE and DELETE prerequisites are checked; no original saved placement or native visual acceptance is invented.

All these manifests retain **`accepted:false`**. The four-water fixtures use synthetic ground and ambient32 white; original map lights, occlusion, ground contact and native software/accelerator fidelity remain separate gates. Parent's bounded native software probe still produced no visible water effect; that failed diagnostic is not a reference or evidence that port rendering matches retail.

## Evidence maintenance and remaining acceptance

[Read-only audit helper](</Users/benjamincooley/RevenantRetailLab/research/audit_water_variant_dispatch.py>) owns the lab research output. Initial provisional mesh-animation language was corrected after decoding all tags and compressed keys. `audit.before-tag-inspection.json` retains exactly the first four fixtures' audit hash **fcc747b5…** for provenance only. `audit.before-pixel-format-correction.json` retains the RiverFall fixture's audit hash **b72c346c…**; current audit metadata correctly accounts for the DDPIXELFORMAT FOURCC field before bit count. Fixture bytes and capture references remain unchanged. The current audit is the behavioral authority; archived metadata must not revive the rejected animation hypothesis.

- [x] Verify exact shipped type IDs, assets, asset hashes and authored tags.
- [x] Establish actual retail generic object/default animator/controller dispatch.
- [x] Scan saved map placements and retain serialized identity evidence.
- [x] Prepare four saved-record fixtures and a fifth real command-creation fixture.
- [x] Retain pre-fix runtime identity/static-image baselines.
- [x] Verify exact-five frame/default-dispatch prerequisites; scoped WaterFlft and RiverFall lifecycle proof is retained.
- [x] Restore and verify RiverFall's authored scrolltex controller in bounded real runtime fixtures.
- [x] Recover the four WFall/WCap authored partsys subset and verify parser/state/transform/runtime prerequisites; device and full visual parity remain open.
- [ ] Capture native software references for each exact variant.
- [ ] Verify full visual A/B in the original shipped map settings, including ambient, colored lights, depth and ground attachment.
- [ ] Accept each exact effect independently in the [burn-down ledger](../EFFECT_BURNDOWN.md).

The initial read-only audit/fixture task changed no production source or guest state. The Oct5 parser/runtime/bridge implementation and parent-owned captures are now recorded separately above, with historical baselines retained. This document does not promote effects to final acceptance; the ledger remains the acceptance authority.

## Final integration and original map context

Root rebuilt the final quality-rate correction and recaptured four saved owners, WaterFlft movement/deletion and both original nine-sector Pit/Pyramid slices. All **100 timeline rows** pass on one frozen binary; sector payloads and original area/ambient are preserved. [Validated aggregate](/Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/partsys-water-20261005/validated-runtime-results.json) retains source/binary/fixture/frame hashes and current regression evidence. The final-source executable oracles are sealed in [source validation](/Users/benjamincooley/RevenantRetailLab/research/partsys/source-validation-manifest.json).

[Pit clip](/Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/partsys-insitu-20261005/pit/port-capture-sealed/port-context.mp4) and [Pyramid clip](/Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/partsys-insitu-20261005/pyramid/port-capture-sealed/port-context.mp4) show the effects in original map surroundings. They retain widgets and current shared rendering limitations; natural-context and visual acceptance gates require matched native evidence. Historical captures above retain their original binary hashes.
