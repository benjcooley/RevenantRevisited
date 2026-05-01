// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *               iobject.h - Object + reflection interfaces              *
// *************************************************************************
//
// Generic object / reflection model used by the editor. Anything that
// wants to appear in the scene tree, the properties inspector, or be
// targeted by editor commands (cut / copy / paste / delete / undo ...)
// implements IObject. The reflection schema for the class is kept in a
// static IReflection table so per-instance access is just member-pointer
// arithmetic + a function pointer.
//
//   class TSector : public IObject {
//       static const IReflection& StaticRefl();
//       const IReflection& GetRefl() const override { return StaticRefl(); }
//       int32_t GetId()        const override { return id; }
//       const char* GetName()  const override { return name.c_str(); }
//       ... your usual fields ...
//   };
//
// Most classes register their schema once (in a .cpp) via the
// REFL_BEGIN / REFL_PROP / REFL_END trio:
//
//   REFL_BEGIN(TSector)
//       REFL_PROP_INT(level,     "Level",      EPropFlag::ReadOnly)
//       REFL_PROP_INT(sector_x,  "Sector X",   EPropFlag::None)
//       REFL_PROP_INT(sector_y,  "Sector Y",   EPropFlag::None)
//       REFL_PROP_VEC3_FLOAT(...)
//   REFL_END()
//
// IReflection also exposes the *object-level* hierarchy via GetParent /
// GetNumChildren / GetChildAt and a deletable flag, so the editor's
// scene tree can drill into any IObject without per-class glue.
//
// *************************************************************************

#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

class IObject;
class IReflection;

// ---------------------------------------------------------------------------
// Property type system
// ---------------------------------------------------------------------------

enum class EPropertyType : uint8_t
{
    Bool,
    Int,
    Float,
    String,
    Vec3Int,        // S3DPoint-ish triple of ints
    Vec3Float,
    Color,          // RGBA float [0..1]
    Enum,
    ObjectRef,      // pointer/handle to another IObject
};

enum class EPropertyFlag : uint8_t
{
    None     = 0,
    ReadOnly = 1 << 0,    // shown but not writable
    Hidden   = 1 << 1,    // hidden from inspector entirely
    Advanced = 1 << 2,    // hidden behind an "advanced" toggle
};
inline EPropertyFlag operator|(EPropertyFlag a, EPropertyFlag b)
    { return EPropertyFlag(uint8_t(a) | uint8_t(b)); }
inline bool HasFlag(EPropertyFlag set, EPropertyFlag bit)
    { return (uint8_t(set) & uint8_t(bit)) != 0; }

struct SVec3i { int32_t x = 0, y = 0, z = 0;
    bool operator==(const SVec3i& o) const { return x==o.x && y==o.y && z==o.z; } };
struct SVec3f { float   x = 0, y = 0, z = 0;
    bool operator==(const SVec3f& o) const { return x==o.x && y==o.y && z==o.z; } };
struct SColor4f { float r = 1, g = 1, b = 1, a = 1;
    bool operator==(const SColor4f& o) const { return r==o.r && g==o.g && b==o.b && a==o.a; } };

// One enum option in an Enum-typed property's choice set.
struct SEnumOption
{
    int32_t     value = 0;
    const char* label = "";
};

// Variant holding any property value. Used for read/write through
// IReflection::Get/Set so editors don't need per-type code paths.
using SPropertyValue = std::variant<
    bool,
    int64_t,
    double,
    std::string,
    SVec3i,
    SVec3f,
    SColor4f,
    IObject*>;

// ---------------------------------------------------------------------------
// Property descriptor (one entry per reflected member)
// ---------------------------------------------------------------------------

struct SPropertyDesc
{
    const char*    name        = "";       // identifier used by code (e.g. "level")
    const char*    label       = "";       // human-friendly UI label
    EPropertyType  type        = EPropertyType::Int;
    EPropertyFlag  flags       = EPropertyFlag::None;

