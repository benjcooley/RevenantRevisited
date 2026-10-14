// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *    shadowzones.cpp - where walking sets a script off (CUBE, PROXIMITY)  *
// *************************************************************************

#include "shadowzones.h"

#include "shadowlog.h"

#include "../area.h"
#include "../file.h"
#include "../mappane.h"
#include "../module.h"
#include "../parse.h"
#include "../player.h"
#include "../revenant.h"
#include "../script.h"
#include "../textencoding.h"

#include <cstdlib>
#include <memory>
#include <set>

namespace {

// Objects a prototype runs on: those named for it, or of the type it names
// (TScriptManager::ObjectScript matches the instance name, else the type).
std::vector<TObjectInstance*> Owners(const char* name, int32_t level)
{
    std::vector<TObjectInstance*> owners;
    if (!name || !name[0])
        return owners;
    for (TMapIterator i(level, nullptr, CHECK_NOINVENT); i; i++)
    {
        const char* instance = i->GetName();
        const char* type = i->GetTypeName();
        if ((instance && !stricmp(instance, name)) || (type && !stricmp(type, name)))
            owners.push_back(i);
    }
    return owners;
}

} // namespace

bool SShadowZone::Contains(const S3DPoint& p) const
{
    if (kind == EKind::Cube)
        return S3DRect(low, high).In(p) != 0;
    // Box first, then dx² + dy² <= 2·dist² (TScript::Triggered).
    const int32_t dx = centre.x - p.x;
    const int32_t dy = centre.y - p.y;
    return std::abs(dx) <= dist && std::abs(dy) <= dist && dx * dx + dy * dy <= 2 * dist * dist;
}

std::string SShadowZone::Describe() const
{
    return std::string(kind == EKind::Cube ? "cube " : "proximity ") + ToUtf8(owner.c_str()) + " (" + file + ")";
}

void TShadowZones::Build(int32_t lvl, TShadowLog& log)
{
    level = lvl;
    zones.clear();

    std::set<std::string> files = {"master.s"};
    for (int32_t i = 0; i < AreaManager.NumAreas(); ++i)
        if (const TArea* area = AreaManager.GetArea(i); area && area->Level() == level && area->ScriptFile()[0])
            files.insert(area->ScriptFile());
    for (const std::string& file : files)
        Read(file);

    int32_t cubes = 0;
    for (const SShadowZone& zone : zones)
        cubes += zone.kind == SShadowZone::EKind::Cube;
    log.Event("zones", "level " + std::to_string(level) + ": " + std::to_string(cubes) + " cubes, " +
                           std::to_string(zones.size() - cubes) + " proximities, from " +
                           std::to_string(files.size()) + " scripts",
              [&](JsonOut& out) {
                  out.Field("level", level);
                  out.Field("cubes", cubes);
                  out.Field("proximities", (int32_t)zones.size() - cubes);
              });
}

void TShadowZones::Read(const std::string& file)
{
    const std::string path = ModuleManager.DataFilePath(file.c_str());
    FILE* fp = TryOpen(path.c_str(), "rb");
    if (!fp)
        return;
    fseek(fp, 0, SEEK_END);
    const long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    std::vector<char> buffer(size_t(std::max(size, 0L)) + 1, '\0');
    const bool read = fread(buffer.data(), 1, size_t(size), fp) == size_t(size);
    fclose(fp);
    if (!read)
        return;

    std::vector<char> filename(file.begin(), file.end());
    filename.push_back('\0');
    const char* playername = Player ? Player->GetName() : nullptr;

    // TScriptManager::ParseScripts' loop, keeping only the triggers. (The
    // cap only guards against a file that never reaches its end.)
    constexpr int32_t kMaxProtos = 10000;
    TStringParseStream s(buffer.data(), (int32_t)size);
    TToken t(s);
    t.Get();
    for (int32_t count = 0; t.Type() != TKN_EOF && count < kMaxProtos; ++count)
    {
        TScriptProto proto(nullptr, nullptr, filename.data(), nullptr);
        proto.ParseScript(t);
        for (int32_t k = 0; k < proto.NumTriggers(); ++k)
        {
            const SScriptTrigger& trigger = proto.triggers[k];
            const bool cube = trigger.type == TRIGGER_CUBE && !trigger.region[0];
            const bool near = trigger.type == TRIGGER_PROXIMITY && playername && !stricmp(trigger.name, playername);
            if (!cube && !near)
                continue;
            const std::vector<TObjectInstance*> owners = Owners(proto.name, level);
            if (owners.empty())
                continue;           // it runs on nothing here, so it never fires
            SShadowZone zone;
            zone.owner = proto.name ? proto.name : "";
            zone.file = file;
            if (cube)
            {
                zone.kind = SShadowZone::EKind::Cube;
                zone.low = trigger.cube.beg;
                zone.high = trigger.cube.end;
                zones.push_back(zone);
            }
            else
            {
                zone.kind = SShadowZone::EKind::Proximity;
                zone.dist = trigger.dist;
                for (const TObjectInstance* owner : owners)
                {
                    zone.centre = owner->Pos();
                    zones.push_back(zone);
                }
            }
        }
    }
}

int32_t TShadowZones::ZoneAt(const S3DPoint& p) const
{
    for (size_t i = 0; i < zones.size(); ++i)
        if (zones[i].Contains(p))
            return (int32_t)i;
    return -1;
}
