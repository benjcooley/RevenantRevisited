// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uisidetabstest.h - --test=ui-sidetabs                                *
// *************************************************************************
//
// Isolated harness for TSideTabsPane: the six round sidebar buttons modelled
// as two independent 3-way region selectors (upper = Book/Stats/Equip,
// lower = Spell/Inv/Map). Built from TSideTabsPane_SPEC.md using the real
// retail strips Up/Down/Select (52x232) from sidebartabsnotex.dat.
//
// *************************************************************************

#pragma once

bool InitializeUISideTabsMode();
void RenderUISideTabsMode();
void CloseUISideTabsMode();
