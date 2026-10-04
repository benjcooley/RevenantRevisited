# LoadDef_selstart — Reconstruction SPEC

> Forensic reconstruction contract for the **Multiplayer Select-Start** DEF
> screen (`selstart.def`). Class = `cls_0x5a4e24` (vftable @ `0x005a4e24`,
> singleton instance @ `DAT_006597c0`). Produced per
> [FORENSICS_PROTOCOL.md](FORENSICS_PROTOCOL.md). Terms per
> [NOMENCLATURE.md](NOMENCLATURE.md). Primitive behavior cited from
> [UI_METHOD_MAP.md](UI_METHOD_MAP.md).
>
> All paths are relative to repo root
> `/Users/benjamincooley/projects/RevenantRevisited/worktrees/ui`. Every number
> states its coordinate space and cites `file:line` + `hex (= dec)`.

---

## §0 — Sources & status

**Status:** `forensics-complete` for the envelope (paint inputs, asset roster,
DEF body, instance/visibility predicate, command dispatch shape, frame/element
table). **Forensics-partial** for two slot bodies that Ghidra did not emit into
`recon/ghidra/cls_0x5a4e24.cpp`: `virt_meth_0x469460` (slot 148 — the
**Apply/OnControl** dispatcher that populates `startmsg` text, hides the OK
button on no-character branches, and routes the OK/Cancel button clicks) and
`virt_meth_0x469ab0` (a non-vtable sibling helper called from the host gameflow
code at 0x469ab0). The body of slot 148 is INFERRED from its string-XREF set in
`recon/ghidra/_data.txt:77910-77975`; verbatim per-instruction dispatch is
UNCONFIRMED-A. The visible widgets, layout, fonts, colors, frames, and chrome
ARE pinned.

**CORRECTION TO TASK BRIEF (decisive — load-bearing).** The task brief
describes selstart as the **SP character-pick screen before NewGame**. This is
**incorrect**. selstart is the **MP host's pick-a-start-position screen** —
opened only when MP mode is active. Three independent confirmations:

1. **Gating predicate at the call site**
   (`recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp:392-404`):
   the only invocation of `FUN_00469370_LoadDef_selstart()` is inside an `if`
   whose first conjunct is `DAT_0066829c != 0`. `DAT_0066829c` is the MP-mode
   flag — set to 1 by the same Initialize body at `:23`
   (`DAT_0066829c = (*(int*)(param_1+0x6d8) == 3)` — `0x6d8` is TPlayScreen's
   screen-mode field; values are `0=SPNewGame / 1=SPLoad / 2=Editor / 3=MP`
   per the cascade at `:269-300`). When `DAT_0066829c == 0` (SP), the gate
   short-circuits and selstart is never opened.
2. **DEF file header**
   (`/Users/benjamincooley/projects/RevenantRevisited/RevenantRevisited/data/resources_unzipped/selstart.def:2`):
   `"// ****** Revenant MP Select Start DEF File  *******"`.
3. **Chrome bitmap**: the panel's `Background` sprite (sole entry in
   `selstartnotex.dat` / `selstarttex.dat`, 394×316) has the literal title
   **"MULTIPLAYER"** rendered onto the chrome. Visual confirmation:
   `/tmp/selstart_dump_notex/00_Background.png`.

The SP "character pick" the brief is thinking of is the `createchar` screen
(`cls_0x5a4d6c` / `createchar.def` — see
[CharCreate_SPEC.md](CharCreate_SPEC.md)). selstart sits in a **different
gameflow position**: in MP, after `LoadNewGame` runs and the players join,
each session selects which of the area's pre-placed `STARTPOS` records they
will spawn at. This spec covers that screen.

### Class identity (HIGH confidence)

| fact | evidence |
|---|---|
| **Class id** = `cls_0x5a4e24` (vftable @ `0x005a4e24`, 46-slot vtable, 396-byte struct) | `recon/ghidra/cls_0x5a4e24__vftable_5a4e24.cpp:1-56`; `recon/ghidra/cls_0x5a4e24.cpp:4-26` |
| **Inherits** from `cls_0x5b93c4` (the DEF-screen popup base, shared with TDeathPane / cls_0x5b9480 / cls_0x5b9584 / cls_0x5b9744 / cls_0x5b963c — the popup/menu family) | ctor passes `(cls_0x5b93c4*)this` to base helpers (`recon/ghidra/cls_0x5a4e24.cpp:37-39, 60-61, 74, 95-97`); vtable shape matches the DEF-screen base shape used by InGameMenuDef and LoadGameDef |
| **Singleton instance** @ `DAT_006597c0` (0x18c = 396 bytes) | XREF set in `recon/ghidra/_data.txt:128609-128627`; passed via `&DAT_006597c0` to the modal-pump bootstrap at `cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp:400-403` (`FUN_0048ed90(&DAT_006597c0,0xffffffff)` then `FUN_0048ed60(&DAT_006597c0)` then `FUN_0048eea0(uVar2,7)`) |
| **DEF asset** = `selstart.def` (PANEL `"default"`) | `recon/discovered/FUN_00469370_LoadDef_selstart.cpp:8-9` passes `s_selstart_005d3bc4` (the DEF filename root) and the literal `Background` sprite is in `selstart{tex,notex}.dat` |
| **Loader / class-Open** = `FUN_00469370_LoadDef_selstart` @ `0x00469370` = `cls_0x5a4e24::virt_meth_0x469370` (vtable slot 0) | `recon/discovered/FUN_00469370_LoadDef_selstart.cpp` (free-form) + `recon/ghidra/cls_0x5a4e24.cpp:32-48` (class-form, identical body) |

### Retail addresses + recon files

| Slot | Method (role) | Addr | Recon file:line |
|---|---|---|---|
| 0 | `Open` (LoadDef_selstart — DefScreen_Open + member init) | `0x00469370` | `recon/discovered/FUN_00469370_LoadDef_selstart.cpp:1-19`, `recon/ghidra/cls_0x5a4e24.cpp:32-48` |
| 76 | `Update` (per-frame; auto-randomizes start-pos if `DAT_00668128` is set) | `0x00469890` | `recon/ghidra/cls_0x5a4e24.cpp:114-130` |
| 108 | `OnInput` (keyboard arrow-key listbox advance) | `0x004693e0` | `recon/ghidra/cls_0x5a4e24.cpp:54-76` |
| 124 | `~Destructor` (vector-delete thunk) | `0x0046d2b0` | `recon/ghidra/cls_0x5a4e24.cpp:161-171` |
| 132 | `DefScreen_Open` (inherited) | `0x00435150` | `recon/discovered/FUN_00435150_DefScreen_Open.cpp` |
| **148** | **`Apply/OnControl`** — populate `startmsg`, hide/show `ok`, dispatch `ok`/`cancel` button clicks | `0x00469460` | **NOT in `recon/ghidra/cls_0x5a4e24.cpp`** — inferred from string XREFs at `recon/ghidra/_data.txt:77910-77975` and 128336/128337/128775 (DAT_00658da0 + DAT_0067680c reads, DAT_005d3c54 "ok" XREF) — body UNCONFIRMED-A |
| 164 | `OnFieldQuery` (FIELD value getter — returns formatted start-pos NAME for the listbox) | `0x004697a0` | `recon/ghidra/cls_0x5a4e24.cpp:82-117` |
| — | `~cls_0x5a4e24` (real dtor) | `0x0046d2d0` | `recon/ghidra/cls_0x5a4e24.cpp:177-195` |
| — | `cls_0x5a4e24` (ctor) | `0x0046d250` | `recon/ghidra/cls_0x5a4e24.cpp:136-155` |

### Other recon files read

