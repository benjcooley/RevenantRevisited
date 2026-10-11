# ManaDrain: genuine actor target, full original render

Exact shipped type `0xad92bc13`, `Magic/Manadrain.I3D` (SHA-256
`9e29c23dbdf24b269de04bd1724e30d85d521774c67da239affaeaf2f4f5fbd5`).
This checkpoint implements the declared stationary Acolyte caster/fighting-target
configuration. Independent appearance review and root integration are separate.

The source-matched cached original startup loads actual rules, classes, resources
and the 74-entry spell list. Two original Acolytes are constructed at
(3185,3185,16)/(3281,3185,16), and original BeginFighting establishes their real
relationship. The real `Mana Drain` variant supplies damage range60..62. Exact
factory/owner `0x5adf18`, animator `0x5ae118`, Init4fbf50, Pulse4fba60,
Animate4fc000 and complete Render4fc440/RenderObject/raster execute.

Standalone residency is explicit: the original class factory creates the owner;
original SetSpell4de7d0 and summoned-effect list insertion41c840 execute, with
Timer's reverse effect pointer assigned as the isolated binding interface. Natural
MapPane insertion/scheduling is not certified. The actual Timer virtual slot is+4
(53f410); +8 is summoned-object Pulse53f800. No actor methods, state, resources,
rand, effect or raster instructions are replaced. Original Spell::Kill marks the
registered owner killed. The inherited boot uses NOSOUND.

For seed1, Pulse's initial damage roll yields62, the victim loses62 mana at tick21
(252→190), caster mana caps at252, phases last20/50/20 ticks, and original cleanup
kills the effect at91. Tick91..100 stays blank. Render consumes one additional
RNG draw per live frame. The executed code differs from the disabled old source:
stream speed range7..13, forty-unit caster offset/reset distance, and the initial
damage RNG must all be preserved. The port uses real constructed characters and
the loaded spell variant; no skeletal attachment, invented targets or generic
particle replacement is used.

Validation:

- ASan/UBSan compiled core:10,495 defined state/RNG/mana fields pass; maximum
  float decimal-output difference4.89e-8. Dormant uninitialized particle fields
  are excluded until original activation1+3*index.
- Every original draw in100 ticks:19,680 matrix channels, maximum difference
  4.77e-6;704,640 authored vertex bytes and97,560 indices match the own asset.
- Wrong old stream speed changes98 source frames; omitting the initial damage
  RNG changes100. Both negative controls reject against unchanged native data.
- Actual Metal100-frame capture exits0. CSV ticks2..101 determine pair selection;
  contact sheets are not simulation samples. All90 GPU render RNG counts and
  first particle positions match original; phase error4.56e-8.
- Fourteen whole pairs5,10,20,21,30,40,50,60,70,71,80,90,91,100 cover startup,
  transfer, moving streams, shrink, kill and blank tail. Original executed
  projected bounds for all nonzero-scale sampled geometry are contained in the
  1280×960 view (aggregate x642.10..803.35,y561.18..708.34). No pose, scale,
  camera or phase fitting is applied.
- Python exact mapping tests17 pass; compiled bridge test verifies exact identity,
  genuine completion-based recreation, and rejection of arbitrary endpoint calls.

Both fixed meshes retain all authored vertices/normals/UVs/indices. Existing
ARGB4444 source-alpha/quantized native-domain policies are reused; no renderer,
shader, parser or other effect policy changes are made. Ordinary-world projection
is retained as a separate100-frame diagnostic. The configured gallery factory
never reseeds the shared game RNG. Only the explicitly named isolated snapshot
factory can apply its requested seed after authentic context construction.

Frozen private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/`.

- `manadrain-target-native-final/manifest.json`:
  `7f917b48d7e8af73e40e5b5ce859316e822cb906ef180eb8951636d30b4e3179`
- `manadrain-target-state-full/manifest.json`:
  `fb4ed57709fc1e89472931d13d1cd5dcdb135676f1af976d3079ce7eae205638`
- `manadrain-target-metal-final/manifest.json`:
  `b7ddb39ef60a02f579b12a28405517fab39ed3a7e0f0afb0fe838252cafd7507`
- `manadrain-review-final/manifest.json`:
  `a36aeb1210d109f4fc5784ba31c88da6a86def598b6254bfc33d7c074b6afaf8`
- `manadrain-target-metal-common/manifest.json`:
  `011d3e7b62102341db2457ee010fa2a0eed769c3bf6cc7e2e430e3cc61432513`
- All current Metal captures use binary
  `21b27f6ecf6f3250fb4b7825493c957003ec90db1541cbae45faa6e5eae9790c`.

The original base owner/PreRender matrix fragments, explicit camera/RAM output
bindings, material-handle COM getter and source38+dir1 lights remain declared
interfaces. Arbitrary actor motion, natural map insertion/cast scheduling,
health-drain variants, sound/device rendering and exact destination/framebuffer
pixels remain open. This configuration does not claim those broader gates.

Configured actual gallery validation also exits0 on that same binary:120 no-UI
640×480 captures at0.1s through290 legacy ticks,117 nonblank/117 distinct images.
The real91-tick completion callback recreates three times; rolled transfers are
60/61/62 (victim mana192/191/190), health stays102/102, positions/flags remain
unchanged and spell timer is0 at each completion. A fourth instance is safely
destroyed at tick16. All four factories log `seeded=0`.
`manadrain-configured-gallery-final/manifest.json` SHA-256:
`d73973f6c9c6987b27801f596366d43e9225bb6fdc35ef2df3eb600a26b3e5ef`.
This is a current translated gallery runtime/cleanup check, not natural gameplay
actor scheduling or a claim of ordinary-world pixel parity.
