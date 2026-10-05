# Dialog runtime — forensics

How retail Revenant runs dialog: the script commands (`say`, `choice`,
`wait response`, `message`, …), the script's side of a conversation, how
a character speaks (action, voice, text), the dialog list, and the
contract between the runtime and `TDialogPane`. Input to the dialog port
([../OPENING_SEQUENCE.md](../OPENING_SEQUENCE.md) §5 step 5). The pane's
look belongs to the UI track
([../../ui/forensics/DialogPane_SPEC.md](../../ui/forensics/DialogPane_SPEC.md));
§4.6 lists what this pass changes in that spec.

Sources: Ghidra (retail `Revenant.exe`, authoritative); the 1998 source
(`/Users/benjamincooley/projects/Revenant/`, pre-release baseline);
shipped data (`resources.rvr`, `Modules/Ahkuilon.rvm`). Decomps cited
here are in [`recon/discovered/dialog/`](../../../recon/discovered/dialog/)
(file name = role + address) and `recon/discovered/commands/`.

Confidence per claim: **C** confirmed (read in decomp or disassembly),
**I** inferred (strong evidence, not read end to end), **U** unknown.

Single player throughout. Multiplayer paths are named where they branch
off and not described further.

## 0. The retail model in brief

- There is no dialog *panel*. `TDialogPane` (global `0x00667cc8`) is a
  manager of floating **entries** (0x160-byte objects, ctor
  `0x00533f10`): one per spoken line, plus one **response entry** that
  holds the current choice list. NPC lines stack down from the top of the
  map view; the player's lines and the response entry stack up from the
  bottom. Each entry shows the speaker's portrait in a ring and the
  wrapped text, fades in and out, and times out. **C**
- `TCharacter::Say` (`0x004d0610`) plays the voice (`Sound/<Language>/<tag>.mp3`),
  starts a `say` action that lasts the voice length + ½ s, and adds a
  speech entry to the pane. The script waits for the speaker to get back
  to idle (wait type 8). **C**
- `choice <label> <tag>` adds to the pane's choice list. `wait response`
  opens the response entry (one invisible button per choice), turns
  player control off, and waits. A click or key `1`–`6` commits the
  choice; the script's wait check jumps to the choice's label; the pane
  then restores control and fades the response entry. **C**
- The script's own choice list (`TScript +0xb8`) and response byte
  (`+0xb6`) are the multiplayer path only. **C**

## 1. Commands

Token types (COMMAND_SYSTEM §2.4): 2 quoted text, 4 identifier, 8
number, 9 end of line, 10 end of input. Handler signature
`(target, token, context, script)`.

### 1.1 `say` — `0x00420140` (context CHARACTER / PLAYER)

```
[<speaker>.]say [nowait] [<frames>] {anim <state> | sound <name>}*
                (choice | "<text>" | <TAG>) {<text-part>}*
```

| Step | Behaviour | Conf. |
|---|---|---|
| `nowait` | Must be the first parameter (string `0x005cae68`). Suppresses the speech wait at the end. Separate from the interpreter's `NOWAIT` prefix — see the quirk below. | C |
| `<frames>` | A number: the action/entry duration in ticks. Default −1 (computed, §3.2). | C |
| `anim <state>` (`0x005cae70`/`0x005cae80`) | Animation state for the say action (≤31 chars), default `"say"`. Shipped: `sitsay`, `scaredsay`, `combat`. | C |
| `sound <name>` (`0x005cae78`/`0x005cae88`) | Voice to play with quoted text (≤31 chars). Forces duration −1 (the voice length wins). Ignored with a tag. | C |
| `anim`/`sound` | Loop: any number, any order, each consumes keyword + value. | C |
| `choice` (`0x005cae90`) | The chosen response's **text** (`DialogPane +0x198[+0x1dc]`, a tag) is looked up as a tag and said with its voice. No response yet → falls through with no new text (says whatever is in the shared buffer `0x00654a88`; retail hazard). | C |
| `"text"` | Copied to the shared buffer `0x00654a88`; said as plain text. | C |
| `TAG` | Copied to `0x00654a88`, looked up (`TDialogList::FindLine` `0x0049d6d0`). Found → said by index (text + voice named by the tag). Not found → the identifier itself is said as plain text. | C |
| text parts | Until end of line: quoted text is appended; an identifier that is a prototype variable appends its value (number in decimal, or string); other tokens are skipped. Only the plain-text path uses the appended text. | C |
| speak | Index path: `TCharacter::SayIndex(idx, frames, anim)` `0x004d09b0`. Text path: `TCharacter::SayText(buf, frames, anim, sound)` `0x004d0950`. Both → `Say` `0x004d0610` (§3). | C |
| wait | Unless `nowait`, and if there is a calling context: `context->WaitSay(speaker)` (`0x00471330` → script `SetWait(8, speaker)`). | C |
| result | Always 0. `say` never returns `CMD_WAIT` (1998 did). | C |

Quirk (retail): `NOWAIT player.say X` (10 shipped uses) **still waits** —
the interpreter's `NOWAIT` only turns result 1 into 0
(`CommandInterpreter_41e8e0.cpp:283`), and `say` sets the speech wait
itself. **C** (code path). Whether this was intended: §7 Q3.

### 1.2 `choice` — `0x004282f0` (context none)

```
choice <label> <text-part> {<text-part>}*
```

| Step | Behaviour | Conf. |
|---|---|---|
| label | Must be an identifier, else result 4. Copied unbounded into an 80-byte buffer. | C |
| first part | Must be quoted text or identifier, else result 4. | C |
| parts | Until end of line, into a 256-byte buffer: quoted text appended raw; identifier = prototype number (decimal) or string variable appended; any other identifier **replaces** the buffer with `DialogLine(ident)` (`0x00533dd0`, §3.7 — in practice the identifier itself); numbers skipped. | C |
| run from a script | `script->AddChoice(label, text)` `0x004932a0` and `script.flags |= 4` (`*param_4 |= 4`). In single player `AddChoice` forwards to `DialogPane.AddChoice` (§2.2). | C |
| console | `DialogPane.AddChoice(label, text)` `0x00535870` directly. | C |
| result | 0. | C |

The stored "text" is the dialog **tag** (`I1LOC01`); the response entry
resolves it when built (§4.2). Shipped forms: `choice <label> <TAG>` (215),
`choice <label> "<TAG>"` (3, same effect).

