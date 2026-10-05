# Gameflow architecture — screens, game flow, game session, commands

Design for the systems the gameflow track owns: the **screen system**,
the **higher-level game flow** (title → game → death/end → title), the
**game session** that owns the world, and the **command system** that
scripts and the console drive. The production **HUD** is rebuilt on the
screen system and is covered here as its first consumer.

Requirements and retail behavior come from the forensics:
[forensics/SCREEN_SYSTEM.md](forensics/SCREEN_SYSTEM.md),
[forensics/GAME_FLOW.md](forensics/GAME_FLOW.md),
[forensics/SAVE_GAME.md](forensics/SAVE_GAME.md),
[forensics/COMMAND_SYSTEM.md](forensics/COMMAND_SYSTEM.md).
Engine conventions this extends: [../FRAME_PIPELINE.md](../FRAME_PIPELINE.md),
[../RENDERER_ARCHITECTURE.md](../RENDERER_ARCHITECTURE.md),
[../ui/ARCHITECTURE.md](../ui/ARCHITECTURE.md),
[../ui/CONVENTIONS.md](../ui/CONVENTIONS.md),
[../PORT_PLAN.md](../PORT_PLAN.md) §2.

**Invariant:** player-visible behavior matches retail exactly (screens,
transitions, timing, script semantics, save files). Structure is modern
where that costs nothing in fidelity; where retail's structure can't be
kept (re-entrant frame loop, world owned by a screen), the divergence is
deliberate and recorded in §7.

## 1. Layers

```
 TGameFlow ─────────── owns ──────────► TGameSession (world lifetime)
   │ sequences screens                     │ map, players, scripts, game
   ▼                                       │ state, time, load/save jobs
 TScreen (current)  ◄── presents ──────────┘
   │ pane tree + modal stack
   ▼
 TPane subtree ──compose/draw──► TRenderer (HUD layer)
   ▲ input (AppEvent → screen → modal top / hit-tested pane)

 Scripts / console ──► CommandInterpreter ──► command handlers ──► game objects
```

- **TGameFlow** decides *which* screen is up and *when* the world
  starts, loads and ends. Nothing else switches screens.
- **TGameSession** owns everything that lives exactly as long as one
  play-through: what retail kept alive between `TPlayScreen::Initialize`
  and `Close`.
- **TScreen** presents and routes input; **TPane** trees hold UI state
  and draw through the renderer. `TPlayScreen` presents a session; it
  does not own it.
- The **command system** is the one way scripts and the console act on
  the world; handlers are thin parsers over game-object methods.

## 2. Game flow — `TGameFlow` (new)

Retail has no flow object: screens poke `nextscreen` globals from
button callbacks, `TPlayer::Animate`, the `endgame` command and WinMain.
`TGameFlow` centralizes that graph so every transition is one named
intent with one implementation.

```cpp
class TGameFlow
{
  public:
    // Boot: intro -> title, or straight into a game/editor (QUICKSTART,
    // --quickstart=<save>, EDITOR, --test routes stay in AppInit).
    void Boot(const SBootOptions& options);

    void StartNewGame();                    // title "New Game"
    void LoadGame(const char* slotName);    // title / in-game / death "Load"
    void RestartAfterDeath();               // death "Restart" (see §8 Q1)
    void PlayerDied();                      // from TPlayer death countdown
    void ReturnToTitle();                   // Quit Module, death Exit, endgame
    void QuitApplication();                 // title Exit, in-game Exit

    TGameSession* Session();                // null outside a game
};
extern TGameFlow GameFlow;
```

- Implemented on the existing screen mechanism: an intent sets the
  current screen's next screen and marks it done; `AppFrame` performs
  the swap. `BootScreen` routing in `AppInit` shrinks to
  `GameFlow.Boot(options)` (+ the `--test` screens, which stay outside
  the flow).
- Screen objects stay globals (`LogoScreen`, `PlayScreen`,
  `DeathScreen`, …) as in retail; only the flow references them for
  transitions.
