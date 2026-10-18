#include "game/effects/legacy/ceyesoriginal.h"
#include "game/imagery/3dimage.h"
#include "game/effects/effect.h"
#include "engine/core/logging.h"
#include "game/imagery/meshextract.h"
#include "engine/render/renderer.h"
#include "engine/render/retailsoftwaretransform.h"
#include "game/effects/ceyesprofile.h"
#include "game/effects/ceyesmatrix.h"
#include "game/effects/cometsoftwarebounds.h"
#include <array>
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include <cmath>
#include <cstring>
#include <memory>

namespace ceyes_original {
struct State {
    TObjectPtr<TObjectInstance> owner;
    struct Part {MeshHandle mesh=0;S3DMat material{};int object=0,slot=0;std::vector<std::array<hmm_vec3,3>> triangles;};
    std::vector<Part> parts;
    S3DPoint source{};
    double accumulated=0;
    ~State(){if(Renderer)for(auto& p:parts)if(p.mesh)Renderer->ReleaseMeshAssetRef(p.mesh);}

};
static bool ValidateCEyes(T3DImagery& image)
{
    if(image.NumStates()!=9 || image.NumObjects()!=4 || image.NumTextures()!=1 ||
       image.NumMaterials()!=1 || image.NumTags()!=1)return false;
    const int lengths[]={120,1,1,89,1,78,1,109,1};
    for(int st=0;st<9;++st)if(image.GetAniLength(st)!=lengths[st] || image.GetAniFlags(st)!=((st==0 || st==3 || st==5 || st==7)?8193:8192))return false;
    const auto hash=[](const void* data,size_t n){uint64_t h=1469598103934665603ull;
        const auto* b=static_cast<const uint8_t*>(data);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;};
    for(int i=0;i<4;++i){S3DObj o{};image.GetObject(i,&o);const auto& p=retail_ceyes::objects[i];
        if(std::strcmp(o.name,p.name) || o.material!=p.material || o.numverts!=p.vertices || o.numfaces!=p.faces)return false;
        for(int st=0;st<9;++st)if(o.parent[st]!=-1 || !o.anikeys[st] || o.numanikeys[st]!=p.keys[st] || hash(o.anikeys[st],p.keys[st]*sizeof(SAniKey32))!=p.key_hash[st])return false;
        for(int slot=0;slot<2;++slot)if(o.numtexfaces[slot]!=p.face_counts[slot])return false;
        std::vector<S3DVertex> v(size_t(p.vertices));std::vector<S3DFace> f(size_t(p.faces));image.GetObjVerts(i,v.data(),0,0);image.GetObjFaces(i,f.data());
        if(hash(v.data(),v.size()*sizeof(S3DVertex))!=p.vertex_hash || hash(f.data(),f.size()*sizeof(S3DFace))!=p.face_hash)return false;
    }
    const auto* tag=image.GetTag(0);if(!tag || tag->state!=0 || tag->frame!=15 || !tag->name || !tag->str || std::strcmp(tag->name,"blendcont") || std::strcmp(tag->str,"litadd"))return false;
    S3DMat m{};image.GetMaterial(0,&m);if(m.texture!=retail_ceyes::material_textures[0] || hash(&m.matdesc.diffuse,17*sizeof(float))!=retail_ceyes::materials[0])return false;
    S3DTex t{};image.GetTexture(0,&t);const auto& f=t.desc.pixelFormat;
    if(t.numframes!=1 || t.desc.width!=32 || t.desc.height!=32 || f.dwRGBBitCount!=16 || f.dwRBitMask!=0xf800 || f.dwGBitMask!=0x7e0 || f.dwBBitMask!=31 || f.dwRGBAlphaBitMask)return false;
    return true;
}
State* Spawn(const S3DPoint& source,int initial_state)
{
    if(!Renderer || initial_state<0 || initial_state>=9)return nullptr;
    auto c=std::make_unique<State>();c->source=source;
    auto* cls=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cls?cls->FindObjType(0x0c052639u):-1;
    if(type<0)return nullptr;
    SObjectDef def{};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos=source;
    c->owner.reset(cls->NewObject(&def));auto* owner=c->owner.get();
    if(!owner || owner->ObjId()!=0x0c052639u || !dynamic_cast<TEffect*>(owner))return nullptr;
    // Controlled coordinates can lie outside loaded map sectors. Establish
    // the explicit fixture transform before creating the animator/controllers.
    SetIntPos(owner, source);owner->Face(0);owner->OnScreen();
    auto* image=dynamic_cast<T3DImagery*>(owner->GetImagery());auto* animator=dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    if(!image || !animator || !ValidateCEyes(*image))return nullptr;
    // Original 40e8d0 renders controllers first, then texture-major objects.
    for(int slot=1;slot<=1;++slot)for(int object=0;object<4;++object){
        std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;
        if(!ExtractSubMeshTextureSlot(image,object,slot,vertices,indices))continue;
        // Actual56e4a0..56e4f0 truncates normalized UV times texture size.
        // Keep authored UVs in the ordinary/common rendering policy.
        if(StartupVfxNativeDomain && Renderer->UsesRetailSoftwareMeshLighting())
            for(auto& v:vertices)for(float& uv:v.uv)uv=std::trunc(uv*32.0f)/32.0f;
        State::Part part;part.object=object;part.slot=slot;S3DObj obj{};
        image->GetObject(object,&obj);image->GetMaterial(obj.material,&part.material);
        for(size_t face=0;face<indices.size();face+=3){
            std::array<hmm_vec3,3> triangle{};
            for(int corner=0;corner<3;++corner){const auto& v=vertices[indices[face+corner]];triangle[corner]={v.pos[0],v.pos[1],v.pos[2]};}
            part.triangles.push_back(triangle);
        }
        const uint64_t key=(0x4345594500000000ull ^ (StartupVfxNativeDomain && Renderer->UsesRetailSoftwareMeshLighting() ? 0x100000000ull : 0))^(uint64_t(uint32_t(image->ImageryId()))<<16)^uint64_t(object*32+slot*4);
        part.mesh=Renderer->RegisterMeshAsset(key,vertices.data(),int(vertices.size()),indices.data(),int(indices.size()),image->GetTextureHandle(slot-1));
        if(!part.mesh)return nullptr;
        Renderer->AddMeshAssetRef(part.mesh);c->parts.push_back(part);
    }
    if(c->parts.size()!=4)return nullptr;
    if(initial_state)owner->SetState(initial_state);
    owner->Animate(false);
    log_info("[ceyes-original] id=0c052639 frames=120 loop=1 objects=4 source=%d,%d,%d native=%d",source.x,source.y,source.z,int(StartupVfxNativeDomain));
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
    hmm_mat4 owner=o->Transform().Matrix();if(StartupVfxNativeDomain){const auto p=IntPos(o);BuildRetailSoftwareOwner(owner,p.x,p.y,p.z,uint8_t(IntFacing(o)));}
    for(const auto& part:c->parts){
        hmm_vec3 p{},r{},s{};if(!image->GetUninterpolatedAniKey(part.object,o->GetState(),o->GetFrame(),p,r,s,true) || s.X<=1e-5f || s.Y<=1e-5f || s.Z<=1e-5f)continue;
        hmm_mat4 local;BuildRetailCEyesMatrix(local,part.object,p,r,s);
        hmm_mat4 world;MtxMultiply(&world,&local,&owner);
        // Executed litadd mode16: CULL_NONE, depth test enabled, no depth write.
        SHelperMeshSubmit q{};q.mesh=part.mesh;q.additive_blend=true;q.retail_lighting=1;
        q.retail_software_pixel_grid=StartupVfxNativeDomain;
        q.retail_software_projection=StartupVfxNativeDomain;q.retail_normal_z_scale=StartupVfxNativeDomain?1.0f:1.0f/WORLD3D_Z_SCALE;
        for(int row=0;row<4;++row)for(int col=0;col<4;++col)q.world[row*4+col]=world.Elements[col][row];
        const auto& m=part.material.matdesc;std::memcpy(q.diffuse,&m.diffuse,16);std::memcpy(q.ambient,&m.ambient,16);std::memcpy(q.specular,&m.specular,16);std::memcpy(q.emissive,&m.emissive,16);q.power=m.power;q.sort_depth=world.Elements[3][2];
        if(StartupVfxNativeDomain && Renderer->UsesRetailSoftwareMeshLighting()){
            // Exact original software guard, independently exercised by this
            // asset's camera quads. Keep contiguous accepted face runs in
            // authored order; common/hardware retains the complete mesh.
            int first=-1;
            for(size_t face=0;face<=part.triangles.size();++face){
                const bool allowed=face<part.triangles.size() && RetailCometTriangleAllowed(part.triangles[face].data(),world);
                if(allowed && first<0)first=int(face);
                if(!allowed && first>=0){q.index_offset=first*3;q.index_count=(int(face)-first)*3;Renderer->SubmitHelperMesh(q);first=-1;}
            }
        }else Renderer->SubmitHelperMesh(q);
    }
}
bool SetState(State* c,int state)
{
    return c && c->owner && state>=0 && state<9 && c->owner->SetState(state);
}
bool IsAlive(const State* c)
{
    return c && c->owner && !(c->owner->GetFlags()&OF_KILL);
}
void Destroy(State* c){delete c;}
}
