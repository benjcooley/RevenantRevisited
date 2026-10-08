// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  sparks.cpp - TParticle3DAnimator ("sparks") port via the d3d:: shim *
// *                                                                       *
// *  Second SHIM VALIDATION PORT (after Fountain). Proves the pattern    *
// *  generalizes beyond the cyan-sparkle-column case to a one-shot       *
// *  burst-with-gravity emitter.                                          *
// *                                                                       *
// *  Source of truth: docs/vfx/forensics/SPARKS_TSparkAnimator.md        *
// *  (665 lines, retail-confirmed with 3 divergences).                   *
// *                                                                       *
// *  Snapshot Render: effect_old.cpp:4944-4986                           *
// *  Snapshot Animate: effect_old.cpp:4790-4942 (non-seeking, no-bounce  *
// *    path only — bounce path is a known-deferred TODO; see §6.2)       *
// *  Snapshot InitParticles: effect_old.cpp:4752-4787                    *
// *  Spark-specific constants: character.cpp:2290-2313 (TCharacter::    *
// *    EffectBurst — the actual caller passes these to InitParticles)   *
// *                                                                       *
// *  Retail fidelity: 3 documented divergences from snapshot, USE RETAIL *
// *    - gravity: snapshot 0.2 → retail 0.25                             *
// *    - trails:  snapshot 1   → retail 2                                *
// *    - bounce:  snapshot 0   → retail 1 (not implemented in iter1)    *
// *                                                                       *
// *  This iter1 implements the non-bounce path. Bounce can be added as   *
// *  an iter2 follow-up — visually it ricochets sparks off floor; the    *
// *  test rig has no floor mesh so it's not critical for validation.    *
// *************************************************************************

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "../d3dport.h"
#include "../logging.h"
#include "../math3d.h"
#include "../imagery.h"
#include "../renderer.h"

extern TRenderer* Renderer;

namespace sparks_shim {

// =========================================================================
// Constants — every value cites a forensics §3 / source line.
// =========================================================================
// Caller-side ("sparks" use) — from character.cpp:2290-2313 / cls_0x5a7b98:
constexpr int32_t kMinCount        = 15;     // character.cpp:2290
constexpr int32_t kMaxCount        = 25;
constexpr float   kPosSpread       = 3.0f;   // ±3 wu each axis (character.cpp:2294-96)
constexpr float   kVelSpread       = 0.5f;   // ±0.5 wu/tick each axis (character.cpp:2300-02)
constexpr float   kGravity         = 0.25f;  // RETAIL (was 0.2 snapshot) (cls_0x5a7b98:4645)
constexpr int32_t kTrails          = 2;      // RETAIL (was 1 snapshot) (cls_0x5a7b98:4646)
constexpr int32_t kMinStart        = 0;      // character.cpp:2305
constexpr int32_t kMaxStart        = 8;
constexpr int32_t kMinLife         = 20;     // character.cpp:2307
constexpr int32_t kMaxLife         = 40;
constexpr bool    kBounce          = true;   // RETAIL (was false snapshot)
                                              // NOT YET IMPLEMENTED — see iter2 TODO

// Maximum count we'll pre-allocate (above kMaxCount to allow for
// future-call variants).
constexpr int32_t kMaxParticles    = 32;

constexpr const char* kImageryPath = "Misc\\Sparks.I3D";

// Snapshot's spark sub-objects are 4 quads (photon/photon01/02/03) over a
// 2×2 atlas texture. `objflags = 1 << (ObjId() & 3)` selects ONE sub-object
// per burst — every particle in a given burst draws the same variant.
constexpr int32_t kNumSparkSubObjs = 4;

// =========================================================================
// Per-effect state.
// =========================================================================
struct State
{
    // Per-particle arrays — snapshot's p[], v[], l[], s[].
    float    p[kMaxParticles][3]   = {};
    float    v[kMaxParticles][3]   = {};
    int32_t  l[kMaxParticles]      = {};   // life ticks remaining (0 = dead)
    int32_t  s[kMaxParticles]      = {};   // start delay ticks remaining
    int32_t  count                 = 0;    // actual particle count this burst

    // Per-burst selection (forensics §3: `objflags = 1 << (ObjId() & 3)`
    // → ONE sub-object per burst).
    int32_t  sub_obj               = 0;    // 0..3

