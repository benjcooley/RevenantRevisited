// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   sha256.h - SHA-256 (FIPS 180-4)                                      *
// *************************************************************************
//
// For the playtest recordings, which name the INI and the start slot they
// were made from by hash (docs/playtest/PROTOCOL.md §4, §6).
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>

class TSha256
{
  public:
    TSha256();

    void Update(const void* data, size_t size);
    void Update(const std::string& bytes) { Update(bytes.data(), bytes.size()); }
    // The digest as 64 lower-case hex digits. Ends the hash.
    [[nodiscard]] std::string HexDigest();

    // A file's bytes; empty when it can't be read.
    [[nodiscard]] static std::string OfFile(const std::filesystem::path& file);

  private:
    void Block(const uint8_t* block);

    std::array<uint32_t, 8> state{};
    std::array<uint8_t, 64> buffer{};
    size_t                  buffered = 0;
    uint64_t                length = 0;     // bytes hashed
};
