# Gameplay / Combat Agent Guide

Protocol for an agent (or the coordinator) picking up a slice of the core
gameplay restoration: combat, the character state machine, the
animation-event system, movement, AI, and input. Sibling of
[../vfx/AGENT_GUIDE.md](../vfx/AGENT_GUIDE.md) (effects) and
[../ui/AGENT_PROTOCOL.md](../ui/AGENT_PROTOCOL.md) (panes/widgets) — same
forensics-first discipline, different subsystem.

The burndown is [BURNDOWN.md](BURNDOWN.md). That's your assignment board.

## 1. Before you start

Read, in order:

1. [BURNDOWN.md](BURNDOWN.md) — what's open, who owns what, what's been handed to the gameflow track.
2. The design docs for the system you're touching: [../COMBAT.md](../COMBAT.md), [../COMBAT_ATTACKS.md](../COMBAT_ATTACKS.md), [../COMBAT_RULES.md](../COMBAT_RULES.md), [../MOVEMENT.md](../MOVEMENT.md), [../ANIMATION.md](../ANIMATION.md), [../ANIMATION_SYSTEM.md](../ANIMATION_SYSTEM.md), [../OBJECT_SYSTEM.md](../OBJECT_SYSTEM.md), [../OBJECT_STATS.md](../OBJECT_STATS.md).
3. [../ui/AGENT_PROTOCOL.md](../ui/AGENT_PROTOCOL.md) — **the shared Ghidra recon rigor** (mislabel cost asymmetry, golden-path string anchors, the "a decomp class may = multiple src classes" caveat, single-writer Ghidra lock). It's written for UI but the rules are subsystem-agnostic; any time you touch the retail binary you follow it. Don't duplicate it here — read it there.

Standing project rules these all touch (memories): no bodges; modern C++ at touch sites; preserve retired code in `attic/` or `#ifdef UNUSED`; retail-first when recon exists; minimal includes; no raw threads; logging facade not `fprintf`. The combat code especially is full of `PT*` typedefs and macros — clean at touch sites, don't sweep.

## 2. Source-of-truth order

For any combat / movement / AI / animation behavior, in priority order:

1. **Retail decomp** — `recon/classes_readable/TCharacter.cpp` (cls_0x5a5320) and siblings, produced by the pipeline in [../../recon/docs/PIPELINE.md](../../recon/docs/PIPELINE.md). Ground truth for *behavior*. Most combat classes are NOT pre-extracted — you'll decompile on demand (see §4).
2. **Pre-release source** — `src/character.cpp`, `src/player.cpp`, `src/charanimator.cpp`, etc. Useful for *intent* and structure, but the snapshot predates retail by ~6 months; combat tuning, the animation-event wiring, and AI shifted late. **Pre-release is where the port lives, but retail wins for behavior.**
3. **Existing port shell** — what's already compiling in `src/`. Extend, don't rebuild.

When retail and pre-release disagree, retail wins for behavior; **note the divergence** in the forensic write-up — the divergence itself is information (it's usually a late-cycle balance or bug fix).

## 3. Forensics first (MANDATORY pre-step for every behavior)

**Before writing port code for a combat / movement / AI / animation
behavior, complete the forensic write-up.** Same hard rule as VFX: we
don't ship "plausible" mechanics, we ship behavior grounded in retail
evidence with explicit notes on deliberate deviations.

Forensics for a gameplay behavior produces a `docs/gameplay/forensics/<topic>.md`
file (or a BURNDOWN note if it's a one-paragraph finding) covering:

1. **Constants** — every numeric the behavior uses. Ranges (hitminrange/
   hitmaxrange), fatigue costs, damage formulas + modifiers, timing
   (looptime, waitticks, nextattack, frame counts at FRAMERATE),
   turn rates, move speeds, leap/dodge distances, AI scan radius +
   cadence. Pull from pre-release `src/` AND cross-check the recon
   decomp if the class is mapped. Note disagreements.
2. **State-machine shape** — the behavior in terms of TActionBlock
   roots + actions + transitions:
   - What root(s) does it run under (ACTION_ANIMATE / ACTION_COMBAT /
     ACTION_BOW)? What's the root animation name (`walk` / `run` /
     `combat` / `comhand` / `bow` / `sneak`)?
   - What's the transition in/out (`SetRoot` / `SetDesired` /
     `ForceCommand` / `interrupt`)? Does it pivot, wait, or snap?
   - What `GetMoveAction(root->action)` resolves to and which
     `<root>f` / `<root>r` step animations exist (verify against the
     I3D state dump — see §3.1).
