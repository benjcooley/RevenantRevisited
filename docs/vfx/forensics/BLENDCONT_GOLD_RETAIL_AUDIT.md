# Retail blendcont and a bounded GoldEffect route

**Current checkpoint:** source-backed Z handoff passes overall isolated default Gold appearance against two own independent retail references. Historical mismatches below remain. Exactcadence/realmapcollision/caster/audio/context/full acceptance stay open. [Terminal bridge result](PARTSYS_Z_DOMAIN_HANDOFF.md).
Current checkpoint, 2026-10-05: Gold retail punctuation/camera/depth and cached-frame corrections are built; real runtime and two independent-reference pairs pass operationally. **Visual mismatch remains** in arc height and visible duration. Cure regression preserves all 240 previously reviewed frames. Detailed source-audit prose and prior integration evidence below retain their separate phase scopes; no original pickup/story or full appearance acceptance is granted.

2026-10-05. Initial read-only audit of the actual shipped executable and asset;
the subsequent authorized bounded implementation is recorded below. No full
engine build, render, guest input or acceptance claim by this worker. Evidence and
eight original/compat byte-identical disassembly ranges are retained in
[audit.json](/Users/benjamincooley/RevenantRetailLab/research/blendcont-20261005/audit.json).

**The final correction below supersedes initial mode16/live-bone-only/frame1
implementation claims:** retail object-name punctuation and cached animator
frame were missing. The first matched visual pair is a retained failure, not
acceptance. Read the final correction section for the current sourced path.

## Actual controller contract

The registration is `0x405750`, literal `blendcont` at `0x5c5c94`, factory
`0x405f60`. The factory allocates `0x30` bytes and installs vtable `0x5a357c`.
OOAnalyzer merged its parser into the partsys class; that merged name is not
the controller's actual identity. The vtable entries are ParseParams `40d3e0`,
ParseItem `405770`, destructor `405fe0`, Initialize `405940`, Close `4059c0`,
Pulse `4060d0`, Render `4059d0`, and auxiliary query `4060f0`.

**Pulse and Render are literal `RET` instructions.** Initialize parses the
parameters through `40d750`. When its object list is empty, it enumerates every
non-null animator object, including empty bones and particle prototypes:

```text
object.flags |= 0x01000000                    // OBJ3D_BLEND
object.blend = (object.blend & 0x40) | mode
controller.object_list.append(object)
```

The writes are at `40597c–405990`. A non-empty preselected object list branches
at `40595f` directly to the success check, skipping these writes. Therefore the
first supported grammar should be a plain recognized mode, not an invented
`obj=` filtered override. The factory does not initialize the mode member;
missing/unrecognized mode must be rejected in the port rather than defaulted.

Close jumps to base `40db50`, which clears the list and its counters. It does
not restore object flags or blend values. Destructor `406000` likewise has no
object-state restoration. Blend changes persist until another writer or object
destruction. This is not a save/restore wrapper around an entire owner's draw.

Parser `405770` recognizes none=0, normal=1, alpha=2, litalpha=4, add=8,
litadd=16, nocheckz=65, litalphaz=68, litaddz=80 and alphaadd=32. Other tokens
delegate to the base parser; supporting all of that grammar is outside the
bounded Gold proposal.

## State, frame, depth and particle interaction

Snapshot [RefreshControllers](../../../legacy/3dimage.cpp:2780) builds controllers
in asset tag order when their state becomes current; state `-1` is initialized
once on first refresh. It removes prior-state controllers on state change.
There is no frame check inside actual blendcont Initialize/Pulse/Render, so a
stored frame 5 or 3 is **not a delayed blend activation**. Snapshot rendering
calls controller Render before drawing base objects, but blendcont Render is
empty. The state-transition removal does not undo the object writes above.

Actual RenderObject `40a8f0` tests OBJ3D_BLEND at `40ae03`, reads object+`348`
and calls SetBlendMode `417d60` for that individual draw. There are no calls to
SaveBlendState `4178e0` or RestoreBlendState `417b00` in the audited RenderObject
range. This does not prove that every surrounding renderer phase omits restore;
it does rule out inventing a blendcont-wide pushed global override.

SetBlendMode's litadd branch `41827b` sets SRCBLEND=ONE and DESTBLEND=ONE at
`4182ca–4182db`. ZWRITE is false without bit `0x80`; ZENABLE requires global Z
enabled and bit `0x40` clear (`418284–4182bb`). Thus litadd16 normally tests Z
without writing it; litaddz80 disables Z testing too. The `z` suffix must not
be read as enabling writes. Preserve the inherited `old & 0x40` bit.

