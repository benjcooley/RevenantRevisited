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
| [x] | Vendor `miniaudio.h` v0.11.22 into `thirdparty/miniaudio/` + CMake include | 2026-05-16 |
| [x] | Vendor `stb_vorbis.c` v1.22 into `thirdparty/stb/` | 2026-05-16 |
| [x] | `src/audio_backend.{h,cpp}` — owns `ma_engine`, sfx/music sound groups, master/sfx/music volume, listener pos, ref-counted shared PCM, one-shot helper | 2026-05-16 |
| [x] | Re-light `sound.cpp` load/play/stop core (TSound::Load/Play/Stop/IsPlaying/IsLooping/Duplicate via backend) | 2026-05-16 |
| [x] | Re-light wave loader (in-tree PCM RIFF/WAVE reader; ACM-decode path retired) | 2026-05-16 |
| [x] | Re-light volume / pan / frequency setters (DirectSound dB + pan units preserved; backend converts to linear) | 2026-05-16 |
| [x] | Re-light TSoundPlayer Mount/Unmount/Play/Stop + Pause/Unpause/SetVolume + dir scan + Init/Close | 2026-05-16 |
| [x] | Drop CD* C-function shim; rewrite `TArea::Init/Close/PlayCDMusic` directly on `audio::MusicPlayFile`; resolves to `<install>/MUSIC/TrackNN.ogg` | 2026-05-16 |
| [x] | Wire `SoundPlayer.Initialize()` as step 22 of `InitGlobals`; `SoundPlayer.Close()` as first step of `ShutdownGlobals` inverse | 2026-05-16 |
| [x] | `--test=audio` mode (auto-fires `open.wav` one-shot + auto-starts music; ImGui panel for SFX/music/volumes) | 2026-05-16 |
| [x] | Smoke-test on macOS — miniaudio comes up on `MacBook Air Speakers`, .ogg playback confirmed audible | 2026-05-16 |
| [x] | Retire pre-port `sound.cpp` to `attic/src/sound_directsound.cpp` | 2026-05-16 |
| [ ] | 3D voices — spatial source per `TSound::Play3D`, listener follows player (currently flat-pan classic mix; spatializer disabled by design) | — |
| [ ] | Settings `[Audio]` section: master / sfx / music / spatial (G3.2 work; backend volume hooks already in place) | — |
| [x] | Surface SFX from `resources.rvr` via new `VFSListByPrefix` + route `LoadWave` through `rev_fopen` — 1057 SFX in registry on GOG install | 2026-05-16 |
| [ ] | Inside-game smoke test — `--test=sector` footsteps / ambient / sword swings (now unblocked; SFX registry populated) | — |
| [x] | Retail sound list (DIALOG.md §3.4): `.wav` + `.mp3` (MP3 decode on), resource dirs at `Initialize`, module dirs at `SetCurModule`, sorted + bsearch; `rev_find_files` replaces `VFSListByPrefix`; `SampleLengthMs` decodes, so registry and lengths work under `--headless`. 1063 resource + 1743 module sounds | 2026-10-05 |

**Exit:** SFX play in `--test=sector`; music can be started/stopped; spatial voices position correctly.

---

## T1 — Main menu  (Phase G3.1)

Design: [ARCHITECTURE.md](ARCHITECTURE.md) §2, §4; forensics: [forensics/SCREEN_SYSTEM.md](forensics/SCREEN_SYSTEM.md), [forensics/GAME_FLOW.md](forensics/GAME_FLOW.md).

