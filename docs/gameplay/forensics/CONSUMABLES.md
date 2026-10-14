# Forensics: eating and drinking (food and potions)

**Topic:** what the shipped game does when the player eats food or drinks a
potion: how it is triggered, what it changes, what it plays and prints, and
how the item is used up. Burndown row G.3.
**Status:** 2026-10-13. Forensics complete; port in `src/food.cpp`,
`src/player.cpp`, `src/playscreen.cpp`, `src/uidragstate.cpp`; checked in the
retail emulator (§10).
**Evidence:** the unchanged retail image (`28bec273…`), read in the baseline
listing (`recon/retail_asm/baseline`) and Ghidra decompiles; the shipped
`class.def` (imagery.rvi), `english.def` (resources.rvr) and `quickload.dat`
(imagery.rvi); runs of the original functions in the in-process emulator
(`tools/retail_runtime/slots/items/food_use.py`). Strings: `0x5e1974`
"food", `0x5e197c` "potion", `0x5e1984` "FOODGOOD1", `0x5e1990` "%s",
`0x5e4230` / `0x5e3800` "pop".

## 1. Function table

| Retail | Identity | Notes |
|---|---|---|
| `0x0050e780` | `TFood::Use(user, with)`, vtable `0xbc` | FOOD (`0x5b2cd4`) and POTION (`0x5b2ee4`) share it; thiscall ret 8 |
| `0x004705f0` | `TObjectInstance::Use` | the USE script triggers, item combining; returns 0 for a plain use |
| `0x0050e750` | FOOD's Amount object stat | `AMT`, id 0, default 1, 1..1000 |
| `0x0050eae0` / `0x0050eb20` | TFood Load / SetAmount | Load makes a 0 amount 1; SetAmount repaints the pack and the shelf |
| `0x00477950` | `DeleteFromInventory(name, n)`, vtable `0x78` | `GiveInventoryTo(null, name, n)` `0x0046fd30` |
| `0x004defe0` | `TEffect::KillThisEffect` | already in the port (`src/effect.cpp`) |
| `0x0044ceb0` | map iterator over a square round a point | `(pos, range, 0xe0, objset, 0, level)` |
| `0x0046de60` | approximate 2D distance | the port's `::Distance(a, b)` |
| `0x0049c430` / `0x0049b650` / `0x0049b990` | SoundPlayer FindSound / load / Play | the port's `PLAY()` |
| `0x0049d800` | DialogList GetLine (`0x65d4d0`) | |
| `0x0054d170` | TTextBar::Print (`0x65c5d0`), cdecl varargs | |
| `0x00538210` | InventoryPane MouseClick | the pack's click (§7.1) |
| `0x0052cfb0` | TBarInvPane MouseClick | the belt's click (§7.2) |
| `0x0051e420` | `TPlayer::UseBelt(n)` | Belt Use 1–5, F5–F9 (§7.3) |
| `0x00519ea0` | `TPlayer::EquipByName(name)` | UseBelt's weapon / armour branch |
| `0x00420050` | the script command `use` | `obj.use [with]` |

## 2. TFood::Use (`0x0050e780`)

```
Use(user, with):
    if TObjectInstance::Use(user, with): return 1                 // 0x0050e792
    if (user && user is a CHARACTER (0x0c) or PLAYER (0x0b)       // 0x0050e7a9
             && user->Health() < 1)                               //   slot 0x1c0
       || with != -1                                              // 0x0050e7cf
       || this->state != 0:                                       // 0x0050e7d8 (+0xc)
        return 0
    if user is a character or player:                             // 0x0050e7e3
        user->SetHealth(user->Health() + this->Health())          // 0x1c4 / 0x1c0
        if user->MaxHealth() < user->Health():                    // 0x1d8
            user->SetHealth(user->MaxHealth())
        user->SetMana(user->Mana() + this->Mana())                // 0x1d4 / 0x1d0
        if user->MaxMana() < user->Mana():                        // 0x1e8
            user->SetMana(user->MaxMana())
        if this->Poison() != 0:                                   // 0x0050e898, food slot 0x1f0
            user->SetPoisoned(0)                                  // 0x0050e8ac, slot 0x1b4
            cure the poison spells on the user                    // §4
    sound = (this->imagery && this->imagery->GetInvImage(0, 0))   // 0x0050e95f..0x0050e981
            ? "food" : "potion"
    if user == Player:                                            // 0x0050e983 / 0x0050e9b1
        id = SoundPlayer.FindSound(sound)                         // 0x0050e995 / 0x0050e9c3
        if id >= 0 and SoundPlayer.Load(id):                      // 0x0050e9a6 / 0x0050e9d4
            SoundPlayer.Play(id, 0x7f, 1, NULL, 0x50, 700)        // 0x0050e9f0
    if this->owner (+0x64):                                       // 0x0050e9f5
        (network guard 0x00676e5d set)
        if this->Amount() <= 1:                                   // 0x0050ea07, slot 0x198
            owner->DeleteFromInventory(this->name, 1)             // 0x0050ea2b, slot 0x78
        else:
            this->SetAmount(Amount() - 1)                         // 0x0050ea18, slot 0x19c
        (network guard cleared)
    if user == Player:                                            // 0x0050ea35
        TextBar.Print("%s", DialogList.GetLine("FOODGOOD1"))      // 0x0050ea57
    return 1
```

