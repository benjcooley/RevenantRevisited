// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   observation.cpp - what a playtest director sees of the game         *
// *************************************************************************

#include "observation.h"

#include "cinematicscreen.h"
#include "cursor.h"
#include "death.h"
#include "dialog.h"
#include "inventory.h"
#include "gameflow.h"
#include "jsonout.h"
#include "loadscreen.h"
#include "logoscreen.h"
#include "mappane.h"
#include "maprenderer.h"
#include "menuscreens.h"
#include "player.h"
#include "playscreen.h"
#include "revenant.h"
#include "spell.h"
#include "textbar.h"
#include "textencoding.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <vector>

namespace
{

int64_t g_messagesSeen = 0;     // text-bar lines the last observation reported

std::string Lower(const char* s)
{
    std::string out = s ? s : "";
    for (char& c : out)
        c = char(std::tolower((unsigned char)c));
    return out;
}

void Point(JsonOut& out, const char* key, const S3DPoint& p)
{
    out.Key(key).Begin('[').Value(p.x).Value(p.y).Value(p.z).End(']');
}

void Pair(JsonOut& out, const char* key, int32_t a, int32_t b)
{
    out.Key(key).Begin('[').Value(a).Value(b).End(']');
}

void ScreenPos(JsonOut& out, const S3DPoint& world)
{
    int32_t sx = 0, sy = 0;
    out.Key("screen");
    if (Observation::Project(world, sx, sy))
        out.Begin('[').Value(sx).Value(sy).End(']');
    else
        out.Null();
}

void Id(JsonOut& out, const char* key, TObjectInstance* inst)
{
    out.Key(key);
    if (inst)
        out.Value(Observation::ObjectId(inst));
    else
        out.Null();
}

const char* ModeName(TCharacter& c)
{
    if (c.IsCombat())    return "combat";
    if (c.IsBowMode())   return "bow";
    if (c.IsSneakMode()) return "sneak";
    return "normal";
}

// The doing block: its action (retail numbering), its name and the ticks
// since it started.
void Action(JsonOut& out, const TCharacter& c)
{
    out.Field("action", c.DoingAction());
    out.FieldString("action_name", ToUtf8(c.DoingName()));
    out.Field("action_tick", (std::max)(0, PlayScreen.GameFrame() - c.DoingSince()));
}

// A quick spell slot's spell, as the spell ring names it (spellpane.cpp).
void QuickSpell(JsonOut& out, TPlayer& p, int32_t slot)
{
    const char* tal = p.GetQuickSpell(slot);
    const SSpellVariant* v = (tal && *tal) ? SpellList.GetVariantDataByTalismans(tal) : nullptr;
    if (v)
        out.String(ToUtf8(DialogList.DisplayName(v->name).c_str()));
    else
        out.Null();
}

// The items the side panel's inventory shows of the player's pack, each with
// its slot, class, own equipment slot (its EqSlot stat, -1 for none) and the
// screen centre of its cell, null while the pane is hidden. A right click on
// a pack item equips it in its own slot (docs/gameplay/forensics/
// PLAYER_INPUT.md §6), so a director equips by input, as a player does. Only
// the twelve cells in view: the pane pages by column, and so would a player.
void WritePack(JsonOut& out, TPlayer& p)
{
    out.Key("pack").Begin('[');
    if (Inventory.GetContainer() == &p)
        for (int32_t index = 0; index < TInventory::kColumns * TInventory::kRows; ++index)
        {
            const int32_t slot = Inventory.FirstSlot() + index;
            TObjectInstance* item = p.GetInventorySlot(slot);
            if (!item)
                continue;
            out.Begin('{');
            out.Field("slot", slot);
            out.FieldString("name", ToUtf8(DialogList.DisplayName(item->GetName()).c_str()));
            out.FieldString("class", Lower(item->GetClassName()));
            out.Field("eqslot", item->FindStat("EqSlot") < 0 ? -1 : item->GetStat("EqSlot"));
            out.Key("screen");
            int32_t x = 0, y = 0;
            if (Inventory.CellCenter(slot, x, y))
                out.Begin('[').Value(x).Value(y).End(']');
            else
                out.Null();
            out.End('}');
        }
    out.End(']');
}

// What is worn and wielded, by equipment slot number (player.h EQ_*).
void WriteEquipped(JsonOut& out, TPlayer& p)
{
    out.Key("equipped").Begin('{');
    for (int32_t slot = 0; slot < NUM_EQ_SLOTS; ++slot)
        if (TObjectInstance* item = p.GetInventorySlot(kInvSlotEquipFirst + slot))
            out.FieldString(std::to_string(slot).c_str(), ToUtf8(DialogList.DisplayName(item->GetName()).c_str()));
    out.End('}');
}

void WritePlayer(JsonOut& out, TPlayer& p)
{
    out.Key("player").Begin('{');
    const S3DPoint pos = p.Pos();
    Point(out, "pos", pos);
    ScreenPos(out, pos);
    out.Field("level", p.GetLevel());
    out.Field("facing", p.GetFace());
    out.Field("radius", p.Radius());
    Pair(out, "health", p.Health(), p.MaxHealth());
    Pair(out, "mana", p.Mana(), p.MaxMana());
    Pair(out, "fatigue", p.Fatigue(), p.MaxFatigue());
    out.FieldString("mode", ModeName(p));
    Action(out, p);
    Id(out, "target", p.Fighting());
    out.Field("gold", p.GetInventoryAmount("Gold"));
    out.Key("xp").Begin('[').Value(p.Level()).Value(p.Exp()).Value(p.NextExp()).End(']');

    out.Key("skills").Begin('{');
    out.Field("attack", p.Skill(SK_ATTACK));
    out.Field("defense", p.Skill(SK_DEFENSE));
    out.Field("invoke", p.Skill(SK_INVOKE));
    out.Field("weapon", p.WeaponSkill(p.WeaponType()));
    out.End('}');

    // char.def's weapon mask bit for the weapon in hand (IsValidAttack's
    // 1 << WeaponType()), 0 with nothing in hand.
    TObjectInstance* weapon = p.PrimeHand();
    out.Key("weapon").Begin('{');
    out.Key("name");
    if (weapon)
        out.String(ToUtf8(DialogList.DisplayName(weapon->GetName()).c_str()));
    else
        out.Null();
    out.Field("mask", weapon ? (1 << p.WeaponType()) : 0);
    out.End('}');

    out.Key("quick_spells").Begin('[');
    for (int32_t slot = 1; slot <= 4; ++slot)
        QuickSpell(out, p, slot);
    out.End(']');

    out.Key("belt").Begin('[');
    for (int32_t box = 0; box < 5; ++box)
    {
        const TObjectInstance* item = p.GetInventorySlot(kInvSlotBeltFirst + box);
        if (item)
            out.String(ToUtf8(item->GetName()));
        else
            out.Null();
    }
    out.End(']');
    WritePack(out, p);
    WriteEquipped(out, p);
    out.End('}');
}

// The newest NPC line still up, and the choices while they are up. Choices
// show in quotes; the observation gives the text alone.
void WriteDialog(JsonOut& out)
{
    const TDialogEntry* line = nullptr;
    const TDialogEntry* responses = nullptr;
    for (const auto& entry : DialogPane.Entries())
    {
        if (entry->IsDismissed())
            continue;
        if (entry->Mode() == TDialogEntry::EMode::NpcSpeech)
            line = entry.get();
        else if (entry->Mode() == TDialogEntry::EMode::Responses && DialogPane.IsShowingResponses())
            responses = entry.get();
    }
    if (!line && !responses)
        return;

    out.Key("dialog").Begin('{');
    if (line)
    {
        TObjectInstance* speaker = line->Speaker();
        if (speaker)
            out.FieldString("speaker", ToUtf8(speaker->GetName()));
        if (!line->Texts().empty())
            out.FieldString("text", ToUtf8(line->Texts().front().c_str()));
    }
    if (responses)
    {
        out.Key("choices").Begin('[');
        for (std::string text : responses->Texts())
        {
            if (text.size() >= 2 && text.front() == '"' && text.back() == '"')
                text = text.substr(1, text.size() - 2);
            out.String(ToUtf8(text.c_str()));
        }
        out.End(']');
    }
    out.End('}');
}

// The objects a director acts on: not tiles, shadows, effects or editor
// marks (the waypoint helpers excepted), not what can't be seen.
bool Listed(TObjectInstance* inst)
{
    if (!inst || inst == Player || inst->IsInInventory())
        return false;
    switch (inst->ObjClass())
    {
      case OBJCLASS_TILE:
      case OBJCLASS_SHADOW:
      case OBJCLASS_EFFECT:
        return false;
      case OBJCLASS_HELPER:
        return true;
      default:
        return !(inst->GetFlags() & (OF_EDITOR | OF_INVISIBLE));
    }
}

void WriteObject(JsonOut& out, TObjectInstance* inst)
{
    out.Begin('{');
    out.Field("id", Observation::ObjectId(inst));
    out.FieldString("name", ToUtf8(inst->GetName()));
    out.FieldString("type", ToUtf8(inst->GetTypeName()));
    out.FieldString("class", Lower(inst->GetClassName()));
    const S3DPoint pos = inst->Pos();
    Point(out, "pos", pos);
    ScreenPos(out, pos);
    if (inst->IsCharacter())
    {
        TCharacter& c = *static_cast<TCharacter*>(inst);
        out.FieldBool("enemy", Player && c.IsEnemy(Player));
        Pair(out, "health", c.Health(), c.MaxHealth());
        Action(out, c);
        Id(out, "target", c.Fighting());
    }
    else if (inst->Amount() > 1)
        out.Field("amount", inst->Amount());
    out.End('}');
}

void WriteObjects(JsonOut& out, int32_t radius)
{
    out.Key("objects").Begin('[');
    if (Player && radius > 0)
    {
        const S3DPoint center = Player->Pos();
        std::vector<std::pair<int64_t, TObjectInstance*>> near;
        for (TMapIterator i(center, radius, CHECK_NOINVENT | CHECK_MAPRECT); i; i++)
        {
            TObjectInstance* inst = i;
            if (!Listed(inst) || inst->GetLevel() != Player->GetLevel())
                continue;
            const S3DPoint pos = inst->Pos();
            const int64_t dx = pos.x - center.x, dy = pos.y - center.y;
            const int64_t d2 = dx * dx + dy * dy;
            if (d2 <= int64_t(radius) * radius)
                near.emplace_back(d2, inst);
        }
        // Nearest first; the id breaks ties, so the order doesn't hang on
        // the sectors' storage.
        std::sort(near.begin(), near.end(), [](const auto& a, const auto& b) {
            return a.first != b.first ? a.first < b.first
                                      : Observation::ObjectId(a.second) < Observation::ObjectId(b.second);
        });
        for (const auto& [d2, inst] : near)
            WriteObject(out, inst);
    }
    out.End(']');
}

// The object under the pointer, as the map pane's hover picks it from the
// last drawn frame (TMapPane::Animate, while the map has the pointer): what
// a click there acts on. Null for nothing, or a pointer the map doesn't have.
void WriteCursor(JsonOut& out)
{
    TObjectInstance* on = nullptr;
    if (CurrentScreen == &PlayScreen && PlayScreen.MapRenderer() && PlayScreen.MapHasPointer(cursorx, cursory))
        on = MapPane.OnObject(cursorx, cursory, nullptr);
    Id(out, "cursor", on);
}

// Walk heights (0: no floor) of the 16-unit cells around Locke, row-major.
void WriteWalkGrid(JsonOut& out, int32_t radius)
{
    if (!Player || radius < 0)
        return;
    const S3DPoint center = Player->Pos();
    const int32_t gx = (center.x - radius) >> WALKMAPSHIFT;
    const int32_t gy = (center.y - radius) >> WALKMAPSHIFT;
    const int32_t cells = ((2 * radius) >> WALKMAPSHIFT) + 1;
    out.Key("walkgrid").Begin('{');
    Pair(out, "origin", gx, gy);
    out.Field("cell", WALKMAPGRANULARITY);
    out.Field("w", cells);
    out.Field("h", cells);
    out.Key("heights").Begin('[');
    // -1 where no sector is loaded to read the cell from: unknown ground,
    // not "no floor" (0), which a director's map must not confuse.
    for (int32_t y = 0; y < cells; ++y)
        for (int32_t x = 0; x < cells; ++x)
            out.Value(MapPane.WalkCellKnown(gx + x, gy + y) ? MapPane.GetWalkGridHeight(gx + x, gy + y) : -1);
    out.End(']');
    out.End('}');
}

} // namespace