- Test/dev entry points are `SBootOptions`: `--quickstart[=save]`
  (retail `QUICKSTART`), `--nointro`, `--menu=<button>` (presses a title
  button through the title screen's real button path), `--loadmap`
  (alias of `--quickstart=<save>`).

## 3. Game session — `TGameSession` (new)

Retail's `TPlayScreen` conflated presentation with the world. The
session takes the world half; the managers it coordinates are unchanged.
Retail behavior: [forensics/SAVE_GAME.md](forensics/SAVE_GAME.md) (save
format, `LoadGame`/`SaveGame`, `curmap`) and
[forensics/GAME_FLOW.md](forensics/GAME_FLOW.md) §2.3 (start modes).

| Responsibility | Retail | Port owner |
|---|---|---|
| Mount start module, areas | `PlayScreen::Initialize` (`0x004609f0`, `TAreaMgr::Initialize`); `Close` | session start / `End` |
| Script prototypes (`master.s`), `state.def` names | `TScriptManager::Initialize` per game | boot (`InitGlobals`), unchanged — see §3.4 |
| Per-game script and state reset | inside `LoadGame` (`0x00496e20`, `ReloadStates`) | `TSaveGame::LoadGame`, as retail |
| Sector working set (`curmap`) | `TMapPane::{Clear,Load,Save}CurMap` | `TMapManager` (sector owner); `TMapPane` methods delegate |
| Save file read/write, slot list, merchant table | save manager (`0x0048d260..0x0048e820`) | `TSaveGame` (evolved to the retail format) |
| New / load / save / end a game | `PlayScreen::Initialize` start modes, in-game menu | `TGameSession` |
| Load the player's level, place players | `PlayScreen::Initialize` (`0x004997d0`) | `TGameSession` |
| Effect imagery, panes, cursors, fades | `PlayScreen::Initialize` | `TPlayScreen` (presentation) |
| Game time, deferred load/save, simulation tick | PlayScreen (`+0x680..`, `+0x5e4..`, `Pulse`) | `TGameSession` — step 2e |

### 3.1 Components

```cpp
// src/gamesession.h — owned by TGameFlow (GameFlow.Session()).
class TGameSession
{
  public:
    // Start a game: queue the load steps for a new game (start mode 0) or a
    // named slot (start mode 1; a slot that can't load -> new game, as retail).
    void Start(const SSessionStart& start);
    bool Step();                         // run the next load step; false when done
    float Progress() const;
    bool  Ready() const;
    bool  Failed() const;

    void End();                          // leave the game: unload world, clear players, close areas

    // Requests made during play, carried out at the start of the next tick
    // (retail PlayScreen +0x5e4..+0x5f0).
    void RequestLoad(const std::string& slot);
    void RequestSave(const std::string& slot);
    void RequestQuickSave();             // retail 0x0047e850
    void RequestReloadLastSlot();        // developer convenience (F9), not retail
    void ProcessRequests();              // called by TPlayScreen::Update
};
```

- **`TGameFlow` owns the session.** Its intents start games through it
  (`StartNewGame`, `LoadGame`, `RestartAfterDeath`, QUICKSTART) and it
  ends the game when the screen that follows the PlayScreen isn't the
  PlayScreen (`ScreenEnded`, called by `AppFrame` after the swap). The
  game therefore never ends inside a simulation tick (e.g. from
  `TPlayer`'s death countdown).

- **`TSaveGame`** keeps its class identity and becomes retail's save
  manager: `SSaveHeader` and the body codec (game states, merchant
  table, player list) are separate from orchestration, so the format can
  be tested on a buffer; `LoadGame(name, flags)`, `LoadNewGame()`,
  `SaveGame(name)`, `RefreshSlots()`, `HasSoldUnique/AddSoldUnique`
  follow the retail functions one for one. The reset sequence in
  `LoadGame` calls the ported subsystem for each retail call and marks
  the unported ones (dialog, buy/sell, area exit, control) with their
  retail address.
- **`TGameState::LoadStream/SaveStream`** move to
  `TInputStream`/`TOutputStream` (the binary streams the format uses).
- **Working set.** `SectorStore` (`src/sectorstore.h`) owns where sector
  files live: the working set `<SavePath>/curmap` (created on demand)
  and the base map through the resource layer — never the install's
  `curmap`. `TMapManager` owns the loaded sectors and gains
  `FlushSectors`, `ClearCurMap`, `LoadCurMap(dir)`, `SaveCurMap(dir)`.
  `rev_fopen` write modes resolve only under SavePath.

### 3.2 New game, step by step

1. `TGameFlow::StartNewGame()` → `Session().Start({NEWGAME})`.
2. Session steps: module + areas → `ClearCurMap` → `LoadNewGame`
   (retail resets, `newgame.sav`, players, game time) → load the main
   player's level (`MapManager.SetCurrentLevel`) → put each player into
   its sector.
3. The flow switches to `TPlayScreen`. Its `Initialize` builds
   presentation only (renderer, HUD, cursors, effect imagery) and binds
   the renderer to `MapManager`'s current map, following
   `CurrentMapChanged` from then on.

A load from a slot is the same with `LoadCurMap(<slot>/CurMap)` +
`LoadGame(slot)`. `End()` runs when the flow leaves the game (title,
death, quit).

### 3.3 What this replaces

- `SpawnDefaultPlayer` + starter loadout in the boot path, and the
  Level-10 floor in `TPlayer::ClearPlayer` (stand-ins for `newgame.sav`).
  The editor's "Place Here" keeps its own use of `SpawnDefaultPlayer`.
- The pre-release `game.sav` wrapper, including HUD state in slots 2–12
  and the automap blob (neither exists in retail saves).
- The renderer choosing the starting level from defaults. Dev overrides
  `--level=L` / `--sector=L_X_Y` still work: the new game loads normally,
  then the player is moved to that sector at walkmap height.

### 3.4 Decisions

- **Script prototypes stay boot-loaded.** Retail re-parsed `master.s` per
  game, but prototypes are static data, `--test` hosts need them, and
  every per-game reset retail performs is in `LoadGame` anyway. (This
  revises the earlier plan to move `ScriptManager.Initialize` into the
  session.)
- **Synchronous first, then staged.** The load is written as an ordered
  step list from the start; 2c runs all steps in one call, 2d adds
  `TLoadScreen`, which runs one step per frame and draws the loading bar
  with retail's increments (forensics: `0x00448680` calls in
  `PlayScreen::Initialize`). No API change between the two.
- **Presenters follow `TMapManager::CurrentMapChanged`** to rebind after
  a load or level change; it is the existing "the world was replaced"
  signal, so the session adds none of its own. Borrowers of sector
  pointers check the map they borrowed from (`TMapPane` keeps a
  `TSafeRef<TGameMap>`).
- **No automap persistence until the retail automap files are ported**
  (SAVE_GAME §8). Dropping the pre-release blob loses automap state
  across save/load in the meantime; carrying it in a retail save is not
  an option.

### 3.5 Delivery

| Step | Content | Verified by |
|---|---|---|
| 2a ✓ | Working set + write policy (`SectorStore`, `rev_fopen`, `TSector`, `TMapManager`) | sectors written under SavePath, nothing written to the install; level 0 loads the pristine 35,549 objects |
| 2b ✓ | `TSaveGame` retail format + `TGameState` streams + slot list | `newgame.sav` parses; header, game states and merchant table written byte-identical to retail (player body: see SAVE_GAME §10) |
| 2c ✓ | `TGameSession`, flow/PlayScreen integration, in-game requests, stand-ins removed | `--quickstart` and title New Game: Locke L1, 25/25 HP, in the Keep resurrection chamber (filmstrip); `--savecycle-test` round trip identical |
| 2d | `TLoadScreen` + staged steps | loading bar matches retail; filmstrip |
| 2e | Game time and the simulation tick into the session | time and time-of-day match retail (TIME.md) |

- `IRuntimeMode` (game/editor) stays inside `TPlayScreen`: it is a
  presentation/input policy, and editor mode decides whether the session
  ticks.

## 4. Screen system — evolving `TScreen` / `TPane`

Class identities stay; the changes make the A.2 retained tree the real
UI model and replace the re-entrant modal loop.

### 4.1 Drawing

- **One HUD layer per screen.** `TScreen` registers itself with the
  renderer while active and, in `Draw()`, walks its visible pane tree
  in order (then the modal stack) calling `TPane::Draw()`. Panes no
  longer register individual HUD drawables. Cursor, debug and editor
  overlays remain separate HUD drawables at their own z.
- **Two pane draw hooks** (new virtuals on `TPane`):
  - `Compose()` — runs before the frame's passes (the Animate phase)
    for dirty visible panes; re-renders the pane's cached `TSurface`
    with the `…ToTarget` primitives. The "cached panel" pattern the UI
    track established, made part of the pane contract.
  - `Draw()` — runs inside the HUD pass; submits the cached surface
    and any live per-frame draws (`DrawSurface`, `DrawBitmap`, text).
  Legacy `DrawBackground` / `Animate(bool)` remain only for panes not yet
  migrated and are removed as panes move over.
- Dirty tracking uses the existing `SetDirty` propagation; children
  compose before parents.

### 4.2 Modal stack (replaces exclusive panes + `RunModal`)

```cpp
using TModalDone = std::function<void(int32_t result)>;
void TScreen::PushModal(TPane* pane, uint32_t flags, TModalDone done);
void TPane::EndModal(int32_t result);   // pops and calls done(result) next frame
```

- Flags keep retail values: `0x10` only the top modal pulses/animates;
  `0x100` re-applies UI blit effects under the modal (retail
  `0x004aacb0`, purpose unconfirmed — stored, not yet rendered). Input
  goes only to the top modal.
- The modal stack *is* retail's exclusive-pane stack (retail `RunModal`
  pushes through `SetExclusivePane`), evolved with completion callbacks
  and results; the legacy `SetExclusivePane` callers keep working.
