# Playtest findings

Bugs and gaps that Jev's sessions surfaced in the games, with what reproduces
each. Newest first. Each entry: what was seen, the evidence, the repro, and
who owns the fix.

## F11. A pickup into a full pack put the item back (port bug, fixed)

**Seen 2026-10-11**, reading for GRIND.md §4.3, which left it [L]: "the
item probably vanishes in retail; the port puts it back".

**Retail.** Pickup (`0x004cfef0`) takes the item off the map
(RemoveFromMap, vtable `0x90`), then AddToInventory (`0x0046f3d0`). With no
free carried slot (0–254) the add fails before any merge, so even gold
that could join a pile fails. Pickup then only prints CARRYFULL2. Nothing
puts the item back: it is off the map and in no inventory.

**Evidence.** The `left the keep` waypoint: the Arakna killed at the
prompt (F8), the pack filled to slot 254 with 250 `@addinv "Club"` lines,
Locke put 40 west of the Arakna's 56 gold and turned to it, then G.
- Retail: "Get Gold", "56 Gold Picked up.", "Can't carry any more."; the
  gold is gone from the floor and not carried (gold 0).
- Port before: the same lines, and the gold back on the floor.
- Port after: as retail.
- A left click on the gold was tried first; neither game's hover pick
  found the coins at their projected pixel, so G (F10) stands in.

**Resolution.** Fixed on `playtest/loot`: Pickup no longer puts the item
back. It prints CARRYFULL2 and deletes the item, which retail leaks; it is
out of reach either way. Pickup also plays the item's class sound now.

## F10. The U and G keys did nothing (port bug, fixed)

**Seen 2026-10-11**, reading for GRIND.md §4.2: the port's
`TPlayScreen::Command` dispatched neither Use nor Get, and its control
table had no Combat Get.

**Retail.** Command (`0x0047cf40`) runs Use (`0x004d01e0`) for command 0xe
and Get (`0x004d0430`) for 0xf. The control table (`0x005d5500`) binds Use
to U and T (normal mode), Get to G (normal mode) and Combat Get to G
(combat and bow modes).
- Use: the nearest container, exit, or character neither the user nor its
  enemy, within 60 of the point 60 ahead; nothing flagged invisible. "Use
  <name>" or "Talk to <type>", or "There is nothing to use".
- Get: the nearest object within 80 of the point 40 ahead that shows a
  cursor or is an item, not the user, nothing invisible. "Get <name>" or
  "There is nothing to get", then Pickup, which takes only items: a nearer
  chest or door blocks it with "Get <name>" and nothing else.

**Evidence.** The `left the keep` waypoint, the retail emulator and the
port fed the same input. Each target was set up at the prompt: Locke put
30 west of it (`@pos`), turned to it (`@faceobject`), then the key.

| Step | Retail | Port before | Port after |
|---|---|---|---|
| G by the dead Arakna's gold | "Get Gold", "56 Gold Picked up.", gold 56 | (no drop, F8) | as retail |
| C, then G by its Bread (combat mode) | "Get Bread", "Bread Picked up." | (no drop, F8) | as retail |
| U by the chest ForChestE (3488, 23216) | "Use gothic chest", "gothic chest opened.", open | nothing | as retail |
| G by a Club put down 10 away (`@addat`) | "Get Club", "Club Picked up." | nothing | as retail |

**Resolution.** Fixed on `playtest/loot`: Command dispatches both, the
table has Combat Get, and TryUse / TryGet are retail's Use and Get (the
1998 versions searched 60 ahead for both, printed fixed English and
skipped the invisible and enemy tests).

The first run found a second bug: Use answered "There is nothing to use"
beside the chest. `TMapPane::FindObjectsInRange` kept the 1998 insertion,
which copied bytes rather than entries of its distances and never moved
the ids. With two or more objects in range the list held stale ids. Retail
(`0x00452060`) moves both, nearest first; so does the port now. This
affects every caller: AI searches, spells, missiles, the console.

