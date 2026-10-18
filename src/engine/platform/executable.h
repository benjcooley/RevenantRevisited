// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   executable.h - where the running program lives in files and memory  *
// *************************************************************************
//
// The engine finds its own assets and a player's install relative to the
// program file (docs/DATA_LAYOUT.md §1); traces name code addresses
// relative to the image (combattrace.cpp).
//
// Implementations:
//   src/platform/posix/executable.cpp     (macOS, Linux)
//   src/platform/windows/executable.cpp

#pragma once

#include <cstdint>
#include <filesystem>

namespace rev_platform
{

// The directory holding the running executable, or an empty path when the
// OS won't say.
std::filesystem::path ExecutableDir();

// `address` as an offset into the loaded image (executable or library)
// holding it: the same from run to run despite address randomization.
// The address itself when no image claims it.
uintptr_t ImageOffset(const void* address);

} // namespace rev_platform
