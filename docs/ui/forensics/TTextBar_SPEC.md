# TTextBar — Reconstruction SPEC

The message feed over the bottom-left of the map view, plus the map-loading
progress bar drawn under its newest line. Retail leaf class `cls_0x5a5560`
(vtable `0x5a5560`), global singleton `0x65c5d0`.

Confidence marks: **C** confirmed in code (decomp or disassembly), **S**
confirmed by a retail screenshot, **U** unconfirmed.

---

## §0 — Sources & status

**Class:** `cls_0x5a5560` = **TTextBar** (vtable `0x005a5560`). Global instance
`0x0065c5d0`, wired by the global-ctor stub `FUN_00480725`. Identified by the
string `"Trouble initializing text bar"` at its `Initialize` call in
`TPlayScreen::Initialize` (`0x0047abf8`). **C**

**Port status (2026-10-05): ported.** `src/textbar.{h,cpp}` is the retail
class; `TPlayScreen` adds it as a production pane (§3.3); `--test=ui-textbar`
hosts the same pane with a scripted feed. Port mapping and deviations: §13.
Not ported: the typed-message prompt and the multiplayer chat feed (§10).

**Vtable / methods** (vtable dumped from `Revenant.exe` at `0x5a5560`, 35 slots;
overrides at 0/1/7/13/19/20/23/28, the rest inherited from `TPane`
`0x5a4494`). Slot roles follow the retail pane contract established for
`TDialogPane` (gameflow `DIALOG.md` §4.3): `+0x4c` per-tick pulse from the
screen's pane pass `0x0048fda0`, `+0x50` redraw from the draw pass
`0x0048ff00`, `+0x1c`/`+0x5c` the `NoTexOverlay` off/on draws.

| slot | addr | role | recon file |
|---|---|---|---|
| 0 | `0x54bf70` | `Initialize` | `recon/discovered/FUN_0054bf70_TTextBar_init.cpp` |
| 1 | `0x54c3d0` | `Close` | `cls_0x5a5560_TTextBar_Close_54c3d0.cpp` |
| 7 (`+0x1c`) | `0x54c600` | **Draw**, `NoTexOverlay` off: per-line alpha fade | `cls_0x5a5560_TTextBar_DrawOverlay_54c600.cpp` |
| 13 (`+0x34`) | `0x54c9c0` | **Hide** | `cls_0x5a5560_TTextBar_Hide_54c9c0.cpp` |
| 19 (`+0x4c`) | `0x54c460` | **Pulse**: age lines; multiplayer chat feed | `cls_0x5a5560_TTextBar_Pulse_54c460.cpp` |
| 20 (`+0x50`) | `0x54c440` | **redraw**: if dirty, `Composite(-1)`, `SetDirty(false)` | `cls_0x5a5560_TTextBar_Compose_54c440.cpp` |
| 23 (`+0x5c`) | `0x54c780` | **Draw**, `NoTexOverlay` on | `cls_0x5a5560_TTextBar_DrawPlain_54c780.cpp` |
| 28 (`+0x70`) | `0x54d4a0` | `CharPress`: the typed-message prompt | `cls_0x5a5560_TTextBar_CharPress_54d4a0.cpp` |
| — | `0x54cd40` | `Composite(index)`: render line `index` (−1: all) | `cls_0x5a5560_TTextBar_FUN_54cd40.cpp` |
| — | `0x54cb00` | `DrawHealthBar`: the strip under line 0 | `FUN_0054cb00_TTextBar_DrawHealthBlock.cpp` |
| — | `0x54ca20` | `SetHealthDisplay(name)` | `cls_0x5a5560_TTextBar_SetHealthDisplay_54ca20.cpp` |
| — | `0x54ca60` | `SetLevels(level, target)` | `cls_0x5a5560_TTextBar_SetLevels_54ca60.cpp` |
| — | `0x54cad0` | `ClearHealthDisplay()` | `cls_0x5a5560_TTextBar_ClearHealthDisplay_54cad0.cpp` |
| — | `0x54cbb0` | `DrawImmediate()`: line 0 straight to the display, mid-tick | `cls_0x5a5560_TTextBar_DrawImmediate_54cbb0.cpp` |
| — | `0x54d0c0` | `AddLine(type, color, text)` | `cls_0x5a5560_TTextBar_AddLine_54d0c0.cpp` |
| — | `0x54d170` | `Print(fmt, ...)` → `VPrint(1, …)` | `cls_0x5a5560_TTextBar_Print_54d170.cpp` |
| — | `0x54d190` | `Print(type, fmt, ...)` → `VPrint` | `cls_0x5a5560_TTextBar_PrintType_54d190.cpp` |
| — | `0x54d1b0` | `VPrint(type, fmt, va)` | `cls_0x5a5560_TTextBar_VPrint_54d1b0.cpp` |
| — | `0x54d2f0` | open the prompt | `cls_0x5a5560_TTextBar_BeginInput_54d2f0.cpp` |
| — | `0x54d390` | commit the prompt line | `cls_0x5a5560_TTextBar_CommitInput_54d390.cpp` |
| — | `0x54d700` | submit typed text (chat / `@` script / cheat words) | `cls_0x5a5560_TTextBar_SubmitInput_54d700.cpp` |

