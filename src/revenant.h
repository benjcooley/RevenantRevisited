// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   Revenant.h - Main Include File                      *
// *************************************************************************

#pragma once

#include "revdefs.h"
#include "revtypes.h"

#include <HandmadeMath.h>
#include <sokol_gfx.h>

#include <cinttypes>
#include <cstddef>
#include <cstdio>
#include <cstdlib>

// *****************************
// * Game Specific Definitions *
// *****************************

// ****** Setup Memory Checking Stuff ******

// Force _strdup() to use debug malloc too..
#define _strdup(s) strcpy((char*)malloc(strlen(s) + 1), s)

// Display Parameters

// Run path
#define RUNPATHLEN 100
extern char RunPath[MAXPATHLEN];
extern char SavePath[MAXPATHLEN];

extern char ClassDefPath[MAXPATHLEN]; // Where to load / save Class.Def
extern char ExileRCPath[MAXPATHLEN];  // Where to run ExileRC from & where
                                      // the graphics for the resources are
extern char ResourcePath[MAXPATHLEN]; // Where to read / write the resources
extern char BaseMapPath[MAXPATHLEN];  // Where the untouched version of the game map is stored
extern char CurMapPath[MAXPATHLEN];   // Where the current map is stored

// Multi-Monitor Variables (Note: Additional Monitor globals are defined in MONITOR.H)
extern int32_t MonitorNum;                 // Monitor game will run on (default is 1, primary)
extern int32_t MonitorX, MonitorY;         // Relative position of monitor in desktop coordinates
extern int32_t MonitorW, MonitorH;         // Width and height of monitor (before changing video modes)
extern char DXDriverMatchStr[FILENAMELEN]; // Will use first DX driver who's description contains the given string (not case sensitive)
                                           // i.e. use if string is "permidia" and driver desc is "Glint Permidia 2 3D"

// Game speed variable
extern int32_t GameSpeed;           // Value 1-5 which determines how fast the game is running
                                    // Used to switch on/off processor intensive effects
extern int32_t ViolenceLevel;       // Value 0-5 which determines how much blood and gore, etc.
                                    // 0-none

// Animation toggles..
extern bool WaitVertRetrace;    // Does vertical retrace checking (not used now)
extern bool ShowDrawing;        // Swaps the back for the front buffer so we can wach redraw
extern bool ShowBufNames;       // Shows labels for back/front buffer (not used now)
extern bool NoNormals;          // Turns normal lights on/off
extern bool NoFrameSkip;        // Turns frame skipping on/off
extern bool PauseFrameSkip;     // Set during disk loading to avoid skipping afterwards
extern bool ClearBeforeDraw;    // Clear before drawing update buffers?
extern bool NoScrollZBuffer;    // Causes the system to not scroll the display z buffer
extern bool SmoothScroll;       // Do smooth scrolling
extern bool ScrollLock;         // Lock scrolling to the character
extern bool DrawRealtimeShadows; // Draw alpha shadows for 3D characters
extern bool Show3D;             // Shows 3D objects and chars
extern bool Interpolate;        // Causes 3D animations to interpolate when state changes
extern bool DoPageFlip;         // Causes pages to flip (if off, screen will stop updating)
extern bool PauseWhenNotActive; // Causes program and threads to pause when not active
extern bool Windowed;           // True if game is to be run in a window
extern bool SingleBuffer;       // True if game using single buffering (true if Windowed, or UsingHardware false)
extern bool Borderless;         // When windowed, cut out the border (menu, other windows garabage)?
extern bool NoWideBuffers;      // Doesn't allow video buffers with stides wider than their widths
extern bool UseDirect3D2;       // True if want to be able to use drawprimive stuff
extern bool UseSoftware3D;      // Use software 3D (False assumes we want hardware if available)
extern bool SoftRampMode;       // Use RAMP mode for software 3D (hardware defaults to what's available)
extern bool SoftRGBMode;        // Use RGB mode for software 3D (hardware defaults to what's available)
extern bool UseBlue;            // Causes system to use BLUE software renderer
extern bool UseDrawPrimitive;   // If false, 3D system does everything with Execute Buffers (always false if UseDirect3D2 is false)
extern bool UsingHardware;      // Set by 3D system to true if 3D hardware accel or false if software only
extern bool SoftRGBMode;        // Use RGB mode for software 3D (hardware defaults to what's available)
extern bool CacheExBufs;        // Attempt to cache execute buffers for chars and other animation
extern bool PreloadSectors;     // If true, will load a large sector cache for faster scrolling
extern bool NoPulseObjs;        // Prevents the game objects from being pulsed
extern bool NoAnimateObjs;      // Prevents the game objects from being animated
extern bool NoVidZBufLock;      // Prevents simultaneous locking of video and zbuffer (for voodoo)
extern bool NoBlitZBuffer;      // Card is incapable of blitting to the display ZBuffer (voodoo)
extern bool UseClearZBuffer;    // Use a clear z buffer for z restores (i.e. Viewport->Clear())
extern bool UnlockImmediately;  // Causes video buffers to be unlocked immediately after they're locked (faster)
extern bool IsVooDoo;           // True if using a voodoo card
extern bool IsMMX;              // Has MMX extensions
extern bool CaptureVideo;       // True when system is capturing video
extern bool NoAI;               // Turns off monster AI
extern bool AutoBeginCombat;    // Automatically begins combat if enemy in range and facing him
extern bool PlaySpeech;         // Play speech wave files
extern bool ShowDialog;         // Show dialog text if speech wave is played

