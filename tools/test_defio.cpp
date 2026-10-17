// *************************************************************************
// *  test_defio.cpp - DEF text -> classes -> packed -> classes -> DEF     *
// *************************************************************************

#include "test_defio_types.h"
#include "../src/defio.h"
#include "../src/nodeinst.h"
#include "../src/nodepack.h"
#include "../src/packed.h"

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

namespace {

const char* kDoc = R"(
    active = true
    pos = [1, 2, 3]

    node core
    {
        pos = [0, 0, 10]
        glow
        {
            radius = 25
            color = [1, 0.5, 0]
            mode = steady
            texture = "Magic\\flare.i3d"
        }
        node tip
        {
            pos = [0, 0, 20]
            glow inner { radius = 5 }
        }
    }
    node ring
    {
        rot = [90, 0, 0]
    }
)";

void CheckTree(const TNodeInst& root)
{
    EXPECT_FLOAT_EQ(root.LocalPos().Y, 2.0f);
    ASSERT_EQ(root.GetChildCount(), 2);
    const TNodeInst* core = root.FindPath("core");
    ASSERT_NE(core, nullptr);
    EXPECT_FLOAT_EQ(core->LocalPos().Z, 10.0f);
    ASSERT_EQ(core->NumComponents(), 1);
    const STestGlow* g = core->GetComponent<STestGlow>();
    ASSERT_NE(g, nullptr);
    EXPECT_FLOAT_EQ(g->radius, 25.0f);
    EXPECT_EQ(g->color, (SRgba{1.0f, 0.5f, 0.0f, 1.0f}));
    EXPECT_EQ(g->mode, ETestMode::Steady);
    EXPECT_EQ(g->texture, "Magic\\flare.i3d");
    EXPECT_TRUE(g->Label().empty());
    EXPECT_EQ(g->loaded_count, 1);                     // OnLoaded once, after the fields
    EXPECT_FLOAT_EQ(g->radius_sq, 625.0f);
    EXPECT_FLOAT_EQ(g->radius_at_attach, 25.0f);       // attached after loading

    const TNodeInst* tip = root.FindPath("core/tip");
    ASSERT_NE(tip, nullptr);
    EXPECT_FLOAT_EQ(tip->LocalPos().Z, 20.0f);
    const STestGlow* inner = tip->GetComponent<STestGlow>();
    ASSERT_NE(inner, nullptr);
    EXPECT_FLOAT_EQ(inner->radius, 5.0f);
    EXPECT_EQ(inner->Label(), "inner");
    EXPECT_EQ(inner->mode, ETestMode::Pulse);          // default kept

    const TNodeInst* ring = root.FindPath("ring");
    ASSERT_NE(ring, nullptr);
    EXPECT_NEAR(ring->LocalRotDeg().X, 90.0f, 1e-3f);
    // World position composes through the tree.
    EXPECT_FLOAT_EQ(tip->Transform().Pos().Z, 33.0f);
}

} // namespace

TEST(DefIo, ReadsIntoClasses)
{
    TNodeInst root("root");
    std::vector<std::unique_ptr<TNodeInst>> owned;
    std::string err;
    ASSERT_TRUE(defio::Read(kDoc, root, owned, &err)) << err;
    EXPECT_EQ(owned.size(), 3u);
    CheckTree(root);
}

TEST(DefIo, PackedRoundTrip)
{
    TNodeInst root("root");
    std::vector<std::unique_ptr<TNodeInst>> owned;
    ASSERT_TRUE(defio::Read(kDoc, root, owned));

    TPackedWriter w;
    nodepack::Pack(root, w);
    EXPECT_GT(w.Buffer().size(), 40u);

    TNodeInst root2("root");
    std::vector<std::unique_ptr<TNodeInst>> owned2;
    TPackedReader r(w.Buffer());
    std::string err;
    ASSERT_TRUE(nodepack::Unpack(r, root2, owned2, &err)) << err;
    CheckTree(root2);

    // Packing the unpacked tree gives identical bytes.
    TPackedWriter w2;
    nodepack::Pack(root2, w2);
    EXPECT_EQ(w.Buffer(), w2.Buffer());
}

