# Script engine — forensics

How retail Revenant runs object scripts: prototypes and triggers, which
object a script belongs to, how a trigger starts a block, how lines run,
and how a script waits. Input to ARCHITECTURE.md §6 and
[../OPENING_SEQUENCE.md](../OPENING_SEQUENCE.md). The command interpreter
the lines go through is in [COMMAND_SYSTEM.md](COMMAND_SYSTEM.md).

Sources: Ghidra (retail, authoritative) and the 1998 source
(`/Users/benjamincooley/projects/Revenant/script.{h,cpp}`, readable
baseline). Retail fidelity: **retail-confirmed** unless marked.

## 1. Retail vs the 1998 source

The text format, prototypes, trigger keywords and the block/label
interpreter are the 1998 design. Retail changed the execution model:

| | 1998 source | Retail |
|---|---|---|
| Who waits | the character (`TCharacter::Wait/WaitResponse/WaitChar`, `waittype`) | the script (`TScript::SetWait`, wait type at `+0xb4`) — any object can wait |
| When a script runs | `TCharacter::Pulse` calls `Continue` only when the current action is done | the object's pulse calls `Continue` every tick; `Continue` returns at once while a wait is unsatisfied |
| Who triggered it | not recorded | the triggering object becomes the script's **user** (`+0xc4`, alias `user`) |
| What the script took | nothing to restore | flags at `+0x00` (control, dialog pane, camera); `End` gives them back |
| Dialog choices | `DialogPane` labels | a choice list on the script (`+0xb8`) that the response wait jumps through |

## 2. `TScript` layout (0xe8 bytes; ctor `0x00492170`)

| Offset | Field |
|---|---|
| +0x00 | taken flags: 1 control off, 2 (mp/ui), 4 dialog pane shown, 8 camera taken |
| +0x04 | top prototype (chain through each prototype's parent) |
| +0x08 | current prototype (`+0x04` text, `+0x40` length) |
| +0x0c | owner object (the script's context) |
| +0x10 | trigger-user guard: map index of the user that started the running trigger, -1 none |
| +0x18 | manual trigger type requested (`newtrigger`) |
| +0x1c | type of the trigger running |
| +0x20 | manual trigger string (`newtriggerstr`); +0x34 second string (USE) |
| +0x48 | instruction pointer (next line), 0 = not running |
| +0x4c | script flags (0x10000 paused) |
| +0x54.. | block stack, 8 bytes per level: loop start, conditional (0xdeaf = undefined) |
| +0xa4 | block depth |
| +0xa8 | trigger record running |
| +0xac / +0xb0 | ip / depth of an ALWAYS block interrupted by a higher trigger (resumed after) |
| +0xb4 | wait type (byte), see §5 |
| +0xb5 | multiplayer wait countdown |
| +0xb6 | chosen response index |
| +0xb8 | choice list (32-byte entries: label…) |
| +0xbc | wait parameter (frames left, or the object waited on) |
| +0xc4 | user object; +0xc8 second object; +0xcc user alias (`"user"`) |
| +0xd8 / +0xdc / +0xe4 | a pending `say` started at the next `Continue` |

## 3. Triggers

Types (1998 enum, same in retail): 1 ALWAYS, 2 TRIGGER, 3 DIALOG,
4 PROXIMITY, 5 CUBE, 6 ACTIVATE, 7 USE, 8 GIVE, 9 GET, 10 COMBAT,
11 DEAD.

Parsing (`TScriptProto::ParseScript` `0x00494e20`): `CUBE [<name>]
x,y,z x,y,z`. With no name the trigger takes the **prototype's** name; a
name alone (no coordinates) selects a named region; `NULL` means no name.

Test (`0x004927b0`, called per trigger by `Continue`). A trigger never
fires if it is the one running, or if its priority is below the running
one. Then:

| Type | Fires when |
|---|---|
| ALWAYS | always |
| TRIGGER, GIVE, GET | that manual trigger was requested with a matching string |
| USE | requested, string matches either USE string |
| DIALOG, ACTIVATE, COMBAT, DEAD | that manual trigger was requested |
| PROXIMITY | requested; or the name is the player's and the player is within `dist` of the owner (box, then dx²+dy² ≤ 2·dist²). User = player |
| CUBE | requested; or the name is `player` / the player's name and the player is inside the cube (user = player); otherwise some character or player is inside the cube on the owner's level (`0x00452480`) **and is not the named object** (user = that object) |

So an unnamed CUBE — named after its own prototype — fires when someone
*other than its owner* enters. (The 1998 test checked the owner's own
position: the opposite.)

After a firing test, if a trigger-user guard is set (`+0x10`) and that
object still exists (`0x00452690`), the trigger does not fire; once the
object is gone the guard clears.

## 4. `Continue` (`0x004933d0`)

Called from the object's pulse (`0x004708e0`, `0x00471260`) and the
character's (`0x004c3371`).

1. Skip if paused, the engine is reloading scripts, or the owner is
   paralysed.
2. If waiting: return unless the wait is satisfied (`0x00492d70`, §5).
3. A pending `say`: start it and wait for it (type 8).
4. Trigger scan, top prototype then parents: the first trigger that
   fires starts (`0x00492440`), unless an ALWAYS block is running and the
   new one is also ALWAYS. Starting records the trigger type, clears the
   block stack, calls the owner's (and user's) "script started" hook
   (vtable 0x148), and sets the trigger-user guard to the user's map index
   when the user alias is set. An ALWAYS block interrupted by a higher
   trigger is saved (+0xac/+0xb0) and resumed when that trigger ends.
5. Run lines through `CommandInterpreter` until a wait, the end, or 6000
   lines (infinite-loop guard). Result flags drive the block stack
   (BEGIN/END/IF/ELSE/LOOP/JUMP, §6.1 of ARCHITECTURE).
6. At the end: the "script ended" hook (vtable 0x14c) and `End`.

`End` (`0x00493e40`) clears the guard, the wait and the choice list, and
gives back what the script took: control on (flag 1), dialog pane closed
(4), camera back on the player (8).

## 5. Waits

`SetWait(type, param)` (`0x00492b00`) does nothing if already waiting.
Response waits (2, 5, 10) for the main player open the dialog pane's
choice list (`0x00535e90`) unless one is already up.

| Type | Satisfied when (`0x00492d70`) |
|---|---|
| 0 | not waiting (with `nowait`: immediately) |
| 2, 5, 10 | the player picked a response: jump to that choice's label |
| 3 | the object waited on finished its action (vtable 0x154) |
| 4 | a frame count ran out |
| 6 | the screen fade finished (`0x0048eb00`) |
| 7 | the buy/sell screen closed |
| 8 | the object waited on stopped talking |
| 9 | the object waited on is dead (health < 1) |

## 6. Attaching scripts to objects

`TScriptManager::ObjectScript` (`0x00497370`) matches a prototype to an
object by instance name, then by type name. Objects match when created or
loaded. Script files loaded later — area scripts load on
`TArea::Enter` (`0x0041ba00`) in single player — are announced with
`N_SCRIPTADDED` to every loaded object (`ParseScripts` `0x00496860`),
which matches itself again. Object names in sector and save files are
stored with bit 7 set on every byte (`TObjectInstance::Load`
`0x00472430`).

## 7. Port state (2026-10-04)

- Attachment: fixed (names decoded, world-wide notify).
- Instruction pointer: an offset into the prototype text (the 1998 raw
  pointer broke on 64-bit; fixed).
- Trigger test, user, guard, taken flags, waits: 1998 versions; being
  ported from retail (this doc).
