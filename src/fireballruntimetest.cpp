#include "fireballruntimetest.h"

#include "effect.h"
#include "logging.h"
#include "mappane.h"
#include "maprenderer.h"
#include "testconfig.h"

#include <memory>
#include <set>
#include <tuple>

extern TObjectClass EffectClass;

// Run inside a fully initialized headless port, using an actual loaded map's
// walk heights, class factory and normal virtual Pulse. No console parser,
// preview tick helper, fake ground query or host-set per-tick positions.
bool RunFireballRuntimeTest(TMapRenderer& renderer)
{
    if(!StartupSceneCameraSet||!renderer.InitializeFromStartupArgs())return false;
    S3DPoint center{StartupSceneCamera[1],StartupSceneCamera[2],StartupSceneCamera[3]};
    if(!MapPane.BindCommandMapWindow(StartupSceneCamera[0],center))return false;
    const int32_t height=MapPane.GetWalkHeight(center);
    if(height<=0)
    {
        log_error("[fireball-runtime] requires an actual nonzero walk-height floor; got%d",height);
        return false;
    }
    SObjectDef definition;
    definition.objclass=EffectClass.ClassId();definition.objtype=EffectClass.FindObjType(0x63fd382au);
    definition.level=StartupSceneCamera[0];definition.pos={center.x,center.y,height+128};
    if(definition.objtype<0)return false;
    std::unique_ptr<TObjectInstance> owned(EffectClass.NewObject(&definition));
    auto* effect=dynamic_cast<TFireBallEffect*>(owned.get());
    if(!effect||effect->ObjId()!=0x63fd382au)
    {
        log_error("[fireball-runtime] actual type factory did not produce TFireBallEffect");return false;
    }
    const S3DPoint source=effect->Pos();
    S3DPoint destination{source.x+480,source.y,source.z};
    if(!effect->SetProjectileEndpoints(source,destination))return false;
    if(effect->ProjectileState().speed_fixed!=8*65536||!effect->IsAlive())return false;
    std::set<std::tuple<int,int,int>> positions;
    bool launched=false,impacted=false,killed=false,generic_did_not_move=true,state_matches=true;
    for(int tick=0;tick<192;++tick)
    {
        const S3DPoint before=effect->Pos();
        effect->Move(); // Same generic walk that the actual map invokes.
        const S3DPoint unmoved=effect->Pos();
        generic_did_not_move&=before.x==unmoved.x&&before.y==unmoved.y&&before.z==unmoved.z;
        effect->Pulse(); // Actual production virtual runtime method.
        const auto& state=effect->ProjectileState();const S3DPoint current=effect->Pos();
        positions.emplace(current.x,current.y,current.z);
        launched|=state.state==1;impacted|=state.state==2;
        state_matches&=effect->GetState()==state.state;
        killed|=(effect->GetFlags()&OF_KILL)!=0;
        log_info("[fireball-runtime] tick=%d state=%d pos=%d,%d,%d vel=%d,%d,%d accum=%d,%d,%d range=%d flags=%u alive=%d",
            tick+1,state.state,current.x,current.y,current.z,
            state.velocity_fixed.x,state.velocity_fixed.y,state.velocity_fixed.z,
            state.accumulator.x,state.accumulator.y,state.accumulator.z,
            state.range,effect->GetFlags(),effect->IsAlive());
        if(killed)break;
    }
    const bool pass=generic_did_not_move&&state_matches&&positions.size()>2&&launched&&impacted&&killed;
    log_info("[fireball-runtime] COMPLETE status=%s type=63fd382a floor=%d source=%d,%d,%d destination=%d,%d,%d distinct_positions=%zu launched=%d impacted=%d kill_requested=%d generic_move_avoids_double_step=%d object_state_matches_controller=%d preview_used=0 world_queries=actual map_reaper_verified=0 visual_parity=0",
        pass?"PASS":"FAIL",height,source.x,source.y,source.z,destination.x,destination.y,destination.z,
        positions.size(),launched,impacted,killed,generic_did_not_move,state_matches);
    return pass;
}
