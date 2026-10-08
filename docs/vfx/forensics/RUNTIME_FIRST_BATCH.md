# First reference batch — runtime dispatch audit

2026-10-04. This audit follows actual object creation/loading and map
submission, rather than the VFX browser's separate `SpawnForTest` factories.
Line references describe the working source at audit time. No guest was
controlled or game state changed for this audit.

## Shared dispatch and ownership

The authoritative entries are in `data/imagery.rvi:class.def`:

- Flame base `0x50ba373b`, `Magic\Flame.I3D`, line 3362.
- Mist `0x2093487a`, `Magic\Mist.i3d`, line 3400.
- CyanFont `0x22491405`, RedFont `0x335a2516`, GreenFont `0x446b3627`,
  BlueFont `0x557c4738`: `Misc\Sparkle.I3D`, lines 3350–3353.
- Fizzle `0xab8800dd`, `Magic\Fizzle.I3D`, line 3407.

`TObjectClass::AddType` (`src/object.cpp:2850`) resolves a named object
builder, falling back to the class builder. `FindObjType(uint32_t)` at 2963
resolves saved type IDs; name lookup at 2934 is case insensitive. The load
path at 2141/2194 builds the object and at 2224 loads its serialized body.
`TObjectInstance::Load` at 2438 applies the saved map index. Runtime
`TMapPane::NewObject` (`src/mappane.cpp:1357`) also changes identity while
placing the instance into its sector. Components must attach after these
identity changes, rather than registering updates in the object constructor.

The map drawable build calls `T3DImagery::AttachAnimatorComponents`
(`src/maprenderer.cpp:2032`). That method resolves the type-name animator
builder, falling back to the class/default builder (`src/3dimage.cpp:2046`).
The default `AttachComponents` is empty (`src/3dimage.h:355`). Old named
registrations inside `src/effect_old.cpp` do not count: the entire file is
under `#if 0` starting at line 10.

An attached `TFlipbookBillboardComponent` can replace the static asset and
submit arbitrary authored quads via its virtual `Submit`, despite its
historical name (`src/maprenderer.cpp:953`, 2039, 1081). This existing
boundary is the smallest safe integration point for the reference ports.
Update registration is owned by the component, and `AddComponent` attaches
and activates it (`src/object.cpp:790`). The water implementation at
`src/effect.cpp:3873–3943` demonstrates separated update/submit, idempotent
attachment, borrowed owner imagery and attachment after stable identity.

## Flame base: runtime connected, acceptance open

Active `DEFINE_BUILDER("FLAME", TFlameEffect)` exists at
`src/effect.cpp:1282`. The serialized-object constructor deliberately does
not attach early (`src/effect.h:580`). `TFlameAnimatorComponentBuilder`
at `effect.cpp:1360` attaches the visual through `AttachVisualComponent`.
That method checks for an existing visual and loads the actual owner imagery
(`effect.cpp:1290`). `TFlameQuadComponent::Submit` at 1246 uses owner position
and full transform, authored corners and atlas UVs.

The port's actual City map regression retained eight changing cels after the
owner-identity repair. This is runtime animation evidence, not full visual
acceptance. Next: repeat software capture on the shipped mounting, preserve
its lighting and verify occlusion/shared camera projection. Colored Flames
share the literal name `Flame` but separate type IDs; this audit covers only
the base ID.

## Mist: preview connected, runtime visual missing at audit

Active `TMistEffect::SpawnForTest` (`src/effect.cpp:4262`) explicitly attaches
a particle component; the separate canonical bespoke factory at 9100 seeds
its own reference state. Neither method is called by the actual object
registry. There is no active named Mist object/animator registration; it
falls back to generic `TEffect` (`effect.cpp:1135`) and default animator.
Consequently a loaded/created Mist object does not inherit the preview's
50-particle authored rendering or timing simply because its I3D loads.

No Mist command caller was found in the nine shipped Ahkuilon `.s` members;
this does not rule out saved map placements, which still need decoding.
The fresh retail editor reference proves retail can instantiate the type.

Smallest next implementation: factor canonical Mist state initialization,
advance and submission away from its sector-less factory; attach an owned
reference component through a named Mist animator hook after stable identity.
Keep software diagnostic compositing explicitly selectable; see
[M05_SOFTWARE_REFERENCE.md](M05_SOFTWARE_REFERENCE.md). Then load/create the
real type ID in a map and verify movement, lifecycle and cleanup.

## Four Fountain colors: shared missing hook at audit

