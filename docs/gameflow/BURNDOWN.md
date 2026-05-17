# Gameflow burndown

The live checklist. Update this as work lands. If this chat is lost,
a new agent starting from [README.md](README.md) + [PLAN.md](PLAN.md)
should be able to read this file, run `git log --oneline main..HEAD`,
and pick up where the previous one left off.

Legend: `[ ]` open · `[~]` in progress · `[x]` done · `[!]` blocked
(reason inline) · `[-]` cancelled.

Date format: ISO (YYYY-MM-DD). The "Last touched" column is set when
a status changes.

---

## T0 — Audio system  (Phase G1.1)

| Status | Item | Last touched |
|--------|------|--------------|
| [ ] | Vendor `miniaudio.h` into `thirdparty/miniaudio/` + CMake target | — |
| [ ] | Verify `stb_vorbis` available (vendor if not) | — |
| [ ] | `src/audio_backend.{h,cpp}` — owns `ma_engine`, master volume, worker plumbing | — |
| [ ] | Re-light `sound.cpp` `#if 0` blocks: load/play/stop core | — |
| [ ] | Re-light wave loader (file path + resource-pack path) | — |
| [ ] | Re-light volume / pan / frequency setters | — |
| [ ] | Re-light primary buffer / 3D listener path | — |
| [ ] | CD-audio MCI shim → `data/Music/*.ogg` mapping | — |
| [ ] | 3D voices — spatial source per `TSound::Play3D`, listener follows player | — |
| [ ] | Settings `[Audio]` section: master / sfx / music / spatial | — |
| [ ] | `--test=audio` mode (one SFX + one music track) | — |
| [ ] | Smoke-test inside `--test=sector`: footsteps, ambient, sword swings | — |

**Exit:** SFX play in `--test=sector`; music can be started/stopped; spatial voices position correctly.

---

## T1 — Main menu  (Phase G3.1)

| Status | Item | Last touched |
|--------|------|--------------|
| [ ] | Recon: spawn agent to identify retail menu class (TMenuScreen? TMainScreen?) | — |
| [ ] | `src/mainmenuscreen.{h,cpp}` — TScreen with buttons New/Load/Settings/Credits/Quit | — |
| [ ] | Title art loader + background draw | — |
| [ ] | Ambient music loop (depends on T0) | — |
| [ ] | Boot path change in `revmain.cpp`: push TMainMenuScreen instead of straight to PlayScreen | — |
| [ ] | ESC from PlayScreen → in-game menu (subset: Save/Load/Settings/Main Menu/Quit) | — |
| [ ] | `--test=mainmenu` standalone mode | — |

**Exit:** App launches into a working main menu; ESC pulls it up mid-session.

---

## T2 — Settings screen  (Phase G3.2)

| Status | Item | Last touched |
|--------|------|--------------|
| [ ] | `src/settingsscreen.{h,cpp}` — tabbed pane (Display/Audio/Controls/Game) | — |
| [ ] | Display tab — resolution mode, windowed, vsync | — |
| [ ] | Audio tab — master/sfx/music/spatial (depends on T0) | — |
| [ ] | Controls tab — keybinding viewer (rebinding deferred) | — |
| [ ] | Game tab — difficulty, autosave, log behavior | — |
| [ ] | Apply / Save Settings buttons (round-trip through existing INI) | — |
| [ ] | Coord with ui track on widget styling | — |

**Exit:** Settings opens from main menu and in-game; changes persist; existing SRevisitedSettings is the SoT.

---

## T3 — New Game  (Phase G3.3)

| Status | Item | Last touched |
|--------|------|--------------|
| [ ] | `TPlayScreen::NewGame()` already exists — audit it; expand if it just calls `LoadGame(0)` | — |
| [ ] | curmap clear on NewGame; reset save state | — |
| [ ] | Spawn default player at start location (depends on T4) | — |
| [ ] | Hook from TMainMenuScreen NEW button | — |
| [ ] | Optional difficulty selector | — |

**Exit:** New Game from menu lands a fresh player in the starting sector.

---

## T4 — Starting scripts / location  (Phase G3.3)

| Status | Item | Last touched |
|--------|------|--------------|
| [ ] | Recon: identify how retail marks the start location (AREA.DEF START keyword? tagged TObject?) | — |
| [ ] | Read the start position from module data, not hard-coded | — |
| [ ] | Fire the starting TRIGGER_ALWAYS script in the start sector | — |
| [ ] | Verify with the GOG Misthaven module data | — |

**Exit:** New Game uses module data for the start position and fires the opening script.

---

## T5 — Save game / state capture  (Phase G1.2)

| Status | Item | Last touched |
|--------|------|--------------|
| [ ] | Audit `TSaveGame::WriteGame/ReadGame` against current `TObjectInstance::Save/Load` | — |
| [ ] | Confirm version IDs match `docs/SAVE_GAME.md` | — |
| [ ] | Confirm `LoadCurMap/SaveCurMap` work with modern TModuleManager + Revisited overlay | — |
| [ ] | Confirm `TScript::ip` saves as offset, not raw pointer | — |
| [ ] | Round-trip test: spawn → walk → save → restart → load → verify | — |
| [ ] | `--test=savecycle` headless round-trip | — |

**Exit:** Save/load round-trips player + script state cleanly.

---

## T6 — Load / save UI  (Phase G3.1 / G3.3)

| Status | Item | Last touched |
|--------|------|--------------|
| [ ] | `TSaveLoadPane` — list of slots with thumbnail / timestamp / location label | — |
| [ ] | Slot scanner — enumerate `gameXX.sav` + extract header metadata | — |
| [ ] | Save flow from in-game menu (slot picker + overwrite confirm) | — |
| [ ] | Load flow from main menu and from death pane | — |
| [ ] | Delete-slot action with confirm | — |

