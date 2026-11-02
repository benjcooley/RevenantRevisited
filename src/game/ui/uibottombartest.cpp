// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uibottombartest.cpp - --test=ui-bottombar: TBottomBarPane            *
// *************************************************************************
//
// See uibottombartest.h.
//
// *************************************************************************

#include "game/ui/uibottombartest.h"

#include "game/ui/bottombar.h"
#include "engine/render/display.h"
#include "game/ui/hudstate.h"
#include "engine/core/jsonout.h"
#include "engine/core/logging.h"
#include "game/object/object.h"
#include "game/object/player.h"
#include "game/ui/playscreen.h"
#include "engine/core/revdefs.h"
#include "game/app/testconfig.h"
#include "engine/core/textencoding.h"
#include "engine/core/revtime.h"
#include "game/ui/uidemoplayer.h"
#include "game/ui/uipanehost.h"

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

extern TObjectClass TalismanClass;

namespace {

constexpr int32_t kSidePanelWidth = 0xbc;   // TPlayScreen::Pulse 0x0047b4d0
constexpr int32_t kQuickSpells = 4;

std::unique_ptr<TBottomBarPane> g_bar;
TTestPaneHost g_host;

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
    if (!g_bar->Initialize() || !g_host.Host(*g_bar, backdrop))
    {
        log_error("[ui-bottombar] the bottom bar did not initialize");
        g_bar.reset();
        return false;
    }
    log_info("[ui-bottombar] bar at (%d, %d) %d x %d, %d boxes", g_bar->GetPosX(), g_bar->GetPosY(),
             g_bar->GetWidth(), g_bar->GetHeight(), g_bar->Shelf().NumBoxes());
    return true;
}

void UnhostBar()
{
    g_host.Unhost();
    if (!g_bar)
        return;
    g_bar->Close();
    g_bar.reset();
}

// ---- the A/B case ----------------------------------------------------------

struct SCase
{
    int32_t width = WIDTH - kSidePanelWidth;
    std::string spells[kQuickSpells];
    std::vector<std::string> castable;
    std::vector<SAbItem> belt;
    int32_t frame = 0;
};

// "width=W;spells=A,B,C,D;castable=A,B;belt=I|I|...;frame=N"
bool ParseCase(const std::string& text, SCase& c)
{
    for (const std::string& field : SplitAbField(text, ';'))
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
            const std::vector<std::string> spells = SplitAbField(value, ',');
            if (spells.size() > kQuickSpells)
                return false;
            for (size_t i = 0; i < spells.size(); ++i)
                c.spells[i] = spells[i];
        }
        else if (key == "castable")
            c.castable = value.empty() ? std::vector<std::string>() : SplitAbField(value, ',');
        else if (key == "belt")
        {
            for (const std::string& slot : SplitAbField(value, '|'))
                if (!SAbItem::Parse(slot, c.belt.emplace_back()))
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
        if (const TObjectDef* info = TalismanClass.GetObjType(type))
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
        if (!c.belt[n].Place(Player, kInvSlotBeltFirst + int32_t(n)))
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
        const char* talismans = Player->GetQuickSpell(ring);
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
TAbClock g_clock;

}  // namespace

bool InitializeUIBottomBarMode()
{
    return HostBar(MapViewWidth(), true);
}

bool InitializeUIBottomBarModeEmbedded()
{
    return HostBar(MapViewWidth(), false);
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
    g_host.PulseTicks();
}

void CloseUIBottomBarMode()
{
    UnhostBar();
}

bool InitializeABBottomBarMode()
{
    g_case = {};
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
    if (!g_bar || !g_clock.Stop())
        return;
    g_bar->Pulse();
    if (!StartupAbOut.empty() && !WriteShown(StartupAbOut))
        log_error("[ab-bottombar] could not write %s", StartupAbOut.c_str());
    log_info("[ab-bottombar] pulsed at frame %lld", static_cast<long long>(TTime::FrameCount()));
}

void CloseABBottomBarMode()
{
    g_clock.Restore();
    UnhostBar();
    UIDemoPlayer::Remove();
}
