# Story triggers: how places change the town

Going to certain places changes Misthaven: Gus moves house, a shop gets new
stock, the townsfolk get new lines. This page explains how that works in
the shipped scripts, lists every trigger that does it, and says what the
Shadow plan must do so that each one fires and each reader runs
([ARCHITECTURE.md](ARCHITECTURE.md) §9.4).

**Sources.** The module's own files: `state.def`, `area.def` and the
`.s` scripts in `Ahkuilon.rvm`, and `master.s` from `resources.rvr` (the
pack copy, which the game reads first; [DATA_LAYOUT.md](../DATA_LAYOUT.md)
§2.4). Object positions and flags come from the base map's sector files
(decoded by `tools/savefmt/revsave.py`). A citation `town.s:2474` is a line
of that script. Text is Windows-1252.

**Generated tables.** `tools/shadow/storymap.py` builds the map from those
files. The tables below are its output, trimmed; the full report has a
section per variable.

```sh
python3 tools/shadow/storymap.py                 # Markdown, every variable
python3 tools/shadow/storymap.py --var MISTSTATE # one variable
python3 tools/shadow/storymap.py --format json   # for the coverage gate
```

**Marks.** [C] read in the scripts, the data or the port's source. [L]
likely: follows from what was read, not run. [U] unconfirmed. Nothing on
this page was played.

## 1. How a place turns on town scripts

### 1.1 Story variables are the only channel

A script line can only reach objects on its own object's level, within
about 2,900 units, in loaded sectors (`FindClosestObject`;
[COMMAND_SYSTEM.md](../gameflow/forensics/COMMAND_SYSTEM.md) §2.4, "Naming
an object"). A script in the tower can't touch anyone in town. [C]

So a place changes the town in one way: a script there writes a story
variable (`SET <var> = <n>`), and a town script reads it later
(`IF <var> <op> <n>`). The variables are declared in `state.def` with their
starting values. `SET` on an undeclared name is ignored, and `IF` on one
compares retail's sentinel (COMMAND_SYSTEM.md §4); Ahkuilon has none. [C]

The other thing that travels is Locke himself: his position and his pack.
In town only Gus2 reads the pack (`player.GETITEMAMOUNT "DIAMOND RING"`,
`town.s:145`, `155`). [C]

Counts: 79 variables (three are declared twice, harmlessly:
PAULINESTATE, MAUREENSTATE, JASONSTATE); 233 `SET` lines; 317 `IF` lines on
a variable. Twenty variables are never written or read (AUSTINSTATE,
CARLSTATE, the five DEMO* states, ELAHNISTATE, FIGHTINGSTATE, HERSHSTATE,
JASON2STATE, JHAGASTATE, JONGSTATE, MARKSTATE, MATTSTATE, PAULSTATE,
REDOSTATE, REXSTATE, RIZSTATE, YHAGOROSTATE). [C]

### 1.2 Writers

A `SET` runs inside a trigger block. What starts the block decides where
Locke has to be ([SCRIPT_ENGINE.md](../gameflow/forensics/SCRIPT_ENGINE.md)
§3):

| Trigger | Fires when | Locke must |
|---|---|---|
| `CUBE player x,y,z x,y,z` | Locke is inside the box | stand in it |
| `CUBE x,y,z x,y,z` (unnamed) | any character other than the owner is inside | stand in it (or a monster wanders in) |
| `PROXIMITY` | Locke is near the owner | none in Ahkuilon |
| `ACTIVATE` | an exit is walked onto (the teleport stones) | step on it |
| `USE` / `USE "<item>"` | Locke uses the object, or uses the item on it | click it, or drop the item on it |
| `DIALOG` | Locke talks to the NPC | talk, and pick the choice that reaches the `SET` |
| `ALWAYS` | every pass, while nothing else runs | be nearby (§1.4) |

A CUBE is a position test made each time its owner's script runs, so
teleporting over one doesn't fire it and teleporting into one does
(ARCHITECTURE.md §5.4). Several story moves rely on the second: a script
puts Locke inside the next cube (Gus2 → GUS3, Gatekeeper1 → CHIEF1 →
AUSTIN, Lv46D5 → Sabu6). [C]

An `ALWAYS` block that writes is a *watcher*: an invisible dummy that polls
a condition, usually a boss's health, and writes the story variable when it
holds. BOSS1DUMMY (`tower.s:19-31`) and CHIEF2 (`forest.s:2275-2332`) set
MISTSTATE this way. [C]

### 1.3 A reader runs only where its script is loaded

`area.def` maps levels, and rectangles on a level, to script files.
`TAreaManager::Pulse` picks the last area that holds the camera
(`src/area.cpp:676`). Entering it loads its script (`TArea::Enter`,
`src/area.cpp:550-552`); leaving clears it (`TArea::Exit`,
`src/area.cpp:582-584`). Objects pick up a newly loaded prototype through
`N_SCRIPTADDED` and drop a cleared one through `N_SCRIPTDELETED`
(`src/script.cpp:1566`, `1648`; `src/object.cpp:2342`). `master.s` is
loaded once at start and is always there (`src/script.cpp:1498`). [C]

| File | Loads on | Area (area.def line) |
|---|---|---|
| `forest.s` | level 0 | The Forest (36); Misthaven, rect x 765–3664, y 24974–27888 (64) |
| `town.s` | level 1 | House Interior (79): every interior in town |
| `keep.s` | levels 2, 6 | The Keep (91, 103) |
| `tower.s` | level 3 | The Ancient Tower (115) |
| `arakna.s` | level 4 | Arakna Island (127) |
| `ruins.s` | level 5 | The Ruins (50) |
| `cave.s` | levels 30, 31, 32, 36, 37, 39 | the caves (139–207) |
| `dungeon.s` | levels 41–49 | the dungeons (211–314) |
| `labyrinth.s` | levels 51–58 | The Labyrinth (318–418) |
| `master.s` | every level | loaded at start |

So "the town" is two places with two scripts [C]:

- **The streets** are the Misthaven rect on level 0. Their script is
  `forest.s`, the same file as the whole forest. The only story reader
  standing there is Lucas1 (2345, 26851). The town's doors (`barin`,
  `libraryin`, `portin`) are in the rect.