- Retail sequences written as nested `RunModal` calls (in-game menu →
  load → back to menu) become continuation chains:
  `PushModal(menu, …, [](int r){ if (r == LOAD) PushModal(load, …,
  [](int r2){ if (!r2) ShowInGameMenu(); }); … })`. Same screens, same
  order, same results; the frame loop is never re-entered.
- Screen events stay: `TPane::OnScreenEvent(code, param)` broadcast for
  closing (`0x100`), modal pushed (`0x101`) — retail `OnEvent`
  `0x00490960`.

### 4.3 Input

`AppEvent` → current screen → top modal if any, else the pane tree
hit-tested front-to-back including children (mouse-up still reaches
every pane, as retail). Drags capture the pointer through the existing
cross-pane drag state (`uidragstate`). `--input-script` feeds the same
path for every screen (test modes included).

### 4.4 Layout and resolution

Panes keep retail 640×480 rects as their Classic geometry; Revisited
canvases position them with A.2 anchors, laid out on the screen's root
when the canvas resizes. Classic output must stay pixel-identical.

### 4.5 DEF screens are panes

Retail DEF screens are `TButtonPane`s opened modally. The UI track's
DEF engine (`TDefScreen`, data-driven widgets) becomes a `TPane`
subclass so the in-game menu, load/save, options and popups push as
modals; the title's Load/Options screens host the same pane in a small
`TScreen`.

