// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  fizzle.cpp - TFizzleEffect port via the d3d:: shim                  *
// *                                                                       *
// *  Third SHIM VALIDATION PORT (after Fountain + Sparks). First port    *
// *  to exercise d3d::RenderObjectSpinning (WorldXY ground-tipped        *
// *  billboard with per-particle in-plane rotation) — the shim          *
// *  extension added specifically for this effect.                       *
// *                                                                       *
// *  Source of truth: docs/vfx/forensics/X21_TFizzleEffect.md            *
// *  Snapshot Animate: effect_old.cpp:12361-12487                        *
// *  Snapshot Render:  effect_old.cpp:12489-12502 (effect-side bracket)  *
// *                  + effectcomp.cpp:1070-1109 (per-system per-particle)*
// *  Snapshot Init:    effect_old.cpp:12350-12358 (3 systems w/ box01/02/03)*
// *                                                                       *
// *  Retail fidelity: retail-partial. Registration + asset payload       *
// *  byte-identical to shipped; DUST_* per-tick constants are            *
// *  snapshot-only (no retail animator body extracted; see forensics §2.1).*
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

namespace fizzle_shim {

// =========================================================================
// Constants — every value cites forensics §3 / source line.
// All snapshot-only (no retail body extracted, see forensics §2.1).
// =========================================================================
constexpr int32_t kDustCount       = 30;      // effect_old.cpp:12293 — per-system cap
constexpr int32_t kDustFrame       = 15;      // effect_old.cpp:12294 — emission window (ticks)
constexpr int32_t kDustSpread      = 15;      // effect_old.cpp:12295 — ±wu XY jitter
constexpr int32_t kDustMinZVel     = 5;       // effect_old.cpp:12296 — ×0.1 → -0.5 wu/tick (down)
constexpr int32_t kDustMaxZVel     = 45;      // effect_old.cpp:12297 — ×0.1 → -4.5 wu/tick (down)
constexpr int32_t kDustRot         = 15;      // effect_old.cpp:12298 — deg/tick spin range
constexpr int32_t kDustMinScl      = 5;       // effect_old.cpp:12299 — ×0.01 → 0.05 max scale
constexpr int32_t kDustMaxScl      = 25;      // effect_old.cpp:12300 — ×0.01 → 0.25 max scale
constexpr float   kDustSclInc      = 0.05f;   // effect_old.cpp:12301 — grow rate
constexpr float   kDustSclDec      = 0.02f;   // effect_old.cpp:12302 — shrink rate
constexpr float   kDustAdd         = 1.5f;    // effect_old.cpp:12303 — spawn accumulator/tick
constexpr int32_t kSpawnZMin       = 70;      // effect_old.cpp:12387 — wu above origin
constexpr int32_t kSpawnZMax       = 130;
constexpr int32_t kLifeSpanGrow    = 100;     // effect_old.cpp:12405 — phase tag = growing
constexpr int32_t kLifeSpanShrink  = 200;     // effect_old.cpp:12452 — phase tag = shrinking
constexpr int32_t kLifeSpanDead    = 0;       // effect_old.cpp:12463 — phase tag = freed

constexpr float   kFlickerScale    = 1.5f;    // effectcomp.cpp:1095-97 — boost when flicker set
constexpr float   kStaticRotZ      = -float(M_PI) / 4.0f;  // effectcomp.cpp:1089 — -45° static spin
constexpr float   kTipRotX         = -float(M_PI) / 2.0f;  // effectcomp.cpp:1088 — ground tip (unused; RenderObjectSpinning handles via WorldXY)

constexpr int32_t kNumSystems      = 3;       // blue / red / purple
constexpr const char* kImageryPath = "Magic\\Fizzle.I3D";

// Index↔label mapping per forensics §4:
//   sub-obj 0 = box01 = blue
//   sub-obj 1 = box02 = purple
//   sub-obj 2 = box03 = red (the "red" system draws box03)
// Note the snapshot's Init crosses indices: red.Init(GetObject(2)) +
// purple.Init(GetObject(1)). We just map system-index → sub-object-index
// directly:
constexpr int32_t kSystemSubObj[kNumSystems] = { 0, 1, 2 };  // blue, purple, red

// Per-particle base size. Particles' max scale is 0.05..0.25 (forensics
// §3). The authored quad's half-extent in WU isn't measured precisely;
// matching the Fountain/Sparks family with ~256 wu base yields a peak
// particle size of 0.25*256 = 64 wu, which is consistent with the dust-
// puff visual size in retail combat footage.
constexpr float   kFizzleBaseSizeWu = 256.0f;

// =========================================================================
// Per-particle state — matches snapshot SParticle + animator's temp/flicker.
// =========================================================================
struct Particle
{
    bool    used        = false;
    float   pos[3]      = {0, 0, 0};
    float   vel[3]      = {0, 0, 0};
    float   scl         = 0.0f;       // uniform (snapshot writes scl.x=y=z)
    float   rot_z       = 0.0f;       // radians (snapshot uses deg, we convert at use)
    int32_t life        = 0;          // tick counter (incremented by ParticleSystem)
    int32_t life_span   = kLifeSpanGrow;  // phase: 100=grow, 200=shrink, 0=dead
    float   max_scl     = 0.1f;       // per-particle peak scale (temp.z)
    float   spin_rate   = 0.0f;       // deg/tick (temp.y)
    bool    flicker     = false;      // re-rolled per tick (×1.5 scale boost when set)
};

struct ParticleSystem
{
    Particle p[kDustCount];           // bounded
};

struct State
{
    ParticleSystem systems[kNumSystems];   // blue, purple, red
    float    base_pos[3] = {0, 0, 0};
    int32_t  frame_count = 0;
    float    add         = 0.0f;           // emission accumulator
    bool     alive       = true;

