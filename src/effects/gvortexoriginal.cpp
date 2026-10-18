#include "gvortexoriginal.h"
#include "../3dimage.h"
#include "../effect.h"
#include "../logging.h"
#include "../meshextract.h"
#include "../renderer.h"
#include "../retailsoftwaretransform.h"
#include "../gvortexprofile.h"
#include "../gvortexmatrix.h"
#include "../cometsoftwarebounds.h"
#include <array>
#include "../testconfig.h"
#include "../time.h"
#include <cmath>
#include <cstring>
#include <memory>

namespace gvortex_original {
struct State {
    std::unique_ptr<TObjectInstance> owner;
    struct Part {MeshHandle mesh=0;S3DMat material{};int object=0,slot=0;std::vector<std::array<hmm_vec3,3>> triangles;};
    std::vector<Part> parts;
    S3DPoint source{};
    double accumulated=0;
    ~State(){if(Renderer)for(auto& p:parts)if(p.mesh)Renderer->ReleaseMeshAssetRef(p.mesh);}

};
static bool ValidateGVortex(T3DImagery& image)
{
    if(image.NumStates()!=1 || image.NumObjects()!=12 || image.NumTextures()!=3 ||
       image.NumMaterials()!=3 || image.NumTags()!=2 || image.GetAniLength(0)!=240 || image.GetAniFlags(0)!=8192)return false;
    const auto hash=[](const void* data,size_t n){uint64_t h=1469598103934665603ull;
        const auto* b=static_cast<const uint8_t*>(data);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;};
    for(int i=0;i<12;++i){S3DObj o{};image.GetObject(i,&o);const auto& p=retail_gvortex::objects[i];
        if(std::strcmp(o.name,p.name) || o.material!=p.material || o.numverts!=p.vertices || o.numfaces!=p.faces)return false;
        for(int st=0;st<1;++st)if(o.parent[st]!=-1 || !o.anikeys[st] || o.numanikeys[st]!=p.keys[st] || hash(o.anikeys[st],p.keys[st]*sizeof(SAniKey32))!=p.key_hash[st])return false;
        for(int slot=0;slot<4;++slot)if(o.numtexfaces[slot]!=p.face_counts[slot])return false;
        std::vector<S3DVertex> v(size_t(p.vertices));std::vector<S3DFace> f(size_t(p.faces));image.GetObjVerts(i,v.data(),0,0);image.GetObjFaces(i,f.data());
        if(hash(v.data(),v.size()*sizeof(S3DVertex))!=p.vertex_hash || hash(f.data(),f.size()*sizeof(S3DFace))!=p.face_hash)return false;
    }
    for(int i=0;i<2;++i){const auto* tag=image.GetTag(i);if(!tag || tag->state!=0 || tag->frame!=(i==0?1:2) || !tag->name || !tag->str || std::strcmp(tag->name,i==0?"play":"blendcont") || std::strcmp(tag->str,i==0?"gvortex":"litadd"))return false;}
    for(int i=0;i<3;++i){S3DMat m{};image.GetMaterial(i,&m);if(m.texture!=retail_gvortex::material_textures[i] || hash(&m.matdesc.diffuse,17*sizeof(float))!=retail_gvortex::materials[i])return false;
        S3DTex t{};image.GetTexture(i,&t);const auto& f=t.desc.pixelFormat;const int size=i==0?64:128;
        if(t.numframes!=1 || t.desc.width!=size || t.desc.height!=size || f.dwRGBBitCount!=16 || f.dwRBitMask!=0xf800 || f.dwGBitMask!=0x7e0 || f.dwBBitMask!=31 || f.dwRGBAlphaBitMask)return false;
    }
    return true;
}
State* Spawn(const S3DPoint& source)
{
    if(!Renderer)return nullptr;
    auto c=std::make_unique<State>();c->source=source;
    auto* cls=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cls?cls->FindObjType(0xad99bd33u):-1;
    if(type<0)return nullptr;
    SObjectDef def{};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos=source;
    c->owner.reset(cls->NewObject(&def));auto* owner=c->owner.get();
    if(!owner || owner->ObjId()!=0xad99bd33u || !dynamic_cast<TEffect*>(owner))return nullptr;
    // Controlled coordinates can lie outside loaded map sectors. Establish
    // the explicit fixture transform before creating the animator/controllers.
    SetIntPos(owner, source);owner->Face(0);owner->OnScreen();
    auto* image=dynamic_cast<T3DImagery*>(owner->GetImagery());auto* animator=dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    if(!image || !animator || !ValidateGVortex(*image))return nullptr;
    // Original 40e8d0 renders controllers first, then texture-major objects.
    for(int slot=1;slot<=3;++slot)for(int object=0;object<12;++object){
        std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;
        if(!ExtractSubMeshTextureSlot(image,object,slot,vertices,indices))continue;
        State::Part part;part.object=object;part.slot=slot;S3DObj obj{};
        image->GetObject(object,&obj);image->GetMaterial(obj.material,&part.material);
        for(size_t face=0;face<indices.size();face+=3){
            std::array<hmm_vec3,3> triangle{};
            for(int corner=0;corner<3;++corner){const auto& v=vertices[indices[face+corner]];triangle[corner]={v.pos[0],v.pos[1],v.pos[2]};}
            part.triangles.push_back(triangle);
        }
        const uint64_t key=0x47564f5200000000ull^(uint64_t(uint32_t(image->ImageryId()))<<16)^uint64_t(object*32+slot*4);
        part.mesh=Renderer->RegisterMeshAsset(key,vertices.data(),int(vertices.size()),indices.data(),int(indices.size()),image->GetTextureHandle(slot-1));
        if(!part.mesh)return nullptr;
        Renderer->AddMeshAssetRef(part.mesh);c->parts.push_back(part);
    }
    if(c->parts.size()!=12)return nullptr;
    owner->Animate(false);
    log_info("[gvortex-original] id=ad99bd33 frames=240 objects=12 source=%d,%d,%d native=%d",source.x,source.y,source.z,int(StartupVfxNativeDomain));
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
        hmm_mat4 local;BuildRetailGVortexMatrix(local,part.object,p,r,s);
        hmm_mat4 world;MtxMultiply(&world,&local,&owner);
        // Executed litadd mode16; four authored $ flares retain mode80.
        SHelperMeshSubmit q{};q.mesh=part.mesh;q.additive_blend=true;q.retail_lighting=1;q.retail_gold_no_depth=part.object>=8;
        q.retail_software_projection=StartupVfxNativeDomain;q.retail_normal_z_scale=StartupVfxNativeDomain?1.0f:1.0f/WORLD3D_Z_SCALE;
        for(int row=0;row<4;++row)for(int col=0;col<4;++col)q.world[row*4+col]=world.Elements[col][row];
        const auto& m=part.material.matdesc;std::memcpy(q.diffuse,&m.diffuse,16);std::memcpy(q.ambient,&m.ambient,16);std::memcpy(q.specular,&m.specular,16);std::memcpy(q.emissive,&m.emissive,16);q.power=m.power;q.sort_depth=world.Elements[3][2];
        if(StartupVfxNativeDomain && Renderer->UsesRetailSoftwareMeshLighting()){
            // Shared independently proven original software guard. This asset's
            // own phase39 widening control is unchanged. Keep face runs in
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
bool IsAlive(const State* c)
{
    return c && c->owner && !(c->owner->GetFlags()&OF_KILL);
}
void Destroy(State* c){delete c;}
}
