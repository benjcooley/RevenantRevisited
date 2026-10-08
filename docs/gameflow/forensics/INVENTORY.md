# Inventory — forensics

How retail Revenant puts an item into an object's inventory, takes it out,
gives it away and merges it into a pile of its kind: `TObjectInstance`'s
inventory virtuals and the item classes' overrides. Input for the port of
the inventory model. Companion to [SAVE_GAME.md](SAVE_GAME.md) (how
inventories are stored), [COMMAND_SYSTEM.md](COMMAND_SYSTEM.md) (`addinv`,
`delinv`, `give`, `take`) and the shop
([../../ui/forensics/BuySellScreen_SPEC.md](../../ui/forensics/BuySellScreen_SPEC.md)).

Sources: Ghidra on `data/Revenant.exe` (authoritative); the 1998 source
(pre-release) for names. Decompiles are in `recon/discovered/` as
`cls_<Class>_<Name>_<addr>.cpp`; where the decompiler drops blocks or
confuses stack slots, the disassembly is beside them
(`cls_TObjectInstance_AddToInventory_46f3d0.disasm.txt`,
`cls_TObjectInstance_InventoryWalks.disasm.txt`). The vtable slots of every
object class are in `recon/discovered/inventory_VTABLES.txt`.

Confidence: **[C]** read in the decompile or the disassembly, or seen in a
run; **[I]** inferred.

## 0. Summary

- **Adding moves.** `AddToInventory(item, slot)` takes the item out of the
  inventory it was in (unequipping it there), so callers needn't. [C]
- **Gold, food and potions merge.** Once the item has left its old
  inventory, AddToInventory asks it (vtable `0x98`) to join a pile of its
  kind in the new owner's inventory, bags included. Gold joins the first
  item named "Gold" that is money of its type; food and potions join the
  first item named as their type. The pile's amount grows, the item is
  deleted, and AddToInventory reports success. Everything else, ammo
  included, never merges. [C]
- **A player's pouch takes its kind.** With no slot given, a player's new
  item goes into the first item named exactly "Pouch" whose first item is
  of the new item's class (a belt pouch of potions takes new potions). [C]
- **The item already in a slot makes room**: it goes where the added item
  came from (a swap) when the item moved within the same inventory tree,
  else to a free slot. [C]
- **Searches see bags.** Finding by name, by class and by id, and counting
  an amount, walk the whole tree: the object's items and, depth first, the
  items inside them. Finding a free slot and the item at a slot don't. [C]
- **Ids**: an item without one gets a fresh id (`MakeIndex` `0x0044ce30`)
  when it is added. [C]
- **Multiplayer only**: a container can forward its inventory to another
  object (vtable `0x170`), which only the multiplayer Swag Bag uses; the
  network messages. Not ported. [C]
- **Port** (2026-10-05): ported onto `TObjectInstance`, `TMoney` and
  `TFood`; deviations in §6.

## 1. The virtual slots

`TObjectInstance`'s vtable (`0x5a50e8`) and every object class's (SAVE_GAME.md
§11.3 names them) were read slot by slot. The overloads sit in reverse order
of declaration, as MSVC lays them out. [C]

