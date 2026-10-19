#include "game/effects/legacy/combatflashpreview.h"

#include "game/imagery/3dimage.h"
#include "game/effects/combatflashprofile.h"
#include "game/effects/effect.h"
#include "engine/core/logging.h"
#include "game/imagery/meshextract.h"
#include "game/object/object.h"
#include "engine/render/renderer.h"
#include "engine/render/retailsoftwaretransform.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <memory>
#include <vector>

namespace combatflash_preview {
namespace profile = retail_combatflash_profile;

struct State
{
    struct Flare
    {
        int32_t object = -1;
        MeshHandle mesh = 0;
        S3DMaterial material{};
    };
    TObjectPtr<TObjectInstance> owner;
    TSafeRef<> identity;
    int32_t state = 0;
    std::vector<Flare> flares;   // every authored '#impact' flare with geometry
    double accumulated = 0;
    uint64_t ticks = 0;
    bool show_particles=true, show_base=true;

    ~State()
    {
        if (Renderer)
            for (const Flare& flare : flares)
                if (flare.mesh) Renderer->ReleaseMeshAssetRef(flare.mesh);
    }
};

State* Spawn(const S3DPoint& origin, int start, bool show_particles, bool show_base)
{
    if (!Renderer || start < 1 || start > 9) return nullptr;
    auto context = std::make_unique<State>();
    context->state = 2 * (start - 1);
    TObjectClass* effect_class = TObjectClass::GetClass(OBJCLASS_EFFECT);
    const int32_t type = effect_class ? effect_class->FindObjType(profile::type_id) : -1;
    if (type < 0) return nullptr;
    SObjectState definition = {};
    definition.objclass = OBJCLASS_EFFECT;
    definition.objtype = short(type);
    definition.pos = origin;
    context->owner.reset(effect_class->NewObject(&definition));
    TObjectInstance* owner = context->owner.get();
    if (!owner || owner->ObjId() != profile::type_id || owner->GetMapIndex() <= 0 ||
        !dynamic_cast<TEffect*>(owner)) return nullptr;
    // EffectCombatFlash's SetState (vtable +0x18) before the first animate.
    owner->SetState(context->state);
    if (owner->GetState() != context->state) return nullptr;
    context->identity = owner;
    context->show_particles=show_particles; context->show_base=show_base;
    owner->OnScreen();
    auto* imagery = dynamic_cast<T3DImagery*>(owner->GetImagery());
    auto* animator = dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    if (!imagery || !animator || !imagery->HasCombatFlashPartSysProfile() ||
        animator->PartSysUnsupported() || animator->PartSysControllerCount() != 1)
    {
        log_warn("[combatflash-preview] start%d: owner lacks the admitted profile or its single controller", start);
        return nullptr;
    }

    // Cache the actual authored flares, not generated billboards. The keys
    // are preview-specific; source imagery and runtime mesh ownership are
    // unchanged.
    for (int32_t object = 0; object < imagery->NumObjects(); ++object)
    {
        const char* name = imagery->GetObjectName(object);
        if (!name || std::strncmp(name, "#impact", 7)) continue;
        // The face table decides what draws (the map path walks it the same
        // way): #impact07 and #impact09 keep a material but no textured faces.
        const int32_t faces = imagery->NumObjFaces(object);
        std::vector<S3DFace> face_list(size_t((std::max)(faces, 0)));
        int32_t starts[MAXTEXTURES + 1] = {}, counts[MAXTEXTURES + 1] = {};
        if (faces > 0)
            imagery->GetObjFaces(object, face_list.data(), starts, counts);
        int32_t slot = 0;
        for (int32_t t = 1; t <= imagery->NumTextures(); ++t)
            if (counts[t] > 0) slot = t;
        if (slot == 0) continue;
        S3DObj authored = {};
        S3DMat material = {};
        imagery->GetObject(object, &authored);
        imagery->GetMaterial(authored.material, &material);
        const TTextureHandle texture = imagery->GetTextureHandle(slot - 1);
        std::vector<SMeshVertex> vertices;
        std::vector<uint16_t> indices;
        if (texture == kInvalidTexture || !ExtractSubMeshTextureSlot(imagery, object, slot, vertices, indices))
        {
            log_warn("[combatflash-preview] start%d: flare %s (slot %d) did not extract", start, name, slot);
            return nullptr;
        }
        // Original 56e3e7..56e4f5 truncates UV*64 before its normal RGB565
        // raster: the native reference draws its own mesh so.
        if (StartupVfxNativeDomain)
            for (auto& vertex : vertices)
                for (float& uv : vertex.uv) uv = std::trunc(uv * 64.f) / 64.f;
        State::Flare flare;
        flare.object = object;
        flare.material = material.matdesc;
        const uint64_t key = (StartupVfxNativeDomain ? 0x4346535052455700ull : 0x4346535052455600ull) |
                             uint64_t(object);   // "CFSPREV"/"CFSPREW" + object
        flare.mesh = Renderer->RegisterMeshAsset(key, vertices.data(), int32_t(vertices.size()),
            indices.data(), int32_t(indices.size()), texture);
        if (!flare.mesh) return nullptr;
        Renderer->AddMeshAssetRef(flare.mesh);
        context->flares.push_back(flare);
    }
    owner->Animate(false);
    log_info("[native-domain] enabled=%d camera_z=0 owner=CombatFlash_START%d_POS2", int(StartupVfxNativeDomain), start);
    log_info("[combatflash-preview] actual owner id=%08x map_index=%d generation=%u default-animator=1 state=%d (start%d) flares=%zu origin=(%d,%d,%d)",
             owner->ObjId(), owner->GetMapIndex(), owner->SafeRefGen(), context->state, start,
             context->flares.size(), origin.x, origin.y, origin.z);
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
        if (!owner || owner != context->owner.get() || owner->GetState() != context->state) return;
        // Same order as the real map path. A prior OF_KILL is reaped on the
        // following pulse rather than deleting from inside animator update.
        owner->NextFrame();
        if (owner->GetFlags() & OF_KILL)
        {
            log_info("[combatflash-preview] natural reap map_index=%d generation=%u state=%d tick=%llu frame=%d",
                     owner->GetMapIndex(), owner->SafeRefGen(), context->state,
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
    if (!context || !Renderer || !context->owner || (context->owner->GetFlags() & OF_KILL)) return;
    TObjectInstance* owner = context->identity.Get();
    if (!owner || owner != context->owner.get() || owner->GetState() != context->state) return;
    owner->Animate(false); // render pose refresh only; never advances NextFrame/Pulse
    auto* animator = dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    auto* imagery = dynamic_cast<T3DImagery*>(owner->GetImagery());
    if (!animator || !imagery || animator->PartSysUnsupported()) return;
    // Retail default Render: controllers first (40e8ed), then the meshes (40eaa3).
    const hmm_vec3 render_scale = {1, 1, 1};
    const int32_t prototype = profile::prototypes[context->state / 2];
    S3DObj authored = {};
    S3DMat material = {};
    imagery->GetObject(prototype, &authored);
    imagery->GetMaterial(authored.material, &material);
    animator->SubmitPartSys(*Renderer, prototype, material.texture + 1, render_scale, StartupVfxNativeDomain,
                            context->show_particles);
    if (!context->show_base) return;
    for (const State::Flare& flare : context->flares)
    {
        uint32_t blend = 0;
        hmm_mat4 world;
        if (!animator->CombatFlashBaseMeshBlend(flare.object, blend) || blend != 16 ||
            !animator->CombatFlashBaseMeshWorldMatrix(flare.object, world)) continue;
        SHelperMeshSubmit mesh = {};
        mesh.mesh = flare.mesh;
        for (int row = 0; row < 4; ++row)
            for (int column = 0; column < 4; ++column)
                mesh.world[row * 4 + column] = world.Elements[column][row];
        const auto copy = [](float* destination, const SRenderColor& color) {
            destination[0] = color.r; destination[1] = color.g;
            destination[2] = color.b; destination[3] = color.a;
        };
        copy(mesh.diffuse, flare.material.diffuse); copy(mesh.ambient, flare.material.ambient);
        copy(mesh.specular, flare.material.specular); copy(mesh.emissive, flare.material.emissive);
        mesh.power = flare.material.power;
        mesh.additive_blend = true;
        mesh.retail_lighting = 1;
        mesh.retail_software_projection = StartupVfxNativeDomain;
        mesh.retail_software_pixel_grid = StartupVfxNativeDomain;
        if (StartupVfxNativeDomain)
            mesh.retail_normal_z_scale = 1.0f;
        mesh.retail_additive_cull_none = StartupVfxNativeDomain; // observed original SetRenderState(22,1)
        mesh.retail_gold_no_depth = false; // Mode16 keeps depth testing; no '$' flag.
        Renderer->SubmitGoldFlareAfterFx(mesh); // Existing after-FX queue; depth remains enabled.
    }
}

bool Alive(State* context)
{
    if (!context || !context->owner) return false;
    auto* owner = context->identity.Get();
    return owner && owner == context->owner.get() && !(owner->GetFlags() & OF_KILL);
}
void Destroy(State* context) { delete context; }
}
