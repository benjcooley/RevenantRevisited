#include "sandswirlruntime.h"
#include "../character.h"
#include "../mappane.h"
#include "../mapmanager.h"
#include "../gamemap.h"
#include "../framesnap.h"
#include "../logging.h"
#include "../time.h"
#include "../testconfig.h"
#include <cstdlib>

namespace sandswirl_runtime {
struct MapFixture {
 static constexpr int level=253;
 TGameMap* previous=nullptr;
 bool map_created=false,window=false;
 TSafeRef<TCharacter> caster,target;
 TSafeRef<Effect> owner;
 std::unique_ptr<TSpell> spell;
 double accumulator=0;int ticks=0;int64_t rendered=-1;
 bool trace=std::getenv("REV_SANDSWIRL_MAP_TRACE")!=nullptr;
 ~MapFixture(){
  if(auto*o=owner.Get())o->SetSpell(nullptr);
  if(auto*c=caster.Get())if(auto*t=target.Get()){
   log_info("[sandswirl-map-cleanup] tick=%d health=%d/%d flags=%x/%x spell_timer=%d",ticks,c->Health(),t->Health(),c->Flags(),t->Flags(),spell?spell->TimerValue():-1);
   log_info("[sandswirl-map-actors] source=%d,%d,%d target=%d,%d,%d",c->Pos().x,c->Pos().y,c->Pos().z,t->Pos().x,t->Pos().y,t->Pos().z);
  }
  spell.reset();
  if(window)MapPane.ReleaseCommandMapWindow();
  if(map_created){MapManager.SetCurrentMap(previous);MapManager.Evict(level);}
 }
};
static TObjectInstance* NewMapObject(int cls,int type,const S3DPoint&p){
 if(type<0)return nullptr;SObjectDef d{};d.objclass=short(cls);d.objtype=short(type);d.pos=p;d.level=MapFixture::level;
 const int id=MapPane.NewObject(&d);return id<0?nullptr:MapPane.GetInstance(id);
}
static void Hex(const void*p,size_t n,char*out){const char*d="0123456789abcdef";const auto*b=static_cast<const uint8_t*>(p);for(size_t i=0;i<n;++i){out[2*i]=d[b[i]>>4];out[2*i+1]=d[b[i]&15];}out[2*n]=0;}
static MapFixture* Spawn(const S3DPoint& origin,bool restored){
 if(!Renderer || !SpellList.Initialize())return nullptr;auto s=std::make_unique<MapFixture>();s->previous=MapManager.CurrentMap();
 const int sx=origin.x>>SECTORWSHIFT,sy=origin.y>>SECTORHSHIFT;
 if(!MapManager.CreateTransient(MapFixture::level,sx-2,sy-2,sx+2,sy+2))return nullptr;s->map_created=true;
 if(!MapPane.BindCommandMapWindow(MapFixture::level,origin))return nullptr;s->window=true;
 auto*characters=TObjectClass::GetClass(OBJCLASS_CHARACTER);auto*effects=TObjectClass::GetClass(OBJCLASS_EFFECT);if(!characters || !effects)return nullptr;
 const int actor_type=characters->FindObjType("Acolyte");
 s->caster=dynamic_cast<TCharacter*>(NewMapObject(OBJCLASS_CHARACTER,actor_type,origin));
 s->target=dynamic_cast<TCharacter*>(NewMapObject(OBJCLASS_CHARACTER,actor_type,{origin.x+96,origin.y,origin.z}));
 if(!s->caster.Get() || !s->target.Get() || !s->caster->BeginFighting(s->target.Get()))return nullptr;
 auto*data=SpellList.GetSpellDataByName("Quicksand");auto*variant=SpellList.GetVariantDataByName("Quicksand");if(!data || !variant)return nullptr;
 TObjectInstance*target=s->target.Get();S3DPoint offset=origin;s->spell=std::make_unique<TSpell>(s->caster.Get(),&target,1,&offset,data,variant);
 auto*missing=dynamic_cast<Effect*>(NewMapObject(OBJCLASS_EFFECT,effects->FindObjType("Sandswirl"),origin));
 if(!missing)return nullptr;
 const auto before_missing=RandomDraws();const bool missing_ok=!missing->InitializeRuntime() && (missing->Flags()&OF_KILL) && !missing->ParticleChild() && RandomDraws()==before_missing;
 MapPane.DeleteObject(missing);if(!missing_ok)return nullptr;
 log_info("[sandswirl-map-negative] missing_sand_failed_closed=1 no_child=1 rng_unchanged=1 actual_map_delete=1");
 if(restored){
  // Explicit diagnostic configuration only. Restores the supplied legacy
  // Class.Def Sand/fecaface entry; keeps the shipped registry file unchanged.
  int sand=effects->FindObjType("sand");
  if(sand<0){char name[]="Sand",filename[]="Magic\\Sandswirl.i3d";sand=effects->AddType(name,filename,0xfecafaceu);}
  if(sand<0 || effects->FindObjType(0xfecafaceu)!=sand)return nullptr;
  log_info("[sandswirl-restored-binding] class=25 name=Sand id=fecaface resource=Magic/Sandswirl.i3d explicit_legacy_restoration=1 shipped_registry_file_unchanged=1");
 }
 const auto&snap=FrameSnap::GetConfig();if(snap.seed_set)SeedRandom(snap.seed);
 // Unmodified map APIs, exact named builder, no supplied allocation return.
 s->owner=dynamic_cast<Effect*>(NewMapObject(OBJCLASS_EFFECT,effects->FindObjType("Sandswirl"),origin));if(!s->owner.Get())return nullptr;
 s->owner->SetSpell(s->spell.get());if(restored)s->owner->UseRestoredLegacySand(41);else s->owner->UseQualifiedSparks(41);
 if(!s->owner->InitializeRuntime())return nullptr;
 log_info("[sandswirl-map-fixture] diagnostic_only=1 child=%s seekz=41 named_spell=Quicksand exact_sandswirl_variant=0 actors=%d,%d parent=%d seed=%u rng=%llu map_alloc_rng=none",restored?"RestoredLegacySand":"Sparks",s->caster->GetMapIndex(),s->target->GetMapIndex(),s->owner->GetMapIndex(),snap.seed,static_cast<unsigned long long>(RandomDraws()));
 log_info("[native-domain] enabled=%d camera_z=0 Sandswirl actual map child=%s",int(StartupVfxNativeDomain),restored?"RestoredLegacySand":"Sparks");
 return s.release();
}
MapFixture* SpawnMapFixture(const S3DPoint&o){return Spawn(o,false);}
MapFixture* SpawnRestoredSandMapFixture(const S3DPoint&o){return Spawn(o,true);}
void AdvanceMapFixture(MapFixture*s,double dt){
 if(!s || !std::isfinite(dt) || dt<=0)return;s->accumulator+=dt;
 while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;++s->ticks;auto*owner=s->owner.Get();if(!owner)return;
  // Same declared snapshot ordering as the original diagnostic: children born
  // this tick first Pulse next tick. Each call is the actual object's virtual.
  std::vector<TSafeRef<TObjectInstance>> objects;objects.emplace_back(owner);if(auto*c=owner->ParticleChild())objects.emplace_back(c);
  for(int id:owner->SpawnedQuicksand())if(auto*q=MapPane.GetInstance(id))objects.emplace_back(q);
  for(auto&ref:objects)if(auto*o=ref.Get())if(!(o->Flags()&OF_KILL)){o->NextFrame();o->Pulse();}
  const auto*p=owner->ParticleState();
  log_info("[sandswirl-map-phase] tick=%d parent=%d parent_kill=%d child_kill=%d query=%zu quicksands=%zu rng=%llu spell_timer=%d",s->ticks,owner->ParentFrame(),int(bool(owner->Flags()&OF_KILL)),p?int(p->killed):-1,owner->QueriedObjects().size(),owner->SpawnedQuicksand().size(),static_cast<unsigned long long>(RandomDraws()),s->spell->TimerValue());
  for(int id:owner->SpawnedQuicksand())if(auto*q=MapPane.GetInstance(id))log_info("[sandswirl-map-quicksand] tick=%d id=%d frame=%d kill=%d",s->ticks,id,q->GetFrame(),int(bool(q->Flags()&OF_KILL)));
  if(s->trace && p)for(size_t i=0;i<p->pool.size();++i){char hex[81];Hex(&p->pool[i],40,hex);log_info("[sandswirl-map-pool] tick=%d slot=%zu data=%s",s->ticks,i,hex);}
 }
}
void SubmitMapFixture(MapFixture*s){
 if(!s || !Renderer || s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();
 if(auto*owner=s->owner.Get())if(auto*child=owner->ParticleChild())if(auto*v=child->GetComponent<TFlipbookBillboardComponent>()){
  v->Submit(*Renderer,*child);
  if(s->trace)if(auto*p=owner->ParticleState()){int draws=0;p->Render([&](int object,retail_seeking_particles::Vec pos){char hex[25];Hex(&pos,12,hex);log_info("[sandswirl-map-packet] tick=%d draw=%d object=%d pos=%s",s->ticks,draws++,object,hex);});log_info("[sandswirl-map-render] tick=%d draws=%d",s->ticks,draws);}
 }
}
void DestroyMapFixture(MapFixture*s){delete s;}
}
