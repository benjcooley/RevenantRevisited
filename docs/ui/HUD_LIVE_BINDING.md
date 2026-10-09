# HUD live binding — the panes read the main player

Status: done (2026-10-05): every HUD pane reads the main player; the
`--test=ui-*` modes read a demo player. Owner: gameflow track.
Context: [ARCHITECTURE.md](ARCHITECTURE.md), [../gameflow/ARCHITECTURE.md](../gameflow/ARCHITECTURE.md) §5,
[CLASSIC_HUD_REFERENCE.md](CLASSIC_HUD_REFERENCE.md), the per-pane specs in [forensics/](forensics/).

## 1. Problem

`TPlayScreen` embeds the `--test=ui-hud` harness as its HUD
(`InitializeUIHudMode` / `RenderUIHudModeEmbedded`). That harness was
stage 1 of the UI plan: every pane rebuilt standalone, each driven by its
own sample data. Stage 2, binding the panes to the game, never happened, so
a new game shows the sample data instead of Locke:

| Pane | Data it shows today | Where the data lives |
|---|---|---|
| Status bar | Locke L26 1833/2174/191, a "Vermis L18" second card | `g_player` / `g_target` in `uiplyrstatusbartest.cpp`, synthetic driver |
| Stat sheet | the `sample_screen_1.jpg` numbers | `SPlayerSheet g_player` in `uistatstest.cpp` |
| Equip | its own spawned TPlayer wearing a Brown Leather set | `SpawnBodyInstance` / `SpawnDemoSlots` in `uiequiptest.cpp` |
| Inventory, belt | spawned sample items, 14475 gold | `UIDragState::harness_inv/harness_barinv/harness_equip` |
| Quick spells, spell book | Advanced Healing, Iron Skin, Fire Flash, Ice Bolt (+ Heal) | binding tables in `uiquickspelltest.cpp` / `uispellbooktest.cpp` |

The `harness_*` arrays are worse than sample data: they are a second
inventory model that drag/drop mutates, unrelated to any player.

## 2. Retail: the HUD reads the player

Every retail HUD pane reads the global player `DAT_00667fcc` (the port's
`Player`, maintained by `TPlayerManager`) when it paints; none keeps a copy
of game data:

| Pane | Retail read | Cite |
|---|---|---|
| Status bar | player vtable `+0x1c0/+0x1c8/+0x1d0` (Health/Fatigue/Mana), `+0x1d8/+0x1e0/+0x1e8` (max), `+0x354` Level, name; **target = `player->root->obj` when root action is COMBAT (3) or BOW (0x19)** = port `TCharacter::Fighting()`; a vanished target keeps drawing from its map index (`+0xb8`) while it fades | slot23 `FUN_0054af20:61-71,623-628`; slot19 `FUN_00549da0` |
| Stat sheet | `FIELD x` → pane resolver `FUN_00547240` (`train*`, `objdesc`, weapon type, `stmod*`) → object virtual `+0xc8` chain: TPlayer `0x0051dfb0` (`class`) → TCharacter `0x004d5260` (`armor`, `maxhealth/fatigue/mana`, `attackpct`, `defensepct`, `damage`, `stealth`) → TObjectInstance `0x00472f80` (`name`, `objtype`, `objclass`, `statmod`, `experience`, else the object stat of that name) | decompiled 2026-10-05, §6 |
| Inventory | `player->GetInventorySlot(page + col*3 + row)`; gold = `(*player+0x84)("Gold")`, the port's `GetInventoryAmount` (the player has no Gold stat); count = `Amount()` when > 1 | InventoryPane_SPEC §5, §8 |
| Equip | `player->GetEquip(i)` icons; the paperdoll draws the player's model with the pane's own animation counters (`mbr_0xbc/0xc4`, anim `"walk"` from `DAT_005e4060`) | EquipPane_SPEC §5, §9 |
| Belt | items in the player's inventory with `inventnum = 0x10b + N`; a `"Pouch"` shows its first item's icon and its item count | BarInvPane_SPEC §5 step 7 |
| Quick spells | `player + 0x2cc + slot*6`: talisman code per button (port `TPlayer::GetQuickSpell`) | QuickSpellPane_SPEC §1 |
| Spell book | `player + 0x2ec` known-spell list (talisman codes) | SpellbookPane_SPEC §1; SAVE_GAME §11.4 |

One inventory carries everything: grid slots are `inventnum` 0..,
equipment `256 + EQ_*` (`TPlayer::RefreshEquip`), the belt `0x10b + N`.

