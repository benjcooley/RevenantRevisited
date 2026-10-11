# FireCone: original software and actual Metal lifecycle

FireCone now has a full normal-lit original reference beside actual current
Metal captures, using the existing explicit native-domain preview policy.
The opt-in adapter builds the previously proved retail owner from the actual
initialized owner position and facing, marks helper draws for the existing
native projector, and uses that projector's Y expression for the source face
orientation check. Ordinary map owners keep their existing common-world path.
No renderer/shader code or particle parameters changed in this batch.

The source initialization raises the declared origin `(0,0,0)` to ownerZ100.
Both branches execute the same null-spell lifecycle at24Hz with seed1 and one
rendered warmup frame. Retained capture numbers are one-based: frame001 follows
render2/tick2. This comes from `FrameSnap::TickAfterRender` and the actual logs,
not a searched temporal alignment. Six sampled RNG counts agree exactly:
44,220,563,1123,1243,1243. Native Initialize/Animate/Render, owner construction,
relative RenderObject composition, authored normals/UVs/RGB565 textures,
Illuminate and software raster all execute in the pinned retail executable.
Source light is declared ambient38/255, directional1, no point lights; actual
Metal logs confirm those values. Native blend mode8 is executed explicitly.

Private evidence under
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/`:

- `firecone-visual-native-final/manifest.json`:14 original samples plus the
  complete100-step lifecycle trace. Early `firecone-visual-native` is historical
  pre-review work without the one-render warmup and is superseded.
- `firecone-metal-common/` and `firecone-metal-native/`:100 frames each from
  actual Metal binary SHA256
  `e238e156895dbd7ab105977b95b5bac91744138938ee0943c3c553450764b519`.
- `firecone-frozen-review/manifest.json`:SHA256
  `218bb8c637e8a3811ed333ee2385c84769a8a9b0dce8cb4e0af9ce7dc89f1a2b`.
  It pins the14 original/common/native-domain image triples, six RNG count
  pairs, actual lighting logs, capture manifests and map lifecycle evidence.
- `firecone-frozen-review/contact.png`:six fixed lifecycle rows, columns
  original software, Metal common, Metal native domain. Identical crops and
  nearest2x presentation; no pose, color, scale or timing fit.
- `firecone-map-current/FireCone/`:fresh actual FontRuntimeLab capture using
  the original natural-lifecycle command fixture. All60 frames and5 command
  rows pass, including initialized ownerZ116, real move `(20,10,40)` and natural
  absence. Last20 frames exactly restore the floor. `map-contact.png` in the
  frozen review shows actual frames; no map-native pixel equality is claimed.

Visual inspection shows matching orange flame lobes, green-gray smoke and
spreading burst pattern in the native-domain preview. Across14 samples the
summed absolute RGB error is2,066,480 for common Metal and418,625 for native
Metal, about80% lower. This is an ablation diagnostic, not an acceptance
threshold or strict pixel parity. GPU sampling, edge/color interpolation and
destination precision still differ. Both lifecycles finish at tick93; late
particles leave the fixed viewport before final destruction, so blank late
images alone are not used to prove destruction. The native state trace and
actual runtime finish log supply that evidence.

Root appearance review is separate. This configuration has face0, cameraZ0,
null spell, explicit lights and the declared viewport. Natural caster/audio,
other facings/devices and ordinary map projector parity remain outside it.
Previous exact260-slot state/RNG and owner-lit local-translation proofs remain
valid; this batch adds actual GPU appearance and a fresh map lifecycle.

Validation: full Metal build; both100-frame preview runs;60-frame actual map
run;14 complete original render samples; all3 FireCone native render tests,
including negative scale/flicker and wrong-degree negative control. Capture
and reference hashes, clean exit, cadence, source-light logs and six sampled
RNG counts are checked by `firecone_visual_review.py`.
