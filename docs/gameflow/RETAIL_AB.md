# Gameflow A/B against retail code

Plan (2026-10-07). The gameflow track checks the port against the shipped
game by running retail's own functions in the in-process emulator
(`tools/retail_runtime`, [../RETAIL_AB_TESTING.md](../RETAIL_AB_TESTING.md)
in the main checkout) on inputs we control, running the port's equivalent on
the same inputs, and comparing the results. One original function at a time,
warm checkpoints, milliseconds per case — not a game played through.

Screens and the DOSBox lab ([RETAIL_CAPTURE.md](RETAIL_CAPTURE.md)) stay for
seeing the real game when a question needs it. A/B work goes here.

## 1. How a gameflow A/B works

- **Retail side.** A fixture in the gameflow slot of the emulator
  (`tools/retail_runtime/slots/gameflow/`): it lays out the inputs in guest
  memory — retail's own structures, built by retail's own constructors
  where they exist — calls the original function, and reads the result
  back as JSON. Inputs come from the shipped data (scripts, `class.def`,
  the module) or are listed in the case file.
- **Port side.** A small test binary in this tree (`tools/test_*`), calling
  the port's function on the same case file and writing the same JSON.
- **Compare.** One command runs both over every case and prints the first
  difference per case. Cases cover the shipped data exhaustively where
  that's cheap (every script, every DIALOG block), plus edge cases.
- **Speed and control.** One persistent emulator process per run,
  checkpoint after setup, restore per case. Inputs, RNG and the clock are
  the case's; nothing reads the wall clock.
- **Evidence.** Each run records the retail build hash (unchanged
  baseline unless a named build is used), the port commit and the case
  file hash.

## 2. First targets

In order; each is a closed set of inputs with a pure-ish result.

| # | Retail | Port | Cases | Settles |
|---|---|---|---|---|
| 1 | `TScriptProto::ParseScript` `0x00494e20` | `TScriptProto` parse | every shipped `.s` file (module and `master.s`) | trigger list, conditions, block structure, labels: the parser the whole story stands on |
| 2 | the trigger test `0x004927b0` | `TScript` trigger test | each trigger kind with the inputs it reads (distance, state, user, flags) | which triggers fire when (SCRIPT_ENGINE.md) |
| 3 | speech duration (`Say` `0x004d0610`, DIALOG.md §3.2) | `TCharacter::Say` duration | every `say` line of the module, with and without a voice | how long a line holds the script (T9) |
| 4 | dialog layout: pane pulse `0x005351d0`, entry pulse `0x005348f0` | `TDialogPane::LayOut`, `TDialogEntry` | entry stacks of 1–8 entries, both stacks, restacks over 12 ticks | positions and slides (DIALOG.md §4.4) |
| 5 | the command interpreter `0x0041e8e0` on argument parsing only | `CommandInterpreter` | every command line in the shipped scripts | token and argument parsing, "Bad parameters" cases (COMMAND_SYSTEM.md) |

Later: game-state reads and `IF` evaluation, `TTextBar::Print` line
splitting, save-record serializers against SAVE_GAME.md, the gamma ambient
offset.

## 3. Changing the emulator and the retail code

Both are ours to change when it makes A/B faster or more controllable.

- **Gameflow's fixtures** live in their own slot directory, so they don't
  touch what other tracks run.
- **The emulator core.** A change every track benefits from (an API, a
  slot loader, speed) goes into the shared core as a small, documented
  change, compatible with the VFX probes and their tests
  (`test_runtime.py`, `test_controls.py`, …), which must still pass.
- **The retail assembly.** A named build may patch the retail code: a
  logging hook, a fixed RNG or clock, an entry point exposed for a
  fixture, an init step skipped. The unchanged baseline is never edited;
  every patch has a manifest (addresses, original bytes, purpose) and a
  build records its parent hash. A patched build is an instrument: a
  result that depends on the patch says so, and the unchanged control runs
  beside it.
- **Shared, not private.** A patch or emulator change that works and could
  help another track goes into the shared catalogue
  (`recon/retail_asm/patches/`: one directory per patch with its
  manifest, sources and a line on what it's for and how it was verified),
  listed in RETAIL_AB_TESTING.md, rather than living in one agent's
  workspace.
