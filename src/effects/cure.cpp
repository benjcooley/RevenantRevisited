#include "cure.h"

#include <algorithm>
#include <cmath>

namespace cure_retail
{
namespace
{
// Folded retail angle conversion at 0x5a39f0. Do not use the partsys 6.28.
constexpr float kDegree = 0.01745329238474369f;
int Draw(const RandomRange& random, int a, int b)
{
    if (a == b) return a; // retail 0x483300 does not consume rand()
    if (a > b) std::swap(a, b);
    return random(a, b);
}
float Angle(const RandomRange& random) { return float(Draw(random,0,359))*kDegree; }
void AngularVelocity(Particle& p, int maximum, const RandomRange& random)
{
    const int axes=Draw(random,0,2);
    if (axes==0) { p.angvel.x=float(Draw(random,5,maximum))*kDegree; p.angvel.y=float(Draw(random,5,maximum))*kDegree; }
    else if (axes==1) { p.angvel.x=float(Draw(random,5,maximum))*kDegree; p.angvel.z=float(Draw(random,5,maximum))*kDegree; }
    else { p.angvel.y=float(Draw(random,5,maximum))*kDegree; p.angvel.z=float(Draw(random,5,maximum))*kDegree; }
}
// Third row of the source Rx*Ry*Rz matrix, with float storage between matrix
// operations, transformed by v=(0,0,dist). Owner/world coordinates stay out.
Vec3 Orbit(const Vec3& angle, float dist)
{
    // Retail matrix rotation computes double trig then stores its float
    // coefficients. The float overload can differ by one binary32 ULP.
    const float cx=float(std::cos(double(angle.x))), sx=float(std::sin(double(angle.x)));
    const float cy=float(std::cos(double(angle.y))), sy=float(std::sin(double(angle.y)));
    const float cz=float(std::cos(double(angle.z))), sz=float(std::sin(double(angle.z)));
    const float x=cx*sy, y=-sx, z=cx*cy;
    const float rx=float(static_cast<long double>(x)*cz-static_cast<long double>(y)*sz);
    const float ry=float(static_cast<long double>(x)*sz+static_cast<long double>(y)*cz);
    return {rx*dist,ry*dist,z*dist};
}
void Rotate(Particle& p)
{
    p.angle.x+=p.angvel.x; p.angle.y+=p.angvel.y; p.angle.z+=p.angvel.z;
}
void PositionBall(Particle& p)
{
    const Vec3 v=Orbit(p.angle,p.dist);
    p.pos={p.pivot.x+v.x,p.pivot.y+v.y,p.pivot.z+v.z/2.0f};
    if(p.pos.z<10.0f) p.pos.z=10.0f;
}
}
void Initialize(State& state, const RandomRange& random)
{
    state=State{};
    for(int i=0;i<kBallCount;++i)
        state.balls[i].starttime=i==0?0:Draw(random,0,15);
}
void AdvanceOneTick(State& state, const RandomRange& random)
{
    if(!state.alive) return;
    ++state.framenum;
    for(int i=0;i<kBallCount;++i)
    {
        Particle& ball=state.balls[i];
        if(ball.state==0)
        {
            if(ball.starttime>0) --ball.starttime;
            if(ball.starttime==0)
            {
                ball.pivot={0,0,35.0f+float((i-kBallCount/2)*(100/kBallCount))};
                ball.size=10.0f;
                ball.angle={Angle(random),Angle(random),Angle(random)};
                ball.maindist=60.0f; ball.disttheta=0;
                ball.color=Draw(random,0,0);
                AngularVelocity(ball,9,random);
                ball.state=1;
                ++state.numactiveballs;
            }
        }
        if(ball.state==1)
        {
            Rotate(ball);
            ball.size+=.1f;
            // Native FST stores the phase but FCOS still consumes its
            // unrounded x87 sum before the final distance store.
            const double phase=double(ball.disttheta)+double(.04f);
            ball.disttheta=float(phase);
            ball.dist=float(double(ball.maindist)+10.0*std::cos(phase));
            if(!ball.linedup)
            {
                if(ball.maindist>2.0f) ball.maindist-=1.5f;
                else
                {
                    ball.maindist=2.0f; ball.linedup=1;
                    if(++state.ballslinedup==kBallCount)
                        for(auto& other:state.balls) other.state=2;
                }
            }
            PositionBall(ball);
        }
        else if(ball.state==2)
        {
            Rotate(ball);
            // Literal snapshot condition, retained in retail 0x4e109d..10cb.
            const float sizevel=(ball.pivot.z>30.0f||ball.pivot.z<40.0f)?1.0f:0.0f;
            if(ball.size>.1f) ball.size-=sizevel; else ball.size=.1f;
            if(ball.pivot.z>35.0f) ball.pivot.z-=.5f;
            else if(ball.pivot.z<35.0f) ball.pivot.z+=.5f;
            PositionBall(ball);
        }
    }
    int newswirleys=0;
    if(state.ballslinedup==kBallCount)
    {
        state.sm[state.killing].state=4;
        if(++state.killing>=kParticleCount) state.alive=false;
    }
    else newswirleys=1;
    for(auto& particle:state.sm)
    {
        if(particle.state==0&&newswirleys)
        {
            // Source uses ordinal 0..count-1, not a compact active-ball list.
            particle.whichball=Draw(random,0,state.numactiveballs-1);
            particle.color=state.balls[particle.whichball].color;
            particle.angle={Angle(random),Angle(random),Angle(random)};
            particle.disttheta=Angle(random); // unused later; source RNG draw stays
            particle.size=.01f;
            AngularVelocity(particle,10,random);
            particle.state=3;
            --newswirleys;
        }
        if(particle.state==3)
        {
            Rotate(particle);
            const Particle& ball=state.balls[particle.whichball];
            particle.dist=ball.size;
            if(state.ballslinedup!=kBallCount)
            {
                if(particle.size<.5f) particle.size+=.03f;
                else particle.size=.5f;
            }
            particle.pivot=ball.pos;
            const Vec3 v=Orbit(particle.angle,particle.dist);
            particle.pos={particle.pivot.x+v.x,particle.pivot.y+v.y,particle.pivot.z+v.z};
        }
    }
}
int VisibleCount(const State& state)
{
    if(!state.alive) return 0;
    int count=0;
    for(const auto& p:state.sm) if(p.state==3&&p.size>0) ++count;
    return count;
}
}

