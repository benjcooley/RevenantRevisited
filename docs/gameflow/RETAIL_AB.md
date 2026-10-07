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
| 6 | block stepping: `TScript::Continue` `0x004933d0` with the interpreter, its commands as boundaries | `TScript::Continue` | every trigger block and every label of the shipped scripts, plus edge scripts | which lines run after a jump, where a block ends (the label depth of target 1) |

Status (2026-10-07): 1, 3, 4, 6 and 2 run (§4, targets 1–5 in the order they were done); 5 not started.

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
python3 tools/retail_ab/retail_ab.py script-step           # Continue over every block and label
python3 tools/retail_ab/retail_ab.py trigger-test          # the trigger test in a fixture world
```

Targets: `script-parse`, `say-duration`, `dialog-layout`, `script-step`,
`trigger-test`. Each run also leaves the retail dump beside the port's
(`<target>.retail.jsonl`, `<target>.port.jsonl`) for reading a case in full.

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
SteffanK (`:Start`, `:misthaven`, `:cult`, `:letyoulive`). **Settled by
the block-stepping A/B (target 4 below): retail does run on, and not only
after those 13** -- every `jump` leaves retail one level deeper than the
port, wherever the label sits, so any jump in a trigger that isn't its
object's last runs on into the next trigger. Not changed in the port; a
decision (§8 of DIALOG.md: retail hazards are decided, not copied).

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
The port: 0.7 s for all 29. Since the native dirty-page tracking (Speed,
below): 19.6 s for the 29 in one process, forest.s 4.0 s.

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

**Results** (case set `81b137c2…`, 1,558 cases; port `00052ca`): all 1,545 shipped inputs
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

**Results** (case set `24ca8932…`; port `00052ca`): every entry's lifetime, base, offset, target, 16.16 position,
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

### Target 4: block stepping (`Continue`) — done

```sh
python3 tools/retail_ab/retail_ab.py script-step [--jobs N]
```

**Retail side** (`script_step.py`): the original `TScript::Continue`
`0x004933d0` -- its line loop, the IF/ELSE/loop/block-end handling,
SkipBlock, SkipLine and the tokenizer -- with the original command
interpreter `0x0041e8e0` and its table. `begin`, `end`, `else` and `jump`
run as original code (`jump` `0x00420c70` → `0x00471290` → `TScript::Jump`
`0x00493fa0`). Every other command's handler is a recorded boundary that
logs the command and returns 0 (success, no wait); `if` and `while` return
the run's condition (every one true, or every one false, encoded as
`0x0041fb70` / `0x0041fbc0` encode theirs); `wait response` (and
`responsenohide`, `respnohide`, `respctrlon`) also sets a wait, so the run
stops where the script would wait for the player -- what a choice jumps to
is a label run. The table's class contexts are set to −1 in the fixture's
memory so every handler is reached. The context resolver `0x0041e690`
answers every name with the owner, a character-class object whose vtable
`+0x150` is the original `ParseCommand` `0x004713b0` (TCharacter's: it
answers "unrecognized"); `+0x144/+0x148/+0x14c` are recorded.

Each file is parsed with the original parser (`script_parse.py`); then, per
prototype and condition policy, each trigger block runs from its start
(original `Start` `0x00492440`, the trigger's type, depth 0) and each label
from the original `Jump` on a fresh script, followed by one
`Continue(1)`. The script's top prototype is an empty one, so its trigger
scan fires nothing and only that block runs. Observation hooks: the
interpreter call `0x00493942` (stream position after the line's first
token, depth) and its return `0x00493947` (bits). A run stops after 256
lines or 8 jumps (the hook sets the pause bit `Break` sets): a loop through
a label costs retail a re-read of the prototype from its top per pass.

**Port side**: `TScript::Continue` on the same blocks: the command table's
handlers swapped for the run (begin/end/else kept; if, while, jump, wait as
above; class contexts −1), `Start(proto, −1)` + `Jump` for a label,
`StartTrigger` for a trigger. The line sequence comes from a new observer
on the line loop (`TScript::IStepObserver`: before and after each
interpreter call; nothing observes it in the game). The port has no owner
object, so its resolver fails prefixed commands; the compare keeps only the
result bits that steer `Continue` (WAIT, DELETED, CONDTRUE/FALSE, ELSE,
SKIPBLOCK, LOOP, BEGIN, END, JUMP, WAITSAY).

**Compared**, per run: the file lines handed to the interpreter, the depth
before each, the steering bits, how the run ends (block end, wait, cap),
the end ip and depth, the script errors (text bar).

**Cases**: the 14 shipped files (409 triggers, 403 labels) and 4 edge files
in `tools/retail_ab/cases/script_step/` (triggers after a label's trigger,
IF/ELSE/ELSE IF, WHILE, jump loops, an unknown label, a quoted context, a
number line, nesting 9 deep, an unclosed IF), each under both condition
policies: 1,680 runs (818 trigger and 806 label runs shipped).

**Results** (case set `613feb21…`, retail `28bec273…`, port `5e76b0f`):

| Runs | Same lines | + the object's `END` only | Run on into the next trigger | Other |
|---|---|---|---|---|
| trigger blocks, 850 | 841 | 5 | 2 | 2 (edge: `ELSE IF`) |
| labels, 830 | 546 | 136 | 142 | 6 (edge: `ELSE IF`; depth 10) |

Every run ends the same way on both sides (block end, response wait, or
cap). The shipped files differ only by the depth after a jump (below) and
target 1's resume point (where a run capped right after a jump stops);
master.s, multiplayer.s and labyrinth.s match outright.

**The label depth, settled.** After a `Jump`, retail's block depth is the
port's + 1 in all 830 label runs (2–5 against 1–4): retail counts the
object's own `BEGIN` (target 1). `Continue` ends a block only when the
depth drops below 1 after a line (`0x00493c99`), so at the trigger's `END`
retail is still at 1 and goes on: it hands the next trigger's header line
to the command interpreter as a command, runs that trigger's block as part
of the current one, and stops at the object's `END`. This follows *every*
jump -- a `jump` command or a taken response, to any label, nested or not
-- so it isn't limited to target 1's 13 trigger-level labels. A trigger
block that executes a jump runs on the same way (Jong1's and RandK's first
trigger). When the trigger is the object's last, the only extra line is the
object's `END` (136 label and 5 trigger runs): nothing happens.

Where it reaches another trigger (shipped): 19 module objects and 3 of the
demo, 91 labels or trigger blocks -- dungeon.s Druhgslave2, Druhgslave3,
Slave2; forest.s Olihoot1, Jong1, Gatekeeper1; keep.s DalyK, RandK,
SteffanK, SardokT; town.s Heather1, Verhoeven1, Geralt1, CAMERON1,
HRUTHFORD1, GINA1, CRONUS1, BAYNE1, Rubold1; demo.s GINA1, CRONUS1, Rubold1.
In all of them the label's trigger is the DIALOG block and the next trigger
is `ALWAYS`, except BAYNE1, where it is `CUBE player 3335,20211,0
3440,20231,300`. Neither header is a command (TCharacter's ParseCommand
answers "unrecognized"; "Unrecognized command." goes to the console, shown
only in the editor), so the block goes on with that trigger's `BEGIN` and
body:

- **the ALWAYS body runs once, in full, as the tail of the conversation**,
  its IFs evaluated then: DalyK walks his patrol (`GOTO 12300 12594`,
  `PIVOT 190`, `WAIT 30`, `GOTO 12132 12590`, `PIVOT 60`, `WAIT 30`);
  SteffanK, Gatekeeper1, Heather1, Verhoeven1, Geralt1 (39 commands),
  CAMERON1, HRUTHFORD1, GINA1, CRONUS1, Rubold1 walk their waypoint
  routes; Druhgslave2/3 and Slave2 pick (`TRY PICK`) or face their spots by
  SABUKILLSTATE; Olihoot1 floats (`TRY "FLOATING"`, `WAIT 24`); RandK and
  SardokT turn invisible (`TOGGLE INVISIBLE = 1`) when TENDRICKSTATE > 1 /
  SARDOKSTATE = 5; Jong1 waits 24 ticks and, if JONGLOOKSTATE = 3, sets the
  player's health to LOKHEALTH -- every path out of his training dialog
  (`:TRAINEND`) sets JONGLOOKSTATE to 1 first, so that never happens there;
- **BAYNE1**: the CUBE body is `IF BAYNESTATE = 0` (the street fight); the
  dialog that runs into it only happens at BAYNESTATE 1 and sets 2 before
  its end, so the IF is false: nothing.

**What the player sees in the three scripts named in target 1**:

- *keep.s DalyK* (`:A`, `:B`, `:C`, `:Marker`; `:Start` is waited at):
  after the talk ends (`control on`, `SETCDVOLUME FULL`), Daly walks one
  round of his patrol -- the same route his ALWAYS block walks -- as part
  of the conversation's block. While it runs he can't be talked to again:
  the block is still the one the player started, and its trigger guard
  (the player's id at `+0x10`, set when the block started, cleared by
  `End`) keeps every trigger of his from firing while the player exists.
  When the round ends, his interrupted ALWAYS block resumes where the
  conversation cut it (`+0xac`), so he walks one extra round before
  picking up where he was.
- *keep.s SteffanK* (`:letyoulive`; `:Start`, `:misthaven` and `:cult`
  reach a response wait or the jump that leads there): the same with his
  four-leg `goto … / wait 10` patrol.
- *forest.s Jong1* (`:COMBAT`, `:SPARENDYES`, `:TRAINEND`, the
  `:complete1`–`8` lessons; `:SPARYES` stops at its response wait): one
  second (`WAIT 24`) after the lesson ends during which Jong can't be
  talked to; nothing else (JONGLOOKSTATE is 1 by then).

So, with the shipped scripts, nothing new appears on screen: the NPC's
idle routine starts one pass early, inside the conversation's block, and
for that pass he refuses a new conversation. The port ends the block at the
trigger's `END` and is unchanged; whether to follow retail is a decision
(DIALOG.md §8).

By-product: Jong1's DIALOG block jumps to `NOCOMPLETE2` … `NOCOMPLETE9`
(forest.s lines 660–688) and only `:nocomplete1` exists; with TRAINSTATE
2–9 both retail and the port report "Jump to an unknown label attempted"
on the text bar and go on after the jump line.

Other differences (edge files only):

- **`ELSE IF`** (no shipped script has one). Retail's interpreter leaves
  the rest of a line unread when the result has bit `0x100` (ELSE); then
  `Continue`, finding `IF` after the ELSE (`0x00493b06`), rewinds to the
  line's start and runs the IF on the next pass. After a true IF, its skip
  of the else part consumes the next line -- the ELSE IF's `BEGIN` -- so
  retail runs the ELSE IF's body anyway and ends the trigger at its `END`;
  after a false IF it evaluates the ELSE IF and goes on (and reports "Bad
  token in trigger block"). The port's interpreter skips the rest of every
  line, so the IF is never run and the next ELSE reports "ELSE without
  matching IF".
- **An ELSE after a jump into a block**: retail's block slots that the jump
  opened hold whatever the script last left there (0, "false", in a fresh
  script) and run the ELSE body; the port's are COND_UNDEF and it also
  prints "ELSE without matching IF" on the text bar. Same lines.
- **Depth 10**: retail's block array (`+0x54`, 10 slots of 8 bytes) ends at
  `+0xa4`, the depth itself; at depth 10 the slot's loop-start field *is*
  the depth, so the first line there resets it to 0 and ends the block. A
  label 9 levels deep in the port is 10 in retail (shipped labels: retail
  depth 5 at most).
- **Unknown label**: the same behaviour; the error's line number differs
  (retail counts from the OBJECT line).
- **An unclosed object** (`unbalanced.s`): the port's text for it differs
  (target 1), and it reports "Bad token in trigger block" each pass.

**Speed**: 91 s for all 18 files in 6 processes (`--jobs 6`; the driver
splits a file's runs across processes, `step_split`/`step_merge`), 524 s of
emulation in all. Before the caps and the split: 1,428 s in one process
with the Python write hook, 1,030 s with the native one (identical retail
dumps, all 18 files); forest.s was 85% of it. The port: 0.9 s.

### Target 5: the trigger test — done

```sh
python3 tools/retail_ab/retail_ab.py trigger-test
```

**Retail side** (`trigger_test.py`): the original trigger test `0x004927b0`
(`TScript::Triggered(trigger, priority, context)`) on one trigger record
per case, in a fixture world: objects in guest memory with the fields the
test reads (class `+0x04`, level `+0x0e`, position `+0x10..+0x18`, name
`+0x38`, id `+0x40`), one of them the owner, one the player (the global
`0x00667fcc`) or none; a script from the original constructor with the
case's request state (type `+0x18`, strings `+0x20`/`+0x34`, guard
`+0x10`, running record `+0xa8`). Recorded boundaries: the map iterator
`0x0044cf80`/`0x0044d080` as the CUBE search `0x00452480` drives it (the
world's objects on the level asked, in list order; the search's own
inclusive containment test runs as original), and the object lookup by id
`0x00452690` (the guard exists or not).

**Port side**: `TScript::Triggered` through a probe it befriends
(`RetailAB::TriggerProbe`: sets the request state and the guard, reads the
user, alias and guard back). Its world reads go through a new seam,
`TScript::ITriggerWorld` (the main player, and MapPane.ObjectInCube's
answer; unset in the game, where they are `Player` and the map pane). The
fixture world answers ObjectInCube with that function's loop -- the first
moving object inside the cube by `S3DRect::In` -- over the objects on the
window's level (the player's, else the owner's). Fixture objects are
`TObjectInstance`s with a name, class, level and position, registered in
the map pane's index under their ids so the guard's `TSafeRef` finds them.

**Compared**: fires, the user recorded (`+0xc4` / `triggerer`), its alias
(`+0xcc` / `useralias`), the guard after.

**Cases** (138): every type 0–12 with no request, its own request and
another's; priorities below, equal and above the running one; the running
record; TRIGGER/GIVE/GET/USE names (case, the second USE string, empty,
19 characters); PROXIMITY over the player's name and the box and circle
edges, dist 0, negative, 40,000 (2·dist² past 2³¹), the player absent or on
another level; CUBE named `player`/`PLAYER`/the player's name inside, on
each face, past each face, reversed corners; CUBE searches: another
character, the owner first or second in the list, unnamed, named object,
an item first, a second player-class object, no player, other levels; the
guard existing or gone.

**Results** (case set `c2fb3b2c…`, port `5e76b0f`): 134 match. The rule is retail's
(SCRIPT_ENGINE.md §3) in every case but two kinds:

- **The running-trigger record** (`+0xa8` equal to the trigger: retail
  never fires it). Unreachable: nothing in the script engine
  (`0x00492000`–`0x00497f00`) stores to `+0xa8`, and the constructor leaves
  it as allocated (zero). The port has no such field. One case.
- **Whose level the CUBE search walks**: retail asks `0x00452480` for the
  owner's level (`+0x0e` of the context); the port's MapPane.ObjectInCube
  walks the map pane's window, i.e. the player's level. With the owner on
  level 1 and the player on 0, a character in the cube on level 1 fires
  the trigger in retail, not in the port, and one on level 0 the reverse.
  Three cases. Same answer whenever owner and player share a level. Read
  from the code, not tested here (the fixture has no sectors): retail
  walks the loaded sectors of that level inside the cube's rectangle
  (iterator flags `0x6e0`), the port only the pane's 3×3 window, so a cube
  outside the window is never searched in the port.

**Speed**: 138 cases in 0.1 s (~1,000 cases/s); the port 0.03 s.

### Speed: checkpoint restore (shared core, 2026-10-07)

The runtime's per-write dirty-page hook was a Python callback on every
guest store. It is now native: `dirtypages.c`, a `UC_HOOK_MEM_WRITE`
callback in C built with `cc` on first use (`tools/retail_runtime/README.md`,
"Checkpoint restore"). Unicorn 2.1's copy-on-write context snapshot was
tried first and rejected: in 2.1.4 it refuses `mem_protect` after a
snapshot (VirtualAlloc commit/decommit), brings back on restore a region
mapped and unmapped after the snapshot, and crashed the process after a
restore that followed a new mapping.

| | before (Python hook) | after (native) |
|---|---|---|
| script parse, forest.s alone | 12.5 s | 4.0 s (no tracking at all: 2.8 s) |
| `retail_ab.py script-parse`, 29 cases, one process | 56 s | 19.6 s |
| `retail_ab.py script-step`, 18 files, one process, uncapped | 1,428 s | 1,030 s |
| flame VFX probe, median warm pair / setup | 12.0 / 127 ms | 10.8 / 73 ms |
| one guest store | 770–1020 ns | 130 ns (none: 100 ns) |

Identical results: the parse dump hash and dirty-page count, all 18
stepping retail dumps, the flame manifest apart from timings. The six
runtime suites pass before and after (53 tests at the last run; test_memory
gains one that runs guest stores under both trackers).

The driver also runs `--jobs N` fixture processes (default half the
cores): say-duration 22 → 15 s, dialog-layout 1.1 → 0.3 s with 4.

### Next

- **The run-on decision** (target 4): follow retail (count the object's
  `BEGIN` -- the port's text would have to keep retail's extent, target
  1 -- and let a block end only below depth 1) or keep the port's ending
  at the trigger's `END`. The A/B above is the evidence; the visible cost
  is small either way.
- **The CUBE search level** (target 5): retail searches the owner's level
  over its loaded sectors; the port's map pane its window. Likely a port
  fix (a level-aware, loaded-sector search, as `FindClosestObject`
  already does, COMMAND_SYSTEM.md §2.3), to be decided.
- **The rest of `Continue`**: waits across calls (`WaitSatisfied`
  `0x00492d70`) and the trigger scan (priorities, an interrupted ALWAYS
  block's resume at `+0xac`): a run-loop A/B with several `Continue` calls
  and trigger requests, on the same fixture world.
- **The command interpreter's argument parsing** (plan 5): the command
  boundaries of `script_step.py` are the start -- a boundary per command
  that records its parsed arguments instead of its effect.
