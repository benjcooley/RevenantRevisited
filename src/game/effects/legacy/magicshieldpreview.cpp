#include "game/effects/legacy/magicshieldpreview.h"
#include "game/effects/magicshieldcore.h"
#include "game/effects/magicshieldprofile.h"
#include "game/effects/effect.h"
#include "game/object/character.h"
#include "engine/render/retailsoftwaretransform.h"
#include "game/imagery/meshextract.h"
#include "game/app/testconfig.h"
#include "engine/core/logging.h"
#include <cmath>
#include <cstring>
#include <memory>
class TMagicShieldOriginal:public TEffect {
 magicshield_core::Owner lifecycle;
public:
 TMagicShieldOriginal(TObjectImagery*i):TEffect(i){Initialize();}
 TMagicShieldOriginal(SObjectDef*d,TObjectImagery*i):TEffect(d,i){Initialize();}
 void Initialize(){SetCommandDone(false);lifecycle={};auto p=IntPos(*this);p.z=int(RetailSoftwareOwnerZ(p.z));SetPos(p);}
 void Pulse()override{TEffect::Pulse();auto*actor=spell?dynamic_cast<TCharacter*>(spell->GetInvoker()):nullptr;lifecycle.Pulse(spell && actor,spell?spell->VariantData()->mindamage:0,[&](float v){actor->SetMagicResistance(v);});if(lifecycle.dead)KillThisEffect();}
 int Phase()const{return lifecycle.phase;}int Countdown()const{return lifecycle.countdown;}
};
DEFINE_BUILDER("MagicShield",TMagicShieldOriginal)
REGISTER_BUILDER(TMagicShieldOriginal)
namespace magicshield_preview {
struct State {TObjectPtr<TCharacter>actor;std::unique_ptr<TSpell>spell;TObjectPtr<TMagicShieldOriginal>owner;magicshield_core::State core;MeshHandle meshes[2]{};S3DMat materials[2]{};double accumulator=0;int tick=0;int64_t rendered=-1;~State(){if(owner)owner->SetSpell(nullptr);if(Renderer)for(auto m:meshes)if(m)Renderer->ReleaseMeshAssetRef(m);}};
State* Spawn(const S3DPoint&origin,bool fire_flash){
 if(!Renderer || !SpellList.Initialize())return nullptr;const char*name=fire_flash?"Fire Flash":"Aura";auto*data=SpellList.GetSpellDataByName(name);auto*variant=SpellList.GetVariantDataByName(name);if(!data||!variant||std::strcmp(variant->name,name)||variant->mindamage!=(fire_flash?50:0))return nullptr;
 auto s=std::make_unique<State>();auto*characters=TObjectClass::GetClass(OBJCLASS_CHARACTER);int type=characters?characters->FindObjType("Acolyte"):-1;if(type<0)return nullptr;SObjectDef d={};d.objclass=OBJCLASS_CHARACTER;d.objtype=short(type);d.pos=origin;s->actor.reset(dynamic_cast<TCharacter*>(characters->NewObject(&d)));if(!s->actor||s->actor->Health()!=102||s->actor->Radius()!=16)return nullptr;s->actor->Face(96);
 auto*effects=TObjectClass::GetClass(OBJCLASS_EFFECT);type=effects?effects->FindObjType(0xad92bc1bu):-1;if(type<0)return nullptr;d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(dynamic_cast<TMagicShieldOriginal*>(effects->NewObject(&d)));if(!s->owner||s->owner->ObjId()!=0xad92bc1bu)return nullptr;TObjectInstance*target=s->actor.get();S3DPoint source=origin;s->spell=std::make_unique<TSpell>(s->actor.get(),&target,1,&source,data,variant);s->owner->SetSpell(s->spell.get());s->owner->OnScreen();
 auto*im=dynamic_cast<T3DImagery*>(s->owner->GetImagery());if(!im||!magicshield_profile::Matches(im->GetResFilename(),im->GetBody(),im->GetResSize(),im->NumStates(),im->GetAniLength(0),im->GetAniFlags(0))||im->NumObjects()!=6||im->NumVerts()!=1218||im->NumFaces()!=2076||im->NumMaterials()!=6||im->NumTextures()!=6)return nullptr;
 for(int i=0;i<2;++i){std::vector<SMeshVertex>v;std::vector<uint16_t>f;if(!ExtractSubMeshTextureSlot(im,i,i+1,v,f)||v.size()!=size_t(i?4:402)||f.size()!=size_t(i?6:2070))return nullptr;im->GetMaterial(i,&s->materials[i]);s->meshes[i]=Renderer->RegisterMeshAsset(0x4d534849454c4400ull+i,v.data(),int(v.size()),f.data(),int(f.size()),im->GetTextureHandle(i));if(!s->meshes[i])return nullptr;Renderer->AddMeshAssetRef(s->meshes[i]);S3DTex t={};im->GetTexture(i,&t);log_info("[magicshield-texture] object=%d frames=%d %ux%u masks=%x,%x,%x,%x",i,t.numframes,t.desc.width,t.desc.height,t.desc.pixelFormat.dwRBitMask,t.desc.pixelFormat.dwGBitMask,t.desc.pixelFormat.dwBBitMask,t.desc.pixelFormat.dwRGBAlphaBitMask);}
 if(std::string(StartupTestMode)=="vfx")SeedRandom(2);s->core.Initialize([](int a,int b){return random(a,b);});log_info("[native-domain] enabled=%d camera_z=0 MagicShield genuine named spell",int(StartupVfxNativeDomain));log_info("[magicshield-public] typed_id=ad92bc1b variant=%s minimum=%d selector_storage0_control=1 source_seed_after_actor=%d",name,variant->mindamage,int(std::string(StartupTestMode)=="vfx"));return s.release();
}
bool Alive(State*s){return s&&s->owner&&!(s->owner->Flags()&OF_KILL);}
void Advance(State*s,double dt){if(!Alive(s)||!std::isfinite(dt)||dt<=0)return;s->accumulator+=dt;while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;if(!Alive(s))return;++s->tick;s->owner->NextFrame();s->owner->Pulse();if(!Alive(s)){log_info("[magicshield-kill] tick=%d phase=%d resistance=%.9g",s->tick,s->owner->Phase(),s->actor->GetMagicResistance());return;}s->core.Animate(s->owner->Phase(),IntPos(s->actor),[](int a,int b){return random(a,b);});}}
void SubmitWorld(State*s){if(!Alive(s)||!Renderer||s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();int draws=0;s->core.Render(s->owner->Phase(),[&](int object,int,const hmm_mat4&world){SHelperMeshSubmit q={};q.mesh=s->meshes[object];q.retail_lighting=3;q.retail_source_alpha=true;q.retail_alpha_source_quantize=StartupVfxNativeDomain;q.retail_software_projection=StartupVfxNativeDomain;const auto&m=s->materials[object].matdesc;std::memcpy(q.diffuse,&m.diffuse,16);std::memcpy(q.ambient,&m.ambient,16);std::memcpy(q.specular,&m.specular,16);std::memcpy(q.emissive,&m.emissive,16);q.power=m.power;for(int r=0;r<4;++r)for(int c=0;c<4;++c)q.world[r*4+c]=world.Elements[c][r];Renderer->SubmitHelperMesh(q);++draws;});log_info("[magicshield-phase] tick=%d phase=%d countdown=%d angle=%.9g scale=%.9g,%.9g,%.9g resistance=%.9g draws=%d rng=%llu",s->tick,s->owner->Phase(),s->owner->Countdown(),s->core.angle,s->core.scale.X,s->core.scale.Y,s->core.scale.Z,s->actor->GetMagicResistance(),draws,(unsigned long long)RandomDraws());}
void Destroy(State*s){delete s;}
}
