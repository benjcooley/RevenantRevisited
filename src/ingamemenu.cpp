// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   ingamemenu.cpp - the ESC menu and the dialogs it runs during play   *
// *************************************************************************

#include "ingamemenu.h"

#include "gameflow.h"
#include "logging.h"
#include "playscreen.h"
#include "sound.h"

namespace {

// 0x00537110: the menu's rect on the 640x480 screen.
constexpr int32_t kMenuX = 126;
constexpr int32_t kMenuY = 65;
constexpr int32_t kMenuW = 394;
constexpr int32_t kMenuH = 316;

// RunModal flags for every in-game dialog: `MP ? 7 : 0xf` (0x0047e500) --
// input to the dialog only, and in single player the world paused.
constexpr uint32_t kDialogModalFlags = TScreen::MODAL_GAME;

// Popup flags of quitgameyn / exitgameyn (0x00537190, 0x0047c630): Yes/No,
// opaque chrome.
constexpr uint32_t kConfirmFlags = TPopupPane::POPUP_YESNO | TPopupPane::POPUP_ALPHA;

}  // namespace

// ********************
// * TInGameMenuPane  *
// ********************

bool TInGameMenuPane::OpenMenu(int32_t x, int32_t y)
{
    if (!Open("ingamemenu", "ingamemenu", DEF_INGAME, x + kMenuX, y + kMenuY, kMenuW, kMenuH,
              "ingamemenu"))
        return false;
    SoundPlayer.PauseSamples();
    return true;
}

void TInGameMenuPane::Close()
{
    confirm.Close();
    if (IsOpen())
        SoundPlayer.ResumeSamples();
    TDefPane::Close();
}

void TInGameMenuPane::OnActivate(const SDefWidget& widget, int32_t buttonIndex)
{
    (void)buttonIndex;
    if (widget.name == "options")
        Finish(RESULT_OPTIONS);
    else if (widget.name == "load")
        Finish(RESULT_LOAD);
    else if (widget.name == "save")
    {
        // DAT_0065d0d0 (PlayScreen +0x5e0): the click does nothing while the
        // player has no control.
        if (PlayScreen.IsControlOn())
            Finish(RESULT_SAVE);
        else
            log_info("[ingamemenu] save refused: no player control");
    }
    else if (widget.name == "quit")
        Confirm("quitgameyn", RESULT_QUIT);
    else if (widget.name == "exit")
        Confirm("exitgameyn", RESULT_EXIT);
    else if (widget.name == "resume")
        Finish(RESULT_RESUME);
}

// The question as a popup over the menu; Yes ends the menu with `result`.
// REVSYNC-DIVERGENCE: retail also released every sound effect on Yes
// (0x0049c8f0); leaving the game stops the area sounds here.
void TInGameMenuPane::Confirm(const char* question, EResult result)
{
    if (!Screen())
        return;
    confirm.Ask(*Screen(), question, kConfirmFlags, [this, result](int32_t answer) {
        if (answer == TPopupPane::ANSWER_YES)
            Finish(result);
    });
}

void TInGameMenuPane::OnKey(int32_t vk, bool down)
{
    if (vk == VK_ESCAPE && down)
        Finish(RESULT_RESUME);
}

// ****************
// * TInGameMenu  *
// ****************

void TInGameMenu::Open()
{
    // 0x0047e500 returns when the menu pane is already open (DAT_0066f788).
    if (menu.IsOpen())
        return;
    int32_t ox = 0, oy = 0;
    ClassicCanvasOrigin(ox, oy);
    if (!menu.OpenMenu(ox, oy))
    {
        log_error("[ingamemenu] can't open the in-game menu");
        return;
    }
    log_info("[ingamemenu] open");
    if (!screen.PushModal(&menu, kDialogModalFlags, [this](int32_t result) { MenuDone(result); }))
        menu.Close();
}

// The switch after RunModal in 0x0047e500: a dialog's Exit (result 0) comes
// back to the menu, as does Options always; Quit and Exit leave.
void TInGameMenu::MenuDone(int32_t result)
{
    menu.Close();
    screen.Redraw();                // PlayScreen dirty (DAT_0065cb40)
    log_info("[ingamemenu] result %d", result);
    switch (result)
    {
    case TInGameMenuPane::RESULT_LOAD:
        OpenLoad([this](int32_t loaded) { if (loaded == TSaveSlotPane::RESULT_EXIT) Open(); });
        break;
    case TInGameMenuPane::RESULT_SAVE:
        OpenSave([this](int32_t saved) { if (saved == TSaveSlotPane::RESULT_EXIT) Open(); });
        break;
    case TInGameMenuPane::RESULT_OPTIONS:
        OpenOptions([this](int32_t) { Open(); });
        break;
    case TInGameMenuPane::RESULT_QUIT:
        GameFlow.ReturnToTitle();
        break;
    case TInGameMenuPane::RESULT_EXIT:
        // REVSYNC-DIVERGENCE: PostQuitMessage left at once; the flow's quit
        // fades the play screen out first.
        GameFlow.QuitApplication();
        break;
    default:
        break;                      // Resume
    }
}

