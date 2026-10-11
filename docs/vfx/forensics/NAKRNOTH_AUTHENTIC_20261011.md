# Nakrnoth: loaded caller and terminal color-vector compatibility

Exact effect `Nakrnoth` (`0xad92bd21`, `Magic/Nakrnoth.I3D`) has the default effect owner `5a85ac` and default animator `5a370c`. Its shipped asset SHA256 is `a69506a5499b4af8c1e7fbb2e8b58505ca8e9b6a10417f3e5524f5ef0dfd9c2e`: one nonlooping 775-frame state, 63 objects, five textures, BlendCont at tag frame 17 and a ten-emitter PartSys at 41. The actual loaded constructor and default renderer run without a fabricated boss actor or spell.

The shipped color expression ends `100:(180,180,255]`. `nakrnoth_parser_probe.py` executes original controller initialization and expression evaluation for both that literal and a diagnostic version with the final `)` inserted. All three original expression key blocks, controller constants and 72 evaluated channels are identical. The diagnostic is not an asset replacement. The production parser now accepts a complete vector immediately followed by its curve's closing `]`; missing components, extra components, an unclosed nonterminal vector and other punctuation remain rejected. `test_partsys_terminal_vector.cpp` checks 101 equivalent color samples, subsequent parsed fields and six rejection controls under ASan/UBSan. Existing zero-position-jitter controls also pass.

Native evidence is private under `research/vfx-wide-20261010/hard-alpha/nakrnoth-parser-{first,repeat}.json`. `nakrnoth_visual_reference.py` runs actual NewObject/CreateAnimator, NextFrame/whole owner Pulse, Animate and BeginScene/PreRender/Render/PostRender/EndScene. It uses the declared fixed owner `(3185,3185,16)`, camera `(3185,3185,0)`, RAM framebuffer and ambient 38/directional 1. Terminal raster dispatch may be omitted only on unsampled ticks; renderer-side controller calls and RNG still execute every tick. The original shipped parameters are unchanged.

This is a source/parser checkpoint, not visual acceptance. Complete lifetime, source-controller comparison and actual Metal appearance remain in progress. Natural boss/spell/map/audio/device behavior is not claimed.

Root integrated build and ASan/UBSan terminal-vector controls pass. The fresh original differential now explicitly asserts controller constants as well as expression keys/evaluations; shipped and closed-vector results match. This is parser compatibility evidence only, not Nakrnoth appearance credit.

## Complete standalone source checkpoint

`nakrnoth_original` now owns an exact typed null-spell owner, all 63 authored key tracks, the original ten cached emitter poses and the 260-slot PartSys. The profile checks geometry, material, texture format, tag literals and key hashes. Real `OF_KILL` ends it at tick 775; it is already visually empty by 700. The old bespoke catalogue entry remains, while exact review mapping selects `Nakrnoth_ORIGINAL` with its real liveness callback and rejects aliases. The Static preview style prevents the harness's short spell retrigger from truncating this long sequence.

Original NewObject `409f61..409fda` interprets the ten `*path` names as hidden. The genuine loaded first-render flags are `81000001` for objects 1–10 and `81000000` for jewel 11. The standalone adapter preserves these hidden paths, prototype 0, `#` camera matrices, `$` no-depth bit and texture-major base order. A first erroneous untextured-path submission and a separate short harness-retrigger film remain private negative controls; neither is accepted evidence.

The whole 790-tick original records 90,820 RNG calls and real kill 775. `nakrnoth_source_probe.py` compiles production key decoding, matrices, parser and PartSys under ASan/UBSan: 645,775 particle fields and 187,840 ordinary local-matrix fields pass. All RNG, live/dead, age and lifespan values are exact. Maximum matrix difference is 0.00001331; accumulated position difference at world coordinates 3185 is 0.001953, retained under a declared 0.0021 limit. Controller Render mutates prototype 0, so that prototype is excluded only from the ordinary base-matrix comparison. 130 altered-profile controls reject. All 22 mapping tests and the compiled review bridge pass.

The late ring demonstrates the original software raster's literal 640/480 primitive-span rejection. At tick 661, widening only `.rdata5b9cc0/5b9cbc` restores the outer ring with identical draw packets, controller pool, owner state and RNG. The exact adapter uses the existing `RetailCometTriangleAllowed` only when both native-domain comparison and software lighting policy are enabled; ordinary/common rendering retains all geometry. No shared renderer or shader change was made.

Private evidence roots: `nakrnoth-native-whole`, `nakrnoth-native-main`, `nakrnoth-source-final`, `nakrnoth-native-tiles`, `nakrnoth-native-middle`, `nakrnoth-metal-final`, and `nakrnoth-metal-view0` through `view5` under `research/vfx-wide-20261010/hard-alpha`. The initial 1280×960 film is retained as a clipped late-expansion diagnostic. A fixed regular 2×3 set of supported 1920×1080 windows has centers `(1440,920),(480,920),(1440,520),(480,520),(1440,120),(480,120)`. No geometry, color, time or scale changes accompany those views. Every repeated same-tick view Render preserves the original particle pool/RNG/lifetime, and each tick agrees with the independent full original clock. `nakrnoth-coverage.json` checks 32,794 authored triangles across all 774 active post-Pulse poses: 126 original span rejections, all 32,668 admitted triangles wholly covered, minimum window margin 120.79 pixels. Raw windows and overlap discrepancies remain separate from any union contact sheet.

This checkpoint is ready for independent appearance review, not approval. The natural boss/spell/map/audio/device and exact-pixel boundaries remain open. Native software framebuffer quantization, nearest sampling and overlapping-window raster differences remain explicit.

