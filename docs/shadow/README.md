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
REVENANT_SAVE_PATH=<dir with a Revenant.ini using retail paths> \
build/Revenant --headless --seed=1 --shadow=tools/shadow/ahkuilon.plan
```

| Flag | Meaning |
|---|---|
| `--shadow=<plan>` | run the plan; implies turbo time and deterministic level loads |
| `--shadow-stage=<id>` | start at a stage, from its predecessor's waypoint |
| `--shadow-until=<id>` | stop after a stage |
| `--shadow-out=<dir>` | where the run writes its log (default `shadow/` beside the save path) |
| `--shadow-lenient` | log ledger violations instead of stopping |
| `--shadow-combat=real` | fight with the player bot in the real game instead of the duel model |
| `--seed=N` | the game's random stream and Shadow's own |

The run writes `shadow.jsonl`, `summary.txt` and `coverage.txt`, and the
milestone saves go to the waypoint catalogue
(`~/RevenantRevisited-testsaves/waypoints/port/shadow-*`). Exit code 0 is a
pass, 1 a failed step, 2 a ledger violation.
