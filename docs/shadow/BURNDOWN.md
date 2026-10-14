# Shadow Smoke burndown

The live checklist for [ARCHITECTURE.md](ARCHITECTURE.md). Worktree
`worktrees/shadow`, branch `feature/shadow`. A new agent starts with
README.md, then ARCHITECTURE.md, then this page, then
`git log --oneline origin/main..HEAD`.

Legend: `[ ]` open · `[~]` in progress · `[x]` done · `[!]` blocked (reason
inline) · `[-]` cancelled. Dates are ISO.

## What Locke does

Every player activity the smoke must cover, where it lives and how far
it has got.

| Activity | How | Phase | Status |
|---|---|---|---|
| Scenes and cutscenes | `scene`; movies skipped with Escape | P0 | done (the opening) |
| Dialog choices | `talk … choose …`, digit keys | P0 | done (the opening's menus) |
| Story checks | `expect`; every story-variable change logged | P0 | done |
| Milestone saves | `save`, filed in the waypoint catalogue | P0 | done |
| Travel between places | proven walks, real travel time (§5) | P1 | in progress |
| Doors, keys, levers | `use … with …`, transitions (§5.3) | P1 | — |
| Town teleports | stones in the route graph (§5.3) | P1 | — |
| Fights on the way | travel and evade (§5.5) | P1–P3 | — |
| Fights | the duel model (§6) | P2 | — |
| Getting poisoned, paralysed, buffed | real state through the duel (§8.7) | P2 | — |
| Looting monsters | real death drop, `Pickup` of every item (§8.8) | P2 | — |
| Opening chests, picking locks | `Take` / `UseOn` through `TContainer::Use`, `CheckKeyUse` (§8.8) | P4 | — |
| Monster generators | `TMonsterGen` (§7) | P3 | agent porting |
| Levelling up | real `AwardKillExp` | P2 | — |
| New stat points | `Train`: the level-up markers (§8.3) | P4 | — |
| Buying armour, weapons, potions | the shop's keys (§8.1) | P4 | — |
| Selling what is no longer useful | §8.1 | P4 | — |
| Readying equipment: armour, the fight's weapon, belt, quick spells, a buff | `TPlayer::Equip` with the game's `CanEquip` (§8.10) | P4 | — |
| Using potions and food | `Consume`, retail `TFood::Use` (§8.2) | P4 | agent porting `TFood::Use` |
| Placing items, using bags | the real `AddToInventory`: belt for healing, pouches for consumables (§8.9) | P4 | — |
| Talismans into the Spell Pouch | moved there on arrival; casting checks it (§8.9) | P4 | — |
| Learning new spells | `Invent`: casting a talisman combination teaches it; scrolls to check (§8.6) | P4 | — |
| Readying spells | quick-spell slots (§8.6) | P4 | — |
| Casting spells | `CastByName` in the duel (§6.7) | P2 | agent extracting the damage dice |
| Learning combos | Jong's lessons when due: AttackLevel, the lesson move landed (§8.5) | P4 | — |
| Using combos in fights | chain attacks keyed inside their windows (§6.4) | P2 | — |
| Grinding by level, back to town | §8.5 | P4 | — |
| Story locations that change the town | the story-trigger map and gate (§9.4) | P5 | agent mapping |

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
| [ ] | `TCharacter::RollAttack` extracted from `IsValidAttack`; katas C1/C2 unchanged | — |
| [ ] | Duel timeline: monster cadence, attack choice, blocks, retreat; Locke's blows, fatigue | — |
| [ ] | Outcomes through `Damage`, the award functions, `DropLoot`, `LastAttack` | — |
| [ ] | Tactics: rotation, counter timing, blocks, fatigue, potions, disengage; the 64-run size-up | — |
| [ ] | Combos: chain attacks keyed inside `chainexptime` (§6.4) | — |
| [ ] | Interrupts: a monster fighting Locke in the live world starts a fight first (§6.5) | — |
| [ ] | Several monsters at once | — |
| [ ] | Spells: `CastByName` and the pure part of `TSpell::Damage` | — |
| [ ] | Effects: poison, paralysis, stat effects, damage types, monster spells (§8.7) | — |
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
| [ ] | Shopping list scored by the duel model; selling what is no longer useful (outgrown gear, spent keep-list items) | — |
| [ ] | Level-up markers by style | — |
| [ ] | Jong's lessons | — |
| [ ] | Spell invention and quick-spell slots | — |
| [ ] | The pack: talismans into the Spell Pouch, healing on the belt, pouches for consumables (§8.9) | — |
| [ ] | Grind sites, linger rules, return to town | — |
| [ ] | Chest survey per level; chest tours by value per minute; locked chests by key or lockpicks; traps (§8.8) | — |
| [ ] | Loot every kill: pick up the death drop; a full pack keeps the best per slot (§8.8) | — |
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