| Status | Item | Last touched |
|--------|------|--------------|
| [x] | Recon: retail TLogoScreen (cls_0x5a5d18), button callbacks, WinMain boot, QUICKSTART | 2026-10-04 |
| [x] | Screen foundation: pane Compose/Draw contract, modal stack (evolved exclusive panes), pane-tree input, one scripted-input path | 2026-10-04 (c52d4f1) |
| [x] | DEF engine as a pane (`TDefPane`; `TOptionsPane`), sprite buttons + text for code-built panes | 2026-10-04 (c52d4f1, ca25da2) |
| [x] | `TGameFlow` intents; boot = intro `Mix_FMV1.smk` -> title; `--quickstart[=save]`, `--nointro`, `--menu=` | 2026-10-04 (ca25da2) |
| [x] | Production `TLogoScreen` (title, version text) — verified vs retail screenshot | 2026-10-04 (ca25da2) |
| [x] | Loading screen between the title and the game (retail bar, staged session steps) | 2026-10-05 |
| [x] | Screen fades (title, PlayScreen, scripts) | 2026-10-05 |
| [x] | Load Game / Options screens from the title: `TLoadGameScreen` / `TOptionsScreen` host the load dialog and `TOptionsPane` ([forensics/INGAME_MENU.md](forensics/INGAME_MENU.md) §8) | 2026-10-05 |
| [x] | ESC in-game menu as a modal continuation chain (retail 0x0047e500): thumbnail first, world paused under it (retail modal flags 0xf), Load / Save / Options / Quit Module (`quitgameyn`) / Exit Program (`exitgameyn`) / Resume; Save refused without control; retail Game Options / Load / Save / Quick Save controls ([forensics/INGAME_MENU.md](forensics/INGAME_MENU.md)) | 2026-10-05 |

**Exit:** App launches into a working main menu; ESC pulls it up mid-session.

---

## T2 — Settings screen  (Phase G3.2)

Retail-first: the settings screen is retail's Options pane
([forensics/OPTIONS.md](forensics/OPTIONS.md)). The tabbed
`settingsscreen` rows that were here predate that decision and are
dropped. Revisited-only settings would extend this pane (a Revisited
`options.def` in the overlay) or the `[Revisited]` section
(`SRevisitedSettings`) later; they don't go in `[Options]`, which retail
reads.

| Status | Item | Last touched |
|--------|------|--------------|
| [x] | Retail options pane (`options.def`, `TOptionsPane`) opens from the title and in game and returns; the key list and rebinding | 2026-10-05 |
| [x] | Toggles and sliders bound to the settings as retail: copies on open, the music level live, OK applies and saves `[Options]` (retail keys, `%d` / `Yes`/`No`) and `[Controls]`, Cancel drops them; rebinds held until OK; `[Options]` read at boot (`ReadOptions`, `src/gameoptions.*`) and saved at shutdown; music / effects levels on the audio groups | 2026-10-05 |
| [x] | `[Controls]` read when the control map is built (it never was, so OK overwrote the player's bindings with the port's table) | 2026-10-05 |
| [x] | Gamma on screen: the ambient offset in `TMapPane::SetAmbientLight` and OK's re-set, as retail; no display ramp, which the retail captures show has no effect (OPTIONS.md §7.11, §9; LIGHTING_FIDELITY.md §2.5, §8; question 92) | 2026-10-07 |
| [ ] | Combat reads `CombatFace` and `NoCombatResults` (gameplay track; OPTIONS.md §7.4, §7.7) | — |
| [ ] | DEF slider: a track click pages by the slider's page size, as retail (DEF engine; OPTIONS.md §8) | — |
| [ ] | The control table → retail's (`0x005d5500`: order, defaults, entries), so a missing `[Controls]` gives retail's keys | — |

**Exit:** Options opens from the title and in game; changes take effect and persist in `Revenant.ini` as retail's did, and the GOG install's INI loads unchanged.

---

## T3 — New Game  (Phase G3.3)