// Watch zbuffer and normal draws
extern bool ShowZBuffer;
extern bool ShowNormalBuffer;

// Program run states
extern bool AppActive;          // Flag for if the game is the active application
extern bool Editor;             // This is true if we are in edit mode
extern bool StartInEditor;      // Whether to start the program in editor mode
extern bool NoQuickLoad;        // Prevent the program from attempting to load the IMAGERY.DAT file

// Render state controls
extern bool FlatShade;
extern bool SimpleLight;
extern bool DitherEnable;
extern bool BlendEnable;
extern bool SpecularEnable;
extern bool UseTextures;
extern bool ZEnable;
extern bool BilinearFilter;
extern bool NoUpdateRects;
extern bool Double3D, Triple3D;
extern bool UseDirLight;
extern int32_t DirLightPercent;

// 3D Lighting adjustment percent values
extern int32_t MaxLights;           // Maximum number of lights that can affect a 3d object
extern int32_t Ambient3D;           // Adjust this below 100 for darker ambient, or above for lighter
extern int32_t LightRange3D;        // Adjust this below 100 to decrease 3D light range, or above to increase

// Whether map pane is full screen or not
extern bool FullScreen;

// Music and sound effects
extern bool SoundSystemOn;

// Is control, shift, or alt down
extern bool CtrlDown, ShiftDown, AltDown;

// Character movement mode
extern bool GridSnap;

// Total size of preload area (in sectors) if PreloadSectors is on
extern int32_t PreloadSectorSize;

// Total size of chunk cache to allocate (in MB's)
extern int32_t ChunkCacheSize;

// Frame ticks for doubletap checking
extern int32_t DoubleTapTicks;

// Global Map Pane Sizes
extern int32_t MAPPANEX, MAPPANEY, MAPPANEWIDTH, MAPPANEHEIGHT;
extern int32_t SCROLLBUFWIDTH, SCROLLBUFHEIGHT;

// GetString variables
extern char *TempStr;
extern int32_t MaxStringLength;
extern int32_t CurGetStringChar;
extern bool GetStringDone;

// Ambient light variables
extern int32_t    ambient;
extern double lred;
extern double lgreen;
extern double lblue;

