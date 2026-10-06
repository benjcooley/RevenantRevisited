# Forensics: player input to action (commands, movement mode, attacks, equipping)

**Topic:** how a key or mouse press reaches the player's action state
machine in the shipped game: the control map, `TPlayScreen::Command`,
the per-tick `UpdateMove`, the attack button path, and the two HUD
gestures that equip an item. Written for the first fight (Rahul, after
the opening), which the port could not win.
**Status:** forensics complete for the paths below (2026-10-06); port in
`src/playscreen.cpp`, `src/runtimemode.cpp`, `src/character.*`,
`src/uidragstate.*`, `src/uisidebartest.cpp`.
**Evidence:** Ghidra decompiles and disassembly of the retail
`Revenant.exe` (addresses below), retail strings read from the binary,
the 1998 source (`legacy/playscreen.cpp`, `legacy/ctrlmap.cpp`,
`src/equip.cpp`), and headless runs from a save made right after the
opening (Rahul attacking, Locke unarmed).

## 1. Control map: held flags and the "changed" mask

`TControlMap::GetCommand(key, down, modes)` returns a key's down command
on a press and its up command on a release, and keeps two masks of the
held controls' `CMDFLAG_*` bits: the state (`cmdflagstate`, retail
`0x0065a9c4`) and a changed mask (`cmdflagchanged`, retail `0x0065a9c8`).

The changed mask is only ever ORed into, never cleared, in 1998 and in
retail alike. Retail's writers: `0x0047de30` (UpdateMove), `0x004cee70`
(Stop), `0x0054d2f0` / `0x0054d4a0` (text-bar input), code at
`0x00490ea8..0x00490ed0` (no function defined there); every one ORs.
So "changed" means "some held control changed at some point", not
"changed this tick". Besides key edges, GetCommand's first loop ORs in
the flag of every control whose mode is no longer active: entering
combat mode marks `CMDFLAG_SNEAK` (the Sneak control has no combat mode)
on the next key event.

## 2. UpdateMove (`0x0047de30`), every tick

Reads the state for: bow aim left/right (`0x20` / `0x2` while the bow is
drawn), the direction (bits 0–7, first set bit, 32 per step), Leap
(`0x400`, combat or bow root, `0x004d2be0`), Go (`0x004ced20`), the
combat block (`0x800` → `Block(10000)` `0x004d2e30`; released →
`StopBlock` `0x004d30f0`). With no direction held it Stops (`0x004cee70`)
when `changed` is non-zero and the player is doing a move action (2, 4,
0x1a) that isn't a goto. Nothing here touches the movement mode.

## 3. Command (`0x0047cf40`)

Held back while control is off (`DAT_00666924`). The cases this track
needs (retail numbering; the port keeps the 1998 enum, `playscreen.h`):

| Retail | Action | Port |
|---|---|---|
| 1 | combat mode: `BeginFighting(0, 3)` `0x004d3b90`, or `EndFighting` `0x004d3fd0` | `GAMECMD_COMBAT` |
| 2 | bow mode: `BeginFighting(0, 0x19)` / `EndFighting` | `GAMECMD_BOW` (not dispatched yet) |
| 3 | sneak toggle: `StartSneak` `0x004cf2e0`, or `SetWalkMode` when the root is "sneak" | 1998 table: Sneak is held like Run (`GAMECMD_MOVEDOWN/UP`) |
| 0x21–0x23 | `ButtonAttack(1..3)` `0x004d2480` | `GAMECMD_SWING/THRUST/CHOP` |
| 0x24–0x2f | `ButtonAttack(4..15)` | `GAMECMD_COMBO1..12` |
| 0x4a / 0x4b | run key down / up: `state & 0x100` → `SetRunMode` `0x004cf490`, else `SetWalkMode` `0x004cf000` | `GAMECMD_MOVEDOWN/UP` |
| 0x52–0x55 | options, load, save, quick save | as before |

The 1998 `Command` did the same for its table: `MOVEDOWN`/`MOVEUP` →
run, else sneak, else walk, from the state; `BLOCKDOWN/UP` and the
direction commands do nothing there ("handled every frame in UpdateMove").

## 4. The fault: the movement mode re-set every tick

The port had moved the movement-mode switch into `UpdateMove` and keyed
it on `changed & CMDFLAG_RUN` / `CMDFLAG_SNEAK`, reading the mask as a
per-tick edge. After the first key event in combat mode the sneak bit
stays set (§1), so every tick called `Player->SetWalkMode()`.

