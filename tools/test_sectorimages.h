// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   test_sectorimages.h - sector images for the save tests              *
// *************************************************************************
//
// Builds sector files (`<level>_<x>_<y>.DAT`) the way the game writes them,
// for test_savefile and test_sectorstore.
#pragma once

#include "../src/game/save/savefile.h"

#include <cstdint>
#include <cstring>
#include <vector>

namespace SectorImages
{

using SaveFile::SectorStateHash;

constexpr int32_t kLevel = 2, kX = 8, kY = 10;
constexpr int16_t kTile = 9, kCharacter = 12, kContainer = 5;

inline void Put16(std::vector<uint8_t>& b, uint16_t v) { b.push_back(uint8_t(v)); b.push_back(uint8_t(v >> 8)); }
inline void Put32(std::vector<uint8_t>& b, uint32_t v) { for (int i = 0; i < 4; ++i) b.push_back(uint8_t(v >> (8 * i))); }

// An object record as SaveObject writes one: the header, TObjectInstance::
// Save's fields (an immobile object), `payload`, and `inventory` after the
// body (map version 14 on).
struct SObj
{
    int16_t  objclass = kTile;
    uint32_t objtype  = 1;
    int32_t  mapindex = 0;
    int32_t  x = 0, y = 0;
    std::vector<uint8_t> payload;
    std::vector<uint8_t> inventory;
};

inline std::vector<uint8_t> Record(const SObj& o, int32_t version = 15)
{
    std::vector<uint8_t> body;
    body.push_back(0);                          // no name
    Put32(body, 1);                             // flags: OF_IMMOBILE (no velocity)
    Put32(body, uint32_t(o.x));
    Put32(body, uint32_t(o.y));
    Put32(body, 0);
    Put16(body, 0);                             // state
    Put16(body, 0xffff);                        // inventnum
    Put16(body, 0);                             // invindex
    Put32(body, 0xffffffff);                    // shadow
    body.push_back(0); body.push_back(0); body.push_back(0);    // rotations
    Put32(body, uint32_t(o.mapindex));
    body.insert(body.end(), o.payload.begin(), o.payload.end());

    std::vector<uint8_t> rec;
    Put16(rec, 3);                              // object version
    Put16(rec, uint16_t(o.objclass));
    Put32(rec, o.objtype);
    if (version < 14)
    {
        Put16(rec, uint16_t(body.size()));
        rec.insert(rec.end(), body.begin(), body.end());
        return rec;
    }
    Put16(rec, uint16_t(body.size() + o.inventory.size()));
    Put16(rec, uint16_t(o.inventory.size()));
    rec.insert(rec.end(), body.begin(), body.end());
    rec.insert(rec.end(), o.inventory.begin(), o.inventory.end());
    return rec;
}

// Sector `level`_`x`_`y`'s image of `slots` (an empty vector: an empty
// slot), with its state hash as SaveFile computes it.
inline std::vector<uint8_t> Image(const std::vector<std::vector<uint8_t>>& slots, int32_t version = 15,
                                  int32_t level = kLevel, int32_t x = kX, int32_t y = kY)
{
    std::vector<uint8_t> b = {'M', 'A', 'P', ' '};
    Put32(b, uint32_t(version));
    const size_t hashAt = b.size();
    if (version > 13)
        Put32(b, 0);
    Put32(b, uint32_t(slots.size()));
    for (const std::vector<uint8_t>& slot : slots)
    {
        if (slot.empty())
            Put16(b, 0xffff);
        else
            b.insert(b.end(), slot.begin(), slot.end());
    }
    if (version > 13)
    {
        const uint32_t hash = SectorStateHash(level, x, y, b);
        std::memcpy(b.data() + hashAt, &hash, 4);
    }
    return b;
}

inline std::vector<SObj> SampleObjects()
{
    std::vector<SObj> objs;
    for (int i = 0; i < 6; ++i)
        objs.push_back({kTile, uint32_t(10 + i), 1000 + i, i * 64, 0, {uint8_t(i), 7, 7}, {}});
    objs.push_back({kCharacter, 40, 2000, 300, 300, {1, 2, 3, 4}, {}});
    objs.push_back({kContainer, 50, 3000, 500, 100, {9}, {0, 0, 0, 0}});
    return objs;
}

inline std::vector<std::vector<uint8_t>> Records(const std::vector<SObj>& objs, int32_t version = 15)
{
    std::vector<std::vector<uint8_t>> out;
    for (const SObj& o : objs)
        out.push_back(Record(o, version));
    return out;
}

}  // namespace SectorImages