Particle Render `4026d0` explicitly ORs object flags with `0x140206c` and copies
the particle's own blend into prototype+`348` at `4026e9`. That writer overrides
the companion's prototype mode on each particle draw. **Do not apply the
companion mode to emitted particles.** Default partsys16 and an explicit
partsys alpha2 remain separate from the base mesh's blendcont mode.

## GoldEffect's actual visible content

Shipped identity is `goldeffect`, `0xd0c0f035`, `misc\\Goldp.i3d`; there is no
named retail animator in the retained registration inventory. Actual asset
`Imagery/Misc/goldp.i3d` SHA256 is
`0236d13a7715160d4079cabb7fc6bf9907bb591b0cae3b8e13754ac32dd07d04`.
It has one non-looping, NOMOTION `start` state of 45 frames, eight vertices,
four faces and these objects:

- `gold`: an empty animated emitter bone.
- `#$iflare`: a populated four-vertex/two-face base quad using texture0,
  RGB565 64×64. Its state0 animation keys are non-null. The pre-implementation audit
  considered only the source IsHidden rule; retail punctuation-derived instance
  flags also matter and are now under concrete correction. Retain authored
  transforms/materials according to those source rules;
  a `#` or `$` in its name is not a license to drop it. Individual frames can
  still have zero scale or other authored poses.
- `#goldp`: the four-vertex/two-face particle prototype using texture1,
  ARGB4444 16×16. The partsys owns and hides this template; do not draw another
  static copy.

State0 tags are frame5 `blendcont litadd`, then frame10 partsys with emitter
`gold`, prototype `#goldp`, PPS `[0:400,2:0]`, speed45:50, **blendmode=alpha**,
scale1.25, life35:35, white color, zero local rotation, gravity1.5,
friction0.05, spread8, azimuth8 and bounce−25:−25. All fields are already
represented by the recovered parser/state module. Both controllers initialize
on the state refresh; curve evaluation uses the raw animation frame. Do not
turn the stored tag frames into activation delays or replace the coins with
the old snapshot Gold proxy.

## Default non-looping lifetime and minimal scope

Actual retail TEffect::Pulse at `4dee83–4deed5` checks an existing animator,
null spell pointer, state flags with AF_LOOPING `0x1` clear, and CommandDone
true. It then calls SetFlags with old flags OR `0x1000` (OF_KILL), followed by
SetCommandDone(false). CommandDone accessor `477d30` reads object+`80`. This
is the authored owner's animation-completion cleanup; do not add a guessed
particle-drain lifetime. Native return-to-ground alone is not object deletion
proof. **Pre-implementation port behavior:** TEffect reset Gold to frame0 and cleared CommandDone,
so allowing the controller without repairing that exact-type owner path would
remain incomplete.

A viable bounded implementation is **GoldEffect only**, on the generic
TEffect/default animator route: exact ID and verified asset/profile, state0
plain litadd initialization metadata, one recovered partsys track, preserved
base flare, explicit particle alpha2, raw-frame curves at 24Hz, and the retail
null-spell non-looping completion predicate. Use per-instance/per-draw blend
metadata; never mutate shared imagery or a global particle blend. Keep current
four-water topology, mode16, looping state and caller gates unchanged.

Require a real NewObject or loaded-sector test with nonzero MapIndex, base
flare submission, changing coin transforms, source counts/RNG, frame progression
through completion, stable-identity natural absence, explicit deletion, and the
retained native SW comparison. The RGB565 base and ARGB4444 particles use
different SW raster paths; existing alpha-over evidence must be applied through
an explicit format/render policy, not an additive-companion shortcut. Full
material/device and linked-spell acceptance remain separate gates.

Comet and CombatFlash are **not enabled by this audit**. Comet has multiple
tracks and relvel; CombatFlash has 18 states and some trail parameters. A later
integration can reuse this controller contract, but must retain their separate
state/caller/renderer evidence and unsupported-field safeguards.

