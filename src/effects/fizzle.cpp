// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  fizzle.cpp - TFizzleEffect port via the d3d:: shim                  *
// *                                                                       *
// *  Authored four-corner geometry submitted through SubmitFxQuad.      *
// *  Particle rotation precedes the X tip and static Z rotation,        *
// *  preserving the source matrix order and original vertex UVs.         *
// *                                                                       *
// *  Source of truth: docs/vfx/forensics/X21_TFizzleEffect.md            *
// *  Snapshot Animate: effect_old.cpp:12361-12487                        *
// *  Snapshot Render:  effect_old.cpp:12489-12502 (effect-side bracket)  *
// *                  + effectcomp.cpp:1070-1109 (per-system per-particle)*
// *  Snapshot Init:    effect_old.cpp:12350-12358 (3 systems w/ box01/02/03)*
// *                                                                       *
// *  Retail animator 0x4f4050 and particle helper 0x50c220 audited.       *
// *  Constants/order match; visual/runtime acceptance remains pending.  *
// *************************************************************************

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <memory>
#include <vector>

#include "../d3dport.h"
#include "../logging.h"
#include "../math3d.h"
#include "../imagery.h"
#include "../renderer.h"
#include "../time.h"
#include "../effect.h"

extern TRenderer* Renderer;

namespace fizzle_shim {

// =========================================================================
// Constants — every value cites forensics §3 / source line.
// Retail constants recovered directly from the executable; see forensics §2.1.
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
constexpr float   kTipRotX         = -float(M_PI) / 2.0f;  // effectcomp.cpp:1088 — tip authored XY quad upright

constexpr int32_t kNumSystems      = 3;       // blue / red / purple
constexpr const char* kImageryPath = "Magic\\Fizzle.I3D";

// Index↔label mapping per forensics §4:
//   sub-obj 0 = box01 = blue
//   sub-obj 1 = box02 = purple
//   sub-obj 2 = box03 = red (the "red" system draws box03)
// Note the snapshot's Init crosses indices: red.Init(GetObject(2)) +
// purple.Init(GetObject(1)). We just map system-index → sub-object-index
// in the original blue/red/purple traversal order:
constexpr int32_t kSystemSubObj[kNumSystems] = { 0, 2, 1 };  // blue, red, purple

// =========================================================================
// Per-particle state — matches snapshot SParticle + animator's temp/flicker.
// =========================================================================
struct Particle
{
    bool    used        = false;
    float   pos[3]      = {0, 0, 0};
    float   vel[3]      = {0, 0, 0};
    float   scl         = 0.0f;       // uniform (snapshot writes scl.x=y=z)
    float   rot_z_deg       = 0.0f;       // degrees, matching the snapshot accumulator
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
    ParticleSystem systems[kNumSystems];   // blue, red, purple
    float    base_pos[3] = {0, 0, 0};
    int32_t  frame_count = 0;
    float    add         = 0.0f;           // emission accumulator
    bool     alive       = true;
    double   sim_accum_seconds = 0.0;

    // Per-sub-object asset data (one set per system / sub-object).
    T3DImagery*    imagery     = nullptr;
    TTextureHandle texture[kNumSystems]   = {kInvalidTexture,
                                              kInvalidTexture,
                                              kInvalidTexture};
    float          diffuse[kNumSystems][4]  = {{1,1,1,1}, {1,1,1,1}, {1,1,1,1}};
    float          emissive[kNumSystems][4] = {{0,0,0,1}, {0,0,0,1}, {0,0,0,1}};
    S3DVertex      vertices[kNumSystems][4] = {};

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
    // Source constructs all parameters before choosing a destination system.
    // Keep even the overwritten rotation draw: it advances the source RNG.
    Particle particle;
    Particle* slot = &particle;
    slot->used        = true;
    slot->pos[0]      = float(snap_random(-kDustSpread, kDustSpread));   // :12385
    slot->pos[1]      = float(snap_random(-kDustSpread, kDustSpread));   // :12386
    slot->pos[2]      = float(snap_random(kSpawnZMin, kSpawnZMax));      // :12387
    slot->vel[0]      = 0.0f;                                             // :12390
    slot->vel[1]      = 0.0f;                                             // :12391
    slot->vel[2]      = -float(snap_random(kDustMinZVel, kDustMaxZVel))   // :12392
                        * 0.1f;                                            //   ×0.1
    slot->scl         = 0.0f;                                             // :12398 — start invisible
    (void)snap_random(0, 359);                                          // :12401 — overwritten draw
    slot->rot_z_deg       = 0.0f;                                             // :12402
    slot->life        = 0;
    slot->life_span   = kLifeSpanGrow;                                   // :12405
    slot->flicker     = (snap_random(0, 1) != 0);                        // :12408
    slot->spin_rate   = float(snap_random(-kDustRot, kDustRot));         // :12411 — deg/tick
    slot->max_scl     = float(snap_random(kDustMinScl, kDustMaxScl))      // :12413
                        * 0.01f;                                           //   ×0.01
    const int32_t sysidx = snap_random(1, kNumSystems) - 1;             // :12415
    for (int32_t i = 0; i < kDustCount; ++i)
    {
        if (!st->systems[sysidx].p[i].used)
        {
            st->systems[sysidx].p[i] = particle;
            return;
        }
    }
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
                // Retail 0x4f42a3..0x4f42c7 compares the x87 sum after
                // FST, before rounding it for the comparison. Two nearby
                // float operands sum exactly in double (e.g. .2f + .05f).
                const double grown_scale = double(p.scl) + double(kDustSclInc);
                p.scl = float(grown_scale);
                // :12451-52 — peak reached, flip to shrinking
                if (grown_scale > double(p.max_scl)) p.life_span = kLifeSpanShrink;
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

