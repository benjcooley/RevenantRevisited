// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  RevMain.cpp - System main module                     *
// *************************************************************************

#include <stdarg.h>

// For multimonitor support
#define COMPILE_MULTIMON_STUBS
#include <algorithm>
#include <cassert>
#include <cctype>
#include <fcntl.h>
#include <math.h>
#include <sstream>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <time.h>

#include "argh.h"

#include "revenant.h"
#include "assetcache.h"
#include "logging.h"
#include "fonttable.h"
#include "3dscene.h"
#include "bitmap.h"
#include "display.h"
#include "graphics.h"
#include "module.h"
#include "playscreen.h"
#include "revisited_defaults.h"
#include "revisited_settings.h"
#include "testscreen.h"
#include "testmodes.h"
#include "testconfig.h"
#include "time.h"
#include "mappane.h"
#include "object.h"
#include "automap.h"
#include "inventory.h"
#include "mainwnd.h"
#include "savegame.h"
#include "screen.h"
#include "font.h"
#include "chunkcache.h"
#include "timer.h"
#include "equip.h"
#include "parse.h"
#include "mapmanager.h"
#include "player.h"
#include "multictrl.h"
#include "spell.h"
#include "spellpane.h"
#include "statpane.h"
#include "script.h"
#include "textbar.h"
#include "statusbar.h"
#include "sound.h"
#include "editorstub.h"
#include "dls.h"
#include "ctrlmap.h"
#include "area.h"
#include "rules.h"
#include "dialog.h"
#include "debugui.h"
#include "cursor.h"
#include "platform/cursor.h"

#include <sokol_app.h>

// TODO(port): Subsystem 7 — MSVC CRT debug heap (crtdbg.h) not available on
// Clang/macOS. Stub the macros so assertions compile. Real validation should
// come from AddressSanitizer and a portable heap checker.
#define _CrtCheckMemory() (1)
#define _CRT_ERROR 0
#define _RPT0(mode, msg) ((void)0)
#define _RPT1(mode, msg, a) ((void)0)
#define _RPT2(mode, msg, a, b) ((void)0)

// Global Variables
// TODO(port): hInstance / StartMemory / PauseMutex were Win32-only.
// PauseMutex was for the loader-thread pause; Subsystem 4 will replace it
// with a worker-pool pause primitive. StartMemory was a GlobalMemoryStatus
// snapshot at WinMain entry, used only in debug prints. Dropped here.
HANDLE PauseMutex = nullptr;

// Game directories
char RunPath[MAXPATHLEN];
char SavePath[MAXPATHLEN];

// Define the Editor paths
char ClassDefPath[MAXPATHLEN];               // Where to load / save Class.Def
char ExileRCPath[MAXPATHLEN];                // Where to run ExileRC from & where
                                             // the graphics for the resources are
char ResourcePath[MAXPATHLEN];               // Where to read / write the resources
char BaseMapPath[MAXPATHLEN];                // Where the untouched version of the game map is stored
char CurMapPath[MAXPATHLEN];                 // Where the current map is stored

// Current language
TString Language;                   // Where the current map is stored

// Active and next game screens
TScreen*        CurrentScreen;      // Currently displayed screen object
TScreen*        NextScreen;         // Next Screen to be display object

// Loaded game assets
TFont*          SystemFont;         // Basic utility font for game
TFont*          DialogFont;         // Dialog font
TFont*          DialogFontShadow;   // Dialog font shadow
TFont*          SmallFont;          // Small game font
TFont*          GameFont;           // Medium game font
TFont*          GoldFont;           // Medium gold font
TFont*          MetalFont;          // Small gold/metal font
TFont*          MenuFont;           // Menu font
TPlayer*        Player;             // Main player for the game
TMulti*         GameData;           // Multiresource for in game data

// Global singleton objects
TDisplay        Display;            // Display object (TODO: see revenant.h note)
T3DScene        Scene3D;            // Display pointer
TPlayScreen     PlayScreen;         // PlayScreen Object
TMapPane        MapPane;            // Map pane for PlayScreen
TInventory      Inventory;          // Inventory of objects
THealthBar      HealthBar;          // Character's health
TStaminaBar     StaminaBar;         // Character's fatigue
TTextBar        TextBar;            // Info line for player
TMultiCtrlPane  MultiCtrl;          // Multipane control panel (buttons)
TEquipPane      EquipPane;          // Equipment pane
TAutoMap        AutoMap;            // Automapper
TSpellPane      SpellPane;          // Talisman stuff
TQuickSpellPane QuickSpells;        // Quick spell buttons
TStatPane       StatPane;           // Display of char's abilities
TScriptManager  ScriptManager;      // Manages all scripts in the game
TMainWindow     MainWindow;         // Windows Object
TSaveGame       SaveGame;           // SaveGame Object
TChunkCache     ChunkCache;         // Tile Cache
TTimer          Timer;              // Timer Object
#if 0 // TODO(port): Subsystem 3 — video capture (DirectShow → AVFoundation/sokol)
TVideoCapture   VideoCapture;       // Video capture object
#endif
TSoundPlayer    SoundPlayer;        // Sound effects player
TControlMap     ControlMap;         // Contains the key/joystick mappings for game control
TAreaManager    AreaManager;        // Manages the game area system
TPlayerManager  PlayerManager;      // Manages the game player list
TAssetCache     AssetCache;         // Shared engine CPU/source asset cache
TMapManager     MapManager;         // Cache of loaded TGameMap levels +
                                    // current-map pointer; renderer / pane
                                    // listen for CurrentMapChanged.
TRules  Rules;                      // Manages global rules data (classes, chars, stats for attacks, etc.)
// TODO(port): CRITICAL_SECTION replaced by Subsystem 4 threading work
// (worker pool + async completions). Removed here; revutils.cpp still
// references it via the stub in BEGIN_CRITICAL / END_CRITICAL below.
TSpellList      SpellList;          // a list of spells in the game
TDialogList     DialogList;         // List of dialog and other game messages for current language

// Game speed variable
int32_t GameSpeed = 3;              // Value 1-5 which determines how fast the game is running
                                    // Used to switch on/off processor intensive effects
// Violence Level
int32_t ViolenceLevel = 5;          // Value 0-5 which determines how much blood and gore, etc. to use
                                    // 0 = none

// Total size of preload area (in sectors) if PreloadSectors is on
int32_t PreloadSectorSize = -1;

// Total size of chunk cache to allocate (in MB's)
int32_t ChunkCacheSize = -1;    // -1 is calculate based on available physical memory

// Clear before drawing update buffers?
bool ClearBeforeDraw = true;        // Clear before drawing update buffers?
bool NoFrameSkip     = false;       // Disables frame skipping on slowdown
bool PauseFrameSkip  = false;       // Set during disk loading to avoid skipping afterwards
bool NoScrollZBuffer = true;        // Disables updating to video memory ZBuffer
bool WaitVertRetrace = true;        // Wait vert retrace (not used)
bool ShowDrawing     = false;       // Flips front and back buffer so you can see drawing
bool NoNormals       = true;        // Changes to no normals lighting mode
bool SmoothScroll    = true;        // Do smooth scrolling
bool ScrollLock      = true;        // Lock scrolling to the character
bool DrawRealtimeShadows = true;    // Draw alpha shadows for 3D characters
bool Show3D          = true;        // Turn on the 3D system
bool Interpolate     = true;        // Causes 3D animations to interpolate when state changes
bool DoPageFlip      = true;        // Allows the PageFlip function to flip pages
bool PauseWhenNotActive = true;     // Causes game to pause when not active
bool AppActive       = true;        // Flag for if the game is the active application
                                    // (starts true — sokol_app doesn't always deliver FOCUSED
                                    // at launch on macOS, and the window is already active)
bool NoWideBuffers   = false;       // Doesn't allow video buffers with stides wider than their widths
bool UseDirect3D2    = true;        // True if you want to be able to use drawprimive stuff
bool UseSoftware3D   = false;       // Use software 3D (False assumes we want hardware if available)
bool SoftRampMode    = true;        // Use RAMP mode for software 3D (default)
bool SoftRGBMode     = false;       // Use RGB mode for software 3D
bool UseBlue         = true;        // Use BLUE software renderer instead of DirectX
bool UseDrawPrimitive = true;       // If false, 3D system does everything with Execute Buffers (always false if UseDirect3D2 is false)
bool CacheExBufs     = true;        // Attempt to cache execute buffers for chars and other animation
bool PreloadSectors  = true;        // If true, will load a large sector cache for faster scrolling
bool NoPulseObjs     = false;       // Prevents the game objects from being pulsed
bool NoAnimateObjs   = false;       // Prevents the game objects from being animated
bool NoVidZBufLock   = false;       // Prevents simultaneous locking of video and zbuffer (for voodoo)
bool NoBlitZBuffer   = false;       // Card is incapable of blitting to the display ZBuffer (voodoo)
bool UseClearZBuffer = false;       // Use a clear z buffer for z restores (i.e. Viewport->Clear())
bool IsVooDoo        = false;       // True if using a voodoo card
bool IsMMX           = false;       // Has MMX extensions
bool NoAI            = false;       // Turns off monster AI
bool AutoBeginCombat = true;        // Automatically begins combat if enemy in range and facing him
bool PlaySpeech      = true;        // Play speech wave files
bool ShowDialog      = false;       // Show dialog lines (always shows if no speech file found)

// Multi-Monitor Variables
int32_t MonitorNum = 0;         // Monitor game will run on (0=default (primary), 1=monitor 1, 2=monitor 2, etc.)
int32_t MonitorX = 0;           // Relative position of monitor in desktop coordinates
int32_t MonitorY = 0;
int32_t MonitorW = 640;         // Relative position of monitor in desktop coordinates
int32_t MonitorH = 480;
// TODO(port): HMONITOR / MONITORINFOEX were Win32 state populated by the
// retired MonitorEnumProc. sokol_app doesn't expose a monitor index; the
// engine will need reworked multi-display support or we drop the feature.

