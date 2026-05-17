# Gameflow plan

Phased plan for the gameflow track. Companion to [README.md](README.md)
(scope, current state) and [BURNDOWN.md](BURNDOWN.md) (concrete
checklist).

The plan is organized into phases by **dependency**, not by listed
order. Audio (T0) and the scripting/dialog runtime spine (T8, T9, plus
T5 save persistence) underpin almost everything else, so they land
early. Front-end screens (T1, T2, T6, T12) come once the runtime they
front for actually exists.

## Phase G1 — Foundations (no UI yet)

The goal of G1 is "the engine can already do everything, even though
you can't see it from a menu." Boot path stays on `--loadmap` /
`--test=...`; we don't break the path that already works.

### G1.1 — Audio backend (T0)

Replace DirectSound + MCI stubs with a working mixer. The user has
already vendored `sokol`; we add `miniaudio` (single header, BSD-style
license, native CoreAudio backend on Darwin) into `thirdparty/` and
write a thin facade behind the existing `TSound` / `TSoundPlayer`
API so call sites don't churn.

Sub-steps:
- Vendor `miniaudio.h` into `thirdparty/miniaudio/`. CMake target.
- New `src/audio_backend.{h,cpp}` — owns the `ma_engine`, mixer config,
  master volume, and the worker thread (one — per [feedback-threading]
  it goes through our worker abstraction, not raw `std::thread`).
- Re-light `sound.cpp`: replace each `#if 0` block with a call into
  the backend. Preserve `TSound::Load / Play / Stop` shape; preserve
  the existing wave-resource path.
- Music loader: ogg/vorbis via `stb_vorbis` (already in
  `thirdparty/stb/`? — verify, otherwise vendor); honors
  `data/Music/*.ogg`. Replaces the CD-audio MCI path; `CDPlayTrack`
  becomes a thin shim that maps track numbers to ogg filenames.
- 3D voices: spatial source per voice, listener follows player; gated
  behind a `[Audio] Spatial=on` setting so Classic mode can stay
  flat.
- Test mode: `--test=audio` plays a sample and a music track for
  ear-verification.

Exit criterion: gameplay sounds play during `--test=sector`; a music
track can be started/stopped; gunshots / ambient loops position
themselves correctly relative to the player.

### G1.2 — Save/load runtime (T5)

`TSaveGame` is already pre-release code that mostly works. G1.2 is
about *verifying* it against the modern engine and shoring up the
edges.

Sub-steps:
- Audit `TSaveGame::WriteGame / ReadGame` against current
  `TObjectInstance::SaveObject / LoadObject`. Confirm version IDs are
  consistent with `docs/SAVE_GAME.md`.
- Confirm `LoadCurMap / SaveCurMap` flow works with the modern
  `TModuleManager` + the Revisited overlay system (per
  [reference-revisited-overlay-system] in memory: per-save curmap
  must coexist with the overlay; the overlay never gets written).
- Round-trip test: spawn default player → walk → save → restart →
  load → verify position, inventory, time-of-day, and at least one
  modified sector survive.
- Wire script state preservation: `TScript::ip` is a `char*` into
  the prototype's text — we need to confirm save/restore writes the
  *offset*, not the raw pointer. (Per docs/SCRIPTING.md it does, but
  verify in code.)
- Light a `--test=savecycle` mode that does the round-trip above
  headlessly.

Exit criterion: save → quit → relaunch → load returns the player to
the same state, with NPC scripts resuming mid-`wait`.

### G1.3 — Scripting engine integration (T8)

`TScript / TScriptManager` is intact, but it has to actually be
*called* from the modern frame loop. Currently `TPlayScreen::Update`
doesn't pump scripts.

Sub-steps:
- Confirm `TScriptManager::ContinueAllScripts()` compiles + links
  against the current engine; fix any stale type references.
- Hook it into `TPlayScreen::Update` between tick and draw (per
  [project-tick-draw-separation] in memory).
- Trigger plumbing:
  - TRIGGER_ALWAYS — already runs from ContinueAllScripts.
  - TRIGGER_DIALOG — fired from the player's click handler when the
    target is a character; routes through TDialogPane (G2.3).
  - TRIGGER_PROXIMITY — pulse-based, per-frame distance check on
    proximity-tagged scripts; cheap.
  - TRIGGER_CUBE — entry detection per Update.
  - TRIGGER_ACTIVATE / USE / GIVE / GET — fired from the
    inventory/use UI path (coord with ui track).
  - TRIGGER_COMBAT / DEAD — fired from character state changes
    (coord with core gameplay track).
- Console: bring up a minimal in-game `~`-console (ImGui panel) that
  routes lines into `CommandInterpreter`. *Not* a separate thread —
  the 1998 threaded console is a luxury; mainline + queue is fine.
- Test mode: `--test=script` spawns an empty sector with one NPC
  running a "patrol-and-say" script.

