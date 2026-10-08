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
| Dialog choices | `DialogPane` labels | the dialog pane's choices; the wait check jumps to the picked label. A choice list on the script (`+0xb8`) exists only for multiplayer (DIALOG.md §2) |

## 2. `TScript` layout (0xe8 bytes; ctor `0x00492170`)

| Offset | Field |
|---|---|
| +0x00 | taken flags: 1 control off, 2 screen faded out (`fadescreenout` sets, `fadescreenin` clears; End fades back in only for a multiplayer host, SCREEN_SYSTEM.md §2.6), 4 dialog pane shown, 8 camera taken |
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
| +0xb6 | a remote player's response index (multiplayer) |
| +0xb8 | a remote player's choice list (multiplayer) |
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
   (BEGIN/END/IF/ELSE/LOOP/JUMP, §4.2).
6. At the end: the "script ended" hook (vtable 0x14c) and `End`.

`End` (`0x00493e40`) clears the guard, the wait and the choice list, and
gives back what the script took: control on (flag 1), dialog pane closed
(4), camera back on the player (8).

### 4.1 The prototype's text and the tokenizer

`ParseScript` (`0x00494e20`) keeps the **whole block** as the prototype's
text (`+0x04`, length `+0x40`): from the `OBJECT` keyword (the stream
position less the token's length and the character the tokenizer holds,
right after the first `SkipBlanks`) through the object's `END` and the line
break after it (the `WhiteGet` that checks for it). Trigger positions
(`+0x04` of a trigger record) are offsets into it, to the line after the
trigger's header. `TScriptManager::Save` (`0x00496690`, editor only)
writes each text as is followed by `"\r\n"`, in a file opened `"wb"` after
a header written with LFs.

The tokenizer (`TToken::Get` `0x00478a10`) reads every character through
the same step: carriage returns are skipped wherever they are, and a line
feed counts a line as it is read. A token holds the character after it
(`+0x2c`), which is never a CR: after an identifier that ends a CRLF line
the stream has passed the LF and the line is counted. So:

- a script error reports the line the tokenizer has read up to: a bad
  parameter at the end of its line is reported on the next one;
- the stream position after a line's last token is the next line's start;
- a `SetPos` on the stream leaves the held character in the token, and the
  next `Get` returns it first (the `ELSE IF` rewind below depends on it).

A 0xFF byte is the end of the stream (`GetChar` sign-extends it to −1).

### 4.2 Block stepping (`0x00493827`–`0x00493d81`)

Per line, from the stream position (`thisline`, one back when the
tokenizer stands inside a word):

1. `Get`, `SkipBlanks`. A pending `ELSE IF` position (below): `SetPos` to
   it, `WhiteGet`.
2. A `:` line is skipped (`SkipLine`). A line starting with an identifier,
   keyword or quoted text goes to `CommandInterpreter` (`0x00493942`);
   anything else prints "Bad token in trigger block".
3. The result bits, in this order:
   - `0x20` (context deleted): return.
   - `0x40` (IF true): the level's conditional = 1. `0x80` (IF false):
     `LineGet`; a `BEGIN` → `SkipBlock`, else `SkipLine`; conditional = 0.
   - `0x100` (ELSE): conditional `0xdeaf` → "ELSE without matching IF";
     conditional 1 → `LineGet`, then `SkipBlock` or `SkipLine` as for a
     false IF, and conditional = `0xdeaf`. Then, if the token is `IF`
     (`0x00493b06`): `SetPos(thisline)`, `Get`, `SkipBlanks` (the `ELSE`),
     note the stream position as pending, `Get`. The interpreter leaves the
     rest of an ELSE line unread (`0x0041ed34` skips it only without bit
     `0x100`), so after `ELSE IF c` the token is `IF` when the IF before
     was false, and the IF runs on the next pass -- read after that line's
     first token and the character the token held, so it reports "Bad token
     in trigger block" first when that line was a `BEGIN`. When the IF
     before was true, the skip starts at the condition: it ends on the ELSE
     line, the end-of-line skip drops the next line (the ELSE IF's
     `BEGIN`), and the ELSE IF's body runs one level up.
   - `0x200` (SKIPBLOCK): `SkipBlock`.
   - `0x800` (BEGIN): depth + 1; that level's loop start = 0, conditional =
     `0xdeaf`.
   - `0x1000` (END): depth − 1; below 0 → "END without matching BEGIN".
   - The level's loop start, if set: the stream goes there and it clears.
     `0x400` (LOOP): the level's loop start = `thisline`.
   - `0x1` (WAIT) or a wait set: the ip = the stream position; return.
   - `0x2000` (JUMP): the stream goes to the ip (`Jump` set it).
