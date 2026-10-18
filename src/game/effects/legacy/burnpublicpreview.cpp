#include "game/effects/legacy/burnpublicpreview.h"
#include "game/effects/legacy/burnprojection.h"
#include "game/effects/burncore.h"
#include "game/effects/burnprofile.h"
#include "game/effects/effect.h"
#include "game/object/character.h"
#include "game/imagery/meshextract.h"
#include "engine/render/retailsoftwaretransform.h"
#include "engine/render/framesnap.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include "engine/core/logging.h"
#include <cmath>
#include <memory>
#include <vector>
DEFINE_BUILDER("Burn",TBurnEffect)
REGISTER_BUILDER(TBurnEffect)
void TBurnEffect::Initialize(){character=nullptr;frame=0;SetNotify(3);}
void TBurnEffect::Pulse(){TEffect::Pulse();++frame;if(random(0,34)<2 && frame<50){if(spell)spell->Damage(character);else if(character)character->Damage(random(1,2),DT_BURN);}if(character){S3DPoint p;p = IntPos(character);p.z=int(double(p.z)*double(1.4600000381469727f));SetPos(p);}}
void TBurnEffect::Notify(int32_t notify,void*ptr){if(GetSector()==ptr)return;TObjectInstance::Notify(notify,ptr);if(character && NOTIFY_DELETED(ptr,character)){SetFlags(Flags()|OF_KILL);character->ClearBurn();character=nullptr;}}
namespace burn_public_preview {
struct State {
 TObjectPtr<TCharacter>actor;TObjectPtr<TBurnEffect>owner;burn_core::State core;
 MeshHandle mesh[2]{};double accumulator=0;int64_t rendered=-1;
 ~State(){if(actor)actor->ClearBurn();if(Renderer)for(auto m:mesh)if(m)Renderer->ReleaseMeshAssetRef(m);}
};
// This fixture declares the same state14/frame0, interpolation-off actor
// control as its original reference. GetObjectMatrix samples the live render
// fraction, so use the real integer API without altering other actors' policy.
static bool IntegerBone(T3DAnimator*anim,int index,hmm_mat4&matrix){
 if(!anim || index<0 || index>=anim->NumObjects())return false;
 auto*obj=anim->GetObject(index);if(!obj)return false;
 struct Restore{bool saved=Interpolate;Restore(){Interpolate=false;}~Restore(){Interpolate=saved;}}restore;
 if(!anim->Get3DImagery()->CalcObjectMatrix(obj,14,0,nullptr,true))return false;
 matrix=obj->matrix;return true;
}
State* Spawn(const S3DPoint&origin){
 if(!Renderer)return nullptr;auto s=std::make_unique<State>();auto*ac=TObjectClass::GetClass(OBJCLASS_CHARACTER);const int at=ac?ac->FindObjType("Acolyte"):-1;if(at<0)return nullptr;
 SObjectDef a={};a.objclass=OBJCLASS_CHARACTER;a.objtype=short(at);a.pos=origin;s->actor.reset(dynamic_cast<TCharacter*>(ac->NewObject(&a)));if(!s->actor || s->actor->Health()!=102 || s->actor->Radius()!=16)return nullptr;
 s->actor->Face(96); // Declared shared facing; native constructor has96 in this fixture.
 auto* anim=dynamic_cast<T3DAnimator*>(s->actor->GetAnimator());if(!anim || anim->NumObjects()!=22 || static_cast<TObjectInstance*>(s->actor.get())->GetState()!=14)return nullptr;
 auto* ec=TObjectClass::GetClass(OBJCLASS_EFFECT);const int et=ec?ec->FindObjType(0x55471bffu):-1;if(et<0)return nullptr;SObjectDef e={};e.objclass=OBJCLASS_EFFECT;e.objtype=short(et);e.pos=origin;s->owner.reset(dynamic_cast<TBurnEffect*>(ec->NewObject(&e)));if(!s->owner)return nullptr;s->owner->Set(s->actor.get());s->owner->OnScreen();
 auto*im=dynamic_cast<T3DImagery*>(s->owner->GetImagery());if(!im || !burn_profile::Matches(im->GetResFilename(),im->GetBody(),im->GetResSize(),im->NumStates(),im->GetAniLength(0),im->GetAniFlags(0)))return nullptr;
 for(int i=0;i<2;++i){std::vector<SMeshVertex>v;std::vector<uint16_t>f;if(!ExtractSubMeshTextureSlot(im,i,i+1,v,f)||v.size()!=4||f.size()!=6)return nullptr;s->mesh[i]=Renderer->RegisterMeshAsset(0x4255524e00000000ull+i,v.data(),4,f.data(),6,im->GetTextureHandle(i));if(!s->mesh[i])return nullptr;Renderer->AddMeshAssetRef(s->mesh[i]);}
 if(std::string(StartupTestMode)=="vfx")SeedRandom(2);
 log_info("[native-domain] enabled=%d camera_z=0 genuineBurn publicSet",int(StartupVfxNativeDomain));log_info("[burn-public] typed_id=55471bff actor=Acolyte state14frame0 facing96 integer_pose_control=1 source_seed_after_actor=%d poseFraction=%.9g",int(std::string(StartupTestMode)=="vfx"),TTime::LegacyFrameFraction());return s.release();
}
bool Alive(State*s){return s && s->owner && !s->core.done && !(s->owner->Flags()&OF_KILL);}
void Advance(State*s,double seconds){if(!Alive(s)||!std::isfinite(seconds)||seconds<=0)return;s->accumulator+=seconds;while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;s->owner->NextFrame();s->owner->Pulse();if(!Alive(s))return;auto*anim=dynamic_cast<T3DAnimator*>(s->actor->GetAnimator());hmm_mat4 root;const auto p=IntPos(s->actor);BuildRetailSoftwareOwner(root,p.x,p.y,p.z,uint8_t(IntFacing(s->actor)));s->core.Advance([](int a,int b){return random(a,b);},[&](int i,hmm_mat4&m){return IntegerBone(anim,i,m);},root,22);if(s->core.done){s->owner->KillThisEffect();s->actor->ClearBurn();}}
}
void SubmitWorld(State*s){if(!Alive(s)||!Renderer||s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();int draws=0;s->core.Render([&](int pool,int slot,int object,const hmm_mat4&local){const auto&particle=(pool?s->core.smoke:s->core.fire)[slot];const auto world=burn_projection::World(local,particle.position,StartupVfxNativeDomain);SHelperMeshSubmit d={};d.mesh=s->mesh[object];d.retail_lighting=1;d.additive_blend=true;d.retail_additive_cull_none=true;d.retail_software_projection=StartupVfxNativeDomain;d.retail_normal_z_scale=StartupVfxNativeDomain?1.f:1.f/WORLD3D_Z_SCALE;for(int r=0;r<4;++r)for(int c=0;c<4;++c)d.world[r*4+c]=world.Elements[c][r];Renderer->SubmitHelperMesh(d);++draws;});log_info("[burn-phase] tick=%d to_add=%d draws=%d health=%d actor_state=%d frame=%d rng_draws=%llu",s->core.frame,s->core.to_add,draws,s->actor->Health(),static_cast<TObjectInstance*>(s->actor.get())->GetState(),s->actor->GetFrame(),(unsigned long long)RandomDraws());
 if(std::string(StartupTestMode)=="vfx")for(int pool=0;pool<2;++pool)for(int slot=0;slot<70;++slot){const auto&p=(pool?s->core.smoke:s->core.fire)[slot];if(p.used)log_info("[burn-slot] tick=%d pool=%d slot=%d p=%.9g,%.9g,%.9g scale=%.9g life=%d span=%d",s->core.frame,pool,slot,p.position.X,p.position.Y,p.position.Z,p.scale.X,p.life,p.span);}}
void Destroy(State*s){delete s;}
}