Helpers: `0x00438ed0` line text (`util_DrawLineText_438ed0.cpp`) →
`0x004be2b0` GDI text; `0x004bd8c0` `PutHue` (`util_PutHue_4bd8c0.cpp`) →
`0x004b21d0` hue-change blit (`util_HueChangeBlit_4b21d0.cpp`); `0x00521c60`
font-table lookup (`util_FontTableGetFont_521c60.cpp`).

Decomps were extracted with `DecompileBatch.java` from a copy of
`data/RevenantDev`; constants and the colour table were read from
`Revenant.exe` with `objdump`.

### §0.1 Corrections to the first version of this spec (2026-10-05)

| claim | retail | cite |
|---|---|---|
| up to `0x64` records, 9 visible | at most **9** lines: `+0x64` (offset 100) holds `DAT_005e5800` = 9; the buffer holds 12 (`0x450` bytes) | `0x54bf70`, `0x54d0c0` |
| the health bar shows the combat target | it is the **map-loading progress bar**; its one caller is the map loader | `0x004598c8`, §6.5 |
| Pulse feeds the combat target's name | that half feeds **multiplayer chat** (gated on the MP flag `DAT_0066829c`) | `0x54c460` |
| slot 7 Classic, slot 23 hi-res | `DAT_006680c8` is the ini's `NoTexOverlay`: slot 7 is the textured overlay path (fades), slot 23 the plain path | `DIALOG.md` §4.6, `0x00484d6c` |
| fg surface 12 lines, two 1-line bg surfaces | `+0x84` is the 1-line scratch; `+0x88`/`+0x8c` are 12 lines | `0x54bf70` |
| text cell top at (4, 9) | `9` is the **baseline** (cap tops 2 px under the line top) | §8, captures |
| colours: type 8 yellow, type 0x10 purple | type 8 red (255,40,40), type 0x10 sky blue (0,190,255) | §7 |
| `texthealthbar` in `PLAYSCRN.DAT` | `gamedata.dat` (the archive at `0x0065abc0`), entry 98 | `0x54cb82` |
| rect fixed by Initialize | `TPlayScreen`'s layout moves it (§3.3) | `0x0047bc50` |
| fallback rect 406×198, padding 14 | the ctor stub's rect is y 406, 198 × 14 (x from a register) | `0x00480725` |

---

## §1 — Overview

1. **Message feed.** `Print` formats a message and splits it at `'\n'`; each
   piece becomes a **line record** (type, colour, life in ticks, text). New
   lines go to the bottom; up to nine show, stacked upward. The **three
   newest never age**; an older line lives 120 ticks (5 s) from the moment
   it moves past them and fades over its last 24. **C** (code), **S** (retail
   captures: `docs/ui/sample_screen_1.jpg`, `_2.jpg` each show exactly three
   lines at the bottom-left of the map view).
2. **Loading bar.** While a map loads synchronously, the map loader
   (`MapPane.cpp`, `0x004597b0`) puts "Loading Map... Please Wait" on the
   bar as its newest line and draws the `texthealthbar` strip under it,
   sliding in and turning from red to green as the load progresses. **C**
3. **Typed-message prompt** (Enter, slot 28): a `"Message: "` line the player
   types into; in multiplayer it is chat, in single player `@` lines run a
   script line on the player and some words toggle cheats. **C**; not ported.