Root independently compiles the integrated controller/key/matrix code:645775particlefields agree within the declared0.0021world-position tolerance (max0.0019525),187840ordinarymatrixchannels max1.33e-5, zero state/RNG errors. Application/compiledgallerybridge/28mappingtests pass. Main40stride20PNGs byte-identical through781actual24Hz updates/render withcleanexit. The six-window late ring brightness discrepancy remains held for causal analysis; no appearance approval follows from the positive main-view repeat.

## Terrain and complete-view follow-up

`nakrnoth_ground_probe.py` observes the actual PartSys call at `403082` to
`452e10`, and its return at `403087`, without replacing either. Across the
same 790 original lifecycle ticks, all 49,675 calls return zero. Their actual
MapPane receiver is `6668d8`, level and option are zero, and queried integer
coordinate bounds are X 3143–3246, Y 3129–3209, Z −86–102. Every owner
flag/frame/RNG checkpoint remains identical to the frozen whole reference.
`nakrnoth-ground-native/manifest.json` SHA256 is
`a705b0bb0577aa8b075001e0aa69fae2911b487a7c558f075ae9f44010b361ef`.
The source zero-height callback is consequently grounded in this explicit
empty-map fixture; it does not claim natural terrain behavior.

The owned ordinary animator rejects Nakrnoth's unsupported generic companion
controller/profile and retains no generic PartSys runtime. The standalone
adapter alone advances the audited PartSys. This avoids consuming RNG twice;
the exact source/native lifecycle comparisons include the complete 90,820-call
sequence.

All six corrected late Metal films assert simulation ticks
601/621/641/661/681/701 and actual 1920×1080 dimensions. Earlier auxiliary
films used warmup 25 frames by mistake, were rejected by CSV timing, and remain
separate diagnostics. `nakrnoth_visual_review.py` freezes forty main pairs,
36 raw late-window pairs, immutable coverage unions and source/configuration
hashes. The pair manifest SHA256 is
`67c060d61ef002a22234e4f2c4f2e1baa89f9d296d91eac63eaf881606e5b8d2`;
its captured binary SHA256 is
`3fe353e57b2318c9db380dd031474c5322a5d02349fa3c0358329add5f73c15a`.

Appearance remains held: the complete expansion has matching shape and timing,
but original shock rings are visibly brighter than Metal at 641/681. Actual
loaded original Illuminate produces RGB5 `(23,23,23)` for the 16 shock vertices
in those two phases, matching the helper's normalized-normal calculation.
Single-original-triangle isolation still reaches `(30,62,30)`, ruling out
opposite-facing duplicate geometry as the cause. The downstream raster/state
boundary is under investigation; no light, tint or geometry correction is
inferred from these images.

## Larger-reference lighting correction

The held late-ring brightness was caused by the native viewport fixture, not
the portable renderer. The default 1280×960 `FireScene` registered one original
directional light. Nakrnoth's larger-viewport constructor then created another
`LitSoftwareFixture` and registered another light without removing the first.
The directional registration belongs to the original scene; replacing the
Python fixture does not remove it. The corrected constructor executes original
`56d120` for the old fixture's directional registration before replacement.
No engine, geometry, color, material, shader, binary or Metal capture changes
accompany this correction.

`nakrnoth_lighting_probe.py` runs actual loaded full Animate/PreRender/Render at
the declared diagnostic pose 641 for both supported viewport sizes. At
`56f10f`, original transformed ring colors are `(23,23,23)` with one light.
Explicitly adding a second original directional registration (`56cf00`) changes
them to `(31,31,31)`; removing it (`56d120`) restores `(23,23,23)` exactly.
All authored draw packets and owner matrices, flags, frame and RNG remain equal
across these controls. This proves the brighter single-triangle raster did not
require duplicated geometry. Normalization of the scaled authored normals was
correct throughout. The retained control manifest SHA256 is
`a9af349bca855840a4985177100e64c7a26052b86f9813e31ad8e95f2b2382db`.

The prior `nakrnoth-native-tiles`, `nakrnoth-native-middle` and
`nakrnoth-review-final` artifacts are explicitly superseded duplicate-light
negative controls. Corrected full-lifecycle references use
`nakrnoth-native-tiles-single-light` and `nakrnoth-native-middle-single-light`;
review output uses `nakrnoth-review-single-light`. The original 790-tick pool,
1280×960 main native film, terrain proof, source comparison and all actual Metal
films remain unchanged. Only the corrected independent-review pins can be
considered for appearance acceptance.

The corrected six-window runs are complete: each advances all 701 original
simulation/render ticks, records exactly 601/621/641/661/681/701, and preserves
all baseline owner/RNG checkpoints and same-tick controller pools. Outer-view
manifest SHA256 is
`4bd87f5cd73502c9c02c280724d7221b5813c271cc8c51da22cd1e49d6c73772`;
middle-view manifest SHA256 is
`94ee8790395c2f872a7221aaa6dd69f64b64b5e5557632b26eb7677cf1951645`.
The repeated primitive-span negative control retains 799 observations/two
rejects under original bounds versus 800/zero with widened bounds.

Corrected review manifest SHA256 is
`37930305bed44e9df2513b6290bbd832ea7e12a0e8191a757d828b869e617bed`.
It pins all forty unchanged main pairs, 36 corrected native/unchanged Metal raw
window pairs, the six coverage unions, source/terrain/lighting proofs, and the
rejected duplicate-light review manifest. Lane inspection finds the artificial
ring brightness discrepancy resolved; sampling, RGB565 accumulation, raw-window
overlap and exact framebuffer differences remain. Independent approval and
integrated-gallery validation belong to the root reviewer.
