// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *               editorprefs.h - Editor preference key-value store        *
// *************************************************************************
//
// One stop shop for "remember this between editor sessions" -- splitter
// sizes, panel-collapsed flags, last-used asset path, console history,
// recent map jumps, anything. Backed by a single defdoc Document that
// reads / writes <save_dir>/editor.def.
//
// Use:
//
//   const float w = EditorPrefs::GetFloat("scene.width", 280.0f);
//   EditorPrefs::SetFloat("scene.width", new_w);
//
// Path syntax is dotted: "scene.width", "console.input.history.0". Each
// segment is a key in a nested defdoc Object. Missing segments are
// auto-created on Set, return the fallback on Get.
//
// Typical lifecycle: EditorPrefs::Load() once at boot, EditorPrefs::Save()
// on F12 exit / app shutdown. Reading + writing in between is pure
// in-memory.
//
// Delete <save_dir>/editor.def -> editor next launch comes up in a clean
// "fresh" state with all fallbacks. The file format is the same .def the
// rest of the project uses, so it's easy to inspect / edit by hand.
//
// *************************************************************************

#pragma once

#include "defdoc.h"

#include <string>

namespace EditorPrefs {

// Load editor.def from disk. Best-effort -- missing or malformed file is
// fine, all callers fall back to their defaults. Returns true on
// successful read.
bool Load(const char* path = nullptr);

// Write the current in-memory store back to disk. Returns true on
// successful write.
bool Save(const char* path = nullptr);

// Reset all in-memory prefs (does not touch the file). After this every
// Get... returns its fallback.
void Reset();

// Typed leaf access. Path is dotted, e.g. "scene.width".
int32_t     GetInt    (const char* path, int32_t fallback = 0);
float       GetFloat  (const char* path, float fallback = 0.0f);
double      GetDouble (const char* path, double fallback = 0.0);
bool        GetBool   (const char* path, bool fallback = false);
std::string GetString (const char* path, const std::string& fallback = {});

void        SetInt    (const char* path, int32_t value);
void        SetFloat  (const char* path, float value);
void        SetDouble (const char* path, double value);
void        SetBool   (const char* path, bool value);
void        SetString (const char* path, const std::string& value);

// True if the path resolves to a leaf (or object) in the store. Useful
// to distinguish "user has explicitly set this" from "fallback applied".
bool        Has       (const char* path);

// Direct access to the sub-object at `path` (created if absent). For
// callers that want to write a few related keys without re-walking the
// path each time, or to introspect blocks of state.
defdoc::Node& Section(const char* path);

// Direct access to the root Document. Mostly for tests / power users;
// regular code should use the typed accessors.
defdoc::Document& Doc();

} // namespace EditorPrefs
