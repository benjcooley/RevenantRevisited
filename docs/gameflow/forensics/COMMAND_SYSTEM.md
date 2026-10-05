# Command system — forensics

The retail script/console command layer: the command table, the
interpreter that parses one command line and dispatches it, how a
command's target object is resolved, how parameters are evaluated, and
the protocol commands use to make a running script wait. Input to
[../ARCHITECTURE.md](../ARCHITECTURE.md). The script engine that runs
command lines (`TScript`, triggers, the wait state) is adjacent; only
its contract with the command layer is covered here.

Retail fidelity: **retail-confirmed** unless marked.

## 1. Requirements

- **Run the shipped scripts unmodified**: `master.s` (resources) and the
  Ahkuilon module scripts (`keep.s`, `town.s`, `forest.s`, `cave.s`,
  `dungeon.s`, `labyrinth.s`, `ruins.s`, `tower.s`, `arakna.s`). About
  10,900 lines; ~8,700 command lines. Usage by family is in the
  appendix — control flow (`begin/end/if/wait/set/jump`, ~3,650 uses)
  and speech (`say/choice`, ~2,000) dominate, then character (~1,170),
  presentation (~850), object (~700), buy/sell (~240), inventory (~100).
- **The same commands drive the console and editor** (retail
  `TConsolePane`; editor-only commands are rejected outside the editor).
- Exact retail parsing behavior matters: case-insensitive names, the
  `nowait` prefix, `<object>.<command>` and `group<N>.<command>` forms,
  per-command parameter grammars (`stat health = 0`, `toggle invisible
  = 1`, `say I1SAR00`, `choice A I2LOC00`, `pos x y z level`, quoted
  names like `addinv "Short Sword"`), and error recovery (a bad line is
  reported and skipped; the script continues).

## 2. Retail structure

### 2.1 Table

`SCommand Commands[189]` @ `0x005c6e88`, 7 dwords each: name, handler,
classcontext, classcontext2, requiresparams, editoronly, usage. Full
dump: [../../../recon/discovered/commands/RETAIL_COMMAND_TABLE.txt](../../../recon/discovered/commands/RETAIL_COMMAND_TABLE.txt);
one decomp per handler in the same folder. Class contexts use the
objclass ids (−1 none, 0 any, 10 EXIT, 11 PLAYER, 12 CHARACTER, 15
HELPER, 22 SCROLL; second id is an alternative). The pre-release
snapshot has 96 of the 189.

### 2.2 Interpreter — `CommandInterpreter(context, token, abbrevlen, script)` `0x0041e8e0`

1. Skip blanks; end of input → 0. Copy the first token; record the
   dialog context (`0x00533dc0`).
2. If the token is an identifier/text: a leading `nowait` sets a flag
   and advances. If the next token is `.`:
   - `group<N>.` → group mode (N parsed from the name);
   - otherwise resolve the name with the context resolver (§2.3);
     unresolved → "`<name>`: Context not found", result 2 (the
     pre-release returned 0 here).
   The command name must follow the dot.
3. Look the name up top to bottom: abbreviation match when `abbrevlen`
   > 0 (console), exact case-insensitive match otherwise (scripts).
   Editor-only outside the editor → error; class-context mismatch
   (unless group mode) → "Object context required" / "Command not
   available for context's class".
4. Advance past the name; `requiresparams` with nothing left → usage.
5. Dispatch:
   - group mode: iterate map objects (`TMapIterator` `0x0044cf80/
     0x0044d080`) with group == N and a matching class, calling the
     handler for each; stop on an error result;
   - otherwise `handler(target, token, context, script)`.
6. No table match (result 2) and a target → the target's own command
   parser (object vtable `+0x150`, `TObjectInstance::ParseCommand`).
7. Report: 4 "Bad parameters", 8 "low memory", 4|0x10 usage, 2
   "Unrecognized command"; otherwise, if tokens remain on the line and
   the result lacks bit `0x100` (`CMD_ELSE` in the pre-release enum),
   warn "(extra parameters ignored)" for non-error results, then skip to
   end of line.
8. `nowait` turns result 1 into 0. Result `0x20` (context deleted)
   returns immediately.
9. If there is a calling context and a target, and the context's script
   is not already waiting (`0x00471390`): result 1 → `script->WaitChar
   (target)`; result `0x4000` → `script->WaitSay(target)`.

### 2.3 Context resolver `0x0041e690(name, context, script)`

In order: `this` → context; `user` → the script's user (`0x00492ac0`:
the triggering player, else the main player); `player` → main player;
`target` → the context character's current combat target (when its
action is attack or `0x19`); `current` → `script+0xd4`; `party<N>` →
N-th party member (multiplayer); the script's two alias slots
(`script+0xcc`/`+0xc4`, `+0xd0`/`+0xc8`); otherwise the closest map
object with that name (`TMapPane::FindClosestObject` `0x00451fe0`,
measured from the context or the map center).

### 2.4 Parameters and values

Handlers read their own parameters with the `TToken` API (retail
addresses: `Is` `0x00479700` with optional abbreviation, `WhiteGet`
`0x00479580`, `Get` `0x00478a10`, `SkipLine` `0x004795c0`,
`SkipBlanks` `0x00479680`; token types 1 whitespace, 2 text, 4 ident,
8 number, 9 return, 10 EOF — identical to `src/parse.h`). Value
sources a parameter can name:
- literals (numbers, quoted text, `true/false/on/off`);
- game-state variables (`TGameState`, `state.def`; `set` writes them,
  `0x004975d0` reads them);