3. **Animation events** — the I3D `play` / `beg` / `end` tags
   ([src/3dimage.cpp](../../src/3dimage.cpp) loads them into `T3DAnimator`/
   imagery `tags`). For the behavior's animation: which (state, frame)
   tags fire, what they do (footstep sound, weapon-swipe spawn,
   impact-frame hit resolution, state-begin/-end hooks). **This is the
   subsystem most likely to be unported** — the tags load but nothing
   consumes them yet (footsteps are the canonical gap).
4. **Trigger sites** — what makes this behavior happen: a `GAMECMD_*`
   from input dispatch, an AI decision in `TCharacter::AI()`, a script
   opcode (gameflow track), a hit-resolution callback, an exit/door
   activation. Cite file:line.
5. **Data dependencies** — `rules.def` (CHAR attack/impact/stat entries),
   `class.def` (object stats, EqSlot, weapon masks), the I3D animation
   set. Quote the relevant rows. Names must be alnum-only in DEF files
   (see memory feedback-parse-no-underscore-keywords).
6. **Verification rig** — how does the test harness exercise this on
   the *real* code path (see §5)? Standalone (spawn + tick)?
   Character-attached (needs a live TCharAnimator)? Two-character
   (attacker + target for hit resolution)? Player-input-driven (needs
   a synthesized GAMECMD)?
7. **Gaps / unknowns** — what's still guessed vs documented.

Only after the write-up carries these facts do you start the port. A
port grounded in evidence is reviewable against the evidence; a port
grounded in vibes guarantees rework.

### 3.1 The I3D animation-state dump

The fastest forensic primitive for any animation/movement/combat
question: dump the character's animation state names. There's a
one-shot `[anim-dump]` in [src/playscreen.cpp](../../src/playscreen.cpp)
`SpawnDefaultPlayer` that logs all of Locke's `states[i].animname`
entries (`img->GetHeader()->states[i].animname`). It answers "does a
`run` state exist?" (yes — 233), "what are the move-step variants?"
(`runf`, `combatrunf`, ...), and which states a behavior can transition
to. Re-point it at any character to inspect their set. Keep it gated /
removable — it's a probe, not shipping output.

## 4. Ghidra recon for combat classes

`TCharacter` (cls_0x5a5320) is the big one and a prime **merging**
suspect per [../ui/AGENT_PROTOCOL.md](../ui/AGENT_PROTOCOL.md) Rule 2:
TCharacter ← TComplexObject ← TObjectInstance is a deep hierarchy and
OOAnalyzer flattens it. Expect the decomp "class" to be a super-set;
expect static helpers + bare functions (e.g. `ConvertToFacing`,
`ScreenToWorld`, damage math) to be **missing** and need on-demand
extraction.

The pipeline + scripts live under `recon/`:
- Ground truth: `RevenantGhidra/` (never edit).
- Readable: `recon/classes_readable/`.
- Scripts: `recon/ghidra_scripts/` — `DecompileAddr.java`,
  `DumpVtable.java`, `FindStringRefs.java`, `WhoRefsString.java`,
  `CallersOf.java`, `FindByName.java`.
- On-demand extraction + naming convention: [../../recon/docs/PIPELINE.md](../../recon/docs/PIPELINE.md), and the `discovered/` workflow in [../ui/AGENT_PROTOCOL.md](../ui/AGENT_PROTOCOL.md) §"Sibling rule".

The golden path for identifying a combat method is the same as UI: a
string anchor in the retail decomp (a `Status()` / error / log string)
cross-referenced to the same string in pre-release `src/`. Combat has
fewer strings than UI, so lean harder on: vtable-slot matches against
the `TCharacter` hierarchy, calls into already-identified methods
(`Damage`, `Go`, `SetRoot`), and numeric-literal density (attack
range / fatigue / turn-rate constants). **Two independent lines of
evidence before any rename.** Ghidra project is single-writer —
serialize CLI access if you fan out.