- **The interiors** are all on level 1: the tavern, Gus's house, the
  library and the shops. Their script is `town.s`, loaded only while the
  camera is on level 1.

Moving between the forest and the Misthaven rect switches areas, so
`forest.s` is cleared and loaded again: every forest.s script restarts from
idle, and a block that was running is cut off. Story variables are global
and survive. The same happens between Keep levels 2 and 6. [C] in the port;
[L] that retail does the same (the port's area code is retail-synced for
the area choice, `src/area.cpp:661-675`, but the clear-on-exit wasn't
checked against Ghidra here).

### 1.4 …and only near Locke

A script runs from its object's pulse (SCRIPT_ENGINE.md §4), and only the
objects in the map pane's 3×3 sector window around the camera are pulsed
(`TMapPane::PulseObjects`, `src/mappane.cpp:3931`; the window,
`src/mappane.h:360-366`, `src/mappane.cpp:4036`). A sector is 1024 units
square (`src/revdefs.h:66-72`). So an object's ALWAYS and CUBE triggers run
only while it is within about one to two thousand units of Locke on his
level. [C] in the port; [L] for retail (the port's comments cite retail's
`0x004580f0` and say the window is retail's default iterator mode).

DIALOG, USE and ACTIVATE are requested by Locke's own action, next to the
object, so this costs them nothing. It matters for watchers: **an ALWAYS
reader acts only when Locke comes near it after the variable changed.**
Of the 91 cubes in the module's scripts, the 72 whose owner the sector
decoder finds all lie within 1024 units of it, so a cube is in reach
whenever Locke stands in it. `storymap.py` prints the owner's distance for
any cube that isn't. [C]

### 1.5 Readers that don't need a return

- **DIALOG readers** evaluate their `IF`s when Locke talks. Nothing needs to
  have run before: the next conversation picks the new branch. Most town
  readers are of this kind (§2.3). [C]
- **ALWAYS readers** check on every pass while loaded and pulsed. They need
  Locke nearby after the change, not a fresh entry into the area. [C]
- No reader needs Locke to *re-enter* the town. Re-entering restarts the
  scripts (§1.3), which loses nothing. [C]
- Many one-shot blocks end with `TOGGLE PAUSE = 1`, which sets the object's
  `OF_PAUSE` flag; `ContinueScript` then never runs that object's script
  again (`src/object.cpp:2128-2132`). SARDOK4, NAKRA, BOSS1DUMMY, CHIEF1,
  AUSTIN, JASON1 and Jhagadummy do this. [C] The flag is part of the
  object's record in the sector files (`flags`), so it likely survives a
  save and load. [L]

### 1.6 What actually changes the town

The town reads two variables that other places write, plus its own
memory of conversations. From `storymap.py`'s "Town readers" section [C]:

| Variable | Town readers | Writes from outside the town |
|---|---|---|
| MISTSTATE | Lucas1 (forest.s, streets); CAMERON1, CRONUS1, Geralt1, GINA1, Gus2, GUS2DUMMY, Heather1, HRUTHFORD1, Kylie1, Maureen1, Pauline1, Rubold1, Verhoeven1 (town.s) | `tower.s:27` BOSS1DUMMY ALWAYS = 3; `arakna.s:64` Gus4 DIALOG = 4; `forest.s:2314` CHIEF2 ALWAYS = 5; `cave.s:63` SARDOK4 CUBE = 6; `dungeon.s:96` NAKRA CUBE = 7 (and `town.s:2474` Gus1 = 2, in town) |
| JASONSTATE | Geralt1 (`town.s:802`, `868`) | `forest.s:1985` jason1 DIALOG = 3; `forest.s:2003` JASON1 ALWAYS = 1 |
| BAYNESTATE, CAMERONSTATE, GERALTSTATE, GUS2STATE, GUSSTATE, HEATHERSTATE, HEATHER2STATE, KYLIESTATE, LUCASSTATE, MAUREENSTATE, PAULINESTATE, RUBOLDSTATE, VERHOEVENSTATE | each read only by its own NPC | none: each NPC remembers what it already said |

So the places that change the town are the places that move MISTSTATE, plus
the Druhg camp (Jason). Each MISTSTATE value is set in exactly one place
(STORY.md §2). [C]

## 2. Location writes

### 2.1 Every write from a CUBE, PROXIMITY, ACTIVATE or USE block

From `storymap.py`'s "Location writes" section, with what each enables.
"Town" says whether a town reader reads it. [C] unless marked.

| Variable = value | Writer | Where Locke must be | Gate on the write | Read by, enables | Town | Stage |
|---|---|---|---|---|---|---|
| MISTSTATE = 6 | SARDOK4 CUBE, `cave.s:63` | level 37, box x 9750–9900, y 9400–9423, z 0–144; unnamed, so any character but Sardok4 fires it | none on the cube; the SET is under `:HERE`, which both choices reach (`:MADEIT` falls through) | 11 town NPCs change (§2.3); Olihoot1's dagger scene; SardokT's S8b talk; TendrickT | **yes** | S8 |
| MISTSTATE = 7 | NAKRA CUBE, `dungeon.s:96` | level 49, box x 9800–10000, y 9800–9994, **z 240–400** (on the altar) | none | 7 town NPCs change, among them Hruthford's third stock; Olihoot1's death scene; SardokT's betrayal (S13) | **yes** | S12 |
| BAYNESTATE = 1 | BAYNE1 CUBE player, `town.s:2335` | level 1, the tavern, box x 3335–3440, y 20211–20231, z 0–300 | BAYNESTATE = 0 | Bayne's conversation, `town.s:2251-2309` (21 lines, 5 choices); before it Bayne has nothing to say | yes, Bayne only | optional |
| VINCESTATE = 1 | CHIEF1 CUBE, `forest.s:2218` | level 0, box x 13499–13501, y 17727–17729; Gatekeeper1's talk puts Locke there | amulet count = 1, under `:WORTHY` | CHIEF2's watcher takes Locke's weapons (`forest.s:2282-2286`) | no | S6 |
| RINGSTATE = 1 | AUSTIN CUBE, `forest.s:44` | level 0, box x 12190–12194, y 17514–17518, z 110–114; CHIEF1 puts Locke there | RINGSTATE = 0 | AUSTIN itself (one-shot) | no | S6 |
| RAHULSTATE = 1 | SardokR CUBE, `keep.s:660` | level 2, box x 1145–1265, y 475–619, z 0–60 | under `:figureout` | MUDOKON, `keep.s:10` | no | Keep chain |
| SABUSTATE = 98, 99 | Lv48Lev88, Lv48Lev6 USE, `dungeon.s:180`, `232` | level 48, the levers | none | their B cubes, `dungeon.s:189`, `241`: unlock Lv48D1, Lv48D4 and set SABUSTATE back to 0 | no | S10 |
| SABUSTATE = 0 | Lv48Lev88B, Lv48Lev6B CUBE, `dungeon.s:201`, `253` | level 48; the lever's script puts Locke inside | SABUSTATE = 98 / 99 | Sabu6's capture cube needs 0 (`dungeon.s:854`) | no | S10 |
| SABUSTATE = 1 | Sabu6 CUBE, `dungeon.s:861` | level 46, box x 9850–10150, y 9570–9700, z 0–100 | SABUSTATE = 0 | Sabu6's stab cube (`dungeon.s:818`) | no | S11 |
| SABUKILLSTATE = 1 | Sabu6 CUBE, `dungeon.s:847` | level 46, box x 10000–10016, y 9306–9322, z 100–200; `Lv46D5` lands Locke there | SABUSTATE = 1 | the five slaves' guardroom-key talks (20 IF lines, `dungeon.s:1056-1416`) | no | S11 |
| JHAGATELSTATE = 1 | Jhaga2 CUBE, `dungeon.s:80` | level 49, box x 3250–3300, y 4880–5390, z 600–700 | none | LV49BTEL1, `dungeon.s:7`: no way back during the fight | no | S12 |
| ANDREASTATE = 3, 4 | ANDRIA3 CUBE (level 47), `dungeon.s:1478`; ANDRIA2 CUBE (level 49), `dungeon.s:1438` | the arrival boxes of the vision | ANDREASTATE = 3 for the second | each other | no | S12 |
| TOWNTELSTATE = n | TOWNTELn ACTIVATE, `master.s:195`–`547` | the stone (STORY.md §5) | TOWNTELACTIVESTATE = 0, TOWNTELSTATE < n | TownTel0's destination in the Keep (`master.s:2-183`) | no | optional (routes) |
| PEDONE…PEDSIX = 1 | Ped* `USE "<vial>"`, `ruins.s:127-431` | level 5, the pedestals | none | the pedestal's own plain USE. WarpRuin (`ruins.s:433-462`) reads the pedestals' object **state** (`STATE 1`), not these variables | no | S9 |
| WARPxSTATE = 1 | BARRIERx CUBE, `labyrinth.s:66`–`363` | level 57, the barriers; the marker's script puts Locke there | none | Markerx pauses itself (`labyrinth.s:52` and the like) | no | S14 |

**Only two location writes reach the town, and both are on the main
path:** SARDOK4's cube (MISTSTATE 6, S8) and the NAKRA altar (MISTSTATE 7,
S12). The one optional location trigger that changes anyone in town is
BAYNE1's tavern cube, and it only unlocks Bayne's own conversation. [C]

