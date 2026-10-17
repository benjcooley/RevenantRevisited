// *************************************************************************
// *       test_props.cpp - generator polyfill + packed codec + DEF bake   *
// *************************************************************************

#include "test_props_types.h"
#include "../src/packed.h"
#include "../src/propdef.h"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

namespace {
const refl::SType& T(const char* name)
{
    const refl::SType* t = refl::Find(name);
    if (!t) throw std::runtime_error(std::string("not registered: ") + name);
    return *t;
}
} // namespace

TEST(Props, TableFromAnnotations)
{
    const refl::SType* tp = refl::Of<STestMesh>();
    ASSERT_NE(tp, nullptr);
    const refl::SType& type = *tp;
    EXPECT_STREQ(type.name, "STestMesh");
    ASSERT_EQ(type.prop_count, 8);
    const refl::SProp* t = type.props;
    EXPECT_EQ(t[0].id, 1);
    EXPECT_STREQ(t[1].name, "diffuse");
    EXPECT_EQ(t[1].type, EPropType::Color);
    EXPECT_EQ(t[2].flags, uint8_t(PropFlag_NoSave));
    EXPECT_EQ(t[6].type, EPropType::Enum);
    ASSERT_EQ(t[6].option_count, 3);
    EXPECT_STREQ(t[6].options[2].name, "additive_straight");
    EXPECT_EQ(refl::FindProp(type, "Diffuse"), &t[1]);          // case-insensitive
    EXPECT_EQ(refl::FindProp(type, uint16_t(7)), &t[6]);
    EXPECT_EQ(refl::FindProp(type, "nope"), nullptr);
    EXPECT_EQ(&T("stestmesh"), &type);                          // by name, case-insensitive

    STestMesh m;
    EXPECT_EQ(refl::Of(m), &type);                              // from an object
    EXPECT_EQ(refl::Of<STestEmpty>(), nullptr);                 // not annotated
}

TEST(Props, InheritanceSeesBaseProps)
{
    const refl::SType* d = refl::Of<STestDerived>();
    ASSERT_NE(d, nullptr);
    EXPECT_EQ(d->prop_count, 1);
    ASSERT_NE(refl::FindProp(*d, "diffuse"), nullptr);          // from the base
    ASSERT_NE(d->base, nullptr);
    EXPECT_STREQ(d->base->name, "STestMesh");

    STestDerived obj;
    EXPECT_TRUE(refl::Set(*d, &obj, 20, SPropValue::Float(9.0f)));
    EXPECT_TRUE(refl::Set(*d, &obj, 5, SPropValue::Int(3)));    // base's frame
    EXPECT_FLOAT_EQ(obj.extra, 9.0f);
    EXPECT_EQ(obj.frame, 3);
    EXPECT_EQ(obj.changed_count, 1);                            // base hook ran for the base set

    TPackedWriter w;
    refl::Pack(*d, &obj, w);
    STestDerived back;
    TPackedReader r(w.Buffer());
    refl::Unpack(*d, &back, r);
    EXPECT_FLOAT_EQ(back.extra, 9.0f);
    EXPECT_EQ(back.frame, 3);
}

TEST(Props, SetGetAndHook)
{
    STestMesh m;
    const refl::SType& type = *refl::Of(m);
    EXPECT_TRUE(refl::Set(type, &m, 2, SPropValue::Color(0.5f, 0.25f, 0.0f, 1.0f)));
    EXPECT_EQ(m.diffuse, (SRgba{0.5f, 0.25f, 0.0f, 1.0f}));
    EXPECT_EQ(m.changed_count, 1);
    EXPECT_EQ(m.last_changed, 2);

    EXPECT_FALSE(refl::Set(type, &m, 2, SPropValue::Float(1.0f)));        // type mismatch
    EXPECT_TRUE(refl::Set(type, &m, 3, SPropValue::Int(3)));              // int -> float coercion
    EXPECT_FLOAT_EQ(m.uv_scroll, 3.0f);
    EXPECT_FALSE(refl::Set(type, &m, 8, SPropValue::Int(1)));             // read-only
    EXPECT_EQ(m.generation, 7);
    EXPECT_FALSE(refl::Set(type, &m, 999, SPropValue::Int(1)));           // unknown id

    EXPECT_TRUE(refl::Set(type, &m, 6, SPropValue::String("cyl01")));
    EXPECT_EQ(m.object, "cyl01");
    EXPECT_TRUE(refl::Set(type, &m, 7, SPropValue::Enum(int32_t(ETestBlend::Additive))));
    EXPECT_EQ(m.blend, ETestBlend::Additive);

    SPropValue v;
    ASSERT_TRUE(refl::Get(type, &m, 6, v));
    EXPECT_EQ(v.type, EPropType::String);
    EXPECT_EQ(v.str, "cyl01");
    ASSERT_TRUE(refl::Get(type, &m, 8, v));
    EXPECT_EQ(v.i, 7);
}

