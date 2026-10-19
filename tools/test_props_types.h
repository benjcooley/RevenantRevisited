// Test fixture for tools/reflect/gen.py + TPropertyHost. An annotated
// struct in the exact shape a prefab component will use.
#pragma once

#include "../src/engine/refl/refl.h"

#include <HandmadeMath.h>

#include <string>

enum class ETestBlend : uint8_t
{
    Alpha,
    Additive,
    AdditiveStraight,   // DEF spelling: additive_straight
};

struct STestMesh
{
    REFL_IDS(1, 19)
    PROP(1)                  bool        visible   = true;
    PROP(2)                  SRgba       diffuse   = {1, 1, 1, 1};
    PROP_NOSAVE(3, "UV scroll rate") float   uv_scroll = 0.0f;
    PROP(4)                  hmm_vec3    pos       = {{0, 0, 0}};
    PROP(5)                  int32_t     frame     = 0;
    PROP(6)                  std::string object;
    PROP(7)                  ETestBlend  blend     = ETestBlend::Alpha;
    PROP_READONLY(8)         int32_t     generation = 7;

    int32_t changed_count = 0;
    uint16_t last_changed = 0;
    void PropChanged(uint16_t id) { ++changed_count; last_changed = id; }
};

// A class with no annotations is simply not in the registry.
struct STestEmpty
{
    int32_t x = 0;
};

// Inheritance: a derived annotated class sees its base's properties.
struct STestDerived : public STestMesh
{
    REFL_IDS(20, 29)
    PROP(20) float extra = 2.5f;
};

// Private annotated fields: the class befriends the generated thunks.
class STestPrivate
{
  public:
    REFL_FRIEND(STestPrivate)
    [[nodiscard]] float Speed() const { return speed_; }
  private:
    PROP(1, "Units per second") float   speed_ = 3.0f;
    PROP(2)                     int32_t count_ = 2;
};

// Accessor-backed properties (the TNode shape: state lives behind
// setters, e.g. in a TTransform).
struct STestAccessor
{
    PROP_FN(1, hmm_vec3,    pos,   LocalPos, SetLocalPos);
    PROP_FN(2, float,       scale, Scale,    SetScale, def(1.0f));
    PROP_FN(3, std::string, name,  Name,     );                   // read-only: no setter
    PROP_FN_HIDDEN(4, ETestBlend, blend, Blend, SetBlend, def(ETestBlend::Alpha));

    hmm_vec3    LocalPos() const { return pos_; }
    void        SetLocalPos(const hmm_vec3& p) { pos_ = p; ++sets; }
    float       Scale() const { return scale_; }
    void        SetScale(float s) { scale_ = s; ++sets; }
    const char* Name() const { return "node"; }
    ETestBlend  Blend() const { return blend_; }
    void        SetBlend(ETestBlend b) { blend_ = b; ++sets; }

    int32_t sets = 0;
  private:
    hmm_vec3   pos_   = {{0, 0, 0}};
    float      scale_ = 1.0f;
    ETestBlend blend_ = ETestBlend::Alpha;
};
