# Combat dojo: combat and spell casting A/B against retail

Plan and ledger (started 2026-10-07). The combat track checks the port's
combat, movement and spell code against the shipped game by running
retail's own functions in the in-process emulator on states we control,
running the port's functions on the same states, and comparing what each
decided. One behaviour at a time, milliseconds per case; then multi-tick
sequences built from behaviours that already match.

Same machinery as gameflow's ([../gameflow/RETAIL_AB.md](../gameflow/RETAIL_AB.md)):
a retail fixture in the emulator's combat slot
(`tools/retail_runtime/slots/combat/`, main checkout), the port's
`Revenant --retail-ab=<target>` (`src/retailab_combat.cpp`), one compare
command (`tools/retail_ab/retail_ab.py <target>`). Forensics per behaviour
go in [forensics/](forensics/) as [AGENT_GUIDE.md](AGENT_GUIDE.md) asks;
this file is the plan, the kata ledger and the results.

Priority (2026-10-07, from the author): **orbit and movement first.** In
the port, characters don't circle each other in combat and movement is
wrong. Damage, attacks and spells follow.

## 1. How a kata works

A kata is one retail behaviour with a closed set of inputs and an
observable result.

- **Retail side.** The fixture builds the inputs in guest memory as
  retail's own structures (retail constructors where they exist; the
  layouts in §6 otherwise), calls the original function and dumps the
  result as JSON. Retail code runs natively except at **seams**.
- **Port side.** The same case drives the port's function on port objects
  built for it (fixture subclasses that answer the same seams), and the
  same JSON comes out.
- **Seams sit at the same level on both sides.** A seam answers a query
  from the case (a state exists, a stat's value, the world's characters)
  instead of running the code behind it. Both sides answer it from the
  same case data, so the comparison tests the code between the seams and
  nothing else. Every seam call is recorded and is part of the compared
  result: which queries a function makes, in what order, is behaviour.
  What sits behind a seam gets its own kata later (imagery state lookup,
  the stat system, the map iterator).
- **Compared result.** Everything the function decided: its return value,
  the action blocks it created or changed (by meaning, not raw bits:
  §6.2), angles, the fields it wrote on the character, its seam calls, the
  RNG draws (§3).
- **Coverage.** The fixture records which basic blocks of the function
  under test the case set reached. A kata is complete when every reachable
  branch of the retail function has a case (or the unreachable ones are
  listed).
- **Evidence.** Each run records the retail build hash, the port commit,
  the case-set hash and every difference (the gameflow driver's
  `report.json`).

## 2. Kata ledger

Status: `[ ]` not started · `[~]` fixture built, differences open ·
`[x]` matches · `[!]` blocked. Addresses are retail; verified in the
disassembly unless marked *(order)* (identified by vtable/source order
only).

### M — movement and orbit (first)