// Language
extern char Language[NAMELEN];              // Where the current map is stored

// Simple Support Functions
inline char *strncpyz(char* dst, const char* src, int32_t n)
  { strncpy(dst, src, n-1); dst[n-1] = 0; return dst; }
  // Does a strncpy and insures that last character is always null
inline char *strncatz(char* dst, const char* src, int32_t n)
  { int32_t l = strlen(dst); return strncpyz(dst + l, src, n - l); }
  // Concatenates a src with dst, but does not go beyound n-1 chars for dest, and insures
  // a null is appended at end of dst
char *itos(int32_t val, char* buf, int32_t buflen);
  // Efficiently converts an int32_t to a string given a buffer of 'buflen' size
int32_t stricmp(const char* s1, const char* s2);
  // Case insensitive string compare
int32_t copyfiles(const char *from, const char *to, bool overwrite = true);
  // Copy file command (uses wildcards!!)
int32_t deletefiles(const char *name);
  // Delete file command (uses wildcards!!)
int32_t flen(FILE* f);
  // Returns the length of a file
uint32_t tickcount();
  // Returns system ticks (in milliseconds) since system was turned on

// Prints error, or fatal error message, and exits
void Error(const char *error, const char *extra = nullptr);
void FatalError(const char *error, const char *extra = nullptr);
void ThreadError(const char *error, const char *extra = nullptr);

// Event and Mutex error stuff
void WaitSingleErr(HANDLE obj);
void WaitMultipleErr(uint32_t objs, CONST HANDLE *obj, bool all);

// Critical Section Functions
void BEGIN_CRITICAL();
void END_CRITICAL();

// Load a resource functions
void *LoadRCResource(char *name, int32_t id); // Shouldn't be used anymore
void FreeRC(void *p);                         // Same here

// Makes a file path given the current settings of RunPath and SavePath
char *makepath(char *name, char *buf, int32_t buflen);
// Open a FILE in the program path using the makepath() function
FILE *popen(char *file, char *flags);

// Random number generation
int32_t random(int32_t min, int32_t max);

// Comma delimited list functions (useful for strings in "abcd,defg,hijk" format)
// If dst is nullptr, retuns result pointer from static internal buffer
char *listrnd(char *src, char *dst = nullptr, int32_t len = 0);
    // Get random string from comma list
char *listget(char *src, int32_t num, char *dst = nullptr, int32_t len = 0);
    // Get num string from comma list
int32_t listnum(char *src);
    // Get total number of strings in comma list
bool listin(char *src, char *in);
    // Returns true if string is in comma list (case insensitive)

// Causes all game threads to pause
void PauseThreads();
void ResumeThreads();

// Exit the game (Now why would somebody want to do that?)
void ExitGame();

// Prints out game status info to the display as game loads
void Status(const char *fmt, ...);

// Memory free functions
uint32_t MemUsed();    // Percentage of memory used
uint32_t FreeMem();    // Free system memory (virtual)
uint32_t TotalMem();   // Total system memory (virtual)
uint32_t FreePhys();   // Free physical memory
uint32_t TotalPhys();  // Total physical memory
uint32_t FreePage();   // Free paging file memory
uint32_t TotalPage();  // Total paging file memory

// INI File Functions
void INISetSection(char *newsection);
int32_t INIGetInt(char *key, int32_t def = 0, char *format = nullptr);
void INISetInt(char *key, int32_t i, char *format = nullptr);
char *INIGetText(char *key, char *def, char *buf, int32_t buflen);
void INISetText(char *key, char *str);
char *INIGetStr(char *key, char *def = nullptr, char *buf = nullptr, int32_t buflen = 0);
void INISetStr(char *key, char *str);
int32_t INIGetArray(char *key, int32_t size, int32_t *ary, int32_t defsize = 0, int32_t *defary = nullptr, char *format = nullptr);
void INISetArray(char *key, int32_t size, int32_t ary[], char *format = nullptr);
bool INIGetBool(char *key, bool def = false, char *yes = nullptr, char *no = nullptr);
void INISetBool(char *key, bool on, char *yes = nullptr, char *no = nullptr);
bool INIGetYesNo(char *key, bool def = false);
void INISetYesNo(char *key, bool on);
bool INIGetTrueFalse(char *key, bool def = false);
void INISetTrueFalse(char *key, bool on);
bool INIGetOnOff(char *key, bool def = false);
void INISetOnOff(char *key, bool on);
bool INIParse(char *key, char *def, char *format, ...);
void INIPrint(char *key, char *format, ...);

