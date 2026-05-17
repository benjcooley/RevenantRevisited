# TBarInvPane port status

Retail vtable: **`cls_0x5a56d4`** (31 slots, leaf-TPane shape). Global instance: `0x65b028`. Wired directly via static-init: `0x00480ce8` and `0x00480d00` both `MOV dword ptr [0x0065b028], 0x5a56d4` (idempotent .CRT$ ctor pattern).

Our source: **none yet** — class is NOT forward-declared in `src/revtypes.h`. **NEW retail class with no pre-release predecessor; needs full reconstruction.** Class name "TBarInvPane" inferred from retail error string `"Trouble initializing BarInv pane"` (~85% naming confidence — could equally be `TBarInventoryPane`, `TItemBarPane`).

The potion/key/consumable slot strip — the row of small icon boxes between the spell ring and the side bar.

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started · ❓ unidentified.

## Identification evidence

See [B.r5 §"TBarInvPane"](../../../docs/ui/briefs/B_r5_bottom_panes.md#tbarinvpane-cls_0x5a56d4) for full evidence trail. Summary:

1. Retail string `"Trouble initializing BarInv pane"` @ `0x5d72dc`
2. Direct .data vtable wire at `0x00480ce8` (`MOV [0x65b028], 0x5a56d4`)
3. Vtable shape (leaf-TPane fingerprint: slot 1=`0x491970`, slot 2=`0x491bd0`, slot 9=`0x491a80`, slot 16=`0x491bb0`) — matches Wave-1A TBottomPane/TSidePane shape exactly
4. Init body loads `"BarInvBox"` sprite + configures width-derived slot grid `(screen_width - 0xdc) / 0x2d`

## Vtable (31 slots, methods not yet ported)

| Slot | Method | Retail addr | Status | Notes |
|---|---|---|---|---|
| 0 | `Initialize` | 0x52c970 | 🟡 extracted | 129-byte body in `recon/discovered/FUN_0052c970_BarInv_init.cpp`. Calls TPane base init, loads "BarInvBox" sprite into +0x21, computes slot count `(DAT_0065b644 - 0xdc) / 0x2d` (screen-width-derived) into +0x23, calls 2 own virtuals at offsets +0x20 and +0x2c. |
| 1 | (TPane base) | 0x491970 | ⚫ not-started | Standard TPane Close. |
| 2 | (TPane base) | 0x491bd0 | ⚫ not-started | Shared base method. |
| 9 | (TPane base) | 0x491a80 | ⚫ not-started | Shared base method. |
| 12 | (override) | 0x487f30 | ⚫ not-started | Custom override. |
| 16 | (TPane base) | 0x491bb0 | ⚫ not-started | Shared base method. |
| 18 | (override) | 0x52ca00 | ⚫ not-started | |
| 20 | (override) | 0x52ca50 | ⚫ not-started | |
| 21 | (override) | 0x52ca70 | ⚫ not-started | |
| 22 | (override) | 0x52cd80 | ⚫ not-started | |
| 25 | (override) | 0x52cfb0 | ⚫ not-started | |
| 26 | (override) | 0x52d6e0 | ⚫ not-started | |

Overrides cluster in `0x52c***..0x52d***` range — likely all TBarInvPane methods (Draw, Update, ItemAtPos, etc.).

## Suspected public API (needs per-method extraction to confirm)

| Method | Status | Notes |
|---|---|---|
| `Initialize()` | 🟡 partial | Done — slot 0 above. |
| Slot lookup (item at mouse pos) | ⚫ not-started | Likely one of slot 18/20/21. |
| Drag/drop integration | ⚫ not-started | Likely calls into Player inventory state. |
| Drawing | ⚫ not-started | Slot 18/20/21 candidates; uses BarInvBox sprite repeated `(width-0xdc)/0x2d` times. |

## Open questions

- Confirm name: is it really `TBarInvPane`, or `TBarInventoryPane`, `TItemBarPane`, etc.? Naming inference from retail string only; ~85% confidence on the exact name.
- Identify the slot-count formula's parameters — `0xdc` = 220 (probably left+right margin); `0x2d` = 45 (slot pitch in pixels)
- What's the source of items shown? Player's inventory? A dedicated quick-bar binding?
- Verify NEW-class status — grep `src/` for any other "bar inv" / "potion shelf" / "key slot" references to rule out pre-release predecessor

## Related

- Class: cls_0x5a56d4 (this)
- B.r5 brief: `docs/ui/briefs/B_r5_bottom_panes.md`
- Pre-release: NONE (NEW retail class)
- Sibling bottom-area panes: TQuickSpellPane (cls_0x5a5a30), TBottomBarPane (cls_0x5a5808), TBottomPane container (cls_0x5a5468)
- TPane base (parent class, likely cls_0x5a4494) — pending Wave-2B
