// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 2024 Cinematix                       *
// *                  revutils.cpp - Utility functions                     *
// *************************************************************************

#include "revutils.h"

#include "display.h"
#include "imagery.h"
#include "logging.h"
#include "mainwnd.h"
#include "mappane.h"
#include "parse.h"
#include "timer.h"

#include <chrono>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__APPLE__)
#  include <mach-o/dyld.h>     // _NSGetExecutablePath
#endif
#if defined(__linux__)
#  include <unistd.h>          // readlink
#endif

#include <cassert>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <string>
#include <system_error>

#include <SimpleIni.h>
#include <miniz.h>

#include <memory>
#include <unordered_map>
#include <vector>

// ************************************************************************
// *                          Support Functions                           *
// ************************************************************************

bool ThreadsPaused = false;

void PauseThreads()
{
    ThreadsPaused = true;
    TObjectImagery::PauseLoader();
    PAUSEUPDATE;
}

void ResumeThreads()
{
    ThreadsPaused = false;
    TObjectImagery::ResumeLoader();
    MapPane.ResumeUpdate();
}

// *************** Simple Support Functions *****************

char *itos(int32_t val, char *buf, int32_t buflen)
{
    if (!buf)
        return buf;
    if (buflen < 2)
    {
        if (buf != nullptr)
            buf[0] = '\0';
        return buf;
    }
    if (buflen >= 12) // Twelve is big enough for anything
    {
        std::snprintf(buf, buflen, "%d", val);
        return buf;
    }
    char b[12];
    std::snprintf(b, sizeof(b), "%d", val);
    strncpy(buf, b, buflen - 1);
    buf[buflen - 1] = '\0';
    return buf;
}

int32_t stricmp(const char* s1, const char* s2)
{
  assert(s1 != NULL);
  assert(s2 != NULL);

#ifdef HAVE_STRCASECMP
    return strcasecmp(s1, s2)
#else
    while (tolower((unsigned char) *s1) == tolower((unsigned char) *s2)) 
    {
        if (*s1 == '\0')
        {
            return 0;
        }
        s1++;
        s2++;
    }

    return (int) tolower((unsigned char) *s1) -
        (int) tolower((unsigned char) *s2);
#endif
}

int32_t strnicmp(const char* s1, const char* s2, size_t n)
{
    assert(s1 != nullptr);
    assert(s2 != nullptr);
    while (n-- > 0)
    {
        int c1 = tolower((unsigned char) *s1++);
        int c2 = tolower((unsigned char) *s2++);
        if (c1 != c2)
            return c1 - c2;
        if (c1 == 0)
            return 0;
    }
    return 0;
}

// Splits a Win32-style "<dir>\<pattern>" path into the directory and the
// glob pattern. Accepts both '\\' and '/' separators. If no separator is
// present the whole string is treated as the pattern in the current dir.
namespace {

void split_glob(const char *spec, std::filesystem::path &dir, std::string &pattern)
{
    namespace fs = std::filesystem;
    if (!spec) { dir = "."; pattern = ""; return; }
    std::string s = spec;
    for (auto &c : s) if (c == '\\') c = '/';
    auto slash = s.find_last_of('/');
    if (slash == std::string::npos) { dir = "."; pattern = s; }
    else                            { dir = s.substr(0, slash); pattern = s.substr(slash + 1); }
}

// Win32 wildcard match: '*' = 0+ chars, '?' = exactly one. Case-insensitive
// to match Win32 FindFirstFile semantics; the surrounding code throws
// arbitrary case at us (e.g. "*.DAT" vs files named "*.dat").
bool glob_match(const char *pat, const char *str)
{
    while (*pat)
    {
        if (*pat == '*')
        {
            while (*pat == '*') pat++;
            if (!*pat) return true;
            for (; *str; ++str)
                if (glob_match(pat, str)) return true;
            return false;
        }
        if (!*str) return false;
        if (*pat != '?' && tolower((unsigned char)*pat) != tolower((unsigned char)*str))
            return false;
        ++pat; ++str;
    }
    return *str == 0;
}

// Resolve a Win32-style "directory" (possibly `.` or `<savepath>...`) to
// an absolute filesystem::path via makepath, in normalized form. If the
// path exists already we return it; otherwise we still return the
// resolved candidate so callers can mkdir.
std::filesystem::path resolve_dir(const std::filesystem::path &p)
{
    namespace fs = std::filesystem;
    if (p.is_absolute()) return fs::weakly_canonical(p);
    char buf[MAXPATHLEN];
    char tmp[MAXPATHLEN];
    strncpyz(tmp, p.string().c_str(), MAXPATHLEN);
    makepath(tmp, buf, MAXPATHLEN);  // SavePath-relative + normalize
    return fs::weakly_canonical(fs::path(buf));
}

} // namespace

// Copies one file or every wildcard-matched file from `from` (a glob like
// "<dir>\\*.DAT") into the directory portion of `to`. Returns the count
// copied. Silently skips matches that can't be opened. Creates the target
// directory if needed.
int32_t copyfiles(const char *from, const char *to, bool overwrite)
{
    namespace fs = std::filesystem;
    if (!from || !to) return 0;

    fs::path src_dir, dst_dir;
    std::string src_pat, dst_pat;
    split_glob(from, src_dir, src_pat);
    split_glob(to,   dst_dir, dst_pat);

    src_dir = resolve_dir(src_dir);
    dst_dir = resolve_dir(dst_dir);

    std::error_code ec;
    fs::create_directories(dst_dir, ec);

    if (!fs::exists(src_dir, ec) || !fs::is_directory(src_dir, ec))
        return 0;

    int32_t copied = 0;
    const auto opt = overwrite ? fs::copy_options::overwrite_existing
                               : fs::copy_options::skip_existing;
    for (const auto &ent : fs::directory_iterator(src_dir, ec))
    {
        if (!ent.is_regular_file()) continue;
        const std::string name = ent.path().filename().string();
        if (!glob_match(src_pat.c_str(), name.c_str())) continue;
        fs::copy_file(ent.path(), dst_dir / name, opt, ec);
        if (!ec) copied++;
    }
    return copied;
}

