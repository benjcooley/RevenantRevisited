# TBookPane port status

Retail vtable: **`cls_0x5b5808`** (47-slot vtable at 0x5b5808 — inherits TScrollPane's 46-slot vtable, overrides slot 0 (Initialize) and slot 80 (DrawBackground), adds 1 new slot 184). Identified by Wave-4B (2026-05-16) — see [B.r12 brief](../../../docs/ui/briefs/B_r12_oog_closeout.md) §B.r18 for the golden-path evidence chain.

Our source: [src/scroll.h](../../../src/scroll.h) lines 79-87, [src/scroll.cpp](../../../src/scroll.cpp). The retail Initialize is an exact behaviour match to `TBookPane::Initialize` in src — same asset names ("book.dat", "book", "downup"/"downdown", "updown"/"upup", "exitup"/"exitdown"), same VK_NEXT/VK_PRIOR/VK_ESCAPE keycodes, same NewButton call shape.

**Inheritance is exactly src's `TBookPane final : public TScrollPane`** — the vtable diff (only slots 0 and 80 overridden) matches src's declaration that only Initialize and DrawBackground are overridden virtuals. No new fields.

**Activation:** opened via `TScroll::Use()` when the TScroll game object's type is "Book" (src/scroll.cpp line 78 dispatch). Per CLASSIC_HUD_REFERENCE §3c, renders **in the right-sidebar slot** (paired-page two-side layout per src/scroll.cpp BookBtnScrollDown / BOOKLINES math).

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started.

## Vtable (47 slots — only leaf overrides documented; 46 slots inherited from TScrollPane)

| Slot | Method (src) | Retail addr | Our source | Status | Notes |
|---:|:---|:---|:---|:---|:---|
| 0 | `Initialize` (overrides TScrollPane) | 0x005217e0 | `src/scroll.cpp:262` | 🟡 partial | Body in `recon/discovered/cls_0x5b5808_TBookPane_Initialize_5217e0.cpp`. Loads book.dat, "book" bitmap for centring, 3 buttons (down/up/exit). Same VK keys as TScrollPane. |
| 4 | `Close` | 0x005212c0 (inherited from TScrollPane) | `src/scroll.cpp:137` | 🟡 partial | Inherited — src declares TScrollPane::Close and TBookPane doesn't override. |
| 80 | `DrawBackground` (overrides TScrollPane) | 0x00521a20 | `src/scroll.cpp:289` | ⚫ not-started | Need clean body extraction. Per src renders two pages side-by-side from the bound TScroll text with BOOKLINES per page. |
| 108 | `KeyPress` (inferred) | 0x00521530 (inherited from TScrollPane) | `src/scroll.h:57` | ⚫ not-started | Body in cls_0x5b5808.cpp shows same switch-on-VK pattern as TScrollPane. |
| 124 | scalar-dtor | 0x00521ec0 | implicit | ⚫ not-started | Calls ~TBookPane (0x00521ee0). |
| 184 | (new TBookPane-only slot) | 0x00521fb0 | (no src match) | ⚫ not-started | Allocates 0xd8-byte cls_0x5a50e8 widget. Possibly a paging-helper widget factory; defer. |
| (ctor) | `TBookPane` | 0x00521e60 | `src/scroll.h:83` | 🟡 partial | Trivial — chains to TScrollPane() then sets vftptr. Same 640x480 geometry. |
| (dtor) | `~TBookPane` | 0x00521ee0 | implicit | ⚫ not-started | |

## Struct (408 bytes — identical to TScrollPane)

No new fields beyond TScrollPane's. The `scrolldata` field at 0x180 holds book.dat instead of scroll.dat; the `scroll` field at 0x17c binds to the TScroll game object whose type is "Book".

## Visual test plan

- `--test=ui-book`: synthesise a TScroll game object with `type="Book"` and multi-page text. Open TBookPane bound to it. Validate two-page side-by-side render + PgUp/PgDn paging.

## Open questions

1. Slot 184 (FUN_00521fb0, cls_0x5a50e8 factory) — what is the helper widget? Probably page-state cache. Cross-reference call sites + extract body.
2. DrawBackground (slot 80, FUN_00521a20) — need body extraction + side-by-side diff against src/scroll.cpp::TBookPane::DrawBackground lines 289+.
3. TBookPane's "Book" sidebar tab in TSideTabsPane — Wave-3B noted the tab label; Wave-4A's sidebar cascade work should resolve which click handler instantiates this class. Closes the loop on the spellbook/book/scroll UI taxonomy.