What it does **not** do:

- **Fatigue, Cure and Fill are never read.** An apple's 50 fatigue does
  nothing. The "Poison" stat is the cure flag: the Cure Poison potion has
  Poison 1, Cure 0.
- **No animation.** The user's state and actions are left alone, and so is
  the item's (the 1998 `TFood::Use` set it to state 1; retail never does).
- **An item lying on the map is not used up** (no owner): its stats, its
  sound and the message all happen and it stays. Retail never gets there
  from play: a click on an item on the floor picks it up (§7.4).
- A null user is read as a character before the test (`0x0050e7e3` reads
  `user+4`): retail would fault. No caller passes one.
- The cap compares after adding, so **a stat already above its maximum is
  brought down to it** by any food, even one that adds nothing to it.
- `with != -1` refuses: food can't be used *with* another item. A pack
  drag of one item onto food lands in AddToInventory instead (§7.1).

## 3. The numbers

Retail's FOOD stat registration (static init `0x0050e5ec..0x0050e771`, one
`SStatEntry` each, ids in order) — the POTION class copies FOOD's at
class.def load, being based on it:

| Id | Stat | Code | Default | Min | Max |
|---|---|---|---|---|---|
| 0 | Value | VAL | 0 | −1 | 1000000 |
| 1 | Health | HLTH | 0 | 0 | 1000000 |
| 2 | Mana | MANA | 0 | 0 | 1000000 |
| 3 | Fatigue | FATG | 0 | 0 | 1000000 |
| 4 | Poison | PSN | 0 | 0 | 1000000 |
| 5 | Cure | CURE | 0 | 0 | 1000000 |
| 6 | Fill | FILL | 0 | 0 | 1000000 |
| 7 | SaleType | STYP | 0 | 0 | 3 |
| obj 0 | Amount | AMT | 1 | 1 | 1000 |

The accessor slots read them by those ids: `0x1c0` Health
(`0x0050ec20`, `DAT_0066d2e4`), `0x1d0` Mana (`0x0050ec50`,
`DAT_0066d260`), `0x1f0` Poison (`0x0050ecb0`, `DAT_0066d2ec`); the
names the ids are registered under were read from the registration
pushes (`0x5e18f0..0x5e196c`).

The user's slots (TCharacter `0x5a7848` / TPlayer `0x5b4f30`): `0x1b0` /
`0x1b4` Poisoned (object stat 1), `0x1c0` / `0x1c4` Health (object stat
3), `0x1d0` / `0x1d4` Mana, `0x1d8` MaxHealth, `0x1e8` MaxMana (a player's
are the level formulas `0x00520630` / `0x00520770`, a character's its
chardata).

The shipped items (`class.def` 384–402, 3535–3544; type rows are the stats
in id order):

| Item | Class | Health | Mana | Other | Sound (§5) |
|---|---|---|---|---|---|
| Ale Mug | FOOD | +20 | | | food |
| Apple, Banana, Celery, Grapes, Orange, Watermelon | FOOD | +10 | +20 | Fatigue 50 (unused) | food |
| Bread | FOOD | +40 | | | food |
| Carrot, Lettuce | FOOD | +10 | +30 | | food |
| Cheese | FOOD | +20 | +10 | Fatigue 10 (unused) | food |
| Chocolate | FOOD | +15 | +15 | | food |
| Drum Stick, Meat, Sausage | FOOD | +50 | | | food |
| Fish | FOOD | | +50 | Fatigue 10 (unused) | food |
| Milk Bottle | FOOD | +15 | | | food |
| Muffin | FOOD | +30 | +10 | | food |
| Wine Glass | FOOD | +30 | | | food |
| Lesser Healing / Mana / Restoration | POTION | +100 / – / +100 | – / +100 / +100 | | potion |
| Greater Healing / Mana / Restoration | POTION | +400 / – / +400 | – / +400 / +400 | | potion |
| Enchanted Healing / Mana / Restoration | POTION | +800 / – / +800 | – / +800 / +800 | | potion |
| Cure Poison | POTION | | | Poison 1: cures | potion |

