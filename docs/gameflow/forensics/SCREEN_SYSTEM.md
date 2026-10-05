# Screen system — forensics

Retail behavior of the screen / pane / modal layer, the requirements it
implies, and where the port stands today. Input to the design in
[../ARCHITECTURE.md](../ARCHITECTURE.md). Addresses are retail
`Revenant.exe`; decomps live under `recon/discovered/` (screen helpers
decompiled for this write-up are listed in §7).

Retail fidelity: **retail-confirmed** for everything in §2 unless marked
otherwise.

## 1. Requirements (classic docs + manual)

From [MANUAL.md](../../MANUAL.md) §III "Interface", [UI.md](../../UI.md),
[HUD.md](../../HUD.md):

- **Main menu**: New Game, Load Game, Multiplayer, Options, Exit.
  Load Game opens a save list with a screenshot per save and LOAD/EXIT
  buttons; EXIT returns to the main menu. Options opens the options
  screen.
- **In-game menu** (ESC): Load Game, Save Game, Options, Quit Module
  (returns to the main menu). Save Game: pick slot, type description,
  SAVE returns to the game, EXIT returns to the in-game menu. Saving is
  not allowed during a conversation.
- **Death**: a death screen with Restart / Load / Exit.
- **Pause** (`P`), **exit confirmation** (Alt+Tab / close prompts
  `EXITGAMEYN`), **loading bar** while a game loads.
- The HUD is a set of panes on the play screen; dialogs and the in-game
  menus overlay the running game.

## 2. Retail structure

### 2.1 Screen manager

- One current screen (`CurrentScreen`, `DAT_00667fd0`). WinMain runs
  `for (s = boot; s; s = ShowScreen(s, 0))`.
- `ShowScreen(screen, n)` `0x004909d0`: set current, clear the `done`
  flag, `BeginScreen` (→ `Initialize`) unless the screen is the one
  pre-initialized screen (`DAT_0065bb14`), run `TimerLoop(n)`, then
  `Close` (vtable `+0x08`), clear the pane array and exclusive stack,
  release background buffers, and return `screen->nextscreen` (`+0x18`)
  unless the app is quitting (`DAT_006682b8`).
- `TimerLoop(n)` `0x004911b0`: per iteration pump Win32 messages, handle
  Alt+Tab → `EXITGAMEYN` yes/no popup → `PostQuitMessage`, keep the
  24 Hz pacing/frame-skip statistics, then call the screen's
  `TimerTick(draw)` (vtable `+0x44`; draw is false on skipped frames).
  `n = 0` runs until the screen finishes; `n = k` runs exactly `k` frames
  and returns. The game never blocks.
- Closing a screen: `TScreen::Close` request `0x0048ea40` — close every
  exclusive pane (vtable `+0x08`), set `done` (`+0x54`), hide the
  screen's focus pane (`+0x44`, vtable `+0x2c`), then broadcast event
  `0x100` (closing). Next screen is whatever `nextscreen` was set to
  first.

### 2.2 Screen layout and virtual interface

`TScreen` (retail, from `ShowScreen` / `Close` / `SetExclusive` / the
`TLogoScreen` and `TPlayScreen` vtables @ `0x5a5d18` / `0x5a5320`):

| Offset | Field |
|---|---|
| `+0x04..+0x14` | pane array (`TPointerArray`, ≤ 32) |
| `+0x18` | `nextscreen` |
| `+0x1c` | exclusive depth (≤ 4) |
| `+0x20..+0x2c` | exclusive pane indices |
| `+0x30..+0x3c` | exclusive flags |
| `+0x40`, `+0x44` | background / focus pane pointers |
| `+0x48` | frame counter |
| `+0x4c` | first-frame flag |
| `+0x50` | dirty |
| `+0x54` | done |

Vtable slots (shared order, `TScreen` base entries in parentheses):
0 dtor, 1 `Initialize`, 2 `Close`, 3 `Pulse`, 4 `DrawBackground`,
5 `Animate`, 6 `MouseClick`, 7 `MouseMove` (`0x490110`), 8 `KeyPress`,
9 `CharPress` (`0x4902c0`), 10 `Joystick`, 11 (`0x490530`),
16 `OnEvent(code, param)` (`0x490960`), 17 `TimerTick` region
(`0x490bd0` / `+0x44`), 18 (`0x491870`). PlayScreen overrides 0–6, 8,
10, 12–14 and 19.