`SetWalkMode` returns at once in walk mode (`IsWalkMode`). In unarmed
combat the root is `"hand"`; `IsWalkMode` tested the 1998 name
`"comhand"` (retail `0x004cf000` tests `"hand"`, string `0x005e064c`),
so it built a new `"hand"` root with the interrupt flag and forced it,
every tick:

- Locke's state restarted every tick (a random incidental `NN:hand`
  variant at frame 0–1), seen in the probe as `st=192 fr=1`, `st=170
  fr=1`, `st=193 fr=1` on consecutive samples.
- An attack begun by a key press was replaced by the root on the next
  tick, long before its impact frame (`ResolveAttack` resolves the hit at
  `frame == attack->impacttime`), so no hit ever landed. `ButtonAttack`
  itself returned true.

Armed (root `"combat"`) the early return held and attacks worked; but the
sword never reached the hand (§6).

The author's log showed every press refused with "still in prior
attack". That gate (`0x004d139c`) refuses while the character is doing
an attack (action 7) whose `nextwait` its frame (short, `+0x5c`) hasn't
reached. In the attack record `nextwait` is `+0xc4`, after impacttime
`+0xbc` and chainexptime `+0xc0` (which `0x004d1120`'s chain test reads);
`recon/discovered/player_combat_notes.md` puts nextwait at `+0xbc`, one
field early. Headless reproductions with the old build gave
accepted-then-clobbered presses instead; the exact refusal wasn't
reproduced. With the fix the gate
refuses only a press inside the current attack's `nextwait` window, as
retail does.

**Fix (port):** run/sneak/walk switch on the run and sneak keys' own
commands in `TPlayScreen::Command`, as retail and 1998 do; `UpdateMove`
no longer touches the mode. The gameplay command switch moved from the
game mode's `HandleKey` into `TPlayScreen::Command` (retail's shape:
the key handler passes the control map's command to Command); block
stays in `UpdateMove` only (the old `HandleKey` also called `Block()` on
the key, a second, 1998-less path). `IsWalkMode` / `IsRunMode` test
retail's `"hand"` / `"handrun"` (`0x004c9790` reads `"handrun"`
`0x005e0610`, `"combatrun"` `0x005e0618`, `"bowrun"` `0x005e0608`).
`IsValidAttack`'s default-mode gate rejects a combat root named `"walk"`
(`0x004d12bf`: `Is(DAT_005e0318)`, `"walk"`), the former TODO.

## 5. Attack path (unchanged, for reference)

`ButtonAttack` `0x004d2480`: a live chain (`CA_CHAIN`, within
`chainexptime`, fewer than 3 hits) only counts the press; otherwise
`FindButtonAttack` → `IsValidAttack` `0x004d1120` → `DoAttack`
`0x004d2120` (an `ACTION_ATTACK` block, interrupt + noroot, forced).
Retail's `IsValidAttack` differs from the port's in order and detail
(it checks `HasActionAni` first; the character flag `+0x128 & 2`;
bow fatigue; the damage formula through vtable `+0x2cc/+0x2d4/+0x2d0/
+0x224/+0x2b0`); see §8.

## 6. Equipping from the HUD

Both retail gestures put an item of the pack or the belt into **its own**
equipment slot, the item's `eqslot` stat, never the cell under the
cursor:

- **Drop on the equip pane** (`TEquipPane::MouseClick` `0x005363e0`,
  button up 4): with nothing held from the pane itself, a held pack item
  (`DAT_0065d67c`) or belt item (`0x10b + DAT_0065b090`) let go anywhere
  inside the pane (`0 <= x < w`, `0 <= y < h`) → `eqslot`; `-1` →
  `EQUIPUNABLE` on the text bar (`TDialogList::GetLine` `0x0049d800`,
  `TTextBar::Print` `0x0054d170`); `< 11` and `CanEquip` `0x00519300` →
  `Equip` `0x005199b0`, then the item's class sound `0x00473a10`, and a
  light source (class 6) re-sets the walk root (`SetWalkMode`). An item
  held from the pane is just let go.
- **Right button up on a pack item** (`InventoryPane::MouseClick`
  `0x00538210`, button 5, nothing held): a talisman of the player's goes
  into his "Spell Pouch" (pouch sound); any other item as above, or
  `EQUIPUNABLE`. In "stats mode" (`DAT_0065c9e0`) the right click shows
  the item's info instead (`0x005496a0`).

Button-down on a pack cell records the grab (button 1); button-up on a
cell moves/swaps/uses (`+0xbc` / `+0xb8` on the target item).