- `recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp:23, 269-300, 392-404` — TPlayScreen's screen-mode cascade (where `DAT_0066829c` is set) and the only call site of `FUN_00469370_LoadDef_selstart`.
- `recon/discovered/FUN_00435150_DefScreen_Open.cpp` — the DefScreen-base Open shim that the selstart Open delegates to.
- `recon/discovered/FUN_00435040_DefScreen_LoadAndShow.cpp` — the LoadAndShow inner (loads `"background"` sprite, parses `<panel>.def`, calls Activate).
- `recon/discovered/FUN_00435150_DefScreen_Open.cpp`, `recon/classes/cls_0x5b93c4.cpp:81-143, 222-256, 260-295, 330-360` — DefScreen base paint walk (`virt_meth_0x435cb0` slot 140 = paint, `virt_meth_0x435d70` slot 76-base = update, `virt_meth_0x436090` slot 92 = per-child draw — see [InGameMenuDef_SPEC.md §5](InGameMenuDef_SPEC.md) for the canonical DEF paint walk this screen inherits).
- `recon/ghidra/cls_0x4609f0.cpp:180-210` — `meth_0x460ca0` (lookup-by-index on the per-area STARTPOS registry); used by selstart's listbox to read start-pos records.
- `recon/ghidra/cls_0x5a3d44.cpp:560-589` — `meth_0x430b80` = `TDefListbox::SetSelection(idx)`; the LISTBOX widget class used by `startlist`.
- `recon/ghidra/cls_0x5b98b8.cpp:910-985` — `meth_0x436900` (GetWidget by index), `meth_0x436930` (FindWidgetIndexByName), `meth_0x436980` (FindWidgetByName→ptr); used by the OnInput slot 108 and Update slot 76 to grab the listbox widget by name.
- `recon/ghidra/_data.txt:77904-78010, 128336-128338, 128609-128627, 128775` — string XREFs naming every widget and global the four selstart-specific virtuals reference.
- `RevenantRevisited/data/resources_unzipped/selstart.def` — the literal on-disk DEF source, fully reproduced in §2.1.
- `RevenantRevisited/data/resources_unzipped/widgets.def:81-145` — the STYLE defaults the selstart controls inherit from.
- Asset dump:
  `python3 tools/ui/dump_dat.py /Users/benjamincooley/projects/RevenantRevisited/RevenantRevisited/data/resources_unzipped/selstartnotex.dat --out-dir /tmp/selstart_dump_notex`
  → 1 entry `Background` 394×316 flags=0x402 kc=0x0 (`/tmp/selstart_dump_notex/00_Background.png`).
  `selstarttex.dat` reports the same dims with a different flags word (0x10400 = `BM_COMPRESSED | BM_5BITPAL` — unsupported by `dump_dat.py`; the texture-pack variant is for the high-res renderer's mipmap branch and not the Classic paint path — UNCONFIRMED-D).
  `widgetsnotex.dat` (the shared widgets pack) → 39 entries; used for BUTTON/FRAME/LISTBOX chrome (see §2).
- Visual cross-check: `Background` PNG shows the literal **"MULTIPLAYER"** title art, confirming MP scope (§0).

### Errors to close in existing ports
None. There is no existing `src/selstart*` shell — greenfield.

### Snapshot note (supplementary — superseded by retail)
There is **no snapshot `src/selstart*`** in any of `RevenantRepo/`, `Classes_*`,
or `worktrees/ui/src/`. The retail design (DEF-screen popup + listbox-of-startpos)
appears nowhere in the 1998/99 source — MP networking and the selstart screen
were post-snapshot work. The DEF file in `data/resources_unzipped/selstart.def`
is the authoritative shipped data and is treated as definitive.

---

## §1 — Overview

The Select-Start screen is the **MP host's pick-start-position modal**, shown
during MP boot-up after the player object exists and the area's `STARTPOS`
records are loaded. The screen presents:

- A **chrome popup** (394×316 "MULTIPLAYER"-titled bitmap) anchored at screen
  `(126, 65)` — the same anchor as the in-game menu — i.e. visually centered
  on a 640×480 stage (with the same small `(+3, −17)` literal-vs-true-centre
  offset called out in [InGameMenuDef_SPEC.md §3](InGameMenuDef_SPEC.md), since
  the popup family shares the 394×316 chrome size).
- A short **`startmsg` TEXT** widget at pane-local `(32, 60, 306, 72)`
  displaying a localized "pick a start position" message. The exact message
  text is computed at populate-time by `virt_meth_0x469460` (slot 148) based on
  module-flag globals `DAT_0067680c` / `DAT_00676810` / `DAT_00676814`,
  selecting among the STRINGREF keys `MPNOPENALTY`, `MPPENALTY`,
  `MPRESURRECT`, `MPSTARTGAME` (string XREFs at `_data.txt:77920-77951`).
- A **`startlist` LISTBOX** (vertical, 255×84 at pane-local `(60, 145)`) of the
  current area's STARTPOS records. The listbox's FIELD binding is `startpos`,
  resolved by `virt_meth_0x4697a0` (slot 164) on a per-row callback. Each row's
  display string is `"%s (%s)"` — start-pos name + area/module name, format
  literal `s__s___s__005d3c88` = `"%s (%s)"` at `_data.txt:77985`.
- Two **BUTTONs** at the bottom: `ok` (pane-local `(84, 240, 93, 27)`,
  TEXT `"Ok"`) and `cancel` (pane-local `(201, 240, 93, 27)`, TEXT `"Cancel"`).
  The string `"ok"` appears twice as raw bytes (`DAT_005d3c54` and
  `DAT_005d3ca4`, `_data.txt:77960, 78002`) — these are the literal NAME
  strings the dispatcher (slot 148) and Update (slot 76) compare widget names
  against to decide which button to hide / which button to fire.

When opened, the screen freezes the play scene and modally pumps. The user picks
a STARTPOS in the listbox, clicks **Ok**, and the chosen start-pos record is
applied as the player's spawn point. **Cancel** is also wired in retail
(string XREF `s_cancel_005d3c6c`), though the host-only context means it
typically just resigns the MP join.

**When shown / hidden.**
- **Shown** (sole call site): `recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp:398-403`:
  ```c
  if (   DAT_0066829c != 0                                  // MP mode
      && DAT_00667fcc != 0                                  // player obj exists
      && ((DAT_00667fcc[0xdb] & 0x100000U) == 0 || level==0)
      && DAT_0066829c != 0 && DAT_00659800 == 0             // not "newgame-in-progress"
      && (level==0 || (DAT_00667fcc[0xdb] & 0x100000U) != 0))
  {
      FUN_0047c580(0);                                       // stop frame swap
      FUN_00469370_LoadDef_selstart();                       // class Open
      FUN_0048ed90(&DAT_006597c0, 0xffffffff);               // register modal
      FUN_0048eea0(FUN_0048ed60(&DAT_006597c0), 7);          // pump w/ priority 7
  }
  ```
  The condition `level < 1` is `(**vtable+0x1c0)()` (the **Health** getter,
  per UI_METHOD_MAP §15b) — the screen opens when the player is dead/un-spawned
  OR the world flag bit `0x100000` is set (UNCONFIRMED — likely the "needs to
  pick start position" world bit).
- **Hidden**: the OK / Cancel button click (handled in `virt_meth_0x469460`
  slot 148) sets `this->mbr_0x5c` (the return code) and invokes
  `cls_0x5b98b8::virt_meth_0x434f30` (TPane::Close — `recon/ghidra/cls_0x5a4e24.cpp:199`)
  via the modal pump. (UNCONFIRMED-A: exact button → mbr_0x5c value mapping —
  by analogy with InGameMenuDef §6.4, "ok" → some positive value, "cancel" →
  some other positive value or 0.)
- **Visibility predicate** for "is up": the modal pump `FUN_0048eea0` runs on
  the instance — outside the pump, the modal is closed.

**Instances:** singleton — `DAT_006597c0` is the storage for the one
`cls_0x5a4e24` instance. The ctor (`recon/ghidra/cls_0x5a4e24.cpp:136-155`)
initializes screen-w/h fields `mbr_0xc=0x280=640`, `mbr_0x10=0x1e0=480`,
`mbr_0x1c=0x280`, `mbr_0x20=0x1e0` (the stage size) and the child-array
manager `field_0x88` (capacity 0x10=16 widgets — adequate for the 4 named
widgets in selstart.def plus headroom).

**Plain-language description.** A 394×316 ironwork-bordered popup with the
title "MULTIPLAYER" stamped at the top of its chrome. The popup contains a
short message ("MPSTARTGAME …" or similar), a vertical scrollable list of
start-position names, and two buttons (OK / Cancel) at the bottom. The user
picks one of the area's STARTPOSes (or one is auto-randomized if a global
trigger is set) and confirms with OK.

---

## §2 — Asset roster (table)

All measurements from `tools/ui/dump_dat.py` (Pillow-decoded).

| asset | archive | entry name | WxH (measured) | role | source rects | cite |
|---|---|---|---|---|---|---|
| Background | `selstartnotex.dat` / `selstarttex.dat` | `Background` | 394×316 | full modal chrome (frame + "MULTIPLAYER" title art baked in) | whole rect | `tools/ui/dump_dat.py selstartnotex.dat` → entry 0, flags=0x402 kc=0x0 |
| ClearFrame3 | `widgetstex.dat` / `widgetsnotex.dat` | `ClearFrame3` | 64×57 | TEXT widget chrome (9-slice) — used by `startmsg` per the STYLE TEXT default in selstart.def:4-6 (which **overrides** widgets.def's STYLE TEXT — see §6.5) | 9-slice `FRAME 5 5 5 5 MARGINS 0 0 4 4` | `widgets.def:84-86` (inherited base), `selstart.def:4-6` (local override) — both name `ClearFrame3` |
| VScrollRect | `widgetstex.dat` / `widgetsnotex.dat` | `VScrollRect` | 74×82 | LISTBOX chrome (9-slice background) — per widgets.def:120 STYLE LISTBOX VLIST | 9-slice `FRAME 8 26 26 26 MARGINS 0 0 4 4 NOCENTER` | `widgets.def:120` |
| VScrollUpU/D | `widgetstex.dat` / `widgetsnotex.dat` | `VScrollUpU`, `VScrollUpD` | 16×11 | listbox scrollbar UP button (up/down states) | whole each | `widgets.def:123, 109`, dump entries 27/28 |
| VScrollDownU/D | `widgetstex.dat` / `widgetsnotex.dat` | `VScrollDownU`, `VScrollDownD` | 16×11 | listbox scrollbar DOWN button | whole each | `widgets.def:124, 110`, dump entries 29/30 |
| VScrollThumbU/D | `widgetstex.dat` / `widgetsnotex.dat` | `VScrollThumbU`, `VScrollThumbD` | 14×13 | listbox scrollbar THUMB | whole each | `widgets.def:125, 111`, dump entries 25/26 |
| VScrollBar | `widgetstex.dat` / `widgetsnotex.dat` | `VScrollBar` | 22×74 | listbox scrollbar TRACK (9-slice) | 9-slice `FRAME 6 22 6 22 MARGINS 0 0 4 4` | `widgets.def:108`, dump entry 24 |
| ClearFrame2 | `widgetstex.dat` / `widgetsnotex.dat` | `ClearFrame2` | 64×57 | BUTTON-up sprite (9-slice) — for `ok` and `cancel` per widgets.def:89 STYLE BUTTON NORMAL | 9-slice `FRAME 10 10 12 12 MARGINS 0 0 4 4` | `widgets.def:88-93`, dump entry 3 |
| FillFrame2 | `widgetstex.dat` / `widgetsnotex.dat` | `FillFrame2` | 64×57 | BUTTON-down sprite (9-slice) | 9-slice `FRAME 10 10 12 12` | `widgets.def:89`, dump entry 4 |
| "Med" font | (font registry) | `Med` | n/a (TTF Arimo-14 in port; bitmap font in retail) | text for TEXT, LISTBOX rows, BUTTON labels | n/a | `widgets.def:86, 91, 92, 122`; `selstart.def:6` |

**Confirmed**: every asset is present in
`/Users/benjamincooley/projects/RevenantRevisited/RevenantRevisited/data/resources_unzipped/`
and decoded via `tools/ui/dump_dat.py`. No procedural fallbacks are needed.

### §2.1 — Literal DEF body (decisive)

The on-disk file is short and is the authoritative source for every coord and
flag in §3/§4/§8.

```
// Revenant - Copyright 1999 Cinematix Studios, Inc.
// ****** Revenant MP Select Start DEF File  *******

STYLE TEXT BGBITMAP "ClearFrame3"\
  FRAME 5 5 5 5 MARGINS 0 0 4 4 NOCENTER\
  FONT "Med" RECT 3 0 3 0 COLOR 255 255 255 FLAGS TEXT_CENTER | TEXT_SHADOW DRAWMODE DM_USEDEFAULT

PANEL "default"
BEGIN

// Message box
  TEXT POS 32 60 306 72 NAME "startmsg" TEXT "[message]"

// List box
  LISTBOX POS 60 145 255 84 NAME "startlist" FIELD "startpos" FLAGS LISTFLAG_VLIST ITEM 1000 18 SELCOLOR 0 112 74

// Buttons
  BUTTON POS 84 240 93 27 NAME "ok" TEXT "Ok"
  BUTTON POS 201 240 93 27 NAME "cancel" TEXT "Cancel"

END
```

(Source:
`/Users/benjamincooley/projects/RevenantRevisited/RevenantRevisited/data/resources_unzipped/selstart.def:1-22`,
shipping retail asset, treated as authoritative.)

Three things to note before any layout work:
1. The TEXT STYLE on lines 4-6 **redefines** the widgets.def TEXT STYLE for
   this panel — the local override uses `TEXT_CENTER` (instead of widgets.def's
   `TEXT_LEFT | TEXT_VCENTER`), retains `TEXT_SHADOW`, and changes the
   margin/cell `RECT` to `3 0 3 0`. The `startmsg` TEXT is centered, NOT
   left-aligned.
2. The `LISTBOX ... ITEM 1000 18` declares item-cell height = 18 px and item
   count = 1000 (the maximum row count the listbox preallocates; the actual
   number of rows is driven by the area's STARTPOS count read at populate
   time, see §6.5).
3. `SELCOLOR 0 112 74` is the selected-row highlight color (RGB `(0, 112, 74)`
   — dark teal-green).

---

## §3 — Coordinate frames & surfaces

### Frame table (mandatory)

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| **screen** | framebuffer TL | (root) | `(0, 0)` | the 640×480 framebuffer (Classic) | ctor `mbr_0xc=0x280=640`, `mbr_0x10=0x1e0=480` (`recon/ghidra/cls_0x5a4e24.cpp:148-151`) |
| **pane-local** | screen | TL | screen `(0x7e, 0x41) = (126, 65)` — passed as `param_4=0x7e, param_5=0x41` to `DefScreen_Open` | the 394×316 modal pane. Width = `0x18a = 394` (param_6), height = `0x13c = 316` (param_7). TL-anchored constant — NOT centred programmatically; the literal `(126, 65)` is `(+3, −17)` from true 640×480 centre, by intent (the same off-centre as InGameMenuDef — these panels share the 394×316 popup chrome size, see [InGameMenuDef_SPEC.md §3](InGameMenuDef_SPEC.md)). | `recon/discovered/FUN_00469370_LoadDef_selstart.cpp:8-9` (literal hex `0x7e, 0x41, 0x18a, 0x13c`) |
| **chrome (Background bitmap)** | pane | TL | pane `(0, 0)` — DEF engine paint walk blits `mbr_0x84` (the loaded backdrop sprite) at `(0, 0)` opaque (`recon/classes/cls_0x5b93c4.cpp:235-239`) | the 394×316 `Background` chrome with the baked "MULTIPLAYER" title | base paint walk `virt_meth_0x435cb0` |
| **startmsg-cell** | pane | TL | pane `(32, 60)` from `TEXT POS 32 60 306 72` | the 306×72 TEXT widget cell (the "pick a start position" message box) | `selstart.def:12` |
| **startlist-cell** | pane | TL | pane `(60, 145)` from `LISTBOX POS 60 145 255 84` | the 255×84 LISTBOX cell (start-pos list) | `selstart.def:15` |
| **button-stack** | pane | TL | the two buttons share `y = 240`, distinct x: `ok=84`, `cancel=201` — horizontal pair, not a stack. Pitch ok→cancel: `201 − 84 = 117` px (= 93 button + 24 gap). | the row of two BUTTON cells | `selstart.def:18-19` |
| **button[ok] cell** | button-stack | TL | pane `(84, 240)` from `BUTTON POS 84 240 93 27` | OK button's 93×27 cell | `selstart.def:18` |
| **button[cancel] cell** | button-stack | TL | pane `(201, 240)` from `BUTTON POS 201 240 93 27` | Cancel button's 93×27 cell | `selstart.def:19` |
| **source-rect** | (per asset) | TL | (0, 0) within the asset's own pixel rect | sub-rect copied from a source bitmap (e.g. 9-slice corner) | NOMENCLATURE §1 |

### Composition formula

For any element placed in the DEF file at `POS X Y W H`, the final screen
coordinate is:

```
screen_x = 126 + X        # = 0x7e + X
screen_y = 65  + Y        # = 0x41 + Y
```

DEF coords ARE pane-local. The pane is TL-anchored to literal `(126, 65)` on
the 640×480 stage. No centring math at runtime.

### Verification (frame composition cross-check, NOMENCLATURE §1)

For every element below, the formula gives a single composed coord auditable
against the §4 row:

- `startmsg`: pane (32, 60) → screen (158, 125). Cell extends to screen
  (158+306, 125+72) = (464, 197). Right edge (464) inside the pane's right
  edge (126+394=520) by 56 px (margin to right chrome border ~ 26 px after
  subtracting pane right border ~30). ✓
- `startlist`: pane (60, 145) → screen (186, 210). Cell extends to (441, 294).
  Inside pane right (520) and bottom (381). ✓
- `ok`: pane (84, 240) → screen (210, 305). Extends to (303, 332).
- `cancel`: pane (201, 240) → screen (327, 305). Extends to (420, 332).

Buttons share y=240; the pair sits at pane-local y_bottom=267 which is 49 px
above the pane's bottom edge (316) — the chrome lower-frame border eats some
of that, leaving ~14-16 visible px below the buttons (matches the visible art
in `/tmp/selstart_dump_notex/00_Background.png`).

### Pane backing store

Inherited from DefScreen base. TPane allocates a backing surface in the
class's open path (`vtable[+0x20]` ctor-ish open, called from
`FUN_00435040_DefScreen_LoadAndShow:30`). The DEF engine walks the child
widget array (`this+0x98`, count `this+0x88` — capacity 0x10 set by
`cls_0x41c7f0::cls_0x41c7f0(...,0x10)` at `recon/ghidra/cls_0x5a4e24.cpp:161`)
and draws each child directly to the pane buffer. **No mosaic / scratch
surface is involved.**

**Direct-renderer contract**: per NOMENCLATURE §3, the port composes the modal
into one offscreen `TSurface` RT, then `DrawSurface`s it to the HUD swapchain.

### Layout diagram (final pane-local coords)

```
pane-local origin (= screen (126, 65))
0                                                                   394
┌─────────────────────────────────────────────────────────────────────┐
│  Background chrome 394×316 (with baked "MULTIPLAYER" title art)     │
│                                                                     │
│         ┌── startmsg TEXT (32, 60, 306, 72) ──────────┐             │
│         │   centered+shadowed body text                │             │
│  60 ──> │   chrome = ClearFrame3 (9-slice 5,5,5,5)     │             │
│         │   font = "Med", color (255,255,255) shadow   │             │
│         │   text content @ populate time, from one of: │             │
│         │   MPNOPENALTY / MPPENALTY / MPRESURRECT /    │             │
│         │   MPSTARTGAME / MPSELSTARTFAILED             │             │
│         └──────────────────────────────────────────────┘             │
│                            132                                       │
│                                                                     │
│              ┌── startlist LISTBOX (60, 145, 255, 84) ──┐            │
│              │   VLIST, row h=18, max 1000 rows         │            │
│ 145 ──>      │   bg = VScrollRect, scrollbar v          │            │
│              │   selcolor = (0,112,74) — dark teal       │            │
│              │   FIELD "startpos" → slot 164 callback   │            │
│              │   format = "%s (%s)" (pos name, area)    │            │
│              └────────────────────────────────────────────┘          │
│                            229                                       │
│                                                                     │
│                                                                     │
│      ┌─[Ok]──── (84, 240, 93, 27) ──┐ ┌─[Cancel] (201, 240, 93, 27)┐│
│ 240──> │ ClearFrame2 / FillFrame2   │ │ ClearFrame2 / FillFrame2   ││
│      └────────────────────────────┘ └────────────────────────────┘  │
│                            267                                       │
│                                                                     │
│                                                                 316 │
└─────────────────────────────────────────────────────────────────────┘
   0       84              177  201               294
   (ok left)         (ok right)  (cancel left)  (cancel right)
```

Composed to screen: add `(126, 65)`.

---

## §4 — Static element layout (table)

All `(x,y,w,h)` in **pane-local** space (this spec's single chosen final
space). Screen = pane + `(126, 65)`. Frame column references §3.

| element | frame | pane (x,y) | (w,h) | source rect | sprite | cite |
|---|---|---|---|---|---|---|
| Background chrome | pane | (0, 0) | (394, 316) | whole 394×316 | `Background` (`selstart{tex,notex}.dat`) | DEF-engine bg blit, dim by `dump_dat.py` |
| TEXT "startmsg" cell | startmsg-cell | (32, 60) | (306, 72) | n/a (text) — chrome = ClearFrame3 9-slice over the cell rect | `ClearFrame3` (chrome), text rendered via `FUN_004be2b0` | `selstart.def:12`, STYLE @ `:4-6` |
| LISTBOX "startlist" cell | startlist-cell | (60, 145) | (255, 84) | n/a — `VScrollRect` 9-slice over the cell rect + scrollbar at right edge | `VScrollRect` (chrome) + `VScroll*` (scrollbar) | `selstart.def:15`, STYLE LISTBOX VLIST @ `widgets.def:119-125` |
| LISTBOX scrollbar (sub-region) | startlist-cell | listbox-local `RELRB 21 4 5 4` = pane `(60 + 255 − 21 − 4, 145 + 4)` = pane `(290, 149)`, size `(21, 84−4−4)=(21, 76)` | (21, 76) (UNCONFIRMED-C — needs §6.6) | 9-slice `VScrollBar 22×74` clipped to 21 wide | `VScrollBar` etc. | `widgets.def:123-125`. The `RELRB` resolution math is UNCONFIRMED-C (DEF parser-internal). |
| BUTTON "ok" | button[ok] cell | (84, 240) | (93, 27) | 9-slice (`FRAME 10 10 12 12`, widgets.def:90) | up=`ClearFrame2`, down=`FillFrame2` | `selstart.def:18`, STYLE BUTTON NORMAL @ `widgets.def:88-93` |
| BUTTON "cancel" | button[cancel] cell | (201, 240) | (93, 27) | 9-slice (`FRAME 10 10 12 12`) | up=`ClearFrame2`, down=`FillFrame2` | `selstart.def:19`, STYLE BUTTON NORMAL @ `widgets.def:88-93` |

**Mirror**: none. This is a single-sided centred popup.

**Verification of button row:**
- `ok_x + ok_w = 84 + 93 = 177`; `cancel_x = 201`; gap = `201 − 177 = 24 px`.
- Both share y=240 and h=27. The pair is **not** centred horizontally inside
  the 394-wide pane — the pair extends `pane_x ∈ [84, 294]` whose midpoint is
  189; pane midpoint is 197; off-centre by 8 px (left-biased). This is the
  literal retail layout.
- Pair right edge `294` is `100` px clear of pane right (`394`); pair left
  edge `84` is `84` px clear of pane left (`0`). The discrepancy
  (`84 vs 100`) is the off-centre.

---

## §5 — Draw order / composition

Selstart inherits the entire DEF-screen paint walk from `cls_0x5b93c4`. There
is no override of paint slots (140 = `FUN_00435cb0`, 80 = `FUN_00435de0`, 88 =
`virt_meth_0x435f90`, 92 = `FUN_00436090`, 76-base = `virt_meth_0x435d70`) on
`cls_0x5a4e24__vftable_5a4e24` (compare `cls_0x5a4e24__vftable_5a4e24.cpp`
slots 80/88/92/140 to `cls_0x5b93c4__vftable_5b93c4.cpp` — identical pointers).

The order matches [InGameMenuDef_SPEC.md §5](InGameMenuDef_SPEC.md) verbatim:

1. **Background blit** (`virt_meth_0x435cb0`, `recon/classes/cls_0x5b93c4.cpp:235-239`):
   `mbr_0x84` (the loaded `Background` 394×316 sprite, attached by
   `FUN_00435660` after the `vtable[+0xa0]("background", ...)` resolve in
   `FUN_00435040_DefScreen_LoadAndShow:31-33`) is blitted opaque
   (`DM_USEDEFAULT = 0x80000000`) into the pane buffer at pane-local `(0, 0)`
   via `FUN_00438d80_BlitEffect_StructInit(buf, 0,0,0,0, w, h, 0x80000000)`
   then `(*+0x5c)(buf, sprite, 0, 0)`. (Per UI_METHOD_MAP §8.1 / §15a.)
2. **Per-child walk** (`recon/classes/cls_0x5b93c4.cpp:240-249`): iterate
   `this->field_0x98[i]` for i in 0 .. `this->field_0x88`, drawing each child
   via its `vtable+0x40` (TWidget Draw). DEF declaration order in
   `selstart.def`:
   - i=0: `TEXT "startmsg"` — STYLE-TEXT renderer draws `ClearFrame3` 9-slice
     chrome at `(32, 60, 306, 72)`, then renders the populated text via
     `FUN_004be2b0` with the STYLE-supplied font/color/flags.
   - i=1: `LISTBOX "startlist"` — `cls_0x5a3d44` (TDefListbox) Draw:
     `VScrollRect` 9-slice chrome at `(60, 145, 255, 84)`, then iterates the
     visible rows (clipped to ITEM h=18), invoking the FIELD callback
     `virt_meth_0x4697a0` (slot 164) per row to format the row text via
     `"%s (%s)"`, finally the scrollbar (UP/THUMB/DOWN) at the listbox's
     right edge.
   - i=2: `BUTTON "ok"` — TButton Draw selects `ClearFrame2` (up) /
     `FillFrame2` (down) per state, 9-slice into `(84, 240, 93, 27)`, then
     the centered "Ok" label.
   - i=3: `BUTTON "cancel"` — same pattern at `(201, 240, 93, 27)`.
3. **Focused child late-draw** (`recon/classes/cls_0x5b93c4.cpp:251-254`):
   `this->mbr_0xa8` re-drawn last (focus overlay). At rest this is 0.
4. **Effect-pipeline iterate** (UI_METHOD_MAP §8): the global blit-effect
   slots are walked. Empty for this screen.

The composition is **single-pass into the pane buffer**, then the buffer is
blitted to the HUD swapchain by the screen manager.

There is no separate `DrawBackground` / `Draw` / `Update` split for this
class — `0x435cb0` (paint) and `0x436090` (per-child) collectively handle the
whole frame; the per-frame Update is `virt_meth_0x469890` (slot 76, override
— see §6.4 — which calls the inherited base update first, then runs the
auto-randomize check).

---

## §6 — Algorithms (pseudocode per helper)

### §6.1 — `LoadDef_selstart` (FUN_00469370 / vtable slot 0)

The Open trampoline.

```c
// recon/discovered/FUN_00469370_LoadDef_selstart.cpp:1-19
// recon/ghidra/cls_0x5a4e24.cpp:32-48 (class-form, identical body)
int LoadDef_selstart(cls_0x5a4e24* this) {
    int ok = cls_0x5b93c4::virt_meth_0x435150(             // = DefScreen_Open
        (cls_0x5b93c4*)this,
        /*screen_name*/  "selstart",                       // s_005d3bc4
        /*panel_name?*/  &DAT_00659cb8,                    // singleton state ptr
                                                           //   (UNCONFIRMED-E:
                                                           //   either DEF-panel
                                                           //   name buffer or
                                                           //   a defaults
                                                           //   struct — only
                                                           //   address visible)
        /*flags*/        0x11,                             // bit0=tex-pack,
                                                           //   bit4=modal
                                                           //   (matches in-game
                                                           //   menu pattern)
        /*x*/            0x7e   = 126,                     // pane screen x
        /*y*/            0x41   = 65,                      // pane screen y
        /*w*/            0x18a  = 394,                     // pane w
        /*h*/            0x13c  = 316,                     // pane h
        /*bg_w*/         0x15c  = 348,                     // STYLE "background"
                                                           //   cell w (used by
                                                           //   the engine bg
                                                           //   sprite logic)
        /*bg_h*/         0x54   = 84,                      // STYLE bg cell h
        /*bg_resource*/  "widgets",                        // s_005d3bbc
        /*outer_name*/   "selstart"                        // s_005d3bb0 (passed
                                                           //   as in_stack_0..2c
                                                           //   to virt_meth_0x435150)
    );
    if (ok == 0) return 0;
    this->mbr_0x180 = 0;                                   // clear member 0x180
    this->mbr_0x17c = 0;                                   // clear current-startpos
    this->mbr_0x184 = 0;                                   // clear "frame-acted" flag
    this->mbr_0x188 = 0;                                   // clear "auto-pick trigger"
    return 1;
}
```

Notes:
- `bg_w=0x15c=348, bg_h=0x54=84`: see UNCONFIRMED-B in InGameMenuDef §14
  (UNCONFIRMED-G in that spec) — these are stored somewhere by
  `DefScreen_LoadAndShow` and probably describe the engine's tile/clip rect
  for the "background" widget the engine paints inside the pane. Distinct
  from the panel chrome size (394×316). The 348×84 figure is bigger than the
  startmsg cell (306×72) but in the same vicinity — they may relate to the
  message-box chrome region. UNCONFIRMED-B (this spec).
- The chrome `Background` sprite is resolved by name in
  `FUN_00435040:31` (`vtable+0xa0("background", 0xffffffff)`). The runtime
  resolver looks in the currently-mounted widgets pack first, then in the
  per-screen `selstart{tex,notex}.dat` (loaded by the
  `outer_name="selstart"` handler `FUN_00435b20`, UNCONFIRMED-E). The
  per-screen .dat is what supplies the `Background` entry — `widgets*.dat`
  has no entry of that name.

### §6.2 — `DefScreen_Open` and `DefScreen_LoadAndShow` (inherited)

Same as documented in [InGameMenuDef_SPEC.md §6.2 / §6.3](InGameMenuDef_SPEC.md).
No selstart-specific behavior beyond the literal args in §6.1.

`flags=0x11` → bit 0 (tex-pack) and bit 4 (modal-style/fade-in ramp).
`bg_resource="widgets"` → the global widgets pack is shared (not freshly
loaded). The inner `LoadAndShow` reads `<panel>.def` = `selstart.def`,
runs `DefWidget_LoadFile` + `DefWidget_ParseBuffer` + `DefWidget_ParsePanel`,
and emits the 4 child widgets in declaration order.

### §6.3 — `virt_meth_0x4693e0` — OnInput / KeyPress (slot 108)

Arrow-key advance of the listbox selection.

```c
// recon/ghidra/cls_0x5a4e24.cpp:54-76
void OnInput(cls_0x5a4e24* this, int vkey /*param_1*/, int isDown /*in_stack_00000008*/) {
    cls_0x5b98b8::meth_0x436930(this);                      // FindWidgetByName(this)
                                                            //   — looks up
                                                            //   "startlist"
                                                            //   (the implicit
                                                            //   ECX-passed
                                                            //   name; the body
                                                            //   uses an
                                                            //   in-stack
                                                            //   string ptr
                                                            //   set by the
                                                            //   caller — the
                                                            //   InputDispatch
                                                            //   shim)
    cls_0x5a3d44* lb = cls_0x5b98b8::meth_0x436900(this);   // GetWidget(idx)
                                                            //   = the listbox
                                                            //   widget
    if (lb != NULL) {
        if (vkey == 0x26 /*VK_LEFT (sic — see note)*/ && isDown) {
            cls_0x5a3d44::meth_0x430b80(lb);                // step (UNCONFIRMED
                                                            //   direction —
                                                            //   the in-stack
                                                            //   arg picks
                                                            //   prev/next;
                                                            //   see below)
            return;
        }
        if (vkey == 0x28 /*VK_DOWN*/ && isDown) {
            cls_0x5a3d44::meth_0x430b80(lb);
            return;
        }
    }
    cls_0x5b93c4::virt_meth_0x4361f0(this, vkey);            // base default
}
```

VK codes: `0x26 = VK_UP`, `0x28 = VK_DOWN` (Win32 standard). So **Up arrow**
and **Down arrow** advance the selection (which direction depends on the
unobserved in-stack arg to `meth_0x430b80` — `meth_0x430b80` is the
`SetSelection(idx)` setter, so the dispatcher passes the prev/next index
value computed inline). The fall-through `cls_0x5b93c4::virt_meth_0x4361f0`
is the base-class default (passes the keystroke to base TPane handling — TAB
focus, ESC close, etc.).

### §6.4 — `virt_meth_0x469890` — Update / per-frame tick (slot 76)

Per-frame; auto-randomizes the start-pos when the trigger flag is set.

```c
// recon/ghidra/cls_0x5a4e24.cpp:114-130
void Update(cls_0x5a4e24* this) {
    cls_0x5b93c4::virt_meth_0x435d70(this);                  // base Update walk
                                                             //   (drives the
                                                             //   open/close
                                                             //   ramp, etc.)
    if (DAT_00668128 != 0                                    // global
                                                             //   "auto-pick
                                                             //   enabled"
                                                             //   trigger
                                                             //   (UNCONFIRMED:
                                                             //   set by MP
                                                             //   join logic
                                                             //   when the host
                                                             //   wants to
                                                             //   randomize
                                                             //   newly-joining
                                                             //   players?)
        && this->mbr_0x188 != 0                              // this-screen
                                                             //   "needs random
                                                             //   pick" flag
        && this->mbr_0x184 == 0)                             // not yet acted
                                                             //   this open
    {
        this->mbr_0x188 = 0;                                 // consume trigger
        this->mbr_0x184 = 1;                                 // mark acted
        int idx = FUN_00483300_RandomRange(0,
                  *(int*)(this->mbr_0x17c + 0x98) - 1);      // RNG over the
                                                             //   per-area
                                                             //   STARTPOS
                                                             //   count
                                                             //   (mbr_0x17c
                                                             //   →
                                                             //   area-record;
                                                             //   +0x88 = ct,
                                                             //   +0x98 =
                                                             //   array)
        cls_0x5a3d44::meth_0x430b80(                         // listbox
            (cls_0x5a3d44*)this->mbr_0x17c);                 //   SetSelection
                                                             //   (idx) — yes,
                                                             //   the listbox
                                                             //   widget is
                                                             //   reached via
                                                             //   mbr_0x17c
                                                             //   here, NOT the
                                                             //   area record;
                                                             //   see
                                                             //   UNCONFIRMED-F
        cls_0x5b98b8::meth_0x436980(this);                   // FindWidgetByName
                                                             //   (probably
                                                             //   resolves
                                                             //   "ok" — the
                                                             //   ok button —
                                                             //   so it can
                                                             //   trigger a
                                                             //   synthetic
                                                             //   click)
        this->vftptr_0x0->virt_meth_0x469460_148(this);      // call slot 148
                                                             //   (Apply) to
                                                             //   commit the
                                                             //   pick + close
    }
}
```

**UNCONFIRMED-F**: the use of `mbr_0x17c` is ambiguous — at `:132` it's
treated as a record with `+0x88`/`+0x98` (area/STARTPOS-record shape), at
`:133` it's treated as a listbox widget (the type passed to `meth_0x430b80`).
The most consistent reading is that `mbr_0x17c` holds the **listbox widget**
pointer (cached during populate, slot 148), AND that widget's `+0x88`/`+0x98`
are its own row-count / row-array (NOT the area's STARTPOS array — but the
listbox's row array is in turn populated by slot 148 from the area's
STARTPOS array, so the count and array are the same). This is consistent
with the listbox being `cls_0x5a3d44`, whose `meth_0x430b80(idx)` uses an
in-stack arg that's the index. Resolve by extracting slot 148.

### §6.5 — `virt_meth_0x469460` — Apply / OnControl (slot 148) — UNCONFIRMED-A

**Body NOT in `recon/ghidra/cls_0x5a4e24.cpp`.** Inferred from the function's
complete string-XREF set in `recon/ghidra/_data.txt:77910-77975, 128336-128338,
128775` and from the global-reads observed by Ghidra on this function. The
verbatim per-instruction control flow is UNCONFIRMED-A — **but the set of
behaviours the body performs is bounded by the string-XREFs and the slot's
role in the parent vtable family.**

Strings used by `virt_meth_0x469460` (verbatim from `_data.txt`):

| addr | string | meaning |
|---|---|---|
| `:0046948a` | `s_startmsg_005d3bdc` = `"startmsg"` | widget NAME (TEXT) — looked up by name to set the message text |
| `:004694bd` | `s_mpnopenalty_005d3be8` = `"mpnopenalty"` | UNCONFIRMED — possibly a panel NAME or STRINGREF key (see UNCONFIRMED-A) |
| `:004694e8` | `s_mpnopenalty_005d3bf4` = `"mpnopenalty"` (2nd buffer) | duplicate — possibly the localization key, vs the panel id |
| `:00469516` | `s_mppenalty_005d3c00` = `"mppenalty"` | likewise |
| `:00469538` | `s_mpnopenalty_005d3c0c` | likewise |
| `:00469558` | `s_mppenalty_005d3c18` | likewise |
| `:00469581` | `s_mppenalty_005d3c24` | likewise |
| `:004695a3` | `s_mpresurrect_005d3c30` = `"mpresurrect"` | likewise |
| `:004695c5` | `s_mpstartgame_005d3c3c` = `"mpstartgame"` | likewise (the "start game" / "pick start" message variant) |
| `:004695f8` | `s_startlist_005d3c48` = `"startlist"` | widget NAME (LISTBOX) — looked up by name to populate rows / read selection |
| `:00469687` | `DAT_005d3c54` = literal bytes `"ok\0"` | widget NAME (BUTTON) — looked up by name to hide/show or fire the OK button |
| `:004696a1` | `PTR_Sleep_005a310c` | the Win32 `Sleep` API import — used somewhere in this body (UNCONFIRMED: presumably a `Sleep(0)` yield in a wait-for-pump loop) |
| `:004696e9` | `s_MPSELSTARTFAILED_005d3c58` | STRINGREF key for the "failure" message variant |
| `:0046975a` | `s_cancel_005d3c6c` = `"cancel"` | widget NAME (BUTTON) — looked up by name to dispatch Cancel click |

Globals read by this function (per `_data.txt:128336-128338, 128775`):
- `DAT_00658da0` — the active-area/module index (read twice — at `:0046960c`
  and `:004696ad`). Used to fetch the area's STARTPOS list (see §6.6).
- `DAT_0067680c` (read at `:004694b4`), `DAT_00676810` (4 reads
  `:0046950e/00469572/0046964b/0046966d`), `DAT_00676814` (UNCONFIRMED but
  in the same region — the disasm cluster reads several +0x4-stride globals).
  These are the **MP-mode penalty/resurrect/start-game world flags** that
  select among the message variants. (Compare InGameMenuDef's
  `DAT_0066829c` / `DAT_0067682c` / `DAT_0065d0d0` MP-flag set — same
  family, different bits.)

**Inferred behaviour** (UNCONFIRMED-A, but the only reading consistent with the
strings + globals + the analogous InGameMenuDef slot 148 body):

```c
// Inferred body — verbatim disasm extraction recommended (see UNCONFIRMED-A)
int Apply_or_OnControl(cls_0x5a4e24* this) {
    // Two callees by family:
    // (1) Populate path — called from selstart.def open + every refresh.
    //     - Resolve the LISTBOX widget by name "startlist" → cache to
    //       this->mbr_0x17c (see §6.4 caching).
    //     - Look at DAT_00676810 / DAT_0067680c / DAT_00676814 to pick the
    //       message variant (MPNOPENALTY / MPPENALTY / MPRESURRECT /
    //       MPSTARTGAME / MPSELSTARTFAILED).
    //     - StringRef-lookup the chosen key via
    //       cls_0x45f7c0::meth_0x49d800 (the STRINGREF resolver, same one
    //       used by virt_meth_0x4697a0 :103 and InGameMenuDef MPMUSTBEHOST).
    //     - Set the "startmsg" TEXT widget's content with the resolved string
    //       (probably via cls_0x5b98b8::meth_0x436980("startmsg") then a
    //       widget-content setter at +0x... ).
    //     - Hide / show the "ok" button conditionally based on the chosen
    //       branch (DAT_005d3c54 = "ok" XREF is the hide/show target).
    //     - Optionally Sleep(0) (the PTR_Sleep XREF) — likely a yield in a
    //       host-side wait-for-clients loop.
    //
    // (2) OnControl path (event == 3000 / BUTTON click) — the same slot is
    //     the OnControl dispatcher in this DEF-screen family (see
    //     InGameMenuDef_SPEC §6.4). Per the InGameMenuDef pattern, when
    //     called with (widget, 3000), this body switches on widget->name:
    //       - "ok" → commit the listbox selection (write the chosen
    //         STARTPOS into the player or world), then this->mbr_0x5c =
    //         <ok-code>, call vtable[+0x08] (TPane::Close).
    //       - "cancel" → set this->mbr_0x5c = <cancel-code>, Close.
    //
    // The dual-mode nature (populate + dispatch) is normal for this slot —
    // see the InGameMenuDef body which also runs string lookups + button
    // dispatch in slot 148.
    //
    // UNCONFIRMED-A: the exact distinction between (1) and (2) (entry-arg
    // checks), the precise globals → message-variant mapping, the
    // sequence of widget-content / hide-show setters, and the Sleep loop
    // condition must be extracted from the disassembly at 0x00469460..
    // 0x004697a0 (the function is ~840 bytes — non-trivial).
}
```

**Resolve**: `DecompileAddr.java 0x00469460` (Ghidra) or
`objdump -d --start-address=0x00469460 --stop-address=0x004697a0
data/Revenant.exe` and verify against the string-XREF set above.

### §6.6 — `virt_meth_0x4697a0` — OnFieldQuery / "startpos" FIELD getter (slot 164)

The listbox row-content callback. Format the row text for a given start-pos
index.

```c
// recon/ghidra/cls_0x5a4e24.cpp:82-117
int OnFieldQuery(cls_0x5a4e24* this,
                 char* field_name,                            // param_1
                 char* out_buf,                               // param_2
                 undefined4 unused3, undefined4 unused4,
                 int row_index) {                             // in_stack_00000014
    *out_buf = '\0';                                          // start empty
    if (stricmp(field_name, "startpos") != 0) {
        // Not our field — defer to base.
        return cls_0x5b93c4::virt_meth_0x436ea0(this);
    }
    cls_0x4609f0_TModuleMgr* registry =
        &UNK_0065a630.field_0x138;                            // module
                                                              //   registry
    int area_rec = cls_0x4609f0::meth_0x460ca0(
        registry, DAT_00658da0);                              // active area
                                                              //   record
    if (row_index < *(int*)(area_rec + 0x88)) {               // bound-check
                                                              //   vs the
                                                              //   area's
                                                              //   STARTPOS
                                                              //   array
        const char* startpos_name =
            *(const char**)(*(int*)(area_rec + 0x98)
                            + row_index * 4);                 // STARTPOS[row]
                                                              //   .name (a
                                                              //   pointer at
                                                              //   the start of
                                                              //   the record;
                                                              //   the area
                                                              //   STARTPOS
                                                              //   record's
                                                              //   first field
                                                              //   IS the name)
        const char* mp_game_label =
            cls_0x45f7c0::meth_0x49d800(
                &UNK_0065d1c0.field_0x310);                   // STRINGREF
                                                              //   "MPGAME"
                                                              //   resolved via
                                                              //   s_mpgame_005d3c80
        FUN_0058b100(out_buf, "%s (%s)",
                     startpos_name, mp_game_label);           // sprintf into
                                                              //   row buffer
        return 1;
    }
    // Past-end (row_index >= count) — fall through to the "??" /
    // "MPPARTY" formatter.
    int alt_rec = cls_0x45f7c0::meth_0x51fdd0(
        &DAT_0065a890, (char)DAT_00667fcc, 0);                // probably:
                                                              //   look up the
                                                              //   player's
                                                              //   party /
                                                              //   character
                                                              //   record
    const char* alt_name = (alt_rec == 0)
        ? (const char*)&DAT_005d3c90                          // = literal "??"
                                                              //   (raw bytes
                                                              //   3f 3f at
                                                              //   005d3c90)
        : *(const char**)(alt_rec + 0x38);                    // record's name
                                                              //   field at
                                                              //   +0x38
    const char* mp_party_label =
        cls_0x45f7c0::meth_0x49d800(
            &UNK_0065d1c0.field_0x310);                       // STRINGREF
                                                              //   "MPPARTY"
                                                              //   resolved via
                                                              //   s_mpparty_005d3c94
    FUN_0058b100(out_buf, "%s (%s)",
                 alt_name, mp_party_label);
    return 1;
}
```

So each listbox row is one of:
- `"<STARTPOS_name> (MPGAME)"` for in-range rows (the area's predefined
  start-points).
- `"<party_member_name> (MPPARTY)"` for past-end rows (party members joining
  the game whose start-pos is determined by who they are joining, not by an
  area STARTPOS).
- `"?? (MPPARTY)"` if the party lookup returns null.

**Cite** (per UI_METHOD_MAP §10): `FUN_0058b100` is the `sprintf`-family
buffer-format function. Format literal `"%s (%s)"` at `_data.txt:77985` /
`:77998`.

**Cite (key sub-call)**: `cls_0x45f7c0::meth_0x49d800` is the STRINGREF
resolver (the same one used everywhere in DEF screens for localized text,
e.g. `MPMUSTBEHOST` in InGameMenuDef_SPEC §6.4).

### §6.7 — Destructor (`~cls_0x5a4e24` @ `0x46d2d0`)

```c
// recon/ghidra/cls_0x5a4e24.cpp:177-195
cls_0x5a4e24* ~cls_0x5a4e24(cls_0x5a4e24* this) {
    this->vftptr_0x0 = &cls_0x5a4510__vftable_5a4510;        // re-vtable to
                                                             //   intermediate
                                                             //   base
    cls_0x5b98b8::virt_meth_0x434f30(this);                  // TPane::Close
                                                             //   chain
    FUN_004830f0(this->field_0x98);                          // free child array
    return this;
}
```

Nothing selstart-specific. Same shape as the other DEF-screen popup dtors.

---

## §7 — Effects & shadows

No panel-private shadow registry. Defaults come from the DEF widget engine and
the local `STYLE TEXT` override in `selstart.def:4-6`:

- **TEXT "startmsg"** uses `FLAGS TEXT_CENTER | TEXT_SHADOW`
  (`selstart.def:6`). `TEXT_SHADOW = 0x400` (per `widgets.def:24`). Per
  UI_METHOD_MAP §5, the `0x400` bit on the font id drives the 3-pass black
  drop shadow at `(0,0)` + `(+1,0)` + `(0,+1)`, then the colored pass at
  `(0,0)`. Color = `(255, 255, 255)` (`COLOR 255 255 255` in the STYLE).
- **TEXT cell chrome (`ClearFrame3`)** uses `kc=0x7c1f` (magenta in RGB555),
  per `dump_dat.py /widgetsnotex.dat` entry 6. Per UI_METHOD_MAP §16, the
  port handles magenta as global transparency for these widget chromes.
- **BUTTON "ok" / "cancel" labels** use `FLAGS TEXT_CENTER | TEXT_VCENTER |
  TEXT_SINGLELINE | TEXT_SHADOW` (`widgets.def:91`) for the up state,
  COLOR `(255, 255, 255)`; the down state is COLOR `(0, 0, 0)` (down labels
  have no SHADOW flag, `widgets.def:92`). Both labels rendered through the
  same FUN_004be2b0 path.
- **LISTBOX row text** uses `FONT "Med" COLOR 255 255 255 FLAGS TEXT_LEFT |
  TEXT_VCENTER | TEXT_SHADOW | TEXT_SINGLELINE` (`widgets.def:122`). The
  selected row's background is the LISTBOX's `SELCOLOR 0 112 74` (dark
  teal-green per `selstart.def:15`); the selected row's text color stays
  white per the LISTBOX STYLE.
- **Background chrome** has `kc=0x0` (`dump_dat.py selstartnotex.dat` entry
  0) — opaque, no chroma key needed.

- **Per-side shadow** is the standard right-bottom 3-pass for all SHADOW-flagged
  text — there is no left/right mirror in this panel.

- **Pink-halo bug** — see UI_METHOD_MAP §16 and
  [[project-retail-pink-halo-bug]]. The retail magenta-cleared scratch +
  antialiased text shadow can produce a pink fringe on the startmsg / button
  labels. **DO NOT REPRODUCE.** Use real alpha (no magenta colorkey) when
  compositing text. The intent is white-text/black-shadow on the up state.

---

## §8 — Text rendering (table)

All cells in pane-local space. Screen = pane + (126, 65).

| string | cell (pane x,y,w,h) | font | px | color | h-align | v-align | shadow | format string | source value | cite |
|---|---|---|---|---|---|---|---|---|---|---|
| (startmsg body — runtime-resolved) | (32, 60, 306, 72) | "Med" | (TTF Arimo-14 in port) | (255, 255, 255) | **center** (TEXT_CENTER) | **top** (no TEXT_VCENTER in the local STYLE TEXT override — NOMENCLATURE §2 default) | 3-pass black (TEXT_SHADOW=0x400) | (no `printf` format — direct STRINGREF text) | one of: `STRINGREF("MPNOPENALTY") / STRINGREF("MPPENALTY") / STRINGREF("MPRESURRECT") / STRINGREF("MPSTARTGAME") / STRINGREF("MPSELSTARTFAILED")` — branch chosen by DAT_00676810/0c/14 flags in slot 148 | `selstart.def:4-6, 12`; `_data.txt:77920, 77928, 77933, 77942, 77947, 77951, 77965` |
| "Ok" (BUTTON label, up) | (84, 240, 93, 27) | "Med" | (Arimo-14) | (255, 255, 255) | center | vcenter | yes | literal | DEF | `selstart.def:18`; STYLE @ `widgets.def:90-91` |
| "Ok" (BUTTON label, down) | (84, 240, 93, 27) | "Med" | (Arimo-14) | (0, 0, 0) | center | vcenter | **no** (down state has no SHADOW per widgets.def:92) | literal | DEF | `selstart.def:18`; STYLE @ `widgets.def:90, 92` |
| "Cancel" (BUTTON label, up) | (201, 240, 93, 27) | "Med" | (Arimo-14) | (255, 255, 255) | center | vcenter | yes | literal | DEF | `selstart.def:19`; STYLE @ `widgets.def:90-91` |
| "Cancel" (BUTTON label, down) | (201, 240, 93, 27) | "Med" | (Arimo-14) | (0, 0, 0) | center | vcenter | no | literal | DEF | `selstart.def:19`; STYLE @ `widgets.def:90, 92` |
| LISTBOX row (per in-range row) | row-relative — 18 px h, full row width inside listbox content rect (255 − scrollbar w − margins, see §6.6) | "Med" | (Arimo-14) | (255, 255, 255) (selected row background = (0, 112, 74) — see SELCOLOR) | left (TEXT_LEFT) | vcenter (TEXT_VCENTER) | yes (TEXT_SHADOW) | `"%s (%s)"` | `(STARTPOS_name, STRINGREF("MPGAME"))` | `selstart.def:15`; LISTBOX STYLE @ `widgets.def:121-122`; format/source @ `_data.txt:77980, 77985`; cls_0x5a4e24.cpp:104 |
| LISTBOX row (past-end / party row) | as above | "Med" | (Arimo-14) | (255, 255, 255) | left | vcenter | yes | `"%s (%s)"` | `(party_member_name or "??", STRINGREF("MPPARTY"))` | as above; `_data.txt:77994, 77998`; cls_0x5a4e24.cpp:115 |

**Decoded font/format bits:**
- **TEXT_CENTER** = `0x2` (`widgets.def:15`) → `FUN_004be2b0` `param_10 & 2`
  → DrawTextA `DT_CENTER = 0x1` (`UI_METHOD_MAP §5:148-153`).
- **TEXT_VCENTER** = `0x40` (`widgets.def:20`) → `param_10 & 0x40` → DrawTextA
  `DT_VCENTER = 0x4` is OR-d in (UI_METHOD_MAP §5 step 6 — the body sets
  `0x28` which is DT_VCENTER + DT_TOP combo; the override path that wins is
  `0x40` → `0x28`).
- **TEXT_LEFT** = `0x1` (`widgets.def:14`) → `param_10 & 1` → DrawTextA
  `DT_LEFT = 0x0` (default; the format word stays at the `0x2810` base).
- **TEXT_SHADOW** = `0x400` (`widgets.def:24`) → propagated into `param_8`
  (font id) so `param_8 & 0x400` fires the 3-pass shadow at
  `FUN_004be2b0:377, 389`.
- **TEXT_SINGLELINE** = `0x80` (`widgets.def:21`) → `param_10 & 0x80` →
  single-line override (forces `0x20` = DT_TOP, but TEXT_VCENTER's `0x40`
  wins when both present).
- **Color packing**: widgets.def `COLOR R G B` is parsed into the
  `param_7` RGB word; the byte-swap to BGR for GDI `SetTextColor` happens
  inside `FUN_004be2b0:378` (UI_METHOD_MAP §5/§6).

**Vertical alignment for startmsg:** the local STYLE TEXT override
(`selstart.def:6`) has **no `TEXT_VCENTER`** — only `TEXT_CENTER |
TEXT_SHADOW`. Per NOMENCLATURE §2 default, the message text **stacks DOWN from
the cell TOP** by line height. Multi-line message blocks leave the lower part
of the 306×72 cell empty. (This is intentional — the message lines are short
to medium, no need to vertically centre.)

**Localization**: every message variant is a STRINGREF key (`MPNOPENALTY`,
`MPPENALTY`, `MPRESURRECT`, `MPSTARTGAME`, `MPSELSTARTFAILED`, `MPGAME`,
`MPPARTY`). The actual displayed strings come from the runtime string table —
the port's localization pipeline (`[[project-localization]]`) resolves them.

---

## §9 — Animation & dynamic behavior

- **No transitions on open** by default. The base class's `flags & 0x10`
  ramp (`cls_0x5b93c4::mbr_0xbc`/`mbr_0xc0` at `:269-283`) is wired but its
  visual effect is the same UNCONFIRMED-C carried over from
  [InGameMenuDef_SPEC.md §9](InGameMenuDef_SPEC.md): possibly a 1-tick
  activation debounce or a brief fade-in. selstart's `flags=0x11` so bit 4
  IS set. UNCONFIRMED.
- **No hover-fade on the buttons.** The BUTTON widget engine flips Up↔Down
  sprites on mouse-down/up without an intermediate ramp.
- **Auto-randomize tick** (the only on-screen state animation): per §6.4,
  when `DAT_00668128 != 0` and `this->mbr_0x188 != 0` and `this->mbr_0x184
  == 0`, the next Update tick consumes the trigger, picks a random STARTPOS
  index via `RandomRange(0, count-1)`, applies it to the listbox, and fires
  slot 148 (which presumably auto-clicks OK and closes the modal). This is
  the "MP host auto-pick for joining clients" path.

  ```
  ramp this->mbr_0x184 (acted-this-open):
    range   0..1
    step    +1 once when (DAT_00668128 && mbr_0x188 && !mbr_0x184) holds
            (single-shot per modal open)
    target  1
    drive   DAT_00668128 = the global auto-pick enable
    on-end  hold at 1 (won't fire again until modal re-opened)
    cite    recon/ghidra/cls_0x5a4e24.cpp:129-137
  ```

  No tween — single-frame state flip.
- **Dirty/redraw**: standard TPane/`cls_0x5b93c4`. Each child widget owns its
  own dirty bit (per `recon/classes/cls_0x5b93c4.cpp:240-249` walk: the
  `piVar1[5] & 2` check is the "hidden" mask; the `0x800` and `0x200` masks
  drive the DEFW selection of which slot is called). Per
  [[feedback-versions-over-flags]], the port should use a monotonic version
  counter on the cached field values.
- **No state machine beyond Open → Pump → (Apply | Cancel) → Close.**

---

## §10 — Input & dispatch

Interactive. Standard DEF-engine input flow plus the `cls_0x5a4e24` overrides
identified above.

1. **Mouse input** arrives at the pane via the screen manager's input
   dispatch; TPane's per-child hit-test (`cls_0x5b93c4::virt_meth_0x4364d0` at
   `:548-566`) routes clicks to the BUTTON / LISTBOX whose rect contains the
   pointer.
