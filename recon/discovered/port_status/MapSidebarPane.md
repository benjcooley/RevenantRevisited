# MapSidebarPane port status

**Role descriptor (not retail class name):** `MapSidebarPane`. Lower-region sidebar position 2 (label "Map").

**Retail vtable:** `cls_0x5a5658` (32 slots — confirmed via DumpVtable layout adjacency: cls_0x5a55dc ends at slot 31 with cls_0x5a5658 beginning). **Inheritance shape:** direct TPane leaf. **Global instance:** `0x65b4f0`. **Ctor:** inlined into CRT init thunk at `0x4808a0` (no separate ctor function). **Dtor:** `FUN_0052a400_MapSidebarPane_dtor` (atexit thunk @ `0x480910`). **Width:** 0x132 bytes; pane rect 0xbc × 0xae at offset 0x1c4 (from inline-init constants).

**Candidate src/ class:** `TAutoMap` (src/automap.h) — minimap renderer. `RECON_UI_COVERAGE.md` notes PERFECT yaml mapping for retail TAutoMap.

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started. See [README.md](README.md).

## Evidence chain (Wave-4A)

1. **Sidebar position**: lower-region mode value 1 (button label "Map" at y=0xbb).
2. **Mode-state global**: `DAT_0065d1bc == 1` selects this pane (dispatcher case 0xc — Map-only hotkey).
3. **Pane global**: `DAT_0065b4f0` — referenced in dispatcher case 0xc and sidebar code.
4. **Inline-init pattern**: disasm at 0x4808a0 shows `MOV dword ptr [0x0065b4f0], 0x5a5658` plus rect constants 0x1c4 / 0x132 / 0xbc / 0xae. Inlined-ctor style identical to Equip pane.
5. **Dtor**: atexit thunk @ 0x480910 calls `FUN_0052a400` via `MOV ECX, 0x65b4f0 / JMP 0x52a400`.

## Open hunts

- DumpVtable @ cls_0x5a5658 to enumerate slot-by-slot leaf overrides.
- Per `RECON_UI_COVERAGE.md`: PERFECT yaml mapping exists for TAutoMap. Pull the yaml-mapped methods and verify each one's address lands inside cls_0x5a5658's vtable. If they match, promote rename to `cls_0x5a5658_TAutoMap`.
- The dtor at `FUN_0052a400` is the entry point for `TAutoMap::~TAutoMap()` (if the candidate identification holds).
- Cross-reference vs src/automap.h: the pane renders a top-down minimap tracking the player's discovered area. Likely reads `DAT_00667fcc` (player) for position + the area's map-discovery bitmap.

## Related

- Brief: `docs/ui/briefs/B_r11_sidebar_tab_cascade.md`.
- Sibling inlined-ctor pane: `EquipSidebarPane.md`.