// Deletes every file in `name`'s directory matching its glob (e.g.
// "<dir>\\*.DAT"). Returns the count deleted.
int32_t deletefiles(const char *name)
{
    namespace fs = std::filesystem;
    if (!name) return 0;

    fs::path dir;
    std::string pat;
    split_glob(name, dir, pat);
    dir = resolve_dir(dir);

    std::error_code ec;
    if (!fs::exists(dir, ec) || !fs::is_directory(dir, ec))
        return 0;

    int32_t deleted = 0;
    for (const auto &ent : fs::directory_iterator(dir, ec))
    {
        if (!ent.is_regular_file()) continue;
        const std::string filename = ent.path().filename().string();
        if (!glob_match(pat.c_str(), filename.c_str())) continue;
        if (fs::remove(ent.path(), ec)) deleted++;
    }
    return deleted;
}

#if 0 // legacy Win32 deletefiles preserved for reference until we mothball
int32_t deletefiles_LEGACY_unused(const char *name)
{
    struct _finddata_t data;

    char dir[MAXPATHLEN], file[MAXPATHLEN];

    char fdrive[_MAX_DRIVE];
    char fdir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char fext[_MAX_EXT];

    _splitpath(name, fdrive, fdir, fname, fext);
    sprintf(dir, "%s%s", fdrive, fdir);

    int32_t deleted = 0;

    int32_t found = _findfirst(name, &data);
    while (found != -1)
    {
        strncpyz(file, dir, MAXPATHLEN);
        strncatz(file, data.name, MAXPATHLEN);
        if (DeleteFile(file))
            deleted++;

        found = _findfirst(name, &data);
    }

    return deleted;
}
#endif // legacy Win32 deletefiles_LEGACY_unused

int32_t flen(FILE* f)
{
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    return static_cast<int32_t>(n);
}

// Returns system ticks in milliseconds since system was turned on
uint32_t tickcount()
{
    using namespace std::chrono;
    static const auto epoch = steady_clock::now();
    return static_cast<uint32_t>(
        duration_cast<milliseconds>(steady_clock::now() - epoch).count());
}

// *************** Error Functions *****************

void ThreadError(const char *error, const char *extra)
{
    char buf[101];
    snprintf(buf, sizeof(buf), error, extra);

#if 0 // TODO(port): Subsystem 2 — DirectDraw/Win32 presentation (→ sokol)
    if (extra)
        _RPT1(_CRT_ERROR, error, extra);
    else
        _RPT0(_CRT_ERROR, error);

//  MSG Message;
//  while (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
//  {
//      TranslateMessage(&Message);
//      DispatchMessage(&Message);
//  }

//  MessageBox(nullptr, buf, "FATAL ERROR", MB_ICONSTOP | MB_OK);
#else
    if (extra)
        log_error(error, extra);
    else
        log_error("%s", error);
#endif
}

void FatalError(const char *error, const char *extra)
{
    static bool alreadyin;

    if (alreadyin)
        exit(1);
    alreadyin = true;

    char buf[101];
    snprintf(buf, sizeof(buf), error, extra);

  // Write error string to display
    Status(buf);
    Status("Press any key to exit");

  // FatalError aborts the process; the OS reclaims everything. Skipping
  // the partial Shutdown dance keeps this path from racing the live
  // ShutdownGlobals path and matches the "InitGlobals/ShutdownGlobals
  // are the canonical callers" rule for file-scope globals.
    if (extra)
        log_fatal(error, extra);
    else
        log_fatal("%s", error);

    exit(1);
}

void Error(const char *error, const char *extra)
{
    FatalError(error, extra);
/*
    char buf[101];
    sprintf(buf, error, extra);

  // Process windows messages (for Jong)
    MSG Message;
    while (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }

    MessageBox(nullptr, buf, "FATAL ERROR", MB_ICONSTOP | MB_OK);

    exit(1);
*/
}

// TODO(port): Subsystem 4 — threading. These two helpers model blocking on
// Win32 HANDLE-based events/mutexes and will be replaced by the worker-pool
// / completions API (see feedback_threading memory + PORT_PLAN Phase 3).
// Bodies stubbed so Phase 2 links; current call sites in mappane.cpp still
// compile against the signatures.
void WaitSingleErr(HANDLE /*obj*/)
{
}

void WaitMultipleErr(uint32_t /*objs*/, const HANDLE* /*obj*/, bool /*all*/)
{
}

// *************** Display Status Functions *****************

void Status(const char *fmt, ...)
{
#if 0 // TODO(port): Subsystem 2 — DirectDraw/Win32 presentation (→ sokol)
    static char buf[1024]; // Temporary buf for output
    static int32_t y = 10;

    if (!DirectDraw || !SystemFont || !Display.GetSurface())
        return;

    va_list marker;
    va_start(marker, fmt);
    vsnprintf(buf, sizeof(buf), fmt, marker);
    va_end(marker);

    char *s = buf;
    char *p = buf;
    while (1)
    {
        if (*p == '\n' || *p == '\0')
        {
            char save = *p;
            *p = '\0';
            if (strlen(s) > 0)
            {
                Display.WriteText(s, 10, y, 1, SystemFont, nullptr, DM_TRANSPARENT | DM_ALIAS);
                Display.PutToScreen(0, 0, Display.Width(), Display.Height());
                y += 10;
            }
            if (!save)
                break;
            s = p + 1;
        }
        p++;
    }
#else
    char buf[1024];
    va_list marker;
    va_start(marker, fmt);
    vsnprintf(buf, sizeof(buf), fmt, marker);
    va_end(marker);
    // Drop trailing newline — the logger adds its own.
    size_t blen = strlen(buf);
    while (blen > 0 && (buf[blen - 1] == '\n' || buf[blen - 1] == '\r'))
        buf[--blen] = '\0';
    log_info("%s", buf);
#endif
}

// *************** Critical Section Functions *****************

void BEGIN_CRITICAL()
{
#if 0 // TODO(port): Subsystem 4 — threading (→ std::mutex / worker pool)
    EnterCriticalSection(&CriticalSection);
#endif
}

void END_CRITICAL()
{
#if 0 // TODO(port): Subsystem 4 — threading (→ std::mutex / worker pool)
    LeaveCriticalSection(&CriticalSection);
#endif
}

// ****** Exit the Game - Why would anyone want to do that? ******

void ExitGame()
{
#if 0 // TODO(port): Subsystem 2 — DirectDraw/Win32 presentation (→ sokol_app)
    PostQuitMessage(0);
#else
    std::exit(0);
#endif
}

