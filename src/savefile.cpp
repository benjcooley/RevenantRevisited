// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   savefile.cpp - the save file: one file per save slot                *
// *************************************************************************
//
// See savefile.h and docs/gameflow/SAVE_FORMAT.md.

#include "savefile.h"

#include "retailadler32.h"

#define XXH_INLINE_ALL
#include "xxhash.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <map>
#include <system_error>
#include <utility>

namespace fs = std::filesystem;

namespace SaveFile
{
namespace
{

// ---- Bytes ------------------------------------------------------------------

class TWriter
{
  public:
    void U16(uint16_t v)
    {
        bytes.push_back(uint8_t(v));
        bytes.push_back(uint8_t(v >> 8));
    }
    void U32(uint32_t v)
    {
        for (int i = 0; i < 4; ++i)
            bytes.push_back(uint8_t(v >> (8 * i)));
    }
    void U64(uint64_t v)
    {
        U32(uint32_t(v));
        U32(uint32_t(v >> 32));
    }
    void I32(int32_t v) { U32(uint32_t(v)); }
    void Bytes(const uint8_t* data, size_t size) { bytes.insert(bytes.end(), data, data + size); }
    void Bytes(const std::vector<uint8_t>& data) { Bytes(data.data(), data.size()); }

    // A chunk: its id, a size patched in by EndChunk, then what's written
    // between.
    size_t BeginChunk(TChunkId id)
    {
        U32(id);
        const size_t at = bytes.size();
        U32(0);
        return at;
    }
    void EndChunk(size_t at) { Patch32(at, uint32_t(bytes.size() - at - 4)); }
    void Chunk(TChunkId id, const std::vector<uint8_t>& data)
    {
        const size_t at = BeginChunk(id);
        Bytes(data);
        EndChunk(at);
    }
    void Patch32(size_t at, uint32_t v)
    {
        for (int i = 0; i < 4; ++i)
            bytes[at + size_t(i)] = uint8_t(v >> (8 * i));
    }

    std::vector<uint8_t> bytes;
};

class TReader
{
  public:
    TReader() = default;
    TReader(const uint8_t* data, size_t size) : at(data), end(data + size) {}

    [[nodiscard]] size_t Remaining() const { return size_t(end - at); }
    [[nodiscard]] bool U8(uint8_t& v)
    {
        if (Remaining() < 1)
            return false;
        v = *at++;
        return true;
    }
    [[nodiscard]] bool U16(uint16_t& v)
    {
        if (Remaining() < 2)
            return false;
        v = uint16_t(at[0] | at[1] << 8);
        at += 2;
        return true;
    }
    [[nodiscard]] bool U32(uint32_t& v)
    {
        if (Remaining() < 4)
            return false;
        v = uint32_t(at[0]) | uint32_t(at[1]) << 8 | uint32_t(at[2]) << 16 | uint32_t(at[3]) << 24;
        at += 4;
        return true;
    }
    [[nodiscard]] bool U64(uint64_t& v)
    {
        uint32_t lo = 0, hi = 0;
        if (!U32(lo) || !U32(hi))
            return false;
        v = uint64_t(lo) | uint64_t(hi) << 32;
        return true;
    }
    [[nodiscard]] bool I16(int16_t& v)
    {
        uint16_t u = 0;
        if (!U16(u))
            return false;
        v = int16_t(u);
        return true;
    }
    [[nodiscard]] bool I32(int32_t& v)
    {
        uint32_t u = 0;
        if (!U32(u))
            return false;
        v = int32_t(u);
        return true;
    }
    [[nodiscard]] bool Skip(size_t count)
    {
        if (Remaining() < count)
            return false;
        at += count;
        return true;
    }
    [[nodiscard]] bool Bytes(size_t count, std::vector<uint8_t>& out)
    {
        if (Remaining() < count)
            return false;
        out.assign(at, at + count);
        at += count;
        return true;
    }
    // The next chunk: its id and a reader over its data.
    [[nodiscard]] bool Chunk(TChunkId& id, TReader& data)
    {
        uint32_t size = 0;
        if (!U32(id) || !U32(size) || Remaining() < size)
            return false;
        data = TReader(at, size);
        at += size;
        return true;
    }
    [[nodiscard]] const uint8_t* Position() const { return at; }

