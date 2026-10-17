// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   refl.h - reflection, kept entirely outside the classes it describes *
// *            (docs/vfx/PREFAB_SYSTEM.md §2)                             *
// *************************************************************************
//
// A class that wants tweakable / serializable / animatable state does one
// thing: it annotates members.
//
//     class TMeshComponent : public TObjectComponent
//     {
//         PROP(1)                  bool        visible   = true;
//         PROP(2)                  SRgba       diffuse   = {1, 1, 1, 1};
//         PROP(3, PropFlag_NoSave) float       uv_scroll = 0.0f;
//         PROP_FN(4, hmm_vec3, pos, LocalPos, SetLocalPos);   // accessor-backed
//         void OnPropChanged(uint16_t id);                     // optional hook
//     };
//
// No base class, no macro that declares members, no virtuals. The
// annotation is the C++26 one (P3394) when the compiler has it and
// nothing otherwise; until then tools/reflect/gen.py reads the PROP lines
// and emits, outside the class, exactly what reflection would give us: a
// property table and typed get / set thunks, registered under the class's
// type_info. Everything that needs to know about properties -- the packed
// codec, the DEF bake, the editor inspector and undo, animation tracks --
// asks this registry for the type's description and works on
// (description, void* object) pairs.
//
// Annotated members must be accessible from outside the class (public),
// or exposed through PROP_FN accessors. The id is also the wire tag in
// packed buffers: append-only, never renumbered, never reused.
//
// *************************************************************************

#pragma once

#include <cctype>
#include <cstdint>
#include <cstring>
#include <string_view>
#include <typeinfo>

class TPackedWriter;
class TPackedReader;

// C++26 annotation (P3394) when the compiler has it; nothing otherwise.
#if defined(__cpp_annotations)
#  define PROP(...) [[=refl::SMeta{__VA_ARGS__}]]
#else
#  define PROP(...)
#endif
// Accessor-backed property: Getter() returns `type`, Setter(const type&)
// stores it; empty Setter = read-only. An annotation on the class.
#define PROP_FN(...)

enum class EPropType : uint8_t
{
    Bool,
    Int,
    Float,
    String,
    Vec3,
    Color,      // rgba, 0..1
    Enum,       // int payload; the table names the options
    Ref,        // soft-ref id of another node
    Trigger,    // write 1 to fire; reads as 0
};

enum EPropFlag : uint8_t
{
    PropFlag_None     = 0,
    PropFlag_ReadOnly = 1 << 0,   // shown but not writable
    PropFlag_Hidden   = 1 << 1,   // not shown in the inspector
    PropFlag_NoSave   = 1 << 2,   // runtime-only; never packed / written to DEF
};

// Color as the property system carries it (floats 0..1).
struct SRgba
{
    float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
    bool operator==(const SRgba& o) const { return r == o.r && g == o.g && b == o.b && a == o.a; }
};

// Tagged union carried through get / set. Strings are views: a get returns
// a view into the object's own storage (valid while it lives); a set
// receives a view the object copies.
struct SPropValue
{
    EPropType type = EPropType::Float;
    union
    {
        bool    b;
        int32_t i;
        float   f;
        float   v3[3];
        float   c4[4];
    };
    std::string_view str;

    SPropValue() : c4{0, 0, 0, 0} {}

    static SPropValue Bool(bool v)               { SPropValue p; p.type = EPropType::Bool;    p.b = v;  return p; }
    static SPropValue Int(int32_t v)             { SPropValue p; p.type = EPropType::Int;     p.i = v;  return p; }
    static SPropValue Float(float v)             { SPropValue p; p.type = EPropType::Float;   p.f = v;  return p; }
    static SPropValue Enum(int32_t v)            { SPropValue p; p.type = EPropType::Enum;    p.i = v;  return p; }
    static SPropValue Ref(int32_t id)            { SPropValue p; p.type = EPropType::Ref;     p.i = id; return p; }
    static SPropValue Trigger()                  { SPropValue p; p.type = EPropType::Trigger; p.b = true; return p; }
    static SPropValue String(std::string_view s) { SPropValue p; p.type = EPropType::String;  p.str = s; return p; }
    static SPropValue Vec3(float x, float y, float z)
        { SPropValue p; p.type = EPropType::Vec3; p.v3[0] = x; p.v3[1] = y; p.v3[2] = z; return p; }
    static SPropValue Color(float r, float g, float b, float a)
        { SPropValue p; p.type = EPropType::Color; p.c4[0] = r; p.c4[1] = g; p.c4[2] = b; p.c4[3] = a; return p; }

    [[nodiscard]] float AsFloat() const
    {
        switch (type)
        {
            case EPropType::Bool:
            case EPropType::Trigger: return b ? 1.0f : 0.0f;
            case EPropType::Int:
            case EPropType::Enum:
            case EPropType::Ref:     return float(i);
            case EPropType::Float:   return f;
            case EPropType::Vec3:    return v3[0];
            case EPropType::Color:   return c4[0];
            case EPropType::String:  return 0.0f;
        }
        return 0.0f;
    }
};