// Debug bool variable
extern bool Debug;
extern bool Ignore3D;   // Whether to disallow 3D imagery (to make it run on systmes without Direct3D)

// Forces video mode to assume 15 or 16 bit
extern bool Force15Bit;
extern bool Force16Bit;

// Tick Sync variable
extern bool TickOccured;

// Disable timer
extern bool DisableTimer;

// Is System Closing Down?
extern bool Closing;

// For setting up 3D object bounding rects in the editor
extern bool UpdatingBoundingRect;

// Last Frame Milliseconds
extern int32_t LastFrameTicks;
extern bool ShowFramesPerSecond;

// Global Objects
extern PTScreen     CurrentScreen;      // Currently displayed screen object 
extern PTScreen     NextScreen;         // Next Screen to be display object
extern TDisplay     display;            // Display object
extern T3DScene     Scene3D;            // 3d Object
extern PTDisplay    Display;            // Display object
extern TPlayScreen  PlayScreen;         // PlayScreen Object
extern TLogoScreen  LogoScreen;         // LogoScreen Object
extern TMapPane     MapPane;            // Main map pane for PlayScreen
extern THealthBar   HealthBar;          // Character's health
extern TStaminaBar  StaminaBar;         // Character's fatigue
extern TInventory   Inventory;          // Inventory of objects
extern TTextBar     TextBar;            // Info line for player
extern TMultiCtrlPane   MultiCtrl;      // Multipane control panel (buttons)
extern TEquipPane   EquipPane;          // Equipment pane
extern TAutoMap     AutoMap;            // Automapper
extern TSpellPane   SpellPane;          // Talisman stuff
extern TQuickSpellPane  QuickSpells;    // Quick spell buttons
extern TStatPane    StatPane;           // Display of char's abilities
extern TDialogPane  DialogPane;         // Dialog panel
extern TMainWindow  MainWindow;         // Windows Object
extern TScriptManager   ScriptManager;  // Manages all scripts in the game
extern TSaveGame    SaveGame;           // SaveGame Object
extern TChunkCache  ChunkCache;         // Tile Cache
extern TTimer       Timer;              // Timer Object
extern TVideoCapture VideoCapture;      // Video capture object
extern PTFont       SystemFont;         // Basic utility font
extern PTFont       DialogFont;         // Dialog font
extern PTFont       DialogFontShadow;   // Dialog font shadow
extern PTFont       SmallFont;          // Small game font
extern PTFont       GameFont;           // Medium game font
extern PTFont       GoldFont;           // Medium gold font
extern PTFont       MetalFont;          // Small gold/metal font
extern PTFont       MenuFont;           // Menu font
extern PTPlayer     Player;             // The active player for current game
extern PTMulti      GameData;           // Global game data pointer
extern TSoundPlayer SoundPlayer;        // Sound effects player
extern TControlMap  ControlMap;         // Contains the key/joystick mappings for game control
extern TAreaManager AreaManager;        // Manages the game area system
extern TPlayerManager PlayerManager;    // Stores the current player list for the game
extern TRules       Rules;              // Stores rules, like classes, char types, attacks, tables, etc.
extern TSpellList   SpellList;          // a list of spells in the game
extern TDialogList  DialogList;         // List of dialog and other game messages for current language
