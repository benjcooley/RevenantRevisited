// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *          nodepack.cpp - packed reader / writer for a node tree        *
// *************************************************************************

#include "nodepack.h"
#include "nodeinst.h"
#include "packed.h"
#include "refl.h"

namespace nodepack {

namespace {

constexpr uint16_t kSubName  = 1;    // def_name (component) / name (child)
constexpr uint16_t kSubLabel = 2;
constexpr uint16_t kSubBody  = 3;    // component props
constexpr uint16_t kSubNode  = 2;    // child node message

void PackNode(const TNodeInst& node, TPackedWriter& w)
{
    if (const refl::SType* t = refl::Of(node))
        refl::Pack(*t, &node, w);

    for (int32_t i = 0; i < node.NumComponents(); ++i)
    {
        const TObjectComponent* c = node.GetComponent(i);
        if (!c) continue;
        const refl::SType* ct = refl::Of(*c);
        if (!ct || !*ct->def_name)
            continue;                                // runtime-only component: not persisted
        TPackedWriter body;
        refl::Pack(*ct, c, body);
        TPackedWriter sub;
        sub.String(kSubName, ct->def_name);
        if (!c->Label().empty())
            sub.String(kSubLabel, c->Label());
        sub.Message(kSubBody, body);
        w.Message(kTagComponent, sub);
    }

    for (int32_t i = 0; i < node.GetChildCount(); ++i)
    {
        const TNodeInst* child = node.GetChildAt(i);
        if (!child) continue;
        TPackedWriter body;
        PackNode(*child, body);
        TPackedWriter sub;
        sub.String(kSubName, child->GetName() ? child->GetName() : "");
        sub.Message(kSubNode, body);
        w.Message(kTagChild, sub);
    }
}

bool UnpackNode(TPackedReader& r, TNodeInst& node,
                std::vector<std::unique_ptr<TNodeInst>>& owned, std::string& error)
{
    const refl::SType* t = refl::Of(node);
    bool ok = true;
    SPackedField f;
    while (r.Next(f))
    {
        if (f.id == kTagComponent)
        {
            TPackedReader sub(f);
            std::string def_name, label;
            SPackedField body = {};
            bool have_body = false;
            SPackedField sf;
            while (sub.Next(sf))
            {
                if (sf.id == kSubName)  def_name = std::string(TPackedReader::String(sf));
                if (sf.id == kSubLabel) label    = std::string(TPackedReader::String(sf));
                if (sf.id == kSubBody)  { body = sf; have_body = true; }
            }
            const refl::SType* ct = refl::FindByDefName(def_name);
            if (!ct || !ct->create_component)
            {
                if (error.empty()) error = "unknown component '" + def_name + "' skipped";
                ok = false;
                continue;
            }
            std::unique_ptr<TObjectComponent> c(ct->create_component());
            c->SetLabel(label);
            if (have_body)
            {
                TPackedReader br(body);
                refl::Unpack(*ct, c.get(), br);
            }
            refl::Loaded(*ct, c.get());              // fields complete, before attach
            node.AddComponent(std::move(c));
            continue;
        }
        if (f.id == kTagChild)
        {
            TPackedReader sub(f);
            std::string name;
            SPackedField body = {};
            bool have_body = false;
            SPackedField sf;
            while (sub.Next(sf))
            {
                if (sf.id == kSubName) name = std::string(TPackedReader::String(sf));
                if (sf.id == kSubNode) { body = sf; have_body = true; }
            }
            const refl::SType* nt = refl::FindByDefName("node");
            if (!nt || !nt->create_node)
            {
                if (error.empty()) error = "node type not registered";
                return false;
            }
            TNodeInst* child = nt->create_node();
            child->SetNodeName(name);
            child->SetParent(&node);
            owned.emplace_back(child);
            if (have_body)
            {
                TPackedReader br(body);
                if (!UnpackNode(br, *child, owned, error))
                    ok = false;
            }
            continue;
        }
        // A plain property of this node.
        if (!t) continue;
        const refl::SProp* p = refl::FindProp(*t, f.id);
        if (!p) continue;
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
        refl::Set(*t, &node, p->id, v);
    }
    if (t)
        refl::Loaded(*t, &node);                     // node complete: fields, components, children
    return ok;
}

} // namespace

void Pack(const TNodeInst& root, TPackedWriter& w)
{
    PackNode(root, w);
}

bool Unpack(TPackedReader& r, TNodeInst& root,
            std::vector<std::unique_ptr<TNodeInst>>& owned, std::string* error)
{
    std::string err;
    const bool ok = UnpackNode(r, root, owned, err);
    if (error) *error = err;
    return ok;
}

} // namespace nodepack