Exit criterion: scripts pump every frame; at least one triggered
script (proximity or dialog) fires from in-game; the console can
execute a `say` command.

### G1.4 — Dialog engine (T9)

`TDialogPane` is intact and modeled as a TPane. It needs:
- Linking into the screen stack so `TPlayScreen` can show it.
- The `say` script command routing through it (currently writes to
  the old text bar — verify and re-route).
- `wait response` semantics: a script blocks on the pane returning a
  choice; pane unblocks the script via the script manager.
- Localization hookup: dialog lines come through `TDialogList` /
  `LANGUAGE.DEF` (per [project-localization]). Confirm the loader
  works with the modern font/atlas path.

Sub-steps:
- Verify `TDialogList::Initialize` finds `ENGLISH.DEF` under the
  active module.
- Wire `say "tag"` → `TDialogList::GetLine(tag)` → `TDialogPane` push.
- Implement choice buttons rendering against the modern UI layout
  system (coord with ui track on style).
- Test mode: `--test=dialog` shows the merchant example from
  docs/SCRIPTING.md.

Exit criterion: clicking an NPC in `--test=script` brings up a
TDialogPane with options; selecting one returns control to the
script and triggers the right branch.

## Phase G2 — Walking through a game

Now that the runtime exists, plumb the *experience* of being in a
session: doors that open, returning to Misthaven, a log that records
the conversation, dying and respawning.

### G2.1 — Exits, doors, Misthaven persistence (T10)

`TExit` (`src/exit.cpp`) is the core. The pre-release implementation
already has:
- `SExitRef` linked list of exit targets per level.
- Open/close state machine (`EXIT_CLOSED / OPEN / CLOSING / OPENING`).
- Position transfer between exit targets (`mapindex` → target exit).

What's needed:
- Level transition glue: when the player crosses an active exit, the
  module's level-load path runs, the new sector streams in, and the
  player object is re-parented at the target exit's mapindex.
- "Return to Misthaven, restore position": every time the player
  leaves the Misthaven hub, the curmap is saved; on return, the
  curmap is restored. This is already conceptually in the
  `BASEMAPDIR`/`CURMAPDIR` design (see docs/SAVE_GAME.md) — verify
  it's actually running the curmap path on transitions, not just on
  full saves.
- Lock/key + script-gated exits: TExit derives from TContainer for
  lock state; respect that. Script `TRIGGER_USE` lets keys open them.
- Test mode: `--test=exits` two sectors connected by a door; walk
  through, walk back, verify state survives.

Exit criterion: walking through a door in `--test=exits` transports
the player to the linked sector; walking back restores the original
sector's changed state.

### G2.2 — Death and restart (T7)

`TDeathPane` exists as a button pane with background art. The
mechanic:
- When the player object goes to 0 HP and finishes its death
  animation, push TDeathPane onto the screen stack.
- The pane offers Restart-Last-Save / Load-Game / Main-Menu / Quit.
- Restart-Last-Save calls `TSaveGame::ReadGame(last)`.
- If there is no save, fall back to NewGame (G2.4 below).

Sub-steps:
- Hook into `TPlayer::Die` or `TCharacter::OnDeath` (whatever the
  modern equivalent is) to push the pane after the animation.
- Make sure the world keeps rendering behind the pane (the death
  background should ride over the live scene), unless the original
  art was full-screen — confirm against `data/` art.
- Verify `TPlayScreen::LoadGame(0)` is the right "restart" hook.

Exit criterion: kill the player in `--test=sector`; the death pane
shows; clicking restart respawns with the last save.

### G2.3 — Game log panel (T11)

A scrollable log of dialog lines, system messages, and important
events. New code — there isn't an existing 1998 class for this in
the snapshot. Implementation:
- `TGameLogPane : TPane`, retained-mode (per
  [feedback-ui-retained-mode]).
- Backed by a ring buffer of `SLogEntry { timestamp, category,
  text, color }`.
- A `log_to_player(category, fmt, ...)` API that the dialog system,
  inventory pickup, combat damage messages, etc. all push into.
- A keybinding (default `L`) to open/close it; auto-pop when a new
  important message arrives (config option).
- Style: shares the UI track's TPane/UIStyle infra; no novel widget.
- Test mode: `--test=log` pumps a synthetic stream into it.

Exit criterion: dialog lines from G1.4 also appear in the log; the
log persists across the session (and survives save/load).

## Phase G3 — Front-end

With the runtime working, build the screens the player actually
sees on launch.

### G3.1 — Main menu (T1)

`TMainMenuScreen : TScreen` — the first thing the engine pushes
after Initialize() once we stop auto-jumping into PlayScreen.
- New Game → G3.3.
- Load Game → push TLoadGamePane → restore selected slot →
  swap to TPlayScreen.
