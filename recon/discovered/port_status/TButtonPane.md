# TButtonPane port status

Retail vtable: **`cls_0x5a45c8`** (31 slots). Confirmed by Wave-3A (2026-05-16) as the **TButtonPane base class** — sits between TPane (cls_0x5a4494) and the various button-bearing pane leaf classes (TQuickSpellPane, TBottomBarPane, TSideTabsPane, the genuine TStatPane, the scroll-pane class cls_0x5a5ae8). See [B.r8 brief](../../../docs/ui/briefs/B_r8_charpane_draw_buttonpane.md) for the 4-line evidence chain.

Our source: [src/button.h](../../../src/button.h) lines 90-126 — substantial existing TButtonPane definition (pre-release era). The retail layout matches the src class structure closely; reconstructing the per-method bodies into the existing src/button.cpp is the port work remaining.

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started. See [README.md](README.md).

## Vtable (31 slots, overrides identified — bodies extraction pending for most)

| Slot | Method (inferred) | Retail addr | Inheritance | Our source | Status | Notes |
|---:|:---|:---:|:---:|:---|:---:|:---|
| 0 | `Initialize` | 0x00434e40 | override (was TPane 0x491900) | `src/button.h:96` | 🟡 partial | Body extracted by Wave-2B (`cls_0x5a45c8_intermediate_init_434e40.cpp`); calls TPane::Initialize then zeros 0x100-byte buffer at field [0x26] (Buttons-array storage) then zeros bookkeeping state then calls vtable[11]. **Renamed Wave-3A.** |
| 1 | `Close` | 0x00434f30 | override (was TPane 0x491970) | `src/button.h:97` | ⚫ not-started | Body not yet extracted; slot-position pairing convention with Initialize. **Renamed Wave-3A on slot-position evidence.** |
| 2 | (TButtonPane override of TPane 0x491bd0) | 0x00435010 | override | — | ⚫ not-started | Used by TButtonPane + 3 leaf subclasses. Body needs extraction. Could be `PaneResized` (non-trivial TPane base) overridden because button positions need re-layout. |
| 3..6 | TPane base stubs | 0x444f40..0x444f70 | inherited | — | ⚫ not-started | Inherited from TPane base. |
| 7 | (TButtonPane override; per src likely `Update`) | 0x00436010 | override | `src/button.h:106` | ⚫ not-started | TButtonPane src `Update() { TPane::Update(); RedrawButtons(); }` — body extraction should confirm. |
| 8 | TPane base stub | 0x00444f90 | inherited | — | ⚫ not-started | Inherited. |
| 9 | TPane base shared | 0x00491a80 | inherited | — | ⚫ not-started | Inherited. |
| 10 | (TButtonPane override of TPane 0x444fb0) | 0x00445210 | override | — | ⚫ not-started | Body extraction needed. |
| 11..15 | TPane base stubs | 0x444fc0..0x445000 | inherited | — | ⚫ not-started | Inherited. |
| 16 | TPane base shared | 0x00491bb0 | inherited | — | ⚫ not-started | Inherited. |
| 17, 18 | TPane base stubs | 0x445020, 0x445030 | inherited | — | ⚫ not-started | Inherited. |
| 19 | `DrawBackground` (per src) | 0x00435d70 | override | `src/button.h:101` | ⚫ not-started | TButtonPane overrides DrawBackground to render the button background art. |
| 20 | `Animate(bool draw)` (per src) | 0x00435de0 | override | `src/button.h:102` | ⚫ not-started | TButtonPane overrides Animate to draw all buttons per-frame. |
| 21 | TPane base stub (Overlay default) | 0x004451e0 | inherited | — | ⚫ not-started | Inherited; TButtonPane doesn't override Overlay. |
| 22 | (TButtonPane override; likely `MouseClick`) | 0x00435f90 | override | `src/button.h:99` | ⚫ not-started | Body extraction needed. Slot adjacency with slot 23 (MouseMove) + slot 25/26 suggests this is MouseClick. |
| 23 | (TButtonPane override; likely `MouseMove`) | 0x00436090 | override | `src/button.h:100` | ⚫ not-started | Body extraction needed. |
| 24 | TPane base stub | 0x00445080 | inherited | — | ⚫ not-started | Inherited. |
| 25 | (TButtonPane override; likely `KeyPress`) | 0x00436530 | override | `src/button.h:98` | ⚫ not-started | Body extraction needed. |
| 26 | (TButtonPane override; likely `CharPress`) | 0x00436660 | override | — | ⚫ not-started | TButtonPane src doesn't have CharPress in the override list (src/button.h:90-126), but the slot is overridden in retail. Could be a non-listed virtual. |
| 27 | (TButtonPane override of TPane 0x4451f0) | 0x004361f0 | override | — | ⚫ not-started | Body extraction needed. |
| 28 | (TButtonPane override of TPane 0x445200) | 0x00436460 | override | — | ⚫ not-started | Body extraction needed. |
| 29 | (TButtonPane override of TPane 0x4450b0) | 0x00436340 | override | — | ⚫ not-started | Body extraction needed. |
| 30 | TPane base stub | 0x004450c0 | inherited | — | ⚫ not-started | Inherited. |