Original executable SHA256:
`28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.
Compatibility executable SHA256:
`33545a2f4e055dfaa3a62e3b03488d6f3bd63fe6c9827b1f88e141e1387073eb`.

## Implemented vertical slice and verification handoff

The authorized follow-up changes only `3dimage.h/.cpp`, the generic Gold case
in `effect.cpp`, `maprenderer.cpp`, and a queue API in `renderer.h/.cpp`.
There is no new Gold-specific choreography, effect builder, animator or
simulation module. The helper shader strings and default helper queue are
unchanged by this follow-up.

`T3DImagery::ValidateGoldPartSysProfile` checks the literal Goldp path, version3,
non-morph body, one 45-frame NOMOTION state, all three object identities and
populated/visible quads, exact ordered tags/parameters and texture formats.
Modified tags, states, modes, object topology or texture formats fail closed.
The existing partsys profile additionally checks the authored triangle topology
and UV-bearing vertices. Gold's alpha2 exception and recognized blendcont
companion apply only to that imagery profile. Runtime additionally checks exact
owner ID `0xd0c0f035` and state0. The four water caller/mode16 profiles stay
enabled; Comet, CombatFlash and other parsed assets remain unsupported.

State-entry initialization applies the recovered object flags/blend writes to
the live animator objects. `GoldBaseMeshBlend` exposes only actual object1,
`#$iflare`, and no particle-global override. The map bridge copies the flare's
actual cached mesh, authored material fields and live bone matrix to an
additive helper submission in software mode1. The particle bridge keeps the
actual #goldp vertices, UVs, source transforms/color and separate alpha2 mode;
its existing `retail_texture=1` path selects nearest wrapped source texels.
No shared mesh, UV, material or texture sampler mutation is needed.

**Render order matters:** retail particle controller Render precedes default
base mesh rendering. Alpha coins followed by the additive flare differ from
flare followed by alpha coins. `SubmitGoldFlareAfterFx` therefore queues just
this flare after `DrainFxQueue`, before overlays. It uses the existing helper
pass: both that pass and FX attach the same RGBA8 `lit_target` and DEPTH
`depth_target`, load color/depth, and the helper additive pipelines retain
LESS_EQUAL/no-write depth state. Normal helper submissions keep their original
queue and timing. For multiple overlapping Gold owners or other intervening
transparent owners, the global FX batch does not reconstruct original
owner-by-owner interleaving; that ordering remains an explicit open gate.

The exact Gold generic Pulse preserves raw frame progression and adds the
retail null-spell, existing-animator, non-looping CommandDone kill predicate.
It sets OF_KILL and lets the existing map pulse reap later; it does not delete
inside animation or infer lifetime from particle counts. Linked-spell lifetime
is not accepted by these editor/null-spell tests.

Focused checker:

```sh
python3 /Users/benjamincooley/RevenantRetailLab/check_gold_authored.py
```

It compiles extracted **production** profile and generic Pulse bodies against
small immutable resource/owner stubs, plus the actual parser/state module,
using ASan/UBSan. 14,070 checks passed: exact shipped tag/profile success,
26 malformed profile cases, path normalization, the complete cleanup predicate
truth table, retained water/unknown-type frame behavior, actual parser fields,
and 16 seeded Gold state runs through the complete 45-frame state. Capacity is
583; PPS produces 16 then 9 particles, source spawn draws total125, particles
retain scale1.25/alpha1/blend2 and finish before owner animation completion.
These counts are source contracts, not fitted native image counts. Existing
partsys instruction oracles establish the shared math/RNG implementation; this
new focused checker does not claim a separate bit-exact x86 Gold replay.

The same checker validates pass attachment/depth policy and an independent
noncommuting alpha/add equation: source order yields .25, reversed order .20
for its fixed example. It does not execute a GPU pass. Hashes, generated
contract, sanitizer output and render-order gate are retained in
[validation.json](/Users/benjamincooley/RevenantRetailLab/research/gold-authored-20261005/validation.json).

Root integration should use real `add goldeffect`/loaded-map creation, exact
default animator and existing map pulses, requiring nonzero MapIndex,
`[blendcont] init` mode16 on three objects, one partsys emitter/controller,
capacity583, both changing coins and the authored base flare, natural stable
identity absence after animation completion, and explicit-delete cleanup.
For a manual preview, instantiate the actual owner/default animator and submit
the cached authored base mesh through `SubmitGoldFlareAfterFx` **after
BeginTilePass**, with additive=true and retail_lighting=1. Advance only on real
24Hz simulation ticks; do not combine a globally pulsed owner with another
preview ticker. The real sector path is the preferred first verification.
Retain matched retail ambient/directional inputs through the existing lighting
environment. Positional-light selection and multiple-owner transparency remain
separate gates; do not fit brightness to the native clip. Full runtime,
visual fidelity and acceptance await the integration owner's build/captures.

## Matched-backdrop actual-owner preview

The production generic route subsequently passed root's actual-sector fixture
at [gold-generic-static-20261005](/Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/gold-generic-static-20261005/command-capture):
actual Add/default animator, one controller/emitter with capacity583 and live
quads, and natural stable-identity absence. That sector's floor/clear color
differs from the retained native ground PNG, so it establishes runtime behavior
rather than a matched-background visual pair.