#ifndef CURE_STATE_ONLY
#include "../3dimage.h"
#include "../character.h"
#include "../imagery.h"
#include "../logging.h"
#include "../mappane.h"
#include "../math3d.h"
#include "../meshextract.h"
#include "../revutils.h"
#include "../spell.h"
#include "../time.h"

#include <cstring>
#include <string>
#include <vector>

extern TRenderer* Renderer;

namespace
{
constexpr uint32_t kCureId=0x152dafefu;
constexpr const char* kCureAsset="Magic\\Cure.I3D";
int RetailRandom(int a,int b) { return ::random(a,b); }

class TCureRuntimeComponent final : public TFlipbookBillboardComponent
{
public:
    const char* ComponentName() const override { return "cure_reference"; }
    void Submit(TRenderer&,const TObjectInstance& owner) const override
    {
        if(auto* effect=dynamic_cast<const TCureEffect_Bespoke*>(&owner)) effect->Submit(DebugMode());
    }
protected:
    void OnUpdate() override
    {
        if(auto* effect=dynamic_cast<TCureEffect_Bespoke*>(Owner())) effect->Advance(TTime::DeltaTime());
    }
};

class TCureObjectBuilder final : public TObjectBuilder
{
public:
    TCureObjectBuilder():TObjectBuilder("Cure") {}
    TObjectInstance* Build(TObjectImagery* imagery) override { return new TCureEffect_Bespoke(imagery); }
    TObjectInstance* Build(SObjectDef* def,TObjectImagery* imagery) override
    { return new TCureEffect_Bespoke(def,imagery); }
};
class TCureAnimatorBuilder final : public T3DAnimatorBuilder
{
public:
    TCureAnimatorBuilder():T3DAnimatorBuilder("Cure") {}
    T3DAnimator* Build(TObjectInstance* owner) override { return new T3DAnimator(owner); }
    void AttachComponents(TObjectInstance* owner) override
    {
        if(!owner||owner->ObjClass()!=OBJCLASS_EFFECT||owner->ObjId()!=kCureId||owner->GetMapIndex()<=0||
           (owner->Flags()&OF_KILL)||owner->GetComponent<TCureRuntimeComponent>()) return;
        if(auto* effect=dynamic_cast<TCureEffect_Bespoke*>(owner)) effect->Initialize(true);
        else log_error("[cure-runtime] exact Cure owner was not built by its typed object builder");
    }
};
TCureObjectBuilder cure_object_builder;
TCureAnimatorBuilder cure_animator_builder;
}

