#include "cataclysmoriginal.h"
#include "../3dimage.h"
#include "../effect.h"
#include "../logging.h"
#include "../meshextract.h"
#include "../cataclysmmatrix.h"
#include "../cataclysmprofile.h"
#include "../cometsoftwarebounds.h"
#include "../testconfig.h"
#include "../time.h"
#include <array>
#include <cmath>
#include <cstring>
#include <memory>
namespace cataclysm_original {
struct State {
    TObjectPtr<TObjectInstance> owner;
    cataclysm_core::State core;
    struct Part {MeshHandle mesh=0;S3DMat material{};std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;};
    std::array<Part,5> parts{};
    double accumulated=0;
    int ticks=0;
    bool yellow_variant=false;
    ~State(){if(Renderer)for(auto& p:parts)if(p.mesh)Renderer->ReleaseMeshAssetRef(p.mesh);}
};
static bool ValidateCataclysm(T3DImagery& image) {
    if(image.NumStates()!=1 || image.NumObjects()!=5 || image.NumTextures()!=4 || image.NumMaterials()!=5 || image.NumTags()!=0 || image.GetAniLength(0)!=100)return false;
    const auto hash=[](const void* data,size_t n){uint64_t h=1469598103934665603ull;const auto* b=static_cast<const uint8_t*>(data);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;};
    for(int i=0;i<5;++i){S3DObj o{};image.GetObject(i,&o);const auto& p=retail_cataclysm::objects[i];
        if(std::strcmp(o.name,p.name) || o.material!=p.material || o.numverts!=p.vertices || o.numfaces!=p.faces || o.parent[0]!=-1)return false;
        for(int slot=0;slot<5;++slot)if(o.numtexfaces[slot]!=p.face_counts[slot])return false;
        std::vector<S3DVertex> v(size_t(p.vertices));std::vector<S3DFace> f(size_t(p.faces));image.GetObjVerts(i,v.data(),0,0);image.GetObjFaces(i,f.data());
        if(hash(v.data(),v.size()*sizeof(S3DVertex))!=p.vertex_hash || hash(f.data(),f.size()*sizeof(S3DFace))!=p.face_hash)return false;
    }
    for(int i=0;i<4;++i){S3DTex t{};image.GetTexture(i,&t);const auto& f=t.desc.pixelFormat;const int size=i==0?256:i==3?128:32;
        if(t.numframes!=1 || t.desc.width!=size || t.desc.height!=size || f.dwRGBBitCount!=16)return false;
        if(i==3){if(f.dwRBitMask!=0xf00 || f.dwGBitMask!=0xf0 || f.dwBBitMask!=0xf || f.dwRGBAlphaBitMask!=0xf000)return false;}
        else if(f.dwRBitMask!=0xf800 || f.dwGBitMask!=0x7e0 || f.dwBBitMask!=31 || f.dwRGBAlphaBitMask)return false;
    }
    return true;
}
State* Spawn(const S3DPoint& source,bool yellow_variant) {
    if(!Renderer)return nullptr;auto c=std::make_unique<State>();c->yellow_variant=yellow_variant;const uint32_t id=yellow_variant?0xad92fc13u:0xad92bc19u;auto* cls=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cls?cls->FindObjType(id):-1;if(type<0)return nullptr;
    SObjectDef def{};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos=source;c->owner.reset(cls->NewObject(&def));
    auto* owner=c->owner.get();if(!owner || owner->ObjId()!=id)return nullptr;
    SetIntPos(owner, source);owner->Face(0);owner->OnScreen();auto* image=dynamic_cast<T3DImagery*>(owner->GetImagery());
    if(!image || !ValidateCataclysm(*image)){log_warn("[cataclysm-original] exact profile rejected");return nullptr;}
    constexpr int slots[]={1,2,0,3,4};
    for(int i=0;i<5;++i){auto& p=c->parts[i];if(!ExtractSubMeshTextureSlot(image,i,slots[i],p.vertices,p.indices))return nullptr;
        S3DObj object{};image->GetObject(i,&object);image->GetMaterial(object.material,&p.material);
        // The dust's declared prelit constant is supplied per draw; geometry is immutable.
        auto vertices=p.vertices;if(i==2)for(auto& v:vertices){v.normal[0]=v.normal[1]=v.normal[2]=1;v.uv[0]=0;v.uv[1]=1;}
        p.mesh=Renderer->RegisterMeshAsset(0x4341544100000000ull^(uint64_t(uint32_t(image->ImageryId()))<<8)^uint64_t(i),vertices.data(),int(vertices.size()),p.indices.data(),int(p.indices.size()),slots[i]?image->GetTextureHandle(slots[i]-1):Renderer->WhiteTextureHandle());
        if(!p.mesh)return nullptr;Renderer->AddMeshAssetRef(p.mesh);
    }
    c->core.Initialize([](int lo,int hi){return random(lo,hi);});
    log_info("[cataclysm-original] id=%08x raw_local_z=%d pieces=30 dust=70 glows=50 null_spell=1 empty_character_world=1 source=%d,%d,%d",id,int(yellow_variant),source.x,source.y,source.z);
    log_info("[native-domain] enabled=%d camera_z=0",int(StartupVfxNativeDomain));return c.release();
}
bool IsAlive(const State* c){return c && c->owner && c->core.alive && !(c->owner->GetFlags()&OF_KILL);}
void Advance(State* c,double seconds) {
    if(!c || !std::isfinite(seconds) || seconds<=0)return;c->accumulated+=seconds;
    while(c->accumulated+1e-10>=TTime::LegacyFrameSeconds && IsAlive(c)){
        c->accumulated-=TTime::LegacyFrameSeconds;c->owner->NextFrame();c->owner->Pulse();
        c->core.Pulse([](int lo,int hi){return random(lo,hi);});++c->ticks;
        if(!c->core.alive)c->owner->SetFlags(c->owner->GetFlags()|OF_KILL);
    }
}
void SubmitWorld(State* c,EFxDebugMode) {
    if(!IsAlive(c) || !Renderer)return;
    const auto source=IntPos(c->owner);
    cataclysm_core::ForEachDraw(c->core,[&](const cataclysm_core::Draw& d){
        if(d.scale<=1e-5f)return;const auto& p=c->parts[d.object];hmm_mat4 world;cataclysm_core::Matrix(world,d,c->yellow_variant);
        // Translation relocates the explicit empty-world fixture for map review.
        world.Elements[3][0]+=source.x;world.Elements[3][1]+=source.y;world.Elements[3][2]+=RetailSoftwareOwnerZ(source.z);
        if(!StartupVfxNativeDomain)for(int col=0;col<4;++col)world.Elements[col][2]*=WORLD3D_Z_SCALE;
        SHelperMeshSubmit q{};q.mesh=p.mesh;q.additive_blend=d.mode==8;q.retail_additive_cull_none=d.mode==8;q.retail_source_alpha=d.mode==2;
        q.retail_lighting=d.object==4?3:d.object==2?4:d.object==1?6:1;
        q.retail_alpha_source_quantize=StartupVfxNativeDomain && d.object==4;
        q.retail_software_projection=StartupVfxNativeDomain;q.retail_normal_z_scale=StartupVfxNativeDomain?1.f:1.f/WORLD3D_Z_SCALE;
        const auto& m=p.material.matdesc;std::memcpy(q.diffuse,&m.diffuse,16);std::memcpy(q.ambient,&m.ambient,16);std::memcpy(q.specular,&m.specular,16);std::memcpy(q.emissive,&m.emissive,16);q.power=m.power;
        if(d.object==1){q.diffuse[0]=float((d.color>>19)&31);q.diffuse[1]=float((d.color>>11)&31);q.diffuse[2]=float((d.color>>3)&31);q.diffuse[3]=float(d.color>>27);}
        // Full original dust LVERTEX writes one packed color to every vertex.
        if(d.object==2){q.retail_prelit_rgba5=true;q.retail_prelit_color_uniform=true;q.retail_prelit_color=((d.color>>27)<<15)|(((d.color>>19)&31)<<10)|(((d.color>>11)&31)<<5)|((d.color>>3)&31);}
        for(int r=0;r<4;++r)for(int col=0;col<4;++col)q.world[r*4+col]=world.Elements[col][r];q.sort_depth=RetailSoftwareOwnerZ(source.z);
        if(d.object==2 || StartupVfxNativeDomain){
            q.index_count=3;
            for(size_t face=0;face<p.indices.size();face+=3){hmm_vec3 triangle[3];for(int i=0;i<3;++i){const auto& v=p.vertices[p.indices[face+i]];triangle[i]={v.pos[0],v.pos[1],v.pos[2]};}
                if(StartupVfxNativeDomain && Renderer->UsesRetailSoftwareMeshLighting() && !RetailCometTriangleAllowed(triangle,world))continue;
                if(d.object==2 && StartupVfxNativeDomain){q.retail_flat_depth=true;float farthest=-1e30f;for(auto v:triangle){hmm_vec3 w;MtxTransform(&world,&v,&w);const float depth=-.8748177289962769f*(w.X+w.Y)-.7142857909202576f*w.Z;if(depth>farthest)farthest=depth;}q.retail_flat_depth_offset=farthest;}
                q.index_offset=int(face);Renderer->SubmitHelperMesh(q);
            }
        }else Renderer->SubmitHelperMesh(q);
    });
    log_info("[cataclysm-phase] tick=%d life=%d phase=%d random=%llu",c->ticks,c->core.life,c->core.phase,static_cast<unsigned long long>(RandomDraws()));
}
void Destroy(State* c){delete c;}
}