4. Read to the line's end. Depth below 1: the block ends (ip and flags
   `+0x48`/`+0x4c` cleared). Otherwise the iteration count (6000) goes
   down; at 0, "Infinite loop detected" and the block ends.

The block levels (`+0x54`, 8 bytes each: loop start, conditional) are
never cleared as a whole: the allocator zero-fills a new script
(`0x00482fb0`), the constructor sets level 0's conditional to `0xdeaf`,
BEGIN sets the level it opens, and `Start`/`Reset` (`0x00492440`,
`0x004924f0`) only set the depth to 0. A level a jump lands in keeps what
it last held -- 0 in a fresh script -- so an ELSE there runs its body
without an error.

**Jump** (`0x00493fa0`) counts every `BEGIN` and `END` token from the top
of the text into the depth (`0x004940cd`–`0x00494152`), the object's own
`BEGIN` included: a label directly in a trigger block is at depth 2, one
inside an `IF … BEGIN` at 3. It resumes at the stream position after the
label's name (`0x00494208`), the start of the next line (§4.1). So after a
jump the trigger's `END` leaves the depth at 1, and the block **runs on**:
the next trigger's header line goes to the interpreter (TCharacter's
`ParseCommand` answers "Unrecognized command.", on the console, seen only
in the editor), then that trigger's `BEGIN` and body run as part of the
block, and so on to the object's `END`, which ends it. A trigger block
that executes a `jump` itself runs on the same way. With the shipped
scripts (RETAIL_AB.md, target 4): 22 objects, 91 labels and blocks reach
another trigger, all a DIALOG block followed by `ALWAYS` except town.s
BAYNE1 (`CUBE`, whose `IF` is false by then). The ALWAYS body runs once as
the conversation's tail: keep.s DalyK and SteffanK walk one round of their
patrol, forest.s Jong1 waits 24 ticks, a dozen townsfolk walk their routes
-- and while it runs the trigger-user guard (`+0x10`) keeps the NPC from
being talked to again. When it ends, the ALWAYS block the conversation
interrupted resumes (`+0xac`).

At depth 10 retail's level is `+0xa4`, the depth itself: the first line
there resets the depth to 0 (its "loop start") and ends the block. The port
doesn't reproduce the overlap; it stops a block at depth 10 with "Blocks
nested too deep" (§7). No shipped script goes past depth 5.

## 5. Waits

`SetWait(type, param)` (`0x00492b00`) does nothing if already waiting.
Response waits (2, 5, 10) for the main player open the dialog pane's
choice list (`0x00535e90`) unless one is already up.

| Type | Satisfied when (`0x00492d70`) |
|---|---|
| 0 | not waiting (with `nowait`: immediately) |
| 2, 5, 10 | the player picked a response: jump to that choice's label |
| 3 | the object waited on finished its action: a complex object (OF_COMPLEX) is back in its root state (doing == root), or plays a looping animation (`AF_LOOPING`, imagery slot 0x8c) and its command is done (vtable 0x154); any other object, its command is done. So a walk, a chain of non-looping steps, holds the wait until it arrives |
| 4 | a frame count ran out |
| 6 | the screen fade finished (`0x0048eb00`) |
| 7 | the buy/sell screen closed |
| 8 | the object waited on stopped talking |
| 9 | the object waited on is dead (health < 1) |

## 6. Attaching scripts to objects

