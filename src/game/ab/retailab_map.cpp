// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *   retailab_map.cpp - the port's side of the map query A/B dumps       *
// *************************************************************************
//
// The combat dojo's katas M11 / M12 (docs/gameplay/COMBAT_DOJO.md;
// forensics/MAP_QUERIES.md): TMapPane::LineOfSight and FindObjectsInRange on
// a stand-in map, mirroring tools/retail_runtime/slots/combat/map_query.py.
// The walk cells come from the case's ground (SCaseScope's seam, each read
// recorded); the objects a search walks from its `objects`, through the map
// pane's iterator seam (SetMapObjects).

#include "game/ab/retailab_fixture.h"

#include "game/world/mappane.h"

namespace RetailAB
{
namespace
{

using namespace Fixture;

// An out-point's fill (map_query.py UNWRITTEN): still there, it wasn't written.
constexpr int32_t kUnwritten = -7777;

S3DPoint ReadPoint(const JsonValue& v)
{
    return S3DPoint((int32_t)v[0].Int(), (int32_t)v[1].Int(), (int32_t)v[2].Int());
}

void WritePoint(JsonOut& j, const char* key, const S3DPoint& p)
{
    j.Key(key).Begin('[').Value(p.x).Value(p.y).Value(p.z).End(']');
}

// A map object that isn't a character: a class, a place, a map index and an
// inventory slot (the retail fixture's stand-in item, whose vtable answers
// IsInInventory alone).
class TStandInObject : public TObjectInstance
{
  public:
    explicit TStandInObject(const JsonValue& spec) : TObjectInstance(nullptr)
    {
        objclass = (short)spec["class"].Int();
        Node()->SetMapIndex((int32_t)spec["id"].Int());
        SetIntPos(*this, ReadPoint(spec["pos"]));
        SetInventNum((short)spec["inventnum"].Int(-1));
    }
};

// Case (field 0, JSON): see slots/combat/map_query.py. `call`:
// "line-of-sight", "find-objects".
std::string MapCall(const Case& c, std::string& error)
{
    try
    {
        if (!LoadGameData(error))
            return {};
        const JsonValue cs = JsonValue::Parse(c.Field(0));
        TFixtureWorld world(cs);
        // Every object by name with the level the map search finds it on,
        // and by map index for the dump.
        std::vector<TObjectPtr<TStandInObject>> items;
        std::map<std::string, SMapObject> objects;
        for (const JsonValue& spec : cs["chars"].Items())
            objects[spec["name"].Str()] = SMapObject{world.Get(spec["name"].Str()), (int32_t)spec["level"].Int(0)};
        for (const JsonValue& spec : cs["items"].Items())
        {
            items.push_back(TObjectPtr<TStandInObject>(new TStandInObject(spec)));
            objects[spec["name"].Str()] = SMapObject{items.back().get(), (int32_t)spec["level"].Int(0)};
        }
        std::map<int32_t, std::string> names;
        for (const auto& [name, m] : objects)
            names[m.object->GetMapIndex()] = name;
        std::vector<SMapObject> walked;
        for (const JsonValue& n : cs["objects"].Items())
            walked.push_back(objects.at(n.Str()));

        SCaseScope scope(cs, world);
        SetMapObjects(std::move(walked));

        JsonOut j;
        j.Begin('{').FieldString("schema", "combat.map.v1").FieldString("side", "port");
        const std::string call = cs["call"].Str();
        if (call == "line-of-sight")
        {
            const bool outputs = cs["outputs"].Bool(false);
            S3DPoint blockedat(kUnwritten, kUnwritten, kUnwritten), laststep = blockedat;
            const bool clear = MapPane.LineOfSight(ReadPoint(cs["from"]), ReadPoint(cs["to"]),
                                                   (int32_t)cs["level"].Int(0), outputs ? &blockedat : nullptr,
                                                   outputs ? &laststep : nullptr);
            j.Field("returned", clear ? 1 : 0);
            if (outputs)
            {
                WritePoint(j, "blockedat", blockedat);
                WritePoint(j, "laststep", laststep);
            }
        }
        else if (call == "find-objects")
        {
            const int32_t maxnum = (int32_t)cs["maxnum"].Int();
            std::vector<int32_t> array((size_t)(std::max)(maxnum, 0) + 1, -1);
            const int32_t found = MapPane.FindObjectsInRange(ReadPoint(cs["pos"]), (int32_t)cs["level"].Int(-1),
                array.data(), (int32_t)cs["width"].Int(), (int32_t)cs["height"].Int(0),
                (int32_t)cs["objclass"].Int(-1), maxnum, (int32_t)cs["objset"].Int(0));
            j.Field("returned", found).Key("array").Begin('[');
            for (int32_t index : array)
            {
                if (index == -1)
                    j.Null();
                else if (auto it = names.find(index); it != names.end())
                    j.String(it->second);
                else
                    j.String("#" + std::to_string(index));
            }
            j.End(']');
        }
        else
            throw std::runtime_error("unknown call '" + call + "'");
        WriteSeams(j);
        WriteDraws(j);
        j.End('}');
        return j.str();
    }
    catch (const std::exception& e)
    {
        error = e.what();
        return {};
    }
}

}  // namespace

// Registered with the A/B driver by name (retailab.h).
static const bool registered = RegisterTarget("map-los", MapCall) && RegisterTarget("map-find-objects", MapCall);

}  // namespace RetailAB
