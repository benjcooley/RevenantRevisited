// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *     packed.h - tagged byte-buffer codec (docs/vfx/PREFAB_SYSTEM.md    *
// *                §2.5). Bake a DEF once, read it fast forever.          *
// *************************************************************************
//
// A message is a run of fields. Each field is a varint tag
// `(id << 2) | wire` followed by its payload:
//
//   wire 0  Varint   bool / int (zigzag) / enum / uid and ref (64-bit)
//   wire 1  Fixed32  float
//   wire 2  Bytes    varint length + bytes: string, vec3 (12 B),
//                    color (16 B), nested message
//   wire 3  reserved
//
// An unknown id is skipped by its wire kind without consulting any table;
// a missing id keeps the field's declared default. Ids are the only
// version. The reader is a cursor: Next() hands back id, wire, a pointer
// to the payload and its size.
//
// *************************************************************************

#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

enum class EPackedWire : uint8_t
{
    Varint  = 0,
    Fixed32 = 1,
    Bytes   = 2,
};

struct SPackedField
{
    uint16_t       id    = 0;
    EPackedWire    wire  = EPackedWire::Varint;
    const uint8_t* value = nullptr;   // payload start (for Varint: the varint bytes)
    uint32_t       size  = 0;         // payload bytes (Varint: encoded length)
};

class TPackedWriter
{
  public:
    void Bool(uint16_t id, bool v)                 { Varint(id, v ? 1u : 0u); }
    void Int(uint16_t id, int32_t v)               { Varint(id, Zigzag(v)); }
    void Enum(uint16_t id, int32_t v)              { Varint(id, Zigzag(v)); }
    void Uid(uint16_t id, uint64_t v)              { Tag(id, EPackedWire::Varint); PutVarint64(v); }
    void Float(uint16_t id, float v);
    void Vec3(uint16_t id, const float v[3]);
    void Color(uint16_t id, const float c[4]);
    void String(uint16_t id, std::string_view s)   { Bytes(id, s.data(), uint32_t(s.size())); }
    void Bytes(uint16_t id, const void* data, uint32_t size);
    // Nested message: append another writer's buffer as a Bytes field.
    void Message(uint16_t id, const TPackedWriter& nested) { Bytes(id, nested.buf_.data(), uint32_t(nested.buf_.size())); }

    [[nodiscard]] const std::vector<uint8_t>& Buffer() const { return buf_; }
    [[nodiscard]] std::vector<uint8_t>& Buffer() { return buf_; }
    void Clear() { buf_.clear(); }

    [[nodiscard]] static uint32_t Zigzag(int32_t v) { return (uint32_t(v) << 1) ^ uint32_t(v >> 31); }
    [[nodiscard]] static int32_t  Unzigzag(uint32_t u) { return int32_t(u >> 1) ^ -int32_t(u & 1u); }

  private:
    void Tag(uint16_t id, EPackedWire wire) { PutVarint((uint32_t(id) << 2) | uint32_t(wire)); }
    void Varint(uint16_t id, uint32_t v)    { Tag(id, EPackedWire::Varint); PutVarint(v); }
    void PutVarint(uint32_t v);
    void PutVarint64(uint64_t v);

    std::vector<uint8_t> buf_;
};

class TPackedReader
{
  public:
    TPackedReader() = default;
    TPackedReader(const uint8_t* data, uint32_t size) : cur_(data), end_(data + size) {}
    explicit TPackedReader(const std::vector<uint8_t>& buf)
        : cur_(buf.data()), end_(buf.data() + buf.size()) {}
    // Sub-reader over a Bytes field's payload (nested message).
    explicit TPackedReader(const SPackedField& f) : cur_(f.value), end_(f.value + f.size) {}

    // Advances to the next field. Returns false at end of message or on a
    // malformed tag (truncated varint / payload); AtEnd() tells which.
    bool Next(SPackedField& f);
    [[nodiscard]] bool AtEnd() const { return cur_ >= end_; }

    // Typed payload reads. They check the wire kind and size; a mismatch
    // returns the fallback so a retired-and-reused id can never crash a
    // decoder (ids are not reused, but the check is one compare).
    [[nodiscard]] static bool    Bool(const SPackedField& f, bool fallback = false);
    [[nodiscard]] static int32_t Int(const SPackedField& f, int32_t fallback = 0);
    [[nodiscard]] static uint64_t Uid(const SPackedField& f, uint64_t fallback = 0);
    [[nodiscard]] static float   Float(const SPackedField& f, float fallback = 0.0f);
    static bool Vec3(const SPackedField& f, float out[3]);
    static bool Color(const SPackedField& f, float out[4]);
    [[nodiscard]] static std::string_view String(const SPackedField& f);

  private:
    bool GetVarint(uint32_t& out);
    bool GetVarint64(uint64_t& out);

    const uint8_t* cur_ = nullptr;
    const uint8_t* end_ = nullptr;
};