// Game flags
bool Windowed = false;      // Do we run the game in a window in NORMAL mode (instead of EXLUSIVE)
bool SingleBuffer = false;  // True if single buffering (true if Windowed=true, or UsingHardware=false)
bool Borderless = false;    // When windowed, cut out the border (menu, other windows garabage)?
bool Debug = false;         // Should program be run in debug compatible mode?
bool Ignore3D = false;      // Whether to disallow 3D imagery (to make it run on systmes without Direct3D)
// `Editor` global moved to src/editor.cpp; revmain just references it.
extern bool Editor;
bool StartInEditor = false; // Whether to start the program in editor mode
bool RevisitedEnabled = false; // --revisited: mount Revisited overlay (FATAL if requested but missing)
bool NoQuickLoad = false;   // true if program should not try to use IMAGERY.DAT file
bool Force15Bit = false;    // Forces video mode to assume 15 bit
bool Force16Bit = false;    // Forces video mode to assume 16 bit
bool FullScreen = false;    // Full screen mode
bool ShowZBuffer      = false;  // Show Z buffer for debugging
bool ShowNormalBuffer = false;  // Show normal buffer for debugging
// UnlockImmediately = true was a DirectDraw-era debug aid (let you
// step through the CPU rasterizer with the surface unlocked). Under
// the sokol GPU path, Unlock() is where the CPU staging buffer is
// uploaded to the GPU texture -- unlocking BEFORE the rasterizer writes
// uploads the (zeroed) buffer and the actual pixels never make it to
// the GPU. Every Display.Put / Display.Box silently no-ops.
// Default must be false on the new path.
bool UnlockImmediately = false;

// Video capture globals (set in command line parse, used after display is initialized)
static bool dovideocap = false;
static int32_t videocapmegs, videocapfps;

// Driver name string (allows user to select driver by just typing in name of card)
char DXDriverMatchStr[FILENAMELEN]; // Will use first DX driver who's description has the given string in it
                                    // i.e. use if string is "permidia" and driver desc is "Glint Permidia 2 3D"

// Optional save-game path requested via `--loadmap=...` on the command line.
// When non-empty, AppInit hands it to PlayScreen.LoadGameFile() so the engine
// restores a live session on first Pulse().
char StartupSavePath[MAXPATHLEN] = "";

// Window/backbuffer size selected before sokol creates the native window.
// WIDTH/HEIGHT stay as the classic 640x480 layout baseline; the renderer and
// display surfaces use these runtime values.
int32_t GameScreenWidth = WIDTH;
int32_t GameScreenHeight = HEIGHT;

// Tick Sync variable
bool TickOccured = false;

// Disable Screen calls to timer functions
bool DisableTimer = false;

// Last Frame Milliseconds (Do we show it?)
int32_t LastFrameTicks;
bool ShowFramesPerSecond = false;

// Character movement mode
bool GridSnap = false;

// Music and sound effects
bool SoundSystemOn = true;

// Is Control, Shift, or Alt down
bool CtrlDown, ShiftDown, AltDown;

// Is System Closing Down?
bool Closing = false;

// Set by SAPP_EVENTTYPE_MOUSE_MOVE; consumed once per frame in AppFrame. A
// single frame can see dozens of move events on macOS — forwarding every one
// synchronously to CurrentScreen->MouseMove() backs up the NSEvent queue and
// the cursor appears to lag seconds behind reality. Coalesce to one call per
// frame with the latest cursor position.
static bool g_pendingMouseMove = false;

// For setting up 3D object bounding rects in the editor
bool UpdatingBoundingRect = false;

// Is joystick available?
bool HasJoyStick = false;
// TODO(port): JOYINFO was Win32 joyGetPos result; Subsystem 7 (input)
// will rework joystick support via sokol_app/IOKit.

// Memory used by program
uint32_t ImageryMemUsage = 0;

// Frame ticks for doubletap checking
int32_t DoubleTapTicks = 6;

// Global Map Pane Sizes (Set to initial frame sizes to insure
int32_t MAPPANEX, MAPPANEY, MAPPANEWIDTH, MAPPANEHEIGHT;
int32_t SCROLLBUFWIDTH, SCROLLBUFHEIGHT;

// ************************************************************************
// *                          Support Functions                           *
// ************************************************************************
//
// ATTIC: every utility below (thread pause gate, itos/stricmp, file copy,
// tickcount, FatalError/Error/Status, INI access, xmalloc family, random,
// list helpers, Mem*/*Phys/*Page) now lives in revutils.cpp. The 1998
// bodies are kept under `#if 0` so a reviewer can diff the old Win32 paths
// against the macOS-oriented rewrites. Delete this block once the port
// stabilizes.

#if 0 // ATTIC: duplicated in revutils.cpp
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
        snprintf(buf, buflen, "%d", val);
        return buf;
    }
    char b[12];
    snprintf(b, sizeof(b), "%d", val);
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

#if 0 // TODO(port): Subsystem 6 — filesystem enumeration (→ POSIX opendir/readdir)
      // Duplicate of the canonical implementations in revutils.cpp — kept
      // disabled here until the enumeration port lands, then this dead copy
      // will be deleted outright.
// Copies a file, or group of files using wildcards
int32_t copyfiles(const char *from, const char *to, bool overwrite)
{
    if (!from || !to)
        return 0;

    struct _finddata_t data;

    char fdrive[_MAX_DRIVE];
    char fdir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char fext[_MAX_EXT];

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
#endif

#if 0 // TODO(port): Subsystem 1 — duplicate of revutils.cpp flen()/tickcount(); retired here
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
#endif

// *************** Error Functions *****************

void ThreadError(char *error, char *extra)
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
        fprintf(stderr, error, extra);
    else
        fprintf(stderr, "%s", error);
    fprintf(stderr, "\n");
#endif
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

  // FatalError aborts the process; the OS reclaims everything. Skipping
  // the partial teardown dance keeps this path from racing the live
  // ShutdownGlobals path and matches the "InitGlobals/ShutdownGlobals
  // are the canonical callers" rule for file-scope globals.
    if (extra)
        fprintf(stderr, error, extra);
    else
        fprintf(stderr, "%s", error);
    fprintf(stderr, "\n");

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

// Duplicate of revutils.cpp definitions. Agent A rewrites revmain.cpp
// around sokol_app; this block will be replaced wholesale there.
#if 0
void WaitSingleErr(HANDLE obj) {}
void WaitMultipleErr(uint32_t objs, const HANDLE *obj, bool all) {}
#endif

// *************** Display Status Functions *****************

void Status(const char *fmt, ...)
{
#if 0 // TODO(port): Subsystem 2 — DirectDraw/Win32 presentation (→ sokol)
    static char buf[1024]; // Temporary buf for output
    static y = 10;

    if (!DirectDraw || !SystemFont || !Display.GetSurface())
        return;

    va_list marker;
    va_start(marker, fmt);
    vsprintf(buf, fmt, marker);

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
    va_list marker;
    va_start(marker, fmt);
    vfprintf(stderr, fmt, marker);
    va_end(marker);
    fprintf(stderr, "\n");
#endif
}

// *************** Critical Section Functions *****************
// TODO(port): these are stubs; Subsystem 4 (threading) will land proper
// mutex-backed critical sections. Safe as no-ops while we are single-
// threaded during Phase 2 bringup.

void BEGIN_CRITICAL() {}
void END_CRITICAL() {}

// ****** Exit the Game - Why would anyone want to do that? ******

void ExitGame()
{
    sapp_request_quit();
}

// *************** Settings Functions *****************
//
// The INI accessor family (INISetPath/INIGetInt/INIGetBool/...) lives in
// revutils.cpp now — the revutils port rebuilt them on top of CSimpleIniA.
// The Win32 GetPrivateProfile* bodies that used to live here have been
// retired to attic/src/revutils_win32_ini.cpp. Leaving the banner as a
// landmark for anyone grepping.
#if 0  // ATTIC: retired duplicate — see revutils.cpp for the live impl

static TString INIPath;
static TString INISection;

void INISetSection(const char* newsection)
{
    INISection = newsection;
}

void INISetPath(const char *runpath)
{
    char *ininame = "Revenant.ini";

    strcpy(INIPath, SavePath);
    strcat(INIPath, ininame);

  // Make sure INI file is in writable (SavePath) directory
    if (stricmp(RunPath, SavePath) != 0) // Run/Save path are different
    {
        FILE *fp = fopen(INIPath, "r");
        if (!fp)    // INI file not in SavePath
        {
            char from[MAXPATHLEN];
            strcpy(from, RunPath);
            strcat(from, ininame);
            copyfiles(from, INIPath);
        }
    }
}

int32_t INIGetInt(const char *key, int32_t def, char *format)
{
    int32_t i = GetPrivateProfileInt(INISection, key, def, INIPath);

    INISetInt(key, i, format);

    return i;
}

void INISetInt(const char *key, int32_t i, char *format)
{
    if (!format)
        format = "%d";

    char buf[20];
    sprintf(buf, format, i);

    WritePrivateProfileString(INISection, key, buf, INIPath);
}

char *INIGetText(const char *key, char *def, char *buf, int32_t buflen)
{
    static char s[128];
    if (!buf)
    {
        buf = s;
        buflen = 128;
    }

    if (!def)
        def = "";

  // Quote the string
    char qdef[128];
    strncpyz(qdef, "\"", 128);
    strncatz(qdef, def, 128);
    strncatz(qdef, "\"", 128);

    GetPrivateProfileString(INISection, key, qdef, buf, buflen, INIPath);

    if (buf[0] == '\"')
    {
        int32_t l = strlen(buf);
        memmove(buf, buf + 1, l - 2);
        buf[l - 2] = '\0';
    }

    INISetText(key, buf);

    return buf;
}

void INISetText(char *key, char *str)
{
  // Quote the string
    char qstr[128];
    strncpyz(qstr, "\"", 128);
    strncatz(qstr, str, 128);
    strncatz(qstr, "\"", 128);

    WritePrivateProfileString(INISection, key, qstr, INIPath);
}

char *INIGetStr(char *key, char *def, char *buf, int32_t buflen)
{
    static char s[128];
    if (!buf)
    {
        buf = s;
        buflen = 128;
    }

    if (!def)
        def = "";

    GetPrivateProfileString(INISection, key, def, buf, buflen, INIPath);

    INISetStr(key, buf);

    return buf;
}

void INISetStr(char *key, char *str)
{
    WritePrivateProfileString(INISection, key, str, INIPath);
}

int32_t INIGetArray(char *key, int32_t size, int32_t *ary, int32_t defsize, int32_t *defary, char *format)
{
    char buf[128];

    if (ary != defary)
        memset(ary, 0, sizeof(int32_t) * size);

    GetPrivateProfileString(INISection, key, "", buf, 128, INIPath);

    int32_t newsize = 0;
    if (!buf[0])
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
        char *tok = nullptr;
        do
        {
            tok = strtok((newsize < 1) ? buf : nullptr, ",");
            if (tok)
            {
                ary[newsize] = atol(tok);
                newsize++;
            }
        } while (tok && newsize < size);
    }   

    INISetArray(key, newsize, ary, format);

    return newsize;
}