    // Read/write the value through opaque member access. The first param is
    // the IObject; the second is the value variant.
    using GetFn = void (*)(const IObject* obj, SPropertyValue& out);
    using SetFn = void (*)(IObject* obj, const SPropertyValue& in);
    GetFn          get         = nullptr;
    SetFn          set         = nullptr;

    // For Enum properties: contiguous options list. For ObjectRef: the
    // expected target class id (0 = any). Otherwise unused.
    const SEnumOption* enum_options = nullptr;
    int32_t            enum_count   = 0;

    // Inclusive numeric range hints for the inspector (drag / slider widgets).
    // Both zero = "no hint, use default range".
    double         min_hint    = 0.0;
    double         max_hint    = 0.0;
};

// ---------------------------------------------------------------------------
// Reflection schema (class-static, one per type)
// ---------------------------------------------------------------------------

class IReflection
{
public:
    virtual ~IReflection() = default;

    // Schema -- per-class.
    virtual const char*           GetClassName() const = 0;
    virtual int32_t               GetClassId()   const = 0;   // 0 = unregistered
    virtual int32_t               NumProperties() const = 0;
    virtual const SPropertyDesc&  GetProperty(int32_t i) const = 0;
    virtual const SPropertyDesc*  FindProperty(const char* name) const = 0;

    // Per-instance accessors. Implementations forward to the matching
    // SPropertyDesc::get / set.
    virtual void GetValue(const IObject* obj, const SPropertyDesc& p, SPropertyValue& out) const = 0;
    virtual void SetValue(IObject* obj, const SPropertyDesc& p, const SPropertyValue& in) const = 0;

    // Object hierarchy -- the scene tree's view of this object.
    virtual IObject* GetParent     (const IObject* obj) const { (void)obj; return nullptr; }
    virtual int32_t  GetNumChildren(const IObject* obj) const { (void)obj; return 0; }
    virtual IObject* GetChildAt    (const IObject* obj, int32_t i) const { (void)obj; (void)i; return nullptr; }

    // Editor metadata. IsDeletable=false hides the Delete command for
    // this object (sectors, players, etc.). CanReparent=false keeps it
    // pinned in its current tree slot.
    virtual bool     IsDeletable  (const IObject* /*obj*/) const { return true;  }
    virtual bool     CanReparent  (const IObject* /*obj*/) const { return false; }
};

// ---------------------------------------------------------------------------
// IObject -- the editable interface
// ---------------------------------------------------------------------------

class IObject
{
public:
    virtual ~IObject() = default;

    virtual int32_t            GetId()   const = 0;
    virtual const char*        GetName() const = 0;
    virtual const IReflection& GetRefl() const = 0;
};

// ---------------------------------------------------------------------------
// Default reflection implementation
// ---------------------------------------------------------------------------

// Concrete IReflection backed by a property table. Most classes hand
// REFL_BEGIN/REFL_END a static instance of this; FindProperty does a
// case-insensitive linear scan (property tables are small).
class TReflectionTable : public IReflection
{
public:
    TReflectionTable(const char* class_name, int32_t class_id,
                     std::vector<SPropertyDesc> props);

    const char*          GetClassName()   const override { return name_; }
    int32_t              GetClassId()     const override { return id_; }
    int32_t              NumProperties()  const override { return int32_t(props_.size()); }
    const SPropertyDesc& GetProperty(int32_t i)        const override { return props_[i]; }
    const SPropertyDesc* FindProperty(const char* name) const override;

    void GetValue(const IObject* obj, const SPropertyDesc& p, SPropertyValue& out) const override
        { if (p.get) p.get(obj, out); }
    void SetValue(IObject* obj, const SPropertyDesc& p, const SPropertyValue& in) const override
        { if (p.set) p.set(obj, in); }

private:
    const char*                name_;
    int32_t                    id_;
    std::vector<SPropertyDesc> props_;
};

