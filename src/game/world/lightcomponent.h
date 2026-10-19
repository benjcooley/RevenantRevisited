// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   lightcomponent.h - a point light carried by a node                  *
// *************************************************************************
//
// Retail kept a light as an SLightDef inside every object. It is a
// component now: a map object has one exactly when its OF_LIGHT flag is set
// (TObjectInstance::ResetFlags keeps the two in step; the flag is what the
// sector stream records), and a prefab node can carry one too.
//
// The data is retail's, in retail's types, so the sector stream writes the
// same bytes: an offset from the owner in world units, an 8-bit colour, the
// radius ("intensity") in world units, the falloff multiplier the DLS light
// tables use, and the LIGHT_* flags. The two runtime handles are the 3D
// scene's light (held while the owner is on screen) and the DLS light-table
// id (one of four, held while the light is in the drawn window).
//
// *************************************************************************

#pragma once

#include "engine/core/revengine.h"
#include "engine/render/graphics.h"
#include "engine/scene/component.h"

// Light flags
#define LIGHT_DIR       (1 << 0)                // Directional light
#define LIGHT_SUN       (1 << 1)                // Sunlight
#define LIGHT_MOON      (1 << 2)                // Moonlight

class TLightComponent final : public TComponent
{
    DEF_COMPONENT("light")
    REFL_FRIEND(TLightComponent)
    friend class TLegacyDeserialize;    // the retail stream (legacystream.h)
    friend class TLegacySerialize;

  public:
    PROP_FN(1, S3DPoint, offset,     Offset,     SetOffset,     "Offset from the owner, world units");
    PROP_FN(2, SRgba,    color,      ColorRgba,  SetColorRgba,  "Light colour");
    PROP_FN(3, int32_t,  intensity,  Intensity,  SetIntensity,  "Radius in world units (0 = off)");
    PROP_FN(4, int32_t,  multiplier, Multiplier, SetMultiplier, "Falloff multiplier for the retail light tables (<= 0: the default)");
    PROP_FN(5, int32_t,  flags,      Flags,      SetFlags,      "LIGHT_DIR / LIGHT_SUN / LIGHT_MOON bits");

    [[nodiscard]] const char* ComponentName() const override { return "light"; }

    [[nodiscard]] const S3DPoint& Offset() const     { return offset_; }
    [[nodiscard]] const SColor&   Color() const      { return color_; }
    [[nodiscard]] int32_t         Intensity() const  { return intensity_; }
    [[nodiscard]] int32_t         Multiplier() const { return multiplier_; }
    [[nodiscard]] int32_t         Flags() const      { return flags_; }
    [[nodiscard]] SRgba           ColorRgba() const;

    // Each setter keeps the drawn light, the scene light and the light-table
    // id in step, the way retail's object setters did.
    void SetOffset(const S3DPoint& offset);
    void SetColor(SColor color);
    void SetColorRgba(const SRgba& color);
    void SetIntensity(int32_t intensity);
    void SetMultiplier(int32_t multiplier);
    void SetFlags(int32_t flags)  { flags_ = uint8_t(flags); }
    void AddFlags(int32_t flags)  { flags_ |= uint8_t(flags); }

    // ---- In the world --------------------------------------------------
    [[nodiscard]] S3DPoint WorldPos() const;
        // The owner's position plus the offset
    void ScreenRect(SRect& r) const;
        // The lit extent on screen (a square of the radius around the light)
    [[nodiscard]] int32_t IlluminationAt(const S3DPoint& p) const;
        // 0..255 brightness this light gives a point, from the DLS tables
    void Redraw();
        // Marks the lit area for a background redraw
    void Draw(TSurface* surface, bool resetid);
        // The software light pass: the owner's light icon, then the DLS
        // light under its light-table id (a fresh one when the pane redraws)

    // ---- The 3D scene light: held while the owner is on screen --------
    void EnterScene();
    void LeaveScene();
    void OwnerMoved();
    [[nodiscard]] int32_t SceneIndex() const { return scene_index_; }

    // ---- The DLS light-table id: one of four, held while drawn ---------
    int32_t AcquireTableId();
    void ReleaseTableId();
    [[nodiscard]] int32_t TableId() const { return table_id_; }

  protected:
    void OnDetach() override;

  private:
    uint8_t  flags_ = 0;                    // LIGHT_* bits
    short    multiplier_ = -1;              // Falloff multiplier (<= 0: the tables' default)
    S3DPoint offset_ = {0, 0, 0};           // Offset from the owner
    SColor   color_ = {0, 0, 0};            // 8-bit RGB
    uint8_t  intensity_ = 0;                // Radius in world units
    int32_t  scene_index_ = -1;             // T3DScene light, -1 when off screen
    int32_t  table_id_ = -1;                // DLS light-table id, -1 when not drawn
};