const char* Observation::ScreenName()
{
    // Between a quick start's load and the first frame no screen is up yet;
    // the game in the session is what the play screen shows next.
    const TScreen* screen = CurrentScreen;
    if (!screen)
        return GameFlow.Session().Ready() ? "play" : "loading";
    if (screen == &PlayScreen)
    {
        if (GameFlow.Session().Loading() || GameFlow.Session().LevelLoading())
            return "loading";
        if (PlayScreen.PlayingMovie())
            return "movie";
        if (PlayScreen.InGameMenuOpen())
            return "menu";
        if (DialogPane.IsShowingResponses())
            return "dialog";
        return "play";
    }
    if (screen == &LogoScreen)
        return "title";
    if (screen == &LoadScreen)
        return "loading";
    if (screen == &DeathScreen)
        return "death";
    if (screen == &CinematicScreen)
        return "movie";
    if (screen == &LoadGameScreen || screen == &OptionsScreen)
        return "menu";
    return "other";
}

int32_t Observation::ObjectId(TObjectInstance* inst)
{
    if (!inst)
        return -1;
    if (inst == Player)
        return 0;
    const int32_t index = inst->GetMapIndex();
    return index == 0 && Player ? Player->GetMapIndex() : index;
}

std::string Observation::CheckFields(uint64_t drawsBase)
{
    JsonOut out;
    out.Begin('{');
    out.FieldString("screen", ScreenName());
    if (Player)
    {
        out.Field("level", Player->GetLevel());
        Point(out, "pos", Player->Pos());
        out.Field("facing", Player->GetFace());
        out.Field("health", Player->Health());
        out.Field("mana", Player->Mana());
        out.Field("fatigue", Player->Fatigue());
        out.Field("action", Player->DoingAction());
        out.Key("target");
        if (TCharacter* target = Player->Fighting())
        {
            out.Begin('{');
            Point(out, "pos", target->Pos());
            out.Field("health", target->Health());
            out.End('}');
        }
        else
            out.Null();
    }
    out.Field("draws", (long long)(RandomDraws() - drawsBase));
    out.End('}');
    return out.str();
}

