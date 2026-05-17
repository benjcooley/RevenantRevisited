// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 2024 Cinematix                       *
// *                   revutils.h - Utility functions                      *
// *************************************************************************

#pragma once

#include "revenant.h"

#include <string>
#include <vector>

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
int32_t strnicmp(const char* s1, const char* s2, size_t n);
  // Case insensitive bounded string compare
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
void WaitMultipleErr(uint32_t objs, const HANDLE *obj, bool all);

// Critical Section Functions
void BEGIN_CRITICAL();
void END_CRITICAL();

// Posix replacement for the Win32 GetProgramPaths probe. Sets RunPath
// (user's existing Revenant install — read-only assets) and SavePath
// (per-user writable data dir — saves, INI) with trailing '/'. Both
// buffers are at least `buflen` bytes. FatalErrors if SavePath can't
// be created or written to (read-only-media refusal).
void rev_resolve_program_paths(char *RunPath, char *SavePath, int32_t buflen);

// Locate the Revenant Revisited overlay (our enhancement layer — strictly
// opt-in: empty/missing means vanilla retail). Resolution order:
//   1. $REVENANT_REVISITED_PATH (explicit override)
//   2. <exe-dir>/RevenantRevisited.rvr   (production: shipped pack)
//   3. <RunPath>/RevenantRevisited.rvr   (production: in user's install)
//   4. <repo-root>/revisited/resources/  (dev: loose folder beside src/)
// Returns "" if no overlay found (engine then runs vanilla).
// See revisited/README.md for the convention.
const char *rev_resolve_revisited_overlay();

// Makes a file path given the current settings of RunPath and SavePath
char *makepath(char *name, char *buf, int32_t buflen);
// Open a FILE relative to SavePath / RunPath. Named to avoid the libc
// popen(3) shell-pipe function, which would otherwise win overload
// resolution on POSIX.
FILE *rev_fopen(const char *file, const char *flags);

// Resource archive VFS — retail shipped data/resources.rvr, data/imagery.rvi
// and data/Modules/<name>.rvm as stored (uncompressed) ZIPs. WinMain mounted
// the two base archives individually, and a per-module archive was swapped
// in when the active module changed. We mirror that lifecycle: call these
// explicitly; nothing is auto-scanned.
//
// Resolution order in rev_fopen: SavePath → RunPath → active module → base.
bool MountArchive(const char *name);    // name looked up under data root, e.g. "resources.rvr"
bool MountModule(const char *name);     // mounts data/Modules/<name>.rvm (unmounts any prior)
void UnmountModule();
void UnmountAll();

// Enumerate file entries across all mounted archives whose in-archive
// path starts with `prefix` (case-insensitive). Returns lowercase
// basenames (with extension); appended to `out`. Used by subsystems
// that need to discover assets without per-file probing, e.g. the
// sound registry walking Sound/effects/*.wav out of resources.rvr.
// Returns the number of new entries appended.
size_t VFSListByPrefix(const char *prefix, std::vector<std::string> &out);

// Random number generation
int32_t random(int32_t min, int32_t max);

// Comma delimited list functions (useful for strings in "abcd,defg,hijk" format)
// If dst is nullptr, retuns result pointer from static internal buffer
char *listrnd(const char *src, char *dst = nullptr, int32_t len = 0);
    // Get random string from comma list
char *listget(const char *src, int32_t num, char *dst = nullptr, int32_t len = 0);
    // Get num string from comma list
int32_t listnum(const char *src);
    // Get total number of strings in comma list
bool listin(const char *src, const char *in);
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
void INISetPath(const char *runpath);
void INISetSection(const char *newsection);
int32_t INIGetInt(const char *key, int32_t def = 0, char *format = nullptr);
void INISetInt(const char *key, int32_t i, char *format = nullptr);
char *INIGetText(const char *key, char *def, char *buf, int32_t buflen);
void INISetText(const char *key, char *str);
char *INIGetStr(const char *key, char *def = nullptr, char *buf = nullptr, int32_t buflen = 0);
void INISetStr(const char *key, char *str);
int32_t INIGetArray(const char *key, int32_t size, int32_t *ary, int32_t defsize = 0, int32_t *defary = nullptr, char *format = nullptr);
void INISetArray(const char *key, int32_t size, int32_t ary[], const char *format = nullptr);
bool INIGetBool(const char *key, bool def = false, const char *yes = nullptr, const char *no = nullptr);
void INISetBool(const char *key, bool on, const char *yes = nullptr, const char *no = nullptr);
bool INIGetYesNo(const char *key, bool def = false);
void INISetYesNo(const char *key, bool on);
bool INIGetTrueFalse(const char *key, bool def = false);
void INISetTrueFalse(const char *key, bool on);
bool INIGetOnOff(const char *key, bool def = false);
void INISetOnOff(const char *key, bool on);
bool INIParse(const char *key, const char *def, char *format, ...);
void INIPrint(const char *key, const char *format, ...);
