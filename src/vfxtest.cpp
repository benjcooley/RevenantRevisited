// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           vfxtest.cpp - --test=vfx browser harness                    *
// *************************************************************************
//
// Single-effect preview harness for the Phase 1 VFX restoration. See
// docs/vfx/TEST_MODE.md for the user-facing spec and
// docs/vfx/PHASE1_SPINE.md for the renderer API the registered effects
// submit into.
//
// Lifecycle:
//   --test=vfx -> TestModes::Initialize("vfx") -> VfxTest::Initialize
//   VfxTest::Initialize replays deferred registrations and spawns the
//   first effect's context. Render() runs the per-frame submit hook of
//   the active effect inside a minimal lit scene (ground tile + sun).
//   Key handling cycles effects, restarts, pauses, and steps debug mode.
//
// *************************************************************************

#include "vfxtest.h"

#include "display.h"
#include "effect.h"
#include "logging.h"
#include "object.h"
#include "particlefx.h"
#include "renderer.h"
#include "revenant.h"     // VK_LEFT, VK_RIGHT, VK_SPACE
#include "surface.h"
#include "time.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>

namespace {

struct SState
{
    std::vector<VfxTest::SEffect> deferred;
    std::vector<VfxTest::SEffect> catalogue;
    int32_t      active_idx       = -1;
    void*        active_ctx       = nullptr;
    EFxDebugMode debug_mode       = EFxDebugMode::Normal;
    bool         paused           = false;
    bool         step_once        = false;
    int64_t      frames_rendered  = 0;
    bool         initialized      = false;
    bool         first_submit_logged = false;
};

SState g_state;

void SpawnActive()
{
    if (g_state.active_idx < 0 || g_state.active_idx >= int32_t(g_state.catalogue.size()))
        return;
    const auto& e = g_state.catalogue[g_state.active_idx];
    if (e.factory)
        g_state.active_ctx = e.factory();
    g_state.first_submit_logged = false;
    log_info("[vfx] active='%s' family='%s' pipeline='%s' debug=%d",
             e.id.c_str(), e.family.c_str(), e.pipeline.c_str(),
             int(g_state.debug_mode));
}

void DestroyActive()
{
    if (g_state.active_idx < 0) return;
    const auto& e = g_state.catalogue[g_state.active_idx];
    if (e.destroy && g_state.active_ctx)
        e.destroy(g_state.active_ctx);
    g_state.active_ctx = nullptr;
}

void SelectIndex(int32_t idx)
{
    if (g_state.catalogue.empty()) return;
    const int32_t n = int32_t(g_state.catalogue.size());
    idx = ((idx % n) + n) % n;
    if (idx == g_state.active_idx) return;
    DestroyActive();
    g_state.active_idx = idx;
    SpawnActive();
}

void Restart()
{
    if (g_state.active_idx < 0) return;
    DestroyActive();
    SpawnActive();
    log_info("[vfx] restart idx=%d id='%s'",
             g_state.active_idx,
             g_state.catalogue[g_state.active_idx].id.c_str());
}

void CycleDebug()
{
    int v = int(g_state.debug_mode) + 1;
    if (v > int(EFxDebugMode::CurrentFrame)) v = 0;
    g_state.debug_mode = EFxDebugMode(v);
    log_info("[vfx] debug_mode -> %d", v);
}

}  // namespace

