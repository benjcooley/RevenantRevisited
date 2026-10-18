#include "regenerationspellpreview.h"
#include "../effect.h"
#include "../character.h"
#include "../3dimage.h"
#include "../mapmanager.h"
#include "../gamemap.h"
#include "../mappane.h"
#include "../testconfig.h"
#include "../logging.h"
#include <cmath>
#include <memory>

namespace regeneration_spell_preview {
// The shipped Regeneration as this fixture reviews it: a Strike spell with
// the plain direct FOLLOW control (no bone, pattern, multiple targets,
// damage, poison, sound or imagery override).
static bool IsShippedRegeneration(TStrikeSpell& strike)
{
    const SSpellData* spell=strike.SpellData();const SSpellVariant* variant=strike.VariantData();
    if(!spell || !variant || stricmp(variant->name,"Regeneration") ||
       stricmp(variant->effect,"Regeneration") || !variant->controldata)return false;
    const auto& c=*variant->controldata;
    return !stricmp(c.name,"Strike") && c.flags==0 && c.radius==0 && c.hits==1 &&
        c.duration==2880 && c.duration2==24 && c.wait==0 && c.pattern==SPAT_NONE &&
        c.shake==0 && c.repeatdamage==0 && c.rangedamage[0]==0 && c.rangedamage[1]==0 &&
        c.attach[0]==0 && c.attachmultiple==nullptr && c.sound[0]==0 && c.imagery==-1 &&
        c.hittarget==0 && c.oncaster==0 && c.attachset==0 && c.follow==1 && c.posset==1 &&
        c.repeatset==0 && c.multipletargets==0 && c.pos[0]==0 && c.pos[1]==0 && c.pos[2]==0 &&
        spell->poisonchance==0;
}
struct State {
    static constexpr int level=248;
    TGameMap* previous=nullptr;
    bool owns_map=false,done=false;
    TSafeRef<> invoker,target,owner;
    std::unique_ptr<TSpell> spell;
    S3DPoint origin{};
    int step=0;
    double accumulated=0;
    ~State() {
        spell.reset();
        if(owns_map) {
            MapPane.ReleaseCommandMapWindow();
            MapManager.SetCurrentMap(previous);
            MapManager.Evict(level);
        }
    }
};
State* Spawn(const S3DPoint& origin) {
    // This fixture temporarily owns a disposable map. It is never a gallery
    // or gameplay factory, and refuses to replace an existing cached level.
    if(std::string(StartupTestMode)!="vfx" || !Renderer || origin.x<0 || origin.y<0 ||
       MapManager.GetCached(State::level) || !SpellList.Initialize())return nullptr;
    auto s=std::make_unique<State>();s->previous=MapManager.CurrentMap();s->origin=origin;
    if(!MapManager.CreateTransient(State::level,origin.x>>SECTORWSHIFT,origin.y>>SECTORHSHIFT,
                                  (origin.x+256)>>SECTORWSHIFT,(origin.y+256)>>SECTORHSHIFT))return nullptr;
    s->owns_map=true;
    if(!MapPane.BindCommandMapWindow(State::level,origin))return nullptr;
    auto* cls=TObjectClass::GetClass(OBJCLASS_CHARACTER);const int type=cls?cls->FindObjType("Acolyte"):-1;
    if(type<0)return nullptr;
    SObjectDef def={};def.objclass=OBJCLASS_CHARACTER;def.objtype=short(type);def.level=State::level;def.pos=origin;
    s->invoker=MapPane.GetInstance(MapPane.NewObject(&def));def.pos.x+=96;
    s->target=MapPane.GetInstance(MapPane.NewObject(&def));
    auto* actor=dynamic_cast<TCharacter*>(s->invoker.Get());auto* target=dynamic_cast<TCharacter*>(s->target.Get());
    if(!actor || !target || !actor->GetSector() || !target->GetSector() || actor->Health()!=102 || actor->Radius()!=16)return nullptr;
    if(!actor->BeginFighting(target,ACTION_COMBAT) || actor->Fighting()!=target)return nullptr;
    auto* data=SpellList.GetSpellDataByName("Regeneration");auto* variant=SpellList.GetVariantDataByName("Regeneration");
    const auto* creator=FindSpellClass("Strike");if(!data || !variant || !creator)return nullptr;
    TObjectInstance* targets[]={target};S3DPoint source={0,0,0};
    s->spell.reset(creator->create(actor,targets,1,&source,data,variant,nullptr));
    auto* strike=dynamic_cast<TStrikeSpell*>(s->spell.get());if(!strike || !IsShippedRegeneration(*strike))return nullptr;
    log_info("[native-domain] enabled=%d camera_z=0 owner=actual_world_RzRxRyT particle=MODELZ",int(StartupVfxNativeDomain));
    log_info("[regeneration-spell] genuine source creator=Strike variant=Regeneration transient_level=%d origin=%d,%d,%d target=%d,%d,%d follow=invoker duration=2880 blink=24",State::level,origin.x,origin.y,origin.z,IntPos(target).x,IntPos(target).y,IntPos(target).z);
    return s.release();
}
bool Alive(State* s) {return s && s->spell && !s->done;}
void Advance(State* s,double seconds) {
    if(!Alive(s) || !std::isfinite(seconds) || seconds<=0)return;
    s->accumulated+=seconds;
    while(s->accumulated+1e-10>=TTime::LegacyFrameSeconds && Alive(s)) {
        s->accumulated-=TTime::LegacyFrameSeconds;
        s->spell->Pulse();const bool done=s->spell->Timer();
        if(s->step==8)if(auto* actor=s->invoker.Get()) {
            S3DPoint moved=s->origin;moved.x+=115;moved.y+=5;moved.z+=8;
            actor->SetPos(moved,State::level,false);
        }
        if(auto* effect=dynamic_cast<TEffect*>(s->spell->GetEffect(0))) {
            s->owner=effect;effect->NextFrame();effect->Pulse();
            const auto& light=effect->LightRamp();const auto p=IntPos(effect);
            log_info("[regeneration-owner] step=%d done=%d flags=%x pos=%d,%d,%d follow=%d remaining=%d delay=%d light=%d,%d,%d,%d resident=%d frame=%d",s->step,int(done),effect->Flags(),p.x,p.y,p.z,effect->FollowTime(),effect->Duration(),effect->StartWait(),light.in,light.out,light.value,light.peak,int(effect->GetSector()!=nullptr),effect->GetFrame());
        } else log_info("[regeneration-owner] step=%d done=%d absent=1",s->step,int(done));
        ++s->step;
        if(done) {
            s->done=true;
            if(auto* effect=s->owner.Get()) {
                log_info("[regeneration-spell-end] step=%d flags=%x bound=%d",s->step-1,effect->Flags(),int(static_cast<TEffect*>(effect)->GetSpell()!=nullptr));
                MapPane.DeleteObject(effect);s->owner.Clear();
            }
        }
    }
}
void SubmitWorld(State* s) {
    if(!Alive(s) || !Renderer)return;
    auto* owner=dynamic_cast<TEffect*>(s->owner.Get());if(!owner || owner->Flags()&(OF_KILL|OF_INVISIBLE))return;
    owner->Animate(false);
    auto* animator=dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    if(!animator || animator->PartSysUnsupported() || animator->PartSysControllerCount()!=1)return;
    animator->SubmitPartSys(*Renderer,0,1,{1,1,1},StartupVfxNativeDomain);
}
void Destroy(State* s) {delete s;}
}
