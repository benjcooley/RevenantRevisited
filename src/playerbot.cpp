// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *   playerbot.cpp - Locke played by code, the way a person plays him    *
// *************************************************************************
//
// docs/gameplay/PLAYER_BOT.md. Four parts, in the order a tick runs them:
// the senses (what a player could see on screen, read `reaction` ticks
// late), the ground (open and tight spots from the walk grid), the brain
// (one state a tick and the keys it wants), and the hands (the player's
// key bindings, pressed through the game mode's key entry).

#include "playerbot.h"

#include "character.h"
#include "combattrace.h"
#include "complexobj.h"
#include "ctrlmap.h"
#include "mappane.h"
#include "player.h"
#include "playscreen.h"
#include "revenant.h"
#include "rules.h"
#include "runtimemode.h"

#include <algorithm>
#include <cmath>

TPlayerBot PlayerBot;

namespace
{
constexpr int32_t kAware = 640;         // how far it looks for foes and missiles
constexpr int32_t kMaxStep = 0x20;      // the walk step a character takes (retail's Blocked)
constexpr int32_t kCell = 16;           // a walk cell
constexpr int32_t kSurveyTicks = 6;     // how often the ground is surveyed
constexpr int32_t kCalmTicks = 72;      // no foe this long: out of combat
constexpr int32_t kMissileHorizon = 24; // ticks ahead a missile is a threat
constexpr int32_t kAttackCommands[3] = {GAMECMD_SWING, GAMECMD_THRUST, GAMECMD_CHOP};

int32_t CellOf(int32_t v) { return (v + kCell / 2) >> 4; }

// The direction key for a move angle: UpdateMove's bit i is angle i * 32.
uint32_t DirectionFlag(int32_t angle)
{
    return angle < 0 ? 0u : 1u << (((angle & 0xff) + 16) / 32 % 8);
}

// The dodge command rolling toward a move angle: retail's eight are left,
// right, up, down, up-left, up-right, down-left, down-right (Dodge 0..7);
// the angle's direction key is up-right, right, ... up (DirectionFlag).
int32_t DodgeCommand(int32_t angle)
{
    constexpr int32_t kDodgeByKey[8] = {5, 1, 7, 3, 6, 0, 4, 2};
    return GAMECMD_DODGELEFT + kDodgeByKey[((angle & 0xff) + 16) / 32 % 8];
}

// A command's (or a held control's) keys in the control map: the first
// binding's keys, modifiers first.
bool BindingKeys(int32_t downcmd, uint32_t flag, std::vector<int32_t>& keys)
{
    keys.clear();
    for (int32_t i = 0; i < ControlMap.NumControls(); ++i)
    {
        SControlEntry ce{};
        ControlMap.GetControlEntry(i, &ce);
        const bool match = flag ? ce.cmdflag == flag : (downcmd && ce.downcmd == downcmd);
        if (!match)
            continue;
        for (int32_t code = 0; code < CODESPERCOMMAND && keys.empty(); ++code)
            for (int32_t k = 0; k < KEYSPERCODE; ++k)
                if (ce.codes[code].keys[k] > 0)
                    keys.push_back(ce.codes[code].keys[k]);
        if (!keys.empty())
            return true;
    }
    return false;
}

const char* StateName(int32_t s)
{
    static constexpr const char* kNames[] = {"idle", "standoff", "approach", "strike", "backoff", "evade",
                                             "block", "missile", "fallback"};
    return s >= 0 && s < int32_t(std::size(kNames)) ? kNames[s] : "?";
}

int32_t Length2D(const S3DPoint& v) { return int32_t(std::lround(std::hypot(double(v.x), double(v.y)))); }

// An attack that strikes: not an animation played as an "attack" (CA_PLAYANIM:
// a monster's side step, jump or hiss), not a spell.
bool IsBlow(const SCharAttackData& at) { return !(at.flags & (CA_PLAYANIM | CA_MAGICATTACK)); }
}  // namespace

// ---- Control --------------------------------------------------------------

void TPlayerBot::Enable(bool on)
{
    if (on == enabled)
        return;
    enabled = on;
    if (!on)
        ReleaseAll();
    views.clear();
    lastmissiles.clear();
    state = EState::Idle;
    surveyed = -1000;
    if (Player)
        lastpos = IntPos(Player);
}

uint32_t TPlayerBot::Random()
{
    rng ^= rng << 13;
    rng ^= rng >> 17;
    rng ^= rng << 5;
    return rng;
}

