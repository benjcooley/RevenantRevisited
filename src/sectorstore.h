// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   sectorstore.h - the sectors the game has changed                    *
// *************************************************************************
//
// A sector file (`<level>_<x>_<y>.DAT`) comes from the working set -- every
// sector the current game has written, held in memory -- or, when the
// working set has none, from the base map: the active module's Map/ through
// the resource layer. A save file holds the working set as each sector's
// object changes against the base map (SaveFile::SSectorDelta,
// docs/gameflow/SAVE_FORMAT.md); a retail save slot holds the sectors whole
// in its CurMap/. Retail kept the working set as files in a "curmap" folder;
// the port keeps no such folder. The install stays read-only and a new game
// is an empty working set.
//
// This is storage only, on the main thread. Which sectors are loaded in
// memory, and when they are written, belongs to TMapManager / TGameMap.
// See docs/gameflow/forensics/SAVE_GAME.md §2.
#pragma once

#include "savefile.h"

#include <cstdint>
#include <filesystem>
#include <utility>
#include <vector>

namespace SectorStore
{

// ---- What the sectors read and write ------------------------------------

// Sector file `filename`: the working set's copy if there is one, else the
// base map's. False if neither has it.
[[nodiscard]] bool Read(const char* filename, std::vector<uint8_t>& bytes);
[[nodiscard]] bool Exists(const char* filename);

// Puts sector file `filename` in the working set (a sector the same as the
// base map's leaves it).
void Write(const char* filename, const uint8_t* data, size_t size);

// Empties the working set: the world reverts to the base map.
void Clear();

// The working set's sectors on `level`, as (x, y).
[[nodiscard]] std::vector<std::pair<int32_t, int32_t>> SectorsOn(int32_t level);

// ---- The base map ---------------------------------------------------------

// <SavePath>/<BaseMapPath>map: the loose base map the editor publishes to.
// Reads of the base map check it before the module's packed Map/.
[[nodiscard]] std::filesystem::path BaseMapDir();

// The base map's copy of `filename`, never the working set's. False when the
// base map has none.
[[nodiscard]] bool ReadBase(const char* filename, std::vector<uint8_t>& bytes);

// ---- Saves ----------------------------------------------------------------

// The working set as a save file holds it, sorted: a sector holding the base
// map's objects as they are is left out.
[[nodiscard]] std::vector<SaveFile::SSectorDelta> Deltas();

// Replaces the working set with a save file's sectors, rebuilt against the
// base map. A change whose object the base no longer has is left out with a
// warning (that object is the base map's).
void SetDeltas(const std::vector<SaveFile::SSectorDelta>& deltas);

// Replaces the working set with the sector files in `dir` (a retail save
// slot's CurMap). Returns the number read.
int32_t ImportFrom(const std::filesystem::path& dir);

// Replaces the sector files in `dir` (created if needed) with the working
// set's: a retail slot's CurMap. Returns the number written.
int32_t ExportTo(const std::filesystem::path& dir);

// Writes the sectors `deltas` rebuild from the base map into `dir`, as
// ExportTo does, leaving the working set alone (a save file exported as a
// retail slot).
int32_t ExportDeltasTo(const std::vector<SaveFile::SSectorDelta>& deltas, const std::filesystem::path& dir);

// ---- The editor -----------------------------------------------------------

// Writes the working set's sectors into BaseMapDir() over the files there,
// keeping the sectors published before, and takes each sector written out of
// the working set: the base map now holds it. Saves made against the earlier
// base map still apply, object by object (SaveFile::ApplySectorDelta).
// Returns the number written.
int32_t Publish();

}  // namespace SectorStore
