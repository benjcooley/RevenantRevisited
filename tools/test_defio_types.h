// Fixture for the DEF reader / writer and node packing: one component type
// the way a prefab component will be declared.
#pragma once

#include "../src/objectcomponent.h"
#include "../src/refl.h"

#include <HandmadeMath.h>

#include <string>
#include <vector>

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

    // Derived state: what a constructor would have computed from the
    // fields. Set by PostLoad, so it is valid by the time OnAttach runs.
    float   radius_sq      = -1.0f;
    int32_t loaded_count   = 0;
    float   radius_at_attach = -1.0f;

    void PostLoad() { radius_sq = radius * radius; ++loaded_count; }
    void OnAttach() override { radius_at_attach = radius; }
    const char* ComponentName() const override { return "glow"; }
};

struct STestRange
{
    PROP(1) float lo = 0.0f;
    PROP(2) float hi = 1.0f;
};

struct STestKey
{
    PROP(1) std::string name;
    PROP(2) float       time  = 0.0f;
    PROP(3) float       value = 0.0f;
};

// A flat annotated struct (no node, no components): the ReadObject path,
// with a nested record (`range { }`) and a list of records (`key x { }`).
struct STestSettings
{
    PROP(1) bool    enabled = true;
    PROP(2) int32_t count   = 4;
    PROP(3) float   rate    = 0.5f;
    PROP(4) SRgba   tint    = {0.2f, 0.4f, 0.6f, 1.0f};
    PROP(5) std::string name = "default";
    PROP(6) STestRange range;
    PROP(7) std::vector<STestKey> key;
};