### 4.6 Screens

| Screen | Basis |
|---|---|
| `TCinematicScreen` | existing; intro and full-screen movies (opens files through `rev_fopen`) |
| `TLogoScreen` | retail title: `TButtonPane` + five `TButton`s from `menus.dat` |
| `TLoadScreen` | retail loading bar (`loadbar.dat` / module `loadscreen.bmp`), shows session load progress |
| `TPlayScreen` | presents the session; HUD panes; in-game menu, dialog, popups as modals; in-game `playmovie` as a modal pane |
| `TDeathScreen` | retail death screen with `TDeathPane` (Restart / Load / Exit) |
| Load / Options screens | small screens hosting the DEF panes (title route) |

## 5. HUD (production rebuild)

The retail HUD is a pane tree on `TPlayScreen` (forensics GAME_FLOW
§2.3 step 5): map pane, side pane (tabs, equip, automap, stats,
spellbook), bottom pane (bar inventory, quick spells, bottom bar), text
bar, dialog, player status bar. The rebuild:

- **Evolves the existing pane classes into their retail successors**
  (`TInventory`, `TStatPane`, `TSpellPane`, `TEquipPane`, `TTextBar`,
  `TQuickSpellPane`, `TDialogPane`; the 1998 `THealthBar`/`TStaminaBar`
  (`TStatusBar`) into retail's `TPlyrStatusBar`, `TMultiCtrlPane` into
  the retail side-tab control — each mapping confirmed in a HUD
  forensics pass before porting) and adds the retail containers
  (`TSidePane`, `TBottomPane`, `TSideTabsPane`, `TBarInvPane`,
  `TBottomBarPane`) as `TPane` subclasses in the tree, each drawing
  through §4.1.
