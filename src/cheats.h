// *************************************************************************
// *                         Revenant Revisited                            *
// *               cheats.h - The cheat words of the text bar              *
// *************************************************************************

#pragma once

// REVSYNC: the cheat words TTextBar::SubmitInput (0x0054d700) recognises in
// single player, typed at the text bar's prompt (docs/ui/forensics/
// TTextBar_SPEC.md §10). The toggles themselves live with what they gate:
// CheatAlreadyDead, CheatNahkranoth and NoAI (revenant.h), MagicCheat
// (spell.h).

enum class ECheatResult
{
    Unknown,    // not a cheat word: retail says and plays nothing
    Enabled,    // "cheatenabled"
    Disabled,   // "cheatdisabled"
};

// Applies the cheat `word` names, compared ignoring case.
[[nodiscard]] ECheatResult ApplyCheatWord(const char* word);

extern bool CheatLookUnderTheHood;  // retail 0x0066812c: "lookunderthehood", and "debug" sets it too
extern bool CheatDebug;             // retail 0x00668130: "debug", retail's developer hotkeys
                                    // (DebugOverlay_SPEC §5); nothing in the port reads either yet
