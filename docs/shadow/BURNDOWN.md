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
| Fights on the way | travel and evade (§5.5) | P1–P3 | written; first exercised on the way to the Druhg camp |
| Fights | the duel model (§6) | P2 | working (Rahul) |
| Getting poisoned, paralysed, buffed | real state through the duel (§8.7) | P2 | — |
| Looting monsters | real death drop, `Pickup` of every item (§8.8) | P2 | done |
| Opening chests, picking locks | `take` through `TContainer::Use`; keys by `with`; lockpicks to come (§8.8) | P4 | chests done (the Keep's 500 gold); lockpicks open |
| Monster generators | `TMonsterGen` (§7) | P3 | ported and merged |
| Levelling up | real `AwardKillExp` | P2 | experience from kills works; a level-up yet to be seen in a run |
| New stat points | `Train`: the level-up markers (§8.3) | P4 | — |
| Buying armour, weapons, potions | the shop's keys (§8.1) | P4 | buying done (a potion at Elahni's); the shopping list to come |
| Selling what is no longer useful | §8.1 | P4 | — |
| Readying equipment: armour, the fight's weapon, belt, quick spells, a buff | `TPlayer::Equip` with the game's `CanEquip` (§8.10) | P4 | — |
| Using potions and food | `Consume`, retail `TFood::Use` (§8.2) | P4 | done (retail TFood::Use merged); in fights below 35% health |
| Placing items, using bags | the real `AddToInventory`: belt for healing, pouches for consumables (§8.9) | P4 | — |
| Talismans into the Spell Pouch | moved there on arrival; casting checks it (§8.9) | P4 | — |
| Learning new spells | `invent`: casting a talisman combination in the spell pane teaches it; a scroll only shows its text (§8.6) | P4 | done |
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
| [~] | Encounters on the route: every aggressive enemy whose sight range and LineOfSight reach a point of the walk; the leg stops there for the fight, then goes on (written; first met on S2's way to the Druhg camp). Pricing routes by the duel model to do | 2026-10-14 |
| [~] | Travel modes: `travel` fights what sees the walk; `evade` fights ⌈N·share⌉ (¼ slower, ½ as fast), at least one (written). The policy's choice to do | 2026-10-14 |
| [ ] | Generator encounters on the route: exposure × slot frequency, real spawns through `TMonsterGen` (after P3) | — |
| [x] | Travel time from Locke's real speeds (`walkf` 5.71, `runf` 8.93 units a tick); run/walk pacing by fatigue; the walk's ticks pass before he arrives | 2026-10-13 |
| [x] | Proofs in the log (path sampled every 256 units); the ledger accepts a jump only where a proven travel said he'd land | 2026-10-13 |
| [~] | Gate: the Keep chain's walks pass (Rand, Tendrick with every branch, the GowE scene, KeepExit, then S1: 13 min 53 s of game, 0 violations); tower door without key 42 to test | 2026-10-14 |
| [ ] | Level 0 takes ~25 s of wall time to load each time Locke returns (the game releases levels with no player, as retail). Worth a look once runs get long | — |

## P2 Duel

| Status | Item | Last touched |
|---|---|---|
| [x] | `TCharacter::RollAttack` extracted from `IsValidAttack` (and `RollDamagePercent` from the four attack choices): C3 port output byte-identical before and after (5206/5206 against retail); arena fights identical draw for draw | 2026-10-13 |
| [x] | Duel timeline (TShadowDuel): monster cadence (ATTACKFREQ), RandomAttack's pick, OnAttacked's guard, RETREATAT; Locke's blows by IsValidAttack's animation-free tests, fatigue; one fight in one tick | 2026-10-14 |
| [x] | Outcomes through the game: `WeighBlow`/`LandBlow` extracted from ResolveHit (Damage, the kill, skill and stealth awards); the bodies drop their loot and Locke picks it up; the ledger's progress cause. `LastAttack` for Jong still to do | 2026-10-14 |
| [~] | Tactics: rotation, chains first, guard a seen blow (35%), never the fatigued blow, a potion under 35% health. Disengage and the 64-run size-up (trial mode is built) to do | 2026-10-14 |
| [ ] | Combos: chain attacks keyed inside `chainexptime` (§6.4) | — |
| [x] | Interrupts: anything fighting Locke in the live world is fought first (Rahul, as the opening ends) | 2026-10-14 |
| [x] | Several monsters at once: each its own timer, at most three in reach | 2026-10-14 |
| [~] | Spells: `CastByName` and the pure part of `TSpell::Damage` (static `TSpell::Damage(caster, target, spelldata, variant)`, done; the duel side waits) | 2026-10-13 |
| [ ] | Effects: poison, paralysis, stat effects, damage types, monster spells (§8.7) | — |
| [ ] | Calibration suite against the arena and player bot; tolerances | — |
| [ ] | `--shadow-combat=real` | — |
| [x] | Gate: from a new game through Rahul (won barehanded, 214 ticks), Rand, Tendrick, the Keep's exit, Gus's deal and the first potions: 23 min 24 s of game in 30 s, six waypoints, no violations | 2026-10-14 |

## P3 Generators

| Status | Item | Last touched |
|---|---|---|
| [x] | `TMonsterGen` retail first: slots, Pulse (off screen by the animate pass's verdict), spawn, treasure kit, Load/Save in retail's layout (agent, shadow/gen) | 2026-10-14 |
| [x] | `addmonstertype`, `delmonstertype`, `maxmonsters`, `monstertypes` | 2026-10-14 |
| [ ] | Gate: respawn cadence against the emulator | — |

## P4 Purse

| Status | Item | Last touched |
|---|---|---|
| [x] | Retail `TFood::Use` (`0x0050e780`): health and mana to the cap, poison cured (and its spell effects ended), one fewer, the last deleted, FOODGOOD1 | 2026-10-13 |
| [x] | `buy "<item>" [n] from <merchant>` / `sell ... to ...`: the merchant's shop script sets up the stock (Buy/Sell answer), then the shop's own `TBuySellPane::Buy`/`Sell` per item and `Exit`, no drawer keys (the author's call, 2026-10-14); the ledger allows exactly the trade. Elahni sells Locke his second Lesser Healing for 250. Selling is written, not yet exercised | 2026-10-14 |
| [x] | `equip`, `unequip` (TPlayer::Equip with the game's CanEquip), `consume` (the item's Use), `train` (TrainStat: the level-up picks); ledger causes for the picks and for what food gives | 2026-10-14 |
| [x] | Selling what is no longer useful: `sell junk to <merchant>` (shadowpurse.cpp): every weapon but the one in hand and the best spare, armour no better than what he wears in its slot; the shop's own Sell takes the unworn piece first. Gina buys back a spare Short Sword for 58, Cronus the old cloth shirt for 3 | 2026-10-14 |
| [x] | Readying armour (§8.10) whenever the pack's counts change: each armour slot wears its best piece by Protection through TPlayer::Equip and its CanEquip (a bought Black Leather Chest Plate goes on at once). The ledger audits base attributes and skills, so what equipment and stat effects add needs no cause | 2026-10-14 |
| [ ] | Shopping list scored by the duel model (armour per gold, weapons by kill rate, jewellery by STATLINE); spent keep-list items; selling on the restock trip (needs door legs to Gina's and Cronus's) | — |
| [ ] | Level-up markers by style | — |
| [ ] | Jong's lessons | — |
| [x] | Spell invention: at each step boundary after the pack changes (or `invent`), every spell.def combination he holds and doesn't know goes in the construct slot and through the pane's own `TSpellPane::Invoke`; the book learns it cast or not. The opening's Life, Moon and Soul teach Poison, Nourish, Might, Fist Mastery and Physical Paralysis. A spell the style uses goes on a free quick-spell ring (Fist Mastery for hands) | 2026-10-14 |
| [x] | The pack kept in order whenever its counts change (shadowpack.cpp, the game's AddToInventory): talismans into the Spell Pouch (the opening's three go in at once), potions onto belt boxes F5–F9 one kind a box (the Keep chest's Lesser Healing to box 1; bought ones merge onto it), scrolls and tradeable odd bits into Pouches of their own | 2026-10-14 |
| [ ] | The duel drinks only from the belt (`ShadowBeltHealing`), once the calibrated duel lands | — |
| [x] | `grind until level <n> \| skill <name> <n> [restock <routine>]`: sites scored by Value against his level, trial duels size each monster up, generators farmed from out of sight, food then potions below 60%, rest out of sight to 90% when nothing is left (never set off hurt), the restock routine when out of healing with the gold for it | 2026-10-14 |
| [ ] | Gate: the forest grind to level 4 passes. Open: a level-1 Locke loses 15–25 health an Arakna fight for ~10 exp and ~30 gold, against 250-gold potions and 1 HP a minute, so he rests an hour every few fights. Waits on the duel calibration (is that what a player takes?) | — |
| [ ] | Barehanded practice for Baez (S6): weapon off, `grind until skill hands 3` in the forest, `expect skill hands >= 3` before the arena (the author: without Hands Locke can't beat him) | — |
| [x] | Chest survey: every container of a level with its lock, key and contents, logged as Locke first reaches the level | 2026-10-14 |
| [x] | `take <chest> [with "<key>"]`: walk up, the real `TContainer::Use` a tick (open, one item each, close); the ledger allows exactly what the chest held. The Keep's three unlocked chests give their 500 gold, a Lesser Healing and an Apple | 2026-10-14 |
| [ ] | Chest tours planned by value per minute of travel; lockpicks on locked chests; traps (§8.8) | — |
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
| [x] | Save round trip: Locke's whole state (every item where it is, bags and belt and equipment, stats, skills, picks, spells, quick spells, story variables) recorded at each save; `--shadow-verify=<file>` with `--quickstart=<slot>` writes it from a fresh load; `shadow.py --check` compares field by field. The plan's first six saves come back unchanged | 2026-10-14 |
| [x] | A guarded way: trial duels size up a leg's fight; he runs past what he'd lose to, and turns back (travel fails as guarded) when even that is lost. Death ends the run | 2026-10-14 |
| [~] | Play styles (§8.4): `style` / `--shadow-style` = melee, hands, bow, spells, mixed. Done: the setting, the level-up picks (`train` alone takes the style's), the first forest grind barehanded for Baez in every style. To do: readying, duel tactics, shopping, spells | 2026-10-14 |
| [ ] | Bows in the duel: bow-mode shots while a monster closes (needed for the Solifuge anyway) | — |
| [ ] | Spells in the duel (§6.8): CastByName, the static TSpell::Damage, buffs | — |
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
