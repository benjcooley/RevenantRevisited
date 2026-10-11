// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uipanehost.cpp - what the --test=ui-* / ab-* pane hosts share        *
// *************************************************************************
//
// See uipanehost.h.
//
// *************************************************************************

#include "uipanehost.h"

#include "logging.h"
#include "screen.h"
#include "time.h"
#include "uidemoplayer.h"

#include <cstdio>
#include <sstream>

void TTestPaneHost::TBackdrop::Draw()
{
    Renderer->FillScreen(0.0f, 0.0f, 0.0f, 1.0f);
}

bool TTestPaneHost::Host(TPane& hosted, bool withBackdrop)
{
    if (!CurrentScreen)
        return false;
    pane = &hosted;
    CurrentScreen->AddPane(pane);
    backdrop = withBackdrop && Renderer;
    if (backdrop)
        Renderer->AddHud(&backdropDrawable, 0.0f);
    lastTick = TTime::LegacyFrameCount() - 1;
    return true;
}

void TTestPaneHost::Unhost()
{
    if (backdrop && Renderer)
        Renderer->RemoveHud(&backdropDrawable);
    backdrop = false;
    if (pane && CurrentScreen)
        CurrentScreen->RemovePane(pane);
    pane = nullptr;
}

void TTestPaneHost::PulseTicks()
{
    for (const int64_t now = TTime::LegacyFrameCount(); lastTick < now; ++lastTick)
        if (pane && !pane->IsHidden())
            pane->Pulse();
}

bool TAbClock::Stop()
{
    if (stopped || TTime::LegacyFrameFraction() != 0.0)
        return false;
    timeScale = TTime::TimeScale();
    TTime::SetTimeScale(0.0);
    stopped = true;
    return true;
}

void TAbClock::Restore()
{
    if (stopped)
        TTime::SetTimeScale(timeScale);
    stopped = false;
}

std::vector<std::string> SplitAbField(const std::string& text, char separator)
{
    std::vector<std::string> parts;
    std::istringstream in(text);
    for (std::string part; std::getline(in, part, separator);)
        parts.push_back(part);
    if (!text.empty() && text.back() == separator)
        parts.emplace_back();
    return parts;
}

bool SAbItem::Parse(const std::string& text, SAbItem& item)
{
    item = {};
    if (text == "-")
        return true;
    std::string head = text;
    const size_t open = text.find('(');
    if (open != std::string::npos)
    {
        if (text.back() != ')')
            return false;
        head = text.substr(0, open);
        for (const std::string& content : SplitAbField(text.substr(open + 1, text.size() - open - 2), ','))
            if (!Parse(content, item.contents.emplace_back()) || item.contents.back().type.empty())
                return false;
    }
    const size_t star = head.find('*');
    item.type = head.substr(0, star);
    if (star != std::string::npos
        && (std::sscanf(head.c_str() + star + 1, "%d", &item.amount) != 1 || item.amount < 1))
        return false;
    return !item.type.empty();
}

bool SAbItem::Place(TObjectInstance* owner, int32_t slot) const
{
    if (type.empty())
        return true;
    TObjectInstance* placed = UIDemoPlayer::AddItem(owner, type.c_str(), slot, amount > 0 ? amount : 1);
    if (!placed)
        return false;
    for (size_t index = 0; index < contents.size(); ++index)
        if (!contents[index].Place(placed, static_cast<int32_t>(index)))
            return false;
    return true;
}
