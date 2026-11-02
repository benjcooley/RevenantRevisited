// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       file.h - Common File IO                         *
// *************************************************************************

#pragma once

#include "engine/core/revengine.h"

//==============================================================================
//                           File.CPP Function Prototypes
//==============================================================================

void *LoadFile(const char *filepath, void *filebuf = nullptr, int32_t *filesize = nullptr);
bool FileExists(const char *filepath);
int32_t FileSize(const char *filepath);
FILE *TryOpen(const char *name, const char *mode);
bool TryDelete(const char *name);
bool TryRename(const char *oldname, const char *newname);

#include <cstdio>
#include <string>

// ---- The engine's file access -------------------------------------------
// Both are implemented by the game's VFS (game/app/revutils.cpp) until it
// moves into the engine: the engine declares what it needs, the game
// provides it.

enum class EOpenOrder : uint8_t
{
    PackFirst,
    LooseFirst,
};

// Open a FILE relative to the configured roots. Named to avoid the libc
// popen(3) shell-pipe function, which would otherwise win overload
// resolution on POSIX.
//   Reads:  for each root (save path, overlay, run path), try the mounted
//           pack whose directory holds the path and the loose file, in
//           `order`; then the legacy fallbacks.
//   Writes: the save path only; the caller creates any directories.
FILE *rev_fopen(const char *file, const char *flags, EOpenOrder order = EOpenOrder::PackFirst);

// Path of an engine asset shipped beside the binary (fonts, editor icons):
// "<assets>/<relpath>", or "" when no assets directory exists (logged once).
// Resolution: $REVENANT_ASSETS_PATH, <exe>/assets, <exe>/../Resources/assets,
// <repo-root>/assets.
[[nodiscard]] std::string rev_engine_asset(const char *relpath);