`TScriptManager::ObjectScript` (`0x00497370`) matches a prototype to an
object by instance name (`+0x38`, which an unnamed object shares with its
type), then by type name (`**(+0x4c)`): the `master.s` door prototypes
(DOOR1, PORTEW, ...) reach every door of their type this way. A new
script waits for a trigger (`InitScript` `0x00471150` ends with
`TScript::Reset` `0x004924f0`). Objects match when created or
loaded. Script files loaded later — area scripts load on
`TArea::Enter` (`0x0041ba00`) in single player — are announced with
`N_SCRIPTADDED` to every loaded object (`ParseScripts` `0x00496860`),
which matches itself again. Object names in sector and save files are
stored with bit 7 set on every byte (`TObjectInstance::Load`
`0x00472430`).

## 7. Port state (2026-10-07)

Retail:
- attachment (names decoded, world-wide notify; the second pass matched
  the class name until 2026-10-05, so no type-named prototype attached,
  and a new script started at offset 0, running its first trigger's
  header line as a command);
- prototype variables: `DATA` blocks (`0x00495750`/`0x00495830`, NUMBER
  and TEXT) on the prototype, the manager's lookups (`0x00497b40`,
  `0x00497800`, `0x00497a30`, `0x00497700`, `0x00497910`: every
  prototype named like the object or its type; reads take the first,
  writes go to all), `setprotovariable`, and their readers (expressions,
  `say`, `goto`, `stat`); only `forest.s` declares any (Jong's training);
- the instruction pointer, an offset into the prototype text (the 1998
  raw pointer broke on 64-bit);
- the trigger test (`0x004927b0`), the user and the trigger-user guard.
  A CUBE trigger's search for someone in the cube is retail's `0x00452480`
  (2026-10-07): the first moving object inside the cube on the **owner's**
  level, from that level's loaded sectors that meet the cube's map rect
  (iterator `0x0044cf80` flags `0x6e0`; the port's `TMapIterator(level,
  rect, …)`, `MapPane.ObjectInCube`). The 1998 search walked the map
  pane's 3×3 window, the player's level. The running-trigger record
  (`+0xa8`) the test also compares with is never written in retail, so the
  port has none;
- "running" is retail's test, the ip being set (`+0x48`); until
  2026-10-05 the port tested its `priority` (retail's flags, `+0x4c`,
  which no start sets), so every block counted as idle and an ALWAYS
  block restarted after each wait (TendrickR never reached its
  `If Rahul.stat health = 0`, and the chamber door never unlocked);
- `Continue`'s wait gate, `SetWait` and the wait check, the
  interpreter's wait post-hook and `wait`'s grammar. The type-3 check
  ended on any finished animation until 2026-10-05, so a script walk
  (`goto`, `gotorelative…`) went on after its first step
  ([COMMAND_SYSTEM.md](COMMAND_SYSTEM.md) §6.5);
- which lines run: `Continue` hands a line that starts with an identifier,
  a keyword or quoted text (token types 4, 3, 2) to the interpreter, skips a
  `:label` line, and reports anything else as "Bad token in trigger
  block". The port left quoted text out until 2026-10-05, so forest.s's
  `"TRAINING SWORD".DELETE` (MUDOKON1's ALWAYS block, line 1907, run once
  Jong has set `EQUIPSTATE`) printed that error on the text bar instead of
  running;
- object names and expressions (`if`, `while`): `src/scriptvalue.cpp`,
  [COMMAND_SYSTEM.md §2.4](COMMAND_SYSTEM.md);
- the trigger scan with the ALWAYS interrupt/resume (`+0xac`/`+0xb0`):
  a firing trigger starts if the script is idle, runs an ALWAYS block
  (saved to resume) or runs a block that interrupted one; an ALWAYS
  block starts only on an idle script, resuming where it was cut off.
  The "running trigger" record `+0xa8` that the trigger test compares
  with is never written in retail, so it plays no part; the trigger-user
  guard and the priority test keep a trigger from re-firing over itself;
- manual trigger requests (`0x00492640`, `TScript::Trigger`): the
  prototype search, the guard refusal, the user and second object with
  their aliases, the second USE name. Callers pass retail's arguments:

  | Caller | Request |
  |---|---|
  | `TObjectInstance::Use` `0x004705f0` | USE `<item>`, user "user", item "item"; alone: USE `<name>`/`<type>`, user "user", and on the user's script USE `<name>` with this object as "item" |
  | `TCharacter::Use` `0x004d4a60` | GET `<item>` (user "user", item "item"); GIVE `<item>` on the giver's script (this character "user"); DIALOG (player "user") |
  | `TCharacter::BeginFighting` `0x004d3b90` | COMBAT, the opponent as "user" and "enemy" |
  | the `trigger` command `0x00423710` | TRIGGER `<name>` |
  | dying | DEAD |

Not ported:
- the taken flags' control (1) and camera (8) bits and `End` giving
  them back (the dialog bit, 4, and the fade bit, 2, are ported);
- the multiplayer choice list (`+0xb8`/`+0xb6`); single-player responses
  go through the dialog pane (DIALOG.md, ported)
- the pending `say` (`+0xd8`…);
- a refused request's busy reply (`0x00494620`, multiplayer, needs
  `busysay`/`busymsg`);
- `TExit::Activate`'s ACTIVATE request has no user yet (retail
  `0x0050d3a0` passes the activating object, defaulting to the player,
  and leaves the exit to a scripted exit's block) — exits port;
- `TCharacter::Use`'s side effects when GET, GIVE or DIALOG fires:
  incidentals off for both characters, `0x004cee70`, `+0x108`;
- two retail USE callers whose classes aren't ported (vtables `0x5b70bc`,
  `0x5b72f8`, slot `0x110`: a delayed use that toggles state 2/3, then
  USE `<name>`/`<type>` with no user);
- (the screen-fade wait is ported, SCREEN_SYSTEM.md §2.6; the buy/sell
  wait ends when the shop is no longer in use, COMMAND_SYSTEM.md §6.6).

`Jump` (`0x00493fa0`), the `jump` command's and a picked response's (§4.2).
The search starts at the top of the prototype's text (the 1998 "skip down
to the current trigger" loop moves nothing: the fresh token isn't a BEGIN,
so `SkipBlock` `0x004795f0` returns at once) and steps token by token
(`LineGet` `0x004795a0`), counting `BEGIN`/`END` into the depth. A label it
can't find prints "Jump to an unknown label attempted" (`0x005da1dc`) and
returns 0 with the ip unchanged and the depth counted to the end of the
text (0, so the block ends after the jump's line). History of the port:

- 2026-10-05: the resume point after the label's name and the counted
  depth replaced the 1998 code, which skipped the label's line
  (`SkipLine`, eating the next line's first token: every shop's `:sell1` /
  `buysellshoptype sell …` lost its shop type) and set the depth to 1 ("a
  bit hacky"; a choice inside an `IF … BEGIN` ended the trigger at that
  IF's `END` and skipped `CONTROL ON` / `SETCDVOLUME FULL`: Gatekeeper1,
  Heather1, Pauline1, Verhoeven1, Kylie1, the shops of Hruthford, Gina and
  Cronus; found by the NPC sweep, STORY_TESTING.md §7). The 1998 code also
  restarted the script on an unknown label; no shipped script has one.
- 2026-10-07: the port's text was still the 1998 body (the lines between
  the object's `BEGIN` and `END`), so its depths were retail's − 1 and its
  blocks ended at the trigger's `END`; and its tokenizer held the CR after
  a word at a line's end, so the ip after a label was the LF before the
  next line. Both follow retail now (§4.1, §4.2): the text is the whole
  block, the depth counts the object's `BEGIN`, a block runs on after a
  jump, and the ip is the next line's start. Checked by the retail A/B
  (RETAIL_AB.md targets 1 and 4): every shipped label and block steps as
  retail's.

Port deviations in block stepping:

- **Depth 10.** Retail's tenth level overlays the depth field (§4.2); the
  port has the ten levels and stops a block that reaches depth 10 with
  "Blocks nested too deep". No shipped script nests that deep (retail
  depth 5 at most).
- An `END` too many (depth −1): retail reads level −1, its flags at
  `+0x4c`, as a loop start; the port touches no level outside 0–9. The
  block ends after the line either way.
