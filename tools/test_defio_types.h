// Fixture for the DEF reader / writer and node packing: one component type
// the way a prefab component will be declared.
#pragma once

#include "../src/objectcomponent.h"
#include "../src/refl.h"

#include <HandmadeMath.h>

#include <string>

enum class ETestMode : uint8_t
{
    Pulse,
    Steady,
};

class STestGlow : public TObjectComponent
{
  public:
    DEF_COMPONENT("glow")

    PROP(1, "Glow radius in world units")   float       radius  = 10.0f;
    PROP(2, "Tint")                         SRgba       color   = {1, 1, 1, 1};
    PROP(3)                                 ETestMode   mode    = ETestMode::Pulse;
    PROP(4, "Texture asset path")           std::string texture;
    PROP_NOSAVE(5)                          float       phase   = 0.0f;

    const char* ComponentName() const override { return "glow"; }
};

// A flat annotated struct (no node, no components): the ReadObject path.
struct STestSettings
{
    PROP(1) bool    enabled = true;
    PROP(2) int32_t count   = 4;
    PROP(3) float   rate    = 0.5f;
    PROP(4) SRgba   tint    = {0.2f, 0.4f, 0.6f, 1.0f};
    PROP(5) std::string name = "default";
};