void INISetArray(char *key, int32_t size, int32_t ary[], char *format)
{
    char buf[128];

    if (!format)
        format = "%d";

    buf[0] = '\0';
    for (int32_t c = 0; c < size; c++)
    {
        if (c >= 1)
            strcat(buf, ",");
        int32_t len = strlen(buf);
        sprintf(buf + len, format, ary[c]);
    }

    WritePrivateProfileString(INISection, key, buf, INIPath);
}

bool INIGetBool(char *key, bool def, char *yes, char *no)
{
    char buf[30];
    char getyes[30], getno[30];

    if (!yes)
        strcpy(getyes,"yes on true 1");
    else
    {
        strcpy(getyes, yes);
        strlwr(getyes);
    }

    if (!no)
        strcpy(getno, "no off false 0");
    else
    {
        strcpy(getno, no);
        strlwr(getno);
    }

    GetPrivateProfileString(INISection, key, "", buf, 30, INIPath);
    strlwr(buf);

    bool b; 
    if (!strstr(yes, buf))
        b = true;
    else if (!strstr(no, buf))
        b = false;
    else
        b = def;

    INISetBool(key, b, yes, no);

    return b;
}

void INISetBool(char *key, bool on, char *yes, char *no)
{
    if (!yes)
        yes = "1";
    if (!no)
        no = "0";

    WritePrivateProfileString(INISection, key, on ? yes : no, INIPath);
}

bool INIGetYesNo(const char *key, bool def)
{
    return INIGetBool(key, def, "Yes", "No");
}

void INISetYesNo(const char *key, bool on)
{
    INISetBool(key, on, "Yes", "No");
}

bool INIGetTrueFalse(const char *key, bool def)
{
    return INIGetBool(key, def, "True", "False");
}

void INISetTrueFalse(const char *key, bool on)
{
    INISetBool(key, on, "True", "False");
}

bool INIGetOnOff(const char *key, bool def)
{
    return INIGetBool(key, def, "On", "Off");
}

void INISetOnOff(const char *key, bool on)
{
    INISetBool(key, on, "On", "Off");
}

// Grab the ParseAnything function from PARSE.CPP
bool ParseAnything(bool stack, TToken &t, const char *format, va_list ap);

bool INIParse(const char *key, cons char *def, const char *format, ...)
{
    char buf[128];

    INIGetStr(key, def, buf, 128);

    va_list ap;
    va_start(ap, format);

    TStringParseStream s(buf, strlen(buf));
    TToken t(s);
    t.Get();

    bool retval = ParseAnything(true, t, format, ap);

    va_end(ap);

    return retval;
}

void INIPrint(const char *key, const char *format, ...)
{
    char buf[128];

    va_list marker;
    va_start(marker, format);

    vsprintf(buf, format, marker);

    va_end(marker);

    INISetStr(key, buf);
}

#endif // ATTIC: retired duplicate INI block

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
#if 0 // TODO(port): Subsystem 7 — debug heap instrumentation (MSVC CRT → AddressSanitizer)
    TotalAllocated = TotalAllocated - _msize(p) + size;
    if (TotalAllocated > MaxAllocated)
        MaxAllocated = TotalAllocated;
#endif
    p = realloc(p, size);
    return p;
}

