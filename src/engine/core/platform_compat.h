// *************************************************************************
// *                      RevenantRevisited (port)                         *
// *   platform_compat.h - Temporary Phase 2 shims for Win32/DirectX types *
// *                                                                       *
// *  Each alias in this file is scheduled for removal as its subsystem    *
// *  lands. Do not add new shims here without a paired TODO(port)         *
// *  comment identifying when it goes away. See docs/PORT_PLAN.md.        *
// *************************************************************************

#pragma once

#include <cstddef>
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
// TODO(port): remove when Subsystems 2a and 3 land.
// On Windows this must agree with <winnt.h>, which TUs reaching the OS
// (miniaudio, sokol) also include; the macros are spelled as the SDK's so
// either may come first.
#if defined(_WIN32)
using HRESULT = long;
#else
using HRESULT = int32_t;
#endif
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#define FAILED(hr)    (((HRESULT)(hr)) < 0)

// Case-insensitive compares under the names the 1998 code uses. The
// Windows C runtime declares both itself (its "oldnames", extern "C", so
// a C++ declaration of ours would clash); everywhere else they are ours.
#if defined(_WIN32)
#include <string.h>
#else
int32_t stricmp(const char* s1, const char* s2);
int32_t strnicmp(const char* s1, const char* s2, size_t n);
#endif

// The 1998 critical section around the shared caches. The port is single
// threaded (threading is a later subsystem), so both are no-ops.
void BEGIN_CRITICAL();
void END_CRITICAL();