- **Reuses the UI track's evidence**, not its structure: the specs'
  coordinates/assets and the harness draw code move into the pane
  classes; the `ui*test.cpp` harnesses become thin `--test` hosts that
  instantiate the production panes against a real player fixture. No
  "synthetic state" switches in production code.
- Panes bind to game data through `TSafeRef` (player, target) and the
  session; UI state that retail saved (`pane` in save slot 1) is saved
  the retail way. HUD-only state that retail did not save is not written
  into the retail save header.

## 6. Command system

Scripts keep retail's text-interpreted execution (decision 2026-10-04):
`TScript` advances through prototype text and hands one line at a time
to the interpreter. The command layer around it is restructured:

### 6.1 Descriptors and handlers

```cpp
struct SCommandContext
{
    TObjectInstance* target = nullptr;   // "<obj>." or the caller
    TObjectInstance* caller = nullptr;   // object whose script/console ran it
    TScript*         script = nullptr;   // null for console/editor
    bool             console = false;    // abbreviations, console output
};
using TCommandHandler = uint32_t (*)(SCommandContext& ctx, TCommandArgs& args);

struct SCommand                           // one row per retail entry
{
    const char*     name;
    TCommandHandler handler;
    int32_t         classcontext, classcontext2;
    bool            requiresparams, editoronly;
    const char*     usage;
    uint32_t        retailaddr;           // provenance
};
```

- The table stays one list in retail order (data, not code). Handlers
  live in per-family files (`cmd_flow.cpp`, `cmd_speech.cpp`,
  `cmd_character.cpp`, `cmd_object.cpp`, `cmd_inventory.cpp`,
  `cmd_presentation.cpp`, `cmd_gameflow.cpp`, `cmd_editor.cpp`),
  replacing the 3,700-line `command.cpp`; the interpreter and table
  stay in `command.cpp`.
- Result flags keep retail values (they are the protocol with
  `TScript::Continue`): `CMD_WAIT` 1, `CMD_BADCOMMAND` 2,
  `CMD_BADPARAMS` 4, `CMD_OUTOFMEM` 8, `CMD_USAGE` 0x10,
  `CMD_DELETED` 0x20, `CMD_CONDTRUE` 0x40, `CMD_CONDFALSE` 0x80,
  `CMD_ELSE` 0x100, `CMD_SKIPBLOCK` 0x200, `CMD_LOOP` 0x400,
  `CMD_BEGIN` 0x800, `CMD_END` 0x1000, `CMD_JUMP` 0x2000, and retail's
  `CMD_WAITSAY` 0x4000 — as a typed flags enum.

### 6.2 Arguments, objects, values

- **`TCommandArgs`** wraps the `TToken` stream with the primitives
  retail handlers use (keyword test with optional abbreviation, number,
  name, quoted text, optional `=`, end-of-line, rest-of-line), so each
  port expresses its grammar in a few calls and consumes tokens exactly
  like the retail body.
- **`ResolveScriptObject(name, caller, script)`** — retail resolver
  `0x0041e690`: `this`, `user`, `player`, `target`, `current`,
  `party<N>`, script aliases, then nearest object by name.
