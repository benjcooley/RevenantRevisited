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

## 4. What runs (2026-10-07)

### How to run

```sh
python3 tools/retail_ab/retail_ab.py script-parse          # first difference per case
python3 tools/retail_ab/retail_ab.py script-parse --all    # every difference
python3 tools/retail_ab/retail_ab.py script-parse --case town
```

One command per target. It writes the cases under `build/retail_ab/<target>/`,
runs the retail fixture (`tools/retail_runtime/slots/gameflow/<target>.py`
in the main checkout, one persistent emulator process, checkpoint after
setup, restore per case) and the port (`build/Revenant --retail-ab=<target>
--ab-cases=… --ab-out=…`, one process, no window, from `sokol_main`;
`src/retailab.cpp`), compares the two dumps and prints the first difference
per case, then the differences by kind. `report.json` holds every
difference, the retail build SHA-256, the port commit (`+dirty` when the
tree has changes), the port binary's hash, the case set's hash and the
timing. Exit status 0 only when everything matches.

Paths: `RETAIL_RUNTIME` (default: the main checkout's `tools/retail_runtime`),
`RETAIL_PY` (the retail-asm venv), `REVENANT_DATA_PATH` (default
`~/RevenantRetailLab/retail-cd/REVENANT`), `--port` (default
`build/Revenant`).

### Target 1: script parse — done

**Retail side** (`script_parse.py`): the original tokenizer, `ParseScript`
`0x00494e20`, `ParseCriteria` `0x00494c50`, the DATA parser `0x00495750` /
`0x00495830`, the trigger adds `0x00497f60`, the `TScriptProto` and
`TScript` constructors and `TScript::Jump` `0x00493fa0` all run as original
code over each file, driven as `TScriptManager::ParseScripts` `0x00496860`
drives them (its stream and token laid out as its inline code builds them,
`0x00496892`–`0x004968fe`; one `ParseScript` per OBJECT block until the
end). The three places a script error goes are recorded boundaries: the
text bar `0x0054d170`, the editor console `0x0041ee50` and the error log
`0x004820b0`. One observation hook (`0x00494e6c`) reads where `ParseScript`
starts the prototype's text. Not run: the manager's bookkeeping (a later
block with the same name replacing an earlier one, `N_SCRIPTADDED`).

**Dump** (schema `gameflow.scriptparse.v1`), per prototype: name, parent,
the text's extent in the file, whether `ParseScript` returned −1 (the block
is discarded), and every error it reported; per trigger: type, block start
(as a file line:column), name, cube, distance, `+0x38`, region; the DATA
variables; and every `:label` in the text resolved by `Jump`: found, where
the script resumes (file line:column), at what block depth, and its errors.
Retail's prototype holds no label table (Jump scans the text), so the labels
are compared through Jump.

Layout settled by the run: trigger record 0x50 bytes — type `+0x00`, pos
`+0x04`, name[20] `+0x08`, cube `+0x1c`, dist `+0x34`, priority `+0x38`
(read by the trigger test, never set by the parser), region[20] `+0x3c`;
variable record 0x2c bytes — type `+0x00` (0 NUMBER, 1 TEXT), value pointer
`+0x04` (an int, or a 30-byte text), name `+0x08`, value size `+0x28`.

