// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  fireswarm.cpp - TFireSwarmAnimator port via the d3d::* shim          *
// *                                                                       *
// *  SHIM VALIDATION PORT (F05 TFireSwarmEffect) — wave-2 shim port.     *
// *  Forensics source: docs/vfx/forensics/F05_TFireSwarmEffect.md         *
// *  Snapshot source:  src/effect_old.cpp:10453-10562                     *
// *  Retail decomp:    recon/classes/cls_0x5abe4c.cpp                     *
// *                                                                       *
// *  =====================================================================*
// *  Iteration history                                                    *
// *  =====================================================================*
// *  iter1 (snapshot-literal):                                            *
// *    Drew tube01 only per snapshot effect_old.cpp:10531 GetObject(1).   *
// *    User: "effect looks incomplete, only an expanding ring."           *
// *                                                                       *
// *  iter2 (asset-full):                                                  *
// *    Added box01 central glow disc alongside tube01 ring.               *
// *    User: "still incomplete — particle effects missing?"               *
// *                                                                       *
// *  iter3-5 (speculative 4×50 particle field):                          *
// *    Added approximated 4-system particle field per recon hints at     *
// *    cls_0x5abe4c.cpp (4 ptr arrays mbr_0x160[0..3], 50-entry tables, *
// *    100-frame counter). Tried particle textures (iter3), single-glow *
// *    + tints (iter4), smaller swarm (iter5). User reverted with:       *
// *    "I confirmed that the 'original' version of FireSwarm was right.  *
// *     The game version just is showing the circular effect. I'd leave  *
// *     it as is until we can determine if it's used in game."           *
// *                                                                       *
// *  iter6 (back to iter2 — CURRENT):                                    *
// *    Reverted to cylinder ring + central glow disc only. The 4         *
// *    unextracted method bodies on cls_0x5abe4c may or may not          *
// *    implement particles; the in-game observable visual is the         *
// *    cylinder + glow as in iter2. Left as "complete pending Ghidra     *
// *    extraction" — see "Future investigation" block below.             *
// *  =====================================================================*
// *                                                                       *
// *  Retail fidelity: retail-partial.                                     *
// *    - Registration string "FireSwarm" confirmed in retail binary at    *
// *      .rdata 005e11b8 (2 XREFs at 004f00d0 + 004f0110 — forensics     *
// *      §2.1.3, _data.txt:107531).                                       *
// *    - Asset Magic\FireSwarm.i3d byte-identical (24,300 B,              *
// *      MD5 927caed9595abffae3ea7a939b3a240a — forensics §2.1.2).       *
// *    - Cylinder + glow visual user-confirmed matches the in-game       *
// *      effect (round-6 verify, iter2 form). The 4 unextracted method   *
// *      bodies on cls_0x5abe4c may implement other behavior that         *
// *      doesn't manifest in the observable visual (e.g. dead-code,      *
// *      gameplay coupling without visual output, or simply different    *
// *      semantics from what the structure hinted at).                    *
// *    - Lifetime = 100 ticks = 4.166 s, matching retail kill counter   *
// *      at cls_0x5abe4c.cpp:109 (`if (100 < this->mbr_0xfc)`).          *
// *      Snapshot's FIRESWARM_DURATION=75 was the WIP value; retail's    *
// *      100 is the shipped value (still retail-confirmed even with      *
// *      iter6 revert, since this is just a counter threshold).          *
// *                                                                       *
// *  Architecture                                                         *
// *  ============                                                         *
// *    Two WorldMesh sub-objects of Magic\FireSwarm.i3d:                  *
// *      GetObject(0) = "box01"  — flat quad at z=0, soft red/pink       *
// *                                glow disc texture (texture[0]).        *
// *      GetObject(1) = "tube01" — 64-vert cylinder z=0..20, flame-      *
// *                                tongue texture (texture[1]).           *
// *                                                                       *
// *    Per-frame transform (applied to BOTH sub-objects):                 *
// *      Scale(cylhscl, cylhscl, cylvscl) · RotZ(cylth). No translate;    *
// *      mesh sits at effect world pos via inst_world.                    *
// *      box01's flat z=0 geometry means cylvscl has no effect on it     *
// *      (z*scale=0); only cylhscl matters → disc expands radially in    *
// *      lockstep with the tube radius.                                   *
// *                                                                       *
// *    3-float accumulator (cylhscl, cylvscl, cylth) + lifetime.         *
// *    Framerate-independent via delta-time integration.                  *
// *    WorldMesh draw MUST go through submit_world callback (Gotcha #4). *
// *    One-shot: kills at lifetime > kRetailDurationSec.                  *
// *                                                                       *
// *  Lighting category (WAVE_1_LESSONS §1.2a): Self-lit.                 *
// *    ARGB4444 real-alpha texture; Alpha blend respects authored alpha. *
// *    Emissive zeroed after LoadMaterial (§1.1 — white-saturation fix). *
// *                                                                       *
// *  Blend = Alpha (SetBlendState — forensics §7).                       *
// *  Depth = TestNoWrite (forensics §7).                                 *
// *  Cull  = engine default (forensics §7).                              *
// *                                                                       *
// *  =====================================================================*
// *  Future investigation (NOT iter6's scope)                            *
// *  =====================================================================*
// *  The retail class cls_0x5abe4c has 5 unextracted vtable bodies:     *
// *      virt_meth_0x4efbd0 (vftable +24)  — likely Initialize           *
// *      virt_meth_0x4efa90 (vftable +100) — likely per-system step      *
// *      virt_meth_0x4efe50 (vftable +52)  — possibly Render             *
// *      virt_meth_0x4f0070 (vftable +60)  — unknown                     *
// *      virt_meth_0x4efcc0 (vftable +96)  — particle allocator          *
// *  Plus 4 per-system decay rates at field_0x100[0..3] and 4 pointer   *
// *  arrays at mbr_0x160[0..3]. The hints suggest the class is          *
// *  structured as a particle emitter, but a future Ghidra pass should  *
// *  extract the method bodies before adding particle code — three      *
// *  iterations of speculation produced wrong visuals that the user     *
// *  rejected. The observable visual is the cylinder + glow per iter2.  *
// *  =====================================================================*
// *************************************************************************

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "../d3dport.h"
#include "../logging.h"
#include "../math3d.h"
#include "../imagery.h"
#include "../renderer.h"
#include "../time.h"

