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
#include "revtypes.h"     // S3DPoint

class TObjectInstance;

namespace VfxTest {

// Preview style controls how the harness drives the effect over time --
// real game spawn patterns aren't a single continuous emitter. Combat
// effects accent strikes at varied screen positions; spell effects play
// once at the ground plane; ambient effects (torch) persist at a fixed
// spot. The factory is called each time the harness wants to (re)spawn
// the effect; the harness passes the world position to use.
enum class EVfxPreviewStyle : uint8_t
{
    Static,        // spawn once at origin, never re-trigger (torch, ambient)
    Combat,        // re-spawn every ~0.6s at a random spot in the visible area
                   // (hit flash, blood splat, swing arc) -- mimics combat cadence
    SpellGround,   // re-spawn every ~3s at the ground plane (cast/teleport burst)
    Projectile,    // re-spawn every ~1.2s travelling from a source point along a vector
};

// Per-effect registration record. `factory` is called to (re)spawn the
// effect at a harness-provided origin; it returns a context pointer the
// harness passes back into `submit` and `destroy`. `submit` is called
// once per frame while the effect is selected and the harness is
// unpaused.
struct SEffect
{
    std::string id;
    std::string family;
    std::string pipeline;   // "FB" / "PE" / "SR" / "LS" / "MP" / "VO" / "CX"
    EVfxPreviewStyle preview_style = EVfxPreviewStyle::Static;
    std::function<void*(const S3DPoint&)>                  factory;
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