The port accepted only a drop on the exact matching cell, so a drop on
the paper doll (the natural target) cancelled; its right click was a
test stand-in that toggled an unused field. A drop on the exact hand
slot always worked and ran `TPlayer::Equip` (headless: `[drag] DROP inv
slot=5 -> equip slot=4`, then `IsValidAttack` sees weapon type 2).

**Fix (port):** `UIDragState::EquipInOwnSlot` (the shared "equip it");
a drop anywhere on the equip pane commits through it; a right button up
on a pack item calls it. Not ported: the item class sound, the torch
root refresh (`GetTorchRoot`, TPlayer vtable `+0x30c`, isn't in the
port), the talisman → Spell Pouch branch, the stats-mode info.

Not reproduced: the author's sword drag that logged `[drag] promote` and
then neither DROP nor CANCEL. Every left button up during a drag reaches
the HUD's drop handler and logs one of them; a release the app never
saw (outside the window, or while it was inactive: `revmain.cpp` drops
mouse events when `!AppActive`) would leave the drag latched
(AUTHOR_QUESTIONS 130).

## 7. Verification rig

Headless from the post-opening save (`--quickstart="New Game"`), input
scripts: `V` opens the bottom bar and side tabs, the Equip tab at
(605,280) opens equip + pack; pack cell (col, row) at
`(460 + 45 col, 348 + 44 row)`, slot `3 col + row` (Short Sword slot 5,
boots 6, pants 7, shirt 8); the paper doll body at about (545,175).
Drag: `move; left_down; moveto X Y 300; left_up`. Right click:
`move; right_down; wait 80; right_up`. Attacks `a`/`s`/`d` in combat
mode (`Enter` toggles it; Locke starts the fight already in it).

Result (2026-10-06): sword dropped on the doll → hand; boots, pants,
shirt by right click; 23 attack presses started, 3 refused for range;
Rahul 80 → 0 in 22 s (the sword, a press every 0.7 s), `combat to
dead`; TendrickR's scene to `ressexit.stat locked=0`. Retail reference
(dosbox-x capture of a New Game, kept by the coordinator): the same
scene lines in the same order, about 70 s from "I wonder…" to "Return
here…" (port 67 s).

## 8. Gaps (combat track)

- Hit resolution `0x004c62b0` isn't the port's `ResolveHit`: retail
  prints combat results (`Hit vs. Locke Dmg:4 Def:10 Off:12`, `Glancing
  Blow`, `Critical strike`; option NoCombatResults) and has glancing and
  critical outcomes the port lacks.
- Attack fatigue: retail spends it (Rahul's fatigue falls to 18 in the
  retail capture); the port's `ResolveAttack` has the subtraction
  commented out, so Rahul's stays at 104.
- Rahul barely hurts an idle Locke: 92 → 68 health in 200 s of no input
  (about 0.12 a second). In retail (dosbox-x capture, AutoCombat=Yes, no
  input) Locke goes 98 → 0 in about 140 s (0.7 a second): Rahul keeps
  closing in, Locke is knocked across the room (pit, wall emblem, the
  bookshelf corridor), and the text bar logs `Glancing Blow` (1–2),
  `Light Hit` (3), `Hit` (4–5) and `Critical strike` (6) against Def 10 /
  Off 12. Retail's Rahul shows 86–88 health and spends fatigue (35–48);
  the port's shows 80 and 104 throughout. A probe on his hits
  (2026-10-06) showed nearly every attack chosen from out of hit range
  and off-angle: distance 44–72 against `hitminrange..hitmaxrange` 0..40
  (spinkick, roundhouse), facing off by 50 against `hitangle` 32. The
  attack is picked by `mindist..maxdist`, wider than the hit range, and
  he chains attacks without closing in or turning. The monster AI
  (`0x004c8b60`, per-monster `0x004c9b70`) and hit resolution
  (`0x004c62b0`) need the retail sync; his damage per hit is 0–2 where
  retail's log shows 1 (glancing) to 4 (hit).
- `IsValidAttack` (`0x004d1120`) and `SetWalkMode` (`0x004cf000`, guards
  for interactive attacks, a dead character, the torch root, and a copy
  of the root rather than a new block) are still the 1998 bodies with
  retail patches.
- Command cases not dispatched: bow mode (2), retail's sneak toggle (3),
  invoke (0x14–0x17), use/get, bow aim/shoot, walk/combat/sneak actions.
- `TComplexObject::TryCommand` / `ForceCommand` (`0x004db450` /
  `0x004db4d0`; the `recon/discovered/cls_TCO_*` file names have the two
  swapped) differ from the 1998 bodies: when no transition is found,
  retail's ForceCommand tries a commanded state's 100% variant before a
  random one, and a root-to-root transition last.