2. **BUTTON click** → BUTTON widget's mouse handler emits the "control event"
   `event = 3000 = 0xbb8` to the panel via slot 148
   (`virt_meth_0x469460` — UNCONFIRMED-A body). The dispatcher switches on
   widget NAME (`"ok"` / `"cancel"`), sets `this->mbr_0x5c` and calls
   `vtable[+0x08]` (TPane::Close).
3. **Keyboard arrow keys** (Up / Down — VK_UP=0x26, VK_DOWN=0x28) advance the
   listbox selection via `virt_meth_0x4693e0` (slot 108, §6.3). Other keys
   fall through to base.
4. **LISTBOX direct click** on a row sets the selection; the listbox widget
   class (`cls_0x5a3d44`) handles the row hit-test internally; double-click
   is NOT enabled (LISTFLAG_DBLCLICK is not set in `selstart.def:15`).
5. **Modal pump** (`FUN_0048eea0`) reads `this->mbr_0x5c` on close and returns
   it to the caller (TPlayScreen Initialize). The caller's switch is at the
   end of `recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp`
   — but actually the caller does NOT branch on the return; it just resumes
   normal init after the modal closes. The chosen STARTPOS is committed via
   side-effects (the listbox SetSelection writes the chosen index into a
   global the engine reads at game-spawn time — UNCONFIRMED-G).

