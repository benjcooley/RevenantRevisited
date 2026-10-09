# HUD slot: the retail HUD in the emulator

The UI track's retail side for the HUD A/B (plan: `docs/ui/HUD_REBUILD.md`).
The shipped game boots by its own code (`hudworld.py`): CRT, the 571 static
constructors, then WinMain up to the end of game init. HUD fixtures
checkpoint that world and paint original panes into RAM surfaces.

```sh
PY=/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python
"$PY" tools/retail_runtime/slots/hud/hudworld.py recon/retail_asm/baseline/Revenant.rebuilt.exe
```

prints the boot summary: configuration flags, fonts created, boundaries,
threads. Boot takes about 95 s on this Mac (class loading dominates).
Fixtures pay that once per process, then restore per case.

## Configuration

A 16 MB 3D-capable DirectDraw card whose Direct3D offers no texture-capable
device. That is retail's own `DAT_00669ad8 == 0` path:
- texture overlays: `DAT_006680c8 == 0`, the Classic HUD, `StatusBar.dat`
  etc.
- the internal software rasterizer: `DAT_005d7a28 == 1`.

Retail forces the NoTex HUD in three cases: video memory of 8 MB or less, no
`DDCAPS_3D`, or SOFTWARE3D/BLUE (see HUD_REBUILD.md §3). A hardware player
saw the same composition rasterized by the D3D device. The device's
filtering and alignment are a recorded renderer limit.

## Modules

| Module | Answers |
|---|---|
| `hudworld.py` | boot sequence and boundaries: the boot log (recorded, no file I/O) and DirectPlay init (success, no network object) |
| `system.py` | the machine: Win98 version/locale paths, 64 MB, wall clock 1999-12-31 + virtual time, volumes, env vars, recorded `timeSetEvent` timers (never fire on their own), no DirectInput; MessageBoxA fails the run |
| `winfs.py` | the install read-only at `C:\REVENANT` (immutable `bytes`, shared by checkpoints), canonical paths, find/attributes/directories |
| `profile.py` | `GetPrivateProfile*` over the CD's Revenant.ini |
| `window.py` | the game window: class, HWND bound to its WndProc in the core queue, metrics |
| `ddraw.py` | DirectDraw 4 + clipper + Direct3D 3 device/viewport/material on RAM surfaces; `EnumDevices` offers the RGB emulation device through a guest thunk |
| `gdi.py`, `ttf.py` | fonts (LOGFONT kept), surface DCs, text metrics from the TrueType tables GDI uses (hdmx/VDMX); TextOutA/DrawTextA recorded, no glyphs yet |

Anything not implemented fails by name. Each module's docstring lists its
choices.

## Core changes this slot needed (feature/ui)

- `Runtime.api_dlls`: a slot handler can serve imports outside
  kernel32/user32/winmm (ddraw, gdi32, advapi32, ole32, dinput, mss32).
- Mutexes (`CreateMutexA` / `ReleaseMutex`) with recursive ownership in the
  scheduler (`test_threads.py`).
- `SetFilePointer` before the start sets `ERROR_NEGATIVE_SEEK`. The CRT's
  text-mode append open depends on it (`test_runtime.py`).
- `CreateThread` ignores flag bits it does not define, as Windows does.
  Retail's `_beginthreadex(..., 1, ...)` passes 1.
- `Runtime.call_sp`: host calls can start below a guest frame that stays
  live (WinMain stopped mid-way).

## Fixtures

Each fixture checkpoints the booted world and restores it per case; with
`--serve` it speaks JSONL (`{"id", "case"}` -> `{"id", "ok", "result"}`).

| Fixture | Runs |
|---|---|
| `plyrstatusbar.py` | TPlyrStatusBar (`0x0065a8c0`) over fixture characters: the frame, the last frame's primitives, the GDI text calls, and the blend masks |
| `draw_ab.py` | retail's `TSurface::Put` on explicit bytes: the oracle for retail's 2D blits and conversions (`--bitmaps` dumps StatusBar.dat's) |

Shared by the pane fixtures:

| Module | Does |
|---|---|
| `hudscene.py` | archives through retail's loader, fixture characters (stats, class, portrait), one frame of a pane, the back buffer |
| `overlayraster.py` | draws the overlay quads (T3DScene `0x00414550`) as the D3D device does: texels 1:1, modulated by the tint, alpha-blended. Retail's own fallback rasterizer ignores the tint's alpha and drifts a texel on wide quads |
| `blendmap.py` | marks the screen pixels drawn from texels retail blended in 4-bit steps while composing its ARGB4444 textures |

The port side and the compare are `tools/retail_ab/hud_ab.py` (run under
`caffeinate -du`; see docs/DEBUG_TOOLING.md).