// *************** Settings Functions *****************
//
// Subsystem 5 port: these were Win32 GetPrivateProfile* / WritePrivateProfile*
// calls in the 1999 source. Now backed by simpleini (CSimpleIniA) with the
// same public signatures so call sites in ctrlmap.cpp, editor.cpp, and the
// main settings loader don't need touching. The original Win32 bodies live
// in attic/src/revutils_win32_ini.cpp.
//
// Behaviour preserved from the original: every INIGet* re-writes the value
// (default or parsed) back to the file via its matching INISet*. That's how
// Revenant.ini ends up as a self-documenting record of every option the
// game queries. To keep that semantic with simpleini, each Set flushes to
// disk immediately via SaveFile(). Volume is tiny (a few dozen keys at
// startup) so the IO cost is irrelevant.

namespace {

std::filesystem::path g_iniPath;
std::string g_iniSection;
CSimpleIniA g_ini;
bool g_iniLoaded = false;

void EnsureLoaded()
{
    if (g_iniLoaded)
        return;
    g_ini.SetUnicode(false);
    g_ini.SetMultiKey(false);
    g_ini.SetQuotes(false);
    if (!g_iniPath.empty())
        g_ini.LoadFile(g_iniPath.string().c_str());
    g_iniLoaded = true;
}

void Flush()
{
    if (g_iniPath.empty())
        return;
    g_ini.SaveFile(g_iniPath.string().c_str());
}

// Lowercase in-place helper (replacement for the Win32 strlwr() used by the
// old INIGetBool body).
std::string ToLower(const char* s)
{
    std::string out = s ? s : "";
    for (char& c : out)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return out;
}

} // namespace

void INISetSection(const char* newsection)
{
    g_iniSection = newsection ? newsection : "";
}

// Called from revmain at startup to fix the .ini location relative to the
// user's save directory. Not declared in revutils.h because revmain owns
// path policy; keep the definition here so revutils.cpp is self-contained.
void INISetPath(const char* /*runpath*/)
{
    constexpr const char* ininame = "Revenant.ini";

    std::filesystem::path savePath = SavePath;
    std::filesystem::path runPath  = RunPath;

    g_iniPath = savePath / ininame;

    // If SavePath and RunPath differ and SavePath doesn't yet hold an .ini,
    // seed it from RunPath so first-run defaults come from the install.
    std::error_code ec;
    if (!std::filesystem::equivalent(savePath, runPath, ec))
    {
        if (!std::filesystem::exists(g_iniPath, ec))
        {
            std::filesystem::path src = runPath / ininame;
            if (std::filesystem::exists(src, ec))
                std::filesystem::copy_file(src, g_iniPath,
                    std::filesystem::copy_options::overwrite_existing, ec);
        }
    }

    g_iniLoaded = false;
    EnsureLoaded();
}

int32_t INIGetInt(const char* key, int32_t def, char* format)
{
    EnsureLoaded();
    const int32_t i = static_cast<int32_t>(
        g_ini.GetLongValue(g_iniSection.c_str(), key, def));
    INISetInt(key, i, format);
    return i;
}

void INISetInt(const char* key, int32_t i, char* format)
{
    EnsureLoaded();
    if (!format)
        format = const_cast<char*>("%d");

    char buf[32];
    std::snprintf(buf, sizeof(buf), format, i);

    g_ini.SetValue(g_iniSection.c_str(), key, buf);
    Flush();
}

char* INIGetText(const char* key, char* def, char* buf, int32_t buflen)
{
    EnsureLoaded();

    static char s[128];
    if (!buf)
    {
        buf = s;
        buflen = 128;
    }
    if (!def)
        def = const_cast<char*>("");

    // Original semantics: stored text is quoted; default is also quoted for
    // the Win32 GetPrivateProfileString call so the retrieved raw value ends
    // with surrounding quotes, which the old code then strips.
    const char* raw = g_ini.GetValue(g_iniSection.c_str(), key, nullptr);
    std::string value;
    if (raw)
    {
        value = raw;
        if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
            value = value.substr(1, value.size() - 2);
    }
    else
    {
        value = def;
    }

    strncpyz(buf, value.c_str(), buflen);
    INISetText(key, buf);
    return buf;
}

void INISetText(const char* key, char* str)
{
    EnsureLoaded();
    std::string quoted = "\"";
    quoted += (str ? str : "");
    quoted += "\"";
    g_ini.SetValue(g_iniSection.c_str(), key, quoted.c_str());
    Flush();
}

char* INIGetStr(const char* key, char* def, char* buf, int32_t buflen)
{
    EnsureLoaded();

    static char s[128];
    if (!buf)
    {
        buf = s;
        buflen = 128;
    }
    if (!def)
        def = const_cast<char*>("");

    const char* raw = g_ini.GetValue(g_iniSection.c_str(), key, def);
    strncpyz(buf, raw ? raw : "", buflen);
    INISetStr(key, buf);
    return buf;
}

void INISetStr(const char* key, char* str)
{
    EnsureLoaded();
    g_ini.SetValue(g_iniSection.c_str(), key, str ? str : "");
    Flush();
}

int32_t INIGetArray(const char* key, int32_t size, int32_t* ary,
                    int32_t defsize, int32_t* defary, char* format)
{
    EnsureLoaded();

    if (ary != defary)
        std::memset(ary, 0, sizeof(int32_t) * size);

    const char* raw = g_ini.GetValue(g_iniSection.c_str(), key, "");
    std::string buf = raw ? raw : "";

    int32_t newsize = 0;
    if (buf.empty())
    {
        if (defary)
        {
            while (newsize < defsize && newsize < size)
            {
                ary[newsize] = defary[newsize];
                newsize++;
            }
        }
    }
    else
    {
        // strtok mutates — use a mutable copy.
        std::string scratch = buf;
        char* tok = std::strtok(scratch.data(), ",");
        while (tok && newsize < size)
        {
            ary[newsize++] = std::atol(tok);
            tok = std::strtok(nullptr, ",");
        }
    }

    INISetArray(key, newsize, ary, format);
    return newsize;
}

void INISetArray(const char* key, int32_t size, int32_t ary[], const char* format)
{
    EnsureLoaded();
    if (!format)
        format = "%d";

    std::string out;
    char tmp[32];
    for (int32_t c = 0; c < size; c++)
    {
        if (c >= 1)
            out.push_back(',');
        std::snprintf(tmp, sizeof(tmp), format, ary[c]);
        out += tmp;
    }

    g_ini.SetValue(g_iniSection.c_str(), key, out.c_str());
    Flush();
}

