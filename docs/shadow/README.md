# Shadow Smoke

A headless, deterministic playthrough of Ahkuilon that tests the real game
end to end in minutes. Locke teleports between activities along walks the
planner has proved legal, and resolves his battles with a duel model that
throws retail's dice. Scripts, scenes, dialog, shops, level-ups, spells,
generators and saves are the real game. He may not cheat, and a ledger
checks every change to him.

- [ARCHITECTURE.md](ARCHITECTURE.md): the design.
- [BURNDOWN.md](BURNDOWN.md): the live checklist.

## Running it

```
tools/shadow/shadow.py                      # the whole plan, seed 1
tools/shadow/shadow.py --until opening --check
```

`shadow.py` runs the game from a run directory of its own (a temp dir, or
`--run-dir`), with a save path holding `~/RevenantRevisited-testsaves/Revenant.ini`.
It then files the run's saves in the waypoint catalogue and, with `--check`,
load-checks each one in a fresh game. The game alone:

```
REVENANT_SAVE_PATH=<dir with a Revenant.ini using retail paths> \
build/Revenant --headless --shadow=tools/shadow/ahkuilon.plan
```

| Flag | Meaning |
|---|---|
| `--shadow=<plan>` | run the plan; implies turbo time and deterministic level loads |
| `--shadow-until=<id>` | stop after a stage |
| `--shadow-out=<dir>` | where the run writes its log (default `<save path>/shadow/<plan>-s<seed>-<time>`) |
| `--shadow-lenient` | log ledger violations instead of stopping |
| `--seed=N` | the game's random stream and Shadow's own (default 1) |

The run writes `shadow.jsonl`, `summary.txt` and `waypoints.json`, and the
milestone saves go to the waypoint catalogue
(`~/RevenantRevisited-testsaves/waypoints/port/shadow-*`). Exit code 0 is a
pass, 1 a failed step, 2 a ledger violation.
