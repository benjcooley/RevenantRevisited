# Inventory actions — forensics

What the shipped game does when the player clicks, right-clicks, drags
and hovers items in the side panel's inventory (`TInventory`), the belt
on the bottom bar (`TBarInvPane`) and the paper doll (`TEquipPane`), and
what the items' `Use` and `CursorType` do. Written for four port bugs:
items can't be dragged into bags, the cursor doesn't change over an item,
clicking food or a potion does nothing, and equipment can't be readied.

Builds on [INVENTORY.md](INVENTORY.md) (AddToInventory, merging, the
Pouch rule), [../../ui/forensics/InventoryPane_SPEC.md](../../ui/forensics/InventoryPane_SPEC.md)
§9-§10, [../../ui/forensics/BarInvPane_SPEC.md](../../ui/forensics/BarInvPane_SPEC.md),
[../../ui/forensics/EquipPane_SPEC.md](../../ui/forensics/EquipPane_SPEC.md),
[../../ui/forensics/MouseCursor_SPEC.md](../../ui/forensics/MouseCursor_SPEC.md)
and [../../gameplay/forensics/PLAYER_INPUT.md](../../gameplay/forensics/PLAYER_INPUT.md) §6.
It replaces their guesses where they differ (the "use-vs-swap gate", the
belt's click body, the equip pane's left-down).

Sources: the retail asm listing
(`recon/retail_asm/baseline/sections/00001000_section_0.asm`, byte-identical
to the shipped exe), read function by function; vtables, strings and stat
tables read from `Revenant.exe`; the retail `class.def` (in `imagery.rvi`)
and `english.def` (in `resources.rvr`). The 1998 source
(`/Users/benjamincooley/projects/Revenant/`, cited as `1998 file:line`)
for names; where it differs from the asm, the asm wins.

Confidence: **[C]** read in the asm; **[I]** inferred; anything not
settled is marked UNCONFIRMED.

## 0. Summary

1. **Bags.** A drag that ends on another item's cell calls that item's
   `Use(player, dragged item's id)` (vtable `0xbc`). A bag's `Use`
   (`TContainer::Use` `0x004dc180`, INVCONTAINER shares it) puts the
   dragged item in the bag (`AddToInventory(item, -1)`, "pouch" sound)
   when the bag is in the player's own inventory. A plain click on a bag
   (no drag) calls `Use(player, -1)`, which shows the bag's contents in the
   inventory pane (`SetContainer` `0x005391a0`); a click on the pane's
   container icon (top left) goes back up to the owner. [C]
2. **Hover cursor.** Each item pane's `Animate` calls
   `CursorOverObject(item, 0)` (`0x0043a820`) for the item under the
   pointer while no button is held. That asks the item's
   `CursorType(DragObj)` (vtable `0xc0`) and shows the named corner
   picture in place of the arrow: food and potions "mouth", scrolls
   "eye", a bag that is openable or holds something "hand"; weapons,
   armour, keys, talismans show nothing. [C]