bool INIGetBool(const char* key, bool def, const char* yes, const char* no)
{
    EnsureLoaded();

    const std::string yesTokens = yes ? ToLower(yes) : "yes on true 1";
    const std::string noTokens  = no  ? ToLower(no)  : "no off false 0";

    const char* raw = g_ini.GetValue(g_iniSection.c_str(), key, "");
    const std::string val = ToLower(raw ? raw : "");

    bool b;
    if (!val.empty() && yesTokens.find(val) != std::string::npos)
        b = true;
    else if (!val.empty() && noTokens.find(val) != std::string::npos)
        b = false;
    else
        b = def;

    INISetBool(key, b, yes, no);
    return b;
}

void INISetBool(const char* key, bool on, const char* yes, const char* no)
{
    EnsureLoaded();
    if (!yes) yes = "1";
    if (!no)  no  = "0";
    g_ini.SetValue(g_iniSection.c_str(), key, on ? yes : no);
    Flush();
}

bool INIGetYesNo(const char* key, bool def)      { return INIGetBool(key, def, "Yes", "No"); }
void INISetYesNo(const char* key, bool on)       {        INISetBool(key, on, "Yes", "No"); }
bool INIGetTrueFalse(const char* key, bool def)  { return INIGetBool(key, def, "True", "False"); }
void INISetTrueFalse(const char* key, bool on)   {        INISetBool(key, on, "True", "False"); }
bool INIGetOnOff(const char* key, bool def)      { return INIGetBool(key, def, "On", "Off"); }
void INISetOnOff(const char* key, bool on)       {        INISetBool(key, on, "On", "Off"); }

// Declared in parse.cpp. Used by INIParse below.
bool ParseAnything(bool stack, TToken& t, const char* format, va_list ap);

bool INIParse(const char* key, const char* def, char* format, ...)
{
    char buf[128];
    INIGetStr(key, const_cast<char*>(def), buf, 128);

    va_list ap;
    va_start(ap, format);

    TStringParseStream s(buf, strlen(buf));
    TToken t(s);
    t.Get();
    const bool retval = ParseAnything(true, t, format, ap);

    va_end(ap);
    return retval;
}

void INIPrint(const char* key, const char* format, ...)
{
    char buf[128];

    va_list marker;
    va_start(marker, format);
    std::vsnprintf(buf, sizeof(buf), format, marker);
    va_end(marker);

    INISetStr(key, buf);
}

// *************** Allocation Functions *****************

int32_t TotalAllocated, MaxAllocated;

#ifdef _DEBUG

#undef malloc
#undef free
#undef realloc

void *xmalloc(int32_t size, char *file, int32_t line)
{
    TotalAllocated += size;
    if (TotalAllocated > MaxAllocated)
        MaxAllocated = TotalAllocated;
    void *p = _malloc_dbg(size, _NORMAL_BLOCK, file, line);
    return p;
}

void *xrealloc(void *p, int32_t size, char *file, int32_t line)
{
    TotalAllocated = TotalAllocated - _msize(p) + size;
    if (TotalAllocated > MaxAllocated)
        MaxAllocated = TotalAllocated;
    p = _realloc_dbg(p, size, _NORMAL_BLOCK, file, line);
    return p;
}

void xfree(void *p)
{
    if (p != nullptr)
        TotalAllocated -= _msize(p);
    _free_dbg(p, _NORMAL_BLOCK);
}

void* __cdecl operator new(size_t size)
{
    void *p = xmalloc(size,"new",0);
    if (!p)
        FatalError("OUT OF MEMORY - Increase your virtual swap file size", nullptr);
    else
        memset(p, 0, size);

    return p;
}

void __cdecl operator delete(void *pointer)
{
    xfree(pointer);
}

#else

#undef malloc
#undef free
#undef realloc

void *xmalloc(int32_t size)
{
    TotalAllocated += size;
    if (TotalAllocated > MaxAllocated)
        MaxAllocated = TotalAllocated;
    void *p = malloc(size);
    memset(p, 0, size);
    return p;
}

void *xrealloc(void *p, int32_t size)
{
#if 0 // TODO(port): Subsystem 7 — allocation tracking (→ platform-agnostic msize)
    TotalAllocated = TotalAllocated - _msize(p) + size;
    if (TotalAllocated > MaxAllocated)
        MaxAllocated = TotalAllocated;
#endif
    p = realloc(p, size);
    return p;
}

void xfree(void *p)
{
#if 0 // TODO(port): Subsystem 7 — allocation tracking (→ platform-agnostic msize)
    if (p != nullptr)
        TotalAllocated -= _msize(p);
#endif
    free(p);
}

void* operator new(size_t size)
{
    void *p = xmalloc(size);
    if (!p)
        FatalError("OUT OF MEMORY - Increase your virtual swap file size", nullptr);
    return p;
}

void operator delete(void *pointer) noexcept
{
    xfree(pointer);
}

#endif

// Forward declarations for helpers defined further down the TU.
static void rev_normalize_sep(char *p);
namespace {
    std::filesystem::path vfs_resolve_data_root();
    const std::filesystem::path &vfs_data_root();
}

// Walk up from `start` looking for a directory that contains both `src`
// and `revisited` siblings (our repo layout). Returns empty path on
// failure. Used by overlay discovery for the dev workflow when the exe
// lives in <repo>/build/.
static std::filesystem::path find_repo_root(const std::filesystem::path &start)
{
    namespace fs = std::filesystem;
    std::error_code ec;
    fs::path p = start;
    for (int i = 0; i < 8 && !p.empty() && p != p.root_path(); ++i)
    {
        if (fs::exists(p / "src", ec) && fs::exists(p / "revisited", ec))
            return p;
        p = p.parent_path();
    }
    return {};
}