Every FOOD and POTION type uses a `Misc\*.I2D` (2D) imagery.

## 4. Curing poison (`0x0050e8a6..0x0050e95e`)

With `Poison() != 0`, after `user->SetPoisoned(0)`:

```
for each object of the pulse set (objset 4) in the square user.pos ± 0x80
        on the user's level, no inventories                  // 0x0044ceb0(pos, 0x80, 0xe0, 4, 0, level)
    if Distance(user.pos, obj.pos) <= 0x80                    // 0x0046de60, `< 0x81`
       and obj is an EFFECT (class 0x19)                      // 0x0050e911
       and obj.spell (+0xd8) is set
       and the spell's first target (count +0xc >= 1, +0x10) is the user
       and the spell's data (+0x11c) has DAMAGETYPE 9 (poison, +0x80)
        obj->KillThisEffect()                                 // 0x004defe0
```

`KillThisEffect` cures the spell's first target again when the spell has a
POISONCHANCE, kills the effect and ends its spell. The iterator's
arguments are CharBlocking's (`0x004d4dc5`: `pos, 0x80, 0xe0, 2, 0, level`)
with the pulse set in place of the characters, which the port already
maps to `TMapIterator(level, &rect, CHECK_NOINVENT, objset)`
(`ForCharactersNear`, `src/character.cpp`).

## 5. The sound

The choice is the item's look, not its class: an item whose imagery has a
**still inventory picture** for state 0 is eaten ("food"); one shown in the
pack only by an **inventory animation** is drunk ("potion"). Every FOOD
type has a still picture; every POTION type has only an animation (the
port's `TObjectInstance::InventoryImage` notes the same). Both files ship
in resources.rvr, `Sound/effects/food.wav` (11 034 bytes) and
`Sound/effects/potion.wav` (45 460). The play is the plain one-shot:
full volume, not positioned (with no position `0x0049b990` never takes
its 3D path, so 0x50 / 700 don't apply).

The 2D imagery's GetInvImage (`0x00419ef0`, slot `0xd4` of `0x5a384c`)
answers the entry's **icon copy** (`entry+0x74`) when it has one, else the
state's own still picture (body `+8 + state·0x14`). The copy is made by the
tail of `TObjectImagery::LoadBody` (`0x00447ccf`): state 0's still picture,
else frame 0 of its inventory animation. So once a potion's imagery has a
copy, GetInvImage is not null and the potion plays "food". Whether it has
one depends on how its body was loaded:

- `LoadBody` called while the body is missing or loading (a synchronous
  load, `0x00447ac0` with wait) makes the copy at its end. The emulator
  shows it: a fresh Lesser Healing's first GetInvImage returns null and
  makes the copy; the second returns the copy (§10).
- The loader thread's load (`0x0044786x`: body `+0x60`, status 3) doesn't.
  A body it loaded first is never copied by the 2D accessors, which call
  `LoadBody` only when the body is missing (`0x00419ecc`, `0x00419f03`,
  `0x00419f3c`, `0x00419f6c`).
- quickload.dat would restore the copies at boot (reader `0x00447e10`,
  version 3, `0x00446270(..., icon, -1)`), but the shipped one is version 2
  (header `HDRS`, 2, 2900 entries; each has a 40×40 icon) and is refused.
- `0x004464b0` (an icon by imagery id, which forces the copy) is called
  from `0x004429d0` and `0x004690f2`, in editor code.

In play a potion's body usually arrives by the loader thread first
(`NewObject` leaves it requested, status 1), so potions sound as potions;
one whose first access won the race sounds like food from then on. The
port's 2D imagery keeps no icon copy, so its rule gives "food" for food
and "potion" for potions every time (§9).

## 6. The message and the stack

- The text bar shows **FOODGOOD1**, "The item was consumed. You feel
  rejuvenated." (english.def line 207), for the main player only.
  FOODGOOD2 ("The useful portions of the item have been consumed…") is
  never referenced by the executable.
- **One unit is used.** A stack of more than one loses one
  (`SetAmount`). The last unit leaves through the owner's
  `DeleteFromInventory(name, 1)`: the first item of that name in the
  owner's inventory, bags included (`GiveInventoryTo` → `FindObjInventory`
  by name), is taken — normally the item itself; with two stacks of one
  name in one owner, the first found loses the unit. Nothing is left
  behind: no apple core, no bottle (whatever the manual says).
- The network guard `0x00676e5d` (set around the removal) only tells the
  multiplayer messages not to echo it.

## 7. Trigger sites

### 7.1 The pack (InventoryPane `0x00538210`)

- **Left down (event 1)** on a cell: the cell's slot is remembered
  (`+0x180`, `+0x184`), the press point (`+0x190`, `+0x194`), drag flag
  `+0x18c` = 0.
- **Move** (`0x00538c10`): a move of 2 pixels or more on either axis
  (`|x − startx| >= 2 or |y − starty| >= 2`) starts a drag (`+0x18c` = 1).
- **Left up (event 4)** on a cell, the press on a cell
  (`0x00538846..0x005388da`): the items at the press slot and at the
  release slot; **not dragged and the same item: `item->Use(Player, -1)`**
  (`0x005388d2`). A dragged item released on another item: that item's
  `Use(Player, dragged item's id)`; in a network game that one asks the
  item's `CanUse` (slot `0xb8`) first. If Use returns 0 (or the cells differ):
  `container->AddToInventory(item, release slot)` (into its own slot, which
  does nothing) and **"pop"** (`0x005388e0`, `0x5e4230`).
- **Right up (event 5):** equip it in its own slot (its `eqslot`
  stat), or `EQUIPUNABLE` on the text bar; a food has no slot, so food
  answers EQUIPUNABLE. A talisman goes into the Spell Pouch.

### 7.2 The belt (TBarInvPane `0x0052cfb0`)

The same rules on the belt's boxes (slot `0x10b +` box, press slot
`+0x64`, drag flag `+0x6c`, 2 pixels in its move `0x0052d6e0`): the use at
`0x0052d409`, the "pop" at `0x0052d425` (`0x5e3800`).