// ---------------------------------------------------------------------------
// Macros for declaring class reflection tables. Place inside an anonymous
// namespace in the class's .cpp; the static initialiser builds the table
// once at program start.
//
//   namespace {
//       REFL_BEGIN(TSector, /*class_id=*/CLASS_SECTOR)
//           REFL_PROP_INT_RO("level",   "Level",     TSector, level)
//           REFL_PROP_INT   ("sector_x","Sector X",  TSector, sector_x)
//           REFL_PROP_INT   ("sector_y","Sector Y",  TSector, sector_y)
//       REFL_END(TSector)
//   }
//
// Then on the class: const IReflection& TSector::StaticRefl() { return s_refl; }
// ---------------------------------------------------------------------------

#define REFL_BEGIN(ClassT, ClassId)                                     \
    static const std::vector<SPropertyDesc> _refl_props_##ClassT = {    \
        /* expansion of REFL_PROP_xxx entries here */
#define REFL_END(ClassT)                                                \
    };                                                                  \
    static const TReflectionTable s_refl_##ClassT(                      \
        #ClassT, /*ClassId*/ 0, _refl_props_##ClassT);

// One-property helpers. They build SPropertyDesc entries by capturing
// ClassT-typed member-pointers and wrapping them in the right typed
// get/set lambdas.
//
// For brevity these are header-implemented inline; if a class needs
// custom getters/setters (computed properties), declare the SPropertyDesc
// entry by hand instead.

#define REFL_PROP_INT(field_name, label_str, ClassT, member_name)       \
    SPropertyDesc{                                                      \
        field_name, label_str, EPropertyType::Int, EPropertyFlag::None, \
        +[](const IObject* o, SPropertyValue& v)                        \
            { v = int64_t(static_cast<const ClassT*>(o)->member_name); }, \
        +[](IObject* o, const SPropertyValue& v)                        \
            { if (auto p = std::get_if<int64_t>(&v))                    \
                static_cast<ClassT*>(o)->member_name = int32_t(*p); },  \
        nullptr, 0, 0.0, 0.0,                                            \
    },

#define REFL_PROP_INT_RO(field_name, label_str, ClassT, member_name)    \
    SPropertyDesc{                                                      \
        field_name, label_str, EPropertyType::Int, EPropertyFlag::ReadOnly, \
        +[](const IObject* o, SPropertyValue& v)                        \
            { v = int64_t(static_cast<const ClassT*>(o)->member_name); }, \
        nullptr, nullptr, 0, 0.0, 0.0,                                   \
    },

#define REFL_PROP_FLOAT(field_name, label_str, ClassT, member_name)     \
    SPropertyDesc{                                                      \
        field_name, label_str, EPropertyType::Float, EPropertyFlag::None, \
        +[](const IObject* o, SPropertyValue& v)                        \
            { v = double(static_cast<const ClassT*>(o)->member_name); }, \
        +[](IObject* o, const SPropertyValue& v)                        \
            { if (auto p = std::get_if<double>(&v))                     \
                static_cast<ClassT*>(o)->member_name = float(*p); },    \
        nullptr, 0, 0.0, 0.0,                                            \
    },

#define REFL_PROP_BOOL(field_name, label_str, ClassT, member_name)      \
    SPropertyDesc{                                                      \
        field_name, label_str, EPropertyType::Bool, EPropertyFlag::None, \
        +[](const IObject* o, SPropertyValue& v)                        \
            { v = static_cast<const ClassT*>(o)->member_name; },        \
        +[](IObject* o, const SPropertyValue& v)                        \
            { if (auto p = std::get_if<bool>(&v))                       \
                static_cast<ClassT*>(o)->member_name = *p; },           \
        nullptr, 0, 0.0, 0.0,                                            \
    },

#define REFL_PROP_STRING(field_name, label_str, ClassT, member_name)    \
    SPropertyDesc{                                                      \
        field_name, label_str, EPropertyType::String, EPropertyFlag::None, \
        +[](const IObject* o, SPropertyValue& v)                        \
            { v = std::string(static_cast<const ClassT*>(o)->member_name); }, \
        +[](IObject* o, const SPropertyValue& v)                        \
            { if (auto p = std::get_if<std::string>(&v))                \
                static_cast<ClassT*>(o)->member_name = *p; },           \
        nullptr, 0, 0.0, 0.0,                                            \
    },