### 2.3 Panes, exclusive stack, events

- Panes live in a flat array; input and draw walk it in order.
- **Exclusive stack** (`SetExclusivePane` `0x0048eea0`): up to 4. Flags
  per entry: `0x10` = only the top exclusive pane animates; `0x100` =
  run `0x004aacb0` (`BlitEffect_Iterate`) over the whole screen with
  mode 6 — it re-applies the registered UI blit-effect regions (glow
  text etc.) that overlap the screen; exact visual purpose unconfirmed,
  it is **not** a dimming pass.
  Pushing broadcasts event `0x101` with the pane. Input goes only to the
  top exclusive pane.
- **Screen → pane events** (`OnEvent` `0x00490960`): forwards
  `(code, param)` to every pane (pane vtable `+0x78`). Codes seen:
  `0x100` screen closing, `0x101` exclusive pushed, `0x103` (also calls
  `0x00416fb0`; meaning unconfirmed).

### 2.4 Modal panes

`RunModal(pane, flags)` `0x0048f040`: add the pane to the current
screen, push it exclusive with the screen's current flags `| flags`,
then `while (pane is open) TimerLoop(1);` and return the pane's result
code (`+0x5c`). The world keeps running (or stays paused, per the
screen's own pause state) because every iteration is a full frame. The
caller simply continues after the modal returns.

DEF screens are panes: `DefScreen_Open` `0x00435150` initializes a
`TButtonPane` and loads the `.def`/`.dat` into it. The in-game menu
(`0x00537110`, `ingamemenu.def` / `mpingamemenu.def`, rect
126,65 394×316, flags `0x11`) is opened and run with `RunModal`.

### 2.5 Screens and transitions

| Screen | Object | Entered from | Leaves to |
|---|---|---|---|
| Intro movie | Movies.cpp `0x004bc470` (played inside WinMain before the loop) | boot | title |
| Title `TLogoScreen` | `0x0065d358` | boot, death Exit, in-game Quit, `endgame` | PlayScreen (New Game), Load screen, MP, Options, quit |
| Load game screen | `0x0066fa78` | title, death Load | PlayScreen / title |
| Options screen | `0x0066fe88` | title | title |
| `TPlayScreen` | `0x0065caf0` | title, death Restart, Load screen, QUICKSTART, EDITOR | death screen, title |
| Death `TDeathScreen` + `TDeathPane` | `0x0066f680` / `0x0066f500` | `TPlayer::Animate` | PlayScreen (Restart), Load screen, title (Exit) |
| MP screens | `0x00659bd8` | title | — (out of scope) |

Inside PlayScreen, overlays are modal panes, not screens: the in-game
menu (`0x0047e500`) loops `RunModal(ingamemenu)` → result 1 Load
(loadgame pane, modal) / 2 Save (savegame pane) / 3 Options (options
pane) / 4 Quit Module (`nextscreen = title`, close) / 5 exit app /
otherwise resume. Dialog choices, popups and buy/sell are panes too.

Death: `TPlayer::Animate` (`0x00518aa0`) restarts a 192-frame countdown
while the player is alive; once health < 1 and it runs out, PlayScreen's
`nextscreen = death screen` and PlayScreen closes (the world is torn
down). `endgame` (`0x00427060`) sets PlayScreen's `nextscreen = title`
and closes it.

### 2.6 Play-screen fade

The screen fade is state on `TPlayScreen` (`+0x6a0..+0x6c4`):
`0x0047ebc0(on)` starts a fade out (`+0x6b0`, step `+0x6c4 = 2`) or
requests a fade in (`+0x6b4`); `0x0047ecc0` requests fade-in when faded;
`+0x6c0` is a mode read by `fadescreenout` (`0x0047ed20`).
`Initialize` calls `0x0047ebc0(1)`. Fade timing/rendering is in
PlayScreen's pulse/animate (not yet decoded).

### 2.7 Loading screen

`TLoadScreen` (`cls_0x4485a0`): `PlayScreen::Initialize` opens
`loadbar.dat` (or the module's `loadscreen.bmp`) via `0x004485a0` and
advances the bar with `0x00448680(step)` between initialization stages.
Spec: [../../ui/forensics/LoadingScreen_SPEC.md](../../ui/forensics/LoadingScreen_SPEC.md).

### 2.8 Pre-release vs retail

The 1998 snapshot (`legacy/screen.cpp`) already has the `ShowScreen` /
`TimerLoop` / flat pane array / exclusive-with-`complete` model, but no
`RunModal`, no screen→pane event broadcast, no exclusive flags (`0x10`,
`0x100`) and no DEF panes. Retail grew the modal layer late (in-game
menu, load/save/options panes, popups, dialog), so the port has no
snapshot source for it — the decomps above are the only reference.

## 3. Port state (2026-10-04)

- `AppFrame` drives one screen non-reentrantly: `ShowScreen` only
  initializes; `AppFrame` calls `Tick()` then `DrawFrame()` and swaps to
  `GetNextScreen()` when `IsDone()` ([FRAME_PIPELINE.md](../../FRAME_PIPELINE.md)).
- `TScreen` / `TPane` keep the retail flat-array + exclusive model; the
  UI track added the retained tree (parent/children), layout, anchors,
  9-slice, `UIStyle`, clip API (A.2) — unused by any production pane.
- 2D presentation goes through `TRenderer` HUD drawables (`AddHud`), not
  through the pane walk. Each reconstructed UI panel registers its own
  drawable.
- Screens in the tree: `TPlayScreen`, `TTestScreen`, `TCinematicScreen`.
  The title screen, load/options screens, death screen and in-game menu
  exist only as `--test` harnesses (`uimainmenutest`, `uideathtest`,
  `uidefscreentest`) or as the DEF engine (`TDefScreen`, a compose-to-
  texture engine rather than a pane).
- `IRuntimeMode` (game / editor) lives inside `TPlayScreen`.

**Below the quality bar (reported 2026-10-04):** the production HUD is
assembled from test-mode free functions (`InitializeUIHudMode` in
`uihudtest.cpp`, with "synthetic state" toggles); those harnesses are
parallel implementations of the original pane classes (`TStatusBar`,
`TInventory`, `TStatPane`, `TSpellPane`, `TEquipPane`, `TMultiCtrlPane`,
`TTextBar`, `TQuickSpellPane`), which remain unused. Hand-rolled button
hit-testing in the title/death harnesses instead of
`TButtonPane`/`TButton`.

## 4. Constraints that force a divergence

1. **No re-entrant frame loop.** sokol owns the outer loop and calls
   `AppFrame` once per frame. Retail's `RunModal` / `TimerLoop(1)`
   call-stack style (the caller resumes when the modal returns) must
   become continuation style: the modal's result is delivered to a
   completion handler on a later frame. Behavior is unchanged.
2. **GPU compositor.** Pane drawing reaches the screen through
   `TRenderer` submission, not `Display` blits into a CPU backbuffer.
3. **Resolution independence.** Classic 640×480 must stay
   pixel-faithful; Revisited canvases lay out through anchors (A.2c).
4. **World lifetime.** Retail ties the world to PlayScreen
   `Initialize`/`Close`; the agreed design moves it to a game session so
   overlays and screen changes don't own it.

## 5. Open questions for the author

- Death screen **Restart**: retail only switches back to PlayScreen with
  the previous start mode (start name/index were cleared after use), so
  after a loaded game it would fall through to `GAMENOTFOUND` → new
  game. Is that the intended behavior, or did Restart reload the last
  save?
- Event code `0x103` and the pre-initialized screen `DAT_0065bb14`:
  what were they for?

## 7. Decomps used

`0x004909d0` ShowScreen, `0x004911b0` TimerLoop, `0x0048ea40` Close,
`0x0048eea0` SetExclusive, `0x0048ed90` AddPane, `0x0048f040` RunModal,
`0x00490960` OnEvent, `0x0048ff00` Animate, `0x0047e500` in-game menu,
`0x0047e660` load dialog, `0x00533950/70/90` death buttons,
`0x00537110/70` in-game menu open/close, `0x0047ebc0/ecc0/ed20` fade,
`0x0047c580` SetControl, plus the existing TLogoScreen / TPlayScreen /
TDeathScreen / DefScreen decomps in `recon/discovered/`.
