// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   test_savefile.cpp - the save file format (src/savefile.*)           *
// *************************************************************************

#include "test_sectorimages.h"

#include "../src/savefile.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using namespace SaveFile;
using namespace SectorImages;

namespace {

std::vector<uint8_t> Apply(const SSectorDelta& delta, const std::vector<uint8_t>& base, SApplyResult* result = nullptr)
{
    std::vector<uint8_t> image;
    SApplyResult r;
    EXPECT_TRUE(ApplySectorDelta(delta, &base, image, r));
    if (result)
        *result = r;
    return image;
}

int Count(const SSectorDelta& d, SObjectChange::EKind kind)
{
    return int(std::count_if(d.changes.begin(), d.changes.end(),
                             [&](const SObjectChange& c) { return c.kind == kind; }));
}

TEST(SectorDelta, UnchangedHasNoChanges)
{
    const std::vector<uint8_t> base = Image(Records(SampleObjects()));
    const SSectorDelta delta = MakeSectorDelta(kLevel, kX, kY, &base, base);
    EXPECT_FALSE(delta.whole);
    EXPECT_TRUE(delta.changes.empty());
    EXPECT_EQ(Apply(delta, base), base);
}

TEST(SectorDelta, ReplaceDeleteAdd)
{
    const std::vector<SObj> objs = SampleObjects();
    const std::vector<uint8_t> base = Image(Records(objs));

    std::vector<SObj> now = objs;
    now[6].x += 40;                                     // the character moved
    now[7].inventory = {1, 0, 0, 0};                    // the chest changed
    std::vector<std::vector<uint8_t>> slots = Records(now);
    slots[2].clear();                                   // a tile taken
    slots.push_back(Record({kTile, 99, 7000, 0, 0, {5}, {}}));   // one dropped
    const std::vector<uint8_t> image = Image(slots);

    const SSectorDelta delta = MakeSectorDelta(kLevel, kX, kY, &base, image);
    EXPECT_FALSE(delta.whole);
    EXPECT_EQ(Count(delta, SObjectChange::EKind::Replace), 2);
    EXPECT_EQ(Count(delta, SObjectChange::EKind::Delete), 1);
    EXPECT_EQ(Count(delta, SObjectChange::EKind::Add), 1);

    SApplyResult result;
    EXPECT_EQ(Apply(delta, base, &result), image);
    EXPECT_EQ(result.applied, 4);
    EXPECT_EQ(result.missed, 0);
}

TEST(SectorDelta, AnotherObjectInASlotIsADeleteAndAnAdd)
{
    const std::vector<SObj> objs = SampleObjects();
    const std::vector<uint8_t> base = Image(Records(objs));
    std::vector<std::vector<uint8_t>> slots = Records(objs);
    slots[1] = Record({kTile, 77, 5555, 0, 0, {}, {}});  // the slot reused by another object
    const SSectorDelta delta = MakeSectorDelta(kLevel, kX, kY, &base, Image(slots));
    EXPECT_EQ(Count(delta, SObjectChange::EKind::Replace), 0);
    EXPECT_EQ(Count(delta, SObjectChange::EKind::Delete), 1);
    EXPECT_EQ(Count(delta, SObjectChange::EKind::Add), 1);

    // Rebuilt: the slot empty, the new object after the base's.
    std::vector<std::vector<uint8_t>> expected = Records(objs);
    expected[1].clear();
    expected.push_back(slots[1]);
    EXPECT_EQ(Apply(delta, base), Image(expected));
}

TEST(SectorDelta, SurvivesTheSectorChangingUnderneath)
{
    const std::vector<SObj> objs = SampleObjects();
    const std::vector<uint8_t> base = Image(Records(objs));
    std::vector<SObj> now = objs;
    now[3].payload = {42};                              // the game changed tile 3
    now[6].x += 10;                                     // and the character
    const SSectorDelta delta = MakeSectorDelta(kLevel, kX, kY, &base, Image(Records(now)));
    ASSERT_EQ(Count(delta, SObjectChange::EKind::Replace), 2);

    // The module's sector is edited: an object inserted first, tile 0
    // edited, and the character -- one the save changed -- edited too.
    std::vector<SObj> edited = objs;
    edited[0].payload = {8, 8};
    edited[6].payload = {9, 9, 9, 9};
    edited.insert(edited.begin(), {kTile, 5, 999, 0, 0, {}, {}});
    const std::vector<uint8_t> newBase = Image(Records(edited));

    SApplyResult result;
    const std::vector<uint8_t> image = Apply(delta, newBase, &result);
    EXPECT_EQ(result.applied, 1);                       // tile 3, found where it now is
    EXPECT_EQ(result.missed, 1);                        // the character's record isn't the one changed

    std::vector<SObj> expected = edited;
    expected[4].payload = {42};
    EXPECT_EQ(image, Image(Records(expected)));
}

TEST(SectorDelta, IdenticalRecordsAreTakenInTurn)
{
    std::vector<SObj> objs = SampleObjects();
    objs[2] = objs[1];                                  // a stacked copy: same bytes
    const std::vector<uint8_t> base = Image(Records(objs));
    std::vector<std::vector<uint8_t>> slots = Records(objs);
    slots[2].clear();
    const std::vector<uint8_t> image = Image(slots);

    const SSectorDelta delta = MakeSectorDelta(kLevel, kX, kY, &base, image);
    ASSERT_EQ(delta.changes.size(), 1u);
    EXPECT_EQ(Apply(delta, base), image);

    const std::vector<uint64_t> keys = ObjectKeys(kLevel, kX, kY, base);
    EXPECT_NE(keys[1], keys[2]);                        // the copy number keeps them apart
}

TEST(SectorDelta, KeysDependOnTheSector)
{
    const std::vector<uint8_t> base = Image(Records(SampleObjects()));
    EXPECT_NE(ObjectKeys(kLevel, kX, kY, base)[0], ObjectKeys(kLevel, kX + 1, kY, base)[0]);
}

TEST(SectorDelta, WholeWhenThereIsNoUsableBase)
{
    const std::vector<uint8_t> image = Image(Records(SampleObjects()));
    EXPECT_TRUE(MakeSectorDelta(kLevel, kX, kY, nullptr, image).whole);
    const std::vector<uint8_t> older = Image(Records(SampleObjects(), 13), 13);   // no inventory sizes
    const SSectorDelta delta = MakeSectorDelta(kLevel, kX, kY, &older, image);
    EXPECT_TRUE(delta.whole);
    std::vector<uint8_t> out;
    SApplyResult result;
    ASSERT_TRUE(ApplySectorDelta(delta, nullptr, out, result));
    EXPECT_EQ(out, image);
}

TEST(SectorDelta, ALaterVersionAppliesToItsOwnBase)
{
    const std::vector<SObj> objs = SampleObjects();
    const std::vector<uint8_t> base = Image(Records(objs), 14);
    EXPECT_TRUE(MakeSectorDelta(kLevel, kX, kY, &base, Image(Records(objs))).changes.empty());

    std::vector<SObj> now = objs;
    now[6].x += 40;
    const std::vector<uint8_t> image = Image(Records(now));
    const SSectorDelta delta = MakeSectorDelta(kLevel, kX, kY, &base, image);
    ASSERT_FALSE(delta.whole);
    EXPECT_EQ(delta.version, 15);
    ASSERT_EQ(delta.changes.size(), 1u);
    EXPECT_EQ(Apply(delta, base), image);               // rebuilt at the delta's version

    // Another version-14 sector: its records can't be vouched for as 15's.
    std::vector<SObj> edited = objs;
    edited[0].payload = {8, 8};
    const std::vector<uint8_t> editedBase = Image(Records(edited), 14);
    std::vector<uint8_t> out;
    SApplyResult result;
    EXPECT_FALSE(ApplySectorDelta(delta, &editedBase, out, result));
    EXPECT_NE(result.failure, nullptr);

    // The sector saved at 15 since: matched by key as usual.
    const std::vector<uint8_t> savedBase = Image(Records(edited));
    std::vector<SObj> expected = edited;
    expected[6].x += 40;
    EXPECT_EQ(Apply(delta, savedBase), Image(Records(expected)));
}

TEST(SectorDelta, StateHashSkipsInventories)
{
    std::vector<SObj> objs = SampleObjects();
    const uint32_t without = SectorStateHash(kLevel, kX, kY, Image(Records(objs)));
    objs[6].inventory = {1, 0, 0, 0};                   // the character's inventory changed
    EXPECT_EQ(SectorStateHash(kLevel, kX, kY, Image(Records(objs))), without);
    objs[6].payload = {7};                              // its own record changed
    EXPECT_NE(SectorStateHash(kLevel, kX, kY, Image(Records(objs))), without);
}

SSaveFile SampleSave()
{
    const std::vector<SObj> objs = SampleObjects();
    const std::vector<uint8_t> base = Image(Records(objs));
    std::vector<SObj> now = objs;
    now[6].x += 1;

    SSaveFile save;
    save.game      = {1, 2, 3, 4, 5};
    save.thumbnail = {0x89, 'P', 'N', 'G'};
    SSectorDelta delta = MakeSectorDelta(kLevel, kX, kY, &base, Image(Records(now)));
    save.sectors.push_back(delta);
    save.sectors.push_back(MakeSectorDelta(1, 3, 4, nullptr, base));
    return save;
}

TEST(SaveFile, EncodesAndDecodes)
{
    SSaveFile save = SampleSave();
    save.other.push_back({MakeId('N', 'O', 'T', 'E'), {7, 7}});
    std::sort(save.sectors.begin(), save.sectors.end());

    const std::vector<uint8_t> bytes = Encode(save);
    ASSERT_GE(bytes.size(), 8u);
    EXPECT_EQ(std::string(bytes.begin(), bytes.begin() + 4), "RVSV");

    SSaveFile back;
    std::string error;
    ASSERT_TRUE(Decode(bytes, back, error)) << error;
    EXPECT_EQ(back.game, save.game);
    EXPECT_EQ(back.thumbnail, save.thumbnail);
    ASSERT_EQ(back.sectors.size(), 2u);
    EXPECT_TRUE(back.sectors[0].whole);
    EXPECT_EQ(back.sectors[0].image, save.sectors[0].image);
    EXPECT_EQ(back.sectors[1].version, save.sectors[1].version);
    EXPECT_EQ(back.sectors[1].basehash, save.sectors[1].basehash);
    EXPECT_NE(back.sectors[1].basehash, 0u);
    ASSERT_EQ(back.sectors[1].changes.size(), save.sectors[1].changes.size());
    for (size_t i = 0; i < save.sectors[1].changes.size(); ++i)
    {
        const SObjectChange& a = save.sectors[1].changes[i];
        const SObjectChange& b = back.sectors[1].changes[i];
        EXPECT_EQ(a.kind, b.kind);
        EXPECT_EQ(a.key, b.key);
        EXPECT_EQ(a.objclass, b.objclass);
        EXPECT_EQ(a.objtype, b.objtype);
        EXPECT_EQ(a.record, b.record);
    }
    ASSERT_EQ(back.other.size(), 1u);
    EXPECT_EQ(back.other[0].id, MakeId('N', 'O', 'T', 'E'));
}

TEST(SaveFile, RejectsWhatItCantRead)
{
    std::string error;
    SSaveFile out;
    EXPECT_FALSE(Decode({}, out, error));
    EXPECT_FALSE(Decode({'F', 'O', 'R', 'M', 0, 0, 0, 0}, out, error));

    std::vector<uint8_t> bytes = Encode(SampleSave());
    std::vector<uint8_t> cut(bytes.begin(), bytes.end() - 3);
    EXPECT_FALSE(Decode(cut, out, error));

    bytes[16] = 99;                                     // HEAD's version (after RVSV, size, HEAD, size)
    EXPECT_FALSE(Decode(bytes, out, error));
    EXPECT_NE(error.find("newer"), std::string::npos);
}

TEST(SaveFile, WritesAndReadsAFile)
{
    const fs::path dir = fs::temp_directory_path() / "revenant_test_savefile";
    fs::remove_all(dir);
    const fs::path file = dir / (std::string("Slot") + kExtension);

    std::string error;
    ASSERT_TRUE(Write(file, SampleSave(), error)) << error;
    EXPECT_FALSE(fs::exists(fs::path(file).concat(".partial")));

    SSaveFile back;
    ASSERT_TRUE(Read(file, back, error)) << error;
    ASSERT_EQ(back.sectors.size(), 2u);
    EXPECT_EQ(back.sectors[0].level, 1);                // written sorted
    EXPECT_EQ(back.sectors[1].level, 2);
    fs::remove_all(dir);
}

TEST(SaveFile, SectorFileNames)
{
    EXPECT_EQ(SectorFileName(2, 8, 10), "2_8_10.DAT");
    int32_t level = 0, x = 0, y = 0;
    ASSERT_TRUE(ParseSectorFileName("2_13_14.DAT", level, x, y));
    EXPECT_EQ(level, 2);
    EXPECT_EQ(x, 13);
    EXPECT_EQ(y, 14);
    EXPECT_TRUE(ParseSectorFileName("0_1_2.dat", level, x, y));
    EXPECT_FALSE(ParseSectorFileName("2_13.DAT", level, x, y));
    EXPECT_FALSE(ParseSectorFileName("2_13_14.BAK", level, x, y));
    EXPECT_FALSE(ParseSectorFileName("2_13_14.DATX", level, x, y));
}

}  // namespace
