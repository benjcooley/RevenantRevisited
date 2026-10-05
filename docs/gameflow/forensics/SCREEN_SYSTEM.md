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
  and returns. The game never blocks. A finished screen (`done`) leaves
  the loop only once its fade-out has reached black (§2.6).
- Closing a screen: `TScreen::Close` request `0x0048ea40` — close every
  exclusive pane (vtable `+0x08`), set `done` (`+0x54`), start the
  screen's fade-out (fade-out slot `+0x44`, fader vtable `+0x2c`; §2.6),
  then broadcast event `0x100` (closing). Next screen is whatever
  `nextscreen` was set to first.

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
| `+0x40`, `+0x44` | fade-in / fade-out fader slots (§2.6) |
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

### 2.6 Screen fade

*Correction:* earlier versions of this section took PlayScreen
`+0x6a0..+0x6c4` (`0x0047ebc0` / `0x0047ecc0` / `0x0047ed20`) for the
fade. Those fields are PlayScreen's bottom drawer (DIALOG.md §1.4: mode 1
editor/console, 2 HUD panes, 3 buy/sell); `0x0047ece0`, which LoadGame's
reset calls, is a drawer close request too. The fade is its own class.

**The fader** — one small class in `Screen.cpp` (its draw asserts name
`d:\revenant\Screen.cpp`, lines 0x7a7–0x7b8), vtable `0x005a4c30` (the
only vtable that uses these methods), 24 bytes; `TScreenFade` in the
port:

| Offset | Field |
|---|---|
| `+0x00` | vtable `0x005a4c30` |
| `+0x04` | flags: `2` fading in, `4` fading out (`6` = busy); `0x10` / `0x20` make the screen skip its pulse/animate and its draw passes while this fader isn't settled (no screen sets them) |
| `+0x08` | current step `cur`: 0 black … `steps` clear |
| `+0x0c` | target step |
| `+0x10` | `steps` |
| `+0x14` | a 4-byte value from `Setup` (`0x00444e20(0)`, likely a color); the draw never reads it |

| Slot | Address | What |
|---|---|---|
| `+0x00` | `0x00491c90` | Step: `cur` one toward the target |
| `+0x04`, `+0x08`, `+0x0c`, `+0x14`, `+0x34` | `0x0046cf40/50/60/70`, `0x0046cfb0` | empty hooks the screen's draw/animate passes call |
| `+0x10` | `0x00491cb0` | Draw (below) |
| `+0x18` | `0x0046cf80` | IsBusy: `flags & 6` |
| `+0x1c` | `0x0046cfc0` | IsVisible: busy, or `cur != steps` |
| `+0x20` | `0x00491c50` | IsFadedIn: `cur == steps`, not fading in |
| `+0x24` | `0x00491c70` | IsFadedOut: `cur == 0`, not fading out |
| `+0x28` | `0x00491bf0` | FadeIn: unless fading in, target = `steps`; if `cur != steps`: `cur -= 1`, flags → fading in |
| `+0x2c` | `0x00491c20` | FadeOut: unless fading out, target = 0; if `cur != 0`: `cur += 1`, flags → fading out |
| `+0x30` | `0x0046cf90` | Setup(steps, value, flags): `cur` = target = 0 (black) |

**Owners.** `TScreen` has a fade-in slot (`+0x40`) and a fade-out slot
(`+0x44`). Eight screen constructors embed a fader (vtable stores at
`0x0046cefd`, `0x0046d10d`, `0x0046d33d`, `0x0046d41d` — screens
`0x5a4be4`, `0x5a4d20`, `0x5a4edc`, `0x5a4f28`, at `+0x70`; `0x00488e7d`
TLogoScreen `+0x70`; `0x00539f6d` screen `0x5b9538` `+0x70`; `0x0053c6b4`
screen `0x5b9974` `+0xc0`; `0x0047a62a` TPlayScreen `+0x5bc`), and their
`Initialize` calls `Setup(8, 0, 0)` and points both slots at it:
TPlayScreen `0x0047b10c..0x0047b13e`, only when `0x00668158` (the
`EDITOR` command line, `0x00483bc0`) is off; TLogoScreen
`0x0053a31d..0x0053a34d`. TDeathScreen has none. `DAT_0065cb30` /
`DAT_0065cb34` are PlayScreen (`0x0065caf0`) `+0x40` / `+0x44`, which is
why nothing stores to them directly. **C**

