// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   loadscreen.h - TLoadScreen, the loading bar while a game starts     *
// *************************************************************************
//
// Retail draws the loading bar (cls_0x4485a0) from TPlayScreen::Initialize
// (0x0047a660), stepping it after each subsystem init inside one long frame,
// then filling it as the sectors around the player load. The port loads a
// game as a sequence of session steps (TGameSession), so the bar is a screen
// of its own: the title fades out, TLoadScreen runs one step per tick with
// the bar at retail's running total for it, then the play screen fades in
// (docs/gameflow/ARCHITECTURE.md §3.5 2d, §7). Visual reference:
// docs/ui/forensics/LoadingScreen_SPEC.md.
#pragma once

#include "screen.h"

#include <cstdint>
#include <memory>

class TBitmap;
class TMulti;
class TSurface;

// The bar itself: loadbar.dat's "background" with "bar" drawn over it as a
// left slice as wide as the progress.
class TLoadBarPane : public TPane
{
  public:
    static constexpr int32_t kFull = 1000;     // progress is per mille

    TLoadBarPane() : TPane(0, 0, 640, 480) {}

    // REVSYNC: 0x004485a0 -- loadbar.dat's "background" and "bar". Retail
    // also took <module>\loadscreen.bmp as the background when it exists; no
    // shipped module has one, and the port doesn't read it yet.
    bool Initialize() override;
    // REVSYNC: 0x00448650
    void Close() override;

    // REVSYNC: 0x00448680 -- advance by `delta` per mille, clamped to full.
    void Step(int32_t delta);
    // REVSYNC: 0x00448800 -- set to `fraction` of full.
    void Set(float fraction);
    [[nodiscard]] int32_t Progress() const { return progress; }

    void Compose() override;
    void Draw() override;

  private:
    TMulti*   dat        = nullptr;
    TBitmap*  background = nullptr;
    TBitmap*  bar        = nullptr;
    std::unique_ptr<TSurface> surface;      // background + bar, composed at 640x480
    int32_t   progress   = 0;               // retail +0xc
    int32_t   composed   = -1;              // the progress `surface` shows
};

class TLoadScreen : public TScreen
{
  public:
    bool Initialize() override;
    void Close() override;
    // Each tick after the first frame, TGameFlow runs the next load step.
    void Pulse() override;

    // The session's progress so far (per mille).
    void SetProgress(int32_t permille);

  private:
    TLoadBarPane bar;
};

extern TLoadScreen LoadScreen;
