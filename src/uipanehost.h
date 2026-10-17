// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uipanehost.h - what the --test=ui-* / ab-* pane hosts share          *
// *************************************************************************
//
// The thin test-mode hosts of the production HUD panes (uibottombartest,
// uiinventorytest) put a pane on the test screen and drive it as the play
// screen would; their A/B modes run one case for tools/retail_ab/hud_ab.py.
//
// - TTestPaneHost: the pane on the test screen, black under it if asked
//   (retail's frame is cleared), pulsed once a legacy tick -- the test
//   screen runs no pane pass.
// - TAbClock: an A/B case runs on the first frame at a tick boundary (run it
//   with --snapstep=0.03125: every fourth frame is one) and stops the clock
//   there, so the snapshot shows the pane as that tick leaves it.
// - SAbItem: an inventory item in a case, "Type", "Type*N" for an amount,
//   "Type(Item,Item)" for a pouch's slots 0, 1, ..., "-" for none; Place
//   puts it in an inventory slot through the demo player's fixture API.
//
// *************************************************************************

#pragma once

#include "renderer.h"

#include <cstdint>
#include <string>
#include <vector>

class JsonOut;
class TObjectInstance;
class TPane;
struct STextDraw;

class TTestPaneHost
{
  public:
    TTestPaneHost() = default;
    TTestPaneHost(const TTestPaneHost&) = delete;
    TTestPaneHost& operator=(const TTestPaneHost&) = delete;

    // Adds the (initialized, placed) pane to the test screen.
    bool Host(TPane& pane, bool backdrop);
    void Unhost();
    // Pulses the pane once for each legacy tick since the last call.
    void PulseTicks();
    [[nodiscard]] TPane* Pane() const { return pane; }

  private:
    class TBackdrop final : public THudDrawable
    {
      public:
        void Draw() override;
    };

    TPane* pane = nullptr;
    TBackdrop backdropDrawable;
    bool backdrop = false;
    int64_t lastTick = 0;
};

class TAbClock
{
  public:
    // True on the frame the case should run: the first at a tick boundary.
    // The clock stops there.
    bool Stop();
    // Restarts the clock if Stop stopped it.
    void Restore();
    [[nodiscard]] bool Stopped() const { return stopped; }

  private:
    bool stopped = false;
    double timeScale = 1.0;
};

struct SAbItem
{
    std::string type;               // empty: no item
    int32_t amount = 0;             // 0: the type's own
    std::vector<SAbItem> contents;  // a pouch's slots 0, 1, ...

    static bool Parse(const std::string& text, SAbItem& item);
    // Puts it in `owner`'s inventory `slot`; false if it can't.
    bool Place(TObjectInstance* owner, int32_t slot) const;
};

// `text` split at each `separator` (a trailing one makes an empty last part).
std::vector<std::string> SplitAbField(const std::string& text, char separator);

// The text draws a host recorded (SetTextDrawRecorder) as the A/B reads
// them: a "text" array of {text, face, x, y, w, h, align, color, shadowed,
// wrapped, lines: [[x, y, line], ...]}.
void WriteTextDraws(JsonOut& json, const std::vector<STextDraw>& text);