- per-prototype script variables declared in `DATA`/`NUMBER` blocks
  (`TScriptManager` `0x00497b40` type, `0x00497800` number,
  `0x00497a30` string; matched on the context's name or type name);
- dialog tags (`english.def`, `TDialogList`);
- object stats (`stat`, `if <obj>.stat <name> <op> <value>`).

#### Naming an object (`0x0041e690`)

The interpreter's `<context>.` prefix, `wait death` and the evaluator's
object values all resolve names here, given the calling object and its
script:

| Name | Object |
|---|---|
| `this` | the caller |
| `user` | the script's user (`0x00492ac0`: the triggering player, else the main player) |
| `player` | the main player |
| `target` | a character's opponent: the object of its root action when that is COMBAT (3) or BOW (25) (`TCharacter::Fighting`); nothing for other callers |
| `current` | the script's `+0xd4`, set only by `setcurrent` (`0x00428e50`) |
| `party<N>` | the Nth player sharing the base's party name (`TPlayer +0x494`), base = the script's user, else the caller if a player; the base when no member matches |
| an alias | the script's user alias (`+0xcc` → `+0xc4`) or second alias (`+0xd0` → `+0xc8`), named by manual trigger requests (`0x00492640`) |
| anything else | `TMapPane::FindClosestObject(name, caller, exact, all)`: the nearest object with exactly that name (ignoring case) in the sector window |

Names are compared ignoring case and in full (`stricmp`). An unknown
`<context>.` makes the interpreter report "Context not found" and return
`CMD_BADCOMMAND`. `FindClosestObject`'s `partial` flag selects an
abbreviation match; the 1998 source had the two branches swapped, and
retail passes `partial` only from `get`, `select` and `swap`.

#### Expressions (`0x0041f230`)

`if` and `while` evaluate the rest of the line, as does
`setprotovariable` after its `=`. Left to right, no precedence:

- operands: a number; quoted text, hashed (each character's offset from
  `A`, shifted to its index, OR-ed); a bare name — the context's
  prototype variable (`0x00497800`), else a game state (`0x004975d0`),
  else 0; `<object>.<member> …` (below);
- operators (table `0x005c8350`, the codes are its indices): `=` `<>`
  `>` `<` `>=` `<=` `and` `or` `not` `+` `-` `*` `/`, a symbol operator
  being one or two symbol tokens. `ApplyOperation` (`0x0041f0b0`):
  comparisons and `and`/`or`/`not` leave the right operand as the
  running operand, arithmetic leaves its result, `/0` is 0;
- the result is the last operation's value (or the single operand).

Members of `<object>.` (resolved as above; the object missing ends the
expression with the result so far):

| Member | Value |
|---|---|
| `state` | the object's state |
| `getitemamount` / `amount` `<item>` | inventory amount by name (vtable `0x84`) |
| `hasemptyslot` | vtable `0x88` |
| `isoutside <obj>` | which side of the object `<obj>` is on (`0x0050d2b0`) |
| `maxslots` | the outermost container's capacity (`0x00470040`) |
| `timeofday` | the PlayScreen's time of day |
| `random` | 1–100 |
| `face` | the facing byte |
| `getdistance` / `getdist <obj>` | 2D table distance (vtable `0x04`), 0 if `<obj>` is missing |
| `isatrelativeposition` / `isrelpos <obj> <dx> [<dy>]` | within 50 of `<obj>`'s position + (dx, dy) |
| `isatrelativedistance <obj> <dist> [<angle>]` | within 50 of the spot `dist` from `<obj>`, measured from its facing turned by `angle` (behind it for a positive `dist`; §6.5) |
| `lastattack = "<attack>"` | the character's last attack had that name and landed (`+0x160` attack, `+0x168` its impact result from `0x004c62b0`) |
| `position.x` / `.y` / `.z` | position |
| `groupinrange …` | the `groupinrange` command's result |
| anything else | the member word is skipped and the next word names a statistic (`0x00473900` checks it exists, else 0): `Rahul.stat health` |

Quirks the shipped scripts rely on (the port keeps them):
- the operator handler steps past its spacing with `WhiteGet`, so in
  `PedSix.state=1` (no spaces) the `1` is consumed and the line tests
  the state alone;
- only `not` may come before the first operand, and it is left pending,
  so `not X` reads as `X`;
- `random`'s optional range is tested on the keyword token, so it
  never applies.

Retail's undefined cases fail the expression in the port: an
unrecognized token (retail loops forever), `isatrelativeposition` on a
missing object (dereferenced), `isatrelativedistance` on a missing object
(the evaluator returns 4 without a value, so `if` and `while` read their
own untouched result variable, the caller's address, and come out true),
`position.` with no x/y/z (stale value).
An expression that fails makes `if` return false with
`CMD_BADPARAMS` (`0x84`); `while` returns `CMD_BADPARAMS`.

### 2.5 Wait protocol (owned by TScript)

`TScript::SetWait(type, param)` `0x00492b00` (no-op while a wait is
active; `+0xb4` wait type, `+0xbc` wait object): 2 response, 3 target's
action done (`WaitChar`), 4 N frames, 5 response (variant), 6 screen
fade, 7 buy/sell screen, 8 target's speech (`WaitSay`), 9 target death,
10 response + control on. Response waits by the player open the dialog
pane (`0x00535e90`). Commands reach it through `TObjectInstance`
helpers that test `+0x84` (the object's script): `0x004712b0`
(SetWait), `0x004712d0` (frames), `0x00471310` (WaitChar), `0x00471330`
(WaitSay), `0x00471350` (death), `0x00471390` (is waiting).

### 2.6 Output

`Output` `0x0041ee50` formats into a shared buffer and writes to the
console only in the editor with the console open. Handlers report
errors through it; scripts never see the text. The port also logs it at
debug level (`[console]`), so headless `--exec` runs can read it.

## 3. Pre-release vs retail

- Handler signature: pre-release `(context, token)`; retail adds the
  calling context and the running script.
- Retail adds 93 commands (appendix), the `0x4000` speech-wait result,
  the script-side wait machine (pre-release waited on the character's
  action queue: `TCharacter::Wait/WaitChar/WaitResponse`), the alias /
  `this` / `user` / `target` / `current` / `party<N>` resolver (pre-
  release: `FindClosestObject` only), and an error result for an
  unresolved context.

## 4. Port state (2026-10-05, `feature/gameflow`)

- `Commands[]` mirrors the retail table (order, contexts, flags, usage
  text) and handlers take the retail argument list. Missing commands are
  `CmdNotPorted` stubs: they log once with the retail address and answer
  `CMD_BADCOMMAND`, as retail answers a command it doesn't have. The
  interpreter then offers the line to the target's `ParseCommand` (slot
  `0x150`, which answers 2 for every class), reports "Unrecognized
  command", and the script goes on with the next line.
- The commands of the Keep's opening scene are ported: `incidentals`,
  `fadecharacterout`/`fadecharacterin`, `playerlevel`, `setcdvolume`
  (§6). So are the door prototypes' `faceobject`, `gotorelativedistance`,
  `gotorelativeposition` and the `isatrelativedistance` member, and `face`
  is checked against retail (§6.5). `fadescreenout`/`fadescreenin` and
  `wait screenfade` are ported
  (SCREEN_SYSTEM.md §2.6). `wait` takes any object as its context, as
  retail; with the 1998 character-only context a door's `WAIT` failed
  the class check.
- The exit commands are ported (EXITS.md §4, §7): `activate` (forced,
  the script's user if a player), `follow`, `operate`, `setfromexit`,
  `pos` (retail grammar, including the no-argument form; through
  `TObjectInstance::Teleport`), and `statmod` (`0x00428200`).
- `endgame` (`0x00427060`) returns to the title; `playmovie`
  (`0x00427d80`) plays `MoviePath\<file>` as a modal movie pane on the
  PlayScreen after stopping the music (`0x0049a560`). Retail's player
  blocked, so the next line ran after the movie; the port answers
  `CMD_WAIT` to end the pass and holds the world (scripts included) until
  the movie is over.
- `try` (`0x00422c30`) takes a quoted state as well as a bare word (`%t`,
  else `%s`): the door prototypes' `TRY "WOPENDOORIN"` failed before.
  `goto` (`0x004204f0`) takes an object name (keep.s `goto Point2`, the
  town and forest waypoints) and walks to it (`0x004cee50`); otherwise
  each coordinate may name a number variable.
- The fighting commands forest.s needs are ported over the character's
  own calls: `beginfighting` (`0x00427cd0`) finds its target with
  `FindObject` among the characters (`0x00451d70`, objset 2: the active
  window's objects by name, not the resolver's aliases) and calls
  `BeginFighting(target, COMBAT)` (`0x004d3b90`); `endfighting`
  (`0x00427d30`), despite its name, calls `BeginFighting` with no target,
  so the character squares up to the closest enemy (no script uses it);
  `specificattack <n>` (`0x00427c80`) is `SpecificAttack(n)`
  (`0x004d2a60`). `giveweapons "<name>"` (`0x00422150`, slot `0x68`
  `TObjectInstance::GiveWeapons` `0x00477780`) moves every weapon, ranged
  weapon and ammo, bags included, to the first object of that name; on the
  way retail's `RemoveFromInventory` (`0x0046faf0`) unequips a player's
  item, which the port's lacked (the equipment kept pointing at the item).
  Open, combat track: `SpecificAttack` and `BeginFighting` are still the
  1998 bodies; the trainer's `specificattack 25` beside the dummy answers
  "Invalid Attack" in the port (decomps in `recon/discovered/`).
- Prototype variables are ported (SCRIPT_ENGINE.md §7): `setprotovariable`
  (`0x0041fd70`), bare names in expressions, `say` text parts, `goto`
  coordinates, `stat`'s value. `choice` checks a name's type on the
  context but retail read the value with no object (a number came out as
  the not-found value, text dereferenced null); the port appends nothing
  for a variable there. `addat` (`0x00421770`) is ported on `add`'s
  body (coordinates as numbers or variables, then an amount and the
  type). The buy/sell criteria commands read their bounds the same way
  (§6.6). `getitemname`/`getitemvalue` also read variables; no shipped
  script uses them and they aren't ported.
- The buy/sell family is ported (§6.6): the shop is PlayScreen's bottom
  drawer (`TBuySellPane`, [BuySellScreen_SPEC.md](../../ui/forensics/BuySellScreen_SPEC.md)),
  and `wait buysell` holds the script until the shop's Exit.
- A line the interpreter skips (class context mismatch, editor-only,
  missing parameters) takes the next line with it: `SkipLine`
  (`0x004795c0`) reads past the line's end into the next line's first
  token, and `Continue` (`0x004933d0`) then skips "the rest of the
  line" — the next one. Retail, kept. A port command that skips where
  retail's doesn't is the bug to fix (as `wait`'s context was).
  *Correction (2026-10-05):* this note used to say the same skip runs
  after every `:label` line a jump lands on. It doesn't: `TScript::Jump`
  (`0x00493fa0`) sets the ip right after the label's name
  (`0x00494208`), so the line after the label runs. The port's 1998
  `Jump` skipped the label line with `SkipLine`, eating the next line's
  first token; labels followed by a blank line hid it, but every shop's
  `:sell1` is followed by `buysellshoptype sell …`, which was lost
  (SCRIPT_ENGINE.md §7).
- `stat <name> = <n>` (`0x00424010`) sets the stat and then answers
  "bad parameters" in a script: it requires end-of-file after the value
  (`type != 10` → 4), and a script line ends in a return. Retail, kept
  (the message goes to the console). A value that isn't a number is
  looked up as a number variable of the caller's prototypes
  (`0x00497800`, sentinel `0xfeced300` → bad parameters).
- `--exec` (`src/consoleexec.cpp`) is a separate command queue —
  **below the bar** (duplicates the console's job).
- The interpreter's context syntax, the object resolver and the
  expression evaluator are retail (`src/scriptvalue.cpp`). Not ported:
  `setcurrent`, multiplayer parties,
  and the members `maxslots`,
  `lastattack` (needs the attack-impact result, `TCharacter +0x168`) and
  `groupinrange` — those fail the expression and log once.
- A command answering `CMD_WAIT` on a character holds the script until
  the character is back in its root state, or ends a loop of a looping
  animation (retail `0x00492d70`); the port had released it at the end
  of any animation, so a script walk went on after its first step
  (§6.5, SCRIPT_ENGINE.md §5).

## 5. Open questions for the author

- `timelimit`'s usage text is the `script edit/pause/resume/end`
  help — a leftover, or is it really the script-control command?
- `0x4000` vs `CMD_WAIT`: was the speech wait added specifically so
  `nowait say` could still let the script continue while the speaker
  talks?
- When a trigger starts, `TScript::Continue` turns incidentals off for the
  script's object and its two alias objects (the user: Locke, for a CUBE
  trigger he walks into), but turns them back on only if the trigger runs
  to its end in the same pass. A trigger that waits leaves them off. Was
  that intended? (§6.2)
- `playerlevel` gives each level's attribute points with `random(0, 6)`
  over seven stat ids starting at Strn, so one point in seven goes to the
  Attack skill. Intended? (§6.1)
- Settled ([OPTIONS.md](OPTIONS.md) §7.9): `MusicVolume` isn't applied
  at boot; it reaches the CD when the Options pane opens, on a drag and on
  OK, and until then the CD keeps the OS mixer's level (question 16).
- Walking to and facing objects (§6.5): questions 45–49 in
  [../AUTHOR_QUESTIONS.md](../AUTHOR_QUESTIONS.md).

## 6. Ported commands

§6.1–§6.4 are the Keep's opening scene; §6.5 the door prototypes'
walking and facing.

Sardok's block in `keep.s` (`SardokR`, the first scene of a new game)
runs `incidentals off`, `fadecharacterout player`,
`player.PLAYERLEVEL 1`, `player.toggle invisible`,
`fadecharacterin player`, `SETCDVOLUME HALF` and `incidentals on`.

Vtables (for the slots below): TCharacter `0x005a7848` (slot 0
`0x004d6c00`), TPlayer `0x005b4f30` (slot 0 `0x0051ff00`). `0x005a7b98`
and `0x005b5364` belong to other classes: their stat slots are the
TObjectInstance stubs. Character slots: `0xdc` GetObjStat, `0xe8`
SetObjStat, `0x138`/`0x13c` FindState/FindTransitionState(…, pcnt),
`0x148`/`0x14c` incidentals off/on, `0x1c0`/`0x1c4` Health/SetHealth,
`0x1c8`/`0x1cc` Fatigue, `0x1d0`/`0x1d4` Mana, `0x1d8`/`0x1e0`/`0x1e8`
MaxHealth/MaxFatigue/MaxMana, `0x208` SetDesired(ab, flags), `0x210`
UpdateAction, `0x214`/`0x218` TryCommand/ForceCommand(ab, bits, flags),
`0x2d8` Transparency, `0x340` the say handler. TPlayer adds `0x358`
SetLevel and `0x418` AddSkillExp.

### 6.1 `playerlevel` `0x00428640`

A number is required (else 4); `__ftol` of it goes to
`TPlayer::SetPlayerLevel` `0x0051d840`, then the token advances.
SetPlayerLevel:

1. SetLevel(n) (`0x005201f0`, the Level stat entry, id 17).
2. Attributes `0x22+i` (Strn … Luck) = |class STATREQS[i]|, or 14 when 0
   (chardata `+0xfc` → classdata `+0x190`, STATREQS at `+0x20`).
3. For each skill: level `0x28+s` = 0, experience `0x33+s` = 0, next
   level's experience `0x3e+s` = 300.
4. For each level 1 … n−1: (level < 15 ? 2 : 1) points, each to stat
   `0x22 + random(0, 6)` (the seventh id is the Attack skill), then
   AddSkillExp(s, 333 + 100·(level − 1)) for every skill.
5. Stat 20 (`AttackLevel`) = n; health, mana, fatigue to their maximums;
   RefreshStats `0x0051c660`.

`TPlayer::AddSkillExp` `0x0051ac90`: at skill level ≥ 30 the experience
is pinned to SkillExp(30); otherwise the experience is added, and if it
reaches SkillExp(level + 1) the skill goes up one level and its next
experience becomes SkillExp(level + 2). In a network game it first
forwards the call, or drops it for a remote player. SkillExp
`0x0048cc90` reads a 30-entry table (Rules `+0x168`) that
`TRules::Initialize` `0x0048b690` computes: 300, then +100·i + 300 per
level.

Retail stat ids are CLASS.DEF's PLAYER OBJSTATS order (17 Level, 20
AttackLevel, `0x18`–`0x1d` Max{Health,Mana,Fatigue}{Flat,Pct}, `0x22`
Strn … `0x27` Luck, then four rows of 11 skills: level, exp, next exp,
cap). The port's `charstats.h` uses the same numbering (SAVE_GAME.md
§10 item 12): `PLRVAL_FIRST + PLRVAL_ATTACKLEVEL`, `PLRSTAT_FIRST`,
`SK_FIRST`, `SKE_FIRST`, `SKN_FIRST`, `SKC_FIRST`.

Port: `TPlayer::SetPlayerLevel`, `TPlayer::AddSkillExp`,
`TRules::SkillExpForLevel`, `TPlayer::RefreshStats`. Retail TPlayer
keeps a second, equipment- and spell-modified copy of the stats
(`+0x34c` count, `+0x350` {id, value} pairs): SetObjStat writes both,
GetObjStat reads the copy, RefreshStats rebuilds it, caps attributes and
skills at 30 in it, and clamps health/mana/fatigue. The port does the
same ([PLAYER_STATS.md](../../gameplay/forensics/PLAYER_STATS.md)).
Inside SetPlayerLevel the two agree (every value it reads was just
written). SetPlayerLevel leaves Exp and NextExp alone.

Opening scene, Locke from `newgame.sav`: STR 16 CON 12 AGI 14 RFL 14
MND 14 LCK 16, ten skills at 30, AttackLevel 0, H 25/100 M 0/105
F 78/78 → after `playerlevel 1` (class Revenant, STATREQS 18, −12, 0, 0,
14, 0 in file order: the fifth value is Mind's): STR 18 CON 12 AGI 14
RFL 14 MND 14 LCK 14, skills 0 / 0 / 300, AttackLevel 1, H 100/100,
M 105/105, F 78/78. The script then sets AttackLevel to 0 itself.

### 6.2 `incidentals` `0x00428250`

`on` clears and `off` sets bit 2 of `charflags` (TCharacter `+0x110`);
any other word → "State must be included", 4. The word isn't consumed
(the interpreter skips it with the rest of the line).

The bit picks between a state's random variants. Imagery states named
`NN:name` are frequency variants (`3:walk`, `75:walk`, `100:walk` …):
FindState rolls 1–100 and takes the variant whose number is the smallest
one ≥ the roll; pcnt 100 always takes the 100% variant. Bit 0 of
TryCommand/ForceCommand's flags makes every lookup use pcnt 100.
Readers of `charflags & 2`:

- `0x004c3429`, UpdateAction (`0x004c3260`): the per-frame
  TryCommand(desired, bits, incidentals off ? 1 : 0);
- `0x004c8437`, the say handler (`0x004c8400`, the port's ResolveSay):
  ForceCommand(root, 0, incidentals off ? 1 : 0).

SetDesired `0x004db3a0` forwards a flags argument to ForceCommand too;
its callers pass registers that look zeroed (not checked one by one).

Other writers: a script trigger fired on a character sets the bit —
`0x004c245c` (hit, trigger `0xb`), `0x004d3f94` (trigger `0xa`),
`0x004d4add`/`0x004d4b15` (TCharacter::Use, triggers 9 and 8). Slot
`0x148` (`0x004d5f70`) sets it and, when the character is doing its
root, clears action flag `0x10` on doing and desired; slot `0x14c`
(`0x004d5f60`) clears it. `TScript::Continue` `0x004933d0` calls `0x148`
on the script's object and its two alias objects when a trigger starts,
and `0x14c` on them when the script ends in the same call that started
the trigger.

Port: `TCharacter::SetIncidentals`/`Incidentals` (`charflags` now starts
at 0; retail's allocator zeroed objects), the flags argument on
`TComplexObject::TryCommand`/`ForceCommand` (`kCommandNoIncidentals`),
`FindState`/`FindTransitionState` taking pcnt, and the two readers. Not
ported: the trigger writers and the TScript calls (script engine and
character owners). Opening scene: while it is off Sardok's root resolves
to `100:walk` every time; after `incidentals on` he plays `75:walk`.

### 6.3 `setcdvolume` `0x00428b20`

`half` → the CD's base volume / 2, `full` → the base volume; anything
else (or nothing) → 4. The value goes to `0x0049a610` (the CD's current
volume, clamped 0–`0x60`, set on the redbook device while playing), then
the token advances.

The CD object is `0x0065abc8`: `+0` redbook handle, `+4` base volume
(`DAT_0065abcc`), `+8` playing, `+0xc` current volume, `+0x10`/`+0x14`
fade step/pending (stepped by `0x0049a380`). Init `0x0049a270` sets the
base to `0x60` and current to the device's own volume. The Options pane
calls `0x0049a5c0` when it opens, on a `Music` drag (`0x0053b431`) and on
OK (`0x0053af87`); it sets base and current to the device's readback. The
INI `MusicVolume` (default `0x7f`) is read at boot (`0x00484ae0`) and
written back (`0x00484ed0`) through `0x005d7a9c`, but reaches the CD only
through the Options pane ([OPTIONS.md](OPTIONS.md) §7.9).

Port: the player's music volume is the music group volume
(`audio::SetMusicVolume`); `setcdvolume` sets the scale on the music
voice (`audio::MusicSetVolume` 0.5 or 1), which now lasts across tracks
like the redbook volume. Retail's half is an integer halving of the base.
The music group volume is the player's `MusicVolume`, set at boot and by
the Options pane (`ApplyMusicVolume`, OPTIONS.md §9).

### 6.4 `fadecharacterout` `0x00428020`, `fadecharacterin` `0x00428070`

The name resolves through `0x0041e690` from the command's context; not
found → 4. Then `TCharacter::Fade` `0x004d56c0` with −1 or +1 (on
whatever object was found), and in a network game the fade is sent to
the other players. The name isn't consumed.

| Offset | Field |
|---|---|
| `+0x194` | visibility 0–100 |
| `+0x198` | step, subtracted each pulse (positive fades out) |
| `+0x19c` | limit (−1: run to 0 or 100) |
| `+0x1a0` | direction −1 / 1 / 0 (written only; no reader found) |
| `+0x1a4` | invisibility-spell flag |

- Fade `0x004d56c0`: +1 → step −5, limit 100, only while Health > 0;
  anything else → step 5, limit 0.
- SetFade `0x004d5730` (effects `0x004e6c34`, `0x004e6d1a`, `0x004e7087`;
  network `0x005836d2`): applies when Health > 0 or step ≥ 0; a negative
  visibility keeps the current one.
- UpdateFade `0x004d57a0`, called only from TCharacter::Pulse
  `0x004c1bb0`, every pulse: visibility −= step; fading in past 0 clears
  OF_INVISIBLE; stops at the limit, at 0, or (out, or with no limit) at
  100.
- SetInvisible `0x004d5880` (invisibility spell `0x004fd8c0`): on → step
  5 to 30; off → back to 100 while alive.
- ClearChar `0x004c18a0`: 100 / 5 / 100 / 0 / 0. Load `0x004d4eb0` ends
  with 100 / 0 / 100 / 0. Appear `0x004d4460` (slot 8 move + flags reset):
  0 / 4 / 100 / 1, which snaps to 100 on the next pulse.
- Transparency `0x004c5a50`: the visibility clamped to 0–100 (40 at most
  for a player in player state 2 of a network game).

What brings Locke back in the opening scene: `newgame.sav` stores him
with OF_INVISIBLE set; `player.toggle invisible` clears it and
`fadecharacterin` raises the visibility 0 → 100 in 20 pulses (UpdateFade
would also have cleared OF_INVISIBLE on its first step). The resurrect
state is only the animation.

Port: `TCharacter::Fade`, `SetFade`, `UpdateFade`, `SetInvisibleSpell`
(retail bodies), UpdateFade from `TCharacter::Pulse`, the direction
field, retail `Transparency()` (the pre-release one also hid aggressive
monsters the player hadn't seen). The commands reject a name that isn't
a character (retail wrote the fade fields of any object). Opening scene:
Locke 100 → 0 over pulses 64–84, 0 → 100 over pulses 134–154.

**The draw path (ported 2026-10-05).** In retail the character animator
(`0x004d7a50`) skips an OF_INVISIBLE character (outside the editor) and
otherwise moves its alpha toward Transparency()/100 by 0.05 per drawn
frame, not drawing below 0.01. The port does the same, time-based at 1.2
alpha per second (retail's 0.05 per frame at 24 Hz), and draws a partly
faded character through a translucent pass lit by the light pass's own
model: [RENDERER_ARCHITECTURE.md](../../RENDERER_ARCHITECTURE.md) has the
design and the deviations.

### 6.5 Walking to and facing an object: `faceobject`, `gotorelativedistance`, `gotorelativeposition`, `isatrelativedistance`

The door prototypes in `master.s` ([EXITS.md](EXITS.md) §1.9) walk the
user to a spot beside the door, check that he got there, and turn him to
it. The context is the user; `THIS` is the door:

```
USER.GOTORELATIVEDISTANCE THIS 42 -32
IF USER.ISATRELATIVEDISTANCE THIS 42 -32 = 1
BEGIN
    USER.FACEOBJECT THIS 15
```

Shipped command lines (the loose `Resources/master.s`, quoted above, and
the copy in `resources.rvr` agree on these; they differ elsewhere:
`USER.` against `player.`, and where DOOR1/DOOR2 operate the door. The
port reads the archive copy, its trace shows `PLAYER.STOP`; which one
retail reads is the resource layer's choice, `0x004a13f0`, not checked
here):

| | master.s | keep.s | town.s | forest.s | cave.s | dungeon.s | tower.s | total |
|---|---|---|---|---|---|---|---|---|
| `face` | 12 | | 1 | 1 | 5 | 17 | | 36 |
| `faceobject` | 10 | | 4 | 2 | | | 1 | 17 |
| `gotorelativedistance` | 28 | | 1 | | | | | 29 |
| `gotorelativeposition` | | 4 | 19 | 42 | 10 | 10 | 1 | 86 |
| member `isatrelativedistance` | 8 | | | | | | | 8 |
| member `isatrelativeposition` | | | | | 5 | 3 | | 8 |

`labyrinth.s`, `ruins.s` and `arakna.s` use none. No script gives
`gotorelativeposition` a second pair; the seven `faceobject` lines
outside `master.s` give no offset.

**The handlers** (decompiles and disassembly in `recon/discovered/commands/`):

| Command | Grammar | Effect | Answers |
|---|---|---|---|
| `face` `0x00420800` | `[<obj>.]face <angle>` | facing byte (`+0x36`) and move angle (`+0xb0`) = angle, on any object | 1; 4 without a number |
| `faceobject` `0x00420840` | `[<obj>.]faceobject <name> [<offset>]` | facing and move angle = `AngleTo(name)` (`0x0046ea90`) + offset | 1; 4 when the name finds nothing |
| `gotorelativedistance` `0x00420710` | `<character>.gotorelativedistance <name> <distance> [<angle>]` | `Goto(spot, 0)`, the spot below | 1; 4 without a distance; "Can't find any object by that name." and 4 |
| `gotorelativeposition` `0x004205c0` | `<character>.gotorelativeposition <name> <dx> <dy> [<dx2> <dy2>]` | `Goto(name + (dx, dy), 0)`, or `+ (dx2, dy2)` when that spot is nearer the walker (`Distance2D` `0x0046de60`; a tie keeps the first) | 1; 4 without both numbers; "Can't find…" and 4 |

The name is the current token's text, then `WhiteGet`; numbers are
`Parse "%d"` (`0x0047a410`), checked before the missing-object message.
These three resolve the name from the **calling** object (handler
argument 3): in a door's script `THIS` is the door. `pivotobject`
(`0x00420900`) resolves from the turning object (argument 1) instead.

**The spot** `gotorelativedistance` walks to and `isatrelativedistance`
tests (inline x87 in both; the decompiles drop the multiplications by the
distance, the disassembly has them): with `a` = the object's facing byte
+ angle and `k` = the float at `0x005a3a90` (`0x3cc9d9aa`, 2π/255),

    x = obj.x + trunc(distance · sin((a + 0x7f) · k))
    y = obj.y + trunc(distance · cos(a · k)),  z = obj.z

Facing `f` points along (sin f, −cos f) (`ConvertToVector`), so for a
positive distance the spot lies behind the object, turned by `angle`,
skewed by the `0x7f` and the 255 steps (about a unit at the door
distances). The Keep's `ressexit` (sector 2_1_1) stands at (1190, 1120)
facing 0: `-25 18` → (1200, 1098) inside the resurrection chamber,
`42 -32` → (1220, 1149) outside; `isoutside` picks the spot on the
user's side.

**The member** `isatrelativedistance <name> <distance> [<angle>]` reads
its tokens like `isatrelativeposition`: past the member word, the name
(resolved from the caller), then the numbers; no distance → 0. True when
`Distance2D(spot, member's object) < 0x32`. A missing object prints the
message and returns 4 from the evaluator without a value (§2.4).

**The walk.** `TCharacter::Goto(x, y, item)` `0x004cedb0`: `Go(AngleTo)`
(`0x004ce350`); if it went, the target (x, y, own z) and flag `0x1000`
go on the desired block, or on the doing block when desired is the root
(the walk started at once, or Go turned the current step); `+0x288` =
item when one is given (a click on a far item: walk there, pick it up,
`0x004cfef0`). The commands pass none. `ResolveAction` `0x004c3490`
sends a MOVE to `ResolveMove` `0x004c5e90`: with a target, at
|dx| + |dy| − min/2 < 8 it snaps onto the target (`MoveTo`), marks the
block nowaitdone (and picks the item up); otherwise each step heads at
the target. The combat-mode moves (`0x004c7f80`, `0x004c7980`) clear the
target unless an item is pending, so a goto in combat mode keeps its
first heading; the door prototypes turn `COMBAT OFF` first.

**The wait.** Answer 1 makes the calling script `WaitChar` the context
(type 3). `0x00492d70` takes a complex object as done only when it is
back in its root state, or, playing a looping animation (`AF_LOOPING`,
imagery slot `0x8c`), when its command is done. Walk steps don't loop,
so the script waits for the arrival (or for a blocked walk's return to
root). `face` and `faceobject` change no action, so their wait ends at
once on an idle character.

**Port.** The four as above (`src/command.cpp`, `src/scriptvalue.cpp`),
the spot as `RelativeDistanceSpot` (`src/scriptvalue.h`), shared.
`TCharacter::Goto` now gives the target to retail's block (it wrote
`doing`, which isn't the walk when the walk could not start at once). The
wait check above is retail now (it ended on any finished animation: the
KeepExit walk below went on 80 units into 175). Not ported: Goto's item
argument and flag `0x1000` (no caller passes an item; only the
combat-mode moves read the flag, and those aren't retail yet).

Deviations:
- The spot is computed in double with the C library's sin/cos, where
  retail used x87 `fsin`/`fcos` (precision of the products unknown: the
  FPU control word at the time isn't known). Only a product within
  rounding of a whole number can truncate differently.
- `isatrelativedistance` on a missing object fails the expression (§2.4);
  no shipped script can reach it (the object is always `THIS`).

Checked (headless, `--quickstart --sector=…`, `--exec`):
- `ressexit`: Locke placed at (1150, 980); `if player.isatrelativedistance
  ressexit -25 18 = 1` → `0x80`; `gotorelativedistance ressexit -25 18`
  → he heads for (1200, 1098) and stops, blocked, at (1178, 1065), 40
  short (the reason the prototypes test with a 50 margin); the same `if`
  → `0x40`, with `42 -32` → `0x80`, with `= 0` → `0x80`;
  `faceobject ressexit -32` → facing 87, `faceobject ressexit` → 119.
  `gotorelativeposition ressexit 60 60 10 -20` takes the nearer second
  spot (1200, 1100). Missing object: the member → `if` `0x84` with the
  message, `gotorelativeposition` → 4 with the message; no distance → 4;
  `faceobject` → 4.
- KeepExit (2_11_12), `use keepexit` (its USE block):
  `player.GOTORELATIVEPOSITION THIS 0 -60` from (12208, 12512) → spot
  (12236, 12682); the script resumes with `STATE "OPENING"` when Locke is
  back in his root state at exactly that spot, then runs to its end.
- `ressexit`'s DOOR1 block (`ressexit.stat locked = 0; use ressexit`,
  with the exits port): from inside at (1150, 980) it takes the `-25 18`
  branch, walks to (1178, 1065) as above, the `IF … ISATRELATIVEDISTANCE` holds,
  `FACEOBJECT THIS -32` → facing 87, the door opens, the fade and
  `ACTIVATE` follow; from outside at (1260, 1230) the `42 -32` branch →
  (1220, 1149), `FACEOBJECT THIS 15` → facing 238. The block's
  `NOWAIT player.TRY "WOPENDOORIN"` answers bad parameters: the port's
  `try` reads only `%t`, retail falls back to `%s` (`0x005cb5b4`) for a
  quoted state.

### 6.6 Buy/sell: the `buysell*` family

The shop's scripts (`town.s`: Elahni, Hruthford, Gina, Cronus — 42 shops
opened, 265 lines) fill the shop pane `0x0065a3b8` and open it as
PlayScreen's bottom drawer. The pane, its paint, the price rules and the
transactions are in [BuySellScreen_SPEC.md](../../ui/forensics/BuySellScreen_SPEC.md).
A typical block:

```
buysellinit
buysellsalesperson GINA1
buysellnogolddialog II9GIN03
buysellpurchasedialog II9GIN04
choice buy1 BSBUY2 / choice sell1 BSSELL2 / choice stop1 BSEXIT2 / wait response
:buy1
buysellshoptype buy WEAPON
BuySellAddCriteria "minstrength" 1 VALUE
buysellscreen
wait buysell
jump start1
:sell1
buysellshoptype sell WEAPON
buyselladdbuyitems
buysellscreen
wait buysell
jump start1
```

Every handler answers 0 (never `CMD_WAIT`); `wait buysell` (script wait 7,
`0x0049301d`) holds the script until the shop's `+0x1b0` is 0 — its Exit,
or the drawer closing. Decomps: `recon/discovered/commands/cmd_buysell*.cpp`.

| Command | Handler → shop | Grammar | Effect | Errors |
|---|---|---|---|---|
| `buysellinit` | `0x00427080` → `0x0052f390` | — | Initialize: in use, first row 0, no selection/hover; builds the pane the first time (§9 of the spec). Rows, salesperson, dialog tags and customer are kept | — |
| `buysellsalesperson` | `0x004279f0` → `0x00532fb0` | `<name>` | `+0x1a0 = FindClosestObject(name, target, exact)` (`0x00451fe0`); may be null | not text/ident → 4 |
| `buysellnogolddialog` | `0x00427a20` → `0x00533040` | `<tag>` | the no-gold dialog tag (`+0x1a8`) | not text/ident → 4 |
| `buysellpurchasedialog` | `0x00427a50` → `0x00532fe0` | `<tag>` | the purchase tag (`+0x1a4`) | not text/ident → 4 |
| `buysellshoptype` | `0x00427870` | `buy\|sell weapon\|armor\|misc` | shop type `+0x17c`: buy 9 / 5 / 0x11, sell 10 / 6 / 0x12 | neither word → "Buy Sell Option" 4; a bad second word → "Missing Shop Type" 4 |
| `buyselladd` | `0x00427500` → `0x00530670` | `[<amount>] <type>` | stock one type (`amount` default 1, used by misc shops) | — |
| `buyselladdcriteria` | `0x00427240` → `0x00530af0` | `<stat> <min> <max>` | stock every type of the shop's class with class stat `stat` in [min, max] | not text/ident → "Name required" 4; a bound neither a number nor a known number variable → "Invalid Params" / "Invalid min/max value" 4 |
| `buysellremovecriteria` | `0x004273a0` → `0x00532340` | `<stat> <min> <max>` | drop rows whose type has `stat` in [min, max] | as above |
| `buyselladdbuyitems` | `0x00427860` → `0x00531b70` | — | the customer's sellable items, one row each | — |
| `buyselladdbuyitem` | `0x00427810` → `0x00531d70` | `<name>` | the customer's sellable items of that name | not text/ident → 4 |
| `buyselladdbuycriteria` | `0x00427550` → `0x00531fc0` | `<stat> <min> <max>` | the customer's sellable items with `stat` in [min, max] | as `buyselladdcriteria` |
| `buysellremovebuycriteria` | `0x004276b0` → `0x005321f0` = `0x00532340` | `<stat> <min> <max>` | as `buysellremovecriteria` | as above |
| `buysellremove` | `0x00427840` → `0x00532210` | `<name>` | drop the rows of that display name | — |
| `buysellscreen` | `0x00427090` | — | open the shop: the customer (`+0x1b4`) is the target if a player, else the caller if a player, else the script's user when it is aliased `user` and is a player, else none; PlayScreen `+0x6b8 = 1` (the drawer opens on its next pulse) | — |

Grammar details (retail, kept):
- `buysellshoptype` tests each word with `Is` (`0x00479700`, no
  abbreviation) and steps with `Get` + `WhiteGet`; shipped scripts write the
  words in either case.
- `buyselladd` takes a number token as the amount, then the next token as the
  type; any other first token is the type with amount 1. It doesn't check
  the type token.
- The criteria commands copy the stat name into a 32-byte buffer (retail
  overflows on a longer name; the port truncates), then read each bound as a
  number token or, for an identifier, the caller's prototype number variable
  (`0x00497800`, not found = `-20000000`). `BuySellAddCriteria "minstrength"
  1 VALUE` in `town.s` reads Gina's `VALUE`, set just before by
  `SETPROTOVARIABLE VALUE = player.STAT "LEVEL" / 3 + 16`.
- `buysellsalesperson`, the dialog commands and `buysellremove` don't consume
  their token; the interpreter skips the rest of the line.
- `buysellscreen` also writes the dialog pane's responder (`+0x194`) and
  "control on while choosing" (`+0x1e8`); single player reads neither before
  `SetWait` rewrites them (DIALOG.md §4.1), so the port leaves them alone. In a
  network game it sends the shop to the customer's machine (`0x00586a60`);
  not ported.

**Drawer.** `buysellscreen` only requests the shop. PlayScreen's pulse
(`0x0047b4d0`) re-initializes it and switches the bottom drawer to mode 3
(spec §1). Other drawer closes now reach it: `hideresponse` (`0x00426d40` →
`0x0047ecc0`, any drawer mode), `fadescreenout` and the camera calls
(`0x00427f44`, `0x00453914`, `0x0045397c`: mode 3 only), and `LoadGame`'s
reset (`0x0047ece0`, then `0x00532f40` clears the rows).

**Port.** `src/cmd_buysell.cpp` (the first per-family handler file,
ARCHITECTURE §6.1) parses each command as above and calls `BuySellPane`
(`src/buysell.{h,cpp}`) and `TPlayScreen::RequestBuySell`. Checked
(headless, `town.s`): see BURNDOWN.md T13.

## Appendix — command catalog

Generated from `RETAIL_COMMAND_TABLE.txt`, the pre-release table, and a
census of the shipped scripts ("Script uses" = command lines invoking
it, `nowait`/`<object>.` forms included).

| Command | Handler | Family | Context | Params | Editor | Script uses | Pre-release |
|---|---|---|---|---|---|---|---|
| `activate` | `0x00420100` | object | EXIT/any |  |  | 29 | yes |
| `add` | `0x004213c0` | object | none | yes |  | 7 | yes |
| `addat` | `0x00421770` | object | none | yes |  | 35 | — |
| `addmonstertype` | `0x00427ac0` | monster generator | HELPER | yes |  |  | — |
| `addnear` | `0x00421bc0` | object | any | yes |  |  | — |
| `addinv` | `0x00422000` | inventory | any | yes |  | 25 | yes |
| `addrc` | `0x004438d0` | editor/debug | none |  | yes |  | yes |
| `amb` | `0x00425940` | presentation | none | yes |  |  | yes |
| `ambcolor` | `0x00425980` | presentation | none | yes |  |  | yes |
| `ambsoundget` | `0x00420fe0` | presentation | any |  | yes |  | — |
| `ambsoundset` | `0x00420dc0` | presentation | any | yes |  |  | — |
| `animreg` | `0x00426030` | editor/debug | any |  | yes |  | yes |
| `animz` | `0x004260d0` | editor/debug | any |  | yes |  | yes |
| `attack` | `0x00420a60` | character | CHARACTER/PLAYER | yes |  |  | yes |
| `begin` | `0x0041fb50` | flow | none |  |  | 935 | yes |
| `beginfighting` | `0x00427cd0` | character | CHARACTER/PLAYER | yes |  | 10 | — |
| `biggenerate` | `0x00426d50` | editor/debug | none |  | yes |  | — |
| `block` | `0x00420a90` | character | CHARACTER/PLAYER |  |  |  | yes |
| `bounds` | `0x00423940` | editor/debug | any | yes | yes |  | yes |
| `burn` | `0x00420d50` | character | CHARACTER/PLAYER |  |  | 19 | yes |
| `busymsg` | `0x00429850` | speech/dialog | none | yes |  |  | — |
| `busysay` | `0x004298b0` | speech/dialog | none | yes |  |  | — |
| `buyselladd` | `0x00427500` | buy/sell | none | yes |  | 94 | — |
| `buyselladdbuycriteria` | `0x00427550` | buy/sell | none | yes |  | 6 | — |
| `buyselladdbuyitem` | `0x00427810` | buy/sell | none |  |  | 13 | — |
| `buyselladdbuyitems` | `0x00427860` | buy/sell | none |  |  | 8 | — |
| `buyselladdcriteria` | `0x00427240` | buy/sell | none | yes |  | 8 | — |
| `buysellinit` | `0x00427080` | buy/sell | none |  |  | 12 | — |
| `buysellnogolddialog` | `0x00427a20` | buy/sell | none | yes |  | 12 | — |
| `buysellpurchasedialog` | `0x00427a50` | buy/sell | none | yes |  | 12 | — |
| `buysellremove` | `0x00427840` | buy/sell | none | yes |  |  | — |
| `buysellremovebuycriteria` | `0x004276b0` | buy/sell | none | yes |  | 8 | — |
| `buysellremovecriteria` | `0x004273a0` | buy/sell | none | yes |  | 8 | — |
| `buysellsalesperson` | `0x004279f0` | buy/sell | none | yes |  | 12 | — |
| `buysellscreen` | `0x00427090` | buy/sell | none |  |  | 24 | — |
| `buysellshoptype` | `0x00427870` | buy/sell | none | yes |  | 24 | — |
| `calcwalk` | `0x004222f0` | editor/debug | none |  | yes |  | yes |
| `cast` | `0x004210f0` | character | CHARACTER/PLAYER | yes |  | 3 | yes |
| `centeron` | `0x00424db0` | presentation | any |  |  | 17 | yes |
| `choice` | `0x004282f0` | speech/dialog | none | yes |  | 218 | yes |
| `cleanids` | `0x00428a30` | flow | none |  |  |  | — |
| `combat` | `0x00420980` | character | CHARACTER/PLAYER | yes |  | 136 | yes |
| `control` | `0x00420ab0` | presentation | none |  |  | 353 | yes |
| `createmodule` | `0x00428c30` | editor/debug | none | yes | yes |  | — |
| `curplayer` | `0x00423770` | multiplayer | PLAYER |  |  |  | yes |
| `delete` | `0x00423fd0` | object | any |  |  | 30 | yes |
| `delinv` | `0x004220f0` | inventory | any |  |  | 59 | yes |
| `delmonstertype` | `0x00427b60` | monster generator | HELPER | yes |  |  | — |
| `delrc` | `0x00444790` | editor/debug | none |  | yes |  | yes |
| `deselect` | `0x00421360` | editor/debug | none |  | yes |  | yes |
| `dispinv` | `0x00422070` | inventory | any |  | yes |  | — |
| `drop` | `0x004281c0` | inventory | PLAYER | yes |  |  | — |
| `dumptaglist` | `0x004287c0` | editor/debug | any | yes | yes |  | — |
| `dumptaglisterrors` | `0x004288f0` | editor/debug | any | yes | yes |  | — |
| `dxstats` | `0x00426670` | debug | none |  |  |  | yes |
| `else` | `0x0041fbb0` | flow | none |  |  | 19 | yes |
| `end` | `0x0041fb60` | flow | none |  |  | 935 | yes |
| `endfighting` | `0x00427d30` | character | CHARACTER |  |  |  | — |
| `endgame` | `0x00427060` | game flow | none |  |  | 1 | — |
| `equip` | `0x00428140` | inventory | PLAYER | yes |  | 2 | — |
| `exit` | `0x00423040` | object | EXIT | yes | yes |  | yes |
| `extents` | `0x004237b0` | editor/debug | any |  | yes |  | yes |
| `face` | `0x00420800` | character | any | yes |  | 36 | yes |
| `faceobject` | `0x00420840` | character | any | yes |  | 17 | — |
| `fadecharacterin` | `0x00428070` | presentation | none | yes |  | 18 | — |
| `fadecharacterout` | `0x00428020` | presentation | none | yes |  | 27 | — |
| `fadescreenin` | `0x00427f60` | presentation | none |  |  | 112 | — |
| `fadescreenout` | `0x00427e80` | presentation | none |  |  | 112 | — |
| `flip` | `0x00426220` | object | any |  |  |  | yes |
| `follow` | `0x004230e0` | object | EXIT |  | yes |  | yes |
| `force` | `0x00422cc0` | character | any | yes |  | 1 | yes |
| `forget` | `0x004282e0` | flow | none |  |  |  | — |
| `fow` | `0x00425440` | presentation | none |  |  |  | — |
| `frame` | `0x00422bf0` | object | any | yes |  |  | yes |
| `gamaps` | `0x00425390` | editor/debug | none | yes | yes |  | — |
| `gamapw` | `0x004252f0` | editor/debug | none | yes | yes |  | — |
| `generate` | `0x00426b20` | editor/debug | none |  | yes |  | yes |
| `get` | `0x00423120` | inventory | any |  |  | 1 | yes |
| `getitemamount` | `0x00426be0` | inventory | CHARACTER/any | yes |  |  | — |
| `getitemname` | `0x00426d60` | inventory | any | yes |  |  | — |
| `getitemvalue` | `0x00426e20` | inventory | any | yes |  |  | — |
| `getstate` | `0x00421040` | character | CHARACTER/PLAYER |  |  |  | yes |
| `give` | `0x00422190` | inventory | any | yes |  | 1 | yes |
| `giveweapons` | `0x00422150` | character | CHARACTER/PLAYER | yes |  | 2 | — |
| `go` | `0x004204b0` | character | CHARACTER/PLAYER | yes |  |  | yes |
| `goto` | `0x004204f0` | character | CHARACTER/PLAYER | yes |  | 99 | yes |
| `gotorelativedistance` | `0x00420710` | character | CHARACTER/PLAYER | yes |  | 29 | — |
| `gotorelativeposition` | `0x004205c0` | character | CHARACTER/PLAYER | yes |  | 86 | — |
| `group` | `0x004235a0` | editor/debug | any |  | yes |  | yes |
| `groupface` | `0x00429390` | multiplayer | PLAYER | yes |  |  | — |
| `groupgoto` | `0x00429480` | multiplayer | PLAYER | yes |  |  | — |
| `groupinrange` | `0x00428f50` | multiplayer | PLAYER |  |  |  | — |
| `grouppos` | `0x00429590` | multiplayer | PLAYER | yes |  |  | — |
| `hasfreeslot` | `0x00426c60` | inventory | CHARACTER/any | yes |  |  | — |
| `haslevel` | `0x00429180` | multiplayer | PLAYER/any | yes |  |  | — |
| `help` | `0x0041ef50` | editor/debug | none |  | yes |  | yes |
| `hasplayer` | `0x00429060` | multiplayer | PLAYER/any | yes |  |  | — |
| `hideobjects` | `0x00427010` | presentation | none | yes |  |  | — |
| `hideresponse` | `0x00426d40` | speech/dialog | none |  |  |  | — |
| `hasnumplayers` | `0x00429290` | multiplayer | PLAYER/any | yes |  |  | — |
| `if` | `0x0041fb70` | flow | none | yes |  | 447 | yes |
| `incidentals` | `0x00428250` | presentation | CHARACTER/PLAYER | yes |  | 10 | — |
| `jump` | `0x00420c70` | flow | none | yes |  | 186 | yes |
| `jumpclass` | `0x004296c0` | flow | any | yes |  |  | — |
| `jumpname` | `0x00429770` | flow | any | yes |  |  | — |
| `knockback` | `0x00420d00` | character | CHARACTER/PLAYER | yes |  |  | yes |
| `level` | `0x00422d70` | object | none | yes |  |  | yes |
| `light` | `0x00425c10` | object | any | yes |  |  | yes |
| `load` | `0x00426270` | editor/debug | none |  | yes |  | yes |
| `loadgame` | `0x00428540` | game flow | none | yes |  |  | — |
| `lock` | `0x00426230` | object | any |  | yes |  | yes |
| `map` | `0x00425170` | editor/debug | none | yes | yes |  | — |
| `mapindex` | `0x00426ed0` | object | any |  | yes |  | — |
| `maxmonsters` | `0x00427c30` | monster generator | HELPER |  |  |  | — |
| `memory` | `0x004267f0` | debug | none |  |  |  | yes |
| `message` | `0x004280e0` | speech/dialog | none | yes |  | 27 | — |
| `mono` | `0x004259f0` | presentation | none | yes |  |  | yes |
| `monstertypes` | `0x00427bd0` | monster generator | HELPER |  |  |  | — |
| `move` | `0x00423c40` | object | any | yes |  | 6 | yes |
| `name` | `0x00426150` | object | any | yes |  |  | yes |
| `newgame` | `0x00423760` | game flow | none |  |  |  | yes |
| `operate` | `0x00426cd0` | object | EXIT | yes |  | 22 | — |
| `pivot` | `0x004208c0` | character | CHARACTER/PLAYER | yes |  | 72 | yes |
| `pivotobject` | `0x00420900` | character | any | yes |  | 264 | — |
| `play` | `0x00423610` | presentation | none | yes |  | 16 | yes |
| `play3d` | `0x004236a0` | presentation | any | yes |  |  | yes |
| `playerlevel` | `0x00428640` | character | PLAYER | yes |  | 1 | — |
| `playmovie` | `0x00427d80` | game flow | none | yes |  | 3 | — |
| `pos` | `0x00423d40` | object | any |  |  | 137 | yes |
| `pulp` | `0x00420ca0` | character | CHARACTER/PLAYER | yes |  |  | yes |
| `random` | `0x00427d40` | flow | none | yes |  |  | — |
| `reg` | `0x00425f80` | editor/debug | any |  | yes |  | yes |
| `reloadstates` | `0x00428a20` | flow | none |  |  |  | — |
| `replace` | `0x00425aa0` | object | any | yes |  |  | yes |
| `restore` | `0x00423110` | character | CHARACTER/PLAYER |  |  |  | yes |
| `reveal` | `0x00422d40` | object | any |  |  |  | yes |
| `rotate` | `0x00423f20` | editor/debug | any |  | yes |  | yes |
| `samap` | `0x00425420` | presentation | none |  |  |  | — |
| `save` | `0x00426360` | editor/debug | none |  | yes |  | yes |
| `savegame` | `0x004285b0` | game flow | none | yes |  |  | — |
| `savelevelsectors` | `0x00426350` | editor/debug | none |  | yes |  | — |
| `savetilebm` | `0x00444bf0` | editor/debug | none |  | yes |  | yes |
| `say` | `0x00420140` | speech/dialog | CHARACTER/PLAYER | yes |  | 1753 | yes |
| `script` | `0x004247e0` | editor/debug | none |  | yes |  | yes |
| `scrollto` | `0x00424dd0` | presentation | any |  |  | 51 | yes |
| `sectorcommand` | `0x004231b0` | editor/debug | none | yes | yes |  | yes |
| `select` | `0x004211a0` | editor/debug | none | yes | yes |  | yes |
| `set` | `0x0041fc00` | flow | none | yes |  | 233 | yes |
| `setcdvolume` | `0x00428b20` | presentation | none | yes |  | 138 | — |
| `setcurmodule` | `0x00428d40` | editor/debug | none | yes | yes |  | — |
| `setcurrent` | `0x00428e50` | flow | none | yes |  |  | — |
| `setdrip` | `0x00420d60` | object | any | yes |  |  | yes |
| `setfromexit` | `0x00428a40` | object | EXIT |  |  | 28 | — |
| `setprotovariable` | `0x0041fd70` | flow | none | yes |  | 27 | — |
| `short` | `0x00428ab0` | editor/debug | none | yes | yes |  | — |
| `show` | `0x00425770` | presentation | none | yes |  |  | yes |
| `showobjects` | `0x00426fc0` | presentation | none | yes |  |  | — |
| `size` | `0x00426f30` | object | any | yes |  |  | — |
| `smoothscroll` | `0x00422f20` | presentation | none | yes |  |  | yes |
| `specificattack` | `0x00427c80` | character | CHARACTER | yes |  | 22 | — |
| `stat` | `0x00424010` | object | any |  |  | 56 | yes |
| `state` | `0x004227b0` | object | any | yes |  | 130 | yes |
| `statmod` | `0x00428200` | character | PLAYER | yes |  |  | — |
| `stop` | `0x00420c50` | character | CHARACTER/PLAYER |  |  | 181 | yes |
| `swap` | `0x004234e0` | object | any | yes | yes |  | yes |
| `swapcdtrack` | `0x00428b90` | presentation | none | yes |  |  | — |
| `take` | `0x00422230` | inventory | any | yes |  |  | yes |
| `template` | `0x00422dd0` | editor/debug | any |  | yes |  | yes |
| `test` | `0x00428690` | editor/debug | none |  | yes |  | — |
| `text` | `0x00422ff0` | object | SCROLL |  | yes |  | yes |
| `textdump` | `0x00428a70` | editor/debug | none | yes | yes |  | — |
| `tilewalk` | `0x00422760` | editor/debug | TILE |  | yes |  | yes |
| `timelimit` | `0x00429820` | flow | none | yes |  |  | — |
| `timeofday` | `0x00427a80` | presentation | none |  |  |  | — |
| `toback` | `0x00423580` | editor/debug | any |  | yes |  | yes |
| `tofront` | `0x00423560` | editor/debug | any |  | yes |  | yes |
| `toggle` | `0x00423a90` | object | any | yes |  | 158 | yes |
| `trigger` | `0x00423710` | flow | any | yes |  |  | yes |
| `try` | `0x00422c30` | character | any | yes |  | 190 | yes |
| `undo` | `0x004237a0` | editor/debug | none |  | yes |  | yes |
| `unequip` | `0x00428180` | inventory | PLAYER | yes |  | 10 | — |
| `unlock` | `0x00426250` | object | any |  | yes |  | yes |
| `use` | `0x00420050` | object | any |  |  | 63 | yes |
| `visible` | `0x004210b0` | character | CHARACTER/PLAYER | yes |  |  | yes |
| `wait` | `0x0041fe30` | flow | any |  |  | 874 | yes |
| `walkcopy` | `0x004223c0` | editor/debug | none | yes | yes |  | — |
| `walkmap` | `0x00422300` | editor/debug | any | yes | yes |  | yes |
| `while` | `0x0041fbc0` | flow | none | yes |  |  | yes |
| `zoffset` | `0x00425eb0` | editor/debug | any |  | yes |  | yes |
