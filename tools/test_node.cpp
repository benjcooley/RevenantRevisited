// *************************************************************************
// *     test_node.cpp - TNode: hierarchy, paths, components,      *
// *                         soft refs, annotated properties               *
// *************************************************************************

#include "../src/node.h"
#include "../src/packed.h"

#include <gtest/gtest.h>

#include <memory>

namespace {

int32_t g_detached = 0;   // counted externally: RemoveComponent destroys the component

struct STestComponent : public TComponent
{
    int32_t attached = 0, updates = 0;
    const char* ComponentName() const override { return "test"; }
    void OnAttach() override { ++attached; RegisterUpdate(&TComponent::Update); }
    void OnDetach() override { ++g_detached; UnregisterUpdate(&TComponent::Update); }
  protected:
    void OnUpdate() override { ++updates; }
};

} // namespace

TEST(Node, HierarchyAndPaths)
{
    TNode root("root");
    TNode core("core"), ring("ring"), flare("flare");
    core.SetParent(&root);
    ring.SetParent(&root);
    flare.SetParent(&core);

    EXPECT_EQ(root.GetChildCount(), 2);
    EXPECT_EQ(root.GetChildAt(0), &core);
    EXPECT_EQ(core.Parent(), &root);
    EXPECT_EQ(root.Parent(), nullptr);
    EXPECT_EQ(root.FindChild("RING"), &ring);            // case-insensitive
    EXPECT_EQ(root.FindPath("core/flare"), &flare);
    EXPECT_EQ(root.FindPath("core/"), &core);
    EXPECT_EQ(root.FindPath(""), &root);
    EXPECT_EQ(root.FindPath("core/nope"), nullptr);
    EXPECT_EQ(core.Transform().Node(), &core);            // back-pointer

    // World position composes through the node transforms.
    root.SetLocalPos(hmm_vec3{{100, 0, 0}});
    core.SetLocalPos(hmm_vec3{{0, 10, 0}});
    flare.SetLocalPos(hmm_vec3{{0, 0, 1}});
    const hmm_vec3 wp = flare.Transform().Pos();
    EXPECT_FLOAT_EQ(wp.X, 100); EXPECT_FLOAT_EQ(wp.Y, 10); EXPECT_FLOAT_EQ(wp.Z, 1);

    // Destroying a child detaches it from the parent's list.
    {
        TNode temp("temp");
        temp.SetParent(&root);
        EXPECT_EQ(root.GetChildCount(), 3);
    }
    EXPECT_EQ(root.GetChildCount(), 2);
    EXPECT_EQ(root.FindChild("temp"), nullptr);
}

TEST(Node, ComponentsAndSafeRefs)
{
    auto node = std::make_unique<TNode>("n");
    auto comp = std::make_unique<STestComponent>();
    STestComponent* raw = comp.get();
    const int32_t slot = node->AddComponent(std::move(comp));
    ASSERT_EQ(slot, 0);
    EXPECT_EQ(raw->Owner(), node.get());
    EXPECT_EQ(raw->OwnerInstance(), nullptr);             // plain node, not a map object
    EXPECT_EQ(raw->attached, 1);                          // plain nodes activate on add
    EXPECT_EQ(node->GetComponent<STestComponent>(), raw);
    EXPECT_EQ(node->GetComponent(slot), raw);

    TComponent::RunUpdateList();
    EXPECT_EQ(raw->updates, 1);

    TSafeComponentRef<STestComponent> ref(raw);
    EXPECT_TRUE(ref);
    EXPECT_EQ(ref.Get(), raw);
    TSafeRef<TNode> nref(node.get());
    EXPECT_EQ(nref.Get(), node.get());

    node->RemoveComponent(slot);                           // detaches and destroys it
    EXPECT_EQ(g_detached, 1);
    EXPECT_FALSE(ref);                                     // slot freed
    TComponent::RunUpdateList();                     // entry pruned, no crash

    // Slot reuse gets a fresh generation: a stale ref must not resolve.
    auto comp2 = std::make_unique<STestComponent>();
    STestComponent* raw2 = comp2.get();
    EXPECT_EQ(node->AddComponent(std::move(comp2)), 0);
    EXPECT_FALSE(ref);
    TSafeComponentRef<STestComponent> ref2(raw2);
    EXPECT_TRUE(ref2);

    node.reset();                                          // destroys components
    EXPECT_EQ(g_detached, 2);
    EXPECT_FALSE(ref2);
    EXPECT_FALSE(nref);
}

TEST(Node, AnnotatedProperties)
{
    const refl::SType* tp = refl::Of<TNode>();
    ASSERT_NE(tp, nullptr);
    EXPECT_NE(refl::FindProp(*tp, "pos"), nullptr);
    EXPECT_NE(refl::FindProp(*tp, "rot"), nullptr);
    EXPECT_NE(refl::FindProp(*tp, "scl"), nullptr);
    EXPECT_NE(refl::FindProp(*tp, "active"), nullptr);
    EXPECT_TRUE(refl::FindProp(*tp, "name")->read_only);
    // Accessor-backed defaults must match a fresh node (scl is 1,1,1 not 0,0,0).
    {
        TNode fresh("fresh");
        for (const refl::SType* t = tp; t; t = t->base)
            for (int32_t i = 0; i < t->prop_count; ++i)
            {
                const refl::SProp& p = t->props[i];
                if (p.read_only) continue;              // name: per instance
                SPropValue v;
                ASSERT_TRUE(refl::Get(*tp, &fresh, p.id, v)) << p.name;
                EXPECT_TRUE(refl::Equals(v, p.def)) << p.name << " default does not match a fresh node";
            }
        EXPECT_FLOAT_EQ(refl::FindProp(*tp, "scl")->def.v3[0], 1.0f);
    }

    TNode n("glow");
    EXPECT_TRUE(refl::Set(*tp, &n, refl::FindProp(*tp, "pos")->id, SPropValue::Vec3(1, 2, 3)));
    EXPECT_FLOAT_EQ(n.LocalPos().Z, 3.0f);
    EXPECT_TRUE(refl::SetLane(*tp, &n, refl::FindProp(*tp, "scl")->id, 2, 1.5f));
    EXPECT_FLOAT_EQ(n.LocalScl().Z, 1.5f);
    EXPECT_TRUE(refl::Set(*tp, &n, refl::FindProp(*tp, "rot")->id, SPropValue::Vec3(0, 0, 90)));
    EXPECT_NEAR(n.LocalRotDeg().Z, 90.0f, 1e-3f);
    EXPECT_TRUE(refl::SetLane(*tp, &n, refl::FindProp(*tp, "active")->id, 0, 0.0f));
    EXPECT_FALSE(n.Active());
    SPropValue v;
    ASSERT_TRUE(refl::Get(*tp, &n, refl::FindProp(*tp, "name")->id, v));
    EXPECT_EQ(v.str, "glow");

    // Packed round trip of a node's own properties.
    TPackedWriter w;
    refl::Pack(*tp, &n, w);
    TNode m("other");
    TPackedReader r(w.Buffer());
    refl::Unpack(*tp, &m, r);
    EXPECT_FLOAT_EQ(m.LocalPos().Y, 2.0f);
    EXPECT_FLOAT_EQ(m.LocalScl().Z, 1.5f);
    EXPECT_FALSE(m.Active());
    EXPECT_STREQ(m.GetName(), "other");                   // name is read-only: not packed
}
