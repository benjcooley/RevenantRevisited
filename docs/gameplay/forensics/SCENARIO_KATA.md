# Forensics: the whole tick (the scenario kata)

**Topic:** what one game tick of the shipped game runs, in order, from the
key presses before it to the animation frames after it; which of it plays
the game; and how kata `combat-scenario` runs it in the emulator for a
small world, tick after tick, against the port's own tick on the same
world and random tape.
**Status:** 2026-10-11. Order read from the disassembly; the kata runs it
(§5). Every address below was read in the asm (`rdis`), not taken from
Ghidra labels.
**Evidence:** the unchanged retail image (`recon/retail_asm/baseline`).
The per-object functions are documented elsewhere and not repeated:
[CHARACTER_PULSE.md](CHARACTER_PULSE.md) (Pulse), [COMBAT_MOTION.md](COMBAT_MOTION.md)
(UpdateAction, the resolvers, Move, SetObjectMotion, NextFrame),
[COMBAT_ATTACK_CHOICE.md](COMBAT_ATTACK_CHOICE.md) (AI, the attack choice),
[COMBAT_HIT.md](COMBAT_HIT.md) (hits, Damage), [PLAYER_INPUT.md](PLAYER_INPUT.md)
(UpdateMove, Command).

Why a kata of the whole tick: each kata so far runs one retail function a
case. Whole-fight behaviour -- when a monster turns, walks in, bites, how
often -- could only be compared through the DOSBox lab's video, which
draws 3-8 frames a second and can't place an event below half a second
([../COMBAT_ARENA.md](../COMBAT_ARENA.md)). The emulator runs retail's tick
exactly; the kata compares every tick.

## 1. The main loop (`0x491100`..`0x491858`)

Each iteration:

1. The message pump: PeekMessage / TranslateMessage / DispatchMessage
   (`0x491278`..`0x4912ce`) while messages wait. A key goes WndProc →
   the screen's key handler (`0x47c630`) → the control map → TPlayScreen::
   Command `0x47cf40`: an attack key is the player's ButtonAttack(n)
   `0x4d2480` there and then (cases 0x21..0x2f, `0x47d8a2`; PLAYER_INPUT.md
   §3), a held control sets the control map's state (`0x65a9c4`) and
   changed (`0x65a9c8`) masks. **So input lands before the tick.**
2. The frame timing (GetTickCount, `0x49133d`..).
3. `CurrentScreen->TimerTick(draw)` (screen slot `0x44`, `0x49143c`):
   §2.
