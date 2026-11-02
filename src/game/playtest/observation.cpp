// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   observation.cpp - what a playtest director sees of the game         *
// *************************************************************************

#include "game/playtest/observation.h"

#include "game/ui/bottombar.h"
#include "game/object/charstats.h"
#include "game/ui/cinematicscreen.h"
#include "game/object/container.h"
#include "game/ui/cursor.h"
#include "game/ui/death.h"
#include "game/script/dialog.h"
#include "game/object/inventory.h"
#include "game/app/gameflow.h"
#include "engine/core/jsonout.h"
#include "game/ui/loadscreen.h"
#include "game/ui/logoscreen.h"
#include "game/world/mappane.h"
#include "game/world/maprenderer.h"
#include "game/ui/menuscreens.h"
#include "game/object/player.h"
#include "game/ui/playscreen.h"
#include "game/app/revenant.h"
#include "game/spells/spell.h"
#include "game/ui/textbar.h"
#include "engine/core/textencoding.h"
#include "game/ui/uisidebartest.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <sstream>
#include <vector>

namespace
{

int64_t g_messagesSeen = 0;     // text-bar lines the last observation reported

constexpr int32_t kBeltObserved = 5;   // the belt slots F5-F9 use

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

// A quick spell slot's spell, by its spell.def name.
void QuickSpell(JsonOut& out, TPlayer& p, int32_t slot)
{
    const char* tal = p.GetQuickSpell(slot);
    const SSpellVariant* v = (tal && *tal) ? SpellList.GetVariantDataByTalismans(tal) : nullptr;
    if (v)
        out.String(ToUtf8(v->name));
    else
        out.Null();
}

// An item's own equipment slot, its EqSlot stat; -1 for an item without one.
int32_t EqSlot(TObjectInstance& item)
{
    return item.FindStat("EqSlot") < 0 ? -1 : item.GetStat("EqSlot");
}

// The numbers a grinding director weighs an item by (docs/playtest/GRIND.md
// §8), by the names GetStat takes (class.def STATS). An armour's EqSlot is
// the item's `eqslot`.
const std::vector<const char*>& ItemNumberNames(int32_t objclass)
{
    static const std::vector<const char*> none;
    static const std::vector<const char*> weapon{ "Type", "Damage", "MinStrength", "Value" };
    static const std::vector<const char*> armor{ "Protection", "MinStrength", "MinConstitution", "Value" };
    static const std::vector<const char*> food{ "Health", "Mana", "Poison" };
    switch (objclass)
    {
      case OBJCLASS_WEAPON: return weapon;
      case OBJCLASS_ARMOR:  return armor;
      case OBJCLASS_FOOD:
      case OBJCLASS_POTION: return food;
      default:              return none;
    }
}

// An item's amount, its own equipment slot and its numbers, each read the
// way the game reads it, GetStat(name) on the item: so a number that comes
// from elsewhere than retail's (weapon.def and armor.def bound over
// class.def, GRIND §5.1) shows as a difference between the games.
void WriteItemNumbers(JsonOut& out, TObjectInstance& item)
{
    out.Field("amount", item.Amount());
    out.Field("eqslot", EqSlot(item));
    out.Key("stats").Begin('{');
    for (const char* name : ItemNumberNames(item.ObjClass()))
        out.Field(Lower(name).c_str(), item.GetStat(name));
    out.End('}');
}

// The items `holder` holds, in slot order.
std::vector<TObjectInstance*> HeldItems(TObjectInstance& holder)
{
    std::vector<TObjectInstance*> items;
    for (TInventoryIterator i(&holder); i; i++)
        items.push_back(i.Item());
    std::sort(items.begin(), items.end(),
              [](const TObjectInstance* a, const TObjectInstance* b) { return a->InventNum() < b->InventNum(); });
    return items;
}

// Every item the player carries: the pack (slots 0..0xfe), what he wears
// and wields (0x100..), the belt (0x10b..), and what the bags among them
// hold, each with `bag`, the slots of the bags it is in, outermost first.
void WriteItems(JsonOut& out, TObjectInstance& holder, std::vector<int32_t>& bag)
{
    for (TObjectInstance* item : HeldItems(holder))
    {
        out.Begin('{');
        out.Field("slot", item->InventNum());
        out.FieldString("name", ToUtf8(item->GetName()));
        out.FieldString("class", Lower(item->GetClassName()));
        WriteItemNumbers(out, *item);
        if (!bag.empty())
        {
            out.Key("bag").Begin('[');
            for (int32_t slot : bag)
                out.Value(slot);
            out.End(']');
        }
        out.End('}');
        if (item->NumInventoryItems() > 0)
        {
            bag.push_back(item->InventNum());
            WriteItems(out, *item, bag);
            bag.pop_back();
        }
    }
}

// A STATLINE as words: the stored line's spacing (a leading blank, the
// blank INITIALIZE leaves) is the store's, not the effect's.
std::string StatLineWords(const std::string& line)
{
    std::istringstream words(line);
    std::string word, out;
    while (words >> word)
        out += (out.empty() ? "" : " ") + word;
    return out;
}

// The stat effect in force -- one at a time: AddStatEffect ends the others
// -- and the ticks till it ends by the player's play clock (1/100 s), null
// for one without a TIME. Null with none.
void WriteEffect(JsonOut& out, const TPlayer& p)
{
    out.Key("effect");
    if (p.StatEffects().empty())
    {
        out.Null();
        return;
    }
    const TPlayer::SStatEffect& effect = p.StatEffects().back();
    out.Begin('{');
    out.FieldString("statline", ToUtf8(StatLineWords(effect.statline).c_str()));
    out.Key("ticks");
    if (effect.expires < 0)
        out.Null();
    else
        out.Value((int64_t(effect.expires) - p.PlayClock()) * 24 / 100);
    out.End('}');
}

// The skills by name, each as [level, experience, experience for the next
// level], from the modified stats (skills capped at 30 there).
void WriteSkills(JsonOut& out, TPlayer& p)
{
    out.Key("skills").Begin('{');
    for (int32_t skill = 0; skill < NUM_SKILLS; ++skill)
        out.Key(kSkillNames[skill]).Begin('[')
           .Value(p.Skill(skill)).Value(p.SkillExp(skill)).Value(p.GetObjStat(SKN_FIRST + skill)).End(']');
    out.End('}');
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
            out.FieldString("name", ToUtf8(item->GetName()));
            out.FieldString("class", Lower(item->GetClassName()));
            out.Field("eqslot", EqSlot(*item));
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
            out.FieldString(std::to_string(slot).c_str(), ToUtf8(item->GetName()));
    out.End('}');
}

void WritePlayer(JsonOut& out, TPlayer& p)
{
    out.Key("player").Begin('{');
    const S3DPoint pos = IntPos(p);
    Point(out, "pos", pos);
    ScreenPos(out, pos);
    out.Field("level", p.GetLevel());
    out.Field("facing", IntFacing(p));
    out.Field("radius", p.Radius());
    Pair(out, "health", p.Health(), p.MaxHealth());
    Pair(out, "mana", p.Mana(), p.MaxMana());
    Pair(out, "fatigue", p.Fatigue(), p.MaxFatigue());
    out.FieldString("mode", ModeName(p));
    Action(out, p);
    Id(out, "target", p.Fighting());
    out.Field("gold", p.GetInventoryAmount("Gold"));
    out.Key("xp").Begin('[').Value(p.Level()).Value(p.Exp()).Value(p.NextExp()).End(']');
    out.Field("attacklevel", p.AttackLevel());

    out.Key("stats").Begin('{');
    for (int32_t stat = 0; stat < NUM_PLRSTATS; ++stat)
        out.Field(kAttributeNames[stat], p.PlyrStat(stat));
    out.End('}');
    WriteSkills(out, p);

    const std::array<int32_t, 3>& levelup = p.LevelUpStats();
    out.Key("levelup").Begin('[').Value(levelup[0]).Value(levelup[1]).Value(levelup[2]).End(']');

    out.Key("combat").Begin('{');
    out.Field("offense", p.Offense());
    out.Field("defense", p.Defense());
    out.Field("armor", p.ArmorValue());
    out.Field("damage", p.WeaponDamage());
    out.End('}');
    WriteEffect(out, p);

    // char.def's weapon mask bit for the weapon in hand (IsValidAttack's
    // 1 << WeaponType()), 0 with nothing in hand, and the skill a kill
    // with it raises (stat 43 + WeaponType: Hands barehanded).
    TObjectInstance* weapon = p.PrimeHand();
    const int32_t skill = SK_WEAPONSKILLS + p.WeaponType();
    out.Key("weapon").Begin('{');
    out.Key("name");
    if (weapon)
        out.String(ToUtf8(weapon->GetName()));
    else
        out.Null();
    out.Field("mask", weapon ? (1 << p.WeaponType()) : 0);
    out.Key("skill");
    if (skill >= 0 && skill < NUM_SKILLS)
        out.String(kSkillNames[skill]);
    else
        out.Null();
    out.End('}');

    out.Key("quick_spells").Begin('[');
    for (int32_t slot = 1; slot <= 4; ++slot)
        QuickSpell(out, p, slot);
    out.End(']');

    out.Key("belt").Begin('[');
    for (int32_t box = 0; box < kBeltObserved; ++box)
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

    out.Key("items").Begin('[');
    std::vector<int32_t> bag;
    WriteItems(out, p, bag);
    out.End(']');
    out.End('}');
}

// Where a press picks up what is worn: the equip pane's cell for each
// equipment slot (EQ_*), null while the side panel doesn't show the pane.
// A drag from there to the pack unequips.
void WriteEquipSlots(JsonOut& out)
{
    out.Key("equip_slots");
    int32_t x = 0, y = 0;
    if (CurrentScreen != &PlayScreen || !UIEquipSlotCenter(0, x, y))
    {
        out.Null();
        return;
    }
    out.Begin('[');
    for (int32_t slot = 0; slot < NUM_EQ_SLOTS; ++slot)
        if (UIEquipSlotCenter(slot, x, y))
            out.Begin('[').Value(x).Value(y).End(']');
        else
            out.Null();
    out.End(']');
}

// The belt's boxes on the bottom bar, in step with `player.belt`: entry n
// the centre of the box showing belt slot n, null where no box shows it;
// null while the bar is hidden. A drop there puts an item on the belt.
void WriteBeltBoxes(JsonOut& out)
{
    TBottomBarPane* bar = CurrentScreen == &PlayScreen ? PlayScreen.BottomBar() : nullptr;
    out.Key("belt_boxes");
    if (!bar || !bar->IsOpen() || bar->IsHidden())
    {
        out.Null();
        return;
    }
    out.Begin('[');
    for (int32_t box = 0; box < kBeltObserved; ++box)
    {
        int32_t x = 0, y = 0;
        if (bar->Shelf().BoxCenter(box, x, y))
            out.Begin('[').Value(x).Value(y).End(']');
        else
            out.Null();
    }
    out.End(']');
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
    const S3DPoint pos = IntPos(inst);
    Point(out, "pos", pos);
    ScreenPos(out, pos);
    if (inst->IsCharacter())
    {
        TCharacter& c = *static_cast<TCharacter*>(inst);
        out.FieldBool("enemy", Player && c.IsEnemy(Player));
        Pair(out, "health", c.Health(), c.MaxHealth());
        // Its class Value, what a kill earns and its to-hit go by (the kill
        // formula reads it so, GetStat("Value")); a corpse below 1 health.
        out.Field("value", c.GetStat("Value"));
        out.FieldBool("dead", c.Health() < 1);
        Action(out, c);
        Id(out, "target", c.Fighting());
        out.End('}');
        return;
    }
    if (inst->Amount() > 1)
        out.Field("amount", inst->Amount());
    if (inst->IsInventoryItem())
    {
        out.Key("item").Begin('{');
        WriteItemNumbers(out, *inst);
        out.End('}');
    }
    if (inst->ObjClass() == OBJCLASS_CONTAINER)
    {
        // Open (its state, 0 shut), whether a use opens it, locked, and what
        // it holds: a use takes one item.
        TContainer& box = *static_cast<TContainer*>(inst);
        out.FieldBool("open", box.GetState() != 0);
        out.FieldBool("openable", box.Openable() != 0);
        out.FieldBool("locked", box.Locked() != 0);
        out.Key("contents").Begin('[');
        for (TObjectInstance* item : HeldItems(box))
        {
            out.Begin('{');
            out.Field("slot", item->InventNum());
            out.FieldString("name", ToUtf8(item->GetName()));
            out.Field("amount", item->Amount());
            out.End('}');
        }
        out.End(']');
    }
    out.End('}');
}

void WriteObjects(JsonOut& out, int32_t radius)
{
    out.Key("objects").Begin('[');
    if (Player && radius > 0)
    {
        const S3DPoint center = IntPos(Player);
        std::vector<std::pair<int64_t, TObjectInstance*>> near;
        for (TMapIterator i(center, radius, CHECK_NOINVENT | CHECK_MAPRECT); i; i++)
        {
            TObjectInstance* inst = i;
            if (!Listed(inst) || inst->GetLevel() != Player->GetLevel())
                continue;
            const S3DPoint pos = IntPos(inst);
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
    const S3DPoint center = IntPos(Player);
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
        Point(out, "pos", IntPos(Player));
        out.Field("facing", IntFacing(Player));
        out.Field("health", Player->Health());
        out.Field("mana", Player->Mana());
        out.Field("fatigue", Player->Fatigue());
        out.Field("action", Player->DoingAction());
        out.Key("target");
        if (TCharacter* target = Player->Fighting())
        {
            out.Begin('{');
            Point(out, "pos", IntPos(target));
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
    WriteEquipSlots(out);
    WriteBeltBoxes(out);
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
