# SaveGameDef — Forensics Spec

**Status:** forensics-complete (for the screen *envelope* — DEF chrome + open/close
+ command-dispatch overrides + screenshot capture + asset roster). The interior
DEF widget *rendering* (text/edit/listbox/button paint paths) is handled by the
shared **DEF widget engine** (`docs/ui/port_status/DefWidgetEngine.md`) and is
*out of scope* of this per-panel spec — they will be specced once, in the engine
brief, and reused by every DEF screen (savegame, loadgame, options, popup, etc.).
This spec pins everything *specific to the SaveGame screen*: the dedicated
override class, the Init/Close/Dispatch overrides, the screenshot prologue, the
chrome bitmap, the exact widget tree from `savegame.def`, and the wiring
between dialog widgets ↔ disk-save action.

---

## §0 — Sources & status

- **Class:** `cls_0x5b963c` (vtable at `0x005b963c`) — **the brief's claim of "no
  dedicated class" is wrong**, see §1. Instance size: **420 bytes**
  (`recon/ghidra/cls_0x5b963c.cpp:30`). The instance lives as an embedded global
  at `DAT_0066fb08` (the modal-state slot index sentinel is the immediately
  preceding 4-byte `DAT_0066fb04 = 0xffffffff` selection-index global,
  `cls_0x5b963c.cpp:56`).
- **Class chain (descent):**
  `TPane (0x5a4494) ← TButtonPane (cls_0x5b98b8, slot-0 = FUN_00434e40_TButtonPane_Initialize,
  recon/ghidra/cls_0x5b98b8__vftable_5b98b8.cpp:6) ← TDefScreen (cls_0x5b93c4, adds
  DefScreen_Open at slot 132, recon/ghidra/cls_0x5b93c4__vftable_5b93c4.cpp:40) ←
  TSaveGameScreen (cls_0x5b963c)`.
- **Vtable overrides (savegame vs base TDefScreen,
  `recon/ghidra/cls_0x5b963c__vftable_5b963c.cpp`):**

  | slot | offset | savegame fn | role | base (TDefScreen) | base role |
  |---:|---:|---|---|---|---|
  | 0 | `+0x00` | `FUN_005399f0` | **Initialize / Open** | `0x5339b0` | (base init) |
  | 1 | `+0x04` | `FUN_00539ab0` | **Close** | (base close) | — |
  | 31 | `+0x7c` | `FUN_0053a1d0` | destructor wrapper | (auto) | — |
  | 37 | `+0x94` | `FUN_00539c00` | **Command dispatch** (Save / Exit / picture-update branches) | `0x4451e0` | (generic activate) |
  | 40 | `+0xa0` | `FUN_00539bc0` | per-control activation / **listbox-item activation** | `0x436090` | (generic) |
  | 41 | `+0xa4` | `FUN_00539ae0` | **ListBox FIELD/format callback** (the "name" column) | `0x445080` | (generic) |
- **Sibling that confirms pattern:** `cls_0x5b9584` (LoadGame screen) — identical
  layout, identical override pattern, `recon/ghidra/cls_0x5b9584.cpp` +
  `cls_0x5b9584__vftable_5b9584.cpp`. Where savegame loses a method the LoadGame
  side often has it (load-side `0x539590` = command dispatch, has more visible
  string anchors that map back identically). Used for cross-validation.
- **Activator on the play-screen side:** `FUN_0047cf40_TPlayScreen_DispatchCommand`
  **case `0x54`**
  (`recon/discovered/FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:480-507`) —
  takes the screenshot **prologue** (see §5/§6) then invokes the activator stub
  `FUN_005399f0_LoadDef_savegame` (the freestanding wrapper that just calls
  `cls_0x5b963c::Init` via the global instance) and pumps the modal loop via
  `FUN_0048f040(&DAT_0066fb08, …)` then closes via `cls_0x5b963c::Close`
  (`recon/ghidra/cls_0x5a5320_likely_TCharacter.cpp:8031-8033` and `:8084-8086`
  show the same pump in the `meth_0x47e6b0` wrapper).
- **Recon files read:**
  - `recon/discovered/FUN_005399f0_LoadDef_savegame.cpp` (the per-screen
    activator stub).
  - `recon/discovered/FUN_0047cf40_TPlayScreen_DispatchCommand.cpp` (case 0x54 —
    screenshot prologue).
  - `recon/discovered/FUN_00435150_DefScreen_Open.cpp` (the DEF engine entry —
    body confirms the 11-arg signature dispatched to from `0x5399f0`).
  - `recon/discovered/FUN_00435040_DefScreen_LoadAndShow.cpp` (the
    geometry-routing helper called inside `DefScreen_Open`).
  - `recon/discovered/FUN_004377c0_DefWidget_LoadFile.cpp` (concatenates
    `widgets.def` + `savegame.def` into the buffer at `this+0xc8`).
  - `recon/discovered/FUN_00438df0_SurfaceBlitWrapper.cpp` (the
    `SDrawParam`-fill thin blit wrapper used by the screenshot prologue).
  - `recon/discovered/save_system_notes.md` (the on-disk save-slot layout and
    the `<slot>/ss.bmp` writer specification).
  - `recon/discovered/renames/agent_player_init.txt:109-117` (the `SaveGame`
    name + size 2066-byte body).
  - `recon/discovered/renames/agent_ui_wave2_defengine.txt:60-300`
    (DEF-engine call-graph reference).
  - `recon/ghidra/cls_0x5b963c.cpp` (the savegame screen class — Init/Close/
    ctor/dtor only; **command dispatch slot 0x539c00 + ListBox callbacks
    0x539ae0/0x539bc0 are NOT extracted by Ghidra** — see UNCONFIRMED items 1
    and 2).
  - `recon/ghidra/cls_0x5b963c__vftable_5b963c.cpp` (full vtable layout).
  - `recon/ghidra/cls_0x5b9584.cpp` + `cls_0x5b9584__vftable_5b9584.cpp`
    (LoadGame sibling for cross-check).
  - `recon/ghidra/_data.txt:114450-114730` (every widget-name string + xrefs
    for slot 0x539ae0 / 0x539bc0 / 0x539c00 / 0x539590 — these are the
    decisive evidence for the un-extracted command-dispatch behavior).
  - `recon/discovered/port_status/DefWidgetEngine.md` (engine-wide port plan,
    incl. the 8 widget class roster: BITMAP, FRAME, TEXT, BUTTON, SCROLLBAR,
    LISTBOX, EDIT, DROPLIST).
- **Assets read:** `data/resources_unzipped/savegame.def`,
  `data/resources_unzipped/widgets.def`, `data/resources_unzipped/savegametex.dat`,
  `data/resources_unzipped/savegamenotex.dat`,
  `data/resources_unzipped/widgetstex.dat` — all measured with
  `tools/ui/dump_dat.py` (see §2).
- **Snapshot note (supplementary — superseded by retail):** no 1998/99 snapshot
  `src/savegame*.cpp` UI exists for this screen. `src/savegame.{h,cpp}` in the
  current port covers only the *disk* SaveGame/ReadGame (see `save_system_notes.md`
  §"High-level Save/Load") — it is the WriteGame/ReadGame `param_2` callee of the
  Save action; the screen itself was never ported. Nothing to migrate from
  snapshot for the UI side.
- **Errors to close:** none — there is no prior port of this screen to correct.

---

## §1 — Overview

A **modal save-game dialog**. Shown by clicking Save in the in-game pause menu
(triggers `TPlayScreen::DispatchCommand` command `0x54 = 84`,
`FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:480`).

The dialog presents:

- a **thumbnail picture** of the slot's saved gameworld (the `ss.bmp` of
  whatever the highlighted slot is, or the just-captured live thumbnail for the
  "new save" path);
- **module-name** and **character-name** captions below the picture;
- a **multi-line "gamename"** caption above the picture;
- an **EDIT** for the new save-slot name (`nameedit`, default "New Game",
  maxlen 30);
- a **vertical LISTBOX** of existing slots (`gamelist`), with a single column
  `gamelist_name`;
- two **BUTTONS**: **Save Game** + **Exit**.