void Observation::Write(JsonOut& out, int32_t objects, int32_t walkgrid)
{
    out.FieldString("screen", ScreenName());
    out.FieldBool("control", CurrentScreen == &PlayScreen && PlayScreen.IsControlOn());
    if (Player)
        WritePlayer(out, *Player);
    else
        out.Key("player").Null();
    WriteCursor(out);
    WriteDialog(out);

    out.Key("messages").Begin('[');
    for (const std::string& line : TextBar.LinesSince(g_messagesSeen))
        out.String(ToUtf8(line.c_str()));
    out.End(']');
    g_messagesSeen = TextBar.LinesAdded();

    WriteObjects(out, objects);
    WriteWalkGrid(out, walkgrid);
}

bool Observation::Project(const S3DPoint& world, int32_t& x, int32_t& y)
{
    const TMapRenderer* view = PlayScreen.MapRenderer();
    if (!view || CurrentScreen != &PlayScreen)
        return false;
    int32_t mapx = 0, mapy = 0;
    ::WorldToScreen(world, mapx, mapy);
    return view->MapScreenToScreen(mapx, mapy, x, y);
}

bool Observation::Pick(int32_t x, int32_t y, S3DPoint& pos, int32_t& object)
{
    object = -1;
    if (!Player || !PlayScreen.MapRenderer() || CurrentScreen != &PlayScreen)
        return false;
    MapPane.ScreenToMapPos(x, y, pos, 50);
    if (TObjectInstance* on = MapPane.OnObject(x, y))
        object = ObjectId(on);
    return true;
}
