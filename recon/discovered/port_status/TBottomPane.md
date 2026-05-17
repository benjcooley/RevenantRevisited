# TBottomPane port status

Retail vtable: **`cls_0x5a5468`** (31 slots). Global instance: `0x667c58`. Recon body decompilations live in `recon/discovered/cls_0x5a5468_TBottomPane_*.cpp`. Our source: none yet — class is forward-declared at [src/revtypes.h:1167](../../../../src/revtypes.h) (`_CLASSDEF(TBottomPane)`) but no `.h`/`.cpp` exists. **Needs full reconstruction.**

The bottom-area container. The actual content (quickspell row, potion shelf, bottom bar) is initialized by sibling panes (see "Related" below). TBottomPane itself is a thin holder.

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started. See [README.md](README.md).

## Vtable (31 slots, methods not yet ported)

| Slot | Method | Retail addr | Our source | Status | Notes |
|---|---|---|---|---|---|
| 0 | `Initialize` | 0x52d8a0 | — | ⚫ not-started | Trivial: `FUN_00491900(); return 1;` — just calls TPane base init. Body in `cls_0x5a5468_TBottomPane_Initialize_52d8a0.cpp`. |
| 1 | (thunk → 0x491970) | 0x52d8b0 | — | ⚫ not-started | Thunk to base; likely Close. |
| 2 | (shared base) | 0x491bd0 | — | ⚫ not-started | Shared with sibling panes. |
| 3 | (per-class) | 0x52d910 | — | ⚫ not-started | |
| 4 | (per-class) | 0x52d9d0 | — | ⚫ not-started | |
| 8 | (per-class) | 0x52da60 | — | ⚫ not-started | |
| 18 | (per-class) | 0x52da00 | — | ⚫ not-started | |
| 20 | (per-class) | 0x52d8c0 | — | ⚫ not-started | |
| ... | other slots | various | — | ⚫ not-started | Base defaults. |

## Sibling panes (separately constructed, not children of TBottomPane vtable-wise)

These pane-init calls happen alongside TBottomPane in TPlayScreen::Initialize. **All three resolved by Wave-2A — see [B.r5](../../../docs/ui/briefs/B_r5_bottom_panes.md):**

- `FUN_00544160` = **TQuickSpellPane::Initialize** (`cls_0x5a5a30`, TButtonPane subclass) — the 4-slot spell-ring strip with Ring{U,D,G} sprites
- `FUN_0052c970` = **TBarInvPane::Initialize** (`cls_0x5a56d4`, leaf TPane, NEW retail class) — the potion/key slot row with "BarInvBox" sprite, width-derived slot count
- `FUN_0052c780` = **TBottomBarPane::Initialize** (`cls_0x5a5808`, TButtonPane subclass, NEW retail class) — utility/action button row with "UtilityBar" + "BarEndCap" sprites; **DISTINCT from this TBottomPane container**

The relationship between TBottomPane and these (parent/child vs. peer) is still not established vtable-wise; they're four distinct globals. They likely composite at draw time without TBottomPane owning them — TBottomPane appears to be a transparent backdrop / region marker.

## Open questions

- Why is TBottomPane::Initialize so trivial? Likely most logic is in Pulse/Draw or it's just a transparent backdrop / region marker that the other three panes draw into.

## Related

- "Bottom bar" init: FUN_0052c780 — confusingly similar name, distinct class.
- "Quick spell pane" init: FUN_00544160 — bottom quickspell row.
- "BarInv pane" init: FUN_0052c970 — potion shelf.
- Forward-decl: [src/revtypes.h:1167](../../../../src/revtypes.h).
