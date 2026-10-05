// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   ingamemenu.h - the ESC menu and the dialogs it runs during play     *
// *************************************************************************
//
// Retail TPlayScreen::InGameMenu (0x0047e500) opens ingamemenu.def as a modal
// pane and loops over nested RunModal calls: Load and Save run their dialog
// and come back to the menu when it's left with Exit; Options always comes
// back; Quit Module returns to the title; Exit Program quits. The port can't
// nest frame loops, so TInGameMenu runs the same panes in the same order as a
// chain of PushModal completions (docs/gameflow/ARCHITECTURE.md §4.2). The
// play screen's hotkeys open the dialogs directly through it too. Behaviour:
// docs/gameflow/forensics/INGAME_MENU.md.
#pragma once

#include "defpane.h"
#include "optionspane.h"
#include "popuppane.h"
#include "savegamepane.h"

#include <cstdint>
#include <string>

// Retail's menu pane (cls_0x5b9480, instance 0x0066f748).
class TInGameMenuPane : public TDefPane
{
  public:
    // Retail results (+0x5c, read by 0x0047e500).
    enum EResult : int32_t
    {
        RESULT_NONE = -1, RESULT_LOAD = 1, RESULT_SAVE, RESULT_OPTIONS,
        RESULT_QUIT, RESULT_EXIT, RESULT_RESUME
    };

    // REVSYNC: Open @ 0x00537110 -- ingamemenu.def panel "ingamemenu",
    // DEF flags 0x11, at (126, 65) 394x316 on the 640x480 screen whose
    // corner is display (x,y); the sound effects pause.
    bool OpenMenu(int32_t x, int32_t y);
    // REVSYNC: Close @ 0x00537170 -- the sound effects resume.
    void Close() override;

  protected:
    // REVSYNC: Control @ 0x00537190 -- the buttons' results; Save only while
    // the player has control (PlayScreen +0x5e0, so not in a conversation or
    // a cutscene); Quit and Exit ask first.
    void OnActivate(const SDefWidget& widget, int32_t buttonIndex) override;
    // REVSYNC: KeyPress @ 0x00537400 -- ESC resumes; no other key.
    void OnKey(int32_t vk, bool down) override;

  private:
    void Confirm(const char* question, EResult result);

    TPopupPane confirm;     // quitgameyn / exitgameyn
};

// The play screen's in-game dialogs.
class TInGameMenu
{
  public:
    explicit TInGameMenu(TScreen& host) : screen(host) {}

    // REVSYNC: 0x0047e500 -- the menu (ESC), unless it is already up.
    void Open();
    // The dialogs on their own: REVSYNC: 0x0047e660 (Load Game command 0x53),
    // Command 0x54 (Save Game), 0x0047e700 (Game Options command 0x52).
    // `done` gets the dialog's result once it has closed.
    void OpenLoad(TScreen::TModalDone done = nullptr);
    void OpenSave(TScreen::TModalDone done = nullptr);
    void OpenOptions(TScreen::TModalDone done = nullptr);
    // Demo mode's ESC (0x0047c630): "exitgameyn", then quit on Yes.
    void AskExit();

    // The load dialog's in-game load, which the play screen stages
    // (TPlayScreen::StepGameLoad): its progress per mille, and its end.
    // Nothing happens unless the load came from the dialog.
    void LoadProgress(int32_t permille);
    void LoadFinished(bool loaded);

    // True while any of its panes is up.
    [[nodiscard]] bool IsOpen() const;
    // The play screen is closing: close whatever is still up.
    void Close();

  private:
    void MenuDone(int32_t result);
    void BeginLoad();
    void EndLoad(bool loaded);

    TScreen&        screen;
    TInGameMenuPane menu;
    TLoadGamePane   load;
    TSaveGamePane   save;
    TOptionsPane    options;
    TPopupPane      popup;
    TPopupPane      progress;       // "loadingmap" (retail 0x0066ff10)
    bool            loading = false;  // the dialog's load is under way
};