[[nodiscard]] constexpr int32_t PropLaneCount(EPropType t)
{
    switch (t)
    {
        case EPropType::Vec3:   return 3;
        case EPropType::Color:  return 4;
        case EPropType::String: return 0;
        default:                return 1;
    }
}

// Lane suffix -> index ("x","y","z" / "r","g","b","a"); -1 if unknown.
[[nodiscard]] inline int32_t PropLaneIndex(EPropType t, std::string_view lane)
{
    if (lane.size() != 1) return -1;
    const char c = char(std::tolower(static_cast<unsigned char>(lane[0])));
    switch (t)
    {
        case EPropType::Vec3:  return c == 'x' ? 0 : c == 'y' ? 1 : c == 'z' ? 2 : -1;
        case EPropType::Color: return c == 'r' ? 0 : c == 'g' ? 1 : c == 'b' ? 2 : c == 'a' ? 3 : -1;
        default:               return -1;
    }
}

[[nodiscard]] inline float PropGetLane(const SPropValue& v, int32_t lane)
{
    switch (v.type)
    {
        case EPropType::Vec3:  return (lane >= 0 && lane < 3) ? v.v3[lane] : 0.0f;
        case EPropType::Color: return (lane >= 0 && lane < 4) ? v.c4[lane] : 0.0f;
        default:               return v.AsFloat();
    }
}

inline void PropSetLane(SPropValue& v, int32_t lane, float x)
{
    switch (v.type)
    {
        case EPropType::Vec3:    if (lane >= 0 && lane < 3) v.v3[lane] = x; break;
        case EPropType::Color:   if (lane >= 0 && lane < 4) v.c4[lane] = x; break;
        case EPropType::Bool:
        case EPropType::Trigger: v.b = x >= 0.5f; break;
        case EPropType::Int:
        case EPropType::Enum:
        case EPropType::Ref:     v.i = int32_t(x); break;
        case EPropType::Float:   v.f = x; break;
        case EPropType::String:  break;
    }
}

namespace refl {

// What the annotation carries (the generator reads the same arguments).
struct SMeta
{
    uint16_t id;
    uint8_t  flags = PropFlag_None;
};

struct SEnumOption
{
    int32_t     value;
    const char* name;      // DEF-text spelling (snake_case of the enumerator)
};

struct SProp
{
    uint16_t           id           = 0;
    const char*        name         = "";
    EPropType          type         = EPropType::Float;
    uint8_t            flags        = PropFlag_None;
    const SEnumOption* options      = nullptr;   // Enum only
    int32_t            option_count = 0;
};

// One per annotated class, emitted by the generator and registered at
// static-init time. `base` is linked by name on first lookup so a derived
// class sees its base's properties too.
struct SType
{
    const char*           name       = "";
    const char*           base_name  = "";
    const std::type_info* info       = nullptr;
    const SProp*          props      = nullptr;
    int32_t               prop_count = 0;
    bool (*set)(void* obj, uint16_t id, const SPropValue& v)  = nullptr;
    bool (*get)(const void* obj, uint16_t id, SPropValue& out) = nullptr;
    const SType*          base       = nullptr;
};

// Registry. Generated code registers; everyone else looks up.
void Register(SType& type);
[[nodiscard]] const SType* Find(const std::type_info& info);
[[nodiscard]] const SType* Find(std::string_view class_name);

// Description of an object's dynamic type (polymorphic classes) or
// static type (plain structs). nullptr if the type is not annotated.
template <class T>
[[nodiscard]] const SType* Of(const T& obj) { return Find(typeid(obj)); }
template <class T>
[[nodiscard]] const SType* Of() { return Find(typeid(T)); }

// Property lookup, walking the base chain.
[[nodiscard]] const SProp* FindProp(const SType& type, std::string_view name);
[[nodiscard]] const SProp* FindProp(const SType& type, uint16_t id);
[[nodiscard]] const SEnumOption* FindEnumOption(const SProp& prop, std::string_view name);
[[nodiscard]] const char* EnumName(const SProp& prop, int32_t value);   // "" if unknown

// Typed access. Unknown id or type mismatch -> false (callers treat an
// unknown id as a retired field, never as a runtime error).
bool Set(const SType& type, void* obj, uint16_t id, const SPropValue& value);
bool Get(const SType& type, const void* obj, uint16_t id, SPropValue& out);

// Lane-generic read / write for tracks and sliders: fetch the whole value,
// patch one lane, write it back.
bool SetLane(const SType& type, void* obj, uint16_t id, int32_t lane, float x);
bool GetLane(const SType& type, const void* obj, uint16_t id, int32_t lane, float& out);

// Packed codec over the table. Pack skips NoSave / ReadOnly fields; Unpack
// applies every field it recognises and skips the rest.
void Pack(const SType& type, const void* obj, TPackedWriter& w);
void Unpack(const SType& type, void* obj, TPackedReader& r);

// Static-init registrar used by generated code.
struct SRegistrar
{
    explicit SRegistrar(SType& type) { Register(type); }
};

} // namespace refl
