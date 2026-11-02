// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *       shadowduel.cpp - Shadow Smoke's fights, without the animation     *
// *************************************************************************

#include "game/shadow/shadowduel.h"

#include "game/shadow/shadowledger.h"
#include "game/shadow/shadowlog.h"
#include "game/shadow/shadowpack.h"
#include "game/shadow/shadowtravel.h"

#include "game/imagery/3dimage.h"
#include "game/object/character.h"
#include "game/object/player.h"
#include "game/app/revenant.h"
#include "game/app/revutils.h"
#include "game/script/rules.h"
#include "engine/core/textencoding.h"

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
// The game's angles: 256 to a turn. Facing further than kAway from the one
// it faces is turned away: open to his blows (IsValidAttack), and a guard
// that doesn't count (ResolveHit).
constexpr double kTurn = 256.0;
constexpr double kRadiansPerTurn = 6.283185307179586;
constexpr double kAway = 0x30;
// The angle between two of the eight direction keys.
constexpr double kKeyAngle = kTurn / 8;
// A guard is refused this far from its foe (Block 0x4d2e30).
constexpr double kGuardRange = 120.0;
// A monster that hasn't noticed Locke looks for a target once every 32
// frames (the AI's acquisition, COMBAT_ATTACK_CHOICE.md §3.10.1).
constexpr int32_t kAcquireTicks = 32;
// Out of sight in its retreat, a monster has got away.
constexpr double kSight = 640.0;
// The player bot's habits that aren't SShadowTactics' (PLAYER_BOT.md §2-3):
// its weave strafes 18..40 ticks, 70% of them to the side with more room
// (on open ground, the same side); an
// approach is called off when a blow will land within kThreat ticks; it
// stops kStrikeShort inside its blow's range to press it, and treats a
// blow as coming while within its reach plus kReadMargin.
constexpr int32_t kWeaveMin = 18;
constexpr int32_t kWeaveMax = 40;
constexpr int32_t kWeaveSidePct = 70;
constexpr int32_t kThreat = 6;
constexpr double kStrikeShort = 4.0;
constexpr double kReadMargin = 8.0;
// A swing's own motion stops this close to its target (ResolveAttack: no
// lunge runs through the one it is aimed at).
constexpr double kLungeStop = 10.0;
// Attacks the timeline can't make: spells (§6.8), and those made in a mode
// other than the combat stance (sneaking, walking, the bow, running).
constexpr int32_t kUnmodelled = CA_MAGICATTACK | CA_SNEAKMODE | CA_WALKMODE | CA_BOWMODE | CA_RUNNING;

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

double Radians(double angle)
{
    return angle * kRadiansPerTurn / kTurn;
}

SShadowPoint Direction(double angle)
{
    return {std::cos(Radians(angle)), std::sin(Radians(angle))};
}

double Bearing(const SShadowPoint& from, const SShadowPoint& to)
{
    const double a = std::atan2(to.y - from.y, to.x - from.x) * kTurn / kRadiansPerTurn;
    return a < 0.0 ? a + kTurn : a;
}

// An angle in [0, 256).
double Wrap(double angle)
{
    const double a = std::fmod(angle, kTurn);
    return a < 0.0 ? a + kTurn : a;
}

// a - b, in (-128, 128].
double AngleOff(double a, double b)
{
    double d = std::fmod(a - b, kTurn);
    if (d > kTurn / 2)
        d -= kTurn;
    else if (d <= -kTurn / 2)
        d += kTurn;
    return d;
}

std::string NameOf(TCharacter& who)
{
    const char* name = who.GetName() && who.GetName()[0] ? who.GetName() : who.GetTypeName();
    return ToUtf8(name ? name : "?");
}

// Fatigue a tick (TCharacter::PulseRecovery, which runs whatever the
// character is doing short of running).
double Recovery(TCharacter& who)
{
    double perTick = Rules.fatiguerecovval * (100.0 / 24.0) / std::max(Rules.fatiguerecovrate, 1);
    if (who.ObjClass() == OBJCLASS_PLAYER)
        perTick += static_cast<TPlayer&>(who).FatiguePct() / 1440.0;
    return perTick;
}

// The prefix of a character's combat animations (AnimPrefix in the combat
// root): a monster's "c"; Locke's "h" barehanded, "c" with a weapon.
std::string Prefix(TCharacter& who)
{
    if (who.ObjClass() != OBJCLASS_PLAYER)
        return "c";
    return static_cast<TPlayer&>(who).WeaponType() == WT_HAND ? "h" : "c";
}

// An attack that strikes: not a move played as an attack (a monster's side
// step, jump or hiss), not a spell.
bool IsBlow(const SCharAttackData& a)
{
    return !(a.flags & (CA_PLAYANIM | CA_MAGICATTACK));
}

