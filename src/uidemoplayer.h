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

} // namespace UIDemoPlayer