The four original animator registrations at `effect_old.cpp:3909–3927` are
disabled. Active bespoke rendering (`effect.cpp:12857`) and shim state
(`src/effects/fountain.cpp`) are called from `src/vfxtest.cpp`, not from
runtime dispatch. Generic `TEffect` is the correct object shell, but the
default animator does not create the ten-bubble particle state. All four
types therefore need a named runtime animator/component hook; no new retail
object type or guessed general-effects definition is required.

The selected authored subobjects are exact: Cyan=0, Red=1, Green=2, Blue=3.
Bind owner imagery, preserve its subobject material/UV/geometry, own one
shim state per instance, advance through the component update list, and
submit using the current owner transform. Reusing a standalone preview
owner would make movement/rotation and destruction diverge from map state.

Natural shipped script evidence was read directly from compressed
`data/Modules/Ahkuilon.rvm`:

- **CyanFont:** `forest.s:246`, Styxx tomb unlock:
  `ADDAT 4268 23080 105 CYANFONT`. This requires runtime creation at the
  authored X/Y anchor and a persistent effect. Retail ADDAT inherits map-center Z;
  its third numeric argument is amount, not height.
- **RedFont:** `dungeon.s:782–785`, Sabu vanish uses `add redfont`, moves
  it up 100 units, waits 24 ticks, then deletes it. Lines 903–906 also use
  `ADDAT 10001 9458 144 redfont`, move, wait 48 and delete. Movement and
  explicit script deletion are part of the acceptance case.
- **GreenFont/BlueFont:** no caller in those nine scripts. Keep them as
  shipped registered variants; verify actual map creation/loading by their
  IDs and investigate saved placements before declaring them unused.

The runtime component hook is the immediate narrow implementation task.
Natural Cyan/Red story acceptance has an additional script-command gap:
`src/script.cpp:291` routes through `CommandInterpreter`, but the active
command table initially lacked `addat`. The implementation added below
restores the proven numeric form; authored story execution remains untested.
`CmdAdd` uses map-camera position, matching retail handler 0x4213c0;
there is no evidence for replacing it with the passed object position.
The subsequent scoped move/delete commands remain separate lifecycle gates.

## Fizzle: natural spell route exists, visual/init gaps remain

`TPlayer::InvokeQuickSpell` (`src/player.cpp:547`) calls `CastByName("Fizzle")`
for an empty slot, missing talismans or failed cast. The active route is
`TCharacter::CastByName` (`src/character.cpp:5239`) →
`TSpellManager::CastByName` (`src/spell.cpp:492`) →
`TSpell::Timer` (426), which resolves `variant->effect` and calls
`MapPane.NewObject` (454). `data/Resources/spell.def:1395–1409` includes the
Fizzle variant with effect name `fizzle`, invoke2, delay 5 and wait 20.

Two concrete blockers remain:

1. No active Fizzle object/animator visual registration. The current
   `TFizzleEffect::SpawnForTest` (`src/effect.cpp:3435`) does asset/state
   initialization, and `TickAndSubmitForTest` (3557) drives it only when
   the harness invokes it. Ordinary factory creation uses generic TEffect.
   Registering a typed constructor alone would still omit that setup.
2. No call to `SpellList.Initialize()` or `SpellList.Load()` was found in
   active `src`; `TSpellList` starts empty (`src/spell.h:80`) and its getters
   do not lazily load data (`spell.cpp:260`, 339). CastByName rejects a missing
   spell/variant. Startup flow must initialize the actual spell list before
   claiming the natural trigger works.

Smallest next visual implementation: split existing Fizzle initialization,
advance and submission into the typed owner/reference component, attach via
the stable-identity animator hook, and kill the actual map owner after the
last particle dies. Preserve its spell association and current transform.
The preview's one-shot IsAlive behavior is insufficient proof of runtime
deletion. Then exercise an empty quickspell through the real loaded player
and verify delay, single burst, colored sprites and removal, after spell
initialization is repaired.

## Acceptance policy

Only Flame currently demonstrates an active map visual among these seven
rows. The remaining previews establish implementation candidates, not
natural runtime completion. Component-hook work closes one gate; it does not
resolve renderer blend, shared projection/light/depth, story command
semantics or natural-trigger evidence. Use normal type-ID creation and
serialized loading in addition to isolated preview captures.

## Follow-on Fountain runtime hook

After the read-only audit above, `src/effects/fountain.cpp` gained four
`TFountainRuntimeBuilder` hooks using the exact type IDs/subobjects above.
They attach an owned `TFountainRuntimeComponent` through the existing lazy
animator attachment path. It binds the actual owner's imagery, preserves
the shim's initialization/animation bodies, updates through the component
registry, and submits quads with the owner's live transform. The visual
replaces the generic static Sparkle mesh.

