// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  fountain.cpp - TFountainAnimator reference with authored quads      *
// *                                                                       *
// *  SHIM VALIDATION PORT — proves the d3d::* shim is rich enough that a *
// *  transcription of the snapshot Initialize/Animate/Render bodies      *
// *  produces a working render, with no convention-translation work       *
// *  required by the porter.                                              *
// *                                                                       *
// *  Source of truth: docs/vfx/forensics/FOUNTAIN_TFountainAnimator.md   *
// *  (973 lines, 94 cites). Snapshot Render body at effect_old.cpp:3861. *
// *                                                                       *
// *  Retail fidelity: retail-partial. Asset MD5 byte-identical between   *
// *  snapshot and shipped (md5 8211199ce94bb64f4f0b52b6ed94c4d8 — see    *
// *  forensics §2.1 + §4); registration names (CYANFONT/REDFONT/...)     *
// *  confirmed via retail builder thunks. Per-tick constants are         *
// *  snapshot-only (no decompiled retail Render body).                   *
// *                                                                       *
// *  Blend remains snapshot Alpha. Native visual verification pending. *
// *************************************************************************

#include <cstdint>
#include <cstdlib>     // for std::rand
#include <cstring>
#include <memory>

#include "../d3dport.h"
#include "../effect.h"
#include "../logging.h"
#include "../math3d.h"
#include "../imagery.h"
#include "../renderer.h"
#include "../time.h"

extern TRenderer* Renderer;

namespace fountain_shim {

// --------------------------------------------------------------------------
// Constants — every value here cites a snapshot line (forensics §3).
// --------------------------------------------------------------------------
constexpr int32_t kNumFountainBubbles = 10;       // src/effect.h:758 — NUM_FOUNTAIN_BUBBLES
constexpr int32_t kFountainRadius     = 20;       // src/effect.h:760 — FOUNTAIN_RADIUS (wu)
constexpr float   kFountainScaleStep  = 0.15f;    // src/effect.h:759 — FOUNTAIN_SCALE_STEP
constexpr float   kInitialBubbleScale = 2.0f;     // effect_old.cpp:3808, 3847
constexpr const char* kImageryPath    = "Misc\\Sparkle.I3D";

constexpr double  kTickSeconds = 1.0 / 24.0;

// Inclusive range random — matches the snapshot's random(lo, hi) semantics
// (lo and hi both inclusive; not <hi exclusive).
static int32_t snap_random(int32_t lo, int32_t hi)
{
    if (hi <= lo) return lo;
    return lo + (std::rand() % (hi - lo + 1));
}

// --------------------------------------------------------------------------
// Per-effect state. One per spawned Fountain instance.
// --------------------------------------------------------------------------
struct State
{
    ~State()
    {
        if (owns_imagery && imagery)
            TObjectImagery::FreeImagery(imagery);
    }
    int32_t  colorobj      = 0;          // 0/1/2/3 — sub-object selector
    float    base_pos[3]   = {0,0,0};    // effect world position (animator.pos)
    bool     alive         = true;

    // Per-bubble particle state (snapshot's `p[N]`, `scale[N]`, `rise[N]`,
    // `framenum[N]` arrays, declared at src/effect.h:761-764).
    float    p[kNumFountainBubbles][3]   = {};
    float    scale[kNumFountainBubbles]  = {};
    float    rise[kNumFountainBubbles]   = {};
    int32_t  framenum[kNumFountainBubbles] = {};

    // Resolved asset data. Texture handle is bound lazily on first Submit
    // since at Spawn time the I3D loader hasn't always uploaded the
    // texture to the GPU yet.
    T3DImagery*    imagery     = nullptr;
    bool          owns_imagery = false;       // standalone LoadImagery reference only
    TTextureHandle texture     = kInvalidTexture;
    float          diffuse[4]  = {1,1,1,1};   // sub-object's authored DIFFUSE
    S3DVertex      vertices[4] = {};          // authored corners and UVs
    double         sim_accum_seconds = 0.0;
    uint64_t       sim_ticks = 0;

