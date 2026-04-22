// *************************************************************************
// *                         Cinematix Revenant                            *
// *                                                                        *
// *   testscreen.h — bring-up harness for the retail sync port             *
// *                                                                        *
// *   See recon/docs/RETAIL_SYNC_PLAN.md for context. This screen is       *
// *   routed in via --test=<mode> so we can exercise one subsystem at a    *
// *   time as we synchronize classes from Ghidra.                          *
// *************************************************************************

#pragma once

#include "screen.h"

class TTestScreen : public TScreen
{
  public:
    TTestScreen() = default;

    bool Initialize() override;
    void Close() override;
    void Pulse() override;
    void DrawBackground() override;
    void Animate(bool draw) override;
    void KeyPress(int32_t key, bool down) override;
    void MouseClick(int32_t button, int32_t x, int32_t y) override;
    void MouseMove(int32_t button, int32_t x, int32_t y) override;

  private:
    int64_t lastLoggedLegacyFrame = -1;
};

extern TTestScreen TestScreen;