| Slot | Base | Role | Overridden by |
|---|---|---|---|
| `0x50` | `0x0046f7e0` | AddToInventory(type unique id, n, slot) | — |
| `0x54` | `0x0046f940` | AddToInventory(name, n, slot) | — |
| `0x58` | `0x0046f3d0` | AddToInventory(item, slot) | containers `0x004ddc40` |
| `0x5c` | `0x0046fad0` | can be traded: Value (`0x190`) ≠ −1 | containers `0x004dd3a0`: not an INVCONTAINER "Swag Bag" |
| `0x60` | `0x0046faf0` | RemoveFromInventory | TAmmo `0x004bf730`, TMoney `0x00515c00`, TTalisman `0x00523a70` |
| `0x64` | `0x0046fbe0` | MoveInventory(item, to, slot): `to->AddToInventory(item, slot)` if the item is in this tree | — |
| `0x68` | `0x00477780` | GiveWeapons(to) | — |
| `0x6c` | `0x0046fc40` | GiveInventoryTo(to, item, n) | — |
| `0x70` | `0x0046fd30` | GiveInventoryTo(to, name, n) | — |
| `0x74` / `0x78` | `0x00477970` / `0x00477950` | DeleteFromInventory(item / name, n): the gives with no recipient | — |
| `0x7c` / `0x80` | `0x00477990` / `0x004779c0` | GetInventory(index) / NumInventoryItems | — |
| `0x84` | `0x0046fde0` | GetInventoryAmount(name) | — |
| `0x88` | `0x0046fea0` | HasEmptySlot: FindFreeInventorySlot < 0xff | — |
| `0x8c` / `0x90` | `0x0046f3a0` / `0x0046f3c0` | AddToMap / RemoveFromMap | TMoney `0x00516660` (AddToMap: pile state first) |
| `0x94` | `0x0046fef0` | FindFreeInventorySlot | — |
| `0x98` | `0x0046fee0` (returns 0) | **MergeInto(new owner)** | TMoney `0x00515b50`, TFood and TPotion `0x0050ea80`, TAmmo `0x004bf680` |
| `0x9c` / `0xa0` | no-ops | inventory icon taken / released | TMoney `0x00516830` / `0x00516870` |
| `0xa4` | `0x004703c0` | FindObjInventory(class, type) | — |
| `0xa8` | `0x00470280` | FindObjInventory(name) | — |
| `0xac` | `0x00470330` | FindObjInventory(id) | — |
| `0xb0` | `0x00470480` | DropInventory(item): out, to this object's position, onto the map | — |
| `0x170` / `0x174` | return 0 / no-op | linked inventory, get / set | containers `0x004ddd70` / `0x004ddd80` (`+0xd8`) |
| `0x198` / `0x19c` | 1 / no-op | Amount / SetAmount | TMoney, TFood, TAmmo |

The containers are CONTAINER, INVCONTAINER, VIAL RACK and the EXIT family.
TCharacter and TPlayer override none of these slots. No other object
vtable overrides `0x58`, `0x98` or `0x170`. [C]

Retail TObjectInstance fields used here: `+0x38` name, `+0x40` id
(mapindex), `+0x44` sector, `+0x4c` type info (its first field is the
type's name), `+0x64` owner, `+0x68` / `+0x78` the inventory array's count
and items, `+0x7c` slot (inventnum), `+0x7e` index in the owner's array
(invindex). [C]

## 2. AddToInventory(item, slot) — `0x0046f3d0`

In order (disassembly addresses; the decompiler hides step 2):

1. **Linked inventory** (`0x0046f3db`): if this object's vtable `0x170`
   returns an object, the call goes to that object's AddToInventory and
   returns its result. Only containers return one, and only multiplayer
   sets it (§5). [C]
2. **No slot** (`slot < 0`, `0x0046f414`):
   - **Pouch** (`0x0046f41c..0x0046f4c5`): on a player (objclass 11), a
     nested walk of the player's inventory (§4) looks for an item whose name
     is "Pouch" (stricmp, string `0x005d4828`) and whose first inventory
     item (vtable `0x7c`, index 0) has the new item's objclass. The first
     such pouch gets the call, `pouch->AddToInventory(item, slot)`, and its
     result is returned. [C]
   - Otherwise the slot is FindFreeInventorySlot (`0x94`); 0xff or more
     fails (returns 0). [C]
3. **Slot range** (`0x0046f4cc`): more than 0x115 fails. Slots: 0..0xfe
   carried, 0x100..0x10a equipment, 0x10b..0x115 belt (`revdefs.h`). [C]
4. **The item at that slot** (`0x0046f504`): a direct walk finds the item
   whose slot is `slot` (with a linked inventory: GetInventorySlot
   `0x004701f0` on it). If that is the item being added, return 1 at once. [C]
5. `item->OffScreen()` (vtable `0x128`). [C]
6. **Its old place** (`0x0046f544..0x0046f5b5`): if the item has an owner:
   when the top of this object's owner chain (this object if it has no
   owner) holds the item somewhere in its tree (FindObjInventory by id,
   vtable `0xac`, nested), the item's owner and slot are remembered as its
   old place. Then the item leaves its owner (`RemoveFromInventory`, vtable
   `0x60`, with the network guard `0x00676e5d` set). RemoveFromInventory
   unequips an item leaving a player's equipment slot (`0x0046faf0`). [C]
