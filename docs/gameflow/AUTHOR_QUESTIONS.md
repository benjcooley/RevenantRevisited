# Questions for the author

Things the port can't settle from the code: how the shipped game behaved,
what was intended, and retail screenshots to compare against. Each says
where it came from; the port keeps retail's behaviour until it's answered.
Answer inline (or in chat) and the owning doc gets updated.

## Retail screenshots wanted (dosbox-x)

| # | Shot | Settles | From |
|---|---|---|---|
| S1 | **Taken 2026-10-06:** `reference/opening/05_s1_pit.png` ([RETAIL_CAPTURE.md](RETAIL_CAPTURE.md) §3). Opening, Locke in the pit before he moves, `RealTimeLight=No`, `EnhancedLighting=No`, 640×480 | Classic lighting 1:1 | [LIGHTING_FIDELITY.md](../LIGHTING_FIDELITY.md) |
| S2 | Same, `RealTimeLight=No`, `EnhancedLighting=Yes` | character overbright and key light | LIGHTING_FIDELITY |
| S3 | Same, `RealTimeLight=Yes`, `EnhancedLighting=Yes` (the shipped default) | the triangle-grid look players remember | LIGHTING_FIDELITY |
| S4 | S2 settings, Locke walked from the pit toward the wall torch, one shot per character width | how lights fall off on characters | LIGHTING_FIDELITY |
| S5 | Misthaven by day, `RealTimeLight=No`, Locke facing each screen diagonal | which side the character key light hits | LIGHTING_FIDELITY |
| S6 | **Partly taken 2026-10-06** (`NoTexOverlay=No`; RETAIL_CAPTURE §3, shots 06–09: "Where am I?", Sardok's line, the choices with the side panel open, Tendrick's lines). Still wanted: the side panel closed, the hover, the click and the next ½ s, the keys. Note the retail ini's `NoTexOverlay`. Then: Locke's "Where am I?" (`I1LOC00`); Sardok's "Welcome back from the dead, Revenant." (`I1SAR00`); the three choices with the side panel open and closed (V); the mouse over "Who am I?" then off the list; a click on choice 3 and the next ½ s; Tendrick's first line; while choosing, Up Up Down Enter | box colours (the port reads Locke/choices as azure 60,175,255, Sardok green, Tendrick yellow), positions, Ring portrait, typeface and shadow, hover/click behaviour and sounds, slide on pick | [forensics/DIALOG.md](forensics/DIALOG.md) §4.2, §4.4 |
| S7 | Load the port-written save slots (`Port Resave`, `Port Played`, `Port New Game`) and re-save | retail reads port saves | [SAVE_INTEROP_TEST.md](SAVE_INTEROP_TEST.md) §4 |
| S8 | A door transition frame by frame: the KEEPIN door into the Keep (`0_1_23`) | the fade's seven cover levels, the two-tick black hold, whether the cursor is covered | [forensics/SCREEN_SYSTEM.md](forensics/SCREEN_SYSTEM.md) §2.6 |
| S9 | Any door or teleport with music playing | whether the music dips during a screen fade | SCREEN_SYSTEM §2.6 |
| S10 | A click and a key press during the opening, before the first line | whether it fades out and restarts without the intro (PlayScreen `+0x5dc`) | SCREEN_SYSTEM §2.6 |
| S11 | Enter the Keep from outside through its gate (`keepin`, forest side) and capture the first second inside | how dark the arrival corner is (the port shows it black beside a lit hall) | [forensics/EXITS.md](forensics/EXITS.md) §7 |
| S12 | **Partly taken 2026-10-06:** "Locke entered The Keep" and the combat log over the Keep floor (RETAIL_CAPTURE §3, shots 05, 10, 11); still wanted: the 10 s wait. In the Keep, open a chest and pick something up so two or three messages show over a light floor; then wait 10 s. Note `NoTexOverlay` | text bar colour (the code says gold 255,200,0), shadow, position (x 4, baseline 9, 12 px lines from the map's bottom edge), how many lines stay | [../ui/forensics/TTextBar_SPEC.md](../ui/forensics/TTextBar_SPEC.md) §7, §8 |
| S13 | In a played game (control on), default ini (no `NOTEXOVERLAYS`): ESC and the next ⅓ s frame by frame; the menu; Save Game with a few slots; Load Game from the menu, then Load Game on a slot and the next second; Quit Module's question; title Load Game and Options | the in-game chrome's translucency and fade, the popup's look, the list's scrollbar and selection, the in-game load's progress popup | [forensics/INGAME_MENU.md](forensics/INGAME_MENU.md) §4.2, §5, §9 |
| S14 | In town (Misthaven), talk to Elahni (potions) with some gold, default ini: Buy Items and the panel as it opens; the mouse over the second row; a click on the first row; Buy (the gold and her line); the ↓ arrow; Exit and the next second | the shop's fonts, colours (names violet 130,13,197, hover 230,150,255, selected 200,83,255; gold and labels 255,186,0; stat lines grey), row positions, the icons; whether the bottom bar comes back after Exit | [../ui/forensics/BuySellScreen_SPEC.md](../ui/forensics/BuySellScreen_SPEC.md) §4, §8, §1 |
| S15 | Sell Items at Elahni with potions in the pack, in a bag and in a belt pouch; then at Cronus (armor) with a worn and an unworn piece; sell one of each | which items a Sell shop lists (bags, belt pouches, worn armor), the sell prices (Value × 0.3) | BuySellScreen_SPEC §6.3, §6.4 |
| S16 | One spot in the Keep hall, Locke standing still, `RealTimeLight=No`, fullscreen: one shot each at Gamma 0, 2 and 4 (set in Options, OK, then wait for the area ambient to settle — or restart between shots, since each OK adds the offset again) | Partly settled 2026-10-07 from the opening captures: under dosbox-x the ramp has no effect (the HUD doesn't change), and the ambient offset is in effect (S1 renders at ambient 14 at level 3). Still open: the GOG wrapper (dgVoodoo) and levels 0, 2, 4 | [forensics/OPTIONS.md](forensics/OPTIONS.md) §7.11, [../LIGHTING_FIDELITY.md](../LIGHTING_FIDELITY.md) §8 |
| S17 | In a played game, ESC → Load Game → a slot on another level → Load Game, frame by frame until control returns; then the same for a slot on the same level | the popup's look, the bar's colour and steps (80 after the save is read, then the sector fill, closing at 80%), what stands behind it, whether the world moves while the popup fades in and out, whether the dialog vanishes at once | [forensics/INGAME_MENU.md](forensics/INGAME_MENU.md) §5.1, §9 |

## Dialog ([forensics/DIALOG.md](forensics/DIALOG.md) §7)

1. Was the floating speech-box design (NPC lines at the top, Locke's
   lines and the choices at the bottom, portrait in a ring) the shipped
   UI, with `Dialog.dat` a leftover of the 1998 bottom panel?
2. `ShowDialog`: retail always puts the text in the boxes; the option only
   blanks a copy nothing draws. Meant to hide subtitles when a voice
   plays? Should Revisited honour it?
3. `NOWAIT player.say` still waits in retail (10 shipped uses). Intended,
   or should those lines overlap the next one?
4. `[me]`/`[chr]` substitution is dead in retail (an inverted byte test).
   The port keeps it working; no shipped line uses it. Keep it?
5. `busysay`/`busymsg`: multiplayer "this NPC is busy with another
   player" lines? Their `&=` on the script's flags: a typo for `|=`?
6. `hideresponse`: a 1998 command kept after dialog moved off the bottom
   drawer? (No shipped script uses it.)
7. `revenant.ini [Controls] DialogSkip=A,B,C,JOY1,JOY2,JOY3` — the exe
   never reads it (Space and joystick button 2 are hard-coded). From
   another build or the launcher?
8. Eight choices are stored but the keys reach six. Were eight ever used?
9. Locke's lines and the choices: retail's code reads their colour as
   light azure (60,175,255), which font.def's "Dialog" colour agrees
   with -- not the gold an earlier spec assumed. Is azure what shipped?
10. While choosing, the hovered choice stays lit after the pointer leaves
    the list (and arrows + Enter pick). Intended?
11. Is the Ring in the texture-format `StatusBar.dat` the same art as the
    `StatusBarNoTex.dat` one the port draws?
12. Times New Roman is drawn with Tinos (metric-compatible, OFL). Fine?
13. `Sardok.say` appears 16 times in the module scripts (e.g. Tendrick's
   throne-room dialog, `I4SAR04`/`I4SAR05`), but no object is named
   exactly "Sardok" near them (they are `SardokT`, `SardokR`, …) and
   retail's lookup is exact. Do those lines play in retail?

## Commands and scripts ([forensics/COMMAND_SYSTEM.md](forensics/COMMAND_SYSTEM.md) §5–6)

14. When a trigger starts, retail turns incidentals off for the script's
    character and the triggering player, and back on only if the trigger
    finishes in the same pass — so a trigger that waits leaves them off.
    Intended?
15. A level-up stat point lands on the Attack skill one time in seven
    (`random(0,6)` over seven ids). Intended?
16. ~~Music plays at full CD volume until the player confirms the Options
    screen, ignoring the INI `MusicVolume`. Intended?~~ Settled from the
    code ([forensics/OPTIONS.md](forensics/OPTIONS.md) §7.9): the CD keeps
    the OS mixer's own level until the Options pane opens (opening it
    applies `MusicVolume`, not only OK); that mixer level outlived the
    process, so the player's last setting normally carried over.
17. `timelimit`'s usage text is the `script edit/pause/resume/end` help —
    a leftover, or is it the script-control command?
72. `endfighting` starts a fight instead of ending one: it calls
    BeginFighting with no target (`0x00427d30`), so the character squares
    up to the closest enemy. No shipped script uses it. Was it meant to end
    combat (as `combat off` does)?

## Saves ([SAVE_INTEROP_TEST.md](SAVE_INTEROP_TEST.md), [forensics/SAVE_GAME.md](forensics/SAVE_GAME.md))

18. What clears the player's AI flag (`0x20`) in retail? (Retail saves
    have it clear; no interop effect.)
