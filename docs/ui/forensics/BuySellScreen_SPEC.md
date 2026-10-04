# TBuySellScreen — Reconstruction SPEC (Shop / Trade panel)

> Pixel-accurate forensic spec for the **shop / buy-sell** screen — the modal
> NPC-trade interface that lets the player buy items from a salesperson's wares
> (from script `BuySellAdd`) or sell items from their own inventory (script
> `BuySellAddbuyitem`), with prices computed per-item from the object class's
> `Value` stat and modified by mode / SaleType.
>
> Produced per [FORENSICS_PROTOCOL.md](FORENSICS_PROTOCOL.md). Terms per
> [NOMENCLATURE.md](NOMENCLATURE.md). Primitive behavior cited from
> [UI_METHOD_MAP.md](UI_METHOD_MAP.md).
>
> All paths relative to repo root
> `/Users/benjamincooley/projects/RevenantRevisited/worktrees/ui`.
> Every number names its coordinate space and cites `file:line` + `0xHEX = DEC`.

---

## §0 — Sources & status

**Status:** `forensics-partial`. The class identity, ctor pane geometry, the
asset roster (buysell.dat measured), all four button registrations + hotkeys,
the localization-tag → label resolution path, the cell record struct size, the
init-time scratch surface, the script-API command bindings (15 commands), the
modal-state field map, and the dispatch path (Up/Down/Activate/Exit + page-
selector 1/2/3) are all firmly pinned from extracted recon. **The paint body
`virt_meth_0x52f7d0_84` at `0x52f7d0` is NOT extracted in the snapshot recon**
(the vtable file lists the slot but no decompiled body exists in
`recon/classes/`, `recon/classes_original/`, `recon/ghidra/`, or
`recon/discovered/`). The string-XREF map at `_data.txt:113741..113773` proves
the paint code's literal sequence (BuySellMain chrome, "GOLD" prefix, BSGOLD,
"%s %d" cell-text format, BSBUY/BSSELL/BSEXIT button labels) and that's the
authoritative trace for the order of calls — but the **exact cell rect, grid
pitch, and label rect literals** must be decoded from disassembly (§14-A). The
**price-compute helper `FUN_0052da90`** body is also missing from `recon/`
(only XREFs of its constant pool are present at `_data.txt:113053..113195+`),
proving it is a large per-class table-driven function (§14-B). UNCONFIRMED
items each carry a precise resolve step (§14). No engine code written.

**Class identity:** `TBuySellScreen = cls_0x5a5d64` (vftable @ `0x5a5d64`,
`recon/classes/cls_0x5a5d64.cpp:73`). TPane-derived (parent ctor calls
`cls_0x5a4494_TPane::virt_meth_0x434e40` at `cls_0x5a5d64.cpp:164`); inherits
through TButtonPane (`cls_0x5b98b8::meth_0x436790` adds the buttons at
`:187,196,205,214`). Instance size **440 bytes** (`cls_0x5a5d64.cpp:37`
`Length: 440`).

**Key method addresses (vtable at `0x5a5d64`,
`recon/classes/cls_0x5a5d64__vftable_5a5d64.cpp`):**

| slot | role | addr | recon |
|---|---|---|---|
| 0 | `Initialize` | `0x52f390` | `cls_0x5a5d64.cpp:141-240` |
| 4 | `Close` / cleanup | `0x52f6f0` | `cls_0x5a5d64.cpp:244-272` |
| 52 | `Reset` / Clear-wares | `0x530600` | `cls_0x5a5d64.cpp:386-410` |
| 80 | `Show` / DrawBackground wrapper | `0x52f7a0` | `cls_0x5a5d64.cpp:276-286` |
| **84** | **Paint** (`DrawGrid` / `Draw`) | **`0x52f7d0`** | **NOT EXTRACTED** — confirmed via XREF chain at `_data.txt:113741..113773`. §14-A |
| 100 | `MouseClick` / hit-test | `0x52fd50` | `cls_0x5a5d64.cpp:333-382` |
| 108 | `KeyPress` / nav | `0x52fa60` | `cls_0x5a5d64.cpp:290-329` |
| 124 | dtor wrapper | `0x489070` | `cls_0x5a5d64.cpp:120-137` |

**Sub-classes (sibling panes that share the cell-record format):**

- `cls_0x530af0` — **inventory-grid sub-pane** (the player-inventory column in
  Sell mode; iterates 8 player-inventory `TObjectClass` tables —
  `DAT_0065a14c` weapons, `DAT_0065a150` armor, etc. — and adds each matching
  item via `FUN_0052da90`). Class is 384 bytes (`cls_0x530af0.cpp:9`). Main
  method `meth_0x530af0` at `0x530af0` (`cls_0x530af0.cpp:13-667`).
- `cls_0x531fc0` — **wares-list sub-pane** (the salesperson's wares-for-sale
  column in Buy mode; iterates a linked list off `mbr_0x1b4 = waresHead`,
  filters by SaleType stat). 440 bytes (`cls_0x531fc0.cpp:13`). Method
  `meth_0x531fc0` at `0x531fc0`.
- `cls_0x531b70` — **wares-list-variant** with identical iteration shape, but
  passes the cell scratch buffer as the first arg to `FUN_0052da90` (the
  store-then-copy variant). 440 bytes (`recon/ghidra/cls_0x531b70.cpp:13`).
  Used in one buy/sell mode permutation (§14-G).

**Recon files read (authoritative):**

- `recon/classes/cls_0x5a5d64.cpp` (full ctor + Init + Close + Reset + nav +
  hit-test bodies; vtable layout dump).
- `recon/classes/cls_0x5a5d64__vftable_5a5d64.cpp` (46-slot vtable; confirms
  slot 84 is `0x52f7d0`).
- `recon/classes/cls_0x530af0.cpp` (the 8-class-table inventory-grid sub-pane
  body; 667 lines, complete).
- `recon/classes/cls_0x531fc0.cpp` (the SaleType-filtered wares sub-pane body;
  complete).
- `recon/ghidra/cls_0x531b70.cpp` (the variant sub-pane body; complete).
- `recon/classes/_data.txt:113716..113775` — full string-XREF map for the paint
  method (10 string anchors, each citing the call address in
  `virt_meth_0x52f7d0`). This is the load-bearing evidence for the paint
  sequence.
