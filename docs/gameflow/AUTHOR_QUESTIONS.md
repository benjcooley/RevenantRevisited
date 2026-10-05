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
| S6 | The opening's first spoken line, Sardok's reply, and the first choice list | dialog box layout and colours (byte order of the speaker colours) | [forensics/DIALOG.md](forensics/DIALOG.md) §4.2 |
| S7 | Load the port-written save slots (`Port Resave`, `Port Played`, `Port New Game`) and re-save | retail reads port saves | [SAVE_INTEROP_TEST.md](SAVE_INTEROP_TEST.md) §4 |

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
9. `Sardok.say` appears 16 times in the module scripts (e.g. Tendrick's
   throne-room dialog, `I4SAR04`/`I4SAR05`), but no object is named
   exactly "Sardok" near them (they are `SardokT`, `SardokR`, …) and
   retail's lookup is exact. Do those lines play in retail?

## Commands and scripts ([forensics/COMMAND_SYSTEM.md](forensics/COMMAND_SYSTEM.md) §5–6)

10. When a trigger starts, retail turns incidentals off for the script's
    character and the triggering player, and back on only if the trigger
    finishes in the same pass — so a trigger that waits leaves them off.
    Intended?
11. A level-up stat point lands on the Attack skill one time in seven
    (`random(0,6)` over seven ids). Intended?
12. Music plays at full CD volume until the player confirms the Options
    screen, ignoring the INI `MusicVolume`. Intended?
13. `timelimit`'s usage text is the `script edit/pause/resume/end` help —
    a leftover, or is it the script-control command?

## Saves ([SAVE_INTEROP_TEST.md](SAVE_INTEROP_TEST.md), [forensics/SAVE_GAME.md](forensics/SAVE_GAME.md))

14. What clears the player's AI flag (`0x20`) in retail? (Retail saves
    have it clear; no interop effect.)
15. Retail stores the HUD state in the player record but never reads it
    back; the port restores the sidebar from it (a marked divergence).
    Keep, or match retail?
16. What is the HUD word `DAT_0065d19c`?
17. `newgame.sav` stores Locke invisible (the opening clears it). What
    does retail do loading `Port New Game`, saved at that moment?

## HUD ([../ui/HUD_LIVE_BINDING.md](../ui/HUD_LIVE_BINDING.md) §7)

18. Does the retail paperdoll animate or hold one pose? Which animation?
19. With no portrait icon, does retail show an empty frame?
20. Does a new game start with 0 of 26 mana?
21. Does a level-up rewrite NextExp?
22. Do two-word quick-spell names split at the first space, and does
    dragging one ring onto another swap them?
23. Does "Advanced healing" show its own icon or Heal's?