### 2.2 Conversations and watchers that change the town

The other MISTSTATE and JASONSTATE writes come from talks and watchers [C]:

| Variable = value | Writer | Who and where | Choice or condition | Stage |
|---|---|---|---|---|
| MISTSTATE = 2 | Gus1 DIALOG, `town.s:2474` | the tavern, level 1 (4090, 20436) | `:Deal` (third menu, choice 1) | S1 |
| JASONSTATE = 1 | JASON1 ALWAYS, `forest.s:2003` | the Druhg camp, level 0 (2996, 15846) | MISTSTATE > 1 and JASONSTATE = 0, Locke nearby | S2 |
| JASONSTATE = 3 | jason1 DIALOG, `forest.s:1985` | same | none (the talk that gives the Golden Sun Key) | S2 |
| MISTSTATE = 3 | BOSS1DUMMY ALWAYS, `tower.s:27` | level 3 (4893, 4641), by the dragon | `SIDIOUS.STAT HEALTH = 0`, checked after `WAIT 24` | S3 |
| MISTSTATE = 4 | Gus4 DIALOG, `arakna.s:64` | Arakna Island, level 4 (10127, 9975) | `:Found` (choice 2), with the amulet | S5 |
| MISTSTATE = 5 | CHIEF2 ALWAYS, `forest.s:2314` | the Ogrok arena, level 0 (12161, 16985) | `BAEZ.STAT HEALTH < 1` and MISTSTATE < 5, after `WAIT 24` | S6 |

