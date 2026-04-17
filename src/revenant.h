// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   Revenant.h - Main Include File                      *
// *************************************************************************

#pragma once

#include "platform_compat.h"
#include "revdefs.h"
#include "revtypes.h"
#include "revutils.h"

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
extern char StartupSavePath[];  // --loadmap=<path>; empty if no startup auto-load

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
extern int32_t ambient;
extern double lred;
extern double lgreen;
extern double lblue;

// Language
extern TString Language; // Where the current map is stored

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
extern TScreen*     CurrentScreen;      // Currently displayed screen object 
extern TScreen*     NextScreen;         // Next Screen to be display object
extern TDisplay     display;            // Display object
extern T3DScene     Scene3D;            // 3d Object
extern TDisplay*    Display;            // Display object
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
extern TFont*       SystemFont;         // Basic utility font
extern TFont*       DialogFont;         // Dialog font
extern TFont*       DialogFontShadow;   // Dialog font shadow
extern TFont*       SmallFont;          // Small game font
extern TFont*       GameFont;           // Medium game font
extern TFont*       GoldFont;           // Medium gold font
extern TFont*       MetalFont;          // Small gold/metal font
extern TFont*       MenuFont;           // Menu font
extern TPlayer*     Player;             // The active player for current game
extern TMulti*      GameData;           // Global game data pointer
extern TFontTable*  FontTable;          // FONT.DEF driven named font registry
extern TSoundPlayer SoundPlayer;        // Sound effects player
extern TControlMap  ControlMap;         // Contains the key/joystick mappings for game control
extern TAreaManager AreaManager;        // Manages the game area system
extern TPlayerManager PlayerManager;    // Stores the current player list for the game
extern TRules       Rules;              // Stores rules, like classes, char types, attacks, tables, etc.
extern TSpellList   SpellList;          // a list of spells in the game
extern TDialogList  DialogList;         // List of dialog and other game messages for current language
