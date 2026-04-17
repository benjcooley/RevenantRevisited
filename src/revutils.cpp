// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 2024 Cinematix                       *
// *                  revutils.cpp - Utility functions                     *
// *************************************************************************

#include "revutils.h"

#include "parse.h"

#include <fcntl.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <cctype>
#include <cstring>
#include <filesystem>
#include <string>
#include <system_error>

#include <SimpleIni.h>

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
        if (buf > 0)
            buf[0] = nullptr;
        return buf;
    }
    if (buflen >= 12) // Twelve is big enough for anything
    {
        _itoa(val, buf, 10);
        return buf;
    }
    char b[12];
    _itoa(val, b, 10);
    strncpy(buf, b, buflen - 1);
    buf[buflen - 1] = nullptr;
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

// Copies a file, or group of files using wildcards
int32_t copyfiles(const char *from, const char *to, bool overwrite)
{
    if (!from || !to)
        return 0;

    struct _finddata_t data;

    TStackString fdrive;
    TStackString fdir;
    TStackString fname;
    TStackString fext;

    char fromdir[MAXPATHLEN];
    char todir[MAXPATHLEN];
    char source[MAXPATHLEN];
    char dest[MAXPATHLEN];

    _splitpath(from, fdrive, fdir, fname, fext);
    sprintf(fromdir, "%s%s", fdrive, fdir);
    if (!fromdir[0])
        return 0;
    if (fromdir[strlen(fromdir) - 1] != '\\')
        strncatz(fromdir, "\\", MAXPATHLEN);

    _splitpath(to, fdrive, fdir, fname, fext);
    sprintf(todir, "%s%s", fdrive, fdir);
    if (!todir[0])
        return 0;
    if (todir[strlen(todir) - 1] != '\\')
        strncatz(todir, "\\", MAXPATHLEN);

    int32_t copied = 0;

    int32_t found, handle;
    found = handle = _findfirst(from, &data);
    while (found != -1)
    {
        sprintf(source, "%s%s", fromdir, data.name);
        sprintf(dest, "%s%s", todir, data.name);

        if (CopyFile(source, dest, !overwrite))
            copied++;

        found = _findnext(handle, &data);
    }

    return copied;
}

// Deletes a file, or group of files using wildcards
int32_t deletefiles(const char *name)
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

uint64_t flen(FILE* f)
{
    fseek(fp, 0L, SEEK_END);
    uint64_t sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    return sz;
}

// Returns system ticks in milliseconds since system was turned on
uint32_t tickcount()
{
    return GetTickCount();
}

// *************** Error Functions *****************

void ThreadError(const char *error, const char *extra)
{
    char buf[101];
    sprintf(buf, error, extra);

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
}

void FatalError(const char *error, const char *extra)
{
    static bool alreadyin;

    if (alreadyin)
        exit(1);
    alreadyin = true;

    char buf[101];
    sprintf(buf, error, extra);

  // Write error string to display
    Status(buf);
    Status("Press any key to exit");

  // Prevent passing of windows message (input etc) to current screen
    CurrentScreen = nullptr;

  // Stop timer stuff
    Timer.Close();

  // Close the display
    Display->Close();

  // Close Direct Draw
    CloseDirectDraw();

  // Close the window!  
    MainWindow.Close();

    MSG Message;
    while (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }

    if (!DirectDraw)
    {
        if (extra)
            _RPT1(_CRT_ERROR, error, extra);
        else
            _RPT0(_CRT_ERROR, error);
    }

//  ShowWindow(MainWindow.Hwnd(), 0);
//  MessageBox(nullptr, buf, "FATAL ERROR", MB_ICONSTOP | MB_OK);

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
    static char buf[1024]; // Temporary buf for output
    static y = 10;

    if (!DirectDraw || !SystemFont || !Display->GetSurface())
        return;

    va_list marker;
    va_start(marker, fmt);
    vsprintf(buf, fmt, marker);

    char *s = buf;
    char *p = buf;
    while (1)
    {
        if (*p == '\n' || *p == nullptr)
        {
            char save = *p;
            *p = nullptr;
            if (strlen(s) > 0)
            {
                Display->WriteText(s, 10, y, 1, SystemFont, nullptr, DM_TRANSPARENT | DM_ALIAS);
                Display->PutToScreen(0, 0, Display->Width(), Display->Height());
                y += 10;
            }
            if (!save)
                break;
            s = p + 1;
        }
        p++;
    }
}