The 1998 pane (`textbar.cpp` before this port) showed one line of text, or the
name and health of the creature Locke was fighting. Retail replaced both; the
target's health went to `TPlyrStatusBar`.

---

## §2 — Asset roster

| asset | archive | entry | size | role | cite |
|---|---|---|---|---|---|
| `texthealthbar` | `gamedata.dat` (`GameData`, `0x0065abc0`) | idx 98 | **200 × 11**, flags `0x2` (RGB555), keycolor 0 | the loading strip; a green gradient, transparent (0) right-hand corner | `0x54cb82` `mov ecx,[0x65abc0]; push "texthealthbar"; call 0x46d710`; measured by TMulti parse |
| font `"Small"` | `font.def` | `WINFONT "Small" FONT "Arial" 12` | 12 px, `LEXTRA` 0 | all line text | `DAT_0065abc4` set at `0x485dcf` |

All 2112 opaque strip pixels are green-dominant, so the hue blit recolours the
whole strip (§6.4).

---

## §3 — Coordinate frames & surfaces

### §3.1 Line height

`lineH = font +0x50 (height) + font +0x54 (LEXTRA)` = 12 + 0 = **12** for
"Small" (`0x54bf70`; the same reading as the dialog pane's line, `DIALOG.md`
§4.2). Retail captures show a 12 px pitch. **C S**

### §3.2 Initialize's rect

```
pane.x = MapPane.x                       (DAT_006668dc)
pane.y = MapPane.y + MapPane.h − 9·lineH (DAT_006668e0 + DAT_006668e8 − …)
pane.w = MapPane.w                       (DAT_006668e4)
pane.h = 9·lineH                         (108)
```
At that point `TPlayScreen::Initialize` has made the map pane the whole display
(`0x0047aae6..0x0047aafd`). **C**

### §3.3 The play screen's layout

`TPlayScreen`'s layout code (`0x0047bc50`, in the drawer/panel handling of
its pulse, `cls_0x5a5320_TPlayScreen_Pulse_47b4d0.cpp`) re-anchors the bar
whenever it lays the panels out, if the bar is open:

```
newx = 0
newy = bottom pane top (0x00667c70) − pane.h
neww = display width − side tabs width (0x0065be6c) − side pane width (0x0066615c)
newh = pane.h
```
In Classic 640 × 480 with the side pane (188) and bottom pane (60) open:
**(0, 312) 400 × 108**. The side tabs strip is 52 px (`TSideTabsPane_SPEC.md`).
**C**

`TPlayScreen::Initialize` adds the bar after the side tabs and before the
player status bar (`0x0047ad63..0x0047adc0`: MapPane, DialogPane, `0x666140`,
`0x667c58`, SideTabs, **TextBar**, PlyrStatusBar), so it draws over the
dialog entries. **C**

### §3.4 Surfaces

| field | size | role | cite |
|---|---|---|---|
| `+0x84` | display width × lineH | one line's scratch: cleared, bar, text | `0x54bf70` (`FUN_004a5740(+0x70, lineH)`) |
| `+0x88` | display width × 12·lineH | lines by slot (software path) | `FUN_004bb5c0(+0x70, +0x74)` |
| `+0x8c` | display width × 12·lineH | lines by slot, drawn from | same |
| `+0x7c` | — | slot y of line 0 = surface height − lineH | `0x54c36b` |

Line `i` lives at slot y `(+0x7c − i·lineH) mod height`; the slots never roll,
so any change to the records recomposes every line. **C**

### §3.5 On screen

Line `i` (0 = newest) draws at screen `(pane.x, pane.y + pane.h − (i+1)·lineH)`,
`pane.w` wide (`0x54c600`: `y = +0x10 + +8 − lineH`, step `−lineH`; width
`+0xc`). Its text pen is `(4, 9)` inside the line: x 4, **baseline 9**. **C S**

```
map view (0,0)–(452,420), Classic
 …
 y 384 ┌ line 2 ─────────────────────────────┐  caps 386..392
 y 396 ├ line 1 ───────────────────────────── ┤  caps 398..404
 y 408 ├ line 0 (newest) ──────────────────── ┤  caps 410..416, bar at y 409
 y 420 └──────────── bottom pane ─────────────┘
       x 0 .. 400 (side tabs at 400..452)
```