TEST(DefIo, WriteThenReadAgain)
{
    TNodeInst root("root");
    std::vector<std::unique_ptr<TNodeInst>> owned;
    ASSERT_TRUE(defio::Read(kDoc, root, owned));

    const std::string text = defio::Write(root);
    // Only non-default values are written, with components and children nested.
    EXPECT_EQ(text.find("node root"), std::string::npos);       // the document IS the root's body
    EXPECT_NE(text.find("node core"), std::string::npos);
    EXPECT_NE(text.find("glow inner"), std::string::npos);
    EXPECT_NE(text.find("mode = steady"), std::string::npos);
    EXPECT_EQ(text.find("phase"), std::string::npos);            // PROP_NOSAVE
    EXPECT_EQ(text.find("active"), std::string::npos);           // equals default

    TNodeInst root3("root");
    std::vector<std::unique_ptr<TNodeInst>> owned3;
    std::string err;
    ASSERT_TRUE(defio::Read(text, root3, owned3, &err)) << err << "\n" << text;
    CheckTree(root3);

    TPackedWriter a, b;
    nodepack::Pack(root, a);
    nodepack::Pack(root3, b);
    EXPECT_EQ(a.Buffer(), b.Buffer());
}

TEST(DefIo, StrictErrorsWithLines)
{
    TNodeInst root("root");
    std::vector<std::unique_ptr<TNodeInst>> owned;
    std::string err;

    EXPECT_FALSE(defio::Read("pos = [1, 2, 3]\nnode a\n{\n    glow { radiius = 1 }\n}\n", root, owned, &err));
    EXPECT_NE(err.find("line 4"), std::string::npos) << err;
    EXPECT_NE(err.find("radiius"), std::string::npos) << err;

    EXPECT_FALSE(defio::Read("node a\n{\n    sparkle { x = 1 }\n}\n", root, owned, &err));
    EXPECT_NE(err.find("unknown block 'sparkle'"), std::string::npos) << err;

    EXPECT_FALSE(defio::Read("node a { glow { mode = flicker } }", root, owned, &err));
    EXPECT_NE(err.find("unknown option 'flicker'"), std::string::npos) << err;

    EXPECT_FALSE(defio::Read("name = \"x\"", root, owned, &err));
    EXPECT_NE(err.find("read-only"), std::string::npos) << err;

    EXPECT_FALSE(defio::Read("node a { pos = [1, 2 }", root, owned, &err));   // parse error
    EXPECT_FALSE(err.empty());
}

TEST(DefIo, FlatObject)
{
    STestSettings s;
    std::string err;
    ASSERT_TRUE(defio::ReadObject("enabled = false\ncount = 9\ntint = [1, 0, 0]\nname = custom\n", s, &err)) << err;
    EXPECT_FALSE(s.enabled);
    EXPECT_EQ(s.count, 9);
    EXPECT_FLOAT_EQ(s.rate, 0.5f);
    EXPECT_EQ(s.tint, (SRgba{1, 0, 0, 1}));
    EXPECT_EQ(s.name, "custom");

    std::string out;
    defio::WriteObject(s, out);
    EXPECT_EQ(out, "enabled = false\ncount = 9\ntint = [1, 0, 0, 1]\nname = custom\n");

    EXPECT_FALSE(defio::ReadObject("node x { }", s, &err));       // no blocks in a flat object
    EXPECT_NE(err.find("needs a node"), std::string::npos) << err;
    EXPECT_FALSE(defio::ReadObject("sparkle { }", s, &err));
    EXPECT_NE(err.find("unknown block"), std::string::npos) << err;

    // Reset to table defaults.
    refl::Reset(*refl::Of(s), &s);
    EXPECT_TRUE(s.enabled);
    EXPECT_EQ(s.count, 4);
    EXPECT_EQ(s.name, "default");
    EXPECT_EQ(s.tint, (SRgba{0.2f, 0.4f, 0.6f, 1.0f}));
}