void TPlayerBot::Tick()
{
    if (!enabled)
        return;
    ++tick;
    Flush();
    if (!Player || Player->IsDead() || !PlayScreen.IsControlOn() || PlayScreen.IsDemoMode())
    {
        Hold(0);
        return;
    }
    const S3DPoint before = lastpos;
    See();
    const S3DPoint me = IntPos(Player);
  // A way held that goes nowhere (into a wall, a rock, a character the
  // shove can't get round) is left out a second.
    if (heldangle >= 0 && Length2D(me - before) == 0 && !Player->IsDoing(ACTION_ATTACK))
    {
        if (++stuckticks >= 6)
        {
            blockedangle = heldangle;
            blockeduntil = tick + 24;
            stuckticks = 0;
        }
    }
    else
        stuckticks = 0;
    if (tick - surveyed >= kSurveyTicks || std::abs(CellOf(me.x) - surveyx) > kWindow / 2 ||
        std::abs(CellOf(me.y) - surveyy) > kWindow / 2)
        Survey();
    Decide();
}

// ---- The senses --------------------------------------------------------------

void TPlayerBot::See()
{
    SView v;
    v.tick = tick;
    const S3DPoint me = IntPos(Player);
    const int32_t moved = Length2D(me - lastpos);
    if (moved > 0 && moved < 64)
        speed = speed * 0.8f + float(moved) * 0.2f;
    lastpos = me;

    SRect r{me.x - kAware, me.y - kAware, me.x + kAware, me.y + kAware};
    for (TMapIterator i(Player->GetLevel(), &r, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
    {
        auto* c = static_cast<TCharacter*>(i.Item());
        if (!c || c == Player || c->IsDead() || !(c->IsEnemy(Player) || c->Fighting() == Player))
            continue;
        SFoe f;
        f.who = c;
        f.pos = IntPos(c);
        f.edge = Player->Distance(c);
        f.bearing = Player->AngleTo(c);
      // Its reach: the longest of its blows (an attack that only plays an
      // animation -- a side step, a jump, a hiss -- strikes nothing).
        if (SCharData* cd = c->GetCharData())
        {
            for (int32_t a = 0; a < cd->attacks.NumItems(); ++a)
                if (IsBlow(cd->attacks[a]))
                    f.reach = (std::max)(f.reach, cd->attacks[a].hitmaxrange);
            if (f.reach <= 0)
                f.reach = cd->maxattackrange;
        }
      // A blow in the making, as a player reads the wind-up: the swing's
      // impact frame ahead, Locke within its reach and arc.
        const SCharAttackData* blow = c->CurrentBlow();
        if (blow && IsBlow(*blow))
        {
            const int32_t left = blow->impacttime - c->GetFrame();
            if (left >= 0)
            {
                f.swinging = true;
                f.impact = left;
                f.hitreach = blow->hitmaxrange;
                f.aimed = std::abs(AngleDiff(IntFacing(c), c->AngleTo(Player))) <= blow->hitangle;
            }
        }
        v.foes.push_back(f);
    }

  // Things flying at Locke: anything moving that isn't a character, its
  // velocity its last tick's move, its closest approach ahead.
    std::vector<std::pair<int32_t, S3DPoint>> now;
    for (TMapIterator i(Player->GetLevel(), &r, CHECK_NOINVENT, OBJSET_MOVING); i; i++)
    {
        TObjectInstance* o = i.Item();
        if (!o || o->IsCharacter())
            continue;
        now.emplace_back(o->GetMapIndex(), IntPos(o));
        auto it = std::find_if(lastmissiles.begin(), lastmissiles.end(),
                               [&](const auto& p) { return p.first == o->GetMapIndex(); });
        if (it == lastmissiles.end())
            continue;
        SMissile m;
        m.id = o->GetMapIndex();
        m.pos = IntPos(o);
        m.vel = m.pos - it->second;
        const double v2 = double(m.vel.x) * m.vel.x + double(m.vel.y) * m.vel.y;
        if (v2 < 9.0)
            continue;                                   // barely moving: not a missile
        const S3DPoint rel = me - m.pos;
        const double t = std::clamp((double(rel.x) * m.vel.x + double(rel.y) * m.vel.y) / v2, 0.0,
                                    double(kMissileHorizon));
        m.when = int32_t(t);
        m.closest = int32_t(std::hypot(rel.x - m.vel.x * t, rel.y - m.vel.y * t));
        v.missiles.push_back(m);
    }
    lastmissiles = std::move(now);

    views.push_back(std::move(v));
    while (int32_t(views.size()) > profile.reaction + 1)
        views.pop_front();
}

const TPlayerBot::SView& TPlayerBot::Seen() const
{
    return views.front();
}

// ---- The ground ------------------------------------------------------------------

TPlayerBot::SCell* TPlayerBot::Cell(int32_t gx, int32_t gy)
{
    const int32_t x = gx - surveyx + kWindow, y = gy - surveyy + kWindow;
    if (x < 0 || y < 0 || x >= kSpan || y >= kSpan)
        return nullptr;
    return &cells[size_t(y * kSpan + x)];
}

// Which cells Locke can stand on (retail's Blocked rule round a point: no
// hole within his radius, no step over 0x20), how much room each has, and
// the paths to them from where he stands.
void TPlayerBot::Survey()
{
    const S3DPoint me = IntPos(Player);
    surveyx = CellOf(me.x);
    surveyy = CellOf(me.y);
    surveyed = tick;
    const int32_t radius = Player->Radius();
    for (int32_t y = 0; y < kSpan; ++y)
        for (int32_t x = 0; x < kSpan; ++x)
        {
            SCell& c = cells[size_t(y * kSpan + x)];
            c = SCell{};
            const int32_t gx = surveyx - kWindow + x, gy = surveyy - kWindow + y;
            c.height = int16_t(MapPane.GetWalkGridHeight(gx, gy));
            if (c.height <= 0)
                continue;
            int32_t maxdelta = 0, height = 0;
            bool hole = false;
            MapPane.GetWalkHeightRadius(S3DPoint(gx * kCell, gy * kCell, c.height), radius, maxdelta, height, hole);
            c.stand = !hole && maxdelta <= kMaxStep;
        }

    constexpr int32_t kDx[8] = {1, -1, 0, 0, 1, 1, -1, -1};
    constexpr int32_t kDy[8] = {0, 0, 1, -1, 1, -1, 1, -1};
    std::vector<int32_t> queue;
    queue.reserve(cells.size());

  // Clearance: cells to the nearest place Locke can't stand (the window's
  // edge counts as open).
    for (size_t i = 0; i < cells.size(); ++i)
    {
        cells[i].clear = cells[i].stand ? int16_t(kSpan) : int16_t(0);
        if (!cells[i].stand)
            queue.push_back(int32_t(i));
    }
    for (size_t head = 0; head < queue.size(); ++head)
    {
        const int32_t i = queue[head], x = i % kSpan, y = i / kSpan;
        for (int32_t d = 0; d < 8; ++d)
        {
            const int32_t nx = x + kDx[d], ny = y + kDy[d];
            if (nx < 0 || ny < 0 || nx >= kSpan || ny >= kSpan)
                continue;
            SCell& n = cells[size_t(ny * kSpan + nx)];
            if (n.clear > cells[size_t(i)].clear + 1)
            {
                n.clear = int16_t(cells[size_t(i)].clear + 1);
                queue.push_back(ny * kSpan + nx);
            }
        }
    }

  // Paths from Locke: steps between standable cells within the walk step.
    queue.clear();
    const int32_t start = kWindow * kSpan + kWindow;
    cells[size_t(start)].steps = 0;
    queue.push_back(start);
    for (size_t head = 0; head < queue.size(); ++head)
    {
        const int32_t i = queue[head], x = i % kSpan, y = i / kSpan;
        const SCell& from = cells[size_t(i)];
        for (int32_t d = 0; d < 8; ++d)
        {
            const int32_t nx = x + kDx[d], ny = y + kDy[d];
            if (nx < 0 || ny < 0 || nx >= kSpan || ny >= kSpan)
                continue;
            SCell& n = cells[size_t(ny * kSpan + nx)];
            if (!n.stand || n.steps >= 0 || std::abs(n.height - from.height) > kMaxStep)
                continue;
            n.steps = int16_t(from.steps + 1);
            n.from = int16_t(i);
            queue.push_back(ny * kSpan + nx);
        }
    }
}

int32_t TPlayerBot::Clearance(const S3DPoint& p)
{
    const SCell* c = Cell(CellOf(p.x), CellOf(p.y));
    return c ? c->clear : kSpan;
}

bool TPlayerBot::OpenSpot(S3DPoint& spot)
{
    const SView& v = views.back();
    const int32_t here = Clearance(IntPos(Player));
    double best = -1e9;
    int32_t bestcell = -1;
    for (int32_t i = 0; i < int32_t(cells.size()); ++i)
    {
        const SCell& c = cells[size_t(i)];
        if (c.steps < 0 || c.clear < profile.openness + 2 || c.clear <= here + 1)
            continue;
        const S3DPoint p((surveyx - kWindow + i % kSpan) * kCell, (surveyy - kWindow + i / kSpan) * kCell, c.height);
        int32_t nearest = kAware;
        for (const SFoe& f : v.foes)
            nearest = (std::min)(nearest, Length2D(p - f.pos));
        const double score = (std::min<int32_t>)(c.clear, 10) * 10.0 - c.steps * 0.6 + (std::min)(nearest, 320) / 16.0;
        if (score > best)
        {
            best = score;
            bestcell = i;
        }
    }
    if (bestcell < 0)
        return false;
    spot = S3DPoint((surveyx - kWindow + bestcell % kSpan) * kCell, (surveyy - kWindow + bestcell / kSpan) * kCell,
                    cells[size_t(bestcell)].height);
    return true;
}

int32_t TPlayerBot::PathAngle(const S3DPoint& spot)
{
    const SCell* goal = Cell(CellOf(spot.x), CellOf(spot.y));
    if (!goal || goal->steps < 0)
        return -1;
  // Walk back from the spot to the cell three steps out from Locke.
    int32_t i = int32_t(goal - cells.data());
    while (cells[size_t(i)].steps > 3 && cells[size_t(i)].from >= 0)
        i = cells[size_t(i)].from;
    const S3DPoint waypoint((surveyx - kWindow + i % kSpan) * kCell, (surveyy - kWindow + i / kSpan) * kCell,
                            cells[size_t(i)].height);
    return ConvertToFacing(IntPos(Player), waypoint);
}

int32_t TPlayerBot::Steer(int32_t angle, bool room)
{
    const S3DPoint me = IntPos(Player);
    const SCell* here = Cell(CellOf(me.x), CellOf(me.y));
    const int32_t blocked = DirectionFlag(tick < blockeduntil ? blockedangle : -1);
    int32_t best = -1, bestclear = -1;
    for (const int32_t turn : {0, 32, -32, 64, -64})
    {
        const int32_t a = (angle + turn) & 0xff;
        if (blocked && DirectionFlag(a) == uint32_t(blocked))
            continue;
        S3DPoint step;
        ConvertToVector(a, (std::max)(int32_t(speed * 3.0f), kCell), step);
        const SCell* c = Cell(CellOf(me.x + step.x), CellOf(me.y + step.y));
        if (!c || !c->stand || (here && std::abs(c->height - here->height) > kMaxStep))
            continue;
        if (!room)
            return a;
        if (c->clear > bestclear)
        {
            best = a;
            bestclear = c->clear;
        }
    }
    return best;
}

// ---- The brain --------------------------------------------------------------------

void TPlayerBot::SetState(EState s, const char* why)
{
    if (s == state)
        return;
    state = s;
    if (CombatTrace::Enabled())
        CombatTrace::Event(Player, "bot", "state=%s\twhy=%s", StateName(int32_t(s)), why);
}

void TPlayerBot::Decide()
{
    const SView& now = views.back();
    const SView& seen = Seen();
    const S3DPoint me = IntPos(Player);

  // The foe to fight: the nearest.
    const SFoe* foe = nullptr;
    for (const SFoe& f : now.foes)
        if (!foe || f.edge < foe->edge)
            foe = &f;
    if (!foe)
    {
        heldangle = -1;
        Hold(0);
        SetState(EState::Idle, "no foe");
        if (Player->IsCombat() && tick - calmsince > kCalmTicks)
            Tap(GAMECMD_COMBAT);
        return;
    }
    calmsince = tick;
    if (!Player->IsCombat() && foe->edge <= foe->reach + 160)
        Tap(GAMECMD_COMBAT);

    const int32_t standoff = foe->reach + profile.margin;
    const int32_t radius = Player->Radius();
    auto side_with_room = [&](int32_t bearing) {
        S3DPoint l, r;
        ConvertToVector((bearing + 64) & 0xff, 48, r);
        ConvertToVector((bearing - 64) & 0xff, 48, l);
        return Clearance(me + r) >= Clearance(me + l) ? 1 : -1;
    };
    auto go = [&](int32_t angle, bool room) {
        heldangle = Steer(angle, room);
        Hold(DirectionFlag(heldangle));
    };
    auto move = [&](int32_t angle) { go(angle, false); };      // toward something
    auto shift = [&](int32_t angle) { go(angle, true); };      // away or aside: where there's room

  // Holding a block until the blow it meets has passed.
    if (tick < blockuntil)
    {
        Hold(CMDFLAG_BLOCK);
        return;
    }

  // 1. A blow coming (as seen `reaction` ticks ago): step out of its reach
  //    or arc if there's time, else block or dodge, by the profile's odds.
    for (const SFoe& f : seen.foes)
    {
        if (!f.swinging || !f.aimed)
            continue;
        const int32_t left = f.impact - (tick - seen.tick);
        const TCharacter* who = f.who.Get();
        if (left < 0 || !who)
            continue;
        const int32_t edge = Player->Distance(who);
        if (edge > f.hitreach + 8)
            continue;
        const int32_t backticks = int32_t(std::ceil(float(f.hitreach + 8 - edge) / speed));
        const int32_t sideticks = int32_t(std::ceil(float(radius * 2 + 8) / speed));
        const int32_t bearing = Player->AngleTo(who);
        if (sideticks <= left)
        {
            SetState(EState::Evade, "blow coming: side step");
            shift(bearing + 64 * side_with_room(bearing));
            return;
        }
        if (backticks <= left)
        {
            SetState(EState::Evade, "blow coming: back away");
            shift(bearing + 128);
            return;
        }
        const float roll = Chance();
        if (roll < profile.block)
        {
            SetState(EState::Block, "blow coming: block");
            blockuntil = tick + left + 2;
            Hold(CMDFLAG_BLOCK);
            return;
        }
        if (roll < profile.block + profile.dodge)
        {
            SetState(EState::Evade, "blow coming: dodge");
            Hold(0);
            Tap(DodgeCommand(bearing + 64 * side_with_room(bearing)));
            return;
        }
    }

  // 2. A missile coming: across its path, to the side with room.
    for (const SMissile& m : seen.missiles)
    {
        if (m.closest > radius + 16 || m.when > kMissileHorizon)
            continue;
        const int32_t heading = ConvertToFacing(S3DPoint(0, 0, 0), m.vel);
        const S3DPoint rel = me - m.pos;
        const int32_t cross = int32_t((int64_t(m.vel.x) * rel.y - int64_t(m.vel.y) * rel.x) > 0 ? -1 : 1);
        SetState(EState::Missile, "missile coming");
        shift(heading + 64 * cross);
        return;
    }

  // 3. A tight spot, the foe not on top of him: fall back to open ground,
  //    and keep going till there (or the foe closes, or it takes too long).
    if (state == EState::Fallback)
    {
        const bool there = Length2D(fallbackspot - me) <= 2 * kCell;
        const int32_t angle = there ? -1 : PathAngle(fallbackspot);
        if (there || angle < 0 || tick >= fallbackuntil || foe->edge <= foe->reach + 8)
        {
            fallbackcooldown = tick + 48;
            SetState(EState::Standoff, there ? "open ground" : "fall back over");
        }
        else
        {
            move(angle);
            return;
        }
    }
    if (tick >= fallbackcooldown && Clearance(me) < profile.openness && foe->edge > foe->reach &&
        !Player->IsDoing(ACTION_ATTACK) && OpenSpot(fallbackspot))
    {
        const int32_t angle = PathAngle(fallbackspot);
        if (angle >= 0)
        {
            fallbackuntil = tick + 72;
            SetState(EState::Fallback, "tight spot");
            move(angle);
            return;
        }
    }

  // 4. Offense: let a swing play out, then back off to the standoff band.
    if (Player->IsDoing(ACTION_ATTACK))
    {
        Hold(0);
        SetState(EState::Strike, "swinging");
        return;
    }
    if (state == EState::Strike)
        SetState(EState::BackOff, "swing done");
    if (state == EState::BackOff)
    {
        if (foe->edge < standoff)
        {
            shift(foe->bearing + 128);
            return;
        }
        SetState(EState::Standoff, "out of reach");
    }

  // Coming in: to the chosen attack's range, then press it.
    const SCharData* cd = Player->GetCharData();
    if (state == EState::Approach && cd && attackbutton >= 1)
    {
        int32_t range = 0;          // where the blow lands: its reach, within the distance it can be used at
        for (int32_t a = 0; a < cd->attacks.NumItems(); ++a)
        {
            const SCharAttackData& at = cd->attacks[a];
            if (at.button == attackbutton && IsBlow(at))
                range = (std::max)(range, at.maxdist > 0 ? (std::min)(at.hitmaxrange, at.maxdist) : at.hitmaxrange);
        }
        const bool threatened = foe->swinging && foe->aimed && foe->impact <= 6;
        if (threatened || range <= 0)
            SetState(EState::Standoff, "closing called off");
        else if (foe->edge <= range - 4)
        {
            Hold(0);
            Tap(kAttackCommands[attackbutton - 1]);
            SetState(EState::Strike, "in range: strike");
            return;
        }
        else
        {
            move(foe->bearing);
            return;
        }
    }

  // Deciding to come in: the foe open, Locke rested enough.
    const bool rested = Player->Fatigue() * 4 > Player->MaxFatigue();
    if (cd && rested && !foe->swinging && tick % 12 == 0 && Chance() < profile.aggression)
    {
        std::vector<int32_t> buttons;
        for (int32_t a = 0; a < cd->attacks.NumItems(); ++a)
        {
            const SCharAttackData& at = cd->attacks[a];
            if (at.button >= 1 && at.button <= 3 && IsBlow(at) && at.hitmaxrange > 0 &&
                std::find(buttons.begin(), buttons.end(), at.button) == buttons.end())
                buttons.push_back(at.button);
        }
        if (!buttons.empty())
        {
            attackbutton = buttons[Random() % buttons.size()];
            SetState(EState::Approach, "foe open");
            move(foe->bearing);
            return;
        }
    }

  // 5. Standing off: hold the band just outside its reach, weaving.
    SetState(EState::Standoff, "waiting");
    if (foe->edge < standoff - 8)
        shift(foe->bearing + 128);
    else if (foe->edge > standoff + 32)
        move(foe->bearing);
    else
    {
        if (tick >= weaveuntil)
        {
            weaveside = Chance() < 0.7f ? side_with_room(foe->bearing) : -side_with_room(foe->bearing);
            weaveuntil = tick + Between(profile.weaveMin, profile.weaveMax);
        }
        shift(foe->bearing + 64 * weaveside);
    }
}

// ---- The hands --------------------------------------------------------------------

void TPlayerBot::Key(int32_t key, bool down)
{
    if (IRuntimeMode* mode = CurrentMode())
        mode->HandleKey(key, down);
}

void TPlayerBot::Hold(uint32_t cmdflags)
{
  // The keys for the wanted controls.
    std::vector<int32_t> want, keys;
    for (uint32_t bit = 1; bit <= CMDFLAG_BLOCK; bit <<= 1)
        if ((cmdflags & bit) && BindingKeys(0, bit, keys))
            want.insert(want.end(), keys.begin(), keys.end());

  // A control the game let go of (the player's Stop releases every one)
  // while its key is still down takes a fresh press, as a player's would.
    uint32_t live = 0, changed = 0;
    ControlMap.GetCommandFlags(live, changed);
    for (uint32_t bit = 1; bit <= CMDFLAG_BLOCK; bit <<= 1)
        if ((cmdflags & bit) && (heldflags & bit) && !(live & bit) && BindingKeys(0, bit, keys))
            for (int32_t k : keys)
                if (std::find(heldkeys.begin(), heldkeys.end(), k) != heldkeys.end())
                {
                    Key(k, false);
                    heldkeys.erase(std::find(heldkeys.begin(), heldkeys.end(), k));
                }

    for (size_t i = heldkeys.size(); i-- > 0;)
        if (std::find(want.begin(), want.end(), heldkeys[i]) == want.end())
        {
            Key(heldkeys[i], false);
            heldkeys.erase(heldkeys.begin() + std::ptrdiff_t(i));
        }
    for (int32_t k : want)
        if (std::find(heldkeys.begin(), heldkeys.end(), k) == heldkeys.end())
        {
            Key(k, true);
            heldkeys.push_back(k);
        }
    heldflags = cmdflags;
}

void TPlayerBot::Tap(int32_t command)
{
    std::vector<int32_t> keys;
    if (!BindingKeys(command, 0, keys))
        return;
    for (int32_t k : keys)
        Key(k, true);
    tapkeys.insert(tapkeys.end(), keys.begin(), keys.end());
    if (CombatTrace::Enabled())
        CombatTrace::Event(Player, "bot", "tap=%d", command);
}

void TPlayerBot::Flush()
{
    for (size_t i = tapkeys.size(); i-- > 0;)
        Key(tapkeys[i], false);
    tapkeys.clear();
}

void TPlayerBot::ReleaseAll()
{
    Flush();
    for (size_t i = heldkeys.size(); i-- > 0;)
        Key(heldkeys[i], false);
    heldkeys.clear();
    heldflags = 0;
}
