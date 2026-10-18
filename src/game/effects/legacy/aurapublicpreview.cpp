#include "game/effects/legacy/aurapublicpreview.h"
#include "game/effects/auracore.h"
#include "game/effects/auraprofile.h"
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
DEFINE_BUILDER("Aura",TAuraEffect)
REGISTER_BUILDER(TAuraEffect)
void TAuraEffect::Initialize(){first_time=true;}
void TAuraEffect::Pulse(){TEffect::Pulse();if(first_time && spell){spell->SetDefense(spell->VariantData()->mindamage);spell->SetOffense(spell->VariantData()->maxdamage);first_time=false;}S3DPoint p=GetCharacter()?IntPos(GetCharacter()):IntPos(*this);SetPos(p);if(spell){spell->SetDefense(spell->VariantData()->mindamage);spell->SetOffense(spell->VariantData()->maxdamage);}}
namespace aura_public_preview {
struct State {
 TObjectPtr<TCharacter>actor;std::unique_ptr<TSpell>spell;TObjectPtr<TAuraEffect>owner;aura_core::State core;
 MeshHandle mesh{};double accumulator=0;int64_t rendered=-1;
 ~State(){if(owner)owner->SetSpell(nullptr);if(Renderer && mesh)Renderer->ReleaseMeshAssetRef(mesh);}
};
// Match the declared original Interpolate=false integer-pose control through
// the real source CalcObjectMatrix API. Restore the ordinary global setting
// before returning; no saved/native pose or skeleton is injected.
static bool IntegerBone(T3DAnimator*anim,int index,hmm_mat4&matrix){
 if(!anim || index<0 || index>=anim->NumObjects())return false;
 auto*obj=anim->GetObject(index);if(!obj)return false;
 struct Restore{bool saved=Interpolate;Restore(){Interpolate=false;}~Restore(){Interpolate=saved;}}restore;
 if(!anim->Get3DImagery()->CalcObjectMatrix(obj,14,0,nullptr,true))return false;
 matrix=obj->matrix;return true;
}
State* Spawn(const S3DPoint&origin){
 if(!Renderer || !SpellList.Initialize())return nullptr;auto*data=SpellList.GetSpellDataByName("Aura");auto*variant=SpellList.GetVariantDataByName("Aura");if(!data || !variant || variant->mindamage!=0 || variant->maxdamage!=0)return nullptr;auto s=std::make_unique<State>();auto*ac=TObjectClass::GetClass(OBJCLASS_CHARACTER);const int at=ac?ac->FindObjType("Acolyte"):-1;if(at<0)return nullptr;
 SObjectDef a={};a.objclass=OBJCLASS_CHARACTER;a.objtype=short(at);a.pos=origin;s->actor.reset(dynamic_cast<TCharacter*>(ac->NewObject(&a)));if(!s->actor || s->actor->Health()!=102 || s->actor->Radius()!=16)return nullptr;
 s->actor->Face(96); // Declared shared facing; native constructor has96 in this fixture.
 auto* anim=dynamic_cast<T3DAnimator*>(s->actor->GetAnimator());if(!anim || anim->NumObjects()!=22 || static_cast<TObjectInstance*>(s->actor.get())->GetState()!=14)return nullptr;
 auto* ec=TObjectClass::GetClass(OBJCLASS_EFFECT);const int et=ec?ec->FindObjType(0x00ab1d1du):-1;if(et<0)return nullptr;SObjectDef e={};e.objclass=OBJCLASS_EFFECT;e.objtype=short(et);e.pos=origin;s->owner.reset(dynamic_cast<TAuraEffect*>(ec->NewObject(&e)));if(!s->owner)return nullptr;TObjectInstance*target=s->actor.get();S3DPoint source=origin;s->spell=std::make_unique<TSpell>(s->actor.get(),&target,1,&source,data,variant);s->owner->SetSpell(s->spell.get());s->owner->OnScreen();
 auto*im=dynamic_cast<T3DImagery*>(s->owner->GetImagery());if(!im || !aura_profile::Matches(im->GetResFilename(),im->GetBody(),im->GetResSize(),im->NumStates(),im->GetAniLength(0),im->GetAniFlags(0)))return nullptr;
 for(int index=0;index<22;++index){hmm_mat4 m;if(!IntegerBone(anim,index,m))return nullptr;log_info("[aura-actor-pose] object=%d ok=1 matrix=%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g",index,m.Elements[0][0],m.Elements[0][1],m.Elements[0][2],m.Elements[0][3],m.Elements[1][0],m.Elements[1][1],m.Elements[1][2],m.Elements[1][3],m.Elements[2][0],m.Elements[2][1],m.Elements[2][2],m.Elements[2][3],m.Elements[3][0],m.Elements[3][1],m.Elements[3][2],m.Elements[3][3]);}
 log_info("[aura-resource] size=%u objects=%d verts=%d faces=%d materials=%d textures=%d frames=%d flags=%x",unsigned(im->GetResSize()),im->NumObjects(),im->NumVerts(),im->NumFaces(),im->NumMaterials(),im->NumTextures(),im->GetAniLength(0),im->GetAniFlags(0));
 std::vector<SMeshVertex>v;std::vector<uint16_t>f;if(!ExtractSubMeshTextureSlot(im,0,1,v,f))return nullptr;s->mesh=Renderer->RegisterMeshAsset(0x4155524100000000ull,v.data(),int(v.size()),f.data(),int(f.size()),im->GetTextureHandle(0));if(!s->mesh)return nullptr;Renderer->AddMeshAssetRef(s->mesh);
 if(std::string(StartupTestMode)=="vfx")SeedRandom(2);
 log_info("[native-domain] enabled=%d camera_z=0 genuineAura publicSetSpell",int(StartupVfxNativeDomain));log_info("[aura-public] typed_id=00ab1d1d actor=Acolyte state14frame0 facing96 integer_pose_control=1 source_CalcObjectMatrix_Interpolate0=1 selector_zero_control=1 source_seed_after_actor=%d poseFraction=%.9g",int(std::string(StartupTestMode)=="vfx"),TTime::LegacyFrameFraction());return s.release();
}
bool Alive(State*s){return s && s->owner && !s->core.done && !(s->owner->Flags()&OF_KILL);}
void Advance(State*s,double seconds){if(!Alive(s)||!std::isfinite(seconds)||seconds<=0)return;s->accumulator+=seconds;while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;s->owner->NextFrame();s->owner->Pulse();if(!Alive(s))return;auto*anim=dynamic_cast<T3DAnimator*>(s->actor->GetAnimator());hmm_mat4 root;const auto p=IntPos(s->actor);BuildRetailSoftwareOwner(root,p.x,p.y,p.z,uint8_t(IntFacing(s->actor)));// Original constructor storage in this cached-boot case observes limit0.
 s->core.Advance([](int a,int b){return random(a,b);},[&](int i,hmm_mat4&m){return IntegerBone(anim,i,m);},root,0,p);if(s->core.done){s->owner->KillThisEffect();s->spell->Kill();}}
}
void SubmitWorld(State*s){if(!Alive(s)||!Renderer||s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();int draws=0;hmm_mat4 owner;const auto p=IntPos(s->owner);BuildRetailSoftwareOwner(owner,p.x,p.y,p.z,0);s->core.Render([&](int,const hmm_mat4&local){hmm_mat4 world;MtxMultiply(&world,const_cast<hmm_mat4*>(&local),&owner);SHelperMeshSubmit d={};d.mesh=s->mesh;d.retail_lighting=1;d.additive_blend=true;d.retail_additive_cull_none=true;d.retail_software_projection=StartupVfxNativeDomain;for(int r=0;r<4;++r)for(int c=0;c<4;++c)d.world[r*4+c]=world.Elements[c][r];Renderer->SubmitHelperMesh(d);++draws;});log_info("[aura-phase] tick=%d to_add=%d draws=%d defense=%d offense=%d rng_draws=%llu",s->core.frame,s->core.to_add,draws,s->spell->GetDefense(),s->spell->GetOffense(),(unsigned long long)RandomDraws());
 if(std::string(StartupTestMode)=="vfx")for(int i=0;i<100;++i){const auto&p=s->core.particles[i];if(p.used)log_info("[aura-slot] tick=%d slot=%d p=%.9g,%.9g,%.9g scale=%.9g life=%d span=%d",s->core.frame,i,p.position.X,p.position.Y,p.position.Z,p.scale.X,p.life,p.span);}}
void Destroy(State*s){delete s;}
}
