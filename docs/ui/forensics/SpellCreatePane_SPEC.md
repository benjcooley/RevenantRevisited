# TSpellPane — Reconstruction SPEC (Spell creation / talisman composer)

> Pixel-faithful reconstruction spec for the in-game HUD **spell creation pane** —
> the lower sidebar "Spell" tab where the player **assembles spells by selecting
> talismans** and invokes them. Distinct from `SpellbookPane` (the upper sidebar
> spell-list scroll) and `TQuickSpellPane` (the bottom-bar 4-slot quick-cast
> ring). Follow FORENSICS_PROTOCOL §0–§14 template. All coordinates name their
> frame; every literal is cited `file:line` with hex(=dec).
>
> **STATUS:** `forensics-skeleton` — class identity + assets + envelope confirmed
> this wave; element layout / draw algorithm / animation / input dispatch still
> to be traced in a later forensics pass.

---

## §0 — Sources & status

**Class:** `TSpellPane` = `cls_0x5a5978` (vtable @ `0x5a5978`); global instance @
`0x6661b0`. Naming **98% confirmed** — the snapshot `src/spellpane.h:62-66`
defines `TSpellPane : public TButtonPane` with the EXACT comment "This pane is
where the user assembles spells with various talismans and invokes them"; rect
ctor `TButtonPane(MULTIPANEX, MULTIPANEY, MULTIPANEWIDTH, MULTIPANEHEIGHT)`
matches the retail ctor literal pattern (sidebar lower-region MULTIPANE slot),
and the asset roster (12 talisman icons + SpellInv/SpellBack/SpellArw/SpellName/
SpellTal buttons + `spellconstr` chrome) maps 1:1 to TSpellPane's snapshot API
(`AddTal/RemoveTal/Invoke/Scroll/ToggleTalismanNames`). The earlier port-status
file labels `cls_0x5a5978` as "QuickSpellSidebarPane" (placeholder) — that label
is **superseded** by this identification.

**Key method addresses (vtable on `cls_0x5a5978`, 46 slots — TButtonPane shape):**
| role | addr | source |
|---|---|---|
| Initialize (slot 0) | `0x5432a0` | `recon/ghidra/cls_0x5a5978.cpp:109-200` |
| ctor | `0x488460` | `recon/ghidra/cls_0x5a5978.cpp:30-51`; `recon/discovered/cls_0x5a5978_QuickSpellSidebarPane_ctor_488460.cpp:1-22` |
| Draw (slot 20) | `0x5435c0` | `cls_0x5a5978.cpp:204-215`; `cls_0x5a5978__vftable_5a5978.cpp:27` |
| **DrawBackground / chrome (slot 84)** | `0x5435f0` | xrefs in `_data.txt:115840,116182,116186,116191,116195,116200,116206,116211` — paints chrome + talisman row using `spell pouch`/`spellpouch`/`%scandy`/`SpellNameU/D`/`SpellTalU/D`; body NOT YET EXTRACTED into recon/discovered |
| (slot 100) `virt_meth_0x543b40` | `0x543b40` | `cls_0x5a5978__vftable_5a5978.cpp:32`; uses "Code" stat key (`_data.txt:116225`) |
| Button-click dispatch (slot 104) | `0x543c40` | `cls_0x5a5978.cpp:219-240` — checks `param_1==1` and the hovered/selected talisman index |
| Click-target resolve | `0x543ca0` | xref from `0x543c40:232`; uses `spell pouch`/`spellpouch` strings |
| Helper `FUN_00542640` | `0x542640` | uses asset `SPLOUTTAL` (`_data.txt:116086`) — likely talisman pouch tooltip / count helper |
| Helper `FUN_00543e40` | `0x543e40` | uses "Code" stat key — talisman→spell resolution path |

**Recon files read:**
- `recon/ghidra/cls_0x5a5978.cpp` (ctor, dtor, Initialize, Draw, click handler)
- `recon/ghidra/cls_0x5a5978__vftable_5a5978.cpp` (46-slot vtable)
- `recon/discovered/cls_0x5a5978_QuickSpellSidebarPane_ctor_488460.cpp` (ctor copy)
- `recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp:220-223`
  (Initialize call site + error string "Trouble initializing spell pane")
