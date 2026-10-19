// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   objectdef.h - instance state and the archetype                      *
// *************************************************************************
//
// Two things with "def" in their name, kept apart on purpose
// (docs/OBJECT_MODEL_PLAN.md phase 4):
//
//   SObjectState  instance state: the record of one object. What a spawn
//                 site fills, what a file stores, what an instance keeps as
//                 the state it was loaded or spawned in (Reset puts it
//                 back). Plain data, reflected, derived per class with the
//                 fields that class's stream carries. Retail called it
//                 SObjectDef and derived the instance from it.
//
//   TObjectDef    class information: the archetype, one per CLASS.DEF
//                 type, owned by the type's TObjectClass (its type table is
//                 the registry every instance points into through
//                 Archetype()). Carries the type-level data (name, imagery,
//                 type stats, the retail unique id) and the type's default
//                 state, and does the work in code: Create makes an
//                 instance of the right C++ class, Apply puts a record's
//                 values onto a live instance and its components, Capture
//                 takes them back out. A class with state of its own
//                 derives (TCharacterDef : TStateObjectDef<SCharacterState>)
//                 and overrides; most use the base.
//
// Which C++ class a type gets is decided by name, as the builders did: a
// def class registers under a type name ("ArrowWallS") or a class name
// ("Character"), and TObjectClass::AddType asks the registry for the type
// name first, then the class name.
//
// *************************************************************************

#pragma once

#include "engine/core/revtypes.h"
#include "engine/refl/refl.h"

#include <memory>

_CLASSDEF(TObjectInstance)
_CLASSDEF(TObjectImagery)

typedef TSizableArray<int32_t, 0, 1> TStatisticList;

// Instance state: the record of one object. Ids 1-99 are the base's; a
// derived record takes its own hundred (REFL_IDS) so the base can grow.
struct SObjectState
{
    REFL_IDS(1, 99)

    PROP(1,  "Object class")                                   short    objclass = -1;
    PROP(2,  "Type within the class")                          short    objtype  = -1;
    PROP(3,  "OF_* flags")                                     uint32_t flags    = 0;
    PROP(4,  "Animation state")                                uint16_t state    = 0;
    PROP(5,  "Map level")                                      uint16_t level    = 0;
    PROP(6,  "Position, the object's exact centre")            S3DPoint pos = {0, 0, 0};
    PROP(7,  "Movement velocity, 1/256 wu per tick")           S3DPoint vel = {0, 0, 0};
    PROP(8,  "Movement integrator carry")                      S3DPoint accum = {0, 0, 0};
    PROP(9,  "Rotation about x, 0-255")                        uint8_t  rotatex = 0;
    PROP(10, "Rotation about y, 0-255")                        uint8_t  rotatey = 0;
    union
    {
        PROP(11, "Rotation about z, the facing, 0-255")        uint8_t  rotatez = 0;
        uint8_t facing;
    };
    PROP(12, "Object group")                                   uint8_t  group = 0;
    PROP(13, "Persistent identity; 0 keeps the node's")       TUid     uid;
};

// The archetype.
class TObjectDef
{
  public:
    TObjectDef() = default;
    virtual ~TObjectDef();
    TObjectDef(const TObjectDef&) = delete;
    TObjectDef& operator=(const TObjectDef&) = delete;

  // ---- The type (was SObjectInfo; keep it small, there are thousands) --
    char*           name      = nullptr;    // Type name (strdup'd)
    int32_t         imageryid = 0;          // Registered imagery for the type
    TObjectImagery* imagery   = nullptr;    // That imagery once loaded
    TStatisticList  stats;                  // Class statistics for the type
    TStatisticList  objstats;               // Object statistics' defaults for the type
    uint32_t        uniqueid  = 0;          // Retail's unique type id (ObjId)

    [[nodiscard]] const char* DefName() const { return defname_; }
        // The name this def class was registered under (type or class name)
    [[nodiscard]] virtual const SObjectState& DefaultState() const = 0;
    [[nodiscard]] virtual SObjectState& DefaultState() = 0;
        // The state an object of the type starts in: each def class holds
        // its own class's state record (TStateObjectDef) and hands it up

  // ---- Factory -------------------------------------------------------
    virtual TObjectInstance* Create(SObjectState& state, TObjectImagery* img) const = 0;
        // A new instance of the type's C++ class from a record (the
        // constructor applies the record; `state` is clamped as retail did)
    virtual TObjectInstance* Create(TObjectImagery* img) const = 0;
        // A bare instance with no record (harnesses, stand-ins)

  // ---- Instance state <-> the live instance ---------------------------
    virtual void Apply(const SObjectState& state, TObjectInstance& inst) const;
        // The record's values onto the instance and its components; the
        // class-derived flags are the instance's to re-add
    virtual void Capture(const TObjectInstance& inst, SObjectState& state) const;
        // The instance's values into the record

  // ---- The def-class registry: name -> a new def of that class --------
    using FMake = std::unique_ptr<TObjectDef> (*)();
    static void Register(const char* name, FMake make);
    [[nodiscard]] static std::unique_ptr<TObjectDef> Make(const char* name);
        // nullptr when nothing is registered under `name`
    struct SRegistrar
    {
        SRegistrar(const char* name, FMake make) { Register(name, make); }
    };

  private:
    const char* defname_ = "";
};

// A def class whose type's default state is a `State` record (SObjectState
// or a class's derived record); the base reaches it upcast.
template <class State>
class TStateObjectDef : public TObjectDef
{
  public:
    State defaultState;
    [[nodiscard]] const SObjectState& DefaultState() const override { return defaultState; }
    [[nodiscard]] SObjectState& DefaultState() override { return defaultState; }
};

// A def class for an object class with no state of its own:
// `DEFINE_OBJECT_DEF("Character", TCharacter)` in the header,
// `REGISTER_OBJECT_DEF(TCharacter)` in one .cpp. A class with state, Apply
// and Capture of its own writes its def class by hand on
// TStateObjectDef<ItsState> and registers it the same way.
#define DEFINE_OBJECT_DEF(defname, obj)                                         \
class obj##Def final : public TStateObjectDef<SObjectState>                     \
{                                                                               \
  public:                                                                       \
    static constexpr const char* kDefName = defname;                            \
    TObjectInstance* Create(SObjectState& state, TObjectImagery* img) const override \
        { return new obj(&state, img); }                                        \
    TObjectInstance* Create(TObjectImagery* img) const override                 \
        { return new obj(img); }                                                \
};

#define REGISTER_OBJECT_DEF(obj)                                                \
static TObjectDef::SRegistrar obj##DefRegistrar(obj##Def::kDefName,            \
    []() -> std::unique_ptr<TObjectDef> { return std::make_unique<obj##Def>(); });