void xfree(void *p)
{
#if 0 // TODO(port): Subsystem 7 — debug heap instrumentation (MSVC CRT → AddressSanitizer)
    if (p != nullptr)
        TotalAllocated -= _msize(p);
#endif
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

// makepath() lives in src/revutils.cpp — single definition keeps the
// SavePath-relative resolution + separator normalization in one place.
// (The duplicate previously here was an ODR violation; the linker silently
// picked one of the two identical bodies.)

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

    dst[0] = '\0';

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
    *d = '\0';

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

#if 0 // TODO(port): Subsystem 7 — Win32 GlobalMemoryStatus → mach/sysctl equivalents
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
#endif // ATTIC: duplicated in revutils.cpp

// ************************************************************************
// *                     Initialization Functions                         *
// ************************************************************************

// TODO(port): MMX paths are not shipped and scheduled for deletion; see
// feedback_mmx_unused. The original CPUID-based HasMMX() is in
// attic/src/revmain_win32_entry.cpp. Hard-coded false for now.
bool HasMMX()
{
    return false;
}

void GetProgramPaths(char *lpCmdLine, char *RunPath, char *SavePath)
{
#if 0 // TODO(port): Subsystem 8 — Win32 path discovery (GetModuleFileName/unlink/mkdir/strlwr)
  char *p;
  // Get The Run Path
    GetModuleFileName(hInstance, RunPath, RUNPATHLEN - 1);
    p = RunPath + strlen(RunPath) - 1;
    while (p != RunPath && *p != '\\')
        *p-- = nullptr;

  // Get save path
    strcpy(SavePath, RunPath);
    char *tfname = "test.fil";
    char fname[60];
    strcpy(fname, SavePath);
    strcat(fname, tfname);
    FILE *f = fopen(fname, "wb");
    if (f)
    {
        fclose(f);
        unlink(fname);
    }
    else
    {
        strcpy(SavePath, "c:\\Revenant");
        for (int32_t qq = 0; qq < 5; qq++)
        {
            if (mkdir(SavePath) == 0)
                break;
            strcpy(fname, SavePath);
            strcat(fname, "\\");
            strcat(fname, tfname);
            f = fopen(fname, "wb");
            if (f)
            {
                fclose(f);
                unlink(fname);
                break;
            }
            SavePath[0]++;
        }
        strcat(SavePath, "\\");
    }

    strlwr(SavePath);
    strlwr(RunPath);
#else
    // Posix port. Mirrors retail's intent (RunPath = exe dir,
    // SavePath = writable user-data dir; both end with '/'). Retail
    // probed test.fil in RunPath and fell back to c:\Revenant when the
    // install was on a CD; on macOS/Linux we go straight to a per-user
    // Application Support / XDG_DATA_HOME dir which is always writable.
    (void)lpCmdLine;
    rev_resolve_program_paths(RunPath, SavePath, RUNPATHLEN);
#endif
}

// Command-line parsing uses argh (thirdparty/argh, single-header). Flag
// names are case-insensitive at the engine level — argh itself is
// case-sensitive, so we register every flag twice (lower + upper) to keep
// legacy `WINDOWED` style and modern `--windowed` / `--loadmap` working.
// Values preserve their original case (paths, driver names).

namespace {

// Lowercase a std::string in place (ASCII only).
std::string to_lower(std::string s)
{
    for (auto &c : s) c = (char)std::tolower((unsigned char)c);
    return s;
}

// True if the given parser saw any spelling of `name` as a boolean flag.
// Accepts e.g. "windowed" matching `--windowed`, `-windowed`, `/windowed`,
// `WINDOWED` — argh strips the leading dashes, we normalize case.
bool arg_flag(const argh::parser &cmd, const char *name)
{
    const std::string n = to_lower(name);
    for (const auto &f : cmd.flags())
        if (to_lower(f) == n)
            return true;
    return false;
}

// If `name` was given as `--name=value` or `--name value`, copy the
// value into `out`. Returns true on hit. Value case is preserved.
bool arg_param(const argh::parser &cmd, const char *name, std::string &out)
{
    const std::string n = to_lower(name);
    for (const auto &p : cmd.params())
    {
        if (to_lower(p.first) == n)
        {
            out = p.second;
            return true;
        }
    }
    return false;
}

template <typename T>
bool arg_param_to(const argh::parser &cmd, const char *name, T &out)
{
    std::string v;
    if (!arg_param(cmd, name, v)) return false;
    std::istringstream iss(v);
    iss >> out;
    return !iss.fail();
}

struct SSupportedResolution
{
    int32_t width;
    int32_t height;
};

constexpr SSupportedResolution kSupportedResolutions[] =
{
    { 640,  480  },
    { 800,  600  },
    { 960,  540  },
    { 960,  720  },
    { 1024, 768  },
    { 1280, 720  },
    { 1280, 800  },
    { 1280, 860  },
    { 1280, 960  },
    { 1280, 1024 },
    { 1366, 768  },
    { 1440, 900  },
    { 1600, 900  },
    { 1920, 1080 },
};

constexpr const char* kSupportedResolutionText =
    "640x480, 800x600, 960x540, 960x720, 1024x768, "
    "1280x720, 1280x800, 1280x860, 1280x960, 1280x1024, "
    "1366x768, 1440x900, 1600x900, 1920x1080";

bool IsSupportedResolution(int32_t width, int32_t height)
{
    for (const SSupportedResolution& r : kSupportedResolutions)
        if (r.width == width && r.height == height)
            return true;
    return false;
}

bool ParsePositiveInt(const char* begin, const char* end, int32_t& out)
{
    while (begin < end && std::isspace((unsigned char)*begin)) ++begin;
    while (end > begin && std::isspace((unsigned char)*(end - 1))) --end;
    if (begin >= end)
        return false;

    char* parse_end = nullptr;
    const long value = std::strtol(begin, &parse_end, 10);
    if (parse_end != end || value <= 0 || value > 32767)
        return false;

    out = int32_t(value);
    return true;
}

bool ParseResolutionValue(const std::string& value, int32_t& width, int32_t& height)
{
    const size_t sep = value.find_first_of("xX,");
    if (sep == std::string::npos)
        return false;

    const char* text = value.c_str();
    return ParsePositiveInt(text, text + sep, width) &&
           ParsePositiveInt(text + sep + 1, text + value.size(), height);
}

bool ParseResolutionPair(const char* width_text, const char* height_text,
                         int32_t& width, int32_t& height)
{
    if (!width_text || !height_text)
        return false;
    return ParsePositiveInt(width_text, width_text + strlen(width_text), width) &&
           ParsePositiveInt(height_text, height_text + strlen(height_text), height);
}

std::string StripArgPrefix(const char* arg)
{
    std::string s = arg ? arg : "";
    while (!s.empty() && (s[0] == '-' || s[0] == '/'))
        s.erase(0, 1);
    return s;
}

enum class EResolutionArgResult
{
    Missing,
    Valid,
    Invalid,
};

EResolutionArgResult ParseResolutionArg(int argc, char** argv,
                                        int32_t& width, int32_t& height,
                                        std::string& raw_value)
{
    for (int i = 1; i < argc; ++i)
    {
        std::string opt = StripArgPrefix(argv[i]);
        const size_t eq = opt.find('=');
        const std::string key = to_lower(opt.substr(0, eq));
        if (key != "resolution" && key != "res")
            continue;

        bool parsed = false;
        if (eq != std::string::npos)
        {
            raw_value = opt.substr(eq + 1);
            parsed = ParseResolutionValue(raw_value, width, height);
        }
        else if (i + 1 < argc)
        {
            raw_value = argv[i + 1];
            parsed = ParseResolutionValue(raw_value, width, height);
            if (!parsed && i + 2 < argc)
            {
                raw_value = std::string(argv[i + 1]) + " " + argv[i + 2];
                parsed = ParseResolutionPair(argv[i + 1], argv[i + 2], width, height);
            }
        }

        return parsed ? EResolutionArgResult::Valid : EResolutionArgResult::Invalid;
    }

    return EResolutionArgResult::Missing;
}

void ApplyCommandLineResolution(int argc, char** argv)
{
    int32_t width = WIDTH;
    int32_t height = HEIGHT;
    std::string raw;
    const EResolutionArgResult result = ParseResolutionArg(argc, argv, width, height, raw);
    if (result == EResolutionArgResult::Missing)
        return;

    if (result == EResolutionArgResult::Invalid || !IsSupportedResolution(width, height))
    {
        log_warn("[video] ignoring unsupported --resolution '%s'; supported: %s",
                 raw.empty() ? "(missing)" : raw.c_str(),
                 kSupportedResolutionText);
        return;
    }

    GameScreenWidth = width;
    GameScreenHeight = height;
    log_info("[video] resolution set to %dx%d", GameScreenWidth, GameScreenHeight);
}

} // namespace

void GetParameters(int argc, char **argv)
{
    argh::parser cmd;
    // Flags that consume a value (so argh doesn't treat their value as
    // a positional). Everything else is a boolean flag.
    cmd.add_params({
        "gamespeed", "monitor", "violencelevel", "preloadsize",
        "chunkcachesize", "driver", "device", "videocap", "fastlock",
        "loadmap", "lang", "test", "level", "resolution", "res",
    });
    cmd.parse(argc, argv);

  // Game speed first — performance knobs below depend on it
    if (!arg_param_to(cmd, "gamespeed", GameSpeed)) {} // leaves prior value
    if (GameSpeed < 1) GameSpeed = 1;
    if (GameSpeed > 5) GameSpeed = 5;

    if (GameSpeed <= 1) { SmoothScroll = false; MaxLights = 1; }
    if (GameSpeed < 3)  { UseDirLight = false; }

  // Boolean switches
    if (arg_flag(cmd, "debug"))            Debug = true;
    if (arg_flag(cmd, "force15bit"))       Force15Bit = true;
    if (arg_flag(cmd, "force16bit"))       Force16Bit = true;
    if (arg_flag(cmd, "editor"))           StartInEditor = true;
    if (arg_flag(cmd, "revisited"))        RevisitedEnabled = true;
    if (arg_flag(cmd, "windowed"))         Windowed = true;
    if (arg_flag(cmd, "noquickload"))      NoQuickLoad = true;
    if (arg_flag(cmd, "borderless"))       Borderless = true;
    if (arg_flag(cmd, "nowide"))           NoWideBuffers = true;
    if (arg_flag(cmd, "fullscreen"))       FullScreen = true;
    if (arg_flag(cmd, "nopreloadsectors")) { PreloadSectors = false; PreloadSectorSize = 3; }
    if (arg_flag(cmd, "ignore3d"))         Ignore3D = true;
    if (arg_flag(cmd, "olddirect3d"))      { UseDirect3D2 = false; UseDrawPrimitive = false; }
    if (arg_flag(cmd, "software3d"))       UseSoftware3D = true;
    if (arg_flag(cmd, "rampmode"))         { SoftRampMode = true;  SoftRGBMode = false; UseSoftware3D = true; }
    if (arg_flag(cmd, "rgbmode"))          { SoftRampMode = false; SoftRGBMode = true;  UseSoftware3D = true; }
    if (arg_flag(cmd, "blue"))             { UseBlue = true; SoftRampMode = true; SoftRGBMode = false; UseSoftware3D = true; }
    if (arg_flag(cmd, "nodrawprim"))       UseDrawPrimitive = false;
    if (arg_flag(cmd, "nocacheexbufs"))    CacheExBufs = false;
    if (arg_flag(cmd, "novidzlock"))       NoVidZBufLock = true;
    if (arg_flag(cmd, "clearz"))           { UseClearZBuffer = true; NoVidZBufLock = false; }
    if (arg_flag(cmd, "voodoo"))           { IsVooDoo = true; NoVidZBufLock = true; NoBlitZBuffer = true; UseClearZBuffer = true; }
    if (arg_flag(cmd, "nommx"))            IsMMX = false;

  // Value-taking knobs. Use parenthesized std::min/clamp idiom so legacy
  // min/max macros from revtypes.h don't hijack them.
    arg_param_to(cmd, "monitor", MonitorNum);
    if (int v; arg_param_to(cmd, "violencelevel", v))
    { ViolenceLevel = (std::clamp)(v, 0, 5); }
    if (int v; arg_param_to(cmd, "preloadsize", v))
    {
        PreloadSectorSize = (v > 8) ? 8 : v;
        if (PreloadSectorSize < 3) { PreloadSectors = false; PreloadSectorSize = 3; }
    }
    if (int v; arg_param_to(cmd, "chunkcachesize", v))
    { ChunkCacheSize = (v > 256) ? 256 : v; }

  // Driver/device: first match wins, value case preserved
    {
        std::string d;
        if (!arg_param(cmd, "driver", d)) arg_param(cmd, "device", d);
        if (!d.empty()) { strncpyz(DXDriverMatchStr, d.c_str(), FILENAMELEN); }
    }

  // VIDEOCAP=megs,fps
    {
        std::string vc;
        if (arg_param(cmd, "videocap", vc))
        {
            dovideocap = true;
            videocapmegs = FRAMERATE;
            videocapfps  = FRAMERATE;
            const size_t comma = vc.find(',');
            videocapmegs = (std::clamp)(atoi(vc.substr(0, comma).c_str()), 4, 128);
            videocapfps  = (comma == std::string::npos)
                           ? FRAMERATE
                           : (std::clamp)(atoi(vc.substr(comma + 1).c_str()), 8, (int)FRAMERATE);
        }
        else
            dovideocap = false;
    }

  // FASTLOCK=on|off|1|0
    {
        std::string v;
        if (arg_param(cmd, "fastlock", v))
        {
            const std::string lv = to_lower(v);
            UnlockImmediately = (lv == "on" || lv == "1" || lv == "true");
        }
    }

  // LOADMAP=<save-file> — jump straight into a loaded save at boot,
  // skipping any menu screens. Path is resolved via rev_fopen (SavePath,
  // RunPath, module dir, then VFS archives).
    {
        std::string p;
        if (arg_param(cmd, "loadmap", p))
            strncpyz(StartupSavePath, p.c_str(), MAXPATHLEN);
    }

  // TEST=<mode> — route to TTestScreen instead of LogoScreen/PlayScreen.
  // See recon/docs/RETAIL_SYNC_PLAN.md.
    {
        std::string p;
        if (arg_param(cmd, "test", p))
            strncpyz(StartupTestMode, p.c_str(), sizeof(StartupTestMode));
    }

  // VFX=<effect_id> — pre-select an effect in --test=vfx (e.g.
  // --vfx=TStripEffect). Empty = default (first alphabetically).
    {
        std::string p;
        if (arg_param(cmd, "vfx", p))
            strncpyz(StartupVfxId, p.c_str(), sizeof(StartupVfxId));
    }

  // VFX-NO-UI — suppress the ImGui VFX Browser panel (for clean
  // effect-only screencaps). Flag-style: --vfx-no-ui (no value).
    if (arg_flag(cmd, "vfx-no-ui"))
        StartupVfxHideUi = true;

  // SECTOR=L_X_Y — pick which sector --test=sector keeps alive and renders.
  // Empty = the default hard-coded pick (0_2_25, Misthaven).
    {
        std::string p;
        if (arg_param(cmd, "sector", p))
            strncpyz(StartupSectorId, p.c_str(), sizeof(StartupSectorId));
    }

  // LEVEL=L — load the whole requested level in --test=sector and anchor the
  // initial camera/origin at world 0,0 instead of assuming sector 0,0 exists.
    {
        std::string p;
        if (arg_param(cmd, "level", p))
            strncpyz(StartupLevelId, p.c_str(), sizeof(StartupLevelId));
    }

  // ASSET=path — which imagery --test=i3d3d loads.
    {
        std::string p;
        if (arg_param(cmd, "asset", p))
            strncpyz(StartupAssetPath, p.c_str(), sizeof(StartupAssetPath));
    }

  // SCALE=f — uniform mesh scale multiplier for --test=i3d3d (0 = auto-fit).
    {
        std::string p;
        if (arg_param(cmd, "scale", p))
            StartupAssetScale = float(atof(p.c_str()));
    }

  // DUMPTILES=path — export tile albedo PNGs to the given folder, creating it if needed.
    {
        std::string p;
        if (arg_param(cmd, "dumptiles", p))
            strncpyz(StartupDumpTilesPath, p.c_str(), MAXPATHLEN);
    }
}

void GetINISettings()
{
  // ***** Get Program Paths *****

    INISetSection("Paths");
    INIGetText("ClassDefPath", ".", ClassDefPath, MAXPATHLEN);
    INIGetText("ExileRCPath", ".", ExileRCPath, MAXPATHLEN);
    INIGetText("ResourcePath", ".", ResourcePath, MAXPATHLEN);
    INIGetText("CurMapPath", ".", CurMapPath, MAXPATHLEN);
    INIGetText("BaseMapPath", ".", BaseMapPath, MAXPATHLEN);

    // Make sure each string ends with a backslash
    if (ClassDefPath[strlen(ClassDefPath) - 1] != '\\')
        strcat(ClassDefPath, "\\");

    if (ExileRCPath[strlen(ExileRCPath) - 1] != '\\')
        strcat(ExileRCPath, "\\");

    if (ResourcePath[strlen(ResourcePath) - 1] != '\\')
        strcat(ResourcePath, "\\");

    if (CurMapPath[strlen(CurMapPath) - 1] != '\\')
        strcat(CurMapPath, "\\");

    if (BaseMapPath[strlen(BaseMapPath) - 1] != '\\')
        strcat(BaseMapPath, "\\");

    INISetSection("Lighting");
    MaxLights = INIGetInt("MaxLights", 1);
    Ambient3D = INIGetInt("Ambient3D", 100);
    LightRange3D = INIGetInt("LightRange3D", 180);
    LightMult3D = INIGetInt("LightMult3D", 250);
    EnhancedLighting = INIGetYesNo("EnhancedLighting", true);

    INISetSection("Options");
    DoubleTapTicks = INIGetInt("DoubleTapTicks", 6);
}

// This function gets called as soon as the display system finds the right driver.
// You can check the parameters of the driver and set system flags here so that things
// work correctly.
void DriverSetupCallback()
{
#if 0 // TODO(port): Subsystem 2 — DirectDraw driver description inspection (→ sokol backend)
    char buf[DRIVERDESCLEN];
    strncpyz(buf, DirectDrawDesc, DRIVERDESCLEN);
    strlwr(buf);

    // Set special flags for VooDoo cards
    if (strstr(buf, "voodoo"))  // Only voodoo cards would match this!
    {
        NoVidZBufLock = true; // Don't try to lock video and zbuffer at same time
        NoBlitZBuffer = true; // Can't blit to or from the zbuffer
        UseClearZBuffer = true; // Use a secondary clear zbuffer for drawing instead of display zbuffer
    }
#endif
}

bool InitLanguage()
{
  // Set language. The actual DialogList load happens inside InitGlobals
  // alongside the rest of the engine's lifecycle steps; that keeps every
  // singleton's Initialize/Init call in one canonical caller.
    Language = "english";

// Old MAYHEM stuff
#if 0
   // Get Language file
    p = strstr(lpCmdLine, "LANG=");
    if (p)
        Language = atoi(p + 5);
    else
        Language = GetProfileInt("intl", "iCountry", 1);
    char buf[20];
    wsprintf(buf, "LANGUAGE.%03d", Language);
    f = fopen(buf, "rb");
    if (!f)
        Language = ENGLISH;
    else
        fclose(f);

    if (Language == ENGLISH)
        SecondLang = ENGLISH;

    if (strstr(lpCmdLine, "KOR"))
        SecondLang = KOREAN;

    LoadLanguage(Language);
#endif

    return true;
}

// TODO(port): multi-monitor selection. sokol_app puts the window on the
// user's active display; the MONITOR=n command-line selector from the
// Win32 build is not wired up yet. Originals in attic/src/revmain_win32_entry.cpp.
bool InitMonitor()
{
    MonitorX = 0;
    MonitorY = 0;
    MonitorW = GameScreenWidth;
    MonitorH = GameScreenHeight;
    return true;
}

// ************************************************************************
// *                  InitGlobals and ShutdownGlobals                      *
// ************************************************************************
//
// One canonical caller for every file-scope global's lifecycle: each
// singleton's existing API name (Initialize/Close, Init/Shutdown, or
// AllocCache, depending on the class) is invoked from here in a fixed
// construction order. ShutdownGlobals tears them down in EXACT reverse,
// with comments numbered to match step-for-step.

bool InitGlobals()
{
    // (1) MainWindow — sokol_app already owns the actual window by the
    // time AppInit calls us, so this is just the engine-side state-flip.
    if (!MainWindow.Initialize())
        FatalError("Couldn't bring up MainWindow", nullptr);

    // (2) FontTable — parses FONT.DEF. Runs before Display.Initialize()
    // (sokol_gfx setup) because GPU atlas building is deferred to first use.
    FontTable = new TFontTable;
    if (!FontTable->Initialize())
        FatalError("Unable to load FONT.DEF");

    SystemFont       = FontTable->Bitmap("System");
    DialogFont       = FontTable->Bitmap("Dialog");
    DialogFontShadow = FontTable->Bitmap("Dialog");  // WINFONT - shadow handled by TrueType later
    SmallFont        = FontTable->Bitmap("Small");
    GameFont         = FontTable->Bitmap("Med");
    
#if 0 // TODO(port): Subsystem 7 — MSVC CRT debug heap check (_CrtCheckMemory)
    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif
  // (8) ImageryPath — pure setting, no allocation.
    if (NoNormals == true)
        TObjectImagery::SetImageryPath(NONORMALPATH);
    else
        TObjectImagery::SetImageryPath(NORMALPATH);

#if 0 // TODO(port): Subsystem 7 — MSVC CRT debug heap check (_CrtCheckMemory)
    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif
  // Allocate the chunk cache as 1/4 free physical memory or 4MB
    int32_t physmegs = TotalPhys() / (1024 * 1024);
    if (ChunkCacheSize < 1)
    {
        if (physmegs < 16)
        {
#if 0 // TODO(port): Subsystem 2 — Win32 MessageBox warning; stub as warning log
            uint32_t res = MessageBox(nullptr, "This game requires at least 16MB of system minimum to run well. "
                             "If you press OK the game will load normally, but game "
                             "performance will be severely degraded.",
                             "WARNING", MB_ICONSTOP | MB_OK);

            if (res == IDCANCEL)
                return false;
#else
            fprintf(stderr, "WARNING: less than 16MB physical memory detected; continuing.\n");
#endif

            ChunkCacheSize = 2;
        }
        else if (physmegs <= 24)
            ChunkCacheSize = 3;
        else if (physmegs <= 32)
            ChunkCacheSize = 4;
        else
            ChunkCacheSize = min(physmegs - 32, 16);
    }
#if 0 // TODO(port): Subsystem 7 — MSVC CRT debug heap check (_CrtCheckMemory)
    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif
    if (PreloadSectorSize < 0)
    {
        if (physmegs < 16)
        {
            PreloadSectorSize = 3;
            PreloadSectors = false;
        }
        else if (physmegs <= 24)
        {
            PreloadSectorSize = 3;
            PreloadSectors = false;
        }
        else if (physmegs <= 32)
        {
            PreloadSectorSize = 4;
            PreloadSectors = true;
        }
        else
        {
            PreloadSectorSize = 5;
            PreloadSectors = true;
        }
    }
    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
    // (9) ChunkCache — AllocCache reserves the per-tile pinned buffers.
    ChunkCache.AllocCache(ChunkCacheSize);
    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }

  // Do the critical section object
#if 0 // TODO(port): Subsystem 4 — Win32 CRITICAL_SECTION (→ std::mutex / worker pool)
    InitializeCriticalSection(&CriticalSection);
#endif

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // (10) Display — sokol_gfx context setup + backbuffer/zbuffer surfaces
  // + ImGui wiring. Heavy on sokol calls but no game data is loaded.
    if (!Display.Initialize(GameScreenWidth, GameScreenHeight, BPP))
        FatalError("Couldn't open main display", nullptr);

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Now display is initialized, set up video cap system (if needed)
#if 0 // TODO(port): Subsystem 3 — video capture (DirectShow → AVFoundation/sokol)
    // (11) VideoCapture — optional, off by default; the entire block is
    // currently #if 0'd because the Win32 capture backend isn't ported.
    if (dovideocap)
        VideoCapture.Initialize(videocapmegs, videocapfps);
#endif

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // (12) Color tables — free-function setup; nothing to undo.
    MakeColorTables();

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Print stats
    Status("Revenant Version 0.30 Build %s %s\n"
           "Copyright (C) Cinematix Studios, Inc. 1998\n", __DATE__, __TIME__);
    if (IsMMX)
        Status("MMX detected, using advanced lighting\n");
    else
        Status("No MMX detected, using normal lighting\n");
    Status("Physical memory %d MB\n", physmegs);
    Status("Chunk cache size %d MB\n", ChunkCacheSize);
#if 0 // TODO(port): Subsystem 2 — DirectDraw device enumeration (→ sokol backend)
    Status("Available display devices: %s\n", DriversAvailable);
    Status("Current display device: %s - %s\n", DirectDrawName, DirectDrawDesc);
#endif
    Status("Preload sectors %s, preload size %d\n", (PreloadSectors?"ON":"OFF"), PreloadSectorSize);

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // (13) Timer
    Status("Initializing timers\n");
    if (!Timer.Initialize())
        FatalError("Couldn't initialize timer", nullptr);

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Capture the mouse cursor
//  SetCapture(MainWindow.Hwnd());

  // (14) Direct Input / Joystick — pending Subsystem 5 port; no-op now.
    Status("Initializing direct input\n");
#if 0 // TODO(port): Subsystem 5 — DirectInput (→ sokol_app event translation)
    InitializeDirectInput();
#endif

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Initialize Joysticks (if any)
    Status("Initializing joysticks\n");
#if 0 // TODO(port): Subsystem 5 — DirectInput joystick (→ GameController framework)
    InitializeJoysticks();
#endif

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // (15) Imagery loader thread — static-method API, paired with
  // TObjectImagery::EndLoaderThread() in ShutdownGlobals.
    TObjectImagery::BeginLoaderThread();

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // (16) TObjectClass — static load (paired with TObjectClass::FreeClasses
  // in ShutdownGlobals). Loads class.def into the global class registry.
    Status("Loading classes with %s option\n", NoQuickLoad?"NOQUICKLOAD":"QUICKLOAD");
    TObjectClass::LoadClasses();

    if (StartupDumpTilesPath[0])
    {
        Status("Dumping tiles to %s\n", StartupDumpTilesPath);
        if (!TestModes::DumpTilesToFolder(StartupDumpTilesPath))
            FatalError("Failed dumping any tiles to %s", StartupDumpTilesPath);
        Status("Tile dump complete. Exiting.\n");
        return false;
    }

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // (17) PlayerManager
    Status("Initializing multi-player manager\n");
    PlayerManager.Initialize();

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // (18) Rules — Initialize() loads rules.def + char.def.
    Status("Loading game rules\n");
    if (!Rules.Initialize())
        FatalError("Unable to load game rules");

  // (19) DialogList — Initialize() loads <Language>.def. Pulled out of
  // the old InitLanguage() helper so all global lifecycle calls live in
  // one canonical caller.
    if (!DialogList.Initialize())
        FatalError("Unable to load dialog list");

  // (20) AssetCache - shared CPU/source asset registry. Loading here gives
  // callers a safe engine handle, not renderer GPU residency.
    AssetCache.Init();

  // (21) MapManager — Init() is a state-flip; the cache populates lazily
  // as PlayScreen / TMapRenderer call GetOrLoad. Has to be live before
  // any subsystem that observes CurrentMapChanged.
    MapManager.Init();

  // (22) Sound — brings up miniaudio + scans data/sound/effects + the
  // language voice dir for WAVs. Comes after ResourcePath / Language are
  // valid and the data manager is up; before any caller can fire SFX.
    Status("Initializing audio system\n");
    SoundPlayer.Initialize();   // failure here is non-fatal; game runs silent

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
    return true;
}

