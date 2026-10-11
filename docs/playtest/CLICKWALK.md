# Click to walk in retail: forensics and the `clickwalk` build

The rule ([PLAN.md](PLAN.md) §10), for both games:

> A left click on open ground (no object under the pointer, outside
> combat's attack cursor) walks Locke to the map point under the pointer,
> through the game's own `Goto` (retail `0x004cedb0`), which turns to any of
> 256 angles and snaps onto the point within 8. Holding the run key while
> clicking runs.

This document is the retail half: where the shipped game drops such a
click, the gates around it, the point the right-button walk resolves, how
`Goto` is called and what the run key does, then the patch (the named build
`clickwalk`, catalogued in
[`recon/retail_asm/patches/clickwalk/`](../../recon/retail_asm/patches/clickwalk/README.md)),
how it was verified, and what the port needs to match it (§9).

Sources: disassembly of the byte-identical rebuilt baseline
(`recon/retail_asm/baseline`, SHA-256 `28bec273…72b5`), the Ghidra
decompiles in `recon/discovered/mapinput/`, and runs of the original code in
the emulator (§8). Companion to
[MAP_INPUT.md](../gameflow/forensics/MAP_INPUT.md) (the pick, the cursor,
clicks) and [PLAYER_INPUT.md](../gameplay/forensics/PLAYER_INPUT.md) (keys,
`Command`, `UpdateMove`). Confidence: **[C]** read in the disassembly or
run in the emulator, **[I]** inferred, **[U]** unknown.

## 0. Summary

- A left release whose press and release were both on nothing reaches
  `0x0044ff51 test ebp, ebp` / `0x0044ff53 je 0x0045046f` in
  `TMapPane::MouseClick` `0x0044f140` and goes straight to the function's
  common exit: retail does nothing. [C]
- The combat gate (`0x0044fbef..0x0044fc29`) and the bow gate
  (`0x0044fc2f..0x0044fcde`) come earlier on the same release; the editor
  takes a separate branch at `0x0044f188`. None of them reaches
  `0x0044ff53`, except a release in combat or bow mode that their gates let
  through (§2.3). [C]
- The right-button walk's point is `ScreenToWorld` `0x0046dad0` of the
  pointer's map-screen position at Locke's z + 50 (UpdateMouseMovement
  `0x0044ee00`; the same expression is `GetMouseMapPos` `0x0044ed50`). [C]
- `Goto` is `thiscall` on the character, `(int x, int y, TObjectInstance*
  pickup)`, `ret 0xc`, returns 1 when `Go` took the walk, else 0. [C]
- The run key needs no patch: while it is held the root is "run" and
  `Go` builds the run step; the walk keeps its target when the gait
  changes. [C in the emulator for the press; I for a mid-walk change]
- The player is the global `0x00667fcc`; the map pane is `0x006668d8`. [C]
- **The patch**: `0x0044ff53` now jumps to a 101-byte cave at `0x005a2a40`
  (`.text`'s zero padding) that, outside combat mode and bow mode, calls
  `ScreenToWorld(posx + x, posy + y, &p, Locke.z + 50)` and
  `Player->Goto(p.x, p.y, 0)`, then rejoins `0x0045046f`. Build SHA-256
  `016a73ad2d38a2318c5397690146387c48a4e666f50e3ab42ee6fc36ca303762`.
- **Verified** in the emulator on the original `MouseClick`: the click
  calls `Goto` once with exactly the right-button walk's point and Locke's
  walk block heads there; the baseline does nothing; eleven other click
  situations run identically on both builds; booted to game init on the HUD
  slot's machine, the two builds' whole machines differ only in the patched
  bytes; over game ticks on flat ground Locke turns to the point and stops
  on it (§8). **In a loaded level** (the play slot, 2026-10-11) a click
  into the room walks Locke onto the point exactly; a goal past the room's
  ledge ends in retail's blocked slide (§8.4).

## 1. The left button in `TMapPane::MouseClick` (`0x0044f140`)

`thiscall (button, x, y)`, `ret 0xc`; `esi` = the pane, `edi` = x and
`ebx` = y on entry to the body; `x`, `y` are pane-relative (the body tests
`0 <= x < w` and turns them into the side tabs' frame as `pane.x - tabs.x +
x`, `0x0044f146..0x0044f17b`; the map pane sits at 0, 0). In the body's
frame x and y are at `[esp + 0xec]` and `[esp + 0xf0]`. [C]

### 1.1 Press (button 1), `0x0044f97d..0x0044fb83`

| Address | What [C] |
|---|---|
| `0x0044f986..0x0044f9b0` | Player (`0x00667fcc`) non-null, `0 <= x < w` (`+0x0c`), `0 <= y < h` (`+0x10`), else return |
| `0x0044f9b6..0x0044fa82` | Root (`[Player + 0xe0]`) action 3 (combat mode) and hover cursor `+0x12c` none (-1) or swords (6): `on = OnObject(x, y, 0)`; a character (class `0xc`) other than the target (`root+0x44`) becomes the target if `IsEnemy` `0x004c89c0` accepts it (`SetFighting` `0x004d4790`), else nothing; anything else (nothing, the target, an object) is `ButtonAttack(random(1, 3))` `0x004d2480`. Returns **without** setting `clicked` |
| `0x0044fa85..0x0044fb21` | Root action `0x19` (bow mode), cursor -1 or 6: if `IsBowDrawn` `0x004d1050` is false, `DrawBow` `0x004d0aa0` and `AimBow(ConvertToFacing(Locke, GetMouseMapPos(50)))` `0x004d0c70`; `clicked` (`+0x120`) = 1. `onobject` is **not** touched |
| `0x0044fb24..0x0044fb83` | Otherwise: `on = OnObject(x, y, 0)` `0x00452520`; `onobject` (`+0xf8`) = `on->mapindex` (`+0x40`) or -1; `clicked` = 1; an inventory item (slot `0x134`) also goes to the stats pane `0x005496a0` |

### 1.2 Release (button 4), `0x0044fb86..0x004504c2`

| Address | What [C] |
|---|---|
| `0x0044fb8f..0x0044fba7` | Player non-null and alive: slot `0x1c0` (`0x004d6f00`, Health through GetObjStat) >= 1, else to the exit |
| `0x0044fbad..0x0044fbe9` | `0 <= x < w`, `0 <= y < h`, else to the exit |
| `0x0044fbef..0x0044fc29` | **Combat gate**: root action 3, cursor -1 or 6, and `clicked` → exit |
| `0x0044fc2f..0x0044fcde` | **Bow gate**: root action `0x19`, `IsBowDrawn`, and `clicked` → `ShootBow(ConvertToFacing(Locke, ScreenToWorld(posx - pane.x + cursorx, posy - pane.y + cursory, Locke.z + 50)))` `0x004d0fd0` → exit |
| `0x0044fce3..0x0044fd3a` | `with`: the item held from the pack (`0x0065d674`/`0x0065d67c`), the belt (`0x0065b088`/`0x0065b090 + 0x10b`), else `[Player + 0x2a0 + 4·[0x0065b878]]`, the equipment pane's held slot |
| `0x0044fd3a..0x0044fd5b` | `on = OnObject(x, y, with)`; `ebp` = on, `ebx` = `on->mapindex` or -1 |
| `0x0044fd5e..0x0044fd66` | not `clicked` → the drop path `0x0044fff8` (a drag from the HUD let go over the map) |
| `0x0044fd6c..0x0044fd6e` | `on` null → `0x0044ff45` |
| `0x0044fd74..0x0044fd92` | `Distance(on, Player)` (slot 4) <= `0x60`, or `on` a character (class `0xc`) → `0x0044ff45` |
| `0x0044fd98..0x0044ff40` | farther: walk toward it — `FindClickPos` `0x0044e930`, 64 short, the line tested every 32 (`GetWalkHeightRadius` `0x004530a0`), `Goto(goal.x, goal.y, on)` at `0x0044ff17`, else `ITEMTOFAR` |
| `0x0044ff45..0x0044ff4b` | `cmp [esi+0xf8], ebx; jne 0x0045046f`: the press must have been on the same object, or on nothing for a release on nothing |
| **`0x0044ff51..0x0044ff53`** | **`test ebp, ebp; je 0x0045046f`: on nothing, nothing happens.** This is where open ground falls through |
| `0x0044ff59..0x0044fff3` | on the object: `Use(Player, -1)` (slot `0xbc`), or for an inventory item `Pickup` `0x004cfef0` (not players or characters) |
| `0x0045046f..0x004504b5` | the common exit: `clicked` = 0; inside the pane the drag bitmap, name and object are cleared (`0x0043a100`, `0x0043a170`, `0x0043a140`) |

So "open ground" for this rule is exactly: `OnObject` returned null at the
press and at the release. `OnObject` is MAP_INPUT.md §2: no 3D mesh drawn
over the pointer's probe pixels, and no 2D object with a cursor whose
rectangle holds the point (tiles never count). [C, MAP_INPUT.md] Locke
himself is a mesh the probe picks, so a click on his own body takes the
same-object branch (`Use`), not the walk. [I]

**Correction to MAP_INPUT.md §4.2**: the block the press and release test,
`[Player + 0xe0]`, is the **root** action block (the mode), not `doing`
(`+0xd8`). `Goto` (`0x004cedec..0x004cedfc`) reads desired `+0xdc`, root
`+0xe0` and doing `+0xd8`, and the combat slot's layouts agree. "Combat
mode" is root action 3, "bow mode" root action `0x19`. [C]

## 2. Paths the patch must leave alone

### 2.1 The editor

`0x0044f188`: with `Editor` (`0x00668154`) set, the whole function is the
editor's branch (`0x0044f197..0x0044f84a`); the patched instruction is on
the play branch only. [C]

### 2.2 Combat and the attack cursor

In combat mode with the attack cursor (none or swords), the press attacks
or takes a target and leaves `clicked` clear; the release then fails the
`clicked` test of the combat gate, goes on to `OnObject`, finds `clicked`
clear and takes the drop path, which does nothing without a held item. It
never reaches `0x0044ff53`. [C]

### 2.3 What the gates let through

Two releases pass the gates and still reach `0x0044ff53` with nothing
under the pointer:

- **Combat mode with a non-attack cursor**: the hover cursor is
  re-picked only every 8th frame (MAP_INPUT.md §3), so for up to 8 frames
  after the pointer leaves a door, an item or a person, a press on open
  ground takes the plain path (`clicked` set, `onobject` -1).
- **Bow mode with the bow not yet drawn at the release**: the press drew
  it and set `clicked` but left `onobject` as the last press made it.

Retail does nothing in both. The patch keeps that: it walks only when the
root is neither combat (3) nor bow (`0x19`). In combat mode open ground
always shows the attack cursor once the cursor is re-picked, so this is
the rule's "outside combat's attack cursor" without its 8-frame race. [C]

### 2.4 Clicks on objects, drags, the far walk

Untouched: they branch away before `0x0044ff53` (§1.2). The far walk's own
`Goto` at `0x0044ff17` is unchanged. [C]

## 3. The map point under the pointer

### 3.1 `UpdateMouseMovement` `0x0044ee00`

Called for the right button's press (`0x0044f901`, `push ebx; push edi`:
y, x) and for moves while the right button walks; it ignores those
arguments and reads the polled cursor:

```
0x0044ee13..0x0044ee58:
  p = ScreenToWorld(posx - pane.x + cursorx,      // [esi+0x78] - [esi+4] + [0x00668510]
                    posy - pane.y + cursory,      // [esi+0x7c] - [esi+8] + [0x00668514]
                    &p, Player.z + 0x32)          // [Player+0x18] + 50
0x0044ee67: angle = ConvertToFacing(Player.pos, p)   // 0x0046dc60
```

then the 16-unit dead zone, the wedge cursor and the numpad key (MAP_INPUT.md
§5). [C] The same expression, as a function, is `GetMouseMapPos(S3DPoint*
p, int zoffset)` `0x0044ed50` (`thiscall`, `ret 8`), which the bow's press
calls with `0x32` (`0x0044faea`); `0x0044ed90` is `GetMouseMapAngle`. [C]

### 3.2 `ScreenToWorld` `0x0046dad0`

`cdecl (sx, sy, S3DPoint* out, z)`: if z, `sy += z * 867 / 1000` (the
multiply by 867 at `0x0046dada..0x0046dae2`, the division by 1000 as a
magic multiply, truncating toward zero); `out = (sy + sx/2, sy - sx/2, z)`
with `sx/2` truncating toward zero (`cdq; sub; sar`). No globals. [C]

### 3.3 The polled cursor and the click's own point

`cursorx`/`cursory` (`0x00668510`/`14`) are set in one place in play: the
screen loop's poll after its message pump, `GetCursorPos` less the client
origin (`GetClientRect`, `ClientToScreen`), `0x00490d6f..0x00490dd7`, which
also sends the move on (`0x0048f950`). (The cursor draw `0x0043a4aa` only
clamps them.) A click's `x`, `y` come from the button message itself:
`WndProc` `0x00448cc0` → `0x0048f870(button, LOWORD(lParam), HIWORD(lParam))`
(`0x004498d0..0x004498e5` for `WM_LBUTTONUP`) → `TPlayScreen::MouseClick`
`0x0047ce80` → `TScreen::MouseClick` `0x004903b0` → the pane. Messages are
pumped before the poll, so during a click `cursorx` is the position from
the previous pass. [C]

The patch therefore converts **the click's own point**, `(posx + x, posy +
y)`, by UpdateMouseMovement's conversion (same function, same z): the point
`OnObject` just found to be open ground. It equals `GetMouseMapPos`'s
whenever the pointer did not move between the last poll and the button
message; under the playtest hosts it always is the click point, including a
press and release delivered in one tick (PROTOCOL.md §2), where the polled
cursor would still be the previous one. It is also what the port computes
(§9). [C for the code paths; the choice is the patch's]

## 4. `Goto` `0x004cedb0`

```
thiscall TCharacter::Goto(int x, int y, TObjectInstance* pickup), ret 0xc
  angle = ConvertToFacing(&this->pos, &(x, y, this->pos.z))  0x004cedd8  (0..255)
  if !Go(angle): return 0                                     0x004cede3  (Go 0x004ce350)
  ab = desired (+0xdc) != root (+0xe0) ? desired : doing (+0xd8)
  ab->target = (x, y, this->pos.z)                            +0x38, +0x3c, +0x40
  ab->flags |= 0x1000                                         +0x60 (a Goto's walk)
  if pickup: this->+0x288 = pickup                            (picked up on arrival)
  net notify 0x00586680 (returns at once: 0x00676828 = 0 offline)
  return 1
```

[C, `0x004cedb0..0x004cee49`] `Go` refuses (returns 0, so `Goto` does
nothing) when Locke is dead, in an interactive action, or doing something
other than his root or a move, or when the first 4 units are blocked
(COMBAT_MOVEMENT.md §2 and the port's REVSYNC of `0x004ce350`; not re-read
here). The walk then runs in `ResolveMove`, which arrives within 8
(MAP_INPUT.md §4.3, `0x004c6155`; not re-read here). A Goto walk survives
`UpdateMove`'s stop with no direction held because its block has a target:
`0x0047e030..0x0047e043` stop only a move whose target is 0, 0, 0. [C; run
in §8.2, `open-ground-walk`]

## 5. The run key

- The run key's down and up are `Command` `0x0047cf40` cases `0x4a`/`0x4b`:
  with the held state's bit `0x100`, `SetRunMode` `0x004cf490`, else
  `SetWalkMode` `0x004cf000` (PLAYER_INPUT.md §3). [C]
- Standing in walk mode, `SetRunMode` makes the root "run" (`0x005e011c`).
  `Go` names its step after the root, so the click's `Goto` builds "runf":
  run in the emulator (§8, `open-ground-run`). [C]
- Mid-walk, `SetRunMode` and `SetWalkMode` rebuild the moving `doing` block
  with the copy constructor `0x004daae0` (`rep movsd` of all 100 bytes, then
  the new name and action; calls at `0x004cf76e`, `0x004cf81f` and
  `0x004cf1d8`, `0x004cf27f`, `0x004cf3c3`): the target and the `0x1000`
  flag go with it, so pressing or letting go of the run key during a click
  walk changes the gait and keeps the destination. [C for the copy; I for
  the whole path, not run mid-walk]
- With no key and no mouse button held at all, the screen loop drops the
  run state: `GetKeyboardState` shows nothing down and `0x00668518` is 0 →
  `SetWalkMode` if bit `0x100` was set, state cleared
  (`0x00490e77..0x00490ed0`). Run lasts while the key is physically held.
  [C]

So "holding the run key while clicking runs" is retail's own behaviour once
the click reaches `Goto`; the patch adds nothing for it.

## 6. The player and the map pane

| | |
|---|---|
| Player | `0x00667fcc`, `TPlayer*` (class `0xb`, vtable `0x005b4f30`, 0x674 bytes); position `+0x10/+0x14/+0x18`; doing `+0xd8`, desired `+0xdc`, root `+0xe0` |
| Map pane | `0x006668d8` (vtable `0x005a5370`): origin `+4/+8`, size `+0xc/+0x10`, scroll origin `posx`/`posy` `+0x78/+0x7c`, `onobject` `+0xf8`, right-button walk flag `+0x11c`, `clicked` `+0x120`, held numpad key `+0x128`, hover cursor `+0x12c` |
| Pointer | `cursorx`/`cursory` `0x00668510`/`0x00668514` (client pixels) |

[C]

## 7. The patch: named build `clickwalk`

Catalogue: [`recon/retail_asm/patches/clickwalk/`](../../recon/retail_asm/patches/clickwalk/README.md)
(source diff `clickwalk.patch`, `manifest.json`, `build.py`,
`disassembly.diff`). Built from the baseline assembly source through the
named-build workspace (`tools/retail_runtime/variants.py`); NASM 3.02.

**At `0x0044ff53`** the `je near` keeps its opcode and length, retargeted:

```
0044ff53: 0f8416050000   je 0x45046f     ->   0f84e72a1500   je 0x5a2a40
```

**The cave**, `0x005a2a40..0x005a2aa5`, in the 1,482 zero bytes between the
last code of `.text` (`0x005a2a35`) and the section's raw end:

```
005a2a40  mov  eax, [0x667fcc]          ; Player (non-null: 0x0044fb95)
005a2a45  mov  ecx, [eax+0xe0]          ; root
005a2a4b  test ecx, ecx / je .done
005a2a4f  mov  edx, [ecx]               ; root action
005a2a51  cmp  edx, 3    / je .done     ; combat mode
005a2a56  cmp  edx, 0x19 / je .done     ; bow mode
005a2a5b  sub  esp, 12                  ; S3DPoint p
005a2a5e  push Player.z + 0x32          ; z
005a2a65  push &p
005a2a6a  push posy + y                 ; [esi+0x7c] + [esp+0x104]
005a2a75  push posx + x                 ; [esi+0x78] + [esp+0x104]
005a2a80  call 0x46dad0                 ; ScreenToWorld; add esp, 16
005a2a88  mov  ecx, [0x667fcc]
005a2a8e  push 0 / push p.y / push p.x
005a2a98  call 0x4cedb0                 ; Goto(p.x, p.y, 0); add esp, 12
005a2aa0  .done: jmp 0x45046f           ; MouseClick's common exit
```

It uses only `eax`, `ecx`, `edx` (both callees keep `ebx`, `esi`, `edi`,
`ebp`); the exit needs `esi` and the frame, both intact.

**The header**: `.text` VirtualSize `0x001a1a36` → `0x001a1aa5` (file
`0x208`), so the loaded section covers the cave on any loader. The mapped
size is unchanged (both round to `0x001a2000`, the raw size).

| | |
|---|---|
| Declared spans (file offsets) | `[0x208, 0x20c)`, `[0x4ff53, 0x4ff59)`, `[0x1a2a40, 0x1a2aa5)` |
| Unchanged | image base, entry, every section's RVA, raw offset and raw size, SizeOfImage, file length, every other byte |
| SHA-256 | `016a73ad2d38a2318c5397690146387c48a4e666f50e3ab42ee6fc36ca303762` |

Behaviour it adds, and only this: a release inside the pane, on nothing,
pressed on nothing, by a live Locke, in neither combat nor bow mode, calls
`Goto` toward the click's point at his own height. Like retail's far-object
walk (`0x0044ff17`), it doesn't look at script control: while a script holds
Locke, `Go` refuses unless he is at his root or walking (§4); the director
doesn't click during scenes (PLAN.md §9). A `Goto` that `Go` refuses does
nothing, as retail's does.

### Build it

```sh
PY=/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python
"$PY" recon/retail_asm/patches/clickwalk/build.py
```

prints the record: `build_id` (e.g. `clickwalk-016a73ad2d38a231-4c19d80b`),
`executable` (`recon/retail_asm/variants/clickwalk/builds/<id>/Revenant.clickwalk.exe`)
and `build_json`. The play host (`tools/retail_runtime/slots/play/`) takes
that `build.json`; `HudWorld.open(executable, build=build_json)` boots it.

## 8. Verification (2026-10-10)

### 8.1 Build and contract

```sh
"$PY" recon/retail_asm/patches/clickwalk/build.py          # SHA, contract, bytes: pass
"$PY" tools/retail_asm/retail_patch.py diff --manifest recon/retail_asm/patches/clickwalk/manifest.json \
  --baseline recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --variant recon/retail_asm/variants/clickwalk/builds/<id>/Revenant.clickwalk.exe
```

- The assembled SHA-256 is the manifest's; `build_contract.verify_build`
  passes with the declared spans (an undeclared byte fails it,
  `test_clickwalk.py`); NASM's only warning is the baseline's own
  (`unsigned byte exceeds bounds`, the same line).
- The whole-`.text` linear-sweep disassembly diff has two hunks: the `je`
  at `0x0044ff53`, and the cave with the tail's zero count, plus the
  header's VirtualSize. Changed bytes all fall in the declared spans
  (`disassembly.diff`).

### 8.2 The click in the emulator

`tools/retail_runtime/clickwalk_probe.py` runs the original
`TMapPane::MouseClick` on the map pane global with the pointer at a pane
point, for the baseline and the build, and compares. Locke is a real
`TPlayer` built by the combat slot's `CombatWorld` (vtable, action blocks by
the original constructor, his states and stats from the case), so `Goto`,
`Go`, `SetRunMode` and the transition lookup run as original. `OnObject` is
the seam that says "open ground" (null) or names an object; `ScreenToWorld`
and `Goto` are observed and run as original; combat and bow actions, the
cursor bitmaps and the screen's key press are recorded seams. Locke stands
at (6280, 4338, 33), facing 64, the view centred on him.

| Case | baseline | clickwalk |
|---|---|---|
| `open-ground`: press and release at (400, 200), walk mode | nothing | `Goto(6323, 4301, 0)`; doing `walkf` (via `walk to walkf`) with target (6323, 4301, 33) and the `0x1000` flag |
| `open-ground-turn`: (200, 330), behind him | nothing | `Goto(6353, 4531, 0)`; a pivot-first `walk` block, `waitpivot`, the same target |
| `open-ground-run`: `SetRunMode` first (the run key), then (400, 200) | root "run", nothing more | `Goto(6323, 4301, 0)`; doing `runf` with the target |
| `open-ground-walk`: the click, then 60 game ticks on flat ground | Locke stands still | facing 48 → 35 at once, re-aimed 34..36 each tick; at tick 14 Locke stands **exactly** on (6323, 4301) (`ResolveMove`'s within 8, then `MoveTo`); the `walkf to walk` stop transition then carries him 7 more units (its synthetic motion) and he stands at his root |
| `right-button` at (400, 200) | `ScreenToWorld(2022, 5241, z 83)` → (6323, 4301), `wedge-e`, key `0x27` | identical |
| combat, attack cursor (none / swords) | `ButtonAttack(2)`, release nothing | identical |
| combat, stale door cursor | nothing | identical; the cave ran and declined |
| bow, not drawn at release | `DrawBow`, `AimBow(35)`, release nothing | identical; the cave ran and declined |
| bow drawn | `ShootBow(35)` | identical |
| press on Tendrick, release on ground; press on ground, release on him; drag release (no press); outside the pane; Locke dead | nothing | identical |

The game ticks of `open-ground-walk` are retail's own order (combat kata
M8): `UpdateMove` `0x0047de30` with nothing held and a control used before
(so its stop is armed), then `Pulse` (UpdateAction, `ResolveMove`), `Move`
(`MoveStep`, the walk grid as original over a flat ground seam),
`SetObjectMotion` over a synthetic walk cycle (5 units a frame), the game
frame, `NextFrame`. So the "any of 256 angles, onto the point within 8"
half of the rule and `UpdateMove`'s stop exemption are retail's own code,
run.

The point the click walks to is, argument for argument, the right-button
walk's for the same pointer (`ScreenToWorld(2022, 5241, &p, 83)`), and an
independent Python `ScreenToWorld` gives the same (6323, 4301). The
`clickwalk` code at `0x005a2a40` ran in the four walking cases and the two
declining ones, never otherwise; the baseline never runs there.

```sh
"$PY" tools/retail_runtime/clickwalk_probe.py --baseline recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --clickwalk <exe> --build <build.json>                    # failures: []
"$PY" -m unittest discover -s tools/retail_runtime -p 'test_clickwalk.py'   # 7 tests OK
```

### 8.3 On the HUD slot's booted game

Both executables booted by their own code through game init on the HUD
slot's machine (`HudWorld.open(exe, build=…)`; `hudworld.py` now takes a
named build that passes the build contract). After the boot the two
machines are the same except for the patch: the boundary calls (16), the
host modules' states (system, files, profile, DirectDraw, windows, GDI),
the configuration (NoTexOverlay 0, InternalRaster 1, TexturedDevice 0) and
all 631 mapped guest regions byte for byte, apart from the patched bytes
themselves (`0x00400208`, `0x0044ff55..58`, `0x005a2a40..a5`). On those
booted worlds the same click suite gives the same results as in §8.2
(`failures: []`). No level is loaded at that point (the HUD boot stops at
`0x00486e7f`, before the first screen), so the player and the pane state
are still the fixture's.

```sh
"$PY" tools/retail_runtime/clickwalk_probe.py --world hud \
  --baseline recon/retail_asm/baseline/Revenant.rebuilt.exe --clickwalk <exe> --build <build.json>
```

About 2 to 4.5 minutes per executable to boot on a loaded Mac (95 s
unloaded); the boot images are cached in `slots/hud/__pycache__`.

### 8.4 In a loaded level (play slot, 2026-10-11)

Run by the play slot's agent (`slots/play`, in-process, the software
machine with Software3D = Yes; their script
`scratchpad/retail/proto/clickprobe.py`), not by this track. The start is a
quick save taken just after the opening ("Quick Save 1"), with Locke at
(1383, 488, 17) and Rahul hostile nearby. That save was made after Locke
walked east until the room's east ledge stopped him, so he stands against
it at x ≈ 1383, and any goal with a larger x is past the edge.

- **Baseline**: a click on the floor leaves Locke where he is (150 ticks).
- **Into the room**, screen (160, 180): the goal is (1362, 534) (z + 50 =
  67 for the conversion). After the release the doing block is `walk`
  with target (1362, 534, 17) and flag `0x1000`. His facing turns 64 →
  112 → 145, then `walkf` at angle 146–147: (1379, 498) at t = 36, (1371,
  514) at t = 39, (1364, 529) at t = 42. At t = 45 **he stands exactly on
  (1362, 534)** and the doing block returns to the root.
- **Toward the ledge**, screen (260, 250): the goal is (1482, 554), past
  the edge. The target is set as above (flags `0x1600`: walkto, noroot,
  waitpivot). He faces 88 → 75 but only slides along +y at about 1 unit a
  tick, (1383, 488) → (1383, 527), and gives up at t = 63 at (1380, 532),
  at his root.
- Their first sample, screen (150, 300) → (1477, 659), is the same case:
  past the ledge, a slide along +y, stopped 99 short.

So in a real level the patched click reaches `Goto` with the rule's point,
Locke turns to it and stands on it, and a goal past an edge ends in
retail's own blocked slide, as a far-object walk would. Not logged in
those runs: the Move bits per tick and the walk-grid heights along the
blocked line.

Note on the point itself (the rule's, not the patch's): at z + 50 the
screen point lands 43 units further in +x and +y (one screen row of 43:
`50 * 867 / 1000`) than the floor point under the pointer at Locke's own
height; near an edge that can put the goal past it. The director's
`walk.py` should treat such a stop as "blocked" and pick a nearer point.

### 8.5 Still to verify in real play

- In a loaded level: with the run key held, running; a re-click mid-walk
  retargeting. The play slot runs this build.
- Mid-walk run key changes (§5, [I]).
- The DOSBox lab has not run the build.

## 9. The port (read only)

**`GetMouseMapPos` computes the same point, given the same screen point.**
`TMapPane::GetMouseMapPos(p, zoffset)` (`src/mappane.cpp`) is
`PaneToMapScreen(cursorx, cursory)` then `::ScreenToWorld(mapx, mapy, p,
Player z + zoffset)`. `::ScreenToWorld` (`src/object.cpp`) is retail's
`0x0046dad0` exactly (867/1000 and `x/2`, C truncation). `PaneToMapScreen`
is the map view's camera origin plus the pointer, `x + posx` in Classic
640×480 at zoom 1 (at other resolutions and zooms the port's own mapping,
no retail counterpart). The port sets `cursorx`/`cursory` from the button
event before dispatching it (`src/revmain.cpp`), so at a release
`GetMouseMapPos` converts the click's point, as the patch does. If the
input queue (PLAN.md §6.1) dispatches a click before applying its
position, it would not: converting the release's own `(x, y)`
(`PaneToMapScreen(x, y, …)`) avoids depending on that.

**What `PlayMouseRelease` needs** (`src/mappane.cpp`), in its `if (clicked)`
branch, after the existing far-object and same-object cases:

```cpp
if (!on && onobject == -1 && !Player->IsCombat() && !Player->IsBowMode())
{
    S3DPoint p;
    GetMouseMapPos(p, 50);              // or from the release's (x, y)
    Player->Goto(p.x, p.y, nullptr);
}
```

- `onobject == -1`: a press on an object released over nothing still does
  nothing (`0x0044ff4b`).
- The mode test is the root (`IsCombat()`/`IsBowMode()` are `IsRoot`), as
  the cave tests it; it also covers the port's own bow press, which, like
  retail's, leaves `onobject` alone.
- The other gates are already the port's: alive (`!IsDead()`, retail
  Health >= 1), inside the view, the combat attack-cursor and bow-drawn
  early returns, `OnObject` with the held item.
- `Goto(x, y, nullptr)` is the port's REVSYNC of `0x004cedb0` (Go, the
  block, `walkto`; `gotoitem` untouched without an item).
- For the run key, the port's `SetRunMode`/`SetWalkMode` should keep a
  moving block's target when they swap the gait, as retail's copy does
  (§5); worth a check, not part of the click rule.
