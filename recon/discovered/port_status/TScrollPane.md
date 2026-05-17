# TScrollPane port status

Retail vtable: **`cls_0x5b5750`** (46-slot TButtonPane-with-DEF-intermediate subclass at vtable address 0x5b5750). Identified by Wave-4B (2026-05-16) — see [B.r12 brief](../../../docs/ui/briefs/B_r12_oog_closeout.md) §B.r18 for the golden-path evidence chain.

Our source: [src/scroll.h](../../../src/scroll.h) lines 48-73, [src/scroll.cpp](../../../src/scroll.cpp). The retail Initialize is an exact behaviour match to `TScrollPane::Initialize` in src — same asset names ("scroll.dat", "scroll", "downup"/"downdown", "updown"/"upup", "exitup"/"exitdown"), same VK_NEXT/VK_PRIOR/VK_ESCAPE keycodes, same NewButton call shape.

**Activation:** TScrollPane is opened via `TScroll::Use()` (src/scroll.cpp line 78 dispatches Book vs Scroll types) which AddPane's a TScrollPane bound to the reading TScroll game object. Per CLASSIC_HUD_REFERENCE §3c, the scroll renders **in the right-sidebar slot** (not as a full-screen overlay) — verify against retail screenshot during port.

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started.

## Vtable (46 slots; ALL slots inherited from TPane / TButtonPane-with-DEF except those listed)

| Slot | Method (src) | Retail addr | Our source | Status | Notes |
|---:|:---|:---|:---|:---|:---|
| 0 | `Initialize` | 0x00521080 | `src/scroll.cpp:115` | 🟡 partial | Body in `recon/discovered/cls_0x5b5750_TScrollPane_Initialize_521080.cpp`. Loads scroll.dat, fetches "scroll" bitmap for centring, creates 3 buttons (down/up/exit) with VK_NEXT/VK_PRIOR/VK_ESCAPE. |
| 4 | `Close` | 0x005212c0 | `src/scroll.cpp:137` | 🟡 partial | Body in cls_0x5b5808.cpp (shared by TBookPane). Calls TButtonPane intermediate Close, frees scrolldata, calls PlayScreen.{ReleaseExclusivePane,RemovePane,Redraw}. |
| 80 | `DrawBackground` | 0x00521360 | `src/scroll.cpp:154` | 🟡 partial | Body in `recon/classes_original/cls_0x5b5750.cpp` (virt_meth_0x521360). Renders scroll bitmap centred + iterates text lines from this->scroll->text. |
| 108 | `KeyPress` (inferred) | 0x00521530 | `src/scroll.h:57` | ⚫ not-started | Body in cls_0x5b5808.cpp (shared). Switch on 0x23/0x24/0x26/0x28 (VK_END/HOME/LEFT/RIGHT) adjusting this->line by ±100000 (page) or ±1 (line). |
| 124 | scalar-dtor | 0x00521dd0 | implicit | ⚫ not-started | Calls ~TScrollPane (0x00521df0). |
| (ctor) | `TScrollPane` | 0x00521d70 | `src/scroll.h:52` | 🟡 partial | Sets pane to (0,0,640,480) = (0, 0, WIDTH, HEIGHT). |
| (dtor) | `~TScrollPane` | 0x00521df0 | implicit | ⚫ not-started | |

All other slots (3, 5..9, 28, 32, 36, 40..76, 84..120, 128..180) are inherited from TPane (cls_0x5a4494) and TButtonPane-with-DEF intermediate (cls_0x5a45c8) — see [TPane.md](TPane.md).

## Struct (408 bytes — confirmed)

Inferred field layout (from method bodies):

| Offset | Field | Type | src match | Notes |
|---:|:---|:---|:---|:---|
| 0 | vftptr | `cls_0x5b5750::vftable_5b5750 *` | (implicit) | |
| 0x4..0x88 | base TPane fields | (inherited) | `src/screen.h::TPane` | x/y/w/h/cliprect/etc |
| 0x88..0x17c | base TButtonPane-w-DEF fields | (inherited) | (need full TButtonPane intermediate port-status) | |
| 0x17c | `scroll` | `TScroll*` | `src/scroll.h:66 PTScroll scroll;` | The TScroll game object being read |
| 0x180 | `scrolldata` | `TMulti*` | `src/scroll.h:68 TMulti* scrolldata;` | The "scroll.dat" multi |
| 0x188 | `line` | `int32_t` | `src/scroll.h:71 int32_t line;` | Scroll position |
| 0x18c | `numlines` | `int32_t` | `src/scroll.h:72 int32_t numlines;` | Total lines in text |
| 0x190 | `scrollfont` (or font state #1) | `TFont*` | `src/scroll.h:69 TFont* scrollfont;` | Font for body text |
| 0x194 | (font state #2 / colour?) | dword | (no src match yet) | |

## Visual test plan

- `--test=ui-scroll`: synthesise a TScroll game object with sample text, open TScrollPane bound to it. Render in the right-sidebar slot. Validate text wraps + scrolls correctly via PgUp/PgDn/arrow keys.

## Open questions

1. Field at 0x194 — possibly font colour state or line-height cache. Cross-reference src/font.h.
2. KeyPress (slot 108) — extract body cleanly + match against src/scroll.h:57's KeyPress signature.
3. TScrollPane lives in the RIGHT SIDEBAR slot per CLASSIC_HUD_REFERENCE §3c, but the ctor sets full-screen 640x480 dims. Reconcile: probably the pane is sized full-screen for clip purposes but Renders only within the sidebar via clip rect setup in TButtonPane intermediate.
