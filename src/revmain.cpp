// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  RevMain.cpp - System main module                     *
// *************************************************************************

#include <stdarg.h>

// For multimonitor support
#define COMPILE_MULTIMON_STUBS
#include <fcntl.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "revenant.h"
#include "3dscene.h"
#include "bitmap.h"
#include "display.h"
#include "graphics.h"
#include "playscreen.h"
#include "mappane.h"
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
#include "player.h"
#include "multictrl.h"
#include "spell.h"
#include "spellpane.h"
#include "statpane.h"
#include "script.h"
#include "textbar.h"
#include "statusbar.h"
#include "sound.h"
#include "editor.h"
#include "dls.h"
#include "ctrlmap.h"
#include "area.h"
#include "rules.h"
#include "dialog.h"

// Global Variables
HINSTANCE hInstance;
HANDLE PauseMutex;
MEMORYSTATUS StartMemory;

// Game directories
TString RunPath;
TString SavePath;

// Define the Editor paths
TString ClassDefPath;               // Where to load / save Class.Def
TString ExileRCPath;                // Where to run ExileRC from & where
                                    // the graphics for the resources are
TString ResourcePath;               // Where to read / write the resources
TString BaseMapPath;                // Where the untouched version of the game map is stored
TString CurMapPath;                 // Where the current map is stored

// Current language
TString Language;                   // Where the current map is stored

// Global Structure Defines
TScreen*        CurrentScreen;      // Currently displayed screen object
TScreen*        NextScreen;         // Next Screen to be display object
TDisplay        display;            // Display object
TDisplay*       Display = &display; // Display pointer
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
TVideoCapture   VideoCapture;       // Video capture object
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
TSoundPlayer    SoundPlayer;        // Sound effects player
TControlMap     ControlMap;         // Contains the key/joystick mappings for game control
TAreaManager    AreaManager;        // Manages the game area system
TPlayerManager  PlayerManager;      // Manages the game player list
TRules  Rules;                      // Manages global rules data (classes, chars, stats for attacks, etc.)
CRITICAL_SECTION CriticalSection;   // Controls enter critical section functions;
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
HMONITOR Monitor = nullptr;     // Windows monitor handle
MONITORINFOEX MonitorInfo;      // Windows monitor info structure

// Game flags
bool Windowed = false;      // Do we run the game in a window in NORMAL mode (instead of EXLUSIVE)
bool SingleBuffer = false;  // True if single buffering (true if Windowed=true, or UsingHardware=false)
bool Borderless = false;    // When windowed, cut out the border (menu, other windows garabage)?
bool Debug = false;         // Should program be run in debug compatible mode?
bool Ignore3D = false;      // Whether to disallow 3D imagery (to make it run on systmes without Direct3D)
bool Editor = false;        // true when in editor mode
bool StartInEditor = false; // Whether to start the program in editor mode
bool NoQuickLoad = false;   // true if program should not try to use IMAGERY.DAT file
bool Force15Bit = false;    // Forces video mode to assume 15 bit
bool Force16Bit = false;    // Forces video mode to assume 16 bit
bool FullScreen = false;    // Full screen mode
bool ShowZBuffer      = false;  // Show Z buffer for debugging
bool ShowNormalBuffer = false;  // Show normal buffer for debugging
bool UnlockImmediately = true; // Unlock DD Surf after lock (for debugging, allows stepping through draw code)

// Video capture globals (set in command line parse, used after display is initialized)
static bool dovideocap = false;
static int32_t videocapmegs, videocapfps;

// Driver name string (allows user to select driver by just typing in name of card)
char DXDriverMatchStr[FILENAMELEN]; // Will use first DX driver who's description has the given string in it
                                    // i.e. use if string is "permidia" and driver desc is "Glint Permidia 2 3D"

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

// For setting up 3D object bounding rects in the editor
bool UpdatingBoundingRect = false;

// Is joystick available?
bool HasJoyStick = false;
JOYINFO JoyInfo;

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

void ThreadError(char *error, char *extra)
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

// Duplicate of revutils.cpp definitions. Agent A rewrites revmain.cpp
// around sokol_app; this block will be replaced wholesale there.
#if 0
void WaitSingleErr(HANDLE obj) {}
void WaitMultipleErr(uint32_t objs, const HANDLE *obj, bool all) {}
#endif

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
        buf[l - 2] = nullptr;
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

    buf[0] = nullptr;
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

// ************************************************************************
// *                     Initialization Functions                         *
// ************************************************************************

#define cpuid __asm _emit 0x0f __asm _emit 0xa2

#define MMXBIT (1 << 23)