- `recon/classes/_data.txt:113053..113195+` — the s_Value/BSARM*/BSWEA*/Protection/
  Damage/Min Strength/etc. XREF table for `FUN_0052da90` — proves the
  function's body is a large per-class-tier dispatch returning a `(label,
  bonus)` tuple per item.
- `recon/classes/_data.txt:59040..59326` — the 15 script-API command bindings
  (buyselladd/buyselladdbuyitem/buyselladdbuycriteria/buysellinit/...).
- `data/Modules/Demo/demo.s:155-208` — the canonical script-API usage example
  (the Elahni shop in the demo).
- `data/resources_unzipped/english.def` (BS-prefix tag table) — confirms
  BSGOLD/BSBUY/BSSELL/BSEXIT/BSGP/BSWEA1/BSARM1/etc. are **localization tag
  IDs**, NOT bitmap names.
- `tools/ui/dump_dat.py buysell.dat` — measured asset dimensions (9 entries).

**Prior briefs consulted:** none — this is the user-confirmed missing **32nd
panel** spec.

**Errors to close:** none — no `src/` port shell for TBuySellScreen.

**Snapshot note (supplementary — superseded by retail).** A snapshot
`src/buysell.cpp` (or similar) does **not** exist in the worktree pre-release
`src/` tree (`grep -rli buysell src/` returns nothing). This is a retail-only
class with no 1998/99 superseded design; everything is from the retail decomp.

---

## §1 — Overview

The **Buy-Sell screen** is the modal shop / trade UI shown when an NPC
salesperson runs `buysellscreen` from a dialog script (cf.
`data/Modules/Demo/demo.s:184` — Elahni the spell-trainer in the demo). It is
a **452×178** bottom-anchored panel (chrome `BuySellMain` measured 452×178)
that overlays the lower half of the play screen and presents two columns:

- a **wares grid** on the left (items the NPC will sell, populated from script
  `BuySellAdd <itemname>`; or items the NPC will buy from the player, populated
  from `BuySellAddbuyitem <itemname>`),
- a **player-inventory grid** on the right (the items the player has that
  match the current SaleType filter — only ever populated in Sell mode, walked
  by the `cls_0x530af0` sub-pane through the eight `DAT_0065a14c..0x65a19c`
  `TObjectClass` tables).

Per row the cell shows the item name + its **computed price** in the format
`"%s %d"` (e.g. `"Lesser Healing 25"`) followed by the **gold-piece glyph** /
abbreviation tag `BSGP = "gp"`. The pane's top strip shows the player's
**current gold total** (label "GOLD" + integer value), and the bottom strip
has three large action buttons rendered as `BSBUY` / `BSSELL` / `BSEXIT`
(English tags resolve to "Buy" / "Sell" / "Exit"). A pair of small **up/down
scroll arrows** (`BuySellUp` 22×21, `BuySellDown` 22×46) lets the player scroll
each list.

**Instances:** ONE global (no mirroring, no player/target chip split — single
modal panel). Hosted modally above TPlayScreen during the `wait buysell`
script call (`demo.s:185`).

**Plain language.** When the dialog says "Hello, would you like to buy or
sell?" and the player picks Buy, the screen flips to this shop panel. The left
column lists the NPC's stock (each row shows the item name and what it costs);
the right column lists the player's inventory items that this shop will buy
from them. The top corner shows how much gold the player has now; the bottom
has three big buttons ("Buy", "Sell", "Exit") with arrow buttons to scroll
through long lists. The active mode (buy vs sell) controls which column is
the wares column and which is the inventory column, and the SaleType set by
`buysellshoptype` filters what kinds of items show up.

**Shown / hidden:** opened via script command `buysellscreen` (string anchor
`_data.txt:59304` `s_buysellscreen_005c8bac`); script then yields with `wait
buysell` until the player clicks Exit (`BuySellExit` button at hotkey VK_E =
`0x45 = 69`, callback `LAB_00530570`). The "Trouble initializing buysell
pane" error path (if Init returns 0 at `cls_0x5a5d64.cpp:175`) would route
through TPlayScreen the same way TDialogPane does. Visibility predicate
UNCONFIRMED (§14-F).

---

## §2 — Asset roster

Archive: `data/resources_unzipped/buysell.dat` (CGSR TMulti, 9 entries).
Measured with `tools/ui/dump_dat.py buysell.dat`.

| asset | archive | entry | WxH (measured) | flags | role | cite |
|---|---|---|---|---|---|---|
| `BuySellMain` | buysell.dat | [0] | **452×178** | `0x2` (15-bit) | screen chrome / background plate; stored `mbr_0x190` | `dump`; `cls_0x5a5d64.cpp:168` `FUN_0047f670("buysell.dat",…)`; XREF `_data.txt:113741` (paint body 0x52f88d) |
| `BuySellUpU` | buysell.dat | [1] | **22×21** | `0x402` (15-bit + shadow flag) | scroll-up button (idle/up) | `dump`; bound by name `BuySellUp` (`_data.txt:113720`) |
| `BuySellUpD` | buysell.dat | [2] | **22×21** | `0x402` | scroll-up button (pressed/down) | `dump`; ditto |
| `BuySellDownU` | buysell.dat | [3] | **22×46** | `0x402` | scroll-down button (idle) — **note 2× height** (composite up+down arrow stack) | `dump`; `BuySellDown` (`_data.txt:113725`) |
| `BuySellDownD` | buysell.dat | [4] | **22×46** | `0x402` | scroll-down button (pressed) | `dump`; ditto |
| `BuySellActivateU` | buysell.dat | [5] | **106×41** | `0x402` | confirm-purchase button (idle) | `dump`; `BuySellActivate` (`_data.txt:113729`) |
| `BuySellActivateD` | buysell.dat | [6] | **106×41** | `0x402` | confirm-purchase button (pressed) | `dump`; ditto |
| `BuySellExitU` | buysell.dat | [7] | **112×41** | `0x402` | exit-shop button (idle) | `dump`; `BuySellExit` (`_data.txt:113733`) |
| `BuySellExitD` | buysell.dat | [8] | **112×41** | `0x402` | exit-shop button (pressed) | `dump`; ditto |

**Localization tag labels** (resolved via
`data/resources_unzipped/english.def`; these are **text strings rendered
through `FUN_004be2b0`, NOT bitmap entries**):

| tag id | english.def value | role |
|---|---|---|
| `BSGOLD` | `"Gold"` | top-of-pane "Gold" label prefix (companion to player gold int) |
| `BSGP` | `"gp"` | gold-piece suffix in price cells (the `_%d_gp` part) |
| `BSBUY` | `"Buy"` | rendered onto the BuySellActivate button face in **Buy mode** |
| `BSSELL` | `"Sell"` | rendered onto the BuySellActivate button face in **Sell mode** |
| `BSEXIT` | `"Exit"` | rendered onto the BuySellExit button face |
| `BSWEA1` | `"Damage "` | per-weapon stat label, used in price-row by `FUN_0052da90` |
| `BSWEA2` | `"Min Strength"` | per-weapon stat label, used in price-row by `FUN_0052da90` |
| `BSARM1` | `"Protection"` | per-armor stat label |
| `BSARM2` | `"Rst Poison"` | per-armor stat label |
| `BSARM3` | `"Stlth"` | per-armor stat label |
| `BSARM4` | (empty in en) | per-armor stat label (Min Strn) |
| `BSARM5` | (empty in en) | per-armor stat label (Min Cons) |
| `BSFOOD1..4` | (empty in en) | per-food stat labels |
| `BSAMMO1` | (empty in en) | per-ammo stat label |

Tags cited from `data/resources_unzipped/english.def` (verified by `grep "^BS"
english.def`); paint XREFs at `_data.txt:113754` (`s_BSGOLD_005e3d8c`),
`113763` (`s_BSBUY_005e3d9c`), `113767` (`s_BSSELL_005e3da4`), `113772`
(`s_BSEXIT_005e3dac`); the price-format string is `s_%s_%d_005e3d94` at
`_data.txt:113759` — literal **`"%s %d"`** (one space, then the value).

**Notes / gotchas:**

1. **There is NO `BuySellMain` second entry**, NO `BSGOLD/BSBUY/BSSELL/BSEXIT`
   bitmap entries in buysell.dat, NO embedded coin glyph. The 4-button-pair
   chrome + chrome plate are all 9 entries; everything else is text.
2. The `BSBUY`/`BSSELL` choice tags from `demo.s:165` (`BSBUY2`/`BSSELL2`/
   `BSEXIT2`) are **dialog-choice tag IDs** rendered by TDialogPane, **NOT**
   buy/sell-screen labels. Distinct system; same `BS` prefix is coincidental
   namespace.
3. The "GOLD" string at `_data.txt:113746` `DAT_005e3d84` (literal `"GOLD"`,
   4 chars) referenced at paint address `0x52f8bc` is the **prefix label
   string used as `param_1` of an `sprintf`** to build the player's gold
   display (see §6). It is a **fallback / inline literal**, NOT a tag lookup
   — followed at `0x52f8c8` by the BSGOLD tag for the actual label. The
   intent is: render the localized "Gold" label (BSGOLD → "Gold") + a sprintf
   of "GOLD %d" → "GOLD 1234" overflow buffer (UNCONFIRMED-A — confirm both
   strings actually emit; possible the literal is a debug-fallback when tag
   lookup fails). See §14-A.
4. **External coin-glyph assets exist** but are NOT loaded by TBuySellScreen:
   `data/resources_unzipped/medgold.dat`, `medgolds.dat`, `medgoldg.dat`,
   `medgoldglow.dat`, `medgoldsel.dat`, `medgoldshad.dat`, `medgoldsilv.dat`,
   `smallgold.dat` (+ companions). All CGSR-format (verified `xxd | head`).
   These appear to be **dialog/inventory/multiplayer-only assets** —
   `grep -rln medgold recon/` returns no XREF from `0x52f7d0` paint body.
   Treat as out-of-scope for this panel until §14-A re-extraction proves
   otherwise.
5. The pre-release snapshot has **no** TBuySellScreen — `grep -rli buysell
   src/` returns nothing.

---

## §3 — Coordinate frames & surfaces

**Pane rect (screen):** set in ctor `cls_0x5a5d64.cpp:54-62`:
- `mbr_0x4 = 0` (pane x — initial; bottom-left-anchored to play screen).
- `mbr_0x8 = 0x12e = 302` (pane y).
- `mbr_0xc = 0x1c4 = 452` (pane w — **matches chrome BuySellMain width**).
- `mbr_0x10 = 0xb2 = 178` (pane h — **matches chrome BuySellMain height**).
- clip rect `(mbr_0x14, mbr_0x18, mbr_0x1c, mbr_0x20)` = `(0, 302, 452, 178)`
  (identical, no clip overhang).

So the panel sits at **screen `(0, 302) 452×178`** — **bottom-anchored**
(pane bottom = `302 + 178 = 480` = screen-h). With the play screen being
640×480, the panel covers the bottom-left **452 px out of 640** wide. There
is no chip model — the chrome bitmap is the full pane rect, drawn opaque at
pane-local `(0,0)`. The right ~188 px of the screen remains visible HUD
(presumably the right sidebar). **UNCONFIRMED-H:** verify the panel is
left-anchored vs centered (could be repositioned by a Show / Setup helper
after ctor — confirm by extracting `0x52f7d0` and reading the first blit's
`dst.x`).

### Frame table (mandatory)

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| **screen** | framebuffer | — | `(0,0)` | final pixels (640×480) | — |
| **bs-pane** | screen | **BL** (bottom-left) | screen `(0, 0x12e)` = `(0, 302)`; pane bottom = `302+178 = 480` | the whole BuySell screen (chrome + grids + labels + buttons) | ctor `:54-62` (`mbr_0x4=0, mbr_0x8=0x12e, mbr_0xc=0x1c4, mbr_0x10=0xb2`) |
| **chrome** | bs-pane | TL | bs-pane-local `(0,0)` | `BuySellMain` 452×178 backdrop | paint XREF `0x52f88d` → blit at (0,0) (assumed; UNCONFIRMED-A) |
| **wares-grid** | bs-pane | TL | bs-pane-local UNCONFIRMED (~`(x_L, y_top)`) | left column — NPC wares (Buy mode) or player inventory (Sell mode) | sub-pane `cls_0x531fc0`/`cls_0x531b70`; UNCONFIRMED-A |
| **inv-grid** | bs-pane | TR | bs-pane-local UNCONFIRMED (~`(x_R, y_top)`) | right column — player inventory items the NPC will buy | sub-pane `cls_0x530af0`; UNCONFIRMED-A |
| **cell(grid,row)** | grid | TL | grid-local `(0, row*rowH)` | one wares row (name + price text + selection hilite) | per cell record `mbr_0x198 + row*0x48` (`cls_0x530af0.cpp:493,627`) |
| **gold-strip** | bs-pane | TL | bs-pane-local UNCONFIRMED (top area; "GOLD %d" cell) | the player's current gold display | paint XREF `0x52f8bc` (literal "GOLD") + `0x52f8dc` ("%s %d") |
| **button-row** | bs-pane | BL | bs-pane-local UNCONFIRMED (bottom strip) | the 3 large action buttons (Activate / Exit) + 2 scroll buttons | buttons registered in Init; positions UNCONFIRMED-A |
| **source-rect** | each bitmap | TL | (sx,sy) into named asset | sub-rect copied out (full bmp for all buysell.dat entries) | dump |

**Anchor detection.** Pane x is the literal constant `0` (ctor `:54`); ctor
field `mbr_0x4 = 0` — TL-anchored at x. Pane y is `0x12e = 302`, and panel
bottom = `302+178 = 480` (screen-h) — **BL-anchored**, with bottom offset 0.
For resolution re-layout, anchor the pane to the **bottom-left** corner of
the play-screen (the right 188 px is the right-sidebar HUD).

**Composition formula (to screen):**
```
screen_x = 0 + x_in_pane
screen_y = 302 + y_in_pane
```

### Surfaces / two-stage blit

The ctor allocates one scratch surface at `cls_0x5a5d64.cpp:224-237`:

- `FUN_00482fb0(0x78)` allocates a 120-byte `cls_0x5a68d8` instance.
- `cls_0x5a68d8::meth_0x4a5740(this_00, 400, 0x18)` — **creates a 400×24 (=
  `0x190` × `0x18`) surface** (`:234`). This is the **per-cell text scratch
  RT** used to compose the row text into a fixed-size buffer before blitting
  the row into the wares/inv grid. 400 px wide easily covers the row's name
  + price text + glyph horizontal extent (the grids must be ≤ 400 px wide).
  24 px high gives a comfortable single-row text height (~14-px glyph + 3-px
  shadow + 7-px padding — fits a 12px-or-so font).
- This scratch lives at `mbr_0x19c` for the lifetime of the pane.

The cell record array lives at `mbr_0x198` (allocated/grown by
`cls_0x533280::meth_0x533280` in the sub-panes when `mbr_0x196 < mbr_0x194 +
4`, growing by 4 records at a time; each record is **`0x48 = 72` bytes**, see
the `for (iVar4 = 0x12; …) *puVar5++ = *puVar10++` inner copy at
`cls_0x530af0.cpp:97-103`). The 72-byte record holds the per-cell snapshot
of `FUN_0052da90`'s output (item-class ptr, label tag, computed price,
display name pointer, stat-bonus pair, etc.).

`mbr_0x194` = current count; `mbr_0x196` = capacity. `mbr_0x1b4` = wares head
(for the sub-pane that walks an NPC's wares list — set by script command
`buyselladd` via `meth_0x5883f0`, `cls_0x5a5d64.cpp:910-916`).

### Layout diagram (bs-pane-local, schematic — exact rects UNCONFIRMED)

```
bs-pane  (bs-pane-local; origin = screen (0,302))   w=452  h=178
0                                                                452
0  ┌──────────────────────────────────────────────────────────────┐
   │  [BuySellMain chrome — 452x178 opaque @ (0,0)]               │
   │                                                              │
   │  [Gold label   "Gold"  +  player-gold int  @ top area]       │
   │   (rendered text via 0x52f8bc "GOLD" + 0x52f8c8 BSGOLD       │
   │    + 0x52f8dc "%s %d")                                       │
   │                                                              │
   │  ┌──────────────────┐         ┌──────────────────┐           │
   │  │  WARES GRID      │         │  INV GRID        │           │
   │  │  (NPC items      │         │  (player items   │           │
   │  │   in Buy mode)   │         │   in Sell mode)  │           │
   │  │  row: item name  │         │  row: item name  │           │
   │  │       + " %d"    │         │       + " %d"    │           │
   │  │       price      │         │       price      │           │
   │  │  + per-stat tag  │         │  + per-stat tag  │           │
   │  │   (BSWEA1/ARM1/  │         │   (BSWEA1/ARM1/  │           │
   │  │    BSGP)         │         │    BSGP)         │           │
   │  │  + selection     │         │  + selection     │           │
   │  │   hilite if row  │         │   hilite if row  │           │
   │  │   == mbr_0x180   │         │   == mbr_0x180   │           │
   │  └──────────────────┘         └──────────────────┘           │
   │   [BuySellUp 22x21]            [BuySellUp 22x21]             │
   │   [BuySellDown 22x46]          [BuySellDown 22x46]           │
   │   (scroll btns — UNCONFIRMED right-side positions)           │
   │                                                              │
   │   [BuySellActivate 106x41  label = BSBUY / BSSELL ]          │
   │   [BuySellExit     112x41  label = BSEXIT          ]         │
   └──────────────────────────────────────────────────────────────┘
