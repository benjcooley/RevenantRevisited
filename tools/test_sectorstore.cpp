// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   test_sectorstore.cpp - the working set and the base map             *
// *************************************************************************
//
// src/sectorstore.cpp and src/savefile.cpp as they are, over a temporary
// folder standing in for the base map.

#include "test_sectorimages.h"

#include "../src/revenant.h"
#include "../src/sectorstore.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using namespace SectorImages;

// ---- The engine's path layer, reduced to one folder -------------------------
// Retail paths ("<BaseMapPath>map\<file>") with '/' for '\'.

char BaseMapPath[MAXPATHLEN];

namespace {
std::string Native(const char* path)
{
    std::string s = path;
    std::replace(s.begin(), s.end(), '\\', '/');
    return s;
}
}  // namespace

FILE* rev_fopen(const char* file, const char* flags, EOpenOrder)
{
    return std::fopen(Native(file).c_str(), flags);
}

char* makepath(char* name, char* buf, int32_t buflen)
{
    std::snprintf(buf, size_t(buflen), "%s", Native(name).c_str());
    return buf;
}

int32_t stricmp(const char* a, const char* b)
{
    for (; *a && std::tolower(uint8_t(*a)) == std::tolower(uint8_t(*b)); ++a, ++b)
        ;
    return std::tolower(uint8_t(*a)) - std::tolower(uint8_t(*b));
}

namespace {

constexpr int32_t kOtherY = kY + 1;

std::vector<uint8_t> ReadFile(const fs::path& file)
{
    std::ifstream in(file, std::ios::binary);
    return {std::istreambuf_iterator<char>(in), {}};
}

void WriteFile(const fs::path& file, const std::vector<uint8_t>& bytes)
{
    fs::create_directories(file.parent_path());
    std::ofstream(file, std::ios::binary).write(reinterpret_cast<const char*>(bytes.data()),
                                                 std::streamsize(bytes.size()));
}

void Write(const std::string& name, const std::vector<uint8_t>& image)
{
    SectorStore::Write(name.c_str(), image.data(), image.size());
}

std::vector<uint8_t> Read(const std::string& name)
{
    std::vector<uint8_t> bytes;
    EXPECT_TRUE(SectorStore::Read(name.c_str(), bytes)) << name;
    return bytes;
}

class SectorStoreTest : public testing::Test
{
  protected:
    void SetUp() override
    {
        // One folder per test: ctest runs them side by side.
        root = fs::temp_directory_path() / "revenant_test_sectorstore" /
               testing::UnitTest::GetInstance()->current_test_info()->name();
        fs::remove_all(root);
        fs::create_directories(root / "map");
        std::snprintf(BaseMapPath, sizeof(BaseMapPath), "%s/", root.string().c_str());
        SectorStore::Clear();

        objs = SampleObjects();
        base = Image(Records(objs));
        WriteFile(root / "map" / name, base);
        WriteFile(root / "map" / other, Image(Records(objs), 15, kLevel, kX, kOtherY));
    }
    void TearDown() override
    {
        SectorStore::Clear();
        fs::remove_all(root);
    }

    // Sector `name` with the character moved by `dx`.
    std::vector<uint8_t> Moved(int32_t dx, int32_t y = kY) const
    {
        std::vector<SObj> now = objs;
        now[6].x += dx;
        return Image(Records(now), 15, kLevel, kX, y);
    }

    fs::path root;
    std::vector<SObj> objs;
    std::vector<uint8_t> base;
    const std::string name  = SaveFile::SectorFileName(kLevel, kX, kY);
    const std::string other = SaveFile::SectorFileName(kLevel, kX, kOtherY);
};

TEST_F(SectorStoreTest, KeepsWhatDiffersFromTheBaseMap)
{
    Write(name, base);
    EXPECT_TRUE(SectorStore::SectorsOn(kLevel).empty());

    Write(name, Moved(40));
    EXPECT_EQ(SectorStore::SectorsOn(kLevel).size(), 1u);
    EXPECT_EQ(Read(name), Moved(40));
    std::vector<uint8_t> fromBase;
    ASSERT_TRUE(SectorStore::ReadBase(name.c_str(), fromBase));
    EXPECT_EQ(fromBase, base);
}

TEST_F(SectorStoreTest, DeltasRebuildTheWorkingSet)
{
    const std::string added = SaveFile::SectorFileName(kLevel, 0, 0);    // no such sector in the base map
    const std::vector<uint8_t> addedImage = Image(Records(objs), 15, kLevel, 0, 0);
    Write(name, Moved(40));
    Write(added, addedImage);

    const std::vector<SaveFile::SSectorDelta> deltas = SectorStore::Deltas();
    ASSERT_EQ(deltas.size(), 2u);
    EXPECT_TRUE(deltas[0].whole);                       // 2_8_0 sorts first
    EXPECT_FALSE(deltas[1].whole);
    EXPECT_EQ(deltas[1].changes.size(), 1u);

    SectorStore::Clear();
    EXPECT_EQ(Read(name), base);
    SectorStore::SetDeltas(deltas);
    EXPECT_EQ(Read(name), Moved(40));
    EXPECT_EQ(Read(added), addedImage);
}

TEST_F(SectorStoreTest, TheBaseMapSavedAtALaterVersionIsNoChange)
{
    WriteFile(root / "map" / name, Image(Records(objs), 14));
    Write(name, base);                                  // the same objects, written at 15
    EXPECT_EQ(SectorStore::SectorsOn(kLevel).size(), 1u);
    EXPECT_TRUE(SectorStore::Deltas().empty());
}

TEST_F(SectorStoreTest, PublishingKeepsWhatWasPublishedBefore)
{
    Write(name, Moved(40));
    EXPECT_EQ(SectorStore::Publish(), 1);
    EXPECT_TRUE(SectorStore::SectorsOn(kLevel).empty());
    EXPECT_EQ(ReadFile(root / "map" / name), Moved(40));

    Write(name, Moved(40));                             // the published sector, unchanged since
    Write(other, Moved(8, kOtherY));
    EXPECT_EQ(SectorStore::Publish(), 1);
    EXPECT_EQ(ReadFile(root / "map" / name), Moved(40));
    EXPECT_EQ(ReadFile(root / "map" / other), Moved(8, kOtherY));
}

TEST_F(SectorStoreTest, ASlotFolderHoldsTheWorkingSetAlone)
{
    const fs::path slot = root / "slot" / "CurMap";
    WriteFile(slot / other, base);                      // the slot's previous save
    Write(name, Moved(40));
    EXPECT_EQ(SectorStore::ExportTo(slot), 1);
    EXPECT_FALSE(fs::exists(slot / other));
    EXPECT_EQ(ReadFile(slot / name), Moved(40));

    SectorStore::Clear();
    WriteFile(slot / other, Image(Records(objs), 15, kLevel, kX, kOtherY));  // the same as the base map's
    EXPECT_EQ(SectorStore::ImportFrom(slot), 2);
    EXPECT_EQ(SectorStore::SectorsOn(kLevel).size(), 1u);
    EXPECT_EQ(Read(name), Moved(40));
}

}  // namespace
