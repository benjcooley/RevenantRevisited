// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *     shadowledger.cpp - Shadow Smoke's audit of every change to Locke   *
// *************************************************************************

#include "shadowledger.h"

#include "shadowlog.h"

#include "../exit.h"
#include "../player.h"
#include "../revenant.h"
#include "../rules.h"
#include "../script.h"
#include "../spell.h"
#include "../textencoding.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <sstream>

namespace {

// A move further than this in one tick is a jump (a teleport), not a step:
// a run covers well under 32 units a tick, a knockback's snap under 64.
constexpr int32_t kJumpDistance = 96;
// A jump that lands this close to an exit.def target went through it.
constexpr int32_t kExitSlack = 64;

std::string Lower(const char* s)
{
    std::string out = s ? s : "";
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return (char)std::tolower(c); });
    return out;
}

int32_t Distance2D(const S3DPoint& a, const S3DPoint& b)
{
    return (int32_t)std::hypot(double(a.x - b.x), double(a.y - b.y));
}

// The most a vital regenerates over `ticks` (TCharacter::PulseRecovery):
// `value` points every `rate` hundredths of a second, plus `pct` points a
// minute, accumulated in 1/10000 points with up to one point carried over.
int32_t RegenBound(int64_t ticks, int32_t value, int32_t rate, int32_t pct)
{
    const double perTick = (100.0 / 24.0) * value / std::max(rate, 1) + std::max(pct, 0) / 1440.0;
    return 1 + (int32_t)std::ceil(perTick * double(ticks));
}

std::string PointText(const S3DPoint& p, int32_t level)
{
    std::ostringstream out;
    out << '(' << p.x << ", " << p.y << ", " << p.z << ") level " << level;
    return out.str();
}

} // namespace

SShadowSnapshot SShadowSnapshot::Take(TPlayer& player, int64_t tick)
{
    SShadowSnapshot s;
    s.tick = tick;
    s.pos = player.Pos();
    s.maplevel = player.GetLevel();
    s.health = player.Health();
    s.mana = player.Mana();
    s.fatigue = player.Fatigue();
    s.gold = player.GetInventoryAmount("Gold");
    s.level = player.Level();
    s.exp = player.Exp();
    s.attacklevel = player.AttackLevel();
    // The stats as kept: what equipment and stat effects add on top come
    // and go with them, and are no one's to explain.
    for (int32_t i = 0; i < NUM_PLRSTATS; ++i)
        s.stats[i] = player.BaseObjStat(PLRSTAT_FIRST + i);
    for (int32_t i = 0; i < NUM_SKILLS; ++i)
    {
        s.skills[i] = player.BaseObjStat(SK_FIRST + i);
        s.skillexp[i] = player.BaseObjStat(SKE_FIRST + i);
    }
    s.levelup = player.LevelUpStats();
    const TGameState& states = ScriptManager.GameStates();
    s.states.resize(states.NumStates());
    for (int32_t i = 0; i < states.NumStates(); ++i)
        s.states[i] = states.State(i);
    for (TConstInventoryIterator i(&player, EInvWalk::Nested); i; i++)
    {
        const std::string name = Lower(i.Item()->GetName());
        if (name != "gold")
            s.items[name] += std::max(i.Item()->Amount(), 1);
    }
    return s;
}

void TShadowLedger::Start(TPlayer& player, int64_t tick)
{
    last = SShadowSnapshot::Take(player, tick);
    started = true;
    scripts.clear();
    TScript::SetStepObserver(this);
}

void TShadowLedger::Stop()
{
    TScript::SetStepObserver(nullptr);
    started = false;
}

void TShadowLedger::AllowTravel(const S3DPoint& landing, int32_t level, int64_t until)
{
    travels.push_back({landing, level, until});
}

