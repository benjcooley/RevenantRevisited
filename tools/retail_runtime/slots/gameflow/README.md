# Gameflow slot: retail A/B fixtures

The gameflow track's retail sides for its A/B tests against the port
(plan and results: `docs/gameflow/RETAIL_AB.md` on `feature/gameflow`).
Each fixture runs original retail functions on the unchanged baseline
(`28bec273…`), one persistent process per run: setup once, checkpoint,
restore per case. The port side and the compare command live on the
gameflow branch (`tools/retail_ab/retail_ab.py <target>`), which starts
these with `--serve` and feeds them JSONL.

| Fixture | Original code | Recorded boundaries |
|---|---|---|
| `script_parse.py` | `TScriptProto::ParseScript` `0x00494e20` (tokenizer, ParseCriteria, DATA), `TScript::Jump` `0x00493fa0` | text bar `0x0054d170`, console `0x0041ee50`, error log `0x004820b0` |
| `say_duration.py` | `TCharacter::Say` `0x004d0610`, `DialogLine` `0x00533dd0` | sound player FindSound/Mount/Play/Length, `DialogPane.AddSpeech` `0x00535b90`; speaker vtable stubs |
| `dialog_layout.py` | `TDialogPane` pulse `0x005351d0`, entry pulse `0x005348f0`, `Dismiss` `0x00534a40`, entry dtor | screen invalidate `0x004aacb0` |
| `script_step.py` | `TScript::Continue` `0x004933d0` (line loop, ELSE/IF/loop handling, block end), the command interpreter `0x0041e8e0` and its table, `begin`/`end`/`else`/`jump` (`0x00420c70` -> `0x00471290` -> `Jump` `0x00493fa0`), `Start` `0x00492440`, the parser (via `script_parse.py`) | every other command handler (logs, returns 0; `if`/`while` return the run's condition; `wait response` sets a wait), the context resolver `0x0041e690`, text bar/console/error log, owner vtable `+0x144/+0x148/+0x14c` |
| `trigger_test.py` | the trigger test `0x004927b0`, the CUBE search `0x00452480`'s own containment test, the `TScript` ctor | the map iterator `0x0044cf80`/`0x0044d080` (the fixture world's objects on the level asked), the object lookup by id `0x00452690` (the guard) |

Measured (2026-10-07, this Mac): script parse 0.5 cases/s (a whole script
file per case; `Jump` re-tokenizing every label dominates), say 71 cases/s
(~0.7 ms per `Say`), dialog layout 51 cases/s (~0.06 ms per entry-tick),
trigger test ~1,000 cases/s. Setup (load, CRT init, checkpoint) ~10 ms.
Block stepping is the heavy one: 1,650 runs (every trigger block and label
of 18 files, twice) cost ~1,000 s of emulation in one process, ~85% of it
forest.s's Jong1 (a 1,170-line prototype whose jump loops each re-read it
from the top); `script_step.py` caps a run at 256 lines and 8 jumps, and
takes a `share` [k, n] of a file's runs, so the driver (`retail_ab.py
--jobs N`) splits a file over N processes. Those figures are with the
runtime's native dirty-page tracking (README.md, "Checkpoint restore"):
the Python hook it replaced cost the parse fixture 3x (forest.s 12.5 s ->
4.0 s) and stepping 1.4x.

## Reusable pieces (`fixture.py`)

- `start(exe)`: Runtime on the verified baseline with the original CRT
  heap/TLS init (`0x0058ed0d` up to `0x0058ed8e`), so `0x00482fb0` (malloc)
  and friends work.
- `Boundaries(vm).add(address, name, pop, handler)`: answer an original
  function's entry from Python under its own ABI (`pop` = the argument
  bytes the callee removes, 0 for cdecl); every call is recorded.
  `add_stub(name, pop, handler)` makes a fresh address for a synthetic
  vtable slot.
- `serve(sha, schema, setup_ms, run)`: the JSONL loop (`{"id","case"}` in,
  `{"id","ok","result"|"error"}` out; a failing case doesn't stop the
  process).

Run one directly:

```sh
PY=/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python
"$PY" tools/retail_runtime/slots/gameflow/script_parse.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe --case path/to/file.s
```
