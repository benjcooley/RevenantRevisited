# TSideTabsPane port status

Retail vtable: **`cls_0x5a5750`** (32+ slots). Global instance: `0x65be50`. Constructor: `0x487c60` (sets base vtable 0x5a4494, calls FUN_0041c7f0, then writes derived vtable 0x5a5750). Recon body decompilations live in `recon/discovered/cls_0x5a5750_TSideTabsPane_*.cpp`. Our source: none yet — class is forward-declared at [src/revtypes.h:1165](../../../../src/revtypes.h) (`_CLASSDEF(TSideTabsPane)`) but no `.h`/`.cpp` exists. **Needs full reconstruction.**

The vertical mode-switcher button strip on the right edge of the sidebar. Per the Initialize body, it owns six TButton instances labeled (in order of construction): unnamed (DAT_005e4720), "Stats", "Equip", "Spell", unnamed (DAT_005e4740 = likely "Inventory"), unnamed (DAT_005e4744 = likely "Automap"). When clicked, each button switches `TSidePane` to display the corresponding content sub-pane.

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started. See [README.md](README.md).

## Vtable (32+ slots, methods not yet ported)

| Slot | Method | Retail addr | Our source | Status | Notes |
|---|---|---|---|---|---|
| 0 | `Initialize` | 0x53cc30 | — | ⚫ not-started | Loads 3 graphics via FUN_0046d710 (stored at +0x5f/+0x60/+0x61), allocates 3 mosaic surfaces at +0x62/+0x63/+0x64 sized from the loaded graphic, then creates 6 TButton via FUN_0042c600. Calls `this->vftptr[8]()` at end. Body in `cls_0x5a5750_TSideTabsPane_Initialize_53cc30.cpp`. |
| 1 | (per-class, likely Close) | 0x53d360 | — | ⚫ not-started | |
| 2 | (per-class, base ~ Pulse) | 0x435010 | — | ⚫ not-started | NOT the shared 0x491bd0 — TSideTabsPane has its own slot 2. |
| 7 | (per-class) | 0x53d420 | — | ⚫ not-started | |
| 19 | (per-class) | 0x53d3a0 | — | ⚫ not-started | |
| 20 | (per-class) | 0x53d400 | — | ⚫ not-started | |
| 23 | (per-class) | 0x53d540 | — | ⚫ not-started | |
| 25 | (per-class) | 0x53d6e0 | — | ⚫ not-started | |
| 26..29 | (per-class, button-handling region?) | 0x436660 / 0x4361f0 / 0x436460 / 0x436340 | — | ⚫ not-started | Cluster of slots in the 0x436xxx range — possibly TButton-list management base methods. |
| ... | other slots | various | — | ⚫ not-started | Base defaults. |

## Button labels (the 6 mode-switcher buttons, in declaration order)

From the Initialize body's FUN_0042c600 calls:

| # | Label string | X | Y | W | H | DAT_0065d1?? check |
|---|---|---|---|---|---|---|
| 1 | DAT_005e4720 (unnamed) | 3 | 2 | 0x24 | 0x23 | DAT_0065d1b8 != 2 |
| 2 | "Stats" | 3 | 0x27 | 0x24 | 0x23 | DAT_0065d1b8 != 1 |
| 3 | "Equip" | 3 | 0x4c | 0x24 | 0x23 | DAT_0065d1b8 != 0 |
| 4 | "Spell" | 3 | 0x71 | 0x24 | 0x23 | DAT_0065d1bc != 2 |
| 5 | DAT_005e4740 (unnamed) | 3 | 0x96 | 0x24 | 0x23 | DAT_0065d1bc != 0 |
| 6 | DAT_005e4744 (unnamed) | 3 | 0xbb | 0x24 | 0x23 | DAT_0065d1bc != 1 |

DAT_0065d1b8 / DAT_0065d1bc look like "currently active tab" tracking variables (the `!=` check disables the button for its own active mode).

## Open questions

- What are the three unnamed string-DATs? (Likely "Map" or icon-only — these are probably the icon-style buttons.)
- What is `cls_0x5a4494` (the base vtable used in the ctor)? Likely the TPane base class.
- The ctor pattern (base-vtable + base-ctor + derived-vtable) is classic single-inheritance — confirms TSideTabsPane inherits from one class directly.

## Related

- TSidePane: `cls_0x5a53ec` (the container the tabs switch between).
- TBottomPane: `cls_0x5a5468` (sibling pane at bottom).
- Forward-decl: [src/revtypes.h:1165](../../../../src/revtypes.h).
