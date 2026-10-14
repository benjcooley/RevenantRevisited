# Shadow Smoke: architecture

Shadow Smoke plays Ahkuilon from the new game to the credits, headless and
deterministic, in minutes instead of hours. Locke does everything a player
does, with two abstractions:

- **He doesn't walk.** He teleports to the next activity, but only after
  the planner has proved that a legal walk exists on the real map, in its
  current state: doors, locks, gates, barriers and hazards included.
- **He doesn't animate his fights.** A battle is resolved by a duel model
  that throws retail's dice with retail's numbers. The results go through
  the game's own bookkeeping: health, death, experience, skills, level-ups
  and loot.

Everything else is the real game: scripts, scenes, dialog (with choices
picked), shops, inventory, level-ups, spells, monster generators and saves.
The run writes save games at the story's milestones, so any section of the
game can be tested from a save in seconds.

This page is the design. [BURNDOWN.md](BURNDOWN.md) is the live checklist,
and [README.md](README.md) explains how to run it.

## 1. Rules

### 1.1 Locke plays fair

Shadow Smoke may not cheat. That is enforced in code, not left to the
plan's discipline:

- **No console, no cheats.** Shadow never calls `CommandInterpreter`,
  `ApplyCheatWord`, `SetMoney`, `AddToInventory`, `SetPlayerLevel` or any
  `Cmd*` for the player. Scripts may (they are the game).
- **Locke's verbs only.** Every change Shadow makes goes through one of a
  small set of player verbs (§3), each implemented by the call a real
  player's input reaches: `TCharacter::Use`, `Pickup`, `TPlayer::Equip`,
  `TFood::Use`, the shop's transaction, `TrainStat`, the spell pane's
  invoke, the save request. Travel and combat are the only abstractions.
- **The ledger audits every change** (§4). After every tick it compares
  Locke's gold, experience, level, stats, skills, items, health and
  position with the last tick. Every difference must be explained by a
  cause registered by a verb, a kill, a script line or the game's own
  clock (regeneration). An unexplained difference fails the run.

### 1.2 Never easier than walking

Wherever an abstraction has to guess, it guesses against Locke:

- The monsters along the route engage him, and the generators along it
  spawn at their real rate while he passes (§5.5). Evading them costs
  fatigue and still leaves at least one fight.
- A closed door or gate blocks, whether or not its walkmap says so.
- A trigger volume on the route fires as if he had walked into it.
- Travel takes the time the walk would take, at walking speed, so the
  world's clocks (generators, timed scripts, poison) keep running.

A pass is then evidence that a real player could pass. A failure is
either a port bug, a data bug or a plan bug, and the run log says which
step failed.

Combat is the exception. It is calibrated to match real fights (§6.6),
neither harder nor easier.

## 2. Shape

```
                 --shadow=<plan>  (+ --headless, --seed)
                          │
   TPlayScreen::Update ──▶ TShadowRun ──────────────── shadow/<run>/ log, report
                          │   ▲ ledger checks every tick
        ┌──────────┬──────┴───┬───────────┬────────────┬────────────┐
        ▼          ▼          ▼           ▼            ▼            ▼
   TShadowPlan  TShadowNav  TShadowDuel  TShadowPurse TShadowGrind  TShadowWaypoints
   (stages,     (walk       (battles)    (shops,      (sites,       (saves,
    steps)       proofs)                  potions,     lingering)    catalogue)
                                          level-ups,
                                          spells)
        └──────────┴──── all act through TShadowVerbs ────┴────────────┘
                                   │
               the real game: TPlayer, TCharacter, TExit, TContainer,
               TBuySellPane, TDialogPane, TScriptManager, TGameSession
```

- **`TShadowRun`** is the driver. It is pulsed once a tick from
  `TPlayScreen::Update`, after the session's requests and before the mode
  tick, like the save-cycle test it generalises. It executes the plan's
  current step, waits while the game is busy (a scene, a fade, a level
  load, a script wait), checks the ledger and writes the log.
- **`TShadowPlan`** is the parsed plan file (§9): the stages of the story,
  with their steps, their readiness, the places and the policies.
- **`TShadowNav`** proves walks and plans routes (§5).
- **`TShadowDuel`** resolves battles (§6).
- **`TShadowPurse`** owns money and growth (§8): what to buy and sell, when
  to drink, which attributes to train, which spells to make.
- **`TShadowGrind`** decides where to fight and for how long (§8.5).
- **`TShadowWaypoints`** writes the milestone saves into the waypoint
  catalogue the playtest tools already use (§10).

Everything lives in `src/shadow/`. The game gets no Shadow code beyond the
one hook in `TPlayScreen::Update` and the fixes in §12. Those are real port
gaps that Shadow exposes.

## 3. Locke's verbs

`TShadowVerbs` is the only object that changes the game. Each verb
registers its expected effects with the ledger, then acts the way a
player's input does:

