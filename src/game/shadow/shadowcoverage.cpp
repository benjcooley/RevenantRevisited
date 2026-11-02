// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *      shadowcoverage.cpp - the script lines a run executes, and when     *
// *************************************************************************

#include "game/shadow/shadowcoverage.h"

#include "engine/core/jsonout.h"
#include "game/script/script.h"
#include "engine/core/textencoding.h"

#include <algorithm>
#include <filesystem>

void TShadowCoverage::Line(TScript& script, int32_t offset, int64_t tick)
{
    const TScriptProto* proto = script.GetScriptProto();
    if (!proto || !proto->text || offset < 0 || offset > proto->len)
        return;
    // The block's text is the file's from the OBJECT line on, so its line
    // breaks before the offset count the lines from the OBJECT line.
    const int32_t line = (int32_t)std::count(proto->text, proto->text + offset, '\n');
    const std::string file = proto->filename ? std::filesystem::path(proto->filename).filename().string() : "?";
    lines[file][proto->name ? proto->name : "?"][line] = tick;
}

void TShadowCoverage::StateChanged(const std::string& name, int32_t was, int32_t is, int64_t tick)
{
    changes.push_back({name, was, is, tick});
}

std::string TShadowCoverage::Json() const
{
    JsonOut out;
    out.Begin('{');
    out.Key("blocks").Begin('[');
    for (const auto& [file, objects] : lines)
        for (const auto& [object, ran] : objects)
        {
            out.Begin('{');
            out.FieldString("file", ToUtf8(file.c_str()));
            out.FieldString("object", ToUtf8(object.c_str()));
            out.Key("lines").Begin('{');
            for (const auto& [line, tick] : ran)
                out.Field(std::to_string(line).c_str(), tick);
            out.End('}');
            out.End('}');
        }
    out.End(']');
    out.Key("changes").Begin('[');
    for (const SChange& c : changes)
    {
        out.Begin('{');
        out.FieldString("var", c.name);
        out.Field("was", c.was);
        out.Field("is", c.is);
        out.Field("tick", c.tick);
        out.End('}');
    }
    out.End(']');
    out.End('}');
    return out.str();
}
