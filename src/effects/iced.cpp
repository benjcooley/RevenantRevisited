// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  iced.cpp - TIcedAnimator port via the d3d::* shim                   *
// *                                                                       *
// *  SHIM VALIDATION PORT (I22 TIcedEffect) — fourth shim port.          *
// *  Forensics source: docs/vfx/forensics/I22_TIcedEffect.md             *
// *  Snapshot source:  src/effect_old.cpp:8716-8985                      *
// *                    src/effect.h:1574-1600 (TIcedAnimator declaration) *
// *                                                                       *
// *  Retail fidelity: retail-partial. Lifecycle core (ICED_DURATION=240, *
// *  per-frame Stop(), OF_ICED/OF_PARALIZE early-exit + clear at shatter) *
// *  confirmed via cls_0x5ab460::virt_meth_0x4ec890. Asset iced.I3D is   *
// *  byte-identical (MD5 10e7e15b3cd6b491e47ab1743b5cdf8e). Shatter-     *
// *  physics tuning constants (gravity, bounce, scatter, scale/life RNG, *
// *  chunk count) are snapshot-only — see forensics §2.1 and §13.8.      *
// *                                                                       *
// *  Architecture notes:                                                  *
// *  - Crystal hold phase: sub-objects 1..min(frameon*4,50) from         *
// *    iced.I3D, all with shared RotateZ(angle) → WorldMesh draws.       *
// *  - Shatter/tumble phase: sub-object 0 (cube) re-instanced 30× with  *
// *    per-chunk Scale·RotX·RotY·RotZ·Translate → WorldMesh draws.       *
// *  - Both phases must be submitted from submit_world (Gotcha #4).      *
// *  - No icedchar coupling in test harness (anchor at world origin).    *
// *  - No audio, no dynamic light, no texture animation (forensics §9-11)*
// *                                                                       *
// *  Blend = Alpha (SetBlendState — forensics §7).                       *
// *  Lit-mode = Unlit (no material zeroing, no per-vertex color — asset  *
// *  color passes through under MODULATE — forensics §7).                *
// *  Depth = TestNoWrite (SetBlendState — forensics §7).                 *
// *************************************************************************

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "../d3dport.h"
#include "../logging.h"
#include "../math3d.h"
#include "../imagery.h"
#include "../renderer.h"
#include "../time.h"

extern TRenderer* Renderer;

