#include "game/effects/legacy/dragonfirepreview.h"
#include "game/effects/effect.h"
#include "game/effects/dragonfirepose.h"
#include "engine/render/framesnap.h"
#include "engine/core/logging.h"
#include "game/imagery/meshextract.h"
#include "engine/render/retailsoftwaretransform.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include <memory>
#include <cstring>
namespace dragonfire_preview {
struct State {
 TObjectPtr<TObjectInstance> owner;retail_dragonfire::State core;MeshHandle meshes[2]{};S3DMat materials[2]{};
 double accumulator=0;int ticks=0;int64_t rendered=-1;uint64_t initial_draws=0;
 ~State(){log_info("[dragonfire-cleanup] tick=%d alive=%d",ticks,int(core.alive));if(Renderer)for(auto m:meshes)if(m)Renderer->ReleaseMeshAssetRef(m);}
};
static uint64_t Hash(const void* data,size_t n,uint64_t h=1469598103934665603ull){const auto* b=static_cast<const uint8_t*>(data);for(size_t i=0;i<n;++i)h=(h^b[i])*1099511628211ull;return h;}
static uint64_t PoolHash(const retail_dragonfire::Pool& pool){uint64_t h=1469598103934665603ull;for(const auto& p:pool.particles){h=Hash(&p.used,4,h);if(p.used){h=Hash(&p,72,h);h=Hash(&p.life,8,h);}}return h;}
static void Log(State* s){log_info("[dragonfire-phase] tick=%d frame=%d alive=%d rng=%llu fire=%llx smoke=%llx",s->ticks,s->core.frame,int(s->core.alive),static_cast<unsigned long long>(RandomDraws()-s->initial_draws),static_cast<unsigned long long>(PoolHash(s->core.fire)),static_cast<unsigned long long>(PoolHash(s->core.smoke)));}
State* Spawn(const S3DPoint& origin){
 if(!Renderer)return nullptr;auto s=std::make_unique<State>();auto* cl=TObjectClass::GetClass(OBJCLASS_EFFECT);int type=cl?cl->FindObjType(0xad92bc1au):-1;if(type<0)return nullptr;
 SObjectState d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));if(!s->owner || s->owner->ObjId()!=0xad92bc1au)return nullptr;
 auto* img=dynamic_cast<T3DImagery*>(s->owner->GetImagery());if(!img || img->NumObjects()!=2 || img->NumTextures()!=2 || img->NumMaterials()!=2 || img->NumStates()!=1 || img->NumTags()!=0)return nullptr;
 for(int i=0;i<2;++i){S3DObj o={};img->GetObject(i,&o);if(o.numverts!=4 || o.numfaces!=2 || o.material!=i || o.parent[0]!=-1)return nullptr;
  S3DVertex raw[4];S3DFace faces[2];img->GetObjVerts(i,raw,0,0);img->GetObjFaces(i,faces);if(Hash(raw,sizeof(raw))!=0x8d7ec6f02e765863ull || Hash(faces,sizeof(faces))!=0xf44208a16a617ed8ull)return nullptr;
  S3DTex t={};img->GetTexture(i,&t);const auto& pf=t.desc.pixelFormat;if(t.numframes!=1 || t.desc.width!=64 || t.desc.height!=64 || pf.dwRGBBitCount!=16 || pf.dwRBitMask!=0xf800 || pf.dwGBitMask!=0x7e0 || pf.dwBBitMask!=31 || pf.dwRGBAlphaBitMask || img->GetTextureHandle(i)==kInvalidTexture)return nullptr;
  std::vector<SMeshVertex> verts;std::vector<uint16_t> indices;if(!ExtractSubMeshTextureSlot(img,i,i+1,verts,indices) || verts.size()!=4 || indices.size()!=6)return nullptr;img->GetMaterial(i,&s->materials[i]);
  s->meshes[i]=Renderer->RegisterMeshAsset(0x4452474649520000ull+i,verts.data(),4,indices.data(),6,img->GetTextureHandle(i));if(!s->meshes[i])return nullptr;Renderer->AddMeshAssetRef(s->meshes[i]);
 }
 s->initial_draws=RandomDraws();Log(s.get());log_info("[dragonfire] exact_id=ad92bc1a fire=300 smoke=300 null_spell=1 relative=1 emission=34");log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT",int(StartupVfxNativeDomain));return s.release();
}
bool Alive(State* s){return s && s->core.alive && !(s->owner->Flags()&OF_KILL);}
void Advance(State* s,double dt){if(!s || !std::isfinite(dt) || dt<=0)return;s->accumulator+=dt;while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;if(!Alive(s))return;s->owner->NextFrame();s->owner->Pulse();if(!Alive(s))return;++s->ticks;s->core.Advance([](int lo,int hi){return random(lo,hi);});if(!s->core.alive)s->owner->SetFlags(s->owner->Flags()|OF_KILL);Log(s);}}
void SubmitWorld(State* s){
 if(!Alive(s) || !Renderer || s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();hmm_mat4 root;const auto& p=IntPos(s->owner);if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,p.x,p.y,p.z,uint8_t(IntFacing(s->owner)));else root=s->owner->Transform().Matrix();
 const float facing=float(-double(IntFacing(s->owner))*360.0/256.0*0.017453292519943295);
 for(int i=0;i<2;++i)for(const auto& particle:(i?s->core.smoke:s->core.fire).particles)if(particle.used){hmm_mat4 local=retail_dragonfire::LocalMatrix(particle,facing),world;MtxMultiply(&world,&local,&root);
  SHelperMeshSubmit q={};q.mesh=s->meshes[i];q.additive_blend=true;q.retail_lighting=1;q.retail_software_projection=StartupVfxNativeDomain;
  const auto& m=s->materials[i].matdesc;std::memcpy(q.diffuse,&m.diffuse,16);std::memcpy(q.ambient,&m.ambient,16);std::memcpy(q.specular,&m.specular,16);std::memcpy(q.emissive,&m.emissive,16);q.power=m.power;
  for(int r=0;r<4;++r)for(int c=0;c<4;++c)q.world[r*4+c]=world.Elements[c][r];Renderer->SubmitHelperMesh(q);
 }
}
void Destroy(State* s){delete s;}
}