The screen is opened modally via the standard DEF-screen pump:
`Init` (`cls_0x5b963c::virt_meth_0x5399f0`) → `FUN_0048f040(&DAT_0066fb08, …)`
runs the modal loop dispatching keyboard/mouse events through TPane → on
Save/Exit hit, the **command dispatch** (`cls_0x5b963c::virt_meth_0x539c00`)
runs (writes the .sav, deletes prior slot, or just exits) → `Close`
(`cls_0x5b963c::virt_meth_0x539ab0`) destroys the screenshot scratch surface.
Confirmed shape: `FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:498-505`
("`FUN_005399f0_LoadDef_savegame(); FUN_0048f040(&DAT_0066fb08, …);
FUN_00539ab0();`").

A key behavior unique to the Save dialog (NOT shared by Load):
**screenshot capture of the current play-screen happens BEFORE the dialog
opens.** The thumbnail goes to disk as `<RunPath>\ss.bmp`
(`FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:498`,
`FUN_004a2960("ss.bmp", 3)`). After the user picks a slot and confirms, the
save-action handler copies that top-level `ss.bmp` into
`<SavePath>\Save\Single\<slot>\ss.bmp` (per `save_system_notes.md` §"Save game"
step 7 + the second `s_ss.bmp_005e43cc` reference inside dispatcher
`virt_meth_0x539c00` at `_data.txt:114715` — confirms the dispatcher mentions
`ss.bmp` and writes the saved-slot copy).

---

## §2 — Asset roster

The screen pulls bitmaps from **three** archives — measured WxH via
`tools/ui/dump_dat.py`:

| asset | archive | entry name | WxH (measured) | role | source rects | cite |
|---|---|---|---|---|---|---|
| `Background` | `savegamenotex.dat` (also `savegametex.dat` 5BITPAL variant + `loadgamealpha.dat` alias — identical bytes) | `Background` | **640×480** | Full-screen chrome (stone-frame + "SAVE GAME" gold gold-on-black banner @ ~y=0..28) drawn FIRST as the background of the modal | whole | `dump_dat.py savegamenotex.dat`: `[0] Background : 640x480 flags=0x2 kc=0x0` |
| `ClearFrame2` | `widgetstex.dat` | `ClearFrame2` | **64×57** | inner panels — "pictureframe" (50,87,222,166) + below-pic frame (50,268,221,106); drawn 9-slice via STYLE FRAME default (FRAME 14 14 16 16) | 9-slice from `widgets.def:81-82` STYLE FRAME defaults (BITMAP "ClearFrame1" FRAME 14 14 16 16 MARGINS 0 0 4 4); overridden per-widget to `ClearFrame2` | `dump_dat.py widgetstex.dat`: `[3] ClearFrame2 : 64x57` |
| `ClearFrame3` | `widgetstex.dat` | `ClearFrame3` | **64×57** | text-cell background (STYLE TEXT default, BGBITMAP "ClearFrame3" FRAME 5 5 5 5) | 9-slice | `dump_dat.py`: `[6] ClearFrame3 : 64x57`; `widgets.def:84-86` |
| `ClearFrame4` | `widgetstex.dat` | `ClearFrame4` | **64×57** | edit-box backdrop (STYLE EDIT default, BGBITMAP "ClearFrame4" FRAME 5 5 5 5) | 9-slice | `widgets.def:127-129`; `[9] ClearFrame4 : 64x57` |
| `VScrollBar` | `widgetstex.dat` | `VScrollBar` | **22×74** | listbox vertical scroll bar track (9-slice via FRAME 6 22 6 22) | 9-slice | `widgets.def:107-111`; `[24] VScrollBar : 22x74` |
| `VScrollRect` | `widgetstex.dat` | `VScrollRect` | **74×82** | LISTBOX outer chrome (STYLE LISTBOX VLIST BGBITMAP "VScrollRect" FRAME 8 26 26 26) | 9-slice | `widgets.def:120`; `[23] VScrollRect : 74x82` |
| `VScrollUpU` / `VScrollUpD` | `widgetstex.dat` | `VScrollUpU` / `VScrollUpD` | **16×11** / 16×11 | scrollbar up button (1,1 offset) | atom blit | `[27]/[28]`; `widgets.def:109` |
| `VScrollDownU` / `VScrollDownD` | `widgetstex.dat` | `VScrollDownU` / `VScrollDownD` | **16×11** / 16×11 | scrollbar down button (1, RELB 12) | atom blit | `[29]/[30]`; `widgets.def:110` |
| `VScrollThumb` (U/D) | `widgetstex.dat` | `VScrollThumbU` / `VScrollThumbD` | **14×13** / 14×13 | scrollbar thumb (19 30 2 — w/h/?) | atom blit | `[25]/[26]`; `widgets.def:111` |
| **slot thumbnail** | (runtime, **not** an archive) | — | logical 216×160 ("`picture`" rect 54,91,216,160) drawn from a 640×480 source (downsampled by the listbox-item-changed callback) | `<SavePath>\Save\Single\<slotname>\ss.bmp` for an EXISTING slot; `<RunPath>\ss.bmp` for the just-captured live thumbnail | — | `FUN_005399f0:14` allocates a `0xd8×0xa0 = 216×160` BMP-format scratch surface to hold the rescaled thumbnail; `_data.txt:114715` shows `s_ss.bmp_005e43cc` referenced by the command dispatcher at `0x539db3` |

Source-rect maps:

```
ClearFrame2 (64×57) — 9-slice corners 14 14 16 16, margins 0 0 4 4
  ┌────┬──────┬────┐
  │ TL │  T   │ TR │   y=0..14 / x=0..14, 14..(64-16)=14..48, 48..64
  ├────┼──────┼────┤
  │ L  │  C   │ R  │   y=14..(57-16)=14..41
  ├────┼──────┼────┤
  │ BL │  B   │ BR │   y=41..57
  └────┴──────┴────┘

VScrollRect (74×82) — 9-slice 8 26 26 26  (LISTBOX outer chrome)
   x: 0..8, 8..(74-26)=8..48, 48..74
   y: 0..26, 26..(82-26)=26..56, 56..82

VScrollBar (22×74) — 9-slice 6 22 6 22  (VSCROLLBAR track)
```

Note for the reconstruction agent: the 9-slice numbers in `widgets.def` follow
the legacy Cinematix convention `FRAME L T R B` — **L+R sum < bitmap width**
and **T+B sum < bitmap height**; the middle stretches.

---

## §3 — Coordinate frames & surfaces

**Pane geometry (logical / DEF coordinate space):**

- The DEF panel `default` declares a single root frame `FRAME POS 35 70 562 320`
  (`savegame.def:7`) — but all child widget POS coords are given in **DEF-screen
  coordinates** (`screen-DEF`), origin `(0,0)` at the top-left of the **640×480
  modal screen**, *not* relative to the parent frame. Cross-check: the LISTBOX
  is at `POS 284 117 298 257` (`savegame.def:29`); `284+298 = 582`, well past
  the parent frame's right edge at `35+562 = 597`, so children are not
  parent-relative. The parent FRAME is a *decorative outline* drawn at
  `(35,70)` only.
- The 640×480 `Background` bitmap fills the whole modal — its visible chrome
  is the stone frame at (~12,~6) to (~628,~474), with the "SAVE GAME" plate
  centered horizontally at top (banner ≈ (228,0)..(412,40), measured from
  rendered PNG dump).
- **DefScreen_Open call arguments** (the geometry contract, decoded from
  `FUN_005399f0_LoadDef_savegame.cpp:23-24`):

  ```
  FUN_00435150_DefScreen_Open(
      panel_name = "savegame"            (s_savegame_005e4330)
      screen_name = "default"            (s_default_005e4328)
      flags       = 0x11      = 17       (CTRLFLAG_SELECTABLE | CTRLFLAG_DISABLED?
                                          UNCONFIRMED-D — see UNCONFIRMED #4 for
                                          DefScreen flag semantics)
      x           = 0
      y           = 0
      w           = 0x280    = 640       — screen target W
      h           = 0x1e0    = 480       — screen target H
      bg_w        = 0x1c2    = 450       — Background sprite footprint W (UNCONFIRMED-D)
      bg_h        = 0xa0     = 160       — Background sprite footprint H (UNCONFIRMED-D)
      bg_sprite_name = "widgets"
      outer_name  = "savegame"
  );
  ```

  > Per `recon/discovered/renames/agent_ui_wave2_defengine.txt:221-225` the
  > documented signature is `DefScreen_Open(this, panel_name, screen_name,
  > flags, x, y, w, h, bg_w, bg_h, bg_sprite_name, outer_name)`. The `bg_w/bg_h`
  > pair (450,160) does NOT match the 640×480 `Background` measured dims — see
  > UNCONFIRMED #5. Most plausible reading: `(bg_w, bg_h)` are the chrome
  > "inner content rect" used to center the cursor / clip the LISTBOX scrollbar
  > area, not the bitmap dims.

- **Sprite "background"** is then resolved by `DefScreen_LoadAndShow` at
  `FUN_00435040_DefScreen_LoadAndShow.cpp:31`:
  `(**(code **)(*param_1 + 0xa0))(s_background_005cd8d4, 0xffffffff);` — a
  vtable+0xa0 (`virt_meth_0x445080` in the base = `BindBackground`) call. The
  string literal is **"background"** (lowercase). Asset resolution
  follows: `FUN_0046d710("background")` returns the **`Background` entry**
  in the active `<screenname>tex.dat` archive — i.e. `savegametex.dat[0]
  Background`. UNCONFIRMED-M: archive-mount mechanism (which archive is "active"
  when this name is resolved) is the standard `widgetsXXX.dat` + per-screen
  `XXXtex.dat`/`XXXnotex.dat`/`XXXalpha.dat` overlay; spec the choice as
  `savegamenotex.dat` for the default 5-bit-no-alpha path, fall back per
  bitdepth.

**Scratch surface (`mbr_0x19c`):**

- Allocated by `FUN_005399f0_LoadDef_savegame.cpp:13-15`:
  ```
  iVar1   = (*(int *)PTR_DAT_005d79e0)[6]()  // display->GetFormat() / +0x18 vtable
  uVar2   = FUN_004a1ec0( 0xd8=216, 0xa0=160,
                          ((-(iVar1 != 0xf) & 2) + 2)  /*format selector*/ )
  this->mbr_0x19c = uVar2;
  ```
- The format expression `(-(iVar1 != 0xf) & 2) + 2`:
  - if `displayFormat == 0xf`: `(0 & 2) + 2 = 2`  → format **2 = `BM_15BIT`**
    (`src/revdefs.h:308`)
  - else: `((-1) & 2) + 2 = 4`  → format **4** (UNCONFIRMED-M, likely
    `BM_16BIT` companion; see UNCONFIRMED #6)
- This **216×160** surface is the **picture preview** target — the live screen
  thumbnail (downsampled to 216×160) lives here while the dialog is open, then
  `FUN_004a22f0` (line :20) initializes/clears it with `(extraout_ECX, 0x80000000,
  0xffff)` — a fill of `0x80000000=DM_USEDEFAULT` (opaque-clear). Freed in
  `Close` at `cls_0x5b963c.cpp:121`: `FUN_004830f0(this->mbr_0x19c); this->mbr_0x19c = 0;`.
- The `0xd8 × 0xa0 = 216 × 160` dimensions are exactly the `BITMAP POS 54 91
  216 160 NAME "picture" FIELD "picture"` rectangle from `savegame.def:11` —
  decisive evidence the scratch surface IS the live picture target.

**Frame table:**

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| `screen-DEF` | the framebuffer-targeted modal surface | TL | `(0,0)` (modal is full-screen 640×480) | every widget POS in `savegame.def` | `savegame.def:7-38` |
| `pane` | `screen-DEF` | TL | `(0,0)` — `DefScreen_Open` x=0, y=0 | the screen instance's internal element list (TPane has +0x4/+0x8 = x,y; ctor at `cls_0x5b963c.cpp:88-95` sets `mbr_0x4=0, mbr_0x8=0`) — identical to `screen-DEF` | `cls_0x5b963c.cpp:90,93` |
| `chrome` (Background) | `screen-DEF` | TL | `(0,0)` — drawn full-screen | the stone frame + "SAVE GAME" banner art | `FUN_00435040_DefScreen_LoadAndShow.cpp:31` |
| `picture-cell` | `screen-DEF` | TL | `(54, 91)` — from `BITMAP POS 54 91 …` (`savegame.def:11`) | the live thumbnail | `savegame.def:11` |
| `picture-scratch` | (off-screen) | — | own origin `(0,0)`; size `(0xd8, 0xa0) = (216,160)` | the downsampled thumbnail bytes (or the raw `ss.bmp` for a slot) | `FUN_005399f0:14` |
| `pictureframe` | `screen-DEF` | TL | `(50, 87)` — `FRAME POS 50 87 222 166` | the inset 9-slice frame around the picture | `savegame.def:10` |
| `info-frame` | `screen-DEF` | TL | `(50, 268)` — `FRAME POS 50 268 221 106` | the bottom-left captions block | `savegame.def:13` |
| `gamename-cell` | `screen-DEF` | TL | `(55, 277)` — `TEXT POS 55 277 210 40` | the multi-line saved-game title caption (cleared with the BG) | `savegame.def:15` |
| `modlabel-cell` | `screen-DEF` | TL | `(55, 315)` — `TEXT POS 55 315 75 20` | the literal label "Module:" (right-justified) | `savegame.def:18` |
| `modname-cell` | `screen-DEF` | TL | `(130, 315)` — `TEXT POS 130 315 130 20` | the module name value (left-justified) | `savegame.def:19` |
| `charlabel-cell` | `screen-DEF` | TL | `(55, 335)` — `TEXT POS 55 335 75 20` | literal "Character:" (right-justified) | `savegame.def:21` |
| `charname-cell` | `screen-DEF` | TL | `(130, 335)` — `TEXT POS 130 335 130 20` | the character name (left-justified) | `savegame.def:22` |
| `gamename-edit-label-cell` | `screen-DEF` | TL | `(284, 89)` — `TEXT POS 284 89 85 20 TEXT "Game Name"` | static "Game Name" label | `savegame.def:26` |
| `nameedit-cell` | `screen-DEF` | TL | `(372, 89)` — `EDIT POS 372 89 208 20` | EDIT widget (new-save name) | `savegame.def:27` |
| `gamelist` | `screen-DEF` | TL | `(284, 117)` — `LISTBOX POS 284 117 298 257` | vertical LISTBOX of slot names | `savegame.def:29` |
| `gamelist-item` | `gamelist`-content | TL | row N → content-y `N * 18`; content-x `(2, 0)` from `FIELD 2 0 272 18` inside LISTBOX `BEGIN..END` | one row's text cell (width 272, height 18) | `savegame.def:29-32` |
| `savegame-button` | `screen-DEF` | TL | `(158, 406)` — `BUTTON POS 158 406 140 30 NAME "savegame"` | the SAVE GAME button | `savegame.def:37` |
| `exit-button` | `screen-DEF` | TL | `(350, 406)` — `BUTTON POS 350 406 140 30 NAME "exit"` | the EXIT button | `savegame.def:38` |

**Composition formula:**

Every widget composes trivially:
```
screen_x = widget_def_pos_x          (the DEF POS is already in screen-DEF)
screen_y = widget_def_pos_y
```

…with two non-trivial scratch composites:

1. **Picture scratch → picture-cell:** `(picture-scratch:(0,0)..(216,160)) ⟶ screen-DEF:(54,91)..(270,251)`
   — i.e. the scratch is blitted *as-is* (no scaling at blit time; scaling
   happens earlier when the thumbnail is decoded INTO the scratch).
2. **9-slice chrome rects** (per FRAME / EDIT / LISTBOX / BUTTON widget) — the
   shared DEF widget engine does the 9-slice; the panel spec doesn't.

**Verification:** each child's POS uses a bare constant (no `parent_w - k` or
`parent_h - k` form anywhere in `savegame.def`), so **every widget is
TL-anchored to `screen-DEF`**. That makes the chip model trivial here: the DEF
screen is one chip with origin = screen origin.

**Layout diagram (640×480 screen-DEF):**

```
   x   0  35    50  54        270 284  372       582 597    640
   y                                                        
   0   ╔══════════════════════════════════════════════════════════╗
       ║ Background (stone chrome, "SAVE GAME" banner ~228..412)  ║
  70   ║ ┌──────────────────────────────────── (decorative parent)║
       ║ │ (FRAME 35,70,562,320 — outline only)                   ║
  87   ║ │ ┌──pictureframe (50,87,222,166)──┐                     ║
  89   ║ │ │ ┌──picture (54,91,216,160)─────┤ ┌Game Name (284,89,85,20)
       ║ │ │ │                              │ │ [nameedit (372,89,208,20) ─ EDIT]
       ║ │ │ │   LIVE THUMBNAIL              │ │                   ║
       ║ │ │ │   (216×160 from ss.bmp)       │ ┌───────────────────╗
       ║ │ │ │                              │ │                   ║
       ║ │ │ │                              │ │ LISTBOX gamelist  ║
       ║ │ │ │                              │ │ (284,117,298,257) ║
 251   ║ │ │ └──────────────────────────────┤ │                   ║
       ║ │ └────────────────────────────────┘ │                   ║
 268   ║ │ ┌──info-frame (50,268,221,106)──┐ │                   ║
 277   ║ │ │  ▼ gamename (55,277,210,40)    │ │                   ║
       ║ │ │      "No Saved Game"           │ │                   ║
       ║ │ │      (CENTER, SHADOW, ELIPSES) │ │                   ║
 315   ║ │ │  Module: (55,315,75,20) [▶][modname (130,315,130,20)]║
 335   ║ │ │  Character: ▶[charname (130,335,130,20)]              ║
 374   ║ │ └─────────────────────────────────┘ │                   ║
       ║ │                                    │                   ║
 374   ║ │                                    └───────────────────╝
 390   ║ │                                                          ║
 406   ║ │   ┌──[SAVE GAME] btn (158,406,140,30)──┐   ┌─[EXIT] btn (350,406,140,30)┐
 436   ║ │   └─────────────────────────────────────┘   └────────────────────────────┘
       ║ └─────────────────────────────────────────────────────────╝
 480   ╚══════════════════════════════════════════════════════════╝
```

---

## §4 — Static element layout (per-widget, in `screen-DEF` space)

Every (x,y,w,h) below is *final composed* in `screen-DEF` and copied verbatim
from `savegame.def`. Style fields (BGBITMAP / FRAME geometry / DRAWMODE / RECT)
are inherited from `widgets.def` STYLE defaults unless explicitly overridden.

| widget | type | space | (x,y) | (w,h) | style source | overrides / extra | cite |
|---|---|---|---|---|---|---|---|
| (decorative) parent frame | FRAME | screen-DEF | (35,70) | (562,320) | STYLE FRAME (`widgets.def:81`) BITMAP "ClearFrame1" FRAME 14 14 16 16 MARGINS 0 0 4 4 | none — uses STYLE defaults | `savegame.def:7` |
| pictureframe | FRAME | screen-DEF | (50,87) | (222,166) | STYLE FRAME (`widgets.def:81`) | **BITMAP "ClearFrame2"** (override) | `savegame.def:10` |
| picture | BITMAP | screen-DEF | (54,91) | (216,160) | none (bare BITMAP — DRAWMODE default) | FIELD "picture" → bound at runtime to picture-scratch surface | `savegame.def:11` |
| info-frame | FRAME | screen-DEF | (50,268) | (221,106) | STYLE FRAME (`widgets.def:81`) | BITMAP "ClearFrame2" (override) | `savegame.def:13` |
| gamename | TEXT | screen-DEF | (55,277) | (210,40) | STYLE TEXT (`widgets.def:84-86`) BGBITMAP "ClearFrame3" FRAME 5 5 5 5 MARGINS 0 0 4 4 FONT "Med" RECT 3 0 3 0 COLOR (255,255,255) FLAGS TEXT_LEFT\|TEXT_VCENTER\|TEXT_SHADOW DRAWMODE DM_USEDEFAULT | flags **OVERRIDE** to `TEXT_CENTER\|TEXT_SHADOW\|TEXT_ELIPSES` + CTRLFLAG_CLEARBG; default text "No Saved Game" | `savegame.def:15-16` |
| (modlabel) "Module:" | TEXT | screen-DEF | (55,315) | (75,20) | STYLE TEXT (`widgets.def:84-86`) | flags = TEXTFLAG_RIGHT; literal text "Module:" | `savegame.def:18` |
| modname | TEXT | screen-DEF | (130,315) | (130,20) | STYLE TEXT defaults | CTRLFLAG_CLEARBG; default text "none" | `savegame.def:19` |
| (charlabel) "Character:" | TEXT | screen-DEF | (55,335) | (75,20) | STYLE TEXT defaults | flags = TEXTFLAG_RIGHT; literal "Character:" | `savegame.def:21` |
| charname | TEXT | screen-DEF | (130,335) | (130,20) | STYLE TEXT defaults | CTRLFLAG_CLEARBG; default text "none" | `savegame.def:22` |
| (label) "Game Name" | TEXT | screen-DEF | (284,89) | (85,20) | STYLE TEXT defaults | literal text "Game Name" (uses default left/vcenter/shadow) | `savegame.def:26` |
| nameedit | EDIT | screen-DEF | (372,89) | (208,20) | STYLE EDIT NORM (`widgets.def:127-129`) BGBITMAP "ClearFrame4" FRAME 5 5 5 5 MARGINS 0 0 4 4 FONT "Med" RECT 4 2 6 2 COLOR (255,255,255) EDITCOLOR (255,255,0) FLAGS TEXT_LEFT\|TEXT_SHADOW DRAWMODE DM_USEDEFAULT | MAXLEN 30; default text "New Game" | `savegame.def:27` |
| gamelist | LISTBOX | screen-DEF | (284,117) | (298,257) | STYLE LISTBOX VLIST (`widgets.def:119-125`) BGBITMAP "VScrollRect" FRAME 8 26 26 26 MARGINS 0 0 4 4 NOCENTER RECT 5 5 23 5 ITEM 1000 16 SELCOLOR (0,112,74) FONT "Med" COLOR (255,255,255) FLAGS TEXT_LEFT\|TEXT_VCENTER\|TEXT_SHADOW\|TEXT_SINGLELINE DRAWMODE DM_USEDEFAULT SCROLLBAR RELRB 21 4 5 4 (style) UP "VScrollUp" 1,1 DOWN "VScrollDown" 1 RELB 12 THUMB "VScrollThumb" 19 30 2 | flags = LISTFLAG_VLIST \| LISTFLAG_USEFORMAT; **ITEM 260 18** (per-item w/h overrides STYLE's 1000 16); FIELD "listfield" | `savegame.def:29` |
| gamelist row (FIELD inside BEGIN..END) | LISTBOX-ITEM | gamelist-content | (2,0) | (272,18) | flags TEXT_LEFT\|TEXT_SHADOW; field-id "gamelist_name" | one column per row; FIELD body emits the slot's name string | `savegame.def:30-32` |
| savegame-button ("Save Game") | BUTTON | screen-DEF | (158,406) | (140,30) | STYLE BUTTON NORMAL (`widgets.def:88-93`) UP "ClearFrame2" DOWN "FillFrame2" FRAME 10 10 12 12 MARGINS 0 0 4 4 UPLABEL "Med" COLOR (255,255,255) FLAGS TEXT_CENTER\|TEXT_VCENTER\|TEXT_SINGLELINE\|TEXT_SHADOW DOWNLABEL "Med" COLOR (0,0,0) FLAGS TEXT_CENTER\|TEXT_VCENTER\|TEXT_SINGLELINE UPLABELRECT 0 0 0 0 DOWNLABELRECT 0 0 0 0 | label TEXT "Save Game" | `savegame.def:37` |
| exit-button ("Exit") | BUTTON | screen-DEF | (350,406) | (140,30) | STYLE BUTTON NORMAL (`widgets.def:88-93`) | label TEXT "Exit" | `savegame.def:38` |

> **Mirror note:** this screen does **not** mirror any element. Only the left
> info column has a "right-justified label / left-justified value" pair (Module:
> / modname; Character: / charname); both pairs are at fixed `screen-DEF`
> coords, not derived from each other.

---

## §5 — Draw order / composition

The screen draws as a **strict back-to-front** sequence. Each DEF widget paints
itself in DEF-declaration order via the standard `DefWidget_DispatchControl`
(`recon/discovered/FUN_00436ec0_DefWidget_DispatchControl.cpp`) → per-widget
`Draw()` call (vtable+0x50 family, see UI_METHOD_MAP §15).

**Phase A — One-time, when command 0x54 fires** (TPlayScreen DispatchCommand case
`0x54`, `FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:480-505`):

1. **Screenshot prologue** (lines 481-497):
   - `iVar3 = (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x18))(0);` — query the
     display surface format (`TDisplay::GetFormat()`, slot `+0x18`).
   - `uVar4 = FUN_004a1ec0(0x280, 0x1e0, fmtSelector);` — allocate a **full-size
     scratch (640×480, format = `((-(iVar3!=0xf) & 2) + 2)`)**, the screenshot
     target.
   - `puVar5 = FUN_00482fb0(0x6c);` — `operator new(0x6c = 108)` for a TSurface
     wrapper.
   - `FUN_004bcb00();` (line 489) — TSurface base ctor.
   - `*puVar5 = &PTR_FUN_005a3ee4;` — patch the vtable to the right TSurface
     descendant (UNCONFIRMED-D — vtable identity at `0x5a3ee4`).
   - `FUN_004a3a40(uVar4);` — attach the 640×480 bitmap to the new TSurface
     wrapper.
   - `(**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();` — `TDisplay::Lock()` /
     `BeginCapture()` (slot `+0x24`, UNCONFIRMED-M).
   - `FUN_00438df0(0,0, PTR_DAT_005d79e0,  0,0, display.w, display.h, 0x80000000, 0, 0);` —
     **blit the live framebuffer to the new scratch surface** via the
     `SDrawParam` thin wrapper. `display.w = *(uint*)(PTR_DAT_005d79e0+4)`,
     `display.h = *(uint*)(PTR_DAT_005d79e0+8)`. (`recon/discovered/FUN_00438df0_SurfaceBlitWrapper.cpp:32-54`
     — fills `local_30=dx=display.w`, `local_2c=dy=0`, `…`, calls `(**(*surface+0x5c))(&dp, src=PTR_DAT_005d79e0, …)`
     i.e. the standard `ParamBlit` with effects.)
   - `FUN_004a2960(s_ss_bmp_005d76a4, 3);` —
     **`SaveBmp("ss.bmp", quality=3)`** — writes the just-captured 640×480
     scratch to `<RunPath>\ss.bmp`. Confirmed identity of `FUN_004a2960` via
     `_data.txt:92300`: it owns a `"wb"` mode-string at `DAT_005dad44` → it's a
     `fopen(name, "wb")` writer. The `s_ss_bmp_005d76a4` literal at the call
     site = `"ss.bmp"`.
   - `(**(code **)*puVar5)(1);` — `TSurface::vt_slot_0(1)` — release the scratch
     wrapper (the `1` = `delete_this` flag).
   - `FUN_004830f0(uVar4);` — `operator delete(scratchBitmap)`.

2. **`FUN_005399f0_LoadDef_savegame()`** (line 503) — fires the savegame
   screen Initialize (see Phase B below).

3. **`FUN_0048f040(&DAT_0066fb08, mask);` (line 504)** — modal pump on the
   global savegame screen instance. The `mask` `= (DAT_0066829c ? 0x7 : 0xf)` —
   in-game vs. logo-screen input mask. Pumps until the screen Closes (the
   dispatcher returns).

4. **`FUN_00539ab0()` (line 505)** — savegame `Close` — frees the picture
   scratch (`this->mbr_0x19c`) and chains to `cls_0x5b98b8::virt_meth_0x434f30`
   (`TButtonPane::Close`).

5. `_DAT_0065cb40 = 1;` (line 506) — sets a "redraw next frame" flag on the
   parent screen.

**Phase B — Inside `Init` (`cls_0x5b963c::virt_meth_0x5399f0`,
`FUN_005399f0_LoadDef_savegame.cpp`):**

1. `FUN_0048d260(DAT_0066829c);` — TPlayer / TGame "pause" hook (UNCONFIRMED-M
   — likely `MainPlayer::PauseScripts()`).
2. Allocate picture-scratch (216×160) as documented in §3.
3. `FUN_00444e20(0);` — TPane / TButtonPane base reset (UNCONFIRMED-M; called
   with arg `0` here, and with `0xffffffff` from SpellbookSidebarPane —
   probably `TPane::SetSelection(idx_or_clear)`).
4. `FUN_004a22f0(picture_scratch_surface, drawmode=DM_USEDEFAULT, color=0xffff);` —
   initialize/fill the picture surface. The `0xffff` = `zpos` default
   (UI_METHOD_MAP §15a `+0x64` Box semantics).
5. `DAT_0066fb04 = 0xffffffff;` — set the global **selected-slot-index** to
   "no slot selected".
6. `FUN_00435150_DefScreen_Open(...)` — call into the DEF engine with the
   args documented in §3.

**Phase C — Per-frame paint, inside `DefWidget_DispatchControl` (the DEF
engine's draw walk):**

For each child in declaration order — see `savegame.def:7-42`:

1. **Implicit background:** the `background` bitmap is pre-bound by
   `FUN_00435040_DefScreen_LoadAndShow:31` (`vtable+0xa0("background", -1)` =
   BindBackground) and drawn first by the engine's clear pass before any
   widget paints.
2. **Decorative parent FRAME** (35,70,562,320) — `ClearFrame1` 9-slice.
3. **`pictureframe` FRAME** (50,87,222,166) — `ClearFrame2` 9-slice.
4. **`picture` BITMAP** (54,91,216,160) — blits the picture-scratch surface.
   The bind happens runtime: the FIELD "picture" attribute (`savegame.def:11`)
   tells the widget engine to call `this->FieldGet("picture")` → returns the
   live scratch surface. UNCONFIRMED-M which method on the savegame class
   exposes "picture" — most likely the standard `cls_0x5b93c4::virt_meth_0x436460`
   "GetField" base or the savegame override at vtable+0xa0
   (`cls_0x5b963c::virt_meth_0x539bc0`); `_data.txt:114650` shows
   `s_picture_005e4350` referenced inside `virt_meth_0x539bc0` at offset
   `0x539bc6` — i.e. **`virt_meth_0x539bc0` IS the GetField/Activate that
   matches FIELD "picture"** and returns the scratch surface. Decisive.
5. **Bottom info FRAME** (50,268,221,106) — `ClearFrame2` 9-slice.
6. **`gamename` TEXT** (55,277,210,40) — 9-slice `ClearFrame3` BG + 3-pass
   black drop-shadow Med-font white text, **center-aligned**, with elipses
   truncation. Default text "No Saved Game"; bound to whichever slot is
   highlighted in `gamelist`.
7. **"Module:" TEXT** (55,315,75,20) — right-justified, default style (no BG,
   white, shadow).
8. **`modname` TEXT** (130,315,130,20) — left-justified white, shadow, BG
   cleared.
9. **"Character:" TEXT** (55,335,75,20) — right-justified.
10. **`charname` TEXT** (130,335,130,20) — left-justified, BG cleared.
11. **"Game Name" TEXT** (284,89,85,20) — left/vcenter/shadow default.
12. **`nameedit` EDIT** (372,89,208,20) — `ClearFrame4` 9-slice BG + Med-font
    yellow caret color (`EDITCOLOR 255 255 0`), text "New Game".
13. **`gamelist` LISTBOX** (284,117,298,257) — `VScrollRect` 9-slice chrome +
    `VScrollBar/Up/Down/Thumb` scrollbar + per-row repeated FIELD draw
    (`FIELD 2 0 272 18 FLAGS TEXT_LEFT|TEXT_SHADOW "gamelist_name"`). Row
    height **18**, content rect inset by **RECT 5 5 23 5** from chrome (top
    inset 5, left 5, right 23 — leaves room for the scrollbar, bottom 5).
    Selection color `SELCOLOR 0 112 74` (dark teal) — drawn behind the
    selected row.
14. **`savegame` BUTTON** (158,406,140,30) — STYLE BUTTON NORMAL,
    `ClearFrame2`/`FillFrame2` 9-slice for up/down states + label "Save Game"
    in white (up) / black (down), center+vcenter+singleline+shadow.
15. **`exit` BUTTON** (350,406,140,30) — same style, label "Exit".

The shared DEF engine handles each widget's hover/focus/down state — not a
per-spec concern.

---

## §6 — Algorithms (pseudocode per helper)

Three helpers are SaveGame-specific:

### 6.1 `cls_0x5b963c::Init` (= `FUN_005399f0_LoadDef_savegame`)

```python
def Init(self):
    # 1. Suspend gameplay / pause sim
    FUN_0048d260(DAT_0066829c)            # PauseScripts(main_player_or_null)

    # 2. Allocate the 216x160 picture scratch
    fmt_id = display.GetFormat()          # vtable+0x18
    bm_fmt = ((-(fmt_id != 0x0f) & 2) + 2)  # 2 (BM_15BIT) if fmt==0xf else 4 (UNCONFIRMED #6)
    self.mbr_0x19c = NewBitmap(w=216, h=160, fmt=bm_fmt)

    # 3. TPane base reset
    FUN_00444e20(0)                       # TPane.Reset(0) — UNCONFIRMED-M

    # 4. Clear the picture scratch to opaque
    FUN_004a22f0(self.mbr_0x19c,
                 drawmode=0x80000000,    # DM_USEDEFAULT (opaque-clear)
                 color=0xFFFF)            # zpos default

    # 5. No slot is initially selected
    DAT_0066fb04 = -1                     # selected_slot_index

    # 6. Pre-load DEF widgets + chrome (engine entry)
    DefScreen_Open(
        panel_name = "savegame", screen_name = "default",
        flags = 0x11, x = 0, y = 0,
        w = 640, h = 480,
        bg_w = 450, bg_h = 160,          # UNCONFIRMED-D — see §3
        bg_sprite_name = "widgets",
        outer_name = "savegame")
    return 1
```

### 6.2 `cls_0x5b963c::Close` (= `FUN_00539ab0`)

```python
def Close(self):
    if self.mbr_0x19c != 0:
        FreeBitmap(self.mbr_0x19c)
    self.mbr_0x19c = 0
    cls_0x5b98b8::virt_meth_0x434f30(self)   # TButtonPane.Close (base chain)
```

### 6.3 Screenshot prologue (caller-side, before Init runs)

```python
def OnSavePressed():           # TPlayScreen.DispatchCommand case 0x54
    fmt = display.GetFormat()
    full_bm = NewBitmap(640, 480, fmt = ((-(fmt != 0x0f) & 2) + 2))
    surf = TSurface_new(0x6c)
    surf.vtable = PTR_FUN_005a3ee4         # UNCONFIRMED-D
    surf.attach(full_bm)
    display.Lock()                          # vtable+0x24
    # Blit live framebuffer → full_bm via SDrawParam-fill wrapper
    BlitSDP(dx=0, dy=0,
            src=display, sx=0, sy=0,
            dwidth=display.w, dheight=display.h,
            drawmode=0x80000000,            # DM_USEDEFAULT (opaque)
            color=0, intensity=0)
    SaveBmp("ss.bmp", quality=3)            # FUN_004a2960 - writes to <RunPath>\ss.bmp
    surf.release_or_delete(1)
    FreeBitmap(full_bm)
    FUN_005399f0_LoadDef_savegame()         # cls_0x5b963c::Init via global instance
    FUN_0048f040(&savegame_screen_global,   # modal pump
                 mask = 0x07 if DAT_0066829c else 0x0f)
    cls_0x5b963c::Close(savegame_screen_global)
    _DAT_0065cb40 = 1                        # mark playscreen dirty
```

### 6.4 SaveBmp `FUN_004a2960` (signature only, body not extracted)

Observed signature: `int SaveBmp(const char* path, int quality)`. Confirmed
via:
- The `"wb"` literal at `DAT_005dad44` referenced from inside the body
  (`recon/ghidra/_data.txt:92300` — XREF `FUN_004a2960:004a29b2 (*)`) — it's a
  `fopen(name, "wb")` writer, the Windows BMP encoder.
- Call sites use `(name, 3)` everywhere it's hit — `3` is presumably the BMP
  bitdepth selector (24-bit RGB) but UNCONFIRMED #3.

### 6.5 SaveGame disk action `FUN_0048d720`

The on-disk save itself is `SaveGame(name, flags)` at `0x0048d720` (2066-byte
body, `recon/discovered/renames/agent_player_init.txt:109-117`). The savegame
DEF screen's command dispatcher (slot 148 = `0x539c00`) calls into this with
the EDIT-widget's text as `name`, and at the same time **copies the just-saved
`<RunPath>\ss.bmp` into `<SavePath>\Save\Single\<name>\ss.bmp`** as the slot
thumbnail. See `recon/discovered/save_system_notes.md` §"Save game" steps
3-5-7 for the directory-clear-then-write contract: SaveGame `DeleteFileA("ss.bmp")`
inside the slot, then writes a fresh copy of `<RunPath>\ss.bmp` into the slot.
The DEF screen's dispatcher merely orchestrates: pass EDIT text → SaveGame →
on success, screen self-closes by returning a non-zero modal result from
`FUN_0048f040`. The dispatcher body is **not extracted** — see UNCONFIRMED #1
for the precise control-flow and per-button branching.

---

## §7 — Effects & shadows

- **Text 3-pass drop shadow:** every TEXT/EDIT/LISTBOX/BUTTON label gets a
  1px-SE-shadow from `FUN_004be2b0_CompositeBuffer`'s `param_8 & 0x400` gate
  (UI_METHOD_MAP §5). The DEF flag bit `TEXT_SHADOW = 0x0400` (per
  `widgets.def:24`) is OR'd into the font index by the widget renderer — every
  text element in `savegame.def` either inherits TEXT_SHADOW from STYLE or
  explicitly sets it.
- **Bitmap drop shadows:** none. No widget in `savegame.def` uses a shadowed
  blit. The Background, 9-slice frames, and atlases all blit opaque (no
  `DM_TRANSPARENT` keying needed since they cover their footprint).
- **Chroma key:** the LISTBOX selection highlight color
  `SELCOLOR 0 112 74` (dark teal) is drawn as a filled rect *behind* the
  selected row's text — NOT a chroma-key (per
  `widgets.def:121`).
- **Background opaqueness:** `Background` in `savegamenotex.dat` is
  `flags=0x2` (`BM_15BIT`, no alpha, kc=0); it covers the whole 640×480 modal,
  so no transparency math runs against it.

---

## §8 — Text rendering

The font `"Med"` is FONT.DEF entry `Med`. Every TEXT-bearing widget on this
screen uses it (default per `widgets.def:86,91,98,103,128,133,142`).

| string | cell (screen-DEF, x,y,w,h) | font | px | color (RGB) | h-align | v-align | shadow | format / source | cite |
|---|---|---|---|---|---|---|---|---|---|
| `"No Saved Game"` (or live slot's title) | (55, 277, 210, 40) | "Med" | (UNCONFIRMED #7) | (255,255,255) | **center** | top (default — see NOMENCLATURE §2 "v-align") | yes (TEXT_SHADOW) | bound to slot's title getter; default text "No Saved Game" when empty | `savegame.def:15-16` |
| `"Module:"` | (55, 315, 75, 20) | "Med" | UNCONFIRMED | (255,255,255) | **right** (TEXTFLAG_RIGHT) | top | yes (inherited STYLE TEXT_SHADOW) | literal | `savegame.def:18` |
| (slot's module name) | (130, 315, 130, 20) | "Med" | UNCONFIRMED | (255,255,255) | left (default) | top | yes | bound to slot getter; default "none" | `savegame.def:19` |
| `"Character:"` | (55, 335, 75, 20) | "Med" | UNCONFIRMED | (255,255,255) | **right** | top | yes | literal | `savegame.def:21` |
| (slot's character name) | (130, 335, 130, 20) | "Med" | UNCONFIRMED | (255,255,255) | left | top | yes | bound to slot getter; default "none" | `savegame.def:22` |
| `"Game Name"` | (284, 89, 85, 20) | "Med" | UNCONFIRMED | (255,255,255) | left | vcenter (STYLE TEXT default) | yes | literal | `savegame.def:26` |
| (edit content — "New Game" default) | (372, 89, 208, 20) cell; inner RECT=(4,2,6,2) inset | "Med" | UNCONFIRMED | (255,255,255) for text; EDIT caret color (255,255,0) | left | top | yes | EDIT widget content; MAXLEN=30 | `savegame.def:27`; STYLE EDIT NORM `widgets.def:127-129` |
| each `gamelist` row | row-cell in LISTBOX content space (2, N*18, 272, 18) | "Med" | UNCONFIRMED | (255,255,255); SELECTED row text on top of (0,112,74) bg | left (TEXT_LEFT), vcenter (TEXT_VCENTER from STYLE), singleline | (vcenter applies via DT_VCENTER) | yes | bound via FIELD `gamelist_name` to slot-name getter | `savegame.def:30-32`; STYLE LISTBOX `widgets.def:120-122` |
| `"Save Game"` | (158, 406, 140, 30) | "Med" | UNCONFIRMED | (255,255,255) up / (0,0,0) down | center | vcenter | yes (up state) / no (down state) | literal | `savegame.def:37`; STYLE BUTTON `widgets.def:88-93` |
| `"Exit"` | (350, 406, 140, 30) | "Med" | UNCONFIRMED | (255,255,255) / (0,0,0) | center | vcenter | yes (up) / no (down) | literal | `savegame.def:38` |

> **px size:** the Med font's pixel size is NOT recorded in `savegame.def`; it
> comes from FONT.DEF (likely `data/Imagery/...`). UNCONFIRMED #7 — measure
> from FONT.DEF / the existing port's font.cpp.

> **Pink-halo retail bug** ([[project-retail-pink-halo-bug]]): the magenta-key
> + antialiased-shadow combination on the 9-slice chrome behind TEXT widgets
> can produce a pink fringe. The port reproduces intent (white text + 3-pass
> black shadow), NOT the bug.

> **v-align reminder:** STYLE TEXT uses `TEXT_VCENTER` (vcenter), STYLE LISTBOX
> uses `TEXT_VCENTER|TEXT_SINGLELINE` (vcenter, never wraps). The `gamename`
> TEXT widget is the ONE exception — it overrides flags to
> `TEXT_CENTER|TEXT_SHADOW|TEXT_ELIPSES`, **NO TEXT_VCENTER** → its text hangs
> from the cell TOP (per NOMENCLATURE §2 default), so a single line will sit at
> the top of the 40px-tall cell, not centered. (Decisive cite:
> `savegame.def:15-16` — only `TEXT_CENTER` + `TEXT_SHADOW` + `TEXT_ELIPSES` are
> listed; no `TEXT_VCENTER`.)

---

## §9 — Animation & dynamic behavior

- **No tweens or ramps on this screen.** Hover/focus/down state changes on the
  EDIT / LISTBOX / BUTTON widgets are handled by the shared DEF engine as
  instant state flips (no fade ramp).
- **The only "animated" surface is the picture preview**: when the user
  highlights a different row in `gamelist`, `picture-scratch` is rebuilt by
  loading that slot's `<SavePath>\Save\Single\<name>\ss.bmp`, downscaled to
  216×160, and the BITMAP widget redraws on next paint. UNCONFIRMED #2 —
  precise rebuild trigger: most likely `cls_0x5b963c::virt_meth_0x539bc0` (slot
  +0xa0) — the per-control activate slot — fires on "selected-row changed",
  loads slot's `ss.bmp`, and stamps it into `mbr_0x19c`. The decisive evidence
  is the `s_picture_005e4350` xref @ `0x539bc6` (`_data.txt:114650`).
- **Dirty model:** the DEF engine uses a per-control `CTRLFLAG_DIRTY = 0x20`
  bit (`widgets.def:35`) to mark widgets needing redraw. Slot-selection change
  sets DIRTY on `picture`, `gamename`, `modname`, `charname` simultaneously,
  per the cluster of widget-name strings referenced inside the dispatcher
  body (`_data.txt:114668-114682`).

---

## §10 — Input & dispatch

The screen is purely interactive — modal pump = `FUN_0048f040`.

| control | type | hit rect (screen-DEF) | command id / behavior | cite |
|---|---|---|---|---|
| `nameedit` EDIT | text input | (372, 89, 208, 20) | typed chars edit the buffer (MAXLEN 30); Enter does NOT submit (EDITFLAG_ENTERMSG NOT set on this widget) | `savegame.def:27` |
| `gamelist` LISTBOX | row click → select | content rows in (284+5, 117+5, 298-23-5, 257-5-5) sub-rect; per-row 18px | sets `DAT_0066fb04 = row_index`; fires "selected" event → loads that slot's ss.bmp into `picture-scratch`, copies its `gamename`/`modname`/`charname` into the TEXT widgets, **copies the slot name into `nameedit`** (so a single click + Save overwrites that slot — UNCONFIRMED #2, inferred from sibling LoadGame `virt_meth_0x539590` flow). Mouse wheel + scrollbar control scroll | `savegame.def:29-32` + `_data.txt:114641` (the FIELD-row `name` xref at `_data.txt:114641` from `virt_meth_0x539ae0`) |
| `savegame` BUTTON | mouse / Enter | (158, 406, 140, 30) | runs `cls_0x5b963c::virt_meth_0x539c00` (the override of slot 148). The dispatcher's xrefs at `_data.txt:114663,114715,114720` show it touches `nameedit`, `ss.bmp`, and the `"Locke"` literal (the default-slot probe). Pseudo-flow: `name = nameedit.text; SaveGame(name, flags=0); copy "<RunPath>\ss.bmp" → "<SavePath>\Save\Single\<name>\ss.bmp"; modal_result = ENUM_SAVE`. UNCONFIRMED #1 — exact return value, "name exists ⇒ overwrite prompt" branch, and Multi-vs-Single dir selection logic. | `savegame.def:37` + `_data.txt:114663,114682,114715,114720` |
| `exit` BUTTON | mouse / Esc | (350, 406, 140, 30) | sets modal_result = ENUM_EXIT (sibling LoadGame uses the equivalent `DAT_005e42c8 = "exit"` literal at `virt_meth_0x539590:00539651`). | `savegame.def:38` + `_data.txt:114687` (`DAT_005e43a4 = "exit"` xref from `virt_meth_0x539c00`) |
| Esc key | modal cancel | global | DEF engine input dispatcher (`FUN_004361f0_DefWidget_DispatchInput`) maps Esc → modal cancel = same as Exit. | `recon/discovered/FUN_004361f0_DefWidget_DispatchInput.cpp` |
| Enter key | activate focused widget | global | Engine vkey 0x0d → focused control's vtable+0x2c (activate). For the gamelist, this is "open/save current row"; for `nameedit`, just commit edit. | `recon/discovered/renames/agent_ui_wave2_defengine.txt:203-208` |
| `R` key | (dev) hot-reload of `savegame.def` | global | Engine vkey 0x52 → re-runs `DefWidget_LoadFile(this, "savegame")`. **Dev-only**; gated by build flag in port. | `agent_ui_wave2_defengine.txt:203-208` |

**Modal state globals:**
- `DAT_0066fb04` (4 bytes): currently-selected slot index in `gamelist`. Set
  to `-1` on `Init` (`FUN_005399f0:22`). Read+written by `virt_meth_0x539590`
  (LoadGame side; symmetrical for save), written by `virt_meth_0x539c00`
  (savegame dispatcher) per `_data.txt:137179`.
- `DAT_0066fb08` (420 bytes): the savegame screen instance itself (the global
  `cls_0x5b963c` lives statically embedded here — alignment `align(420)` per
  `_data.txt:137192`).

---

## §11 — Retail bugs NOT to reproduce

1. **Pink halo around shadowed text** ([[project-retail-pink-halo-bug]]) —
   the 9-slice chrome behind TEXT widgets (Module:/Character:/gamename/etc.)
   on `ClearFrame3` can chroma-key-bleed magenta into the antialiased shadow
   edge. The port renders text + black shadow on real alpha (per
   UI_METHOD_MAP §16, `Renderer->CompositeSwapchainTinted` family) — never
   reproduce.
2. **The `ss.bmp` overlap window** — if the user hits Save quickly between
   the prologue's `SaveBmp` and the dispatcher's `SaveGame`, retail can leave
   stale `<RunPath>\ss.bmp` content if the save-action throws before reaching
   the file-copy step. The port should either (a) capture into a TSurface
   in-RAM and pass it to SaveGame (no intermediate file), or (b) treat
   `<RunPath>\ss.bmp` as scratch and always re-capture in the dispatcher too.
   This is a robustness concern, not a visible artifact, but worth flagging.
3. **`bg_w/bg_h = (450, 160)` mismatch with the 640×480 Background bitmap**
   (UNCONFIRMED #5) — if this turns out to be intended (a chrome inner rect
   used for clipping or cursor centering), the spec is fine; if it's a typo
   in the activator (some older version of the screen used a smaller
   chrome), reproducing it verbatim is correct since DEF engine treats those
   args as logical sizes and the actual Background bitmap is sized by
   `DefScreen_Open` to fit `(w,h) = (640,480)`.

---

## §12 — Reconstruction pseudocode

The reconstruction agent should NOT special-case this screen — it should
implement it as **one of many DEF screens**, sharing the engine work with
loadgame, options, popup, etc. Concretely:

```cpp
// (reconstruction agent skeleton — for orientation only)

class TSaveGameScreen : public TDefScreen {
  TBitmap*  picture_scratch_ = nullptr;       // 216x160 BM_15BIT
public:
  int  Initialize() override;                  // = virt_meth_0x5399f0
  void Close()     override;                   // = virt_meth_0x539ab0
  // engine-driven:
  void DrawBackground() override;              // engine: sequences §5 Phase C
  // SaveGame-specific override slots:
  int  CommandDispatch(int cmd) override;      // slot 148 = 0x539c00 — SAVE / EXIT branches
  int  Activate(TWidget* w)     override;      // slot 160 = 0x539bc0 — picture rebind on row select
  int  ListField(int row, int col, char* out, int outsz) override;  // slot 164 = 0x539ae0 — emit slot.name
};

int TSaveGameScreen::Initialize() {
  PauseScripts(MainPlayer());                                   // FUN_0048d260
  auto fmt = Display()->GetFormat();
  picture_scratch_ = NewBitmap(216, 160,
      fmt == 0xF ? BM_15BIT : 4 /* UNCONFIRMED-#6 */);
  TButtonPane::Reset(0);                                        // FUN_00444e20
  ClearBitmap(picture_scratch_, DM_USEDEFAULT, 0xFFFF);          // FUN_004a22f0
  selected_slot_index_global_ = -1;                              // DAT_0066fb04
  return TDefScreen::Open(
      /* panel_name */    "savegame",
      /* screen_name */   "default",
      /* flags */         0x11,
      /* x,y */           0, 0,
      /* w,h */           640, 480,
      /* bg_w,bg_h */     450, 160,
      /* bg_sprite */     "widgets",
      /* outer_name */    "savegame");
}

void TSaveGameScreen::Close() {
  if (picture_scratch_) { FreeBitmap(picture_scratch_); picture_scratch_ = nullptr; }
  TButtonPane::Close();
}

// the play-screen side (entry from DispatchCommand case 0x54):
void TPlayScreen::OnSaveCommand() {
  // Capture the live framebuffer to <RunPath>\ss.bmp:
  auto thumb = Renderer::CaptureFramebufferToBitmap(640, 480);
  SaveBmp("ss.bmp", thumb, /*quality=*/3);                       // FUN_004a2960
  // Run the modal:
  auto& screen = savegame_screen_global_;                        // DAT_0066fb08
  screen.Initialize();
  ModalPump(&screen, in_game_ ? 0x07 : 0x0F);                    // FUN_0048f040
  screen.Close();
  redraw_play_screen_ = true;
}

// virtually each FIELD-bound widget (picture/gamename/modname/charname/listrow)
// pulls its value via:
TWidget::FieldGet("picture")   → screen.picture_scratch_
TWidget::FieldGet("gamename")  → SlotMeta(selected).gamename
TWidget::FieldGet("modname")   → SlotMeta(selected).modname
TWidget::FieldGet("charname")  → SlotMeta(selected).charname
TWidget::FieldGet("listfield") → SlotList()              // the gamelist source
ListField(row, col, out, outsz) emits SlotList()[row].name for col == "gamelist_name"
```

> The reconstruction agent must **NOT** open-code text drawing, 9-slice
> chrome, scrollbar handling, button hit-test, or EDIT input — those are the
> DEF-engine's job (`docs/ui/port_status/DefWidgetEngine.md`). This screen is
> 100% data after the engine lands.

---

## §13 — Port mapping notes

Each retail primitive used by this screen maps to a canonical port primitive
(UI_METHOD_MAP §12):

| retail call | what it does here | canonical port method | home |
|---|---|---|---|
| `FUN_004a1ec0(w,h,fmt[,0])` | allocate offscreen bitmap of given format | `NewBitmap(w,h,fmt)` (TBitmap factory) | bitmap layer |
| `FUN_004a22f0(bm, drawmode, color)` | initialize/clear bitmap to opaque | `TSurface::Clear(drawmode, color)` or `Renderer->FillBitmap(bm, color)` | renderer / surface |
| `FUN_00438df0(0,0,src,0,0,w,h,DM_USEDEFAULT,0,0)` | SDrawParam-fill blit wrapper (no shadow, no effects) | `Renderer->DrawSurfaceToTarget(src_surface, 0,0)` opaque | renderer |
| `FUN_004a2960(name, q)` | write a BMP from current TSurface | **NEW PRIMITIVE NEEDED** — `Renderer->SaveBmp(const char* path, TBitmap*, int bitdepth)` (currently absent from port) | renderer (new) |
| `FUN_004830f0(ptr)` | `operator delete(ptr)` | C++ `delete` | — |
| `FUN_0046d710(name)` (xref to "background") | resource lookup by name | `AssetCache::GetBitmapByName(name)` | asset cache |
| `FUN_00435150_DefScreen_Open(...)` | DEF engine entry — load+parse `widgets.def` + `<name>.def`, build widget tree, run modal | `DefScreen::Open(panel, name, flags, x,y,w,h, bgw,bgh, bgsprite, outer)` | def-screen engine (NEW) |
| `FUN_0048f040(&screen, mask)` | modal pump | `ModalRunner::Pump(&screen, mask)` | engine pump (NEW) |
| `FUN_0048d260(player_or_null)` | pause hook | `MainPlayer::Pause()` / `ScriptManager::Pause()` (UNCONFIRMED-M) | gameplay layer |
| `FUN_00444e20(arg)` | TPane base reset/selection (UNCONFIRMED-M) | `TPane::SetSelection(arg)` | pane base |
| `FUN_0048d720(name, flags)` | disk SaveGame | `SaveGame(name, flags)` — currently in `src/savegame.cpp::WriteGame` but missing the slot-dir-create + MapPane::SaveCurMap + ss.bmp copy steps (see `save_system_notes.md` §"Known bugs" #4) | savegame |

**Missing primitives to flag:**
- `Renderer->SaveBmp(path, bitmap, depth)` — for both the prologue ss.bmp
  capture and the slot's ss.bmp copy.
- The whole **DEF widget engine** — 8 widget classes, 9-slice chrome, modal
  pump (`FUN_0048f040`). Tracked in `docs/ui/port_status/DefWidgetEngine.md`.
  Until it lands, this screen cannot reconstruct.

---

## §14 — UNCONFIRMED / open questions

1. **`cls_0x5b963c::virt_meth_0x539c00` (the SaveGame button dispatcher) body.**
   The xref evidence in `_data.txt:114654-114723` shows the dispatcher
   references widget names `gamelist`, `picture`, `nameedit`, `gamename`,
   `modname`, `charname`, `savegame` (twice), `"exit"` (twice), `gamelist`
   (again), `ss.bmp`, `"Locke"` — but the body is not in
   `recon/discovered/`. Impact if wrong: incorrect Save button behavior
   (might silently no-op, double-save, save to wrong dir, or skip the ss.bmp
   copy). **Resolve:** `DecompileAddr.java 0x00539c00` and write the body to
   `recon/discovered/FUN_00539c00_TSaveGameScreen_CommandDispatch.cpp`.
   Cross-check the sibling: extract `0x00539590` simultaneously (the
   LoadGame dispatcher — same xref shape, body almost certainly mirrors).

2. **`cls_0x5b963c::virt_meth_0x539bc0` (the per-control activation /
   picture-bind body).** Strong inference: it's the FieldGet/Activate that
   binds widget name `"picture"` to `this->mbr_0x19c` (`_data.txt:114650`
   `s_picture_005e4350` @ `0x539bc6`). Also presumably handles the
   row-selection callback that reloads the slot's `<SavePath>\Save\Single\<row.name>\ss.bmp`
   into the picture-scratch. Impact if wrong: picture preview won't update,
   or `picture` widget shows the wrong/stale image. **Resolve:**
   `DecompileAddr.java 0x00539bc0`.

3. **`FUN_004a2960` quality arg.** Always called as `(name, 3)`. The `3` is
   presumably "24-bit BMP" (vs 1=monochrome, 2=8-bit, 3=24-bit, 4=32-bit) —
   but only the body would confirm. Impact if wrong: trivial — the disk
   format differs but the dialog still works. **Resolve:** `DecompileAddr.java 0x004a2960`.

4. **DefScreen flag `0x11`.** `0x11 = CTRLFLAG_SELECTABLE (0x10) |
   CTRLFLAG_ERROR (0x01)` per `widgets.def:30-34` — but CTRLFLAG_ERROR is
   nonsensical to set at open. More likely `0x11` is the DEF-engine's modal
   *mode* mask (a different name-space from CTRLFLAG_*). Impact if wrong:
   the screen opens in wrong modal mode (focus rules different). **Resolve:**
   trace `param_4 & 1` in `FUN_00435150` (line 21 of recon discovered) — it
   selects between two `DAT_*` writes; identifying those distinguishes "0x11 =
   modal-focus + a-vs-b flag".

5. **`bg_w, bg_h = (450, 160)` mismatch with `Background` bitmap (640×480).**
   Could be (a) a chrome inner-rect for clipping, (b) the modal centering
   reference, (c) a typo in the activator carrying over from an older
   smaller-banner version, or (d) the size of the **"SAVE GAME" banner plate**
   on the Background (visually the plate is roughly 184×30 centered at
   ~y=0..40, so 450×160 isn't *that* either). Impact if wrong: chrome could
   be slightly off, or one widget could end up clipped. **Resolve:** trace
   what `DefScreen_Open` does with those args inside `FUN_00435150` —
   `param_4..param_9` ride the stack into `LoadAndShow` (line 58); examine
   `FUN_00435040:24-29` which stores `in_stack_00000010/14/18/1c/20/24` into
   `param_1[5/6/0x2c/0x2d]` and locals.

6. **Picture-scratch format selector** `((-(fmt!=0xF) & 2) + 2)`. Resolves
   to 2 when display-format is `0xF`, else 4. The `BM_15BIT = 0x2` mapping is
   solid (`src/revdefs.h:308`); the value `4` is *not* a standard BM_* in
   `revdefs.h` — most likely `BM_15BIT | BM_8BIT(0x4)` or a separate "screen
   format" enum. Impact: the live-thumbnail conversion could land in the
   wrong pixel layout. **Resolve:** dump `TDisplay::GetFormat`'s return values
   across `revdefs.h` BM_* (and the Bitmap-class internal "format" enum at
   `src/bitmap.h`) and pin the value `4`.

7. **Med font pixel size & metrics.** Not in `savegame.def` — comes from
   FONT.DEF (likely `data/Imagery/Pix/Fonts/` or equivalent). Impact: text
   sizing off. **Resolve:** identify "Med" entry in FONT.DEF or in the
   existing port's `font.cpp` Med-font fallback.

8. **Behavior of `FUN_00444e20(arg)`.** Called as `(0)` here and as
   `(0xffffffff)` from SpellbookSidebarPane. Looks like a TPane-base
   selection-or-reset helper. Impact: minor; if wrong, focus may start on a
   different control. **Resolve:** `DecompileAddr.java 0x00444e20`.

9. **The `TSurface` vtable identity at `PTR_FUN_005a3ee4`** (the screenshot
   wrapper's vtable). Confirmed by usage (the wrapper attaches to a TBitmap
   and supports the standard `+0x5c` ParamBlit + `vtable[0](1)` destructor).
   Impact: minor — the port can use any TSurface descendant that takes a
   TBitmap. **Resolve:** `DumpVtable.java 0x005a3ee4`.

10. **Multi vs Single save-dir branch.** `save_system_notes.md` notes the
    SaveGame disk function picks `\Save\Single\` vs `\Save\Multi\` vs
    `\Save\Chars\` based on the current "save context". Whether that context
    is global or carried in `cls_0x5b963c` (the savegame screen) — UNCONFIRMED.
    Impact: in multiplayer, the wrong save folder gets used. **Resolve:**
    once dispatcher body (#1) is extracted, the branch will be visible.

11. **The "Locke" string literal** referenced at `virt_meth_0x539c00:00539e39`
    (`_data.txt:114720`). Almost certainly the default character-name when
    the save dialog opens with no slot selected (mirrors the createchar
    default at `s_Locke_005d330c` per `agent_player_init.txt:83`). UNCONFIRMED —
    might also be a fallback player-id for the active player when the player
    object isn't yet set. Impact: a missing default name string in one
    edge-case. **Resolve:** dispatcher body extract (#1).

---

## Quick reference

- **Class:** `cls_0x5b963c` (TSaveGameScreen) — vtable `0x5b963c`, size 420 B,
  embedded global at `DAT_0066fb08`.
- **Open path:** TPlayScreen DispatchCommand case `0x54` →
  screenshot-capture-to-`<RunPath>\ss.bmp` → `Init` (`FUN_005399f0`) → modal
  pump `FUN_0048f040(&screen, mask)` → `Close` (`FUN_00539ab0`).
- **Screen size:** full 640×480 modal (centered over the play-screen).
- **Background:** `Background` from `savegamenotex.dat[0]` (640×480, BM_15BIT).
- **DEF layout:** `data/resources_unzipped/savegame.def` — 14 widgets total
  (1 decorative FRAME + 2 chrome FRAMEs + 1 BITMAP + 6 TEXTs + 1 EDIT + 1
  LISTBOX + 2 BUTTONs).
- **Picture scratch:** 216×160 BM_15BIT-or-4 surface @ `this+0x19c`, blitted
  into the `BITMAP "picture"` widget via `FIELD "picture"`.
- **Modal globals:** `DAT_0066fb04` = selected-slot-index (init `-1`),
  `DAT_0066fb08` = screen instance.
