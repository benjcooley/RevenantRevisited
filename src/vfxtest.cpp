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
#include "logging.h"
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
    // Camera centred at world origin, kCam world-units back.
    Renderer->SetLight(0.6f, -0.6f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f, 0.25f);
    Renderer->SetAmbientColor(0.55f, 0.55f, 0.55f);
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
    // drains the FX queue at the tail.
    Renderer->BeginTilePass(0.08f, 0.10f, 0.13f, 1.0f);
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
// These four effects exist solely to validate the Phase 1 FX submission
// pipelines end-to-end. They are placeholders -- real per-effect ports
// in Phase 2 will replace them with the actual TFlameEffect /
// TBloodEffect / TStripEffect / TFlareAnimator implementations.
//
// FB / PE / SR / LS pipelines each get one test entry; smoke and ribbon
// also have static texture handles created lazily on first spawn.
//
// *************************************************************************

namespace {

TTextureHandle WhiteTexture()
{
    return Renderer ? Renderer->WhiteTextureHandle() : kInvalidTexture;
}

TTextureHandle OrangeFlameTexture()
{
    if (!Renderer) return kInvalidTexture;
    return Renderer->SolidColorTexture(0x4658464C414D45ull,  // "FXFLAME"
                                       0xFF1880FFu,           // ABGR: warm orange
                                       "vfx.flame.solid");
}

TTextureHandle GreyParticleTexture()
{
    if (!Renderer) return kInvalidTexture;
    return Renderer->SolidColorTexture(0x46584353504B45ull,  // "FXSMOKE"
                                       0xFFB0B0B0u,
                                       "vfx.smoke.solid");
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

// --- FB: torch-flame placeholder -----------------------------------------
// Mimics what TFlipbookBillboardComponent::Submit does for a flame at
// world origin. One persistent billboard with a tiny vertical "wobble".
struct SFlameCtx {
    float age = 0.0f;
};

void* FlameSpawn()    { return new SFlameCtx(); }
void  FlameDestroy(void* c) { delete static_cast<SFlameCtx*>(c); }
void  FlameSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SFlameCtx*>(cp);
    c->age += float(TTime::DeltaTime());
    SBillboardDrawItem it = {};
    it.world_pos[0] = 0.0f;
    it.world_pos[1] = 0.0f;
    it.world_pos[2] = 40.0f + 4.0f * std::sin(c->age * 6.0f);
    it.size_wu[0]   = 60.0f;
    it.size_wu[1]   = 100.0f + 8.0f * std::sin(c->age * 4.0f);
    it.color_rgba[0] = 1.0f;
    it.color_rgba[1] = 0.55f;
    it.color_rgba[2] = 0.18f;
    it.color_rgba[3] = 1.0f;
    it.uv_rect[0] = 0.0f; it.uv_rect[1] = 0.0f;
    it.uv_rect[2] = 1.0f; it.uv_rect[3] = 1.0f;
    it.key.texture     = OrangeFlameTexture();
    it.key.pipeline_id = uint16_t(EFxPipeline::Billboard);
    it.key.blend       = uint8_t(EFxBlend::Additive);
    it.key.depth_mode  = uint8_t(EFxDepthMode::TestNoWrite);
    it.debug_mode      = dbg;
    Renderer->SubmitFxBillboard(it);
}

// --- PE: smoke plume placeholder via TParticleBucket ---------------------
// Spawns ~32 particles in a column; recycles when they reach the top.
struct SSmokeCtx {
    TParticleBucket* bucket = nullptr;
    float age = 0.0f;
    float spawn_accum = 0.0f;
};

void* SmokeSpawn()
{
    auto* c = new SSmokeCtx();
    SParticleBucketDesc desc = {};
    desc.name = "vfx.smoke";
    desc.blend = EParticleBlendMode::Alpha;
    desc.texture = GreyParticleTexture();
    desc.texture_width = 1;
    desc.texture_height = 1;
    desc.default_width  = 28.0f;
    desc.default_height = 28.0f;
    SParticleBufferLayout layout = {};
    ParticleLayoutAddVar(layout, EParticleVar::OwnerId);
    ParticleLayoutAddVar(layout, EParticleVar::Life);
    ParticleLayoutAddVar(layout, EParticleVar::Age);
    ParticleLayoutAddVar(layout, EParticleVar::DrawPos);
    ParticleLayoutAddVar(layout, EParticleVar::DrawScl);
    ParticleLayoutAddVar(layout, EParticleVar::DrawColor);
    c->bucket = ParticleManager().GetOrCreateGlobalBucket(desc, layout);
    return c;
}

void SmokeDestroy(void* cp)
{
    auto* c = static_cast<SSmokeCtx*>(cp);
    if (c->bucket)
        c->bucket->KillParticlesByOwner(7.0f);
    delete c;
}

void SmokeSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SSmokeCtx*>(cp);
    if (!c->bucket) return;
    const float dt = float(TTime::DeltaTime());
    c->age += dt;

    // Advance existing particles (simple Euler rise).
    for (int32_t i = 0; i < c->bucket->Count(); ++i)
    {
        if (float* dp = c->bucket->VarPtr(i, EParticleVar::DrawPos))
            dp[2] += dt * 70.0f;
        if (float* age = c->bucket->VarPtr(i, EParticleVar::Age))
            *age += dt;
    }

    // Spawn ~12 / sec, cap at 32 live.
    c->spawn_accum += dt * 12.0f;
    while (c->spawn_accum >= 1.0f && c->bucket->Count() < 32)
    {
        c->spawn_accum -= 1.0f;
        const int32_t pi = c->bucket->AddParticle(7.0f, 1.5f);
        if (pi < 0) break;
        if (float* dp = c->bucket->VarPtr(pi, EParticleVar::DrawPos))
        {
            dp[0] = (float(std::rand()) / float(RAND_MAX) - 0.5f) * 20.0f;
            dp[1] = (float(std::rand()) / float(RAND_MAX) - 0.5f) * 20.0f;
            dp[2] = 0.0f;
        }
        if (float* dc = c->bucket->VarPtr(pi, EParticleVar::DrawColor))
        {
            dc[0] = 0.7f; dc[1] = 0.7f; dc[2] = 0.7f; dc[3] = 0.6f;
        }
        if (float* ds = c->bucket->VarPtr(pi, EParticleVar::DrawScl))
        {
            ds[0] = 28.0f; ds[1] = 28.0f;
        }
    }

    Renderer->SubmitFxParticleBucket(*c->bucket, dbg);
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
        flame.id       = "TFlameEffect.placeholder";
        flame.family   = "fire";
        flame.pipeline = "FB";
        flame.factory  = []() -> void* { return FlameSpawn(); };
        flame.submit   = [](void* c, EFxDebugMode d) { FlameSubmit(c, d); };
        flame.destroy  = [](void* c) { FlameDestroy(c); };
        VfxTest::DeferredRegister(flame);

        VfxTest::SEffect smoke = {};
        smoke.id       = "TSmokeEffect.placeholder";
        smoke.family   = "particles";
        smoke.pipeline = "PE";
        smoke.factory  = []() -> void* { return SmokeSpawn(); };
        smoke.submit   = [](void* c, EFxDebugMode d) { SmokeSubmit(c, d); };
        smoke.destroy  = [](void* c) { SmokeDestroy(c); };
        VfxTest::DeferredRegister(smoke);

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