**Hit rects** (pane-local; add (126, 65) for screen-space):

| control | command intent | hit rect (pane-local) | cite |
|---|---|---|---|
| LISTBOX rows | per-row selection | (60, 145, 255, 84) — listbox content area, minus scrollbar at right (~21 px) | `selstart.def:15` |
| BUTTON "ok" | confirm & close | (84, 240, 93, 27) | `selstart.def:18`, §6.5 |
| BUTTON "cancel" | abandon & close | (201, 240, 93, 27) | `selstart.def:19`, §6.5 |
| (Up arrow) | listbox prev | n/a | `virt_meth_0x4693e0:64-67` |
| (Down arrow) | listbox next | n/a | `virt_meth_0x4693e0:69-71` |
| (ESC) | UNCONFIRMED — likely Cancel via base default | n/a | `cls_0x5b93c4::virt_meth_0x4361f0` (base default in `virt_meth_0x4693e0:74`) |

**Modal-state globals consulted:**

| global | meaning | effect on dispatch |
|---|---|---|
| `DAT_0066829c` | MP-mode flag (0=SP, 1=MP) | screen only opens when MP — entire panel is gated by this from TPlayScreen Initialize |
| `DAT_00668128` | "auto-randomize start-pos" trigger | when nonzero AND `this->mbr_0x188 != 0` AND `this->mbr_0x184 == 0`, the Update tick auto-picks via `RandomRange` and fires slot 148 (probably auto-OK) |
| `DAT_0067680c`, `DAT_00676810`, `DAT_00676814` | MP-mode penalty/resurrect/start-game state flags | select among `MPNOPENALTY` / `MPPENALTY` / `MPRESURRECT` / `MPSTARTGAME` / `MPSELSTARTFAILED` startmsg variants in slot 148 (UNCONFIRMED exact mapping — A) |
| `DAT_00658da0` | active area/module index | feeds the area STARTPOS lookup for the listbox rows (slot 164, §6.6) |
| `this->mbr_0x17c` | cached LISTBOX widget pointer (set by slot 148 populate path) | source of selection state for the auto-randomize path |
| `this->mbr_0x184` | "acted this open" flag (single-shot guard for auto-pick) | prevents auto-pick from firing twice per modal open |
| `this->mbr_0x188` | "needs auto-pick" trigger (set by caller, UNCONFIRMED who sets it) | input to the auto-randomize guard |
| `this->mbr_0x5c` | return code (per InGameMenu pattern) | read by the modal pump on close to tell the caller which button was pressed (UNCONFIRMED exact ok/cancel codes — A) |

