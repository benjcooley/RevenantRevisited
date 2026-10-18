// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *            node.cpp - TNode + component update list           *
// *************************************************************************

#include "node.h"

#include <cctype>
#include <cmath>

// ---- component update list ----------------------------------------------
//
// Components opt into per-pulse updates with RegisterUpdate. The list holds
// safe component refs, so a component that died prunes its own entry the
// next time the list runs.

namespace {

struct SComponentUpdateEntry
{
    TSafeComponentRef<TObjectComponent> component;
    TObjectComponentUpdateMethod method = nullptr;
};

std::vector<SComponentUpdateEntry> g_component_updates;

constexpr float kDegToRad = 3.14159265358979323846f / 180.0f;
constexpr float kRadToDeg = 180.0f / 3.14159265358979323846f;

bool IEquals(std::string_view a, const char* b)
{
    if (!b) return false;
    size_t n = 0;
    for (; b[n]; ++n)
    {
        if (n >= a.size()) return false;
        if (std::tolower(static_cast<unsigned char>(a[n])) != std::tolower(static_cast<unsigned char>(b[n])))
            return false;
    }
    return n == a.size();
}

} // namespace

void TObjectComponent::RegisterUpdate(TObjectComponentUpdateMethod method)
{
    if (!method || !Owner())
        return;

    for (const SComponentUpdateEntry& entry : g_component_updates)
        if (entry.method == method && entry.component.Get() == this)
            return;

    SComponentUpdateEntry entry = {};
    entry.component.Set(this);
    entry.method = method;
    g_component_updates.push_back(entry);
}

void TObjectComponent::UnregisterUpdate(TObjectComponentUpdateMethod method)
{
    for (size_t i = 0; i < g_component_updates.size(); )
    {
        TObjectComponent* component = g_component_updates[i].component.Get();
        if (!component || (component == this && (!method || g_component_updates[i].method == method)))
            g_component_updates.erase(g_component_updates.begin() + ptrdiff_t(i));
        else
            ++i;
    }
}

void TObjectComponent::RunUpdateList()
{
    for (size_t i = 0; i < g_component_updates.size(); )
    {
        SComponentUpdateEntry entry = g_component_updates[i];
        TObjectComponent* component = entry.component.Get();
        if (!component || !entry.method)
        {
            g_component_updates.erase(g_component_updates.begin() + ptrdiff_t(i));
            continue;
        }

        (component->*entry.method)();
        ++i;
    }
}

TObjectInstance* TObjectComponent::OwnerInstance() const
{
    return owner ? owner->AsObjectInstance() : nullptr;
}

// ---- TNode -------------------------------------------------------------

TNode::TNode()
{
    transform_.SetNode(this);
}

TNode::TNode(std::string name)
    : node_name_(std::move(name))
{
    transform_.SetNode(this);
}

TNode::~TNode()
{
    DetachComponents();
}

hmm_vec3 TNode::LocalRotDeg() const
{
    const hmm_vec3 r = transform_.LocalRotEuler();
    return hmm_vec3{{ r.X * kRadToDeg, r.Y * kRadToDeg, r.Z * kRadToDeg }};
}

void TNode::SetLocalRotDeg(const hmm_vec3& deg)
{
    transform_.SetLocalRotEuler(hmm_vec3{{ deg.X * kDegToRad, deg.Y * kDegToRad, deg.Z * kDegToRad }});
}

TNode* TNode::Parent() const
{
    const TTransform* p = transform_.Parent();
    return p ? p->Node() : nullptr;
}

void TNode::SetParent(TNode* parent)
{
    transform_.SetParent(parent ? &parent->transform_ : nullptr);
}

TNode* TNode::GetChildAt(int32_t index) const
{
    const TTransform* c = transform_.GetChildAt(index);
    return c ? c->Node() : nullptr;
}

TNode* TNode::FindChild(std::string_view name) const
{
    const int32_t n = GetChildCount();
    for (int32_t i = 0; i < n; ++i)
        if (TNode* child = GetChildAt(i))
            if (IEquals(name, child->GetName()))
                return child;
    return nullptr;
}

TNode* TNode::FindPath(std::string_view path) const
{
    const TNode* cur = this;
    while (!path.empty())
    {
        const size_t slash = path.find('/');
        const std::string_view seg = path.substr(0, slash);
        path = slash == std::string_view::npos ? std::string_view{} : path.substr(slash + 1);
        if (seg.empty() || seg == ".")
            continue;
        cur = cur->FindChild(seg);
        if (!cur)
            return nullptr;
    }
    return const_cast<TNode*>(cur);
}

int32_t TNode::AddComponent(std::unique_ptr<TObjectComponent> component)
{
    if (!component || component->Owner())
        return -1;

    static uint32_t s_next_component_generation = 1;
    int32_t slot = -1;
    for (int32_t i = 0; i < int32_t(components_.size()); ++i)
    {
        if (!components_[size_t(i)])
        {
            slot = i;
            break;
        }
    }
    if (slot < 0)
    {
        slot = int32_t(components_.size());
        components_.push_back(nullptr);
    }

    TObjectComponent* raw = component.get();
    components_[size_t(slot)] = std::move(component);

    raw->Attach(this, slot, ++s_next_component_generation);
    if (ActivatesComponentsOnAdd())
        raw->Activate();
    return slot;
}

void TNode::ActivateComponents()
{
    for (const auto& component_ptr : components_)
        if (TObjectComponent* c = component_ptr.get())
            c->Activate();
}

void TNode::DetachComponents()
{
    for (const auto& component_ptr : components_)
        if (TObjectComponent* component = component_ptr.get())
            component->Detach();
    components_.clear();
}

void TNode::RemoveComponent(int32_t component_slot)
{
    if ((uint32_t)component_slot >= (uint32_t)components_.size())
        return;
    TObjectComponent* component = components_[size_t(component_slot)].get();
    if (component)
        component->Detach();
    components_[size_t(component_slot)].reset();
}

TObjectComponent* TNode::GetComponent(int32_t component_slot) const
{
    if ((uint32_t)component_slot >= (uint32_t)components_.size())
        return nullptr;
    return components_[size_t(component_slot)].get();
}
