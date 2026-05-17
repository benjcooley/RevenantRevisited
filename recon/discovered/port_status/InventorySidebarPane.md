# InventorySidebarPane port status

**Role descriptor (not retail class name):** `InventorySidebarPane`. Lower-region sidebar position 1 (label "Inv").

**Retail vtable:** `cls_0x5a58c0` (size suspected 32 slots, TPane-direct leaf shape; vtable confirmation deferred). **Global instance:** `0x65d4f8`. **Ctor:** `FUN_00487f50_InventorySidebarPane_ctor` (CRT thunk @ `0x480620`). **Dtor:** `FUN_00537980_InventorySidebarPane_dtor` (atexit thunk @ `0x480640`). **Width:** 0x132 bytes.

**Candidate src/ class:** `TInventory` (src/inventory.h) — item grid.

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started. See [README.md](README.md).

## Evidence chain (Wave-4A)

1. **Sidebar position**: lower-region mode value 0 (button label "Inv" at y=0x96).
2. **Mode-state global**: `DAT_0065d1bc == 0` selects this pane (dispatcher case 9 sets it; case 0xb is its dedicated hotkey).
3. **Pane global**: `DAT_0065d4f8` — 25+ references; STRONG corroboration that this is the inventory pane: most references are item-pickup/drop helpers in the 0x46f3d0-0x47001e and 0x472430 range (gameplay-side inventory mutations: FUN_0046f3d0, FUN_0046faf0, FUN_0046fc40, FUN_0046fd30, FUN_0046ff80, FUN_00472430).
4. **Ctor**: `FUN_00487f50` writes `*this = &PTR_FUN_005a58c0`.

## Open hunts

- DumpVtable @ cls_0x5a58c0 to confirm slot count and identify Initialize/Draw/MouseClick virtuals.
- Cross-reference vs src/inventory.h: confirm `TInventory` is the item-grid renderer. Existing port-status doc for `TBarInvPane.md` (bottom-bar inventory) should be compared — the sidebar Inventory class may share or extend that one.
- Per user (RETAIL_UI_RECOVERY_PLAN Tier 11): the container relationship between upper "Equip" (paper-doll grid) and lower "Inv" (item grid) is TBD. They could be separate panes (as the vtable separation suggests) OR the lower-Inv pane fills the inventory grid when Equip is the active upper-region pane. Wave-4A finding clarifies they are SEPARATE classes with SEPARATE region-mode controls, so they ARE separate panes that can be active simultaneously.

## Related

- Brief: `docs/ui/briefs/B_r11_sidebar_tab_cascade.md`.
- Ctor body: `recon/discovered/cls_0x5a58c0_InventorySidebarPane_ctor_487f50.cpp`.
- Sibling bottom-bar inv: `TBarInvPane.md` (different class, different position).