namespace iced_shim {

// ==========================================================================
// Constants — every value cites forensics §3 + snapshot line.
// "wu" = world units. random(a,b) = inclusive on both ends (revutils.cpp).
// ==========================================================================

// Hold duration (forensics §3; retail-confirmed via cls_0x5ab460.cpp:48).
// snapshot effect_old.cpp:8720 — ICED_DURATION = 24 * ICED_LENGTH
constexpr int32_t kIcedLength   = 10;              // ICED_LENGTH (effect_old.cpp:8719)
constexpr int32_t kIcedDuration = 24 * kIcedLength; // ICED_DURATION = 240 (effect_old.cpp:8720)

// Gravity for chunk tumble (snapshot-only — forensics §2.1).
// effect_old.cpp:8721 — ICED_CHUNK_GRAVITY = 0.25 wu/frame²
constexpr float kIcedChunkGravity = 0.25f;

// Max shatter chunks (snapshot-only — forensics §3, effect.h:1573).
constexpr int32_t kMaxIcedChunks = 30;

// Crystal reveal cap: sub-object loop bound is min(frameon*4, 50).
// effect_old.cpp:8909 — loop: for (i=1; i < min(frameon*4, 50); i++)
// Forensics §4: sub-objects 1..43 = face01..face43, 44..49 = icicle01..06
constexpr int32_t kRevealCap = 50;

// Bounce physics (snapshot-only — forensics §3, §6.3, §13.6).
// effect_old.cpp:8876 — "fix this" — flat local-z floor, not world height
constexpr float kBouncePlane   = 16.0f;  // p.z <= 16 → bounce (effect_old.cpp:8876)
constexpr float kBounceLift    = 20.0f;  // p.z = 20 after bounce (effect_old.cpp:8878)
constexpr float kBounceVelMul  = -0.5f;  // v.z *= -0.5 (effect_old.cpp:8879)
constexpr float kBounceSpinMul = 2.0f;   // w *= 2 (effect_old.cpp:8880-8882)

// 24 Hz sim tick gate (per framerate-independent convention; port protocol §6).
// One "frame" in the snapshot = one sim tick at 24 Hz.
constexpr int32_t kSimTickMs = 1000 / 24;  // ≈41.6ms per tick

// Imagery path (forensics §4 — "Iced" registry name at Class.Def:2056).
constexpr const char* kImageryPath = "Magic\\iced.I3D";

// Sub-object indices (forensics §4 — file-order verified via string dump).
//   0  = cube           (the single shatter-chunk mesh)
//   1..43 = face01..face43  (crystal shell facets)
//   44..49 = icicle01..06   (icicle sub-objects)
constexpr int32_t kSubObjCube     = 0;   // the chunk mesh
constexpr int32_t kSubObjFaceBase = 1;   // first crystal facet
// kRevealCap = 50 is the exclusive upper bound for the hold-phase loop

// Total sub-objects to register: cube(0) + faces(1..43) + icicles(44..49) = 50
// We register indices 0..49 (kRevealCap items 0..(kRevealCap-1)).
constexpr int32_t kTotalSubObjs = 50;

// M_2PI — angular wrap for chunk spin (revdefs.h:24)
constexpr float kM2Pi = 2.0f * 3.14159265358979323846f;

// ==========================================================================
// Inclusive-range random — matches snapshot random(lo, hi) semantics.
// (src/revutils.cpp:1597-1613: rand() % (max-min+1) + min)
// ==========================================================================
static int32_t snap_random(int32_t lo, int32_t hi)
{
    if (hi <= lo) return lo;
    return lo + (std::rand() % (hi - lo + 1));
}

// ==========================================================================
// Per-chunk state — flattened from the snapshot's p/v/t/w/s/l arrays.
// (src/effect.h:1718-1720 — hmm_vec3 p/v/t/w/s [MAX_ICED_CHUNKS], l[30])
// ==========================================================================
struct Chunk
{
    float pos[3] = {0,0,0};   // p[i] — current position (effect local space)
    float vel[3] = {0,0,0};   // v[i] — velocity wu/frame
    float rot[3] = {0,0,0};   // t[i] — angular position (radians)
    float ang[3] = {0,0,0};   // w[i] — angular velocity (radians/frame)
    float scl[3] = {0,0,0};   // s[i] — per-axis scale
    int32_t life = 0;          // l[i] — bounces remaining; 0 = dead
};

// ==========================================================================
// Per-effect state.
// ==========================================================================
struct State
{
    // Core animator state (effect.h:1715-1721)
    int32_t  frameon       = 0;     // per-sim-tick counter (effect_old.cpp:8752)
    float    angle         = 0.0f;  // fixed Z-spin for crystal, set at Initialize
    int32_t  donebouncing  = 0;     // all chunks dead → KillThisEffect

    // Victim coupling: not wired in test harness (no MapPane/character).
    // In production, icedchar would be stored here and polled each tick.
    // The early-exit branches (IsIced, IsDead) are guarded out here.

    // Chunk working set (snapshot: TIcedAnimator member arrays)
    Chunk    chunks[kMaxIcedChunks];

    // World position of the effect (anchor = victim pos in production;
    // in test harness = world origin or fixed test position).
    float base_pos[3] = {0,0,0};

    // Framerate-independence accumulator (24 Hz gate)
    double sim_accum_ms = 0.0;

    // Lifecycle
    bool alive = true;

    // Asset state — loaded at Spawn, used throughout lifetime.
    T3DImagery* imagery = nullptr;  // cached to defer per-sub-obj tex lookup

    // Pre-registered mesh handles for WorldMesh draws.
    // Index matches sub-object index: handles[0]=cube, handles[1..49]=faces+icicles.
    MeshHandle handles[kTotalSubObjs] = {};

