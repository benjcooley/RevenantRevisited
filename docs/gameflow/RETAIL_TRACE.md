# Retail trace — the shipped game logging what it does

Plan (2026-10-07). The retail executable now reassembles byte-identical from
source (`recon/retail_asm`, built by `tools/retail_asm`), and a hook can run
compiled C inside it: a hook steals a function's prologue bytes, jumps to a
wrapper in the image's spare space, calls freestanding C that writes through
the game's own imports, then runs the stolen bytes and jumps back. The
gameflow track uses this to make retail write the same trace the port writes
(`[script]`, `[dialog]`, `[textbar]`, `[session]`), so a retail run and a
port run of the same scene compare line by line instead of by eye.

## 1. Why

Screenshots settle how things look; they can't show which script line ran,
in what order, with what values. Questions the story tests keep hitting:

- does a scene run the same lines in the same order (the opening, Tendrick's
  scene, the NPC sweep's 34 DIALOG blocks)?
- how long does a `say` hold the script (Kylie's ~40 s lines and the
  level-46 speakers that never return to root, BURNDOWN T9)?
- what a test reads (`If Rahul.stat health = 0`, game states) at the
  moment it reads it;
- the dialog pane's entry rects, the text bar's lines, level entry and
  release.

## 2. What gets traced

| Trace line | Retail hook | Port equivalent |
|---|---|---|
| `[script] <object>: <line>` | the command interpreter `0x0041e8e0` (context, token, abbrevlen, script): the line from the script's ip | `script.cpp` `TraceLine` |
| `[script] <object>: trigger N of '<name>' starts` / `ends` | trigger start `0x00492440`, `End` `0x00493e40` | `script.cpp` |
| `[dialog] <speaker> says: <text>` | `TCharacter::Say` `0x004d0610` (this, text, frames, anim, sound) | `dialog.cpp` |
| `[dialog] choice N committed (label '<l>')` | the response taken, `0x00492d70` types 2/5/10 | `dialog.cpp` |
| `[dialog] rect <speaker> mode M "<text>" at X,Y size WxH fade F texts (x0,y0)-(x1,y1) …` — each entry's screen rect and its text rects, when they change | entry pulse `0x005348f0` (this = entry): speaker `+0x04` (map index → name `+0x38`), mode `+0x08`, base `+0x18/+0x1c` + offset `+0x20/+0x24`, size `+0x28/+0x2c`, fade `+0x54`, text rects `+0x80` (entry-relative, inclusive) | `TDialogEntry::TraceGeometry`, at the start of `Pulse` |
| `[state] <VAR> = <n>` | the game-state setter behind `set` | `command.cpp` |
| `[textbar] <text>` | `TTextBar::Print` | `textbar.cpp` |
| `[session] entered level N` | level entry | `gamesession.cpp` |

Every line carries the game tick (24 Hz), not wall time, so two runs line
up by simulation time. The port's trace gets the same tick prefix under a
switch, so the two logs diff directly.

## 3. How

The trace runs inside the **in-process retail emulator**
(`docs/RETAIL_AB_TESTING.md`, `tools/retail_runtime`), not the DOSBox lab:
a named private build of the shipped exe, `function_hook.py` hooks, and the
log captured as a virtual file. The DOSBox `RevTrace.exe` path (uploading a
patched exe into the Win98 lab and pulling `retail-trace.log`) was a
false start — see the note at the end of §5.

- **Hook spec, not hand-written hooks.** A declarative list (address,
  handler, what to capture: `ecx` for `this`, stack arguments by index)
  generates every wrapper and one C translation unit in one build, added to
  a named emulator build.
- **C runtime.** One trace log, opened once and appended a line at a time so
  a crash keeps what was written. Formatting uses the game's own statically
  linked CRT `sprintf` (§5), so the handlers carry no printf of their own.
  Every line is prefixed with the game tick (§5). Handlers only read game
  memory; they never change game state.
- **The hook targets** — the four addresses, the fields each handler reads,
  the speaker-name getter, how to recover the current line from the script,
  the CRT `sprintf`, the tick global, and which prologues relocate — are in
  §5, confirmed from the retail decomp and ready for the named-build
  workflow.
- **Waiting on the emulator.** Full retail startup does not run in the
  in-process emulator yet (`tools/retail_runtime/README.md`), so the
  gameflow trace is blocked on that. The addresses and handler logic in §5
  are ready to drop in once it boots.
- **A/B through the emulator** is planned in
  [RETAIL_AB.md](RETAIL_AB.md) (feature/gameflow, a1c3309).

## 4. Order

1. Multi-hook spec, the C runtime, the compatibility patch; one hook
   (`Say`) run in the guest to prove the path end to end.
2. The `[script]` and trigger hooks; the opening traced in retail and
   diffed against the port.
3. The rest of §2; the tick prefix in the port; the storytest diff.
4. Use: the Keep chain and the NPC sweep against retail; the speech-wait
   question (T9).

The generated assembly (`recon/retail_asm/baseline`, ~160 MB) is not
committed: `tools/retail_asm/reconstruct.py` rebuilds it from the retail
exe. The tools and hook sources are.

## 5. Hook targets, for the emulator's named-build workflow

Confirmed from the retail decomp and disassembly (2026-10-07).

| Line | Target | Handler reads | Prologue (stolen bytes) |
|---|---|---|---|
| `[dialog] <name> says: <text>` | `TCharacter::Say` `0x004d0610`, thiscall (text, frames, anim, sound) | `this`, arg0 text; skip NULL text | `mov eax,fs:[0]`, 6 bytes, relocatable |
| `[script] <obj>: <line>` | `CommandInterpreter` `0x0041e8e0`, cdecl (context, token, abbrevlen, script) | arg0 context, arg1 token | `sub esp,0x5c; push ebx; mov ebx,[esp+0x64]`, 8 bytes, relocatable |
| `[script] <obj>: trigger N of '<proto>' starts` | `TScript::Start` `0x00492440`, thiscall (proto, pos, priority), `ret 0xc` | `this` script, arg0 proto, arg1 pos | `push esi; mov esi,ecx; mov eax,[esi+0x48]`, 6 bytes, relocatable |
| `[script] <obj>: trigger N ends` | `TScript::End` `0x00493e40`, thiscall | `this` script; log only if ip `+0x48` ≠ 0 | `push ecx/ebx/ebp/esi; mov esi,ecx`, 6 bytes, relocatable |

None of the four prologues has relative control flow, so the stolen bytes run unchanged in a wrapper.

- **Speaker / object name:** `*(char**)(obj+0x38)`, or "?" when NULL; this matches the port's `GetName()`. The context is `*(script+0x0c)`. The proto name is `*(char**)(proto+0x00)`; the field at `+0x3c` is the filename.
- **Trigger N:** at `End`, read `*(int*)(script+0x1c)`. At `Start` that field is still stale, because the caller writes it after `Start` returns. Instead, scan the trigger records (pointer array at `proto+0x1c`, count at `+0x44`, fallback record at `+0x20`) for `rec+4 == pos`; N is `rec+0`.
- **Line text:** the script's ip (`+0x48`) is written only when the script stops, so it cannot give the current line. Instead read the token's stream: `stream = *(token+0x0c)`, `buf = *(stream+0x08)`, `ptr = *(stream+0x10)`. Back up from `ptr` to the start of the line, trim blanks and stop at EOL; this is the port's `TraceLine`.
- **CRT:** `sprintf` is at `0x0058b100` and returns the length; `_snprintf` is `0x0058ecbc`, `_vsnprintf` `0x0058d202`, `_output` `0x00590329`.
- **IAT:** `CreateFileA` `0x005a30bc`, `WriteFile` `0x005a30c8`.
- **Tick:** the 24 Hz frame counter is `*(*(u32*)0x00667fd0 + 0x48)` (CurrentScreen; incremented at `0x00491454` and `0x004917f7`).
- **Unconfirmed:** in an emulated run, the `Say` handler produced exactly `1234 [dialog] Locke says: Where am I?`. The other three handlers were never verified in a run. A DOSBox `RevTrace.exe` booted to the title, then hung Win98 during the New Game opening; the cause was not diagnosed. `Say` logs at entry, before the health check.
- **Diff:** `tools/retailtrace/trace_diff.py --retail <retail-trace.log> --port <revenant.log>` compares the `[script]`/`[dialog]` sequences and prints the first divergence. On the port side it reads only the TRACE `[script]` lines and the DEBUG `says:` lines.
