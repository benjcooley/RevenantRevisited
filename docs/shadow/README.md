# Shadow Smoke

A headless, deterministic playthrough of Ahkuilon that tests the real game
end to end in minutes. Locke teleports between activities along walks the
planner has proved legal, and resolves his battles with a duel model that
throws retail's dice. Scripts, scenes, dialog, shops, level-ups, spells,
generators and saves are the real game. He may not cheat, and a ledger
checks every change to him.

- [ARCHITECTURE.md](ARCHITECTURE.md): the design.
- [BURNDOWN.md](BURNDOWN.md): the live checklist.
- [CALIBRATION.md](CALIBRATION.md): the duel model against real fights.
- [SAVEPOINTS.md](SAVEPOINTS.md): a save for every part of the game, and
  playing the plan in segments from them.

## Running it

```
tools/shadow/shadow.py                      # the whole plan, seed 1
tools/shadow/shadow.py --until opening --check
tools/shadow/shadow.py --segment druhgcamp  # one stage, from the save point before it
tools/shadow/shadow.py --segments --check   # every segment at once
tools/shadow/savepoints.py list             # the save point of every story point
```

`shadow.py` runs the game from a run directory of its own (a temp dir, or
`--run-dir`), with a save path holding `~/RevenantRevisited-testsaves/Revenant.ini`.
With `--check`, each save makes the round trip in a fresh game first. Then
the run's saves are filed as save points (SAVEPOINTS.md). The game alone:

```
REVENANT_SAVE_PATH=<dir with a Revenant.ini using retail paths> \
build/Revenant --headless --shadow=tools/shadow/ahkuilon.plan
```

| Flag | Meaning |
|---|---|
| `--shadow=<plan>` | run the plan; implies turbo time and deterministic level loads |
| `--shadow-until=<id>` | stop after a stage |
| `--shadow-stage=<id>` | start at a stage, on the save `--quickstart=<slot>` loads |
| `--shadow-style=<style>` | melee, hands, bow, spells or mixed, over the plan's `style` |
| `--shadow-verify=<file>` | no plan: Locke's state after the `--quickstart` save loads, for the round trip |
| `--shadow-out=<dir>` | where the run writes its log (default `<save path>/shadow/<plan>-s<seed>-<time>`) |
| `--shadow-lenient` | log ledger violations instead of stopping |
| `--seed=N` | the game's random stream and Shadow's own (default 1) |

The duel harness of the calibration (CALIBRATION.md) takes the place of a
plan: after the `--exec` lines have set the game up, it throws trial duels
and writes `duels.jsonl` to `--shadow-out`.

| Flag | Meaning |
|---|---|
| `--shadow-duel=<foe type>[,...]` | the duel harness: Locke against the nearest of each type |
| `--shadow-duel-runs=N` | trial duels, seeds 1..N (default 200) |
| `--shadow-duel-trace=N` | the first N duels' timelines, `trace-<seed>.txt` |

`tools/shadow/calibrate.py` drives it, and the arena beside it.

The run writes `shadow.jsonl`, `summary.txt` and `waypoints.json`, and the
milestone saves go to the waypoint catalogue
(`~/RevenantRevisited-testsaves/waypoints/port/shadow-*`). Exit code 0 is a
pass, 1 a failed step, 2 a ledger violation.
