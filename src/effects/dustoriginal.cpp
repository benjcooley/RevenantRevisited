#include "dustoriginal.h"
#include "../3dimage.h"
#include "../effect.h"
#include "../logging.h"
#include "../renderer.h"
#include "../testconfig.h"
#include "../time.h"
#include <cmath>
#include <memory>

namespace dust_original {
struct State {
    std::unique_ptr<TObjectInstance> owner;
    S3DPoint source{};
    double accumulated=0;

};
State* Spawn(const S3DPoint& source)
{
    if(!Renderer)return nullptr;
    auto c=std::make_unique<State>();c->source=source;
    auto* cls=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cls?cls->FindObjType(0x0c052637u):-1;
    if(type<0)return nullptr;
    SObjectDef def{};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos=source;
    c->owner.reset(cls->NewObject(&def));auto* owner=c->owner.get();
    if(!owner || owner->ObjId()!=0x0c052637u || !dynamic_cast<TEffect*>(owner))return nullptr;
    // Controlled coordinates can lie outside loaded map sectors. Establish
    // the explicit fixture transform before creating the animator/controllers.
    SetIntPos(owner, source);owner->Face(0);owner->OnScreen();
    auto* image=dynamic_cast<T3DImagery*>(owner->GetImagery());auto* animator=dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    if(!image || !animator || !image->HasRetailDustProfile() || animator->PartSysUnsupported() || animator->PartSysControllerCount()!=1)return nullptr;
    animator->SetDustGround([](int,int,int){return 1;});
    owner->Animate(false);
    log_info("[dust-original] id=0c052637 frames=20 controllers=1 source=%d,%d,%d native=%d",source.x,source.y,source.z,int(StartupVfxNativeDomain));
    log_info("[native-domain] enabled=%d camera_z=0",int(StartupVfxNativeDomain));
    return c.release();
}
void Advance(State* c,double seconds)
{
    if(!c || !c->owner || !std::isfinite(seconds) || seconds<=0)return;
    c->accumulated+=seconds;auto* o=c->owner.get();
    while(c->accumulated+1e-10>=TTime::LegacyFrameSeconds && !(o->GetFlags()&OF_KILL)){
        c->accumulated-=TTime::LegacyFrameSeconds;o->NextFrame();
        o->Pulse();
    }
}
void SubmitWorld(State* c,EFxDebugMode)
{
    if(!c || !c->owner || !Renderer || (c->owner->GetFlags()&OF_KILL))return;
    auto* o=c->owner.get();o->Animate(false);auto* a=dynamic_cast<T3DAnimator*>(o->GetAnimator());
    // Original single controller emits from its four authored roots.
    for(int prototype:{1})a->SubmitPartSys(*Renderer,prototype,1,{1,1,1},StartupVfxNativeDomain);

}
bool IsAlive(const State* c)
{
    return c && c->owner && !(c->owner->GetFlags()&OF_KILL);
}
void Destroy(State* c){delete c;}
}