JASONSTATE matters to the town only through Geralt: at MISTSTATE 2–3 he
gives the tower plan while JASONSTATE < 3 (`town.s:802`), and different
lines once Jason has given Locke the key (`town.s:868`). [C]

### 2.3 What each MISTSTATE step changes

From `storymap.py`'s "MISTSTATE by value" section: the objects whose
`IF MISTSTATE` branch differs between neighbouring values. The generated
table gives each branch's line; a few are shown here. [C]

| Step | Set by (stage) | In town | Elsewhere |
|---|---|---|---|
| 1 → 2 | Gus1, S1 | Geralt1 (703 → 800); GUS2DUMMY seats and shows Gus2 (301 → 305) | JASON1 shown at the camp; SardokT |
| 2 → 3 | BOSS1DUMMY, S3 | **nothing** | DRAGONOUT opens; Harowen1, Morganna1; SardokT |
| 3 → 4 | Gus4, S5 | Lucas1, CRONUS1, Geralt1, GINA1, Gus2, GUS2DUMMY (second pass, 315), **HRUTHFORD1 (stock 2)**, Maureen1, Pauline1, Rubold1, Verhoeven1 | Jong1; SardokT |
| 4 → 5 | CHIEF2, S6 | CAMERON1, Verhoeven1 | Gatekeeper1 lets Locke straight in; Olihoot1; CHIEF2; Jong1; SardokT |
| 5 → 6 | SARDOK4, S8 | Lucas1, CAMERON1, CRONUS1, Geralt1, GINA1, Gus2, Heather1, Kylie1, Maureen1, Pauline1, Rubold1 | Olihoot1 (the dagger scene); SardokT (S8b); TendrickT; CHIEF2; Jong1 |
| 6 → 7 | NAKRA, S12 | CRONUS1, Geralt1, GINA1, Gus2, **HRUTHFORD1 (stock 3)**, Kylie1, Rubold1 | Olihoot1 (his death); SardokT (to the Labyrinth, S13); Jong1 |

