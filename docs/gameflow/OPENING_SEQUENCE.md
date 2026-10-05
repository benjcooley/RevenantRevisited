# Opening sequence — acceptance test for scripting, commands and dialog

The first minutes of a new game exercise most of the script engine, the
command set and the dialog system. Getting them to play exactly as retail
is the acceptance test for gameflow step 3 (ARCHITECTURE.md §9). This
doc is the trace: what the opening needs, what the port has, what's next.

## 1. The scene (`Ahkuilon/keep.s`, `OBJECT "SardokR"`, lines 479–670)

Locke wakes in the Keep's resurrection chamber (newgame.sav puts him at
1212,545 on level 2). Entering the Keep area loads `keep.s`; the player
standing in Sardok's trigger cube (`CUBE 1145,475,0 1265,619,60`) starts
the block:

1. Control off, incidentals off; Locke faded out; camera centred on him;
   level 1 and attack level 0.
2. A vortex effect (`add gvortex`) rises; Locke faded back in and
   resurrected (`try resurrect`); music to half volume.
3. Voiced dialog: Locke and Sardok talk (`say I1LOC00`, `say I1SAR00`…),
   characters turning to face each other (`pivotobject`) and walking
   (`goto`).
4. Two rounds of dialog choices (`Choice <label> <text>`,
   `wait response`, `jump`) until the player picks the exit choice.
5. Sardok gives the spell pouch, talismans and a scroll (`addinv`, `get`);
   Tendrick gives clothes and a sword.
6. Rahul bursts in through the exit door (`RESSEXIT.STATE …`, fade in,
   `goto`), speaks, and sets `RAHULSTATE` for the next scene.

## 2. What it needs

| Need | Retail | Port (2026-10-05) |
|---|---|---|
| Area scripts attach to objects already in the world | `ParseScripts` `0x00496860` notifies the map (`N_SCRIPTADDED`); objects match by name (`ObjectScript` `0x00497370`) | retail |
| CUBE trigger fires when the player enters | trigger scan in `TScript::Continue` `0x004933d0` (`0x004927b0` per trigger) | retail |
| Line execution, blocks, labels, `jump`, `nowait` | `Continue`, `CommandInterpreter` `0x0041e8e0` | 1998 block engine; retail context syntax |
| Waits: frames, character done, speech done, dialog response, screen fade | `TScript` wait machine: `SetWait` `0x00492b00`, check `0x00492d70` | retail; the screen-fade wait passes at once |
| Conditions (`If Rahul.stat health = 0` in Tendrick's ALWAYS block) | evaluator `0x0041f230`, resolver `0x0041e690` | retail |
| Commands used | see §3 | 6 ported from retail, rest 1998 |
| Dialog: speech text + voice, choice list, response | `TDialogPane`, speech | 1998 `TDialogPane`, unwired |
| Presentation: fades, camera, control off | PlayScreen fade state, `centeron`/`scrollto`; character fade (`TCharacter` `+0x194`) | partial; character fades run but aren't drawn (COMMAND_SYSTEM.md §6.4) |

Where it stands (2026-10-05): the whole block runs, headless with key
presses for the choices -- trigger, resurrection, voiced lines paced by
their voices, both rounds of choices, the gifts, Rahul's entrance,
RAHULSTATE, control back. Not yet visible: the dialog boxes (presentation
in progress) and the character fades (renderer).

## 3. Commands in the block (order of first use)

`control`, `incidentals`*, `fadecharacterout`*, `centeron`,
`playerlevel`*, `stat`, `wait`, `add`, `move`, `toggle`,
`fadecharacterin`*, `try`, `setcdvolume`*, `say`, `scrollto`,
`pivotobject`*, `goto`, `choice`, `jump`, `addinv`, `get`, `play`,
`pivot`, `state`, `set`.

\* ported from retail (`forensics/COMMAND_SYSTEM.md` §6; `pivotobject`
in its own commit). The rest run their 1998 bodies and each needs
checking against its retail handler (`recon/discovered/commands/`).

## 4. Sources

- The 1998 source (`/Users/benjamincooley/projects/Revenant/`) is the
  readable baseline for structure and most command bodies.
- Retail Ghidra decomps give the delta to release. The script engine's
  delta is large: retail moved waits from `TCharacter` into `TScript`
  (typed waits, a choice list, per-script wait object), and the trigger
  and continue logic changed with it.
- Findings go to `forensics/SCRIPT_ENGINE.md` (engine) and
  `forensics/COMMAND_SYSTEM.md` (commands) as each piece is ported.

## 5. Order of work

1. ~~Script attachment for area-loaded scripts (whole loaded world).~~
2. ~~Trigger scan and `Continue` (retail), so the block starts.~~
3. Command layer foundation (ARCHITECTURE §6): ~~resolver,
   evaluator~~; context and arguments; then the block's commands in
   order, each checked against retail.
4. ~~The `TScript` wait machine.~~
5. ~~Dialog: speech text and voice, choices, response~~ (drawing in progress).
6. Presentation: fades, camera, control.

Verification: headless runs logging each executed line and command
result; filmstrips at the key beats (vortex, first dialog, choice list,
Rahul's entrance); an input script that picks the choices.
