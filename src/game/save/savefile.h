// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   savefile.h - the save file: one file per save slot                  *
// *************************************************************************
//
// A save slot as one file (`<SaveGamePath>Single/<name>.rvsave`) instead of
// retail's folder of game.sav, ss.bmp and one CurMap/*.DAT per changed
// sector. IFF-style chunks: a 4-byte id, a 4-byte little-endian size, then
// that many bytes. Readers skip chunks they don't know, so blocks can be
// added without breaking older readers. The game state is retail's game.sav
// unchanged; each changed sector is held as the changes to its objects --
// added, deleted, replaced -- against the module's own copy of the sector
// (the base). A changed object is found again by a key made from its sector
// and its record's bytes in the base, so when the module's sector changes
// under a save, the save's changes to the objects that are still there
// still apply. Format: docs/gameflow/SAVE_FORMAT.md.
//
// This is the file format and the sector-image arithmetic only, free of the
// engine: TSaveGame fills and reads the file, SectorStore makes and applies
// the deltas against the base map.
#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace SaveFile
{

// A chunk id: four characters, in file order ('R','V','S','V').
using TChunkId = uint32_t;
[[nodiscard]] constexpr TChunkId MakeId(char a, char b, char c, char d)
{
    return uint32_t(uint8_t(a)) | uint32_t(uint8_t(b)) << 8 | uint32_t(uint8_t(c)) << 16 | uint32_t(uint8_t(d)) << 24;
}

inline constexpr TChunkId kFormId      = MakeId('R', 'V', 'S', 'V');   // the file: every other chunk inside it
inline constexpr TChunkId kHeadId      = MakeId('H', 'E', 'A', 'D');   // format version
inline constexpr TChunkId kGameId      = MakeId('G', 'A', 'M', 'E');   // retail's game.sav, as is
inline constexpr TChunkId kThumbnailId = MakeId('T', 'H', 'U', 'M');   // the thumbnail, a PNG file
inline constexpr TChunkId kSectorsId   = MakeId('S', 'E', 'C', 'T');   // the changed sectors

inline constexpr uint32_t kFormatVersion = 1;
inline constexpr const char* kExtension = ".rvsave";

// ---- Sector images ----------------------------------------------------------
// A sector file (`<level>_<x>_<y>.DAT`, recon SECTOR_FILE_FORMAT.md): "MAP ",
// its version, a state hash (version 14 on), the slot count, then each slot:
// an object record, or an empty slot.

// One object change.
struct SObjectChange
{
    enum class EKind : uint8_t { Add = 1, Delete = 2, Replace = 3 };

    EKind    kind     = EKind::Add;
    uint64_t key      = 0;        // Delete / Replace: the base object's key
    int16_t  objclass = -1;       // the base object's class and type (Delete / Replace),
    uint32_t objtype  = 0;        //   for the match and the log
    std::vector<uint8_t> record;  // Add / Replace: the object's record
};

// A changed sector: the changes to its objects against the base, or, where
// they can't be (the module has no such sector, or one whose slots are laid
// out another way), the whole sector.
struct SSectorDelta
{
    int32_t level = 0, x = 0, y = 0;
    bool    whole = false;
    std::vector<uint8_t>       image;     // when whole
    std::vector<SObjectChange> changes;   // otherwise, with
    int32_t  version  = 0;                //   the sector version the records are written in
    uint64_t basehash = 0;                //   and XXH64 of the base they were made against

    [[nodiscard]] bool operator<(const SSectorDelta& other) const
    {
        if (level != other.level) return level < other.level;
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};

// The changes that turn `base` (null: the module has no such sector) into
// `image`. A slot holding the same object in both (class, type and map id)
// with other bytes is a Replace; an object only in `base` a Delete; one only
// in `image`, or a slot whose object is another, an Add. None: `image` is
// `base` (perhaps saved at a later sector version).
[[nodiscard]] SSectorDelta MakeSectorDelta(int32_t level, int32_t x, int32_t y,
                                           const std::vector<uint8_t>* base,
                                           const std::vector<uint8_t>& image);

struct SApplyResult
{
    int32_t     applied = 0;
    int32_t     missed  = 0;      // Delete / Replace whose object isn't in the base
    const char* failure = nullptr;  // why, when ApplySectorDelta can't apply the delta
};
// Rebuilds the saved sector from `delta` and the module's copy `base` (null:
// none): its objects in place, a deleted one's slot left empty, the added
// ones after, at the delta's sector version. A change whose object isn't in
// the base any more is left out (counted in `missed`). False when the delta
// can't be applied at all: no base, a base that isn't a sector image, or one
// whose slots are laid out another way. A base of another sector version
// than the delta's must be the base the delta was made against: the base
// records it keeps were the delta version's bytes only there (an object's
// bytes can differ between versions: TMonsterGen's from 15).
[[nodiscard]] bool ApplySectorDelta(const SSectorDelta& delta, const std::vector<uint8_t>* base,
                                    std::vector<uint8_t>& image, SApplyResult& result);

// The key of each object in a sector image, in slot order (0 for an empty
// slot): XXH64 of its record, seeded by the sector's file name and the
// record's copy number among identical ones in the image.
[[nodiscard]] std::vector<uint64_t> ObjectKeys(int32_t level, int32_t x, int32_t y,
                                               const std::vector<uint8_t>& image);

// The state hash a sector image's header carries (retail 0x00499e90,
// SAVE_GAME.md §11.6), from its records: its coordinates and slot count,
// then each character's record without its inventory. Retail also hashed
// the empty record a player in the sector saves as; an image doesn't say
// which empty slots those were.
[[nodiscard]] uint32_t SectorStateHash(int32_t level, int32_t x, int32_t y, const std::vector<uint8_t>& image);

// ---- The file ------------------------------------------------------------

// A chunk the reader doesn't know, kept so a rewrite (TSaveGame's late
// thumbnail) carries it over.
struct SChunk
{
    TChunkId id = 0;
    std::vector<uint8_t> data;
};

struct SSaveFile
{
    std::vector<uint8_t>      game;        // GAME: the save in retail's game.sav format
    std::vector<uint8_t>      thumbnail;   // THUM: a PNG file; empty: none
    std::vector<SSectorDelta> sectors;     // SECT: sorted, one per sector
    std::vector<SChunk>       other;       // chunks this version doesn't know
};

// The file's bytes. Sorts nothing: `save.sectors` must be sorted.
[[nodiscard]] std::vector<uint8_t> Encode(const SSaveFile& save);
// False, with `error`, when `bytes` isn't a save file this version reads.
[[nodiscard]] bool Decode(const std::vector<uint8_t>& bytes, SSaveFile& save, std::string& error);

// Writes beside `file` and renames over it, so a failed write leaves the
// previous save. Sorts `save.sectors`.
bool Write(const std::filesystem::path& file, SSaveFile save, std::string& error);
[[nodiscard]] bool Read(const std::filesystem::path& file, SSaveFile& save, std::string& error);

// The sector file name `<level>_<x>_<y>.DAT`, and back.
[[nodiscard]] std::string SectorFileName(int32_t level, int32_t x, int32_t y);
[[nodiscard]] bool ParseSectorFileName(const std::string& name, int32_t& level, int32_t& x, int32_t& y);

}  // namespace SaveFile
