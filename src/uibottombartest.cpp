// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uibottombartest.cpp - --test=ui-bottombar: TBottomBarPane            *
// *************************************************************************
//
// See uibottombartest.h.
//
// *************************************************************************

#include "uibottombartest.h"

#include "bottombar.h"
#include "display.h"
#include "hudstate.h"
#include "jsonout.h"
#include "logging.h"
#include "object.h"
#include "player.h"
#include "playscreen.h"
#include "renderer.h"
#include "revdefs.h"
#include "screen.h"
#include "spell.h"
#include "testconfig.h"
#include "textencoding.h"
#include "time.h"
#include "uidemoplayer.h"

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

extern TObjectClass TalismanClass;

namespace {

constexpr int32_t kSidePanelWidth = 0xbc;   // TPlayScreen::Pulse 0x0047b4d0
constexpr int32_t kQuickSpells = 4;

// Black under the bar, as retail's frame is cleared (alone, the demo too:
// the bar is opaque).
class TBackdrop final : public THudDrawable
{
  public:
    void Draw() override { Renderer->FillScreen(0.0f, 0.0f, 0.0f, 1.0f); }
};

TBackdrop g_backdropDrawable;
std::unique_ptr<TBottomBarPane> g_bar;
THudDrawable* g_backdrop = nullptr;
int64_t g_lastTick = 0;

int32_t DisplayWidth() { return Display.Width() > 0 ? Display.Width() : WIDTH; }
int32_t DisplayHeight() { return Display.Height() > 0 ? Display.Height() : HEIGHT; }

// Along the bottom of the display, as wide as the map view: the display less
// the side panel while it is open.
int32_t MapViewWidth()
{
    return DisplayWidth() - (GetHudState().sidebarState == HUD_SIDEBAR_OPEN ? kSidePanelWidth : 0);
}

bool HostBar(int32_t width, bool backdrop)
{
    g_bar = std::make_unique<TBottomBarPane>();
    g_bar->Place(0, DisplayHeight() - TBottomBarPane::kHeight, width);
    if (!CurrentScreen || !g_bar->Initialize())
    {
        log_error("[ui-bottombar] the bottom bar did not initialize");
        g_bar.reset();
        return false;
    }
    CurrentScreen->AddPane(g_bar.get());
    if (backdrop && Renderer)
    {
        g_backdrop = &g_backdropDrawable;
        Renderer->AddHud(g_backdrop, 0.0f);
    }
    log_info("[ui-bottombar] bar at (%d, %d) %d x %d, %d boxes", g_bar->GetPosX(), g_bar->GetPosY(),
             g_bar->GetWidth(), g_bar->GetHeight(), g_bar->Shelf().NumBoxes());
    return true;
}

void UnhostBar()
{
    if (Renderer && g_backdrop)
        Renderer->RemoveHud(g_backdrop);
    g_backdrop = nullptr;
    if (!g_bar)
        return;
    if (CurrentScreen)
        CurrentScreen->RemovePane(g_bar.get());
    g_bar->Close();
    g_bar.reset();
}

// ---- the A/B case ----------------------------------------------------------

struct SItem
{
    std::string type;               // empty: the slot is empty
    int32_t amount = 1;
    std::vector<SItem> contents;    // a pouch's slots 0, 1, ...
};

struct SCase
{
    int32_t width = WIDTH - kSidePanelWidth;
    std::string spells[kQuickSpells];
    std::vector<std::string> castable;
    std::vector<SItem> belt;
    int32_t frame = 0;
};

std::vector<std::string> Split(const std::string& text, char separator)
{
    std::vector<std::string> parts;
    std::istringstream in(text);
    for (std::string part; std::getline(in, part, separator);)
        parts.push_back(part);
    if (!text.empty() && text.back() == separator)
        parts.emplace_back();
    return parts;
}

// "Type", "Type*N", "Pouch(Type,Type*N)", or "-" for an empty slot.
bool ParseItem(const std::string& text, SItem& item)
{
    if (text == "-")
        return true;
    std::string head = text;
    const size_t open = text.find('(');
    if (open != std::string::npos)
    {
        if (text.back() != ')')
            return false;
        head = text.substr(0, open);
        for (const std::string& content : Split(text.substr(open + 1, text.size() - open - 2), ','))
            if (!ParseItem(content, item.contents.emplace_back()) || item.contents.back().type.empty())
                return false;
    }
    const size_t star = head.find('*');
    item.type = head.substr(0, star);
    if (star != std::string::npos && (std::sscanf(head.c_str() + star + 1, "%d", &item.amount) != 1 || item.amount < 1))
        return false;
    return !item.type.empty();
}

// "width=W;spells=A,B,C,D;castable=A,B;belt=I|I|...;frame=N"
bool ParseCase(const std::string& text, SCase& c)
{
    for (const std::string& field : Split(text, ';'))
    {
        const size_t eq = field.find('=');
        const std::string key = field.substr(0, eq);
        const std::string value = eq == std::string::npos ? std::string() : field.substr(eq + 1);
        if (key == "width")
        {
            if (std::sscanf(value.c_str(), "%d", &c.width) != 1 || c.width <= 0)
                return false;
        }
        else if (key == "spells")
        {
            const std::vector<std::string> spells = Split(value, ',');
            if (spells.size() > kQuickSpells)
                return false;
            for (size_t i = 0; i < spells.size(); ++i)
                c.spells[i] = spells[i];
        }
        else if (key == "castable")
            c.castable = value.empty() ? std::vector<std::string>() : Split(value, ',');
        else if (key == "belt")
        {
            for (const std::string& slot : Split(value, '|'))
                if (!ParseItem(slot, c.belt.emplace_back()))
                    return false;
            if (c.belt.size() > size_t(kInvSlotLast - kInvSlotBeltFirst + 1))
                return false;
        }
        else if (key == "frame")
        {
            if (std::sscanf(value.c_str(), "%d", &c.frame) != 1 || c.frame < 0)
                return false;
        }
        else
            return false;
    }
    return true;
}

// The talisman type whose Code is `code`, or null.
const char* TalismanType(char code)
{
    for (int32_t type = 0; type < TalismanClass.NumTypes(); ++type)
        if (const SObjectInfo* info = TalismanClass.GetObjType(type))
            if (TalismanClass.GetStat(type, "Code") == code)
                return info->name;
    return nullptr;
}

// A Spell Pouch holding the talismans of each castable spell. The player
// can then cast exactly those (TPlayer::HasTalismans) -- so a case doesn't
// make a ring castable whose spell the castable ones' talismans also make.
bool GiveTalismans(const std::vector<std::string>& castable)
{
    if (castable.empty())
        return true;
    TObjectInstance* pouch = UIDemoPlayer::AddItem(Player, "Spell Pouch", -1);
    if (!pouch)
        pouch = UIDemoPlayer::AddItem(Player, "spellpouch", -1);
    if (!pouch)
        return false;
    for (const std::string& talismans : castable)
        for (const char code : talismans)
        {
            const char* type = TalismanType(code);
            if (!type || !UIDemoPlayer::AddItem(pouch, type, -1))
            {
                log_error("[ab-bottombar] no talisman '%c' for \"%s\"", code, talismans.c_str());
                return false;
            }
        }
    return true;
}

bool PlaceItem(TObjectInstance* owner, const SItem& item, int32_t slot)
{
    if (item.type.empty())
        return true;
    TObjectInstance* placed = UIDemoPlayer::AddItem(owner, item.type.c_str(), slot, item.amount);
    if (!placed)
        return false;
    for (size_t index = 0; index < item.contents.size(); ++index)
        if (!PlaceItem(placed, item.contents[index], int32_t(index)))
            return false;
    return true;
}

bool ApplyCase(const SCase& c)
{
    for (int32_t ring = 1; ring <= kQuickSpells; ++ring)
    {
        std::string talismans = c.spells[ring - 1];
        Player->SetQuickSpell(ring, talismans.data());
    }
    if (!GiveTalismans(c.castable))
        return false;
    UIDemoPlayer::RemoveItems(Player, kInvSlotBeltFirst, kInvSlotLast);
    for (size_t n = 0; n < c.belt.size(); ++n)
        if (!PlaceItem(Player, c.belt[n], kInvSlotBeltFirst + int32_t(n)))
        {
            log_error("[ab-bottombar] could not put \"%s\" in belt slot %d", c.belt[n].type.c_str(), int32_t(n));
            return false;
        }
    PlayScreen.SetFixtureState(c.frame, PlayScreen.IsControlOn(), PlayScreen.IsDemoMode());
    return true;
}

// What the rings and the boxes show, for the A/B's report.
bool WriteShown(const std::string& path)
{
    JsonOut json;
    json.Begin('{');
    json.FieldString("schema", "port.bottombar.v1");
    json.Key("pane").Begin('[').Value(g_bar->GetPosX()).Value(g_bar->GetPosY()).Value(g_bar->GetWidth())
        .Value(g_bar->GetHeight()).End(']');
    json.Field("frame", PlayScreen.GameFrame());
    json.Key("rings").Begin('[');
    for (int32_t ring = 1; ring <= kQuickSpells; ++ring)
    {
        char* talismans = Player->GetQuickSpell(ring);
        json.Begin('{').FieldString("talismans", talismans ? talismans : "")
            .Field("castable", talismans && *talismans && Player->HasTalismans(talismans) ? 1 : 0).End('}');
    }
    json.End(']');
    json.Key("boxes").Begin('[');
    for (int32_t box = 0; box < g_bar->Shelf().NumBoxes(); ++box)
    {
        TObjectInstance* item = Player->GetInventorySlot(kInvSlotBeltFirst + box);
        if (!item)
        {
            json.Null();
            continue;
        }
        json.Begin('{').FieldString("type", ToUtf8(item->GetName())).Field("amount", item->Amount())
            .Field("items", item->RealNumInventoryItems()).End('}');
    }
    json.End(']');
    json.End('}');
    std::ofstream out(path, std::ios::binary);
    out << json.str() << '\n';
    return bool(out);
}

SCase g_case;
bool g_caseRun = false;
double g_timeScale = 1.0;

void RunCase()
{
    g_timeScale = TTime::TimeScale();
    TTime::SetTimeScale(0.0);
    g_bar->Pulse();
    g_caseRun = true;
    if (!StartupAbOut.empty() && !WriteShown(StartupAbOut))
        log_error("[ab-bottombar] could not write %s", StartupAbOut.c_str());
    log_info("[ab-bottombar] pulsed at frame %lld", static_cast<long long>(TTime::FrameCount()));
}

}  // namespace