- `recon/discovered/port_status/QuickSpellSidebarPane.md` (prior placeholder label)
- `recon/ghidra/_data.txt:116080-116228` (the full `spellconstr/SpellInv/SpellBack/
  SpellArw/SpellName/SpellTal/spell pouch/spellpouch/%scandy/Code` string region)
- `src/spellpane.h:15-102` (snapshot TTalismanButton + TSpellPane definitions —
  the snapshot is pre-release but the comment + method shapes anchor the identity)
- `src/spellpane.cpp:20-100+` (snapshot Old[] talisman name table + TTalismanButton
  Draw/AddTalisman/Backspace bodies)
- `src/revdefs.h:142-145` (`MULTIPANEX/Y/W/H` = `(459, 346, 168, 128)` snapshot;
  retail tunes to `(452, 306, 188, 174)`)
- `src/spell.h` (talisman/spell data model — referenced by ctor's `TalismanClass`)
- `docs/ui/forensics/SpellbookPane_SPEC.md` (sibling — out of scope for this spec;
  shares sidebar mode-switch dispatcher)
- `docs/ui/forensics/QuickSpellPane_SPEC.md` (sibling — out of scope; lives in
  bottom bar, not the sidebar)
- `docs/ui/forensics/TSideTabsPane_SPEC.md` (parent dispatcher that selects which
  lower-region pane is active)

**Prior briefs:** `docs/ui/briefs/B_r11_sidebar_tab_cascade.md` (referenced by
QuickSpellSidebarPane.md); no dedicated SpellPane brief exists.

**Method-map references:** UI_METHOD_MAP §4 (sprite stamp), §8.5 (BlitEffect
flush), §15a (surface vtable) — same primitives as siblings; no new gap
identified from envelope inspection (slot-84 body still to extract).

**Errors to close (existing port):** snapshot `src/spellpane.{h,cpp}` exists but
is **not wired into the modern boot** (TSpellPane is forward-decl only in the
current TPlayScreen path). The snapshot Draw layout uses snapshot-era literals
that are NOT retail-faithful; retail must drive coordinates.

**Snapshot vs retail (per [feedback-snapshot-vs-retail]):**
- **Identity:** retail vtable `cls_0x5a5978` ↔ snapshot `TSpellPane` (`src/spellpane.h:62`).
  Match strength: comment + method-name + asset-vocab + rect-family all align.
- **Behavior parity (~70% confident):** snapshot defines 12 talisman buttons +
  a quickspell pouch + Invoke/Backspace/AddTalisman; retail has those same 12
  talisman icons + Invoke + Backspace ("back") + the same talisman-add flow
  (per `Old[]` table at `src/spellpane.cpp:22-23`).
- **Coordinate parity (LOW):** snapshot uses `MULTIPANEX/Y/W/H = (459,346,168,128)`;
  retail uses `(452,306,188,174)` — DIFFERENT. Treat snapshot coords as
  **non-binding**. Retail ctor literals are the source of truth.
- **Asset parity (HIGH):** snapshot `Old[]` lists 12 talisman names matching retail's
  12 `S*` icons (`SSun/SLife/SOcean/SLaw/SSoul/SStars/SDeath/SChaos/SSky/SEarth/SWard/SMoon`).

---

## §1 — Overview

The **spell creation pane** is the lower right-sidebar tab where the player
**composes a spell by selecting talismans**. The composer holds a buffer of up to
`SPELLSIZE` talisman characters; the player adds talismans by clicking icons (12
shown), removes them with the "back" button, invokes the assembled spell with
"Invoke" (or "min"-ifies / pouches it), and pages through saved compositions via
the up/down arrow buttons. Spell name and current talisman row display the live
composition. Successful invocation casts the spell via the script/spell engine
(spell.def is the spell rules database, NOT a UI def — UI side just emits the
talisman string and calls `Invoke`).

**Instance:** exactly one (`0x6661b0`); the right sidebar's lower region cycles
through TInventory / TEquipPane / **TSpellPane** / TStatPane via `TSideTabsPane`
button dispatch (`DAT_0065d1bc == 2` selects this pane per `QuickSpellSidebarPane.md:19`).

**When shown:** active when the lower-sidebar mode = 2 (the "Spell" tab on the
sidebar's bottom button group); paired in dispatcher cases 7 and 0xa with the
upper "Book" tab, so the "show me my spells" hotkey lights up both spellbook
(upper) + composer (lower) at once.

**Plain language:** Picture a parchment-coloured panel about the size of an
inventory window pinned to the right-hand sidebar. Across the top is the spell
name and a row of 12 small symbol icons — each is a talisman the player can drop
into the spell. The middle area is the "spell pouch" — the talismans the player
has currently typed/selected to make a spell. Buttons along the bottom edge
include "Invoke" (cast), "back" (delete last talisman), up/down arrows
(presumably page or scroll), and a "min" toggle (minimise / collapse names).

**Relationship to siblings:**
- `SpellbookPane` (`cls_0x5a5ae8`, upper sidebar): shows the player's **known**
  spells. The composer (TSpellPane) is **separate** — it's the workshop.
- `TQuickSpellPane` (`cls_0x5a5a30`, bottom bar): the 4-slot **quick-cast** ring;
  TSpellPane likely populates / drives one of those 4 slots (the snapshot
  `TTalismanButton` has a `quickspellid` field tying composed spells to a slot).
- `spell.def`: the spell-rules database (NAME, ICONNAME, TALISMANS, MANA, …) —
  **not** a UI DEF; the composer's "did this combo match a known spell?" lookup
  reads spell.def-loaded data via the script engine.

---

## §2 — Asset roster

Archive: `spellpane.dat` (25 entries). Measured with `tools/ui/dump_dat.py` →
`/tmp/dat_dump/spellpane.json`.

| asset | idx | WxH | regx,regy | flags | kc | role | cite |
|---|---|---|---|---|---|---|---|
| **spellconstr** | 0 | **188×174** | 0,0 | `0x2` BM_15BIT | `0x0` | full pane chrome / background (= ctor w/h) | `_data.txt:116096`; dump idx0 |
| **SpellInvD** | 1 | 142×36 | 0,0 | `0x2` | `0x0` | Invoke button — DOWN state | dump idx1; `_data.txt:116100` |
| **SpellInvU** | 2 | 142×36 | 0,0 | `0x2` | `0x0` | Invoke button — UP state | dump idx2; `_data.txt:116100` |
| **SpellBackD** | 3 | 22×20 | 0,0 | `0x2` | `0x0` | Back / backspace button — DOWN | dump idx3; `_data.txt:116119` |
| **SpellBackU** | 4 | 22×20 | 0,0 | `0x2` | `0x0` | Back / backspace button — UP | dump idx4; `_data.txt:116114` |
| **SpellArwDD** | 5 | 20×26 | 0,0 | `0x2` | `0x0` | Arrow-down DOWN | dump idx5; `_data.txt:116153` |
| **SpellArwDU** | 6 | 20×26 | 0,0 | `0x2` | `0x0` | Arrow-down UP | dump idx6; `_data.txt:116148` |
| **SpellArwUD** | 7 | 20×26 | 0,0 | `0x2` | `0x0` | Arrow-up DOWN | dump idx7; `_data.txt:116138` |
| **SpellArwUU** | 8 | 20×26 | 0,0 | `0x2` | `0x0` | Arrow-up UP | dump idx8; `_data.txt:116133` |
| **SpellNameD** | 9 | 18×18 | 0,0 | `0x2` | `0x0` | Spell-name toggle/button — DOWN | dump idx9; `_data.txt:116172,116200` |
| **SpellNameU** | 10 | 18×18 | 0,0 | `0x2` | `0x0` | Spell-name toggle/button — UP | dump idx10; `_data.txt:116167,116195` |
| **SpellTalD** | 11 | 18×18 | 0,0 | `0x2` | `0x0` | Talisman-names toggle — DOWN | dump idx11; `_data.txt:116211` |
| **SpellTalU** | 12 | 18×18 | 0,0 | `0x2` | `0x0` | Talisman-names toggle — UP | dump idx12; `_data.txt:116206` |
| **SStars** | 13 | 32×32 | 0,0 | `0x104` BM_16BIT+BM_ALPHA | `0x0` | Stars talisman icon (alpha-keyed) | dump idx13 |
| **SLaw** | 14 | 32×32 | 0,0 | `0x104` | `0x0` | Law talisman icon | dump idx14 |
| **SLife** | 15 | 32×32 | 0,0 | `0x104` | `0x0` | Life talisman icon | dump idx15 |
| **SSky** | 16 | 32×32 | 0,0 | `0x104` | `0x0` | Sky talisman icon | dump idx16 |
| **SChaos** | 17 | 32×32 | 0,0 | `0x104` | `0x0` | Chaos talisman icon | dump idx17 |
| **SDeath** | 18 | 32×32 | 0,0 | `0x104` | `0x0` | Death talisman icon | dump idx18 |
| **SSoul** | 19 | 32×32 | 0,0 | `0x104` | `0x0` | Soul talisman icon | dump idx19 |
| **SSun** | 20 | 32×32 | 0,0 | `0x104` | `0x0` | Sun talisman icon | dump idx20 |
| **SOcean** | 21 | 32×32 | 0,0 | `0x104` | `0x0` | Ocean talisman icon | dump idx21 |
| **SMoon** | 22 | 32×32 | 0,0 | `0x104` | `0x0` | Moon talisman icon | dump idx22 |
| **SEarth** | 23 | 32×32 | 0,0 | `0x104` | `0x0` | Earth talisman icon | dump idx23 |
| **SWard** | 24 | 32×32 | 0,0 | `0x104` | `0x0` | Ward talisman icon | dump idx24 |

The 12 `S*` icons map 1:1 to the snapshot `Old[]` table (`src/spellpane.cpp:22-23`)
which lists `{"Sun","Life","Ocean","Law","Soul","Stars","Death","Chaos","Sky",
"Earth","Ward","Moon"}` — same 12 talismans the spell.def recipes reference (per
`spell.def:14-26`).

**String anchors used at runtime:** `"spell pouch"` (`_data.txt:116182`),
`"spellpouch"` (`:116186`), `"%scandy"` (`:116191`), `"Code"` (`:116225`) —
all referenced by Draw `0x5435f0` and click `0x543c40`. `"Code"` is the
TalismanClass stat key (matches snapshot `src/spellpane.cpp:50`
`TalismanClass.GetStat(t,"Code")`).

---

## §3 — Coordinate frames & surfaces

### Pane rect (ctor)

`cls_0x5a5978.cpp:32-43`:

| field | ctor value | meaning | cite |
|---|---|---|---|
| `mbr_0x4` (x) | `0x1c4 = 452` | pane x | `cls_0x5a5978.cpp:35` |
| `mbr_0x8` (y) | `0x132 = 306` | pane y | `cls_0x5a5978.cpp:37` |
| `mbr_0xc` (w) | `0xbc = 188` | pane width | `cls_0x5a5978.cpp:39` |
| `mbr_0x10` (h) | `0xae = 174` | pane height | `cls_0x5a5978.cpp:42` |
| `mbr_0x14/0x18/0x1c/0x20` | mirrors | template x/y/w/h | `cls_0x5a5978.cpp:36,38,40,43` |

Rect @ Classic 640×480: **(x=452, y=306, w=188, h=174)** — the **right-sidebar
lower MULTIPANE slot** (same family as InventoryPane / EquipPane / StatPane /
SpellbookPane — `docs/ui/forensics/InventoryPane_SPEC.md`, etc.). NOT bottom-anchored
in the same sense as `TBottomBarPane`; this pane is **right-edge-anchored**
(x = display_w − 188 at any width) at a fixed pane-relative position.

> Why "bottom pane" in the user clue: this is the **lower** sidebar slot in the
> right-hand column (sidebar splits into upper region @ y≈12 and lower region @
> y≈306). It's the bottom HALF of the SIDEBAR, NOT the bottom bar that hosts
> QuickSpell + BarInv. The "bottom" in the clue refers to vertical band within
> the sidebar column, not the bottom command/status strip.

### Frame table — to fill in next forensics pass

(UNCONFIRMED — placeholders pending slot-84 extraction):

| frame | parent | anchor | origin (formula + literal) | what is expressed | cite |
|---|---|---|---|---|---|
| screen | framebuffer | TL | (0,0) | final pixels | — |
| sidebar-region | screen | TR (right-edge anchored) | `x = display_w − 188`; `y = 306` (or display-h-anchored, TBD) | the whole pane | `cls_0x5a5978.cpp:35,37` |
| chrome-draw | sidebar-region | TL | sidebar-local `(0,0)` | spellconstr background | TBD (slot 84) |
| talisman-row | sidebar-region | TL | TBD (12 icons × 32px) | the 12 selectable talismans | TBD |
| spell-pouch | sidebar-region | TL | TBD | currently-composed talisman buffer | TBD ("spell pouch" anchor at slot 84) |
| button-row | sidebar-region | TL | TBD (Invoke / Back / Arrows / Tal toggle / Name toggle) | command buttons | partial — see §10 |

### Button positions FROM Initialize (`0x5432a0`)

Initialize wires 5 buttons by calling `cls_0x5a3c68::cls_0x5a3c68(...)` (a
`TDefButton`-like ctor) — the literal `(x,y,w,h)` args from the decomp:

| # | sprite anchor | (x, y, w, h) pane-local | command | callback | cite |
|---|---|---|---|---|---|
| 1 | `s_spell_005e5100` (set 0x18,0x88, w=0x8e=142, h=0x24=36) | (0x18=24, 0x88=136, 142, 36) | (none) | `LAB_00542d50` | `cls_0x5a5978.cpp:136-138` |
| 2 | `DAT_005e5120` = "back" | (0xa4=164, 0x8f=143, 0x16=22, 0x14=20) | (none) | `LAB_00542e30` | `cls_0x5a5978.cpp:152-154` |
| 3 | `DAT_005e5140` = "up" | (0xa6=166, 0x52=82, 0x14=20, 0x1a=26) | hotkey 8 | `LAB_00542f00` | `cls_0x5a5978.cpp:164-166` |
| 4 | `DAT_005e515c` = "down" | (0xa6=166, 0x70=112, 0x14=20, 0x1a=26) | hotkey 8 | `LAB_00542ed0` | `cls_0x5a5978.cpp:176-178` |
| 5 | `DAT_005e517c` = "min" | (0xa6=166, 0x25=37, 0x12=18, 0x12=18) | (none) | `LAB_00542f30` | `cls_0x5a5978.cpp:188-190` |

So pane-local **button layout (right column @ x=164-166)**:
- "min" toggle @ (166, 37) 18×18
- "up" arrow @ (166, 82) 20×26
- "down" arrow @ (166, 112) 20×26
- "back" button @ (164, 143) 22×20

And spell name display @ (24, 136) 142×36 (the long Invoke / spell-name bar).

This gives a sidebar layout: top section = talisman icons (12 of 32×32 plus
"Code"-driven labels), middle = spell pouch composition area, right column =
the 4-button vertical strip, bottom = the wide spell-name / Invoke bar.

---

## §4–§13 — TBD (next forensics pass)

This skeleton spec confirms class identity, assets, ctor rect, and button
positions. The following sections need a follow-on forensics pass that extracts:

- **§4 Static element layout** — the 12 talisman icons' (x,y) positions on the
  pane; the "spell pouch" composition cells (likely 11 cells based on snapshot
  `SPELLSIZE`); the spell-name text region.
- **§5 Draw order** — extract `virt_meth_0x5435f0` (slot 84) body to determine
  composition order of: chrome → talisman strip → pouch → name → buttons.
- **§6 Algorithms** — talisman-icon lookup (the `%scandy` format-string is used
  to derive an asset name per Code — likely "S<letter>candy" or per-talisman
  cursor sprite); spell-recipe match (TalismanClass.GetStat "Code" lookup);
  Invoke-trigger flow into spell.cpp.
- **§7 Effects & shadows** — talisman icons are BM_ALPHA (kc=0); chrome is
  opaque (kc=0); text shadow scheme TBD.
- **§8 Text rendering** — spell name + talisman names (when "Tal" toggle on) +
  any tooltip text. Font + cell + 3-pass shadow flags to extract.
- **§9 Animation** — pouch fill/empty (likely instant), button down-states.
- **§10 Input & dispatch** — full hit-rects for the 12 talisman icons +
  pouch-area click-to-remove + the 5 already-known buttons. Click handler
  body `0x543c40` already partly decoded (see §0); the callbacks
  `LAB_00542d50/e30/f00/ed0/f30` need decompile.
- **§11 Retail bugs** — none known yet.
- **§12 Reconstruction pseudocode** — pending §4-§10.
- **§13 Port mapping** — pending §4-§10.

### §14 — UNCONFIRMED / open questions

1. **DrawBackground (slot 84) body.** Currently inferred from string-anchor
   xrefs only — body NOT extracted to `recon/discovered/`. **Resolve:**
   `DecompileAddr.java 0x005435f0` and write
   `recon/discovered/cls_0x5a5978_TSpellPane_DrawBackground_5435f0.cpp`.
2. **Talisman icon layout.** The 12 `S*` icons must be laid out in a grid or
   row on the pane; positions not in Initialize (so they're emitted by
   DrawBackground using an index loop). **Resolve:** the slot-84 extraction.
3. **Spell pouch cell count + size.** `SPELLSIZE` is snapshot-only — retail's
   max talisman count not yet read from the decomp. **Resolve:** look for the
   loop bound in `0x5435f0`; also dump TalismanClass to count types.
4. **`%scandy` format string usage.** Used to format an asset name per talisman.
   Likely `"<one-char>candy"` — possibly a cursor / drag-talisman sprite name.
   **Resolve:** trace `:00543779` and `:00545988` call sites.
5. **Coordinate parity at non-Classic widths.** Snapshot uses
   `MULTIPANEX/Y/W/H = (459,346,168,128)`; retail `(452,306,188,174)`.
   Right-anchor formula likely `x = display_w − 188` — confirm by tracing
   SetRect / relayout path.
6. **Mode-switch wire-in.** Dispatcher cases 7/0xa pair lower="Spell" with
   upper="Book" (`QuickSpellSidebarPane.md:25`). Where is the lone "Spell"
   tab-button click handled? **Resolve:** TSideTabsPane dispatcher.
7. **"min" button function.** DAT="min" — likely "minimise names" (snapshot
   `TSpellPane::ToggleTalismanNames`). Confirm by tracing `LAB_00542f30`.
8. **Snapshot ↔ retail behavior diff.** Snapshot's `clickedtal/onclickedtal`
   drag-talisman state — does retail still have drag? **Resolve:** trace
   slot-104 `virt_meth_0x543c40` body fully + MouseMove (slot 108 thunk?).
9. **`SPLOUTTAL` asset.** Used by `FUN_00542640` (`_data.txt:116086`). Possibly
   "Spell Out Talisman" — a no-such-talisman warning sprite? **Resolve:**
   grep for it in spellpane.dat (it's NOT in the 25-entry list — likely a
   string, not an asset name; verify).

---

## Negative findings

- **No separate spell-creation `.def` file exists.** `spell.def` is the spell
  rules database (NAME/ICONNAME/TALISMANS/MANA/DAMAGETYPE/ANIMATION/VARIANT) —
  it is NOT a UI DEF and is not loaded as a screen. The composer UI is
  **bitmap-driven** via spellpane.dat (no DEF), unlike OOG screens
  (LoadGameDef/SaveGameDef/InGameMenuDef/OptionsDef/SelStartDef) which are
  DEF-driven.
- **No bottom-bar host involvement.** The composer is **not** a sibling of
  QuickSpellPane or BarInvPane on the bottom bar — its rect places it in the
  right-sidebar lower MULTIPANE slot. The earlier coverage gap was that the
  prior wave mis-labelled `cls_0x5a5978` as a generic "QuickSpellSidebarPane"
  instead of TSpellPane (the talisman composer).
- **The composed-spell ↔ quick-cast bridge** exists: snapshot `TTalismanButton`
  holds a `quickspellid` field (`src/spellpane.h:52`), and `Player->SetQuickSpell`
  is called on each Add/Backspace (`src/spellpane.cpp:84`). So the composer
  writes through to the bottom-bar 4-slot ring — explains why some prior code
  conflated them. They are still **separate classes / panes / files**.
