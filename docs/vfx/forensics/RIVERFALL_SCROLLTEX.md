# RiverFall — authored scrolltex restoration

2026-10-04: the literal RiverFall UV controller is restored through the ordinary mesh path. Source-expression checks, integrated build, active Metal rendering and actual create/move/persist/delete captures pass. Retail visual review remains open. This does not restore the retail general `partsys` system or establish overall water/lighting fidelity.

## Evidence and source behavior

Shipped `RiverFall` ID **0xd0c0f03a**, imagery `Misc\riverfall.I3D`, contains exactly one controller tag: **state0, frame0**, `scrolltex`, parameters **`obj=water,du=0,dv=0.01 `**. Actual archive member `Imagery/Misc/riverfall.I3D` SHA-256 **3ca23b872888cb078bdafd6b7f6fb36c1e42a40a6b2da4fa30b7bba42dc12a32**. The tag is source data, rather than an inferred water effect family.

`legacy/3dcont.cpp:68` parses du/dv as double then stores float. `Initialize` at line90 creates instance-local copies of selected object's vertices and saves their original UVs. `Render` at line141 first returns if the owner's frame is below the tag frame; then lines148–150 compute a **float global PlayScreen.FrameCount**, multiply by stored float du/dv, and lines160–166 assign **original UV + offset**. The formula is global, without an instance birth-time subtraction or render-FPS multiplier; textures remain synchronized when another instance appears later. `legacy/3dimage.cpp:2780` RefreshControllers retains state-1 controllers and recreates state-specific controllers on state changes.

## Port implementation

`T3DImagery` holds immutable parsed scrolltex metadata: target object index, authored state/frame and float du/dv. Parsing runs after the subobjects exist and before texture upload. Supported grammar is explicit single **obj=name,du=number,dv=number**, in any parameter order, case-insensitive names; both rates must be finite. Object lists, quoted names, omitted parameters, unknown objects/parameters and overlapping controllers on the same object's state are diagnosed instead of interpreted approximately. This is a limited reusable parser, not a complete replacement controller hierarchy.

`ScrollTexOffset` returns zero unless the target object matches, authored state matches (or is -1), and the live owner frame reaches the tag frame. Otherwise it casts **TTime::LegacyFrameCount()** to float, then performs the source du/dv products. `maprenderer.cpp` supplies the current owner's state/frame and this global 24Hz counter to each mesh submission. Existing owner/bone world transforms remain the placement path. It does not mutate shared imagery vertices, shared GPU meshes, cached poses, or another instance's UVs; it performs no per-frame mesh registration or GPU asset allocation.

`SMeshSubmit` now has zero-default uv_offset[2]. Its existing instance buffer row grows from24 to28 floats: original world/tint/object-ID fields retain offsets0–23, UV occupies24–25, padding26–27 is zero. Vertex attribute9 is FLOAT2. The hand-authored Metal/GLSL/HLSL mesh vertex shaders add that instance offset to the original mesh UV. Other submissions get zero offset. These headers contain shader source strings, not generated shader artifacts.

Valid scrolltex imagery uploads its texture with repeat sampling through an explicit renderer registration argument. Untagged/unsupported imagery keeps the previous clamp default. Repeat texture asset keys include a sampler discriminator; pre-existing clamp keys remain unchanged. This avoids a cached clamp upload being reused as a repeat texture. The scroll controller source establishes the UV expression, but does not itself issue a texture-address render state; complete device/sampler/filter parity still requires native comparison. Repeat is the explicit port policy for authored scrolling, rather than reducing UV modulo1 before interpolation and clamping at the seam.

The query currently affects the **ordinary instanced mesh** path. Transparent helper meshes, custom replacement VFX submissions, full legacy controller lists/overlap rules, and controllers that must retain modified instance vertices after a later loop falls below a nonzero tag frame are outside this bounded implementation. Literal RiverFall's sole tag has frame0 and state0, so that legacy carry-over nuance does not arise for it. Unsupported inputs are visible in diagnostics; do not treat every scrolltex asset as accepted.

## Clock and validation

