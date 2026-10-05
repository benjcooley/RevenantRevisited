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

## 5. The Keep chain (verified 2026-10-05)

| Stage | Start | Commands | Ends with |
|---|---|---|---|
| Opening | `--quickstart` | opening input (OPENING_SEQUENCE.md) | `SardokR: END`; Rahul attacks |
| Rahul, Tendrick's scene | post-opening save | `rahul.stat health = 0` | `ressexit.stat locked=0` (~100 s) |
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
