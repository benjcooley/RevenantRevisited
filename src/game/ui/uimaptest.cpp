// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uimaptest.cpp - --test=ui-map: TAutoMap                              *
// *************************************************************************
//
// See uimaptest.h.
//
// *************************************************************************

#include "game/ui/uimaptest.h"

#include "game/world/automap.h"
#include "engine/render/display.h"
#include "game/ui/hudstate.h"
#include "engine/core/jsonout.h"
#include "engine/core/logging.h"
#include "engine/core/revdefs.h"
#include "game/app/revenant.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include "game/ui/uipanehost.h"

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>

namespace {

constexpr int32_t kLowerSlotTop = 306;      // the side panel's upper slot is 306 tall

// REVSYNC: 0x0052c1a0 -- inside "+" (pane (8, 3)) and "-" (pane (144, 3)).
constexpr int32_t kPlusX = 8 + 16, kMinusX = 0x90 + 16, kButtonY = 3 + 16;

TTestPaneHost g_host;
bool g_followHud = false;                   // shown only with the side panel's map tab

int32_t DisplayWidth() { return Display.Width() > 0 ? Display.Width() : WIDTH; }

bool HostAutoMap(bool backdrop, bool followHud)
{
    AutoMap.Resize(DisplayWidth() - TAutoMap::kWidth, kLowerSlotTop, TAutoMap::kWidth, TAutoMap::kHeight);
    if (!AutoMap.Initialize() || !g_host.Host(AutoMap, backdrop))
    {
        log_error("[ui-map] the automap did not initialize");
        return false;
    }
    g_followHud = followHud;
    log_info("[ui-map] pane at (%d, %d) %d x %d", AutoMap.GetPosX(), AutoMap.GetPosY(), AutoMap.GetWidth(),
             AutoMap.GetHeight());
    return true;
}

// ---- the A/B case ----------------------------------------------------------

struct SCase
{
    int32_t plus = 0;
    int32_t minus = 0;
};

// "plus=N;minus=N"
bool ParseCase(const std::string& text, SCase& c)
{
    for (const std::string& field : SplitAbField(text, ';'))
    {
        const size_t eq = field.find('=');
        const std::string key = field.substr(0, eq);
        const std::string value = eq == std::string::npos ? std::string() : field.substr(eq + 1);
        int32_t* number = key == "plus" ? &c.plus : key == "minus" ? &c.minus : nullptr;
        if (!number || std::sscanf(value.c_str(), "%d", number) != 1)
            return false;
    }
    return true;
}

bool WriteShown(const std::string& path)
{
    JsonOut json;
    json.Begin('{');
    json.FieldString("schema", "port.automap.v1");
    json.Key("pane").Begin('[').Value(AutoMap.GetPosX()).Value(AutoMap.GetPosY()).Value(AutoMap.GetWidth())
        .Value(AutoMap.GetHeight()).End(']');
    json.Field("zoom", AutoMap.Zoom());
    json.Field("plus", AutoMap.ZoomInHeld() ? 1 : 0);
    json.Field("minus", AutoMap.ZoomOutHeld() ? 1 : 0);
    json.End('}');
    std::ofstream out(path, std::ios::binary);
    out << json.str() << '\n';
    return bool(out);
}

TAbClock g_clock;
SCase g_case;

}  // namespace

bool InitializeUIMapMode()
{
    return HostAutoMap(true, false);
}

bool InitializeUIMapModeEmbedded()
{
    return HostAutoMap(false, true);
}

// The test screen doesn't run the screen's pane pass, so the host does what
// TPlayScreen does for the pane: shows it with the side panel's map tab and
// pulses it once a tick.
void RenderUIMapMode()
{
    if (!g_host.Pane())
        return;
    const SHudState& hud = GetHudState();
    const bool shown = !g_followHud || (hud.sidebarState == HUD_SIDEBAR_OPEN && hud.bottomSlot == HUD_BOT_MAP);
    if (shown == AutoMap.IsHidden())
        shown ? AutoMap.Show() : AutoMap.Hide();
    g_host.PulseTicks();
}

void CloseUIMapMode()
{
    g_host.Unhost();
    AutoMap.Close();
}

bool InitializeABAutoMapMode()
{
    if (!ParseCase(StartupAbCase, g_case))
    {
        log_error("[ab-automap] bad --ab-case \"%s\"", StartupAbCase.c_str());
        return false;
    }
    return HostAutoMap(true, false);
}

void RenderABAutoMapMode()
{
    if (!g_host.Pane() || !g_clock.Stop())
        return;
    if (g_case.plus)
        AutoMap.MouseClick(MB_LEFTDOWN, kPlusX, kButtonY);
    if (g_case.minus)
        AutoMap.MouseClick(MB_LEFTDOWN, kMinusX, kButtonY);
    if (!StartupAbOut.empty() && !WriteShown(StartupAbOut))
        log_error("[ab-automap] could not write %s", StartupAbOut.c_str());
    log_info("[ab-automap] composed at frame %lld", static_cast<long long>(TTime::FrameCount()));
}

void CloseABAutoMapMode()
{
    g_clock.Restore();
    g_host.Unhost();
    AutoMap.Close();
}
