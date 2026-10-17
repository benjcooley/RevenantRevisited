#include "jhagaattackoriginal.h"
#include "../3dimage.h"
#include "../effect.h"
#include "../logging.h"
#include "../meshextract.h"
#include "../renderer.h"
#include "../retailsoftwaretransform.h"
#include "../jhagaattackmatrix.h"
#include "../jhagaattackcore.h"
#include "../testconfig.h"
#include "../time.h"
#include <cmath>
#include <cstring>
#include <memory>

namespace jhagaattack_original {
struct State {
    std::unique_ptr<TObjectInstance> owner;
    struct Part {MeshHandle mesh=0;S3DMat material{};int object=0,slot=0;};
    std::vector<Part> parts;
    retail_jhagaattack::State simulation;
    double accumulated=0;
    int ticks=0;
    ~State(){if(Renderer)for(auto& p:parts)if(p.mesh)Renderer->ReleaseMeshAssetRef(p.mesh);}

};
State* Spawn(const S3DPoint& source)
{
    if(!Renderer)return nullptr;
    auto c=std::make_unique<State>();c->simulation.Initialize(source.x,source.y,source.z);
    auto* cls=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cls?cls->FindObjType(0xaeaeeb32u):-1;
    if(type<0)return nullptr;
    SObjectDef def{};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos=source;
    c->owner.reset(cls->NewObject(&def));auto* owner=c->owner.get();
    if(!owner || owner->ObjId()!=0xaeaeeb32u || !dynamic_cast<TEffect*>(owner))return nullptr;
    // Controlled coordinates can lie outside loaded map sectors. Establish
    // the explicit fixture transform before creating the animator/controllers.
    owner->ForcePos(source);owner->Face(0);owner->OnScreen();
    auto* image=dynamic_cast<T3DImagery*>(owner->GetImagery());auto* animator=dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    if(!image || !animator || !image->HasRetailJhagaAttackProfile() || animator->PartSysUnsupported())return nullptr;
    animator->SetJhagaAttackGround([](int,int,int){return 1;});
    // Original 40e8d0 renders controllers first, then texture-major objects.
    for(int slot=1;slot<=1;++slot)for(int object=4;object<5;++object){
        std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;
        if(!ExtractSubMeshTextureSlot(image,object,slot,vertices,indices))continue;
        State::Part part;part.object=object;part.slot=slot;S3DObj obj{};image->GetObject(object,&obj);image->GetMaterial(obj.material,&part.material);
        const uint64_t key=0x4a48474100000000ull^(uint64_t(uint32_t(image->ImageryId()))<<16)^uint64_t(object*3+slot);
        part.mesh=Renderer->RegisterMeshAsset(key,vertices.data(),int(vertices.size()),indices.data(),int(indices.size()),image->GetTextureHandle(slot-1));
        if(!part.mesh)return nullptr;Renderer->AddMeshAssetRef(part.mesh);c->parts.push_back(part);
    }
    if(c->parts.size()!=1)return nullptr;
    owner->Animate(false);
    log_info("[jhagaattack-original] id=aeaeeb32 states=20/15/55 source=%d,%d,%d destination=%d,%d,%d null_spell=1 ground=1 native=%d",source.x,source.y,source.z,source.x,source.y-120,source.z,int(StartupVfxNativeDomain));
    log_info("[native-domain] enabled=%d camera_z=0",int(StartupVfxNativeDomain));
    return c.release();
}
void Advance(State* c,double seconds)
{
    if(!c || !c->owner || !std::isfinite(seconds) || seconds<=0)return;
    c->accumulated+=seconds;auto* o=c->owner.get();
    while(c->accumulated+1e-10>=TTime::LegacyFrameSeconds && !(o->GetFlags()&OF_KILL)){
        c->accumulated-=TTime::LegacyFrameSeconds;o->NextFrame();++c->ticks;const int previous=c->simulation.phase;c->simulation.Pulse();
        if(c->simulation.phase!=previous)o->SetState(c->simulation.phase);
        o->ForcePos({c->simulation.x,c->simulation.y,c->simulation.z});
        if(!c->simulation.alive)o->SetFlags(o->GetFlags()|OF_KILL|OF_PULSE);
        o->Pulse();
    }
}
void SubmitWorld(State* c,EFxDebugMode)
{
    if(!c || !c->owner || !Renderer || (c->owner->GetFlags()&OF_KILL))return;
    auto* o=c->owner.get();o->Animate(false);auto* a=dynamic_cast<T3DAnimator*>(o->GetAnimator());auto* image=a->Get3DImagery();
    // Original refreshes the authored prototype before controller rendering;
    // a rejected zero-scale matrix keeps its previous cached translation.
    hmm_vec3 prototype_position{},prototype_rotation{},prototype_scale{};
    if(image->GetUninterpolatedAniKey(0,o->GetState(),o->GetFrame(),prototype_position,prototype_rotation,prototype_scale,true) &&
       prototype_scale.X>1e-5f && prototype_scale.Y>1e-5f && prototype_scale.Z>1e-5f)
        a->SetJhagaAttackObjectZeroOrigin(prototype_position);
    a->SubmitPartSys(*Renderer,0,1,{1,1,1},StartupVfxNativeDomain);
    a->SubmitPartSys(*Renderer,3,1,{1,1,1},StartupVfxNativeDomain);
    const auto stats=a->PartSysStats();
    log_info("[jhagaattack-phase] tick=%d state=%d frame=%d position=%d,%d,%d alive=%zu random=%llu",c->ticks,o->GetState(),o->GetFrame(),o->Pos().x,o->Pos().y,o->Pos().z,size_t(stats.alive),static_cast<unsigned long long>(RandomDraws()));
    hmm_mat4 owner=o->Transform().Matrix();if(StartupVfxNativeDomain){const auto p=o->Pos();BuildRetailSoftwareOwner(owner,p.x,p.y,p.z,uint8_t(o->GetFace()));}
    for(const auto& part:c->parts){
        hmm_vec3 p{},r{},s{};if(!image->GetUninterpolatedAniKey(part.object,o->GetState(),o->GetFrame(),p,r,s,true) || s.X<=1e-5f || s.Y<=1e-5f || s.Z<=1e-5f)continue;
        hmm_mat4 local;BuildRetailJhagaAttackMatrix(local,part.object,p,r,s);
        hmm_mat4 world;MtxMultiply(&world,&local,&owner);
        SHelperMeshSubmit q{};q.mesh=part.mesh;q.additive_blend=true;q.retail_lighting=1;q.retail_gold_no_depth=true;q.retail_positive_face_cull=true;
        q.retail_software_projection=StartupVfxNativeDomain;q.retail_normal_z_scale=StartupVfxNativeDomain?1.0f:1.0f/WORLD3D_Z_SCALE;
        for(int row=0;row<4;++row)for(int col=0;col<4;++col)q.world[row*4+col]=world.Elements[col][row];
        const auto& m=part.material.matdesc;std::memcpy(q.diffuse,&m.diffuse,16);std::memcpy(q.ambient,&m.ambient,16);std::memcpy(q.specular,&m.specular,16);std::memcpy(q.emissive,&m.emissive,16);q.power=m.power;q.sort_depth=world.Elements[3][2];Renderer->SubmitHelperMesh(q);
    }
}
bool IsAlive(const State* c){return c && c->owner && !(c->owner->GetFlags()&OF_KILL);}
void Destroy(State* c){delete c;}
}
