# In-game menu, load / save / options dialogs — forensics

How retail Revenant opens the ESC menu during play, what each of its
buttons runs, how the load, save and options dialogs work in the game and
from the title, and what the modal flags they run under do (whether the
world pauses). Companion to [SCREEN_SYSTEM.md](SCREEN_SYSTEM.md) (screens,
the exclusive stack, `RunModal`), [GAME_FLOW.md](GAME_FLOW.md) and
[SAVE_GAME.md](SAVE_GAME.md). The widget-level layout of each DEF screen
is in `docs/ui/forensics/{InGameMenu,LoadGame,SaveGame,Options,Popup}Def_SPEC.md`;
this document is the behaviour around them.

Retail fidelity: **retail-confirmed** (Ghidra decomp of `Revenant.exe`
1.22, 2026-10-05) unless marked *inferred*. Decomps not already under
`recon/discovered/` are listed in §12.

## 1. Requirements (manual)

- ESC in the game opens the in-game menu: Load Game, Save Game, Game
  Options, Quit Module (back to the main menu), Exit Program, Resume Game.
- Save Game: pick a slot or type a name; SAVE returns to the game, EXIT
  returns to the in-game menu. Saving isn't allowed during a conversation.
- Load Game (title, in game, death screen): a save list with a picture per
  save, LOAD / EXIT.
- Options opens the options screen.

## 2. Opening the menu

### 2.1 The key

ESC is **not a control-map entry**. It is a case in `TPlayScreen::KeyPress`
`0x0047c630` (vtable `0x5a5320` slot 12), ahead of the control map:

```
KeyPress(key, down):                                         0x0047c630
  if DAT_0065c670 return               (text-bar typing, *inferred*: slot 13
                                       sends chars to 0x0054d4a0 then)
  if (paused (+0x5d4) and key != 'P')  -> tail
  TScreen::KeyPress(key, down)         (the panes, 0x00490660)
  if !down -> tail
  if a modal is up with any flag set  -> tail
  switch key:
    0x1b (ESC):
      if the editor is off (DAT_00668154 == 0):
        if demo mode (+0x5d8) == 0:
          write .\ss.bmp from the display (scale 3, SAVE_GAME §11.7)
          InGameMenu()                                       0x0047e500
        else:
          pause samples; r = popup("exitgameyn", 3); resume samples
          if r: PostQuitMessage(0)
      else: clear +0x6bc (set by F12 after writing ss.bmp; *inferred*: a
            frozen screenshot)
    'P', F12, editor keys ...
  tail: unless the editor is on, or the top modal has flag 2:
    command = ControlMap(key, down, mode)  -> Command(command) (slot 19, 0x0047cf40)
```

So ESC opens the menu whenever no flagged modal is up, whatever the
player's control state. `+0x5d8` is demo mode (`SetDemoMode`
`0x0047c550`; `Initialize` sets it from the module's flags): there ESC
asks to quit instead.

### 2.2 Control-map commands that open the dialogs directly

The last four of retail's 69 controls (table `0x005d5500`, 0x4c bytes per
entry: name, INI name, mode mask, 3×3 key chords, command, up-command,
flags) open the dialogs without the menu:

| # | Name / INI name | Default keys (VK) | Command | `Command` case (`0x0047cf40`) |
|---|---|---|---|---|
| 65 | Game Options / `GameOpts` | `O` | `0x52` | `0x0047e700`: options pane, from game |
| 66 | Load Game / `LoadGame` | Ctrl + `0x5b` (VK_LWIN) | `0x53` | load pane, from game (`= 0x0047e660`) |
| 67 | Save Game / `SaveGame` | Ctrl + `0x5d` (VK_APPS) | `0x54` | write `ss.bmp` (`0x0047dc05`), then the save pane |
| 68 | Quick Save / `QuickSave` | Ctrl + `0x08` (Backspace) | `0x55` | write `ss.bmp` (`0x0047dd08`), then QuickSave `0x0047e850` |

