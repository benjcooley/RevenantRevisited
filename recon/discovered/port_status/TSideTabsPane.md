# TSideTabsPane port status

Retail vtable: **`cls_0x5a5750`** (46 slots — confirmed Wave-4A). Global instance: `0x65be50`. Constructor: `0x487c60` (sets base vtable 0x5a4494, calls FUN_0041c7f0, then writes derived vtable 0x5a5750). Recon body decompilations live in `recon/discovered/cls_0x5a5750_TSideTabsPane_*.cpp`. Full 50-slot vtable dump at `recon/discovered/cls_0x5a5750_vtable.txt`. Our source: none yet — class is forward-declared at [src/revtypes.h:1165](../../../../src/revtypes.h) (`_CLASSDEF(TSideTabsPane)`) but no `.h`/`.cpp` exists. **Needs full reconstruction.**

The vertical mode-switcher button strip on the right edge of the sidebar. Per Wave-4A (`B_r11_sidebar_tab_cascade`), the 6 round buttons are **two independent 3-way region selectors**: 3 buttons for the upper sidebar content (mode in `DAT_0065d1b8`) and 3 for the lower sidebar content (mode in `DAT_0065d1bc`). `sample_screen_1.jpg` (Stats upper + Map lower visible simultaneously) confirms this two-region model.

The class itself does NOT contain the visibility-toggle logic. Per-button click routes through the **TPlayScreen central command dispatcher** (`FUN_0047cf40_TPlayScreen_DispatchCommand`) which is the sole writer of the two mode globals and which calls the content-pane vtable Close on the previous active pane.

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started. See [README.md](README.md).

## Vtable (46 slots — leaf overrides extracted Wave-4A)

| Slot | Method | Retail addr | Status | Notes |
|---|---|---|---|---|
| 0 | `Initialize` | 0x53cc30 | ⚫ extracted | Body in `cls_0x5a5750_TSideTabsPane_Initialize_53cc30.cpp` (Wave-1A). |
| 1 | `Close` (leaf) | 0x53d360 | ⚫ extracted | Frees 3 mosaic surfaces at +0x188/0x18c/0x190 then calls base TButtonPane Close (0x434f30). Body in `cls_0x5a5750_TSideTabsPane_Close_53d360.cpp`. |
| 2 | (intermediate shared) | 0x435010 | ⚫ not-started | TButtonPane intermediate slot 2. |
| 7 | `DrawBackground` (leaf) | 0x53d420 | ⚫ extracted | Inactive-pass per-button bitmap blit with per-button fade alpha. Body in `cls_0x5a5750_TSideTabsPane_DrawBackground_53d420.cpp`. |
| 19 | `Update` (leaf) | 0x53d3a0 | ⚫ extracted | Per-button hover-fade animator (slot[6] = target 0/8, slot[0] = current, ±1 per tick). Body in `cls_0x5a5750_TSideTabsPane_Update_53d3a0.cpp`. |
| 20 | (leaf override) | 0x53d400 | ⚫ not-extracted | TBD this wave. |
| 23 | `Draw` (leaf, main pass) | 0x53d540 | ⚫ extracted | Composites bright pass when slot[0] > 0x80 (= active button gets bright variant). Body in `cls_0x5a5750_TSideTabsPane_Draw_53d540.cpp`. |
| 25 | `MouseClick` (leaf) | 0x53d6e0 | ⚫ extracted | Tests event type 4/5 + bounds; marks 3 HUD-overlay dirty flags then falls through to FUN_00436530 (TButtonPane intermediate per-button callback pump). Body in `cls_0x5a5750_TSideTabsPane_MouseClick_53d6e0.cpp`. |
| 26 | (intermediate shared) | 0x436660 | ⚫ not-started | Per-button callback pump (lookup-by-index of registered button callbacks). |
| 27 | (= `DefWidget_DispatchInput`) | 0x4361f0 | ⚫ identified | Per B.r7 brief — shared intermediate input dispatcher. |
| 31 | (leaf override; probable scalar-dtor wrapper) | 0x487cb0 | ⚫ not-extracted | TBD. |
| 37 | (leaf override) | 0x53d790 | ⚫ not-extracted | TBD. |
| 44 | (= `DefWidget_ParsePanel`) | 0x437000 | ⚫ identified | Per B.r7 — shared intermediate. |
| ... | TPane base + TButtonPane intermediate inherited slots | various | — | See `cls_0x5a5750_vtable.txt`. |

Slot 46 (= 0x5a5808) marks the end of cls_0x5a5750's vtable and start of cls_0x5a5808 TBottomBarPane (per Wave-3B's neighbourhood note). Vtable size = 46.

## Button labels and roster (the 6 mode-switcher buttons)

From the Initialize body's FUN_0042c600 calls — string block at `0x005e4720` contains **"Book.Stats.Equip.Spell.Inv.Map"** (6 null-terminated tab labels):

| Position | Label | Y | Region | Mode value | Content pane global | cls_0x (content) | Role descriptor |
|---|---|---|---|---:|---|---|---|
| 1 | Book  | 2    | upper | 2 | `0x65a9d8` | `cls_0x5a5ae8` | SpellbookSidebarPane (Wave-3B) |
| 2 | Stats | 0x27 | upper | 1 | `0x65b140` | `cls_0x5a5ba0` | StatsSidebarPane |
| 3 | Equip | 0x4c | upper | 0 | `0x65b7e0` | `cls_0x5a55dc` | EquipSidebarPane |
| 4 | Spell | 0x71 | lower | 2 | `0x6661b0` | `cls_0x5a5978` | QuickSpellSidebarPane |
| 5 | Inv   | 0x96 | lower | 0 | `0x65d4f8` | `cls_0x5a58c0` | InventorySidebarPane |
| 6 | Map   | 0xbb | lower | 1 | `0x65b4f0` | `cls_0x5a5658` | MapSidebarPane |

`DAT_0065d1b8` = upper region active mode (0=Equip, 1=Stats, 2=Book).
`DAT_0065d1bc` = lower region active mode (0=Inv, 1=Map, 2=Spell).

The `bVar6 = DAT_0065d1b8 != N` check in Init disables each button for its own active mode (the active button is grayed-out / non-clickable).

## Visibility model (CONFIRMED — Wave-4A)

**Two independent 3-way region selectors.** NOT a flat 1-of-6 mode picker. NOT independent toggles. NOT preset combinations.

Each region (upper, lower) shows exactly ONE content pane at any time. The 6 buttons each select ONE specific mode for ONE specific region. The two regions are switched independently — Stats upper + Map lower simultaneously is valid (and is exactly what `sample_screen_1.jpg` shows).

## Related

- TSidePane: `cls_0x5a53ec` (likely owns the upper/lower content-region containers; `FUN_0053cab0` and `FUN_0053cb40` are the upper/lower region setters called by the dispatcher).
- TBottomPane: `cls_0x5a5468` (sibling pane at bottom).
- TPlayScreen command dispatcher: `FUN_0047cf40_TPlayScreen_DispatchCommand` — central command roster with 80+ cases including the 6 sidebar mode commands (cases 7-0xc).
- Forward-decl: [src/revtypes.h:1165](../../../../src/revtypes.h).

## Wave-4A summary

`docs/ui/briefs/B_r11_sidebar_tab_cascade.md` documents the full button-cascade extraction: vtable dump, MouseClick handler analysis, central dispatcher discovery, content-pane global back-tracing, ctor identification per pane, vtable identification per pane. The full sidebar content roster (6 of 6 positions) is identified.
