// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *      uitextbartest.h - --test=ui-textbar: the TTextBar pane           *
// *************************************************************************
//
// Hosts the production text bar (TextBar, textbar.h) on the test screen and
// plays a scripted feed into it through its public calls: messages (one of
// them two lines, one a typed Notice) and the map-loading bar
// (SetHealthDisplay, SetLevels, ClearHealthDisplay). The feed is test data
// in this host only. Lines stack, take the black shadow, and the older ones
// fade; a --filmstrip over ~14 s shows the whole run.
//
// With no play screen HUD up, the map view is the whole display, so the bar
// sits at the bottom of the screen, as wide as the screen less the side
// tabs.
//
// *************************************************************************

#pragma once

bool InitializeUITextBarMode();
void RenderUITextBarMode();
void CloseUITextBarMode();