A retail INI writes them as `GameOpts = O`, `LoadGame = CTRL-LWIN`,
`SaveGame = CTRL-APPS`, `QuickSave = CTRL-BS`. Each runs its pane with
`RunModal` and the same flags as the menu (§4), then sets PlayScreen
dirty. `Command` returns at once while control is off (`DAT_00666924`,
set by `SetControl(0)` `0x0047c580`) or the game is paused, so none of
these work during a cutscene or a conversation.

## 3. The menu loop — `0x0047e500`

```
InGameMenu():                                                0x0047e500
  if the menu pane is open (DAT_0066f788 = pane +0x40) return
  loop:
    MenuPane.Open()                     0x00537110
    r = RunModal(MenuPane, flags)       flags = MP ? 7 : 0xf (§4)
    MenuPane.Close()                    0x00537170
    switch r:
      1 Load:    loadFromGame = 1; LoadPane.Open(); r2 = RunModal(LoadPane, flags);
                 LoadPane.Close(); PlayScreen.dirty = 1
                 if r2 != 0 return (loaded)  else loop (Exit -> menu)
      2 Save:    SavePane.Open(); r2 = RunModal(SavePane, flags); SavePane.Close()
                 PlayScreen.dirty = 1; if r2 != 0 return (saved) else loop
      3 Options: fromGame = 1; OptionsPane.Open(); RunModal(OptionsPane, flags);
                 OptionsPane.Close(); PlayScreen.dirty = 1; loop (always back)
      4 Quit:    PlayScreen.nextscreen = TLogoScreen; close request; return
      5 Exit:    PostQuitMessage(0); return
      default (6 Resume, ESC): return
```

`PlayScreen.dirty` is `DAT_0065cb40` = PlayScreen (`0x0065caf0`) `+0x50`,
TScreen's dirty flag (this also identifies the flag `LoadGame` sets,
SAVE_GAME §4 step 7). *Correction:* InGameMenuDef_SPEC §10 reads cases 1
and 2 as not returning to the menu; the decomp re-opens it when the
sub-dialog's result is 0 (its Exit button), as the manual says.

### 3.1 The menu pane — `cls_0x5b9480`, instance `0x0066f748`

- **Open** `0x00537110`: `DefScreen_Open("ingamemenu", MP ? "mpingamemenu"
  : "ingamemenu", 0x11, 126, 65, 394, 316, 200, 80, "widgets",
  "ingamemenu")`; on success pause the samples (`0x0049c830`, §4.3).
- **Close** `0x00537170`: resume the samples (`0x0049c890`), then the DEF
  pane close (`0x00434f30`).
- **Control** `0x00537190` (event 3000 = a button clicked), by widget name:

| Button | Result | Condition |
|---|---|---|
| `options` | 3 | — |
| `load` | 1 | single player only |
| `save` | 2 | **only while control is on**: `DAT_0065d0d0` is PlayScreen `+0x5e0`, the control flag; off, the click does nothing. Multiplayer non-host: popup `MPMUSTBEHOST` |
| `saveplr` | — | multiplayer: save the player, popup `MPPLAYERSAVED` |
| `quit` | 4 | after popup `quitgameyn` (Yes/No) answers Yes; then all samples are released (`0x0049c8f0`) |
| `exit` | 5 | after popup `exitgameyn` answers Yes; samples released |
| `resume` | 6 | — |
| anything else | −1 | the pane stays open |

  The result goes to `+0x5c` and the pane closes (vtable `+0x08`), which
  ends its `RunModal`.
- **KeyPress** `0x00537400`: ESC down → result 6, close. No other key.

The conversation check is the control flag: scripts turn control off for
cutscenes, and the dialog pane turns it off while the player chooses a
response (DIALOG.md, `0x00535e90`).

## 4. Modal flags, the pause, the DEF pane flags

### 4.1 `RunModal` flags (exclusive-stack entry flags)

`RunModal` `0x0048f040` pushes with the enclosing modal's flags OR'd in.
Every in-game dialog (menu, load, save, options, from the menu or a
hotkey) passes `MP ? 7 : 0xf`; popups (`0x0053c060`) pass 7. Each bit
narrows one screen pass to the **top** exclusive pane:

