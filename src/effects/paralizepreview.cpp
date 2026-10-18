#include "paralizepreview.h"
#include "../effect.h"
#include "../character.h"
#include "../framesnap.h"
#include "../paralizecore.h"
#include "../meshextract.h"
#include "../retailsoftwaretransform.h"
#include "../logging.h"
#include "../testconfig.h"
#include "../time.h"
#include <memory>
#include <cstring>
#include <vector>
namespace paralize_preview {
struct State {
 TObjectPtr<TObjectInstance> owner;
 paralize_core::State core;
 paralize_core::Vec origin;
 TObjectPtr<TCharacter> invoker,target;
 MeshHandle mesh=0,egg=0;
 float egg_uv=0;
 S3DPoint display_origin={};
 uint32_t target_initial_flags=0;
 bool target_cleanup_done=false;
 void CleanupTarget(int completed_tick=-1){
  if(!target || target_cleanup_done)return;
  target->SetParalize((target_initial_flags&OF_PARALIZE)!=0);
  target->SetFlag(OF_DISABLED,(target_initial_flags&OF_DISABLED)!=0);
  S3DPoint p;p = IntPos(target);
  log_info("[paralize-target-cleanup] tick=%d alive=%d flags_before=%x flags_after=%x health=%d target=(%d,%d,%d)",completed_tick<0?ticks:completed_tick,int(core.alive),target_initial_flags,target->Flags(),target->Health(),p.x,p.y,p.z);
  target_cleanup_done=true;
 }
 double accumulator=0;
 int ticks=0;
 int64_t rendered=-1;
 ~State(){CleanupTarget();if(Renderer){if(mesh)Renderer->ReleaseMeshAssetRef(mesh);if(egg)Renderer->ReleaseMeshAssetRef(egg);}}
};
State* Spawn(const S3DPoint& origin)
{
 if(!Renderer)return nullptr;
 auto s=std::make_unique<State>();auto* cl=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cl?cl->FindObjType(0xad92bc14u):-1;if(type<0)return nullptr;
 SObjectDef def={};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos=origin;s->owner.reset(cl->NewObject(&def));
 if(!s->owner || s->owner->ObjId()!=0xad92bc14u)return nullptr;
 auto* img=dynamic_cast<T3DImagery*>(s->owner->GetImagery());
 if(!img || img->NumStates()!=1 || img->GetAniLength(0)!=10 || img->GetAniFlags(0)!=0x2001 || img->NumObjects()!=6 || img->NumTextures()!=6 || img->NumMaterials()!=6 || img->NumTags()!=0)return nullptr;
 const char* names[]={"lightface0","sphere01","lightface1","sphere02","sphere03","lightface2"};for(int i=0;i<6;++i)if(std::strcmp(img->GetObjectName(i),names[i]) || img->GetObjectParent(i,0)!=-1)return nullptr;
 S3DTex tex={};img->GetTexture(0,&tex);const auto& pf=tex.desc.pixelFormat;
 if(tex.numframes!=1 || tex.desc.width!=128 || tex.desc.height!=128 || pf.dwRGBBitCount!=16 || pf.dwRBitMask!=0xf800 || pf.dwGBitMask!=0x7e0 || pf.dwBBitMask!=0x1f || pf.dwRGBAlphaBitMask || img->GetTextureHandle(0)==kInvalidTexture)return nullptr;
 std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;
 if(!ExtractSubMeshTextureSlot(img,0,1,vertices,indices) || vertices.size()!=4 || indices.size()!=6)return nullptr;
 s->mesh=Renderer->RegisterMeshAsset(0x504152414c495a31ull,vertices.data(),4,indices.data(),6,img->GetTextureHandle(0));if(!s->mesh)return nullptr;Renderer->AddMeshAssetRef(s->mesh);
 hmm_mat4 owner;BuildRetailSoftwareOwner(owner,origin.x,origin.y,origin.z,0);s->origin={owner.Elements[3][0],owner.Elements[3][1],owner.Elements[3][2]};
 log_info("[native-domain] enabled=%d camera_z=0 owner=absolute_SCL1_POS2 Paralize1",int(StartupVfxNativeDomain));
 log_info("[paralize] typed_id=ad92bc14 spell=Physical_Paralysis invoker=null targets=0 native_domain=%d",int(StartupVfxNativeDomain));return s.release();
}
static State* SpawnWithTarget(const S3DPoint& origin,bool isolated_seed)
{
 const auto& config=kConfiguredTarget;
 const uint64_t setup_draws=RandomDraws();
 auto s=std::unique_ptr<State>(Spawn(origin));if(!s)return nullptr;
 auto* cl=TObjectClass::GetClass(OBJCLASS_CHARACTER);const int type=cl?cl->FindObjType(config.actor_type):-1;if(type<0)return nullptr;
 auto make=[&](S3DPoint p){SObjectDef def={};def.objclass=OBJCLASS_CHARACTER;def.objtype=short(type);def.pos=p;return TObjectPtr<TCharacter>(dynamic_cast<TCharacter*>(cl->NewObject(&def)));};
 s->invoker=make({config.source_x,config.source_y,config.source_z});s->target=make({config.source_x+config.target_dx,config.source_y+config.target_dy,config.source_z+config.target_dz});
 if(s->target)s->target_initial_flags=s->target->Flags();
 if(!s->invoker || !s->target || s->target->Radius()!=config.expected_radius || !s->invoker->BeginFighting(s->target.get()) || s->invoker->Fighting()!=s->target.get())return nullptr;
 auto* img=dynamic_cast<T3DImagery*>(s->owner->GetImagery());std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;
 if(!ExtractSubMeshTextureSlot(img,1,2,vertices,indices) || vertices.size()!=151 || indices.empty())return nullptr;
 s->egg=Renderer->RegisterMeshAsset(0x5041524145474731ull,vertices.data(),int(vertices.size()),indices.data(),int(indices.size()),img->GetTextureHandle(1));if(!s->egg)return nullptr;Renderer->AddMeshAssetRef(s->egg);
 s->origin={float(config.source_x),float(config.source_y),float(int(RetailSoftwareOwnerZ(config.source_z)))};s->display_origin=origin;
 // Explicit isolated-fixture seed is applied after authentic actor setup,
 // matching actual native srand after the same context construction.
 const auto& snap=FrameSnap::GetConfig();if(isolated_seed && snap.seed_set)SeedRandom(snap.seed);
 log_info("[paralize-target-rng] actor_setup_draws=%llu seeded=%d seed=%u",static_cast<unsigned long long>(RandomDraws()-setup_draws),int(isolated_seed && snap.seed_set),snap.seed);
 log_info("[paralize-target] fixture=real_Acolytes invoker=(3185,3185,16) target=(3281,3185,16) radius=%d display=(%d,%d,%d)",s->target->Radius(),origin.x,origin.y,origin.z);
 return s.release();
}
State* SpawnTargetFixture(const S3DPoint& origin){return SpawnWithTarget(origin,true);}
// Gallery respects the shared game RNG; only the explicit isolated snapshot
// factory above reapplies a requested seed after actor construction.
State* SpawnConfiguredTarget(const S3DPoint& origin){return SpawnWithTarget(origin,false);}
bool Alive(State* s){return s && s->core.alive;}
void Advance(State* s,double dt)
{
 if(!s || !std::isfinite(dt) || dt<=0)return;s->accumulator+=dt;
 while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;if(!Alive(s))return;if(s->target){
   S3DPoint t;t = IntPos(s->target);s->target->Disable();s->target->Stop();s->target->SetParalize(true);
   s->core.TickTarget([](int a,int b){return random(a,b);},s->origin,{t.x,t.y,int(RetailSoftwareOwnerZ(t.z))},s->target->Radius(),1);
   if(!s->core.alive){s->owner->SetFlag(OF_KILL);s->CleanupTarget(s->ticks+1);}
  }else s->core.Tick([](int a,int b){return random(a,b);},s->origin);++s->ticks;
  const auto& p=s->core.position[0];log_info("[paralize-phase] tick=%d variant=%d phase=%d count=%d scale=%.9g knot=%.9g first=(%.9g,%.9g,%.9g) alive=%d",s->ticks,s->core.variant,s->core.phase,s->core.count,s->core.scale,s->core.knot,p.x,p.y,p.z,int(s->core.alive));}
}
void SubmitWorld(State* s)
{
 if(!Alive(s) || !Renderer || s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();
 // Render owns the UV cadence, including frames when the sphere is scale0.
 if(s->target)s->egg_uv-=s->core.uv;
 paralize_core::ForEachDraw(s->core,s->egg_uv,[&](const paralize_core::Draw& d){
  SHelperMeshSubmit item={};item.mesh=d.object?s->egg:s->mesh;
  item.world[15]=1;item.world[0]=item.world[5]=item.world[10]=d.scale;
  item.world[3]=d.position.x;item.world[7]=d.position.y;item.world[11]=d.position.z;
  if(s->target){item.world[3]+=s->display_origin.x-kConfiguredTarget.source_x;item.world[7]+=s->display_origin.y-kConfiguredTarget.source_y;item.world[11]+=RetailSoftwareOwnerZ(s->display_origin.z)-RetailSoftwareOwnerZ(kConfiguredTarget.source_z);}
  item.additive_blend=true;item.retail_lighting=d.object?6:1;
  if(d.object){item.retail_additive_cull_none=true;for(float& c:item.diffuse)c=31.f;item.uv_offset[0]=item.uv_offset[1]=d.uv;}
  item.retail_software_projection=StartupVfxNativeDomain;
  if(s->target)Renderer->SubmitHelperMesh(item);else Renderer->SubmitGoldFlareAfterFx(item);
 });
}
void Destroy(State* s){delete s;}
}
