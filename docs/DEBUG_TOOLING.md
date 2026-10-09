# Debug Tooling

How to run the engine under AddressSanitizer + UndefinedBehaviorSanitizer,
how headless runs start and stop, and what lldb needs on macOS. Logging
goes through `src/logging.h` (rxi `log.c`), to stdout and `revenant.log`
in the working directory.

## Toolchain: must be at least as new as macOS

The sanitizer runtimes ship with the compiler (`libclang_rt.asan_osx_dynamic.dylib`
inside the active Xcode), and a runtime older than the OS may not start.

What happened in October 2026: the machine was on macOS 26 while
`xcode-select` still pointed at Xcode 16.1 (Apple clang 16). Every ASan
binary, down to an empty `int main(void) { return 0; }`, aborted before
`main` with

```
AddressSanitizer: CHECK failed: sanitizer_malloc_mac.inc:189 "((!asan_init_is_running)) != (0)" (0x0, 0x0)
```

The crash report gave the cause:

```
__asan::AsanInitInternal → InitializeShadowMemory → MemoryRangeIsAvailable
  → MemoryMappingLayout::Next → __sanitizer::get_dyld_hdr
  → dyld_shared_cache_iterate_text_swift (dyld) → _Block_copy → malloc
  → __sanitizer_mz_malloc → CHECK(!asan_init_is_running)
```

On macOS 26, the dyld call that the old runtime uses to find dyld's own
header allocates memory. That allocation goes back into ASan's malloc zone
while ASan is still initializing. Nothing in our code or in the linked
frameworks was involved, and no environment variable changed the result.
Xcode 27 (Apple clang 21) ships a runtime that works on macOS 26. The same
stale Xcode also broke `xcrun` and the `/usr/bin/cc` shims, so a fresh CMake
configure failed too.

Configuring with `-DREVENANT_ASAN=ON` now compiles and runs a trivial
sanitized program (`REVENANT_SANITIZER_RUNTIME_STARTS`). If the runtime
can't start, configuration stops with an error that names the compiler,
instead of a full build producing a binary that dies before `main`.

After an Xcode update, existing build directories still have the old SDK
path cached. Drop it when reconfiguring:

```sh
cmake -U CMAKE_OSX_SYSROOT -S . -B build
cmake -U CMAKE_OSX_SYSROOT -S . -B build-asan
```

When a process dies before `main` or without a debugger attached, macOS
still writes a crash report with a symbolized backtrace:
`~/Library/Logs/DiagnosticReports/<binary>-<date>.ips` (JSON after the
first line; the crashing thread has `"triggered": true`).

## ASan + UBSan build

```sh
cmake -S . -B build-asan -DREVENANT_ASAN=ON
cmake --build build-asan -j 12
```

Flags: `-fsanitize=address,undefined -fno-sanitize=vptr -fsanitize-recover=address
-fno-omit-frame-pointer -O1 -g`. vptr checking is off for the reason given in
`CMakeLists.txt`.