19. Retail stores the HUD state in the player record but never reads it
    back; the port restores the sidebar from it (a marked divergence).
    Keep, or match retail?
20. What is the HUD word `DAT_0065d19c`?
21. `newgame.sav` stores Locke invisible (the opening clears it). What
    does retail do loading `Port New Game`, saved at that moment?

## HUD ([../ui/HUD_LIVE_BINDING.md](../ui/HUD_LIVE_BINDING.md) §7)

22. Does the retail paperdoll animate or hold one pose? Which animation?
23. With no portrait icon, does retail show an empty frame?
24. Does a new game start with 0 of 105 mana? (`newgame.sav` stores 0;
    retail's `New Game1` has 7 of 105.)
25. *(Answered by the decompile: the kill-experience level-up sets
    NextExp; `playerlevel` leaves it.)*
26. Do two-word quick-spell names split at the first space, and does
    dragging one ring onto another swap them?
27. Does "Advanced healing" show its own icon or Heal's?

## Exits ([forensics/EXITS.md](forensics/EXITS.md) §10)

28. Door1, Door2, PortEW and PortNS are excluded from walk-over
    activation by name, so their `master.s` USE scripts own them. Was
    leaving `InDoor1`, `InportEW` and `InportNS` off that list deliberate?
29. `setfromexit` does `exitflags &= 4`, which changes nothing on a door
    or teleport stone; its 28 uses mark the destination stone. Meant to
    be `|= 4`, with the player's on-exit flag doing the real work?
30. Press plates (`PressPlate`) never animate down: their 1998 `Activate`
    ended up in a vtable slot nothing calls. Known at the time?
31. A key or lockpick attempt on a locked door, even a failed one, lets
    the door's USE script run: it swings open, but the teleport refuses.
    Seen in play?
32. What does the `TileFlags` stat (EXIT, TILE, CONTAINER, INVCONTAINER)
    control?
33. A lever whose `closing` animation finishes goes to state 0
    (`openingout`), where other exits go to `closed`. Levers springing
    back, or a slip?
34. `exit.def`'s `mapindex` and ambient fields are written by the editor
    but never read. Were they ever applied on arrival?
35. Shot wanted (dosbox-x): after Rahul dies, click the resurrection
    chamber door (`ressexit`) and capture the transition to the hall —
    does "Loading Map... Please Wait" appear in the text bar?

## Player stats ([../gameplay/forensics/PLAYER_STATS.md](../gameplay/forensics/PLAYER_STATS.md) §11)

36. rules.def's comment names the STATREQS columns "... luck, mind", but
    the shipped code gives the fifth value to Mind and the sixth to Luck.
    Which was intended?
37. ClearPlayer divides the free attribute budget by six (each free
    attribute gets 6–7 points; a character totals 63, not 84). Intended?
38. Adding a stat effect removes every other active one, so only one
    buff is ever in force. Intended?
39. armor.def's `FAT %15`, `LEV n` and the trailing `%` in `Hands 15%`
    never worked as written, and `Fatigue %4` scales current fatigue.
    What were they meant to do?

## Screens and fades ([forensics/SCREEN_SYSTEM.md](forensics/SCREEN_SYSTEM.md) §5)

40. Death screen Restart switches back to PlayScreen with the start name
    already cleared, so after a loaded game it falls through to a new
    game. Intended, or did Restart reload the last save?
41. Event code `0x103` and the pre-initialized screen `DAT_0065bb14`:
    what were they for?
42. In single player, a block that fades the screen out and ends without
    fading in leaves it black (retail's `End` fades back in only for a
    multiplayer host). Intended? (All 79 shipped pairs are balanced.)
43. Was PlayScreen's restart flow (`+0x5dc`: stop the player's script,
    fade out, load `newgame`, fade in) the "skip the opening" path?