// REVSYNC: 0x0047e660 + the load dialog's in-game branch (0x00539590): the
// chosen slot replaces the game. REVSYNC-DIVERGENCE: retail loaded inside the
// dialog behind its "loadingmap" progress popup; the port hands the slot to
// the session, which loads it at the start of the next tick.
void TInGameMenu::OpenLoad(TScreen::TModalDone done)
{
    if (load.IsOpen())
        return;
    int32_t ox = 0, oy = 0;
    ClassicCanvasOrigin(ox, oy);
    if (!load.OpenLoad(true, ox, oy))
    {
        log_error("[ingamemenu] can't open the load dialog");
        return;
    }
    load.SetOnActivate([this](TDefPane&, const SDefWidget& widget, int32_t) {
        load.Finish(widget.name == "loadgame" ? TSaveSlotPane::RESULT_DONE
                                              : TSaveSlotPane::RESULT_EXIT);
    });
    const bool pushed = screen.PushModal(&load, kDialogModalFlags, [this, done](int32_t result) {
        if (result == TSaveSlotPane::RESULT_DONE)
        {
            const std::string slot = load.SelectedSlot();
            log_info("[ingamemenu] load '%s'", slot.c_str());
            GameFlow.Session().RequestLoad(slot);
        }
        load.Close();
        screen.Redraw();
        if (done)
            done(result);
    });
    if (!pushed)
        load.Close();
}

// Command 0x54 and the menu's case 2. REVSYNC-DIVERGENCE: retail's dialog
// called SaveGame itself (0x0048d720); the port hands the name to the
// session, which saves at the start of the next tick.
void TInGameMenu::OpenSave(TScreen::TModalDone done)
{
    if (save.IsOpen())
        return;
    int32_t ox = 0, oy = 0;
    ClassicCanvasOrigin(ox, oy);
    if (!save.OpenSave(ox, oy))
    {
        log_error("[ingamemenu] can't open the save dialog");
        return;
    }
    const bool pushed = screen.PushModal(&save, kDialogModalFlags, [this, done](int32_t result) {
        if (result == TSaveSlotPane::RESULT_DONE)
        {
            log_info("[ingamemenu] save '%s'", save.SaveName().c_str());
            GameFlow.Session().RequestSave(save.SaveName());
        }
        save.Close();
        screen.Redraw();
        if (done)
            done(result);
    });
    if (!pushed)
        save.Close();
}

// REVSYNC: 0x0047e700 -- the options pane from the game (fromGame = 1);
// "ok" and "cancel" both close it (0x0053aa90).
void TInGameMenu::OpenOptions(TScreen::TModalDone done)
{
    if (options.IsOpen())
        return;
    int32_t ox = 0, oy = 0;
    ClassicCanvasOrigin(ox, oy);
    if (!options.OpenOptions(true, ox, oy))
    {
        log_error("[ingamemenu] Trouble initializing Options pane");
        return;
    }
    options.SetOnActivate([this](TDefPane&, const SDefWidget& widget, int32_t) {
        if (widget.name == "ok" || widget.name == "cancel")
            options.Finish(0);
    });
    const bool pushed = screen.PushModal(&options, kDialogModalFlags, [this, done](int32_t result) {
        options.Close();
        screen.Redraw();
        if (done)
            done(result);
    });
    if (!pushed)
        options.Close();
}

// REVSYNC: the ESC case of 0x0047c630 in demo mode: the effects pause around
// "exitgameyn"; Yes quits.
void TInGameMenu::AskExit()
{
    if (popup.IsOpen())
        return;
    SoundPlayer.PauseSamples();
    const bool asked = popup.Ask(screen, "exitgameyn", kConfirmFlags, [](int32_t answer) {
        SoundPlayer.ResumeSamples();
        if (answer == TPopupPane::ANSWER_YES)
            GameFlow.QuitApplication();
    });
    if (!asked)
        SoundPlayer.ResumeSamples();
}

bool TInGameMenu::IsOpen() const
{
    return menu.IsOpen() || load.IsOpen() || save.IsOpen() || options.IsOpen() || popup.IsOpen();
}

void TInGameMenu::Close()
{
    menu.Close();
    load.Close();
    save.Close();
    options.Close();
    popup.Close();
}