bool TShadowLedger::LandedOnTravel(const SShadowSnapshot& now)
{
    const auto landed = std::find_if(travels.begin(), travels.end(), [&](const STravel& t) {
        return t.level == now.maplevel && Distance2D(t.landing, now.pos) <= kExitSlack;
    });
    if (landed == travels.end())
        return false;
    travels.erase(landed);
    return true;
}

void TShadowLedger::AllowItems(std::string cause, std::map<std::string, int32_t> items, int32_t gold, int64_t until)
{
    if (gold > 0)
        items["gold"] += gold;
    gifts.push_back({std::move(cause), std::move(items), until});
}

int32_t TShadowLedger::Explain(const std::string& item, int32_t gain, std::string& causes)
{
    for (SItems& gift : gifts)
    {
        const auto it = gift.items.find(item);
        if (it == gift.items.end() || it->second <= 0 || gain <= 0)
            continue;
        const int32_t taken = std::min(gain, it->second);
        it->second -= taken;
        gain -= taken;
        causes += (causes.empty() ? "" : ", ") + gift.cause;
    }
    return gain;
}

void TShadowLedger::AllowVitals(std::string cause, int32_t health, int32_t mana, int64_t until)
{
    meals.push_back({std::move(cause), health, mana, until});
}

void TShadowLedger::BeforeLine(TScript& script, int32_t offset)
{
    const TScriptProto* proto = script.GetScriptProto();
    scripts.insert(proto && proto->name ? proto->name : "?");
    coverage.Line(script, offset, audittick);
}

std::string TShadowLedger::ScriptCause() const
{
    std::string cause = "script";
    const char* sep = " ";
    for (const std::string& name : scripts)
    {
        cause += sep + ToUtf8(name.c_str());
        sep = ", ";
    }
    return cause;
}

bool TShadowLedger::LandedOnExit(const SShadowSnapshot& now, std::string& exitname) const
{
    for (const TExit::STarget& exit : TExit::Targets())
        if (exit.level == now.maplevel && Distance2D(exit.target, now.pos) <= kExitSlack)
        {
            exitname = exit.name;
            return true;
        }
    return false;
}

void TShadowLedger::Violation(TShadowLog& log, const char* what, const std::string& text)
{
    ++violations;
    log.Event("violation", std::string(what) + ": " + text, [&](JsonOut& out) {
        out.FieldString("what", what);
        out.FieldString("text", text);
    });
}