Shops [C]:

- **Hruthford** (`HRUTHFORD1`, level 1 (9228, 9466); rings, amulets,
  food) has three stock lists. MISTSTATE 1–3: 24 items (`town.s:1552-1617`).
  4–6: the same plus Amulet of Cunning, Emerald Collar, Fire Ruby Ring and
  Sigil Ring (`town.s:1619-1691`). 7: those plus Band of Fury, Emerald
  Bracelet, Gloves of Striking and Necklace of Courage (`town.s:1693` on).
- **Gina** (weapons, `town.s:1793`) and **Cronus** (armour, `town.s:2001`)
  stock every type of their class whose `minstrength` lies in
  [1, VALUE], with VALUE = Locke's level / 3 + 16 (`town.s:1808`, `2017`;
  COMMAND_SYSTEM.md §6.6, `buyselladdcriteria`). Their stock follows
  Locke's level, not the story; MISTSTATE only changes their lines.

## 3. Advancing: the watchers

Every ALWAYS block that reads a story variable, with what it waits for.
"Waits first" is the `WAIT` before its first `IF`: the block sleeps that
long each time it starts, so it looks about once a second. "Once" means the
block resets its own guard or pauses itself. [C] unless marked.

| Watcher | Where | Waits first | Waits for → does | Runs | Locke must be |
|---|---|---|---|---|---|
| GUS2DUMMY, `town.s:297-325` | level 1 (7728, 8640), Gus's house | 24 | MISTSTATE = 1 → hides Gus2. MISTSTATE > 1, GUS2STATE = 0 → moves Gus2 to his chair, shows him, sits him, GUS2STATE = 1. MISTSTATE > 3, GUS2STATE = 1 → again, GUS2STATE = 2 | hide: every pass; the others once | on level 1 near Gus's house |
| JASON1, `forest.s:1991-2019` | level 0 (2996, 15846), the Druhg camp | none | MISTSTATE = 1 → hides him. MISTSTATE > 1, JASONSTATE = 0 → shows him, JASONSTATE = 1. JASONSTATE = 3 → hides and moves him, pauses | hide: every pass; the others once | at the camp |
| BOSS1DUMMY, `tower.s:19-31` | level 3 (4893, 4641) | 24 | Sidious's health = 0 → MISTSTATE = 3, pauses | once | on the dragon's floor until it fires |
| CHIEF2, `forest.s:2275-2332` | level 0 (12161, 16985) | 24 | VINCESTATE = 1 → takes Locke's weapons. Baez's health < 1 → the speech, Soul Cleaver, weapons back, MISTSTATE = 5 | once each (guarded by MISTSTATE < 5) | in the arena |
| Jhagadummy, `dungeon.s:33-54` | level 49 (3457, 4495) | 24 | Jhaga2's health = 0 → JHAGATELSTATE = 0, moves Locke on, pauses | once | by Jhaga's platform |
| YOURMOMS, `labyrinth.s:3-28` | level 58 (9902, 8438) | 24 | Yhagoro's health < 1, ENDSTATE = 0 → the end movies, ENDGAME | once | by Yhagoro |
| KeepExit, `keep.s:1277-1289` | level 2 (12236, 12742) | 24 | TENDRICKSTATE > 1 → unlocks the gate | every pass | by the gate |
| RandK, `keep.s:402-410`; SardokT, `keep.s:1006-1013` | level 2 | 24 / none | TENDRICKSTATE > 1 / SARDOKSTATE = 5 → hide | every pass | nearby |
| Heather1, Rubold1 (`town.s:113-132`, `2649-2669`) | level 1 | 24 / none | HEATHER2STATE = 0 / RUBOLDSTATE > 0 → walk their rounds | every pass | nearby |
| the five slaves, `dungeon.s:1081-1416` | level 46 | none | SABUKILLSTATE 0 / 1 → their idle behaviour | every pass | nearby |