struct TCureEffect_Bespoke::Impl
{
    cure_retail::State state;
    T3DImagery* imagery=nullptr; // borrowed; owner retains/releases its imagery
    MeshHandle mesh=0;
    S3DMat material{};
    std::vector<SMeshVertex> vertices;
    std::vector<uint16_t> indices;
    bool initialized=false,simulation_initialized=false,logged_tick=false;
    bool first_spell_pulse=true,aim_bound=false;
    bool runtime_owned=false;
    double accumulator=0;
    ~Impl() { if(Renderer&&mesh) Renderer->ReleaseMeshAssetRef(mesh); }
    bool BindMesh()
    {
        if(mesh) return true;
        if(!Renderer||!imagery) return false;
        S3DTex texture{};
        imagery->GetTexture(material.texture,&texture);
        if(texture.htexture==kInvalidTexture) return false;
        const uint64_t key=0x4355524500000000ull^(uint64_t(uint32_t(imagery->ImageryId()))<<16);
        mesh=Renderer->RegisterMeshAsset(key,vertices.data(),int32_t(vertices.size()),
                                      indices.data(),int32_t(indices.size()),texture.htexture);
        if(mesh) Renderer->AddMeshAssetRef(mesh);
        return mesh!=0;
    }
};

TCureEffect_Bespoke::TCureEffect_Bespoke(TObjectImagery* imagery)
    :TEffect(imagery),impl_(std::make_unique<Impl>()) {}
TCureEffect_Bespoke::TCureEffect_Bespoke(SObjectDef* def,TObjectImagery* imagery)
    :TEffect(def,imagery),impl_(std::make_unique<Impl>()) {}
TCureEffect_Bespoke::~TCureEffect_Bespoke()=default;

void TCureEffect_Bespoke::FirstSpellPulse()
{
    if(!impl_->first_spell_pulse||!GetSpell()) return;
    auto* invoker=dynamic_cast<TCharacter*>(GetSpell()->GetInvokerRef().Get());
    if(!invoker) return;
    TCharacter* target=invoker->Fighting();
    if(!target)
        for(TMapIterator it(static_cast<PSRect>(nullptr),CHECK_NOINVENT,OBJSET_CHARACTER);it;it++)
        {
            auto* candidate=dynamic_cast<TCharacter*>(it.Item());
            if(!candidate||candidate==invoker||candidate->IsDead()||
               ::Distance(invoker->Pos(),candidate->Pos())>400||!invoker->IsEnemy(candidate)) continue;
            target=candidate;
            break;
        }
    if(target) { SetPos(target->Pos()); target->SetPoisoned(true); }
    impl_->first_spell_pulse=false;
}

void TCureEffect_Bespoke::BindFacing()
{
    if(impl_->aim_bound) return;
    angle=0;
    if(auto* linked=GetSpell())
        if(auto* invoker=linked->GetInvokerRef().Get())
        {
            auto* target=linked->GetTargetRef().Get();
            angle=target&&target!=invoker?ConvertToFacing(Pos(),target->Pos()):invoker->GetFace();
        }
    impl_->state.facing=float(angle);
    impl_->aim_bound=true;
}

