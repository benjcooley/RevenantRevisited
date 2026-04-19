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
| `FUN_00471ce0` @ `0x471ce0` | `TObjectInstance::LoadObject(RTInputStream, int version, BOOL ismap)` (static) | XREFs the string `s_Object_in_map_file_has_invalid_c_005d4954` at `0x471dd8` ("Object in map file has invalid class - possible file corruption" — our [object.cpp:1660](../../src/object.cpp#L1660)). Also xrefs `s_Bad_object_005d4994` and `s_Object_unique_id_0x%x_not_found_005d49a4`. Callers: (a) `cls_0x45f7c0::meth_0x48df70` lines 2490/2506 — the **save-game loader** (strings `_Default_Save_`, `_newgame_sav`, `Invalid_save_file__s_in_save_dir`; reads 0x80-byte save header before object loop); (b) `cls_0x5ba0b4` lines 816/923 — **network object-sync** path, `FUN_00471ce0(…, 0xf, 1)` with `version=15` hardcoded, gated by `DAT_00676828/0067682c` net-connected flags; (c) `cls_0x5b8e94` line 1364 — likely `forcesimple` recursive fallback inside LoadObject itself. Sig matches 1998 source [Revenant/object.cpp:1583](/Users/benjamincooley/projects/Revenant/object.cpp#L1583). |
| `FUN_00498780` @ `0x498780` | `TSector::Load(TSector*, RTInputStream, BOOL ismap)` (method) | XREFs the MAP FCC constant `DAT_005da750` ("MAP " = `0x20504150`) at `0x498788`. Reads `first`/`version`/`numobjects` int32s; `if (version > 13)` reads an extra int32 stored at `TSector+0xb4` — this is the v14+ `statehash` fingerprint field (see `recon/docs/SECTOR_FILE_FORMAT.md`). Loops `numobjects` calls to `FUN_00471ce0(stream, version, ismap)`. |
| `FUN_00498c90` @ `0x498c90` | `TSector::Save(TSector*, ROutputStream)` (method) | Two xrefs to MAP FCC `DAT_005da750`: first pass computes the new hash (no real write), second pass writes the final header. Calls `FUN_00499e90` to regenerate the `statehash` at save time. Writes `[MAP "][version=0xf][statehash][numobjects][objects…]` for v14+. |
| `FUN_00499e90` @ `0x499e90` | `TSector statehash computation` (static helper) | Hash regenerator called by `TSector::Save`. Feeds sector coords (level/sectorx/sectory), numobjects, and the serialized body of state-bearing objects (obj classes `0xb`/`0xc` — TCharacter/TPlayer) through hash-family helpers `FUN_0056ff60`/`FUN_0056ff80`/`FUN_0049cdd0` (signature resembles CRC32 but algorithm is unconfirmed; used as a change-detection fingerprint, not integrity-cryptographic). |

**Important:** All `FUN_*` addresses above (and every snapshot in `recon/ghidra/cls_*.cpp`) are taken from `data/Revenant.exe` in Ghidra project `RevenantDev`. Other exes in the tree (`legacy/Revenant.exe`, `RevenantRepo/Revenant.exe`, `Revenant backup.exe`) are different builds with different address spaces and feature sets (e.g., the 1.1 MB builds lack the v14+ `statehash` field). Never cross-reference `FUN_*` addresses across different exes.


## How to extend this list

Free functions reveal themselves through:

- **XREFs in `_data.txt`** to Win32 APIs or well-known globals (`hInstance`, window handle, etc.).
- **Method-pointer assignments** (e.g. `lpfnWndProc`, `atexit`, callback-table entries).
- **Absence from any `cls_*.cpp`** — if `grep -l FUN_xxxxxxxx recon/ghidra/` only returns `_data.txt`, it's a free function.

Add new entries here as they're identified so future sessions don't re-derive them.
