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
Conditions (`if`, `while`) go through one evaluator, `0x0041f230`
(not yet decompiled for this write-up).

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
errors through it; scripts never see the text.

## 3. Pre-release vs retail

- Handler signature: pre-release `(context, token)`; retail adds the
  calling context and the running script.
- Retail adds 93 commands (appendix), the `0x4000` speech-wait result,
  the script-side wait machine (pre-release waited on the character's
  action queue: `TCharacter::Wait/WaitChar/WaitResponse`), the alias /
  `this` / `user` / `target` / `current` / `party<N>` resolver (pre-
  release: `FindClosestObject` only), and an error result for an
  unresolved context.

## 4. Port state (2026-10-04, `feature/gameflow`)

- `Commands[]` mirrors the retail table (order, contexts, flags, usage
  text) and handlers take the retail argument list. Missing commands are
  stubs (`CmdNotPorted`) that **report success without doing the work —
  below the bar; to be replaced** by an honest "not ported" result.
- `--exec` (`src/consoleexec.cpp`) is a separate command queue —
  **below the bar** (duplicates the console's job).
- Interpreter and resolver are still pre-release.

## 5. Open questions for the author

- `timelimit`'s usage text is the `script edit/pause/resume/end`
  help — a leftover, or is it really the script-control command?
- `0x4000` vs `CMD_WAIT`: was the speech wait added specifically so
  `nowait say` could still let the script continue while the speaker
  talks?

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
