#include "vfxreviewspawn.h"
#include "revenant.h"
#include "object.h"
#include "effect.h"
#include "3dimage.h"

#include <cstring>
#include <initializer_list>

namespace {
bool ReviewProjectileType(uint32_t id)
{
    // Actual missile/beam identities, not a substring guess from the label.
    switch (id) {
        case 0x63fd382au: // FireBall
        case 0x63fd3827u: // YFireBall: shared preview class, generic map factory
        case 0x113803f4u: // Photon
        case 0xb1c4c90fu: // IceBolt
        case 0x10da54d0u: // LightStrip
        case 0xad92bd1eu: // queenarrow
        case 0xad92bd1fu: // arroweffect
        case 0xd000f005u: // headfireball
            return true;
        default: return false;
    }
}

bool ReviewKnownNullSpellBuilder(const char* name)
{
    // These active builders allocate TEffect or the null-safe current runtime
    // leaves. Legacy effect2/effect3/missileeffect/strip registrations under
    // #if0 and SpawnForTest-only classes do not create extra map factories.
    static constexpr const char* names[] = {
        "EFFECT", "StillWater", "FlowWater", "BendWater1", "BendWater2",
        "SewerWater", "Wave", "WaveS", "WaveM", "speaker", "FLAME",
        "FireBall", "Ripple", "Drip", "Cure", "Mist", "SymGlow", "Pixie",
        "Waterfall", "Water", "FireFlash", "FIRECONE", "Streamer",
        "FireSwarm", "FaultFire", "MistFog", "SetVortex"
    };
    if (!name) return false;
    for (const char* known : names)
        if (stricmp(name, known) == 0) return true;
    return false;
}

bool ReviewParticleRenderer(T3DImagery& image, uint32_t id)
{
    // Match RefreshPartSysControllers' exact runtime admissions. Grammar
    // success alone does not enable arbitrary emitter/controller identities.
    return (id == 0xd0c0f035u && image.HasGoldPartSysProfile()) ||
           (id == 0xad92bd29u && image.HasCombatFlashStart1PartSysProfile()) ||
           (id == 0x5be39ae0u && image.HasRetailMightPartSysProfile()) ||
           (id == 0x82aeb30fu && image.HasRetailImmortalmightPartSysProfile()) ||
           (id == 0xb0e024dfu && image.HasRetailFmasteryPartSysProfile()) ||
           image.HasRetailSpeedFamilyPartSysProfile(id) ||
           image.HasRetailStaticParticleProfile(id) ||
           (id >= 0xd0c0f036u && id <= 0xd0c0f039u);
}

bool ReviewGenericBuilder(const char* name)
{
    // These aliases all construct TEffect, without a custom visual runtime.
    for(const char* alias : {"EFFECT", "StillWater", "FlowWater", "BendWater1",
        "BendWater2", "SewerWater", "Wave", "WaveS", "WaveM", "speaker"})
        if(stricmp(name,alias)==0)return true;
    return false;
}
}

VfxReviewSpawnInfo DescribeVfxReviewSpawn(uint32_t type_id)
{
    VfxReviewSpawnInfo info;
    info.projectile = ReviewProjectileType(type_id);
    auto* effect_class = TObjectClass::GetClass(OBJCLASS_EFFECT);
    if (!effect_class) {
        info.description = "EFFECT catalog not loaded";
        return info;
    }
    info.object_type = effect_class->FindObjType(type_id);
    if (info.object_type < 0) {
        info.description = "Type missing from EFFECT catalog";
        return info;
    }
    const auto* entry = effect_class->GetObjType(info.object_type);
    if (!entry || entry->uniqueid != type_id || !entry->objbuilder || entry->imageryid < 0) {
        info.description = "EFFECT factory or imagery mapping unavailable";
        return info;
    }
    info.builder_name = entry->objbuilder->GetTypeName();
    if (!ReviewKnownNullSpellBuilder(info.builder_name)) {
        info.description = "Unknown map constructor; label only";
        return info;
    }
    info.safe_factory = true;
    info.supports_endpoints = type_id == 0x63fd382au &&
                             stricmp(info.builder_name, "FireBall") == 0;
    info.renderer_supported = !ReviewGenericBuilder(info.builder_name);
    info.uses_effect_runtime = info.renderer_supported;
    if (info.projectile && !info.supports_endpoints) {
        info.safe_factory = false;
        info.description = "Projectile endpoint launch unavailable; label only";
        return info;
    }
    info.description = info.supports_endpoints ? "FireBall: original missile Pulse and endpoint aim" :
                       info.projectile ? "Endpoint launch unavailable; inspect current map drawable only" :
                       info.renderer_supported ? "Current map factory; normal authored lifecycle" :
                       "Generic map factory; drawable/controller support inspected after spawn";
    return info;
}

