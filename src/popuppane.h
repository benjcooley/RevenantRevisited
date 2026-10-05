// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   popuppane.h - TPopupPane, retail's message / yes-no popup            *
// *************************************************************************
//
// Retail's popup (instance 0x00670090; open 0x0053bf00, control 0x0053bfa0,
// run by 0x0053c060): popup.def's "yesno", "okcancel" or "ok" panel with a
// message from the string table, run modally until a button answers. Yes and
// Ok answer 1, No and Cancel 0; Enter and ESC press them (the DEF buttons'
// default keys). Layout: docs/ui/forensics/PopupDef_SPEC.md; behaviour:
// docs/gameflow/forensics/INGAME_MENU.md §9.
#pragma once

#include "defpane.h"

#include <cstdint>
#include <string>

class TPopupPane : public TDefPane
{
  public:
    // Retail popup flags (0x0053bf00).
    static constexpr uint32_t POPUP_ALPHA    = 0x01;   // opaque chrome (DEF flags 0), else in-game overlay
    static constexpr uint32_t POPUP_YESNO    = 0x02;   // "yesno" panel
    static constexpr uint32_t POPUP_OKCANCEL = 0x04;   // "okcancel" panel (else "ok")
    static constexpr uint32_t POPUP_TEXT     = 0x10;   // `message` is the text, not a string-table key

    static constexpr int32_t ANSWER_NO  = 0;
    static constexpr int32_t ANSWER_YES = 1;

    // REVSYNC: 0x0053c060 -- push the popup on `screen` as a modal (retail
    // RunModal flags 7, the enclosing modal's added) and hand the answer to
    // `done`. False, with `done` not called, when it can't open (retail
    // answered 0).
    bool Ask(TScreen& screen, const char* message, uint32_t flags,
             TScreen::TModalDone done);

  protected:
    // REVSYNC: 0x0053bfa0 event 1 -- the message into the "message" TEXT.
    void OnOpened() override;
    // REVSYNC: 0x0053bfa0 event 3000 -- ok/yes answer 1, cancel/no 0.
    void OnActivate(const SDefWidget& widget, int32_t buttonIndex) override;

  private:
    std::string message;
};
