// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   popuppane.cpp - TPopupPane, retail's message / yes-no popup          *
// *************************************************************************

#include "popuppane.h"

#include "dialog.h"     // DialogList
#include "logging.h"

namespace {

// 0x0053bf00: the popup's rect on the 640x480 screen, and its .def / dat.
constexpr int32_t kPopupX = 129;
constexpr int32_t kPopupY = 122;
constexpr int32_t kPopupW = 398;
constexpr int32_t kPopupH = 212;
constexpr const char* kPopupDef = "popup";

}  // namespace

bool TPopupPane::Ask(TScreen& screen, const char* text, uint32_t flags,
                     TScreen::TModalDone done)
{
    if (IsOpen() || !text)
        return false;

    // The message: the string-table line for the key, or the text itself.
    message = (flags & POPUP_TEXT) ? std::string(text) : std::string(DialogList.GetLine(text));

    const char* panel = (flags & POPUP_YESNO)    ? "yesno"
                      : (flags & POPUP_OKCANCEL) ? "okcancel" : "ok";
    const uint32_t defFlags = (flags & POPUP_ALPHA) ? 0 : DEF_INGAME;
    int32_t ox = 0, oy = 0;
    ClassicCanvasOrigin(ox, oy);
    if (!Open(kPopupDef, panel, defFlags, ox + kPopupX, oy + kPopupY, kPopupW, kPopupH, kPopupDef))
    {
        log_error("[popup] can't open '%s'", panel);
        return false;
    }

    log_info("[popup] %s: %s", panel, text);
    const bool pushed = screen.PushModal(this, TScreen::MODAL_INPUT,
                                         [this, done = std::move(done)](int32_t answer) {
                                             Close();
                                             if (done)
                                                 done(answer);
                                         });
    if (!pushed)
        Close();
    return pushed;
}

void TPopupPane::OnOpened()
{
    SetText("message", message);
}

void TPopupPane::OnActivate(const SDefWidget& widget, int32_t buttonIndex)
{
    (void)buttonIndex;
    if (widget.name == "ok" || widget.name == "yes")
        Finish(ANSWER_YES);
    else if (widget.name == "cancel" || widget.name == "no")
        Finish(ANSWER_NO);
}