3. **Eating, drinking.** A plain click on an item calls its
   `Use(player, -1)`. Food and potions share `0x0050e780`: health and
   mana go up by the item's stats (clamped), a "Poison" stat cures
   poison, the "food" or "potion" sound plays, the pile loses one (or the
   item is deleted), and the text bar says FOODGOOD1 ("The item was
   consumed. You feel rejuvenated."). [C]
4. **Readying.** Right button up on a pack or belt item, or a drop of a
   pack or belt item anywhere on the paper doll, equips it in its own
   `eqslot` stat's slot (`CanEquip` `0x00519300`, `Equip` `0x005199b0`)
   and plays the item's class sound. No `eqslot` prints EQUIPUNABLE.
   Retail `CanEquip` also refuses a weapon type the character can't use,
   too little Strength (weapons, armour) and too little Constitution
   (armour), with the requirement on the text bar. [C]

Before 2026-10-13 the port had none of 1-3 and only part of 4; §9 has
what it does now.

## 1. Conventions

**Events** are the `MB_*` codes (`1998 RevDefs.h:269-278`): 1 left down,
2 right down, 4 left up, 5 right up. A button release goes to every pane
wherever it lands, not just the pane under the pointer (retail
`TScreen::MouseClick` `0x004904ef..0x004904fc`, as noted in
`src/playscreen.cpp`; `1998 screen.cpp:316-327`). So each pane's left-up
handler runs, and a pane learns about a drag that started in another pane
through that pane's **held slot**, which outlives the grab. [C]

**Pane objects and fields** (retail globals; `+0x50` is each pane's dirty
flag):

| Pane | Object | Grab | Held | Dragging | Start x / y | Other |
|---|---|---|---|---|---|---|
| Inventory | `0x0065d4f8` (vtable `0x005a58c0`) | `+0x180` | `+0x184` = `0x0065d67c` | `+0x18c` | `+0x190` / `+0x194` | `+0x17c` = `0x0065d674` container shown; `+0x188` first slot (page) |
| Belt | `0x0065b028` (vtable `0x005a56d4`) | `+0x64` (box index) | `+0x68` = `0x0065b090` | `+0x6c` | `+0x70` / `+0x74` | `+0x60` = `0x0065b088` character; slot = `0x10b` + index |
| Paper doll | `0x0065b7e0` [I] (vtable `0x005a55dc`) | `+0x94` | `+0x98` = `0x0065b878` | none | none | EQ index 0..10; the player's equipment array is `player+0x2a0` |

Each pane's MouseClick is vtable `+0x64`, MouseMove `+0x68`, Animate
`+0x58`: Inventory `0x00538210` / `0x00538c10` / `0x00537f90`; belt
`0x0052cfb0` / `0x0052d6e0` / `0x0052cd80`; paper doll `0x005363e0` /
(base) / `0x00536970`. [C]

**Item cells.** Inventory cell (column c 0..3, row r 0..2) at pane
`(8 + 45c, 42 + 44r)`, 40×40; slot = `3c + r + page`
(`0x0053835e..0x005383c7`). Belt box n at bar `(220 + 45n, 10)`, 45×40
hot. Paper doll: the 11 `{x, y}` pairs at `0x005e3f60`, 40×40. [C]

**The player** is `0x00667fcc`. Every item action passes him as the user,
not the pane's container. [C]

**Multiplayer flag `0x0066829c`.** `TPlayScreen::Initialize` (`0x0047a660`)
sets it at `0x0047a687` to "entry mode (`+0x6d8`) is 3", the multiplayer
mode. It is the gate InventoryPane_SPEC called the "use-vs-swap gate"
(§3.2). It is 0 in single player. [C]

## 2. The item virtuals

All 122 object vtables (those with `TObjectInstance::Distance`
`0x0046ea20` at `+4`), plus TCharacter `0x005a7848` and TPlayer
`0x005b4f30`, were read at these slots. Class names come from the builder
registrations (`push "<NAME>"; mov ecx, builder; call; mov [builder],
builder vtable`), whose build function writes the object vtable. [C]

| Slot | Base | Role |
|---|---|---|
| `0xb8` | `0x004704d0` | **CanCombine(user, with)**: no class overrides it. Returns 1 when `with` is an item (OF_INVENTORY `0x40000000`) with the same owner and class, both have the "Combining" stat and their types differ by 1; or both have the same nonzero "CombineID" and their "TargetID"s differ (and aren't both set). Else 0. |
| `0xbc` | `0x004705f0` | **Use(user, with)**: `with` is an object id (mapindex) or -1. |
| `0xc0` | `0x00477a20` (returns -1) | **CursorType(with)**: a `CURSOR_*` index or -1. |
| `0xd4` | `0x00473600` | GetStat(name), case-insensitive (stricmp `0x0059a530`); objstats first, then class stats. |
| `0x130` | `0x0046f190` | InventoryImage: the imagery's GetInvImage(state) (imagery `+0xd4`), else frame 0 of GetInvAnimation (`+0xd8`). |

Overrides of `Use` / `CursorType` (vtable `+0xbc` / `+0xc0`):

| Class (builder name) | Vtable | Use | CursorType |
|---|---|---|---|
| ITEM-like base: ARMOR `0x005a761c`, KEY `0x005b3800`, LIGHTSOURCE `0x005b39fc`, TALISMAN `0x005b619c`, RANGEDWEAPON `0x005b5364`, AMMO family `0x005a6944..0x005a73f8`, TILE | | base `0x004705f0` | base: -1 |
| FOOD | `0x005b2cd4` | `0x0050e780` | `0x0050ebe0` |
| POTION | `0x005b2ee4` | `0x0050e780` (the same) | `0x0050ebe0` (the same) |
| CONTAINER, INVCONTAINER | `0x005a8118`, `0x005a8360` | `0x004dc180` | `0x004dd370` |
| VIAL RACK | `0x005a7ed0` | `0x004dd970` | `0x004ddb90` |
| MONEY | `0x005b4a7c` | `0x00515e70` | `0x00515f10` |
| SCROLL | `0x005b5560` | `0x00520eb0` | `0x00521c90` |
| WEAPON | `0x005b9088` | `0x00528d80` | `0x00528e20` |
| TOOL | `0x005b65cc` | `0x005240c0` (base, result as 0/1) | `0x005240e0` (-1) |
| TCharacter / TPlayer | `0x005a7848` / `0x005b4f30` | `0x004d4a60` / `0x005201b0` (returns 0) | `0x004d4a00` / `0x005201c0` (-1) |
| EXIT family, traps | `0x005b2258` etc. | `0x0050df30`, `0x0050dae0`, `0x0050d1a0`, `0x0050dc20`; traps `0x00524450`.. | `0x0050df40`, `0x0050e020`, `0x0050d370`; traps `0x00527540`.. |

In the 1998 source `Use` and `CursorType` follow `IsInInventory`
(`1998 object.h:858-863`); retail inserted CanCombine before `Use`. [C]

Stat getters used below (class-specific slots start at `0x1f0`; each
getter reads a stat index registered at startup, so the names are
certain): characters `0x1b0`/`0x1b4` Poisoned/SetPoisoned, `0x1c0`/`0x1c4`
Health/SetHealth, `0x1d0`/`0x1d4` Mana/SetMana, `0x1d8` MaxHealth, `0x1e8`
MaxMana; food `0x1c0` Health, `0x1d0` Mana, `0x1f0` Poison; containers
`0x1f0` Openable, `0x208` Locked, `0x220` Explosive, `0x238` TargetID;
weapons `0x1f0` Type, `0x218` MinStrength; armour `0x210` MinStrength,
`0x218` MinConstitution. [C]

## 3. The inventory pane — MouseClick `0x00538210`

First the button-pane base (`0x00436530`) gives the page arrows their
click. Nothing happens without a container (`+0x17c`). [C]

### 3.1 Left down (1)

Inside the pane: grab = held = the slot under the pointer (-1 off the
cells), start = (x, y), dragging = 0 (`0x0053851a..0x005385e6`). Nothing
else; the drag starts in MouseMove. [C] (`1998 inventory.cpp:98-104`.)

**MouseMove** `0x00538c10`, with button state 1 or 2: over the container
icon (`10 <= x <= 30`, `10 <= y < 30`) it lights the icon (`+0x1a4`).
When not dragging and the pointer is 2 px or more from the start in x or
y: the grabbed item's InventoryImage becomes the drag bitmap
(`SetDragBitmap` `0x0043a100`) at the grab offset
`((start - (8, 42)) mod (45, 44))`, or (20, 20) when that point of the
image is clear (`0x004a3060`); `SetDragObj(item)` (`0x0043a140`), which
also plays the item's class sound (§7); dragging = 1. [C]
(`1998 inventory.cpp:179-208`, same.)

### 3.2 Left up (4) on a cell, a drag or click from this pane

`0x0053878e..0x005388f2`. src = container's item at grab, dst = the item
at the cell under the pointer:

```
if (!src) end
if (!dst)                          -> move
if (dragging && dst != src)        -> with = src->id (+0x40); try use
if (dragging && dst == src)        -> move     (no-op: same slot)
if (!dragging && dst != src)       -> move
if (!dragging && dst == src)       -> with = -1; try use        // a click
try use:
    if (0x0066829c && cell != grab && dst->objclass != INVCONTAINER(17))
        if (!dst->CanCombine(player, with)) -> move              // multiplayer only
    if (dst->Use(player, with)) { repaint; end }
    -> move
move:
    container->AddToInventory(src, cell); play "pop"
```

So a click without a drag uses the item, and a drag onto another item
uses the target with the dragged item, falling back to a swap when `Use`
returns 0 (AddToInventory puts the displaced item where `src` was,
INVENTORY.md §2 step 10; a food or potion of the same name merges, §3
there). [C]

1998 (`inventory.cpp:125-149`) called `oi->Use(GetTopContainer(), ...)`
and swapped slot numbers by hand (`SwapSlots`); retail passes the player
and moves through AddToInventory, and adds the multiplayer CanCombine
gate and the "pop" sound. [C]

### 3.3 Left up on the container icon

`0x005385e9..0x00538725`: `10 <= x <= 30`, `10 <= y < 30`, and the shown
container has an owner (a bag):

- nothing grabbed here: show the owner (inline SetContainer: `+0x17c` =
  owner, rebuild the icon `0x00538e40`, page 0, arrows reset; multiplayer
  notifies the server `0x00584680`);
- an item grabbed here: if the owner has a free slot
  (FindFreeInventorySlot, vtable `0x94`, `< 0xff`),
  `owner->AddToInventory(item, that slot)` and "pouch". [C]

(`1998 inventory.cpp:107-124`, same in intent; 1998's box was the whole
left margin.)

### 3.4 Left up on a cell, a drag from another pane

`0x00538900..0x00538c06`, when nothing was grabbed here and the release
is on a cell:

**From the paper doll** (`0x0065b878 >= 0`): eq = the player's equipment
in that slot, dst = the cell's item (`0x00538992..0x00538ae5`).

- dst empty: `Equip(NULL, held slot)`, `container->AddToInventory(eq,
  cell)`, "pop".
- dst there and `CanEquip(dst, held slot)`: `Equip(dst, held slot)`, a
  swap (Equip puts `eq` where `dst` was, §5.2).
- otherwise the code falls into the use logic with this pane's dragging
  flag, which is always 0 here, so nothing happens. [C]
- Then a light source re-sets the walk root (`0x004cf000`) either way.

**From the belt** (`0x0065b090 >= 0`; `0x00538af3..0x00538c06`): the belt
is flagged for repaint; b = the belt item, dst = the cell's item. If b is
the shown container or one of its owners, nothing happens (a pouch can't
go into itself). dst there: `dst->Use(player, b == dst ? -1 : b->id)`
(multiplayer CanCombine first, as §3.2); if that returns 0, or dst is
empty: `container->AddToInventory(b, cell)`, "pop". [C]

1998 (`inventory.cpp:150-167`) took only equipment, and only onto an
empty cell. [C]

### 3.5 End of every left up

`0x00538728..0x00538784`: grab = -1, dragging = 0 (the held slot stays
for the other panes); if the release is inside the pane,
`SetDragBitmap(0,0,0)`, `SetDragObj(0)`, the spell-drag name cleared
(`0x0043a170`) and a repaint. [C]

### 3.6 Right up (5)

Inside the pane only (`0x0053823c..0x00538517`):

- **Stats mode** (`0x0065c9e0`): the item under the pointer shows its
  info (`0x0047eff0`, then `0x005496a0` on `0x0065b140`); on no item and
  `0x0065d1b8 == 1`, the player's. [C]
- Otherwise only with nothing held anywhere (this pane's grab, the doll's
  `0x0065b878`, the belt's `0x0065b090` all < 0). The item under the
  pointer:
  - a TALISMAN (class 3) the player owns directly: into his "Spell Pouch"
    (`FindObjInventory` by name, `AddToInventory(item, -1)`, "pouch"), or
    EQUIPUNABLE when he has none;
  - `slot = item->GetStat("eqslot")`; -1: EQUIPUNABLE on the text bar
    (`TDialogList::GetLine` `0x0049d800`, `TTextBar::Print` `0x0054d170`);
    11 or more: nothing; `CanEquip(item, slot)` then `Equip(item,
    slot)`, and when Equip succeeds the item's class sound (`0x00473a10`);
    repaint; a light source (class 6) re-sets the walk root (`0x004cf000`).
  - **no item under the pointer:** the container's carried items are
    renumbered 0, 1, 2, ... in array order, closing the gaps
    (`0x0046ff80`; network message; inventory repaint). [C]

PLAYER_INPUT.md §6 has the equip half; the compaction is new here.

## 4. The belt — MouseClick `0x0052cfb0`

Nothing without a character (`+0x60`). The box under the pointer is
`(x - 220) / 45` within the visible boxes and `y - 10 < 40`. [C]

- **Right up (5)** (`0x0052cfc2..0x0052d26d`): the same as the inventory
  (§3.6): stats-mode info; with nothing held anywhere (belt grab,
  `0x0065b878`, inventory held `0x0065d67c`) the talisman → Spell Pouch
  rule, EQUIPUNABLE, CanEquip/Equip/class sound/walk root; an empty box
  compacts the character's carried items (`0x0046ff80`). [C]
- **Left down (1)** (`0x0052d270..0x0052d301`): grab = held = box index,
  start = (x, y), dragging = 0. [C]
- **MouseMove** `0x0052d6e0`: buttons 1 or 2, the same 2 px promotion,
  drag bitmap at `((start - (220, 10)) mod (45, 40))` or (20, 20),
  `SetDragObj` (class sound), dragging = 1 (BarInvPane_SPEC §10). [C]
- **Left up (4) on a box** (`0x0052d304..0x0052d6d1`):
  - own grab: the §3.2 logic with belt slots (`0x10b` + index):
    click on the same item → `Use(player, -1)`; drag onto another item →
    `dst->Use(player, src->id)`, else `character->AddToInventory(src,
    box slot)` and "pop". [C]
  - a doll item held (`0x0065b878`): box empty → `Equip(NULL, held)`,
    `AddToInventory(eq, box slot)`, "pop"; box full and `CanEquip(box
    item, held)` → `Equip(box item, held)`; then the light-source walk
    root. [C]
  - an inventory item held (`0x0065d67c`): the inventory is flagged and
    told (`+0x90`); src = the shown container's held item. A box item
    that is src's owner refuses it (it is already in that bag). Box item
    there → `Use(player, src == it ? -1 : src->id)` (multiplayer
    CanCombine first); if 0, or the box is empty,
    `AddToInventory(src, box slot)` and "pop". [C]
- **End of every left up** (`0x0052d476..0x0052d4b7`): grab = -1,
  dragging = 0; inside the bar, the drag bitmap and drag object are
  cleared and the bar repaints. [C]

BarInvPane_SPEC §10 had this handler as "msg == 5 only"; the body above
supersedes it.

## 5. The paper doll, CanEquip and Equip

### 5.1 TEquipPane::MouseClick `0x005363e0`

Nothing without a player. The slot under the pointer is the first of the
11 rects at `0x005e3f60` that holds it. [C]

- **Right down (2)** (`0x005363f7..0x00536475`): the equipped item's
  info (`0x0047eff0`, `0x005496a0`). [C]
- **Left down (1)** (`0x00536478..0x00536557`): if the slot holds an item
  with an InventoryImage, grab = held = slot, and the drag starts at once
  (no threshold): `SetDragBitmap(image, x - slot x, y - slot y)`,
  `SetDragObj(item)` (class sound), repaint. [C]
- **Left up (4)** (`0x0053655a..0x0053668e`):
  - own grab: just let go (grab = -1, repaint). The inventory or belt,
    which see the same release, move it if it landed on them (§3.4, §4).
    A doll item dropped on another doll slot does nothing. [C]
  - otherwise, released anywhere inside the pane: the held item is the
    inventory's (`0x0065d674`'s item at `0x0065d67c`), else the belt's
    (`0x0065b088`'s item at `0x10b + 0x0065b090`). `slot =
    GetStat("eqslot")`; -1 → EQUIPUNABLE; 11 or more → nothing;
    `CanEquip` then `Equip`, the class sound when Equip succeeded, the
    light-source walk root. [C]
  - always: drag bitmap and object cleared, spell-drag name cleared,
    inventory flagged (`0x0065d548`), repaint. [C]

1998 (`equip.cpp:53-132`) took only inventory items and let an accessory
fall into the other accessory slot when its own was full; retail goes to
the item's slot only, takes belt items, and adds EQUIPUNABLE, the sound
and the walk root. [C]

### 5.2 CanEquip `0x00519300` (TPlayer)

Refuses (returns 0) when, in order:

1. there is no item;
2. the item's class has no "EqSlot" stat (`0x00474210` on the class's
   stat list) — food, potions, scrolls, bags, money;
3. its EqSlot isn't the requested slot;
4. a WEAPON (class 1):
   - the player has no action animation for its root: Type 6 or 7 →
     `HasActionAni(GetBowRoot(item))` (vtable `0x308` `0x0051b3b0`,
     `0x1f0` `0x004d6c20`), else `HasActionAni(GetCombatRoot(item))`
     (`0x304` `0x0051b340`: "cstaff" for Type 5, "cknife" for Type 1,
     else "combat");
   - the weapon-type bit `1 << Type` is clear in the mask the
     character's data points to (`[player+0xfc]` → `+0x190` → `+0x70`;
     by its shape the char.def weapon mask, UNCONFIRMED name);
   - its MinStrength exceeds the player's objstat 34 (Strn in class.def's
     PLAYER list; `[player+0x350]+0x114` when `[player+0x34c] > 34`):
     the player gets "This item is not equipable. ( Min Strn: N )"
     (`"%s. ( %s: %d )"`, EQUIPUNABLE, BSARM4); [C]
5. an ARMOR (class 2) whose MinStrength exceeds Strn or whose
   MinConstitution exceeds Cons (objstat 35): "This item is not
   equipable. ( Min Strn: N )  ( Min Cons: M )" (EQUIPUNABLE, BSARM4,
   BSARM5). [C]

Otherwise 1. The messages print only for the main player. 1998 CanEquip
(`player.cpp:197-225`) had checks 1-4a only. [C]

Data: class.def gives the Short Sword and the Training Sword MinStrength
16, chainmail 20 / 18, cloth 16 / 12, and the PLAYER class's Strn and
Cons default to 14. A new game's Locke (the save after the opening) wears
his starting boots, shirt and Short Sword under the retail checks in the
port, so his Strn and Cons meet them. [C]

### 5.3 Equip `0x005199b0` (TPlayer)

`Equip(item, slot)`: slot 11 or more fails; slot < 0 takes the item's
"eqslot". Already there → 1. `CanEquip` must pass. The displaced item
goes where the new one was: in the same inventory the two swap slot
numbers (or the new item is added at `0x100 + slot`); from elsewhere (a
bag) the new item is removed, the displaced one added to that container
at the freed slot (or a free one), and the new one added to the player
at `0x100 + slot`. Repaint flags, network message `0x00584e00`, the
equipment array entry, the light-source walk root, the combat / bow root
refresh for slots 4 and 7, then `0x0051c660`. Returns 1. [C]

The port's `TPlayer::Equip` (`src/player.cpp:964`) follows this layout.

## 6. The hover cursor

### 6.1 Who calls it

- Inventory Animate `0x00537f90`: with a container and no button held
  (`0x00668518 == 0`), the slot under the pointer (cursor `0x00668510`,
  `0x00668514` minus the pane position, the cell math of §1), the item
  there (`0x004701f0`), then `CursorOverObject(item, 0)`. The rest of
  Animate draws the animated icons with the game frame (`0x0047e920`). [C]
- Belt Animate `0x0052cd80`: the same for the box under the pointer
  (`0x0052ce0f`). [C]
- Paper doll Animate `0x00536970`: the same for the doll slot under the
  pointer (`0x005369fd`). With no slot it passes the word before the
  equipment array (`player+0x29c`) instead of NULL (a harmless retail slip
  as long as that word is 0; UNCONFIRMED what it holds). [C]
- (The map pane picks its own hover object; out of scope here.)

### 6.2 CursorOverObject `0x0043a820`

If `toppriority` is 0 and a top-priority corner is already set
(`0x006563b0`), nothing. No item, nothing. `type =
item->CursorType(DragObj)` (`0x006563cc`); -1, nothing; else the bitmap
named `CursorTypes[type]` (`0x005ce648`: "hand", "eye", "mouth", "door",
"stairs", "hourglass", "swords") from the play screen's archive becomes
the corner picture (`0x006563c4`), priority 0. `DrawMouseCursor` draws
the corner picture **instead of** the arrow and clears it every frame
(MouseCursor_SPEC §5). [C] (`1998 cursor.cpp:155-166`.)

Since Animate runs only with no button held, and every left up clears
DragObj, an inventory hover always passes `with = NULL`. [I]

### 6.3 What each item shows (CursorType)

| Class | Function | Result | 1998 |
|---|---|---|---|
| FOOD, POTION | `0x0050ebe0` | no `with`: 2 "mouth"; with: -1 | `food.h:27`, same |
| SCROLL | `0x00521c90` | no `with`: 1 "eye"; with: -1 | `scroll.h:34`, same |
| CONTAINER, INVCONTAINER | `0x004dd370` | Openable or holds items (`0x00470040`, the array count), and not OF_INVISIBLE (`0x80`): 0 "hand"; else -1; `with` ignored | `container.cpp:135-141` (no invisible test) |
| MONEY | `0x00515f10` | `with` of the same class and type, not invisible: "hand"; else -1 | `money.cpp:154` also capped the amounts |
| WEAPON | `0x00528e20` | `with` named "Poison Vial", in state 0, a Type 1 (knife) weapon, not invisible: "hand"; else -1 | `weapon.cpp:45` (state VIAL_STATE_FULL) |
| VIAL RACK | `0x004ddb90` | state > 0 and (no `with`, or state < 4 and `with` "Poison Vial"), not invisible: "hand" | `container.cpp:299` |
| TCharacter | `0x004d4a00` | `with` or invisible: -1; dead (Health ≤ 0): "hand" when it holds items; alive: decided by Aggressive (`0x1a8`), not read further | `character.cpp:3986` |
| TPlayer, TOOL, everything else (ARMOR, KEY, LIGHTSOURCE, TALISMAN, RANGEDWEAPON, AMMO) | `0x005201c0`, `0x005240e0`, base `0x00477a20` | -1 | — |

So in the inventory: food and potions show the mouth, scrolls the eye,
a bag the hand (a Pouch only when it holds something: its Openable is 0
in class.def), and equipment nothing. "stairs" and "hourglass" are not
in `playscrn.dat` (MouseCursor_SPEC §11). [C]

## 7. Sounds

- `SetDragObj(item)` (`0x0043a140`) tail-calls `0x00473a10`, the item's
  class sound: MONEY "gold", RANGEDWEAPON "bow", WEAPON "weapon", FOOD
  "food", POTION "potion", SCROLL "scroll", ARMOR "armor" / "jewel" /
  "jewel2", KEY "keys" (and "viles" for a "Vial"); the branch details for
  ARMOR and TALISMAN are UNCONFIRMED. So the class sound plays when a
  drag starts and after a successful equip. [C]
- "pop": a move or swap in the inventory or belt (§3.2, §3.4, §4). [C]
- "pouch": into a bag, up to the owner, a talisman into the Spell
  Pouch. [C]
- All through the sound manager `0x00667548` (`0x0049c430` find,
  `0x0049b650` loaded, `0x0049b990` play). [C]

## 8. The items' Use

All overrides call the base first and return 1 at once when it returns
1 (a combination). [C]

### 8.1 Base Use `0x004705f0`

Multiplayer only (`0x0066829c`, `0x004705f0..0x0047067d`): a player's
flag `+0x36c` bit 2 is cleared unless the item is a bag, and the use is
offered to the network layer (`0x005847c0`, `0x00584710`), which can end
it with 1. Then `with` → object (`0x00452690`):

- with an object: this item's script gets TRIGGER_USE (7) keyed on that
  object's name, with "user" and "item" aliases;
- without: this item's script gets TRIGGER_USE with "user"; the user's
  script gets TRIGGER_USE keyed on this item's name with "item".
- **Combining** (`0x004706bc..0x004708dc`): an OF_INVENTORY `with` of the
  same owner and class, both "Combining", types one apart: both leave the
  owner (`0x60`), are put on the map and flagged KILL, and the owner gets
  an item of the higher type + 1 through `AddToInventory` by unique type
  id (vtable `0x50`). Returns 1. Otherwise 0. [C] (The count / slot
  arguments of that `0x50` call were not pinned down: UNCONFIRMED.)

1998 (`object.cpp:956-1002`) had the combining block commented out.

### 8.2 Food and potions `0x0050e780` (`TFood::Use`, POTION's too)

```
if (base Use) return 1
if (user is CHARACTER or PLAYER and user->Health() <= 0) return 0
if (with != -1) return 0
if (state != 0) return 0
if (user is CHARACTER or PLAYER):
    user->SetHealth(user->Health() + Health());   clamp to MaxHealth
    user->SetMana(user->Mana() + Mana());         clamp to MaxMana
    if (Poison()):                                 // food stat "Poison"
        user->SetPoisoned(0)
        for each EFFECT (class 25) within 0x80 of the user, attached to
            him, of kind 9: end it (0x004defe0)    // UNCONFIRMED: kind 9 = poison
if (user == player): play "food" if the item has a still inventory image
    (imagery +0xd4), else "potion"
if (owner):                                        // network guard 0x00676e5d
    if (Amount() > 1) SetAmount(Amount() - 1)
    else owner->DeleteFromInventory(name, 1)       // vtable 0x78
if (user == player): text bar "%s" FOODGOOD1
return 1
```

FOODGOOD1 is "The item was consumed. You feel rejuvenated."
(english.def). Fatigue and Cure are not read. Food not in an inventory
isn't used up. `DeleteFromInventory(name, 1)` deletes the first item of
that name in the owner's tree, which is normally this one. [C]

class.def: potions give Health and/or Mana 100 / 400 / 800 by grade;
"Cure Poison" has Poison 1; food gives small Health / Mana. [C]

1998 (`food.cpp:27-56`): no base call, no dead or `with` checks, no cure,
no sound, no text; it set state 1 ("eaten") instead of using up the pile,
and updated the health / stamina bars itself. There is no 1998
`potion.cpp`; retail POTION shares FOOD's code. [C]

### 8.3 Containers and bags `0x004dc180`

```
inst = the with object
if (base Use) return 1
if (state == 0 && CheckKeyUse(user, inst))  return 1      // 0x004dd480
if (Locked()): user == player: CONTISLOCKED; return 0
if (Openable() && state == 0):
    inst: user == player: FULLCONTCLOSED / "The X is closed."; return 0
    SetState(1); open sound by type name ("Lab..." LabChestOpen,
    "ForChest" StoneOpen, "Pot" ClayOpen, else TreasureOpen, for
    "Chest" types); Explosive / Poisonous traps; CONTOPENED; return 1
top = this item's topmost owner (none: 0)
if (top == user):                                         // in his inventory
    if (!inst && (slot < 0xff || slot > 0x10b) && !CombineID):
        if (user == player):
            if (the pane shows this bag) SetContainer(top)  // close it
            else SetContainer(this)                         // open it
        return 1
    if (CanCombine(user, with)): the TargetID item is made in the owner
        of whichever has a TargetID (AddToInventory(unique id, 1, -1)),
        and both leave their inventories; return 1
    if (NumInventoryItems() < 0xfe && inst && TargetID in {-1, 0, 1}
        && !CombineID && FindFreeInventorySlot() < 0xff):
        AddToInventory(inst, -1); user == player: "pouch"
    return 1
else:                                                     // on the ground
    no inst: take the first item (FULLCONTTAKEN...), or close
    inst: put it in
return 1
```

Lines `0x004dc880..0x004dcabb` for the inventory half. A bag returns 1
even when it took nothing (full, or an ingredient), so the pane never
swaps with a bag. Pouches have Openable 0 and TargetID 1; a Flask is
Openable 1 (opened by the first click, shown by the second); vials and
herbs have a CombineID, so they never open or take items. The slot test
leaves out 0xff..0x10b: equipment and the first belt box, so a bag in the
first belt box doesn't open when clicked (looks like an off-by-one;
read, not seen in a run). [C]

1998 (`container.cpp:37-133`): no combining, no sounds, no dialog lines,
"in inventory" meant "same top container as the pane", and it opened the
bag again instead of closing it. [C]

`TContainer::AddToInventory` (`0x004ddc40`) repaints the spell pane for a
"Spell Pouch" (INVENTORY.md §2.1). A player's `AddToInventory(item, -1)`
first tries a "Pouch" whose first item has the new item's class
(INVENTORY.md §2 step 2). [C]

### 8.4 Scrolls `0x00520eb0`

With no `with` and the player as user: the scroll pane (`0x0066dcb0`),
or the book pane (`0x0066db18`) for a type name containing "Book" or
"book" (`0x0058ad30`), gets the scroll (`+0x17c`) and its prepared text
(`+0x18c`), the play screen opens it (`0x0048ed90`, `0x0048eea0`),
"scroll" plays. Returns 0, so the inventory then
"moves" the scroll onto its own cell: a no-op, and "pop" plays too. [C]
(`1998 scroll.cpp:70-84`, same apart from the player check and sound.)

### 8.5 The rest

- MONEY `0x00515e70`: a `with` of the same class and type is removed from
  the map and deleted, and this pile takes both amounts; returns 1. No cap
  (1998 `money.cpp:125` capped at MAXMONEYIMAGE). [C]
- WEAPON `0x00528d80`: `with` named "Poison Vial" in state 0 on a Type 1
  weapon: the vial goes to state 1 (`+0x18`), "%s envenomed."; 1. [C]
- TOOL `0x005240c0`: the base, as 0/1 (1998 `tool.cpp:20` returned FALSE
  without the base, so no scripts). [C]
- KEY, ARMOR, LIGHTSOURCE, TALISMAN, RANGEDWEAPON, AMMO: the base (scripts,
  combining). A key acts as the `with` of a locked container's
  `CheckKeyUse` (`0x004dd480`: "unlock succeed" / CONTUNLOCKED, "unlock
  failed" / CONTWRONGKEY; tools pick, CONTPICKFAIL / CONTPICKTOUGH). [C]
- TPlayer `0x005201b0`: 0. [C]

## 9. The port

Port = `feature/gameflow`, 2026-10-13. The HUD's item input stays in the
side panel (`HandleMouseClickUISidebarModeConsumed`,
`src/uisidebartest.cpp`), which hit-tests the cells, boxes, doll slots and
the container icon; `UIDragState` (`src/uidragstate.cpp`) acts on the
items. One grab (source pane, cell, item, the source pane's container)
stands in for the panes' grab and held slots.

| Behaviour | Retail | Port |
|---|---|---|
| Click an inventory or belt item → `Use(player, -1)`; drag onto an item → `dst->Use(player, src)`; else a move through `AddToInventory` + "pop" | §3.2, §4 | `UIDragState::Release` → `ReleaseInPane` |
| Belt item onto the inventory, inventory item onto the belt (Use, else move; never into itself) | §3.4, §4 | `ReleaseFromPane` |
| Doll item onto a cell or box (unequip, or trade places via CanEquip) | §3.4, §4 | `ReleaseWorn` |
| Pack or belt item dropped anywhere on the doll → own eqslot, class sound | §5.1 | `EquipInOwnSlot` (any carried item, bags included) |
| Right up: talisman → Spell Pouch, else equip; no item → `CompactInventory` | §3.6, §4 | `UIDragState::RightClick`, inventory and belt boxes |
| Container icon: up to the owner, or the grabbed item up into it | §3.3 | `ReleaseOnContainerIcon` |
| The inventory shows the opened bag; its cells and drags read the shown container | §1 | `TInventory::Pulse`, the side panel's left down |
| Drag threshold 2 px; the doll's grab is a drag at once | §3.1, §5.1 | `kDragThresholdPx`, `BeginDragLayers` |
| `SetDragObj` on the drag (the class sound; `with` for CursorType) | §7 | `Promote`; `SetDragObj` plays `PlayItemSound` |
| Hover: `CursorOverObject` for the inventory, belt and doll | §6 | `AnimateUISidebarItemCursor`, from the game mode's Animate off the map |
| Food / potion Use | §8.2 | `TFood::Use` |
| Container Use: the scripts, CONTISLOCKED, the carried half (open/close, combine, take with "pouch") | §8.3 | `TContainer::Use`, `UseCarried` |
| CanCombine; AddToInventory by unique id | §2, §8.3 | `TObjectInstance::CanCombine`, `AddToInventoryByUniqueId` |
| Container CursorType tests OF_INVISIBLE | §6.3 | `TContainer::CursorType` |
| CanEquip: weapon mask, MinStrength, MinConstitution and their messages | §5.2 | `TPlayer::CanEquip`; WEAPON's MinStrength and ARMOR's MinStrength / MinConstitution are registered stats now |

The corner picture and the released drag bitmap now end as the next frame
begins (`BeginCursorFrame`) rather than inside the cursor's draw: the port
draws a frame's HUD again for captures (the mirror pass), which before
showed no hover cursor at all.

Deviations:

- Two ingredients that combine (§8.3) are deleted; retail took them out of
  their inventories and left them nowhere.
- The belt's right up off its boxes does nothing; retail read equipment
  slot 10 there (`0x10a`), so it compacted the inventory, or re-equipped
  what is worn in that slot with its sound.
- After removals, `CompactInventory`'s order can differ from retail's: the
  port's inventory array leaves holes that new items fill, retail's closed
  up on every removal.

Not ported yet:

| Behaviour | Retail |
|---|---|
| Right up in stats mode: the item's information | `0x00538267..0x00538333`, `0x0052cff9..` |
| The walk-root refresh after (un)equipping a light source | `0x004cf000` |
| Container Use: opening (sounds by type, the Explosive and Poisonous traps, the dialog lines) and the half on the ground | `0x004dc22a..0x004dc87d`, `0x004dcac0..` |
| Base Use combining (no shipped item has a nonzero "Combining") | `0x004706bc..0x004708dc` |
| Money Use without the 1998 cap; Tool Use calling the base | `0x00515e70`, `0x005240c0` |
| Dragging an item from the HUD onto the map or a world object (`TMapPane` still reads `TInventory::GetHeldSlot`, which is -1) | not covered here |
| Multiplayer: the CanCombine gate, the network messages | `0x0066829c` |

## 10. Not settled

- What `player+0x29c` holds (the doll's no-slot hover reads it, §6.1).
- The name of the CanEquip weapon mask field (§5.2).
- Effect kind 9 being poison (§8.2).
- The arguments of the base Use's combine `AddToInventory` (vtable
  `0x50`, §8.1).
- The class sound's ARMOR / TALISMAN / KEY branches (§7).
- The order in which panes get a release (TPlayScreen's pane list); the
  held slots make the result the same either way. [I]
- Not covered: dragging items from the HUD onto the map or onto a world
  object (the map pane's drop), and picking up from the map.
