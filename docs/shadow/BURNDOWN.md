# Shadow Smoke burndown

The live checklist for [ARCHITECTURE.md](ARCHITECTURE.md). Worktree
`worktrees/shadow`, branch `feature/shadow`. A new agent starts with
README.md, then ARCHITECTURE.md, then this page, then
`git log --oneline origin/main..HEAD`.

Legend: `[ ]` open · `[~]` in progress · `[x]` done · `[!]` blocked (reason
inline) · `[-]` cancelled. Dates are ISO.

## P0 Spine

| Status | Item | Last touched |
|---|---|---|
| [x] | Design: ARCHITECTURE.md, README.md, this page | 2026-10-13 |
| [ ] | Turbo time: `TTime::BeginFixedFrames(n)`, a per-frame tick budget under `--shadow` | — |
| [ ] | `--shadow` flags; `TShadowRun` hosted in `TPlayScreen::Update`; deterministic level loads | — |
| [ ] | Plan parser: stages, steps, places, keep list; errors with file:line | — |
| [ ] | Event log (`shadow.jsonl`) and `summary.txt`; exit codes | — |
| [ ] | Ledger: per-tick snapshot and diff; verb, script, kill, clock and exit causes; cheat flags | — |
| [ ] | Verbs: `talk`, `choose` (public `TDialogPane::Choose`), `scene`, `expect`, `save` (gated) | — |
| [ ] | Movies skipped through the player's path | — |
| [ ] | Waypoint export in the playtest catalogue's format; load check | — |
| [ ] | Gate: the opening to "control on" headless; waypoint `shadow-opening` loads | — |

## P1 Nav

| Status | Item | Last touched |
|---|---|---|
| [ ] | Walk field per level with the `Blocked` rule; regions by flood fill; world version | — |
| [ ] | Blocker overlay: closed doors and gates, NOWALK barriers, barrier effects; `nav-discrepancy` findings | — |
| [ ] | Transition graph: exit.def exits with locks and keys | — |
| [ ] | Script scan: `pos` on the player in CUBE, USE, ACTIVATE and DIALOG blocks; conditional edges tried live | — |
| [ ] | A* within regions; cube cost and route splitting at crossings | — |
| [ ] | Encounters on the route: monsters already there (sight, LineOfSight), priced by the duel model | — |
| [ ] | Travel modes: `travel` fights every monster on the leg; `evade` runs and fights ⌈N·share⌉, at least one; the policy's choice | — |
| [ ] | Generator encounters on the route: exposure × slot frequency, real spawns through `TMonsterGen` (after P3) | — |
| [ ] | Travel time from Locke's real walk and run speeds; run/walk pacing by fatigue; the leg's ticks run at the destination | — |
| [ ] | Proofs in the log; a teleport without a proof is a violation | — |
| [ ] | Gate: the Keep chain's walks; tower door refused without key 42 | — |

## P2 Duel

| Status | Item | Last touched |
|---|---|---|
| [ ] | `TCharacter::RollAttack` extracted from `IsValidAttack`; katas C1/C2 unchanged | — |
| [ ] | Duel timeline: monster cadence, attack choice, blocks, retreat; Locke's blows, fatigue | — |
| [ ] | Outcomes through `Damage`, the award functions, `DropLoot`, `LastAttack` | — |
| [ ] | Tactics: rotation, counter timing, blocks, fatigue, potions, disengage; the 64-run size-up | — |
| [ ] | Several monsters at once | — |
| [ ] | Spells: `CastByName` and the pure part of `TSpell::Damage` | — |
| [ ] | Calibration suite against the arena and player bot; tolerances | — |
| [ ] | `--shadow-combat=real` | — |
| [ ] | Gate: Rahul; the Keep chain complete | — |

## P3 Generators

| Status | Item | Last touched |
|---|---|---|
| [ ] | `TMonsterGen` retail first: slots, Pulse, spawn, treasure, Load/Save | — |
| [ ] | `addmonstertype`, `delmonstertype`, `maxmonsters`, `monstertypes` | — |
| [ ] | Gate: respawn cadence against the emulator | — |

## P4 Purse

| Status | Item | Last touched |
|---|---|---|
| [ ] | Retail `TFood::Use` (`0x0050e780`) | — |
| [ ] | Public shop verbs (`Transact`, `Exit`); buy, sell | — |
| [ ] | Shopping list scored by the duel model | — |
| [ ] | Level-up markers by style | — |
| [ ] | Jong's lessons | — |
| [ ] | Spell invention and quick-spell slots | — |
| [ ] | Grind sites, linger rules, return to town | — |
| [ ] | Gate: level 1 to 6 with shopping trips | — |

## P5 Story

| Status | Item | Last touched |
|---|---|---|
| [ ] | `tools/shadow/ahkuilon.plan`, S0–S15 | — |
| [ ] | Readiness levels from the duel model | — |
| [ ] | Stage-end checks: NPCs alive, keep list, control | — |
| [ ] | Script coverage report | — |
| [ ] | Gate: new game to credits; every milestone saved and load-checked | — |

## P6 Hardening

| Status | Item | Last touched |
|---|---|---|
| [ ] | 20-seed sweep | — |
| [ ] | Waypoints load in retail (emulator or DOSBox lab) | — |
| [ ] | A local CI target for machines with the game data | — |

## Findings

Port bugs and data questions the runs turn up, with the stage and tick.
None yet.