  private:
    const uint8_t* at  = nullptr;
    const uint8_t* end = nullptr;
};

// ---- Sector images ----------------------------------------------------------

constexpr char     kSectorFcc[4]       = {'M', 'A', 'P', ' '};
constexpr int32_t  kStateHashVersion   = 14;      // the header carries a state hash from here (> 13)
constexpr int32_t  kBlockSizeVersion   = 4;       // records carry their size from here
constexpr int32_t  kObjVersionVersion  = 8;       // records start with the object's version from here
constexpr int32_t  kMaxSlots           = 65536;
constexpr int16_t  kClassPlayer        = 11;
constexpr int16_t  kClassCharacter     = 12;
constexpr uint32_t kImmobile           = 1u << 0;     // OF_IMMOBILE: no velocity saved
constexpr uint32_t kNonMap             = 1u << 19;    // OF_NONMAP: the level saved
constexpr uint32_t kZeroStateHash      = 0xf0f0f0f0;  // retail's stand-in for a hash of 0
constexpr uint64_t kGolden             = 0x9E3779B97F4A7C15ull;

// How a sector version lays out the header and each slot. Versions with the
// same framing differ only inside object bodies.
[[nodiscard]] int32_t Framing(int32_t version)
{
    return int32_t(version >= kObjVersionVersion) + int32_t(version >= kStateHashVersion);
}

[[nodiscard]] uint64_t HashOf(const std::vector<uint8_t>& bytes)
{
    return XXH64(bytes.data(), bytes.size(), 0);
}

// One slot of a sector image.
struct SSlot
{
    size_t   begin = 0, end = 0;          // its bytes in the image
    bool     empty = true;
    int16_t  objclass = -1;
    uint32_t objtype = 0;                 // the class's object type id
    size_t   sizesAt = 0;                 // where the record's block sizes are
    size_t   bodyBegin = 0;
    int16_t  blocksize = 0, invblocksize = 0;
    bool     hasMapIndex = false;
    int32_t  mapindex = 0;
};

struct SImage
{
    int32_t version = 0;
    std::vector<SSlot> slots;
};

// The object's map id from the start of its body, TObjectInstance::Save's
// fields (name, flags, position, velocity unless immobile, state, level when
// non-map, inventory slot and index, shadow, rotations, then the id).
bool ReadMapIndex(const uint8_t* body, size_t size, int32_t& mapindex)
{
    TReader r(body, size);
    uint8_t namelen = 0;
    uint32_t flags = 0;
    if (!r.U8(namelen) || !r.Skip(namelen) || !r.U32(flags) || !r.Skip(12))
        return false;
    if (!(flags & kImmobile) && !r.Skip(12))
        return false;
    if (!r.Skip(2) || ((flags & kNonMap) && !r.Skip(2)) || !r.Skip(2 + 2 + 4 + 3))
        return false;
    return r.I32(mapindex);
}

// Splits a sector image into its slots. False when it isn't one, or is of a
// version whose records can't be stepped over (before block sizes).
bool ParseImage(const std::vector<uint8_t>& bytes, SImage& image)
{
    image = {};
    TReader r(bytes.data(), bytes.size());
    uint32_t fcc = 0, statehash = 0;
    int32_t count = 0;
    if (bytes.size() < 12 || std::memcmp(bytes.data(), kSectorFcc, 4) != 0 || !r.U32(fcc) ||
        !r.I32(image.version) || image.version < kBlockSizeVersion)
        return false;
    if (image.version >= kStateHashVersion && !r.U32(statehash))
        return false;
    if (!r.I32(count) || count < 0 || count > kMaxSlots)
        return false;

    image.slots.reserve(size_t(count));
    for (int32_t i = 0; i < count; ++i)
    {
        SSlot& slot = image.slots.emplace_back();
        slot.begin = size_t(r.Position() - bytes.data());
        int16_t objversion = 0;
        if (image.version >= kObjVersionVersion && !r.I16(objversion))
            return false;
        if (objversion < 0)
        {
            slot.end = size_t(r.Position() - bytes.data());
            continue;
        }
        if (!r.I16(slot.objclass))
            return false;
        if (slot.objclass < 0)
        {
            slot.end = size_t(r.Position() - bytes.data());
            continue;
        }
        slot.sizesAt = size_t(r.Position() - bytes.data()) + 4;
        if (!r.U32(slot.objtype) || !r.I16(slot.blocksize) || slot.blocksize < 0)
            return false;
        if (image.version >= kStateHashVersion && (!r.I16(slot.invblocksize) || slot.invblocksize < 0 ||
                                                   slot.invblocksize > slot.blocksize))
            return false;
        slot.bodyBegin = size_t(r.Position() - bytes.data());
        if (!r.Skip(size_t(slot.blocksize)))
            return false;
        slot.end   = size_t(r.Position() - bytes.data());
        slot.empty = false;
        slot.hasMapIndex = ReadMapIndex(bytes.data() + slot.bodyBegin, size_t(slot.blocksize - slot.invblocksize),
                                        slot.mapindex);
    }
    return true;
}

bool SameBytes(const std::vector<uint8_t>& a, const SSlot& sa, const std::vector<uint8_t>& b, const SSlot& sb)
{
    return sa.end - sa.begin == sb.end - sb.begin &&
           std::memcmp(a.data() + sa.begin, b.data() + sb.begin, sa.end - sa.begin) == 0;
}

// Both slots hold the same object: class, type and map id.
bool SameObject(const SSlot& a, const SSlot& b)
{
    return a.objclass == b.objclass && a.objtype == b.objtype && a.hasMapIndex && b.hasMapIndex &&
           a.mapindex == b.mapindex;
}

std::vector<uint64_t> KeysOf(int32_t level, int32_t x, int32_t y, const std::vector<uint8_t>& bytes,
                             const SImage& image)
{
    const std::string name = SectorFileName(level, x, y);
    const uint64_t seed = XXH64(name.data(), name.size(), 0);
    std::vector<uint64_t> keys(image.slots.size(), 0);
    std::map<std::pair<uint64_t, size_t>, uint64_t> copies;    // (record hash, size) -> copies so far
    for (size_t i = 0; i < image.slots.size(); ++i)
    {
        const SSlot& slot = image.slots[i];
        if (slot.empty)
            continue;
        const uint8_t* record = bytes.data() + slot.begin;
        const size_t size = slot.end - slot.begin;
        uint64_t& copy = copies[{XXH64(record, size, 0), size}];
        keys[i] = XXH64(record, size, seed + copy * kGolden);
        ++copy;
    }
    return keys;
}

uint32_t StateHashOf(int32_t level, int32_t x, int32_t y, const std::vector<uint8_t>& bytes, const SImage& image)
{
    TRetailAdler32 hash;
    const int32_t header[4] = {level, x, y, int32_t(image.slots.size())};
    for (int32_t value : header)
        hash.Update(&value, sizeof(value));
    // Each character's record as SaveObject writes it without inventory: the
    // block size the body's alone, no inventory size, the body.
    for (const SSlot& slot : image.slots)
    {
        if (slot.empty || (slot.objclass != kClassCharacter && slot.objclass != kClassPlayer))
            continue;
        std::vector<uint8_t> record(bytes.begin() + ptrdiff_t(slot.begin), bytes.begin() + ptrdiff_t(slot.bodyBegin));
        const int16_t body = int16_t(slot.blocksize - slot.invblocksize);
        record[slot.sizesAt - slot.begin]     = uint8_t(body);
        record[slot.sizesAt - slot.begin + 1] = uint8_t(uint16_t(body) >> 8);
        record[slot.sizesAt - slot.begin + 2] = 0;
        record[slot.sizesAt - slot.begin + 3] = 0;
        record.insert(record.end(), bytes.begin() + ptrdiff_t(slot.bodyBegin),
                      bytes.begin() + ptrdiff_t(slot.bodyBegin) + body);
        hash.Update(record.data(), record.size());
    }
    return hash.Value() != 0 ? hash.Value() : kZeroStateHash;
}

SSectorDelta Whole(int32_t level, int32_t x, int32_t y, const std::vector<uint8_t>& image)
{
    SSectorDelta delta;
    delta.level = level;
    delta.x     = x;
    delta.y     = y;
    delta.whole = true;
    delta.image = image;
    return delta;
}

bool DecodeSectors(TReader& r, std::vector<SSectorDelta>& sectors, std::string& error)
{
    uint32_t count = 0;
    if (!r.U32(count))
    {
        error = "SECT: no count";
        return false;
    }
    sectors.clear();
    for (uint32_t i = 0; i < count; ++i)
    {
        SSectorDelta& s = sectors.emplace_back();
        uint8_t whole = 0;
        uint32_t size = 0;
        if (!r.I32(s.level) || !r.I32(s.x) || !r.I32(s.y) || !r.U8(whole))
        {
            error = "SECT: sector " + std::to_string(i) + " is cut short";
            return false;
        }
        s.whole = whole != 0;
        if (s.whole)
        {
            if (!r.U32(size) || !r.Bytes(size, s.image))
            {
                error = "SECT: sector " + std::to_string(i) + " is cut short";
                return false;
            }
            continue;
        }
        uint32_t changes = 0;
        if (!r.I32(s.version) || !r.U64(s.basehash) || !r.U32(changes))
        {
            error = "SECT: sector " + std::to_string(i) + " is cut short";
            return false;
        }
        for (uint32_t c = 0; c < changes; ++c)
        {
            SObjectChange& change = s.changes.emplace_back();
            uint8_t kind = 0;
            if (!r.U8(kind) || !r.U64(change.key) || !r.I16(change.objclass) || !r.U32(change.objtype) ||
                !r.U32(size) || !r.Bytes(size, change.record) ||
                kind < uint8_t(SObjectChange::EKind::Add) || kind > uint8_t(SObjectChange::EKind::Replace))
            {
                error = "SECT: sector " + std::to_string(i) + " change " + std::to_string(c) + " is damaged";
                return false;
            }
            change.kind = SObjectChange::EKind(kind);
        }
    }
    return true;
}

}  // namespace

// ---- Sector deltas ----------------------------------------------------------

std::vector<uint64_t> ObjectKeys(int32_t level, int32_t x, int32_t y, const std::vector<uint8_t>& image)
{
    SImage parsed;
    if (!ParseImage(image, parsed))
        return {};
    return KeysOf(level, x, y, image, parsed);
}

uint32_t SectorStateHash(int32_t level, int32_t x, int32_t y, const std::vector<uint8_t>& image)
{
    SImage parsed;
    return ParseImage(image, parsed) ? StateHashOf(level, x, y, image, parsed) : 0;
}

SSectorDelta MakeSectorDelta(int32_t level, int32_t x, int32_t y, const std::vector<uint8_t>* base,
                             const std::vector<uint8_t>& image)
{
    SImage was, now;
    if (!base || !ParseImage(*base, was) || !ParseImage(image, now) || Framing(was.version) != Framing(now.version))
        return Whole(level, x, y, image);

    SSectorDelta delta;
    delta.level    = level;
    delta.x        = x;
    delta.y        = y;
    delta.version  = now.version;
    delta.basehash = HashOf(*base);
    const std::vector<uint64_t> keys = KeysOf(level, x, y, *base, was);
    std::vector<SObjectChange> adds;
    auto record = [&](const SSlot& slot) {
        return std::vector<uint8_t>(image.begin() + ptrdiff_t(slot.begin), image.begin() + ptrdiff_t(slot.end));
    };
    auto change = [&](SObjectChange::EKind kind, size_t i) {
        SObjectChange c;
        c.kind     = kind;
        c.key      = keys[i];
        c.objclass = was.slots[i].objclass;
        c.objtype  = was.slots[i].objtype;
        return c;
    };
    const size_t slots = std::max(was.slots.size(), now.slots.size());
    for (size_t i = 0; i < slots; ++i)
    {
        const SSlot* before = i < was.slots.size() && !was.slots[i].empty ? &was.slots[i] : nullptr;
        const SSlot* after  = i < now.slots.size() && !now.slots[i].empty ? &now.slots[i] : nullptr;
        if (before && after && SameBytes(*base, *before, image, *after))
            continue;
        if (before && after && SameObject(*before, *after))
        {
            SObjectChange c = change(SObjectChange::EKind::Replace, i);
            c.record = record(*after);
            delta.changes.push_back(std::move(c));
            continue;
        }
        if (before)
            delta.changes.push_back(change(SObjectChange::EKind::Delete, i));
        if (after)
        {
            SObjectChange add;
            add.kind   = SObjectChange::EKind::Add;
            add.record = record(*after);
            adds.push_back(std::move(add));
        }
    }
    for (SObjectChange& add : adds)
        delta.changes.push_back(std::move(add));
    return delta;
}

bool ApplySectorDelta(const SSectorDelta& delta, const std::vector<uint8_t>* base, std::vector<uint8_t>& image,
                      SApplyResult& result)
{
    result = {};
    if (delta.whole)
    {
        image = delta.image;
        return true;
    }
    SImage was;
    if (!base || !ParseImage(*base, was))
    {
        result.failure = base ? "the module's sector is unreadable" : "the module has no such sector";
        return false;
    }
    if (Framing(was.version) != Framing(delta.version) ||
        (was.version != delta.version && HashOf(*base) != delta.basehash))
    {
        result.failure = "the module's sector is another version than the one they were made against";
        return false;
    }

    // Each base slot's bytes, as the changes leave them: the base's, a
    // replacement, or an empty slot.
    enum class EState : uint8_t { Base, Replaced, Emptied };
    struct SOut
    {
        EState state = EState::Base;
        const std::vector<uint8_t>* replacement = nullptr;
    };
    // MSVC: vector(n) cannot default-construct_at SOut here (C2672); the
    // copy-insert form compiles everywhere.
    std::vector<SOut> out(was.slots.size(), SOut{});
    const std::vector<uint64_t> keys = KeysOf(delta.level, delta.x, delta.y, *base, was);
    std::multimap<uint64_t, size_t> byKey;
    for (size_t i = 0; i < keys.size(); ++i)
        if (!was.slots[i].empty)
            byKey.emplace(keys[i], i);

    std::vector<const std::vector<uint8_t>*> adds;
    for (const SObjectChange& change : delta.changes)
    {
        if (change.kind == SObjectChange::EKind::Add)
        {
            adds.push_back(&change.record);
            ++result.applied;
            continue;
        }
        // The base object the change was made against: same key, class and
        // type, not yet claimed by another change.
        size_t found = was.slots.size();
        const auto [first, last] = byKey.equal_range(change.key);
        for (auto it = first; it != last; ++it)
        {
            const SSlot& slot = was.slots[it->second];
            if (out[it->second].state == EState::Base && slot.objclass == change.objclass &&
                slot.objtype == change.objtype)
            {
                found = it->second;
                break;
            }
        }
        if (found == was.slots.size())
        {
            ++result.missed;
            continue;
        }
        out[found].state       = change.kind == SObjectChange::EKind::Delete ? EState::Emptied : EState::Replaced;
        out[found].replacement = &change.record;
        ++result.applied;
    }

    TWriter w;
    w.Bytes(reinterpret_cast<const uint8_t*>(kSectorFcc), 4);
    w.I32(delta.version);
    const size_t hashAt = w.bytes.size();
    if (delta.version >= kStateHashVersion)
        w.U32(0);
    w.I32(int32_t(was.slots.size() + adds.size()));
    for (size_t i = 0; i < was.slots.size(); ++i)
    {
        const SSlot& slot = was.slots[i];
        switch (out[i].state)
        {
            case EState::Base:
                w.Bytes(base->data() + slot.begin, slot.end - slot.begin);
                break;
            case EState::Replaced:
                w.Bytes(*out[i].replacement);
                break;
            case EState::Emptied:
                w.U16(0xffff);          // an empty slot: its first field (object version, or class before 8) -1
                break;
        }
    }
    for (const std::vector<uint8_t>* add : adds)
        w.Bytes(*add);

    image = std::move(w.bytes);
    SImage rebuilt;
    if (!ParseImage(image, rebuilt))
    {
        result.failure = "a saved record is damaged";
        return false;
    }
    if (delta.version >= kStateHashVersion)
    {
        const uint32_t hash = StateHashOf(delta.level, delta.x, delta.y, image, rebuilt);
        for (int i = 0; i < 4; ++i)
            image[hashAt + size_t(i)] = uint8_t(hash >> (8 * i));
    }
    return true;
}

// ---- The file ---------------------------------------------------------------

std::vector<uint8_t> Encode(const SSaveFile& save)
{
    TWriter w;
    const size_t form = w.BeginChunk(kFormId);

    const size_t head = w.BeginChunk(kHeadId);
    w.U32(kFormatVersion);
    w.U32(0);                                   // flags
    w.EndChunk(head);

    w.Chunk(kGameId, save.game);
    if (!save.thumbnail.empty())
        w.Chunk(kThumbnailId, save.thumbnail);

    const size_t sect = w.BeginChunk(kSectorsId);
    w.U32(uint32_t(save.sectors.size()));
    for (const SSectorDelta& s : save.sectors)
    {
        w.I32(s.level);
        w.I32(s.x);
        w.I32(s.y);
        w.bytes.push_back(s.whole ? 1 : 0);
        if (s.whole)
        {
            w.U32(uint32_t(s.image.size()));
            w.Bytes(s.image);
            continue;
        }
        w.I32(s.version);
        w.U64(s.basehash);
        w.U32(uint32_t(s.changes.size()));
        for (const SObjectChange& c : s.changes)
        {
            w.bytes.push_back(uint8_t(c.kind));
            w.U64(c.key);
            w.U16(uint16_t(c.objclass));
            w.U32(c.objtype);
            w.U32(uint32_t(c.record.size()));
            w.Bytes(c.record);
        }
    }
    w.EndChunk(sect);

    for (const SChunk& c : save.other)
        w.Chunk(c.id, c.data);

    w.EndChunk(form);
    return std::move(w.bytes);
}

bool Decode(const std::vector<uint8_t>& bytes, SSaveFile& save, std::string& error)
{
    save = {};
    TReader file(bytes.data(), bytes.size());
    TChunkId id = 0;
    TReader form;
    if (!file.Chunk(id, form) || id != kFormId)
    {
        error = "not a save file";
        return false;
    }

    bool head = false, game = false;
    TReader chunk;
    while (form.Remaining() > 0)
    {
        if (!form.Chunk(id, chunk))
        {
            error = "a chunk runs past the end of the file";
            return false;
        }
        switch (id)
        {
            case kHeadId:
            {
                uint32_t version = 0;
                if (!chunk.U32(version))
                {
                    error = "HEAD: no version";
                    return false;
                }
                if (version > kFormatVersion)
                {
                    error = "format version " + std::to_string(version) + " is newer than this build's " +
                            std::to_string(kFormatVersion);
                    return false;
                }
                head = true;
                break;
            }
            case kGameId:
                game = chunk.Bytes(chunk.Remaining(), save.game);
                break;
            case kThumbnailId:
                (void)chunk.Bytes(chunk.Remaining(), save.thumbnail);
                break;
            case kSectorsId:
                if (!DecodeSectors(chunk, save.sectors, error))
                    return false;
                break;
            default:
            {
                SChunk& other = save.other.emplace_back();
                other.id = id;
                (void)chunk.Bytes(chunk.Remaining(), other.data);
                break;
            }
        }
    }
    if (!head || !game)
    {
        error = !head ? "no HEAD chunk" : "no GAME chunk";
        return false;
    }
    return true;
}

bool Write(const fs::path& file, SSaveFile save, std::string& error)
{
    std::sort(save.sectors.begin(), save.sectors.end());
    const std::vector<uint8_t> bytes = Encode(save);

    std::error_code ec;
    if (file.has_parent_path())
        fs::create_directories(file.parent_path(), ec);
    const fs::path partial = fs::path(file).concat(".partial");
    FILE* fp = std::fopen(partial.string().c_str(), "wb");
    bool ok = fp && std::fwrite(bytes.data(), 1, bytes.size(), fp) == bytes.size();
    if (fp)
        ok = (std::fclose(fp) == 0) && ok;
    if (ok)
    {
        fs::rename(partial, file, ec);
        ok = !ec;
    }
    if (!ok)
    {
        fs::remove(partial, ec);
        error = "can't write " + file.string();
        return false;
    }
    return true;
}

bool Read(const fs::path& file, SSaveFile& save, std::string& error)
{
    std::vector<uint8_t> bytes;
    FILE* fp = std::fopen(file.string().c_str(), "rb");
    if (!fp)
    {
        error = "can't open " + file.string();
        return false;
    }
    uint8_t chunk[16384];
    size_t got = 0;
    while ((got = std::fread(chunk, 1, sizeof(chunk), fp)) > 0)
        bytes.insert(bytes.end(), chunk, chunk + got);
    std::fclose(fp);
    if (!Decode(bytes, save, error))
    {
        error = file.string() + ": " + error;
        return false;
    }
    return true;
}

std::string SectorFileName(int32_t level, int32_t x, int32_t y)
{
    char name[48];
    std::snprintf(name, sizeof(name), "%d_%d_%d.DAT", level, x, y);
    return name;
}

bool ParseSectorFileName(const std::string& name, int32_t& level, int32_t& x, int32_t& y)
{
    char ext[8] = {};
    int consumed = 0;
    if (std::sscanf(name.c_str(), "%d_%d_%d.%7s%n", &level, &x, &y, ext, &consumed) != 4)
        return false;
    for (char& c : ext)
        c = char(c >= 'a' && c <= 'z' ? c - 'a' + 'A' : c);
    return std::strcmp(ext, "DAT") == 0 && size_t(consumed) == name.size();
}

}  // namespace SaveFile
