#include "funneloriginal.h"
#include "../3dimage.h"
#include "../effect.h"
#include "../logging.h"
#include "../meshextract.h"
#include "../renderer.h"
#include "../retailsoftwaretransform.h"
#include "../funnelmatrix.h"
#include "../testconfig.h"
#include "../time.h"
#include <cmath>
#include <cstring>
#include <memory>

namespace funnel_original {
struct State {
    std::unique_ptr<TObjectInstance> owner;
    struct Part {MeshHandle mesh=0;S3DMat material{};int object=0,slot=0;};
    std::vector<Part> parts;
    S3DPoint source{},target{},velocity{};
    int counter=0,ground_z=0;
    double accumulated=0;
    ~State(){if(Renderer)for(auto& p:parts)if(p.mesh)Renderer->ReleaseMeshAssetRef(p.mesh);}
    int Ground(int x,int y,int)const {
        const int64_t dx=int64_t(target.x)-source.x,dy=int64_t(target.y)-source.y;
        return (int64_t(x)-target.x)*dx+(int64_t(y)-target.y)*dy>=0?0:1;
    }
};
State* Spawn(const S3DPoint& source,const S3DPoint& target)
{
    if(!Renderer || (source.x==target.x && source.y==target.y))return nullptr;
    auto c=std::make_unique<State>();c->source=source;c->target=target;
    auto* cls=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cls?cls->FindObjType(0xad92bc17u):-1;
    if(type<0)return nullptr;
    SObjectDef def{};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos=source;
    c->owner.reset(cls->NewObject(&def));auto* owner=c->owner.get();
    if(!owner || owner->ObjId()!=0xad92bc17u || !dynamic_cast<TEffect*>(owner))return nullptr;
    // Controlled coordinates can lie outside loaded map sectors. Establish
    // the explicit fixture transform before creating the animator/controllers.
    owner->ForcePos(source);owner->Face(ConvertToFacing(source,target));owner->OnScreen();
    auto* image=dynamic_cast<T3DImagery*>(owner->GetImagery());auto* animator=dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    if(!image || !animator || !image->HasRetailFunnelProfile() || animator->PartSysUnsupported() || animator->PartSysControllerCount()!=2)return nullptr;
    auto* context=c.get();animator->SetFunnelGround([context](int x,int y,int z){return context->Ground(x,y,z);});
    // Original 40e8d0 renders controllers first, then texture-major objects.
    for(int slot=1;slot<=3;++slot)for(int object=0;object<4;++object){
        std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;
        if(!ExtractSubMeshTextureSlot(image,object,slot,vertices,indices))continue;
        State::Part part;part.object=object;part.slot=slot;S3DObj obj{};image->GetObject(object,&obj);image->GetMaterial(obj.material,&part.material);
        const uint64_t key=0x46554e4e00000000ull^(uint64_t(uint32_t(image->ImageryId()))<<16)^uint64_t(object*6+slot);
        part.mesh=Renderer->RegisterMeshAsset(key,vertices.data(),int(vertices.size()),indices.data(),int(indices.size()),image->GetTextureHandle(slot-1));
        if(!part.mesh)return nullptr;Renderer->AddMeshAssetRef(part.mesh);c->parts.push_back(part);
    }
    if(c->parts.size()!=4)return nullptr;
    owner->Animate(false);
    log_info("[funnel-original] id=ad92bc17 states=10/30/10 source=%d,%d,%d target=%d,%d,%d terrain=1/void0 native=%d",source.x,source.y,source.z,target.x,target.y,target.z,int(StartupVfxNativeDomain));
    log_info("[native-domain] enabled=%d camera_z=0",int(StartupVfxNativeDomain));
    return c.release();
}
void Advance(State* c,double seconds)
{
    if(!c || !c->owner || !std::isfinite(seconds) || seconds<=0)return;
    c->accumulated+=seconds;auto* o=c->owner.get();
    while(c->accumulated+1e-10>=TTime::LegacyFrameSeconds && !(o->GetFlags()&OF_KILL)){
        c->accumulated-=TTime::LegacyFrameSeconds;o->NextFrame();++c->counter;
        if(c->counter==1){ConvertToVector(o->GetFace(),4,c->velocity);o->Face(0);}
        switch(o->GetState()){
        case 0:if(o->CommandDone())o->SetState(1);break;
        case 1:{auto p=o->Pos();
            if(c->counter<=300){const int h=c->Ground(p.x,p.y,p.z);if(h>p.z+16 || !h)c->counter=300;}
            if(o->CommandDone() && c->counter>=300){o->SetState(2);break;}
            const bool wrapped=o->CommandDone();
            if(wrapped){o->SetFrame(0);o->SetCommandDone(false);}
            p.x+=c->velocity.x;p.y+=c->velocity.y;
            const int h=c->Ground(p.x,p.y,p.z);if(!wrapped || h<p.z+16)p.z=h;
            o->ForcePos(p);break;}
        case 2:if(o->CommandDone())o->SetFlags(o->GetFlags()|OF_KILL|OF_PULSE);break;
        }
        o->Pulse();
    }
}
void SubmitWorld(State* c,EFxDebugMode)
{
    if(!c || !c->owner || !Renderer || (c->owner->GetFlags()&OF_KILL))return;
    auto* o=c->owner.get();o->Animate(false);auto* a=dynamic_cast<T3DAnimator*>(o->GetAnimator());auto* image=a->Get3DImagery();
    a->SubmitPartSys(*Renderer,4,4,{1,1,1},StartupVfxNativeDomain);
    a->SubmitPartSys(*Renderer,6,5,{1,1,1},StartupVfxNativeDomain);
    hmm_mat4 owner=o->Transform().Matrix();if(StartupVfxNativeDomain){const auto p=o->Pos();BuildRetailSoftwareOwner(owner,p.x,p.y,p.z,uint8_t(o->GetFace()));}
    for(const auto& part:c->parts){
        hmm_vec3 p{},r{},s{};if(!image->GetUninterpolatedAniKey(part.object,o->GetState(),o->GetFrame(),p,r,s,true) || s.X<=1e-5f || s.Y<=1e-5f || s.Z<=1e-5f)continue;
        hmm_mat4 local;BuildRetailFunnelMatrix(local,part.object,p,r,s);
        hmm_mat4 world;MtxMultiply(&world,&local,&owner);
        SHelperMeshSubmit q{};q.mesh=part.mesh;q.additive_blend=true;q.retail_lighting=1;q.retail_additive_cull_none=true;
        q.retail_software_projection=StartupVfxNativeDomain;q.retail_normal_z_scale=StartupVfxNativeDomain?1.0f:1.0f/WORLD3D_Z_SCALE;
        for(int row=0;row<4;++row)for(int col=0;col<4;++col)q.world[row*4+col]=world.Elements[col][row];
        const auto& m=part.material.matdesc;std::memcpy(q.diffuse,&m.diffuse,16);std::memcpy(q.ambient,&m.ambient,16);std::memcpy(q.specular,&m.specular,16);std::memcpy(q.emissive,&m.emissive,16);q.power=m.power;q.sort_depth=world.Elements[3][2];Renderer->SubmitHelperMesh(q);
    }
}
bool IsAlive(const State* c){return c && c->owner && !(c->owner->GetFlags()&OF_KILL);}
void Destroy(State* c){delete c;}
}
