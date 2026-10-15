# Save points: a save for every part of the game

Shadow Smoke plays the game legitimately and saves at every story point.
Those saves are kept as the **save point repository**: one current save for
each part of the game. A segment of the plan starts from them, and anyone
can load them to test, debug or play a section without playing up to it.

## What a save point is

- **A story point** is a `save` step of the plan (`tools/shadow/ahkuilon.plan`):
  "rahul defeated", "rand questioned", …, "level 4", "jason freed". The
  plan's stages run in the order of STORY.md, so the story points cover the
  game from the Keep to Yhagoro as the plan grows.
- **A save** is the game's own save (`TGameSession::RequestSave`), made by a
  passing Shadow run: Locke got there legitimately, under the ledger. It
  is kept as the game wrote it. That is a retail slot folder on main today
  (`game.sav`, `ss.bmp`, `CurMap/`, about 3.7 MB). With the single-file
  format (`<slot>.rvsave`, docs/gameflow/SAVE_FORMAT.md, on
  `feature/savediff`), it is one file of 100–270 KB. On a test merge, all
  eight segments passed on it, and every `.rvsave` came back unchanged in
  its round trip.
- **Its record** is a catalogue entry (`tools/playtest/waypoints.py`, at
  `~/RevenantRevisited-testsaves/waypoints/port/shadow-<point>--<time>/`)
  plus a sidecar, `shadow.json`, written when the run files it:

  | Field | Meaning |
  |---|---|
  | `point` | the story point |
  | `plan`, `plan_sha` | the plan file and its contents' hash when the save was made |
  | `stage` | the plan stage that saved it |
  | `seed`, `style` | the run's seed and play style |
  | `started_from` | the save point the run began at, or `newgame` |
  | `round_trip` | `true`: a fresh game loaded it and Locke came back the same (`--check`, ARCHITECTURE.md §10); `false`: he didn't; `null`: unchecked |
  | `differences` | what didn't come back, when it failed |

- **The save point** of a story point is its newest save whose round trip
  didn't fail, and that isn't retired in the catalogue.

## Using it

```sh
tools/shadow/savepoints.py list                     # every story point and its save point
tools/shadow/savepoints.py export "jason freed" --to "<game>/Save/Single"
tools/shadow/savepoints.py export all --to "<game>/Save/Single"
```

`export` copies the slot under its own name (`Shadow jason freed` or
`Shadow jason freed.rvsave`), so the game's Load screen lists it. A retail
folder loads in the port and in retail (save interop). A save file needs a
port that reads them; `TSaveGame::ExportRetail` writes one out as a retail
folder.

## Playing in segments

A segment is the part of the plan between two save points. `shadow.py`
plays one or all of them:

```sh
tools/shadow/shadow.py --segment druhgcamp --check  # S2 alone, from "level 4"
tools/shadow/shadow.py --segments --jobs 4 --check  # every segment at once
```

- A segment that ends with stage S starts from the newest save point of
  the stages before S, nearest first, at the stage after that save's. With
  none, it starts from a new game. Missing save points make a segment
  longer, never wrong.
- The run loads the save (`--quickstart`) and starts the plan at that stage
  (`--shadow-stage`). A death in it reloads that same save (§6.6).
- Its own saves are filed with their sidecars, so a passing segment
  refreshes the save point it ends at. `--segments` therefore refreshes the
  whole repository at once.
- Segments run in parallel, each in its own run folder with its own copy of
  the binary. The whole game takes as long as its longest segment: the
  forest grind, a few minutes. The plan up to "jason freed" takes about
  four minutes run end to end.

**When a segment's start is stale.** A save point made by an older plan or
build is still a valid save: Locke got there legitimately. `list` marks
those whose plan hash differs. A segment started from one tests its stage
from that state. Refresh the chain with `--segments` once the plan has
changed upstream of it.

## Keeping it

The repository lives in `~/RevenantRevisited-testsaves/waypoints/` beside
the playtest track's waypoints, and isn't in the main repo: saves are the
game's data. A save is about 3.7 MB, so a full refresh adds about 30 MB.
`list` shows how many are filed per story point. Trim them with:

```sh
tools/shadow/savepoints.py prune --keep 3        # lists what would go
tools/shadow/savepoints.py prune --keep 3 --yes  # deletes it
```

It keeps each story point's newest good saves and drops older ones and
any whose round trip failed. A story point always keeps its save point.
