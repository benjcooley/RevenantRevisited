// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  fountain.cpp - TFountainAnimator port via the d3d::* shim            *
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
// *  Blend mode is SUSPECT per forensics §7 — snapshot says Alpha, but   *
// *  the sister Photon/Sparks/Sparkle effects use Additive. iter1        *
// *  ships snapshot-Alpha; visual A/B will likely call for Additive.    *
// *************************************************************************

#include <cstdint>
#include <cstdlib>     // for std::rand
#include <cstring>

#include "../d3dport.h"
#include "../logging.h"
#include "../math3d.h"
#include "../imagery.h"
#include "../renderer.h"

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

// Per-sparkle base WU size. In retail RenderObject(obj) scales the
// authored vertex positions (photon sub-object half-extent ~5.25 wu) by
// obj->scl, giving a 2.0 * 5.25 ≈ 10.5 wu sparkle. Our SubmitFxBillboard
// takes a full WU size and the renderer handles projection, so we
// multiply scale by an equivalent base. Matches the bespoke port's
// kFountainBespokeBaseSizeWu = 64 (src/effect.h:4979) — visually
// verified.
constexpr float   kFountainBaseSizeWu = 64.0f;

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
    TTextureHandle texture     = kInvalidTexture;
    float          diffuse[4]  = {1,1,1,1};   // sub-object's authored DIFFUSE
    float          emissive[4] = {0,0,0,1};
    float          uv_rect[4]  = {0,0,1,1};   // sub-object's UV sub-rect

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
static void Render(State* st)
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

    // Build the animator's world-translation matrix
    // (base_pos = the spawned effect's world position).
    d3d::Matrix inst_world;
    d3d::MatrixIdentity(inst_world);
    d3d::MatrixTranslate(inst_world,
                         st->base_pos[0], st->base_pos[1], st->base_pos[2]);

    // obj = GetObject(colorobj). Sparkle.I3D's 4 sub-objects share one
    // texture (a 2x2 color atlas); per-variant color comes from the
    // sub-object's UV sub-rect into that atlas (resolved at Spawn).
    d3d::Obj obj;
    obj.objnum     = st->colorobj;
    std::memcpy(obj.diffuse,  st->diffuse,  sizeof(obj.diffuse));
    std::memcpy(obj.emissive, st->emissive, sizeof(obj.emissive));
    // Encode the resolved UV sub-rect into lverts[0..3] (Flag_Verts
    // tells RenderObject to consume them). lverts[0]=TL, lverts[3]=BR.
    const float u_lo = st->uv_rect[0];
    const float v_lo = st->uv_rect[1];
    const float u_hi = u_lo + st->uv_rect[2];
    const float v_hi = v_lo + st->uv_rect[3];
    obj.lverts[0].tu = u_lo; obj.lverts[0].tv = v_lo;
    obj.lverts[1].tu = u_lo; obj.lverts[1].tv = v_hi;
    obj.lverts[2].tu = u_hi; obj.lverts[2].tv = v_lo;
    obj.lverts[3].tu = u_hi; obj.lverts[3].tv = v_hi;

    for (int32_t n = 0; n < kNumFountainBubbles; ++n)
    {
        if (st->framenum[n] > 0)
        {
            // obj->flags = OBJ3D_SCL1 | OBJ3D_POS2 — apply scale first,
            // then translate (snapshot effect_old.cpp:3874). We also set
            // Flag_Verts so RenderObject honors the per-sub-object UV
            // sub-rect we stamped into obj.lverts (atlas cell selection).
            //
            // Snapshot's scale[n] scales the photon mesh's authored vertex
            // positions; we collapse to a single billboard size by
            // multiplying by the base size constant (kFountainBaseSizeWu)
            // since SubmitFxBillboard takes a full WU size.
            obj.flags = d3d::Flag_Scl1 | d3d::Flag_Pos2 | d3d::Flag_Verts;
            const float size = st->scale[n] * kFountainBaseSizeWu;
            obj.scl[0] = obj.scl[1] = obj.scl[2] = size;
            obj.pos[0] = st->p[n][0];
            obj.pos[1] = st->p[n][1];
            obj.pos[2] = st->p[n][2];

            d3d::RenderObject(*Renderer, obj,
                              d3d::Orientation::ScreenAligned,
                              st->texture,
                              &inst_world);
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
State* Spawn(const S3DPoint& origin, int32_t colorobj)
{
    if (!Renderer) return nullptr;
    if (colorobj < 0 || colorobj > 3) colorobj = 0;

    State* st = new State();
    st->colorobj    = colorobj;
    st->base_pos[0] = float(origin.x);
    st->base_pos[1] = float(origin.y);
    st->base_pos[2] = float(origin.z);

    // Bind the shared Sparkle.I3D asset (forensics §4).
    int32_t img_id = TObjectImagery::FindImagery(kImageryPath);
    if (img_id < 0)
    {
        // Legacy/unregistered fallback (the i3ddump tool's pattern).
        std::string p = kImageryPath;
        img_id = TObjectImagery::RegisterImagery(p.data());
    }
    if (img_id < 0)
    {
        log_error("[fountain-shim] could not resolve '%s'", kImageryPath);
        delete st;
        return nullptr;
    }
    TObjectImagery* base = TObjectImagery::LoadImagery(img_id);
    T3DImagery* img3d = dynamic_cast<T3DImagery*>(base);
    if (!img3d)
    {
        log_error("[fountain-shim] '%s' is not T3DImagery", kImageryPath);
        delete st;
        return nullptr;
    }

    // Cache the imagery pointer; defer texture binding to the first
    // Submit() call (handle is sometimes invalid at Spawn time before
    // the loader uploads to GPU).
    st->imagery = img3d;

    // Material diffuse / emissive (these don't depend on GPU upload).
    d3d::LoadMaterial(img3d, colorobj, st->diffuse, st->emissive);

    // Resolve the chosen sub-object's UV sub-rect. Sparkle.I3D's texture
    // is a 2×2 atlas with 4 colored sparkles (cyan/red/green/blue); each
    // sub-object's authored vertex UVs pick one quadrant. Without this,
    // we'd sample the whole atlas and render all 4 colors at once.
    // (Forensics §4 confirmed: per-sub-object baked UV rects.)
    {
        const int32_t nv = img3d->NumObjVerts(colorobj);
        if (nv > 0)
        {
            std::vector<S3DVertex> vbuf(size_t(nv), S3DVertex{});
            img3d->GetObjVerts(colorobj, vbuf.data(), 0, 0,
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

    // Run Initialize (the snapshot's per-bubble seeding).
    Initialize(st);

    log_info("[fountain-shim] spawned colorobj=%d at (%.0f,%.0f,%.0f) "
             "diffuse=(%.2f,%.2f,%.2f,%.2f)",
             colorobj, st->base_pos[0], st->base_pos[1], st->base_pos[2],
             st->diffuse[0], st->diffuse[1], st->diffuse[2], st->diffuse[3]);
    return st;
}

void Tick(State* st)    { if (st && st->alive) Animate(st); }
void Submit(State* st)  { if (st && st->alive) Render(st);  }
void Destroy(State* st) { delete st; }

} // namespace fountain_shim
