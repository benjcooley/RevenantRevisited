// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *      uitextbartest.cpp - --test=ui-textbar: the TTextBar pane         *
// *************************************************************************
//
// See uitextbartest.h.
//
// *************************************************************************

#include "uitextbartest.h"

#include "dialog.h"
#include "logging.h"
#include "renderer.h"
#include "screen.h"
#include "textbar.h"
#include "time.h"

#include <cstdint>

namespace {

using ELineType = TTextBar::ELineType;

// The loading bar's run, as the map loader drives it (0x004597b0): the line,
// the level stepped from 0 to 180 (percent * 180 / 1000), then the bar goes.
constexpr int64_t kLoadStart = 72;
constexpr int64_t kLoadEnd   = 120;
constexpr int64_t kLoadStep  = 6;

// A flat, mid-toned backdrop under the screen's pane layer, so the black
// shadow reads (the test screen draws no world).
class TBackdrop final : public THudDrawable
{
  public:
    void Draw() override { Renderer->FillScreen(0.40f, 0.34f, 0.27f, 1.0f); }
};

TBackdrop g_backdrop;
int64_t g_firstTick = 0;
int64_t g_lastTick  = 0;

void LogFeed(int64_t tick)
{
    const int32_t n = TextBar.NumLines();
    log_info("[ui-textbar] tick %lld: %d line%s, newest \"%s\"%s", static_cast<long long>(tick), n,
             n == 1 ? "" : "s", n ? TextBar.Line(0).text.c_str() : "",
             TextBar.IsHealthDisplayed() ? " over the loading bar" : "");
}

// Test data, by tick since the mode started (24 per second). Ticks 6 and 12
// print Windows-1252 dialog lines (the apostrophe 0x92, the e-grave 0xE8;
// docs/ui/TEXT_RENDERING.md); they fade before the end, so the three lines
// left are the same as without them.
void Feed(int64_t tick)
{
    if (tick > kLoadStart && tick <= kLoadEnd)
    {
        if ((tick - kLoadStart) % kLoadStep)
            return;
        const int32_t percent = int32_t((tick - kLoadStart) * 1000 / (kLoadEnd - kLoadStart));
        TextBar.SetLevels(percent * 180 / 1000, percent * 180 / 1000);
        LogFeed(tick);
        return;
    }

    switch (tick)
    {
        case 0:   TextBar.Print("Locke entered The Keep");                                  break;
        case 6:   TextBar.Print("%s", DialogList.GetLine("I2MIY01"));                       break;
        case 12:  TextBar.Print("%s", DialogList.GetLine("XII12NAV01"));                    break;
        case 18:  TextBar.Print("%s", DialogList.GetLine("DOORLOCKED"));                    break;
        case 36:  TextBar.Print(ELineType::Notice, "%s", DialogList.GetLine("ITEMTOFAR"));  break;
        case 54:  TextBar.Print("Picked up Greater Mana.\nChest opened.");                   break;
        case kLoadStart: TextBar.SetHealthDisplay(DialogList.GetLine("LOADMAPMSG"));        break;
        case kLoadEnd + kLoadStep: TextBar.ClearHealthDisplay();                           break;
        case 138: TextBar.Print("Got the Hammer of Wounding");                              break;
        case 156: TextBar.Print("%s envenomed.", "Short Sword");                            break;
        default:  return;
    }
    LogFeed(tick);
}

}  // namespace

bool InitializeUITextBarMode()
{
    if (!CurrentScreen || !TextBar.Initialize())
    {
        log_error("[ui-textbar] the text bar did not initialize");
        return false;
    }
    CurrentScreen->AddPane(&TextBar);
    if (Renderer)
        Renderer->AddHud(&g_backdrop, 0.0f);
    log_info("[ui-textbar] pane at (%d, %d) %d x %d", TextBar.GetPosX(), TextBar.GetPosY(),
             TextBar.GetWidth(), TextBar.GetHeight());

    g_firstTick = TTime::LegacyFrameCount();
    g_lastTick = g_firstTick - 1;
    return true;
}

// The test screen doesn't run the screen's pane pass, so the host does, as
// TPlayScreen::Pulse does: per tick, the world (here the feed), then the pane.
void RenderUITextBarMode()
{
    const int64_t now = TTime::LegacyFrameCount();
    while (g_lastTick < now)
    {
        ++g_lastTick;
        Feed(g_lastTick - g_firstTick);
        TextBar.Pulse();
    }
}

void CloseUITextBarMode()
{
    if (Renderer)
        Renderer->RemoveHud(&g_backdrop);
    if (CurrentScreen)
        CurrentScreen->RemovePane(&TextBar);
    TextBar.Close();
}
