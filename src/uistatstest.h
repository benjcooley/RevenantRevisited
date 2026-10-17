// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uistatstest.h - --test=ui-stats: TStatPane                           *
// *************************************************************************
//
// Hosts the production stat pane (TStatPane, statpane.h) on the test screen
// in the side panel's upper slot, for the demo player (uidemoplayer.h). The
// test screen routes the pointer to it, and the player has control, so its
// page and train hot spots work. --test=ui-hud hosts it the same way, shown
// while the side panel is open on the stats tab.
//
// --test=ab-statpane is one case of the pane's retail A/B
// (tools/retail_ab/hud_ab.py statpane), from --ab-case, fields split by ';':
//   stats=Name:N,...   the player's object stats by name (OBJSTATS: Level,
//                      Exp, Strn, Sword, SwordExp, ...)
//   levelup=A,B,L      the level-up picks (0..5: Strn..Luck) and the slot
//                      written last
//   format=page1|page2 the format shown
//   click=X,Y|...      pane points clicked, in order, after the pane is up
//                      (the hot spots: "page2" at 103,115, train at 50,126+12n)
// On the first frame at a tick boundary (run it with --snapstep=0.03125)
// the host stops the clock and composes the pane once, over black,
// recording its text. --snap=<png> writes the frame (give it
// --snapwarmup=12); --ab-out=<json> writes what the pane shows -- every
// object stat by name and the figures its fields read (TCharacter
// getters by retail vtable slot), which the retail side fakes -- and its
// text draws.
//
// *************************************************************************

#pragma once

bool InitializeUIStatsMode();
bool InitializeUIStatsModeEmbedded();           // --test=ui-hud: shown with the side panel's stats tab
void RenderUIStatsMode();
void CloseUIStatsMode();

bool InitializeABStatPaneMode();
void RenderABStatPaneMode();
void CloseABStatPaneMode();
