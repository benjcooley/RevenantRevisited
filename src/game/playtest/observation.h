// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   observation.h - what a playtest director sees of the game           *
// *************************************************************************
//
// The playtest's view of the live game, read from the objects themselves:
// the check lines a recording carries (docs/playtest/PROTOCOL.md §3), the
// observation the control channel's `observe` answers (§7), and the screen
// conversions `project` and `pick` use, which are the map pane's own.
// Read-only: nothing here changes the game or draws a random number.
#pragma once

#include <cstdint>
#include <string>

class JsonOut;
class TObjectInstance;
struct S3DPoint;

namespace Observation
{

// The screen showing, by the protocol's names: title, loading, play, menu,
// dialog, death, movie, other.
[[nodiscard]] const char* ScreenName();

// A check line's fields as a JSON object. `draws` counts from `drawsBase`,
// RandomDraws() when the session started.
[[nodiscard]] std::string CheckFields(uint64_t drawsBase);

// The members of the observation (§7), into an open object. `objects` is the
// radius to list objects in; `walkgrid` a radius for the walk grid, or
// negative for none. The text-bar messages are those since the last call.
void Write(JsonOut& out, int32_t objects, int32_t walkgrid);

// The protocol's id for an object: its map index, but 0 for the main player
// (an object that has map index 0 takes the player's).
[[nodiscard]] int32_t ObjectId(TObjectInstance* inst);

// A world point's game-screen pixel, as the map view drew it; false off the
// play field.
[[nodiscard]] bool Project(const S3DPoint& world, int32_t& x, int32_t& y);

// The map point under a game-screen pixel by the right-button walk's
// conversion (Locke's height + 50), and the object the map pane picks there
// (-1: none). False without a player or a map view.
[[nodiscard]] bool Pick(int32_t x, int32_t y, S3DPoint& pos, int32_t& object);

} // namespace Observation