`TTime::FrameCount()` counts render frames and is deliberately unused. `TTime::LegacyFrameCount()` advances from scaled elapsed simulation time at24Hz and holds during time scale0. This preserves global synchronization and slow motion. It is a global app simulation clock; exact phase matching against a retail PlayScreen restart is not established. The existing accumulated-double/floor clock can differ by one tick at mathematically exact boundaries depending on render cadence; no global clock change was included. This caveat is distinct from erroneously advancing scroll by60 or144 ticks per second.

`/Users/benjamincooley/RevenantRetailLab/check_scrolltex.py` compiles **extracted production parser, initializer, query, actual instance-packing loop, texture key function and wrap selection**, plus the actual legacy Render body as an independent oracle and actual TTime implementation. Passed:

- **27 bit-exact** UV comparisons using three nontrivial original UVs, including negative/out-of-range coordinates, at nine global tick counts (including the float-integer precision boundary).
- Two simultaneous instances sharing imagery remain synchronized; inactive states/other objects cannot change another instance; tag-frame boundary and state-1 matching are checked.
- Parser acceptance/rejection, unknown object/list diagnostics and overlap diagnostics.
- Actual28-float packing preserves world/tint/object ID and distinct UV offsets, with zero padding.
- Repeat/clamp selection and distinct repeat keys with unchanged original clamp keys.
- **2,610** rational clock comparisons away from ambiguous exact boundaries at15/24/30/60/144 render Hz, with scales0/.125/.5/1/4. Every scroll query uses the fixed global legacy counter.

Offline Metal compilation was attempted using the actual MSL source. Installed Xcode reports **missing Metal Toolchain**, directing `xcodebuild -downloadComponent MetalToolchain`. No compiler result is claimed. Root integration's live shader creation and capture must verify the active Metal path. No full build, guest control or port render was performed by this agent.

Artifacts: `lab/research/riverfall-scrolltex/riverfall.I3D`, `source-validation.json` with modified source hashes, extracted `mesh-0.metal`, and `lab/research/riverfall-scrolltex-source-validation.log`.

## Parent integration and retained runtime evidence

Binary SHA-256 **9291e9e5d272988ef525ecd8b1cbe11b114c5289c90eaf257fa3bff3f0282005** builds successfully. The 163 existing particle/transform/host tests and animation-system checks pass. Actual Metal shader creation and mesh draws succeed in the following captures, closing the active-backend gate despite the missing offline compiler.

Under `/Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/water-authored-20261004/`:

- `riverfall-commands/pre-fix-capture/manifest.json`: eight identity/placement rows pass, but 120 images contain only two distinct states from creation and MOVE. The texture does not scroll.
- `riverfall-commands/post-fix-capture/manifest.json`: the same eight rows pass and 120 samples contain **96 distinct images**. Frame observations progress from1 through72; the retained `review.png` shows the authored water mesh over synthetic ground.
- `riverfall-lifecycle/post-fix-capture/manifest.json`: a separate derived timeline adds ordinary DELETE at tick84 and `@absent` at85, retaining the earlier commands unchanged. All **10 rows pass**; 120 samples contain85 distinct images and the final12 ground-only images are byte-identical. `lifecycle-review.json`, `runtime.mp4`, `effect-review.png` and `deleted-review.png` retain the review evidence.

This fixture creates the exact retail type through ADDAT, raises it from inherited Z16 to128 with ordinary MOVE, verifies one default animator and removes it normally. The backing module and surrounding floor are synthetic; no original-map or story-trigger acceptance is claimed. No saved RiverFall record was found in the 4,896-sector scan. Owned capture processes required termination after completed output because teardown stalled (exit−15).

The fresh native software attempt `captures/runs/sw-riverfall-scroll-20261004/manifest.json` accepted the commands but showed no effect pixels. Both settled screenshots match the effect-free ground below the recorded FPS band. Its lossless AVI is retained as a rejected reference. Port animation does not establish native parity; global clock phase, sampler/filtering, shading and original use remain open.

See [WATER_VARIANT_DISPATCH.md](WATER_VARIANT_DISPATCH.md) for the four neighboring water types. Their authored frames now advance and wrap, but their missing `partsys` controllers leave the visuals static. This texture-scroll restoration does not implement their emitters.
