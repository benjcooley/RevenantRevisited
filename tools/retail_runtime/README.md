# Thin retail runtime

For dependencies and locally generated reference prerequisites, start with [SETUP.md](SETUP.md).

Overall goal: convert and verify all 176 shipped VFX rows. This is the primary
fast test tool; DOSBox-X with the emulated 3D device is reserved for targeted
software-rendering diagnostics. See [the active agent handoff](../../docs/RETAIL_AB_TESTING.md),
[direct controls](README-controls.md), [effect A/B fixtures](README-effects.md)
and [private named builds/parallel sessions](README-parallel.md).

This runs retail x86 instructions in a Mac process using Unicorn. It loads a PE
once and restores changed memory pages, CPU context and virtual API state between
scenarios. It does not boot DOSBox, Windows, or another virtual machine.

The first working scenario calls the **original** software render-state setter
`0x56d400` and getter `0x56d4b0`. The initial 500-iteration benchmark measured
about **0.036 ms mean per warm loop**, including restore, both calls, a result
assertion and a fixed-clock advance. PE loading took about 2.5 ms. These timings
cover small functions; they do not predict HUD or combat performance.

## Run it here

From the `RevenantRevisited` repository (the inner folder):

```sh
PY=/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python
"$PY" tools/retail_runtime/run.py recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --scenario tools/retail_runtime/scenarios/render_state.json --repeat 100
```

