# New Game sequence — retail script, command and dialog restoration

Working plan for getting **Main Menu → New Game → the opening resurrection
scene** to play through on retail data, by restoring the retail script engine,
command interpreter, dialog system and save loading. Owned by the gameflow
track (`feature/gameflow`); covers BURNDOWN items T1, T3, T4, T5, T8, T9.

## 1. Target

Exit criteria, in order:

1. `./build/Revenant` plays the intro FMV, shows the main menu, and **New Game**
   starts the game the way retail does.
2. `--quickstart` (retail `QUICKSTART`) skips intro + menu straight into a new
   game; `--quickstart=<save>` loads that save. `--menu=<button>` drives the
   main menu (auto-presses New Game / Load Game / Options / Exit) for tests.
3. A new game puts Locke in the Keep resurrection chamber from the module's
   `newgame.sav`, and `keep.s` `OBJECT "SardokR"` → `CUBE` trigger fires.
4. The resurrection scene runs end to end: fade-in, `try resurrect`, Sardok and
   Tendrick speak (text + voice), the three dialog choice menus work, the
   inventory hand-out happens, Rahul appears and attacks, and after Rahul dies
   `TendrickR`'s `ALWAYS` script plays the aftermath and returns control.
5. Other scripted scenes (town/forest NPC dialogs) work the same way.

## 2. What retail does (decoded)

All addresses are retail `Revenant.exe`; decomps are under `recon/discovered/`.

**Boot** — WinMain `0x004865a0` (`FUN_004865a0_WinMain.cpp`):
- Normal start: play `<MoviePath>\Mix_FMV1.smk` (`MoviePath` INI key, default
  `.\Disk2` in the shipped `revenant.ini`), then `NextScreen = LogoScreen`.
- `EDITOR`: `PlayScreen.SetStartMode(2)`, `NextScreen = PlayScreen`.
- `QUICKSTART` (GetParameters `0x00483bc0`): `SetStartMode(0)` (new game), or
  `SetStartMode(1, name)` with `QUICKSTART="name"` (load game), then PlayScreen.
  No intro, no menu.
- Multiplayer/lobby launches go to the MP screen (out of scope).

**Main menu** — TLogoScreen `cls_0x5a5d18`, Initialize `0x0053a2c0`: five
buttons from `menus.dat` (`MenuNewGame/LoadGame/Multi/Options/Exit`). Handlers:
- New Game `0x0053a1f0`: `PlayScreen.SetStartMode(0,-1,-1,nullptr)`;
  `NextScreen = PlayScreen`; close.
- Load Game `0x0053a220`: `NextScreen = LoadGame screen` (`0x0066fa78`).
- Multi `0x0053a240`: MP screen. Options `0x0053a260`: options screen.
- Exit `0x0053a2a0`: `NextScreen = nullptr` (quit).

**Start modes** — `TPlayScreen::SetStartMode` `0x0047f4c0` stores
`mode / x / y / name` at `+0x6d8..+0x6e4`; `TPlayScreen::Initialize`
`0x0047a660` consumes them after building the panes:
- `0` new game: `MapPane.ClearCurMap()`; `LoadNewGame()` `0x0048e610` =
  `LoadGame("newgame", 1)` `0x0048df70` → reads `<module>\newgame.sav`.
- `1` load: `LoadGame(name)`; on failure shows `GAMENOTFOUND` and falls back
  to `LoadNewGame()`.
- `2` editor, `3` multiplayer.

**`newgame.sav`** (Ahkuilon, 2396 bytes) is a retail-format save: 0x80-byte
header (gametime, …, version `15` at `+0x1c`, module name `"ahkuilon"` at
`+0x20`), then the TGameState stream (82 states, names XOR `0x80`), then the
player. The pre-release `TSaveGame::ReadGame` does not understand it.

**Opening scene** — `keep.s`, `OBJECT "SardokR"`, trigger
`CUBE 1145,475,0 1265,619,60`. The new-game player position is inside that
cube, so the scene starts as soon as scripts pulse. Commands it needs:
`control off/on`, `incidentals on/off`, `fadecharacterout/in`, `centeron`,
`playerlevel`, `stat`, `wait N`, `nowait add`, `move`, `toggle invisible`,
`try resurrect`, `setcdvolume half/full`, `say <tag>`, `scrollto`,
`pivotobject`, `goto`, `choice`, `wait response`, `jump`, `addinv`, `get`,
`play`, `state`, `set`, `toggle pause`, then Rahul's fight and `TendrickR`'s
`ALWAYS` aftermath (`if Rahul.stat health = 0`, `force walk`, `pos … level`).