bool HasMMX()
{
    uint32_t version;
    uint32_t features;

    __asm
    {
        mov eax, 1
        cpuid
        mov version, eax
        mov features, edx
    }

    return (features & MMXBIT) != 0;
}

void GetProgramPaths(char *lpCmdLine, char *RunPath, char *SavePath)
{

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
}

void GetParameters(char *lpCmdLine)
{
    char *ptr;

  // Find out game speed first!
    ptr = strstr(lpCmdLine, "GAMESPEED=");  // Set game speed
    if (ptr)                                
    {
        GameSpeed = atoi(ptr + 10);
        if (GameSpeed < 1)
            GameSpeed = 1;
        if (GameSpeed > 5)
            GameSpeed = 5;
    }

  // Setup performance variables
    if (GameSpeed <= 1)
    {
        SmoothScroll = false;
        MaxLights = 1;
    }
    if (GameSpeed < 3)
    {
        UseDirLight = false;
    } 

  // Do rest of stuff...

    if (strstr(lpCmdLine, "DEBUG"))
        Debug = true;                       // Sets up debug compatible mode.

    if (strstr(lpCmdLine, "FORCE15BIT"))
        Force15Bit = true;                  // Forces 15 bit mode

    if (strstr(lpCmdLine, "FORCE16BIT"))
        Force16Bit = true;                  // Forces 15 bit mode

    if (strstr(lpCmdLine, "EDITOR"))        // Start in editor mode
        StartInEditor = true;

    if (strstr(lpCmdLine, "WINDOWED"))      // Run game in a window
        Windowed = true;

    if (strstr(lpCmdLine, "NOQUICKLOAD"))   // Run game in a window
        NoQuickLoad = true;

    if (strstr(lpCmdLine, "BORDERLESS"))    // Run game with no border
        Borderless = true;

    if (strstr(lpCmdLine, "NOWIDE"))        // Prevents wide video buffers (stride > width)
        NoWideBuffers = true;

    if (strstr(lpCmdLine, "FULLSCREEN"))    // Causes game map to start in full screen mode
        FullScreen = true;

    if (strstr(lpCmdLine, "NOPRELOADSECTORS"))  // Causes sectors to NOT be cached
    {
        PreloadSectors = false;
        PreloadSectorSize = 3;
    }

    ptr = strstr(lpCmdLine, "MONITOR=");    // Set monitor number
    if (ptr)                                
        MonitorNum = atoi(ptr + 8);

    ptr = strstr(lpCmdLine, "VIOLENCELEVEL=");  // Set Violence level
    if (ptr)                                
    {
        ViolenceLevel = atoi(ptr + 14);
        if (ViolenceLevel < 0)
            ViolenceLevel = 0;
        if (ViolenceLevel > 5)
            ViolenceLevel = 5;
    }

    ptr = strstr(lpCmdLine, "PRELOADSIZE=");    // Set game speed
    if (ptr)                                
    {
        PreloadSectorSize = atoi(ptr + 12);
        if (PreloadSectorSize > 8)
            PreloadSectorSize = 8;
        if (PreloadSectorSize < 3)
        {
            PreloadSectors = false;
            PreloadSectorSize = 3;
        }
    }

    ptr = strstr(lpCmdLine, "CHUNKCACHESIZE="); // Set game speed
    if (ptr)                                
    {
        ChunkCacheSize = atoi(ptr + 15);
        if (ChunkCacheSize > 256)
            ChunkCacheSize = 256;
    }

    ptr = strstr(lpCmdLine, "DRIVER="); // Set display device
    if (!ptr)
        ptr = strstr(lpCmdLine, "DEVICE="); // Set display device (same thing)
    if (ptr)
    {
        ptr += 7;
        char *d = DXDriverMatchStr;
        while (*ptr && *ptr != ' ')
            *d++ = *ptr++;
        *d = 0;
        strlwr(DXDriverMatchStr);
    }

    if (strstr(lpCmdLine, "IGNORE3D"))      // Run game without Direct3D objects
        Ignore3D = true;

    if (strstr(lpCmdLine, "OLDDIRECT3D"))   // Prevents game from using draw primitives
    {
        UseDirect3D2 = false;               // Doesn't initialize Direct3D 2 stuff
        UseDrawPrimitive = false;           // Forces system to use execute buffers
    }

    if (strstr(lpCmdLine, "SOFTWARE3D"))    // Turns on default software 3D (BLUE)
    {
        UseSoftware3D = true;
    }

    if (strstr(lpCmdLine, "RAMPMODE"))      // Runs software 3D in ramp emulation mode
    {
        SoftRampMode = true;
        SoftRGBMode = false;
        UseSoftware3D = true;
    }

    if (strstr(lpCmdLine, "RGBMODE"))       // Runs software 3D in rgb mode
    {
        SoftRampMode = false;
        SoftRGBMode = true;
        UseSoftware3D = true;
    }

    if (strstr(lpCmdLine, "BLUE"))  // Causes game to use software 3D
    {
        UseBlue = true;
        SoftRampMode = true;        // Initializes Direct3D as if it was going to use 
        SoftRGBMode = false;        // The software RGB mode (BUT WE NEVER DO!)
        UseSoftware3D = true;
    }

    if (strstr(lpCmdLine, "NODRAWPRIM"))    // Forces system to use execute buffers
        UseDrawPrimitive = false;

    if (strstr(lpCmdLine, "NOCACHEEXBUFS")) // Prevents system from caching execute buffers
        CacheExBufs = false;

    if (strstr(lpCmdLine, "NOVIDZLOCK"))    // Prevents system from simultaneously lcoking the
        NoVidZBufLock = true;               // display video and z surfaces (locks the VooDoo)

    if (strstr(lpCmdLine, "CLEARZ"))        // Causes system to allocate an extra ZBuffer so 
    {                                       // we can use the Viewport->Clear() function to 
        UseClearZBuffer = true;             // update the screen ZBuffer, and so that we can
        NoVidZBufLock = false;              // draw to the ZBuffer in TDisplay (since 
    }                                       // it won't be the tree screen ZBuffer.

    if (strstr(lpCmdLine, "VOODOO"))        // This is a voodoo card
    {                                       // we can use the Viewport->Clear() function to 
        IsVooDoo = true;                    
        NoVidZBufLock = true;   // Don't try to lock video and zbuffer at same time
        NoBlitZBuffer = true;   // Can't blit to or from the zbuffer
        UseClearZBuffer = true; // Use a secondary clear zbuffer for drawing instead of display zbuffer 
    }

    if (strstr(lpCmdLine, "NOMMX"))         // Force into no MMX mode
        IsMMX = false;

    ptr = strstr(lpCmdLine, "VIDEOCAP="); // VIDEOCAP=megs,fps 
    if (ptr)
    {
        dovideocap = true;
        videocapmegs = atoi(ptr + 9);
        if (videocapmegs < 4)
            videocapmegs = 4;
        else if (videocapmegs > 128)
            videocapmegs = 128;
        if (ptr[10] == ',')
            ptr += 11;
        else if (ptr[11] == ',')
            ptr += 12;
        else if (ptr[12] == ',')
            ptr += 13;
        else
            ptr = nullptr;
        videocapfps = FRAMERATE;
        if (ptr)
            videocapfps = atoi(ptr);
        if (videocapfps < 8)
            videocapfps = 8;
        else if (videocapfps > FRAMERATE)
            videocapfps = FRAMERATE;
    }
    else
        dovideocap = false;

    ptr = strstr(lpCmdLine, "FASTLOCK=");
    if (ptr)                                // Sets the buffer locking mode.  If on, causes 
    {                                       // buffers to be unlocked immediately after locked (faster)
        UnlockImmediately = !strnicmp(ptr + 9, "on", 2) ||
                            !strnicmp(ptr + 9, "1", 1);
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

    INISetSection("Options");
    DoubleTapTicks = INIGetInt("DoubleTapTicks", 6);
}

// This function gets called as soon as the display system finds the right driver.
// You can check the parameters of the driver and set system flags here so that things
// work correctly.
void DriverSetupCallback()
{
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
}

bool InitLanguage()
{
  // Set language
    strcpy(Language, "english");

  // Load Language file
    DialogList.Initialize();

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

bool CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hDC, LPRECT lpRect, LPARAM)
{
    MonitorInfo.cbSize = sizeof(MonitorInfo);
    Monitor = hMonitor;
    GetMonitorInfo(hMonitor, (MONITORINFO*)&MonitorInfo);
    MonitorX = MonitorInfo.rcMonitor.left;
    MonitorY = MonitorInfo.rcMonitor.top;
    MonitorW = MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left;
    MonitorH = MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top;

    char *ptr = strstr(MonitorInfo.szDevice, "Display");
    if (!ptr)
        ptr = strstr(MonitorInfo.szDevice, "display");
    if (!ptr)
        ptr = strstr(MonitorInfo.szDevice, "DISPLAY");
    int32_t dispnum = atoi(ptr + 7);

    if (MonitorNum <= 1 && ((dispnum == 1) || (dispnum == 0)))
    {
        if (MonitorNum == 0)
            strcpy(MonitorInfo.szDevice, "display");
        return false;
    }

    if (MonitorNum == dispnum)
        return false;

    return true;
} 

bool InitMonitor()
{
    if (MonitorNum > GetSystemMetrics(SM_CMONITORS) || MonitorNum < 0)
        return false;
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, nullptr);

    return true;
}

