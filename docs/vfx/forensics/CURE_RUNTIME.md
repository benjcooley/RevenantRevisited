# Cure null-spell runtime implementation

Current status, 2026-10-05: built source-state implementation, actual null-spell lifecycle and isolated retail visual appearance pass. Real linked-character/SpellData caller and full-context acceptance remain open. The source ownership/integration notes below retain their historical phase scope.

Source ownership: new `src/effects/cure.h/.cpp` only. Registration and retail
function/asset proof are in CURE_RETAIL_AUDIT.md. Visual acceptance is false;
engine compilation, map lifecycle and matched software-reference captures are
root-owned integration work.

## Integration API

Include `effects/cure.h`. `TCureEffect_Bespoke` implements:

- `Initialize(bool attach=true)`; borrowed actual owner imagery and one
  `cure_reference` component when attached.
- `SpawnForTest_BESPOKE(origin)`; loads the canonical `Magic\\Cure.I3D`,
  initializes without a globally updated component and never retriggers.
- `Advance(seconds)`; separate 24 Hz accumulation and simulation.
- `Submit(EFxDebugMode)`; helper-mesh world submission, no simulation/RNG.
- `TickAndSubmitForTest_BESPOKE`, `IsAlive`, `SimulationTicks`,
  `VisibleParticles`.

The preview's tick callback must call `Advance(TTime::DeltaTime())` only.
Its separate `submit_world` callback calls `Submit(Normal)` after
`BeginTilePass`. Calling the combined method as both callbacks would double
the clock. The helper path does not implement the FX debug shader ladder;
nonnormal modes explicitly submit nothing, rather than replacing authored
color or geometry.

The object builder is named Cure; the typed animator attachment additionally
requires EFFECT class, exact ID `0x152dafef`, positive final MapIndex, no kill
flag and no existing Cure component. It reports an incompatible generic owner
instead of manufacturing another owner/imagery instance. The component
replaces the static default visual, including the two unused color meshes.

## Preserved state and source boundaries

The portable `cure_retail::State` retains five ball records and 80 swirl records
of 84 bytes each, source field layout, states, integer counters and ordered
traversal. Initialize consumes four delay draws exactly once. Equal random
endpoints return the endpoint without consuming the random stream. The
simulation retains source same-tick activation/update, ordinal ball selection,
unused distance-phase draw, folded retail degree multiplier, matrix orbit,
literal OR shrink branch, delayed clamps, one death record per legacy tick,
and natural completion at kill index 80. It has no emission retrigger or
offscreen owner kill.

Object 0 alone supplies the extracted authored positions, normals, UVs and six
indices. No camera billboard, generic glow quad, extra guide-ball draw, tint,
light, fitted size or fitted lifetime is introduced. All diffuse, ambient,
specular, emissive and power fields are forwarded unchanged from its material.
One shared immutable GPU mesh is registered lazily when its texture is ready;
per-particle submission contains only an instance transform and materials.

The Render matrix calls follow retail: fixed X rotation, fixed Z rotation,
negative initialized facing rotation, uniform size, particle local position,
then the live owner transform. The CPU simulation stays local. It neither
stores the owner's map coordinates in each particle nor adds them a second
time. Matrix memory is converted once for the renderer's column-vector rows.

The helper-mesh API has a distinct mesh projector. Its initial material shader
produced white rectangles through texture-independent specular; that failed
capture is retained. The explicit software helper mode now provides source
five-bit vertex light and texture modulation without mutating materials; see
CURE_HELPER_SOFTWARE.md. The procedural FX projector's Z conversion must not
be applied blindly to this mesh path. No ad-hoc brightness/Z factor is added.
The current owner matrix's WORLD3D_Z_SCALE enters once; parity with the retail
view/world stack remains a separate integration/reference gate.