void ShutdownGlobals()
{
  // Tear down in EXACT reverse of InitGlobals' construction order, so each
  // global is shut down while everything it depends on is still alive.
  // The numbered comments below mirror the numbered steps at the top of
  // InitGlobals — keep the two in lockstep when adding/removing
  // subsystems. Each shutdown call leaves members in empty/zero state so
  // the trivial dtor that runs at process exit only walks already-emptied
  // structures.
  //
  // Game-state-level teardown (active screen + worker threads) runs once
  // before the InitGlobals-inverse so per-map state has somewhere alive
  // to unwind into.

  // ---- game-state-level teardown (above InitGlobals) ----

  // Stop the active screen (PlayScreen owns the renderer + its listeners
  // on TGameMap). Drop it before the maps it observes get evicted.
    if (CurrentScreen)
    {
        CurrentScreen->Close();
        CurrentScreen = nullptr;
    }

  // Resume any paused worker threads so EndLoaderThread can join cleanly.
    extern bool ThreadsPaused;
    if (ThreadsPaused)
        ResumeThreads();

  // ---- inverse of InitGlobals ----

  // (22) Sound — stop the playback thread + drain music before anything
  // else unwinds; nothing else depends on audio so this is the safest
  // first inverse step. Also flushes the cached SFX list.
    SoundPlayer.Close();

  // (21) MapManager - inits last in InitGlobals, so closes first here.
  // Walks each cached TGameMap → Unload → CloseSector while sectors,
  // player inventories, and scripts are still alive. Was the use-after-
  // free site that motivated the whole Init/Shutdown refactor.
    MapManager.Shutdown();

  // (20) AssetCache - outlives maps and any source asset handles they held.
    AssetCache.Shutdown();

  // (19) DialogList
    DialogList.Close();

  // (18) Rules
    Rules.Close();

  // (17) PlayerManager
    PlayerManager.Close();

  // (16) TObjectClass — free the class registry/instances.
    TObjectClass::FreeClasses();

  // (15) Imagery loader thread
    TObjectImagery::EndLoaderThread();

  // (14) DirectInput / joystick teardown
#if 0 // TODO(port): Subsystem 5 — DirectInput teardown (→ sokol_app cleanup)
    CloseDirectInput();
    ReleaseCapture();
#endif

  // (13) Timer
    Timer.Close();

  // (12) Color tables — no per-instance cleanup needed.

  // (11) Video capture
#if 0 // TODO(port): Subsystem 3 — video capture (DirectShow → AVFoundation/sokol)
    VideoCapture.Close();
#endif

  // (10) Display
    Display.Close();

  // (9) ChunkCache — currently no shutdown; allocations free at process
  // exit via the dtor.

  // (8) ImageryPath — pure setting, nothing to undo.

  // (2) FontTable
    if (FontTable) {
        FontTable->Close();
        delete FontTable;
        FontTable = nullptr;
    }
    SystemFont = DialogFont = DialogFontShadow = SmallFont = GameFont = nullptr;

#if 0 // TODO(port): Subsystem 2 — Win32 message pump drain (→ sokol_app cleanup)
    MSG Message;
    while (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
#endif

  // (1) MainWindow — inverse of step 1 in InitGlobals.
    MainWindow.Close();

#if 0 // TODO(port): Subsystem 5 — Win32 ClipCursor (→ sapp_lock_mouse)
    ClipCursor(nullptr);
#endif
}

// ************************************************************************
// *               sokol_main - macOS/sokol_app Entry Point               *
// ************************************************************************
//
// sokol_app owns the window and the run loop. Our lifecycle is:
//   1. sokol_main()  -> return sapp_desc describing the window + callbacks
//   2. AppInit()     -> sapp has created the window + Metal context.
//                       Call InitGlobals() which brings up TDisplay (which
//                       itself calls sg_setup via sapp_sgcontext()),
//                       timers, imagery loader, rules, etc.
//   3. AppFrame()    -> one game tick per call. Currently drives the
//                       legacy screen dispatch by pumping one frame of
//                       the current TScreen.
//   4. AppEvent()    -> translate sapp_event -> TScreen input hooks.
//   5. AppCleanup()  -> ShutdownGlobals() and teardown.
//
// The pre-port WinMain / MessageLoop / RegisterClass scaffolding lives
// in attic/src/revmain_win32_entry.cpp and attic/src/mainwnd_win32.cpp.

#include <sokol_glue.h>
#include "sokol_imgui.h"
#include "imgui.h"

// Forward decls for the sokol callbacks (defined below sokol_main).
static void AppInit();
static void AppFrame();
static void AppEvent(const sapp_event* ev);
static void AppCleanup();

// The screen we were told to start on. Latched in AppInit, consumed in
// AppFrame. When a screen ends (TScreen::ShowScreen logic formerly in
// the old WinMain loop) we advance to GetNextScreen().
static TScreen* BootScreen = nullptr;
static bool SystemInitialized = false;

// Saved argv for AppInit — sokol owns the run loop after sokol_main
// returns, so we stash the pointers for the later GetParameters call.
static int    g_argc = 0;
static char **g_argv = nullptr;

sapp_desc sokol_main(int argc, char* argv[])
{
    // Stand up logging before anything else so early failures are visible.
    rev_logging_init("revenant.log");

    g_argc = argc;
    g_argv = argv;

    IsMMX = false;
    ApplyCommandLineResolution(argc, argv);

    sapp_desc desc = {};
    desc.init_cb = AppInit;
    desc.frame_cb = AppFrame;
    desc.event_cb = AppEvent;
    desc.cleanup_cb = AppCleanup;
    desc.width = GameScreenWidth;
    desc.height = GameScreenHeight;
    desc.window_title = "Revenant";
    desc.high_dpi = false;
    desc.sample_count = 1;
    // Enable OS clipboard. Required for the editor's Cut/Copy/Paste
    // commands -- sokol_imgui's clipboard handlers route through
    // sapp_get/set_clipboard_string which silently no-op when this
    // flag is false. Bumped buf size beyond the 8KB default since
    // serialized object blocks can run large.
    desc.enable_clipboard = true;
    desc.clipboard_size   = 1 << 20;     // 1 MB
    // Windowed by default; Borderless/FullScreen come from INI/args and
    // take effect before the window opens only if set via argv. Anything
    // else stays at sokol defaults.
    Windowed = true;
    return desc;
}

static void AppInit()
{
    // Equivalent to the top of the retired WinMain: set paths, load INI,
    // parse command line, initialize monitor info, language, window, and
    // the rest of the system.

    srand((unsigned)time(nullptr));

    // Posix port of retail's WinMain GetProgramPaths probe — RunPath is
    // the executable directory, SavePath is the per-user writable
    // Application Support / XDG_DATA_HOME dir (created if missing).
    rev_resolve_program_paths(RunPath, SavePath, MAXPATHLEN);
    INISetPath(RunPath);

    GetINISettings();

    GetParameters(g_argc, g_argv);

    // Base resource archives — retail WinMain opened these explicitly before
    // anything that calls rev_fopen (FontTable->Initialize, LoadClasses...).
    if (!MountArchive("resources.rvr"))
        FatalError("Error in pack file RESOURCE.RVR", nullptr);
    if (!MountArchive("imagery.rvi"))
        FatalError("Error in pack file IMAGERY.RVI", nullptr);

    // Enumerate available modules (data/Modules/*) and mount the main one.
    // ModuleManager.Initialize parses each module.def into a TModule, then
    // resolves [Modules]/MainModule from the INI; SetCurModule does the
    // actual VFS mount. Per-module files (area.def, master.s, sector .DATs,
    // etc.) resolve through that mount thereafter.
    if (!ModuleManager.Initialize())
        FatalError("No playable modules found under data/Modules/", nullptr);
    if (!ModuleManager.SetCurModule(ModuleManager.MainIndex()))
        FatalError("Unable to mount main module", nullptr);

    if (!InitMonitor())
        FatalError("Invalid monitor selected", nullptr);

    InitLanguage();

    // MainWindow's lifecycle now lives at the top of InitGlobals so every
    // singleton flows through the canonical caller; sokol_app already
    // owns the actual window so there is no construction-ordering issue.
    if (!InitGlobals())
    {
        sapp_request_quit();
        return;
    }

    // Revisited tunables. Layered defaults, lowest priority first:
    //   1. SRevisitedSettings field initializers (already in place at static init)
    //   2. revisited_defaults.cpp baked globals (this call)
    //   3. Revenant.ini [Revisited] overlay     (gated on --revisited)
    //   4. debug-panel live tweaks              (later in session)
    // Apply runs unconditionally so even classic-retail boots get the
    // modernized engine's baseline sun/AO/shadow state pushed to Renderer.
    // MapRenderer doesn't exist until PlayScreen::Initialize -- that side
    // of Apply fires from there.
    GetBakedRevisitedDefaults(RevisitedSettings);
    LoadRevisitedSettings();
    ApplyRevisitedSettingsToRenderer(Renderer);

    if (StartupTestMode[0])
    {
        log_info("[boot] routing to TestScreen, mode='%s'", StartupTestMode);
        BootScreen = &TestScreen;
    }
    else
    {
        log_info("[boot] routing to PlayScreen");
        BootScreen = &PlayScreen;

        // --loadmap=<file>: hand the save path to PlayScreen; its Pulse()
        // picks it up on the first tick, clears the current map, and streams
        // the player object — the same path the menu uses for "Continue".
        if (StartupSavePath[0])
        {
            log_info("[boot] auto-loading save '%s'", StartupSavePath);
            PlayScreen.LoadGameFile(StartupSavePath);
        }
    }

    SystemInitialized = true;
}

static void AppFrame()
{
    // Drive the legacy screen dispatch. The pre-port top-level loop was:
    //     while (NextScreen) NextScreen = TScreen::ShowScreen(NextScreen, 0);
    // TScreen::ShowScreen is blocking (runs that screen's TimerLoop),
    // Every screen (test + production) flows through the same path now that
    // TScreen::TimerTick is sokol-native. ShowScreen Initialize()s a screen
    // and hands back; AppFrame calls its TimerTick once per sokol frame;
    // when the screen sets `done`, we EndCurrentScreen and advance to
    // GetNextScreen (or quit if there isn't one).

    if (!SystemInitialized || Closing)
        return;

    TTime::BeginFrame(sapp_frame_duration());

    // Start the ImGui frame before Animate/draw so screens can build debug
    // panels from their normal per-frame code. simgui_render() is called
    // inside TDisplay::FlipPage on the swapchain pass.
    {
        // KEEP trickle enabled (ImGui 1.92 default). It splits ambiguous
        // input transitions across frames, which is what makes Mac
        // Cmd-modified shortcuts work (sokol's Cmd-held auto-keyup
        // fires KEY_DOWN+KEY_UP in the same NSEvent — without trickle,
        // Z appears to never be pressed). Trickle's per-event-type
        // rules already consolidate mouse-move spam, so high-Hz mice
        // don't actually pile up. See sokol issue #233 +
        // memory/project_imgui_mac_modifiers.md.
        simgui_frame_desc_t fd = {};
        fd.width       = sapp_width();
        fd.height      = sapp_height();
        fd.delta_time  = sapp_frame_duration();
        fd.dpi_scale   = sapp_dpi_scale();
        simgui_new_frame(&fd);
    }

    // Begin next queued screen if none is active.
    if (!CurrentScreen && BootScreen)
    {
        TScreen *next = BootScreen;
        BootScreen = nullptr;
        if (!TScreen::ShowScreen(next, 0))
        {
            sapp_request_quit();
            return;
        }
    }

    if (!CurrentScreen)
    {
        sapp_request_quit();
        return;
    }

    // Flush coalesced mouse move. See AppEvent MOUSE_MOVE for the rationale.
    if (g_pendingMouseMove)
    {
        g_pendingMouseMove = false;
        if (AppActive)
            CurrentScreen->MouseMove(mousebutton, cursorx, cursory);
    }

    // Tick / Draw split (see docs/FRAME_PIPELINE.md). Tick catches up
    // missed legacy 24Hz pulses (pure sim, no draw calls); DrawFrame
    // opens the Overlay2D pass on the backbuffer and runs the screen's
    // 2D draw work. Anything in the active screen's Pulse goes through
    // Tick; anything in its Animate goes through DrawFrame.
    CurrentScreen->Tick();
    TObjectComponent::RunUpdateList();
    CurrentScreen->DrawFrame();
    DebugUI::DrawFrame();

    // Present the frame: composite backbuffer onto the swapchain and commit
    // the Metal command buffer. Without this, sokol's cmd_buffer is never
    // released/nil'd and its dangling pointer crashes the next frame's
    // sg_begin_pass. Skip in headless mode (no Display.Initialize() yet
    // / already Close()d) — IsActive() guards the back-buffer dereference
    // path inside FlipPage.
    if (Display.IsActive())
        Display.FlipPage();

    if (CurrentScreen->IsDone() || Closing)
    {
        TScreen *next = CurrentScreen->GetNextScreen();
        TScreen::EndCurrentScreen();
        BootScreen = next;
        if (!next && !Closing)
            sapp_request_quit();
    }
}

static void AppCleanup()
{
    // ShutdownGlobals() handles MainWindow.Close() as its very last step;
    // AppCleanup just drives that and then unmounts the resource archives.
    if (SystemInitialized)
        ShutdownGlobals();
    UnmountAll();
}

// Translate an sapp_keycode into the legacy VK_* codes the screen / pane
// key handlers were written against. Only the keys the engine actually
// switches on need an accurate mapping; everything else passes through
// as the sokol enum value (the game's key-binding system compares codes
// numerically, it doesn't care what they are as long as they're stable).
static int32_t SappKeyToVK(sapp_keycode key)
{
    switch (key)
    {
        case SAPP_KEYCODE_LEFT_CONTROL:
        case SAPP_KEYCODE_RIGHT_CONTROL: return VK_CONTROL;
        case SAPP_KEYCODE_LEFT_ALT:
        case SAPP_KEYCODE_RIGHT_ALT:     return VK_MENU;
        case SAPP_KEYCODE_LEFT_SHIFT:
        case SAPP_KEYCODE_RIGHT_SHIFT:   return VK_SHIFT;
        case SAPP_KEYCODE_BACKSPACE:     return VK_BACK;
        case SAPP_KEYCODE_ENTER:         return VK_RETURN;
        case SAPP_KEYCODE_TAB:           return VK_TAB;
        case SAPP_KEYCODE_ESCAPE:        return VK_ESCAPE;
        case SAPP_KEYCODE_SPACE:         return VK_SPACE;
        case SAPP_KEYCODE_LEFT:          return VK_LEFT;
        case SAPP_KEYCODE_RIGHT:         return VK_RIGHT;
        case SAPP_KEYCODE_UP:            return VK_UP;
        case SAPP_KEYCODE_DOWN:          return VK_DOWN;
        case SAPP_KEYCODE_INSERT:        return VK_INSERT;
        case SAPP_KEYCODE_DELETE:        return VK_DELETE;
        case SAPP_KEYCODE_HOME:          return VK_HOME;
        case SAPP_KEYCODE_END:           return VK_END;
        case SAPP_KEYCODE_PAGE_UP:       return VK_PRIOR;
        case SAPP_KEYCODE_PAGE_DOWN:     return VK_NEXT;
        case SAPP_KEYCODE_F1:            return VK_F1;
        case SAPP_KEYCODE_F2:            return VK_F2;
        case SAPP_KEYCODE_F3:            return VK_F3;
        case SAPP_KEYCODE_F4:            return VK_F4;
        case SAPP_KEYCODE_F5:            return VK_F5;
        case SAPP_KEYCODE_F6:            return VK_F6;
        case SAPP_KEYCODE_F7:            return VK_F7;
        case SAPP_KEYCODE_F8:            return VK_F8;
        case SAPP_KEYCODE_F9:            return VK_F9;
        case SAPP_KEYCODE_F10:           return VK_F10;
        case SAPP_KEYCODE_F11:           return VK_F11;
        case SAPP_KEYCODE_F12:           return VK_F12;
        default:                          return (int32_t)key;
    }
}

static void AppEvent(const sapp_event* ev)
{
    if (!ev) return;

    // ImGui sees every event. simgui_handle_event returns the OR of
    // WantCaptureKeyboard and WantCaptureMouse, which swallows mouse drags
    // whenever a slider has keyboard focus — so we split the flags and gate
    // each event class on the one that matters.
    simgui_handle_event(ev);
    ImGuiIO& io = ImGui::GetIO();
    const bool imgui_kbd   = io.WantCaptureKeyboard;
    const bool imgui_mouse = io.WantCaptureMouse;

    switch (ev->type)
    {
      case SAPP_EVENTTYPE_KEY_DOWN:
      {
        const int32_t vk = SappKeyToVK(ev->key_code);
        if (vk == VK_CONTROL) CtrlDown = true;
        else if (vk == VK_MENU) AltDown = true;
        else if (vk == VK_SHIFT) ShiftDown = true;

        // macOS Cmd+Q quits the app, matching the platform convention.
        // ImGui doesn't get to swallow this -- the user always wants it
        // to work even if a slider has keyboard focus.
        if (ev->key_code == SAPP_KEYCODE_Q
            && (ev->modifiers & SAPP_MODIFIER_SUPER))
        {
            sapp_request_quit();
            break;
        }

        if (vk == VK_F10)
        {
            DebugUI::SetVisible(!DebugUI::IsVisible());
            break;
        }

        if (!AppActive) break;
        if (imgui_kbd) break;
        if (CurrentScreen)
            CurrentScreen->KeyPress(vk, true);
        break;
      }

      case SAPP_EVENTTYPE_KEY_UP:
      {
        const int32_t vk = SappKeyToVK(ev->key_code);
        if (vk == VK_CONTROL) CtrlDown = false;
        else if (vk == VK_MENU) AltDown = false;
        else if (vk == VK_SHIFT) ShiftDown = false;

        if (!AppActive) break;
        if (imgui_kbd) break;
        if (CurrentScreen)
            CurrentScreen->KeyPress(vk, false);
        break;
      }

      case SAPP_EVENTTYPE_CHAR:
      {
        if (!AppActive) break;
        if (imgui_kbd) break;
        if (CtrlDown && ShiftDown) break;
        if (CurrentScreen)
            CurrentScreen->CharPress((int32_t)ev->char_code, true);
        break;
      }

      case SAPP_EVENTTYPE_MOUSE_ENTER:
      {
        // Re-assert the game cursor whenever the mouse re-enters our
        // window: AppKit auto-pops [NSCursor set] to the system arrow
        // across window/view crossings, so the initial set in
        // SetMouseBitmap silently decays. Re-asserting on enter is the
        // minimal fix that doesn't fight AppKit when the mouse legitimately
        // belongs to another app (where we don't get this event).
        if (!imgui_mouse)
            rev_platform::ReassertOSCursor();
        break;
      }

      case SAPP_EVENTTYPE_MOUSE_LEAVE:
      {
        // Mouse moved outside our window. [NSCursor set] is process-
        // local; AppKit normally pops to whatever cursor the next
        // window/app installs, but if no other window actively manages
        // its cursor (desktop, an app without cursor rects, ...) our
        // last [set] persists visually -- so the user sees the game
        // cursor floating over the desktop. Explicitly hand back to
        // the system arrow on leave; other apps that DO manage their
        // cursor will override it the moment the mouse hits them.
        rev_platform::ResetOSCursor();
        break;
      }

      case SAPP_EVENTTYPE_MOUSE_MOVE:
      {
        cursorx = (int32_t)ev->mouse_x;
        cursory = (int32_t)ev->mouse_y;
        if (!AppActive) break;
        // Belt-and-suspenders re-assert: some window managers / setups
        // skip the explicit MOUSE_ENTER but always send MOUSE_MOVE once
        // the cursor is inside the window. The OS-cursor backend
        // short-circuits when our cached cursor is already current, so
        // this is a single pointer compare on the hot path.
        if (!imgui_mouse)
            rev_platform::ReassertOSCursor();
        const bool sector_test_drag =
            StartupTestMode[0] &&
            stricmp(StartupTestMode, "sector") == 0 &&
            (mousebutton != 0);
        if (imgui_mouse && !sector_test_drag) break;
        // Coalesce. A single frame can now see dozens of MOUSE_MOVE events
        // (trackpads fire at 120–500 Hz and we drain the whole NSEvent queue
        // at the top of drawRect). Calling CurrentScreen->MouseMove for each
        // one drives pane/screen logic harder than needed — just flag a
        // pending move and AppFrame forwards the latest cursor position once
        // per frame.
        g_pendingMouseMove = true;
        break;
      }

      case SAPP_EVENTTYPE_MOUSE_DOWN:
      {
        cursorx = (int32_t)ev->mouse_x;
        cursory = (int32_t)ev->mouse_y;
        if (!AppActive) break;
        if (imgui_mouse) break;
        int32_t btn = 0;
        switch (ev->mouse_button)
        {
          case SAPP_MOUSEBUTTON_LEFT:   btn = MB_LEFTDOWN;   mousebutton |= MB_LEFTDOWN;   break;
          case SAPP_MOUSEBUTTON_RIGHT:  btn = MB_RIGHTDOWN;  mousebutton |= MB_RIGHTDOWN;  break;
          case SAPP_MOUSEBUTTON_MIDDLE: btn = MB_MIDDLEDOWN; mousebutton |= MB_MIDDLEDOWN; break;
          default: break;
        }
        if (btn && CurrentScreen)
            CurrentScreen->MouseClick(btn, cursorx, cursory);
        break;
      }

      case SAPP_EVENTTYPE_MOUSE_UP:
      {
        cursorx = (int32_t)ev->mouse_x;
        cursory = (int32_t)ev->mouse_y;
        if (!AppActive) break;
        // MouseUp always forwarded so in-progress drags can clear even if
        // the release lands over an ImGui panel; otherwise drag state latches.
        int32_t btn = 0;
        switch (ev->mouse_button)
        {
          case SAPP_MOUSEBUTTON_LEFT:   btn = MB_LEFTUP;   mousebutton &= ~MB_LEFTDOWN;   break;
          case SAPP_MOUSEBUTTON_RIGHT:  btn = MB_RIGHTUP;  mousebutton &= ~MB_RIGHTDOWN;  break;
          case SAPP_MOUSEBUTTON_MIDDLE: btn = MB_MIDDLEUP; mousebutton &= ~MB_MIDDLEDOWN; break;
          default: break;
        }
        if (btn && CurrentScreen)
            CurrentScreen->MouseClick(btn, cursorx, cursory);
        break;
      }

      case SAPP_EVENTTYPE_RESIZED:
      {
        // TODO(port): TDisplay is hard-coded WIDTH x HEIGHT. When the
        // renderer learns to cope with resize we should forward new
        // dimensions here.
        break;
      }

      case SAPP_EVENTTYPE_FOCUSED:
      case SAPP_EVENTTYPE_RESUMED:
      {
        AppActive = true;
        if (CurrentScreen)
            CurrentScreen->Redraw();
        break;
      }

      case SAPP_EVENTTYPE_UNFOCUSED:
      case SAPP_EVENTTYPE_SUSPENDED:
      case SAPP_EVENTTYPE_ICONIFIED:
      {
        AppActive = false;
        break;
      }

      case SAPP_EVENTTYPE_QUIT_REQUESTED:
      {
        Closing = true;
        break;
      }

      default:
        break;
    }
}

// Retired original WinMain body. Replaced by sokol_main + AppInit above.
// Kept here in #if 0 briefly so the diff to attic/src/revmain_win32_entry.cpp
// is easy to eyeball; delete once the sokol path is verified at runtime.
#if 0
static void UnusedWinMainAnchor_()
{
#ifdef _DEBUG
   _CrtMemState s1;

   _CrtMemCheckpoint( &s1 );

    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
#endif

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Preset capabilities flags
    IsMMX = HasMMX();

#if 0 // TODO(port): Subsystem 7 — MEMORYSTATUS snapshot; dropped, see header comment
    StartMemory.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&StartMemory);
#endif

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

    ::hInstance = hInstance;

    int32_t size = sizeof(SObjectInfo);

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Set windows to return fail when file not found..
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Calls small heap alloc routine for requests < MAXSMALLHEAP
    _set_sbh_threshold( MAXSMALLHEAP );

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Randomize!!
    srand( (unsigned)time( nullptr ) );

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Get upper case command line
    strupr(lpCmdLine);

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Gets location of program path and save path
    GetProgramPaths(lpCmdLine, RunPath, SavePath);
    _chdir(RunPath);
    INISetPath(RunPath);

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // What version of Windows is this
    uint32_t ver  = GetVersion();
    uint8_t major = LOBYTE(ver);
    uint8_t minor = HIBYTE(ver);

    if (major <= 3) FatalError("This game requires Windows '95/NT 4.0 (or higher)!", nullptr);

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Get INI file settings
    GetINISettings();

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Get Program Parameters
    GetParameters(lpCmdLine);

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif
  // Init Pause Thread Mutex
    PauseMutex = CreateMutex(nullptr, false, nullptr);

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Init monitor information
    if (!InitMonitor())
        FatalError("Invalid monitor selected", nullptr);

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Initialize language resources
    if (!InitLanguage());

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Create the main window
    if (!MainWindow.Initialize(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
        FatalError("Couldn't create main window", nullptr);

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Ok, now initialize the rest of the system
    if (!InitGlobals())
        return 0;

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Figure out first screen
    TScreen* NextScreen = &PlayScreen;

  // Do game screens
    while (NextScreen)
        NextScreen = TScreen::ShowScreen(NextScreen, 0);

  // Close the system down
    ShutdownGlobals();

    return 0;
}
#endif  // retired WinMain body
