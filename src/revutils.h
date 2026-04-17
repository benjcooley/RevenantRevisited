// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 2024 Cinematix                       *
// *                   revutils.h - Utility functions                      *
// *************************************************************************

#pragma once

#include "revenant.h"

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
void WaitMultipleErr(uint32_t objs, const HANDLE *obj, bool all);

// Critical Section Functions
void BEGIN_CRITICAL();
void END_CRITICAL();

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
