#include "goldpreview.h"

#include "../3dimage.h"
#include "../effect.h"
#include "../logging.h"
#include "../meshextract.h"
#include "../object.h"
#include "../renderer.h"
#include "../time.h"

#include <cmath>
#include <cstring>
#include <memory>
#include <vector>

namespace gold_authored_preview {
struct State
{
    std::unique_ptr<TObjectInstance> owner;
    TSafeRef<> identity;
    MeshHandle flare_mesh = 0;
    S3DMaterial flare_material{};
    double accumulated = 0;
    uint64_t ticks = 0;

    ~State()
    {
        if (Renderer && flare_mesh) Renderer->ReleaseMeshAssetRef(flare_mesh);
    }
};

State* Spawn(const S3DPoint& origin)
{
    if (!Renderer) return nullptr;
    auto context = std::make_unique<State>();
    TObjectClass* effect_class = TObjectClass::GetClass(OBJCLASS_EFFECT);
    const int32_t type = effect_class ? effect_class->FindObjType(0xd0c0f035u) : -1;
    if (type < 0) return nullptr;
    SObjectDef definition = {};
    definition.objclass = OBJCLASS_EFFECT;
    definition.objtype = short(type);
    definition.pos = origin;
    context->owner.reset(effect_class->NewObject(&definition));
    TObjectInstance* owner = context->owner.get();
    if (!owner || owner->ObjId() != 0xd0c0f035u || owner->GetMapIndex() <= 0 ||
        !dynamic_cast<TEffect*>(owner)) return nullptr;
    context->identity = owner;
    owner->OnScreen();
    auto* imagery = dynamic_cast<T3DImagery*>(owner->GetImagery());
    auto* animator = dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    uint32_t blend = 0;
    if (!imagery || !animator || !imagery->HasGoldPartSysProfile() ||
        animator->PartSysUnsupported() || animator->PartSysControllerCount() != 1 ||
        !animator->GoldBaseMeshBlend(1, blend) || blend != 80) return nullptr;

    std::vector<SMeshVertex> vertices;
    std::vector<uint16_t> indices;
    const TTextureHandle texture = imagery->GetTextureHandle(0);
    if (texture == kInvalidTexture || !ExtractSubMeshTextureSlot(imagery, 1, 1, vertices, indices))
        return nullptr;
    // Cache the actual authored flare, not a generated billboard. This key is
    // preview-specific; source imagery and runtime mesh ownership are unchanged.
    constexpr uint64_t key = 0x474f4c4450524556ull; // GOLDPREV
    context->flare_mesh = Renderer->RegisterMeshAsset(key, vertices.data(), int32_t(vertices.size()),
        indices.data(), int32_t(indices.size()), texture);
    if (!context->flare_mesh) return nullptr;
    Renderer->AddMeshAssetRef(context->flare_mesh);
    S3DObj object = {};
    S3DMat material = {};
    imagery->GetObject(1, &object);
    imagery->GetMaterial(object.material, &material);
    context->flare_material = material.matdesc;
    owner->Animate(false);
    log_info("[gold-preview] actual owner id=%08x map_index=%d generation=%u default-animator=1 controllers=1 origin=(%d,%d,%d)",
             owner->ObjId(), owner->GetMapIndex(), owner->SafeRefGen(), origin.x, origin.y, origin.z);
    return context.release();
}

void Advance(State* context, double seconds)
{
    if (!context || !context->owner || !std::isfinite(seconds) || seconds <= 0) return;
    context->accumulated += seconds;
    while (context->accumulated + 1e-10 >= TTime::LegacyFrameSeconds && context->owner)
    {
        context->accumulated -= TTime::LegacyFrameSeconds;
        TObjectInstance* owner = context->identity.Get();
        if (!owner || owner != context->owner.get()) return;
        // Same order as the real map path. A prior OF_KILL is reaped on the
        // following pulse rather than deleting from inside animator update.
        owner->NextFrame();
        if (owner->GetFlags() & OF_KILL)
        {
            log_info("[gold-preview] natural reap map_index=%d generation=%u tick=%llu frame=%d",
                     owner->GetMapIndex(), owner->SafeRefGen(),
                     static_cast<unsigned long long>(context->ticks), owner->GetFrame());
            context->owner.reset();
            return;
        }
        owner->Pulse(); // actual generic TEffect + actual default partsys Pulse
        ++context->ticks;
    }
}

void SubmitWorld(State* context, EFxDebugMode)
{
    if (!context || !Renderer || !context->owner) return;
    TObjectInstance* owner = context->identity.Get();
    if (!owner || owner != context->owner.get()) return;
    owner->Animate(false); // render pose refresh only; never advances NextFrame/Pulse
    auto* animator = dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    if (!animator || animator->PartSysUnsupported()) return;
    const hmm_vec3 render_scale = {1, 1, 1};
    animator->SubmitPartSys(*Renderer, 2, 2, render_scale);
    uint32_t blend = 0;
    S3DAnimObj* bone = animator->GetObject(1);
    if (!bone || !animator->GoldBaseMeshBlend(1, blend) || blend != 80) return;
    SHelperMeshSubmit mesh = {};
    mesh.mesh = context->flare_mesh;
    hmm_mat4 world;
    if (!animator->GoldBaseMeshWorldMatrix(world)) return;
    for (int row = 0; row < 4; ++row)
        for (int column = 0; column < 4; ++column)
            mesh.world[row * 4 + column] = world.Elements[column][row];
    const auto& material = context->flare_material;
    const auto copy = [](float* destination, const SRenderColor& color) {
        destination[0] = color.r; destination[1] = color.g;
        destination[2] = color.b; destination[3] = color.a;
    };
    copy(mesh.diffuse, material.diffuse); copy(mesh.ambient, material.ambient);
    copy(mesh.specular, material.specular); copy(mesh.emissive, material.emissive);
    mesh.power = material.power;
    mesh.additive_blend = true;
    mesh.retail_lighting = 1;
    mesh.retail_gold_no_depth = true;
    Renderer->SubmitGoldFlareAfterFx(mesh);
}

void Destroy(State* context) { delete context; }
}