const char *rev_resolve_revisited_overlay()
{
    namespace fs = std::filesystem;
    static std::string s_resolved;
    static bool s_done = false;
    if (s_done) return s_resolved.c_str();
    s_done = true;

    // Strict opt-in: the Revisited overlay only applies when the user
    // launches with --revisited. Without the flag, return empty even if
    // a revisited/ folder or RevenantRevisited.rvr sits next to the exe,
    // so the game runs pure vanilla retail by default.
    if (!RevisitedEnabled)
        return "";

    auto looks_like_overlay = [](const fs::path &p) -> bool {
        // For now: anything that exists. The overlay can be empty (no
        // patches yet) and that's fine — we just won't find any files
        // in it. Phase 2+ may add a manifest sentinel here.
        std::error_code ec;
        return fs::exists(p, ec);
    };

    // 1) explicit env-var override
    if (const char *env = getenv("REVENANT_REVISITED_PATH"))
    {
        fs::path p(env);
        if (looks_like_overlay(p))
        {
            s_resolved = (p.string() + "/");
            log_info("[overlay] revisited: %s (env)", s_resolved.c_str());
            return s_resolved.c_str();
        }
    }

    // Resolve exe directory once.
    fs::path exe_dir;
#if defined(__APPLE__)
    {
        char raw[MAXPATHLEN];
        uint32_t sz = sizeof(raw);
        if (_NSGetExecutablePath(raw, &sz) == 0)
        {
            std::error_code ec;
            fs::path p = fs::weakly_canonical(fs::path(raw), ec);
            exe_dir = (ec ? fs::path(raw) : p).parent_path();
        }
    }
#elif defined(__linux__)
    {
        char raw[MAXPATHLEN];
        ssize_t n = readlink("/proc/self/exe", raw, sizeof(raw) - 1);
        if (n > 0) { raw[n] = 0; exe_dir = fs::path(raw).parent_path(); }
    }
#endif

    // 2) production: <exe-dir>/RevenantRevisited.rvr
    // 3) production alt: <RunPath>/RevenantRevisited.rvr — RunPath is set
    //    by rev_resolve_program_paths so check the global directly.
    std::error_code ec;
    for (const fs::path cand : { exe_dir / "RevenantRevisited.rvr",
                                  fs::path(RunPath) / "RevenantRevisited.rvr" })
    {
        if (cand.empty()) continue;
        if (fs::exists(cand, ec))
        {
            s_resolved = cand.string();
            log_info("[overlay] revisited: %s (rvr archive)", s_resolved.c_str());
            return s_resolved.c_str();
        }
    }

    // 4) dev: walk up from exe_dir to find <repo>/revisited/resources/.
    if (!exe_dir.empty())
    {
        fs::path repo = find_repo_root(exe_dir);
        if (!repo.empty())
        {
            fs::path p = repo / "revisited" / "resources";
            if (looks_like_overlay(p))
            {
                s_resolved = (p.string() + "/");
                log_info("[overlay] revisited: %s (loose dev folder)",
                         s_resolved.c_str());
                return s_resolved.c_str();
            }
        }
    }

    // --revisited was requested but no overlay is reachable. Hard fail —
    // the user explicitly asked for Revisited features and silently
    // dropping back to vanilla would mask the misconfiguration. Log the
    // full search path first since FatalError's display buffer is short.
    log_error("[overlay] --revisited overlay not found. Searched (in order):");
    log_error("  1) $REVENANT_REVISITED_PATH");
    log_error("  2) <exe-dir>/RevenantRevisited.rvr");
    log_error("  3) <RunPath>/RevenantRevisited.rvr");
    log_error("  4) <repo-root>/revisited/resources/ (dev layout)");
    FatalError("--revisited specified but no overlay found (see log for search paths)", nullptr);
    return "";
}

// Resolve the platform-conventional per-user data directory:
//   macOS:   ~/Library/Application Support/Revenant/
//   Linux:   $XDG_DATA_HOME/Revenant/ (default ~/.local/share/Revenant/)
//   Windows: %LOCALAPPDATA%\Revenant\
// Returns an empty path on failure (no $HOME, etc.).
static std::filesystem::path resolve_user_data_dir()
{
    namespace fs = std::filesystem;
#if defined(__APPLE__)
    if (const char *home = getenv("HOME"))
        return fs::path(home) / "Library" / "Application Support" / "Revenant";
#elif defined(__linux__)
    if (const char *xdg = getenv("XDG_DATA_HOME"))
        return fs::path(xdg) / "Revenant";
    if (const char *home = getenv("HOME"))
        return fs::path(home) / ".local" / "share" / "Revenant";
#elif defined(_WIN32)
    if (const char *appdata = getenv("LOCALAPPDATA"))
        return fs::path(appdata) / "Revenant";
#endif
    return {};
}

// Posix port of retail's GetProgramPaths probe. Splits the two roles
// retail collapsed when installed on HD:
//   * RunPath  = install dir (read-only-safe assets: Imagery/, Modules/,
//                resources.rvr, imagery.rvi). Resolved via the same
//                logic that VFS uses (env var $REVENANT_DATA_PATH, or
//                cwd / cwd/data / cwd/../data — see vfs_resolve_data_root).
//                Inside a macOS .app bundle, this naturally lands in
//                Contents/Resources/data/ when shipped that way.
//   * SavePath = writable per-user data dir (Revenant.ini, curmap/,
//                Save/Single/<n>/, …) at the platform-conventional
//                Application Support / XDG_DATA_HOME / LOCALAPPDATA.
//                Created if missing; if creation fails we abort —
//                Revenant requires a writable user-data dir and refuses
//                to run from non-writable media.
// Both buffers are filled with a trailing '/' so makepath / rev_fopen
// can concat directly.
void rev_resolve_program_paths(char *RunPath, char *SavePath, int32_t buflen)
{
    namespace fs = std::filesystem;

    // ---------- SavePath: per-user writable dir, mandatory. ----------
    fs::path save = resolve_user_data_dir();
    if (save.empty())
    {
        FatalError("Revenant: cannot resolve per-user data directory. "
                   "Set $HOME (Linux/macOS) or %LOCALAPPDATA% (Windows).");
    }
    {
        std::error_code ec;
        fs::create_directories(save, ec);
        if (ec)
        {
            char msg[512];
            std::snprintf(msg, sizeof(msg),
                "Revenant: cannot create user-data directory '%s' (%s). "
                "The game requires writable storage and refuses to run "
                "from read-only media.",
                save.string().c_str(), ec.message().c_str());
            FatalError(msg);
        }
        // Sanity-check writability with an actual probe — create_directories
        // can succeed on read-only mounts in odd circumstances.
        const fs::path probe = save / ".revenant_write_test";
        if (FILE *fp = fopen(probe.string().c_str(), "wb"))
        {
            fclose(fp);
            std::error_code rmec;
            fs::remove(probe, rmec);
        }
        else
        {
            char msg[512];
            std::snprintf(msg, sizeof(msg),
                "Revenant: user-data directory '%s' exists but is not "
                "writable. The game refuses to run from read-only media.",
                save.string().c_str());
            FatalError(msg);
        }
        std::string s = save.string() + "/";
        strncpyz(SavePath, s.c_str(), buflen);
    }

    // ---------- RunPath: install dir (read-only assets). ----------
    // Reuse the VFS data-root probe — it already supports the
    // $REVENANT_DATA_PATH env var and cwd-relative dev fallbacks. If
    // SavePath itself looks like a populated install (someone copied
    // the data tree there), prefer that — matches retail's HD-install
    // case where SavePath == RunPath.
    fs::path run;
    {
        std::error_code ec;
        if (fs::exists(save / "imagery.rvi", ec) ||
            fs::exists(save / "Modules", ec))
        {
            run = save;
        }
        else
        {
            run = vfs_data_root();
        }
    }
    if (!run.empty())
    {
        std::string s = run.string() + "/";
        strncpyz(RunPath, s.c_str(), buflen);
    }
    else
    {
        // Couldn't find an install. Fall back to SavePath so the engine
        // doesn't crash before logging — but most asset reads will
        // FatalError shortly with a clear "missing data" message from
        // the VFS layer, which the user can act on.
        log_error("[paths] could not locate the Revenant install (looked in "
                  "$REVENANT_DATA_PATH, %s, ./data/, ../data/). Asset loads "
                  "will fail. Install the data tree under '%s' or set "
                  "$REVENANT_DATA_PATH.",
                  save.string().c_str(), save.string().c_str());
        strncpyz(RunPath, SavePath, buflen);
    }

    log_info("[paths] RunPath  (install)  = %s", RunPath);
    log_info("[paths] SavePath (user data) = %s", SavePath);
}

