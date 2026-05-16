// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           vfxtest.h - --test=vfx browser harness                      *
// *************************************************************************
//
// The --test=vfx mode boots a single-effect preview that exercises the
// Phase 1 FX submission API (see docs/vfx/PHASE1_SPINE.md and
// docs/vfx/TEST_MODE.md).
//
// Effects register themselves with this module at startup using
// VfxTest::Register; the browser builds a sorted catalogue and lets the
// user step through them with Left/Right arrows. Each entry is a tiny
// data record + factory + per-frame submit hook. The harness owns the
// active effect's lifecycle (spawn / destroy / restart).
//
// *************************************************************************

#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "renderer.h"

class TObjectInstance;

namespace VfxTest {

// Per-effect registration record. `factory` is called to (re)spawn the
// effect; it returns a context pointer the harness passes back into
// `submit` and `destroy`. `submit` is called once per frame while the
// effect is selected and the harness is unpaused; it should call into
// Renderer->SubmitFx* and/or Renderer->AddPointLight as appropriate.
struct SEffect
{
    std::string id;
    std::string family;
    std::string pipeline;   // "FB" / "PE" / "SR" / "LS" / "MP" / "VO" / "CX"
    std::function<void*()>                                 factory;
    std::function<void(void* ctx, EFxDebugMode dbg)>       submit;
    std::function<void(void* ctx)>                         destroy;
};

void Register(const SEffect& effect);

bool Initialize();
void Close();
void Render();
void HandleKeyPress(int32_t key, bool down);

// The map renderer's flame builder spawns a TFlameEffect when the
// owning T3DImagery initializes. The harness needs to defer registration
// until after the renderer is alive (TFlameEffect uses real GPU
// textures). Callers register here at process startup; we replay the
// registrations into VfxTest::Register the first time the browser
// initializes.
void DeferredRegister(const SEffect& effect);

}  // namespace VfxTest
