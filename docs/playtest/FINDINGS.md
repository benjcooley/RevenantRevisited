# Playtest findings

Bugs and gaps that Jev's sessions surfaced in the games, with what reproduces
each. Newest first. Each entry: what was seen, the evidence, the repro, and
who owns the fix.

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