char *makepath(char *name, char *buf, int32_t buflen)
{
  // If root path expicitly given, (i.e. "c:\", or "\" or "\\" or "..") use it
    if (name[0] == '\\' || name[1] == ':' || (name[0] == '.' && name[1] == '.'))
    {
        strncpyz(buf, name, buflen);
        rev_normalize_sep(buf);
        return buf;
    }

  // If root path is ".", substitute SavePath or RunPath
    if (name[0] == '.')
    {
        name++;
        while (name[0] == '\\' || name[0] == '/')
            name++;
    }

  // Always try SavePath first (SavePath will always be writable directory on hard drive)
    strncpyz(buf, SavePath, buflen);
    strncatz(buf, name, buflen);
    rev_normalize_sep(buf);

    return buf;
}

// Normalize Win32 backslash separators to POSIX forward slashes in place.
// The 1998 data paths were authored with '\' throughout; retail INI values
// still carry them. Anywhere the engine builds a path from those strings
// and hands it to fopen(3), we have to translate.
static void rev_normalize_sep(char *p)
{
    for (; *p; ++p)
        if (*p == '\\')
            *p = '/';
}

// ============================================================================
// ZIP-VFS: retail pack-archive mount layer
// ----------------------------------------------------------------------------
// Retail shipped data/resources.rvr, data/imagery.rvi and data/Modules/<N>.rvm
// as stored (uncompressed) ZIPs. WinMain (FUN_004865a0) mounted the two base
// archives explicitly — see FUN_0045f980 + DAT_00666448 — and a per-module
// archive was swapped in when the active module changed. Different modules
// share root filenames (module.def, state.def, exit.def, area.def, ...) so
// a flat "index everything up front" scan would collide across modules.
//
// API: MountArchive / MountModule / UnmountModule / UnmountAll.
// Lookup in rev_fopen: SavePath → RunPath → active module map → base map.
// ============================================================================