| Verb | What it calls | Ledger cause |
|---|---|---|
| `Travel(dest)` | `TObjectInstance::Teleport`, with a `TShadowNav` proof | `travel` (position, level) |
| `Talk(npc)` | `npc->Use(Player, -1)`: fires the DIALOG trigger | none of its own; the script's changes are `script` |
| `Choose(n)` | the digit key, through the input queue | — |
| `UseOn(target, item)` | `target->Use(Player, item)`: keys on doors and chests, USE triggers | `use` |
| `Take(container, item)` | `TContainer::Use`, then the take | `take` (item, gold) |
| `PickUp(item)` | `TCharacter::Pickup` | `pickup` (item, gold) |
| `Equip(item)` / `Unequip(item)` | `TPlayer::Equip` / the unequip path | `equip` |
| `Consume(item)` | `item->Use(Player, -1)`, i.e. `TFood::Use` | `consume` (health, mana, item) |
| `Buy(row)` / `Sell(row)` | the shop's keys (row 1–3, A, arrows, E), through the input queue | `shop` (item, gold) |
| `SkipMovie()` | Escape, through the input queue | — |
| `Train(attr)` | `TPlayer::TrainStat` | `train` (level-up markers) |
| `Invent(talismans)` | the spell pane's invoke: `SpellBook.Reveal`, `TPlayer::LearnSpell` | `learn` |
| `Fight(foes)` | `TShadowDuel` (§6) | `kill`, `hit`, `cast` |
| `Save(name)` | `TGameSession::RequestSave`, gated (§10) | — |
| `Load(name)` | `TGameSession::RequestLoad`, after a death (§6.5) | resets the ledger |

What a player does with a key, Shadow does with the same key: it pushes
the key onto the input queue (`InputQueue.PushKey`), which turbo drains
before every tick (§11). What a player does with a click on the world
(talk, use, take, pick up), Shadow does by calling the function the click
reaches, because turning a world point into a screen click buys nothing
headless.

Talking, using and taking also need Locke to be in reach. `Travel` puts
him there first: next to the NPC, in front of the door, at the chest.

## 4. The ledger

`TShadowLedger` snapshots Locke at the end of every tick:

- position and level;
- health, mana and fatigue;
- gold, experience and level;
- the six attributes and the eleven skills (level and experience);
- the level-up markers and AttackLevel;
- every item he carries, bags included, with amounts.

It diffs each snapshot against the last one and matches every change to a
cause:

- **Verb causes** are registered just before the verb acts, with the
  changes they allow ("gold −195, Short Sword +1").
- **Kill causes** come from the game's award seams
  (`TPlayer::killExpSeam`, `skillExpSeam`, `stealthExpSeam`). Experience
  can only rise through them.
- **Script causes** come from the script engine's step observer
  (`TScript::SetStepObserver`). A tick in which a script line ran may
  change anything that line's command can change: `addinv`, `delinv`,
  `give`, `pos`, `stat`, `setgameflag`.
- **Clock causes** are the regeneration rates of rules.def. Health, mana
  and fatigue may rise by at most what `PulseRecovery` allows for the
  ticks that passed.
- **Exits** (`TExit::Activate`) may move Locke to their exit.def target.

Anything else is a violation. It is logged with the tick, the field, the
values before and after, and the causes open at the time. The run stops
unless `--shadow-lenient` is given.

The ledger also checks that the cheat flags (`CheatAlreadyDead`,
`NoAI` and the rest) stay off for the whole run.

## 5. Navigation: proving a walk

The question `TShadowNav` answers: *could Locke walk from here to there
right now, and what would happen on the way?*

### 5.1 The walk field

A level's walkmap is already whole in memory once the level is loaded:
`TGameMap::FinishLoad` stamps every object's walkmap into every sector
(16-unit cells, one height each, 0 for no floor). Different floors are
different levels, so a cell has one height.

`TShadowWalkField` builds, per level, the cells a character of Locke's
radius can stand on. It uses the game's own rule, `TCharacter::Blocked`
through `TMapPane::GetWalkHeightRadius`: no hole within the radius, no
step of more than 0x20 between neighbouring cells, ground not 0. It then
labels the connected components ("regions") with a flood fill over
8-connected cells whose height steps stay within 0x20.

Level 0 is about 2,000 × 2,000 cells. The flood fill is linear in cells,
and the field is cached per level and per **world version**: a counter
that every walkmap change bumps (§5.2), so a door that opens invalidates
only that level's field.

### 5.2 Blockers: doors, gates, barriers

The walkmap reflects each object's current imagery state, but whether a
closed door or a portcullis raises its cells is unverified (EXITS.md
marks it [I]). Shadow doesn't depend on it. A blocker overlay marks
these cells impassable on top of the walkmap:

- **Doors and gates** (`TExit` with `Openable`) in a closed or closing
  state, over their footprint. A locked door is closed by definition.
