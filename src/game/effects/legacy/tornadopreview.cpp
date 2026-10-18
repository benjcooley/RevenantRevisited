#include "game/effects/legacy/tornadopreview.h"
#include "game/effects/effect.h"
#include "game/object/character.h"
#include "game/world/mappane.h"
#include "game/spells/spell.h"
#include "engine/render/framesnap.h"
#include "game/effects/tornadopose.h"
#include "game/imagery/meshextract.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include "engine/core/logging.h"
#include <memory>
#include <cstring>
namespace tornado_preview {
struct State {
 TObjectPtr<TObjectInstance> owner;TObjectPtr<TCharacter> caster,target;std::unique_ptr<TSpell> spell;
 tornado_core::State core;MeshHandle meshes[4]{};S3DPoint display={};double accumulator=0;uint64_t initial_draws=0;int64_t rendered=-1;
 ~State(){if(caster && target && spell)log_info("[tornado-cleanup] tick=%d alive=%d health=%d/%d flags=%x/%x spell_timer=%d damage_events=%d",core.frame,int(core.alive),caster->Health(),target->Health(),caster->Flags(),target->Flags(),spell->TimerValue(),core.damage_events);if(auto* effect=dynamic_cast<TEffect*>(owner.get()))effect->SetSpell(nullptr);if(Renderer)for(auto mesh:meshes)if(mesh)Renderer->ReleaseMeshAssetRef(mesh);}
};
static State* Spawn(const S3DPoint& origin,bool isolated){
 if(!Renderer || !SpellList.Initialize())return nullptr;
 auto* data=SpellList.GetSpellDataByName("Tornado");auto* variant=SpellList.GetVariantDataByName("Tornado");
 if(!data || !variant || std::strcmp(variant->name,"Tornado") || std::strcmp(variant->effect,"Funnel") || variant->mindamage!=110 || variant->maxdamage!=140)return nullptr;
 auto s=std::make_unique<State>();s->display=origin;auto* effects=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=effects?effects->FindObjType(0x2784abdeu):-1;if(type<0)return nullptr;
 SObjectDef def={};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos={3185,3185,16};s->owner.reset(effects->NewObject(&def));if(!s->owner || s->owner->ObjId()!=0x2784abdeu)return nullptr;
 auto* img=dynamic_cast<T3DImagery*>(s->owner->GetImagery());if(!img || img->NumObjects()!=4 || img->NumStates()!=1 || img->GetAniLength(0)!=100 || img->NumTextures()!=3 || img->NumMaterials()!=3 || img->NumTags()!=0)return nullptr;
 const char* names[]={"box01","cube","box02","sphere01"};const int counts[]={4,10,4,65},indices_expected[]={6,12,6,108},textures[]={0,1,2,1};
 for(int i=0;i<4;++i){if(std::strcmp(img->GetObjectName(i),names[i]) || img->GetObjectParent(i,0)!=-1)return nullptr;const int texslot=textures[i];S3DTex tex={};img->GetTexture(texslot,&tex);const auto& p=tex.desc.pixelFormat;
  if(tex.numframes!=1 || tex.desc.width!=uint32_t(i==0?64:128) || tex.desc.height!=uint32_t(i==0?64:128) || p.dwRGBBitCount!=16 || p.dwRBitMask!=uint32_t(i==0?0xf800:0xf00) || p.dwGBitMask!=uint32_t(i==0?0x7e0:0xf0) || p.dwBBitMask!=uint32_t(i==0?0x1f:0xf) || p.dwRGBAlphaBitMask!=uint32_t(i==0?0:0xf000) || img->GetTextureHandle(texslot)==kInvalidTexture)return nullptr;
  std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;if(!ExtractSubMeshTextureSlot(img,i,texslot+1,vertices,indices) || vertices.size()!=size_t(counts[i]) || indices.size()!=size_t(indices_expected[i]))return nullptr;
  s->meshes[i]=Renderer->RegisterMeshAsset(0x544f524e41440000ull+i,vertices.data(),int(vertices.size()),indices.data(),int(indices.size()),img->GetTextureHandle(texslot));if(!s->meshes[i])return nullptr;Renderer->AddMeshAssetRef(s->meshes[i]);
 }
 auto* characters=TObjectClass::GetClass(OBJCLASS_CHARACTER);const int actor_type=characters?characters->FindObjType("Acolyte"):-1;if(actor_type<0)return nullptr;
 auto make=[&](S3DPoint p){SObjectDef d={};d.objclass=OBJCLASS_CHARACTER;d.objtype=short(actor_type);d.pos=p;return TObjectPtr<TCharacter>(dynamic_cast<TCharacter*>(characters->NewObject(&d)));};s->caster=make({3185,3185,16});s->target=make({3281,3185,16});if(!s->caster || !s->target || !s->caster->BeginFighting(s->target.get()))return nullptr;
 TObjectInstance* target=s->target.get();S3DPoint offset={0,0,0};s->spell=std::make_unique<TSpell>(s->caster.get(),&target,1,&offset,data,variant);if(auto* effect=dynamic_cast<TEffect*>(s->owner.get()))effect->SetSpell(s->spell.get());else return nullptr;
 const auto& snap=FrameSnap::GetConfig();if(isolated && snap.seed_set)SeedRandom(snap.seed);s->initial_draws=RandomDraws();s->core.Initialize([](int a,int b){return random(a,b);});
 log_info("[tornado-rng] seeded=%d seed=%u initial_draws=%llu",int(isolated && snap.seed_set),snap.seed,static_cast<unsigned long long>(RandomDraws()-s->initial_draws));
 log_info("[native-domain] enabled=%d camera_z=0 owner=native_matrix Tornado explicit named variant",int(StartupVfxNativeDomain));log_info("[tornado] exact_id=2784abde variant=Tornado natural_effect=Funnel explicit_owner=Tornado actors=(3185,3185,16)/(3281,3185,16)");return s.release();
}
State* SpawnTargetFixture(const S3DPoint& origin){return Spawn(origin,true);}State* SpawnConfiguredTarget(const S3DPoint& origin){return Spawn(origin,false);}
bool Alive(State* s){return s && s->core.alive && !(s->owner->GetFlags()&OF_KILL);}
void Advance(State* s,double dt){
 if(!s || !std::isfinite(dt) || dt<=0)return;s->accumulator+=dt;
 while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;if(!Alive(s))return;s->owner->NextFrame();const int events=s->core.damage_events;s->core.Advance([](int a,int b){return random(a,b);});
  if(!s->core.alive){s->spell->Kill();s->owner->SetFlag(OF_KILL|OF_PULSE);}
  if(s->core.damage_events!=events){
   // Original4de250 constructs44ceb0 with radius200, flags0xe0,
   // character set2 and caster level. The proven fixture's genuine actors
   // are not sector residents. Execute that query; reject other contexts
   // rather than pretending to implement the unverified damage branch.
   const S3DPoint pos=IntPos(s->owner);SRect rectangle{pos.x-200,pos.y-200,pos.x+200,pos.y+200};
   TMapIterator characters(s->caster->GetLevel(),&rectangle,CHECK_NOINVENT|CHECK_MAPRECT|CHECK_LOADED,OBJSET_CHARACTER);
   if(characters){log_error("[tornado] unsupported nonempty damage query; configured fixture stopped");s->core.alive=false;s->spell->Kill();s->owner->SetFlag(OF_KILL|OF_PULSE);}
   else log_info("[tornado-damage-query] tick=%d radius=200 loaded=1 characters=0",s->core.frame);
  }
  const auto& p=s->core.swirls[0];log_info("[tornado-phase] tick=%d rng=%llu flame_on=%d first=(%.9g,%.9g,%.9g) scale=%.9g alive=%d",s->core.frame,static_cast<unsigned long long>(RandomDraws()-s->initial_draws),s->core.flame_on,p.pos.x,p.pos.y,p.pos.z,p.scale,int(Alive(s)));
 }
}
void SubmitWorld(State* s){
 if(!Alive(s) || !Renderer || s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();hmm_mat4 owner;BuildRetailSoftwareOwner(owner,s->display.x,s->display.y,s->display.z,0);
 tornado_core::ForEachDraw(s->core,owner,0,[&](int object,const hmm_mat4& world){SHelperMeshSubmit draw={};draw.mesh=s->meshes[object];draw.retail_lighting=object==0?1:3;draw.additive_blend=object==0;draw.retail_additive_cull_none=object==0;draw.retail_source_alpha=object!=0;draw.retail_alpha_source_quantize=StartupVfxNativeDomain && object!=0;draw.retail_software_projection=StartupVfxNativeDomain;for(int row=0;row<4;++row)for(int col=0;col<4;++col)draw.world[row*4+col]=world.Elements[col][row];Renderer->SubmitHelperMesh(draw);});
}
void Destroy(State* s){delete s;}
}