44. Was the music meant to dip with screen fades?

## Walking to and facing objects ([forensics/COMMAND_SYSTEM.md](forensics/COMMAND_SYSTEM.md) §6.5)

45. The door prototypes check `IF USER.ISATRELATIVEDISTANCE THIS … = 1`
    after walking Locke to the door, and only then turn him and play the
    opening animation. When did that walk fail to arrive in the shipped
    game: another character in the way, a door blocking its own spot?
    And was skipping the turn and the animation (the fade and teleport
    still happen) the intended fallback?
46. `gotorelativedistance`'s spot uses `sin((facing + angle + 0x7f) · k)`
    and `cos((facing + angle) · k)` with `k` = 2π/255, so the spot sits
    a little off the line straight behind the object. Was `0x7f` meant to
    be a half turn (`0x80`) and 255 meant to be 256? The port keeps the
    shipped numbers.
47. In combat mode a script `goto` loses its target on the first step
    (the combat-mode move clears it unless an item pick-up is pending),
    so the character keeps walking in its first direction. Is that why
    every door prototype turns `COMBAT OFF` before walking? Did any
    shipped scene walk a fighting character?
48. `faceobject` and the `gotorelative…` commands look their object up
    from the script's owner (`THIS` is the door), while `pivotobject`
    looks it up from the character that turns (`player.pivotobject
    this` names Locke). Was the difference deliberate?