The process loads once; `--repeat` restores the same checkpoint on each iteration.
Every result includes assertions, API calls, restored-page count and elapsed time.
The scenario pins the retail executable SHA so addresses cannot silently be used
with a different binary. The rebuilt image is byte-identical to the retail CD
image: `28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.

To retain one process across different scenarios, omit `--scenario` and send one
JSON request per line on stdin. Responses are one JSON line each, with the same
optional `id`:

```json
{"id":1,"command":"status"}
{"id":2,"command":"execute","scenario":{"operations":[{"op":"call","address":"0x56d400","args":[20,6]},{"op":"read_u32","address":"0x675f10","expect":6}]}}
```

Use `--setup FILE.json` for an array of one-time setup operations. Commands are
`status`, `execute`, `checkpoint` and `restore`. Every `execute` starts from the
checkpoint. To make a successful scenario's final state the new checkpoint, send
`checkpoint` immediately after its result. A failed scenario rolls back before
returning an error.

Scenario operations:

- `allocate`: name and size; creates zeroed guest memory, referenced as `$name`.
- `surface`: name, width and height; creates a pitched RGB565 buffer.
- `write` / `write_u32`: explicit guest address and hex bytes / value.
- `write_code`: guest address and instruction bytes. Marks allocated stubs as
  code and invalidates Unicorn's translated blocks after patches. PE executable
  section writes invalidate automatically, as does checkpoint restoration of
  code. Use this for warm instrumentation and changed instruction stubs;
  data-driven stubs can keep fixed code and change only scalar inputs.
- `mount_file`: reads an explicit host file into a case-insensitive guest path.
  Use in setup for asset input; snapshots reset scenario writes.
- `post_message`: queues an explicit message, parameters and cursor coordinates.
- `register_window`: associates a virtual HWND with an original guest WndProc.
- `call`: guest address, stack arguments, optional `this` for ECX, instruction
  budget and optional stop address. Optional `expect_eax` asserts the return.
- `read_u32`: reads state, with optional `expect` assertion.
- `advance`: advances the virtual clock by ticks at 24 Hz. It **does not execute
  a game update**; scenarios must call the relevant original update functions.
- `capture`: hashes a named surface and optionally exports its raw RGB565 bytes
  to an explicit host `output` path. Dimensions and byte stride accompany hashes.

The default call supports x86 stack arguments and ECX thiscall. Fastcall EDX,
variadic calls, aggregate returns and unusual conventions require an adapter.
The instruction limit stops loops; there is no wall-time or instruction-count
claim that a complete game tick has run.

## What works and what remains

Verified: PE mapping and imports (including ordinals), original setter/getter
execution, warm reset, CPU/memory/heap-position reset, fixed clocks, explicit
MSVC RNG stream, in-memory files, compiled C hook logging, fault rollback, and
raw surface capture. Guest-thread `Sleep`, basic `PeekMessageA` filtering,
`PostQuitMessage`, `SetTimer`/`KillTimer`, and guest callback dispatch also work.
Queue, timers and clock restore with the snapshot. Import handlers run only at
API boundaries; ordinary retail
instructions execute in Unicorn without per-instruction Python callbacks.

The virtual APIs currently cover a limited kernel32 file/clock/allocation subset,
user32 queue/timer operations and winmm/`_inmm.dll` `timeGetTime`. Unknown DLL APIs
and unsupported asynchronous/64-bit
file operations stop execution. File APIs operate on a scenario's in-memory
filesystem; they never implicitly write the host filesystem. This is a scoped
compatibility implementation, not complete Win32 semantics. The allocator is
monotonic; scoped HeapCreate/Alloc/Free/ReAlloc/Size/Destroy are implemented.
VirtualAlloc reserve/commit and VirtualFree decommit/release preserve page
protections and roll back mappings. The heap allocation layout is a selected
policy, not a clone of Windows allocator internals.

Threads have separate CPU contexts, stacks and FS thread-information segments.
CreateThread, suspension/resume, ExitThread, manual/auto-reset events, wait-any/
wait-all, recursive critical sections, TLS and per-thread last error are implemented.
A deterministic scheduler runs bounded instruction slices and yields on sleep/
waits. Virtual time advances explicitly by scenario ticks or to the next finite
wake/timeout when no threads are runnable. This is not Windows scheduling
reproduction: busy loops do not create wall-clock time, priority is recorded
without host priority mapping, and there is no general exception dispatcher.
Deadlocks and exhausted instruction-slice budgets are explicit failures.
`WM_TIMER` generation is lazy and coalesces missed periods. Posted messages take
precedence. WndProc/TIMERPROC dispatch executes guest code with the original
stdcall argument convention. The current queue has no sent-message handling,
window hierarchy, character translation, paint invalidation or per-thread queues.

## Required Windows environment

The agreed scope is the following. Keep it in the same persistent Mac process,
so restoring a test does not require booting a guest OS:

- **DirectDraw/Direct3D:** use Revenant's original software 3D renderer. Supply
  RAM-backed DirectDraw surfaces, pixel formats, locks, copies, flips and caps.
  Implement Direct3D interfaces still reached by that software path. No
  accelerator backend is planned for this reference path.
- **Threads:** guest CPU contexts, separate stacks and thread-local state, with
  a deterministic cooperative scheduler. Include events, waits and critical
  sections. Preserve runnable/blocked/sleeping states in checkpoints. Thread
  creation and the scoped synchronization APIs above are implemented; general
  SEH, guard-page growth and a complete Windows thread environment remain.
- **Message queue:** keyboard/mouse events and timers as scripted input; invoke
  the retail WndProc. Basic posted-message polling/dispatch exists; per-thread
  queues, sent messages and translation remain.
- **Display:** fixed-resolution RGB565 front/back buffers, offscreen by default,
  with optional viewing/input. Surface allocation and capture exist; original
  camera, RGB565/ARGB4444 lookup generation, projection, texture lock/unlock and
  software raster calls now draw actual pixels into those buffers. General
  DirectDraw/display objects and full window creation remain incomplete.
- **Audio:** virtual playback handles, sample positions, completion and time;
  optional audible output. Retail imports Miles `mss32.dll` and `smackw32.dll`,
  so their reached playback/cutscene boundaries need adapters. Audio support
  remains unimplemented. Silent output must preserve playback state.
- **Filesystem:** retail assets and saves at Windows paths, warmed read caches,
  resettable write overlays and deterministic enumeration. Basic virtual file
  reads/writes and explicit host-file mounting exist; directories, current path,
  searches, INI APIs and lazy asset-tree mounting remain.
- **Sleep/timers:** one virtual clock for game time, waits, periodic callbacks
  and audio. Basic clocks/sleep/user32 timers exist. Multimedia callbacks and
  Windows scheduling fidelity remain.

The API subset follows documented [Sleep](https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-sleep),
[PeekMessage](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-peekmessagea)
and [SetTimer](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-settimer)
contracts where implemented. The chosen deterministic policy and remaining
Win98 compatibility gaps above are explicit; current documentation alone does
not establish every Win98 scheduling detail.

**Full retail startup, HUD drawing and Locke/monster combat are still incomplete.**
The stock entrypoint now passes original heap/TLS/thread initialization and
reaches the missing `GetStringTypeW` locale API. Software fixtures stop original
CRT initialization at `0x58ed8e`, then run original camera/table/texture/raster
functions without needing the rest of startup. Flame and Ripple have real
rendered frontend A/B evidence; both frontends use the same original raster,
so full Revisited GPU-backend and natural-map acceptance remain separate.

A named experimental image must pass the explicit [build contract](build_contract.py):
actual SHA, baseline ancestry and unchanged fixed-address PE layout. Bare changed
images are rejected. Flame's `--build build.json` supports logging experiments;
behavioral equivalence is tested rather than inferred from ancestry.

The next implementation gates are:

- [x] Load the exact retail image and execute original instructions in process.
- [x] Reset state cheaply and run assertions repeatedly.
- [x] Give scripts a persistent JSON interface and deterministic API boundaries.
- [x] Direct fixture/entity controls, reset and independent parallel named builds.
- [x] Original RGB565/ARGB4444 software raster and depth output into RAM.
- [x] Flame/Ripple frontend geometry/animation and shared-raster image comparisons.
- [x] Compiled C logging at actual Flame Render, with unchanged color/depth output.
- [x] Run the original Classic HUD HP bar kernel across changed stats/direction/fade.
- [ ] Connect those original bar draws to retail textures and RGB565 pixels.
- [ ] Prepare one valid retail HUD fixture from assets and known object layouts.
- [ ] Connect its original bitmap drawing path to RGB565 RAM and capture it.
- [ ] Vary one actual HUD statistic and compare both captures with Revisited.
- [ ] Prepare Locke/monster state and invoke original combat updates at fixed ticks.
- [ ] Control each observed RNG/time/input source, then compare combat traces.
- [ ] Load one initialized game state once and checkpoint it for broader scenarios.

Start with targeted original subsystem calls to reduce compatibility work. Extend
the original startup path when a subsystem needs its initialized state. Keep the
game's calculations and software rasterization in original machine code; virtual
APIs should supply the environment rather than replace behavior under test.

## Full retail boot (HUD slot)

`slots/hud/hudworld.py` boots the shipped game by its own code: CRT, all
571 static constructors, then WinMain through game init (DirectDraw, the
640x480x16 display, fonts, 3D scene, rules, classes, widgets). It stops
before the intro movie. The slot's Win32/DirectDraw/GDI environment and
the core changes it needed (`api_dlls`, mutexes, `ERROR_NEGATIVE_SEEK`,
`CreateThread` flags, `call_sp`) are listed in
[slots/hud/README.md](slots/hud/README.md).

## Checkpoint restore: native dirty pages

`checkpoint()` records the CPU, heap, clock, files and virtual APIs; after
it, every guest page written is saved once (its content before the first
write) and `restore()` writes those pages back. The tracking is native
(2026-10-07, gameflow track): [`dirtypages.c`](dirtypages.c), a
`UC_HOOK_MEM_WRITE` callback in C, built with the system `cc` into
`__pycache__/` on first use ([`dirtypages.py`](dirtypages.py); the library
is keyed by the source's hash, so an edit rebuilds it and parallel sessions
share it). Before, a Python callback ran on every guest store. Without
`cc` the runtime keeps that Python hook (`vm._native_dirty is None`); the
pages and results are the same either way (`test_memory.py` runs both).

Measured on this Mac:

| | Python hook | native | no tracking |
|---|---|---|---|
| one guest store (microbenchmark) | 770–1020 ns | 130 ns | 100 ns |
| gameflow script parse, forest.s (57M instructions, 68 labels) | 12.5 s | 4.0 s | 2.8 s |
| flame VFX probe, median warm pair / setup | 12.0 ms / 127 ms | 10.8 ms / 73 ms | — |

Flame's output surfaces were already outside the hook (`bulk_writes`), so
it gains less. Identical outputs: the parse dump hash and page count, and
the flame manifest apart from timings.

For code outside `runtime.py`: `vm.dirty_pages` (how many pages restore
will write) and `vm.forget_page(page)` (a page whose mapping went away;
`memory.py` calls it on release). `vm.dirty` remains the Python hook's dict.

Not used: Unicorn 2.1's copy-on-write context snapshot
(`UC_CTL_CONTEXT_MEMORY`). In 2.1.4 it refuses `mem_protect` after a
snapshot (`UC_ERR_ARG`; VirtualAlloc commit and decommit use it), and
unmapping part of a region; it brings back on restore a region that was
mapped and unmapped after the snapshot (mapping that address again fails
with `UC_ERR_MAP`); and the process crashed (SIGSEGV) reading memory after a
restore that followed a write and a new mapping beside a snapshotted page.

## Verify and benchmark

```sh
"$PY" -m unittest discover -s tools/retail_runtime -p 'test_*.py' -v
"$PY" tools/retail_runtime/test_runtime.py \
  --output recon/retail_asm/runtime/replay-verification.json
