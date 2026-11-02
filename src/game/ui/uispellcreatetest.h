// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uispellcreatetest.h - --test=ui-spellcreate: TSpellPane              *
// *************************************************************************
//
// Hosts the production spell pane (TSpellPane, spellpane.h) on the test
// screen in the side panel's lower slot, for the demo player
// (uidemoplayer.h) with a spell pouch of talismans. The test screen routes
// the pointer to it, and the player has control, so its talismans and
// buttons work. --test=ui-hud hosts it the same way beside the HUD
// harness's other side panes, shown while the side panel is open on the
// spell tab.
//
// --test=ab-spellpane is one case of the spell pane's retail A/B
// (tools/retail_ab/hud_ab.py spellpane), from --ab-case, fields split by ';':
//   items=I@S|I@S|...  the player's carried items: an item (uipanehost.h,
//                      SAbItem; "Spell Pouch(Sun,Moon)" for a pouch of
//                      talismans) and its inventory slot
//   spell=TAL          the construct spell, talisman codes
//   names=0|1          the names toggle
//   first=N            the first talisman listed with names
//   pressed=T          the talisman type pressed (the host presses it)
//   invoke=0|1         the "spell" button held down
// On the first frame at a tick boundary (run it with --snapstep=0.03125)
// the host stops the clock and pulses the pane once, over black.
// --snap=<png> writes the frame (give it --snapwarmup=12); --ab-out=<json>
// writes what the pane shows.
//
// *************************************************************************

#pragma once

bool InitializeUISpellCreateMode();
bool InitializeUISpellCreateModeEmbedded();     // --test=ui-hud: shown with the side panel's spell tab
void RenderUISpellCreateMode();
void CloseUISpellCreateMode();

bool InitializeABSpellPaneMode();
void RenderABSpellPaneMode();
void CloseABSpellPaneMode();