int32_t TShadowLedger::Audit(TPlayer& player, int64_t tick, TShadowLog& log)
{
    if (!started)
        return 0;

    const int32_t before = violations;
    audittick = tick;
    const SShadowSnapshot now = SShadowSnapshot::Take(player, tick);
    const bool scripted = !scripts.empty();
    const std::string scriptcause = scripted ? ScriptCause() : std::string();
    const int64_t ticks = std::max<int64_t>(now.tick - last.tick, 1);

    // ---- cheats: never, whoever set them ----
    struct SCheat { const char* name; bool on; };
    for (const SCheat cheat : {SCheat{"alreadydead", CheatAlreadyDead}, SCheat{"nahkranoth", CheatNahkranoth},
                               SCheat{"dummies", NoAI}, SCheat{"abracadabra", MagicCheat}})
        if (cheat.on)
            Violation(log, "cheat", std::string(cheat.name) + " is on");

    // ---- position: a jump needs a script, or an exit it went through ----
    if (now.maplevel != last.maplevel || Distance2D(now.pos, last.pos) > kJumpDistance)
    {
        const std::string text = PointText(last.pos, last.maplevel) + " to " + PointText(now.pos, now.maplevel);
        std::string exitname;
        if (scripted)
            log.Event("moved", text + " by " + scriptcause, [&](JsonOut& out) {
                out.FieldString("cause", scriptcause);
                out.Key("to").Begin('[').Value(now.pos.x).Value(now.pos.y).Value(now.pos.z).Value(now.maplevel).End(']');
            });
        else if (LandedOnTravel(now))
            log.Event("moved", text + " by a proven travel", [&](JsonOut& out) {
                out.FieldString("cause", "travel");
                out.Key("to").Begin('[').Value(now.pos.x).Value(now.pos.y).Value(now.pos.z).Value(now.maplevel).End(']');
            });
        else if (LandedOnExit(now, exitname))
            log.Event("moved", text + " through exit " + exitname, [&](JsonOut& out) {
                out.FieldString("cause", "exit " + exitname);
                out.Key("to").Begin('[').Value(now.pos.x).Value(now.pos.y).Value(now.pos.z).Value(now.maplevel).End(']');
            });
        else
            Violation(log, "jump", text);
    }

    // ---- numbers that only rise for a reason ----
    auto gain = [&](const char* field, int32_t was, int32_t is) {
        if (is == was)
            return;
        const std::string text = std::string(field) + " " + std::to_string(was) + " -> " + std::to_string(is);
        // A registered gift (a take) names the gain better than "a script
        // ran this tick" does; scripts like a teleport stone's run every tick.
        std::string given;
        if (is > was && !strcmp(field, "gold") && Explain("gold", is - was, given) == 0)
        {
            log.Event("gain", text + " by " + given, [&](JsonOut& out) {
                out.FieldString("field", field);
                out.Field("was", was);
                out.Field("is", is);
                out.FieldString("cause", given);
            });
            return;
        }
        // A registered cause first: an area's ALWAYS script may be running
        // lines in the same tick without touching him.
        if (is > was && tick <= progressuntil && strcmp(field, "gold") != 0)
        {
            log.Event("gain", text + " by " + progresscause, [&](JsonOut& out) {
                out.FieldString("field", field);
                out.Field("was", was);
                out.Field("is", is);
                out.FieldString("cause", progresscause);
            });
            return;
        }
        if (is < was || scripted)
            log.Event(is < was ? "loss" : "gain", text + (scripted ? " by " + scriptcause : ""), [&](JsonOut& out) {
                out.FieldString("field", field);
                out.Field("was", was);
                out.Field("is", is);
                if (scripted)
                    out.FieldString("cause", scriptcause);
            });
        else
            Violation(log, "gain", text);
    };
    gain("gold", last.gold, now.gold);
    gain("level", last.level, now.level);
    gain("exp", last.exp, now.exp);
    gain("attacklevel", last.attacklevel, now.attacklevel);
    for (int32_t i = 0; i < NUM_PLRSTATS; ++i)
        gain(kAttributeNames[i], last.stats[i], now.stats[i]);
    for (int32_t i = 0; i < NUM_SKILLS; ++i)
    {
        gain(kSkillNames[i], last.skills[i], now.skills[i]);
        gain((std::string(kSkillNames[i]) + " exp").c_str(), last.skillexp[i], now.skillexp[i]);
    }
    if (now.levelup != last.levelup && now.level == last.level)
    {
        if (tick <= traininguntil)
            log.Event("train", "the level-up picks are now " + std::string(kAttributeNames[now.levelup[0]]) + " and " +
                                   kAttributeNames[now.levelup[1]]);
        else if (!scripted)
            Violation(log, "levelup", "the level-up markers changed with no cause");
    }

    // ---- items ----
    std::set<std::string> names;
    for (const auto& [name, amount] : last.items)
        names.insert(name);
    for (const auto& [name, amount] : now.items)
        names.insert(name);
    for (const std::string& name : names)
    {
        const auto was_it = last.items.find(name);
        const auto is_it = now.items.find(name);
        const int32_t was = was_it == last.items.end() ? 0 : was_it->second;
        const int32_t is = is_it == now.items.end() ? 0 : is_it->second;
        if (was == is)
            continue;
        ++itemsversion;
        const std::string utf8 = ToUtf8(name.c_str());
        const std::string text = utf8 + " " + std::to_string(was) + " -> " + std::to_string(is);
        std::string given;
        if (is > was && Explain(name, is - was, given) == 0)
            log.Event("gain", text + " by " + given, [&](JsonOut& out) {
                out.FieldString("item", utf8);
                out.Field("was", was);
                out.Field("is", is);
                out.FieldString("cause", given);
            });
        else if (scripted)
            log.Event(is < was ? "loss" : "gain", text + " by " + scriptcause, [&](JsonOut& out) {
                out.FieldString("item", utf8);
                out.Field("was", was);
                out.Field("is", is);
                out.FieldString("cause", scriptcause);
            });
        else if (is > was)
            Violation(log, "item", text);
        else
            log.Event("finding", "lost with no cause: " + text, [&](JsonOut& out) {
                out.FieldString("what", "item lost");
                out.FieldString("item", utf8);
                out.Field("was", was);
                out.Field("is", is);
            });
    }

    // ---- the story: only scripts move it ----
    const TGameState& states = ScriptManager.GameStates();
    for (size_t i = 0; i < now.states.size(); ++i)
    {
        const int32_t was = i < last.states.size() ? last.states[i] : 0;
        const int32_t is = now.states[i];
        if (was == is)
            continue;
        const std::string name = ToUtf8(states.StateName((int32_t)i));
        const std::string text = name + " " + std::to_string(was) + " -> " + std::to_string(is);
        coverage.StateChanged(name, was, is, tick);
        if (!scripted)
        {
            Violation(log, "story", text + " with no script running");
            continue;
        }
        log.Event("story", text + " by " + scriptcause, [&](JsonOut& out) {
            out.FieldString("state", name);
            out.Field("was", was);
            out.Field("is", is);
            out.FieldString("cause", scriptcause);
            out.Key("at").Begin('[').Value(now.pos.x).Value(now.pos.y).Value(now.pos.z).Value(now.maplevel).End(']');
        });
    }

    // ---- vitals: regeneration, a level-up's refill, or a script ----
    const bool levelled = now.level > last.level;
    auto vital = [&](const char* field, int32_t was, int32_t is, int32_t value, int32_t rate, int32_t pct) {
        if (is <= was || scripted || levelled)
            return;
        int32_t bound = RegenBound(ticks, value, rate, pct);
        // Food and potions: what they give, once.
        const bool health = !strcmp(field, "health");
        for (SVitals& meal : meals)
        {
            int32_t& gives = health ? meal.health : meal.mana;
            if (gives <= 0 || !strcmp(field, "fatigue"))
                continue;
            const int32_t used = std::min(gives, std::max(0, is - was - bound));
            gives -= used;
            bound += used;
            if (used > 0)
                log.Event("gain", std::string(field) + " " + std::to_string(was) + " -> " + std::to_string(is) +
                                      " by " + meal.cause);
        }
        if (is - was > bound)
            Violation(log, "vital", std::string(field) + " " + std::to_string(was) + " -> " + std::to_string(is) +
                                    " in " + std::to_string(ticks) + " ticks (regeneration allows " +
                                    std::to_string(bound) + ")");
    };
    vital("health", last.health, now.health, Rules.healthrecovval, Rules.healthrecovrate, player.HealthPct());
    vital("mana", last.mana, now.mana, Rules.manarecovval, Rules.manarecovrate, player.ManaPct());
    vital("fatigue", last.fatigue, now.fatigue, Rules.fatiguerecovval, Rules.fatiguerecovrate, player.FatiguePct());

    last = now;
    scripts.clear();
    travels.erase(std::remove_if(travels.begin(), travels.end(), [&](const STravel& t) { return t.until < tick; }),
                  travels.end());
    gifts.erase(std::remove_if(gifts.begin(), gifts.end(), [&](const SItems& g) { return g.until < tick; }),
                gifts.end());
    meals.erase(std::remove_if(meals.begin(), meals.end(), [&](const SVitals& m) { return m.until < tick; }),
                meals.end());
    return violations - before;
}