| Bit | Pass | Address | Effect |
|---|---|---|---|
| `0x01` | mouse buttons | `0x00490530` | only the top pane gets clicks |
| `0x02` | keys, chars | `0x00490660`, `0x00490760` | only the top pane gets keys; PlayScreen's control-map commands are skipped (`0x0047c630`, `0x0047cdb0`) |
| `0x04` | joystick | `0x00490860` | only the top pane |
| `0x08` | pulse | `0x0048fda0`, `0x004902c0` | only the top pane pulses — the map pane, and with it every object, script and character, stops; PlayScreen also skips the automap update (`0x0052b9f0`) and the game clock (`0x0047c2c0`) |
| `0x10` | animate | `0x0048ff00` | only the top pane animates (draws) |
| `0x20` | a second draw pass | `0x00490030` | only the top pane |
| `0x40` | pass `+0x1c` | `0x00490110` | only the top pane |
| any of `0xf0` | cursor shadow | `0x0047c2c0`, `0x0043a5a0` | PlayScreen doesn't draw the cursor's shadow outside the map view |
| `0x100` | — | `0x0048eea0` | re-apply the UI blit effects (SCREEN_SYSTEM §2.3) |

Without a bit, that pass goes to every pane once, the modal included
(it is in the pane array). **The world pauses under the single-player
menu and dialogs (bit `0x08`) but keeps drawing (no `0x10`)**: the frozen
game shows behind them. In multiplayer (7) the world keeps running.
PlayScreen's deferred loads and saves (SAVE_GAME §7.1) are processed
before the pane pulse, so they are not held by the pause.

### 4.2 DEF pane flags (`DefScreen_Open`'s third argument, pane `+0x60`)

Separate from the modal flags. The in-game dialogs open with `0x11`, the
title ones with 0.

- **`0x01` — overlay chrome.** The screen's own dat is
  `<name>alpha.dat` when clear, else `<name>tex.dat`, or
  `<name>notex.dat` with the `NOTEXOVERLAYS` / `NOBLITTEXTURES` command
  line (`DAT_006680c8`) (`0x00435b20`); the shared widget pack likewise:
  `widgetsalpha.dat` (`DAT_0065bb10`) or `widgetstex.dat` /
  `widgetsnotex.dat` (`DAT_0066733c`), loaded at boot (`0x00486277`,
  `0x004862a1`). The alpha variants are opaque full-screen art for the
  title route; tex is ARGB4444 drawn as a texture over the game.
- **`0x10` — fade.** A level at `+0xbc` steps once per pulse toward the
  target `+0xc0` (5 after `TButtonPane::Initialize` `0x00434e40`)
  (`0x00435d70`); the pane's composed surface is drawn with alpha
  `level * 255 / 5` (`0x00436090`, tex path only). Closing such a pane
  (`0x00435010`) sets the target to 0 and closes for real when the level
  reaches 0. The menu and the in-game dialogs therefore fade in and out
  over 5 pulses (~0.2 s).

### 4.3 Sound

`0x0049c830` stops every *playing* sample in the sound player's 16 2D and
16 3D slots (`AIL_stop_sample`), `0x0049c890` resumes the stopped ones,
`0x0049c8f0` releases all of them. Music is not a slot and keeps playing.
The menu pauses on open and resumes on close; `P` (pause) and the
demo-mode exit popup do the same.

## 5. The load dialog

Entered from: the menu (case 1), the Load Game command (`0x53`,
`0x0047e660` is the same sequence), the title (`0x0053a220`) and the death
screen (`0x00533970`), the last two through the Load Game screen (§8).

**Pane** `cls_0x5b9584`, instance `0x0066f8d0`; `+0x198` is
`loadFromGame` (`DAT_0066fa68`), `+0x234` the selected slot
(`DAT_0066fb04`).

- **Open** `0x00539380`: rescan the single-player slots (`0x0048d260(0)`);
  create a 216×160 16-bit thumbnail bitmap (`+0x19c`) cleared to black;
  selection −1; `DefScreen_Open("loadgame", "default", loadFromGame ? 0x11
  : 0, 0, 0, 640, 480, 450, 160, "widgets", "loadgame")`.