---

## §4 — Line record

Stride `0x5c` (92) bytes (`0x54d0c0`, `0x54c460`, `0x54cd40`). **C**

| off | field | notes |
|---|---|---|
| `+0x00` | type | §7 |
| `+0x04` | colour | read for type `0x40` only; 0 → green |
| `+0x08` | life, ticks | 120 when added |
| `+0x0c` | text | `strncpy(…, 0x4f)`, NUL at `+0x5b` |

Pane fields: `+0x60` line count, `+0x64` max lines (9), `+0x68` first aging
line (3), `+0x6c` record buffer, `+0x70` display width, `+0x74` 12·lineH,
`+0x78` lineH, `+0x90` bar shown, `+0x94` "redraw the screen on the next
`DrawImmediate`", `+0x98` level, `+0x9c` target level, `+0xa0` prompt open,
`+0xa4` prompt room, `+0xd0` prompt text, `+0x120` last chat message seen.

---

## §5 — Draw order / composition

**Composite(index)** `0x54cd40`, for each line `i < count` with `index == −1`
or `index == i`:

1. colour from the type (§7);
2. clear `+0x84` (to `DAT_006668d0`);
3. if `index == 0` and the bar is shown: `DrawHealthBar` (§6.4);
4. text at (4, 9), font `"Small"`, flags `0x400` (`0x00438ed0` adds `0x80`,
   single line), drawmode `0x80000000`;
5. blit `+0x84` into slot `i` of `+0x88` (transparent `0x100` in the
   software path), then `+0x88`'s slot into `+0x8c`.

So **`Composite(−1)` never draws the bar**, and `Composite(0)` redraws only
line 0. Callers: `AddLine` and a dirty pane (slot 20) → `−1`; `SetLevels`,
`ClearHealthDisplay`, `Hide`, the prompt → `0`. (The function opens with a
`Box(40, 0, 40, 300, 0x997b)` on `+0x84`, which each line's clear wipes: a
leftover.) **C**

**Draw**, `NoTexOverlay` off (slot 7, `0x54c600`): bands of lines with equal
alpha, each a textured quad (`0x00414d70`) from `+0x8c` to the display,
diffuse `alpha << 24 | 0xffffff`, blend mode 4 when alpha < 255, else 2.
**Draw**, `NoTexOverlay` on (slot 23, `0x54c780`): bands drawn opaque through
`ParamBlit`, but only those with alpha > `0x80`. Slot 23 then runs the UI blit-effect pass `0x004aacb0`
over the pane when `DAT_005d7a18` is 0. Both clear `+0x94`. **C**

---

## §6 — Algorithms

### §6.1 VPrint — `0x54d1b0`

```
if !open: return
if TEXTDUMP (DAT_00668178, the -TEXTDUMP switch): append the message to
    "<path>TextDump.txt", with a "Revenant Text Dump executed at" header once
vsprintf(buf[256], fmt, va)
for each '\n' in buf: cut there; AddLine(type, 0, piece)
if the rest is non-empty: AddLine(type, 0, buf)      # buf, not the rest (retail bug, §11)
```
`Print(fmt, …)` passes type 1; `Print(type, fmt, …)` its type. **C**

### §6.2 AddLine — `0x54d0c0`

```
if !text or text[0] in {'\0', ' '}: return            # a leading space drops the line
slot = prompt open ? 1 : 0
memmove(records + slot + 1, records + slot, (11 − slot) records)
count = min(count + 1, 9)
records[slot] = { type, colour, 120, text[:79] }
Composite(−1)
```
**C**

### §6.3 Pulse — `0x54c460`

```
for i = count − 1 down to 3:                          # +0x68 = 3: lines 0..2 never age
    records[i].life −= 1
    if records[i].life < 1 and i == count − 1: count −= 1
if multiplayer and a player:                          # not ported
    for each chat message since +0x120 (FUN_00570760):
        skip our own; sender < 0 → AddLine(8, …)
        else AddLine(0x40, team colour (0x005e20b8[min(team, 16)]) or green, text), sound "viles"
```
Older lines start at higher indices, so the tail always expires first. **C**

### §6.4 DrawHealthBar — `0x54cb00`