178
```

Exact (x,y) for each frame are UNCONFIRMED until §14-A. The diagram is
schematic — drawn from the asset dims + paint XREF order + the two-grid
buy/sell semantics of the sub-panes.

---

## §4 — Static element layout

**All x,y are bs-pane-local (compose with screen `(0, 302)`).** Many positions
are UNCONFIRMED pending §14-A extraction of `0x52f7d0`; the cited values are
inferred from asset widths + standard centering / corner-anchor patterns and
must be confirmed by reading the paint disassembly.

| element | space | (x,y) | (w,h) | source | drawmode | cite |
|---|---|---|---|---|---|---|
| chrome `BuySellMain` | bs-pane | (0,0) | (452,178) | full bmp (entry 0) | `0x80000000` (opaque) | XREF `_data.txt:113741` (paint `0x52f88d`); standard "chrome at (0,0)" pattern (cf. InventoryPane §4) |
| "GOLD" / BSGOLD label | bs-pane | UNCONFIRMED-A | (text) | tag "Gold" + sprintf "GOLD %d" | text | XREF `_data.txt:113745` (`DAT_005e3d84` literal "GOLD" @ 0x52f8bc), `113754` (BSGOLD @ 0x52f8c8) |
| price-format `"%s %d"` | sub-pane cell | UNCONFIRMED-A | (text, 400×24 max — scratch dim) | per-cell name + price | text | XREF `_data.txt:113759` (paint `0x52f8dc`) |
| BSBUY/BSSELL button label | activate-btn | UNCONFIRMED-A (text centered on 106×41 face) | (text) | tag "Buy"/"Sell" | text | XREF `_data.txt:113763` (`0x52f9c9`), `113767` (`0x52f9ef`) |
| BSEXIT button label | exit-btn | UNCONFIRMED-A (text centered on 112×41 face) | (text) | tag "Exit" | text | XREF `_data.txt:113772` (paint `0x52fa2e`) |
| **BuySellUp** scroll button | bs-pane | UNCONFIRMED-A | (22,21) | buysell.dat[1/2] U/D | button stamp | Init `cls_0x5a5d64.cpp:183` (hotkey `0x26 = VK_UP`) |
| **BuySellDown** scroll button | bs-pane | UNCONFIRMED-A | (22,46) | buysell.dat[3/4] U/D | button stamp | Init `:192` (hotkey `0x28 = VK_DOWN`) |
| **BuySellActivate** confirm button | bs-pane | UNCONFIRMED-A | (106,41) | buysell.dat[5/6] U/D | button stamp | Init `:201` (hotkey `0x41 = 'A'`) |
| **BuySellExit** exit button | bs-pane | UNCONFIRMED-A | (112,41) | buysell.dat[7/8] U/D | button stamp | Init `:210` (hotkey `0x45 = 'E'`) |

**Button geometry note.** Buttons are registered through `cls_0x5a3c68`
(TButton-like leaf widget; cls is 328 bytes per `cls_0x5a3c68.cpp:39`). The
ctor at `0x42c500` (`cls_0x5a3c68.cpp:130-181`) reads a 4-int rect-like blob
from a meta field (`pdVar3[0..3]` at `:171-177`: `mbr_0x60 = -pdVar3[2]`,
`mbr_0x64 = -pdVar3[1]`, `mbr_0x68 = pdVar3[0]`, `mbr_0x6c = pdVar3[3]`) —
this is a **negative-anchor** initialization, suggesting buttons may be
anchored to the **pane's bottom-right corner** via a `(-offsetX, -offsetY,
w, h)` quad (standard "negative coord = anchor to far edge" idiom). The
button's `(x,y)` is then stored at the widget's `mbr_0x60/0x64`. **Where the
rect-blob is sourced from (a bitmap header? a per-button DEF entry? a hardcoded
table?) is UNCONFIRMED-D.** Best resolution: extract `cls_0x42c500` body and
the 4 button instances' `mbr_0x9c` blob locations.

**Symmetry note.** Two scroll buttons (Up + Down) likely sit on EACH grid side
(one pair per grid). The Init only registers ONE pair, so either (a) the
buttons act on whichever grid is currently focused (mbr_0x17c bit-flags
determine focus — see §6 mode flags), or (b) the same physical buttons drive
both columns globally with focus indicating which list scrolls. Confirm by
reading the keypress handler `virt_meth_0x52fa60` (already extracted —
analyzed in §10).

---

## §5 — Draw order / composition (vtable slot 84 = `0x52f7d0`)

**The paint body is NOT extracted in the snapshot recon.** What follows is the
**ordered call sequence proven by the string-XREF map**
`_data.txt:113741..113773`, which shows the literals in the order they are
loaded into registers within the function `virt_meth_0x52f7d0`:

| call addr | string at this addr | inferred call |
|---|---|---|
| `0x52f88d` | `"BuySellMain"` (`_data.txt:113741`) | resolve-by-name → chrome handle, blit opaque at (0,0) |
| `0x52f8bc` | literal `"GOLD"` (`_data.txt:113745`, `DAT_005e3d84`) | likely `sprintf(buf, "GOLD %d", gold)` — see UNCONFIRMED-A; possibly a fallback when BSGOLD tag lookup fails |
| `0x52f8c8` | `"BSGOLD"` tag (`_data.txt:113754`) | tag lookup → "Gold"; render as text in top label area |
| `0x52f8dc` | `"%s %d"` (`_data.txt:113759`) | per-cell sprintf format: `"<item name> <price>"` (one space) |
| `0x52f9c9` | `"BSBUY"` (`_data.txt:113763`) | tag lookup → "Buy"; render as Activate-button face label (in Buy mode) |
| `0x52f9ef` | `"BSSELL"` (`_data.txt:113767`) | tag lookup → "Sell"; render as Activate-button face label (in Sell mode) |
| `0x52fa2e` | `"BSEXIT"` (`_data.txt:113772`) | tag lookup → "Exit"; render as Exit-button face label |

Inferred top-level paint sequence (UNCONFIRMED-A: exact ordering of step 5-7,
exact cell rects, exact label positions):

1. **Pane setup / clip.** Standard TPane: if `mbr_0x50 != 0` (visible flag,
   slot 80 `cls_0x5a5d64.cpp:281`), proceed. Compose into the pane's render
   target.
2. **Chrome.** Resolve `BuySellMain` by name (`FUN_0046d710` UI_METHOD_MAP §9,
   in the by-name form), blit opaque at bs-pane-local `(0,0)`, size 452×178.
3. **Gold strip.** Build the gold display text. The two string references at
   `0x52f8bc` (literal "GOLD") + `0x52f8c8` (BSGOLD tag) together drive one or
   two text writes — likely (a) the BSGOLD label "Gold" is drawn at one fixed
   cell, then (b) the player's gold int is `sprintf`'d via the literal "GOLD"
   buffer (`"GOLD %d"`) and drawn adjacent. UNCONFIRMED-A.
4. **Wares grid.** Iterate `mbr_0x198` cell records (`(*+0x198)[0..mbr_0x194-1]`,
   each 72 bytes). For each record, compose into the 400×24 scratch
   (`mbr_0x19c`) using format `"%s %d"` (XREF `0x52f8dc`) — `param_1` = item
   name, `param_2` = price. Then blit the scratch row into the wares-grid
   column at `(grid_x, row_top + row * rowH)`. Highlight the selected row
   (where row == `mbr_0x180`).
5. **Inv grid.** Same as wares grid but for the other column (sub-pane
   `cls_0x530af0` iterates 8 player-inventory class tables).
6. **Activate-button label.** Based on mode (`mbr_0x17c & 8` = Buy / `& 0x10` =
   Sell, see §6), render the appropriate tag (BSBUY or BSSELL) centered on
   the BuySellActivate button face. Text rendered via `FUN_004be2b0` (the
   font helper); button bitmap itself is blitted by the inherited slot-44
   `virt_meth_0x444fc0` button-children draw.
7. **Exit-button label.** Render BSEXIT text centered on the BuySellExit
   button face.
8. **Button children paint** (vtable slot 44 = `virt_meth_0x444fc0` inherited
   from TButtonPane base) paints the 4 button sprites (Up/Down/Activate/Exit
   U or D states) over the chrome. Order TBD — see UNCONFIRMED-A.
9. Finalize pane render target (clip reset / dirty-rect for display path).

---

## §6 — Algorithms (pseudocode per helper)

### 6.1 Mode flags (`mbr_0x17c`) — the modal state

`mbr_0x17c` (offset `+0x17c`, 4-byte field) is a **bit-mask** of the current
shop mode and SaleType filter, written by the script-API setter
`meth_0x5883e0` (`cls_0x5a5d64.cpp:895-904`):

```
void cls_0x5a5d64::SetMode(uint32_t flags) {
    this->mbr_0x17c = flags;
}
```

Decoded bit semantics (read from sub-pane filtering):

| bit | meaning | evidence |
|---|---|---|
| `0x02` | getter `meth_0x588400` (`cls_0x5a5d64.cpp:921-927`) returns `mbr_0x17c & 2` — a global "shop is open" / "wares dirty" flag | `return this->mbr_0x17c & 2;` |
| `0x04` | **Sell mode** (player → NPC). Sub-pane `cls_0x531fc0` switch case `2` triggers when `(mbr_0x17c & 4) != 0` or `(mbr_0x17c & 0x10) != 0` (`cls_0x531fc0.cpp:59-61`); `cls_0x530af0` uses `((uVar5 & 4) == 0)` to gate the inventory-walk branch (`:46-47`) | direct |
| `0x08` | **Buy mode** (NPC → player). `cls_0x531fc0.cpp:55-56`: `case 1: bVar1 = this->mbr_0x17c & 8;` (selection-gate uses bit 8) | direct |
| `0x10` | **inventory-walk mode** (Sell): the cls_0x530af0 sub-pane is active. `cls_0x5a5d64.cpp:434` `if ((this->mbr_0x17c & 0x10) != 0)` gates the 8-class-table walk in `meth_0x530670` | direct |

These bits select between the wares list (NPC's items, in `mbr_0x1b4` linked
list, walked by `cls_0x531fc0`/`cls_0x531b70`) and the player-inventory grid
(walked by `cls_0x530af0` through `DAT_0065a14c..0x65a19c` class tables —
the 8 standard `TObjectClass` instances for weapon / armor / food / ammo /
magic / potion / misc / scroll).

### 6.2 `FUN_0052da90(item, mode, type)` — the price-compute (UNCONFIRMED body)

**Body NOT extracted.** Signature confirmed from 10+ call sites and `_data.txt`
XREF map:

```
int FUN_0052da90(
    void* item,              // an item / object-class instance (the item being priced)
    uint  mode,              // this->mbr_0x17c (the buy/sell + SaleType flags)
    uint  type               // 1 in cls_0x530af0 calls; varying in cls_0x5a5d64 calls
);
// returns: 0 if item is excluded from this shop (filtered out by SaleType / not
//          for sale); non-zero (price?) if accepted and the cell record was
//          populated.
```

**Behavior (inferred from string-XREF map at `_data.txt:113053..113700+`):**
the function is a large class-tiered dispatch:

1. Look up the item's **`Value` stat** (`_data.txt:113053` `s_Value_005e380c`
   XREF `FUN_0052da90:0052db44` — `oi->Stat("Value")`). This is the **base
   gold value** stored on the object class (cf. user-supplied context:
   "Price source: computed at draw-time by FUN_0052da90 from the object
   class's 'Value' stat").
2. Switch on the item's class (weapon / armor / food / ammo / etc.) — each
   branch reads a **per-class stat name** (BSARM1..5, BSWEA1..2, BSFOOD1..4,
   BSAMMO1) and a **display label** (Protection, Damage, Min Strength, etc.)
   for the price-row sub-text. The XREF table at `_data.txt:113057..113700`
   shows alternating BSARM%d/BSWEA%d tag-name lookups + plain-English label
   lookups (Protection, Damage, Min Strength, etc.) — confirms the function
   builds **(tag_id, value)** pairs to be rendered alongside the price.
3. Apply mode-modifier: in Buy mode (NPC selling to player), price is the
   base value times some markup; in Sell mode (player to NPC), the price is
   discounted. The exact formula is UNCONFIRMED-B; the snapshot retail-port
   protocol notes that gold totals are clamped to 0..max via `min(player_gold,
   value)` checks at the callback (LAB_00530570 buy-confirm path).
4. **Output:** populate the 18-int (72-byte) stack buffer `auStack_48[18]`
   (cf. `cls_0x531b70.cpp:73` `FUN_0052da90(auStack_48, item, mode, type)` —
   note the 4-arg form with the buffer prepended; the 3-arg form returns the
   buffer by value via stack-out-pointer). The buffer is then copied into
   `mbr_0x198 + row*0x48` (sub-pane `cls_0x530af0.cpp:97-103`).
5. **Returns 0** when the item is excluded (wrong SaleType, NPC doesn't deal
   in this category, item is broken / equipped / cursed / etc.); non-zero
   otherwise. Cf. every call site: `if (iVar4 == 0) goto skip_cell`.

**Resolve (§14-B):** `DecompileAddr.java 0x0052da90` to extract the 5800-byte
body. Expected output: a per-class-tier dispatch (weapons / armor / scrolls /
food / potions / misc) with per-class stat extraction + label assembly. The
72-byte cell record holds `(itemPtr, classTier, displayName, price,
stat1_tag, stat1_value, stat2_tag, stat2_value, sale-flag)` etc.

### 6.3 Cell record struct (`mbr_0x198[i]`, 0x48=72 bytes each)

Inferred from the inner copy loop `for (iVar4 = 0x12; …) *puVar5++ = *puVar10++;`
in `cls_0x530af0.cpp:97-103` (18 dwords = 72 bytes), and the field-access
patterns in `cls_0x531fc0`:

| offset | size | role | source |
|---|---|---|---|
| `+0x00` | 4 | item pointer (TObjectInstance*) | first arg passed to FUN_0052da90 |
| `+0x04..+0x44` | 16×4 | 16 dwords of per-cell payload — likely (displayName ptr, classTier, price, stat1_tag, stat1_value, stat2_tag, stat2_value, …) | UNCONFIRMED-B |

The exact field layout is UNCONFIRMED-B and must be decoded from §14-B
(extracting `FUN_0052da90`).

### 6.4 Sub-pane: `cls_0x530af0::meth_0x530af0(this, param_1, param_2)` — inventory walker

The Sell-mode player-inventory walker. Iterates 8 `TObjectClass` tables in
this fixed order (cf. `cls_0x530af0.cpp:46-176, 244+, …`):

| pass | class table | offset | role |
|---|---|---|---|
| 1 | `DAT_0065a14c` (`numclasses > 1`) | weapons | walk every player-owned weapon, call `FUN_0052da90` for each |
| 2 | `DAT_0065a150` (`numclasses > 2`) | armor | walk player armor |
| 3 | `DAT_0065a158` (`numclasses > 4`) | food | walk player food |
| 4 | `DAT_0065a190` (`numclasses > 0x12`) | scrolls (item 0x12) | walk player scrolls |
| 5 | `DAT_0065a19c` (`numclasses > 0x15`) | (item 0x15) | walk player class-21 |
| 6 | `DAT_0065a15c` (`numclasses > 5`) | item 5 | walk player class-5 (potions?) |
| 7 | `DAT_0065a18c` (`numclasses > 0x11`) | item 0x11 | walk player class-17 |

For each item: call `FUN_0052da90(item, mbr_0x17c, 1)`; if non-zero, grow
`mbr_0x198` if full (by `cls_0x533280::meth_0x533280` — adds 4 records), copy
the 72-byte returned buffer into `mbr_0x198 + (current_count)*0x48`, increment
`mbr_0x194`.

The control flow uses **two parameter bounds** (param_2 = lower limit, param_3
on stack = upper limit) to gate cells by price range — used for filtering
"items NPC will buy in this range".

### 6.5 Sub-pane: `cls_0x531fc0::meth_0x531fc0(this, ...)` — SaleType-filtered wares walker

The Buy-mode NPC-wares walker. Walks a linked list off `mbr_0x1b4` (the wares
head — set by `buyselladd`). For each item:

1. Filter by sale-type bit-flags (the `switch (*(undefined2 *)(piVar2 + 1))`
   at `cls_0x531fc0.cpp:54-69`):
   - case 1 → check `mbr_0x17c & 8` (Buy mode bit)
   - case 2 → check `(mbr_0x17c & 4) || (mbr_0x17c & 0x10)` (Sell mode bits)
   - cases 4/5/0x11/0x12/0x15 → check `mbr_0x17c & 0x10`
   - other cases → skip
2. Read item-class's `SaleType` stat (vtable `+0xd4` getter call —
   `cls_0x531fc0.cpp:52` `(**(code **)(*piVar2 + 0xd4))(s_SaleType_005e3e80)`);
   require `iVar3 != 2` (SaleType != 2; SaleType==2 means "never for sale").
3. Read item's price (vtable `+0x190` getter — `(*piVar2 + 400)()` at
   `:53`); require `iVar3 != -1` (not "no price").
4. Read item's lookup index (vtable `+0xd4` again at `:73`); pass to
   `FUN_0052da90(iVar3, mbr_0x17c, vtable+0x198 result)` for the cell record.
5. Apply lo/hi price gate (param_2/param_3 = price range from
   `buyselladdbuycriteria`).
6. Grow + copy 72-byte cell record into `mbr_0x198`.

The `cls_0x531b70` variant differs only in passing the cell scratch buffer
directly as `FUN_0052da90`'s first arg (the 4-arg form) — semantically
identical, different calling convention. **UNCONFIRMED-G:** which one runs in
which mode permutation.

### 6.6 `meth_0x530670` — inline wares-add (used by `buyselladdbuyitem`)

The `buyselladdbuyitem` script command (param_1 = item-class index) calls a
class-table-lookup pipeline that ultimately writes one row into
`mbr_0x198`. The function at `0x530670` (`cls_0x5a5d64.cpp:414-635`):

1. **If `mbr_0x17c & 0x10`** (inventory-grid mode): try seven class-table
   lookups in priority order (DAT_0065a150 / 14c / 158 / 190 / 15c / 18c /
   19c) — looking for the item across multiple class buckets. If found, gate
   on `class.numerator < ...` checks and a final `iVar4 == 1` (active item)
   test → call `FUN_0052da90` → grow/copy 72-byte record.
2. **Otherwise** (Buy mode): pick class table by `mbr_0x17c & 4` (selects
   class 1 vs class 2); call `FUN_0052da90` → grow/copy.

The grow-and-copy idiom (`mbr_0x196 < mbr_0x194` → realloc to
`mbr_0x196 + 4` cells, copy old → free old, advance) is the canonical
dynamic-array pattern used throughout these sub-panes.

---

## §7 — Effects & shadows

- **Drop shadow** on the chrome `BuySellMain` — none indicated (the chrome
  bitmap is `flags=0x2` opaque). For the action buttons (BuySellUp / Down /
  Activate / Exit), all are flagged `0x402` = `BM_15BIT | 0x400` — the
  `0x400` bit signals **shadow enabled** at the blit (UI_METHOD_MAP §5
  treatment of bit `0x400` in fontIndex == shadow flag; the same bit on a
  bitmap flag word similarly enables shadow pass). The shadow direction (dx,
  dy) is set by the surrounding `FUN_00438d80(buf, dx, dy)` call in the
  paint body (UNCONFIRMED-A — exact dx, dy not extracted). **Assume per-spec
  retail convention**: BL-anchored panel + buttons cast `(+4, +4)` shadow
  down-right (UI_METHOD_MAP §8 / NOMENCLATURE §4 "drop shadow").
- **Text 3-pass shadow.** All text labels (BSGOLD / BSBUY / BSSELL / BSEXIT
  plus per-cell `"%s %d"` row text) go through `FUN_004be2b0_CompositeBuffer`
  (UI_METHOD_MAP §5). The font index used carries the `0x400` shadow bit (the
  retail convention for HUD labels; cf. TPlyrStatusBar `0x401`/`0x402`/
  `0x404`). Result: every label gets a **1-px black drop shadow toward right
  + bottom**, colored text on top.
- **Pink-halo bug**: any text rendered into a magenta-cleared scratch
  composite will exhibit the pink-halo artifact (UI_METHOD_MAP §16,
  [[project-retail-pink-halo-bug]]). The 400×24 scratch (`mbr_0x19c`) is
  almost certainly magenta-cleared between rows, so retail shop captures will
  show a pink fringe around the price-row text — **do not reproduce**.
- **Chroma key.** All buysell.dat entries have `kc=0x0` (from the dump). The
  chrome is opaque (flags=0x2). The button-state bitmaps are flags=0x402
  (`BM_15BIT | 0x400`-shadow) — they likely carry **alpha**, not magenta. Do
  NOT apply the global magenta key on the U/D button sprites.

---

## §8 — Text rendering (table)

All text in this panel goes through `FUN_004be2b0_CompositeBuffer` (the GDI
DrawTextA + offscreen-HDC pipeline; UI_METHOD_MAP §5). Exact font index, font
px, and per-text rect are UNCONFIRMED-A (require paint body extraction).

| string | cell (space, x,y,w,h) | font | px | color | h-align | v-align | shadow | format | source | cite |
|---|---|---|---|---|---|---|---|---|---|---|
| "Gold" (BSGOLD tag) | bs-pane UNCONFIRMED top strip | UNCONFIRMED-A | UNCONFIRMED-A | UNCONFIRMED-A (likely cream/gold) | UNCONFIRMED-A (likely left) | top | 3-pass black (if font bit `0x400` set; matches HUD convention) | `"%s"` (tag id rendered as is) | localization | XREF `_data.txt:113754` (paint `0x52f8c8`) |
| player gold value | bs-pane UNCONFIRMED top strip | UNCONFIRMED-A | UNCONFIRMED-A | UNCONFIRMED-A | UNCONFIRMED-A | top | 3-pass | `"%d"` (or `"GOLD %d"` via the literal "GOLD" buffer) | `player.Gold()` (`DAT_00667fcc` + getter — UNCONFIRMED-C) | XREF `0x52f8bc` |
| per-row `"%s %d"` | scratch (`mbr_0x19c`, 400×24) | UNCONFIRMED-A | UNCONFIRMED-A | UNCONFIRMED-A | left | top | 3-pass | `"%s %d"` | cell record (item name + price) | XREF `_data.txt:113759` (paint `0x52f8dc`) |
| BSBUY/BSSELL btn label | activate-btn face (106×41) | UNCONFIRMED-A | UNCONFIRMED-A | UNCONFIRMED-A (likely cream/gold) | center | center (likely DT_VCENTER set since button face is square-ish; UNCONFIRMED-A — could be top with vertical centering by manual rect computation) | 3-pass | `"%s"` (tag id) | localization | XREF `0x52f9c9`/`0x52f9ef` |
| BSEXIT btn label | exit-btn face (112×41) | UNCONFIRMED-A | UNCONFIRMED-A | UNCONFIRMED-A | center | center | 3-pass | `"%s"` | localization | XREF `0x52fa2e` |
| per-row per-stat (BSWEA1 = "Damage ", BSARM1 = "Protection", BSGP = "gp", etc.) | scratch (`mbr_0x19c`) | UNCONFIRMED-A | UNCONFIRMED-A | UNCONFIRMED-A (likely smaller/dimmer than name) | UNCONFIRMED-A | top | 3-pass | UNCONFIRMED — likely `"%s%d"` or `"%s %d"` | `FUN_0052da90` cell record stat fields | XREF for each tag in `_data.txt:113057..113700+` |

---

## §9 — Animation & dynamic behavior

The retail TBuySellScreen has **no continuous animation** in the extracted code
paths. The `mbr_0x184` (last-redraw-row?), `mbr_0x188` (selected row),
`mbr_0x18c` (scroll-offset?) are 4-byte fields written by the nav handler
(`virt_meth_0x52fa60`) and read by the paint method to drive **selection
highlight** + **scroll position**. There is no fade / hover ramp / tween in
extracted code (unlike TDialogPane's choice-fade).

```
selection-state mbr_0x180 (current selected row index in the grid):
  range   0 .. (mbr_0x194 − 1)             # capped to # of populated cells
  step    +1 per VK_DOWN, −1 per VK_UP
  step    set absolute to 0/1/2 per VK_1/VK_2/VK_3 keypress
          (these are also "select page" keys — see §10)
  on-end  clamp to (mbr_0x194 − 1) at upper edge
  drive   virt_meth_0x52fa60 / virt_meth_0x52fd50 (mouse + key)
  cite    cls_0x5a5d64.cpp:299-317; :344-378