**Open.** `addat` puts a new object at the camera's height. After the same
input the port's camera stood at z 41 and retail's at 32, so the Club came
in at different heights (the camera, not loot).

## F9. Corpses never went away (port bug, fixed)

**Seen 2026-10-11**, reading for GRIND.md §3.1: a corpse lay in the port
until its sector reloaded.

**Retail.** TCharacter::Pulse (`0x004c1bb0`) counts a dead character's
pulses (`+0x228`). On the 240th it starts the fade the character draws
with, `SetFade(100, 1, 0)`: fully seen, then 1 less a pulse. When the fade
reaches 0 it sets KILL, and the next PulseObjects (`0x004580f0`) deletes
the object, unless it is a player.

**Evidence.** The `left the keep` waypoint; Locke put at (3361, 23235),
the Arakna (3269, 23173) killed at the prompt with `@arakna.stat health =
0`; 400 ticks observed one by one.
- Retail: the corpse is listed through tick 339 after the kill and gone at
  tick 340 (two runs, Locke at (3017, 23195) and at (3361, 23235)).
- Port before: the corpse is listed all 400 ticks.
- Port after: gone at tick 340, as retail. In the frames it is drawn whole
  until tick 240, faint by 320 and gone by 338.
- The emulator's frames show the corpse whole until it goes. Its machine
  runs retail's software renderer, whose rasterizer most likely ignores
  the material alpha TCharAnimator sets (`0x004d82a0`) [L]; on a 3D card
  retail blends the corpse out [L]. The port draws that fade, as it does
  the invisibility spell's.

**Resolution.** Fixed on `playtest/loot`: the dead branch counts the
pulses (`deadticks`), fades and KILLs as retail. PulseObjects no longer
deletes a KILLed player (retail fades and KILLs a dead player too, and
keeps him).

**Open.** A character killed by setting its health (the prompt's
`stat health = 0`, a script) plays no death in the port: the dead branch
forces the 1998 "blood" block, where retail's builds `<root> to cdead`,
then `cdead` or `dead` (`0x004c21e9..0x004c243d`). In the emulator the
Arakna's corpse moves a few units as its death plays; the port's stands
still. Deaths by Damage are unaffected.

## F8. A dead monster kept its loot (port bug, fixed)

**Seen 2026-10-11**, reading for GRIND.md §4.1: the port left a dead
monster's things in its corpse, to be taken a click at a time.

**Retail.** On every pulse a dead character other than the player holds
anything, Pulse calls DropInventory (`0x004d6570`):
- The "gold" sound plays.
- Every item it can trade (vtable `0x5c`: Value ≠ −1, every container but
  the Swag Bag) goes to the floor:
  - gold at the corpse's x + 30, y + 30;
  - anything else a few 16-unit steps out in a random direction;
  - then its class sound (`0x00473a10`).
- Each item is thrown there by its animator (`0x00445ef0`, pulse
  `0x00445b20`): raised by its registration height + 2, it settles a step
  short of its spot and faces where it spun to.
- Every item has an animator: `TAnimImagery::NeedsAnimator` (`0x00418fc0`)
  says yes for any item, 2D ones included.

**Evidence.** As F9, with a Short Sword added to the Arakna first
(`@arakna.addinv "short sword"`); it carried Bread and 56 gold.
- Retail, on the tick of the kill: Gold at (3299, 23203, 35), the corpse +
  (30, 30); Bread at z 50 and the sword at z 36, each about 48 units out.
  The Bread moves 8 units on to rest at tick 37, the sword 17 units at
  tick 41. The corpse holds nothing. (A second run, Locke at (3017,
  23195), threw them elsewhere: Bread 12 units on at tick 37, the sword
  14 at tick 59.)
- Port before: nothing on the floor for 400 ticks; the things stay in the
  corpse.
- Port after: Gold at the same spot; Bread at z 50 and the sword at z 38,
  48 units out; they settle 15 units on, at ticks 57 and 59. The
  directions and settling times differ from retail's because each game
  draws its own random numbers by then.