    // Per-sub-obj material (diffuse/emissive) for unlit render.
    float diffuse[kTotalSubObjs][4]  = {};
    float emissive[kTotalSubObjs][4] = {};

    // Diagnostic
    int32_t spawn_log_done = 0;
};

// ==========================================================================
// Initialize — transcribed from snapshot effect_old.cpp:8749-8756.
//
// Initialize():                         // :8749-8756
//     T3DAnimator::Initialize()
//     frameon = 0
//     angle = random(0,359) * TORADIAN  // one fixed Z-spin for the crystal
//     icedchar = null
//     donebouncing = 0
// ==========================================================================
static void Initialize(State* st)
{
    st->frameon      = 0;                                           // :8752
    st->angle        = float(snap_random(0, 359))
                       * (3.14159265358979323846f / 180.0f);       // :8753 — TORADIAN
    st->donebouncing = 0;                                           // :8755
    // icedchar = null; no victim in test harness
}

// ==========================================================================
// SeedChunks — Phase 2 shatter init, transcribed from snapshot
// effect_old.cpp:8806-8847. Runs once when frameon == kIcedDuration.
//
// Spawns MAX_ICED_CHUNKS=30 chunks in a 4-quadrant a/b cycle.
// Forensics §6.2 has the full pseudocode.
// ==========================================================================
static void SeedChunks(State* st)
{
    int32_t a = 0, b = 0;
    for (int32_t i = 0; i < kMaxIcedChunks; ++i)
    {
        const int32_t c = (a ? 1 : -1);           // :8810 — quadrant sign X
        const int32_t d = (b ? 1 : -1);           // :8811 — quadrant sign Y

        Chunk& ch = st->chunks[i];

        // Velocity (:8813-8815)
        ch.vel[0] = float(c * snap_random(5, 10)) / 10.0f
                  + float(snap_random(-5, 5))      / 10.0f;
        ch.vel[1] = float(d * snap_random(5, 10)) / 10.0f
                  + float(snap_random(-5, 5))      / 10.0f;
        ch.vel[2] = float(snap_random(0, 10))     / 10.0f;  // upward [0..1]

        // Position (:8817-8819)
        ch.pos[0] = float(c * snap_random(20, 40) + snap_random(-20, 20));
        ch.pos[1] = float(d * snap_random(20, 40) + snap_random(-20, 20));
        ch.pos[2] = float((i / 6) * 25);  // rows of 6, 25 wu apart (comment says 4 but math=6; §13.5)

        // Angular velocity (:8821)
        // w.z stays 0 (commented out in snapshot :8822 — §13.3 do-not-reconstruct)
        ch.ang[0] = ch.ang[1] = float(snap_random(0, 25)) / 100.0f;
        ch.ang[2] = 0.0f;

        // Angular position (:8824-8826)
        // t.x/y stay 0 (commented out in snapshot :8824-8825 — §13.3)
        ch.rot[0] = 0.0f;
        ch.rot[1] = 0.0f;
        ch.rot[2] = std::atan2(ch.vel[1], ch.vel[0]);  // :8826 — face travel direction

        // Scale (:8828-8839) — 50% large chunks, 50% small shards
        if (snap_random(0, 1))
        {
            ch.scl[0] = ch.scl[1] = ch.scl[2] =
                float(snap_random(25, 75)) / 100.0f;   // large: [0.25..0.75]
        }
        else
        {
            ch.scl[0] = ch.scl[1] = ch.scl[2] =
                float(snap_random(10, 15)) / 100.0f;   // small: [0.10..0.15]
        }

        // Life in bounces (:8841)
        ch.life = snap_random(2, 3);

        // Advance quadrant cycle (:8843-8845)
        a = 1 - a;
        if (a) b = 1 - b;
    }
}

// ==========================================================================
// Tick — 24 Hz sim step. Wraps snapshot Animate body (effect_old.cpp:
// 8777-8891). Called once per sim tick by the framerate-independent gate.
// ==========================================================================
static void SimTick(State* st)
{
    st->frameon++;  // :8783

    // Victim-coupling lifecycle (:8785-8797): guarded out in test harness.
    // In production: icedchar->Stop() every frame; early-exit if
    // !IsIced() || IsDead() (snap frameon = kIcedDuration to trigger shatter).

    // Phase 2: SHATTER INIT — runs exactly once when frameon == kIcedDuration.
    // (:8798-8847)
    if (st->frameon == kIcedDuration)
    {
        // icedchar->SetParalize(false) guarded out in test harness.
        SeedChunks(st);
    }

    // Phase 3: TUMBLE + BOUNCE (frameon > kIcedDuration).
    // (:8848-8886)
    if (st->frameon > kIcedDuration)
    {
        st->donebouncing = 1;
        for (int32_t i = 0; i < kMaxIcedChunks; ++i)
        {
            Chunk& ch = st->chunks[i];
            if (ch.life == 0)
                continue;
            st->donebouncing = 0;

            // Integrate position (:8856-8858)
            ch.pos[0] += ch.vel[0];
            ch.pos[1] += ch.vel[1];
            ch.pos[2] += ch.vel[2];

            // Gravity (:8859)
            ch.vel[2] -= kIcedChunkGravity;

            // Integrate spin (:8860-8862)
            ch.rot[0] += ch.ang[0];
            ch.rot[1] += ch.ang[1];
            ch.rot[2] += ch.ang[2];

            // Wrap angles into [0, 2π] (:8863-8874)
            auto wrap_angle = [](float r) -> float {
                if (r > kM2Pi)  r -= kM2Pi;
                if (r < 0.0f)   r += kM2Pi;
                return r;
            };
            ch.rot[0] = wrap_angle(ch.rot[0]);
            ch.rot[1] = wrap_angle(ch.rot[1]);
            ch.rot[2] = wrap_angle(ch.rot[2]);

            // Bounce at floor (:8876-8884)
            // Hardcoded flat local-z=16 plane — dev note "fix this" (:8876)
            // Reconstruct as-is for fidelity (§13.6).
            if (ch.pos[2] <= kBouncePlane)
            {
                ch.pos[2] = kBounceLift;        // snap above floor
                ch.vel[2] *= kBounceVelMul;     // damped reflection
                ch.ang[0] *= kBounceSpinMul;    // spin doubles on bounce
                ch.ang[1] *= kBounceSpinMul;
                ch.ang[2] *= kBounceSpinMul;
                ch.life--;
            }
        }
    }

    // Self-destruct when all chunks are dead (:8887-8890)
    if (st->donebouncing)
        st->alive = false;
}

// ==========================================================================
// Animate — framerate-independent wrapper over SimTick.
// Advances sim at 24 Hz regardless of render frame rate.
// ==========================================================================
static void Animate(State* st, double delta_ms)
{
    if (!st->alive) return;
    st->sim_accum_ms += delta_ms;
    while (st->sim_accum_ms >= double(kSimTickMs))
    {
        st->sim_accum_ms -= double(kSimTickMs);
        SimTick(st);
        if (!st->alive) break;
    }
}

// ==========================================================================
// Render — transcribed from snapshot effect_old.cpp:8900-8953.
// Must be called from the harness submit_world callback (Gotcha #4).
//
// Render():
//     SaveBlendState()
//     SetBlendState()                             // Alpha
//     if frameon < ICED_DURATION:
//         for i=1; i < min(frameon*4, 50); i++:  // crystal hold path
//             obj = GetObject(i)
//             D3DMATRIXClear(&obj->matrix)
//             obj->flags = OBJ3D_MATRIX
//             D3DMATRIXRotateZ(&obj->matrix, angle)
//             ResetExtents(); RenderObject(obj); UpdateExtents()
//     else:
//         obj = GetObject(0)                      // shatter/tumble path
//         for i=0; i < MAX_ICED_CHUNKS; i++:
//             if l[i] == 0: continue
//             ResetExtents()
//             D3DMATRIXClear(&obj->matrix); obj->flags = OBJ3D_MATRIX
//             D3DMATRIXScale(&obj->matrix, &s[i])
//             D3DMATRIXRotateX(&obj->matrix, t[i].x)
//             D3DMATRIXRotateY(&obj->matrix, t[i].y)
//             D3DMATRIXRotateZ(&obj->matrix, t[i].z)
//             D3DMATRIXTranslate(&obj->matrix, &p[i])
//             RenderObject(obj)
//             UpdateExtents()
//     RestoreBlendState()
//
// Matrix build order per forensics §7: Scale · RotX · RotY · RotZ · Translate
// (right-mul, math3d.h:9-11 convention — "vertex applied S first, then Rx, …").
// The shim's accumulate-in-D3D-order matches this semantics directly.
// ==========================================================================
static void Render(State* st)
{
    if (!Renderer || !st->alive) return;

    // Lazy texture binding (Gotcha #1 — defer until GPU upload complete).
    // We check whether all needed mesh handles are registered, not textures
    // directly (RegisterSubMesh handles the texture binding internally).
    // We do need at least one handle valid to draw anything.
    // The handles were registered at Spawn; if any are 0, skip this frame.
    // (In practice Spawn will have logged an error if registration failed.)

    // SaveBlendState + RestoreBlendState (RAII).
    d3d::BlendStateGuard blend_scope;
    d3d::SetBlendState();  // Alpha — forensics §7 (SetBlendState(), not additive)

    // Build the animator's world-translation matrix.
    // The effect's local space origin = victim pos (effect local space);
    // inst_world translates all sub-object draws to world space.
    d3d::Matrix inst_world;
    d3d::MatrixIdentity(inst_world);
    d3d::MatrixTranslate(inst_world,
                         st->base_pos[0], st->base_pos[1], st->base_pos[2]);

    const d3d::BlendMode blend = d3d::CurrentBlend();

    if (st->frameon < kIcedDuration)
    {
        // ---- Crystal HOLD phase (forensics §7 hold path, :8907-8920) ----
        // Reveal sub-objects i=1..min(frameon*4, 50) progressively.
        // Each sub-object uses the shared fixed Z-rotation (angle set at Initialize).
        // build order: Clear → RotateZ(angle) → draw (no scale, no translate)
        const int32_t reveal_raw   = st->frameon * 4;
        const int32_t reveal_count = reveal_raw < kRevealCap ? reveal_raw : kRevealCap;

        for (int32_t i = kSubObjFaceBase; i < reveal_count; ++i)
        {
            if (i >= kTotalSubObjs) break;
            if (st->handles[i] == 0) continue;

            // D3DMATRIXClear + D3DMATRIXRotateZ(angle) (:8913, :8915)
            // OBJ3D_MATRIX path — the matrix is built explicitly.
            d3d::Matrix local;
            d3d::MatrixIdentity(local);         // D3DMATRIXClear
            d3d::MatrixRotateZ(local, st->angle); // D3DMATRIXRotateZ(angle)

            d3d::RenderObject(*Renderer,
                              st->handles[i],
                              local,
                              st->diffuse[i],
                              st->emissive[i],
                              blend,
                              &inst_world,
                              st->base_pos[2]);
        }
    }
    else
    {
        // ---- Shatter / TUMBLE phase (forensics §7 tumble path, :8922-8949) ----
        // Sub-object 0 (cube) re-instanced MAX_ICED_CHUNKS=30 times.
        // Per-chunk: Scale(s[i]) · RotX(t.x) · RotY(t.y) · RotZ(t.z) · Translate(p[i])
        const MeshHandle cube_mesh = st->handles[kSubObjCube];
        if (cube_mesh == 0) return;

        for (int32_t i = 0; i < kMaxIcedChunks; ++i)
        {
            const Chunk& ch = st->chunks[i];
            if (ch.life == 0) continue;

            // D3DMATRIXClear + Scale + RotX + RotY + RotZ + Translate (:8930-8945)
            // Accumulate in snapshot order (Scale first, Translate last).
            d3d::Matrix local;
            d3d::MatrixIdentity(local);                          // D3DMATRIXClear
            d3d::MatrixScale(local, ch.scl[0], ch.scl[1], ch.scl[2]);  // :8932
            d3d::MatrixRotateX(local, ch.rot[0]);               // :8935
            d3d::MatrixRotateY(local, ch.rot[1]);               // :8937
            d3d::MatrixRotateZ(local, ch.rot[2]);               // :8939
            d3d::MatrixTranslate(local, ch.pos[0], ch.pos[1], ch.pos[2]); // :8941-8943

            // sort_z: use chunk's world z for rough back-to-front ordering
            const float sort_z = st->base_pos[2] + ch.pos[2];

            d3d::RenderObject(*Renderer,
                              cube_mesh,
                              local,
                              st->diffuse[kSubObjCube],
                              st->emissive[kSubObjCube],
                              blend,
                              &inst_world,
                              sort_z);
        }
    }

    if (!st->spawn_log_done)
    {
        // Diagnostic: confirm material colors loaded for representative sub-objects.
        // diffuse=(r,g,b,a) emissive=(r,g,b,a) — should be emissive=(0,0,0,1) after
        // the zero-clamp in Spawn(); if any component != 0 the white-wash bug persists.
        // Sub-obj 0 = cube (shatter chunk), 1 = face01 (first facet), 44 = icicle02.
        const int32_t diag_idxs[3] = { 0, 1, 44 };
        log_info("[iced-shim] first render: frameon=%d "
                 "diffuse[0..N]=(%.2f,%.2f,%.2f,%.2f)(%.2f,%.2f,%.2f,%.2f)(%.2f,%.2f,%.2f,%.2f) "
                 "emissive[0..N]=(%.2f,%.2f,%.2f,%.2f)(%.2f,%.2f,%.2f,%.2f)(%.2f,%.2f,%.2f,%.2f)",
                 st->frameon,
                 st->diffuse[diag_idxs[0]][0], st->diffuse[diag_idxs[0]][1],
                 st->diffuse[diag_idxs[0]][2], st->diffuse[diag_idxs[0]][3],
                 st->diffuse[diag_idxs[1]][0], st->diffuse[diag_idxs[1]][1],
                 st->diffuse[diag_idxs[1]][2], st->diffuse[diag_idxs[1]][3],
                 st->diffuse[diag_idxs[2]][0], st->diffuse[diag_idxs[2]][1],
                 st->diffuse[diag_idxs[2]][2], st->diffuse[diag_idxs[2]][3],
                 st->emissive[diag_idxs[0]][0], st->emissive[diag_idxs[0]][1],
                 st->emissive[diag_idxs[0]][2], st->emissive[diag_idxs[0]][3],
                 st->emissive[diag_idxs[1]][0], st->emissive[diag_idxs[1]][1],
                 st->emissive[diag_idxs[1]][2], st->emissive[diag_idxs[1]][3],
                 st->emissive[diag_idxs[2]][0], st->emissive[diag_idxs[2]][1],
                 st->emissive[diag_idxs[2]][2], st->emissive[diag_idxs[2]][3]);
        st->spawn_log_done = 1;
    }
}

// ==========================================================================
// Spawn — load iced.I3D, register sub-object meshes, run Initialize.
// ==========================================================================
State* Spawn(const S3DPoint& origin)
{
    if (!Renderer) return nullptr;

    State* st = new State();
    st->base_pos[0] = float(origin.x);
    st->base_pos[1] = float(origin.y);
    st->base_pos[2] = float(origin.z);

    // Load Magic\iced.I3D (forensics §4 — "Iced" registry name)
    int32_t img_id = TObjectImagery::FindImagery(kImageryPath);
    if (img_id < 0)
    {
        std::string p_str = kImageryPath;
        img_id = TObjectImagery::RegisterImagery(p_str.data());
    }
    if (img_id < 0)
    {
        log_error("[iced-shim] could not resolve '%s'", kImageryPath);
        delete st;
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[iced-shim] '%s' is not T3DImagery", kImageryPath);
        delete st;
        return nullptr;
    }
    st->imagery = img3d;

    // Poke NumObjects() to trigger lazy mesh/texture slot population
    // (pattern from bespoke ports — NumTextures() alone doesn't trigger upload).
    const int32_t num_obj = img3d->NumObjects();
    const int32_t num_tex = img3d->NumTextures();

    if (num_obj <= 0)
    {
        log_error("[iced-shim] '%s' has no sub-objects", kImageryPath);
        delete st;
        return nullptr;
    }

    // Register mesh handles for each sub-object we'll draw.
    // Indices 0..49: 0=cube, 1..43=face01..face43, 44..49=icicle01..06
    // (forensics §4 — file-order confirmed via string dump).
    const int32_t reg_count = kTotalSubObjs < num_obj ? kTotalSubObjs : num_obj;
    for (int32_t i = 0; i < reg_count; ++i)
    {
        st->handles[i] = d3d::RegisterSubMesh(*Renderer, img3d, i, num_tex);
        d3d::LoadMaterial(img3d, i, st->diffuse[i], st->emissive[i]);
        // iced.I3D material emissive = (1,1,1) — the asset's D3DMATERIAL emissive
        // channel. In the helper-mesh shader "emissive" is additive self-illumination
        // (col += emissive.rgb), so passing (1,1,1) blows every fragment to white,
        // washing out the ice-blue texture color. Forensics §7: "Unlit / self-lit,
        // without material zeroing … asset color passes through under MODULATE" —
        // no additive emissive contribution; the texture carries the ice-blue color.
        // Zero it here so the shader's ambient path (diffuse→ambient route) drives
        // the unlit look: col = tex.rgb * diffuse.rgb * ambient_factor.
        st->emissive[i][0] = 0.0f;
        st->emissive[i][1] = 0.0f;
        st->emissive[i][2] = 0.0f;
        // st->emissive[i][3] stays 1.0f (alpha channel, not used in shader formula)
        if (st->handles[i] == 0)
        {
            log_warn("[iced-shim] sub-obj %d mesh registration failed "
                     "(may be empty/degenerate)", i);
        }
    }

    if (st->handles[kSubObjCube] == 0)
    {
        log_warn("[iced-shim] cube sub-obj (0) mesh handle is 0 — "
                 "tumble phase will be invisible");
    }

    // Run Initialize (snapshot :8749-8756)
    Initialize(st);

    log_info("[iced-shim] spawned at (%d,%d,%d) num_obj=%d num_tex=%d "
             "cube_handle=%u angle=%.3f",
             origin.x, origin.y, origin.z,
             num_obj, num_tex, st->handles[kSubObjCube], double(st->angle));
    return st;
}

// ==========================================================================
// Public harness entry points.
// Tick drives the 24 Hz accumulator.
// Submit (SubmitWorld) must be called from submit_world callback only.
// ==========================================================================

// Tick advances the sim accumulator using the real wall-clock delta from
// TTime::DeltaTime(). The 24 Hz gate inside Animate() ensures sim ticks fire
// at the correct rate regardless of render frame rate.
void Tick(State* st)
{
    if (st && st->alive)
    {
        // Feed the real wall-clock delta into the 24 Hz accumulator.
        // TTime::DeltaTime() returns seconds; ×1000 → milliseconds.
        // Using a fixed 1000/60 here would make the sim run at exactly
        // 60 Hz × (1000/60 ms) = 1000 ms/sec of sim time regardless of
        // actual frame rate. At 120 fps the sim would run 2× too fast;
        // at 30 fps 0.5× too slow. Real delta-time is correct.
        // (Pattern mirrors TIcedEffect_Bespoke::TickAndSubmitForTest_BESPOKE
        // effect.cpp:8634 — sim_accum_ms_ += TTime::DeltaTime() * 1000.0)
        Animate(st, TTime::DeltaTime() * 1000.0);
    }
}

// SubmitWorld — called from submit_world callback (after BeginTilePass).
// WorldMesh draws MUST be submitted from submit_world (Gotcha #4).
void SubmitWorld(State* st)
{
    if (st && st->alive)
        Render(st);
}

// Submit is a no-op stub — all draws are WorldMesh, routed via SubmitWorld.
// It exists so the harness wiring is consistent with the other shim ports.
void Submit(State* /*st*/)
{
    // No billboard draws; all draws go through SubmitWorld.
}

void Destroy(State* st) { delete st; }

} // namespace iced_shim