---

## §11 — Retail bugs NOT to reproduce

1. **Pink-halo on text** (`startmsg` body, BUTTON labels, LISTBOX rows) —
   see UI_METHOD_MAP §16 and `[[project-retail-pink-halo-bug]]`. The retail
   magenta-cleared scratch + antialiased text shadow produces a pink fringe
   around shadowed labels. The intent is white-text / black-shadow. Use real
   alpha (no magenta colorkey) when compositing text.
2. **Off-centre modal**: pane origin `(126, 65)` is `(+3, −17)` from true 640×480
   centre. NOT a bug — the chrome art is asymmetric and the literal anchor
   is by intent (same as InGameMenuDef which shares the 394×316 chrome size).
   Reproduce literally; do not "fix" by centring programmatically.
3. **Slight left-bias of the OK/Cancel pair** within the pane: the pair
   spans pane x ∈ [84, 294], whose midpoint (189) is 8 px left of the pane
   midpoint (197). NOT a bug — the retail DEF places them at literal
   `POS 84` and `POS 201`. Reproduce literally.
4. **No keyboard accelerator for OK / Cancel**: there is no Enter / ESC
   default-button mapping in slot 108 — only Up/Down for listbox advance.
   ESC may fall through to the base class default (which closes — typical
   TPane behaviour), but this is UNCONFIRMED. NOT a bug; do not add
   accelerators on a hunch.
