# Paralize1: authentic one-target rendering

Exact `paralize1` `0xad92bc14`, `Magic/Paralize.I3D`, now has a separate
`TParalize1_AUTHENTIC_TARGET_FIXTURE` preview. The existing named, null-invoker
preview remains partial and unchanged. This fixture does not silently convert
that unbounded branch into a target effect.

The original reference reuses the source-matched boot and genuine Acolytes
described in `AUTHENTIC_CHARACTER_PARALIZE_20261011.md`: invoker `(3185,3185,16)`,
fighting target `(3281,3185,16)`, original Radius16, real Physical Paralysis
spell creator and actual typed owner/animator factories. Original `srand(1)`
runs after context construction; original `rand` instructions execute throughout.
The source fixture also seeds after its real actor construction when an explicit
snapshot seed was requested. No target radius, action, actor flags or stats are
fabricated in the original.

The native fixture executes whole owner NextFrame/Pulse and custom Render
`4fb650`, RenderObject, vertex conversion/illumination and software raster on
every live tick. Original owner/PreRender matrix fragments remain the declared
base-animation boundary. The camera is `(3185,3185,0)`, viewport1280x960,
screen center `(640,640)`, source ambient38/directional1. Actual BeginScene and
EndScene run; after BeginScene binds the HUD display, original camera APIs
`54de20/54dd70` select the isolated RAM color/depth targets. The only added COM
method behavior retains material-handle GetLightState/SetLightState key1 from
the actual booted device; it does not supply colors or substitute resources.

The source adapter uses real Acolyte instances and their GetPos/Radius,
BeginFighting, Disable, Stop and paralysis flag operations. A separate core
implements the observed one-target variant; the production draw iterator also
drives the independent compiled pose comparison. Two immutable mesh keys cover
the authored four-vertex sprite and151-vertex sphere. There are fifteen ordered
sprite calls and, once their scale is nonzero, three ordered egg passes.
Render controls UV updates; Pulse controls movement and lifetime.

One material source discrepancy was resolved from the executable rather than
the disabled source listing. Original `4fb7c5..4fb85f` uses target X/Y unchanged
and target Z plus literal40 (`5a4a64`). The disabled listing's `-10,-20,+20`
offsets were incorrect. Original LVERTEX conversion `408a8e` supplies diffuse
`0xfffffff8`; the existing explicit prelit helper carries its RGB5 value31.
Neither correction changes the global converter, projector or shader.

The port fixture keeps its actors at the declared source coordinates. Its
explicit display origin relocates only the effect's submitted scene, allowing
an isolated camera and a separate translated-origin smoke test. It does not
claim natural map target selection, animated character attachment or a complete
source spell scheduler. Original target cleanup and source owner kill are real;
post-kill source ticks stop simulation and RNG use.

Evidence resides under
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/`.
Native, compiled-state, actual Metal, translated-origin and review manifests
are separate. The numbered movie has280 captures, whose actual timing CSV
maps to simulation ticks2..281; the additional `frame-filmstrip.png` is a
generated contact sheet, not an initial or extra simulated frame. Natural
caller/audio/device, other spell variants, framebuffer quantization and exact
pixel equality remain open. Approval is recorded separately by the reviewer.

## Frozen comparison

The original completes280 ticks, kills at260, then leaves owner state/RNG
unchanged. It executes11,400 actual rand calls. Target Health102, position and
flags return unchanged. The source film has the same kill and blank tail;
actual source phase logs agree with the compiled production core within5e-6.
Sixteen same-tick pairs cover5..280. Original projection of every submitted
vertex in those sampled frames stays strictly within the fixed viewport.

The compiled ASan/UBSan comparison checks67,480 owner fields and3,744 matrix
channels. All integer/RNG/draw-count checks,13,392 authored XYZ channels and
17,388 indices agree. The initial .003 absolute/2e-6 relative float diagnostic
tolerance is **not widened**:22 exceptions remain explicitly recorded, maximum
0.006254196 world units and0.00366336 in sampled matrices. Accumulated per-vertex
native UV versus the source uniform offset differs by at most0.00009351.
The report is `bounded_float_residual`, not an exact-state pass. Compiled
negative controls reject the stale egg offsets (564 draw packets) and missing
target-center offset (91,851 state bytes/4,299 draw packets).

The same binary also finishes the translated `(100,100,16)` isolated fixture:
70 captures at stride4, full24Hz simulation through278,65 nonblank samples,
66 distinct images and clean exit. This is supporting translation/liveness
evidence, not a natural map target-selection claim.

The retained null-invoker regression still passes7,712 state fields and its
existing negative controls. Its fresh32-frame movie is pixel-hash identical to
all32 retained same-configuration frames before this target route was added.

Binary SHA256: `f6080773940ec9631586959355aa5093274fc04b20a1096b933ebd46d55250c1`.
Each following path is relative to the evidence root above and names
`manifest.json`:

- `paralize-target-native-first`: `6af5f590ad401bdd4074b35b432238a8b2784199a94a4209a79821d6198a75c1`
- `paralize-target-state-reviewed`: `710c62b32293f6b7ad9e8a42ed1668e82689b2d4fae7af14fc8c8076cf2abb19`
- `paralize-target-metal-final`: `da07dbadbc8845ed3572aaafe57d57eca7ff4b16f2a9807c90eb6cd385c8bcc0`
- `paralize-target-metal-translated`: `f693c5abc15f7138cc50384ca58be606ae5d9b5bc872f0d3753d6ecea2bf60ce`
- `paralize-target-review-final2`: `c51dd5aeeae55c81f69e219c8f655ebeed680ad14699bae91c5e15cf45e5a4c8`
