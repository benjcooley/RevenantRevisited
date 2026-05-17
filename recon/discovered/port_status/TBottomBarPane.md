# TBottomBarPane port status

Retail vtable: **`cls_0x5a5808`** (35+ slots, TButtonPane shape). Global instance: `0x65b638`. Wired by ctor stub at `0x00480550` → `FUN_00487da0` (TButtonPane-style single-inheritance ctor: base vtable `0x5a4494` (TPane) → call `FUN_0041c7f0(0x10,0x10)` (TButtonPane base ctor) → derived vtable `0x5a5808`).

**Distinct from `TBottomPane` (cls_0x5a5468)** — that's the container, this is the inner utility/action button row.

Our source: **none yet** — class is NOT forward-declared in `src/revtypes.h`. **NEW retail class with no pre-release predecessor; needs full reconstruction.** Class name "TBottomBarPane" inferred from retail error string `"Trouble initializing Bottom bar"` (~85% naming confidence — could be `TUtilityBarPane`, `TActionBarPane`, `TBottomButtonPane`).

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started · ❓ unidentified.

## Identification evidence

See [B.r5 §"TBottomBarPane"](../../../docs/ui/briefs/B_r5_bottom_panes.md#tbottombarpane-cls_0x5a5808) for full evidence trail. Summary:

1. Retail string `"Trouble initializing Bottom bar"` @ `0x5d7344`
2. Vtable wire via ctor at `0x00487da0` — **byte-identical** to TQuickSpellPane ctor at `0x00488580` except for derived vtable address (strong evidence of shared base class = TButtonPane)
3. Vtable shape (TButtonPane fingerprint, slot 2 = `0x435010`)
4. Init body loads `"UtilityBar"` + `"BarEndCap"` sprites (decorative bar frame)
5. Rect dims 420×60 (same as TQuickSpellPane — sits on the same bottom row pixel band)

## Vtable highlights (35+ slots)

| Slot | Method | Retail addr | Status | Notes |
|---|---|---|---|---|
| 0 | `Initialize` | 0x52c780 | 🟡 extracted | 59-byte body in `recon/discovered/FUN_0052c780_BottomBar_init.cpp`. Calls `FUN_00434e40()` (likely TButtonPane base init), loads "UtilityBar" into +0x180 and "BarEndCap" into +0x184. |
| 1 | (override) | 0x52c7c0 | ⚫ not-started | Custom; calls TPane Close + 2 calls into 0x434f30. |
| 2 | (TButtonPane base) | 0x435010 | ⚫ not-started | Shared with TQuickSpellPane / TSideTabsPane. |
| 7 | (TButtonPane base) | 0x436010 | ⚫ not-started | |
| 8 | (override) | 0x52c930 | ⚫ not-started | |
| 13 | (override) | 0x52c7e0 | ⚫ not-started | |
| 18 | (override) | 0x52c910 | ⚫ not-started | |
| 19 | (override) | 0x52c960 | ⚫ not-started | |
| 20 | (override) | 0x52c800 | ⚫ not-started | |
| 21 | (override) | 0x52c880 | ⚫ not-started | |
| 23 | (TButtonPane base) | 0x436090 | ⚫ not-started | |
| 25-29 | various | 0x487e80..0x487ef0 | ⚫ not-started | Possibly button management / mouse handling. |

## Suspected role

The "UtilityBar" + "BarEndCap" naming + button-strip rect + TButtonPane base = a row of action buttons (menu / inventory toggle / map toggle / etc.) flanked by decorative end-caps. The actual button instances must be registered somewhere — possibly in the TButtonPane base ctor (`FUN_0041c7f0(0x10,0x10)`) which we haven't extracted, or in one of the custom slot virtuals.

## Open questions

- Confirm name: `TBottomBarPane` vs `TUtilityBarPane` vs `TActionBarPane` — only the retail error string and src class-name conventions to go on
- Find the button-registration path — Initialize is only 59 bytes and doesn't add buttons, so they're either added by the ctor's base call or by a sibling method
- Identify which buttons are present (compare against CLASSIC_HUD_REFERENCE.md screenshots)

## Related

- Class: cls_0x5a5808 (this)
- Constructor: `recon/discovered/cls_0x5a5808_TBottomBarPane_ctor_487da0.cpp` (byte-identical to TQuickSpellPane ctor except final vtable)
- B.r5 brief: `docs/ui/briefs/B_r5_bottom_panes.md`
- Pre-release: NONE (NEW retail class)
- Sibling bottom-area panes: TQuickSpellPane (cls_0x5a5a30), TBarInvPane (cls_0x5a56d4), TBottomPane container (cls_0x5a5468)
- TButtonPane base (vtable address pending identification)
