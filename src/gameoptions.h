// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   gameoptions.h - the player's options ([Options] in Revenant.ini)    *
// *************************************************************************
//
// Retail keeps the player's options in globals, reads them from the
// [Options] section of Revenant.ini at boot and writes them back on the
// Options pane's OK and at shutdown, in its own keys and format (ints as
// "%d", flags as Yes/No). The port does the same through the INI layer in
// revutils, so the GOG install's Revenant.ini and a retail-written one load
// unchanged; docs/gameflow/forensics/OPTIONS.md.
//
// The options the port already had are declared in revenant.h
// (AutoBeginCombat, PlaySpeech, ShowDialog, ViolenceLevel, DoubleTapTicks,
// EnhancedLighting). The ones below came with the Options pane; their
// defaults are retail's .data values.

#pragma once

#include <cstdint>

extern int32_t MusicVolume;       // MusicVolume: music level; 0..0x60 is heard (ApplyMusicVolume)
extern int32_t EffectsVolume;     // EffectsVolume: sound effects level, 0..0x7f
extern int32_t GammaLevel;        // GammaLevel, 0..4: the map ambient offset (GammaAmbientOffset); no display ramp (OPTIONS.md §9)
extern bool    RealTimeLight;     // RealTimeLight: kept and saved; Classic renders the RealTimeLight=No image
extern bool    CombatFace;        // CombatFace: face the target while moving in combat (TCharacter::Go, ResolveCombat)
extern bool    NoCombatResults;   // NoCombatResults: kept and saved; combat results aren't printed yet
extern bool    NoGameSpeedLimit;  // "Limit Game Speed" unchecked; this session only, never saved;
                                  // the port's simulation always runs on the 24 Hz tick

// The ambient light GammaLevel adds to every map ambient: retail's
// TMapPane::SetAmbientLight (0x00453640) stores light + (GammaLevel * 5 - 10) * 2,
// floored at 0. Level 2 adds nothing, 3 adds 10, 4 adds 20. OPTIONS.md §7.11.
[[nodiscard]] constexpr int32_t GammaAmbientOffset(int32_t gamma_level)
{
    return (gamma_level * 5 - 10) * 2;
}

// REVSYNC: ReadOptions @ 0x00484ae0 -- the [Options] keys that have a port
// owner, each written back (a missing key gets its default), then the music
// and effects levels applied. Runs after GetINISettings and before the
// command line, as retail.
void ReadOptions();

// REVSYNC: SaveOptions @ 0x00484ed0 -- the same keys, from the globals.
// Keys the port has no owner for are left as the file holds them.
void SaveOptions();
