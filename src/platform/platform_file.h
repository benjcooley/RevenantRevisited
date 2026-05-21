// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *        platform_file.h - Platform file/stream helpers (agnostic)      *
// *************************************************************************
//
// Platform-agnostic interface; the per-OS mechanics live in
// platform/platform_file.cpp. Engine code in src/ includes this and stays
// free of OS headers and #ifdefs.
//
// *************************************************************************

#pragma once

#include <cstdint>
#include <cstdio>
#include <vector>

namespace rev_platform {

// Wrap an in-memory byte buffer as a read-only, seekable stdio stream.
// Takes ownership of `bytes`. Returns nullptr on failure; the caller closes
// the result with fclose(). The backend is chosen per-OS: funopen() on
// BSD/macOS (zero-copy over the buffer), and a self-deleting temp file on
// Windows / Linux (which lack funopen()).
FILE* OpenBytesAsReadStream(std::vector<uint8_t> bytes);

} // namespace rev_platform