// How far a character's blows reach: the longest of them (the bot's reading
// of a foe, PLAYER_BOT.md §1), its MAXATTACKRANGE when it has none.
double Reach(TCharacter& who)
{
    const SCharData* data = who.GetCharData();
    int32_t reach = 0;
    for (int32_t i = 0; i < data->attacks.NumItems(); ++i)
        if (IsBlow(data->attacks[i]))
            reach = std::max(reach, data->attacks[i].hitmaxrange);
    return reach > 0 ? reach : data->maxattackrange;
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

double TShadowDuel::Edge(const SFighter& a, const SFighter& b) const
{
    // TCharacter::Distance: centre to centre less both radii.
    return std::max(0.0, std::hypot(a.pos.x - b.pos.x, a.pos.y - b.pos.y) - a.radius - b.radius);
}

const SShadowAnim* TShadowDuel::Anim(TCharacter& who, const std::string& name)
{
    auto* imagery = dynamic_cast<T3DImagery*>(who.GetImagery());
    if (!imagery || !imagery->HasState(name.c_str()))
        return nullptr;
    const auto key = std::make_pair(static_cast<const void*>(imagery), name);
    if (const auto found = anims.find(key); found != anims.end())
        return &found->second;

    // The state's frames, one a tick, and their root motion as
    // SetObjectMotion applies it: each frame moves the character `dist`
    // along its facing turned by `ang`. FindState is given its percent so it
    // draws nothing from the game's random stream.
    SShadowAnim anim;
    const int32_t state = imagery->FindState(name.c_str(), 100);
    anim.frames = std::max(1, imagery->GetAniLength(state));
    const bool still = (imagery->GetAniFlags(state) & AF_NOMOTION) != 0;
    for (int32_t frame = 0; frame < anim.frames; ++frame)
    {
        int32_t dist = 0, vert = 0, ang = 0, rotx = 0, roty = 0, rotz = 0;
        SShadowPoint step;
        if (!still && imagery->GetMotion(state, frame, dist, vert, ang, rotx, roty, rotz))
        {
            const SShadowPoint way = Direction(ang);
            step = {way.x * dist / ROLLOVER, way.y * dist / ROLLOVER};
        }
        anim.steps.push_back(step);
    }
    return &anims.emplace(key, std::move(anim)).first->second;
}

void TShadowDuel::Note(int32_t now, const SFighter& f, const std::string& what)
{
    if (!trace)
        return;
    char where[96];
    snprintf(where, sizeof(where), "\t(%.0f,%.0f) face %.0f", f.pos.x, f.pos.y, f.face);
    *trace += std::to_string(now) + "\t" + NameOf(*f.who) + "\t" + what + where + "\n";
}

bool TShadowDuel::Open(const SFighter& target, const SFighter& attacker) const
{
    // IsValidAttack: a target facing away, or in an attack of its own -- a
    // blow or a move.
    return std::abs(AngleOff(target.face, Bearing(target.pos, attacker.pos))) >= kAway ||
           target.act == EAct::Blow || target.act == EAct::Move;
}

bool TShadowDuel::Strikes(const SFighter& f, const SFighter& target, const SCharAttackData& attack) const
{
    // ResolveHit: the target within the blow's range and inside its arc, as
    // the attacker faces when the blow falls.
    const double edge = Edge(f, target);
    if (edge < attack.hitminrange || edge > attack.hitmaxrange)
        return false;
    return std::abs(AngleOff(f.face, Bearing(f.pos, target.pos))) <= attack.hitangle;
}

void TShadowDuel::Walk(SFighter& f, double angle, double speed)
{
    // A player steps the way one of the eight direction keys points
    // (UpdateMove's bit i is angle i * 32), the nearest to where he means to
    // go; so a weave or a back-off drifts in or out a little. A new key sets
    // his facing, which then holds while the key does (the bot's trace): the
    // forward key faces the way it points, rounded off his foe; another
    // faces his foe as the step begins.
    if (f.locke)
    {
        angle = std::round(angle / kKeyAngle) * kKeyAngle;
        if (angle != f.key && f.target)
        {
            // He turns to it at the step's turn rate (Go: 8 a tick, 4 more
            // for each 32 past the first 32 he has to turn).
            const double toward = Bearing(f.pos, f.target->pos);
            f.aim = std::abs(AngleOff(angle, toward)) <= kKeyAngle / 2 ? angle : toward;
            f.turnrate = std::max(0.0, std::abs(AngleOff(f.aim, f.face)) - 32.0) / 32.0 * 4.0 + 8.0;
        }
        f.key = angle;
        f.face = Wrap(f.face + std::clamp(AngleOff(f.aim, f.face), -f.turnrate, f.turnrate));
    }
    const SShadowPoint step = Direction(angle);
    f.pos.x += step.x * speed;
    f.pos.y += step.y * speed;
    f.moved = true;
}

void TShadowDuel::StartAction(SFighter& f, EAct act, const SShadowAnim* anim, int32_t now, int32_t length)
{
    f.act = act;
    f.anim = anim;
    f.animat = now;
    // An animation of n frames holds its last a tick before the next begins.
    f.until = now + (length > 0 ? length : anim ? anim->frames + 1 : 1);
}

void TShadowDuel::Advance(SFighter& f, int32_t now)
{
    if (f.act == EAct::Dead)
        return;
    // The action's frame for this tick carries him on: a blow, a flinch or
    // a guard along the facing it began with; a move with the facing turned
    // to his foe each tick (ResolveAttack re-aims a PLAYANIM at its target),
    // so a monster's side step or run circles him.
    const int32_t frame = now - f.animat - 1;
    const bool lunging = f.act == EAct::Blow && f.target && now < f.impact;
    if (f.anim && f.act != EAct::Stand && frame >= 0 && frame < f.anim->frames &&
        !(lunging && Edge(f, *f.target) <= kLungeStop))
    {
        if (f.act == EAct::Move && f.target)
            f.face = Bearing(f.pos, f.target->pos);
        const SShadowPoint step = f.anim->steps[size_t(frame)];
        const SShadowPoint ahead = Direction(f.face), aside = Direction(f.face + kTurn / 4);
        f.pos.x += ahead.x * step.x + aside.x * step.y;
        f.pos.y += ahead.y * step.x + aside.y * step.y;
    }
    if (f.act != EAct::Stand && now >= f.until)
    {
        f.act = EAct::Stand;
        f.anim = nullptr;
        f.doing = "";
        f.impactflags = 0;
    }
}

bool TShadowDuel::Usable(const SFighter& f, const SFighter& target, const SCharAttackData& a, int32_t pcnt,
                         int32_t now) const
{
    // IsValidAttack's tests before the dice, in its order; the search
    // applies its own flag mask and button.
    if (a.flags & kUnmodelled)
        return false;
    if (!f.who->HasActionAni(a.attackname))
        return false;
    if (a.flags & CA_FATIGUEATTACK)
    {
        if (f.fatigue > a.maxfatigue)
            return false;
    }
    else if (f.fatigue < a.fatigue)
        return false;
    if (a.attackpcnt < pcnt)
        return false;
    const double edge = Edge(f, target);
    if (a.maxdist > 0 && (edge < a.mindist || edge > a.maxdist))
        return false;
    if (a.flags & CA_PLAYANIM)
        return !f.locke;            // a monster's move; Locke's rolls have no button
    // A monster attacks only when its AI's timer has run out.
    if (!f.locke && f.nextattack != 0)
        return false;
    if ((a.flags & CA_MOVING) && !f.moved)
        return false;
    // What the target is doing: stunned, knocked down, the state a response
    // answers.
    if ((a.flags & CA_ATTACKSTUN) && !(target.act == EAct::Flinch && (target.impactflags & CAI_STUN)))
        return false;
    if ((a.flags & CA_ATTACKDOWN) && !(target.act == EAct::Flinch && (target.impactflags & CAI_KNOCKDOWN)))
        return false;
    if (a.responsename[0] && stricmp(target.doing, a.responsename) != 0)
        return false;
    // A chain link follows its attack in time.
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
        if (!strcmp(a.attackname, "sunsetflipper") && stricmp(NameOf(*target.who).c_str(), "Baez") != 0)
            return false;
    }
    return true;
}