New `effects/goldpreview.h/.cpp` and the narrow VFX catalogue entry
**`TGoldEffect_AUTHORED_TAGS`** provide the matched-backdrop route. This is an
adapter to the actual class `NewObject` with exact ID, positive MapIndex and
generation SafeRef; `OnScreen` creates its real default T3DAnimator and tag
controllers. It is sector-less and is advanced only by the adapter, not also
by a map object iteration or a preview-specific global component updater.
The old `TGoldEffect_BESPOKE` entry remains the separately named invalid proxy;
it is not used by this adapter or given retail credit.

`Advance` accumulates scaled render delta into the fixed 24Hz source clock,
executing real `NextFrame` then real owner `Pulse` once per elapsed tick. It
reaps a previous OF_KILL on the following tick, preserves the weak identity,
and never respawns. This reproduces the actual sector ordering: starting from
frame0, its first consumed simulation frame is1. It does not reset or fit the
phase to force the pure frames0→1 checker peak of25; the integrated runtime
fixture's live8 particles is compatible with its different first consumed
frame. Native image counts do not justify a phase edit.

`submit_world` refreshes the real live animator pose without ticking, submits
coins through production `SubmitPartSys(2,2)`, then submits the immutable
authored flare mesh via `SubmitGoldFlareAfterFx`. It copies the actual material
and transposes the live bone world matrix exactly as the map bridge does.
Mesh asset references are acquired/released by the adapter. There is no
generated flare billboard, phase/RNG mutation, background subtraction, new
crop or color/size adjustment. The Static catalogue style disables automatic
preview retriggering; restart creates a new owner with a new generation.

Use the existing matched PNG backdrop, camera320,170, origin relative0 and
matched retail ambient/directional inputs, for example through the existing
`ab_compare.port_frames` helper with ID `TGoldEffect_AUTHORED_TAGS`. Factory,
advance and world-submit hooks are already registered; the root build's
recursive source glob picks up the new module. The same single-owner
render-order and positional-light limitations above remain explicit.

[check_gold_preview.py](/Users/benjamincooley/RevenantRetailLab/check_gold_preview.py)
passed nine focused ASan/UBSan cases using the extracted production Advance
body and the actual SafeRef generation implementation: 6/12/24/30/60/120Hz,
multi-tick catchup, zero/negative/nonfinite delta, independent owners, natural
weak-reference invalidation and no respawn. Each complete run performs45
owner pulses and46 NextFrame calls before reaping, independent of render rate.
Source checks also verify real exact-ID class creation, no simulation calls
from world submission, production particles-before-flare calls and authored
matrix/material preservation. These tests use a small owner completion stub;
they do not claim a second real-engine run or GPU acceptance. Results and
source hashes are in
[preview validation.json](/Users/benjamincooley/RevenantRetailLab/research/gold-preview-20261005/validation.json).

The first integration build caught incorrect quoted include paths in the new
subdirectory module. The seven engine includes were corrected to `../` paths,
matching neighboring effects; the local header include stayed unchanged. All
paths resolve, and reconstruction/hash comparison proves this was an exact
include-only delta from the sanitizer-validated source. The retained failed
build log and [include-fix.json](/Users/benjamincooley/RevenantRetailLab/research/gold-preview-20261005/include-fix.json)
preserve that checkpoint. Behavior tests were not unnecessarily repeated for
the include correction; the integration owner rebuilds and captures next.

## Source correction after the first matched visual failure

Root's first matched pair, binary
`28ba1e9d4783889d971bc05c6bcadeb5327fc57ecba575452c41212fa4af1ed3`,
retains exact-zero backgrounds but a concrete visual mismatch: missing flare,
only eight coins and differing projected geometry. It remains unaccepted at
[sw-fps-gold-authored-tags-20261005](/Users/benjamincooley/RevenantRetailLab/captures/ab/sw-fps-gold-authored-tags-20261005/manifest.json).
Two recovered source omissions explain actionable differences without fitting
the clip. Seven additional original/compat byte-identical assembly ranges are
in [retail-correction-provenance.json](/Users/benjamincooley/RevenantRetailLab/research/gold-preview-20261005/retail-correction-provenance.json).

Actual imagery NewObject `409ca0`, punctuation branch `409f61–409fda`, first
tests whether the initial byte is outside ASCII0–9/a–z/A–Z. Only then it searches
the entire name: `#` ORs flag `0x20000000`; `*` ORs HIDE1; `$` ensures BLEND
metadata and ORs mode bit40. Thus `#$iflare` retains bit40 through the already
recovered blendcont `(old & 40) | 16`, yielding **mode80**, not16. The corrected
Gold helper submission explicitly disables Z testing and writes; ordinary
helpers retain LESS_EQUAL/no-write. New ALWAYS/no-write pipeline variants are
selected only by Gold's explicit payload, additive blend and software mode1.
No shader or broader helper/water policy changed.

