# TSidePane port status

Retail vtable: **`cls_0x5a53ec`** (31 slots). Global instance: `0x666140`. Recon body decompilations live in `recon/discovered/cls_0x5a53ec_TSidePane_*.cpp`. Our source: none yet — class is forward-declared at [src/revtypes.h:1164](../../../../src/revtypes.h) (`_CLASSDEF(TSidePane)`) but no `.h`/`.cpp` exists. **Needs full reconstruction.**

The right-sidebar container. Holds the switchable content panes (Stats / Equip / Spell / Inventory / Automap) and likely composites them under a single root. The mode-switcher buttons themselves live on a sibling pane (`TSideTabsPane`).

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started. See [README.md](README.md).

## Vtable (31 slots, methods not yet ported)

| Slot | Method | Retail addr | Our source | Status | Notes |
|---|---|---|---|---|---|
| 0 | `Initialize` | 0x53c8c0 | — | ⚫ not-started | Trivial: calls TPane base init (FUN_00491900) then zeros 4 fields at +0x60/+0x64/+0x6c/+0x6f. Body in `cls_0x5a53ec_TSidePane_Initialize_53c8c0.cpp`. |
| 1 | (thunk → 0x491970, likely Close) | 0x53c8f0 | — | ⚫ not-started | Thunk to base. |
| 2 | (shared base, likely Pulse) | 0x491bd0 | — | ⚫ not-started | Shared with sibling panes. |
| 3 | (per-class) | 0x53c970 | — | ⚫ not-started | |
| 4 | (per-class) | 0x53ca30 | — | ⚫ not-started | |
| 18 | (per-class) | 0x53ca60 | — | ⚫ not-started | |
| 20 | (per-class) | 0x53c900 | — | ⚫ not-started | |
| ... | other slots | various | — | ⚫ not-started | Mostly base defaults (0x444f**, 0x445***). |

## Surface layout

Empty Initialize suggests the side pane has no surfaces of its own — it likely just holds child-pane references at the zeroed fields +0x60..+0x6f and composites their output. Confirm by extracting slot 3/4 (likely Draw / Animate).

## Open questions

- Which fields hold child-pane pointers (the 4 zeroed in Initialize)?
- How does TSidePane coordinate with TSideTabsPane for mode-switching?

## Related

- TSideTabsPane: `cls_0x5a5750` (the mode-switcher button strip)
- Forward-decl: [src/revtypes.h:1164](../../../../src/revtypes.h).