### 7.3 F5–F9 (`TPlayer::UseBelt` `0x0051e420`)

Retail's control table (`0x005d5500`, 76-byte entries) has entries 16–20,
"Belt Use 1".."Belt Use 5" / `BeltUse1`..`BeltUse5`, on `VK_F5`..`VK_F9`,
in the normal, combat and bow modes (7), commands `0x18`..`0x1c`.
`TPlayScreen::Command` (`0x0047d882`): with a player,
`Player->UseBelt(command − 0x18)`.

```
UseBelt(n):                                       // 0x0051e420
    item = GetInventorySlot(0x10b + n)            // 0x004701f0
    if !item: return
    if item is a WEAPON (1) or ARMOR (2):
        EquipByName(item->name)                   // 0x00519ea0: the first item of that name
                                                  // in the inventory, its EqSlot (slot 0x188);
                                                  // the equipped one takes its inventory slot;
                                                  // Equip (0x005199b0)
    else:
        item->Use(this, -1)
    the shelf is redrawn (0x0065b078)
```

So a potion or food directly in a belt box is drunk or eaten by its key; a
Pouch in a box is "used" (opened) instead.

### 7.4 Elsewhere

- **The world:** a click on an item on the floor picks it up
  (`TMapPane::MouseClick` `0x0044f140`; MAP_INPUT.md §4.3), it is not
  eaten; it shows the hand, as any item does (MAP_INPUT.md §3).
- **Dropping food on Locke** (a drag let go over him on the map): `Locke
  ->Use(player, food)` = `TPlayer::Use` (`0x005201b0`), which returns 0:
  nothing is eaten. The paperdoll (`0x005363e0`) never calls Use.
- **Scripts:** `obj.use` (`0x00420050`) calls the object's Use with the
  context character as user.
- **The Use key** (`TCharacter::UseNearest` `0x004d01e0`) only looks at
  containers, exits and characters.

## 8. 1998 against retail

