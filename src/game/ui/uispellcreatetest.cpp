// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uispellcreatetest.cpp - --test=ui-spellcreate: TSpellPane            *
// *************************************************************************
//
// See uispellcreatetest.h.
//
// *************************************************************************

#include "game/ui/uispellcreatetest.h"

#include "engine/render/display.h"
#include "game/ui/hudstate.h"
#include "engine/core/jsonout.h"
#include "engine/core/logging.h"
#include "game/object/object.h"
#include "game/object/player.h"
#include "game/ui/playscreen.h"
#include "engine/core/revdefs.h"
#include "game/app/revenant.h"
#include "game/ui/spellpane.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include "game/ui/uidemoplayer.h"
#include "game/ui/uipanehost.h"

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

extern TObjectClass TalismanClass;

namespace {

constexpr int32_t kLowerSlotTop = 306;      // the side panel's upper slot is 306 tall
constexpr const char* kPouchType = "Spell Pouch";

TTestPaneHost g_host;
bool g_followHud = false;                   // shown only with the side panel's spell tab

int32_t DisplayWidth() { return Display.Width() > 0 ? Display.Width() : WIDTH; }

// A pouch of one of each talisman, and a second Sun and Moon, for a player
// who carries none: enough to put any spell together.
bool GiveDemoPouch()
{
    if (!Player || Player->FindObjInventory(kPouchType))
        return true;
    TObjectInstance* pouch = UIDemoPlayer::AddItem(Player, kPouchType, -1);
    if (!pouch)
        return false;
    for (int32_t type = 0; type < TalismanClass.NumTypes(); ++type)
        UIDemoPlayer::AddItem(pouch, TalismanClass.GetObjType(type)->name, -1);
    UIDemoPlayer::AddItem(pouch, "Sun", -1);
    UIDemoPlayer::AddItem(pouch, "Moon", -1);
    return true;
}

bool HostSpellPane(bool backdrop, bool followHud)
{
    SpellPane.Resize(DisplayWidth() - TSpellPane::kWidth, kLowerSlotTop, TSpellPane::kWidth, TSpellPane::kHeight);
    if (!SpellPane.Initialize() || !g_host.Host(SpellPane, backdrop))
    {
        log_error("[ui-spellcreate] the spell pane did not initialize");
        return false;
    }
    // The pane takes clicks while the player has control.
    PlayScreen.SetFixtureState(PlayScreen.GameFrame(), true, PlayScreen.IsDemoMode());
    g_followHud = followHud;
    log_info("[ui-spellcreate] pane at (%d, %d) %d x %d", SpellPane.GetPosX(), SpellPane.GetPosY(),
             SpellPane.GetWidth(), SpellPane.GetHeight());
    return true;
}

void UnhostSpellPane()
{
    g_host.Unhost();
    SpellPane.Close();
}

// ---- the A/B case ----------------------------------------------------------

struct SCase
{
    std::vector<std::pair<SAbItem, int32_t>> items;     // item, slot
    std::string spell;
    int32_t names = 1;
    int32_t first = 0;
    int32_t pressed = -1;
    int32_t invoke = 0;
};

// "items=I@S|...;spell=TAL;names=N;first=N;pressed=T;invoke=N"
bool ParseCase(const std::string& text, SCase& c)
{
    for (const std::string& field : SplitAbField(text, ';'))
    {
        const size_t eq = field.find('=');
        const std::string key = field.substr(0, eq);
        const std::string value = eq == std::string::npos ? std::string() : field.substr(eq + 1);
        int32_t* number = key == "names" ? &c.names : key == "first" ? &c.first : key == "pressed" ? &c.pressed
                        : key == "invoke" ? &c.invoke : nullptr;
        if (number)
        {
            if (std::sscanf(value.c_str(), "%d", number) != 1)
                return false;
        }
        else if (key == "spell")
            c.spell = value;
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
    // The arrows move the first line two talismans at a time.
    return c.first >= 0 && c.first % 2 == 0 && c.spell.size() < MAXTALISMANLEN;
}

// Presses talisman `type` where the pane shows it, as the pointer would.
bool PressTalisman(int32_t type)
{
    for (int32_t y = 0; y < TSpellPane::kHeight; ++y)
        for (int32_t x = 0; x < TSpellPane::kWidth; ++x)
            if (SpellPane.TalismanAt(x, y) == type)
            {
                SpellPane.MouseClick(MB_LEFTDOWN, x, y);
                return SpellPane.PressedTalisman() == type;
            }
    return false;
}

bool ApplyCase(const SCase& c)
{
    UIDemoPlayer::RemoveItems(Player, 0, kInvSlotLastCarried);
    for (const auto& [item, slot] : c.items)
        if (!item.Place(Player, slot))
        {
            log_error("[ab-spellpane] could not put \"%s\" in slot %d", item.type.c_str(), slot);
            return false;
        }
    Player->SetQuickSpell(QSPELL_CONSTRUCT, c.spell.c_str());
    if (SpellPane.ShowNames() != (c.names != 0))
        SpellPane.ToggleNames();
    SpellPane.Pulse();
    SpellPane.Scroll(c.first / 2);
    if (c.pressed >= 0 && !PressTalisman(c.pressed))
    {
        log_error("[ab-spellpane] talisman %d isn't shown to press", c.pressed);
        return false;
    }
    if (c.invoke)
        SpellPane.Button(0)->SetState(true);
    return true;
}

// What the pane shows, for the A/B's report.
bool WriteShown(const std::string& path)
{
    JsonOut json;
    json.Begin('{');
    json.FieldString("schema", "port.spellpane.v1");
    json.Key("pane").Begin('[').Value(SpellPane.GetPosX()).Value(SpellPane.GetPosY()).Value(SpellPane.GetWidth())
        .Value(SpellPane.GetHeight()).End(']');
    json.FieldString("spell", Player->GetQuickSpell(QSPELL_CONSTRUCT));
    json.Field("names", SpellPane.ShowNames() ? 1 : 0);
    json.Field("first", SpellPane.FirstLine());
    json.Field("pressed", SpellPane.PressedTalisman());
    json.Field("invoke", SpellPane.Button(0)->GetState() ? 1 : 0);
    json.End('}');
    std::ofstream out(path, std::ios::binary);
    out << json.str() << '\n';
    return bool(out);
}

TAbClock g_clock;

}  // namespace

bool InitializeUISpellCreateMode()
{
    return GiveDemoPouch() && HostSpellPane(true, false);
}

bool InitializeUISpellCreateModeEmbedded()
{
    return GiveDemoPouch() && HostSpellPane(false, true);
}

// The test screen doesn't run the screen's pane pass, so the host does what
// TPlayScreen does for the pane: shows it with the side panel's spell tab and
// pulses it once a tick.
void RenderUISpellCreateMode()
{
    if (!g_host.Pane())
        return;
    const SHudState& hud = GetHudState();
    const bool shown = !g_followHud || (hud.sidebarState == HUD_SIDEBAR_OPEN && hud.bottomSlot == HUD_BOT_SPELL);
    if (shown == SpellPane.IsHidden())
        shown ? SpellPane.Show() : SpellPane.Hide();
    g_host.PulseTicks();
}

void CloseUISpellCreateMode()
{
    UnhostSpellPane();
}

bool InitializeABSpellPaneMode()
{
    SCase c;
    if (!ParseCase(StartupAbCase, c))
    {
        log_error("[ab-spellpane] bad --ab-case \"%s\"", StartupAbCase.c_str());
        return false;
    }
    if (!UIDemoPlayer::Install() || !Player)
    {
        log_error("[ab-spellpane] no demo player");
        return false;
    }
    return HostSpellPane(true, false) && ApplyCase(c);
}

void RenderABSpellPaneMode()
{
    if (!g_host.Pane() || !g_clock.Stop())
        return;
    SpellPane.Pulse();
    if (!StartupAbOut.empty() && !WriteShown(StartupAbOut))
        log_error("[ab-spellpane] could not write %s", StartupAbOut.c_str());
    log_info("[ab-spellpane] pulsed at frame %lld", static_cast<long long>(TTime::FrameCount()));
}

void CloseABSpellPaneMode()
{
    g_clock.Restore();
    UnhostSpellPane();
    UIDemoPlayer::Remove();
}