    // Per-sub-object asset data (one set per system / sub-object).
    T3DImagery*    imagery     = nullptr;
    TTextureHandle texture[kNumSystems]   = {kInvalidTexture,
                                              kInvalidTexture,
                                              kInvalidTexture};
    float          diffuse[kNumSystems][4]  = {{1,1,1,1}, {1,1,1,1}, {1,1,1,1}};
    float          emissive[kNumSystems][4] = {{0,0,0,1}, {0,0,0,1}, {0,0,0,1}};
    float          uv_rect[kNumSystems][4]  = {{0,0,1,1}, {0,0,1,1}, {0,0,1,1}};

    int32_t  spawn_log_done = 0;
};

// random(lo, hi) — inclusive on both ends, matches snapshot semantics.
static int32_t snap_random(int32_t lo, int32_t hi)
{
    if (hi <= lo) return lo;
    return lo + (std::rand() % (hi - lo + 1));
}

// =========================================================================
// SpawnParticle — picks one of the 3 systems at random, fills a fresh
// particle. Transcribed from snapshot effect_old.cpp:12380-12422.
// =========================================================================
static void SpawnParticle(State* st)
{
    // random(1, 3) → pick system 0..2 (snapshot uses 1..3 but it's the
    // same set; the random branch in :12415-12422 picks ONE system per
    // particle, which we mirror).
    const int32_t sysidx = snap_random(0, kNumSystems - 1);

    // Find a free slot in that system; if full, drop the particle
    // (matches snapshot bounded behavior — system caps at DUST_COUNT
    // per forensics §3 "particle budget" note).
    Particle* slot = nullptr;
    for (int32_t i = 0; i < kDustCount; ++i)
    {
        if (!st->systems[sysidx].p[i].used)
        {
            slot = &st->systems[sysidx].p[i];
            break;
        }
    }
    if (!slot) return;

    slot->used        = true;
    slot->pos[0]      = float(snap_random(-kDustSpread, kDustSpread));   // :12385
    slot->pos[1]      = float(snap_random(-kDustSpread, kDustSpread));   // :12386
    slot->pos[2]      = float(snap_random(kSpawnZMin, kSpawnZMax));      // :12387
    slot->vel[0]      = 0.0f;                                             // :12390
    slot->vel[1]      = 0.0f;                                             // :12391
    slot->vel[2]      = -float(snap_random(kDustMinZVel, kDustMaxZVel))   // :12392
                        * 0.1f;                                            //   ×0.1
    slot->scl         = 0.0f;                                             // :12398 — start invisible
    slot->rot_z       = 0.0f;                                             // :12401-02
    slot->life        = 0;
    slot->life_span   = kLifeSpanGrow;                                   // :12405
    slot->flicker     = (snap_random(0, 1) != 0);                        // :12408
    slot->spin_rate   = float(snap_random(-kDustRot, kDustRot));         // :12411 — deg/tick
    slot->max_scl     = float(snap_random(kDustMinScl, kDustMaxScl))      // :12413
                        * 0.01f;                                           //   ×0.01
}

// =========================================================================
// Animate — transcribed from snapshot effect_old.cpp:12361-12487 +
// effectcomp.cpp:1041-1068 (TParticleSystem::Animate per-system step).
// =========================================================================
static void Animate(State* st)
{
    if (!st->alive) return;

    // :12365 — keep effect alive every tick
    // :12367 — accumulator advance
    st->add += kDustAdd;
    // :12371 — frame counter advance
    st->frame_count++;

    // :12373-75 — TParticleSystem::Animate per system (the generic part).
    // For Fizzle, check_move = true, so: pos += vel; vel *= acc (acc=1.0
    // so velocity unchanged). Also: if life >= life_span: used=false.
    for (int32_t s = 0; s < kNumSystems; ++s)
    {
        for (int32_t i = 0; i < kDustCount; ++i)
        {
            Particle& p = st->systems[s].p[i];
            if (!p.used) continue;
            // effectcomp.cpp:1046 — generic system kills when life >= life_span.
            // life_span=0 (dead phase) triggers immediate kill here.
            if (p.life >= p.life_span)
            {
                p.used = false;
                continue;
            }
            p.life++;
            // :1057-1060 — check_move=true branch: straight-line integrate.
            p.pos[0] += p.vel[0];
            p.pos[1] += p.vel[1];
            p.pos[2] += p.vel[2];
            // vel *= acc (acc=1.0) → no change
        }
    }

    // :12378-12422 — emission loop, only while frame_count < DUST_FRAME.
    while (st->add > 1.0f && st->frame_count < kDustFrame)
    {
        st->add -= 1.0f;
        SpawnParticle(st);
    }

    // :12429-12476 — per-particle scale state machine + spin.
    bool any_alive = false;
    for (int32_t s = 0; s < kNumSystems; ++s)
    {
        for (int32_t i = 0; i < kDustCount; ++i)
        {
            Particle& p = st->systems[s].p[i];
            if (!p.used) continue;

            if (p.life_span == kLifeSpanGrow)
            {
                // :12447-49 — grow
                p.scl += kDustSclInc;
                // :12451-52 — peak reached, flip to shrinking
                if (p.scl > p.max_scl) p.life_span = kLifeSpanShrink;
            }
            else if (p.life_span == kLifeSpanShrink)
            {
                // :12456-58 — shrink
                p.scl -= kDustSclDec;
                // :12460-64 — done, mark for next-tick reap by TParticleSystem
                if (p.scl <= 0.0f)
                {
                    p.scl = 0.0f;
                    p.life_span = kLifeSpanDead;
                }
            }

            // :12467 — spin (radians per tick; snapshot stores deg, our
            // particle stores deg too via spin_rate, converted at use).
            p.rot_z += p.spin_rate * (float(M_PI) / 180.0f);
            // wrap to [0, 2π) for cleanliness (snapshot wraps to [0,360))
            while (p.rot_z >= 2.0f * float(M_PI)) p.rot_z -= 2.0f * float(M_PI);
            while (p.rot_z <  0.0f)               p.rot_z += 2.0f * float(M_PI);

            // :12475 — re-roll flicker every tick
            p.flicker = (snap_random(0, 1) != 0);

            any_alive = true;
        }
    }

    // :12482-85 — self-kill once emission is over AND every particle done.
    if (st->frame_count >= kDustFrame && !any_alive)
        st->alive = false;
}

// =========================================================================
// Render — transcribed from effect_old.cpp:12489-12502 (effect bracket) +
// effectcomp.cpp:1070-1109 (per-particle).
// =========================================================================
static void Render(State* st)
{
    if (!Renderer || !st->alive) return;

    // Lazy texture binding (gotcha #1) — defer per-sub-object texture
    // resolution until the I3D loader has uploaded to GPU. Re-tries
    // every Submit until all 3 are valid.
    if (st->imagery)
    {
        for (int32_t s = 0; s < kNumSystems; ++s)
        {
            if (st->texture[s] != kInvalidTexture) continue;
            const int32_t sub = kSystemSubObj[s];
            // Each sub-object has its own texture (forensics §4 — 3
            // textures total, one per dust-puff color). The texture
            // index matches the sub-object's authored material.texture.
            S3DObj o = {};
            st->imagery->GetObject(sub, &o);
            int32_t tex_idx = 0;
            if (o.material >= 0 && o.material < st->imagery->NumMaterials())
            {
                S3DMat m = {};
                st->imagery->GetMaterial(o.material, &m);
                tex_idx = (m.texture >= 0 &&
                           m.texture < st->imagery->NumTextures())
                          ? m.texture : sub;
            }
            else
            {
                tex_idx = sub;
            }
            if (tex_idx >= st->imagery->NumTextures()) tex_idx = 0;
            S3DTex t = {};
            st->imagery->GetTexture(tex_idx, &t);
            st->texture[s] = t.htexture;
        }
    }
    // If any texture is still invalid this tick, wait another.
    for (int32_t s = 0; s < kNumSystems; ++s)
        if (st->texture[s] == kInvalidTexture) return;

    // :12492-93 — SaveBlendState + SetBlendState (Alpha).
    d3d::BlendStateGuard scope;
    d3d::SetBlendState();

    // Animator world translate.
    d3d::Matrix inst_world;
    d3d::MatrixIdentity(inst_world);
    d3d::MatrixTranslate(inst_world,
                         st->base_pos[0], st->base_pos[1], st->base_pos[2]);

    // Per-particle draw loop. The static -π/4 rotZ and the per-particle
    // spin compose: final rot = particle.rot_z + kStaticRotZ.
    // RenderObjectSpinning takes a single rotation_rad value covering
    // both — and applies the WorldXY tip internally via SubmitFxParticle.
    d3d::Obj obj;
    for (int32_t s = 0; s < kNumSystems; ++s)
    {
        const int32_t sub = kSystemSubObj[s];
        obj.objnum = sub;
        std::memcpy(obj.diffuse,  st->diffuse[s],  sizeof(obj.diffuse));
        std::memcpy(obj.emissive, st->emissive[s], sizeof(obj.emissive));
        const float u_lo = st->uv_rect[s][0];
        const float v_lo = st->uv_rect[s][1];
        const float u_hi = u_lo + st->uv_rect[s][2];
        const float v_hi = v_lo + st->uv_rect[s][3];
        obj.lverts[0].tu = u_lo; obj.lverts[0].tv = v_lo;
        obj.lverts[1].tu = u_lo; obj.lverts[1].tv = v_hi;
        obj.lverts[2].tu = u_hi; obj.lverts[2].tv = v_lo;
        obj.lverts[3].tu = u_hi; obj.lverts[3].tv = v_hi;

        for (int32_t i = 0; i < kDustCount; ++i)
        {
            const Particle& p = st->systems[s].p[i];
            if (!p.used) continue;
            // :1092-99 — scale with flicker boost
            float scl = p.scl;
            if (p.flicker) scl *= kFlickerScale;
            if (scl <= 0.0f) continue;

            obj.flags = d3d::Flag_Matrix | d3d::Flag_Verts;
            const float size = scl * kFizzleBaseSizeWu;
            obj.scl[0] = obj.scl[1] = obj.scl[2] = size;
            obj.pos[0] = p.pos[0];
            obj.pos[1] = p.pos[1];
            obj.pos[2] = p.pos[2];

            // Composite rotation = per-particle spin + static -π/4
            // (the WorldXY -π/2 tip is handled by RenderObjectSpinning
            // via its WorldXY orientation routing).
            const float rot = p.rot_z + kStaticRotZ;
            d3d::RenderObjectSpinning(*Renderer, obj, rot,
                                      st->texture[s], &inst_world);
        }
    }

    if (!st->spawn_log_done)
    {
        log_info("[fizzle-shim] first render: frame=%d add=%.2f "
                 "tex=(%u,%u,%u) base=(%.0f,%.0f,%.0f)",
                 st->frame_count, double(st->add),
                 st->texture[0], st->texture[1], st->texture[2],
                 st->base_pos[0], st->base_pos[1], st->base_pos[2]);
        st->spawn_log_done = 1;
    }
}

// =========================================================================
// Spawn — load asset, resolve sub-object materials + UV sub-rects.
// =========================================================================
State* Spawn(const S3DPoint& origin)
{
    if (!Renderer) return nullptr;

    State* st = new State();
    st->base_pos[0] = float(origin.x);
    st->base_pos[1] = float(origin.y);
    st->base_pos[2] = float(origin.z);

    int32_t img_id = TObjectImagery::FindImagery(kImageryPath);
    if (img_id < 0)
    {
        std::string p = kImageryPath;
        img_id = TObjectImagery::RegisterImagery(p.data());
    }
    if (img_id < 0)
    {
        log_error("[fizzle-shim] could not resolve '%s'", kImageryPath);
        delete st;
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[fizzle-shim] '%s' is not T3DImagery", kImageryPath);
        delete st;
        return nullptr;
    }
    st->imagery = img3d;

    // Resolve material + UV sub-rect for each of the 3 sub-objects.
    for (int32_t s = 0; s < kNumSystems; ++s)
    {
        const int32_t sub = kSystemSubObj[s];
        d3d::LoadMaterial(img3d, sub, st->diffuse[s], st->emissive[s]);

        const int32_t nv = img3d->NumObjVerts(sub);
        if (nv > 0)
        {
            std::vector<S3DVertex> vbuf(size_t(nv), S3DVertex{});
            img3d->GetObjVerts(sub, vbuf.data(), 0, 0,
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
            st->uv_rect[s][0] = minu;
            st->uv_rect[s][1] = minv;
            st->uv_rect[s][2] = maxu - minu;
            st->uv_rect[s][3] = maxv - minv;
        }
    }

    log_info("[fizzle-shim] spawned at (%d,%d,%d) systems=%d cap=%d",
             origin.x, origin.y, origin.z, kNumSystems, kDustCount);
    return st;
}

void Tick(State* st)    { if (st && st->alive) Animate(st); }
void Submit(State* st)  { if (st && st->alive) Render(st);  }
void Destroy(State* st) { delete st; }

} // namespace fizzle_shim
