// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uimaptest.h - --test=ui-map: TAutoMap                                *
// *************************************************************************
//
// Hosts the production automap (TAutoMap, automap.h) on the test screen in
// the side panel's lower slot. The test screen routes the pointer to it, so
// "+" and "-" zoom while held and dragging inside the ring scrolls; with no
// level loaded the map inside is the empty, black mosaic. --test=ui-hud
// hosts it the same way beside the HUD harness's upper panes, shown while
// the side panel is open on the map tab.
//
// --test=ab-automap is one case of the automap's retail A/B
// (tools/retail_ab/hud_ab.py automap), from --ab-case, fields split by ';':
//   plus=0|1    "+" held
//   minus=0|1   "-" held
// The host presses them as the pointer would and, with the clock stopped on
// a tick boundary, composes the pane once over black without pulsing it (so
// the zoom stays at 100). --snap=<png> writes the frame (give it
// --snapwarmup=12); --ab-out=<json> writes what the pane shows.
//
// *************************************************************************

#pragma once

bool InitializeUIMapMode();
bool InitializeUIMapModeEmbedded();     // --test=ui-hud: shown with the side panel's map tab
void RenderUIMapMode();
void CloseUIMapMode();

bool InitializeABAutoMapMode();
void RenderABAutoMapMode();
void CloseABAutoMapMode();