- **Close** `0x00539440`: free the thumbnail, close the pane.
- **Control** `0x00539590`:
  - event 1 (after the .def is built): find `gamelist`, `picture`,
    `gamename`, `modname`, `charname`; list count = slot count; **select
    the last slot** (`0x00430b80(count − 1)`, which raises event 5000);
    hotkeys: Enter on `loadgame`, ESC on `exit`.
  - event 5000 on `gamelist` (selection changed): remember the row; load
    `<slot dir>\ss.bmp` into the thumbnail when it exists (`0x004a2ce0`,
    the file must be exactly 216×160; 8- or 24-bit); `gamename` = slot
    name, `modname` = the slot's module, `charname` = `"Locke"` (a
    literal).
  - event 3000 `loadgame` with a slot selected: result 1;
    `SetStartMode(1, current module, slot)`. From the title: Load Game
    screen's next = PlayScreen, close request (the PlayScreen loads it in
    `Initialize`, behind the loading bar). In game: the load in §5.1,
    then the pane closes.
  - event 3000 `exit`: result 0; in game close the pane; from the title
    the Load Game screen's next = the title, close request.
- **Field getters**: `0x00539470` answers `gamelist_name` (row *n*: slot
  *n*'s name); `0x00539550` answers `picture` with the thumbnail bitmap.

### 5.1 The in-game load

`0x00539590` event 3000 `loadgame`, from the game, after
`SetStartMode(1, module, slot)` (`0x0047f4c0`, recorded for a later
`Initialize`). Everything runs inside the button's handler; the frame loop
turns only where noted.

1. `0x0043a020(0)`: no cursor bitmap.
2. `TimerLoop(1)` (`0x004911b0`): one frame — the world, the HUD and the
   load dialog, without the cursor.
3. `0x0053c1d0("loadingmap")` (§9): the progress popup opens and fades in;
   frames run until it is in, and one more.
4. `0x0048e5b0(slot, 0)`: `LoadGame` (SAVE_GAME.md §4). No frame runs; the
   screen keeps showing the last one.
5. `0x0053c3d0(0x50)`: the bar to 80.
6. With sector preloading on (`DAT_005d7a30`) and a player
   (`DAT_00667fcc`): `0x004997d0(player pos +0x10, level +0xe,
   0x00539990)` loads the sectors around the player (`0x004998b0`,
   `PreloadSectorSize` across); its callback sets the bar to
   `progress × 800 / 1000`. The bar is drawn cumulatively (§9), so it
   stays at 80 until the sectors pass it.
7. `0x0053c3d0(800)`, then `0x0053c360`: the popup fades out (frames run:
   the new world, the HUD and the load dialog under the fading popup)
   and closes.
8. The map pane: centre-on flag 8 (`DAT_006669b0 = MapPane +0xd8`: jump
   at the next update), `0x00454390` (scroll position and sector update),
   `0x00458750(1)` (full redraw).
9. `0x0047c580(1)`: control on.
10. The PlayScreen's fade-in slot (`DAT_0065cb30` = PlayScreen `+0x40`,
    vtable `+0x28`, `0x00491bf0`): nothing unless the screen was faded
    out.
11. `0x0043a020("cursor")`: the game cursor.
12. The side tabs pane (`0x0065be50`): `+0x48`/`+0x4c` cleared and its
    redraw slot (`+0x28`) called — as the PlayScreen pulse does for every
    HUD pane on a drawer change.
13. The pane's own close (vtable `+8`, `0x00539440`), not the fading close
    slot: the dialog goes at once, with result 1, and the menu loop
    returns (§3).

The frame's request load (`0x0047bfab`, console `loadgame`, `newgame`)
is the other in-game path: "Loading Game %s... Please Wait"
(`loadgamefmt`) on the text bar, `LoadGame`, control on; no popup, and the
sectors come in through the map pane's sector update.

## 6. The save dialog

Entered from the menu (case 2) and the Save Game command (`0x54`).

**Pane** `cls_0x5b963c`, instance `0x0066fb08`.