TEST(Props, Lanes)
{
    const refl::SType& type = *refl::Of<STestMesh>();
    STestMesh m;
    EXPECT_TRUE(refl::SetLane(type, &m, 4, 2, 42.0f));                    // pos.z
    EXPECT_FLOAT_EQ(m.pos.Z, 42.0f);
    EXPECT_TRUE(refl::SetLane(type, &m, 2, 3, 0.25f));                    // diffuse.a
    EXPECT_FLOAT_EQ(m.diffuse.a, 0.25f);
    EXPECT_FALSE(refl::SetLane(type, &m, 4, 3, 1.0f));                    // no 4th lane on vec3
    EXPECT_TRUE(refl::SetLane(type, &m, 1, 0, 0.0f));                     // bool from float
    EXPECT_FALSE(m.visible);
    float x = 0;
    ASSERT_TRUE(refl::GetLane(type, &m, 2, 3, x));
    EXPECT_FLOAT_EQ(x, 0.25f);
    EXPECT_EQ(PropLaneIndex(EPropType::Color, "a"), 3);
    EXPECT_EQ(PropLaneIndex(EPropType::Vec3, "y"), 1);
    EXPECT_EQ(PropLaneIndex(EPropType::Float, "x"), -1);
}

TEST(Packed, RoundTrip)
{
    const refl::SType& type = *refl::Of<STestMesh>();
    STestMesh a;
    a.visible = false;
    a.diffuse = {0.1f, 0.2f, 0.3f, 0.4f};
    a.uv_scroll = 9.0f;                 // NoSave: must not survive
    a.pos = {{1, -2, 3}};
    a.frame = -17;
    a.object = "ring02";
    a.blend = ETestBlend::AdditiveStraight;

    TPackedWriter w;
    refl::Pack(type, &a, w);

    STestMesh b;
    TPackedReader r(w.Buffer());
    refl::Unpack(type, &b, r);
    EXPECT_TRUE(r.AtEnd());

    EXPECT_EQ(b.visible, false);
    EXPECT_EQ(b.diffuse, a.diffuse);
    EXPECT_FLOAT_EQ(b.uv_scroll, 0.0f);
    EXPECT_FLOAT_EQ(b.pos.X, 1); EXPECT_FLOAT_EQ(b.pos.Y, -2); EXPECT_FLOAT_EQ(b.pos.Z, 3);
    EXPECT_EQ(b.frame, -17);
    EXPECT_EQ(b.object, "ring02");
    EXPECT_EQ(b.blend, ETestBlend::AdditiveStraight);
    EXPECT_EQ(b.generation, 7);         // read-only: not packed, default kept
}

TEST(Packed, UnknownFieldsAreSkipped)
{
    const refl::SType& type = *refl::Of<STestMesh>();
    TPackedWriter w;
    w.Int(1000, 5);                     // unknown varint
    w.Float(1001, 2.0f);                // unknown fixed32
    w.String(1002, "junk");             // unknown bytes
    w.Int(5, 3);                        // frame
    TPackedWriter nested;
    nested.Int(1, 1);
    w.Message(1003, nested);            // unknown nested message
    w.String(6, "after");               // object, after the junk

    STestMesh m;
    TPackedReader r(w.Buffer());
    refl::Unpack(type, &m, r);
    EXPECT_TRUE(r.AtEnd());
    EXPECT_EQ(m.frame, 3);
    EXPECT_EQ(m.object, "after");
}

TEST(Packed, TruncatedBufferStopsCleanly)
{
    const refl::SType& type = *refl::Of<STestMesh>();
    TPackedWriter w;
    w.String(6, "hello");
    std::vector<uint8_t> cut(w.Buffer().begin(), w.Buffer().end() - 2);
    STestMesh m;
    TPackedReader r(cut);
    refl::Unpack(type, &m, r);
    EXPECT_EQ(m.object, "");
    EXPECT_FALSE(r.AtEnd());            // stopped on the malformed field
}

TEST(Packed, Varints)
{
    TPackedWriter w;
    w.Int(1, 0); w.Int(2, -1); w.Int(3, 300); w.Int(4, INT32_MIN); w.Int(5, INT32_MAX);
    TPackedReader r(w.Buffer());
    SPackedField f;
    ASSERT_TRUE(r.Next(f)); EXPECT_EQ(TPackedReader::Int(f), 0);
    ASSERT_TRUE(r.Next(f)); EXPECT_EQ(TPackedReader::Int(f), -1);
    ASSERT_TRUE(r.Next(f)); EXPECT_EQ(TPackedReader::Int(f), 300);
    ASSERT_TRUE(r.Next(f)); EXPECT_EQ(TPackedReader::Int(f), INT32_MIN);
    ASSERT_TRUE(r.Next(f)); EXPECT_EQ(TPackedReader::Int(f), INT32_MAX);
    EXPECT_FALSE(r.Next(f));
}