### 1.3 `message` — `0x004280e0`

```
message ("<text>" | <TAG>)
```

Quoted text, or `TDialogList::GetLine(tag)` (`0x0049d800`, never NULL —
a miss gives `"[TAG]"`), printed with `TTextBar::Print`
(`0x0054d170` on `TextBar` `0x0065c5d0`): `vsprintf` into 256 bytes
(the text is the format string), split at `\n`, one text-bar line each,
also appended to `TextDump.txt` when text dumping is on. Any other token:
"There was no message to display!" and result 2. Reads only the first
token. **C**. Shipped: 18 tags (`JONGM1`… combat-move hints, all in the
base `english.def`) and `"EXCELLENT !"` ×9; none contains `%`.

### 1.4 `hideresponse` — `0x00426d40`

Calls `0x0047ecc0` on PlayScreen: if the bottom drawer is open
(`+0x6ac`), request it closed (`+0x6b4 = 1`). Returns 0. **C**

The drawer is PlayScreen's slide-up bottom area (`TPlayScreen::Pulse`
`0x0047b4d0`, `+0x6a0..+0x6c4`): mode 1 = editor/console (140 px;
opened by `0x0043d410`, closed by `0x0043d6c0`, which writes the
`[Editor]` ini section), 2 = HUD panes,
3 = buy/sell (178 px, `buysell.dat`, `0x0052f390`). **I** (modes from the
open/close calls). Retail responses never use the drawer, so in practice
`hideresponse` closes the buy/sell panel. The name fits the 1998 design,
where the dialog pane replaced the lower panes. No shipped script uses
it. The camera calls `0x004538d0`/`0x00453940` and `fadescreenout`
`0x00427e80` do the same close, but only in mode 3. **C**

### 1.5 `busysay` `0x004298b0`, `busymsg` `0x00429850`

```
busysay ("<text>" | <TAG>)        busymsg ("<text>" | <TAG>)
```

Script only (no script → result 4). A tag must exist (`FindLine ≥ 0`)
else result 4.

| | Stores on the script | Conf. |
|---|---|---|
| `busysay` | `+0xd8 = strdup(text)`, `+0xdc = strdup(tag)` (voice) or NULL for quoted text (`0x00494530`) | C |
| `busymsg` | `+0xe0 = strdup(text)` (`0x004944c0`) | C |
| both | `flags &= 0x10000` / `flags &= 0x20000` — an AND, so all taken flags (§2.6) are cleared. Probably meant `|=`. | C (code); intent U |

Used when someone else triggers a script that is busy with another user
(`TScript::Busy` `0x00494620`, called from the manual-trigger request
`0x00492640` when the trigger-user guard `+0x10` is set to another
object). Effects (§2.5) are multiplayer except the deferred say. In
single player the only requester is the player, who is the guard, so
`Busy` never fires. **I**. No shipped script uses either command.

### 1.6 `wait` response forms — `0x0041fe30`

| Keyword | Wait type | Conf. |
|---|---|---|
| `response` (`0x005cadd0`) | 2 | C |
| `responsenohide` (`0x005caddc`) | 2 — identical to `response` | C |
| `respnohide` (`0x005cadec`) | 2 — identical | C |
| `respctrlon` (`0x005cadf8`, re-tested at `0x005cae04`) | 10 — player control stays on | C |

`context->SetWait(type, 0)` (`0x004712b0`), then result 1. The script is
already waiting by then, so the interpreter's `CMD_WAIT → WaitChar`
post-hook does nothing. Type 5 (also a response wait) has no caller in
single player; only the multiplayer relay `0x0058338f` passes a
network-supplied type. **C**

### 1.7 Shipped usage (all module scripts + `master.s`)

| Form | Uses |
|---|---|
| `say TAG` / `<obj>.say TAG` | 981 / 686 |
| `say anim <state> TAG` / `<obj>.say anim <state> TAG` | 74 / 1 |
| `NOWAIT <obj>.say TAG` / `NOWAIT say TAG` | 10 / 1 |
| `choice <label> TAG` / `choice <label> "TAG"` | 215 / 3 |
| `wait response` / `wait respctrlon` | 85 / 1 |
| `message TAG` / `message "text"` | 18 / 9 |
| `say` with number, `sound`, `choice`, quoted text, `say nowait` | 0 |
| `busysay`, `busymsg`, `hideresponse`, `responsenohide`, `respnohide` | 0 |

Scripts answer a choice with `player.say <TAG>` after the label, not
`say choice`.

## 2. The script side

### 2.1 Fields (`TScript`, 0xe8 bytes; SCRIPT_ENGINE §2)

| Offset | Dialog use | Conf. |
|---|---|---|
| +0x00 | taken flags; bit 4 = "issued choices" (set by `choice`, cleared when the response is taken, cleaned up by `End`) | C |
| +0xb4 | wait type (2/5/10 response, 8 speech) | C |
| +0xb5 | multiplayer wait countdown (0x78) | C |
| +0xb6 | response index from a remote player (`0x00492c40`) — MP | C |
| +0xb8 | choice list for a remote player — MP: `malloc(0x108)` `{int kind=1; int count; char label[8][32];}` | C |
| +0xbc | wait object (for responses: the player answering) | C |
| +0xc4 / +0xcc | user / user alias (`"user"`) — decides who answers | C |
| +0xd8 / +0xdc | busy line text / its voice tag (`busysay`) | C |
| +0xe0 | busy message (`busymsg`) | C |
| +0xe4 | speaker of a deferred busy line | C |

### 2.2 `TScript::AddChoice(label, text)` — `0x004932a0`

Forwards to `DialogPane.AddChoice(label, text)` unless **all** hold:
multiplayer (`0x0066829c`), host (`0x0067682c`), the user (`+0xc4`) is a
player other than the local one, and the user alias (`+0xcc`) is
`"user"`. Otherwise (MP): appends the label (≤31 chars, max 8) to
`+0xb8` and sends the choice to that player (`0x00586f80`). **C**

### 2.3 Opening the responses — `SetWait` `0x00492b00`

