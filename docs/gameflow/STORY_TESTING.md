# Testing the story headless

How the gameflow track drives scripted scenes and conversations without a
person at the keyboard, and checks them. The opening's acceptance test is
[OPENING_SEQUENCE.md](OPENING_SEQUENCE.md); this page covers the general
method and the story chain after it. Tooling reference:
[../DEBUG_TOOLING.md](../DEBUG_TOOLING.md).

## 1. A run

```
REVENANT_SAVE_PATH=<scratch>/save      # holds a Revenant.ini with retail paths
REVENANT_DATA_PATH=<checkout>/data     # in a worktree whose data/ is LFS pointers
build/Revenant --headless --max-runtime=<s> \
  --quickstart                         # new game: the opening plays
  --quickstart="<slot>"                # or start from <SavePath>/Save/Single/<slot>
  --exec "sleep 48; <command>; ..."    # console commands; sleeps are ticks (24/s)
  --input-script="wait 5000; key_press 1; ...; take_snapshot"
  --filmstrip=<N>,0 --snapprefix=<scratch>/cap/name_
```

- Run each test from its own working directory with its own copy of the
  binary (`build/Revenant.<name>`): runs then go side by side and a rebuild
  doesn't touch a running test. The engine assets (fonts, effects.def) are
  found from the binary, so `build/` is the place for the copy.
- `revenant.log` traces every script line (`[script] <object>: <line>`),
  console output (`[console]`), speech (`[dialog] X says:`), committed
  choices (`[dialog] choice N committed (label '...')`), the text bar
  (`[textbar]`), level entries (`[session] entered level N`) and saves.
  Game text is Windows-1252: use `LC_ALL=C` with `cut`/`grep`.

## 2. Input script traps

- `mouse_click` clicks where the cursor is; its argument is the hold time.
  Position it first: `move 408 175; mouse_click`.
- There is no repeat syntax, and a trailing `wait` with no event after it
  is dropped: end a script with `take_snapshot`.
- The input clock starts with the play screen and leaves out long frames;
  `--exec` sleeps count ticks. Line the two up with generous windows, not
  exact times.

## 3. Conversations

Choices take the keys `1`..`6` while they're up; a digit pressed when no
choice is showing is ignored. So press a digit every 4-5 s over a window
that brackets the menu: a loop back to the same menu just picks again, and
a later menu with fewer choices ignores a digit past its count. Check the
path in the log (`choice N committed`) rather than trusting the timing.

