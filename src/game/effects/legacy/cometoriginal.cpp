#include "game/effects/legacy/cometoriginal.h"
#include "game/imagery/3dimage.h"
#include "game/effects/effect.h"
#include "engine/core/logging.h"
#include "game/imagery/meshextract.h"
#include "engine/render/renderer.h"
#include "engine/render/retailsoftwaretransform.h"
#include "game/effects/cometmatrix.h"
#include "game/effects/cometsoftwarebounds.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include <cmath>
#include <cstring>
#include <memory>

namespace comet_original {
struct State {
    TObjectPtr<TObjectInstance> owner;
    struct Part {MeshHandle mesh=0;S3DMat material{};int object=0,slot=0,triangle=0;hmm_vec3 vertices[3]{};};
    std::vector<Part> parts;
    S3DPoint source{};
    double accumulated=0;
    ~State(){if(Renderer)for(auto& p:parts)if(p.mesh)Renderer->ReleaseMeshAssetRef(p.mesh);}

};
State* Spawn(const S3DPoint& source)
{
    if(!Renderer)return nullptr;
    auto c=std::make_unique<State>();c->source=source;
    auto* cls=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cls?cls->FindObjType(0xf32bcfacu):-1;
    if(type<0)return nullptr;
    SObjectState def{};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos=source;
    c->owner.reset(cls->NewObject(&def));auto* owner=c->owner.get();
    if(!owner || owner->ObjId()!=0xf32bcfacu || !dynamic_cast<TEffect*>(owner))return nullptr;
    // Controlled coordinates can lie outside loaded map sectors. Establish
    // the explicit fixture transform before creating the animator/controllers.
    SetIntPos(owner, source);owner->Face(0);owner->OnScreen();
    auto* image=dynamic_cast<T3DImagery*>(owner->GetImagery());auto* animator=dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    if(!image || !animator || !image->HasRetailCometProfile() || animator->PartSysUnsupported() || animator->PartSysControllerCount()!=3)return nullptr;
    animator->SetCometGround([](int,int,int){return 1;});
    // Original 40e8d0 renders controllers first, then texture-major objects.
    for(int slot=1;slot<=2;++slot)for(int object=0;object<8;++object){
        if(object==0 || object==2 || object==5)continue; // Hidden particle prototypes.
        std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;
        if(!ExtractSubMeshTextureSlot(image,object,slot,vertices,indices))continue;
        // Separate immutable triangles permit the actual software edge guard;
        // modern/hardware policy still draws every authored triangle.
        for(size_t triangle=0;triangle<indices.size();triangle+=3){
            State::Part part;part.object=object;part.slot=slot;part.triangle=int(triangle/3);S3DObj obj{};image->GetObject(object,&obj);image->GetMaterial(obj.material,&part.material);
            for(int corner=0;corner<3;++corner){const auto& v=vertices[indices[triangle+corner]];part.vertices[corner]={v.pos[0],v.pos[1],v.pos[2]};}
            const uint64_t key=0x434f4d5400000000ull^(uint64_t(uint32_t(image->ImageryId()))<<16)^uint64_t(object*16+slot*4);
            part.mesh=Renderer->RegisterMeshAsset(key,vertices.data(),int(vertices.size()),indices.data(),int(indices.size()),image->GetTextureHandle(slot-1));
            if(!part.mesh)return nullptr;Renderer->AddMeshAssetRef(part.mesh);c->parts.push_back(part);
        }
    }
    if(c->parts.size()!=4)return nullptr;
    owner->Animate(false);
    log_info("[comet-original] id=f32bcfac frames=45 controllers=3 source=%d,%d,%d native=%d",source.x,source.y,source.z,int(StartupVfxNativeDomain));
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
    // Preserve native controller tag order: comet, trail, then impact sparks.
    for(int prototype:{0,5,2})a->SubmitPartSys(*Renderer,prototype,1,{1,1,1},StartupVfxNativeDomain);
    hmm_mat4 owner=o->Transform().Matrix();if(StartupVfxNativeDomain){const auto p=IntPos(o);BuildRetailSoftwareOwner(owner,p.x,p.y,p.z,uint8_t(IntFacing(o)));}
    for(const auto& part:c->parts){
        hmm_vec3 p{},r{},s{};if(!image->GetUninterpolatedAniKey(part.object,o->GetState(),o->GetFrame(),p,r,s,true) || s.X<=1e-5f || s.Y<=1e-5f || s.Z<=1e-5f)continue;
        hmm_mat4 local;BuildRetailCometMatrix(local,part.object,p,r,s);
        hmm_mat4 world;MtxMultiply(&world,&local,&owner);
        if(StartupVfxNativeDomain && Renderer->UsesRetailSoftwareMeshLighting()){
            hmm_mat4 native_owner,native_world;const auto position=IntPos(o);
            BuildRetailSoftwareOwner(native_owner,position.x,position.y,position.z,uint8_t(IntFacing(o)));
            MtxMultiply(&native_world,&local,&native_owner);
            if(!RetailCometTriangleAllowed(part.vertices,native_world))continue;
        }
        SHelperMeshSubmit q{};q.mesh=part.mesh;q.index_offset=part.triangle*3;q.index_count=3;q.additive_blend=true;q.retail_lighting=1;q.retail_gold_no_depth=true;q.retail_positive_face_cull=true;
        q.retail_software_projection=StartupVfxNativeDomain;q.retail_normal_z_scale=StartupVfxNativeDomain?1.0f:1.0f/WORLD3D_Z_SCALE;
        for(int row=0;row<4;++row)for(int col=0;col<4;++col)q.world[row*4+col]=world.Elements[col][row];
        const auto& m=part.material.matdesc;std::memcpy(q.diffuse,&m.diffuse,16);std::memcpy(q.ambient,&m.ambient,16);std::memcpy(q.specular,&m.specular,16);std::memcpy(q.emissive,&m.emissive,16);q.power=m.power;q.sort_depth=world.Elements[3][2];Renderer->SubmitHelperMesh(q);
    }
}
bool IsAlive(const State* c){return c && c->owner && !(c->owner->GetFlags()&OF_KILL);}
void Destroy(State* c){delete c;}
}
