#include "arroworiginal.h"
#include "../3dimage.h"
#include "../effect.h"
#include "../logging.h"
#include "../meshextract.h"
#include "../arrowmatrix.h"
#include "../arrowprofile.h"
#include "../testconfig.h"
#include "../time.h"
#include <array>
#include <cstring>
#include <memory>
namespace arrow_original {
struct State {
    std::unique_ptr<TObjectInstance> owner;
    arrow_core::State core;arrow_core::AttachedCaller attached;bool queen=true;
    arrow_core::State& Core(){return queen?core:attached.core;}
    const arrow_core::State& Core()const{return queen?core:attached.core;}
    S3DPoint source{},target{};int facing=64,ticks=0;
    struct Part{MeshHandle mesh=0;S3DMat material{};std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;};
    std::array<Part,7> parts;
    double accumulated=0;
    std::vector<arrow_core::Draw> draws;
    int64_t rendered=-1;
    ~State(){if(Renderer)for(auto& p:parts)if(p.mesh)Renderer->ReleaseMeshAssetRef(p.mesh);}
};
static bool Validate(T3DImagery& image){
    if(image.NumStates()!=1 || image.GetAniLength(0)!=100 || image.NumObjects()!=7 || image.NumTextures()!=0 || image.NumMaterials()!=7 || image.NumTags()!=0)return false;
    const auto hash=[](const void* data,size_t n){uint64_t h=1469598103934665603ull;const auto* p=static_cast<const uint8_t*>(data);while(n--)h=(h^*p++)*1099511628211ull;return h;};
    for(int i=0;i<7;++i){S3DObj o{};image.GetObject(i,&o);const auto& p=retail_arrow::objects[i];
        if(std::strcmp(o.name,p.name) || o.material!=i || o.numverts!=8 || o.numfaces!=6 || o.parent[0]!=-1 || o.numtexfaces[0]!=6)return false;
        S3DVertex v[8];S3DFace f[6];image.GetObjVerts(i,v,0,0);image.GetObjFaces(i,f);
        if(hash(v,sizeof(v))!=p.vertices || hash(f,sizeof(f))!=p.faces)return false;
    }return true;
}
static State* Spawn(const S3DPoint& source,const S3DPoint& target,bool queen){
    if(!Renderer || (source.x==target.x && source.y==target.y))return nullptr;
    auto c=std::make_unique<State>();c->source=source;c->target=target;c->facing=ConvertToFacing(source,target);c->queen=queen;
    c->attached.position={source.x,source.y,source.z};S3DPoint v{};ConvertToVector(c->facing,20*65536,v);c->attached.velocity_fixed={v.x,v.y,v.z};
    auto* cls=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cls?cls->FindObjType(queen?0xad92bd1eu:0xad92bd1fu):-1;if(type<0)return nullptr;
    SObjectDef def{};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos=source;c->owner.reset(cls->NewObject(&def));auto* o=c->owner.get();
    if(!o || o->ObjId()!=(queen?0xad92bd1eu:0xad92bd1fu))return nullptr;o->ForcePos(source);o->Face(0);o->OnScreen();auto* im=dynamic_cast<T3DImagery*>(o->GetImagery());
    if(!im || !Validate(*im)){log_warn("[arrow-original] exact profile rejected");return nullptr;}
    for(int i=0;i<7;++i){auto& p=c->parts[i];if(!ExtractSubMeshTextureSlot(im,i,0,p.vertices,p.indices))return nullptr;
        S3DObj object{};im->GetObject(i,&object);im->GetMaterial(object.material,&p.material);
        p.mesh=Renderer->RegisterMeshAsset(0x4152524f00000000ull^(uint64_t(uint32_t(im->ImageryId()))<<8)^uint64_t(i),p.vertices.data(),int(p.vertices.size()),p.indices.data(),int(p.indices.size()),Renderer->WhiteTextureHandle());
        if(!p.mesh)return nullptr;Renderer->AddMeshAssetRef(p.mesh);
    }
    log_info("[arrow-original] type=%s id=%08x source=%d,%d,%d target=%d,%d,%d face=%d",queen?"QueenArrow":"ArrowEffect",queen?0xad92bd1eu:0xad92bd1fu,source.x,source.y,source.z,target.x,target.y,target.z,c->facing);
    log_info("[native-domain] enabled=%d camera_z=0",int(StartupVfxNativeDomain));return c.release();
}
State* SpawnQueen(const S3DPoint& source,const S3DPoint& target){return Spawn(source,target,true);}
State* SpawnArrow(const S3DPoint& source,const S3DPoint& target){return Spawn(source,target,false);}
bool IsAlive(const State* s){return s && s->Core().alive && s->owner && !(s->owner->GetFlags()&OF_KILL);}
void Advance(State* s,double seconds){
    if(!IsAlive(s) || !std::isfinite(seconds) || seconds<=0)return;s->accumulated+=seconds;
    while(s->accumulated+1e-10>=TTime::LegacyFrameSeconds && IsAlive(s)){
        s->accumulated-=TTime::LegacyFrameSeconds;s->owner->NextFrame();
        const auto blocked=[&](arrow_core::Point p){
            return (int64_t(p.x)-s->target.x)*(int64_t(s->target.x)-s->source.x)+(int64_t(p.y)-s->target.y)*(int64_t(s->target.y)-s->source.y)>=0;
        };
        if(s->queen)s->core.Pulse({s->source.x,s->source.y,s->source.z},s->facing,blocked,[](int lo,int hi){return random(lo,hi);});
        else s->attached.Pulse(s->facing,blocked);++s->ticks;
        const auto p=s->Core().position;s->owner->ForcePos({p.x,p.y,p.z});
        if(!s->Core().alive)s->owner->SetFlags(s->owner->GetFlags()|OF_KILL|OF_PULSE);
        s->draws.clear();
        if(IsAlive(s))s->Core().Render([](int lo,int hi){return random(lo,hi);},
            [&](const arrow_core::Draw& d){s->draws.push_back(d);});
    }
}
void SubmitWorld(State* s,EFxDebugMode){
    if(!IsAlive(s) || !Renderer || s->rendered==TTime::FrameCount())return;
    s->rendered=TTime::FrameCount();
    for(const auto& d:s->draws){
        const auto& p=s->parts[d.object];hmm_mat4 world;arrow_core::Matrix(world,d);
        if(!StartupVfxNativeDomain)for(int col=0;col<4;++col)world.Elements[col][2]*=WORLD3D_Z_SCALE;
        SHelperMeshSubmit q{};q.mesh=p.mesh;q.retail_lighting=4;q.retail_software_projection=StartupVfxNativeDomain;q.retail_normal_z_scale=StartupVfxNativeDomain?1.f:1.f/WORLD3D_Z_SCALE;
        // Native untextured RGB5 kernel ignores Scene8's blend/ZWRITE settings.
        // The common hardware path retains the authored ONE/ONE scene policy.
        const bool software=Renderer->UsesRetailSoftwareMeshLighting();q.retail_source_alpha=software;q.additive_blend=!software;q.retail_additive_cull_none=true;
        const auto& m=p.material.matdesc;std::memcpy(q.diffuse,&m.diffuse,16);std::memcpy(q.ambient,&m.ambient,16);std::memcpy(q.specular,&m.specular,16);std::memcpy(q.emissive,&m.emissive,16);q.power=m.power;
        for(int row=0;row<4;++row)for(int col=0;col<4;++col)q.world[row*4+col]=world.Elements[col][row];q.sort_depth=0;
        if(software){q.retail_flat_depth=true;q.index_count=3;
            for(size_t face=0;face<p.indices.size();face+=3){float farthest=-1e30f;for(int i=0;i<3;++i){const auto& v=p.vertices[p.indices[face+i]];hmm_vec3 l={v.pos[0],v.pos[1],v.pos[2]},w;MtxTransform(&world,&l,&w);
                const float z=StartupVfxNativeDomain?-.8748177289962769f*(w.X+w.Y)-.7142857909202576f*w.Z:-.8660254037844386f*(w.X+w.Y)-.5f*w.Z;if(z>farthest)farthest=z;}
                q.index_offset=int(face);q.retail_flat_depth_offset=farthest;Renderer->SubmitHelperMesh(q);}
        }else Renderer->SubmitHelperMesh(q);
    }
    log_info("[arrow-phase] tick=%d grow=%d countdown=%d random=%llu",s->ticks,s->Core().grow,s->Core().countdown,static_cast<unsigned long long>(RandomDraws()));
}
void Destroy(State* s){delete s;}
}