7. **Merge** (`0x0046f5bc`): `item->MergeInto(this)` (vtable `0x98`). If it
   returns nonzero: the network message (§5), the item is detached
   (`0x0046e630`) and deleted, the inventory pane (`0x0065d4f8`) and the
   belt pane (`0x0065b028`, flag `+0x50` = `0x0065b078`) are flagged for
   repaint, and AddToInventory returns 1. The item at the slot is left alone. [C]
8. **Making room** (`0x0046f636`): the item at the slot leaves
   (RemoveFromInventory under the guard). [C]
9. **Placement** (`0x0046f64f..0x0046f69c`): the item is appended to this
   object's array (fails if the array can't grow), gets an id if it has none
   (`0x0044ce30`, MapPane's MakeIndex), takes `slot`, its index and this
   owner, and leaves the map: position, level and sector 0. [C]
10. **The displaced item** (`0x0046f69f`): to the old place when step 6
    found one (`oldowner->AddToInventory(displaced, oldslot)`), else
    `this->AddToInventory(displaced, -1)`. If that fails the displaced item
    is in no inventory (lost). [C]
11. **Swag Bag** (`0x0046f6cb`): multiplayer, §5. [C]
12. The network message (§5); the inventory pane repaints when this object
    or the old owner (or their linked inventories) is the container it
    shows; the belt pane repaints when this object or the old owner is its
    character (`+0x60` = `0x0065b088`). Return 1. [C]

AddToInventory does not call SignalAddedToInventory (vtable `0x9c`); the
inventory pane calls it on the container it starts to show, and `0xa0` on
the one it stops showing (`0x005496a0`). [C]

### 2.1 The containers' AddToInventory — `0x004ddc40`

The base, then, for an object whose name is "Spell Pouch" or "SpellPouch",
the spell pane repaints (`0x00666200 = 1`, `0x006661b0` vtable `0x90`). [C]

## 3. Merging — vtable `0x98`

Called by AddToInventory alone (step 7), on an item already out of its old
inventory, with the new owner as argument. Each override calls the base
(which returns 0) first. [C]

| Class | Function | Rule |
|---|---|---|
| TObjectInstance | `0x0046fee0` | never merges |
| TMoney | `0x00515b50` | `pile = newowner->FindObjInventory("Gold")` (string `0x005e1d98`; nested). If there is one and it has this item's objclass and objtype: `pile->SetAmount(pile->Amount() + Amount())`, return 1. Otherwise: if this item's owner is the inventory pane's container, take an inventory icon for its count; return 0. [C] |
| TFood, TPotion | `0x0050ea80` | `stack = newowner->FindObjInventory(<this item's type name>)` (nested; any class). If found: `stack->SetAmount(stack->Amount() + Amount())`, return 1; else 0. [C] |
| TAmmo | `0x004bf680` | never merges: errors "Too many ammo object types" past type 15, and, when a stat of the item (vtable `0x218`, a GetStat) is set and its owner is the inventory pane's container, takes an inventory icon (for its amount if its name is "Arrow", else for 1). Returns 0. [C] |

There is no cap: piles grow without limit. Gold's money state follows its
amount: TMoney's SetAmount (`0x00515c90`), after the icons, sets state 0
below 11, 2 below 101, 4 below 301, 6 below 501, else 8, then the amount
(stat). TMoney's AddToMap (`0x00516660`) sets the same state. [C] TFood's
SetAmount (`0x0050eb20`) sets the amount and repaints the inventory pane
and belt when the item is theirs. [C]

## 4. The walk, and the searches

`0x0046dfb0` steps an iterator `{flags, root, container, linked-from,
linked-to, index, item}` (init `0x00477870`). Flag 1: after an item that
holds items, its items come next (depth first), and an exhausted bag
climbs back to the item after it, up to the root. Flag 2: also enter a
container's linked inventory (§5). [C]

| Function | Walk |
|---|---|
| FindObjInventory(name) `0xa8`, (class, type) `0xa4`, (id) `0xac`; GetInventoryAmount `0x84` | flags 3: nested (and linked) |
| FindFreeInventorySlot `0x94` (lowest slot no direct item holds), GetInventorySlot `0x004701f0`, AddToInventory's slot check | flags 0: direct |
| the Pouch search (§2 step 2) | flag 1 |

GetInventoryAmount counts each matching item's Amount, at least 1. [C]

## 5. Giving, deleting; multiplayer

**GiveInventoryTo(to, name, n)** `0x0046fd30`: n < 1 gives nothing; then
while n > 0, the item = FindObjInventory(name) (nested), stopping when
there is none; `given = GiveInventoryTo(to, item, n)`; n −= given. The
panes repaint for the last item's owner. Returns the total given. [C]

**GiveInventoryTo(to, item, n)** `0x0046fc40`: n < 1 or no item gives
nothing. amount = the item's Amount, at least 1. If n ≥ amount the whole
item leaves (`RemoveFromInventory`) and goes to `to->AddToInventory(item,
-1)` (deleted with no recipient); returns amount whether or not the add
worked. Otherwise the item keeps amount − n and, with a recipient,
`to->AddToInventory(<the giver's name>, n, -1)` (`[EBX+0x38]`, EBX = this):
a new object named after the **giver**, not the item; if that fails, 0
(the n already subtracted are lost, and the loop above calls again on the
same pile). Returns n. [C] (Question 121.)

**DeleteFromInventory** `0x74` / `0x78`: the same with no recipient. [C]

**Linked inventory** (`0x170` / `0x174`, container `+0xd8`): set only by
AddToInventory's Swag Bag step (a player with a multiplayer identity at
`+0x494` adding an item named "Swag Bag" links it to the Swag Bag of the
other player of its record, found by `0x0051f840` / `0x0051eea0`) and by
`0x0051f370` (team Swag Bags); cleared by RemoveFromInventory. In single
player it is always empty. [C]