| Status | Item | Last touched |
|--------|------|--------------|
| [x] | New Game = retail start mode 0: `ClearCurMap` + `LoadNewGame` (module `newgame.sav`) through `TGameSession` | 2026-10-04 |
| [x] | curmap clear on NewGame (`SectorStore` working set under SavePath) | 2026-10-04 |
| [x] | Player from `newgame.sav` (Locke L1, 25/25 HP, Keep resurrection chamber); default-spawn stand-in and Level-10 floor removed | 2026-10-04 |
| [x] | Title New Game button → `TGameFlow::StartNewGame` | 2026-10-04 |
| [-] | Difficulty selector — not in retail | 2026-10-04 |

**Exit:** New Game from menu lands a fresh player in the starting sector.

---

## T4 — Starting scripts / location  (Phase G3.3)

| Status | Item | Last touched |
|--------|------|--------------|
| [x] | Recon: retail's start location is the player in the module's `newgame.sav` (forensics/SAVE_GAME.md) | 2026-10-04 |
| [x] | Read the start position from module data, not hard-coded | 2026-10-04 |
| [x] | The opening runs end to end (CUBE trigger, voiced lines, choices, gifts, Rahul, control back) — [OPENING_SEQUENCE.md](OPENING_SEQUENCE.md) | 2026-10-05 |
| [x] | Verified on the GOG Ahkuilon module data | 2026-10-05 |

**Exit:** New Game uses module data for the start position and fires the opening script.

---

## T5 — Save game / state capture  (Phase G1.2)

| Status | Item | Last touched |
|--------|------|--------------|
| [x] | Audit `TSaveGame::WriteGame/ReadGame` against current `TObjectInstance::Save/Load` | 2026-05-19 (T5_FORENSIC.md) |
| [x] | Confirm version IDs match `docs/SAVE_GAME.md` (= MAP_VERSION 15; envelope + per-class version gates symmetric) | 2026-05-19 |
| [-] | Confirm `LoadCurMap/SaveCurMap` work with modern TModuleManager + Revisited overlay — out of scope per architectural rule (map orchestration uses modern engine; see T5_FORENSIC §1.3) | 2026-05-19 |
| [x] | Script state across saves: retail saves none; `LoadGame` resets every script (`0x00496e20`). The ip is a text offset. | 2026-10-05 |
| [~] | Round-trip test: spawn → walk → save → restart → load → verify (gated on interactive F5/F9; `--savecycle-test` is the non-interactive proxy) | 2026-05-19 |
| [x] | `--savecycle-test` headless round-trip harness | 2026-05-19 |
| [x] | Fix LoadInventory to place items at saved slot (retail-faithful; closes exit-crash candidate C2) | 2026-05-19 |
| [x] | Post-load sector attach for loaded player (closes Locke-in-ground candidate H3) | 2026-05-19 |
| [x] | REVSYNC headers on SaveObject / LoadObject / Load / Save / LoadInventory / SaveInventory | 2026-05-19 |
| [x] | Harden WriteGame/ReadGame diagnostic log (Pos() canonical; sector liveness; inv count) | 2026-05-19 |

| [x] | Retail save format: header, game states, merchant table, player list; slots under `SaveGamePath/Single/<name>` with `CurMap/` (forensics/SAVE_GAME.md) | 2026-10-04 |
| [x] | Retail `LoadGame` reset sequence (curmap, scripts, states, areas, players, control) | 2026-10-04 |
| [x] | In-game requests: Quick Save (retail control, Ctrl+Backspace since 2026-10-05; was F5), F9 = dev reload of the last slot; `--savecycle-test` runs through them | 2026-10-04 |
| [x] | Save interop: every class streams retail's layout (player objversion 15), sector hashes, `ss.bmp` thumbnail (ARCHITECTURE §3.5 2f); retail-side check in dosbox-x pending ([SAVE_INTEROP_TEST.md](SAVE_INTEROP_TEST.md)) | 2026-10-05 |
| [x] | Player stats retail (maxima, RefreshStats, armor, level-up) — [../gameplay/forensics/PLAYER_STATS.md](../gameplay/forensics/PLAYER_STATS.md) | 2026-10-05 |
| [x] | Automap persistence: retail keeps the explored masks in the player record (SAVE_GAME.md §11.5), streamed with the save interop work; the per-sector automap bitmaps are a render cache | 2026-10-05 |

