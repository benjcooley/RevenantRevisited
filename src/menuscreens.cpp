// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   menuscreens.cpp - the title route's Load Game and Options screens   *
// *************************************************************************

#include "menuscreens.h"

#include "gameflow.h"
#include "logging.h"
#include "multi.h"
#include "renderer.h"
#include "revenant.h"

TLoadGameScreen LoadGameScreen;
TOptionsScreen  OptionsScreen;

namespace {

constexpr float kCursorHudZ = 1000.0f;

// Retail installs GameData's "cursor" (0x0043a020) on both screens.
void ShowCursor(TCursorHud& hud)
{
    if (PTBitmap cursor = GameData ? GameData->Bitmap(const_cast<char*>("cursor")) : nullptr)
        SetMouseBitmap(cursor);
    if (Renderer)
        Renderer->AddHud(&hud, kCursorHudZ);
}

}  // namespace

// *******************
// * TLoadGameScreen *
// *******************

bool TLoadGameScreen::Initialize()
{
    log_info("[loadgamescreen] initialize");
    ShowCursor(cursorHud);

    // 0x005392e0: the fader, starting black, into both slots; TScreen fades
    // it in once Initialize returns and out when a button closes the screen.
    screenfade.Setup(TScreenFade::kDefaultSteps);
    fade = &screenfade;

    int32_t ox = 0, oy = 0;
    ClassicCanvasOrigin(ox, oy);
    if (!pane.OpenLoad(false, ox, oy))
    {
        log_error("[loadgamescreen] can't open the load dialog");
        return false;
    }
    // REVSYNC: the load dialog's title branch (0x00539590): Load Game starts
    // the slot through the PlayScreen (retail SetStartMode(1) + next =
    // PlayScreen; here the flow's load, behind the loading screen); Exit goes
    // back to the title.
    pane.SetOnActivate([this](TDefPane&, const SDefWidget& widget, int32_t) {
        if (IsDone())               // already on its way out
            return;
        if (widget.name == "loadgame")
        {
            const std::string slot = pane.SelectedSlot();
            log_info("[loadgamescreen] load '%s'", slot.c_str());
            GameFlow.LoadGame(slot.c_str());
        }
        else if (widget.name == "exit")
            GameFlow.ReturnToTitle();
    });
    AddPane(&pane);
    return true;
}

void TLoadGameScreen::Close()
{
    if (Renderer)
        Renderer->RemoveHud(&cursorHud);
    RemovePane(&pane);
    pane.Close();
}

// ******************
// * TOptionsScreen *
// ******************

bool TOptionsScreen::Initialize()
{
    log_info("[optionsscreen] initialize");
    int32_t ox = 0, oy = 0;
    ClassicCanvasOrigin(ox, oy);
    if (!pane.OpenOptions(false, ox, oy))
    {
        log_error("[optionsscreen] Trouble initializing Options pane");
        return false;
    }
    // REVSYNC: 0x0053aa90 with fromGame = 0: "ok" (after applying) and
    // "cancel" both go back to the title.
    pane.SetOnActivate([this](TDefPane&, const SDefWidget& widget, int32_t) {
        if (!IsDone() && (widget.name == "ok" || widget.name == "cancel"))
            GameFlow.ReturnToTitle();
    });
    ShowCursor(cursorHud);
    AddPane(&pane);
    return true;
}

void TOptionsScreen::Close()
{
    if (Renderer)
        Renderer->RemoveHud(&cursorHud);
    RemovePane(&pane);
    pane.Close();
}
