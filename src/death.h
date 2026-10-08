// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *            death.h - TDeathScreen and TDeathPane (game over)          *
// *************************************************************************
//
// Retail shows a separate "game over" screen when Locke dies: TDeathScreen
// (cls_0x5b9374, Initialize 0x005338a0) hosting TDeathPane (cls_0x5b93c4,
// Initialize 0x005339b0) - the death.dat "background" with Restart / Load /
// Exit buttons and a random death voice. Reference:
// docs/ui/forensics/DeathPane_SPEC.md.
#pragma once

#include "cursor.h"     // TCursorHud
#include "defpane.h"
#include "screen.h"

class TDeathPane : public TDefPane
{
  public:
    // REVSYNC: TDeathPane::Initialize @ 0x005339b0. Opens the pane at display
    // (x,y), plays a death voice and adds the three buttons.
    bool OpenDeath(int32_t x, int32_t y);
};

class TDeathScreen : public TScreen
{
  public:
    bool Initialize() override;
    void Close() override;

  private:
    TDeathPane pane;            // retail keeps it in a global (0x0066f500)
    TCursorHud cursorHud;
};

extern TDeathScreen DeathScreen;
