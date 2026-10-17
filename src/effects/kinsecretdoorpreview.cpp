#include "kinsecretdoorpreview.h"
#include "../effect.h"
#include "../3dimage.h"
#include "../kinsecretdoorprofile.h"
#include "../meshextract.h"
#include "../logging.h"
#include "../renderer.h"
#include "../time.h"
#include <cmath>
#include <memory>
#include <vector>

namespace kinsecretdoor_still_preview {
struct State {
    std::unique_ptr<TObjectInstance> owner;
    TSafeRef<> identity;
    MeshHandle meshes[2]={};
    S3DMat materials[2]={}; // Full authored descriptors retained; RGB565 SW ignores fabricated emissive/specular.
    double accumulator=0;
    ~State() { if(Renderer) for(const auto mesh:meshes) if(mesh) Renderer->ReleaseMeshAssetRef(mesh); }
};
State* Spawn(const S3DPoint& origin)
{
    if(!Renderer) return nullptr;
    auto context=std::make_unique<State>();
    TObjectClass* cls=TObjectClass::GetClass(OBJCLASS_EFFECT);
    const int type=cls?cls->FindObjType(0xad92bd23u):-1;
    if(type<0) return nullptr;
    SObjectDef definition={};definition.objclass=OBJCLASS_EFFECT;definition.objtype=short(type);definition.pos=origin;
    context->owner.reset(cls->NewObject(&definition));
    auto* owner=context->owner.get();
    auto* imagery=owner?dynamic_cast<T3DImagery*>(owner->GetImagery()):nullptr;
    if(!IsRetailKinSecretDoorStill(owner,imagery)||owner->GetMapIndex()<=0) return nullptr;
    context->identity=owner;owner->OnScreen();owner->Animate(false);
    auto* animator=dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    if(!animator || !animator->GetObject(0)) return nullptr;
    for(int slot=0;slot<2;++slot) {
        std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;
        if(!ExtractSubMeshTextureSlot(imagery,0,slot+1,vertices,indices)||indices.size()!=size_t(slot==0?158*3:102*3)) return nullptr;
        const uint64_t key=0x4b444f4f52000000ull^(uint64_t(uint32_t(imagery->ImageryId()))<<8)^uint64_t(slot);
        context->meshes[slot]=Renderer->RegisterMeshAsset(key,vertices.data(),int32_t(vertices.size()),
            indices.data(),int32_t(indices.size()),imagery->GetTextureHandle(slot));
        if(!context->meshes[slot]) return nullptr;
        Renderer->AddMeshAssetRef(context->meshes[slot]);
        imagery->GetMaterial(slot,&context->materials[slot]);
    }
    log_info("[kinsecretdoor-still] actual generic owner id=%08x map_index=%d state=0 opaque=1 source-cull=1 vertices=231 triangles=260",
        owner->ObjId(),owner->GetMapIndex());
    return context.release();
}
void Advance(State* context,double seconds)
{
    if(!context||!context->owner||!std::isfinite(seconds)||seconds<=0) return;
    context->accumulator+=seconds;
    while(context->accumulator+1e-10>=TTime::LegacyFrameSeconds) {
        context->accumulator-=TTime::LegacyFrameSeconds;
        auto* owner=context->identity.Get();
        auto* imagery=owner?dynamic_cast<T3DImagery*>(owner->GetImagery()):nullptr;
        if(!IsRetailKinSecretDoorStill(owner,imagery)) return;
        owner->NextFrame();owner->Pulse(); // Actual generic persistent still owner; no new clock/reset/respawn.
    }
}
void SubmitWorld(State* context,EFxDebugMode mode)
{
    if(!context||!Renderer||mode!=EFxDebugMode::Normal) return;
    auto* owner=context->identity.Get();
    auto* imagery=owner?dynamic_cast<T3DImagery*>(owner->GetImagery()):nullptr;
    if(!IsRetailKinSecretDoorStill(owner,imagery)) return;
    owner->Animate(false);
    auto* animator=dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    auto* bone=animator?animator->GetObject(0):nullptr;
    if(!bone) return;
    const auto& world=bone->transform.Matrix(); // Same live bone path as ordinary map renderer.
    for(const auto handle:context->meshes) {
        SMeshSubmit mesh={};mesh.mesh=handle;
        for(int row=0;row<4;++row) for(int column=0;column<4;++column) mesh.world[row*4+column]=world.Elements[column][row];
        mesh.tint[0]=mesh.tint[1]=mesh.tint[2]=mesh.tint[3]=1;
        mesh.retail_lighting=1;
        mesh.retail_positive_face_cull=true;
        Renderer->SubmitMesh(mesh); // Ordinary opaque G-buffer: depth LESS_EQUAL + write=true.
    }
}
void Destroy(State* context) { delete context; }
}