`use <npc>` opens an NPC's DIALOG block with Locke as the user. Name
lookups (`use`, `<npc>.stat`, script `goto <waypoint>`) find the
nearest object of that name on the caller's level within ~2,900 units
(retail's reach, COMMAND_SYSTEM.md §2.3): teleport Locke near the NPC
first (`player.pos x y z level`).

## 4. Checkpoints

Long chains go in stages, each ending with a save through the in-game menu
(ESC, `move 408 175; mouse_click`, `key_press enter`: saves under the
selected slot's name; typing a name needs a click in the field first,
INGAME_MENU.md §6). The next stage starts from that slot. Keep the slots
with the test notes; a save made mid-scene restores mid-scene.

Save is refused while control is off (a cutscene or conversation), as
retail.

## 5. The Keep chain (verified 2026-10-05; the fight 2026-10-06)

| Stage | Start | Commands | Ends with |
|---|---|---|---|
| Opening | `--quickstart` | opening input (OPENING_SEQUENCE.md) | `SardokR: END`; Rahul attacks |
| Rahul, Tendrick's scene | post-opening save | `rahul.stat health = 0` | `ressexit.stat locked=0` (~100 s) |
| Rahul by combat | post-opening save | input: `V`, the Equip tab (605,280), drag the Short Sword (525,456) onto the paper doll (545,175), right-click boots, pants, shirt (575, 368/412/456), `Enter`, `a`/`s`/`d` every 0.7 s ([../gameplay/forensics/PLAYER_INPUT.md](../gameplay/forensics/PLAYER_INPUT.md) §7) | Rahul `combat to dead` in ~25 s, then `ressexit.stat locked=0` |
| Rand | same, after the scene | `player.pos 6280 4338 33 2; use RandK` | `set TENDRICKSTATE=1` |
| Tendrick | Rand save | `player.pos 11973 9645 304 2; use TendrickT` | level 6 (`GowE`), back to the Keep, `TENDRICKSTATE = 2`, `KeepExit` unlocked |
| Keep exit | Tendrick save | `use KeepExit` | level 0, outside the gate |

Retail's own `New Game1` slot (SAVE_INTEROP_TEST.md) starts in the forest on
level 0 and is the base for forest and town tests; the shops were tested
from there (BuySellScreen_SPEC.md).

## 6. Regressions

After a change to scripts, commands, dialog or the session, run the
opening to `SardokR: END` with no `ERROR` lines, and a conversation with
loops (Rand: `jump Start` and two menus). A run that stops logging for
minutes was the App Nap stall headless runs now opt out of
(DEBUG_TOOLING.md).


## 7. The NPC sweep (`tools/storytest/storytest.py`)

Talks to every scripted NPC, one game per NPC, and reports where the port
breaks.

```
export REVENANT_DATA_PATH=<install>      # Modules/Ahkuilon.rvm, imagery.rvi
S="<slot dir>"                            # e.g. retail's New Game1
tools/storytest/storytest.py --slot "$S" locate [--scripts forest.s,town.s]
tools/storytest/storytest.py --slot "$S" run --out <scratch>/pass1 \
    --ini <Revenant.ini with retail paths> [--jobs 4] [--npc NAME ...] \
    [--keys 3,1] [--set MISTSTATE=6] [--set "rahul.stat health = 0"] \
    [--window 600] [--tag k3] [--binary build-x/Revenant]
tools/storytest/storytest.py --slot "$S" report [--reanalyze] [--retail] <scratch>/pass1 ...
```

**locate** reads the scripts and maps from the module archive itself:
every `OBJECT` block with a `DIALOG` trigger, matched to the object it
attaches to (instance name, else type name from `class.def`, as
`TScriptManager::ObjectScript`) on the levels whose areas load that script
(`area.def`). A sector is the slot's `CurMap` copy when it has one, else the
base map's. Per block: choices, `say` lines, the game states its `IF`s test,
and any `jump`/`choice` label missing from its OBJECT block.

**run** gives each NPC a directory under `--out` (a SavePath holding the slot
and the INI; `revenant.log`, `run.json`, `result.json`) and a binary copy
beside the binary (`build/Revenant` unless `--binary`). `--exec`:
`player.pos` 32,32 from the NPC (`--offset`), the `--set` lines (`VAR=N`, or
any console line), `use <npc>`. `--input-script`: from 12 s, every 4 s, the
`--keys` cycle — `1` (the default), `3,1` (3, then 1 for a menu with fewer
choices), `2x5/3` (2 for five cycles, then 3), `1x4/ex4/3` (buy, `e` closes
the shop, then leave). The window is 60 s + 5 s a `say` + 20 s a menu (at
most 600, or `--window`); a run stops 5 s after the block's `trigger 3 ends`
trace line, after 120 s with no line of the block (`--idle`), or at the
window's end. `<out>/names.json` lists every name an object answers to, on
every level.

**report** prints per run whether the block started and ended, its lines,
menus and committed choices, or where it stopped; then each issue once, with
the script line behind it and how many runs saw it:

| Kind | Meaning |
|---|---|
| `console` | an error the interpreter printed (Bad parameters, Unrecognized command, Context not found, extra parameters …) |
| `script error`, `not ported`, `error`, `fatal` | the matching log lines |
| `early end` | the block ended at an inner `END`, not its own: its last lines never ran. A block that ran on past its own `END` after a jump (into the next trigger, as retail's does: SCRIPT_ENGINE.md §4.2; the run is marked "ran on to the object's END") must end at the object's `END` |
| `hang` | the block was still running at the end |
| `loop` | still running, the same choice taken three times or more (the key schedule, not the block) |
| `shop open` | still waiting for the shop's Exit |
| `menu` | a `wait response` with no menu shown |
| `crash` | an exit code other than 0 (or the sweep's own stop) |
| `retail` | console output retail prints for that line too (hidden without `--retail`) |
| `data` | a lookup of a name no object has on any level (hidden without `--retail`) |

The `retail` table is in the tool (`RETAIL_CONSOLE`), each entry with the
handler that answers so: `jump`, the buy/sell name and criteria commands
and `fadecharacterout`/`in` leave a token ("extra parameters ignored"),
`stat x = y` answers bad parameters (COMMAND_SYSTEM.md §4, §6.4, §6.6),
and a trigger's header line reached by a block running on after a jump
(`ALWAYS`, `CUBE …`) is no command ("Unrecognized command.").

### 7.1 The sweep of 2026-10-05

From retail's `New Game1` (forest, level 0, `MISTSTATE` 1), the 34 forest.s
and town.s DIALOG blocks: 32 have an object (`Shari1` and forest.s's
`Pepper1` have none, AUTHOR_QUESTIONS 102). Passes: keys `1` (all 32);
`3,1` for the 18 with menus, `2,1` for 12 of them, `4,1` for Rubold1;
`1x5/ex4/2x2/ex4/3` for the four shops (buy, Exit, sell, Exit, leave);
`MISTSTATE=6` with `3,1` for the 17 that test it; and, teleported in, the 14
DIALOG NPCs of tower.s, ruins.s, cave.s, arakna.s and dungeon.s with `3,1`.
Before the fixes, five blocks ended early at an inner END (Gatekeeper1,
Heather1, Pauline1, Verhoeven1, Kylie1) and so did the Exit choice of
Hruthford's, Gina's and Cronus's shops, every `set` printed extra
parameters, and Jong's training printed "Bad token in trigger block".
After them every forest and town block runs to its END in some pass, except
Jong1: the training loops on `if player.lastattack = …`, a member not
ported (combat track; BURNDOWN T8). A choice that jumps
back to its own menu (Geralt1, Gus1, Rubold1) loops while the same key is
pressed; another key leaves. Kylie1 needs a longer window (`--window 900`):
each of her lines holds the script ~40 s, others 4–10 s (BURNDOWN T9).

The level-46 slave camp blocks (Shegra, Slave1, Slave2, Druhgslave2,
Druhgslave3) stop on their first `say` or `try`: the speaker never gets back
to its root state (likely the same cause as Kylie's, BURNDOWN T9).

### 7.2 The run-on after a jump (2026-10-07)

Retail's block stepping (SCRIPT_ENGINE.md §4.2: after a jump a block runs
on past its trigger's END to the object's END) checked with the sweep from
`New Game1`, keys `1`, the 32 forest and town NPCs, the binary before
(`c0185e1`) and after (`ac2d17f`). 26 runs are the same. The six that
differ:

| NPC | Before | After | Why |
|---|---|---|---|
| Gatekeeper1 | 42 lines, ended | 64, ended | the block runs on through his ALWAYS waypoint walk ("Unrecognized command." for the `ALWAYS` line, as retail) |
| Verhoeven1 | 37, ended | 57, ended | the same, his ALWAYS route |
| Gus1, Pauline1 | 84 / 32, ended | 85 / 33, ended | the DIALOG block is the object's last: one more line, the object's END |
| Heather1 | 45, ended | 48, not ended | the block runs on into her ALWAYS block and waits there (`WAIT 24`); the conversation has just moved Locke to level 0 (`PLAYER.POS … 0`), so level 1 is released and Heather with her script (retail frees the old level's sectors the same way); the tool sees no end and reports a hang |
| Jong1 | 6,098 lines | 6,050 | both loop on `player.lastattack` (not ported, T8) to the window's end; the count is the window |

Also run: Jong1 with `JONGMEETSTATE=1` (SPARYES, then SPARENDYES): 75
lines before, 81 after, the six of his ALWAYS block (`WAIT 24`, `IF
JONGLOOKSTATE = 3`, false); keep.s DalyK and RandK from the Keep saves
(RETAIL_AB.md, target 4).