```

**Dirty / redraw model.** The pane uses the standard TPane `mbr_0x48` (dirty
flag) + `mbr_0x4c` (deeper-dirty flag) pair, both set by Reset
(`meth_0x530600`, `cls_0x5a5d64.cpp:407-409`). The Reset path also frees the
`mbr_0x198` cell array (`:401-405`), reinitializing the grid contents. The
script-API `buyselladd*` commands grow `mbr_0x198` incrementally and presumably
set the dirty flag.

---

## §10 — Input & dispatch

### 10.1 Buttons (registered in Init `virt_meth_0x52f390`)

| button | bitmap stem | hotkey | callback | cite |
|---|---|---|---|---|
| **BuySellUp** | `BuySellUp{U,D}` 22×21 | `0x26 = 38 = VK_UP` | `meth_0x52fe90` | `cls_0x5a5d64.cpp:183` |
| **BuySellDown** | `BuySellDown{U,D}` 22×46 | `0x28 = 40 = VK_DOWN` | `meth_0x52fee0` | `:192` |
| **BuySellActivate** | `BuySellActivate{U,D}` 106×41 | `0x41 = 65 = 'A'` | `cls_0x5b93c4::meth_0x52ff40` | `:201` |
| **BuySellExit** | `BuySellExit{U,D}` 112×41 | `0x45 = 69 = 'E'` | `LAB_00530570` (closes the screen + ends `wait buysell`) | `:210` |

Each button is registered via `cls_0x5a3c68::cls_0x5a3c68(this, buysellDat,
name, hotkey, callback, 0, 0, 0, 0x10, 0xffffffff)` then added through the
base `cls_0x5b98b8::meth_0x436790` (the children-add). After each button
add, the inherited child's `+0x14 & ~0x20` flag is masked (`:216-222`) —
clears the bit-5 flag, possibly a "render in HUD overlay" hint.

### 10.2 Keyboard nav (`virt_meth_0x52fa60`, `cls_0x5a5d64.cpp:290-329`)

Reads from the inherited `meth_0x4361f0` keypress path. Custom handling:

- `param_1 == 0x31` (VK_1 / '1') → `mbr_0x188 = mbr_0x180` then bound-clamp +
  redraw via vtable `+0x44` (`:299-303`). Selects the **first** group / page.
- `param_1 == 0x32` (VK_2 / '2') → `mbr_0x188 = mbr_0x180 + 1` (`:306-308`).
  Selects the **second** group / page.
- `param_1 == 0x33` (VK_3 / '3') → `mbr_0x188 = mbr_0x180 + 2` (`:310-312`).
  Selects the **third** group / page.
- After every set: clamp `mbr_0x188 = min(mbr_0x188, mbr_0x194-1)`
  (`:315-317`).
- Keys `B/V/b/v` (`0x42/0x56/0x62/0x76`) → fallthrough (no special handling,
  just propagated; possibly for "Buy"/"View" mode toggles in editor
  contexts, UNCONFIRMED).
- All other keys → propagate to base `cls_0x5b93c4::virt_meth_0x4361f0`
  (`:319-327`) — this handles `BuySellExit` (E), button shortcuts, etc.

### 10.3 Mouse handler (`virt_meth_0x52fd50`, `cls_0x5a5d64.cpp:333-382`)

Parameter shape (3-arg): `(this, button, mouse_y, mouse_x_on_stack)`. Body:

1. Always call base `cls_0x5b93c4::virt_meth_0x436530(this, button, y)` first
   (`:343`) — the parent dispatches to children (button click).
2. Only proceed if `param_1 == 4 || param_1 == 5` (left/right button down?)
   AND `param_2 < 200` (y bound — the cell area is within first 200 px of
   pane).
3. Hit-test the mouse x against THREE column ranges:
   - `0x2b < x ≤ 0x55` (43 < x ≤ 85): set `mbr_0x188 = mbr_0x180`
     (`:366-368`) — select **column 0** (left/wares column?).
   - `0x57 ≤ x ≤ 0x81` (87 ≤ x ≤ 129): set `mbr_0x188 = mbr_0x180 + 1`
     (`:357-359`) — select **column 1**.
   - `0x82 < x < 0xae` (130 < x < 174): set `mbr_0x188 = mbr_0x180 + 2`
     (`:347-349`) — select **column 2**.
4. After: emit a `cls_0x41c7d0::meth_0x49b990(DAT_00667548, '?', 0x7f, 1)`
   sound (likely "click1" — string anchor at `_data.txt:113777` for the
   ASCII char emitted as a sound id).
5. Clamp `mbr_0x188` to `mbr_0x194-1`, redraw.

**Implication (UNCONFIRMED).** The x-ranges `(43..85)`, `(87..129)`,
`(130..174)` correspond to THREE column-pickable hot-zones on the **top
strip** of the pane (y < 200, but pane is only 178 tall — so this covers
the whole pane vertically). These are **three category-tab regions** at the
top, mapped to keys `1`/`2`/`3`. **UNCONFIRMED-E:** verify these are
category tabs vs scroll-bar zones vs page-step buttons. The interpretation
matches the demo's three categories (`misc` / `weapons` / `armor`) but the
exact button labels are not in the asset list.

### 10.4 Exit / close path (`LAB_00530570`, `meth_0x530670` linked area)

`BuySellExit` callback `LAB_00530570` is referenced at `cls_0x5a5d64.cpp:210`
but the body is in a separate function. The Reset path `meth_0x530600`
(`:386-410`) is called from this callback to free `mbr_0x198` (the cell
records) and reset the dirty flags. The script `wait buysell` (`demo.s:185`)
yields until this exit fires; `LAB_00530570` then signals the script engine
to resume.

---

## §11 — Retail bugs NOT to reproduce

1. **Pink-halo bug** on all text labels (UI_METHOD_MAP §16,
   [[project-retail-pink-halo-bug]]). The 400×24 cell-text scratch
   (`mbr_0x19c`) is magenta-cleared between rows; the antialiased text
   shadow edges sample the cleared pixels → magenta fringe survives the
   keyed blit → pink halo on every BSGOLD / per-row / BSBUY / BSSELL / BSEXIT
   label. **Fix:** render text with real alpha (no chroma key) on the
   400×24 scratch, blit with `DrawSurfaceTinted` not `DrawSurface`+key.
2. **Shadow trap on button labels** (`[[project-retail-shadow-semantics]]`).
   If the paint code uses `FUN_00438d80(buf, +4, +4)` for button-label
   shadows, the recon `(dx, dy)` is **where the SHADOW LANDS**, not where the
   content draws. The content must blit at `dst - (dx, dy)`. A
   reconstruction that uses the recon (dx, dy) as content dst gets the label
   offset 4 px down-right of intended.
3. **NULL-tag rendering**: BSARM4/BSARM5/BSFOOD1..4/BSAMMO1 are EMPTY in the
   English `english.def` (verified by `strings | grep "^BS" | head`). If a
   shop carries an item whose stat-label resolves to one of these empty
   tags, the row will render with the tag-id literal (`"BSARM4 0"`) or with
   no label (depending on the tag-resolver's empty-string behavior). Either
   way it looks broken. Decision for the port: **fall back to a sensible
   English string ("Min Strn", "Min Cons", etc.) when the tag is empty**,
   matching the in-code labels at `_data.txt:113145` `s_Min_Strn` and
   `_data.txt:113170` `s_Min_Cons` (these are inline literals already in the
   exe — likely the original developer's reference defaults).

---

## §12 — Reconstruction pseudocode

```cpp
// vtable slot 84 == TBuySellScreen::Paint == 0x52f7d0
void TBuySellScreen::Paint(TSurface* dst) {
    if (!mbr_0x50) return;                           // not visible

    Renderer* R = Renderer::Get();

    // §5 step 2 — chrome (XREF 0x52f88d, "BuySellMain")
    TBitmap* chrome = mbr_0x190->ResolveByName("BuySellMain");
    R->DrawBitmapToTarget(chrome, dst, /*x=*/ 0, /*y=*/ 0);   // bs-pane-local

    // §5 step 3 — gold strip (XREF 0x52f8bc literal "GOLD" + 0x52f8c8 tag BSGOLD)
    const char* goldLabel = Localize("BSGOLD");      // "Gold" in en
    int playerGold = Player::Get()->Gold();          // UNCONFIRMED-C
    char buf[64];
    // UNCONFIRMED-A: exact rect for the gold strip
    DrawTextToTarget(dst, /*atlas=*/atlasHUD,
                     goldLabel, /*x=*/ kGoldLabelX, /*y=*/ kGoldLabelY,
                     /*w=*/ kGoldLabelW, kAlignLeft, kColorGold,
                     /*shadow=*/ true);
    snprintf(buf, sizeof buf, "%d", playerGold);
    DrawTextToTarget(dst, atlasHUD, buf, kGoldValueX, kGoldValueY,
                     kGoldValueW, kAlignLeft, kColorGold, /*shadow=*/ true);

    // §5 step 4/5 — wares / inv grids
    PaintGridColumn(dst, mbr_0x198, mbr_0x194, /*col=*/ Left,  selectedRow=mbr_0x180);
    PaintGridColumn(dst, /*other side*/      , /*…*/,  Right, /*…*/);

    // §5 step 6/7 — activate / exit button labels
    bool buyMode = (mbr_0x17c & 0x08) != 0;
    const char* activateTag = buyMode ? "BSBUY" : "BSSELL";
    DrawTextToTarget(dst, atlasHUD, Localize(activateTag),
                     kActivateBtnX, kActivateBtnY, /*w=*/ 106, kAlignCenter,
                     kColorBtnLabel, /*shadow=*/ true);
    DrawTextToTarget(dst, atlasHUD, Localize("BSEXIT"),
                     kExitBtnX, kExitBtnY, /*w=*/ 112, kAlignCenter,
                     kColorBtnLabel, /*shadow=*/ true);

    // §5 step 8 — button-child paint (inherited slot 44 on TButtonPane base)
    PaintButtonChildren(dst);                        // BuySellUp/Down/Activate/Exit
}

