# QuickSpellSidebarPane port status

**Role descriptor (not retail class name):** `QuickSpellSidebarPane`. Lower-region sidebar position 3 (label "Spell").

**Retail vtable:** `cls_0x5a5978` (size suspected 32 slots, TPane-direct leaf — adjacent vtable to cls_0x5a58c0 and cls_0x5a5658 which are confirmed 32-slot leaves). **Global instance:** `0x6661b0`. **Ctor:** `FUN_00488460_QuickSpellSidebarPane_ctor` (CRT thunk @ `0x4809e0`). **Dtor:** `FUN_004884c0` (atexit thunk @ `0x480a00`). **Width:** 0x132 bytes.

**Candidate src/ class:** UNCLEAR — three plausible candidates per src/:
- `TQuickSpellPane` (src/spellpane.h) — but `cls_0x5a5a30 = TQuickSpellPane` is already identified (the bottom-bar quickspell row at the bottom of the screen, see `TQuickSpellPane.md`).
- `TSpellPane` (src/spellpane.h) — the talisman composer. Per Wave-3B's rejection of this for cls_0x5a5ae8: TSpellPane has a distinct content source (TalismanClass.NumTypes()) and button vocabulary that doesn't match the sidebar.
- A retail-era class NOT in src/ (added between pre-release snapshot and ship) — Wave-3B precedent for cls_0x5a5ae8.

This lower-sidebar "Spell" tab is **distinct from `cls_0x5a5a30` (TQuickSpellPane bottom-bar)** — they are different classes, at different positions, with separate globals.

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started. See [README.md](README.md).

## Evidence chain (Wave-4A)

1. **Sidebar position**: lower-region mode value 2 (button label "Spell" at y=0x71).
2. **Mode-state global**: `DAT_0065d1bc == 2` selects this pane (dispatcher cases 7 and 0xa — both set this value, paired with the upper Book mode).
3. **Pane global**: `DAT_006661b0` — 25+ references; dispatcher case 7 calls vtable+0x28 (Close) on it.
4. **Ctor**: `FUN_00488460` writes `*this = &PTR_FUN_005a5978`.

## Visibility wrinkle (Wave-4A finding)

The dispatcher cases 7 and 0xa always pair the lower "Spell" tab with the upper "Book" tab — i.e., these specific hotkeys switch BOTH regions at once. But the BUTTON STRIP (TSideTabsPane) treats them as independent — clicking the "Spell" button alone (via FUN_00436900-routed callback) would only change the lower region. The case 7 / case 0xa pairing is likely a keyboard hotkey ("press [some-key] to switch to spell view") that semantically means "show me my spells" and thus opens BOTH the spellbook (Book upper) AND the quickcast row (Spell lower).

## Open hunts

- DumpVtable @ cls_0x5a5978 to confirm slot count and identify Initialize/Draw/MouseClick virtuals.
- Per Wave-3B's protocol (cost-asymmetry rule), do NOT rename cls_0x5a5978 to TSpellPane / TQuickSpellPane until per-method body extraction confirms the class identity.
- Visual reconstruction: the CLASSIC_HUD §4 quickspell bar has 4 slots; the sidebar "Spell" tab content is unknown — could be a paginated full spell-list (like SpellbookSidebarPane upper) OR a quickcast-shortcut grid OR something else entirely.

## Related

- Brief: `docs/ui/briefs/B_r11_sidebar_tab_cascade.md`.
- Bottom-bar quickspell (different class): `TQuickSpellPane.md` (cls_0x5a5a30).
- Spellbook upper sibling: `SpellbookSidebarPane.md` (cls_0x5a5ae8, Wave-3B).
- Ctor body: `recon/discovered/cls_0x5a5978_QuickSpellSidebarPane_ctor_488460.cpp`.
