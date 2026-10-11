# Play slot: the shipped game, played a tick at a time

The retail host of the playtest track (docs/playtest/PLAN.md §7,
PROTOCOL.md). The shipped Revenant.exe -- or a named build of it -- boots
by its own code on the HUD slot's machine, continues into play
(`QUICKSTART`), and runs one 24 Hz tick per `step` tick under a JSON Lines
control channel, deterministically: virtual time, a cooperative scheduler,
no host time or randomness. All game behaviour is retail's code; the host
supplies Windows and the boundaries listed below.

```sh
PY=/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python
"$PY" tools/retail_runtime/slots/play/server.py [--build build.json] [--machine software|hud]
                                               [--watch DIR] [--pace 24] [--work DIR]
```

then PROTOCOL.md §5 on stdin/stdout (`tools/playtest/host.py`'s
`RetailHost` launches it). The first run of a machine boots it (about
100-130 s) and caches the image; later runs load it in about 1.5 s.

## The machine

The HUD slot's (`slots/hud`: Windows 98, 64 MB, DirectDraw 4 / Direct3D 3
on RAM surfaces, the install read-only at `C:\REVENANT`), on one of two
Revenant.ini settings (`playworld.machine_ini`):

| `--machine` | INI | What retail draws |
|---|---|---|
| `software` (default) | the install's, `[Options] Software3D = Yes` | retail's software renderer, as a player without a 3D card saw it: NoTex path (`0x006680c8 = 1`), the map blitted in 2D, the 2D HUD, meshes by the internal rasterizer |
| `hud` | the install's | the HUD slot's declared hybrid (texture overlays on the internal rasterizer). Retail never ran this way: its internal rasterizer draws the map's ARGB1555 background texture (TSurface format `0x20000`) with its ARGB4444 span `0x0056bc30`, so the floor is noise |

A recording's header names its machine and `ini_sha`; replay refuses a
recording made on the other one.

## Start (`playworld.py`)

- **QUICKSTART.** Retail parses its command line once, in GetParameters
  `0x00483bc0`, called from WinMain before game init: `QUICKSTART[="name"]`
  sets `0x00668184` and copies the name to `0x0066603c`. Nothing reads
  either before WinMain `0x00486f8a`, after game init, where they pick
  TPlayScreen's start mode (`0x0047f4c0`: 0 new game, 1 load by name). The
  boot image stops at `0x00486e7f`, after game init, so the host writes the
  two parsed values there -- what a boot with `NOSOUND QUICKSTART[="name"]`
  holds -- and WinMain's copy of the line (`0x0066819c`). One boot image
  serves new games and every slot.
- **Activation.** Windows sends WM_ACTIVATEAPP(TRUE) when the fullscreen
  window shows; the HUD machine records ShowWindow without messages, so the
  slot posts it before the first frame (the WndProc keeps wParam in
  `0x006582c8`; TimerLoop pumps in a busy loop while it is 0).
- **Seed.** `start(seed=N)` calls retail's own srand `0x0058c575` on the
  main thread before WinMain continues (the CRT's rand is per thread). The
  boot's srand(time()) at `0x0048662d` saw the virtual clock, so without
  it the seed would be a constant too.
- **Slot start.** The bundle's `start/<slot>` is copied into
  `Save\Single\<slot>` (`overlay.place_slot`) before WinMain continues.
- `start` returns at the play screen's first tick wait: the module and
  the game are loaded inside TPlayScreen::Initialize. A new game's opening
  then plays under `step`, control off.

## The tick

`PlayWorld.step(events)`: post the tick's input, advance the virtual clock
by 1000/24 ms, fire retail's timer once, and run every guest thread until
the machine is quiet: the main thread waiting on the tick event
(`0x006687f0`) and no thread runnable.

- Retail's timer is `timeSetEvent(41 ms, TimerCallback 0x0049e2f0)`
  (clockticks `0x006687ec`, TickOccurred `0x006682a4`, PulseEvent). A guest
  thread stands in for winmm's (`mmtimer.py`): it waits on an event the
  host sets once per tick and calls the recorded callback.
- `GetTickCount` / `timeGetTime` are the virtual clock, so TimerLoop's
  frame-rate estimate (`0x0049133d`..`0x00491401`) never drops below 23.5
  and it never skips a frame: one tick is one TimerLoop pass that draws
  (TimerTick(1); verified by the screen's frame counter, +1 per step, in
  `test_playworld.py`).
- Long work inside a frame (loading a level) does not move virtual time:
  lockstep, as PLAN.md §5 asks.

## Input (`userinput.py`)

Traced, not assumed: keys and characters reach retail as WM_KEYDOWN / UP /
WM_CHAR through the WndProc `0x00448cc0`; mouse buttons as
WM_[LRM]BUTTONDOWN / UP (MouseClick `0x0048f870`, held mask `0x00668518`);
the pointer is polled with GetCursorPos every TimerTick (`0x00490d6f`),
the WndProc ignores WM_MOUSEMOVE (posted all the same); GetKeyboardState is
read only by TimerTick's idle test. Messages carry Windows' lParam bits
(scan code, extended, repeat, Alt context, transition) and MK_ flags; Alt
makes WM_SYSKEY*. The keyboard state follows the messages the game
removes, as Windows' does. TranslateMessage posts nothing: the recording
carries characters as their own events. DirectInput (and so the
joystick) stays absent, as in the HUD machine.

## Frames, files, observation

- Frames (`frames.py`): the primary surface, which retail presents to by
  blitting its back buffer; RGB565 to RGB888 by bit replication, PNG.
- Text (`gditext.py`): the HUD machine's GDI records TextOutA / DrawTextA
  and draws nothing, so retail's own text compose (`0x004be2b0`: a work
  surface keyed 0x7f7f, three black DrawTextA passes for the shadow, one
  in colour, then a keyed blit) had empty pixels. The play slot also draws
  the glyphs into the DC's surface: FreeType's (Pillow) hinted monochrome
  shapes from the same font files, placed by gdi.py's GDI metrics (the
  advances and ascent retail laid the text out with). Approximate, not
  Windows 98's rasterizer; it shows the dialog, the choices, the text bar
  and the status numbers as retail composes them.