int32_t TShadowDuel::AfterDice(const SFighter& target, const SCharAttackData& a, int32_t damage) const
{
    // IsValidAttack after the dice: a death blow only when it kills; then the
    // impact the target will play, the first, or a later death impact when
    // the blow kills -- refused when the target has no animation for it.
    const int32_t health = Health(target);
    if ((a.flags & CA_DEATH) && damage < health)
        return kRefused;
    const bool kills = damage >= health;
    int32_t impact = -1;
    for (int32_t k = 0; k < a.numimpacts; ++k)
    {
        const SCharAttackImpact& imp = a.impacts[k];
        if (!(kills && (imp.flags & CAI_DEATH)) && impact >= 0)
            continue;
        if (!(imp.flags & CAI_DEATH))
        {
            const std::string name =
                (imp.flags & CAI_INTERACTIVE) ? std::string(imp.impactname) : Prefix(*target.who) + imp.impactname;
            if (!target.who->HasActionAni(name.c_str()))
                return kRefused;
        }
        impact = k;
    }
    return impact;
}

TShadowDuel::SPick TShadowDuel::ButtonSearch(SFighter& f, SFighter& target, int32_t button, int32_t now)
{
    // FindButtonAttack: responses, then specials, then plain attacks, each in
    // table order. A candidate that gets as far as the dice tiers them again
    // (retail's compounding): a death blow that won't kill, tried before the
    // plain blow of the same key, leaves its tier on it.
    const SCharData* data = f.who->GetCharData();
    const int32_t dmgpcnt = f.who->StrengthMod();      // a player's, in the damage percentage's place
    const bool open = Open(target, f);
    SAttackRoll dice;
    for (const int32_t want : {CA_RESPONSE, CA_SPECIAL, 0})
        for (int32_t i = 0; i < data->attacks.NumItems(); ++i)
        {
            const SCharAttackData& a = data->attacks[i];
            if ((a.flags & (CA_RESPONSE | CA_SPECIAL)) != want || a.button != button || !Usable(f, target, a, 0, now))
                continue;
            const SAttackRoll rolled = f.who->RollAttack(*target.who, a, dmgpcnt, open, dice);
            dice = {rolled.Tiered(), rolled.tohit, rolled.roll};
            const int32_t impact = AfterDice(target, a, dice.damage);
            if (impact == kRefused)
                continue;
            return {&a, dice.damage, dice.tohit, dice.roll, impact};
        }
    return {};
}