| # | Kata | Retail | Port | Status |
|---|---|---|---|---|
| M1 | action-state core: SetRoot / SetDoing / SetDesired / TryCommand / ForceCommand / UpdateAction | `0x4db2d0` `0x4db340` `0x4db3a0` `0x4db450` (Try) `0x4db4d0` (Force) `0x4db1d0` | `TComplexObject::*` | [~] SetDesired, ForceCommand ported (exercised by M3/M5); own kata open |
| M1u | the action tick: UpdateAction (stealth reset, sleep, ResolveAction dispatch, the done rule, the fall, TryCommand), ResolveMove, ResolvePivot, ResolveSay, ResetStealthValues, Visibility | `0x4c3260` `0x4c3490` `0x4c5e90` `0x4c8470` `0x4c8400` `0x4cdbb0` `0x4c5aa0` | `TCharacter::UpdateAction` and the resolvers | [x] 255/255 (`combat-update`; six mutations caught) |
| M2 | angle/distance kernels: AngleDiff, ConvertToFacing, Distance, ConvertToVector, object Distance/AngleTo | `0x46ded0` `0x46dc60` `0x46de60` `0x46db20` `0x46ea20` `0x46ea90` | `AngleDiff`, `ConvertToFacing`, `Distance`, `ConvertToVector`, `TObjectInstance::Distance/AngleTo` | [x] 30 batches, every angle pair, ~28k vectors |
| M3 | combat walking: Go(angle), empty world | `0x4ce350` | `TCharacter::Go(int)` | [x] 762/762 |
| M4 | combat walking with retargeting (world + sight/hearing seams) | `0x4ce350`, FindCharacters `0x4cd690` | same | [ ] |
| M5 | combat resolve tick: ResolveCombat / ResolveCombatMove (+ SetFighting `0x4d4790`) | `0x4c7980` `0x4c7f80` | `ResolveCombat`, `ResolveCombatMove` | [x] 654/654 |
| M6 | player input per tick: UpdateMove → Go / Stop / Leap (Block and the bow aim seamed: their tracks), and Stop | `0x47de30`, `0x4cee70` | `TPlayScreen::UpdateMove`, `TCharacter::Stop` | [x] 280/280 (`combat-input`; two mutations caught) |
| M7 | displacement: Move / MoveStep (velocity, blocking, shove), FindClearPath, CharBlocking, GetWalkHeight / GetWalkHeightRadius | `0x4c46d0` `0x4c3bc0`, TPlayer `0x518df0`, `0x4c39d0` `0x4d4db0` `0x452e10` `0x4530a0` | `TCharacter::Move`, `MoveStep`, `Blocked`, `CharBlocking`, `TMapPane::GetWalkHeight*` | [x] 786/786 (`combat-move`; six mutations caught) |
| M8 | orbit sequence: N ticks of Go → Pulse (UpdateAction) → Move → SetObjectMotion → frame → NextFrame, with SetState / ResetState / T3DImagery::SetObjectMotion / NextFrame as original over motion tables; walking, strafing round a target, a monster curving, walls, loop / ping-pong / reverse roots | `0x490bd0` order; `0x46f250` `0x46f1e0` `0x40cd20` `0x40cc40` `0x470cc0` | the tick, `TObjectInstance::SetState` / `NextFrame`, `T3DImagery::SetObjectMotion` | [~] 32/32 sequences, ~1,500 ticks (`combat-sequence`); motion tables synthetic, real I3D motion next |
| M9 | AI combat movement: approach, combat range, retreat, wander | `0x4c8b60`, `0x4c9790` | `TCharacter::AI` | [ ] |
| M9b | perception: IsEnemy, CanSeeCharacter, FindCharacters (hearing inlined), Hearing, Sight, HasSeenMe / SetHasSeen, the map iterator's characters | `0x4c89c0` `0x4cd540` `0x4cd690` `0x4cda80` `0x4cdb30` `0x4c58f0` `0x4c5940` | same | [x] 1007/1007 (`combat-perceive`; nine mutations caught); the line of sight `0x4533d0` is the case's on both sides (its own kata open) |
| M10 | leap, side step, knock back, pivot, stop | `0x4d2be0` `0x4d6220` `0x4d3750` `0x4c8470` `0x4cee70` | same | [~] SideStep, Leap, StartRetreat `0x4d5fc0`, KnockBack: 482/482 (`combat-steps`; five mutations caught); Stop in M6; Pivot `0x4c8470` open |

### C — melee

