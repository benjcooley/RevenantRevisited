// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uidemoplayer.h - demo subject for the --test=ui-* HUD modes          *
// *************************************************************************
//
// The HUD panes read the main player (`Player`, retail DAT_00667fcc). The
// --test=ui-* modes have no game, so this fixture builds a real TPlayer
// carrying the sample content the panes were reconstructed against and
// installs it through TPlayerManager, plus the opponent the status bar's
// target card shows. testmodes.cpp installs it around the modes whose
// panes read a player; the pane code never refers to it.
//
// See docs/ui/HUD_LIVE_BINDING.md.
//
// *************************************************************************

#pragma once

#include <cstdint>

class TObjectInstance;
class TPlayer;

namespace UIDemoPlayer
{

// Build the demo player and its opponent and make the player the main
// player. Idempotent.
bool Install();

// Demo-only motion, once per frame: the opponent is engaged and released
// on a cycle and its bars sweep, so the target card fades and drains.
void Pulse();

// Release the main player and delete everything Install built.
void Remove();

// The opponent Install built (fighting the player), or null.
TPlayer* Opponent();

// For the A/B hosts that lay out a case's inventory:
// A new item of the named type (whatever its class) in `owner`'s inventory
// at `slot` (a free slot when negative), `amount` of it, never stacked with
// another. Null if there is no such type or the slot is taken.
TObjectInstance* AddItem(TObjectInstance* owner, const char* name, int32_t slot, int32_t amount = 1);
// Deletes the items in `owner`'s inventory slots first..last.
void RemoveItems(TObjectInstance* owner, int32_t first, int32_t last);

} // namespace UIDemoPlayer
