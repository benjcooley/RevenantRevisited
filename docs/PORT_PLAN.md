# Port Plan

Roadmap for getting the 1998 Revenant source in `src/` compiling, then
running, on modern macOS via sokol. Companion to
[ARCHITECTURE.md](ARCHITECTURE.md) (how the engine is structured) and
[recon/docs/PIPELINE.md](../recon/docs/PIPELINE.md) (how we recover
missing features from the release binary).

## 1. Current state

- **Source:** 194 files in `src/`, an earlier pre-release snapshot. C++17 required per `CMakeLists.txt`. Build fails — DirectX types still referenced.
- **Sokol migration (partial):** `TDisplay`, `TSurface`, `BlitHandler` have been ported to `sokol_gfx` with direct sg_ handle members (no wrapper layer, value semantics). `HandmadeMath` replaces `D3DMATRIX`.
- **Dead code:** ~18 files are entirely wrapped in `#if 0` (DirectDraw, DirectInput, most DirectSound, MCI CD audio, legacy asm). Another ~38 files still have live DirectX/Win32 references outside `#if 0`.
- **Modernization ~40%:** `nullptr` universal (1200+ sites), `#pragma once` on ~75% of headers, `constexpr` in spots, T/S/P class-prefix convention preserved. **Not yet adopted:** smart pointers, `auto`, range-for, `static_cast`/`dynamic_cast`, STL containers. Raw `new`/`delete` still dominant (~438 sites).

## 2. Style conventions (enforce for new/touched code)

Follow what's already established — don't invent new patterns mid-port.

- **Type naming:** keep `T`-prefix for classes, `S`-prefix for structs, `P`-prefix for pointer typedefs.
- **Methods:** PascalCase (`Initialize()`, `GetBitmap()`).
- **Headers:** `#pragma once` (migrate stragglers as touched).
- **Sokol abstraction:** store `sg_image` / `sg_pipeline` / `sg_buffer` directly as members — no wrapper class, no smart pointer, no adapter layer. Cleanup via `sg_destroy_*`. This matches `TSurface`.
- **Ownership:** current code is raw `new`/`delete`. Don't retrofit smart pointers on untouched files, but prefer RAII / value semantics / `std::unique_ptr` for *new* code and when rewriting a subsystem.
- **Modern C++:** prefer `nullptr`, `constexpr`, `enum class`, `static_cast`, `override`, `std::string` / `std::vector` when the change is local to the file you're already editing. Don't go on sweeps.
- **Macros:** don't reintroduce removed macros. The `_CLASSDEF` / `_STRUCTDEF` / `_PTRDEF` typedef macros in `src/revtypes.h` and the `min`/`max` macros are flagged for eventual removal but left alone unless we're already in the file.
- **Dead code:** `#if 0` blocks mark abandoned DirectX paths. Delete them when the replacement is in and verified — don't carry them forward.
- **Completion markers:** currently the codebase uses `#if 0` as the only incompleteness marker. Going forward, prefer `// TODO(port):` comments so work-in-progress is greppable.

## 3. Subsystem inventory

Mapped from the Direct3D / Windows API dependency audit.

