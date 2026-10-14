// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *       shadowduel.cpp - Shadow Smoke's fights, without the animation     *
// *************************************************************************

#include "shadowduel.h"

#include "shadowledger.h"
#include "shadowlog.h"
#include "shadowtravel.h"

#include "../character.h"
#include "../player.h"
#include "../revenant.h"
#include "../revutils.h"
#include "../rules.h"
#include "../textencoding.h"

#include <algorithm>
#include <cmath>
#include <optional>

namespace {

constexpr int32_t kTicksPerSecond = 24;
// No fight of the game's runs this long; past it the fight has stalled.
constexpr int32_t kFightLimit = 5 * 60 * kTicksPerSecond;
// At most this many monsters crowd a player at once; the rest wait their turn.
constexpr int32_t kMaxInReach = 3;
// Drinking from the belt takes about a second of the fight.
constexpr int32_t kDrinkTicks = kTicksPerSecond;
// The closest a monster fights from (some char.def blocks give 0).
constexpr int32_t kMinReach = 10;

// The flags of attacks the timeline doesn't model: those that need a mode
// other than combat, a target held or stunned, an animation only, a spell
// (§6.8), a move or a run under way.
constexpr int32_t kUnmodelled = CA_PLAYANIM | CA_MAGICATTACK | CA_SNEAKMODE | CA_WALKMODE | CA_BOWMODE |
                                CA_INTERACTIVE | CA_ATTACKSTUN | CA_ATTACKDOWN | CA_MOVING | CA_RUNNING |
                                CA_ACTION;

// A trial fight borrows the game's random stream for Shadow's own, so the
// game's dice code throws them without moving the game's stream. (The
// game's draw counter still counts them; only a playtest replay reads it.)
std::mt19937* lentstream = nullptr;
int32_t LentRandom()
{
    return int32_t((*lentstream)() & 0x7fff);
}
struct SLendRandom
{
    explicit SLendRandom(std::mt19937& stream)
    {
        lentstream = &stream;
        SetRandomSource(&LentRandom);
    }
    ~SLendRandom()
    {
        SetRandomSource(nullptr);
        lentstream = nullptr;
    }
    SLendRandom(const SLendRandom&) = delete;
    SLendRandom& operator=(const SLendRandom&) = delete;
};

int32_t Distance2D(const S3DPoint& a, const S3DPoint& b)
{
    return (int32_t)std::lround(std::hypot(double(a.x - b.x), double(a.y - b.y)));
}

std::string NameOf(TCharacter& who)
{
    const char* name = who.GetName() && who.GetName()[0] ? who.GetName() : who.GetTypeName();
    return ToUtf8(name ? name : "?");
}

// Fatigue a tick that isn't spent attacking (TCharacter::PulseRecovery).
double Recovery(TCharacter& who)
{
    double perTick = Rules.fatiguerecovval * (100.0 / 24.0) / std::max(Rules.fatiguerecovrate, 1);
    if (who.ObjClass() == OBJCLASS_PLAYER)
        perTick += static_cast<TPlayer&>(who).FatiguePct() / 1440.0;
    return perTick;
}

} // namespace

int32_t TShadowDuel::Draw(int32_t low, int32_t high)
{
    return std::uniform_int_distribution<int32_t>(low, high)(random);
}

int32_t TShadowDuel::Health(const SFighter& f) const
{
    return mode == EMode::Live ? f.who->Health() : f.health;
}

bool TShadowDuel::Usable(SFighter& f, const SCharAttackData& a, int32_t dist, int32_t now) const
{
    // IsValidAttack's tests that don't read animation state.
    if (a.flags & kUnmodelled)
        return false;
    if (a.responsename[0])
        return false;               // a response needs the target in a state
    if (!f.who->HasActionAni(a.attackname))
        return false;
    if (a.flags & CA_FATIGUEATTACK)
    {
        if (f.fatigue > a.maxfatigue)
            return false;
    }
    else if (f.fatigue < a.fatigue)
        return false;
    if (a.maxdist > 0 && (dist < a.mindist || dist > a.maxdist))
        return false;
    if ((a.flags & (CA_CHAIN | CA_AUTOCOMBO)) && a.chainname[0])
    {
        if (!f.last || stricmp(f.last->attackname, a.chainname) != 0 || now - f.lasttick > f.last->chainexptime)
            return false;
    }
    if (f.locke)
    {
        auto& player = static_cast<TPlayer&>(*f.who);
        if (!(a.weaponmask & (1 << player.WeaponType())))
            return false;
        if (player.AttackLevel() < a.attackskill)
            return false;
        if (player.Skill(SK_WEAPONSKILLS + player.WeaponType()) < a.weaponskill)
            return false;
        if (!strcmp(a.attackname, "sunsetflipper"))
            return false;           // Baez's alone, in the arena (IsValidAttack)
    }
    return true;
}