Flag `0x20000000` selects actual CalcObjectMatrix `40a71f–40a852` when retail
global `6671f0` is zero. The software path rebuilds a matrix in this exact order:

```text
Scale * Rx * Ry * Rz * fixedY * fixedZ
then write the preserved translation into _41/_42/_43
```

`fixedY` uses exact binary32 cosine `3f51b3f2` (.8191519975662231) and sine
`3f12d5e0` (.5735759735107422), with the negative sine at row0/column2.
`fixedZ` has diagonal `bf3504e6` (−.7071059942245483), row0/column1
`3f3504e6` and row1/column0 `bf3504e6`. These literals are exporter/source
camera transforms, not approximate angles chosen to match an image. The new
pure helper `goldauthoredmatrix.h` retains the exact coefficients and order.
The actual authored flare mesh/material/keys remain; the Gold bridge obtains
the source animation-key TRS, applies this branch, then the live owner's root
matrix. Particle prototypes with `#` retain the same source camera matrices
after sampled local rotation and before absolute translation. Gold's emitted
particles still override their prototype's blend with explicit alpha2: the
flare's depth-disabled mode80 never becomes their global mode.

Independent executable execution compares the new helper plus extracted
production math functions against actual retail **40a420**, with only the
GetAniKey boundary isolated to identical exact TRS inputs. All retail matrix
and x87 helper instructions execute unmodified. 36 poses, including authored
flare values, translation, rotation and nonuniform scaling, pass720 matrix and
projected-corner comparisons; maximum matrix difference is2.3961e−7 against
3e−6 tolerance, projected-corner tolerance6e−5. **Scope correction: both local matrices use the SAME HOST projection formula for the corner comparison. This proves local matrix math, not actual retail view/projection/root/bitmap depth or full projected placement.** This is bounded numerical
evidence, not a bit-exact GPU/retail visual claim. See
[matrix-validation.json](/Users/benjamincooley/RevenantRetailLab/research/gold-preview-20261005/matrix-validation.json).

The first emission differs because actual partsys Pulse `40376b/403771` reads
**controller.animator+14**, not owner+5c. Animator constructor `445940` initializes
that cached frame to0. Animate `445a7e` reads owner frame and setter `410ac0`
writes cache+14 later; default animator Pulse `40e2e0` does not copy it first.
Four actual-x86 frame-reader/Animate cases retain owner frames1–4 while reading
cached0–3 before Animate, without resetting the owner. Gold-only FillInputs
therefore now reads `animator.GetFrame()`; water input readers are unchanged.

The independent evidence agent's stronger full-Pulse replay holds owner
frames1/2/3 and RNG/externals identical: cached0/1/2 evaluates PPS400/200/0 and
produces16/25/25 live particles; substituting owner1/2/3 evaluates200/0/0 and
produces8/8/8. See
[independent lifecycle audit](/Users/benjamincooley/RevenantRetailLab/research/gold-lifecycle-independent-20261005/audit.json).
This demonstrates the first-frame bug directly from the executable, not native
image counts. Multiple catch-up ticks can repeat a cached frame until Animate;
the correction preserves that dependency. It does not silently normalize
particle counts across all render rates or add an owner reset.

Updated focused profile/state and preview-clock sanitizer checks pass14,070
and9 cases respectively. Their clock proof counts pulses/reaping; it does not
claim render-cadence-independent particle emission. The new cached-frame and
punctuation/depth checks are retained with the earlier failure evidence. Root
owns the combined build, renewed real-runtime fixtures and matched pairs.
Positional lighting, multiple-owner transparency, other authored asset flags,
animation-key/raster/device parity and linked-spell scope remain separate gates.
**Visual acceptance remains false until new comparisons are reviewed.**

## Bounded remaining mismatch: defer without appearance fitting

The corrected binary
`60026cf18d66e044ce505a337e10afd4f9506266d9b880e7607349e62345755b`
restores flare, projected coin geometry and the first emission. Root retained
two completed matched-background pairs, `sw-fps-gold-authored-tags-fixed-20261005`
and `sw-fps-gold-authored-tags-repeat-fixed-20261005`. Both retail seeds reach
top y40 and last visible frames53/54; the port reaches y75 and frame45.
Runtime static/move/delete fixtures pass and Cure's240 frames remain identical
to its accepted baseline, but these facts do not clear Gold's visual gate.

A fresh bounded read-only audit did **not** prove another correction:

