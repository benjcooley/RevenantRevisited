// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *    retailblood.cpp - retail combat blood (TBloodAnimator 0x005acd18)  *
// *************************************************************************
//
// See retailblood.h for the overview and docs/gameplay/forensics/COMBAT_EFFECTS.md
// §2 for the transcription notes (stack layout, constants, the deliberate
// one-tick render ease).

#include "retailblood.h"

#include "../3dscene.h"
#include "../effect.h"
#include "../logging.h"
#include "../mappane.h"
#include "../math3d.h"
#include "../renderer.h"
#include "../revenant.h"
#include "../revutils.h"
#include "../time.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>

// *********************
// * TBloodAnimator    *
// *********************

namespace
{
const retail_blood::Random kGameRandom = [](int32_t low, int32_t high) { return random(low, high); };

// FLY droplets stand up to face the camera: RotateX(-pi/2) then
// RotateZ(-pi/4), the literal floats 0xbfc90fdb / 0xbf490fdb.
constexpr float kStandUp = -0x1.921fb6p+0f;
constexpr float kFaceCamera = -0x1.921fb6p-1f;
} // namespace

void TBloodAnimator::LoadQuad(int32_t object, SQuad& quad) const
{
    quad = {};
    T3DImagery* imagery = Get3DImagery();
    if (!imagery || object < 0 || object >= imagery->NumObjects() ||
        imagery->NumObjVerts(object) != 4 || imagery->NumObjFaces(object) != 2)
        return;
    S3DVertex vertices[4] = {};
    imagery->GetObjVerts(object, vertices);
    S3DFace faces[2] = {};
    int32_t starts[MAXTEXTURES + 1] = {}, counts[MAXTEXTURES + 1] = {};
    imagery->GetObjFaces(object, faces, starts, counts);
    for (int32_t slot = 1; slot <= MAXTEXTURES; ++slot)
        if (counts[slot] == 2)
            quad.texture = slot - 1;
    if (quad.texture < 0)
        return;
    // SQuadDrawItem draws (2,0,3) and (1,3,0): corners 0 and 3 must be the
    // diagonal the two authored faces share.
    const uint16_t a[3] = {faces[0].v1, faces[0].v2, faces[0].v3};
    const uint16_t b[3] = {faces[1].v1, faces[1].v2, faces[1].v3};
    int32_t shared[2] = {-1, -1}, n = 0;
    for (uint16_t v : a)
        if (std::find(std::begin(b), std::end(b), v) != std::end(b) && n < 2)
            shared[n++] = v;
    if (n != 2)
        return;
    const auto other = [&](const uint16_t (&face)[3]) {
        for (uint16_t v : face)
            if (v != shared[0] && v != shared[1]) return int32_t(v);
        return -1;
    };
    const int32_t order[4] = {shared[0], other(b), other(a), shared[1]};
    for (int32_t corner = 0; corner < 4; ++corner)
    {
        if (order[corner] < 0 || order[corner] > 3)
            return;
        const S3DVertex& vertex = vertices[order[corner]];
        quad.pos[corner][0] = vertex.pos.X;
        quad.pos[corner][1] = vertex.pos.Y;
        quad.pos[corner][2] = vertex.pos.Z;
        quad.uv[corner][0] = vertex.tu;
        quad.uv[corner][1] = vertex.tv;
    }
    quad.valid = true;
}

// REVSYNC: TBloodAnimator::Initialize @ 0x004f18d0 -- the spray from the
// effect's parameters (GetParams, vtable +0x204) and the violence level.
void TBloodAnimator::Initialize()
{
    T3DAnimator::Initialize();
    retail_blood::SParams params;
    if (auto* effect = dynamic_cast<TBloodEffect*>(inst))
    {
        int32_t count = 0;   // stored at +0x198; the animator never reads it
        effect->GetParams(&params.height, &params.hangle, &params.vangle,
                          &params.hspread, &params.vspread, &count);
    }
    spray_height = params.height;
    // Retail +0x534..+0x550: GetObject(4..7) the masks, (0..3) the colours.
    for (int32_t size = 0; size < 4; ++size)
    {
        LoadQuad(4 + size, mask[size_t(size)]);
        LoadQuad(size, color[size_t(size)]);
    }
    spray.Initialize(params, ViolenceLevel, kGameRandom);
    previous = spray.Drops();
    initialized = true;
}

// REVSYNC: TBloodAnimator::Pulse @ 0x004f1c60 -- T3DAnimator::Pulse, one
// spray tick, and OF_KILL once nothing moved (0x004f1db9).
void TBloodAnimator::Pulse()
{
    T3DAnimator::Pulse();
    if (!initialized)
        return;
    previous = spray.Drops();
    if (spray.Step(ViolenceLevel, kGameRandom))
        inst->SetFlags(inst->GetFlags() | OF_KILL);
}