## 3. Design

**The subject is the main player.** Panes read `Player` exactly where retail
reads `DAT_00667fcc`. No binding API, no per-pane copy, no "is this a demo"
switch: a change made by a script, the console or gameplay is on screen the
next frame because the pane reads the object every frame (retail compares
cached values each paint for the same reason). References a pane must keep
across frames (the fading-out target, the paperdoll's meshes) are
`TSafeRef`s, so a deleted object resolves to null.

**The harness supplies the subject.** `src/uidemoplayer.{h,cpp}` builds a
real `TPlayer` ("Locke") carrying the sample content the panes were built
against — stats, equipment, inventory, belt, gold, quick spells, known
spells — and installs it as the main player through `TPlayerManager` for
the `--test=ui-*` modes that read a subject (`testmodes.cpp` does this
around the mode's Initialize/Close; the pane code never calls it). It also
owns the sample opponent and the demo-only motion the status bar used to
synthesize (the opponent's bars sweep, the target comes and goes) by
driving the real objects through their public API (`SetHealth`,
`BeginCombat`, `SetFighting`).

**Item cells act on the subject.** The `harness_*` arrays go. Inventory,
belt and equip slots resolve their item from the player's inventory, and a
committed drag moves the item there: `SetInventNum` to the destination slot
(swapping with an occupant), and `TPlayer::Equip` for equipment slots,
following the 1998 `TEquipPane::MouseClick` algorithm the retail panes
share (`invslot.h` banner). Icons come from the item (`InventoryImage`,
plus the item's `invanim` stepped on the legacy 24 Hz tick).

**Objects answer the stat sheet.** The retail field resolver is an object
virtual; the port gains `TObjectInstance::GetFieldText` with the
TCharacter / TPlayer overrides, ported from the decompiles. Branches whose
inputs the port's object model does not have yet (`statmod`,
`attackpct`, `defensepct`, `damage`, `stealth`) return "not resolved", which is what retail does for an unknown
field; each is listed in §6.

## 4. Order of work

Each step builds, keeps the game and the test modes running, and is
verified in game (`--quickstart`, filmstrip) and in its `--test=ui-*` modes.

1. This document. (0f90749)
2. Demo player fixture; the status bar reads `Player` / `Fighting()`. (a1653a9)
3. `GetFieldText` on the objects; the stat sheet resolves its fields
   through it. (b456098)
4. `TPlayer::Equip` keeps retail's inventory bookkeeping (the equipped
   item moves to `0x100 + slot`, the displaced item takes its place), so
   the panes and drag/drop can rely on slot numbers; `equip` / `unequip`
   console commands ported. (73f7618)
5. Inventory, belt and equip slots read the player's inventory; drag/drop
   commits into it; `harness_*` removed; the paperdoll draws the player
   (pane-owned state, rebinds when the player or its equipment changes).
   (c3ec07a)
6. Quick spells and the spell book read the player (`GetQuickSpell`, the
   known-spell list); `SpellList` loads in `TPlayScreen::Initialize`.
   (8ede9f9)

Deliberately not in this change: moving the pane code out of the
`ui*test.cpp` files into production pane classes and retiring
`InitializeUIHudMode` from `TPlayScreen` (gameflow ARCHITECTURE §9 step 4).
After this change the pane code reads only the subject and the demo data
lives only in the fixture, so that move is mechanical.

## 5. Deviations from retail

Also in the commit messages.

- **Portrait fallback.** Resolved 2026-10-09 (HUD_REBUILD.md): the status
  bar draws the character's InventoryImage as retail does. With
  `T3DImagery::GetInvImage` answering the state-0 icon whatever the state
  (retail `0x0040ce60`), characters have it, and the `LockeFace` fallback is
  gone.
- **Paperdoll pose.** Retail animates the paperdoll through its own
  counters (`mbr_0xbc/0xc4`) with an animation chosen through vtable
  `+0x131`, which is not identified. The pane holds frame 0 of the
  recovered `"walk"` state (`DAT_005e4060`), the pose the harness showed;
  stepping the walk cycle turns the model, so it is not the idle retail
  plays.
- **Armor.** `armor` is `ArmorValue()` (the player's: ACBonus plus the
  Protection of the armor worn, `0x00519850`), plus the main player's
  DmgResMisc (vtable `+0x260`); the armor of the effects on the character
  (`FUN_005407d0`, a list at `+0x170` the port doesn't have) is not
  ported. The other unported resolver branches are in §6; they draw
  nothing, as retail does for an unknown field.
- **Spell lookup.** A talisman code is matched to its spell.def variant
  exactly. `TSpellList::GetVariantDataByTalismans` compares talisman
  counts, not order, so it maps DEB ("Advanced healing") to BED ("Restore
  Life"). Which lookup retail's panes use is not confirmed.
- **Spell icon key.** SpellIcons.dat names some circles by variant
  ("Advanced Healing") and some by spell ("Iron Skin"); the panes try the
  variant name, then the spell name, ignoring case
  (QuickSpellPane_SPEC UNCONFIRMED-D). Lead for confirming it: spell.def
  gives each spell an `ICONNAME`, which retail's loader parses
  (`meth_0x53e4e0`, `"ICONNAME %30s"` into the spell record's `+0x5c`) and
  the port's `SSpellData::Load` skips. The Heal spell's is `"Heal"`, so if
  the panes key by `ICONNAME` the "Advanced healing" ring shows the Heal
  circle, not the "Advanced Healing" one.
- **Quick-spell labels.** The variant name splits at its first space into
  the label above and below the ring, the rule the harness used. Retail's
  wrap rule is not confirmed.
- **Spell book text.** The description is the spell's `DESCRIPTION`
  (shared by its variants), word-wrapped into the (54,4,85,86) cell;
  skill and mana are the variant's. Labels are the dialog tags
  `SPANESKILLS` / `SPANEMANA`.
- **Spell drops.** Dropping a spell on a ring only logs: the spell book
  doesn't start drags yet, and a ring-to-ring drag's retail effect is
  unknown.

Test-mode output that changed because the data is now real, not because
the panes changed:

- Food and potions show no count: their classes have no `Amount`.
- The belt pouch's overlay is its first potion (retail rule) instead of a
  copy of the pouch.
- The stat sheet's modifier column and the `0`s after the train arrows are
  gone: those fields don't resolve, and retail draws nothing for them.
- The demo player's maxima come from the port's rules for a level-26 Locke,
  so the bars are filled to the sample's ratios, not its numbers.
- The spell book rows show spell.def's descriptions, skill and mana.

## 6. Field resolver coverage

| Field | Retail | Port |
|---|---|---|
| `train<stat>` | `"+"` when the stat is a pending level-up choice (`player[0xd8/0xd9]`, Level < 15) | `""` — the port has no level-up choice state |
| `objdesc`, weapon type, `stmod<stat><n>` | pane-level strings / stat modifier | not ported (Page1 does not use them) |
| `class` | class name | `chardata->classdata->name` |
| `maxhealth`, `maxfatigue`, `maxmana` | `+0x1d8/+0x1e0/+0x1e8` | `MaxHealth()` / `MaxFatigue()` / `MaxMana()` |
| `armor` | `+0x2bc` plus two player bonuses (`FUN_005407d0`, `+0x260`) | `ArmorValue()` + DmgResMisc; `FUN_005407d0` not ported |
| `damage`, `attackpct`, `defensepct`, `stealth` | combat formulas | not ported (inputs unidentified) |
| `name` | localized name (dialog tag of the name's letters/digits, else the name) | same |
| `objtype`, `objclass` | localized class name | same |
| `statmod` | modifier list | not ported |
| `experience` | a CHARACTER's kill experience for the main player (`0x0051a5b0`) | same |
| anything else | object stat of that name | `GetStat` when the stat exists (e.g. `nextexp`, a PLAYER class stat) |

## 7. Open questions (in-game behaviour recon doesn't settle)

Current behaviour is kept until these are answered.

1. Paperdoll: does retail's paperdoll animate (an idle loop), or hold one
   pose? Which animation?
2. Answered by the emulator (2026-10-09): retail draws whatever the
   character's +0x130 image is, which is its state-0 icon. With none, the
   ring is empty. The port now does the same.
3. New game: Locke starts with 0 of 105 mana (newgame.sav stores 0). Is
   that retail?
4. Answered by the decompile (gameplay/forensics/PLAYER_STATS.md §7):
   `Nxt` is the PLAYER stat `NextExp`; retail's kill-experience level-up
   (`0x0051a630`) sets it to the next level's figure, `playerlevel` leaves
   it. Ported.
5. Quick spells: do two-word names always split at the first space, and
   does dragging one ring onto another swap them?
6. Spell icons: does "Advanced healing" show its own circle or Heal's?
   (Settles whether the panes key icons by `ICONNAME`; §5.)
