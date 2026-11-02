// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 2024 Cinematix                       *
// *                   revutils.h - Utility functions                      *
// *************************************************************************

#pragma once

#include "game/app/revenant.h"

#include <cstdint>
#include <string>
#include <vector>

// Simple Support Functions
char *itos(int32_t val, char* buf, int32_t buflen);
  // Efficiently converts an int32_t to a string given a buffer of 'buflen' size
int32_t copyfiles(const char *from, const char *to, bool overwrite = true);
  // Copy file command (uses wildcards!!)
int32_t deletefiles(const char *name);
  // Delete file command (uses wildcards!!)
int32_t flen(FILE* f);
  // Returns the length of a file
  // Returns system ticks (in milliseconds) since system was turned on

// Deterministic runs (--control, --record, --replay; docs/playtest/PLAN.md
// §6.3): the game reads no wall clock and asks no other thread how it is
// doing, so a run repeats exactly from its seed and its input.
void SetDeterministicRun(bool on);
[[nodiscard]] bool DeterministicRun();
[[nodiscard]] uint32_t GameTickCount();
  // The milliseconds game logic reads where retail read GetTickCount: the
  // wall clock (tickcount), or in a deterministic run the simulation clock
  // (as the retail emulator's virtual GetTickCount advances with its ticks)

// Prints error, or fatal error message, and exits
void ThreadError(const char *error, const char *extra = nullptr);

// Event and Mutex error stuff
void WaitSingleErr(HANDLE obj);
void WaitMultipleErr(uint32_t objs, const HANDLE *obj, bool all);

// Critical Section Functions

// Posix replacement for the Win32 GetProgramPaths probe. Sets RunPath
// (user's existing Revenant install — read-only assets) and SavePath
// (per-user writable data dir — saves, INI) with trailing '/'. Both
// buffers are at least `buflen` bytes. FatalErrors if SavePath can't
// be created or written to (read-only-media refusal).
void rev_resolve_program_paths(char *RunPath, char *SavePath, int32_t buflen);

// Locate the Revenant Revisited overlay (our enhancement layer — strictly
// opt-in: gated on the --revisited CLI flag. Without the flag this returns
// "" unconditionally and the engine runs vanilla. With the flag, resolution
// order is:
//   1. $REVENANT_REVISITED_PATH (explicit override)
//   2. <exe-dir>/RevenantRevisited.rvr   (production: shipped pack)
//   3. <RunPath>/RevenantRevisited.rvr   (production: in user's install)
//   4. <repo-root>/revisited/resources/  (dev: loose folder beside src/)
// If --revisited is set but no overlay is reachable, this FatalErrors —
// silently dropping back to vanilla would mask the misconfiguration.
// See revisited/README.md for the convention.
const char *rev_resolve_revisited_overlay();

// Makes a file path given the current settings of RunPath and SavePath
char *makepath(char *name, char *buf, int32_t buflen);

// Which copy of a file wins when both a pack archive and a loose file
// answer the same path. Retail's pack-aware open (FUN_004a1240, reached
// through FUN_004a13f0) takes this as its third argument: nearly every
// loader passes PackFirst; the .def screen loader (FUN_004377c0) passes
// LooseFirst, which is how loose patch files beside the packs take effect.
// rev_fopen and EOpenOrder: engine/core/file.h (the game implements the
// pack-aware open below until the VFS moves into the engine).

// Reads a whole file through rev_fopen. Returns false if it can't be opened
// or read.
bool rev_read_file(const char *name, std::vector<uint8_t> &out,
                   EOpenOrder order = EOpenOrder::PackFirst);

// True when `name` resolves to a mounted-pack entry or a loose file under
// SavePath, the Revisited overlay or RunPath. Unlike rev_fopen it does not
// use the legacy by-file-name fallback, so it answers for exactly the path
// given — retail's FUN_004a1c00, which the loaders use to pick between a
// module's copy of a file and the shared one.
[[nodiscard]] bool rev_file_exists(const char *name);

// "<preferred_dir><file>" when rev_file_exists says so, otherwise
// "<fallback_dir><file>". This is the either/or lookup retail inlines in its
// loaders: ImageryPath before ClassDefPath for class.def and the rules
// rosters, the active module before ClassDefPath for module data.
[[nodiscard]] std::string rev_first_existing(const char *preferred_dir,
                                             const char *fallback_dir,
                                             const char *file);

// Resource archive VFS — retail shipped data/resources.rvr, data/imagery.rvi
// and data/Modules/<name>.rvm as stored (uncompressed) ZIPs. WinMain mounted
// the two base archives individually, and a per-module archive was swapped
// in when the active module changed. We mirror that lifecycle: call these
// explicitly; nothing is auto-scanned.
//
// A mounted archive stands in for the directory its path names without the
// extension: <RunPath>/resources.rvr answers <RunPath>/Resources/..., and
// Modules/Ahkuilon.rvm answers Modules/Ahkuilon/... (case-insensitive), as
// retail's TPackFile does (FUN_0049ee20 / FUN_004a0380).
bool MountArchive(const char *name);    // name looked up under data root, e.g. "resources.rvr"
bool MountModule(const char *name);     // mounts data/Modules/<name>.rvm (unmounts any prior)
void UnmountModule();
void UnmountAll();

// rev_engine_asset: engine/core/file.h.


// The files directly in directory `dir` whose extension is `ext` (".wav";
// case-insensitive), as file names, appended to `names`. Retail's pack-aware
// findfirst/findnext (FUN_004a19d0 / FUN_004a1b20) over "<dir>*<ext>": the
// mounted pack whose directory holds `dir` answers if it has any match, and
// only otherwise is the loose directory listed. A relative `dir` is looked
// up under SavePath, the Revisited overlay and RunPath, as rev_fopen does; a
// name found under an earlier root hides the same name under a later one,
// so each name listed opens with rev_fopen(dir + name). When no root
// answers, a relative `dir` is looked up as a path inside the base packs
// (the legacy fallback for older port INIs, as rev_fopen's by-name lookup).
// Returns the number of names appended.
size_t rev_find_files(const char *dir, const char *ext, std::vector<std::string> &names);

// Random number generation. The shipped game drew every random number from
// the MSVC C runtime's rand() (a linear congruential generator, 0..32767);
// random() keeps that generator, so a seed gives retail's sequence and a
// seeded run (--seed) repeats exactly.
int32_t random(int32_t min, int32_t max);
    // min..max inclusive (either order); min == max returns min without a draw
int32_t GameRand();
    // The next value of the generator, 0..32767 (MSVC rand())
void SeedRandom(uint32_t seed);
    // Seeds the generator (and the C library's rand(), which some effects
    // still call directly)
uint64_t RandomDraws();
    // Values drawn since start: a run's draw count is part of its trace
using RandomObserver = void (*)(int32_t value, const void* caller);
void SetRandomObserver(RandomObserver observer);
using RandomSource = int32_t (*)();
void SetRandomSource(RandomSource source);
    // Retail A/B fixtures (the dojo's RNG tape): when set, every value
    // comes from `source` (0..32767) instead of the generator
using RandomRangeObserver = void (*)(int32_t lo, int32_t hi, int32_t result);
void SetRandomRangeObserver(RandomRangeObserver observer);
    // Retail A/B fixtures: told every random(lo, hi) and its result
    // Determinism debugging (--combattrace-rng): told every draw and the
    // code address that asked for it (random()'s caller, or GameRand's)

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
