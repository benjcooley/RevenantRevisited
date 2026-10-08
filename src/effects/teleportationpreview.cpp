// Actual owner/default animator adapter for the exact shipped Teleportation loop.
#include "teleportationpreview.h"
#include "../3dimage.h"
#include "../effect.h"
#include "../logging.h"
#include "../meshextract.h"
#include "../object.h"
#include "../renderer.h"
#include "../time.h"
#include <cmath>
#include <memory>
#include <vector>

namespace teleportation_authored_preview {
struct State {
    struct Part { int32_t object=-1; MeshHandle mesh=0; S3DMaterial material{}; };
    std::unique_ptr<TObjectInstance> owner;
    TSafeRef<> identity;
    std::vector<Part> parts;
    double accumulated=0;
    uint64_t ticks=0;
    ~State() { if (Renderer) for (const auto& p:parts) if(p.mesh) Renderer->ReleaseMeshAssetRef(p.mesh); }
};
State* Spawn(const S3DPoint& origin) {
    if (!Renderer) return nullptr;
    auto context=std::make_unique<State>();
    auto* effect_class=TObjectClass::GetClass(OBJCLASS_EFFECT);
    const int32_t type=effect_class ? effect_class->FindObjType(0xad92bd40u) : -1;
    if(type<0) return nullptr;
    SObjectDef definition={}; definition.objclass=OBJCLASS_EFFECT; definition.objtype=short(type); definition.pos=origin;
    context->owner.reset(effect_class->NewObject(&definition));
    auto* owner=context->owner.get();
    if(!owner || owner->ObjId()!=0xad92bd40u || owner->GetMapIndex()<=0 || !dynamic_cast<TEffect*>(owner)) return nullptr;
    context->identity=owner; owner->OnScreen();
    auto* image=dynamic_cast<T3DImagery*>(owner->GetImagery());
    auto* animator=dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    if(!image || !animator || !image->HasRetailTeleportationProfile() || owner->GetState()!=0) return nullptr;
    constexpr int face_groups[6][9]={{0,36,0,0,0,0,0,0,0},{0,36,0,0,0,0,0,0,0},{0,0,40,20,0,0,0,0,0},{0,0,0,0,40,20,0,0,0},{0,0,0,0,0,0,40,20,0},{0,0,0,0,0,0,0,0,2}};
    for(int object=0;object<6;++object) for(int slot=1;slot<=8;++slot) {
        if(!face_groups[object][slot]) continue;
        std::vector<SMeshVertex> vertices; std::vector<uint16_t> indices;
        const auto texture=image->GetTextureHandle(slot-1);
        if(texture==kInvalidTexture || !ExtractSubMeshTextureSlot(image,object,slot,vertices,indices)) return nullptr;
        State::Part part; part.object=object;
        const uint64_t key=0x54454c5050524556ull ^ (uint64_t(object+1)<<32) ^ uint64_t(slot);
        part.mesh=Renderer->RegisterMeshAsset(key,vertices.data(),int32_t(vertices.size()),indices.data(),int32_t(indices.size()),texture);
        if(!part.mesh) return nullptr;
        Renderer->AddMeshAssetRef(part.mesh);
        S3DObj obj={}; S3DMat material={}; image->GetObject(object,&obj); image->GetMaterial(obj.material,&material);
        part.material=material.matdesc; context->parts.push_back(part);
    }
    owner->Animate(false);
    log_info("[teleportation-preview] actual owner id=ad92bd40 map_index=%d generation=%u objects=6 frames=100 loop=1 blend=16",owner->GetMapIndex(),owner->SafeRefGen());
    return context.release();
}
void Advance(State* context,double seconds) {
    if(!context || !context->owner || !std::isfinite(seconds) || seconds<=0) return;
    context->accumulated+=seconds;
    while(context->accumulated+1e-10>=TTime::LegacyFrameSeconds && context->owner) {
        context->accumulated-=TTime::LegacyFrameSeconds;
        auto* owner=context->identity.Get(); if(!owner || owner!=context->owner.get()) return;
        owner->NextFrame();
        if(owner->GetFlags()&OF_KILL) { context->owner.reset(); return; }
        owner->Pulse(); ++context->ticks; // production generic loop progression, no frame/reset or respawn
    }
}
void SubmitWorld(State* context,EFxDebugMode) {
    if(!context || !Renderer || !context->owner) return;
    auto* owner=context->identity.Get(); if(!owner || owner!=context->owner.get()) return;
    owner->Animate(false);
    auto* animator=dynamic_cast<T3DAnimator*>(owner->GetAnimator()); if(!animator) return;
    const auto copy=[](float* out,const SRenderColor& c) { out[0]=c.r;out[1]=c.g;out[2]=c.b;out[3]=c.a; };
    for(const auto& part:context->parts) {
        hmm_mat4 world; if(!animator->TeleportationMeshWorldMatrix(part.object,world)) continue;
        SHelperMeshSubmit mesh={}; mesh.mesh=part.mesh;
        for(int row=0;row<4;++row) for(int col=0;col<4;++col) mesh.world[row*4+col]=world.Elements[col][row];
        copy(mesh.diffuse,part.material.diffuse);copy(mesh.ambient,part.material.ambient);
        copy(mesh.specular,part.material.specular);copy(mesh.emissive,part.material.emissive);mesh.power=part.material.power;
        mesh.additive_blend=true;mesh.retail_lighting=1;mesh.retail_gold_no_depth=false;mesh.retail_positive_face_cull=true;
        Renderer->SubmitGoldFlareAfterFx(mesh); // exact authored object/texture-pass order0..5/1..8, litadd16/depth-test/noZwrite
    }
}
void Destroy(State* context) { delete context; }
}
