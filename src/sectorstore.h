// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   sectorstore.h - where sector files live on disk                     *
// *************************************************************************
//
// A sector file (`<level>_<x>_<y>.DAT`) is read from the working set — every
// sector the current game has written, retail's "curmap" under SavePath — or,
// when the working set has no copy, from the base map: the active module's
// Map/ through the resource layer. Sectors are written only to the working
// set, so the install stays read-only and a new game is simply an empty
// working set. A save slot keeps a frozen copy of the working set
// (<slot>/CurMap); loading the slot imports it back.
//
// This is file storage only. Which sectors are loaded in memory, and when
// they are written, belongs to TMapManager / TGameMap.
// See docs/gameflow/forensics/SAVE_GAME.md §2.
#pragma once

#include <cstdint>
#include <cstdio>
#include <filesystem>

namespace SectorStore
{

// <SavePath>/<CurMapPath>curmap. Created by the first write.
[[nodiscard]] std::filesystem::path WorkingSetDir();

// <SavePath>/<BaseMapPath>map: the loose base map the editor publishes to.
// Reads of the base map check it before the module's packed Map/.
[[nodiscard]] std::filesystem::path BaseMapDir();

// Opens sector file `filename` for reading: the working set's copy if there
// is one, otherwise the base map's. Null if neither has it.
[[nodiscard]] FILE* OpenForRead(const char* filename);

// Opens the working set's copy of `filename` for writing. Null on failure.
[[nodiscard]] FILE* OpenForWrite(const char* filename);

// Deletes every sector file in the working set.
void Clear();

// Replaces the working set with the sector files in `dir` (a save slot's
// CurMap). Returns the number of files copied.
int32_t ImportFrom(const std::filesystem::path& dir);

// Replaces the sector files in `dir` (created if needed) with the working
// set. Returns the number of files copied.
int32_t ExportTo(const std::filesystem::path& dir);

}  // namespace SectorStore