namespace {

struct VFSArchive
{
    std::string path;
    mz_zip_archive zip{};
    ~VFSArchive() { mz_zip_reader_end(&zip); }
};

struct VFSEntry
{
    VFSArchive *arch;
    mz_uint file_index;
    size_t size;
};

std::vector<std::unique_ptr<VFSArchive>> g_base_archives;
std::unordered_map<std::string, VFSEntry> g_base_map;
std::unique_ptr<VFSArchive> g_module_archive;
std::unordered_map<std::string, VFSEntry> g_module_map;
// Pre-release modules ship unpacked as data/Modules/<name>/; retail uses
// data/Modules/<name>.rvm ZIPs. MountModule detects which and we fall back
// to filesystem lookup from this directory when set.
std::filesystem::path g_module_dir;

std::filesystem::path g_data_root;
bool g_data_root_resolved = false;

std::string vfs_lower(std::string s)
{
    for (auto &c : s)
        c = (char)tolower((unsigned char)c);
    return s;
}

std::string vfs_basename_lower(const char *p)
{
    const char *name = p;
    for (const char *q = p; *q; ++q)
        if (*q == '/' || *q == '\\')
            name = q + 1;
    return vfs_lower(std::string(name));
}

// Locate the user's existing Revenant install (read-only assets:
// Imagery/, Modules/, resources.rvr, imagery.rvi). We do NOT ship a
// copy of these — the user must already own a Revenant install (GoG,
// CD/DVD copy, etc.). Resolution order:
//   1. $REVENANT_DATA_PATH env var (explicit override)
//   2. cwd, cwd/data, cwd/../data         (dev: running from repo root)
//   3. <exe-dir>, <exe-dir>/data, <exe-dir>/../data,
//      <exe-dir>/../Resources/data         (drop our binary into the user's install)
// Returns empty path on failure; caller is responsible for surfacing a
// clear "set REVENANT_DATA_PATH or place our binary alongside your
// Revenant install" message.
std::filesystem::path vfs_resolve_data_root()
{
    namespace fs = std::filesystem;
    auto looks_like_revenant = [](const fs::path &cand) -> bool {
        std::error_code ec;
        return fs::exists(cand / "imagery.rvi", ec) ||
               fs::exists(cand / "resources.rvr", ec) ||
               fs::exists(cand / "Modules", ec);
    };

    if (const char *env = getenv("REVENANT_DATA_PATH"))
    {
        fs::path p(env);
        if (fs::exists(p) && looks_like_revenant(p))
            return p;
    }

    const fs::path cwd = fs::current_path();
    for (const fs::path cand : {cwd, cwd / "data", cwd / ".." / "data"})
    {
        std::error_code ec;
        if (looks_like_revenant(cand))
            return fs::canonical(cand, ec);
    }

    fs::path exe_dir;
#if defined(__APPLE__)
    {
        char raw[MAXPATHLEN];
        uint32_t sz = sizeof(raw);
        if (_NSGetExecutablePath(raw, &sz) == 0)
        {
            std::error_code ec;
            fs::path p = fs::weakly_canonical(fs::path(raw), ec);
            exe_dir = (ec ? fs::path(raw) : p).parent_path();
        }
    }
#elif defined(__linux__)
    {
        char raw[MAXPATHLEN];
        ssize_t n = readlink("/proc/self/exe", raw, sizeof(raw) - 1);
        if (n > 0) { raw[n] = 0; exe_dir = fs::path(raw).parent_path(); }
    }
#endif
    if (!exe_dir.empty())
    {
        for (const fs::path cand : {
            exe_dir,
            exe_dir / "data",
            exe_dir / ".." / "data",
            // macOS .app bundle: Contents/MacOS/<exe> → ../Resources/data/
            exe_dir / ".." / "Resources" / "data",
        })
        {
            std::error_code ec;
            if (looks_like_revenant(cand))
                return fs::canonical(cand, ec);
        }
    }

    return {};
}

const std::filesystem::path &vfs_data_root()
{
    if (!g_data_root_resolved)
    {
        g_data_root_resolved = true;
        g_data_root = vfs_resolve_data_root();
    }
    return g_data_root;
}

std::unique_ptr<VFSArchive> vfs_open_archive(const std::filesystem::path &path)
{
    auto arc = std::make_unique<VFSArchive>();
    arc->path = path.string();
    if (!mz_zip_reader_init_file(&arc->zip, arc->path.c_str(), 0))
        return nullptr;
    return arc;
}

void vfs_index_archive(VFSArchive *arc, std::unordered_map<std::string, VFSEntry> &map)
{
    const mz_uint n = mz_zip_reader_get_num_files(&arc->zip);
    for (mz_uint i = 0; i < n; ++i)
    {
        mz_zip_archive_file_stat st;
        if (!mz_zip_reader_file_stat(&arc->zip, i, &st))
            continue;
        if (st.m_is_directory)
            continue;
        // First-wins within a single archive (same as retail)
        map.try_emplace(vfs_basename_lower(st.m_filename),
                        VFSEntry{arc, i, (size_t)st.m_uncomp_size});
    }
}

struct VFSHandle
{
    std::vector<uint8_t> buf;
    size_t pos;
};

int vfs_read(void *c, char *p, int n)
{
    auto *h = static_cast<VFSHandle *>(c);
    const size_t avail = h->buf.size() - h->pos;
    size_t copy = n < 0 ? 0 : (size_t)n;
    if (copy > avail)
        copy = avail;
    memcpy(p, h->buf.data() + h->pos, copy);
    h->pos += copy;
    return (int)copy;
}

fpos_t vfs_seek(void *c, fpos_t off, int whence)
{
    auto *h = static_cast<VFSHandle *>(c);
    fpos_t newpos = 0;
    switch (whence)
    {
    case SEEK_SET: newpos = off; break;
    case SEEK_CUR: newpos = (fpos_t)h->pos + off; break;
    case SEEK_END: newpos = (fpos_t)h->buf.size() + off; break;
    default: return -1;
    }
    if (newpos < 0 || (size_t)newpos > (fpos_t)h->buf.size())
        return -1;
    h->pos = (size_t)newpos;
    return newpos;
}

int vfs_close(void *c)
{
    delete static_cast<VFSHandle *>(c);
    return 0;
}

FILE *rev_vfs_open(const char *name, const char *flags)
{
    if (!flags || flags[0] != 'r' || strchr(flags, '+'))
        return nullptr;

    const auto key = vfs_basename_lower(name);

    const VFSEntry *entry = nullptr;
    if (auto it = g_module_map.find(key); it != g_module_map.end())
        entry = &it->second;
    else if (auto it = g_base_map.find(key); it != g_base_map.end())
        entry = &it->second;
    if (!entry)
        return nullptr;

    auto h = std::make_unique<VFSHandle>();
    h->buf.resize(entry->size);
    h->pos = 0;
    if (!mz_zip_reader_extract_to_mem(&entry->arch->zip, entry->file_index,
                                      h->buf.data(), h->buf.size(), 0))
        return nullptr;

    FILE *fp = funopen(h.get(), vfs_read, nullptr, vfs_seek, vfs_close);
    if (fp)
        h.release();
    return fp;
}

} // anonymous namespace

bool MountArchive(const char *name)
{
    namespace fs = std::filesystem;
    const fs::path root = vfs_data_root();
    if (root.empty())
    {
        log_error("[vfs] MountArchive(%s): no data root found", name);
        return false;
    }

    const fs::path p = root / name;
    auto arc = vfs_open_archive(p);
    if (!arc)
    {
        log_error("[vfs] error in pack file %s (%s)", name, p.string().c_str());
        return false;
    }

    VFSArchive *raw = arc.get();
    g_base_archives.push_back(std::move(arc));
    vfs_index_archive(raw, g_base_map);
    return true;
}

bool MountModule(const char *name)
{
    namespace fs = std::filesystem;
    UnmountModule();

    const fs::path root = vfs_data_root();
    if (root.empty())
    {
        log_error("[vfs] MountModule(%s): no data root found", name);
        return false;
    }

    // Pre-release unpacked module dir first.
    const fs::path dir = root / "Modules" / name;
    std::error_code ec;
    if (fs::is_directory(dir, ec))
    {
        g_module_dir = dir;
        log_info("[vfs] mounted module dir %s", dir.string().c_str());
        return true;
    }

    const fs::path p = root / "Modules" / (std::string(name) + ".rvm");
    auto arc = vfs_open_archive(p);
    if (!arc)
    {
        log_error("[vfs] error in pack file %s.rvm (%s)", name, p.string().c_str());
        return false;
    }

    VFSArchive *raw = arc.get();
    g_module_archive = std::move(arc);
    vfs_index_archive(raw, g_module_map);
    return true;
}

void UnmountModule()
{
    g_module_map.clear();
    g_module_archive.reset();
    g_module_dir.clear();
}

void UnmountAll()
{
    UnmountModule();
    g_base_map.clear();
    g_base_archives.clear();
}

