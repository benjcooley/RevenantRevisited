# Exits — forensics

How retail Revenant takes the player through doors, stairs, teleporters
and level changes: the `TExit` class family, the exit list (`exit.def`),
the strip a player walks onto, the scripted doors in `master.s`, the
commands scripts use, and how a level is left and entered. Input for the
exits port. Companion to [GAME_FLOW.md](GAME_FLOW.md),
[SCRIPT_ENGINE.md](SCRIPT_ENGINE.md), [COMMAND_SYSTEM.md](COMMAND_SYSTEM.md),
[SAVE_GAME.md](SAVE_GAME.md) and [../ARCHITECTURE.md](../ARCHITECTURE.md) §3.

Sources: Ghidra on `data/Revenant.exe` (authoritative), the 1998 source
(`/Users/benjamincooley/projects/Revenant/exit.cpp`, pre-release), and the
shipped data: `Modules/Ahkuilon.rvm` (`exit.def`, `area.def`, the module
scripts, the `Map/` sectors), `resources.rvr` (`master.s`, `english.def`)
and `imagery.rvi` (`class.def`, door imagery). Every retail function cited
is in [`recon/discovered/exits/`](../../../recon/discovered/exits/) as a raw
decompile named `<Role>_<addr>.cpp`; vtables are in
[`VTABLES.txt`](../../../recon/discovered/exits/VTABLES.txt). Command
decompiles are in `recon/discovered/commands/`.

Confidence per claim: **[C]** confirmed (read in the decompile or the
disassembly, or measured in the data), **[I]** inferred (strong evidence,
not read directly), **[U]** unknown.

## 0. Summary

- **Doors are scripts.** Retail's `TExit::Use` no longer opens a door: it
  checks the lock and runs the object's USE trigger. The door types
  `Door1`, `Door2`, `CavDoor1/2`, `PortEW/NS` have USE prototypes in
  `master.s`: walk to the door, open it (`operate player` swings it away
  from Locke), fade out, fade in, `activate` (teleport), close it.
  `InDoor1` and `InportEW/NS` open, walk Locke through and close, with no
  teleport (§1.9). [C]
- **Walking onto a strip** teleports only *unscripted* exits whose
  per-object `AutoActivate` stat is 1 (stairs, teleport pads, lab gates,
  cave mouths). `Door1`, `Door2`, `PortEW`, `PortNS` are excluded by type
  name, so walking into those doors does nothing. Only **players** are
  checked (the `PlayerManager` list), not NPCs. [C]
- **A scripted exit** that the player walks onto fires its ACTIVATE
  trigger with the player as `user` and stops there; the script does the
  rest (the town teleport stones: fade, `player.pos x y z level`). [C]
- **The teleport** is the same sequence as the `pos` command: clear the
  character's combat target, snap the camera if it follows that
  character, `SetPos(target, level)`. There is no loading screen. A level
  change loads the destination's sectors synchronously in the next map
  update while the text bar shows "Loading Map... Please Wait" with a
  progress bar; the player is put back into the map once its sector is
  loaded; the area manager then leaves the old area (its scripts are
  dropped) and enters the new one (its script file is loaded). [C]
- **Exit list**: `exit.def` maps an exit object's *instance name* to a
  destination `(x, y, z) level`. 155 entries ship; 94 name an EXIT object
  in the base map. [C]
- **New in retail**: a 6-state door (opening/closing toward or away from
  the user), `Operate(user)`, `isoutside`, the `AutoActivate` and
  `TileFlags` stats, the localized locked message, two TRAP-class device
  classes (`TrapLever`, `TrapPressPlate`). Gone: `TSpikeWall` (spike walls
  are TRAP objects now), the 1998 "Door" push hack, the Delay counter.
- **Port**: ported (2026-10-05); what remains is listed in §7. Before
  that, `src/exit.*` was the 1998 code, the exit list was never read, a
  const mismatch hid every `CursorType` override, and a teleport to another
  level dropped the player out of every sector.

## 1. TExit in retail

### 1.1 Identification and vtable