**Screen lifecycle.** **C**
- `BeginScreen` `0x0048e8f0`: after `Initialize`, the fade-in slot's
  FadeIn. A screen comes up black and fades in.
- Close request `0x0048ea40`: the fade-out slot's FadeOut.
- `TimerLoop` `0x004911b0` leaves when `done` and (neither slot busy, or
  the fade-out slot IsFadedOut): a closing screen keeps running until
  black.
- Pulse pass `0x0048f180`: the screen's pulse (vtable `+0x10`), then
  Step on each busy slot. While a slot is busy fading in it also posts
  `+steps` to `0x0065abd8` (to `0x0065abdc` if one is pending); fading
  out, `-steps`. Area music changes post `±8` to the same globals
  (`0x0041a3b0`, `0x0041b1d0`, `0x0041ba00`, `0x0041bd10`, `0x0041c600`)
  and the music player `0x0041b240` won't start its next track while
  `0x0065abd8` is set: a music fade request. Its consumer wasn't found in
  the disassembled code. **I**
- `TimerTick` `0x00490bd0` (and the immediate repaint `0x00491870`):
  after every screen pass and the cursor (`0x0043a480`), Draw on each
  visible slot. The fade covers everything, the cursor included.

**Draw** `0x00491cb0` (640×480). If `cur != steps`: `v` = `cur` clamped
to `0..steps`, `level = v * 31 / (steps - 1)` (integer). `level < 1`:
fill the display black, flip, fill again (both buffers black).
`level < 31`: a black D3D quad over 640×480 (XYZRHW|DIFFUSE|SPECULAR|TEX1,
no texture, alpha blend SRCALPHA/INVSRCALPHA, z off, cull none), alpha
`trunc((1 - level / 31) * 255)` (`0x005a5fe8` = 1/31, `0x005a352c` = 255).
`level >= 31`: nothing. Then the busy flags clear if `cur` == target.
With 8 steps: `cur` 0 black; 1–6 alpha 222, 189, 148, 115, 74, 41; 7 and
8 clear. **C**

**Timing** (24 Hz ticks, 8 steps). A fade asked for in tick N's pulse:
that tick's step undoes the ±1. Out: drawn clear at N and N+1, alpha 41
… 222 at N+2 … N+7, black at N+8, busy cleared by N+8's draw, so a
`wait screenfade` lets go at N+9's pulse. In from black: black at N,
alpha 222 at N+1 … clear from N+7, busy cleared at N+8. A third of a
second each way. **C**

**Scripts.** **C**
- `fadescreenout` `0x00427e80`, single player: if the fade-out slot is
  set, FadeOut, `DialogPane.ClearSpeech(false)` (`0x00535d80`), and if
  PlayScreen's drawer is in mode 3 (`0x0047ed20`) close it
  (`0x0047ecc0`); then the script's taken bit 2. Returns 1 (the
  interpreter's "wait for the target" result). Multiplayer: sets the
  user player's state bit 8 through SetPlayerState `0x0051d680` (which
  fades that player's screen) and waits 0x20 frames unless the user is
  the local player.
- `fadescreenin` `0x00427f60`: the fade-in slot's FadeIn, clears taken
  bit 2, returns 1. Multiplayer: clears state bit 8.
- `wait screenfade` (type 6, check `0x00492d70`): satisfied once
  PlayScreen's IsFading `0x0048eb00` (either slot busy) is false.