TEST(PropDef, ApplyAndWrite)
{
    const char* text =
        "mesh core\n"
        "{\n"
        "    visible = false\n"
        "    diffuse = [0.5, 0.25, 0.0]\n"
        "    pos = [10, 20, 30]\n"
        "    frame = 4\n"
        "    object = \"cyl01\"\n"
        "    blend = additive_straight\n"
        "    child sub { frame = 1 }\n"       // sub-block: ignored by Apply
        "}\n";
    defdoc::Document doc = defdoc::Document::parse(text);
    const defdoc::Node* block = doc.root().blocks("mesh").front();
    ASSERT_NE(block, nullptr);

    STestMesh m;
    std::string err;
    EXPECT_TRUE(propdef::Apply(m, *block, &err)) << err;
    EXPECT_FALSE(m.visible);
    EXPECT_EQ(m.diffuse, (SRgba{0.5f, 0.25f, 0.0f, 1.0f}));
    EXPECT_FLOAT_EQ(m.pos.Y, 20.0f);
    EXPECT_EQ(m.frame, 4);
    EXPECT_EQ(m.object, "cyl01");
    EXPECT_EQ(m.blend, ETestBlend::AdditiveStraight);

    // Write back, re-apply onto a fresh object, compare.
    defdoc::Node out;
    propdef::Write(m, out);
    EXPECT_FALSE(out.contains("uv_scroll"));       // NoSave
    EXPECT_FALSE(out.contains("generation"));      // ReadOnly
    EXPECT_EQ(out["blend"].as_string(), "additive_straight");
    STestMesh m2;
    EXPECT_TRUE(propdef::Apply(m2, out, &err)) << err;
    EXPECT_EQ(m2.object, "cyl01");
    EXPECT_EQ(m2.diffuse, m.diffuse);
    EXPECT_EQ(m2.blend, m.blend);
}

TEST(PropDef, StrictErrors)
{
    STestMesh m;
    std::string err;
    defdoc::Document doc = defdoc::Document::parse("mesh x { colour = [1,1,1] }");
    EXPECT_FALSE(propdef::Apply(m, *doc.root().blocks("mesh").front(), &err));
    EXPECT_NE(err.find("unknown property 'colour'"), std::string::npos) << err;

    doc = defdoc::Document::parse("mesh x { blend = subtractive }");
    EXPECT_FALSE(propdef::Apply(m, *doc.root().blocks("mesh").front(), &err));
    EXPECT_NE(err.find("unknown option 'subtractive'"), std::string::npos) << err;

    doc = defdoc::Document::parse("mesh x { generation = 3 }");
    EXPECT_FALSE(propdef::Apply(m, *doc.root().blocks("mesh").front(), &err));
    EXPECT_NE(err.find("read-only"), std::string::npos) << err;
    EXPECT_EQ(m.generation, 7);
}

TEST(Props, AccessorBacked)
{
    const refl::SType& type = *refl::Of<STestAccessor>();
    STestAccessor a;
    ASSERT_EQ(type.prop_count, 4);
    const refl::SProp* t = type.props;
    EXPECT_STREQ(t[0].name, "pos");
    EXPECT_EQ(t[2].flags & PropFlag_ReadOnly, PropFlag_ReadOnly);
    EXPECT_EQ(t[3].flags & PropFlag_Hidden, PropFlag_Hidden);

    EXPECT_TRUE(refl::Set(type, &a, 1, SPropValue::Vec3(1, 2, 3)));
    EXPECT_FLOAT_EQ(a.LocalPos().Z, 3.0f);
    EXPECT_TRUE(refl::SetLane(type, &a, 1, 1, 9.0f));
    EXPECT_FLOAT_EQ(a.LocalPos().Y, 9.0f);
    EXPECT_TRUE(refl::Set(type, &a, 2, SPropValue::Int(2)));
    EXPECT_FLOAT_EQ(a.Scale(), 2.0f);
    EXPECT_FALSE(refl::Set(type, &a, 3, SPropValue::String("x")));
    EXPECT_TRUE(refl::Set(type, &a, 4, SPropValue::Enum(int32_t(ETestBlend::Additive))));
    EXPECT_EQ(a.Blend(), ETestBlend::Additive);
    EXPECT_EQ(a.sets, 4);

    SPropValue v;
    ASSERT_TRUE(refl::Get(type, &a, 3, v));
    EXPECT_EQ(v.str, "node");
    ASSERT_TRUE(refl::Get(type, &a, 1, v));
    EXPECT_FLOAT_EQ(v.v3[1], 9.0f);

    // Packed round trip skips the read-only name and keeps the hidden blend.
    TPackedWriter w;
    refl::Pack(type, &a, w);
    STestAccessor b;
    TPackedReader r(w.Buffer());
    refl::Unpack(type, &b, r);
    EXPECT_FLOAT_EQ(b.LocalPos().Y, 9.0f);
    EXPECT_FLOAT_EQ(b.Scale(), 2.0f);
    EXPECT_EQ(b.Blend(), ETestBlend::Additive);
}