WarpRuin (`ruins.s:433-462`) is a watcher too, but it reads the pedestals'
object states, not a story variable, so `storymap.py` doesn't list it.

Three facts follow, each one a thing a walking player gets for free and a
teleporting Locke doesn't:

1. **Gus2 starts invisible.** His base-map record (`Map/1_7_8.dat`) has
   flags `0x0006c0a8`, which include `OF_INVISIBLE` (bit 7). Only
   GUS2DUMMY shows him, and only when Locke is on level 1 near the house,
   one `WAIT 24` after town.s attaches. After Gus1's deal Locke is in the
   tavern, about 12,300 units away, so Gus2 stays hidden until Locke goes
   to the house. [C]
2. **A watcher fires only if Locke stays.** After Sidious, Baez, Jhaga or
   Yhagoro dies, the story variable changes on the watcher's next pass, up
   to about a second later, and only while the watcher is in Locke's
   window. Until then the gate it opens stays shut: DRAGONOUT still refuses
   to move Locke at MISTSTATE 2 (`tower.s:8`). [C]
3. **An NPC can be hidden at first sight.** jason1 is visible in the base
   map (flags `0x0406c028`), and JASON1's watcher hides him on its first
   pass while MISTSTATE = 1. A walker never gets close enough to talk to
   him before that [L]: the watcher runs once he is within the window, well
   beyond talking reach. A teleport followed at once by a talk might reach
   him first and get the Golden Sun Key before Gus's deal; which runs first
   on that tick is [U]. His DIALOG has no gate (`forest.s:1950-1990`). [C]