13 overrides total — more than the 7 src::button.h lists. Discrepancy explained by either (a) retail TButtonPane had additional virtuals that were de-virtualized in pre-release src/, or (b) slot-to-name mapping is shifted because the retail TPane vtable order differs from current src/screen.h declaration order. **Wave-4 should extract slots 7, 19, 20, 22, 23, 25, 26, 27, 28, 29 bodies to resolve.**

## Inheritance / derived classes

Multiple confirmed-distinct leaf classes inherit from cls_0x5a45c8:

| Derived class | cls addr | Identification source |
|---|---|---|
| TQuickSpellPane | `cls_0x5a5a30` | Wave-2A: "quick spell pane" string anchor + src/spellpane.h forward decl + RingU/D/G spell-ring sprites |
| TBottomBarPane | `cls_0x5a5808` | Wave-2A: "Bottom bar" string anchor + UtilityBar/BarEndCap sprites + byte-identical ctor to TQuickSpellPane (B.r5) |
| TSideTabsPane | `cls_0x5a5750` | Wave-1A re-launch: "SideTabs pane" string anchor + 6 TButton creations (Stats/Equip/Spell labels) + src/revtypes.h forward decl |
| ScrollPane (TScrollPane / TBookPane / similar) | `cls_0x5a5ae8` | Wave-2B: 2nd "stat pane" init (FUN_005449e0) loads ScrlTop/ScrlBottom/ScrollPaper sprites + scroll-up/down TButtons; 3-stage MSVC ctor pattern in cls_0x5a5ae8_ctor_488620.cpp explicitly writes 0x5a45c8 intermediate |
| TStatPane (the genuine 1st "stat pane") | unknown leaf cls @ global 0x65b140 | Wave-2B: FUN_00546b50 Init loads `statpane.def` + "RingT" portrait; body starts with FUN_00434e40 call (TButtonPane init). Leaf class identity not yet pinned but TButtonPane derivation is confirmed. |

5+ derived classes use cls_0x5a45c8 as base = robust base-class pattern.

## TButtonPane ctor calling pattern

The MSVC ctor for cls_0x5a45c8 itself appears OPTIMIZED OUT of most derived class ctors. Pattern observed (e.g. TQuickSpellPane ctor at FUN_00488580):

```c
*param_1 = &PTR_FUN_005a4494;   // TPane base vtable
// rect setup (x, y, w, h fields)
FUN_0041c7f0(0x10, 0x10);       // TPointerArray<TButton>::TPointerArray(MAXBUTTONS, ?)
*param_1 = &PTR_FUN_005a5a30;   // leaf vtable (directly -- TButtonPane vtable write skipped)
```

The compiler skips writing the TButtonPane vtable because the leaf vtable immediately overwrites it. Only classes with non-trivial code between the TButtonPane write and the leaf write keep the intermediate (e.g. cls_0x5a5ae8 which has heap allocation + error handling between the two).

## Open questions

- Slots 2, 7, 10, 19, 20, 22, 23, 25, 26, 27, 28, 29 need body extraction to confirm src-name mapping.
- The src::button.h override list (7 virtuals: Initialize, Close, KeyPress, MouseClick, MouseMove, DrawBackground, Animate) is shorter than the 13 retail slots overridden — either src is incomplete OR the slot-to-name mapping is shifted vs current src/screen.h declaration order.
- `FUN_00434f30` (slot 1 = Close) needs body extraction to formally confirm the slot-1 = Close pairing (renamed Wave-3A on convention; extraction would upgrade to body-semantics evidence).

## Related

- TPane base: `cls_0x5a4494` (Wave-2B confirmed). Pre-release src: [src/screen.h](../../../src/screen.h) class TPane.
- Pre-release src impl: [src/button.h](../../../src/button.h) class TButtonPane.
- Brief: [B.r8](../../../docs/ui/briefs/B_r8_charpane_draw_buttonpane.md).