// ************************************************************************
// *                    Initialize and Close System                       *
// ************************************************************************

bool InitSystem()
{
    SystemFont = TFont::LoadFont(100);
    DialogFont = TFont::LoadFont(101);
    DialogFontShadow = TFont::LoadFont(102);
    SmallFont = TFont::LoadFont(104);
    GameFont = TFont::LoadFont(105);
//  GoldFont = TFont::LoadFont(105);
    
    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Set correct imagery path
    if (NoNormals == true)
        TObjectImagery::SetImageryPath(NONORMALPATH);
    else
        TObjectImagery::SetImageryPath(NORMALPATH);

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Allocate the chunk cache as 1/4 free physical memory or 4MB
    int32_t physmegs = TotalPhys() / (1024 * 1024);
    if (ChunkCacheSize < 1)
    {
        if (physmegs < 16)
        {
            uint32_t res = MessageBox(nullptr, "This game requires at least 16MB of system minimum to run well. "
                             "If you press OK the game will load normally, but game "
                             "performance will be severely degraded.",
                             "WARNING", MB_ICONSTOP | MB_OK);
            
            if (res == IDCANCEL)
                return false;

            ChunkCacheSize = 2;
        }
        else if (physmegs <= 24)
            ChunkCacheSize = 3;
        else if (physmegs <= 32)
            ChunkCacheSize = 4;
        else
            ChunkCacheSize = min(physmegs - 32, 16);
    }
    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
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
    ChunkCache.AllocCache(ChunkCacheSize);
    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }

  // Do the critical section object
    InitializeCriticalSection(&CriticalSection);

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Create main display
    if (!Display->Initialize(WIDTH, HEIGHT, BPP))
        FatalError("Couldn't open main display", nullptr);

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Now display is initialized, set up video cap system (if needed)
    if (dovideocap)
        VideoCapture.Initialize(videocapmegs, videocapfps);

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Setup system color tables
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
    Status("Available display devices: %s\n", DriversAvailable);
    Status("Current display device: %s - %s\n", DirectDrawName, DirectDrawDesc);
    Status("Preload sectors %s, preload size %d\n", (PreloadSectors?"ON":"OFF"), PreloadSectorSize);

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Initialize the timer
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

  // Initialize Direct Input
    Status("Initializing direct input\n");
    InitializeDirectInput();

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Initialize Joysticks (if any)
    Status("Initializing joysticks\n");
    InitializeJoysticks();

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Start loader thread..
    TObjectImagery::BeginLoaderThread();

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Load class.def
    Status("Loading classes with %s option\n", NoQuickLoad?"NOQUICKLOAD":"QUICKLOAD");
    TObjectClass::LoadClasses();

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Setup player manager
    Status("Initializing multi-player manager\n");
    PlayerManager.Initialize();

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Load rules data
    Status("Loading game rules\n");
    Rules.Initialize();

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
    return true;
}

