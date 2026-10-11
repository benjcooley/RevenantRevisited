// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uispellbooktest.h - --test=ui-spellbook: TSpellBookPane              *
// *************************************************************************
//
// Hosts the production spell book (TSpellBookPane, spellbook.h) on the test
// screen in the side panel's upper slot, for the demo player
// (uidemoplayer.h) and its sample spells. The test screen routes the pointer
// to it, and the player has control, so its circles, arrows and parchment
// work. --test=ui-hud hosts it the same way, shown while the side panel is
// open on the book tab.
//
// --test=ab-spellbook is one case of the book's retail A/B
// (tools/retail_ab/hud_ab.py spellbook), from --ab-case, fields split by ';':
//   spells=TAL,TAL,...  the known spells, talisman codes, in learn order
//   items=I@S|...       the player's carried items (uipanehost.h, SAbItem;
//                       "Spell Pouch(Law,Soul)" for a pouch of talismans)
//                       and their inventory slots
//   mana=N              the player's mana
//   scroll=N            the window's top, px down the rows (clamped)
//   pressed=R           the row whose circle is pressed (the host presses it)
// On the first frame at a tick boundary (run it with --snapstep=0.03125)
// the host stops the clock and composes the book once, over black,
// recording its text. --snap=<png> writes the frame (give it
// --snapwarmup=12); --ab-out=<json> writes what the book shows and its text
// draws.
//
// *************************************************************************

#pragma once

bool InitializeUISpellbookMode();
bool InitializeUISpellbookModeEmbedded();       // --test=ui-hud: shown with the side panel's book tab
void RenderUISpellbookMode();
void CloseUISpellbookMode();

bool InitializeABSpellBookMode();
void RenderABSpellBookMode();
void CloseABSpellBookMode();