- `TScript::End` `0x00493e40`, taken bit 2: fades back in only when
  single player **and** `0x0067682c` is set — the network layer's host
  flag (no store to it anywhere in the exe; a member of the network
  object around `0x00676738`). In single player End just drops the bit,
  so a block that fades out and ends leaves the screen black.
- Ahkuilon's scripts use each command 79 times, nearly always as
  `FADESCREENOUT / WAIT SCREENFADE / [player.pos, toggle invisible,
  control on] / FADESCREENIN` (sometimes `nowait`).

**Other callers.**
- PlayScreen restart: the key/click handlers `0x0047ce80` / `0x0047cf40`
  stop the player's script (`0x00492490`) when it runs trigger 1 and no
  `PAUSE`, restore control, FadeOut and set `+0x5dc`; PlayScreen's
  per-frame controller (`0x0047bdf6`) waits until the fade is done, then
  loads `newgame` (`0x0048e610` = LoadByName("newgame", 1)), clears
  `+0x5dc` and fades in. Probably skipping the opening. **I**
- The in-game Load pane fades in after loading (`0x00539792`, after
  LoadByIndex and SetControl(1)). **C**
- SetPlayerState `0x0051d680`, multiplayer local player: state bit 8 set
  → FadeOut + ClearSpeech + drawer close; cleared → FadeIn unless already
  faded in or busy.
- `0x0054cbb0` (a pane's draw, called from `0x004597b0`) reads
  IsFadedOut (`0x0048eaf0`).

**Port (2026-10-05).** `TScreenFade` (`src/screen.{h,cpp}`) with the
retail methods; `TScreen::fade` stands for both slots; `BeginScreen`
fades in; `TScreen::RequestClose` is the close request (used by
`TGameFlow::SwitchTo`), `ReadyToEnd` the TimerLoop exit test (used by
`AppFrame`), `IsFading` `0x0048eb00`. TPlayScreen (not in the editor)
and TLogoScreen embed one. The cover is the screen's top HUD layer
(z 2000, above the cursor HUD's 1000), drawn with
`TRenderer::FillScreen`. The level moves 24 steps a second and is brought
up to each tick right after its pulse, so waits release on retail's
ticks; the cover is interpolated from the last tick to the frame time.
While fading in it is drawn one step behind the level, because retail
held each step's cover until the next pulse had run and scripts teleport
right after `fadescreenin`; interpolating ahead showed the old picture
through the lightening cover. `fadescreenout` / `fadescreenin` / `wait
screenfade` and End's bit 2 are ported (single player). Not ported: the
drawer close in `fadescreenout` (no drawer), the music fade request, the
`0x10` / `0x20` flags (unused), the restart flow (`+0x5dc`), the Load
pane's fade-in (no in-game Load pane yet), multiplayer.

**Deviations:** no 31-level quantization; a fade-in's reveal trails
retail's by one tick; the cover spans the whole window at any resolution;
in windowed mode the OS cursor stays above it; the editor (toggled in a
running game) draws no cover, where retail's editor had no fader at all.

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
- The screen fade is ported (§2.6): a closing screen with a fader runs
  on until black (`ReadyToEnd`); TPlayScreen and TLogoScreen fade.

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
`0x00537110/70` in-game menu open/close, `0x0047ebc0/ecc0/ed20/ece0`
drawer, `0x0047c580` SetControl; for the fade (§2.6): `0x00427e80`,
`0x00427f60`, `0x00493e40`, `0x00492d70`, `0x0048ead0/eaf0/eb00`,
`0x0048e8f0`, `0x0048f180`, `0x0048f340/450/560/680/760`, `0x00490bd0`,
`0x00491870`, `0x00491bf0..0x0049204e`, `0x0046cf40..0x0046cfc0`,
`0x0047a620`, `0x0047be00`, `0x0047ce80`, `0x0047cf40`, `0x0048e610`,
`0x0051d680`, `0x0054cbb0`, `0x0041b240`, `0x00539700..0x005397a1`, plus the existing TLogoScreen / TPlayScreen /
TDeathScreen / DefScreen decomps in `recon/discovered/`.