void TBuySellScreen::PaintGridColumn(TSurface* dst, Cell* cells, int count,
                                     Side col, int selectedRow) {
    // The 400x24 row scratch (mbr_0x19c) holds one row at a time.
    TSurface* rowRT = mbr_0x19c;
    for (int row = 0; row < count; ++row) {
        // §5 step 4: format "<name> <price>" via XREF 0x52f8dc "%s %d"
        const Cell& c = cells[row];
        char rowBuf[128];
        snprintf(rowBuf, sizeof rowBuf, "%s %d", c.displayName, c.price);
        ClearMagenta(rowRT);                         // NOTE: retail pink-halo bug
                                                     // — for port, use alpha clear
                                                     // (no chroma key) to avoid fringe
        DrawTextToTarget(rowRT, atlasHUD, rowBuf,
                         /*x=*/ 0, /*y=*/ 0, /*w=*/ 400, kAlignLeft,
                         kColorRowText, /*shadow=*/ true);
        // append per-stat row (e.g. "Damage 12 gp") — UNCONFIRMED-B per-cell-stat fields
        // …

        bool hilite = (row == selectedRow);
        int rowX  = (col == Left  ? kWaresColX : kInvColX);   // UNCONFIRMED-A
        int rowY  = kGridTopY + row * kRowH;
        if (hilite) {
            R->FillRect(dst, rowX, rowY, /*w=*/ kRowW, /*h=*/ kRowH, kColorSel);
        }
        R->DrawSurfaceToTarget(rowRT, dst, rowX, rowY);
    }
}

