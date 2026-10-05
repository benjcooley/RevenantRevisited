# Game flow — forensics

How retail Revenant moves from launch through title, new/loaded game,
play, death and end of game, and what owns the game world's lifetime.
Input to [../ARCHITECTURE.md](../ARCHITECTURE.md). Companion to
[SCREEN_SYSTEM.md](SCREEN_SYSTEM.md) (the screen/modal mechanics) and
[../../../recon/discovered/save_system_notes.md](../../../recon/discovered/save_system_notes.md)
(save/load orchestration).

Retail fidelity: **retail-confirmed** unless marked.

## 1. Requirements (manual)

- Launch → intro movie → main menu (New Game / Load Game / Multiplayer /
  Options / Exit).
- New Game starts the single-player game; Load Game resumes a save.
- ESC in game → in-game menu (Load / Save / Options / Quit Module);
  Quit Module returns to the main menu.
- Death → death screen (Restart / Load / Exit).
- The story ends with movies and credits, then back to the main menu.
- `P` pauses. Alt+Tab / closing asks "exit game?".
- Command line: `QUICKSTART` (skip intro + menu) and `EDITOR`
  (see `data/cmdline.txt` for the full retail list).

## 2. Retail flow

### 2.1 Boot — WinMain `0x004865a0`

After engine init (`FUN_00485870`), in order:
1. Multiplayer lobby launch → MP screen.
2. `QUICKSTART` (`GetParameters` `0x00483bc0` sets `DAT_00668184`;
   `QUICKSTART="name"` also sets `DAT_0066603c`) →
   `PlayScreen.SetStartMode(0)` (new game) or `SetStartMode(1, name)`
   (load) → PlayScreen.
3. `EDITOR` → `SetStartMode(2)` → PlayScreen.
4. Otherwise play `<MoviePath>\Mix_FMV1.smk` (`MoviePath` INI key,
   default `.\Resources\FMV`; shipped ini: `.\Disk2`) → `TLogoScreen`.

Then `for (s = boot; s; s = ShowScreen(s, 0))`.

### 2.2 Title screen — TLogoScreen

`Initialize` `0x0053a2c0`: clears `PlayerManager`, loads `menus.dat`,
cursor, a `TButtonPane` with five `TButton`s built from
`menus.dat` sprites; callbacks at `0x0053a1f0..0x0053a2a0`:

| Button | Action |
|---|---|
| New Game | `PlayScreen.SetStartMode(0,-1,-1,nullptr)`; next = PlayScreen |
| Load Game | next = load screen (`0x0066fa78`), `loadFromGame = 0` |
| Multi | next = MP screen |
| Options | next = options screen (`0x0066fe88`) |
| Exit | next = none (quit) |

### 2.3 Starting a game — TPlayScreen

`SetStartMode(mode, module, game, name)` `0x0047f4c0` stores
`+0x6d8..+0x6e4` (mode 4 ignored). `Initialize` `0x0047a660`:
1. Loading bar (`loadbar.dat` or `<module>\loadscreen.bmp`).
2. Mount the start module (`module == -1` → main module), via
   `0x004609f0`.
3. Load HUD resources (`EquipPane.dat`, `SpellPane.dat`, …,
   `StatusBar.dat`/`NoTex`, `Dialog.dat`, …), advancing the bar.
4. `TScriptManager::Initialize` (master.s + state.def),
   `TAreaMgr::Initialize`.
5. Initialize panes: map pane, side pane, bottom pane, side tabs,
   automap, inventory, quick spell, bar-inv, text bar, dialog, bottom
   bar, equip, spell, stat, spellbook, player status bar; add the
   top-level ones to the screen.
6. Cursors; `SetFade(1)`.
7. Start mode: **0** `MapPane.ClearCurMap()` + `LoadNewGame()`
   (= `LoadGame("newgame", 1)` → `<module>\newgame.sav`);
   **1** resolve name → index, `ClearCurMap`, `LoadGame(index)`, on
   failure show `GAMENOTFOUND` in the text bar and `LoadNewGame()`;
   **2** editor; **3** MP.
