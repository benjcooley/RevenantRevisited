#include "game/effects/legacy/sandswirlruntime.h"
#include "game/effects/legacy/sandswirlresource.h"
#include "game/object/character.h"
#include "game/world/mappane.h"
#include "game/world/mapmanager.h"
#include "game/world/gamemap.h"
#include "game/imagery/meshextract.h"
#include "engine/render/retailsoftwaretransform.h"
#include "game/effects/quicksandcore.h"
#include "game/app/testconfig.h"
#include "engine/render/framesnap.h"
#include "engine/core/logging.h"
#include <cstring>
#include <cstdlib>
namespace sandswirl_runtime {
namespace {
struct GameRandom { int Range(int a,int b){return random(a,b);} };
class HiddenVisual final : public TFlipbookBillboardComponent {
public:
 HiddenVisual(){Configure(kInvalidTexture,1,1,1,1,1,0,0,false,true);}
 void OnAttach() override {} void Submit(TRenderer&,const TObjectInstance&) const override {}
};
class SeekingAnimator final : public T3DAnimator {
public:
 retail_seeking_particles::State core;
 explicit SeekingAnimator(TObjectInstance* owner):T3DAnimator(owner){}
 void Pulse() override {if(inst->Flags()&OF_KILL)return;T3DAnimator::Pulse();core.Advance();inst->SetCommandDone(core.done);if(core.killed)inst->SetFlag(OF_KILL);}
 bool ResetTargets(const retail_seeking_particles::Params& p){GameRandom rng;return core.ResetTargets(p,rng);}
};
static uint32_t Hash(const void*p,size_t n){uint32_t h=2166136261u;const auto*b=static_cast<const uint8_t*>(p);while(n--)h=(h^*b++)*16777619u;return h;}
class SeekingVisual final : public TFlipbookBillboardComponent {
public:
 MeshHandle meshes[4]{};
 int objects=0;bool restored_sand=false;
 ~SeekingVisual() override {if(Renderer)for(auto m:meshes)if(m)Renderer->ReleaseMeshAssetRef(m);}
 void OnAttach() override {} // owner animator Pulse is the only simulation clock
 bool Initialize(TObjectInstance& owner,bool restored){
  if(!Renderer || owner.ObjId()!=(restored?0xfecafaceu:0x14db0f2eu))return false;
  objects=restored?1:4;restored_sand=restored;
  auto*im=dynamic_cast<T3DImagery*>(owner.GetImagery());if(!im || im->NumObjects()!=objects || im->NumTextures()!=1 || (restored && (im->NumStates()!=1 || im->GetAniLength(0)!=1)))return false;
  S3DTex tex{};im->GetTexture(0,&tex);const auto&pf=tex.desc.pixelFormat;
  if(tex.numframes!=1 || tex.desc.width!=64 || tex.desc.height!=64 || pf.dwRGBBitCount!=16 || pf.dwRBitMask!=0xf00 || pf.dwGBitMask!=0xf0 || pf.dwBBitMask!=0xf || pf.dwRGBAlphaBitMask!=0xf000 || im->GetTextureHandle(0)==kInvalidTexture)return false;
  constexpr uint32_t hashes[]={0x9ee0e036u,0x7b4a7030u,0xf424a66au,0x597d591fu};
  for(int i=0;i<objects;++i){S3DVertex raw[4]{};im->GetObjVerts(i,raw);std::vector<SMeshVertex> v;std::vector<uint16_t> ix;
   if(Hash(raw,sizeof raw)!=(restored?0x9fb5ee98u:hashes[i]) || !ExtractSubMeshTextureSlot(im,i,1,v,ix) || v.size()!=4 || ix.size()!=6 || Hash(ix.data(),12)!=0x4d2f57c6u)return false;
   if(restored && !LegacySandResourceAllowed(owner.ObjId(),objects,im->NumStates(),im->GetAniLength(0),Hash(raw,sizeof raw),Hash(ix.data(),12)))return false;
   meshes[i]=Renderer->RegisterMeshAsset((restored?0x53574d53414e4400ull:0x53574d4150504100ull)|uint64_t(i),v.data(),4,ix.data(),6,im->GetTextureHandle(0));if(!meshes[i])return false;Renderer->AddMeshAssetRef(meshes[i]);}
  Configure(im->GetTextureHandle(0),64,64,1,1,1,0,0,false,true);if(restored)log_info("[sandswirl-sand-raster] original_scene=2 argb4444=1 source_term_quantize=%d destination_rounding_open=1",int(StartupVfxNativeDomain));return true;
 }
 void Submit(TRenderer&r,const TObjectInstance&owner) const override {
  auto*a=owner.GetComponent<SeekingAnimator>();if(!a || owner.Flags()&OF_KILL)return;
  hmm_mat4 root;const auto&o=IntPos(owner);if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,o.x,o.y,o.z,IntFacing(owner));else root=owner.Transform().Matrix();
  a->core.Render([&](int object,retail_seeking_particles::Vec p){hmm_mat4 local,world;MtxClear(&local);const hmm_vec3 pos={p.x,p.y,p.z};MtxTranslate(&local,&pos);MtxMultiply(&world,&local,&root);
   SHelperMeshSubmit q{};q.mesh=meshes[object];q.retail_lighting=3;q.retail_source_alpha=true;q.retail_alpha_source_quantize=restored_sand && StartupVfxNativeDomain;q.retail_software_projection=StartupVfxNativeDomain;q.retail_normal_z_scale=StartupVfxNativeDomain?1.f:1.f/WORLD3D_Z_SCALE;
   for(int row=0;row<4;++row)for(int col=0;col<4;++col)q.world[row*4+col]=world.Elements[col][row];r.SubmitHelperMesh(q);});
 }
};
bool AttachParticle(TObjectInstance& owner,const retail_seeking_particles::Params&p,bool restored){
 if(owner.GetComponent<TFlipbookBillboardComponent>())return false;
 auto visual=std::make_unique<SeekingVisual>();if(!visual->Initialize(owner,restored))return false;
 auto animator=std::make_unique<SeekingAnimator>(&owner);animator->Initialize();GameRandom rng;if(!animator->core.Init(p,visual->objects,rng))return false;
 owner.FreeAnimator();owner.AddComponent(std::move(animator));owner.AddComponent(std::move(visual));return true;
}
class NullQuicksandAnimator final : public T3DAnimator {
public:
 retail_quicksand::State core;
 explicit NullQuicksandAnimator(TObjectInstance*owner):T3DAnimator(owner){}
 void Pulse() override {if(inst->Flags()&OF_KILL)return;T3DAnimator::Pulse();core.Pulse();inst->SetCommandDone(false);if(!core.alive)inst->SetFlag(OF_KILL);}
};
bool AttachNullQuicksand(TObjectInstance&owner){
 auto*effect=dynamic_cast<TEffect*>(&owner);auto*im=dynamic_cast<T3DImagery*>(owner.GetImagery());
 if(!effect || effect->GetSpell() || owner.ObjId()!=0xbabefaceu || !im || im->NumObjects()!=4 || im->NumStates()!=1 || im->GetAniLength(0)!=3)return false;
 // Original Sandswirl calls public4edb30, which is ret8: no target binding.
 auto animator=std::make_unique<NullQuicksandAnimator>(&owner);animator->Initialize();owner.FreeAnimator();owner.AddComponent(std::move(animator));owner.AddComponent(std::make_unique<HiddenVisual>());return true;
}
}
struct Effect::Runtime {
 retail_sandswirl::Controller parent;TSafeRef<TObjectInstance> child;
 bool attempted=false,qualified=false,restored=false;int32_t seekz=0;
 std::vector<int32_t> quicksands,query;
};
Effect::Effect(TObjectImagery*im):TEffect(im),runtime(std::make_unique<Runtime>()){AddComponent(std::make_unique<HiddenVisual>());}
Effect::Effect(SObjectDef*d,TObjectImagery*im):TEffect(d,im),runtime(std::make_unique<Runtime>()){AddComponent(std::make_unique<HiddenVisual>());}
Effect::~Effect()=default;
bool Effect::UseQualifiedSparks(int32_t seekz){if(runtime->attempted)return false;runtime->qualified=true;runtime->restored=false;runtime->seekz=seekz;return true;}
bool Effect::UseRestoredLegacySand(int32_t seekz){if(runtime->attempted)return false;runtime->restored=true;runtime->qualified=false;runtime->seekz=seekz;return true;}
bool Effect::Ready()const{return runtime->attempted && runtime->parent.Reason()==retail_sandswirl::Stop::None;}
int Effect::ParentFrame()const{return runtime->parent.Frame();}
TObjectInstance* Effect::ParticleChild()const{return runtime->child.Get();}
const retail_seeking_particles::State* Effect::ParticleState()const{auto*c=ParticleChild();auto*a=c?c->GetComponent<SeekingAnimator>():nullptr;return a?&a->core:nullptr;}
const std::vector<int32_t>& Effect::SpawnedQuicksand()const{return runtime->quicksands;}
const std::vector<int32_t>& Effect::QueriedObjects()const{return runtime->query;}
bool Effect::InitializeRuntime(){
 if(runtime->attempted)return Ready();runtime->attempted=true;
 if(ObjId()!=0xbecefedau){runtime->parent.Start(false);SetFlag(OF_KILL);return false;}
 auto*cl=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cl?(runtime->qualified?cl->FindObjType(0x14db0f2eu):cl->FindObjType("sand")):-1;
 if(type<0 || (!runtime->qualified && !runtime->restored)){runtime->parent.Start(false);SetFlag(OF_KILL);log_warn("[sandswirl-runtime] missing intended sand dependency; no alias or static fallback");return false;}
 SObjectDef d{};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.level=MapPane.GetMapLevel();d.pos=IntPos(*this);
 const int index=MapPane.NewObject(&d);auto*child=MapPane.GetInstance(index);if(!child){runtime->parent.Start(false);SetFlag(OF_KILL);return false;}
 child->CreateAnimator();int direction=0;
 if(spell){auto*invoker=spell->GetInvokerRef().Get();auto*target=spell->GetTargetRef().Get();if(target && target!=invoker)direction=ConvertToFacing(IntPos(*this),IntPos(target));else if(invoker)direction=IntFacing(invoker);else{child->SetFlag(OF_KILL);runtime->parent.Start(false);SetFlag(OF_KILL);return false;}}
 S3DPoint vector;ConvertToVector(direction,100,vector);
 retail_seeking_particles::Params p;p.count=random(50,100);p.pos={0,0,70};
 p.pspread={50,50,50};p.dir={float(double(vector.x)/100),float(double(vector.y)/100),float(double(vector.z)/100)};p.spread={.5f,.5f,.5f};p.trails=1;p.maxstart=p.count/2;p.minlife=100;p.maxlife=150;p.killobj=1;p.objflags=15;p.seeking=1;p.seekz=runtime->seekz;p.target_count=1;p.targets[0]={0,0,100};p.turn=.5f;p.speed=5;
 if(!AttachParticle(*child,p,runtime->restored)){child->SetFlag(OF_KILL);runtime->parent.Start(false);SetFlag(OF_KILL);log_warn("[sandswirl-runtime] child particle ABI/resource not verified");return false;}
 runtime->child=child;runtime->parent.Start(true);log_info("[sandswirl-runtime] restored_legacy_sand=%d",int(runtime->restored));log_info("[sandswirl-runtime] qualified=%d parent=%d child=%d particles=%d angle=%d seekz=%d map_rng_policy=monotonic_no_rng",int(runtime->qualified),GetMapIndex(),index,p.count,direction,p.seekz);return true;
}
void Effect::Pulse(){
 if(Flags()&OF_KILL)return;TEffect::Pulse();if(!runtime->attempted && !InitializeRuntime())return;
 if(!runtime->parent.Alive())return;retail_sandswirl::Caller caller;
 if(runtime->parent.NeedsQueryNextTick()){
  caller.valid=spell && spell->GetInvokerRef().Get();caller.caster=reinterpret_cast<uintptr_t>(caller.valid?spell->GetInvokerRef().Get():nullptr);caller.origin={IntPos(*this).x,IntPos(*this).y,IntPos(*this).z};
  if(caller.valid){int indices[MAXFOUNDOBJS]{};const int count=MapPane.FindObjectsInRange(IntPos(*this),indices,400,0,OBJCLASS_CHARACTER);runtime->query.assign(indices,indices+count);
   for(int i=0;i<count && caller.count<5;++i){auto*character=dynamic_cast<TCharacter*>(MapPane.GetInstance(indices[i]));if(character && character!=spell->GetInvokerRef().Get() && !character->IsDead() && character->Health()>0){const auto&p=IntPos(character);caller.found[caller.count++]={reinterpret_cast<uintptr_t>(character),{p.x,p.y,p.z},character->Health()};}}}
 }
 const auto result=runtime->parent.Advance(caller);
 for(unsigned i=0;i<result.spawn_count;++i){auto*target=reinterpret_cast<TObjectInstance*>(result.spawn_for[i]);auto*cl=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cl?cl->FindObjType("Quicksand"):-1;if(type<0)continue;
  SObjectDef d{};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.level=MapPane.GetMapLevel();d.pos=IntPos(target);const int index=MapPane.NewObject(&d);auto*child=MapPane.GetInstance(index);
  if(child && AttachNullQuicksand(*child)){runtime->quicksands.push_back(index);log_info("[sandswirl-runtime-quicksand] parent_tick=%d child=%d null_spell=1 initializer_ret8=1",runtime->parent.Frame(),index);}else if(child)child->SetFlag(OF_KILL);
 }
 if(result.reset_targets){auto*child=runtime->child.Get();auto*anim=child?child->GetComponent<SeekingAnimator>():nullptr;if(!anim){SetFlag(OF_KILL);return;}auto p=anim->core.params;const auto&t=runtime->parent.Targets();p.seeking=t.seeking;p.target_count=int(t.count);p.turn=t.turn;p.speed=t.speed;p.autorange=t.autorange;p.hitrange=t.hitrange;
  for(unsigned i=0;i<t.count;++i)p.targets[i]={t.positions[i][0],t.positions[i][1],t.positions[i][2]};if(!anim->ResetTargets(p))SetFlag(OF_KILL);}
 if(result.kill){
  // Original4ee384..4ee402 clears the spell poison marker and completes the
  // caller once this non-light-owning effect is killed. Do not broaden the
  // generic TEffect helper used by unrelated effects.
  if(spell && spell->PoisonChance()!=0)if(auto*target=dynamic_cast<TCharacter*>(spell->GetTargetRef().Get()))target->SetPoisoned(0);
  KillThisEffect();if(spell)spell->Kill();
 }
}
}
// Exact class name, resolved normally by class.def. No child alias registered.
using TSandswirlRuntimeEffect=sandswirl_runtime::Effect;
DEFINE_BUILDER("Sandswirl", TSandswirlRuntimeEffect)
REGISTER_BUILDER(TSandswirlRuntimeEffect)
