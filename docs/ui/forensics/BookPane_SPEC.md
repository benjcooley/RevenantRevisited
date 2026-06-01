# TBookPane forensics spec

Status: **forensics-complete** for static layout (chrome + 3 buttons + 2-page text)
and per-page text composition; DrawBackground body is **inferred 1:1 from the
1998/99 src/scroll.cpp::TBookPane::DrawBackground (retail-superseding evidence:
TBookPane::Initialize at 0x5217e0 matches snapshot Initialize literal-for-literal
across all 6 numeric args of all 3 buttons, both centring divisors, both VK
keycodes, both bitmap names → the same source compiled both — see §0)**.
The retail DrawBackground body at `0x521a20` is not yet dropped to
`recon/discovered/`; it must be extracted to upgrade two minor UNCONFIRMED items
to confirmed (see §14). All numeric coordinates below cite either the retail
Initialize disassembly or the snapshot `scroll.cpp` line that is byte-identical
to retail Initialize.

---

## §0 — Sources & status

**Panel class id + address.** `cls_0x5b5808` = `TBookPane`, vtable @ `0x005b5808`.
Inherits the 46-slot TScrollPane vtable (`cls_0x5b5750` @ `0x005b5750`) and
overrides slot 0 (Initialize) and slot 80 (DrawBackground); adds 1 new slot 184
(a paging-helper widget factory; deferred).