49. Shot wanted (dosbox-x): open the Keep's `ressexit` door from inside
    the resurrection chamber, one frame when Locke stops beside the door
    and one after he turns to it. Settles where the side spot is and
    which way he faces. The port aims him at (1200, 1098); he stops,
    blocked, at (1178, 1065) and turns to facing 87.

## Text bar ([../ui/forensics/TTextBar_SPEC.md](../ui/forensics/TTextBar_SPEC.md) §14)

50. The three newest messages never fade (`DAT_005e5804` = 3); only older
    ones age out, five seconds after they move up past the third. Both
    sample screenshots show three lines left at the bottom of the map. Was
    a standing "last three messages" log the intent?
51. Ordinary messages are drawn gold (255,200,0) in the code, but the sample
    screenshots show them pale peach with magenta fringes. Which did the
    shipped game show? (Shot S12.)
52. The `texthealthbar` strip is only the map-loading progress bar in
    retail; the 1998 code showed the combat target's name and health in the
    text bar. Was the combat readout dropped on purpose when the status bar
    got its target side?
53. `Print` splits a message at `'\n'`, but after a newline it adds the
    message's first piece again instead of the last (`0x0054d2ba`). Did any
    shipped message contain a newline? The port adds the last piece.
54. A message that starts with a space is dropped. Intended?
55. Enter opens a "Message: " prompt in single player too: `@` lines run a
    script line on Locke and words like `alchemy` or `abracadabra` toggle
    cheats. Was that meant to ship, and should the port keep it? (Also,
    hiding the bar runs whatever was half-typed.) Retail 2026-10-06: it
    works in a single-player New Game, "Locke: alreadydead" then "Cheat
    Enabled" (RETAIL_CAPTURE §2).