4. `CurrentScreen+0x48`++ (`0x491454`): the screen frame, the clock the
   exit mark wears off on (Pulse's `+0x114`).

## 2. TScreen::TimerTick `0x490bd0`, the play screen's

In order (COMBAT_MOTION.md §2 has the slots):

| # | Call | Plays the game? |
|---|---|---|
| 1 | slot `0x0c` `0x47b4d0`: the side panes open / close | no |
| 2 | each pane's slot `0x48`: resize | no |
| 3 | slot `0x10` **TPlayScreen::Pulse `0x47bd20`** (skipped under a modal with bit `0x10`) | §2.1 |
| 4 | draw only: slot `0x14` (background) | no |
| 5 | slot `0x18` **TPlayScreen::Animate `0x47c2c0`**, drawn or not | §2.2 |
| 6 | draw only: slots `0x1c`, `0x20` | no |
| 7 | slot `0x24` `0x4902c0` → the top pane's slot `0x60` → TMapPane thunk `0x4545f0` → **NextFrameObjects `0x457ef0`** | §2.3 |

### 2.1 TPlayScreen::Pulse `0x47bd20`

Returns at once while `+0x5d4` (paused). Then save / load requests, area
resets, the multiplayer and HUD bookkeeping; then:

- **`+0x5e0` (the player has control): UpdateMove `0x47de30`** (the held
  direction → Go / Stop / Leap, block → Block / StopBlock, the bow aim;
  PLAYER_INPUT.md §2), then `0x47e0f0` -- the debug camera's keys, only
  in camera modes 2 and 3 (`0x6671f0`); nothing of the player's.
- `+0x6d4` → `0x47f3a0`; `0x65a298` (a level reset request) → the map
  pane's reload (`0x458ff0`, `0x44e460`, ...).
- **TScreen::Pulse `0x48fda0`**: in a network game only, the player's
  idle state (player state value 2) after 0x78 screen frames without
  input (`0x48fdaf`: `[0x66829c]` gates it); single player never sets it
  here. Then the top pane's Pulse → **TMapPane::Pulse `0x454390`**:
  - the level change `0x45b080`, `0x45f1e0` (when `+0x8f8`), `0x4537b0`;
  - **PulseObjects `0x4580f0`**: unless `NoPulseObjs` (`0x6680d4`), every
    object of the pulse set (iterator `0x44cf80` with flags `0xa0`, objset
    4): one with OF_KILL (`0x1000`) whose slot `0x3c` answers true (all 52
    object vtables in the image hold `0x477930`, `return 1`) and that
    isn't a player (`0x45815b`) is taken out of the map (`0x451840`);
    every other one Pulses (slot `0x110`: TCharacter `0x4c1bb0`, TPlayer
    `0x518aa0`), in a structured exception frame that deletes an object
    that faults;
  - **MoveObjects `0x458390`**: unless `NoPulseObjs`, every object of the
    moving set (objset 1): Move (slot `0x114`), its bits to `+0xbc`; then
    every one: SetObjectMotion (slot `0x118`);
  - the map position `0x4539d0`, the camera (`0x41c410`), the sector window
    `0x459220`.
- `0x52b9f0`: the areas (ambient, music) unless a modal has bit 8.

Inside Pulse (CHARACTER_PULSE.md §2): the spell list, the animator's Pulse
(tags, sounds), TComplexObject::Pulse → **UpdateAction `0x4c3260` with
the previous tick's move bits** (ResolveAction and the resolvers:
COMBAT_MOTION.md §3), the death bookkeeping, the player's auto-combat,
**AI `0x4c8b60`** (for OF_AI, or Wander + AI with charflags `0x100000`),
recovery, poison, the timers. The player's AI call returns at once unless
`0x100000`. So the order a character sees is

`[keys: Command] → UpdateMove → Pulse (UpdateAction → resolvers; AI) →
Move → SetObjectMotion → GameFrame++ → NextFrame`

and an AI decision of tick t is acted on by tick t+1's UpdateAction.

### 2.2 TPlayScreen::Animate `0x47c2c0`

Skipped while `+0x50`. TScreen::Animate `0x490030` → TMapPane::Animate
`0x454450`: each object's Animate (slot `0x11c`; a character's
`0x4c36b0` is its animator's Animate, slot `0x30`: the pose for drawing).
Then, unless paused (`+0x5d4`), in the editor (`0x668154`), `0x666920`,
the map pane closed (slot `0x40`), before the game is up (`0x666918`) or
under a modal with bit 8: **`+0x680`++** and **game time `+0x68c` =
GameFrame × 100 / 24** (64-bit, `0x47c37d..0x47c3af`), GameFrame =
`[+0x688] − [+0x684] + [+0x680]`; then the time of day.

### 2.3 NextFrameObjects `0x457ef0`

Unless `NoAnimateObjs` (`0x6680d8`): every object of the animate set
(objset 5); one in an inventory (slot `0xb4`) only when the Player owns it
(`+0x64`); NextFrame (slot `0x120`).

### 2.4 The port

TPlayScreen::Pulse (playscreen.cpp) → the game mode's Tick → UpdateMove
(while control is on) → MapPane.Pulse() → PulseObjects, MoveObjects (the
camera and the sector window after); then AdvanceGameFrame (the frame
count, game time, time of day: 0x47c2c0's); then NextFrameObjects. The
same order. Not in the port's tick: object Animate (the port's animators
pose at render rate). It moves nothing; whether a controller retail's
Animate runs ever draws a random number (the particle code round
`0x402842` does) is open (§6).

## 3. The kata (`combat-scenario`)