TShadowDuel::SPick TShadowDuel::PcntSearch(SFighter& f, SFighter& target, int32_t now)
{
    // AIAttack's RandomAttack: a percentage, the damage percentage, then
    // FindPcntAttack's up to twice as many random picks as there are
    // attacks, the first valid one taken. A move needs no dice.
    const SCharData* data = f.who->GetCharData();
    const int32_t pcnt = ::random(1, 100);
    const int32_t dmgpcnt = TCharacter::RollDamagePercent();
    const bool open = Open(target, f);
    const int32_t n = data->attacks.NumItems();
    SAttackRoll dice;
    for (int32_t k = 0; k < n * 2; ++k)
    {
        const SCharAttackData& a = data->attacks[::random(0, n - 1)];
        if (!Usable(f, target, a, pcnt, now))
            continue;
        if (a.flags & CA_PLAYANIM)
            return {&a};
        const SAttackRoll rolled = f.who->RollAttack(*target.who, a, dmgpcnt, open, dice);
        dice = {rolled.Tiered(), rolled.tohit, rolled.roll};
        const int32_t impact = AfterDice(target, a, dice.damage);
        if (impact == kRefused)
            continue;
        return {&a, dice.damage, dice.tohit, dice.roll, impact};
    }
    return {};
}

void TShadowDuel::StartBlow(SFighter& f, SFighter& target, const SPick& pick, int32_t now,
                            SShadowFightReport& report)
{
    const SCharAttackData& attack = *pick.attack;
    StartAction(f, EAct::Blow, Anim(*f.who, attack.attackname), now, 0);
    f.doing = attack.attackname;
    f.target = &target;
    f.swing = &attack;
    f.impact = now + std::max(1, attack.impacttime);
    f.until = std::max(f.until, f.impact + 1);
    f.damage = pick.damage;
    f.tohit = pick.tohit;
    f.roll = pick.roll;
    f.impactnum = pick.impactnum;
    f.read = false;
    f.last = &attack;
    f.lasttick = now;
    Note(now, f, std::string("swing ") + attack.attackname + " edge " + std::to_string(int32_t(Edge(f, target))) +
                     " to-hit " + std::to_string(pick.tohit) + " roll " + std::to_string(pick.roll) + " damage " +
                     std::to_string(pick.damage) + " impact " + std::to_string(f.impact));
    if (f.locke)
    {
        ++report.swings;
        f.button2 = f.button1;
        f.button1 = attack.button;
        // A blow that will only glance makes a monster raise its guard as the
        // swing begins (IsValidAttack); then the swing tells it it is being
        // attacked (OnAttacked).
        if (pick.tohit - pick.roll <= Rules.tohitdamage[3].minvalue)
            MonsterGuard(target, f, now, true);
        MonsterGuard(target, f, now, false);
    }
    else
        ++report.foeswings;
}