- Files (`overlay.py`): writes land in memory over the read-only install,
  only under `Save\`, `CurMap\`, `ss.bmp` and INI files (anything else fails
  by name); they persist across steps and in images. `export_slot` copies
  `Save\Single\<slot>` out; `@last` is the slot the game wrote last.
  Created names keep their case (winfs), so the game's own slot scan sees
  "Quick Save 1".
- Observation and check lines (`observe.py`): memory reads at the
  documented layouts, each cited in the module, plus getters retail itself
  uses for computed values (maximum health / fatigue / mana, WeaponType)
  called through `PlayWorld.peek`, which refuses any call that makes an API
  call or draws a random number. Text-bar lines and speech are read where
  retail makes them (`TTextBar::AddLine` past its empty-line test
  `0x0054d0e3`, `TCharacter::Say` `0x004d0610`).
- `draws`: every call of the CRT rand `0x0058c582`, the game's random
  stream (random() `0x00483300` draws only when its bounds differ; 32
  other callers draw directly) -- what the port's RandomDraws() counts.

| Field | Read from |
|---|---|
| `screen` | CurrentScreen `0x00667fd0` against the screen objects; `menu` while the in-game menu's open flag `0x0066f788`, `dialog` while the dialog pane's response entry `0x00667cc8+0x190` |
| `control` | TPlayScreen `+0x5e0` (`0x0065d0d0`) |
| player | Player `0x00667fcc`: pos `+0x10/14/18`, level `+0x0e`, facing `+0x36`; mode from `root` `+0xe0`; stats from the player's copy (`+0x34c` count, `+0x350` {id, value}), ids from `0x0066ca4c` / `0x0066ca3c` / `0x0066ca38`; maxima through vtable `+0x1d8/+0x1e0/+0x1e8`; `doing` `+0xd8` (action `+0`, name `+4`); target = `root` `+0xe0` -> obj `+0x44` while root is combat (3) or bow (0x19); xp stats 17-19; skills stats 40-42 and 43 + WeaponType (vtable `+0x2cc`); weapon = equipment hand `+0x2b0`, mask `1 << WeaponType`; belt = inventory items (`+0x68` count, `+0x78` data) in slots `0x10b..` |
| `objects` | the loaded sectors (`0x00668578` count, `0x00668588` array; `+0x98` loaded, `+0xb8` count, `+0xc8` objects) on the player's level: id `+0x40`, name `+0x38`, type name `*(+0x4c)`, class `+0x04` (OBJCLASS_IDS.md), health and action as the player's |
| `screen` (player, objects), `project` | retail's WorldToScreen `0x0046d7a0` less the map pane's scroll origin `0x006668d8+0x78/+0x7c`; null off the pane |
| `pick` | retail's ScreenToWorld `0x0046dad0` at Locke's z + 50, as UpdateMouseMovement `0x0044ee00`; the object under the pointer needs the 3D pick, which draws a frame: not answered |
| `dialog` | the response entry's texts (`+0x5c` count, `+0x60` texts), the latest NPC entry's speaker and text |
| `messages` | text-bar lines since the previous `observe` |

`mode` is the root block's, as IsValidAttack's mode test reads it.
Not read (not verified): `gold`
(the MONEY amount stat), `quick_spells` (talisman codes at `+0x2cc`, not
names), `walkgrid`; `action_tick` only for an attack (the one retail
keeps as `lastattack` `+0x160`, started at GameFrame `+0x164`).

## Server (`server.py`)

PROTOCOL.md §5: hello, start, step, observe, project, pick, export_slot,
mark, replay (`verify`), checkpoint / restore (machine images on disk, with
the recording so far), caption, quit. The log on stderr is one line per
entry, `t=<tick> <kind> <detail>`: commands, input events, boundaries,
text-bar lines, speech. `--watch DIR` writes `frame.png` (zlib level 1,
written then renamed) after every tick and `caption.txt`; `--pace N` caps
ticks per wall-clock second. A `start` with an `ini` other than the
machine's is refused: retail reads the INI during game init, before play.

## Boundaries

The HUD machine's (boot log `0x004820b0`, network init `0x00575890`). The
opening reached no other: with NOSOUND no Miles call, no Smacker (the intro
movie is before QUICKSTART's branch). An in-game movie, DirectPlay or audio
call would stop the run by name.

## Measured (this Mac, 2026-10-10)

See the commit that adds the evidence; numbers there.

## Gaps

- Text glyphs are approximate (gditext.py): FreeType, not Win98 GDI.
- `pick` without the object; `gold`, `quick_spells`, `walkgrid` not read;
  `action_tick` for attacks only.
- The guest heap is 64 MB (`Runtime.HEAP_SIZE`); with block reuse a long
  session holds steady, but a level change has not been measured.