5. **No known artifact specific to selstart.** The MP-host-only context
   means QA coverage is thin; if a halo or color-key artifact appears
   during playtesting, treat it as a reuse of the pink-halo / chroma-key
   bug rather than a new selstart issue.

---

## §12 — Reconstruction pseudocode

The screen is a DEF-driven popup, so the implementor wires the open / pump /
close glue and lets the DEF engine paint. Reconstruction stops short of the
DEF widget engine itself (per [InGameMenuDef_SPEC.md §12](InGameMenuDef_SPEC.md)
and `DefWidgetEngine.md`).

```cpp
// Singleton — TSelStartScreen mirrors cls_0x5a4e24.
// Ctor (mirror recon/ghidra/cls_0x5a4e24.cpp:136-155):
//   screen_w = 640;  screen_h = 480;
//   buf_w = 640;     buf_h = 480;
//   child_array = TArray<TWidget*>(/*cap=*/16);
//   mbr_0x17c = 0;   mbr_0x180 = 0;
//   mbr_0x184 = 0;   mbr_0x188 = 0;

void TSelStartScreen::Open() {
    if (!DefScreen::Open(
            /*screen_name*/   "selstart",
            /*panel_name*/    "default",      // PANEL "default" in selstart.def
                                              //   — NOT the same as the
                                              //   screen name; the DEF lists
                                              //   ONE panel named "default"
            /*flags*/          0x11,           // bit0=tex-pack, bit4=modal
            /*x*/              126, /*y*/ 65,  // 0x7e, 0x41
            /*w*/              394, /*h*/ 316, // 0x18a, 0x13c
            /*bg_w*/           348, /*bg_h*/ 84,// 0x15c, 0x54 (engine bg cell)
            /*bg_resource*/    "widgets",
            /*outer_name*/     "selstart")) {
        return;
    }
    mbr_0x180 = 0;
    mbr_0x17c = 0;
    mbr_0x184 = 0;
    mbr_0x188 = 0;
    // NB: there is NO pause-all-audio call here (selstart isn't a pause
    // menu); the playscreen continues animating its background freeze
    // frame underneath. Audio handling is governed by TPlayScreen
    // Initialize's caller, NOT this screen.
}

// Per-frame Update (vtable slot 76 override).
void TSelStartScreen::Update() {
    DefScreen::Update();                          // base ramp + child walk
                                                  //   (cls_0x5b93c4::virt_meth_0x435d70)
    if (g_AutoPickStartPos &&                     // = DAT_00668128
        mbr_0x188 != 0 &&
        mbr_0x184 == 0)
    {
        mbr_0x188 = 0;
        mbr_0x184 = 1;
        // Read the listbox widget's row count (already populated by slot 148
        // Apply during open); pick a random row.
        TDefListbox* lb = (TDefListbox*)mbr_0x17c; // cached during Apply
        int count = lb->row_count;                 // = lb->mbr_0x98 in retail
        int idx = RandomRange(0, count - 1);
        lb->SetSelection(idx);                     // meth_0x430b80
        // Synthesize an "ok" click to commit + close.
        TButton* ok_btn = FindWidgetByName(this, "ok");  // meth_0x436980
        OnControl(this, /*widget*/ ok_btn, /*event*/ 3000); // slot 148 entry
    }
}

// Slot 148 — Apply / OnControl (UNCONFIRMED-A body — see §6.5).
// Implementor: extract slot 148 disasm before writing this; the inferred
// shape is below.
int TSelStartScreen::OnControl(TWidget* w, int event) {
    if (event == 3000 /*BUTTON click*/) {
        const char* name = w->name;
        if (icmp(name, "ok") == 0) {
            CommitListSelection();                 // write the chosen
                                                   //   STARTPOS to the
                                                   //   player object (the
                                                   //   exact getter/setter
                                                   //   path is
                                                   //   UNCONFIRMED-G)
            mbr_0x5c = /*ok-code*/ 1;              // UNCONFIRMED exact value
            Close();
            return 1;
        }
        if (icmp(name, "cancel") == 0) {
            mbr_0x5c = /*cancel-code*/ 0;          // UNCONFIRMED
            Close();
            return 1;
        }
    }
    // Populate path — called from slot 0 Open after DefScreen_Open returns,
    // and from external Refresh hooks.
    TDefListbox* lb = (TDefListbox*)FindWidgetByName(this, "startlist");
    mbr_0x17c = (uint32_t)lb;                      // cache for Update
    TDefText*   msg = (TDefText*)  FindWidgetByName(this, "startmsg");
    const char* key;
    if (g_MPFlags & MP_RESURRECT)        key = "MPRESURRECT";
    else if (g_MPFlags & MP_PENALTY)     key = "MPPENALTY";
    else if (g_MPFlags & MP_NOPENALTY)   key = "MPNOPENALTY";
    else                                 key = "MPSTARTGAME";
    // Mapping above is UNCONFIRMED-A; the disasm has 4-5 branches that may
    // include a MPSELSTARTFAILED fallback for a join error.
    msg->SetText(g_StringRefs.Lookup(key));
    // Optionally hide the "ok" button on a join-pending state (UNCONFIRMED).
    return 1;
}

// Slot 108 — OnInput (per §6.3).
bool TSelStartScreen::OnInput(int vkey, bool isDown) {
    if (isDown) {
        TDefListbox* lb = (TDefListbox*)FindWidgetByName(this, "startlist");
        if (lb) {
            if (vkey == VK_UP)   { lb->SetSelection(lb->cur - 1); return true; }
            if (vkey == VK_DOWN) { lb->SetSelection(lb->cur + 1); return true; }
        }
    }
    return DefScreen::OnInput(vkey, isDown);       // base default
}

// Slot 164 — OnFieldQuery (per §6.6).
int TSelStartScreen::OnFieldQuery(const char* field, char* out_buf,
                                  int row_index)
{
    out_buf[0] = '\0';
    if (icmp(field, "startpos") != 0) return DefScreen::OnFieldQuery(field);
    TAreaRecord* area = g_ModuleMgr.GetArea(g_ActiveAreaIndex); // DAT_00658da0
    if (row_index < area->startpos_count) {
        const char* startpos_name = area->startpos[row_index].name;
        const char* mpgame_label  = g_StringRefs.Lookup("MPGAME");
        snprintf(out_buf, /*sz*/N, "%s (%s)", startpos_name, mpgame_label);
        return 1;
    }
    // Past-end — show the joining-party member's slot.
    TPartyMember* pm = g_PartyMgr.Find((char)g_Player, /*flag*/ 0);
    const char* alt_name = pm ? pm->name : "??";
    const char* mpparty_label = g_StringRefs.Lookup("MPPARTY");
    snprintf(out_buf, /*sz*/N, "%s (%s)", alt_name, mpparty_label);
    return 1;
}

// At the caller — TPlayScreen Initialize, mirroring cls_0x5a5320_..._47a660:392-404:
void TPlayScreen::Initialize() {
    // ... earlier init ...
    if (g_MPMode &&                                 // = DAT_0066829c
        g_Player != nullptr &&
        (level <= 0 || (g_Player->flags & 0x100000) == 0))
    {
        StopFrameSwap(false);
        g_SelStartScreen.Open();
        RegisterModal(&g_SelStartScreen, /*priority*/ -1);
        PumpModal(&g_SelStartScreen, /*priority*/ 7);
        // Close is implicit when the modal pump exits.
    }
    // ... rest of init ...
}
```