| # | Kata | Retail | Port | Status |
|---|---|---|---|---|
| C1 | CalculateDamage | `0x4c4860` | `TCharacter::CalculateDamage` | [x] 40 cases x 594 inputs (characters; the player's resist/armour slots open) |
| C2 | Damage (impact / death choice), TObjectInstance::Damage, TPlayer Killed / Died | `0x4c4950` `0x46e970` `0x518ed0` `0x518f90` | `TCharacter::Damage` and its parts | [x] 1137/1137 (`melee-damage`); the floating damage number waits for a post-character overlay (COMBAT_HIT.md §9.2) |
| C3 | attack choice: ButtonAttack / IsValidAttack / FindButtonAttack / FindPcntAttack / RandomAttack | `0x4d2480` `0x4d1120` `0x4d1ff0` `0x4d1eb0` `0x4d2900` | same | [x] 5143/5143 (`melee-attack-choice`) |
| C4 | hit resolution: ResolveAttack → ResolveHit (to-hit, tiers), OnAttacked | `0x4c6dd0` `0x4c62b0` | same | [x] 7855/7855 (`melee-hit`) |
| C5 | block / dodge / impact / dead resolvers, EffectCombatFlash | `0x4d2e30` `0x4d30f0` `0x4d3150` `0x4c74b0` `0x4c7810` `0x4c77a0` `0x4c8500` | same | [x] 2100/2100 (`melee-resolvers`); the eight dodge controls (commands 0x30-0x37) are input work |
| C3b | the AI as a whole: acquisition, the attack branch, the move branch, the waypoint, AI_PerMonster (non-boss) | `0x4c8b60` `0x4c9b70` | `TCharacter::AI`, `AIAttack`, `AIMove`, `NearestWaypoint`, `AIPerMonster` | [x] 925/925 (`melee-ai`); bosses (Baez, Solifuge, Jhaga, Yhagoro) are BURNDOWN phase H; Wander `0x4c9790` open (M9) |
| C6 | Pulse: regen, fatigue, poison, chains, death | `0x4c1bb0`, TPlayer `0x518aa0` | `TCharacter::Pulse`, `TPlayer::Pulse` | [ ] |
| C7 | experience and level-up | `0x51a630` | `AwardKillExp` / `AwardSkillExp` | [ ] |

### S — spells

| # | Kata | Retail | Port | Status |
|---|---|---|---|---|
| S1 | talismans → spell, quick spell, fizzle | `0x51b5d0` `0x51b7c0` | `TSpellList::GetSpellDataByTalismans`, `TPlayer::HasTalismans` | [x] `spell-lookup` 75, `spell-talismans` 340, `spell-quick` 49 |
| S2 | cast gates: mana, wait, fail roll | `0x53fe80` (CastByTalismans), `0x4d5c20` | `TSpellManager::Cast*` | [x] 2071/2071 (`spell-cast`) |
| S3 | spell damage, poison roll | `0x53f560` `0x53f090` | `TSpell::Damage`, `TSpell` ctor | [x] `spell-new` 426, `spell-damage` 1090 |
| S4 | missiles: flight, hit, damage | `0x510220`, AreaDamage `0x4de3c0` | missile effects | [x] `missile-area` 64 (AreaDamage `0x4de3c0`), `missile-arrow` 119 (TAmmo), `missile-bow` 114 (DrawBow ... ResolveBowShoot), `missile-fireball` 21 cases / 840 ticks; spell lifetimes, RANGEDAMAGE effects and the Iced effect open |

### D — data

| # | Kata | Retail | Port | Status |
|---|---|---|---|---|
| D0 | which rules files retail reads (loose `Resources/` against `resources.rvr`) | `0x4a13f0` -> `0x4a1240` | `rev_fopen` | [x] packs first (§5.6) |
| D1 | rules.def / stats.def / char.def / weapon.def / armor.def parse (`combat-data`): TRules::Initialize + Load, every CLASS, CHARACTER, ATTACK, IMPACT, WEAPON, ARMOR, STATLEVEL, then BindTypes | `0x48b690` `0x48b990` `0x4891c0` `0x489850` `0x48cab0` | `TRules::Initialize`, `Load`, `SClassData::Load`, `SCharData::Load`, `BindTypes` | [x] 4 cases (shipped, GameSpeed 5, GOG loose, every-tag edge), all fields (~39,700 per shipped case) |
| D1s | spell.def parse: every SPELL | `0x53ead0` | `SSpellData::Load` | [x] 3/3 (`spell-data`) |

## 3. Determinism

- **RNG.** Retail draws from the MSVC CRT `rand` (`0x58c582`, LCG
  `s = s*214013 + 2531011`, `(s>>16) & 0x7fff`, seed per thread at
  `ptd+0x14`) through `random(lo, hi)` (`0x483300`: `lo` when equal, else
  `lo + rand() % (hi-lo+1)`). The port's `random()` (`revutils.cpp`) uses
  the host libc `rand`, a different generator. For A/B both sides draw from
  one **tape**: the case's list of values, handed out in order; each side
  records every draw as `(lo, hi, value)`. The draws' count, order and
  ranges are compared; that is how a missing or extra roll shows up.
- **Reseeding.** Retail reseeds the global stream from the wall clock when
  it gives an object a unique id (`0x44ce30`, `0x451150`, `0x451430`:
  `srand(rand() + GetTickCount())`). A fixture that spawns objects pins
  `GetTickCount` and `time`.
- **Time.** Fixtures set the game frame (`GameFrame` `0x47e920` reads the
  PlayScreen at `0x65caf0`) and the clock per case; nothing reads the wall
  clock. Cadences keyed on the frame (retarget every 8 frames for the
  player, 32 for monsters, `(GameFrame ^ id) & mask`) are case inputs.

## 4. Realistic state: captured retail games

Forged fixtures need every global and field a function reads to hold the
values it holds in a running game. Reading them off statically doesn't
scale (the IsValidTarget PlayScreen flags in §5 are an example). The plan
for that:

- **A capture build** (named retail build, recon/retail_asm/patches/):
  a hook in the play loop that, on a key, writes the process's committed
  memory (VirtualQuery walk) and the CPU-independent globals to a file, and
  a savegame beside it.
- **Captured in the DOSBox lab** at chosen combat moments (the Keep
  fight, an Araknid in the forest).
- **Loaded by the emulator** as a whole guest image: the Player, the map,
  chardata, imagery and every global as the game had them. Retail
  functions then run on real state; seams stay only for what the capture
  can't hold (devices, threads).
- **The port** loads the savegame from the same moment (save interop is a
  gameflow invariant) and runs the same ticks and input.

Until then, every global a fixture sets is listed in its kata as an
assumption, and both values are run where the right one is unknown.

## 5. Findings (2026-10-07/08)

Status 2026-10-08: the orbit is ported (Go, ResolveCombat,
ResolveCombatMove, SetFighting, SetDesired, ForceCommand, IsValidTarget,
edge-to-edge Distance, CombatFace) and matches retail on every M3/M5
case. Items 1-4c below describe the port *before* that; they are fixed
unless noted. Open: the action core's own kata (M1), displacement (M7),
sequences (M8), and everything melee (C, COMBAT_HIT.md), spells (S) and
data (D).

Detail and evidence in [forensics/COMBAT_MOVEMENT.md](forensics/COMBAT_MOVEMENT.md).

1. **`CombatFace`: retail faces the opponent while moving in combat.**
   `Revenant.ini` `CombatFace = Yes` (default; global `0x5d7a64`, also set
   by the Options screen). With it on, Go (`0x4ce350`) and ResolveCombat
   (`0x4c7980`) keep a moving player's facing on the target and pick the
   strafe animation from the angle between moving and facing
   (GetAngleMoveAnim): that is the orbit. Monsters always do it. The port
   has no CombatFace; its ResolveCombat and Go are the 1998 versions.
2. **ResolveCombat differs from the 1998 version** the port carries:
   target re-acquisition is gated by frame cadence (player every 8 frames,
   monsters every 32, unless the AI is off), a new target must lie within
   45° of the move direction, turn rates are `((|Δ|−32)⁺/32)·4 + 8`
   (monsters, standing player) or `·8 + 16` (moving player's new step),
   and the goto target is cleared only when `+0x288` is 0.
3. **The action-state core differs.** Retail SetDesired refuses a new
   block while a pending one waits and *doing* has priority; the port
   checks the desired block's priority. (Both send an interrupting block to
   ForceCommand: the recon files for `0x4db450` / `0x4db4d0` have
   Try/ForceCommand swapped.) The port's SetState also skips restarting a
   looping state that is set again, which retail does. These sit under
   every movement.
4. **Action block flags moved.** Retail: firsttime `0x1`, transition
   `0x2`, priority `0x10`, interrupt `0x20`, nowaitdone `0x40`, dontforce
   `0x80`, stop `0x100`, waitpivot `0x200`, noroot `0x400`, goto `0x1000`;
   the port's 1998 bitfield has priority `0x8` through noroot `0x200`. A
   bit was inserted below priority in retail (not yet identified).
4a. **Players never pivot in combat** (Go): retail makes the strafe step
   at once and turns on the way; the port stops to pivot when the facing is
   off by more than 45°. Retail Go also always writes the held angle into
   the root block, probes 4 units ahead, refuses a dead mover, and changes
   an existing step in place.
4b. **Distance is edge to edge.** Retail TCharacter::Distance subtracts
   both characters' Radius (class.def); the port measures centre to
   centre, so every combat range is off by `r1 + r2` (32 for two radius-16
   characters).
4c. **Retargeting is on a frame cadence** (player every 8 frames, monsters
   every 32) and needs the new target within 45° of the move direction;
   the port retargets every tick. Retail checks sight; the port doesn't.
4d. **The player-control flag** (PlayScreen `+0x5e0`): while a script holds
   control, monsters can't target the player.
5. **CalculateDamage** (`0x4c4860`): retail is
   `max(1, (100 − Resist(type)) · ((100 + mod) · d / 100) / 100 − Armor())`,
   then halved for magic types (6–9) with charflags `0x200` or physical
   with `0x100`, zero for magic types on Baez (`+0x280 == 1`), and ÷7 with
   `0x10` unless freeze. Resist is `chardata->damagemods[type]` for
   characters, a stat for the player; Armor is `chardata->armor` or the
   player's equipped protection. The port adds `damagemods` (sign flipped)
   and ignores armor and the flags.
6. **Data provenance (D0, settled).** Retail opens every combat data
   file (rules.def, char.def, class.def, spell.def, weapon/armor/equip,
   stats, master.s) from the packs first (`0x4a13f0` -> `0x4a1240`, arg 0)
   and only then a loose file; only the .def screen loader `0x4377c0` is
   loose-first. So the retail lab's stray loose files (the 1998 rules.def,
   a 14-values-different char.def) were never read, and the DOSBox
   captures used the 1999 data. feature/combat reads the packs first too
   (gameflow 36faa5f); main still read the loose 1998 rules.def. The parse
   differed (forensics/COMBAT_DATA.md §11) and now matches (kata D1,
   2026-10-08): ENEMIES/GROUPS 80 bytes (Locke's enemies keep
   "supernatural,undead"), TOHIT*, TOHITDAMAGE and AMMODATA, FATIGUEATTACK
   (maxfatigue, flag 0x10000), swipe frames, MAGICATTACK conditions, every
   CHARACTER tag (MAGICFREQ, BLEEDER, NOPARALYZE, RETREAT*, RUNFATIGUE,
   POISONCHANCE, LABELHEIGHT, SWIPEFULL, ATTACHEFFECT), CLASS WEAPONS,
   retail's defaults, late name binding (BindTypes), and retail's fatal
   errors (an unknown tag, a repeated CHARACTER / WEAPON / ARMOR, an unknown
   CLASS). Nothing reads the new fields yet: to-hit (C4) and the AI (M9,
   C3) do.