```
step level toward target: |Δ| < 5 → level = target; else ±4
hue = level · 155 / 176;  hue = hue < 17 ? 0 : hue − 16
PutHue(+0x84, x = min(0, level − 186), y = 1, texthealthbar, 0x100, hue)
```
`PutHue` `0x004bd8c0` blits with `drawmode | 0x100000` (DM_CHANGEHUE), colour
= hue. The hue blit `0x004b21d0` matches the 1998 `PutHueChange`
(`graphics.cpp`): for each non-zero pixel whose green (5-bit × 8) exceeds red
and blue, `v = g / 255`, `s = (g − min(r, b)) / g`, and HSV → RGB with the new
hue, terms truncated (`__ftol`); other pixels unchanged; 0 is skipped under
`0x100`. The strip runs red (empty) → green (186: hue 147). **C**

### §6.5 The loading-bar API

| call | effect | cite |
|---|---|---|
| `SetHealthDisplay(name)` | `+0x90 = +0x94 = 1`; `AddLine(0x80, 0, name)` | `0x54ca20` |
| `SetLevels(level, target)` | if `+0x90 == 0`: `+0x90 = +0x94 = 1`, `AddLine(0x80, 0, GetLine("loadmsg"))`; set both; `Composite(0)` | `0x54ca60` |
| `ClearHealthDisplay()` | `+0x90 = +0x98 = +0x9c = 0`; `records[0].type = 1`; `Composite(0)` | `0x54cad0` |
| `DrawImmediate()` | open, shown, bar up: if `+0x94`, redraw the screen (`CurrentScreen` slot 18) and clear it; else draw line 0's band at the display's bottom, flip | `0x54cbb0` |

Only caller, the map loader `0x004597b0` (gated on: bar open, not hidden, on
screen, screen frame count > 0): `SetHealthDisplay(GetLine("loadmapmsg"))`,
`DrawImmediate`, `PutToScreen`; per step the callback `0x00459a00`:
`SetLevels(p · 180 / 1000, same)` (p in ‰), `DrawImmediate`, `PutToScreen`;
at the end `ClearHealthDisplay`, `DrawImmediate`, `PutToScreen`. After a load
"Loading Map... Please Wait" stays in the feed as an ordinary line (sample
screenshot 1 shows it among the three lines). **C S**

### §6.6 Hide — `0x54c9c0`

```
prompt off; records[0].type = 0x40; Composite(0)
SubmitInput(+0xd0)                    # whatever was typed runs (§11)
bar off, level = target = 0; records[0].type = 1; Composite(0)
hidden = ignoreinput = 1
```
Called from `TPlayScreen`'s panel layout when its drawer state (`+0x6c4`)
is 0 or 3 (`0x0047b874`, `0x0047b8d0`); the other branch shows it again
(hidden and ignore-input cleared, then slot 10, `0x0047b96b`). **C**

---

## §7 — Colours

Static initializers `0x0054be70..0x0054bf68` build the table as bytes
`[0] [1] [2]`; the GDI text call swaps bytes 0 and 2 into its COLORREF
(`0x004be2b0`), so byte 2 is red. As `0x00RRGGBB`:

| type | global | bytes | RGB | retail callers |
|---|---|---|---|---|
| 1 (and any unlisted value) | `0x0067064c` | `00 c8 ff` | **(255,200,0)** gold | `Print(fmt, …)`: 111 call sites |
| 2 | `0x00670664` | `ff 00 b4` | (180,0,255) violet | none |
| 4 | `0x00670654` | `b4 00 ff` | (255,0,180) pink | none |
| 8 | `0x00670668` | `28 28 ff` | (255,40,40) red | multiplayer server lines |
| `0x10` | `0x0067065c` | `ff be 00` | (0,190,255) sky blue | `Print(0x10, ITEMTOFAR)` "You are too far away." (`0x0044ff38`) |
| `0x20` | `0x00670658` | `ff ff ff` | white | the prompt line |
| `0x40` | record `+0x04`, else `0x00670660` | `00 d2 00` | the line's own, else (0,210,0) green | chat (`0x00464daf`, `0x0046ca68`, Pulse), the committed prompt |
| `0x80` | `0x00670650` | `ff ff ff` | white | `SetHealthDisplay`, `SetLevels` |