void TShadowDuel::MonsterGuard(SFighter& m, const SFighter& attacker, int32_t now, bool forced)
{
    // OnAttacked: at BLOCKFREQ (an attack it can still catch: before its
    // blocktime), and IsValidAttack's forced guard; Block refuses a
    // flinching or far monster. A guard cuts short what it was doing -- a
    // move, or its own blow.
    if (m.act == EAct::Dead)
        return;
    const SCharData* data = m.who->GetCharData();
    if (!forced && (attacker.swing->blocktime <= 0 || ::random(1, 100) > data->blockfreq))
        return;
    if (m.act == EAct::Flinch || m.act == EAct::Guard || Edge(m, attacker) > kGuardRange)
        return;
    const SShadowAnim* anim = Anim(*m.who, Prefix(*m.who) + "block");
    if (!anim)
        return;
    m.swing = nullptr;
    m.impact = -1;
    m.face = Bearing(m.pos, attacker.pos);
    StartAction(m, EAct::Guard, anim, now, ::random(data->blockmin, data->blockmax));
    m.doing = "block";
    Note(now, m, forced ? "guard (forced)" : "guard");
}

void TShadowDuel::Flinch(SFighter& f, const SCharAttackData& attack, int32_t impactnum, int32_t now)
{
    // The impact the blow carries plays on the one it struck (ResolveHit),
    // cutting short whatever he was doing -- his own swing too. A stun or a
    // knockdown holds its loop a while longer.
    if (impactnum < 0 || f.act == EAct::Dead)
        return;
    const SCharAttackImpact& imp = attack.impacts[impactnum];
    const std::string name =
        (imp.flags & (CAI_DEATH | CAI_INTERACTIVE)) ? std::string(imp.impactname) : Prefix(*f.who) + imp.impactname;
    const SShadowAnim* anim = Anim(*f.who, name);
    if (!anim)
        return;
    f.swing = nullptr;
    f.impact = -1;
    const int32_t hold = (imp.flags & (CAI_STUN | CAI_KNOCKDOWN)) ? imp.looptime : 0;
    StartAction(f, EAct::Flinch, anim, now, anim->frames + 1 + hold);
    f.doing = imp.impactname;
    f.impactflags = imp.flags;
    Note(now, f, "flinch " + name + " until " + std::to_string(f.until));
}