At natural completion the owner is marked through the existing
`KillThisEffect`/`OF_KILL|OF_PULSE` lifecycle. No owner is deleted while the
global component update list is traversed. Runtime logs record attachment,
first simulation tick and final frame/kill index for stable-identity timeline
assertions.

## Linked-spell predicate correction

Retail Cure's first owner Pulse can move to a fighting/nearby target and set
poison, while its natural end conditionally clears target poison. That end
condition was initially misclassified as a variant word at `+0xc8`. Subsequent
constructor/parser/accessor proof establishes SSpellData.poisonchance instead;
see CURE_SPELL_DATA.md. That default/tag/accessor is now restored, along with
the source first-Pulse and conditional target-zero cleanup. Later actor reads
use constructor-captured weak identities. No unconditional/name-based cure
predicate or probability clamp is introduced.

Null-spell editor/preview aiming remains explicitly zero. Real character Cast
acceptance still requires the parent's actor/owner runtime fixture; the shipped
CureP definition actually binds Streamer, so a linked bespoke Cure test must
be described as a synthetic binding. The original heal sound launch is outside
this bounded visual module; no audio equivalence is claimed.

## Source-state checker

`/Users/benjamincooley/RevenantRetailLab/check_cure_state.py` prepares an
independent C++ oracle by extracting the original snapshot TCureAnimator
Initialize/Animate bodies and constants. The adaptations are retained in
`research/cure/state-validation.json`: graphics/owner hooks become null-spell
stubs, matrix helpers have an independent general matrix implementation,
original loop scope is made modern-C++ compatible, and angle conversion uses
the separately recovered retail folded float. The production state's compact
orbit implementation is not copied into the oracle.

The parent executes `python3 .../check_cure_state.py --compile`. It compiles
only the portable state section (`CURE_STATE_ONLY`) with ASan/UBSan and compares
all 85 records, counters, RNG stream and natural death across 64 seeds. Integer
and RNG state must agree exactly; float positions use a recorded bound of
max(1e-5 absolute, eight binary32 ULPs). The result and source/oracle/binary
hashes are retained in the same manifest. Preparing the checker alone is not
a passing state-validation claim, and a passing checker is not retail visual
acceptance.

No compiler, full build, port render or guest input was run by this child.
Root owns those actions and their binary/configuration capture manifests.

## Final root integration evidence, 2026-10-05

Final binary `e829e8dbd53c8d4d26f7c82f21159147137d0905181d2591bf9cde7670764870` passes both real Cure command fixtures: five natural-expiry rows and six explicit-delete rows, visible animation before/after MOVE, one reference component and 14/29 exact-ground tail frames. Manifests are under `lab/captures/runtime-fixtures/cure-{natural,explicit-delete}-software-helper-final-20261005/command-capture/`. No linked-character fixture has executed.

The final `sw-fps-cure-software-helper-final-20261005` pair contains 240 source-helper preview frames and zero background drift on both sides below the recorded FPS band. All 240 are byte-identical to the prior root-reviewed software-helper pair and its hashed zero-red GPU check. Root accepts overall isolated blue-glow shape/size/spread/motion across independent RNG, without fitted position/scale/color/phase. This does not accept original actor/target, point-light, story/audio or full map context.

The retained source-state audit passes 8,358 ticks over 64 seeds (14,960,820 checks). Current `CURE_SPELL_DATA.md` and `research/cure/spell-data/validation.json` separately establish 625 sanitized parser/probability/cleanup/weak-actor contracts and actual SpellData POISONCHANCE semantics. Full actor/spell execution is pending; shipped CureP points to Streamer, not literal Cure. Historical preview crash, white rectangles and failed all-ground map output remain uncredited diagnostics. Completed capture processes needed termination during stalled teardown. See the authoritative [burn-down](../EFFECT_BURNDOWN.md) and [integrity audit](/Users/benjamincooley/RevenantRetailLab/research/ledger-reconciliation-20261005/cure-firecone-final-integrity.json).