- Actual Pulse `403cd9` calls CalcObjectMatrix with current owner state/frame,
  then `403cf9` obtains that refreshed emitter matrix from animator `40ef80`.
  The current-owner emitter sampling is source-backed; the cached animator
  frame governs expression evaluation separately. Do not delay all emitter
  poses merely because the expressions read cached frame0.
- Port `GetObjectMatrix` refreshes the live pose and returns an owner-local
  bone matrix. Gold's emitter has constant authored TRS, including rotationX
  `804/256` and uniform scale `87/256`. No arbitrary identity emitter matrix,
  extra root multiplier, larger speed or different lifespan is justified.
  The original Pulse also obtains an object point at `4039da` before later
  refreshing its matrix; a complete initialization/first-spawn trace is needed
  to compare all of those boundaries rather than assume their inputs.
- The preview's sector-less `GetWalkHeight` falls through to0 when neither
  legacy window nor current-map sector exists (`mappane.cpp:1969`). Native's
  nine visible plates have actual walk data. A matched PNG does not measure
  their collision height. Native owner z16 versus relative preview z0 might
  describe equivalent ground, but that equivalence remains unmeasured.
- Snapshot FRAMERATE24 and integer41ms timer are compatible with the observed
  yellow24.4 FPS display. PPS's1/24 coefficient and AVI tail duration are not
  independent retail scheduler proof. No20Hz clock change is warranted.

The next useful probe is a first-ten-tick original-x86 Gold replay using its
decoded actual emitter poses and the real fixture's measured walk heights,
compared with logged port birth position, transformed velocity, gravity,
collision and age. Keep initialization, pre-refresh point, current refreshed
matrix, cached expression frame and render-time submission as separate inputs.
This would distinguish simulation/state mismatch from projection or fixture
collision without fitting native arc/tail measurements. The bounded audit
starts no engine, renderer, compiler or guest process and changes no source.
Gold remains explicitly deferred while work can proceed to CombatFlash.

## Pre-punctuation/cache integration and retained mismatch, 2026-10-05

Root's includes-fixed build exits 0 (`research/gold-preview-20261005/build-includes-fixed.log`). The real preview adapter `src/effects/goldpreview.h/.cpp` creates exact generic Gold with a positive final identity and the actual default animator/controller route. It advances a single lifecycle through the port map's NextFrame/Pulse order and submits without another simulation tick. 9 extracted production clock/SafeRef cases pass under ASan/UBSan. The include-only correction is separately recorded; source files are now changing for the subsequent flags/frame fix, so the old proof hashes describe their retained checkpoint, not the unbuilt correction.

`research/gold-authored-20261005/validation.json` records 14,070 focused exact-profile/parser/state/cleanup contracts. It explicitly does not claim raw x86 Gold-state execution, float-bitexact parity, compiled GPU execution inside that pure checker, or overlapping multi-owner interleaving. The corrected route uses one default animator; there is no `gold_reference` component. Base-flare and alpha coin blending remain separate, with the per-owner controller-before-base ordering path implemented.

The [static fixture](/Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/gold-generic-static-20261005/command-capture/manifest.json) passes 4 rows, temporal animation and source natural cleanup with 40 exact-ground tail images. It contains no MOVE or DELETE; inherited labels are corrected. The separate [MOVE/DELETE fixture](/Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/gold-generic-move-delete-20261005/command-capture/manifest.json) passes 8 rows, visible original/moved animation and 88 exact-ground tail images. Both retain binary `362b6c90ef5959a8133ae145bbf2f7f06801ad7395903e894fcd1c4f083c551d`, fixture/module/commands/log/timing/image hashes and one default animator. The corrected move/delete fixture hash is `437d064a5c5524e411e370dc893b6c6b6db95bb09d8f27deda163531fbecc3eb`; its prior metadata hash is retained with the correction reason.

The [actual authored-tag pair](/Users/benjamincooley/RevenantRetailLab/captures/ab/sw-fps-gold-authored-tags-20261005/manifest.json) is complete with 240 frames, binary `28ba1e9d4783889d971bc05c6bcadeb5327fc57ecba575452c41212fa4af1ed3`, both backgrounds 0 and no fitted size, color, position, clock or seed. Root's `gold-active-zoom.png` SHA256 `fdc2ee614468fb8d7b0f39f767af6930f927ee7d6949497244b2012c2e261baa` shows a concrete mismatch: native early additive flare is absent and port coins are fewer/smaller with a different distribution. Source-backed punctuation and cached animator-frame corrections are underway. No visual credit, driver equivalence or fitted correction is granted by this pair.

