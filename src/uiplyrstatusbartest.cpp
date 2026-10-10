// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiplyrstatusbartest.cpp - --test=ui-plyrstatusbar: TPlyrStatusBar    *
// *************************************************************************
//
// See uiplyrstatusbartest.h.
//
// *************************************************************************

#include "uiplyrstatusbartest.h"

#include "bitmap.h"
#include "jsonout.h"
#include "logging.h"
#include "player.h"
#include "renderer.h"
#include "screen.h"
#include "statusbar.h"
#include "testconfig.h"
#include "textencoding.h"
#include "time.h"
#include "uidemoplayer.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>

namespace {

// A flat backdrop under the screen's pane layer (the test screen draws no
// world).
class TBackdrop final : public THudDrawable
{
  public:
    TBackdrop(float r, float g, float b) : red(r), green(g), blue(b) {}
    void Draw() override { Renderer->FillScreen(red, green, blue, 1.0f); }

  private:
    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;
};

// Mid-toned for the demo, so the chips' edges and the text's black shadow
// read; black for the A/B, as retail's frame is cleared.
TBackdrop g_demoBackdrop(0.32f, 0.36f, 0.28f);
TBackdrop g_abBackdrop(0.0f, 0.0f, 0.0f);

std::unique_ptr<TPlyrStatusBar> g_statusbar;
THudDrawable* g_backdrop = nullptr;
int64_t g_lastTick = 0;

bool HostPane(THudDrawable& backdrop)
{
    g_statusbar = std::make_unique<TPlyrStatusBar>();
    if (!CurrentScreen || !g_statusbar->Initialize())
    {
        log_error("[ui-plyrstatusbar] the status bar did not initialize");
        g_statusbar.reset();
        return false;
    }
    CurrentScreen->AddPane(g_statusbar.get());
    g_backdrop = &backdrop;
    if (Renderer)
        Renderer->AddHud(g_backdrop, 0.0f);
    log_info("[ui-plyrstatusbar] pane at (%d, %d) %d x %d", g_statusbar->GetPosX(), g_statusbar->GetPosY(),
             g_statusbar->GetWidth(), g_statusbar->GetHeight());
    return true;
}

void UnhostPane()
{
    if (Renderer && g_backdrop)
        Renderer->RemoveHud(g_backdrop);
    g_backdrop = nullptr;
    if (!g_statusbar)
        return;
    if (CurrentScreen)
        CurrentScreen->RemovePane(g_statusbar.get());
    g_statusbar->Close();
    g_statusbar.reset();
}

// ---- the A/B case ----------------------------------------------------------

// Health, mana and fatigue as fractions of the character's maxima.
struct SFills
{
    double health = 1.0;
    double mana = 1.0;
    double fatigue = 1.0;
};

struct SCase
{
    SFills player;
    std::optional<SFills> target;       // none: the player isn't fighting
    int32_t ticks = 6;
};

bool ParseFills(const std::string& text, SFills& fills)
{
    return std::sscanf(text.c_str(), "%lf,%lf,%lf", &fills.health, &fills.mana, &fills.fatigue) == 3;
}

// "player=H,M,F;target=H,M,F|none;ticks=N"
bool ParseCase(const std::string& text, SCase& c)
{
    std::istringstream fields(text);
    for (std::string field; std::getline(fields, field, ';');)
    {
        const size_t eq = field.find('=');
        const std::string key = field.substr(0, eq);
        const std::string value = eq == std::string::npos ? std::string() : field.substr(eq + 1);
        if (key == "player")
        {
            if (!ParseFills(value, c.player))
                return false;
        }
        else if (key == "target")
        {
            c.target.reset();
            if (value != "none" && !ParseFills(value, c.target.emplace()))
                return false;
        }
        else if (key == "ticks")
        {
            if (std::sscanf(value.c_str(), "%d", &c.ticks) != 1 || c.ticks < 0)
                return false;
        }
        else
            return false;
    }
    return true;
}

void ApplyFills(TCharacter& character, const SFills& fills)
{
    character.SetHealth(int32_t(character.MaxHealth() * fills.health));
    character.SetMana(int32_t(character.MaxMana() * fills.mana));
    character.SetFatigue(int32_t(character.MaxFatigue() * fills.fatigue));
}

// A TBitmap's bytes as stored -- the header, the pixels, and the blocks its
// relative offsets point at -- in hex: the retail fixture loads them as is.
std::string BitmapHex(TBitmap& bitmap)
{
    const auto* base = reinterpret_cast<const uint8_t*>(&bitmap);
    const uint8_t* end = base + offsetof(TBitmapData, data8) + bitmap.datasize;
    const auto extend = [&](const OFFSET& block, uint32_t size) {
        if (size && block.offset)
            end = (std::max)(end, static_cast<const uint8_t*>(static_cast<void*>(block)) + size);
    };
    extend(bitmap.alias, bitmap.aliassize);
    extend(bitmap.alpha, bitmap.alphasize);
    extend(bitmap.zbuffer, bitmap.zbuffersize);
    extend(bitmap.normal, bitmap.normalsize);
    extend(bitmap.palette, bitmap.palettesize);

    static constexpr char kDigits[] = "0123456789abcdef";
    std::string hex;
    hex.reserve(size_t(end - base) * 2);
    for (const uint8_t* p = base; p < end; ++p)
    {
        hex += kDigits[*p >> 4];
        hex += kDigits[*p & 15];
    }
    return hex;
}

// What a chip shows of its character, in the retail fixture's case schema
// (tools/retail_runtime/slots/hud/plyrstatusbar.py).
void WriteCharacter(JsonOut& json, const char* key, TCharacter* character)
{
    json.Key(key);
    if (!character)
    {
        json.Null();
        return;
    }
    const bool player = character->ObjClass() == OBJCLASS_PLAYER;
    json.Begin('{');
    json.FieldString("name", ToUtf8(character->GetName() ? character->GetName() : ""));
    json.FieldString("class", player ? "player" : "character");
    json.Field("level", player ? static_cast<TPlayer*>(character)->Level() : 0);
    json.Field("health", character->Health());
    json.Field("max_health", character->MaxHealth());
    json.Field("mana", character->Mana());
    json.Field("max_mana", character->MaxMana());
    json.Field("fatigue", character->Fatigue());
    json.Field("max_fatigue", character->MaxFatigue());
    if (TBitmap* portrait = character->InventoryImage())
        json.Key("portrait").Begin('{').FieldString("tbitmap", BitmapHex(*portrait)).End('}');
    json.End('}');
}

bool WriteShown(const std::string& path, const SCase& c, TCharacter* player, TCharacter* target)
{
    JsonOut json;
    json.Begin('{');
    json.FieldString("schema", "port.plyrstatusbar.v1");
    json.Key("pane").Begin('[').Value(g_statusbar->GetPosX()).Value(g_statusbar->GetPosY())
        .Value(g_statusbar->GetWidth()).Value(g_statusbar->GetHeight()).End(']');
    json.Field("ticks", c.ticks);
    WriteCharacter(json, "player", player);
    WriteCharacter(json, "target", target);
    json.End('}');
    std::ofstream out(path, std::ios::binary);
    out << json.str() << '\n';
    return bool(out);
}

// The A/B case waits for a frame on a tick boundary (--snapstep=0.03125
// makes every fourth one), then stops the clock and runs the pane.
SCase g_case;
bool g_caseRun = false;
double g_timeScale = 1.0;

void RunCase()
{
    g_timeScale = TTime::TimeScale();
    TTime::SetTimeScale(0.0);
    for (int32_t tick = 0; tick < g_case.ticks; ++tick)
        g_statusbar->Pulse();
    g_caseRun = true;

    TCharacter* target = g_case.target ? UIDemoPlayer::Opponent() : nullptr;
    if (!StartupAbOut.empty() && !WriteShown(StartupAbOut, g_case, Player, target))
        log_error("[ab-plyrstatusbar] could not write %s", StartupAbOut.c_str());
    log_info("[ab-plyrstatusbar] ran %d tick%s at frame %lld", g_case.ticks, g_case.ticks == 1 ? "" : "s",
             static_cast<long long>(TTime::FrameCount()));
}

}  // namespace

