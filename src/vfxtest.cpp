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
#include "imgui.h"
#include "logging.h"
#include "object.h"
#include "particlefx.h"
#include "renderer.h"
#include "revenant.h"     // VK_LEFT, VK_RIGHT, VK_SPACE
#include "stripeffect.h"  // TStripEffect (S01 SR-pipeline port)
#include "surface.h"
#include "testconfig.h"  // StartupVfxId, StartupVfxHideUi
#include "time.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
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
    // Retrigger pattern state -- driven by the active effect's
    // EVfxPreviewStyle. Resets on SelectIndex / Restart.
    float        retrigger_timer  = 0.0f;
};

SState g_state;

// Per-style retrigger cadence + position policy. Static = never re-fires.
S3DPoint PickPreviewOrigin(VfxTest::EVfxPreviewStyle style)
{
    auto frand = []() { return float(std::rand()) / float(RAND_MAX); };
    switch (style)
    {
        case VfxTest::EVfxPreviewStyle::Static:
            return S3DPoint{0, 0, 0};
        case VfxTest::EVfxPreviewStyle::Combat:
        {
            // Random spot inside a ~240wu radius square at z=0 (the
            // visible scene area at the default iso camera). Combat
            // accents land all over the screen.
            constexpr float kHalf = 240.0f;
            return S3DPoint{ int32_t((frand() - 0.5f) * 2.0f * kHalf),
                             int32_t((frand() - 0.5f) * 2.0f * kHalf),
                             0 };
        }
        case VfxTest::EVfxPreviewStyle::SpellGround:
        {
            // Centred at origin, small jitter so successive casts
            // don't perfectly overlap. Spell casts read as ground-
            // level events: low Z, narrow XY range.
            constexpr float kJitter = 40.0f;
            return S3DPoint{ int32_t((frand() - 0.5f) * 2.0f * kJitter),
                             int32_t((frand() - 0.5f) * 2.0f * kJitter),
                             0 };
        }
        case VfxTest::EVfxPreviewStyle::Projectile:
            // TODO Phase 2.x: launch from one side toward the other
            // along a vector. Placeholder = static for now.
            return S3DPoint{-200, 0, 80};
    }
    return S3DPoint{0, 0, 0};
}

float RetriggerInterval(VfxTest::EVfxPreviewStyle style)
{
    switch (style)
    {
        case VfxTest::EVfxPreviewStyle::Static:       return 0.0f; // never
        case VfxTest::EVfxPreviewStyle::Combat:       return 0.6f;
        case VfxTest::EVfxPreviewStyle::SpellGround:  return 3.0f;
        case VfxTest::EVfxPreviewStyle::Projectile:   return 1.2f;
    }
    return 0.0f;
}