// vtable slot 108 == keypress (cls_0x5a5d64.cpp:290-329)
void TBuySellScreen::KeyPress(int vk, …) {
    int* base = &mbr_0x180;
    switch (vk) {
        case '1': mbr_0x188 = *base + 0; ClampAndRedraw(); break;
        case '2': mbr_0x188 = *base + 1; ClampAndRedraw(); break;
        case '3': mbr_0x188 = *base + 2; ClampAndRedraw(); break;
        case 'B': case 'V': case 'b': case 'v':
            /* fallthrough — no special handling */
            break;
        default:
            return cls_0x5b93c4::KeyPress(vk, …);     // base — handles button hotkeys
    }
}
void TBuySellScreen::ClampAndRedraw() {
    if ((int16_t)mbr_0x194 <= (int32_t)mbr_0x188)
        mbr_0x188 = (int16_t)mbr_0x194 - 1;
    Invalidate();                                    // vtable +0x44
}

// vtable slot 100 == mouse click (cls_0x5a5d64.cpp:333-382)
void TBuySellScreen::MouseClick(int button, int y, int x) {
    cls_0x5b93c4::MouseClick(button, y);             // dispatch to button children
    if (!(button == 4 || button == 5)) return;       // L/R down only
    if (y >= 200) return;                            // cell area only
    int newSel = mbr_0x180;
    if      (x >  0x2b && x <= 0x55) newSel = mbr_0x180;     // col 0
    else if (x >= 0x57 && x <= 0x81) newSel = mbr_0x180 + 1; // col 1
    else if (x >  0x82 && x <  0xae) newSel = mbr_0x180 + 2; // col 2
    else return;
    mbr_0x188 = newSel;
    PlayClick("click1");                             // _data.txt:113777
    ClampAndRedraw();
}
```

---

## §13 — Port mapping notes

Per UI_METHOD_MAP §12 (canonical port primitives):

| retail call | canonical port method | home |
|---|---|---|
| chrome blit (opaque `0x80000000`) | `DrawBitmapToTarget(bm, dst, x, y)` | renderer |
| button stamp (alpha `0x2000`) | `DrawBitmapToTarget` (alpha-aware) | renderer |
| text composite | `DrawTextShadowedToTarget(atlas, text, cellX, cellY, cellW, ETextAlign, r, g, b)` | font.cpp |
| build TTF atlas | `BuildTTFAtlas(path, px)` | font.cpp |
| compose-row scratch | `TSurface` 400×24 created via `Renderer::CreateOffscreen(400, 24, BGRA)`; composed into; then `DrawSurfaceToTarget(rowRT, dst, rowX, rowY)` | renderer + surface |
| 4-button child paint | `TButtonPane::PaintChildren` (inherited from the port's TButtonPane base — same call path as TDialogPane / TDeathPane / TLogoScreen) | uipane |
| localization tag → string | `Localize(tagId)` reading `english.def` | locale subsystem ([[project-localization]]) |
| select highlight | `FillRect(dst, x, y, w, h, ARGB)` | renderer |
| script API binding (15 commands) | dispatch table in the script-engine command registry; map each `buysell*` command to a setter on the TBuySellScreen instance (see §6 + Init) | script (out of scope for UI port) |

**Compose-to-target contract.** Every draw above goes into the **same**
`TSurface* dst` (the bs-pane render target), then the whole RT is blitted to
the HUD swapchain at panel-draw time (§3 frame composition: `screen = (0,302)`).
Do **not** mix swapchain-direct calls (`DrawBitmap` without `ToTarget`) into
this pane — the scale-drift trap (NOMENCLATURE §3 direct-renderer contract).

**Missing primitives:** none (everything maps to existing renderer/font
helpers). The script-engine binding for the 15 `buysell*` commands needs a
small adapter layer in the script subsystem (out of scope for UI).

---

## §14 — UNCONFIRMED / open questions

### A. **Paint body `virt_meth_0x52f7d0` not extracted** (LOAD-BEARING)

**What I tried.** Searched `recon/classes/`, `recon/classes_original/`,
`recon/ghidra/`, `recon/discovered/` for any file containing the body of
`0x52f7d0`. The vtable file lists the slot (`cls_0x5a5d64__vftable_5a5d64.cpp:28`)
and the wrapper `virt_meth_0x52f7a0` calls it (`cls_0x5a5d64.cpp:282`), but
no decompiled body for `0x52f7d0` exists in any recon folder. Evidence for
the call sequence comes from the **string-XREF map at `_data.txt:113741..113773`**
(10 string anchors at addresses `0x52f88d/8bc/8c8/8dc/9c9/9ef/a2e`), which is
authoritative for the ORDER but not for the per-call coordinate literals.

**Impact if wrong.** Without the body: every (x,y) and (w,h) literal in §4
and the gold-strip / button-label cell rects in §8 are UNKNOWN. The
implementer cannot lay out the panel pixel-accurate without these.

**Resolve.** `DecompileAddr.java 0x0052f7d0` then save to
`recon/discovered/cls_0x5a5d64_TBuySellScreen_Paint_52f7d0.cpp`. If Ghidra
drops branches as "unreachable" (a known pattern in these complex paint
bodies), also dump the disassembly: `Disasm.java 0x0052f7d0..0x0052fa50`. The
load-bearing decode targets are:
- The chrome blit dst (probably (0,0); confirm via the first `FUN_004bd680`
  or `+0x5c` blit args).
- The gold-strip rect (the `0x52f8bc`+`0x52f8c8`+`0x52f8dc` area — the cell
  rect feeding `FUN_004be2b0`).
- The two grid origins + cell pitches (cross-check against the mouse hit-test
  x-ranges `0x2b/0x55/0x57/0x81/0x82/0xae` in §10.3).
- The activate/exit button positions + their per-label text rects.
- The font index passed (which controls shadow bit `0x400` + HDC table index).
- The text color packing (typically white `0xffffff` or cream `0xdfd7a8`).

### B. **`FUN_0052da90` body not extracted** (the price-compute helper)

**What I tried.** `grep -rn "FUN_0052da90 = " recon/ghidra/ recon/discovered/`
returns no body. Only call-site evidence (10+ XREFs in `cls_0x530af0`,
`cls_0x531fc0`, `cls_0x531b70`, `cls_0x5a5d64`) and the per-stat-label XREF
map at `_data.txt:113053..113700+` (s_Value at 113053; alternating
BSARM*/BSWEA*/BSFOOD* tag + English-label pairs through 113700+).

**Impact if wrong.** The 72-byte cell record's field layout is the load-
bearing data for §6.3 / §8 per-row stat rendering. Without it the renderer
can show name + price but not the per-stat sub-label (Damage / Protection /
Min Strn) — which is the meaningful "what does this item give me" hint a
player needs to compare items.

**Resolve.** `DecompileAddr.java 0x0052da90`. The expected behavior: switch
on item-class index (TObjectClass → switch on classId 0..7+), per branch
read 1-2 stats (e.g. weapon: Damage + Min Strength; armor: Protection +
Rst Poison + Stlth + Min Strn + Min Cons), compute price = Value * mode-
modifier (+markup for Buy, −discount for Sell), pack (itemPtr, displayName,
classTier, price, stat1_tag, stat1_value, stat2_tag, stat2_value) into the
stack-out buffer.

### C. **Player.Gold() vtable getter is UNCONFIRMED.**

The "GOLD %d" / BSGOLD display sources from somewhere on the player object
(`DAT_00667fcc`). The standard pattern is a getter at `+0x84` (`(*player+0x84)(s_Gold)`
similar to InventoryPane's "Gold" text). UNCONFIRMED until §14-A pins the
exact call site. **Resolve:** read the paint body around `0x52f8bc`-`0x52f8e0`.

### D. **Button geometry source** (cls_0x5a3c68 `mbr_0x9c` rect blob).

The 4 buttons are positioned via a rect-like blob read from `mbr_0x9c`
(`cls_0x5a3c68.cpp:171-177`). Where this blob is populated (a bitmap header
field? a per-button DEF entry? a hardcoded table in `0x52f7d0`?) is
UNCONFIRMED. **Resolve:** trace `mbr_0x9c` writes; `DecompileAddr 0x42c500`
to confirm ctor's read of pdVar3[0..3].

### E. **Mouse hit-test x-ranges (43..85, 87..129, 130..174)** in
`virt_meth_0x52fd50` (`cls_0x5a5d64.cpp:347-378`) — these may be (a) three
**category-tab buttons** in the top strip, (b) three **scroll-bar zones**, or
(c) three **page-step buttons**. The exact role is UNCONFIRMED. Each
hit-region maps to `mbr_0x188 = mbr_0x180 + n` where n∈{0,1,2}. **Resolve:**
extract the paint body and look for tabs / buttons drawn at these x-ranges in
the top 200 px.

### F. **Visibility predicate.** No clean "Show / Hide" XREF found yet. The
flag `mbr_0x50` is the visibility gate; the script command `buysellscreen`
sets it to 1. The Reset path (`meth_0x530600`) does NOT clear `mbr_0x50` —
the actual hide is presumably in `LAB_00530570` (BuySellExit callback) or in
the `buysell` wait-condition release. **Resolve:** extract `0x530570` body.

### G. **`cls_0x531b70` vs `cls_0x531fc0` mode permutation.** Both walk
`mbr_0x1b4` wares list with identical filter logic; the only difference is
that cls_0x531b70 passes the cell scratch as the first arg to FUN_0052da90.
Which one runs in which buy/sell + SaleType combo is UNCONFIRMED. **Resolve:**
extract the paint method (§14-A) and look for which sub-pane it dispatches
to.

### H. **Pane positioning verification.** Ctor sets `mbr_0x4=0`,
`mbr_0x8=0x12e=302`. Bottom-anchored to play screen (302+178=480). But this
puts the right edge at x=452 in a 640-wide framebuffer — leaving 188 px of
right-sidebar HUD visible behind the panel? Or does some Show / Setup helper
reposition the pane center-bottom? **Resolve:** trace the `buysellscreen`
script-command implementation (the dispatcher at `_data.txt:59304+`) to
see if it calls a Show with explicit (x, y) overrides.

### I. **External coin glyph (medgold.dat / smallgold.dat).** These exist as
data files (`data/resources_unzipped/medgold*.dat` + `smallgold*.dat`,
verified all CGSR format) but are not in TBuySellScreen's XREF graph. They
may be **dialog/inventory-pane** assets (`grep "medgold" recon/` returns no
XREF from `0x52f7d0`). If the §14-A extraction reveals a `FUN_0046d710("medgold")`
or similar call inside the paint body, this becomes another asset to dump.
**Resolve:** confirm during §14-A extraction.

---

## Self-verification

- Frame table re-checked: pane is BL-anchored to play screen (`mbr_0x4=0`,
  `mbr_0x8=302`, `mbr_0x10=178` → bottom edge = 480 = screen-h). ✓
- Asset roster: 9 entries from buysell.dat dumped + measured (chrome 452×178
  matches pane size). ✓
- Button hotkeys decoded as Win32 VK codes: VK_UP=0x26, VK_DOWN=0x28,
  'A'=0x41, 'E'=0x45. ✓ Matches BuySellUp/Down/Activate/Exit semantics.
- Cell record size: 0x48=72 bytes confirmed by the 18-iteration copy loop in
  `cls_0x530af0.cpp:97-103` (`for (iVar4=0x12;…) *puVar5++=*puVar10++`). ✓
- Scratch surface size: 400×24 (=0x190 × 0x18) confirmed at
  `cls_0x5a5d64.cpp:234`. ✓
- BS-tag → english.def mapping verified via `grep "^BS" english.def`. ✓
- 15 script-API commands enumerated and tied back to `_data.txt:59040..59326`. ✓
- The paint body NOT being extracted is honestly flagged as the load-bearing
  UNCONFIRMED-A — without it the (x,y) literals for §4 / §8 cells / §10.3
  column zones must be filled in by §14-A extraction before reconstruction can
  begin pixel-accurate.

**Could a stranger rebuild this panel pixel-accurate from this spec alone?**
Not yet — the §14-A extraction blocks pixel-accurate layout. They can build
the asset roster, the script-API plumbing, the mode-flag state machine, the
cell-record growth loop, the keypress handler, and the localization-tag
pipeline. But the chrome / gold-strip / grid / button rects all sit behind
§14-A. The reconstruction agent must escalate §14-A as a forensics blocker
before committing pixel coords.
