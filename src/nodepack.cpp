// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *          nodepack.cpp - packed reader / writer for a node tree        *
// *************************************************************************

#include "nodepack.h"
#include "node.h"
#include "packed.h"
#include "refl.h"

namespace nodepack {

namespace {

void PackNode(const TNode& node, TPackedWriter& w)
{
    if (const refl::SType* t = refl::Of(node))
        refl::Pack(*t, &node, w);

    for (int32_t i = 0; i < node.NumComponents(); ++i)
    {
        const TComponent* c = node.GetComponent(i);
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
        const TNode* child = node.GetChildAt(i);
        if (!child) continue;
        TPackedWriter body;
        PackNode(*child, body);
        TPackedWriter sub;
        sub.String(kSubName, child->GetName() ? child->GetName() : "");
        sub.Message(kSubNode, body);
        w.Message(kTagChild, sub);
    }
}

bool UnpackNode(TPackedReader& r, TNode& node,
                std::vector<std::unique_ptr<TNode>>& owned, std::string& error,
                bool apply_props, bool post_load)
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
            std::unique_ptr<TComponent> c(ct->create_component());
            c->SetLabel(label);
            if (have_body)
            {
                TPackedReader br(body);
                refl::Unpack(*ct, c.get(), br);
            }
            if (post_load)
                refl::PostLoad(*ct, c.get());          // fields complete, before attach
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
            TNode* child = nt->create_node();
            child->SetNodeName(name);
            child->SetParent(&node);
            owned.emplace_back(child);
            if (have_body)
            {
                TPackedReader br(body);
                if (!UnpackNode(br, *child, owned, error, true, post_load))
                    ok = false;
            }
            continue;
        }
        // A plain property of this node.
        if (t && apply_props)
            refl::ApplyPacked(*t, &node, f);
    }
    if (t && post_load)
        refl::PostLoad(*t, &node);                     // node complete: fields, components, children
    return ok;
}

} // namespace

void Pack(const TNode& root, TPackedWriter& w)
{
    PackNode(root, w);
}

bool Unpack(TPackedReader& r, TNode& root,
            std::vector<std::unique_ptr<TNode>>& owned, std::string* error,
            bool apply_root_props, bool post_load)
{
    std::string err;
    const bool ok = UnpackNode(r, root, owned, err, apply_root_props, post_load);
    if (error) *error = err;
    return ok;
}

} // namespace nodepack