The [second native recording](/Users/benjamincooley/RevenantRetailLab/captures/runs/sw-fps-goldeffect-repeat-20261005/manifest.json) is independently clean, with 38 derivative images and exact natural/delete/restore ground. It adds no second type/reference row. Its corrected pair has not completed, so repeatable-A/B coverage remains unchanged. Natural pickup/story, original setting, multiple-owner ordering and complete device/raster fidelity remain open. Capture teardown still needed termination after complete output.

Post-Gold checks pass 16 particle, 108 transform, 8 host VFX tests and 34,174 software-helper source/equation cases. Source helper report wording is corrected to distinguish pure equations from actual matched-ambient VFX/GPU evidence, without a production edit. The [integrity audit](/Users/benjamincooley/RevenantRetailLab/research/ledger-reconciliation-20261005/gold-authored-integration-integrity.json) verifies the terminal runtime/pair/native files and source/test manifest hashes. The [authoritative burn-down](../EFFECT_BURNDOWN.md) still tracks all 176 required retail entries with zero full acceptance; easy-first is an ordering policy, not a smaller finish line.

## Corrected build/runtime/repeated-pair evidence, 2026-10-05

Combined build exits 0, binary `60026cf18d66e044ce505a337e10afd4f9506266d9b880e7607349e62345755b`. The source-owned correction section above remains the authority for literal flags/matrix/cached-frame behavior; this section records root's terminal integration evidence. 720 matrix/corner comparisons, four cache/Animate cases, 14,070 focused contracts and 9 preview cases pass. Particle 16 and transform 108 checks also pass. None is a full GPU/retail bitexactness or natural-context claim.

The new [static fixture](/Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/gold-generic-static-retail-flags-fixed-20261005/command-capture/manifest.json) passes 4 rows, 40 exact-ground tail frames and one default animator, confirming natural absence by tick60. Its actual log observes 16 particles at owner frame1 and 25 by ownerframe3, matching the independently recovered cached-frame dependency. The [MOVE/DELETE fixture](/Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/gold-generic-move-delete-retail-flags-fixed-20261005/command-capture/manifest.json) separately passes 8 rows and 88 exact-ground tail frames. Their labels distinguish temporal/natural cleanup from explicit movement/deletion; no fictional Gold component is asserted.

Both [fixed first-reference pair](/Users/benjamincooley/RevenantRetailLab/captures/ab/sw-fps-gold-authored-tags-fixed-20261005/manifest.json) and [fixed independent-reference repeat](/Users/benjamincooley/RevenantRetailLab/captures/ab/sw-fps-gold-authored-tags-repeat-fixed-20261005/manifest.json) complete 240 frames with backgrounds 0, raw elapsed timing and no phase/size/color/seed fit. Their retained `gold-active-zoom.png` and `active-frame-metrics.json` are hashed. Recovered flags restore the early additive flare and coin geometry/density. **Root review remains mismatch:** both native random seeds have image apex y40 and last visible elapsed frames53/54, while port has y75 and ends at frame45. These sampled pixel/frame measurements neither justify fitted simulation changes nor establish full random-stream equivalence. Remaining source audit is read-only and bounded.

[Cure regression](/Users/benjamincooley/RevenantRetailLab/captures/ab/sw-fps-cure-post-gold-regression-20261005/manifest.json) on the same corrected binary has 240 raw preview files exactly matching the previously passed final Cure pair. The Gold-only pipeline branch preserves existing isolated Cure appearance; no new linked-character/story/context credit is given. The [fixed-checkpoint integrity record](/Users/benjamincooley/RevenantRetailLab/research/ledger-reconciliation-20261005/gold-retail-flags-fixed/integrity.json) freezes source validation snapshots and verifies all terminal frame/media/fixture/module/log/timing/reference hashes.

Gold now proves operational repeatability across two independent native recordings, so that gate rises once for the same retail row. It does not add a reference row, another paired-effect row or visual pass. Natural pickup/story, original setting, positional lighting, multiple-owner transparency and final device/raster equivalence remain open. All 176 retail entries remain required unfinished work in the [burn-down](../EFFECT_BURNDOWN.md); easy-first is work order only.

## Integration queue after bounded deferral

Gold is deferred from the easy queue, with every built fix, real runtime check and repeated pair retained. The [remaining-mismatch audit](/Users/benjamincooley/RevenantRetailLab/research/gold-preview-20261005/remaining-mismatch-audit.json), SHA256 `ccb30d04909c22e2d7a584e5a3cca8eb7fa0d6aff50fb32c8588e2f4f1b49fd5`, grants no visual/full credit and proves no further root/projection multiplier or timing/velocity/geometry edit. Current owner state/frame emitter matrix sampling remains source-consistent; cached animator frame controls expression lookup. Retail's earlier emitter point read (`4039da`) and later refreshed matrix (`403cd9`, copy `403cf9→40ef80`) are separate inputs for the future first-ten-tick replay.