Attachment checks final map identity, expected EFFECT type ID and an
already-attached Fountain component before seeding. Texture readiness is
checked before creating state, avoiding discarded RNG draws during upload
retries. The runtime state borrows owner imagery; components are destroyed
before that imagery is freed (`src/object.cpp:670–690`). Standalone shim
state owns/releases its independently loaded imagery reference. Component
detach unregisters updates through the existing base lifecycle.

The component logs one attachment and one first simulation tick per instance,
including type ID, map index and color. Syntax and unchanged source-dynamics
checks passed. The integrated build and actual serialized-sector regression
now pass. `~/RevenantRetailLab/fountain_runtime_fixture.py` clones a genuine
shipped BlueFont body into an owned temporary module, preserving real load
semantics and substituting the four type IDs and explicit test positions.
It loads nine ground tiles and four EFFECT objects through `--test=sector`.

Evidence is in
`~/RevenantRetailLab/captures/runtime-fixtures/fountain-four-20261004/`.
The manifest records binary SHA-256
`4143e05d210817f1bf596a4891251b3f179373a61f0b901fe202f717bab648a4`,
module/sector/asset provenance, four distinct positive MapIndex values,
exactly one attachment and first simulation tick per owner, and **73 changing
ROI images per color across 90 frames**. The representative frame shows
cyan/red/green/blue left to right on real loaded ground geometry. The first
test module lacked `area.def`; that failed run was retained, and the successful
module includes the shipped definition verbatim.

The owned process completed its filmstrip, then required termination for the
known test-mode teardown stall. This proves actual sector loading and map
animation, not destruction/scene-change cleanup. Runtime creation, movement,
deletion, natural story triggers and matched retail map fidelity remain open.


## Retail command dispatch and reusable lifecycle timeline (2026-10-04)

