#include "mightpreview.h"
#include "../3dimage.h"
#include "../effect.h"
#include "../object.h"
#include "../renderer.h"
#include "../logging.h"
#include "../time.h"
#include <memory>
#include <cmath>
namespace might_authored_preview {
struct State{std::unique_ptr<TObjectInstance>owner;TSafeRef<>identity;double accumulated=0;};
State*Spawn(const S3DPoint&origin){
 if(!Renderer)return nullptr;auto c=std::make_unique<State>();auto*cl=TObjectClass::GetClass(OBJCLASS_EFFECT);
 const int type=cl?cl->FindObjType(0x5be39ae0u):-1;if(type<0)return nullptr;
 SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;c->owner.reset(cl->NewObject(&d));
 auto*owner=c->owner.get();if(!owner || owner->ObjId()!=0x5be39ae0u || owner->GetMapIndex()<=0 || !dynamic_cast<TEffect*>(owner))return nullptr;
 c->identity=owner;owner->OnScreen();auto*im=dynamic_cast<T3DImagery*>(owner->GetImagery());auto*a=dynamic_cast<T3DAnimator*>(owner->GetAnimator());
 if(!im || !a || !im->HasRetailMightPartSysProfile() || a->PartSysUnsupported() || a->PartSysControllerCount()!=1)return nullptr;
 owner->Animate(false);log_info("[might-preview] actualowner id=5be39ae0 map_index=%d controllers=1 emitters=1 capacity=%zu",owner->GetMapIndex(),a->PartSysStats().capacity);return c.release();
}
void Advance(State*c,double seconds){if(!c || !c->owner || !std::isfinite(seconds) || seconds<=0)return;c->accumulated+=seconds;
 while(c->accumulated+1e-10>=TTime::LegacyFrameSeconds && c->owner){c->accumulated-=TTime::LegacyFrameSeconds;auto*o=c->identity.Get();if(!o || o!=c->owner.get())return;o->NextFrame();if(o->GetFlags()&OF_KILL){c->owner.reset();return;}o->Pulse();}}
void SubmitWorld(State*c,EFxDebugMode){if(!c || !c->owner || !Renderer)return;auto*o=c->identity.Get();if(!o || o!=c->owner.get())return;o->Animate(false);auto*a=dynamic_cast<T3DAnimator*>(o->GetAnimator());if(a && !a->PartSysUnsupported())a->SubmitPartSys(*Renderer,0,1,{1,1,1});}
void Destroy(State*c){delete c;}
}