## 5. Verifying

1. **Test mode.** Prefer a `--test=<mode>` that exercises the behavior
   in isolation over booting the full game (memory
   feedback-test-modes-for-ui applies here too). Combat wants a
   two-character rig (attacker + target) so hit resolution / damage /
   death / impact-stagger have a real target. Movement wants a single
   character + a ground plane. Build the rig as a reusable primitive
   when a new category appears, mirroring vfxtest's `Rig*` helpers.
2. **Visual + log check.** Combat fires `[character]` / `[player-attacks]`
   log lines; movement + animation events should log the tag fired.
   Self-verify via screencapture (memory reference-visual-verification)
   before declaring done — "could a stranger confirm the swing
   connected / the footstep fired / the run animation played?"
3. **Ask the user.** Benjamin Cooley wrote this combat system in 1999.
   For "how was this *supposed* to feel / work" questions, ask rather
   than guess. That's a how-it-works/game-history question — in scope
   for asking per memory feedback-question-scope.

## 6. Coordination with the gameflow track

Several gameplay-adjacent systems are owned by **gameflow**, not here
(see [BURNDOWN.md](BURNDOWN.md) header + [../gameflow/README.md](../gameflow/README.md)):
save/load, scripts, exits/doors, menus, new-game, death/restart, and
the **audio backend**. Combat/movement *trigger* sounds (footsteps,
swings, impacts) but the SoundPlayer + miniaudio backend are gameflow's.
When wiring an animation-event sound, you call `SoundPlayer.Play(...)`
(theirs) from the event consumer (ours). If the play call reaches the
backend but is silent, that's a gameflow bug; if the event never fires,
that's ours.

## 7. Handoff

Leave every behavior in a pickup-able state: BURNDOWN row status
current, a one-line "where I left off", forensic file committed even if
the port isn't done (the forensics are reusable on their own). Commit
before stopping. If you find a behavior that isn't in the burndown,
add a row — over-list and reconcile.

## 8. Open forensics queue (seed)

Live gaps surfaced during play-testing, each needs the §3 write-up
before/with its port:

- **Animation-event system (footsteps).** FORENSICS DONE —
  [forensics/animation-events.md](forensics/animation-events.md).
  Correction to the original framing: the tag-dispatch chain is
  **already ported** verbatim from 1999 (`TCharacter::Pulse` →
  `T3DAnimator::Pulse` ([3dimage.cpp:2243](../../src/3dimage.cpp#L2243))
  → `T3DImagery::PlaySound` → `FindTag("play")` → `PlayWave` →
  `SoundPlayer.Play`). `beg`/`end` are state-continuation labels (not
  callbacks); controller tags were deliberately removed; **swipe is
  combat-state-driven and impact hits key off `attack->impacttime`,
  neither is tag-driven.** So footsteps is the ONLY tag-dependent gap,
  and it's narrow: confirm the walk states actually carry `play` tags
  and that `PlayWave` resolves the sound. Probe live in
  `T3DImagery::PlaySound` (`[footprobe]`).
- **Run / sneak mode.** `SetRunMode` / `SetSneakMode` exist
  ([character.cpp:3312](../../src/character.cpp#L3312)) and the `run`
  animation exists (state 233). Wired the `R`/`S` CMDFLAG edge →
  Set*Mode in `UpdateMove`, but the run animation reportedly doesn't
  play. Forensic Q: is `SetRunMode` reached (CMDFLAG_RUN firing through
  ControlMap), and does the root swap survive `Go()`'s re-resolution?
  Trace `ctrlmap.cpp` GetCommand → cmdflagstate → UpdateMove edge.
- **Cursor ground shadow.** `DecodeBitmapToRGBA`
  ([bitmapdecode.cpp:42](../../src/bitmapdecode.cpp#L42)) ignores the
  alias/alpha buffer the shadow's soft shape lives in (legacy drew with
  `DM_ALIAS`). Wedge shadow renders invisible/wrong. Forensic Q: what's
  the shadow bitmap's actual format (8-bit + alias buffer? 16-bit +
  keycolor?), and does the decoder need an alias→alpha path. Smaller
  than the other two — a bitmap-decoder gap, not a gameplay subsystem.
