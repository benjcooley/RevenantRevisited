// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *          refl.cpp - registry, lookups, lanes, packed codec            *
// *************************************************************************

#include "engine/refl/refl.h"
#include "engine/refl/packed.h"

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

const SType* FindByDefName(std::string_view def_name)
{
    for (SType* t : Types())
        if (t->def_name && *t->def_name && IEquals(def_name, t->def_name))
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

bool Equals(const SPropValue& a, const SPropValue& b)
{
    if (a.type != b.type) return false;
    switch (a.type)
    {
        case EPropType::Struct:
        case EPropType::List:    return false;
        case EPropType::Bool:
        case EPropType::Trigger: return a.b == b.b;
        case EPropType::Int:
        case EPropType::Enum:    return a.i == b.i;
        case EPropType::Uid:
        case EPropType::Ref:     return a.u == b.u;
        case EPropType::Float:   return a.f == b.f;
        case EPropType::String:  return a.str == b.str;
        case EPropType::Vec3:    return a.v3[0] == b.v3[0] && a.v3[1] == b.v3[1] && a.v3[2] == b.v3[2];
        case EPropType::Color:   return a.c4[0] == b.c4[0] && a.c4[1] == b.c4[1] && a.c4[2] == b.c4[2] && a.c4[3] == b.c4[3];
    }
    return false;
}

void Reset(const SType& type, void* obj)
{
    for (const SType* t = &type; t; t = t->base)
    {
        for (int32_t i = 0; i < t->prop_count; ++i)
        {
            const SProp& p = t->props[i];
            if (p.read_only) continue;
            if (p.type == EPropType::Trigger) continue;
            if (p.type == EPropType::List) { if (t->list_clear) t->list_clear(obj, p.id); continue; }
            if (p.type == EPropType::Struct)
            {
                if (const SType* et = ElemType(p))
                    if (void* n = t->nested ? t->nested(obj, p.id) : nullptr)
                        Reset(*et, n);
                continue;
            }
            if (t->set) t->set(obj, p.id, p.def);
        }
        if (t->base == nullptr && t->base_name && *t->base_name)
            LinkBase(const_cast<SType&>(*t));
    }
}

void PostLoad(const SType& type, void* obj)
{
    std::vector<const SType*> chain;
    for (const SType* t = &type; t; t = t->base)
    {
        chain.push_back(t);
        if (t->base == nullptr && t->base_name && *t->base_name)
            LinkBase(const_cast<SType&>(*t));
    }
    for (auto it = chain.rbegin(); it != chain.rend(); ++it)
        if ((*it)->post_load)
            (*it)->post_load(obj);
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

const SType* ElemType(const SProp& prop)
{
    if (!prop.elem_type_name || !*prop.elem_type_name) return nullptr;
    return Find(prop.elem_type_name);
}

namespace {
// Finds the type in the chain whose own table holds `id`.
const SType* OwnerOf(const SType& type, uint16_t id)
{
    for (const SType* t = &type; t; t = t->base)
    {
        for (int32_t i = 0; i < t->prop_count; ++i)
            if (t->props[i].id == id)
                return t;
        if (t->base == nullptr && t->base_name && *t->base_name)
            LinkBase(const_cast<SType&>(*t));
    }
    return nullptr;
}
} // namespace

void* Nested(const SType& type, void* obj, uint16_t id)
{
    const SType* t = OwnerOf(type, id);
    return (t && t->nested) ? t->nested(obj, id) : nullptr;
}
int32_t ListCount(const SType& type, const void* obj, uint16_t id)
{
    const SType* t = OwnerOf(type, id);
    return (t && t->list_count) ? t->list_count(obj, id) : 0;
}
void* ListAt(const SType& type, void* obj, uint16_t id, int32_t index)
{
    const SType* t = OwnerOf(type, id);
    return (t && t->list_at) ? t->list_at(obj, id, index) : nullptr;
}
void* ListAdd(const SType& type, void* obj, uint16_t id)
{
    const SType* t = OwnerOf(type, id);
    return (t && t->list_add) ? t->list_add(obj, id) : nullptr;
}
void ListClear(const SType& type, void* obj, uint16_t id)
{
    const SType* t = OwnerOf(type, id);
    if (t && t->list_clear) t->list_clear(obj, id);
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
            if (!p.serialized || p.read_only)
                continue;
            if (p.type == EPropType::List)
            {
                const SType* et = ElemType(p);
                if (!et || !t->list_count || !t->list_at) continue;
                const int32_t n = t->list_count(obj, p.id);
                for (int32_t k = 0; k < n; ++k)
                {
                    TPackedWriter sub;
                    Pack(*et, t->list_at(const_cast<void*>(obj), p.id, k), sub);
                    w.Message(p.id, sub);
                }
                continue;
            }
            if (p.type == EPropType::Struct)
            {
                const SType* et = ElemType(p);
                if (!et || !t->nested) continue;
                TPackedWriter sub;
                Pack(*et, t->nested(const_cast<void*>(obj), p.id), sub);
                w.Message(p.id, sub);
                continue;
            }
            SPropValue v;
            if (!t->get || !t->get(obj, p.id, v))
                continue;
            switch (p.type)
            {
                case EPropType::Bool:    w.Bool(p.id, v.b);      break;
                case EPropType::Int:     w.Int(p.id, v.i);       break;
                case EPropType::Enum:    w.Enum(p.id, v.i);      break;
                case EPropType::Uid:
                case EPropType::Ref:     w.Uid(p.id, v.u);       break;
                case EPropType::Float:   w.Float(p.id, v.f);     break;
                case EPropType::String:  w.String(p.id, v.str);  break;
                case EPropType::Vec3:    w.Vec3(p.id, v.v3);     break;
                case EPropType::Color:   w.Color(p.id, v.c4);    break;
                case EPropType::Trigger:
                case EPropType::Struct:
                case EPropType::List:    break;
            }
        }
        if (t->base == nullptr && t->base_name && *t->base_name)
            LinkBase(const_cast<SType&>(*t));
    }
}