void SpawnActive()
{
    if (g_state.active_idx < 0 || g_state.active_idx >= int32_t(g_state.catalogue.size()))
        return;
    const auto& e = g_state.catalogue[g_state.active_idx];
    if (e.factory)
        g_state.active_ctx = e.factory(PickPreviewOrigin(e.preview_style));
    g_state.first_submit_logged = false;
    g_state.retrigger_timer = RetriggerInterval(e.preview_style);
    log_info("[vfx] active='%s' family='%s' pipeline='%s' style=%d debug=%d",
             e.id.c_str(), e.family.c_str(), e.pipeline.c_str(),
             int(e.preview_style), int(g_state.debug_mode));
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

const char* DebugModeName(EFxDebugMode m)
{
    switch (m)
    {
        case EFxDebugMode::Normal:       return "Normal";
        case EFxDebugMode::SolidColor:   return "Solid";
        case EFxDebugMode::FullTexture:  return "FullTex";
        case EFxDebugMode::CurrentFrame: return "CurrFrame";
    }
    return "?";
}

void DrawBrowserPanel()
{
    ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.86f);
    constexpr ImGuiWindowFlags kFlags = ImGuiWindowFlags_AlwaysAutoResize |
                                        ImGuiWindowFlags_NoSavedSettings |
                                        ImGuiWindowFlags_NoFocusOnAppearing;
    if (!ImGui::Begin("VFX Browser", nullptr, kFlags))
    {
        ImGui::End();
        return;
    }

    const int32_t n = int32_t(g_state.catalogue.size());

    if (n == 0)
    {
        ImGui::TextDisabled("(no effects registered)");
    }
    else
    {
        const int32_t cur = (g_state.active_idx < 0 || g_state.active_idx >= n)
            ? 0 : g_state.active_idx;
        const VfxTest::SEffect& sel = g_state.catalogue[cur];

        ImGui::Text("Effect %d / %d", cur + 1, n);
        ImGui::SetNextItemWidth(360.0f);
        if (ImGui::BeginCombo("##effect", sel.id.c_str()))
        {
            for (int32_t i = 0; i < n; ++i)
            {
                const bool selected = (i == cur);
                if (ImGui::Selectable(g_state.catalogue[i].id.c_str(), selected))
                    SelectIndex(i);
                if (selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        if (ImGui::ArrowButton("##prev", ImGuiDir_Left))  SelectIndex(cur - 1);
        ImGui::SameLine();
        if (ImGui::ArrowButton("##next", ImGuiDir_Right)) SelectIndex(cur + 1);

        ImGui::Separator();
        ImGui::Text("Family:   %s", sel.family.empty()   ? "-" : sel.family.c_str());
        ImGui::Text("Pipeline: %s", sel.pipeline.empty() ? "-" : sel.pipeline.c_str());
    }

    ImGui::Separator();
    int32_t mode = int32_t(g_state.debug_mode);
    ImGui::TextUnformatted("Debug:");
    ImGui::SameLine();
    if (ImGui::RadioButton("Normal",    &mode, 0)) g_state.debug_mode = EFxDebugMode::Normal;
    ImGui::SameLine();
    if (ImGui::RadioButton("Solid",     &mode, 1)) g_state.debug_mode = EFxDebugMode::SolidColor;
    ImGui::SameLine();
    if (ImGui::RadioButton("FullTex",   &mode, 2)) g_state.debug_mode = EFxDebugMode::FullTexture;
    ImGui::SameLine();
    if (ImGui::RadioButton("CurrFrame", &mode, 3)) g_state.debug_mode = EFxDebugMode::CurrentFrame;

    ImGui::Checkbox("Pause", &g_state.paused);
    ImGui::SameLine();
    if (ImGui::Button("Step"))    { if (g_state.paused) g_state.step_once = true; }
    ImGui::SameLine();
    if (ImGui::Button("Restart")) Restart();

    ImGui::Separator();
    ImGui::TextDisabled("frame=%lld  debug=%s%s",
                        static_cast<long long>(g_state.frames_rendered),
                        DebugModeName(g_state.debug_mode),
                        g_state.paused ? "  [paused]" : "");
    ImGui::TextDisabled("Keys: \xe2\x86\x90/\xe2\x86\x92  cycle   D  debug   "
                        "Space  pause   .  step   R  restart   Esc  quit");

    ImGui::End();
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
        // --vfx=<id> CLI flag: pre-select an effect by id (case-
        // sensitive match against catalogue entry's id). Falls back
        // to first alphabetically-sorted entry if no match.
        g_state.active_idx = 0;
        if (StartupVfxId[0])
        {
            bool matched = false;
            for (int32_t i = 0; i < int32_t(g_state.catalogue.size()); ++i)
            {
                if (g_state.catalogue[i].id == StartupVfxId)
                {
                    g_state.active_idx = i;
                    matched = true;
                    log_info("[vfx] --vfx='%s' matched entry %d",
                             StartupVfxId, i);
                    break;
                }
            }
            if (!matched)
                log_warn("[vfx] --vfx='%s' did not match any entry; "
                         "using default index 0", StartupVfxId);
        }
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
    if (should_tick && g_state.active_idx >= 0)
    {
        const auto& e = g_state.catalogue[g_state.active_idx];
        // Re-trigger pattern per preview style: non-Static styles
        // tick down a timer, destroy the previous instance and spawn
        // a fresh one at a new origin when the timer expires. This
        // is what makes a combat splat play repeatedly across the
        // visible area instead of firing once and never again.
        const float interval = RetriggerInterval(e.preview_style);
        if (interval > 0.0f)
        {
            g_state.retrigger_timer -= float(TTime::DeltaTime());
            if (g_state.retrigger_timer <= 0.0f)
            {
                if (e.destroy && g_state.active_ctx)
                    e.destroy(g_state.active_ctx);
                g_state.active_ctx = nullptr;
                if (e.factory)
                    g_state.active_ctx = e.factory(PickPreviewOrigin(e.preview_style));
                g_state.retrigger_timer = interval;
            }
        }
        if (e.submit && g_state.active_ctx)
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
    // drains the FX queue at the tail. Mid-gray clear -- per author's
    // diagnostic preference, the gray bg helps see particle quad shapes
    // against the bg (vs black where dim particles disappear).
    Renderer->BeginTilePass(0.45f, 0.46f, 0.50f, 1.0f);
    Renderer->EndTilePass();
    Renderer->RunLightingPass();

    if (!StartupVfxHideUi)
        DrawBrowserPanel();
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
// end-to-end. The FB (F01), PE (B01), and SR (S01) slots are real
// per-effect Phase 2 ports (`TFlameEffect`, `TBloodEffect`,
// `TStripEffect`) — the lambdas here are thin shims that defer all
// spawn / kinematic / draw work to the effect class. The LS (X17) slot
// is still a placeholder pending its own Phase 2 port (`TFlareAnimator`).
//
// The flare placeholder keeps a static solid-color texture handle created
// lazily on first spawn; the real effects bring their own textures via
// the imagery cache (see TFlameEffect::SpawnForTest / TBloodEffect::SpawnForTest)
// or fall back to the renderer's white texture handle (TStripEffect — the
// Magic\lightning.* asset hasn't been identified yet; tracked as a S01
// forensics gap).
//
// *************************************************************************

namespace {

TTextureHandle WhiteTexture()
{
    return Renderer ? Renderer->WhiteTextureHandle() : kInvalidTexture;
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

void* FlameSpawn(const S3DPoint& origin)
{
    auto* c = new SFlameCtx();
    c->flame = TFlameEffect::SpawnForTest(origin);
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

void* BloodSpawn(const S3DPoint& origin)
{
    auto* c = new SBloodCtx();
    c->blood = TBloodEffect::SpawnForTest(origin);
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

// --- SR: real TStripEffect (S01 lightning bolt) --------------------------
// Spawns a sector-less TStripEffect at the harness-provided origin via
// SpawnForTest and drives its segment ring through TickAndSubmitForTest
// each frame. Mirrors the F01 / B01 pattern: the harness lambda is a thin
// shim; all spawn / kinematic / draw logic lives on the real effect class.
// See src/stripeffect.cpp for the SR-pipeline scope boundary (Phase 2.3 =
// strip-only validator; glow + sparks composite lands in S01a).
struct SStripCtx {
    TStripEffect* strip = nullptr;
};

void* StripSpawn(const S3DPoint& origin)
{
    auto* c = new SStripCtx();
    c->strip = TStripEffect::SpawnForTest(origin);
    if (!c->strip)
        log_warn("[vfx] TStripEffect::SpawnForTest returned null; S01 entry will draw nothing");
    return c;
}

void StripDestroy(void* cp)
{
    auto* c = static_cast<SStripCtx*>(cp);
    delete c->strip;
    delete c;
}

void StripSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SStripCtx*>(cp);
    if (!c->strip)
        return;
    c->strip->TickAndSubmitForTest(dbg);
}

// --- FB: real TRippleEffect (procedural ring atlas, H03) ----------------
// Spawns a sector-less TRippleEffect at the harness-provided origin via
// SpawnForTest and drives its ring expansion through TickAndSubmitForTest
// each frame. The ripple uses no I3D asset (no `Magic\Ripple.I3D` or
// equivalent exists in the data tree — see INVENTORY H03 gap 7.2); the
// 4x4 atlas is built procedurally in src/effect.cpp::RippleAtlasTexture.
// The harness lambda is a thin shim — all spawn / animator / submit
// logic lives on the real TRippleEffect class. Mirrors F01 / B01 / S01.
//
// SpellGround preview style — water ripple is a ground-anchored event
// (drip from above lands; the ring expands at the impact point), so
// the cadence reads as "drip every few seconds, small XY jitter".
struct SRippleCtx {
    TRippleEffect* ripple = nullptr;
};

void* RippleSpawn(const S3DPoint& origin)
{
    auto* c = new SRippleCtx();
    c->ripple = TRippleEffect::SpawnForTest(origin);
    if (!c->ripple)
        log_warn("[vfx] TRippleEffect::SpawnForTest returned null; H03 entry will draw nothing");
    return c;
}

void RippleDestroy(void* cp)
{
    auto* c = static_cast<SRippleCtx*>(cp);
    delete c->ripple;
    delete c;
}

void RippleSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SRippleCtx*>(cp);
    if (!c->ripple)
        return;
    c->ripple->TickAndSubmitForTest(dbg);
}

// --- PE: real TMistEffect (Magic/mist.i3d, M05) --------------------------
// Spawns a sector-less TMistEffect at the harness-provided origin via
// SpawnForTest and drives its 50 long-lived ascending wisps through
// TickAndSubmitForTest each frame. Unlike B01 (one-shot 10-droplet
// burst), this is a *continuous emitter*: drops self-recycle in place
// on landing, so the bucket count stays steady-state at 50 from spawn
// to destroy. See INVENTORY M05 forensics §3 for the cadence and §6
// for the rig category.
//
// Static preview style — mist is ambient environmental, never re-fires;
// the harness destroying-and-respawning would defeat the test value
// (the per-drop steady-state needs a few seconds to develop). Cycle
// out and back in with Left/Right to restart.
struct SMistCtx {
    TMistEffect* mist = nullptr;
};

void* MistSpawn(const S3DPoint& origin)
{
    auto* c = new SMistCtx();
    c->mist = TMistEffect::SpawnForTest(origin);
    if (!c->mist)
        log_warn("[vfx] TMistEffect::SpawnForTest returned null; M05 entry will draw nothing");
    return c;
}

void MistDestroy(void* cp)
{
    auto* c = static_cast<SMistCtx*>(cp);
    delete c->mist;
    delete c;
}

void MistSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SMistCtx*>(cp);
    if (!c->mist)
        return;
    c->mist->TickAndSubmitForTest(dbg);
}

// --- FB+LS: real THaloEffect (procedural radial-gradient texture, L02) --
// Spawns a sector-less THaloEffect at the harness-provided origin via
// SpawnForTest and drives its triangle-wave scale envelope through
// TickAndSubmitForTest each frame. Halo submits one additive billboard
// (FB pipeline) AND re-adds one dynamic point light (LS pipeline) each
// frame — the row's "FB+LS" pipeline-tag in INVENTORY. Mirrors the
// F01/H03/M05 lambda-shim pattern; all spawn/animator/submit logic
// lives on THaloEffect.
//
// SpellGround preview style — the halo's triangle-wave + brief lifetime
// reads as a spell-cast ground halo (per INVENTORY L02 §5 inference):
// ground-anchored event, occasional re-fire at small XY jitter, 3.0 sec
// retrigger cadence (same as H03 ripple).
struct SHaloCtx {
    THaloEffect* halo = nullptr;
};

void* HaloSpawn(const S3DPoint& origin)
{
    auto* c = new SHaloCtx();
    c->halo = THaloEffect::SpawnForTest(origin);
    if (!c->halo)
        log_warn("[vfx] THaloEffect::SpawnForTest returned null; L02 entry will draw nothing");
    return c;
}

void HaloDestroy(void* cp)
{
    auto* c = static_cast<SHaloCtx*>(cp);
    delete c->halo;
    delete c;
}

void HaloSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SHaloCtx*>(cp);
    if (!c->halo)
        return;
    c->halo->TickAndSubmitForTest(dbg);
}

// --- FB: real TFireEffect (procedural orange/yellow scatter, F03) --------
// Spawns a sector-less TFireEffect at the harness-provided origin via
// SpawnForTest and ticks its 15-quad scatter scatter through
// TickAndSubmitForTest each frame. FB pipeline only (no particle bucket,
// no point light); per INVENTORY F03 forensics §6 the pre-release effect
// renders additive textured quads directly.
//
// SpellGround preview style — F03 has no live retail caller (forensics
// §4) so the cadence choice is a presentational decision; SpellGround
// 3 sec re-fire reads as "another patch of ambient fire pops up nearby",
// which matches the scatter-patch ambient-fire semantic and the
// spell-killed lifetime in pre-release Pulse.
struct SFireCtx {
    TFireEffect* fire = nullptr;
};

void* FireSpawn(const S3DPoint& origin)
{
    auto* c = new SFireCtx();
    c->fire = TFireEffect::SpawnForTest(origin);
    if (!c->fire)
        log_warn("[vfx] TFireEffect::SpawnForTest returned null; F03 entry will draw nothing");
    return c;
}

void FireDestroy(void* cp)
{
    auto* c = static_cast<SFireCtx*>(cp);
    delete c->fire;
    delete c;
}

void FireSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SFireCtx*>(cp);
    if (!c->fire)
        return;
    c->fire->TickAndSubmitForTest(dbg);
}

// --- LS: flare + dynamic point light placeholder -------------------------
struct SFlareCtx {
    float age = 0.0f;
    S3DPoint origin = {0, 0, 0};
};

void* FlareSpawn(const S3DPoint& origin)
{
    auto* c = new SFlareCtx();
    c->origin = origin;
    return c;
}
void  FlareDestroy(void* cp) { delete static_cast<SFlareCtx*>(cp); }

void FlareSubmit(void* cp, EFxDebugMode dbg)
{
    auto* c = static_cast<SFlareCtx*>(cp);
    c->age += float(TTime::DeltaTime());
    const float pulse = 0.7f + 0.3f * std::sin(c->age * 2.0f);

    // Submit a small additive billboard for the visible glow.
    const float wx = float(c->origin.x);
    const float wy = float(c->origin.y);
    const float wz = float(c->origin.z) + 80.0f;
    SBillboardDrawItem it = {};
    it.world_pos[0] = wx;
    it.world_pos[1] = wy;
    it.world_pos[2] = wz;
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
    Renderer->AddPointLight(wx, wy, wz, 320.0f,
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
        flame.id            = "TFlameEffect";
        flame.family        = "fire";
        flame.pipeline      = "FB";
        flame.preview_style = VfxTest::EVfxPreviewStyle::Static;
        flame.factory       = [](const S3DPoint& o) -> void* { return FlameSpawn(o); };
        flame.submit        = [](void* c, EFxDebugMode d) { FlameSubmit(c, d); };
        flame.destroy       = [](void* c) { FlameDestroy(c); };
        VfxTest::DeferredRegister(flame);

        VfxTest::SEffect blood = {};
        blood.id            = "TBloodEffect";
        blood.family        = "blood";
        blood.pipeline      = "PE";
        blood.preview_style = VfxTest::EVfxPreviewStyle::Combat;
        blood.factory       = [](const S3DPoint& o) -> void* { return BloodSpawn(o); };
        blood.submit        = [](void* c, EFxDebugMode d) { BloodSubmit(c, d); };
        blood.destroy       = [](void* c) { BloodDestroy(c); };
        VfxTest::DeferredRegister(blood);

        VfxTest::SEffect strip = {};
        strip.id            = "TStripEffect";
        strip.family        = "strip";
        strip.pipeline      = "SR";
        // Lightning bolt is combat-cadence: re-fires across the screen
        // like successive casts (the in-game cadence is roughly one
        // bolt per cast-cycle; the harness re-fire every ~0.6s reads
        // as a flurry of casts, which is fine for the SR-pipeline
        // validator visual.)
        strip.preview_style = VfxTest::EVfxPreviewStyle::Combat;
        strip.factory       = [](const S3DPoint& o) -> void* { return StripSpawn(o); };
        strip.submit        = [](void* c, EFxDebugMode d) { StripSubmit(c, d); };
        strip.destroy       = [](void* c) { StripDestroy(c); };
        VfxTest::DeferredRegister(strip);

        VfxTest::SEffect ripple = {};
        ripple.id            = "TRippleEffect";
        ripple.family        = "water";
        ripple.pipeline      = "FB";
        // Ripple = drip-into-water event: ground-level, occasional
        // re-fire at small XY jitter. Matches SpellGround cadence.
        ripple.preview_style = VfxTest::EVfxPreviewStyle::SpellGround;
        ripple.factory       = [](const S3DPoint& o) -> void* { return RippleSpawn(o); };
        ripple.submit        = [](void* c, EFxDebugMode d) { RippleSubmit(c, d); };
        ripple.destroy       = [](void* c) { RippleDestroy(c); };
        VfxTest::DeferredRegister(ripple);

        VfxTest::SEffect mist = {};
        mist.id            = "TMistEffect";
        mist.family        = "ambient";
        mist.pipeline      = "PE";
        // Mist is a continuous ambient emitter — Static cadence (never
        // re-fires; cycle out/in to restart). See INVENTORY M05 §6.
        mist.preview_style = VfxTest::EVfxPreviewStyle::Static;
        mist.factory       = [](const S3DPoint& o) -> void* { return MistSpawn(o); };
        mist.submit        = [](void* c, EFxDebugMode d) { MistSubmit(c, d); };
        mist.destroy       = [](void* c) { MistDestroy(c); };
        VfxTest::DeferredRegister(mist);

        VfxTest::SEffect halo = {};
        halo.id            = "THaloEffect";
        halo.family        = "light";
        halo.pipeline      = "FB+LS";
        // Halo = spell-cast ground halo (per INVENTORY L02 §5): brief
        // triangle-wave pulse, ground-anchored, ~1 sec lifetime.
        // SpellGround cadence (3 sec retrigger, small XY jitter) reads
        // as repeated spell casts at the player's feet.
        halo.preview_style = VfxTest::EVfxPreviewStyle::SpellGround;
        halo.factory       = [](const S3DPoint& o) -> void* { return HaloSpawn(o); };
        halo.submit        = [](void* c, EFxDebugMode d) { HaloSubmit(c, d); };
        halo.destroy       = [](void* c) { HaloDestroy(c); };
        VfxTest::DeferredRegister(halo);

        VfxTest::SEffect fire = {};
        fire.id            = "TFireEffect";
        fire.family        = "fire";
        fire.pipeline      = "FB";
        // F03 = ambient scatter-patch fire (per INVENTORY F03 §6).
        // SpellGround re-fire cadence reads as "occasional patch flares
        // up nearby" — matches the spell-killed pre-release lifetime
        // (forensics §4) and the scatter-patch ambient semantic.
        fire.preview_style = VfxTest::EVfxPreviewStyle::SpellGround;
        fire.factory       = [](const S3DPoint& o) -> void* { return FireSpawn(o); };
        fire.submit        = [](void* c, EFxDebugMode d) { FireSubmit(c, d); };
        fire.destroy       = [](void* c) { FireDestroy(c); };
        VfxTest::DeferredRegister(fire);

        VfxTest::SEffect flare = {};
        flare.id            = "TFlareAnimator.placeholder";
        flare.family        = "light";
        flare.pipeline      = "LS";
        // Flare = spell-cast burst: ground-level, occasional re-fire.
        flare.preview_style = VfxTest::EVfxPreviewStyle::SpellGround;
        flare.factory       = [](const S3DPoint& o) -> void* { return FlareSpawn(o); };
        flare.submit        = [](void* c, EFxDebugMode d) { FlareSubmit(c, d); };
        flare.destroy       = [](void* c) { FlareDestroy(c); };
        VfxTest::DeferredRegister(flare);
    }
};
SVfxTestBootstrap g_vfx_test_bootstrap;
}  // namespace
