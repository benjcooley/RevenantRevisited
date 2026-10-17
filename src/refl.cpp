// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *          refl.cpp - registry, lookups, lanes, packed codec            *
// *************************************************************************

#include "refl.h"
#include "packed.h"

#include <vector>

namespace refl {

namespace {

bool IEquals(std::string_view a, const char* b)
{
    if (!b) return false;
    const size_t n = std::strlen(b);
    if (a.size() != n) return false;
    for (size_t i = 0; i < n; ++i)
        if (std::tolower(static_cast<unsigned char>(a[i])) != std::tolower(static_cast<unsigned char>(b[i])))
            return false;
    return true;
}

// Process-lifetime table (see the TSafeObjectBase registry note: never a
// destructed static, generated registrars may run late).
std::vector<SType*>& Types()
{
    static auto* v = new std::vector<SType*>();
    return *v;
}

void LinkBase(SType& t)
{
    if (t.base || !t.base_name || !*t.base_name)
        return;
    for (SType* other : Types())
        if (other != &t && IEquals(t.base_name, other->name))
        {
            t.base = other;
            return;
        }
}

} // namespace

void Register(SType& type)
{
    Types().push_back(&type);
}

const SType* Find(const std::type_info& info)
{
    for (SType* t : Types())
        if (t->info && *t->info == info)
        {
            LinkBase(*t);
            return t;
        }
    return nullptr;
}

const SType* Find(std::string_view class_name)
{
    for (SType* t : Types())
        if (IEquals(class_name, t->name))
        {
            LinkBase(*t);
            return t;
        }
    return nullptr;
}

const SProp* FindProp(const SType& type, std::string_view name)
{
    for (const SType* t = &type; t; t = t->base)
    {
        for (int32_t i = 0; i < t->prop_count; ++i)
            if (IEquals(name, t->props[i].name))
                return &t->props[i];
        if (t->base == nullptr && t->base_name && *t->base_name)
            LinkBase(const_cast<SType&>(*t));
    }
    return nullptr;
}

const SProp* FindProp(const SType& type, uint16_t id)
{
    for (const SType* t = &type; t; t = t->base)
    {
        for (int32_t i = 0; i < t->prop_count; ++i)
            if (t->props[i].id == id)
                return &t->props[i];
        if (t->base == nullptr && t->base_name && *t->base_name)
            LinkBase(const_cast<SType&>(*t));
    }
    return nullptr;
}

const SEnumOption* FindEnumOption(const SProp& prop, std::string_view name)
{
    for (int32_t i = 0; i < prop.option_count; ++i)
        if (IEquals(name, prop.options[i].name))
            return &prop.options[i];
    return nullptr;
}

const char* EnumName(const SProp& prop, int32_t value)
{
    for (int32_t i = 0; i < prop.option_count; ++i)
        if (prop.options[i].value == value)
            return prop.options[i].name;
    return "";
}

bool Set(const SType& type, void* obj, uint16_t id, const SPropValue& value)
{
    for (const SType* t = &type; t; t = t->base)
    {
        if (t->set && t->set(obj, id, value))
            return true;
        if (t->base == nullptr && t->base_name && *t->base_name)
            LinkBase(const_cast<SType&>(*t));
    }
    return false;
}

bool Get(const SType& type, const void* obj, uint16_t id, SPropValue& out)
{
    for (const SType* t = &type; t; t = t->base)
    {
        if (t->get && t->get(obj, id, out))
            return true;
        if (t->base == nullptr && t->base_name && *t->base_name)
            LinkBase(const_cast<SType&>(*t));
    }
    return false;
}

bool SetLane(const SType& type, void* obj, uint16_t id, int32_t lane, float x)
{
    const SProp* p = FindProp(type, id);
    if (!p || lane < 0 || lane >= PropLaneCount(p->type))
        return false;
    SPropValue v;
    if (!Get(type, obj, id, v))
        v.type = p->type;    // write-only (trigger): synthesize the declared type
    PropSetLane(v, lane, x);
    return Set(type, obj, id, v);
}

bool GetLane(const SType& type, const void* obj, uint16_t id, int32_t lane, float& out)
{
    const SProp* p = FindProp(type, id);
    if (!p || lane < 0 || lane >= PropLaneCount(p->type))
        return false;
    SPropValue v;
    if (!Get(type, obj, id, v))
        return false;
    out = PropGetLane(v, lane);
    return true;
}

void Pack(const SType& type, const void* obj, TPackedWriter& w)
{
    for (const SType* t = &type; t; t = t->base)
    {
        for (int32_t i = 0; i < t->prop_count; ++i)
        {
            const SProp& p = t->props[i];
            if (p.flags & (PropFlag_NoSave | PropFlag_ReadOnly))
                continue;
            SPropValue v;
            if (!t->get || !t->get(obj, p.id, v))
                continue;
            switch (p.type)
            {
                case EPropType::Bool:    w.Bool(p.id, v.b);      break;
                case EPropType::Int:     w.Int(p.id, v.i);       break;
                case EPropType::Enum:    w.Enum(p.id, v.i);      break;
                case EPropType::Ref:     w.Int(p.id, v.i);       break;
                case EPropType::Float:   w.Float(p.id, v.f);     break;
                case EPropType::String:  w.String(p.id, v.str);  break;
                case EPropType::Vec3:    w.Vec3(p.id, v.v3);     break;
                case EPropType::Color:   w.Color(p.id, v.c4);    break;
                case EPropType::Trigger: break;                  // never persisted
            }
        }
        if (t->base == nullptr && t->base_name && *t->base_name)
            LinkBase(const_cast<SType&>(*t));
    }
}

void Unpack(const SType& type, void* obj, TPackedReader& r)
{
    SPackedField f;
    while (r.Next(f))
    {
        const SProp* p = FindProp(type, f.id);
        if (!p)
            continue;   // retired or unknown id: already skipped by wire kind
        SPropValue v;
        switch (p->type)
        {
            case EPropType::Bool:    v = SPropValue::Bool(TPackedReader::Bool(f));     break;
            case EPropType::Int:     v = SPropValue::Int(TPackedReader::Int(f));       break;
            case EPropType::Enum:    v = SPropValue::Enum(TPackedReader::Int(f));      break;
            case EPropType::Ref:     v = SPropValue::Ref(TPackedReader::Int(f));       break;
            case EPropType::Float:   v = SPropValue::Float(TPackedReader::Float(f));   break;
            case EPropType::String:  v = SPropValue::String(TPackedReader::String(f)); break;
            case EPropType::Vec3:    v.type = EPropType::Vec3;  if (!TPackedReader::Vec3(f, v.v3))  continue; break;
            case EPropType::Color:   v.type = EPropType::Color; if (!TPackedReader::Color(f, v.c4)) continue; break;
            case EPropType::Trigger: continue;
        }
        Set(type, obj, p->id, v);
    }
}

} // namespace refl