extern TRenderer* Renderer;

namespace fireswarm_shim {

// ==========================================================================
// Constants — cylinder + central glow disc only (iter6 form).
// ==========================================================================

// Total lifetime — retail-confirmed at cls_0x5abe4c.cpp:109
// (`if (100 < this->mbr_0xfc)`). Snapshot's FIRESWARM_DURATION=75 was the
// pre-release WIP value. We honour the retail value 100 even though the
// particle systems that 100-frame counter probably gated are not ported
// here (iter6 revert per user direction).
constexpr float kRetailDurationSec = 100.0f / 24.0f;   // 4.166 s — retail-confirmed

// Cylinder/glow mesh transform animation rates (snapshot-only, forensics §3).
constexpr float kCylHSclStep  = 0.4f;                  // effect_old.cpp:10470
constexpr float kCylHSclRate  = kCylHSclStep * 24.0f;  // 9.6 scale-units/sec
constexpr float kCylThStep    = 0.5f;                  // effect_old.cpp:10471
constexpr float kCylThRate    = kCylThStep * 24.0f;    // 12.0 rad/sec
constexpr float kCylVSclInit  = 30.0f;                 // effect_old.cpp:10472
constexpr float kCylVSclStep  = 0.4f;                  // effect_old.cpp:10473
constexpr float kCylVSclRate  = kCylVSclStep * 24.0f;  // 9.6 scale-units/sec (decrement)
constexpr float kCylHSclInit  = kCylHSclStep;          // effect_old.cpp:10488
constexpr float kCylThInit    = 0.0f;                  // effect_old.cpp:10487

// 2π wrap constant for yaw accumulator (src/revdefs.h:24 — M_2PI).
constexpr float kM2Pi = 2.0f * 3.14159265358979323846f;

// Sub-object indices (forensics §4 + i3d_dump_all/fireswarm/manifest.txt):
//   0 = box01  — flat quad (12 verts) at z=0,
//                bbox=(-5.13,-4.99,0)..(5.13,4.99,0).
//                Texture[0] = soft red/pink glow disc (radial gradient).
//   1 = tube01 — cylinder (174 verts), z=0..20,
//                bbox=(-5.38,-5.38,0)..(5.38,5.38,20).
//                Texture[1] = orange flame tongues.
//
// User foreground verify after iter1 (tube01-only): "incomplete, only
// expanding ring". User verify after iter2 (both): final confirmed in-game
// visual. iter3-5 tried adding particles to match a hypothesized retail
// visual; user reverted with "the game version just is showing the
// circular effect" — so the cylinder + glow IS the shipped visual.
constexpr int32_t kSubObjBox  = 0;   // box01 — central glow disc
constexpr int32_t kSubObjTube = 1;   // tube01 — flame cylinder
constexpr int32_t kNumSubObjs = 2;

// Imagery path (Class.Def:2062, retail class.def in data/imagery.rvi).
constexpr const char* kImageryPath = "Magic\\FireSwarm.i3d";

// 24 Hz sim rate — framerate-independent base (PORT_CHECKLIST §B.4).
constexpr float kSimHz     = 24.0f;
constexpr float kSimTickMs = 1000.0f / kSimHz;

// ==========================================================================
// Per-effect state.
// ==========================================================================

struct State
{
    // Animator scalars (snapshot effect.h:2014-2018; forensics §6.3).
    float    cylhscl     = kCylHSclInit;    // XY radius scale
    float    cylvscl     = kCylVSclInit;    // Z height scale
    float    cylth       = kCylThInit;      // yaw angle (radians)