void CloseSystem()
{
  // Delete dialog list
    DialogList.Close();

  // Kill fonts
    delete SystemFont;
    delete DialogFont;
    delete DialogFontShadow;
    delete SmallFont;
    delete GameFont;
//  delete GoldFont;

  // Kill video capture buffers (if they are allocated)
    VideoCapture.Close();

  // Kill rules data
    Rules.Close();

  // Close player manager
    PlayerManager.Close();

  // Free all object classes in the class system
    TObjectClass::FreeClasses();

  // Resume all threads
    if (ThreadsPaused)
        ResumeThreads();        

    if (CurrentScreen)
    {
        CurrentScreen->Close();
        CurrentScreen = nullptr;
    }

  // End loader thread..
    TObjectImagery::EndLoaderThread();

  // Close Sound System
    CDStop();
    CDClose();
    SoundPlayer.Close();

  // Release DirectInput (including joystick)
    CloseDirectInput();

  // Release the mouse capture
    ReleaseCapture();

  // Stop timer stuff
    Timer.Close();

  // Close the display
    Display->Close();

    MSG Message;
    while (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }

  // Close the main window
    MainWindow.Close();

  // Clear out any clipping rectangle
    ClipCursor(nullptr);
}

// ************************************************************************
// *                   WinMain - Main Program Function                    *
// ************************************************************************

/* Pretty simple here.  Just make a main window, start its message loop
 * going, then when the message loop returns, close the program. */

int32_t PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int32_t nCmdShow)
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

    StartMemory.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&StartMemory);

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
    if (!InitSystem())
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
    CloseSystem();

    return 0;
}
