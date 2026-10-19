#include "game/effects/legacy/retailbloodpreview.h"

#include "game/effects/legacy/retailblood.h"

#include "game/effects/effect.h"
#include "engine/core/logging.h"
#include "game/object/object.h"
#include "engine/render/renderer.h"
#include "game/app/revutils.h"
#include "engine/core/revtime.h"

#include <cmath>
#include <memory>

namespace retail_blood_preview {
namespace {
constexpr uint32_t kBloodTypeId = 0xddc4042eu;   // "Blood", Misc\Blood.I3D
constexpr int32_t kHeight = 50;                  // EffectBurst's default height
constexpr int32_t kFacing = 0;                   // the bleeder's facing; the spray heads away
}

struct State
{
    TObjectPtr<TObjectInstance> owner;
    TSafeRef<> identity;
    double accumulated = 0;
    uint64_t ticks = 0;
};

State* Spawn(const S3DPoint& origin)
{
    TObjectClass* effect_class = TObjectClass::GetClass(OBJCLASS_EFFECT);
    const int32_t type = effect_class ? effect_class->FindObjType(kBloodTypeId) : -1;
    if (type < 0) return nullptr;
    auto context = std::make_unique<State>();
    SObjectState definition = {};
    definition.objclass = OBJCLASS_EFFECT;
    definition.objtype = short(type);
    definition.pos = origin;
    definition.pos.z += kHeight;
    context->owner.reset(effect_class->NewObject(&definition));
    auto* blood = dynamic_cast<TBloodEffect*>(context->owner.get());
    if (!blood || blood->GetMapIndex() <= 0)
    {
        log_warn("[retail-blood-preview] the Blood type did not build a TBloodEffect");
        return nullptr;
    }
    // EffectBurst 0x004c8682: SetParams(height, facing + 0x80, 3, 5, 5, random(1, 5)).
    blood->SetParams(kHeight, kFacing + 0x80, 3, 5, 5, random(1, 5));
    context->identity = blood;
    blood->OnScreen();
    if (!dynamic_cast<TBloodAnimator*>(blood->GetAnimator()))
    {
        log_warn("[retail-blood-preview] the Blood owner has no TBloodAnimator");
        return nullptr;
    }
    int32_t drops = 0;
    for (const auto& drop : static_cast<TBloodAnimator*>(blood->GetAnimator())->Spray().Drops())
        drops += drop.used ? 1 : 0;
    log_info("[retail-blood-preview] owner map_index=%d drops=%d origin=(%d,%d,%d)",
             blood->GetMapIndex(), drops, origin.x, origin.y, origin.z);
    return context.release();
}

void Advance(State* context, double seconds)
{
    if (!context || !context->owner || !std::isfinite(seconds) || seconds <= 0) return;
    context->accumulated += seconds;
    while (context->accumulated + 1e-10 >= TTime::LegacyFrameSeconds && context->owner)
    {
        context->accumulated -= TTime::LegacyFrameSeconds;
        TObjectInstance* owner = context->identity.Get();
        if (!owner || owner != context->owner.get()) return;
        if (owner->GetFlags() & OF_KILL)
        {
            log_info("[retail-blood-preview] natural reap map_index=%d tick=%llu",
                     owner->GetMapIndex(), static_cast<unsigned long long>(context->ticks));
            context->owner.reset();
            return;
        }
        owner->Pulse();   // TEffect::Pulse -> TBloodAnimator::Pulse (the droplet tick)
        ++context->ticks;
    }
}

void Submit(State* context, EFxDebugMode mode)
{
    if (!context || !Renderer || !context->owner) return;
    TObjectInstance* owner = context->identity.Get();
    if (!owner || owner != context->owner.get()) return;
    if (auto* animator = dynamic_cast<const TBloodAnimator*>(owner->GetAnimator()))
        animator->Submit(*Renderer, mode);
}

bool Alive(State* context)
{
    if (!context || !context->owner) return false;
    auto* owner = context->identity.Get();
    return owner && owner == context->owner.get() && !(owner->GetFlags() & OF_KILL);
}

void Destroy(State* context) { delete context; }
}