The per-widget paint of TEXT / LISTBOX / BUTTON / FRAME chrome is the
responsibility of the DEF widget renderer (`DefWidgetEngine.md`). This spec
inventories the values that renderer must consume (§4 / §8) but does not
re-spec widget paint here — that belongs in the engine spec, which when
landed covers all DEF screens uniformly.

---

## §13 — Port mapping notes

| retail call (in §5/§6) | port primitive | home |
|---|---|---|
| `(*+0x5c)(buf, sprite, 0, 0)` opaque blit (base paint walk for Background) | `Renderer->DrawBitmapToTarget(bm, x, y)` (opaque) | renderer |
| `FUN_00438d80_BlitEffect_StructInit` for opaque dest rect | `MakeDP(...)` (struct fill) | inline / shared |
| 9-slice for `ClearFrame3` (startmsg), `VScrollRect` (listbox), `ClearFrame2`/`FillFrame2` (buttons), `VScrollBar` (scrollbar track) | `Renderer->DrawBitmapNineSlice(bm, dst_rect, slice, margins)` — needs new primitive (UNCONFIRMED — see InGameMenuDef §13 "Missing primitives") | renderer (new) |
| TEXT widget body | `font.cpp DrawTextShadowedToTarget(atlas, text, cell_x, cell_y, cell_w, cell_h, kAlignCenterHTopV, R, G, B, /*shadow*/ true)` | font.cpp |
| BUTTON label | `font.cpp DrawTextShadowedToTarget(...kAlignCenterHCenterV...)` (up state) / `DrawTextToTarget(...without shadow)` (down state) | font.cpp |
| LISTBOX row text | `font.cpp DrawTextShadowedToTarget(...kAlignLeftHCenterV...)` per row | font.cpp |
| LISTBOX SELCOLOR row background | `Renderer->FillRect(x,y,w,row_h, SELCOLOR)` — needs straight rect-fill primitive | renderer |
| Format `"%s (%s)"` | `std::format` / `snprintf` | inline (per UI_METHOD_MAP §10) |
| STRINGREF resolve (`cls_0x45f7c0::meth_0x49d800`) | `g_StringRefs.Lookup(key)` — needs the localization table from `[[project-localization]]` | localization |
| Random-pick (`FUN_00483300_RandomRange`) | `RandomRange(low, high)` — port already has a RNG | math/util |
| Modal pump (`FUN_0048eea0` / `FUN_0048ed90`) | application-level event loop (wrap as a "modal screen" helper) | screen manager |

