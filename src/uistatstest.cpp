// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uistatstest.cpp - --test=ui-stats: TStatPane                         *
// *************************************************************************
//
// See uistatstest.h.
//
// *************************************************************************

#include "uistatstest.h"

#include "display.h"
#include "font.h"
#include "hudstate.h"
#include "jsonout.h"
#include "logging.h"
#include "player.h"
#include "playscreen.h"
#include "revdefs.h"
#include "statpane.h"
#include "testconfig.h"
#include "textencoding.h"
#include "time.h"
#include "uidemoplayer.h"
#include "uipanehost.h"

#include <cstdio>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

namespace {

TTestPaneHost g_host;
bool g_followHud = false;                   // shown only with the side panel's stats tab

int32_t DisplayWidth() { return Display.Width() > 0 ? Display.Width() : WIDTH; }

bool HostStatPane(bool backdrop, bool followHud)
{
    StatPane.Resize(DisplayWidth() - TStatPane::kWidth, 0, TStatPane::kWidth, TStatPane::kHeight);
    if (!StatPane.Initialize() || !g_host.Host(StatPane, backdrop))
    {
        log_error("[ui-stats] the stat pane did not initialize");
        return false;
    }
    PlayScreen.SetFixtureState(PlayScreen.GameFrame(), true, PlayScreen.IsDemoMode());
    g_followHud = followHud;
    log_info("[ui-stats] pane at (%d, %d) %d x %d", StatPane.GetPosX(), StatPane.GetPosY(), StatPane.GetWidth(),
             StatPane.GetHeight());
    return true;
}

void UnhostStatPane()
{
    g_host.Unhost();
    StatPane.Close();
}

// ---- the A/B case ----------------------------------------------------------

struct SCase
{
    std::vector<std::pair<std::string, int32_t>> stats;
    int32_t levelup[3] = { 0, 0, 0 };       // the level-up picks and the one made last
    std::string format = "page1";
    std::vector<std::pair<int32_t, int32_t>> clicks;    // pane points clicked, in order
};

// "stats=Name:N,...;levelup=A,B,L;format=page1"
bool ParseCase(const std::string& text, SCase& c)
{
    for (const std::string& field : SplitAbField(text, ';'))
    {
        const size_t eq = field.find('=');
        const std::string key = field.substr(0, eq);
        const std::string value = eq == std::string::npos ? std::string() : field.substr(eq + 1);
        if (key == "stats")
        {
            for (const std::string& stat : SplitAbField(value, ','))
            {
                const size_t colon = stat.find(':');
                int32_t number = 0;
                if (colon == std::string::npos || std::sscanf(stat.c_str() + colon + 1, "%d", &number) != 1)
                    return false;
                c.stats.emplace_back(stat.substr(0, colon), number);
            }
        }
        else if (key == "levelup")
        {
            if (std::sscanf(value.c_str(), "%d,%d,%d", &c.levelup[0], &c.levelup[1], &c.levelup[2]) != 3)
                return false;
        }
        else if (key == "format")
            c.format = value;
        else if (key == "click")
        {
            for (const std::string& point : SplitAbField(value, '|'))
            {
                int32_t x = 0, y = 0;
                if (std::sscanf(point.c_str(), "%d,%d", &x, &y) != 2)
                    return false;
                c.clicks.emplace_back(x, y);
            }
        }
        else
            return false;
    }
    return true;
}

bool ApplyCase(const SCase& c)
{
    for (const auto& [name, value] : c.stats)
    {
        const int32_t id = Player->FindObjStat(name.c_str());
        if (id < 0)
        {
            log_error("[ab-statpane] the player has no stat \"%s\"", name.c_str());
            return false;
        }
        Player->SetObjStat(id, value);
    }
    Player->SetLevelUpPicks({ c.levelup[0], c.levelup[1], c.levelup[2] });
    StatPane.SetObject(Player);
    StatPane.SetFormat(c.format.c_str());
    StatPane.Pulse();
    // Each click as the pointer makes it, the hot spots of the block shown.
    for (const auto& [x, y] : c.clicks)
    {
        StatPane.MouseClick(MB_LEFTDOWN, x, y);
        StatPane.MouseClick(MB_LEFTUP, x, y);
        StatPane.Pulse();
    }
    return true;
}

const char* AlignName(ETextAlign align)
{
    return align == ETextAlign::Center ? "center" : align == ETextAlign::Right ? "right" : "left";
}

// What the pane shows, the inputs its fields read, and its text draws.
bool WriteShown(const std::string& path, const std::vector<STextDraw>& text)
{
    char field[64];
    auto fieldtext = [&field](const char* name) {
        field[0] = 0;
        Player->GetFieldText(name, field, sizeof(field));
        return std::string(field);
    };
    JsonOut json;
    json.Begin('{');
    json.FieldString("schema", "port.statpane.v1");
    json.Key("pane").Begin('[').Value(StatPane.GetPosX()).Value(StatPane.GetPosY()).Value(StatPane.GetWidth())
        .Value(StatPane.GetHeight()).End(']');
    json.FieldString("format", StatPane.Format());
    json.FieldString("name", Player->GetTypeName());
    json.FieldString("class", ToUtf8(fieldtext("class").c_str()));
    json.Key("levelup").Begin('[').Value(Player->LevelUpPick(0)).Value(Player->LevelUpPick(1))
        .Value(Player->LevelUpPick(2)).End(']');
    json.Key("objstats").Begin('{');
    for (int32_t id = 0; id < Player->NumObjStats(); ++id)
        json.Field(Player->ObjStatName(id), Player->GetObjStat(id));
    json.End('}');
    json.Field("max_health", Player->MaxHealth()).Field("max_fatigue", Player->MaxFatigue())
        .Field("max_mana", Player->MaxMana());
    // The getters the fields read (TCharacter::GetFieldText), by retail
    // vtable slot, for the retail side to answer with.
    json.Key("combat").Begin('{');
    const std::pair<const char*, int32_t> getters[] = {
        { "0x260", Player->DmgResMisc() }, { "0x2b0", Player->DamageMod() }, { "0x2bc", Player->ArmorValue() },
        { "0x2c0", Player->Defense() }, { "0x2c4", Player->Offense() }, { "0x2cc", Player->WeaponType() },
        { "0x2d0", Player->WeaponDamage() }, { "0x2e8", Player->StealthMod() }, { "0x2ec", Player->LuckMod() },
        { "0x3d4", Player->EdgeBonus() },
    };
    for (const auto& [slot, value] : getters)
        json.Field(slot, value);
    json.End('}');
    json.Key("text").Begin('[');
    for (const STextDraw& draw : text)
    {
        json.Begin('{');
        json.FieldString("text", ToUtf8(draw.text.c_str()));
        json.FieldString("face", draw.face);
        json.Field("x", draw.x).Field("y", draw.y).Field("w", draw.w).Field("h", draw.h);
        json.FieldString("align", AlignName(draw.align));
        json.Key("color").Begin('[').Value(draw.r).Value(draw.g).Value(draw.b).End(']');
        json.FieldBool("shadowed", draw.shadowed).FieldBool("wrapped", draw.wrapped);
        json.Key("lines").Begin('[');
        for (const std::string& line : draw.lines)
            json.String(ToUtf8(line.c_str()));
        json.End(']');
        json.End('}');
    }
    json.End(']');
    json.End('}');
    std::ofstream out(path, std::ios::binary);
    out << json.str() << '\n';
    return bool(out);
}

TAbClock g_clock;

}  // namespace

