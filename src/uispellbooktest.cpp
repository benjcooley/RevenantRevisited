// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uispellbooktest.cpp - --test=ui-spellbook: TSpellBookPane            *
// *************************************************************************
//
// See uispellbooktest.h.
//
// *************************************************************************

#include "uispellbooktest.h"

#include "display.h"
#include "font.h"
#include "hudstate.h"
#include "jsonout.h"
#include "logging.h"
#include "player.h"
#include "playscreen.h"
#include "revdefs.h"
#include "spellbook.h"
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
bool g_followHud = false;                   // shown only with the side panel's book tab

int32_t DisplayWidth() { return Display.Width() > 0 ? Display.Width() : WIDTH; }

bool HostSpellBook(bool backdrop, bool followHud)
{
    SpellBook.Resize(DisplayWidth() - TSpellBookPane::kWidth, 0, TSpellBookPane::kWidth, TSpellBookPane::kHeight);
    if (!SpellBook.Initialize() || !g_host.Host(SpellBook, backdrop))
    {
        log_error("[ui-spellbook] the spell book did not initialize");
        return false;
    }
    // The book takes clicks while the player has control.
    PlayScreen.SetFixtureState(PlayScreen.GameFrame(), true, PlayScreen.IsDemoMode());
    g_followHud = followHud;
    log_info("[ui-spellbook] pane at (%d, %d) %d x %d, %d spells", SpellBook.GetPosX(), SpellBook.GetPosY(),
             SpellBook.GetWidth(), SpellBook.GetHeight(), Player ? Player->NumKnownSpells() : 0);
    return true;
}

void UnhostSpellBook()
{
    g_host.Unhost();
    SpellBook.Close();
}

// ---- the A/B case ----------------------------------------------------------

struct SCase
{
    std::vector<std::string> spells;
    std::vector<std::pair<SAbItem, int32_t>> items;     // item, slot
    int32_t mana = 100;
    int32_t scroll = 0;
    int32_t pressed = -1;
};

// "spells=TAL,...;items=I@S|...;mana=N;scroll=N;pressed=R"
bool ParseCase(const std::string& text, SCase& c)
{
    for (const std::string& field : SplitAbField(text, ';'))
    {
        const size_t eq = field.find('=');
        const std::string key = field.substr(0, eq);
        const std::string value = eq == std::string::npos ? std::string() : field.substr(eq + 1);
        int32_t* number = key == "mana" ? &c.mana : key == "scroll" ? &c.scroll : key == "pressed" ? &c.pressed
                        : nullptr;
        if (number)
        {
            if (std::sscanf(value.c_str(), "%d", number) != 1)
                return false;
        }
        else if (key == "spells")
        {
            for (const std::string& spell : SplitAbField(value, ','))
                if (!spell.empty())
                    c.spells.push_back(spell);
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
    return true;
}

bool ApplyCase(const SCase& c)
{
    UIDemoPlayer::RemoveItems(Player, 0, kInvSlotLastCarried);
    for (const auto& [item, slot] : c.items)
        if (!item.Place(Player, slot))
        {
            log_error("[ab-spellbook] could not put \"%s\" in slot %d", item.type.c_str(), slot);
            return false;
        }
    while (Player->NumKnownSpells() > 0)
        Player->ForgetSpell(0);
    for (const std::string& spell : c.spells)
        Player->LearnSpell(spell.c_str());
    Player->SetMana(c.mana);
    SpellBook.Rebuild();
    SpellBook.ScrollTo(c.scroll);
    if (c.pressed >= 0)
    {
        int32_t x = 0, y = 0;
        if (!SpellBook.CirclePoint(c.pressed, x, y))
        {
            log_error("[ab-spellbook] no row %d to press", c.pressed);
            return false;
        }
        SpellBook.MouseClick(MB_LEFTDOWN, x, y);
        if (SpellBook.PressedRow() != c.pressed)
        {
            log_error("[ab-spellbook] row %d's circle isn't in the window to press", c.pressed);
            return false;
        }
    }
    return true;
}


// What the book shows and its text draws (pane-local), for the A/B.
bool WriteShown(const std::string& path, const std::vector<STextDraw>& text)
{
    JsonOut json;
    json.Begin('{');
    json.FieldString("schema", "port.spellbook.v1");
    json.Key("pane").Begin('[').Value(SpellBook.GetPosX()).Value(SpellBook.GetPosY()).Value(SpellBook.GetWidth())
        .Value(SpellBook.GetHeight()).End(']');
    json.Key("spells").Begin('[');
    for (int32_t i = 0; i < Player->NumKnownSpells(); ++i)
        json.String(Player->KnownSpell(i));
    json.End(']');
    json.Field("mana", Player->Mana());
    json.Field("scroll", SpellBook.ScrollPos());
    json.Field("pressed", SpellBook.PressedRow());
    WriteTextDraws(json, text);
    json.End('}');
    std::ofstream out(path, std::ios::binary);
    out << json.str() << '\n';
    return bool(out);
}

TAbClock g_clock;

}  // namespace

bool InitializeUISpellbookMode()
{
    return HostSpellBook(true, false);
}

bool InitializeUISpellbookModeEmbedded()
{
    return HostSpellBook(false, true);
}

// The test screen doesn't run the screen's pane pass, so the host does what
// TPlayScreen does for the book: shows it with the side panel's book tab and
// pulses it once a tick.
void RenderUISpellbookMode()
{
    if (!g_host.Pane())
        return;
    const SHudState& hud = GetHudState();
    const bool shown = !g_followHud || (hud.sidebarState == HUD_SIDEBAR_OPEN && hud.topSlot == HUD_TOP_BOOK);
    if (shown == SpellBook.IsHidden())
        shown ? SpellBook.Show() : SpellBook.Hide();
    g_host.PulseTicks();
}

void CloseUISpellbookMode()
{
    UnhostSpellBook();
}

bool InitializeABSpellBookMode()
{
    SCase c;
    if (!ParseCase(StartupAbCase, c))
    {
        log_error("[ab-spellbook] bad --ab-case \"%s\"", StartupAbCase.c_str());
        return false;
    }
    if (!UIDemoPlayer::Install() || !Player)
    {
        log_error("[ab-spellbook] no demo player");
        return false;
    }
    return HostSpellBook(true, false) && ApplyCase(c);
}

void RenderABSpellBookMode()
{
    if (!g_host.Pane() || !g_clock.Stop())
        return;
    std::vector<STextDraw> text;
    SetTextDrawRecorder(&text);
    SpellBook.SetDirty(true);
    SpellBook.Compose();
    SetTextDrawRecorder(nullptr);
    if (!StartupAbOut.empty() && !WriteShown(StartupAbOut, text))
        log_error("[ab-spellbook] could not write %s", StartupAbOut.c_str());
    log_info("[ab-spellbook] composed at frame %lld, %zu text draws", static_cast<long long>(TTime::FrameCount()),
             text.size());
}

void CloseABSpellBookMode()
{
    g_clock.Restore();
    UnhostSpellBook();
    UIDemoPlayer::Remove();
}