namespace VfxTest {

void DeferredRegister(const SEffect& effect)
{
    g_state.deferred.push_back(effect);
}

void Register(const SEffect& effect)
{
    g_state.catalogue.push_back(effect);
    std::sort(g_state.catalogue.begin(), g_state.catalogue.end(),
              [](const SEffect& a, const SEffect& b) {
                  if (a.family != b.family) return a.family < b.family;
                  return a.id < b.id;
              });
    // Selection by stored id (sort may have shuffled active index).
    if (g_state.active_idx >= 0 && !g_state.catalogue.empty())
    {
        const std::string active_id =
            g_state.catalogue[g_state.active_idx].id;
        for (int32_t i = 0; i < int32_t(g_state.catalogue.size()); ++i)
            if (g_state.catalogue[i].id == active_id) { g_state.active_idx = i; break; }
    }
}

bool Initialize()
{
    g_state.catalogue.clear();
    g_state.active_idx = -1;
    g_state.active_ctx = nullptr;
    g_state.frames_rendered = 0;
    g_state.paused = false;
    g_state.step_once = false;
    g_state.debug_mode = EFxDebugMode::Normal;
    g_state.first_submit_logged = false;

    for (const auto& e : g_state.deferred)
        Register(e);

    g_state.initialized = true;

    log_info("[vfx] --test=vfx initialize (%d effects registered)",
             int(g_state.catalogue.size()));
    for (size_t i = 0; i < g_state.catalogue.size(); ++i)
        log_info("[vfx]   [%zu] id='%s' family='%s' pipeline='%s'",
                 i,
                 g_state.catalogue[i].id.c_str(),
                 g_state.catalogue[i].family.c_str(),
                 g_state.catalogue[i].pipeline.c_str());

    if (!g_state.catalogue.empty())
    {
        g_state.active_idx = 0;
        SpawnActive();
    }
    else
    {
        log_warn("[vfx] no effects registered -- empty browser");
    }
    return true;
}

void Close()
{
    DestroyActive();
    g_state.catalogue.clear();
    g_state.active_idx = -1;
    g_state.initialized = false;
    log_info("[vfx] close (frames=%lld)", static_cast<long long>(g_state.frames_rendered));
}

void Render()
{
    if (!Renderer || !Display.IsActive() || !Display.BackBuffer())
        return;

    ++g_state.frames_rendered;

    const int32_t vw = Display.Width();
    const int32_t vh = Display.Height();
    const int32_t cam_ox = vw / 2;
    const int32_t cam_oy = vh / 2;

    // Neutral 3D scene: directional sun, modern lighting, no shadows/AO.
    // Camera centred at world origin, kCam world-units back. Scene is
    // deliberately bright + neutral so effects read clearly against the
    // background regardless of their own color; effects render in the
    // post-lighting fx_pass so the sun/ambient don't tint them anyway.
    Renderer->SetLight(0.6f, -0.6f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f, 0.85f);
    Renderer->SetAmbientColor(0.80f, 0.80f, 0.80f);
    Renderer->SetAmbientOcclusion(false, 12.0f, 1.0f, 0.15f, 96.0f);
    Renderer->SetNormalLightingHardness(1.0f);
    Renderer->SetLightingMode(1);
    Renderer->SetSunShadow(false, 24.0f, 3.0f, 32);
    Renderer->SetShadowWorldDir(0.6f, -0.6f, 0.4f);
    Renderer->SetShadowVariance(0.0f, 0.0f, 1.0f);

    constexpr float kCam  = 2750.0f;
    constexpr float zHalf =  512.0f;
    const float znear = kCam - zHalf;
    const float zfar  = kCam + zHalf;
    Renderer->SetReconstructionParams(float(cam_ox), float(cam_oy),
                                      znear, zfar, 0.0f, 0.0f, kCam, 0.0f);
    Renderer->ClearPointLights();

    // Step 1: active-effect submit hook. LS-pipeline effects call
    // AddPointLight here so RunLightingPass picks them up; FB / PE / SR
    // effects fill the fx queue, drained by DrainFxQueue inside
    // RunLightingPass.
    const bool should_tick = !g_state.paused || g_state.step_once;
    g_state.step_once = false;
    if (should_tick && g_state.active_idx >= 0 && g_state.active_ctx)
    {
        const auto& e = g_state.catalogue[g_state.active_idx];
        if (e.submit)
        {
            e.submit(g_state.active_ctx, g_state.debug_mode);
            if (!g_state.first_submit_logged)
            {
                g_state.first_submit_logged = true;
                log_info("[vfx] first submit: id='%s' pipeline='%s' debug=%d",
                         e.id.c_str(), e.pipeline.c_str(), int(g_state.debug_mode));
            }
        }
    }

    // Step 2: empty tile pass (clear-only) + lighting. RunLightingPass
    // drains the FX queue at the tail. Mid-gray clear so coloured
    // effects (red blood, yellow flame, white flare) all read clearly.
    Renderer->BeginTilePass(0.45f, 0.46f, 0.50f, 1.0f);
    Renderer->EndTilePass();
    Renderer->RunLightingPass();
}

void HandleKeyPress(int32_t key, bool down)
{
    if (!down) return;
    switch (key)
    {
        case VK_LEFT:    SelectIndex(g_state.active_idx - 1); break;
        case VK_RIGHT:   SelectIndex(g_state.active_idx + 1); break;
        case 'R':        Restart(); break;
        case VK_SPACE:   g_state.paused = !g_state.paused;
                         log_info("[vfx] paused=%d", g_state.paused ? 1 : 0); break;
        case '.':        if (g_state.paused) g_state.step_once = true; break;
        case 'D':        CycleDebug(); break;
        case '1':        g_state.debug_mode = EFxDebugMode::Normal;       log_info("[vfx] debug_mode -> 0"); break;
        case '2':        g_state.debug_mode = EFxDebugMode::SolidColor;   log_info("[vfx] debug_mode -> 1"); break;
        case '3':        g_state.debug_mode = EFxDebugMode::FullTexture;  log_info("[vfx] debug_mode -> 2"); break;
        case '4':        g_state.debug_mode = EFxDebugMode::CurrentFrame; log_info("[vfx] debug_mode -> 3"); break;
        default: break;
    }
}

}  // namespace VfxTest

