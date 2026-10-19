// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *          nodepack.cpp - packed reader / writer for a node tree        *
// *************************************************************************

#include "engine/refl/nodepack.h"

#include <unordered_map>
#include "engine/scene/node.h"
#include "engine/refl/packed.h"
#include "engine/refl/refl.h"

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
            // A child the document gives no uid takes one derived from its
            // parent's and its place, so a document reads the same way
            // every time; a uid in the body replaces it below.
            child->SetUid(TUid::Derived(node.Uid().value, uint64_t(node.GetChildCount())));
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

namespace {

// Rewrites every node ref in `obj` (nested records included) found in `map`.
void RemapRefs(const refl::SType& type, void* obj, const std::unordered_map<uint64_t, uint64_t>& map)
{
    for (const refl::SType* t = &type; t; t = t->base)
    {
        for (int32_t i = 0; i < t->prop_count; ++i)
        {
            const refl::SProp& p = t->props[i];
            if (p.type == EPropType::Struct)
            {
                if (const refl::SType* et = refl::ElemType(p))
                    if (void* n = refl::Nested(type, obj, p.id))
                        RemapRefs(*et, n, map);
                continue;
            }
            if (p.type == EPropType::List)
            {
                const refl::SType* et = refl::ElemType(p);
                if (!et) continue;
                const int32_t count = refl::ListCount(type, obj, p.id);
                for (int32_t k = 0; k < count; ++k)
                    if (void* e = refl::ListAt(type, obj, p.id, k))
                        RemapRefs(*et, e, map);
                continue;
            }
            if (p.type != EPropType::Ref || p.ref_kind != ERefKind::Node || p.read_only)
                continue;
            SPropValue v;
            if (!refl::Get(type, obj, p.id, v))
                continue;
            auto it = map.find(v.u);
            if (it != map.end())
                refl::Set(type, obj, p.id, SPropValue::Ref(it->second));
        }
    }
}

void RemapNode(TNode& node, const std::unordered_map<uint64_t, uint64_t>& map)
{
    if (const refl::SType* t = refl::Of(node))
        RemapRefs(*t, &node, map);
    for (int32_t i = 0; i < node.NumComponents(); ++i)
        if (TComponent* c = node.GetComponent(i))
            if (const refl::SType* ct = refl::Of(*c))
                RemapRefs(*ct, c, map);
}

} // namespace

void Reidentify(TNode& root, const std::vector<std::unique_ptr<TNode>>& stamped)
{
    std::unordered_map<uint64_t, uint64_t> map;
    for (const auto& node : stamped)
    {
        if (!node) continue;
        const TUid fresh = TUid::Random();
        map[node->Uid().value] = fresh.value;
        node->SetUid(fresh);
    }
    if (map.empty())
        return;
    RemapNode(root, map);
    for (const auto& node : stamped)
        if (node)
            RemapNode(*node, map);
}

} // namespace nodepack
