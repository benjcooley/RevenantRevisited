// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *           death.cpp - TDeathScreen and TDeathPane (game over)         *
// *************************************************************************

#include "death.h"

#include "gameflow.h"
#include "logging.h"
#include "multi.h"
#include "renderer.h"
#include "revenant.h"
#include "revutils.h"   // random()
#include "sound.h"

#include <cstring>

TDeathScreen DeathScreen;

namespace {

// Death voices, in retail order (strings 0x005e3ef0..0x005e3f10). Retail picks
// RandomRange(0,4) over this run of five pointers. The shipped voice is
// go1sar00, so retail's "gosar00" lookup finds nothing - kept as retail has it.
const char* const kDeathVoices[] = { "goluc01", "goand01", "gojha01", "gooli01", "gosar00" };

constexpr float kCursorHudZ = 1000.0f;

}  // namespace

bool TDeathPane::OpenDeath(int32_t x, int32_t y)
{
    if (!OpenChrome(x, y, WIDTH, HEIGHT, "death.dat", "background"))
        return false;

    // REVSYNC-DIVERGENCE: retail plays the voice with extra parameters
    // (0x0049b990(id, 0x7f, 1, 0, 0x50, 700), meaning unconfirmed); the port
    // plays it at the sound player's defaults.
    char* voice = const_cast<char*>(kDeathVoices[random(0, 4)]);
    const int32_t id = SoundPlayer.FindSound(voice);
    if (id >= 0 && SoundPlayer.Mount(id))
        SoundPlayer.Play(id);
    else
        log_info("[death] voice '%s' not available", voice);

    // Buttons from death.dat sprites, placed by their registration points
    // (TButton(multi, name) 0x0042c400, as on the title screen).
    AddSpriteButton("restart", "Restart");
    AddSpriteButton("load", "Load");
    AddSpriteButton("exit", "Exit");
    return true;
}

// REVSYNC: TDeathScreen::Initialize @ 0x005338a0
bool TDeathScreen::Initialize()
{
    log_info("[death] initialize");
    int32_t ox = 0, oy = 0;
    ClassicCanvasOrigin(ox, oy);
    if (!pane.OpenDeath(ox, oy))
    {
        log_error("[death] Trouble initializing Death pane");
        return false;
    }

    // REVSYNC: button callbacks @ 0x00533950 (Restart -> PlayScreen),
    // 0x00533970 (Load -> load-game screen), 0x00533990 (Exit -> title).
    pane.SetOnActivate([](TDefPane&, const SDefWidget& widget, int32_t) {
        if (widget.name == "restart")
            GameFlow.RestartAfterDeath();
        else if (widget.name == "load")
            log_warn("[death] Load Game screen not ported yet");   // retail 0x0066fa78
        else if (widget.name == "exit")
            GameFlow.ReturnToTitle();
    });
    AddPane(&pane);

    if (PTBitmap cursor = GameData ? GameData->Bitmap(const_cast<char*>("cursor")) : nullptr)
        SetMouseBitmap(cursor);
    if (Renderer)
        Renderer->AddHud(&cursorHud, kCursorHudZ);
    return true;
}

void TDeathScreen::Close()
{
    if (Renderer)
        Renderer->RemoveHud(&cursorHud);
    RemovePane(&pane);
    pane.Close();
}