**Network**: `0x00585880` / `0x00585ab0` (add within a tree / from
outside, on `0x00676e08`) and `0x005859c0` (remove) send nothing unless a
network game runs (`0x00676828`); the guard `0x00676e5d` (`+0x55` of the
same object) silences the remove message while AddToInventory moves an
item. [C]

## 6. Port mapping and deviations

| Retail | Port |
|---|---|
| `0x0046f3d0` | `TObjectInstance::AddToInventory(TObjectInstance*, int32_t)` (`src/object.cpp`) |
| step 9 | `TObjectInstance::PlaceInInventory` (also the UI demo kit's way to lay out items exactly) |
| `0xac` from the top owner | `TObjectInstance::Holds(item)`: walks the item's owners |
| `0x98` | `virtual bool MergeInto(TObjectInstance* newowner)`; `TMoney::MergeInto`, `TFood::MergeInto` (TPotion is a TFood) |
| `0x0046f940` | `AddToInventory(const char*, int32_t, int32_t)` |
| `0x0046fd30` + `0x0046fc40`, `0x00477950` | `GiveInventoryTo(to, name, n)`, `DeleteFromInventory(name, n)` |
| `0x0046dfb0` flags 0 / 1 | `TInventoryIterator` / `TConstInventoryIterator` with `EInvWalk::Direct` / `Nested` |
| `0xa8`, `0xa4`, `0x84` | `FindObjInventory(name)`, `FindObjInventory(class, type)`, `GetInventoryAmount`: nested |
| TMoney SetAmount `0x00515c90` | `TMoney::SetAmount`: icons, the pile state, the amount |
| `0x0046faf0` | `RemoveFromInventory` with `OnInventoryRemove` (TPlayer unequips), unchanged |
| TTalisman `0x00523a70` | `TTalisman::RemoveFromInventory`: now checks the owner first, as retail |

Deviations:

1. **Multiplayer not ported**: the linked inventory (`0x170`/`0x174`) and
   what follows it in the walks, the Swag Bag link, the network messages
   and their guard.
2. A null item, or an object added to itself, fails.
3. **SignalAddedToInventory** is still called when an item is placed: the
   port's TMoney and TAmmo inventory icons are counted there (retail
   counted them in the pane's container switch and in MergeInto's
   no-merge branch, which the port leaves out).
