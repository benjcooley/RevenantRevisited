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
| [x] | Turbo: several simulation steps per drawn frame, each with its own input drain; the step count adapts to a 12 ms simulation budget. The opening: 6 min 23 s of game time in about 1 s | 2026-10-13 |
| [x] | `--shadow`, `--shadow-out`, `--shadow-until`, `--shadow-lenient`; `TShadowRun` pulsed first in `TPlayScreen::Update`; `DeterministicRun`; seed 1 by default | 2026-10-13 |
| [x] | Plan parser: module, stages, `start`, `scene`, `talk`, `expect`, `save`; errors with file:line; `test_shadowplan` (CI). Places and the keep list come with P1 | 2026-10-13 |
| [x] | Event log (`shadow.jsonl`, `[smoke]` lines in revenant.log), `summary.txt`, `waypoints.json`; exit codes 0/1/2 | 2026-10-13 |
| [~] | Ledger: per-tick snapshot and diff; script, clock and exit causes; story variables; cheat flags (verb and kill causes come with P1/P2) | 2026-10-13 |
| [x] | Verbs: `talk`, `choose` (digit keys through the input queue), `scene`, `expect`, `save` (waits for idle control) | 2026-10-13 |
| [~] | Movies skipped with Escape through the input queue (written; first exercised by S8's FMV) | 2026-10-13 |
| [x] | `tools/shadow/shadow.py`: runs the plan, files each save in the waypoint catalogue as `shadow <name>`, `--check` load-checks it | 2026-10-13 |
| [x] | Gate: the opening (every line of both menus) passes in ~10 s; two seed-1 runs give identical logs; `shadow opening` filed and load-checked | 2026-10-13 |

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
| [x] | `TCharacter::RollAttack` extracted from `IsValidAttack` (and `RollDamagePercent` from the four attack choices): C3 port output byte-identical before and after (5206/5206 against retail); arena fights identical draw for draw | 2026-10-13 |
| [ ] | Duel timeline: monster cadence, attack choice, blocks, retreat; Locke's blows, fatigue | — |
| [ ] | Outcomes through `Damage`, the award functions, `DropLoot`, `LastAttack` | — |
| [ ] | Tactics: rotation, counter timing, blocks, fatigue, potions, disengage; the 64-run size-up | — |
| [ ] | Several monsters at once | — |
| [~] | Spells: `CastByName` and the pure part of `TSpell::Damage` (static `TSpell::Damage(caster, target, spelldata, variant)`, done; the duel side waits) | 2026-10-13 |
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
| [~] | Story-trigger map: `tools/shadow/storymap.py`, STORY_TRIGGERS.md (forensics agent running) | 2026-10-13 |
| [ ] | Story-trigger coverage gate: every required writer hit, town readers ran after | — |
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
