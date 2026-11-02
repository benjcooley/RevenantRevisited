#include "game/effects/legacy/dragonattackoriginal.h"
#include "game/imagery/3dimage.h"
#include "game/effects/effect.h"
#include "engine/core/logging.h"
#include "game/imagery/meshextract.h"
#include "engine/render/renderer.h"
#include "engine/render/retailsoftwaretransform.h"
#include "game/effects/headfireballmatrix.h"
#include "game/effects/dragonattackcore.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include <cmath>
#include <cstring>
#include <memory>

namespace dragonattack_original {
struct State {
    TObjectPtr<TObjectInstance> owner;
    struct Part {MeshHandle mesh=0;S3DMat material{};int object=0,slot=0;};
    std::vector<Part> parts;
    retail_dragonattack::State simulation;
    double accumulated=0;
    int ticks=0;
    ~State(){if(Renderer)for(auto& p:parts)if(p.mesh)Renderer->ReleaseMeshAssetRef(p.mesh);}

};
State* Spawn(const S3DPoint& source)
{
    if(!Renderer)return nullptr;
    auto c=std::make_unique<State>();if(!c->simulation.Initialize(source.x,source.y,source.z,14,14,source.x+280,source.y,0))return nullptr;
    auto* cls=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cls?cls->FindObjType(0xba09feadu):-1;
    if(type<0)return nullptr;
    SObjectState def{};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos=source;
    c->owner.reset(cls->NewObject(&def));auto* owner=c->owner.get();
    if(!owner || owner->ObjId()!=0xba09feadu || !dynamic_cast<TEffect*>(owner))return nullptr;
    // Controlled coordinates can lie outside loaded map sectors. Establish
    // the explicit fixture transform before creating the animator/controllers.
    SetIntPos(owner, source);owner->Face(0);owner->OnScreen();
    auto* image=dynamic_cast<T3DImagery*>(owner->GetImagery());auto* animator=dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    if(!image || !animator || !image->HasRetailHeadFireBallProfile() || animator->PartSysUnsupported())return nullptr;
    animator->SetDragonAttackGround([](int,int,int){return 0;});
    // Original 40e8d0 renders controllers first, then texture-major objects.
    for(int slot=1;slot<=1;++slot)for(int object=4;object<5;++object){
        std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;
        if(!ExtractSubMeshTextureSlot(image,object,slot,vertices,indices))continue;
        State::Part part;part.object=object;part.slot=slot;S3DObj obj{};image->GetObject(object,&obj);image->GetMaterial(obj.material,&part.material);
        const uint64_t key=0x4452414700000000ull^(uint64_t(uint32_t(image->ImageryId()))<<16)^uint64_t(object*3+slot);
        part.mesh=Renderer->RegisterMeshAsset(key,vertices.data(),int(vertices.size()),indices.data(),int(indices.size()),image->GetTextureHandle(slot-1));
        if(!part.mesh)return nullptr;Renderer->AddMeshAssetRef(part.mesh);c->parts.push_back(part);
    }
    if(c->parts.size()!=1)return nullptr;
    owner->Animate(false);
    log_info("[dragonattack-original] id=ba09fead states=20/15/55 source=%d,%d,%d destination=%d,%d,%d caller=SidDragonAttack invoker_face=96 target=Fighting empty_ground=0 native=%d",source.x,source.y,source.z,source.x+280,source.y,0,int(StartupVfxNativeDomain));
    log_info("[native-domain] enabled=%d camera_z=0",int(StartupVfxNativeDomain));
    return c.release();
}
void Advance(State* c,double seconds)
{
    if(!c || !c->owner || !std::isfinite(seconds) || seconds<=0)return;
    c->accumulated+=seconds;auto* o=c->owner.get();
    while(c->accumulated+1e-10>=TTime::LegacyFrameSeconds && !(o->GetFlags()&OF_KILL)){
        c->accumulated-=TTime::LegacyFrameSeconds;o->NextFrame();++c->ticks;
        // Actual4de800 pulses the animator before custom505a90 moves/changes state.
        o->Pulse();const int previous=c->simulation.state;c->simulation.Pulse(0);
        if(c->simulation.state!=previous)o->SetState(c->simulation.state);
        o->SetFrame(c->simulation.frame);
        S3DPoint velocity={c->simulation.vx,c->simulation.vy,c->simulation.vz};o->SetVel(velocity);
        if(c->simulation.state>0)o->SetFlags(o->GetFlags()|OF_NOCOLLISION);
        SetIntPos(o, {c->simulation.x,c->simulation.y,c->simulation.z});
        if(!c->simulation.alive)o->SetFlags(o->GetFlags()|OF_KILL|OF_PULSE);
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
        a->SetDragonAttackPrototypePose(prototype_position,prototype_scale);
    a->SubmitPartSys(*Renderer,0,1,{1,1,1},StartupVfxNativeDomain);
    a->SubmitPartSys(*Renderer,3,1,{1,1,1},StartupVfxNativeDomain);
    const auto stats=a->PartSysStats();
    const auto& cache=a->DragonAttackPrototype().value;
    log_info("[dragonattack-phase] tick=%d state=%d frame=%d position=%d,%d,%d alive=%zu capacity=%zu random=%llu cache=%.9g,%.9g,%.9g",c->ticks,o->GetState(),o->GetFrame(),IntPos(o).x,IntPos(o).y,IntPos(o).z,size_t(stats.alive),size_t(stats.capacity),static_cast<unsigned long long>(RandomDraws()),cache[0],cache[1],cache[2]);
    hmm_mat4 owner=o->Transform().Matrix();if(StartupVfxNativeDomain){const auto p=IntPos(o);BuildRetailSoftwareOwner(owner,p.x,p.y,p.z,uint8_t(IntFacing(o)));}
    for(const auto& part:c->parts){
        hmm_vec3 p{},r{},s{};if(!image->GetUninterpolatedAniKey(part.object,o->GetState(),o->GetFrame(),p,r,s,true) || s.X<=1e-5f || s.Y<=1e-5f || s.Z<=1e-5f)continue;
        hmm_mat4 local;BuildRetailHeadFireBallMatrix(local,part.object,p,r,s);
        hmm_mat4 world;MtxMultiply(&world,&local,&owner);
        SHelperMeshSubmit q{};q.mesh=part.mesh;q.additive_blend=true;q.retail_lighting=1;q.retail_gold_no_depth=false;q.retail_positive_face_cull=true;
        q.retail_software_projection=StartupVfxNativeDomain;q.retail_normal_z_scale=StartupVfxNativeDomain?1.0f:1.0f/WORLD3D_Z_SCALE;
        for(int row=0;row<4;++row)for(int col=0;col<4;++col)q.world[row*4+col]=world.Elements[col][row];
        const auto& m=part.material.matdesc;std::memcpy(q.diffuse,&m.diffuse,16);std::memcpy(q.ambient,&m.ambient,16);std::memcpy(q.specular,&m.specular,16);std::memcpy(q.emissive,&m.emissive,16);q.power=m.power;q.sort_depth=world.Elements[3][2];Renderer->SubmitHelperMesh(q);
    }
}
bool IsAlive(const State* c){return c && c->owner && !(c->owner->GetFlags()&OF_KILL);}
void Destroy(State* c){delete c;}
}