Retail `Revenant.exe` has SHA-256
`28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.
`recon/ghidra/cls_0x4922c0.cpp:904` is the recovered TScript continuation
at 0x4933d0: it calls interpreter 0x41e8e0 with object context, token,
abbreviation zero, and script pointer. That interpreter scans one command
table at 0x5c6e88, stride 28 (0x41ea17–0x41ea7e). It checks the editor-only
field at offset 20 (0x41eb44), then dispatches the selected handler at
0x41ed0b. Thus this is not a separate editor-only ADDAT implementation.

ADDAT's row is 0x5c6ec0: handler 0x421770, class contexts -1/-1,
requiresparams=1, editoronly=0 (`recon/classes/_data.txt:58635`). Handler
0x42179b reads X, 0x4217e4 reads Y, and 0x42182e saves an optional third
number as amount. Initial Z comes from global map-center Z at 0x666990.
After creating **one** map object at 0x421ab0, it calls virtual SetAmount
once at 0x421b53 when amount differs from one. Therefore the shipped
`ADDAT 4268 23080 105 CYANFONT` and `ADDAT 10001 9458 144 redfont`
mean amounts 105 and 144 on one owner, not Z values or repeated spawns.
The port restores numeric ADDAT through the shared existing ADD creation
path; default ADD camera semantics are preserved. Retail also resolves
identifier coordinates through scoped script state lookup 0x497800
(`recon/ghidra/cls_0x4975d0.cpp:120`). The snapshot lacks that later scoped
API; numeric support does not claim that identifier form. The unrelated
snapshot `StringVal` is a string hash and must not substitute for it.

`--scene-command-file=<path>` is an optional **sector-test-only** timeline:
`elapsed_24Hz_tick | context name/type (or -) | engine command`.
Normal commands execute `CommandInterpreter`; the test does not synthesize
Fountain movement. The opt-in timeline requires explicit scene camera and
borrows the already-loaded current map into MapPane's active window, with
no sector ownership transfer or duplicate loading. It releases those pointers
before renderer/map teardown; FreeAllSectors also guards the borrowed window.
At 24Hz it calls the actual NextFrameObjects/PulseObjects/MoveObjects paths
for authored frames, object/script pulses and OF_KILL cleanup. `@expect HEX_ID X Y Z COMPONENT COUNT` observes live
position, exact type, and exact component multiplicity. `@absent` checks a
previously bound MapIndex/type identity, preventing a misspelled context
from passing deletion. Every row logs its result and before/after object
identity, position, amount, component name, slot, and generation. Failed
rows request quit and log FAILED; consumers must require COMPLETE and zero
FAIL rows rather than treating process exit alone as success. Timelines
must be sorted by tick. No flag leaves the existing sector path unchanged.

Preparation helper `~/RevenantRetailLab/prepare_fountain_command_fixture.py`
creates an owned ground-only module and commands file under
`captures/runtime-fixtures/fountain-command-20261004/`. All four fountains
are created through ADD/NewObject, moved through scoped MOVE, checked for
one component after movement, deleted through DELETE, and checked absent.
A Fizzle ADD/NewObject row verifies its one-shot natural component cleanup;
it does **not** prove spell casting. Mist is also created through ADD, moved and checked persistent at tick90,
then deleted and checked absent at tick93. Run at least 94 legacy ticks to execute
all assertions, retain log and binary/fixture hashes, and let the parent
serialize the renderer. The execution below passed. This isolates lifecycle
regressions; authored in-situ map A/B remains required for visual signoff.

Startup now initializes SpellList after Rules and closes it before Rules;
those APIs already guard repeated calls. This removes the observed empty
spell lookup initialization gate. It does not prove character-driven spell
execution or the separately audited TSpell lifetime repairs.


## Current lifecycle result: PASS (2026-10-04, 21:09 local)

The prepared ground-only command fixture was executed with the rebuilt port,
SHA-256 `517788209cd912c2556c27a142474b5f167d6238dda753b7a3359db4e23a04fc`.
Evidence: `~/RevenantRetailLab/captures/runtime-fixtures/fountain-command-20261004/command-capture/manifest.json`,
`run.log`, 130 PNGs, filmstrip, and `lifecycle-review.png`.
The manifest retains binary, fixture, command-file, log and image hashes.
Every one of **38** engine-command/observation rows returned PASS, ending
`[scene-command] COMPLETE rows=38 failures=0`.

The initial map contained exactly nine ground tiles and no effects. CyanFont
was created by ADDAT with amount1; the other colors, Mist and Fizzle by ADD.
Thus the result uses actual `MapPane.NewObject` and source builder dispatch,
not a serialized effect record or SpawnForTest. Exactly one Fountain runtime
attachment occurred per color, using IDs 22491405/335a2516/446b3627/557c4738
and positive final MapIndexes 268435466/468/470/472. Position checks passed
after authored command placement and after real MOVE +20,+10,+40. Each
owner still had exactly one `fountain_reference` component after movement.
All four explicit DELETE commands succeeded at tick48; stable bound identity
checks found them absent at tick49.

Mist attached once as ID2093487a, MapIndex268435474, with 50 drops. It moved
from (10000,10000,16) to (10020,10010,56), retained one `mist_reference`
component at ticks13 and90, and was deleted at92 and absent at93. Fizzle
attached once as IDab8800dd, MapIndex268435476, finished naturally at its
simulation frame31, set OF_KILL, and was absent in the real map registry by
tick62. The normal PulseObjects/Nuke/DeleteObject path reaped it; no test
command explicitly deleted Fizzle or retriggered its particles.

The image sequence has 93 distinct full frames. Visual review confirms
changing cyan/red/green/blue effects early, Mist continuing after Fountain
deletions, and only ground after Mist deletion; the final nine frames are
identical ground images. This supports the command lifecycle observations.
Component ownership detaches/unregisters before object imagery is freed
(`src/object.cpp:670–690`), and later frames continued after deletion without
stale updates or a crash. There is no instrumented global registration-count
assertion; the evidence is live owner/component multiplicity, deletion from
registry, source ownership order and stable post-deletion playback.

SpellList startup/load succeeded without a grammar or missing-file error.
The runner waited for the completed filmstrip, then terminated and reaped
its **own** port process after it failed to exit promptly (exit -15). This
remains a shutdown/test-harness issue; successful normal shutdown was not
proved. The render slot was released before further parent work.

Relevant current source: command.cpp:1672/1797 (shared ADD and numeric
ADDAT), mappane.cpp:3890/3910 (borrow/release), mappane.cpp:3860 (ownership
guard), testmodes.cpp:3180/3238 (timeline and real map tick),
revmain.cpp:2174/2255 (SpellList lifecycle). Syntax checks passed for all
changed translation units; the integrated parent build passed.

The earlier missing-attachment and missing-command observations above are
retained as historical audit findings. **Current command/runtime lifecycle
regression is verified; authored story triggering, character-driven Fizzle
casting, matched in-situ retail rendering and visual fidelity remain open.**
This result does not convert synthetic fixture evidence into final effect
acceptance or override the in-situ A/B requirement.
