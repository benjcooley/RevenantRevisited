// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *       shadowtravel.cpp - Shadow Smoke's travel: a proven walk, then go  *
// *************************************************************************

#include "shadowtravel.h"

#include "shadowledger.h"
#include "shadowlog.h"
#include "shadowencounters.h"
#include "shadowplan.h"

#include "../3dimage.h"
#include "../exit.h"
#include "../mappane.h"
#include "../player.h"
#include "../revenant.h"
#include "../rules.h"
#include "../textencoding.h"

#include <algorithm>
#include <charconv>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstdlib>

namespace {

// He keeps this much fatigue in hand while running: about the dearest blow
// barehanded (GRIND.md §2.5, 12-24), so he arrives able to fight.
constexpr double kFatigueReserve = 24.0;
// Closer than this, he is there already.
constexpr int32_t kThere = 24;
// How far around a place or an object the destination may be looked for.
constexpr int32_t kPlaceSlack = 96;
constexpr int32_t kApproachSlack = 64;
// An object that isn't a character takes this much room in the approach.
constexpr int32_t kObjectRadius = 24;
// The proof samples the path this often.
constexpr int32_t kProofSpacing = 256;
// A step into a trigger zone costs this much more (A* units; a straight
// step is 10): a walk goes up to about 60 cells out of its way to keep out.
constexpr int32_t kZoneStepCost = 600;
// More stops in zones on one travel than this is a loop.
constexpr int32_t kMaxCrossings = 8;
// After a zone fires: control back for this long means its scene is over.
constexpr int32_t kZoneSettle = 2 * 24;
// Through an exit: how long it may take to set off once he is on it, and
// how far he must have gone for it to count.
constexpr int32_t kExitTicks = 10 * 24;
constexpr int32_t kExitJump = 200;

int32_t Distance2D(const S3DPoint& a, const S3DPoint& b)
{
    return (int32_t)std::lround(std::hypot(double(a.x - b.x), double(a.y - b.y)));
}

// `type@x,y`: the name and the point it was given.
bool SplitAt(const std::string& name, std::string& base, S3DPoint& at)
{
    const size_t sign = name.find('@');
    const size_t comma = name.find(',', sign + 1);
    if (sign == std::string::npos || comma == std::string::npos)
        return false;
    base = name.substr(0, sign);
    const char* first = name.data() + sign + 1;
    const char* end = name.data() + name.size();
    return std::from_chars(first, name.data() + comma, at.x).ptr == name.data() + comma &&
           std::from_chars(name.data() + comma + 1, end, at.y).ptr == end;
}

void WritePoint(JsonOut& out, const char* key, const S3DPoint& p)
{
    out.Key(key).Begin('[').Value(p.x).Value(p.y).Value(p.z).End(']');
}

} // namespace

SShadowPace PaceLeg(int32_t length, int32_t fatigue, int32_t maxfatigue, const SShadowGait& gait)
{
    SShadowPace pace;
    if (length <= 0 || gait.walk <= 0.0)
        return pace;
    double f = fatigue;
    double covered = 0.0;
    double ran = 0.0;
    bool running = gait.run > 0.0 && f > kFatigueReserve;
    while (covered < length)
    {
        if (running && f > kFatigueReserve)
        {
            covered += gait.run;
            ran += gait.run;
            f -= gait.rundrain;
        }
        else
        {
            // Walk until fatigue is full again, then run on.
            running = false;
            covered += gait.walk;
            f = std::min<double>(maxfatigue, f + gait.recovery);
            if (gait.run > 0.0 && f >= maxfatigue)
                running = true;
        }
        ++pace.ticks;
    }
    pace.fatigue = std::clamp<int32_t>((int32_t)std::lround(f), 0, maxfatigue);
    pace.ran = (int32_t)std::lround(std::min<double>(ran, length));
    pace.walked = length - pace.ran;
    return pace;
}

const TShadowWalkField* TShadowNav::Field(TShadowLog& log)
{
    const int32_t level = Player->GetLevel();
    const int32_t radius = Player->Radius();
    if (field.Current(level, radius))
        return &field;
    const auto start = std::chrono::steady_clock::now();
    if (!field.Build(radius))
        return nullptr;
    const double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
    log.Event("walkfield", "level " + std::to_string(level) + ": " + std::to_string(field.Regions()) +
                               " regions for radius " + std::to_string(radius) + " in " +
                               std::to_string((int32_t)ms) + " ms",
              [&](JsonOut& out) {
                  out.Field("level", level);
                  out.Field("radius", radius);
                  out.Field("regions", field.Regions());
                  out.Field("ms", (int32_t)ms);
              });
    return &field;
}