void TShadowDuel::Start(SFighter& f, SFighter& target, const SCharAttackData& attack, int32_t now,
                        int32_t dmgpcnt)
{
    // The dice, thrown as the attack is chosen (IsValidAttack): a target
    // that is swinging itself is open to it.
    const SAttackRoll roll = f.who->RollAttack(*target.who, attack, dmgpcnt, target.swing != nullptr);
    f.swing = &attack;
    f.at = &target;
    f.impact = now + std::max(1, attack.impacttime);
    f.busy = now + std::max(attack.nextwait, attack.impacttime + 1);
    f.damage = roll.Tiered();
    f.tohit = roll.tohit;
    f.roll = roll.roll;
    f.read = false;
    f.last = &attack;
    f.lasttick = now;

    // A monster attacked may guard (OnAttacked): at BLOCKFREQ, for
    // BLOCKMIN..BLOCKMAX, while the blow is early enough to catch.
    if (!target.locke && !target.swing && target.guard < now)
    {
        const SCharData* data = target.who->GetCharData();
        if (::random(1, 100) <= data->blockfreq)
            target.guard = now + ::random(data->blockmin, data->blockmax);
    }
}

void TShadowDuel::Land(SFighter& f, int32_t now, SShadowFightReport& report, TShadowLog* log)
{
    SFighter& target = *f.at;
    const SCharAttackData& attack = *f.swing;
    f.swing = nullptr;
    f.impact = -1;
    f.fatigue = std::max(0.0, f.fatigue - attack.fatigue);       // paid at the impact
    if (Health(f) <= 0 || Health(target) <= 0 || target.retreating)
        return;

    const bool guarding = target.guard >= now;
    const SBlowOutcome blow = TCharacter::WeighBlow(f.damage, f.tohit, f.roll, guarding);
    // A guarding player takes nothing from a glance (ResolveHit).
    const bool lands = blow.hit || !(target.locke && guarding);
    if (mode == EMode::Live)
        f.who->LandBlow(*target.who, blow.damage, lands, nullptr);
    else if (lands)
        target.health -= blow.damage;
    if (f.locke)
    {
        ++report.blows;
        report.hits += blow.hit;
        report.dealt += lands ? blow.damage : 0;
    }
    else
    {
        report.taken += lands;
        report.suffered += lands ? blow.damage : 0;
    }
    if (log && mode == EMode::Live)
        log->Event("blow",
                   NameOf(*f.who) + " " + attack.attackname + " on " + NameOf(*target.who) + ": to-hit " +
                       std::to_string(f.tohit) + ", roll " + std::to_string(f.roll) + ", " +
                       (blow.blocked ? "guarded" : blow.hit ? "hit" : "glance") + " for " +
                       std::to_string(lands ? blow.damage : 0),
                   [&](JsonOut& out) {
                       out.Field("t", now);
                       out.FieldString("by", NameOf(*f.who));
                       out.FieldString("attack", attack.attackname);
                       out.FieldString("on", NameOf(*target.who));
                       out.Field("tohit", f.tohit);
                       out.Field("roll", f.roll);
                       out.Field("damage", lands ? blow.damage : 0);
                       out.FieldBool("hit", blow.hit);
                       out.FieldBool("guarded", blow.blocked);
                   });

    if (target.locke || Health(target) > 0)
    {
        // A monster at its RETREATAT runs (TCharacter::Damage); it gets away
        // when it outruns him.
        if (!target.locke && Health(target) <= target.who->GetCharData()->retreatat)
        {
            target.retreating = true;
            if (ShadowMoveSpeed(*target.who, {"runf", "combatf", "walkf"}) >=
                ShadowMoveSpeed(*f.who, {"runf"}))
                report.escaped = true;
            else
                target.ready = now + kTicksPerSecond * 2;      // he catches it
        }
        return;
    }
    report.killed.push_back(NameOf(*target.who));
}