Running (isolated, headless; see [Headless runs](#headless-runs) for the
environment variables):

```sh
REVENANT_SAVE_PATH=/tmp/rev-save \
  ./build-asan/Revenant --quickstart --headless --max-runtime=40 > asan.log 2>&1
grep -E "ERROR: AddressSanitizer|runtime error|SUMMARY:" asan.log
```

- **ASan stops at the first error** by default. To report past a known
  bug, for example to reach shutdown, set `ASAN_OPTIONS=halt_on_error=0`.
  This works because the build compiles with `-fsanitize-recover=address`.
  Each distinct error is still reported once.
- **UBSan reports** (`runtime error: …`) print a file:line and execution
  continues. Add `UBSAN_OPTIONS=print_stacktrace=1` for stacks. The packed
  1998 resource readers (`stream.h`, `resource.cpp`, `font.cpp`,
  `bitmapdecode.cpp`, …) make up most of the misaligned-load and
  misaligned-store reports.
- **Symbolization works without setup.** ASan uses `llvm-symbolizer` when
  it's on `PATH` and `atos` otherwise; `atos` prints `function file.cpp:line`.
  For full paths and columns, point ASan at LLVM's symbolizer:
  `ASAN_SYMBOLIZER_PATH=$(brew --prefix llvm)/bin/llvm-symbolizer`.
  Xcode doesn't ship `llvm-symbolizer`; a path that doesn't exist only
  produces `WARNING: invalid path to external symbolizer!` and offsets.
  No dSYM is built: the DWARF stays in the object files under `build-asan/`
  and is found through the binary's debug map. Keep the build directory as
  it was when the binary ran, or create a portable dSYM with
  `dsymutil build-asan/Revenant`.
- **Leak checking isn't available.** Apple's ASan runtime rejects
  `detect_leaks=1` ("not supported on this platform").
- On macOS, ASan calls `abort()` after a report (`abort_on_error`
  defaults to 1 on Apple platforms; the exit status is 134). A debugger
  therefore stops at the report, and macOS writes a crash report.

## Headless runs

`--headless` keeps the window off screen (our sokol_app patch:
`desc.hidden`, `desc.no_dock_icon`), hides the Dock icon and silences audio.
Snapshots read an offscreen render target, so `--snap` and `--filmstrip`
work without a visible window. A hidden, silent app with no Dock icon is
what macOS App Nap throttles, which held back the frame timer for minutes
(a run that never reached its first frame; another stalled ~10 min), so
`--headless` also takes an `NSProcessInfo` activity that opts out
(`HeadlessWindow::KeepAwake`, from `sokol_main`). The activity also holds
off idle system sleep: on 2026-10-09 an unattended run froze for ten
minutes because the Mac (on battery) idle-slept under it, about five
seconds after the display turned off (`pmset -g log` shows "Entering Sleep
state due to 'Idle Sleep'"). The display may still sleep.

A headless run paces its own frames. MTKView draws from the display's
refresh, which stops while the Mac's display sleeps. A run started in
that state logged "logging initialized" and then sat in the event loop
until its watchdog fired (2026-10-08: arena runs and builds hung for
minutes or more). For `desc.hidden` our sokol patch therefore pauses the
view and draws it from a run-loop timer at the display's rate
(`headlessTimerFired:`). Verified with the display on (the arena's 1440
ticks in ~41 s), and a run kept logging with the display off until the
system slept (2026-10-09). If a run stalls with the display off and the
Mac awake, look next at `CAMetalLayer nextDrawable` blocking on a layer
that is never composited.

Each of these quit requests ends the process through the normal path
(`AppCleanup` → `ShutdownGlobals`):

- the title screen's Exit, or `--nointro --menu=exit`;
- a finished `--snap` or `--filmstrip`;
- the end of an `--input-script`;
- any other `sapp_request_quit()`.

The log ends with `[shutdown] begin` and `[shutdown] complete`.

`--max-runtime=N` is only a watchdog. It hard-exits with `std::_Exit(0)`
after N seconds and skips all teardown. A run that ends this way never
runs the shutdown code, so pair it with a real quit trigger and use it
only as a safety net.

How the quit works: sokol_app quits by sending `performClose:` to its
window and lets AppKit terminate the app after the last window closes.
AppKit doesn't do that for a window that was never on screen. For
`desc.hidden`, our sokol patch therefore terminates explicitly
(`windowWillClose:` in `_sapp_macos_window_delegate`), and
`HeadlessWindow::HideAllWindows()` must leave the window closable:
`performClose:` does nothing for a window without a close button.

Isolating a test run:

- `REVENANT_SAVE_PATH=<dir>`: per-user data (INI, `curmap/`, saves). Copy
  `~/Library/Application Support/Revenant/Revenant.ini` into it first.
- `REVENANT_DATA_PATH=<checkout>/data`: read-only game data. Needed in
  worktrees, where `data/*.rvr` can be Git LFS pointer files.
- `revenant.log` and `imgui.ini` land in the working directory; run from
  the repository root (gitignored there) or a scratch directory. The
  TrueType fonts are engine assets found from the executable
  (`rev_engine_asset`, DATA_LAYOUT.md §5), so any directory works.

Examples:

```sh
# Title screen → Exit; clean shutdown under ASan.
REVENANT_SAVE_PATH=/tmp/rev-save ./build-asan/Revenant --headless --nointro --menu=exit

# New game, one snapshot after 120 frames, then a clean quit through the
# PlayScreen/world teardown.
REVENANT_SAVE_PATH=/tmp/rev-save ASAN_OPTIONS=halt_on_error=0 \
  ./build-asan/Revenant --quickstart --headless --snapwarmup=120 --snap=/tmp/qs.png
```

## lldb

On this machine `DevToolsSecurity -status` reports *Developer mode is
currently disabled*. In that state `debugserver` needs an administrator
authorization (the `system.privilege.taskport` right) before it can control
a process, and macOS asks for it in a GUI dialog. When lldb runs from an
agent, over SSH, or from any other shell where nobody sees that dialog,
`process launch` / `attach` waits indefinitely.

One-time fix, run by the user in a terminal (needs an admin password):

```sh
sudo DevToolsSecurity -enable
id -Gn | tr ' ' '\n' | grep -x _developer     # the user must be in _developer
# if not:  sudo dseditgroup -o edit -a "$USER" -t user _developer
```

Members of `_developer` can then debug their own processes without a
prompt. As an alternative, without enabling it system-wide, start lldb
once from Terminal.app and approve the dialog; the authorization lasts for
a while and then has to be given again.

The Developer Tools list in System Settings → Privacy & Security
(`spctl developer-mode enable-terminal` adds Terminal to it) is a separate
switch: apps on that list can run local software that doesn't pass
Gatekeeper's checks. It doesn't control the debugger authorization dialog.

Binaries built here are linker-signed ad hoc, without the hardened
runtime, so lldb can debug them once it is authorized; no `get-task-allow`
entitlement is needed. Then:

```sh
REVENANT_SAVE_PATH=/tmp/rev-save lldb -- ./build-asan/Revenant --headless --nointro
(lldb) run
```

lldb passes its environment to the process it launches. Because ASan
aborts after its report, lldb stops there with the full stack. Symbols
come from the object files through the debug map, as for `atos`.
None of the lldb steps could be verified here, because debugserver can't
be authorized in this environment.
