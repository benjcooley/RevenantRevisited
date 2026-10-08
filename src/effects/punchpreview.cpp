// STAGED ONLY. Install into src/effects after explicit integration approval.
// Requires the exact-ID generic Pulse fix in the adjacent staged patch.
#include "punchpreview.h"
#include "../3dimage.h"
#include "../effect.h"
#include "../logging.h"
#include "../meshextract.h"
#include "../object.h"
#include "../renderer.h"
#include "../time.h"
#include <cmath>
#include <memory>
#include <string>
#include <vector>

namespace punch_authored_preview {
struct State {
    struct Part { int32_t object = -1, texture_slot = -1; MeshHandle mesh = 0; S3DMaterial material{}; };
    std::unique_ptr<TObjectInstance> owner;
    TSafeRef<> identity;
    std::vector<Part> parts;
    double accumulated = 0;
    uint64_t ticks = 0;
    bool unsupported = false;
    ~State() { if (Renderer) for (const auto& p : parts) if (p.mesh) Renderer->ReleaseMeshAssetRef(p.mesh); }
};

State* Spawn(const S3DPoint& origin) {
    if (!Renderer) return nullptr;
    auto context = std::make_unique<State>();
    auto* effect_class = TObjectClass::GetClass(OBJCLASS_EFFECT);
    const int32_t type = effect_class ? effect_class->FindObjType(0x0c05263au) : -1;
    if (type < 0) return nullptr;
    SObjectDef definition = {};
    definition.objclass = OBJCLASS_EFFECT; definition.objtype = short(type); definition.pos = origin;
    context->owner.reset(effect_class->NewObject(&definition));
    auto* owner = context->owner.get();
    if (!owner || owner->ObjId() != 0x0c05263au || owner->GetMapIndex() <= 0 || !dynamic_cast<TEffect*>(owner)) return nullptr;
    context->identity = owner;
    owner->OnScreen();
    auto* image = dynamic_cast<T3DImagery*>(owner->GetImagery());
    auto* animator = dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    if (!image || !animator || !image->HasRetailPunchProfile() || image->NumStates() != 1 || image->GetAniLength(0) != 581 ||
        image->GetAniFlags(0) != 0x2001 || image->NumObjects() != 8 || image->NumTextures() != 2 || image->NumTags() != 0)
        return nullptr;
    constexpr const char* names[8] = {"phead01","pbody01","prightarm01","pleftarm01","jrightarm","jbody","jhead","jrightarm01"};
    constexpr int facecounts[8] = {71,15,19,15,15,26,44,15};
    for (int32_t object = 0; object < 8; ++object) {
        if (std::string(image->GetObjectName(object)) != names[object] || image->IsHidden(object, 0) ||
            image->GetObjectParent(object, 0) != -1 || image->NumObjFaces(object) != facecounts[object]) return nullptr;
        const int32_t slot = object < 4 ? 1 : 2;
        S3DTex texture = {}; image->GetTexture(slot - 1, &texture);
        const auto& pf = texture.desc.pixelFormat;
        if (texture.desc.width != 64 || texture.desc.height != 64 || texture.numframes != 1 ||
            pf.dwRGBBitCount != 16 || pf.dwRBitMask != 0xf800 || pf.dwGBitMask != 0x07e0 ||
            pf.dwBBitMask != 0x001f || pf.dwRGBAlphaBitMask) return nullptr;
        std::vector<SMeshVertex> vertices; std::vector<uint16_t> indices;
        const auto albedo = image->GetTextureHandle(slot - 1);
        if (albedo == kInvalidTexture || !ExtractSubMeshTextureSlot(image, object, slot, vertices, indices) ||
            indices.size() != size_t(facecounts[object] * 3)) return nullptr;
        State::Part part; part.object = object; part.texture_slot = slot;
        const uint64_t key = 0x50554e4350524556ull ^ (uint64_t(object + 1) << 32) ^ uint64_t(slot);
        part.mesh = Renderer->RegisterMeshAsset(key, vertices.data(), int32_t(vertices.size()), indices.data(), int32_t(indices.size()), albedo);
        if (!part.mesh) return nullptr;
        Renderer->AddMeshAssetRef(part.mesh);
        S3DObj obj = {}; S3DMat material = {}; image->GetObject(object, &obj); image->GetMaterial(obj.material, &material);
        part.material = material.matdesc; // retained immutable source metadata; Blue RGB path does not add specular/emissive
        context->parts.push_back(part);
    }
    owner->Animate(false);
    log_info("[punch-preview] actual owner id=%08x map_index=%d generation=%u objects=%zu frames=581 loop=1",
             owner->ObjId(), owner->GetMapIndex(), owner->SafeRefGen(), context->parts.size());
    return context.release();
}

void Advance(State* context, double seconds) {
    if (!context || !context->owner || context->unsupported || !std::isfinite(seconds) || seconds <= 0) return;
    context->accumulated += seconds;
    while (context->accumulated + 1e-10 >= TTime::LegacyFrameSeconds) {
        context->accumulated -= TTime::LegacyFrameSeconds;
        auto* owner = context->identity.Get();
        if (!owner || owner != context->owner.get()) return;
        owner->NextFrame();
        if (owner->GetFlags() & OF_KILL) { context->owner.reset(); return; }
        const int32_t frame = owner->GetFrame();
        owner->Pulse(); // never bypass generic Pulse or rewrite its output frame
        if (owner->GetFrame() != frame) {
            log_warn("[punch-preview] unsupported: generic Pulse reset authored frame; exact-ID patch is required");
            context->unsupported = true; return;
        }
        ++context->ticks;
    }
}

void SubmitWorld(State* context, EFxDebugMode) {
    if (!context || !Renderer || !context->owner || context->unsupported) return;
    auto* owner = context->identity.Get();
    if (!owner || owner != context->owner.get()) return;
    owner->Animate(false);
    auto* animator = dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    if (!animator) return;
    for (const auto& part : context->parts) {
        auto* bone = animator->GetObject(part.object);
        if (!bone) return;
        const auto& world = bone->transform.Matrix();
        SMeshSubmit mesh = {};
        mesh.mesh = part.mesh; mesh.obj_id = uint32_t(owner->GetMapIndex());
        for (int row = 0; row < 4; ++row) for (int column = 0; column < 4; ++column)
            mesh.world[row * 4 + column] = world.Elements[column][row];
        mesh.tint[0] = mesh.tint[1] = mesh.tint[2] = mesh.tint[3] = 1;
        mesh.retail_lighting = 1;
        mesh.retail_positive_face_cull = true;
        Renderer->SubmitMesh(mesh); // normal opaque source-cull/depth-write path
    }
}
void Destroy(State* context) { delete context; }
}