            // :12467-72 — accumulate degrees and wrap as in the source.
            p.rot_z_deg += p.spin_rate;
            while (p.rot_z_deg >= 360.0f) p.rot_z_deg -= 360.0f;
            while (p.rot_z_deg < 0.0f) p.rot_z_deg += 360.0f;

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
static bool BindTextures(State* st)
{
    // Lazy texture binding (gotcha #1) — defer per-sub-object texture
    // resolution until the I3D loader has uploaded to GPU. Re-tries
    // every Submit until all 3 are valid.
    if (st->imagery && st->imagery->NumTextures() > 0)
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
        if (st->texture[s] == kInvalidTexture) return false;
    return true;
}

static void Render(State* st, const TObjectInstance* owner = nullptr,
                   EFxDebugMode debug_mode = EFxDebugMode::Normal)
{
    if (!Renderer || !st->alive || (owner && (owner->Flags() & OF_KILL))) return;
    if (!BindTextures(st)) return;

    // :12492-93 — SaveBlendState + SetBlendState (Alpha).
    d3d::BlendStateGuard scope;
    d3d::SetBlendState();

    // Apply the source's complete matrix chain to the authored mesh.
    // The live Z spin precedes the X tip and cannot be combined with the
    // final static Z rotation into a WorldXY billboard angle.
    SQuadDrawItem item = {};
    item.key.blend = uint8_t(EFxBlend::Alpha);
    item.key.depth_mode = uint8_t(EFxDepthMode::TestNoWrite);
    item.light_mode = EFxLightMode::Unlit;
    item.debug_mode = debug_mode;
    for (int32_t s = 0; s < kNumSystems; ++s)
    {
        item.key.texture = st->texture[s];
        std::memcpy(item.color_rgba, st->diffuse[s], sizeof(item.color_rgba));
        for (int32_t i = 0; i < kDustCount; ++i)
        {
            const Particle& p = st->systems[s].p[i];
            if (!p.used || p.scl <= 0.0f) continue;
            hmm_mat4 local = {};
            MtxClear(&local);
            MtxRotateZ(&local, p.rot_z_deg * float(M_PI) / 180.0f);
            MtxRotateX(&local, kTipRotX);
            MtxRotateZ(&local, kStaticRotZ);
            const float size = p.scl * (p.flicker ? kFlickerScale : 1.0f);
            const hmm_vec3 scale = {size, size, size};
            MtxScale(&local, &scale);
            const hmm_vec3 pos = {p.pos[0], p.pos[1], p.pos[2]};
            MtxTranslate(&local, &pos);
            for (int32_t corner = 0; corner < 4; ++corner)
            {
                hmm_vec3 point = {};
                MtxTransform(&local, &st->vertices[s][corner].pos, &point);
                if (owner)
                {
                    hmm_vec3 world_point = {};
                    MtxTransform(&owner->Transform().Matrix(), &point, &world_point);
                    item.world_pos[corner][0] = world_point.X;
                    item.world_pos[corner][1] = world_point.Y;
                    item.world_pos[corner][2] = world_point.Z;
                }
                else
                {
                    // Keep the standalone reference preview's anchor unchanged.
                    item.world_pos[corner][0] = st->base_pos[0] + point.X;
                    item.world_pos[corner][1] = st->base_pos[1] + point.Y;
                    item.world_pos[corner][2] = st->base_pos[2] + point.Z;
                }
                item.uv[corner][0] = st->vertices[s][corner].tu;
                item.uv[corner][1] = st->vertices[s][corner].tv;
            }
            Renderer->SubmitFxQuad(item);
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
// Bind a borrowed owner asset without creating another map instance or
// advancing the RNG. The component is destroyed before its owner's imagery.
// =========================================================================
static State* BindImagery(T3DImagery* img3d, const S3DPoint& origin)
{
    if (!img3d || img3d->NumObjects() < kNumSystems) return nullptr;
    auto st = std::make_unique<State>();
    st->base_pos[0] = float(origin.x);
    st->base_pos[1] = float(origin.y);
    st->base_pos[2] = float(origin.z);

    st->imagery = img3d;

    // Resolve material and four authored corners for each sub-object.
    for (int32_t s = 0; s < kNumSystems; ++s)
    {
        const int32_t sub = kSystemSubObj[s];
        d3d::LoadMaterial(img3d, sub, st->diffuse[s], st->emissive[s]);

        if (img3d->NumObjVerts(sub) != 4)
        {
            log_error("[fizzle-shim] sub-object %d must have four authored corners", sub);
            return nullptr;
        }
        img3d->GetObjVerts(sub, st->vertices[s]);
    }

    log_info("[fizzle-shim] spawned at (%d,%d,%d) systems=%d cap=%d",
             origin.x, origin.y, origin.z, kNumSystems, kDustCount);
    return st.release();
}

State* Spawn(const S3DPoint& origin)
{
    if (!Renderer) return nullptr;
    int32_t img_id = TObjectImagery::FindImagery(kImageryPath);
    if (img_id < 0)
    {
        std::string path = kImageryPath;
        img_id = TObjectImagery::RegisterImagery(path.data());
    }
    if (img_id < 0)
    {
        log_error("[fizzle-shim] could not resolve '%s'", kImageryPath);
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    auto* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[fizzle-shim] '%s' is not T3DImagery", kImageryPath);
        if (base) TObjectImagery::FreeImagery(base);
        return nullptr;
    }
    State* state = BindImagery(img3d, origin);
    if (!state) TObjectImagery::FreeImagery(base);
    return state;
}

void Tick(State* st)
{
    if (!st || !st->alive) return;
    st->sim_accum_seconds += TTime::DeltaTime();
    while (st->sim_accum_seconds + 1e-12 >= TTime::LegacyFrameSeconds && st->alive)
    {
        st->sim_accum_seconds -= TTime::LegacyFrameSeconds;
        Animate(st);
    }
}
void Submit(State* st)  { if (st && st->alive) Render(st);  }
void Destroy(State* st) { delete st; }

namespace {
constexpr uint32_t kFizzleTypeId = 0xab8800ddu;

class TFizzleRuntimeComponent final : public TFlipbookBillboardComponent
{
  public:
    explicit TFizzleRuntimeComponent(State* state) : state_(state) {}
    [[nodiscard]] const char* ComponentName() const override { return "fizzle_reference"; }
    void Submit(TRenderer&, const TObjectInstance& owner) const override
    {
        Render(state_.get(), &owner, DebugMode());
    }
  protected:
    void OnUpdate() override
    {
        TObjectInstance* owner = Owner();
        if (!owner || (owner->Flags() & OF_KILL)) return;
        Tick(state_.get());
        if (!logged_tick_ && state_->frame_count > 0)
        {
            logged_tick_ = true;
            log_info("[fizzle-runtime] first simulation tick type='%s' id=%08x map_index=%d frame=%d",
                     owner->GetTypeName(), owner->ObjId(), owner->GetMapIndex(), state_->frame_count);
        }
        if (!state_->alive)
        {
            // The normal map/game tick reaps OF_KILL owners. Never delete
            // an owner while the global component update list is iterating.
            // The generic TEffect shell keeps its existing spell pointer.
            if (auto* effect = dynamic_cast<TEffect*>(owner))
                effect->KillThisEffect();
            else
                owner->SetFlags(OF_KILL | OF_PULSE);
            log_info("[fizzle-runtime] finished type='%s' id=%08x map_index=%d frame=%d marked_kill=1",
                     owner->GetTypeName(), owner->ObjId(), owner->GetMapIndex(), state_->frame_count);
        }
    }
  private:
    std::unique_ptr<State> state_;
    bool logged_tick_ = false;
};

class TFizzleRuntimeBuilder final : public T3DAnimatorBuilder
{
  public:
    TFizzleRuntimeBuilder() : T3DAnimatorBuilder("Fizzle") {}
    T3DAnimator* Build(TObjectInstance* owner) override { return new T3DAnimator(owner); }
    void AttachComponents(TObjectInstance* owner) override
    {
        if (!owner || owner->GetMapIndex() <= 0 || owner->ObjClass() != OBJCLASS_EFFECT ||
            owner->ObjId() != kFizzleTypeId || (owner->Flags() & OF_KILL) ||
            owner->GetComponent<TFizzleRuntimeComponent>())
            return;
        auto* imagery = dynamic_cast<T3DImagery*>(owner->GetImagery());
        auto state = std::unique_ptr<State>(BindImagery(imagery, owner->Pos()));
        if (!state || !BindTextures(state.get())) return; // retry after upload
        S3DTex texture = {};
        imagery->GetTexture(0, &texture);
        auto component = std::make_unique<TFizzleRuntimeComponent>(state.release());
        component->Configure(texture.htexture, int32_t(texture.desc.width),
                             int32_t(texture.desc.height), 1, 1, 1,
                             1.0f, 1.0f, false, true);
        owner->AddComponent(std::move(component));
        const S3DPoint position = owner->Pos();
        log_info("[fizzle-runtime] attached type='%s' id=%08x map_index=%d origin=(%d,%d,%d)",
                 owner->GetTypeName(), owner->ObjId(), owner->GetMapIndex(),
                 position.x, position.y, position.z);
    }
};

TFizzleRuntimeBuilder fizzle_runtime_builder;
} // namespace

} // namespace fizzle_shim