- **Open** `0x005399f0`: rescan the slots (single or multi); thumbnail
  as the load pane; `DefScreen_Open("savegame", "default", 0x11, 0, 0,
  640, 480, 450, 160, "widgets", "savegame")`.
- **Control** `0x00539c00`:
  - event 1: as the load pane, plus the `nameedit` EDIT (MAXLEN 30,
    initial text `"New Game"` from the .def); hotkeys Enter on `savegame`,
    ESC on `exit`; the last slot is selected.
  - event 5000 on `gamelist`: `nameedit` = the slot's name; thumbnail and
    fields as the load pane.
  - event 3000 (or 6001): the name is `nameedit`'s text with every
    character below `0x20` or above `0x7e` and every `. \ / ? * | < > : "`
    removed. `savegame` with a non-empty name: result 1, **`SaveGame(name,
    0)` at once** (`0x0048d720`, not PlayScreen's deferred request), close.
    `exit`: result 0, close.

There is no overwrite confirmation and no delete (PopupDef_SPEC
§13a.6).

**The name field takes typing only after a click.** Nothing gives the
EDIT focus when the dialog opens (`0x005399f0`, event 1, `DefScreen_Open`;
the DEF engine's `0x004369f0` is the keyboard-navigation highlight, not
edit focus). The EDIT starts editing on a left press inside it
(`0x00432ab0`: its `+0x90`, the pane's focus `+0xa0`, the caret blink);
its character handler (`0x00432f70`) drops every character while it isn't
editing, and characters reach it through the pane (`0x00436460`) either
way. So typing straight after the dialog opens changes nothing, and
Enter then is `savegame`'s key: it saves under the name shown (the last
slot's, or "New Game"). While editing, characters append (a click
doesn't clear the field), Backspace deletes, and Enter ends editing and
sends event 6001, which saves nothing (its sender is the EDIT, not
`savegame`); a second Enter saves. A press outside the field ends
editing too (`0x00436530` → `0x00433100`). Verified in the port
2026-10-05 (headless: type without a click → name unchanged; click, type,
Enter, Enter → saved under the typed name).

## 7. The options dialog

Entered from the menu (case 3), the Game Options command (`0x52`,
`0x0047e700`) and the title. **Pane** `cls_0x5b9744`, instance
`0x0066fcc0`; `+0x17c` is `fromGame` (`DAT_0066fe3c`). Open `0x0053a8b0`
uses DEF flags `fromGame ? 0x11 : 0`; the control handler `0x0053aa90`
(OptionsDef_SPEC §6.3): `ok` applies the settings and saves `[Options]`
and `[Controls]`, `cancel` restores the gamma; from the game either
closes the pane (back to the menu), from the title the Options screen's
next = the title. What each control reads and writes, and the INI, are
in [OPTIONS.md](OPTIONS.md).

## 8. The title-route screens

| Screen | Object, vtable | Initialize / Close | Fader | Pane |
|---|---|---|---|---|
| Load Game | `0x0066fa78`, `0x5b9538` (ctor `0x00539f60`) | `0x005392e0`: done = 0, the cursor, fader `Setup(8)` into both slots, open the load pane, `AddPane`. `0x00539360`: `RemovePane`, close the pane | yes (`+0x70`) | the load pane as an ordinary pane, `loadFromGame = 0` |
| Options | `0x0066fe88`, `0x5b96f4` (ctor `0x0053bd60`) | `0x0053a800`: done = 0, open the options pane (error "Trouble initializing Options pane"), `fromGame = 0`, the cursor, `AddPane`. `0x0053a860`: `RemovePane`, free the rebind buffer, close | none | the options pane |

- Title `Load Game` `0x0053a220`: next = Load Game screen, close,
  `loadFromGame = 0`. Title `Options` `0x0053a260`: next = Options.
- Death `Load` `0x00533970`: `loadFromGame = 0`, next = Load Game screen,
  close. Its Exit then returns to the title, not to the death screen.

## 9. Popups

`0x0053c060(key, flags)` (PopupDef_SPEC §6): returns 0 when no screen is
up; saves the cursor and sets `cursor`; opens the popup pane
(`0x00670090`, `0x0053bf00`): panel `yesno` (flags `& 2`), `okcancel`
(`& 4`) or `ok`; the message is `DialogList.GetLine(key)` unless flags
`& 0x10` (then `key` is the text); DEF flags 0 (alpha chrome) when flags
`& 1`, else `0x11`; rect 129, 122, 398×212. `RunModal(popup, 7)`; Yes/OK
→ 1, No/Cancel → 0 (`0x0053bfa0`). `quitgameyn` / `exitgameyn` use flags 3:
Yes/No, alpha chrome. Texts (`english.def`): QUITGAMEYN "\nAre you sure
you want to quit the current module?", EXITGAMEYN "\nAre you sure you want
to exit the game and return to Windows?".

The progress popup (§5.1) is its own instance (`0x0066ff10`), used only
by the in-game load:

- **Open** `0x0053c1d0(key)`: a 280×24 bar strip (`0x004a1ec0`, the
  display's format) cleared to the transparent key colour; the message
  `DialogList.GetLine(key)` (`0x0049d800`; `loadingmap` = "The game is
  loading map graphics, characters, and animations... Please Wait.");
  `DefScreen_Open("popup", "progress", 0x11, …, 129, 122, 398, 212, …)`;
  `AddPane` and `SetExclusivePane(pane, 7)` (`0x0048ed90`, `0x0048eea0`)
  — not `RunModal`, so the enclosing dialog's flags are **not** added
  (`0x0048f040` ORs them in; `0x0048eea0` doesn't) and the world pulses
  under the popup; then frames until the fade is in, plus one.
- **Progress** `0x0053c3d0(n)`: a rectangle `n × 280 / 1000` wide in
  colour `0x00429950(0xaa, 0, 0)` (RGB 170, 0, 0) filled into the strip —
  never cleared, so the bar only grows — blitted with the key colour
  transparent (`0x004bd680`, mode `0x100`) at the `progress` widget's
  place (panel 49, 126) and the display flipped (`0x004a9ee0`); the widget
  is marked for redraw.
- **Close** `0x0053c360`: the fading close slot (`0x00435010`), frames
  until it is out, plus one; the strip is freed.

## 10. Port (2026-10-05)

| Retail | Port |
|---|---|
| ESC case in `KeyPress` `0x0047c630` | `TPlayScreen::KeyPress`: ESC opens the menu (`OpenInGameMenu`) when no flagged modal is up and the editor is off; in demo mode `TInGameMenu::AskExit` (`exitgameyn`) |
| `ss.bmp` before the menu | `TSaveGame::CaptureThumbnail` takes a completion; the menu opens once the frame without it has been read back (`TDisplay` now answers a failed readback too, so the menu can't be lost) |
| controls 65–68, `Command` `0x52`–`0x55` | four entries appended to the port's control table with retail's keys and INI names (`GAMECMD_GAMEOPTIONS` … `GAMECMD_QUICKSAVE`); `TPlayScreen::Command` runs them while control is on. F5 no longer quick-saves (retail F5 is Belt Use 2); F9 (reload, not retail) stays a developer key |
| `0x0047e500` and its nested `RunModal`s | `TInGameMenu` (`src/ingamemenu.*`, owned by `TPlayScreen`): a continuation chain of `PushModal` completions — same panes, order and results |
| menu pane `0x00537110`–`0x00537400` | `TInGameMenuPane`: results 1–6, Save gated on `PlayScreen.IsControlOn()`, Quit/Exit through `TPopupPane`, ESC = Resume, samples paused while open |
| load / save panes | `TSaveSlotPane` (the shared slot list, thumbnail, fields, Enter/ESC keys, last slot selected) under `TLoadGamePane` / `TSaveGamePane` (`src/savegamepane.*`); `TSaveGamePane::SanitizeName` is retail's filter |
| options pane | `TOptionsPane::OpenOptions(fromGame, x, y)`; the settings and key bindings bound as retail's ([OPTIONS.md](OPTIONS.md) §9); after them "ok" / "cancel" go to the host |
| popup `0x0053c060` | `TPopupPane::Ask(screen, key, flags, done)` (`src/popuppane.*`): retail's flags and panels, Yes/Ok 1, No/Cancel 0 |
| modal flags `0xf` | `TScreen::MODAL_*` carry retail's bits and each pass honours its bit; `TPlayScreen::Update` stops the world (mode tick, level entry, areas, game clock) under `MODAL_PAUSE`; mouse and keys go to the modal only, the HUD and the control map get none |
| DEF flags `0x01`, `0x10` | `TDefPane::Open(def, panel, flags, rect, datBase)`: `tex` / `alpha` chrome for the screen and the widget pack; the fade, time-based at the pulse rate; `Finish(result)` ends the modal after the fade-out |
| DEF widget behaviour the dialogs use | retail events as virtuals (`OnOpened` = 1, `OnActivate` = 3000, `OnListSelect` = 5000, `OnKey`, `DrawField`); button keys (own, Enter for ok/yes, ESC for cancel/no) firing on key-down; the `click1` sound on a DEF button; EDIT text from character events, Backspace, Enter ends editing, a press outside ends it; the VLIST list's scrollbar (arrows, page, thumb); word-wrapped TEXT and `TEXT_ELIPSES` |
| in-game Load (§5.1) | `TInGameMenu::BeginLoad`: no cursor; the `loadingmap` popup (`TPopupPane::OpenProgress`, pushed with exactly `MODAL_INPUT` through `TScreen::PushExclusive`); once it is in, `GameFlow.Session().RequestLoad(slot)`. The session runs the load a step a tick (`LoadGameState` to 80, `EnterWorld` to 800 at `progress × 800`); `TPlayScreen::StepGameLoad` holds the world and draws a still behind it (below), feeding `LoadProgress`; then `LoadFinished`: bar at 800, the popup fades out over the live new world, `EndLoad`: camera jump (`MapPane.SnapIfFollowing`), control on, the screen's fade-in, the game cursor, the dialog ends at once with result 1 |
| the screen standing still while `LoadGame` runs | `TPlayScreen::StepGameLoad`: the first tick asks for a capture of the frame's layers under the pane tree (`Display.RequestCapture(…, TScreen::kPaneLayerZ)`: the world and the HUD panels, without the panes and the cursor); the still goes up as a renderer HUD layer at z 50 (over the HUD panels, under the pane tree); then each tick runs one session step. While the still is up the world doesn't tick, render or take input, the HUD panels aren't refreshed, and save / load requests wait; the panes (text bar, dialog pane, the load dialog, the popup) keep drawing live over it |
| the progress popup (§9) | `TPopupPane::OpenProgress` / `SetProgress` (the bar never shrinks) / `CloseProgress`; the `progress` BITMAP field is the bar, drawn in the popup's compose |
| request loads (console, F9, `--savecycle-test`) | the same staged load behind the still, without a popup |
| in-game Save: `SaveGame` at once | the host hands the name to `GameFlow.Session().RequestSave` (next tick) |
| Load Game / Options screens | `TLoadGameScreen` (with its fader) / `TOptionsScreen` (`src/menuscreens.*`), entered through `GameFlow.ShowLoadGameScreen()` (title, death) / `ShowOptionsScreen()`; a title load goes through `GameFlow.LoadGame(slot)` and the loading screen; Exit through `GameFlow.ReturnToTitle()` |
| samples paused under the menu | `TSoundPlayer::PauseSamples` / `ResumeSamples`: every playing sound and duplicate stops where it is and starts again; music keeps playing; a paused sound counts as playing, so the dying-sound collector keeps it |

Fixed on the way: `TScreen::ReleaseExclusivePane` moved each remaining
modal's completion onto itself, which emptied it — a modal under a popup
lost its completion when the popup closed.

Test tooling: `--input-script` has `type TEXT` (character events) for
EDIT fields.

**Deviations**

- The in-game load runs a step a tick instead of in one call. The world
  and the HUD panels stand still as in retail (a still of them); the panes
  over them stay live, so the text bar and the dialog pane draw as they
  are, unchanged by the load in practice. The popup asks for the load once
  it has faded in; the load starts at the next tick, a frame later than
  retail.
- The port loads the player's whole level (`EnterWorld`), not
  `PreloadSectorSize` sectors around him; the bar runs over that.
- The cursor: `SetMouseBitmap(nullptr)` removes the in-frame cursor; in
  windowed play with the OS pointer the pointer stays visible.
- The side tabs need no redraw call: the port redraws every pane every
  frame.
- Request loads (console `loadgame`, F9, `--savecycle-test`) don't print
  retail's "Loading Game %s... Please Wait" on the text bar.
- The thumbnail of a slot without `ss.bmp` shows black; retail kept the
  previously selected slot's picture (its bitmap was only overwritten on a
  successful load).
- Exit Program fades the PlayScreen out before quitting
  (`GameFlow.QuitApplication`); retail's `PostQuitMessage` left at once.
- `NOTEXOVERLAYS` isn't a port option: in-game dialogs always use the
  `tex` chrome.
- The release of all samples before Quit/Exit (`0x0049c8f0`) isn't
  ported; leaving the game stops the area sounds (`TGameSession::End`).
- The popup doesn't swap in the `cursor` bitmap (`0x0053c060`): the game
  cursor is already that one wherever the port opens it.
- The `0x20` / `0x40` modal bits are stored but no port pass reads them:
  the port's Compose/Draw passes follow `0x10`. The `0xf0` cursor-shadow
  test is the legacy `DrawMouseShadow`'s (`InCompleteExclusion`, `0x10`).
- Mouse moves follow `MODAL_MOUSE` like clicks; retail's move pass isn't
  pinned to a bit.

## 11. Open questions

See [../AUTHOR_QUESTIONS.md](../AUTHOR_QUESTIONS.md) §"In-game menu" (60–65,
110–111; shots S13, S17).

## 12. Decomps used

`0x0047c630` KeyPress, `0x0047c400`, `0x0047c2c0`, `0x0047cdb0`,
`0x0047bd20` (the per-frame controller), `0x0047b4d0`, `0x0047e500`,
`0x0047e660`, `0x0047e700`, `0x0047c550`, `0x0047f4c0`, `0x00537110`,
`0x00537170`, `0x00537190`, `0x00537400`, `0x00537420`, `0x00539380`,
`0x00539440`, `0x00539470`, `0x00539550`, `0x00539590`, `0x005399f0`,
`0x00539ab0`, `0x00539ae0`, `0x00539bc0`, `0x00539c00`, `0x00539f60`,
`0x00539fc0`, `0x005392e0`, `0x00539360`, `0x0053a800`, `0x0053a860`,
`0x0053a8b0`, `0x0053aa60`, `0x0053aa90`, `0x0053b9e0`, `0x0053bd60`,
`0x00533950/70`, `0x0053a220/60`, `0x0053c060`, `0x0053c1d0`,
`0x0053c360`, `0x0053c3d0`, `0x00435010`, `0x00435040`, `0x00435990`,
`0x00435b20`, `0x00435cb0`, `0x00435d70`, `0x00435de0`, `0x00436090`,
`0x00436de0`, `0x004361f0`, `0x00430b80`, `0x00430c50`, `0x0048f040`,
`0x0048eea0`, `0x0048fda0`, `0x0048ff00`, `0x00490030`, `0x00490110`,
`0x004902c0`, `0x00490530`, `0x00490660`, `0x00490760`, `0x00490860`,
`0x0048d260`, `0x004a2ce0`, `0x0049c830`, `0x0049c890`, `0x0049c8f0`; the
control table at `0x005d5500` (decoded with `peread.py`). The in-game load
(§5.1, §9): `0x0047f4c0`, `0x0043a020`, `0x004911b0`, `0x0048e5b0`,
`0x004997d0`, `0x004998b0`, `0x00539990`, `0x00454390`, `0x00458750`,
`0x0047c580`, `0x00491bf0`, `0x0048eea0`, `0x0048f040`, `0x00429950`,
`0x004384a0`, `0x004bde60`, `0x004bd680`, `0x004a9ee0`, `0x00435010`,
the request block `0x0047bfab..0x0047c0b8`; references to `0x0065cb30`
and `0x0065be50`.
