# TBuySellScreen — Reconstruction SPEC (the shop)

> The shop: a merchant's wares (or the player's sellable items) listed three
> rows at a time in the bottom drawer of the play screen, with the player's
> gold, a Buy/Sell button and an Exit button. Scripts fill and open it
> through the `buysell*` commands ([COMMAND_SYSTEM.md](../../gameflow/forensics/COMMAND_SYSTEM.md) §6.6).
>
> Produced per [FORENSICS_PROTOCOL.md](FORENSICS_PROTOCOL.md). Terms per
> [NOMENCLATURE.md](NOMENCLATURE.md). Primitives per [UI_METHOD_MAP.md](UI_METHOD_MAP.md).

---

## §0 — Sources & status

**Status: `forensics-complete`** (2026-10-05, Ghidra decomp + disassembly of
`Revenant.exe` 1.22). Every coordinate, colour, font and flag below is a
literal from the paint body `0x0052f7d0` and the row draw `0x0052f040`,
cross-checked against the disassembly where the decompiler mangles stack
arguments. Port: `src/buysell.{h,cpp}` (`TBuySellPane`), the drawer in
`TPlayScreen`, the commands in `src/cmd_buysell.cpp`.

**Class identity.** `TBuySellScreen = cls_0x5a5d64`, vtable `0x005a5d64`,
one global instance at **`0x0065a3b8`** (0x1b8 bytes), constructed by
`0x00488ef0` with rect (0, 302, 452, 178). A TButtonPane (the DEF pane family:
initialize `0x00434e40`, button draw `0x00435de0`, key dispatch `0x004361f0`,
mouse `0x00436530` / `0x00436660`).

**Decomps** (`recon/discovered/buysell/`, file names carry the address):

