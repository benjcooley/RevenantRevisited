// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *           editoradapters.cpp - IObject adapters for game classes      *
// *************************************************************************

#include "editoradapters.h"

#include "object.h"
#include "sector.h"

#include <cstdio>

// ---------------------------------------------------------------------------
// Reflection tables
// ---------------------------------------------------------------------------
//
// We hand-roll get/set lambdas here because TSector / TObjectInstance
// expose accessor methods (SectorLevel, Pos, GetState, ...) rather than
// raw fields, which the generic REFL_PROP_* macros assume. The result is
// the same shape as what the macros would produce; just typed by hand.
//
// Each get lambda casts `IObject*` to the adapter type, dereferences the
// underlying game object, and stuffs the value into SPropertyValue.

namespace {

// ---- TSector schema ----------------------------------------------------

const std::vector<SPropertyDesc> kSectorProps = {
    SPropertyDesc{
        "level", "Level",
        EPropertyType::Int, EPropertyFlag::ReadOnly,
        +[](const IObject* o, SPropertyValue& v) {
            const auto* a = static_cast<const TSectorAdapter*>(o);
            v = int64_t(a->Sector() ? a->Sector()->SectorLevel() : 0);
        },
        nullptr, nullptr, 0, 0.0, 0.0,
    },
    SPropertyDesc{
        "sector_x", "Sector X",
        EPropertyType::Int, EPropertyFlag::ReadOnly,
        +[](const IObject* o, SPropertyValue& v) {
            const auto* a = static_cast<const TSectorAdapter*>(o);
            v = int64_t(a->Sector() ? a->Sector()->SectorX() : 0);
        },
        nullptr, nullptr, 0, 0.0, 0.0,
    },
    SPropertyDesc{
        "sector_y", "Sector Y",
        EPropertyType::Int, EPropertyFlag::ReadOnly,
        +[](const IObject* o, SPropertyValue& v) {
            const auto* a = static_cast<const TSectorAdapter*>(o);
            v = int64_t(a->Sector() ? a->Sector()->SectorY() : 0);
        },
        nullptr, nullptr, 0, 0.0, 0.0,
    },
    SPropertyDesc{
        "num_items", "Object Count",
        EPropertyType::Int, EPropertyFlag::ReadOnly,
        +[](const IObject* o, SPropertyValue& v) {
            const auto* a = static_cast<const TSectorAdapter*>(o);
            v = int64_t(a->Sector() ? a->Sector()->NumItems() : 0);
        },
        nullptr, nullptr, 0, 0.0, 0.0,
    },
};

class TSectorRefl : public TReflectionTable
{
public:
    TSectorRefl() : TReflectionTable("TSector", /*class_id=*/0, kSectorProps) {}
    bool IsDeletable(const IObject* /*obj*/) const override { return false; }
    int32_t GetNumChildren(const IObject* obj) const override {
        const auto* a = static_cast<const TSectorAdapter*>(obj);
        return a->Sector() ? a->Sector()->NumItems() : 0;
    }
    IObject* GetChildAt(const IObject* obj, int32_t i) const override {
        const auto* a = static_cast<const TSectorAdapter*>(obj);
        if (!a->Sector()) return nullptr;
        TObjectInstance* oi = a->Sector()->GetInstance(i);
        if (!oi) return nullptr;
        return EditorAdapters().For(oi);
    }
};

const TSectorRefl& SectorRefl()
{
    static TSectorRefl s;
    return s;
}

// ---- TObjectInstance schema --------------------------------------------

const std::vector<SPropertyDesc> kInstanceProps = {
    SPropertyDesc{
        "class", "Class",
        EPropertyType::String, EPropertyFlag::ReadOnly,
        +[](const IObject* o, SPropertyValue& v) {
            const auto* a = static_cast<const TInstanceAdapter*>(o);
            const char* s = a->Instance() ? a->Instance()->GetClassName() : "";
            v = std::string(s ? s : "");
        },
        nullptr, nullptr, 0, 0.0, 0.0,
    },
    SPropertyDesc{
        "type", "Type",
        EPropertyType::String, EPropertyFlag::ReadOnly,
        +[](const IObject* o, SPropertyValue& v) {
            const auto* a = static_cast<const TInstanceAdapter*>(o);
            const char* s = a->Instance() ? a->Instance()->GetTypeName() : "";
            v = std::string(s ? s : "");
        },
        nullptr, nullptr, 0, 0.0, 0.0,
    },
    SPropertyDesc{
        "pos", "Position",
        EPropertyType::Vec3Int, EPropertyFlag::None,
        +[](const IObject* o, SPropertyValue& v) {
            const auto* a = static_cast<const TInstanceAdapter*>(o);
            if (!a->Instance()) { v = SVec3i{}; return; }
            const S3DPoint& p = a->Instance()->Pos();
            v = SVec3i{ p.x, p.y, p.z };
        },
        +[](IObject* o, const SPropertyValue& v) {
            auto* a = static_cast<TInstanceAdapter*>(o);
            if (!a->Instance()) return;
            if (auto p = std::get_if<SVec3i>(&v))
                a->Instance()->SetPos(S3DPoint{ p->x, p->y, p->z });
        },
        nullptr, 0, 0.0, 0.0,
    },
    SPropertyDesc{
        "state", "State",
        EPropertyType::Int, EPropertyFlag::None,
        +[](const IObject* o, SPropertyValue& v) {
            const auto* a = static_cast<const TInstanceAdapter*>(o);
            v = int64_t(a->Instance() ? a->Instance()->GetState() : 0);
        },
        +[](IObject* o, const SPropertyValue& v) {
            auto* a = static_cast<TInstanceAdapter*>(o);
            if (!a->Instance()) return;
            if (auto p = std::get_if<int64_t>(&v))
                a->Instance()->SetState(int32_t(*p));
        },
        nullptr, 0, 0.0, 0.0,
    },
    SPropertyDesc{
        "frame", "Frame",
        EPropertyType::Int, EPropertyFlag::ReadOnly,
        +[](const IObject* o, SPropertyValue& v) {
            const auto* a = static_cast<const TInstanceAdapter*>(o);
            v = int64_t(a->Instance() ? a->Instance()->GetFrame() : 0);
        },
        nullptr, nullptr, 0, 0.0, 0.0,
    },
    SPropertyDesc{
        "mapindex", "Map Index",
        EPropertyType::Int, EPropertyFlag::ReadOnly,
        +[](const IObject* o, SPropertyValue& v) {
            const auto* a = static_cast<const TInstanceAdapter*>(o);
            v = int64_t(a->Instance() ? a->Instance()->GetMapIndex() : -1);
        },
        nullptr, nullptr, 0, 0.0, 0.0,
    },
};

const TReflectionTable& InstanceRefl()
{
    static const TReflectionTable s("TObjectInstance", /*class_id=*/0, kInstanceProps);
    return s;
}

} // namespace

