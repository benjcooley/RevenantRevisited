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
// TODO(port): remove when Subsystems 2a and 3 land.
using HRESULT = int32_t;
#define SUCCEEDED(hr) ((HRESULT)(hr) >= 0)
#define FAILED(hr)    ((HRESULT)(hr) < 0)