- **NOWALK barriers**: objects whose `OF_NOWALK` toggle is off (the
  labyrinth barriers, the Ogrok ring wall, `CAVMS*`), over their imagery
  footprint.
- **Effects that are barriers** (`barruin`, the labyrinth warps) while
  they exist.

Where the overlay blocks a cell that the walkmap says is walkable, the
run logs a `nav-discrepancy` finding. That is a port bug or a data
question about the walkmap, worth a look either way.

### 5.3 Transitions between places

Regions connect through transitions. `TShadowNav` builds the graph of
(level, region) nodes from three sources:

1. **exit.def exits.** An exit object whose strip lies in a region leads
   to the region holding its exit.def target. It is usable while the exit
   is unlocked, or while Locke carries its key (then the route includes
   `UseOn(door, key)`).
2. **Script transitions.** Every loaded script's trigger blocks are
   scanned for a `pos` command on the player (`player.pos`, `locke.pos`).
   The block's trigger is recorded: a CUBE volume, a USE or ACTIVATE on an
   object, or a DIALOG. So is the target of each `pos`. This is the table
   STORY.md §4 built by hand, read from the scripts instead. It finds the
   teleport stones (`master.s` ACTIVATE), the cave chain, the lever→gate
   jumps and the story scenes.
3. **Script gates are tried, not predicted.** Script transitions carry
   `IF` conditions on game state, and Shadow doesn't run scripts
   statically. A `pos` guarded only by simple comparisons of story
   variables with numbers (`IF TOWNTELSTATE = 5`) is planned against the
   current game state. Anything else is marked *conditional*: a route may
   use it only when the plan names it, or as a last resort. Either way,
   taking an edge means performing its trigger and letting the real
   script decide. If Locke doesn't arrive where the plan expected, the
   edge is marked shut for this world version, and the planner replans.

**The town teleports.** The stones (`TOWNTEL1`–`14`, STORY.md §5) are
ordinary edges of this graph. Each one sends Locke to the Keep and raises
`TOWNTELSTATE` to its number. `TownTel0` in the Keep sends him back to the
highest-numbered stone he has touched, which the planner reads from
`TOWNTELSTATE`. Because they cost almost nothing to use, they carry every
return to town from the cave levels on (§8.5). Stepping on a stone by
accident is also a hazard: routes that only pass one keep off it (§5.4).

Routes are Dijkstra over the graph. A leg inside a region costs its A*
path length, plus the danger along the path (§5.5).

### 5.4 Trigger volumes on the way

Teleporting over a CUBE doesn't fire it: the script tests Locke's
position each tick, so it doesn't notice crossings. A walk through it
would fire it. So:

- The A* inside a region gives cells inside a CUBE a high cost. If a
  route can go around, it does. Trigger volumes that are the step's
  destination don't count.
- If a route must cross one, the walk is split there. Locke travels to
  the cube's entry cell, the game runs until the script is idle and
  control is on, and then the rest of the route is re-planned from
  wherever he is.

The cubes come from the same script scan as §5.3: every CUBE trigger
block of every script that is loaded for the level.

### 5.5 Combat on the way

Travel between two points is not free. When Locke walks, the sector
window moves with him: the monsters along his way notice him, and the
generators along his way keep spawning. `TShadowEncounters` works out the
fights a leg brings, from two sources.

**Monsters already there.** These are placed monsters, and spawns left
from earlier. One is on the leg when all of these hold:

- it is alive, aggressive, and not an ally;
- a point of the route lies within its sight (640), and `LineOfSight`
  from the monster to that point is clear.

**Monsters the generators make.** A generator along the route spawns
monsters of its own types while Locke is near: a forest with Kantha
generators means Kantha on the road. For each generator whose 300-unit
area lies within sight of the route:

- The **exposure** is the time Locke would spend within sight of its
  area, at his travel pace (§5.6).
- Each of its slots yields `exposure / frequency` expected spawns of that
  slot's type, rolled on Shadow's stream. That is the generator's real
  rate (70–91 s a monster), compressed into the walk. The total is limited
  by its cap, less the characters already within 300 units.
- A spawn is real. It goes through `TMonsterGen`'s own spawn (§7): a
  walkable spot near the generator, full health, the slot's treasure.
  The generator's clock restarts, as after any spawn. The monster is then
  on the leg like any other.

**Travel modes: fight or evade.** Going from one forest to the next past
X generators that make Y monsters, Locke meets the Y, plus the monsters
already on the route. What he does about them depends on how he travels:

- **`travel`** (the default): he goes at the player's pace (§5.6) and
  fights every monster on the leg.
- **`evade`**: he runs (fatigue permitting) and gets past most of them.
  He still fights some: of the N on the leg, he fights the
  ⌈N · share⌉ nearest the route, and **at least one** whenever N is
  above 0. The share is 1/4 when they are slower than his run and 1/2
  when they are as fast or faster.

  The monsters he gets past stay in the world, where they are, as real
  spawns would. Evading costs the run's fatigue (§5.6).

