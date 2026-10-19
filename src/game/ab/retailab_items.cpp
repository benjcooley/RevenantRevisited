// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *    retailab_items.cpp - the port's side of the item A/B dumps         *
// *************************************************************************
//
// `item-food-use`: TFood::Use (retail 0x0050e780) on the case's item and user,
// dumped in the schema of the retail fixture
// tools/retail_runtime/slots/items/food_use.py
// (docs/gameplay/forensics/CONSUMABLES.md §10). The user is a fixture
// character (retailab_fixture.h): its stats are the case's and its stat writes
// are recorded. The item is a real object of the case's type in the user's
// pack, in a pouch there, or on its own. The sound looked up (FindSound), the
// text-bar lines and the cure's map scan (the map pane's iterator seam) are
// recorded as the retail fixture's boundaries record them.

#include "game/ab/retailab_fixture.h"

#include "game/script/dialog.h"
#include "game/object/food.h"
#include "engine/core/logging.h"
#include "game/world/mappane.h"
#include "game/audio/sound.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace RetailAB
{
namespace
{

using namespace Fixture;

// What the retail fixture's FindSound answers (food_use.py SOUND_ID).
constexpr int32_t kSoundId = 7;

// The case's run: the sounds looked up and the text-bar lines, while Use runs.
struct SFoodUseLog
{
    bool recording = false;
    std::vector<std::string> sounds;
    std::vector<std::string> text;
};
SFoodUseLog g_log;

int32_t CaseFindSound(const char* name, int32_t /*nr*/)
{
    if (g_log.recording)
        g_log.sounds.emplace_back(name ? name : "");
    return kSoundId;
}

// TTextBar::VPrint logs each message as "[textbar] <text>".
void CaptureTextBar(log_Event* ev)
{
    if (!g_log.recording || ev->level != LOG_DEBUG || !ev->fmt || strncmp(ev->fmt, "[textbar] ", 10) != 0)
        return;
    char buf[2048];
    vsnprintf(buf, sizeof(buf), ev->fmt, ev->ap);
    g_log.text.emplace_back(buf + 10);
}

// The seams this target answers, put back when the case ends: stat reads
// unrecorded, the main player the case's, no map objects for the cure (the
// iterator's query recorded), FindSound answered.
class SFoodUseScope
{
  public:
    SFoodUseScope()
    {
        static const bool captured = log_add_callback(CaptureTextBar, nullptr, LOG_DEBUG) == 0;
        (void)captured;
        savedPlayer = Player;
        quietReads = true;
        SetMapObjects({});
        TSoundPlayer::findSeam = CaseFindSound;
        g_log = SFoodUseLog{};
        ClearSeams();
    }
    ~SFoodUseScope()
    {
        g_log.recording = false;
        TSoundPlayer::findSeam = nullptr;
        TMapPane::mapObjectsSeam = nullptr;
        quietReads = false;
        Player = savedPlayer;
    }
    SFoodUseScope(const SFoodUseScope&) = delete;
    SFoodUseScope& operator=(const SFoodUseScope&) = delete;

  private:
    TPlayer* savedPlayer = nullptr;
};

// A new object of the named type, as retail's AddToInventory makes one by
// name (the first class with that type).
TObjectInstance* NewObjectNamed(const std::string& type)
{
    for (int32_t c = 0; c < MAXOBJECTCLASSES; ++c)
        if (TObjectClass* cl = TObjectClass::GetClass(c))
            if (const int32_t objtype = cl->FindObjType(type.c_str()); objtype >= 0)
            {
                SObjectState def{};
                def.objclass = (short)cl->ClassId();
                def.objtype = (short)objtype;
                if (TObjectInstance* inst = cl->NewObject(&def))
                    return inst;
                break;
            }
    throw std::runtime_error("no object type '" + type + "'");
}

TObjectInstance* AddNamed(TObjectInstance* owner, const std::string& type, int32_t slot)
{
    if (!owner->AddToInventory(type.c_str(), 1, slot))
        throw std::runtime_error("AddToInventory refused '" + type + "' at slot " + std::to_string(slot));
    TObjectInstance* item = owner->GetInventorySlot(slot);
    if (!item)
        throw std::runtime_error("no '" + type + "' at slot " + std::to_string(slot));
    return item;
}

// Case (field 0, JSON): food_use.py's case, its user also given as a fixture
// character (`chars`, by retail_ab's targets_items.py).
std::string FoodUse(const Case& c, std::string& error)
{
    try
    {
        if (!LoadGameData(error))
            return {};
        static const bool messages = DialogList.Initialize();      // english.def: FOODGOOD1
        if (!messages)
        {
            error = "can't load the game's messages";
            return {};
        }
        const JsonValue cs = JsonValue::Parse(c.Field(0));
        TFixtureWorld world(cs);
        SFoodUseScope scope;
        TCharacter* user = world.Get(cs["user"]["name"].Str());
        user->ForceLevel((int32_t)cs["user"]["level"].Int());
        const std::string main = cs["main"].Str();
        Player = main == "user" ? static_cast<TPlayer*>(user)
               : main == "other" ? static_cast<TPlayer*>(world.Get("Somebody"))
               : nullptr;

        // The item: in the pack (or on the belt) at its slot, in a pouch at
        // that slot, or on its own.
        const JsonValue& spec = cs["item"];
        const std::string where = spec.Has("where") ? spec["where"].Str() : "pack";
        int32_t slot = (int32_t)spec["slot"].Int();
        TObjectInstance* owner = user;
        TObjectInstance* item = nullptr;
        TObjectPtr<TObjectInstance> onMap;
        if (where == "map")
        {
            onMap.reset(NewObjectNamed(spec["type"].Str()));
            item = onMap.get();
            owner = nullptr;
        }
        else if (where == "pouch")
        {
            owner = AddNamed(user, "Pouch", slot);
            slot = 0;
            item = AddNamed(owner, spec["type"].Str(), slot);
        }
        else
            item = AddNamed(user, spec["type"].Str(), slot);
        if (spec["amount"].Int(1) != 1)
            item->SetAmount((int32_t)spec["amount"].Int());

        ClearSeams();
        g_log.recording = true;
        const bool returned = item->Use(user, (int32_t)cs["with"].Int(-1));
        g_log.recording = false;

        // The stat writes and the cure's scan, from the seams in order.
        JsonOut writes, scans;
        writes.Begin('[');
        scans.Begin('[');
        for (const std::string& record : Seams())
        {
            const JsonValue s = JsonValue::Parse(record);
            if (s["seam"].Str() == "SetObjStat" && s["who"].Str() == cs["user"]["name"].Str())
                writes.Begin('[').String(s["stat"].Str()).Value(s["value"].Int()).End(']');
            else if (s["seam"].Str() == "MapIterator")
            {
                scans.Begin('{').Key("rect").Begin('[');
                for (const JsonValue& v : s["rect"].Items())
                    scans.Value(v.Int());
                scans.End(']').Field("objset", s["objset"].Int()).Field("level", s["level"].Int()).End('}');
            }
        }
        writes.End(']');
        scans.End(']');

        const bool present = !owner || owner->GetInventorySlot(slot) == item;
        JsonOut j;
        j.Begin('{').FieldString("schema", "items.food_use.v1").FieldString("case", c.name);
        j.Field("returned", returned ? 1 : 0);
        j.Key("writes").Raw(writes.str());
        j.Key("sounds").Begin('[');
        for (const std::string& s : g_log.sounds)
            j.String(s);
        j.End(']');
        j.Key("text").Begin('[');
        for (const std::string& t : g_log.text)
            j.String(t);
        j.End(']');
        j.Key("scans").Raw(scans.str());
        j.Key("item").Begin('{').FieldBool("present", present).Key("amount");
        if (present)
            j.Value(item->Amount());
        else
            j.Null();
        j.End('}').End('}');
        return j.str();
    }
    catch (const std::exception& e)
    {
        error = e.what();
        return {};
    }
}

}  // namespace

static const bool registered = RegisterTarget("item-food-use", FoodUse);

}  // namespace RetailAB
