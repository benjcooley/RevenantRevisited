# StatsSidebarPane port status

**Role descriptor (not retail class name):** `StatsSidebarPane`. Upper-region sidebar position 1 (label "Stats").

**Retail vtable:** `cls_0x5a5ba0` (size TBD; 46-slot TButtonPane-w-DEF intermediate shape suspected from sibling cls_0x5a5ae8 having 46 slots ending at slot-46 = 0x5a5ba0 — implies cls_0x5a5ba0 is the NEXT 46-slot class). **Global instance:** `0x65b140`. **Ctor:** `FUN_00488910_StatsSidebarPane_ctor` (CRT init thunk @ `0x480d10`; ctor body explicitly writes &PTR_FUN_005a4494 then &PTR_FUN_005a5ba0; width 0x132 = 306 bytes).

**Candidate src/ class:** `TStatPane` (src/statpane.h). Wave-2B flagged the vtable[0] = `0x546b50` ("second stat-pane init") as the candidate TStatPane Initialize. NOT YET renamed to TStatPane pending per-method vtable analysis to confirm.

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started. See [README.md](README.md).

## Evidence chain (Wave-4A)

1. **Sidebar position**: appears at upper-region mode value 1 (button label "Stats" at y=0x27 in TSideTabsPane Init).
2. **Mode-state global**: `DAT_0065d1b8 == 1` selects this pane (per dispatcher case 8 in `FUN_0047cf40_TPlayScreen_DispatchCommand`).
3. **Pane global**: `DAT_0065b140` — referenced 21 times across HUD/playscreen/sidebar code; the dispatcher case 8 calls vtable+0x28 on it (Close).
4. **Ctor**: `FUN_00488910` writes `*this = &PTR_FUN_005a5ba0`.

## Open hunts

- DumpVtable @ cls_0x5a5ba0 to confirm slot count (predict 46) and slot[0] = `FUN_00546b50` (the "second stat-pane init" Wave-2B candidate).
- Cross-reference vs src/statpane.h: confirm `TStatPane` is a single-instance pane that reads from the player's stat block. If vtable methods line up with the src class shape (Initialize, Draw, MouseClick), promote rename to `cls_0x5a5ba0_TStatPane`.

## Related

- Sibling intermediate: cls_0x5a5ae8 (upper Book = SpellbookSidebarPane, Wave-3B).
- Brief: `docs/ui/briefs/B_r11_sidebar_tab_cascade.md`.
- Ctor body: `recon/discovered/cls_0x5a5ba0_StatsSidebarPane_ctor_488910.cpp`.