56. Line types 2 (violet) and 4 (pink) have colours but no callers. What
    were they for?

## In-game menu, load, save, options ([forensics/INGAME_MENU.md](forensics/INGAME_MENU.md))

60. The menu's Save Game does nothing while the player has no control
    (a conversation, a cutscene) — no message, the click is ignored.
    Was that how "saving isn't allowed during a conversation" was meant
    to read, or was a message planned?
61. The Load Game and Save Game controls default to Ctrl + left Windows
    key and Ctrl + Menu (Apps) key (`CTRL-LWIN`, `CTRL-APPS`; Quick Save
    Ctrl + Backspace, Game Options `O`). Were the Windows keys intended?
    (On the port's keyboard the same codes are Ctrl+`[` and Ctrl+`]`.)
62. The load and save dialogs open with the last slot of the list
    selected (alphabetical on NTFS), not the most recent save, and the
    save dialog then offers that slot's name. Intended?
63. A slot without a picture kept the previously selected slot's
    picture in the dialogs (the bitmap was only overwritten when a
    picture loaded); the port shows black. Was a "No Picture" text meant
    to cover it (`loadgame.def` has one commented out)?
64. The "Character:" field always reads "Locke" (a literal), whatever
    the save. Was it to show the player's name for multiplayer saves?
65. Demo mode (module flag, `SetDemoMode`) makes ESC ask "exit the game?"
    instead of opening the menu. Which builds or modules ran in demo
    mode — the attract loop, a trade-show demo?
110. The in-game load's bar never fills: the sector load maps onto 0–800
     of 1000 and the popup closes at 800 (`0x00539990`). Was 800 meant to
     leave room for a step after the sectors, or is it a leftover?
111. The load's progress popup is pushed without the dialog's pause
     (`SetExclusivePane` 7, not `RunModal`), so the old world runs for the
     few frames the popup fades in, and the new one while it fades out.
     Intended, or should the world have held?

## Options ([forensics/OPTIONS.md](forensics/OPTIONS.md))

90. Cancel restores only the gamma. A music level dragged on the slider
    keeps playing after Cancel, while the saved `MusicVolume` stays the old
    one, until Options is opened again. Was Cancel meant to put the music
    back too?
91. The Violence slider stops at 4, but the exe's default is 5 (the
    manual's "Level 5 is satisfyingly bloody" reads as the slider's top).
    Opening Options and pressing OK turns a default 5 into 4, which allows
    fewer and smaller blood particles. Was 5 meant to be the slider's top
    (a 0..5 range), or the default meant to be 4?
92. Gamma does two things: a display gamma ramp, and an offset of
    (level − 2) × 10 on every area's ambient light (`SetAmbientLight`). All
    five ramps darken the midtones (level 4 only a little, level 2 like a
    1.8 power curve), and the shipped ini says `GammaLevel = 4`, so the
    shipped look was the level-4 ramp plus 20 ambient. Was that the intended
    picture, and did the ramp take effect on the hardware of the time?
    *2026-10-07:* the port now renders the ambient offset, as retail. It
    renders no ramp: in the dosbox-x captures the ramp never takes effect
    (the HUD is pixel-identical before and after the Options pane), and
    the Classic look is matched to those captures (LIGHTING_FIDELITY.md
    §8). If the ramp was part of the intended picture on real cards, it
    could come back as a Revisited option.
93. Each Options OK re-applies the gamma offset to the current ambient,
    which already includes it, so pressing OK repeatedly at level 3 or 4
    brightens the area until its ambient is next set. A bug? *2026-10-07:*
    seen in the shipped game: in the opening capture session the pane
    opened and closed through OK several times, and the Keep got brighter
    each time while the HUD stayed the same (`opening-20261006-run2.avi`,
    about 8:56, 10:03, 10:28, 11:58; OPTIONS.md §7.11). The port keeps it.

