// *************************************************************************
// *                      RevenantRevisited (port)                         *
// *   platform_compat.h - Temporary Phase 2 shims for Win32/DirectX types *
// *                                                                       *
// *  Each alias in this file is scheduled for removal as its subsystem    *
// *  lands. Do not add new shims here without a paired TODO(port)         *
// *  comment identifying when it goes away. See docs/PORT_PLAN.md.        *
// *************************************************************************

#pragma once

#include <cstdint>

// ---- Subsystem 4 (threading) — Phase 3 ----
// Win32 opaque handle. Used by the thread / event / mutex / semaphore API.
// Removed when the threading port lands: worker pool + async completions
// replace the `HANDLE`/`WaitForSingleObject` model entirely.
// TODO(port): remove with Subsystem 4.
using HANDLE = void*;

// ---- Subsystem 2a (animated 3D) / Subsystem 3 (DD stragglers) — Phase 2 ----
// COM return code. Most call sites only check succeeded/failed, so during
// each subsystem rewrite the signature converts to `bool` or the value is
// dropped entirely. Retained as a shim only while legacy DirectX call
// sites still exist.
//
// On macOS/Linux there's no Windows SDK, so we define HRESULT ourselves. On
// Windows it must agree with <winnt.h>, which declares `typedef long HRESULT;`
// behind the `_HRESULT_DEFINED` guard. A handful of TUs (sokol_imgui /
// sokol_impl) end up seeing both this header and <winnt.h>, so we reuse that
// guard and match its `long` type: whichever header is parsed first wins and
// the other skips. (int32_t is `int`, which MSVC rejects as a conflicting
// redefinition vs. `long`.) This keeps windows.h out of the general TUs --
// it stays confined to where sokol does its thing.
// TODO(port): remove when Subsystems 2a and 3 land.
#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
#if defined(_WIN32)
using HRESULT = long;      // matches <winnt.h>
#else
using HRESULT = int32_t;
#endif
#endif
#ifndef SUCCEEDED
#define SUCCEEDED(hr) ((HRESULT)(hr) >= 0)
#endif
#ifndef FAILED
#define FAILED(hr)    ((HRESULT)(hr) < 0)
#endif