| # | Subsystem                       | State             | Blocking types                                                              | Scope      | Replacement                                                                                   |
|---|---------------------------------|-------------------|-----------------------------------------------------------------------------|------------|-----------------------------------------------------------------------------------------------|
| 1 | Win32 windowing + message loop  | untouched         | `HWND`, `HINSTANCE`, `WNDPROC`, `WPARAM`/`LPARAM`, `LRESULT`                | 2 files    | `sokol_app` events → `TScreen`                                                                 |
| 2a | Direct3D — **animated** 3D (characters, effects, projectiles) | untouched | `LPDIRECT3DEXECUTEBUFFER`, `D3DVERTEXTYPE`, `D3DPRIMITIVETYPE`, `D3DMATERIALHANDLE`, `HRESULT` | 7+ files | Standard `sokol_gfx` pipelines + vertex buffers; `HandmadeMath` for transforms. Straightforward. |
| 2b | DLS — static-tile per-pixel lighting (the renderer that was too slow to ship in 1999) | untouched; spec lives as MMX in `src/dls.cpp` | n/a — CPU SIMD, not a DirectX type issue | 1 main file + tile compositing | **One Metal/sokol_gfx pixel shader.** Port the algorithm from the MMX code; delete MMX once shader verified. Do not recreate the shipped triangle-grid approximation. |
| 3 | DirectDraw surfaces (stragglers) | partly migrated  | `LPDIRECTDRAWSURFACE`, `DDSURFACEDESC`, `LPDIRECTDRAWCLIPPER`               | 3 files    | Already on `sg_image`; finish converting `3dimage.h::S3DTex`, `multisurface.h::GetSGImage`     |
| 4 | Win32 threading + synchronization | untouched       | `CRITICAL_SECTION`, `HANDLE`, `CreateThread`, `CreateEvent`, `CreateMutex`, `WaitForSingleObject` | 5 files | `std::thread`, `std::mutex`, `std::condition_variable`, `std::binary_semaphore`                 |
| 5 | Win32 config / INI              | partly modern     | `GetPrivateProfileString/Int`                                               | 2 files    | Small INI parser (`simpleini`) or switch to JSON; `std::filesystem` for path handling           |
| 6 | DirectSound                     | mostly stubbed    | `LPDIRECTSOUND`, `LPDIRECTSOUNDBUFFER`, `LPDIRECTSOUND3DBUFFER`             | 1 header, deferred | `miniaudio` (header-only) or `sokol_audio`                                              |
| 7 | DirectInput                     | fully stubbed     | `LPDIRECTINPUT`, `DIJOYSTATE`                                               | 1 file, deferred | `sokol_app` keyboard/mouse; `GLFW`/IOKit for gamepad later                                   |
| 8 | DirectPlay (multiplayer)        | unknown / deferred | TBD                                                                        | TBD        | Deferred — multiplayer was "terrible" per author; revisit post-singleplayer                   |
| 9 | MMX / inline asm (non-DLS)      | delete            | `_mm_*`, `__asm` in `bitmap.cpp`, `graphics.cpp`, `automap.cpp`, `revmain.cpp`, `chunkcache.cpp` | 5 files | Experimental; not in shipped build. **Delete on contact.** No fallback needed. |
| 10 | `<windows.h>` / COM misc        | scattered        | `#include <windows.h>` in 13 files                                          | broad      | Remove per file as the above subsystems are addressed                                          |

## 4. Phased plan

### Phase 2 — Clean compile (not yet runnable)

Goal: `cmake --build build` exits 0. No runtime expectation.

1. **`platform_compat.h`** — new header providing minimal shim types: `HRESULT` → `int` alias with `SUCCEEDED`/`FAILED` macros (or drop entirely and convert call-sites to `bool`); drop or alias Win32 handle types where the calling code is about to be rewritten. Prefer *conversion* over *shim* where scope is small.
2. **Subsystem 1 (windowing):** rewrite `src/mainwnd.h/.cpp` and `src/revmain.cpp` entry around `sokol_app`. Route events to `TScreen::HandleInput`.
3. **Subsystem 2a (animated 3D):** convert `src/3dscene.h/.cpp`, `src/3dimage.h/.cpp`, and effect files (`effect.cpp`, `missileeffect.cpp`, `stripeffect.cpp`) from Execute Buffer model to `sokol_gfx` pipeline + vertex buffer. Type aliases for the value types (`D3DVERTEXTYPE`, `D3DPRIMITIVETYPE`) can land in `platform_compat.h` if it keeps the diff bounded. This is standard triangle rendering — no shader novelty.
   *Subsystem 2b (DLS static-tile shader) is Phase 3 work — not required to reach compile.*
4. **Subsystem 3 (DD stragglers):** replace remaining `LPDIRECTDRAWSURFACE` in `src/3dimage.h` and `src/multisurface.h` with `sg_image` / a thin owned handle type.
5. **Subsystem 5 (INI):** replace `GetPrivateProfileString/Int` with a small portable INI reader (vendor `simpleini` into `thirdparty/`) — isolated, low risk.
6. **Subsystems 6, 7, 8:** *stub only* — empty implementations behind the existing classes so the link succeeds. No real audio/input/multiplayer yet.
7. **Remove `<windows.h>` includes** as their callers are rewired. Delete `#if 0` DirectX blocks whose replacements are in.