"$PY" tools/retail_runtime/benchmark.py recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --iterations 500 --output recon/retail_asm/runtime/warm-loop-benchmark.json
"$PY" tools/retail_runtime/trace_hud_bar.py recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --repeat 20 --output recon/retail_asm/runtime/hud-bar-trace.json
```

The current suite contains 64 tests, including 100 scenario replays, cross-page
memory reset, CPU and host-state
reset, 30 runs of an actual compiled C logging hook through virtual Win32 calls,
unknown API rejection, fault rollback, surface capture and a persistent JSONL
process serving multiple requests after invalid input, virtual sleep/multimedia
clocks, message/timer replay and stdcall callback delegation. Reports live under
`recon/retail_asm/runtime/`. The logger test needs the separate experimental
`experiments/winmain_skip/Revenant.hooked.exe`; it deliberately bypasses WinMain
after logging and does not demonstrate original game startup.

The original Classic HP bar kernel `0x54a5d0` also replays 48 configurations
(stats, direction and fade), 20 times each. Its draws are intercepted at
`0x414d70` and recorded. The first measurement averaged about 0.11 ms per loop.
This gives original clamp, slice, source rectangle and tint decisions. It does
not produce retail pixels or validate Revisited's rendering yet.

For another environment, create a Python venv with `unicorn` and `capstone`.
Current versions are recorded alongside the benchmark. The latter is used by
the shared PE tooling. DOSBox control is independent and was not used here.
