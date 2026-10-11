#include "cameraparticlespreview.h"
#include "../3dimage.h"
#include "../effect.h"
#include "../logging.h"
#include "../meshextract.h"
#include "../renderer.h"
#include "../retailsoftwaretransform.h"
#include "../testconfig.h"
#include "../time.h"
#include <cmath>
#include <cstring>
#include <memory>
#include <vector>

namespace camera_particles_preview {
struct State
{
    struct Part {int object=0;MeshHandle mesh=0;S3DMaterial material{};};
    std::unique_ptr<TObjectInstance> owner;
    TSafeRef<> identity;
    std::vector<Part> parts;
    double accumulated=0;
    uint64_t ticks=0;
    ~State(){if(Renderer)for(const auto& part:parts)if(part.mesh)Renderer->ReleaseMeshAssetRef(part.mesh);}
};

State* Spawn(const S3DPoint& origin,uint32_t type_id)
{
    if(!Renderer || (type_id!=0x42e0fcd0u && type_id!=0xad92bd39u))return nullptr;
    auto state=std::make_unique<State>();
    auto* cl=TObjectClass::GetClass(OBJCLASS_EFFECT);
    const int type=cl?cl->FindObjType(type_id):-1;if(type<0)return nullptr;
    SObjectDef definition={};definition.objclass=OBJCLASS_EFFECT;definition.objtype=short(type);definition.pos=origin;
    state->owner.reset(cl->NewObject(&definition));auto* owner=state->owner.get();
    if(!owner || owner->ObjId()!=type_id || owner->GetState()!=0 || !dynamic_cast<TEffect*>(owner))return nullptr;
    state->identity=owner;owner->OnScreen();
    auto* imagery=dynamic_cast<T3DImagery*>(owner->GetImagery());
    auto* animator=dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    if(!imagery || !animator || !imagery->HasRetailCameraParticleProfile(type_id) ||
       animator->PartSysUnsupported() || animator->PartSysControllerCount()!=1)return nullptr;
    for(int object=1;object<imagery->NumObjects();++object){
        if(imagery->GetObjectName(object)[0]!='#')continue;
        std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;
        if(!ExtractSubMeshTextureSlot(imagery,object,1,vertices,indices))return nullptr;
        const auto texture=imagery->GetTextureHandle(0);if(texture==kInvalidTexture)return nullptr;
        State::Part part;part.object=object;
        const uint64_t key=0x4341500000000000ull^(uint64_t(type_id)<<8)^uint64_t(object);
        part.mesh=Renderer->RegisterMeshAsset(key,vertices.data(),int32_t(vertices.size()),indices.data(),int32_t(indices.size()),texture);
        if(!part.mesh)return nullptr;Renderer->AddMeshAssetRef(part.mesh);
        S3DObj resource={};S3DMat material={};imagery->GetObject(object,&resource);imagery->GetMaterial(resource.material,&material);
        part.material=material.matdesc;state->parts.push_back(part);
    }
    if(state->parts.size()!=3)return nullptr;
    owner->Animate(false);
    log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT particle=MODELZ",int(StartupVfxNativeDomain));
    log_info("[camera-particles-preview] actual owner id=%08x map_index=%d controllers=1 bases=3 origin=%d,%d,%d",type_id,owner->GetMapIndex(),origin.x,origin.y,origin.z);
    return state.release();
}

void Advance(State* state,double seconds)
{
    if(!state || !state->owner || !std::isfinite(seconds) || seconds<=0)return;
    state->accumulated+=seconds;
    while(state->accumulated+1e-10>=TTime::LegacyFrameSeconds){
        state->accumulated-=TTime::LegacyFrameSeconds;
        auto* owner=state->identity.Get();if(!owner || owner!=state->owner.get())return;
        owner->NextFrame();
        if(owner->GetFlags()&OF_KILL){state->owner.reset();return;}
        owner->Pulse();++state->ticks;
    }
}

void SubmitWorld(State* state)
{
    if(!state || !Renderer || !state->owner)return;
    auto* owner=state->identity.Get();if(!owner || owner!=state->owner.get())return;
    owner->Animate(false);
    auto* animator=dynamic_cast<T3DAnimator*>(owner->GetAnimator());if(!animator || animator->PartSysUnsupported())return;
    animator->SubmitPartSys(*Renderer,0,1,{1,1,1},StartupVfxNativeDomain);
    hmm_mat4 native_owner;
    if(StartupVfxNativeDomain){const auto& p=owner->Pos();BuildRetailSoftwareOwner(native_owner,p.x,p.y,p.z,uint8_t(owner->GetFace()));}
    for(const auto& part:state->parts){
        hmm_mat4 world;uint32_t blend=0;
        if(!animator->CameraParticleBaseMeshBlend(part.object,blend) ||
           !animator->CameraParticleBaseMeshWorldMatrix(part.object,world,StartupVfxNativeDomain?&native_owner:nullptr))continue;
        SHelperMeshSubmit mesh={};mesh.mesh=part.mesh;
        for(int row=0;row<4;++row)for(int column=0;column<4;++column)mesh.world[row*4+column]=world.Elements[column][row];
        const auto copy=[](float* out,const SRenderColor& c){out[0]=c.r;out[1]=c.g;out[2]=c.b;out[3]=c.a;};
        copy(mesh.diffuse,part.material.diffuse);copy(mesh.ambient,part.material.ambient);
        copy(mesh.specular,part.material.specular);copy(mesh.emissive,part.material.emissive);mesh.power=part.material.power;
        mesh.additive_blend=blend==16u || blend==80u;mesh.retail_gold_no_depth=blend==80u;
        mesh.retail_lighting=1;mesh.retail_positive_face_cull=true;mesh.retail_software_projection=StartupVfxNativeDomain;
        Renderer->SubmitGoldFlareAfterFx(mesh);
    }
}
void Destroy(State* state){delete state;}
}