// *************** Critical Section Functions *****************

void BEGIN_CRITICAL()
{
    EnterCriticalSection(&CriticalSection);
}

void END_CRITICAL()
{
    LeaveCriticalSection(&CriticalSection);
}

// ****** Exit the Game - Why would anyone want to do that? ******

void ExitGame()
{
    PostQuitMessage(0);
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
    TotalAllocated = TotalAllocated - _msize(p) + size;
    if (TotalAllocated > MaxAllocated)
        MaxAllocated = TotalAllocated;
    p = realloc(p, size);
    return p;
}

void xfree(void *p)
{
    if (p != nullptr)
        TotalAllocated -= _msize(p);
    free(p);
}

void* __cdecl operator new(size_t size)
{
    void *p = xmalloc(size);
    if (!p)
        FatalError("OUT OF MEMORY - Increase your virtual swap file size", nullptr);
    return p;
}

void __cdecl operator delete(void *pointer)
{
    xfree(pointer);
}

#endif

char *makepath(char *name, char *buf, int32_t buflen)
{
  // If root path expicitly given, (i.e. "c:\", or "\" or "\\" or "..") use it
    if (name[0] == '\\' || name[1] == ':' || (name[0] == '.' && name[1] == '.'))
    {
        strncpyz(buf, name, buflen);
        return buf;
    }

  // If root path is ".", substitute SavePath or RunPath
    if (name[0] == '.')
    {
        name++;
        while (name[0] == '\\')
            name++;
    }

  // Always try SavePath first (SavePath will always be writable directory on hard drive)
    strncpyz(buf, SavePath, buflen);
    strncatz(buf, name, buflen);

    return buf;
}

FILE *popen(char *name, char *flags)
{
    char fn[MAXPATHLEN];
    
  // If root path expicitly given, (i.e. "c:\", or "\" or "\\" or "..") use it
    if (name[0] == '\\' || name[1] == ':' || (name[0] == '.' && name[1] == '.'))
        return fopen(name, flags);

  // If root path is ".", substitute SavePath or RunPath
    if (name[0] == '.')
    {
        name++;
        while (name[0] == '\\')
            name++;
    }

  // Always try SavePath first (SavePath will always be writable directory on hard drive)
    strncpyz(fn, SavePath, MAXPATHLEN);
    strncatz(fn, name, MAXPATHLEN);

  // If SavePath fails, and we have a different run path, try it 
  // (RunPath can be read only CD-ROM)
    FILE *fp = fopen(fn, flags);
    if (!fp && stricmp(SavePath, RunPath) != 0)
    {
        strncpyz(fn, RunPath, MAXPATHLEN);
        strncatz(fn, name, MAXPATHLEN);
        fp = fopen(fn, flags);
    }

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
char *listget(char *src, int32_t num, char *dst, int32_t len)
{
    if (!dst)
    {
        dst = listbuf;
        len = LISTBUFLEN;
    }

    if (len <= 0)
        return dst;

    dst[0] = nullptr;

    char *p = src;
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
    *d = nullptr;

    return dst;
}

// Get total number of strings in comma list
int32_t listnum(char *src)
{
    char *p = src;
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
char *listrnd(char *src, char *dst, int32_t len)
{
    return listget(src, random(0, listnum(src) - 1), dst, len); 
}

// Returns true if string is in comma list (case insensitive)
bool listin(char *src, char *in)
{
    char *s = src;
    char *i = in;

    while (*s)
    {
        if (*s == ',')
        {
            if (*i == nullptr)
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

    return *i == nullptr;
}

// ********* Free Memory Functions **********

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
