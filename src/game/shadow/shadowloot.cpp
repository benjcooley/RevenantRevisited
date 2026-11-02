// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *       shadowloot.cpp - Shadow Smoke's chests: what they hold, taking    *
// *************************************************************************

#include "game/shadow/shadowloot.h"

#include "game/shadow/shadowlog.h"

#include "game/object/container.h"
#include "game/world/mappane.h"
#include "game/app/revenant.h"
#include "engine/core/textencoding.h"

#include <algorithm>
#include <cctype>

namespace {

std::string Lower(const char* s)
{
    std::string out = s ? s : "";
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return (char)std::tolower(c); });
    return out;
}

} // namespace

std::string SShadowChest::Handle() const
{
    return type + "@" + std::to_string(pos.x) + "," + std::to_string(pos.y);
}

SShadowChest ShadowReadChest(TContainer& chest)
{
    SShadowChest c;
    c.chest = &chest;
    c.type = chest.GetTypeName() ? chest.GetTypeName() : "";
    c.name = chest.GetName() && chest.GetName()[0] ? chest.GetName() : c.type;
    c.pos = IntPos(chest);
    c.openable = chest.Openable() != 0;
    c.locked = chest.Locked() != 0;
    c.key = chest.KeyId();
    c.pick = chest.PickDifficulty();
    for (TInventoryIterator i(&chest, EInvWalk::Nested); i; i++)
    {
        const std::string name = Lower(i.Item()->GetName());
        const int32_t amount = std::max(i.Item()->Amount(), 1);
        if (name == "gold")
            c.gold += amount;
        else
            c.contents[name] += amount;
    }
    return c;
}

std::vector<SShadowChest> ShadowSurveyChests(int32_t level)
{
    std::vector<SShadowChest> chests;
    for (TMapIterator i(level, nullptr, CHECK_NOINVENT); i; i++)
        if (i->ObjClass() == OBJCLASS_CONTAINER)
            chests.push_back(ShadowReadChest(*static_cast<TContainer*>(i.Item())));
    return chests;
}

void ShadowLogChests(int32_t level, TShadowLog& log)
{
    const std::vector<SShadowChest> chests = ShadowSurveyChests(level);
    int32_t gold = 0, locked = 0, items = 0;
    for (const SShadowChest& c : chests)
    {
        gold += c.gold;
        locked += c.locked;
        for (const auto& [name, amount] : c.contents)
            items += amount;
    }
    log.Event("chests",
              "level " + std::to_string(level) + ": " + std::to_string(chests.size()) + " containers (" +
                  std::to_string(locked) + " locked), " + std::to_string(gold) + " gold and " +
                  std::to_string(items) + " items in them",
              [&](JsonOut& out) {
                  out.Field("level", level);
                  out.Key("chests").Begin('[');
                  for (const SShadowChest& c : chests)
                  {
                      if (!c.gold && c.contents.empty())
                          continue;     // the empty ones say nothing
                      out.Begin('{');
                      out.FieldString("handle", ToUtf8(c.Handle().c_str()));
                      out.FieldString("name", ToUtf8(c.name.c_str()));
                      out.Key("pos").Begin('[').Value(c.pos.x).Value(c.pos.y).Value(c.pos.z).End(']');
                      out.FieldBool("locked", c.locked);
                      if (c.key)
                          out.Field("key", c.key);
                      if (c.pick)
                          out.Field("pick", c.pick);
                      out.Field("gold", c.gold);
                      out.Key("items").Begin('{');
                      for (const auto& [name, amount] : c.contents)
                          out.Field(ToUtf8(name.c_str()).c_str(), amount);
                      out.End('}');
                      out.End('}');
                  }
                  out.End(']');
              });
}