**Cases**: 14 shipped files (the module's 9 `.s`, `resources.rvr`'s
`master.s` and `multiplayer.s`, the loose `Resources/master.s`, the demo
module's `demo.s` and `master.s`) and 15 edge files in
`tools/retail_ab/cases/script_parse/` (every trigger form, CUBE with NULL /
a region / reversed corners, PROXIMITY forms, bad parameters, unknown
trigger, header forms, DATA forms, labels, a missing END, LF-only, a 0xFF
byte, Windows-1252 names).

**Results** (case set `9bb50965…`, retail `28bec273…`):

Shipped: 264 prototypes, 409 triggers, 10 DATA variables and 403 labels.
Names, trigger types, block starts, trigger names, cubes, distances,
variables, parse errors (none) and which labels resolve are identical.
Three differences remain, all systematic:

| Kind | Count | Retail | Port | Cause |
|---|---|---|---|---|
| text extent | 264 | the whole block, `OBJECT` through `END` and its line break (retail's `TScriptManager::Save` `0x00496690` writes this text as is) | the body between the object's `BEGIN` line and `END` (the 1998 design; `WriteScript` re-adds the header) | the port's `ParseScript` keeps the 1998 extent |
| label depth | 403 | the port's + 1 (2–5): the object's own `BEGIN` counts | 1–4 | follows from the extent: `Jump` counts BEGIN/END from the start of the text |
| label resume point | 400 | the start of the line after the label | right after the label's name | retail's tokenizer holds the character after a token, so the stream has consumed the label line's break; the port's rewinds. The same line runs next either way |

The depth matters: `Continue` ends a block when the depth drops below 1
after a line (`0x004933d0`). After a `Jump` to a label directly in a trigger
block (retail depth 2), the trigger's `END` leaves retail at depth 1, so —
if the rest of `Continue` agrees — retail runs on into the next trigger's
header line and block until the object's `END`. 48 of the 61 such labels
sit in their object's last trigger (nothing follows); 13 don't: forest.s
Jong1 (`:COMBAT`, `:SPARYES`, `:SPARENDYES`, `:TRAINEND`), keep.s DalyK
(`:Start`, `:A`, `:B`, `:C`, `:Marker`, followed by an ALWAYS patrol) and
SteffanK (`:Start`, `:misthaven`, `:cult`, `:letyoulive`). Not changed in
the port: it needs the `Continue` A/B (target 2's trigger test and the run
loop) to confirm, then a decision (§8 of DIALOG.md: retail hazards are
decided, not copied).

Edge cases (malformed input; the port's behaviour stands unless noted):

- **Fixed in the port**: `CUBE NULL …` leaves the name empty (the port
  named the trigger "NULL"); `CUBE <name> <region>` keeps the region name
  with an empty cube (the port reported "Invalid cube trigger" and "Bad
  parameter" and built the cube from the previous trigger's corners);
  "RETURN expected in <file>:<token>" after the object's `END`.
- Unknown trigger, or an object with no `BEGIN`: retail stops the
  prototype ("LOAD STOPPED!! Unknown trigger …", returns −1, ParseScripts
  discards it) and parses on from where it stopped, so the rest of the
  block becomes further broken prototypes; the port reports the trigger
  and goes on (a deliberate deviation, `script.cpp`).
- A 0xFF byte (ÿ) anywhere: retail's string stream returns it as −1, the
  tokenizer's end of stream, and the parse goes wrong from there; the port
  reads it as a character. No shipped script has one.
- A bad parameter that ends its line is reported one line later by retail
  (its tokenizer has read the line break).
- DATA lines with a missing or mistyped value (`NUMBER X`, `TEXT X`,
  `NUMBER X "seven"`): retail never finishes the block (instruction budget
  exhausted; the hang SCRIPT_ENGINE.md §7 describes); the port reads on.

**Speed**: retail 29 cases in 56 s (0.52 cases/s; 14 shipped files 26 s).
Label resolution is ~90% of it — every `Jump` re-tokenizes the prototype
from the top (forest.s: 68 labels, 52M of 57M instructions) — and the
runtime's per-write dirty-page hook costs ~70% of the wall time (forest.s
10.8 s with it, 3.2 s without). Setup (load + CRT init + checkpoint) 10 ms.
The port: 0.7 s for all 29.

### Target 2: speech duration — done

```sh
python3 tools/retail_ab/retail_ab.py say-duration
```

**Retail side** (`say_duration.py`): the original `TCharacter::Say`
`0x004d0610` — `DialogLine` `0x00533dd0`, the action block constructor
`0x004da9f0`, strdup and the x87 duration arithmetic run as original code —
on a fixture speaker (zeroed object, synthetic vtable: Health `0x1c0`
answers 100, TryCommand `0x218` records the action block). Single player,
PlaySpeech and ShowDialog on. Recorded boundaries: the sound player's
FindSound `0x0049c430` (the case decides whether the voice is found), Mount
`0x0049b650`, Play `0x0049b990`, the sample length `0x0049c640` (the case's
milliseconds) and `DialogPane.AddSpeech` `0x00535b90`. Each case runs twice:
voice found (with its length) and not found. Compared: the action's `wait`,
the ticks AddSpeech gets, and `strlen` of the `DialogLine` output.

**Port side**: `DialogLine` and `TCharacter::SpeechTicks` (the duration
rule, split out of `Say` for this) on the same inputs; the voice's length
is `audio::DecodedLengthMs` of the shipped `.mp3` — what
`TSoundPlayer::SampleLengthMs` measures. The port runs first and retail
gets the port's lengths: Miles (`AIL_sample_ms_position`) doesn't run in
the emulator, so whether the port's decoded length equals Miles' is **not**
tested here.

Retail's rule, confirmed: `frames ≥ 0` wins; a voice found with length
L > 0 gives `12 − ftol(L × 0.001f × −24.0f)` = `12 + ⌊24L/1000⌋`; else
`2 × strlen(line) + 36`. The voice counts whenever FindSound found it, even
if it fails to load or play.

**Cases**: every `say` of the module's scripts and `master.s` /
`multiplayer.s` (1,753 lines; every one is `say TAG` with a voiced tag, no
frames, no quoted text, no `choice`), as 1,545 distinct inputs; a sweep of
the voice length over 0–30,000 ms (every millisecond); and 12 edge cases
(given frames with and without a voice, odd/even/empty/one-character
lines, Windows-1252 bytes, brackets, a `[me]` after a high byte, 255- and
300-character lines, a missing voice).

**Results** (case set `fb7744b1…` without edges): all 1,545 shipped inputs
and all 30,001 sweep points match, voiced and unvoiced (voice lengths 339–
14,811 ms). Edge cases: 8 of 12 match; the other 4:

- `[[me]] and [chr]`, `é[me]`: retail's `DialogLine` copies the text as is
  (the inverted lead-byte test, DIALOG.md §3.7, and a `[` after a high byte
  is copied as that byte's trail byte); the port substitutes and unescapes
  (a deliberate deviation, `dialog.cpp`), so it paces such a line 12 ticks
  shorter. No shipped line has a `[`: the shipped run is the evidence.
- 255 and 300 characters: retail faults. `DialogLine` copies into a
  256-byte stack buffer with no bound before its `strncpy(n − 1)`, so a line
  of 255 bytes or more overwrites its return address. No shipped line is
  that long. The port truncates; a retail hazard, not to copy.

**Speed**: 71 cases/s (1,558 cases in 22 s, 31,557 `Say` calls with the
sweep; ~0.7 ms per call); the port 1 s.

### Target 3: dialog layout — done

```sh
python3 tools/retail_ab/retail_ab.py dialog-layout
```

**Retail side** (`dialog_layout.py`): the original pane pulse `0x005351d0`
once per tick — every entry's pulse `0x005348f0`, the layout, the deletion
of faded entries (original destructor `0x005343e0`, free, and the entry
array's Remove `0x0041cb80`), the base `TButtonPane` pulse `0x00435d70` —
and the original `Dismiss` `0x00534a40` for a case's dismiss events. The
pane is a zeroed fixture with its entry array from the original
TPointerArray constructor and Add; the geometry globals are the case's
(map view `0x006668e0/e4/e8`, side tabs `0x0065be5c`, status bar
`0x0065a8c8/d0`). Entries are 0x160-byte records from the original
allocator, set as `AddSpeech`'s constructor call sets them (base and offset
−10000, width 400, fade 0 toward 12, one text) with the height the port
measured for the same texts (the retail constructor wraps with GDI fonts).
Recorded boundary: the destructor's screen invalidate `0x004aacb0`.

**Port side**: a fresh `TDialogPane` (`UseFont(nullptr, 20)`: the Dialog
line height, texts breaking only at their `\n`), entries through the new
`TDialogPane::AddEntry` (what `AddSpeech` and `ShowResponses` now share),
`TDialogPane::Pulse` per tick, and each entry's `Placement()`.

**Cases**: 54 timelines on the Classic geometry (640×480 map view, side
tabs 52, status bar 0x70): for the NPC stack and the player stack, 1–8
entries added together (the oldest leaves first: every departure restacks
the rest), staggered by 6 ticks (arrivals push the player stack up while
it slides), and newest-first; same-tick expiries; dismissals in the middle
of a slide; both stacks interleaved; no-timeout entries. Heights 44–80.

**Results**: every entry's lifetime, base, offset, target, 16.16 position,
step, dismissed flag and fade are identical at every tick in all 54
timelines (18,139 entry-ticks). The time-based presentation interpolates
between these tick states, so it agrees with retail at tick boundaries.
One difference (30 instants in 16 timelines): which entries still exist
the tick after a group of neighbours fades out together. Retail's deletion
loop (`0x005353fb`) advances its index after Remove compacts the array, so
it deletes every other one of them per tick; the port deletes them all at
once. They are faded out (fade 0) and hold no slot, so nothing on screen
differs. Not ported (no visible effect).

**Speed**: 51 cases/s (54 cases, 1.1 s; ~0.06 ms per entry-tick); port
0.6 s.

### What the emulator lacks for the rest (the trigger test, the command interpreter)

- **The trigger test** (`0x004927b0`) reads the player (`0x00667fcc`:
  position, name), the owner's level, the object list for CUBE
  (`0x00452480`, every character on the owner's level) and object
  existence (`0x00452690`). It needs fixture objects with the right
  instance layout (name `+0x38`, position `+0x10..+0x18`, level `+0x0e`)
  and either the map's object enumeration as a boundary or a small object
  table; no new emulator capability, but a fixture world of a few objects.
- **`Continue` and the run loop** (needed to settle the label-depth
  question of target 1): the command interpreter `0x0041e8e0` runs every
  line, so a fixture must either stub every command a block uses (as
  recorded boundaries) or run the real ones, which touch most of the game.
  A per-command boundary table is the practical route.
- **The command interpreter's argument parsing** (`0x0041e8e0`): the
  command table and each command's parse run before its effect, but the
  effects are in the same functions; a named build that returns after the
  parse (an assembly patch per command family) would isolate it.
- **Speed**: the runtime tracks dirty pages with a Python callback on
  every guest write; it is ~70% of the script-parse wall time (forest.s:
  10.8 s with it, 3.2 s without). Unicorn 2.1's native copy-on-write
  memory snapshot (`UC_CTL_CONTEXT_MEMORY`) would remove it, but it
  conflicts with the runtime's VirtualAlloc mapping restore
  (`memory.py restore_mappings`); a core change for later, with every
  track's tests.
