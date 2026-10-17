// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *               defio.cpp - DEF reader / writer over reflection         *
// *************************************************************************

#include "defio.h"
#include "defdoc.h"
#include "nodeinst.h"

#include <cstdio>

namespace defio {

namespace {

// ---- leaf value conversion ------------------------------------------------

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

// `tmp` backs string views for the String case.
bool NodeToValue(const refl::SProp& p, const defdoc::Node& n, SPropValue& v,
                 std::string& tmp, std::string& why)
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
            tmp = n.as_string();
            v = SPropValue::String(tmp);
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

// ---- the reader sink --------------------------------------------------------

class TReaderSink : public defdoc::Sink
{
  public:
    TReaderSink(const refl::SType& root_type, void* root_obj, TNodeInst* root_node,
                std::vector<std::unique_ptr<TNodeInst>>* owned)
        : owned_(owned)
    {
        frames_.push_back({ &root_type, root_obj, root_node });
    }

    [[nodiscard]] bool Ok() const { return error_.empty(); }
    [[nodiscard]] const std::string& Error() const { return error_; }

    void Value(const std::string& key, defdoc::Node value, int line) override
    {
        SFrame& f = frames_.back();
        if (!f.type)
            return;                                  // inside an unknown block: skip
        if (!key.empty() && key[0] == '$')
            return;
        const refl::SProp* p = refl::FindProp(*f.type, key);
        if (!p)
        {
            Fail(line, "unknown property '" + key + "' in " + f.type->name);
            return;
        }
        if (p->read_only)
        {
            Fail(line, "property '" + key + "' is read-only");
            return;
        }
        SPropValue v;
        std::string tmp, why;
        if (!NodeToValue(*p, value, v, tmp, why))
        {
            Fail(line, "property '" + key + "': " + why);
            return;
        }
        if (!refl::Set(*f.type, f.obj, p->id, v))
            Fail(line, "property '" + key + "': rejected by the setter");
    }

    void Item(defdoc::Node, int line) override
    {
        Fail(line, "unexpected bare value");
    }

    void BeginBlock(const std::string& key, defdoc::Node header, int line) override
    {
        SFrame& cur = frames_.back();
        const refl::SType* type = cur.type ? refl::FindByDefName(key) : nullptr;
        if (!type)
        {
            if (cur.type)
                Fail(line, "unknown block '" + key + "'");
            frames_.push_back({});                   // skip frame
            return;
        }
        const std::string label = header["$name"].as_string();
        if (type->create_node)
        {
            if (!cur.node)
            {
                Fail(line, "block '" + key + "' needs a node to attach to");
                frames_.push_back({});
                return;
            }
            if (!owned_)
            {
                Fail(line, "block '" + key + "': child nodes are not allowed here");
                frames_.push_back({});
                return;
            }
            TNodeInst* child = type->create_node();
            child->SetNodeName(label);
            child->SetParent(cur.node);
            owned_->emplace_back(child);
            frames_.push_back({ type, child, child });
        }
        else if (type->create_component)
        {
            if (!cur.node)
            {
                Fail(line, "component '" + key + "' needs a node to attach to");
                frames_.push_back({});
                return;
            }
            std::unique_ptr<TObjectComponent> c(type->create_component());
            c->SetLabel(label);
            TObjectComponent* raw = c.get();
            cur.node->AddComponent(std::move(c));
            frames_.push_back({ type, raw, nullptr });
        }
        else
        {
            Fail(line, "block '" + key + "' has no factory");
            frames_.push_back({});
            return;
        }
        // Header key-values (`node x pos=[..]`) apply as properties.
        for (const auto& kv : header.as_object())
            if (!kv.first.empty() && kv.first[0] != '$')
                Value(kv.first, kv.second, line);
    }

    void EndBlock() override
    {
        if (frames_.size() > 1)
            frames_.pop_back();
    }

  private:
    struct SFrame
    {
        const refl::SType* type = nullptr;           // nullptr = skipping
        void*              obj  = nullptr;
        TNodeInst*         node = nullptr;           // when the object is a node
    };

    void Fail(int line, const std::string& msg)
    {
        if (!error_.empty())
            return;
        error_ = "line " + std::to_string(line) + ": " + msg;
    }