// ---------------------------------------------------------------------------
// Adapter accessors
// ---------------------------------------------------------------------------

int32_t TSectorAdapter::GetId() const
{
    if (!sec_) return -1;
    // Pack level/sx/sy into a stable id (fits 32 bits well within usual map sizes).
    return (sec_->SectorLevel() << 24) | (sec_->SectorX() << 12) | sec_->SectorY();
}

const char* TSectorAdapter::GetName() const
{
    if (!sec_) return "(invalid sector)";
    std::snprintf(name_buf_, sizeof(name_buf_), "Sector %d_%d_%d",
                  sec_->SectorLevel(), sec_->SectorX(), sec_->SectorY());
    return name_buf_;
}

const IReflection& TSectorAdapter::GetRefl() const { return SectorRefl(); }

int32_t TInstanceAdapter::GetId() const
{
    return oi_ ? oi_->GetMapIndex() : -1;
}

const char* TInstanceAdapter::GetName() const
{
    if (!oi_) return "(invalid instance)";
    if (const char* tn = oi_->GetTypeName(); tn && *tn) return tn;
    if (const char* cn = oi_->GetClassName(); cn && *cn) return cn;
    return "(unnamed)";
}

const IReflection& TInstanceAdapter::GetRefl() const { return InstanceRefl(); }

// ---------------------------------------------------------------------------
// Adapter cache
// ---------------------------------------------------------------------------

TSectorAdapter* TEditorAdapterCache::For(TSector* sec)
{
    if (!sec) return nullptr;
    auto it = sectors_.find(sec);
    if (it != sectors_.end()) return it->second.get();
    auto adapter = std::make_unique<TSectorAdapter>(sec);
    TSectorAdapter* raw = adapter.get();
    sectors_.emplace(sec, std::move(adapter));
    return raw;
}

TInstanceAdapter* TEditorAdapterCache::For(TObjectInstance* oi)
{
    if (!oi) return nullptr;
    auto it = instances_.find(oi);
    if (it != instances_.end()) return it->second.get();
    auto adapter = std::make_unique<TInstanceAdapter>(oi);
    TInstanceAdapter* raw = adapter.get();
    instances_.emplace(oi, std::move(adapter));
    return raw;
}

void TEditorAdapterCache::Clear()
{
    sectors_.clear();
    instances_.clear();
}

TEditorAdapterCache& EditorAdapters()
{
    static TEditorAdapterCache s;
    return s;
}