    // Lifetime accumulator. Kill at lifetime_s > kRetailDurationSec
    // (= 100/24 = 4.166 s, retail-confirmed cls_0x5abe4c.cpp:109).
    float    lifetime_s  = 0.0f;

    // Framerate-independent accumulator (PORT_CHECKLIST §B.4).
    double   sim_accum_ms = 0.0;
    int32_t  sim_tick     = 0;

    // World anchor.
    float    base_pos[3] = {0.0f, 0.0f, 0.0f};

    // Lifecycle.
    bool     alive       = true;

    // --- Asset state ---
    T3DImagery* imagery        = nullptr;

    // Pre-registered mesh handles for both WorldMesh draws.
    MeshHandle mesh[kNumSubObjs] = {0, 0};

    // Per-sub-object material colors (loaded from asset materials).
    float    diffuse[kNumSubObjs][4]  = {{1,1,1,1}, {1,1,1,1}};
    float    emissive[kNumSubObjs][4] = {{0,0,0,1}, {0,0,0,1}};

    // Diagnostic flags.
    bool     logged_first_render = false;
    bool     logged_end          = false;
};

// ==========================================================================
// Initialize — transcribed from snapshot effect_old.cpp:10483-10490.
//
// Initialize():
//     T3DAnimator::Initialize()              // :10485
//     frameon = 0                            // :10486 (→ lifetime_s = 0)
//     cylth   = 0.0                          // :10487
//     cylhscl = FIRESWARM_CYLHSCLSTEP        // :10488  (= 0.4)
//     cylvscl = FIRESWARM_CYLVSCLINIT        // :10489  (= 30.0)
// ==========================================================================
static void Initialize(State* st)
{
    st->cylhscl      = kCylHSclInit;    // effect_old.cpp:10488
    st->cylvscl      = kCylVSclInit;    // effect_old.cpp:10489
    st->cylth        = kCylThInit;      // effect_old.cpp:10487
    st->lifetime_s   = 0.0f;           // replaces frameon=0 (:10486)
    st->sim_tick     = 0;
    st->sim_accum_ms = 0.0;
    st->alive        = true;
}

// ==========================================================================
// AnimateOneSimTick — one sim tick at 24 Hz. Cylinder + glow only.
// Transcribed from snapshot effect_old.cpp:10499-10515, with retail-
// confirmed kill threshold (100 ticks, not snapshot's 75).
// ==========================================================================
static void AnimateOneSimTick(State* st, float dt)
{
    if (!st->alive) return;

    st->lifetime_s += dt;
    st->sim_tick   += 1;

    // Yaw advance: 12.0 rad/sec (effect_old.cpp:10506).
    st->cylth += kCylThRate * dt;
    if (st->cylth > kM2Pi)
        st->cylth -= kM2Pi;

    // XY radius grows, Z height shrinks (effect_old.cpp:10509-10510).
    st->cylhscl += kCylHSclRate * dt;
    st->cylvscl -= kCylVSclRate * dt;
    // Clamp Z scale at 0 so the cylinder doesn't invert in the final frames.
    if (st->cylvscl < 0.0f) st->cylvscl = 0.0f;

    // Kill at retail-confirmed lifetime > 100 ticks = 4.166 s
    // (cls_0x5abe4c.cpp:109; snapshot's 75 was the WIP value).
    if (st->lifetime_s > kRetailDurationSec)
        st->alive = false;
}

// ==========================================================================
// Tick — framerate-independent 24 Hz gate (PORT_CHECKLIST §B.4).
// ==========================================================================
void Tick(State* st)
{
    if (!st) return;

    st->sim_accum_ms += TTime::DeltaTime() * 1000.0;
    while (st->sim_accum_ms >= double(kSimTickMs))
    {
        st->sim_accum_ms -= double(kSimTickMs);
        AnimateOneSimTick(st, 1.0f / kSimHz);
        if (!st->alive) break;
    }

    if (!st->alive && !st->logged_end)
    {
        log_info("[fireswarm-shim] effect dead at lifetime_s=%.3f sim_tick=%d",
                 double(st->lifetime_s), st->sim_tick);
        st->logged_end = true;
    }
}

// ==========================================================================
// Render — transcribed from snapshot effect_old.cpp:10524-10549. iter2 form:
// draws BOTH box01 (sub-obj 0) and tube01 (sub-obj 1) with the shared
// per-frame transform. MUST be called from submit_world (Gotcha #4).
//
// Snapshot reference (applied to BOTH sub-objects per iter2 forensics §13.5
// resolution from user feedback):
//     SaveBlendState()                    // :10526 → BlendStateGuard
//     SetBlendState()                     // :10527 → Alpha
//     for sub_obj in {0=box01, 1=tube01}:
//         obj = GetObject(sub_obj)        // :10531 (iter2 expansion)
//         obj->flags = OBJ3D_MATRIX       // :10532
//         D3DMATRIXClear(&obj->matrix)    // :10533 → MatrixIdentity
//         obj->scl = (cylhscl, cylhscl, cylvscl) // :10535-10537
//         D3DMATRIXScale(&obj->matrix, &scl)    // :10538
//         D3DMATRIXRotateZ(&obj->matrix, cylth) // :10540
//         RenderObject(obj)               // :10543
//     RestoreBlendState()                 // :10546 → BlendStateGuard dtor
//
// box01's flat z=0 geometry means cylvscl has no effect on it (z*scale=0);
// only cylhscl matters → disc expands radially in lockstep with the tube
// ring's outer radius.
// ==========================================================================
static void Render(State* st)
{
    if (!Renderer || !st->alive) return;

    // SaveBlendState + RestoreBlendState via RAII guard.
    d3d::BlendStateGuard blend_scope;

    // SetBlendState() — Alpha. Matches snapshot effect_old.cpp:10527 + the
    // ARGB4444 real-alpha asset format (forensics §7 BLEND SANITY-CHECK).
    d3d::SetBlendState();
    const d3d::BlendMode blend = d3d::CurrentBlend();

    // Build inst_world: translate to effect's world position.
    d3d::Matrix inst_world;
    d3d::MatrixIdentity(inst_world);
    d3d::MatrixTranslate(inst_world,
                         st->base_pos[0], st->base_pos[1], st->base_pos[2]);

    // Build the per-frame local transform (shared across both sub-objects):
    //   D3DMATRIXClear → D3DMATRIXScale(cylhscl, cylhscl, cylvscl) → D3DMATRIXRotateZ(cylth)
    //   effect_old.cpp:10533, 10538, 10540.
    d3d::Matrix local;
    d3d::MatrixIdentity(local);
    d3d::MatrixScale(local, st->cylhscl, st->cylhscl, st->cylvscl);
    d3d::MatrixRotateZ(local, st->cylth);

    const float sort_z = st->base_pos[2];

    // Draw box01 (central glow disc) first so any front-to-back ordering
    // tiebreak in the transparent queue keeps it behind the brighter ring.
    if (st->mesh[kSubObjBox] != 0)
    {
        d3d::RenderObject(*Renderer,
                          st->mesh[kSubObjBox],
                          local,
                          st->diffuse[kSubObjBox],
                          st->emissive[kSubObjBox],
                          blend,
                          &inst_world,
                          sort_z);
    }

    // Draw tube01 (flame cylinder ring).
    if (st->mesh[kSubObjTube] != 0)
    {
        d3d::RenderObject(*Renderer,
                          st->mesh[kSubObjTube],
                          local,
                          st->diffuse[kSubObjTube],
                          st->emissive[kSubObjTube],
                          blend,
                          &inst_world,
                          sort_z);
    }

    if (!st->logged_first_render)
    {
        log_info("[fireswarm-shim] first render: lifetime_s=%.3f sim_tick=%d "
                 "cylhscl=%.2f cylvscl=%.2f cylth=%.3f "
                 "box_mesh=%u tube_mesh=%u "
                 "box.diffuse=(%.2f,%.2f,%.2f,%.2f) "
                 "tube.diffuse=(%.2f,%.2f,%.2f,%.2f)",
                 double(st->lifetime_s), st->sim_tick,
                 double(st->cylhscl), double(st->cylvscl), double(st->cylth),
                 st->mesh[kSubObjBox], st->mesh[kSubObjTube],
                 double(st->diffuse[kSubObjBox][0]), double(st->diffuse[kSubObjBox][1]),
                 double(st->diffuse[kSubObjBox][2]), double(st->diffuse[kSubObjBox][3]),
                 double(st->diffuse[kSubObjTube][0]), double(st->diffuse[kSubObjTube][1]),
                 double(st->diffuse[kSubObjTube][2]), double(st->diffuse[kSubObjTube][3]));
        st->logged_first_render = true;
    }
}

// ==========================================================================
// Public harness entry points.
// ==========================================================================

State* Spawn(const S3DPoint& origin)
{
    if (!Renderer) return nullptr;

    State* st = new State();
    st->base_pos[0] = float(origin.x);
    st->base_pos[1] = float(origin.y);
    st->base_pos[2] = float(origin.z);

    // Load Magic\FireSwarm.i3d.
    int32_t img_id = TObjectImagery::FindImagery(kImageryPath);
    if (img_id < 0)
    {
        std::string p_str = kImageryPath;
        img_id = TObjectImagery::RegisterImagery(p_str.data());
    }
    if (img_id < 0)
    {
        log_error("[fireswarm-shim] could not resolve '%s'", kImageryPath);
        delete st;
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[fireswarm-shim] '%s' is not T3DImagery", kImageryPath);
        delete st;
        return nullptr;
    }
    st->imagery = img3d;

    const int32_t num_obj = img3d->NumObjects();
    const int32_t num_tex = img3d->NumTextures();

    if (num_obj < kNumSubObjs)
    {
        log_error("[fireswarm-shim] '%s' has only %d sub-objects; expected >=%d "
                  "(box01 + tube01)", kImageryPath, num_obj, kNumSubObjs);
        delete st;
        return nullptr;
    }

    // Register BOTH sub-objects (mesh handles + material colors).
    for (int32_t i = 0; i < kNumSubObjs; ++i)
    {
        st->mesh[i] = d3d::RegisterSubMesh(*Renderer, img3d, i, num_tex);
        if (st->mesh[i] == 0)
        {
            log_warn("[fireswarm-shim] sub-obj %d mesh registration failed — "
                     "that piece will not render", i);
        }

        d3d::LoadMaterial(img3d, i, st->diffuse[i], st->emissive[i]);

        // Zero emissive (WAVE_1_LESSONS §1.1 / PORT_CHECKLIST §B.2):
        // I3D D3DMATERIAL.Emissive defaults to (1,1,1) which the helper-mesh
        // shader adds on top of texture, saturating fragments to white. Zero
        // it so the texture colour passes through cleanly under Alpha blend.
        st->emissive[i][0] = 0.0f;
        st->emissive[i][1] = 0.0f;
        st->emissive[i][2] = 0.0f;
        // emissive[i][3] stays 1.0f (alpha channel — not in emissive sum)
    }

    Initialize(st);

    log_info("[fireswarm-shim] Spawn ok: at (%d,%d,%d) num_obj=%d num_tex=%d "
             "box_mesh=%u tube_mesh=%u "
             "cylhscl=%.2f cylvscl=%.2f cylth=%.3f "
             "lifetime_target_s=%.3f",
             origin.x, origin.y, origin.z,
             num_obj, num_tex,
             st->mesh[kSubObjBox], st->mesh[kSubObjTube],
             double(st->cylhscl), double(st->cylvscl), double(st->cylth),
             double(kRetailDurationSec));
    return st;
}

// Submit — no-op stub. All draws are WorldMesh and route through SubmitWorld.
void Submit(State* /*st*/) {}

// SubmitWorld — WorldMesh draws MUST go through submit_world (Gotcha #4).
void SubmitWorld(State* st)
{
    if (!st) return;
    Render(st);
}

// IsAlive — auto-respawn helper (PORT_CHECKLIST §B.7).
bool IsAlive(const State* st)
{
    return st && st->alive;
}

void Destroy(State* st) { delete st; }

} // namespace fireswarm_shim