**Missing primitives** (→ "needs new primitive"):
- DEF widget renderer (whole engine — outstanding from
  `DefWidgetEngine.md:106-115`).
- 9-slice draw with explicit slice + margins
  (`[[project-ui-layout-system]]`).
- LISTBOX widget paint (rows + scrollbar + selection highlight) — first
  in this UI wave to need it; reuse infrastructure across all
  list-bearing DEF screens (loadgame, savegame, options, this).
- TEXT widget paint with local-STYLE override (the
  `STYLE TEXT BGBITMAP "ClearFrame3"` in selstart.def:4-6 overrides the
  widgets.def default — the renderer must honor per-panel STYLE
  overrides).

---

## §14 — UNCONFIRMED / open questions

- **UNCONFIRMED-A — slot 148 body (`virt_meth_0x469460`).** Ghidra did not
  emit a body for slot 148 into `recon/ghidra/cls_0x5a4e24.cpp`. The
  string-XREF set (§6.5) bounds the behaviour — the body MUST do (i) populate
  the `startmsg` text by selecting among 4-5 STRINGREF keys based on three
  MP-flag globals, (ii) cache the LISTBOX widget pointer to `mbr_0x17c`,
  (iii) handle button click events for `ok` / `cancel`, (iv) possibly hide
  the OK button conditionally, and (v) perform a Sleep(0) yield somewhere
  (the `PTR_Sleep_005a310c` XREF). The exact branch order, the precise
  globals → message mapping, and the `ok` / `cancel` return-code values are
  UNCONFIRMED. **Impact:** without this body, the implementor cannot match
  retail behaviour for the message variant the user sees, cannot bind the
  OK / Cancel buttons correctly, and cannot implement the auto-pick path
  (which calls slot 148). **Resolve:**
  `DecompileAddr.java 0x00469460` (Ghidra), or
  `objdump -d --start-address=0x00469460 --stop-address=0x004697a0
  data/Revenant.exe`. Verify the extracted body against the string-XREF set
  at `recon/ghidra/_data.txt:77910-77975, 128336-128338, 128775`.

- **UNCONFIRMED-B — `bg_w=0x15c=348`, `bg_h=0x54=84` semantics.** These
  args 8 and 9 to `DefScreen_Open` (§6.1) are stored somewhere by
  `DefScreen_LoadAndShow` but not visible in the Ghidra-decoded signature.
  Same UNCONFIRMED carried from InGameMenuDef_SPEC §14 G. **Impact:** wrong
  bg tile/clip rect under the chrome (likely invisible — chrome is opaque).
  **Resolve:** `DecompileAddr.java 0x00435660` (the bg-attach helper called
  after the `+0xa0` lookup in `FUN_00435040:32`) and check whether `bg_w` /
  `bg_h` are consumed.

- **UNCONFIRMED-C — LISTBOX scrollbar sub-region resolution.** The
  scrollbar's position is given by `RELRB 21 4 5 4` in the STYLE LISTBOX
  VLIST (`widgets.def:123`) — relative to the listbox right/bottom corners
  with offsets 21 (from right), 4 (from top), 5 (from right? — the meaning of
  the 4 numbers in a `RELRB` is the DEF parser's contract). The §4 table
  gives an approximate `(290, 149, 21, 76)` for the scrollbar rect, but the
  exact resolution requires reading the DEF parser. **Impact:** the
  scrollbar may sit a few px off; the listbox content cell width may need a
  matching subtract. **Resolve:** extract the DEF parser's `RELRB` /
  `RELRR` / `RELB` interpretation (in `FUN_00437620_DefWidget_ParseBuffer`
  or the widget factory `FUN_00438220_DefWidget_TDefScrollbar_factory`).

- **UNCONFIRMED-D — texture-pack flags 0x10400 on the `selstarttex.dat`
  Background entry.** `dump_dat.py` rejects it as "unsupported bit-depth"
  (flags=0x10400 = `BM_COMPRESSED | BM_5BITPAL`). This is a high-res mipmap
  variant for retail's hi-res branch and not the Classic Background. The
  Classic Background from `selstartnotex.dat` is fully decoded and is the
  asset we draw. **Impact:** none if we standardize on Classic; only matters
  if the port adds a hi-res render path (out of scope per task framing).
  **Resolve:** N/A unless hi-res branch is reactivated.

- **UNCONFIRMED-E — `DAT_00659cb8` arg in `LoadDef_selstart`.** Passed as
  `param_2` to `DefScreen_Open` (§6.1, position 2). The other DEF-screen
  openers (LoadDef_ingamemenu, LoadDef_loadgame, LoadDef_savegame) pass a
  different per-screen DAT here (`DAT_0066f748` for ingamemenu,
  `DAT_0066f8d0` for loadgame). These DATs are the **singleton instances**
  of those screens — passed in as the `this` for the inner method's per-
  screen state. For selstart, the singleton instance IS `DAT_006597c0` (per
  the modal-pump bootstrap at `Initialize_47a660.cpp:400`), so
  `DAT_00659cb8` is something else — likely a per-screen DEF-engine state
  block, a panel-name backing buffer, or a callback registration. **Impact:**
  wrong second arg to DefScreen_Open could break engine state binding.
  **Resolve:** `xref` on `DAT_00659cb8` in Ghidra; cross-reference with
  the InGameMenu's `&DAT_00659c80` (line `recon/discovered/FUN_00465a50_LoadDef_createchar.cpp:56`)
  — same offset family, likely same role.

- **UNCONFIRMED-F — `mbr_0x17c` identity.** In §6.4 the same field is
  treated as both an area-STARTPOS record (`mbr_0x17c + 0x98` is read as a
  count) AND as a listbox widget (passed to `meth_0x430b80`). The most
  consistent reading is that `mbr_0x17c` holds the **listbox widget**
  pointer, whose `+0x88`/`+0x98` are its own row-count/row-array fields
  (which happen to mirror the area STARTPOS shape, because the listbox is
  populated from it). **Impact:** if wrong, the auto-randomize path picks
  random in the wrong array. **Resolve:** extract slot 148 (which caches
  this pointer); the cache target's struct shape will disambiguate.

- **UNCONFIRMED-G — STARTPOS commit path.** The chosen listbox index ends
  up as the player's spawn point. The exact write target (a global like
  `DAT_006596dc`, a field on the player object, a callback into the script
  manager, or a side-effect of `meth_0x430b80` itself via the area record
  pointer) is not pinned. **Impact:** the user can pick a start-pos but the
  game spawns them at default. **Resolve:** extract slot 148 + inspect
  `meth_0x430b80`'s consumers; trace `DAT_006596dc` writers as well (it's
  near `DAT_006597c0` in `_data.txt`).

- **UNCONFIRMED-H — `0x469ab0` / sibling function role.** Functions
  `virt_meth_0x469ab0`, `virt_meth_0x46a720`, `virt_meth_0x46bd60` (per
  `_data.txt:128610-128627`) are XREF'd to the singleton `DAT_006597c0` and
  share a common namespace with the slot bodies. These are presumably the
  external "trigger this screen / commit this pick / handle MP-join event"
  hooks called from the MP gameflow. They are not in `recon/ghidra/cls_0x5a4e24*.cpp`
  (suggesting they are NOT methods on `cls_0x5a4e24` despite touching its
  singleton — likely free functions in the MP boot pipeline). **Impact:**
  partial understanding of the broader gameflow integration; doesn't block
  the modal's visual reconstruction. **Resolve:** if the gameflow track
  needs them, extract via `DecompileAddr.java 0x00469ab0` /
  `0x0046a720` / `0x0046bd60`.

- **UNCONFIRMED-I — `(***+0x1c0)()` predicate at `:394, :396`.** Per
  UI_METHOD_MAP §15b, `+0x1c0` on the character object is the **Health**
  getter. The gate `iVar3 < 1` then reads "if health < 1" — the screen
  opens when the player is dead (which makes sense for MP resurrect /
  start-game scenarios). The companion bit check `DAT_00667fcc[0xdb] &
  0x100000U` is the "needs to pick start position" world bit. **Impact:**
  understanding when the modal triggers vs not — doesn't affect rendering.
  **Resolve:** verify Health is `+0x1c0` on the live character class
  (already HIGH-confidence per UI_METHOD_MAP §15b table).

- **UNCONFIRMED-J — ESC behaviour.** `virt_meth_0x4693e0` slot 108
  (`recon/ghidra/cls_0x5a4e24.cpp:54-76`) handles only VK_UP / VK_DOWN
  for the listbox, then falls through to `cls_0x5b93c4::virt_meth_0x4361f0`
  (base input dispatch). Whether the base routes ESC to Cancel is
  UNCONFIRMED. **Impact:** keyboard UX. **Resolve:** read
  `cls_0x5b93c4::virt_meth_0x4361f0` body (slot 108 base — at `0x004361f0`).

---

## End

Forensics complete envelope for the MP Select-Start popup. The §6.5 slot-148
body is partial (UNCONFIRMED-A); the visible widgets + chrome + frames + DEF
text rendering + listbox row format + per-frame Update + arrow-key input are
fully pinned. The reconstruction agent should land §6.5 (extract
`virt_meth_0x469460` via `DecompileAddr.java 0x00469460`) before wiring the
button-click handler and the auto-randomize path. The TASK BRIEF FRAMING IS
WRONG (selstart is **MP**, not SP character-pick) — see §0 for the three
independent confirmations.
