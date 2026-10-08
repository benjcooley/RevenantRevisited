// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  gameoptions.cpp - the player's options ([Options] in Revenant.ini)   *
// *************************************************************************

#include "gameoptions.h"

#include "revenant.h"   // the options declared there (see gameoptions.h)
#include "revutils.h"   // INI layer
#include "sound.h"      // ApplyMusicVolume / ApplyEffectsVolume

// Retail .data defaults (OPTIONS.md §1).
int32_t MusicVolume      = 127;    // 0x005d7a9c
int32_t EffectsVolume    = 127;    // 0x005d7aa0
int32_t GammaLevel       = 3;      // 0x005d7a48
bool    RealTimeLight    = true;   // 0x005d7a18
bool    CombatFace       = true;   // 0x005d7a64
bool    NoCombatResults  = false;  // 0x00668194
bool    NoGameSpeedLimit = false;  // 0x006682a8

namespace {

constexpr const char* kOptionsSection = "Options";

}  // namespace

void ReadOptions()
{
    // Retail's order; each key's default is the global's current value. Keys
    // with no port owner (ZoomSpeed, Software3D, SquishyScroll, BeepOnChat,
    // ...) are not read; OPTIONS.md §9 lists them.
    INISetSection(kOptionsSection);
    DoubleTapTicks   = INIGetInt("DoubleTapTicks", DoubleTapTicks);
    ShowDialog       = INIGetYesNo("ShowDialog", ShowDialog);
    PlaySpeech       = INIGetYesNo("PlaySpeech", PlaySpeech);
    CombatFace       = INIGetYesNo("CombatFace", CombatFace);
    AutoBeginCombat  = INIGetYesNo("AutoCombat", AutoBeginCombat);
    ViolenceLevel    = INIGetInt("Violence", ViolenceLevel);
    GammaLevel       = INIGetInt("GammaLevel", GammaLevel);
    MusicVolume      = INIGetInt("MusicVolume", MusicVolume);
    EffectsVolume    = INIGetInt("EffectsVolume", EffectsVolume);
    EnhancedLighting = INIGetYesNo("EnhancedLighting", EnhancedLighting);
    RealTimeLight    = INIGetYesNo("RealTimeLight", RealTimeLight);
    NoCombatResults  = INIGetYesNo("NoCombatResults", NoCombatResults);

    // REVSYNC-DIVERGENCE: retail read EffectsVolume at each sample's start
    // and never applied MusicVolume at boot: the CD kept the OS mixer's
    // level, which outlived the process (OPTIONS.md §7.9). The port has no
    // such mixer, so the saved levels are applied here.
    ApplyMusicVolume(MusicVolume);
    ApplyEffectsVolume(EffectsVolume);
}

void SaveOptions()
{
    INISetSection(kOptionsSection);
    INISetInt("DoubleTapTicks", DoubleTapTicks);
    INISetYesNo("ShowDialog", ShowDialog);
    INISetYesNo("PlaySpeech", PlaySpeech);
    INISetYesNo("CombatFace", CombatFace);
    INISetYesNo("AutoCombat", AutoBeginCombat);
    INISetInt("Violence", ViolenceLevel);
    INISetInt("GammaLevel", GammaLevel);
    INISetInt("MusicVolume", MusicVolume);
    INISetInt("EffectsVolume", EffectsVolume);
    INISetYesNo("EnhancedLighting", EnhancedLighting);
    INISetYesNo("RealTimeLight", RealTimeLight);
    INISetYesNo("NoCombatResults", NoCombatResults);
}
