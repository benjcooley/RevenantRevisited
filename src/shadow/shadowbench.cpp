// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *    shadowbench.cpp - many trial duels from one game state (calibration) *
// *************************************************************************

#include "shadowbench.h"

#include "shadowduel.h"
#include "shadowlog.h"

#include "../character.h"
#include "../consoleexec.h"
#include "../mappane.h"
#include "../player.h"
#include "../revenant.h"
#include "../textencoding.h"

#include <algorithm>
#include <sstream>

namespace {

// The foes are looked for this far around Locke (the arena is 2048 across).
constexpr int32_t kFoeRange = 2048;
constexpr int32_t kMaxNear = 64;
// The set-up is done but the foes aren't there: give up after this long.
constexpr int32_t kWaitTicks = 48;

bool SameName(const char* a, const std::string& b)
{
    return a && stricmp(a, b.c_str()) == 0;
}

std::string NameOf(const TObjectInstance& o)
{
    const char* name = o.GetName() && o.GetName()[0] ? o.GetName() : o.GetTypeName();
    return ToUtf8(name ? name : "?");
}

std::string Describe(TCharacter& who)
{
    std::ostringstream text;
    text << NameOf(who) << " (health " << who.Health() << ", at " << IntPos(who).x << "," << IntPos(who).y
         << ")";
    return text.str();
}

} // namespace

EShadowStatus TShadowDuelBench::Pulse(TShadowLog& log)
{
    // The scenario's console lines first (level, gear, the monsters), and
    // a world at rest: no load, no fade.
    if (!ShadowGameIdle() || StartupExecPending())
        return EShadowStatus::Running;
    const std::vector<TCharacter*> foes = FindFoes();
    if (foes.size() < wanted.size())
    {
        if (++waited < kWaitTicks)
            return EShadowStatus::Running;
        failure = "the set-up made " + std::to_string(foes.size()) + " of the " + std::to_string(wanted.size()) +
                  " foes asked for";
        return EShadowStatus::Failed;
    }
    Run(foes, log);
    return EShadowStatus::Done;
}

std::vector<TCharacter*> TShadowDuelBench::FindFoes() const
{
    int32_t indices[kMaxNear];
    const int32_t n = MapPane.FindObjectsInRange(IntPos(Player), Player->GetLevel(), indices, kFoeRange, 0,
                                                 OBJCLASS_CHARACTER, kMaxNear, OBJSET_ALL);
    std::vector<TCharacter*> near;
    for (int32_t i = 0; i < n; ++i)
        if (auto* who = dynamic_cast<TCharacter*>(MapPane.GetInstance(indices[i])); who && who->Health() > 0)
            near.push_back(who);
    std::sort(near.begin(), near.end(),
              [](TCharacter* a, TCharacter* b) { return Player->Distance(a) < Player->Distance(b); });

    std::vector<TCharacter*> found;
    for (const std::string& type : wanted)
        for (TCharacter* who : near)
            if (SameName(who->GetTypeName(), type) && std::find(found.begin(), found.end(), who) == found.end())
            {
                found.push_back(who);
                break;
            }
    return found;
}

void TShadowDuelBench::Run(const std::vector<TCharacter*>& foes, TShadowLog& log) const
{
    TPlayer& locke = *Player;
    std::string names;
    for (TCharacter* foe : foes)
        names += (names.empty() ? "" : ", ") + Describe(*foe);
    TObjectInstance* hand = locke.PrimeHand();
    const TObjectInstance* weapon = hand && hand->ObjClass() == OBJCLASS_WEAPON ? hand : nullptr;
    log.Event("bench",
              std::to_string(runs) + " trial duels: Locke level " + std::to_string(locke.Level()) + ", health " +
                  std::to_string(locke.Health()) + ", " + (weapon ? NameOf(*weapon) : "barehanded") +
                  ", armour " + std::to_string(locke.ArmorValue()) + "; against " + names,
              [&](JsonOut& out) {
                  out.Field("runs", runs);
                  out.Field("level", locke.Level());
                  out.Field("health", locke.Health());
                  out.FieldString("weapon", weapon ? NameOf(*weapon) : "");
                  out.Field("armor", locke.ArmorValue());
                  out.FieldString("foes", names);
              });

    // Seed n is the n-th fight: the same seeds always give the same fights.
    std::string lines;
    int32_t won = 0, died = 0;
    int64_t ticks = 0;
    for (int32_t seed = 1; seed <= runs; ++seed)
    {
        TShadowDuel duel(TShadowDuel::EMode::Trial, SShadowTactics{}, uint32_t(seed));
        std::string timeline;
        if (seed <= traced)
            duel.TraceInto(&timeline);
        const SShadowFightReport r = duel.Fight(locke, foes, 0, nullptr, nullptr);
        if (seed <= traced)
            log.WriteFile(("trace-" + std::to_string(seed) + ".txt").c_str(), timeline);
        won += r.won;
        died += r.died;
        ticks += r.ticks;
        JsonOut out;
        out.Begin('{');
        out.Field("seed", seed);
        out.FieldBool("won", r.won);
        out.FieldBool("died", r.died);
        out.FieldBool("escaped", r.escaped);
        out.FieldBool("stalled", r.stalled);
        out.Field("ticks", r.ticks);
        out.Field("first", r.first);
        out.Field("swings", r.swings);
        out.Field("blows", r.blows);
        out.Field("hits", r.hits);
        out.Field("dealt", r.dealt);
        out.Field("foeswings", r.foeswings);
        out.Field("foeblows", r.foeblows);
        out.Field("foehits", r.foehits);
        out.Field("foemoves", r.foemoves);
        out.Field("taken", r.taken);
        out.Field("suffered", r.suffered);
        out.Field("blocks", r.blocks);
        out.Field("evades", r.evades);
        out.Field("potions", r.potions);
        out.End('}');
        lines += out.str() + "\n";
    }
    log.WriteFile("duels.jsonl", lines);
    log.Event("bench-done",
              std::to_string(won) + " won, " + std::to_string(died) + " lost of " + std::to_string(runs) +
                  ", mean " + std::to_string(runs ? ticks / runs : 0) + " ticks",
              [&](JsonOut& out) {
                  out.Field("won", won);
                  out.Field("died", died);
                  out.Field("runs", runs);
              });
}