**C.** The sample screenshots show the lines pale peach with magenta fringes
(the retail pink-halo artifact over JPEG); the code says gold. **U** (S12,
question 51).

**Shadow:** flags `0x400` select the 3-pass black shadow (base, +1 x, +1 y)
under the coloured pass (UI_METHOD_MAP §5), as the dialog's `0x401`. **C**

---

## §8 — Text rendering

| string | cell | font | colour | align | shadow | cite |
|---|---|---|---|---|---|---|
| each line | line-local pen (4, 9): **9 is the baseline** | "Small" (Arial 12) | §7 | left, single line (`0x80`) | yes | `0x54ce94` push 9, push 4 → `0x00438ed0` → `0x004be2b0(x, y, 10000, 10000, …)` |

Baseline: both sample captures put each line's cap tops 2 px and its last cap
row 8 px under the line's top (lines at y 384, 396, 408; caps 386–392,
398–404, 410–416), x from 4. GDI's Arial 12 has ascent 10, so the cell top is
y − 10 = −1: the single-line path of `0x004be2b0` positions by baseline. The
mechanism inside `0x004be2b0` is **U**; the placement is **S**.

---

## §9 — Animation

```
line life:  120 → 0, −1 per tick, only for lines 3..8          (0x54c460)
alpha:      life < 24 ? life · 255 / 24 : 255                   (0x54c600)
            NoTexOverlay on: drawn only while alpha > 128       (0x54c780)
bar level:  ±4 per Composite(0) toward target, snap within 4    (0x54cb00)
```
The three newest lines stay until pushed down; then each holds 4 s and fades
for 1 s.

---

## §10 — Input

The typed-message prompt. **C** throughout (decomp and asm of the four
functions below; the cheat effects' callees named from their own bodies).

State: `+0xa0` prompt open, `+0xa4` room (80 − prefix length), `+0xd0` the
typed text (80 bytes). Nothing clears `+0xd0` but BeginInput.

**CharPress** `0x54d4a0` (slot 28; WM_CHAR, so Enter is 13 and Backspace 8):
```
gate = control on (0x0065d0d0, PlayScreen +0x5e0) and not
       (single player and Player's root action COMBAT or BOW)
if !gate:
    if prompt closed: return
    CommitInput(); return                     // any char commits what is typed
if !down: return
if prompt closed:
    if ch != 13: return
    BeginInput(); return                      // inlined copy of 0x54d2f0
if ch == 13: CommitInput(); return
if ch == 8: drop the last byte (two for a DBCS pair)
else if 32 <= ch < 256: append ch if room (+0xa4) allows
(else, a control char: nothing)
records[0].text = GetLine("msgprefix") + typed; Composite(0)
```
**BeginInput** `0x54d2f0`: only when closed. `ControlMap.ReleaseAll()`
(`0x65a9c8 |= 0x65a9c4; 0x65a9c4 = 0`); with a Player: `Stop(0)`
`0x4cee70` and `SetWalkMode()` `0x4cf000`; room = 80 − strlen(prefix);
typed = ""; `AddLine(0x20, 0, GetLine("msgprefix"))` ("Message: "); open.

**AddLine while open** puts the new line second, under the prompt (§6.2).

**CommitInput** `0x54d390`: only when open. Closes; `records[0]` becomes
type `0x40` in the player's colour (multiplayer slot colour
`0x5e20b8[min(+0x4d8, 16)]` when `Player+0x494`, else the chat green
`0x00670660`), text "<Player name>: <typed>" (79 chars at most);
`Composite(0)`; `SubmitInput(typed)`. Other callers: `0x47d0d5`,
`0x47d136` (TPlayScreen) and `0x4d3d2d` (BeginFighting): starting a fight
commits the line.

**TPlayScreen::KeyPress** `0x47c630` returns at once while the prompt is
open (`0x47c63e`: `[0x65c670]`, which is TextBar `+0xa0`): no panes, no
hotkeys, no Escape. Escape can't cancel the prompt; only Enter (or a
gate change) ends it.

**SubmitInput** `0x54d700`, `text`:
- multiplayer: send as chat (`0x5701f0`); done.
- `@` with a Player: the rest runs through CommandInterpreter
  `0x41e8e0`(Player, a string stream over it, 1, 0); done.