double ShadowMoveSpeed(TObjectInstance& who, std::initializer_list<const char*> states)
{
    // The root motion of a state's frames, one frame a tick, in world units.
    // FindState is given its percent so it draws nothing from the game's
    // random stream.
    auto* imagery = dynamic_cast<T3DImagery*>(who.GetImagery());
    if (!imagery)
        return 0.0;
    for (const char* name : states)
    {
        if (!imagery->HasState(name))
            continue;
        const int32_t state = imagery->FindState(name, 100);
        const int32_t frames = imagery->GetAniLength(state);
        int64_t sum = 0;
        for (int32_t frame = 0; frame < frames; ++frame)
        {
            int32_t dist = 0, vert = 0, ang = 0, rotx = 0, roty = 0, rotz = 0;
            if (imagery->GetMotion(state, frame, dist, vert, ang, rotx, roty, rotz))
                sum += dist;
        }
        return frames > 0 ? double(sum) / frames / ROLLOVER : 0.0;
    }
    return 0.0;
}

const SShadowGait& TShadowNav::Gait(TShadowLog& log)
{
    if (gait)
        return *gait;
    SShadowGait g;
    // "walk" and "run" are the stances a mode stands in; walking and running
    // forward are "walkf" and "runf".
    g.walk = ShadowMoveSpeed(*Player, {"walkf"});
    g.run = ShadowMoveSpeed(*Player, {"runf"});
    // Points every `rate` hundredths of a second (TCharacter::PulseRecovery),
    // a tick being 100/24 hundredths.
    if (const SCharData* data = Player->GetCharData())
        g.rundrain = data->runfatigue[0] * (100.0 / 24.0) / std::max(data->runfatigue[1], 1);
    g.recovery = Rules.fatiguerecovval * (100.0 / 24.0) / std::max(Rules.fatiguerecovrate, 1) +
                 Player->FatiguePct() / 1440.0;
    gait = g;
    char text[160];
    snprintf(text, sizeof(text), "walk %.2f, run %.2f units a tick; running costs %.2f fatigue a tick, rest gives %.2f",
             g.walk, g.run, g.rundrain, g.recovery);
    log.Event("gait", text, [&](JsonOut& out) {
        out.Field("walk_milli", (int64_t)std::lround(g.walk * 1000));
        out.Field("run_milli", (int64_t)std::lround(g.run * 1000));
        out.Field("rundrain_milli", (int64_t)std::lround(g.rundrain * 1000));
        out.Field("recovery_milli", (int64_t)std::lround(g.recovery * 1000));
    });
    return *gait;
}

const TShadowZones& TShadowNav::Zones(TShadowLog& log)
{
    if (zones.Level() != Player->GetLevel())
        zones.Build(Player->GetLevel(), log);
    return zones;
}

TObjectInstance* ShadowFindObject(const std::string& name, int32_t level, const S3DPoint& near)
{
    std::string base = name;
    S3DPoint at = near;
    SplitAt(name, base, at);

    // An instance name first; failing that, a type name (a monster or an NPC
    // with no name of its own answers to its type, as in the scripts).
    for (const bool bytype : {false, true})
    {
        TObjectInstance* best = nullptr;
        int64_t bestdist = INT64_MAX;
        for (TMapIterator i(level, nullptr, CHECK_NOINVENT); i; i++)
        {
            const char* n = bytype ? i->GetTypeName() : i->GetName();
            if (!n || stricmp(n, base.c_str()) != 0)
                continue;
            const S3DPoint p = i->Pos();
            const int64_t d = int64_t(p.x - at.x) * (p.x - at.x) + int64_t(p.y - at.y) * (p.y - at.y);
            if (d < bestdist)
            {
                bestdist = d;
                best = i;
            }
        }
        if (best)
            return best;
    }
    return nullptr;
}