## Camera and control ([forensics/COMMAND_SYSTEM.md](forensics/COMMAND_SYSTEM.md) §4)

70. `scrollto <x> <y> <z>` jumps rather than scrolls: SetCameraPos
    (`0x00453940`) masks the scroll bit `centeron`/`scrollto` pass, so
    only `scrollto <object>` scrolls. Intended? (The opening's
    `scrollto 1207 667` gives two coordinates where the parser wants
    three, `%i %i %i` at `0x005cbc84`, so that line does nothing at all.)
71. A block that ends with the camera off the player snaps it back to him
    (`TScript::End`), and one that ends with control off turns it back
    on. Were scripts written to rely on that, or is it a safety net?

## Shops ([../ui/forensics/BuySellScreen_SPEC.md](../ui/forensics/BuySellScreen_SPEC.md), [forensics/COMMAND_SYSTEM.md](forensics/COMMAND_SYSTEM.md) §6.6)

80. Selling pays the type's Value × 0.3 (truncated) for the whole item, so
    a stack (a quiver of arrows, anything with Amount > 1) sells for one
    unit's price. Intended, or was the amount meant to count?
81. Retail's drawer close (`0x0047ecc0` / `0x0047ece0`) closes whatever the
    bottom drawer holds. `LoadGame` calls it, so an in-game load would
    close the HUD's bottom bar (belt, quick spells) until the player opens
    it again, and so would `hideresponse`. Was that seen in play? The port
    closes only the shop there for now.
82. After a shop's Exit the drawer stays closed: the bottom bar does not
    come back until Lower Panel (B). Intended, or was the bar meant to
    return with the shop's close?
83. A unique type (class stat SaleType 1) is in no shop until the player
    sells one; from then on every shop that lists it stocks it, and buying
    it back doesn't take it out of the merchant table. Was that the
    design (sell your finds, buy them back), or should a purchase have
    removed the entry?
84. A misc shop filled with `buyselladdcriteria` walks the armor class
    twice, so matching armor is listed twice (`0x00530af0`). No shipped
    script does that. Was the second meant to be another class?
85. `buysellremovecriteria` finds a row's type by its display name; a
    localized name (from a dialog tag) would never match, so the row
    would stay. Known? (English: display names equal type names.)
86. Armor without an ARMOR.DEF entry labels its fifth stat with BSARM4
    again ("Min Strn" twice; ARMOR.DEF armor uses BSARM5 "Min Cons").
    A typo?
87. The shop is not modal: it is the play screen's bottom drawer and the
    world keeps running (the shop scripts turn control off). Was pausing
    ever considered?
88. Buying with a full pack does nothing and says nothing. Was a line
    planned (there is a no-gold line but no no-room line)?
89. Hruthford stocks the Bracelet of Fortune and the Ivory Pendant for
    0 gp: their class.def `Value` is 0 (most jewelry is, and is also
    SaleType 1). Were jewelry prices still to be filled in, or meant to
    come from ARMOR.DEF's BASICMODS value column (which the shop never
    reads)?
123. The first shop visit of a game that goes straight to Sell lists
     nothing: `BuySellAddbuyitem` lists the customer's items, the customer
     is only set by `buysellscreen` (after the add lines), and the shop's
     first `buysellinit` clears it. Choosing Buy first, or any later
     visit, works. Did players see an empty Sell list the first time?

## Inventory ([forensics/INVENTORY.md](forensics/INVENTORY.md))

120. Gold, food and potions merge into a pile of their kind when they join
     an inventory, but ammo never does: TAmmo's merge hook (`0x004bf680`)
     only counts its inventory icon. So arrows bought or picked up stay
     separate piles. Intended (quivers kept apart?), or an unfinished
     merge?
