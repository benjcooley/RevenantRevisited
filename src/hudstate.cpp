// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  hudstate.cpp - Serializable HUD state                                *
// *************************************************************************

#include "hudstate.h"

#include "logging.h"

#include <cstdio>
#include <cstring>

namespace {
SHudState g_hudState;
}

SHudState& GetHudState() { return g_hudState; }

bool SaveHudState(const SHudState& s, const char* path)
{
    if (!path) return false;
    FILE* f = std::fopen(path, "wb");
    if (!f) { log_warn("[hudstate] save: cannot open %s", path); return false; }

    const uint32_t magic   = SHudState::kMagic;
    const uint32_t version = SHudState::kVersion;
    bool ok = true;
    ok &= (std::fwrite(&magic,   sizeof(magic),   1, f) == 1);
    ok &= (std::fwrite(&version, sizeof(version), 1, f) == 1);
    ok &= (std::fwrite(&s,       sizeof(s),       1, f) == 1);

    std::fclose(f);
    if (!ok) log_warn("[hudstate] save: short write to %s", path);
    return ok;
}

bool LoadHudState(SHudState& s, const char* path)
{
    if (!path) return false;
    FILE* f = std::fopen(path, "rb");
    if (!f) return false;

    uint32_t magic = 0, version = 0;
    bool ok = true;
    ok &= (std::fread(&magic,   sizeof(magic),   1, f) == 1);
    ok &= (std::fread(&version, sizeof(version), 1, f) == 1);

    if (!ok || magic != SHudState::kMagic)
    {
        std::fclose(f);
        log_warn("[hudstate] load: bad magic in %s", path);
        return false;
    }
    if (version != SHudState::kVersion)
    {
        std::fclose(f);
        log_warn("[hudstate] load: version mismatch in %s (got %u, want %u)",
                 path, version, SHudState::kVersion);
        return false;
    }

    SHudState scratch;
    ok &= (std::fread(&scratch, sizeof(scratch), 1, f) == 1);
    std::fclose(f);
    if (!ok) { log_warn("[hudstate] load: short read from %s", path); return false; }

    s = scratch;
    return true;
}
