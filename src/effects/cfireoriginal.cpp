#include "cfireoriginal.h"
#include "../3dimage.h"
#include "../effect.h"
#include "../logging.h"
#include "../meshextract.h"
#include "../renderer.h"
#include "../retailsoftwaretransform.h"
#include "../cfireprofile.h"
#include "../testconfig.h"
#include "../time.h"
#include "../speedauthoredmatrix.h"
#include <cmath>
#include <cstring>
#include <memory>
namespace cfire_original {
struct State {
 std::unique_ptr<TObjectInstance> owner;
 struct Part {MeshHandle mesh=0;S3DMat material{};int object=0;};
 std::vector<Part> parts;double accumulated=0;int ticks=0;int64_t rendered=-1;
 ~State(){if(Renderer)for(auto&p:parts)if(p.mesh)Renderer->ReleaseMeshAssetRef(p.mesh);}
};
static bool ValidateCFire(T3DImagery& image)
{
    if(image.NumStates()!=1 || image.NumObjects()!=4 || image.NumTextures()!=2 ||
       image.NumMaterials()!=3 || image.NumTags()!=2 || image.GetAniLength(0)!=30 || image.GetAniFlags(0)!=8192)return false;
    const auto hash=[](const void* data,size_t n){uint64_t h=1469598103934665603ull;
        const auto* b=static_cast<const uint8_t*>(data);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;};
    for(int i=0;i<4;++i){S3DObj o{};image.GetObject(i,&o);const auto& p=retail_cfire::objects[i];
        if(std::strcmp(o.name,p.name) || o.material!=p.material || o.numverts!=p.vertices || o.numfaces!=p.faces)return false;
        for(int st=0;st<1;++st)if(o.parent[st]!=-1 || !o.anikeys[st] || o.numanikeys[st]!=p.keys || hash(o.anikeys[st],p.keys*sizeof(SAniKey32))!=p.key_hash)return false;
        for(int slot=0;slot<3;++slot)if(o.numtexfaces[slot]!=p.bins[slot])return false;
        std::vector<S3DVertex> v(size_t(p.vertices));std::vector<S3DFace> f(size_t(p.faces));image.GetObjVerts(i,v.data(),0,0);image.GetObjFaces(i,f.data());
        if(hash(v.data(),v.size()*sizeof(S3DVertex))!=p.vertex_hash || hash(f.data(),f.size()*sizeof(S3DFace))!=p.face_hash)return false;
    }
    for(int i=0;i<2;++i){const auto* tag=image.GetTag(i);if(!tag || tag->state || tag->frame!=(i?9:2) || !tag->name || !tag->str || std::strcmp(tag->name,"partsys") || std::strcmp(tag->str,i?"filename csparks":"filename cfire"))return false;}
    for(int i=0;i<3;++i){S3DMat m{};image.GetMaterial(i,&m);if(m.texture!=retail_cfire::material_textures[i] || hash(&m.matdesc.diffuse,17*sizeof(float))!=retail_cfire::materials[i])return false;}
    for(int i=0;i<2;++i){S3DTex t{};image.GetTexture(i,&t);const auto& f=t.desc.pixelFormat;
        if(t.numframes!=1 || t.desc.width!=(i?32:64) || t.desc.height!=(i?32:64) || f.dwRGBBitCount!=16 || f.dwRBitMask!=0xf00 || f.dwGBitMask!=0xf0 || f.dwBBitMask!=15 || f.dwRGBAlphaBitMask!=0xf000)return false;}
    return true;
}
State* Spawn(const S3DPoint& source)
{
 if(!Renderer)return nullptr;auto c=std::make_unique<State>();
 auto* cls=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cls?cls->FindObjType(0xad92bd27u):-1;if(type<0)return nullptr;
 SObjectDef def{};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos=source;c->owner.reset(cls->NewObject(&def));auto* o=c->owner.get();
 if(!o || o->ObjId()!=0xad92bd27u || !dynamic_cast<TEffect*>(o))return nullptr;
 o->ForcePos(source);o->Face(0);o->OnScreen();auto* image=dynamic_cast<T3DImagery*>(o->GetImagery());
 if(!image || !dynamic_cast<T3DAnimator*>(o->GetAnimator()) || !ValidateCFire(*image))return nullptr;
 // Actual installed filename opens both fail before the prototypes are hidden.
 // Keep only those literal base meshes; no replacement particle definition.
 for(int slot=1;slot<=2;++slot){const int object=(slot-1)*2;std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;
  if(!ExtractSubMeshTextureSlot(image,object,slot,vertices,indices))return nullptr;
  State::Part part;part.object=object;S3DObj obj{};image->GetObject(object,&obj);image->GetMaterial(obj.material,&part.material);
  const uint64_t key=0x4346495200000000ull^(uint64_t(uint32_t(image->ImageryId()))<<16)^uint64_t(slot);
  part.mesh=Renderer->RegisterMeshAsset(key,vertices.data(),int(vertices.size()),indices.data(),int(indices.size()),image->GetTextureHandle(slot-1));
  if(!part.mesh)return nullptr;Renderer->AddMeshAssetRef(part.mesh);c->parts.push_back(part);
 }
 log_info("[cfire-installed] id=ad92bd27 frames=30 base_draws=2 missing_presets=cfire,csparks");
 log_info("[native-domain] enabled=%d camera_z=0",int(StartupVfxNativeDomain));return c.release();
}
void Advance(State* c,double seconds)
{
 if(!c || !c->owner || !std::isfinite(seconds) || seconds<=0)return;c->accumulated+=seconds;
 while(c->accumulated+1e-10>=TTime::LegacyFrameSeconds && !(c->owner->GetFlags()&OF_KILL)){
  c->accumulated-=TTime::LegacyFrameSeconds;c->owner->NextFrame();c->owner->Pulse();++c->ticks;
 }
}
void SubmitWorld(State* c,EFxDebugMode)
{
 if(!c || !c->owner || !Renderer || (c->owner->GetFlags()&OF_KILL) || c->rendered==TTime::FrameCount())return;c->rendered=TTime::FrameCount();
 auto* o=c->owner.get();auto* image=static_cast<T3DImagery*>(o->GetImagery());hmm_mat4 owner=o->Transform().Matrix();
 if(StartupVfxNativeDomain){const auto p=o->Pos();BuildRetailSoftwareOwner(owner,p.x,p.y,p.z,uint8_t(o->GetFace()));}
 for(const auto& part:c->parts){hmm_vec3 p{},r{},s{};if(!image->GetUninterpolatedAniKey(part.object,0,o->GetFrame(),p,r,s,true))continue;
  hmm_mat4 local,world;BuildRetailSpeedCameraMatrix(local,p,r,s);MtxMultiply(&world,&local,&owner);
  // Original full Render: SRCALPHA/INVSRCALPHA, CULL_NONE, Z test without writes.
  SHelperMeshSubmit q{};q.mesh=part.mesh;q.retail_lighting=3;q.retail_source_alpha=true;
  q.retail_alpha_source_quantize=true;q.retail_software_projection=StartupVfxNativeDomain;
  for(int row=0;row<4;++row)for(int col=0;col<4;++col)q.world[row*4+col]=world.Elements[col][row];
  const auto&m=part.material.matdesc;std::memcpy(q.diffuse,&m.diffuse,16);std::memcpy(q.ambient,&m.ambient,16);std::memcpy(q.specular,&m.specular,16);std::memcpy(q.emissive,&m.emissive,16);q.power=m.power;q.sort_depth=world.Elements[3][2];Renderer->SubmitHelperMesh(q);
 }
}
bool IsAlive(const State* c){return c && c->owner && !(c->owner->GetFlags()&OF_KILL);}
void Destroy(State* c){delete c;}
}