**Exit criterion:** clean `cmake --build build` on macOS. Likely the executable crashes immediately on launch — that's fine.

### Phase 3 — First pixel

1. **Subsystem 4 (threading):** replace the loader / timer / update threads with `std::thread` + `std::condition_variable`. `TImagery` loader, `TMapPane` update, and `Timer` are the main consumers.
2. **Asset paths:** point config at `data/` (the former `RevenantBin/`). Verify the startup sector / imagery / character-class files load — this is the first real compatibility test against the GOG data set.
3. **First render — animated objects:** reach a rendered frame of the title screen or an in-game sector with animated 3D working. Expect format issues — triage by whether it's a code bug vs. an incompatibility between our pre-release snapshot and the shipped 1998 data formats.
4. **DLS static-tile pixel shader (Subsystem 2b):** port the lighting algorithm from `src/dls.cpp`'s MMX code into a Metal/sokol_gfx pixel shader that runs per-pixel over the static tile imagery with the Z-buffer. This is the renderer that was too slow to ship in 1999 — modern GPU makes it free. Delete the MMX source once the shader output matches expectations.
5. **Input:** hook `sokol_app` keyboard/mouse into the game command system (`GAMECMD_*`).
6. **Audio:** swap `miniaudio` in behind `TSoundPlayer`. CD audio (MCI) stays stubbed — replace with ogg loader pointed at `data/Music/`.

**Exit criterion:** game launches, renders, and accepts input on macOS.

### Phase 4 — Feature recovery from Ghidra

Only after Phase 3 is stable.

1. Use `recon/` to diff shipped release behavior against our pre-release snapshot. Prioritize the systems the user called out as WIP at snapshot time:
   - scripting (TScriptManager)
   - spell effects
   - automap
   - in-game editor
   - (multiplayer deferred — author notes it shipped broken)
2. Port missing functions and fields per the mappings in `recon/mappings/`. One subsystem per commit; verify against `recon/classes_readable/` as the reference.

### Phase 5 — Cleanup / presentability

Rolling, not a single phase: as each file is touched, fold in:
- Remove surviving `#if 0` blocks whose content is obsolete.
- Migrate `new`/`delete` → `std::unique_ptr` where ownership is clear.
- Convert raw arrays → `std::vector` / `std::array` where bounds are dynamic / fixed.
- Replace C-style casts with `static_cast` / `reinterpret_cast` at touch sites.
- Kill the `_CLASSDEF` / `_STRUCTDEF` / `_PTRDEF` macros once a subsystem no longer depends on the generated `PT*` pointer typedefs.

## 5. Risks

- **Data compatibility.** The `src/` snapshot predates release by ~6 months; `data/` is the shipped 1998 set. Animation / imagery / save / sector formats may have shifted late in development. Expect loader failures in Phase 3 that require reading release-era struct layouts out of `recon/` to repair.
- **Sokol coverage gap.** `sokol_gfx` is a good Metal abstraction but provides no 3D math, no mesh loading, and no scene management — those all have to come from the existing engine code or `HandmadeMath`.
- **DLS shader authoring.** The `src/dls.cpp` MMX code is the spec for the static-tile per-pixel lighting shader. It's the one non-trivial shader in the port, and the whole visual identity of static backgrounds depends on it. Allow time to get it right; keep the MMX source readable until the shader matches.
- **Renderer fork is intentional and final.** We're resurrecting the *pre-release ambitious* renderer (per-pixel DLS via shader), not the *shipped compromise* (overlapped triangle grid with lit vertices). Two reasons: (1) modern GPUs make the original algorithm essentially free, so it produces a visually better result with no performance concern; (2) the shipped triangle-grid approach is genuinely complex and not tractable to reconstruct from the `recon/ghidra/` decomp. `recon/ghidra/`'s renderer classes are therefore reference material for gameplay systems only, never for the render pipeline.
- **Multiplayer via DirectPlay.** Not replaced anywhere in sokol's ecosystem. Deferred.

## 6. What's explicitly not in scope

- Windows / Linux builds (macOS first; add later).
- Cross-compilation.
- Level editor UI modernization beyond what it takes to not crash.
- Content changes (no new levels, art, sound, story).
- Rewriting the scripting language.