The mode is the plan's (`travel <place> evade`) or the policy's. Locke
evades when the leg's expected fights would cost more health or potions
than he can spare, or when they are worth little (monsters whose Value is
far below his level give almost no experience, GRIND.md §1.2). Otherwise
he travels and fights, and the route becomes part of the grind.

This compresses the generators' clocks: in retail a generator that has
been away from Locke for long only restarts its clock on the first pulse
(GRIND.md §3.1), so a quick pass spawns less. That is the price of not
walking, and it errs toward more fighting, never less.

**Planning and fighting.** The expected cost of a leg's encounters is the
duel model's health loss against them (§6.4). It goes into the route's
cost, so the planner prefers quiet routes, the way a player keeps to the
road. Once a route is taken, the leg is split at each encounter in order:

1. Locke travels to the point where the monster engages.
2. The step becomes a `Fight` with it, and with every other engaging
   monster that sees that point.
3. Travel resumes from there.

Every roll is logged with the leg's proof (§5.7): the chance, the
generator's slot, and the result. The decision rolls use Shadow's own
random stream, so they don't shift the game's.

### 5.6 Travel time

Game time advances by exactly what the walk or run would take. The world's
clocks therefore keep their real pace: generators spawn, timed scripts
advance, and poison ticks.

- **Speeds** are Locke's real ones: the walk and run states' root motion
  per frame in his imagery, the movement `Go` applies. They are measured
  once per run and logged. The ascent of slopes doesn't change them, as in
  the game.
- **Pace** is the player's. Running costs `RUNFATIGUE` (−10 a second, with
  no fatigue recovery), and walking recovers it (about 13 a second;
  GRIND.md §6.1). So a leg is paced in stretches: run while fatigue is
  above a reserve, then walk until it is back. A short leg is run whole.
  Locke arrives with the fatigue the real run would have left him.
- **The ticks really run.** After the teleport, the game runs the leg's
  ticks with Locke standing at the destination, out of combat. Standing
  recovers fatigue at the walking rate, so the walking stretches take care
  of themselves. The cost of the last running stretch is then taken off
  at arrival. That is a decrease, which the ledger always allows, logged
  with its cause (`travel: ran`).

Health and mana regeneration run too, but at about one point a minute they
don't matter.

### 5.7 The proof

Every `Travel` writes a proof to the log:

- the start and end points;
- the regions it passed;
- the path length, and the cells sampled every 256 units;
- the doors it opened and the keys it used;
- the cubes it crossed;
- the encounters it fought;
- the game time it took.

A teleport without a proof is a ledger violation.

## 6. Combat: the duel model

### 6.1 What is real and what is modelled

Real (called, not copied):

- the attack records from char.def: bands, `impacttime`, `nextwait`,
  `blocktime`, fatigue, `damagemod`, weapon masks, `attackskill`,
  `weaponskill`, `attackpcnt`;
- `Offense()` and `Defense()`, `CalculateDamage`, `StrengthMod`,
  `EdgeBonus`, `ToHitDamageFactor` and the rules.def to-hit constants;
- the monster's AI numbers: `ATTACKFREQ`, `BLOCKFREQ`, block min and max,
  `RETREATAT`, `MAGICFREQ`;
- the game's random stream (`random`), so a seed replays a run;
- the outcome: `TCharacter::Damage`, death, `AwardKillExp`,
  `AwardSkillExp`, `AwardStealthExp`, the corpse's `DropLoot`, fatigue
  spent, mana spent, `LastAttack`.

Modelled: where the two stand and when each acts. There is no
animation, no movement, and no collision.

### 6.2 One implementation of the roll

The dice are thrown by `TCharacter::RollAttack`, which `IsValidAttack`
calls where it used to throw them inline; everything that needs
animation state stays in `IsValidAttack`:

```cpp
// The dice of one blow (COMBAT_HIT.md §3.1): the damage before its tier,
// the to-hit and the roll; -1 is a die not thrown yet.
struct SAttackRoll { int32_t damage = -1, tohit = -1, roll = -1;
                     bool Hits() const; int32_t Tiered() const; };

SAttackRoll TCharacter::RollAttack(TCharacter& target, const SCharAttackData& attack,
                                   int32_t dmgpcnt, bool targetopen, SAttackRoll thrown = {});
static int32_t TCharacter::RollDamagePercent();   // random(1,50) + random(1,50)
```

- `targetopen` is what `IsValidAttack` reads from the animation: the
  target faces away (`FaceAngleTo` ≥ 0x30) or is swinging itself. The duel
  model hands in its own answer.
- `dmgpcnt` is the attack choice's `RollDamagePercent`, drawn before the
  search (`ButtonAttack`, `RandomAttack`, `SpecificAttack`,
  `ButtonAction`); for the player `IsValidAttack` passes `StrengthMod`
  instead, but the two draws are still made.