| Address | Role | vtable slot |
|---|---|---|
| `0x0052f390` | Initialize (also `buysellinit`) | 0 |
| `0x0052f6f0` | Close | 1 |
| `0x00530600` | Reset (drawer close) | 13 |
| `0x0052f7a0` | Draw wrapper: paint, then the button pane's draw | 20 |
| `0x0052f7d0` | **Paint** (+ `.asm`) | 21 |
| `0x0052f040` | Draw one row (+ `.asm`) | — |
| `0x0052fd50` | MouseClick | 25 |
| `0x0052fca0` | MouseMove | 26 |
| `0x0052fa60` | KeyPress | 27 |
| `0x0052fb30` | Joystick | 29 |
| `0x0052fe90` / `0x0052fee0` | BuySellUp / BuySellDown callbacks | — |
| `0x0052ff40` | BuySellActivate callback: buy or sell (+ `.asm`) | — |
| `0x00530570` | BuySellExit callback | — |
| `0x0052da90` | **BuildItem**: one row record from a type name (the price helper; + tail `.asm`) | — |
| `0x00530670` / `0x00530af0` | Add (by name) / AddCriteria (stock) | — |
| `0x00531d70` / `0x00531b70` / `0x00531fc0` | AddBuyItem / AddBuyItems / AddBuyCriteria (the customer's items); the Ghidra bodies lose their loops, the OOAnalyzer ones in `recon/classes/cls_0x5a5d64.cpp`, `cls_0x531b70.cpp`, `cls_0x531fc0.cpp` are whole | — |
| `0x00532210` / `0x00532340` / `0x005321f0` | Remove (by name) / RemoveCriteria / RemoveBuyCriteria (= RemoveCriteria) | — |
| `0x00532f40` | Clear the rows (LoadGame) | — |
| `0x00532fb0` / `0x00532fe0` / `0x00533040` | SetSalesperson / SetPurchaseDialog / SetNoGoldDialog | — |
| `0x0052f310` | Free one record's strings | — |
| `0x005330a0` | Delete one inventory item by name (+ `.asm`) | — |
| `0x0048cce0` | Expand a STATLINE into text | — |
| `0x0046e7f0` | An item's display name | — |
| `0x0048cb50` | TRules: the WEAPON.DEF / ARMOR.DEF entry of a type | — |
| `0x0048ac30` / `0x0048b1a0` | TRules: WEAPON / ARMOR loaders (BASICMODS offsets) | — |
| `0x0047b4d0` | TPlayScreen Pulse: the drawer (`recon/discovered/cls_0x5a5320_TPlayScreen_Pulse_47b4d0.cpp`, open path `.asm` here) | — |

**Corrections to the previous version of this spec** (inferred before the
paint body was decompiled): the panel is one list, not two grids; `+0x1b4` is
the customer, not a wares list; `+0x17c` bit 1 is Buy and bit 2 Sell (not 8 /
4); the mouse "column" ranges are row bands in y; `0x00530af0` searches the
object classes, not the player's inventory; the BS* labels are not empty in
english.def; there is no "GOLD %d" fallback; the shop is not a modal and does
not pause the world (§1); the Activate button's label is Buy *or* Sell, chosen
by the shop type.

## §1 — Overview

A script fills the shop and opens it (`town.s`, Elahni's potions):

```
buysellinit
buysellsalesperson elahni1
buysellnogolddialog II22ELA02
buysellpurchasedialog II22ELA03
choice buy1 BSBUY2 / choice sell1 BSSELL2 / choice stop1 BSEXIT2 / wait response
:buy1
buysellshoptype buy misc
BuySellAdd "Lesser Healing"   (… ten potions)
buysellscreen
wait buysell
jump start1
```

**It is PlayScreen's bottom drawer, not a modal.** `buysellscreen` sets
PlayScreen `+0x6b8`; PlayScreen's pulse (`0x0047b4d0`) then re-initializes the
shop and switches the drawer (`+0x6c0`) to mode 3: the HUD panes in the
drawer (bottom bar `0x0065b638`, belt `0x0065b028`, quick spells `0x0065c6f8`)
are removed, the text bar is hidden (`TTextBar::Hide` `0x0054c9c0`), the side
panel is opened, and the shop is added as an ordinary pane (`AddPane(shop,
-1)`) at the bottom left, 178 px tall. The world keeps running; the shop
scripts turn control off first, so the player can't walk away. No modal
flags, no pause.

Exit (`0x00530570`) clears the shop (`+0x1b0 = 0`, the rows freed) and
PlayScreen `+0x6b8`; the next pulse closes the drawer: `Reset` the shop,
`RemovePane`, show the text bar again, drawer mode back to 2 (HUD) — but the
drawer stays **closed**: the HUD's bottom bar does not come back until the
player opens it (the Lower Panel command). `wait buysell` (script wait 7)
ends when `+0x1b0` is 0 (`0x0049301d`).

## §2 — Assets

`buysell.dat` (`FUN_0047f670("buysell.dat", -1, 0)`, kept at `+0x190`).
Measured with `tools/ui/dump_dat.py`:

| Entry | Size | Registration | Role |
|---|---|---|---|
| `BuySellMain` | 452×178 | (0, 0) | the panel, opaque (flags 0x2) |
| `BuySellUpU` / `BuySellUpD` | 22×21 | (−425, −50) | scroll up, up/down faces |
| `BuySellDownU` / `BuySellDownD` | 22×46 | (−425, −125) | scroll down |
| `BuySellActivateU` / `…D` | 106×41 | (0, 0) | Buy / Sell |
| `BuySellExitU` / `…D` | 112×41 | (−340, 0) | Exit |

The buttons are TButtons built from the dat (`0x0042c400(dat, name, key,
callback, 0, 0, 0, 0x10, -1)`), placed at their registration points:
Activate (0, 0), Exit (340, 0), Up (425, 50), Down (425, 125), pane-local.
No hover face (`…S`) exists.

Fonts (FONT.DEF, indices from the static init `0x00485db1..`):
`DAT_00667540` = **"Large"** (Times New Roman 20 bold), `DAT_0065abc4` =
**"Small"** (Arial 12). Text tags (english.def): `BSGOLD` "Gold", `BSGP` "gp",
`BSBUY` "Buy", `BSSELL` "Sell", `BSEXIT` "Exit", `BSWEA1` "Damage " (sic,
trailing space), `BSWEA2` "Min Strength", `BSARM1..5` "Protection", "Rst
Poison", "Stlth", "Min Strn", "Min Cons", `BSFOOD1..4` "Cure Health", "Cure
Mana", "Cure Fatigue", "Cure Poison", `BSAMMO1` "Quiver of NUMBER NAME's".

## §3 — Frames

| Frame | Parent | Origin | Cite |
|---|---|---|---|
| screen | — | (0, 0), 640×480 Classic | — |
| shop pane | screen | **(0, screen_h − 178)**, 452×178, bottom-left | ctor `0x00488ef0` (0, 0x12e = 302); the drawer is `screen_h − 0xb2` (`0x0047b4d0`) |
| row *r* (0..2) | shop pane | (0, 42·(r+1)) | row draw |

The panel's right edge (452) meets the side panel (188 wide), which the shop
opens; the map view above it is 302 tall at 640×480.

## §4 — Layout (pane-local, literals from `0x0052f7d0` / `0x0052f040`)

| Element | Rect (x, y, w, h) | Source |
|---|---|---|
| chrome | (0, 0) 452×178 | `BuySellMain` stamped at (0,0) with the pane's drawmode |
| gold | text (125, 0, 98, 44) | paint `0x0052f90a..14` |
| Buy / Sell label | text (0, 0, 100, 44) | paint `0x0052f9ff..09` |
| Exit label | text (345, 0, 100, 44) | paint `0x0052fa39..46` |
| row *r* icon | stamp at (5, 44 + 43·r) | row `0x0052f2e8..fd` |
| row *r* name | text (60, 42·(r+1), 340, 20) | row `0x0052f0cb..0a` |
| row *r* price | text (356, 42·(r+1), 68, 20) | row `0x0052f15d..69` |
| row *r* stats | text (64, 62 + 42·r, 400, 12) | row `0x0052f1f7..37` |
| row *r* description | text (64, 74 + 42·r, 400, 12) | row `0x0052f284..91` |

*r* = item index mod 3 (`0x0052f0b4..bc`); the first shown item is `+0x180`
and the scroll steps by 3, so *r* = index − first.

## §5 — Draw order (`0x0052f7a0` → `0x0052f7d0`)

`0x0052f7a0`: while visible (`+0x50`), the paint (slot 21), then the button
pane's own draw (slot 11). The paint:

1. Mark the four buttons to be drawn with the pane (button flag `0x20`; Init
   cleared it).
2. Chrome `BuySellMain` at (0, 0).
3. Gold: `"%s %d"` of `GetLine("BSGOLD")` and the customer's
   `GetInventoryAmount("GOLD")` (vtable `0x84`) → "Gold 1234". With no
   customer the text is never formatted (the buffer is stale; the port draws
   nothing).
4. Rows `+0x180` … `min(+0x180 + 3, count) − 1`, each `DrawRow(index,
   state)`: state 1 if index = `+0x18c` (hover), else 2 if index = `+0x188`
   (selected), else 0.
5. The buttons (`0x00435de0`).
6. The Activate label: `BSBUY` if the shop type has bit 1, else `BSSELL` if
   bit 2, else none.
7. The Exit label `BSEXIT`.

`DrawRow` (`0x0052f040`): name, price, stats line, description, then the
icon.

## §6 — Algorithms

### 6.1 Shop type (`+0x17c`, `buysellshoptype`)

| Command | Value | Bits |
|---|---|---|
| `buy weapon` / `buy armor` / `buy misc` | 9 / 5 / 0x11 | 1 = Buy |
| `sell weapon` / `sell armor` / `sell misc` | 10 / 6 / 0x12 | 2 = Sell |
| | | 8 weapon, 4 armor, 0x10 misc |

### 6.2 BuildItem `0x0052da90(name, type, amount)` — one row

Returns false when no class has a type of that name (classes 0 up, first
match, exact name ignoring case). The record (retail 0x48 bytes):

| Offset | Field | Value |
|---|---|---|
| +0x00 | description | below |
| +0x04 | display name | `0x0046e7f0`: the name with everything but letters and digits removed, looked up as a dialog tag; a hit gives the tag's line, a miss (`"[…"`) the name itself |
| +0x08 | type name | the name |
| +0x0c..+0x30 | stats | up to five (label, value) pairs |
| +0x34 | stat count | |
| +0x38 | **price** | the type's class stat **`Value`** (0 when the class has none); in a Sell shop `__ftol(Value × 0.3f)` (`0x0052efe4`, float `0x005aedb0`) |
| +0x3c | amount | the caller's (Add: the command's number, default 1; criteria: 1; the customer's items: the item's `Amount`) |
| +0x40 | icon source | the type's imagery (`0x00446b10(imageryid, 1)`) |

**Weapons and armor with a WEAPON.DEF / ARMOR.DEF entry** (`0x0048cb50`):

- description: the entry's DESCRIPTION tag's line if the tag exists, else the
  entry's STATLINE expanded (`0x0048cce0`, below), else none;
- weapon stats (2): `BSWEA1` damage = BASICMODS[2], `BSWEA2` min strength =
  BASICMODS[7];
- armor stats (5): `BSARM1` protection [1], `BSARM2` resist poison [3],
  `BSARM3` stealth [4], `BSARM4` min strength [6], `BSARM5` min constitution
  [7] (BASICMODS in file order; the armor loader stores [4] and [5] swapped,
  which the record offsets `+0xb4`/`+0xb0` undo).

**Otherwise, by class** (stats from the type's class stats; no
description):

| Class | Stats |
|---|---|
| 1 weapon | `BSWEA1` Damage, `BSWEA2` MinStrength |
| 2 armor | `BSARM1` Protection, `BSARM2` ResistPoison, `BSARM3` Stealth, `BSARM4` MinStrength, `BSARM4` MinConstitution (retail uses tag BSARM4 twice; its fallback text for the fifth is "Min Cons") |
| 4 food, 0x12 potion | `BSFOOD1` Health, `BSFOOD2` Mana, `BSFOOD3` Fatigue, `BSFOOD4` Poison |
| 0x15 ammo | none; the display name becomes `BSAMMO1` with NUMBER → the amount and NAME → the name ("Quiver of 20 Arrow's") |
| others | none |

Each label is the tag's line when the tag exists (`FindLine`), else the
literal in parentheses above ("Protection", "Rst Poison", "Stlth", "Min
Strn", "Min Cons", "Damage", "Min Strength", "Cure Health", …).

**STATLINE expansion `0x0048cce0`.** Words separated by spaces; for each
word: the line of tag `STATCFG<word>` (a miss gives `"[STATCFG…]"`), then a
space, then the run of digits / `-` / `+` / `%` that follows (spaces inside
it skipped), then a space. `DmgResPoison 6` → "Resist Poison Attack 6 ". A
line is cut short when the input is used up while copying it.

### 6.3 Filling the shop

**Stock** (Buy shops):

- `Add(name, amount)` `0x00530670`: the type in the shop's class — weapon
  shops class 1, armor shops class 2, misc shops the first of classes 2, 1, 4,
  0x12, 5, 0x11, 0x15 that has it — then the sale rule, then BuildItem.
- `AddCriteria(stat, min, max)` `0x00530af0`: every type of the shop's class
  (misc: classes 1, 2, **2**, 4, 0x12, 0x15, 5, 0x11 — armor twice) whose
  class stat `stat` is in [min, max] (absent = 0), sale rule, BuildItem with
  amount 1.
- **Sale rule** (class stat `SaleType`): 0 or absent → stocked; 1 → stocked
  only if the save's merchant table has (class, type) (`0x0048e630`); 2 or
  more → never.

**The customer's items** (Sell shops), walking the customer's inventory with
nested inventories (bags, pouches) included:

- `AddBuyItems()` `0x00531b70`: every item with inventory slot < 0x100 (not
  equipped, not on the belt itself), `SaleType` ≠ 2, `Value` ≠ −1 (vtable
  `0x190`), whose class matches: class 1 needs a weapon shop, class 2 an
  armor **or misc** shop, classes 4, 5, 0x11, 0x12, 0x15 a misc shop. Each
  item is a row: BuildItem(its name, type, its `Amount`).
- `AddBuyItem(name)` `0x00531d70`: the same, only items of that name.
- `AddBuyCriteria(stat, min, max)` `0x00531fc0`: the same, only items whose
  stat (object, else class) is in [min, max].

**Removing:** `Remove(name)` `0x00532210` drops the rows whose display name
equals `name` (case-sensitive). `RemoveCriteria(stat, min, max)` `0x00532340`
(also `buysellremovebuycriteria`): for each class of the shop (misc: 1, 2, 4,
0x12, 0x15, 5, 0x11) that has the stat, drop the rows whose **display name**
names a type of that class with the stat in [min, max].

### 6.4 Buying and selling (Activate `0x0052ff40`)

Nothing happens without a selected row (`+0x188` ≠ −1, ≤ count) and a
customer.

**Buy** (bit 1):

1. The customer's gold (`GetInventoryAmount("GOLD")`) < price: the
   salesperson says the no-gold tag (`SayTag(tag, -1, 0)` `0x004d0a20`) —
   only with both set, and not while it is already saying something (its
   doing action is 0x10). The selection stays.
2. Else, if the customer has an empty slot (vtable `0x88`): `AddToInventory(
   type, n, -1)` with n = the row's amount in a misc shop, else 1 (a misc
   row without a type name uses the display name), then `DeleteFromInventory(
   "GOLD", price)`; the salesperson says the purchase tag (same conditions).
   The row stays (stock is endless); the selection stays.
3. No empty slot: nothing.

**Sell** (bit 2):

1. If the type's `SaleType` is 1 and the merchant table hasn't the (class,
   type) yet, add it (`0x0048e670`): the item is now stocked by every shop
   that lists it.
2. Delete one item of that name from the customer (`0x005330a0`: the first,
   preferring one that isn't equipped, looking inside containers of class 5
   and 0x11; the whole item, whatever its amount). If one was deleted:
   `AddToInventory("GOLD", price, -1)` and `TPlayer::RefreshEquip`
   (`0x00519230`).
3. The row is removed either way; the selection is cleared.

A no-gold or purchase line is said only when the line is set
(`buysellnogolddialog` / `buysellpurchasedialog`) and a salesperson is set.

### 6.5 Scrolling and selection

- Up (`0x0052fe90`): first −= 3, not below 0. Down (`0x0052fee0`): first += 3,
  undone if it passes the last item. Both clear the selection and the hover.
- Keys `1` `2` `3`: select first + 0 / 1 / 2, clamped to count − 1.
- Mouse (button up, left or right, x < 200): y in [43, 85], [87, 129], [131,
  173] selects first + 0 / 1 / 2, plays `click1` (`0x0049b990(id, 0x7f, 1,
  0, 0x50, 700)`), clamps.
- Mouse move: the hover row by the same bands (x < 200), clamped; −1
  elsewhere.
- Joystick: up / down move the selection one row, paging the view; button 1
  is Activate, button 2 Exit.
- Keys `B` `V` `b` `v` are swallowed (not offered to the buttons); every
  other key goes to the button pane: ↑ Up, ↓ Down, `A` Activate, `E` Exit.

## §7 — Effects & shadows

All text is drawn with the 3-pass black shadow (flag `0x400`). Buttons and
icons are plain stamps (icons: drawmode `0x110`, transparent). No alpha, no
glow, no animation beyond the hover/selection colours.

## §8 — Text

Colours are stored B, G, R (TTextBar convention); shown here as R, G, B.

| Text | Rect | Font | Flags | Colour | Format |
|---|---|---|---|---|---|
| gold | (125, 0, 98, 44) | Large | `0x441`: left, single line v-centred, shadow | (255, 186, 0) | `"%s %d"` BSGOLD, gold |
| Buy / Sell | (0, 0, 100, 44) | Large | `0x442`: centred, v-centred, shadow | (255, 186, 0) | the tag |
| Exit | (345, 0, 100, 44) | Large | `0x442` | (255, 186, 0) | BSEXIT |
| name | (60, y, 340, 20) | Large | `0x401`: left, top, word-wrapped | state 0 (130, 13, 197), 1 hover (230, 150, 255), 2 selected (200, 83, 255) | `"%s"` display name |
| price | (356, y, 68, 20) | Large | `0x404`: right, top | as the name | `"%d%s"` price, BSGP → "25gp" |
| stats | (64, y+20, 400, 12) | Small | `0x401` | (162, 162, 162) | `"%s%s: %+d  "` per non-zero stat → "Damage : +13  Min Strength: +16  " |
| description | (64, y+32, 400, 12) | Small | `0x401` | (162, 162, 162) | `"%s"` |

GDI clipped each text to its rect (no `DT_NOCLIP`), so the 12-px stats and
description rects show one line.

## §9 — Dynamic state

| Field | Meaning | Written by |
|---|---|---|
| `+0x17c` | shop type | `buysellshoptype` |
| `+0x180` | first row shown | Init 0, Up/Down, joystick |
| `+0x184` | rows shown | Init 3 |
| `+0x188` | selected row, −1 none | Init −1, keys, mouse, joystick, Up/Down −1, a sale −1 |
| `+0x18c` | hover row, −1 none | Init −1, mouse move, Up/Down −1 |
| `+0x190` | `buysell.dat` | Init (first time) |
| `+0x194/196/198` | rows: count / capacity / array | the fill commands, Exit, Reset, Clear |
| `+0x19c` | a 400×24 surface | Init; never drawn into |
| `+0x1a0` | salesperson | `buysellsalesperson` (`FindClosestObject(name, caller)`) |
| `+0x1a4` / `+0x1a8` | purchase / no-gold tags | the dialog commands |
| `+0x1ac` | assets built | Init, Close |
| `+0x1b0` | in use (script wait 7) | Init 1; Exit, Reset 0 |
| `+0x1b4` | customer | `buysellscreen` |

Init (`0x0052f390`) sets `+0x1b0` = 1, first 0, selection and hover −1; the
first time it also builds the pane, the buttons and the 400×24 surface and
clears salesperson, tags and customer. Re-initializing keeps the rows, the
salesperson, the tags and the customer. Rows go only through Exit, Reset (the
drawer closing), Clear (`LoadGame`) and the remove commands.

## §10 — Input summary

| Input | Effect |
|---|---|
| Activate (click, `A`, joystick button 1) | buy / sell the selected row (§6.4) |
| Exit (click, `E`, joystick button 2) | `+0x1b0` = 0, rows freed, PlayScreen `+0x6b8` = 0 → the drawer closes on the next pulse |
| Up / Down (click, ↑ / ↓) | page the rows (§6.5) |
| `1` `2` `3`, row click | select |
| Lower Panel (B, PlayScreen command 5, control on) | closes the drawer, so the shop (PlayScreen `0x0047cf40` case 5) |

## §11 — Retail behaviour kept, flagged

1. The misc criteria walk visits armor (class 2) twice, so a misc shop filled
   by `buyselladdcriteria` lists matching armor twice. No shipped script
   does that (criteria are used only by the weapon and armor shops).
2. Selling a stacked item (Amount > 1) deletes the whole item for one unit's
   price.
3. `RemoveCriteria` finds types by the row's display name: a row whose
   display name differs from its type name (a localized name) is never
   removed.
4. Armor's fifth stat label uses tag `BSARM4` again (class-stat path), so it
   reads "Min Strn" twice in English unless BSARM4 is missing.
5. With no customer the gold text is a stale buffer (the port draws nothing).
6. Buying with a full inventory does nothing and says nothing.

## §12 — Retail bugs not reproduced

- The pink halo (chroma-keyed text, UI_METHOD_MAP §16).

## §13 — Port mapping

| Retail | Port |
|---|---|
| `TBuySellScreen` `0x0065a3b8` | `TBuySellPane BuySellPane` (`src/buysell.{h,cpp}`), a `TDefPane`: `OpenChrome("buysell.dat", "BuySellMain")` + four `AddSpriteButton`s with their keys |
| the 0x48-byte record | `SBuySellItem` (strings owned, the icon's imagery held by a counted reference) |
| Paint `0x0052f7d0` | `TBuySellPane::Paint` (the `TDefPane::Paint` hook: chrome, gold, rows, `PaintWidgets()`, labels) |
| `0x004be2b0` text | `DrawTextShadowedToTarget` with `FontTable->Atlas("Large"/"Small")` |
| drawer `+0x6a0..+0x6c4` | `TPlayScreen::EDrawer`, `RequestBuySell`, `CloseDrawer`, `UpdateDrawer` |
| `wait buysell` | `EScriptWait::BuySell` → `!BuySellPane.IsActive()` |
| merchant table | `TSaveGame::HasSoldUnique` / `AddSoldUnique` |

### Port deviations (also in the code's comments)

1. Text goes through the metric-compatible TrueType faces of FONT.DEF
   "Large" / "Small", not GDI. GDI clipped each text to its rect; the
   port draws the wrapped lines that start inside the rect, whole (the
   12-px stats and description rects can show a descender GDI cut).
2. With no customer the gold text is not drawn (retail drew a stale
   buffer). The panel recomposes when the customer's gold changes; retail
   repainted only on its own input.
3. If the shop can't initialize (no `buysell.dat`), the port drops the
   request and lets `wait buysell` end; retail retried every pulse and the
   script waited forever.
4. A sold item is deleted at once (as `DeleteFromInventory` does); retail
   flagged it `OF_KILL` and took it out of the inventory. The inventory
   pane's refresh for a sold container (`0x005391a0`) isn't ported.
5. Only a character salesperson speaks (retail read `+0xd8` of whatever
   object `buysellsalesperson` found).
6. Drawer closes reach the shop only; retail's `LoadGame` and
   `hideresponse` would also close the HUD's bottom bar (AUTHOR_QUESTIONS 81).
7. The port's buttons raise only clicks; retail's Activate callback also
   cleared the selection on its other button events.
8. The sell price is computed in double from the float 0.3f (retail: x87);
   identical for any Value below 2^29.
9. A misc row's `Value` for the sell filter is read as the type's class
   stat for every class (retail: vtable `0x190`, which only the item
   classes implement).
10. The 400×24 surface (`+0x19c`) isn't created: nothing draws into it.
11. Criteria stat names are cut at 31 characters (retail overran its
    32-byte buffer).
12. Each row holds a counted reference to its icon's imagery and releases
    it with the row (retail took one per row and never released it).
13. Multiplayer (`0x00586a60`, `0x005869a0`, `0x00586a10`) and
    `getitemname` / `getitemvalue` are not ported.

## §14 — Open items

None block the port. Unverified against a running retail game: the exact
glyph placement of GDI text (the port draws with the metric-compatible TTF
faces), which needs retail screenshots ([AUTHOR_QUESTIONS.md](../../gameflow/AUTHOR_QUESTIONS.md) S14, S15).
