#include "pulppreview.h"
#include "../effect.h"
#include "../character.h"
#include "../mappane.h"
#include "../framesnap.h"
#include "../pulppose.h"
#include "../pulpprofile.h"
#include "../meshextract.h"
#include "../testconfig.h"
#include "../time.h"
#include "../logging.h"
#include <memory>
#include <cstring>
namespace pulp_preview {
struct State {
 TObjectPtr<TObjectInstance> owner;TObjectPtr<TCharacter> actor;pulp_core::State core;
 MeshHandle meshes[11]{};S3DMat materials[11]{};S3DPoint display={};double accumulator=0;uint64_t initial_draws=0;int64_t rendered=-1;uint32_t actor_flags=0;bool context_valid=true;int decal_requests=0;
 ~State(){if(actor){actor->SetFlag(OF_INVISIBLE,(actor_flags&OF_INVISIBLE)!=0);log_info("[pulp-cleanup] tick=%d flags=%x health=%d decals=%d",core.tick,actor->Flags(),actor->Health(),decal_requests);}if(Renderer)for(auto mesh:meshes)if(mesh)Renderer->ReleaseMeshAssetRef(mesh);}
};
static uint64_t Hash(const void* p,size_t n){uint64_t h=1469598103934665603ull;const auto* b=static_cast<const uint8_t*>(p);for(size_t i=0;i<n;++i)h=(h^b[i])*1099511628211ull;return h;}
static void LogState(State* s){log_info("[pulp-state] tick=%d body=%llx blood=%llx",s->core.tick,static_cast<unsigned long long>(Hash(s->core.bodies.data(),sizeof(s->core.bodies))),static_cast<unsigned long long>(Hash(s->core.blood.data(),sizeof(s->core.blood))));}
static int Ground(State* s,pulp_core::Point p){const int height=MapPane.GetWalkHeight({p.x,p.y,p.z});if(height){log_error("[pulp] unsupported nonzero terrain=%d at%d,%d,%d",height,p.x,p.y,p.z);s->context_valid=false;}return height;}
static State* Spawn(const S3DPoint& origin,bool isolated){
 if(!Renderer)return nullptr;auto s=std::make_unique<State>();s->display=origin;
 auto* effects=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=effects?effects->FindObjType(0x152dafddu):-1;if(type<0)return nullptr;
 SObjectDef def={};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos={10000,10000,16};s->owner.reset(effects->NewObject(&def));if(!s->owner || s->owner->ObjId()!=0x152dafddu)return nullptr;
 auto* image=dynamic_cast<T3DImagery*>(s->owner->GetImagery());if(!image || image->NumObjects()!=11 || image->NumStates()!=1 || image->GetAniLength(0)!=1 || image->NumMaterials()!=4 || image->NumTextures()!=4 || image->NumTags()!=0)return nullptr;
 for(int i=0;i<4;++i){S3DTex t={};image->GetTexture(i,&t);const auto& f=t.desc.pixelFormat;if(t.numframes!=1 || t.desc.width!=uint32_t(i==1?16:32) || t.desc.height!=t.desc.width || f.dwRGBBitCount!=16 || f.dwRBitMask!=0xf800 || f.dwGBitMask!=0x7e0 || f.dwBBitMask!=31 || f.dwRGBAlphaBitMask || image->GetTextureHandle(i)==kInvalidTexture)return nullptr;}
 for(int i=0;i<11;++i){const auto& p=pulp_profile::objects[i];S3DObj o={};image->GetObject(i,&o);if(std::strcmp(o.name,p.name) || o.material!=p.material || o.numverts!=p.vertices || o.numfaces!=p.faces || o.parent[0]!=-1)return nullptr;
  std::vector<S3DVertex> raw(size_t(p.vertices));std::vector<S3DFace> faces(size_t(p.faces));image->GetObjVerts(i,raw.data(),0,0);image->GetObjFaces(i,faces.data());if(Hash(raw.data(),raw.size()*sizeof(S3DVertex))!=p.vertex_hash || Hash(faces.data(),faces.size()*sizeof(S3DFace))!=p.face_hash)return nullptr;
  std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;if(!ExtractSubMeshTextureSlot(image,i,p.slot,vertices,indices) || indices.size()!=size_t(p.faces*3))return nullptr;image->GetMaterial(o.material,&s->materials[i]);
  s->meshes[i]=Renderer->RegisterMeshAsset(0x50554c5000000000ull+i,vertices.data(),int(vertices.size()),indices.data(),int(indices.size()),image->GetTextureHandle(p.slot-1));if(!s->meshes[i])return nullptr;Renderer->AddMeshAssetRef(s->meshes[i]);
 }
 auto* chars=TObjectClass::GetClass(OBJCLASS_CHARACTER);const int actor_type=chars?chars->FindObjType("Acolyte"):-1;if(actor_type<0)return nullptr;
 SObjectDef character={};character.objclass=OBJCLASS_CHARACTER;character.objtype=short(actor_type);character.pos={10000,10000,16};s->actor.reset(dynamic_cast<TCharacter*>(chars->NewObject(&character)));if(!s->actor || !s->actor->CreateAnimator())return nullptr;
 auto* animator=dynamic_cast<T3DAnimator*>(s->actor->GetAnimator());if(!animator || animator->NumObjects()!=22)return nullptr;s->actor_flags=s->actor->Flags();
 const auto& snap=FrameSnap::GetConfig();if(isolated && snap.seed_set)SeedRandom(snap.seed);s->initial_draws=RandomDraws();
 s->core.Initialize({0,0,8},{10000,10000,16},animator->NumObjects(),[](int a,int b){return random(a,b);},[&](int i){const auto* object=animator->GetObject(i);return pulp_core::Vec{object->rot.X,object->rot.Y,object->rot.Z};},[&](auto p){return Ground(s.get(),p);});
 if(!s->core.valid || !s->context_valid)return nullptr;s->actor->SetFlag(OF_INVISIBLE);LogState(s.get());
 log_info("[pulp-rng] seeded=%d seed=%u initial_draws=%llu intrinsic_map_clock=0",int(isolated && snap.seed_set),snap.seed,static_cast<unsigned long long>(RandomDraws()-s->initial_draws));
 log_info("[pulp] exact_id=152dafdd actor=Acolyte objects=22 public_velocity=0,0,8 counts=5,5,5 persistent=1");log_info("[native-domain] enabled=%d camera_z=0 owner=absolute_float",int(StartupVfxNativeDomain));return s.release();
}
State* SpawnActorFixture(const S3DPoint& o){return Spawn(o,true);}State* SpawnConfiguredActor(const S3DPoint& o){return Spawn(o,false);}
bool Alive(State* s){return s && s->context_valid && s->core.valid && !(s->owner->Flags()&OF_KILL);}
void Advance(State* s,double dt){
 if(!s || !std::isfinite(dt) || dt<=0)return;s->accumulator+=dt;
 while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;if(!Alive(s))return;s->owner->NextFrame();s->core.Advance({10000,10000,16},[](int a,int b){return random(a,b);},[&](auto p){return Ground(s,p);},[&](bool settled,int variant,auto p){
   char name[24];std::snprintf(name,sizeof(name),settled?"Blood%d":"BloodS%d",variant);auto* tiles=TObjectClass::GetClass(OBJCLASS_TILE);const int type=tiles?tiles->FindObjType(name):-1;if(type<0){s->context_valid=false;log_error("[pulp] missing original tile%s",name);return;}
   SObjectDef def={};def.objclass=OBJCLASS_TILE;def.objtype=short(type);def.pos={p.x,p.y,p.z};def.level=uint16_t(MapPane.GetMapLevel());const auto before=RandomDraws();const int index=MapPane.NewObject(&def);++s->decal_requests;
   if(index!=-1 || RandomDraws()!=before){s->context_valid=false;log_error("[pulp] unsupported decal result=%d constructor_rng=%llu",index,static_cast<unsigned long long>(RandomDraws()-before));return;}
   // Original44ce30 MakeIndex does these operations even when AddObject fails.
   // Keep modern global IDs unchanged; this explicit fixture declares clock0.
   const uint32_t first=uint32_t(GameRand());SeedRandom(first);(void)GameRand();
   log_info("[pulp-decal] tick=%d tile=%s result=-1 intrinsic_reseed=1 clock_ms=0",s->core.tick,name);
  });LogState(s);log_info("[pulp-phase] tick=%d rng=%llu decals=%d valid=%d",s->core.tick,static_cast<unsigned long long>(RandomDraws()-s->initial_draws),s->decal_requests,int(Alive(s)));}
}
void SubmitWorld(State* s){
 if(!Alive(s) || !Renderer || s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();
 pulp_core::ForEachDraw(s->core,[&](int object,hmm_mat4 m){m.Elements[3][0]+=s->display.x-10000;m.Elements[3][1]+=s->display.y-10000;m.Elements[3][2]+=s->display.z-16;
  if(!StartupVfxNativeDomain)for(int c=0;c<4;++c)m.Elements[c][2]*=WORLD3D_Z_SCALE;
  SHelperMeshSubmit q={};q.mesh=s->meshes[object];q.retail_lighting=1;q.retail_opaque=true;q.retail_positive_face_cull=true;q.retail_software_projection=StartupVfxNativeDomain;q.retail_normal_z_scale=StartupVfxNativeDomain?1.f:1.f/WORLD3D_Z_SCALE;
  const auto& material=s->materials[object].matdesc;std::memcpy(q.diffuse,&material.diffuse,16);std::memcpy(q.ambient,&material.ambient,16);std::memcpy(q.specular,&material.specular,16);std::memcpy(q.emissive,&material.emissive,16);q.power=material.power;
  for(int r=0;r<4;++r)for(int c=0;c<4;++c)q.world[r*4+c]=m.Elements[c][r];Renderer->SubmitHelperMesh(q);
 });
}
void Destroy(State* s){delete s;}
}
