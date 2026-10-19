// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *          shadowlog.cpp - a Shadow Smoke run's event log                *
// *************************************************************************

#include "game/shadow/shadowlog.h"

#include "engine/core/logging.h"

#include <filesystem>
#include <fstream>

bool TShadowLog::Open(const std::string& path)
{
    Close();
    std::error_code ec;
    std::filesystem::create_directories(path, ec);
    if (ec)
    {
        log_error("[smoke] can't create %s: %s", path.c_str(), ec.message().c_str());
        return false;
    }
    dir = path;
    const std::string jsonl = (std::filesystem::path(dir) / "shadow.jsonl").string();
    file = fopen(jsonl.c_str(), "w");
    if (!file)
    {
        log_error("[smoke] can't write %s", jsonl.c_str());
        return false;
    }
    return true;
}

void TShadowLog::Close()
{
    if (file)
        fclose(file);
    file = nullptr;
}

void TShadowLog::Event(const char* type, const std::string& text, const std::function<void(JsonOut&)>& fill)
{
    log_info("[smoke] %06lld %s: %s", (long long)tick, type, text.c_str());
    if (!file)
        return;
    JsonOut out;
    out.Begin('{');
    out.Field("tick", tick);
    out.FieldString("event", type);
    if (fill)
        fill(out);
    out.End('}');
    fprintf(file, "%s\n", out.str().c_str());
    fflush(file);
}

bool TShadowLog::WriteFile(const char* name, const std::string& contents) const
{
    std::ofstream out(std::filesystem::path(dir) / name, std::ios::binary);
    out << contents;
    return bool(out);
}
