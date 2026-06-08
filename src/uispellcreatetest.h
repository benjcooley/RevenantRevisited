// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uispellcreatetest.h - --test=ui-spellcreate                          *
// *************************************************************************
//
// Isolated harness for TSpellPane (cls_0x5a5978), the right-sidebar lower
// "Spell" tab where the player composes spells from talismans. 188x174
// parchment chrome (spellpane.dat:spellconstr) + 5 buttons (Invoke long
// bar, Back, Up/Down arrows, Min toggle) + 12 talisman icons (SStars,
// SLaw, SLife, SSky, SChaos, SDeath, SSoul, SSun, SOcean, SMoon, SEarth,
// SWard — 32x32 BM_16BIT|BM_ALPHA). Built from
// docs/ui/forensics/SpellCreatePane_SPEC.md (clean-room; spec status =
// forensics-skeleton — §0-§3 confirmed, §4-§13 still TBD).
//
// *************************************************************************

#pragma once

#include <cstdint>

bool InitializeUISpellCreateMode();
void RenderUISpellCreateMode();
void RenderUISpellCreateModeEmbedded();
void SetUISpellCreateModeVisible(bool visible);
void CloseUISpellCreateMode();
// #10b/c/d — Mouse handler for talisman click + backspace + invoke.
void HandleMouseClickUISpellCreateMode(int32_t button, int32_t x, int32_t y);