Retail objects are made by builders; each `REGISTER_BUILDER` stores a
one-slot builder vtable whose `Build` news the object and writes its
vtable. Following those stores (the code that writes a builder vtable
pushes the builder's name just before) gives every class of the exit
family [C]:

| Builder name | Builder vtable | Build | Object vtable | Class | Size | Registration |
|---|---|---|---|---|---|---|
| `EXIT` | `0x5b2250` | `0x0050e360` | **`0x5b27cc`** | TExit | 0xf8 | `0x0050c640` |
| `PressPlate` | `0x5b2254` | `0x0050de90` | `0x5b2258` | TPressPlate | 0xf8 | `0x0050da00` |
| `UpBlock` | `0x5b24dc` | `0x0050df80` | `0x5b24e0` | TUpBlock | 0xf8 | `0x0050da60` |
| `DragonEnt` (3D animator) | `0x5b2760` | `0x0050e060` | `0x5b2764` | TDragonEntAnimator | 0xfc | `0x0050db90` |
| `LEVER` | `0x5b27c8` | `0x0050e430` | `0x5b2a4c` | TLever | 0x108 | `0x0050dc00` |
| `TrapLever` (CLASS TRAP) | `0x5b70b8` | `0x00527870` | `0x5b70bc` | TTrapLever | 0xf8 | `0x00524d80` |
| `TrapPressPlate` (CLASS TRAP) | `0x5b72f4` | `0x00527950` | `0x5b72f8` | TTrapPressPlate | 0xfc | `0x00524fc0` |

There is no `SpikeWall` or `ELEVATOR` builder (no such strings in the
exe) [C]. A type uses the builder named like the type, else the class's
(`EXIT`) — the 1998 `TObjectClass::AddType` rule, case-insensitive [I]:
`Lever` → TLever, `PressPlate` → TPressPlate, both `UpBlock` types →
TUpBlock, every other EXIT type → TExit.

TExit's vtable (`0x5b27cc`, 0xa0 slots) [C] — TContainer's (`0x5a8118`)
ends at 0x244; TExit adds 0x248..0x27c:

| Slot | Function | Retail |
|---|---|---|
| 0x0 | `0x0050e400` | scalar dtor |
| 0xbc | `0x0050d1a0` | `Use(user, with)` (§1.8) |
| 0xc0 | `0x0050d370` | `CursorType(with)` |
| 0xc4 | `0x0050d980` | `UseRange` (empty, as 1998) |
| 0x110 | `0x0050d640` | `Pulse()` (§1.6) |
| 0x154 | `0x00477d30` (base) | `CommandDone()` = `+0x80` |
| 0x158 | `0x0050e350` | `SetCommandDone(v)`: `+0x80 = v` (same body as the base `0x00471b50`) |
| 0x160 / 0x164 | `0x0050d990` / `0x0050d9c0` | `Load` / `Save` (§1.8) |
| 0x1f0..0x21c | `0x0050e170..0x0050e300` | stat get/set pairs (§1.2) |
| **0x248** | `0x0050d3a0` | **`Activate(user, flag)`** (§1.7) |
| 0x24c | `0x0050d510` | `Unactivate()` |
| 0x250 | `0x0050d230` | `Operate(user)` (§1.8) |
| 0x254..0x278 | `0x0050e1a0..0x0050e330` | stat get/set pairs (§1.2) |
| 0x27c | `0x0050ce70` | `GetExitStrip(regx, regy, regz, w, l, h)` (§1.4) |

Base slots the exit code calls [C]: 0x8 `SetPos`, 0x18 `SetState(int)`,
0x20 `HasAnimator`, 0x24 `GetAnimator` (`+0x58`), 0x40 `SetFlags`,
0x138 `FindState(name, pcnt)`, 0xdc `GetObjStat(id)`, 0xd8 `GetStat(id)`.

### 1.2 Fields and stats

| Offset | Field | Evidence |
|---|---|---|
| +0x0c | `state` (u16) | Operate, SetExitState [C] |
| +0x36 | facing byte | GetExitStrip, IsOutside [C] |
| +0x38 | instance `name` (char*) — the exit-list key | Activate compares it with `ref->name` (`0x0050d445`) [C] |
| +0x4c | `inf` (type info; `*inf` = type name) | Pulse's type test [C] |
| +0x80 | `commanddone` | slots 0x154/0x158 [C] |
| +0x84 | `script` | Activate, `TObjectInstance::Pulse` [C] |
| +0xdc, +0xe0 | zeroed by the builder (TContainer fields) | `0x0050e360` [C] |
| +0xe4 | `exitflags` | Pulse, Load/Save, `setfromexit` [C] |
| +0xe8 | `wait` (only TLever uses it) | TLever::Pulse [C] |
| +0x104 | TLever `usedir` = 3 (+0xf8 `targetpos`, unused) | `0x0050e430` [C] |

The builder sets `OF_IMMOBILE | OF_PULSE` (`flags |= 0x8001`) [C].

Stats (registrations `0x0050c6a0..0x0050c850`; ECX of each names its
`SStatEntry`; class.def `CLASS "EXIT"` declares the same) [C]:

| Stat | 4CC | Kind | id | default / min / max | SStatEntry | Get / Set slot | New in retail |
|---|---|---|---|---|---|---|---|
| Openable | OPEN | class | 0 | 0 / 0 / 1 | `0x0066d238` | 0x1f0 / 0x1f4 | |
| Facing | FACE | class | 1 | 0 / 0 / 255 | `0x0066d1c0` | 0x254 / 0x258 | |
| UseCenter | USE | class | 2 | 0 / 0 / 2 | `0x0066d1c8` | 0x25c / 0x260 | |
| StopMoving | STMV | class | 3 | 0 / 1 / 1 | `0x0066d1bc` | 0x264 / 0x268 | |
| Delay | DLY | class | 4 | 0 / 0 / 1000 | `0x0066d1e8` | 0x26c / 0x270 | |
| TileFlags | TFLG | class | 5 | 0 / 1 / 32 | `0x0066d248` | 0x1f8 / 0x1fc | yes |
| Locked | LOCK | object | 0 | 0 / 0 / 1 | `0x0066d22c` | 0x208 / 0x20c | |
| KeyId | KEY | object | 1 | 0 / 0 / 100000 | `0x0066d1d8` | 0x210 / 0x214 | |
| PickDifficulty | PICK | object | 2 | 0 / 0 / 100000 | `0x0066d1b8` | 0x218 / 0x21c | |
| AutoActivate | AACT | object | 3 | 0 / 0 / 1 | `0x0066d1cc` | 0x274 / 0x278 | yes |

`StopMoving` is read by nothing [C, no caller in the exit code]; `Delay`
only by TLever. `TileFlags` (also a stat of CONTAINER, TILE and
INVCONTAINER) has no reader in the exit code; its consumer is not
identified [U].

### 1.3 The exit list (`exit.def`)

`SExitRef` (0x24 bytes, singly linked from `exitlist` `0x0066d1c4`,
dirty flag `0x0066d24c`) [C]:

| Offset | Field |
|---|---|
| +0x00 | `name` (strdup) |
| +0x04 | `target` x, y, z |
| +0x10 | `level` |
| +0x14 | `mapindex` (written, never read by retail) |
| +0x18 | `ambient` (written, never read) |
| +0x1c..+0x1e | ambient colour bytes: blue, green, red (parsed with `%d` into byte addresses in reverse order, so each write's high bytes are overwritten by the next; `next` is set after parsing) |
| +0x20 | `next` |

- **`ReadExitList(reload)`** `0x0050c8f0` [C]: path = `<ClassDefPath>exit.def`
  (`"%s%s"`, `0x0065bd48`); if a module is active and
  `<modules path><module dirname>\exit.def` exists (`0x004a1c00`), that
  file instead. Opens it through the resource layer (`0x004a13f0`, "rb").
  One entry per line, `Parse` format at `0x005e171c`:
  `%t (%d, %d, %d) level %d mapindex %d ambient %d (%d, %d, %d)` (the
  shipped file writes `LEVEL`/`MAPINDEX`; matching is case-insensitive
  [I]). Entries are pushed at the head, so the list is in reverse file
  order. With `reload`, a name already in the list is skipped. Clears the
  dirty flag. Any parse error aborts the read (returns 0).
- **`WriteExitList`** `0x0050cca0` [C]: only if dirty; re-reads with
  `reload` (merging entries added by others), writes the module's
  `exit.def` if that file exists on disk (`0x00481260`) else the
  ClassDefPath one, format
  `0x005e1788` with `mapindex 0x%x`.
- **`AddExit(name, inst, getamb)`** `0x0050cfc0` [C] (editor `exit`
  command): find-or-add `name`; if `inst` is an EXIT, `target` = centre of
  its strip (from `GetExitStrip`) + its position, `mapindex` = its map
  index, ambient = the map's ambient light and colour (or -1/white with
  `noambient`); otherwise `target` = its position, `mapindex` = -1.
  `level` = the map pane's level (`0x00666970`). Marks dirty.
- **`Initialize`** `0x0050c880` (clear dirty, `ReadExitList(0)`) is called
  from the map pane's initialize `0x0044d5c0`; **`Close`** `0x0050c8a0`
  (`WriteExitList`, free) from its close `0x0044d9c0` — both inside the
  PlayScreen's lifetime, after the module is mounted [C callers; I that
  these are TMapPane::Initialize/Close, as in 1998 `mappane.cpp:365`].
- **Lookup**: Activate walks the list comparing `stricmp(ref->name,
  this->name)` (`0x0059a530`, the CRT `_stricmp` [I]) — the **instance**
  name, not the type name. [C]

Shipped `Ahkuilon/exit.def` [C]: 155 entries (sections `//Demo`,
`//Town`, `//Ancient Tower`, `//Dungeons`, `//Dungeon Teleports`,
`//Keep`, `//Labyrinth`, `//multiplayer`). 94 entries match a named EXIT
object in the module's base map: Door1 34, DunTeleport 24, PortNS 9,
PortEW 8, LabGateE 7, LabGateS 7, Door2 4, Elevator 1. `resources.rvr`
carries an older copy (2 coordinates differ: `Lv41D10`, `Lv49bTel2`).

### 1.4 The exit strip

`GetExitStrip` `0x0050ce70` [C] is the 1998 function unchanged: the
object's facing bound box (`0x00470f50`), height from the imagery's world
bound box (min 1). `UseCenter` 2 (rotating walls): `regx = regx/4*3`,
`regy = (regy+1)/2`, `w = w/4*3`, `l = (l+1)/2`; `UseCenter` 1
(elevators, teleporters, press plates): halve all four. Otherwise
`Facing()` (or the facing byte when negative) picks a one-cell strip:
`[0x20,0x60)` east `regx += 1-w, w = 1`; `[0x60,0xa0)` south
`regy += 1-l, l = 1`; `[0xa0,0xe0)` west `w = 1`; else north `l = 1`.

A position is on the strip when `gx = (regx*16 - x + px) / 16` and
`gy = (regy*16 - y + py) / 16` (C division) satisfy `0 <= gx < w`,
`0 <= gy < l`; z is not tested (the imagery's `GetWorldRegZ` is called and
discarded, as in 1998). [C]

### 1.5 Exit states

`SetExitState(es)` `0x0050d530` [C] maps a state number to a state name,
then `SetState` (falls back to `es` itself when no name is found, and
passes any other number through):

| es | State name tried | Fallback |
|---|---|---|
| 0 | `openingout` | `closed to open` |
| 1 | `open` | — |
| 2 | `closingout` | `open to closed` |
| 3 | `closed` | — |
| 4 | `openingin` | `closed to open` |
| 5 | `closingin` | `open to closed` |

