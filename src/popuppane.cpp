// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   popuppane.cpp - TPopupPane, retail's message / yes-no popup          *
// *************************************************************************

#include "popuppane.h"

#include "dialog.h"     // DialogList
#include "logging.h"
#include "renderer.h"   // Renderer (the progress bar)
#include "surface.h"

#include <algorithm>
#include <utility>

namespace {

// 0x0053bf00: the popup's rect on the 640x480 screen, and its .def / dat.
constexpr int32_t kPopupX = 129;
constexpr int32_t kPopupY = 122;
constexpr int32_t kPopupW = 398;
constexpr int32_t kPopupH = 212;
constexpr const char* kPopupDef = "popup";

// 0x0053c3d0: the bar strip's colour, 0x00429950(0xaa, 0, 0), over the
// strip's transparent key colour.
constexpr uint8_t kBarRed = 0xaa;

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

bool TPopupPane::OpenProgress(TScreen& screen, const char* key, std::function<void()> shown)
{
    if (IsOpen() || !key)
        return false;

    message = DialogList.GetLine(key);
    bar     = 0;
    int32_t ox = 0, oy = 0;
    ClassicCanvasOrigin(ox, oy);
    if (!Open(kPopupDef, "progress", DEF_INGAME, ox + kPopupX, oy + kPopupY, kPopupW, kPopupH,
              kPopupDef))
    {
        log_error("[popup] can't open 'progress'");
        return false;
    }

    log_info("[popup] progress: %s", key);
    onShown    = std::move(shown);
    shownPulse = false;
    const bool pushed = screen.PushExclusive(this, TScreen::MODAL_INPUT, [this](int32_t) {
        Close();
        if (std::function<void()> closed = std::exchange(onClosed, nullptr))
            closed();
    });
    if (!pushed)
    {
        onShown = nullptr;
        Close();
    }
    return pushed;
}

void TPopupPane::SetProgress(int32_t permille)
{
    const int32_t value = std::clamp(permille, 0, 1000);
    if (value <= bar)
        return;
    bar = value;
    SetDirty(true);
}

void TPopupPane::CloseProgress(std::function<void()> closed)
{
    onShown  = nullptr;
    onClosed = std::move(closed);
    Finish(0);
}

void TPopupPane::Pulse()
{
    TDefPane::Pulse();
    if (!onShown || !FadedIn())
        return;
    if (!shownPulse)
    {
        shownPulse = true;
        return;
    }
    std::exchange(onShown, nullptr)();
}

void TPopupPane::OnOpened()
{
    SetText("message", message);
}

void TPopupPane::DrawField(const SDefWidget& widget)
{
    if (widget.field != "progress")
    {
        TDefPane::DrawField(widget);
        return;
    }
    TSurface* target = Surface();
    const int32_t   filled = widget.w * bar / 1000;
    if (target && filled > 0)
        Renderer->DrawSolidRectToTarget(widget.x, widget.y, filled, widget.h, target->Width(),
                                        target->Height(), kBarRed, 0, 0, 255);
}

void TPopupPane::OnActivate(const SDefWidget& widget, int32_t buttonIndex)
{
    (void)buttonIndex;
    if (widget.name == "ok" || widget.name == "yes")
        Finish(ANSWER_YES);
    else if (widget.name == "cancel" || widget.name == "no")
        Finish(ANSWER_NO);
}
