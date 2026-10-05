# Questions for the author

Things the port can't settle from the code: how the shipped game behaved,
what was intended, and retail screenshots to compare against. Each says
where it came from; the port keeps retail's behaviour until it's answered.
Answer inline (or in chat) and the owning doc gets updated.

## Retail screenshots wanted (dosbox-x)

| # | Shot | Settles | From |
|---|---|---|---|
| S1 | Opening, Locke in the pit before he moves, `RealTimeLight=No`, `EnhancedLighting=No`, 640×480 | Classic lighting 1:1 | [LIGHTING_FIDELITY.md](../LIGHTING_FIDELITY.md) |
| S2 | Same, `RealTimeLight=No`, `EnhancedLighting=Yes` | character overbright and key light | LIGHTING_FIDELITY |
| S3 | Same, `RealTimeLight=Yes`, `EnhancedLighting=Yes` (the shipped default) | the triangle-grid look players remember | LIGHTING_FIDELITY |
| S4 | S2 settings, Locke walked from the pit toward the wall torch, one shot per character width | how lights fall off on characters | LIGHTING_FIDELITY |
| S5 | Misthaven by day, `RealTimeLight=No`, Locke facing each screen diagonal | which side the character key light hits | LIGHTING_FIDELITY |
| S6 | Note the retail ini's `NoTexOverlay`. Then: Locke's "Where am I?" (`I1LOC00`); Sardok's "Welcome back from the dead, Revenant." (`I1SAR00`); the three choices with the side panel open and closed (V); the mouse over "Who am I?" then off the list; a click on choice 3 and the next ½ s; Tendrick's first line; while choosing, Up Up Down Enter | box colours (the port reads Locke/choices as azure 60,175,255, Sardok green, Tendrick yellow), positions, Ring portrait, typeface and shadow, hover/click behaviour and sounds, slide on pick | [forensics/DIALOG.md](forensics/DIALOG.md) §4.2, §4.4 |
| S7 | Load the port-written save slots (`Port Resave`, `Port Played`, `Port New Game`) and re-save | retail reads port saves | [SAVE_INTEROP_TEST.md](SAVE_INTEROP_TEST.md) §4 |
| S8 | A door transition frame by frame: the KEEPIN door into the Keep (`0_1_23`) | the fade's seven cover levels, the two-tick black hold, whether the cursor is covered | [forensics/SCREEN_SYSTEM.md](forensics/SCREEN_SYSTEM.md) §2.6 |
| S9 | Any door or teleport with music playing | whether the music dips during a screen fade | SCREEN_SYSTEM §2.6 |
| S10 | A click and a key press during the opening, before the first line | whether it fades out and restarts without the intro (PlayScreen `+0x5dc`) | SCREEN_SYSTEM §2.6 |
| S11 | Enter the Keep from outside through its gate (`keepin`, forest side) and capture the first second inside | how dark the arrival corner is (the port shows it black beside a lit hall) | [forensics/EXITS.md](forensics/EXITS.md) §7 |
| S12 | In the Keep, open a chest and pick something up so two or three messages show over a light floor; then wait 10 s. Note `NoTexOverlay` | text bar colour (the code says gold 255,200,0), shadow, position (x 4, baseline 9, 12 px lines from the map's bottom edge), how many lines stay | [../ui/forensics/TTextBar_SPEC.md](../ui/forensics/TTextBar_SPEC.md) §7, §8 |
| S13 | In a played game (control on), default ini (no `NOTEXOVERLAYS`): ESC and the next ⅓ s frame by frame; the menu; Save Game with a few slots; Load Game from the menu, then Load Game on a slot and the next second; Quit Module's question; title Load Game and Options | the in-game chrome's translucency and fade, the popup's look, the list's scrollbar and selection, the in-game load's progress popup | [forensics/INGAME_MENU.md](forensics/INGAME_MENU.md) §4.2, §5, §9 |
| S14 | In town (Misthaven), talk to Elahni (potions) with some gold, default ini: Buy Items and the panel as it opens; the mouse over the second row; a click on the first row; Buy (the gold and her line); the ↓ arrow; Exit and the next second | the shop's fonts, colours (names violet 130,13,197, hover 230,150,255, selected 200,83,255; gold and labels 255,186,0; stat lines grey), row positions, the icons; whether the bottom bar comes back after Exit | [../ui/forensics/BuySellScreen_SPEC.md](../ui/forensics/BuySellScreen_SPEC.md) §4, §8, §1 |
| S15 | Sell Items at Elahni with potions in the pack, in a bag and in a belt pouch; then at Cronus (armor) with a worn and an unworn piece; sell one of each | which items a Sell shop lists (bags, belt pouches, worn armor), the sell prices (Value × 0.3) | BuySellScreen_SPEC §6.3, §6.4 |
| S16 | One spot in the Keep hall, Locke standing still, `RealTimeLight=No`, fullscreen: one shot each at Gamma 0, 2 and 4 (set in Options, OK, then wait for the area ambient to settle — or restart between shots, since each OK adds the offset again) | whether the gamma ramp is in effect under dosbox-x / the GOG wrapper, and how much the ambient offset brightens the floor | [forensics/OPTIONS.md](forensics/OPTIONS.md) §7.11 |

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
    hiding the bar runs whatever was half-typed.)
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
    picture, and did the ramp take effect on the hardware of the time? The
    port renders neither yet (shot S16).
93. Each Options OK re-applies the gamma offset to the current ambient,
    which already includes it, so pressing OK repeatedly at level 3 or 4
    brightens the area until its ambient is next set. A bug?

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
