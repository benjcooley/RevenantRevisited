#include "ymanadrainpreview.h"
#include "../effect.h"
#include "../character.h"
#include "../spell.h"
#include "../framesnap.h"
#include "../manadrainpose.h"
#include "../ymanadraincore.h"
#include "../meshextract.h"
#include "../retailsoftwaretransform.h"
#include "../testconfig.h"
#include "../time.h"
#include "../logging.h"
#include <memory>
#include <cstring>
#include <algorithm>
namespace ymanadrain_preview {
struct State {
 TObjectPtr<TObjectInstance> owner;
 TObjectPtr<TCharacter> caster,target;
 std::unique_ptr<TSpell> spell;
 ymanadrain_core::State core;
 MeshHandle meshes[2]{};
 S3DPoint display={};
 int minimum=0,maximum=0,tick=0;
 double accumulator=0;
 uint64_t initial_draws=0;
 int64_t rendered=-1;
 ~State(){if(caster && target && spell){const auto& p=IntPos(target);log_info("[ymanadrain-cleanup] tick=%d alive=%d mana=%d/%d health=%d/%d target=(%d,%d,%d) flags=%x/%x spell_timer=%d",tick,int(core.alive),caster->Mana(),target->Mana(),caster->Health(),target->Health(),p.x,p.y,p.z,caster->Flags(),target->Flags(),spell->TimerValue());}if(auto* effect=dynamic_cast<TEffect*>(owner.get()))effect->SetSpell(nullptr);if(Renderer)for(auto mesh:meshes)if(mesh)Renderer->ReleaseMeshAssetRef(mesh);}
};
static State* Spawn(const S3DPoint& origin,bool isolated_seed){
 if(!Renderer || !SpellList.Initialize())return nullptr;
 auto* data=SpellList.GetSpellDataByName("YManaDrain");auto* variant=SpellList.GetVariantDataByName("YManaDrain");
 if(!data || !variant || std::strcmp(variant->name,"YManaDrain") || variant->mindamage!=160 || variant->maxdamage!=260)return nullptr;
 auto s=std::make_unique<State>();s->display=origin;s->minimum=variant->mindamage;s->maximum=variant->maxdamage;
 auto* effects=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=effects?effects->FindObjType(0xae92bc14u):-1;if(type<0)return nullptr;
 SObjectDef def={};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos={3185,3185,16};s->owner.reset(effects->NewObject(&def));if(!s->owner || s->owner->ObjId()!=0xae92bc14u)return nullptr;
 auto* img=dynamic_cast<T3DImagery*>(s->owner->GetImagery());if(!img || img->NumObjects()!=4 || img->NumStates()!=1 || img->GetAniLength(0)!=100 || img->NumTextures()!=4 || img->NumMaterials()!=4 || img->NumTags()!=0)return nullptr;
 const char* names[]={"cone01","rectangle01","rectangle02","cone02"};for(int i=0;i<4;++i)if(std::strcmp(img->GetObjectName(i),names[i]) || img->GetObjectParent(i,0)!=-1)return nullptr;
 for(int i=0;i<2;++i){const int object=i+2;S3DTex tex={};img->GetTexture(object,&tex);const auto& p=tex.desc.pixelFormat;
  if(tex.numframes!=1 || tex.desc.width!=128 || tex.desc.height!=128 || p.dwRGBBitCount!=16 || p.dwRBitMask!=0xf00 || p.dwGBitMask!=0xf0 || p.dwBBitMask!=0xf || p.dwRGBAlphaBitMask!=0xf000 || img->GetTextureHandle(object)==kInvalidTexture)return nullptr;
  std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;if(!ExtractSubMeshTextureSlot(img,object,object+1,vertices,indices) || vertices.size()!=size_t(i?194:4) || indices.size()!=size_t(i?1008:6))return nullptr;
  s->meshes[i]=Renderer->RegisterMeshAsset(0x594d414e41440000ull+i,vertices.data(),int(vertices.size()),indices.data(),int(indices.size()),img->GetTextureHandle(object));if(!s->meshes[i])return nullptr;Renderer->AddMeshAssetRef(s->meshes[i]);
 }
 auto* characters=TObjectClass::GetClass(OBJCLASS_CHARACTER);const int actor_type=characters?characters->FindObjType("Acolyte"):-1;if(actor_type<0)return nullptr;
 auto make=[&](S3DPoint p){SObjectDef d={};d.objclass=OBJCLASS_CHARACTER;d.objtype=short(actor_type);d.pos=p;return TObjectPtr<TCharacter>(dynamic_cast<TCharacter*>(characters->NewObject(&d)));};
 s->caster=make({3185,3185,16});s->target=make({3281,3185,16});if(!s->caster || !s->target || !s->caster->BeginFighting(s->target.get()) || s->caster->Fighting()!=s->target.get())return nullptr;
 TObjectInstance* target=s->target.get();S3DPoint offset={0,0,0};s->spell=std::make_unique<TSpell>(s->caster.get(),&target,1,&offset,data,variant);if(auto* effect=dynamic_cast<TEffect*>(s->owner.get()))effect->SetSpell(s->spell.get());else return nullptr;
 const auto& snap=FrameSnap::GetConfig();if(isolated_seed && snap.seed_set)SeedRandom(snap.seed);s->initial_draws=RandomDraws();
 log_info("[ymanadrain-rng] seeded=%d seed=%u",int(isolated_seed && snap.seed_set),snap.seed);
 log_info("[native-domain] enabled=%d camera_z=0 owner=absolute_matrix YManaDrain authentic actors",int(StartupVfxNativeDomain));
 log_info("[ymanadrain] exact_id=ae92bc14 source=(3185,3185,16) target=(3281,3185,16) variant=YManaDrain min=%d max=%d mana=%d/%d",s->minimum,s->maximum,s->caster->Mana(),s->target->Mana());return s.release();
}
State* SpawnTargetFixture(const S3DPoint& origin){return Spawn(origin,true);}
State* SpawnConfiguredTarget(const S3DPoint& origin){return Spawn(origin,false);}
bool Alive(State* s){return s && s->core.alive && !(s->owner->GetFlags()&OF_KILL);}
void Advance(State* s,double dt){
 if(!s || !std::isfinite(dt) || dt<=0)return;s->accumulator+=dt;
 while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;if(!Alive(s))return;s->owner->NextFrame();
  const auto& a=IntPos(s->caster);const auto& b=IntPos(s->target);
  s->core.Advance({a.x,a.y,int(RetailSoftwareOwnerZ(a.z))},{b.x,b.y,int(RetailSoftwareOwnerZ(b.z))},ConvertToFacing(a,b),s->minimum,s->maximum,
   [](int low,int high){return random(low,high);},[](manadrain_core::Point x,manadrain_core::Point y){return ::Distance(S3DPoint(x.x,x.y,x.z),S3DPoint(y.x,y.y,y.z));},
   [&](int amount){amount=ymanadrain_core::TransferAmount(s->caster->Mana(),s->caster->MaxMana(),s->target->Mana(),amount);s->caster->SetMana(s->caster->Mana()+amount);s->target->SetMana(s->target->Mana()-amount);return amount;},
   [&]{s->spell->Kill();s->owner->SetFlag(OF_KILL);});
  ++s->tick;
  log_info("[ymanadrain-phase] tick=%d stage=%d remaining=%d amount=%d scale=%.9g mana=%d/%d alive=%d",s->tick,s->core.stage,s->core.ticks,s->core.amount,s->core.scale,s->caster->Mana(),s->target->Mana(),int(Alive(s)));
 }
}
void SubmitWorld(State* s){
 if(!Alive(s) || !Renderer || s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();const float glow=s->core.RenderGlowScale([](int a,int b){return random(a,b);});
 const auto& p=s->core.position[0];log_info("[ymanadrain-draw] tick=%d rng=%llu first=(%d,%d,%d) scale=%.9g rotation=%.9g glow=%.9g",s->tick,static_cast<unsigned long long>(RandomDraws()-s->initial_draws),p.x,p.y,p.z,s->core.scale,s->core.rotation,glow);
 manadrain_core::ForEachDraw(s->core,s->core.facing,glow,[&](int object,hmm_mat4 matrix,bool){
  matrix.Elements[3][0]+=s->display.x-3185;matrix.Elements[3][1]+=s->display.y-3185;matrix.Elements[3][2]+=RetailSoftwareOwnerZ(s->display.z)-RetailSoftwareOwnerZ(16);
  SHelperMeshSubmit draw={};draw.mesh=s->meshes[object-2];draw.retail_lighting=3;draw.retail_source_alpha=true;draw.retail_alpha_source_quantize=StartupVfxNativeDomain;draw.retail_software_projection=StartupVfxNativeDomain;
  for(int row=0;row<4;++row)for(int col=0;col<4;++col)draw.world[row*4+col]=matrix.Elements[col][row];Renderer->SubmitHelperMesh(draw);
 });
}
void Destroy(State* s){delete s;}
}