bool TShadowTravel::Destination(SShadowContext& context, S3DPoint& out, std::string& failure)
{
    const int32_t level = Player->GetLevel();
    const TShadowWalkField* field = context.nav.Field(context.log);
    if (!field)
    {
        failure = "no walk field for level " + std::to_string(level);
        return false;
    }

    // A place: the point itself, or the object it names.
    std::string objectname = target;
    if (const SShadowPlace* place = context.plan.FindPlace(target))
    {
        if (place->object.empty())
        {
            if (place->level != level)
            {
                failure = target + " is on level " + std::to_string(place->level) + "; Locke is on " +
                          std::to_string(level);
                return false;
            }
            const std::optional<S3DPoint> spot =
                field->NearestStandable(S3DPoint(place->x, place->y, place->z), kPlaceSlack, region);
            if (!spot)
            {
                failure = "nowhere Locke can stand within " + std::to_string(kPlaceSlack) + " of " + target +
                          " that he can walk to";
                return false;
            }
            out = *spot;
            return true;
        }
        objectname = place->object;
    }

    TObjectInstance* found = ShadowFindObject(objectname, level, Player->Pos());
    if (!found)
    {
        failure = "no " + objectname + " on level " + std::to_string(level);
        return false;
    }
    object = found;

    if (arrive == EArrive::Onto)
        if (auto* exit = dynamic_cast<TExit*>(found))
        {
            if (exit->Locked())
            {
                failure = objectname + " is locked (key " + std::to_string(exit->KeyId()) +
                          "): use it with its key first";
                return false;
            }
            viaexit = true;
            viadoor = exit->IsScriptedDoor();
            if (!viadoor)
                return OntoStrip(*field, *exit, out, failure);
        }

    // Next to it, on Locke's side: the standable cells around it in his
    // region, between touching it and a step further, nearest him.
    const int32_t room = (found->IsCharacter() ? static_cast<TCharacter*>(found)->Radius() : kObjectRadius) +
                         Player->Radius();
    const S3DPoint centre = found->Pos();
    const std::optional<S3DPoint> spot = field->NearestStandable(centre, room + kApproachSlack, region);
    if (!spot)
    {
        failure = "Locke can't walk to within " + std::to_string(room + kApproachSlack) + " of " + objectname;
        return false;
    }
    // NearestStandable gives the cell nearest the object, which may be under
    // it; step back out along the line toward Locke until there is room.
    S3DPoint best = *spot;
    const S3DPoint locke = Player->Pos();
    const double dx = locke.x - centre.x, dy = locke.y - centre.y;
    const double len = std::max(1.0, std::hypot(dx, dy));
    for (int32_t back = room; back <= room + kApproachSlack; back += 8)
    {
        const S3DPoint probe(centre.x + int32_t(dx / len * back), centre.y + int32_t(dy / len * back), centre.z);
        if (const std::optional<S3DPoint> s = field->NearestStandable(probe, 16, region);
            s && Distance2D(*s, centre) >= room)
        {
            best = *s;
            break;
        }
    }
    out = best;
    return true;
}

bool TShadowTravel::OntoStrip(const TShadowWalkField& field, TExit& exit, S3DPoint& dest, std::string& failure) const
{
    const SRect strip = exit.StripRect();
    const S3DPoint middle((strip.left + strip.right) / 2, (strip.top + strip.bottom) / 2, exit.Pos().z);
    std::optional<S3DPoint> best;
    int32_t bestdist = INT32_MAX;
    for (int32_t y = strip.top + GRIDSIZE / 2; y < strip.bottom; y += GRIDSIZE)
        for (int32_t x = strip.left + GRIDSIZE / 2; x < strip.right; x += GRIDSIZE)
        {
            const std::optional<S3DPoint> spot = field.NearestStandable(S3DPoint(x, y, middle.z), GRIDSIZE, region);
            if (!spot || !exit.IsOnStrip(*spot))
                continue;
            const int32_t d = Distance2D(*spot, middle);
            if (d < bestdist)
            {
                bestdist = d;
                best = spot;
            }
        }
    if (!best)
    {
        failure = "Locke can't walk onto " + target + "'s strip";
        return false;
    }
    dest = *best;
    return true;
}