```
SetWait(type, param):
  if waittype != 0: return
  waiter = (alias == "user" && user && user is PLAYER) ? user : Player
  if type in {2,5,10} and waiter == Player:
      if DialogPane+0x190 (response entry) != 0: return     // NO wait is set
      DialogPane+0x1e8 = (type == 10)                       // control on while choosing
      DialogPane+0x194 = waiter
      DialogPane.ShowResponses()                            // 0x00535e90, result ignored
  waittype = type
  waitobj  = (type in {2,5,6,7,10}) ? waiter : param
  (MP: notify the remote waiter, countdown 0x78)
```
**C**. Two hazards: a second `wait response` while a response entry is up
returns without waiting (the script runs on into its first label); a
`wait response` with no choices waits forever (`ShowResponses` needs
`numchoices > 0`).

### 2.4 Taking the response — wait check `0x00492d70`, types 2/5/10

Single player (or the waiter is the local player):
```
if DialogPane+0x1e4 (responded, DAT_00667eac):
    flags &= ~4
    label = DialogPane+0x1b8[ DialogPane+0x1dc ]           // DAT_00667e80[DAT_00667ea4], ≤31 chars
    if label[0]: Jump(owner, label)                         // 0x00493fa0
    satisfied
```
The script does **not** clear `+0x1e4`; the pane does, in its own pulse
(§4.3 `Pulse`). Order within a tick (**C** for the pass, **I** that the map
pane's pulse runs the scripts): `TScreen` pulses panes in add order
(`0x0048fda0`); PlayScreen adds `MapPane` (`0x006668d8`) before
`DialogPane` (`TPlayScreen::Initialize` `:240-241`). So the script sees
the commit first, then the pane consumes it. If the waiting script is
not pulsed that tick the response is lost — the port should hand the
response to the script explicitly instead of relying on pass order.

Multiplayer, remote waiter: after the countdown is cleared by the reply
(`0x00492c40` sets `+0xb6`, `+0xb5 = 0`), `label = +0xb8.label[min(+0xb6,
count−1)]`, free the list, jump. **C**

### 2.5 The speech wait (type 8) and the deferred busy line

Wait check, type 8, object `o` (**C**):

| `o` | Satisfied when |
|---|---|
| NULL (deleted) | the caller's own command is done (`Continue`'s argument) |
| not a complex object (`flags+8 & 0x20000` clear) | `o->CommandDone()` (vtable 0x154, `+0x80`) and its current animation state has flag 1 |
| complex object (characters) | `doing == root` and `desired == root` (vtable 0x204 / 0x1f4) and `doing` is not transitioning (`+0x60 & 2`) |

So the script resumes when the speaker is idle in its root state, not
when the say action merely ends. The 1998 equivalent used
`CommandDone`/`IsTalking`.

Deferred busy line (`Continue` `0x004933d0`, after the wait gate):
```
if +0xe4 && +0xd8:
    (+0xe4)->SayText(+0xd8, -1, NULL, +0xdc)
    SetWait(8, +0xe4); +0xe4 = 0; return
```
`+0xe4` is set by `TScript::Busy(user)` when the script is waiting on its
owner's action or speech (types 3/8): the owner says the busy line as
soon as that wait ends. Otherwise, in MP, the owner says it at once.
`busymsg` text goes to the requesting player's text bar (MP only). **C**

### 2.6 Taken flag 4 and `End` — `0x00493e40`

`End` clears the wait and the MP list, then gives back what the script
took. Flag 4: `DialogPane.SkipSpeech()` (`0x00536010` — stop every
speaker's voice and fade their entries, only when no response entry is
up), then clear the bit. Flag 1 → control on; 8 → camera back on the
player. **C**. Because the response clears flag 4, `End` only does this
when a script ends between `choice` and its response (no `wait
response`, or the block was cut off). `End` never touches the response
entry or the choice list.

## 3. Speech

### 3.1 `TCharacter::Say(text, frames, anim, sound)` — `0x004d0610`

| # | Step | Conf. |
|---|---|---|
| 1 | `text == NULL` or health ≤ 0 (vtable 0x1c0) → return 0. | C |
| 2 | `+0x260` (current voice) = −1. If `sound` and `PlaySpeech` (`0x005d7a60`, ini `[Options] PlaySpeech`, default Yes): `id = SoundPlayer.FindSound(sound)` (`0x0049c430`); `+0x260 = id`; load (`0x0049b650`) and play **non-positional** at volume 0x7f (`0x0049b990(id, 0x7f, 1, NULL, 0x50, 700)`). MP remote speaker: positional, only within 0x400 and on the listener's level. | C |
| 3 | `DialogLine(text)` → 256-byte line (§3.7). | C |
| 4 | New action block (`0x004da9f0`, 100 bytes): name = `anim` or `"say"`, action = 0x10 (`ACTION_SAY`). | C |
| 5 | Action text `+0x5c` = `strdup(line)`, unless the voice played and `ShowDialog` (`0x00668188`, ini `[Options] ShowDialog`; exe default No, the shipped ini says Yes) is off → NULL. | C |
| 6 | Duration `+0x28` (§3.2). Flags `+0x60 |= 0x800` (1998 set `loop`). | C (flag meaning I) |
| 7 | `TryCommand(action, 0, 0)` (vtable 0x218): start the say animation. | C |
| 8 | Single player (or local speaker): `DialogPane.AddSpeech(this, line, duration)` (`0x00535b90`) — **always**, whatever `ShowDialog` says. | C |
| 9 | Return 1. | C |

Wrappers: `SayText` `0x004d0950` (Say + MP broadcast `0x00586e60`);
`SayIndex(idx, frames, anim)` `0x004d09b0` (text = `GetLine(idx)`,
sound = `GetTag(idx)` — the voice is named by the tag; MP `0x00586f00`);
`SayTag(tag, frames, anim)` `0x004d0a20` (used by buy/sell `0x0052ff40`
and MP). **C**

### 3.2 Duration (ticks, 24 Hz)

| Case | `wait` | Conf. |
|---|---|---|
| `frames ≥ 0` given | `frames` (wins even when a voice plays — tag path) | C |
| voice playing | `12 + trunc(len_ms × 24 / 1000)` — `len_ms` from the started sample (`0x0049c640`, `AIL_sample_ms_position` total); constants `0.001f` `0x005a3570`, `−24.0f` `0x005a7dc4` | C |
| no voice (or length 0) | `2 × strlen(line) + 36` | C |

1998: voice length × 24 / 100 (sic), else `20 + max(15, len × 5/4)`.

### 3.3 How the say action ends

- `TCharacter::UpdateAction` (vtable 0x210, `0x004c3260`, tail
  `0x004c346d`) decrements `doing->wait` once per tick while > 0. **C**