    // Effect world origin (caller-passed; particles position is local
    // relative to this).
    float    base_pos[3]           = {0, 0, 0};

    // Lifecycle.
    bool     alive                 = true;

    // Resolved asset data (texture handle lazy-bound on first Submit per
    // gotcha #1; UV sub-rect resolved at Spawn per gotcha #2).
    T3DImagery*    imagery     = nullptr;
    TTextureHandle texture     = kInvalidTexture;
    float          diffuse[4]  = {1, 1, 1, 1};
    float          emissive[4] = {0, 0, 0, 1};
    float          uv_rect[4]  = {0, 0, 1, 1};

    // Diagnostics.
    int32_t  spawn_log_done    = 0;
};

// Inclusive-range random matching snapshot semantics
// (`rand() % (max-min+1) + min`, src/revutils.cpp:1609-1610).
static int32_t snap_random(int32_t lo, int32_t hi)
{
    if (hi <= lo) return lo;
    return lo + (std::rand() % (hi - lo + 1));
}
static float snap_random_signed_unit()
{
    // random(-100, 100) / 100.0 — yields ~[-1, +1] inclusive.
    return float(snap_random(-100, 100)) / 100.0f;
}

// =========================================================================
// InitParticles — transcribed from snapshot effect_old.cpp:4752-4787.
//
//   for c in 0..particles-1:
//       p[c] = params.pos    + params.pspread * random_signed_unit()
//       v[c] = params.dir    + params.spread  * random_signed_unit()
//       l[c] = random(minlife, maxlife)
//       s[c] = random(minstart, maxstart)
//       o[c] = (per-particle sub-object pick — but for sparks, objflags
//              has a single bit so all particles share the chosen one)
// =========================================================================
static void InitParticles(State* st, const float dir_xyz[3])
{
    st->count = snap_random(kMinCount, kMaxCount);
    if (st->count > kMaxParticles) st->count = kMaxParticles;

    for (int32_t c = 0; c < st->count; ++c)
    {
        // pos = params.pos + pspread * jitter; params.pos == (0,0,0) in
        // particle-local space (caller's params.pos becomes our base_pos
        // and is applied at Render via the world translate).
        st->p[c][0] = kPosSpread * snap_random_signed_unit();
        st->p[c][1] = kPosSpread * snap_random_signed_unit();
        st->p[c][2] = kPosSpread * snap_random_signed_unit();

        // vel = params.dir + spread * jitter
        st->v[c][0] = dir_xyz[0] + kVelSpread * snap_random_signed_unit();
        st->v[c][1] = dir_xyz[1] + kVelSpread * snap_random_signed_unit();
        st->v[c][2] = dir_xyz[2] + kVelSpread * snap_random_signed_unit();

        st->l[c] = snap_random(kMinLife,  kMaxLife);   // 20..40 ticks
        st->s[c] = snap_random(kMinStart, kMaxStart);  // 0..8 ticks delay
    }
}

// =========================================================================
// Animate — transcribed from snapshot effect_old.cpp:4826-4858 (non-seeking,
// pre-bounce path). When bounce is added (iter2) it inserts at :4852-4858.
//
//   for c in 0..particles-1:
//       if l[c] <= 0: continue
//       if s[c] > 0: s[c]--; continue
//       p[c] += v[c]
//       l[c]--
//       v[c].z -= gravity
//       // bounce skipped (kBounce=true, NYI — see header TODO)
// =========================================================================
static void Animate(State* st)
{
    bool any_alive = false;
    for (int32_t c = 0; c < st->count; ++c)
    {
        if (st->l[c] <= 0) continue;
        any_alive = true;

        if (st->s[c] > 0) { st->s[c]--; continue; }

        st->p[c][0] += st->v[c][0];
        st->p[c][1] += st->v[c][1];
        st->p[c][2] += st->v[c][2];

        st->l[c]--;
        st->v[c][2] -= kGravity;
        // bounce-on-floor (forensics §6.2 :4852-4858) — kBounce=true but
        // not implemented in iter1; test rig has no floor mesh so the
        // visual cost is negligible. iter2 TODO.
    }
    // killobj: when all particles done, self-destruct. (forensics §6.2
    // :4940-4941). For the standalone test rig we let the harness
    // re-spawn rather than freeing.
    if (!any_alive) st->alive = false;
}

// =========================================================================
// Render — transcribed from snapshot effect_old.cpp:4944-4986.
//
//   SaveBlendState(); SetBlendState();         // Alpha
//   ResetExtents()
//   for c in 0..particles-1:
//       if s[c] > 0 or l[c] <= 0: continue
//       pp = p[c]; vv = v[c]
//       obj = GetObject(o[c])                  // per-burst single sub-obj
//       for d in 0..trails-1:                  // trails=2 for retail
//           obj->flags |= OBJ3D_POS1
//           obj->pos = pp
//           RenderObject(obj)
//           pp += vv; vv.z -= gravity          // motion-streak step
//   UpdateExtents(); RestoreBlendState()
// =========================================================================
static void Render(State* st)
{
    if (!Renderer || !st->alive) return;

    // Lazy texture binding (gotcha #1).
    if (st->texture == kInvalidTexture && st->imagery)
    {
        if (st->imagery->NumTextures() > 0)
        {
            S3DTex tex = {};
            st->imagery->GetTexture(0, &tex);
            st->texture = tex.htexture;
        }
        if (st->texture == kInvalidTexture) return;
    }
    if (st->texture == kInvalidTexture) return;

    d3d::BlendStateGuard scope;
    d3d::SetBlendState();   // Alpha — forensics §7 (snapshot-only, dev-confirmed)

    // Animator world translate = effect's base position.
    d3d::Matrix inst_world;
    d3d::MatrixIdentity(inst_world);
    d3d::MatrixTranslate(inst_world,
                         st->base_pos[0], st->base_pos[1], st->base_pos[2]);

    // Shared obj setup — per-burst sub-object pick + UV cell (gotcha #2).
    d3d::Obj obj;
    obj.objnum = st->sub_obj;
    std::memcpy(obj.diffuse,  st->diffuse,  sizeof(obj.diffuse));
    std::memcpy(obj.emissive, st->emissive, sizeof(obj.emissive));
    const float u_lo = st->uv_rect[0];
    const float v_lo = st->uv_rect[1];
    const float u_hi = u_lo + st->uv_rect[2];
    const float v_hi = v_lo + st->uv_rect[3];
    obj.lverts[0].tu = u_lo; obj.lverts[0].tv = v_lo;
    obj.lverts[1].tu = u_lo; obj.lverts[1].tv = v_hi;
    obj.lverts[2].tu = u_hi; obj.lverts[2].tv = v_lo;
    obj.lverts[3].tu = u_hi; obj.lverts[3].tv = v_hi;

    // Per-spark base size (gotcha #3). Sparks.I3D photon sub-object is
    // authored as a ~10x10 wu quad; we match Fountain's pattern of using
    // a single base size constant. 32 wu is half Fountain's 64 since sparks
    // are typically much smaller than fountain sparkles.
    constexpr float kSparkSizeWu = 32.0f;

    for (int32_t c = 0; c < st->count; ++c)
    {
        if (st->s[c] > 0 || st->l[c] <= 0) continue;

        // Snapshot maintains pp + vv locals to step them per trail draw.
        float pp[3] = { st->p[c][0], st->p[c][1], st->p[c][2] };
        float vv[3] = { st->v[c][0], st->v[c][1], st->v[c][2] };

        for (int32_t d = 0; d < kTrails; ++d)
        {
            // obj->flags |= OBJ3D_POS1 (translate only — snapshot :4963).
            // Also Flag_Verts so RenderObject honors the UV sub-rect we
            // stamped into obj.lverts.
            obj.flags = d3d::Flag_Pos1 | d3d::Flag_Verts;
            obj.scl[0] = obj.scl[1] = obj.scl[2] = kSparkSizeWu;
            obj.pos[0] = pp[0];
            obj.pos[1] = pp[1];
            obj.pos[2] = pp[2];

            d3d::RenderObject(*Renderer, obj,
                              d3d::Orientation::ScreenAligned,
                              st->texture, &inst_world);

            // Step pp/vv forward for the next trail copy (motion-streak).
            // snapshot :4967-4970.
            pp[0] += vv[0];
            pp[1] += vv[1];
            pp[2] += vv[2];
            vv[2] -= kGravity;
        }
    }

    if (!st->spawn_log_done)
    {
        log_info("[sparks-shim] first render: count=%d sub_obj=%d "
                 "diffuse=(%.2f,%.2f,%.2f) tex=%u base=(%.0f,%.0f,%.0f) "
                 "uv=(%.3f,%.3f %.3f,%.3f)",
                 st->count, st->sub_obj,
                 st->diffuse[0], st->diffuse[1], st->diffuse[2],
                 st->texture,
                 st->base_pos[0], st->base_pos[1], st->base_pos[2],
                 st->uv_rect[0], st->uv_rect[1],
                 st->uv_rect[2], st->uv_rect[3]);
        st->spawn_log_done = 1;
    }
}

// =========================================================================
// Spawn — load the asset, seed particles, return State.
//
// `sub_obj` selects which of the 4 photon variants this burst uses
// (forensics §6.1: objflags = 1 << (ObjId() & 3)). The test harness can
// pass 0..3 explicitly; the real caller (TCharacter::EffectBurst) derives
// it from the spawning effect's object ID.
// =========================================================================
State* Spawn(const S3DPoint& origin, int32_t sub_obj)
{
    if (!Renderer) return nullptr;
    if (sub_obj < 0 || sub_obj >= kNumSparkSubObjs) sub_obj = 0;

    State* st = new State();
    st->sub_obj    = sub_obj;
    st->base_pos[0] = float(origin.x);
    st->base_pos[1] = float(origin.y);
    st->base_pos[2] = float(origin.z);

    // Bind asset.
    int32_t img_id = TObjectImagery::FindImagery(kImageryPath);
    if (img_id < 0)
    {
        std::string p = kImageryPath;
        img_id = TObjectImagery::RegisterImagery(p.data());
    }
    if (img_id < 0)
    {
        log_error("[sparks-shim] could not resolve '%s'", kImageryPath);
        delete st;
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[sparks-shim] '%s' is not T3DImagery", kImageryPath);
        delete st;
        return nullptr;
    }
    st->imagery = img3d;

    // Material for the chosen sub-object.
    d3d::LoadMaterial(img3d, sub_obj, st->diffuse, st->emissive);

    // UV sub-rect for the chosen sub-object's 2x2 atlas cell (gotcha #2).
    {
        const int32_t nv = img3d->NumObjVerts(sub_obj);
        if (nv > 0)
        {
            std::vector<S3DVertex> vbuf(size_t(nv), S3DVertex{});
            img3d->GetObjVerts(sub_obj, vbuf.data(), 0, 0,
                               ERender3DVertex::Vertex);
            float minu = vbuf[0].tu, maxu = vbuf[0].tu;
            float minv = vbuf[0].tv, maxv = vbuf[0].tv;
            for (int32_t i = 1; i < nv; ++i)
            {
                if (vbuf[i].tu < minu) minu = vbuf[i].tu;
                if (vbuf[i].tu > maxu) maxu = vbuf[i].tu;
                if (vbuf[i].tv < minv) minv = vbuf[i].tv;
                if (vbuf[i].tv > maxv) maxv = vbuf[i].tv;
            }
            st->uv_rect[0] = minu;
            st->uv_rect[1] = minv;
            st->uv_rect[2] = maxu - minu;
            st->uv_rect[3] = maxv - minv;
        }
    }

    // For the standalone test rig we synthesize a plausible direction —
    // upward + outward in +X (real caller passes vect = ConvertToVector(
    // face + jitter, 100)/100, forensics §3 + §5). XY magnitude ≈ 1 wu/tick,
    // z slightly up so gravity pulls sparks back down in a visible arc.
    const float test_dir[3] = { 1.0f, 0.0f, 0.5f };
    InitParticles(st, test_dir);

    log_info("[sparks-shim] spawned sub_obj=%d at (%d,%d,%d) "
             "count=%d gravity=%.3f trails=%d",
             sub_obj, origin.x, origin.y, origin.z,
             st->count, double(kGravity), int(kTrails));
    return st;
}

void Tick(State* st)    { if (st && st->alive) Animate(st); }
void Submit(State* st)  { if (st && st->alive) Render(st);  }
void Destroy(State* st) { delete st; }

} // namespace sparks_shim