VfxReviewSpawnInfo ConfigureVfxReviewSpawn(TObjectInstance& effect,
                                         const S3DPoint& source,
                                         const S3DPoint& destination)
{
    if (effect.ObjClass() != OBJCLASS_EFFECT) {
        VfxReviewSpawnInfo info;
        info.description = "Spawned object is not an EFFECT";
        return info;
    }
    VfxReviewSpawnInfo info = DescribeVfxReviewSpawn(effect.ObjId());
    if (!info.safe_factory) return info;
    auto* fireball = dynamic_cast<TFireBallEffect*>(&effect);
    if (info.supports_endpoints && !fireball) {
        info.safe_factory = false;
        info.supports_endpoints = false;
        info.description = "Map factory did not create the expected projectile class";
        return info;
    }
    if (fireball) {
        info.projectile = true;
        info.supports_endpoints = fireball->SetProjectileEndpoints(source, destination);
        info.renderer_supported = true;
        info.description = info.supports_endpoints ? "FireBall: original missile Pulse and endpoint aim" :
                           "Endpoint setup rejected: non-launch state or coincident points";
    }
    auto* image = dynamic_cast<T3DImagery*>(effect.GetImagery());
    if (!image) {
        info.renderer_supported = false;
        info.description = "No current 3D effect drawable";
        return info;
    }
    // Complete lazy animator/component attachment before testing readiness.
    // Asset faces alone do not establish a working effect runtime.
    const int objects = image->NumObjects();
    image->AttachAnimatorComponents(&effect);
    bool faces = false, particles = false, unknown_controller = false;
    for (int i = 0; i < objects; ++i) {
        const char* name = image->GetObjectName(i);
        if (name && name[0] != '*' && image->NumObjFaces(i) > 0) faces = true;
    }
    for (int i = 0; i < image->NumTags(); ++i) {
        const auto* tag = image->GetTag(i);
        if (!tag || !tag->name) { unknown_controller = true; continue; }
        if (std::strcmp(tag->name, "play") == 0) ++info.authored_sound_tags;
        if (tag->state != -1 && tag->state != 0) continue;
        if (stricmp(tag->name, "partsys") == 0) particles = true;
        else if (stricmp(tag->name, "play") && stricmp(tag->name, "beg") &&
                 stricmp(tag->name, "end") && stricmp(tag->name, "blendcont") &&
                 stricmp(tag->name, "scrolltex")) unknown_controller = true;
    }
    const bool particle_renderer = particles && ReviewParticleRenderer(*image, effect.ObjId());
    const bool replacing_component = effect.GetComponent<TFlipbookBillboardComponent>() != nullptr;
    info.uses_effect_runtime = particle_renderer || replacing_component;
    if (particles && !particle_renderer && !replacing_component) {
        info.renderer_supported = false;
        info.description = "Unsupported authored particle controller; generic geometry may be visible";
    } else if (unknown_controller && !replacing_component) {
        info.renderer_supported = false;
        info.description = "Authored controller not implemented; generic geometry may be visible";
    } else if (!faces && !particle_renderer && !replacing_component &&
               !dynamic_cast<TFireBallEffect*>(&effect)) {
        info.renderer_supported = false;
        info.description = info.authored_sound_tags ? "Audio/helper only; no current visual drawable" :
                           "No current visual drawable";
    } else {
        info.renderer_supported = true;
        if (stricmp(info.builder_name,"EFFECT")==0)
            info.description = "Authored mesh/controller; normal map lifecycle";
    }
    return info;
}