FILE *rev_fopen(const char *name, const char *flags)
{
    if (!name)
        return nullptr;

    char fn[MAXPATHLEN];

  // If root path explicitly given (absolute POSIX, Win32 drive, or "..") use it
    if (name[0] == '/' || name[0] == '\\' || name[1] == ':' ||
        (name[0] == '.' && name[1] == '.'))
    {
        strncpyz(fn, name, MAXPATHLEN);
        rev_normalize_sep(fn);
        if (FILE *fp = fopen(fn, flags))
            return fp;
        return rev_vfs_open(name, flags);
    }

  // If root path is ".", strip it and any following separators
    if (name[0] == '.')
    {
        name++;
        while (name[0] == '\\' || name[0] == '/')
            name++;
    }

  // Always try SavePath first (writable on real installs — saves, INI).
    strncpyz(fn, SavePath, MAXPATHLEN);
    strncatz(fn, name, MAXPATHLEN);
    rev_normalize_sep(fn);

    FILE *fp = fopen(fn, flags);

  // Revisited overlay (Bug fixes, game-behavior tweaks, enhanced
  // graphics/UI). Strictly opt-in: empty string from the resolver means
  // "no overlay, run vanilla". Reads only — never used for write fallback.
  // See revisited/README.md for the contract.
    if (!fp)
    {
        const bool is_read = flags && flags[0] == 'r' && !strchr(flags, '+');
        if (is_read)
        {
            const char *overlay = rev_resolve_revisited_overlay();
            if (overlay && overlay[0])
            {
                strncpyz(fn, overlay, MAXPATHLEN);
                strncatz(fn, name, MAXPATHLEN);
                rev_normalize_sep(fn);
                fp = fopen(fn, flags);
            }
        }
    }

    if (!fp && stricmp(SavePath, RunPath) != 0)
    {
        strncpyz(fn, RunPath, MAXPATHLEN);
        strncatz(fn, name, MAXPATHLEN);
        rev_normalize_sep(fn);
        fp = fopen(fn, flags);
    }
    if (!fp && !g_module_dir.empty())
    {
        // Module-dir fallback: try <module_dir>/<basename>.
        const char *base = name;
        for (const char *q = name; *q; ++q)
            if (*q == '/' || *q == '\\')
                base = q + 1;
        std::filesystem::path mpath = g_module_dir / base;
        fp = fopen(mpath.string().c_str(), flags);
    }
    if (!fp)
    {
        // Data-root fallback: when the engine is launched from a build/ dir
        // with SavePath/RunPath = "./", loose-file paths like
        // "data/Save/Single/Foo/game.sav" won't resolve relative to cwd.
        // Try them under the detected data root as well. Strip a leading
        // "data/" since data_root IS the data dir.
        const std::filesystem::path &root = vfs_data_root();
        if (!root.empty())
        {
            const char *rel = name;
            if (strncasecmp(rel, "data/", 5) == 0 || strncasecmp(rel, "data\\", 5) == 0)
                rel += 5;
            std::filesystem::path dpath = root / rel;
            fp = fopen(dpath.string().c_str(), flags);
        }
    }
    if (!fp)
        fp = rev_vfs_open(name, flags);

    return fp;
}

// Random number function
int32_t random(int32_t min, int32_t max)
{
    if (min == max)
        return min;

    if (max < min)
    {
        int32_t t = min;
        min = max;
        max = t;
    }

    int32_t r = rand() % (max - min + 1);
    r += min;
    return r;
}

// ********* Comma Delimited String Lists **********

// Comma delimited list functions (useful for strings in "abcd,defg,hijk" format)
// If dst is nullptr, retuns result pointer from static internal buffer

#define LISTBUFLEN 128
static char listbuf[LISTBUFLEN];

// Get num string from comma list
char *listget(const char *src, int32_t num, char *dst, int32_t len)
{
    if (!dst)
    {
        dst = listbuf;
        len = LISTBUFLEN;
    }

    if (len <= 0)
        return dst;

    dst[0] = '\0';

    const char *p = src;
    int32_t comma = 0;
    while (*p && comma < num)
    {
        if (*p == ',')
            comma++;
        p++;
    }

    char *d = dst;
    int32_t l = 0;
    while (*p && *p != ',' && l < len - 1)
    {
        *d = *p;
        d++;
        p++;
        l++;
    }
    *d = '\0';

    return dst;
}

// Get total number of strings in comma list
int32_t listnum(const char *src)
{
    const char *p = src;
    int32_t comma = 0;
    while (*p)
    {
        if (*p == ',')
            comma++;
        p++;
    }

    return comma + 1;
}

// Get random string from comma list
char *listrnd(const char *src, char *dst, int32_t len)
{
    return listget(src, random(0, listnum(src) - 1), dst, len);
}

// Returns true if string is in comma list (case insensitive)
bool listin(const char *src, const char *in)
{
    const char *s = src;
    const char *i = in;

    while (*s)
    {
        if (*s == ',')
        {
            if (*i == '\0')
                break;
            i = in;
            s++;
        }
        else if (!*i || toupper(*i) != toupper(*s))
        {
            while (*s && *s != ',')
                s++;
            if (*s == ',')
                s++;
            i = in;
        }
        else
        {
            s++;
            i++;
        }
    }

    return *i == '\0';
}

// ********* Free Memory Functions **********

#if 0 // TODO(port): Subsystem 7 — memory introspection (→ host_statistics64 / sysctl)
uint32_t MemUsed()
{
    MEMORYSTATUS ms;
    ms.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&ms);
    return ms.dwMemoryLoad;
}

uint32_t FreeMem()
{
    MEMORYSTATUS ms;
    ms.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&ms);
    return ms.dwAvailVirtual;
}

uint32_t TotalMem()
{
    MEMORYSTATUS ms;
    ms.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&ms);
    return ms.dwTotalVirtual;
}

uint32_t FreePhys()
{
    MEMORYSTATUS ms;
    ms.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&ms);
    return ms.dwAvailPhys;
}

uint32_t TotalPhys()
{
    MEMORYSTATUS ms;
    ms.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&ms);
    return ms.dwTotalPhys;
}

uint32_t FreePage()
{
    MEMORYSTATUS ms;
    ms.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&ms);
    return ms.dwAvailPageFile;
}

uint32_t TotalPage()
{
    MEMORYSTATUS ms;
    ms.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&ms);
    return ms.dwTotalPageFile;
}
#else
uint32_t MemUsed()   { return 0; }
uint32_t FreeMem()   { return 0; }
uint32_t TotalMem()  { return 0; }
uint32_t FreePhys()  { return 0; }
uint32_t TotalPhys() { return 0; }
uint32_t FreePage()  { return 0; }
uint32_t TotalPage() { return 0; }
#endif
