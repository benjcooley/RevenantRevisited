#include "strikemeteorpreview.h"
#include "../effect.h"
#include "../character.h"
#include "../strikemeteorcore.h"
#include "../cometmatrix.h"
#include "../cometsoftwarebounds.h"
#include "../meshextract.h"
#include "../framesnap.h"
#include "../testconfig.h"
#include "../logging.h"
#include <memory>
#include <cstring>
namespace strike_meteor_preview {
struct State {
 struct Part {MeshHandle mesh=0;S3DMat material{};int object=0,triangle=0;hmm_vec3 vertices[3]{};};
 std::array<TObjectPtr<TObjectInstance>,4> owners;
 TObjectPtr<TCharacter> caster,target;std::unique_ptr<TStrikeSpell> spell;
 TObjectImagery* resource=nullptr;std::vector<Part> parts;strike_meteor::State core;double accumulated=0;uint64_t initial_draws=0;int64_t submitted=-1;
 ~State(){for(auto&o:owners)if(auto*e=dynamic_cast<TEffect*>(o.get()))e->SetSpell(nullptr);if(Renderer)for(auto&p:parts)if(p.mesh)Renderer->ReleaseMeshAssetRef(p.mesh);if(resource)TObjectImagery::FreeImagery(resource);if(spell && caster && target)log_info("[strike-cleanup] tick=%d manager=%d alive=%d health=%d/%d spell_timer=%d",core.tick,core.manager_frame,int(core.alive),caster->Health(),target->Health(),spell->TimerValue());}
};
static State* Spawn(const S3DPoint& origin,bool isolated){
 if(!Renderer || !SpellList.Initialize())return nullptr;
 auto* data=SpellList.GetSpellDataByName("Meteor Storm");auto* var=SpellList.GetVariantDataByName("Meteor Storm");const auto* c=var?var->controldata:nullptr;
 if(!data || !var || std::strcmp(var->effect,"StrikeEffect") || !c || std::strcmp(c->name,"Strike") || c->hits!=4 || c->radius!=265 || c->duration!=93 || c->wait!=48 || c->pattern!=SPAT_RANDOM || c->duration2 || c->follow || c->attachset || c->hittarget || c->oncaster || c->repeatdamage || c->multipletargets)return nullptr;
 auto s=std::make_unique<State>();auto* chars=TObjectClass::GetClass(OBJCLASS_CHARACTER);int actor_type=chars?chars->FindObjType("Acolyte"):-1;if(actor_type<0)return nullptr;
 auto actor=[&](S3DPoint pos){SObjectDef def={};def.objclass=OBJCLASS_CHARACTER;def.objtype=short(actor_type);def.pos=pos;return TObjectPtr<TCharacter>(dynamic_cast<TCharacter*>(chars->NewObject(&def)));};
 s->caster=actor(origin);s->target=actor({origin.x+96,origin.y,origin.z});if(!s->caster || !s->target || !s->caster->BeginFighting(s->target.get()))return nullptr;
 TObjectInstance* target=s->target.get();S3DPoint offset={};s->spell=std::make_unique<TStrikeSpell>(s->caster.get(),&target,1,&offset,data,var,nullptr);
 auto* effects=TObjectClass::GetClass(OBJCLASS_EFFECT);int type=effects?effects->FindObjType(0xad92bd32u):-1;auto* info=type>=0?effects->GetObjType(type):nullptr;
 s->resource=TObjectImagery::LoadImagery(c->imagery);auto* image=dynamic_cast<T3DImagery*>(s->resource);if(!info || !info->objbuilder || !image)return nullptr;
 const auto& snap=FrameSnap::GetConfig();if(isolated && snap.seed_set)SeedRandom(snap.seed);s->initial_draws=RandomDraws();
 s->core.Initialize({origin.x,origin.y,origin.z},[](int lo,int hi){return random(lo,hi);},[](){const uint32_t seed=uint32_t(GameRand());SeedRandom(seed);(void)GameRand();},[](int angle,int radius){S3DPoint p;ConvertToVector(angle,radius,p);return strike_meteor::Point{p.x,p.y,p.z};});
 for(int i=0;i<4;++i){const auto&p=s->core.owners[i].position;SObjectDef def={};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos={p.x,p.y,p.z};
  // Same original class builder with the actual spell's imagery override;
  // default Dummy association and all other class factories remain unchanged.
  auto* owner_image=TObjectImagery::LoadImagery(c->imagery);if(!owner_image)return nullptr;
  s->owners[i].reset(info->objbuilder->Build(&def,owner_image));if(!s->owners[i]){TObjectImagery::FreeImagery(owner_image);return nullptr;}auto* o=s->owners[i].get();auto* e=dynamic_cast<TEffect*>(o);if(!e || o->ObjId()!=0xad92bd32u)return nullptr;
  e->SetSpell(s->spell.get());SetIntPos(o, def.pos);o->Face(0);o->OnScreen();auto* a=dynamic_cast<T3DAnimator*>(o->GetAnimator());if(!a || !image->HasRetailCometProfile() || a->PartSysUnsupported() || a->PartSysControllerCount()!=3)return nullptr;
  a->SetCometGround([](int,int,int){return 0;});
 }
 for(int slot=1;slot<=2;++slot)for(int object=0;object<8;++object){if(object==0 || object==2 || object==5)continue;std::vector<SMeshVertex>vertices;std::vector<uint16_t>indices;if(!ExtractSubMeshTextureSlot(image,object,slot,vertices,indices))continue;
  for(size_t triangle=0;triangle<indices.size();triangle+=3){State::Part p;p.object=object;p.triangle=int(triangle/3);S3DObj obj={};image->GetObject(object,&obj);image->GetMaterial(obj.material,&p.material);for(int k=0;k<3;++k){const auto&v=vertices[indices[triangle+k]];p.vertices[k]={v.pos[0],v.pos[1],v.pos[2]};}
   const uint64_t key=0x5354524b00000000ull^(uint64_t(uint32_t(image->ImageryId()))<<16)^uint64_t(object*16+slot*4);p.mesh=Renderer->RegisterMeshAsset(key,vertices.data(),int(vertices.size()),indices.data(),int(indices.size()),image->GetTextureHandle(slot-1));if(!p.mesh)return nullptr;Renderer->AddMeshAssetRef(p.mesh);s->parts.push_back(p);
  }
 }
 if(s->parts.size()!=4)return nullptr;
 log_info("[strike] exact_id=ad92bd32 variant=MeteorStorm hits=4 duration=93 wait=48 clock=0 source_actors=%d,%d/%d,%d seeded=%d rng=%llu",origin.x,origin.y,origin.x+96,origin.y,int(isolated&&snap.seed_set),(unsigned long long)(RandomDraws()-s->initial_draws));
 log_info("[native-domain] enabled=%d camera_z=0 owner=native_matrix",int(StartupVfxNativeDomain));return s.release();
}
State* SpawnFixture(const S3DPoint&o){return Spawn(o,true);}State* SpawnConfigured(const S3DPoint&o){return Spawn(o,false);}bool Alive(State*s){return s&&s->core.alive;}
void Advance(State*s,double dt){if(!Alive(s)||!std::isfinite(dt)||dt<=0)return;s->accumulated+=dt;while(s->accumulated+1e-10>=TTime::LegacyFrameSeconds && Alive(s)){s->accumulated-=TTime::LegacyFrameSeconds;const auto before=s->core.owners;s->core.Advance();
 for(int i=0;i<4;++i){auto*o=s->owners[i].get();const auto&c=s->core.owners[i];if(!c.alive){o->SetFlag(OF_KILL);static_cast<TEffect*>(o)->SetSpell(nullptr);continue;}if(before[i].animate)o->NextFrame();if(c.animate)o->SetFlag(OF_ANIMATE);else o->ClearFlag(OF_ANIMATE);if(c.pulse)o->Pulse();
  log_info("[strike-owner] tick=%d object=%d frame=%d expected=%d wait=%d life=%d animate=%d",s->core.tick,i,o->GetFrame(),c.frame,c.wait,c.lifetime,int(c.animate));}
 if(!Alive(s))s->spell->Kill();log_info("[strike-phase] tick=%d manager=%d rng=%llu alive=%d",s->core.tick,s->core.manager_frame,(unsigned long long)(RandomDraws()-s->initial_draws),int(Alive(s)));}}
void SubmitWorld(State*s){if(!Alive(s)||!Renderer||s->submitted==TTime::FrameCount())return;s->submitted=TTime::FrameCount();
 for(int i=0;i<4;++i){if(!s->core.owners[i].alive || !s->core.owners[i].animate)continue;auto*o=s->owners[i].get();o->Animate(false);auto*a=dynamic_cast<T3DAnimator*>(o->GetAnimator());auto*image=a->Get3DImagery();for(int prototype:{0,5,2})a->SubmitPartSys(*Renderer,prototype,1,{1,1,1},StartupVfxNativeDomain);
  hmm_mat4 owner=o->Transform().Matrix();if(StartupVfxNativeDomain){auto p=IntPos(o);BuildRetailSoftwareOwner(owner,p.x,p.y,p.z,0);}
  for(const auto&p:s->parts){hmm_vec3 pos={},rot={},scale={};if(!image->GetRetailStrikeAniKey(p.object,o->GetFrame(),pos,rot,scale)||scale.X<=1e-5f||scale.Y<=1e-5f||scale.Z<=1e-5f)continue;hmm_mat4 local,world;BuildRetailCometMatrix(local,p.object,pos,rot,scale);MtxMultiply(&world,&local,&owner);
   if(StartupVfxNativeDomain&&Renderer->UsesRetailSoftwareMeshLighting()&&!RetailCometTriangleAllowed(p.vertices,world))continue;
   SHelperMeshSubmit q={};q.mesh=p.mesh;q.index_offset=p.triangle*3;q.index_count=3;q.additive_blend=true;q.retail_lighting=1;q.retail_gold_no_depth=true;q.retail_positive_face_cull=true;q.retail_software_projection=StartupVfxNativeDomain;q.retail_normal_z_scale=StartupVfxNativeDomain?1.f:1.f/WORLD3D_Z_SCALE;
   for(int r=0;r<4;++r)for(int c=0;c<4;++c)q.world[r*4+c]=world.Elements[c][r];const auto&m=p.material.matdesc;std::memcpy(q.diffuse,&m.diffuse,16);std::memcpy(q.ambient,&m.ambient,16);std::memcpy(q.specular,&m.specular,16);std::memcpy(q.emissive,&m.emissive,16);q.power=m.power;Renderer->SubmitHelperMesh(q);
  }
  log_info("[strike-render] tick=%d owner=%d frame=%d live=%zu rng=%llu",s->core.tick,i,o->GetFrame(),a->PartSysLiveParticles(),(unsigned long long)(RandomDraws()-s->initial_draws));
 }
}
void Destroy(State*s){delete s;}
}
