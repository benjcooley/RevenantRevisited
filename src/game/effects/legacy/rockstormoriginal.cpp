#include "game/effects/legacy/rockstormoriginal.h"
#include "game/imagery/3dimage.h"
#include "game/effects/effect.h"
#include "engine/core/logging.h"
#include "game/imagery/meshextract.h"
#include "game/effects/rockstormmatrix.h"
#include "game/effects/rockstormprofile.h"
#include "game/effects/cometsoftwarebounds.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include <array>
#include <cmath>
#include <cstring>
#include <memory>
namespace rockstorm_original {
struct State {
    TObjectPtr<TObjectInstance> owner;
    rockstorm_core::State core;
    struct Part {MeshHandle mesh=0;S3DMat material{};std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;};
    std::array<Part,5> parts{};
    double accumulated=0;
    int ticks=0;
    ~State(){if(Renderer)for(auto& p:parts)if(p.mesh)Renderer->ReleaseMeshAssetRef(p.mesh);}
};
static bool ValidateRockStorm(T3DImagery& image) {
    if(image.NumStates()!=1 || image.NumObjects()!=5 || image.NumTextures()!=3 || image.NumMaterials()!=3 || image.NumTags()!=0 || image.GetAniLength(0)!=100)return false;
    const auto hash=[](const void* data,size_t n){uint64_t h=1469598103934665603ull;const auto* b=static_cast<const uint8_t*>(data);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;};
    for(int i=0;i<5;++i){S3DObj o{};image.GetObject(i,&o);const auto& p=retail_rockstorm::objects[i];
        if(std::strcmp(o.name,p.name) || o.material!=p.material || o.numverts!=p.vertices || o.numfaces!=p.faces || o.parent[0]!=-1)return false;
        if(!o.anikeys[0] || o.numanikeys[0]!=p.keys || hash(o.anikeys[0],p.keys*sizeof(SAniKey32))!=p.key_hash)return false;
        for(int slot=0;slot<4;++slot)if(o.numtexfaces[slot]!=p.face_counts[slot])return false;
        std::vector<S3DVertex> v(size_t(p.vertices));std::vector<S3DFace> f(size_t(p.faces));image.GetObjVerts(i,v.data(),0,0);image.GetObjFaces(i,f.data());
        if(hash(v.data(),v.size()*sizeof(S3DVertex))!=p.vertex_hash || hash(f.data(),f.size()*sizeof(S3DFace))!=p.face_hash)return false;
    }
    for(int i=0;i<3;++i){S3DTex t{};image.GetTexture(i,&t);const auto& f=t.desc.pixelFormat;const int size=i==1?64:128;
        if(t.numframes!=1 || t.desc.width!=size || t.desc.height!=size || f.dwRGBBitCount!=16)return false;
        if(i>0){if(f.dwRBitMask!=0xf00 || f.dwGBitMask!=0xf0 || f.dwBBitMask!=0xf || f.dwRGBAlphaBitMask!=0xf000)return false;}
        else if(f.dwRBitMask!=0xf800 || f.dwGBitMask!=0x7e0 || f.dwBBitMask!=31 || f.dwRGBAlphaBitMask)return false;
    }
    return true;
}
State* Spawn(const S3DPoint& source) {
    if(!Renderer)return nullptr;auto c=std::make_unique<State>();const uint32_t id=0xad92bd22u;auto* cls=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cls?cls->FindObjType(id):-1;if(type<0)return nullptr;
    SObjectState def{};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos=source;c->owner.reset(cls->NewObject(&def));
    auto* owner=c->owner.get();if(!owner || owner->ObjId()!=id)return nullptr;
    SetIntPos(owner, source);owner->Face(0);owner->OnScreen();auto* image=dynamic_cast<T3DImagery*>(owner->GetImagery());
    if(!image || !ValidateRockStorm(*image)){log_warn("[rockstorm-original] exact profile rejected");return nullptr;}
    constexpr int slots[]={1,1,1,2,3};
    for(int i:{0,3}){auto& p=c->parts[i];if(!ExtractSubMeshTextureSlot(image,i,slots[i],p.vertices,p.indices))return nullptr;
        S3DObj object{};image->GetObject(i,&object);image->GetMaterial(object.material,&p.material);
        const auto& vertices=p.vertices;
        p.mesh=Renderer->RegisterMeshAsset(0x524f434b00000000ull^(uint64_t(uint32_t(image->ImageryId()))<<8)^uint64_t(i),vertices.data(),int(vertices.size()),p.indices.data(),int(p.indices.size()),slots[i]?image->GetTextureHandle(slots[i]-1):Renderer->WhiteTextureHandle());
        if(!p.mesh)return nullptr;Renderer->AddMeshAssetRef(p.mesh);
    }
    c->core.Initialize([](int lo,int hi){return random(lo,hi);});
    c->core.pool.target_position[0]=source.x;c->core.pool.target_position[1]=source.y;c->core.pool.target_position[2]=source.z;
    log_info("[rockstorm-original] id=ad92bd22 level=1 target=%d,%d,%d radius=24 rocks=%d named_spell=Rock_Storm",source.x,source.y,source.z,c->core.pool.count);
    log_info("[native-domain] enabled=%d camera_z=0",int(StartupVfxNativeDomain));return c.release();
}
bool IsAlive(const State* c){return c && c->owner && c->core.alive && !(c->owner->GetFlags()&OF_KILL);}
void Advance(State* c,double seconds) {
    if(!c || !std::isfinite(seconds) || seconds<=0)return;c->accumulated+=seconds;
    while(c->accumulated+1e-10>=TTime::LegacyFrameSeconds && IsAlive(c)){
        c->accumulated-=TTime::LegacyFrameSeconds;c->owner->NextFrame();c->owner->Pulse();
        c->core.Pulse();++c->ticks;
        if(!c->core.alive)c->owner->SetFlags(c->owner->GetFlags()|OF_KILL);
    }
}
void SubmitWorld(State* c,EFxDebugMode) {
    if(!IsAlive(c) || !Renderer)return;
    const auto source=IntPos(c->owner);
    rockstorm_core::ForEachDraw(c->core,[&](const rockstorm_core::Draw& d){
        if(d.scale.X<=1e-5f || d.scale.Y<=1e-5f || d.scale.Z<=1e-5f)return;const auto& p=c->parts[d.object];hmm_mat4 world;rockstorm_core::Matrix(world,d);
        if(!StartupVfxNativeDomain)for(int col=0;col<4;++col)world.Elements[col][2]*=WORLD3D_Z_SCALE;
        SHelperMeshSubmit q{};q.mesh=p.mesh;q.retail_source_alpha=true;q.retail_lighting=d.object==3?3:1;
        q.retail_alpha_source_quantize=StartupVfxNativeDomain && d.object==3;
        q.retail_software_projection=StartupVfxNativeDomain;q.retail_normal_z_scale=StartupVfxNativeDomain?1.f:1.f/WORLD3D_Z_SCALE;
        const auto& m=p.material.matdesc;std::memcpy(q.diffuse,&m.diffuse,16);std::memcpy(q.ambient,&m.ambient,16);std::memcpy(q.specular,&m.specular,16);std::memcpy(q.emissive,&m.emissive,16);q.power=m.power;
        for(int r=0;r<4;++r)for(int col=0;col<4;++col)q.world[r*4+col]=world.Elements[col][r];q.sort_depth=RetailSoftwareOwnerZ(source.z);
        if(StartupVfxNativeDomain){
            q.index_count=3;
            for(size_t face=0;face<p.indices.size();face+=3){hmm_vec3 triangle[3];for(int i=0;i<3;++i){const auto& v=p.vertices[p.indices[face+i]];triangle[i]={v.pos[0],v.pos[1],v.pos[2]};}
                if(StartupVfxNativeDomain && Renderer->UsesRetailSoftwareMeshLighting() && !RetailCometTriangleAllowed(triangle,world))continue;
                q.index_offset=int(face);Renderer->SubmitHelperMesh(q);
            }
        }else Renderer->SubmitHelperMesh(q);
    });
    log_info("[rockstorm-phase] tick=%d stage=%d owner_stage=%d random=%llu",c->ticks,c->core.pool.stage,c->core.owner_stage,static_cast<unsigned long long>(RandomDraws()));
}
void Destroy(State* c){delete c;}
}