8. Preload effect imagery (blood, sparks, impact); MP character select
   if needed.

`Close` `0x0047b290`: `PlayerManager.Clear`, close every pane,
`TAreaMgr::Close`, free HUD resources, `TScriptManager::Close`, stop
sounds. **The game world (players, map, scripts, areas, game state)
lives exactly as long as the PlayScreen.**

Loading and saving mid-session go through PlayScreen's deferred
requests (`0x0047e770`: `+0x5e4` load pending, `+0x5ec` game index,
`+0x5f0` name), processed in its pulse.

### 2.4 In the game

- In-game menu `0x0047e500` (see [INGAME_MENU.md](INGAME_MENU.md)):
  ESC (hard-wired in PlayScreen's KeyPress, not a control) writes the
  thumbnail and opens it; Load / Save / Options run as modal panes over
  the screen, which stands still under them in single player; Quit
  Module (after `quitgameyn`) → next = title; Exit (after `exitgameyn`) →
  quit the app. Save does nothing while control is off (conversations,
  cutscenes).
- `control on/off` (`0x0047c580`): off releases the held movement keys
  (arrows, Home/End/PgUp/PgDn, `R`) and sets the global control-off
  flag; on clears it.
- Pause (`P` / script `toggle pause`) lives on PlayScreen; the
  script-driven screen fade is PlayScreen's fader, a TScreen facility
  ([SCREEN_SYSTEM.md](SCREEN_SYSTEM.md) §2.6).

### 2.5 Leaving the game

- **Death** — `TPlayer::Animate` `0x00518aa0`: a 192-frame countdown
  restarts every frame the player is alive; when health < 1 and it runs
  out, PlayScreen's next = death screen and PlayScreen closes. Death
  pane buttons: Restart → PlayScreen (previous start mode), Load → load
  screen, Exit → title.
- **End of game** — the final script plays `Mix_Fmv3English.smk` and
  `Mix_Credits.smk` with `playmovie`, then `endgame` (`0x00427060`):
  PlayScreen's next = title, close.
- **Quit Module** — next = title. **Exit** — `PostQuitMessage`.
- Script `newgame` (`0x00423760`) → `0x0047e770(0)`: a deferred load
  request on the running PlayScreen.

## 3. Port state (2026-10-04)

- `AppInit` routes straight to `TPlayScreen` (or a `--test` screen);
  there is no intro, title, death screen, load/options screen or
  in-game menu in production. `--loadmap=<save>` jumps into a save.
- `TPlayScreen::Initialize` spawns a stand-in Locke at a fixed forest
  sector with a hand-rolled starter loadout; `newgame.sav` is never
  read (the pre-release `ReadGame` can't parse the retail format).
- `ScriptManager.Initialize()` runs at app boot (InitGlobals), not per
  game as in retail.
- `TCinematicScreen` plays SMK files as a standalone screen.
- `IRuntimeMode` toggles game vs editor *inside* PlayScreen.

## 4. Divergences the port has to make

1. **World lifetime.** Agreed design: a game session owns the world
   (map, players, scripts, game state, time) instead of PlayScreen, so
   overlays, cinematics, death and loading don't need to tear down and
   rebuild the screen to manage it. Player-visible sequence unchanged.
2. **Loading.** Retail initializes synchronously inside one frame while
   repainting the loading bar by hand. The port must stage loading
   across frames (or worker jobs per the threading rule) so the bar
   animates without re-entering the frame loop.
3. **Modal sequences** (in-game menu → load → back) become
   continuation-driven (see [SCREEN_SYSTEM.md](SCREEN_SYSTEM.md) §4).

## 5. Open questions for the author

- Death screen **Restart**: after a loaded game, retail's start name is
  already cleared, so Restart appears to fall through to a new game.
  Was Restart meant to reload the last save?
- Does the title screen play music (the decomp shows no CD track call in
  `TLogoScreen::Initialize`)?
