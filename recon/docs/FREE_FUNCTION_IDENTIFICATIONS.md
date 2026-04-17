# Ghidra Free-Function Identifications

Identified non-class (free) functions from the Ghidra decompile of Revenant.exe.
These don't appear in `recon/ghidra/cls_*.cpp` because OOAnalyzer only classifies
methods belonging to structures — C-style functions stay as `FUN_*` in the raw
disassembly (`recon/ghidra/_data.txt`).

| Ghidra Symbol | Source Identity | Evidence |
|---|---|---|
| `entry` @ `0x58ed00` | `mainCRTStartup` (MSVC CRT stub) | XREFs to `GetVersion` (0x58ed33), `GetCommandLineA` (0x58ed93), `GetStartupInfoA` (0x58edbe), `GetModuleHandleA` (0x58ede1) — classic MSVC CRT startup sequence. Calls WinMain then `__exit`. |
| `FUN_004865a0` @ `0x4865a0` | `WinMain` | Writes `hInstance` to `DAT_00667c28` at `0x48660f` (same global `meth_0x4489a0`/TMainWindow::Initialize reads for `RegisterClassA`/`CreateWindowExA`/`LoadIconA`/`LoadCursorA`). Early calls to `InitializeCriticalSection` (0x4865b1), `SetErrorMode` (0x486615), `GetVersion` (0x486742). Multiple `PeekMessageA`/`TranslateMessage`/`DispatchMessageA` sites at 0x486918, 0x486b01, 0x486c2d, 0x486f1c, 0x4871ac, 0x487287 — logo-screen + main-game message loops. Spans ~0x4865a0–0x487290 (~3 KB), matches old `revmain.cpp` WinMain (InitSystem + logo + play loop + CloseSystem). |
| `FUN_00448cc0` @ `0x448cc0` | `WndProc` (main window procedure) | Assigned to `WNDCLASSA::lpfnWndProc` at `meth_0x4489a0:0x4489b2` (TMainWindow window-class registration). Reads `DAT_00668190` (windowed-mode flag) at 0x448d74/0x448de0. |

## How to extend this list

Free functions reveal themselves through:

- **XREFs in `_data.txt`** to Win32 APIs or well-known globals (`hInstance`, window handle, etc.).
- **Method-pointer assignments** (e.g. `lpfnWndProc`, `atexit`, callback-table entries).
- **Absence from any `cls_*.cpp`** — if `grep -l FUN_xxxxxxxx recon/ghidra/` only returns `_data.txt`, it's a free function.

Add new entries here as they're identified so future sessions don't re-derive them.