**Repro.** Through either host, from `left the keep`: one tick with Enter,
the line `@pos 3361 23235 32` and Enter (a line opens, is typed and runs in
one tick: a monster reaching Locke mid-line puts him in a fight, which
commits it half typed); 3 ticks; the same with `@arakna.addinv "short
sword"`, then with `@arakna.stat health = 0`; observe each tick. The port's
INI needs retail's `[Controls]` (Combat Mode on C), or Enter toggles
combat.

**Resolution.** Fixed on `playtest/loot`: `TCharacter::DropInventory`, the
toss on `TObjectAnimator`, `PlayClassSound`, `TMoney::AddToMap` (the pile's
state first) and retail's `TAnimImagery::NeedsAnimator`. Where the
corpse's own spot fails the walk test, retail tries new directions for
ever; the port leaves the item at the corpse.

**Open (drawing, for the renderer's owners).**
- The flight isn't drawn. Retail draws the item along its arc with a
  four-copy trail (`0x0040f5b0`); the port shows it at its spot until it
  settles.
- The gold pile isn't drawn at all. Coin.I3D carries a particle controller
  the port hasn't audited (`[partsys] unsupported runtime type=Gold`), so
  the renderer suppresses the whole mesh rather than draw a substitute.
- A 2D item put on the map in play isn't seen either: the dropped Bread,
  and a Bread or Apple put down with `@addat`. The renderer's rebuild
  collects them with their still image (checked with a log); they are lost
  later, in the draw. A chest loaded with the sector draws. In the
  emulator all of them show.

## F7. Jong's lessons never completed: `player.lastattack` wasn't ported (port bug, fixed)

**Seen 2026-10-11**, reading for GRIND.md §2.3 and §10. Each of Jong's
lessons (`forest.s:794-1690`) loops on `if player.lastattack = "<move>"`
until Locke lands the move. The port answered the member with "isn't
ported" and failed the expression. So a lesson never ended, and MUDOKON1's
ALWAYS block re-added and re-equipped the Training Sword every second.

**Retail.** The member is in the expression evaluator (`0x0041f230`), at
`0x0041f755`:
- On a character or player whose last attack (`+0x160`) is set: Get,
  WhiteGet, an optional `=` (Get, WhiteGet past it), then the value is
  `Is(lastattack->name)` on the next token (`0x00479700`, case-blind) when
  the hit flag `+0x168` is set, else 0. A final WhiteGet ends it.
- Anything else gives 0 after one WhiteGet. The `=` and the name are then
  left to the evaluator, which compares 0 with the name's hash.
- ResolveAttack (`0x004c6dd0`) sets `+0x160` and clears `+0x168` on an
  attack's first tick. At the impact frame `+0x168` is whether the attack
  struck its main target (ResolveHit `0x004c62b0`).
- **Emulator.** On the forest slot, retail's own CommandInterpreter
  (`0x0041e8e0`) ran each line as the text bar's `@` prompt does
  (`0x0054d700`), through `PlayWorld.peek`, with `+0x160` and `+0x168` set
  per case. Results for `if player.lastattack = "CHARGETHRUST"` (0x40 true,
  0x80 false):

  | Last attack | Hit | Line | Result |
  |---|---|---|---|
  | none | – | as above | 0x80 |
  | chargethrust | 0 | as above | 0x80 |
  | chargethrust | 1 | as above | 0x40 |
  | chargethrust | 1 | `= "chargethrust"` | 0x40 |
  | chargethrust | 1 | no `=` | 0x40 |
  | thrust | 1 | as above | 0x80 |
  | none | – | `… and 1` | 0x40 |
  | chargethrust | 0 | `… and 1` | 0x80 |
  | chargethrust | 1 | `… and 1` | 0x40 |

  The `and 1` rows show the token handling. With no last attack, the `=`
  is left to the evaluator, which compares 0 with the name's hash, and
  `and 1` then tests the hash: true. With a missed attack, the member
  takes the `=` and the name itself: false.

**Resolution.** Fixed on `playtest/items`.
- `MemberValue` (`src/scriptvalue.cpp`) evaluates `lastattack` as
  `0x0041f755` does, token for token.
- `TCharacter::LastAttack` and `LastAttackHit` (`src/character.h`) expose
  `+0x160` and `+0x168`. ResolveAttack already kept both.
- **Port, the same lines from `--exec`** on the forest slot. Before any
  attack: plain 0x80, `and 1` 0x40. After C then A with no one in reach
  (a thrust that strikes nothing): `= "THRUST"`, `= "thrust"`, no `=`,
  `and 1` and `= "CHARGETHRUST"` all give 0x80. These match retail's rows.
- **Port, lesson 1 end to end** (`forest`, Locke placed near Jong,
  `JONGMEETSTATE` 1, choice 2):
  - The script sets ATTACKLEVEL to 1. Control returns with the Training
    Sword re-equipped every second.
  - Practice on the dummy needs it as a target (see Open). The harness
    made it fight Locke (`@mudokon1.beginfighting locke` each round; it
    took on the fourth). Then C targeted it, and A from 260 units charged
    it.
  - When a Charging Thrust struck, the loop took `jump complete1`, printed
    "EXCELLENT !" and set EQUIPSTATE 0. Locke bowed. The script unequipped
    and deleted the Training Sword ("1 TRAINING SWORD deleted") and set
    LEARNSTATE 1. Jong said II2JON17/18.
  - No ADDINV or EQUIP of the sword followed. 240 ticks later Locke held no
    weapon.
  - The pre-fix binary, given the same inputs, logged "'.lastattack' isn't
    ported" and never left the loop. The sword was re-equipped 83 times in
    3,966 ticks. (In that run the dummy never became Locke's target, so it
    shows the failing expression, not a landed move being ignored.)

**Open (retail, not run end to end).** The emulator ran about 2.5 ticks a
minute on the loaded machine, so the whole lesson (about 2,400 ticks) wasn't
played in retail. The practice target is the open question:
- Retail's IsEnemy (`0x004c89c0`, which the port matches) counts the dummy
  only while it fights Locke. Its type `Mudukon` has no GROUPS, and Locke's
  ENEMIES (`player,trainer,human,…`) don't name it. In the port, C left
  Locke without a target, as that reading predicts.
- A with no target picks `thrust`, the first button-1 attack, because the
  distance test is skipped without a target (COMBAT_ATTACK_CHOICE §3.1
  item 8). The thrust strikes no one and never completes a lesson.
- In the port, `@combat jong1` let Locke attack Jong (`trainer`,
  aggressive), and Jong fought back.

How a retail player was meant to land the move is a question for a retail
play-through.

## F6. Weapon and armour numbers came from class.def (port bug, fixed)

**Seen 2026-10-11**, reading for GRIND.md §5.1 and §10. The port's item
prices, damage, Protection, MinStrength and MinConstitution were class.def's
TYPES rows. Retail overwrites them at load with `weapon.def` and `armor.def`
BASICMODS. The Short Sword cost 100 instead of 195. The Club had Damage 0 and
MinStrength 48, so it could never be equipped (retail: 14 and 16). Cloth
armour gave no Protection and asked for Str 16 and Con 12 (retail: 1, 14 and
10).

**Retail.**
- BindTypes (`0x0048cab0`) walks every type of every class. For each it
  calls BindType (`0x0048c930`), then BindTypeData (`0x0048c7f0`) with the
  type's name.
- BindTypeData takes the first class (by id) with a type of that name. For
  WEAPON it finds the `weapon.def` entry of that name (`_stricmp`) and runs
  `0x0048af30`. For ARMOR it finds the `armor.def` entry and runs
  `0x0048b4a0`. Other classes are left alone.
- Those two write each BASICMODS field into the type's class stat of that
  name: FindStat on the class's type stats (`+0x14`), then the type's stats
  (`+0x34[type] → +0x10`). Weapons, in file order: EqSlot, Type, Damage,
  Combining, Poison, Value, DamageType, MinStrength. Armour: EqSlot,
  Protection, Combining, ResistPoison, Stealth, Value, MinStrength,
  MinConstitution. The armour loader (`0x0048b1a0`) stores Stealth at
  `+0xb4` and Value at `+0xb0`, and the copy reads them back by name, so
  the file order holds. SaleType and SwipeFull keep class.def's values.
- BindTypes runs at the end of LoadClasses (`0x0047654f`) and after a rules
  reload (`0x0047c9a0`, `0x0047cb97`).
- **Emulator.** The clickwalk build's boot image stops after game init, so
  class.def is loaded and BindTypes has run. Read from the class registry
  (`0x0065a148`): WEAPON has 69 types, ARMOR 206. Every row equals class.def
  with its entry's BASICMODS laid over it by the mapping above (275 of 275).
  60 weapon rows and 203 armour rows differ from class.def. Types with no
  entry keep class.def's numbers: the daggers, OgrokAxe, the sword parts,
  Amulet of Rogoss, Diamond Ring and Wolfsbane Pendant.
- **Emulator, on items.** On the `forest` slot, GetStat (vtable `+0xd4`,
  `0x00473600`) on Locke's own items gives the same numbers. The Short
  Sword has Damage 13, Value 195 and MinStrength 16. The cloth shirt and
  pants have Protection 1, Value 10, MinStrength 14 and MinConstitution 10.

**Port before the fix.** `TRules::BindType` bound only CHARACTER and PLAYER
types. Nothing copied the item numbers. Checked on 27 types (14 weapons, 13
armour, including Dagger and Amulet of Rogoss), every stat through the
item's GetStat: 196 of 257 values equal retail, 61 differ.

**Repro (port).** On the `forest` slot, add an item, name it, and test a
stat against retail's: `--exec 'sleep 24; add "Short Sword"; "Short
Sword".name it1; if it1.stat value = 195'`. The exec log shows
`(result=0x40)` when the condition holds and `0x80` when it doesn't.
(`it1.stat value` alone prints "Bad parameters." from `--exec`, because
the queue ends each line with a newline token.)

**Resolution.** Fixed on `playtest/items`.
- `TRules::BindTypeData` (`src/rules.cpp`) is retail's `0x0048c7f0`. It
  writes the entry's BASICMODS into the type's stats by name. BindTypes
  calls it after BindType for every type, as retail does.
- After the fix, all 257 sampled values equal retail.
- `crosscheck.py --slot` on the `forest` slot (both games, one tick) shows
  no item variance. Every carried item's numbers match: the Short Sword
  13 / 16 / 195 and the cloth pieces' Protection, requirements and Value.
  The run's five variances are screen positions and the click grid.
- Everything that reads these stats now sees retail's numbers: shop prices
  (`TBuySellPane::BuildItem` reads `Value`), CanEquip's MinStrength and
  MinConstitution, weapon Damage and armour Protection. So does the stock
  filter the town scripts give Gina and Cronus (`minstrength` in [1,
  VALUE], `town.s:1830`, `2039`).

## F5. The right-button walk stopped steering over the HUD (port bug, fixed)

**Seen 2026-10-11.** With the right button held, moving the pointer over
the side panel left Locke walking his old way. In retail he turns
toward the pointer there as anywhere.

**Evidence.**
- `TScreen::MouseMove` (`0x00490530`) gives every pane every move with
  no rect test (`0x004905b9..0x0049063d`).
- `TMapPane::MouseMove` (`0x004504d0`) steers the right-button walk
  (`0x0044ee00`) and aims a drawn bow at the pointer wherever it is.
- Its map point, `posx − pane.x + cursorx`, has no bounds.
- The port gave the world a move only over the play field.
- Its pointer-to-map conversion (`TMapRenderer::ScreenToMapScreen`)
  wrote nothing past the view's edge, so a point there read as the map
  origin.
- **Retail** (play slot, `opening`): right held at (120, 240), then 12
  ticks at (560, 240) over the side panel. Locke moves (+20, −20), the
  same as with the pointer at (380, 215) inside the view, under the
  wedge cursor.
- **Port before the fix:** (0, +68); he kept his old heading.

**Repro (port).** As F4. Compare the 12 ticks with the pointer at
(560, 240) against the same with it at (380, 215).

**Resolution.** Fixed on `playtest/tabs`.
- `TPlayScreen::MouseMove` gives the world every move. A modal holding
  the mouse and a running load still stop them.
- The HUD's own move handling (item drags, the spell book, the paper
  doll, the screen's panes) is unchanged.
- `ScreenToMapScreen` writes the map point past the view's edge too, and
  still returns false there; `ScreenToMapPos` uses it.
- After the fix, the port moves the same over the panel as inside the
  view: (+59, +16) at both points, wedge cursor over the panel.
- Drags, click-walk and tab clicks are unchanged.

**Open (movement, not routing).** For the same pointer, the two games
head differently. Retail turns in place and moves (+20, −20) in 12
ticks; the port moves (+59, +16), whether the pointer is in the view or
not. That is a variance in the right-button walk itself (its direction
sector or its turn) and belongs to whoever owns player movement.

## F4. A right-button walk released over the HUD never stopped (port bug, fixed)

**Seen 2026-10-11**, while checking where retail sends button releases.
Hold the right button on the play field so Locke walks, move over the
side panel, and let go there. Locke kept walking until the next right
click.

**Evidence.**
- `TScreen::MouseClick` hands a release (4, 5, 6) to every pane,
  whatever the point (`0x004904ef..0x004904fc`).
- The map pane's right release (`0x0044f917..0x0044f96a`) ends the walk
  with no rect test: the arrow cursor comes back, the held direction key
  is released, and `+0x11c` is cleared.
- Its left release off the pane only clears `clicked` (`0x0044fbad` →
  `0x0045046f`).
- On a side tab's button it does nothing (`0x0044f17b`).
- The port routed a release by its point, like a press. One over the
  side panel or the bottom bar went to the HUD only, so the map kept
  `walking` and its held key.
- **Retail** (play slot, `opening`): right held at (120, 240), then over
  the side panel at (560, 240), let go there. Locke stops (0 units in
  the next 24 ticks).
- **Port before the fix:** he walked on 131 units.

**Repro (port).** From `opening`, through the port host:
`{"mouse": [120, 240], "buttons": 2}`, 12 ticks later
`{"mouse": [560, 240], "buttons": 2}`, 12 ticks later
`{"mouse": [560, 240], "buttons": 0}`, then step 24 and compare
`player.pos`.

**Resolution.** Fixed on `playtest/tabs`.
- `TPlayScreen::MouseClick` also gives the world every release, wherever
  it lands. The exceptions are a release on a side tab's button and one
  that ends a HUD item drag, which the drag's drop takes.
- The map's own cases do the rest, as in retail.
- After the fix: Locke stops (0 units).
- Unchanged before and after: drags pack → belt, belt → pack, a drag let
  go over the map (cancelled; see Open), a click on a pack item, and
  click-walk and tab clicks.

**Open (not routing).** Retail drops an item dragged out of the HUD onto
the map, or uses it on the object there: `0x0044fff8`, the `FULLSINGDROP`
/ `FULLMULTDROP` messages (MAP_INPUT.md §4.3). The port's HUD drag
(`UIDragState`) cancels instead and the item goes back.

## F3. The side panel came back from the save (port bug, fixed)

**Seen 2026-10-11.** `crosscheck.py --from port:forest` listed the four
pack cells as variances. The port showed Book and Map with the inventory
hidden; retail showed Equip and Inventory.

**Evidence.**
- The `forest` save's HUD words (the player record, SAVE_GAME.md §11.4)
  say Book / Map. The port applied them on every load
  (`TSaveGame::RestoreHud`, a deliberate divergence).
- Retail only writes them. `TPlayer::Load` keeps them at `+0x304..+0x310`
  and nothing reads them. Entering play (`TPlayScreen::Initialize`
  `0x0047a660`) sets the side pane to Equip and Inventory (`0x0047ae4c`,
  `0x0047ae57`) and opens the side panel and the bottom bar (`0x0047ae7a`,
  `0x0047ebc0(1)`).
- An in-game load runs in the pulse (`0x0047bf01..0x0047bfd9`). It leaves
  the side panel as it was, and `LoadGame` closes the bottom bar
  (`0x0047ece0`; AUTHOR_QUESTIONS.md 81).
- Emulator, play slot (SAVE_GAME.md §11.4a). Booted, the words are 0.
  Set to Stats / Map before a start, nothing writes them, yet the panes
  show Equip and Inventory. Map picked in play and then an in-game load
  of the same slot: still Map, with the bottom bar closed.
- The fourth word, once "unidentified", is `+0x6ac`: the bottom drawer
  is open.

**Repro (port).** `crosscheck.py --from port:forest`: the `pack slot N`
variances. In the port host, from `forest`, pick the Map tab and type
`@loadgame "Port Resave"` at the text bar. Before the fix, the load put
Book / Map back; retail keeps Map.

**Resolution.** Fixed on `playtest/tabs`.
- `TPlayScreen::Initialize` resets the side panel to Equip and
  Inventory, open, with the bottom bar.
- Loads no longer apply the words. The port still writes them, the
  fourth from the drawer.
- `CloseDrawer` closes the bottom bar as well as the shop, so an in-game
  load leaves the bar closed.
- Retail bug, not reproduced: after tabs were changed in a session,
  entering play again presses the old tabs over Equip and Inventory
  (§11.4a).
- After the fix, the `forest` crosscheck has no pack variances. Through
  the port host: Equip and Inventory on entering play, Map kept across
  an in-game load, bottom bar closed.

## F2. Clicks around the side tabs never reached the world (port bug, fixed)

**Seen 2026-10-11.** `crosscheck.py --from port:opening` showed 32 grid
cells in the strip x 384..448, y 176..416 where a left click reached the
world in retail but not in the port.

**Evidence.**
- `TMapPane::MouseClick` leaves a press to the side tabs only when one
  of their buttons holds it: `0x004364d0` on `0x0065be50`, asked at
  `0x0044f17b`.
- The buttons are plain 36×35 rects, because their flags (`0x180010`)
  lack the pixel-test bit `0x20000` of `0x0042d0d0`. The 2 px gaps and
  the strip's margins belong to the map.
- The port gave the HUD a 64×240 box there.
- The port also drew the strip 4 px left and 2 px up. Retail lays it
  flush in the play field's corner (Pulse `0x0047bc1a..0x0047bc3d`): at
  (400, 188) with both panels open. A pixel probe of retail's routing
  gave x 403..438 and y 190+37n..224+37n for the buttons.

**Repro (port).** `crosscheck.py --from port:opening` before `dd4b8f71`:
`clicks world port=0 retail=32`.

**Resolution.** Fixed in `dd4b8f71`.
- `TPlayScreen::ClickRoute` asks the strip's own hit test
  (`UISideTabButtonAt`).
- Moves and the hover cursor stay the map's over the whole strip.
- The strip sits flush in the corner.
- After the fix: no `clicks world` variance; the port's pixel runs are
  retail's; a click in a gap walks Locke and a click on a tab switches it
  (MAP_INPUT.md §4, §7.1).

## F1. A dead character saved and loaded is gone (retail behaviour)

**Seen 2026-10-10.** Jev killed Rahul from the `opening` waypoint and the
director quick-saved at once (waypoint `rahul defeated`). Played from that
waypoint, Tendrick's scene never starts, so `ressexit` stays locked ("It seems
to be locked") and the Keep chain stops.

**Evidence.**
- The saved sector `2_1_0.DAT` holds Rahul (type `Rahul`, `<type name>`) with
  `HLT=0`, `state 19`, root action `combat` (revsave dump of the slot).
- After loading the slot, `rahul.stat` answers `rahul: Context not found`,
  and TendrickR's trigger line `If Rahul.stat health = 0` logs `Bad
  parameters` every pass, so its block never runs.
- Without a save and load in between, the same check passes and the scene
  plays (sessions `watch1`, `p3`).

**Cause: retail removes a character loaded dead.** The port follows it
([SAVE_GAME.md](../gameflow/forensics/SAVE_GAME.md) §11.3, "A character
loaded dead"):
- `TCharacter::Load` (`0x004d4eb0`) sets KILL (`0x1000`) on a character
  whose health is below 1, whether it comes from a save or a sector
  reloaded during play. The port does the same (`src/character.cpp:5451`).
- The first `PulseObjects` (`0x004580f0`) deletes it (`0x00451840`). The
  port nukes it there too (`src/mappane.cpp:3809`).
- With `Rahul` gone, the expression evaluator (`0x0041f230`) skips the
  rest of the line with no value and returns 0. `if` (`0x0041fb70`) then
  returns `0x84`, and the interpreter prints "Bad parameters." The port does
  the same (`src/scriptvalue.cpp:386`, `src/command.cpp:1225`).
- **Retail, run on the same slot.** The emulator's play slot loaded `Quick
  Save 1`. Rahul came in with flags `0x0006d028` (the saved `0x0006c028`
  plus KILL) and was deleted on the second tick. TendrickR's `if` returned
  `0x84` with "Bad parameters." at ticks 25, 100, 125, … 225. Control
  stayed on, and no scene ran in 240 ticks.
- **Retail host gap found on the way.** As committed, the host
  (`playtest/retail`) starts every slot as a new game: retail reports
  "Unable to load saved game... starting new game". Two things cause it.
  The slot list that QUICKSTART's name is looked up in (`0x0048d6d0`) was
  built before the slot was placed, and the HUD machine's `winfs` matches
  `*.*` only against names that contain a dot, so even a rescan
  (`0x0048d260`, `Save\Single\*.*`) misses `Quick Save 1`. The run above
  rescanned after placing the slot and gave `*.*` the Windows meaning.

The window is narrow in retail too. It runs from the blow that leaves Rahul
at 0 health to TendrickR's next pass of the `If`, about 25 ticks. The `If`
turns control off, and while control is off retail ignores the Quick Save
control and the menu's Save
([INGAME_MENU.md](../gameflow/forensics/INGAME_MENU.md) §2–3). Once the scene has run
`ressexit.stat locked=0`, a save is safe, because the door keeps the lock
in its saved stats.

**Repro (port; matches retail).** Waypoint `~/RevenantRevisited-testsaves/waypoints/port/rahul-defeated--20261010-231733`:
copy its `slot/Quick Save 1` into `<save>/Save/Single/`, then
`REVENANT_SAVE_PATH=<save> build/Revenant --headless --quickstart="Quick Save 1"
--fixedstep --seed=1 --max-runtime=25 --exec "rahul.stat; sleep 24; rahul.stat"`.
The first `rahul.stat` lists `Flags: MOVING AI KILL ANIMATE PULSE COMPLEX
NOTIFY`. The second answers `rahul: Context not found`.

**Resolution.** This is retail behaviour, so the engine is unchanged. The
playtest saves after Tendrick's scene: `rahul defeated` is
`after_scene(character_dead('Rahul'))` (director.py), done once the scene
has played and control has been back for two seconds. Waypoints from before
that change (`rahul-defeated--20261010-230646`, `-231018`, `-231733`) are
the half-settled kind and should not be played from.
