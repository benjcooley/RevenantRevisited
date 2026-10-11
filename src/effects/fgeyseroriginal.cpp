#include "fgeyseroriginal.h"
#include "../3dimage.h"
#include "../effect.h"
#include "../logging.h"
#include "../meshextract.h"
#include "../renderer.h"
#include "../retailsoftwaretransform.h"
#include "../fgeysermatrix.h"
#include "../testconfig.h"
#include "../time.h"
#include <cmath>
#include <cstring>
#include <memory>

namespace fgeyser_original {
struct State {
    std::unique_ptr<TObjectInstance> owner;
    struct Part {MeshHandle mesh=0;S3DMat material{};int object=0,slot=0;};
    std::vector<Part> parts;
    S3DPoint source{};
    double accumulated=0;
    ~State(){if(Renderer)for(auto& p:parts)if(p.mesh)Renderer->ReleaseMeshAssetRef(p.mesh);}

};
State* Spawn(const S3DPoint& source)
{
    if(!Renderer)return nullptr;
    auto c=std::make_unique<State>();c->source=source;
    auto* cls=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cls?cls->FindObjType(0xad92bd31u):-1;
    if(type<0)return nullptr;
    SObjectDef def{};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos=source;
    c->owner.reset(cls->NewObject(&def));auto* owner=c->owner.get();
    if(!owner || owner->ObjId()!=0xad92bd31u || !dynamic_cast<TEffect*>(owner))return nullptr;
    // Controlled coordinates can lie outside loaded map sectors. Establish
    // the explicit fixture transform before creating the animator/controllers.
    owner->ForcePos(source);owner->Face(0);owner->OnScreen();
    auto* image=dynamic_cast<T3DImagery*>(owner->GetImagery());auto* animator=dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    if(!image || !animator || !image->HasRetailFGeyserProfile() || animator->PartSysUnsupported() || animator->PartSysControllerCount()!=2)return nullptr;
    animator->SetFGeyserGround([](int,int,int){return 1;});
    // Original 40e8d0 renders controllers first, then texture-major objects.
    for(int slot=1;slot<=1;++slot)for(int object=0;object<5;++object){
        if(object==0 || object==2)continue; // Hidden particle prototypes.
        std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;
        if(!ExtractSubMeshTextureSlot(image,object,slot,vertices,indices))continue;
        State::Part part;part.object=object;part.slot=slot;S3DObj obj{};
        image->GetObject(object,&obj);image->GetMaterial(obj.material,&part.material);
        const uint64_t key=0x4647455900000000ull^(uint64_t(uint32_t(image->ImageryId()))<<16)^uint64_t(object*16+slot*4);
        part.mesh=Renderer->RegisterMeshAsset(key,vertices.data(),int(vertices.size()),indices.data(),int(indices.size()),image->GetTextureHandle(slot-1));
        if(!part.mesh)return nullptr;
        Renderer->AddMeshAssetRef(part.mesh);c->parts.push_back(part);
    }
    if(c->parts.size()!=1)return nullptr;
    owner->Animate(false);
    log_info("[fgeyser-original] id=ad92bd31 frames=90 controllers=2 source=%d,%d,%d native=%d",source.x,source.y,source.z,int(StartupVfxNativeDomain));
    log_info("[native-domain] enabled=%d camera_z=0",int(StartupVfxNativeDomain));
    return c.release();
}
void Advance(State* c,double seconds)
{
    if(!c || !c->owner || !std::isfinite(seconds) || seconds<=0)return;
    c->accumulated+=seconds;auto* o=c->owner.get();
    while(c->accumulated+1e-10>=TTime::LegacyFrameSeconds && !(o->GetFlags()&OF_KILL)){
        c->accumulated-=TTime::LegacyFrameSeconds;o->NextFrame();
        o->Pulse();
    }
}
void SubmitWorld(State* c,EFxDebugMode)
{
    if(!c || !c->owner || !Renderer || (c->owner->GetFlags()&OF_KILL))return;
    auto* o=c->owner.get();o->Animate(false);auto* a=dynamic_cast<T3DAnimator*>(o->GetAnimator());auto* image=a->Get3DImagery();
    // Preserve native tag order: fire, then sparks.
    for(int prototype:{0,2})a->SubmitPartSys(*Renderer,prototype,1,{1,1,1},StartupVfxNativeDomain);
    hmm_mat4 owner=o->Transform().Matrix();if(StartupVfxNativeDomain){const auto p=o->Pos();BuildRetailSoftwareOwner(owner,p.x,p.y,p.z,uint8_t(o->GetFace()));}
    for(const auto& part:c->parts){
        hmm_vec3 p{},r{},s{};if(!image->GetUninterpolatedAniKey(part.object,o->GetState(),o->GetFrame(),p,r,s,true) || s.X<=1e-5f || s.Y<=1e-5f || s.Z<=1e-5f)continue;
        hmm_mat4 local;BuildRetailFGeyserMatrix(local,part.object,p,r,s);
        hmm_mat4 world;MtxMultiply(&world,&local,&owner);
        // Executed Scene mode16: two-sided, depth test, no depth write.
        SHelperMeshSubmit q{};q.mesh=part.mesh;q.additive_blend=true;q.retail_lighting=1;
        q.retail_software_projection=StartupVfxNativeDomain;q.retail_normal_z_scale=StartupVfxNativeDomain?1.0f:1.0f/WORLD3D_Z_SCALE;
        for(int row=0;row<4;++row)for(int col=0;col<4;++col)q.world[row*4+col]=world.Elements[col][row];
        const auto& m=part.material.matdesc;std::memcpy(q.diffuse,&m.diffuse,16);std::memcpy(q.ambient,&m.ambient,16);std::memcpy(q.specular,&m.specular,16);std::memcpy(q.emissive,&m.emissive,16);q.power=m.power;q.sort_depth=world.Elements[3][2];Renderer->SubmitHelperMesh(q);
    }
}
void Destroy(State* c){delete c;}
}