- otherwise a cheat word (stricmp), each followed by the line
  `GetLine("cheatenabled")` ("Cheat Enabled") or `"cheatdisabled"` and the
  sound `potionmix` (volume 0x7f). Any other text: nothing at all.

| word | effect | line |
|---|---|---|
| `alreadydead` | toggle `0x668104`: nothing hurts the player | by state |
| `alchemy` | Player: SetMoney(999999) `0x51e900` (the `gold` item set to that amount, added if missing) | enabled |
| `nahkranoth` | toggle `0x668108`: the player's blows land and kill | by state |
| `noamnesia` | Player: SetLevel(30) (slot `0x358`), then SetAttackLevel(Level()) (slot `0x370`, stat `AttackLevel`) | enabled |
| `lookunderthehood` | toggle `0x66812c` | by state |
| `dummies` | toggle `0x668110`: monster AI off | by state |
| `abracadabra` | toggle `0x66810c` (casts cost no mana, can't fail); with a Player: a `spell pouch` (added if missing; the Player itself if none can be had) gets one of every TALISMAN type, moving any already carried elsewhere into it; every spell variant's talisman code is learned (`0x544fb0`); belt, side-tab and spellbook panes flagged dirty (`0x65b078`, `0x65d548`, `0x65aa28`) | by `0x66810c` |
| `potionsnlotions` | every POTION type (class at `0x66d268`): 5 added if missing, else its amount raised to 5 | enabled |
| `gimmesomegrub` | every FOOD type (class at `0x66d2a8`): as above | enabled |
| `debug` | `0x668130 = 0x66812c = !0x668130`: the developer hotkeys in TPlayScreen::KeyPress (DebugOverlay_SPEC §5) | by state |

`alchemy` and `noamnesia` with no Player still say "Cheat Enabled".

**Hide** `0x54c9c0` closes the prompt and submits `+0xd0` whether or not
it was open (§6.6, §11.3).

---

## §11 — Retail bugs

1. **Pink halo** on shadowed text (magenta-keyed scratch + antialiased GDI
   edges). Not reproduced: glyphs carry real alpha.
2. **Print's last piece.** After a `'\n'`, the trailing piece is added as the
   buffer's start (`0x0054d2ba`: `lea eax,[esp+0xd0]`), so "A\nB" shows "A"
   twice. The port adds the trailing piece (question 53).
3. **Hide submits the prompt** (§6.6), open or not. After a commit the
   buffer still holds the last line, so each Hide (the bottom panel's
   drawer opening or closing) runs the last cheat again, toggling it back.
   The port submits only an open prompt.

---

## §12 — Reconstruction

`src/textbar.{h,cpp}`:

```
Initialize   font "Small" atlas, lineH = height + LEXTRA, texthealthbar;
             LayOut; TPane::Initialize; empty feed; RecomposeAll
Pulse        LayOut; age lines 3..count−1, pop the expired tail     (slot 19)
Compose      if dirty: RecomposeAll, clean                           (slot 20)
             if a composition is pending: every line into the RT,
             the strip under line 0 when barshown
Draw         line i: RT slot i → (x, bottom − (i+1)·lineH), w = pane w,
             alpha from life (eased between ticks)                   (slot 7)
RecomposeAll   barshown = false; pending          (Composite(−1))
RecomposeFirst step level if the bar is up; barshown = bar up; pending (Composite(0))
```

---

## §13 — Port mapping and deviations

| retail | port | home |
|---|---|---|
| `0x00438ed0` → `0x004be2b0` text, flags `0x480`, shadow | `DrawTextShadowedAtBaseline(font, text, 4, slot + 9, …)` | `font.cpp` |
| "Small" WINFONT, Arial 12 | `FontTable->Atlas("Small")` = Arimo 12 (Arial-metric) | `fonttable.cpp` |
| `PutHue` → hue blit `0x004b21d0` | `DecodeBitmapHueChangedToRGBA` into a streamed texture, re-decoded when the hue changes; `Renderer->Composite` into the RT | `bitmapdecode.cpp`, `textbar.cpp` |
| `+0x84` / `+0x88` / `+0x8c` mosaic surfaces | one RGBA render target, display width × 9 slots, each slot `lineH + 8` rows (4 above, 4 below) so shadows and descenders stay inside their slot, as retail's 1-line scratch clips them | `textbar.cpp` |
| `Composite` called inline | `RecomposeAll` / `RecomposeFirst` record the outcome (`barshown`) and bump a request counter; `Compose` renders the latest outcome once per frame | `textbar.cpp` |
| slot 7 quads with `alpha << 24 \| 0xffffff` | `DrawSurfaceSubrectTinted(…, 1, 1, 1, alpha)` per line, in the screen's pane layer | `textbar.cpp` |
| `TPlayScreen` layout `0x0047bc50` | `TTextBar::LayOut` each pulse from `PlayScreen.GetMapViewRect()`: (map x, map bottom − 9·lineH, map w − 52, 9·lineH), as `TDialogPane` lays itself out | `textbar.cpp` |
| `TPlayScreen::Initialize` `0x0047abf8` / `0x0047adab` | `TextBar.Initialize()` + `AddPane(&TextBar)` after the HUD harness (so the map view is the HUD's); `RemovePane` + `Close` in `TPlayScreen::Close` | `playscreen.cpp` |

Deviations:

- **Alpha eased between ticks** for the aging lines (frame-rate-independent
  rule); retail steps it per tick in 255/24 steps.
- **`NoTexOverlay` on** (slot 23) is not drawn; the port draws the overlay
  path, as the dialog pane does.
- **Print** logs every message as `[textbar] <text>` at debug level, even
  while the bar is closed, instead of the `TEXTDUMP` file; the 256-byte
  buffer is bounded (`vsnprintf`); the trailing piece after a `'\n'` is added
  (§11.2).
- **`Clear()`** is a port call (empty feed, bar off: what Initialize leaves);
  retail has none, and nothing calls it yet. The level loader,
  `TGameSession::EnterLevel`, drives the bar as retail's loader does (§6.5):
  `SetHealthDisplay(LOADMAPMSG)`, `SetLevels(p · 180 / 1000)` per slice,
  `ClearHealthDisplay`, which leaves the line in the feed.
- **`DrawImmediate`** (`0x54cbb0`) is not ported: the port never blocks a
  frame to load; the level loads a slice per frame instead, with the world
  held (EXITS.md §7). `TPane::DrawImmediate`/`PutToScreen` remain for the unused
  `TSector::LoadPreloadSectors`.
- **Hide** submits only an open prompt (§11.3).
- **Backspace** comes from macOS as DEL (0x7f); the event layer hands it
  on as 8, the WM_CHAR value retail reads.
- **The 1998 combat readout is gone.** `TCharacter::Pulse` no longer calls
  `SetHealthDisplay(name, health)` each tick; retail's `SetHealthDisplay` has
  one caller, the map loader.
- **Not ported:** the multiplayer chat (SubmitInput's first branch, the
  feed in §6.3) and DBCS input (the two-byte append and Backspace).

Prompt (2026-10-10, from the post-opening save, `char enter` / `type` in
the input script): "Message: alreadydead" in white while typing; Enter
gives "Locke: alreadydead" in green and "Cheat Enabled" in gold;
`@addinv "short sword"` adds a sword to the pack; Backspace trims
"abcx" to "abc", which commits with no answer; `alchemy` sets the gold to
999999, `noamnesia` Locke's level to 30, `potionsnlotions` five of each
potion.

Verification (2026-10-05): `--test=ui-textbar --headless --filmstrip=14,1`
(stacking, gold/sky-blue/white colours, shadow over a mid-tone backdrop, the
strip sliding in green under "Loading Map...", the loading line turning
gold, older lines fading, three left); in game, `ressexit` locked: "It seems
to be locked" under "Locke entered The Keep" at x 4, caps 398–404 / 410–416,
matching the retail captures row for row.

---

## §14 — Open questions

1. **The baseline mechanism** in `0x004be2b0`'s single-line path (§8). The
   placement is settled by captures; the code path is not traced.
2. **Message colour on screen** (§7): gold in code, pale in the JPEG
   captures. Screenshot S12.
3. **`+0x94`'s screen redraw** in `DrawImmediate` calls `CurrentScreen`
   slot 18 (`0x00491870`); its role is unconfirmed. Not ported.
4. **`DAT_006668d0`**, the line surface's clear colour, is unread (likely
   the transparent key). The port clears to transparent.