## 4. Checklist for the Shadow plan

### 4.1 Required, in story order

Beyond STORY.md's stages, these are the triggers and waits the plan must
hit. "Why" says what breaks without it.

1. **Keep chain: stand by the Keep gate after GowE.** GowE's scene on level
   6 sets TENDRICKSTATE = 2 and puts Locke at (12208, 12512) on level 2
   (`keep.s:1250-1251`); KeepExit unlocks itself on its next pass
   (`keep.s:1281-1284`). Why: the gate stays locked otherwise. [C]
2. **S1 Gus1, choice `:Deal`** (`town.s:2474`). MISTSTATE 2. [C]
3. **S2 at the camp: let JASON1's watcher run, then talk.** Wait one pass
   after arriving and check jason1 is visible before talking. Why: plays
   fair (§3, fact 3); it also sets JASONSTATE = 1, which nothing requires.
   [L]
4. **S3: after Sidious dies, stay on his floor until MISTSTATE = 3**
   (BOSS1DUMMY, at most ~25 ticks). Don't travel or save first (STORY.md
   §6). Why: DRAGONOUT, the only way off the floor, waits for it. [C]
5. **S4: in Gus's house, wait for GUS2DUMMY, then talk to Gus2.** Arrive
   through `portin` (level 1 (7936, 8976), about 400 units from the dummy),
   let at least 25 ticks run, and check Gus2 is visible. Why: Gus2 is
   invisible until the dummy's pass (§3, fact 1). STORY.md S4 assumes he
   is sitting there. [C]
6. **S5 Gus4, choice `:Found`** (`arakna.s:64`). MISTSTATE 4. [C]
7. **S6: after Baez falls, stay in the arena until CHIEF2's speech ends**
   (MISTSTATE 5). [C]
8. **S8: Locke inside SARDOK4's strip** (y 9400–9423, 23 units deep). It is
   an unnamed cube: a monster that walks in starts the scene with itself as
   the user ([U] what the menu does then). MISTSTATE 6. [C]
9. **S10: each level-48 lever's second half.** The lever's USE sets
   SABUSTATE to 98 or 99 and puts Locke, invisible, inside its B cube; the
   B cube unlocks the door and sets SABUSTATE back to 0
   (`dungeon.s:154-256`). Don't travel or save between the two. Why: Sabu6's
   capture cube needs SABUSTATE = 0 (`dungeon.s:854`), so a lever left
   half-done blocks S11. [C]
10. **S12: after Jhaga dies, stay until Jhagadummy moves Locke;** then the
    NAKRA cube, on the altar (z 240–400). MISTSTATE 7. [C]
11. **S13 SardokT at MISTSTATE 7** (`keep.s:940`). [C]
12. **S15: after Yhagoro falls, stay until YOURMOMS plays the end.** [C]

Generic rule behind 1, 3, 4, 5, 7, 10 and 12: **after every Travel, and after
every kill a watcher waits for, let the game run at least 25 ticks with
Locke where he is before the next verb, and check the expected change.**
ARCHITECTURE.md §5.6 already runs a leg's ticks at the destination; a leg
shorter than a second still needs the minimum. [L]

### 4.2 Optional, but they change the town

