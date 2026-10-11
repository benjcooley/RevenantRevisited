# FlowWater and two bends, 2026-10-10

FlowWater (`0xadbcef15`), BendWater1 (`0xadbcef16`) and BendWater2
(`0xadbcef17`) have complete original-render/actual Metal visual pairs and fresh
real map lifecycle captures. Root independently reviewed all three six-phase
full-frame panels and approved overall forms, colors and scrolling texture
behavior for the recorded native-domain configuration. Software/GPU edge/color
quantization, exact pixel equality and natural scene/device acceptance stay open.

The existing generic map and scrolltex implementations already reproduced the
selected behavior; no shader, renderer, water simulator or general 3dimage engine
change was needed. New exact guards and named actual-owner previews make the
whole comparison reproducible without selecting a guessed effect simulator.
Root's restored unrelated single-state guards were retained unchanged.

## Exact assets and current preview

All three are one-root, one-state/100-frame looping v3 assets with one literal
frame0 `scrolltex` tag, one material and a single-frame128-square ARGB4444 texture.
FlowWater has4 vertices/2 triangles; each bend has28 vertices/36 triangles. The
six-word constant root keys, every vertex/index, material channels, descriptor,
literal tag and object/type/path identity are validated. The original missing
`obj=water` name selects all roots, matching the existing audited controller
fallback. BendWater2 scrolls at−0.01; the other two at+0.01. Texture texels are
whole-asset SHA pinned by the probes, not engine-hashed by the guard.

`scroll_water_profile::Matches` is shared by the new actual-owner preview. Each
preview creates the real typed EFFECT/default animator, calls normal
NextFrame/Pulse/Animate, extracts the literal complete mesh, evaluates actual
ScrollTexOffset with the global legacy tick, and uses the existing exact ARGB4444
helper shader. The explicit native-domain option uses executed-binary-derived
owner/projector constants. No generated geometry, color/size/UV fitting, frame
registration, new spell/caster, or guessed audio was added.

Names: `TFlowWater_AUTHORED_SCROLL`, `TBendWater1_AUTHORED_SCROLL`, and
`TBendWater2_AUTHORED_SCROLL`. Existing real map dispatch remains available.

## Native, Metal, map and review evidence

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-next-scroll-water-20261010/`.
No binary assets, binaries or generated captures are committed.

- `native-visual/manifest.json`: eighteen whole native frames, repeated twice.
  Actual default factory/base/array constructors and full92 named registrations
  plus default constructor execute. Original scrolltex registration401610,
  RefreshControllers40df90, full parser/Initialize, CopyVertices40a0c0,
  Render401990, owner40e470/40e740, key/matrix and complete RenderObject40a8f0
  produce the original FVF_VERTEX dispatch and software pixels. Geometry/UVs/
  colors are not supplied by production. Immutable decoded resources, COM
  material get/set and single-frame texture residency are stated boundaries.
- `metal-visual/{name}/manifest.json`: three actual private-binary60-frame
  captures,24Hz, warmup1, seed1, fixed origin/facing0, black640x340 viewport,
  native domain and declared source light. Source clock logs match capture+1
  and the original owner frame; actual UV offsets agree with±0.01×global tick.
  Every process exits cleanly with a private save path/fixture Revenant.ini.
- `visual-review/manifest.json`: eighteen unchanged full-frame pairs, no crops or
  registration. Nonzero pixel differences remain measured. Each separate
  `{name}-root-approval.json` pins the root-reviewed sample/image hashes, current
  binary, native/Metal/map manifests, profile/preview source and precise scope.
- `map-final/{name}/manifest.json`: all three real NewObject/default animator,
  owner frame1..4, MOVE/position, DELETE/absence lifecycles pass10 observations
  and16 frames each, with four distinct stationary texture frames, visible
  water and exact restored floor. Natural water placement/collision/device
  comparison is separate from this synthetic-floor evidence.
- `profile-final/manifest.json`: actual production guard compiled under
  ASan/UBSan. Twelve literal/path positives and903 independent mutations pass
  their expected outcomes, including every vertex channel/index/key word,
  material float, tag, identity/path and header/descriptor condition.
- `tests-final.log`: three exact-guard/scroll-selector regression tests pass.
  `speed-adapter-regression/manifest.json`: same-source Speed controller/packet/
  native-software regression passes after the additive no-particle adapter option.

Private source binary SHA256:
`535b67bfaa9d6e9220f68df4674006e59677bfbdfe84c25f8ccd697056a08f3f`.
Full private build passes. Relevant runtime/preview files and evidence hashes
are frozen. Central ledger, coverage and review mode files were not edited.

The reusable `NativeScrollScene` also supports no-tag StillWater: initialize
zero controllers, then execute the same complete original object draw. Its
`particle_required=False` resource-adapter option requires no particle tags and
leaves existing particle fixtures' default requirement unchanged. No StillWater
credit is included in this batch. Original normal-mode depth test/write and
positive culling are explicit native scene inputs. Current isolated transparent
helper depth semantics and natural overlapping world geometry are open gates.