The 1998 `TFood::Use` (`src/food.cpp` before this port) added Health and
Mana with caps for a character, set the item's state to 1 and returned
true: no cure, no sound, no message, no unit used, no dead-user or `with`
test, and no USE triggers. Its stat list registered Fill at id 5, over
Cure (class.def then appended Cure at 6, so the type rows' Cure and Fill
columns were read swapped; all zero in the shipped data). Retail added
the potion class (`POTION`, based on FOOD), the stack (`Amount`), and the
rest of §2.

## 9. The port

- `TFood::Use` is §2 in order, through the virtual stat accessors every
  object has (`TObjectInstance` `Health`, `SetHealth`, `MaxHealth`, …),
  as retail calls them; the cure scan walks `TMapPane::ForObjectsIn`
  (the retail iterator's port, now shared with FindObjectsInRange, which
  honours the A/B seam).
- The FOOD stat ids are retail's (Fill 6, Value's minimum −1).
- Clicks: a press and release on one pack or belt cell with no drag is
  `UIDragState::CompleteClick`, which uses the item (`Use(Player, -1)`) or
  asks for "pop"; the drag starts at 2 pixels, as in retail's panes (it was
  4).
- `GAMECMD_BELTUSE1..5` ("Belt Use 1".."5", `BeltUse1`..`5`) on F5–F9 call
  `TPlayer::UseBelt`. The port's developer reload of the last save, which
  had taken F9, is on Shift+F9.
- **Divergences** (each marked `REVSYNC-DIVERGENCE` in the code):
  - The sound follows retail's rule over the port's imagery, which keeps
    no icon copy: potions always "potion" (retail: usually, §5).
  - A null user is refused (retail would fault).
  - No network guard (single player).
  - A press and release on different cells without a drag (possible only
    across a cell edge within 2 pixels) does nothing; retail moves the item
    into the release cell with a "pop".
  - Retail's pack and belt moves ask for "pop" too (silent); the port's
    drop keeps its `open.wav` stand-in (`uidragstate.cpp`, the UI track's).

## 10. Verification

- **A/B** (`python3 tools/retail_ab/retail_ab.py item-food-use`): retail's
  `0x0050e780` in the emulator over the HUD slot's booted world (real items
  from retail's NewObject, a fixture user whose stats are the case's;
  `tools/retail_runtime/slots/items/food_use.py`), and the port's
  `TFood::Use` (`Revenant --retail-ab=item-food-use`,
  `src/retailab_items.cpp`) on the same 49 cases: every FOOD and POTION type
  of the shipped class.def, the caps (over, exactly at, a stat above its
  maximum), stacks, the belt, a pouch, items on the map, a dead user (player
  and character), `with`, a non-player user, a player who isn't the main
  one, the cure (poisoned or not, player or character). Compared: the
  return, every stat written in order, the sounds looked up, the text-bar
  lines, the cure scan's square, objset and level, and the item afterwards
  (still held, its amount). **2026-10-14: 49 match, 0 differences** (retail
  `28bec273…`). The retail side also records Play's arguments: always
  `(id, 0x7f, 1, NULL, 0x50, 700)`. A food in a non-zero state can't be
  staged on the port side (its imagery has one state, and SetState refuses
  others), so that test rests on `0x0050e7d8`.
- **Icon copy** (in the emulator, booted world): a Bread and a Lesser
  Healing from retail's NewObject (bodies requested, status 1, no copy);
  the 2D GetInvImage (slot `0xd4`) twice each. Bread: its picture, then the
  copy. Lesser Healing: null (the body loads, the copy is made), then the
  copy.
- **In game** (post-opening save, headless, `--exec` and `--input-script`):
  Health 40, Mana 30, Bread, three Apples, a Lesser Healing given. A click
  on the Bread: `[food] Locke used Bread (food): health 40 -> 80`, the cell
  empty, FOODGOOD1 on the text bar. A click on the Apples: 80 → 90, mana
  30 → 50, the stack shows 2. The Lesser Healing dragged to belt box 0,
  then F5: `Lesser Healing (potion): health 90 -> 100` (held to Locke's
  maximum, 100), the box empty. A click on the Short Sword: nothing used,
  nothing moved. Captures checked by eye.

## 11. Gaps

- The race in §5 can't be settled without the real loader thread's
  timing; the DOSBox lab could record which wave plays (not done).
- `EquipByName`'s search by name (UseBelt's weapon branch) finds the first
  item of that name; the port equips the item in the box itself (the same
  item unless two items share a name).
