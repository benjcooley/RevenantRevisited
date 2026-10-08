# MeteorStorm retail tag dispatch

2026-10-05. Documentation-only audit. **The current
`TMeteorStormEffect_BESPOKE` should be treated as an invalid candidate association
for retail `MeteorStorm`**, rather than repaired into a snapshot storm and given
retail fidelity credit. No engine, harness or ledger changes were made.

Machine-readable evidence, raw vertices/materials/tags and source hashes:
[audit.json](/Users/benjamincooley/RevenantRetailLab/research/meteorstorm-retail-tags-20261005/audit.json).
That file's SHA256 is
`ed2db3b88782119541a18241e0c604313b9fd714b381ac93ecf7e429afa4aa91`.

## Exact shipped identity and animation

The shipped `data/imagery.rvi` member `class.def:3375` contains:

```text
"MeteorStorm" "Magic\Comet.I3D" 0xf32bcfac {0,0} {}
```

Archive member `Imagery/Magic/comet.I3D` is 44,428 bytes, SHA256
`b7bad620a73c0229dd49a5e668103b8bc396ad877643e53a3e283b8940dc0302`.
Its resource header has one state, `start`, with **45 frames**, animation flags
`0x2000` (`AF_NOMOTION`) and no loop flag. Body version is 3, flags `0xdc`.
It has 20 vertices, 10 faces, eight objects, three materials and two textures.
Textures are single-frame RGB565, 64×64 and 128×128; decoded alpha must not be
mistaken for an authored alpha channel. Full material descriptors are recorded
in the JSON. Texture 0 materials have white or 0.75 diffuse; the white material
also has white emissive metadata, without proving any particular runtime lighting.

Objects in file order are `#comet`, `comet`, `#sparks`, `fgeys`, `#$head`,
`#trail`, `head`, `shock01`. The unprefixed `comet`, `fgeys` and `head` are empty
animated emitter bones. The five populated objects have four vertices/two faces.
The three particle prototypes `#comet`, `#trail` and `#sparks` use texture0 and
the local face order `(2,3,0)/(1,2,0)`, matching the recovered particle bridge's
supported topology. Their **authored UVs select regions of the texture**; they
are not an eight-frame sprite strip for the old storm helper to overwrite.

## Exact controller tags

All four tags belong to state 0. Stored tag frames are included literally;
the recovered partsys controller uses raw animation frame and does not turn
these into inferred activation delays.

- Frame 3: `blendcont`, value `litaddz`.
- Frame 10: `partsys`, emitter `comet`, prototype `#comet`:

```text
obj=(comet),particle=#comet,pps=[0:50,15:150,20:0],lifespan=5:20,initialvelocity=4:-4,scale=[0:0.5,100:3],rlocalrotation=[0:(0,0,0),100:(0,15,0)],color=[0:(255,255,255),15:(255,197,1),45:(255,126,1),65:(255,72,1),100:(10,1,1)],spread=60,azimuth=60,alpha=[0:1,100:0],friction=[0:0.5],emittersize=1,bounce=-80:-80,relvel=0.1
```

- Frame 18: `partsys`, emitter `head`, prototype `#trail`:

```text
obj=(head),particle=#trail,pps=[0:100,10:100,15:0],lifespan=5:5,initialvelocity=1:1,scale=[0:2,100:0],color=[0:(255,255,255),100:(0,0,0)],spread=1
```

- Frame 27: `partsys`, emitter `fgeys`, prototype `#sparks`:

```text
obj=(fgeys),particle=#sparks,pps=[10:0,11:150,12:0],lifespan=10:18,initialvelocity=-15:-25,scale=[0:0.25,50:1,100:0.1],color=[0:(255,255,255),100:(0,0,0)],spread=180,azimuth=180,gravity=1,friction=0.1,bounce=-50:-50
```

## Native timing and why the snapshot association fails

Clean reference:
[sw-fps-meteorstorm-20261005 manifest](/Users/benjamincooley/RevenantRetailLab/captures/runs/sw-fps-meteorstorm-20261005/manifest.json).
Its activity analysis reports 27 distinct viewport images and changing pixels
from 7.666667 through 9.533333 seconds: a 1.866667-second visible span.
The authored 45 frames at 24 Hz take 1.875 seconds, a difference of 0.008333
seconds, less than one 30 Hz sampling interval. Background error, final-ground
error, post-delete error and restored-ground error are all zero.

This timing agreement **supports the default authored animation interpretation**;
it is not a machine execution trace or proof of entity deletion. Image cleanup
and object lifetime must remain separate gates.