**Exit:** Save/load round-trips player + script state cleanly.

**Notes:** Player-object serialization is now retail-faithful and round-trips
cleanly per-field. Two known wrapper-format gaps remain (TGameState binary
stream, retail's automap 0x80 header + 0x200 walkmap) — both out of scope
for T5 (script.cpp just retail-synced; automap retail-sync is a separate
track). See `docs/gameflow/T5_FORENSIC.md` for the full audit and the
REVSYNC-QUESTIONs surfaced for the user.

---

## T6 — Load / save UI  (Phase G3.1 / G3.3)

| Status | Item | Last touched |
|--------|------|--------------|
| [x] | Load and save dialogs on `TDefPane` (`TLoadGamePane`, `TSaveGamePane`): retail slot list, thumbnail, name / module / character, name edit ([forensics/INGAME_MENU.md](forensics/INGAME_MENU.md) §5–6) | 2026-10-05 |
| [x] | Slot list: retail `RefreshSlots` (`<SaveGamePath>/Single/<name>/game.sav`, T5) | 2026-10-04 |
| [x] | Save flow from the in-game menu and the Save Game control (retail has no overwrite confirmation) | 2026-10-05 |
| [x] | Load flow from the title, the in-game menu, the Load Game control and the death screen | 2026-10-05 |
| [-] | Delete-slot action with confirm — not in retail (PopupDef_SPEC §13a.6) | 2026-10-05 |
| [x] | The in-game load's "loadingmap" progress popup over a staged load: `ProcessRequests` starts the load and the PlayScreen runs a step a tick behind a still of the world and HUD panels (captured under the pane tree), the popup's bar at retail's 80 then the sectors to 800; the camera jumps to the loaded player, control on, cursor back, the dialog ends ([forensics/INGAME_MENU.md](forensics/INGAME_MENU.md) §5.1, §9, §10) | 2026-10-05 |
| [x] | Request loads (console `loadgame`, F9, `--savecycle-test`): retail's "Loading Game %s... Please Wait" (`loadgamefmt`, else the built-in line) on the text bar (0x0047bf63), outside the editor; the load dialog passes `announce = false` (it has its popup) | 2026-10-05 |

**Exit:** Slot UI works from main menu, in-game menu, and death pane.

---

## T7 — Death and restart  (Phase G2.2)

| Status | Item | Last touched |
|--------|------|--------------|
| [x] | Retail death countdown (192 frames, TPlayer::Animate 0x00518aa0) -> `GameFlow.PlayerDied()` | 2026-10-04 (64c0f25) |
| [x] | Retail `TDeathScreen` + `TDeathPane` (Restart / Load / Exit, death voice) | 2026-10-04 (64c0f25) |
| [ ] | Restart semantics after a loaded game (author question 40) | — |
| [x] | Death "Load" -> Load Game screen (retail 0x00533970) | 2026-10-05 |
| [x] | Death voices audible (MP3 voice support in the sound player; `gosar00` stays missing, as in retail) | 2026-10-05 |

**Exit:** Player dies → death pane shows → restart/load resumes.

---

## T8 — Scripting engine  (Phase G1.3)

| Status | Item | Last touched |
|--------|------|--------------|
| [x] | Confirm `TScriptManager` compiles + links in current build | 2026-05-17 |
| [x] | Wire `ScriptManager.Initialize()` into InitGlobals (step 23) + `Close()` into inverse | 2026-05-17 (b2bd02a) |
| [x] | Confirmed runtime: master.s parses 24 protos + state.def 82 vars; forest.s adds 19 on area enter | 2026-05-17 |
| [x] | Pump driver verified — `TGameModeImpl::Tick` → `MapPane.PulseObjects` → `inst->Pulse` → `ContinueScript` → `script->Continue(this)` already wired (no new code needed) | 2026-05-17 |
| [x] | Retire/refactor the 3 `#if 0` MSVC-debug-heap blocks in script.cpp | 2026-05-17 (b2bd02a) |
| [x] | **T8.1 RETAIL-SYNC** — `src/script.{cpp,h}` synced against `recon/discovered/cls_TScript*` (~1700 lines decomp). New: TScriptManager.instances + fileowners registries, ObjectScript class-name second-pass match (now actually attaches DOOR1 etc. at boot), TGameState::STATE_INVALID = 0xfeced300, AddScript port, USE-trigger fallback to proto-self name. Continue/Triggered/End keep pre-release C++ because retail bodies hook subsystems not yet ported (dialog FSM, player combat FSM, multi-context vftable slots) — flagged `TODO(revsync)` in-source. Per-method `// REVSYNC: @ <addr>` provenance markers | 2026-05-17 (52904fe) |
| [x] | Retail engine: trigger scan and requests, waits, the evaluator and resolver, aliases, attach by name then type, reset on attach ([forensics/SCRIPT_ENGINE.md](forensics/SCRIPT_ENGINE.md)) | 2026-10-05 |
| [x] | Triggers seen live: ALWAYS, CUBE, DIALOG (TendrickT), ACTIVATE (TownTel0), USE (the door prototypes), COMBAT | 2026-10-05 |
| [-] | PROXIMITY, GIVE, GET, DEAD triggers: ported, but no shipped script declares one (module scripts and master.s) | 2026-10-05 |
| [x] | Prototype variables (DATA blocks, `setprotovariable`, readers) | 2026-10-05 |
| [x] | Commands for the opening, the doors and exits, movement (`goto*`, `face*`), `try`, `statmod`, `addat` ([forensics/COMMAND_SYSTEM.md](forensics/COMMAND_SYSTEM.md) §6) | 2026-10-05 |
| [x] | From the NPC sweep: a `jump` or a taken choice lands as deep as its label sits (retail `Jump`: blocks no longer end at an inner IF's END and skip `CONTROL ON`/`SETCDVOLUME FULL`); a line starting with quoted text runs (`"TRAINING SWORD".DELETE`); `set` steps past its value ([forensics/SCRIPT_ENGINE.md](forensics/SCRIPT_ENGINE.md) §7, [forensics/COMMAND_SYSTEM.md](forensics/COMMAND_SYSTEM.md) §4) | 2026-10-05 |
| [ ] | `lastattack` member (needs the combat track's attack result; Jong's training) | — |
| [ ] | Mainline ImGui console panel (replaces threaded TConsolePane) | — |

**Exit:** Scripts pump every frame; triggered scripts fire from in-game; console executes commands.

**Notes:** Per-object script execution is already plumbed in the engine — every `TObjectInstance::InitScript(ScriptManager.ObjectScript(this))` site attaches a script, every `Pulse()` advances it. What was missing was `ScriptManager.Initialize()` itself (zero callers in the modern boot path); that's fixed. Trigger-firing verification + per-trigger-type live tests are gated on the T8.1 retail sync — pre-release `TScript::Triggered` is 199 retail lines vs. our shorter pre-release impl, and triggers gate every observable script behavior.

---

## T9 — Dialog engine  (Phase G1.4)

| Status | Item | Last touched |
|--------|------|--------------|
| [x] | Retail `TDialogList` (base + module tables), `say`/`choice`/`wait response`, voices paced by their length ([forensics/DIALOG.md](forensics/DIALOG.md)) | 2026-10-05 |
| [x] | Retail floating dialog pane: speech boxes, portrait ring, choices by key or mouse | 2026-10-05 |
| [~] | Retail shots S6 to settle colours, positions and hover behaviour. Taken 2026-10-06 for lines, colours and the choice list ([RETAIL_CAPTURE.md](RETAIL_CAPTURE.md) §3–4): placement, ring and colours match retail; the text sits 2 px right and 3–4 px high (cross-track: the glyph walk's baseline). Hover, click and keys still to shoot | 2026-10-06 |
| [x] | The Keep's story chain plays end to end (headless, choices by key): opening; Rahul's death and Tendrick's scene (`ressexit` unlocked); DOOR1; Rand in the jail (`RandK`: looped choice, two menus, `TENDRICKSTATE = 1`); Tendrick in the throne room (`TendrickT`: four choices with loops, `Finish`, Locke to level 6); the level-6 scene (`GowE`: walks, `cast "electric bolt"`, Rand dies, fade, back to the Keep, `TENDRICKSTATE = 2`, `KeepExit` unlocked); menu saves at each step | 2026-10-05 |
| [x] | Dialog text in Windows-1252 (`’` 0x92, `è`): the TrueType path decodes CP1252 and its atlases hold the printable repertoire ([../ui/TEXT_RENDERING.md](../ui/TEXT_RENDERING.md)) | 2026-10-05 |
| [x] | NPC sweep (`tools/storytest`, [STORY_TESTING.md](STORY_TESTING.md) §7): the 32 forest and town DIALOG NPCs from `New Game1`, several key schedules and `MISTSTATE=6`; every block runs to its END except Jong1 (`lastattack`, T8) | 2026-10-05 |
| [ ] | Some speakers hold the speech wait far past their line: Kylie1 (dancing, root `walk`) ~40 s a line and ~34 s for `PIVOT 50` (others 4–10 s); the slave camp's (level 46: Shegra, Slave1, Slave2, Druhgslave2 on `say`, Druhgslave3 on `try pick`) never get back to root. Not the say's start: `Say` now starts the action as retail does (step 7, `0x004db4d0` = `ForceCommand`) and the hangs remain. The speech wait wants the speaker idle in root and not transitioning (DIALOG.md §2.5); these speakers' state machines don't get there (Kylie's dance root, the slaves' states) -- character/animation lane. Not the player's per-tick root reset fixed 2026-10-06 ([../gameplay/forensics/PLAYER_INPUT.md](../gameplay/forensics/PLAYER_INPUT.md) §4): that came from the player's movement-mode input and touched only the player. Leads: retail `TComplexObject::ForceCommand` `0x004db4d0` differs from the 1998 body the port runs (§8 there) | 2026-10-06 |

**Exit:** Clicking an NPC brings up dialog; choice routes back to script branch.

---

## T10 — Exits / doors / Misthaven persistence  (Phase G2.1)

| Status | Item | Last touched |
|--------|------|--------------|
| [x] | Retail TExit, the exit list, teleports, level changes as a session step ([forensics/EXITS.md](forensics/EXITS.md) §7) | 2026-10-05 |
| [x] | Locks and keys (`CheckKeyUse`), the door prototypes' USE scripts end to end | 2026-10-05 |
| [-] | curmap written on every transition: the port keeps visited levels loaded and writes them when saving (ARCHITECTURE §7) | 2026-10-05 |
| [x] | Walk-on of an unscripted AutoActivate exit: a level-41 teleport pad sends Locke to `Lv41Tel5`'s target | 2026-10-05 |
| [x] | The loading bar fills per sector during the world load (staged `TGameMap` load) | 2026-10-05 |
| [x] | A level's first visit loads a slice per frame under the text bar's retail loading line (`SetHealthDisplay`/`SetLevels`/`ClearHealthDisplay`), the world held | 2026-10-05 |
| [ ] | First visit to level 0 stalls ~7 s in `TMapRenderer::SetMap` after the sectors load (renderer track; the strip is already full) | 2026-10-05 |
| [x] | Name lookups search the loaded sectors as retail's do (`FindClosestObject` `0x00451de0`: the caller's level within ~2,896 units; `FindObject` `0x00451d70`: every level), not the pane's 3x3 window; Hruthford's `GOTO HRUWAY01` with the camera at Cronus now works. Pulses still walk the window; retail's pulse loop's iterator flags aren't checked yet | 2026-10-05 |

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
| [x] | `endgame` (retail `0x00427060`): back to the title | 2026-10-05 |
| [x] | `playmovie` (retail `0x00427d80`): the movie as a modal `TMoviePane` on the PlayScreen, the world held while it plays; the credits are a movie (`Mix_Credits.smk`), not a screen | 2026-10-05 |
| [-] | Credits screen / text loader: retail rolls its credits as a movie | 2026-10-05 |

**Exit:** Calling the endgame command rolls credits; returns to main menu cleanly.

---

## T13 — Shops (buy/sell)

Forensics: [../ui/forensics/BuySellScreen_SPEC.md](../ui/forensics/BuySellScreen_SPEC.md)
(the pane), [forensics/COMMAND_SYSTEM.md](forensics/COMMAND_SYSTEM.md) §6.6
(the commands, the drawer), [forensics/SAVE_GAME.md](forensics/SAVE_GAME.md)
§6 (the merchant table).

| Status | Item | Last touched |
|--------|------|--------------|
| [x] | The 14 `buysell*` commands, retail grammar (`src/cmd_buysell.cpp`, the first per-family handler file) | 2026-10-05 |
| [x] | `TBuySellPane` (retail `TBuySellScreen` `0x0065a3b8`) on `TDefPane`: `buysell.dat` chrome and buttons, the paint `0x0052f7d0` / row `0x0052f040` literals, keys, mouse rows and hover, joystick | 2026-10-05 |
| [x] | Rows: BuildItem `0x0052da90` (Value, sell × 0.3, WEAPON/ARMOR.DEF stats and STATLINE text, class stats, display names, quivers, icons), stock by name and by criteria, the customer's sellable items, removal by name and by criteria | 2026-10-05 |
| [x] | Buy / sell `0x0052ff40`: gold, empty slot, the salesperson's no-gold and purchase lines, the merchant table for unique (SaleType 1) types | 2026-10-05 |
| [x] | PlayScreen's bottom drawer, mode 3: `buysellscreen` opens it (bottom bar out, side panel open, text bar hidden), Exit closes it; `wait buysell` | 2026-10-05 |
| [x] | Other drawer closes: `LoadGame`'s reset (+ the rows), `fadescreenout`, the camera leaving the player, `hideresponse` | 2026-10-05 |
| [x] | `TScript::Jump` lands after the label name (retail `0x00493fa0`); it skipped the next line's first token, which lost every Sell shop's shop type | 2026-10-05 |
| [ ] | `getitemname` / `getitemvalue` (no shipped script uses them) | — |
| [ ] | Multiplayer branches (`0x00586a60`, `0x005869a0`, `0x00586a10`) | — |
| [ ] | Retail shots S14, S15 to check fonts, colours and which pack items a Sell shop lists | — |

**Checked** (headless, `--quickstart`, `--exec "player.pos … 1; player.addinv 1000 gold; player.use elahni1"`, input script):
Elahni (`town.s`, potions): the choices, Buy Items → the drawer opens with
10 rows; key `1` / a row click selects, `A` / the Buy button buys (gold
1000 → 750, "A wise decision my child."), ↓ pages to the Greater potions,
`E` / Exit closes and the choices come back; Sell Items → the two potions
bought, 75 gp each (250 × 0.3); selling one → gold 575. Cronus (armor):
12 rows (`minstrength` 1–16 minus `EQSLOT` 1 and 5–6), the stat line
"Protection: +1  Min Strn: +14  Min Cons: +10", no gold → "You're going to
need more gold for that.", a Red Cloth Shirt sells for 39. Hruthford (misc):
8 of his 24 names stocked (the rest are unique jewelry, SaleType 1), the
jewelry's STATLINE text ("Luck 2", "Armor Class Bonus 2"); Sell Items lists
the Pouch (30 gp), the Ivory Pendant and the Emerald Ring (EQSLOT
criteria) and not the Apple. From `--exec` with control on: `buysellremove`
drops a row, and B (Lower Panel) closes the shop. The opening still plays
to `SardokR: END`.

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
- [ ] GPU asset eviction — renderer track. The renderer keeps every imagery
      asset it has drawn until shutdown (zero-ref assets aren't evicted,
      `RecomputeLoadedMapAssetRefs`), so the image pool must hold the whole
      game's imagery; walking forest → town overflowed 4,096 and sokol
      aborted. Interim: pool 32,768 (`display.cpp`), sized from the data.
      Gameflow releases unused levels on a level change, so evicting
      assets no loaded map references is now safe.
- [ ] One source for the HUD's geometry — track ui (common-host stage).
      The PlayScreen's HUD hosting hard-codes the sidebar (188), bottom bar
      (60) and tab strip (64 × 240, hit test); the dialog entries and the
      text bar each define the side tabs as 52 (`kSideTabsWidth`), and the
      dialog the status bar's bottom (0x70). Retail derives the text bar's
      rect from the panes (`TPlayScreen` layout `0x0047bc50`).
- [ ] Text baseline in the canonical glyph walk — track ui (`font.cpp`).
      The baseline is `cellY + ` the tallest printable-ASCII glyph's rise
      `- kGdiTopLeading (2)`, a constant calibrated on the HUD's Arial
      fonts; GDI's DT_TOP puts it at the font's `tmAscent` (OS/2
      `usWinAscent` scaled). For the dialog's Times New Roman 20 that puts
      every line 3–4 px above retail and 2 px right (measured against the
      shipped game, [RETAIL_CAPTURE.md](RETAIL_CAPTURE.md) §4). The fix is
      the font's own ascent in place of the constant, with each panel
      re-checked against its retail shot.
- [~] `TPlayScreen::HideLowerPanes`/`ShowLowerPanes` are stubs with no
      callers; retail hides and shows the text bar from its drawer states
      (`0x47b874`, `0x47b8d0`, `0x47b96b`) — track ui (HUD drawers). The
      shop's drawer (mode 3) is ported (T13): it hides and shows the text
      bar, closes the bottom bar and opens the side panel. Mode 1 (the
      editor console) isn't; the HUD's bottom bar is still the harness's
      `bottomBarOpen`, and retail's other drawer closes of it (LoadGame,
      `hideresponse`) aren't ported (AUTHOR_QUESTIONS 81).
- [x] `AddToInventory(name, amount)` made a new pile (the shop's gold
      payout showed as a second gold pile). Retail's `0x0046f3d0` merges
      gold, food and potions into a pile of their kind (vtable `0x98`),
      routes a player's item into a Pouch of its kind and swaps the item in
      the target slot; ported 2026-10-05, with the nested searches
      ([forensics/INVENTORY.md](forensics/INVENTORY.md)). Open: author
      questions 120-122.

---

## Completed

- **2026-05-16 — T0 Audio backend stood up.** miniaudio 0.11.22 +
  stb_vorbis v1.22 vendored; new `audio::` facade owns the engine,
  sfx/music groups, and a ref-counted shared-PCM Source type;
  `sound.cpp` rewired end-to-end (TSound + TSoundPlayer); CD* shim
  replaced by direct `audio::MusicPlayFile` calls from `area.cpp`;
  SoundPlayer joined the InitGlobals/ShutdownGlobals chain at step
  22 / first-inverse; `--test=audio` mode added; smoke-tested on
  macOS (CoreAudio → MacBook Air Speakers, .ogg vorbis path
  confirmed audible). Pre-port `sound.cpp` retired to
  `attic/src/sound_directsound.cpp`.