void TShadowDuel::MonsterTurn(SFighter& m, SFighter& locke, int32_t now)
{
    if (m.swing || now < m.busy || now < m.nextattack)
        return;
    // RandomAttack: a percentage, the damage percentage, then up to twice as
    // many random picks as it has attacks (FindPcntAttack).
    const SCharData* data = m.who->GetCharData();
    const int32_t pcnt = ::random(1, 100);
    const int32_t dmgpcnt = TCharacter::RollDamagePercent();
    const int32_t n = data->attacks.NumItems();
    for (int32_t k = 0; k < n * 2; ++k)
    {
        const SCharAttackData& attack = data->attacks[::random(0, n - 1)];
        if (attack.attackpcnt >= pcnt && Usable(m, attack, m.reach, now))
        {
            Start(m, locke, attack, now, dmgpcnt);
            break;
        }
    }
    // AIAttack re-arms its timer from ATTACKFREQ after every try.
    m.nextattack = now + ::random(data->minattackfreq * FRAMERATE / 100, data->maxattackfreq * FRAMERATE / 100);
}

void TShadowDuel::LockeTurn(SFighter& me, std::vector<SFighter>& foes, int32_t now, int64_t tick,
                            SShadowFightReport& report, TShadowLedger* ledger, TShadowLog* log)
{
    if (me.swing || now < me.busy)
        return;
    auto& player = static_cast<TPlayer&>(*me.who);

    // Below his floor, a healing potion.
    if (Health(me) * 100 < player.MaxHealth() * tactics.floorpct)
        for (const char* potion : {"Lesser Healing", "Lesser Restoration", "Greater Healing", "Greater Restoration",
                                   "Enchanted Healing", "Enchanted Restoration"})
            if (TObjectInstance* item = player.FindObjInventory(potion))
            {
                const int32_t gives = item->FindStat("Health") >= 0 ? item->GetStat("Health") : 0;
                if (mode == EMode::Live)
                {
                    if (ledger)
                        ledger->AllowVitals(std::string("drink ") + potion, gives, 0, tick + 2);
                    item->Use(me.who, -1);
                    if (log)
                        log->Event("drink", potion);
                }
                else
                    me.health = std::min(player.MaxHealth(), me.health + gives);
                ++report.potions;
                me.busy = now + kDrinkTicks;
                return;
            }

    // A blow he sees coming in time: guard, as often as he does -- once a
    // blow, when he first reads it.
    for (SFighter& f : foes)
        if (f.swing && !f.read && f.at == &me && f.impact - now >= tactics.reaction && me.guard < now &&
            Health(f) > 0)
        {
            f.read = true;
            if (Draw(1, 100) <= tactics.blockpct)
            {
                me.guard = f.impact + 1;
                me.busy = f.impact + 1;
                ++report.blocks;
                return;
            }
            break;
        }

    // Strike the foe nearest death, with the best attack valid at its range:
    // a chain link when one is open, else the hardest-hitting blow, never
    // the same as the last two (the same-button rule, GRIND.md §2.4).
    SFighter* target = nullptr;
    for (SFighter& f : foes)
        if (Health(f) > 0 && !f.retreating && f.ready <= now && (!target || Health(f) < Health(*target)))
            target = &f;
    if (!target)
        return;
    const SCharData* data = player.GetCharData();
    const SCharAttackData* best = nullptr;
    for (int32_t i = 0; i < data->attacks.NumItems(); ++i)
    {
        const SCharAttackData& a = data->attacks[i];
        if (!Usable(me, a, target->reach, now))
            continue;
        // The fatigued blow does 80% less (GRIND.md §2.4): he steps back
        // and gets his breath instead, unless it finishes the foe.
        if ((a.flags & CA_FATIGUEATTACK) && Health(*target) > 2)
            continue;
        const bool chain = (a.flags & (CA_CHAIN | CA_AUTOCOMBO)) && a.chainname[0];
        const bool repeat = a.button == me.button1 && a.button == me.button2;
        if (repeat && !chain)
            continue;
        if (!best || chain > (((best->flags & (CA_CHAIN | CA_AUTOCOMBO)) && best->chainname[0])) ||
            a.damagemod > best->damagemod)
            best = &a;
    }
    if (!best)
        return;                     // nothing he can throw: he waits for his fatigue
    me.button2 = me.button1;
    me.button1 = best->button;
    Start(me, *target, *best, now, 0);
}