7. **Distance callers (audit 2026-10-08).** Retail's script value
   `getdistance` (`0x41f991`) and SnapDist (`0x46f090`) call the virtual
   (slot 4), so a character's distance there is edge to edge in retail
   too; the trigger test doesn't use object Distance. The port's virtual
   Distance matches all three.
8. **The displacement step (kata M7, 2026-10-09).** The port's Move was
   the 1998 MoveStep without retail's changes (forensics/COMBAT_MOTION.md
   §6 #10-#20); now retail's, verified per case:
   - the ground: more than 16 above it drops to it at once (MOVE_FALLING)
     rather than falling over ticks;
   - no motion and no velocity: NOTMOVING with accum and shovedir reset,
     before any probe;
   - a MoveTo repeats MoveStep up to ten times, ignoring characters;
   - substeps split on `|x| >= |y|` (ties to x) and are nudged to land
     on the target exactly;
   - a blocked substep still reports MOVED, keeps its fraction and the
     loop goes on;
   - blocked steps go through when both the new and the current position
     touch someone; overlapping characters never block each other (so
     they can part);
   - the shove is skipped only when a character blocks in combat, commits
     its side as it probes, and keeps vel;
   - FindClearPath has retail's gates (a hole, a signed step of 0x20
     between cells, dead / interactive attack / flying / invisible /
     MoveTo movers pass characters) and no FALLING exemption;
   - CharBlocking walks the loaded sectors (iterator flags 0xe0) within
     0x80 and skips interactive attackers, fliers, the invisible, MoveTo
     movers and idle players.
   GetWalkHeightRadius is retail's (absolute steps between in-range
   neighbouring cells, a hole flag) instead of the 1998 signed min/max.
   Open: retail's ReturnWalkmap masks heights to 10 bits (`& 0x3ff`,
   the top 6 bits are a field GetWalkHeight's third argument returns),
   the port's walkmap holds 16; the walk root asks the class (slot
   `0x30c`: a player with a light in hand walks "torch", `+0x2b8`
   unconfirmed), which matters only with a positive walk speed. Arena
   effect: fighters now reach each other, so the AI attack spam (C3,
   IsValidAttack's next-attack gate) shows in full (1866 attacks).
9. **The action tick (kata M1u, 2026-10-09).** The port's command states
   were the 1998 set (pending 0, executing 1, completed 2) while retail's
   resolvers, TryCommand and ForceCommand use 0 done / no opinion, 2
   executing, 3 impossible; the port now uses retail's (`COM_DONE`,
   `COM_EXECUTING`, `COM_IMPOSSIBLE`). UpdateAction is retail's:
   - a resolver's "done" still waits for the animation, but the root is
     done unless mid-transition, and only "done" drops priority;
   - the fall needs a "fall" animation and no priority, and no desired
     block is tried that tick;
   - ResetStealthValues runs off the 24-frame beat (and on it for a
     finished non-root action or negative values): 100 for an attack,
     else 70, halved sneaking, `random(1, 25)` -- a draw per character
     nearly every tick, where the port drew `random(1, 100)` with the 1998
     formula;
   - Visibility is the ambient light alone (`min(ambient, 255) * 100 /
     255`), not the 1998 sum of lights and ambient colour.
   ResolveMove bounces off a block (the angle by octant, +0x10, facing and
   move angle with it, back to the root) instead of facing the same way;
   a Goto picks its item up on arrival (ResolveMove, and in combat
   ResolveCombatMove, which also steps toward it). TComplexObject::Pulse
   drops the forced and transition marks at frame 5. ResolvePull is still
   dispatched (retail's is empty; where retail pulls levers is open).
10. **The animation layer (kata M8, 2026-10-09).** SetState now restarts
    the current state like retail (frame 0, prevstate = state, the
    animator told) instead of returning early for a looping one. Retail's
    action code re-forces a looping root when its cycle ends, so the
    restart lands on frame 0 anyway; what the early return changed was
    prevstate, and SetObjectMotion then cleared a ROOT animation's accum
    every tick where retail never does. The pose bridge blends no
    transition between a looping state and itself, so the restart shows
    no pop. NextFrame, ResetState and T3DImagery::SetObjectMotion /
    GetMotion match retail as they were. CommandDone in retail is plain
    `commanddone` (slot 0x154); the port's says done whenever there is no
    animator (every character has one in play). SetState's FreeAnimator
    for permanent-animator types stays a divergence (3D characters always
    need theirs).
11. **The held direction (kata M6, 2026-10-09).** Holding a direction calls
    Go once: UpdateMove skips it while the doing block is a move (walk,
    combat step, bow step) whose move angle is the held one. The port
    compared the object's move angle, which a step's motion data keeps
    nudging (Locke's `combatr` moves at 63), so it called Go almost every
    tick of a strafe. Stop is retail's: the root takes the doing block's
    angles with incidentals off, and the player lets go of every held
    control and of the right-button walk (the map pane's right button up,
    0x0044f140). The right-button walk is the numpad direction of the
    pointer's 45° sector (MAP_INPUT.md §5), so mouse walking and the keys
    meet here. Kept as a divergence: two adjacent arrows walk their
    diagonal (retail: the lowest bit), for keyboards with no numpad. Not
    ported: the debug camera's turn of the direction (0x006671f0, the dev
    'X' key).
12. **Knock-back (kata M10, 2026-10-09).** A blow from behind (the facing
    more than 0x48 off the blow's bearing, which retail measures without
    wrapping: a facing of 250 takes a blow at 10 as from behind) plays the
    back impact `impb` if the character has one and keeps its facing;
    otherwise one of five (`impk`, `imphh`, `imph`, `implh`, `impl`; the
    caller's variant, else `random(0, 4)`) turns it to face the blow. Both
    are named through CombatAnimName (the player's prefix), set the combat
    flash (+0x224) to 5, and force an IMPACT with priority. The port played
    `cimpk` every time and faced the blow by atan2.

13. **Perception (kata M9b, 2026-10-09).** Retail's senses are close to
    binary, and not what the 1998 source's comments describe:
    - Hearing returns 100 anywhere within HEARINGRANGE (the distance less
      the radius and 32), 0 beyond: its scaled value is held to at most 0
      and then floored at 100, a 0..100 clamp turned inside out. HEARINGMIN
      and HEARINGMAX are parsed and never read. Hearing has no line of
      sight. So any noise above 0 within range (edge to edge) is heard.
      ResetStealthValues gives 0 only when sneaking with a draw of 1 (one
      tick in 25).
    - Sight is 0 or 1 within SIGHTRANGE (centre to centre, eye to eye;
      no radius taken off), 0 beyond or asleep; SIGHTMIN / SIGHTMAX are
      never read. Seeing wants a glimpse of 99 or more. A walking
      character's glimpse tops out at 70 (100 only while attacking), so
      without infravision a monster sees him only mid-attack; it finds him
      by ear.
    - The memory (HasSeenMe) holds a character 45 seconds (0x438 frames),
      not ten. FindCharacters keeps one neither heard nor seen only while
      remembered.
    - FindCharacters, looking for enemies, marks a non-enemy as seen and
      passes over an invalid target (dead, invisible, out of combat range)
      without marking it. The first found is never scored (the best starts
      at 10000), and the head of the list takes the lowest score: with an
      angle, the edge distance times how close to the angle it is (plus 1),
      so it favours the near and the off-angle.
    - IsEnemy never counts an idle player (state bit 2). Between players it
      takes the teams (case-insensitive) and the player-killer bit (state
      bit 24, under the session's rule `0x676804`, multiplayer and not
      ported). Then: the one fighting me (a combat or bow root on me), or
      one named, typed or grouped among my ENEMIES who is aggressive or a
      player.
    The port had the 1998 versions throughout (ten-second memory, scaled
    senses, line of sight for hearing, the dead marked as seen).

## 6. Layouts used by the fixtures

Retail, verified against the disassembly where a fixture relies on them.

### 6.1 Objects

- TObjectInstance: class `+0x04` (short; 0xb player, 0xc character),
  flags `+0x08`, state `+0x0c` (short), pos `+0x10`, facing `+0x36`
  (byte), name `+0x38`, id `+0x40`, imagery `+0x54`, frame `+0x5c`
  (short), moveangle `+0xb0`.
- TComplexObject: doing `+0xd8`, desired `+0xdc`, root `+0xe0`.
- TCharacter (0x2a0 bytes, vtable `0x5a7848`): chardata `+0xfc`,
  charflags `+0x110`, retreating `+0x254`, per-monster id `+0x280`.
  Motion: vel `+0x1c`, accum `+0x28` (1/0x10000 units), inventnum
  `+0x7c` (short), movedist `+0xb4`, movevert `+0xb8`, movetopos `+0xec`,
  movepos `+0xf0`, forcenomove `+0x10c`, shovedir `+0x11c`, the sight
  fields a blocked step clears `+0x254` / `+0x258` / `+0x25c`. Move is
  slot `0x114`, SetPos slot 8 (`0x46ed70`).
- TPlayer (0x674 bytes, vtable `0x5b4f30`): state bits `+0x36c`.
- SCharData: damagemods `+0xe4`, combat range max `+0x15c` (read by
  IsValidTarget), armor `+0x1c4`, walk / run / sneak / combat walk speeds
  `+0x1ec` / `+0x1f0` / `+0x1f4` / `+0x1f8` (-1 as shipped).

### 6.2 TActionBlock (100 bytes, ctor `0x4da9f0(name, action)`)

action `+0x00`, name[32] `+0x04`, frame `+0x24` (−1), wait `+0x28`,
angle `+0x2c`, moveangle `+0x30`, turnrate `+0x34` (16), target `+0x38`,
obj `+0x44`, attack `+0x48`, impact `+0x4c`, damage `+0x50`, data `+0x5c`,
flags `+0x60` (1). The layout matches the port's `TActionBlock` except the
flag bits (§5.4): dumps name the flags, each side mapping its own bits.

### 6.3 Seams in use

| Seam | Retail | Port | Answered from |
|---|---|---|---|
| state exists | FindState `0x477c10`, FindTransitionState `0x477c30` (slots `0x138`/`0x13c`) | `FindState`, `FindTransitionState` (virtual) | the case's state table per character |
| animation layer | SetState `0x46f250` (slot `0x18`, recorded, state stored); stand-in imagery at `+0x54`: NumStates `+0x3c`, GetAniFlags `+0x8c`, header frame counts | SetState override; a registered header from the same table, NumStates / GetAniFlags recorded | the case's state table (frames, aniflags) |
| object stats | GetObjStat `0x4d7520` (TPlayer `0x51ae30`), slot `0xdc` | Health / Fatigue / Mana overrides | the case's `stats` |
| type stats | GetStat `0x4d74d0`, slot `0xd8` | Radius override | the case's `classstats` |
| clear path | FindClearPath `0x4c39d0` (cases without a `ground`) | `TCharacter::blockedSeam` | the case's `blocked` |
| sight | CanSeeCharacter `0x4cd540` | `TCharacter::canSeeSeam` | the case's `sees` |
| world | FindCharacters `0x4cd690` (M3/M5: empty world) | `TCharacter::findCharactersSeam` | the case's characters |
| walkmap | sector lookup `0x499e10` + ReturnWalkmap `0x499720` (not recorded) | `TMapPane::walkGridSeam` (GetWalkGridHeight) | the case's `ground`: `z`, `cells` boxes, `nosector` |
| characters near a point | map iterator `0x44ceb0` / `0x44d080` (CharBlocking) | `TCharacter::nearbyCharactersSeam` | the case's `nearby`, else every character |
| position | SetPos `0x46ed70` (slot 8) | SetPos override | recorded, position stored |
| random draws | random `0x483300`, rand `0x58c582` | revutils `SetRandomSource` / `SetRandomRangeObserver` | the case's `tape`, then retail's generator from `seed`; every draw in `draws` |

## 7. The arena: deterministic auto-battles in the port

`tools/combatarena/arena.py run <scenario> [--repeat N] [--watch]` runs
the real game (headless, or a window with `--watch`) from a save slot,
sets up a fight with console commands and lets the AI fight it out:

- `--seed=N`: `random()` is retail's generator (MSVC rand, 15-bit), seeded.
- `--fixedstep`: exactly one 24 Hz tick per frame (TTime::BeginFixedFrame).
- `--combattrace=<file>`: every fighter every tick (position, facing,
  state, blocks, health/fatigue/mana, target) and the events (attack,
  cast, hit/miss with roll and to-hit, damage, death), the RNG draw count
  per tick; `--combattrace-rng=FROM:TO` adds every draw with its caller.
- `--playerai`: Locke runs the character AI (retail's charflags 0x100000
  gate) and fights on his own.
- `--repeat N` runs N copies and requires identical traces.

Scenarios (`tools/combatarena/scenarios/`): Locke vs an Araknid (Demo 1),
natural-faction fights (golem/skeleton, druid/araknid, araknid/arakna)
from retail New Game1. Natural enemies come from char.def ENEMIES; no
scripted targeting is needed. All repeat identically (2026-10-08) since
SpellManager.Pulse moved to the tick (it ran in the draw).

Known gaps the arena shows: the AI re-issues attacks almost every tick
(attack choice is still 1998; COMBAT_ATTACK_CHOICE.md); hit, damage and
fatigue are 1998 (COMBAT_HIT.md).

## 8. Running it

From the combat worktree (`worktrees/combat`, branch `feature/combat`),
after building the port (`cmake --build build --target Revenant`):

```sh
python3 tools/retail_ab/retail_ab.py combat-kernels     # M2
python3 tools/retail_ab/retail_ab.py combat-go          # M3, first difference per case
python3 tools/retail_ab/retail_ab.py combat-resolve     # M5
python3 tools/retail_ab/retail_ab.py combat-move        # M7
python3 tools/retail_ab/retail_ab.py combat-update      # M1u
python3 tools/retail_ab/retail_ab.py combat-sequence    # M8
python3 tools/retail_ab/retail_ab.py combat-input       # M6
python3 tools/retail_ab/retail_ab.py combat-steps       # M10
python3 tools/retail_ab/retail_ab.py combat-perceive    # M9b
tools/walktest/walktest.py "<slot dir>" [--pattern sweep|walks|both|none] [--exec "player.goto X Y; ..."]
python3 tools/retail_ab/retail_ab.py combat-data        # D1, every record field by field
python3 tools/combatarena/arena.py run tools/combatarena/scenarios/locke_vs_araknid.json --repeat 2
python3 tools/retail_ab/retail_ab.py combat-go --all --case go.player.cf1.f0.b64
```

- Cases: `tools/retail_ab/combat_targets.py` (generators and the compare);
  `targets_data.py` for D1 (cases are install folders written under the
  output directory; the edge case's files are in
  `tools/retail_ab/cases/combat_data/`).
- Retail: `tools/retail_runtime/slots/combat/` (versioned with the emulator;
  `fixturekit.py` holds the shared fixture helpers):
  `guest.py` (layouts, shared seams, fault reports with the callers on the
  stack), `combat_call.py` (the method per case), `data_parse.py` (D1).
- Port: `src/retailab_combat.cpp` (`--retail-ab=combat-*`), loading the
  install's class.def once (`REVENANT_DATA_PATH`, set from `--data`);
  `src/retailab_data.cpp` (`combat-data`).
- Output: `build/retail_ab/<target>/` — `report.json`, both dumps.

Speed (2026-10-07, 4 retail processes): M3 762 cases in ~1.5 s retail,
~2.9 s port; M5 654 cases in ~1 s / ~2 s.

## 9. Coordination

- **Gameflow** owns scripts, saves, the DOSBox lab and the driver this
  reuses; combat targets plug in through a registry hook, not a fork.
  Changes to combat paths in `character.cpp` are announced to gameflow
  first (it ported `beginfighting` / `specificattack` there).
- **VFX** owns effect visuals; spell damage and missile hits (S3/S4) are
  combat's, their visuals VFX's.