// *************************************************************************
// * Built-in test effects                                                 *
// *************************************************************************
//
// These four effects validate the Phase 1 FX submission pipelines
// end-to-end. The FB (F01) and PE (B01) slots are real per-effect Phase 2
// ports (`TFlameEffect`, `TBloodEffect`) — the lambdas here are thin
// shims that defer all spawn / kinematic / draw work to the effect class.
// The SR (X16) and LS (X17) slots are still placeholders pending their
// own Phase 2 ports (`TStripEffect`, `TFlareAnimator`).
//
// Ribbon + flare each keep a static solid-color texture handle created
// lazily on first spawn; the real effects bring their own textures via
// the imagery cache (see TFlameEffect::SpawnForTest / TBloodEffect::SpawnForTest).
//
// *************************************************************************

namespace {

TTextureHandle WhiteTexture()
{
    return Renderer ? Renderer->WhiteTextureHandle() : kInvalidTexture;
}

TTextureHandle BlueRibbonTexture()
{
    if (!Renderer) return kInvalidTexture;
    return Renderer->SolidColorTexture(0x46585242424F4Eull,  // "FXRBBON"
                                       0xFFFFC080u,           // soft blue tint
                                       "vfx.ribbon.solid");
}

TTextureHandle GoldFlareTexture()
{
    if (!Renderer) return kInvalidTexture;
    return Renderer->SolidColorTexture(0x46584C4641524full,  // "FXLFLARO"
                                       0xFF40D8FFu,           // warm yellow
                                       "vfx.flare.solid");
}

// --- FB: real TFlameEffect (TorchFlame, Magic/flame.i3d) -----------------
// Spawns a sector-less TFlameEffect at world origin via SpawnForTest and
// drives its flipbook component each frame through the same Submit path
// the in-game per-instance walk uses (maprenderer.cpp Billboard branch).
// SpawnForTest is allowed to fail if the imagery archive isn't mounted /
// the asset is missing; in that case we register a no-op submit so the
// harness still navigates past F01 instead of crashing.
struct SFlameCtx {
    TFlameEffect* flame = nullptr;
};

void* FlameSpawn()
{
    auto* c = new SFlameCtx();
    c->flame = TFlameEffect::SpawnForTest(S3DPoint{0, 0, 0});
    if (!c->flame)
        log_warn("[vfx] TFlameEffect::SpawnForTest returned null; F01 entry will draw nothing");
    return c;
}

void FlameDestroy(void* cp)
{
    auto* c = static_cast<SFlameCtx*>(cp);
    delete c->flame;
    delete c;
}

void FlameSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SFlameCtx*>(cp);
    if (!c->flame || !Renderer)
        return;
    auto* flipbook = c->flame->GetComponent<TFlipbookBillboardComponent>();
    if (!flipbook || flipbook->Texture() == kInvalidTexture)
        return;
    flipbook->SetDebugMode(dbg);
    flipbook->Submit(*Renderer, *c->flame);
}