Original snapshot [Class.Def:2042](/Users/benjamincooley/projects/Revenant/Class.Def:2042)
instead binds `Magic\Meteor.I3D`. Original
[effect.h:699](/Users/benjamincooley/projects/Revenant/effect.h:699) defines a
20-slot target cap and 100-tick spawn phase, followed by a long count drain.
Its [effect.cpp:6161](/Users/benjamincooley/projects/Revenant/effect.cpp:6161)
and [effectcomp.cpp:27](/Users/benjamincooley/projects/Revenant/effectcomp.cpp:27)
use the old `TStormAnimator` quad/atlas helper. That choreography and asset
association differ from the shipped Comet tags and brief native animation.

The existing [MS_TMeteorStormAnimator.md](MS_TMeteorStormAnimator.md) already
records no `MeteorStorm` registration string in the recovered retail string
listing, plus the later `StrikeEffect` generalization. Shipped
`Resources/spell.def:264–280` routes the named **spell** through `StrikeEffect`
and `CONTROLDATA "Strike"`, with Comet imagery. Natural spell casting is
therefore a separate caller investigation from editor placement of this type.

## Current port support and deliberate blockers

The correct intended port route is the existing generic effect/default animator:
[TObjectClass::AddType](../../../src/object.cpp:2852) falls back from an unmatched
type builder to the class builder;
[generic EFFECT builder](../../../src/effect.cpp:1127) produces `TEffect`.
[Animator lookup](../../../src/3dimage.cpp:2245) likewise falls back to the default
`T3DAnimator`. No active MeteorStorm-specific builder/animator was found in
compiled source. Do not add a custom builder for the snapshot storm.

Every field present in the three particle strings is represented by
[partsysdefinition.h](../../../src/partsysdefinition.h) and its parser field
dispatch. The existing pure runtime represents the constant `relvel=0.1`,
negative random ranges, lifetime color/alpha/scale/rotation curves and collision.
The [bridge input assembly](../../../src/3dimage.cpp:2360) supplies object-zero
initialization origin, ordered animated emitter matrices and actual walk height.
This audit did not execute new parser or simulation tests for Comet; field
availability is not Comet output acceptance.

Current fail-closed blockers are concrete:

- [Exact-type gate](../../../src/3dimage.cpp:2409) enables only four water IDs.
- [Companion controller gate](../../../src/3dimage.cpp:99) marks `blendcont`
  unrecovered, so all three Comet particle tracks are unsupported.
- [TEffect::Pulse](../../../src/effect.cpp:1152) resets unaudited types,
  including Comet, to frame 0.
- [PartSysOwnsObject](../../../src/3dimage.cpp:2491) and
  [map rendering](../../../src/maprenderer.cpp:1138) suppress an unsupported
  controller's meshes rather than showing a static substitute.

## Blend controller evidence and open scope

Follow-up: [BLENDCONT_GOLD_RETAIL_AUDIT.md](BLENDCONT_GOLD_RETAIL_AUDIT.md)
recovers the actual Initialize-only, per-object blend writes, no-op Pulse/Render,
no restoration, state-initialization timing and emitted-particle override. The
open statements below describe this audit's initial checkpoint and are superseded
by that executable evidence. No Comet runtime enablement or acceptance follows
from the controller recovery; the immediate bounded proposal is GoldEffect only.

Recovered [function 0x405770](../../../recon/classes/cls_0x5a3544.cpp:212), at
lines 269–272, compares `litaddz` and assigns **`0x50`**. The retail string
listing locates `litaddz` at `0x5c5c5c`, with comparison XREF `0x4058d4`
([listing:57204](../../../recon/classes/_data.txt:57204)). It locates
`blendcont` at `0x5c5c94`, builder XREF `0x405750`
([listing:57230](../../../recon/classes/_data.txt:57230)). OOAnalyzer's merged
class identity must not be treated as complete controller recovery.

Original `3dimage.cpp:2780–2824` creates state-matching controllers in tag order;
`:3009–3011` calls their Render methods before drawing base meshes. This proves
ordering, **not blend state lifetime or scope**. Each partsys definition defaults
to mode 16 in the recovered parser/runtime. Whether companion mode 80 affects
emitted particles, only base meshes, or additional phases is **OPEN**. Render,
Pulse, state/frame gating, save/restore and interactions must be recovered before
changing these safeguards. No global blend override is justified by this audit.

## Next bounded work

Recover the `blendcont litaddz` execution contract first. Then enable only the
audited Comet type through the existing generic route, preserve authored frame
progression and test actual saved-sector/NewObject creation. Require three
controller attachments, animated emitter poses, original colors/UVs, frame
progression through the 45-frame state and independently verified cleanup.
Compare that implementation to the retained native reference. Keep natural
`StrikeEffect/Strike` casting and full material/device fidelity as separate gates.

Recommend updating the ledger to reject the current bespoke association and
replace its next action with this authored-controller work. That recommendation
has not modified ledger state. Visual fidelity, runtime integration and final
acceptance remain false.