**Exit:** Slot UI works from main menu, in-game menu, and death pane.

---

## T7 — Death and restart  (Phase G2.2)

| Status | Item | Last touched |
|--------|------|--------------|
| [ ] | Hook `TCharacter`/`TPlayer` death to push `TDeathPane` after death animation | — |
| [ ] | Wire pane buttons: Restart Last Save / Load / Main Menu / Quit | — |
| [ ] | Restart Last Save → `TSaveGame::ReadGame(last)` | — |
| [ ] | Fallback to NewGame if no save exists | — |
| [ ] | Verify TDeathPane art renders correctly over live scene | — |
| [ ] | `--test=death` mode (spawn player + kill on a key) | — |

**Exit:** Player dies → death pane shows → restart/load resumes.

---

## T8 — Scripting engine  (Phase G1.3)

| Status | Item | Last touched |
|--------|------|--------------|
| [ ] | Confirm `TScriptManager` compiles + links in current build | — |
| [ ] | Pump `ContinueAllScripts()` from `TPlayScreen::Update` | — |
| [ ] | TRIGGER_ALWAYS verified live | — |
| [ ] | TRIGGER_DIALOG — click-on-character handler (depends on T9) | — |
| [ ] | TRIGGER_PROXIMITY — per-frame distance check | — |
| [ ] | TRIGGER_CUBE — entry detection | — |
| [ ] | TRIGGER_ACTIVATE / USE / GIVE / GET — coord with ui inventory | — |
| [ ] | TRIGGER_COMBAT / DEAD — coord with core combat | — |
| [ ] | Mainline ImGui console panel (replaces threaded TConsolePane) | — |
| [ ] | Retire/refactor the 3 `#if 0` MSVC-debug-heap blocks in script.cpp | — |
| [ ] | `--test=script` patrol-and-say NPC sample | — |

**Exit:** Scripts pump every frame; triggered scripts fire from in-game; console executes commands.

---

## T9 — Dialog engine  (Phase G1.4)

| Status | Item | Last touched |
|--------|------|--------------|
| [ ] | Verify `TDialogList::Initialize` finds `ENGLISH.DEF` under active module | — |
| [ ] | `say "tag"` script command → `TDialogList::GetLine` → push to `TDialogPane` | — |
| [ ] | `wait response` script command — block + unblock via script manager | — |
| [ ] | Choice button layout via UI layout system (flow, no pixel positioning) | — |
| [ ] | Style coord with ui track | — |
| [ ] | `--test=dialog` merchant example from docs/SCRIPTING.md | — |

**Exit:** Clicking an NPC brings up dialog; choice routes back to script branch.

---

## T10 — Exits / doors / Misthaven persistence  (Phase G2.1)

| Status | Item | Last touched |
|--------|------|--------------|
| [ ] | Audit `TExit` against current engine — compiles + links | — |
| [ ] | Level transition glue: cross active exit → load target level → re-parent player at target mapindex | — |
| [ ] | Verify curmap save runs on every transition, not just full save | — |
| [ ] | Verify curmap restore runs on return to Misthaven | — |
| [ ] | Lock/key + TRIGGER_USE script gating on exits | — |
| [ ] | `--test=exits` two sectors connected by a door | — |

**Exit:** Walking through a door swaps sectors; walking back restores the changed state.

---

## T11 — Game log panel  (Phase G2.3)

| Status | Item | Last touched |
|--------|------|--------------|
| [ ] | `src/gamelogpane.{h,cpp}` — retained-mode TPane | — |
| [ ] | `SLogEntry` ring buffer | — |
| [ ] | `log_to_player(category, fmt, ...)` API | — |
| [ ] | Wire dialog system to push entries (depends on T9) | — |
| [ ] | Wire inventory pickup + combat damage messages (coord with ui + core) | — |
| [ ] | Keybinding (default `L`) | — |
| [ ] | Save/load round-trips the log | — |
| [ ] | `--test=log` synthetic stream | — |

**Exit:** Dialog lines and game events appear in a readable, scrollable log that survives save/load.

---

## T12 — End of game + credits  (Phase G3.4)

| Status | Item | Last touched |
|--------|------|--------------|
| [ ] | Add `endgame "<credits_file>"` script command | — |
| [ ] | `src/creditsscreen.{h,cpp}` — scrolling text | — |
| [ ] | Credits content loader: `data/Modules/<active>/credits.txt` | — |
| [ ] | Music loop via T0 | — |
| [ ] | Click/key returns to main menu | — |
| [ ] | Persist a "completed" flag for future "New Game+" affordances | — |

**Exit:** Calling the endgame command rolls credits; returns to main menu cleanly.

---

## Cross-track dependencies

These items aren't part of any single track but block others:

- [ ] Worker abstraction (per [feedback-threading]) — already exists in
      the renderer track? Audio thread should ride it. *Owner: confirm with main / vfx.*
- [ ] UI style asset (TPane/UIStyle) usable from gameflow panes —
      track ui owns this; gameflow consumes.
- [ ] Character death hook — track core gameplay owns the death
      animation finish event; gameflow consumes for T7.
- [ ] Inventory use/give/get hooks — track ui owns these; gameflow
      consumes for T8 triggers.

---

## Completed

(Nothing yet beyond plan docs. Each commit to `feature/gameflow` that
lands a checklist item should move that item here with the date and
short link to the commit short-hash.)