| Trigger | What changes | Matters to the smoke |
|---|---|---|
| Hruthford at MISTSTATE 4–6 and 7 (`town.s:1552-1693`) | four new rings and amulets each time | **yes**: the shopping list (ARCHITECTURE.md §8.1). Shop after S5 and after S12 |
| Gina and Cronus | stock grows with Locke's level, not the story | **yes**: weapons and armour come within reach as Locke levels |
| Olihoot1 at MISTSTATE < 5 (level 0 (5837, 24805), outside the town rect) | gives the `SKY` talisman (`forest.s:328`) | **yes**: spells (§8.6) |
| GUS2DUMMY's second pass (MISTSTATE > 3) | Gus2 back in his chair for his later lines | no; only if the plan talks to Gus2 again |
| BAYNE1's tavern cube (`town.s:2315-2342`) | Bayne's conversation (BAYNESTATE 1, then 2) | no. It takes control for ~5 s if Locke walks through it (STORY.md S1) |
| Heather1 at MISTSTATE < 6 (`town.s:15-87`) | her one conversation; it ends by putting Locke outside, level 0 (2974, 25636) (`town.s:85`) | no, but the planner must expect the move |
| Geralt1, Maureen1, Pauline1, Verhoeven1, CAMERON1, Kylie1, Rubold1, Lucas1, Gus2 | new lines at the steps of §2.3; each NPC remembers having talked | no; script coverage only |
| SardokT at MISTSTATE 6 (S8b, `keep.s:883-937`) | SARDOKSTATE 3, read only by SardokT | no |
| Teleport stones | TOWNTELSTATE, TownTel0's destination | routes only (ARCHITECTURE.md §5.3) |

### 4.3 For the coverage gate

ARCHITECTURE.md §9.4 asks that every required writer fire and that every
town reader of a changed variable run after the change. `storymap.py
--format json` gives the gate its data: every event has an `id`
(`file:line`), and `coverage` holds the spine's writers in stage order and,
per town variable, the outside writes and the town readers. [C]

Most town readers are DIALOG branches that run only if Locke talks to that
NPC at that stage: 13 NPCs over six steps. Requiring all of them would mean
touring the town after every stage. A lighter gate [L]:

- **fail** on a missed spine writer (the six MISTSTATE writes), on a
  watcher the story needs that didn't run (GUS2DUMMY before S4; BOSS1DUMMY,
  CHIEF2, Jhagadummy, YOURMOMS), and on the main-path readers (Gus2 at S4,
  SardokT at S13);
- **report** the other town readers as coverage.

## 5. Open questions

1. **Retail parity of the pulse window and the area reload** [U]. The port
   pulses only the 3×3 window and clears an area's script on exit. If
   retail pulsed more widely, GUS2DUMMY could show Gus2 while Locke is
   elsewhere on level 1. A check in the emulator: set MISTSTATE 2 in the
   tavern and see whether Gus2's flags change before Locke visits the house.
2. **Talking to a hidden NPC** [U]. Whether `TCharacter::Use` refuses an
   invisible character. If it doesn't, Shadow's `Talk` must check
   visibility itself (§3, fact 3).
3. **`OF_PAUSE` across a save** [U]. One-shot blocks pause their objects
   (§1.5). Not checked: that a load restores the flag. If it didn't, a scene
   could play again after a load.
4. **SARDOK4 fired by a monster** [U]. The unnamed cube takes any character
   as its user. What the scene's menu and `SET` do then (STORY.md S8).
5. **Health = 0 watchers** [U]. BOSS1DUMMY and Jhagadummy test `= 0`, not
   `< 1` (STORY.md §8, question 8). The duel model's damage must leave a
   dead boss at exactly 0 for them to fire.
6. **Cube owners without a position.** `storymap.py` reports "no object
   found" for ANDRIA3. A scan of all 91 CUBE blocks finds no owner either
   for the cave flame cubes (flamer1–4, flameon, Flameon2) and
   SpiritGuardian. ANDRIA3 is in level 47's sector file as plain text
   (`Map/47_9_11.dat`), a record `revsave` leaves undecoded (STORY.md §8,
   question 10). The others don't appear by name in any base-map sector
   file [C], so those cubes likely never fire [L]; an owner placed by type
   name would be missed by this search [U].