bool InitializeUIBottomBarMode()
{
    if (!HostBar(MapViewWidth(), true))
        return false;
    g_lastTick = TTime::LegacyFrameCount() - 1;
    return true;
}

bool InitializeUIBottomBarModeEmbedded()
{
    if (!HostBar(MapViewWidth(), false))
        return false;
    g_lastTick = TTime::LegacyFrameCount() - 1;
    return true;
}

// The test screen doesn't run the screen's pane pass, so the host does what
// TPlayScreen does for the bar: shows it with the lower panel, lays it out
// against the map view, and pulses it once a tick.
void RenderUIBottomBarMode()
{
    if (!g_bar)
        return;
    const bool open = GetHudState().bottomBarOpen != 0;
    if (open)
        g_bar->Place(0, DisplayHeight() - TBottomBarPane::kHeight, MapViewWidth());
    if (open == g_bar->IsHidden())
        open ? g_bar->Show() : g_bar->Hide();
    for (const int64_t now = TTime::LegacyFrameCount(); g_lastTick < now; ++g_lastTick)
        if (open)
            g_bar->Pulse();
}

void CloseUIBottomBarMode()
{
    UnhostBar();
}

bool InitializeABBottomBarMode()
{
    g_case = {};
    g_caseRun = false;
    if (!ParseCase(StartupAbCase, g_case))
    {
        log_error("[ab-bottombar] bad --ab-case \"%s\"", StartupAbCase.c_str());
        return false;
    }
    if (!UIDemoPlayer::Install() || !Player)
    {
        log_error("[ab-bottombar] no demo player");
        return false;
    }
    return ApplyCase(g_case) && HostBar(g_case.width, true);
}

void RenderABBottomBarMode()
{
    if (g_bar && !g_caseRun && TTime::LegacyFrameFraction() == 0.0)
        RunCase();
}

void CloseABBottomBarMode()
{
    if (g_caseRun)
        TTime::SetTimeScale(g_timeScale);
    g_caseRun = false;
    UnhostBar();
    UIDemoPlayer::Remove();
}