    std::vector<SFrame> frames_;
    std::vector<std::unique_ptr<TNodeInst>>* owned_ = nullptr;
    std::string error_;
};

bool Run(std::string_view text, TReaderSink& sink, std::string* error)
{
    try
    {
        defdoc::Document::parse_into(std::string(text), sink);
    }
    catch (const std::exception& e)
    {
        if (error) *error = e.what();
        return false;
    }
    if (!sink.Ok())
    {
        if (error) *error = sink.Error();
        return false;
    }
    return true;
}

// ---- writer ----------------------------------------------------------------

std::string Fmt(const char* fmt, double v)
{
    char buf[64];
    std::snprintf(buf, sizeof(buf), fmt, v);
    return buf;
}

void Indent(std::string& out, int32_t indent)
{
    for (int32_t i = 0; i < indent; ++i)
        out += "    ";
}

void WriteComponent(const TObjectComponent& c, std::string& out, int32_t indent)
{
    const refl::SType* t = refl::Of(c);
    if (!t || !*t->def_name)
        return;                                      // not a DEF_COMPONENT: nothing to write
    Indent(out, indent);
    out += t->def_name;
    if (!c.Label().empty())
    {
        out += " ";
        out += defdoc::detail::needs_quotes(c.Label()) ? defdoc::detail::quote(c.Label()) : c.Label();
    }
    out += "\n";
    Indent(out, indent); out += "{\n";
    WriteObject(*t, &c, out, indent + 1);
    Indent(out, indent); out += "}\n";
}

void WriteNode(const TNodeInst& node, std::string& out, int32_t indent);

// A node's body: its properties, then its components, then its children.
void WriteNodeBody(const TNodeInst& node, std::string& out, int32_t indent)
{
    if (const refl::SType* t = refl::Of(node))
        WriteObject(*t, &node, out, indent);
    for (int32_t i = 0; i < node.NumComponents(); ++i)
        if (const TObjectComponent* c = node.GetComponent(i))
            WriteComponent(*c, out, indent);
    for (int32_t i = 0; i < node.GetChildCount(); ++i)
        if (const TNodeInst* child = node.GetChildAt(i))
            WriteNode(*child, out, indent);
}

// A child node as a block: `node <name> { body }`.
void WriteNode(const TNodeInst& node, std::string& out, int32_t indent)
{
    const refl::SType* t = refl::Of(node);
    if (!t)
        return;
    const char* keyword = *t->def_name ? t->def_name : "node";
    const std::string name = node.GetName() ? node.GetName() : "";
    Indent(out, indent);
    out += keyword;
    if (!name.empty())
    {
        out += " ";
        out += defdoc::detail::needs_quotes(name) ? defdoc::detail::quote(name) : name;
    }
    out += "\n";
    Indent(out, indent); out += "{\n";
    WriteNodeBody(node, out, indent + 1);
    Indent(out, indent); out += "}\n";
}

} // namespace

bool Read(std::string_view text, TNodeInst& root,
          std::vector<std::unique_ptr<TNodeInst>>& owned, std::string* error)
{
    const refl::SType* t = refl::Of(root);
    if (!t)
    {
        if (error) *error = "root type is not annotated";
        return false;
    }
    TReaderSink sink(*t, &root, &root, &owned);
    return Run(text, sink, error);
}

bool ReadObject(std::string_view text, const refl::SType& type, void* obj, std::string* error)
{
    TReaderSink sink(type, obj, nullptr, nullptr);
    return Run(text, sink, error);
}

std::string FormatValue(const refl::SProp& p, const SPropValue& v)
{
    switch (p.type)
    {
        case EPropType::Bool:    return v.b ? "true" : "false";
        case EPropType::Int:
        case EPropType::Ref:     return std::to_string(v.i);
        case EPropType::Float:   return Fmt("%g", double(v.f));
        case EPropType::String:
        {
            const std::string s(v.str);
            return defdoc::detail::needs_quotes(s) ? defdoc::detail::quote(s) : s;
        }
        case EPropType::Enum:    return refl::EnumName(p, v.i);
        case EPropType::Vec3:
            return "[" + Fmt("%g", double(v.v3[0])) + ", " + Fmt("%g", double(v.v3[1])) + ", " + Fmt("%g", double(v.v3[2])) + "]";
        case EPropType::Color:
            return "[" + Fmt("%g", double(v.c4[0])) + ", " + Fmt("%g", double(v.c4[1])) + ", "
                       + Fmt("%g", double(v.c4[2])) + ", " + Fmt("%g", double(v.c4[3])) + "]";
        case EPropType::Trigger: return "";
    }
    return "";
}

void WriteObject(const refl::SType& type, const void* obj, std::string& out, int32_t indent)
{
    // Base properties first, then the class's own: reads top-down.
    std::vector<const refl::SType*> chain;
    for (const refl::SType* t = &type; t; t = t->base)
        chain.push_back(t);
    for (auto it = chain.rbegin(); it != chain.rend(); ++it)
    {
        const refl::SType& t = **it;
        for (int32_t i = 0; i < t.prop_count; ++i)
        {
            const refl::SProp& p = t.props[i];
            if (!p.serialized || p.read_only)
                continue;
            if (p.type == EPropType::Trigger)
                continue;
            SPropValue v;
            if (!refl::Get(type, obj, p.id, v))
                continue;
            if (refl::Equals(v, p.def))
                continue;                            // defaults are implicit
            Indent(out, indent);
            out += p.name;
            out += " = ";
            out += FormatValue(p, v);
            out += "\n";
        }
    }
}

std::string Write(const TNodeInst& root)
{
    // A document is the root's body: top-level statements are the root's
    // own properties, components and children, which is exactly what Read
    // applies to the root it is given. The root's name lives outside the
    // document (the prefab name, the file name).
    std::string out;
    WriteNodeBody(root, out, 0);
    return out;
}

} // namespace defio