bool InitializeUIStatsMode()
{
    return HostStatPane(true, false);
}

bool InitializeUIStatsModeEmbedded()
{
    return HostStatPane(false, true);
}

// The test screen doesn't run the screen's pane pass, so the host does what
// TPlayScreen does for the pane: shows it with the side panel's stats tab
// and pulses it once a tick.
void RenderUIStatsMode()
{
    if (!g_host.Pane())
        return;
    const SHudState& hud = GetHudState();
    const bool shown = !g_followHud || (hud.sidebarState == HUD_SIDEBAR_OPEN && hud.topSlot == HUD_TOP_STATS);
    if (shown == StatPane.IsHidden())
        shown ? StatPane.Show() : StatPane.Hide();
    g_host.PulseTicks();
}

void CloseUIStatsMode()
{
    UnhostStatPane();
}

bool InitializeABStatPaneMode()
{
    SCase c;
    if (!ParseCase(StartupAbCase, c))
    {
        log_error("[ab-statpane] bad --ab-case \"%s\"", StartupAbCase.c_str());
        return false;
    }
    if (!UIDemoPlayer::Install() || !Player)
    {
        log_error("[ab-statpane] no demo player");
        return false;
    }
    return HostStatPane(true, false) && ApplyCase(c);
}

void RenderABStatPaneMode()
{
    if (!g_host.Pane() || !g_clock.Stop())
        return;
    std::vector<STextDraw> text;
    SetTextDrawRecorder(&text);
    StatPane.SetDirty(true);
    StatPane.Compose();
    SetTextDrawRecorder(nullptr);
    if (!StartupAbOut.empty() && !WriteShown(StartupAbOut, text))
        log_error("[ab-statpane] could not write %s", StartupAbOut.c_str());
    log_info("[ab-statpane] composed at frame %lld, %zu text draws", static_cast<long long>(TTime::FrameCount()),
             text.size());
}

void CloseABStatPaneMode()
{
    g_clock.Restore();
    UnhostStatPane();
    UIDemoPlayer::Remove();
}