    // Diagnostic info
    int32_t spawn_log_done = 0;
};

// --------------------------------------------------------------------------
// Initialize — transcribed from snapshot effect_old.cpp:3797-3813.
//
// for n in 0..NUM_FOUNTAIN_BUBBLES-1:
//     p[n].x = random(-FOUNTAIN_RADIUS, FOUNTAIN_RADIUS)
//     p[n].y = random(-FOUNTAIN_RADIUS, FOUNTAIN_RADIUS)
//     p[n].z = 0.0
//     rise[n]  = random(1,3) / 2.0
//     scale[n] = 2.0
//     framenum[n] = random(-NUM_FOUNTAIN_BUBBLES/2, 0)
// SetColorObject()   // virtual; leaf sets colorobj
// --------------------------------------------------------------------------
static void Initialize(State* st)
{
    for (int32_t n = 0; n < kNumFountainBubbles; ++n)
    {
        st->p[n][0]   = float(snap_random(-kFountainRadius, kFountainRadius));
        st->p[n][1]   = float(snap_random(-kFountainRadius, kFountainRadius));
        st->p[n][2]   = 0.0f;
        st->rise[n]   = float(snap_random(1, 3)) / 2.0f;     // {0.5, 1.0, 1.5}
        st->scale[n]  = kInitialBubbleScale;                  // 2.0
        st->framenum[n] = snap_random(-kNumFountainBubbles / 2, 0);  // -5..0
    }
    // colorobj is set at spawn-time by the caller (the per-variant
    // selector lives outside this transcription, mirroring the leaf class
    // SetColorObject() pattern — see effect.h:805/820/835/850).
}

// --------------------------------------------------------------------------
// Animate — transcribed from snapshot effect_old.cpp:3822-3852.
//
// for n in 0..NUM_FOUNTAIN_BUBBLES-1:
//     framenum[n]++
//     if framenum[n] > 0:
//         p[n].z   += rise[n]
//         scale[n] -= FOUNTAIN_SCALE_STEP
//         if scale[n] <= 0:
//             p[n].x = random(-FOUNTAIN_RADIUS, FOUNTAIN_RADIUS)
//             p[n].y = random(-FOUNTAIN_RADIUS, FOUNTAIN_RADIUS)
//             p[n].z = 0.0
//             rise[n]  = random(1,3) / 2.0
//             scale[n] = 2.0
//             framenum[n] = random(-NUM_FOUNTAIN_BUBBLES/2, 0)
// --------------------------------------------------------------------------
static void Animate(State* st)
{
    for (int32_t n = 0; n < kNumFountainBubbles; ++n)
    {
        st->framenum[n]++;
        if (st->framenum[n] > 0)
        {
            st->p[n][2]  += st->rise[n];
            st->scale[n] -= kFountainScaleStep;
            if (st->scale[n] <= 0.0f)
            {
                st->p[n][0]   = float(snap_random(-kFountainRadius, kFountainRadius));
                st->p[n][1]   = float(snap_random(-kFountainRadius, kFountainRadius));
                st->p[n][2]   = 0.0f;
                st->rise[n]   = float(snap_random(1, 3)) / 2.0f;
                st->scale[n]  = kInitialBubbleScale;
                st->framenum[n] = snap_random(-kNumFountainBubbles / 2, 0);
            }
        }
    }
}

// --------------------------------------------------------------------------
// Render — transcribed from snapshot effect_old.cpp:3861-3887.
//
// Render():
//     SaveBlendState()
//     SetBlendState()                       // Alpha
//     obj = GetObject(colorobj)
//     for n in 0..NUM_FOUNTAIN_BUBBLES-1:
//         if framenum[n] > 0:
//             ResetExtents()
//             obj->flags = OBJ3D_SCL1 | OBJ3D_POS2
//             obj->scl.x = obj->scl.y = obj->scl.z = scale[n]
//             obj->pos = p[n]
//             RenderObject(obj)
//             UpdateExtents()
//     RestoreBlendState()
// --------------------------------------------------------------------------
static void Render(State* st, const TObjectInstance* owner = nullptr,
                   EFxDebugMode debug_mode = EFxDebugMode::Normal)
{
    if (!Renderer || !st->alive) return;

    // Lazy texture binding: at Spawn time the I3D loader hasn't always
    // uploaded the texture to GPU yet. Retry every Submit until we get
    // a valid handle. (The bespoke port avoids this because it goes
    // through TEffect::ActivateComponents which triggers upload.)
    if (st->texture == kInvalidTexture && st->imagery)
    {
        if (st->imagery->NumTextures() > 0)
        {
            S3DTex tex = {};
            st->imagery->GetTexture(0, &tex);
            st->texture = tex.htexture;
        }
        if (st->texture == kInvalidTexture) return;   // wait another tick
    }
    if (st->texture == kInvalidTexture) return;

    // SaveBlendState + RestoreBlendState pattern (scoped via RAII).
    d3d::BlendStateGuard blend_scope;
    d3d::SetBlendState();    // Alpha — snapshot effect_old.cpp:3864

    // Match the effect-owner transform used by the mesh and bespoke paths.
    hmm_mat4 world = {};
    if (owner)
        world = owner->Transform().Matrix();
    else
    {
        MtxClear(&world);
        const hmm_vec3 root_scale = {1.0f, 1.0f, WORLD3D_Z_SCALE};
        MtxScale(&world, &root_scale);
        const hmm_vec3 origin = {st->base_pos[0], st->base_pos[1], st->base_pos[2]};
        MtxTranslate(&world, &origin);
    }
    SQuadDrawItem item = {};
    std::memcpy(item.color_rgba, st->diffuse, sizeof(item.color_rgba));
    item.key.texture = st->texture;
    item.key.blend = uint8_t(EFxBlend::Alpha);
    item.key.depth_mode = uint8_t(EFxDepthMode::TestNoWrite);
    item.light_mode = EFxLightMode::Unlit;
    item.debug_mode = debug_mode;

    for (int32_t n = 0; n < kNumFountainBubbles; ++n)
    {
        if (st->framenum[n] > 0)
        {
            const hmm_vec3 position = {st->p[n][0], st->p[n][1], st->p[n][2]};
            for (int32_t vertex = 0; vertex < 4; ++vertex)
            {
                const S3DVertex& authored = st->vertices[vertex];
                const hmm_vec3 local = authored.pos * st->scale[n] + position;
                hmm_vec3 corner = {};
                MtxTransform(&world, &local, &corner);
                item.world_pos[vertex][0] = corner.X;
                item.world_pos[vertex][1] = corner.Y;
                item.world_pos[vertex][2] = corner.Z;
                item.uv[vertex][0] = authored.tu;
                item.uv[vertex][1] = authored.tv;
            }
            Renderer->SubmitFxQuad(item);
        }
    }

    if (!st->spawn_log_done)
    {
        log_info("[fountain-shim] first render: %d bubbles, colorobj=%d, "
                 "diffuse=(%.2f,%.2f,%.2f,%.2f) tex=%u base=(%.0f,%.0f,%.0f)",
                 kNumFountainBubbles, st->colorobj,
                 st->diffuse[0], st->diffuse[1], st->diffuse[2], st->diffuse[3],
                 st->texture, st->base_pos[0], st->base_pos[1], st->base_pos[2]);
        st->spawn_log_done = 1;
    }
}

// --------------------------------------------------------------------------
// Spawn — load the asset, populate state, run Initialize.
// --------------------------------------------------------------------------
// The runtime owner already owns its loaded imagery. Bind the same asset
// records as the standalone shim without creating a second map instance or
// taking an extra imagery reference. The caller owns the returned state.
static State* BindImagery(T3DImagery* img3d, const S3DPoint& origin, int32_t colorobj)
{
    if (!img3d || colorobj < 0 || colorobj > 3)
    {
        log_error("[fountain-shim] invalid imagery or color sub-object %d", colorobj);
        return nullptr;
    }
    auto st = std::make_unique<State>();
    st->colorobj = colorobj;
    st->base_pos[0] = float(origin.x);
    st->base_pos[1] = float(origin.y);
    st->base_pos[2] = float(origin.z);
    st->imagery = img3d;

    if (colorobj >= img3d->NumObjects() || img3d->NumObjVerts(colorobj) != 4)
    {
        log_error("[fountain-shim] sub-object %d requires an authored four-vertex quad", colorobj);
        return nullptr;
    }
    img3d->GetObjVerts(colorobj, st->vertices, 0, 0, ERender3DVertex::Vertex);
    S3DObj object = {};
    img3d->GetObject(colorobj, &object);
    if (object.material >= 0 && object.material < img3d->NumMaterials())
    {
        S3DMat material = {};
        img3d->GetMaterial(object.material, &material);
        const auto& color = material.matdesc.diffuse;
        st->diffuse[0] = color.r;
        st->diffuse[1] = color.g;
        st->diffuse[2] = color.b;
        st->diffuse[3] = color.a;
    }

    // Run Initialize (the snapshot's per-bubble seeding).
    Initialize(st.get());

    log_info("[fountain-shim] spawned colorobj=%d at (%.0f,%.0f,%.0f) "
             "diffuse=(%.2f,%.2f,%.2f,%.2f)",
             colorobj, st->base_pos[0], st->base_pos[1], st->base_pos[2],
             st->diffuse[0], st->diffuse[1], st->diffuse[2], st->diffuse[3]);
    return st.release();
}

State* Spawn(const S3DPoint& origin, int32_t colorobj)
{
    if (!Renderer) return nullptr;
    if (colorobj < 0 || colorobj > 3) colorobj = 0;
    int32_t img_id = TObjectImagery::FindImagery(kImageryPath);
    if (img_id < 0)
    {
        std::string path = kImageryPath;
        img_id = TObjectImagery::RegisterImagery(path.data());
    }
    if (img_id < 0)
    {
        log_error("[fountain-shim] could not resolve '%s'", kImageryPath);
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    auto* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[fountain-shim] '%s' is not T3DImagery", kImageryPath);
        if (base) TObjectImagery::FreeImagery(base);
        return nullptr;
    }
    State* state = BindImagery(img3d, origin, colorobj);
    if (!state) TObjectImagery::FreeImagery(base);
    else state->owns_imagery = true;
    return state;
}

void Tick(State* st)
{
    if (!st || !st->alive) return;
    st->sim_accum_seconds += TTime::DeltaTime();
    while (st->sim_accum_seconds >= kTickSeconds)
    {
        st->sim_accum_seconds -= kTickSeconds;
        Animate(st);
        ++st->sim_ticks;
    }
}
void Submit(State* st)  { if (st && st->alive) Render(st);  }
void Destroy(State* st) { delete st; }

// Real map/runtime dispatch. Four generic EFFECT types share this state
// implementation; the retail type ID fixes their authored sub-object.
// Attachment runs only after the actual owner's final map identity exists.
namespace {
class TFountainRuntimeComponent final : public TFlipbookBillboardComponent
{
  public:
    explicit TFountainRuntimeComponent(State* state) : state_(state) {}
    [[nodiscard]] const char* ComponentName() const override { return "fountain_reference"; }
    void Submit(TRenderer&, const TObjectInstance& owner) const override
    {
        Render(state_.get(), &owner, DebugMode());
    }
  protected:
    void OnUpdate() override
    {
        Tick(state_.get());
        if (!logged_tick_ && state_->sim_ticks > 0 && Owner())
        {
            logged_tick_ = true;
            log_info("[fountain-runtime] first simulation tick type='%s' id=%08x map_index=%d "
                     "colorobj=%d ticks=%llu",
                     Owner()->GetTypeName(), Owner()->ObjId(), Owner()->GetMapIndex(),
                     state_->colorobj, static_cast<unsigned long long>(state_->sim_ticks));
        }
    }
  private:
    // Imagery is borrowed from Owner; TObjectInstance destroys components
    // before freeing that imagery. State has no independently owned asset.
    std::unique_ptr<State> state_;
    bool logged_tick_ = false;
};

class TFountainRuntimeBuilder final : public T3DAnimatorBuilder
{
  public:
    TFountainRuntimeBuilder(const char* name, uint32_t type_id, int32_t colorobj)
        : T3DAnimatorBuilder(name), type_id_(type_id), colorobj_(colorobj) {}
    T3DAnimator* Build(TObjectInstance* owner) override { return new T3DAnimator(owner); }
    void AttachComponents(TObjectInstance* owner) override
    {
        if (!owner || owner->GetMapIndex() < 0 || owner->ObjClass() != OBJCLASS_EFFECT ||
            owner->ObjId() != type_id_ || owner->GetComponent<TFountainRuntimeComponent>())
            return;
        auto* imagery = dynamic_cast<T3DImagery*>(owner->GetImagery());
        S3DTex texture = {};
        if (!imagery || imagery->NumTextures() <= 0) return;
        imagery->GetTexture(0, &texture);
        if (texture.htexture == kInvalidTexture) return; // lazy hook retries
        // Do not seed a discarded particle state on a texture-upload retry.
        auto state = std::unique_ptr<State>(BindImagery(imagery, owner->Pos(), colorobj_));
        if (!state) return;
        state->texture = texture.htexture;
        auto component = std::make_unique<TFountainRuntimeComponent>(state.release());
        component->Configure(texture.htexture, int32_t(texture.desc.width),
                             int32_t(texture.desc.height), 1, 1, 1,
                             1.0f, 1.0f, false, true);
        owner->AddComponent(std::move(component));
        const S3DPoint position = owner->Pos();
        log_info("[fountain-runtime] attached type='%s' id=%08x map_index=%d "
                 "colorobj=%d texture=%u origin=(%d,%d,%d)",
                 owner->GetTypeName(), owner->ObjId(), owner->GetMapIndex(), colorobj_,
                 texture.htexture, position.x, position.y, position.z);
    }
  private:
    uint32_t type_id_;
    int32_t colorobj_;
};

TFountainRuntimeBuilder cyan_runtime_builder("CyanFont", 0x22491405u, 0);
TFountainRuntimeBuilder red_runtime_builder("RedFont", 0x335a2516u, 1);
TFountainRuntimeBuilder green_runtime_builder("GreenFont", 0x446b3627u, 2);
TFountainRuntimeBuilder blue_runtime_builder("BlueFont", 0x557c4738u, 3);
} // namespace

} // namespace fountain_shim
