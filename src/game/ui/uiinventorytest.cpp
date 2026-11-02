// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiinventorytest.cpp - --test=ui-inventory: TInventory                *
// *************************************************************************
//
// See uiinventorytest.h.
//
// *************************************************************************

#include "game/ui/uiinventorytest.h"

#include "engine/render/display.h"
#include "game/ui/hudstate.h"
#include "game/object/inventory.h"
#include "engine/core/jsonout.h"
#include "engine/core/logging.h"
#include "game/object/object.h"
#include "game/object/player.h"
#include "game/ui/playscreen.h"
#include "engine/core/revdefs.h"
#include "game/app/revenant.h"
#include "game/app/testconfig.h"
#include "engine/core/textencoding.h"
#include "engine/core/revtime.h"
#include "game/ui/uidemoplayer.h"
#include "game/ui/uipanehost.h"

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

namespace {

constexpr int32_t kLowerSlotTop = 306;      // the side panel's upper slot is 306 tall

TTestPaneHost g_host;
bool g_followHud = false;                   // shown only with the side panel's inventory

int32_t DisplayWidth() { return Display.Width() > 0 ? Display.Width() : WIDTH; }

bool HostInventory(bool backdrop, bool followHud)
{
    Inventory.Resize(DisplayWidth() - TInventory::kWidth, kLowerSlotTop, TInventory::kWidth, TInventory::kHeight);
    if (!Inventory.Initialize() || !g_host.Host(Inventory, backdrop))
    {
        log_error("[ui-inventory] the inventory did not initialize");
        return false;
    }
    Inventory.SetContainer(Player);
    g_followHud = followHud;
    log_info("[ui-inventory] pane at (%d, %d) %d x %d", Inventory.GetPosX(), Inventory.GetPosY(),
             Inventory.GetWidth(), Inventory.GetHeight());
    return true;
}

void UnhostInventory()
{
    g_host.Unhost();
    Inventory.SetContainer(nullptr);
    Inventory.Close();
}

// ---- the A/B case ----------------------------------------------------------

struct SCase
{
    std::vector<std::pair<SAbItem, int32_t>> items;     // item, slot
    int32_t bag = -1;
    int32_t first = 0;
    int32_t frame = 0;
};

// "items=I@S|I@S|...;bag=S;first=N;frame=N"
bool ParseCase(const std::string& text, SCase& c)
{
    for (const std::string& field : SplitAbField(text, ';'))
    {
        const size_t eq = field.find('=');
        const std::string key = field.substr(0, eq);
        const std::string value = eq == std::string::npos ? std::string() : field.substr(eq + 1);
        int32_t* number = key == "bag" ? &c.bag : key == "first" ? &c.first : key == "frame" ? &c.frame : nullptr;
        if (number)
        {
            if (std::sscanf(value.c_str(), "%d", number) != 1)
                return false;
        }
        else if (key == "items")
        {
            for (const std::string& entry : SplitAbField(value, '|'))
            {
                const size_t at = entry.rfind('@');
                int32_t slot = -1;
                if (at == std::string::npos || std::sscanf(entry.c_str() + at + 1, "%d", &slot) != 1
                    || slot < 0 || slot > kInvSlotLastCarried)
                    return false;
                auto& item = c.items.emplace_back();
                item.second = slot;
                if (!SAbItem::Parse(entry.substr(0, at), item.first))
                    return false;
            }
        }
        else
            return false;
    }
    return c.first >= 0 && c.first % TInventory::kRows == 0 && c.first <= TInventory::kLastFirstSlot;
}

bool ApplyCase(const SCase& c)
{
    UIDemoPlayer::RemoveItems(Player, 0, kInvSlotLastCarried);
    for (const auto& [item, slot] : c.items)
        if (!item.Place(Player, slot))
        {
            log_error("[ab-inventory] could not put \"%s\" in slot %d", item.type.c_str(), slot);
            return false;
        }
    TObjectInstance* container = Player;
    if (c.bag >= 0 && !(container = Player->GetInventorySlot(c.bag)))
    {
        log_error("[ab-inventory] no bag in slot %d", c.bag);
        return false;
    }
    Inventory.SetContainer(container);
    Inventory.Scroll(c.first / TInventory::kRows);
    PlayScreen.SetFixtureState(c.frame, PlayScreen.IsControlOn(), PlayScreen.IsDemoMode());
    return true;
}

// What the pane shows, for the A/B's report.
bool WriteShown(const std::string& path)
{
    JsonOut json;
    json.Begin('{');
    json.FieldString("schema", "port.inventory.v1");
    json.Key("pane").Begin('[').Value(Inventory.GetPosX()).Value(Inventory.GetPosY()).Value(Inventory.GetWidth())
        .Value(Inventory.GetHeight()).End(']');
    json.Field("frame", PlayScreen.GameFrame());
    json.Field("first", Inventory.FirstSlot());
    json.Field("gold", Player->GetInventoryAmount("Gold"));
    json.Key("cells").Begin('[');
    TObjectInstance* container = Inventory.GetContainer();
    for (int32_t n = 0; n < TInventory::kColumns * TInventory::kRows; ++n)
    {
        TObjectInstance* item = container ? container->GetInventorySlot(Inventory.FirstSlot() + n) : nullptr;
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

TAbClock g_clock;

}  // namespace

bool InitializeUIInventoryMode()
{
    return HostInventory(true, false);
}

bool InitializeUIInventoryModeEmbedded()
{
    return HostInventory(false, true);
}

// The test screen doesn't run the screen's pane pass, so the host does what
// TPlayScreen does for the pane: shows it with the side panel's inventory
// and pulses it once a tick.
void RenderUIInventoryMode()
{
    if (!g_host.Pane())
        return;
    const SHudState& hud = GetHudState();
    const bool shown = !g_followHud || (hud.sidebarState == HUD_SIDEBAR_OPEN && hud.bottomSlot == HUD_BOT_INV);
    if (shown == Inventory.IsHidden())
        shown ? Inventory.Show() : Inventory.Hide();
    g_host.PulseTicks();
}

void CloseUIInventoryMode()
{
    UnhostInventory();
}

bool InitializeABInventoryMode()
{
    SCase c;
    if (!ParseCase(StartupAbCase, c))
    {
        log_error("[ab-inventory] bad --ab-case \"%s\"", StartupAbCase.c_str());
        return false;
    }
    if (!UIDemoPlayer::Install() || !Player)
    {
        log_error("[ab-inventory] no demo player");
        return false;
    }
    return HostInventory(true, false) && ApplyCase(c);
}

void RenderABInventoryMode()
{
    if (!g_host.Pane() || !g_clock.Stop())
        return;
    Inventory.Pulse();
    if (!StartupAbOut.empty() && !WriteShown(StartupAbOut))
        log_error("[ab-inventory] could not write %s", StartupAbOut.c_str());
    log_info("[ab-inventory] pulsed at frame %lld", static_cast<long long>(TTime::FrameCount()));
}

void CloseABInventoryMode()
{
    g_clock.Restore();
    UnhostInventory();
    UIDemoPlayer::Remove();
}