- `thrown` keeps dice already fixed: a search throws once for all its
  candidates, and a repeated button fixes the roll at 100. Both sides' luck
  and defence are read whatever is left to throw, as retail reads them.
- `Tiered()` applies the TOHITDAMAGE tier. `IsValidAttack` applies it to
  the damage it carries every time a candidate gets that far (retail's
  compounding); `ResolveHit` undoes and re-tiers it for a block.

`IsValidAttack`'s results and draws are unchanged, draw for draw: the
attack-choice kata (C3) gives byte-identical port output before and after
(5206/5206 against retail), and twelve arena fights (Locke's AI and the
player bot against Araknids, a Druid's spells; seeds 1-3) trace
identically, every draw included.

### 6.3 The timeline

A duel is a discrete-event simulation in ticks (24 a second):

- **The monster** acts on its own cadence:
  - `nextattack = random(minattackfreq, maxattackfreq) · 24 / 100`;
  - then `RandomAttack(random(1,100))` over the attacks valid at the
    current distance band;
  - impact at the attack's `impacttime`, recovery at its `nextwait`;
  - it blocks with `random(1,100) ≤ blockfreq` while Locke's blow is
    before its `blocktime`;
  - it retreats at `retreatat`.
- **Locke** acts on his tactics (§6.4). Each blow is one of his valid
  attacks for the band he chooses: weapon mask, AttackLevel, weapon
  skill, fatigue. It lands at its `impacttime` and costs its fatigue at
  impact. Fatigue regenerates at the rules.def rate whenever he isn't
  attacking.
- **Distance** is a band (close, mid, lunge), not a position. Locke
  chooses the band; the monster closes at its run speed.
- **Facing.** Locke strikes a monster that is mid-attack with the +25
  bonus when his blow lands inside its swing. A monster never gets the
  bonus against Locke, because a player keeps facing his target.
- **Several monsters.** Each one that sees Locke runs its own cadence.
  At most three are in reach at once, as when monsters crowd a player;
  the rest wait at the edge. Locke fights the one his tactics pick.

At each impact, the real `RollAttack` gives damage, to-hit and roll. A
block cuts the to-hit by 50, and a blocking Locke takes nothing from a
glance (ResolveHit's rules). Then the real `Damage` call applies the hit,
with the attacker set, so death, `Died` and `Killed` run as they do in
play. A player kill calls the three award functions, as `ResolveHit`
does.

### 6.4 Tactics

`TShadowTactics` is a player's head, with the player bot's profile
(PLAYER_BOT.md §3): reaction time, block chance, aggression. Its rules
come from GRIND.md §9:

- Rotate buttons: never the same button three times in a row.
- Strike into a monster's swing (+25), and block its wind-up when the
  reaction time allows it.
- Keep fatigue above the next blow's cost. Back off to recover rather
  than throw the fatigued blow.
- Drink a potion below the health floor: one more of the strongest
  attacker's best hits.
- Cast when a spell beats a blow on expected damage per second within
  the mana budget, and to grind Invoke just before a levelling kill.
- Disengage if no potion is left and the next exchange is likely lost.
  Disengaging succeeds only if Locke's run speed beats the monster's;
  otherwise he fights on.

**Should Locke take this fight?** Before a planned fight, and to price
encounters, the tactics run the duel on a scratch copy of the numbers
with their own random stream, 64 times. That gives the win rate and the
expected health and potions spent. A player sizes up a fight the same
way, so this is judgement, not cheating. The real fight is then thrown
once, on the game's stream.

### 6.5 Death

If Locke dies, Shadow does what a player does: it loads the newest save.
The ledger resets to the save. The death and the reload are logged, and
the stage's death count rises. Two deaths in one stage fail the run,
because a balanced plan shouldn't lose a fight it chose to take twice.

### 6.6 Calibration: is it realistic?

The duel model is right when it matches real fights. The combat arena
(COMBAT_ARENA.md) already runs real ones, headless and deterministic,
with the player bot pressing Locke's keys. The calibration suite runs the
same matrix both ways:

- monsters: Araknid, Under Druhg, Mongrel Druhg, Thrush Druhg, Kantha,
  Pale Ogrok, Vashar and Baez;
- Locke: levels 1, 4 and 8; fists, a sword and a club;
- 200 seeds each.

For each cell it compares the distributions: win rate, duration, health
lost, blows landed by each side, and blocks. A tolerance gates it, and
the result goes into the report. When the two disagree, the model's
cadence or tactics are tuned, never the retail numbers.

The same switch makes the real fight available inside a run:
`--shadow-combat=real` hands each `Fight` to the player bot in the real
game, at turbo speed. It is slower, but it is the referee for a stage
that fails only in the duel model.

### 6.7 Spells

`Cast(spell, target)` calls the real `CastByName`: cooldown, mana, the
Invoke skill roll and Invoke experience, as in play. It then applies the
spell's damage with the pure part of `TSpell::Damage`, extracted the same
way as §6.2: `TSpell::Damage(caster, target, spelldata, variant)`, static,
needs no spell object (`random(min,max)` cut by magic resistance,
`SpellDamageInc`, a player target's `DmgResMagical`), through the real
`Damage` and `AwardKillExp`; the spell's own `Damage(target)` calls it.
Buff spells set the same stat effect the real spell does. Missile flight
is not modelled; a cast
reaches its target at the spell's cast time plus a flight time of
distance over missile speed.

## 7. Monster generators

The game needs them, and the port has none: `addmonstertype` is
`CmdNotPorted`. Shadow's grind depends on them, so they are ported as a
real game class, retail first (`recon/classes/cls_0x5b4cd4.cpp`; GRIND.md
§3.1):

- `TMonsterGen`, a HELPER: up to 5 slots of {type, frequency, treasure,
  start} and a cap;
- its Pulse: off screen only, one spawn per slot every 70–91 s, under its
  cap within 300 units;
- the spawn: 20 tries for a walkable spot 50–150 units out, full health,
  the slot's treasure;
- Load and Save in the retail layout (SAVE_GAME.md);
- the four script commands that are not ported today.

The play loop gets them too. That closes the gap GRIND.md §10 lists.

## 8. Money and growth

### 8.1 The shopping list

`TShadowPurse` shops when Locke is in town. In order:

1. **Sell.** Sell every item that is not equipped, not a quest item (the
   plan's keep list, plus `Value −1`), not better than what he wears,
   and not a stack (selling a stack pays for one unit). Sell to the
   merchant who buys that kind (GRIND.md §7.1).
2. **Healing stock.** The next grind session's expected health loss comes
   from the duel model against that site's monster mix. Buy enough Lesser
   Healing and Lesser Restoration to cover it, plus one spare. Healing
   is never cut to fund gear.
3. **Armour.** Protection per gold, per slot, within MinStrength and
   MinConstitution.
4. **Weapons.** Compare by the duel model's kill rate against the next
   site's monsters, using the style rule (§8.4).
5. **Jewellery.** By its STATLINE's value to the duel, when gold is left.

Each candidate purchase is scored by the duel model's gain in expected
kills per hour, divided by its price. The purse buys greedily down the
list while gold lasts, and always leaves the reserve for the next
healing stock. Stock follows `Level / 3 + 16` (GRIND.md §7.1), so the
purse re-checks it every three levels.

### 8.2 Potions and food

Food and potions go through `TFood::Use`, which must first be the retail
one (§12). Rules:

- A potion is drunk in a fight below the health floor (§6.4).
- Food is eaten out of combat, cheapest per point first.
- Belt boxes hold the healing potions, as a player's would.

There is no hunger in Revenant. Food is healing.

### 8.3 Level-ups

Before each level-up, `Train` sets the two attribute markers (GRIND.md
§1.4). The policy depends on the style:

- **Fists**: Agility and Strength.
- **Weapons**: Strength and Agility.

Constitution is chosen instead while armour Locke could afford needs
more of it. Mind is chosen once the plan's spells need more mana than
Locke has. A marker that reaches 29 moves on, as the game does.

### 8.4 Style

The plan sets a style: `hands`, `weapons` or `mixed`. GRIND.md §9.1 is
the default: fists first-class. Locke keeps a share of kills barehanded
all game, because Baez in the arena must be beaten with fists. He
switches to a weapon against monsters with a high `hand` resistance, and
once a weapon's damage is several times 12. Jong's lessons force the
Training Sword anyway.

### 8.5 Grinding

`TShadowGrind` picks the ground and decides how long to stay.

**Sites.** Hostile placed monsters and generators are clustered within
600 units (the playtest `grind.py` survey, in C++). Each site is scored
by fit: monster Value against Locke's level, with the `hand` resistance
applied in the fists style. The best site within reach is chosen. Ties go
to the nearest, and a site Locke has recently cleared is skipped.

**Linger.** He stays while all of these hold:

- the stage's readiness target (§9.2) isn't met yet;
- health is above the floor, or a potion is left;
- the healing stock is above the reserve for getting back to town;
- the pack has a free slot for loot.

At a generator site, staying means waiting off screen for spawns. The
wait runs at turbo speed (§11).

**Return to town** when the stock is spent, the pack is full, gold buys
the next upgrade on the shopping list, or the target is met. Then shop,
and go out again.

The trip uses the town teleports wherever the route graph has them
(§5.3):

1. Travel to the nearest stone (a short leg, with its fights).
2. The stone sends him to the Keep.
3. Walk out of the Keep to Misthaven's shops, and shop.
4. Walk back to `TownTel0`, which returns him to the highest stone he
   has touched.
5. Travel back to the grind site.

Before the first stone (levels 0 to 4) the trip is a walk both ways, priced
like any other (§5.5).

**Training.** Jong's lessons fall due at levels 1, 3, 6, … 24 (GRIND.md
§2.3). A lesson is a scene plus a fight on the dummy. The lesson's move
must land, which the duel model does through `LastAttack`. The grind
loop goes to Jong whenever a lesson is due and he is reachable.

### 8.6 Spells

Talismans come from the story (Olihoot's Sky and Sun) and from loot.
When Locke holds a combination that makes a spell he hasn't learned,
`Invent` makes it, as a player experimenting in the spell pane does.
Shadow knows the combinations from spell.def; a player would find them
by trying. The new spell goes into a quick-spell slot if it is useful to
the style: damage, a fist buff, healing, cure poison.

## 9. The plan

### 9.1 The file

The plan is a text file in the spirit of the game's own `.s` scripts:
one step per line, grouped into stages. `tools/shadow/ahkuilon.plan`
plays the main quest of STORY.md, S0 (the Keep) to S15 (Yhagoro).

```
# The Ahkuilon main quest, new game to credits.
module   Ahkuilon
style    hands
keep     "Golden Sun Key" "Diamond Ring" "Amulet of Rogoss" "Solifuge Key" ...

place    KeepGate   level 0  1842 24388 451
place    Tavern     object Gus1

stage keep "The Keep"
    start      newgame
    scene                            # the opening plays; control comes back
    fight      Rahul  win
    expect     RAHULSTATE = 0
    talk       RandK  choose 1 1
    expect     RANDSTATE = 1
    ...
    save       "left the keep"

stage s1 "Gus's deal"
    ready      level 2
    talk       Gus1  choose 1 1 1
    expect     MISTSTATE = 2
    save       "gus deal"

stage s3 "The Ancient Tower"
    ready      level 6
    use        TOWERIN  with "Golden Sun Key"
    fight      Sidious  win
    take       chest@4724,5092  "Diamond Ring"
    expect     MISTSTATE = 3
    travel     DRAGONOUT
    save       "dragon dead"
```

Steps:

- `start`
- `travel` (also implied by `talk`, `use`, `take`, `fight`)
- `talk … choose …`
- `use … [with …]`
- `take`
- `fight … [win]`
- `scene`
- `grind until …`
- `shop`
- `train`
- `expect` (game state, items, level)
- `save`

Objects are named by instance name, by type name, or by `type@x,y` when
there are twins (STORY.md's two Jasons and two Shegras).

### 9.2 Readiness

`ready` says what Locke needs before a stage starts: a level, an item, a
health stock. If it isn't met, the grind loop runs until it is (§8.5),
with shopping trips and Jong's lessons. The readiness levels in the shipped
plan come from the duel model's win rate against the stage's boss (Sidious
level 5, 800 HP; Baez level 8, 650 HP, with fists), with a margin.

### 9.3 Checks

- `expect` reads story variables directly through
  `ScriptManager.GameState`. That is why Shadow runs in the engine, not
  over the control channel.
- At the end of each stage, the run also checks:
  - the stage's NPCs that must stay alive (STORY.md §6) are alive;
  - the keep list is still in the pack;
  - control is on.

### 9.4 Story triggers

Going to certain places changes the town. A CUBE, a USE or a
conversation writes a story variable, and the town's scripts read it: Gus
moves to his house, a shop's stock grows, the townsfolk get new lines. Two
things make this easy to miss:

- A writer fires only where Locke actually is (a CUBE tests his position,
  §5.4).
- A reader runs only while its area's script is loaded, which follows the
  camera (`TAreaManager`). A town `ALWAYS` block that reacts to a new
  state runs only once Locke is back in town.

So the plan has to go to those places and come back.

- **[STORY_TRIGGERS.md](STORY_TRIGGERS.md)** maps every story variable,
  generated from the module's scripts by `tools/shadow/storymap.py`:
  - who writes it (trigger, object, place, gate);
  - who reads it, and in which area that reader runs;
  - what it enables.
- **The ledger logs every change of a story variable** with the script
  that made it and where Locke stood. That is the run's record of how the
  story advanced.
- **The coverage gate.** The map's JSON lists the writers a full
  playthrough must hit. At the end of the run, every required writer must
  have fired, and every town reader of a changed variable must have run
  after the change. A writer the run missed fails it, by name and place.

### 9.5 Coverage

The script step observer counts every script line that runs. The report
gives the coverage per script file and per trigger block, so a run shows
which parts of the game it exercised, and which it never touched.

## 10. Saves

`Save` waits until all of these hold:

- control is on;
- no scene, fade, load or dialog is up;
- no monster is within sight;
- no corpse within 640 units still carries loot (the F1 hazard);
- it is not between a gating death and its script's reaction
  (STORY.md §6: Sidious, Baez, Jhaga). The plan marks those windows with
  `fight … win`. The save waits for the stage's `expect` that follows
  it.

Then it saves through `TGameSession::RequestSave` to `Shadow <stage>`, and
exports the slot to the waypoint catalogue,
`~/RevenantRevisited-testsaves/waypoints/port/shadow-<name>--<time>/`.
That is the format `tools/playtest/waypoints.py` reads (`waypoint.json`,
`slot/`), with `story: "shadow"`, the seed and the plan's hash, so
`play.py --from` and `autotest.py` can start from any of them.

Each waypoint is load-checked before it is kept. A fresh process loads
it, runs 48 ticks, and requires the play screen, Locke alive, and the
same level and position.

## 11. Time

The port runs one tick per frame under `--fixedstep`, and a headless
frame comes at the display's rate, so a run would take hours. Shadow
needs the clock to run fast:

- **Turbo.** Under `--shadow`, each drawn frame runs several simulation
  steps. Each step is a whole frame's worth of simulation: a fixed time
  step, the input queue's drain, one tick and the per-frame update list.
  Only the last one draws. revmain runs as many steps as fit in a 12 ms
  budget, so headless runs as fast as the simulation allows. Because every
  step drains input, a key Shadow presses acts on the next tick, not after
  the rest of a large batch.
- **Movies** are skipped the way a player skips them (`TMoviePane::Finish`,
  the ESC path).
- **Level loads** are whole and synchronous, as in a deterministic
  playtest session (`DeterministicRun`).

## 12. Port fixes Shadow needs

These are real gaps that a player hits too. Shadow just finds them
first. Each is fixed in the game, retail first:

| Gap | Fix |
|---|---|
| `TFood::Use` is the 1998 stub: no decrement, no delete, no poison cure, no FOODGOOD1 | port `0x0050e780` (GRIND.md §6.2) |
| No monster generators | `TMonsterGen` (§7) |
| `RequestSave` doesn't refuse while control is off; only the menu does | the gate moves into the session, as retail's `0x0047e850` path has it |
| `Teleport` doesn't set OF_ONEXIT: landing on a strip activates it | as retail: verify in the emulator, then match |
| NOWALK extract leaves holes outside the sector window | re-stamp neighbours on any loaded level |
| Hit dice inside `IsValidAttack` | `RollAttack` (§6.2): done |

## 13. Outputs

A run writes `shadow/<plan>-<seed>-<time>/`:

- `shadow.jsonl`: one event per line. Events cover stage and step
  start and end, travel proofs, fights (each blow with its roll), loot,
  shop transactions, level-ups, saves, deaths and ledger violations.
- `summary.txt`: per stage, the game time, wall time, Locke's level, gold,
  kills, deaths, potions used and waypoints written. Then pass or fail
  with the failing step.
- `coverage.txt`: script coverage (§9.5) and the story-trigger gate (§9.4).
- `revenant.log`: the game's own log.

Exit code 0 on a pass, 1 on a failure, 2 on a ledger violation.

## 14. Phases

| Phase | Delivers | Proves |
|---|---|---|
| **P0 Spine** | `--shadow`, turbo, `TShadowRun`, plan parser, ledger, log; verbs `talk`, `choose`, `scene`, `expect`, `save` | the opening runs to "control on" headless in seconds, and its waypoint loads |
| **P1 Nav** | walk field, regions, blockers, the transition graph with the script scan, A*, cubes on the way, encounters, travel time, proofs | the Keep chain's walks, the shops, the tower door locked without the key |
| **P2 Duel** | `RollAttack`, the duel model, tactics, real outcomes, the calibration suite | Rahul; then the grind math against the arena |
| **P3 Generators** | `TMonsterGen`, retail first | respawn cadence matches the emulator |
| **P4 Purse** | retail `TFood::Use`, public shop verbs, shopping, level-up markers, Jong, spells | a grind loop from level 1 to 6 with shopping trips |
| **P5 Story** | `ahkuilon.plan` S0–S15, readiness, coverage, waypoints | new game to credits, every milestone saved |
| **P6 Hardening** | 20-seed sweeps, waypoint retail-interop checks, local CI target | the run is a regression gate |

## 15. Risks

- **Script transitions with complex gates.** Trying edges live (§5.3)
  avoids static evaluation, but a scripted edge that silently does
  nothing costs a replan. The script scan logs every conditional edge it
  tried.
- **Abstract distance.** Bands without positions can't model being
  cornered, or monsters blocking each other. Calibration (§6.6) measures
  how much that matters, and `--shadow-combat=real` is the referee.
- **Duel versus boss AI.** Baez, the Solifuge, Jhaga and Yhagoro have
  special behaviour (`bossai.cpp`). The duel model takes their attack
  choice from the boss AI's tables where it is pure. Otherwise it plays
  them with `--shadow-combat=real`.
- **Simulation cost at turbo.** Level 0 is big. If one tick of the window
  is slow, travel time dominates. Measure it in P0.