// REVSYNC: TBloodAnimator::Render @ 0x004f1de0 -- per droplet, a mask in
// blend mode 2 (alpha) and the colour in mode 8 (add), both depth-tested
// without writes, prelit by the packed light colour.
//
// REVSYNC-DIVERGENCE: retail drew the tick's state; the port eases each
// droplet from its previous tick's position and size within a stage, so the
// spray moves smoothly at any frame rate (it lags the tick by at most one).
void TBloodAnimator::Submit(TRenderer& renderer, EFxDebugMode debug_mode) const
{
    if (!initialized || !inst)
        return;
    T3DImagery* imagery = Get3DImagery();
    if (!imagery)
        return;
    const float ease = float(TTime::LegacyFrameFraction());
    const hmm_mat4& owner = inst->Transform().Matrix();
    const S3DPoint effect = IntPos(inst);
    const auto& drops = spray.Drops();
    for (size_t i = 0; i < drops.size(); ++i)
    {
        const retail_blood::SDrop& drop = drops[i];
        if (!drop.used)
            continue;
        const retail_blood::SDrop& was = previous[i];
        const bool continuous = was.used && was.stage == drop.stage;
        const auto blend = [&](float from, float to) { return continuous ? from + (to - from) * ease : to; };

        // 0x004f1e93..0x004f1f61: the light at the droplet, averaged over
        // the lights found (twice the area ambient to start with).
        float light = float(2 * MapPane.GetAmbientLight());
        const int32_t x = int32_t(drop.pos[0]) + effect.x;
        const int32_t y = int32_t(drop.pos[1]) + effect.y;
        const int32_t z = int32_t(drop.pos[2]) + effect.z;
        int32_t lights[3] = {-1, -1, -1};
        Scene3D.GetClosestLights(x, y, z, lights[0], lights[1], lights[2]);
        int32_t found = 0;
        while (found < MaxLights && found < 3 && lights[found] >= 0)
        {
            float brightness = 0.0f;
            Scene3D.GetLightBrightness(lights[found], x, y, z, brightness);
            light = float(double(brightness) + double(light));
            ++found;
        }
        light = float(double(light) / double((std::max)(found, 1)));
        const uint32_t argb = retail_blood::PackedVertexColor(light);

        hmm_mat4 local;
        MtxClear(&local);
        const float scale = blend(was.scale, drop.scale);
        const hmm_vec3 scaling = {scale, scale, scale};
        MtxScale(&local, &scaling);
        if (drop.stage == retail_blood::StageFly)
        {
            MtxRotateX(&local, kStandUp);
            MtxRotateZ(&local, kFaceCamera);
        }
        const hmm_vec3 offset = {blend(was.pos[0], drop.pos[0]), blend(was.pos[1], drop.pos[1]),
                                 blend(was.pos[2], drop.pos[2]) - float(spray_height)};
        MtxTranslate(&local, &offset);
        hmm_mat4 world;
        MtxMultiply(&world, &local, &owner);

        const size_t size = (std::min)(size_t(drop.size), size_t(3));
        for (int32_t pass = 0; pass < 2; ++pass)
        {
            const SQuad& quad = pass == 0 ? mask[size] : color[size];
            if (!quad.valid)
                continue;
            SQuadDrawItem item = {};
            item.key.texture = imagery->GetTextureHandle(quad.texture);
            if (item.key.texture == kInvalidTexture)
                continue;
            item.key.blend = uint8_t(pass == 0 ? EFxBlend::Alpha : EFxBlend::AdditiveStraight);
            item.key.depth_mode = uint8_t(EFxDepthMode::TestNoWrite);
            item.light_mode = EFxLightMode::Unlit;
            item.debug_mode = debug_mode;
            item.color_rgba[0] = float((argb >> 16) & 0xff) / 255.0f;
            item.color_rgba[1] = float((argb >> 8) & 0xff) / 255.0f;
            item.color_rgba[2] = float(argb & 0xff) / 255.0f;
            item.color_rgba[3] = float(argb >> 24) / 255.0f;
            for (int32_t corner = 0; corner < 4; ++corner)
            {
                const hmm_vec3 vertex = {quad.pos[corner][0], quad.pos[corner][1], quad.pos[corner][2]};
                hmm_vec3 placed = {};
                MtxTransform(&world, &vertex, &placed);
                item.world_pos[corner][0] = placed.X;
                item.world_pos[corner][1] = placed.Y;
                item.world_pos[corner][2] = placed.Z;
                item.uv[corner][0] = quad.uv[corner][0];
                item.uv[corner][1] = quad.uv[corner][1];
            }
            renderer.SubmitFxQuad(item);
        }
    }
}

// *****************************************
// * Registration: effect, animator, visual *
// *****************************************

namespace
{
// Draws the droplets in place of the Blood.I3D boxes (which never show as
// static meshes). The map's FX walk calls Submit once a frame.
class TBloodVisualComponent final : public TFlipbookBillboardComponent
{
  public:
    [[nodiscard]] const char* ComponentName() const override { return "retail_blood"; }
    void Submit(TRenderer& renderer, const TObjectInstance& owner) const override
    {
        if (auto* animator = dynamic_cast<const TBloodAnimator*>(owner.GetAnimator()))
            animator->Submit(renderer, DebugMode());
    }
};

class TBloodAnimatorBuilder final : public T3DAnimatorBuilder
{
  public:
    TBloodAnimatorBuilder() : T3DAnimatorBuilder("Blood") {}
    T3DAnimator* Build(TObjectInstance* oi) override { return new TBloodAnimator(oi); }
    void AttachComponents(TObjectInstance* oi) override
    {
        if (!oi || oi->GetComponent<TFlipbookBillboardComponent>())
            return;
        auto* imagery = dynamic_cast<T3DImagery*>(oi->GetImagery());
        // The map walk retries until the imagery's textures are resident.
        const TTextureHandle texture = imagery ? imagery->GetTextureHandle(0) : kInvalidTexture;
        if (texture == kInvalidTexture)
            return;
        auto visual = std::make_unique<TBloodVisualComponent>();
        visual->Configure(texture, 128, 128, 1, 1, 1, 0.0f, 0.0f, false, true);
        oi->AddComponent(std::move(visual));
    }
};
} // namespace

// REVSYNC: the "Blood" effect builder 0x004f1870 (TBloodEffect, 0x19c bytes,
// vtable 0x005acb0c) and animator builder 0x004f18b0 (vtable 0x005acd14).
DEFINE_BUILDER("Blood", TBloodEffect)
REGISTER_BUILDER(TBloodEffect)
static TBloodAnimatorBuilder g_blood_animator_builder;