// --- PE: real TBloodEffect (bloodimagery, Misc/Blood.I3D) ---------------
// Spawns a sector-less TBloodEffect at world origin via SpawnForTest and
// drives its owned TParticleBucket through SubmitFxParticleBucket() each
// frame. Mirrors the F01 / TFlameEffect pattern: the harness lambda is a
// thin shim; all the spawn / kinematic / draw logic lives on the real
// effect class. See effect.cpp's TBloodEffect block for the scope
// boundary (Phase 2.2 = PE pipeline gate; Phase 2.2.1 = faithful retail
// kinematics).
struct SBloodCtx {
    TBloodEffect* blood = nullptr;
};

void* BloodSpawn()
{
    auto* c = new SBloodCtx();
    c->blood = TBloodEffect::SpawnForTest(S3DPoint{0, 0, 0});
    if (!c->blood)
        log_warn("[vfx] TBloodEffect::SpawnForTest returned null; B01 entry will draw nothing");
    return c;
}

void BloodDestroy(void* cp)
{
    auto* c = static_cast<SBloodCtx*>(cp);
    delete c->blood;
    delete c;
}

void BloodSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SBloodCtx*>(cp);
    if (!c->blood)
        return;
    c->blood->TickAndSubmitForTest(dbg);
}

// --- SR: static ribbon placeholder ---------------------------------------
struct SRibbonCtx {
    std::vector<SStripSegment> segs;
};

void* RibbonSpawn()
{
    auto* c = new SRibbonCtx();
    constexpr int32_t N = 16;
    c->segs.reserve(N);
    for (int32_t i = 0; i < N; ++i)
    {
        const float t0 = float(i)     / float(N);
        const float t1 = float(i + 1) / float(N);
        SStripSegment s = {};
        s.world_a[0] = -150.0f + 300.0f * t0;
        s.world_a[1] = 0.0f;
        s.world_a[2] = 60.0f + 30.0f * std::sin(t0 * 6.28f);
        s.world_b[0] = -150.0f + 300.0f * t1;
        s.world_b[1] = 0.0f;
        s.world_b[2] = 60.0f + 30.0f * std::sin(t1 * 6.28f);
        s.width_a_wu = 8.0f + 16.0f * t0;
        s.width_b_wu = 8.0f + 16.0f * t1;
        const float a0 = 1.0f - t0;
        const float a1 = 1.0f - t1;
        s.color_a[0] = 0.6f; s.color_a[1] = 0.8f; s.color_a[2] = 1.0f; s.color_a[3] = a0;
        s.color_b[0] = 0.6f; s.color_b[1] = 0.8f; s.color_b[2] = 1.0f; s.color_b[3] = a1;
        s.u_a = t0;
        s.u_b = t1;
        c->segs.push_back(s);
    }
    return c;
}

void RibbonDestroy(void* cp) { delete static_cast<SRibbonCtx*>(cp); }

void RibbonSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SRibbonCtx*>(cp);
    if (c->segs.empty()) return;
    SStripDrawItem it = {};
    it.segments     = c->segs.data();
    it.num_segments = int32_t(c->segs.size());
    it.key.texture     = BlueRibbonTexture();
    it.key.pipeline_id = uint16_t(EFxPipeline::Strip);
    it.key.blend       = uint8_t(EFxBlend::Alpha);
    it.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    it.debug_mode      = dbg;
    Renderer->SubmitFxStrip(it);
}

