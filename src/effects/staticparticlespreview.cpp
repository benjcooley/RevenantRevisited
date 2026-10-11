#include "staticparticlespreview.h"
#include "../3dimage.h"
#include "../effect.h"
#include "../logging.h"
#include "../object.h"
#include "../renderer.h"
#include "../testconfig.h"
#include "../time.h"
#include <cmath>
#include <memory>

namespace static_particles_preview {
struct State {
    std::unique_ptr<TObjectInstance> owner;
    TSafeRef<> identity;
    int prototype = 0;
    double accumulated = 0;
};

State* Spawn(const S3DPoint& origin, uint32_t id)
{
    if (!Renderer || (id != 0xaeaeeb30u && id != 0xaeaeeb33u &&
                      id != 0xaeaeeb29u && id != 0xad92bd38u && id != 0x10ac03deu &&
                      id != 0xe0a3bc43u && id != 0x0c052638u)) return nullptr;
    auto context = std::make_unique<State>();
    auto* effect_class = TObjectClass::GetClass(OBJCLASS_EFFECT);
    const int type = effect_class ? effect_class->FindObjType(id) : -1;
    if (type < 0) return nullptr;
    SObjectDef definition{};
    definition.objclass = OBJCLASS_EFFECT;
    definition.objtype = short(type);
    definition.pos = origin;
    context->owner.reset(effect_class->NewObject(&definition));
    auto* owner = context->owner.get();
    if (!owner || owner->ObjId() != id || owner->GetState() != 0 ||
        owner->GetMapIndex() <= 0 || !dynamic_cast<TEffect*>(owner)) return nullptr;
    context->identity = owner;
    context->prototype = id == 0xad92bd38u ? 1 : 0;
    owner->OnScreen();
    auto* imagery = dynamic_cast<T3DImagery*>(owner->GetImagery());
    auto* animator = dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    if (!imagery || !animator || !imagery->HasRetailStaticParticleProfile(id) ||
        animator->PartSysUnsupported() || animator->PartSysControllerCount() != 1)
        return nullptr;
    owner->Animate(false);
    log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT particle=MODELZ",
             int(StartupVfxNativeDomain));
    log_info("[static-particle-preview] actual owner id=%08x map_index=%d prototype=%d controllers=1 origin=(%d,%d,%d)",
             id, owner->GetMapIndex(), context->prototype, origin.x, origin.y, origin.z);
    return context.release();
}

void Advance(State* context, double seconds)
{
    if (!context || !context->owner || !std::isfinite(seconds) || seconds <= 0) return;
    context->accumulated += seconds;
    while (context->accumulated + 1e-10 >= TTime::LegacyFrameSeconds) {
        context->accumulated -= TTime::LegacyFrameSeconds;
        auto* owner = context->identity.Get();
        if (!owner || owner != context->owner.get() || owner->GetState() != 0) return;
        owner->NextFrame();
        if (owner->GetFlags() & OF_KILL) { context->owner.reset(); return; }
        owner->Pulse();
    }
}

void SubmitWorld(State* context, EFxDebugMode)
{
    if (!context || !Renderer || !context->owner) return;
    auto* owner = context->identity.Get();
    if (!owner || owner != context->owner.get() || owner->GetState() != 0) return;
    owner->Animate(false);
    auto* animator = dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    if (!animator || animator->PartSysUnsupported()) return;
    // Literal assets have only this two-face prototype. All emitter/helper
    // objects have zero faces; there is no base mesh to synthesize or light.
    animator->SubmitPartSys(*Renderer, context->prototype, 1, {1, 1, 1}, StartupVfxNativeDomain);
}
void Destroy(State* context) { delete context; }
}
