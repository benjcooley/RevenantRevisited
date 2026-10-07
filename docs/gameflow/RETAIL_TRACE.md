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
| `[state] <VAR> = <n>` | the game-state setter behind `set` | `command.cpp` |
| `[textbar] <text>` | `TTextBar::Print` | `textbar.cpp` |
| `[session] entered level N` | level entry | `gamesession.cpp` |

Every line carries the game tick (24 Hz), not wall time, so two runs line
up by simulation time. The port's trace gets the same tick prefix under a
switch, so the two logs diff directly.

## 3. How

- **Hook spec, not hand-written hooks.** A list of hooks (address, handler,
  what to capture: `ecx` for `this`, stack arguments by index) generates
  every wrapper in one build. `tools/retail_asm/function_hook.py` does one
  fixed-message hook today; it grows into this.
- **C runtime.** One log file opened once (`retail-trace.log` beside the
  exe), line-buffered writes through the game's `CreateFileA` and
  `WriteFile` imports. Formatting uses the game's own CRT `sprintf` (retail
  links the MSVC CRT statically), so the hooks carry no printf of their own.
  Handlers only read; they never change game state.
- **The lab's compatibility patch.** The lab runs `Rev98.exe`, the shipped
  exe with a 3-byte Win98 patch. The traced exe applies the same bytes and
  ships as `C:\REVENANT\RevTrace.exe`; the shipped files are not touched.
- **Verification.** The baseline still rebuilds byte-identical; a traced
  build differs only in the hooks' spans and the wrapper area (the tool
  checks this); and a guest run of the opening writes a trace whose
  `[script]` lines match the port's for the same scene.
- **Driving it.** `tools/retaillab/retail.py` gains `start --trace`
  (upload and launch `RevTrace.exe`) and `trace pull` (fetch the log).
  `tools/storytest` gains a retail/port trace diff.

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