EShadowStatus TShadowTravel::Pulse(SShadowContext& context, std::string& failure)
{
    switch (stage)
    {
    case EStage::Plan:
    {
        if (!ShadowGameIdle())
            return EShadowStatus::Running;
        const TShadowWalkField* field = context.nav.Field(context.log);
        if (!field)
        {
            failure = "no walk field for level " + std::to_string(Player->GetLevel());
            return EShadowStatus::Failed;
        }
        // Where Locke stands; if the cell under him fails the standing rule
        // (he is on its edge), the nearest one that passes.
        const std::optional<S3DPoint> start = field->NearestStandable(Player->Pos(), 48);
        if (!start)
        {
            failure = "Locke isn't standing anywhere the walk field knows";
            return EShadowStatus::Failed;
        }
        from = *start;
        region = field->RegionAt(from);
        if (!Destination(context, dest, failure))
            return EShadowStatus::Failed;
        // Keep out of the trigger zones on the way: the ones he starts in or
        // is going to don't count.
        const TShadowZones& zones = context.nav.Zones(context.log);
        std::vector<int32_t> avoid;
        for (int32_t z = 0; z < (int32_t)zones.Zones().size(); ++z)
            if (!zones.Zones()[z].Contains(from) && !zones.Zones()[z].Contains(dest))
                avoid.push_back(z);
        const auto inzone = [&](const S3DPoint& p) {
            for (const int32_t z : avoid)
                if (zones.Zones()[z].Contains(p))
                    return z;
            return -1;
        };
        std::optional<TShadowWalkField::SPath> path =
            field->FindPath(from, dest, [&](const S3DPoint& p) { return inzone(p) >= 0 ? kZoneStepCost : 0; });
        if (!path)
        {
            failure = "no walk to " + target + " (Locke's region " + std::to_string(region) + ", the target's " +
                      std::to_string(field->RegionAt(dest)) + ")";
            return EShadowStatus::Failed;
        }
        // Where the walk has to stop on the way: a zone it can't keep out
        // of (stop at its edge, let it fire), or the first monster that would
        // see Locke (stop there and fight) -- whichever comes first.
        crossing.clear();
        meeting.clear();
        size_t stop = path->points.size();
        for (size_t i = 0; i < path->points.size(); ++i)
            if (const int32_t z = inzone(path->points[i]); z >= 0)
            {
                if (++crossings > kMaxCrossings)
                {
                    failure = "the walk to " + target + " keeps stopping in trigger zones";
                    return EShadowStatus::Failed;
                }
                crossing = zones.Zones()[z].Describe();
                stop = i;
                break;
            }
        if (!evaded)
        {
            std::vector<SShadowEncounter> met = ShadowLegEncounters(path->points, Player->GetLevel());
            met.erase(std::remove_if(met.begin(), met.end(),
                                     [&](const SShadowEncounter& e) { return e.monster.Get() == object.Get(); }),
                      met.end());
            if (!met.empty() && met.front().index < stop)
            {
                const size_t fights = evade ? ShadowEvadeShare(met, context.nav.Gait(context.log).run)
                                            : size_t(std::count_if(met.begin(), met.end(), [&](const SShadowEncounter& e) {
                                                  return e.index == met.front().index;
                                              }));
                for (size_t k = 0; k < fights && k < met.size(); ++k)
                    meeting.push_back(met[k].monster);
                evaded = evade;
                crossing.clear();
                stop = met.front().index;
                context.log.Event("encounter", std::to_string(met.size()) + " monsters on the way to " + target +
                                                   "; Locke fights " + std::to_string(meeting.size()) +
                                                   (evade ? " (evading)" : ""));
            }
        }
        if (stop < path->points.size())
        {
            path->points.resize(stop + 1);
            double length = 0.0;
            for (size_t k = 1; k < path->points.size(); ++k)
                length += std::hypot(double(path->points[k].x - path->points[k - 1].x),
                                     double(path->points[k].y - path->points[k - 1].y));
            path->length = (int32_t)std::lround(length);
            dest = path->points.back();
            viaexit = viadoor = false;
        }
        const SShadowGait& gait = context.nav.Gait(context.log);
        pace = PaceLeg(path->length, Player->Fatigue(), Player->MaxFatigue(), gait);

        const int32_t level = Player->GetLevel();
        context.log.Event(
            "travel",
            target + ": " + std::to_string(path->length) + " units, " + std::to_string(pace.ticks) + " ticks (" +
                std::to_string(pace.ran) + " run, " + std::to_string(pace.walked) + " walked), region " +
                std::to_string(region),
            [&](JsonOut& out) {
                out.FieldString("target", ToUtf8(target.c_str()));
                if (!crossing.empty())
                    out.FieldString("stops_in", crossing);
                out.Field("level", level);
                WritePoint(out, "from", from);
                WritePoint(out, "to", dest);
                out.Field("region", region);
                out.Field("length", path->length);
                out.Field("ticks", pace.ticks);
                out.Field("ran", pace.ran);
                out.Field("walked", pace.walked);
                out.Field("fatigue", pace.fatigue);
                out.FieldBool("evade", evade);
                out.Key("path").Begin('[');
                int32_t since = kProofSpacing;
                for (size_t i = 0; i < path->points.size(); ++i)
                {
                    const S3DPoint& p = path->points[i];
                    if (i > 0)
                        since += Distance2D(p, path->points[i - 1]);
                    if (since >= kProofSpacing || i + 1 == path->points.size())
                    {
                        out.Begin('[').Value(p.x).Value(p.y).Value(p.z).End(']');
                        since = 0;
                    }
                }
                out.End(']');
            });
        if (path->length < kThere && crossing.empty() && meeting.empty())
        {
            stage = EStage::Done;
            return EShadowStatus::Done;
        }
        stage = EStage::Walk;
        return EShadowStatus::Running;
    }

    case EStage::Walk:
        // The walk's time passes with the world running; Locke arrives at
        // its end.
        if (++waited < pace.ticks)
            return EShadowStatus::Running;
        stage = EStage::Arrive;
        [[fallthrough]];

    case EStage::Arrive:
    {
        if (!ShadowGameIdle())
            return EShadowStatus::Running;
        // An NPC may have walked on meanwhile: stand by it where it is now,
        // by a walk that still holds.
        if (object && crossing.empty() && meeting.empty())
        {
            S3DPoint now;
            if (!Destination(context, now, failure))
                return EShadowStatus::Failed;
            const TShadowWalkField* field = context.nav.Field(context.log);
            if (!field || !field->FindPath(from, now))
            {
                failure = "no walk to " + target + " where it stands now";
                return EShadowStatus::Failed;
            }
            dest = now;
        }
        const int32_t level = Player->GetLevel();
        context.ledger.AllowTravel(dest, level, context.tick + 2);
        Player->Teleport(dest, level);
        // The last running stretch's cost: standing still meanwhile gave
        // back what walking would have.
        if (pace.fatigue < Player->Fatigue())
            Player->SetFatigue(pace.fatigue);
        if (!meeting.empty())
        {
            std::vector<TCharacter*> foes;
            for (const TSafeRef<TCharacter>& ref : meeting)
                if (TCharacter* foe = ref.Get())
                    foes.push_back(foe);
            fightstep = SShadowStep{};
            fightstep.verb = EShadowVerb::Fight;
            fight = MakeFightRunner(fightstep, std::move(foes));
            meeting.clear();
            stage = EStage::Fighting;
            return EShadowStatus::Running;
        }
        if (!crossing.empty())
        {
            context.log.Event("crossing", crossing + ": Locke stops in it on the way to " + target);
            waited = 0;
            stage = EStage::Crossing;
            return EShadowStatus::Running;
        }
        if (viaexit)
        {
            if (viadoor)
                if (TObjectInstance* door = object.Get())
                {
                    context.log.Event("use", target + ": the door's script takes him through");
                    door->Use(Player, -1);
                }
            startlevel = level;
            waited = 0;
            stage = EStage::Through;
            return EShadowStatus::Running;
        }
        stage = EStage::Done;
        return EShadowStatus::Done;
    }

    case EStage::Through:
        // On the strip, the exit's own pulse takes him through (TExit::Pulse,
        // Activate); at a door, its script does. The session brings the
        // level in.
        if (Player->GetLevel() == startlevel && Distance2D(Player->Pos(), dest) < kExitJump)
        {
            if (++waited > kExitTicks)
            {
                failure = viadoor ? target + "'s script didn't take Locke through"
                                  : "Locke stood on " + target + "'s strip and it didn't take him";
                return EShadowStatus::Failed;
            }
            return EShadowStatus::Running;
        }
        if (!ShadowGameIdle())
            return EShadowStatus::Running;
        context.log.Event("through", target + ": level " + std::to_string(Player->GetLevel()));
        stage = EStage::Done;
        return EShadowStatus::Done;

    case EStage::Fighting:
    {
        const EShadowStatus status = fight->Pulse(context);
        if (status == EShadowStatus::Running)
            return status;
        if (status == EShadowStatus::Failed)
        {
            failure = "on the way to " + target + ": " + fight->Failure();
            return EShadowStatus::Failed;
        }
        fight.reset();
        waited = 0;
        stage = EStage::Plan;
        return EShadowStatus::Running;
    }

    case EStage::Crossing:
        // The zone fires (or its script has nothing to do now); once the
        // game is Locke's again for a moment, the travel goes on from
        // wherever it left him.
        if (!ShadowGameIdle())
        {
            waited = 0;
            return EShadowStatus::Running;
        }
        if (++waited < kZoneSettle)
            return EShadowStatus::Running;
        crossing.clear();
        waited = 0;
        stage = EStage::Plan;
        return EShadowStatus::Running;

    case EStage::Done:
        return EShadowStatus::Done;
    }
    return EShadowStatus::Done;
}
