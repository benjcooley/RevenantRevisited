// Module manager — see module.h.

#include "module.h"

#include "logging.h"
#include "parse.h"
#include "revutils.h"

#include <algorithm>
#include <filesystem>

TModuleManager ModuleManager;

namespace {

constexpr size_t kModNameMax  = 128;
constexpr size_t kModDescMax  = 4096;

// Read the next non-blank, non-#define top-level token in module.def. The
// AREA.DEF parser uses DefineGet() between blocks for the same purpose.
bool advance_to_next_tag(TToken &t)
{
    while (t.Type() == TKN_RETURN)
        t.LineGet();
    return t.Type() != TKN_EOF;
}

} // namespace


bool TModuleManager::LoadModuleDef(const std::string &dirname, TModule &out)
{
    // module.def lives inside the module's own filesystem layer, so we
    // need to mount the candidate first. Initialize() unmounts at the end;
    // SetCurModule() remounts whatever the caller chose.
    if (!MountModule(dirname.c_str()))
    {
        log_warn("[module] %s: cannot mount for module.def probe", dirname.c_str());
        return false;
    }

    FILE *fp = rev_fopen("module.def", "rb");
    if (!fp)
    {
        log_warn("[module] %s: module.def missing", dirname.c_str());
        return false;
    }

    TFileParseStream s(fp, "module.def");
    TToken t(s);

    if (!t.DefineGet())
    {
        log_warn("[module] %s: syntax error in module.def header", dirname.c_str());
        fclose(fp);
        return false;
    }

    char modname[kModNameMax] = {0};
    if (!Parse(t, "MODULE %s\n", modname))
    {
        log_warn("[module] %s: 'MODULE \"name\"' expected", dirname.c_str());
        fclose(fp);
        return false;
    }

    out.dirname = dirname;
    out.name    = modname;          // NAME tag may overwrite below

    if (!t.Is("BEGIN"))
    {
        log_warn("[module] %s: BEGIN expected", dirname.c_str());
        fclose(fp);
        return false;
    }
    t.LineGet();

    while (advance_to_next_tag(t) && !t.Is("END"))
    {
        if (t.Type() != TKN_IDENT)
        {
            t.LineGet();
            continue;
        }

        if (t.Is("NAME"))
        {
            char buf[kModNameMax] = {0};
            if (Parse(t, "NAME %s\n", buf)) out.name = buf;
        }
        else if (t.Is("VERSION"))
        {
            char buf[64] = {0};
            if (Parse(t, "VERSION %s\n", buf)) out.version = buf;
        }
        else if (t.Is("GAMEVERSION"))
        {
            char buf[64] = {0};
            if (Parse(t, "GAMEVERSION %s\n", buf)) out.gameversion = buf;
        }
        else if (t.Is("UNIQUEID"))
        {
            int32_t v = 0;
            if (Parse(t, "UNIQUEID %i\n", &v)) out.uniqueid = (uint32_t)v;
        }
        else if (t.Is("FLAGS"))
        {
            int32_t v = 0;
            if (Parse(t, "FLAGS %i\n", &v)) out.flags = (uint32_t)v;
        }
        else if (t.Is("MINHOURS"))
        {
            Parse(t, "MINHOURS %i\n", &out.minhours);
        }
        else if (t.Is("MAXHOURS"))
        {
            Parse(t, "MAXHOURS %i\n", &out.maxhours);
        }
        else if (t.Is("DESC"))
        {
            char buf[kModDescMax] = {0};
            if (Parse(t, "DESC %s\n", buf)) out.desc = buf;
        }
        else
        {
            // STARTPOS / MUDIP / MUDPORT and any other multiplayer-only or
            // future tags fall through here. Skip to end-of-line — the
            // pre-release source does not consume these and the singleplayer
            // demo does not need them.
            log_info("[module] %s: skipping unhandled tag '%s'", dirname.c_str(), t.Text());
            while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF) t.Get();
            t.LineGet();
        }
    }

    fclose(fp);
    return true;
}