4. "In this tree" is answered by walking the item's owners rather than
   searching the top owner's tree for the item's id: the same answer while
   ids are unique.
5. `AddToInventory(name)` finds the type by asking each class (retail: one
   name-sorted table of every type; the same type while names are unique),
   and deletes an object it couldn't add (retail kept it, in no inventory).
6. `GiveInventoryTo`: part of a pile goes as a new object of the **item's
   type** (retail named it after the giver; question 121); if the
   recipient refuses it, the pile gets its amount back, and a whole item
   the recipient refuses returns to the giver's inventory (retail lost
   both).
7. Repaint: the port refreshes the legacy inventory pane when this object
   or the old owner is its container; the HUD's item cells rebuild from the
   inventory every frame, so retail's belt flag has no counterpart.
8. TMoney's AddToMap state (`0x00516660`) isn't ported (only SetAmount's).
9. The UI demo kit (`src/uidemoplayer.cpp`) places its items
   (`PlaceInInventory`) so its three gold piles and its potions stay apart
   as laid out.
10. Callers that read the item after adding it (container take / put
    messages, the map pane's pickup message, looting a corpse) now read it
    before: a merged item is gone.

Not changed, for the record: the port's inventory array keeps holes where
items left (RemoveFromInventory), where retail closed them up, so after a
removal an item's saved index (`invindex`) can be larger than retail's.
Both loaders ignore the saved value (LoadInventory renumbers).

## 7. Checked (2026-10-05, headless)

- `--quickstart`, `--exec "sleep 48; player.addinv 100 Gold; player.addinv 50
  Gold"`: the first is added at slot 0, the second "50 Gold merged into the
  pile in Locke, now 150".
- Retail slot `New Game1` (Locke with 601 Gold): the same two commands
  give one pile of 751 (state 8) in the save and on the inventory panel;
  before, three piles (601, 100, 50).
- Elahni (`town.s`), new game in town with 1000 gold: Buy Lesser Healing
  (gold 750: part of the pile deleted), Sell it: "75 Gold merged into the
  pile in Locke, now 825", one pile on the panel. `addinv 2 "Lesser
  Mana"` then `addinv "Lesser Mana"`: one stack of 3.
- `New Game1` next to the chest `ForChestE` (1000 Gold): `take ForChestE
  400 Gold` then `600 Gold` merge into Locke's pile (1001, then 1601) and
  the saved chest holds no gold; `give ForChestE 1 "Short Sword"`: "Short
  Sword leaves equipment slot 4, unequipped", the paper doll's hand is
  empty, the saved chest holds the sword and Locke's save doesn't.
- The opening to `SardokR: END` with no `ERROR`; the save byte checks of
  SAVE_INTEROP_TEST.md hold.