// --- LS: flare + dynamic point light placeholder -------------------------
struct SFlareCtx { float age = 0.0f; };

void* FlareSpawn() { return new SFlareCtx(); }
void  FlareDestroy(void* cp) { delete static_cast<SFlareCtx*>(cp); }

void FlareSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SFlareCtx*>(cp);
    c->age += float(TTime::DeltaTime());
    const float pulse = 0.7f + 0.3f * std::sin(c->age * 2.0f);

    // Submit a small additive billboard for the visible glow.
    SBillboardDrawItem it = {};
    it.world_pos[0] = 100.0f;
    it.world_pos[1] = 0.0f;
    it.world_pos[2] = 80.0f;
    it.size_wu[0]   = 50.0f * pulse;
    it.size_wu[1]   = 50.0f * pulse;
    it.color_rgba[0] = 1.0f;
    it.color_rgba[1] = 0.85f;
    it.color_rgba[2] = 0.25f;
    it.color_rgba[3] = pulse;
    it.uv_rect[0] = 0.0f; it.uv_rect[1] = 0.0f; it.uv_rect[2] = 1.0f; it.uv_rect[3] = 1.0f;
    it.key.texture     = GoldFlareTexture();
    it.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
    it.key.blend       = uint8_t(EFxBlend::Additive);
    it.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    it.debug_mode      = dbg;
    Renderer->SubmitFxBillboard(it);

    // And re-add a dynamic point light at the same world position. The
    // LS pipeline coupling is "effects re-add point lights each frame";
    // ClearPointLights ran at the top of VfxTest::Render.
    Renderer->AddPointLight(100.0f, 0.0f, 80.0f, 320.0f,
                            1.0f, 0.85f, 0.25f,
                            1.2f * pulse);
}

}  // namespace

// Defer registration until VfxTest::Initialize runs (renderer must
// exist for SolidColorTexture). The deferred queue is replayed at the
// top of Initialize.
namespace {
struct SVfxTestBootstrap {
    SVfxTestBootstrap() {
        VfxTest::SEffect flame = {};
        flame.id       = "TFlameEffect";
        flame.family   = "fire";
        flame.pipeline = "FB";
        flame.factory  = []() -> void* { return FlameSpawn(); };
        flame.submit   = [](void* c, EFxDebugMode d) { FlameSubmit(c, d); };
        flame.destroy  = [](void* c) { FlameDestroy(c); };
        VfxTest::DeferredRegister(flame);

        VfxTest::SEffect blood = {};
        blood.id       = "TBloodEffect";
        blood.family   = "blood";
        blood.pipeline = "PE";
        blood.factory  = []() -> void* { return BloodSpawn(); };
        blood.submit   = [](void* c, EFxDebugMode d) { BloodSubmit(c, d); };
        blood.destroy  = [](void* c) { BloodDestroy(c); };
        VfxTest::DeferredRegister(blood);

        VfxTest::SEffect ribbon = {};
        ribbon.id       = "TStripEffect.placeholder";
        ribbon.family   = "strip";
        ribbon.pipeline = "SR";
        ribbon.factory  = []() -> void* { return RibbonSpawn(); };
        ribbon.submit   = [](void* c, EFxDebugMode d) { RibbonSubmit(c, d); };
        ribbon.destroy  = [](void* c) { RibbonDestroy(c); };
        VfxTest::DeferredRegister(ribbon);

        VfxTest::SEffect flare = {};
        flare.id       = "TFlareAnimator.placeholder";
        flare.family   = "light";
        flare.pipeline = "LS";
        flare.factory  = []() -> void* { return FlareSpawn(); };
        flare.submit   = [](void* c, EFxDebugMode d) { FlareSubmit(c, d); };
        flare.destroy  = [](void* c) { FlareDestroy(c); };
        VfxTest::DeferredRegister(flare);
    }
};
SVfxTestBootstrap g_vfx_test_bootstrap;
}  // namespace
