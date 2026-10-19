// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   lightcomponent.cpp - a point light carried by a node                *
// *************************************************************************

#include "game/world/lightcomponent.h"

#include "engine/scene/intworld.h"
#include "game/world/3dscene.h"
#include "game/world/dls.h"
#include "game/world/mappane.h"
#include "game/imagery/imagery.h"
#include "game/object/object.h"

#include <cmath>

// DLS brightness routine (gives brightness given distance)
extern double GetLightBrightness(int32_t dist, int32_t intensity, int32_t multiplier);

SRgba TLightComponent::ColorRgba() const
{
    return SRgba{ color_.red / 255.0f, color_.green / 255.0f, color_.blue / 255.0f, 1.0f };
}

void TLightComponent::SetColorRgba(const SRgba& c)
{
    auto byte = [](float v) { return uint8_t(std::lround(std::fmin(std::fmax(v, 0.0f), 1.0f) * 255.0f)); };
    SetColor(SColor{ byte(c.r), byte(c.g), byte(c.b) });
}

S3DPoint TLightComponent::WorldPos() const
{
    S3DPoint p = Owner() ? IntPos(*Owner()) : S3DPoint{0, 0, 0};
    p += offset_;
    return p;
}

void TLightComponent::ScreenRect(SRect& r) const
{
    S3DPoint slpos;
    WorldToScreen(WorldPos(), slpos);
    r.left = slpos.x - intensity_;
    r.top = slpos.y - intensity_;
    r.right = r.left + intensity_ + intensity_;
    r.bottom = r.top + intensity_ + intensity_;
}

void TLightComponent::Redraw()
{
    if (intensity_ == 0)
        return;
    SRect r;
    ScreenRect(r);
    MapPane.AddBgUpdateRect(r, BGDRAW_LIGHTS);
}

// REVSYNC: TObjectInstance::GetIllumination -- the brightness a character
// standing at `p` gets from this light, out of the DLS brightness tables.
int32_t TLightComponent::IlluminationAt(const S3DPoint& p) const
{
    if (intensity_ <= 0)
        return 0;

    S3DPoint d = p;
    d.z += LIGHTINGCHARHEIGHT;
    d -= Owner() ? IntPos(*Owner()) : S3DPoint{0, 0, 0};

    if (abs(d.x) > intensity_ || abs(d.y) > intensity_)
        return 0;

    float dist = (float)sqrt((double)(d.x * d.x + d.y * d.y));
    dist = (float)sqrt((double)(dist * dist + d.z * d.z));
    return (int32_t)(min(GetLightBrightness((int32_t)dist, intensity_, multiplier_), 1.0) * 255.0);
}

// ---- Setters ------------------------------------------------------------

void TLightComponent::SetIntensity(int32_t intensity)
{
    LOCKOBJECTS;    // Prevent update system from drawing any lights right now

    const bool bigger = intensity > intensity_;
    if (!bigger)
        Redraw();

    intensity_ = uint8_t(intensity);

    if (scene_index_ != -1)
        Scene3D.SetLightIntensity(scene_index_, intensity_);

    if (bigger)
        Redraw();

    UNLOCKOBJECTS;  // Allow update system to draw lights now
}

void TLightComponent::SetMultiplier(int32_t multiplier)
{
    LOCKOBJECTS;

    multiplier_ = short(multiplier);

    // The light-table entry names the colour with its multiplier: take a
    // fresh one.
    if (table_id_ != -1)
    {
        FreeLightIndex(table_id_);
        table_id_ = NewLightIndex(color_, multiplier_);
    }

    Redraw();

    UNLOCKOBJECTS;
}

void TLightComponent::SetOffset(const S3DPoint& offset)
{
    LOCKOBJECTS;

    Redraw();

    offset_ = offset;

    if (scene_index_ != -1)
    {
        Scene3D.SetLightPosition(scene_index_, WorldPos());
        Redraw();
    }

    UNLOCKOBJECTS;
}

void TLightComponent::SetColor(SColor color)
{
    if (color.red == color_.red && color.green == color_.green && color.blue == color_.blue)
        return;

    LOCKOBJECTS;

    color_ = color;

    if (scene_index_ != -1)
        Scene3D.SetLightColor(scene_index_, color);

    if (table_id_ != -1)
    {
        FreeLightIndex(table_id_);
        table_id_ = NewLightIndex(color, multiplier_);
    }

    Redraw();

    UNLOCKOBJECTS;
}

// REVSYNC: TObjectInstance::DrawLight @ 0x00471930.
void TLightComponent::Draw(TSurface* surface, bool resetid)
{
    if (intensity_ <= 0)
        return;

    if (TObjectInstance* owner = OwnerInstance())
        if (TObjectImagery* imagery = owner->GetImagery())
            imagery->DrawLight(owner, surface);

    S3DPoint slpos;
    WorldToScreen(WorldPos(), slpos);

    if (resetid || table_id_ == -1)
        table_id_ = NewLightIndex(color_, multiplier_);

    DrawStaticLightNoNormals(slpos, color_, intensity_, surface, table_id_);
}

// ---- The scene light ----------------------------------------------------

void TLightComponent::EnterScene()
{
    if (scene_index_ == -1)
        scene_index_ = Scene3D.AddLight(WorldPos(), color_, intensity_, multiplier_);
}

void TLightComponent::LeaveScene()
{
    if (scene_index_ != -1)
    {
        Scene3D.DeleteLight(scene_index_);
        scene_index_ = -1;
    }
}

void TLightComponent::OwnerMoved()
{
    if (scene_index_ != -1)
        Scene3D.SetLightPosition(scene_index_, WorldPos());
}

// ---- The light-table id -------------------------------------------------

// REVSYNC: TObjectInstance::DrawLight @ 0x00471930 (0x004719af..0x004719cf)
// -- a light drawn without a light-table id gets one from NewLightIndex
// (0x0041d890): one of the four colours the DLS light byte can name
// (docs/LIGHTING_FIDELITY.md §2.6). Retail also re-asked on a full redraw
// (the pane's dirty flag), after RedrawAll had cleared every id's use.
int32_t TLightComponent::AcquireTableId()
{
    if (table_id_ == -1)
        table_id_ = NewLightIndex(color_, multiplier_);
    return table_id_;
}

// REVSYNC: FreeLightIndex @ 0x0041d9e0, from OffScreen and the destructor.
void TLightComponent::ReleaseTableId()
{
    if (table_id_ != -1)
    {
        FreeLightIndex(table_id_);
        table_id_ = -1;
    }
}

void TLightComponent::OnDetach()
{
    LeaveScene();
    ReleaseTableId();
}