The numbering is `Door1.I3D`'s own state order: its header lists
`openingout, open, closingout, closed, openingin, closingin` [C, strings
in `imagery.rvi:Imagery/Misc/door1.i3d`; I that this is the index order].
`KeepDoor`/`Portcullis` list `opening, open, closing, closed`; `Lever`,
`UpBlock`, `WallBlock` list `closed, open, closing, opening` [C strings,
order U].

Auto-step (start of `Pulse`) [C]: when `CommandDone()`, `Openable()`, the
object has an animator with imagery, and the current state's animation is
longer than one frame (`GetAniLength`, imagery slot 0x90), its animation
name (`GetAniName`, slot 0x88) is compared case-insensitively: `CLOSING`
→ `SetExitState(3)`, `OPENING` → `SetExitState(1)`. So the step is driven
by the *animation's* name in the imagery header, not by the state number.
Whether Door1's states carry those animation names is not checked [U];
the shipped door scripts set `CLOSED` themselves.

### 1.6 Pulse: the player crossing the strip

`TExit::Pulse` `0x0050d640` [C]:

```text
TObjectInstance::Pulse()                      // 0x004708e0: animator pulse, script Continue
if !Editor:                                   // 0x00668154
    auto-step (1.5)
    if GetImagery() && (IsServer || !Multiplayer):          // 0x0067682c / 0x0066829c
        GetExitStrip(regx, regy, regz, w, l, h)
        exitflags &= ~5                       // bits 0 and 2 are per-tick
        for i in 0 .. PlayerManager.NumPlayers():           // 0x0065a890, 0x0051ee70 / 0x0051eea0
            p = PlayerManager.GetPlayer(i); skip null
            skip unless p is on the strip (1.4)
            skip if type name is exactly "Door1", "Door2", "PortEW" or "PortNS"   // strcmp, case-sensitive
            exitflags |= 1
            if p.flags & OF_ONEXIT (0x100000): exitflags |= 4
            else: this->Activate(p, 0)        // vtable 0x248
            p->SetOnExit()                    // 0x004cdf30: OF_ONEXIT, exittimestamp (+0x114) = FrameCount
            p->onexit (+0xe4) = this
if !GetAnimator(): SetCommandDone(true)
```

- Only **players** are tested (`0x0065a890` is `PlayerManager`: `TPlayer`'s
  destructor removes itself from it, `0x00518570`). In single player that
  is Locke alone. [C]