void TCureEffect_Bespoke::FinishSpell()
{
    auto* linked=GetSpell();
    // 0x4e1230 reads SpellData, not VariantData. Zero chance skips cleanup.
    // This target is specifically spell target0, not the first-Pulse enemy.
    if(linked&&linked->SpellData()&&linked->PoisonChance()!=0&&linked->GetTargetNum()>0)
        if(auto* target=dynamic_cast<TCharacter*>(linked->GetTargetRef().Get()))
            target->SetPoisoned(false);
    KillThisEffect();
    if(linked) linked->Kill();
}

void TCureEffect_Bespoke::Initialize(bool attach)
{
    if(attach && (GetMapIndex()<0 || ObjClass()!=OBJCLASS_EFFECT || ObjId()!=0x152dafefu)) return;
    if(Flags()&OF_KILL) return;
    if(!impl_->initialized)
    {
        auto* imagery=dynamic_cast<T3DImagery*>(GetImagery());
        if(!imagery||imagery->NumObjects()<1||imagery->NumObjVerts(0)!=4||
           !ExtractSubMesh(imagery,0,impl_->vertices,impl_->indices)||impl_->indices.size()!=6)
        { log_error("[cure-runtime] required authored object0 quad unavailable"); return; }
        S3DObj object{}; imagery->GetObject(0,&object);
        if(object.material<0||object.material>=imagery->NumMaterials()) return;
        imagery->GetMaterial(object.material,&impl_->material);
        if(impl_->material.texture<0||impl_->material.texture>=imagery->NumTextures()) return;
        impl_->imagery=imagery;
        impl_->initialized=true;
    }
    // CPU binding is enough for simulation; GPU texture/mesh binding retries
    // at Submit. Initialization consumes the four original delay draws once.
    if(!impl_->simulation_initialized)
    {
        cure_retail::Initialize(impl_->state,RetailRandom);
        angle=0; // first Advance binds the live spell context, or explicit zero
        impl_->state.facing=0;
        impl_->simulation_initialized=true;
    }
    if(attach&&!GetComponent<TCureRuntimeComponent>())
    {
        impl_->runtime_owned=true;
        S3DTex texture{};
        impl_->imagery->GetTexture(impl_->material.texture,&texture);
        auto component=std::make_unique<TCureRuntimeComponent>();
        component->Configure(texture.htexture,64,64,1,1,1,1,1,true,true);
        AddComponent(std::move(component));
        const auto origin=Pos();
        log_info("[cure-runtime] attached id=%08x map_index=%d origin=(%d,%d,%d) swirls=80 balls=5 visible_object=0",
                 ObjId(),GetMapIndex(),origin.x,origin.y,origin.z);
    }
    else if(attach)
    {
        // The map replacement path waits for a valid texture before calling
        // a visual component. Retry readiness without resetting simulation.
        auto* component=GetComponent<TCureRuntimeComponent>();
        if(component&&component->Texture()==kInvalidTexture)
        {
            S3DTex texture{};
            impl_->imagery->GetTexture(impl_->material.texture,&texture);
            if(texture.htexture!=kInvalidTexture)
                component->Configure(texture.htexture,64,64,1,1,1,1,1,true,true);
        }
    }
}

TCureEffect_Bespoke* TCureEffect_Bespoke::SpawnForTest_BESPOKE(const S3DPoint& origin)
{
    int32_t id=TObjectImagery::FindImagery(kCureAsset);
    if(id<0) { std::string path=kCureAsset; id=TObjectImagery::RegisterImagery(path.data()); }
    TObjectImagery* imagery=id<0?nullptr:TObjectImagery::LoadImagery(id);
    if(!dynamic_cast<T3DImagery*>(imagery))
    { if(imagery) TObjectImagery::FreeImagery(imagery); log_error("[cure-runtime] cannot load '%s'",kCureAsset); return nullptr; }
    auto* effect=new TCureEffect_Bespoke(imagery);
    effect->ForcePos(origin);
    effect->SetMapIndex(MapPane.MakeIndex());
    effect->Initialize(false);
    if(!effect->impl_->initialized) { delete effect; return nullptr; }
    return effect;
}

