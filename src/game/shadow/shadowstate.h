// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *        shadowstate.h - Locke as a save should keep him, for checking    *
// *************************************************************************
//
// The save round trip (docs/shadow/ARCHITECTURE.md §10). At each waypoint
// save the run records Locke's whole state; a fresh process loads the slot
// (`--shadow-verify=<file>`), lets the game settle and records it again,
// and tools/shadow/shadow.py --check compares the two field by field. A
// difference is a save or load that lost something.

#pragma once

#include <string>

class TPlayer;

// Locke's state as one JSON object: where he is, his vitals, level and
// experience, attributes and skills (as kept, and as modified, with the
// stat effects in force), skill experience, level-up picks,
// gold, where each thing he carries is (the pack, a bag, an equipment slot,
// a belt box), the spells he knows, his quick spells, and the story
// variables.
[[nodiscard]] std::string ShadowStateJson(TPlayer& locke);