Retail: `tools/retail_runtime/slots/combat/scenario.py` (the melee
fixture's world, melee_attack.py, plus the tick). Port:
`src/retailab_scenario.cpp` (`Revenant --retail-ab=combat-scenario`;
the melee fixture's world through `retailab_melee.h`). Cases:
`tools/retail_ab/targets_scenario.py`.

### 3.1 One tick, both sides

1. The case's inputs for the tick, in this order: held controls (the
   control map's state and changed masks); a command key (retail's
   TPlayScreen::Command `0x47cf40` as original, the port's
   `TPlayScreen::Command`; so far the combat key, case 1); an attack key
   (ButtonAttack(n) on the player, as Command's cases 0x21..0x2f do).
2. UpdateMove, while the player has control (PlayScreen `+0x5e0`).
3. PulseObjects, MoveObjects -- retail's own loops `0x4580f0` / `0x458390`
   (their map iterator `0x44cf80` answered with the case's characters in
   case order); the port's own `TMapPane::PulseObjects` / `MoveObjects`
   (`tickObjectsSeam` gives the same list).
4. The frame count and game time: retail's fields set as `0x47c37d..`
   does; the port's `TPlayScreen::AdvanceGameFrame`.
5. NextFrameObjects: retail `0x457ef0`, the port's own.
6. The screen frame + 1.

Then the record: the tick, the frame, every character (place, facing,
move angle, state and frame, the three action blocks by meaning with the
attack, impact, damage, to-hit and roll they carry, the attack
bookkeeping, the motion fields -- vel, accum, move distance, commanddone,
framerate, prevstate, stealth values, the combat flash --, health,
fatigue, mana), the tick's seam calls and its random draws. At the end,
each character's AI state and tick state. Everything is compared, tick by
tick; a difference names its tick.

### 3.2 What runs as the game's own code

Everything the tick reaches, on both sides: Pulse (TCharacter, TPlayer),
UpdateAction and the resolvers, AI and the attack choice
(IsValidAttack, the searches, DoAttack), FindCharacters with Hearing,
Sight, CanSeeCharacter, IsEnemy, IsValidTarget and the memory, the
combat key (Command), BeginFighting, EndFighting, SetFighting, Go, Stop,
Leap, SideStep, ResolveAttack,
ResolveHit, Damage and CalculateDamage, KnockBack, Block, the impact /
block / dead resolvers, the recovery and auto-combat, Move, MoveStep,
FindClearPath, CharBlocking and GetWalkHeight(Radius), the line of sight
over the floor (LineOfSight `0x4533d0`, kata M11's; a seam answering
`walls` until the senses merge, the floor being open), SetState,
ResetState, SetObjectMotion / GetMotion, NextFrame, and **the imagery's
state queries** (FindState with its "NN:" frequency variants and their
random(1, 100), FindTransitionState, the frame counts and flags: retail's
T3DImagery `0x446d70` / `0x446f20` / `0x410470` / `0x4104a0`, the port's
TObjectImagery, over the same table).

### 3.3 Seams (answered from the case, the same on both sides)

| Seam | Retail | Port | Why |
|---|---|---|---|
| the tick loops' objects | iterator `0x44cf80` (flags `0xa0`, objsets 4 / 1 / 5) | `TMapPane::tickObjectsSeam` | the case's world, not a loaded map |
| an OF_KILL object taken out | `0x451840`, recorded | `removeObjectSeam`, recorded | no map |
| the walk map | the sector lookup `0x499e10`, ReturnWalkmap `0x499720` | `walkGridSeam` | a flat floor, the case's `ground` (z 17) |
| the characters near a point | iterator `0x44ceb0` / `0x44d080`, recorded | `nearbyCharactersSeam`, recorded | every character of the case |
| SetPos | slot 8 `0x46ed70`, recorded | override, recorded | sectors, walkmaps, redraw |
| object / type stats | GetObjStat / GetStat / SetObjStat, quiet reads, writes recorded | the fixture character's | the stat system's own kata |
| the player's MaxHealth / MaxFatigue / MaxMana | `0x520630` / `0x5206d0` / `0x520770` | overrides | the case's (Locke at level 1: 100 / 78 / 105) |
| the player's WeaponType / WeaponDamage | `0x520810` / `0x520830` | overrides | the case's `weapon` |
| the weapon in hand | a stand-in TWeapon at `+0x2b0` (Type slot `0x1f0`, DamageType `0x210`, GetStat by name `0xd4`) | a real TWeapon of the class.def type, in the hand's inventory slot, RefreshEquip | the case's `wielding` |
| the animator | stand-in, recorded (Pulse, SetNewState, SetComplete, ResetState, ShowNumber) | TFixtureAnimator, recorded | drawing |
| NeedsAnimator | stand-in (`1`) | TFixtureImagery | the animator's lifetime |
| sounds, effects, the text bar, the death screen, RefreshStats, the spell list's pulse, NewObject (blood) | recorded | recorded | outside the game state |
| random / rand | the case's tape, then MSVC's generator from `seed`; every draw recorded | the same | determinism (COMBAT_DOJO.md §3) |

Not run on either side: object Animate (§2.2; the pose for drawing), the
camera, the map position and sector window, the areas, the HUD.

Globals: CombatFace on, single player, NetOwner 1, the player has
control, AutoBeginCombat on, ambient 128, no cheats, the frame count and
game time from the case.

### 3.4 The worlds

The arena's (COMBAT_ARENA.md), built in the fixture rather than loaded:
a flat floor at z 17; Locke and the Araknid as the arena save ArenaA1b
holds them (`tools/savefmt/revsave.py dump` of `game.sav` and sector
`2_24_3`):

- Locke (`11`, class.def radius 16): (25375, 3072, 17), facing 51, state
  246 `6:walk` frame 30, root walk; health 43, fatigue 78, mana 10, level
  1, Strn 18, Cons 12, Agil 14, Rflx 14, Mind 14, Luck 14, skills 0;
  the Short Sword in hand (Type 2, Damage 13, DamageType 0); cloth shirt
  and pants as ACBonus 2 (the fixture's player carries no armour: its
  ArmorValue `0x519850` is ACBonus plus the armour worn); map index
  1955009601; the recovery stamps and play clock of the save.
- The Araknid (`12`, radius 14): (25525, 3072, 17), facing 240, state 33
  `100:combat` frame 1, root combat with no target; health 30, fatigue
  104; map index 268463788.
- The frame count from the save's game time: 294416 × 24 / 100 = 70659.
- The shipped character data and rules (kata D1's dump of the archives,
  as the melee katas take them); each character's whole state table with
  its motion a frame (`tools/retail_ab/cases/motion/{locke,araknid}.full.json`,
  the port's loader through `--dumpi3d`).

## 4. Findings

Run 2026-10-11. Each difference was settled against the asm; the port
was changed where it was wrong.

1. **BeginFighting (`0x4d3b90`) was the 1998 version.** Found at tick 30
   of A1b: when Locke's auto-combat put him into combat, retail's new
   combat root had the angle to the Araknid as its move angle (64); the
   port's carried Locke's move angle (179, his walk's motion). Retail,
   read whole:
   - refused while the root is a run (`combatrun` / `handrun` in combat,
     `bowrun` with the bow, `run`) or `sneak`, or while invoking;
   - only then marks every character seen as fought (no auto-combat on
     them); a dead or invisible target is dropped; the bow needs a ranged
     weapon; no target: FindCharacters(1, −1, facing, 0x20, enemy | hear
     | see); the typed line is committed; already in that root:
     SetFighting;
   - the root's name: the walk root for a walk fighter (charflags
     `0x4000`; slot `0x30c`, "walk", the player's "torch" with a light in
     hand), else the bow's or the combat root's; it needs its state;
   - a step (MOVE, COMBATMOVE, BOWMOVE) stops first; the block takes
     priority only with a transition into it, and interrupts; the doing
     block loses priority when it is the walk root, or the other
     fighting root;
   - **the angle faces the target unless retreating (`+0x254`); the move
     angle is the step's while stepping, else that angle**;
   - a refused block is freed; a script trigger that ran turns the
     incidentals off (charflags 2).
   The port's had no run / sneak / invoke gates, marked the seen before
   them, used IsMoving, faced a retreating character's target, took the
   move angle always, cleared priority only for "walk", leaked a refused
   root, and ignored the trigger's result. Now retail's
   (`TCharacter::BeginFighting`, `GetWalkRoot` added).
2. **A wielded weapon's damage type (`0x51b2f0`).** Found building A4
   (the player's GetDamageType asked the weapon in hand, slot `0x210`, which
   the fixture didn't have). TPlayer's GetDamageType (slot `0x2d4`) returns
   the weapon's DamageType stat (TWeapon `0x529060`, stat registered
   seventh, `0x528cb0`) unless it is −1, and only then the attack's
   (TCharacter `0x4c47d0`). WEAPON.DEF gives every shipped weapon
   DamageType 0, so **an armed player deals misc damage (DT_MISC), not
   cut / puncture / chop**: an enemy's `damagemods` and resistance for
   misc apply. The port computed the type from the weapon type and the
   attack. Now retail's (`TPlayer::GetDamageType`, `TWeapon::DamageType`;
   GetDamageType virtual).
3. **PulseObjects keeps a killed player (`0x45815b`).** An object with
   OF_KILL is taken out of the map unless it is a player, which goes on
   pulsing. The port took out any. Now retail's (found reading the loop
   for this kata; no case reaches it: Locke's corpse never fades in single
   player before the death screen).
4. **The combat key (Command case 1, `0x47d066`) and EndFighting
   (`0x4d3fd0`).** Read for the A8 cases, which press the key. In the
   combat root the key ends the fight and turns the player's auto-combat
   switch (`+0xe8`) off -- so his auto-combat doesn't take him straight
   back in; the switch comes back on only when a look finds no one
   (Pulse `0x4c25d5`) -- and anywhere else, the bow root included, it
   starts a fight (BeginFighting(0, COMBAT)) and turns the switch on. The
   port's key ended or began combat and left the switch alone, and in the
   bow root it ended the fight. EndFighting, read whole: done at once
   outside the combat and bow roots; refused when dead, or in an
   interactive attack (`0x2000000`) or impact (`0x80`) unless charflags
   `0x80000`, or without the walk root's state (slot `0x30c`: the player's
   "torch" with a light in hand); a step stops; the walk block takes
   priority **and is marked a transition** when there is a transition
   into it (`0x4d4164`; UpdateAction's root rule reads that mark); the
   root as doing lets go of its priority; a refused block is freed; the
   move angle follows the facing unless charflags 4; the characters seen
   are marked fought. The port's was 1998's (no health or interactive
   gates, "walk" always, no transition mark, IsMoving, a leak, the move
   angle always). Both now retail's.
5. The rest of the tick matched as the earlier katas left it: the
   Araknid's notice on its 32-frame window (tick 9 of A1b), the turn at 8
   a tick, the walk in, the jump, the bite at tick 54, the hits (9, 7,
   ...), Locke's auto-combat on his own 32-frame window (tick 30), the fatigue
   spent and recovered, the side steps and back-ups, the block.

## 5. Results

`python3 tools/retail_ab/retail_ab.py combat-scenario` (§6 of
COMBAT_DOJO.md lists it with the others).

| Case | Ticks | What happens | Result |
|---|---|---|---|
| `a1b.seed1..4` | 400 | the Araknid finds Locke, walks in, jumps, bites, side steps, backs up; Locke into combat on his own; five or so blows | match |
| `a1b.long` | 900 | as A1b to Locke's death (the death countdown, the death screen), his corpse, the Araknid after | match |
| `a1.seed1` | 200 | the Araknid 450 east hears him and passes him over (beyond its combat range): nothing happens | match |
| `a4.button1..3` | 400 | Locke swinging (one button a case) every 30 ticks at an Araknid of 300 health 60 east, which fights back | match |
| `a4.club` | 400 | the same with a Club (button 2): swings, turn swings, hits and glances both ways, the Araknid's blocks | match |
| `a3.block` | 300 | Locke holding block, the Araknid coming, biting at the block | match |
| `a6.hold0..192` | 160 | Locke in combat holding a direction for 100 ticks (round, in, back, away), the Araknid at 120, then letting go | match |
| `a7.two` | 400 | two Araknids, Locke idle: both find him, who he fights | match |
| `a8.key` | 300 | Locke into combat on his own, out with the combat key at 45 (auto-combat off), struck back into it, the key again at 120 | match |
| `a8.run` | 200 | Locke in his run root presses the combat key (refused), takes the bites | match |

18 cases, about 6,100 ticks, every one equal on both sides (2026-10-11).
Speed: a 400-tick two-character case takes ~3 s in the emulator alone, an
A4 case ~40 s (each swing's attack search runs FindState over Locke's 352
states for each of his ~90 attacks); the port takes ~0.1 s a case. The
whole kata: about a minute with `--jobs 8`.

### 5.1 Mutations

Each broke one line of the port, rebuilt, and ran the kata (cases that
went wrong of 18):

| | Mutation | Caught | First difference |
|---|---|---|---|
| M1 | BeginFighting's new root takes the character's move angle (1998) | 7 | A1b tick 30: Locke's root move angle |
| M2 | the player's damage type ignores the weapon in hand | 1 | A4 club tick 4: the swing's damage |
| M3 | game time at 30 frames a second (the port's old clock) | 18 | the Araknid's fatigue recovery (A1b tick 66), the recovery stamps |
| M4 | MoveObjects in one pass (Move, then SetObjectMotion, per object) | 0 | equivalent for characters (§5.2) |
| M5 | the AI looks for a target every 16 frames, not 32 | 8 | an extra look (FindCharacters' query) in the first ticks |
| M6 | BeginFighting doesn't refuse the run root | 1 | A8 run tick 5: Locke into combat |
| M7 | the combat key leaves auto-combat on | 1 | A8 key tick 45: the switch |
| M8 | EndFighting doesn't mark the walk block a transition | 0 | equivalent here: the transition search in TryCommand / ForceCommand marks it as it plays the transition, and no case leaves the block waiting with the mark unread |

M4 and M8 were kept as retail has them (a reviewer reading the asm finds
the same code); the kata can't tell them from their mutants.

### 5.2 What the kata doesn't check

- The order of the tick's five steps is the fixture's on both sides
  (§3.1, cited to retail); the port's own TPlayScreen::Pulse runs the same
  steps in the same order (§2.4), checked by reading, as the steps are
  wrapped in the play screen's UI work there.
- Splitting MoveObjects' two passes (Move for all, then SetObjectMotion
  for all) into one makes no difference to characters: SetObjectMotion
  changes nothing another character's Move reads (its position stays;
  CharBlocking reads health, attack, flags and the state's flags). The
  kata can't tell them apart; retail's two passes are kept.

## 6. Open

- **Wander (`0x4c9790`)** isn't ported (CHARACTER_PULSE.md §5.1), so a
  player running the AI (charflags `0x100000`, the arena's `--playerai`)
  isn't a case yet.
- The weapon in hand is a stand-in on the retail side (Type, DamageType,
  a stat by name); armour worn is ACBonus. A case that reaches another of
  the weapon's slots fails loudly ("not modelled").
- Object Animate isn't run (§2.2). It moves nothing; if a controller it
  runs draws random numbers (the particle code round `0x402842` does), a
  character with an attached effect would have retail's stream run ahead
  of the kata's. Neither Locke nor the Araknid attaches one.
- The animator is a stand-in on both sides, so its Pulse (the state's
  tags: sounds) isn't in the kata. Retail's tag sound name (`0x40af50`)
  draws random(0, 5) for an "IMP" sound when the weapon kind is a sword
  (`0x40b1da`: "sword" or the two-hander's sample); the port's
  ResolveTagSound doesn't (its REVSYNC-DIVERGENCE: the IMP suffix isn't
  ported). In a real fight that is a draw retail makes and the port
  doesn't, every such sound: the arena's random streams part there. A
  kata of the tag sounds is open.
- PlayScreen `+0x5d4` (Pulse skips everything) and the modal gates are
  case globals, off.
- The player's held light (TPlayer::Pulse's torch flicker, 2-4 draws a
  tick) isn't ported; no case holds a torch.