bool InitializeUIPlyrStatusBarMode()
{
    if (!HostPane(g_demoBackdrop))
        return false;
    g_lastTick = TTime::LegacyFrameCount() - 1;
    return true;
}

// The test screen doesn't run the screen's pane pass, so the host pulses the
// pane once a tick, as TPlayScreen::Pulse does.
void RenderUIPlyrStatusBarMode()
{
    if (!g_statusbar)
        return;
    for (const int64_t now = TTime::LegacyFrameCount(); g_lastTick < now; ++g_lastTick)
        g_statusbar->Pulse();
}

void CloseUIPlyrStatusBarMode()
{
    UnhostPane();
}

bool InitializeABPlyrStatusBarMode()
{
    g_case = {};
    g_caseRun = false;
    if (!ParseCase(StartupAbCase, g_case))
    {
        log_error("[ab-plyrstatusbar] bad --ab-case \"%s\"", StartupAbCase.c_str());
        return false;
    }
    TPlayer* opponent = UIDemoPlayer::Install() ? UIDemoPlayer::Opponent() : nullptr;
    if (!Player || !opponent)
    {
        log_error("[ab-plyrstatusbar] no demo player and opponent");
        return false;
    }
    ApplyFills(*Player, g_case.player);
    if (g_case.target)
        ApplyFills(*opponent, *g_case.target);
    Player->SetFighting(g_case.target ? opponent : nullptr);
    return HostPane(g_abBackdrop);
}

void RenderABPlyrStatusBarMode()
{
    if (g_statusbar && !g_caseRun && TTime::LegacyFrameFraction() == 0.0)
        RunCase();
}

void CloseABPlyrStatusBarMode()
{
    if (g_caseRun)
        TTime::SetTimeScale(g_timeScale);
    g_caseRun = false;
    UnhostPane();
    UIDemoPlayer::Remove();
}
