# EquipSidebarPane port status

**Role descriptor (not retail class name):** `EquipSidebarPane`. Upper-region sidebar position 3 (label "Equip").

**Retail vtable:** `cls_0x5a55dc` (32 slots — confirmed Wave-4A via DumpVtable; ends at slot 31 where cls_0x5a5658 begins). **Inheritance shape:** direct TPane leaf (NO TButtonPane intermediate). **Global instance:** `0x65b7e0`. **Ctor:** inlined into CRT init thunk at `0x4807a0` (no separate ctor function — the init thunk writes the vtable `0x5a55dc` and dimensions directly). **Dtor:** inlined into atexit thunk at `0x4807f0`. **Width:** 0x132 bytes.

**Candidate src/ class:** `TEquipPane` (src/equip.h) — paper-doll + equipment slots around character body.

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started. See [README.md](README.md).

## Vtable (cls_0x5a55dc — 32 slots, leaf overrides marked)

| Slot | Addr | Notes |
|---:|---|---|
| 0 | 0x536360 | Leaf override — Initialize (likely). |
| 1 | 0x491970 | Inherited TPane base Close (0x491970). |
| 2 | 0x491bd0 | TPane base Pulse / FUN_00491bd0. |
| 7 | 0x5370b0 | Leaf override. |
| 19-22 | 0x5368c0 / 0x5366a0 / 0x5366d0 / 0x00536970 | Leaf overrides (animate / per-pass draw). |
| 25 | 0x5363e0 | Leaf override (likely MouseClick). |
| others | various | TPane base defaults. |

## Evidence chain (Wave-4A)

1. **Sidebar position**: appears at upper-region mode value 0 (button label "Equip" at y=0x4c).
2. **Mode-state global**: `DAT_0065d1b8 == 0` selects this pane (dispatcher case 9).
3. **Pane global**: `DAT_0065b7e0` — referenced in dispatcher case 9 and sidebar / playscreen code. Vtable directly written to global at init thunk + dtor thunk (inlined construction pattern).
4. **No separate ctor**: inlined into CRT init thunk @ 0x4807a0 — disasm shows `MOV dword ptr [0x0065b7e0], 0x5a55dc` (canonical MSVC inline-init for a single-instance global with trivial ctor).

## Open hunts

- Decompile each leaf-override (0x536360, 0x5370b0, 0x5368c0, 0x5366a0, 0x5366d0, 0x00536970, 0x5363e0) — likely Initialize / Update / Draw / MouseClick methods.
- Cross-reference vs src/equip.h: confirm `TEquipPane` paper-doll layout matches. The src class probably has equipment-slot fields (head, body, weapon, etc.) — look for asset names ("equip", "paperdoll", item-slot bitmaps) in the leaf methods.
- The class is in the same 0x536xxx region as 0x536b50 (Stats Initialize). The contiguous .text layout suggests Equip + Stats were compiled adjacent (same source file or same translation unit).

## Related

- Brief: `docs/ui/briefs/B_r11_sidebar_tab_cascade.md`.
- Sibling: `MapSidebarPane.md` (also uses inlined ctor pattern).
