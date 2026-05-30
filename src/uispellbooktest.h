// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uispellbooktest.h - --test=ui-spellbook                              *
// *************************************************************************
//
// Isolated harness for SpellbookSidebarPane (cls_0x5a5ae8), the right-sidebar
// "Book" tab: a 188x306 parchment scroll (Scroll chrome from spellscroll.dat
// + tiled ScrollPaper) listing the player's known spells as a vertically
// scrollable column of ~91px-tall rows — per row: 40x40 spell-circle icon
// (SpellIcons.dat keyed by spell name) + cream spell name + 16px-pitch
// talisman recipe glyph row + description/skill block + "Skill: N" / "Mana: N"
// stat lines, with up/down scroll-arrow buttons at pane-local (169,150) /
// (169,174). Built from docs/ui/forensics/SpellbookPane_SPEC.md (clean-room).
//
// *************************************************************************

#pragma once

bool InitializeUISpellbookMode();
void RenderUISpellbookMode();
void CloseUISpellbookMode();
