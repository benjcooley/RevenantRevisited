// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   memfile.h - a read-only FILE over bytes in memory                   *
// *************************************************************************
//
// Pack entries reach the game's stdio readers (fread / fseek / ftell) as
// FILE handles over their extracted bytes. The handle owns the bytes;
// fclose frees them.
//
// Implementations:
//   src/platform/posix/memfile.cpp     (macOS: funopen; Linux: fopencookie)
//   src/platform/windows/memfile.cpp   (a delete-on-close temporary file)

#pragma once

#include <cstdint>
#include <cstdio>
#include <vector>

namespace rev_platform
{

// A FILE reading `bytes` from the start, or nullptr when one can't be made.
// Writes through it are unsupported.
FILE* OpenMemoryFile(std::vector<uint8_t> bytes);

} // namespace rev_platform