void TShadowDuel::Land(SFighter& f, int32_t now, SShadowFightReport& report, TShadowLog* log)
{
    SFighter& target = *f.target;
    const SCharAttackData& attack = *f.swing;
    f.swing = nullptr;
    f.impact = -1;
    f.fatigue = std::max(0.0, f.fatigue - attack.fatigue);       // paid at the impact, struck or not
    if (Health(f) <= 0 || Health(target) <= 0)
        return;
    if (!Strikes(f, target, attack))
    {
        Note(now, f, std::string("whiff ") + attack.attackname + " edge " + std::to_string(int32_t(Edge(f, target))) +
                         " off " + std::to_string(int32_t(AngleOff(f.face, Bearing(f.pos, target.pos)))));
        return;
    }

    // A guard counts against a blow the target faces.
    const bool guarding = target.act == EAct::Guard &&
                          std::abs(AngleOff(target.face, Bearing(target.pos, f.pos))) < kAway;
    SBlowOutcome blow = TCharacter::WeighBlow(f.damage, f.tohit, f.roll, guarding);
    if (attack.flags & CA_INTERACTIVE)
    {
        // A hold lands on a target on the move that isn't blocking; it can't
        // kill without a death impact, and one that fails does nothing.
        const bool moving = target.act == EAct::Move || target.moved;
        if (moving && !(target.act == EAct::Guard && !strcmp(target.doing, "block")))
            blow.hit = true;
        const bool deathimpact =
            f.impactnum >= 0 && (attack.impacts[f.impactnum].flags & CAI_DEATH);
        if (blow.hit && Health(target) - blow.damage < 1 && !deathimpact)
            blow.hit = false;
        if (!blow.hit)
            return;
    }
    // A guarding player takes nothing from a glance (ResolveHit).
    const bool lands = blow.hit || !(target.locke && guarding);
    if (mode == EMode::Live)
        f.who->LandBlow(*target.who, blow.damage, lands, nullptr);
    else if (lands)
        target.health -= blow.damage;
    if (report.first < 0)
        report.first = now;
    Note(now, f, std::string(blow.hit ? "hit " : "glance ") + attack.attackname + " for " +
                     std::to_string(lands ? blow.damage : 0) + (guarding ? " (guarded)" : ""));
    if (f.locke)
    {
        ++report.blows;
        report.hits += blow.hit;
        report.dealt += lands ? blow.damage : 0;
    }
    else
    {
        ++report.foeblows;
        report.foehits += blow.hit;
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

    if (Health(target) <= 0)
    {
        target.act = EAct::Dead;
        target.swing = nullptr;
        target.impact = -1;
        if (!target.locke)
            report.killed.push_back(NameOf(*target.who));
        return;
    }
    if (lands)
        Flinch(target, attack, f.impactnum, now);
    // A hit that leaves a monster at its RETREATAT sends it running for
    // RETREATFOR (TCharacter::Damage, the AI's retreat state).
    if (!target.locke && lands)
    {
        const SCharData* data = target.who->GetCharData();
        if (Health(target) <= data->retreatat && data->retreatfor > 0)
            target.retreat = now + data->retreatfor;
    }
}

void TShadowDuel::MonsterTurn(SFighter& m, SFighter& locke, int32_t now, SShadowFightReport& report)
{
    if (m.act != EAct::Stand)
        return;
    const SCharData* data = m.who->GetCharData();
    m.moved = false;
    // Retreating, it runs straight away from him (Go at AngleTo + 127).
    if (m.retreat >= now)
    {
        m.face = Bearing(locke.pos, m.pos);
        Walk(m, m.face, m.walk);
        if (Edge(m, locke) > kSight)
            report.escaped = true;
        return;
    }
    // Out of its MAXATTACKRANGE it steps in (the AI's move branch).
    m.face = Bearing(m.pos, locke.pos);
    if (Edge(m, locke) > data->maxattackrange)
    {
        Walk(m, m.face, m.walk);
        return;
    }
    // The attack branch, every tick in the combat stance (a monster with no
    // spell has its magic timer at 0): the timer down, a try -- a move any
    // time, a blow only once the timer is 0 -- then down again unless the
    // last attack it made was a move, and re-armed from ATTACKFREQ once
    // below 0 (§3.10.2). "The last attack" is the one before this try's: the
    // game records an attack as it starts to play, the next pulse, and a
    // failed try forgets it. So a blow after a move leaves the timer at 0,
    // and a second blow may follow straight on, as in real fights.
    if (m.nextattack > 0)
        --m.nextattack;
    const SPick pick = PcntSearch(m, locke, now);
    if (!pick.attack)
        m.last = nullptr;
    const bool aftermove = m.last && (m.last->flags & CA_PLAYANIM);
    if (pick.attack && (pick.attack->flags & CA_PLAYANIM))
    {
        StartAction(m, EAct::Move, Anim(*m.who, pick.attack->attackname), now, 0);
        m.doing = pick.attack->attackname;
        m.last = pick.attack;
        m.lasttick = now;
        ++report.foemoves;
        Note(now, m, std::string("move ") + pick.attack->attackname + " edge " +
                         std::to_string(int32_t(Edge(m, locke))));
    }
    else if (pick.attack)
        StartBlow(m, locke, pick, now, report);
    if (!aftermove)
        --m.nextattack;
    if (m.nextattack < 0)
        m.nextattack = ::random(data->minattackfreq * FRAMERATE / 100, data->maxattackfreq * FRAMERATE / 100);
}

void TShadowDuel::LockeTurn(SFighter& me, std::vector<SFighter>& foes, int32_t now, int64_t tick,
                            SShadowFightReport& report, TShadowLedger* ledger, TShadowLog* log)
{
    me.stepped = me.moved;
    me.moved = false;
    LockeChoices(me, foes, now, tick, report, ledger, log);
    if (!me.moved)
        me.key = -1.0;              // no key held: the next step faces anew
}

void TShadowDuel::LockeChoices(SFighter& me, std::vector<SFighter>& foes, int32_t now, int64_t tick,
                               SShadowFightReport& report, TShadowLedger* ledger, TShadowLog* log)
{
    // Swinging, he can still guard (a block cuts his own swing short);
    // flinching, drinking or already guarding, he does nothing new.
    const bool swinging = me.act == EAct::Blow;
    if (me.act != EAct::Stand && !swinging)
        return;
    auto& player = static_cast<TPlayer&>(*me.who);

    // The foe he fights: the nearest (the bot's).
    SFighter* foe = nullptr;
    for (SFighter& f : foes)
        if (Health(f) > 0 && f.act != EAct::Dead && (!foe || Edge(me, f) < Edge(me, *foe)))
            foe = &f;
    if (!foe)
        return;
    const double edge = Edge(me, *foe);
    const double bearing = Bearing(me.pos, foe->pos);
    if (!swinging)
        me.target = foe;

    // Below his floor, the belt's healing potion: in a fight a player drinks
    // from the action bar (F5-F9), nothing else.
    if (!swinging && Health(me) * 100 < player.MaxHealth() * tactics.floorpct)
        if (TObjectInstance* potion = ShadowBeltHealing(player, player.MaxHealth() - Health(me), drunk))
        {
            const int32_t gives = potion->FindStat("Health") >= 0 ? potion->GetStat("Health") : 0;
            const std::string name = ToUtf8(potion->GetName());
            if (mode == EMode::Live)
            {
                if (ledger)
                    ledger->AllowVitals("drink " + name, gives, 0, tick + 2);
                potion->Use(me.who, -1);
                if (log)
                    log->Event("drink", name);
            }
            else
            {
                ++drunk[potion];
                me.health = std::min(player.MaxHealth(), me.health + gives);
            }
            ++report.potions;
            StartAction(me, EAct::Drink, nullptr, now, kDrinkTicks);
            return;
        }

    // 1. A blow coming at him, read once it has been under way longer than
    //    his reaction: out of its reach or arc if there is time, else guard
    //    or roll aside at the tactics' odds -- each tick until it lands --
    //    else take it. Mid-swing his feet don't answer (Go refuses outside
    //    the stance) and a roll needs the stance; only the guard is left.
    for (SFighter& f : foes)
    {
        if (!f.swing || f.target != &me || now - f.animat <= tactics.reaction)
            continue;
        const int32_t left = f.impact - now;
        const double reach = f.swing->hitmaxrange;
        if (left < 0 || Edge(me, f) > reach + kReadMargin ||
            std::abs(AngleOff(f.face, Bearing(f.pos, me.pos))) > f.swing->hitangle)
            continue;
        const double toward = Bearing(me.pos, f.pos);
        const int32_t sideticks = int32_t(std::ceil((2.0 * me.radius + kReadMargin) / me.walk));
        const int32_t backticks = int32_t(std::ceil((reach + kReadMargin - Edge(me, f)) / me.walk));
        if (sideticks <= left || backticks <= left)
        {
            if (!swinging)
            {
                Walk(me, toward + (sideticks <= left ? kTurn / 4 : kTurn / 2), me.walk);
                if (!f.read)
                    ++report.evades;
                f.read = true;
            }
            return;
        }
        const int32_t roll = Draw(1, 100);
        if (roll <= tactics.blockpct)
        {
            me.swing = nullptr;
            me.impact = -1;
            me.face = bearing;
            StartAction(me, EAct::Guard, Anim(*me.who, Prefix(*me.who) + "block"), now, left + 2);
            me.doing = "block";
            ++report.blocks;
            return;
        }
        // A roll needs the combat stance (Dodge): one tried mid-step, as the
        // weaving bot mostly is, does nothing (real fights: most dodges guard
        // nothing).
        if (roll <= tactics.blockpct + tactics.dodgepct && !swinging && !me.stepped)
        {
            // A dodge counts as a guard (ResolveHit), and rolls him aside.
            const SShadowAnim* rolling = Anim(*me.who, Prefix(*me.who) + "rollr");
            StartAction(me, EAct::Guard, rolling, now, rolling ? 0 : left + 2);
            me.doing = "dodge";
            ++report.blocks;
            return;
        }
    }

    if (swinging)
        return;

    // 2. Coming in, striking, backing off (the bot's offense).
    const double standoff = Reach(*foe->who) + tactics.margin;
    if (me.plan == EPlan::BackOff)
    {
        if (edge < standoff)
        {
            Walk(me, bearing + kTurn / 2, me.walk);
            return;
        }
        me.plan = EPlan::Standoff;
    }
    const SCharData* data = player.GetCharData();
    if (me.plan == EPlan::Approach)
    {
        // Where his blow lands: the longest reach among the key's blows,
        // within the distance each can be made from.
        double range = 0.0;
        for (int32_t i = 0; i < data->attacks.NumItems(); ++i)
        {
            const SCharAttackData& a = data->attacks[i];
            if (a.button == me.button && IsBlow(a))
                range = std::max<double>(range, a.maxdist > 0 ? std::min(a.hitmaxrange, a.maxdist) : a.hitmaxrange);
        }
        const bool threatened = foe->swing && foe->target == &me && foe->impact - now <= kThreat;
        if (threatened || range <= 0.0)
            me.plan = EPlan::Standoff;
        else if (edge <= range - kStrikeShort)
        {
            // The key pressed: his step under way carries him on as the blow
            // begins (the bot's trace: two steps from the decision to the
            // swing), and a moving attack may be made.
            Walk(me, bearing, me.walk);
            const SPick pick = ButtonSearch(me, *foe, me.button, now);
            me.moved = false;
            if (pick.attack)
                StartBlow(me, *foe, pick, now, report);
            me.plan = EPlan::BackOff;
            return;
        }
        else
        {
            Walk(me, bearing, me.walk);
            return;
        }
    }

    // A moment to come in: rested, the foe not winding up a blow.
    const bool rested = me.fatigue * 4 > player.MaxFatigue();
    if (rested && !foe->swing && (now + phase) % tactics.window == 0 && Chance(tactics.aggression))
    {
        // Any key with a blow, but never the same one three times running
        // (the third gets roll 100, GRIND.md §2.4).
        std::vector<int32_t> keys;
        for (int32_t i = 0; i < data->attacks.NumItems(); ++i)
        {
            const SCharAttackData& a = data->attacks[i];
            if (a.button >= 1 && a.button <= 3 && IsBlow(a) && a.hitmaxrange > 0 &&
                std::find(keys.begin(), keys.end(), a.button) == keys.end() &&
                !(a.button == me.button1 && a.button == me.button2))
                keys.push_back(a.button);
        }
        if (!keys.empty())
        {
            me.button = keys[size_t(Draw(0, int32_t(keys.size()) - 1))];
            me.plan = EPlan::Approach;
            Walk(me, bearing, me.walk);
            return;
        }
    }

    // 3. Holding off: the band just outside its reach, weaving.
    if (edge < standoff - kReadMargin)
        Walk(me, bearing + kTurn / 2, me.walk);
    else if (edge > standoff + 4 * kReadMargin)
        Walk(me, bearing, me.walk);
    else
    {
        if (now >= me.weaveuntil)
        {
            me.weaveside = Chance(kWeaveSidePct) ? 1 : -1;
            me.weaveuntil = now + Draw(kWeaveMin, kWeaveMax);
        }
        Walk(me, bearing + me.weaveside * kTurn / 4, me.walk);
    }
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
    me.radius = locke.Radius();
    const S3DPoint stand = standat ? *standat : IntPos(locke);
    me.pos = {double(stand.x), double(stand.y)};
    me.walk = std::max(1.0, ShadowMoveSpeed(locke, {locke.WeaponType() == WT_HAND ? "handf" : "combatf", "walkf"}));

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
        f.radius = foe->Radius();
        f.pos = {double(IntPos(foe).x), double(IntPos(foe).y)};
        f.face = Bearing(f.pos, me.pos);
        f.walk = std::max(1.0, ShadowMoveSpeed(*foe, {"combatf", "walkf", "runf"}));
        f.target = &me;
        // One already fighting him is at it; another notices him at its next
        // look round (once every 32 frames).
        if (foe->Fighting() != &locke)
            StartAction(f, EAct::Idle, nullptr, 0, Draw(1, kAcquireTicks));
        them.push_back(f);
    }
    // He starts facing the nearest of them.
    if (!them.empty())
    {
        const auto nearest = std::min_element(them.begin(), them.end(), [&](const SFighter& a, const SFighter& b) {
            return Edge(me, a) < Edge(me, b);
        });
        me.face = me.aim = Bearing(me.pos, nearest->pos);
    }
    if (mode == EMode::Live && ledger)
        ledger->AllowProgress("fight", tick + 2);

    // The bot's moments to come in fall on its own clock, not the fight's.
    phase = Draw(0, tactics.window - 1);
    int32_t now = 0;
    for (; now < kFightLimit; ++now)
    {
        // He decides on the world as the last tick left it (the bot reads
        // its senses before the tick), then everything moves.
        LockeTurn(me, them, now, tick, report, ledger, log);
        Advance(me, now);
        for (SFighter& f : them)
            Advance(f, now);
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
        const bool anyleft =
            std::any_of(them.begin(), them.end(), [&](const SFighter& f) { return Health(f) > 0; });
        if (!anyleft || report.escaped)
        {
            report.won = !anyleft;
            break;
        }
        int32_t inreach = 0;
        for (SFighter& f : them)
        {
            if (Health(f) <= 0)
                continue;
            if (f.act == EAct::Stand && f.retreat < now &&
                Edge(f, me) <= f.who->GetCharData()->maxattackrange && ++inreach > kMaxInReach)
                continue;           // waits at the edge
            MonsterTurn(f, me, now, report);
        }
        if (trace && !them.empty())
        {
            static constexpr const char* kPlans[] = {"standoff", "approach", "backoff"};
            static constexpr const char* kActs[] = {"idle", "stand", "blow", "move", "guard", "flinch", "drink", "dead"};
            const SFighter& first = them.front();
            const char* foeact = first.act == EAct::Stand && first.moved ? (first.retreat >= now ? "flee" : "walk")
                                                                         : kActs[int32_t(first.act)];
            Note(now, me, std::string("tick ") + kPlans[int32_t(me.plan)] + " " + kActs[int32_t(me.act)] + " foe " +
                              foeact + " edge " +
                              std::to_string(int32_t(Edge(me, them.front()))) + " foe at (" +
                              std::to_string(int32_t(them.front().pos.x)) + "," +
                              std::to_string(int32_t(them.front().pos.y)) + ") fatigue " +
                              std::to_string(int32_t(me.fatigue)) + "/" +
                              std::to_string(int32_t(them.front().fatigue)));
        }
        me.fatigue = std::min<double>(locke.MaxFatigue(), me.fatigue + Recovery(locke));
        for (SFighter& f : them)
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
                           out.Field("swings", report.swings);
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