## 3. Shared contracts

These are fixed so the workstreams below can proceed in parallel. Change one
only by updating this section in the same commit.

### 3.1 Command handlers

`COMMAND(x)` = `int32_t x(TObjectInstance* context, TToken& t,
TObjectInstance* scriptcontext, TScript* script)` — the retail argument order
from `CommandInterpreter` `0x0041e8e0`:
- `context` — the object the command applies to (`<obj>.` prefix, else the
  caller's context).
- `scriptcontext` — the object whose script (or console) issued the command.
- `script` — the running `TScript`, or `nullptr` from the console/editor.

The table `Commands[]` in `src/command.cpp` mirrors retail `SCommand[189]` @
`0x005c6e88` (see `recon/discovered/commands/RETAIL_COMMAND_TABLE.txt`).
Commands not yet ported are stubs `CmdNotPorted(...)` grouped by owner at the
end of `command.cpp`. **Port a command by replacing its stub body; do not
re-order or re-flag table rows owned by someone else.** Each retail handler
body is `recon/discovered/commands/cmd_<name>_<addr>.cpp`.

Return values (pre-release names, retail values): `0` done, `CMD_WAIT` (1)
script must wait, `CMD_BADCOMMAND` (2), `CMD_BADPARAMS` (4), `CMD_OUTOFMEM`
(8), `CMD_USAGE` (0x10), `CMD_DELETED` (0x20), plus the control-flow bits.
Retail adds `0x4000` = "wait until the target finishes speaking". Mirror the
retail handler's return value exactly; the interpreter decides what it means.

### 3.2 Waiting (script engine owns; everyone else feeds it)

Retail keeps the wait state on the **script**, not the character
(`TObjectInstance +0x84` is its `TScript*`):
- `TScript::WaitChar(target)` `0x00492cf0` — after a command returns
  `CMD_WAIT` against another object (interpreter calls it through
  `0x00471310`).
- `TScript::WaitSay(target)` `0x00492d10` — after `say` / a `0x4000` result
  (`0x00471330`).
- `wait N` → `0x004712d0`; `wait screenfade` → wait type 6; `wait buysell` → 7;
  `wait response` / `responsenohide` / `respnohide` → 2; `respctrlon` → 10;
  `wait char|obj|object <name>` → WaitChar; `wait death <name>` → `0x00471350`.
  (Decomp: `cmd_wait_41fe30.cpp`.)
- `nowait <command>` turns a `CMD_WAIT` result into `0`.

Polling sides of those waits are seams owned by other workstreams:
- **screen fade** — `bool ScreenFadeInProgress()` in `src/screenfade.h`
  (presentation implements, script engine polls).
- **speech** — `TCharacter::IsTalking()` (dialog implements the retail talk
  state, script engine polls). Keep the name.
- **dialog response** — `TDialogPane` API `Show/Hide/AddChoice(label,text)/
  HasResponded/GetResponseLabel/ResetResponses/Skip` and the global
  `DialogPane` (dialog implements the retail pane behind it, script engine
  calls it). Retail `TScript::AddChoice` `0x004932a0` forwards to
  `DialogPane.AddChoice` (`0x00535870`) in single player.
- **character actions** (walk/pivot/attack done) — the existing
  `TCharacter`/`TComplexObject` action queue (`doing`, `IsDoing`, …).

### 3.3 Speech

`TCharacter::Say` retail `0x004d0950` (`text, wait, anim, sound`) → core
`0x004d0610`; `SayLine(dialogline, wait, anim)` `0x004d09b0` looks the tag up
in `DialogList` (`0x0049d780` text, `0x0049d7c0` voice). Voice files are
`<module>/Sound/english/<tag>.mp3` (e.g. `i1sar00.mp3`); text is
`english.def`. `0x00586e60` / `0x00586f00` / `0x00586f80` are multiplayer
broadcasts — skip.

## 4. Workstreams

Each runs in its own worktree/branch off `feature/gameflow` and merges back.

| Workstream | Owns | Done when |
|---|---|---|
| **A. Boot + menu + start modes** (lead) | `revmain.cpp` boot routing, real `TLogoScreen` screen, `TPlayScreen` start modes, `--quickstart`, `--menu=`, `--nointro` | Exit criteria 1–2; New Game reaches PlayScreen through the retail start-mode path |
| **B. Script engine** | `script.{h,cpp}` (except `TGameState::LoadStream/SaveStream`), `CommandInterpreter` + context resolver `0x0041e690` (aliases `player`/`me`/`chr`/`obj`, `group<N>.`), control flow (`begin/end/if/else/while/set/wait/jump/trigger`), stubs in the *script engine* section, trigger detection (CUBE/PROXIMITY/DIALOG/USE/ACTIVATE/GET/GIVE/COMBAT/DEAD/ALWAYS) | `TScript::Continue/Triggered/End` + interpreter retail-synced; all wait types work against the seams in §3.2 |
| **C. Dialog** | `dialog.{h,cpp}` (`TDialogList`, retail `TDialogPane` per `docs/ui/forensics/DialogPane_SPEC.md`), `TCharacter` speech (text display + voice + talk state), `say`, `choice`, stubs in the *dialog* section | NPC speech shows + voices, choices render and route back to the script |
| **D. Savegame** | `savegame.{h,cpp}`, retail `LoadGame` `0x0048df70` / `WriteGame`, `TGameState::LoadStream/SaveStream`, automap blob, stubs in the *savegame* section | `LoadGame("newgame",1)` lands Locke in the chamber with newgame.sav state; save/load round-trips in retail format |
| **E. World + character commands** | stubs in the *world + character* section, and the existing movement/combat/inventory/object commands (`goto/go/pivot/face/stop/combat/attack/use/try/force/state/stat/toggle/pos/move/add/addinv/delinv/give/get/take/delete/burn/cast/…`) | Every world/character command the shipped scripts use matches its retail body |
| **F. Presentation commands** | stubs in the *presentation* section + `control`, `centeron`, `scrollto`, `play`, `play3d`; `src/screenfade.cpp`; in-game `playmovie` (modal, script waits) | Fades, character fades, music volume/track swap, camera moves, in-game movies, `endgame` all work |

Deferred (stubs stay): buy/sell screen commands, multiplayer group commands,
editor tooling.

## 5. Ground rules

- **Retail first.** The decomp is the source of truth; the 1998 snapshot is a
  readable guide that may differ. Read `recon/discovered/…` before writing
  code, mark ported functions with `// REVSYNC: <name> @ 0x<addr>` and note
  deliberate divergences inline. New decomps go in `recon/discovered/` named
  `<class>_<Name>_<addr>.cpp`.
- **No bodges, no stand-ins.** If retail needs a subsystem that isn't ported,
  port the piece you need or stop and report it — don't fake it.
- **Stay in your lane.** Edit other workstreams' code only when you must, keep
  it minimal, and say so in your commit message and report.
- **Modern C++ while you port** (per `docs/` conventions and the existing
  code): `const`, `nullptr`, field initializers, no new macros, no raw
  `fprintf(stderr)` — use `log_info/warn/error` from `src/logging.h`.
- **Verify by running the game.** Headless runs:
  `./build/Revenant --headless --max-runtime=30 [--loadmap=…] --exec="…"`
  then read the log; `--filmstrip=N,0 --snapprefix=<dir>/x_ --input-script="…"`
  for pictures. `--exec="cmd; sleep 24; cmd"` runs console commands as the
  player (see `src/consoleexec.h`).
- **Commit to your own branch** with clear messages; keep each commit building.

## 6. References

- Script sources (read-only copies): `RevenantRevisited/data/Modules/Ahkuilon_unzipped/*.s`
  and `RevenantRevisited/data/resources_unzipped/master.s` in the main checkout.
  At runtime they come from `data/Modules/Ahkuilon.rvm` and `resources.rvr`.
- Dialog text: `english.def` (module); voice: `Sound/english/*.mp3` (module).
- UI forensics: `docs/ui/forensics/DialogPane_SPEC.md`, `FloatingText_SPEC.md`,
  `MainMenu_SPEC.md`, `TTextBar_SPEC.md`.
- Save format: `recon/discovered/save_system_notes.md`,
  `docs/gameflow/T5_FORENSIC.md`, `docs/SAVE_GAME.md`.
- Ghidra: `recon/discovered/README.md`; batch decompile with
  `recon/ghidra_scripts/DecompileBatch.java <listfile>` (lines of
  `<hex addr> <out path>`). Use your own copy of the project
  (`cp -R data/RevenantDev.gpr data/RevenantDev.rep <dir>/`) so parallel runs
  don't contend for the project lock.