bool TModuleManager::Initialize()
{
    if (initialized) return true;
    modules.clear();
    active_idx = -1;
    main_idx   = -1;

    namespace fs = std::filesystem;
    const fs::path mods_dir = fs::path(RunPath) / "Modules";
    std::error_code ec;
    if (!fs::is_directory(mods_dir, ec))
    {
        log_error("[module] Modules dir not found at %s", mods_dir.string().c_str());
        return false;
    }

    // Enumerate available modules. Each subdirectory is an unpacked module;
    // each <name>.rvm is a packed module. If both exist for the same name
    // (the Ahkuilon / Ahkuilon_unzipped pattern in this tree), the unpacked
    // version wins because that is the dev-iteration target. The _unzipped
    // suffix convention is a developer marker, not a runtime concept, so we
    // don't strip it; the user sees the directory name as the module id.
    std::vector<std::string> candidates;
    for (const auto &e : fs::directory_iterator(mods_dir, ec))
    {
        if (e.is_directory())
        {
            candidates.push_back(e.path().filename().string());
        }
        else if (e.is_regular_file() && e.path().extension() == ".rvm")
        {
            const std::string stem = e.path().stem().string();
            if (std::find(candidates.begin(), candidates.end(), stem) == candidates.end())
                candidates.push_back(stem);
        }
    }
    std::sort(candidates.begin(), candidates.end());

    for (const std::string &d : candidates)
    {
        auto m = std::make_unique<TModule>();
        if (LoadModuleDef(d, *m))
        {
            modules.push_back(std::move(m));
        }
    }

    UnmountModule();   // SetCurModule remounts whichever module the caller picks.

    if (modules.empty())
    {
        log_error("[module] no playable modules found under %s", mods_dir.string().c_str());
        return false;
    }

    // Resolve [Modules]/MainModule from INI; fall back to first singleplayer
    // module, then to the first module overall. The current Revenant.ini
    // ships MainModule = "Ahkuilon"; the dev tree mostly contains
    // Ahkuilon_unzipped instead, so the fallback path is the hot path
    // today and the log line tells you which entry was picked.
    char mainname[kModNameMax] = {0};
    INISetSection("Modules");
    INIGetStr("MainModule", (char *)"Ahkuilon", mainname, sizeof(mainname));
    main_idx = Find(mainname);

    if (main_idx < 0)
    {
        for (int i = 0; i < (int)modules.size(); ++i)
            if (modules[i]->flags & GAMEMOD_SINGLEPLAYER) { main_idx = i; break; }
        if (main_idx < 0) main_idx = 0;
        log_warn("[module] MainModule '%s' not in catalog; falling back to '%s'",
                 mainname, modules[main_idx]->dirname.c_str());
    }

    initialized = true;
    log_info("[module] catalog: %d module(s); main='%s'",
             (int)modules.size(), modules[main_idx]->dirname.c_str());
    return true;
}

void TModuleManager::Close()
{
    UnmountModule();
    modules.clear();
    active_idx  = -1;
    main_idx    = -1;
    initialized = false;
}

int TModuleManager::Find(const char *dirname) const
{
    if (!dirname || !*dirname) return -1;
    for (int i = 0; i < (int)modules.size(); ++i)
        if (modules[i]->dirname == dirname) return i;
    return -1;
}

bool TModuleManager::SetCurModule(int idx)
{
    if (idx < 0 || idx >= (int)modules.size()) return false;
    TModule *m = modules[idx].get();
    if (!MountModule(m->dirname.c_str()))
    {
        log_error("[module] SetCurModule: failed to mount '%s'", m->dirname.c_str());
        return false;
    }
    active_idx = idx;
    log_info("[module] active = '%s' (%s)", m->dirname.c_str(), m->name.c_str());
    return true;
}

bool TModuleManager::SetCurModule(const char *dirname)
{
    const int idx = Find(dirname);
    if (idx < 0)
    {
        log_error("[module] SetCurModule('%s'): not in catalog", dirname ? dirname : "(null)");
        return false;
    }
    return SetCurModule(idx);
}