- `ResolveSay` (vtable 0x340, `0x004c8400`): while `wait > 0` and the
  current action's stop flag (`doing+0x60 & 0x100`) is clear → keep
  going. Else `wait = 0`, `TryCommand(root, 0, (this+0x110 & 2) ? 1 : 0)`
  → back to idle. **C**
- `StopTalking` `0x004d6000`: stop the voice (`SoundPlayer.Stop(+0x260)`
  `0x0049bd90`) and set `forcecommanddone` (`+0x108`); the next
  `TCharacter::Pulse` (`0x004c1bb0`) zeroes `doing->wait`. Used by the
  pane's skip paths and the MP response. **C**
- No `IsTalking` method is called by the runtime; the dead ESC path
  (§4.3) uses `TActionBlock::Is("say")` (`0x004dab80`) inline. **C**

### 3.4 Voice files

| | Retail | Conf. |
|---|---|---|
| Name | The voice is the sound whose name is the dialog tag (`SayIndex` passes the tag). Lookup by `bsearch` over the sound list (`0x0049c430`). | C |
| Resources | `<ResourcePath>sound\<Language>\` scanned at sound init (`0x0049afd0`, also `sound\effects\`; then the imagery path). | C |
| Module | `<ModulesPath><module>\sound\<Language>\` (and `\sound\effects\`) scanned at module mount (`0x0049b220`, called from `0x004609f0` right after the module dialog list). | C |
| `Language` | ini `[Language] Language`, default `English` (`0x00484500`), global `0x0065bc18`. | C |
| Shipped files | `Ahkuilon.rvm`: `Sound/english/<tag>.mp3`, 1,743 files (1,735 of 2,432 module tags voiced; every `I1*` tag). `resources.rvr`: 6 `.mp3` (`GO*`, death voices — not dialog tags). Stored uncompressed. | C |
| Decode | Miles (`AIL_set_named_sample_file`); MP3 via `mp3dec.asi` shipped next to the exe. | I |

### 3.5 How the text is shown

In the dialog pane: `Say` step 8 adds a speech entry (§4). **C**. No
retail counterpart of the 1998 over-head text (`TCharacter::Animate` →
`AddPostCharText` while `doing->wait > 12`) was found: no compare of an
action's `wait` against 12 in memory form, and nothing found reading a
say action's `+0x5c` copy (the one `ShowDialog` gates) to draw it.
**U** — a dosbox-x look at a voiced line settles it. The text bar shows
only `message` output and script errors.

### 3.6 `TDialogList` — global `0x0065d4d0`

Two tables of `{char* tag; char* line}` (8 bytes), each a pointer array:

| Table | Fields | Loaded by | File | Conf. |
|---|---|---|---|---|
| base | count `+0x00`, array `+0x10` | `0x0049ceb0`, from `WinMain` `0x00486bb0` | `<ClassDefPath><Language>.def` — `Resources\english.def` (1,182 tags: UI text, item names, messages) | C |
| module | count `+0x14`, array `+0x24` | `0x0049d2a0`, at module mount `0x004609f0` | `<ModulesPath><module>\<Language>_dialog.def`, else `…\<Language>.def`, else `…\english.def` — `Ahkuilon\english.def` (2,432 tags) | C |

- Parse: `DefineGet` header, then per line `Parse("%63t %4091s")` (tag ≤63,
  line ≤4091), tag upper-cased (`_strupr`), then `qsort` by tag
  (`0x0058c9ff`, comparator `0x0049d190`). **C**
- `FindLine(tag)` `0x0049d6d0`: copy ≤39 chars, upper-case, `bsearch`
  base then module. Index space: base `[0, n₀)`, module `[n₀, n₀+n₁)`;
  −1 if absent. Base wins on a duplicate tag (none overlap in the
  shipped files; the module file has a few internal duplicates, e.g.
  `I1TEN19`, where `bsearch` picks one arbitrarily). **C**
- `GetLine(id)` `0x0049d780` / `GetTag(id)` `0x0049d7c0`: out of range →
  `"[badid]"`, never NULL. **C**
- `GetLine(tag)` `0x0049d800`: miss → `sprintf(static 0x006687c0, "[%s]",
  tag)`, never NULL. **C**

### 3.7 `DialogLine(src, dst, n)` — `0x00533dd0`

Intended (1998): copy, replacing `[me]` with the player's name and `[chr]`
with the dialog context's name (`SetDialogContext` `0x00533dc0`, set by
the interpreter to each command's context); `[[`/`]]` escape. Retail
inverts the lead-byte test (`TEST AL,AL; JGE` at `0x00533df6`): every
ASCII byte is copied **two at a time** and the `[` branch is reachable
only for bytes ≥ 0x80, so substitution never happens; an odd-length
string over-reads past its NUL (output still ends at the NUL). **C**. No
shipped line or script contains `[`, so the output equals the input.

## 4. The pane contract

### 4.1 `TDialogPane` — `0x00667cc8`, vtable `0x005a5c60`, 0x1f0 bytes

| Offset | Field | Conf. |
|---|---|---|
| +0x40 | open (initialized) | C |
| +0x50 | dirty → re-render entry surfaces | C |
| +0x60 | pane flags; bits 0x4/0x8 set while player control is off (meaning U) | C |
| +0x17c/+0x184/+0x18c | entries: pointer array (count, capacity, data) | C |
| +0x190 | the response entry, NULL when none | C |
| +0x194 | the player answering (set by `SetWait`, cleared on commit) | C |
| +0x198[8] | choice texts (tags), `strdup` | C |
| +0x1b8[8] | choice labels, `strdup` | C |
| +0x1d8 | choice count (≤ 8) | C |
| +0x1dc | chosen index, −1 none | C |
| +0x1e4 | response committed | C |
| +0x1e8 | control on while choosing (`respctrlon`) | C |
| +0x1ec | control state saved by `ShowResponses` | C |

Global aliases used by other code: `0x00667e58` = +0x190, `0x00667e5c` =
+0x194, `0x00667e60` = +0x198, `0x00667e80` = +0x1b8, `0x00667ea4` =
+0x1dc, `0x00667eac` = +0x1e4, `0x00667eb0` = +0x1e8.

### 4.2 Entry — 0x160 bytes (port name `TDialogEntry`)

Ctor `0x00533f10(pane, chr, mode, x, y, offx, offy, color, hicolor, count, texts[], labels[], duration)`.

| Offset | Field | Conf. |
|---|---|---|
| +0x00 | owning pane | C |
| +0x04 | speaker map index (`chr+0x40`) | C |
| +0x08 | mode: 1 NPC speech, 2 player speech, 3 response list | C |
| +0x0c / +0x10 | text colour / highlight colour (white) | C |
| +0x14 | ticks left; −1 = no timeout | C |
| +0x18 / +0x1c | base position (screen) | C |
| +0x20 / +0x24 | offset in its stack; −10000 = not laid out yet (not drawn) | C |
| +0x28 / +0x2c | width 400 / height | C |
| +0x30 / +0x34 | target offset; +0x38/+0x3c 16.16 position; +0x40/+0x44 16.16 step | C |
| +0x48 / +0x4c | text surfaces: coloured / white (+0x4c only with `NoTexOverlay`) | C |
| +0x50 | done (dismissed) | C |
| +0x54 / +0x58 | fade step 0–12 / target (12 shown, 0 dismissed) | C |
| +0x5c | text count (1 for speech, choice count for responses, ≤ 8) | C |
| +0x60[8] | text, `strdup` | C |
| +0x80[8] | text rect `{x0, y0, x1, y1}` inclusive | C |
| +0x100[8] | choice button (mode 3) | C |
| +0x120[8] / +0x140[8] | highlight step 0–8 / target (8 while the button is hovered) | C |

Construction: mode 3 texts are `sprintf("\"%s\"", GetLine(tag))` —
choices show the dialog line in quotes; speech texts are copied. Each
text wraps to 350 px (`0x004acb80`, font `0x0065c134`, flags 0x401) in the
rect x 50..399; texts are 10 px apart; height = last `y1 + 1`, minimum 44
with the texts centred vertically. Mode 3 creates one button per choice
(`0x0042c600`, **name = the choice label**, flags 0x100010, rect = the
text rect) and registers it with the pane. Then `Render` `0x00534470`
draws, into the surfaces: the speaker's portrait
(`TObjectInstance::InventoryImage` vtable 0x130, `0x0046f190`) centred at
(22, 22), the `"Ring"` bitmap over it, and the texts in the text colour
(`NoTexOverlay`: also a white copy in +0x4c). **C**

Colours (bytes `[0],[1],[2]` as stored; decoded with the UI track's
convention `[0] = R`,
[UI_METHOD_MAP §6](../../ui/forensics/UI_METHOD_MAP.md)): player speech and choices
`ff af 3c` → RGB(255,175,60) gold; NPC speech by speaker slot, a 16-entry
round-robin table `0x0066f6f8` (counter `0x0066f73c`, first new speaker
gets slot 1), colour = slot & 3: 0 `00 00 ff`, 1 `00 ff 00`, 2 `00 ff ff`,
3 `ff ff 00`; highlight `ff ff ff`. **C** for the bytes, **I** for the
decode — one retail screenshot of the opening settles it. The 1998 pane
used a fixed red choice highlight and blue/red over-head text.

### 4.3 Methods

| Address | Vtbl | Role (port name) | Args → effect | Called by | Conf. |
|---|---|---|---|---|---|
| `0x00534fd0` | 0x00 | Initialize | clear choices/entries, `+0x1dc=−1`, `+0x60 |= 2` | `TPlayScreen::Initialize` | C |
| `0x00535060` | 0x04 | Close | free choices, delete entries | | C |
| `0x00535120` | 0x34 | Hide | hidden + ignore input (`+0x48/+0x4c = 1`); **forgets** entries without deleting them; clears choices | drawer mode 1 opening (`0x0047b4d0`) | C |
| `0x005351d0` | 0x4c | Pulse (per tick) | pulse entries; lay out; delete faded entries; **commit**: if `+0x1e4 && +0x1dc ≥ 0` → `+0x1e4 = 0`, `SetControl(+0x1ec)`, dismiss the response entry, `+0x190 = +0x194 = +0x1e8 = 0` | `TScreen` pane pass `0x0048fda0` | C |
| `0x00535500` | 0x50 | redraw | if dirty, re-render every entry's surfaces | `TScreen` draw pass `0x0048ff00` | C |
| `0x00535550` / `0x005355b0` | 0x1c / 0x5c | draw entries | per entry `DrawOverlay` `0x00534b60` (`NoTexOverlay` off) / `DrawPlain` `0x00534d40` (on) | | C (slot callers U) |
| `0x00535610` | 0x6c | KeyPress(key, down) | Space, no response entry → `SkipSpeech` body; `'1'`–`'6'`, index < count, response entry up → `+0x1dc = key−'1'`, `+0x1e4 = 1` | | C |
| `0x00535760` | 0x74 | Joystick(code, down) | `0x40a` (`JOY2`, key table `0x005cdfd4`), no response entry → skip speech | `TPlayScreen` `0x0047cdb0` → `TScreen` `0x00490860` | C |
| `0x005362b0` | 0x94 | OnControl(button, msg) | `msg == 3000` (button clicked): for each choice, `stricmp(button+0x18 name, label[i]) == 0` → `+0x1dc = i`, `+0x1e4 = 1` (no break: last match wins) | `TButtonPane` | C |
| `0x00535870` | — | AddChoice(label, text) | needs open or a response entry; if a previous answer exists (`+0x1dc ≥ 0`) start a fresh list (dismiss response entry, free, reset); if count < 8 append `strdup` text/label | `choice`, `TScript::AddChoice`, MP `0x00583538` | C |
| `0x00535e90` | — | ShowResponses() | needs count > 0 and a player; `+0x1ec = PlayScreen control (0x0065d0d0)`; `SetControl(+0x1e8)`; create mode-3 entry (player, gold, white, texts, labels, −1); `+0x190 =` it | `SetWait` | C |
| `0x00535b90` | — | AddSpeech(chr, text, ticks) | needs all three (ticks > 0); mode 2 if `chr` is a PLAYER else 1 with slot colour; create entry; append | `TCharacter::Say` | C |
| `0x00536010` | — | SkipSpeech() | open and no response entry: for each entry, `StopTalking` its speaker (`GetInstance(+0x04)`), dismiss it | `End` (flag 4), `ClearSpeech` | C |
| `0x00535a10` | — | ClearResponses() | dismiss the response entry, free choices, count 0, `+0x1dc = −1`, `+0x1e4 = 0` (control **not** restored) | `ClearSpeech`, `ResetForLoad` | C |
| `0x00535d80` | — | ClearSpeech(now) | if open: `SkipSpeech` (no-op when a response entry is up) + `ClearResponses`; `now` → delete all entries, else dismiss all (voices keep playing) | camera retarget away from the player (`0x004538d0`, `0x00453940`), `fadescreenout` `0x00427e80`, player setup `0x00518570`, MP state `0x0051d680` | C |
| `0x005360f0` | — | ResetForLoad() | dismiss response entry, free choices, delete all entries. Its tail (`SetControl(+0x1ec)`, `+0x194->ScriptJump("Finish")`, force-done if talking, `Hide`) is unreachable — it tests `+0x190` after clearing it | `LoadGame` `0x0048df70` only | C |
| `0x00536320` | — | DeleteEntry(i) | dtor `0x005343e0` + free + remove | | C |

Entry methods: `Pulse` `0x005348f0` (count down `+0x14`, at 0 dismiss;
step the slide; step the fade toward `+0x58`; per choice step the
highlight toward 8/0 from the button's hover flag `+0x14 & 8`; move each
button with the entry), `Dismiss` `0x00534a40` (`+0x50 = 1`, `+0x58 = 0`,
remove buttons). **C**

### 4.4 Layout and timing (per pane pulse, 24 Hz)

| | Retail | Conf. |
|---|---|---|
| x | `(MapPane.w (0x006668e4) − pane 0x0065be50.w (0x0065be5c) − 400) / 2` — centred in the map width less the right-hand pane | C (pane identity I) |
| NPC stack (mode 1) | top = pane `0x0065a8c0`.y + .h + 10; entries downward, 5 px apart | C |
| player + responses (modes 2, 3) | bottom = MapPane.y + MapPane.h − 50; entries upward, 5 px apart, oldest on top | C |
| order | creation order; dismissed entries keep their place while fading | C |
| new entry | appears at its slot (no slide) | C |
| restack | existing entries slide to their new slot over 12 ticks | C |
| fade | 12 ticks in, 12 out; removed when faded out and dismissed | C |
| speech lifetime | `ticks` from `Say` (same count as the action's `wait`) | C |
| choice highlight | 8 ticks toward white while hovered | C |
| `NoTexOverlay` on | no fade or highlight ramp: drawn while the target is shown, highlight swaps to the white surface | C |

The port converts these to time (½ s fade/slide, ⅓ s highlight,
lifetimes in seconds), per the frame-rate-independent animation rule.

### 4.5 From a click to the chosen response

1. `choice` × n → `+0x198/+0x1b8`, count n.
2. `wait response` → `SetWait` → `ShowResponses`: control saved and
   turned off (on for `respctrlon`); response entry with n buttons named
   by the labels.
3. Mouse over a choice → its button's hover flag → the entry ramps that
   choice's highlight.
4. Click → `TButtonPane` → `OnControl(button, 3000)` → label match →
   `+0x1dc = i`, `+0x1e4 = 1`. Or key `1`–`6` → same.
5. Next tick, map pulse: the waiting script sees `+0x1e4`, clears flag 4,
   jumps to `label[i]`, runs on.
6. Same tick, pane pulse: `+0x1e4 = 0`, control restored from `+0x1ec`,
   response entry dismissed (fades out), `+0x194`/`+0x1e8` cleared. The
   list and `+0x1dc` survive, so `say choice` still works; the next
   `choice` starts a fresh list.

### 4.6 Against the UI spec ([DialogPane_SPEC.md](../../ui/forensics/DialogPane_SPEC.md))

| Spec section | This pass | Evidence |
|---|---|---|
| §0, §2 chrome `Dialog.dat` 640×140 + `DialogEndCap` | Loaded by PlayScreen into `0x00666444` and freed at close; **never read**. Retail draws no chrome. | xrefs to `0x00666444`: write `0x0047a9d1`, free `0x0047b414/2b` only |
| §1, §3 Pulse mode 1 reserves 140 px for dialog | That is the bottom drawer's editor/console mode; dialog doesn't use the drawer. | §1.4 |
| §3 UNCONFIRMED-G top/bottom anchor | NPC lines top, player lines and responses bottom. | §4.4 |
| §0, §6 `cls_0x534d40` = one choice; per-line rects; typewriter (P, O) | One entry holds all choices (or one speech line); `+0x80[]` are per-text rects; `+0x120/+0x140` are per-choice hover highlight. | §4.2 |
| §6 left 50 px "slot number" (V) | Speaker portrait + `"Ring"`; no numeric prefix; choices are quoted dialog lines. | `0x00534470`, `0x00533f10` |
| §6.overflow cap 6 (C) | 8 stored; keyboard reaches 1–6. | `0x00535870`, `0x00535610` |
| §6.text bake (S, F) | Wrap 350 px, font `0x0065c134`, flags 0x401, 10 px between choices, min height 44. Font identity still per `0x004a4a73`. | `0x00533f10` |
| §6.dialogline_retail (J) | Substitution unreachable in retail. | §3.7 |
| §5 slot 76 `0x5351d0` = paint | Per-tick pulse (layout, timers, commit). | `0x005351d0` |
| §10 MouseClick `0x40a` | Joystick slot, `JOY2`. Choice clicks go through `OnControl` `0x005362b0`. | key table `0x005cdf84` |
| §10, §0 `0x5360f0` = ESC handler | Load-game reset; the retail `KeyPress` has no ESC case. | sole caller `0x0048e16d`; `0x00535610` |
| §6, Q: `DAT_006680c8` = classic/hi-res | ini `NoTexOverlay`. | `0x00484d6c` |
| §10 "choices bound to characters" | Entries are bound to their speaker; skip stops each speaker. | `0x00536010` |

### 4.7 Against the 1998 `TDialogPane` / `TDialogList`

| 1998 (`dialog.h`) | Retail | Note |
|---|---|---|
| `TPane(0, 380, 404, 97)`, `dialog.dat` background | floating entries, no background | |
| `Show()` hides lower panes, control off, not fullscreen | `ShowResponses` saves/sets control only | |
| `Hide()` | commit in `Pulse`; `Hide` `0x00535120` is a drawer-time reset | |
| `AddChoice` (max 4) | `AddChoice` (max 8) | same reset-on-answered rule |
| `SetChoice` / `HasResponded` / `GetResponseLabel` / `GetResponse` | fields `+0x1dc`/`+0x1e4`/`+0x1b8`/`+0x198`, read by `0x00492d70` and `say` directly | |
| `ResetResponses` | `ClearResponses` `0x00535a10` | |
| `Skip()` (current character) | `SkipSpeech` (every speaker) | |
| `KeyPress` 1–4, Space, ESC (`Finish`) | 1–6, Space; no ESC | |
| `MouseClick` slot hit-test | buttons + `OnControl` | |
| `SetCharacter`/`GetCharacter` | `+0x194` (write-only in practice) | |
| — | `AddSpeech`, `ClearSpeech`, `ResetForLoad`, `Pulse`, `Joystick` | new |
| `TDialogList`: one table, `<ClassDefPath><Language>.def`, linear `stricmp`, 255-char lines, NULL on miss | two tables (base + module), sorted, upper-case `bsearch`, 4091 chars, `"[badid]"` / `"[TAG]"` | |

## 5. Retail vs 1998 — runtime deltas

| Area | 1998 | Retail | Conf. |
|---|---|---|---|
| Who waits | character `waittype` (`WAIT_RESPONSE`) | script `+0xb4` (2/5/10) | C |
| `say` result | `CMD_WAIT` (waits for the action) | 0 + `WaitSay(speaker)` on the caller (type 8) | C |
| Speech wait ends | command done | speaker idle in root, nothing desired | C |
| `say` grammar | single text token | text parts, prototype variables, `choice` from pane fields | C |
| Duration | `20 + max(15, len·5/4)`; voice `len·24/100` | `2·len + 36`; voice `12 + ms·24/1000` | C |
| Voice | positional at the speaker | non-positional (SP) | C |
| Text | over-head `AddPostCharText` | pane entry, always (over-head text not found) | C/U |
| `choice` | label + one token; pane only | label + parts; via the script, sets flag 4 | C |
| `wait response` | shows the pane, sets the character | `SetWait` opens responses; control saved/restored | C |
| Response taken | `ScriptJump(label)`, `Hide` | `Jump(label)`; pane pulse restores control | C |
| Script end | hides the pane if it owned it | `End` flag 4 → skip speech | C |
| ESC | skip + `Finish` + close | none | C |
| `message`, `hideresponse`, `busysay`, `busymsg`, `wait respctrlon` | — | new | C |
| Dialog files | `<Language>.def` | base + module `<Language>_dialog.def` → … | C |
| `[me]`/`[chr]` | works | unreachable | C |

## 6. Port state (2026-10-05, `feature/gameflow`, after steps 1–6)

| File | Today | Gap to retail |
|---|---|---|
| `src/dialog.{h,cpp}` | Retail `TDialogList` (base + module tables, steps 1). Retail pane runtime (step 5): `TDialogEntry` (mode, speaker, texts, lifetime, fade, placed), `TDialogPane` entry manager (`AddChoice`, `ShowResponses`, `AddSpeech`, `SkipSpeech`, `ClearResponses`, `ClearSpeech`, `ResetForLoad`, `Pulse` commit, keys Space/1–6/joystick); retail `choice`. Bounded `DialogLine` (substitution kept, see §3.7). | layout, drawing, choice buttons (presentation, in progress); `ClearSpeech` callers (camera retarget, `fadescreenout`), `ResetForLoad` from `LoadGame` |
| `src/script.cpp` | `SetWait` opens the responses (§2.3); the response wait takes the committed pick (§2.4); type 8 = the speaker idle in its root state (§2.5); taken flag 4, `AddChoice`, `End` (§2.6); `Continue` stops after any line that leaves the script waiting | busy fields + deferred say (MP); the MP choice list |
| `src/command.cpp` | Retail `say` (§1.1): grammar, voice via the tag, caller's speech wait, result 0; `wait` response forms through `SetWait`; `say choice` reads the last pick | `message`, `hideresponse`, `busysay`, `busymsg` still `CmdNotPorted`; prototype variables in text parts |
| `src/character.cpp` | Retail `Say`/`SayIndex`/`SayTag` (§3.1–3.2): unpositioned voice, voice-length or text-length durations, line to the pane; `StopTalking` (voice stopped, action ended); retail `ResolveSay` | deviations: the voice paces even with sound output off (retail paced by text); the action is set desired rather than `TryCommand`ed (the port's `TryCommand` drops a block it can't start) |
| `src/sound.cpp` | Retail sound list (step 2 done): `.wav` + `.mp3` from the resource directories at `Initialize` and the module's at `SetCurModule`, sorted, bsearch; `Play` without a position; `SampleLengthMs` (see "Voices in the port" below) | — |

### Proposed port order

Each step is checked on the opening scene (`keep.s` `SardokR`, lines
479–670) against retail in dosbox-x.

1. **Dialog list.** Base + module tables, retail file names, upper-case
   keyed lookup (a hash map is fine; keep base-first), `"[badid]"` /
   `"[TAG]"` misses, 4091-char lines. Load the module table at module
   mount.
2. **Voices.** Register `.mp3` (and `.wav`) under `Sound/<Language>/` from
   resources and the module archive at mount; decode MP3; expose the
   playing length.
3. **Speech.** Retail `Say`/`SayText`/`SayIndex`/`SayTag`, durations,
   non-positional voice, `ShowDialog` gate on the action text,
   `ResolveSay` stop flag, `StopTalking`. Drop `AddPostCharText`.
4. **`say` command and the speech wait.** Retail grammar; `WaitSay` on the
   caller; type-8 rule. The opening then plays voiced to the first
   choice.
5. **Pane runtime.** `TDialogPane` as an entry manager: entries,
   `AddSpeech`, `AddChoice`, `ShowResponses`, `Pulse` (layout, timers,
   commit), `SkipSpeech`, `ClearSpeech`, `ClearResponses`,
   `ResetForLoad`, input (Space, 1–6, `JOY2`, choice buttons). Hand the
   committed response to the waiting script directly (§2.4). The UI
   track draws entries from the corrected spec; until then the runtime
   is checked headless by log plus the input simulator (`key_press 1`).
6. **`choice` / `wait response` / `End`.** Retail `choice` through
   `TScript::AddChoice` with flag 4; `SetWait` opens responses and handles
   control; the wait check jumps; `End` flag 4. Remove the 1998
   `Show`/`Hide`/`SetCharacter` calls and the script-end `Hide`.
7. **Rest.** `message` (text bar), `say choice`, `hideresponse` (drawer
   close, once the drawer exists), `busysay`/`busymsg` (store only),
   `LoadGame` → `ResetForLoad`, camera retarget / `fadescreenout` →
   `ClearSpeech`.

### Voices in the port (step 2, 2026-10-05)

`TSoundPlayer` (`src/sound.{h,cpp}`) keeps retail's sound list. Decomps
cited here are in `recon/discovered/dialog/` and `recon/classes/cls_0x41c7d0.cpp`.

| Retail | Port |
|---|---|
| Sound init `0x0049a830` → `0x0049afd0`: `<ResourcePath>sound\effects\`, `<ResourcePath>sound\<Language>\`, the same two under `ImageryPath` when they differ; qsort | `Initialize` → `LoadResourceSounds` (`InitGlobals` step 22) |
| Module mount `0x004609f0` → `0x0049b220` (after the dialog list): `<ModulesPath><module>\sound\effects\`, `…\sound\<Language>\`; qsort | `TModuleManager::SetCurModule` → `LoadModuleSounds`, after `DialogList.LoadModule()` |
| Leaving a module `0x0049b400` (from `0x004609f0` and the module close `0x00460c10`) | `UnloadModuleSounds`, from `SetCurModule` when a module was active and from `TModuleManager::Close` |
| Directory scan `0x0049ad20`: `*.wav`, then `*.mp3`, through the pack-aware findfirst `0x004a19d0`; name = file name up to its first `.` | `RegisterSounds` over `rev_find_files` |
| `FindSound` `0x0049c430`: bsearch, `_stricmp` | `FindSound`: binary search, `stricmp` |
| Load `0x0049b650` | `Mount` (decodes to 16-bit PCM) |
| Play `0x0049b990(id, 0x7f, 1, NULL, 0x50, 700)`: once, full volume (relative to the SFX volume), no position (0x50/700 are the 3D path's distances) | `Play(id)`: volume 0 is full; without `spos` the sound isn't positioned |
| Stop `0x0049bd90` | `Stop(id)` |
| Sample length `0x0049c640` | `SampleLengthMs(id)` |

On the GOG data: 1,057 `.wav` + 6 `.mp3` resource sounds and 1,743
module voices, 2,806 in all. `I1LOC00` → `i1loc00.mp3`, 1,227 ms (say
duration 12 + 29 = 41 ticks); `I1SAR00` → `i1sar00.mp3`, 3,343 ms (92
ticks). Both agree with `afinfo` (47 and 128 MPEG frames × 1,152 /
44,100 Hz). `build/test_audio_decode` pins the decoder lengths.

Deviations (`REVSYNC-DIVERGENCE` in the code):

- **Registration doesn't need output.** Retail has no sound list when
  output is off or fails to open (`0x00668114`): `FindSound` fails and
  every say falls back to text pacing. The port registers regardless, so a
  `--headless` run paces voiced lines by the voice, as a run with sound
  does. `Functioning()` says whether output is live, for step 3 to decide
  how a player's "sound off" paces.
- **`SampleLengthMs` decodes the file** (once per sound, cached). Retail
  asks Miles for the total of the 2D sample playing the sound
  (`AIL_sample_ms_position`) and gets 0 when it isn't playing. MP3 length
  counts every frame, with no encoder-delay trim; that Miles' `mp3dec.asi`
  does the same is **I**.
- **Decoded when mounted.** Retail kept the file bytes (an LRU cache
  against a memory budget) and Miles decoded while playing.
- **Duplicate names:** a stable sort, so the first registered is found;
  retail's qsort + bsearch find either. The shipped data has none.
- **`.mp3` 2D-only flag** (`+0x18` bit 1) not kept: the port has no 3D
  path, and `Play` with a position applies the 1998 pan law to any sound.
- **Without a position, not positioned** for every caller (ambience,
  `PLAY` one-shots): before, such a sound was pinned to the listener's
  position at play time and faded as the listener moved away.
- **A module sound still playing** when its module goes is freed (it
  stops); retail drops it from the list without freeing it.
- **Older port INI** (`ResourcePath = "."`): `.\sound\effects\` lies
  outside every pack's directory, so `rev_find_files` looks the directory
  up inside the base packs (the by-name fallback's counterpart) and the
  1,063 resource sounds still register. Retail would find none.
- **Boot order:** the port mounts the main module before `InitGlobals`, so
  the module's sounds register before the resource sounds (retail: sound
  init in engine init `0x00485870`, the module later). The list is sorted
  after each step, so the result is the same.

## 7. Open questions (author)

1. Was the floating speech-box design (NPC lines top, Locke's lines and
   choices bottom, portrait in a ring) the shipped UI, with `Dialog.dat`
   a leftover of the 1998 bottom panel?
2. `ShowDialog`: retail always puts the text in the pane; the option only
   blanks the say action's copy, which nothing draws. Was it meant to
   hide subtitles when a voice plays? Should Revisited honour it?
3. `NOWAIT player.say` still waits in retail (10 shipped uses). Intended,
   or should those lines overlap the next one?
4. `[me]`/`[chr]` substitution is dead in retail (inverted lead-byte
   test). Restore it in Revisited?
5. `busysay`/`busymsg`: multiplayer "this NPC is busy talking to another
   player" lines? And the `&=` on the script flags — a typo for `|=`?
6. `hideresponse`: a 1998 command (hide the dialog drawer) kept after
   dialog moved off the drawer?
7. `revenant.ini [Controls] DialogSkip=A,B,C,JOY1,JOY2,JOY3` — no such string
   in `Revenant.exe`; Space and `JOY2` are hard-coded. From another
   build or the launcher?
8. Eight choices stored, keys reach six — was eight ever used?

## 8. Retail hazards (decide in the port, don't copy blindly)

- Response lost if the waiting script isn't pulsed before the pane (§2.4).
- Second `wait response` while one is open: no wait, falls into the first
  label (§2.3). `wait response` with no choices: waits forever.
- Camera retarget or `fadescreenout` during a response wait clears the
  choices; the script then waits forever (`ClearSpeech` → `ClearResponses`).
- `Hide` `0x00535120` drops entries without freeing them.
- `say choice` with no response says the shared buffer's stale text.
- `say` on a dead speaker sets a speech wait that may never end (`Say`
  returns 0, the speaker never returns to root).
- `DialogLine` over-reads odd-length strings (§3.7).
- `choice` label buffer is 80 bytes, unbounded copy.