- Save Game → only enabled mid-session (re-show from in-game ESC).
- Settings → push TSettingsScreen (G3.2).
- Credits → push TCreditsScreen (G3.4).
- Quit → exit.
- Background: title art + ambient music loop (uses T0 music
  player).
- Test mode: `--test=mainmenu` opens it standalone.

Recon: the retail binary likely has a TMenuScreen or TMainScreen.
Spawn a recon agent to confirm field layout before designing the
class shape (mislabel cost asymmetry per
[feedback-recon-agent-protocol] — better to ship a clean modern
class than a wrongly-labelled "port" of something else).

### G3.2 — Settings screen (T2)

Existing `SRevisitedSettings` (per
[project-revisited-settings]) is the source of truth for runtime
state. Settings screen is a UI surface over the same struct + the
existing INI round-trip.

- Tabbed layout: Display / Audio / Controls / Game.
- Display: resolution mode (Classic 640×480 vs Revisited 1920×1080,
  per [project-resolution-modes]), windowed, vsync.
- Audio: master / sfx / music volume, 3D audio on/off.
- Controls: keybinding viewer (rebinding deferred unless trivial).
- Game: difficulty, autosave on/off, log behavior.
- "Apply" calls the existing Apply path; "Save Settings" persists.
- Coord with ui track on widget style.

### G3.3 — New Game + starting scripts (T3 + T4)

Sub-flow on the New Game button:
- (Single difficulty selector if we want it — otherwise hard-coded.)
- Reset `curmap` to the base map; clear save state.
- Spawn the default player object at the module's start location
  (read from `module.def` / `area.def` — see
  [project-revisited-area-def]).
- Fire the module's starting script (the one with
  `TRIGGER_ALWAYS` on the start sector's "narrator" object, or
  whatever the module convention turns out to be).
- Swap to TPlayScreen.

Recon: confirm how the retail binary identifies the start location
(probably an `AREA.DEF` keyword like `START` or a flag on a tagged
TObject). Spawn a recon agent.

### G3.4 — End-of-game + credits (T12)

- Trigger: a script command `endgame "credits_file"` (new command
  in the script interpreter; one line) that swaps to
  TCreditsScreen.
- Credits screen: scrolling text loaded from
  `data/Modules/<active>/credits.txt`, music loop via T0, click /
  any-key returns to main menu.
- Save the highest gametime to a "completed" flag on the user
  profile so the main menu can show "New Game+" or similar later.

## Phase G4 — Polish

Once G1–G3 are landed and we have a playable loop from main menu →
session → death → respawn → quit → relaunch → resume → end → credits,
the polish pass:

- Autosave on level transitions / before bosses.
- Quick-save / quick-load keybinds.
- Settings UI for new-game difficulty.
- Better log-pane filtering.
- Audio: ducking music under dialog, optional reverb per sector.
- Localization sweep — confirm all gameflow surfaces honor
  `TDialogList` and not hard-coded English.

This phase is open-ended; tickets land into BURNDOWN as discovered.

## Cross-cutting conventions

- Code style: per project memory (T-prefix classes, no new macros,
  prefer modern C++ in touched files, minimal includes,
  const-correctness, default member initializers, no raw threads).
- Renderer / HUD: this track does NOT touch the render pipeline. UI
  panes go through the existing TPane stack and the layout system
  the ui track is building.
- Recon: when porting a 1998 class we have a Ghidra mapping for,
  follow [reference-revsync-convention] — recon/classes/ is SoT,
  `// REVSYNC:` header in the file.
- Test modes for every subsystem (per
  [feedback-test-modes-for-ui]) — see the `--test=` suffix at the
  end of each section above.
- Modernization stance: evolve existing classes (`TScript`,
  `TDialogPane`, `TExit`) rather than rewriting parallel modules
  (per [feedback-evolve-dont-replace]).
- No bodges (per [feedback-no-bodges]); fix structural issues
  before they spread (per [feedback-bite-the-bullet]); pull back to
  one-site-at-a-time when blast radius grows (per
  [feedback-small-steps]).

## Risks

- **Audio data compatibility.** The 1998 pre-release sound formats
  (WAVE + MCI cdaudio cue list) may not match what shipped on the
  GOG release. Expect to handle both via the resource pack.
- **Script ⇄ saved-state coupling.** If `TScript::ip` saves as a
  raw pointer offset, a script-text edit between save and load
  corrupts the resume point. May need a stable label-based ip.
- **Misthaven sector volume.** The curmap diff for Misthaven hub
  could be huge after a long session. May need incremental diff
  serialization rather than full sector copies.
- **Recon ambiguity for menus/credits.** No menu-class hint in our
  pre-release `src/`. Recon may not produce a confident mapping;
  in that case we ship clean modern classes and don't pretend
  they're "ports."
- **Dialog UX vs. localization width.** Per
  [project-localization], translated lines blow the original
  layout's bounds. Dialog choice buttons need to be flow-laid-out,
  not pixel-positioned.