SShadowFightReport TShadowDuel::Fight(TPlayer& locke, const std::vector<TCharacter*>& foes, int64_t tick,
                                      TShadowLedger* ledger, TShadowLog* log)
{
    std::optional<SLendRandom> lent;
    if (mode == EMode::Trial)
        lent.emplace(random);

    SShadowFightReport report;
    SFighter me;
    me.who = &locke;
    me.locke = true;
    me.health = locke.Health();
    me.fatigue = locke.Fatigue();

    std::vector<SFighter> them;
    them.reserve(foes.size());
    for (TCharacter* foe : foes)
    {
        if (!foe || foe->Health() <= 0)
            continue;
        SFighter f;
        f.who = foe;
        f.health = foe->Health();
        f.fatigue = foe->Fatigue();
        f.reach = std::max(kMinReach, foe->GetCharData()->maxattackrange);
        const double speed = ShadowMoveSpeed(*foe, {"runf", "combatf", "walkf"});
        const int32_t gap = std::max(0, Distance2D(foe->Pos(), locke.Pos()) - f.reach);
        f.ready = speed > 0 ? int32_t(gap / speed) : 0;
        f.nextattack = f.ready;     // its timer is spent once it is fighting
        them.push_back(f);
    }
    if (mode == EMode::Live && ledger)
        ledger->AllowProgress("fight", tick + 2);

    int32_t now = 0;
    for (; now < kFightLimit; ++now)
    {
        for (SFighter& f : them)
            if (f.impact == now)
                Land(f, now, report, log);
        if (me.impact == now)
            Land(me, now, report, log);
        if (Health(me) <= 0)
        {
            report.died = true;
            break;
        }
        const bool anyleft = std::any_of(them.begin(), them.end(), [&](const SFighter& f) {
            return Health(f) > 0 && !(f.retreating && report.escaped);
        });
        if (!anyleft)
        {
            report.won = !report.escaped;
            break;
        }
        int32_t inreach = 0;
        for (SFighter& f : them)
        {
            if (Health(f) <= 0 || f.retreating || now < f.ready)
                continue;
            if (++inreach > kMaxInReach)
            {
                f.ready = now + 1;  // waits at the edge
                continue;
            }
            MonsterTurn(f, me, now);
        }
        LockeTurn(me, them, now, tick, report, ledger, log);
        // Fatigue comes back to whoever isn't swinging.
        if (!me.swing)
            me.fatigue = std::min<double>(locke.MaxFatigue(), me.fatigue + Recovery(locke));
        for (SFighter& f : them)
            if (!f.swing)
                f.fatigue = std::min<double>(f.who->MaxFatigue(), f.fatigue + Recovery(*f.who));
    }
    report.ticks = now;
    report.stalled = now >= kFightLimit;

    if (mode == EMode::Live)
    {
        // Locke's fatigue as the fight leaves it, less what the fight's ticks,
        // which run next, give back as he stands (ARCHITECTURE.md §6.5).
        const double after = std::max(0.0, me.fatigue - Recovery(locke) * report.ticks);
        if (after < locke.Fatigue())
            locke.SetFatigue((int32_t)after);
        if (log)
            log->Event("fight",
                       std::string(report.won ? "won" : report.died ? "Locke died" : report.escaped ? "it got away"
                                                                                    : "stalled") +
                           " in " + std::to_string(report.ticks) + " ticks: " + std::to_string(report.hits) + "/" +
                           std::to_string(report.blows) + " blows hit for " + std::to_string(report.dealt) +
                           ", took " + std::to_string(report.taken) + " for " + std::to_string(report.suffered),
                       [&](JsonOut& out) {
                           out.FieldBool("won", report.won);
                           out.FieldBool("died", report.died);
                           out.FieldBool("escaped", report.escaped);
                           out.Field("ticks", report.ticks);
                           out.Field("blows", report.blows);
                           out.Field("hits", report.hits);
                           out.Field("dealt", report.dealt);
                           out.Field("taken", report.taken);
                           out.Field("suffered", report.suffered);
                           out.Field("blocks", report.blocks);
                           out.Field("potions", report.potions);
                           out.Key("killed").Begin('[');
                           for (const std::string& name : report.killed)
                               out.String(name);
                           out.End(']');
                       });
    }
    return report;
}