- **Anti-bounce** is the player's `OF_ONEXIT`. `TCharacter::Pulse`
  `0x004c1bb0` (TPlayer's Pulse `0x00518aa0` calls it at `0x00518aa9`)
  clears it once `FrameCount − exittimestamp > 5`
  (`0x004c1c82`), and clears `onexit` after 24 frames (`0x004c1ca9`) [C].
  A player standing on any strip keeps the flag (every pulse refreshes
  it), so arriving on the destination strip does not fire it; the player
  has to be off every strip for 6 frames. Nothing in retail reads
  `onexit` that this survey found [U].
- `exitflags`: bit 0 "a player is on the strip this tick", bit 2 "that
  player arrived with OF_ONEXIT". Retail TExit never reads either; bit 1
  is only TLever's. [C]
- The 1998 `Delay` wait, `EX_ACTIVATED` latch and `Unactivate` on leaving
  the strip are gone from TExit (TLever keeps them, 2.3). [C]

### 1.7 Activate

`TExit::Activate(user, flag)` `0x0050d3a0` [C, disassembly in the decomp
file]:

```text
if Multiplayer && !IsServer: return 0
if !user: user = Player                                    // 0x00667fcc
if Locked(): return 0                                      // slot 0x208
if script && !flag:                                        // +0x84
    script->Trigger(ACTIVATE (6), null, null, user, "user", null, null)   // 0x00492640, "user" 0x005e17d8
    return 1
if !GetObjStat(AutoActivate) && !flag: return 0            // slot 0xdc, 0x0066d1cc
ref = exit-list entry whose name == this->name (stricmp)
if !ref: return script ? 1 : 0
target = ref->target
if user == (MapPane.centeron.flags & CENTERON_OBJ ? centeron.obj : null):   // 0x006669b0 / 0x006669b4
    MapPane.centeron.flags |= 8                            // camera snaps (3.2)
((TCharacter*)user)->SetTarget(null)                       // 0x004d4790: drop the combat target
user->SetPos(target, ref->level, false)                    // slot 0x8
if Multiplayer && IsServer: NetSendTeleport(user, &target, ref->level)   // 0x00586bb0
return 1
```

- `flag` = "forced": the `activate` command passes 1, Pulse and `follow`
  pass 0. Forced skips the script trigger *and* the AutoActivate gate.
- A scripted exit, unforced, only asks its script for ACTIVATE (with the
  user as alias `user`) and returns. SCRIPT_ENGINE §3: ACTIVATE fires
  when requested; if the prototype has no ACTIVATE block nothing runs.
- An unscripted exit, unforced, needs `AutoActivate` = 1 (class.def
  default per type, §2.6; a placed object may override it).
- Placement is exactly `ref->target`: no facing push (the 1998 "Door"
  hack is gone) and no z correction. `AddExit` made targets the centre of
  the destination exit's strip at that object's z. [C]

**What scripts do with ACTIVATE** (shipped): only the 15 town teleport
stones (`TownTel0..14`, type `TwnTeleStone`, `master.s`) have ACTIVATE
blocks. Each: `NOWAIT CONTROL OFF`, `player.STOP`, set `TOWNTELSTATE`,
`FADESCREENOUT`, `WAIT SCREENFADE`, `FADESCREENIN`,
`player.POS x y z level`, `TOWNTELn.SETFROMEXIT` on the destination stone,
`CONTROL ON`; an ALWAYS block resets `TOWNTELACTIVESTATE` every 10
frames. Their exit-list names are unused (the script teleports). [C]

### 1.8 The other methods

- **`Use(user, with)`** `0x0050d1a0` [C]:
  ```text
  if !Openable(): return 0
  item = MapPane.GetInstance(with)                         // 0x00452690
  if !CheckKeyUse(user, item) && Locked():                 // TContainer 0x004dd480
      if user == Player: TextBar.Print(GetString("DOORLOCKED"))   // "It seems to be locked"
      return 0
  TObjectInstance::Use(user, with)                         // 0x004705f0: USE triggers
  return 1
  ```
  It never changes the door's state. `CheckKeyUse` returns 1 for any key
  or lockpick attempt on a locked exit (success unlocks it; failure prints
  CONTWRONGKEY / CONTPICKFAIL / CONTPICKTOUGH), and Use then still runs the
  USE trigger. `TObjectInstance::Use` requests USE `<name>`/`<type>` on the
  exit's script with the user as `user`, and USE `<name>` on the user's
  script with the exit as `item` (SCRIPT_ENGINE §7).
- **`Operate(user)`** `0x0050d230` (the `operate` command) [C]: state 1
  or 0 (open/opening) → `SetExitState(IsOutside(user) ? 2 : 5)`; state 3
  or 2 (closed/closing) → `SetExitState(IsOutside(user) ? 0 : 4)`; states
  4/5: nothing. So a door swings away from whoever operates it.
- **`IsOutside(obj)`** `0x0050d2b0` (also the script member `isoutside`,
  caller `0x0041f51b`) [C]: 0 for null. `a` = the point 10 units along
  `Facing() + facing byte`, `b` = 10 units along that `+ 0x7f`;
  `d = obj.pos − pos`; returns 1 when `Dist2D(a, d) > Dist2D(b, d)` (the
  approximate distance `0x0046de60`) — `obj` is behind the exit's facing.
- **`CursorType(with)`** `0x0050d370` [C]: `Openable()` and not
  `OF_INVISIBLE` → `with ? CURSOR_HAND (0) : CURSOR_DOOR (3)`; else
  `CURSOR_NONE` (−1).
- **`Unactivate()`** `0x0050d510` [C]: `if Openable(): SetExitState(2)`.
  Only TLever's Pulse calls slot 0x24c on an exit.
- **`Load`/`Save`** `0x0050d990`/`0x0050d9c0` [C]: TContainer's, then int32
  `exitflags` (SAVE_GAME §11.3; already ported).

### 1.9 The door scripts (`master.s`)

Prototypes match by instance name, then type name (SCRIPT_ENGINE §6), so
these drive every placed object of the type that has no script of its
own. All start `CONTROL OFF`, `PLAYER.STOP` (most also
`player.COMBAT OFF`) and pick a side with `IF THIS.ISOUTSIDE player = 1`.
[C, `resources.rvr:master.s` lines 565–920]

| Prototype | Opening | Then |
|---|---|---|
| `DOOR1`, `DOOR2` | `GOTORELATIVEDISTANCE` to a side spot (`42 −32` / `−25 18`); if `ISATRELATIVEDISTANCE` there: `FACEOBJECT THIS`, `OPERATE player`, `TRY "WOPENDOOROUT"`/`"WOPENDOORIN"` (the `WCLOSE…` variants when `THIS.STATE = 1`; DOOR1 uses `NOWAIT TRY`) | `WAIT 24`, `FADESCREENOUT`, `WAIT SCREENFADE`, `CONTROL ON`, `FADESCREENIN`, `ACTIVATE`, `THIS.STATE "CLOSED"` ×2 |
| `CAVDOOR1`, `CAVDOOR2` | `GOTORELATIVEDISTANCE`, `FACE`, `NOWAIT OPERATE player`, `NOWAIT TRY "WOPENDOOR…"` | same tail as DOOR1 |
| `PORTEW`, `PORTNS` | `GOTORELATIVEDISTANCE`, `FACE`, `NOWAIT STATE "OPENING"`, `NOWAIT TRY "WOPENPORT"` | same tail |
| `InDoor1` | `GOTORELATIVEDISTANCE`, `FACEOBJECT`, `OPERATE player`, `NOWAIT TRY "WOPENDOOR…"`, `WAIT 24`, walk through (`GOTORELATIVEDISTANCE` to the far side), `WAIT 24`, `CONTROL ON` | `THIS.STATE "CLOSINGOUT"`, `"CLOSED"` ×2 — no teleport |
| `INPORTEW`, `INPORTNS` | `STATE "OPENING"`, `TRY "WOPENPORT"`, `WAIT 48`, walk through | `CONTROL ON`, `STATE "CLOSING"`, `"CLOSED"` ×2 — no teleport |

`FADESCREENIN` runs before `ACTIVATE`, so the teleport happens as the
fade-in starts and the player sees the destination fade in.

## 2. The subclasses

### 2.1 TPressPlate (`PressPlate`, vtable `0x5b2258`)

[C] `Use` → 0 (`0x0050df30`), `CursorType` → −1 (`0x0050df40`),
`Unactivate` → `SetState(0)` (`0x0050da50`). Its 1998 `Activate()`
became `0x0050da20` = `TExit::Activate(Player, arg); SetState(1);
return 1` — but it sits in a **new slot 0x280** (the 1998 signature no
longer matches TExit's), and no code calls slot 0x280 [C: no indirect call
through 0x280 in the binary]. Pulse is TExit's, which never calls
Unactivate. So a press plate behaves as a plain auto-activating exit
(`UseCenter` 1, `AutoActivate` 1) and the engine never moves it down or up.
The base map has two named ones. Dungeon press-plate traps use
TTrapPressPlate (2.5).

### 2.2 TUpBlock (`UpBlock`, vtable `0x5b24e0`; types `UpBlock` ×2: `UpBlock.I3D`, `WallBlock.I3D`)

[C] `Use(user, with)` `0x0050dae0`: if `TObjectInstance::Use` handled it
(a script USE ran) → 1; else only with `with == −1`: state 2 or 3 →
`SetState(0)`, state 0 or 1 → `SetState(2)`, play `grind rock`, → 1.
`Pulse` `0x0050da80`: unless Editor, when `CommandDone()`: state 2 or 5 →
`SetState(3)`; state 0 or 4 → `SetState(1)`; then TExit::Pulse.
`CursorType` → −1. (Raw state numbers, assuming the 0..3 order of 1.5;
the imagery's actual order is unchecked [U].) None placed in the base map
by name.

### 2.3 TLever (`LEVER`, vtable `0x5b2a4c`; type `Lever`)

[C] `Use` `0x0050dc20` = `TExit::Use(user, with)`, returns 1.
`Operate(user)` `0x0050de60`: state not 1 and not 3 → `SetState(3)`, else
`SetState(2)` (raw states; user ignored). `Pulse` `0x0050dc40` keeps the
1998 TExit model, for the main player only:

```text
TObjectInstance::Pulse()
auto-step as TExit but without the length > 1 test, and "CLOSING" -> SetExitState(0) (TExit uses 3); "OPENING" -> SetExitState(1)
if Player && !Editor && GetImagery():
    if Player not on the strip:
        if exitflags & 2: Unactivate()            // slot 0x24c -> TExit: SetExitState(2) if Openable
        exitflags &= ~7
    else:
        if Player.flags & OF_ONEXIT && !(exitflags & 1): exitflags |= 4
        Player->SetOnExit(); exitflags |= 1
        if !(exitflags & 2) && wait++ > Delay():  // +0xe8
            wait = 0
            if Activate(null, 0): exitflags |= 2
```

The base map has nine named levers: `Lv31Lever1..5` (`cave.s`),
`Lv45L5`, `Lv48LEV6`, `lv48lev88` (`dungeon.s`) and `Lv45L11` (no
script). The eight scripted ones have USE blocks: walk up, `OPERATE player`, `player.TRY "WPUSHLEVER"`,
`ACTIVATE`, then script-specific fades and teleports. Their names are not
in `exit.def`, so the `ACTIVATE` there teleports nobody (returns 1 because
they have scripts).

### 2.4 TDragonEntAnimator (`DragonEnt`, animator vtable `0x5b2764`)

[C] As 1998: Animate and Render pass through to T3DAnimator
(`0x0050dbe0`, `0x0050dbf0`); `GetExitStrip` (animator slot 0x60,
`0x0050dbb0`) returns regx 2, regy 2, w 4, l 4, but TExit::GetExitStrip
never consults the animator. The `DragonEnt` type's imagery is an `.I2D`,
so the 3D animator is likely never created [I].

### 2.5 TTrapLever / TTrapPressPlate (CLASS TRAP, vtables `0x5b70bc` / `0x5b72f8`)

Retail-only devices that trigger traps; not exits, but their slot 0x110 is
the "delayed use" SCRIPT_ENGINE §7 left open. TRAP stat slots (from the
TRAP registrations `0x00524252..0x005243a9`): 0x200 Facing, 0x208 Usable,
0x210 IsDevice, 0x218 ActRange, 0x220 Timer, 0x228 SetOff, 0x230 Enabled
(getters; setters +4). [C]

- **TTrapLever** (type `TrapLever`, `Lever.I3D`). `Use` `0x00524da0`: if
  `TObjectInstance::Use` handled it → 1; else, with a user: remember it
  (`+0xe0`), play `lever`, set pending (`+0xd8`), return 0. `Pulse`
  `0x00524e50`: when pending, the user is not walking (`doing->action !=
  ACTION_MOVE`), `!SetOff()` and `Enabled()`: state 1 or 3 →
  `SetState(2)`, else `SetState(3)`; then, if it has a script, request
  USE `<name>`/`<type>` with no user and run the script now
  (`0x00471260`); otherwise `Use(-1)` the nearest TRAP-class object that
  is not a device (`IsDevice() == 0`) and is `Enabled()`. Clear pending.
  [C]
- **TTrapPressPlate** (type `TrapPressPlate`, `PressPlate.I2D`). Slot 0x1f4
  `0x00525030`: track the nearest living character not in an inventory;
  when it stands on the plate's world bound box, set pending (first time)
  and occupied (`+0xf8`), return 1; when it steps off, `SetState(0)`.
  `Use` `0x00524fe0`: unhandled by a script → pending. `Pulse`
  `0x005251e0`: if `Enabled() && !SetOff()` run slot 0x1f4; when pending,
  the character not walking, and state 0: `SetState(1)`, then the same
  script-USE-or-nearest-trap rule as the lever, then play `pressplate`
  (quieter with distance). [C]

### 2.6 EXIT types (class.def) and what walking onto them does

Class stats `{Openable, Facing, UseCenter, StopMoving, Delay, TileFlags}`,
object stats `{Locked, KeyId, PickDifficulty, AutoActivate}` (class
defaults; a placed object can override object stats). "Walk-on" is
TExit::Pulse's behaviour for an unforced `Activate(player, 0)`.

| Type | class / obj stats | Class | Walk onto strip | `master.s` USE |
|---|---|---|---|---|
| Door1 | 1,0,0,0,0,48 / 0,0,0,0 | TExit | nothing (excluded) | DOOR1 |
| InDoor1 | 1,0,0,0,0,48 / 0,0,0,0 | TExit | script ACTIVATE request only | InDoor1 |
| Door2 | 1,0,0,0,0,48 / 0,0,0,0 | TExit | nothing (excluded) | DOOR2 |
| JongDoor, KeepDoor | 1,0,0,0,0,48 / 0,0,0,0 | TExit | needs a script | — |
| Door (DunDoor) | 1,0,0,0,0,0 / 0,0,0,0 | TExit | needs a script | — |
| PortEW, PortNS | 1,0/64,0,0,0,48 / 0,0,0,1 | TExit | nothing (excluded) | PORTEW, PORTNS |
| InportEW, InportNS | 1,0/190,0,0,0,48 / 0,0,0,1 | TExit | script ACTIVATE request only | INPORTEW, INPORTNS |
| CavDoor1, CavDoor2 | −1,−1/64,0,0,0,50 / 0,0,0,1 | TExit | script ACTIVATE request only | CAVDOOR1, CAVDOOR2 |
| DunTeleport, TwnTeleStone | 0,0,1,0,0,2 / 0,0,0,1 | TExit | teleport (scripted stones: ACTIVATE) | — |
| Elevator, IrisDoor | 0,0,1,0,0,0 / 0,0,0,1 | TExit | teleport if listed | — |
| RotateWall | 0,0,2,0,0,0 / 0,0,0,1 | TExit | teleport if listed | — |
| LabGateE/S, ForAcave1/2, DunStr{Dwn,Up}{E,S}, CavDoor01..06, CryptDoor, Grate, SWGrate, RunACofre, DragonEnt | various / 0,0,0,1 | TExit | teleport if listed | — |
| PressPlate | 0,0,1,0,0,2 / 0,0,0,1 | TPressPlate | teleport if listed | — |
| UpBlock (×2) | 0,0,0,0,0,2 / 0,0,0,1 | TUpBlock | teleport if listed | — |
| Lever | 1,0,0,0,0,0 / 0,0,0,1 | TLever | its own Pulse (2.3) | — |
| TwnSStairUE/US, Bookcase3D, KeyHole, Rock Pile | −1,… / 0,0,0,0 | TExit | needs a script | — |
| ForStyxxTomb, CavPrisonPool | −1,… / 1,8000 or 321,0,0 | TExit | locked | — |

`Openable` −1 in class.def: whether the loader clamps it to 0..1 is not
checked [U]; `CAVDOOR1`'s USE block only runs if `Openable()` is non-zero,
so it is presumably kept non-zero [I].

## 3. Moving the player

### 3.1 One teleport, two callers

`TExit::Activate` and the `pos` command (`0x00423d40`) do the same thing
[C]: queue the object's screen rect (`0x00454920`, `pos` only; role [I]),
`SetTarget(null)` on a
character or player (`0x004d4790` clears `doing/root/desired->target`),
set `centeron` flag 8 if the camera follows that object (`pos` also
requires not-Editor), `SetPos(pos, level, override)`, queue the rect again
(`pos`), send the teleport to clients when hosting. `pos` with no
arguments goes to the camera's centre and level. 95 shipped `pos` lines
give a level (§4).

### 3.2 Same level or another level

`TObjectInstance::SetPos` `0x0046ed70` → `TMapPane::CheckPos`
`0x00459f50` [C]:

- Only an `OF_NONMAP` object (a player) can change level; others keep
  theirs. x, y are clamped to 0..0x8000; sectors are 1024 units.
- Same sector → nothing. Another sector that is **loaded** (the global
  loaded-sector list, `0x00499e10(level, sx, sy)`) → remove from the old
  sector, add to the new one (under the sector mutex when the loader
  thread is on).
- A player whose destination sector is **not loaded** (another level, or
  far on the same level) is taken out of the map: its shadow removed, its
  animator freed, removed from its sector; `SetPos` still stores the new
  position and level. Non-players are clamped to their current sector.

Then, every frame, the map pane update (`0x00454390`) runs
**`UpdateMapPos`** `0x004539d0` and the **sector update** `0x00459220`
[C]:

1. *Camera.* The camera centre follows `centeron` (object or point). It
   jumps instead of smooth-scrolling when the level differs, the distance
   is ≥ ~1024, `CENTERON_SCROLL` is off, or **flag 8** is set; flag 8 is
   cleared every frame. The map's `newlevel` becomes the followed object's
   level; a level change calls `RedrawAll` (`0x004546a0`).
2. *Sector update* `0x00459220`: shifts centre/level; if the level changed
   or the centre moved > ~1024, sets `CurrentScreen + 0x6c = 1`
   (meaning [U]); then `0x00459490` marks the sectors around the camera
   **and around every active player on that player's own level** as in
   use and releases the rest (`0x00498460`: each released sector is
   written by `0x00498a40` under the path at `0x0065c02c` — `curmap` [I],
   SAVE_GAME §2 — and freed); `0x004597b0` loads what is missing. It runs
   when the sector window moved, the level changed or a reload is forced
   (`+0x50`), and is postponed while the player is aiming a bow at a
   target (desired action 0x19) or `0x0045f770(3)` holds [U]. If the
   `PRELOADSIZE` square around the centre isn't loaded,
   the text bar shows **`LOADMAPMSG`** ("Loading Map... Please Wait") and is
   presented, the sectors load synchronously (`0x004997d0` →
   `0x004998b0`) with a progress callback (`0x00459a00`) drawing a bar in
   the text bar (`progress × 180 / 1000`) and presenting each sector, then
   the message is cleared and `RedrawAll`; `0x00459b80` puts every active
   player (player state `+0x36c` bit 0) that has no sector back into the
   map (`TMapPane::AddObject` `0x00451090`: into the loaded sector at its
   level and position) and takes inactive ones out.
3. `PRELOADSIZE=` on the command line sets the square's size; otherwise 3
   or 5 sectors from available memory (`0x00483f0f`, `0x00485950`);
   `NOPRELOADSECTORS` turns the cache off. [C]

There is **no loading screen**: the loading bar (`0x00448680`) is drawn
only by engine init (`0x00485870`) and `TPlayScreen::Initialize` [C,
callers]. Any fade is the script's (`FADESCREENOUT`/`FADESCREENIN` in the
door prototypes; note `FADESCREENIN` comes *before* `ACTIVATE`, so the
fade-in reveals the destination).

### 3.3 Areas

`TAreaMgr::Pulse` `0x0041c410` [C] tests areas against the **map pane's
centre and level** (`0x0066697c`, `0x00666970`), not the player: the last
area in `area.def` order that contains them wins (area 0 is the
fallback). If it isn't already active, every active area is left (inline
`TArea::Exit`: ambient sound off, music fade-out, `TScriptManager::Clear`
of the area's scripts in single player) and the new one is entered
(`TArea::Enter` `0x0041ba00`: ambient light/colour — snapped when the
level changed or the camera moved ≥ 0x401, else faded over 72 frames —
music, ambient sounds, `TScriptManager::Load(<area script>)` in single
player, then "`<player> entered <area>`" via BASEENTERED/FULLBASEENTERED
strings). Entering loads the scripts; SCRIPT_ENGINE §6 covers how loaded
objects pick them up (`N_SCRIPTADDED`). The Keep is two areas with the
same script (`area.def`: "The Keep" on level 2 and on level 6), so going
2 → 6 drops and reloads `keep.s`.

### 3.4 What is saved around a level change

Nothing explicit. Sectors that leave the in-use set are written to
`curmap` as they are released (3.2 step 2; SAVE_GAME §2: sectors are read
from `curmap` if present and written there when they unload). No save
file is touched. [C]

### 3.5 Where this goes in the port's session design

ARCHITECTURE §3: `TGameSession` owns the world and level entry is a
session step; `TMapManager` loads whole levels (cached) and owns sectors;
presenters follow `CurrentMapChanged`. Retail's level change maps onto
that as follows:

| Retail | Port owner |
|---|---|
| exit list loaded/written with the map pane (`0x0044d5c0` / `0x0044d9c0`) | session `Start`/`End` (after the module mount), as world data |
| teleport (`Activate`, `pos`): SetTarget, camera flag 8, SetPos | one function shared by both (e.g. `TCharacter::Teleport`), not two copies |
| `CheckPos` takes a player out of the map when its sector isn't loaded | same (the window/map lookup), then the session puts it back |
| sector update: load the camera's level, re-add players (`0x00459220`) | **the session's level-entry step**: when the camera's level differs from `MapManager.CurrentLevel()`, `SetCurrentLevel(level)` and put every player with no sector into the sector under it — the body `EnterWorld` already has, run at the start of the next tick, before the simulation |
| `LOADMAPMSG` + bar in the text bar during the load | `TPlayScreen` draws it from the session's progress while a level loads (staged across frames like the start load, §3.4 of ARCHITECTURE); the simulation doesn't tick meanwhile, as retail's synchronous load stalled it |
| releasing sectors saves them to `curmap` | the port keeps visited levels loaded (`TMapManager` cache) and writes them at `FlushSectors`/`SaveCurMap`; the save is what interop needs |
| area exit/enter from the camera | unchanged: `AreaManager.Pulse` already reads the map pane's position and level |

Divergence to record in ARCHITECTURE §7: the port loads a whole level and
keeps it, so the load message appears only on the first visit (retail
showed it whenever its 3–5 sector window moved onto unloaded sectors).

## 4. Commands

Handlers take `(context, token, caller, script)`; "cc" is the required
context class (10 = EXIT). Usage counts are lines in the shipped scripts:
the nine module scripts in `Ahkuilon.rvm` and `master.s` (resources).
[C]

| Command | Handler | cc / editor | Grammar | Effect | Uses (module / master.s) |
|---|---|---|---|---|---|
| `activate` | `0x00420100` | EXIT / no | `<exit>.activate` | user = the running script's `user` if it is a PLAYER, else `Player`; `Activate(user, 1)` (forced); always 0 | 8 / 6 |
| `operate` | `0x00426cd0` | EXIT / no, needs a param | `<exit>.operate <object>` | resolve `<object>` (`0x0041e690`, so `player`, `user`, names); `Operate(obj)` (slot 0x250); host: `0x00586750`. 4 (bad params) without a context or a name token | 8 / 14 |
| `setfromexit` | `0x00428a40` | EXIT / no | `<exit>.setfromexit` | `exitflags &= 4` (keeps only bit 2). No effect on a TExit (Pulse rewrites the bits every tick, nothing reads them); on a TLever it clears "activated" | 0 / 28 |
| `follow` | `0x004230e0` | EXIT / yes | `<exit>.follow` | `Activate(null, 0)`; on 0 prints "Nothing defined for this exit, can't follow" | 0 / 0 |
| `exit` | `0x00423040` | EXIT / yes | `<object>.exit <name> [noambient]` (usage text says `[posonly]`; the code tests `noambient`) | `AddExit(name, obj, !noambient)`, prints "Exit from '%s' added.", `WriteExitList` | 0 / 0 |
| `save exits` | `0x00426360` | — / yes | `save exits` | `WriteExitList` (`0x00426574`) | 0 / 0 |
| `pos` | `0x00423d40` | any / no | `<obj>.pos [add] <x> <y> [<z> [<level>]]`, or no args = camera centre and level | the teleport (3.1) | 95 lines with a level, 42 without (module + master.s) |
| `level` | `0x00422d70` | — / no | `level <n>` | characters: `SetTarget(null)`; map `newlevel = n`; `RedrawAll` if it differs | 0 / 0 |
| `state` | `0x004227b0` | any / no | `<obj>.state <name>\|<n>` | `SetState(FindState(name))` — raw, not `SetExitState`; returns `CMD_WAIT` (1) on success | (RESSEXIT: 3) |
| `stat` | `0x00424010` | any / no | `<obj>.stat locked=1` … | sets `Locked` | (`ressexit`: 2) |
| `use` | `0x00420050` | any / no | `<obj>.use [<with>]` | `Use` | — |
| `isoutside` (expression member) | `0x0041f230` → `0x0050d2b0` | — | `IF THIS.ISOUTSIDE player = 1` | IsOutside | 0 / 9 |

ACTIVATE as a trigger keyword: 15 blocks (`master.s`, the TownTel
stones). `centeron`/`scrollto`/`map` with a level move the camera to
another level and so, through the sector update, load it (3.2) [I] —
exits don't use them.

## 5. The opening's exits

The Keep is level 2 (and 6). Its EXIT objects in the base map [C, sector
scan]:

| Object | Type | Sector | Behaviour |
|---|---|---|---|
| `ressexit` | Door1 | 2_1_1 (the resurrection chamber) | DOOR1 USE; exit list → (9992, 8966, 576) level 2, beside `ressenter` |
| `ressenter` | Door2 | 2_9_8 | DOOR2 USE; → (1218, 1044, 17) level 2, back into the chamber |
| `jailenter` | Door1 | 2_12_11 | → (6280, 4338, 33) level 2 |
| `jailexit` | Door2 | 2_6_4 (also one on level 6, 6_6_4) | → (12810, 11392, 161) level 2 |
| `KeepExit` | KeepDoor | 2_11_12 | `keep.s` USE: `GOTORELATIVEPOSITION`, state OPENING, fade, `player.POS 1842 24388 451 0` (level 0); locked until `TENDRICKSTATE > 1` (ALWAYS block) |
| `towntel0` | TwnTeleStone | 2_12_10 | `master.s` TownTel0 ACTIVATE |
| (unnamed) | Elevator | — | no name, so no exit-list entry: walking on it does nothing |

**During the scene** (`SardokR`'s CUBE block, `keep.s`): `RESSEXIT.STATE
OPENINGOUT` (Rahul enters), `RESSEXIT.STATE CLOSINGOUT`, `RESSEXIT.STATE
CLOSED`, then `ressexit.STAT locked=1`. `state` sets the named states
directly (Door1 has them). `TendrickR`'s ALWAYS block unlocks it
(`ressexit.stat locked=0`) once Rahul's health is 0.

**After the scene, Locke and a Keep door** [C, from the code above]:

- Walking into `ressexit` (or any Door1/Door2): nothing — the types are
  excluded from strip activation (a closed door presumably also blocks the
  walkmap [I]).
- Clicking it while locked: `Use` → `CheckKeyUse` (no item) → locked →
  "It seems to be locked" in the text bar; nothing else.
- Clicking it after Rahul is dead: `Use` → USE trigger → `master.s` DOOR1
  block: `CONTROL OFF`, `player.COMBAT OFF`, `PLAYER.STOP`; on the side
  `ISOUTSIDE` gives, walk to the door (`GOTORELATIVEDISTANCE 42 −32` or
  `−25 18`), `FACEOBJECT`, `OPERATE player` (opens away from Locke), `TRY
  "WOPENDOOROUT"`/`"WOPENDOORIN"` (closing variants if the door is
  already open); `WAIT 24`; `FADESCREENOUT`; `WAIT SCREENFADE`; `CONTROL
  ON`; `FADESCREENIN`; `ACTIVATE` → `Activate(Locke, 1)` → exit-list
  `ressexit` → camera flag 8, `SetPos((9992, 8966, 576), 2)`; `THIS.STATE
  "CLOSED"` ×2. Same level, but ~9 sectors away, so the destination
  sectors load in the next map update ("Loading Map... Please Wait" if
  they aren't cached) and Locke is put back into the map there. The area
  stays "The Keep" (same level), so nothing reloads.
- `KeepExit` to the outside is a level change 2 → 0 by `pos`: the same
  path with the level load and an area change (Keep → the level-0 area,
  `keep.s` dropped).

## 6. Retail vs the 1998 source

| | 1998 (`exit.cpp`, = the port today) | Retail |
|---|---|---|
| Use | runs USE, then toggles OPEN/CLOSING itself; "It seems to be locked." literal | gate on Openable; lock check (key attempt falls through to USE); localized DOORLOCKED, player only; never toggles |
| Opening a door | Use | `operate` from a script → `Operate(user)`, direction-aware |
| States | 4 (CLOSED, OPEN, CLOSING, OPENING; names `opening`/`closing`) | 6 (opening/closing out and in; names `openingout` …), different numbers |
| Auto-step | state number CLOSING/OPENING when CommandDone | the state's animation name `CLOSING`/`OPENING` |
| Activate | `Activate()`: trigger ACTIVATE (no user), stop if FROMEXIT, list lookup, "Door" +24 push, `Player->SetPos` | `Activate(user, flag)`: MP guard, Locked guard, script → ACTIVATE with user and return, AutoActivate gate, list lookup, SetTarget(null), camera snap, `user->SetPos`, MP broadcast |
| Pulse | `TContainer::Pulse`; global Player only; Delay counter; EX_ON/ACTIVATED/FROMEXIT; Unactivate on leaving | `TObjectInstance::Pulse`; every player; Door1/Door2/PortEW/PortNS excluded; OF_ONEXIT decides; no Delay, no Unactivate; animator-less exits mark CommandDone |
| OF_ONEXIT clear | after 2 frames | after 5 frames; `onexit` pointer cleared after 24 |
| CursorType | Openable → HAND/DOOR | also requires not invisible |
| Stats | Openable, Facing, UseCenter, StopMoving, Delay; Locked, KeyId, PickDifficulty | + TileFlags (class), + AutoActivate (object) |
| Exit list path | ClassDefPath | the module's `exit.def` first |
| New script surface | — | `operate`, `setfromexit`, `isoutside`; `activate` takes the script's user |
| TPressPlate | Activate → SetState(DOWN) | same body in an uncalled slot; plates never go down |
| TLever | `Use` = TExit::Use + 1; Pulse = TExit::Pulse | `Operate` added; own 1998-style Pulse |
| TSpikeWall | impales the player on Activate | gone (TRAP class `SpikeTrapS/N` instead) |
| TElevator | `#if 0` | gone (Elevator is a plain TExit) |
| New classes | — | TTrapLever, TTrapPressPlate (CLASS TRAP) |
| Callers | `TMapPane::Initialize` → `TExit::Initialize` | same, plus `Close` from the map pane's close |

## 7. Port state (feature/gameflow, 2026-10-05)

The survey above found the 1998 exit code; this is what the port has now.

| Piece | State |
|---|---|
| `src/exit.{h,cpp}` | retail: stats (+ TileFlags, AutoActivate), 6-state `SetExitState`, `Use`, `CursorType`, `Activate(user, forced)`, `Unactivate`, `Operate`, `IsOutside`, `Pulse` (players, door-type exclusion, `exitflags`, OF_ONEXIT), TLever (`Use`, `Operate`, its Pulse), TUpBlock, TPressPlate, TDragonEntAnimator. 1998 bodies in `attic/src/exit_1998.cpp` (TSpikeWall too). |
| Exit list | read by the session's `exits` load step (155 entries from `Ahkuilon.rvm`), written/freed at `TGameSession::End` |
| `CursorType` | overrides really override (the const base was the bug) |
| Teleport | `TObjectInstance::Teleport` (drop target, camera snap, `SetPos`), used by `Activate` and `pos` |
| `CheckPos` / `SetPos` | retail: loaded = any sector of a cached level; a player bound for an unloaded level leaves the map (no delete notification); others clamp as retail; a missing level is the object's own |
| Camera | retail `UpdateMapPos` (snap flag 8, far jump, z hysteresis, grid scrolling only with control) |
| Level change | `TGameSession::EnterLevel` each tick: "Loading Map..." for a frame if uncached, `SetCurrentLevel`, players back into sectors; the pane's window stays empty while the camera's level isn't current |
| Commands | `activate`, `follow`, `operate`, `setfromexit`, `pos` (no-argument form) retail; `exit`, `level` as before |
| Members | `isoutside` |
| Scripts | type-named prototypes attach (`ObjectScript` pass 2 matched the class name): the `master.s` door prototypes reach their doors |
| Door walking | `gotorelativedistance`, `gotorelativeposition`, `faceobject`, `isatrelativedistance`, retail `Goto` and the character wait (COMMAND_SYSTEM.md §6.5); `try "QUOTED"` (the door animations) and `goto <object>` |
| `CheckKeyUse` | retail (player-only localized messages, difficulty 0 unpickable, lockpick experience) |
| Not yet | the load progress bar; TTrapLever/TTrapPressPlate (TRAP port) |

Verified (headless, `--quickstart`, `--exec`):
- `ressexit.activate` puts Locke beside `ressenter` with the camera
  snapped to him;
- `player.pos 1842 24388 451 0` (KeepExit's own move) loads level 0 (366
  sectors, 35,549 objects), enters "The Forest", attaches its scripts and
  shows Locke at the Keep gate;
- `use ressexit` while locked: "It seems to be locked"; unlocked, DOOR1's
  USE block runs to the fades, `ACTIVATE` and `STATE "CLOSED"` (its walk
  to the door waits for the movement commands);
- stepping onto `towntel0` (after 6+ frames off any strip) starts its
  ACTIVATE block; with `TOWNTELSTATE = 1` it fades, `player.POS ... 3`
  shows "Loading Map... Please Wait", loads level 3 and enters "The
  Ancient Tower". Teleported straight onto the stone from another strip,
  the player arrives "on an exit" and nothing fires -- retail's
  anti-bounce (`newgame.sav` stores Locke with OF_ONEXIT, and he starts
  on a strip in the chamber).
- Not exercised: an unscripted AutoActivate exit (stairs, DunTeleport)
  walked onto; it shares the strip test with the stone and the list
  lookup with `activate`.

**IsOutside reads past the sine table.** Retail passes the Facing stat
plus the object's facing byte (and that plus 0x7f) to `0x0046db20`
unwrapped. DistX (`0x00634d44`) is followed by DistY (`0x00634f44`), so an
angle of 256..383 takes its x from the cosine table. With facing byte 0
that happens for INPORTNS (Facing 190: the back point is 317). The port
reproduces reads inside the two tables and wraps the rest
(`RetailFacingPoint`, logged once).

## 8. Port order and design

Each step lands with a `--test` or filmstrip check; no parallel paths.

**Design decisions** (coordinator, 2026-10-05; they refine §3.5):

- **Exit list**: a session load step (`TGameSession::LoadExits`, after
  the areas, so the module is mounted) and `TExit::Close` in
  `TGameSession::End`. `TMapPane::Initialize`/`Close`, retail's callers,
  are dead in the port.
- **"Loaded"** in `CheckPos` means a sector of a map in `TMapManager`'s
  cache (`GetCached(level)->FindSector`): the port keeps whole levels, so
  any sector of a cached level is loaded, and only a move to an uncached
  level takes a player out of the map. Non-players stay clamped inside
  their sector, as retail. `CheckPos` works on sectors, not the pane's
  3×3 window (the 1998 window test is what lost far same-level
  teleports).
- **The teleport** is one function, `TObjectInstance::Teleport(pos,
  level)`: a character drops its combat target (`SetFighting(nullptr)`,
  retail `0x004d4790`'s null path), the camera snaps if it follows the
  object (`TMapPane::SnapIfFollowing`, centeron flag 8), then `SetPos`.
  `TExit::Activate` and `pos` call it.
- **The camera**: retail `UpdateMapPos` (`0x004539d0`) replaces the 1998
  body: the snap flag, the ~1024-unit jump, z hysteresis (moves under 9
  units ignored), grid scrolling only with smooth scrolling off and
  control on (`0x0065d0d0` is PlayScreen `+0x5e0`, the control flag).
- **Level entry** is a session step run at the start of each tick
  (`TGameSession::EnterLevel`, from `TPlayScreen::Update` beside
  `ProcessRequests`): when the camera's level (`MapPane.GetMapLevel()`,
  which follows the centeron target) differs from
  `MapManager.CurrentLevel()`, it makes that level current (loading it
  if not cached, after one frame showing `LOADMAPMSG` on the text bar)
  and puts every player with no sector into the one under it — the
  body `EnterWorld` uses at game start, shared. The pane's window binds
  a map only when the map is the camera's level; until the session
  switches, the window is empty.
- **Not ported** (write-only in retail): the player's `onexit` pointer
  (`+0xe4`); `exit.def`'s `mapindex` and ambient fields (read, kept,
  never applied — retail's behaviour).

1. **Prerequisites** (shared with other tracks): make `CursorType`
   override (fix the const mismatch at the base, add `override`); the
   `isatrelativedistance` member; screen fades (the door prototypes wait
   on them).
2. **Exit list lifetime**: `TExit::Initialize`/`Close` from
   `TGameSession::Start`/`End`, after the module mount (retail's map pane
   init/close). Check: `exit.def` entries load (155) on New Game.
3. **TExit data and methods**: the retail stats (TileFlags, AutoActivate),
   6-state `SetExitState`, `Operate(user)` + `IsOutside` (+ the
   `isoutside` member and the `operate` command), retail `Use`
   (Openable gate, CheckKeyUse fall-through, localized DOORLOCKED for the
   player), `CursorType`, `Unactivate`. Move the 1998 bodies that change
   meaning (TSpikeWall, the Door push) to `attic/`.
4. **The teleport primitive** (3.1): one function used by `pos` and
   `Activate`; port retail `pos` on it (no-arg form, `add`, SetTarget,
   camera flag 8 in `TMapPane::UpdateMapPos`).
5. **`Activate(user, flag)`**, `activate` (script user), `follow`,
   `setfromexit`, `exit` (`noambient`); ACTIVATE requests carry the user.
6. **Retail Pulse**: PlayerManager players, the type exclusion, exitflags
   bits, OF_ONEXIT protocol, `onexit`; `TCharacter`'s 5/24-frame clears.
7. **Level entry as a session step** (3.5): detect the camera's level
   change, load it through `TMapManager`, put players back (`EnterWorld`'s
   body, shared), `LOADMAPMSG` in the text bar while staged; fix
   `UpdateActiveWindow` to bind only the matching level. Same-level
   teleports need only the re-add.
8. **Subclasses**: TLever (Use, Operate, its Pulse), TUpBlock, TPressPlate
   (retail behaviour: the uncalled Activate kept as a non-virtual,
   commented), TDragonEntAnimator. TTrapLever/TTrapPressPlate go with the
   TRAP port (`src/trap.*`).
9. **Verification**: the opening — RESSEXIT locked message during the
   fight, the DOOR1 sequence and teleport to (9992, 8966, 576) after it;
   KeepExit to level 0 (area change, `keep.s` dropped); a town door 0 ↔ 1;
   a DunTeleport walk-on; a TownTel stone. Compare against retail in
   dosbox-x where timing matters (fades, the load message).

## 9. Corrections to earlier recon

`recon/discovered/exits_doors_notes.md`, the `cls_TExit_*` files and
`renames/agent_exits_doors.txt` predate this survey; where they disagree,
this document is right:

- Pulse's type test is inverted there: Door1/Door2/PortEW/PortNS are the
  types that **don't** activate on walk-over (`0x0050d854..0x0050d8f8`).
- Pulse iterates `PlayerManager` (`0x0065a890`), not all characters; NPCs
  never trip exits.
- Activate's early return is "has a script and not forced", not
  `EX_FROMEXIT`; slot 0xdc is `GetObjStat`, here AutoActivate
  (`0x0066d1cc` is that stat's entry, not a script global); the list is
  searched by the **instance name** (`+0x38`), not the type name;
  `0x004d4790` clears the combat target.
- `0x0050d230` is `Operate(user)` (slot 0x250, the `operate` command), not
  a script "user" handler; `0x0050d2b0` is `IsOutside` (the `isoutside`
  member).
- `0x0050da20` is not TPressPlate's Activate override: it occupies a new
  slot 0x280 that nothing calls.
- `field_map.md`: TObjectInstance `+0x38` is `name` (not `stats`) and
  `+0x80` is `commanddone` (slots 0x154/0x158), not `notifyflags`.
- SCRIPT_ENGINE §7's unidentified USE callers (vtables `0x5b70bc`,
  `0x5b72f8`) are TTrapLever and TTrapPressPlate (2.5).
- `cls_0x5a7b98_TCharacter_ResolveAttack_4c1bb0.cpp`: `0x004c1bb0` is
  TCharacter's slot 0x110 (`Pulse`) in vtable `0x5a7848`; likewise
  `0x00518aa0` (GAME_FLOW §2.5 "TPlayer::Animate") is TPlayer's slot
  0x110, its Pulse.

## 10. Questions for the author

Also in [../AUTHOR_QUESTIONS.md](../AUTHOR_QUESTIONS.md), 28–35.

1. Door1, Door2, PortEW and PortNS are excluded from walk-over activation
   by name (case-sensitive), so their master.s USE scripts own them.
   Was that the design, and was `InDoor1`/`InportEW`/`InportNS` being
   left out of the list deliberate?
2. `setfromexit` does `exitflags &= 4`, which changes nothing on a TExit;
   the 28 uses mark the destination teleport stone. Was it meant to be
   `|= 4` ("arrived from an exit"), with OF_ONEXIT doing the real work?
3. TPressPlate's 1998 `Activate` ended up in an uncalled vtable slot, so
   press plates never animate down from the engine. Known at the time?
   (The dungeon plate traps use TrapPressPlate.)
4. A key or lockpick attempt on a locked door (even a failed one) lets
   the door's USE script run — the door then swings open but `activate`
   refuses (still locked). Seen in play?
5. What does the `TileFlags` stat (EXIT, TILE, CONTAINER, INVCONTAINER)
   control?
6. TLever's auto-step sends a `CLOSING` animation to state 0
   (`openingout`), where TExit sends it to 3 (`closed`). Intended (levers
   spring back) or a slip?
7. `exit.def`'s `mapindex` and ambient fields are written by the editor
   but never read. Were they ever applied on arrival?
8. Does a Keep door ever show "Loading Map... Please Wait" in retail when
   going chamber → hall (same level, ~9 sectors apart)? A dosbox-x shot
   of that transition would pin the timing.