121. Giving part of a pile to someone (`0x0046fc40`: `give`, `take`, a
     script's partial gift) makes the recipient's new pile from the
     **giver's** name, not the item's. Where a type has that name (Locke's
     own, or a chest's) the recipient gets a new object of that type
     instead of the gold. Where none has (a giver with its own instance
     name), the add fails, the amount already taken off the pile is lost,
     and `GiveInventoryTo` calls again on the same pile until it is small
     enough to move whole. Was that ever seen? No shipped
     script gives part of a pile, so it may never have shown. The port
     uses the item's type.
122. The opening adds Life, Moon and Soul before the Spell Pouch
     (`keep.s` 616–619), and AddToInventory only routes a player's new item
     into an item named exactly "Pouch", so the talismans land loose in
     Locke's pack, beside the pouch. Retail's `New Game1` has them inside
     the Spell Pouch. Did players drag them in, or did something else put
     them there?

## NPC conversations ([STORY_TESTING.md](STORY_TESTING.md) §7)

100. Rubold's idle walk (town.s:2653, his ALWAYS block) starts with
     `gotorelativeposition playerway 0 0`, but no level of the module has
     an object named `playerway` (his other three waypoints are in his
     house, 1_3_19). Retail answers "Can't find any object by that name"
     and the walk goes on from the second leg. Was a waypoint lost from the
     map, and where did it stand?
101. The Ogrok gatekeeper's block turns Locke with `player.PIVOTOBJECT
     GATEKEEPER` four times (forest.s:2046, 2057, 2096, 2125); the object is
     `Gatekeeper1` and nothing answers to `Gatekeeper`, so Locke never turns
     to face him. Meant to be `GATEKEEPER1`?
102. forest.s has two DIALOG blocks with no object: `Shari1` (no object of
     that name on any level; `SHARIL1`, which it turns Locke to, doesn't
     exist either) and a second `Pepper1` (the object stands in a house on
     level 1, where town.s's own `Pepper1` block answers). Leftovers from
     when those townsfolk stood outside?
103. Kylie's conversation turns the music down with `SETCDVOLUME HALF` at
     its start (town.s:2677) and again at its end (2778), where every other
     block sets it back to `FULL`; after talking to her the music stays at
     half volume until the next conversation. A typo for `FULL`?

## The first fight ([../gameplay/forensics/PLAYER_INPUT.md](../gameplay/forensics/PLAYER_INPUT.md))

130. In your session of 2026-10-05 the Short Sword drag logged `[drag]
     promote inv slot=5` and then neither `DROP` nor `CANCEL`. Every
     button release the game sees during a drag ends in one of those, and
     headless the sword now equips when dropped on the hand slot or
     anywhere on the paper doll. Where did you let go: on the doll, on the
     hand slot, outside the window, or after switching to another app (the
     port ignores mouse events while it isn't the active app)? If it
     happens again, the log lines after `promote` will say.
131. Retail's Rahul kills an unarmed Locke in two or three minutes when
     the player does nothing (dosbox-x capture), and the Short Sword starts
     in the pack, not the hand. Was the first fight meant to be hard to win
     without equipping the sword, as the lesson that items have to be
     equipped?

## The resurrection vortex ([../vfx/forensics/X23_GVORTEX_TEffect.md](../vfx/forensics/X23_GVORTEX_TEffect.md))

140. How bright should the resurrection vortex be? Its tag draws every
     object `litadd`: ONE/ONE blending, the texture modulated by the
     object's lighting. Its materials have full emission, so on a Direct3D
     card the texture adds at full strength. The port does that: a
     saturated green column whose base goes yellow-white, with Locke barely
     visible inside it. Retail's software renderer lights 3D vertices from
     the scene's lights alone and never reads the material (`0x0056eb30`),
     so the lab footage (dosbox-x, Software3D) shows a dim, translucent
     column, about a fifth as bright, with Locke clearly floating up inside
     it ([comparison](../vfx/captures/X23_gvortex_port_vs_lab.png)). Which
     look did you intend: the hardware one, or the softer one the software
     renderer gave?
141. Locke's resurrect animation has sound tags for `loc1resscream`
     (frame 33) and `loc1resbreath` (frame 169), but no such samples ship
     in any archive, so retail plays nothing there (the footage has
     neither; its footsteps at frames 163-199 do play). Were they recorded
     and cut? The port stays silent, as retail.