- **Values** — one evaluator for numbers/variables (game state,
  per-prototype `DATA`/`NUMBER` variables, local values, object stats)
  and conditions (retail `0x0041f230`), shared by `if`, `while`, `set`,
  `say`, `choice` and the handlers that accept variables.

### 6.3 Waiting

Handlers express waits only through the retail protocol: return
`CMD_WAIT` / `CMD_WAITSAY` (the interpreter calls
`script->WaitChar/WaitSay(target)`), or call `script->SetWait(type,
param)` for the typed waits (frames, response, screen fade, buy/sell,
death). The wait state machine lives in `TScript` (retail `0x00492b00`);
what each wait polls is owned by its subsystem (character actions,
`TCharacter::IsTalking`, `TDialogPane`, the PlayScreen fade, the
buy/sell screen).

### 6.4 Console

One mainline command queue (`TConsole`, replacing the stub
`TConsolePane`) feeds the interpreter for the editor's ImGui console
panel and `--exec`; output goes to the console view and the logging
facade. No separate queue elsewhere. Script errors are logged with
prototype name and line (diagnostics only; behavior unchanged).

### 6.5 Not-yet-ported commands

Registered with their retail row but answer **unrecognized** (retail's
behavior for a missing command: report, skip the line, continue) and
log once with the retail address. Nothing reports a success it did not
perform.

## 7. Deliberate divergences from retail structure

| Retail | Port | Why | Behavior impact |
|---|---|---|---|
| Re-entrant frame loop for modals (`RunModal`, `TimerLoop(1)`) | Modal stack + completion continuations | sokol owns the outer loop | none |
| World lives in `TPlayScreen` | `TGameSession` owned by `TGameFlow` | overlays, loads and movies don't rebuild the screen; testable | none |
| Loading bar repainted inside one long frame | Staged session load across frames | no re-entrant loop | bar animates the same |
| `nextscreen` set from many sites | `TGameFlow` intents | one transition graph | none |
| `master.s` re-parsed every game | parsed once at boot; per-game resets as retail | static data; `--test` hosts | none |
| `LoadGame` resets the world, then reads the file | reads and checks the file first | a missing or damaged save no longer leaves an empty world | only on failure |
| `TScriptManager` frees script instances on Close | objects own their scripts; the manager's list is non-owning | one owner (the port's objects already freed them: double free) | none |
| Sectors tracked by `TMapPane` | working set managed by `TMapManager` | `TMapManager` owns loaded sectors in the port | none |
| Panes blit into a CPU backbuffer | Pane `Compose`/`Draw` through `TRenderer` | GPU compositor | none (Classic pixel-identical) |
| Handlers with 4 raw args, hand-rolled token parsing | `SCommandContext` + `TCommandArgs` | one parsing vocabulary | none |
| 3,700-line `command.cpp` | per-family handler files, one table | maintainability | none |

## 8. Open questions (author)

1. Death **Restart** after a loaded game: retail appears to fall through
   to a new game (start name already cleared). Keep that, or reload the
   last save?
2. Title screen music: none in `TLogoScreen::Initialize` — correct?

## 9. Implementation order

1. Screen system: `TPane::Compose/Draw`, screen HUD layer, modal stack,
   screen events, input routing; `TLogoScreen`, `TLoadScreen`,
   `TDeathScreen` on it, each with a `--test` host and filmstrip
   verification against the specs.
2. `TGameFlow` + `TGameSession`: boot (intro → title), start modes,
   staged load, death, end of game, quit; world lifecycle moved out of
   `TPlayScreen` / `InitGlobals`. Retail-format `newgame.sav` loading
   (savegame).
3. Command system: descriptors + handler families + `TCommandArgs` +
   resolver + evaluator + `TConsole`; then retail handler ports in
   order of the opening scene, then by script usage. Adjacent: the
   `TScript` wait machine and triggers, dialog.
4. HUD rebuild on the screen system, panel by panel (each verified at
   640×480 and Revisited sizes), then retire the harness-based HUD.

Each step lands on `feature/gameflow` with clean builds and its own
verification; `main` changes only when the user merges.