Return with decoded actual emitter poses, measured native and loaded-sector collision heights and position/velocity/gravity/bounce traces. The sector-less preview returns walk height 0; matched PNG ground is not collision data. Neither the two native seeds' image y40/tail53–54 nor port y75/tail45 warrants fitted height, scale, velocity, lifetime or clock changes. CombatFlash state0/start1 advances next, with a staged draft under review and no new live code credit. Gold remains mandatory unfinished among all 176 required retail effects; counts and the existing operational repetition credit remain unchanged.

## Current mode-isolation integration status

[Same-binary isolation](/Users/benjamincooley/RevenantRetailLab/research/kinsecretdoor-20261005/gold-mode-isolation.json) is complete on binary `69d7dc13558990d33b6590454db207190f67fd7674921bf665f177e6c3d5b89d`: classic, modern and omitted/default auto each 240 raw frames exactly match one another and retained 60026/f4 corrected Gold output. [Frame/media verification](/Users/benjamincooley/RevenantRetailLab/research/ledger-reconciliation-20261005/gold-mode-isolation/integrity.json) checks all 720 images and associated media/timing/reference hashes. Lighting mode is not the cause of the earlier 44-frame a89 difference. That predecessor reproduced itself; its cause remains historical unresolved evidence, with no physics/material/render fix inferred.

This resolves current render-regression isolation only. It neither fixes nor accepts the original retail arc-height/ground/visible-duration discrepancy. The decoded actual-emitter replay, measured collision heights and first ten ticks of state/bounce remain the bounded follow-up, with no fitting. `--vfx-lighting-mode` is documented in the [port diagnostic guide](../../RETAIL_AB_TESTING.md); default auto behavior is preserved. Effect counts, operational-repeat credit and all 176 required unfinished rows remain unchanged.

## 2026-10-05 projection/depth proof scope correction

The720 matrix/corner oracle retains valid local-matrix evidence, but both localmatrices were projected using the same host formula. Actual retail view/projection/root/bitmapdepth was not proved by it. The [new executed software-depth/projector audit](RETAIL_SW_PROJECTION_DEPTH_LIMITS.md) establishes controlled source basis and constant signedMAX triangle depth separately. Full authored corners and sampled native floor depth remain missing; no Gold/teleport per-effect cause, projection multiplier or fittedheight/time change is justified. Localmatrix/cachedframe fixes and operational evidence stay valid; visual and natural/context acceptance stay open.

## 2026-10-05 raw-center/local-model Z handoff, A/B pending

Root applies the [shared source Z bridge](PARTSYS_Z_DOMAIN_HANDOFF.md) in only3dimage.cpp: preserve raw particle simulationcenter, add local transformedMODELoffset×existingWORLD3D_Z_SCALE1.5, then caller render_scale.Z. Source target31e59323…/patchbf191b3b… builds0 binarybb0a0f1c…. Physics, velocity, timing, XY, UV, colors and shader/camera are unchanged; initial fullcenter1.5proposal remains archived/notused.

36corner/9center checks validate the controlled source/math contract only, not compiled production bridge/GPU/live actualowner or final image. Actual-retail localZ basis still differs by5.118%; no fullprojector/floor/cadence acceptance or fittedheight claim. Root is acquiring new Gold pairs against both retained native references; no new A/B or appearance credit until terminal review. Source-fix row count stays fixed because Gold already has a recorded source row. All earlier arc/collision/duration and naturalcontext deferrals remain.

## Terminal post-Z overall appearance pass

Both currentbb0a own240-frame Gold/reference pairs rootPASS overall coin shape/scale/color/spray/higharc with independentrandomseeds. Source bridge only changes renderworld Z; count/emission/gravity/velocity/physics/time unchanged. Porttop39/bothretail40 vsold75 under mask>8; last42/43native vs45port is separate from preserved53/54historical metrics. No fittedphysics/geometry/phase.

Currentnatural4/MOVEDELETE8checks/40+88groundtails pass. [Final shared integrity](PARTSYS_Z_DOMAIN_HANDOFF.md) also retains Combatinconclusive, fourwater runtime-only protection and exactCure regression. Exactnativecadence/fullcycle/realwalkheight/collision/caster/audio/context/full remain open, including sectorlessFloorCallback0 limitation. Two currentreferences strengthen alreadycounted repetition, no duplicate row. Historicala89cause remains separately unresolved.
