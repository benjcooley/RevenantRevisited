// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *        propdef.cpp - DEF text <-> annotated properties               *
// *************************************************************************

#include "propdef.h"

namespace propdef {

namespace {

bool ArrayToFloats(const defdoc::Node& n, float* out, int32_t count)
{
    if (!n.is_array() || int32_t(n.as_array().size()) != count)
        return false;
    for (int32_t i = 0; i < count; ++i)
    {
        const defdoc::Node& e = n.as_array()[size_t(i)];
        if (!e.is_number() && !e.is_bool() && !e.is_string())
            return false;
        out[i] = float(e.as_double());
    }
    return true;
}

bool NodeToValue(const refl::SProp& p, const defdoc::Node& n, SPropValue& v, std::string& why)
{
    switch (p.type)
    {
        case EPropType::Bool:
            if (!n.is_bool() && !n.is_int() && !n.is_string()) { why = "expected bool"; return false; }
            v = SPropValue::Bool(n.as_bool());
            return true;
        case EPropType::Int:
        case EPropType::Ref:
            if (!n.is_number() && !n.is_string()) { why = "expected int"; return false; }
            v = p.type == EPropType::Int ? SPropValue::Int(int32_t(n.as_int())) : SPropValue::Ref(int32_t(n.as_int()));
            return true;
        case EPropType::Float:
            if (!n.is_number() && !n.is_string()) { why = "expected number"; return false; }
            v = SPropValue::Float(float(n.as_double()));
            return true;
        case EPropType::String:
            if (!n.is_string() && !n.is_number() && !n.is_bool()) { why = "expected string"; return false; }
            v = SPropValue::String(n.as_string());   // view into the node; SetProp copies
            return true;
        case EPropType::Enum:
        {
            if (n.is_int()) { v = SPropValue::Enum(int32_t(n.as_int())); return true; }
            const std::string s = n.as_string();
            if (const refl::SEnumOption* opt = refl::FindEnumOption(p, s))
            {
                v = SPropValue::Enum(opt->value);
                return true;
            }
            why = "unknown option '" + s + "' (expected one of:";
            for (int32_t i = 0; i < p.option_count; ++i)
                why += std::string(" ") + p.options[i].name;
            why += ")";
            return false;
        }
        case EPropType::Vec3:
            v.type = EPropType::Vec3;
            if (!ArrayToFloats(n, v.v3, 3)) { why = "expected [x, y, z]"; return false; }
            return true;
        case EPropType::Color:
            v.type = EPropType::Color;
            if (ArrayToFloats(n, v.c4, 4)) return true;
            if (ArrayToFloats(n, v.c4, 3)) { v.c4[3] = 1.0f; return true; }
            why = "expected [r, g, b] or [r, g, b, a]";
            return false;
        case EPropType::Trigger:
            why = "trigger properties cannot be set from a DEF";
            return false;
    }
    return false;
}

} // namespace

bool Apply(const refl::SType& type, void* obj, const defdoc::Node& block, std::string* error)
{
    bool ok = true;
    auto fail = [&](const std::string& msg) {
        if (ok && error) *error = msg;
        ok = false;
    };
    if (!block.is_object())
    {
        fail("expected a block");
        return false;
    }
    for (const auto& kv : block.as_object())
    {
        const std::string& key = kv.first;
        const defdoc::Node& n = kv.second;
        if (key.empty() || key[0] == '$')
            continue;                                   // $name and other parser meta
        if (n.is_object())
            continue;                                   // sub-block: structural, caller's job
        if (n.is_array() && !n.as_array().empty() && n.as_array().front().is_object())
            continue;                                   // list of sub-blocks
        const refl::SProp* p = refl::FindProp(type, key);
        if (!p)
        {
            fail("unknown property '" + key + "'");
            continue;
        }
        if (p->flags & PropFlag_ReadOnly)
        {
            fail("property '" + key + "' is read-only");
            continue;
        }
        SPropValue v;
        std::string why;
        // String views into defdoc nodes: the node outlives the call and
        // SetProp copies, so the view is safe. as_string() on a non-string
        // node builds a temporary, hence the local copy below.
        std::string tmp;
        if (p->type == EPropType::String)
        {
            tmp = n.as_string();
            v = SPropValue::String(tmp);
        }
        else if (!NodeToValue(*p, n, v, why))
        {
            fail("property '" + key + "': " + why);
            continue;
        }
        if (!refl::Set(type, obj, p->id, v))
            fail("property '" + key + "': rejected by the setter");
    }
    return ok;
}

void Write(const refl::SType& type, const void* obj, defdoc::Node& block)
{
    for (const refl::SType* t = &type; t; t = t->base)
    for (int32_t i = 0; i < t->prop_count; ++i)
    {
        const refl::SProp& p = t->props[i];
        if (p.flags & (PropFlag_NoSave | PropFlag_ReadOnly))
            continue;
        SPropValue v;
        if (!refl::Get(type, obj, p.id, v))
            continue;
        defdoc::Node out;
        switch (p.type)
        {
            case EPropType::Bool:    out = defdoc::Node(v.b); break;
            case EPropType::Int:
            case EPropType::Ref:     out = defdoc::Node(int64_t(v.i)); break;
            case EPropType::Float:   out = defdoc::Node(double(v.f)); break;
            case EPropType::String:  out = defdoc::Node(std::string(v.str)); break;
            case EPropType::Enum:    out = defdoc::Node(std::string(refl::EnumName(p, v.i))); break;
            case EPropType::Vec3:    out = defdoc::Node(defdoc::Node::Array{ defdoc::Node(double(v.v3[0])), defdoc::Node(double(v.v3[1])), defdoc::Node(double(v.v3[2])) }); break;
            case EPropType::Color:   out = defdoc::Node(defdoc::Node::Array{ defdoc::Node(double(v.c4[0])), defdoc::Node(double(v.c4[1])), defdoc::Node(double(v.c4[2])), defdoc::Node(double(v.c4[3])) }); break;
            case EPropType::Trigger: continue;
        }
        block[p.name] = std::move(out);
    }
}

} // namespace propdef
