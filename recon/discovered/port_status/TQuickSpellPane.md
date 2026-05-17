# TQuickSpellPane port status

Retail vtable: **`cls_0x5a5a30`** (35+ slots, TButtonPane shape). Global instance: `0x65c6f8`. Wired by ctor stub at `0x00480ad0` → `FUN_00488580` (TButtonPane-style single-inheritance ctor: base vtable `0x5a4494` (TPane) → call `FUN_0041c7f0(0x10,0x10)` (TButtonPane base ctor) → derived vtable `0x5a5a30`).

Our source: [src/spellpane.h](../../../../src/spellpane.h), [src/spellpane.cpp](../../../../src/spellpane.cpp) — pre-release `class TQuickSpellPane : public TButtonPane` with `QUICKSPELLX/Y/WIDTH/HEIGHT` rect template. **Implementation exists pre-release; needs comparison against retail for divergence.**

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started · ❓ unidentified.

## Identification evidence

See [B.r5 §"TQuickSpellPane"](../../../docs/ui/briefs/B_r5_bottom_panes.md#tquickspellpane-cls_0x5a5a30) for full evidence trail. Summary:

1. Retail string `"Trouble initializing quick spell pane"` @ `0x5d72b0`
2. Vtable wire via ctor at `0x00488580`
3. Vtable shape (TButtonPane fingerprint, slot 2 = `0x435010`) — matches `src/spellpane.h` base
4. Init body loads `RingU` / `RingD` / `RingG` sprites — characteristic spell-ring graphics
5. Rect dims 420×60 match `QUICKSPELL*` constants in src

## Vtable highlights (35+ slots)

| Slot | Method | Retail addr | Status | Notes |
|---|---|---|---|---|
| 0 | `Initialize` | 0x544160 | 🟡 extracted | Body in `recon/discovered/FUN_00544160_TQuickSpellPane_init.cpp`. Allocates 4× 0x150-byte sub-entries (likely `TQuickSpellEntry`-equivalent) with vtable `&PTR_FUN_005b9c54`, each loading Ring{U,D,G} sprites; registers via `FUN_00436ae0`. |
| 1 | (TButtonPane base) | 0x434f30 | ⚫ not-started | TButtonPane base method (likely Close). |
| 2 | (TButtonPane base) | 0x435010 | ⚫ not-started | TButtonPane base method. Same as TSideTabsPane slot 2 (Wave-1A). |
| 7 | (override) | 0x436010 | ⚫ not-started | TButtonPane base override. |
| 19-29 | various overrides | 0x435d70..0x436460 | ⚫ not-started | Custom TQuickSpellPane virtuals + TButtonPane overrides. |
| 31 | (ctor?) | 0x4885e0 | ⚫ not-started | Possibly destructor or class-info method. |

The 35+ slot count is consistent with TButtonPane subclasses (TSideTabsPane has similar). No method-count explosion → not Ghidra-merged.

## Per-entry sub-class (0x150 bytes, vtable `&PTR_FUN_005b9c54`)

Allocated inside Initialize — looks like `TQuickSpellEntry` (per-button data record holding the 3 ring sprites + spell binding). Not yet identified — needs separate small extraction.

## Open questions

- Match the 4 sub-entries to specific spell slots (1/2/3 + the 4th un-bound default?)
- Verify the 0x150-byte sub-entry class matches a sibling helper in `src/spellpane.h` or is a NEW retail class
- Map the TButtonPane base vtable (0x5a4494?) to confirm TButtonPane class identity

## Related

- Class: cls_0x5a5a30 (this)
- Constructor: `recon/discovered/cls_0x5a5a30_TQuickSpellPane_ctor_488580.cpp`
- Pre-release: `src/spellpane.h`/`.cpp` (extant)
- Sibling bottom-area panes: TBarInvPane (cls_0x5a56d4), TBottomBarPane (cls_0x5a5808), TBottomPane container (cls_0x5a5468)
