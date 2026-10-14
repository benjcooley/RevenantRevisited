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
| Travel between places | proven walks, real travel time (§5) | P1 | same level done; through doors by their scripts (S1 passes) |
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
| Story locations that change the town | the story-trigger map and gate (§9.4) | P5 | mapped (STORY_TRIGGERS.md); gate to build |

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
| [x] | Walk field per level with the game's standing test (`GetWalkHeightRadius`); regions; A*; `TSector::WalkmapVersion`; `TGameMap::FindSector` is a grid lookup (level 0 in ~0.5 s) | 2026-10-13 |
| [ ] | Blocker overlay: closed doors and gates, NOWALK barriers, barrier effects; `nav-discrepancy` findings | — |
| [x] | Through exits: `travel <exit>` steps onto a strip, or walks up to a scripted door and uses it; waits for the jump; a locked exit is refused with its key number | 2026-10-14 |
| [ ] | Automatic routing across levels (exit graph); today the plan names each exit, which doubles as the walkthrough | — |
| [ ] | Script scan for routing: `pos` on the player in CUBE, USE, ACTIVATE and DIALOG blocks; conditional edges tried live (until then the plan names each scripted transition) | — |
| [x] | Trigger zones (TShadowZones: every literal CUBE and player-named PROXIMITY of the level's area scripts and master.s, with an owner on the level); A* steers around them; a walk that can't stops in the zone, lets it fire, then goes on (tested on BAYNE1's tavern cube) | 2026-10-14 |
| [ ] | Encounters on the route: monsters already there (sight, LineOfSight), priced by the duel model | — |
| [ ] | Travel modes: `travel` fights every monster on the leg; `evade` runs and fights ⌈N·share⌉, at least one; the policy's choice | — |
| [ ] | Generator encounters on the route: exposure × slot frequency, real spawns through `TMonsterGen` (after P3) | — |
| [x] | Travel time from Locke's real speeds (`walkf` 5.71, `runf` 8.93 units a tick); run/walk pacing by fatigue; the walk's ticks pass before he arrives | 2026-10-13 |
| [x] | Proofs in the log (path sampled every 256 units); the ledger accepts a jump only where a proven travel said he'd land | 2026-10-13 |
| [~] | Gate: the Keep chain's walks pass (Rand, Tendrick with every branch, the GowE scene, KeepExit, then S1: 13 min 53 s of game, 0 violations); tower door without key 42 to test | 2026-10-14 |
| [ ] | Level 0 takes ~25 s of wall time to load each time Locke returns (the game releases levels with no player, as retail). Worth a look once runs get long | — |

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
| [x] | Player walkthroughs gathered and checked against STORY.md: WALKTHROUGH.md (eight guides; the route, readiness, answers to STORY.md §8) | 2026-10-14 |
| [ ] | `tools/shadow/ahkuilon.plan`, S0–S15 | — |
| [ ] | Readiness levels from the duel model | — |
| [ ] | Stage-end checks: NPCs alive, keep list, control | — |
| [x] | Story-trigger map: `tools/shadow/storymap.py`, STORY_TRIGGERS.md | 2026-10-13 |
| [ ] | Story-trigger coverage gate: every required writer hit, town readers ran after | — |
| [ ] | Script coverage report | — |
| [ ] | Gate: new game to credits; every milestone saved and load-checked | — |

## P6 Hardening

| Status | Item | Last touched |
|---|---|---|
| [ ] | 20-seed sweep | — |
| [ ] | Waypoints load in retail (emulator or DOSBox lab) | — |
| [ ] | A local CI target for machines with the game data | — |

## Needs the walkthroughs turned up

| Status | Item | Last touched |
|---|---|---|
| [ ] | Item-on-item combining (an ingredient onto an Empty Vial makes the vial, S9; also Angsaar): does the port support it? A verb `combine` | — |
| [ ] | Bows in the duel: the longbow and ice arrows beat the Solifuge in every guide (S7) | — |
| [ ] | Widowett must die for the amulet's chest (S5) | — |
| [ ] | Spell recipes in talisman order: the port matches in order (`spell.cpp:371`); confirm retail does | — |
| [ ] | `POISONCHANCE` is read from char.def but nothing applies it in the port's combat: forensics, then the duel | — |
| [ ] | Retail's dragon-room save bug (Sidious and his loot come back on reload): the plan never saves there | — |

## Findings

Port bugs and data questions the runs turn up, with the stage and tick.
None yet.
