# tools/playtest

The director side of the playtest track: Jev playing retail and the port,
making saves and frame-accurate recordings. Plan: [docs/playtest/PLAN.md](../../docs/playtest/PLAN.md);
the contract the hosts implement: [docs/playtest/PROTOCOL.md](../../docs/playtest/PROTOCOL.md).

| Module | Does |
|---|---|
| `jev.py` | Jev on OpenRouter: typed decisions (`Jev.ask` with `Choice`/`Noul`/`Score`) and the router with screenshots and tools (`Router.ask`); every call to a `Ledger` |
| `rec.py` | the `.rvrec` format: read, write, validate, compare check lines, slot hashes; also a command line (`show`, `validate`, `diff`, `slotsha`) |
| `inputs.py` | a step's events from game controls, with the INI's own bindings (`Bindings.from_ini`), clicks and typing; pointer and held keys carried between steps |
| `host.py` | the protocol's client: `PortHost` (`Revenant --control=stdio`), `RetailHost` (the emulator's play slot) |
| `knowledge.py` | game knowledge from the game's data: char.def (every character's damage modifiers, Locke's attacks and combos with their skill needs), spell.def (talismans, mana, damage); `monster_brief` in words for Jev |
| `combat.py` | the tactician: one Jev request per decision (an intent plus, speculatively, the best option of each intent), distances and enemy phases from char.def's bands and timings, a rule when Jev is unsure, tactics as tick-exact input with measured direction keys |
| `fakehost.py` | a reference host over a toy world, for tests |

Python 3.12, standard library only. The key is `$OPEN_ROUTER_KEY`.

```sh
cd tools/playtest/tests
python3 -m unittest                       # offline
PLAYTEST_LIVE=1 python3 -m unittest test_jev.Live   # one real decision (~$0.00002)
python3 ../rec.py show <bundle>
```
