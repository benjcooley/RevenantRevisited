#include "ytelekinesisoriginal.h"
#include "../3dimage.h"
#include "../effect.h"
#include "../logging.h"
#include "../meshextract.h"
#include "../ytelekinesismatrix.h"
#include "../ytelekinesisprofile.h"
#include "../cometsoftwarebounds.h"
#include "../testconfig.h"
#include "../time.h"
#include <array>
#include <cmath>
#include <cstring>
#include <memory>
namespace ytelekinesis_original {
struct State {
    std::unique_ptr<TObjectInstance> owner;
    ytelekinesis_core::State core;
    struct Part {MeshHandle mesh=0;S3DMat material{};std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;};
    std::array<Part,4> parts{};
    double accumulated=0;
    int ticks=0;
    ~State(){if(Renderer)for(auto& p:parts)if(p.mesh)Renderer->ReleaseMeshAssetRef(p.mesh);}
};
static bool ValidateYTelekinesis(T3DImagery& image) {
    if(image.NumStates()!=1 || image.NumObjects()!=4 || image.NumTextures()!=4 || image.NumMaterials()!=4 || image.NumTags()!=0 || image.GetAniLength(0)!=100)return false;
    const auto hash=[](const void* data,size_t n){uint64_t h=1469598103934665603ull;const auto* b=static_cast<const uint8_t*>(data);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;};
    for(int i=0;i<4;++i){S3DObj o{};image.GetObject(i,&o);const auto& p=retail_ytelekinesis::objects[i];
        if(std::strcmp(o.name,p.name) || o.material!=p.material || o.numverts!=p.vertices || o.numfaces!=p.faces || o.parent[0]!=-1)return false;
        if(!o.anikeys[0] || o.numanikeys[0]!=9 || hash(o.anikeys[0],9*sizeof(SAniKey32))!=p.key_hash)return false;
        for(int slot=0;slot<5;++slot)if(o.numtexfaces[slot]!=(slot==i+1?p.faces:0))return false;
        std::vector<S3DVertex> v(size_t(p.vertices));std::vector<S3DFace> f(size_t(p.faces));image.GetObjVerts(i,v.data(),0,0);image.GetObjFaces(i,f.data());
        if(hash(v.data(),v.size()*sizeof(S3DVertex))!=p.vertex_hash || hash(f.data(),f.size()*sizeof(S3DFace))!=p.face_hash)return false;
    }
    for(int i=0;i<4;++i){S3DTex t{};image.GetTexture(i,&t);const auto& f=t.desc.pixelFormat;
        if(t.numframes!=1 || t.desc.width!=32 || t.desc.height!=32 || f.dwRGBBitCount!=16 || f.dwRBitMask!=0xf800 || f.dwGBitMask!=0x7e0 || f.dwBBitMask!=31 || f.dwRGBAlphaBitMask)return false;
    }
    return true;
}
State* Spawn(const S3DPoint& source) {
    if(!Renderer)return nullptr;auto c=std::make_unique<State>();const uint32_t id=0xaeaeeb28u;auto* cls=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cls?cls->FindObjType(id):-1;if(type<0)return nullptr;
    SObjectDef def{};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos=source;c->owner.reset(cls->NewObject(&def));
    auto* owner=c->owner.get();if(!owner || owner->ObjId()!=id)return nullptr;
    owner->ForcePos(source);owner->Face(0);owner->OnScreen();auto* image=dynamic_cast<T3DImagery*>(owner->GetImagery());
    if(!image || !ValidateYTelekinesis(*image)){log_warn("[ytelekinesis-original] exact profile rejected");return nullptr;}
    constexpr int slots[]={1,2,3,4};
    for(int i=0;i<4;++i){auto& p=c->parts[i];if(!ExtractSubMeshTextureSlot(image,i,slots[i],p.vertices,p.indices))return nullptr;
        S3DObj object{};image->GetObject(i,&object);image->GetMaterial(object.material,&p.material);
        const auto& vertices=p.vertices;
        p.mesh=Renderer->RegisterMeshAsset(0x5954454c00000000ull^(uint64_t(uint32_t(image->ImageryId()))<<8)^uint64_t(i),vertices.data(),int(vertices.size()),p.indices.data(),int(p.indices.size()),slots[i]?image->GetTextureHandle(slots[i]-1):Renderer->WhiteTextureHandle());
        if(!p.mesh)return nullptr;Renderer->AddMeshAssetRef(p.mesh);
    }
    c->core.Initialize([](int lo,int hi){return random(lo,hi);});
    log_info("[ytelekinesis-original] id=aeaeeb28 player=%d,%d,%d null_spell=1 rocks=%d",source.x,source.y,source.z,c->core.count);
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
    const auto source=c->owner->Pos();
    for(int i=0;i<c->core.count;++i){const auto& d=c->core.rocks[i];const auto& p=c->parts[d.mesh];hmm_mat4 world;ytelekinesis_core::Matrix(world,d);
        world.Elements[3][0]+=source.x;world.Elements[3][1]+=source.y;world.Elements[3][2]+=RetailSoftwareOwnerZ(source.z);
        if(!StartupVfxNativeDomain)for(int col=0;col<4;++col)world.Elements[col][2]*=WORLD3D_Z_SCALE;
        SHelperMeshSubmit q{};q.mesh=p.mesh;q.retail_lighting=1;q.retail_opaque=true;q.retail_positive_face_cull=true;
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
    }
    log_info("[ytelekinesis-phase] tick=%d rocks=%d random=%llu",c->ticks,c->core.count,static_cast<unsigned long long>(RandomDraws()));
}
void Destroy(State* c){delete c;}
}
