// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uibottombartest.h - --test=ui-bottombar: TBottomBarPane              *
// *************************************************************************
//
// Hosts the production bottom bar (TBottomBarPane, bottombar.h) -- the bar,
// the quick-spell rings and the potion shelf -- on the test screen along the
// bottom of the display, as wide as the map view the HUD state leaves (the
// display, less the side panel while it is open), shown while the HUD's
// lower panel is. The panes read the demo player the test modes install
// (uidemoplayer.h): its quick spells and its belt. --test=ui-hud hosts it
// the same way beside the HUD harness's other panes.
//
// --test=ab-bottombar is one case of the bar's retail A/B
// (tools/retail_ab/hud_ab.py bottombar), from --ab-case, fields split by ';':
//   width=W           the bar's width (452: the side panel open; 640: shut)
//   spells=A,B,C,D    the talismans of quick spells 1-4 (empty: none)
//   castable=A,B      quick spells the player holds the talismans for (the
//                     host puts them in a Spell Pouch); the rest are grey
//   belt=I|I|...      belt slots 0x10b + n: "-" empty, else a type name,
//                     "*N" for an amount, "(I,I,...)" for a pouch's slots
//                     0, 1, ...
//   frame=N           the game frame, which picks an animated icon's frame
// On the first frame at a tick boundary (run it with --snapstep=0.03125)
// the host stops the clock and pulses the bar once, so the frame shows the
// bar as that pulse leaves it, over black. --snap=<png> writes the frame
// (give it --snapwarmup=12); --ab-out=<json> writes what the rings and the
// boxes show.
//
// *************************************************************************

#pragma once

bool InitializeUIBottomBarMode();
bool InitializeUIBottomBarModeEmbedded();   // --test=ui-hud: no backdrop of its own
void RenderUIBottomBarMode();
void CloseUIBottomBarMode();

bool InitializeABBottomBarMode();
void RenderABBottomBarMode();
void CloseABBottomBarMode();