void TCureEffect_Bespoke::Pulse()
{
    FirstSpellPulse();
    Initialize(true);
    TEffect::Pulse();
}
void TCureEffect_Bespoke::Advance(double seconds)
{
    if(!impl_->initialized||(Flags()&OF_KILL)||!std::isfinite(seconds)||seconds<=0) return;
    // Cover a newly attached component before the first owner Pulse without
    // applying first-Pulse mutation twice. Aiming remains initialized once.
    FirstSpellPulse();
    BindFacing();
    impl_->accumulator+=seconds;
    while(impl_->accumulator+1e-12>=TTime::LegacyFrameSeconds&&impl_->state.alive)
    {
        impl_->accumulator-=TTime::LegacyFrameSeconds;
        SetCommandDone(false);
        cure_retail::AdvanceOneTick(impl_->state,RetailRandom);
    }
    if(!impl_->logged_tick&&impl_->state.framenum>0)
    {
        impl_->logged_tick=true;
        log_info("[cure-runtime] first simulation tick id=%08x map_index=%d frame=%d visible=%d",
                 impl_->runtime_owned?ObjId():0x152dafefu,GetMapIndex(),impl_->state.framenum,VisibleParticles());
    }
    if(!impl_->state.alive)
    {
        FinishSpell();
        log_info("[cure-runtime] finished id=%08x map_index=%d frame=%d killing=%d marked_kill=1",
                 impl_->runtime_owned?ObjId():0x152dafefu,GetMapIndex(),impl_->state.framenum,impl_->state.killing);
    }
}

void TCureEffect_Bespoke::Submit(EFxDebugMode mode) const
{
    if(!Renderer||!IsAlive()||!impl_->BindMesh()) return;
    // Helper mesh preserves normals/materials. It has no FX debug-mode
    // shader ladder; do not synthesize replacement colors for nonnormal modes.
    if(mode!=EFxDebugMode::Normal) return;
    for(const auto& particle:impl_->state.sm)
    {
        if(particle.state!=3||particle.size<=0) continue;
        hmm_mat4 local{},world{};
        MtxClear(&local);
        MtxRotateX(&local,-2.094395160675049f); // bits c0060a92
        MtxRotateZ(&local,-.7853981852531433f); // bits bf490fdb
        MtxRotateZ(&local,float(-double(impl_->state.facing)/256.0*6.283185308));
        const hmm_vec3 size={particle.size,particle.size,particle.size}; MtxScale(&local,&size);
        const hmm_vec3 position={particle.pos.x,particle.pos.y,particle.pos.z}; MtxTranslate(&local,&position);
        // Source local matrix then live owner matrix, once. This is mesh
        // projection, not the FX procedural projector; no ad-hoc Z divisor.
        MtxMultiply(&world,&local,&Transform().Matrix());
        SHelperMeshSubmit submit{};
        submit.mesh=impl_->mesh; submit.additive_blend=true; submit.shadow_plane=false;
        submit.retail_lighting=1; // Audited object0 is RGB565: Blue software modulation.
        // Renderer consumes four rows multiplying a column vector.
        for(int row=0;row<4;++row) for(int col=0;col<4;++col)
            submit.world[row*4+col]=world.Elements[col][row];
        const auto& material=impl_->material.matdesc;
        std::memcpy(submit.diffuse,&material.diffuse,4*sizeof(float));
        std::memcpy(submit.ambient,&material.ambient,4*sizeof(float));
        std::memcpy(submit.specular,&material.specular,4*sizeof(float));
        std::memcpy(submit.emissive,&material.emissive,4*sizeof(float));
        submit.power=material.power;
        submit.sort_depth=world.Elements[3][2];
        Renderer->SubmitHelperMesh(submit);
    }
}
void TCureEffect_Bespoke::TickAndSubmitForTest_BESPOKE(EFxDebugMode mode)
{ Advance(TTime::DeltaTime()); Submit(mode); }
bool TCureEffect_Bespoke::IsAlive() const
{ return impl_->initialized&&impl_->state.alive&&!(Flags()&OF_KILL); }
int TCureEffect_Bespoke::SimulationTicks() const { return impl_->state.framenum; }
int TCureEffect_Bespoke::VisibleParticles() const { return cure_retail::VisibleCount(impl_->state); }
#endif