**Paint method addr.** `TBookPane::DrawBackground` @ `0x00521a20` (vtable slot
80; UNCONFIRMED body — see §14-#1).

**Initialize addr.** `TBookPane::Initialize` @ `0x005217e0` (vtable slot 0;
extracted).

**Recon files read:**
- `recon/discovered/port_status/TBookPane.md` (vtable + struct + open questions)
- `recon/discovered/port_status/TScrollPane.md` (parent struct + slot list)
- `recon/discovered/cls_0x5b5808_TBookPane_Initialize_5217e0.cpp` (retail Init body)
- `recon/ghidra/cls_0x5b5808.cpp` (TBookPane ctor + dtor + inherited Close +
  inherited KeyPress switch)
- `recon/ghidra/cls_0x5b5808__vftable_5b5808.cpp` (full 47-slot vtable layout)
- `recon/ghidra/cls_0x5b5750.cpp` (parent TScrollPane's DrawBackground body at
  `0x00521360` — analogous structure, used to corroborate the two-page text
  algorithm intent)
- `recon/discovered/cls_0x5b5750_TScrollPane_Initialize_521080.cpp` (parent
  Initialize; corroborates the same centring + 3-button pattern)
- `src/scroll.{h,cpp}` (the snapshot Cinematix 1998/99 sources — see Snapshot
  note below)
- `data/resources_unzipped/book.dat` measured via `tools/ui/dump_dat.py` (asset
  dims, §2)
- `data/resources_unzipped/font.def` lines 57-58 (Book font definition)
- `docs/ui/forensics/UI_METHOD_MAP.md` (referenced for §13 port mapping)
- `docs/ui/CLASSIC_HUD_REFERENCE.md` §3c (book/scroll reader visual context)

**Status:** forensics-complete for the visible deliverable. Two items remain
UNCONFIRMED (§14) and need the DrawBackground extract to close.

**Errors to close** (this is a NEW spec for an UNPORTED panel — there is no
existing port to fix; the engine code has only the snapshot `scroll.cpp` shell,
which the reconstruction agent should treat as a starting reference but verify
against this spec): n/a.

**Snapshot note (supplementary — superseded by retail).** The 1998/99 source
exists at `src/scroll.cpp:259-330` (`TBookPane::Initialize`, `BookBtnScrollUp/Down/Exit`,
`TBookPane::DrawBackground`) with the constants `BOOKLINES=16` and `BOOKWIDTH=216`
(`src/scroll.cpp:241-242`). For this panel the snapshot is *unusually* faithful
to retail — TBookPane::Initialize at `0x5217e0` matches snapshot Initialize
**literal-for-literal**:
- `(0x1e0 - bh)/3` (retail `:46`) == `(HEIGHT-bh)/3` (snapshot `:275`)
- `(0x280 - bw)/2` (retail `:47`) == `(WIDTH-bw)/2` (snapshot `:274`)
- down button `(x+0x1b8, y+0x182, 0x1c, 0x16, 0x22)` (retail `:61`) ==
  `(x+440, y+386, 28, 22, VK_NEXT)` (snapshot `:277`)
- up button `(x+0x1d8, y+0x17b, 0x1c, 0x17, 0x21)` (retail `:73`) ==
  `(x+472, y+379, 28, 23, VK_PRIOR)` (snapshot `:278`)
- exit button `(x+0x1f9, y+0x179, 0x1c, 0x1e, 0x1b)` (retail `:85`) ==
  `(x+505, y+377, 28, 30, VK_ESCAPE)` (snapshot `:279`)
- both load `"book.dat"` and look up `"book"` bitmap by name
- both load `"Book"` font (font.def `:58` BMFONT "Book" NORMAL "scrlfont")

The retail KeyPress (`0x00521530` at `cls_0x5b5808.cpp:87-148`) also matches
snapshot 1:1: switch on `0x23/0x24/0x26/0x28` (VK_END / VK_HOME / VK_UP /
VK_DOWN) with `±100000 / ±1` line deltas — matches snapshot `Scroll(±1)` /
`Scroll(±100000)` in `TScrollPane::KeyPress` `src/scroll.cpp:188-210`.

**This 100% Initialize+KeyPress match across all literals is unusually strong
evidence that the retail DrawBackground at `0x521a20` is also the same source
compiled** (the snapshot was branched right before ship). Use the snapshot
`TBookPane::DrawBackground` (`src/scroll.cpp:289-330`) as the authoritative
algorithm for §6/§12, **but mark the few specific magic numbers that cannot be
re-derived from the snapshot alone (lineHeight from "Book" font, the per-side
y-tilt of `+2*side`) as the things to confirm with the DrawBackground drop**
(§14). The snapshot is **not** used for asset names, coordinate spaces, or
visible behavior past `:330` — those come from the retail Initialize + parent
class DrawBackground decomp.

---

## §1 — Overview

`TBookPane` is the in-game **two-page book reader**. It is opened when the
player Uses (`TScroll::Use`, `src/scroll.cpp:72-86`) a TScroll game object
whose `GetTypeName() == "Book"`. A single shared instance (`BookPane`,
`src/scroll.cpp:22`) is installed on `PlayScreen` as an exclusive pane (modal
overlay across the whole game viewport).

It paints, on top of the game scene, a single 602×426 `book` bitmap (open
book with two visible facing pages, see §2), reads the bound `TScroll`'s
text, and lays the text out as **two side-by-side columns of 16 lines each
(BOOKLINES=16, BOOKWIDTH=216 px wide per column)**, with a small centred
page-number ("1", "2", "3", …) above each column. Three buttons live in the
bottom-right of the chrome: a left-page Prev (PgUp), a left-page Next (PgDn),
and Exit (ESC).

It is shown when the player Uses a Book-type TScroll
(`TScroll::Use:74-83`); it is hidden when the user clicks Exit, presses Esc,
or the pane is otherwise closed (`Close` slot 4 @ `0x005212c0`,
`cls_0x5b5808.cpp:67-81`).

Two visual instances? **No — single instance** (`BookPane`,
`src/scroll.cpp:22`). There is no player-vs-target mirroring (the panel is
modal and fills the screen).

---

## §2 — Asset roster

Measured with `python3 tools/ui/dump_dat.py
data/resources_unzipped/book.dat`. All bitmaps are 15-bit (`flags=0x2` =
`BM_15BIT`) and use the chroma-key set in their header.

| asset    | archive  | entry    | WxH (measured) | role                                 | source rects | flags / kc        | cite                                                  |
|---       |---       |---       |---             |---                                   |---           |---                |---                                                    |
| book     | book.dat | `book`   | 602×426        | open-book chrome (two facing pages)  | full         | flags=0x2 kc=0x7c1f (magenta) | `dump_dat.py` output; loaded via `Bitmap("book")` (retail Init `:50`, snapshot `:273`) |
| upup     | book.dat | `upup`   | 28×23          | PgUp button — released               | full         | flags=0x2 kc=0x0 (alpha)      | `dump_dat.py`; retail Init `:72` `s_updown`/`DAT_005e2d04` |
| updown   | book.dat | `updown` | 28×23          | PgUp button — pressed                | full         | flags=0x2 kc=0x0              | `dump_dat.py`; retail Init `:63` `s_updown_005e2d0c` |
| downup   | book.dat | `downup` | 28×22          | PgDn button — released               | full         | flags=0x2 kc=0x0              | `dump_dat.py`; retail Init `:60` `s_downup_005e2ce8` |
| downdown | book.dat | `downdown`| 28×22         | PgDn button — pressed                | full         | flags=0x2 kc=0x0              | `dump_dat.py`; retail Init `:51` `s_downdown_005e2cf0` |
| exitup   | book.dat | `exitup` | 28×30          | Exit button — released               | full         | flags=0x2 kc=0x0              | `dump_dat.py`; retail Init `:84` `s_exitup_005e2d18` |
| exitdown | book.dat | `exitdown`| 28×30         | Exit button — pressed                | full         | flags=0x2 kc=0x0              | `dump_dat.py`; retail Init `:75` `s_exitdown_005e2d20` |
| Book font| -        | (BMFONT)  | (atlas)       | bitmap font for text + page #        | per-glyph    | n/a               | `font.def:58` `BMFONT "Book" NORMAL "scrlfont" LEXTRA 4` |

**Notes on the chrome:** The `book` bitmap uses **magenta colorkey
(`kc=0x7c1f`)**. The corner pixels of the bitmap (outside the open book's
parchment shape) are magenta and will be keyed transparent against the
underlying game scene. The four buttons have `kc=0x0` and rely on real alpha
(see UI_METHOD_MAP §16 — alpha-not-key sprites).

**Font:** `BMFONT "Book" NORMAL "scrlfont" LEXTRA 4` — the same `scrlfont`
bitmap font used by TScrollPane's "Scroll" font (font.def `:57-58`),
4 extra px between lines.

There is no atlas / source-rect map — each asset is one full bitmap.

---

## §3 — Coordinate frames & surfaces

### Pane geometry

The pane occupies the full 640×480 game viewport
(`TBookPane::TBookPane` @ `0x00521e60`,
`recon/ghidra/cls_0x5b5808.cpp:154-174`):
- `this->mbr_0xc = 0x280 = 640` (width)
- `this->mbr_0x10 = 0x1e0 = 480` (height)
- `this->mbr_0x1c = 0x280` / `this->mbr_0x20 = 0x1e0` (clip w / h)
- position `(this->mbr_0x4 = 0, this->mbr_0x8 = 0)`

So the pane = full game viewport `(0, 0, 640, 480)`. (Classic-mode game
viewport is fixed at 640×480; per CLASSIC_HUD_REFERENCE the book reader
takes over the whole viewport as a modal overlay — `:124-128`.)

There is no "live width" `pane_w` concern for this panel — the pane is
**always** 640×480 in Classic mode; the chip model used for the side HUD
panels does not apply.

### Frame table (mandatory)

| frame            | parent       | anchor | origin in parent (formula + literal)                                  | what is expressed in it                          | cite                                                                                  |
|---               |---           |---     |---                                                                    |---                                               |---                                                                                    |
| **screen**       | (root)       | TL     | (0, 0)                                                                | the framebuffer                                  | (root)                                                                                |
| **pane-local**   | screen       | TL     | (0, 0) — pane occupies full screen                                    | the TBookPane's own rect (640×480)               | TBookPane ctor `mbr_0x4/0x8/0xc/0x10` `cls_0x5b5808.cpp:154-174`                       |
| **chrome-local** | pane-local   | center-ish | `x0 = (0x280 − bw)/2 = (640−602)/2 = 19`,   `y0 = (0x1e0 − bh)/3 = (480−426)/3 = 18` | the 602×426 `book` chrome bitmap and everything drawn relative to it (buttons, text) | retail Init `:46-47`; snapshot `scroll.cpp:274-275`; bw,bh from `dump_dat.py`           |
| **L-page-cell**  | chrome-local | TL     | `(0x52, 0x28) = (82, 40)` — the left page's text top-left              | the LEFT page's wrapped body text (BOOKLINES rows × BOOKWIDTH=216 wide) | snapshot `scroll.cpp:307` `x+82+(side*232), y+40+(side*2)` with `side=0`              |
| **R-page-cell**  | chrome-local | TL     | `(0x52 + 0xe8, 0x28 + 2) = (314, 42)` — the right page's text top-left | the RIGHT page's wrapped body text               | snapshot `scroll.cpp:307` with `side=1` (note the +2 y-tilt for the right side)        |
| **L-page-#-cell**| chrome-local | TL     | `(0xb4, 0x15) = (180, 21)` (CENTRED reference, see §8 below)           | the LEFT page-number string ("1", "3", "5", …)   | snapshot `scroll.cpp:305` `x+180+(side*232), y+21+(side*2)`                            |
| **R-page-#-cell**| chrome-local | TL     | `(0xb4 + 0xe8, 0x15 + 2) = (412, 23)`                                  | the RIGHT page-number string ("2", "4", "6", …)  | snapshot `scroll.cpp:305` `side=1`                                                    |
| **down-btn**     | chrome-local | TL     | `(0x1b8, 0x182) = (440, 386)`                                          | the PgDn (next page) button, 28×22               | retail Init `:61`; snapshot `:277`                                                    |
| **up-btn**       | chrome-local | TL     | `(0x1d8, 0x17b) = (472, 379)`                                          | the PgUp (prev page) button, 28×23               | retail Init `:73`; snapshot `:278`                                                    |
| **exit-btn**     | chrome-local | TL     | `(0x1f9, 0x179) = (505, 377)`                                          | the Exit button, 28×30                           | retail Init `:85`; snapshot `:279`                                                    |

### Frame composition formula

For any element at `(ex, ey)` in chrome-local:
```
screen_x = pane_x + (640 − 602)/2 + ex = 0 + 19 + ex
screen_y = pane_y + (480 − 426)/3 + ey = 0 + 18 + ey
```
i.e. **`screen_(x,y) = (19 + ex, 18 + ey)`** under the standard 640×480
Classic viewport.

### Anchor verification (chrome inside pane)

The chrome is **not** TL- or TR-anchored — it is **centred horizontally**
(`x = (W − bw)/2`) and **anchored to the top third** vertically
(`y = (H − bh)/3`, NOT `/2`). Retail Init `:47` literally divides by 3, not
2 — confirmed `(0x1e0 - piVar4[1]) / 3`. This places the open book closer
to the top of the screen than vertical-centre, leaving more empty space
underneath (a deliberate visual choice — likely to leave room for the
mouse/cursor at the bottom of the screen).

For the 602×426 `book` bitmap on a 640×480 viewport: chrome origin =
**screen-local (19, 18)**.

### Scratch / mosaic surfaces

`TBookPane::DrawBackground` (snapshot `:289-330`) uses **no scratch surfaces
of its own** — it composes directly into `Display` via `Display.Put` and
`Display.WriteText`. The buttons own their own draw surfaces internally
(handled by `TButtonPane::DrawBackground` via the `RedrawButtons` call,
`:324`), but those are invisible to this panel's coordinate space.

The TBookPane DrawBackground is gated by `IsDirty()` (`:291`) and only
re-composes when the dirty flag is set (e.g. on page change or initial
display). This is the standard TPane-family redraw model.

### Layout diagram

```
screen 640x480
+──────────────────────────────────────────────────────────────+
│                                                              │
│         chrome origin (19, 18) ── 602x426 "book" bitmap      │
│         ╔══════════════════════════════════════════╗         │
│         ║                                          ║         │
│         ║  page # "1"               page # "2"     ║         │
│         ║   centred                  centred       ║         │
│         ║   ~(180,21) chrome-local   ~(412,23) ⤴   ║         │
│         ║   ↑                        ↑             ║         │
│         ║   ┌─────────────────┐ ┌─────────────────┐║         │
│         ║   │ LEFT page text  │ │ RIGHT page text │║         │
│         ║   │ origin (82,40)  │ │ origin (314,42) │║         │
│         ║   │ wrap=216px      │ │ wrap=216px      │║         │
│         ║   │ 16 lines max    │ │ 16 lines max    │║         │
│         ║   │ (BOOKLINES=16)  │ │  (BOOKLINES=16) │║         │
│         ║   │                 │ │                 │║         │
│         ║   │                 │ │                 │║         │
│         ║   └─────────────────┘ └─────────────────┘║         │
│         ║                                          ║         │
│         ║                       [Pg│Pg│Esc]        ║         │
│         ║                       buttons             ║         │
│         ║                       (440,386)(472,379)  ║         │
│         ║                       (505,377)           ║         │
│         ╚══════════════════════════════════════════╝         │
│                                                              │
│                                                              │
+──────────────────────────────────────────────────────────────+
```

Note the **+2 y-tilt** of the RIGHT side: the right page text starts 2 px
lower than the left page (`y+40 vs y+42`) and the right page number is
2 px lower than the left (`y+21 vs y+23`). This is in the snapshot exactly
(`scroll.cpp:305,307` `... + (side*2)`) and the natural reading is that
the book chrome has a slight visible **fold/curve** that tilts the right
page down by 2 px — reproducing the bitmap's visual perspective.

---

## §4 — Static element layout (table)

All `(x, y)` are **chrome-local** unless noted; final screen coords are
`(19+x, 18+y)` (see §3 composition formula). There is no player/target
mirror — single instance, single column per row (the "two sides" here
are the two pages, not two mirrored panels).

| element            | space         | (x, y)           | (w, h)        | source rect      | cite                                                                          |
|---                 |---            |---               |---            |---               |---                                                                            |
| book chrome        | chrome-local  | (0, 0)           | (602, 426)    | full `book`      | snapshot `scroll.cpp:297` `Display.Put(x, y, bitmap, DM_TRANSPARENT\|DM_BACKGROUND)` |
| page-1 number cell | chrome-local  | (180, 21)        | (text-w, lineH) | n/a (text)     | snapshot `:305` `x+180, y+21, JUSTIFY_CENTER`                                  |
| page-2 number cell | chrome-local  | (412, 23)        | (text-w, lineH) | n/a (text)     | snapshot `:305` `x+180+232, y+21+2`                                            |
| LEFT page text     | chrome-local  | (82, 40)         | (216, 16*lineH) | n/a (text)    | snapshot `:307` `x+82, y+40, BOOKLINES=16, BOOKWIDTH=216`                       |
| RIGHT page text    | chrome-local  | (314, 42)        | (216, 16*lineH) | n/a (text)    | snapshot `:307` `x+82+232, y+40+2`                                             |
| down (PgDn) button | chrome-local  | (440, 386)       | (28, 22)      | full `downup` / `downdown` | retail Init `:61`; snapshot `:277`                                  |
| up (PgUp) button   | chrome-local  | (472, 379)       | (28, 23)      | full `upup` / `updown`     | retail Init `:73`; snapshot `:278`                                  |
| exit button        | chrome-local  | (505, 377)       | (28, 30)      | full `exitup` / `exitdown` | retail Init `:85`; snapshot `:279`                                  |

**Per-side composition arithmetic (showing the math `:305-307` encodes):**

- Page-text origin per side `s ∈ {0, 1}`:
  `(82 + 232*s, 40 + 2*s)` chrome-local
  → side 0: (82, 40), side 1: (314, 42)
- Page-# origin per side `s`:
  `(180 + 232*s, 21 + 2*s)` chrome-local, centred horizontally about that x
  → side 0: (180, 21), side 1: (412, 23)

The constant **232** is the page-pitch (horizontal distance between
matching points on the two pages). It is `0xe8 = 232`. Since each page text
is 216 wide, there is a **16 px gutter** between the two columns of text
(232 − 216 = 16).

The constant **2** is the right-side y-tilt — the right page is drawn 2 px
lower than the left, matching the visual perspective of the book chrome.

The number **180** is the centring reference for the left page number,
roughly the centre of the left page (page-text origin 82 + half page-width
216/2 = 82 + 108 = 190 — i.e. 180 is **10 px left of geometric centre**,
which suggests the page-# cell is laid out as a small region around 180
and centred there; combined with `JUSTIFY_CENTER`, the actual text
centre-anchor is `x = 180` chrome-local). See §8 for the cell width
UNCONFIRMED.

---

## §5 — Draw order / composition

Inferred from snapshot `TBookPane::DrawBackground` `scroll.cpp:289-330`, the
analogous parent `TScrollPane::DrawBackground` retail body
(`cls_0x5b5750.cpp:33-115`), and the standard TPane/TButtonPane redraw
contract. Steps run only when `IsDirty()` returns true (`:291`); the buttons
always re-draw last via the base call.

1. **Dirty check.** If `IsDirty() == false`, **skip steps 2-9**, then jump
   to step 10 (TButtonPane::DrawBackground).  (`scroll.cpp:291`)
2. **Resolve chrome bitmap and centring.** `bitmap = scrolldata->Bitmap("book")`;
   compute `x = (640 − bitmap->width)/2 = 19` and
   `y = (480 − bitmap->height)/3 = 18`. (`scroll.cpp:293-296` ↔ retail Init
   `:46-47`)
3. **Blit chrome.** `Display.Put(x, y, bitmap, DM_TRANSPARENT | DM_BACKGROUND)`
   — opaque-with-key copy onto the Display surface, marked as background
   (so the rest of the frame composes over it). Magenta key (`0x7c1f`)
   transparent. (`scroll.cpp:297`)
4. **Text bail-out.** If `scroll == nullptr` or `scroll->GetText() == nullptr`,
   skip steps 5-7. (`scroll.cpp:299`)
5. **Left-page composition (side=0).**
   - 5a. Format page number: `sprintf(buf, "%d", (line / BOOKLINES) + 1)`
     where `BOOKLINES=16`. (`scroll.cpp:304` with `side=0`)
   - 5b. Draw page number: `Display.WriteText(buf, x+180, y+21, numlines=1,
     scrollfont, color=nullptr, DM_USEDEFAULT, wrapwidth=-1, startline=0,
     JUSTIFY_CENTER)`. (`scroll.cpp:305`)
   - 5c. Draw left-page body: `Display.WriteText(scroll->GetText(), x+82,
     y+40, BOOKLINES=16, scrollfont, color=nullptr, DM_USEDEFAULT,
     BOOKWIDTH=216, startline=line)`. (`scroll.cpp:307`)
6. **Right-page composition (side=1).**
   - 6a. Format page number: `sprintf(buf, "%d", (line / BOOKLINES) + 2)`.
   - 6b. Draw page number: `Display.WriteText(buf, x+412, y+23, 1,
     scrollfont, nullptr, DM_USEDEFAULT, -1, 0, JUSTIFY_CENTER)`.
   - 6c. Draw right-page body: `Display.WriteText(scroll->GetText(), x+314,
     y+42, 16, scrollfont, nullptr, DM_USEDEFAULT, 216, line + 16)`.
7. **Button visibility logic.**
   - If `line < 1` → `Button(1)->Hide()` (the PgUp / up button; see
     §10 for button index map) else `Button(1)->Show()`. (`scroll.cpp:312-315`)
   - If `(line + BOOKLINES*2 − 1) >= numlines` → `Button(0)->Hide()`
     (PgDn / down) else `Button(0)->Show()`. (`scroll.cpp:317-320`)
   - `Button(2)->Show()` (Exit always shown). (`scroll.cpp:322`)
8. **Redraw the visible buttons** into the chrome via `RedrawButtons()`.
   (`scroll.cpp:324`)
9. **Clear dirty.** `SetDirty(false)`. (`scroll.cpp:325`)
10. **Buttons last.** Call `TButtonPane::DrawBackground()` (the base
    TButtonPane vtable slot 80 body) which actually paints the button
    sprites on top of the chrome at their registered chrome-local rects.
    (`scroll.cpp:329`)

The button base-class blit-pass at step 10 happens **every frame**, not
only on dirty — which is how button hover/pressed feedback animates without
a parent invalidate. The pane's own chrome + text only repaints on dirty.

### Pass / slot structure

There is only one paint pass at the TBookPane level: vtable **slot 80**
`DrawBackground` @ `0x521a20`. There is no separate Draw / Update split.
The Update behaviour (advance `line` on key/button) lives in `KeyPress`
(slot 108, `0x521530`, `cls_0x5b5808.cpp:87-148`) and the button callbacks
`BookBtnScrollUp / Down / Exit` (snapshot `:244-257`; the LAB_005217*
labels in retail Init are the corresponding retail function pointers).

---

## §6 — Algorithms (pseudocode per helper)

### `TBookPane::DrawBackground` (slot 80, `0x00521a20`)

Pseudocode based on snapshot `scroll.cpp:289-330` (see §0 Snapshot note for
the equivalence argument). `Display` is the global display surface;
`scroll` is `this->mbr_0x17c` (the bound TScroll); `scrolldata` is
`this->mbr_0x180` (the book.dat TMulti); `scrollfont` is the "Book"
TFont; `line` is `this->mbr_0x188`; `numlines` is `this->mbr_0x18c`.

```
DrawBackground():
    if not IsDirty():
        goto DRAW_BUTTONS
    book = scrolldata->Bitmap("book")             # 602x426 chrome
    chrome_x = (640 - book.width) / 2             # = 19
    chrome_y = (480 - book.height) / 3            # = 18
    Display.Put(chrome_x, chrome_y, book,
                DM_TRANSPARENT | DM_BACKGROUND)
    if scroll != null and scroll.GetText() != null:
        for side in [0, 1]:
            page_num = (line / 16) + side + 1     # BOOKLINES = 16
            sprintf(buf, "%d", page_num)
            Display.WriteText(buf,
                              chrome_x + 180 + side*232,
                              chrome_y +  21 + side*  2,
                              numlines=1,
                              font=scrollfont,
                              color=null,
                              drawmode=DM_USEDEFAULT,
                              wrapwidth=-1,
                              startline=0,
                              justify=JUSTIFY_CENTER)
            Display.WriteText(scroll.GetText(),
                              chrome_x +  82 + side*232,
                              chrome_y +  40 + side*  2,
                              numlines=16,                # BOOKLINES
                              font=scrollfont,
                              color=null,
                              drawmode=DM_USEDEFAULT,
                              wrapwidth=216,              # BOOKWIDTH
                              startline=line + side*16)
    # Button visibility (button index: 0=down, 1=up, 2=exit; see §10)
    if line < 1:        Button(1).Hide() else Button(1).Show()
    if (line + 16*2 - 1) >= numlines:
                        Button(0).Hide() else Button(0).Show()
    Button(2).Show()
    RedrawButtons()
    SetDirty(false)
DRAW_BUTTONS:
    TButtonPane::DrawBackground()   # paints visible buttons over chrome
```

### `TBookPane::Initialize` (slot 0, `0x005217e0`)

Pseudocode from `recon/discovered/cls_0x5b5808_TBookPane_Initialize_5217e0.cpp`.
The struct field offsets `400 / 0x188 / 0x18c / 0x194 / 0x180` are
documented on `port_status/TBookPane.md`. `mbr_0x190` is the font handle
(see port_status struct table). `mbr_0x194` here is set to `300` (decimal),
which corresponds to BOOKWIDTH or a per-line wrap pixel budget hint —
**UNCONFIRMED (different from BOOKWIDTH=216 used in DrawBackground)** —
see §14-#3.

```
Initialize():
    TButtonPane::Initialize()                                  # base init
    this->mbr_0x190 = 0xd8           # = 216  (likely the per-line wrap width
                                     # used for SetScroll's numlines compute —
                                     # NOTE: 0xd8 = 216 = BOOKWIDTH, NOT 300)
    # CORRECTION on first read: the recon literal at :21 is *(this+400) = 0xd8
    # where 400 = 0x190. So mbr_0x190 = 0xd8 = 216 = BOOKWIDTH. The Ghidra var
    # `*(undefined4 *)(param_1 + 400)` and `*(undefined4 *)(param_1 + 0x194) = 300`
    # are two different fields. See port_status struct table.
    this->mbr_0x188 = 0              # line = 0
    this->mbr_0x18c = 0              # numlines = 0
    this->mbr_0x194 = 300            # per-text-wrap or row-budget hint
                                     #   (UNCONFIRMED — see §14-#3)
    this->mbr_0x180 = LoadMulti("book.dat")        # scrolldata = book.dat
    if scroll != null and scroll->mbr_0xd8 != null:
        this->mbr_0x18c = FindNumLinesInText(
                              text     = scroll->mbr_0xd8,    # GetText()
                              font     = global_BookFont_DAT_0065c5c0,
                              wrapw    = this->mbr_0x190 (= 216),
                              startline= 1, 0,
                              maxlines = 10000,
                              ?        = 0)                   # → numlines
    # Recompute per-line metric for the page-#-divisor cache (DAT_0066dac8)
    glyph_w = font_height_table[DAT_0065b7d8]
    DAT_0066dac8 = 300 / (font_table_glyph_w + font_size_field)
                                                # likely a chars-per-page cache
                                                # used by KeyPress paging math
    if scrolldata != null:
        book   = scrolldata->Bitmap("book")     # for centring
        chx = (640 - book.width)  / 2
        chy = (480 - book.height) / 3
        NewButton("down", chx + 440, chy + 386, 28, 22, VK_NEXT (0x22),
                  BookBtnScrollDown,
                  scrolldata->Bitmap("downdown"),
                  scrolldata->Bitmap("downup"))
        NewButton("up",   chx + 472, chy + 379, 28, 23, VK_PRIOR (0x21),
                  BookBtnScrollUp,
                  scrolldata->Bitmap("updown"),
                  scrolldata->Bitmap("upup"))
        NewButton("exit", chx + 505, chy + 377, 28, 30, VK_ESCAPE (0x1B),
                  BookBtnExit,
                  scrolldata->Bitmap("exitdown"),
                  scrolldata->Bitmap("exitup"))
        return 1
    return 0
```

**Notes on Initialize:**
- The 3 `NewButton` calls register the buttons in **chrome-local +
  chrome-origin = pane-local screen** coordinates (chx/chy are added in by
  Init). Button rects are therefore registered against the pane in absolute
  pane coords — NOT in chrome-local. The §3/§4 chrome-local entries
  describe the visual position; the actual hit rect stored by the button
  base class is `(chx+ex, chy+ey, w, h)` pane-local.
- The 14-arg `FUN_00444e30` (retail) is the retail NewButton constructor
  (cf. `recon/ghidra/cls_0x5b5808.cpp:31-62` for the wrapper) — it allocates
  a 0x148-byte button object (cls_0x5a3c68) and registers it on the pane.

### `TBookPane::KeyPress` (slot 108, `0x00521530`) — inherited from TScrollPane

Pseudocode from `recon/ghidra/cls_0x5b5808.cpp:87-148`. The switch is on the
4 arrow / page-jump keys with delta lines:

```
KeyPress(key, downflag):
    TButtonPane::KeyPress(key, downflag)
    if not downflag: return
    switch key:
        0x23 (VK_END):   delta = +100000
        0x24 (VK_HOME):  delta = -100000
        0x26 (VK_UP):    delta = -1
        0x28 (VK_DOWN):  delta = +1
        default: return
    oldline = this->line
    this->line = clamp(this->line + delta, 0, this->numlines)
    if oldline != this->line:
        vtable[slot 44].FUN_00444fc0(this)   # SetDirty / invalidate
```

The slot-44 callback (`FUN_00444fc0`) is the inherited "mark this pane
dirty" / `SetDirty(true)`-equivalent for the TPane family. UNCONFIRMED in
detail — but the snapshot `Scroll(numscrolllines)` does `SetDirty(true)`
when `oldline != line`, matching the retail conditional invoke at
`:107,118,130,142`.

**Page paging via PgUp/PgDn buttons** (NOT `KeyPress`):

- `BookBtnScrollDown` calls `BookPane.Scroll(BOOKLINES * 2)` →
  `line += 32` (snapshot `:251`).
- `BookBtnScrollUp` calls `BookPane.Scroll(-(BOOKLINES * 2))` →
  `line -= 32` (snapshot `:246`).
- `BookBtnExit` calls `BookPane.Close()` (snapshot `:256`).

So a button-driven page turn is **32 lines**, which is exactly two
BOOKLINES-pages (one left + one right). Single-arrow KeyPress (slot 108)
moves a single line at a time (±1), and HOME/END are ±100000 (clamped to
[0, numlines]) — i.e. jump to start/end.

### `TBookPane` ctor (`0x00521e60`) — minor

Pseudocode from `cls_0x5b5808.cpp:154-174`:
```
TBookPane::TBookPane():
    mbr_0xc = 0x280, mbr_0x10 = 0x1e0       # w, h = 640, 480
    mbr_0x1c = 0x280, mbr_0x20 = 0x1e0      # cliprect w, h
    mbr_0x4 = mbr_0x8 = 0                   # x, y = 0
    mbr_0x14 = mbr_0x18 = 0                 # cliprect x, y = 0
    mbr_0x48 = mbr_0x4c = mbr_0x50 = mbr_0x54 = 0      # base fields
    cls_0x41c7f0(this+0x88, 0x10)            # init button-list container (cap 16)
    mbr_0x17c = 0                            # scroll = nullptr
    vftptr = &cls_0x5b5808__vftable_5b5808
```

### `TBookPane::Close` (slot 4, `0x005212c0`) — inherited

Pseudocode from `cls_0x5b5808.cpp:67-81`:
```
Close():
    TButtonPane::Close()
    if mbr_0x180 != 0:                    # free book.dat
        free(mbr_0x180)
    DAT_0065caf0.<release_exclusive>()    # PlayScreen.ReleaseExclusivePane(this)
    DAT_0065caf0.<remove_pane>()          # PlayScreen.RemovePane(this)
    DAT_0065caf0.<redraw>()               # PlayScreen.Redraw()
    mbr_0x17c = 0                         # scroll = nullptr
    UNK_0065cb3c._4_4_ = 1                # "pane closed" / cursor flag
```

### `meth_0x521fb0` (slot 184 — new TBookPane-only slot) — DEFERRED

Allocates a 0xd8-byte object of type `cls_0x5a50e8` (a TScreen-derived class),
sets its vftable, and returns it. Not called from DrawBackground / Init /
KeyPress in any extracted body. Likely a paging-state cache or per-spread
sub-widget factory — **out of scope** for this spec (the visible behavior
is fully captured by Initialize + DrawBackground + KeyPress). Open question
recorded in `port_status/TBookPane.md` and §14-#2.

---

## §7 — Effects & shadows

**Drop shadow:** There is **no drop shadow on the chrome** — `Display.Put`
with `DM_TRANSPARENT | DM_BACKGROUND` (snapshot `:297`) is opaque-with-key.
The chrome bitmap itself contains its own visible shadow/depth painted in
art.

**Text shadow:** `Display.WriteText(... DM_USEDEFAULT ...)` for both
the page-number and body-text writes (snapshot `:305, 307`). The "Book"
font is `BMFONT "Book" NORMAL "scrlfont" LEXTRA 4` (font.def `:58`) —
**no SHADOW field** — so the BMFONT bitmap font path **does not** invoke
the 3-pass shadow logic of `FUN_004be2b0` (which is a WINFONT/GDI-text
path). Bitmap fonts blit each glyph as-is; the "Book" font glyphs are
pre-rendered with whatever shadow / outline the artist painted into the
`scrlfont` bitmap. (Confirms: there is **no 3-pass black shadow** in this
panel, and no pink-halo bug risk.)

**Chroma key:** the `book` bitmap uses `kc=0x7c1f` (magenta in RGB555 — the
canonical Revenant chroma key, UI_METHOD_MAP §16). The 6 button sprites use
`kc=0x0` and rely on **real alpha** (BM_15BIT + alpha plane in the bitmap),
matching the convention noted in UI_METHOD_MAP §16 for sprites that "carry
alpha, not magenta key — do not apply the global magenta key or you punch
holes in black pixels". Reconstruction must respect each asset's per-bitmap
key vs alpha mode (the bitmap loader already does this — see [[feedback-ui-no-mock-use-retail]]).

**Background marking:** `DM_BACKGROUND` (0x10) on the chrome put tells the
display this is a static backdrop layer, so the display can skip
re-clearing the rect when only foreground (buttons / text) changes. The
text and buttons are drawn over it each Redraw without `DM_BACKGROUND`.

---

## §8 — Text rendering (table)

All cells are **chrome-local**; final screen `(x, y) = (19 + x, 18 + y)`.
There are 4 text writes per dirty paint (2 page numbers + 2 body columns)
when `scroll && scroll->GetText()`; 0 when no text.

| string                                 | cell (space, x, y, w, h)                          | font           | px      | color          | h-align       | v-align | shadow | format string | source value                                  | cite                                                |
|---                                     |---                                                |---             |---      |---             |---            |---      |---     |---            |---                                            |---                                                  |
| Left page number ("1", "3", "5", …)    | chrome-local, **anchor centre x=180, y=21**, w/h = single-line text bounds (UNCONFIRMED cell-width — see §14-#4) | Book (`scrlfont`) | bitmap-font (per-glyph from atlas)  | font-default (white-cream — see notes) | **center** (JUSTIFY_CENTER bit `1<<1`) | top (default — no vert-centre flag) | none (BMFONT) | `"%d"` | `(line / 16) + 1`                              | snapshot `scroll.cpp:304-305`                        |
| Right page number ("2", "4", "6", …)   | chrome-local, anchor centre x=412, y=23           | Book           | bitmap   | font-default   | center         | top     | none    | `"%d"`        | `(line / 16) + 2`                              | snapshot `scroll.cpp:304-305` `side=1`              |
| Left page body text                    | chrome-local, **TL (82, 40)**, wrapwidth=216, lines=16 | Book      | bitmap   | font-default   | left (JUSTIFY_LEFT default) | top | none | (raw text — no format) | `scroll->GetText()`, startline=`line`           | snapshot `scroll.cpp:307` `side=0`                  |
| Right page body text                   | chrome-local, **TL (314, 42)**, wrapwidth=216, lines=16 | Book      | bitmap   | font-default   | left           | top     | none    | (raw text)    | `scroll->GetText()`, startline=`line + 16`      | snapshot `scroll.cpp:307` `side=1`                  |

**Notes:**
- **Color:** `WriteText` is called with `color=nullptr` (snapshot `:305,307`)
  → use the font's default color. For BMFONT "Book", no `COLOR` is set in
  font.def (line 58) → the glyphs are blitted as their stored RGB. The
  scrlfont bitmap is artist-coloured (likely a cream / brown ink to match
  the parchment look) — UNCONFIRMED exact RGB (depends on the scrlfont
  atlas pixels), reconstruction loads the BMFONT and uses its stored
  glyph color.
- **h-align (page #):** `JUSTIFY_CENTER` is the `1 << 1` bit (revdefs.h
  `:361`). This sets the text origin x as the **centre anchor** — glyphs
  are blitted so that the rendered string's centre lies at (180, 21) /
  (412, 23). This is HORIZONTAL only; the y stays as the top of the
  glyph row.
- **h-align (body):** the 11th positional arg of `WriteText` defaults to
  `JUSTIFY_LEFT` (`src/surface.h:277`) — body text left-aligns at the cell
  origin and wraps at `wrapwidth=216`.
- **v-align:** for both, top (default — there is no DT_VCENTER analog set).
  Multi-line body stacks DOWN by lineHeight from `y=40` (left) or `y=42`
  (right). The 16-line cell is `16 * lineHeight` tall — total body cell
  height depends on the BMFONT row height (UNCONFIRMED in this spec — see
  §14-#5).
- **Page-# horizontal centring verification:** the left page-text cell
  spans chrome-local x = 82..298 (= 82+216), with geometric centre at
  x=190. The recon-given page-# centre anchor is x=180 — 10 px LEFT of the
  page's geometric centre. This is likely intentional (the page-# sits
  above the left margin of the body text, not above the visual centre of
  the page) but the cell-width to clip the centred text to is **not in
  the snapshot** (passed `wrapwidth=-1` per `:305`) — see §14-#4.
- **Format string:** `"%d"` — a plain decimal page index. There is no
  prefix/suffix. The pages are 1-indexed: line=0 → pages 1,2; line=32 →
  pages 3,4; etc. — `(line / 16) + 1` and `+ 2`.
- **Source value:**
  - Page #: computed from `this->line` (the scroll position, in lines).
  - Body text: `this->scroll->GetText()` — a raw `char*` from the TScroll
    object, set via `TScroll::SetText` and persisted via `TScroll::Load`
    (`src/scroll.cpp:24-32, 34-56`).

---

## §9 — Animation & dynamic behavior

There is **no animation** in this panel — no fades, no tweens, no hover
ramps. (The button hover/pressed swaps are handled by the inherited
TButtonPane base class; they aren't ramped — they are instant up/down
sprite swaps based on cursor and mouse state.)

### Dirty / redraw model

- The pane's `IsDirty()` flag is the only redraw gate for steps 2-9 of §5.
- Dirty becomes **true** when:
  - `Scroll(delta)` is called and `delta != 0` causes `line` to change
    (snapshot `:233-234`). This happens from the 3 button callbacks
    (`BookBtnScrollUp / Down / Exit`) and from `KeyPress` (slot 108) on
    arrow/HOME/END.
  - `SetScroll(s)` rebinds a new TScroll — UNCONFIRMED if SetScroll
    re-marks dirty (snapshot does NOT, but `IsDirty()` is true by default
    after `TButtonPane::Initialize()`).
  - Initial display after `Initialize()` (the default-dirty state inherited
    from the TPane base class).
- Dirty becomes **false** at the end of step 9 (`SetDirty(false)`).
- The buttons re-draw EVERY frame regardless of dirty, via step 10
  (`TButtonPane::DrawBackground()`).

### State machine — none

The panel has no phases or states beyond Open / Closed (Open = on
PlayScreen exclusive pane stack; Closed = removed). No
opening/closing animation; the pane appears/disappears instantly via
`PlayScreen.{AddPane/RemovePane}`.

### Scroll position state

- `this->line` (`mbr_0x188`) — the current top-line offset into the wrapped
  text. Always `[0, numlines]`.
- `this->numlines` (`mbr_0x18c`) — total wrapped lines in the text,
  computed once by `SetScroll` via `FindNumLinesInText(text, BOOKWIDTH)`.
- Mapping `line` → page: page-number on the **left** is `(line / 16) + 1`;
  on the right `+ 2`. So lines `0..15` show as page 1 (left) + 2 (right),
  lines `16..31` show as page 2 (left) + 3 (right) — NOTE: this means
  single-line VK_UP/VK_DOWN scrolling can produce **off-page-pair** views
  where the right page shows the continuation of a left page that
  partially scrolled off-top. This is **by design** (the snapshot says
  exactly this) and the button-driven Pg paging deliberately steps by 32
  lines to land on a clean even-page-pair boundary.

---

## §10 — Input & dispatch

### Hit rects

| control | space (pane-local) | (x, y)             | (w, h)  | command (callback)              | key       | cite                                                            |
|---      |---                 |---                 |---      |---                              |---        |---                                                              |
| down    | pane               | (chx + 440, chy + 386) = (19+440, 18+386) = **(459, 404)** | (28, 22) | `BookBtnScrollDown` (`Scroll(+32)`) | VK_NEXT (0x22) | retail Init `:61` `LAB_00521780`; snapshot `:244-247, 277` |
| up      | pane               | (chx + 472, chy + 379) = **(491, 397)** | (28, 23) | `BookBtnScrollUp` (`Scroll(-32)`)   | VK_PRIOR (0x21) | retail Init `:73` `LAB_00521730`; snapshot `:249-251, 278` |
| exit    | pane               | (chx + 505, chy + 377) = **(524, 395)** | (28, 30) | `BookBtnExit` (`Close()`)           | VK_ESCAPE (0x1B) | retail Init `:85` `LAB_005217d0`; snapshot `:254-257, 279` |

Button registration order is **0 = down, 1 = up, 2 = exit** (the order of
the 3 `NewButton` calls in Init `:61, 73, 85`). The DrawBackground
visibility logic refers to them as `Button(0)`, `Button(1)`, `Button(2)`
in that order (snapshot `:312-322`).

### Keyboard

Dispatched by `TBookPane::KeyPress` (slot 108 = `0x00521530`, inherited
from TScrollPane).

| key | action                                  | cite                                       |
|---  |---                                       |---                                         |
| VK_END (0x23)  | `line += 100000` (then clamped)  | `cls_0x5b5808.cpp:99-108`                  |
| VK_HOME (0x24) | `line -= 100000` (then clamped)  | `cls_0x5b5808.cpp:110-121`                 |
| VK_UP (0x26)   | `line -= 1`                      | `cls_0x5b5808.cpp:122-133`                 |
| VK_DOWN (0x28) | `line += 1`                      | `cls_0x5b5808.cpp:134-145`                 |

The three button-bound keys (VK_NEXT/PgDn, VK_PRIOR/PgUp, VK_ESCAPE) are
dispatched by the BASE button-key handler (TButtonPane's key handler
walks each button's registered key and invokes its callback). VK_NEXT
fires `BookBtnScrollDown` (Scroll +32), VK_PRIOR fires `BookBtnScrollUp`
(Scroll −32), VK_ESCAPE fires `BookBtnExit` (Close). So both individual
arrow-line scrolling AND page-flip scrolling are wired through one
single keyboard dispatch.

### Modal state

The panel is registered as an **exclusive** pane on `PlayScreen`
(`PlayScreen.SetExclusivePane(this, true)`, snapshot `:282`). This blocks
game-world input under it. There are no modal-state globals owned by
this pane.

### Dispatch path

The retail Init wires the three button callbacks as raw function pointers
(`LAB_00521780` / `LAB_00521730` / `LAB_005217d0`, retail Init `:61, 73, 85`).
The snapshot wires them as `BookBtnScrollDown / BookBtnScrollUp /
BookBtnExit`. These are leaf trampolines that just call `BookPane.Scroll(±32)`
or `BookPane.Close()` on the single global `BookPane` instance — there is
no command-id-based dispatch through PlayScreen for this pane.

---

## §11 — Retail bugs NOT to reproduce

I found **none in this panel's own paint path** that need calling out.

In particular:
- **No pink-halo risk:** the body text uses BMFONT (no GDI HDC composite,
  no magenta-cleared scratch → no AA-edge sample of magenta → no pink
  halo). UI_METHOD_MAP §5 pink-halo concerns apply only to WINFONT/GDI
  text paths.
- **No (+4,+4) shadow trap:** the chrome's `Display.Put` does not invoke
  the (dx,dy) drop-shadow registry, so the shadow-offset rule
  ([[project-retail-shadow-semantics]]) doesn't apply here.
- **No bar-render +4,+4 mis-offset:** no stat bars on this panel.

Potential gotchas the reconstruction agent should still note:
1. The **page-# h-centre at chrome-local x=180** is NOT the geometric
   centre of the left page text body (which would be x=190). Centring the
   page-# at x=190 would be visually wrong; copy x=180 verbatim.
2. The **right-side +2 y-tilt** is deliberate art-perspective, not a bug.
   Snapping the right page to the same y as the left will look
   imperceptibly wrong against the book chrome.
3. The 3 buttons use `BM_15BIT` + alpha (`kc=0x0`), NOT magenta key. The
   bitmap decoder must honour the per-bitmap key field, NOT apply the
   global magenta-key default ([[feedback-ui-no-mock-use-retail]] +
   UI_METHOD_MAP §16). The port's `bitmapdecode.cpp` already handles
   this; just don't override with a Spec-side "everything's magenta".

---

## §12 — Reconstruction pseudocode

End-to-end. The constants and frame composition come from §3 / §4 / §8;
this section is a clean implementer-facing walk-through. Use the port
primitives in §13.

```cpp
// === lifetime ===
bool TBookPane::Initialize() {
    TButtonPane::Initialize();
    line = 0; numlines = 0;
    scrolldata = TMulti::LoadMulti("book.dat");
    scrollfont = FontTable->Bitmap("Book");
    SetScroll(scroll);   // recompute numlines now that font is loaded
    if (!scrolldata) return false;

    PTBitmap book = scrolldata->Bitmap("book");           // 602x426
    const int32_t x0 = (640 - book->width)  / 2;          // = 19
    const int32_t y0 = (480 - book->height) / 3;          // = 18

    NewButton("down", x0 + 440, y0 + 386, 28, 22, VK_NEXT,   BookBtnScrollDown,
              scrolldata->Bitmap("downdown"), scrolldata->Bitmap("downup"));
    NewButton("up",   x0 + 472, y0 + 379, 28, 23, VK_PRIOR,  BookBtnScrollUp,
              scrolldata->Bitmap("updown"),   scrolldata->Bitmap("upup"));
    NewButton("exit", x0 + 505, y0 + 377, 28, 30, VK_ESCAPE, BookBtnExit,
              scrolldata->Bitmap("exitdown"), scrolldata->Bitmap("exitup"));

    PlayScreen.AddPane(this);
    PlayScreen.SetExclusivePane(this, true);
    return true;
}

void TBookPane::Close() {
    TButtonPane::Close();
    if (scrolldata) delete scrolldata;
    PlayScreen.ReleaseExclusivePane(this);
    PlayScreen.RemovePane(this);
    PlayScreen.Redraw();
    scroll = nullptr;
}

// === paint (slot 80) ===
void TBookPane::DrawBackground() {
    if (IsDirty()) {
        PTBitmap book = scrolldata->Bitmap("book");
        const int32_t x = (640 - book->width)  / 2;       // = 19
        const int32_t y = (480 - book->height) / 3;       // = 18
        Display.Put(x, y, book, DM_TRANSPARENT | DM_BACKGROUND);

        if (scroll && scroll->GetText()) {
            char buf[80];
            for (int32_t side = 0; side < 2; ++side) {
                sprintf(buf, "%d", (line / BOOKLINES) + side + 1);

                Display.WriteText(buf,
                    x + 180 + side*232,           // page-# centre x
                    y +  21 + side*  2,           // page-# top y
                    /*numlines*/  1,
                    /*font*/      scrollfont,
                    /*color*/     nullptr,
                    /*drawmode*/  DM_USEDEFAULT,
                    /*wrapwidth*/ -1,
                    /*startline*/ 0,
                    /*justify*/   JUSTIFY_CENTER);

                Display.WriteText(scroll->GetText(),
                    x +  82 + side*232,           // body TL x
                    y +  40 + side*  2,           // body TL y
                    /*numlines*/  BOOKLINES,       // = 16
                    /*font*/      scrollfont,
                    /*color*/     nullptr,
                    /*drawmode*/  DM_USEDEFAULT,
                    /*wrapwidth*/ BOOKWIDTH,       // = 216
                    /*startline*/ line + side*BOOKLINES);
            }
        }

        // Button visibility
        Button(1)->SetShown(line >= 1);
        Button(0)->SetShown((line + BOOKLINES*2 - 1) < numlines);
        Button(2)->Show();

        RedrawButtons();
        SetDirty(false);
    }
    TButtonPane::DrawBackground();
}

// === button callbacks (file-scope or member-pointers) ===
void BookBtnScrollUp()   { BookPane.Scroll(-(BOOKLINES * 2)); }   // -32
void BookBtnScrollDown() { BookPane.Scroll( BOOKLINES * 2); }     // +32
void BookBtnExit()       { BookPane.Close(); }

// === keyboard (slot 108; inherited from TScrollPane) ===
void TScrollPane::KeyPress(int32_t key, bool down) {
    TButtonPane::KeyPress(key, down);
    if (!down) return;
    int delta = 0;
    switch (key) {
        case VK_UP:    delta = -1;       break;
        case VK_DOWN:  delta = +1;       break;
        case VK_HOME:  delta = -100000;  break;
        case VK_END:   delta = +100000;  break;
        default: return;
    }
    Scroll(delta);
}

// === scroll position helper ===
void TScrollPane::Scroll(int32_t delta) {
    int32_t oldline = line;
    line = std::clamp(line + delta, 0, numlines);
    if (oldline != line) SetDirty(true);
}
```

`BOOKLINES = 16`, `BOOKWIDTH = 216`. WIDTH=640, HEIGHT=480.

---

## §13 — Port mapping notes

Per UI_METHOD_MAP §12 "canonical port primitives":

| spec call (this doc)                                                                      | port primitive(s)                                                                                          | home          |
|---                                                                                       |---                                                                                                          |---             |
| `Display.Put(x, y, book, DM_TRANSPARENT \| DM_BACKGROUND)`                                | `Renderer->DrawBitmapToTarget(book, x, y)` into the HUD render target — `DM_TRANSPARENT` flag is the bitmap's own chroma-key behaviour (already in the decoded TBitmap); `DM_BACKGROUND` is a no-op in the modern compose-then-blit pipeline (we don't have separate background/foreground layers; the swapchain composite handles overdraw). | renderer       |
| `Display.WriteText(buf, x, y, 1, font, null, DM_USEDEFAULT, -1, 0, JUSTIFY_CENTER)` (page #) | `font.cpp DrawTextToTarget(atlas, buf, cx=x, cy=y, cw=auto, ETextAlign::Center, glyphColor)` — for a BMFONT, the atlas is the pre-rendered glyph sheet from `BuildFontAtlas(TFont*)`. **Use the BMFONT path, NOT BuildTTFAtlas** (that's WINFONT / GDI only). | font           |
| `Display.WriteText(text, x, y, 16, font, null, DM_USEDEFAULT, BOOKWIDTH, startline)` (body) | `font.cpp DrawTextToTarget(atlas, text, cellX=x, cellY=y, wrapWidth=216, maxLines=16, startLine=startline, ETextAlign::Left, glyphColor)` — needs a **wrap-width + skip-N-lines + max-lines** variant on the existing `DrawTextToTarget`; this is the "page-of-text" call shape (also wanted by TScrollPane and TTextBar). **Needs new primitive: `DrawTextWrappedToTarget(atlas, text, x, y, wrapW, maxLines, startLine, align)`** if not already present. | font           |
| `NewButton(...)`                                                                          | reuse the existing `TButtonPane` button system from the port — already in place; just register at the pane-local coords above. | button (existing) |
| `TButtonPane::DrawBackground()` (step 10)                                                  | the base TButtonPane port handles per-button hover/pressed sprite swap + blit. Reuse the existing port code (TPlyrStatusBar / TSideTabsPane already share it). | button (existing) |
| keyboard: VK_UP/VK_DOWN/VK_HOME/VK_END dispatch                                            | already-existing TPane keyboard pipe — register `KeyPress(key, down)` on the pane. | screen (existing) |
| chroma-key vs alpha per-bitmap                                                            | the bitmap decoder (`src/bitmapdecode.cpp:59-66`) already honours per-bitmap key vs alpha; no spec action needed. | bitmap (existing) |

**Missing primitive (likely):** A wrapped-multi-line text draw with
`startLine` and `maxLines` (the BookPane body call shape). If the
`font.cpp` `DrawTextToTarget` family doesn't already accept those, this
panel + TScrollPane + TTextBar should drive its addition (see
`docs/ui/forensics/UI_METHOD_MAP.md` §12). The recon name in retail is
the surface `WriteText` method (NOT `FUN_004be2b0`, which is the GDI
WINFONT path); it ends up walking the BMFONT atlas glyph by glyph.

**No shadow primitive needed** for this panel — BMFONT text has no
3-pass shadow (see §7). Use the plain (non-`Shadowed`) text draw.

**Compose-to-target contract:** compose the chrome + text into the HUD
render target via `…ToTarget` calls, then `DrawSurface` the HUD RT to
the swapchain in the standard HUD pass (NOMENCLATURE §3 "direct-renderer
contract"). Do not mix `…ToTarget` and `…Swapchain` families in this
panel.

---

## §14 — UNCONFIRMED / open questions

1. **DrawBackground body at `0x00521a20` is not yet extracted into
   `recon/discovered/`.** The complete algorithm is inferred from the
   snapshot `TBookPane::DrawBackground` (`src/scroll.cpp:289-330`),
   which is anchored as 1:1-with-retail by the perfect
   `TBookPane::Initialize` literal match (see §0 Snapshot note). Impact
   if wrong: visual layout could differ in subtle ways
   (e.g. side-tilt, page-# centring, button visibility predicate).
   **Resolve:** `DecompileAddr.java 0x00521a20` (Ghidra) to drop the
   body to `recon/discovered/cls_0x5b5808_TBookPane_DrawBackground_521a20.cpp`,
   then diff against the snapshot algorithm. Expected outcome: exact
   match modulo Ghidra's variable naming.

2. **Slot 184 (`FUN_00521fb0`, `cls_0x5a50e8` factory) — purpose
   unknown.** Allocates a 0xd8-byte cls_0x5a50e8 object (TScreen-derived
   per the Ghidra dump). Possibly a per-page render-cache widget, or a
   spread-state object. Not called from any extracted body
   (Initialize / Close / KeyPress / ctor / dtor do not reference it).
   Impact if wrong: only matters if some retail caller (likely
   TPlayScreen / TScroll::Use) constructs a paging helper through this
   slot and the panel relies on it. **Resolve:** grep for callers
   (`grep -rln '0x521fb0\|521fb0' recon/ghidra/`) and decompile any caller.
   If unused except via the vtable, document as "vtable slot wired but
   no visible call site" and skip in the port.

3. **`Initialize :24` `*(this+0x194) = 300` — what is `mbr_0x194`?**
   The retail Init writes both `*(this+0x190) = 0xd8 = 216` (=
   BOOKWIDTH) and `*(this+0x194) = 300`. `mbr_0x190` matches the
   snapshot's `BOOKWIDTH` (passed as the SetScroll wrap-width to
   `FindNumLinesInText`, snapshot `:217` → retail line 37
   `FUN_004acb80(…, uVar1, …)` where uVar1 is loaded from
   `param_1 + 400 = mbr_0x190`). But `mbr_0x194 = 300` has no
   counterpart in snapshot src (the snapshot's `TScrollPane` declares
   only 5 named fields). Theories:
   - (a) A precomputed page-size budget (chars-per-line *
     lines-per-page), since 300 is later used as the dividend for the
     `DAT_0066dac8` cache (`:46` `DAT_0066dac8 = 300 /
     (glyph_w + linespace)`).
   - (b) A horizontal margin / gutter or chrome inset.
   - (c) A "max pixel width" passed to a paging helper that the
     snapshot omitted.
   Impact: this is a derived per-text metric, not a visible coordinate;
   getting it wrong likely causes paging granularity to be off by a
   small amount or PgUp/PgDn to jump 1 too many lines. The visible
   layout is unaffected. **Resolve:** extract `FUN_00521c60` (the
   "metric helper" called at `:45`) and the function pointed to by
   `DAT_0066dac8`'s consumer (likely paging math).

4. **Page-number cell width (the centre-anchor's clip extent).** The
   snapshot calls `Display.WriteText(..., wrapwidth=-1, ...,
   JUSTIFY_CENTER)`. With `wrapwidth=-1` the centring is technically
   "no wrap rectangle, centre around the (x, y) anchor". The exact
   cell width to clip / measure the centring against is NOT specified
   — the renderer measures the string width and places it about
   `x=180` / `x=412` chrome-local. **Resolve:** confirm that the
   port's `DrawTextToTarget` with `wrapwidth=-1 + JUSTIFY_CENTER`
   measures the string and centres about the supplied (x, y) — the
   correct interpretation. If the port's text helper instead requires
   an explicit cell-width for centring, take cell-w = 60 (matching the
   expected 1-2 digit page number rendered in scrlfont) and centre
   inside `(x-30, y, 60, lineHeight)`. Impact if wrong: the page #
   visually drifts a few pixels left/right.

5. **Body text line-height (BMFONT "Book").** `LEXTRA 4` (font.def `:58`)
   means add 4 px of extra line space; the per-glyph row height comes
   from the scrlfont bitmap atlas (not yet measured). The 16-line
   cell tall is `16 * (rowH + 4)` in chrome-local pixels —
   approximately fills the page from y=40 down. **Resolve:** dump
   `scrlfont.dat` via the font-load code (the dat is a TFont-archive
   not a plain TMulti, so `dump_dat.py` rejects it — see the failed
   call to `tools/ui/dump_dat.py scrlfont.dat`). Use the port's
   font loader (`src/font.cpp` + the existing test
   `--test=font`) to load "Book" and read `GetLineHeight()`. The
   reconstruction agent will compute this at runtime; the spec does
   not need a literal value, just the rule "stack DOWN from cell top
   by `rowH + LEXTRA(4)` per line" (NOMENCLATURE §2 v-align rule).

6. **`numlines` cap and overscroll.** `FindNumLinesInText` returns the
   wrapped line count of the entire text at `wrapwidth=BOOKWIDTH=216`.
   The KeyPress / Scroll clamp is `line = clamp(line, 0, numlines)` —
   note that this allows `line == numlines`, which means the displayed
   left page top is **past the last actual line** (an empty page). The
   visibility predicate at snapshot `:317` allows
   `Button(0)->Show()` only when `(line + 32 - 1) < numlines`, which
   prevents reaching that all-empty state via PgDn. But VK_DOWN
   (`+1`) can still produce a partial last-page-pair. This is the
   snapshot behavior; UNCONFIRMED if retail's clamp is identical
   (almost certainly yes — the inherited TScrollPane has the same
   clamp at retail `cls_0x5b5808.cpp:103, 114, 126, 138` `if (numlines
   < line) line = numlines`). Impact: visible at the document end —
   one extra line of empty page is reachable via arrow keys but not
   PgDn.

7. **Page-# color.** font.def `:58` declares no `COLOR` for "Book", so
   the BMFONT renders glyph pixels as stored. Whether the page-#
   ("1") matches the body text ink color (cream/brown) or differs
   (e.g. bolder) is determined by the bitmap font atlas itself.
   Not visible from the recon — only by inspecting the scrlfont atlas.
   Impact: cosmetic only; the renderer just blits the BMFONT
   glyphs and the artist chose the color. Not a port decision.

---

## Summary of UNCONFIRMED list (one-line each)

1. `DrawBackground` body at `0x521a20` not yet extracted (algorithm
   inferred from snapshot via perfect Initialize+KeyPress equivalence).
2. Slot 184 `FUN_00521fb0` purpose (likely a paging helper widget;
   deferred).
3. `mbr_0x194 = 300` meaning (paging metric; cosmetic-only impact).
4. Page-number cell width with `wrapwidth=-1 + JUSTIFY_CENTER`
   (renderer behavior; likely "measure string, centre on anchor").
5. BMFONT "Book" exact line-height (compute at runtime via font loader).
6. Overscroll-by-arrow behavior (snapshot allows; retail almost
   certainly same).
7. Page-# color (determined by scrlfont atlas; cosmetic).

None of these block reconstruction of the **visible layout** — items 1
and 2 are the only ones the reconstruction agent would care about,
and item 1 is high-confidence inferred from the snapshot equivalence.
The reconstruction agent should proceed with this spec, and the parent
agent should dispatch a Ghidra-extract pass on `0x521a20` and
`0x521fb0` as background work to upgrade items 1+2 to confirmed.