void ApplyPacked(const SType& type, void* obj, const SPackedField& f)
{
    const SProp* p = FindProp(type, f.id);
    if (!p)
        return;     // retired or unknown id: already skipped by wire kind
    if (p->type == EPropType::List || p->type == EPropType::Struct)
    {
        const SType* et = ElemType(*p);
        if (!et || f.wire != EPackedWire::Bytes) return;
        void* elem = p->type == EPropType::List ? ListAdd(type, obj, p->id) : Nested(type, obj, p->id);
        if (!elem) return;
        TPackedReader sub(f);
        Unpack(*et, elem, sub);
        PostLoad(*et, elem);
        return;
    }
    SPropValue v;
    switch (p->type)
    {
        case EPropType::Bool:    v = SPropValue::Bool(TPackedReader::Bool(f));     break;
        case EPropType::Int:     v = SPropValue::Int(TPackedReader::Int(f));       break;
        case EPropType::Enum:    v = SPropValue::Enum(TPackedReader::Int(f));      break;
        case EPropType::Uid:     v = SPropValue::Uid(TPackedReader::Uid(f));       break;
        case EPropType::Ref:     v = SPropValue::Ref(TPackedReader::Uid(f));       break;
        case EPropType::Float:   v = SPropValue::Float(TPackedReader::Float(f));   break;
        case EPropType::String:  v = SPropValue::String(TPackedReader::String(f)); break;
        case EPropType::Vec3:    v.type = EPropType::Vec3;  if (!TPackedReader::Vec3(f, v.v3))  return; break;
        case EPropType::Color:   v.type = EPropType::Color; if (!TPackedReader::Color(f, v.c4)) return; break;
        case EPropType::Trigger:
        case EPropType::Struct:
        case EPropType::List:    return;
    }
    Set(type, obj, p->id, v);
}

void Unpack(const SType& type, void* obj, TPackedReader& r)
{
    SPackedField f;
    while (r.Next(f))
        ApplyPacked(type, obj, f);
}

} // namespace refl
