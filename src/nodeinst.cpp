// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *            nodeinst.cpp - TNodeInst + component update list           *
// *************************************************************************

#include "nodeinst.h"

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

// ---- TNodeInst -------------------------------------------------------------

TNodeInst::TNodeInst()
{
    transform_.SetNode(this);
}

TNodeInst::TNodeInst(std::string name)
    : node_name_(std::move(name))
{
    transform_.SetNode(this);
}

TNodeInst::~TNodeInst()
{
    DetachComponents();
}

hmm_vec3 TNodeInst::LocalRotDeg() const
{
    const hmm_vec3 r = transform_.LocalRotEuler();
    return hmm_vec3{{ r.X * kRadToDeg, r.Y * kRadToDeg, r.Z * kRadToDeg }};
}

void TNodeInst::SetLocalRotDeg(const hmm_vec3& deg)
{
    transform_.SetLocalRotEuler(hmm_vec3{{ deg.X * kDegToRad, deg.Y * kDegToRad, deg.Z * kDegToRad }});
}

TNodeInst* TNodeInst::Parent() const
{
    const TTransform* p = transform_.Parent();
    return p ? p->Node() : nullptr;
}

void TNodeInst::SetParent(TNodeInst* parent)
{
    transform_.SetParent(parent ? &parent->transform_ : nullptr);
}

TNodeInst* TNodeInst::GetChildAt(int32_t index) const
{
    const TTransform* c = transform_.GetChildAt(index);
    return c ? c->Node() : nullptr;
}

TNodeInst* TNodeInst::FindChild(std::string_view name) const
{
    const int32_t n = GetChildCount();
    for (int32_t i = 0; i < n; ++i)
        if (TNodeInst* child = GetChildAt(i))
            if (IEquals(name, child->GetName()))
                return child;
    return nullptr;
}

TNodeInst* TNodeInst::FindPath(std::string_view path) const
{
    const TNodeInst* cur = this;
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
    return const_cast<TNodeInst*>(cur);
}

int32_t TNodeInst::AddComponent(std::unique_ptr<TObjectComponent> component)
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

void TNodeInst::ActivateComponents()
{
    for (const auto& component_ptr : components_)
        if (TObjectComponent* c = component_ptr.get())
            c->Activate();
}

void TNodeInst::DetachComponents()
{
    for (const auto& component_ptr : components_)
        if (TObjectComponent* component = component_ptr.get())
            component->Detach();
    components_.clear();
}

void TNodeInst::RemoveComponent(int32_t component_slot)
{
    if ((uint32_t)component_slot >= (uint32_t)components_.size())
        return;
    TObjectComponent* component = components_[size_t(component_slot)].get();
    if (component)
        component->Detach();
    components_[size_t(component_slot)].reset();
}

TObjectComponent* TNodeInst::GetComponent(int32_t component_slot) const
{
    if ((uint32_t)component_slot >= (uint32_t)components_.size())
        return nullptr;
    return components_[size_t(component_slot)].get();
}
